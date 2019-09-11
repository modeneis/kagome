/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP

#include "consensus/grandpa/grandpa_voter.hpp"

#include "clock/clock.hpp"

namespace kagome::consensus::grandpa {
  class GrandpaVoterImpl : public GrandpaVoter {
   public:
    ~GrandpaVoterImpl() override = default;

    GrandpaVoterImpl(std::shared_ptr<clock::SteadyClock> clock)
        : clock_(std::move(clock)) {
      BOOST_ASSERT(clock_ != nullptr);
    }

    void onVotersSetChange(std::shared_ptr<VotersSet> new_voters) override {
      round_ = 0;
      ++counter_;
    }

    void playGrandpaRound(RoundNumber round) override {
      auto now = clock_->now();
    }


   private:
    RoundNumber round_{0};
    MembershipCounter counter_{0};
    std::shared_ptr<clock::SteadyClock> clock_;
  };
}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_VOTER_IMPL_HPP
