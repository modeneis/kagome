/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_BABE_BABE_HPP
#define KAGOME_CORE_CONSENSUS_BABE_BABE_HPP

#include <array>
#include <chrono>
#include <iostream>

#include <sr25519/sr25519.h>
#include <boost/asio.hpp>
#include "consensus/babe/chain.hpp"
#include "consensus/network/network.hpp"
#include "primitives/block.hpp"
#include "scale/scale.hpp"

namespace kagome::consensus::babe {

  using EpochNum = uint64_t;
  using SlotNum = uint64_t;
  using RandomValue = uint64_t;

  struct BabeConfig {
    size_t slot_duration;
    size_t number_of_slots;
    double threshold;
  };

  struct VRFValue {
    uint64_t value;
  };

  struct VRFProof {
    std::array<uint8_t, 32> scallar;
  };

  struct VRFOutput {
    VRFValue value;
    VRFProof proof;
  };

  struct BabePreDigest {
    VRFOutput output;
    std::array<uint8_t, SR25519_PUBLIC_SIZE> author;
    SlotNum slot;
  };

  template <class Stream>
  Stream &operator<<(Stream &s, const BabePreDigest &b) {
    return s << b.output.value.value << b.output.proof.scallar << b.author
             << b.slot;
  }

  struct BlockNode {
    primitives::Block block;
    std::shared_ptr<BlockNode> previousNode;
  };

  class Babe {
   public:
    void runEpoch(EpochNum epoch_number, BabeConfig config,
                  const primitives::Block &finalized_block) {
      auto epoch_start = boost::posix_time::second_clock::local_time();

      auto r = randomness_map_[current_epoch_];
      auto slot_leadership = slotLeadership(r, config.number_of_slots);

      timer_.async_wait(std::bind(runSlot, boost::asio::placeholders::error,
                                  config, 0, slot_leadership, epoch_start));
    }

    void runSlot(const boost::system::error_code &, const BabeConfig &config,
                 SlotNum slot_number,
                 const std::vector<VRFOutput> &slot_leadership,
                 boost::posix_time::ptime &epoch_start) {
      if (slot_number < config.number_of_slots) {
        std::cout << "run slot " << slot_number << std::endl;

        // run slots
        if (slot_leadership[slot_number].value.value < config.threshold) {
          runLeader(slot_number);
        }

        timer_.expires_at(
            epoch_start
            + boost::posix_time::milliseconds(config.slot_duration));
        timer_.async_wait(std::bind(runSlot, boost::asio::placeholders::error,
                                    config, slot_number + 1, slot_leadership,
                                    epoch_start));
        return;
      }
      // else end of epoch
      auto random_value = getRandomValue(chain_.bestChain());
      randomness_map_[current_epoch_ + 2] = random_value;
    }

    void runLeader(size_t slot_number) {
      auto txs = pool_->consumeTransactions();
      auto block = createBlock(txs);
      auto babe_header = getHeader(slot_number);

      auto encoded_res = scale::encode(block, babe_header);
      if (encoded_res) {
        network_.broadcast(encoded_res.value);
      }
    }

    /**
     * Create block from provided transactions
     */
    primitives::Block createBlock(std::vector<primitives::Extrinsic> xts);

    RandomValue getRandomValue(
        const std::vector<primitives::Block> &blockchain) {}

    /**
     * Get tuple consisting of pi (vrf proof), Sb (signature of encoded tuple of
     * slot number, header of block and vrf proof), public key of block
     * producer, slot number and d (vrf value)
     */
    BabeHead getHeader(size_t slot_number, Block block);

    Babe() {
      randomness_map_[0] = 0;
      randomness_map_[1] = 0;

      network.onReceiveBlock([](Block block) {
        if (validate(block)) {
          chain_.addBlock(block);
        }
      });
    }

    bool validate(Block block);

   private:
    std::vector<VRFOutput> slotLeadership(size_t random_value,
                                          size_t slots_number);

    Chain chain_;
    boost::asio::deadline_timer timer_;

    network::Network network_;
    std::unique_map<EpochNum, RandomValue> randomness_map_;
    EpochNum current_epoch_;
    std::shared_ptr<TransactionPool> pool_;
    SessionKeypair s_kp_;
  };

}  // namespace kagome::consensus::babe

#endif  // KAGOME_CORE_CONSENSUS_BABE_BABE_HPP
