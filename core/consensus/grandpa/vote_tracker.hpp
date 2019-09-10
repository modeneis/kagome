/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP

#include <unordered_map>

#include "consensus/grandpa/structs.hpp"

namespace kagome::consensus::grandpa {

  template <typename Vote>
  struct VoteTracker {
    enum class AddVoteResult { SUCCESS, DUPLICATED, VOTED_TWICE };

    enum class ExistsResult { FALSE, TRUE, EQUIVOCATED };
    /**
     * @brief Adds a vote to a storage.
     * @param id peer id
     * @param n vote
     * @return SUCCESS if peer voted once, DUPLICATED if we received duplicated
     * vote, VOTED_TWICE if peer created two different votes
     */
    AddVoteResult addVote(Id id, const SignedMessage<Vote> &n) {
      auto it = votes_.find(id);
      if (it == votes_.end()) {
        votes_.insert({id, n});
        return outcome::success();
      }

      Vote &v = it->second;
      if (v == n) {
        return AddVoteResult::DUPLICATED;
      }

      // peer voted twice for the same round...
      eq_votes_.insert({id, {n, v}});
      return AddVoteResult::VOTED_TWICE;
    }

    const auto &getVotes() const {
      return votes_;
    }

    const auto &getEquivocatedVotes() const {
      return eq_votes_;
    }

    bool existsSingle(const SignedMessage<Vote> &v) const {
      auto it = votes_.find(v.id);
      return it != votes_.end();
    }

    bool existsEquivocated(const SignedMessage<Vote> &v) const {
      auto it = eq_votes_.find(v.id);
      return it != eq_votes_.end();
    }

    Equivocated<Vote> getEquivocated(Id id) const {
      return eq_votes_[id];
    }

   private:
    std::unordered_map<Id, SignedMessage<Vote>> votes_;
    std::unordered_map<Id, Equivocated<Vote>> eq_votes_;
    uint64_t current_weight_{0};
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_TRACKER_HPP
