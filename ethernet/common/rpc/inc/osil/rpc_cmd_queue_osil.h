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

/** @defgroup grp_rpc_cmd_queue_il RPC Cmd Queue Integration Guide
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_cmd_queue_il
    @{

    @file rpc_cmd_queue_osil.h
    @brief RPC Command Queue Integration Interfaces

    @version 0.1 Initial version
*/

#ifndef RPC_CMD_QUEUE_OSIL_H
#define RPC_CMD_QUEUE_OSIL_H

#include <ipc.h>
#include <rpc_cmd_queue.h>


/**
    @name RPC Cmd Queue Integration Interface IDs
    @{
    @brief Integration Interface IDs for RPC Cmd Queue
*/
#define BRCM_SWARCH_RPC_CMD_QUEUE_MSG_TYPE                     (0x45U) /**< @brief #RPC_LoaderMsgType */
/** @} */

/**
    @name RPC Cmd Queue Channel Type
    @{
    @brief RPC Cmd Queue Channel Type
*/
typedef uint32_t RPC_ChannelType;
#define RPC_CHANNEL_LOCAL          (0UL)
#define RPC_CHANNEL_REMOTE         (1UL)
#define RPC_CHANNEL_MAX            (2UL)
/** @} */


/**
    @brief Channel data

    Port number for UDP/TCP ports
    Channel number for RPC
*/
typedef uint32_t RPC_ChannelIdType;

/**
    @brief RPC Command Queue Message Structure
*/
typedef struct _RPC_CmdQRpcMsgType {
    RPC_ChannelType     channel;    /**< @brief channel typedef       */
    RPC_ChannelIdType   id;         /**< @brief channel data          */
    uint32_t            cmd;        /**< @brief command ID            */
    int32_t             error;      /**< @brief error code, valid only
                                                for response          */
    const uint8_t*      payLoad;    /**< @brief Ponter to the pay load*/
    uint16_t            payLoadSize;/**< @brief Payload size          */
} RPC_CmdQRpcMsgType;

/** @brief  RPC Command Queue Init

    Initializes RPC Command Queue State

    @behavior Sync, Non-reentrant

    @pre None

    @param          void

    @return     void

    @post RPC Command Queue is initialized

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_CmdQInit(void);

/** @brief Queue a command received

    This API queues a command recived into RPC CmdQ.
    This API shall be called by RPC_Server when there is a new message recevied
    over RPC channel in MASTER mode (except for IMGL group, that is handled by
    RPC Loader)

    @behavior Sync, Non-reentrant

    @pre RPC Loader is initialized

    @param[in]      aMsg    Message containing command details

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command added to Queue and will be processed
    @retval     #BCM_ERR_NOMEM          Queue is full and could not be pushed to Queue
                                        Or Message payload is too large
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsg is NULL or
                                        aMsg->channel is invalid

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern int32_t RPC_CmdQAddCmd(RPC_CmdQRpcMsgType *aMsg);

/** @brief Handle Incoming RPC responses

    This API handles incoming RPC responses from slave devices.
    This API shall be called by RPC_Server when there is a new message recevied
    over RPC channel in MASTER mode (except for IMGL group, that is handled by
    RPC Loader)

    @behavior Sync, Non-reentrant

    @pre RPC Loader is initialized

    @param[in]      aMsg    Message containing response details

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Response processed
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsg is NULL or
                                        aMsg->channel is invalid

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_CmdQAddResponse(RPC_CmdQRpcMsgType *aMsg);

/** @brief Handle RPC Command Queue events

    This API handles RPC_CMDQ_EVENT. This API shall be called by RPC_Server
    when RPC_CMDQ_EVENT is received

    @behavior Sync, Non-reentrant

    @pre RPC Command Queue is initialized

    @param      void

    @return     void

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_CmdQEventHandler(void);

/** @brief Deinitialize RPC Command Queue

    This API deinitializes RPC Command Queue

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @return     void

    @post RPC Loader is deinitialized

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_CmdQDeInit(void);

#endif  /* RPC_CMD_QUEUE_OSIL_H */
/** @} */
