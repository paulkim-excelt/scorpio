/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

 2. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_rpc_server_async Async Event Interface
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_server_async
    @{

    @file rpc_async.h
    @brief RPC Async Event Notification Interface

    @version 0.1 Initial Version
*/

#ifndef RPC_ASYNC_H
#define RPC_ASYNC_H

#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>
#include <rpc_server.h>
#include <svc.h>

/**
    @name RPC Server Async Notification Architecture IDs
    @{
    @brief Architecture IDs for RPC Server Async Notifications

*/
#define BRCM_SWARCH_RPC_ASYNC_NOTIFY_PROC    (0x10U)   /**< @brief #RPC_AsyncEvent    */
#define BRCM_SWARCH_RPC_ASYNC_RESPONSE_PROC  (0x11U)   /**< @brief #RPC_AsyncResponse */
/** @} */


/** @brief Function to notify events to host/remote party

    Function to be called by independent modules when there is a
    ASYNC event to be notified to client/host/application.

    @behavior Sync, Re-entrant

    @pre RPC Server should be initialized

    @param[in]  aGrpID         Command group ID
    @param[in]  aCompID        Component ID
    @param[in]  aCmdID         Command ID
    @param[in]  aParams        Notification payload to be sent
    @param[in]  aSize          Size of notification payload in bytes

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Notification sent successfully

    @post None

    @trace  #BRCM_SWREQ_RPC_SERVER_ASYNC

    @limitations None
*/
extern int32_t RPC_AsyncEvent(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                              const uint8_t *const aParams, uint32_t aSize);

/** @brief Function to notify a asynchronous response to host

    Function to be called by System task when boot up sequence is complete
    in Flash less mode (Host has given an execute command)

    @behavior Sync, Re-entrant

    @pre RPC Server should be initialized

    @param[in]  aGrpID         Command group ID
    @param[in]  aCompID        Component ID
    @param[in]  aCmdID         Command ID
    @param[in]  aParams        Notification payload to be sent
    @param[in]  aSize          Size of notification payload in bytes

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Notification sent successfully

    @post None

    @trace  #BRCM_SWREQ_RPC_SERVER_ASYNC

    @limitations This should be used only by System Task to notify bootup complete
                 while executing in Flashless mode
*/
extern int32_t RPC_AsyncResponse(BCM_GroupIDType aGrpID, BCM_CompIDType aCompID, uint8_t aCmdID,
                              const uint8_t *const aParams, uint32_t aSize);

#endif /* RPC_ASYNC_H */

/** @} */
