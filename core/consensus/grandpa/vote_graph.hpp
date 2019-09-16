/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP

#include "consensus/grandpa/chain.hpp"
#include "consensus/grandpa/message.hpp"

namespace kagome::consensus::grandpa {

  struct VoteGraph {
    using CumulativeVote = uint32_t;
    // graph entry
    struct Entry {
      BlockNumber number;
      std::vector<BlockHash> ancestors;
      std::vector<BlockHash> descendents;
      CumulativeVote cumulative_vote;
    };

    struct Subchain {
      std::vector<BlockHash> hashes;
      BlockNumber best_number;
    };

    virtual ~VoteGraph() = default;

    using Condition = std::function<bool(CumulativeVote)>;

    explicit VoteGraph(const BlockInfo &base) : base_(base) {}

    const auto &getBase() const {
      return base_;
    }

    /// Insert a vote with given value into the graph at given hash and number.
    /// Increases cumulative vote by 1 for the given block
    virtual outcome::result<void> insert(const BlockInfo &block
                                         /*vote weight */) = 0;

    /// Find the highest block which is either an ancestor of or equal to the
    /// given, which fulfills a condition.
    virtual boost::optional<BlockInfo> findAncestor(
        const BlockInfo &block,
        Condition cond = [](auto &&) { return true; }) = 0;

    /// Find the best GHOST descendent of the given block.
    /// Pass a closure used to evaluate the cumulative vote value.
    ///
    /// The GHOST (hash, number) returned will be the block with highest number
    /// for which the cumulative votes of descendents and itself causes the
    /// closure to evaluate to true.
    ///
    /// This assumes that the evaluation closure is one which returns true for
    /// at most a single descendent of a block, in that only one fork of a block
    /// can be "heavy" enough to trigger the threshold.
    ///
    /// Returns `None` when the given `current_best` does not fulfill the
    /// condition.
    virtual boost::optional<BlockInfo> findGhost(const BlockInfo &current_best,
                                                 Condition cond = [](auto &&) {
                                                   return true;
                                                 }) = 0;


   protected:
    BlockInfo base_;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP
