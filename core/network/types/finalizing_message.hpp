/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_FINALIZING_MESSAGE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_FINALIZING_MESSAGE_HPP

#include "consensus/grandpa/types/common.hpp"
#include "primitives/justification.hpp"

namespace kagome::network {

  struct FinalizingMessage {
    RoundNumber round_number;
    Vote vote;
    primitives::Justification justification;
  };

}  // namespace kagome::network

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_FINALIZING_MESSAGE_HPP
