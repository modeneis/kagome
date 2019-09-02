/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_JUSTIFICATION_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_JUSTIFICATION_HPP

#include "consensus/grandpa/types/common.hpp"

namespace kagome::consensus {

  struct GrandpaJustification {
    Vote vote;                         ///< vote being signed by the peer
    crypto::ED25519Signature pc_sign;  ///< precommit signature
    crypto::ED25519PublicKey
        pubkey;  ///< public key of the peer signed this vote
  };

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_TYPES_JUSTIFICATION_HPP
