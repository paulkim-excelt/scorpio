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

img = None
final_img = None
ud_img = None
magic = 0
edc = 0
img_id = 0
pid = 0
sector_sz = 4 * 1024
img_id_shift = 11
edc_crc = 0x1
max_sz = 0

# *****************************************************************************

class ud_hdr_t:
    def __init__(self):
        self.magic_number = 0
        self.flags = 0
        self.img_max_size = 0
        self.img_size = 0
        self.err_det_code = 0
        self.rsvd1 = 0
        self.rsvd2 = 0
        self.rsvd3 = 0
    def write(self):
        ud_img.write(struct.pack('I', self.magic_number))
        ud_img.write(struct.pack('I', self.flags))
        ud_img.write(struct.pack('I', self.img_max_size))
        ud_img.write(struct.pack('I', self.img_size))
        ud_img.write(struct.pack('I', self.err_det_code))
        ud_img.write(struct.pack('I', self.rsvd1))
        ud_img.write(struct.pack('I', self.rsvd2))
        ud_img.write(struct.pack('I', self.rsvd3))

# *****************************************************************************

def append_img(img):
    with open(img, "rb") as image:
        ud_img.write(image.read())

# ******************************************************************************

def calculate_crc32(file_name):
    crc = 0xFFFFFFFF
    with open(file_name) as image:
        for line in image.read():
            crc = binascii.crc32(line, crc)
        image.close()
        return (crc & 0xFFFFFFFF)

# *****************************************************************************

def create_img():

    global img
    global max_sz
    global magic
    global final_img
    global ud_img
    global edc
    global img_id
    global sector_sz

    ud_hdr = ud_hdr_t()
    ud_img = open(final_img, "wb")
    img_stat = os.stat(img)

    if (not max_sz) or (max_sz < img_stat.st_size):
        max_sz = img_stat.st_size

    if edc_crc == edc:
        ud_hdr.err_det_code = calculate_crc32(img)
    ud_hdr.magic_number = magic
    ud_hdr.img_size = img_stat.st_size
    ud_hdr.img_max_size = ((max_sz + sector_sz - 1)//sector_sz) * sector_sz
    ud_hdr.flags = (img_id << img_id_shift) | edc
    ud_hdr.write()
    append_img(img)

    ud_img.close()

# *****************************************************************************

def validate_args():

    global img
    
    # Check whether the image exists
    if not os.path.isfile(img):
        print("%s file doesn't exist" % img)
        sys.exit(1)

# *****************************************************************************

def usage():
    print("./user_data_img_creator.py --img <img_name> --out <output file name> \
--edc <err det code> --id <img_id> --sector_sz <sector_sz> --magic <img magic> \
--max_sz <img max size>")

# *****************************************************************************

def main():

    global img
    global final_img
    global edc
    global img_id
    global sector_sz
    global max_sz
    global magic
    edc_input = 0

    try:
        opts, args = getopt.getopt(sys.argv[1:], "-h", ["img=", "magic=", "id=",
                                                        "max_sz=", "out=",
                                                        "edc=", "sector_sz="])

    except:
        print("Error in parsing arguments")
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt == "-h":
            usage()
            sys.exit(1)
        elif opt == "--img":
            img = arg
        elif opt == "--magic":
            magic = int(arg, 16)
        elif opt == "--id":
            img_id = int(arg, 16)
        elif opt == "--max_sz":
            max_sz = int(arg, 16)
        elif opt == "--out":
            final_img = arg
        elif opt == "--edc":
            edc = int(arg, 16)
            edc_input = 1
        elif opt == "--sector_sz":
            sector_sz = int(arg, 16)
        else:
            print("Unknown argument %s" % opt)
            sys.exit(1)

    if not img:
        print("--img required")
        usage()
        sys.exit(1)
    if not magic:
        print("--magic required")
        usage()
        sys.exit(1)
    if not img_id:
        print("--id required")
        usage()
        sys.exit(1)
    if not final_img:
        print("--out required")
        usage()
        sys.exit(1)
    if not edc_input:
        print("--edc required")
        usage()
        sys.exit(1)

    validate_args()

    create_img()

# *****************************************************************************

main()
