/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP

#include "consensus/grandpa/message.hpp"

namespace kagome::consensus::grandpa {

  // tracks votes we received during one grandpa round
  struct VoteTracker {
    enum class PushResult {
      SUCCESS,     // peer votes first time and his vote successfully imported
      DUPLICATE,   // peer voted second time with the same vote
      EQUIVOCATED  // peer voted second time with the different vote
    };

    virtual ~VoteTracker() = default;

    // push valid vote to a tracker
    virtual PushResult push(const SignedPrevote &vote) = 0;
    virtual PushResult push(const SignedPrecommit &vote) = 0;

    // returns vector of all votes for all blocks on round r
    virtual std::vector<SignedPrevote> getPrevotes() const = 0;
    virtual std::vector<SignedPrecommit> getPrecommits() const = 0;

    virtual size_t prevoteWeight() const = 0;
    virtual size_t precommitWeight() const = 0;

    // returns vector of all equivocatory votes for all blocks on round r
    virtual std::vector<EquivocatorySignedPrecommit> getEquivocatoryVotes()
        const = 0;

    // total number of observed votes for block B {hash, number} in round R
    // includes votes for B and all its descendants
    virtual size_t getTotalVotes(BlockHash hash, BlockNumber number) = 0;

    virtual Justification getJustification(const BlockInfo &info) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
