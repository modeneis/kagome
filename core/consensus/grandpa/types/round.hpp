/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP

#include <vector>
#include "consensus/grandpa/types/common.hpp"

namespace kagome::consensus {

  struct Round {
    std::vector<primitives::AuthorityId>
        voters_set;       ///< set of authorities participating in this round
    TrackerCounter id_v;  ///< unique identifier
    RoundNumber number;
    RoundDuration duration;
    TimePoint start_time;
  };

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
