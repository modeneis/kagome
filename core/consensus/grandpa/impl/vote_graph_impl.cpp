/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "consensus/grandpa/impl/vote_graph_impl.hpp"

#include <boost/range/adaptor/indexed.hpp>

namespace kagome::consensus::grandpa {

  VoteGraphImpl::VoteGraphImpl(const BlockInfo &base) : base_(base) {}

  const BlockInfo &VoteGraphImpl::getBase() const {
    return base_;
  }

  void VoteGraphImpl::adjustBase(std::vector<BlockHash> ancestry_proof) {}

  outcome::result<void> VoteGraphImpl::insert(
      const kagome::consensus::grandpa::BlockInfo &block,
      std::shared_ptr<Chain> chain) {
    auto rcontaining = findContainingNodes(block);

    if (rcontaining) {
      auto containing = rcontaining.value();
      if (containing.empty()) {
        append(block, chain);
      } else {
        introduceBranch(containing, block.hash, block.number);
      }
    } else {
      // this entry already exists
    }

    auto inspecting_hash = block.hash;

    while (true) {
      VoteGraph::Entry active_entry = entries_[inspecting_hash];

      if (auto parent = active_entry.ancestorBlock(); parent) {
        inspecting_hash = parent.value();
      } else {
        break;
      }
    }
  }

  outcome::result<void> VoteGraphImpl::append(
      const kagome::consensus::grandpa::BlockInfo &block,
      std::shared_ptr<Chain> chain) {
    OUTCOME_TRY(ancestry, chain->ancestry(base_.hash, block.hash));

    ancestry.push_back(base_.hash);

    boost::optional<BlockNumber> ancestor_index = boost::none;
    for (auto [ancestor, i] : ancestry | boost::adaptors::indexed()) {
      if (auto entry = entries_.find(ancestor); entry != entries_.end()) {
        entry.descendents.push_back(block.hash);
        ancestor_index = BlockNumber{i};
      }
    }
  }

}  // namespace kagome::consensus::grandpa
