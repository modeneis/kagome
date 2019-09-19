/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_STATE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_STATE_HPP

#include "consensus/grandpa/message.hpp"

namespace kagome::consensus::grandpa {

  struct RoundState {
    Prevote prevote_ghost;
    BlockInfo best_final_candidate;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_STATE_HPP
