/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

/** @defgroup grp_msgq_il Integration Guide
    @ingroup grp_msgq

    @addtogroup grp_msgq_il
    @{
    @section sec_msgq_integration Integration Guide
    This section describes how to integrate message queue to a system.

    @section sec_msgq_il_seq Sequence Diagram
    @subsection subsec_msgq_il_seq_with_notif SVC Layer (with notifications)
    @image html msgq_seq_il_diagram_with_notif.jpg "SVC Layer and Client-Server communication (with notification)"

    @subsection subsec_msg_il_seq_without notif SVC Layer (without notifications)
    @image html msgq_seq_il_diagram_without_notif.jpg "SVC Layer and Client-Server communication (without notification)"

    @file msg_queue_osil.h
    @brief Message Queue Integration Interfaces
    This header file contains the integration interfaces for Message Queue
    @version 1.10 Imported from docx
*/

#ifndef MSG_QUEUE_OSIL_H
#define MSG_QUEUE_OSIL_H

#include <system.h>
#include <msg_queue.h>

/**
    @name Message Queue Integration Interface IDs
    @{
    @brief Integration Interface IDs for Message Queue
*/
#define BRCM_SWARCH_MSGQ_CMD_TYPE              (0x40U) /**< @brief #MSGQ_CMD_SEND_MSG  */
#define BRCM_SWARCH_MSGQ_IO_TYPE               (0x41U) /**< @brief #MSGQ_IOType        */
#define BRCM_SWARCH_MSGQ_SYS_CMD_REQ_PROC      (0x42U) /**< @brief #MSGQ_SysCmdReq     */
#define BRCM_SWARCH_MSGQ_INT_CMD_HANDLER_PROC  (0x43U) /**< @brief #MSGQ_IntCmdHandler */
#define BRCM_SWARCH_MSGQ_CMD_HANDLER_PROC      (0x44U) /**< @brief #MSGQ_CmdHandler    */
/** @} */

/**
    @name Message Queue Commands
    @{
    @brief Message Queue Commands

    @trace #BRCM_SWREQ_MSGQ_KERNEL_INTERFACE
*/
#define MSGQ_CMD_SEND_MSG         (0UL)   /**< MSGQ send message command        */
#define MSGQ_CMD_RECV_MSG         (1UL)   /**< MSGQ receive message command     */
#define MSGQ_CMD_TRY_CANCEL_MSG   (2UL)   /**< MSGQ try cancel message command  */
#define MSGQ_CMD_VALIDATE_SERVER  (3UL)   /**< MSGQ validate server command     */
#define MSGQ_CMD_COMPLETE_MSG_IDX (4UL)   /**< MSGQ complete message command    */
/** @} */

/**
    @brief Structure for MSGQ SVC Interface

    @trace #BRCM_SWREQ_MSGQ_KERNEL_INTERFACE
*/
typedef struct _MSGQ_IOType {
    int32_t retVal;             /**< @brief Return value of the Message Queue
                                 APIs */
    uint32_t clientMask;        /**< @brief Client Mask to signal upon completion of
                                 message */
    const MSGQ_Type* msgQ;      /**< @brief Pointer to #MsgQ structure*/
    uint32_t idx;               /**< @brief Message Index */
    const MSGQ_MsgHdrType* hdr; /**< @brief Pointer to #MsgHdr structure*/
    const void* msgIn;          /**< @brief Pointer to input message */
    void* msgOut;               /**< @brief Pointer to output message */
    int32_t signalID;           /**< @brief Task ID to be signalled upon success */
    uint32_t signalMask;        /**< @brief Mask to signal signalID upon success */
} MSGQ_IOType;

/** @brief Raise System Command Request

    Interface to raise System Command Request. Upon successfully posting the
    System Command Request, this API shall signal the given signalID and with
    the given signalMask (both part of @a aMsgQIO)

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aMsgQIO     In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request was raised successfully
    @retval     Others                  Error values specific to the command
                                        requested
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        aMsgQIO is NULL

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations None
*/
int32_t MSGQ_SysCmdReq(const uint32_t aCmd, MSGQ_IOType* const aMsgQIO);

/** @brief Internal Message Queue command handler

    This is an internal Message Queue interface that handles Message Queue
    system command requests.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        Command to be processed as received by
                                #MsgQ_CmdHandler.
    @param[in]      aTaskID     ID of task that requested the command
    @param[inout]   aMsgQIO     In-Out structure containing request parameters

    @return     void

    @post TBD

    @note This interface shall be called from #MsgQ_CmdHandler.

    @trace  #BRCM_SWREQ_MSGQ_KERNEL_HANDLER

    @limitations None
*/
void MSGQ_IntCmdHandler(uint32_t aCmd, int32_t aTaskID,
                        MSGQ_IOType* const aMsgQIO);

/** @brief Handle Message Queue commands

    This interface handles all the Message Queue commands requested from
    #SysCmdReq.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre TBD

    @param[in]      aMagic      Message Queue Magic Number
                                (#BCM_MSG_SVC_MAGIC_ID)
    @param[in]      aCmd        Command requested from #SysCmdReq
    @param[inout]   aMsgQSysIO  uint8_t * to be interpreted as #MsgQIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install the same to SVC layer.

    @trace  #BRCM_SWREQ_MSGQ_KERNEL_HANDLER

    @limitations None
*/
void MSGQ_CmdHandler(uint32_t aMagic, uint32_t aCmd, uint8_t * aMsgQSysIO);

#endif /* MSG_QUEUE_OSIL_H */
/** @} */
