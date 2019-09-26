/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_COMMON_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_COMMON_HPP

#include "primitives/common.hpp"
#include "common/wrapper.hpp"
#include "clock/clock.hpp"
#include "crypto/ed25519_types.hpp"

namespace kagome::consensus::grandpa {

  using Id = primitives::AuthorityId;
  using Signature = crypto::ED25519Signature;
  using BlockHash = primitives::BlockHash;
  using BlockNumber = primitives::BlockNumber;
  using RoundNumber = uint64_t;
  using MembershipCounter = uint64_t;

  using VoterSet = std::vector<Id>;

  using Clock = clock::SystemClock ;
  using Duration = Clock::Duration;
  using TimePoint = Clock::TimePoint;

  enum class State {
    START, PROPOSED, PREVOTED,
    PRECOMMITTED
  };
}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_COMMON_HPP
