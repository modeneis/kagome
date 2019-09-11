/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP

#include "common/wrapper.hpp"
#include "primitives/common.hpp"

namespace kagome::consensus::grandpa {

  using Id = common::Wrapper<std::vector<uint8_t>, struct PublicKeyTag>;
  using Signature = common::Wrapper<std::vector<uint8_t>, struct SignatureTag>;
  using BlockHash = primitives::BlockHash;
  using BlockNumber = primitives::BlockNumber;
  using RoundNumber = uint64_t;
  using MembershipCounter = uint64_t;

  struct BlockInfo {
    BlockHash hash;
    BlockNumber number;
  };

  struct Vote {
    enum class Type { PREVOTE = 0, PRECOMMIT = 1 };

    Type type;
    BlockHash hash;
    BlockNumber number;
  };

  struct SignedVote {
    Vote vote;
    Signature signature;
    Id signer;
  };

  // justification for block B in round r
  struct Justification {
    struct Item {
      Vote vote;  // vote for block that is higher than B
      Id signer;
      // signature of voter 'signer' broadcasted during pre-commit subround of
      // round r
      Signature signature;
    };

    std::vector<Item> items;
  };

  // finalizing message or block B in round r
  struct Fin {
    RoundNumber round{0};
    Vote vote;
    Justification justification;
  };

  struct Message {
    RoundNumber round{0};
    Id id;
    SignedVote vote;
    MembershipCounter counter{0};
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
