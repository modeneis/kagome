/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_IMPL_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_IMPL_HPP

#include "consensus/grandpa/vote_graph.hpp"

namespace kagome::consensus::grandpa {

  class VoteGraphImpl : public VoteGraph {
   public:
    explicit VoteGraphImpl(const BlockInfo &base);

    const BlockInfo &getBase() const override;

    void adjustBase(std::vector<BlockHash> ancestry_proof) override;

    outcome::result<void> insert(
        const kagome::consensus::grandpa::BlockInfo &block,
        std::shared_ptr<Chain> chain) override;

    boost::optional<std::vector<BlockHash>> findContainingNodes(
        const kagome::consensus::grandpa::BlockInfo &block) override;

    outcome::result<void> append(
        const kagome::consensus::grandpa::BlockInfo &block,
        std::shared_ptr<Chain> chain) override;

   private:
    BlockInfo base_;
    std::unordered_map<BlockHash, VoteGraph::Entry> entries_;
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_GRAPH_IMPL_HPP
