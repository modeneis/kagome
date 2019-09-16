/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_VOTING_ROUND_IMPL_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_VOTING_ROUND_IMPL_HPP

#include "consensus/grandpa/voting_round.hpp"

#include <boost/asio/basic_waitable_timer.hpp>
#include <boost/range/algorithm/find_if.hpp>
#include "blockchain/block_tree.hpp"
#include "common/logger.hpp"
#include "common/visitor.hpp"
#include "consensus/grandpa/gossiper.hpp"
#include "consensus/grandpa/vote_graph.hpp"
#include "consensus/grandpa/vote_tracker.hpp"

namespace kagome::consensus::grandpa {

  class VotingRoundImpl : public VotingRound {
    using Timer = boost::asio::basic_waitable_timer<std::chrono::system_clock>;

   public:
    ~VotingRoundImpl() override = default;

    VotingRoundImpl(std::shared_ptr<VoterSet> voters,
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
                    common::Logger logger = common::createLogger("Grandpa"))
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

    static VoteGraph::CumulativeVote getThreshold(
        const std::shared_ptr<VoterSet> &voters) {
      return (voters->size() / 2) * 3;
    }

    void onFin(const Fin &f) override {
      switch (state_) {
        case State::START:
        case State::PREVOTED:
          break;
        case State::PRECOMMITTED: {
          tryFinalize();
        }
      }
    }

    void tryFinalize() {
      auto l = block_tree_->getLastFinalized();
      auto e = bestFinalCandidate(round_number_);

      if (e.block_number > l.block_number) {
        if (graph_->findGhost(
                e, [this](const VoteGraph::CumulativeVote &cumulative_vote) {
                  return cumulative_vote > threshold_;
                })) {
          auto justification = tracker_->getJustification(e);

          auto finalized =
              block_tree_->finalize(e.block_hash, std::move(justification));
          if (not finalized) {
            logger_->warn(
                "Could not finalize message during round {}. Error: {}",
                round_number_,
                finalized.error().error_message());
          }
        }
      }
    }

    void onVoteMessage(const VoteMessage &vote_message) override {
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
          auto opt_precommit =
              getFromVariant<SignedPrecommit>(vote_message.vote);
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

    /**
     * @tparam VoteType – either SignedPrevote or SignedPrecommit
     * @param vote – vote is pushed to tracker if it is not equivocated vote and
     * it was not pushed into the tracker before
     */
    template <typename VoteType>
    void onVote(const VoteType &vote) {
      switch (tracker_->push(vote)) {
        case VoteTracker::PushResult::SUCCESS: {
          graph_->insert(vote.vote);
          // self.historical_votes.push_vote(signed_message);
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

    bool completable() {
      auto find_supermajority = [](const std::vector<Prevote> &prevotes) {};

      if (tracker_->getPrecommits().size()
              + tracker_->getEquivocatoryVotes().size()
          > (voters_->size() / 2) * 3) {
        // todo
      }
    }

    void playGrandpaRound() override {
      auto primary = getPrimary();
      if (id_ == primary) {
        gossiper_->fin(prepareFin(round_number_ - 1));
      }

      timer_.expires_at(start_time_ + duration_ * 2);
      timer_.async_wait([this, primary](auto &&ec) {
        if (ec and ec != boost::asio::error::operation_aborted) {
          logger_->error("Error happened during prevote timer: {}",
                         ec.message());
          return;
        }
        findAndGossipPrevote(primary);
      });
    }

    void findAndGossipPrevote(const Id &primary) {
      auto best_final_candidate =
          bestFinalCandidate(round_number_ - 1);  // L in spec

      auto prevotes = tracker_->getPrevotes();

      // check if we have prevote from primary
      auto primary_prevote = boost::find_if(
          prevotes, [&primary](const SignedPrevote &signed_prevote) {
            return signed_prevote.signer == primary;
          });  // B in spec (line 7 of alg 4.7)

      Prevote prevote;  // prevote that will be gossiped (N in spec)

      // if primary has prevoted and we have ghost prevote that
      if (primary_prevote != prevotes.end()) {
        // TODO: line below is wrong
        auto opt_ghost_prevote =
            graph_->findGhost(primary_prevote->vote);  // (B^{r,pv}_v in spec)

        if (opt_ghost_prevote
            and opt_ghost_prevote->block_number
                    >= primary_prevote->vote.block_number
            and primary_prevote->vote.block_number
                    > best_final_candidate.block_number) {
          prevote = primary_prevote->vote;
        }
      } else {
        prevote = graph_->findAncestor(best_final_candidate).value();
      }

      SignedPrevote signed_prevote = signPrevote(prevote);

      VoteMessage message{.vote = signed_prevote,
                          .round_number = round_number_,
                          .counter = counter_,
                          .id = id_};

      gossiper_->vote(message);
      state_ = State::PREVOTED;

      // wait to receive prevotes precommit
      timer_.expires_at(start_time_ + duration_ * 4);
      timer_.async_wait([this, best_final_candidate](auto &&ec) {
        if (ec and ec != boost::asio::error::operation_aborted) {
          logger_->error("Error happened during precommit timer: {}",
                         ec.message());
          return;
        }
        findAndGossipPrecommit(best_final_candidate);
      });
    }

    SignedPrevote signPrevote(const Prevote &prevote) {
      // TODO implement
    }

    SignedPrecommit signPrecommit(const Precommit &precommit) {
      // TODO implement
    }

    void findAndGossipPrecommit(
        const BlockInfo &prev_round_best_final_candidate) {
      auto opt_new_ghost =
          graph_->findGhost(prev_round_best_final_candidate,
                            [this](VoteGraph::CumulativeVote cumulative_vote) {
                              return cumulative_vote > threshold_;
                            });

      if (not opt_new_ghost) {
        // nothing can be broadcasted
        return;
      }

      Precommit precommit{.block_number = opt_new_ghost->block_number,
                          .block_hash = opt_new_ghost->block_hash};

      SignedPrecommit signed_precommit = signPrecommit(precommit);

      VoteMessage message{.vote = signed_precommit,
                          .id = id_,
                          .counter = counter_,
                          .round_number = round_number_};

      gossiper_->vote(message);
      state_ = State::PRECOMMITTED;
    }

    Id getPrimary() const override {
      auto index = round_number_ % voters_->size();
      return voters_->at(index);
    }

    BlockInfo bestFinalCandidate(RoundNumber round) override {
      return {};  // TODO(warchant): implement
    };

   private:
    Fin prepareFin(RoundNumber number) {
      Fin fin;
      fin.round_number = number;
      fin.vote = bestFinalCandidate(fin.round_number);
      fin.justification = tracker_->getJustification(fin.vote);
      return fin;
    }

   private:
    std::shared_ptr<VoterSet> voters_;
    RoundNumber round_number_;
    Duration duration_;  // length of round (T in spec)
    TimePoint start_time_;
    MembershipCounter counter_;
    Id id_;  // id of current peer
    State state_;
    VoteGraph::CumulativeVote threshold_;

    std::unique_ptr<VoteTracker> tracker_;
    std::unique_ptr<VoteGraph> graph_;

    std::shared_ptr<Gossiper> gossiper_;
    std::shared_ptr<Clock> clock_;
    std::shared_ptr<blockchain::BlockTree> block_tree_;

    Timer timer_;

    common::Logger logger_;
  };
}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_VOTING_ROUND_IMPL_HPP
