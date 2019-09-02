/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_COMMON_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_COMMON_HPP

#include "clock/clock.hpp"
#include "primitives/common.hpp"

namespace kagome::consensus {

  struct Vote {
    primitives::BlockHash block_hash;
    primitives::BlockNumber block_number;

    inline bool operator>(const Vote &rhs) const {
      return block_number > rhs.block_number;
    }

    inline bool operator>=(const Vote &rhs) const {
      return block_number >= rhs.block_number;
    }

    inline bool operator<(const Vote &rhs) const {
      return block_number < rhs.block_number;
    }

    inline bool operator<=(const Vote &rhs) const {
      return block_number <= rhs.block_number;
    }
  };

  using TrackerCounter = uint64_t;
  using RoundNumber = uint64_t;
  using RoundDuration = clock::SystemClock::Duration;
  using TimePoint = clock::SystemClock::TimePoint;

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_COMMON_HPP
