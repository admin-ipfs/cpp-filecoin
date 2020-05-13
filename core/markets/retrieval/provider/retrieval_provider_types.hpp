/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef CPP_FILECOIN_CORE_MARKETS_RETRIEVAL_PROVIDER_TYPES_HPP
#define CPP_FILECOIN_CORE_MARKETS_RETRIEVAL_PROVIDER_TYPES_HPP

#include <string>

#include "markets/retrieval/common_types.hpp"
#include "primitives/types.hpp"

namespace fc::markets::retrieval::provider {
  using primitives::TokenAmount;

  /**
   * @struct Provider deal state
   */
  struct ProviderDealState {
    /* Current deal status */
    DealStatus deal_status;

    /* Sender PeerID */
    uint64_t sender_peer_id;

    /* Total bytes sent */
    uint64_t total_sent;

    /* Received tokens amount */
    TokenAmount funds_received;

    /* Signal message */
    std::string message;

    /* Current payment interval, bytes */
    uint64_t current_interval;
  };

  /**
   * @enum Provider events
   */
  enum class ProviderEvent : uint64_t {
    /* Handle new deal proposal from a client */
    EvHandleProposal,

    /* Accept client's deal proposal */
    EvAcceptDeal,

    /* Reject client's deal proposal */
    EvRejectDeal,

    /* Internal error */
    EvInternalError,

    /* Send data/response to a client network error */
    EvNetworkError,

    /* Error reading next block of the requested Piece from a blockstore */
    EvReadBlockError,

    /* Request next payment from a client */
    EvRequestPayment,

    /* Handle client payment */
    EvHandleClientPayment,

    /* Continue sending blocks to a client after received payment */
    EvContinueDeal,

    /* Reading client's payment error */
    EvReadPaymentError,

    /* Saving payment voucher error */
    EvSaveVoucherError,

    /* All blocks of a requested Piece was sent */
    EvBlocksCompleted,

    /* Deal completed */
    EvCloseDeal
  };
}  // namespace fc::markets::retrieval::provider

#endif
