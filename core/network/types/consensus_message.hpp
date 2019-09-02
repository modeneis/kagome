/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_NETWORK_TYPES_CONSENSUS_MESSAGE_HPP
#define KAGOME_CORE_NETWORK_TYPES_CONSENSUS_MESSAGE_HPP

#include "common/blob.hpp"

namespace kagome::network {

  const static common::Blob<4> BABE_ENGINE_IDENTIFIER{{'B', 'A', 'B', 'E'}};
  const static common::Blob<4> GRANDPA_ENGINE_IDENTIFIER{{'F', 'R', 'N', 'K'}};

  /**
   * Data, describing the requested block
   */
  struct ConsensusMessage {
    common::Blob<4> engine_id;  ///< The consensus engine unique identifier
    std::vector<uint8_t> data;  ///< Consensus message payload
  };
}  // namespace kagome::network

#endif  // KAGOME_CORE_NETWORK_TYPES_CONSENSUS_MESSAGE_HPP
