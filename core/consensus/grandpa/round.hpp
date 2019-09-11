/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP

#include <boost/optional.hpp>
#include "consensus/grandpa/message.hpp"
#include "consensus/grandpa/vote_tracker.hpp"
#include "consensus/grandpa/vote_graph.hpp"
#include "consensus/grandpa/voters_set.hpp"

namespace kagome::consensus::grandpa {

  struct Round {
    std::shared_ptr<VotersSet> voters;
    MembershipCounter counter;
    RoundNumber round;

    std::unique_ptr<VoteTracker> votes;
    std::unique_ptr<VoteGraph> graph;
  };



}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
