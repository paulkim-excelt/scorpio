#!/usr/bin/python2

#
# $Id$
# Copyright: Copyright 2019 Broadcom Limited.
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

##  @defgroup grp_comms_config Communication Subsystem Configurator
#   @ingroup grp_eth_switchdrv
#
#   @addtogroup grp_comms_config
#   @{
#   @section sec_comms_config_overview Overview
#   Communication Subsystem Configurator is a python based script that parses the
#   Communication Subsystem XML configuration to generate the configuration binary.
#
#   Communication Subsystem XML configuration follows schema matching to different
#   data/structure types defined in comms_cfg.h to describe #COMMS_CfgType
#
#   Communication Subsystem Configurator section is divided into sub-section mentioned
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
#   @file comms_config.py
#   @brief Communication Subsystem Configurator
#   This script file contains the configurator implementation for Communication
#   Subsystem
#   @version 0.1 Initial version
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
#   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def dump(self, string):
        if self.enableLog == True:
            string += "\n"
            self.log.write(string)

## @brief Communication Subsystem port index maximum value, #COMMS_MAX_PORTS
COMMS_MAX_PORTS              = 16
## @brief Communication Subsystem port FIFO maximum value, #COMMS_PORT_FIFO_MAX
COMMS_PORT_FIFO_MAX          = 8
## @brief Communication Subsystem maximum number of Streams, #COMMS_MAX_ARL_ENTRIES
COMMS_MAX_ARL_ENTRIES        = 512
## @brief Communication Subsystem maximum number of Streams, #COMMS_MAX_STREAMS
COMMS_MAX_STREAMS            = 256
## @brief Communication Subsystem maximum number of VLANs, #COMMS_MAX_VLANS
COMMS_MAX_VLANS              = 512

## @name COMMS_PortType macros
#  @{
#  @brief Communication Subsystem port type, #COMMS_PortType
COMMS_STANDARD_PORT          = 0
COMMS_HOST_PORT              = 1
COMMS_UP_LINK_PORT           = 2
#  @}

## @name COMMS_PolicerPacketFlgMaskType macros
#  @{
#  @brief Communication Subsystem port type, #COMMS_PolicerPacketFlgMaskType
PACKET_UNICAST_LOOKUP_HIT       = 1
PACKET_MULTICAST_LOOKUP_HIT     = 2
PACKET_RES_MULTICAST_LOOKUP_HIT = 4
PACKET_BROADCAST_LOOKUP_HIT     = 8
PACKET_MULTICAST_LOOKUP_FAILED  = 16
PACKET_UNICAST_LOOKUP_FAILED    = 32
#  @}

## @name COMMS_PortPolicerActType macros
#  @{
#  @brief Communication Subsystem port type, #COMMS_PortPolicerActType
PORT_RATE_POLICER_BLOCK_SRC     = 1
PORT_RATE_POLICER_DROP_PKT      = 2
#  @}

#  @}
## @name COMMS_SchedAlgoType macros
#  @{
#  @brief Communication Subsystem port type, #COMMS_SchedAlgoType
COMMS_SCHED_ALGO_SP          = 0
COMMS_SCHED_ALGO_WRR         = 1
COMMS_SCHED_ALGO_DRR         = 2
#  @}

## @name COMMS_SwitchType macros
#  @{
#  @brief Communication Subsystem port type, #COMMS_SwitchType
COMMS_SWITCH_STANDARD            = 0
COMMS_SWITCH_LIGHTSTACK_MASTER   = 1
COMMS_SWITCH_LIGHTSTACK_SLAVE    = 2
#  @}


## @name Communication Subsystem type size macros
#  @{
#  @brief Communication Subsystem type size macros
SIZE_UINT8                      = 1
SIZE_UINT16                     = 2
SIZE_UINT32                     = 4
SIZE_PORTTYPE                   = 1
SIZE_VLANIDTYPE                 = 2
SIZE_ARLENTRYTYPE               = 10
SIZE_STREAMPOLICERCONFIGTYPE    = 24
SIZE_VLANENTRYTYPE              = 6
SIZE_PORTMASKTYPE               = 2
SIZE_PCPTYPE                    = 1
SIZE_PORTPOLICERCFG             = 16
SIZE_PORTINGRESSCFGTYPE         = 36
SIZE_FIFOCFGTYPE                = 21
SIZE_PORTEGRESSCFGTYPE          = 192
SIZE_PORTCFGTYPE                = 241
SIZE_SWITCHTYPE                 = 4
# @}

## @name Communication Subsystem port queue shaper macros
#  @{
#  @brief Communication Subsystem port queue shaper macros
QUEUE_SHAPER_BIT_RATE_PER_TOCKEN    = 64000
QUEUE_SHAPER_BUCKET_UNIT            = 64
#  @}
#  @}

##  @addtogroup grp_comms_config
#   @{
#   @section sec_comms_config_impl Implementation
#   TBD
#
#   @limitations None
#
#   @brief Communication Subsystem Configurator
#
#   Communication Subsystem configurator class for generating Communication Subsystem
#   configuration binary.
#
#   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
class COMMS_CfgConfigurator:

    ##  @brief Instantiation operation
    #
    #   Creates a new instanace of the class
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre TBD
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      xmlFile     XML file containing Communication Subsystem
    #                               configuration
    #   @param[in]      schemaFile  XSD file containing schema for Communication Subsystem
    #   @param[in]      log         Instance of #Log for logging Communication Subsystem
    #                               configuration
    #
    #   @return     An instance of this class
    #
    #   @post TBD
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def __init__(self, xmlFile, schemaFile, log):
        ## @brief Instance of #Log to log switch driver configuration
        self.log = log
        schema = etree.XMLSchema(etree.parse(schemaFile))
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
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      value       Integer to be converted into bytearray
    #   @param[in]      size        Size of the bytearray (in bytes)
    #
    #   @return     Bytearray representation of the given integer
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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

    ##  @brief Generate byte array #COMMS_PortType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      PortType    Objectify tree for #COMMS_PortType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortType(self, PortType):
        if 'standard' == PortType:
            role = COMMS_STANDARD_PORT
        elif 'host' == PortType:
            role = COMMS_HOST_PORT
        else:
            role = COMMS_UP_LINK_PORT

        return self.getByteArray(role, SIZE_PORTTYPE)

    ##  @brief Generate byte array for #COMMS_SwitchType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      swtType     Objectify tree for #COMMS_SwitchType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinarySwitchType(self, swtType):
        if swtType == 'master':
            val = COMMS_SWITCH_LIGHTSTACK_MASTER
        elif swtType == 'slave':
            val = COMMS_SWITCH_LIGHTSTACK_SLAVE
        else:
            val = COMMS_SWITCH_STANDARD

        return self.getByteArray(val, SIZE_SWITCHTYPE)


    ##  @brief Generate byte array for #COMMS_PolicerPacketFlgMaskType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      pktFlgMask  Objectify tree for
    #                               #COMMS_PolicerPacketFlgMaskType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPolicerPacketFlgMask(self, pktFlgMask):

        if pktFlgMask == 'unicast-lookup-hit':
            val = PACKET_UNICAST_LOOKUP_HIT
        elif pktFlgMask == 'multicast-lookup-hit':
            val = PACKET_MULTICAST_LOOKUP_HIT
        elif pktFlgMask == 'reserved-multicast-lookup-hit':
            val = PACKET_RES_MULTICAST_LOOKUP_HIT
        elif pktFlgMask == 'broadcast-lookup-hit':
            val = PACKET_BROADCAST_LOOKUP_HIT
        elif pktFlgMask == 'multicast-lookup-failed':
            val = PACKET_MULTICAST_LOOKUP_FAILED
        else:
            val = PACKET_UNICAST_LOOKUP_FAILED

        return self.getByteArray(val, SIZE_UINT32)

    ##  @brief Generate byte array for #COMMS_PortPolicerActType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      policerAct  Objectify tree for
    #                               #COMMS_PortPolicerActType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortPolicerAct(self, policerAct):
        if policerAct == 'block':
            val = PORT_RATE_POLICER_BLOCK_SRC
        else:
            val = PORT_RATE_POLICER_DROP_PKT

        return self.getByteArray(val, SIZE_UINT32)

    ##  @brief Generate byte array for #COMMS_PortPolicerCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      policerCfg  Objectify tree for
    #                               #COMMS_PortPolicerCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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

    ##  @brief Generate byte array for #COMMS_PortIngressCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      ingressCfg  Objectify tree for
    #                               #COMMS_PortIngressCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortIngressCfg(self, ingressCfg):
        data = bytearray()

        self.log.dump('defaultVLAN = {0},'.format(ingressCfg.defaultVLAN))
        data += self.getByteArray(ingressCfg.defaultVLAN, SIZE_VLANIDTYPE)

        self.log.dump('defaultPrio = {0},'.format(ingressCfg.defaultPrio))
        data += self.getByteArray(ingressCfg.defaultPrio, SIZE_UINT8)

        self.log.dump('dropUntagged = {0},'.format(ingressCfg.dropUntagged))

        if (ingressCfg.dropUntagged == True) != (ingressCfg.defaultVLAN == 0):
            print 'ERROR: dropUntagged and defaultVLAN should be mutually exclusive'
            sys.exit(1)
        data += self.getByteArray(ingressCfg.dropUntagged, SIZE_UINT32)

        if hasattr(ingressCfg, 'tc'):
            self.log.dump('tc = {0},'.format(ingressCfg.tc))
            data += self.getByteArray(ingressCfg.tc, SIZE_UINT8)
        else:
            data += self.getByteArray(8, SIZE_UINT8)

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
                data += self.getByteArray(int(each), SIZE_UINT8)
        else:
            data += bytearray(8)
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #COMMS_ShaperCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      shaperCfg   Objectify tree for #COMMS_ShaperCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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

    ##  @brief Generate byte array for #COMMS_FifoCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      fifoCfg     Objectify tree for #COMMS_FifoCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryFifoCfg(self, fifoCfg):
        data = bytearray()

        self.log.dump('id = {0},'.format(fifoCfg.id))
        data = self.getByteArray(fifoCfg.id, SIZE_UINT32)

        self.log.dump('minLen = {0},'.format(fifoCfg.minLen))
        data += self.getByteArray(fifoCfg.minLen, SIZE_UINT32)

        self.log.dump('tc = {0},'.format(fifoCfg.tc))
        data += self.getByteArray(fifoCfg.tc, SIZE_UINT8)

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
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      schedAlgo   Objectify tree for #ETHWERSWT_SchedAlgoType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinarySchedAlgo(self, schedAlgo):
        if schedAlgo == 'SP':
            val = COMMS_SCHED_ALGO_SP
        elif schedAlgo == 'WRR':
            val = COMMS_SCHED_ALGO_WRR
        else:
            val = COMMS_SCHED_ALGO_DRR

        return self.getByteArray(val, SIZE_UINT8)

    ##  @brief Get integer value for a macro of type #ETHWERSWT_SchedAlgoType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      schedAlgo   String of type #ETHWERSWT_SchedAlgoType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getSchedAlgoVal(self, schedAlgo):
        if schedAlgo == 'COMMS_SCHED_ALGO_SP':
            val = COMMS_SCHED_ALGO_SP
        elif schedAlgo == 'COMMS_SCHED_ALGO_WRR':
            val = COMMS_SCHED_ALGO_WRR
        else:
            val = COMMS_SCHED_ALGO_DRR

        return val

    ##  @brief Generate byte array for #COMMS_PortSchType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      portSch     Objectify tree for #COMMS_PortSchType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortSch(self, portSch):
        data = bytearray()

        algoArray = array('I', [0, 0, 0, 0, 0, 0, 0, 0])
        for i in range(8):
            algoArray[i] = self.getSchedAlgoVal(str(portSch.algo[i]))

        count = algoArray.count(COMMS_SCHED_ALGO_SP)
        if (count != 8) and (count != 0):
            firstSPIdx = algoArray.index(COMMS_SCHED_ALGO_SP)
            if firstSPIdx in range(1, 4):
                print 'ERROR: Invalid Scheduler configuration'
                sys.exit(1)

            for i in range((firstSPIdx + 1), 8):
                if algoArray[i] != COMMS_SCHED_ALGO_SP:
                    print 'ERROR: Invalid Scheduler configuration'
                    sys.exit(1)

        self.log.dump('algo = {')
        for each in portSch.algo:
            self.log.dump('{0},'.format(each))
            data += self.getBinarySchedAlgo(str(each))
        self.log.dump('},')

        return data

    ##  @brief Generate byte array for #COMMS_PortEgressCfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      egressCfg   Objectify tree for
    #                               #COMMS_PortEgressCfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
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
            if cnt < COMMS_PORT_FIFO_MAX:
                data += bytearray(SIZE_FIFOCFGTYPE * (COMMS_PORT_FIFO_MAX - cnt))
                while cnt <COMMS_PORT_FIFO_MAX:
                    self.log.dump('{},')
                    cnt += 1
        else:
            data += bytearray(SIZE_FIFOCFGTYPE * COMMS_PORT_FIFO_MAX)
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
                    data += self.getByteArray(each, SIZE_UINT8)
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


    ##  @brief Generate byte array for #COMMS_SwitchPortConfigType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      portCfg     Objectify tree for #COMMS_SwitchPortConfigType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinarySwitchPortCfg(self, portCfg):
        data = bytearray()

        self.log.dump('role = {0},'.format(portCfg.role))
        data += self.getBinaryPortType(portCfg.role)

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

    ##  @brief Generate byte array for #COMMS_PortConfigType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      portCfg     Objectify tree for #COMMS_PortConfigType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortCfg(self, portCfg):
        data = bytearray()

        data += self.getByteArray(portCfg.portID, SIZE_UINT32)

        self.log.dump('{')
        data += self.getBinarySwitchPortCfg(portCfg.switchPortCfg)
        self.log.dump('}')
        return data

    ##  @brief Generate byte array for MAC address
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_SwitchTablesConfigurator object
    #   @param[in]      macAddr     Objectify tree for MAC address
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryMacAddr(self, macAddr):
        data = bytearray()
        for index in range(0, 17, 3):
            hexStr = '0x'
            hexStr += macAddr[index]
            hexStr += macAddr[index + 1]
            val = int(hexStr, 16)
            data += self.getByteArray(val , SIZE_UINT8)

        return data

    ##  @brief Generate byte array for port mask
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_SwitchTablesConfigurator object
    #   @param[in]      portMask    Objectify tree for port mask
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryPortMask(self, portMask):
        val = int(str(portMask), 16)
        data = self.getByteArray(val, SIZE_PORTMASKTYPE)
        return data

    ##  @brief Generate byte array for #COMMS_VLANEntryType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_SwitchTablesConfigurator object
    #   @param[in]      entry       Objectify tree for #COMMS_VLANEntryType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryVLANEntry(self, entry):
        data = bytearray()

        # Check for valid portMask
        portMask = int(str(entry.portMask), 16)
        if portMask == 0:
            print "portMask for VLAN", entry.vlan, "needs to be non-zero"
            sys.exit(1)

        # Check for tagMask to be a subset of portMask
        tagMask = int(str(entry.tagMask), 16)
        if ((tagMask | portMask) != portMask):
            print "tagMask", entry.tagMask, "is not a subset of portMask",entry.portMask

        data += self.getByteArray(entry.vlan, SIZE_VLANIDTYPE)
        data += self.getBinaryPortMask(entry.portMask)
        data += self.getBinaryPortMask(entry.tagMask)

        return data

    ##  @brief Generate byte array for #COMMS_ARLEntryType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_SwitchTablesConfigurator object
    #   @param[in]      entry       Objectify tree for #COMMS_ARLEntryType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryARLEntry(self, entry):
        data = bytearray()

        data += self.getBinaryMacAddr(str(entry.macAddress))
        data += self.getByteArray(int(entry.vlan), SIZE_VLANIDTYPE)
        portMask = int(str(entry.destinationPortMask), 16)
        if portMask == 0:
            print "portMask", portMask, "needs to be non-zero"
            sys.exit(1)

        # Add a check for unicast mac with multicast portmask
        msNibble = int(str(entry.macAddress)[1], 16)
        isNotPowerOf2 = (portMask & (portMask - 1))
        if ((msNibble & 0x1) == 0) and (isNotPowerOf2 != 0):
            print "Invalid portMask", entry.destinationPortMask, "for unicast entry", entry.macAddress
            sys.exit(1)
        data += self.getBinaryPortMask(entry.destinationPortMask)

        return data

    ##  @brief Generate byte array for #COMMS_StreamPolicerConfigType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self                 #COMMS_CfgConfigurator object
    #   @param[in]      entry                Objectify tree for #COMMS_StreamPolicerConfigType
    #   @param[in]      arlIndex             Index of the ARL entry in the arlTable that this policer belongs to
    #   @param[in]      destinationPortMask  Destination Port mask of the ARL entry that this policer belongs to
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryStreamPolicer(self, entry, arlIndex, destinationPortMask):
        data = bytearray()

        if (entry.rate == 0) or (entry.burstSize == 0):
            print "Policer rate/burstSize need to be non zero"
            sys.exit(1)

        if int(str(entry.ingressPortMask), 16) & destinationPortMask:
            print "IngressPortMask", entry.ingressPortMask, "and destinationPortMask", hex(destinationPortMask), "need to be mutually exclusive"
            sys.exit(1)
        data += self.getByteArray(entry.rate, SIZE_UINT32)
        data += self.getByteArray(entry.burstSize, SIZE_UINT32)
        data += self.getByteArray(entry.dropThreshold, SIZE_UINT32)
        if ((entry.block != False) and (entry.report != False)) and ((entry.dropThreshold == 0) or (entry.monitoringInterval == 0)):
            print "Policer dropThreshold/monitoringInterval need to be non zero for reporting or blocking action"
            sys.exit(1)

        data += self.getByteArray(entry.monitoringInterval, SIZE_UINT32)
        data += self.getByteArray(int(str(entry.ingressPortMask), 16), SIZE_UINT32)
        data += self.getByteArray(arlIndex, SIZE_UINT16)
        action = int(entry.block) | (int(entry.report) << 1)
        data += self.getByteArray(action, SIZE_UINT8)
        # reserved
        data += self.getByteArray(0, SIZE_UINT8)

        return data

    ##  @brief Generate byte array for #COMMS_CfgType
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #   @param[in]      cfg         Objectify tree for #COMMS_CfgType
    #
    #   @return     Bytearray representation
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinaryCOMMSCfg(self, cfg):
        data = bytearray()

        # magicId
        data += self.getByteArray(0x4D4D4F43, SIZE_UINT32)

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
            if cnt < COMMS_MAX_PORTS:
                data += bytearray(SIZE_PORTCFGTYPE * (COMMS_MAX_PORTS - cnt))
                while cnt < COMMS_MAX_PORTS:
                    self.log.dump('{},')
                    cnt += 1
        else:
            data += bytearray(SIZE_PORTCFGTYPE * COMMS_MAX_PORTS)
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

        vidList = {}

        cnt = 0
        tableSz = 0
        if hasattr(cfg, 'vidTable'):
            for each in cfg.vidTable.vid:
                data += self.getBinaryVLANEntry(each)
                vidList[int(each.vlan)] = int(str(each.portMask), 16)
                cnt += 1
            tableSz = cnt
            if cnt < COMMS_MAX_VLANS:
                data += bytearray(SIZE_VLANENTRYTYPE * (COMMS_MAX_VLANS - cnt))
        else:
            data += bytearray(SIZE_VLANENTRYTYPE * COMMS_MAX_VLANS)

        data += self.getByteArray(tableSz, SIZE_UINT32)

        cnt = 0
        tableSz = 0
        if hasattr(cfg, 'streamTable'):
            for each in cfg.streamTable.stream:
                if each.vlan != 0:
                    if each.vlan not in vidList:
                        print "VLAN", each.vlan, "does not exist"
                        sys.exit(1)
                    if (int(str(each.destinationPortMask), 16) | vidList[each.vlan]) != vidList[each.vlan]:
                        print "Portmask for entry", each.destinationPortMask, "needs to be allowed on VLAN", each.vlan
                        sys.exit(1)
                data += self.getBinaryARLEntry(each)
                cnt += 1
            tableSz = cnt
            if cnt < COMMS_MAX_ARL_ENTRIES:
                data += bytearray(SIZE_ARLENTRYTYPE * (COMMS_MAX_ARL_ENTRIES - cnt))
        else:
            data += bytearray(SIZE_ARLENTRYTYPE * COMMS_MAX_ARL_ENTRIES)

        data += self.getByteArray(tableSz, SIZE_UINT32)

        cnt = 0
        numStreams = 0
        if hasattr(cfg, 'streamTable'):
            for each in cfg.streamTable.stream:
                if hasattr (each, 'policerList'):
                    allIngressPortMask = 0
                    for policer in each.policerList.policer:
                        data += self.getBinaryStreamPolicer(policer, cnt, int(str(each.destinationPortMask), 16))
                        numStreams += 1
                        if (allIngressPortMask & int(str(policer.ingressPortMask), 16)):
                            print "Same port cannot be part of multiple policers for a stream"
                            sys.exit(1)
                        else:
                            allIngressPortMask |= int(str(policer.ingressPortMask), 16)
                cnt += 1
            if numStreams < COMMS_MAX_STREAMS:
                data += bytearray(SIZE_STREAMPOLICERCONFIGTYPE * (COMMS_MAX_STREAMS - numStreams))
        else:
            data += bytearray(SIZE_STREAMPOLICERCONFIGTYPE * COMMS_MAX_STREAMS)

        data += self.getByteArray(numStreams, SIZE_UINT32)
        return data

    ##  @brief Generate Communication Subsystem configuration binary
    #
    #   @behavior Sync, Re-entrant
    #
    #   @pre None
    #
    #   @param[in]      self        #COMMS_CfgConfigurator object
    #
    #   @return     Communication Subsystem configuration in binary format
    #
    #   @post None
    #
    #   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
    #
    #   @limitations None
    def getBinData(self):
        binData = bytearray()

        self.log.dump('COMMS_Cfg = {')
        binData = self.getBinaryCOMMSCfg(self.cfg)
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
#   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
#
#   @limitations None
def usage():
    print """
usage: comms_config.py
    [-h, --help]    Display this help message
    -i, --xml       Input XML file
    -o, --out       Output binary file
    -s, --schema    Input Schema file
    [-l, --log]     Log file [loging disabled if not log file is not mentioned]"""
    sys.exit(1)

##  @brief main function for Communication Subsystem Configurator
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
#   @trace #BRCM_SWREQ_COMMS_CFG_TYPE
#
#   @limitations None
def main():
    XMLFile = None
    BinOutputFile = None
    SchemaFile = None
    LogFile = None
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'hi:o:s:l:',
                    ['help=', 'xml=', "out=", "schema=", "log="])
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
        elif opt in ('-s', '--schema'):
            if os.path.isfile(arg) == True:
                SchemaFile = arg
            else:
                print 'File {0} does not exist'.format(arg)
                usage()
        elif opt in ('-l', '--log'):
            LogFile = arg
        else:
            print 'Unknown Argument {0}'.format(opt)
            usage()
    if ((XMLFile == None) or (BinOutputFile == None) or (SchemaFile == None)):
        print 'Missing arguments'
        usage()

    logItem = Log(LogFile)
    logItem.startLogging()
    configurator = COMMS_CfgConfigurator(XMLFile, SchemaFile, logItem)
    fp = open(BinOutputFile, 'wb')
    fp.write(configurator.getBinData())
    fp.close
    print BinOutputFile + ' is generated successfully.'
    logItem.stopLogging()

if __name__ == '__main__':
    main()
## @}
