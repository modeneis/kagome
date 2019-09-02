/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_NETWORK_PEER_SERVER_HPP
#define KAGOME_CORE_NETWORK_PEER_SERVER_HPP

#include "network/types/block_announce.hpp"
#include "network/types/block_request.hpp"
#include "network/types/block_response.hpp"
#include "network/types/finalizing_message.hpp"
#include "network/types/grandpa_stage_message.hpp"

namespace kagome::network {

  /**
   * Networking abstraction for receiving and processing messages as local peer
   */
  class PeerServer {
   public:
    virtual ~PeerServer() = default;

    /**
     * Subscribe for the block requests
     */
    virtual void onBlocksRequest(
        std::function<outcome::result<BlockResponse>(const BlockRequest &)>)
        const = 0;

    /**
     * Process block announcement
     */
    virtual void onBlockAnnounce(
        std::function<void(const BlockAnnounce &)>) const = 0;

    /**
     * Process grandpa messages
     */
    virtual void onFinalizingMessage(
        std::function<void(const FinalizingMessage &)>) const = 0;

    virtual void onPrevote(std::function<void(const Prevote &)>) const = 0;

    virtual void onPrecommit(std::function<void(const Precommit &)>) const = 0;
  };

}  // namespace kagome::network

#endif  // KAGOME_CORE_NETWORK_PEER_SERVER_HPP
