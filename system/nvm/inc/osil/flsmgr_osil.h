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

/** @defgroup grp_flsmgr_il Integration Guide
    @ingroup grp_flsmgr

    @addtogroup grp_flsmgr_il
    @{
    @section sec_flsmgr_integration Integration Guide
    This section describes all the details to integrate flash manager to system.

    @section sec_flsmgr_il_seq Sequence Diagram
    This section describes integration sequence diagram for #FLSMGR_Read,
    #FLSMGR_GetOpResult and #FLSMGR_Cancel. Flash manger asynchronous operations
    like #FLSMGR_Init and #FLSMGR_DeInit shall follow same sequence as
    #FLSMGR_Read.

    @image html flsmgr_seq_il_diagram.jpg "Integration layer sequence diagram"

    @file flsmgr_osil.h
    @brief Flash manager Integration Interfaces
    This header file contains the integration interface for Flash manager.
    @version 0.52 Imported from docx
*/

#ifndef FLSMGR_OSIL_H
#define FLSMGR_OSIL_H

#include <flsmgr.h>

/**
    @name Flash Manager Interface IDs
    @{
    @brief Inegration Interface IDs for Flash Manager
*/
#define BRCM_SWARCH_FLSMGR_IL_SERVER_TASK_MACRO         (0x40U) /**< @brief #FLSMGR_SERVER_TASK_ID */
#define BRCM_SWARCH_FLSMGR_IL_IRQ_PROCESS_TASK_MACRO    (0x41U) /**< @brief #FLASH_IRQ_PROCESS_TASK */
#define BRCM_SWARCH_FLSMGR_IL_EVENTS_MACRO              (0x42U) /**< @brief #PTM_MSGQ_EVENT_ID */
#define BRCM_SWARCH_FLSMGR_IL_CMD_TYPE                  (0x43U) /**< @brief #FLSMGR_CmdType */
#define BRCM_SWARCH_FLSMGR_IL_MSG_TYPE                  (0x44U) /**< @brief #FLSMGR_MsgType */
#define BRCM_SWARCH_FLSMGR_IL_MSG_HANDLER_PROC          (0x45U) /**< @brief #FLSMGR_MsgHandler */
#define BRCM_SWARCH_FLSMGR_IL_EVENT_HANDLER_PROC        (0x46U) /**< @brief #FLSMGR_EventHandler */
#define BRCM_SWARCH_FLSMGR_IL_SEND_MSG_PROC             (0x47U) /**< @brief #FLSMGR_SendMsg */
#define BRCM_SWARCH_FLSMGR_IL_TRY_CANCEL_MSG_PROC       (0x48U) /**< @brief #FLSMGR_TryCancelMsg */
#define BRCM_SWARCH_FLSMGR_IL_RECV_MSG_PROC             (0x49U) /**< @brief #FLSMGR_RecvMsg */
#define BRCM_SWARCH_FLSMGR_IL_SERVER_INIT_PROC          (0x4AU) /**< @brief #FLSMGR_ServerInit */
/** @} */

/**
    @brief Flash manager server task ID

    Server task to handle flash manager Events.

    @trace #BRCM_SWREQ_FLSMGR_IL_SERVER_TASK
*/
#define FLSMGR_SERVER_TASK_ID               (FLSMGR_Server)

/**
    @brief Bottom half task for flash

    Server task to handle Flash Events.

    @trace #BRCM_SWREQ_FLSMGR_IL_IRQ_PROCESS_TASK
*/
#define FLASH_IRQ_PROCESS_TASK              (FLSMGR_Server)

/**
    @name Flash Manager Events
    @{
    @brief Events that are handled by FLSMGR_Server

    @trace #BRCM_SWREQ_FLSMGR_IL_EVENTS
*/
#define PTM_MSGQ_EVENT_ID           (SystemEvent0)  /**< @brief Event for PTM
                                                         message */
#define PTM_FLASH_OP_DONE_EVENT_ID  (SystemEvent1)  /**< @brief Event for flash
                                                         manager operation done
                                                         for PTM */
#define FLSMGR_EVENT_ID             (SystemEvent2)  /**< @brief Event for
                                                         #FLSMGR_Init request */
#define FLSMGR_0_MSGQ_EVENT         (SystemEvent3)  /**< @brief Event for
                                                         message indication from
                                                         Flash 0 */
#define FLSMGR_0_CB_IND_EVENT       (SystemEvent4)  /**< @brief Event for
                                                         callback indication
                                                         from Flash 0 */
#define FLSMGR_1_MSGQ_EVENT         (SystemEvent5)  /**< @brief Event form
                                                         message indication from
                                                         Flash 1 */
#define FLSMGR_1_CB_IND_EVENT       (SystemEvent6)  /**< @brief Event for
                                                         callback indication
                                                         from Flash 1 */
#define FLASH0_EVENT                (SystemEvent7)  /**< @brief Event for
                                                         Flash 0 */
#define FLASH1_EVENT                (SystemEvent8)  /**< @brief Event for
                                                         Flash 1 */
/** @} */

/**
    @name FLSMGR_CmdType
    @{
    @brief Flash manager command type.

    This type shall be used for describing flash manager command in flash
    manager message type (#FLSMGR_MsgType).

    @trace #BRCM_SWREQ_FLSMGR_IL_CMD
*/
typedef uint32_t FLSMGR_CmdType;    /**< @brief Flash manager command type */

/**
    @brief Flash manager message type

    This type shall be used for passing message using message queue.

    @trace #BRCM_SWREQ_FLSMGR_IL_MSG
*/
typedef struct {
    FLSMGR_CmdType      cmd;        /**< @brief Flash manager command */
    FLSMGR_AddrType     addr;       /**< @brief Flash operation address */
    FLSMGR_LenType      len;        /**< @brief Flash operation length */
    const uint8_t *     bufIn;      /**< @brief Input data buffer */
    uint8_t *           bufOut;     /**< @brief Output data buffer */
    FLSMGR_OpResultType opResult;   /**< @brief Operation result */
} FLSMGR_MsgType;

/** @brief Handle flash manager messages

    Interface to handle flash manager messages.

    @behavior Sync, Re-entrant (for different aID)

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aMsg        Pointer to flash manager message

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message processing complete, message
                                        shall be marked as complete.
    @retval     #BCM_ERR_EAGAIN         Message processing in-progress. Message
                                        shall not be marked as complete.
    @retval     #BCM_ERR_INVAL_PARAMS   Returned in the following cases:
                                        - aMsg->cmd is invalid
                                        - Invalid aID
                                        - aMsg is NULL

    @post TBD

    @note This interface shall be called from #FLSMGR_EventHandler.

    @trace  #BRCM_SWREQ_FLSMGR_IL_MSG_HANDLER

    @limitations None
*/
extern int32_t FLSMGR_MsgHandler(FLSMGR_DevIDType aID,
                            FLSMGR_MsgType * const aMsg);

/** @brief Handle flash manager server events

    Interface to handle flash manager server events. This function retrieves
    messages from message queue, which is processed by #FLSMGR_MsgHandler. On
    completion of message processing, message is marked completed in this
    interface.

    @behavior Sync, Re-entrant (for different aID)

    @pre TBD

    @param[in]      aID         Flash manager device ID

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall be
    called from Flash manager server task.

    @trace  #BRCM_SWREQ_FLSMGR_IL_EVENT_HANDLER

    @limitations None
*/
extern void FLSMGR_EventHandler(FLSMGR_DevIDType aID);

/** @brief Send flash manager message

    Interface to post as message to flash manager server. @c aMsgHdr (message
    header) shall be output by this interface for client to query the status of
    the submitted message.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aMsg        Pointer to Flash manager message
    @param[in]      aClientMask Client signal mask, this signal shall be used to
                                notify client about completion of the request
    @param[out]     aMsgHdr     Pointer to unique message header for the message
                                sent, this message header shall be used to:
                                - Query the result of the operation using
                                  #FLSMGR_RecvMsg or
                                - Cancel the operation using #FLSMGR_TryCancel

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is successfully queued to flash
                                        manager message queue
    @retval     Others                  As returned from #MSGQ_SendMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_FLSMGR_IL_SEND_MSG

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_SendMsg(FLSMGR_DevIDType aID,
                              FLSMGR_MsgType * const aMsg,
                              const uint32_t aClientMask,
                              const FLSMGR_MsgHdrType ** const aMsgHdr);

/** @brief Try cancelling flash manager message

    Interface to cancel the message which is previously send using
    #FLSMGR_SendMsg.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aMsgHdr     Unique message header retrieved during
                                #FLSMGR_SendMsg.
    @param[out]     aMsg        Pointer to retrieve processed Flash manager
                                message, if processing is already complete.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is processed and response is
                                        already copied to @c aMsg. In this case,
                                        client shall not wait for signal from
                                        flash manager.
    @retval     Others                  As returned from #MSGQ_TryCancelMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_FLSMGR_IL_TRY_CANCEL_MSG

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_TryCancelMsg(FLSMGR_DevIDType aID,
                                   const FLSMGR_MsgHdrType* const aMsgHdr,
                                   FLSMGR_MsgType * const aMsg);

/** @brief Flash manager receive message

    Interface to query the status of the message sent using #FLSMGR_SendMsg and
    retrieve the processed message.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Flash manager device ID
    @param[in]      aMsgHdr     Unique message header retrieved during
                                #FLSMGR_SendMsg
    @param[out]     aMsg        Pointer to retrieve processed Flash manager
                                message.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is processed and response is
                                        copied to @c aMsg
    @retval     Others                  As returned from #MSGQ_RecvMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @c aID

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_FLSMGR_IL_RECV_MSG

    @limitations This interface shall be called from task context.
*/
extern int32_t FLSMGR_RecvMsg(FLSMGR_DevIDType aID,
                              const FLSMGR_MsgHdrType * const aMsgHdr,
                              FLSMGR_MsgType * const aMsg);

/** @brief Activate the Flash manager Server Task

    This interface activates the Flash manager Server Task.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param          void

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator.

    @trace  #BRCM_SWREQ_FLSMGR_IL_SERVER_INIT

    @limitations None
*/
extern void FLSMGR_ServerInit(void);

#endif /* FLSMGR_OSIL_H */
/** @} */
