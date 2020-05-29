/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_ets_ipc IPC
    @ingroup grp_ets

    @addtogroup grp_ets_ipc
    @{
    @section sec_ets_ipc IPC

    @file ets_ipc.h
    @brief Ethernet TimeSync IPC interfaces
    This header file defines the ETS interface for IPC
    @version 0.1 Initial draft
*/
#ifndef ETS_IPC_H
#define ETS_IPC_H

#include <stdint.h>
#include "ets_osil.h"
#include "mcu_ext.h"

/**
    @name ETS IPC API IDs
    @{
    @brief API IDs for ETS IPC
 */
#define BRCM_SWARCH_ETS_CMDHANDLER_PROC         (0xD0U) /**< @brief #ETS_CmdHandler */
#define BRCM_SWARCH_ETS_GETCMDSTATUS_PROC       (0xD1U) /**< @brief #ETS_GetCmdStatus */
#define BRCM_SWARCH_ETS_MERGERESULT_PROC        (0xD2U) /**< @brief #ETS_MergeResult */
/** @} */

/** @brief Send commands to ETS

    This API queues the management commands to ETS

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Management command information
    @param[in]      aLen                Length of the management command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     pHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command posted successfully

    @post None

    @trace  #TBD

    @limitations This API may not be supported on all the chipsets
*/
int32_t ETS_CmdHandler(ETS_CmdType aCmdID,
                        BCM_CompIDType aCompID,
                        uint8_t *aCmd,
                        uint32_t aLen,
                        const uint32_t aClientMask,
                        const MSGQ_MsgHdrType** const pHdr,
                        uint32_t *aForwardToSlaves);

/** @brief Retreive status of management commands

    This API queries the status of completed management commands which were
    previously issued to ETS

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      pHdr        Pointer to the message queue header used to
                                retrieve the status
    @param[in]      aCompID     Component identifier
    @param[out]     aCmdID      Pointer to the command id to be filled by ETS
    @param[out]     aReply      Pointer to reply message to be filled by ETS
    @param[in]      aReplyLen   Pointer to length of reply message to be filled by ETS

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK     Command status retrieved successfully

    @post None

    @trace  #TBD

    @limitations This API may not be supported on all the chipsets
*/
int32_t ETS_GetCmdStatus(const MSGQ_MsgHdrType* const pHdr,
                         BCM_CompIDType aCompID,
                         ETS_CmdType* const aCmdID,
                         uint8_t *aReply,
                         uint32_t *aReplyLen);

/** @brief Merge results from master/slaves into one response to Host

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID          command id
    @param[in]      aCompID         component id
    @param[in]      aLen            length of response
    @param[out]     aOutResponse    Pointer to merged response (of aLen) to be filled by ETS
    @param[out]     aOutResult      Pointer to merged result (int32_t) to be filled by ETS
    @param[in]      aInResponse1    Pointer to first response message to be merged
    @param[in]      aInResult1      Result1 to be merged (of BCM Error types)
    @param[in]      aInResponse2    Pointer to second response message to be merged
    @param[in]      aInResult2      Result2 to be merged (of BCM Error types)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command status retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aOutResponse is NULL or
                                        aOutResult is NULL or
                                        aInResponse1 is NULL or
                                        aInResponse2 is NULL or
                                        aCompID is invalid

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t ETS_MergeResult (uint8_t aCmdID,
                         BCM_CompIDType aCompID,
                         uint32_t aLen,
                         uint8_t *aOutResponse,
                         int32_t *aOutResult,
                         const uint8_t *aInResponse1,
                         int32_t aInResult1,
                         const uint8_t *aInResponse2,
                         int32_t aInResult2);

#endif /* ETS_IPC_H */
/** @} */
