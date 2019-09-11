/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP

#include "common/wrapper.hpp"
#include "primitives/common.hpp"
#include <boost/variant.hpp>

namespace kagome::consensus::grandpa {

  using Id = primitives::AuthorityId;
  using Signature = common::Wrapper<std::vector<uint8_t>, struct SignatureTag>;
  using BlockHash = primitives::BlockHash;
  using BlockNumber = primitives::BlockNumber;
  using RoundNumber = uint64_t;
  using MembershipCounter = uint64_t;

  using BlockInfo = primitives::BlockInfo;

  struct Prevote: public BlockInfo {};
  struct Precommit: public BlockInfo {};

  using Vote = boost::variant<Prevote, Precommit>;

  struct SignedVote {
    Vote vote;
    Signature signature;
    Id signer;
  };

  // justification for block B in round r
  struct Justification {
    std::vector<SignedVote> items;
  };

  // finalizing message or block B in round r
  struct Fin {
    RoundNumber round{0};
    BlockInfo vote;
    Justification justification;
  };

  // either prevote or precommit
  struct Message {
    RoundNumber round{0};
    MembershipCounter counter{0};
    Id id;
    SignedVote vote;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
