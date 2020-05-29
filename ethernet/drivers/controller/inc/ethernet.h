/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_eth_cntlrdrv_ifc Ethernet Driver Interface
    @ingroup grp_eth_cntlrdrv

    @addtogroup grp_eth_cntlrdrv_ifc
    @{
    @section sec_eth_overview Overview
    @image html eth_overview.jpg "Ethernet driver architecture"
    Ethernet driver has two layers: Driver interface layer and Driver core layer.
    Interface layer is implemented as an unprivileged software layer, while
    driver core is implemented as a privileged software layer which manages
    Ethernet DMA, transceiver and interrupts.

    Ethernet packets are transmitted in FIFO order based on queuing. Ethernet
    driver supports multiple clients transmitting and receiving packets
    simultaneously.

    The Ethernet driver also provides hardware timestamps for time
    synchronization packets. Currently only gPTP SYNC, PDELAY REQUEST &
    PDELAY RESPONSE packets are time-stamped. It also maintains and provides an
    Ethernet reference time base.

    @section sec_eth_fsm State Machine

    @subsection subsec1_eth_fsm Ethernet Driver State Machine
    @image html eth_state_machine.jpg "Ethernet Driver state transitions"
    The state transition for Ethernet driver is depicted in the above figure.
    It has two states as follows:
    - #ETHER_STATE_UNINIT: Driver state before initialization or after
    successful de-initialization
    - #ETHER_STATE_INIT: Driver state after initialization

    @subsection subsec2_eth_fsm Ethernet Controller State Machine
    @image html eth_state_machine_cntrlr.jpg "Ethernet Controller state transitions"
    The state transition for Ethernet controller is depicted in the above figure.
    It has two states as follows:
    - #ETHER_MODE_DOWN: Ethernet controller is down and no transfer is pending
    or in progress.
    - #ETHER_MODE_ACTIVE: Ethernet controller is active and can transmit-receive
    packets.

    @note APIs triggering ETHER_MODE_ACTIVE → ETHER_MODE_ACTIVE transition are:
    - #ETHER_SetMacAddr
    - #ETHER_GetMacAddr
    - #ETHER_GetTxBuffer
    - #ETHER_Send
    - #ETHER_GetRxStats
    - #ETHER_TimeInit
    - #ETHER_TimeInitStateHandler
    - #ETHER_EnableTxTimestamp
    - #ETHER_GetTxTimestamp
    - #ETHER_GetRxTimestamp
    - #ETHER_GetTime
    - #ETHER_SetCorrectionTime
    - #ETHER_SetTime

    @section sec_eth_seq Sequence Diagrams

    @subsection subsec1_eth_seq Ethernet Packet Transmit/Receive
    @image html eth_seq_diagram_pkt_tx_rx.jpg "Packet transmit/receive sequence"

    @subsection subsec2_eth_seq Retrieval of transmit timestamp
    @image html eth_seq_diagram_tx_ts.jpg "Retrieval of transmit timestamp"

    @subsection subsec3_eth_seq Retrieval of receive timestamp
    @image html eth_seq_diagram_rx_ts.jpg "Retrieval of receive timestamp"

    @subsection subsec4_eth_seq Ethernet time initialization – No synchronization/Internal synchronization
    @image html eth_seq_diagram_time_init_int.jpg "Ethernet time initialization with no synchronization/internal synchronization"

    @subsection subsec5_eth_seq Ethernet time initialization – External synchronization
    @image html eth_seq_diagram_time_init_ext.jpg "Ethernet time initialization with external synchronization"

    @subsection subsec6_eth_seq Time Management on a Timing slave
    @image html eth_seq_diagram_timing_slave.jpg "Time management on a timing slave"

    @subsection subsec7_eth_seq Time Management on a Timing Master
    @image html eth_seq_diagram_timing_master.jpg "Time management on a timing master"

    @section sec_eth_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_ethernet.dep

    @limitations For any limitation of the Ethernet driver in specific release,
    please refer release notes accompanying release deliverables.

    @file ethernet/drivers/controller/inc/ethernet.h
    @brief Ethernet Driver interface
    This header file contains the interface functions for Ethernet Driver

    @version 1.26 Imported from docx
*/

#ifndef ETHERNET_H
#define ETHERNET_H

#include <stddef.h>
#include <inttypes.h>
#include <bcm_err.h>
#include <mdio.h>

/**
    @name Ethernet Driver API IDs
    @{
    @brief API IDs for Ethernet Driver
*/
#define BRCM_SWARCH_ETHER_MAC_ADDR_SIZE_MACRO         (0x00U) /**< @brief #ETHER_MAC_ADDR_SIZE */
#define BRCM_SWARCH_ETHER_TYPE_SIZE_MACRO             (0x01U) /**< @brief #ETHER_TYPE_SIZE */
#define BRCM_SWARCH_ETHER_HDR_SIZE_MACRO              (0x02U) /**< @brief #ETHER_HDR_SIZE */
#define BRCM_SWARCH_ETHER_HEART_BEAT_CNT_MACRO        (0x03U) /**< @brief #ETHER_HEART_BEAT_CNT_MAX */
#define BRCM_SWARCH_ETHER_HW_ID_TYPE                  (0x04U) /**< @brief #ETHER_HwIDType */
#define BRCM_SWARCH_ETHER_CHAN_ID_TYPE                (0x05U) /**< @brief #ETHER_ChanIDType */
#define BRCM_SWARCH_ETHER_MODE_TYPE                   (0x06U) /**< @brief #ETHER_ModeType */
#define BRCM_SWARCH_ETHER_SPEED_TYPE                  (0x07U) /**< @brief #ETHER_SpeedType */
#define BRCM_SWARCH_ETHER_DUPLEX_MODE_TYPE            (0x08U) /**< @brief #ETHER_DuplexModeType */
#define BRCM_SWARCH_ETHER_ETHER_TYPE                  (0x09U) /**< @brief #ETHER_EtherType */
#define BRCM_SWARCH_ETHER_PRIO_TYPE                   (0x0AU) /**< @brief #ETHER_PrioType */
#define BRCM_SWARCH_ETHER_PRIO_CHAN_MAP_SZ_MACRO      (0x0BU) /**< @brief #ETHER_PRIO_CHAN_MAP_SZ */
#define BRCM_SWARCH_ETHER_RESULT_TYPE                 (0x0CU) /**< @brief #ETHER_ResultType */
#define BRCM_SWARCH_ETHER_TIMESTAMPQUAL_TYPE          (0x0DU) /**< @brief #ETHER_TimestampQualType */
#define BRCM_SWARCH_ETHER_TIMESYNC_TYPE               (0x0EU) /**< @brief #ETHER_TimeSyncType */
#define BRCM_SWARCH_ETHER_TIMEINITCMD_TYPE            (0x0FU) /**< @brief #ETHER_TimeInitCmdType */
#define BRCM_SWARCH_ETHER_TIMESTAMP_TYPE              (0x10U) /**< @brief #ETHER_TimestampType */
#define BRCM_SWARCH_ETHER_TIMESTAMPDIFF_TYPE          (0x11U) /**< @brief #ETHER_TimestampDiffType */
#define BRCM_SWARCH_ETHER_RATERATIO_TYPE              (0x12U) /**< @brief #ETHER_RateRatioType */
#define BRCM_SWARCH_ETHER_RXSTATS_TYPE                (0x13U) /**< @brief #ETHER_RxStatsType */
#define BRCM_SWARCH_ETHER_TXSTATS_TYPE                (0x14U) /**< @brief #ETHER_TxStatsType */
#define BRCM_SWARCH_ETHER_TIMECFG_TYPE                (0x15U) /**< @brief #ETHER_TimeCfgType */
#define BRCM_SWARCH_ETHER_TIMEINITOUT_TYPE            (0x16U) /**< @brief #ETHER_TimeInitOutType */
#define BRCM_SWARCH_ETHER_CFG_TYPE                    (0x17U) /**< @brief #ETHER_CfgType */
#define BRCM_SWARCH_ETHER_INIT_PROC                   (0x18U) /**< @brief #ETHER_Init */
#define BRCM_SWARCH_ETHER_DEINIT_PROC                 (0x19U) /**< @brief #ETHER_DeInit */
#define BRCM_SWARCH_ETHER_SETMACADDR_PROC             (0x1AU) /**< @brief #ETHER_SetMacAddr */
#define BRCM_SWARCH_ETHER_GETMACADDR_PROC             (0x1BU) /**< @brief #ETHER_GetMacAddr */
#define BRCM_SWARCH_ETHER_GETTXBUFFER_PROC            (0x1CU) /**< @brief #ETHER_GetTxBuffer */
#define BRCM_SWARCH_ETHER_SEND_PROC                   (0x1DU) /**< @brief #ETHER_Send */
#define BRCM_SWARCH_ETHER_GETRXSTATS_PROC             (0x1EU) /**< @brief #ETHER_GetRxStats */
#define BRCM_SWARCH_ETHER_ENABLETXTIMESTAMP_PROC      (0x1FU) /**< @brief #ETHER_EnableTxTimestamp */
#define BRCM_SWARCH_ETHER_GETTXTIMESTAMP_PROC         (0x20U) /**< @brief #ETHER_GetTxTimestamp */
#define BRCM_SWARCH_ETHER_GETRXTIMESTAMP_PROC         (0x21U) /**< @brief #ETHER_GetRxTimestamp */
#define BRCM_SWARCH_ETHER_GETTIME_PROC                (0x22U) /**< @brief #ETHER_GetTime */
#define BRCM_SWARCH_ETHER_SETTIME_PROC                (0x23U) /**< @brief #ETHER_SetTime */
#define BRCM_SWARCH_ETHER_SETCORRECTIONTIME_PROC      (0x24U) /**< @brief #ETHER_SetCorrectionTime */
#define BRCM_SWARCH_ETHER_TIMEINIT_PROC               (0x25U) /**< @brief #ETHER_TimeInit */
#define BRCM_SWARCH_ETHER_SETMODE_PROC                (0x26U) /**< @brief #ETHER_SetMode */
#define BRCM_SWARCH_ETHER_GETMODE_PROC                (0x27U) /**< @brief #ETHER_GetMode */
#define BRCM_SWARCH_ETHER_SETGPTIMER_PROC             (0x28U) /**< @brief #ETHER_SetGPTimer */
#define BRCM_SWARCH_ETHER_VLANID_TYPE                 (0x29U) /**< @brief #ETHER_VLANIDType */
#define BRCM_SWARCH_ETHER_MDIOREAD_PROC               (0x30U) /**< @brief #ETHER_MdioRead */
#define BRCM_SWARCH_ETHER_MDIOWRITE_PROC              (0x31U) /**< @brief #ETHER_MdioWrite */
#define BRCM_SWARCH_ETHER_MDIOCHECKSTATUS_PROC        (0x32U) /**< @brief #ETHER_MdioCheckStatus */

/** @} */

/**
    @brief Ethernet MAC address size

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_MAC_ADDR_SIZE         (6UL)

/**
    @brief Ethernet type size

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
#define ETHER_TYPE_SIZE             (2UL)

/**
    @brief Ethernet header size

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
    TODO: Check under which requirement these macros would come
*/
#define ETHER_HDR_SIZE                \
    ((2UL * ETHER_MAC_ADDR_SIZE) + ETHER_TYPE_SIZE)

/**
    @brief Number of words required to store heartbeat

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
#define ETHER_HEART_BEAT_CNT_MAX    (5UL)

/**
    @name ETHER_HwIDType
    @{
    @brief Index of the Ethernet controller

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_HwIDType; /**< @brief ETHER_HwID typedef */
#define ETHER_HW_ID_0   (0UL)    /**< @brief Ethernet controller 0 ID */
/** @} */

/**
    @brief Index of Ethernet channel within a controller

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_ChanIDType;

/**
    @name ETHER_ModeType
    @{
    @brief Ethernet controller mode

    @trace #BRCM_SWREQ_ETHER_CONTROLLER_INITIALISATION
*/
typedef uint32_t ETHER_ModeType;  /**< @brief ETHER_Mode typedef */
#define ETHER_MODE_DOWN     (1UL) /**< @brief Controller down */
#define ETHER_MODE_ACTIVE   (2UL) /**< @brief Controller active */
/** @} */

/**
    @name ETHER_SpeedType
    @{
    @brief Ethernet controller(MAC) speed

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_SpeedType;     /**< @brief ETHER_Speed typedef */
#define ETHER_SPEED_10MBPS      (0UL) /**< @brief 10 Megabits per second */
#define ETHER_SPEED_100MBPS     (1UL) /**< @brief 100 Megabits per second */
#define ETHER_SPEED_1000MBPS    (2UL) /**< @brief 1000 Megabits per seconds */
/** @} */

/**
    @name ETHER_DuplexModeType
    @{
    @brief Ethernet controller(MAC) duplex mode

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_DuplexModeType;  /**< @brief ETHER_DuplexMode typedef */
#define ETHER_DUPLEX_MODE_FULL  (0x0UL) /**< @brief Full duplex */
#define ETHER_DUPLEX_MODE_HALF  (0x1UL) /**< @brief Half duplex */
/** @} */

/**
    @name ETHER_EtherType
    @{
    @brief Ethernet ethtype field in ETHER II packet

    @trace #BRCM_SWREQ_ETHER_INITIALISATION
*/
typedef uint16_t ETHER_EtherType;         /**< @brief ETHER_EtherType typedef */
#define ETHER_ETHERTYPE_IP      (0x0800U) /**< @brief Ether Type IP */
#define ETHER_ETHERTYPE_ARP     (0x0806U) /**< @brief Ether Type ARP */
#define ETHER_ETHERTYPE_GPTP    (0x88F7U) /**< @brief Ether Type GPTP */
#define ETHER_ETHERTYPE_1722    (0x22F0U) /**< @brief Ether Type 1722 */
#define ETHER_ETHERTYPE_VLANTAG (0x8100U) /**< @brief Ether Type VLAN */
/** @} */

/**
    @name ETHER_PrioType
    @{
    @brief Ethernet packet priority type
    (priority shall be between 0 and  #ETHER_PRIO_MAX)

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint16_t ETHER_PrioType; /**< @brief ETHER_Prio typedef */
#define ETHER_PRIO_MAX  (0x7UL)                             /**< @brief Maximum packet priority */
#define ETHER_PRIO_CHAN_MAP_SZ      (ETHER_PRIO_MAX + 1UL)  /**< @brief Ethernet Priority channel map size */
/** @} */

/**
    @name ETHER_ResultType
    @{
    @brief Ethernet transmit result type

    @trace #BRCM_SWREQ_ETHER_INDICATIONS
*/
typedef uint32_t ETHER_ResultType;    /**< @brief ETHER_Result typedef */
#define ETHER_RESULT_SUCCESS    (1UL) /**< @brief Transmit successful */
#define ETHER_RESULT_FAILURE    (2UL) /**< @brief Transmit failure */
/** @} */

/**
    @name ETHER_VLANIDType
    @{
    @brief 802.1Q VLAN ID type

    VLAN range 0x0 to 4095 where 0x000 and 0xFFF are reserved.

    @trace #BRCM_SWREQ_ETHER_INITIALISATION
*/
typedef uint16_t ETHER_VLANIDType;       /**< @brief 802.1Q VLAN ID type */
#define ETHER_VLANID_MIN     (1UL)       /**< @brief Minimum VLAN ID */
#define ETHER_VLANID_MAX     (4094UL)    /**< @brief Maximum VLAN ID */
/** @} */

/**
    @name ETHER_TimestampQualType
    @{
    @brief Ethernet transmit result type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_TimestampQualType; /**< @brief ETHER_TimestampQual typedef */
#define ETHER_TIMESTAMPQUAL_VALID      (0x0UL)   /**< @brief Valid timestamp  */
#define ETHER_TIMESTAMPQUAL_INVALID    (0x1UL)   /**< @brief Invalid timestamp */
#define ETHER_TIMESTAMPQUAL_UNCERTAIN  (0x2UL)   /**< @brief Uncertain timestamp */
/** @} */

/**
    @name ETHER_TimeSyncType
    @{
    @brief Ethernet Time hardware synchronization type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_TimeSyncType;        /**< @brief ETHER_TimeSync typedef */
#define ETHER_TIMESYNC_NONE        (0x0UL) /**< @brief No synchronization */
#define ETHER_TIMESYNC_INTERNAL    (0x1UL) /**< @brief Internal synchronization */
#define ETHER_TIMESYNC_EXTERNAL    (0x2UL) /**< @brief External synchronization */
/** @} */

/**
    @name ETHER_TimeInitCmdType
    @{
    @brief Ethernet time initialisation command type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
*/
typedef uint32_t ETHER_TimeInitCmdType;           /**< @brief ETHER_TimeInitCmd typedef */
#define ETHER_TIMEINITCMD_0               (1UL) /**< @brief Initialization
                                                      command - 0 */
#define ETHER_TIMEINITCMD_PREP_SYNC       (2UL) /**< @brief Initialization
                                                      command - prepare for
                                                      synchronization */
#define ETHER_TIMEINITCMD_PREP_HB_READ    (3UL) /**< @brief Initialization
                                                      command - prepare for
                                                      heart-beat read */
#define ETHER_TIMEINITCMD_GET_HB          (4UL) /**< @brief Initialization
                                                      command – get heart-beat */
#define ETHER_TIMEINITCMD_1               (5UL) /**< @brief Initialization
                                                      command - 1 */
#define ETHER_TIMEINITCMD_2               (6UL) /**< @brief Initialization
                                                        command - 2 */
/** @} */

/**
    @brief Timestamp information

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TimestampType {
    uint64_t seconds;     /**< @brief seconds component of time */
    uint32_t nanoseconds; /**< @brief nanoseconds component of time */
} ETHER_TimestampType;

/**
    @brief Structure to carry time difference

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TimestampDiffType {
    ETHER_TimestampType absTimestampDiff; /**< @brief Absolute value of time difference */
    uint32_t isNegative;                  /**< @brief Negative or positive time difference */
} ETHER_TimestampDiffType;

/**
    @brief Structure to carry rate ratio

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_RateRatioType {
    ETHER_TimestampDiffType localDiff;       /**< @brief Time difference between
                                                   two RX timestamps at the
                                                   local end */
    ETHER_TimestampDiffType grandMasterDiff; /**< @brief Time difference between
                                                   the corresponding TX
                                                   timestamps at the grand master */
} ETHER_RateRatioType;

/**
    @brief Ethernet RX packet statistics

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_RxStatsType {
    uint32_t gdPkts;          /**< @brief All good packets */
    uint32_t octetsLow;       /**< @brief Octets low packets */
    uint32_t octetsHigh;      /**< @brief octets high packets */
    uint32_t allPkts;         /**< @brief All good or bad packets */
    uint32_t brdCast;         /**< @brief All good broadcast packets */
    uint32_t mutCast;         /**< @brief All good multicast packets */
    uint32_t uniCast;         /**< @brief All good unicast packets */
    uint32_t pkts64;          /**< @brief All good/bad packets with len < 64 */
    uint32_t pkts65_127;      /**< @brief All good/bad packets with len between
                                    65 to 127 */
    uint32_t pkts128_255;     /**< @brief All good/bad packets with len between
                                    128 to 255 */
    uint32_t pkts256_511;     /**< @brief All good/bad packets with len between
                                    256 to 511 */
    uint32_t pkts512_1023;    /**< @brief All good/bad packets with len between
                                    512 to 1023 */
    uint32_t pkts1024_MAX;    /**< @brief All good/bad packets with len between
                                    1024 and the standard maximum packet size
                                    inclusive */
    uint32_t pkts1024_1522;   /**< @brief All good/bad packets with len between
                                    1024 to 1522 */
    uint32_t pkts1523_2047;   /**< @brief All good/bad packets with len between
                                    1523 to 2047 */
    uint32_t pkts2048_4095;   /**< @brief All good/bad packets with len between
                                    2048 to 4095 */
    uint32_t pkts4096_8191;   /**< @brief All good/bad packets with len between
                                    4095 to 8191 */
    uint32_t pkts8192_MAX;    /**< @brief All good/bad packets with len between
                                    8192 to MAX */
    uint32_t pktsJabber;      /**< @brief All pkts with length>1536 and a bad CRC */
    uint32_t pktsOvrSz;       /**< @brief Rx pkts with length > RX_LENGTH and a
                                    good CRC */
    uint32_t pktsFrag;        /**< @brief All pkts with length < 64 and a bad CRC */
    uint32_t pktsRxDrop;      /**< @brief All pkts that experienced an overrun */
    uint32_t pktsCrcAlignErr; /**< @brief All pkts with a bad CRC or odd number
                                    of nibbles */
    uint32_t pktsUndSz;       /**< @brief All pkts with length < 64 with a good CRC */
    uint32_t pktsCrcErr;      /**< @brief All pkts that did not pass FCS check */
    uint32_t pktsRxDiscard;   /**< @brief All pkts that did not pass FCS check */
    uint32_t rxPause;         /**< @brief Number of PAUSE events */
} ETHER_RxStatsType;

/**
    @brief Ethernet TX packet statistics

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TxStatsType {
    uint32_t octets;               /**< @brief Total number of bytes transmitted */
    uint32_t brdCast;              /**< @brief All good broadcast packets */
    uint32_t mutCast;              /**< @brief All good multicast packets */
    uint32_t uniCast;              /**< @brief All good unicast packets */
    uint32_t txDropped;            /**< @brief All dropped packets */
    uint32_t txDroppedErr;         /**< @brief All packets dropped due to error */
    uint32_t txCollision;          /**< @brief TX collisions */
    uint32_t txCollisionSingle;    /**< @brief Tx single collision */
    uint32_t txCollisionMulti;     /**< @brief Tx Multi collision counter */
    uint32_t txLateCollision;      /**< @brief Tx late collison counter */
    uint32_t txExcessiveCollision; /**< @brief Tx excessive collision counter */
    uint32_t txDeferredTransmit;   /**< @brief Tx deferred transmit counter */
    uint32_t txFrameInDiscard;     /**< @brief Tx deferred transmit counter */
    uint32_t txPause;              /**< @brief Number of PAUSE events */
    uint32_t txQ0;                 /**< @brief Total number of good packets
                                         transmitted on COS0 */
    uint32_t txQ1;                 /**< @brief Total number of good packets
                                         transmitted on COS1 */
    uint32_t txQ2;                 /**< @brief Total number of good packets
                                         transmitted on COS2 */
    uint32_t txQ3;                 /**< @brief Total number of good packets
                                         transmitted on COS3 */
    uint32_t txQ4;                 /**< @brief Total number of good packets
                                         transmitted on COS4 */
    uint32_t txQ5;                 /**< @brief Total number of good packets
                                         transmitted on COS5 */
    uint32_t txQ6;                 /**< @brief Total number of good packets
                                         transmitted on COS6 */
    uint32_t txQ7;                 /**< @brief Total number of good packets
                                         transmitted on COS7 */
    uint32_t pkts64;               /**< @brief All good/bad packets with len < 64 */
    uint32_t pkts65_127;           /**< @brief All good/bad packets with len
                                         between 65 to 127 */
    uint32_t pkts128_255;          /**< @brief All good/bad packets with len
                                         between 128 to 255 */
    uint32_t pkts256_511;          /**< @brief All good/bad packets with len
                                         between 256 to 511 */
    uint32_t pkts512_1023;         /**< @brief All good/bad packets with len
                                         between 512 to 1023 */
    uint32_t pkts1024_MAX;         /**< @brief All good/bad packets with len
                                         between 1024 and the standard maximum
                                         packet size inclusive */
} ETHER_TxStatsType;

/**
    @brief Ethernet time configuration type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TimeCfgType {
    uint32_t hwIdx;          /**< @brief Timer hardware instance index for Timer type */
    uint32_t portEnableMask; /**< @brief Timer hardware port enable mask */
    ETHER_TimeSyncType sync; /**< @brief Timer hardware synchronization type */
} ETHER_TimeCfgType;

/**
    @brief Ethernet time initialization output type

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_TimeInitOutType {
    uint16_t heartBeat0[ETHER_HEART_BEAT_CNT_MAX]; /**< @brief Array for
                                                         heartbeat for P1588
                                                         instance 0 */
    uint16_t heartBeat1[ETHER_HEART_BEAT_CNT_MAX]; /**< @brief Array for
                                                         heartbeat for P1588
                                                         instance 1 */
} ETHER_TimeInitOutType;

/**
    @brief Ethernet controller configuration structure

    @trace #BRCM_SWREQ_ETHER_KERNEL_INTERFACE
 */
typedef struct _ETHER_CfgType {
    ETHER_HwIDType hwID;             /**< @brief Ethernet Controller Index */
    ETHER_SpeedType speed;           /**< @brief Ethernet controller(MAC) speed
                                           (default) */
    ETHER_DuplexModeType duplexMode; /**< @brief Ethernet controller(MAC) duplex
                                           mode (default) */
    uint32_t maxRxFrmLen;            /**< @brief Maximum size of RX frame length.
    @note maxRxFrmLen shall include FCS size. e.g.
    Ethernet II frame size = [12 (mac src + dest addr) + 2 (ether type) +
                                1500 (payload) + 4 (VLAN tag) + 4 (CRC)]
                           = 1522 Bytes */
    const uint8_t *macAddr;          /**< @brief Ethernet controller MAC address
                                           in network byte order */
    const ETHER_ChanIDType prioChanMap[ETHER_PRIO_CHAN_MAP_SZ]; /**< @brief Priority to ethernet channel map */
} ETHER_CfgType;

/** @brief Initialize Ethernet driver

    This API initializes ethernet controller and configures corresponding
    DMA descriptors.

    On driver initialization, controller will be set #ETHER_MODE_DOWN. To
    make controller mode to #ETHER_MODE_ACTIVE, explicit #ETHER_SetMode
    call is required with #ETHER_MODE_ACTIVE mode.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aConfig     Pointer to ethernet configurations

    @return     void

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations None
*/
extern void ETHER_Init(ETHER_HwIDType aID,
                       const ETHER_CfgType *const aConfig);

/** @brief De-initialize Ethernet driver

    This API de-initializes ethernet driver. Stops all pending ethernet packet
    transfers & stops receiving ethernet packets. Clears all transmit & receive
    buffers and DMA descriptors.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Ethernet controller ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-initialization successful
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Ethernet controller ID
    @retval     #BCM_ERR_UNINIT         Ethernet driver already uninitialized

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations
    - Buffers queued to Ethernet driver for transfer using #ETHER_Send and  are
    still pending for transfer at the time of #ETHER_DeInit call will not be
    transferred. Transmit completion callbacks for buffers not transferred by
    Ethernet driver shall not be raised.
    - After #ETHER_DeInit call, all the buffer will be owned by Ethernet driver.
    Client shall not use buffers allocated using #ETHER_GetTxBuffer after
    #ETHER_DeInit call for that channel.
*/
extern int32_t ETHER_DeInit(ETHER_HwIDType aID);

/** @brief Set controller mode

    This API sets mode of the given Ethernet controller.

    @note Packet transfer and reception will start on setting controller mode to
    #ETHER_MODE_ACTIVE after #ETHER_Init call.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aMode       Mode to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Mode set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller mode
    @retval     #BCM_ERR_UNINIT         Ethernet driver in uninitialized state

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations Buffers queued to Ethernet driver for transfer using #ETHER_Send
    may not get completed if the controller mode is set to #ETHER_MODE_DOWN.
    Also, completion callback may not be raised for those pending buffers which
    are not transferred.
*/
extern int32_t ETHER_SetMode(ETHER_HwIDType aID,
                             ETHER_ModeType aMode);

/** @brief Get controller mode

    This API retrieves current mode of the given Ethernet controller.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[out]     aMode       Pointer to retrieve mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Mode retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_UNINIT         Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations None
*/
extern int32_t ETHER_GetMode(ETHER_HwIDType aID,
                             ETHER_ModeType *const aMode);

/** @brief Set MAC address

    This API sets MAC address (Ethernet address) of the given Ethernet controller

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aMacAddr    Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MAC address is set successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL
    @retval     #BCM_ERR_UNINIT         Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations None
*/
extern int32_t ETHER_SetMacAddr(ETHER_HwIDType aID,
                                const uint8_t *const aMacAddr);

/** @brief Get MAC address

    This API retrieves MAC address (Ethernet address) of the given Ethernet controller.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[out]     aMacAddr    Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MAC address is retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL
    @retval     #BCM_ERR_UNINIT         Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_INITIALISATION

    @limitations None
*/
extern int32_t ETHER_GetMacAddr(ETHER_HwIDType aID,
                                uint8_t *const aMacAddr);

/** @brief Get transmit buffer

    This API provides free TX buffer to client.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Ethernet controller ID
    @param[in]      aPriority   Priority of TX packet
    @param[out]     aBufIdx     Buffer index
    @param[out]     aBuf        Pointer to TX buffer
    @param[inout]   aLenInout   Length of the requested buffer (in),
                                Length of the allocated buffer (out)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             TX buffer successfully obtained
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS   aPriority greater than ETHER_PRIO_MAX
    @retval     #BCM_ERR_INVAL_PARAMS   aBufIdx is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aLenInOut is NULL
    @retval     #BCM_ERR_NOMEM          No TX buffers are available
    @retval     #BCM_ERR_INVAL_STATE    Invalid controller state
    @retval     #BCM_ERR_UNINIT         Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_COMMUNICATION

    @limitations None
*/
extern int32_t ETHER_GetTxBuffer(ETHER_HwIDType aID,
                                 ETHER_PrioType aPriority,
                                 uint32_t *const aBufIdx,
                                 uint8_t **const aBuf,
                                 uint32_t *const aLenInOut);

/** @brief Queue a Ethernet packet for transmission

    This API queues an Ethernet packet for transmission.

    @behavior Async, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTxConfirmation     TX confirmation flag
                                        - 1UL: Ethernet driver gives the callback
                                        notification with the registered callback
                                        when the packet is sent. (The packet
                                        buffer is not released until the
                                        callback returns)
                                        - 0UL: No callback provided by driver
                                        when packet is sent
    @param[in]      aBufIdx             Buffer index (received by calling
                                        #ETHER_GetTxBuffer)
    @param[in]      aLen                Length of the data in bytes. If this API
                                        is called with aLen as zero, the packet
                                        is not transmitted, instead the buffer
                                        (as given by aBufIdx) is returned back
                                        to the Ethernet driver.
    @param[in]      aDestMacAddr        Pointer to destination MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Packet is queued successfully
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aLen is more than Ethernet frame size
    @retval     #BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state
    @retval     #BCM_ERR_BUSY               No room in DMA descriptor table
    @retval     #BCM_ERR_INVAL_STATE        Invalid controller state
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_COMMUNICATION

    @limitations None
*/
extern int32_t ETHER_Send(ETHER_HwIDType aID,
                          uint32_t aTxConfirmation,
                          uint32_t aBufIdx,
                          uint32_t aLen,
                          const uint8_t *const aDestMacAddr);

/** @brief Get Ethernet RX packet statistics

    This API returns the ethernet RX packet stats for the given controller

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[out]     aStats              Pointer to #ETHER_RxStatsType structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Ethernet RX packet statistics
                                            successfully retrieved
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       aStats is NULL
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_COMMUNICATION

    @limitations None
*/
extern int32_t ETHER_GetRxStats(ETHER_HwIDType aID,
                                ETHER_RxStatsType *const aStats);

/** @brief Enable Timestamping for transmit packet

    This API enables timestamping for a particular transmit buffer

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully enabled timestamp for
                                            the given buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state
    @retval     #BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state.
                                            The buffer should have been received
                                            from an #ETHER_GetTxBuffer call and
                                            not yet transmitted using #ETHER_Send
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations This API shall be invoked only for transmit buffers containing
    gPTP event messages (SYNC, PDELAY REQUEST, PDELAY RESPONSE)
*/
extern int32_t ETHER_EnableTxTimestamp(ETHER_HwIDType aID,
                                       uint32_t aBufIdx);

/** @brief Retrieve TX timestamp

    This API fetches the transmit timestamp captured by the hardware.

    @note This API shall be invoked in the transmit confirmation callback

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aBufIdx             Buffer index (received in the transmit
                                        confirmation)
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was transmitted. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 TX timestamp successfully retrieved
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       aTS is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aTSQual is NULL
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state
    @retval     #BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state.
                                            (API was invoked outside the transmit
                                            confirmation callback).
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations
    - #ETHER_EnableTxTimestamp shall be invoked for this buffer before transmission.
    - Transmission confirmation shall be enabled for this buffer in the
    #ETHER_Send call
*/
extern int32_t ETHER_GetTxTimestamp(ETHER_HwIDType aID,
                                    uint32_t aBufIdx,
                                    ETHER_TimestampType* const aTS,
                                    ETHER_TimestampQualType* const aTSQual);

/** @brief Retrieve RX timestamp

    This API fetches the RX timestamp captured by the hardware.
    This API shall be invoked only once in in the RX indication callback.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPktBuf             Pointer to the received packet buffer
    @param[out]     aTS                 Timestamp captured at the instant the
                                        packet was received. This is derived
                                        from the Ethernet time base.
    @param[out]     aTSQual             Quality of the timestamp. Application
                                        may chose to ignore the timestamps which
                                        do not have quality value #ETHER_TIMESTAMPQUAL_VALID.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 RX timestamp successfully retrieved
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       aPktBuf is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aTS is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aTSQual is NULL
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations Since the hardware only timestamps gPTP SYNC, PDELAY REQUEST
    and PDELAY RESPONSE packets, this API returns valid timestamp only for the
    aforementioned packets.
*/
extern int32_t ETHER_GetRxTimestamp(ETHER_HwIDType aID,
                                    const uint8_t *const aPktBuf,
                                    ETHER_TimestampType *const aTS,
                                    ETHER_TimestampQualType *const aTSQual);

/** @brief Retrieve current Ethernet time

    This API retrieves the Ethernet current time

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[out]     aTS                 Current Ethernet time
    @param[out]     aTSQual             Quality of the retrieved time

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully retrieved current
                                            Ethernet time
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       aTS is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aTSQual is NULL
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations None
*/
extern int32_t ETHER_GetTime(ETHER_HwIDType aID,
                             ETHER_TimestampType *const aTS,
                             ETHER_TimestampQualType *const aTSQual);

/** @brief Set Ethernet time

    This API sets the Ethernet time

    @note For time slaves the #ETHER_SetCorrectionTime API shall be used

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTS                 New Ethernet time to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Ethernet time successfully set
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       aTS is NULL
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations This API shall be invoked only for node operating as time master
*/
extern int32_t ETHER_SetTime(ETHER_HwIDType aID,
                             const ETHER_TimestampType *const aTS);

/** @brief Adjust Ethernet time

    This API adjusts the the local Ethernet clock on the basis of timing
    messages received from the time-master

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aTD                 Time difference between time-master
                                        time and local Ethernet time
    @param[in]      aRR                 Rate ratio between the time master and
                                        local clock

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Ethernet time corrected successfully
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID
    @retval     #BCM_ERR_INVAL_PARAMS       aTD is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aRR is NULL
    @retval     #BCM_ERR_INVAL_STATE        Invalid driver state

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations This API shall only be invoked for nodes operating as time slaves.
*/
extern int32_t ETHER_SetCorrectionTime(ETHER_HwIDType aID,
                                       const ETHER_TimestampDiffType* const aTD,
                                       const ETHER_RateRatioType* const aRR);

/** @brief Ethernet time initialization

    This API performs the first part of the initialization of Ethernet time module.
    @note For completed initialization ETHER_TimeInitStateHandler() with
    appropriate commands.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aCfg                Ethernet Time configuration

    @return     void

    @post None

    @trace #BRCM_SWREQ_ETHER_TIME

    @limitations None
*/
extern void ETHER_TimeInit(ETHER_HwIDType aID,
                           ETHER_TimeCfgType *const aCfg);

/** @brief Set the Ethernet general purpose timer

    This API sets the Ethernet general purpose timer with the provided
    timer period. A period of 0 disables the timer.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID                 Ethernet controller ID
    @param[in]      aPeriod             Timer period in nanoseconds

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Timer successfully enabled
    @retval     #BCM_ERR_INVAL_PARAMS       aPeriod is invalid
    @retval     #BCM_ERR_UNINIT             Ethernet is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid Ethernet controller ID

    @post None

    @trace #BRCM_SWREQ_ETHER_GPTIME

    @limitations None
*/
extern int32_t ETHER_SetGPTimer(ETHER_HwIDType aID,
                                uint32_t aPeriod);

/** @brief MDIO read access

    This API queues a MDIO read access request

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         MDIO hardware controller ID
    @param[inout]   aPkt        Pointer to MDIO packet
    @param[out]     aJobID      Pointer to retrieve job identifier for read job.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully initiated mdio read operation
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aPkt is NULL) or
                                        (aJobID is NULL) or
                                        (aPkt->slaveAddr is invalid) or
                                        (aPkt->deviceType is invalid) or
                                        (aPkt->regAccType is invalide) or
                                        (aPkt->regAddr is invalid)
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_NOMEM          Driver job queue is full.

    @post TBD

    @trace #BRCM_SWREQ_ETHER_MDIO

    @limitations None
*/
extern int32_t ETHER_MdioRead(MDIO_HwIDType aID,
                    MDIO_PktType *aPkt,
                    MDIO_JobIDType *aJobID);

/** @brief MDIO write access

    This API queues a MDIO write access request

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         MDIO hardware controller ID
    @param[inout]   aPkt        Pointer to MDIO packet
    @param[out]     aJobID      Pointer to retrieve job identifier for write job

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully initiated mdio write operation
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aPkt is Null) or
                                        (aJobID is NULL) or
                                        (aPkt->slaveAddr is invalid) or
                                        (aPkt->deviceType is invalid) or
                                        (aPkt->regAccType is invalide) or
                                        (aPkt->regAddr is invalid)
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_NOMEM          Driver job queue is full.

    @post None

    @trace #BRCM_SWREQ_ETHER_MDIO

    @limitations None
*/
extern int32_t ETHER_MdioWrite(MDIO_HwIDType aID,
                        MDIO_PktType *aPkt,
                        MDIO_JobIDType *aJobID);

/** @brief Check Status of MDIO access

    This API checks the status of MDIO status initiated through ETHER_MdioRead
    and ETHER_MdioWrite APIs

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID            MDIO hardware controller ID
    @param[in]      aJobID         MDIO job ID as returned by ETHER_MdioRead or
                                   ETHER_MdioWrite APIs
    @param[inout]   aPkt           MDIO Packet information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_BUSY           Job is pending or in progress.
    @retval     #BCM_ERR_TIMEOUT        Timeout occured while process the job.
                                        This job and all pending job for this
                                        controller would not completed.

    @post None

    @trace #BRCM_SWREQ_ETHER_MDIO

    @limitations None
*/
extern int32_t ETHER_MdioCheckStatus(MDIO_HwIDType aID,
                            MDIO_JobIDType aJobID,
                            MDIO_PktType *aPkt);
#endif /* ETHERNET_H */

/** @} */
