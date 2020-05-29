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
  @addtogroup grp_ets_ifc
  @{
  @file ets_status.h
  @brief Status maintained by ETS subsystem

  This file provides the definition of status of Ethernet TimeSync which
  can be retrieved using the ETS_GetGlobalStatus() and ETS_GetIntfStatus()
  API.

  @version 1.25 Imported from MS Word document
*/

#ifndef ETS_STATUS_H
#define ETS_STATUS_H

/**
    @name ETS Library API IDs
    @{
    @brief API IDs for ETS Library
 */
#define BRCM_SWARCH_ETS_GLBLSTATUSMAGIC_MACRO (0x20U) /**< @brief #ETS_GLBL_STATUS_MAGIC_ID */
#define BRCM_SWARCH_ETS_INTFSTATUSMAGIC_MACRO (0x21U) /**< @brief #ETS_INTF_STATUS_MAGIC_ID */
#define BRCM_SWARCH_ETS_CLOCKIDLEN_MACRO      (0x22U) /**< @brief #ETS_CLOCK_ID_LEN */
#define BRCM_SWARCH_ETS_GMSTATUS_TYPE         (0x23U) /**< @brief #ETS_GMStatusType */
#define BRCM_SWARCH_ETS_CLOCKSTATE_TYPE       (0x24U) /**< @brief #ETS_ClockStateType */
#define BRCM_SWARCH_ETS_TIMESTAMPSTATE_TYPE   (0x25U) /**< @brief #ETS_TimestampStateType */
#define BRCM_SWARCH_ETS_MSG_TYPE              (0x26U) /**< @brief #ETS_MsgType */
#define BRCM_SWARCH_ETS_TIMESTAMP_TYPE        (0x27U) /**< @brief #ETS_TimestampType */
#define BRCM_SWARCH_ETS_CLOCKIDENTITY_TYPE    (0x28U) /**< @brief #ETS_ClockIdentityType */
#define BRCM_SWARCH_ETS_GLOBALSTATUS_TYPE     (0x29U) /**< @brief #ETS_GlobalStatusType */
#define BRCM_SWARCH_ETS_INTFSTATUS_TYPE       (0x2AU) /**< @brief #ETS_IntfStatusType */
/** @} */

/**
    @brief Magic number for the status structure containing 'ETSG'
*/
#define ETS_GLBL_STATUS_MAGIC_ID  (0x47535445UL)

/**
    @brief Magic number for the status structure containing 'ETSI'
*/
#define ETS_INTF_STATUS_MAGIC_ID  (0x49535445UL)

/**
    @brief Length of clock identity - 8 octets
*/
#define ETS_CLOCK_ID_LEN           (8U)

/**
    @name ETS_GMStatusType
    @{
    @brief Grandmaster status

*/
typedef uint32_t ETS_GMStatusType;        /**< @brief typedef for grandmaster status */
#define ETS_GMSTATUS_STARTUP_ABSENT (0UL) /**< @brief GM absent since startup and
                                               the Sync absence timer has
                                               expired */
#define ETS_GMSTATUS_OPER_ABSENT    (1UL) /**< @brief GM has been absent after
                                               startup */
#define ETS_GMSTATUS_UNDETERMINED   (2UL) /**< @brief GM status cannot be
                                               determined because Sync absence
                                               timeout has not yet occured */
#define ETS_GMSTATUS_DISABLED       (3UL) /**< @brief Slave port not asCapable */
#define ETS_GMSTATUS_NO_SLAVEPORT   (4UL) /**< @brief No slave port */
#define ETS_GMSTATUS_PRESENT        (5UL) /**< @brief GM is currently present */
/** @} */

/**
    @name ETS_ClockStateType
    @{
    @brief Network clock status

*/
typedef uint32_t ETS_ClockStateType;      /**< @brief typedef for clock status */
#define ETS_CLOCKSTATE_INIT_GM      (0UL) /**< @brief Clock is yet to be
                                               initialized as GM */
#define ETS_CLOCKSTATE_UPDATE_GM    (1UL) /**< @brief Clock is initialized as
                                               GM */
#define ETS_CLOCKSTATE_INIT_SLAVE   (2UL) /**< @brief Clock is yet to be
                                               initialized as slave */
#define ETS_CLOCKSTATE_UPDATE_SLAVE (3UL) /**< @brief Clock is initialized as
                                               slave */
/** @} */

/**
    @name ETS_TimestampStateType
    @{
    @brief State representing accuracy of the network timestamp

*/
typedef uint32_t ETS_TimestampStateType;    /**< @brief typedef for timestamp state */
#define ETS_TIMESTAMPSTATE_SYNC       (0UL) /**< @brief Local gPTP clock is
                                                  synced to Grandmaster */
#define ETS_TIMESTAMPSTATE_UNSYNC     (1UL) /**< @brief Local gPTP clock is
                                                  not synced to Grandmaster */

#define ETS_TIMESTAMPSTATE_UNCERTAIN  (2UL) /**< @brief Local gPTP clock is in
                                                  holdover */
/** @} */

/**
    @name ETS_MsgType
    @{
    @brief gPTP PDU message type

*/
typedef uint8_t ETS_MsgType;                   /**< @brief typedef for ETS PDU message type   */
#define ETS_MSG_SYNC                     (0U)  /**< @brief Message type of Sync PDU */
#define ETS_MSG_PDELAY_REQ               (2U)  /**< @brief Message type of PDelay Request PDU */
#define ETS_MSG_PDELAY_RESP              (3U)  /**< @brief Message type of PDelay Response PDU */
#define ETS_MSG_FOLLOWUP                 (8U)  /**< @brief Message type of Follow up PDU */
#define ETS_MSG_PDELAY_RESP_FOLLOWUP     (10U) /**< @brief Message type of PDelay Response Follow Up PDU */
#define ETS_MSG_SIGNALING                (12U) /**< @brief Message type of Signaling PDU */
/** @} */

/**
    @brief gPTP Timestamp

    Structure to carry time information represented with respect to the epoch

*/
typedef struct _ETS_TimestampType {
    uint64_t seconds;     /**< @brief Seconds component of time */

    uint32_t nanoseconds; /**< @brief Nanoseconds component of time */
} ETS_TimestampType;

/**
    @brief gPTP clock identity

    Clock identity identifies a time aware system. It is an 8-octet array formed
    by mapping an IEEE EUI-48 (System MAC address) assigned to the time-aware
    system to IEEE EUI-64 format (i.e., to an array of 8 octets).

*/
typedef struct _ETS_ClockIdentityType {
    uint8_t   id[ETS_CLOCK_ID_LEN]; /**< @brief clock identity bytes */
} ETS_ClockIdentityType;

/**
    @brief Global status

    Status maintained by the ETS. Can be retrieved using the
    ETS_GetGlobalStatus() API

*/
typedef struct _ETS_GlobalStatusType {
    uint32_t              magicId;                 /**< @brief Magic word 'ETSG'
                                                        (checked for validity) */

    ETS_GMStatusType      gmStatus;                /**< @brief Grandmaster status */

    int32_t               rateRatioIn;             /**< @brief Incoming rate
                                                        ratio */

    int32_t               gmRateRatio;             /**< @brief Grandmaster rate ratio */

    uint32_t              slavePort;               /**< @brief Port acting as
                                                        slave */

    uint16_t              signalingTxSeqId;        /**< @brief Sequence number for
                                                        transmitted signaling PDUs*/

    int32_t               reqSyncLogInterval;      /**< @brief The signaling sync
                                                        interval sent on the slave
                                                        interface */

    uint32_t              isSignalingTimerStarted; /**< @brief Flag indicating
                                                         whether the signaling timer
                                                         is started or not */

    uint32_t              isSyncReceived;          /**< @brief Flag indicating
                                                        whether atleast one sync has
                                                        been received */

    ETS_ClockStateType    clockState;              /**< @brief Clock state */

    ETS_ClockIdentityType clockId;                 /**< @brief Local clock
                                                        identity */

    ETS_TimestampType     networkTime;             /**< @brief Network time */

    ETS_TimestampStateType networkTimeState;        /**< @brief Network time state */
} ETS_GlobalStatusType;

/**
    @brief Interface status

    Interface status maintained by the ETS. Can be retrieved using the
    ETS_GetIntfStatus() API

*/
typedef struct _ETS_IntfStatusType {
    uint32_t          magicId;                    /**< @brief Magic word 'ETSI'
                                                       (checked for validity) */

    uint32_t          nbrPropDelay;               /**< @brief Peer delay */

    int32_t           nbrRateRatio;               /**< @brief Neighbour rate
                                                       ratio */

    uint32_t          isMeasuringPdelay;          /**< @brief Is Pdelay
                                                       requestor running */

    uint32_t          isAVnuPdelayConfigSaved;    /**< @brief Is the AVnu Pdelay
                                                       configuration saved to
                                                       NVM */

    int32_t           currentLogPdelayInterval;   /**< @brief Current Pdelay
                                                       transmit interval in
                                                       logarithmic scale */

    int32_t           currentLogSyncInterval;     /**< @brief Current Sync
                                                       transmit interval in
                                                       logarithmic scale */

    uint64_t          syncReceiptTimeoutInterval; /**< @brief Sync receipt
                                                       timeout interval */

    ETS_ClockIdentityType partnerClockId;             /**< @brief Peer clock
                                                       identifier */

    uint16_t          partnerPortId;              /**< @brief Peer port
                                                       identifier */

    uint16_t          pDelayTxSeqId;              /**< @brief Pdelay requestor
                                                       sequence number */

    uint16_t          syncTxSeqId;                /**< @brief Sync transmit
                                                       sequence number */

    uint32_t          syncInfoAvailable;          /**< @brief New Sync
                                                       information received on
                                                       slave port */

    uint16_t          syncLastRxSeqId;            /**< @brief Last received Sync
                                                       sequence number on slave
                                                       port */

    int64_t           rxFollowupCorrection;       /**< @brief Incoming
                                                       correction */

    uint64_t          txFollowupCorrection;       /**< @brief Outgoing
                                                       correction */

    int8_t            rxPduInterval;              /**< @brief Received Sync
                                                       interval */

    ETS_TimestampType rxPOT;                      /**< @brief Received Precise
                                                       Origin Timestamp */

    uint32_t          numPdelayRespLost;          /**< @brief Number of
                                                       consecutive pdelay
                                                       timeouts */

    uint32_t          pDelayReqState;             /**< @brief PDelay requestor
                                                       state */

    uint32_t          pDelayRespState;            /**< @brief PDelay responder
                                                       state */

    uint32_t          syncTxState;                /**< @brief Sync master
                                                       state */

    uint32_t          syncRxState;                /**< @brief Sync slave state*/

    ETS_TimestampType syncTxTimestamp;            /**< @brief Sync egress
                                                       timestamp */

    ETS_TimestampType syncRxTimestamp;            /**< @brief Sync ingress
                                                       timestamp */

    ETS_TimestampType pDelayT1;                   /**< @brief PDelay request
                                                       egress timestamp */

    ETS_TimestampType pDelayT2;                   /**< @brief PDelay request
                                                       ingress timestamp */

    ETS_TimestampType pDelayT3;                   /**< @brief PDelay response
                                                       egress timestamp */

    ETS_TimestampType pDelayT4;                   /**< @brief PDelay response
                                                       ingress timestamp */
} ETS_IntfStatusType;
#endif /* ETS_STATUS_H */
/** @} */
