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
/******************************************************************************
 File Name: inet_cfg.c
 Descritpion: This file implements the end points and interfaces for INET stack.
******************************************************************************/

#include <stdint.h>
#include <utils.h>
#include <ee.h>
#include <bcm_err.h>
#include <ulog.h>
#include <ethernet.h>
#include <system.h>
#include <inet_cfg.h>
#include "eecfg.h"

/* MISRA */
#ifndef RTP_MAX_NUM_OUTPUT_PORTS
#define RTP_MAX_NUM_OUTPUT_PORTS (0UL)
#endif
#ifndef RTP_MAX_NUM_INPUT_PORTS
#define RTP_MAX_NUM_INPUT_PORTS (0UL)
#endif

/************** Configure Reassembly Buffers *****************/
DEFINE_INET_REASS_BUF((reass0), (reass0Buf), (reass0Q), (reass0QBuf), (reass0QIdx));
DEFINE_INET_REASS_BUF((reass1), (reass1Buf), (reass1Q), (reass1QBuf), (reass1QIdx));
DEFINE_INET_REASS_BUF((reass2), (reass2Buf), (reass2Q), (reass2QBuf), (reass2QIdx));
DEFINE_INET_REASS_BUF((reass3), (reass3Buf), (reass3Q), (reass3QBuf), (reass3QIdx));
DEFINE_INET_REASS_BUF((reass4), (reass4Buf), (reass4Q), (reass4QBuf), (reass4QIdx));
DEFINE_INET_REASS_BUF((reass5), (reass5Buf), (reass5Q), (reass5QBuf), (reass5QIdx));
DEFINE_INET_REASS_BUF((reass6), (reass6Buf), (reass6Q), (reass6QBuf), (reass6QIdx));
DEFINE_INET_REASS_BUF((reass7), (reass7Buf), (reass7Q), (reass7QBuf), (reass7QIdx));

static INET_ReassemblyBuffer* COMP_SECTION(".data.inet.cache")
reassemblyBufferPool[]=
{
    &reass0,
    &reass1,
    &reass2,
    &reass3,
    &reass4,
    &reass5,
    &reass6,
    &reass7
};

INET_ReassemblyBuffer **inet_reassembly_pool = reassemblyBufferPool;
const uint32_t reass_pool_sz =
           sizeof(reassemblyBufferPool)/sizeof(INET_ReassemblyBuffer*);

/* Allocate memory for ARP cache for HW index zero */
static INET_ARPEntry COMP_SECTION(".data.inet") cacheHwIdx0[INET_MAX_ENTRIES_ARPCACHE];
static INET_ARPEntry* COMP_SECTION(".data.inet") arpCaches[] =
{
    cacheHwIdx0,
};

const uint32_t cache_table_sz = sizeof(arpCaches)/sizeof(INET_ARPEntry*);

/***************** Configure  Interfaces *********************/
#if defined (RTPCOMP_LOOPBACK_TEST) || defined (RTPTS_LOOPBACK_TEST)
DEFINE_INET_INTERFACE((EthPhyIntfIT), (cacheHwIdx0), (EthPhyIntfITCfg), (EthPhyIntfITStats),
                      (ETHER_HW_ID_0), (0U), (0U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))));
#else

#if (0UL < RTP_MAX_NUM_INPUT_PORTS) || defined (INET_TEST) || defined(TFTP_TEST) || defined (ENABLE_RFS)
DEFINE_INET_INTERFACE((EthPhyIntfT), (cacheHwIdx0), (EthPhyIntfCfgT), (EthPhyIntfStatsT),
                      (ETHER_HW_ID_0), (0U), (0U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))));
#endif

#if (0UL < RTP_MAX_NUM_OUTPUT_PORTS)
DEFINE_INET_INTERFACE((EthPhyIntfL), (cacheHwIdx0), (EthPhyIntfCfgL), (EthPhyIntfStatsL),
                      (ETHER_HW_ID_0), (0U), (0U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))));
#endif

#endif

#ifdef INET_TEST
DEFINE_INET_INTERFACE((Vid1024), (cacheHwIdx0), (Vid1024Cfg), (Vid1024Stats),
                      (ETHER_HW_ID_0), (2U), (1024U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (3U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))));
#endif

#if defined(CAMERA_TUNING)
DEFINE_INET_INTERFACE((CmdParserIf), (cacheHwIdx0), (CmdParserCfg), (CmdParserStats),
                      (ETHER_HW_ID_0), (0U), (0U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))));
#endif
#if defined(CTN_ENABLE_LOGGING)
DEFINE_INET_INTERFACE((TunerLogIf), (cacheHwIdx0), (TunerLogCfg), (TunerLogStats),
                      (ETHER_HW_ID_0), (0U), (0U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                      (24U),
                      (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))));
#endif /* CTN_ENABLE_LOGGING */

static INET_SystemInterface* COMP_SECTION(".data.inet") interfaces[] = {
#if defined(INET_TEST)
    &Vid1024,
#endif
#if defined(CAMERA_TUNING)
    &CmdParserIf,
#endif
#if defined(CTN_ENABLE_LOGGING)
    &TunerLogIf,
#endif
#if defined (RTPCOMP_LOOPBACK_TEST) || defined (RTPTS_LOOPBACK_TEST)
    &EthPhyIntfIT,
#else
#if (0UL < RTP_MAX_NUM_INPUT_PORTS) || defined (INET_TEST) || defined(TFTP_TEST) || defined (ENABLE_RFS)
    &EthPhyIntfT,
#endif
#if (0UL < RTP_MAX_NUM_OUTPUT_PORTS)
    &EthPhyIntfL,
#endif
#endif
};

INET_SystemInterface **inet_interface_table = interfaces;

const uint16_t intf_table_sz = sizeof(interfaces)/sizeof(INET_SystemInterface*);

/****************** Configure End Points *********************/
#if defined (RTPTS_LOOPBACK_TEST)
DEFINE_INET_ENDPOINT((RTPTSClient1), (RTPTSClientCfg1), (RTPTSCtxt1), (RTPTSCtxtQ1),
                     (RTPTSCtxtQBuf1), (RTPTSCtxtQIdx1),
                     (BCM_MTL_ID), (INET_UDP_PAYLOAD_ID0), (0UL), (Test_TSRxTask), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (2UL), (5000U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                     (5000U), (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif

/* End point for loopback test where destination and source IP is same */
#if defined (INET_TEST) && ! defined (DISABLE_INET_LOOPBACK_TEST)
DEFINE_INET_ENDPOINT((Test1Client), (Test1ClientCfg), (Test1ClientCtxt), (Test1ClientCtxtQ),
                     (Test1ClientCtxtQBuf), (Test1ClientCtxtQIdx),
                     (0x1234), (INET_UDP_PAYLOAD_ID0), (0UL), (Test2Task), (UDPRxEvent),
                     (Test1Task), (UDPTxEvent), (4UL), (5000U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
DEFINE_INET_ENDPOINT((Test1ClientMC), (Test1ClientCfgMC), (Test1ClientCtxtMC), (Test1ClientCtxtQMC),
                     (Test1ClientCtxtQBufMC), (Test1ClientCtxtQIdxMC),
                     (0x1234), (INET_UDP_PAYLOAD_ID3), (0UL), (Test2Task), (SystemEvent4),
                     (Test1Task), (UDPTxEvent), (4UL), (5003U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
DEFINE_INET_ENDPOINT((Test1ClientBC), (Test1ClientCfgBC), (Test1ClientCtxtBC), (Test1ClientCtxtQBC),
                     (Test1ClientCtxtQBufBC), (Test1ClientCtxtQIdxBC),
                     (0x1234), (INET_UDP_PAYLOAD_ID4), (0UL), (Test2Task), (SystemEvent5),
                     (Test1Task), (UDPTxEvent), (4UL), (5004U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif

#if defined(TFTP_TEST)
DEFINE_INET_ENDPOINT((TFTPReadClient1), (TFTPReadClientCfg1), (TFTPReadClientCtxt1), (TFTPReadClientCtxtQ1),
                     (TFTPReadClientCtxtQBuf1), (TFTPReadClientCtxtQIdx1),
                     (0x1234), (INET_UDP_PAYLOAD_ID0), (0UL), (TFTPTest), (SystemEvent0),
                     (TFTPTest), (SystemEvent0), (2UL), (5000U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                     (69U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
DEFINE_INET_ENDPOINT((TFTPWriteClient1), (TFTPWriteClientCfg1), (TFTPWriteClientCtxt1), (TFTPWriteClientCtxtQ1),
                     (TFTPWriteClientCtxtQBuf1), (TFTPWriteClientCtxtQIdx1),
                     (0x1234), (INET_UDP_PAYLOAD_ID1), (0UL), (TFTPTest), (SystemEvent0),
                     (TFTPTest), (SystemEvent0), (2UL), (5001U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U))),
                     (69U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif

/* End point for sending packets out to a test PC */
#if defined (INET_TEST) && defined (DISABLE_INET_LOOPBACK_TEST)
DEFINE_INET_ENDPOINT((Test1Client), (Test1ClientCfg), (Test1ClientCtxt), (Test1ClientCtxtQ),
                     (Test1ClientCtxtQBuf), (Test1ClientCtxtQIdx),
                     (0x1234), (INET_UDP_PAYLOAD_ID0), (0UL), (Test2Task), (UDPRxEvent),
                     (Test1Task), (UDPTxEvent), (4UL), (5000U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif

#if defined(CAMERA_TUNING)
DEFINE_INET_ENDPOINT((CmdParserClient), (CmdParserClientCfg), (CmdParserClientCtxt), (CmdParserClientCtxtQ),
                     (CmdParserClientCtxtQBuf), (CmdParserClientCtxtQIdx),
                     (0x1111), (INET_UDP_PAYLOAD_ID0), (0UL), (CmdRcvCb), (UDPRxEvent),
                     (TaskCmdParser), (UDPTxEvent), (4UL), (58900U),
                     /* For sending out to PC with IP address 192.168.10.2  */
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (58900U), (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif
#if defined(CTN_ENABLE_LOGGING)
DEFINE_INET_ENDPOINT((TunerLogClient), (TunerLogClientCfg), (TunerLogClientCtxt), (TunerLogClientCtxtQ),
                     (TunerLogClientCtxtQBuf), (TunerLogClientCtxtQIdx),
                     (0x2222), (INET_UDP_PAYLOAD_ID1), (0UL), (Camera_TunerTask), (UDPRxEvent),
                     (Camera_TunerTask), (UDPTxEvent), (4UL), (58901U),
                     /* For sending out to PC with IP address 192.168.10.2  */
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (58901U), (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif /* CTN_ENABLE_LOGGING */

#ifdef INET_TEST
DEFINE_INET_ENDPOINT((Test2Client1), (Test2Client1Cfg), (Test2Client1Ctxt), (Test2Client1CtxtQ),
                     (Test2Client1CtxtQBuf), (Test2Client1CtxtQIdx),
                     (0x1234), (INET_UDP_PAYLOAD_ID1), (1UL), (Test3Client1Task), (UDPRxEvent),
                     (Test3Client1Task), (UDPTxEvent), (4UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDPLITE), (INET_DSCP_CS0));

DEFINE_INET_ENDPOINT((Test2Client2), (Test2Client2Cfg), (Test2Client2Ctxt), (Test2Client2CtxtQ),
                     (Test2Client2CtxtQBuf), (Test2Client2CtxtQIdx),
                     (0x1234), (INET_UDP_PAYLOAD_ID2), (1UL), (Test3Client2Task), (UDPRxEvent),
                     (Test3Client2Task), (UDPTxEvent), (4UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDPLITE), (INET_DSCP_CS0));
#endif

#if (0UL < RTP_MAX_NUM_INPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPSenderClient1), (RTPSenderClientCfg1), (RTPSenderCtxt1), (RTPSenderCtxtQ1),
                     (RTPSenderCtxtQBuf1), (RTPSenderCtxtQIdx1),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID0), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (2UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPSenderClient1), (RTCPSenderClientCfg1), (RTCPSenderCtxt1), (RTCPSenderCtxtQ1),
                     (RTCPSenderCtxtQBuf1), (RTCPSenderCtxtQIdx1),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID1), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (1UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (1UL < RTP_MAX_NUM_INPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPSenderClient2), (RTPSenderClientCfg2), (RTPSenderCtxt2), (RTPSenderCtxtQ2),
                     (RTPSenderCtxtQBuf2), (RTPSenderCtxtQIdx2),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID2), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (2UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPSenderClient2), (RTCPSenderClientCfg2), (RTCPSenderCtxt2), (RTCPSenderCtxtQ2),
                     (RTCPSenderCtxtQBuf2), (RTCPSenderCtxtQIdx2),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID3), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (1UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (2UL < RTP_MAX_NUM_INPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPSenderClient3), (RTPSenderClientCfg3), (RTPSenderCtxt3), (RTPSenderCtxtQ3),
                     (RTPSenderCtxtQBuf3), (RTPSenderCtxtQIdx3),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID4), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (2UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPSenderClient3), (RTCPSenderClientCfg3), (RTCPSenderCtxt3), (RTCPSenderCtxtQ3),
                     (RTCPSenderCtxtQBuf3), (RTCPSenderCtxtQIdx3),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID5), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (1UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (3UL < RTP_MAX_NUM_INPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPSenderClient4), (RTPSenderClientCfg4), (RTPSenderCtxt4), (RTPSenderCtxtQ4),
                     (RTPSenderCtxtQBuf4), (RTPSenderCtxtQIdx4),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID6), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (2UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPSenderClient4), (RTCPSenderClientCfg4), (RTCPSenderCtxt4), (RTCPSenderCtxtQ4),
                     (RTCPSenderCtxtQBuf4), (RTCPSenderCtxtQIdx4),
                     (BCM_RTS_ID), (INET_UDP_PAYLOAD_ID7), (0UL), (RTPS_Task), (UDPRxEvent),
                     (RTPS_Task), (UDPTxEvent), (1UL), (0U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (0UL < RTP_MAX_NUM_OUTPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPRcvrClient1), (RTPRcvrClientCfg1), (RTPRcvrCtxt1), (RTPRcvrCtxtQ1),
                     (RTPRcvrCtxtQBuf1), (RTPRcvrCtxtQIdx1),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID0), (0UL), (RTPR_Task), (SystemEvent4),
                     (RTPR_Task), (UDPTxEvent), (4UL), (5000U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5000U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPRcvrClient1), (RTCPRcvrClientCfg1), (RTCPRcvrCtxt1), (RTCPRcvrCtxtQ1),
                     (RTCPRcvrCtxtQBuf1), (RTCPRcvrCtxtQIdx1),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID1), (0UL), (RTPR_Task), (SystemEvent4),
                     (RTPR_Task), (UDPTxEvent), (1UL), (5001U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5001U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (1UL < RTP_MAX_NUM_OUTPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPRcvrClient2), (RTPRcvrClientCfg2), (RTPRcvrCtxt2), (RTPRcvrCtxtQ2),
                     (RTPRcvrCtxtQBuf2), (RTPRcvrCtxtQIdx2),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID2), (0UL), (RTPR_Task), (SystemEvent5),
                     (RTPR_Task), (UDPTxEvent), (4UL), (5002U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5002U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPRcvrClient2), (RTCPRcvrClientCfg2), (RTCPRcvrCtxt2), (RTCPRcvrCtxtQ2),
                     (RTCPRcvrCtxtQBuf2), (RTCPRcvrCtxtQIdx2),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID3), (0UL), (RTPR_Task), (SystemEvent5),
                     (RTPR_Task), (UDPTxEvent), (1UL), (5003U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5003U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (2UL < RTP_MAX_NUM_OUTPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPRcvrClient3), (RTPRcvrClientCfg3), (RTPRcvrCtxt3), (RTPRcvrCtxtQ3),
                     (RTPRcvrCtxtQBuf3), (RTPRcvrCtxtQIdx3),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID4), (0UL), (RTPR_Task), (SystemEvent6),
                     (RTPR_Task), (UDPTxEvent), (4UL), (5004U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5004U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPRcvrClient3), (RTCPRcvrClientCfg3), (RTCPRcvrCtxt3), (RTCPRcvrCtxtQ3),
                     (RTCPRcvrCtxtQBuf3), (RTCPRcvrCtxtQIdx3),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID5), (0UL), (RTPR_Task), (SystemEvent6),
                     (RTPR_Task), (UDPTxEvent), (1UL), (5005U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5005U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if (3UL < RTP_MAX_NUM_OUTPUT_PORTS)
DEFINE_INET_ENDPOINT((RTPRcvrClient4), (RTPRcvrClientCfg4), (RTPRcvrCtxt4), (RTPRcvrCtxtQ4),
                     (RTPRcvrCtxtQBuf4), (RTPRcvrCtxtQIdx4),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID6), (0UL), (RTPR_Task), (SystemEvent7),
                     (RTPR_Task), (UDPTxEvent), (4UL), (5006U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5006U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
DEFINE_INET_ENDPOINT((RTCPRcvrClient4), (RTCPRcvrClientCfg4), (RTCPRcvrCtxt4), (RTCPRcvrCtxtQ4),
                     (RTCPRcvrCtxtQBuf4), (RTCPRcvrCtxtQIdx4),
                     (BCM_RTR_ID), (INET_UDP_PAYLOAD_ID7), (0UL), (RTPR_Task), (SystemEvent7),
                     (RTPR_Task), (UDPTxEvent), (1UL), (5007U),
                     (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (2U))),
                     (5007U), (INET_L4PROTO_UDP), (INET_DSCP_AF41));
#endif
#endif

#if defined(ENABLE_RFS)
DEFINE_INET_ENDPOINT((RFSRead), (RFSReadCfg), (RFSReadCtxt), (RFSReadCtxtQ),
                     (RFSReadCtxtQBuf), (RFSReadCtxtQIdx),
                     (BCM_RFS_ID), (INET_UDP_PAYLOAD_ID0), (0UL), (RFS_Task), (RFS_INET_EVENT_ID),
                     (RFS_Task), (RFS_INET_EVENT_ID), (4UL), (5000U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
DEFINE_INET_ENDPOINT((RFSWrite), (RFSWriteCfg), (RFSWriteCtxt), (RFSWriteCtxtQ),
                     (RFSWriteCtxtQBuf), (Test1ClientCtxtQIdx),
                     (BCM_RFS_ID), (INET_UDP_PAYLOAD_ID1), (0UL), (RFS_Task), (RFS_INET_EVENT_ID),
                     (RFS_Task), (RFS_INET_EVENT_ID), (4UL), (5001U), (0UL), (0U),
                     (INET_L4PROTO_UDP), (INET_DSCP_CS4));
#endif

static const INET_SystemEndPoint* const COMP_SECTION(".rodata.inet") endPoints[] = {
#ifdef INET_TEST
    &Test2Client1,
    &Test2Client2,
    &Test1Client,
    &Test1ClientMC,
    &Test1ClientBC,
#endif
#if defined(RTPTS_LOOPBACK_TEST)
    &RTPTSClient1,
#endif
#if defined(CAMERA_TUNING)
    &CmdParserClient,
#endif
#if defined(CTN_ENABLE_LOGGING)
    &TunerLogClient,
#endif /* CTN_ENABLE_LOGGING */
#if (0UL < RTP_MAX_NUM_OUTPUT_PORTS)
    &RTPRcvrClient1,
#endif
#if (1UL < RTP_MAX_NUM_OUTPUT_PORTS)
    &RTPRcvrClient2,
#endif
#if (2UL < RTP_MAX_NUM_OUTPUT_PORTS)
    &RTPRcvrClient3,
#endif
#if (3UL < RTP_MAX_NUM_OUTPUT_PORTS)
    &RTPRcvrClient4,
#endif

#if (0UL < RTP_MAX_NUM_OUTPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPRcvrClient1,
#endif
#if (1UL < RTP_MAX_NUM_OUTPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPRcvrClient2,
#endif
#if (2UL < RTP_MAX_NUM_OUTPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPRcvrClient3,
#endif
#if (3UL < RTP_MAX_NUM_OUTPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPRcvrClient4,
#endif

#if (0UL < RTP_MAX_NUM_INPUT_PORTS)
    &RTPSenderClient1,
#endif
#if (1UL < RTP_MAX_NUM_INPUT_PORTS)
    &RTPSenderClient2,
#endif
#if (2UL < RTP_MAX_NUM_INPUT_PORTS)
    &RTPSenderClient3,
#endif
#if (3UL < RTP_MAX_NUM_INPUT_PORTS)
    &RTPSenderClient4,
#endif

#if (0UL < RTP_MAX_NUM_INPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPSenderClient1,
#endif
#if (1UL < RTP_MAX_NUM_INPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPSenderClient2,
#endif
#if (2UL < RTP_MAX_NUM_INPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPSenderClient3,
#endif
#if (3UL < RTP_MAX_NUM_INPUT_PORTS) && (0UL < RTP_ENABLE_RTCP_1733_SUPPORT)
    &RTCPSenderClient4,
#endif
#if defined(TFTP_TEST)
    &TFTPReadClient1,
    &TFTPWriteClient1,
#endif
#if defined(ENABLE_RFS)
    &RFSRead,
    &RFSWrite,
#endif
};

const uint16_t endpoint_table_sz = sizeof(endPoints)/sizeof(INET_SystemEndPoint*);

/***********************************************************************
 FUNCTION NAME: SysCfg_GetInterfaceForEndPoint

 ARGUMENTS:
             aEndPointId     [in]  -> End point identifier
             aSysIntfObj     [out] -> L2 system Interface Object

 DESCRIPTION: Used to retrieve the L2 interface for an end point

 RETURNS: BCM_ERR_OK           L2 interface successfully retrieved
          BCM_ERR_INVAL_PARAMS aIntfId is NULL or end point not found
***********************************************************************/

int32_t SysCfg_GetInterfaceForEndPoint(
                            const INET_EndPointId aEndPointId,
                            INET_SystemInterface** const aSysIntfObj)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aEndPointId >= endpoint_table_sz) || (NULL == aSysIntfObj)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        INET_InterfaceId intfID;
        intfID = endPoints[aEndPointId]->intfId;
        interfaces[intfID]->ifObj.ifID = intfID;
        *aSysIntfObj = interfaces[intfID];
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_GetInterface

 ARGUMENTS:
             aIntfId      [in]  -> L2 Interface Identifier
             aSysIntfObj  [out] -> L2 Interface system object

 DESCRIPTION: Retrieves the L2 system interface for an end point

 RETURNS: BCM_ERR_OK           interface identifier retrieved successfully
          BCM_ERR_INVAL_PARAMS aIntfId is NULL or aEndPointId is invalid
******************************************************************************/
int32_t SysCfg_GetInterfaceByIndex(
                            const INET_InterfaceId aIntfId,
                            INET_SystemInterface** const aSysIntfObj)
{
    int32_t          retVal;

    if (aIntfId >= intf_table_sz) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_OK;
        if (NULL != aSysIntfObj) {
            *aSysIntfObj = interfaces[aIntfId];
        }
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_SearchInterface

 ARGUMENTS:
             aHwId       [in]  -> HW index of the interface
             aIPAddress  [in]  -> IP Address
             aVlan       [in]  -> Vlan tag value
             aSysIntfObj [out] -> L2 system Interface Object

 DESCRIPTION: Used to search for the L2 interface identifier from the
              system configuration The key is <aHwId , Ip addr, Vlan>

 RETURNS: BCM_ERR_OK           L2 interface successfully retrieved
          BCM_ERR_INVAL_PARAMS aSysIntfObj is NULL
          BCM_ERR_NOT_FOUND    No such valid interface exists
***********************************************************************/
int32_t SysCfg_SearchInterface(const uint32_t aHwId,
                               const INET_IPAddress aIPAddress,
                               const uint16_t aVlan,
                               INET_SystemInterface** const aSysIntfObj)
{
    int32_t  retVal;
    uint32_t i;

    if (NULL == aSysIntfObj) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < intf_table_sz; ++i) {
            if((aHwId == interfaces[i]->ifObj.ifCfg->hwIndex) &&
               (aIPAddress == interfaces[i]->ifObj.ifCfg->staticAddress) &&
               (aVlan == interfaces[i]->ifObj.ifCfg->vlanConfig.vlan)) {
                interfaces[i]->ifObj.ifID = i;
                *aSysIntfObj = interfaces[i];
                retVal = BCM_ERR_OK;
                break;
            }
        }
    }

    return retVal;
}


/***********************************************************************
 FUNCTION NAME: SysCfg_GetTotalNumInterfaces

 ARGUMENTS: void

 DESCRIPTION: Used to return the number of interfaces
              registered in system

 RETURNS: uint32_t           Count of the number of interfaces
***********************************************************************/
uint32_t SysCfg_GetTotalNumInterfaces(void)
{
    return intf_table_sz;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_GetEndPointByCompPayID

 ARGUMENTS:
             aCompId         [in]  -> Component identifier
             aPayloadId      [in]  -> Payload identifier
             aEndPtIndex     [out] -> Index of the array where end point is stored
             aEndPointSysObj [out] -> L2 system end point Object

 DESCRIPTION: Used to retrieve the system end point for a client along
              with its configuration and context using
              <Comp ID, Payload ID>

 RETURNS: BCM_ERR_OK           End point successfully retrieved
          BCM_ERR_INVAL_PARAMS aEndPointId is NULL
          BCM_ERR_NOT_FOUND    End point not found
***********************************************************************/
int32_t SysCfg_GetEndPointByCompPayID(
                           const uint64_t aCompId,
                           const uint32_t aPayloadId,
                           INET_EndPointId* const aEndPtIndex,
                           const INET_SystemEndPoint** const aEndPointSysObj)
{
    int32_t  retVal;
    uint32_t i;

    if ((NULL == aEndPointSysObj) || (NULL == aEndPtIndex)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < endpoint_table_sz; ++i) {
            if ((endPoints[i]->compId == aCompId) &&
                (endPoints[i]->payloadId == aPayloadId)) {
                *aEndPtIndex = (INET_EndPointId)i;
                *aEndPointSysObj  = endPoints[i];
                retVal = BCM_ERR_OK;
                break;
            }
        }
    }

    return retVal;
}


/***********************************************************************
 FUNCTION NAME: SysCfg_GetEndPointByProtoPort

 ARGUMENTS:
             aDestPort       [in]  -> The port ID for which the end point
                                      is registered
             aL4Protocol     [in]  -> The protocol for which the end point
                                      is registered
             aEndPtIndex     [out] -> Index of the array where the end point is stored
             aEndPointSysObj [out] -> Configuration for the end point and
                                      context/scratch memory for the end point

 DESCRIPTION: Used to retrieve the system end point for a client along
              with its configuration and context using
              <aDestPort, aL4Protocol>

 RETURNS: BCM_ERR_OK           End point successfully retrieved
          BCM_ERR_INVAL_PARAMS aEndPointSysObj is NULL
          BCM_ERR_NOT_FOUND    End point not found
***********************************************************************/
int32_t SysCfg_GetEndPointByProtoPort(
                           const INET_Port aDestPort,
                           const INET_L4Protocol aL4Protocol,
                           INET_EndPointId* const aEndPtIndex,
                           const INET_SystemEndPoint** const aEndPointSysObj)
{
    int32_t  retVal;
    uint16_t i;
    uint32_t idx = 0UL;

    if ((NULL == aEndPointSysObj) || (NULL == aEndPtIndex)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_NOT_FOUND;

        /* For a local port number alloted by the INET stack, fetch the */
        /* end point index and access it directly                       */
        if ((INET_EPHERMERAL_PORT_START <= aDestPort) &&
            (aDestPort < (INET_EPHERMERAL_PORT_START + INET_MAX_NUM_EPHERMERAL_PORTS))){
            idx = aDestPort & INET_AUTO_PORT_BIT_MASK;
            if ((idx < endpoint_table_sz) &&
                (endPoints[idx]->endPtObj.l4Proto == aL4Protocol)) {
                *aEndPtIndex = (INET_EndPointId)idx;
                *aEndPointSysObj  = endPoints[idx];
                retVal = BCM_ERR_OK;
            }
        }

        if (BCM_ERR_NOT_FOUND == retVal){
            for (i = 0U; i < endpoint_table_sz; ++i) {
                /* Search for the end point which matches the protocol and local port */
                /* We do not match for the remote IP address and port for server kind */
                /* of application where an end point can receive data from multiple   */
                /* remote clients. The local address is not matched here as it is     */
                /* assumed to have been checked at reception.                         */
                if ((endPoints[i]->endPtObj.l4Proto == aL4Protocol) &&
                    (endPoints[i]->endPtObj.cfg->localPort == aDestPort)) {
                    *aEndPtIndex = (INET_EndPointId)i;
                    *aEndPointSysObj  = endPoints[i];
                    retVal = BCM_ERR_OK;
                    break;
                }
            }
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: SysCfg_GetEndPointByIndex

 ARGUMENTS:
             aEndPtId      [in]  -> End point index
             aSysEndptObj  [out] -> End point system object

 DESCRIPTION: Retrieves the end point object

 RETURNS: BCM_ERR_OK           End point identifier retrieved successfully
          BCM_ERR_INVAL_PARAMS aSysEndptObj is NULL or aEndPtId is invalid
******************************************************************************/
int32_t SysCfg_GetEndPointByIndex(
                            const INET_EndPointId aEndPtId,
                            const INET_SystemEndPoint** const aSysEndptObj)
{
    int32_t          retVal;

    if ((aEndPtId >= endpoint_table_sz) || (NULL == aSysEndptObj)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_OK;
        *aSysEndptObj = endPoints[aEndPtId];
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_GetTotalNumEndPoints

 ARGUMENTS: void

 DESCRIPTION: Used to return the number of end points
              registered in system

 RETURNS: uint32_t           Count of the number of end points
***********************************************************************/
uint16_t SysCfg_GetTotalNumEndPoints(void)
{
    return endpoint_table_sz;
}


/******************************************************************************
 FUNCTION NAME: SysCfg_GetReassemblyBuffer

 ARGUMENTS:
             aSrcAddr     [in]  -> Source IP Address in IP header
             aIdent       [in]  -> 16 bit identifier received in IP header
             aReassByffer [out] -> Reassembly buffer

 DESCRIPTION: Used by the INET stack to allocate/fetch a reassembly buffer
              for received IP datagrams. The key for the search is
              <aSrcAddr, aIdent> which can be used to fetch a new or a
              previously allocated buffer

 RETURNS: BCM_ERR_OK             Reassembly buffer allocated successfully
          BCM_ERR_NOMEM          Out of reassembly buffers
******************************************************************************/
int32_t SysCfg_GetReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                   const uint16_t aIdent,
                                   INET_ReassemblyBuffer** const aReassBuffer)
{
    int32_t  retVal = BCM_ERR_NOMEM;
    uint32_t entryFound = 0UL;
    uint32_t emptySlot  = reass_pool_sz;
    uint32_t i;

    for (i = 0UL ; i < reass_pool_sz; ++i) {
        if (INET_REASSEMBLYBUF_ALLOCATED == reassemblyBufferPool[i]->bufState) {
            if ((aSrcAddr == reassemblyBufferPool[i]->ipAddr) &&
                (aIdent == reassemblyBufferPool[i]->ident)) {
                retVal = BCM_ERR_OK;
                entryFound = 1UL;
                *aReassBuffer = reassemblyBufferPool[i];
                break;
            }
        } else {
            if ((emptySlot == reass_pool_sz) &&
                (INET_REASSEMBLYBUF_FREE == reassemblyBufferPool[i]->bufState)){
                emptySlot = i;
            }
        }
    }

    if ((0UL == entryFound) && (emptySlot != reass_pool_sz)) {
        reassemblyBufferPool[emptySlot]->bufState = INET_REASSEMBLYBUF_ALLOCATED;
        reassemblyBufferPool[emptySlot]->ipAddr   = aSrcAddr;
        reassemblyBufferPool[emptySlot]->ident    = aIdent;
        reassemblyBufferPool[emptySlot]->timer    = INET_MAX_REASS_BUF_TIMEOUT;
        *aReassBuffer = reassemblyBufferPool[emptySlot];

        /* Safe to initialize the circular queue here as client has already */
        /* freed it and only CommsTask would be accessing this queue        */
        retVal = CIRCQ_Reset(reassemblyBufferPool[emptySlot]->fragmentQ);
    }

    return retVal;
}


/******************************************************************************
 FUNCTION NAME: SysCfg_GetReassemblyBufferbyPointer

 ARGUMENTS:
             aBuf         [in]  -> Previously allocated reassembly buffer
             aReassBuffer [out] -> Reassembly buffer

 DESCRIPTION: Used by the INET stack to allocate/fetch a reassembly buffer
              for received IP datagrams. The key for the search is
              <aBuf> which can be used to fetch a previously allocated buffer

 RETURNS: BCM_ERR_OK             Reassembly buffer allocated successfully
          BCM_ERR_INVAL_PARAMS   aReassBuffer or aBuf is NULL or not found
******************************************************************************/
int32_t SysCfg_GetReassemblyBufferbyPointer(
                                   const uint8_t* const aBuf,
                                   INET_ReassemblyBuffer** const aReassBuffer)
{
    int32_t  retVal = BCM_ERR_NOT_FOUND;
    uint32_t i;

    for (i = 0UL ; i < reass_pool_sz; ++i) {
        if ((aBuf == reassemblyBufferPool[i]->buffer) &&
            (INET_REASSEMBLYBUF_FREE != reassemblyBufferPool[i]->bufState)) {
            *aReassBuffer = reassemblyBufferPool[i];
            retVal = BCM_ERR_OK;
            break;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: SysCfg_GetReassemblyBufferbyIndex

 ARGUMENTS:
             aIdx                   [in]  -> Buffer index
             INET_ReassemblyBuffer  [out] -> Buffer pointer

 DESCRIPTION: Retrieves the reassembly buffer

 RETURNS: BCM_ERR_OK           Buffer retrieved successfully
          BCM_ERR_INVAL_PARAMS aReassBuffer is NULL or aIdx is invalid
******************************************************************************/
int32_t SysCfg_GetReassemblyBufferbyIndex(
                           const uint32_t aIdx,
                           INET_ReassemblyBuffer** const aReassBuffer)
{
    int retVal = BCM_ERR_OK;

    if (aIdx >= reass_pool_sz) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *aReassBuffer = reassemblyBufferPool[aIdx];
    }
    return retVal;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_GetTotalNumReassemblyBuffers

 ARGUMENTS: void

 DESCRIPTION: Used to return the number of reassembly buffers created
              in system

 RETURNS: uint32_t           Count of the number of buffers
***********************************************************************/
uint32_t SysCfg_GetTotalNumReassemblyBuffers(void)
{
    return reass_pool_sz;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_GetTotalNumARPCaches

 ARGUMENTS: void

 DESCRIPTION: Used to return the number of ARP cache used

 RETURNS: uint32_t           Count of the number of ARP cache
***********************************************************************/
uint32_t SysCfg_GetTotalNumARPCaches(void)
{
    return cache_table_sz;
}

/***********************************************************************
 FUNCTION NAME: SysCfg_GetARPCacheByIndex

 ARGUMENTS:
             aIndex       [in]  -> Cache index
             aARPCache    [in]  -> Pointer to ARP cache

 DESCRIPTION: Returns the ARP cache for the specified index

 RETURNS: BCM_ERR_OK               Cache returned successfully
          BCM_ERR_INVAL_PARAMS     No cache found for specified index
***********************************************************************/
int32_t SysCfg_GetARPCacheByIndex(uint32_t const aIndex,
                                  INET_ARPEntry** const aARPCache)
{
    int32_t retVal;

    if ((NULL == aARPCache) || (cache_table_sz <= aIndex)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        *aARPCache = arpCaches[aIndex];
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

