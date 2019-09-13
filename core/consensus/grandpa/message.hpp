/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP

#include <boost/variant.hpp>
#include "consensus/grandpa/common.hpp"

namespace kagome::consensus::grandpa {

  struct Prevote : public BlockInfo {};
  struct Precommit : public BlockInfo {};

  using Vote = boost::variant<Prevote, Precommit>;

  namespace detail {
    template <typename VoteType>
    struct SignedVote {
      VoteType vote;
      Signature signature;
      Id signer;
    };
  }  // namespace detail

  using SignedPrevote = detail::SignedVote<Prevote>;
  using SignedPrecommit = detail::SignedVote<Precommit>;

  using EquivocatorySignedPrecommit =
      std::pair<SignedPrecommit, SignedPrecommit>;
  using EquivocatorySignedPrevote = std::pair<SignedPrecommit, SignedPrecommit>;

  // justification for block B in round r
  struct Justification {
    std::vector<SignedPrecommit> items;
  };

  // finalizing message or block B in round r
  struct Fin {
    RoundNumber round_number{0};
    BlockInfo vote;
    Justification justification;
  };

  // either prevote or precommit
  struct VoteMessage {
    RoundNumber round_number{0};
    MembershipCounter counter{0};
    Id id;
    boost::variant<SignedPrevote, SignedPrecommit> vote;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_MESSAGE_HPP
