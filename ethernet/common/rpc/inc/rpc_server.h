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
    @defgroup grp_rpc_server_ifc RPC_IPC Server Interface
    @ingroup grp_rpc_server

    @addtogroup grp_rpc_server_ifc
    @{


    @file rpc_server.h
    @brief RPC_IPC Server Application Interface

    @version 0.1 Initial version
*/

#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#define RPC_MAX_CHANNELS IPC_MAX_CHANNELS	/* RPC Max Channels */

/**
 * @brief IPC channel mode type
 *
 * Each IPC channel either works in master or slave mode
 * if Channel is configured in master mode, it will be
 * using the bus interface to read/write to IPC registers
 *
 */
typedef uint8_t RPC_ChannModeType;
#define RPC_CHANN_MODE_SLAVE    (1UL)   /**< @brief RPC channel is in slave mode */
#define RPC_CHANN_MODE_MASTER   (2UL)   /**< @brief RPC channel is in mastre mode */

/**
 * @brief IPC channel ID type
 */
typedef uint8_t RPC_ChannIDType;
#define RPC_CHANNID_INVALID   (0xFFUL)

/** @brief Send RPC Command to SysCommand

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Channel ID
    @param[in]      aCmd	    Channel Command
    @param[in]      aMsg        Message
    @param[in]      aLen        Lengtha of the Message

    @return     #BCM_ERR_OK             Command execution completed
    @return     #BCM_ERR_BUSY           Command is in progress
    @return     #BCM_ERR_UNKNOWN        Integration fault
    @return     #BCM_ERR_NOMEM          Insufficient memory

    @post TBD

    @limitations TBD
*/

extern int32_t RPC_Send(uint8_t aID, uint32_t aCmd, uint8_t *aMsg, uint32_t aLen);

/** @brief Send RPC Get Channel Mode

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMode        Mode

    @return     #BCM_ERR_OK             Command execution completed
    @return     #BCM_ERR_BUSY           Command is in progress

    @post TBD

    @limitations TBD
*/

extern int32_t RPC_GetChannelMode(RPC_ChannModeType aMode);

#endif /* RPC_SERVER_H */
/** @} */
