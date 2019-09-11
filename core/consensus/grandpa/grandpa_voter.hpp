/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP

#include "round_observer.hpp"
#include "message.hpp"
#include "voters_set.hpp"
#include "round.hpp"

namespace kagome::consensus::grandpa {

  struct GrandpaVoter : public RoundObserver {
    ~GrandpaVoter() override = default;

    // executes algorithm 4.6 Join-Leave-Grandpa-Voters
    virtual void onVotersSetChange(std::shared_ptr<VotersSet> new_voters) = 0;

    // executes algorithm 4.7 Play-Grandpa-Round(r)
    virtual void playGrandpaRound(RoundNumber round) = 0;

    // returns true if this peer is primary
    virtual bool isPrimary() const = 0;

    // executes algorithm 4.8 Best-Final-Candidate(r)
    virtual boost::optional<BlockInfo> bestFinalCandidate(RoundNumber round) = 0;

    // executes algorithm 4.9 Attempt-To-Finalize-Round(r)
    virtual void attemptToFinalizeRound(RoundNumber r) = 0;

  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP
