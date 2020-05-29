#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2017-2019 Broadcom Limited.
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
import ConfigParser

flash_sector_size = 4 * 1024
edc_crc = 0x1
userdata_start_addr_min = 1048576 # 1MB, 1024 * 1024 

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
class RSASign:
    def __init__(self, privkey):
        self.hash = SHA256.new()
        self.signature = None
        self.key = RSA.importKey(privkey)
        self.signer =  PKCS1_PSS.new(self.key)

    def hashUpdate(self, string):
        self.hash.update(string)

    def getHex(self):
        return self.hash.hexdigest()

    def getSignature(self):
        self.signature = self.signer.sign(self.hash)
        return self.signature

# *****************************************************************************
class DownloadImage:
    def __init__(self, infile, outdir):
        self.magic_number = 0x54415248 #'T''A''R''H'
        self.img_size = 0
        self.flags = 0
        self.err_det_code = 0
        self.input = infile
        self.outdir = outdir
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
        if (img_id == 0):
            output = open(self.outdir + '/' + os.path.splitext(os.path.basename(self.input))[0] + '.img', 'wb')
        else:
            output = open(self.outdir + '/' + '0x{:02X}'.format(pid) + '_' + '0x{:04X}'.format(img_id) + '.img', 'wb')
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

class PartitionTable:
    def __init__(self, flags, pt_load_addr):
        self.magic_number = 0x5054424C #'P''T''B''L'
        self.version = 0x1
        self.err_det_code = 0L
        self.flags = flags
        self.total_size = 0
        self.load_addr = pt_load_addr
        self.num_entries = 0
        self.hdr_err_det_code = 0
        self.entries = []
    class Entry:
        def __init__(self, pid, addr):
            self.flags = 0x6  # Dual_output | read_only partition | don't_skip
            self.addr = addr
            self.max_size = 0
            self.pid = pid
            self.reserved1 = 0
            self.reserved2 = 0
        def getData(self):
            data = bytearray()
            data += getByteArray(self.flags, 32) # Dual_output | read_only partition | don't_skip
            data += getByteArray(self.addr, 32)
            data += getByteArray(self.max_size, 32)
            data += getByteArray(self.pid, 16)
            data += getByteArray(self.reserved1, 16)
            data += getByteArray(self.reserved2, 32)
            return data
    def addEntry(self, pid, addr):
        self.entries.append(PartitionTable.Entry(pid, addr))
    def getData(self):
        data = bytearray()
        pt_hdr_struct_sz = 36
        pt_entry_struct_sz = 20
        self.num_entries = len(self.entries)
        self.total_size = pt_hdr_struct_sz + (self.num_entries * pt_entry_struct_sz)
        entry_data = bytearray()
        for entry in self.entries:
            entry_data += entry.getData()
        if self.flags == 1: #crc
            crc = 0xFFFFFFFF
            crc = binascii.crc32(entry_data, crc)
            self.err_det_code = crc & 0xFFFFFFFF
        data += getByteArray(self.magic_number, 32)
        data += getByteArray(self.version, 32)
        data += getByteArray(self.err_det_code, 64)
        data += getByteArray(self.flags, 32)
        data += getByteArray(self.total_size, 32)
        data += getByteArray(self.load_addr, 32)
        data += getByteArray(self.num_entries, 32)
        if self.flags == 1: #crc
            crc = 0xFFFFFFFF
            crc = binascii.crc32(data, crc)
            self.hdr_err_det_code = crc & 0xFFFFFFFF
        data += getByteArray(self.hdr_err_det_code, 32)
        data += entry_data
        #print binascii.hexlify(data)
        return data


# *****************************************************************************
class CfgItem:
    def __init__(self, id, maxSz, loadAddr, offset, file):
        self.id = id
        self.maxSz = maxSz
        self.file = file
        self.loadAddr = loadAddr
        self.offset = offset

class ImagePartition:
    def __init__(self, pid, flags, entry, sign, num_configs):
        self.magic_number = 0x494D4748 #'I''M''G''H'
        self.version = 0x1
        self.time_stamp = 0L
        self.img_version = 0x1
        self.err_det_code = 0L
        self.flags = flags
        self.entry_pt = entry
        self.num_imgs = 0
        self.hdr_err_det_code = 0
        self.pid = pid
        self.offset = flash_sector_size
        self.sign = sign
        self.signature = None
        self.cfgList = [CfgItem(0xFFFF, 0, 0, 0, None) for i in range(num_configs)]
        self.currCfgIdx = 0
    def addConfig(self, id, maxSz, loadAddr, file):
        maxSz = math.ceil(maxSz*1.0/flash_sector_size)*flash_sector_size
        self.cfgList[self.currCfgIdx] = CfgItem(id, maxSz, loadAddr, self.offset, file)
        self.offset += maxSz
        self.currCfgIdx += 1

    def addConfigs(self, file, outdir, repo):
        if os.path.isfile(file):
            with open(file) as config:
                for line in config:
                    entry = line.split()
                    if len(entry) > 3:
                        entry[3] = int(entry[3], 16)
                    elif len(entry) > 2:
                        entry.extend([0])
                    elif len(entry) > 1:
                        entry.append([''])
                        entry.extend([0])
                    else:
                        entry.append(hex(flash_sector_size))
                        entry.append('')
                        entry.extend([0])
                    self.addConfig(int(entry[0], 16), int(entry[1], 16), entry[3], repo + '/' + entry[2])
        else:
            print("%s File doesn't exist" % file)
            sys.exit(1)
    def getSize(self):
        return self.offset
    def signAndAddFile(self, privKeyFile, outdir, tar, repo, cnt, config, cfg_section):

        #write the image header and data
        binFileName = outdir + '/' + format(self.pid, '02X') + '.bin'
        binFh = open(binFileName, 'wb')
        binFh.write(self.getData())

        if self.sign == True:
            rsaKey = open(privKeyFile,'rb')
            signer = RSASign(rsaKey.read())
            for cfg in self.cfgList:
                if cfg.file is not None:
                    data = ''
                    fh = open(cfg.file, 'rb')
                    data = fh.read()
                    while data != '':
                        signer.hashUpdate(data)
                        data = fh.read()
                    fh.close()
                    print("sign: pid: %x file:%s sha:%s" %(self.pid, os.path.basename(cfg.file), signer.getHex()))
            self.signature = signer.getSignature()
            binFh.write(self.signature)

        binFh.close()
        tar.add(binFileName, os.path.basename(binFileName))
        config.set(cfg_section, os.path.basename(binFileName)[:-4], os.path.basename(binFileName))

        for cfg in self.cfgList:
            if cfg.file is not None:
                size = os.stat(cfg.file).st_size
                if ((cfg.maxSz < size) and (cnt > 0)):
                    print("Size of %s[%d bytes] is greater than max size allocated[%d bytes]" % (cfg.file, size, cfg.maxSz))
                    sys.exit(1)
                reqname = format(self.pid, '02X') + '_' + format(cfg.id, '04X')
                ext = '.'+ (os.path.basename(cfg.file)).split('.')[-1]
                tar.add(cfg.file, reqname + '.bin')
                config.set(cfg_section, reqname, os.path.basename(cfg.file))
    def getData(self):
        data = bytearray()
        entry_data = bytearray()
        self.num_imgs = len(self.cfgList)
        for cfg in self.cfgList:
            entry_data += getByteArray(self.flags << 2, 32)
            entry_data += getByteArray(0, 64) #err_det_code
            entry_data += getByteArray(cfg.offset, 32)
            entry_data += getByteArray(0, 32) #actual_size
            entry_data += getByteArray(cfg.id, 16)
            entry_data += getByteArray(0, 16) #comp_type
            entry_data += getByteArray(cfg.loadAddr, 32)
            entry_data += getByteArray(cfg.maxSz, 32)
            entry_data += getByteArray(0, 32) #resvd2
        if self.flags == 1: #crc
            crc = 0xFFFFFFFF
            crc = binascii.crc32(entry_data, crc)
            self.err_det_code = crc & 0xFFFFFFFF
        data += getByteArray(self.magic_number, 32)
        data += getByteArray(self.version, 32)
        data += getByteArray(self.time_stamp, 64)
        data += getByteArray(self.img_version, 32)
        data += getByteArray(self.err_det_code, 64)
        data += getByteArray(self.flags, 32)
        data += getByteArray(self.entry_pt, 32)
        data += getByteArray(self.num_imgs, 32)
        if self.flags == 1: #crc
            crc = 0xFFFFFFFF
            crc = binascii.crc32(data, crc)
            self.hdr_err_det_code = crc & 0xFFFFFFFF
        data += getByteArray(self.hdr_err_det_code, 32)
        data += entry_data
        #print binascii.hexlify(data)
        return data

# *****************************************************************************

class UserDataImage:
    def __init__(self):
        self.magic_number = 0x55535244 # 'U''S''R''D'
        self.img_size = 0
        self.flags = 0
        self.err_det_code = 0
        self.rsvd1 = 0
        self.rsvd2 = 0
        self.rsvd3 = 0
    def appendImage(self, infile, output):
        with open(infile, "rb") as image:
            output.write(image.read())
            image.close()
    def writeImage(self, infile, output, edc):
        outdirfile = open(output, "wb")
        if edc_crc == edc:
            crc = 0xFFFFFFFF
            with open(infile) as image:
                for line in image.read():
                    crc = binascii.crc32(line, crc)
                image.close()
            self.err_det_code = (crc & 0xFFFFFFFF)
        img_stat = os.stat(infile)
        self.img_size = img_stat.st_size
        self.flags = edc
        outdirfile.write(getByteArray(self.magic_number, 32))
        outdirfile.write(getByteArray(self.img_size, 32))
        outdirfile.write(getByteArray(self.flags, 32))
        outdirfile.write(getByteArray(self.err_det_code, 64))
        outdirfile.write(getByteArray(self.rsvd1, 32))
        outdirfile.write(getByteArray(self.rsvd2, 32))
        outdirfile.write(getByteArray(self.rsvd3, 32))
        self.appendImage(infile, outdirfile)
        outdirfile.close

# ******************************************************************************

class PTCreator:
    def __init__(self):
        self.repo = None
        self.pt_base = None
        self.pla = None
        self.edc = 0
        self.bl_cnt = 0
        self.bl_cfg = None
        self.syscfg_cnt = 0
        self.sys_cfg = None
        self.fw_cnt = 0
        self.fw_cfg = None
        self.userdata = None
        self.pub_key = None
        self.pub_key_la = 0
        self.priv_key = None
        self.key_cnt = 0
        self.outdir = None
        self.bl_sign = False
        self.sign = False
        self.out = None
        self.flash_addr = 0
        self.partitionTable = None
        self.max_num_img_entries = 0
        self.userdata_start_addr_min = userdata_start_addr_min
    def create(self):
        self.partitionTable = PartitionTable(self.edc, self.pla)
        self.flash_addr = 0 #running pointer in flash address space
        #Account for Partition Table itself
        self.flash_addr = self.pt_base + flash_sector_size

        self.num_configs = self.max_num_img_entries

        blPartition = ImagePartition(0x1, self.edc, self.bl_entry_pt, self.bl_sign, 1)
        blPartition.addConfigs(self.bl_cfg, self.outdir, self.repo)
        for i in range(0,self.bl_cnt):
            self.partitionTable.addEntry(0x1 | ((i+1) << 8), self.flash_addr)
            self.flash_addr += blPartition.getSize()

        sysCfgPartition = ImagePartition(0x3, self.edc, 0, self.sign, self.num_configs )
        sysCfgPartition.addConfigs(self.sys_cfg, self.outdir, self.repo)
        for i in range(0,self.syscfg_cnt):
            self.partitionTable.addEntry(0x3 | ((i+1) << 8), self.flash_addr)
            self.flash_addr += sysCfgPartition.getSize()

        # Check whether the fw_cfg images exist
        if self.fw_cnt > 0:
            fwPartition = ImagePartition(0x5, self.edc, self.fw_entry_pt, self.sign, self.num_configs)
            fwPartition.addConfigs(self.fw_cfg, self.outdir, self.repo)
            for i in range(0,self.fw_cnt):
                self.partitionTable.addEntry(0x5 | ((i+1) << 8), self.flash_addr)
                self.flash_addr += fwPartition.getSize()

        # Check if flash_addr is beyond limit or not. If not, force the addr to boundary.
        if self.flash_addr < self.userdata_start_addr_min :
            self.flash_addr = self.userdata_start_addr_min

        # Check whether the user_data images exist
        if self.userdata != None:
            userDataPartition = ImagePartition(0x9, self.edc, 0, self.sign, self.num_configs)
            userDataPartition.addConfigs(self.userdata, self.outdir, self.repo)
            self.partitionTable.addEntry(0x9 | (1 << 8), self.flash_addr)
            self.flash_addr += userDataPartition.getSize()

        if self.sign == True:
            # create pub key partition and entry in PT table
            # Note: Public key partition is never signed and is looked
            # by ROM to verify the signature of the BL
            keyPartition = ImagePartition(0x8, self.edc, 0, False, self.num_configs)
            keyPartition.addConfig(0, 0x1000, self.pub_key_la, self.pub_key)
            for i in range(0, self.key_cnt):
                self.partitionTable.addEntry(0x8 | (i + 1) << 8, self.flash_addr)
                self.flash_addr += keyPartition.getSize()

        pt_img = open(self.outdir + "/" + self.out + "_pt.bin", "wb")
        pt_img.write(self.partitionTable.getData())
        pt_img.close()

        tar = tarfile.open(self.outdir + '/' + self.out + ".tar", "w")
        tar_file_link_name  = self.outdir + '/' + self.out + '.tar.cfg'
        tar_file_link = ConfigParser.RawConfigParser()
        tar_file_link_section = 'File_Links'
        tar_file_link.add_section(tar_file_link_section)

        tar.add(self.outdir + "/" + self.out + "_pt.bin", 'FF.bin')
        tar_file_link.set(tar_file_link_section, 'FF', self.out +'_pt.bin')

        blPartition.signAndAddFile(self.priv_key, self.outdir,  tar, self.repo, self.bl_cnt,tar_file_link, tar_file_link_section)
        sysCfgPartition.signAndAddFile(self.priv_key, self.outdir,  tar, self.repo, self.syscfg_cnt,tar_file_link, tar_file_link_section)
        if self.fw_cnt > 0:
            fwPartition.signAndAddFile(self.priv_key, self.outdir,  tar, self.repo, self.fw_cnt,tar_file_link, tar_file_link_section)
        if self.sign == True:
            keyPartition.signAndAddFile(self.priv_key, self.outdir,  tar, self.repo, self.key_cnt,tar_file_link, tar_file_link_section)

        if self.userdata != None:
            #userDataPartition
            binFileName = self.outdir + '/' + format(userDataPartition.pid, '02X') + '.bin'
            binFh = open(binFileName, 'wb')
            for i in range(0,1):
                binFh.write(userDataPartition.getData())
            binFh.close()
            tar.add(binFileName, os.path.basename(binFileName))
            tar_file_link.set(tar_file_link_section, os.path.basename(binFileName)[:-4], os.path.basename(binFileName))

        tar.close()

        with open(tar_file_link_name, 'wb') as configfile:
            tar_file_link.write(configfile)

        dwldImage = DownloadImage(self.outdir + '/' + self.out + ".tar", self.outdir)
        dwldImage.writeImage(0x0, 0, 0, self.edc)
#            if ((cfg.maxSz < size) and (self.bl_cnt > 0)):
        #userData = UserDataImage()
        #for data in userDataList:
        #    userData.writeImage(self.repo + '/' + data.file, self.outdir + '/' + str(data.id) + '.img', self.edc)
        #    tar.add(self.outdir + '/' + str(data.id) + '.img', format(user_data_pid, '02X') + '_' + format(data.id, '04X') + '.' + os.path.basename(data.file))


    def usage(self):
        print("./pt_creator.py --repo <repo root> --sector_sz <sector size> \
--pt_base <pt base> --pla <pt load addr> --edc <err det code> \
--bl_cnt <bl count> --bl_cfg <bl cfg file> --bl_entry_pt <bl entry point> \
--fw_cnt <fw count> --fw_cfg <fw cfg file> --fw_entry_pt <fw entry point> \
--user_data <user data> --bl_sign <sign bl> --pub_key <public key> \
--pub_key_la <public key load address> --priv_key <private key> \
--key_cnt <key count> --outdir <final output directory> --max_num_img_entries <max combined allowed size for images>")
    def parse(self, args):
        global flash_sector_size
        try:
            opts, args = getopt.getopt(sys.argv[1:], "-h",
                                        ["repo=", "sector_sz=", "pt_base=",
                                         "pla=", "edc=",
                                         "bl_cnt=", "bl_cfg=", "bl_entry_pt=",
                                         "fw_cnt=", "fw_cfg=","fw_entry_pt=",
                                         "syscfg_cnt=", "sys_cfg=",
                                         "userdata=", "pub_key=", "pub_key_la=",
                                         "priv_key=", "key_cnt=", "bl_sign=",
                                         "outdir=", "out=", "max_num_img_entries="])

        except:
            print("Error in parsing arguments")
            self.usage()
            sys.exit(2)

        for opt, arg in opts:
            if opt == "-h":
                self.usage()
                sys.exit(1)
            elif opt == "--repo":
                self.repo = arg
            elif opt == "--sector_sz":
                flash_sector_size = int(arg, 16)
            elif opt == "--pt_base":
                self.pt_base = int(arg, 16)
            elif opt == "--pla":
                self.pla = int(arg, 16)
            elif opt == "--bl_cnt":
                self.bl_cnt = int(arg, 16)
            elif opt == "--bl_cfg":
                self.bl_cfg = arg
            elif opt == "--bl_entry_pt":
                self.bl_entry_pt = int(arg, 16)
            elif opt == "--syscfg_cnt":
                self.syscfg_cnt = int(arg, 16)
            elif opt == "--sys_cfg":
                self.sys_cfg = arg
            elif opt == "--fw_cnt":
                self.fw_cnt = int(arg, 16)
            elif opt == "--fw_cfg":
                self.fw_cfg = arg
            elif opt == "--fw_entry_pt":
                self.fw_entry_pt = int(arg, 16)
            elif opt == "--userdata":
                self.userdata = arg
            elif opt == "--pub_key":
                self.pub_key = arg
                self.sign = True
            elif opt == "--pub_key_la":
                self.pub_key_la = int(arg, 16)
            elif opt == "--priv_key":
                self.priv_key = arg
            elif opt == "--key_cnt":
                self.key_cnt = int(arg, 16)
            elif opt == "--bl_sign":
                if int(arg, 16) != 0:
                    self.bl_sign = True
            elif opt == "--outdir":
                self.outdir = arg
            elif opt == "--out":
                self.out = arg
            elif opt == "--edc":
                self.edc = int(arg, 16)
            elif opt == "--max_num_img_entries":
                self.max_num_img_entries = int(arg)
            else:
                print("Unknown argument %s" % opt)
                sys.exit(1)

    def validate(self):
        if ((self.repo == None) or (self.pt_base == None) or (self.pla == None) or (self.outdir == None) or (self.out == None)
            or (self.bl_cfg == None) or (self.bl_entry_pt == None)
            or (self.syscfg_cnt == 0) or (self.sys_cfg == None)
            or ((self.pub_key != None) and (self.priv_key == None) and (key_cnt == 0))
            or ((self.pub_key == None) and (self.priv_key != None) and (key_cnt == 0))):
            self.usage()
            sys.exit(1)
        # Out of 11 entries in PT, 1 for Crash partition, 1 for userdata,
        # 2 for security key, 2 for bootloader
        # 2 for syscfg and  2 for fw
        if (self.bl_cnt + self.syscfg_cnt + self.fw_cnt) > (6):
            print("Total number images exceeds the limit")
            sys.exit(1)

# ******************************************************************************
ptCreator = PTCreator()
ptCreator.parse(sys.argv[1:])
ptCreator.validate()
ptCreator.create()
