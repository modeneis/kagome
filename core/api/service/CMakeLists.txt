#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

add_library(api_service
    api_service.hpp
    api_service.cpp
    )
target_link_libraries(api_service
    Boost::boost
    logger
    )
