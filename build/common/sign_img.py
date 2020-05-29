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
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

import sys
import getopt
import os
import struct
import binascii
import math
import tarfile
from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_PSS
from Crypto.Hash import SHA256

# *****************************************************************************
def signImage(privkey, infile, offset, outfile):
    hash = SHA256.new()
    keyFileh = open(privkey, "rb")
    key = RSA.importKey(keyFileh.read())
    keyFileh.close()
    signer =  PKCS1_PSS.new(key)
    infileh = open(infile, "rb")
    message = ''
    if offset > 0:
        message = infileh.seek(offset)
        print "signImage: ignored: %d" %offset

    message = infileh.read()
    while message != '':
        hash.update(message)
        message = infileh.read()
    signature = signer.sign(hash)
    infileh.close()

    infileh = open(infile, "rb")
    outfileh = open(outfile, 'wb')
    outfileh.write(signature)
    if offset > 0:
        message = infileh.seek(offset)

    message = infileh.read()
    while message != '':
        outfileh.write(message)
        message = infileh.read()
    infileh.close()
    outfileh.close()

# *****************************************************************************
def usage():
    print 'sign_img.py -i <input file> -o <output file> -f offset <offset in the input file> -k <private key file>'
    sys.exit(2)

try:
    opts, args = getopt.getopt(sys.argv[1:],"hi:o:k:f:")

except getopt.GetoptError:
    print 'exception'
    usage()

if len(opts) == 0:
    usage()
else:
    infile = None
    outfile = None
    keyfile = None
    offset = 0
    for opt, arg in opts:
        if opt == "-h":
            usage()
        elif opt == "-i":
            infile = arg
        elif opt == "-o":
            outfile = arg
        elif opt == "-k":
            keyfile = arg
        elif opt == "-f":
            offset = int(arg, 10)
        else:
            print("Unknown argument %s" % opt)
            usage()
    if ((infile != None) and (outfile != None) and (keyfile != None)):
        signImage(keyfile, infile, offset, outfile)
    else:
        usage()
