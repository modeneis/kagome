/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP

#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  struct 

  struct Message {
    RoundNumber round;
    MembershipCounter counter;

  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
