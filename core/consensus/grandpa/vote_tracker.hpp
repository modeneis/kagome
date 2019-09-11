/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP

#include "message.hpp"

namespace kagome::consensus::grandpa {

  // tracks votes we received during one grandpa round
  struct VoteTracker {
    virtual ~VoteTracker() = default;

    using EquivocatorySignedVote = std::pair<SignedVote, SignedVote>;

    // push valid vote to a tracker
    virtual void push(const SignedVote &vote) = 0;

    // returns vector of all votes for all blocks on round r
    virtual std::vector<SignedVote> getAllVotes() const = 0;

    // returns vector of all equivocatory votes for all blocks on round r
    virtual std::vector<EquivocatorySignedVote> getEquivocatoryVotes()
        const = 0;

    // total number of observed votes for block B {hash, number} in round R
    // includes votes for B and all its descendants
    virtual size_t getTotalVotes(BlockHash hash, BlockNumber number) = 0;

    virtual Justification getJustification(const BlockInfo& info) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
