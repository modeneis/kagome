/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP

#include "consensus/grandpa/message.hpp"

namespace kagome::consensus::grandpa {

  struct VotersSet {
    virtual ~VotersSet() = default;

    // returns true if given Id exists in voter set
    virtual bool exists(const Id& id) const = 0;

    // getter for current membership counter
    virtual MembershipCounter getMembershipCounter() const = 0;
  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP
