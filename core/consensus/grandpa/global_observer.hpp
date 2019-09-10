/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_GLOBAL_OBSERVER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_GLOBAL_OBSERVER_HPP

#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  struct GlobalObserver {
    virtual ~GlobalObserver() = default;

    virtual void onCommit(Commit c) = 0;

    virtual void onCatchUp(CatchUp c) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_GLOBAL_OBSERVER_HPP
