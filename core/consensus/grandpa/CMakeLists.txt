#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(voting_round
    impl/voting_round_impl.cpp
    )

target_link_libraries(voting_round
    logger
    outcome
    )
