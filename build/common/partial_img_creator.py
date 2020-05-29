#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2017 Broadcom Limited.
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

import getopt
import sys
import os
import struct
import binascii

import tarfile
import shutil
import tempfile

edc_crc = 1

# *****************************************************************************

def getByteArray(entry, size):
    if size == 64:
        data = struct.pack('Q', entry)
    elif size == 16:
        data = struct.pack('H', entry)
    else:
        data = struct.pack('I', entry)
    return bytearray(data)

# *****************************************************************************
class DownloadImage:
    def __init__(self, infile, output):
        self.magic_number = 0x54415248 #'T''A''R''H'
        self.img_size = 0
        self.flags = 0
        self.err_det_code = 0
        self.input = infile
        self.outfile = output
    def appendImage(self, output):
        with open(self.input, "rb") as image:
            output.write(image.read())
            image.close()
    def writeImage(self, pid, copy_id, img_id, edc):
        flash_id = 0
        flash_id_shift = 2
        pid_shift = flash_id_shift + 1
        img_id_shift = pid_shift + 8
        redundant_copy_shift = img_id_shift + 16
        output = None
        output = open(self.outfile, 'wb')
        if edc_crc == edc:
            crc = 0xFFFFFFFF
            with open(self.input, 'rb') as image:
                for line in image.read():
                    crc = binascii.crc32(line, crc)
                image.close()
            self.err_det_code = (crc & 0xFFFFFFFF)
        img_stat = os.stat(self.input)
        self.img_size = img_stat.st_size
        self.flags = (copy_id << redundant_copy_shift) |(img_id << img_id_shift) | (pid << pid_shift) | (flash_id << flash_id_shift) | edc
        output.write(getByteArray(self.magic_number, 32))
        output.write(getByteArray(self.img_size, 32))
        output.write(getByteArray(self.flags, 32))
        output.write(getByteArray(self.err_det_code, 64))
        self.appendImage(output)

# *****************************************************************************

def usage():
    print("./partial_img_creator.py -i <img_name> -o <output_file_name> \
-e <err_det_code> -f <flash_id> -p <pid> -n <img_id>")

# *****************************************************************************

input = None
output = None
edc = None
flash = None
imgid = None
pid = None

try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:e:f:n:p:",
                            ["help=", "input=", "output=", "edc=", "flash=", "imgid=", "pid="])

except:
    print("Error in parsing arguments")
    usage()
    sys.exit(2)

for opt, arg in opts:
    if opt in ("-h", "--help"):
        usage()
        sys.exit(1)
    elif opt in ("-i", "--input"):
        input = arg
    elif opt in ("-o", "--output"):
        output = arg
    elif opt in ("-e", "--edc"):
        edc = int(arg, 16)
    elif opt in ("-f", "--flash"):
        flash = int(arg, 16)
    elif opt in ("-n", "--imgid"):
        imgid = int(arg, 16)
    elif opt in ("-p", "--pid"):
        pid = int(arg, 16)
    else:
        print("Unknown argument %s" % opt)
        sys.exit(1)

if input == None:
    print("-i required")
    usage()
    sys.exit(1)
if pid == None:
    print("-p required")
    usage()
    sys.exit(1)
if imgid == None:
    print("-n required")
    usage()
    sys.exit(1)
if output == None:
    print("-o required")
    usage()
    sys.exit(1)
if flash == None:
    print("-f required")
    usage()
    sys.exit(1)
if edc == None:
    print("-e required")
    usage()
    sys.exit(1)
if not os.path.isfile(input):
    print("%s file doesn't exist" % input)
    sys.exit(1)

file = tempfile.NamedTemporaryFile(delete=False)
print file
filename = file.name
file.close()
tar = tarfile.open(filename, "w")
tar.add(input, format(pid, '02X') + '_' + format(imgid, '04X') + '.' + os.path.basename(input))
tar.close()
dwldImage = DownloadImage(filename, output)
dwldImage.writeImage(0x0, 0, 0, edc)

