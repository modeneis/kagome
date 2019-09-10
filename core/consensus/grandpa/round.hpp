/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP

#include <boost/optional.hpp>
#include <utility>

#include "consensus/grandpa/gossiper.hpp"
#include "consensus/grandpa/round_observer.hpp"
#include "consensus/grandpa/structs.hpp"
#include "consensus/grandpa/vote_graph.hpp"
#include "consensus/grandpa/vote_tracker.hpp"
#include "consensus/grandpa/voters_set.hpp"

namespace kagome::consensus::grandpa {

  struct RoundData {
    Id voter_id;
    Timer prevote_timer;
    Timer precommit_timer;
    std::shared_ptr<Gossiper> outgoing;
  };

  template <typename T>
  struct ImportResult {
    bool valid_voter = false;
    bool duplicated = false;
    boost::optional<EquivocationProof<T>> proof;
  };

  struct RoundState {
    RoundState(RoundNumber round,
               std::shared_ptr<VotersSet> voters,
               BlockInfo base)
        : round_number_(round), voters_(std::move(voters)) {
      // TODO(warchant): graph_ = std::make_shared<...>(...);
    }
    // TODO(warchant): complete

    outcome::result<ImportResult<Prevote>> importPrevote(
        std::shared_ptr<Chain> chain, SignedPrevote prevote) {
      ImportResult<Prevote> result{false, false, boost::none};
      // first, check if Id is a real voter we know about
      if (!voters_->exists(prevote.id)) {
        return result;  // voter is invalid
      }

      result.valid_voter = true;

      if (prevotes_.existsEquivocated(prevote)) {
        // bitfield_context.equivocatedPrevote(id);
        auto equivocated = prevotes_.getEquivocated(prevote.id);
        result.proof =
            EquivocationProof<Prevote>{round_number_, prevote.id, equivocated};
      } else if (prevotes_.existsSingle(prevote)) {
        BlockInfo info{prevote.message.number, prevote.message.hash};
        OUTCOME_TRY(graph_->insert(info, std::move(chain)));
      }

      historical_votes_.prevotes_seen.push_back(prevote);


    }

   private:
    RoundNumber round_number_;
    std::shared_ptr<VotersSet> voters_{};

    std::shared_ptr<VoteGraph> graph_;
    VoteTracker<Prevote> prevotes_;
    VoteTracker<Precommit> precommits_;
    HistoricalVotes historical_votes_{};
    boost::optional<BlockInfo> prevote_ghost_;
    boost::optional<BlockInfo> precommit_ghost_;
    boost::optional<BlockInfo> finalized_;
    boost::optional<BlockInfo> estimate_;
    bool completable_{false};
  };

}  // namespace kagome::consensus::grandpa

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_ROUND_HPP
