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
    @defgroup grp_bl_cmd Command Parser
    @ingroup grp_bl_downloader

    @addtogroup grp_bl_cmd
    @{
    @file bl_command.h
    @brief Command Parser APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_COMMAND_H
#define BL_COMMAND_H

#include <bl_inet.h>

/**
    @name Command Parser API IDs
    @{
    @brief API IDs for Command Parser
*/
#define BRCM_ARCH_BL_CMD_ID_TYPE            (0x00U) /**< @brief #BL_CmdIDType */
#define BRCM_ARCH_BL_CMD_STATUS_TYPE        (0x01U) /**< @brief #BL_CmdStatusType */
#define BRCM_ARCH_BL_CMD_MAGIC_NUM_MACRO    (0x02U) /**< @brief #BL_CMD_MAGIC_NUM */
#define BRCM_ARCH_BL_CMD_LISTEN_PORT_MACRO  (0x03U) /**< @brief #BL_CMD_LISTEN_PORT */
#define BRCM_ARCH_BL_CMD_PKT_TYPE           (0x04U) /**< @brief #BL_CmdPktType */
#define BRCM_ARCH_BL_CMD_INIT_PROC          (0x05U) /**< @brief #BL_CmdInit */
#define BRCM_ARCH_BL_CMD_STATE_HANDLER_PROC (0x06U) /**< @brief #BL_CmdStateHandler */
#define BRCM_ARCH_BL_CMD_SEND_STATUS_PROC   (0x07U) /**< @brief #BL_SendStatus */
#define BRCM_ARCH_BL_CMD_RECV_CB_PROC       (0x08U) /**< @brief #BL_CmdRecvCb */
/** @} */

/**
    @name BL_CmdIDType
    @{
    @brief Command ID type

    @trace #BRCM_ARCH_BL_CMD_ID_TYPE #BRCM_REQ_BL_CMD_ID
*/
typedef uint16_t BL_CmdIDType;                 /**< @brief typedef for BL command */
#define BL_CMD_ID_BL_STATUS        (0x0001U)   /**< @brief Bootloader status command */
#define BL_CMD_ID_ERASE_FLASH      (0x0002U)   /**< @brief Erase flash command */
/** @} */

/**
    @name BL_CmdStatusType
    @{
    @brief Command Status type

    @trace #BRCM_ARCH_BL_CMD_STATUS_TYPE #BRCM_REQ_BL_CMD_STATUS
*/
typedef uint16_t BL_CmdStatusType;             /**< @brief typedef for BL command status */
#define BL_STATUS_SUCCESS          (0x0001U)   /**< @brief Command status success */
#define BL_STATUS_INVALID_CMD      (0x0002U)   /**< @brief Invalid command */
#define BL_STATUS_UNKNOWN_ERR      (0x0003U)   /**< @brief Unknown error during processing */
#define BL_STATUS_BUSY             (0x0004U)   /**< @brief Bootloader busy */
/** @} */

/**
    @brief Command magic number: 'B''R''C''M'

    @trace #BRCM_ARCH_BL_CMD_MAGIC_NUM_MACRO #BRCM_REQ_BL_CMD_MAGIC_NUM
 */
#define BL_CMD_MAGIC_NUM           (0x4252434DU)

/**
    @brief Socket port for listening commands

    @trace #BRCM_ARCH_BL_CMD_LISTEN_PORT_MACRO #BRCM_REQ_BL_CMD_LISTEN_PORT
 */
#define BL_CMD_LISTEN_PORT      (58900U)

/**
    @brief command/response packet format

    @trace #BRCM_ARCH_BL_CMD_PKT_TYPE #BRCM_REQ_BL_CMD_PKT
*/
typedef struct {
    uint32_t        magic;      /**< @brief set to 'BRCM' */
    BL_CmdIDType       cmdID;   /**< @brief command ID */
    BL_CmdStatusType   status;  /**< @brief status */
    uint8_t         data[];     /**< @brief command data */
} BL_CmdPktType;

/** @brief Bootloader command initialization

    This API initializes Bootloader command processing component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CMD_INIT_PROC  #BRCM_REQ_BL_CMD_INIT

    @limitations None
*/
extern void BL_CmdInit(void);

/** @brief Bootloader command state handler

    This API is to handle Bootloader command state processing.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CMD_STATE_HANDLER_PROC  #BRCM_REQ_BL_CMD_STATE_HANDLER

    @limitations None
*/
extern void BL_CmdStateHandler(void);

/** @brief Bootloader send status

    This API sends the Status of the ongoing process to the client with IP aIP to port same as #BL_CMD_LISTEN_PORT

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIP         IP of the client to which status has to be sent
    @param[in]      aStatus     Status of the ongoing process
    @param[in]      aData       Pointer to data buffer
    @param[in]      aLen        Length of the buffer

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CMD_SEND_STATUS_PROC  #BRCM_REQ_BL_CMD_SEND_STATUS

    @limitations None
*/
extern void BL_SendStatus(BL_INET_IPAddress aIP,
                          BL_CmdStatusType aStatus,
                          uint8_t *const aData,
                          uint32_t aLen);

/** @brief This API shall be called from the IP stack when there is an UDP packet

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf        Pointer to data buffer
    @param[in]      aLen        Length of the data
    @param[in]      aSrcIPAddr  Source IP address
    @param[in]      aSrcPort    Source port

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_CMD_RECV_CB_PROC  #BRCM_REQ_BL_CMD_RECV_CB

    @limitations None
*/
extern void BL_CmdRecvCb(const uint8_t* const aBuf,
                         uint16_t aLen,
                         BL_INET_IPAddress aSrcIPAddr,
                         BL_INET_Port aSrcPort);
#endif /* BL_COMMAND_H */

/** @} */
