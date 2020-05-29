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

##  @defgroup grp_ets_configurator Configurator
##  @ingroup grp_ets
##
##  @addtogroup grp_ets_configurator
##  @{
##  @section sec_ets_config_overview Overview
##
##  This file generates the binary configuration image
##  for the ETS library. It has the XML schema for the
##  configuration elements.
##
##  @limitations None
##
##  @file ets_config.py
##  @brief ETS Configurator
##  This script file contains the configurator implementation
##  for ETS library.
##  @version 0.1 Changelist0

import sys
import os.path
import getopt
import lxml
import struct

from lxml import etree
from lxml import objectify
##  @name Component API IDs
##  @{
##  @brief API IDs for ETS Configurator
##

    ## @brief #ets_config.ETS_CfgConfigurator
BRCM_SWARCH_ETS_CONFIGURATOR_CLASS               = 0
    ## @brief #ets_config.ETS_ClockModeSchemaGlobal()
BRCM_SWARCH_ETS_CLOCKMODESCHEMA_GLOBAL           = 1
    ## @brief #ets_config.ETS_RoleSchemaGlobal()
BRCM_SWARCH_ETS_ROLESCHEMA_GLOBAL                = 2
    ## @brief #ets_config.ETS_SyncAbsenceTimeoutSchemaGlobal()
BRCM_SWARCH_ETS_SYNCABSENCETIMEOUTSCHEMA_GLOBAL  = 3
    ## @brief #ets_config.ETS_RateRatioSchemaGlobal()
BRCM_SWARCH_ETS_RATERATIOSCHEMA_GLOBAL           = 4
    ## @brief #ets_config.ETS_PortNumSchemaGlobal()
BRCM_SWARCH_ETS_PORTNUMSCHEMA_GLOBAL             = 5
    ## @brief #ets_config.ETS_InitPdelayIntervalSchemaGlobal()
BRCM_SWARCH_ETS_INITPDELAYINTERVALSCHEMA_GLOBAL  = 6
    ## @brief #ets_config.ETS_OperPdelayIntervalSchemaGlobal()
BRCM_SWARCH_ETS_OPERPDELAYINTVLSCHEMA_GLOBAL     = 7
    ## @brief #ets_config.ETS_InitSyncIntervalSchemaGlobal()
BRCM_SWARCH_ETS_INITSYNCINTVLSCHEMA_GLOBAL       = 8
    ## @brief #ets_config.ETS_OperSyncIntervalSchemaGlobal()
BRCM_SWARCH_ETS_OPERSYNCINTVLSCHEMA_GLOBAL       = 9
    ## @brief #ets_config.ETS_PeerDelaySchemaGlobal()
BRCM_SWARCH_ETS_PEERDELAYSCHEMA_GLOBAL           = 10
    ## @brief #ets_config.ETS_SyncReceiptTimeoutSchemaGlobal()
BRCM_SWARCH_ETS_SYNCRECEIPTTIMEOUTSCHEMA_GLOBAL  = 11
    ## @brief #ets_config.ETS_AllowedLostResponseSchemaGlobal()
BRCM_SWARCH_ETS_ALLOWEDLOSTRESPONSESCHEMA_GLOBAL = 12
    ## @brief #ets_config.ETS_IntfCfgSchemaGlobal()
BRCM_SWARCH_ETS_INTFCFGSCHEMA_GLOBAL             = 13
    ## @brief #ets_config.ETS_IntfListSchemaGlobal()
BRCM_SWARCH_ETS_INTFLISTSCHEMA_GLOBAL            = 14
    ## @brief #ets_config.ETS_CfgSchemaGlobal()
BRCM_SWARCH_ETS_CFGSCHEMA_GLOBAL                 = 15
    ## @brief #ets_config.usage()
BRCM_SWARCH_ETS_USAGE_PROC                       = 16
##  @}


##  @brief Schema for #ETS_ClockMode
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_ClockModeSchemaGlobal = """
    <xs:simpleType name ="ClockModeType">
        <xs:restriction base = "xs:string">
            <xs:enumeration value = "gm"/>
            <xs:enumeration value = "slave"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_Role
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_RoleSchemaGlobal = """
    <xs:simpleType name = "RoleType">
        <xs:restriction base = "xs:string">
            <xs:enumeration value = "master"/>
            <xs:enumeration value = "slave"/>
            <xs:enumeration value = "stacking"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS Sync Absence Timeuot
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_SyncAbsenceTimeoutSchemaGlobal = """
    <xs:simpleType name = "AvnuSyncAbsenceTimeoutType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "1"/>
            <xs:maxInclusive value = "20"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS Rate Ratio
##  Used for both GM Rate Ratio & Neighbour
##  Rate Ratio
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_RateRatioSchemaGlobal = """
    <xs:simpleType name = "RateRatioType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "0"/>
            <xs:maxInclusive value = "4294967295"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS HW port number
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_PortNumSchemaGlobal = """
    <xs:simpleType name = "PortNumType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "0"/>
            <xs:maxInclusive value = "255"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_InitLogPdelay
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_InitPdelayIntervalSchemaGlobal = """
    <xs:simpleType name = "InitLogPdelayIntervalType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:enumeration value = "0"/>
            <xs:enumeration value = "127"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_OperLogPdelay
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_OperPdelayIntervalSchemaGlobal = """
    <xs:simpleType name = "OperLogPdelayIntervalType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:enumeration value = "0"/>
            <xs:enumeration value = "1"/>
            <xs:enumeration value = "2"/>
            <xs:enumeration value = "3"/>
            <xs:enumeration value = "127"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_InitLogSync
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_InitSyncIntervalSchemaGlobal = """
    <xs:simpleType name = "InitLogSyncIntervalType">
        <xs:restriction base = "xs:negativeInteger">
            <xs:enumeration value = "-5"/>
            <xs:enumeration value = "-4"/>
            <xs:enumeration value = "-3"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_OperLogSync
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_OperSyncIntervalSchemaGlobal = """
    <xs:simpleType name = "OperLogSyncIntervalType">
        <xs:restriction base = "xs:nonPositiveInteger">
            <xs:enumeration value = "-3"/>
            <xs:enumeration value = "-2"/>
            <xs:enumeration value = "-1"/>
            <xs:enumeration value = "0"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS Peer Delay
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_PeerDelaySchemaGlobal = """
    <xs:simpleType name = "PeerDelayType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "0"/>
            <xs:maxInclusive value = "1000000"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS Sync Receipt Timeout
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_SyncReceiptTimeoutSchemaGlobal = """
    <xs:simpleType name = "SyncReceiptTimeoutType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "2"/>
            <xs:maxInclusive value = "255"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for ETS Allowed Lost Responses
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_AllowedLostResponseSchemaGlobal = """
    <xs:simpleType name = "AllowedLostResponsesType">
        <xs:restriction base = "xs:unsignedInt">
            <xs:minInclusive value = "0"/>
            <xs:maxInclusive value = "65535"/>
        </xs:restriction>
    </xs:simpleType>"""

##  @brief Schema for #ETS_IntfConfig
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_IntfCfgSchemaGlobal = """
    <xs:complexType name = "ETS_IntfCfg">
        <xs:sequence>
            <xs:element
                name = "hwPortNum"
                type = "PortNumType"
                />
            <xs:element
                name = "asCapable"
                type = "xs:boolean"
                default = "false"
                />
            <xs:element
                name = "role"
                type = "RoleType"
                />
            <xs:element
                name = "initLogPdelayInterval"
                type = "InitLogPdelayIntervalType"
                default = "0"
                minOccurs = "0"
                />
            <xs:element
                name = "operLogPdelayInterval"
                type = "OperLogPdelayIntervalType"
                default = "0"
                minOccurs = "0"
                />
            <xs:element
                name = "initLogSyncInterval"
                type = "InitLogSyncIntervalType"
                default = "-5"
                minOccurs = "0"
                />
            <xs:element
                name = "operLogSyncInterval"
                type = "OperLogSyncIntervalType"
                default = "-3"
                minOccurs = "0"
                />
            <xs:element
                name = "nbrPropDelay"
                type = "PeerDelayType"
                />
            <xs:element
                name = "nbrRateRatio"
                type = "RateRatioType"
                minOccurs = "0"
                default = "1073741824"
                />
            <xs:element
                name = "syncReceiptTimeout"
                type = "SyncReceiptTimeoutType"
                minOccurs = "0"
                default = "3"
                />
            <xs:element
                name = "allowedLostResp"
                type = "AllowedLostResponsesType"
                minOccurs = "0"
                default = "3"
                />
        </xs:sequence>
    </xs:complexType>"""

##  @brief Schema for list of #ETS_IntfConfig
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_IntfListSchemaGlobal = """
    <xs:complexType name = "IntfListType">
        <xs:sequence>
            <xs:element
                name = "intf"
                type = "ETS_IntfCfg"
                minOccurs = "1"
                maxOccurs = "16"
                />
        </xs:sequence>
    </xs:complexType>"""

##  @brief Schema for list of #ETS_IntfConfig
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
ETS_CfgSchemaGlobal = """
    <xs:complexType name = "ETS_Config">
        <xs:sequence>
            <xs:element
                name = "globalAdmin"
                type = "xs:boolean"
                minOccurs = "1"
                maxOccurs = "1"
                />
            <xs:element
                name = "clockMode"
                type = "ClockModeType"
                minOccurs = "1"
                maxOccurs = "1"
                />
            <xs:element
                name = "isBoundaryClock"
                type = "xs:boolean"
                minOccurs = "0"
                maxOccurs = "1"
                />
            <xs:element
                name = "avnuSyncAbsenceTimeout"
                type = "AvnuSyncAbsenceTimeoutType"
                minOccurs = "0"
                default = "10"
                />
            <xs:element
                name = "gmRateRatio"
                type = "RateRatioType"
                minOccurs = "0"
                default = "1073741824"
                />
            <xs:element
                name = "intfCfg"
                type = "IntfListType"
                minOccurs = "1"
                />
        </xs:sequence>
    </xs:complexType>
    <xs:element
        name = "ETS_Cfg"
        type = "ETS_Config"
        />"""

##  @brief ETS Configurator
##
##  Component configurator class for processing the schema
##  elements of ETS library.
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
class ETS_CfgConfigurator:
    ##  @brief Brief description of __init__
    ##
    ##  Configurator initialization function
    ##
    ##  @behavior Sync, Non-reentrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      xmlFile     XML file containing ETS configuration
    ##
    ##  @return         An instance of this class
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_ETS_CONFIG
    ##
    ##  @limitations None
    ##
    ##  @code{.unparsed}
    ##  schemaString = XML schema string
    ##  schemaObj = etree.XMLSchema(objectify.fromstring(schemaString))
    ##  self.parser = objectify.makeparser(schema = schemaObj)
    ##  @endcode

    def __init__(self):
        self.ETS_Cfg = None

        schemaString ="""<?xml version="1.0"?>
                      <xs:schema xmlns:xs = "http://www.w3.org/2001/XMLSchema">"""
        schemaString += ETS_ClockModeSchemaGlobal
        schemaString += ETS_RoleSchemaGlobal
        schemaString += ETS_SyncAbsenceTimeoutSchemaGlobal
        schemaString += ETS_RateRatioSchemaGlobal
        schemaString += ETS_PortNumSchemaGlobal
        schemaString += ETS_InitPdelayIntervalSchemaGlobal
        schemaString += ETS_OperPdelayIntervalSchemaGlobal
        schemaString += ETS_InitSyncIntervalSchemaGlobal
        schemaString += ETS_OperSyncIntervalSchemaGlobal
        schemaString += ETS_PeerDelaySchemaGlobal
        schemaString += ETS_SyncReceiptTimeoutSchemaGlobal
        schemaString += ETS_AllowedLostResponseSchemaGlobal
        schemaString += ETS_IntfCfgSchemaGlobal
        schemaString += ETS_IntfListSchemaGlobal
        schemaString += ETS_CfgSchemaGlobal
        schemaString += """</xs:schema>"""

        schemaObj = etree.XMLSchema(objectify.fromstring(schemaString))
        self.parser = objectify.makeparser(schema = schemaObj)

##  @brief Load XML configuration and convert it to object format
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      self           #ets_config.ETS_CfgConfigurator object
##  @param[in]      xmlfile        XML file path
##
##  @return         void
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
    def loadXML(self, xmlfile):
        fp = open(xmlfile, 'r')
        xml = fp.read()
        fp.close

        self.ETS_Cfg = objectify.fromstring(xml, self.parser)

##  @brief Convert ETS interface configuration to binary format
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      self           #ets_config.ETS_CfgConfigurator object
##  @param[in]      intf           Interface object
##  @param[in]      cfg            ETS Configuration object
##
##  @return         ETS interface configuration in binary format
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
    def getBinaryIntfCfg(self, intf, cfg):

        data = bytearray()

        data += bytearray(struct.pack("B", intf.hwPortNum))
        data += bytearray(struct.pack("B", intf.asCapable))

        if intf.role == 'master':
            data += bytearray(struct.pack('B', 0))
        elif intf.role == 'slave':
            data += bytearray(struct.pack('B', 1))
            self.numSlavePorts += 1
# Validation: Do not allow multiple slave ports on slave device.
            if self.numSlavePorts != 1:
                print 'Multiple slave ports cannot exist'
                sys.exit(1)
        elif intf.role == 'stacking':
            data += bytearray(struct.pack('I', 2))
            self.numStackingPorts += 1

# Validation: Do not allow slave ports on GM device.
# Validation: Do not allow Pdelay to be disabled on slave ports.
        if intf.role == 'slave':
            if intf.initLogPdelayInterval == 127 or intf.operLogPdelayInterval == 127:
                print 'Invalid Pdelay interval for slave port', intf.hwPortNum
                sys.exit(1)
            if cfg.clockMode == 'gm':
                print 'Invalid ' + intf.role + ' role for port', intf.hwPortNum , 'in gm mode'
                sys.exit(1)
        data += bytearray(struct.pack("I", intf.initLogPdelayInterval))
        data += bytearray(struct.pack("I", intf.operLogPdelayInterval))
        data += bytearray(struct.pack("i", intf.initLogSyncInterval))
        data += bytearray(struct.pack("i", intf.operLogSyncInterval))
        data += bytearray(struct.pack("I", intf.nbrPropDelay))
        data += bytearray(struct.pack("I", intf.nbrRateRatio))
        data += bytearray(struct.pack("B", intf.syncReceiptTimeout))
        data += bytearray(struct.pack("<H", intf.allowedLostResp))

        return data

##  @brief Convert ETS clock mode to binary format
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      self           #ets_config.ETS_CfgConfigurator object
##  @param[in]      clockMode      Clock mode string
##
##  @return         ETS clock mode in binary format
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
    def getBinaryClockMode(self, clockMode):
        if clockMode == 'gm':
            val = 0
        elif clockMode == 'slave':
            val = 1

        return struct.pack("B", val)

##  @brief Generate ETS configuration binary data
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      self        #ets_config.ETS_CfgConfigurator object
##  @param[in]      cfg         ETS configuration object
##
##  @return         ETS configuration in binary format
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
    def getBinaryETS_Cfg(self, cfg):
        data = bytearray()
        self.numSlavePorts = 0
        self.numStackingPorts = 0
        self.numInterfaces = 0
        if hasattr(cfg, 'intfCfg'):
            for each in cfg.intfCfg.intf:
                self.numInterfaces += 1

# Populate Magic Number
        data += bytearray(struct.pack('I', 0x50545047))

        data += bytearray(struct.pack("<B", cfg.globalAdmin))
        data += self.getBinaryClockMode(cfg.clockMode)
# Validation: Boundary clock mode not allowed for end points
        if hasattr(cfg, 'isBoundaryClock'):
            if cfg.isBoundaryClock == True:
                if (self.numInterfaces == 1):
                    print 'End point cannot be boundary clock'
                    sys.exit(1)
                if (cfg.clockMode == 'gm'):
                    print 'GM cannot be boundary clock'
                    sys.exit(1)
            data += bytearray(struct.pack("<B", cfg.isBoundaryClock))
        else:
            data += bytearray(struct.pack("<B", 0))

        data += bytearray(struct.pack("<B", cfg.avnuSyncAbsenceTimeout))
        data += bytearray(struct.pack("<I", cfg.gmRateRatio))
        data += bytearray(struct.pack("<I", len(cfg.intfCfg.intf)))

        if hasattr(cfg, 'intfCfg'):
            for each in cfg.intfCfg.intf:
                data += self.getBinaryIntfCfg(each, cfg)

# Validation: Do not allow multiple slave ports on slave device.
        if (cfg.clockMode == 'slave') and (self.numSlavePorts != 1) and (self.numStackingPorts == 0):
            print 'All ports configured as master in slave device'
            sys.exit(1)

        return data

##  @brief Write ETS configuration binary file
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      self        #ets_config.ETS_CfgConfigurator object
##  @param[in]      binfile     Binary output file path
##
##  @return         void
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
    def generateBin(self, binfile):
        binData = bytearray()

        binData = self.getBinaryETS_Cfg(self.ETS_Cfg)

        fout = open(binfile, 'wb')
        fout.write(binData)
        fout.close()

##  @brief Print usage for ETS Configurator
##
##  @behavior Sync, Non-reentrant
##
##  @param          void
##
##  @return void
##
##  @post Application exits
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
def usage():
    print """\
    Usage: ets_config.py
    [-h, --help]    Display this help message
    -i, --xml       Input XML file
    -o, --out       Output binary file"""
    sys.exit(1)

##  @brief main function for ETS Configurator
##
##  @behavior Sync, Re-entrant
##
##  @pre None
##
##  @param[in]      void
##
##  @return void
##
##  @post None
##
##  @trace #BRCM_SWREQ_ETS_CONFIG
##
##  @limitations None
def main():
    XMLFile = None
    BinOutputFile = None
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'hi:o:',
            ['help=', 'xml=', "out="])
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
        else:
            print 'Unknown Argument {0}'.format(opt)
            usage()
    if (XMLFile == None) or (BinOutputFile == None):
        print 'Missing arguments'
        usage()

    configurator = ETS_CfgConfigurator()
    configurator.loadXML(XMLFile)
    configurator.generateBin(BinOutputFile)
    del configurator

if __name__ == '__main__':
    main()
## @}

