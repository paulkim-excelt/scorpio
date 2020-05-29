/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
 * @file bl_inet_cfg.h
 *
 * @brief Ethernet INET configuration header
 */

#ifndef BL_INET_CFG_H
#define BL_INET_CFG_H

/* Includes */
#include "bl_inet.h"
/**********************************************************************
                     Interface Data Structures
**********************************************************************/
/** End point identifier */
typedef uint16_t BL_INET_EndPointId;

/** @def BL_INET_ENDPOINTID_INVALID
    @brief Invalid value of end point identifier
*/
#define BL_INET_ENDPOINTID_INVALID (0xFFFFU)

/** OSI Layer 4 (Transport) protocol as defined by IANA (RFC790) */
typedef uint16_t BL_INET_L4Protocol;

/** @def BL_INET_L4PROTO_ICMP
    @brief Internet Control Message Protocol
*/
#define BL_INET_L4PROTO_ICMP    (1U)

/** @def BL_INET_L4PROTO_TCP
    @brief Transmission Control Protocol
*/
#define BL_INET_L4PROTO_TCP     (6U)

/** @def BL_INET_L4PROTO_UDP
    @brief User Datagram Protocol
*/
#define BL_INET_L4PROTO_UDP     (17U)

/** @def BL_INET_L4PROTO_UDPLITE
    @brief Lightweight User Datagram Protocol
*/
#define BL_INET_L4PROTO_UDPLITE (136U)

/** Flag denoting ARP entry properties */
typedef uint32_t BL_INET_ARPFlag;

/** @def BL_INET_ARP_ENTRY_FLAG_WAIT
    @brief ARP request has been sent and a response is awaited
*/
#define BL_INET_ARP_ENTRY_FLAG_WAIT     1UL

/** @def BL_INET_ARP_ENTRY_FLAG_RESOLVED
    @brief Valid response was received for an ARP request
*/
#define BL_INET_ARP_ENTRY_FLAG_RESOLVED 2UL

/** @def BL_INET_ARP_ENTRY_FLAG_LEARNT
    @brief ARP entry learnt from a gratuitous ARP or an incoming request
*/
#define BL_INET_ARP_ENTRY_FLAG_LEARNT   3UL

/** @def BL_INET_ARP_ENTRY_FLAG_STATIC
    @brief Statically configured ARP entry
*/
#define BL_INET_ARP_ENTRY_FLAG_STATIC   4UL

/** L2 Interface identifier */
typedef uint32_t BL_INET_InterfaceId;

/** @def BL_INET_INTERFACEID_INVALID
    @brief Invalid interface
*/
#define BL_INET_INTERFACEID_INVALID (0xFFFFFFFFUL)

/** Network Interface Controller Index */
typedef uint32_t BL_INET_NIFCtrlIdx;

/** @def BL_INET_NIFCTRLIDX_INVALID
    @brief Invalid NIF control index
*/
#define BL_INET_NIFCTRLIDX_INVALID (0xFFFFFFFFUL)

/** Value of the 6-bit Differentiated Services (DS) field in the IP header */
typedef uint8_t BL_INET_DSCP;

/** @def BL_INET_DSCP_CS0
    @brief Class 0 Selector
*/
#define BL_INET_DSCP_CS0   (0x00U)

/** @def BL_INET_DSCP_CS1
    @brief Class 1 Selector
*/
#define BL_INET_DSCP_CS1   (0x08U)

/** @def BL_INET_DSCP_CS2
    @brief Class 2 Selector
*/
#define BL_INET_DSCP_CS2   (0x10U)

/** @def BL_INET_DSCP_CS3
    @brief Class 3 Selector
*/
#define BL_INET_DSCP_CS3   (0x18U)

/** @def BL_INET_DSCP_CS4
    @brief Class 4 Selector
*/
#define BL_INET_DSCP_CS4   (0x20U)

/** @def BL_INET_DSCP_CS5
    @brief Class 5 Selector
*/
#define BL_INET_DSCP_CS5   (0x28U)

/** @def BL_INET_DSCP_CS6
    @brief Class 6 Selector
*/
#define BL_INET_DSCP_CS6   (0x30U)

/** @def BL_INET_DSCP_CS7
    @brief Class 7 Selector
*/
#define BL_INET_DSCP_CS7   (0x38U)

/** @def BL_INET_DSCP_AF11
    @brief Assured Forwarding Class 1, Low Drop Probability
*/
#define BL_INET_DSCP_AF11  (0x0AU)

/** @def BL_INET_DSCP_AF12
    @brief Assured Forwarding Class 1, Medium Drop Probability
*/
#define BL_INET_DSCP_AF12  (0x0CU)

/** @def BL_INET_DSCP_AF13
    @brief Assured Forwarding Class 1, High Drop Probability
*/
#define BL_INET_DSCP_AF13  (0x0EU)

/** @def BL_INET_DSCP_AF21
    @brief Assured Forwarding Class 2, Low Drop Probability
*/
#define BL_INET_DSCP_AF21  (0x12U)

/** @def BL_INET_DSCP_AF22
    @brief Assured Forwarding Class 2, Medium Drop Probability
*/
#define BL_INET_DSCP_AF22  (0x14U)

/** @def BL_INET_DSCP_AF23
    @brief Assured Forwarding Class 2, High Drop Probability
*/
#define BL_INET_DSCP_AF23  (0x16U)

/** @def BL_INET_DSCP_AF31
    @brief Assured Forwarding Class 3, Low Drop Probability
*/
#define BL_INET_DSCP_AF31  (0x1AU)

/** @def BL_INET_DSCP_AF32
    @brief Assured Forwarding Class 3, Medium Drop Probability
*/
#define BL_INET_DSCP_AF32  (0x1CU)

/** @def BL_INET_DSCP_AF33
    @brief Assured Forwarding Class 3, High Drop Probability
*/
#define BL_INET_DSCP_AF33  (0x1EU)

/** @def BL_INET_DSCP_AF41
    @brief Assured Forwarding Class 4, Low Drop Probability
*/
#define BL_INET_DSCP_AF41  (0x22U)

/** @def BL_INET_DSCP_AF42
    @brief Assured Forwarding Class 4, Medium Drop Probability
*/
#define BL_INET_DSCP_AF42  (0x24U)

/** @def BL_INET_DSCP_AF43
    @brief Assured Forwarding Class 4, High Drop Probability
*/
#define BL_INET_DSCP_AF43  (0x26U)

/** @def BL_INET_DSCP_EF
    @brief Expedited Forwarding
*/
#define BL_INET_DSCP_EF    (0x2EU)

/** @def BL_INET_DSCP_VA
    @brief Voice Admit
*/
#define BL_INET_DSCP_VA    (0x2CU)

/** Netmask expressed in CIDR notation */
typedef uint8_t BL_INET_Netmask;

/** IP address assignment policy */
typedef uint32_t BL_BL_INET_IPAddressAssignmentType;

/** @def BL_INET_IPADDRESSASSIGNMENT_STATIC
    @brief Statically configured IP address
*/
#define BL_INET_IPADDRESSASSIGNMENT_STATIC    (1UL)

/** @def BL_INET_IPADDRESSASSIGNMENT_DYNAMIC
    @brief IP address configured using DHCP
*/
#define BL_INET_IPADDRESSASSIGNMENT_DYNAMIC   (2UL)

/** @def BL_INET_IPADDRESSASSIGNMENT_LINKLOCAL
    @brief Link-local assignment of IP address
*/
#define BL_INET_IPADDRESSASSIGNMENT_LINKLOCAL (3UL)

/** @def BL_INET_CONVERT_IP_ADDRESS
    @brief Converts an IP address from the a.b.c.d notation to BL_INET_IPAddress
*/
#define BL_INET_CONVERT_IP_ADDRESS(A, B, C, D) (A & 0xFFU) << 24U|(B & 0xFFU) << 16U\
                                            |(C & 0xFFU) << 8U | (D & 0xFFU)

/** @struct BL_INET_ARPEntry
    @brief ARP table entry
*/
typedef struct _BL_INET_ARPEntry {
    /** Indicates whether the entry is populated or not */
    uint32_t       valid;

    /** IP Address - used as the key for the lookup */
    BL_INET_IPAddress ipAddr;

    /** MAC Address - used as the result for the lookup */
    uint8_t        macAddr[6U];

    /** Flags to indicate the type of entry */
    BL_INET_ARPFlag   flags;

    /** Time in seconds for the entry to timeout */
    uint32_t       timer;

    /** Exclusive - For critical section protection using Ldrex/Strex */
    uint32_t       exclVal;
} BL_INET_ARPEntry;

/** @struct BL_INET_InterfaceVlanTagConfig
    @brief VLAN tag configuration for a L2 interface
*/
typedef struct _BL_INET_InterfaceVlanTagConfig {
    /** Priority code point value. Valid values are 0-7 */
    uint16_t   pcp;

    /** VLAN number. Valid values are 1-4094. 0 indicates untagged interface */
    uint16_t  vlan;
} BL_INET_InterfaceVlanTagConfig;

/** @struct BL_INET_InterfaceStats
    @brief Structure representing statistics for an interface
*/
typedef struct _BL_INET_InterfaceStats {
    /** Number of packets transmitted */
    uint32_t txPkts;

    /** Number of bytes transmitted */
    uint32_t txBytes;

    /** Number of ARP messages transmitted */
    uint32_t txARP;

    /** Number of ICMP messages transmitted */
    uint32_t txICMP;

    /** Number of transmit packets dropped */
    uint32_t txDropPkts;

    /** Number of transmit packets cancelled */
    uint32_t txCancelPkts;

    /** Number of L2 packets transmitted */
    uint32_t txL2PktCount;

    /** Number of L2 buffer unavailable */
    uint32_t txL2BufNA;

    /** Number of packets received */
    uint64_t rxPkts;

    /** Number of bytes received */
    uint64_t rxBytes;

    /** Number of all incoming packets received */
    uint64_t rxL2PktCount;

    /** Number of received packets dropped */
    uint32_t rxDropPkts;

    /** Number of ICMP messages received */
    uint32_t rxICMP;

    /** Number of ICMP messages not supported */
    uint32_t rxICMPErr;

    /** Number of ARP messages received */
    uint32_t rxARP;
} BL_INET_InterfaceStats;

/** @struct BL_INET_InterfaceConfig
    @brief Configuration for an interface
*/
typedef struct _BL_INET_InterfaceConfig {
    /** Identifier for the L2 controller */
    BL_INET_InterfaceId             hwIndex;

    /** VLAN configuration for the interface */
    BL_INET_InterfaceVlanTagConfig  vlanConfig;

    /** Type of IP address assigned to the interface */
    BL_BL_INET_IPAddressAssignmentType addressType;

    /** IP address assigned to the interface (valid only for static assignment) */
    BL_INET_IPAddress               staticAddress;

    /** Netmask (valid only for static assigment) */
    BL_INET_Netmask                 netmask;

    /** Default route/gateway (valid only for static assignment) */
    BL_INET_IPAddress               defaultRoute;
} BL_INET_InterfaceConfig;

/** @struct BL_INET_InterfaceObject
    @brief Interface Object Structure
*/
typedef struct _BL_INET_InterfaceObject {
    /** Unique interface identifier */
    BL_INET_InterfaceId ifID;

    /** Pointer to the configuration data for this interface */
    const BL_INET_InterfaceConfig *ifCfg;

    /** Pointer to the statistics memory for this interface */
    BL_INET_InterfaceStats  *ifStats;

    /** Network Interface Controller Index */
    BL_INET_NIFCtrlIdx nifCtrlIdx;
} BL_INET_InterfaceObject;

/** @struct BL_INET_SystemInterface
    @brief Structure representing an interface
*/
typedef struct _BL_INET_SystemInterface {
    /** Interface details with configuration */
    BL_INET_InterfaceObject ifObj;

    /** Pointer to the ARP cache for the interface */
    BL_INET_ARPEntry        *arpCache;
} BL_INET_SystemInterface;

/** @struct INET_EndPointState
    @brief Context/Scratch-memory for an endpoint
*/
typedef struct _BL_INET_EndPointState {
    /** Client buffer containing the payload for transmission */
    const uint8_t* txBuf;

    /** Length of the client buffer containing payload for transmission */
    uint16_t       txLength;

    /** Number of packets transmitted */
    uint64_t       txPkts;

    /** Number of bytes transmitted */
    uint64_t       txBytes;

    /** Number of packets received */
    uint64_t       rxGoodPkts;

    /** Number of packets dropped due to no waiting client task */
    uint64_t       rxDropPkts;

    /** Number of bytes received */
    uint64_t       rxBytes;

    /** Index of ARP cache where the remote entry was last resolved */
    uint32_t       arpCacheIndex;
} BL_INET_EndPointState;

/** @struct BL_INET_EndPointConfig
    @brief Configuration for an endpoint
*/
typedef struct _BL_INET_EndPointConfig {
    /** Local port number */
    BL_INET_Port       localPort;

    /** Remote IP address. Provide a valid unicast/multicast address here */
    BL_INET_IPAddress  remoteAddr;

    /** Remote port number */
    BL_INET_Port       remotePort;

    /** OSI L4 protocol */
    BL_INET_L4Protocol l4Proto;

    /** Value of the differentiated services (DS) field in the IP header */
    BL_INET_DSCP       dscp;

} BL_INET_EndPointConfig;

/** @struct BL_INET_EndPointObject
    @brief Endpoint object details
*/
typedef struct _BL_INET_EndPointObject {
    /* Index of the end point identifier object */
    BL_INET_EndPointId endPointID;

    /** Configuration for the end point */
    const BL_INET_EndPointConfig*   cfg;

    /** Context/Scratch memory for the end point */
    BL_INET_EndPointState*          ctxt;

} BL_INET_EndPointObject;

/** @struct BL_INET_SystemEndPoint
    @brief Structure representing an end point
*/
typedef struct _BL_INET_SystemEndPoint {
    /** Instance identifier used to identify the client */
    uint32_t                    payloadId;

    /** Interface identifier */
    BL_INET_InterfaceId            intfId;

    /** End point details */
    BL_INET_EndPointObject         endPtObj;

    /** Packet reception client callback */
    void                        (*rxCb)(const uint8_t* const aBuf,
                                     uint16_t aLen,
                                     BL_INET_IPAddress aSrcIPAddr,
                                     BL_INET_Port aSrcPort);

} BL_INET_SystemEndPoint;

/** @def BL_INET_UDP_PAYLOAD_ID<x>
    @brief List of payload IDs defined statically for UDP communication
           and to uniquely identify the stream and map it to an
           end point and interface
*/
#define BL_INET_UDP_PAYLOAD_ID0            (0UL)
#define BL_INET_UDP_PAYLOAD_ID1            (1UL)
#define BL_INET_UDP_PAYLOAD_ID2            (2UL)
#define BL_INET_UDP_PAYLOAD_ID3            (3UL)
#define BL_INET_UDP_PAYLOAD_ID4            (4UL)
#define BL_INET_UDP_PAYLOAD_ID5            (5UL)
#define BL_INET_UDP_PAYLOAD_ID6            (6UL)
#define BL_INET_UDP_PAYLOAD_ID7            (7UL)
#define BL_INET_UDP_PAYLOAD_ID8            (8UL)
#define BL_INET_UDP_PAYLOAD_ID9            (9UL)
#define BL_INET_UDP_PAYLOAD_ID10           (10UL)
#define BL_INET_UDP_PAYLOAD_ID11           (11UL)

/** @brief Define an interface instance
 *
 * @param[in]   aName        Name of the interface
 * @param[in]   aCache       Name of the ARP cache instance for the interface
 * @param[in]   aStats       Name of the statistics instance for the interface
 * @param[in]   aHwIndex     H/W controller index
 * @param[in]   aPcp         Value of the PCP field in the priority tag
 * @param[in]   aVid         VLAN Id
 * @param[in]   aIPAddr      Static IP address
 * @param[in]   aNetmask     Netmask
 * @param[in]   aDfltRoute   IP address of gateway/default route
 *
 */
#define BL_DEFINE_INET_INTERFACE(aName, aCache, aCfg, aStats, aHwIndex, aPcp,  \
                              aVid, aIPAddr, aNetmask, aDfltRoute)          \
static BL_INET_ARPEntry aCache[INET_MAX_ENTRIES_ARPCACHE];                     \
static BL_INET_InterfaceStats aStats;                                          \
static BL_INET_InterfaceConfig const aCfg =                                    \
{                                                                           \
    .hwIndex       = aHwIndex,                                              \
    .vlanConfig    = {                                                      \
                         .pcp = aPcp,                                       \
                         .vlan = aVid,                                      \
                     },                                                     \
    .addressType   = BL_INET_IPADDRESSASSIGNMENT_STATIC,                       \
    .staticAddress = aIPAddr,                                               \
    .netmask       = aNetmask,                                              \
    .defaultRoute  = aDfltRoute,                                            \
};                                                                          \
static BL_INET_SystemInterface aName =                                         \
{                                                                           \
    .arpCache = aCache,                                                     \
    .ifObj = {                                                              \
        .ifCfg = &aCfg,                                                     \
        .ifStats = &aStats,                                                 \
        .nifCtrlIdx = BL_INET_NIFCTRLIDX_INVALID,                              \
    }                                                                       \
};

/** @brief Define an endpoint
 *
 * @param[in]   aName         Name of end point
 * @param[in]   aCtxt         Name of context for the end point
 * @param[in]   aCompId       Component identifier used to identify the client
 * @param[in]   aPayloadId    Payload identifier used to identify the client
 * @param[in]   aIfId         Interface identifier
 * @param[in]   aRxCb         Packet reception callback
 * @param[in]   aLogNumRxPkts Log2 of maximum number of recv packets to queue
 * @param[in]   aLclPort      Local port number
 * @param[in]   aRmtAddr      IP address of the remote host
 * @param[in]   aRmtPort      Port number of the remote host
 * @param[in]   aL4Proto      L4 protocol type
 * @param[in]   aDscp         DSCP field in outgoing IP datagrams
 *
 */
#define BL_DEFINE_INET_ENDPOINT(aName, aCfg, aCtxt, aPayloadId, aIfId,         \
                             aRxCb, aLogNumRxPkts, aLclPort,                \
                             aRmtAddr, aRmtPort, aL4Proto, aDscp)           \
static BL_INET_EndPointState (aCtxt) =                                         \
{                                                                           \
    .txBuf       = NULL,                                                    \
    .txLength    = 0U,                                                      \
    .rxGoodPkts  = 0ULL,                                                    \
    .rxBytes     = 0ULL,                                                    \
    .rxDropPkts  = 0ULL,                                                    \
    .txPkts      = 0ULL,                                                    \
    .txBytes     = 0ULL                                                     \
};                                                                          \
static const BL_INET_EndPointConfig (aCfg) =                                   \
{                                                                           \
    .localPort     = aLclPort,                                              \
    .remoteAddr    = aRmtAddr,                                              \
    .remotePort    = aRmtPort,                                              \
    .l4Proto       = aL4Proto,                                              \
    .dscp          = aDscp,                                                 \
};                                                                          \
static BL_INET_SystemEndPoint aName =                                          \
{                                                                           \
        .payloadId     = aPayloadId,                                        \
        .intfId        = aIfId,                                             \
        .rxCb          = aRxCb,                                             \
        .endPtObj      = {                                                  \
            .cfg  = &aCfg,                                                  \
            .ctxt = &aCtxt,                                                 \
        }                                                                   \
};
#endif /* BL_INET_CFG_H */
