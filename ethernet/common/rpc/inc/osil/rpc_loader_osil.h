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

/** @defgroup grp_rpc_loader_il RPC Loader Integration Guide
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_loader_il
    @{

    @file rpc_loader_osil.h
    @brief RPC Loader Integration Interfaces

    @version 0.1 Initial version
*/

#ifndef RPC_LOADER_OSIL_H
#define RPC_LOADER_OSIL_H

#include <ipc.h>
#include <rpc_loader.h>


/**
    @name RPC Loader Integration Interface IDs
    @{
    @brief Integration Interface IDs for RPC Loader
*/

#define BRCM_SWARCH_PTM_IL_MSG_TYPE                     (0x45U) /**< @brief #RPC_LoaderMsgType */
#define BRCM_SWARCH_PTM_IL_SEND_MSG_PROC                (0x4DU) /**< @brief #PTM_SendMsg */
#define BRCM_SWARCH_PTM_IL_RECV_MSG_PROC                (0x4EU) /**< @brief #PTM_RecvMsg */
#define BRCM_SWARCH_PTM_IL_PROCESS_MSG_PROC             (0x4FU) /**< @brief #PTM_ProcessMsg */
#define BRCM_SWARCH_PTM_IL_GET_MSG_PROC                 (0x50U) /**< @brief #PTM_GetMsg */
#define BRCM_SWARCH_PTM_IL_GET_STATE_PROC               (0x51U) /**< @brief #PTM_GetState */
#define BRCM_SWARCH_PTM_IL_EVENT_HANDLER_PROC           (0x52U) /**< @brief #PTM_EventHandler */
/** @} */

/**
    @brief RPC Loader message structure
*/
typedef struct _RPC_LoaderRpcMsgType {
    uint32_t            cmd;        /**< @brief command ID            */
    const uint8_t*      payLoad;    /**< @brief Ponter to the pay load*/
    uint16_t            payLoadSize;/**< @brief Payload size          */
    RPC_ChannIDType     id;         /**< @brief RPC Channel ID        */
} RPC_LoaderRpcMsgType;


/** @brief  RPC Loader Init

    Initializes RPC Loader State

    @behavior Sync, Non-reentrant

    @pre None

    @param          void

    @return     void

    @post RPC Loader is initialized

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_IpcLoaderInit(void);

/** @brief Handle Incoming RPC message on Master mode channel

    This API handles incoming RPC messages for IMGL group.
    Here in this case, Slave would send us a response.
    This API shall be called by RPC_Server when there is a new message recevied
    over RPC channel

    @behavior Sync, Non-reentrant

    @pre RPC Loader is initialized

    @param      void

    @return     void

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_IpcLoaderMasterModeMsgHandler(RPC_LoaderRpcMsgType *aMsg);

/** @brief Handle Incoming RPC message on Slave mode channel

    This API handles incoming RPC messages for IMGL group on slave mode channel.
    Here in this case, Host would have sent us a command.
    This API shall be called by RPC_Server when there is a new message recevied
    over RPC channel

    @behavior Sync, Non-reentrant

    @pre RPC Loader is initialized

    @param      void

    @return     void

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_IpcLoaderSlaveModeMsgHandler(RPC_LoaderRpcMsgType *aMsg);

/** @brief Handle RPC Loader events

    This API handles RPC_LOADER_EVENT. This API shall be called by RPC_Server
    when RPC_LOADER_EVENT is received

    @behavior Sync, Non-reentrant

    @pre RPC Loader is initialized

    @param      void

    @return     void

    @post None

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_IpcLoaderEventHandler(void);

/** @brief Deinitialize RPC Loader

    This API deinitializes RPC Loader

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    @return     void

    @post RPC Loader is deinitialized

    @note This interface shall be used by the integrator.

    @limitations None
*/
extern void RPC_IpcLoaderDeInit(void);

#endif  /* RPC_LOADER_OSIL_H */
/** @} */
