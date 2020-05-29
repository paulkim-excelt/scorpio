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
    @defgroup grp_host Host

    @defgroup grp_host_ipc Host IPC
    @ingroup grp_host

    @addtogroup grp_host_ipc
    @{

    @file hipc.h
    @brief Host IPC API
    This header file contains the host interface functions for IPC

    @version 1.0 Initial Version
*/

#ifndef HIPC_H
#define HIPC_H

#include <stdint.h>
#include <endian.h>
#include <byteswap.h>
#include <host_system.h>
#include <sys_ipc_cmds.h>

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define uswap16(x)              bswap_16(x)
#define uswap32(x)              bswap_32(x)
#define uswap64(x)              bswap_64(x)
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define uswap16(x)              (x)
#define uswap32(x)              (x)
#define uswap64(x)              (x)
#else
#error "__BYTE_ORDER__ has to be either __ORDER_BIG_ENDIAN__ or __ORDER_LITTLE_ENDIAN__"
#endif

/**
    @name Install
    @{
    @brief Install type
*/
typedef uint32_t HIPC_InstallType; /**< @brief Install type */
#define HIPC_INSTALL_FACTORY    (0x00000001UL) /**< @brief Factory installation */
#define HIPC_INSTALL_OTA        (0x00000002UL) /**< @brief Over-The-Air installation */
/** @} */

/** @brief Send IPC message

    This API is used to send a message through IPC

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      cmd             Command ID of message
    @param[in]      msg             Pointer to message payload
    @param[in]      len             Actual size of msg copied by the API

    @return     #BCM_ERR_OK             IPC message sent successfully
    @return     #BCM_ERR_NOSUPPORT      IPC message's length is not supported
    @return     #BCM_ERR_UNINIT         IPC mechanism is not yet initialised
    @return     #BCM_ERR_NODEV          Target is undergoing a reboot
    @return     #BCM_ERR_BUSY           Target is not ready to accept anymore messages
    @return     #BCM_ERR_NOMEM          All available IPC buffers are used-up

    @post None

    @limitations None
*/
extern int32_t HIPC_Send(uint32_t cmd, uint8_t *msg, uint32_t len);

/** @brief Receive IPC message

    This API is used to receive an incoming message through IPC

    @behavior Sync, Non Re-entrant

    @pre None

    @param[out]     cmd             Pointer to store the command ID of message
    @param[out]     msg             Pointer to copy the message payload
    @param[in]      len_max         Maximum size of the msg
    @param[out]     len             Actual size of msg copied

    @return     #BCM_ERR_OK             IPC message received successfully
    @return     #BCM_ERR_DATA_INTEG     IPC message's data integrity failed
    @return     #BCM_ERR_NOMEM          IPC message's length is incorrect
    @return     #BCM_ERR_NOT_FOUND      No IPC message available
    @return     #BCM_ERR_UNINIT         IPC mechanism is not yet initialised
    @return     #BCM_ERR_UNKNOWN        Memory access failure

    @post None

    @limitations None
*/
extern int32_t HIPC_Recv(uint32_t *cmd, uint8_t *msg, uint32_t len_max, uint32_t *len);

/** @brief Send a command and receive its response

    This API is used to send a command to target and receive a response to the
    same command

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      id              Command ID of message
    @param[in]      cmd             Pointer to command message payload
    @param[in]      cmd_len         Size of command message payload
    @param[out]     reply           Pointer to copy response payload
    @param[in]      reply_len_max   Maximum size of reponse payload
    @param[out]     reply_len_act   Actual size of reponse payload copied

    @return     #BCM_ERR_OK             IPC message received successfully
    @return     #BCM_ERR_DATA_INTEG     IPC message's data integrity failed
    @return     #BCM_ERR_NOMEM          IPC message's length is incorrect
    @return     #BCM_ERR_NOT_FOUND      No IPC message available
    @return     #BCM_ERR_UNINIT         IPC mechanism is not yet initialised
    @return     #BCM_ERR_UNKNOWN        Memory access failure

    @post None

    @limitations None
*/
extern int32_t HIPC_SendRecv(uint32_t id, uint8_t *cmd, uint32_t cmd_len,
            uint8_t *reply, uint32_t reply_len_max, uint32_t *reply_len_act);

/** @brief Initialize Host IPC

    This API is used to initialise Host IPC

    @behavior Sync, Non Re-entrant

    @pre None

    @param  None

    @return     #BCM_ERR_OK         Success
    @return     others              As reported by Peripheral drivers such as
                                    SPI, GPIO, PCIe etc.

    @post None

    @limitations None
*/
extern int32_t HIPC_Init();

/** @brief Deinitialize Host IPC

    This API is used to deinitialise Host IPC

    @behavior Sync, Non Re-entrant

    @pre None

    @param  None

    @return None

    @post None

    @limitations None
*/
extern void HIPC_Deinit(void);

/** @brief Queue an asynchronous message

    This API is used to queue asynchronous message to the server. This is a
    call-back to be implemented by server.

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      currentSlave    Slave ID
    @param[in]      replyId         Reply ID of the message
    @param[in]      reply           Pointer to message payload
    @param[in]      replyLen        Size of the message payload

    @return     #BCM_ERR_OK             Message queued successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid replyId
    @return     #BCM_ERR_INVAL_PARAMS   reply is NULL

    @post None

    @limitations None
*/
extern int32_t IPC_QueueAsyncMsg(uint32_t currentSlave, uint32_t replyId, uint8_t *reply, uint32_t replyLen);

/** @brief Get Active Slaves

    This API is used to get all active slave IDs

    @behavior Sync, Non Re-entrant

    @pre None

    @param[out]     id          Pointer to an array of IDs to be filled
    @param[inout]   count       Input: Maximum number of IDs that can be filled
                                Output: Actual number of valid IDs filled in the array

    @return     #BCM_ERR_OK             Active slave IDs fetched successfully
    @return     #BCM_ERR_INVAL_PARAMS   id is NULL
    @return     #BCM_ERR_INVAL_PARAMS   count is NULL

    @post None

    @limitations None
*/
extern int32_t HIPC_GetActiveSlaves(uint32_t *id, uint32_t *count);

/** @brief Is stacked

    This API is used to identify is the board is a stacked board

    @behavior Sync, Non Re-entrant

    @pre None

    @param      None

    @return     #TRUE       Is a stacked board
    @return     #FALSE      Is not a stacked board

    @post None

    @limitations None
*/
extern int32_t HIPC_IsStacked(void);

/** @brief Set Slave ID

    This API is used to set the current active slave ID

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      id          ID of slave

    @return     #BCM_ERR_OK             ID set successfully
    @return     #BCM_ERR_INVAL_PARAMS   Invalid id value

    @post None

    @limitations None
*/
extern int32_t HIPC_SetSlave(uint32_t id);

/** @brief Get Slave ID

    This API is used to get the current active slave ID

    @behavior Sync, Non Re-entrant

    @pre None

    @param[out]     id          ID of slave

    @return     #BCM_ERR_OK             ID fetch successful
    @return     #BCM_ERR_INVAL_PARAMS   Invalid id pointer

    @post None

    @limitations None
*/
extern int32_t HIPC_GetSlave(uint32_t *id);

/** @brief Set the bus connection mode

    This API sets the connection mode of the slave

    @behavior Sync, Non Re-entrant

    @pre None

    @param[out]     mode        Mode of the slave bus

    @return     #BCM_ERR_OK             set was successful
    @return     #BCM_ERR_INVAL_PARAMS   Invalid id pointer

    @post None

    @limitations None
*/
extern int32_t HIPC_SetConnMode(mgmt_connection_t mode);

/** @brief Get the bus connection mode

    This API gets the connection mode of the slave

    @behavior Sync, Non Re-entrant

    @pre None

    @param      None

    @return     mgmt_connection_t   Current connection mode

    @post None

    @limitations None
*/
extern mgmt_connection_t HIPC_GetConnMode(void);

/** @brief Probe Slaves

    This API is used to probe active slave devices connected to the host

    @behavior Sync, Non Re-entrant

    @pre None

    @param None

    @return None

    @post None

    @limitations None
*/
extern void HIPC_ProbeSlaves();

/** @brief Ping target

    This API is used to ping target and get it's mode and features

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_INVAL_PARAMS   Invalid info pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid mode pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid targetFeatures pointer
    @return     #BCM_ERR_INVAL_PARAMS   Invalid config
    @return     #BCM_ERR_UNKNOWN        Error reported by target

    @post None

    @limitations None
*/
extern int32_t HIPC_Ping(MgmtInfoType *info, uint32_t *const mode, uint32_t * const targetFeatures);

/** @brief Reboot target chip

    This API is used to reboot target chip. This would reset the CPU, memories,
    peripherals and spare registers.

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle

    @return     #BCM_ERR_OK         Success
    @return     #BCM_ERR_UNKNOWN    Target Memory access errors
    @return     others              Errors reported by HIPC_SendRecv

    @post None

    @limitations None
*/
extern int32_t HIPC_Reboot(MgmtInfoType *info);

/** @brief Enter Update mode

    This API is used to enter update mode

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mode            Mode of installation

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   info is NULL
    @return     #BCM_ERR_NOSUPPORT      Target couldn't enter update mode after reset
    @return     #BCM_ERR_NOSUPPORT      Target bootmode doesn't support entering update mode
    @return     #BCM_ERR_UNKNOWN        Target Memory access errors
    @return     others                  Errors reported by HIPC_SendRecv

    @post None

    @limitations None
*/
extern int32_t HIPC_EnterUpdateMode(MgmtInfoType *info, HIPC_InstallType mode);

/** @brief Execute on target

    This API is used to execute a specific binary on target

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      info            Pointer to #MgmtInfoType handle
    @param[in]      mode            Mode of installation
    @param[in]      image           Pointer to binary
    @param[in]      len             Size of the image

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   Size is 0
    @return     #BCM_ERR_INVAL_PARAMS   image is NULL
    @return     #BCM_ERR_INVAL_PARAMS   info is NULL
    @return     #BCM_ERR_UNKNOWN        Target Memory access errors
    @return     others                  Errors reported by HIPC_SendRecv

    @post None

    @limitations None
*/
extern int32_t HIPC_ExecuteOnTarget(MgmtInfoType *info, HIPC_InstallType mode, const uint8_t* image, uint32_t len);

/** @brief Generate Frame-Sync Pulse

    This API is used to generate a frame sync pulse to target

    @behavior Sync, Non Re-entrant

    @pre None

    @param none

    @return none

    @post None

    @limitations None
*/
extern void HIPC_GenerateFrameSyncPulse(void);

#endif /* HIPC_H */

/** @} */

