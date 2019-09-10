/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTER_HPP

#include "consensus/grandpa/round_observer.hpp"

namespace kagome::consensus::grandpa {

  struct GrandpaVoter : public RoundObserver {
    ~GrandpaVoter() override = default;
  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTER_HPP
