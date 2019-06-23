/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_BABE_CHAIN_HPP
#define KAGOME_CORE_CONSENSUS_BABE_CHAIN_HPP

#include "primitives/block.hpp"

namespace kagome::consensus::babe {

  class Chain {
   public:
    primitives::BlockHeader bestBlockHeader();

    primitives::Block bestBlock();

    std::vector<primitives::Block> bestChain();

    void addBlock(primitives::Block block);
  };

}  // namespace kagome::consensus::babe

#endif  // KAGOME_CORE_CONSENSUS_BABE_CHAIN_HPP
