#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(grandpa
    impl/grandpa_impl.cpp
    )
target_link_libraries(grandpa
    ed25519_types
    logger
    outcome
    )
