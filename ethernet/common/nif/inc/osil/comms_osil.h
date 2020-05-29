/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
 * @file comms_osil.h
 * @brief This file specifes common interfaces for communication devices.
 */

#ifndef COMMS_OSIL_H
#define COMMS_OSIL_H

#include <msg_queue.h>
#include <comms_ipc.h>
#include "ee.h"

/** Bottom-half task for Ethernet */
#define ETH_IRQ_PROCESS_TASK    CommsTask

/** COMMS command processing task */
#define COMMS_COMMAND_PROC_TASK CommsTask

/** Bottom-half task for Ethernet */
#define CAN_IRQ_PROCESS_TASK    CommsTask

/** Bottom-half task for Switch interrupt processing */
#define ETHERSWT_IRQ_PROCESS_TASK   CommsTask

/** Event for Ethernet bottomhalf processing */
#define ETHER_PKT_EVENT         SystemEvent0

/** Event for CAN bottomhalf processing */
#define CAN_EVENT               SystemEvent1

/** Event for Ethernet time hardware bottomhalf processing */
#define ETHER_TIMESTAMP_EVENT   SystemEvent2

/** Events for internet stack */
#define INET_TIMER_EVENT        SystemEvent3

/** Events for Ethernet Xcvr link state change */
#define ETHER_XCVR_LINK_EVENT   SystemEvent4

/** Event for Ethernet bottomhalf processing */
#define ETHER_TIMER_EVENT       SystemEvent5

/** Event for Comms MsgQ processing */
#define COMMS_MSGQ_CMD_EVENT    SystemEvent7

/** Events for Ethernet switch port link state change */
#if ENABLE_ETH_SWITCH
#define ETHERSWT_PORT_LINK_EVENT   SystemEvent6
#else
#define ETHERSWT_PORT_LINK_EVENT   (0UL)
#endif

/** Events for Ethernet switch ARL management */
#if ENABLE_ETH_SWITCH
#define ETHERSWT_ARL_REFRESH_EVENT   SystemEvent9
#else
#define ETHERSWT_ARL_REFRESH_EVENT   (0UL)
#endif

/** Events for Stream monitor */
#ifdef ENABLE_ETH_SWITCH
#define COMMS_STREAM_MONITOR_EVENT   SystemEvent12
#else
#define COMMS_STREAM_MONITOR_EVENT   (0UL)
#endif

/**
 * Common event for communication task. All communication devices shall
 * OR their individual events to generate final COMMS_EVENT.
 */
#define COMMS_EVENT         (ETHER_PKT_EVENT | CAN_EVENT| \
                             ETHER_TIMER_EVENT | ETHER_TIMESTAMP_EVENT | \
                             INET_TIMER_EVENT | ETHER_XCVR_LINK_EVENT |\
                             MDIO_STATE_HANDLER_EVENT | \
                             ETHERSWT_PORT_LINK_EVENT | \
                             COMMS_MSGQ_CMD_EVENT | COMMS_STREAM_MONITOR_EVENT |\
                             ETHERSWT_ARL_REFRESH_EVENT)

/** @brief ETS Initialization API

    This API performs the initialization of the ETS module.

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    @return     void

    @post None

    @note To be implemented.

    @limitations None
*/
void COMMS_InitETS(void);

/** @brief Send commands to COMMS Command queue

    This API queues the commands to COMMS command queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aCmd                Command message
    @param[in]      aClientMask         Client mask to be set on command completion
    @param[out]     aHdr                Pointer to the message queue header used to
                                        retrieve the status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command posted successfully
    @retval     #BCM_ERR_NOMEM          Queue is Full and cannot be added to queue

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_AddToCmdQ(COMMS_CmdRespType *aCmd,
                        const uint32_t aClientMask,
                        const MSGQ_MsgHdrType** const aHdr);

/** @brief Get the response from the COMMS command queue

    This API gets the response from COMMS command queue.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]     aHdr                Pointer to the message queue header used to
                                       retrieve the status
    @param[out]    aResp               Pointer to the response payload

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK            Command posted successfully
    @retval     #BCM_ERR_INVAL_PARAMS  aMsgHdr is NULL or aResp is NULL

    @post None

    @limitations This API may not be supported on all the chipsets
*/
int32_t COMMS_RecvResp (const MSGQ_MsgHdrType* const aMsgHdr,
                        COMMS_CmdRespType* const aResp);

#endif /* COMMS_OSIL_H */
/* Nothing past this line */
