/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "consensus/grandpa/impl/voting_round_impl.hpp"

#include <boost/range/algorithm/find_if.hpp>
#include "common/visitor.hpp"
#include "scale/scale.hpp"

namespace kagome::consensus::grandpa {

  VotingRoundImpl::VotingRoundImpl(
      std::shared_ptr<VoterSet> voters,
      RoundNumber round_number,
      Duration duration,
      TimePoint start_time,
      MembershipCounter counter,
      Id id,
      VoteGraph::CumulativeVote threshold,
      std::unique_ptr<VoteTracker> tracker,
      std::unique_ptr<VoteGraph> graph,
      std::shared_ptr<Gossiper> gossiper,
      std::shared_ptr<Clock> clock,
      std::shared_ptr<blockchain::BlockTree> block_tree,
      Timer timer,
      common::Logger logger)
      : voters_{std::move(voters)},
        round_number_{round_number},
        duration_{duration},
        start_time_{start_time},
        counter_{counter},
        id_{id},
        state_{State::START},
        threshold_{threshold},
        tracker_{std::move(tracker)},
        graph_{std::move(graph)},
        gossiper_{std::move(gossiper)},
        clock_{std::move(clock)},
        block_tree_{std::move(block_tree)},
        timer_{std::move(timer)},
        logger_{std::move(logger)} {
    BOOST_ASSERT(voters_ != nullptr);
    BOOST_ASSERT(tracker_ != nullptr);
    BOOST_ASSERT(graph_ != nullptr);
    BOOST_ASSERT(clock_ != nullptr);
    BOOST_ASSERT(block_tree_ != nullptr);

    threshold_ = getThreshold(voters_);
  }

  VoteGraph::CumulativeVote VotingRoundImpl::getThreshold(
      const std::shared_ptr<VoterSet> &voters) {
    return (voters->size() / 2) * 3;
  }

  void VotingRoundImpl::onFin(const Fin &f) {
    // validate message
    switch (state_) {
      case State::START:
      case State::PREVOTED:
        break;
      case State::PRECOMMITTED: {
        tryFinalize();
      }
    }
  }

  void VotingRoundImpl::tryFinalize() {
    auto l = block_tree_->getLastFinalized();
    auto e = bestFinalCandidate(round_number_);

    if (e.block_number > l.block_number) {
      if (graph_->findGhost(
              e, [this](const VoteGraph::CumulativeVote &cumulative_vote) {
                return cumulative_vote > threshold_;
              })) {
        auto justification = tracker_->getJustification(e);

        primitives::Justification kagome_just{
            .data = common::Buffer{scale::encode(justification).value()}};

        auto finalized = block_tree_->finalize(e.block_hash, kagome_just);
        if (not finalized) {
          logger_->warn("Could not finalize message during round {}. Error: {}",
                        round_number_,
                        finalized.error().message());
        }
      }
    }
  }

  void VotingRoundImpl::onVoteMessage(const VoteMessage &vote_message) {
    // validate message
    switch (state_) {
      case State::START: {
        // as state is start it should be prevote message
        auto opt_prevote = getFromVariant<SignedPrevote>(vote_message.vote);
        if (not opt_prevote) {
          logger_->warn(
              "Received unexpected message after start. Expected: "
              "Prevote. Received: Precommit");
          return;
        }
        const auto &prevote = opt_prevote.value();

        onVote<SignedPrevote>(prevote);

        break;
      }

      case State::PREVOTED: {
        auto opt_precommit = getFromVariant<SignedPrecommit>(vote_message.vote);
        if (not opt_precommit) {
          logger_->warn(
              "Received unexpected message after start. Expected: "
              "Precommit. Received: Prevote");
          return;
        }
        const auto &precommit = opt_precommit.value();

        onVote<SignedPrecommit>(precommit);

        break;
      }

      case State::PRECOMMITTED: {
        tryFinalize();
      }
    }
  }

  template <typename VoteType>
  void VotingRoundImpl::onVote(const VoteType &vote) {
    switch (tracker_->push(vote)) {
      case VoteTracker::PushResult::SUCCESS: {
        if (auto inserted = graph_->insert(vote.vote); not inserted) {
          logger_->warn("Vote {} was not inserted with error: {}",
                        vote.vote.block_hash.toHex(),
                        inserted.error().message());
        }
        break;
      }
      case VoteTracker::PushResult::DUPLICATE: {
        break;
      }
      case VoteTracker::PushResult::EQUIVOCATED: {
        // self.historical_votes.push_vote(signed_message);
        break;
      }
    }

    if (completable() and clock_->now() < timer_.expires_at()) {
      timer_.cancel();
    }
  }

  template void VotingRoundImpl::onVote<SignedPrevote>(const SignedPrevote &);
  template void VotingRoundImpl::onVote<SignedPrecommit>(
      const SignedPrecommit &);

  bool VotingRoundImpl::completable() const {
    auto find_supermajority = [](const std::vector<Prevote> &prevotes) {};

    if (tracker_->getPrecommits().size()
            + tracker_->getEquivocatoryVotes().size()
        > (voters_->size() / 2) * 3) {
      // todo
    }

    return true;
  }

  void VotingRoundImpl::playGrandpaRound() {
    auto primary = getPrimary();
    if (id_ == primary) {
      gossiper_->fin(prepareFin(round_number_ - 1));
    }

    timer_.expires_at(start_time_ + duration_ * 2);
    timer_.async_wait([this, primary](auto &&ec) {
      if (ec == boost::asio::error::operation_aborted) {
        logger_->info("Prevote timer was interrupted in round {}",
                      round_number_);
      } else if (ec) {
        logger_->error("Error happened during prevote timer: {}", ec.message());
        return;
      }
      findPrevote(primary);
    });
  }

  void VotingRoundImpl::findPrevote(const Id &primary) {
    auto prev_round_best_final_candidate =
        bestFinalCandidate(round_number_ - 1);  // L in spec

    auto prevotes = tracker_->getPrevotes();

    // check if we have prevote from primary
    auto primary_prevote =
        getPrevoteBy(primary);  // B in spec (line 7 of alg 4.7)

    Prevote prevote;  // prevote that will be gossiped (N in spec)

    // if primary has prevoted then we broadcast ghost (with not condition) from
    // that
    if (primary_prevote) {
      auto ghost_prevote = graph_->findGhost(primary_prevote->vote)
                               .value();  // (B^{r,pv}_v in spec)

      if (ghost_prevote.block_number >= primary_prevote->vote.block_number
          > prev_round_best_final_candidate.block_number) {
        prevote = primary_prevote->vote;
        return gossipPrevote(signPrevote(prevote));
      }
    }

    // otherwise broadcast best ancestor of bfc for previous round
    auto ancestor =
        graph_->findAncestor(prev_round_best_final_candidate).value();
    prevote.block_hash = ancestor.block_hash;
    prevote.block_number = ancestor.block_number;
    return gossipPrevote(signPrevote(prevote));
  }

  void VotingRoundImpl::gossipPrevote(const SignedPrevote &signed_prevote) {
    VoteMessage message{.vote = signed_prevote,
                        .round_number = round_number_,
                        .counter = counter_,
                        .id = id_};

    gossiper_->vote(message);
    state_ = State::PREVOTED;

    // wait to receive prevotes precommit
    timer_.expires_at(start_time_ + duration_ * 4);
    timer_.async_wait([this](auto &&ec) {
      if (ec and ec != boost::asio::error::operation_aborted) {
        logger_->error("Error happened during precommit timer: {}",
                       ec.message());
        return;
      }
      findAndGossipPrecommit();
    });
  }

  void VotingRoundImpl::findAndGossipPrecommit() {
    const BlockInfo &prev_round_best_final_candidate =
        bestFinalCandidate(round_number_ - 1);

    auto opt_new_ghost =
        graph_->findGhost(prev_round_best_final_candidate,
                          [this](VoteGraph::CumulativeVote cumulative_vote) {
                            return cumulative_vote > threshold_;
                          });

    if (not opt_new_ghost) {
      // nothing can be broadcasted
      return;
    }

    Precommit precommit;
    precommit.block_number = opt_new_ghost->block_number;
    precommit.block_hash = opt_new_ghost->block_hash;

    SignedPrecommit signed_precommit = signPrecommit(precommit);

    VoteMessage message{.vote = signed_precommit,
                        .id = id_,
                        .counter = counter_,
                        .round_number = round_number_};

    gossiper_->vote(message);
    state_ = State::PRECOMMITTED;
  }

  boost::optional<SignedPrevote> VotingRoundImpl::getPrevoteBy(
      const Id &authority) const {
    auto prevotes = tracker_->getPrevotes();

    // check if we have prevote from primary
    auto prevote = boost::find_if(
        prevotes, [&authority](const SignedPrevote &signed_prevote) {
          return signed_prevote.signer == authority;
        });

    if (prevote == prevotes.end()) {
      return boost::none;
    }
    return *prevote;
  }

  VotingRoundImpl::signPrevote(const Prevote &prevote) const {
    // TODO: implement
  }

  SignedPrecommit VotingRoundImpl::signPrecommit(
      const Precommit &precommit) const {
    // TODO implement
  }

  Fin VotingRoundImpl::prepareFin(
      kagome::consensus::grandpa::RoundNumber number) {
    Fin fin;
    fin.round_number = number;
    fin.vote = bestFinalCandidate(fin.round_number);
    fin.justification = tracker_->getJustification(fin.vote);
    return fin;
  }

}  // namespace kagome::consensus::grandpa
