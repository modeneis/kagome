/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTING_ROUND_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTING_ROUND_HPP

#include "consensus/grandpa/common.hpp"
#include "message.hpp"
#include "round_observer.hpp"

namespace kagome::consensus::grandpa {

  struct VotingRound : public RoundObserver {
    ~VotingRound() override = default;

    // executes algorithm 4.7 Play-Grandpa-Round(r)
    virtual void playGrandpaRound() = 0;

    // returns Id of primary peer
    virtual Id getPrimary() const = 0;

    //     executes algorithm 4.8 Best-Final-Candidate(r)
    virtual BlockInfo bestFinalCandidate(RoundNumber round) = 0;

    // executes algorithm 4.9 Attempt-To-Finalize-Round(r)
    virtual void attemptToFinalizeRound(RoundNumber r) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTING_ROUND_HPP
