/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_PRIMITIVES_COMMON_HPP
#define KAGOME_CORE_PRIMITIVES_COMMON_HPP

#include <cstdint>

#include "primitives/session_key.hpp"

namespace kagome::primitives {
  using BlocksRequestId = uint64_t;

  using BlockNumber = uint64_t;
  using BlockHash = common::Hash256;

  using AuthorityId = SessionKey;
  using AuthorityIndex = uint64_t;

  struct BlockInfo {
    BlockNumber block_number{};
    BlockHash block_hash;

    inline bool operator==(const BlockInfo &rhs) const {
      return block_hash == rhs.block_hash and block_number == rhs.block_number;
    }
  };
}  // namespace kagome::primitives

#endif  // KAGOME_CORE_PRIMITIVES_COMMON_HPP
