#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2018 Broadcom Limited.
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
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE IT OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

import sys
import struct
import getopt
import json
import jsonschema
from jsonschema import validate

def removeFileAndExit(fil):
    fil.seek(0)
    fil.truncate()
    fil.close()
    sys.exit(2)

def main(argv):
    try:
        opts, args = getopt.getopt(argv,"s:i:o:",["schema=","config=","outfile="])
    except getopt.GetoptError:
        print 'config.py -s <schema> -i <config> -o <outfile>'
        sys.exit(2)
    for [opt,arg] in opts:
        if opt in ("-s", "--schema"):
            schemafile = arg
        elif opt in ("-i", "--config"):
            configfile = arg
        elif opt in ("-o", "--outfile"):
            imgfile = arg
        else:
            print 'config.py -s <schema> -i <config> -o <outfile>'
            sys.exit(2)

    with open(schemafile, 'r') as myfile:
        schema=myfile.read().replace('\n', '')
    with open(configfile, 'r') as myfile:
        config=myfile.read().replace('\n', '')

    try:
        jsonschema.validate(json.loads(config), json.loads(schema))
    except jsonschema.ValidationError as e:
        print 'Validation error:' + e.message
        sys.exit(2)
    except jsonschema.SchemaError as e:
        print 'Schema error:' + e
        sys.exit(2)
    value = json.loads(config)
# Find out the number of interfaces existing in the config
    numInterfaces = len(value['dot1asCfg']['intfCfg'])

    fil = open(imgfile,'wb+')

# Populate the magic ID
    fil.write(struct.pack('I', 0x50545047))
    fil.write(struct.pack('I', value['dot1asCfg']['globalAdmin']))

    if value['dot1asCfg']['clockMode'] == 'gm':
        fil.write(struct.pack('I', 0))

    if value['dot1asCfg']['clockMode'] == 'slave':
        if numInterfaces > 1:
            fil.write(struct.pack('I', 1))
        else:
            fil.write(struct.pack('I', 2))

    fil.write(struct.pack('I', value['dot1asCfg']['avnuSyncAbsenceTimeout']))
    fil.write(struct.pack('I', value['dot1asCfg']['gmRateRatio']))

    numSlavePorts = 0
    numStackingPorts = 0
# Populate the per interface configuration for the existing interfaces
    for intf in range(0, numInterfaces):
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['hwPortNum']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['asCapable']))

        if value['dot1asCfg']['intfCfg'][intf]['role'] == 'master':
            fil.write(struct.pack('I', 0))
        elif value['dot1asCfg']['intfCfg'][intf]['role'] == 'slave':
            fil.write(struct.pack('I', 1))
            numSlavePorts += 1
# Validation: Do not allow multiple slave ports on slave device.
            if numSlavePorts != 1:
                print 'Multiple slave ports cannot exist'
                removeFileAndExit(fil)
        elif value['dot1asCfg']['intfCfg'][intf]['role'] == 'stacking':
            fil.write(struct.pack('I', 2))
            numStackingPorts += 1

# Validation: Do not allow slave ports on GM device.
# Validation: Do not allow Pdelay to be disabled on slave ports.
        if value['dot1asCfg']['intfCfg'][intf]['role'] == 'slave':
            if value['dot1asCfg']['intfCfg'][intf]['initLogPdelayInterval'] == 127 or value['dot1asCfg']['intfCfg'][intf]['operLogPdelayInterval'] == 127:
                print 'Invalid Pdelay interval for slave port', intf
                removeFileAndExit(fil)
            if value['dot1asCfg']['clockMode'] == 'gm':
                print 'Invalid ' + value['dot1asCfg']['intfCfg'][intf]['role'] + ' role for port', intf, 'in gm mode'
                removeFileAndExit(fil)

        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['initLogPdelayInterval']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['operLogPdelayInterval']))
        fil.write(struct.pack('i', value['dot1asCfg']['intfCfg'][intf]['initLogSyncInterval']))
        fil.write(struct.pack('i', value['dot1asCfg']['intfCfg'][intf]['operLogSyncInterval']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['nbrPropDelay']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['nbrRateRatio']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['syncReceiptTimeout']))
        fil.write(struct.pack('I', value['dot1asCfg']['intfCfg'][intf]['allowedLostResp']))

# Validation: Do not allow multiple slave ports on slave device.
    if (value['dot1asCfg']['clockMode'] == 'slave') and (numSlavePorts != 1) and (numStackingPorts == 0):
         print 'All ports configured as master in slave device'
         removeFileAndExit(fil)

    fil.close()
if __name__ == "__main__":
   main(sys.argv[1:])
