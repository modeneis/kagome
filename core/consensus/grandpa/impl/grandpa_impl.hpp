/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_IMPL_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_IMPL_HPP

#include <scale/scale.hpp>
#include "consensus/grandpa/grandpa.hpp"

#include <boost/asio/basic_waitable_timer.hpp>
#include "clock/clock.hpp"
#include "common/logger.hpp"
#include "consensus/grandpa/vote_repository.hpp"
#include "crypto/ed25519_types.hpp"
#include "crypto/hasher.hpp"
#include "network/network_state.hpp"
#include "network/types/finalizing_message.hpp"
#include "network/types/grandpa_stage_message.hpp"
#include "primitives/common.hpp"
#include "primitives/scheduled_change.hpp"
#include "runtime/grandpa.hpp"

namespace kagome::consensus {

  class GrandpaImpl : public Grandpa {
    using Timer = boost::asio::basic_waitable_timer<std::chrono::system_clock>;

   public:
    ~GrandpaImpl() override = default;

    GrandpaImpl(common::Logger logger = common::createLogger("Grandpa"))
        : logger_{std::move(logger)} {}

    void start() override {
      state_->peer_server->onFinalizingMessage(
          [this](const network::FinalizingMessage &msg) {
            auto round_number = msg.round_number;
            auto it = round_map_.find(round_number);
            if (it == round_map_.end()) {
              logger_->warn("Round {} does not exist", round_number);
              return;
            }

            if (clock_->now() > it->second.start_time + it->second.duration) {
              logger_->warn("Round {} expired", round_number);
              return;
            }

            // TODO check for completability

            // TODO validate vote
          });
    }

    void vote(Round round) override {
      auto now = clock_->now();

      auto primary = primaryVoter(round);

      if (primary == keypair_.publicKey) {
        runPrimary(round);
      }

      // wait for prevote messages
      timer_.expires_at(now + round.duration);
      timer_.async_wait([this](auto &&ec) {
        if (ec) {
          logger_->error("Error happened in timer: {}", ec.message());
        }

        betterVoteNotReceived();
      });
      timer_.cancel()
    }

   private:
    primitives::AuthorityId primaryVoter(const Round &round) const {
      const auto &authorities = round.voters_set;
      return authorities[round.number % authorities.size()];
    }

    network::FinalizingMessage prepareFinalizationMsg(
        const Round &round) const {
      auto opt_final_candidate =
          vote_repository_->bestFinalCandidate(round.number - 1);

      if (not opt_final_candidate) {
        // log error
      }

      auto final_candidate = opt_final_candidate.value();

      network::FinalizingMessage f_msg{
          .vote = final_candidate,
          .round_number = round.number,
          .justification = prepareJusitification(final_candidate)};
      return f_msg;
    }

    primitives::Justification prepareJusitification(Vote candidate) const {
      primitives::Justification justification;
      justification.data.put(
          scale::encode(vote_repository_->getPrevote(candidate)).value());
      return justification;
    }

    void runPrimary(const Round &round) const {
      auto msg = prepareFinalizationMsg(round);
      broadcast(msg);
    }

    void betterVoteNotReceived() {}

    void broadcast(network::FinalizingMessage msg) const {}

    Round current_round_;
    //    std::shared_ptr<runtime::Grandpa> grandpa_api_;
    std::shared_ptr<clock::SystemClock> clock_;
    crypto::ED25519Keypair keypair_;  ///< this peer's keypair
    std::shared_ptr<network::NetworkState> state_;
    std::shared_ptr<crypto::Hasher> hasher_;
    std::shared_ptr<VoteRepository> vote_repository_;
    std::unordered_map<RoundNumber, Round> round_map_;
    Timer timer_;

    common::Logger logger_;
  };

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_IMPL_GRANDPA_IMPL_HPP
