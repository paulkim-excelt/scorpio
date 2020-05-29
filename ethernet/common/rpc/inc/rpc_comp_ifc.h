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

/** @defgroup grp_rpc_comp_ifc RPC Component Interface
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_comp_ifc
    @{

    @file rpc_comp_ifc.h

    @brief Interface for redirecting commands to components

    @version 0.1 Initial draft
*/
#ifndef RPC_COMP_IFC_H
#define RPC_COMP_IFC_H

#include <stdint.h>
#include <msg_queue.h>

/**
    @name IPC System Commands API IDs
    @{
    @brief API IDs for IPS System Commands
 */
#define BRCM_SWARCH_RPC_SENDCMD_TYPE        (0xD0U) /**< @brief #RPC_SendCmdType    */
#define BRCM_SWARCH_RPC_CMDSTATUS_TYPE      (0xD1U) /**< @brief #RPC_CmdStatusType  */
#define BRCM_SWARCH_RPC_MERGERESULT_TYPE    (0xD2U) /**< @brief #RPC_MergeResultType*/
#define BRCM_SWARCH_RPC_COMP_TYPE           (0xD3U) /**< @brief #RPC_CompType       */
/** @} */

/** @brief Send commands to Component

    This API queues the commands to Component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID              Command ID value
    @param[in]      aCompID             Component ID value
    @param[in]      aCmd                Command information
    @param[in]      aLen                Length of the command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     aHdr                Pointer to the message queue header used to
                                        retrieve the status
    @param[out]     aForwardToSlaves    Is this message needed to be forwarded
                                        to slaves (if exist)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command posted successfully
    @retval     #BCM_ERR_NOMEM          Queue is Full and cannot be added to queue
    @retval     #BCM_ERR_INVAL_PARAMS   aCmd is NULL or
                                        aHdr is NULL or
                                        aForwardToSlaves is NULL or
                                        aClientMask is zero or
                                        aCompID does not belong to this group

    @post None

    @trace  #BRCM_SWREQ_RPC_COMPONENT_INTERFACE

    @limitations This API may not be supported on all the chipsets
*/
typedef int32_t (*RPC_SendCmdType)(uint8_t aCmdID,
                                   BCM_CompIDType aCompID,
                                   uint8_t *aCmd,
                                   uint32_t aLen,
                                   const uint32_t aClientMask,
                                   const MSGQ_MsgHdrType** const aHdr,
                                   uint32_t *aForwardToSlaves);

/** @brief Retreive status of queued commands

    This API queries the status of completed commands which were previously issued to
    component via RPC_SendCmdType

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aHdr        Pointer to the message queue header used to retrieve the status
    @param[in]     aCompID     Pointer to the component id to be filled by Component
    @param[out]     aCmdID      Pointer to the command id to be filled by Component
    @param[out]     aReply      Pointer to reply message to be filled by Component
    @param[out]     aReplyLen   Pointer to length of reply message to be filled by Component

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_BUSY       Command is still being processed
    @retval     #BCM_ERR_INVAL_PARAMS   aHdr is NULL or
                                        aCmdID is NULL or
                                        aCompID is NULL or
                                        aReply is NULL or
                                        aReplyLen is NULL

    @post None

    @trace  #BRCM_SWREQ_RPC_COMPONENT_INTERFACE

    @limitations This API may not be supported on all the chipsets
*/
typedef int32_t (*RPC_CmdStatusType)(const MSGQ_MsgHdrType* const aHdr,
                                     BCM_CompIDType aCompID,
                                     uint8_t* const aCmdID,
                                     uint8_t *aReply,
                                     uint32_t *aReplyLen);

/** @brief Merge results from master/slaves into one response to Host

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmdID          command id
    @param[in]      aCompID         component id
    @param[in]      aLen            length of response
    @param[out]     aOutResponse    Pointer to merged response (of aLen) to be filled by Component
    @param[out]     aOutResult      Pointer to merged result (int32_t) to be filled by Component
    @param[in]      aInResponse1    Pointer to first response message to be merged
    @param[in]      aInResult1      Result1 to be merged (of BCM Error types)
    @param[in]      aInResponse2    Pointer to second response message to be merged
    @param[in]      aInResult2      Result2 to be merged (of BCM Error types)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Command status retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aOutResponse is NULL or
                                        aOutResult is NULL or
                                        aInResponse1 is NULL or
                                        aInResponse2 is NULL or
                                        aCompID is invalid

    @post None

    @trace  #BRCM_SWREQ_RPC_COMPONENT_INTERFACE

    @limitations This API may not be supported on all the chipsets
*/
typedef int32_t (*RPC_MergeResultType)(uint8_t aCmdID, BCM_CompIDType aCompID, uint32_t aLen,
                                         uint8_t *aOutResponse,
                                         int32_t *aOutResult,
                                         const uint8_t *aInResponse1,
                                         int32_t aInResult1,
                                         const uint8_t *aInResponse2,
                                         int32_t aInResult2);

/**
    @brief Structure containing component interface functions

    Structure shall be placed in const region, for safety reasons

    @trace #BRCM_SWREQ_RPC_COMPONENT_INTERFACE
 */

typedef struct _RPC_CompType {
    RPC_SendCmdType     sendCmd;        /**< @brief Send command to component           */
    RPC_CmdStatusType   getStatus;      /**< @brief Get status of command from component*/
    RPC_MergeResultType mergeResult;    /**< @brief Merge results of one command from
                                                    multiple devices (master/slaves)    */
} RPC_CompType;

#endif /* RPC_COMP_IFC_H */
/** @} */
