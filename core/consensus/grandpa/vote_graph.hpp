/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP

#include "consensus/grandpa/chain.hpp"
#include "consensus/grandpa/vote_weight.hpp"

namespace kagome::consensus::grandpa {

  struct VoteGraph {
    // graph entry
    struct Entry {
      BlockNumber number;
      BlockHash hash;
      std::vector<BlockHash> ancestors;
      std::vector<BlockHash> descendents;
      VoteWeight cumulative_vote;

      // Get ancestor block by number. Returns `None` if there is no block
      // by that number in the direct ancestry.
      boost::optional<BlockHash> getAncestorBlockBy(BlockNumber n) {
        if (n >= number) {
          return boost::none;
        }
        const auto offset = number - n - 1;
        return ancestors.at(offset);
      }

      boost::optional<BlockHash> getLastAncestor() {
        if (ancestors.empty()) {
          return boost::none;
        }
        // return last ancestor
        return ancestors[ancestors.size() - 1];
      }
    };

    struct Subchain {
      std::vector<primitives::BlockHash> hashes;
      primitives::BlockNumber best_number;
    };

    virtual ~VoteGraph() = default;

    using Condition = std::function<bool(const VoteWeight &)>;

    virtual const BlockInfo &getBase() const = 0;

    /// Adjust the base of the graph. The new base must be an ancestor of the
    /// old base.
    ///
    /// Provide an ancestry proof from the old base to the new. The proof
    /// should be in reverse order from the old base's parent.
    virtual void adjustBase(
        std::vector<primitives::BlockHash> ancestry_proof) = 0;

    /// Insert a vote with given value into the graph at given hash and number.
    virtual outcome::result<void> insert(const BlockInfo &block,
                                         const VoteWeight &vote) = 0;

    /// Find the highest block which is either an ancestor of or equal to the
    /// given, which fulfills a condition.
    virtual boost::optional<BlockInfo> findAncestor(const BlockInfo &block,
                                                    Condition cond) = 0;

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
                                                 Condition cond) = 0;

    // given a key, node pair (which must correspond), assuming this node
    // fulfills the condition, this function will find the highest point at
    // which its descendents merge, which may be the node itself.
    virtual Subchain ghostFindMergePoint(
        const primitives::BlockHash &nodeKey,
        Entry &activeNode,
        /* force constrain?*/ Condition cond) = 0;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_HPP
