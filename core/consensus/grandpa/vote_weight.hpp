/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_WEIGHT_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_WEIGHT_HPP

#include <boost/dynamic_bitset.hpp>
#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  struct VoteWeight {
    boost::dynamic_bitset<> prevotes;
    boost::dynamic_bitset<> precommits;

    auto &operator+=(const VoteWeight &vote) {
      // TODO(warchant): implement
      return *this;
    }

    TotalWeight totalWeight() {}
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_WEIGHT_HPP
