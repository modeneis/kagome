/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_FACTORY_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_FACTORY_HPP

#include "message.hpp"

namespace kagome::consensus::grandpa {

  struct VoteFactory {
    virtual ~VoteFactory() = default;

    virtual SignedVote sign(const Vote&)
  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_FACTORY_HPP
