/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP

#include "consensus/grandpa/grandpa_voter.hpp"

#include "clock/clock.hpp"
#include "blockchain/block_tree.hpp"
#include "consensus/grandpa/gossiper.hpp"

namespace kagome::consensus::grandpa {

  class GrandpaVoterImpl : public GrandpaVoter {
   public:
    ~GrandpaVoterImpl() override = default;

    GrandpaVoterImpl(std::shared_ptr<clock::SteadyClock> clock)
        : clock_(std::move(clock)) {
      BOOST_ASSERT(clock_ != nullptr);
    }

    void onVotersSetChange(const VoterSet& new_voters) override {
      round_ = 0;
      ++counter_;
    }

    void playGrandpaRound(Round round) override {
      auto now = clock_->now();
      auto primary = getPrimary(round);
      if(id_ == primary){
        gossiper_->fin(prepareFin(round));
      }


    }

    Id getPrimary(const Round& round) const override {
      auto index = round.round % round.voters.size();
      return round.voters.at(index);
    }

    virtual BlockInfo bestFinalCandidate(RoundNumber round) override {
      return {}; // TODO(warchant): implement
    };

   private:
    Fin prepareFin(const Round& round) {
      Fin fin;
      fin.round = round.round;
      fin.vote = bestFinalCandidate(fin.round);
      fin.justification = round.votes->getJustification(fin.vote);
      return fin;
    }

   private:
    RoundNumber round_{0};
    MembershipCounter counter_{0};
    Id id_; // id of current peer

    std::shared_ptr<Gossiper> gossiper_;
    std::shared_ptr<clock::SteadyClock> clock_;
    std::shared_ptr<blockchain::BlockTree> block_tree_;
    VoterSet last_voters_;
  };
}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP
