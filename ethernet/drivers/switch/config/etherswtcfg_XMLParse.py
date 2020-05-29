#!/usr/bin/python2

#
# $Id$
# Copyright: Copyright 2017-2019 Broadcom Limited.
# This program is the proprietary software of Broadcom Limited
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

##  @defgroup grp_eth_switch_config Ethernet Switch Configurator
#   @ingroup grp_eth_switchdrv
#
#   @addtogroup grp_eth_switch_config
#   @{
#   @section sec_eth_switch_config_overview Overview
#   Ethernet Switch configurator is a python based script that parses the
#   Ethernet Switch XML configuration to generate the configuration binary.
#   This generated configuratioin binary information is passed to Ethernet
#   Switch driver as aConfig parameter in #ETHERSWT_Init
#
#   Ethernet Switch XML configuration follows schema matching to different
#   data/structure types defined in eth_switch.h to describe #ETHERSWT_CfgType
#
#   Ethernet Switch Configurator section is divided into sub-section mentioned
#   below:
#   - Overview : This section
#   - Configuration : The section to describes different schema and
#     configuration items. Follow this section to update the Ethernet
#     Switch XML configuration according to the use-case.
#   - Implementation : The section to describe the implementation of Ethernet
#     Switch configurator
#
#   @limitations None
#
#   @file etherswtcfg_XMLParse.py
#   @brief Ethernet Switch  Configurator
#   This script file contains the configurator implementation for Ethernet
#   Switch Driver
#   @version 0.51 Initial version
#

import sys
import os.path
import getopt
import lxml
import struct
import math

from lxml import etree
from lxml import objectify
from array import *

##  @brief Log Class
#
#   Log class to log the configuration generated
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
class Log:
    ##  @brief Instantiation operation
    #
    #   Creates a new instanace of the class
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #Log object
    #   @param[in]      logFile     Log file name
    #
    #   @return     An instance of this class
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def __init__(self, logFile):
        ## @brief Log file name
        self.logFile = logFile
        ## @brief Enable/Disable logging
        self.enableLog = False
        ## @brief Log file pointer
        self.log = None

    ##  @brief Start logging
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #Log object
    #
    #   @return     An instance of this class
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def startLogging(self):
        if self.enableLog == False and self.logFile != None:
            self.log = open(self.logFile, 'w')
            self.enableLog = True

    ##  @brief Stop logging
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #Log object
    #
    #   @return     void
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def stopLogging(self):
        if self.enableLog == True:
            self.log.close()
            self.enableLog = False

    ##  @brief Write to log file
    #
    #   Write the string to the log file
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #Log object
    #   @param[in]      string      String to be written to the log file
    #
    #   @return     An instance of this class
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def dump(self, string):
        if self.enableLog == True:
            string += "\n"
            self.log.write(string)

## @brief Ethernet switch port index maximum value, #ETHERSWT_PORT_ID_MAX
ETHERSWT_PORT_ID_MAX            = 9
## @brief Ethernet switch port MAC entry maximum value, #ETHERSWT_PORT_MAC_ENTY_MAX
ETHERSWT_PORT_MAC_ENTY_MAX      = 16
## @brief Ethernet switch port VLAN entry maximum value, #ETHERSWT_PORT_VLAN_ENTY_MAX
ETHERSWT_PORT_VLAN_ENTY_MAX     = 16
## @brief Ethernet switch port FIFO maximum value, #ETHERSWT_PORT_FIFO_MAX
ETHERSWT_PORT_FIFO_MAX          = 8

## @name ETHERSWT_PortType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_PortType
ETHERSWT_STANDARD_PORT          = 0
ETHERSWT_HOST_PORT              = 1
ETHERSWT_UP_LINK_PORT           = 2
#  @}

## @name ETHERSWT_PCPType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_PCPType
ETHERSWT_PCP_0                  = 0
ETHERSWT_PCP_1                  = 1
ETHERSWT_PCP_2                  = 2
ETHERSWT_PCP_3                  = 3
ETHERSWT_PCP_4                  = 4
ETHERSWT_PCP_5                  = 5
ETHERSWT_PCP_6                  = 6
ETHERSWT_PCP_7                  = 7
#  @}

## @name ETHERSWT_VLANFwrdType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_VLANFwrdType
ETHERSWT_VLAN_FRWRD_DONT_SEND   = 0
ETHERSWT_VLAN_FRWRD_TAGGED      = 1
ETHERSWT_VLAN_FRWRD_UNTAGGED    = 2
#  @}

## @name ETHERSWT_TCType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_TCType
ETHERSWT_TC_0                   = 0
ETHERSWT_TC_1                   = 1
ETHERSWT_TC_2                   = 2
ETHERSWT_TC_3                   = 3
ETHERSWT_TC_4                   = 4
ETHERSWT_TC_5                   = 5
ETHERSWT_TC_6                   = 6
ETHERSWT_TC_7                   = 7
ETHERSWT_TC_INVALID             = ETHERSWT_TC_7 + 1
#  @}

## @name ETHERSWT_PolicerPacketFlgMaskType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_PolicerPacketFlgMaskType
PACKET_UNICAST_LOOKUP_HIT       = 1
PACKET_MULTICAST_LOOKUP_HIT     = 2
PACKET_RES_MULTICAST_LOOKUP_HIT = 4
PACKET_BROADCAST_LOOKUP_HIT     = 8
PACKET_MULTICAST_LOOKUP_FAILED  = 16
PACKET_UNICAST_LOOKUP_FAILED    = 32
#  @}

## @name ETHERSWT_PortPolicerActType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_PortPolicerActType
PORT_RATE_POLICER_BLOCK_SRC     = 1
PORT_RATE_POLICER_DROP_PKT      = 2
#  @}

## @name ETHERSWT_SchedAlgoType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_SchedAlgoType
ETHERSWT_SCHED_ALGO_SP          = 0
ETHERSWT_SCHED_ALGO_WRR         = 1
ETHERSWT_SCHED_ALGO_DRR         = 2
#  @}

## @name ETHERSWT_SwitchType macros
#  @{
#  @brief Ethernet switch port type, #ETHERSWT_SwitchType
ETHERSWT_SWITCH_STANDARD            = 0
ETHERSWT_SWITCH_LIGHTSTACK_MASTER   = 1
ETHERSWT_SWITCH_LIGHTSTACK_SLAVE    = 2
#  @}


## @name Ethernet switch type size macros
#  @{
#  @brief Ethernet switch type size macros
SIZE_UINT8                      = 1
SIZE_UINT32                     = 4
SIZE_PORTTYPE                   = 1
SIZE_TCTTYPE                    = 1
SIZE_VLANIDTYPE                 = 2
SIZE_PCPTYPE                    = 1
SIZE_VLANFRWDTYPE               = 1
SIZE_VLANMEMBERCFGTYPE          = 6
SIZE_PORTPOLICERCFG             = 16
SIZE_PORTINGRESSCFGTYPE         = 36
SIZE_FIFOCFGTYPE                = 21
SIZE_PORTEGRESSCFGTYPE          = 192
SIZE_PORTCFGTYPE                = 445
SIZE_VLANMACADDRTYPE            = 2
SIZE_SWITCHTYPE                 = 4
# @}

## @name Ethernet switch port queue shaper macros
#  @{
#  @brief Ethernet switch port queue shaper macros
QUEUE_SHAPER_BIT_RATE_PER_TOCKEN    = 64000
QUEUE_SHAPER_BUCKET_UNIT            = 64
#  @}
#  @}

##  @addtogroup grp_eth_switch_config
#   @{
#   @section sec_eth_switch_config_schema Configuration
#   This section decribes different schema and configuration items of Ethernet
#   Switch XML configuration which is used to generate Ethernet Switch
#   configuration binary.
#
#   Ethernet Switch XML configurations maps one-to-one with #ETHERSWT_CfgType.
#   Below sub-sections describes different XML element of Ethernet switch configuration.
#   Each sub-section contains following:
#   - XML configuration to C data type mapping table of the elements. Fields in table
#   are:
#       -# Schema Type : This column cotains the top-level element type. The top-level
#       element is listed in the first row of the table.
#       -# Element Schema Type : This column contains schema type of sub-elements which
#       are member of the top-level element.
#       -# Element Name : This column contains name of the element discussed.
#       -# Structure::Member Name : This column contains the C data type of structure &
#       members which maps to the XML configuration element.
#   - An example XML configuration snippet for the XML element is described.
#   Configuration snippet has following notation.
#       -# Each XML element configuration starts with <element_name> and ends with
#       </element_name>.
#       -# Sub-elements of the element are indented by a tab to improve readability
#       and to show hierarchy of the element & sub-elements.
#       -# Sub-elements which further have multiple sub-elements are not completely
#       expanded in the snippet. But "..." is added to represent unexpanded multiple
#       sub-elements. For expanded form of these sub-elements refer
#       "<Element name> XML Element" sub-section.
#
#
#  @subsection subsec_eth_switch_config_ethswitch_cfg ETHERSWT_Cfg XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element Name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_CfgTypeSchemaGlobalAnchor "ETHERSWT_CfgType"<td> <td>ETHERSWT_Cfg<td>ETHERSWT_CfgType
#  <tr><td> <td>@ref ETHERSWT_PortCfgListTypeSchemaGlobalAnchor "ETHERSWT_PortCfgListType"<td>portCfgList<td>ETHERSWT_CfgType::portCfgList
#  <tr><td> <td>@ref ETHERSWT_SwitchTypeSchemaGlobalAnchor "ETHERSWT_SwitchType"<td>switchType<td>ETHERSWT_CfgType::switchType
#  </table>
#
#  Example XML configuration snippet for ETHERSWT_Cfg with following configurations:
#  - Port configuration list containing port configurations of all the ports.
#  - Switch type configured as #ETHERSWT_SWITCH_STANDARD.
#   @code{.py}
#   <ETHERSWT_Cfg>
#       <portCfgList>
#           ...
#       </portCfgList>
#       <switchType>ETHERSWT_SWITCH_STANDARD</switchType>
#   </ETHERSWT_Cfg>
#   @endcode
#
#  @subsection subsec_eth_switch_config_portcfglist portCfgList XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_PortCfgListTypeSchemaGlobalAnchor "ETHERSWT_PortCfgListType"<td><td>portCfgList<td>ETHERSWT_PortCfgType[]
#  <tr><td><td>@ref ETHERSWT_PortCfgTypeSchemaGlobalAnchor "ETHERSWT_PortCfgType"<td>portCfg<td>ETHERSWT_CfgType::portCfgList
#  <tr><td><td>...<td>...<td>...
#  <tr><td><td>@ref ETHERSWT_PortCfgTypeSchemaGlobalAnchor "ETHERSWT_PortCfgType"<td>portCfg<td>ETHERSWT_CfgType::portCfgList
#  </table>
#
#  Example XML configuration snippet for portCfgList:
#   @code{.py}
#   <portCfgList>
#       <portCfg>
#           ...
#       </portCfg>
#       ...
#       <portCfg>
#           ...
#       </portCfg>
#   </portCfgList>
#   @endcode
#
#  @subsection subsec_eth_switch_config_portcfg portCfg XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_PortCfgTypeSchemaGlobalAnchor "ETHERSWT_PortCfgType"<td> <td>portCfg<td>ETHERSWT_PortCfgType
#  <tr><td> <td>unsignedInt<td>portID<td>ETHERSWT_PortCfgType::portID
#  <tr><td> <td>unsignedInt<td>role<td>ETHERSWT_PortCfgType::role
#  <tr><td> <td>unsignedInt<td>xcvrID<td>ETHERSWT_PortCfgType::xcvrID
#  <tr><td> <td>unsignedInt<td>enableTimeStamp<td>ETHERSWT_PortCfgType::enableTimeStamp
#  <tr><td> <td>unsignedInt<td>enableJumboFrm<td>ETHERSWT_PortCfgType::enableJumboFrm
#  <tr><td> <td>@ref ETHERSWT_BasicTypesSchemaGlobalAnchor "MACAddrListType"<td>fixedMacAddrList<td>ETHERSWT_PortCfgType::fixedMacAddrList
#  <tr><td> <td>@ref ETHERSWT_VLANMemberCfgListTypeSchemaGlobalAnchor "ETHERSWT_VLANMemberCfgListType"<td>vlanMemList<td>ETHERSWT_PortCfgType::vlanMemList
#  <tr><td> <td>@ref ETHERSWT_PortIngressCfgTypeSchemaGlobalAnchor "ETHERSWT_PortIngressCfgType"<td>ingressCfg<td>ETHERSWT_PortCfgType::ingressCfg
#  <tr><td> <td>@ref ETHERSWT_PortEgressCfgTypeSchemaGlobalAnchor "ETHERSWT_PortEgressCfgType"<td>egressCfg<td>ETHERSWT_PortCfgType::egressCfg
#  </table>
#
#  Example XML configuration snippet for portCfg for portID 0, with following configurations:
#  - Port role set as #ETHERSWT_STANDARD_PORT.
#  - xcvrID 0 mapped to portID 0. This configuration should be populated according to chip & board
#  configuration.
#  - Timestamping of PTP packets is enabled on this port.
#  - Jumbo frames are disabled on this port.
#  - Fixed MAC address list populated with MAC addresses which are visible from this port.
#  - VLAN membership list should list VLAN membership configuration of the port.
#  - Ingress configuration of the port.
#  - Egress configuration of the port.
#   @code{.py}
#       <portCfg>
#           <portID>0</portID>
#           <role>ETHERSWT_STANDARD_PORT</role>
#           <xcvrID>0</xcvrID>
#           <enableTimeStamp>true</enableTimeStamp>
#           <enableJumboFrm>false</enableJumboFrm>
#           <fixedMacAddrList>
#               <macAddr>00:01:02:03:04:05</macAddr>
#               <macAddr>00:01:06:07:08:09</macAddr>
#               ...
#           </fixedMacAddrList>
#           <vlanMemList>
#               <vlanMem>
#                   ...
#               </vlanMem>
#           </vlanMemList>
#           <ingressCfg>
#               ...
#           </ingressCfg>
#           <egressCfg>
#               ...
#           </egressCfg>
#       </portCfg>
#   @endcode
#
#
#  @subsection subsec_eth_switch_config_fixedmacaddrlist fixedMacAddrList XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_BasicTypesSchemaGlobalAnchor "MACAddrListType"<td> <td>fixedMacAddrList<td>ETHERSWT_PortCfgType::fixedMacAddrList
#  <tr><td> <td>@ref ETHERSWT_BasicTypesSchemaGlobalAnchor "MACAddrType"<td>macAddr<td>ETHERSWT_PortCfgType::fixedMacAddrList
#  </table>
#
#  Example XML configuration snippet fixedMacAddrList, with following configurations:
#  - MAC address 00:01:02:03:04:05 and 00:01:06:07:08:09 are added as part of the
#  fixedMacAddrList, which indicates that device with these MAC address
#  are reachable through the port having this fixedMacAddrList.
#   @code{.py}
#   <fixedMacAddrList>
#       <macAddr>00:01:02:03:04:05</macAddr>
#       <macAddr>00:01:06:07:08:09</macAddr>
#       ...
#   </fixedMacAddrList>
#   @endcode
#
#
#  @subsection subsec_eth_switch_config_vlanmemlist vlanMemList XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_VLANMemberCfgListTypeSchemaGlobalAnchor "ETHERSWT_VLANMemberCfgListType"<td> <td>vlanMemList<td>ETHERSWT_PortCfgType::vlanMemList
#  <tr><td> <td>@ref ETHERSWT_VLANMemberCfgTypeSchemaGlobalAnchor "ETHERSWT_VLANMemberCfgType"<td>vlanMem<td>ETHERSWT_PortCfgType::vlanMemList
#  </table>
#
#  Example XML configuration snippet for vlanMemList:
#   @code{.py}
#   <vlanMemList>
#       <vlanMem>
#           ...
#       </vlanMem>
#       <vlanMem>
#           ...
#       </vlanMem>
#       ...
#       ...
#   </vlanMemList>
#   @endcode
#
#  @subsection subsec_eth_switch_config_vlanMem vlanMem XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_VLANMemberCfgTypeSchemaGlobalAnchor "ETHERSWT_VLANMemberCfgType"<td><td>vlanMem<td>ETHERSWT_VLANMemberCfgType[ ]
#  <tr><td> <td>@ref ETHERSWT_VLANIDTypeSchemaGlobalAnchor "ETHERSWT_VLANIDType"<td>vlanID<td>ETHERSWT_VLANMemberCfgType::vlanID
#  <tr><td> <td>@ref ETHERSWT_VLANMacAddrListTypeSchemaGlobalAnchor "ETHERSWT_VLANMacAddrListType"<td>macAddrList<td>ETHERSWT_VLANMemberCfgType::macAddrList
#  <tr><td> <td>@ref ETHERSWT_PCPTypeSchemaGlobalAnchor "ETHERSWT_PCPType"<td>defaultPri<td>ETHERSWT_VLANMemberCfgType::defaultPri
#  <tr><td> <td>@ref ETHERSWT_VLANFwrdTypeSchemaGlobalAnchor "ETHERSWT_VLANFwrdType"<td>forward<td>ETHERSWT_VLANMemberCfgType::forward
#  </table>
#
#  Example XML configuration snippet for VLAN membership for vlanID 1,
#  with following configurations:
#  - VLAN membership for vlanID 1.
#  - macAddrList is equal to 0x3, which indicates 0th and 1st MAC address listed
#  in fixedMacAddrList of the port as participating in VLAN 1.
#  - Default priority for outgoing packets on this VLAN from this port would be
#  set to #ETHERSWT_PCP_0.
#  - Fowarding mode of this port on the VLAN is set to #ETHERSWT_VLAN_FRWRD_UNTAGGED
#   @code{.py}
#   <vlanMem>
#       <vlanID>1</vlanID>
#       <macAddrList>0x0003</macAddrList>
#       <defaultPri>ETHERSWT_PCP_0</defaultPri>
#       <forward>ETHERSWT_VLAN_FRWRD_UNTAGGED</forward>
#   </vlanMem>
#   @endcode
#
#  @note macAddrList is a bit mask for static MAC addresses participating in
#  this VLAN. If the n-th bit is set, then n-th MAC address in
#  ETHERSWT_PortCfgType::fixedMacAddrList participates in this VLAN.
#
#
#  @subsection subsec_eth_switch_config_ingresscfg ingressCfg XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_PortIngressCfgTypeSchemaGlobalAnchor "ETHERSWT_PortIngressCfgType"<td> <td>ingressCfg<td>ETHERSWT_PortIngressCfgType
#  <tr><td> <td>@ref ETHERSWT_VLANIDTypeSchemaGlobalAnchor "ETHERSWT_VLANIDType"<td>defaultVLAN<td>ETHERSWT_PortIngressCfgType::defaultVLAN
#  <tr><td> <td>@ref ETHERSWT_PCPTypeSchemaGlobalAnchor "ETHERSWT_PCPType"<td>defaultPrio<td>ETHERSWT_PortIngressCfgType::defaultPrio
#  <tr><td> <td>boolean<td>dropUntagged<td>ETHERSWT_PortIngressCfgType::dropUntagged
#  <tr><td> <td>@ref ETHERSWT_TCTypeSchemaGlobalAnchor "ETHERSWT_TCType"<td>tc<td>ETHERSWT_PortIngressCfgType::tc
#  <tr><td> <td>boolean<td>policerEn<td>ETHERSWT_PortIngressCfgType::policerEn
#  <tr><td> <td>@ref ETHERSWT_PortPolicerCfgTypeSchemaGlobalAnchor "ETHERSWT_PortPolicerCfgType"<td>policer<td>ETHERSWT_PortIngressCfgType::policer
#  <tr><td> <td>@ref ETHERSWT_TCTypeSchemaGlobalAnchor "ETHERSWT_TCType"<td>pcp2tcMap<td>ETHERSWT_PortIngressCfgType::pcp2tcMap
#  </table>
#
#  Example XML configuration snippet for ingressCfg, with following configurations:
#  - Default VLAN for ingress packets of this port is set to 1. For details of
#  this parameter refer ETHERSWT_PortIngressCfgType::defaultVLAN
#  - Default priority for ingress packets of this port is set to #ETHERSWT_PCP_0.
#  For details of this parameter refer ETHERSWT_PortIngressCfgType::defaultPrio
#  - Untagged ingress packets on this port would be dropped
#  - Traffic class for ingress packets on this port are configured as #ETHERSWT_TC_INVALID
#  - Policer is disabled on this port.
#  - Ingress packets with PCP 0 are mapped to Traffic class 0
#  - Ingress packets with PCP 1 are mapped to Traffic class 1
#  - Ingress packets with PCP 2 are mapped to Traffic class 2
#  - Ingress packets with PCP 3 are mapped to Traffic class 3
#  - Ingress packets with PCP 4 are mapped to Traffic class 4
#  - Ingress packets with PCP 5 are mapped to Traffic class 5
#  - Ingress packets with PCP 6 are mapped to Traffic class 6
#  - Ingress packets with PCP 7 are mapped to Traffic class 7
#
#   @code{.py}
#   <ingressCfg>
#       <defaultVLAN>1</defaultVLAN>
#       <defaultPrio>ETHERSWT_PCP_0</defaultPrio>
#       <dropUntagged>false</dropUntagged>
#       <tc>ETHERSWT_TC_INVALID</tc>
#       <policerEn>false</policerEn>
#       <pcp2tcMap>ETHERSWT_TC_0</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_1</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_2</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_3</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_4</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_5</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_6</pcp2tcMap>
#       <pcp2tcMap>ETHERSWT_TC_7</pcp2tcMap>
#   </ingressCfg>
#   @endcode
#
#
#  @subsection subsec_eth_switch_config_egresscfg egressCfg XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_PortEgressCfgTypeSchemaGlobalAnchor "ETHERSWT_PortEgressCfgType"<td> <td>egressCfg<td>ETHERSWT_PortEgressCfgType
#  <tr><td> <td>@ref ETHERSWT_FifoCfgListTypeSchemaGlobalAnchor "ETHERSWT_FifoCfgListType"<td>fifoList<td>ETHERSWT_PortEgressCfgType::fifoList
#  <tr><td> <td>@ref ETHERSWT_PortSchTypeSchemaGlobalAnchor "ETHERSWT_PortSchType"<td>scheduler<td>ETHERSWT_PortEgressCfgType::scheduler
#  <tr><td> <td>boolean<td>pcpRemarkEn<td>ETHERSWT_PortEgressCfgType::pcpRemarkEn
#  <tr><td> <td>@ref ETHERSWT_PCPTypeSchemaGlobalAnchor "ETHERSWT_PCPType"<td>tc2pcpMap<td>ETHERSWT_PortEgressCfgType::tc2pcpMap
#  </table>
#
#  Example XML configuration snippet for egressCfg, with following configurations:
#  - FIFO list listing FIFO configuration for all the ports.
#  - Port scheduler configuration.
#  - PCP remarking is enabled, which means egress packets with a particular
#  traffic class would be remarked to PCP value as per tc2pcpMap configuration
#  for that traffic class.
#  - Egress packets with traffic class 0 remarked to PCP 1
#  - Egress packets with traffic class 1 remarked to PCP 0
#  - Egress packets with traffic class 2 remarked to PCP 4
#  - Egress packets with traffic class 3 remarked to PCP 5
#  - Egress packets with traffic class 4 remarked to PCP 6
#  - Egress packets with traffic class 5 remarked to PCP 7
#  - Egress packets with traffic class 6 remarked to PCP 2
#  - Egress packets with traffic class 7 remarked to PCP 3
#
#   @code{.py}
#   <egressCfg>
#       <fifoList>
#           <fifo>
#               ...
#           </fifo>
#           <fifo>
#               ...
#           </fifo>
#           ...
#           ...
#       </fifoList>
#       <scheduler>
#           ...
#       </scheduler>
#       <pcpRemarkEn>true</pcpRemarkEn>
#       <tc2pcpMap>ETHERSWT_PCP_1</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_0</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_4</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_5</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_6</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_7</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_2</tc2pcpMap>
#       <tc2pcpMap>ETHERSWT_PCP_3</tc2pcpMap>
#   </egressCfg>
#   @endcode
#
#
#  @subsection subsec_eth_switch_config_egresscfg_fifo fifo XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_FifoCfgTypeSchemaGlobalAnchor "ETHERSWT_FifoCfgType"<td> <td>fifo<td>ETHERSWT_FifoCfgType
#  <tr><td> <td>unsignedInt<td>id<td>ETHERSWT_FifoCfgType::id
#  <tr><td> <td>unsignedInt<td>minLen<td>ETHERSWT_FifoCfgType::minLen
#  <tr><td> <td>@ref ETHERSWT_TCTypeSchemaGlobalAnchor "ETHERSWT_TCType"<td>tc<td>ETHERSWT_FifoCfgType::tc
#  <tr><td> <td>@ref ETHERSWT_ShaperCfgTypeSchemaGlobalAnchor "ETHERSWT_ShaperCfgType"<td>shaper<td>ETHERSWT_FifoCfgType::shaper
#  </table>
#
#  Example XML configuration snippet for fifo, with following configurations:
#  - FIFO configuration of ID 0
#  - FIFO minimum length configured to 0 bytes
#  - Traffic class #ETHERSWT_TC_0 assigned to FIFO ID 0
#  - FIFO shaper are disabled. For detailed description refer ETHERSWT_FifoCfgType::shaper
#
#   @code{.py}
#   <fifo>
#       <id>0</id>
#       <minLen>0</minLen>
#       <tc>ETHERSWT_TC_0</tc>
#       <shaper>
#           <rateBps>0</rateBps>
#           <burstBytes>0</burstBytes>
#           <avbShapingModeEn>false</avbShapingModeEn>
#       </shaper>
#   </fifo>
#   @endcode
#
#
#  @subsection subsec_eth_switch_config_egresscfg_scheduler scheduler XML Element
#
#  <table>
#  <caption id="schema_structure_map"> Schema to data structure/member map
#  </caption>
#  <tr><th rowspan="1" colspan="3"> XML configuration <th colspan="1"> C Data type
#  <tr><th rowspan="1"> Schema Type <th colspan="1"> Element Schema Type <th colspan="1">Element name <th colspan="1"> Structure::Member Name
#  <tr><td>@ref ETHERSWT_PortSchTypeSchemaGlobalAnchor "ETHERSWT_PortSchType"<td> <td>scheduler<td>ETHERSWT_PortSchType
#  <tr><td> <td>@ref ETHERSWT_SchedAlgoTypeSchemaGlobalAnchor "ETHWERSWT_SchedAlgoType"<td>algo<td>ETHERSWT_PortSchType::algo
#  </table>
#
#  Example XML configuration snippet for port scheduler, with following configurations:
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 0 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 1 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 2 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 3 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 4 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 5 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 6 of the port
#  - Scheduler algorithm #ETHERSWT_SCHED_ALGO_SP set for egress FIFO 7 of the port
#
#   @code{.py}
#   <scheduler>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#       <algo>ETHERSWT_SCHED_ALGO_SP</algo>
#   </scheduler>
#   @endcode
#
#  @subsection subsec_eth_switch_config_example_cfg Example XML configurations
#  This subsection descibes lists of example Ethernet Switch configurations
#  provided as part of SDK.
#
#  <table>
#  <caption id="example_switch_config"> Ethernet Switch Example XML Configurations
#  </caption>
#  <tr><th rowspan="1" colspan="1"> XML configuration <th colspan="1"> Description
#  <tr><td>"etherswt_default.xml"<td>Basic switch configuration
#  <tr><td>"etherswt_test.xml"<td>Switch configuration with VLAN MAC configuration
#  <tr><td>"etherswt_pcp5_6_7_to_tc2.xml"<td>Switch configuration with traffic with PCP 5, 6, 7 mapped to TC2 queue
#  </table>
#
#  @limitations None
#

##  @brief Schema for Basic types
#
#   Schema for basic types 32-bit hexadecimal string, MAC Address, and MAC
#   list.
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_BasicTypesSchemaGlobalAnchor
ETHERSWT_BasicTypesSchemaGlobal = """
<xs:simpleType name ="Hex32String">
    <xs:restriction base = "xs:string">
    <xs:pattern value = "0x[0-9A-Fa-f]{8}"/>
    </xs:restriction>
</xs:simpleType>
<xs:simpleType name = "MACAddrType">
    <xs:restriction base = "xs:string">
        <xs:maxLength value="17" />
    </xs:restriction>
</xs:simpleType>
<xs:complexType name = "MACAddrListType">
    <xs:sequence>
        <xs:element
            name = "macAddr"
            type = "MACAddrType"
            minOccurs = "1"
            maxOccurs = "16"/>
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_VLANIDType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_VLANIDTypeSchemaGlobalAnchor
ETHERSWT_VLANIDTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_VLANIDType">
    <xs:restriction base = "xs:unsignedInt">
        <xs:minInclusive value = "0"/>
        <xs:maxInclusive value = "4094"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for list of MAC address participating in a VLAN.
#   This is added as part of #ETHERSWT_VLANMemberCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_VLANMacAddrListTypeSchemaGlobalAnchor
ETHERSWT_VLANMacAddrListTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_VLANMacAddrListType">
    <xs:restriction base = "xs:string">
        <xs:maxLength value="6" />
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_PCPType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PCPTypeSchemaGlobalAnchor
ETHERSWT_PCPTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_PCPType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_PCP_0"/>
        <xs:enumeration value = "ETHERSWT_PCP_1"/>
        <xs:enumeration value = "ETHERSWT_PCP_2"/>
        <xs:enumeration value = "ETHERSWT_PCP_3"/>
        <xs:enumeration value = "ETHERSWT_PCP_4"/>
        <xs:enumeration value = "ETHERSWT_PCP_5"/>
        <xs:enumeration value = "ETHERSWT_PCP_6"/>
        <xs:enumeration value = "ETHERSWT_PCP_7"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_PortType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortTypeSchemaGlobalAnchor
ETHERSWT_PortTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_PortType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_STANDARD_PORT"/>
        <xs:enumeration value = "ETHERSWT_HOST_PORT"/>
        <xs:enumeration value = "ETHERSWT_UP_LINK_PORT"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_TCType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_TCTypeSchemaGlobalAnchor
ETHERSWT_TCTypeSchemaGlobal = """
<xs:simpleType name  = "ETHERSWT_TCType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_TC_0"/>
        <xs:enumeration value = "ETHERSWT_TC_1"/>
        <xs:enumeration value = "ETHERSWT_TC_2"/>
        <xs:enumeration value = "ETHERSWT_TC_3"/>
        <xs:enumeration value = "ETHERSWT_TC_4"/>
        <xs:enumeration value = "ETHERSWT_TC_5"/>
        <xs:enumeration value = "ETHERSWT_TC_6"/>
        <xs:enumeration value = "ETHERSWT_TC_7"/>
        <xs:enumeration value = "ETHERSWT_TC_INVALID"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_PortPolicerActType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortPolicerActTypeAnchor
ETHERSWT_PortPolicerActTypeSchemaGlobal = """
<xs:simpleType name  = "ETHERSWT_PortPolicerActType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "PORT_RATE_POLICER_BLOCK_SRC"/>
        <xs:enumeration value = "PORT_RATE_POLICER_DROP_PKT"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_VLANFwrdType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_VLANFwrdTypeSchemaGlobalAnchor
ETHERSWT_VLANFwrdTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_VLANFwrdType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_VLAN_FRWRD_DONT_SEND"/>
        <xs:enumeration value = "ETHERSWT_VLAN_FRWRD_TAGGED"/>
        <xs:enumeration value = "ETHERSWT_VLAN_FRWRD_UNTAGGED"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHWERSWT_SchedAlgoType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_SchedAlgoTypeSchemaGlobalAnchor
ETHERSWT_SchedAlgoTypeSchemaGlobal = """
<xs:simpleType name = "ETHWERSWT_SchedAlgoType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_SCHED_ALGO_SP"/>
        <xs:enumeration value = "ETHERSWT_SCHED_ALGO_WRR"/>
        <xs:enumeration value = "ETHERSWT_SCHED_ALGO_DRR"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_PolicerPacketFlgMaskType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PolicerPacketFlgMaskTypeSchemaGlobalAnchor
ETHERSWT_PolicerPacketFlgMaskTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_PolicerPacketFlgMaskType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "PACKET_UNICAST_LOOKUP_HIT"/>
        <xs:enumeration value = "PACKET_MULTICAST_LOOKUP_HIT"/>
        <xs:enumeration value = "PACKET_RES_MULTICAST_LOOKUP_HIT"/>
        <xs:enumeration value = "PACKET_BROADCAST_LOOKUP_HIT"/>
        <xs:enumeration value = "PACKET_MULTICAST_LOOKUP_FAILED"/>
        <xs:enumeration value = "PACKET_UNICAST_LOOKUP_FAILED"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_SwitchType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_SwitchTypeSchemaGlobalAnchor
ETHERSWT_SwitchTypeSchemaGlobal = """
<xs:simpleType name = "ETHERSWT_SwitchType">
    <xs:restriction base = "xs:string">
        <xs:enumeration value = "ETHERSWT_SWITCH_STANDARD"/>
        <xs:enumeration value = "ETHERSWT_SWITCH_LIGHTSTACK_MASTER"/>
        <xs:enumeration value = "ETHERSWT_SWITCH_LIGHTSTACK_SLAVE"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Schema for #ETHERSWT_VLANMemberCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_VLANMemberCfgTypeSchemaGlobalAnchor
ETHERSWT_VLANMemberCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_VLANMemberCfgType">
    <xs:sequence>
        <xs:element
            name  = "vlanID"
            type = "ETHERSWT_VLANIDType"
        />
        <xs:element
            name = "macAddrList"
            type = "ETHERSWT_VLANMacAddrListType"
        />
        <xs:element
            name = "defaultPri"
            type = "ETHERSWT_PCPType"
        />
        <xs:element
            name = "forward"
            type = "ETHERSWT_VLANFwrdType"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for list of #ETHERSWT_VLANMemberCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_VLANMemberCfgListTypeSchemaGlobalAnchor
ETHERSWT_VLANMemberCfgListTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_VLANMemberCfgListType">
    <xs:sequence>
        <xs:element
            name = "vlanMem"
            type = "ETHERSWT_VLANMemberCfgType"
            minOccurs = "1"
            maxOccurs = "16"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_PortPolicerCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortPolicerCfgTypeSchemaGlobalAnchor
ETHERSWT_PortPolicerCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortPolicerCfgType">
    <xs:sequence>
        <xs:element
            name = "pktMask"
            type = "ETHERSWT_PolicerPacketFlgMaskType"
        />
        <xs:element
            name = "rateByteCount"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "rateTimeInterval"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "action"
            type = "ETHERSWT_PortPolicerActType"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_PortIngressCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortIngressCfgTypeSchemaGlobalAnchor
ETHERSWT_PortIngressCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortIngressCfgType">
    <xs:sequence>
        <xs:element
            name = "defaultVLAN"
            type = "ETHERSWT_VLANIDType"
        />
        <xs:element
            name = "defaultPrio"
            type = "ETHERSWT_PCPType"
        />
        <xs:element
            name = "dropUntagged"
            type = "xs:boolean"
        />
        <xs:element
            name = "tc"
            type = "ETHERSWT_TCType"
        />
        <xs:element
            name = "policerEn"
            type = "xs:boolean"
            minOccurs = "0"
            default = "false"
        />
        <xs:element
            name = "policer"
            type = "ETHERSWT_PortPolicerCfgType"
            minOccurs = "0"
        />
        <xs:element
            name = "pcp2tcMap"
            type = "ETHERSWT_TCType"
            minOccurs = "0"
            maxOccurs = "8"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_PortSchType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortSchTypeSchemaGlobalAnchor
ETHERSWT_PortSchTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortSchType">
    <xs:sequence>
        <xs:element
            name = "algo"
            type = "ETHWERSWT_SchedAlgoType"
            minOccurs = "8"
            maxOccurs = "8"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_ShaperCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_ShaperCfgTypeSchemaGlobalAnchor
ETHERSWT_ShaperCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_ShaperCfgType">
    <xs:sequence>
        <xs:element
            name = "rateBps"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "burstBytes"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "avbShapingModeEn"
            type = "xs:boolean"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_FifoCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_FifoCfgTypeSchemaGlobalAnchor
ETHERSWT_FifoCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_FifoCfgType">
    <xs:sequence>
        <xs:element
            name = "id"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "minLen"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "tc"
            type = "ETHERSWT_TCType"
        />
        <xs:element
            name = "shaper"
            type = "ETHERSWT_ShaperCfgType"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for a list of #ETHERSWT_FifoCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_FifoCfgListTypeSchemaGlobalAnchor
ETHERSWT_FifoCfgListTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_FifoCfgListType">
    <xs:sequence>
        <xs:element
            name = "fifo"
            type = "ETHERSWT_FifoCfgType"
            minOccurs = "1"
            maxOccurs = "8"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_PortEgressCfgType
#
#  @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#  @anchor ETHERSWT_PortEgressCfgTypeSchemaGlobalAnchor
ETHERSWT_PortEgressCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortEgressCfgType">
    <xs:sequence>
        <xs:element
            name = "fifoList"
            type = "ETHERSWT_FifoCfgListType"
            minOccurs = "0"
        />
        <xs:element
            name = "scheduler"
            type = "ETHERSWT_PortSchType"
        />
        <xs:element
            name = "pcpRemarkEn"
            type = "xs:boolean"
        />
        <xs:element
            name = "tc2pcpMap"
            type = "ETHERSWT_PCPType"
            minOccurs = "0"
            maxOccurs = "8"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for #ETHERSWT_PortCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortCfgTypeSchemaGlobalAnchor
ETHERSWT_PortCfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortCfgType">
    <xs:sequence>
        <xs:element
            name = "portID"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "role"
            type = "ETHERSWT_PortType"
        />
        <xs:element
            name = "xcvrID"
            type = "xs:unsignedInt"
        />
        <xs:element
            name = "enableTimeStamp"
            type = "xs:boolean"
            minOccurs = "0"
            default = "false"
        />
        <xs:element
            name = "enableJumboFrm"
            type = "xs:boolean"
            minOccurs = "0"
            default = "false"
        />
        <xs:element
            name = "fixedMacAddrList"
            type = "MACAddrListType"
            minOccurs = "0"
        />
        <xs:element name = "vlanMemList" type = "ETHERSWT_VLANMemberCfgListType" minOccurs = "0">
            <xs:unique name="uniquevlanID">
                <xs:selector xpath = "vlanMem" />
                <xs:field xpath = "vlanID" />
            </xs:unique>
        </xs:element>
        <xs:element
            name = "ingressCfg"
            type = "ETHERSWT_PortIngressCfgType"
            minOccurs = "0"
        />
        <xs:element
            name = "egressCfg"
            type = "ETHERSWT_PortEgressCfgType"
            minOccurs = "0"
        />
    </xs:sequence>
</xs:complexType>"""

##  @brief Schema for list of #ETHERSWT_PortCfgType
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#   @anchor ETHERSWT_PortCfgListTypeSchemaGlobalAnchor
ETHERSWT_PortCfgListTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_PortCfgListType">
    <xs:sequence>
        <xs:element
            name = "portCfg"
            type = "ETHERSWT_PortCfgType"
            minOccurs = "1"
            maxOccurs = "9"
        />
    </xs:sequence>
</xs:complexType>"""

##  @anchor ETHERSWT_CfgTypeSchemaGlobalAnchor
#   @brief Schema for #ETHERSWT_Cfg
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
ETHERSWT_CfgTypeSchemaGlobal = """
<xs:complexType name = "ETHERSWT_CfgType">
    <xs:sequence>
        <xs:element
            name = "portCfgList"
            type = "ETHERSWT_PortCfgListType"
            minOccurs = "0"
        />
        <xs:element
            name = "switchType"
            type = "ETHERSWT_SwitchType"
            minOccurs = "0"
        />
    </xs:sequence>
</xs:complexType>
<xs:element
    name = "ETHERSWT_Cfg"
    type = "ETHERSWT_CfgType"
    />"""
#  @}

##  @addtogroup grp_eth_switch_config
#   @{
#   @section sec_eth_switch_config_impl Implementation
#   TBD
#
#   @limitations None
#
#   @brief Switch Driver Configurator
#
#   Switch driver configurator class for generating Ethernet Switch driver
#   configuration binary.
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
class ETHERSWT_CfgConfigurator:

    ##  @brief Instantiation operation
    #
    #   Creates a new instanace of the class
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      xmlFile     XML file containing Switch driver
    #                               configuration
    #   @param[in]      log         Instance of #Log for logging switch driver
    #                               configuration
    #
    #   @return     An instance of this class
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def __init__(self, xmlFile, log):
        ## @brief Instance of #Log to log switch driver configuration
        self.log = log
        local = """<?xml version="1.0"?>
                <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">"""
        local += ETHERSWT_BasicTypesSchemaGlobal
        local += ETHERSWT_VLANIDTypeSchemaGlobal
        local += ETHERSWT_VLANMacAddrListTypeSchemaGlobal
        local += ETHERSWT_PCPTypeSchemaGlobal
        local += ETHERSWT_PortTypeSchemaGlobal
        local += ETHERSWT_TCTypeSchemaGlobal
        local += ETHERSWT_PortPolicerActTypeSchemaGlobal
        local += ETHERSWT_VLANFwrdTypeSchemaGlobal
        local += ETHERSWT_SchedAlgoTypeSchemaGlobal
        local += ETHERSWT_PolicerPacketFlgMaskTypeSchemaGlobal
        local += ETHERSWT_SwitchTypeSchemaGlobal
        local += ETHERSWT_VLANMemberCfgTypeSchemaGlobal
        local += ETHERSWT_VLANMemberCfgListTypeSchemaGlobal
        local += ETHERSWT_PortPolicerCfgTypeSchemaGlobal
        local += ETHERSWT_PortIngressCfgTypeSchemaGlobal
        local += ETHERSWT_PortSchTypeSchemaGlobal
        local += ETHERSWT_ShaperCfgTypeSchemaGlobal
        local += ETHERSWT_FifoCfgTypeSchemaGlobal
        local += ETHERSWT_FifoCfgListTypeSchemaGlobal
        local += ETHERSWT_PortEgressCfgTypeSchemaGlobal
        local += ETHERSWT_PortCfgTypeSchemaGlobal
        local += ETHERSWT_PortCfgListTypeSchemaGlobal
        local += ETHERSWT_CfgTypeSchemaGlobal
        local += """</xs:schema>"""
        schema = etree.XMLSchema(objectify.fromstring(local))
        self.parser = objectify.makeparser(schema = schema)

        fp = open(xmlFile, 'r')
        xml = fp.read()
        fp.close

        self.cfg = objectify.fromstring(xml, self.parser)

    ##  @brief Generate byte array
    #
    #   This function generates the bytearray bytearray representation
    #   for the given integer.
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      value       Integer to be converted into bytearray
    #   @param[in]      size        Size of the bytearray (in bytes)
    #
    #   @return     Bytearray representation of the given integer
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getByteArray(self, value, size):
        if size == 1:
            data = struct.pack("<B", value)
        elif size == 2:
            data = struct.pack("<H", value)
        else:
            data = struct.pack("<I", value)

        return bytearray(data)

    ##  @brief Generate byte array #ETHERSWT_PortType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      PortType    Objectify tree for #ETHERSWT_PortType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortType(self, PortType):
        if 'ETHERSWT_STANDARD_PORT' == PortType:
            role = ETHERSWT_STANDARD_PORT
        elif 'ETHERSWT_HOST_PORT' == PortType:
            role = ETHERSWT_HOST_PORT
        else:
            role = ETHERSWT_UP_LINK_PORT

        return self.getByteArray(role, SIZE_PORTTYPE)

    ##  @brief Generate byte array for MAC address
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      macAddr     Objectify tree for MAC address
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryMacAddr(self, macAddr):
        data = bytearray()
        for index in range(0, 17, 3):
            hexStr = '0x'
            hexStr += macAddr[index]
            hexStr += macAddr[index + 1]
            val = int(hexStr, 16)
            self.log.dump('{0},'.format(val))
            data += self.getByteArray(val , SIZE_UINT8)

        return data

    ##  @brief Generate byte array for #ETHERSWT_PCPType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      pcp         Objectify tree for #ETHERSWT_PCPType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPCP(self, pcp):
        if pcp == 'ETHERSWT_PCP_0':
            val = ETHERSWT_PCP_0
        elif pcp == 'ETHERSWT_PCP_1':
            val = ETHERSWT_PCP_1
        elif pcp == 'ETHERSWT_PCP_2':
            val = ETHERSWT_PCP_2
        elif pcp == 'ETHERSWT_PCP_3':
            val = ETHERSWT_PCP_3
        elif pcp == 'ETHERSWT_PCP_4':
            val = ETHERSWT_PCP_4
        elif pcp == 'ETHERSWT_PCP_5':
            val = ETHERSWT_PCP_5
        elif pcp == 'ETHERSWT_PCP_6':
            val = ETHERSWT_PCP_6
        else:
            val = ETHERSWT_PCP_7

        return self.getByteArray(val, SIZE_PCPTYPE)

    ##  @brief Generate byte array for #ETHERSWT_SwitchType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      swtType     Objectify tree for #ETHERSWT_SwitchType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinarySwitchType(self, swtType):
        if swtType == 'ETHERSWT_SWITCH_LIGHTSTACK_MASTER':
            val = ETHERSWT_SWITCH_LIGHTSTACK_MASTER
        elif swtType == 'ETHERSWT_SWITCH_LIGHTSTACK_SLAVE':
            val = ETHERSWT_SWITCH_LIGHTSTACK_SLAVE
        else:
            val = ETHERSWT_SWITCH_STANDARD

        return self.getByteArray(val, SIZE_SWITCHTYPE)

    ##  @brief Generate byte array for #ETHERSWT_VLANFwrdType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      vlanFrwd    Objectify tree for #ETHERSWT_VLANFwrdType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryVLANFwrd(self, vlanFrwd):
        if vlanFrwd == 'ETHERSWT_VLAN_FRWRD_DONT_SEND':
            val = ETHERSWT_VLAN_FRWRD_DONT_SEND
        elif vlanFrwd == 'ETHERSWT_VLAN_FRWRD_TAGGED':
            val = ETHERSWT_VLAN_FRWRD_TAGGED
        else:
            val = ETHERSWT_VLAN_FRWRD_UNTAGGED

        return self.getByteArray(val, SIZE_VLANFRWDTYPE)

    ##  @brief Generate byte array for #ETHERSWT_VLANMemberCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      vlanMemCfg  Objectify tree for
    #                               #ETHERSWT_VLANMemberCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryVLANMemberCfg(self, vlanMemCfg):
        data = bytearray()

        self.log.dump('vlanID = {0},'.format(vlanMemCfg.vlanID))
        data = self.getByteArray(vlanMemCfg.vlanID, SIZE_VLANIDTYPE)

        macAddrListVal = int(str(vlanMemCfg.macAddrList), 16)
        self.log.dump('macAddrList = {0},'.format(macAddrListVal))
        data += self.getByteArray(macAddrListVal, SIZE_VLANMACADDRTYPE)

        self.log.dump('defaultPri = {0},'.format(vlanMemCfg.defaultPri))
        data += self.getBinaryPCP(vlanMemCfg.defaultPri)

        self.log.dump('forward = {0},'.format(vlanMemCfg.forward))
        data += self.getBinaryVLANFwrd(vlanMemCfg.forward)

        return data

    ##  @brief Get MSB position of macAddrListVal
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #ETHERSWT_CfgConfigurator object
    ##  @param[in]      vlanMemCfg  Objectify tree for
    ##                              #ETHERSWT_VLANMemberCfgType
    ##
    ##  @return     MSB Position of macAddrListVal
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    ##
    ##  @limitations None
    def getMSBposOfMacAddrListVal(self, vlanMemCfg):
        macAddrListVal = int(str(vlanMemCfg.macAddrList),16)
        if macAddrListVal == 0 :
            return 0
        else :
            return int(math.floor(math.log(macAddrListVal,2))) + 1

    ##  @brief Generate byte array for #ETHERSWT_TCType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      tct         Objectify tree for #ETHERSWT_TCType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryTC(self, tct):
        if tct == 'ETHERSWT_TC_0':
            val = ETHERSWT_TC_0
        elif tct == 'ETHERSWT_TC_1':
            val = ETHERSWT_TC_1
        elif tct == 'ETHERSWT_TC_2':
            val = ETHERSWT_TC_2
        elif tct == 'ETHERSWT_TC_3':
            val = ETHERSWT_TC_3
        elif tct == 'ETHERSWT_TC_4':
            val = ETHERSWT_TC_4
        elif tct == 'ETHERSWT_TC_5':
            val = ETHERSWT_TC_5
        elif tct == 'ETHERSWT_TC_6':
            val = ETHERSWT_TC_6
        elif tct == 'ETHERSWT_TC_7':
            val = ETHERSWT_TC_7
        else:
            val = ETHERSWT_TC_INVALID

        return self.getByteArray(val, SIZE_TCTTYPE)

    ##  @brief Generate byte array for #ETHERSWT_PolicerPacketFlgMaskType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      pktFlgMask  Objectify tree for
    #                               #ETHERSWT_PolicerPacketFlgMaskType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPolicerPacketFlgMask(self, pktFlgMask):

        if pktFlgMask == 'PACKET_UNICAST_LOOKUP_HIT':
            val = PACKET_UNICAST_LOOKUP_HIT
        elif pktFlgMask == 'PACKET_MULTICAST_LOOKUP_HIT':
            val = PACKET_MULTICAST_LOOKUP_HIT
        elif pktFlgMask == 'PACKET_RES_MULTICAST_LOOKUP_HIT':
            val = PACKET_RES_MULTICAST_LOOKUP_HIT
        elif pktFlgMask == 'PACKET_BROADCAST_LOOKUP_HIT':
            val = PACKET_BROADCAST_LOOKUP_HIT
        elif pktFlgMask == 'PACKET_MULTICAST_LOOKUP_FAILED':
            val = PACKET_MULTICAST_LOOKUP_FAILED
        else:
            val = PACKET_UNICAST_LOOKUP_FAILED

        return self.getByteArray(val, SIZE_UINT32)

    ##  @brief Generate byte array for #ETHERSWT_PortPolicerActType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      policerAct  Objectify tree for
    #                               #ETHERSWT_PortPolicerActType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortPolicerAct(self, policerAct):
        if policerAct == 'PORT_RATE_POLICER_BLOCK_SRC':
            val = PORT_RATE_POLICER_BLOCK_SRC
        else:
            val = PORT_RATE_POLICER_DROP_PKT

        return self.getByteArray(val, SIZE_UINT32)

    ##  @brief Generate byte array for #ETHERSWT_PortPolicerCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      policerCfg  Objectify tree for
    #                               #ETHERSWT_PortPolicerCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortPolicerCfg(self, policerCfg):
        data = bytearray()

        self.log.dump('pktMask = {0},'.format(policerCfg.pktMask))
        data += self.getBinaryPolicerPacketFlgMask(policerCfg.pktMask)

        self.log.dump('rateByteCont = {0},'.format(policerCfg.rateByteCont))
        data += self.getByteArray(policerCfg.rateByteCont, SIZE_UINT32)

        self.log.dump('rateTimeInterval = {0},'.format(policerCfg.rateTimeInterval))
        data += self.getByteArray(policerCfg.rateTimeInterval, SIZE_UINT32)

        self.log.dump('action = {0},'.format(policerCfg.action))
        data += self.getBinaryPortPolicerAct(policerCfg.action)

        return data

    ##  @brief Generate byte array for #ETHERSWT_PortIngressCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      ingressCfg  Objectify tree for
    #                               #ETHERSWT_PortIngressCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortIngressCfg(self, ingressCfg):
        data = bytearray()

        self.log.dump('defaultVLAN = {0},'.format(ingressCfg.defaultVLAN))
        data += self.getByteArray(ingressCfg.defaultVLAN, SIZE_VLANIDTYPE)

        self.log.dump('defaultPrio = {0},'.format(ingressCfg.defaultPrio))
        data += self.getBinaryPCP(ingressCfg.defaultPrio)

        self.log.dump('dropUntagged = {0},'.format(ingressCfg.dropUntagged))

        if (ingressCfg.dropUntagged == True) != (ingressCfg.defaultVLAN == 0):
            print 'ERROR: dropUntagged and defaultVLAN should be mutually exclusive'
            sys.exit(1)
        data += self.getByteArray(ingressCfg.dropUntagged, SIZE_UINT32)

        self.log.dump('tc = {0},'.format(ingressCfg.tc))
        data += self.getBinaryTC(ingressCfg.tc)

        if hasattr(ingressCfg, 'policerEn'):
            self.log.dump('policerEn = {0},'.format(ingressCfg.policerEn))
            if ingressCfg.policerEn:
                data += self.getByteArray(1, SIZE_UINT32)
            else:
                data += self.getByteArray(0, SIZE_UINT32)
        else:
            self.log.dump('policerEn = False,')
            data += bytearray(SIZE_UINT32)

        self.log.dump('policer = {')
        if hasattr(ingressCfg, 'policer'):
            data += self.getBinaryPortPolicerCfg(ingressCfg.policer)
        else:
            data += bytearray(SIZE_PORTPOLICERCFG)
        self.log.dump('},')

        self.log.dump('pcp2tcMap = {')
        if hasattr(ingressCfg, 'pcp2tcMap'):
            for each in ingressCfg.pcp2tcMap:
                self.log.dump('{0},'.format(str(each)))
                data += self.getBinaryTC(str(each))
        else:
            data += bytearray(8)
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #ETHERSWT_ShaperCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      shaperCfg   Objectify tree for #ETHERSWT_ShaperCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryShaperCfg(self, shaperCfg):
        data = bytearray()

        self.log.dump('rateBps = {0},'.format(shaperCfg.rateBps))
        if shaperCfg.rateBps % QUEUE_SHAPER_BIT_RATE_PER_TOCKEN != 0:
            print 'ERROR: rateBps should be a mutiple of {0}'.format(QUEUE_SHAPER_BIT_RATE_PER_TOCKEN)
            sys.exit(1)
        data = self.getByteArray(shaperCfg.rateBps, SIZE_UINT32)

        self.log.dump('burstBytes = {0},'.format(shaperCfg.burstBytes))
        if shaperCfg.burstBytes % QUEUE_SHAPER_BUCKET_UNIT != 0:
            print 'ERROR: burstBytes should be a mutiple of {0}'.format(QUEUE_SHAPER_BUCKET_UNIT)
            sys.exit(1)
        data += self.getByteArray(shaperCfg.burstBytes, SIZE_UINT32)

        self.log.dump('avbShapingModeEn = {0},'.format(shaperCfg.avbShapingModeEn))
        if shaperCfg.avbShapingModeEn:
            data += self.getByteArray(1, SIZE_UINT32)
        else:
            data += self.getByteArray(0, SIZE_UINT32)

        return data

    ##  @brief Generate byte array for #ETHERSWT_FifoCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      fifoCfg     Objectify tree for #ETHERSWT_FifoCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryFifoCfg(self, fifoCfg):
        data = bytearray()

        self.log.dump('id = {0},'.format(fifoCfg.id))
        data = self.getByteArray(fifoCfg.id, SIZE_UINT32)

        self.log.dump('minLen = {0},'.format(fifoCfg.minLen))
        data += self.getByteArray(fifoCfg.minLen, SIZE_UINT32)

        self.log.dump('tc = {0},'.format(fifoCfg.tc))
        data += self.getBinaryTC(fifoCfg.tc)

        self.log.dump('shaper = {')
        data += self.getBinaryShaperCfg(fifoCfg.shaper)
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #ETHWERSWT_SchedAlgoType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      schedAlgo   Objectify tree for #ETHWERSWT_SchedAlgoType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinarySchedAlgo(self, schedAlgo):
        if schedAlgo == 'ETHERSWT_SCHED_ALGO_SP':
            val = ETHERSWT_SCHED_ALGO_SP
        elif schedAlgo == 'ETHERSWT_SCHED_ALGO_WRR':
            val = ETHERSWT_SCHED_ALGO_WRR
        else:
            val = ETHERSWT_SCHED_ALGO_DRR

        return self.getByteArray(val, SIZE_UINT8)

    ##  @brief Get integer value for a macro of type #ETHWERSWT_SchedAlgoType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      schedAlgo   String of type #ETHWERSWT_SchedAlgoType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getSchedAlgoVal(self, schedAlgo):
        if schedAlgo == 'ETHERSWT_SCHED_ALGO_SP':
            val = ETHERSWT_SCHED_ALGO_SP
        elif schedAlgo == 'ETHERSWT_SCHED_ALGO_WRR':
            val = ETHERSWT_SCHED_ALGO_WRR
        else:
            val = ETHERSWT_SCHED_ALGO_DRR

        return val

    ##  @brief Generate byte array for #ETHERSWT_PortSchType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      portSch     Objectify tree for #ETHERSWT_PortSchType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortSch(self, portSch):
        data = bytearray()

        algoArray = array('I', [0, 0, 0, 0, 0, 0, 0, 0])
        for i in range(8):
            algoArray[i] = self.getSchedAlgoVal(str(portSch.algo[i]))

        count = algoArray.count(ETHERSWT_SCHED_ALGO_SP)
        if (count != 8) and (count != 0):
            firstSPIdx = algoArray.index(ETHERSWT_SCHED_ALGO_SP)
            if firstSPIdx in range(1, 4):
                print 'ERROR: Invalid Scheduler configuration'
                sys.exit(1)

            for i in range((firstSPIdx + 1), 8):
                if algoArray[i] != ETHERSWT_SCHED_ALGO_SP:
                    print 'ERROR: Invalid Scheduler configuration'
                    sys.exit(1)

        self.log.dump('algo = {')
        for each in portSch.algo:
            self.log.dump('{0},'.format(each))
            data += self.getBinarySchedAlgo(str(each))
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #ETHERSWT_PortEgressCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      egressCfg   Objectify tree for
    #                               #ETHERSWT_PortEgressCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortEgressCfg(self, egressCfg):
        data = bytearray()

        fifoListSz = 0
        self.log.dump('fifoList = {')
        if hasattr(egressCfg, 'fifoList'):
            cnt = 0
            for each in egressCfg.fifoList.fifo:
                self.log.dump('{')
                data += self.getBinaryFifoCfg(each)
                self.log.dump('},')
                cnt += 1
            fifoListSz = cnt
            if cnt < ETHERSWT_PORT_FIFO_MAX:
                data += bytearray(SIZE_FIFOCFGTYPE * (ETHERSWT_PORT_FIFO_MAX - cnt))
                while cnt <ETHERSWT_PORT_FIFO_MAX:
                    self.log.dump('{},')
                    cnt += 1
        else:
            data += bytearray(SIZE_FIFOCFGTYPE * ETHERSWT_PORT_FIFO_MAX)
        self.log.dump('},')

        self.log.dump('fifoListSz = {0},'.format(fifoListSz))
        data += self.getByteArray(fifoListSz, SIZE_UINT32)

        self.log.dump('scheduler = {')
        data += self.getBinaryPortSch(egressCfg.scheduler)
        self.log.dump('},')

        self.log.dump('pcpRemarkEn = {0},'.format(egressCfg.pcpRemarkEn))
        if egressCfg.pcpRemarkEn:
            data += self.getByteArray(1, SIZE_UINT32)
            if hasattr(egressCfg, 'tc2pcpMap'):
                cnt = 0;
                self.log.dump('tc2pcpMap = {')
                for each in egressCfg.tc2pcpMap:
                    self.log.dump('{0},'.format(each))
                    data += self.getBinaryPCP(each)
                    cnt += 1
                if cnt != 8:
                    print 'ERROR: Only {0} entries for tc2pcpMap'.format(cnt)
                    print 'Exaclty 8 entries are required'
                    sys.exit(1)
                self.log.dump('},')
            else:
                print 'ERROR:tc2pcpMap should be passed when pcpRemarkEn is set to true'
                sys.exit(1)
        else:
            data += self.getByteArray(0, SIZE_UINT32)
            if hasattr(egressCfg, 'tc2pcpMap'):
                print 'WARNING: Ignoring tc2pcpMap as pcpRemarkEn is set to false'
            data += bytearray(SIZE_PCPTYPE * 8)

        return data

    ##  @brief Generate byte array for #ETHERSWT_PortCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      portCfg     Objectify tree for #ETHERSWT_PortCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortCfg(self, portCfg):
        data = bytearray()

        self.log.dump('portID = {0},'.format(portCfg.portID))
        data += self.getByteArray(portCfg.portID, SIZE_UINT32)

        self.log.dump('role = {0},'.format(portCfg.role))
        data += self.getBinaryPortType(portCfg.role)

        self.log.dump('xcvrID = {0},'.format(portCfg.xcvrID))
        data += self.getByteArray(portCfg.xcvrID, SIZE_UINT32)

        if hasattr(portCfg, 'enableTimeStamp'):
            self.log.dump('enableTimeStamp = {0},'.format(portCfg.enableTimeStamp))
            if portCfg.enableTimeStamp:
                data += self.getByteArray(1, SIZE_UINT32)
            else:
                data += bytearray(SIZE_UINT32)
        else:
            self.log.dump('enableTimeStamp = FALSE,')
            data += self.getByteArray(0, SIZE_UINT32)

        if hasattr(portCfg, 'enableJumboFrm'):
            self.log.dump('enableJumboFrm = {0},'.format(portCfg.enableJumboFrm))
            if portCfg.enableJumboFrm:
                data += self.getByteArray(1, SIZE_UINT32)
            else:
                data += self.getByteArray(0, SIZE_UINT32)
        else:
            self.log.dump('enableJumboFrm = FALSE,')
            data += self.getByteArray(0, SIZE_UINT32)

        macAddrListSz = 0
        if hasattr(portCfg, 'fixedMacAddrList'):
            self.log.dump('fixedMacAddrList = {')
            cnt = 0
            for each in portCfg.fixedMacAddrList.macAddr:
                self.log.dump('{')
                data += self.getBinaryMacAddr(str(each))
                self.log.dump('},')
                cnt += 1
            macAddrListSz = cnt
            if cnt < ETHERSWT_PORT_MAC_ENTY_MAX:
                data += bytearray(SIZE_UINT8 * (ETHERSWT_PORT_MAC_ENTY_MAX - cnt) * 6)
                while cnt < ETHERSWT_PORT_MAC_ENTY_MAX:
                    self.log.dump('{},')
                    cnt += 1
            self.log.dump('},')
        else:
            self.log.dump('fixedMacAddrList = {},')
            data += bytearray(SIZE_UINT8 * ETHERSWT_PORT_MAC_ENTY_MAX * 6)

        self.log.dump('macAddrListSz = {0},'.format(macAddrListSz))
        data += self.getByteArray(macAddrListSz, SIZE_UINT32)

        vlanMemListSz = 0
        msbPosMacAddrListVal = 0
        if hasattr(portCfg, 'vlanMemList'):
            cnt = 0
            self.log.dump('vlanMemList = {')
            for each in portCfg.vlanMemList.vlanMem:
                self.log.dump('{')
                data += self.getBinaryVLANMemberCfg(each)
                self.log.dump('},')
                cnt += 1
                msbPosMacAddrListVal = self.getMSBposOfMacAddrListVal(each)
                if msbPosMacAddrListVal > macAddrListSz :
                    print "macAddrList for VLAN %d, port %d is invalid" %(each.vlanID, portCfg.portID)
                    self.log.dump('},')
                    sys.exit(1)
            vlanMemListSz = cnt
            if cnt < ETHERSWT_PORT_VLAN_ENTY_MAX:
                data += bytearray(SIZE_VLANMEMBERCFGTYPE * (ETHERSWT_PORT_VLAN_ENTY_MAX - cnt))
                while cnt < ETHERSWT_PORT_VLAN_ENTY_MAX:
                    self.log.dump('{},')
                    cnt += 1
            self.log.dump('},')
        else:
            self.log.dump('vlanMemList = {},')
            data += bytearray(SIZE_VLANMEMBERCFGTYPE * ETHERSWT_PORT_VLAN_ENTY_MAX)

        self.log.dump('vlanMemListSz = {0},'.format(vlanMemListSz))
        data += self.getByteArray(vlanMemListSz, SIZE_UINT32)

        self.log.dump('ingressCfg = {')
        if hasattr(portCfg, 'ingressCfg'):
            data += self.getBinaryPortIngressCfg(portCfg.ingressCfg)
        else:
            data += bytearray(SIZE_PORTINGRESSCFGTYPE)
        self.log.dump('},')

        self.log.dump('egressCfg = {')
        if hasattr(portCfg, 'egressCfg'):
            data += self.getBinaryPortEgressCfg(portCfg.egressCfg)
        else:
            data += bytearray(SIZE_PORTEGRESSCFGTYPE)
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #ETHERSWT_CfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #   @param[in]      cfg         Objectify tree for #ETHERSWT_CfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinaryETHERSWT_Cfg(self, cfg):
        data = bytearray()

        cnt = 0
        portCfgListSz = 0
        self.log.dump('portCfgList = {')
        if hasattr(cfg, 'portCfgList'):
            for each in cfg.portCfgList.portCfg:
                self.log.dump('{')
                data += self.getBinaryPortCfg(each)
                self.log.dump('},')
                cnt += 1
            portCfgListSz = cnt
            if cnt < ETHERSWT_PORT_ID_MAX:
                data += bytearray(SIZE_PORTCFGTYPE * (ETHERSWT_PORT_ID_MAX - cnt))
                while cnt < ETHERSWT_PORT_ID_MAX:
                    self.log.dump('{},')
                    cnt += 1
        else:
            data += bytearray(SIZE_PORTCFGTYPE * ETHERSWT_PORT_ID_MAX)
        self.log.dump('},')

        self.log.dump('portCfgListSz = {0},'.format(portCfgListSz))
        data += self.getByteArray(portCfgListSz, SIZE_UINT32)

        if hasattr(cfg, 'switchType'):
            val = cfg.switchType
            self.log.dump('switchType = {0},'.format(val))
            data += self.getBinarySwitchType(val)
        else:
            self.log.dump('switchType = 0,')
            data = bytearray(SIZE_UINT32)

        return data

    ##  @brief Generate Ethernet switch driver configuration binary
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #ETHERSWT_CfgConfigurator object
    #
    #   @return     Ethernet switch driver configuration in binary format
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    #
    #   @limitations None
    def getBinData(self):
        binData = bytearray()

        self.log.dump('ETHERSWT_Cfg = {')
        binData = self.getBinaryETHERSWT_Cfg(self.cfg)
        self.log.dump('}')

        return binData

##  @brief Print usage for Switch Configurator
#
#   @behavior Sync, Non-reentrant
#
#   @param          void
#
#   @return void
#
#   @post Application exits
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#
#   @limitations None
def usage():
    print """
usage: etherswtcfg_XMLParse.py
    [-h, --help]    Display this help message
    -i, --xml       Input XML file
    -o, --out       Output binary file
    [-l, --log]     Log file [loging disabled if not log file is not mentioned]"""
    sys.exit(1)

##  @brief main function for Ethernet Switch Configurator
#
#   @behavior Sync, Re-entrant
#
#   @pre TBD
#
#   @param[in]      void
#
#   @return void
#
#   @post TBD
#
#   @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
#
#   @limitations None
def main():
    XMLFile = None
    BinOutputFile = None
    LogFile = None
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'hi:o:l:',
                    ['help=', 'xml=', "out=", "log="])
    except:
        print 'Invalid Arguments'
        usage()
    for opt, arg in optlist:
        if opt in ('-h', '--help'):
            usage()
        elif opt in ('-i', '--xml'):
            if os.path.isfile(arg) == True:
                XMLFile = arg
            else:
                print 'File {0} does not exist'.format(arg)
                usage()
        elif opt in ('-o', '--out'):
            BinOutputFile = arg
        elif opt in ('-l', '--log'):
            LogFile = arg
        else:
            print 'Unknown Argument {0}'.format(opt)
            usage()
    if ((XMLFile == None) or (BinOutputFile == None)):
        print 'Missing arguments'
        usage()

    logItem = Log(LogFile)
    logItem.startLogging()
    configurator = ETHERSWT_CfgConfigurator(XMLFile, logItem)
    fp = open(BinOutputFile, 'wb')
    fp.write(configurator.getBinData())
    fp.close
    logItem.stopLogging()
    del configurator
    del logItem

if __name__ == '__main__':
    main()
## @}
