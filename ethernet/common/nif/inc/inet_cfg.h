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
    @defgroup grp_system_sw_inetcfg INET Configuration
    @ingroup grp_system_sw_cfg

    @addtogroup grp_system_sw_inetcfg
    @{

    @file inetcfg.h
    @brief INET configuration header

    @version 0.9 Imported from document
*/

#ifndef INETCFG_H
#define INETCFG_H

#include <inet_system.h>


/**
    @name UDP Payload IDs
    @{
    @brief List of payload IDs defined statically for UDP communication
           and to uniquley identify the stream and map it to an
           end point and interface
*/
#define INET_UDP_PAYLOAD_ID0            (0UL)
#define INET_UDP_PAYLOAD_ID1            (1UL)
#define INET_UDP_PAYLOAD_ID2            (2UL)
#define INET_UDP_PAYLOAD_ID3            (3UL)
#define INET_UDP_PAYLOAD_ID4            (4UL)
#define INET_UDP_PAYLOAD_ID5            (5UL)
#define INET_UDP_PAYLOAD_ID6            (6UL)
#define INET_UDP_PAYLOAD_ID7            (7UL)
#define INET_UDP_PAYLOAD_ID8            (8UL)
#define INET_UDP_PAYLOAD_ID9            (9UL)
#define INET_UDP_PAYLOAD_ID10           (10UL)
#define INET_UDP_PAYLOAD_ID11           (11UL)
/** @}*/

/** @brief Define the reassembly buffer instance
 *
 * @param[in]   aName       Name of the buffer instance
 * @param[in]   aBuffer     Name of the payload buffer
 * @param[in]   aQ          Name of the circular queue
 * @param[in]   aQBuf       Name of the queue buffer
 * @param[in]   aQIdx       Name of the circular queue index
 *
 */
#define DEFINE_INET_REASS_BUF(aName, aBuffer, aQ, aQBuf, aQIdx)               \
CIRCQ_V2_DEFINE(aQ, INET_LOG2_MAX_REASS_FRAGS, uint16_t, aQBuf,aQIdx,         \
                    (CIRCQ_OVERWRITE_DISABLE), ".data.inet.cache");           \
static uint8_t COMP_SECTION(".data.inet.cache")                               \
                                           aBuffer[INET_MAX_REASS_BUF_SIZE];  \
static INET_ReassemblyBuffer COMP_SECTION(".data.inet.cache") aName =         \
{                                                                             \
    .ipAddr    = 0UL,                                                         \
    .ident     = 0U,                                                          \
    .bufState  = INET_REASSEMBLYBUF_FREE,                                     \
    .fragmentQ = &aQ,                                                         \
    .buffer    = aBuffer,                                                     \
    .curLen    = 0U,                                                          \
    .totalLen  = 0U,                                                          \
    .timer     = 0UL,                                                         \
    .clientEndPtID = INET_ENDPOINTID_INVALID                                  \
};

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
#define DEFINE_INET_INTERFACE(aName, aCache, aCfg, aStats, aHwIndex, aPcp,  \
                              aVid, aIPAddr, aNetmask, aDfltRoute)          \
static INET_InterfaceStats COMP_SECTION(".data.inet") aStats;               \
static INET_InterfaceConfig const COMP_SECTION(".rodata.inet") aCfg =       \
{                                                                           \
    .hwIndex       = aHwIndex,                                              \
    .vlanConfig    = {                                                      \
                         .pcp = aPcp,                                       \
                         .vlan = aVid,                                      \
                     },                                                     \
    .addressType   = INET_IPADDRESSASSIGNMENT_STATIC,                       \
    .staticAddress = aIPAddr,                                               \
    .netmask       = aNetmask,                                              \
    .defaultRoute  = aDfltRoute,                                            \
};                                                                          \
static INET_SystemInterface COMP_SECTION(".data.inet") aName =              \
{                                                                           \
    .arpCache = aCache,                                                     \
    .ifObj = {                                                              \
        .ifCfg = &aCfg,                                                     \
        .ifStats = &aStats,                                                 \
        .nifCtrlIdx = NIF_CNTRLIDTYPE_INVALID,                              \
    }                                                                       \
};

/** @brief Define an endpoint
 *
 * @param[in]   aName         Name of end point
 * @param[in]   aCtxt         Name of context for the end point
 * @param[in]   aCtxtQ        Name of circular queue for the received packets
 * @param[in]   aCtxtQBuf     Name of circular queue buffer
 * @param[in]   aCtxtQIdx     Name of circular queue index
 * @param[in]   aCompId       Component identifier used to identify the client
 * @param[in]   aPayloadId    Payload identifier used to identify the client
 * @param[in]   aIfId         Interface identifier
 * @param[in]   aRxTask       Task to notify on packet reception
 * @param[in]   aTxTask       Task to notify on packet transmission confirmation
 * @param[in]   aLogNumRxPkts Log2 of maximum number of recv packets to queue
 * @param[in]   aLclPort      Local port number
 * @param[in]   aRmtAddr      IP address of the remote host
 * @param[in]   aRmtPort      Port number of the remote host
 * @param[in]   aL4Proto      L4 protocol type
 * @param[in]   aDscp         DSCP field in outgoing IP datagrams
 */
#define DEFINE_INET_ENDPOINT(aName, aCfg, aCtxt, aCtxtQ, aCtxtQBuf,         \
                             aCtxtQIdx, aCompId, aPayloadId, aIfId,         \
                             aRxTask, aRxEvent, aTxTask, aTxEvent,          \
                             aLogNumRxPkts, aLclPort, aRmtAddr,             \
                             aRmtPort, aL4Proto, aDscp)                     \
CIRCQ_V2_DEFINE((aCtxtQ), (aLogNumRxPkts), INET_RxPkt, (aCtxtQBuf),         \
                     (aCtxtQIdx), (CIRCQ_OVERWRITE_DISABLE),                \
                     ".data.inet");                                         \
static INET_EndPointState COMP_SECTION(".data.inet") (aCtxt) =              \
{                                                                           \
    .ident       = 0U,                                                      \
    .sentBytes   = 0U,                                                      \
    .txBuf       = NULL,                                                    \
    .txLength    = 0U,                                                      \
    .rxPktQ      = &aCtxtQ,                                                 \
    .rxGoodPkts  = 0ULL,                                                    \
    .rxBytes     = 0ULL,                                                    \
    .rxTruncPkts = 0ULL,                                                    \
    .rxDropPkts  = 0ULL,                                                    \
    .txPkts      = 0ULL,                                                    \
    .txBytes     = 0ULL                                                     \
};                                                                          \
static INET_EndPointIPPort COMP_SECTION(".data.inet") (aCfg) =              \
{                                                                           \
    .localPort     = aLclPort,                                              \
    .remoteAddr    = aRmtAddr,                                              \
    .remotePort    = aRmtPort,                                              \
};                                                                          \
static INET_SystemEndPoint const COMP_SECTION(".rodata.inet") aName =       \
{                                                                           \
        .compId        = aCompId,                                           \
        .payloadId     = aPayloadId,                                        \
        .intfId        = aIfId,                                             \
        .rxTaskId      = aRxTask,                                           \
        .rxTaskEventId = aRxEvent,                                          \
        .txTaskId      = aTxTask,                                           \
        .txTaskEventId = aTxEvent,                                          \
        .endPtObj      = {                                                  \
            .l4Proto = aL4Proto,                                            \
            .dscp    = aDscp,                                               \
            .cfg     = &aCfg,                                               \
            .ctxt    = &aCtxt,                                              \
        }                                                                   \
};

/** @brief  Get L2 interface for an end point
 *
 *  Used to retrieve the L2 interface for an end point
 *
 * @param[in]   aEndPointId           End point identifier
 * @param[out]  aSysIntfObj           L2 system Interface Object
 *
 * @retval      BCM_ERR_OK            L2 interface successfully
 *                                    retrieved
 * @retval      BCM_ERR_INVAL_PARAMS  aIntfId is NULL or end point not found
 */
int32_t SysCfg_GetInterfaceForEndPoint(
                            const INET_EndPointId aEndPointId,
                            INET_SystemInterface** const aSysIntfObj);

/** @brief  Get L2 interface specified by the index
 *
 *  Used to retrieve the L2 system interface for the index specified
 *
 * @param[in]   aIntfId               Interface index
 * @param[out]  aSysIntfObj           L2 system Interface Object
 *
 * @retval      BCM_ERR_OK            L2 system interface successfully
 *                                    retrieved
 * @retval      BCM_ERR_INVAL_PARAMS  aSysIntfObj is NULL or aIntfId not found
 */
int32_t SysCfg_GetInterfaceByIndex(
                            const INET_InterfaceId aIntfId,
                            INET_SystemInterface** const aSysIntfObj);

/** @brief Searches for an interface using IP Address & VLAN as key
 *
 *  Used to search for the L2 interface identifier from the system configuration
 *  The key is <aHwId , aIPAddr, Vlan>
 *
 * @param[in]  aHwId                  HW index of the interface
 * @param[in]  aIPAddress             IP Address
 * @param[in]  aVlan                  VLAN tag value
 * @param[out] aSysIntfObj            L2 system Interface Object
 *
 * @retval     BCM_ERR_OK             L2 interface identifier retrieved
 * @retval     BCM_ERR_INVAL_PARAMS   aSysIntfObj is NULL
 * @retval     BCM_ERR_NOT_FOUND      No such valid interface exists
 */
int32_t SysCfg_SearchInterface(const uint32_t aHwId,
                               const INET_IPAddress aIPAddress,
                               const uint16_t aVlan,
                               INET_SystemInterface** const aSysIntfObj);

/** @brief Count of interfaces registered in System
 *
 *  Used to return the number of interfaces registered in system
 *
 * @param[in]  void
 *
 * @retval     uint32_t               Count of the number of interfaces
 */
uint32_t SysCfg_GetTotalNumInterfaces(void);

/** @brief  Get end point information
 *
 *  Used to retrieve the system end point for a client along with its
 *  configuration and context
 *
 * @param[in]  aCompId                Component identifier used to identify the
 *                                    client of the end point
 * @param[in]  aPayloadId             Payload identifier used to identify the
 *                                    client of the end point
 * @param[out] aEndPtIndex            Index of the array where end point is stored
 * @param[out] aEndPointSysObj        Configuration for the end point and
 *                                    context/scratch memory for the end point
 *
 * @retval     BCM_ERR_OK             End point successfully retrieved
 * @retval     BCM_ERR_INVAL_PARAMS   aEndPointId is NULL
 * @retval     BCM_ERR_NOT_FOUND      End point not found
 */
int32_t SysCfg_GetEndPointByCompPayID(
                           const uint64_t aCompId,
                           const uint32_t aPayloadId,
                           INET_EndPointId* const aEndPtIndex,
                           const INET_SystemEndPoint** const aEndPointSysObj);

/** @brief  Get end point information
 *
 *  Used to retrieve the system end point for a client along with its
 *  configuration and context
 *
 * @param[in]  aDestPort              The port ID for which the end point
 *                                    is registered
 * @param[in]  aL4Protocol            The protocol for which the end point
 *                                    is registered
 * @param[out] aEndPtIndex            Index of the array where end point is stored
 * @param[out] aEndPointSysObj        Configuration for the end point and
 *                                    context/scratch memory for the end point
 *
 * @retval     BCM_ERR_OK             End point successfully retrieved
 * @retval     BCM_ERR_INVAL_PARAMS   aEndPointSysObj is NULL
 * @retval     BCM_ERR_NOT_FOUND      End point not found
 */
int32_t SysCfg_GetEndPointByProtoPort(
                           const INET_Port aDestPort,
                           const INET_L4Protocol aL4Protocol,
                           INET_EndPointId* const aEndPtIndex,
                           const INET_SystemEndPoint** const aEndPointSysObj);

/** @brief  Get L2 interface specified by the index
 *
 *  Used to retrieve the end point object for the index specified
 *
 * @param[in]   aEndPtId              End point index
 * @param[out]  aSysEndptObj          End point system Object
 *
 * @retval      BCM_ERR_OK            End point successfully retrieved
 * @retval      BCM_ERR_INVAL_PARAMS  aSysEndptObj is NULL or aEndPtId not found
 */
int32_t SysCfg_GetEndPointByIndex(
                            const INET_EndPointId aEndPtId,
                            const INET_SystemEndPoint** const aSysEndptObj);

/** @brief Count of end points registered in System
 *
 *  Used to return the number of endpoints registered in system
 *
 * @param[in]  void
 *
 * @retval     uint32_t               Count of the number of endpoints
 */
uint16_t SysCfg_GetTotalNumEndPoints(void);

/** @brief Get a reassembly buffer
 *
 *  Used by the INET stack to allocate/fetch a reassembly buffer for received IP
 *  datagrams. The key for the search is <aSrcAddr, aIdent> which can be used to
 *  fetch a previously allocated buffer
 *
 * @param[in]  aSrcAddr               Source address in IP datagram being
 *                                    reassembled
 * @param[in]  aIdent                 Identification field from the IP datagram
 * @param[out] aReassBuffer           Reassembly buffer
 *
 * @retval     BCM_ERR_OK             Reassembly buffer allocated successfully
 * @retval     BCM_ERR_NOMEM          Out of reassembly buffers
 */
int32_t SysCfg_GetReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                   const uint16_t aIdent,
                                   INET_ReassemblyBuffer** const aReassBuffer);

/** @brief Get a reassembly buffer
 *
 *  Used by the INET stack to allocate/fetch a reassembly buffer for received IP
 *  datagrams. The key for the search is <aBuf> which can be used to
 *  fetch a previously allocated buffer
 *
 * @param[in]  aBuf                   Previously allocated reassembly buffer
 *                                    pointer
 * @param[out] aReassBuffer           Reassembly buffer
 *
 * @retval     BCM_ERR_OK             Reassembly buffer allocated successfully
 * @retval     BCM_ERR_INVAL_PARAMS   aReassBuffer or aBuf is NULL or not found
 */
int32_t SysCfg_GetReassemblyBufferbyPointer(
                                   const uint8_t* const aBuf,
                                   INET_ReassemblyBuffer** const aReassBuffer);

/** @brief Get a reassembly buffer by index
 *
 *  Used by the INET stack to Get the reassembly buffer by index
 *
 * @param[in]  aIdx                   Index of reassembly buffer
 * @param[out] aReassBuffer           Reassembly buffer
 *
 * @retval     BCM_ERR_OK             Reassembly buffer allocated successfully
 * @retval     BCM_ERR_INVAL_PARAMS   aReassBuffer or aBuf is NULL or not found
 */
int32_t SysCfg_GetReassemblyBufferbyIndex(
                                   const uint32_t aIdx,
                                   INET_ReassemblyBuffer** const aReassBuffer);

/** @brief Count of reassembly buffers used in System
 *
 *  Used to return the number of reassembly buffers created in system
 *
 * @param[in]  void
 *
 * @retval     uint32_t               Count of the number of buffers
 */
uint32_t SysCfg_GetTotalNumReassemblyBuffers(void);

/** @brief Count of ARP caches used in System
 *
 *  Used to return the number of ARP cache created in system
 *
 * @param[in]  void
 *
 * @retval     uint32_t               Count of the ARP cache
 */
uint32_t SysCfg_GetTotalNumARPCaches(void);

/** @brief Get the ARP cache memory by index
 *
 *  Used to return the ARP cache memory based on the specified
 *  index.
 *
 * @param[in]      aIndex             Index of cache array
 * @param[out]     aARPCache          Pointer to ARP cache
 *
 * @retval     BCM_ERR_OK             Cache returned successfully
 * @retval     BCM_ERR_INVAL_PARAMS   Invalid aIndex or aARPCache is NULL
 */
int32_t SysCfg_GetARPCacheByIndex(uint32_t const aIndex,
                                  INET_ARPEntry** const aARPCache);

#endif /* INETCFG_H */

/** @} */
