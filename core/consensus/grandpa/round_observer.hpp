/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_OBSERVER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_OBSERVER_HPP

#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  /**
   * @class Observer
   * @brief observes incoming messages. Abstraction of a network.
   */
  struct RoundObserver {
    virtual ~RoundObserver() = default;

    virtual void onPrecommit(Precommit pc) = 0;

    virtual void onPrevote(Prevote pv) = 0;

    virtual void onPrimaryPropose(PrimaryPropose pv) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_OBSERVER_HPP
