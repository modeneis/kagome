/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_STRUCTS_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_STRUCTS_HPP

#include <boost/asio/steady_timer.hpp>
#include "common/blob.hpp"
#include "common/wrapper.hpp"
#include "crypto/ed25519_types.hpp"
#include "primitives/common.hpp"

namespace kagome::consensus::grandpa {

  using Timer = boost::asio::basic_waitable_timer<std::chrono::steady_clock>;

  using RoundNumber = common::Wrapper<uint64_t, struct RoundNumberTag>;

  /// voter identifier
  using Id = primitives::AuthorityId;

  /// voter signature
  using Signature = crypto::ED25519Signature;

  /// @tparam Message A protocol message or vote.
  template <typename Message>
  struct SignedMessage {
    Message message;
    Signature signature;
    Id id;
  };

  template <typename Message>
  struct Equivocated {
    SignedMessage<Message> first;
    SignedMessage<Message> second;
  };

  /// Proof of an equivocation (double-vote) in a given round.
  template <typename Message>
  struct EquivocationProof {  // NOLINT
    /// The round number equivocated in.
    RoundNumber round{0};
    /// The identity of the equivocator.
    Id id;
    Equivocated<Message> proof;
  };

  template <typename Tag>
  struct BlockVote {
    primitives::BlockHash hash;
    primitives::BlockNumber number;
  };

  using BlockInfo = BlockVote<struct BlockInfoTag>;
  using Precommit = BlockVote<struct PrecommitTag>;
  using Prevote = BlockVote<struct PrevoteTag>;
  using PrimaryPropose = BlockVote<struct PrimaryProposeTag>;

  using SignedPrevote = SignedMessage<Prevote>;
  using SignedPrecommit = SignedMessage<Precommit>;
  using SignedPrimaryPropose = SignedMessage<PrimaryPropose>;

  using MembershipCounter =
      common::Wrapper<uint64_t, struct MembershipCounterTag>;

  MembershipCounter &operator++(MembershipCounter &c) {
    ++c.unwrap_mutable();
    return c;
  }

  /// A commit message which is an aggregate of precommits.
  struct Commit {
    BlockInfo info;
    std::vector<SignedPrecommit> precommits;
  };

  struct CatchUp {
    RoundNumber round;
    std::vector<SignedPrevote> prevotes;
    std::vector<SignedPrecommit> precommits;
    primitives::BlockHash base_hash;
    primitives::BlockNumber base_number;
  };

  using PrevoteEquivocation = EquivocationProof<Prevote>;
  using PrecommitEquivocation = EquivocationProof<Precommit>;

  struct HistoricalVotes {
    std::vector<SignedPrevote> prevotes_seen;
    std::vector<SignedPrecommit> precommits_seen;
    std::vector<SignedPrimaryPropose> proposes_seen;
    uint64_t prevote_idx;
    uint64_t precommit_idx;
  };

  // TODO(akvinikym) 04.09.19: implement the SCALE codecs
  template <class Stream,
            class Tag,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const BlockVote<Tag> &v) {
    return s;
  }

  template <class Stream,
            class Tag,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, BlockVote<Tag> &v) {
    return s;
  }
}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_STRUCTS_HPP
