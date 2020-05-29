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
    @addtogroup grp_ets_il
    @{
    @file ets_osil.c
    @brief ETS Library OS Integration Layer
    This source file contains the integration of ETS to system
    @version 0.1 Imported from Word Document
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <ee.h>
#include <bcm_err.h>
#include <circ_queue.h>
#include <nif.h>
#ifdef ENABLE_ETH_SWITCH
#include <nif_switch.h>
#endif
#include <utils.h>
#include <atomic.h>
#include <rpc_async.h>
#include <chip_config.h>
#include "ets.h"
#include "ets_osil.h"
#include <osil/bcm_osil.h>
#include <comms.h>
#include <imgl.h>
#include <ptm.h>
#include <system.h>
/**
    @name ETS Component Design IDs
    @{
    @brief Design IDs for ETS component

*/
#define BRCM_SWDSGN_ETS_COMP_INSTANCE_MACRO                      (0xA0U) /**< @brief #ETS_COMP_INSTANCE_ID */
#define BRCM_SWDSGN_ETS_TASK_EVENT_COMMAND_MACRO                 (0xA1U) /**< @brief #ETS_TASK_EVENT_COMMAND */
#define BRCM_SWDSGN_ETS_TASK_EVENT_NETWORK_MACRO                 (0xA2U) /**< @brief #ETS_TASK_EVENT_NETWORK */
#define BRCM_SWDSGN_ETS_TASK_EVENT_TIMER_MACRO                   (0xA3U) /**< @brief #ETS_TASK_EVENT_TIMER */
#define BRCM_SWDSGN_ETS_CMDGETMESSAGE_PART_PROC                  (0xA4U) /**< @brief #ETS_CmdGetMessage */
#define BRCM_SWDSGN_ETS_DEFINE_MSGDHRQ_GLOBAL                    (0xA5U) /**< @brief #ETS_CmdHdrQ */
#define BRCM_SWDSGN_ETS_CMDQ_GLOBAL                              (0xA6U) /**< @brief #ETS_CmdQ */
#define BRCM_SWDSGN_ETS_PKTEVENT_TYPE                            (0xA7U) /**< @brief #ETS_PktEvent */
#define BRCM_SWDSGN_ETS_NETWORKEVENT_TYPE                        (0xA8U) /**< @brief #ETS_NetworkEventType */
#define BRCM_SWDSGN_ETS_NETWORKQ_GLOBAL                          (0xA9U) /**< @brief #ETS_NetworkQ */
#define BRCM_SWDSGN_ETS_NETWORKQBUF_GLOBAL                       (0xAAU) /**< @brief #ETS_NetworkQBuf */
#define BRCM_SWDSGN_ETS_NUM_TX_WAITQ_PKTS_MACRO                  (0xABU) /**< @brief #ETS_NUM_TX_WAITQ_PKTS */
#define BRCM_SWDSGN_ETS_TXWAITPKT_TYPE                           (0xACU) /**< @brief #ETS_TxWaitPktType */
#define BRCM_SWDSGN_ETS_CONTEXT_TYPE                             (0xADU) /**< @brief #ETS_Context */
#define BRCM_SWDSGN_ETS_ERRORREPORT_PART_PROC                    (0xAEU) /**< @brief #ETS_ErrorReport */
#define BRCM_SWDSGN_ETS_SENDMSG_PROC                             (0xAFU) /**< @brief #ETS_SendMsg */
#define BRCM_SWDSGN_ETS_RECVRESP_PROC                            (0xB0U) /**< @brief #ETS_RecvResp */
#define BRCM_SWDSGN_ETS_TRYCANCELMSG_PROC                        (0xB1U) /**< @brief #ETS_TryCancelMsg */
#define BRCM_SWDSGN_ETS_RXPKTINDICATION_PROC                     (0xB2U) /**< @brief #ETS_RxPktIndication */
#define BRCM_SWDSGN_ETS_TXPKTCONFIRMATION_PROC                   (0xB3U) /**< @brief #ETS_TxPktConfirmation */
#define BRCM_SWDSGN_ETS_RXMGMTINFOINDICATION_PROC                (0xB4U) /**< @brief #ETS_RxMgmtInfoIndication */
#define BRCM_SWDSGN_ETS_RXPKTTSINDICATION_PROC                   (0xB5U) /**< @brief #ETS_RxPktTSIndication */
#define BRCM_SWDSGN_ETS_TXPKTTSINDICATION_PROC                   (0xB6U) /**< @brief #ETS_TxPktTSIndication */
#define BRCM_SWDSGN_ETS_TRANSMIT_PROC                            (0xB7U) /**< @brief #ETS_Transmit */
#define BRCM_SWDSGN_ETS_CONFIGSAVECALLBACK_PROC                  (0xB8U) /**< @brief #ETS_ConfigSaveCallback */
#define BRCM_SWDSGN_ETS_EVENTCALLBACK_PROC                       (0xB9U) /**< @brief #ETS_EventCallback */
#define BRCM_SWDSGN_ETS_TIMERALARMCB_PROC                        (0xBAU) /**< @brief #ETS_TimerAlarmCb */
#define BRCM_SWDSGN_ETS_PROTOCOLTASK_PROC                        (0xBBU) /**< @brief #ETS_ProtocolTask */
#define BRCM_SWDSGN_ETS_PROCESSNETWORKEVENTS_PART_PROC           (0xBCU) /**< @brief #ETS_ProcessNetworkEvents */
#define BRCM_SWDSGN_ETS_PROCESSCOMMANDS_PART_PROC                (0xBDU) /**< @brief #ETS_ProcessCommands */
#define BRCM_SWDSGN_ETS_PROCESSTIMEREVENT_PART_PROC              (0xBEU) /**< @brief #ETS_ProcessTimerEvent */
#define BRCM_SWDSGN_ETS_HANDLE_GLOBAL                            (0xBFU) /**< @brief #ETS_Handle */
#define BRCM_SWDSGN_ETS_LOAD_CFG_PROC                            (0xC0U) /**< @brief #ETS_LoadCfg */
#define BRCM_SWDSGN_ETS_TASK_EVENT_CFG_MACRO                     (0xC1U) /**< @brief #ETS_EVNT_CFG_LOAD */
#define BRCM_SWDSGN_ETS_COMP_STATE_TYPE                          (0xC2U) /**< @brief #ETS_CompStateType */
#define BRCM_SWDSGN_ETS_RECORDS_Q_SZ_MACRO                       (0xC3U) /**< @brief #ETS_RECORDS_Q_SZ */
#define BRCM_SWDSGN_ETS_RECORDQUEUE_TYPE                         (0xC4U) /**< @brief #ETS_RecordQueueType */
#define BRCM_SWDSGN_ETS_THRESHOLD_TIMER_TICKS                    (0xC5U) /**< @brief #ETS_THRESHOLD_TIMER_TICKS */
#define BRCM_SWDSGN_ETS_SENDRECORDTOHOST_PROC                    (0xC6U) /**< @brief #ETS_SendRecordToHost */
#define BRCM_SWDSGN_ETS_RECORDS_Q_LOG2_SZ_MACRO                  (0xC7U) /**< @brief #ETS_RECORDS_Q_LOG2_SZ */
/** @} */
/**
    @brief Instance identifier for the ETS OSIL
    @trace #BRCM_SWARCH_ETS_INST_ID_MACRO
    @trace #BRCM_SWREQ_ETS_STATUS
*/
#define ETS_COMP_INSTANCE_ID      (0x0U)

/**
    @brief OS Event for notifying ETS protocol task of commands
    @trace #BRCM_SWARCH_ETS_EVENT_COMMAND_MACRO
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_EVNT_COMMAND          SystemEvent0
/**
    @brief OS Event for notifying ETS protocol task of network events
    @trace #BRCM_SWARCH_ETS_EVENT_NETWORK_MACRO
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_EVNT_NETWORK          SystemEvent1
/**
    @brief OS Event for notifying ETS protocol task of timer events
    @trace #BRCM_SWARCH_ETS_EVENT_TIMER_MACRO
    @trace #BRCM_SWREQ_ETS_TIMER
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_EVNT_TIMER            SystemEvent2
/**
    @brief OS Event for notifying ETS protocol task of cfg load event
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_EVNT_CFG_LOAD         SystemEvent3

#ifdef ENABLE_RECORD_NOTIFICATION
/**
    @brief Log2 of circular queue depth, used to store records.
    This number can be configured.

*/
#define ETS_RECORDS_Q_LOG2_SZ (4UL)
/**
    @brief Circular queue depth, used to store records.
    This number can be configured.

*/
#define ETS_RECORDS_Q_SZ (1 << ETS_RECORDS_Q_LOG2_SZ)
/**
    @brief Threshold of timer ticks to trigger send
           records periodically
*/
#define ETS_THRESHOLD_TIMER_TICKS (2U)
#endif

#if defined(ENABLE_RECORD_NOTIFICATION) && defined(ENABLE_RECORD_NOTIFICATION_TEST)
extern int32_t ValidateRecord(ETS_RecordsListType* const aRecord);
#endif

/**
    @brief Get the i-th message from server

    Retrieve the i-th message from server.

    @behavior Sync, non-reentrant

    @pre None

    @param[in]  idx         Index of the message

    @return                 Pointer to the i-th message

    @post None

    @trace #BRCM_SWREQ_ETS_TASK

    @limitations None
*/
void* ETS_CmdGetMessage(uint32_t idx);

/**
    @brief #MSGQ_HdrQType instance of ETS command message queue

    This macro shall be used by the message queue server to initialize an
    instance of #MSGQ_HdrQType.

    @trace #BRCM_SWREQ_ETS_TASK
 */
MSGQ_DEFINE_HDRQ(ETS_CmdHdrQ);

/**
    @brief ETS Command Message Queue

    The message queue for the ETS command queue. All commands
    from the system are posted to this message queue.

    @trace #BRCM_SWREQ_ETS_TASK
*/
MSGQ_DEFINE((ETS_CmdQ), (ETS_ProtocolTask), (ETS_EVNT_COMMAND),
            (MSGQ_CLIENT_SIGNAL_ENABLE), ETS_CmdRespType, (&ETS_CmdHdrQ),
            (ETS_CmdGetMessage));


/* For each port, it is possible to have upto 4 events arrive at the */
/* same time. Consider a slave switch with N ports. This has (N-1)   */
/* master ports. If all the links are up, once the timer fires, a    */
/* PDelay Request is sent out of all ports. The timestamp indication */
/* and the PDelay response and follow up can be received at any time.*/
/* At the same timer, a SYNC message can be sent out of all master   */
/* ports and the timestamp indication can be received later          */
/* asynchronously. In the worst case we can receive a signaling      */
/* message on the master port.                                       */
/* For now we support upto 8 port switch and since the circular queue*/
/* can support (2^N - 1) slots, chosing to keep the number of events */
/* as 31 instead of 32 for switches and 3 for end points             */
#if (ETS_MAX_INTERFACES > 1)
#define ETS_NETWORKQ_LOG2_SIZE   (5UL)
#define ETS_NUM_NETWORK_EVENTS   (31UL)
#else
/**
    @brief Size of the network events queue in logarithmic scale
    @trace #BRCM_SWARCH_ETS_NTWK_LOG2_SIZE_MACRO
    @trace #BRCM_SWREQ_ETS_ENDPT_BRIDGE
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_NETWORKQ_LOG2_SIZE   (3UL)
/**
    @brief Size of the network events queue
    @trace #BRCM_SWARCH_ETS_NUM_NTWK_EVNTS_MACRO
    @trace #BRCM_SWREQ_ETS_ENDPT_BRIDGE
    @trace #BRCM_SWREQ_ETS_TASK
*/
#define ETS_NUM_NETWORK_EVENTS   (7UL)
#endif

/**
    @name ETS_PktEvent
    @{
    @brief Ethernet L2 packet event type

    @trace #BRCM_SWARCH_ETS_PKTEVNT_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef uint32_t ETS_PktEventType; /**< @brief typedef for ETS packet event */
#define ETS_PKTEVENT_RX (0UL)   /**< @brief L2 packet received */
#define ETS_PKTEVENT_TX (1UL)   /**< @brief L2 packet transmitted */
/** @} */

/**
    @name ETS_CompStateType
    @{
    @brief ETS COMPONENT STATE

    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef uint32_t ETS_CompStateType; /**< @brief typedef for ETS component state */
#define ETS_COMP_STATE_RESET    (0UL)   /**< @brief Reset state */
#define ETS_COMP_STATE_RUNNING  (1UL)   /**< @brief Running state */

/** @} */

/**
    @brief Structure for network event queue. This is overloaded to handle
    both transmit and receive packet events

    @trace #BRCM_SWARCH_ETS_NTWKEVENT_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef struct _ETS_NetworkEventType {
    ETS_PktEventType eventId;            /**< @brief Event identifier */
    uint32_t  intfIdx;                   /**< @brief Internal interface
                                              identifier */
    uint32_t  bufIdx;                    /**< @brief L2 packet buffer
                                              identifier */
    uint16_t pktLen;                     /**< @brief L2 packet length. Valid
                                              only for ETS_PKTEVENT_RX */
    ETS_TimestampType timestamp;         /**< @brief HW Timestamp for event
                                              messages */
    uint32_t timestampValid;             /**< @brief Indicates whether the
                                              timestamp field is valid or not */
    uint8_t  rxBuffer[ETS_MAX_PDU_SIZE]; /**< @brief Received packet buffer */
    uint8_t  dstMacAddr[6UL];            /**< @brief Received packet's
                                              destination MAC address */
    uint32_t pktType;                    /**< @brief Transmitted packet's type*/
} ETS_NetworkEventType;

/**
    @brief Network events circular queue handle

    The handle of network events circular queue.

    @trace #BRCM_SWARCH_ETS_NTWKQ_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_TASK
*/
CIRCQ_V2_DEFINE((ETS_NetworkQ), ETS_NETWORKQ_LOG2_SIZE, uint8_t, ETS_NetworkQBuf,
                ETS_NetworkQIdx, (CIRCQ_OVERWRITE_DISABLE), ".data.gptp");

#ifdef ENABLE_RECORD_NOTIFICATION
/**
    @brief Circular queue to store records
*/
CIRCQ_V2_DEFINE((ETS_RecordQ), ETS_RECORDS_Q_LOG2_SZ, ETS_RecordType, ETS_RecordQBuf,
                ETS_RecordQIdx, (CIRCQ_OVERWRITE_DISABLE), ".data.gptp");
#endif

/**
    @brief Macro for size of transmit confirmation wait queue

    Queue to keep track of sent packets for which Tx Confirmation
    is not yet received. We need only 3 slots per interface

    @trace #BRCM_SWARCH_ETS_NUM_TXWAITQ_PKTS_MACRO
    @trace #BRCM_SWREQ_ETS_NIF
*/
#define ETS_NUM_TX_WAITQ_PKTS      (24UL)

/**
    @brief Structure for transmit confirmation wait queue

    @trace #BRCM_SWARCH_ETS_TXWAITPKT_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
*/
typedef struct _ETS_TxWaitPktType {
    uint32_t intfIdx;    /**< @brief Internal interface identifier */
    uint32_t bufIdx;     /**< @brief L2 packet buffer identifier */
    uint32_t pktType;    /**< @brief Transmitted packet's type */
    uint32_t isEventMsg; /**< @brief Does transmitted packet contain an
                              event PDU ? */
} ETS_TxWaitPktType;

/**
    @brief Structure for ETS context

    @trace #BRCM_SWARCH_ETS_CONTEXT_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_STACKING
    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef struct _ETS_ContextType {
    ETS_ConfigType       config;                           /**< @brief Cached
                                                                configuration */
    ETS_CmdRespType      cmds[MSGQ_SIZE];                  /**< @brief Array of
                                                                command response
                                                                structures for the
                                                                command message
                                                                queue */
    ETS_NetworkEventType events[ETS_NUM_NETWORK_EVENTS];   /**< @brief Array of
                                                                network event
                                                                structures for the
                                                                events queue */
    uint32_t             networkQAllocFlags;               /**< @brief Flag to track
                                                                allocation of slots
                                                                in the network
                                                                queue */
    uint32_t             currRxIdx;                        /**< @brief Index of the
                                                                slot in the network
                                                                queue allocated for
                                                                the last received
                                                                packet */
    uint32_t             currTxIdx;                        /**< @brief Index of the
                                                                slot in the network
                                                                queue allocated for
                                                                the last transmitted
                                                                packet */
    ETS_TxWaitPktType     txWaitPkts[ETS_NUM_TX_WAITQ_PKTS];/**< @brief Array of
                                                                 structures for the
                                                                 transmit
                                                                 confirmation wait
                                                                 queue */
    uint32_t              txWaitQAllocFlags;                /**< @brief Flag to track
                                                                 allocation of slots
                                                                 in the transmit
                                                                 confirmation wait
                                                                 queue */
#ifdef HW_EGR_HDR_INV_PORT
    uint32_t              stackingIngressPort;              /**< @brief Stacking port
                                                                 acting as the slave
                                                                 port */
#endif
#ifdef ENABLE_RECORD_NOTIFICATION
    uint8_t               numTimerTicks;                     /**< @brief Count of timer ticks
                                                                 after */
#endif
} ETS_ContextType;

/**
    @brief Global context for ETS OSIL

    The data structure of the ETS OSIL which maintains the
    command, network and transmit confirmation queues.

    @trace #BRCM_SWARCH_ETS_CONTEXT_TYPE
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_STACKING
    @trace #BRCM_SWREQ_ETS_TASK
*/
static ETS_ContextType COMP_SECTION(".data.gptp") ETS_Handle =
{
    .networkQAllocFlags  = ((1UL << ETS_NUM_NETWORK_EVENTS) - 1UL),
    .txWaitQAllocFlags   = ((1UL << ETS_NUM_TX_WAITQ_PKTS) - 1UL),
#ifdef HW_EGR_HDR_INV_PORT
    .stackingIngressPort = ETHERSWT_INTERNAL_CPU_PORT,
#endif
};

/** @brief Process messages received on command queue

    This interface is invoked by the ETS task when it is notified of command(s)
    on the message queue. It retrieves the commands and invokes the appropriate
    library API

    @behavior Sync, Non-reentrant

    @pre None

    @param[out]  aState            Pointer to store state change


    @return void

    @post None

    @trace #BRCM_SWARCH_ETS_PROCESSCMDS_PART_PROC
    @trace #BRCM_SWREQ_ETS_TASK

    @limitations None
*/
static void ETS_ProcessCommands(ETS_CompStateType *const aState);

/** @brief Process messages received on network queue

    This interface is invoked by the ETS task when it is notified of a
    network event. It retrieves all events from the network events queue
    in FIFO order and feeds them to the ETS library.

    @behavior Sync, Non-reentrant

    @pre None

    @return     void

    @post None

    @trace #BRCM_SWARCH_ETS_PROCESSNTWKEVENTS_PART_PROC
    @trace #BRCM_SWREQ_ETS_NIF
    @trace #BRCM_SWREQ_ETS_TASK

    @limitations None
*/
static void ETS_ProcessNetworkEvents(void);

/** @brief Report error information to DET/DEM

    Report error information to DET/DEM

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  aApiID                 API identifier
    @param[in]  aErr                   Error number
    @param[in]  aVal0                  Additional error information
    @param[in]  aVal1                  Additional error information
    @param[in]  aVal2                  Additional error information
    @param[in]  aVal3                  Additional error information

    @return     void

    @post None

    @trace #BRCM_SWARCH_ETS_ERRORREPORT_PART_PROC

    @limitations None
*/
static void ETS_ErrorReport(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3);

/**
    @code{.unparsed}
    MSGQ_SendMsg(ETS_CmdQ);
    @endcode
*/

int32_t ETS_SendMsg (const ETS_CmdRespType* const aCmdMsg,
                     const uint32_t aClientMask,
                     const MSGQ_MsgHdrType ** const aMsgHdr)
{
    int32_t retVal = BCM_ERR_OK;

    /* Check input params */
    if((NULL == aCmdMsg) || (aClientMask == 0UL) || (NULL == aMsgHdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = MSGQ_SendMsg(&ETS_CmdQ, aCmdMsg, aClientMask, aMsgHdr);
    }
    if (BCM_ERR_OK != retVal) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_SENDMSG_PROC, retVal,
                (uint32_t) aCmdMsg, (uint32_t) aClientMask, (uint32_t) aMsgHdr,
                (NULL == aCmdMsg) ? 0UL : aCmdMsg->cmdId);
    }
    return retVal;
}

/**
    @code{.unparsed}
    MSGQ_RecvMsg(ETS_CmdQ);
    @endcode
*/
int32_t ETS_RecvResp (const MSGQ_MsgHdrType* const aMsgHdr,
                      ETS_CmdRespType* const aResp)
{
    int32_t retVal = BCM_ERR_OK;

    /* Check input params */
    if((NULL == aMsgHdr) || (NULL == aResp)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = MSGQ_RecvMsg(&ETS_CmdQ, aMsgHdr, aResp);
    }

    if (BCM_ERR_OK != retVal) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_RECVRESP_PROC, retVal,
                (uint32_t) aMsgHdr, (uint32_t) aResp,
                (NULL == aResp) ? 0UL:aResp->cmdId, 0UL);
    }
    return retVal;
}

/**
    @code{.unparsed}
    MSGQ_TryCancelMsg(ETS_CmdQ)
    @endcode
*/
int32_t ETS_TryCancelMsg(const MSGQ_MsgHdrType* const aMsgHdr,
                         ETS_CmdRespType* const aResp)
{
    int32_t                retVal   = BCM_ERR_OK;

    /* Check input params */
    if((NULL == aMsgHdr) || (NULL == aResp)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = MSGQ_TryCancelMsg(&ETS_CmdQ, aMsgHdr, aResp);
    }

    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_TRYCANCELMSG_PROC, retVal,
                       (uint32_t)aMsgHdr, (uint32_t)aResp, 0UL, 0UL);
    }
    return retVal;
}
/**
    @code{.unparsed}
    event = Allocate(networkQ);

    memcpy(event.rxBuffer, aPktBuf, aPktLen);
    event.eventId = ETS_PKTEVENT_RX;

#if (ETS_MAX_INTERFACES == 1)
    if (ETS_IsGptpEventMsg(aPktBuf)) {
        Push(networkQ, event);
        BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
    }
#endif
    @endcode
*/
void ETS_RxPktIndication (NIF_CntrlIDType aCtrlIdx,
                          const uint8_t *const aPktBuf,
                          uint16_t aPktLen,
                          const uint8_t *const aSrcMacAddr,
                          const uint8_t *const aDestMacAddr,
                          uint32_t aPriority)
{
    ETS_StateType state = ETS_GetState() ;
    int32_t       rc    = BCM_ERR_OK;

    /* Validate */
    if (state != ETS_STATE_RUNNING) {
        rc = BCM_ERR_INVAL_STATE;
        goto end;
    }

    if ((NULL == aPktBuf) || (0U == aPktLen) || (NULL == aDestMacAddr)) {
        rc = BCM_ERR_INVAL_PARAMS;
        goto end;
    }
    ETS_Handle.currRxIdx = ATOMIC_ClrNGetBit(&ETS_Handle.networkQAllocFlags);

    if (ETS_Handle.currRxIdx < ETS_NUM_NETWORK_EVENTS) {
        /* We only copy ETS_MAX_PDU_SIZE since a valid gPTP PDU */
        /* would not be larger than that                        */
        uint16_t  copyLength = (aPktLen > ETS_MAX_PDU_SIZE) ?
                                ETS_MAX_PDU_SIZE : aPktLen;

        BCM_MemCpy(ETS_Handle.events[ETS_Handle.currRxIdx].rxBuffer, aPktBuf, (size_t)copyLength);

        ETS_Handle.events[ETS_Handle.currRxIdx].eventId        = ETS_PKTEVENT_RX;
        ETS_Handle.events[ETS_Handle.currRxIdx].pktLen         = copyLength;
        ETS_Handle.events[ETS_Handle.currRxIdx].bufIdx         = (uint32_t) aPktBuf;
        ETS_Handle.events[ETS_Handle.currRxIdx].timestampValid = FALSE;
        BCM_MemCpy(ETS_Handle.events[ETS_Handle.currRxIdx].dstMacAddr, aDestMacAddr, 6UL);

        /* For end point this is the place to push the general messages */
        /* to the library                                               */
        /* For switch, wait for the management information callback     */
        /* For event messages wait for the timestamp callback           */
#if (ETS_MAX_INTERFACES == 1)
        ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx = 0UL;
        {
            uint32_t    isEventMsg = 0UL;
            rc = ETS_IsGptpEventMsg(ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx, aPktBuf, aPktLen, &isEventMsg);
            if ((BCM_ERR_OK == rc) && (0UL == isEventMsg)) {
                /* Push the packet now */
                rc = CIRCQ_Push(&ETS_NetworkQ, (char*)&ETS_Handle.currRxIdx, 1UL);
                if(BCM_ERR_OK == rc) {
                    /* Inform protocol task */
                    (void)BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
                } else {
                    /* Deallocate the buffer */
                    ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, ETS_Handle.currRxIdx);
                }
            }
        }
#endif
    } else {
        rc = BCM_ERR_NOMEM;
    }
end:
    if ((BCM_ERR_OK != rc) && (ETS_STATE_RUNNING == ETS_GetState())) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_RXPKTINDICATION_PROC, rc, (uint32_t) aPktBuf,
                       (uint32_t)aPktLen, (uint32_t)aDestMacAddr,
                       (uint32_t)ETS_Handle.currRxIdx);
    }
}

/**
    @code{.unparsed}
#if (ETS_MAX_INTERFACES > 1)
    entry = networkQ.last();
    if (aBuf == networkQ[entry].bufIdx) {
        networkQ[entry].intfIdx = aMgmtInfo->PortIdx;
        if (!ETS_IsGptpEventMsg(aBuf)) {
            Push(networkQ, event);
            BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
        }
    }
#endif
    @endcode
*/
void ETS_RxMgmtInfoIndication(NIF_CntrlIDType aCtrlIdx,
                              const uint8_t *const aBuf,
                              const ETHERSWT_MgmtInfoType *const aMgmtInfo)
{
#if (ETS_MAX_INTERFACES > 1)
    int32_t        rc         = BCM_ERR_OK;
    ETS_StateType  state      = ETS_GetState();
    uint32_t       isEventMsg;
    uint32_t       line       = 0UL;

    if (state != ETS_STATE_RUNNING) {
        rc = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto end;
    }

    if ((NULL == aBuf) || (NULL == aMgmtInfo)) {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto end;
    }

    /* Validate that we received the callback for the same packet */
    if (((uint32_t)aBuf == ETS_Handle.events[ETS_Handle.currRxIdx].bufIdx) &&
        (ETS_PKTEVENT_RX == ETS_Handle.events[ETS_Handle.currRxIdx].eventId)) {
#ifdef HW_EGR_HDR_INV_PORT
        if (ETHERSWT_INTERNAL_CPU_PORT == aMgmtInfo->PortIdx) {
            ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx = ETS_Handle.stackingIngressPort;
        } else {
            ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx = aMgmtInfo->PortIdx;
        }
#else
        ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx = aMgmtInfo->PortIdx;
#endif
        /* If this is a general message, no need to wait for the */
        /* timestamp callback                                    */
        rc = ETS_IsGptpEventMsg(ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx,
                                ETS_Handle.events[ETS_Handle.currRxIdx].rxBuffer,
                                ETS_Handle.events[ETS_Handle.currRxIdx].pktLen,
                                &isEventMsg);
        if ((BCM_ERR_OK == rc) && (0UL == isEventMsg)) {
            /* Push the packet now */
            rc = CIRCQ_Push(&ETS_NetworkQ, (char*)&ETS_Handle.currRxIdx, 1UL);
            if(BCM_ERR_OK == rc) {
                /* Inform protocol task */
                (void)BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
            } else {
                /* Deallocate the buffer */
                ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, ETS_Handle.currRxIdx);
                line = __LINE__;
            }
        }
    } else {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
    }

end:
    if ((BCM_ERR_OK != rc) && (ETS_STATE_RUNNING == ETS_GetState())) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_RXMGMTINFOINDICATION_PROC, rc, line,
                       (uint32_t)aBuf, ETS_Handle.events[ETS_Handle.currRxIdx].bufIdx,
                       ETS_Handle.events[ETS_Handle.currRxIdx].eventId);
    }
#endif
}

/**
    @code{.unparsed}
    entry = networkQ.last();
    if (aBuf == networkQ[entry].bufIdx) {
        networkQ[entry].timestamp = *aTS;
        networkQ[entry].timestampValid = *aTSQual;
        Push(networkQ, entry);
        BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
    }
    @endcode
*/

void ETS_RxPktTSIndication(NIF_CntrlIDType aCtrlIdx,
                     const uint8_t *const aBuf,
                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                     const ETHER_TimestampType *const aTS,
                     const ETHER_TimestampQualType* const aTSQual)
{
    int32_t       rc    = BCM_ERR_OK;
    ETS_StateType state = ETS_GetState();
    uint32_t      line  = 0UL;

    if (state != ETS_STATE_RUNNING) {
        rc = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto end;
    }

    if ((NULL == aBuf) || (NULL == aTS) || (NULL == aTSQual)) {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto end;
    }

    /* Validate that we received the callback for the same packet   */
    /* If yes, then this is definitely the place to push the packet */
    /* to the library                                               */
    if (((uint32_t)aBuf == ETS_Handle.events[ETS_Handle.currRxIdx].bufIdx) &&
        (ETS_PKTEVENT_RX == ETS_Handle.events[ETS_Handle.currRxIdx].eventId)) {
        if((NULL != aMgmtInfo) &&
           (aMgmtInfo->PortIdx != ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx)) {
            rc = BCM_ERR_INVAL_PARAMS;
            line = __LINE__;
            goto end;
        }
        ETS_Handle.events[ETS_Handle.currRxIdx].timestamp.seconds = aTS->seconds;
        ETS_Handle.events[ETS_Handle.currRxIdx].timestamp.nanoseconds = aTS->nanoseconds;
        if (ETHER_TIMESTAMPQUAL_VALID == *aTSQual) {
            ETS_Handle.events[ETS_Handle.currRxIdx].timestampValid = TRUE;
        } else {
            ETS_Handle.events[ETS_Handle.currRxIdx].timestampValid = FALSE;
        }
        /* Push the packet now */
        rc = CIRCQ_Push(&ETS_NetworkQ, (char*)&ETS_Handle.currRxIdx, 1UL);
        if(BCM_ERR_OK == rc) {
            /* Inform protocol task */
            (void)BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
        } else {
            /* Deallocate the buffer */
            ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, ETS_Handle.currRxIdx);
            line = __LINE__;
        }
    } else {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
    }
end:
    if ((BCM_ERR_OK != rc) && (ETS_STATE_RUNNING == ETS_GetState())) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_RXPKTTSINDICATION_PROC, rc, line, (uint32_t)aBuf,
                       ETS_Handle.events[ETS_Handle.currRxIdx].bufIdx,
                       ETS_Handle.events[ETS_Handle.currRxIdx].eventId);
        ETS_ErrorReport(BRCM_SWARCH_ETS_RXPKTTSINDICATION_PROC, rc,
                        ETS_Handle.events[ETS_Handle.currRxIdx].intfIdx,
                        (uint32_t) aMgmtInfo,
                        (NULL == aMgmtInfo) ? 0xFFUL : aMgmtInfo->PortIdx, 0UL);
    }
}

/**
    @code{.unparsed}
    entry = find (txWaitQ, aBufIdx);
    event = Allocate(networkQ);
    event.eventId = ETS_PKTEVENT_TX;
    event.pktType = txWaitQ[entry].pktType;
    event.intfIdx = txWaitQ[entry].intfIdx;
    event.bufIdx  = aBufIdx;
    if (!txWaitQ[entry].isEventMsg)  {
        Push(networkQ, event);
        BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
    }
    @endcode
*/
void ETS_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                            uint32_t aBufIdx)
{
    ETS_StateType         state  = ETS_GetState() ;
    int32_t               rc     = BCM_ERR_OK;
    ETS_NetworkEventType  *event = NULL;
    uint32_t              i      = 0UL;
    uint32_t              txWaitFlags = ATOMIC_Get(&ETS_Handle.txWaitQAllocFlags);

    /* First check if the module is in the running state or not */
    if (state != ETS_STATE_RUNNING) {
        rc = BCM_ERR_INVAL_STATE;
        goto end;
    }

    /* To know whether or not to wait for the timestamp callback, we peek  */
    /* into the txWaitPkts queue and also take this opportunity to         */
    /* populate the port information since we are not going to get that in */
    /* any of the callbacks                                                */
    for (; i < ETS_NUM_TX_WAITQ_PKTS; i++) {
        if ((0UL == (txWaitFlags & (1UL << i))) &&
            (aBufIdx == ETS_Handle.txWaitPkts[i].bufIdx )) {

            /* Allocate an entry in the network events queue */
            ETS_Handle.currTxIdx = ATOMIC_ClrNGetBit(&ETS_Handle.networkQAllocFlags);

            if (ETS_Handle.currTxIdx < ETS_NUM_NETWORK_EVENTS) {
                event = &ETS_Handle.events[ETS_Handle.currTxIdx];
                BCM_MemSet(event, 0U, sizeof(ETS_NetworkEventType));

                /* Copy all relevant information from the transmit wait queue */
                event->eventId        = ETS_PKTEVENT_TX;
                event->intfIdx        = ETS_Handle.txWaitPkts[i].intfIdx;
                event->pktType        = ETS_Handle.txWaitPkts[i].pktType;
                event->bufIdx         = aBufIdx;
                event->timestampValid = FALSE;
                if (0UL == ETS_Handle.txWaitPkts[i].isEventMsg) {
                    rc = CIRCQ_Push(&ETS_NetworkQ, (char*)&ETS_Handle.currTxIdx, 1UL);

                    if(BCM_ERR_OK == rc) {
                        (void)BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
                    } else {
                        /* Deallocate the buffer */
                        ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, ETS_Handle.currTxIdx);
                    }
                }
            } else {
                rc = BCM_ERR_NOMEM;
            }
            /* Free up the entry in the transmit wait queue */
            ATOMIC_SetBit(&ETS_Handle.txWaitQAllocFlags, i);
            break;
        }
    }
    if (i == ETS_NUM_TX_WAITQ_PKTS) {
        rc = BCM_ERR_NOT_FOUND;
    }

end:
    if (rc != BCM_ERR_OK) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_TXPKTCONFIRMATION_PROC, rc,
                        (uint32_t)(aCtrlIdx >> 32UL), (uint32_t)aCtrlIdx,
                        aBufIdx, (uint32_t)ETS_Handle.currTxIdx);
    }
}

/**
    @code{.unparsed}
    entry = networkQ.last();
    if (aBufIdx == networkQ[entry].bufIdx) {
        networkQ[entry].timestamp = *aTS;
        networkQ[entry].timestampValid = *aTSQual;
        Push(networkQ, entry);
        BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
    }
    @endcode
*/
void ETS_TxPktTSIndication(NIF_CntrlIDType aCtrlIdx,
                     uint32_t aBufIdx,
                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                     const ETHER_TimestampType *const aTS,
                     const ETHER_TimestampQualType* const aTSQual)
{
    int32_t       rc    = BCM_ERR_OK;
    ETS_StateType state = ETS_GetState();
    uint32_t      line  = 0UL;

    if (state != ETS_STATE_RUNNING) {
        rc = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto end;
    }

    if ((NULL == aTS) || (NULL == aTSQual)) {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto end;
    }

    /* Validate that we received the callback for the same packet */
    if ((aBufIdx == ETS_Handle.events[ETS_Handle.currTxIdx].bufIdx) &&
        (ETS_PKTEVENT_TX == ETS_Handle.events[ETS_Handle.currTxIdx].eventId)) {
        ETS_Handle.events[ETS_Handle.currTxIdx].timestamp.seconds = aTS->seconds;
        ETS_Handle.events[ETS_Handle.currTxIdx].timestamp.nanoseconds = aTS->nanoseconds;
        if (ETHER_TIMESTAMPQUAL_VALID == *aTSQual) {
            ETS_Handle.events[ETS_Handle.currTxIdx].timestampValid = TRUE;
        } else {
            ETS_Handle.events[ETS_Handle.currTxIdx].timestampValid = FALSE;
        }

        /* Push the event now */
        rc = CIRCQ_Push(&ETS_NetworkQ, (char*)&ETS_Handle.currTxIdx, 1UL);

        if(BCM_ERR_OK == rc) {
            (void)BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_NETWORK);
        } else {
            /* Deallocate the buffer */
            ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, ETS_Handle.currTxIdx);
            line = __LINE__;
        }
    } else {
        rc = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
    }
end:
    if (BCM_ERR_OK != rc) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_TXPKTTSINDICATION_PROC, rc, line, aBufIdx,
                        ETS_Handle.events[ETS_Handle.currTxIdx].bufIdx,
                        ETS_Handle.events[ETS_Handle.currTxIdx].eventId);
    }
}

/**
    @code{.unparsed}
    @endcode
*/
void ETS_ConfigSaveCallback (const uint32_t aPortIdx,
                             const uint32_t aPdelay,
                             const int32_t  aRateRatio)
{
    int32_t                ret;
    uint8_t                buf[ETS_CMDRESP_PAYLOAD_SIZE];
    ETS_CmdRespPayloadType notification;

    notification.u8Ptr = buf;
    notification.pDelayNBRR->port   = aPortIdx;
    notification.pDelayNBRR->pDelay = aPdelay;
    notification.pDelayNBRR->nbrr   = aRateRatio;
    ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_EVENT_IF_PDELAY_CHANGED,
                         notification.u8Ptr, sizeof(ETS_PortPDelayAndNBRRType));

    ETS_ErrorReport(BRCM_SWARCH_ETS_CONFIGSAVECALLBACK_PROC, ret,
            __LINE__, aPortIdx, aPdelay, (uint32_t)aRateRatio);
}

/**
    @code{.unparsed}
    switch (aEvent) {
        case ETS_EVENT_IF_UP:
        case ETS_EVENT_IF_DOWN:
#if defined(ENABLE_ETH_SWITCH)
            ETHERSWT_SetLedState(aPortIdx, aEvent);
#endif
#if defined(ENABLE_IPC)
            RPC_AsyncEvent(aPortIdx, aEvent);
#endif
            break;
        case ETS_EVENT_SYNC:
        case ETS_EVENT_SYNC_LOST:
        case ETS_EVENT_IF_LOST_RESP_EXCEEDED:
        case ETS_EVENT_IF_LOST_RESP_RECOVERED:
#if defined(ENABLE_IPC)
            RPC_AsyncEvent(aPortIdx, aEvent);
#endif
            break;
    }
    @endcode
*/
void ETS_EventCallback (const uint32_t      aPortIdx,
                        const ETS_EventType aEvent)
{
    int32_t                ret;
    uint32_t               line;
    uint8_t                buf[ETS_CMDRESP_PAYLOAD_SIZE];
    ETS_CmdRespPayloadType notification;

    notification.u8Ptr = buf;
    *notification.port = aPortIdx;

    switch (aEvent) {
        case ETS_EVENT_IF_UP:
#if defined(ENABLE_ETH_SWITCH)
            ret = ETHERSWT_SetLedState(0UL, aPortIdx, ETHERSWT_LED_AVB, TRUE);
            line = __LINE__;
            if (ret == BCM_ERR_OK) {
#endif
                ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                     notification.u8Ptr, sizeof(aPortIdx));
                line = __LINE__;
#if defined(ENABLE_ETH_SWITCH)
            }
#endif
            break;
        case ETS_EVENT_IF_DOWN:
#if defined(ENABLE_ETH_SWITCH)
            ret = ETHERSWT_SetLedState(0UL, aPortIdx, ETHERSWT_LED_AVB, FALSE);
            line = __LINE__;
            if (ret == BCM_ERR_OK) {
#endif
                ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                     notification.u8Ptr, sizeof(aPortIdx));
                line = __LINE__;
#if defined(ENABLE_ETH_SWITCH)
            }
#endif
            break;
        case ETS_EVENT_SYNC:
            ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                 NULL, 0UL);
            line = __LINE__;
            break;
        case ETS_EVENT_SYNC_LOST:
            ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                 NULL, 0UL);
            line = __LINE__;
            break;
        case ETS_EVENT_IF_LOST_RESP_EXCEEDED:
            ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                 notification.u8Ptr, sizeof(aPortIdx));
            line = __LINE__;
            break;
        case ETS_EVENT_IF_LOST_RESP_RECOVERED:
            ret = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, aEvent,
                                 notification.u8Ptr, sizeof(aPortIdx));
            line = __LINE__;
            break;
        default:
            ret = BCM_ERR_NOT_FOUND;
            line = __LINE__;
            break;
    }
    if (BCM_ERR_OK != ret) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_EVENTCALLBACK_PROC, ret, line,
                        aPortIdx, aEvent, 0UL);
    }
}

/**
    @code{.unparsed}
#ifndef ENDPOINT
    NIF_SwtSetMgmtInfo(aHwPortIdx);
#endif
    if (0UL != aLen) {
        if (1UL == aIsEventMsg) {
            NIF_EnableTxTimestamp(aBufIdx);
        }

        freeIdx= Allocate(&txWaitQ);
        txWaitQ[freeIdx].intfIdx    = aIntfIdx;
        txWaitQ[freeIdx].bufIdx     = aBufIdx;
        txWaitQ[freeIdx].pktType    = aPktType;
        txWaitQ[freeIdx].isEventMsg = aIsEventMsg;
    }
    NIF_Send(aBufIdx);
    @endcode
*/
int32_t ETS_Transmit(const uint32_t        aIntfNum,
                     const NIF_CntrlIDType aCtrlIdx,
                     const uint32_t        aBufIdx,
                     const uint32_t        aLen,
                     const uint32_t        aPktType,
                     const uint32_t        aIsEventMsg,
                     const uint8_t* const  aDstMacAddr)
{
    int32_t   rc      = BCM_ERR_OK;
    uint32_t  line    = 0UL;
    uint32_t  freeIdx = 0xFFUL;
#if (ETS_MAX_INTERFACES > 1)
    ETHERSWT_MgmtInfoType mgmtInfo;

    mgmtInfo.switchIdx = 0UL;
    mgmtInfo.PortIdx   = aIntfNum;
    rc = NIF_SwtSetMgmtInfo(aCtrlIdx, aBufIdx, &mgmtInfo);
    if (BCM_ERR_OK != rc) {
        line = __LINE__;
        /* Free the buffer */
        (void)NIF_Send(aCtrlIdx, ETHER_ETHERTYPE_GPTP, 1UL, aBufIdx, 0UL, aDstMacAddr);
        goto end;
    }
#endif

    if ((1UL == aIsEventMsg) && (0UL != aLen)){
        /* mark the packet for egress timestamping */
        rc = NIF_EnableTxTimestamp(aCtrlIdx, aBufIdx);
    }

    if(BCM_ERR_OK == rc) {
        /* Insert entry in the transmit wait queue to wait for TxConfirmation */
        if (0UL != aLen) {
            /* Store the bufIdx along with the pktType so that we can */
            /* identify the packet on a TxConfirmation                */
            freeIdx= ATOMIC_ClrNGetBit(&ETS_Handle.txWaitQAllocFlags);

            if (freeIdx < ETS_NUM_TX_WAITQ_PKTS) {
                ETS_Handle.txWaitPkts[freeIdx].intfIdx    = aIntfNum;
                ETS_Handle.txWaitPkts[freeIdx].bufIdx     = aBufIdx;
                ETS_Handle.txWaitPkts[freeIdx].pktType    = aPktType;
                ETS_Handle.txWaitPkts[freeIdx].isEventMsg = aIsEventMsg;
            } else {
                rc = BCM_ERR_NOMEM;
                line = __LINE__;
                goto end;
            }
        }
        rc = NIF_Send(aCtrlIdx, ETHER_ETHERTYPE_GPTP, 1UL, aBufIdx, aLen, aDstMacAddr);
    } else {
        line = __LINE__;
        /* Free the buffer */
        (void)NIF_Send(aCtrlIdx, ETHER_ETHERTYPE_GPTP, 1UL, aBufIdx, 0UL, aDstMacAddr);
    }
end:
    if (BCM_ERR_OK != rc) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_TRANSMIT_PROC, rc, line,
                        aIntfNum, aBufIdx, aLen);
        ETS_ErrorReport(BRCM_SWARCH_ETS_TRANSMIT_PROC, rc, aPktType, aIsEventMsg,
                        ETS_Handle.txWaitQAllocFlags, (uint32_t)freeIdx);
    }
    return rc;
}

/**
    @code{.unparsed}
    if (ETS_STATE_RUNNING == ETS_GetState()) {
        BCM_SetEvent(ETS_ProtocolTask, ETS_EVNT_TIMER);
    }
    @endcode
*/
void ETS_TimerAlarmCb(void)
{
    uint8_t       erikaRet;
    ETS_StateType state = ETS_GetState();

    if (state == ETS_STATE_RUNNING) {
        erikaRet = SetEvent(ETS_ProtocolTask, ETS_EVNT_TIMER);
        if (erikaRet != E_OK) {
            ETS_ErrorReport(BRCM_SWARCH_ETS_TIMERALARMCB_PROC, BCM_ERR_UNKNOWN,
                    (uint32_t) erikaRet, 0UL, 0UL, 0UL);
        }
    }
}

static int32_t ETS_LoadCfg(void)
{
    int32_t                 retVal;
    IMGL_LoadStatusType imglStatus;

    retVal = IMGL_LoadImg(BRCM_ETS_CONFIG, (uint8_t*)&ETS_Handle.config, 0UL, sizeof(ETS_ConfigType), ETS_EVNT_CFG_LOAD, &imglStatus);
    if (BCM_ERR_OK == retVal) {
        (void)BCM_WaitEvent(ETS_EVNT_CFG_LOAD);
        (void)BCM_ClearEvent(ETS_EVNT_CFG_LOAD);
        retVal = IMGL_GetStatus(&imglStatus);
        if (retVal != BCM_ERR_OK) {
            ETS_ErrorReport(BRCM_SWDSGN_ETS_LOAD_CFG_PROC , retVal,
                    __LINE__, 0UL, 0UL, 0UL);
            retVal = BCM_ERR_UNKNOWN;
        } else {
#ifdef ENABLE_ETH_SWITCH
            MCU_ExtendedInfoType stackingInfo = {0U};
#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
            retVal = MCU_GetExtendedInfo(&stackingInfo);
            if (BCM_ERR_OK != retVal) {
                ETS_ErrorReport(BRCM_SWDSGN_ETS_LOAD_CFG_PROC, retVal,
                        (uint32_t)__LINE__, 0UL, 0UL, 0UL);
            }
#endif
            COMMS_UpdateETSConfig(&stackingInfo, &ETS_Handle.config, TRUE);
#endif
        }
    } else {
        ETS_ErrorReport(BRCM_SWDSGN_ETS_LOAD_CFG_PROC , retVal, __LINE__, 0UL, 0UL, 0UL);
        retVal = BCM_ERR_UNKNOWN;
    }

    return retVal;
}

#ifdef ENABLE_RECORD_NOTIFICATION
/**
    @code{.unparsed}
    @endcode
*/
static void ETS_SendRecordToHost(void)
{
    uint32_t isEmpty = TRUE;
    int32_t rc = BCM_ERR_OK;
    ETS_RecordsListType record;
    record.numRecords      = 0UL;
    rc = CIRCQ_IsEmpty(&ETS_RecordQ, &isEmpty);
    if ((BCM_ERR_OK == rc) && (FALSE == isEmpty)) {
        /* Dequeue the records and send to host */
            record.numRecords = CIRCQ_Pop(&ETS_RecordQ, (char* const)&record.records, ETS_MAX_RECORDS_PER_IPC_MSG);
        if (record.numRecords != 0UL) {
#ifdef ENABLE_RECORD_NOTIFICATION_TEST
            rc = ValidateRecord(&record);
#else
            ETS_CmdRespPayloadType notification;
            notification.records = &record;
            rc = RPC_AsyncEvent(BCM_GROUPID_COMMS, BCM_ETS_ID, ETS_EVENT_IF_RECORD,
                    notification.u8Ptr, sizeof(ETS_RecordsListType));
#endif
        }
    }
        if (BCM_ERR_OK != rc) {
            ETS_ErrorReport(BRCM_SWDSGN_ETS_SENDRECORDTOHOST_PROC, rc,
                           (uint32_t) isEmpty, record.numRecords,
                            0UL, 0UL);
        }
}
#endif
/**
    @code{.unparsed}
    @endcode
*/
void ETS_RecordCallback(ETS_RecordType* const aRecord)
{
#ifdef ENABLE_RECORD_NOTIFICATION
    int32_t                ret;
    ret = CIRCQ_Push(&ETS_RecordQ, (char*)aRecord, 1UL);
    if (BCM_ERR_NOMEM == ret) {
        /* Flush the queue */
        ETS_SendRecordToHost();
        ret = CIRCQ_Push(&ETS_RecordQ, (char*)aRecord, 1UL);
    }
    if (BCM_ERR_OK != ret) {
        ETS_ErrorReport(BRCM_SWARCH_ETS_RECORDCB_PROC, ret, 0UL, 0UL, 0UL, 0UL);
    }
#else
    /* Purposefully ignore */
#endif
}
/**
    @code{.unparsed}
    @endcode
*/

#ifdef ENABLE_RECORD_NOTIFICATION
void ETS_PeriodicSendRecords(void)
{
    /* Send record/s to host,
       Period can be congifured using ETS_THRESHOLD_TIMER_TICKS */
    ETS_Handle.numTimerTicks++;
    if(ETS_THRESHOLD_TIMER_TICKS <= ETS_Handle.numTimerTicks) {
        ETS_SendRecordToHost();
        ETS_Handle.numTimerTicks = 0U;
    }
}
#endif /* ENABLE_RECORD_NOTIFICATION */

/** @brief Protocol Task

    The ETS protocol task runnable function.  It waits for the following events
    (in decreasing order of priority) and takes action once they occur:
    <ol>
    <li> Message posted on Command Queue </li>
    <li> Message posted on Network Queue </li>
    <li> Periodic Timer </li>
    </ol>

    @behavior Sync, Non-reentrant

    @pre None

    @return      void

    @post None

    @trace #BRCM_SWREQ_ETS_TASK

    @limitations This task needs to be lower in priority than the task context
    in which the callbacks from network interface are invoked

    @code{.unparsed}

    state = RESET;
    prevState = RESET;

    rc = ETS_LoadCfg();
    if (SUCCESS) {
        ETS_SendMsg(INIT);
    }

    while (1UL) {
        BCM_WaitEvent(ETS_EVNT_COMMAND | ETS_EVNT_NETWORK | ETS_EVNT_TIMER | ETS_EVNT_CFG_LOAD | ShutdownEvent);
        BCM_GetEvent(ETS_ProtocolTask, &getMask);
        BCM_ClearEvent(getMask);

        if (ETS_EVNT_COMMAND & getMask) {
            ETS_ProcessCommands(&state);

            if (state change) {
                Notify comms
            }
        }

        if (ETS_EVNT_CFG_LOAD &getMask) {
            ETS_RecvResp();
        }

        if (ETS_EVNT_NETWORK & getMask) {
            ETS_ProcessNetworkEvents();
        }

        if (ETS_EVNT_TIMER & getMask) {
            ETS_ProcessTimerEvent();
        }

        if (ShutdownEvent & getMask) {
            break;
        }
    }
    BCM_TerminateTask();
    @endcode
*/
TASK(ETS_ProtocolTask)
{
    int32_t retVal;
    BCM_EventMaskType getMask  = 0UL;
    ETS_CompStateType state = ETS_COMP_STATE_RESET;
    ETS_CompStateType prevState = ETS_COMP_STATE_RESET;
    const MSGQ_MsgHdrType *msgHdr = NULL;


    /* Init and configure*/
    retVal = ETS_LoadCfg();

    if (BCM_ERR_OK == retVal) {
        /* Send init command to self*/
        ETS_CmdRespType         cmdResp;
        cmdResp.payloadPtr.u8Ptr = cmdResp.payload;
        BCM_MemCpy(cmdResp.payloadPtr.config, &ETS_Handle.config, sizeof(ETS_ConfigType));
        cmdResp.cmdId  = ETS_CMD_INIT;
        cmdResp.ret    = BCM_ERR_UNKNOWN;
        retVal = ETS_SendMsg(&cmdResp, ETS_EVNT_CFG_LOAD, &msgHdr);
    }
    if (BCM_ERR_OK != retVal) {
        COMMS_NotifyState(COMMS_SUBSYSTEM_ETS, COMMS_SUBSYSTEM_STATE_ERROR);
        ETS_ErrorReport(BRCM_SWDSGN_ETS_PROTOCOLTASK_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
    while (1UL) {
        /* Wait for client to notify server (us) */
        (void) BCM_WaitEvent(ETS_EVNT_COMMAND | ETS_EVNT_NETWORK | ETS_EVNT_TIMER | ETS_EVNT_CFG_LOAD | ShutdownEvent);
        (void) BCM_GetEvent(ETS_ProtocolTask, &getMask);
        (void) BCM_ClearEvent(getMask);

        /* First process commands */
        if (0UL != (ETS_EVNT_COMMAND & getMask)) {
            ETS_ProcessCommands(&state);

            /* if a state change is detected send the notification to comms */
            if (prevState != state) {
                if (ETS_COMP_STATE_RUNNING == state) {
                    COMMS_NotifyState(COMMS_SUBSYSTEM_ETS, COMMS_SUBSYSTEM_STATE_INIT);
                }
                prevState = state;
            }
        }

        if (0UL != (ETS_EVNT_CFG_LOAD & getMask)) {
            ETS_CmdRespType resp;
            (void)ETS_RecvResp(msgHdr, &resp);
        }

        if (ETS_COMP_STATE_RUNNING == state) {
            /* Now process network events */
            if (0UL != (ETS_EVNT_NETWORK & getMask)) {
                ETS_ProcessNetworkEvents();
            }

            if (0UL != (ETS_EVNT_TIMER & getMask)) {
                /* Finally process timer event */
                ETS_ProcessTimerEvent();
#ifdef ENABLE_RECORD_NOTIFICATION
                /* Send record/s to host */
                ETS_PeriodicSendRecords();
#endif
            }
        }

        if (0UL != (ShutdownEvent & getMask)) {
            break;
        }
    }

    if (ETS_COMP_STATE_RUNNING == state) {
        retVal = ETS_DeInit();
    } else {
        retVal = BCM_ERR_OK;
    }

    if (BCM_ERR_OK == retVal) {
        COMMS_NotifyState(COMMS_SUBSYSTEM_ETS, COMMS_SUBSYSTEM_STATE_UNINIT);
    } else {
        COMMS_NotifyState(COMMS_SUBSYSTEM_ETS, COMMS_SUBSYSTEM_STATE_ERROR);
    }

    (void) BCM_TerminateTask();
}

static void ETS_Restart(uint32_t idx)
{
    ETS_StateType state = ETS_GetState() ;

    if (ETS_STATE_RUNNING == state) {
        ETS_Handle.cmds[idx].ret = ETS_DeInit();

        /* Reset the data structures after changing state to RESET */
        (void)CIRCQ_Reset(&ETS_NetworkQ);

        BCM_MemSet(ETS_Handle.txWaitPkts, 0U, sizeof(ETS_Handle.txWaitPkts));
        BCM_MemSet(ETS_Handle.events, 0U, sizeof(ETS_Handle.events));

        ETS_Handle.networkQAllocFlags  = ((1UL << ETS_NUM_NETWORK_EVENTS) - 1UL);
        ETS_Handle.txWaitQAllocFlags   = ((1UL << ETS_NUM_TX_WAITQ_PKTS) - 1UL);
        ETS_Handle.currRxIdx           = ETS_NUM_NETWORK_EVENTS;
        ETS_Handle.currTxIdx           = ETS_NUM_NETWORK_EVENTS;
    }

    ETS_Handle.cmds[idx].ret = ETS_Init(&ETS_Handle.config);

}

/**
    @code{.unparsed}
    while (idx = GetMsg(cmdQ)) {
        switch(cmds[idx].cmdId) {
            case ETS_CMD_INIT:
                ETS_Init(cmds[idx].config);
                Init(networkQ);
                state = INIT;
            break;
            case ETS_CMD_DEINIT:
                ETS_DeInit();
                state = RESET;
                break;
            default:
                break;
        }
        CompleteMsg(cmdQ, idx);
    }
    return running;
    @endcode
*/
static void ETS_ProcessCommands(ETS_CompStateType *const aState)
{
    uint32_t   idx     = 0UL;
    int32_t    retVal;
#ifdef HW_EGR_HDR_INV_PORT
    uint32_t   i;
#endif

    /* Try to fetch messages from the command message queue */
    do {
        retVal = MSGQ_GetMsgIdx(&ETS_CmdQ, &idx);

        if(BCM_ERR_OK == retVal) {
            ETS_Handle.cmds[idx].payloadPtr.u8Ptr = ETS_Handle.cmds[idx].payload;
            switch(ETS_Handle.cmds[idx].cmdId)
            {
                case ETS_CMD_INIT:

                    /* Initialize the state machines and data structures */

                    ETS_Handle.cmds[idx].ret = ETS_Init(ETS_Handle.cmds[idx].payloadPtr.config);

                    if (BCM_ERR_OK == ETS_Handle.cmds[idx].ret) {
                        /* Initialize network queue */
                        retVal = CIRCQ_Reset(&ETS_NetworkQ);
                        if (BCM_ERR_OK == retVal) {
                            *aState = ETS_COMP_STATE_RUNNING;
                            /* Initialize the queues */
                            BCM_MemSet(ETS_Handle.txWaitPkts, 0U, sizeof(ETS_Handle.txWaitPkts));
                            BCM_MemSet(ETS_Handle.events, 0U, sizeof(ETS_Handle.events));

                            /* Cache the received configuration */
                            BCM_MemCpy(&ETS_Handle.config, ETS_Handle.cmds[idx].payloadPtr.config, sizeof(ETS_ConfigType));
#ifdef HW_EGR_HDR_INV_PORT
                            /* In case we are a master stacking device operating as a gPTP slave */
                            /* any gPTP PDUs received will contain the source as port 7 instead  */
                            /* of the actual ingress port                                        */
                            /* In case there is no explicit slave port configured on this device,*/
                            /* find out the single stacking port which is asCapable              */
                            for (i = 0UL ; i < ETS_MAX_INTERFACES; ++i) {
                                if (ETS_ADMINMODE_ENABLE == ETS_Handle.config.intfCfg[i].asCapable) {
                                    if (ETS_ROLE_SLAVE == ETS_Handle.config.intfCfg[i].role) {
                                        /* We do not expect to receive any gPTP packets on this */
                                        /* device. In case we do, they are erroneous and should */
                                        /* be dropped                                           */
                                        ETS_Handle.stackingIngressPort = ETHERSWT_INTERNAL_CPU_PORT;
                                        break;
                                    } else if (ETS_ROLE_STACKING == ETS_Handle.config.intfCfg[i].role) {
                                        ETS_Handle.stackingIngressPort = ETS_Handle.config.intfCfg[i].hwPortNum;
                                    }
                                }
                            }
#endif
                        }
                    }
                    break;
                case ETS_CMD_DEINIT:
                    ETS_Handle.cmds[idx].ret = ETS_DeInit();
                    *aState = ETS_COMP_STATE_RESET;
                    break;
                case ETS_CMD_RESETSTATS:
                    ETS_Handle.cmds[idx].ret = ETS_ResetStats(*ETS_Handle.cmds[idx].payloadPtr.port);
                    break;
                case ETS_CMD_SETCONFIG:
                    if (ETS_CMDRESP_PAYLOAD_SIZE < sizeof(ETS_ConfigType)) {
                        ETS_Handle.cmds[idx].ret = BCM_ERR_NOMEM;
                    } else {
                        BCM_MemCpy(&ETS_Handle.config, ETS_Handle.cmds[idx].payloadPtr.config, sizeof(ETS_ConfigType));
                        ETS_Restart(idx);
                    }
                    break;
                case ETS_CMD_GETCONFIG:
                    if (ETS_CMDRESP_PAYLOAD_SIZE < sizeof(ETS_ConfigType)) {
                        ETS_Handle.cmds[idx].ret = BCM_ERR_NOMEM;
                    } else {
                        BCM_MemCpy(ETS_Handle.cmds[idx].payloadPtr.config, &ETS_Handle.config, sizeof(ETS_ConfigType));
                        ETS_Handle.cmds[idx].ret = BCM_ERR_OK;
                    }
                    break;
                case ETS_CMD_GETGLOBALSTATUS:
                    ETS_Handle.cmds[idx].payloadPtr.globalStatus->magicId = ETS_GLBL_STATUS_MAGIC_ID;
                    if (ETS_CMDRESP_PAYLOAD_SIZE < sizeof(ETS_GlobalStatusType)) {
                        ETS_Handle.cmds[idx].ret = BCM_ERR_NOMEM;
                    } else {
                        ETS_Handle.cmds[idx].ret = ETS_GetGlobalStatus(ETS_Handle.cmds[idx].payloadPtr.globalStatus);
                    }
                    break;
                case ETS_CMD_GETINTFSTATUS:
                    if (ETS_CMDRESP_PAYLOAD_SIZE < sizeof(ETS_PortStatsAndStatusType)) {
                        ETS_Handle.cmds[idx].ret = BCM_ERR_NOMEM;
                    } else {
                        ETS_Handle.cmds[idx].payloadPtr.portStatus->status.magicId = ETS_INTF_STATUS_MAGIC_ID;
                        ETS_Handle.cmds[idx].ret = ETS_GetIntfStatus(ETS_Handle.cmds[idx].payloadPtr.portStatus->num,
                                &ETS_Handle.cmds[idx].payloadPtr.portStatus->status);
                        if (BCM_ERR_OK == ETS_Handle.cmds[idx].ret) {
                            ETS_Handle.cmds[idx].payloadPtr.portStatus->stats.magicId = ETS_STATS_MAGIC_ID;
                            ETS_Handle.cmds[idx].ret = ETS_GetStats(ETS_Handle.cmds[idx].payloadPtr.portStatus->num,
                                    &ETS_Handle.cmds[idx].payloadPtr.portStatus->stats);
                        }
                    }
                    break;
                case ETS_CMD_SETGLOBALTIME:
                    if (ETS_CMDRESP_PAYLOAD_SIZE < sizeof(ETS_TimestampType)) {
                        ETS_Handle.cmds[idx].ret = BCM_ERR_NOMEM;
                    } else {
                        ETS_Handle.cmds[idx].ret = ETS_SetGlobalTime(ETS_Handle.cmds[idx].payloadPtr.time);
                    }
                    break;
#ifdef ENABLE_RECORD_NOTIFICATION
                case ETS_CMD_NOTIFY_RECORD:
                    ETS_Handle.cmds[idx].ret = ETS_EnableRecordNotification(*ETS_Handle.cmds[idx].payloadPtr.isNotifyingRecord);
                    if ((BCM_ERR_OK == ETS_Handle.cmds[idx].ret) &&
                       (FALSE == *ETS_Handle.cmds[idx].payloadPtr.isNotifyingRecord)) {
                        ETS_Handle.cmds[idx].ret = CIRCQ_Reset(&ETS_RecordQ);
                    }
                    break;
#endif
                default:
                    ETS_Handle.cmds[idx].ret = BCM_ERR_INVAL_PARAMS;
                    break;
            }
            retVal = MSGQ_CompleteMsgIdx(&ETS_CmdQ, idx);
        }

    } while(BCM_ERR_OK == retVal);

}

/**
    @code{.unparsed}
    while(event = Pop(networkQ)) {
        switch(event.eventId) {
            case ETS_PKTEVENT_RX:
                ETS_RxPacket(event.intfIdx,
                             event.rxBuffer,
                             event.pktLen,
                             event.dstMacAddr,
                             event.timestamp,
                             event.timestampValid);
                break;
            case ETS_PKTEVENT_TX:
                ETS_TxConfirmation(event.intfIdx,
                                   event.pktType,
                                   event.timestamp,
                                   event.timestampValid);
                break;
        }
    }
    @endcode
*/
static void ETS_ProcessNetworkEvents(void)
{
    uint32_t  idx;
    uint32_t  numEvents;
    uint8_t   idxArray[ETS_NUM_NETWORK_EVENTS];
    int32_t   rc = BCM_ERR_OK;

    BCM_MemSet(idxArray, 0U, sizeof(idxArray));

    /* Retrieve all messages from the queue one shot to maintain FIFO order */
    numEvents = CIRCQ_Pop(&ETS_NetworkQ, (char*)idxArray, ETS_NUM_NETWORK_EVENTS);
    for (idx = 0UL; idx < numEvents; idx++) {
        switch(ETS_Handle.events[idxArray[idx]].eventId)
        {
            case ETS_PKTEVENT_RX:
                rc = ETS_RxPacket(ETS_Handle.events[idxArray[idx]].intfIdx,
                                  ETS_Handle.events[idxArray[idx]].rxBuffer,
                                  ETS_Handle.events[idxArray[idx]].pktLen,
                                  ETS_Handle.events[idxArray[idx]].dstMacAddr,
                                  &ETS_Handle.events[idxArray[idx]].timestamp,
                                  ETS_Handle.events[idxArray[idx]].timestampValid);
                /* Deallocate the event */
                ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, idxArray[idx]);
                break;
            case ETS_PKTEVENT_TX:
                rc = ETS_TxConfirmation(ETS_Handle.events[idxArray[idx]].intfIdx,
                                        ETS_Handle.events[idxArray[idx]].pktType,
                                        &ETS_Handle.events[idxArray[idx]].timestamp,
                                        ETS_Handle.events[idxArray[idx]].timestampValid);
                /* Deallocate the event from network queue */
                ATOMIC_SetBit(&ETS_Handle.networkQAllocFlags, idxArray[idx]);
                break;
            default:
                rc = BCM_ERR_INVAL_PARAMS;
                break;
        }
        if (BCM_ERR_OK != rc) {
            uint8_t* macAddr = ETS_Handle.events[idxArray[idx]].dstMacAddr;
            ETS_ErrorReport(BRCM_SWDSGN_ETS_PROCESSNETWORKEVENTS_PART_PROC, rc,
                            idxArray[idx],
                            ETS_Handle.events[idxArray[idx]].eventId,
                            ETS_Handle.events[idxArray[idx]].intfIdx,
                            ETS_Handle.events[idxArray[idx]].bufIdx);
            ETS_ErrorReport(BRCM_SWDSGN_ETS_PROCESSNETWORKEVENTS_PART_PROC, rc,
                            ETS_Handle.events[idxArray[idx]].pktLen,
                            ETS_Handle.events[idxArray[idx]].pktType,
                            (uint32_t)macAddr[0] |
                            ((uint32_t)macAddr[1] << 8UL) |
                            ((uint32_t)macAddr[2] << 16UL) |
                            ((uint32_t)macAddr[3] << 24UL),
                            (uint32_t)macAddr[4] |
                            ((uint32_t)macAddr[5] << 8UL));
        }
    }
}

/**
    @code{.unparsed}
    BCM_ReportError();
    @endcode
*/
static void ETS_ErrorReport(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
#ifndef ULOG
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};

    BCM_ReportError(BCM_ETS_ID, ETS_COMP_INSTANCE_ID, aApiID,
            aErr, 4UL, values);
#endif
}

/**
    @code{.unparsed}
    if (i < MSGQ_SIZE) {
        return &ETS_Handle.cmds[i];
    }
    @endcode
*/
void* ETS_CmdGetMessage(uint32_t idx)
{
    void* pRet = NULL;

    if (idx < MSGQ_SIZE) {

        pRet = (void *)(&ETS_Handle.cmds[idx]);
    }
    return pRet;
}

/** @} */
