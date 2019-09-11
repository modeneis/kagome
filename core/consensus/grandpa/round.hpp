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

    bool isCompletable() const {
      return false;
    };

   private:

    std::shared_ptr<VotersSet> voters_;
    MembershipCounter counter_;
    RoundNumber round_;

    std::unique_ptr<VoteTracker> votes_;
    std::unique_ptr<VoteGraph> graph_;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
