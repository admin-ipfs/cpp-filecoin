#
# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0
#

addtest(hamt_test
    hamt_test.cpp
    )
target_link_libraries(hamt_test
    hamt
    hexutil
    ipfs_datastore_in_memory
    )
