/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_VARINT_READER_HPP
#define KAGOME_VARINT_READER_HPP

#include <functional>
#include <memory>
#include <optional>

#include "libp2p/basic/readwriter.hpp"
#include "libp2p/multi/uvarint.hpp"

namespace libp2p::basic {
  class VarintReader {
   public:
    /**
     * Read a varint from the connection
     * @param conn to be read from
     * @param cb to be called, when a varint or maximum bytes from the
     * connection are read
     */
    static void readVarint(
        std::shared_ptr<ReadWriter> conn,
        std::function<void(std::optional<multi::UVarint>)> cb);

   private:
    static void readVarint(
        std::shared_ptr<ReadWriter> conn,
        std::function<void(std::optional<multi::UVarint>)> cb,
        uint8_t current_length,
        std::shared_ptr<std::vector<uint8_t>> varint_buf);
  };
}  // namespace libp2p::basic

#endif  // KAGOME_VARINT_READER_HPP
