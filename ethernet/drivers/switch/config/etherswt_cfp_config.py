#!/usr/bin/python2

#
# $Id$
# Copyright: Copyright 2018-2019 Broadcom Limited.
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

##  @defgroup grp_cfp_config CFP Configurator
##  @ingroup grp_eth_switchdrv
##
##  @addtogroup grp_cfp_config
##  @{
##  @section sec_cfp_config_overview Overview
##  TBD
##
##  @limitations None
##
##  @file etherswt_cfp_config.py
##  @brief CFP Configurator
##  This script file contains the configurator implementation for CFP
##  @version 0.1 Initial version
##

import sys
import os.path
import getopt
import lxml
import struct

from lxml import etree
from lxml import objectify
from array import *

CFP_MAX_RULES                  = 256
CFP_MAX_UDFS                   = 9
CFP_MAX_SLICES                 = 3

#CFP_PktLenCorrType
CFP_PKTLENCORR_NONE            = 0
CFP_PKTLENCORR_ADD_PRE_SFD     = 1
CFP_PKTLENCORR_ADD_PRE_SFD_IFG = 2

#CFP_UDFBaseType
CFP_UDFBASE_SOP                = 0
CFP_UDFBASE_ENDL2HDR           = 2
CFP_UDFBASE_ENDL3HDR           = 3

#CFP_AcceptableFrameTypeShift
CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT             = 21
CFP_ACCEPTABLEFRAME_PRIO_TAGGED_SHIFT          = 22
CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT          = 23

#CFP_L2FramingType
CFP_L2FRAMING_DIXV2            = 0
CFP_L2FRAMING_SNAP_PUB         = 1
CFP_L2FRAMING_LLC              = 2
CFP_L2FRAMING_SNAP_PVT         = 3

#CFP_L3FramingType
CFP_L3FRAMING_IPV4             = 0
CFP_L3FRAMING_IPV6             = 1
CFP_L3FRAMING_NONIP            = 3

#CFP_TTLValType
CFP_TTL_VAL_0                  = 0
CFP_TTL_VAL_1                  = 1
CFP_TTL_VAL_OTHERS             = 2
CFP_TTL_VAL_255                = 3

#CFP_ChangeFwdMapType
CFP_CHANGEFWDMAP_NON           = 0
CFP_CHANGEFWDMAP_REM           = 1
CFP_CHANGEFWDMAP_REP           = 2
CFP_CHANGEFWDMAP_ADD           = 3

#CFP_ColorType
CFP_COLOR_GREEN                = 0
CFP_COLOR_YELLOW               = 1
CFP_COLOR_RED                  = 2

#CFP_PolicerModeType
CFP_POLICERMODE_RFC2698        = 0
CFP_POLICERMODE_RFC4115        = 1
CFP_POLICERMODE_MEF            = 2
CFP_POLICERMODE_DISABLED       = 3

# Shift for mode field in policer flags
POLICERMODE_SHIFT              = 3

SIZE_UINT8                     = 1
SIZE_UINT16                    = 2
SIZE_UINT32                    = 4
SIZE_RULECFGTYPE               = 112
SIZE_UDFTYPE                   = 5

##  @brief CFP Configurator
##
##  CFP configurator class for generating CFP configuration binary.
##
##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
class CFP_CfgConfigurator:
    ##  @brief Instantiation operation
    ##
    ##  Creates a new instanace of the class
    ##
    ##  @behavior Sync, Non-reentrant
    ##
    ##  @pre TBD
    ##
    ##  @param[in]      self            #CFP_CfgConfigurator object
    ##  @param[in]      xmlFile         XML file containing CFP configuration
    ##  @param[in]      schemaFile      schema file for CFP configuration
    ##
    ##  @return     An instance of this class
    ##
    ##  @post TBD
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def __init__(self, xmlFile, schemaFile):
        schema = etree.XMLSchema(etree.parse(schemaFile))
        self.parser = objectify.makeparser(schema = schema)

        with open(xmlFile, 'r') as file:
            xml = file.read()

        self.cfg = objectify.fromstring(xml, self.parser)

    ##  @brief Generate byte array
    ##
    ##  This function generates the bytearray representation
    ##  for the given integer.
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      value       Integer to be converted into bytearray
    ##  @param[in]      size        Size of the bytearray (in bytes)
    ##
    ##  @return     Bytearray representation of the given integer
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getByteArray(self, value, size):
        if size == 1:
            data = struct.pack("<B", value)
        elif size == 2:
            data = struct.pack("<H", value)
        else:
            data = struct.pack("<I", value)

        return bytearray(data)

    ##  @brief Convert string to integer for #CFP_UDFBaseType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      base        Objectify tree for #CFP_UDFBaseType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryUDFBase(self, base):
        if base == 'SOP':
            val = CFP_UDFBASE_SOP
        elif base == 'End-L2-Header':
            val = CFP_UDFBASE_ENDL2HDR
        else:
            val = CFP_UDFBASE_ENDL3HDR
        return val

    ##  @brief Convert string to integer for #CFP_ChangeFwdMapType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      chngFwdMap  Objectify tree for #CFP_ChangeFwdMapType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryChngFwdMap(self, chngFwdMap):
        if chngFwdMap == 'No':
            val = CFP_CHANGEFWDMAP_NON
        elif chngFwdMap == 'Remove':
            val = CFP_CHANGEFWDMAP_REM
        elif chngFwdMap == 'Replace':
            val = CFP_CHANGEFWDMAP_REP
        else:
            val = CFP_CHANGEFWDMAP_ADD
        return val

    ##  @brief Convert string to integer for #CFP_L2FramingType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      l2Framing   Objectify tree for #CFP_L2FramingType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryL2Framing(self, l2Framing):
        if l2Framing == 'SNAP-Public':
            val = CFP_L2FRAMING_SNAP_PUB
        elif l2Framing == 'SNAP-Private':
            val = CFP_L2FRAMING_SNAP_PVT
        elif l2Framing == 'LLC':
            val = CFP_L2FRAMING_LLC
        else:
            val = CFP_L2FRAMING_DIXV2
        return val

    ##  @brief Compose tag status value and mask from tag elements
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      acceptableFrame   Objectify tree for #CFP_AcceptableFrameType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getTagValueAndMask(self, tag):
        mask = 0
        val = 0
        if hasattr(tag, 'acceptableFrame'):
            for acceptableFrame in tag.acceptableFrame:
                if ('Un-Tagged' == acceptableFrame):
                    val |= 1 << CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT
                elif ('Priority-Tagged' == acceptableFrame):
                    val |= 1 << CFP_ACCEPTABLEFRAME_PRIO_TAGGED_SHIFT
                else:
                    val |= 1 << CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT
            if (val == ((1 << CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT) + \
                        (1 << CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT))) :
               print("Error: Un-tagged or VLAN-Tagged is not a supported combination.")
               sys.exit(1)
        else:
            val =  (1 << CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT) \
                    | (1 << CFP_ACCEPTABLEFRAME_PRIO_TAGGED_SHIFT)\
                    | (1 << CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT)

        if hasattr(tag, 'id'):
            val += tag.id << 2
            val += 1 << 18
            if hasattr(tag, 'idMask'):
                mask += tag.idMask
            else:
                mask += 4095
        if hasattr(tag, 'dei'):
            val += int(tag.dei == True) << 14
            val += 1 << 19
        if hasattr(tag, 'pcp'):
            val += tag.pcp << 15
            val += 1 << 20
            if hasattr(tag, 'pcpMask'):
                mask += tag.pcpMask << 13
            else:
                mask += 7 << 13

        return val, mask

    ##  @brief Convert string to integer for #CFP_PolicerModeType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      policerMode Objectify tree for #CFP_PolicerModeType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryPolicerMode(self, policerMode):
        if policerMode == 'RFC2698':
            val = CFP_POLICERMODE_RFC2698
        elif policerMode == 'RFC4115':
            val = CFP_POLICERMODE_RFC4115
        elif policerMode == 'MEF':
            val = CFP_POLICERMODE_MEF
        else:
            val = CFP_POLICERMODE_DISABLED
        return val

    ##  @brief Convert string to integer for #CFP_TTLValType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      ttl         Objectify tree for #CFP_TTLValType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryTTLRange(self, ttl):
        if ttl == '0':
            val = CFP_TTL_VAL_0
        elif ttl == '1':
            val = CFP_TTL_VAL_1
        elif ttl == '255':
            val = CFP_TTL_VAL_255
        else:
            val = CFP_TTL_VAL_OTHERS
        return val

    ##  @brief Convert string to integer for #CFP_ColorType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      color       Objectify tree for #CFP_ColorType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryColor(self, color):
        if color == 'Red':
            val = CFP_COLOR_RED
        elif color == 'Yellow':
            val = CFP_COLOR_YELLOW
        else:
            val = CFP_COLOR_GREEN
        return val

    ##  @brief Convert string to integer for #CFP_PktLenCorrType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      corr        Objectify tree for #CFP_PktLenCorrType
    ##
    ##  @return     Integer representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryPktLenCorr(self, corr):
        if corr == 'Add-Pre-SFD-IFG':
            val = CFP_PKTLENCORR_ADD_PRE_SFD_IFG
        elif corr == 'Add-Pre-SFD':
            val = CFP_PKTLENCORR_ADD_PRE_SFD
        else:
            val = CFP_PKTLENCORR_NONE
        return val

    ##  @brief Generate byte array for #CFP_UDFType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      udfCfg      Objectify tree for #CFP_UDFType
    ##
    ##  @return     Bytearray representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryUDFCfg(self, udfCfg):
        data = bytearray()

        # value
        val = int(str(udfCfg.value), 16)
        data += self.getByteArray(val, SIZE_UINT16)

        # mask
        if hasattr(udfCfg, 'mask'):
            val = int(str(udfCfg.mask), 16)
        else:
            val = 65535
        data += self.getByteArray(val, SIZE_UINT16)

        # baseAndOffset
        val = udfCfg.offset + (self.getBinaryUDFBase(udfCfg.baseFld) << 5)
        data += self.getByteArray(val, SIZE_UINT8)

        return data

    ##  @brief Generate byte array for #CFP_RuleType
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #CFP_CfgConfigurator object
    ##  @param[in]      ruleCfg     Objectify tree for #CFP_RuleType
    ##
    ##  @return     Bytearray representation
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryRuleCfg(self, ruleCfg, l3Framing, row):
        data = bytearray()
        mask = 0
        val = 0
        if (CFP_L3FRAMING_NONIP == l3Framing):
            if hasattr(ruleCfg.key, 'etherType'):
                val = int(str(ruleCfg.key.etherType), 16)
                if hasattr(ruleCfg.key, 'etherTypeMask'):
                    mask |= int(str(ruleCfg.key.etherTypeMask), 16)
                else:
                    mask |= 0xFFFF
        elif (CFP_L3FRAMING_IPV4 == l3Framing) or (CFP_L3FRAMING_IPV6 == l3Framing):
            if hasattr(ruleCfg.key, 'ttlRange'):
                val += self.getBinaryTTLRange(ruleCfg.key.ttlRange)
                mask |= 0x3
            if hasattr(ruleCfg.key, 'authentication'):
                mask |= 0x4
                if ruleCfg.key.authentication:
                    val += 4
            if hasattr(ruleCfg.key, 'nonFirstFrag'):
                mask |= 0x8
                if ruleCfg.key.nonFirstFrag:
                    val += 8
            if hasattr(ruleCfg.key, 'fragmentation'):
                mask |= 0x10
                if ruleCfg.key.fragmentation:
                    val += 16
            if hasattr(ruleCfg.key, 'protocol'):
                val += int(str(ruleCfg.key.protocol), 16) << 5
                if hasattr(ruleCfg.key, 'protocolMask'):
                    mask |= int(str(ruleCfg.key.protocolMask), 16) << 5
                else:
                    mask |= 0x1FE0
            if hasattr(ruleCfg.key, 'tos'):
                val += int(str(ruleCfg.key.tos), 16) << 13
                if hasattr(ruleCfg.key, 'tosMask'):
                    mask |= int(str(ruleCfg.key.tosMask), 16) << 13
                else:
                    mask |= 0x1FE000
        else:
            print('Unsupported')

        # flags
        data += self.getByteArray(val, SIZE_UINT32)

        # flagsMask
        data += self.getByteArray(mask, SIZE_UINT32)

        # cTagFlags
        val = 0
        mask = 0
        if hasattr(ruleCfg.key, 'cTag'):
            val, mask = self.getTagValueAndMask(ruleCfg.key.cTag)
        else:
            val = (1 << CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT) \
                   | (1 << CFP_ACCEPTABLEFRAME_PRIO_TAGGED_SHIFT) \
                   | (1 << CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT)
        data += self.getByteArray(val, SIZE_UINT32)

        # cTagMask
        data += self.getByteArray(mask, SIZE_UINT16)

        # sTagFlags
        val = 0
        mask = 0
        if hasattr(ruleCfg.key, 'sTag'):
            val, mask = self.getTagValueAndMask(ruleCfg.key.sTag)
        else:
            val = (1 << CFP_ACCEPTABLEFRAME_UNTAGGED_SHIFT) \
                   | (1 << CFP_ACCEPTABLEFRAME_PRIO_TAGGED_SHIFT) \
                   | (1 << CFP_ACCEPTABLEFRAME_VLAN_TAGGED_SHIFT)
        data += self.getByteArray(val, SIZE_UINT32)


        # sTagMask
        data += self.getByteArray(mask, SIZE_UINT16)

        # ingressPortBitmap
        val = int(str(ruleCfg.key.ingressPortBitmap), 16)
        data += self.getByteArray(val, SIZE_UINT16)

        udfCnt = 0
        # udfList
        if hasattr(ruleCfg.key, 'udfList'):
            udfList = []
            for udf in ruleCfg.key.udfList.udf:
                udfList.append((udf.baseFld, udf.offset))
            # check for duplicates
            if len(udfList) != len(set(udfList)):
                print('Error: Duplicate UDFs (Base,Offset)')
                sys.exit(1)
            if (len(set(self.curSliceUDFs).union(set(udfList))) > 9):
                self.sliceNum -= 1
                if self.debug:
                    print 'Moving row', row, 'to slice', self.sliceNum, 'because of no space for additional UDFs, current set:', self.curSliceUDFs, '\n'
                self.curSliceUDFs = set(udfList)
                if (self.sliceNum < 0):
                    print('Error: Exceeded max UDFs possible in all slices row', row)
                    sys.exit(1)
            else:
                self.curSliceUDFs = self.curSliceUDFs.union(set(udfList))
            if self.debug:
                print 'row', row, '(' + self.type + ') configured on slice', self.sliceNum, 'using', len(udfList), 'UDFs:', udfList, '\n'
            for each in ruleCfg.key.udfList.udf:
                data += self.getBinaryUDFCfg(each)
                udfCnt += 1
            data += bytearray(SIZE_UDFTYPE * (CFP_MAX_UDFS - udfCnt))
        else:
            data += bytearray(SIZE_UDFTYPE * CFP_MAX_UDFS)
            if self.debug:
                print 'row', row, '(' + self.type + ') configured on slice', self.sliceNum, 'using 0 UDFs\n'

        # numEnabledUDFs
        data += self.getByteArray(udfCnt, SIZE_UINT8)

        # l2Framing
        if hasattr(ruleCfg.key, 'l2Framing'):
            val = self.getBinaryL2Framing(ruleCfg.key.l2Framing)
            if (((CFP_L3FRAMING_IPV4 == l3Framing) or (CFP_L3FRAMING_IPV6 == l3Framing)) and
               ((val != CFP_L2FRAMING_DIXV2) and
               (val != CFP_L2FRAMING_SNAP_PUB))):
               print("Error: Only DIXv2 and SNAP-Public allowed as L2 framing type with IPv4 and IPv6")
               sys.exit(1)
        else:
            val = CFP_L2FRAMING_DIXV2
        data += self.getByteArray(val, SIZE_UINT8)

        # l3Framing
        data += self.getByteArray(l3Framing, SIZE_UINT8)

        # enablePolicer
        if hasattr(ruleCfg.action, 'enablePolicer'):
            # cir
            data += self.getByteArray(ruleCfg.action.enablePolicer.cir.bktSize * 8, SIZE_UINT32)
            data += self.getByteArray(ruleCfg.action.enablePolicer.cir.burstSize, SIZE_UINT32)
            data += self.getByteArray((ruleCfg.action.enablePolicer.cir.rate * 256)/1000, SIZE_UINT32)
            # eir
            if hasattr(ruleCfg.action.enablePolicer, 'eir'):
                data += self.getByteArray(ruleCfg.action.enablePolicer.eir.bktSize * 8, SIZE_UINT32)
                data += self.getByteArray(ruleCfg.action.enablePolicer.eir.burstSize, SIZE_UINT32)
                data += self.getByteArray((ruleCfg.action.enablePolicer.eir.rate * 256)/1000, SIZE_UINT32)
            else:
                data += bytearray(3*SIZE_UINT32)
            # policerFlags
            val = self.getBinaryPolicerMode(ruleCfg.action.enablePolicer.policerMode) << POLICERMODE_SHIFT
            if hasattr(ruleCfg.action.enablePolicer, 'colorMode'):
                if ruleCfg.action.enablePolicer.colorMode:
                    val += 1
            if hasattr(ruleCfg.action.enablePolicer, 'policerAction'):
                if ruleCfg.action.enablePolicer.policerAction:
                    val += 2
            if hasattr(ruleCfg.action.enablePolicer, 'couplingFlag'):
                if ruleCfg.action.enablePolicer.couplingFlag:
                    val += 4
            data += self.getByteArray(val, SIZE_UINT8)
        else:
            data += bytearray(6*SIZE_UINT32)
            # Set policer mode to disabled
            val = CFP_POLICERMODE_DISABLED << POLICERMODE_SHIFT
            data += self.getByteArray(val, SIZE_UINT8)


        # changeInBoundDestination
        val = 0
        if hasattr(ruleCfg.action, 'changeInBoundDestination'):
            val = int(str(ruleCfg.action.changeInBoundDestination.portMap), 16) & 0xFFFF
            val += self.getBinaryChngFwdMap(ruleCfg.action.changeInBoundDestination.change) << 16
        data += self.getByteArray(val, SIZE_UINT32)

        # changeOutOfBoundDestination
        val = 0
        if hasattr(ruleCfg.action, 'changeOutOfBoundDestination'):
            val = int(str(ruleCfg.action.changeOutOfBoundDestination.portMap), 16) & 0xFFFF
            val += self.getBinaryChngFwdMap(ruleCfg.action.changeOutOfBoundDestination.change) << 16
        data += self.getByteArray(val, SIZE_UINT32)

        # changeInBoundTOS
        val = 0
        if hasattr(ruleCfg.action, 'changeInBoundTOS'):
            if (CFP_L3FRAMING_IPV6 == l3Framing) or (CFP_L3FRAMING_IPV4 == l3Framing):
                val = int(str(ruleCfg.action.changeInBoundTOS), 16)
                val += 0x40
        data += self.getByteArray(val, SIZE_UINT8)

        # changeOutOfBoundTOS
        val = 0
        if hasattr(ruleCfg.action, 'changeOutOfBoundTOS'):
            if (CFP_L3FRAMING_IPV6 == l3Framing) or (CFP_L3FRAMING_IPV4 == l3Framing):
                val = int(str(ruleCfg.action.changeOutOfBoundTOS), 16)
                val += 0x40
        data += self.getByteArray(val, SIZE_UINT8)

        # tcFlags
        val = 0
        if hasattr(ruleCfg.action, 'changeTC'):
            val = 8 + ruleCfg.action.changeTC
        data += self.getByteArray(val, SIZE_UINT8)

        # reasonCode
        val = 0
        if hasattr(ruleCfg.action, 'reasonCode'):
            val = int(str(ruleCfg.action.reasonCode), 16)
        data += self.getByteArray(val, SIZE_UINT8)

        # otherFlags
        val = 0
        if hasattr(ruleCfg.action, 'bypassVLAN'):
            if ruleCfg.action.bypassVLAN:
                val += 1
        if hasattr(ruleCfg.action, 'bypassEAP'):
            if ruleCfg.action.bypassEAP:
                val += 2
        if hasattr(ruleCfg.action, 'bypassSTP'):
            if ruleCfg.action.bypassSTP:
                val += 4
        if hasattr(ruleCfg.action, 'loopbackEn'):
            if ruleCfg.action.loopbackEn:
                val += 8
        if hasattr(ruleCfg.action, 'redDfltProfile'):
            if  ruleCfg.action.redDfltProfile:
                val += 16
        data += self.getByteArray(val, SIZE_UINT8)

        # colorFlags
        val = 0
        if hasattr(ruleCfg.action, 'changeColor'):
            val = 1 + (self.getBinaryColor(ruleCfg.action.changeColor) << 1)
        data += self.getByteArray(val, SIZE_UINT8)

        # chainID
        data += self.getByteArray(0, SIZE_UINT8)

        # rowAndSlice
        data += self.getByteArray(row | (self.sliceNum) << 12, SIZE_UINT16)

        return data
    ##  @brief Generate CFP configuration binary data
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #cfp_config.CFP_CfgConfigurator object
    ##  @param[in]      cfg         CFP configuration object
    ##
    ##  @return         CFP configuration in binary format
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
    ##
    ##  @limitations None
    def getBinaryCFP_Cfg(self, cfg):
        data = bytearray()
        cnt = 0
        portBitmask = 0

        # magicId
        data += self.getByteArray(0x43504643, SIZE_UINT32)

        # ruleList
        if hasattr(cfg, 'nonIPRules'):
            self.sliceNum = CFP_MAX_SLICES-1
            self.curSliceUDFs = set()
            self.type = 'non-IP'
            for each in cfg.nonIPRules.rule:
                data += self.getBinaryRuleCfg(each, CFP_L3FRAMING_NONIP, cnt)
                portBitmask |=  int(str(each.key.ingressPortBitmap), 16)
                cnt += 1

        if hasattr(cfg, 'ipv4Rules'):
            self.sliceNum = CFP_MAX_SLICES-1
            self.curSliceUDFs = set()
            self.type = 'IPv4'
            for each in cfg.ipv4Rules.rule:
                data += self.getBinaryRuleCfg(each, CFP_L3FRAMING_IPV4, cnt)
                portBitmask |=  int(str(each.key.ingressPortBitmap), 16)
                cnt += 1

        if hasattr(cfg, 'ipv6Rules'):
            self.sliceNum = CFP_MAX_SLICES-1
            self.curSliceUDFs = set()
            self.type = 'IPv6'
            for each in cfg.ipv6Rules.rule:
                data += self.getBinaryRuleCfg(each, CFP_L3FRAMING_IPV6, cnt)
                portBitmask |=  int(str(each.key.ingressPortBitmap), 16)
                cnt += 1

        if cnt < CFP_MAX_RULES:
            data += bytearray(SIZE_RULECFGTYPE * (CFP_MAX_RULES - cnt))
        elif cnt > CFP_MAX_RULES:
            print("Error: Number of rules=", cnt, "exceeds allowed", CFP_MAX_RULES)
            sys.exit(1)

        # ruleListSz
        data += self.getByteArray(cnt, SIZE_UINT16)

        # pktLenCorr
        val = 0
        if hasattr(cfg, 'pktLenCorrection'):
            val = self.getBinaryPktLenCorr(cfg.pktLenCorrection)
        data += self.getByteArray(val, SIZE_UINT8)

        # portEnableMask
        if hasattr(cfg, 'portEnableMask'):
            data += self.getByteArray(int(str(cfg.portBitmask), 16), SIZE_UINT16)
        else:
            data += self.getByteArray(portBitmask, SIZE_UINT16)

        return data
    ##  @brief Generate Ethernet switch driver configuration binary
    ##
    ##  @behavior Sync, Re-entrant
    ##
    ##  @pre None
    ##
    ##  @param[in]      self        #ETHERSWT_CfgConfigurator object
    ##
    ##  @return     Ethernet switch driver configuration in binary format
    ##
    ##  @post None
    ##
    ##  @trace #BRCM_SWREQ_ETHERSWT_CFG_TYPE
    ##
    ##  @limitations None
    def generateBin(self, binfile):
        binData = bytearray()

        binData = self.getBinaryCFP_Cfg(self.cfg)
        fout = open(binfile, 'wb')
        fout.write(binData)
        fout.close()

##  @brief Print usage for CFP Configurator
##
##  @behavior Sync, Non-reentrant
##
##  @param          void
##
##  @return void
##
##  @post Application exits
##
##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
##
##  @limitations None
def usage():
    print """
usage: etherswt_cfp_config.py
    [-h, --help]    Display this help message
    -i, --xml       Input XML file
    -o, --out       Output binary file
    -s, --schema    Schema file
    -d, --debug     Enable debug information"""
    sys.exit(2)

##  @brief main function for CFP Configurator
##
##  @behavior Sync, Re-entrant
##
##  @pre TBD
##
##  @param[in]      void
##
##  @return void
##
##  @post TBD
##
##  @trace #BRCM_SWREQ_CFP_CONFIGURATOR
##
##  @limitations None
def main():
    XMLFile = None
    BinOutputFile = None
    SchemaFile = None
    debug = 0
    try:
        optlist, args = getopt.getopt(sys.argv[1:], 'hdi:o:s:',
                    ['help', 'debug', 'xml=', 'out=', 'schema='])
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
        elif opt in ('-d', '--debug'):
            debug = 1
            print '======== Interpret UDF as follows: (Base, Offset) ======== \n'
        else:
            print 'Unknown Argument {0}'.format(opt)
            usage()
    if ((XMLFile == None) or (BinOutputFile == None) or (SchemaFile == None)):
        print 'Missing arguments'
        usage()

    configurator = CFP_CfgConfigurator(XMLFile, SchemaFile)
    configurator.debug = debug
    configurator.generateBin(BinOutputFile)
    print BinOutputFile + ' is generated.'

if __name__ == '__main__':
    main()
## @}
