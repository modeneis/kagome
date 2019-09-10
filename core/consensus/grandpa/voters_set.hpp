/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP

#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  struct VotersSet {
    virtual ~VotersSet() = default;

    // returns true if given Id exists in voter set
    virtual bool exists(const Id& id) const = 0;

    // whenever voters set is changed, callback should be executed with new membership counter
    virtual void onChange(std::function<void(MembershipCounter)> cb) = 0;

    // getter for current membership counter
    virtual MembershipCounter getMembershipCounter() const = 0;
  };

}

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTERS_SET_HPP