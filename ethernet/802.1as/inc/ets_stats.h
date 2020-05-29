/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
  @addtogroup grp_ets_ifc
  @{
  @file ets_stats.h
  @brief Statistics maintained by ETS subsystem

  This file provides the definition of statistics of ETS which can be retrieved
  using the ETS_GetStats() API. The statistics do not clear on read. They can
  be reset using the ETS_ResetStats() API.

  @version 1.25 Imported from MS Word document
*/

#ifndef ETS_STATS_H
#define ETS_STATS_H

/**
    @name ETS Library API IDs
    @{
    @brief API IDs for ETS Library
 */
#define BRCM_SWARCH_ETS_STATSMAGIC_MACRO    (0x10U)  /**< @brief #ETS_STATS_MAGIC_ID */
#define BRCM_SWARCH_ETS_STATS_TYPE          (0x11U) /**< @brief #ETS_StatsType */
/** @} */

/**
   @brief Statistics structure magic

   Magic number for the statistics structure containing 'ETSS'
*/
#define ETS_STATS_MAGIC_ID  (0x53535445UL)

/** @struct  ETS_StatsType
    @brief Statistics maintained by ETS
*/
typedef struct _ETS_StatsType {
    uint32_t magicId;                     /**< @brief Magic word 'ETSS'
                                               (checked for validity) */

    uint32_t syncTxCount;                 /**< @brief Number of Sync packets
                                               transmitted */

    uint32_t syncRxCount;                 /**< @brief Number of Sync packets
                                               received */

    uint32_t syncTransmitTimeouts;        /**< @brief Number of Sync packets
                                               transmitted as proxy */

    uint32_t syncRxDiscards;              /**< @brief Number of received Sync
                                               packets discarded */

    uint32_t followUpTxCount;             /**< @brief Number of Follow Up
                                               packets transmitted */

    uint32_t followUpRxCount;             /**< @brief Number of Follow Up
                                               packets received */

    uint32_t followUpRxDiscards;          /**< @brief Number of received Follow
                                               Up packets discarded */

    uint32_t signalingTxCount;            /**< @brief Number of Signaling
                                               packets transmitted */

    uint32_t signalingRxCount;            /**< @brief Number of Signaling
                                               packets received */

    uint32_t signalingRxDiscards;         /**< @brief Number of received
                                               Signaling packets discarded */

    uint32_t pDelayReqTxCount;            /**< @brief Number of PDelay Request
                                               packets transmitted */

    uint32_t pDelayReqRxCount;            /**< @brief Number of PDelay Request
                                               packets received */

    uint32_t pDelayReqRxDiscards;         /**< @brief Number of received PDelay
                                               Request packets discarded */

    uint32_t pDelayRespTxCount;           /**< @brief Number of PDelay Response
                                               packets transmitted */

    uint32_t pDelayRespRxCount;           /**< @brief Number of PDelay Response
                                               packets received */

    uint32_t pDelayRespRxDiscards;        /**< @brief Number of received PDelay
                                               Response packets discarded */

    uint32_t pDelayRespFollowUpTxCount;   /**< @brief Number of PDelay Follow Up
                                               Response packets transmitted */

    uint32_t pDelayRespFollowUpRxCount;   /**< @brief Number of PDelay Follow Up
                                               Response packets received */

    uint32_t syncReceiptTimeouts;         /**< @brief Number of instances where
                                               Sync packets were not received
                                               for N (syncReceiptTimeout)
                                               consecutive intervals */

    uint32_t followupReceiptTimeouts;     /**< @brief Number of Follow Up
                                               packets not received within 1
                                               Sync interval */

    uint32_t pDelayReceiptTimeouts;       /**< @brief Time-outs in Reception of
                                               PDelay Response or Pdelay
                                               Response Follow Up packets */

    uint32_t badPdelayValues;             /**< @brief Invalid PDelay values,
                                               say negative */

    uint32_t txErrors;                    /**< @brief Number of packets not
                                               transmitted due to errors */

    uint32_t tsErrors;                    /**< @brief Number of errors in
                                               retrieving hardware timestamps */

    uint32_t ptpDiscardCount;             /**< @brief Sum of
                                               pDelayReceiptTimeouts &
                                               followupReceiptTimeouts */

    uint32_t parseFailed;                 /**< @brief Number of invalid packets
                                               received */

    uint32_t txConf;                      /**< @brief Number of transmit
                                               confirmations received */

    uint32_t pDelayLostResponsesExceeded; /**< @brief Number of Pdelay Responses
                                               lost */
} ETS_StatsType;

#endif /* ETS_STATS_H */
/** @} */
