/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_rpc_cmd_queue_ifc RPC Command Queue
    @ingroup grp_comms_sw

    @addtogroup grp_rpc_cmd_queue_ifc
    @{

    @file rpc_cmd_queue.h
    @brief RPC Command Queue Interface

    @version 0.1 Initial version
*/

#ifndef RPC_CMD_QUEUE_H
#define RPC_CMD_QUEUE_H

#include <msg_queue.h>
#include <rpc_server.h>
#include <osil/imgl_osil.h>
/**
    @name Component API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_RPC_SEND_CMD_PROC          (0x20U)   /**< @brief #RPC_SendCmd      */
#define BRCM_SWARCH_RPC_GET_CMD_STATUS_PROC    (0x21U)   /**< @brief #RPC_GetCmdStatus */
/** @} */

/** @brief Send a command to RPC Command Queue

    This is an asynchronous API which sends a message to the RPC Command Queue
    (to RPC Server). Upon the completion, client is signaled.

    This queue is to be used by Master mode device to control slave devices in a stacking
    configuration

    Client shall call #RPC_CmdQGetCmdStatus API to get the status of the previously
    posted request.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]  aGrpID         Command group ID
    @param[in]  aCompID        Component ID
    @param[in]  aCmdID         Command ID
    @param[in]  aParams        Notification payload to be sent
    @param[in]  aSize          Size of notification payload in bytes
    @param[in]  aClientMask    Client event mask
    @param[out] aHdr           Message handle (Pointer to message header).
                               Shall be used by the client in #PTM_GetStatus
                               API to query the result of this request

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Request to process command is accepted and
                                        shall be processed asynchronously. Upon
                                        completion of the request, client shall
                                        be signaled using @c aClientMask
    @retval     #BCM_ERR_INVAL_PARAMS   @c aParams is NULL or
                                        @c aHdr is NULL

    @post TBD

    @limitations This API shall be called from the task context.
*/
extern int32_t RPC_CmdQSendCmd(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                                const uint8_t *const aParams, uint32_t aSize, uint32_t aClientMask,
                                const MSGQ_MsgHdrType** const aHdr);

/** @brief Get the status of previously posted command

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aHdr        Handle of the Message
    @param[out]     aResponse   Response return code for the command. This field is valid
                                only if this API returns #BCM_ERR_OK.
    @param[out]     aParams     Response data for the command. This is valid
                                only if this API returns #BCM_ERR_OK.
    @param[inout]   aSize       Size of the response loaded. This field is valid
                                only if this API returns #BCM_ERR_OK.

    @return     #BCM_ERR_OK             Command execution completed
    @return     #BCM_ERR_BUSY           Command is in progress
    @return     #BCM_ERR_UNKNOWN        Integration fault
    @return     #BCM_ERR_NOMEM          Insufficient memory

    @post TBD

    @limitations This API shall be called from the task context.
*/
extern int32_t RPC_CmdQGetCmdStatus(const MSGQ_MsgHdrType *const aHdr, int32_t * const aResponse,
                                   uint8_t *const aParams, uint32_t * const aSize);


#endif /* RPC_CMD_QUEUE_H */
/** @} */
