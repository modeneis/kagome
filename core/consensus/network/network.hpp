/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_NETWORK_NETWORK_HPP
#define KAGOME_CORE_CONSENSUS_NETWORK_NETWORK_HPP

#include <functional>
#include <gsl/span>

namespace kagome::consensus::network {

  class Network {
   public:
    void broadcast(gsl::span<uint8_t> message) {}

    void onReceiveMessage(const std::function<void(gsl::span<uint8_t>)> &){};
  };

}  // namespace kagome::consensus::network

#endif  // KAGOME_CORE_CONSENSUS_NETWORK_NETWORK_HPP
