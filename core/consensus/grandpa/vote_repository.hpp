/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_REPOSITORY_HPP
#define KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_REPOSITORY_HPP

#include <boost/optional.hpp>
#include <outcome/result.hpp>
#include "consensus/grandpa/types/justification.hpp"
#include "crypto/ed25519_types.hpp"

namespace kagome::consensus {

  class VoteRepository {
   public:
    virtual ~VoteRepository() = default;

    /**
     * Put prevote \param v from \param authority during round \param
     * round_number signed by \param signature
     * @return void if signature valid and put succeeded, error otherwise
     */
    virtual outcome::result<void> putPrevote(
        crypto::ED25519PublicKey authority,
        Vote v,
        RoundNumber round_number,
        crypto::ED25519Signature signature) = 0;

    /**
     * Put precommit \param v from \param authority during round \param
     * round_number signed by \param signature
     * @return void if signature valid and put succeeded, error otherwise
     */
    virtual outcome::result<void> putPrecommit(
        crypto::ED25519PublicKey authority,
        Vote v,
        RoundNumber round_number,
        crypto::ED25519Signature signature) = 0;

    /**
     * Get justification for \param candidate during prevote of round \param
     * round_number
     * @return vector of justifications
     *
     * @note GrandpaJustification is the vector of votes from authorities that
     * have voted for the block >= candidate
     */
    virtual std::vector<GrandpaJustification> getPrevote(
        RoundNumber round_number, Vote candidate) const = 0;
    virtual std::vector<GrandpaJustification> getPrecommit(
        RoundNumber round_number, Vote candidate) const = 0;

    /**
     * Best candidate during \param round_number
     * @return Vote v such that there are more than 2/3 GrandpaJustifications
     * for Votes that are >= than v
     */
    virtual boost::optional<Vote> bestFinalCandidate(
        RoundNumber round_number) const = 0;
  };

}  // namespace kagome::consensus

#endif  // KAGOME_CORE_CONSENSUS_GRANDPA_VOTE_REPOSITORY_HPP
