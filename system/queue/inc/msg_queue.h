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

/**
    @defgroup grp_msgq_ifc Interface
    @ingroup grp_msgq

    @addtogroup grp_msgq_ifc
    @{
    @section sec_msgq_fsm State Machine
    @subsection subsec_msgq_fsm_with_notif With Client Notification
    Each message has the following states:
    -# @b Init: All messages are in @c Init state after the component
       initialization.
    -# @b Posted: When a client posts a request, the message shall be moved to
       @c Posted state.
    -# @b Processing: When service provider starts to process the request, the
       message shall be moved to @c Processing state.
    -# @b Completed: Upon completion of the request (success/error), the message
       shall be moved to @c Completed state. In case of error, the component
       shall update the component error type in component specific structure.
       When message is moved to @c Completed state, service provider shall
       signal the client. Upon receiving the signal, Client shall call
       #MSGQ_RecvMsg to get the server response, which in turn moves the message
       to @c Init state.

    Below figure depicts the state transition for a message:
    @image html  msgq_state_machine_with_notif.jpg "State machine (with notification)"

    @subsection subsec_msgq_fsm_without_notif Without Client Notification
    Each message has the following states:
    -# @b Init: All messages are in @c Init state after the component
       initialization.
    -# @b Posted: When a client posts a request, the message state shall be
       moved to @c Posted state. After message is moved to @c Posted state,
       client shall trigger an event to the service provider.
    -# @b Processing: When service provider starts to process the request, the
       message shall be moved to @c Processing state. Upon completion of the
       request (success/error), the message shall be moved to @c Init state. In
       this mode, no notifications are sent to client.

    Below figure depicts the state transition for a message (without client
    notification):
    @image html msgq_state_machine_without_notif.jpg "State machine (without notifications)"

    @section sec_msgq_seq Sequence Diagrams
    @subsection subsec_msgq_seq_with_notif Client-Server communication with notifications
    @image html msgq_seq_diagram_with_notif.jpg "Client-Server Communication (with notifications)"

    @subsection subsec_msgq_seq_without_notif Client-Server communication without notifications
    @image html msgq_seq_diagram_without_notif.jpg "Client-Server Communication (without notifications)"

    @section sec_msgq_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @file msg_queue.h
    @brief Message Queue application interface
    This header file contains the interfaces for Message Queue
    @version 1.10 Imported from docx
*/

#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

/**
    @name Message Queue API IDs
    @{
    @brief API IDs for Message Queue
*/
#define BRCM_SWARCH_MSGQ_SIZE_MACRO             (0x00U) /**< @brief #MSGQ_SIZE                  */
#define BRCM_SWARCH_MSGQ_HDR_MAGIC_MACRO        (0x01U) /**< @brief #MSGQ_HDR_MAGIC             */
#define BRCM_SWARCH_MSGQ_INVAL_CLIENT_ID_MACRO  (0x02U) /**< @brief #MSGQ_INVAL_CLIENTID        */
#define BRCM_SWARCH_MSGQ_NO_CLIENTMASK_MACRO    (0x03U) /**< @brief #MSGQ_NO_CLIENTMASK         */
#define BRCM_SWARCH_MSGQ_MSG_STATUS_TYPE        (0x04U) /**< @brief #MSGQ_MsgStatusType         */
#define BRCM_SWARCH_MSGQ_MSGHDR_TYPE            (0x05U) /**< @brief #MSGQ_MsgHdrType            */
#define BRCM_SWARCH_MSGQ_HDR_Q_MAGIC_MACRO      (0x06U) /**< @brief #MSGQ_HDRQ_MAGIC            */
#define BRCM_SWARCH_MSGQ_HDRQ_FLAGS_TYPE        (0x07U) /**< @brief #MSGQ_HdrQFlagsType         */
#define BRCM_SWARCH_MSGQ_MSGHDRQ_TYPE           (0x08U) /**< @brief #MSGQ_HdrQType              */
#define BRCM_SWARCH_MSGQ_MSGQ_INIT_HDRQ_MACRO   (0x09U) /**< @brief #MSGQ_INIT_HDRQ             */
#define BRCM_SWARCH_MSGQ_MSGQ_DEFINE_HDRQ_MACRO (0x0AU) /**< @brief #MSGQ_DEFINE_HDRQ           */
#define BRCM_SWARCH_MSGQ_QUEUE_MAGIC_MACRO      (0x0BU) /**< @brief #MSGQ_MAGIC                 */
#define BRCM_SWARCH_MSGQ_CLIENT_SIGNAL_MACRO    (0x0CU) /**< @brief #MSGQ_CLIENT_SIGNAL_ENABLE  */
#define BRCM_SWARCH_MSGQ_MSG_QUEUE_TYPE         (0x0DU) /**< @brief #MSGQ_Type                  */
#define BRCM_SWARCH_MSGQ_MSGQ_INIT_MACRO        (0x0EU) /**< @brief #MSGQ_INIT                  */
#define BRCM_SWARCH_MSGQ_MSGQ_DEFINE_MACRO      (0x0FU) /**< @brief #MSGQ_DEFINE                */
#define BRCM_SWARCH_MSGQ_MSGQ_DEFINE_V2_MACRO   (0x10U) /**< @brief #MSGQ_DEFINE_V2             */
#define BRCM_SWARCH_MSGQ_SEND_MSG_PROC          (0x11U) /**< @brief #MSGQ_SendMsg               */
#define BRCM_SWARCH_MSGQ_TRY_CANCEL_MSG_PROC    (0x12U) /**< @brief #MSGQ_TryCancelMsg          */
#define BRCM_SWARCH_MSGQ_RECV_MSG_PROC          (0x13U) /**< @brief #MSGQ_RecvMsg               */
#define BRCM_SWARCH_MSGQ_GET_MSG_IDX_PROC       (0x14U) /**< @brief #MSGQ_GetMsgIdx             */
#define BRCM_SWARCH_MSGQ_COMPLETE_MSG_IDX_PROC  (0x15U) /**< @brief #MSGQ_CompleteMsgIdx        */
#define BRCM_SWARCH_MSGQ_GET_MSG_TYPE           (0x16U) /**< @brief #MSGQ_GetMsgType            */
#define BRCM_SWARCH_MSGQ_INVALID_INDEX_MACRO    (0x17U) /**< @brief #MSGQ_INVALID_INDEX         */
/** @} */

#include <stdint.h>

/**
    @brief Size of the Message Queue

    Server shall allocate 16 messages, although at any given point of time,
    message queue holds only 15 messages.

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_SIZE       (16UL)

/**
    @brief Message header magic

    Message header magic: "MSGH".

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_HDR_MAGIC       (0x4847534DUL)

/**
    @brief Invalid Message Queue client ID

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_INVAL_CLIENTID (-1L)

/**
    @brief Message queue invalid index

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_INVALID_INDEX              (0xFFFFFFFFUL)

/**
    @brief No Message Queue client mask

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_NO_CLIENTMASK  (0UL)

/**
    @name Message status
    @{
    @brief Status of a message object

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
typedef volatile uint32_t MSGQ_MsgStatusType;
#define MSGQ_MSG_STATUS_INIT       (0UL) /**< @brief Message is in initialized state.
                                         This is the state at reset */
#define MSGQ_MSG_STATUS_POSTED     (1UL) /**< @brief Client has posted the message */
#define MSGQ_MSG_STATUS_PROCESSING (2UL) /**< @brief Server is processing the
                                         message */
#define MSGQ_MSG_STATUS_COMPLETED  (3UL) /**< @brief Server completed processing the
                                         message */
/** @} */

/**
    @brief Header structure

    Header structure for a message containing requester and state information.

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
typedef struct _MSGQ_MsgHdrType {
    uint32_t magic;             /**< @brief Magic word containing \‘MSGH\'
                                     (#MSGQ_HDR_MAGIC) */
    int32_t clientID;           /**< @brief Task ID of client to signal upon
                                     completion (reset value =
                                     #MSGQ_INVAL_CLIENTID) */
    uint32_t clientMask;        /**< @brief Client event mask to signal upon
                                     completion (reset value =
                                     #MSGQ_NO_CLIENTMASK) */
    MSGQ_MsgStatusType status;  /**< @brief Status of the current message object */
} MSGQ_MsgHdrType;

/**
    @brief Message header queue magic

    Message header queue magic: "MSQH".

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_HDRQ_MAGIC (0x4851534DUL)

/**
    @name Message header queue flags
    @{
    @brief Flags to be used as part of #MSGQ_HdrQType

    <BR>bits[31-16]: 1 bit allocation for each slot
    <BR>bits[15-12]: Reserved, Read as '0'
    <BR>bits[11-8]: 4-bit read index which server is processing,
    <BR>bits[7-4]: Reserved, Read as '0'
    <BR>bits[3-0]: 4-bit write index which the client will write next

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
typedef volatile uint32_t MSGQ_HdrQFlagsType;
#define MSGQ_HDRQ_FLAGS_WINDEX_MASK     (0xFUL)     /**< @brief Write index mask      */
#define MSGQ_HDRQ_FLAGS_WINDEX_SHIFT    (0UL)       /**< @brief Write index bit shift */
#define MSGQ_HDRQ_FLAGS_RINDEX_MASK     (0xF00UL)   /**< @brief Read index mask       */
#define MSGQ_HDRQ_FLAGS_RINDEX_SHIFT    (8UL)       /**< @brief Read index bit shift  */
#define MSGQ_HDRQ_FLAGS_STATUS_MASK     (0x1UL)     /**< @brief Status mask           */
#define MSGQ_HDRQ_FLAGS_STATUS_SIZE     (0x1UL)     /**< @brief Status size           */
#define MSGQ_HDRQ_FLAGS_STATUS_SHIFT    (16UL)      /**< @brief Status bit shift      */
#define MSGQ_HDRQ_FLAGS_STATUS_BIT(aIdx)                \
                (MSGQ_HDRQ_FLAGS_STATUS_MASK <<         \
                (((aIdx) * MSGQ_HDRQ_FLAGS_STATUS_SIZE) \
                  + MSGQ_HDRQ_FLAGS_STATUS_SHIFT))  /**< @brief Get bit mask with the
                                                        corresponding bit of Message  */
/** @} */

/**
    @brief Message header queue structure

    Message queue structure to be instantiated by Service provider.

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
typedef struct _MSGQ_HdrQType{
    uint32_t magic;                 /**< @brief Magic word \'MSQH\'
                                            (#MSGQ_HDRQ_MAGIC) */
    MSGQ_HdrQFlagsType flags;       /**< @brief #MSGQ_HdrQFlagsType */
    MSGQ_MsgHdrType hdr[MSGQ_SIZE]; /**< @brief Array of Header structures
                                         containing client and message
                                         information */
} MSGQ_HdrQType;

#if (MSGQ_SIZE != 16UL)
#error "Wrong MSGQ_SIZE defined"
#else

/**
    @brief Macro to initialize #MSGQ_HdrQType structure

    This macro shall be used by server to initialize instance of #MSGQ_HdrQType.

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
 */
#define MSGQ_INIT_HDRQ                                          \
{                                                               \
.magic = MSGQ_HDRQ_MAGIC,                                       \
.flags = (MSGQ_SIZE - 1UL) << MSGQ_HDRQ_FLAGS_RINDEX_SHIFT,     \
.hdr = {{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                   \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT},      \
{MSGQ_HDR_MAGIC, MSGQ_INVAL_CLIENTID,                           \
                MSGQ_NO_CLIENTMASK, MSGQ_MSG_STATUS_INIT}}      \
}

/**
    @brief Macro to define and initialize #MSGQ_HdrQType instance of name @a aMsgHdrQ

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType (of name @a aMsgHdrQ).

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
 */
#define MSGQ_DEFINE_HDRQ(aMsgHdrQ)                                        \
    static MSGQ_HdrQType (aMsgHdrQ) = MSGQ_INIT_HDRQ;
#endif

/**
    @brief Message Queue Magic

    Message Queue Magic 'MSGQ'

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_MAGIC (0x5147534DUL)

/**
    @name Enable/Disable client notification
    @{

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
#define MSGQ_CLIENT_SIGNAL_ENABLE    (1UL)   /**< @brief Client notification enabled */
#define MSGQ_CLIENT_SIGNAL_DISABLE   (0UL)   /**< @brief Client notification disabled */
/** @} */

/** @brief Get the i'th message from the server

    Function pointer pointing to a function to get the ith message from the
    server. This function pointer shall be provided by the server.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aArg1       Brief description of aArg1

    @return     void *                  Pointer to the ith message

    @post None

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION

    @limitations None
*/
typedef void* (*MSGQ_GetMsgType)(uint32_t i);

/**
    @brief Message Queue handle

    Detailed structure description.

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
*/
typedef struct _MSGQ_Type {
    uint32_t magic;                 /**< @brief Magic word 'MSGQ'
                                         (#MSG_QUEUE_MAGIC checked for
                                         corruption) */
    int32_t serverID;               /**< @brief Task ID of server to be signaled
                                         on reception of the message by message
                                         queue. */
    uint32_t serverMask;            /**< @brief Server signal mask */
    uint32_t enableNotification;    /**< @brief Enable/Disable client
                                         notifications:
                                         - #MSGQ_CLIENT_SIGNAL_ENABLE
                                         - #MSGQ_CLIENT_SIGNAL_DISABLE */
    uint32_t msgSize;               /**< @brief Size of message structure
                                         (defined by server) */
    MSGQ_HdrQType *msgHdrQ;         /**< @brief Pointer to Queue of message
                                         headers */
    MSGQ_GetMsgType getMsg;         /**< @brief Function pointer to get i'th message */
} MSGQ_Type;

/**
    @brief Macro to initialize #MsgQ structure

    This macro shall be used by server to initialize #MsgQ.

    @param[in]      aServerID           ID of the server task, which is the only
                                        task allowed to call #MSGQ_GetMsgIdx and
                                        #MSGQ_CompleteMsgIdx
    @param[in]      aServerMask         Event mask to signal the server task
    @param[in]      aEnableNotification Enable Client notifications
    @param[in]      aMsgType            Type of the C object representing the
                                        message (This is used to get the size of
                                        the C object using sizeof operator)
    @param[in]      aMsgHdrQ            Pointer to #MSGQ_HdrQType
    @param[in]      aGetMessage         Callback to get ith message from the
                                        queue

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
 */
#define MSGQ_INIT(aServerID, aServerMask, aEnableNotification,  \
                    aMsgType, aMsgHdrQ, aGetMessage)            \
{                                                               \
    .magic = MSGQ_MAGIC,                                        \
    .serverID = (aServerID),                                    \
    .serverMask = (aServerMask),                                \
    .enableNotification = (aEnableNotification),                \
    .msgSize = sizeof(aMsgType),                                \
    .msgHdrQ = (aMsgHdrQ),                                      \
    .getMsg = (aGetMessage)                                     \
}

/**
    @brief Macro to define and initialize MsgQ structure

    This macro shall be used by server to define and initialize an instance of
    #MsgQ.

    @param[in]      aMsgQ               Name of the #MsgQ instance
    @param[in]      aServerID           ID of the server task, which is the only
                                        task allowed to call #MSGQ_GetMsgIdx and
                                        #MSGQ_CompleteMsgIdx
    @param[in]      aServerMask         Event mask to signal the server task
    @param[in]      aEnableNotification Enable Client notifications
    @param[in]      aMsgType            Type of the C object representing the
                                        message (This is used to get the size of
                                        the C object using sizeof operator)
    @param[in]      aMsgHdrQ            variable name for MSGQ_MsgHdrQ
    @param[in]      aGetMessage         Callback to get ith message from the
                                        queue

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
 */
#define MSGQ_DEFINE(aMsgQ, aServerID, aServerMask, aEnableNotification,     \
                    aMsgType, aMsgHdrQ, aGetMessage)                        \
static const MSGQ_Type (aMsgQ) = MSGQ_INIT(aServerID, aServerMask,          \
                                    aEnableNotification, aMsgType, aMsgHdrQ,\
                                    aGetMessage);

/**
    @brief Macro to define and initialize MsgQ structure with Memory Protection

    This macro shall be used by server to define and initialize an instance of
    #MsgQ.

    @param[in]      aMsgQ               Name of the #MsgQ instance
    @param[in]      aServerID           ID of the server task, which is the only
                                        task allowed to call #MSGQ_GetMsgIdx and
                                        #MSGQ_CompleteMsgIdx
    @param[in]      aServerMask         Event mask to signal the server task
    @param[in]      aEnableNotification Enable Client notifications
    @param[in]      aMsgType            Type of the C object representing the
                                        message (This is used to get the size of
                                        the C object using sizeof operator)
    @param[in]      aMsgHdrQ            variable name for MSGQ_MsgHdrQ
    @param[in]      aGetMessage         Callback to get ith message from the
                                        queue
    @param[in]      aSecName            Name of the Memory section

    @trace #BRCM_SWREQ_MSGQ_CONFIGURATION
 */
#define MSGQ_DEFINE_V2(aMsgQ, aServerID, aServerMask, aEnableNotification,   \
                    aMsgType, aMsgHdrQ, aGetMessage, aSecName)               \
static COMP_SECTION(aSecName) MSGQ_HdrQType (aMsgHdrQ) = MSGQ_INIT_HDRQ;     \
static const MSGQ_Type (aMsgQ) = MSGQ_INIT(aServerID, aServerMask,           \
                                    aEnableNotification, aMsgType, &aMsgHdrQ,\
                                    aGetMessage);

/** @brief Queue message

    API to queue a message in the message queue. A Handle is output by this API
    for client to query the status of the queued message (called by client).
    Upon successfully posting the message to the queue, the API signals the
    server.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      msgQHandle  Handle to message queue
    @param[in]      msg         Pointer to the message
    @param[in]      clientMask  Client signal mask
    @param[out]     pHdr        Pointer to #MSGQ_MsgHdrType

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is successfully queued to the
                                        message queue.
    @retval     #BCM_ERR_BUSY           Queue is full
    @retval     #BCM_ERR_UNKNOWN        Unknown error while signaling server
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid @a aHdr magic
    @retval     #BCM_ERR_INVAL_PARAMS   clientMask is 0 (when Client
                                        Notifications are enabled)
    @retval     #BCM_ERR_INVAL_PARAMS   @a msg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   msgQHandle does not have a valid magic
                                        'MSGQ'
    @retval     #BCM_ERR_INVAL_PARAMS   @a msgQHandle is NULL

    @post TBD

    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations This API shall be called only from task context.
*/
int32_t MSGQ_SendMsg(const MSGQ_Type* const msgQHandle,
                    const void* const msg,
                    const uint32_t clientMask,
                    const MSGQ_MsgHdrType** const pHdr);

/** @brief Try cancelling a message

    API to try to cancel the message which was previously queued by the client
    using #MSGQ_SendMsg API. If the request is in the Queue and not currently
    being processed by the server (i.e. not in progress), it shall be removed
    from the Message Queue, the handle to the message header shalll become
    invalid and API returns #BCM_ERR_CANCELLED. If the request is already in
    progress (i.e. being processed by the server), it cannot be cancelled and
    client need to wait for the completion of the message. In this case, API
    shall return #BCM_ERR_BUSY.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMsgQHandle Handle to message queue
    @param[in]      aHdr        Pointer to message header
    @param[out]     aMsg        Pointer to the message


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_CANCELLED      Message is cancelled
    @retval     #BCM_ERR_OK             Message processing is completed by the
                                        server and response is copied to @a aMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @a aHdr does not belong to
                                        @a aMsgQHandle
    @retval     #BCM_ERR_UNKNOWN        Unknown error while signaling server
    @retval     #BCM_ERR_BUSY           Message cannot be cancelled
    @retval     #BCM_ERR_INVAL_PARAMS   Message is in @c Init state
    @retval     #BCM_ERR_AUTH_FAILED    Task ID of the caller is not matching
                                        with the requestor’s Task ID
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid aHdr magic
    @retval     #BCM_ERR_INVAL_MAGIC    Message header Q magic is invalid
    @retval     #BCM_ERR_INVAL_MAGIC    @a msgQ->msgHdrQ is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsg is  NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsgQHandle has invalid magic
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsgQHandle is NULL

    @post TBD

    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations This API shall be called only from task context.
*/
int32_t MSGQ_TryCancelMsg(const MSGQ_Type* const aMsgQHandle,
                        const MSGQ_MsgHdrType* const aHdr,
                        void* const aMsg);

/** @brief Query the status of submitted message

    API to query the status of the submitted message and get response. Upon
    successfully receiving the response, the API signals the server task as
    well.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMsgQHandle Handle to message queue
    @param[in]      aHdr        Pointer to message header
    @param[out]     aMsg        Pointer to the message

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message processing is completed by
                                        server and response is copied to
                                        @a aMsg. Client no longer owns the
                                        message header.
    @retval     #BCM_ERR_UNKNOWN        Unknown error while signaling server
    @retval     #BCM_ERR_BUSY           Message is not processed yet
    @retval     #BCM_ERR_INVAL_PARAMS   API is called in wrong state
    @retval     #BCM_ERR_AUTH_FAILED    Task ID of the caller is not matching
                                        with the requestor’s Task ID
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid aHdr magic
    @retval     #BCM_ERR_INVAL_MAGIC    msgQ->msgHdrQ has invalid magic
    @retval     #BCM_ERR_INVAL_MAGIC    msgQ->msgHdrQ is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsg is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aHdr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsgQHandle does not have a valid
                                        magic
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMsgQHandle is NULL

    @post TBD

    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations This API shall be called by the client only when client
    notifications are enabled.
    @limitations API shall be called only from task context.
*/
int32_t MSGQ_RecvMsg(const MSGQ_Type* const aMsgQHandle,
                    const MSGQ_MsgHdrType* const aHdr,
                    void* const aMsg);

/** @brief Get the index of the pending message to be processed

    This API shall be called by the server. If this API is called from any task
    other than the server task, the API shall return #BCM_ERR_AUTH_FAILED.

    @behavior Sync, Re-entrant for different message queues

    @pre TBD

    @param[in]      msgQHandle  Handle to message queue
    @param[out]     idx         Pointer to index of message (valid only if API
                                returns #BCM_ERR_OK)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             There is a new message in the queue and
                                        the index has been successfully
                                        retrieved
    @retval     #BCM_ERR_BUSY           No new message in queue or client is
                                        busy in posting new message
    @retval     #BCM_ERR_AUTH_FAILED    Calling task is not authorized to get
                                        the message index
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid header magic
    @retval     #BCM_ERR_INVAL_MAGIC    Header is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a idx is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a msgQHandle does not have a valid magic
    @retval     #BCM_ERR_INVAL_PARAMS   @a msgQHandle is NULL

    @post TBD

    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations This API should be called only from server task context.
*/
int32_t MSGQ_GetMsgIdx(const MSGQ_Type* const msgQHandle,
                        uint32_t* const idx);

/** @brief Notify a message processing completion

    API to be called by server to notify a message processing completion. Upon
    marking a message as completed, the corresponding client is signaled. If
    this API is called from any task other than the server task, the API shall
    return #BCM_ERR_AUTH_FAILED.

    @behavior Sync, Re-entrant for different message queues

    @pre TBD

    @param[in]      msgQHandle  Pointer to MsgQHandle where message processing
                                completion needs to be notified
    @param[in]      idx         Index of the message to be marked as complete

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message successfully moved to completed
                                        state in message queue
    @retval     #BCM_ERR_UNKNOWN        Unknown error while signaling client
    @retval     #BCM_ERR_UNKNOWN        Message already in completed state
    @retval     #BCM_ERR_INVAL_PARAMS   API called from wrong state
    @retval     #BCM_ERR_AUTH_FAILED    Calling task is not authorized to
                                        complete the message
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid header magic
    @retval     #BCM_ERR_INVAL_MAGIC    msgQHandle->msgHdrQ does not have a
                                        valid magic
    @retval     #BCM_ERR_INVAL_PARAMS   @a idx is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a msgQHandle does not have a valid
                                        magic
    @retval     #BCM_ERR_INVAL_PARAMS   @a msgQHandle is NULL

    @post TBD

    @trace #BRCM_SWREQ_MSGQ_USER_INTERFACE

    @limitations This API shall be called only from server task context.
*/
int32_t MSGQ_CompleteMsgIdx(const MSGQ_Type* const msgQHandle,
                            const uint32_t idx);
#endif /* MSG_QUEUE_H */
/** @} */
