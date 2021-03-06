/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_PRIMITIVES_TRANSACTION_VALIDITY_HPP
#define KAGOME_CORE_PRIMITIVES_TRANSACTION_VALIDITY_HPP

#include <cstdint>
#include <vector>

#include <boost/variant.hpp>
#include "primitives/transaction.hpp"

namespace kagome::primitives {

  /**
   * This is the same structure as in
   * https://github.com/paritytech/substrate/blob/master/core/sr-primitives/src/transaction_validity.rs
   */

  /// Transaction is valid
  struct Valid {
    /**
     * @brief Priority of the transaction.
     * Priority determines the ordering of two transactions that have all
     * their dependencies (required tags) satisfied.
     */
    TransactionPriority priority;

    /**
     * @brief Transaction dependencies
     * A non-empty list signifies that some other transactions which provide
     * given tags are required to be included before that one.
     */
    std::vector<TransactionTag> requires;

    /**
     * @brief Provided tags
     * A list of tags this transaction provides. Successfuly importing the
     * transaction will enable other transactions that depend on (require)
     * those tags to be included as well. Provided and requried tags allow
     * Substrate to build a dependency graph of transactions and import them
     * in the right (linear) order.
     */
    std::vector<TransactionTag> provides;

    /**
     * @brief Transaction longevity
     * Longevity describes minimum number of blocks the validity is correct.
     * After this period transaction should be removed from the pool or
     * revalidated.
     */
    TransactionLongevity longevity;

    bool operator==(const Valid &rhs) const {
      return priority == rhs.priority and requires == rhs.requires
             and provides == rhs.provides and longevity == rhs.longevity;
    }

    bool operator!=(const Valid &rhs) const {
      return !operator==(rhs);
    }
  };

  /// Transaction is invalid. Details are described by the error code.
  struct Invalid {
    uint8_t error;

    bool operator==(const Invalid &rhs) const {
      return error == rhs.error;
    }
    bool operator!=(const Invalid &rhs) const {
      return error != rhs.error;
    }
  };

  /// Transaction validity can't be determined.
  struct Unknown {
    uint8_t error;

    bool operator==(const Unknown &rhs) const {
      return error == rhs.error;
    }
    bool operator!=(const Unknown &rhs) const {
      return error != rhs.error;
    }
  };

  /**
   * Information on a transaction's validity and, if valid, on how it relate to
   * other transactions.
   */
  using TransactionValidity = boost::variant<Invalid, Valid, Unknown>;

  /**
   * @brief outputs object of type Invalid to stream
   * @tparam Stream stream type
   * @param s stream reference
   * @param v value to output
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const Invalid &v) {
    return s << v.error;
  }

  /**
   * @brief outputs object of type Valid to stream
   * @tparam Stream stream type
   * @param s stream reference
   * @param v value to output
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const Valid &v) {
    return s << v.priority << v.requires << v.provides << v.longevity;
  }

  /**
   * @brief outputs object of type Unknown to stream
   * @tparam Stream stream type
   * @param s stream reference
   * @param v value to output
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_encoder_stream>>
  Stream &operator<<(Stream &s, const Unknown &v) {
    return s << v.error;
  }

  /**
   * @brief decodes object of type Invalid from stream
   * @tparam Stream input stream type
   * @param s stream reference
   * @param v value to decode
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, Invalid &v) {
    return s >> v.error;
  }

  /**
   * @brief decodes object of type Valid from stream
   * @tparam Stream input stream type
   * @param s stream reference
   * @param v value to decode
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, Valid &v) {
    return s >> v.priority >> v.requires >> v.provides >> v.longevity;
  }

  /**
   * @brief decodes object of type Unknown from stream
   * @tparam Stream input stream type
   * @param s stream reference
   * @param v value to decode
   * @return reference to stream
   */
  template <class Stream,
            typename = std::enable_if_t<Stream::is_decoder_stream>>
  Stream &operator>>(Stream &s, Unknown &v) {
    return s >> v.error;
  }
}  // namespace kagome::primitives

#endif  // KAGOME_CORE_PRIMITIVES_TRANSACTION_VALIDITY_HPP
