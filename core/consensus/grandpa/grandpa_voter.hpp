/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP

#include "round_observer.hpp"
#include "message.hpp"
#include "round.hpp"

namespace kagome::consensus::grandpa {

  using VoterSet = std::vector<Id>;

  struct GrandpaVoter : public RoundObserver {
    ~GrandpaVoter() override = default;

    // executes algorithm 4.6 Join-Leave-Grandpa-Voters
    virtual void onVotersSetChange(const VoterSet& new_voters) = 0;

    // executes algorithm 4.7 Play-Grandpa-Round(r)
    virtual void playGrandpaRound(Round round) = 0;

    // returns Id of primary peer
    virtual Id getPrimary(const Round& round) const = 0;

//     executes algorithm 4.8 Best-Final-Candidate(r)
    virtual BlockInfo bestFinalCandidate(RoundNumber round) = 0;

    // executes algorithm 4.9 Attempt-To-Finalize-Round(r)
    virtual void attemptToFinalizeRound(RoundNumber r) = 0;

  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_VOTER_HPP
