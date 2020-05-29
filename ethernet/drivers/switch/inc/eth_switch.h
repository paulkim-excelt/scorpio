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
    @defgroup grp_eth_switchdrv_ifc Ethernet Switch Driver Interfaces
    @ingroup grp_eth_switchdrv

    @addtogroup grp_eth_switchdrv_ifc
    @{
    @section sec_eth_switch_overview Overview
    @image html eth_switch_overview.jpg "Ethernet Switch driver architecture"
    An Ethernet switch consists of several ports of different types. Ethernet
    switch driver interface provides a hardware independent interface to the
    user to configure the switch. Switch driver does not configure the
    underlying ports of the switch. It is configured by external entity in the
    system (transceiver driver or Bootloader). Switch driver provides static
    configuration of the switch (per port basis) during the initialization
    (#ETHERSWT_Init).

    Picture below depicts the switch configuration structure layout
    (#ETHERSWT_CfgType):
    @image html eth_switch_cfg_layout.jpg "Switch Configuration"

    Switch configuration consistis of per port configuration
    (#ETHERSWT_PortCfgType), global CFP rules and UDF rules.

    A typical L2 switch consists of fixed number of ports and provides basic
    features like:
    - ARL learning (hardware learning/software learning)
    - MAC based forwarding of the packet
    - VLAN based forwarding of the packet
    - Priority based forwarding mechanism (PCP field based)
    - ARL table control (static Addition/deletion of MAC address)

    Switch configuration structure (#ETHERSWT_CfgType) provides the mechanism to
    configure these features of switch on per port basis. Configuration of the
    each port is modelled by #ETHERSWT_PortCfgType structure.

    @c ETHERSWT_PortCfgType::fixedMacAddrList field of port configuration
    structure provides the mechanism to add MAC addresses into the switch ARL
    table statically (never aged out).

    @c ETHERSWT_PortCfgType::vlanMemList field of the port configuration
    structure provides the mechanism for the port to participate in VLANs. This
    membership defines the ingress, egress and forwarding behavior. This
    configuration also defines the Tagging/Un-tagging behavior of the port.
    Table below shows an example of the forwarding behavior within 8-port
    switch:
    <table>
    <caption id="fwd_behaviour">Forwarding Behaviour witin 8-port switch
    </caption>
    <tr><th rowspan="2"> VLAN ID <th colspan="8"> Port Number
    <tr><th>0<th>1<th>2<th>3<th>4<th>5<th>6<th>7
    <tr><td>1<td>Tagged<td>Tagged<td>-<td>-<td>Untagged<td>-<td>-<td>-
    <tr><td>2<td>Tagged<td>-<td>-<td>-<td>Tagged<td>-<td>-<td>-
    <tr><td colspan="9">...
    <tr><td>4094<td>..<td>..<td>..<td>..<td>..<td>..<td>..<td>..
    </table>

    Behavior of Incoming packets with VLAN ID “1”:
    - Forward it to port 0 and 1 tagged
    - Forward it to port 4 as untagged
    - Don’t forward this packet to port 2, 3, 5, 6 and 7

    Similarly behavior of the incoming packet with VLAN ID “2”:
    - Forward it to port 0 and 4 as tagged
    - Don’t forward this packet to rest of the ports

    VLAN ID is modelled by #ETHERSWT_VLANMemberCfgType::vlandID.

    Forwarding behavior as Don’t send/Tagged/Untagged is modelled by
    #ETHERSWT_VLANMemberCfgType::forward.

    @anchor eth_switch_port_forward_config
    Apart from above VLAN based forwarding behavior, a port can be configured
    to:
    -# <b>Drop all the untagged packets</b>: This is modelled by @c dropUntagged
       field of the ingress port configuration structure. If set to #TRUE, all
       the untagged packets will be dropped at the ingress side of the port.
    -# <b>Forward untagged (forward the packet as is)</b>:  This is modelled by
       @c dropUntagged field of the ingress port configuration structure. If set
       to #FALSE, all the untagged packets will be forwarded untagged to
       egress port.
    -# <b>Forward tagged</b>: Tag all the untagged packets arriving at this port
       with default VLAN and default priority. This is modelled by
       @c defaultVLAN and @c defaultPrio field of the ingress port configuration
       structure.

    @c #ETHERSWT_PortCfgType::ingressCfg field of the port configuration
    structure defines the Port ingress configuration and
    #ETHERSWT_PortCfgType::egressCfg field of the port configuration structure
    defines the egress configuration of the port.

    Switch consists of fixed number of ports and it assumed that each port
    contains fixed number of the egress FIFOs (queues) where incoming packets
    are buffered/queued. Configuration of the each FIFO is modelled by
    #ETHERSWT_FifoCfgType structure. Incoming packets are processed depending
    on the shaping and scheduling algorithm of the FIFO. Shaper configuration
    provides mechanism to control the burst size; minimum length and byte rate
    of the FIFO. Configuration of shaping algorithm of the FIFO is modelled by
    #ETHERSWT_ShaperCfgType structure. Scheduling algorithm of the Port is
    modelled by #ETHERSWT_PortSchType configuration structure.

    Apart from the modelling of the egress port configuration, it is also
    important to understand how the priority based routing is configured in the
    switch. As an example, Ethernet priority field (PCP field) of the packet can
    be evaluated and mapped to a so-called Traffic class. Each traffic class can
    then be mapped to egress FIFO. Another example is each port can have fixed
    traffic class (represented by @c tc field of the ingress port configuration
    structure #ETHERSWT_PortIngressCfgType), where each incoming packet is
    treated at the same priority.

    So for the routing of the packet, there are two kinds of mapping which are
    possible:
    -# <b>Ingress port to traffic class mapping</b>: In this method, port
       traffic class is fixed. This is modelled by @c tc field of the ingress
       port configuration structure. Table below shows an example:
       |Port    |Traffic Class  |
       |:------:|:-------------:|
       |1,2,3   |7              |
       |4,5     |6              |
       |6       |5              |
       |7,8     |1              |
    -# <b>PCP to traffic class mapping</b>: In this method, PCP field of the
       incoming packet is evaluated a mapped to a traffic class. This mapping is
       modelled by @c pcp2tcMap table in the ingress port configuration
       structure. Table below shows an example:
       |PCP     |Traffic Class  |
       |:------:|:-------------:|
       |0       |7              |
       |1,2     |6              |
       |3       |5              |
       |4,5,6,7 |1              |

    After mapping the incoming packet to a traffic class, they are mapped to
    certain FIFO at the egress side of the switch. This table is modelled by
    @c tc field of port FIFO configuration structure (#ETHERSWT_FifoCfgType).
    Table shows an example:
       |Traffic Class   |FIFO   |
       |:--------------:|:-----:|
       |7               |0      |
       |6               |2      |
       |5,4             |1      |
       |3,2,1,0         |3      |

    @section sec_eth_switch_fsm State Machine
    @image html eth_switch_state_machine.jpg "Ethernet Switch state machine"
    API list for ETHERSWT_STATE_INIT to ETHERSWT_STATE_INIT transition:
    - #ETHERSWT_GetPortMode
    - #ETHERSWT_SetPortMode
    - #ETHERSWT_GetPortLinkState
    - #ETHERSWT_GetPortSpeed
    - #ETHERSWT_GetPortDuplexMode
    - #ETHERSWT_GetPortMacAddr
    - #ETHERSWT_GetARLTable
    - #ETHERSWT_ReadReg
    - #ETHERSWT_WriteReg
    - #ETHERSWT_EnableVLAN
    - #ETHERSWT_SetMACLearningMode
    - #ETHERSWT_GetMACLearningMode
    - #ETHERSWT_SetPortMirrorConfig
    - #ETHERSWT_GetPortMirrorConfig
    - #ETHERSWT_SetPortMirrorState
    - #ETHERSWT_GetPortMirrorState
    - #ETHERSWT_GetRxStat
    - #ETHERSWT_GetTxStat
    - #ETHERSWT_GetSQIValue

    @section sec_eth_switch_seq Sequence Diagrams
    @subsection subsec_eth_switch_seq_tx Switch Driver – Ethernet Driver TX
    The figure below depicts Ethernet driver and Switch driver call sequence for
    transmit packet in case of Switch Management mode. In case of
    non-timestamped packet omit #ETHERSWT_EnableTxTimestamp and
    #ETHERSWT_TxTSInd call sequence.
    @image html eth_switch_seq_tx.jpg "Switch – Ethernet TX timestamp sequence"

    @subsection subsec_eth_swtich_seq_rx Switch Driver – Ethernet Driver RX
    The figure below depicts Ethernet driver and Switch driver call sequence for
    time-stamped receive packet in case of Switch Management mode. In case of
    non-timestamped packet omit #ETHERSWT_RxTSInd call sequence.
    @image html eth_switch_seq_rx.jpg "Switch – Ethernet RX timestamp sequence"

    @section sec_eth_switch_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          | NA                                                |
    | Memory Bandwidth  | NA                                                |
    | HW Utilization    | NA                                                |

    @includedoc drivers_eth_switch.dep

    @limitations None

    @file eth_switch.h
    @brief Application programmer interface for Ethernet Switch Driver
    This header file contains the interfaces for Ethernet Switch Driver
    @version 0.51 Imported from docx
 */

#ifndef ETH_SWITCH_H
#define ETH_SWITCH_H

#include <ethernet.h>
#include <eth_xcvr.h>
#include <compiler.h>

/**
    @name Ethernet Switch driver API IDs
    @{
    @brief API IDs for  Ethernet Switch driver
*/
#define BRCM_SWARCH_ETHERSWT_PORT_ID_MAX_MACRO            (0x00U) /**< @brief #ETHERSWT_PORT_ID_MAX */
#define BRCM_SWARCH_ETHERSWT_PORT_FIFO_MAX_MACRO          (0x01U) /**< @brief #ETHERSWT_PORT_FIFO_MAX */
#define BRCM_SWARCH_ETHERSWT_PORT_MAC_ENTY_MAX_MACRO      (0x02U) /**< @brief #ETHERSWT_PORT_MAC_ENTY_MAX */
#define BRCM_SWARCH_ETHERSWT_PORT_VLAN_ENTY_MAX_MACRO     (0x03U) /**< @brief #ETHERSWT_PORT_VLAN_ENTY_MAX */
#define BRCM_SWARCH_ETHERSWT_HW_ID_TYPE                   (0x04U) /**< @brief #ETHERSWT_HwIDType */
#define BRCM_SWARCH_ETHERSWT_PORT_ID_TYPE                 (0x05U) /**< @brief #ETHERSWT_PortIDType */
#define BRCM_SWARCH_ETHERSWT_VLAN_ID_TYPE                 (0x06U) /**< @brief #ETHERSWT_VLANIDType */
#define BRCM_SWARCH_ETHERSWT_PCP_TYPE                     (0x07U) /**< @brief #ETHERSWT_PCPType */
#define BRCM_SWARCH_ETHERSWT_PORT_TYPE                    (0x08U) /**< @brief #ETHERSWT_PortType */
#define BRCM_SWARCH_ETHERSWT_TC_TYPE                      (0x09U) /**< @brief #ETHERSWT_TCType */
#define BRCM_SWARCH_ETHERSWT_PORT_POLICER_ACT_TYPE        (0x0AU) /**< @brief #ETHERSWT_PortPolicerActType */
#define BRCM_SWARCH_ETHERSWT_VLAN_FWRD_TYPE               (0x0BU) /**< @brief #ETHERSWT_VLANFwrdType */
#define BRCM_SWARCH_ETHERSWT_SCHED_ALGO_TYPE              (0x0CU) /**< @brief #ETHWERSWT_SchedAlgoType */
#define BRCM_SWARCH_ETHERSWT_MAC_LEARN_MODE_TYPE          (0x0DU) /**< @brief #ETHERSWT_MacLearningMode */
#define BRCM_SWARCH_ETHERSWT_POLICER_FLAG_TYPE            (0x0EU) /**< @brief #ETHERSWT_PolicerPacketFlgMaskType */
#define BRCM_SWARCH_ETHERSWT_PORT_MIRROR_STATE_TYPE       (0x0FU) /**< @brief #ETHERSWT_PortMirrorStateType */
#define BRCM_SWARCH_ETHERSWT_LED_TYPE                     (0x10U) /**< @brief #ETHERSWT_LedType */
#define BRCM_SWARCH_ETHERSWT_SWITCH_TYPE                  (0x11U) /**< @brief #ETHERSWT_SwitchType */
#define BRCM_SWARCH_ETHERSWT_MGMT_INFO_TYPE               (0x12U) /**< @brief #ETHERSWT_MgmtInfoType */
#define BRCM_SWARCH_ETHERSWT_ARL_ENTRY_TYPE               (0x15U) /**< @brief #ETHERSWT_ARLEntryType */
#define BRCM_SWARCH_ETHERSWT_VLAN_MEM_CFG_TYPE            (0x16U) /**< @brief #ETHERSWT_VLANMemberCfgType */
#define BRCM_SWARCH_ETHERSWT_TRAFFIC_DIR_MACRO            (0x17U) /**< @brief #TRAFFIC_DIRECTION_INGRESS */
#define BRCM_SWARCH_ETHERSWT_PORT_MIRROR_CFG_TYPE         (0x18U) /**< @brief #ETHERSWT_PortMirrorCfgType */
#define BRCM_SWARCH_ETHERSWT_PORT_POLICER_CFG_TYPE        (0x19U) /**< @brief #ETHERSWT_PortPolicerCfgType */
#define BRCM_SWARCH_ETHERSWT_PORT_INGRESS_CFG_TYPE        (0x1AU) /**< @brief #ETHERSWT_PortIngressCfgType */
#define BRCM_SWARCH_ETHERSWT_PORT_SCH_TYPE                (0x1BU) /**< @brief #ETHERSWT_PortSchType */
#define BRCM_SWARCH_ETHERSWT_PORT_SHAPER_CFG_TYPE         (0x1CU) /**< @brief #ETHERSWT_ShaperCfgType */
#define BRCM_SWARCH_ETHERSWT_FIFO_CFG_TYPE                (0x1DU) /**< @brief #ETHERSWT_FifoCfgType */
#define BRCM_SWARCH_ETHERSWT_PORT_EGRESS_CFG_TYPE         (0x1EU) /**< @brief #ETHERSWT_PortEgressCfgType */
#define BRCM_SWARCH_ETHERSWT_PORT_CFG_TYPE                (0x1FU) /**< @brief #ETHERSWT_PortCfgType */
#define BRCM_SWARCH_ETHERSWT_CFG_TYPE                     (0x20U) /**< @brief #ETHERSWT_CfgType */
#define BRCM_SWARCH_ETHERSWT_INIT_PROC                    (0x21U) /**< @brief #ETHERSWT_Init */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_MODE_PROC           (0x22U) /**< @brief #ETHERSWT_GetPortMode */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_MODE_PROC           (0x24U) /**< @brief #ETHERSWT_SetPortMode */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_LINK_STATE_PROC     (0x25U) /**< @brief #ETHERSWT_GetPortLinkState */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_SPEED_PROC          (0x26U) /**< @brief #ETHERSWT_GetPortSpeed */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_DUPLEX_MODE_PROC    (0x27U) /**< @brief #ETHERSWT_GetPortDuplexMode */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_MAC_ADDR_PROC       (0x28U) /**< @brief #ETHERSWT_GetPortMacAddr */
#define BRCM_SWARCH_ETHERSWT_GET_ARL_TBL_PROC             (0x29U) /**< @brief #ETHERSWT_GetARLTable */
#define BRCM_SWARCH_ETHERSWT_READ_REG_PROC                (0x2AU) /**< @brief #ETHERSWT_ReadReg */
#define BRCM_SWARCH_ETHERSWT_WRITE_REG_PROC               (0x2BU) /**< @brief #ETHERSWT_WriteReg */
#define BRCM_SWARCH_ETHERSWT_ENABLE_VLAN_PROC             (0x2CU) /**< @brief #ETHERSWT_EnableVLAN */
#define BRCM_SWARCH_ETHERSWT_SET_MAC_LEARN_MODE_PROC      (0x2DU) /**< @brief #ETHERSWT_SetMACLearningMode */
#define BRCM_SWARCH_ETHERSWT_GET_MAC_LEARN_MODE_PROC      (0x2EU) /**< @brief #ETHERSWT_GetMACLearningMode */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_CFG_PROC     (0x2FU) /**< @brief #ETHERSWT_SetPortMirrorConfig */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_MIRROR_CFG_PROC     (0x30U) /**< @brief #ETHERSWT_GetPortMirrorConfig */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_STATE_PROC   (0x31U) /**< @brief #ETHERSWT_SetPortMirrorState */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_MIRROR_STATE_PROC   (0x32U) /**< @brief #ETHERSWT_GetPortMirrorState */
#define BRCM_SWARCH_ETHERSWT_GET_RX_STAT_PROC             (0x33U) /**< @brief #ETHERSWT_GetRxStat */
#define BRCM_SWARCH_ETHERSWT_GET_TX_STAT_PROC             (0x34U) /**< @brief #ETHERSWT_GetTxStat */
#define BRCM_SWARCH_ETHERSWT_GET_SQI_VAL_PROC             (0x35U) /**< @brief #ETHERSWT_GetSQIValue */
#define BRCM_SWARCH_ETHERSWT_MGMT_INIT_PROC               (0x36U) /**< @brief #ETHERSWT_MgmtInit */
#define BRCM_SWARCH_ETHERSWT_ENABLE_TX_TIMESTAMP_PROC     (0x37U) /**< @brief #ETHERSWT_EnableTxTimestamp */
#define BRCM_SWARCH_ETHERSWT_SET_MGMT_INFO_PROC           (0x38U) /**< @brief #ETHERSWT_SetMgmtInfo */
#define BRCM_SWARCH_ETHERSWT_SET_LED_STATE_PROC           (0x39U) /**< @brief #ETHERSWT_SetLedState */
#define BRCM_SWARCH_ETHERSWT_DUMBFWD_MODE_TYPE            (0x3AU) /**< @brief #ETHERSWT_DumbFwdModeType */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_SPEED_PROC          (0x3BU) /**< @brief #ETHERSWT_SetPortSpeed */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_MASTERMODE_PROC     (0x3CU) /**< @brief #ETHERSWT_GetPortMasterMode */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_MASTERMODE_PROC     (0x3DU) /**< @brief #ETHERSWT_SetPortMasterMode */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_LOOPBACKMODE_PROC   (0x3EU) /**< @brief #ETHERSWT_GetPortLoopbackMode */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_LOOPBACKMODE_PROC   (0x3FU) /**< @brief #ETHERSWT_SetPortLoopbackMode */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_JUMBOMODE_PROC      (0x40U) /**< @brief #ETHERSWT_GetPortJumboMode */
#define BRCM_SWARCH_ETHERSWT_SET_PORT_JUMBOMODE_PROC      (0x41U) /**< @brief #ETHERSWT_SetPortJumboMode */
#define BRCM_SWARCH_ETHERSWT_GET_DUMBFWDMODE_PROC         (0x42U) /**< @brief #ETHERSWT_GetDumbFwdMode */
#define BRCM_SWARCH_ETHERSWT_SET_DUMBFWDMODE_PROC         (0x43U) /**< @brief #ETHERSWT_SetDumbFwdMode */
#define BRCM_SWARCH_ETHERSWT_ENABLE_PORT_AUTONEG_PROC     (0x44U) /**< @brief #ETHERSWT_EnablePortAutoNeg */
#define BRCM_SWARCH_ETHERSWT_GET_PORT_AUTONEG_STATUS_PROC (0x45U) /**< @brief #ETHERSWT_GetPortAutoNegStatus */
#define BRCM_SWARCH_ETHERSWT_GET_XCVR_STATS_PROC          (0x46U) /**< @brief #ETHERSWT_GetXcvrStats */
#define BRCM_SWARCH_ETHERSWT_GET_LEDSTATE_PROC            (0x47U) /**< @brief #ETHERSWT_GetLedState */
#define BRCM_SWARCH_ETHERSWT_MGMT_DEINIT_PROC             (0x48U) /**< @brief #ETHERSWT_MgmtDeInit */
/** @} */

/**
    @brief Maximum number of ports supported by Ethernet switch driver.

    @trace #BRCM_SWARCH_ETHERSWT_PORT_ID_MAX_MACRO #BRCM_REQ_ETHERSWT_PORT_ID_MAX
*/
#define ETHERSWT_PORT_ID_MAX                    (9UL)

/**
    @brief Maximum number of QoS FIFO per port

    @trace #BRCM_SWARCH_ETHERSWT_PORT_FIFO_MAX_MACRO #BRCM_REQ_ETHERSWT_PORT_FIFO_MAX
*/
#define ETHERSWT_PORT_FIFO_MAX                  (8UL)

/**
    @brief Maximum number of MAC entries per port

    @trace #BRCM_SWARCH_ETHERSWT_PORT_MAC_ENTY_MAX_MACRO #BRCM_REQ_ETHERSWT_PORT_MAC_ENTY_MAX
*/
#define ETHERSWT_PORT_MAC_ENTY_MAX            (16UL)

/**
    @brief Maximum number of VLAN entries per port

    @trace #BRCM_SWARCH_ETHERSWT_PORT_VLAN_ENTY_MAX_MACRO #BRCM_REQ_ETHERSWT_PORT_VLAN_ENTY_MAX
*/
#define ETHERSWT_PORT_VLAN_ENTY_MAX          (16UL)

/**
    @name ETHERSWT_HwIDType
    @{
    @brief Switch hardware index type

    @trace #BRCM_SWARCH_ETHERSWT_HW_ID_TYPE #BRCM_REQ_ETHERSWT_HW_ID
*/
typedef uint32_t ETHERSWT_HwIDType;     /**< @brief  Switch hardware ID type */
/** @} */

/**
    @name ETHERSWT_PortIDType
    @{
    @brief Switch port index type

    @trace #BRCM_SWARCH_ETHERSWT_PORT_ID_TYPE #BRCM_REQ_ETHERSWT_PORT_ID
*/
typedef uint32_t ETHERSWT_PortIDType;   /**< @brief Switch port index type */
/** @} */

/**
    @name ETHERSWT_VLANIDType
    @{
    @brief 802.1Q VLAN ID type

    VLAN range 0x0 to 4095 where 0x000 and 0xFFF are reserved.

    @trace #BRCM_SWARCH_ETHERSWT_VLAN_ID_TYPE #BRCM_REQ_ETHERSWT_VLAN_ID
*/
typedef uint16_t ETHERSWT_VLANIDType;       /**< @brief 802.1Q VLAN ID type */
#define ETHERSWT_VLANID_MIN     (1UL)       /**< @brief Minimum VLAN ID */
#define ETHERSWT_VLANID_MAX     (4094UL)    /**< @brief Maximum VLAN ID */
/** @} */

/**
    @name ETHERSWT_VLANIngressFilterMode
    @{
    @brief VLAN ingress filter mode

    @trace #BRCM_SWARCH_ETHERSWT_VLAN_INGRESS_FILTER_MODE_TYPE #BRCM_REQ_ETHERSWT_VLAN_INGRESS_FILTER_MODE_TYPE
*/
typedef uint32_t ETHERSWT_VLANIngressFilterModeType;  /**< @brief VLAN ingress
                                                       filter mode type */
#define ETHERSWT_VLAN_INGRESS_FILTER_MODE_DISABLED   (0U)    /**< @brief Vlan
                                                 ingress filter mode disabled */
#define ETHERSWT_VLAN_INGRESS_FILTER_MODE_ENABLED    (1U)    /**< @brief Vlan
                                                 ingress filter mode enabled */
/** @} */

/**
    @name ETHERSWT_PCPType
    @{
    @brief 802.1Q Priority type

    @trace #BRCM_SWARCH_ETHERSWT_PCP_TYPE #BRCM_REQ_ETHERSWT_PCP
*/
typedef uint8_t ETHERSWT_PCPType;       /**< @brief 802.1Q Priority type */
#define ETHERSWT_PCP_0          (0UL)   /**< @brief PCP 0 */
#define ETHERSWT_PCP_1          (1UL)   /**< @brief PCP 1 */
#define ETHERSWT_PCP_2          (2UL)   /**< @brief PCP 2 */
#define ETHERSWT_PCP_3          (3UL)   /**< @brief PCP 3 */
#define ETHERSWT_PCP_4          (4UL)   /**< @brief PCP 4 */
#define ETHERSWT_PCP_5          (5UL)   /**< @brief PCP 5 */
#define ETHERSWT_PCP_6          (6UL)   /**< @brief PCP 6 */
#define ETHERSWT_PCP_7          (7UL)   /**< @brief PCP 7 */
/** @} */

/**
    @name ETHERSWT_PortType
    @{
    @brief Type of the switch port

    @trace #BRCM_SWARCH_ETHERSWT_PORT_TYPE #BRCM_REQ_ETHERSWT_PORT
*/
typedef uint8_t ETHERSWT_PortType;      /**< @brief Switch port type */
#define ETHERSWT_STANDARD_PORT  (0UL)   /**< @brief A standard Port is a
                                             normal switch port connected to
                                             a transceiver. */
#define ETHERSWT_HOST_PORT      (1UL)   /**< @brief Port is connected to Host
                                             ECU controlling the switch */
#define ETHERSWT_UP_LINK_PORT   (2UL)   /**< @brief Port is coupling port which
                                             is connected to another coupling
                                             element in the same ECU (to model a
                                             cascade switch). An uplink port
                                             have special behaviour w.r.t
                                             software and hardware. */
/** @} */

/**
    @name ETHERSWT_TCType
    @{
    @brief Traffic class type

    @trace #BRCM_SWARCH_ETHERSWT_TC_TYPE #BRCM_REQ_ETHERSWT_TC
*/
typedef uint8_t ETHERSWT_TCType;    /**< @brief Traffic class type */
#define ETHERSWT_TC_0       (0UL)   /**< @brief Traffic class 0 */
#define ETHERSWT_TC_1       (1UL)   /**< @brief Traffic class 1 */
#define ETHERSWT_TC_2       (2UL)   /**< @brief Traffic class 2 */
#define ETHERSWT_TC_3       (3UL)   /**< @brief Traffic class 3 */
#define ETHERSWT_TC_4       (4UL)   /**< @brief Traffic class 4 */
#define ETHERSWT_TC_5       (5UL)   /**< @brief Traffic class 5 */
#define ETHERSWT_TC_6       (6UL)   /**< @brief Traffic class 6 */
#define ETHERSWT_TC_7       (7UL)   /**< @brief Traffic class 7 */
#define ETHERSWT_TC_INVALID (ETHERSWT_TC_7 + 1) /**< @brief Invalid traffic
                                                     class */
/** @} */

/**
    @name ETHERSWT_PortPolicerActType
    @{
    @brief Port policer action type

    Action to be taken by the switch when rate violation happens on the port.

    @trace #BRCM_SWARCH_ETHERSWT_PORT_POLICER_ACT_TYPE #BRCM_REQ_ETHERSWT_PORT_POLICER_ACT
*/
typedef uint32_t ETHERSWT_PortPolicerActType;   /**< @brief Port policer action
                                                     type */
#define PORT_RATE_POLICER_BLOCK_SRC     (1UL)   /**< @brief Block the source
                                                     (using MAC flow control:
                                                     Pause frame) on rate
                                                     violation */
#define PORT_RATE_POLICER_DROP_PKT      (2UL)   /**< @brief Drop the packet on
                                                     rate violation */
/** @} */

/**
    @name ETHERSWT_VLANFwrdType
    @{
    @brief VLAN forwarding type

    @trace #BRCM_SWARCH_ETHERSWT_VLAN_FWRD_TYPE #BRCM_REQ_ETHERSWT_VLAN_FWRD
*/
typedef uint8_t ETHERSWT_VLANFwrdType;          /**< @brief VLAN forwarding
                                                     type */
#define ETHERSWT_VLAN_FRWRD_DONT_SEND   (0U)    /**< @brief Don't send on this
                                                     port */
#define ETHERSWT_VLAN_FRWRD_TAGGED      (1U)    /**< @brief Send tagged packets
                                                     at this port */
#define ETHERSWT_VLAN_FRWRD_UNTAGGED    (2U)    /**< @brief Send untagged
                                                     packets at this port */
/** @} */

/**
    @name ETHWERSWT_SchedAlgoType
    @{
    @brief Port scheduler type

    @trace #BRCM_SWARCH_ETHERSWT_SCHED_ALGO_TYPE #BRCM_REQ_ETHERSWT_SCHED_ALGO
*/
typedef uint8_t ETHWERSWT_SchedAlgoType;/**< @brief Port scheduler type */
#define ETHERSWT_SCHED_ALGO_SP  (0U)    /**< @brief Strict priority scheduler */
#define ETHERSWT_SCHED_ALGO_WRR (1U)    /**< @brief Weighted round robin
                                             scheduler */
#define ETHERSWT_SCHED_ALGO_DRR (2U)    /**< @brief Deficit round robin
                                             scheduler */
/** @} */

/**
    @name ETHERSWT_MacLearningMode
    @{
    @brief Port ARL learning mode type

    @trace #BRCM_SWARCH_ETHERSWT_MAC_LEARN_MODE_TYPE #BRCM_REQ_ETHERSWT_MAC_LEARN_MODE
*/
typedef uint8_t ETHERSWT_MacLearningMode;       /**< @brief ARL learning mode
                                                     type */
#define ETHERSWT_MACLEARNING_DISABLED   (0U)    /**< @brief MAC learning
                                                     disabled (MAC address is
                                                     not learned either by
                                                     software or hardware) */
#define ETHERSWT_MACLEARNING_HW_ENABLED (1U)    /**< @brief Hardware learning
                                                     mode enabled */
#define ETHERSWT_MACLEARNING_SW_ENABLED (2U)    /**< @brief Software learning
                                                     mode enabled. In this mode
                                                     hardware does not
                                                     automatically learns the
                                                     ARL and does not create or
                                                     update the ARL table.
                                                     If the packet is coming
                                                     from unknown source
                                                     address, it is forwarded to
                                                     CPU for learning */
/** @} */

/**
    @name ETHERSWT_PolicerPacketFlgMaskType
    @{
    @brief Ingress port policer flags

    @trace #BRCM_SWARCH_ETHERSWT_POLICER_FLAG_TYPE #BRCM_REQ_ETHERSWT_POLICER_FLAG
*/
typedef uint32_t ETHERSWT_PolicerPacketFlgMaskType; /**< @brief  Policer flags
                                                         type */
#define PACKET_UNICAST_LOOKUP_HIT       (0x1UL)     /**< @brief Packet unicast
                                                         look-up hit */
#define PACKET_MULTICAST_LOOKUP_HIT     (0x2UL)     /**< @brief Packet multicast
                                                         look-up hit */
#define PACKET_RES_MULTICAST_LOOKUP_HIT (0x4UL)     /**< @brief Packet reserved
                                                         multicast look-up hit */
#define PACKET_BROADCAST_LOOUP_HIT      (0x8UL)     /**< @brief Packet broadcast
                                                         look-up hit */
#define PACKET_MULTICAST_LOOKUP_FAILED  (0x10UL)    /**< @brief Packet multicast
                                                         look-up failed */
#define PACKET_UNICAST_LOOKUP_FAILED    (0x20UL)    /**< @brief Packet unicast
                                                         look-up failed */
/** @} */

/**
    @name ETHERSWT_PortMirrorStateType
    @{
    @brief Port mirror state type

    @trace #BRCM_SWARCH_ETHERSWT_PORT_MIRROR_STATE_TYPE #BRCM_REQ_ETHERSWT_PORT_MIRROR_STATE
*/
typedef uint32_t ETHERSWT_PortMirrorStateType;          /**< @brief Port mirror
                                                             state type */
#define ETHERSWT_PORT_MIRROR_STATE_DISABLED     (0x0UL) /**< @brief Port mirror
                                                             state disabled */
#define ETHERSWT_PORT_MIRROR_STATE_ENABLED      (0x1UL) /**< @brief Port mirror
                                                             state enabled */
/** @} */

/**
    @name ETHERSWT_LedType
    @{
    @brief Switch LED type

    @trace #BRCM_SWARCH_ETHERSWT_LED_TYPE #BRCM_REQ_ETHERSWT_LED
*/
typedef uint32_t ETHERSWT_LedType;          /**< @brief Switch LED type */
#define ETHERSWT_LED_AVB            (1UL)   /**< @brief AVB LED */
/** @} */

/**
    @name ETHERSWT_SwitchType
    @{
    @brief Type of the swtich

    @trace #BRCM_SWARCH_ETHERSWT_SWITCH_TYPE #BRCM_REQ_ETHERSWT_SWITCH
*/
typedef uint32_t ETHERSWT_SwitchType;               /**< @brief Type of the
                                                         switch */
#define ETHERSWT_SWITCH_STANDARD            (0UL)   /**< @brief Standard
                                                         switch is an Ethernet
                                                         switch which operates
                                                         as standalone switch
                                                     */
#define ETHERSWT_SWITCH_LIGHTSTACK_MASTER   (1UL)   /**< @brief Light stack
                                                         master switch is an
                                                         Ethernet switch which
                                                         operates as master
                                                         switch in Light stack
                                                         configuration. Refer to
                                                         @ref sec_ref_trm for
                                                         Light stack switch
                                                         operation
                                                     */
#define ETHERSWT_SWITCH_LIGHTSTACK_SLAVE    (2UL)   /**< @brief Light stack
                                                         slave switch is an
                                                         Ethernet switch which
                                                         operates as slave
                                                         switch in Light stack
                                                         configuration. Refer to
                                                         @ref sec_ref_trm for
                                                         Light stack switch
                                                         operation
                                                    */
/** @} */

/**
    @name ETHERSWT_DumbFwdModeType
    @{
    @brief Dumb forward mode

    @trace #BRCM_SWARCH_ETHERSWT_SWITCH_TYPE #BRCM_REQ_ETHERSWT_SWITCH
*/
typedef uint32_t ETHERSWT_DumbFwdModeType;  /**< @brief Dumb forward mode */
#define ETHERSWT_DUMBFWD_ENABLE     (0UL)   /**< @brief Dumb forward enable */
#define ETHERSWT_DUMBFWD_DISABLE    (1UL)   /**< @brief Dumb forward disable */
/** @} */

/**
    @brief Switch management information

    @trace #BRCM_SWARCH_ETHERSWT_MGMT_INFO_TYPE #BRCM_REQ_ETHERSWT_MGMT_INFO
*/
typedef struct {
    ETHERSWT_HwIDType   switchIdx;  /**< @brief Switch index */
    ETHERSWT_PortIDType PortIdx;    /**< @brief Port index */
} ETHERSWT_MgmtInfoType;

/**
    @name ETHERSWT_PortStatsType
    @{
    @brief Switch statistics type

    @trace #BRCM_SWARCH_ETHERSWT_STATS_TYPE #BRCM_REQ_ETHERSWT_STATS
*/
typedef struct _ETHERSWT_PortStatsType {
    uint32_t linkStateChangeCount;  /**< @brief Link state change count */
} ETHERSWT_PortStatsType;
/** @} */

/**
    @brief ARL table entry

    @trace #BRCM_SWARCH_ETHERSWT_ARL_ENTRY_TYPE #BRCM_REQ_ETHERSWT_ARL_ENTRY
*/
typedef struct {
    uint8_t             macAddr[6]; /**< @brief MAC address */
    ETHERSWT_VLANIDType vlanID;     /**< @brief VLAN ID */
    ETHERSWT_PortIDType portMask;   /**< @brief Switch port mask */
    uint16_t            reserved;   /**< @brief Reserved */
} ETHERSWT_ARLEntryType;

/**
    @brief VLAN membership configuration

    @trace #BRCM_SWARCH_ETHERSWT_VLAN_MEM_CFG_TYPE #BRCM_REQ_ETHERSWT_VLAN_MEM_CFG
*/
typedef struct COMP_PACKED {
    ETHERSWT_VLANIDType vlanID;     /**< @brief VLAN ID. Membership of this port
                                         on this virtual network, frames with
                                         this VLAN ID shall be transmitted and
                                         received over this port. */
    uint16_t  macAddrList;          /**< @brief Bit mask for participating MAC
                                         addresses in the VLAN. If the nth bit
                                         is set, then nth MAC address in
                                         ETHERSWT_PortCfgType::fixedMacAddrList
                                         participates in this VLAN. */
    ETHERSWT_PCPType defaultPri;    /**< @brief Default priority for outgoing
                                         packets. Range 0 – 7. */
    ETHERSWT_VLANFwrdType forward;  /**< @brief Packet forwarding type. */
} ETHERSWT_VLANMemberCfgType;

/**
    @name Traffic direction
    @{
    @brief Dirrection of traffic

    @trace #BRCM_SWARCH_ETHERSWT_TRAFFIC_DIR_MACRO #BRCM_REQ_ETHERSWT_TRAFFIC_DIR
*/
typedef uint8_t ETHERSWT_TrafficDirType;
#define ETHERSWT_TRAFFICDIR_INGRESS   (0x0UL) /**< @brief Ingress */
#define ETHERSWT_TRAFFICDIR_EGRESS    (0x1UL) /**< @brief Egress  */
/** @} */

/**
    @brief Port mirror configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_MIRROR_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_MIRROR_CFG
*/
typedef struct {
    uint16_t portMask;              /**< @brief Mirrored port mask. This is bit
                                         mask value of the ports whose traffic
                                         shall be mirrored to the given port */
    uint16_t packetDivider;         /**< @brief Packet divider (only a subset of
                                         the received frame shall be mirrored).
                                         For example if packetDivider is set to
                                         10, then out of 10 packets passing the
                                         filtering, only 1 shall be mirrored */
    uint8_t srcMacAddrFilter[6UL];  /**< @brief Source MAC address that shall be
                                         mirrored */
    uint8_t destMacAddrFilter[6UL]; /**< @brief Destination MAC address that
                                         shall be mirrored */
} ETHERSWT_PortMirrorCfgType;


/**
    @brief Port policer configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_POLICER_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_POLICER_CFG
*/
typedef struct COMP_PACKED {
    ETHERSWT_PolicerPacketFlgMaskType pktMask;  /**< @brief Port policer packet
                                                     flag type (bit mask value
                                                     of the packet types on
                                                     which this policer is to be
                                                     applied). */
    uint32_t rateByteCount;                     /**< @brief Amount of the data
                                                     (excluding header) which
                                                     can be received on this
                                                     port in given time
                                                     (rateTimeInterval) */
    uint32_t rateTimeInterval;                  /**< @brief Time interval in
                                                     seconds where configured
                                                     bytes can be received
                                                     without a rate limitation. */
    ETHERSWT_PortPolicerActType action;         /**< @brief Action to be taken
                                                     by the switch in case of
                                                     the rate violation. */
} ETHERSWT_PortPolicerCfgType;

/**
    @brief Port ingress configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_INGRESS_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_INGRESS_CFG
*/
typedef struct COMP_PACKED {
    ETHERSWT_VLANIDType     defaultVLAN;        /**< @brief Default VLAN for
                                                     this port. For more detail
                                                     refer @ref
                                                     eth_switch_port_forward_config
                                                     "Ingress config" */
    ETHERSWT_PCPType        defaultPrio;        /**< @brief Default priority of
                                                     the VLAN. For more detail
                                                     refer @ref
                                                     eth_switch_port_forward_config
                                                     "Ingress config" */
    uint32_t                dropUntagged;       /**< @brief If default VLAN is
                                                     not configured and this is
                                                     set to TRUE, all the
                                                     untagged frames shall be
                                                     dropped in this port.
                                                     For more detail refer @ref
                                                     eth_switch_port_forward_config
                                                     "Ingress config" */
    ETHERSWT_TCType         tc;                 /**< @brief Port traffic class.
                                                     If configured, all
                                                     the ingress packet on this
                                                     port shall be assinged this
                                                     traffic class. If set to
                                                     #ETHERSWT_TC_INVALID, no
                                                     traffic class based
                                                     assignment shall be done on
                                                     this port. */
    uint32_t                 policerEn;         /**< @brief Enable/disable
                                                     policer.
                                                     @note This parameter is
                                                     not supported by driver.
                                                 */
    ETHERSWT_PortPolicerCfgType policer;        /**< @brief Port policer
                                                     configurations.
                                                     @note This parameter is
                                                     not supported by driver.
                                                 */
    ETHERSWT_TCType             pcp2tcMap[8UL]; /**< @brief PCP to traffic class
                                                     map. */
} ETHERSWT_PortIngressCfgType;

/**
    @brief Port scheduler type

    @trace #BRCM_SWARCH_ETHERSWT_PORT_SCH_TYPE #BRCM_REQ_ETHERSWT_PORT_SCH
*/
typedef struct COMP_PACKED {
    ETHWERSWT_SchedAlgoType algo[ETHERSWT_PORT_FIFO_MAX];   /**< @brief Array of
                                                    scheduler algorithm type for
                                                    all port FIFOs.
                                                    @note Not all possible
                                                    combinations of scheduler
                                                    configurations are
                                                    supported. Refer to @ref
                                                    sec_ref_trm for the list of
                                                    supported scheduler
                                                    configurations. */
} ETHERSWT_PortSchType;

/**
    @brief Port shaper configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_SHAPER_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_SHAPER_CFG
*/
typedef struct {
    uint32_t rateBps;           /**< @brief Increase in credit in bits per
                                     second (shall be multiple of 64000) */
    uint32_t burstBytes;        /**< @brief Bucket size in bytes (shall be a
                                     multiple of 64). */
    uint32_t avbShapingModeEn;  /**< @brief Enable AVB shaping mode. */
} ETHERSWT_ShaperCfgType;

/**
    @brief FIFO configuration in egress port

    @trace #BRCM_SWARCH_ETHERSWT_FIFO_CFG_TYPE #BRCM_REQ_ETHERSWT_FIFO_CFG
*/
typedef struct COMP_PACKED {
    uint32_t id;                    /**< @brief Index of FIFO */
    uint32_t minLen;                /**< @brief FIFO minimum length in bytes
                                         (guaranteed size of FIFO) */
    ETHERSWT_TCType tc;             /**< @brief Traffic class assigned to this
                                         FIFO. */
    ETHERSWT_ShaperCfgType shaper;  /**< @brief Shaper configuration. Port shaper
                                      is configured by ETHERSWT_FifoCfgType::rateBps
                                      and ETHERSWT_FifoCfgType::burstBytes. A
                                      non-zero value of rateBps and burstBytes
                                      enables port shaper.
                                      Additionally, AVB shaper is enabled/disabled
                                      by ETHERSWT_ShaperCfgType::avbShapingModeEn
                                     */
} ETHERSWT_FifoCfgType;

/**
    @brief Port egress configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_EGRESS_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_EGRESS_CFG
*/
typedef struct COMP_PACKED {

    ETHERSWT_FifoCfgType fifoList[ETHERSWT_PORT_FIFO_MAX];  /**< @brief
                                            Configuration of port FIFOs. */
    uint32_t             fifoListSz;        /**< @brief Size of fifoList. */
    ETHERSWT_PortSchType scheduler;         /**< @brief Port scheduler */
    uint32_t             pcpRemarkEn;       /**< @brief Enable/disable PCP
                                                 remarking */
    ETHERSWT_PCPType     tc2pcpMap[8UL];    /**< @brief Traffic class to PCP map

                                                 @note If pcpRemarkEn is set as false,
                                                 all instances of this
                                                 sub-element are ignored.
                                                 However if pcpRemarkEn is set
                                                 as TRUE, exactly 8 instances of
                                                 tc2pcpMap field shall be
                                                 provided, in the order of
                                                 TC0, TC1, … TC7. */
} ETHERSWT_PortEgressCfgType;

/**
    @brief Port configuration

    @trace #BRCM_SWARCH_ETHERSWT_PORT_CFG_TYPE #BRCM_REQ_ETHERSWT_PORT_CFG
*/
typedef struct COMP_PACKED {
    uint32_t portID;            /**< @brief Port Index */
    ETHERSWT_PortType role;     /**< @brief Port role */
    uint32_t xcvrID;            /**< @brief Index of transceiver connected to
                                     this port */
    uint32_t enableTimeStamp;   /**< @brief Enable or disable timestamp support*/
    uint32_t enableJumboFrm;    /**< @brief Enable jumbo frames support on this
                                     port. */
    uint8_t fixedMacAddrList[ETHERSWT_PORT_MAC_ENTY_MAX][6UL];  /**< @brief
                                     Fixed MAC address which can be reached from
                                     this port */
    uint32_t macAddrListSz;     /**< @brief Size of the MAC address list */
    ETHERSWT_VLANMemberCfgType vlanMemList[ETHERSWT_PORT_VLAN_ENTY_MAX];/**<
                                     @brief VLAN membership list of this port.*/
    uint32_t vlanMemListSz;     /**< @brief Size of the VLAN membership list */
    ETHERSWT_PortIngressCfgType ingressCfg; /**< @brief Port ingress configuration */
    ETHERSWT_PortEgressCfgType egressCfg;   /**< @brief Port egress configuration */
} ETHERSWT_PortCfgType;

/**
    @brief Ethernet switch configuration

    @trace #BRCM_SWARCH_ETHERSWT_CFG_TYPE #BRCM_REQ_ETHERSWT_CFG
*/
typedef struct COMP_PACKED {
    ETHERSWT_PortCfgType portCfgList[ETHERSWT_PORT_ID_MAX]; /**< @brief
                                         Configuration list for Switch ports */
    uint32_t portCfgListSz;         /**< @brief Size of the port configuration
                                         list (portCfgList). */

    ETHERSWT_SwitchType switchType; /**< @brief Type of switch w.r.t to
                                         operation */
} ETHERSWT_CfgType;

/** @brief Initliaze Ethernet switch driver

    This API initializes Ethernet switch driver with given configuration
    parameters.

    @behavior Sync, Re-entrant (for different switch index)

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aConfig     Pointer to switch configuration

    @return     void

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_INIT_PROC  #BRCM_REQ_ETHERSWT_INIT

    @limitations This API doesn’t perform reset of switch. Switch port are
    enabled by system before initialization of switch driver.
*/
void ETHERSWT_Init(ETHERSWT_HwIDType aSwtID,
        const ETHERSWT_CfgType *const aConfig);

/** @brief Retrieve port type

    This API retrieves type of a particular port. This API retrieves type of the
    indexed switch port from system xcvr configuration.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aBusMode    Pointer to retrieve Bus Mode
    @param[out]     aPhyMedia   Pointer to retrieve Phy Media

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved Port type
    @retval     #BCM_ERR_INVAL_PARAMS   aType is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #SysCfg_GetXcvrConfig
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_TYPE_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_TYPE

    @limitations None
*/
int32_t ETHERSWT_GetPortType(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BusModeType *const aBusMode,
        ETHXCVR_PhyMediaType *const aPhyMedia);

/** @brief Retrieve port mode

    This API retrieves mode of a particular port. This API retrieves mode of the
    indexed switch port by calling #ETHXCVR_GetMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aMode       Pointer to retrieve Port mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved Port mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_MODE_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_ModeType *const aMode);

/** @brief Set port mode

    This API sets mode of a particular port. This API set mode of the indexed
    switch port by calling #ETHXCVR_SetMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aMode       Port mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_SetMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_MODE_PROC  #BRCM_REQ_ETHERSWT_SET_PORT_MODE

    @limitations None
*/
int32_t ETHERSWT_SetPortMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_ModeType aMode);

/** @brief Retrieve link state of a port

    This API retrieves link state of a particular port. This API retrieves link
    state of the indexed switch port by calling #ETHXCVR_GetLinkState.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aLinkState  Pointer to retrieve port link state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved Port link state
    @retval     #BCM_ERR_INVAL_PARAMS   aLinkState is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #Others                 As returned by #ETHXCVR_GetLinkState
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_LINK_STATE_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_LINK_STATE

    @limitations None
*/
int32_t ETHERSWT_GetPortLinkState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_LinkStateType *const aLinkState);

/** @brief Retrieve port speed

    This API retrieves speed of a particular port. This API retrieves speed of
    the indexed switch port by calling #ETHXCVR_GetSpeed.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aSpeed      Pointer to retrieve port speed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port speed
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetSpeed
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_SPEED_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_SPEED

    @limitations None
*/
int32_t ETHERSWT_GetPortSpeed(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_SpeedType *const aSpeed);

/** @brief Set port speed

    This API set speed of a particular port.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aSpeed      Port speed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port speed
    @retval     #BCM_ERR_INVAL_PARAMS   aSpeed is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_SetSpeed
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_SPEED_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_SPEED

    @limitations None
*/
int32_t ETHERSWT_SetPortSpeed(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_SpeedType aSpeed);

/** @brief Retrieve port master/slave mode

    This API retrieves master/slave mode of a particular port.
    This API retrieves master/slave mode of the indexed switch port by
    calling #ETHXCVR_GetMasterMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aMasterMode Pointer to retrieve port master mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port master mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMasterMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetMasterMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_MASTER_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_GET_PORT_MASTER_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortMasterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMasterMode);

/** @brief Set port master/slave mode

    This API set master/slave mode of a particular port. This API sets
    master/slave mode of the indexed switch port by calling #ETHXCVR_SetMasterMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aMasterMode Port master/slave mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully sets port master mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMasterMode is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_SetMasterMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_MASTER_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_MASTER_MODE

    @limitations None
*/
int32_t ETHERSWT_SetPortMasterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMasterMode);

/** @brief Retrieve port loopback mode

    This API retrieves loopback mode of the indexed switch
    port by calling #ETHXCVR_GetLoopbackModeType.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aMode       Pointer to retrieve port loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port loopback mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetLoopbackMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_LOOPBACK_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_GET_PORT_LOOPBACK_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortLoopbackMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode);

/** @brief Set port loopback mode

    This API set loopback mode of the indexed switch port by calling
    #ETHXCVR_SetLoopbackMode. This API sets PHY loopback.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aMode       Port loopback mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port loopback mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_SetLoopbackMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_LOOPBACK_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_LOOPBACK_MODE

    @limitations None
*/
int32_t ETHERSWT_SetPortLoopbackMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode);

/** @brief Retrieve port jumbo mode

    This API retrieves jumbo mode of a particular port.
    This API retrieves jumbo mode of the indexed switch
    port by calling #ETHXCVR_GetJumboModeType.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aMode       Pointer to retrieve port jumbo mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port jumbo mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetJumboMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_JUMBO_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_GET_PORT_JUMBO_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType *const aMode);

/** @brief Set port jumbo mode

    This API set jumbo mode of a particular port. This API
    set jumbo mode of the indexed switch port by calling
    #ETHXCVR_SetJumboMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aMode       Port jumbo mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port jumbo mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_SetJumboMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_JUMBO_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_JUMBO_MODE

    @limitations None
*/
int32_t ETHERSWT_SetPortJumboMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_BooleanType aMode);

/** @brief Retrieve dumb forward mode for switch

    This API retrieves dumb forward mode of switch.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aMode       Pointer to retrieve switch dumb forward mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port dumb forward mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_DUMBFWD_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_GET_DUMBFWD_MODE

    @limitations None
*/
int32_t ETHERSWT_GetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType *const aMode);

/** @brief Set switch dumb forward mode

    This API set dumb forward mode for switch.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aMode       Port dumb forward mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port dumb forward mode
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_DUMBFWD_MODE_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_DUMBFWD_MODE

    @limitations None
*/
int32_t ETHERSWT_SetDumbFwdMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_DumbFwdModeType aMode);

/** @brief Enable port start/re-start auto-negotiation

    This API is to start/re-start auto-negotiation of a particular port.
    This API start/restarts auto-negotiation of the indexed switch port
    by calling #ETHXCVR_EnableAutoNeg.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully start/restart port auto-neg
    @retval     #BCM_ERR_INVAL_PARAMS   aPort is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_EnableAutoNeg
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized
    @retval     #BCM_ERR_NOSUPPORT      Auto-neg not supported

    @post TBD

    @note Currently, this API is not supported.

    @trace  #BRCM_SWARCH_ETHERSWT_ENABLE_PORT_AUTONEG_PROC
    @trace  #BRCM_REQ_ETHERSWT_ENABLE_PORT_AUTONEG

    @limitations None
*/
int32_t ETHERSWT_EnablePortAutoNeg(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID);

/** @brief Get port auto-negotiation status

    This API get auto-negotiation of a particular port. This API
    retrieves auto-negotiation status of the indexed switch port
    by calling #ETHXCVR_GetAutoNegStatus.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID          Switch index
    @param[in]      aPortID         Port index
    @param[out]     aAutoNegStatus  Pointer to retrieve Port auto-negotiation status

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port auto-neg status
    @retval     #BCM_ERR_INVAL_PARAMS   aAutoNegStatus is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetAutoNegStatus
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_AUTONEG_STATUS_PROC
    @trace  #BRCM_REQ_ETHERSWT_SET_PORT_AUTONEG_STATUS_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortAutoNegStatus(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_AutoNegStatusType *const aAutoNegStatus);

/** @brief Retrieve duplex mode of a port

    This API retrieves duplex mode of a particular port. This API retrieves
    duplex mode of the indexed switch port by calling #ETHXCVR_GetDuplexMode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aDuplexMode Pointer to retrieve duplex mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port duplex mode
    @retval     #BCM_ERR_INVAL_PARAMS   aDuplexMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetDuplexMode
    @retval     #BCM_ERR_NOT_FOUND      Port index not found in the given
                                        switch's port configuration list
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_DUPLEX_MODE_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_DUPLEX_MODE

    @limitations None
*/
int32_t ETHERSWT_GetPortDuplexMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHXCVR_DuplexModeType *const aDuplexMode);

/** @brief Identify a port to reach given MAC address

    This API searches through switch ports to identify a port to reach given MAC
    address.

    @note The switch performs search for indentifying a port to reach a
    given MAC address based on MAC address + VLAN combination.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aPortID     Pointer to retrieve Port index
    @param[in]      aMacAddr    Pointer to retrieve MAC address
    @param[in]      aVlanID     Vlan ID in which MAC Address is participating

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port index from
                                        which MAC address is reachable
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN Id
    @retval     #BCM_ERR_NOT_FOUND      There is no port which can reach this
                                        MAC address (+ VLAN combination )
    @retval     #BCM_ERR_TIME_OUT       Switch driver read-write time-out
    @retval     #BCM_ERR_INVAL_PARAMS   aPortID is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Multicast Address
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_MAC_ADDR_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_MAC_ADDR

    @limitations None
*/
int32_t ETHERSWT_GetPortMacAddr(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID,
        const uint8_t *const aMacAddr,
        ETHERSWT_VLANIDType aVlanID);

/** @brief Retrieve ARL table

    This API read the complete ARL table from the switch and copies it to the
    buffer given by @c aARLTbl.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aARLTbl     Pointer to retrieve ARL table
    @param[inout]   aTblSize    - in: Maximum number of entries that can be
                                  copied in the ARL table buffer
                                - out: Actual number of entries copied in the
                                  ARL table buffer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved ARL table
    @retval     #BCM_ERR_TIME_OUT       Switch driver read-write time-out
    @retval     #BCM_ERR_INVAL_PARAMS   aTblSize is NULL or <BR>
                                        aARLTbl is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized
    @todo       #BCM_ERR_INVAL_PARAMS   aTbleSize is zero. Where are we checking
                                        this in the code?

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_ARL_TBL_PROC  #BRCM_REQ_ETHERSWT_GET_ARL_TBL

    @limitations None
*/
int32_t ETHERSWT_GetARLTable(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLTbl,
        uint16_t *const aTblSize);

/** @brief Read switch register

    This API read the switch register and returns eth value in aVal.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aAddr       Register Address
    @param[out]     aVal        Value read from the switch register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully read register value
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading the
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aPage is out of range
    @retval     #BCM_ERR_INVAL_PARAMS   aOffset is out of range
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_INVAL_PARAMS   aVal is NULL
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_READ_REG_PROC  #BRCM_REQ_ETHERSWT_READ_REG

    @limitations None
*/
int32_t ETHERSWT_ReadReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t *const aVal);

/** @brief Write to Switch register

    This API writes the switch register.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aAddr       Register Address
    @param[in]      aVal        Value to write to the switch register

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully written register value
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while writing the
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aPage is out of range
    @retval     #BCM_ERR_INVAL_PARAMS   aOffset is out of range
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_WRITE_REG_PROC  #BRCM_REQ_ETHERSWT_WRITE_REG

    @limitations None
*/
int32_t ETHERSWT_WriteReg(ETHERSWT_HwIDType aSwtID,
        uint32_t aAddr,
        uint64_t aVal);

/** @brief Enable/disable VLAN

    This API enables/disables VLAN for a given port.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aVlanID     VLAN ID
    @param[in]      aEnable     - #TRUE: Enable given VLAN for this port
                                - #FALSE: Disable given VLAN for this port

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully enabled/disabled VLAN
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_NOT_FOUND      aVlanID is not configured for this port
    @retval     #BCM_ERR_UNKNOWN        Switch Driver configuration not found
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_ENABLE_VLAN_PROC  #BRCM_REQ_ETHERSWT_ENABLE_VLAN

    @limitations None
*/
int32_t ETHERSWT_EnableVLAN(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aEnable);

/** @brief Set MAC learning mode

    This API sets MAC learning mode for index switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aMode       MAC learning mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set MAC learning mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid MAC learning mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_MAC_LEARN_MODE_PROC  #BRCM_REQ_ETHERSWT_SET_MAC_LEARN_MODE

    @limitations None
*/
int32_t ETHERSWT_SetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode aMode);

/** @brief Get MAC learning mode

    This API retrieves MAC learning mode for index switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aMode       Pointer to retrieve MAC learning mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved MAC learning mode
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aMode is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_MAC_LEARN_MODE_PROC  #BRCM_REQ_ETHERSWT_GET_MAC_LEARN_MODE

    @limitations None
*/
int32_t ETHERSWT_GetMACLearningMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_MacLearningMode *const aMode);

/** @brief Set port mirror configuration

    This API sets port mirror configurations for indexed switch port. After
    mirror configuration, user shall call #ETHERSWT_SetPortMirrorState API to
    enable or disable the port mirroring.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID        Switch index
    @param[in]      aPortID       Port index
    @param[in]      aDirection    Traffic direction
    @param[in]      aConfig       Pointer to port mirror configurations

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port mirror
                                        configuration
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid traffic direction
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid configuration
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_CFG_PROC  #BRCM_REQ_ETHERSWT_SET_PORT_MIRROR_CFG

    @limitations None
*/
int32_t ETHERSWT_SetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig);

/** @brief Get port mirror configuration

    This API retrieves port mirror configurations for indexed switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID        Switch index
    @param[in]      aPortID       Port index
    @param[in]      aDirection    Traffic direction
    @param[out]     aConfig       Pointer to retrieve port mirror configurations

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port mirror
                                        configurations
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_STATE    aPortID not previously configured as
                                        mirror capture port
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid traffic direction
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Port ID
    @retval     #BCM_ERR_INVAL_PARAMS   aConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_MIRROR_CFG_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_MIRROR_CFG

    @limitations None
*/
int32_t ETHERSWT_GetPortMirrorConfig(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_TrafficDirType aDirection,
        ETHERSWT_PortMirrorCfgType *const aConfig);

/** @brief Set port mirror state

    This API sets port mirror state for indexed switch port. Port mirroring only
    works if the user has configured the capture port previously by calling
    #ETHERSWT_SetPortMirrorConfig API.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aState      Port mirror state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port mirror state
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port mirror state
    @retval     #BCM_ERR_INVAL_STATE    aPortID not previously configured as
                                        mirror capture port
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_MIRROR_STATE_PROC  #BRCM_REQ_ETHERSWT_SET_PORT_MIRROR_STATE

    @limitations None
*/
int32_t ETHERSWT_SetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType aState);

/** @brief Get port mirror state

    This API retrieves port mirror state for the given switch

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aState      Pointer to retrieve port mirror state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieve port mirror state
    @retval     #BCM_ERR_INVAL_STATE    aPortID not previously configured as
                                        mirror capture port
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_MIRROR_STATE_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_MIRROR_STATE

    @limitations None
*/
int32_t ETHERSWT_GetPortMirrorState(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_PortMirrorStateType *const aState);

/** @brief Get mirror capture port

    This API retrieves mirroring capture port for the given switch

    @behavior Sync, Re-entrant for different aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aPortID     Pointer to retrieve capture port index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieve capture port
    @retval     #BCM_ERR_NOT_FOUND      No capture port is configured.
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   aPortID is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_MIRROR_CAPTURE_PORT_PROC  #BRCM_REQ_ETHERSWT_GET_MIRROR_CAPTURE_PORT

    @limitations None
*/
int32_t ETHERSWT_GetMirrorCapturePort(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType *const aPortID);

/** @brief Get RX statistics

    This API retrieves Rx statistics for indexed switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aRxStat     Pointer to retrieve RX statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port RX
                                        statistics
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   aRxStat is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_RX_STAT_PROC  #BRCM_REQ_ETHERSWT_GET_RX_STAT

    @limitations None
*/
int32_t ETHERSWT_GetRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_RxStatsType *const aRxStat);

/** @brief Clear RX statistics

    This API Clear Rx statistics for indexed switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully cleared port RX
                                        statistics
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_CLEAR_STAT_PROC  #BRCM_REQ_ETHERSWT_CLEAR_RX_STAT

    @limitations None
*/
int32_t ETHERSWT_ClearRxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID);

/** @brief Get TX statistics

    This API retrieves TX statistics for indexed switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aTxStat     Pointer to retrieve TX statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port TX
                                        statistics
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   aTxStat is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_TX_STAT_PROC  #BRCM_REQ_ETHERSWT_GET_TX_STAT

    @limitations None
*/
int32_t ETHERSWT_GetTxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHER_TxStatsType *const aTxStat);

/** @brief Clear TX statistics

    This API clears TX statistics for indexed switch port.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully cleared port TX
                                        statistics
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_CLEAR_TX_STAT_PROC  #BRCM_REQ_ETHERSWT_CLEAR_TX_STAT

    @limitations None
*/
int32_t ETHERSWT_ClearTxStat(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID);

/** @brief Get port transceiver statistics

    This API retrieves statistics for indexed switch port - transceiver.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aStats      Pointer to retrieve port statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port statistics
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   aStats is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_XCVR_STATS_PROC
    @trace  #BRCM_REQ_ETHERSWT_GET_XCVR_STATS

    @limitations None
*/
int32_t ETHERSWT_GetXcvrStats(ETHERSWT_HwIDType aSwtID,
         ETHERSWT_PortIDType aPortID,
         ETHERSWT_PortStatsType *const aStats);

/** @brief Retrieve SQI value

    This API retrieves SQI value for indexed switch port. SQI value is retrieved
    by calling #ETHXCVR_GetSQIValue API.

    @behavior Sync, Re-entrant for different aPortID or aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aSQIValue   Pointer to retrieve SQI value

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port SQI value
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     Others                  As returned by #ETHXCVR_GetSQIValue
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_SQI_VAL_PROC  #BRCM_REQ_ETHERSWT_GET_SQI_VAL

    @limitations None
*/
int32_t ETHERSWT_GetSQIValue(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        uint32_t *const aSQIValue);

/** @brief Initialize switch management functionality

    This API initializes the switch management functionality. Switch management
    functionality control the Ethernet frame regarding port ingress/egress
    functionality and timestamp feature of the port.

    @behavior Sync, Re-entrant for different aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully initialized switch
                                        management functionality
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized
    @retval     #BCM_ERR_INVAL_STATE    Switch management functionality
                                        already initialised.
    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_MGMT_INIT_PROC  #BRCM_REQ_ETHERSWT_MGMT_INIT

    @limitations None
*/
int32_t ETHERSWT_MgmtInit(ETHERSWT_HwIDType aSwtID);

/** @brief De-Initialize switch management functionality

    This API de-initializes the switch management functionality.

    @behavior Sync, Re-entrant for different aSwtID

    @pre TBD

    @param[in]      aSwtID      Switch index

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully de-initialized switch
                                        management functionality
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized
    @retval     #BCM_ERR_INVAL_STATE    Switch management functionality
                                        already un-initialised.
    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_MGMT_DEINIT_PROC  #BRCM_REQ_ETHERSWT_MGMT_DEINIT

    @limitations None
*/
int32_t ETHERSWT_MgmtDeInit(ETHERSWT_HwIDType aSwtID);

/** @brief Enable TX timestamp

    This API adapts a transmit buffer to add the management information and
    prepare for transmission.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Controller index
    @param[in]      aBufIdx     Ethernet buffer index
    @param[inout]   aMgmtInfo   Pointer to management information
                                - In: Pointer to the position of the EthType in
                                  common Ethernet frame
                                - Out: Pointer to the position of the EthType in
                                  management frame

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully enabled timestamp for
                                            the given buffer index
    @retval     #BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state
    @retval     #BCM_ERR_INVAL_PARAMS       Given TX channel is not allocated for gPTP
    @retval     #BCM_ERR_INVAL_PARAMS       aMgmtInfo is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid ethernet buffer index
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid switch index
    @retval     #BCM_ERR_INVAL_STATE        Management functionality not
                                            initialized
    @retval     #BCM_ERR_UNINIT             Switch driver not initialized
    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_ENABLE_TX_TIMESTAMP_PROC  #BRCM_REQ_ETHERSWT_ENABLE_TX_TIMESTAMP

    @limitations None
*/
int32_t ETHERSWT_EnableTxTimestamp(ETHER_HwIDType aCntrlID,
                                   uint32_t aBufIdx,
                                   ETHERSWT_MgmtInfoType *const aMgmtInfo);

/** @brief Set management information

    This API inserts the management information into the indexed Ethernet buffer
    which was previously adapted for the transmission by calling
    #ETHERSWT_TxAdaptBuffer() to achieve the transmission on specific ports of
    the switch.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCntrlID    Index of Ethernet controller
    @param[in]      aBufIdx     Ethernet buffer index
    @param[in]      aMgmtInfo   Pointer to management information to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Successfully set Management
                                            information
    @retval     #BCM_ERR_INVAL_BUF_STATE    Invalid packet buffer state
    @retval     #BCM_ERR_INVAL_PARAMS       aMgmtInfo is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       Invalid ethernet buffer index
    @retval     #BCM_ERR_INVAL_STATE        Management functionality not
                                            initialized
    @retval     #BCM_ERR_UNINIT             Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_MGMT_INFO_PROC  #BRCM_REQ_ETHERSWT_SET_MGMT_INFO

    @limitations None
*/
int32_t ETHERSWT_SetMgmtInfo(ETHER_HwIDType aCntrlID,
                            uint32_t aBufIdx,
                            ETHERSWT_MgmtInfoType * const aMgmtInfo);

/** @brief Set LED state

    This API sets the LED state as per aTurnOn parameter for the given LED type.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aArg1       Brief description of aArg1
    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aLedType    LED Type
    @param[in]      aTurnOn     Flag to indicate the state of LED
                                - #TRUE: LED turn-on
                                - #FALSE: LED turn-off

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successful
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @todo       #BCM_ERR_NOSUPPORT
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   LED type is not supported
    @retval     #BCM_ERR_INVAL_PARAMS   aTurnOn in invalid
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port ID
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_LED_STATE_PROC  #BRCM_REQ_ETHERSWT_SET_LED_STATE

    @limitations None
*/
extern int32_t ETHERSWT_SetLedState(ETHERSWT_HwIDType aSwtID,
                                ETHERSWT_PortIDType aPortID,
                                ETHERSWT_LedType aLedType,
                                uint32_t aTurnOn);

/** @brief Get LED state

    This API retrieves LED state for the given port index

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID          Switch index
    @param[in]      aPortID         Port index
    @param[in]      aLedType        LED Type
    @param[in]      aIsLedStateOn   Pointer to retrieve LED state
                                    - #TRUE: LED turn-on
                                    - #FALSE: LED turn-off

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successful
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @todo       #BCM_ERR_NOSUPPORT
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch registers
    @retval     #BCM_ERR_INVAL_PARAMS   LED type is not supported
    @retval     #BCM_ERR_INVAL_PARAMS   aIsLedStateOn in NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port ID
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_LED_STATE_PROC  #BRCM_REQ_ETHERSWT_SET_LED_STATE

    @limitations None
*/
extern int32_t ETHERSWT_GetLedState(ETHERSWT_HwIDType aSwtID,
                                ETHERSWT_PortIDType aPortID,
                                ETHERSWT_LedType aLedType,
                                uint32_t *const aIsStateOn);

/** @brief Get ports participating in a VLAN

    This API retrieves port mask of the ports participating in a given
    VLAN.

    @param[in]      aSwtID      Switch index
    @param[in]      aVlanID     VLAN ID
    @param[out]     aPortMask   Bitmask representing the ports participating
                                in the given VLAN. If n-th bit is set to 1
                                represents n-th port is participating in the
                                VLAN
    @param[out]     aTagMask    Tag mask representing the ports supporting
                                tagged packets. If n-th bit is set respresents
                                n-th port is in tagged packet mode.
                                Based on this bitmask, Egress packets would be
                                sent as untagged or tagged packets
    @param[out]     aStaticPortMask   Bitmask representing the ports
                                participating in the given VLAN, which is added
                                to VLAN table during switch initialisation
                                (static entry). If n-th bit is set to 1
                                represents n-th port is participating in the
                                VLAN.
    @note The static ports can not be removed during runtime by calling
    ETHERSWT_RemoveVLANPorts.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieving ports
                                        participating in VLAN.
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aTagMask is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aPortMask is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN identifier
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_ENABLE_VLAN_PROC  #BRCM_REQ_ETHERSWT_ENABLE_VLAN

    @limitations None
*/
int32_t ETHERSWT_GetVLANPorts(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t *const aPortMask,
        uint32_t *const aTagMask,
        uint32_t *const aStaticPortMask);


/** @brief Add ports to VLAN

    This API adds given ports to a VLAN in tagged or untagged mode.

    @param[in]      aSwtID      Switch index
    @param[in]      aPortMask   Port mask
    @param[in]      aVlanID     VLAN ID
    @param[in]      aTaggedMask Tagged mask

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully added port to VLAN
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid Tagged mode mask
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN identifier
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port mask
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_ENABLE_VLAN_PROC  #BRCM_REQ_ETHERSWT_ENABLE_VLAN

    @limitations None
*/
int32_t ETHERSWT_AddVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID,
        uint32_t aTaggedMask);

/** @brief Remove dynamically added ports from VLAN

    This API remove dynamically added ports from a VLAN, according to the port
    mask parameter.

    @behavior Sync, Re-entrant

    @note This API would not remove static entries(Ports/VLAN) from VLAN table.

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortMask   Port mask
    @param[in]      aVlanID     VLAN ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully removed port from VLAN
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Deletion of port/s which is/are not part of VLAN
    @retval     #BCM_ERR_NOPERM         Deletion of port/s which is/are part of static
                                        VLAN during initialization
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port mask (zero)
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN identifier
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_REMOVE_VLAN_PORTS_PROC  #BRCM_REQ_ETHERSWT_REMOVE_VLAN_PORTS

    @limitations None
*/
int32_t ETHERSWT_RemoveVLANPorts(ETHERSWT_HwIDType aSwtID,
        uint32_t aPortMask,
        ETHERSWT_VLANIDType aVlanID);

/** @brief Set port default VLAN

    This API set default VLAN ID and priority for a given port. These
    attributes would be utilised to untagged packets ingressing at
    that port.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aPortVlanID Port default VLAN ID
    @param[in]      aPriority   Port default priority

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set port VLAN ID and
                                        port priority
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid priority value
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN identifier
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_PORT_DEFAULT_VLAN_PROC  #BRCM_REQ_ETHERSWT_SET_PORT_DEFAULT_VLAN

    @limitations None
*/
int32_t ETHERSWT_SetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType aVlanID,
        ETHER_PrioType aPrio);

/** @brief Get port default VLAN

    This API retrieves default VLAN ID and priority for a given port. These
    attributes would be utilised to untagged packets ingressing at that port.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[out]     aVlanID     Pointer to retrieve port's default VLAN ID
    @param[out]     aPrio       Pointer to retrieve Port's default priority

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved port VLAN ID and
                                        port priority
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aPrio is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   aVlanID is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_PORT_DEFAULT_VLAN_PROC  #BRCM_REQ_ETHERSWT_GET_PORT_DEFAULT_VLAN

    @limitations None
*/
int32_t ETHERSWT_GetPortDefaultVlan(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_PortIDType aPortID,
        ETHERSWT_VLANIDType *const aVlanID,
        ETHER_PrioType *const aPrio);

/** @brief Set Switch VLAN ingress filter mode

    This API set switch VLAN ingress filter mode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aMode       VLAN Ingress filter mode

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set switch
                                        VLAN ingress filter mode
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN ingress filter mode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_VLAN_INGRESS_FILTER_MODE_PROC  #BRCM_REQ_ETHERSWT_SET_VLAN_INGRESS_FILTER_MODE

    @limitations None
*/
int32_t ETHERSWT_SetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType aMode);

/** @brief Retreive Switch VLAN ingress filter mode

    This API retrieves switch VLAN ingress filter mode.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aMode       Pointer to retrieve VLAN ingress filter state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved switch
                                        VLAN ingress filter
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid aMode
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_VLAN_INGRESS_FILTER_MODE_PROC  #BRCM_REQ_ETHERSWT_GET_VLAN_INGRESS_FILTER_MODE

    @limitations None
*/
int32_t ETHERSWT_GetVLANIngressFilterMode(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_VLANIngressFilterModeType *const aMode);

/** @brief Set Switch Age Time

    This API sets switch FDB age time. Dynamically learnt entries will
    age out within aAge to 2*aAge seconds. If 0 value is passed as aAge,
    it disables FDB ageing.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aAge        Switch FDB age time

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully set FDB age
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_SET_AGE_PROC  #BRCM_REQ_ETHERSWT_SET_AGE

    @limitations None
*/
int32_t ETHERSWT_SetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t aAge);

/** @brief Retrieve Switch FDB age time

    This API retreives switch FDB age time.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[out]     aAge        Pointer to retrieve FDB age

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved FDB age
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   aAge is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_GET_AGE_PROC  #BRCM_REQ_ETHERSWT_GET_AGE

    @limitations None
*/
int32_t ETHERSWT_GetAge(ETHERSWT_HwIDType aSwtID,
        uint32_t *const aAge);

/** @brief Add entry to ARL table

    This API adds a new entry to ARL table.

    @note aARLEntry->reserved are not respected in this API

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aARLEntry   Pointer to ARL entry

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully added entry to ARL table
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_INVAL_PARAMS   Trying to add an ARL entry with destination map
                                        which is not the subset of registered VLAN map
    @retval     #BCM_ERR_INVAL_PARAMS   Trying to add an ARL entry with VLAN which doesn’t exist
    @retval     #BCM_ERR_INVAL_PARAMS   aARLEntry is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_ADD_ARL_ENTRY_PROC  #BRCM_REQ_ETHERSWT_ADD_ARL_ENTRY

    @limitations None
*/
int32_t ETHERSWT_AddARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry);

/** @brief Delete entry from ARL table

    This API delete an existing entry from ARL table.

    @note aARLEntry->portMask and aARLEntry->reserved are not respected in
    this API

    @note This API would not delete static ARL entries.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aSwtID      Switch index
    @param[in]      aPortID     Port index
    @param[in]      aVlanID     VLAN ID
    @param[in]      aMacAddr     Pointer to MAC address

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully removed entry from ARL table
    @retval     #BCM_ERR_TIME_OUT       Timeout occurred while reading/writing
                                        switch register
    @retval     #BCM_ERR_NOT_FOUND      Entry not found in ARL table
    @retval     #BCM_ERR_INVAL_PARAMS   aMacAddr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid VLAN identifier
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid port index
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid switch index
    @retval     #BCM_ERR_UNINIT         Switch driver not initialized

    @post TBD

    @trace  #BRCM_SWARCH_ETHERSWT_DELETE_ARL_ENTRY_PROC  #BRCM_REQ_ETHERSWT_DELETE_ARL_ENTRY

    @limitations None
*/
int32_t ETHERSWT_DeleteARLEntry(ETHERSWT_HwIDType aSwtID,
        ETHERSWT_ARLEntryType *const aARLEntry);

#endif /* ETH_SWITCH_H */

/** @} */
