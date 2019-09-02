/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_MESSAGE_BODY_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_MESSAGE_BODY_HPP

#include "consensus/grandpa/types/common.hpp"
#include "crypto/ed25519_types.hpp"

namespace kagome::network {

  struct StageMessage {
    RoundNumber round_number;
    TrackerCounter id_v;
    Vote vote;
    crypto::ED25519PublicKey authority;
    crypto::ED25519Signature signature;
  };

  struct Prevote : public StageMessage {};

  template <class Stream,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const Prevote &mb) {
    // implement
    return s;
  }

  template <class Stream,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, Prevote &mb) {
    // implement
    return s;
  }

  struct Precommit : public StageMessage {};

  template <class Stream,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const Precommit &mb) {
    // implement
    return s;
  }

  template <class Stream,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, Precommit &mb) {
    // implement
    return s;
  }

}  // namespace kagome::network

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_MESSAGE_BODY_HPP
