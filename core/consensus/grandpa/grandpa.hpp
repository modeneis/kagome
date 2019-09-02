/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_HPP

#include <cstdint>

#include "consensus/grandpa/types/round.hpp"

namespace kagome::consensus {

  class Grandpa {
   public:
    virtual ~Grandpa() = default;

    virtual void vote(Round round) = 0;

    virtual void start();
  };

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_GRANDPA_HPP
