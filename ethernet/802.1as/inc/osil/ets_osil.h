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

/** @defgroup grp_ets_il Integration Guide
    @ingroup grp_ets

    @addtogroup grp_ets_il
    @{
    @section sec_ets_integration Integration Guide
    This page describes functions needed by an integrator. These need to be
    implemented by the integrator.

    ETS operates as an asynchronous event driven system. The ETS library
    implements the core logic for PDU parsing, creation and the various state
    machines for gPTP operation. The ETS OSIL provides a wrapper around the
    library and abstracts the interaction with the rest of the system. A single
    instance of the ETS module is required for operation in either endpoint or
    bridge mode.

    @section sec_comp_il_fsm State Machine
    The ETS OSIL does not have a state machine. It relies on the state machine
    in the library.

    @section sec_comp_il_seq Sequence Diagram
    <BR>
    The figure below depicts the typical flow for initialization and
    deinitialization
    @image html etscomp_init_deinit_seq.png
    The figure below depicts the typical flow of PDU transmission
    @image html etscomp_tx_seq.png
    The figure below depicts the typical flow of PDU transmit confirmation
    @image html etscomp_tx_conf_seq.png
    The figure below depicts the typical flow of PDU reception
    @image html etscomp_rx_seq.png
    The figure below depicts the flow for timer invocation
    @image html etscomp_timer.png
    The figure below depicts the typical flow of callback events
    @image html etscomp_event_seq.png

    @file ets_osil.h
    @brief Ethernet TimeSync library integration interfaces.
    This header file defines the integration interfaces for ETS library.
    @version 1.25 Imported from MS Word document
*/
#ifndef ETS_OSIL_H
#define ETS_OSIL_H

#include <nif.h>
#include <msg_queue.h>
#include <rpc_cmds.h>
#include <bcm_time.h>
#include "ets.h"

/**
    @name ETS IL API IDs
    @{
    @brief API IDs for ETS IL
 */
#define BRCM_SWARCH_ETS_EVENT_TYPE                               (0x80U) /**< @brief #ETS_Event */
#define BRCM_SWARCH_ETS_CMDTYPE_TYPE                             (0x81U) /**< @brief #ETS_CmdType */
#define BRCM_SWARCH_ETS_CMDRESP_TYPE                             (0x82U) /**< @brief #ETS_CmdRespType */
#define BRCM_SWARCH_ETS_RECORD_TYPE                              (0x83U) /**< @brief #ETS_RecordType */
#define BRCM_SWARCH_ETS_RECORDDEFS_TYPE                          (0x84U) /**< @brief #ETS_RecordDefsType */
#define BRCM_SWARCH_ETS_CMDRESPPAYLOADSIZE_MACRO                 (0x85U) /**< @brief #ETS_CMDRESP_PAYLOAD_SIZE */
#define BRCM_SWARCH_ETS_MAX_RECORDS_PER_IPC_MSG_MACRO            (0x86U) /**< @brief #ETS_MAX_RECORDS_PER_IPC */

#define BRCM_SWARCH_ETS_SENDMSG_PROC                  (0x8AU) /**< @brief #ETS_SendMsg */
#define BRCM_SWARCH_ETS_RECVRESP_PROC                 (0x8BU) /**< @brief #ETS_RecvResp */
#define BRCM_SWARCH_ETS_TRYCANCELMSG_PROC             (0x8CU) /**< @brief #ETS_TryCancelMsg */
#define BRCM_SWARCH_ETS_RXPKTINDICATION_PROC          (0x8DU) /**< @brief #ETS_RxPktIndication */
#define BRCM_SWARCH_ETS_TXPKTCONFIRMATION_PROC        (0x8EU) /**< @brief #ETS_TxPktConfirmation */
#define BRCM_SWARCH_ETS_RXMGMTINFOINDICATION_PROC     (0x8FU) /**< @brief #ETS_RxMgmtInfoIndication */
#define BRCM_SWARCH_ETS_RXPKTTSINDICATION_PROC        (0x90U) /**< @brief #ETS_RxPktTSIndication */
#define BRCM_SWARCH_ETS_TXPKTTSINDICATION_PROC        (0x91U) /**< @brief #ETS_TxPktTSIndication */
#define BRCM_SWARCH_ETS_TRANSMIT_PROC                 (0x92U) /**< @brief #ETS_Transmit */
#define BRCM_SWARCH_ETS_CONFIGSAVECALLBACK_PROC       (0x93U) /**< @brief #ETS_ConfigSaveCallback */
#define BRCM_SWARCH_ETS_EVENTCALLBACK_PROC            (0x94U) /**< @brief #ETS_EventCallback */
#define BRCM_SWARCH_ETS_SHELL_PROC                    (0x95U) /**< @brief #ETS_Shell */
#define BRCM_SWARCH_ETS_TIMERALARMCB_PROC             (0x96U) /**< @brief #ETS_TimerAlarmCb */
#define BRCM_SWARCH_ETS_RECORDCB_PROC                 (0x97U) /**< @brief #ETS_RecordCallback */
/** @} */

/**
    @brief  Size of the payload in the message queue command response
*/
#define ETS_CMDRESP_PAYLOAD_SIZE                 (RPC_RESPPAYLOADSZ)

/**
    @brief Maximum number of event records per IPC message

*/
#define ETS_MAX_RECORDS_PER_IPC_MSG      (((RPC_RESPPAYLOADSZ) - sizeof(uint32_t)) / sizeof(ETS_RecordType))

/**
  @name ETS_Event
  @{
  @brief Event reported by ETS

*/
typedef uint8_t ETS_EventType;                  /**< @brief typedef for Events
                                                     reported by ETS */
#define ETS_EVENT_IF_UP                    (0U) /**< @brief gPTP up on the interface */
#define ETS_EVENT_IF_DOWN                  (1U) /**< @brief gPTP down on the interface */
#define ETS_EVENT_SYNC                     (2U) /**< @brief Synced to network time */
#define ETS_EVENT_SYNC_LOST                (3U) /**< @brief Network time Sync lost */
#define ETS_EVENT_IF_LOST_RESP_EXCEEDED    (4U) /**< @brief PDelay lost
                                                      responses exceeded */
#define ETS_EVENT_IF_LOST_RESP_RECOVERED   (5U) /**< @brief Interface has
                                                       resumed receiving PDelay
                                                       response */
#define ETS_EVENT_IF_PDELAY_CHANGED        (6U) /**< @brief PDelay changed by more than
                                                      100u on the interface */
#define ETS_EVENT_IF_RECORD                (7U) /**< @brief  Record notification */
/** @} */

/**
    @name ETS_CmdType
    @{
    @brief Command identifiers for the ETS module

    @trace #BRCM_SWARCH_ETS_CMDTYPE_TYPE
    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef uint8_t ETS_CmdType;           /**< @brief typedef for ETS Command */
#define ETS_CMD_INIT             (0U)  /**< @brief Initialization command */
#define ETS_CMD_DEINIT           (1U)  /**< @brief De-Initialization command */
#define ETS_CMD_RESETSTATS       (2U)  /**< @brief Reset interface statistics command */
#define ETS_CMD_SETCONFIG        (3U) /**< @brief Set configuration */
#define ETS_CMD_GETCONFIG        (4U) /**< @brief Get configuration */
#define ETS_CMD_GETGLOBALSTATUS  (5U) /**< @brief Get global status */
#define ETS_CMD_GETINTFSTATUS    (6U) /**< @brief Get interface status */
#define ETS_CMD_SETGLOBALTIME    (7U) /**< @brief Set global time */
#define ETS_CMD_NOTIFY_RECORD    (8U) /**< @brief Start/Stop sending records to host */
/** @} */

/**
    @name ETS_RecordDefsType
    @{
    @brief Definitions for Record

*/
typedef uint32_t ETS_RecordDefsType;                            /**< @brief typedef for Record definitions */
#define ETS_RECORD_NUM_MASK                 (0xFFFFFF00UL)      /**< @brief Mask for the recordNum field in the portAndRecNum parameter */
#define ETS_RECORD_NUM_SHIFT                (8UL)               /**< @brief Bit shift for the recordNum field in the portAndRecNum parameter */
#define ETS_RECORD_PORT_NUM_MASK            (0x000000FFUL)      /**< @brief Mask for the portIdx field in the portAndRecNum parameter */
#define ETS_RECORD_PORT_NUM_SHIFT           (0UL)               /**< @brief Bit shift for the portIdx field in the portAndRecNum parameter */
#define ETS_RECORD_IS_VALID_MASK            (0x80000000UL)      /**< @brief Mask for the valid field in the flag parameter */
#define ETS_RECORD_IS_VALID_SHIFT           (31UL)              /**< @brief Bit shift for the valid field in the flag parameter */
#define ETS_RECORD_ROLE_MASK                (0x60000000UL)      /**< @brief Mask for the role field in the flag parameter */
#define ETS_RECORD_ROLE_SHIFT               (29UL)              /**< @brief Bit shift for the role field in the flag parameter */
#define ETS_RECORD_GM_STATUS_MASK           (0x1C000000UL)      /**< @brief Mask for the gmStatus field in the flag parameter */
#define ETS_RECORD_GM_STATUS_SHIFT          (26UL)              /**< @brief Bit shift for the gmStatus field in the flag parameter */
#define ETS_RECORD_IS_LINK_UP_MASK          (0x02000000UL)      /**< @brief Mask for the isLinkUp field in the flag parameter */
#define ETS_RECORD_IS_LINK_UP_SHIFT         (25UL)              /**< @brief Bit shift for the isLinkUp field in the flag parameter */
#define ETS_RECORD_IS_TX_MASK               (0x01000000UL)      /**< @brief Mask for the isTx field in the flag parameter */
#define ETS_RECORD_IS_TX_SHIFT              (24UL)              /**< @brief Bit shift for the isTx field in the flag parameter */
#define ETS_RECORD_PACKET_TYPE_MASK         (0x00F00000UL)      /**< @brief Mask for the packetType field in the flag parameter */
#define ETS_RECORD_PACKET_TYPE_SHIFT        (20UL)              /**< @brief Bit shift for the packetType field in the flag parameter */
#define ETS_RECORD_RESERVED_MASK            (0x000F0000UL)      /**< @brief Mask for the reserved field in the flag parameter */
#define ETS_RECORD_RESERVED_SHIFT           (16UL)              /**< @brief Bit shift for the reserved field in the flag parameter */
#define ETS_RECORD_SEQ_NUM_MASK             (0x0000FFFFUL)      /**< @brief Mask for the seqNum field in the flag parameter */
#define ETS_RECORD_SEQ_NUM_SHIFT            (0UL)               /**< @brief Bit shift for the seqNum field in the flag parameter */
/** @} */

/**
    @brief Record
*/
typedef struct _ETS_RecordType {
    BCM_TimeType           gmTime;             /**< @brief Calculated or received gPTP-Time (reference GM) */
    BCM_TimeType           localTime;          /**< @brief Local clock timestamp */
    uint32_t               portAndRecNum;      /**< @brief combined value of port index and recordNum
                                                    encoded in a bitfield as follows:
                                                    recordNum(24) | portIdx (8)*/
    uint32_t               flag;               /**< @brief combined value of all flags
                                                    encoded in a bitfield as follows:
                                                    valid(1) | role (2) | gmStatus (3) | isLinkUp (1) | isTx (1) | packetType (4) | reserved (4) | seqNum (16)*/
} ETS_RecordType;

/**
    @brief Collection of records per IPC message
*/
typedef struct _ETS_RecordsListType {
    uint32_t                 numRecords;
    ETS_RecordType records[ETS_MAX_RECORDS_PER_IPC_MSG];
} ETS_RecordsListType;

/**
    @brief Port statistics and status
*/
typedef struct _ETS_PortStatsAndStatusType {
    uint32_t           num;    /**< @brief Port number */
    ETS_StatsType      stats;  /**< @brief Port statistics */
    ETS_IntfStatusType status; /**< @brief Port status */
} ETS_PortStatsAndStatusType;

/**
    @brief Port PDelay and Neighbour Rate Ratio
*/
typedef struct _ETS_PortPDelayAndNBRRType {
    uint32_t port;     /**< @brief Port number */
    uint32_t pDelay;  /**< @brief Peer Delay */
    int32_t  nbrr;    /**< @brief Neighbour Rate Ratio */
} ETS_PortPDelayAndNBRRType;

/**
    @brief Payload union for command response
*/
typedef union _ETS_CmdRespPayloadType {
    uint8_t                       *u8Ptr;            /**< @brief Pointer to byte stream */
    ETS_ConfigType                *config;           /**< @brief Pointer to configuration */
    ETS_PortStatsAndStatusType    *portStatus;       /**< @brief Pointer to port status and statistics */
    ETS_GlobalStatusType          *globalStatus;     /**< @brief Pointer to global status */
    ETS_TimestampType             *time;             /**< @brief Pointer to time */
    uint32_t                      *port;             /**< @brief Pointer to port number */
    ETS_PortPDelayAndNBRRType     *pDelayNBRR;       /**< @brief Pointer to PDelay and Neighbour Rate Ratio */
    ETS_RecordsListType *records;                    /**< @brief Pointer to records */
    uint32_t                      *isNotifyingRecord;/**< @brief Pointer to
                                                          notification record
                                                          state */
} ETS_CmdRespPayloadType;

/**
    @brief Checks for the payload size
*/
_Static_assert(sizeof(ETS_ConfigType) <= ETS_CMDRESP_PAYLOAD_SIZE, "ETS_ConfigType does not fit in payload");
_Static_assert(sizeof(ETS_PortStatsAndStatusType) <= ETS_CMDRESP_PAYLOAD_SIZE, "ETS_PortStatsAndStatusType does not fit in payload");
_Static_assert(sizeof(ETS_GlobalStatusType) <= ETS_CMDRESP_PAYLOAD_SIZE, "ETS_GlobalStatusType does not fit in payload");
_Static_assert(sizeof(ETS_TimestampType) <= ETS_CMDRESP_PAYLOAD_SIZE, "ETS_TimestampType does not fit in payload");
_Static_assert(sizeof(ETS_PortPDelayAndNBRRType) <= ETS_CMDRESP_PAYLOAD_SIZE, "ETS_PortPDelayAndNBRRType does not fit in payload");
_Static_assert(sizeof(ETS_RecordsListType) <= RPC_RESPPAYLOADSZ, "ETS_RecordsListType does not fit in payload");

/**
    @brief Command-Response structure for command queue

    @trace #BRCM_SWARCH_ETS_CMDRESP_TYPE
    @trace #BRCM_SWREQ_ETS_TASK
*/
typedef struct _ETS_CmdRespType {
    ETS_CmdType            cmdId;                             /**< @brief Command identifier */
    int32_t                ret;                               /**< @brief Return status */
    uint8_t                payload[ETS_CMDRESP_PAYLOAD_SIZE]; /**< @brief Space for command response payload */
    ETS_CmdRespPayloadType payloadPtr;                        /**< @brief Pointer to payload */
} ETS_CmdRespType;

/** @brief Send a command to ETS protocol task

    @pre ETS protocol task is active

    Send a command to ETS Protocol Task

    @behavior Sync, Re-entrant

    @param[in]   aCmdMsg               Pointer to ETS message
    @param[in]   aClientMask           Event mask of the task which has to be
                                       notified on command completion
    @param[out]  aMsgHdr               Pointer to unique message header for the message
                                       sent, this message header shall be used to:
                                       - Query the result of the operation using
                                       #ETS_RecvResp or
                                       - Cancel the operation using #ETS_TryCancelMsg
    @retval  #BCM_ERR_OK               Message is successfully queued
    @retval  Others                    As returned from #MSGQ_SendMsg
    @retval  #BCM_ERR_INVAL_PARAMS     @c aMsgHdr is NULL
    @retval  #BCM_ERR_INVAL_PARAMS     @c aCmdMsg is NULL
    @retval  #BCM_ERR_INVAL_PARAMS     @c aClientMask is 0

    @post None

    @limitations None
*/
int32_t ETS_SendMsg (const ETS_CmdRespType* const aCmdMsg,
                     const uint32_t aClientMask,
                     const MSGQ_MsgHdrType ** const aMsgHdr);

/** @brief Receive response from ETS

    Interface to query the status of the message sent using #ETS_SendMsg and
    retrieve the processed message.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMsgHdr     Unique message header retrieved during
                                #ETS_SendMsg
    @param[out]     aResp       Pointer to retrieve processed message

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is processed and response is
                                        copied to @c aResp
    @retval     Others                  As returned from #MSGQ_RecvMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aResp is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL

    @post TBD

    @limitations None
*/
int32_t ETS_RecvResp (const MSGQ_MsgHdrType* const aMsgHdr,
                      ETS_CmdRespType* const aResp);

/** @brief Try cancelling ETS message

    Interface to cancel the message which is previously send using
    #ETS_SendMsg.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aMsgHdr     Unique message header retrieved during
                                #ETS_SendMsg.
    @param[out]     aResp       Pointer to retrieve processed ETS message,
                                if processing is already complete.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Message is processed and response is
                                        already copied to @c aResp. In this case,
                                        client shall not wait for signal from ETS
    @retval     Others                  As returned from #MSGQ_TryCancelMsg
    @retval     #BCM_ERR_INVAL_PARAMS   @c aResp is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @c aMsgHdr is NULL

    @post TBD

    @limitations None
*/
int32_t ETS_TryCancelMsg(const MSGQ_MsgHdrType* const aMsgHdr,
                         ETS_CmdRespType* const aResp);

/** @brief Receive gPTP PDU

    @pre None

    Invoked by the network interface to handover gPTP PDU to the module
    which has registered for the gPTP ethertype

    @behavior Sync, Non-reentrant

    @param[in]   aCtrlIdx           NIF controller identifier
    @param[in]   aPktBuf            Pointer to packet buffer
    @param[in]   aPktLen            Length of packet
    @param[in]   aSrcMacAddr        Source MAC addrss of packet
    @param[in]   aDestMacAddr       Destination MAC address of packet
    @param[in]   aPriority          Priority of L2 packet

    @return      void

    @post None

    @limitations To be invoked by a higher priority task than ETS protocol task
*/
void ETS_RxPktIndication (NIF_CntrlIDType aCtrlIdx,
                          const uint8_t *const aPktBuf,
                          uint16_t aPktLen,
                          const uint8_t *const aSrcMacAddr,
                          const uint8_t *const aDestMacAddr,
                          uint32_t aPriority);

/** @brief gPTP PDU transmit confirmation

    @pre None

    Invoked by the network interface to inform that a gPTP PDU was
    successfully transmitted

    @behavior Sync, Non-reentrant

    @param[in]   aCtrlIdx           NIF controller identifier
    @param[in]   aBufIdx            Buffer identifier for the packet

    @return      void

    @post None

    @limitations To be invoked by a higher priority task than ETS protocol task
*/
void ETS_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                            uint32_t aBufIdx);

/** @brief Persist the Pdelay and Rate Ratio in NVM

    @pre None

    Peer delay and neighbor rate ratio for an interface to be stored in
    NVM. This is expected to be implemented by the integration layer

    @behavior Sync, Non-reentrant

    @param[in]   aPortIdx           Port number
    @param[in]   aPdelay            Peer delay on the interface
    @param[in]   aRateRatio         Neighbor rate ratio on the interface

    @return      void

    @post None

    @limitations This is invoked in the context of the ETS protocol task and
    care should be taken to not perform any time consuming operations which
    could affect protocol performance
*/
void ETS_ConfigSaveCallback (const uint32_t aPortIdx,
                             const uint32_t aPdelay,
                             const int32_t  aRateRatio);

/** @brief Receive packet timestamp callback

    @pre ETS_RxPktIndication() should have been invoked with the same
    identifiers i.e. aCtrlIdx & aBuf. When operating in bridge mode,
    ETS_RxMgmtInfoIndication() should also have been invoked with the same
    identifiers i.e. aCtrlIdx, aBuf and aMgmtInfo

    Invoked by the network interface with the ingress hardware timestamp
    corresponding to a received packet

    @behavior Sync, Non-reentrant

    @param[in]   aCtrlIdx        Controller index
    @param[in]   aBuf            Pointer to the packet buffer
    @param[in]   aMgmtInfo       Pointer to management information
    @param[in]   aTS             Pointer to timestamp
    @param[in]   aTSQual         Pointer to quality of captured timestamp

    @return      void

    @post None

    @limitations To be invoked by a higher priority task than ETS protocol task
*/
void ETS_RxPktTSIndication(NIF_CntrlIDType aCtrlIdx,
                     const uint8_t *const aBuf,
                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                     const ETHER_TimestampType *const aTS,
                     const ETHER_TimestampQualType* const aTSQual);

/** @brief Transmit packet timestamp callback

    @pre ETS_TxPktConfirmation() should have been invoked with the same
    identifiers i.e. aCtrlIdx & aBufIdx

    Invoked by the network interface with the egress hardware timestamp
    corresponding to a transmitted packet

    @behavior Sync, Non-reentrant

    @param[in]   aCtrlIdx        Controller index
    @param[in]   aBuf            Pointer to the packet buffer
    @param[in]   aMgmtInfo       Pointer to management information
    @param[in]   aTS             Pointer to timestamp
    @param[in]   aTSQual         Pointer to quality of captured timestamp

    @return      void

    @post None

    @limitations To be invoked by a higher priority task than ETS protocol task
*/
void ETS_TxPktTSIndication(NIF_CntrlIDType aCtrlIdx,
                     uint32_t aBufIdx,
                     const ETHERSWT_MgmtInfoType *const aMgmtInfo,
                     const ETHER_TimestampType *const aTS,
                     const ETHER_TimestampQualType* const aTSQual);

/** @brief Receive packet management information callback

    @pre ETS_RxPktIndication() should have been invoked with the same
    identifiers i.e. aCtrlIdx & aBuf

    Invoked by the network interface with management information of
    each RX packet. This is applicable when operating as a bridge.

    @behavior Sync, Non-reentrant

    @param[in]   aCtrlIdx    Controller index in the context of NIF
    @param[in]   aBuf        Pointer to the packet buffer
    @param[in]   aMgmtInfo   Pointer to management information

    @return      void

    @post None

    @limitations To be invoked by a higher priority task than ETS protocol task
*/
void ETS_RxMgmtInfoIndication(NIF_CntrlIDType aCtrlIdx,
                              const uint8_t *const aBuf,
                              const ETHERSWT_MgmtInfoType *const aMgmtInfo);

/** @brief Event notification from the module

    @pre None

    Notifies the higher layers of gPTP events. This is expected to be
    implemented by the integration layer

    @behavior Sync, Non-reentrant

    @param[in]   aPortIdx           Port number
    @param[in]   aEvent             Event to be notified

    @return      void

    @post None

    @limitations This is invoked in the context of the ETS protocol task and
    care should be taken to not perform any time consuming operations which
    could affect protocol performance
*/
void ETS_EventCallback (const uint32_t aPortIdx,
                        const ETS_EventType aEvent);

/** @brief Transmit 802.1as PDU

    @pre None

    Transmits the provided L2 buffer. Enables timestamping for event messages
    and adds an entry to the transmit confirmation wait queue.
    For bridge mode also sets the management information.

    @behavior Sync, Non-reentrant

    @param[in]  aIntfNum                Interface number
    @param[in]  aCtrlIdx                NIF controller index
    @param[in]  aBufIdx                 Identifier of the L2 buffer to be sent
    @param[in]  aLen                    Length of PDU in the buffer
    @param[in]  aPktType                gPTP packet type
    @param[in]  aIsEventMsg             Is PDU an event messageg
    @param[in]  aDstMacAddr             Destination MAC address for the PDU

    @return  #BCM_ERR_OK                Transmit succeeded
    @return  #BCM_ERR_NOMEM             The waitTxQ is full
    @return  #BCM_ERR_INVAL_PARAMS      Buffer index is invalid
    @return  #BCM_ERR_BUSY              Network interface queue full
    @return  #BCM_ERR_INVAL_STATE       Network interface is in invalid state
    @return  #BCM_ERR_INVAL_BUF_STATE   Buffer is in invalid state

    @post None

    @limitations This is invoked in the context of the ETS protocol task and
    care should be taken to not perform any time consuming operations which
    could affect protocol performance
*/
int32_t ETS_Transmit(const uint32_t        aIntfNum,
                     const NIF_CntrlIDType aCtrlIdx,
                     const uint32_t        aBufIdx,
                     const uint32_t        aLen,
                     const uint32_t        aPktType,
                     const uint32_t        aIsEventMsg,
                     const uint8_t* const  aDstMacAddr);

/** @brief Timer Alarm Callback

    Callback from the OS on expiry of the ETS timer alarm

    @behavior Sync, Non-reentrant

    @pre None

    @return      void

    @post None

    @limitations None
*/
void ETS_TimerAlarmCb(void);

/** @brief Callback reporting the records for an interface

    @pre None

    Report containing records for all packet types for an interface
    Invoked once every 31.25ms

    @behavior Sync, Non-reentrant

    @param[in]  aRecord          Pointer to record

    @return  void

    @post None

    @note This routine needs to be implemented by the integrator
*/
void ETS_RecordCallback(ETS_RecordType *aRecord);

/** @brief Handle shell command

    @pre None

    This interface is invoked by the Console command handler to handle input
    gPTP commands

    @behavior Sync, Non-reentrant

    @param[in]   aConsoleID           Console identifier
    @param[in]   aArgString           String containing input shell command
    @param[in]   aArgStringLen        Length of aArgString

    @retval      void

    @post None

    @limitations User log buffer is used for responding to the input command
*/
void ETS_Shell(uint32_t aConsoleID,
               char* aArgString,
               uint32_t aArgStringLen);

#endif /* ETS_OSIL_H*/
/** @} */
