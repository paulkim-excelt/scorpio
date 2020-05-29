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
from PyQt4.QtGui import *
from PyQt4.QtCore import pyqtSlot,SIGNAL,SLOT,QThread
from PyQt4.QtCore import *
from ctypes import *
from shutil import copyfile

import zlib
import struct
import binascii
import tarfile
import threading
import netifaces
import socket
import ctypes
import platform
import sys
import getopt
import Queue
import os
import shutil
import atexit
import zipfile
import tempfile
import time
import serial
import glob

from Crypto.PublicKey import RSA
from Crypto.Signature import PKCS1_PSS
from Crypto.Hash import SHA256
from Crypto import Random
import random

import datetime
# *****************************************************************************

fail_test_case_num = 0
read_test_case_num = 0
class RSASign:
    def __init__(self, privkey):
        self.hash = SHA256.new()
        self.signature = None
        self.key = privkey
        self.signer =  PKCS1_PSS.new(self.key)

    def hashUpdate(self, string):
        self.hash.update(string)

    def getSignature(self):
        self.signature = self.signer.sign(self.hash)
        return self.signature

class GenRSAData:
    def __init__(self):
        self.privKey = RSA.generate(2048,Random.new().read)
        self.pubKey = self.privKey.publickey()
        self.msg = bytearray.fromhex('{:0256x}'.format(random.getrandbits(128*8)))
        self.rsa = RSASign(self.privKey)
        self.rsa.hashUpdate(self.msg)
    def getSignature(self):
        return bytearray(self.rsa.getSignature())
    def getPubKey(self):
        return bytearray.fromhex('{:0512x}'.format(self.pubKey.n))
    def getMsg(self):
        return self.msg
    def getPrivKey(self):
        return bytearray.fromhex('{:0512x}'.format(self.privKey.n))
    def updateMsg(self):
        self.msg = bytearray.fromhex('{:0256x}'.format(random.getrandbits(128*8)))
        self.rsa = RSASign(self.privKey)
        self.rsa.hashUpdate(self.msg)

class RAMClient(QObject):
    def __init__(self):
        super(RAMClient, self).__init__()
        loader = ctypes.cdll
        if sys.platform.startswith('win'):
            library = os.environ['BRCM_FLASHER_SEGGER_PATH'] + '\jlinkarm.dll'
            stdcallLoader =  ctypes.windll
        elif sys.platform.startswith('linux'):
            library = os.environ['BRCM_FLASHER_SEGGER_PATH'] + '/libjlinkarm.so.6'
            stdcallLoader = ctypes.cdll
        else:
            print ('%s OS is not supported' % sys.platform)
        self.iJLink, self.iBackendInfo = loader.LoadLibrary(library), library
        self.iStdCalliJLink = stdcallLoader.LoadLibrary(library)
        self.iRegVal = ctypes.c_int16()
        self.iSeqNum = 0
    def openSession(self, dev = None):
        if dev != None:
            self.iJLink.JLINKARM_EMU_SelectByUSBSN(dev)
        self.iJLink.JLINKARM_Open()
    def closeSession(self):
        self.iJLink.JLINKARM_Close()
    def cleanUp(self):
        self.closeSession()
    def listDevices(self, trigger):
        if self.iJLink.JLINKARM_EMU_GetNumDevices() > 0:
            self.openSession()
            if (self.iJLink.JLINKARM_GetSN() >= 0):
                print str(self.iJLink.JLINKARM_GetSN())
                if trigger != None:
                    trigger.emit(str(self.iJLink.JLINKARM_GetSN()))
            self.closeSession()

    def findAddr(self, fname, txt):
        with open(fname) as searchfile:
            for line in searchfile:
                if txt in line:
                    addr = line.split()
                    if (addr[1] == txt):
                        searchfile.close()
                        return addr[0]

    def TestSetup(self, folder):
        bl_found = 0
        mapfile_found = 0
        timeout = 100
        print 'Configuring iJLink'
        self.iJLink.JLINKARM_SetSpeed(5000) # 5000 kHz
        self.iJLink.JLINKARM_CORE_Select(0x0E0100FF) # JLINK_CORE_CORTEX_M7
        self.iJLink.JLINKARM_TIF_Select(0) # JLINKARM_TIF_JTAG
        if self.iJLink.JLINKARM_Connect() >= 0:
            print 'JLink connected successfully'
        else:
            print 'JLink connection failed'
            return 0
        for file in os.listdir(folder):
            if file.endswith("erika.map"):
                mapfile_found = 1
                self.iMsgAddr = int(self.findAddr(folder+'/'+file, "RSATest_Msg"), 16)
                self.iKeyAddr = int(self.findAddr(folder+'/'+file, "RSATest_Key"), 16)
                self.iSigAddr = int(self.findAddr(folder+'/'+file, "RSATest_Sig"), 16)
                self.iMsgLenAddr = int(self.findAddr(folder+'/'+file, "RSATest_MsgLen"), 16)
                break
        return mapfile_found
    def sendByteArray(self,address,data):
        self.iJLink.JLINKARM_WriteMem.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]
        self.iJLink.JLINKARM_WriteMem.restype = ctypes.c_int
        mytype = ctypes.c_char * len(data)
        h = mytype.from_buffer(data)
        r = self.iJLink.JLINKARM_WriteMem(address, len(data), h)
        return r
    def updateKey(self):
        self.testDataGen = GenRSAData()
    def GenTestData(self,length,load_file=False,input_vec_dir='./'):
        #pause it
        self.iJLink.JLINKARM_Halt()
        if not load_file:
            self.testDataGen.updateMsg()
            signature = self.testDataGen.getSignature()
            pubKey = self.testDataGen.getPubKey()
            msg = self.testDataGen.getMsg()
            priv = self.testDataGen.getPrivKey()
        else:
            global read_test_case_num
            fp = open(input_vec_dir+'test_'+str(read_test_case_num)+'_sig.bin','rb')
            signature = bytearray(fp.read())
            fp.close()

            fp = open(input_vec_dir+'test_'+str(read_test_case_num)+'_pub.bin','rb')
            pubKey = bytearray(fp.read())
            fp.close()

            fp = open(input_vec_dir+'test_'+str(read_test_case_num)+'_msg.bin','rb')
            msg = bytearray(fp.read())
            fp.close()

            fp = open(input_vec_dir+'test_'+str(read_test_case_num)+'_priv.bin','rb')
            priv = bytearray(fp.read())
            fp.close()
            read_test_case_num += 1
        #send the signature
        res = self.sendByteArray(self.iSigAddr,signature)
        if res == 0:
            print 'send: signature failed '
        res = self.sendByteArray(self.iKeyAddr,pubKey)
        if res == 0:
            print 'send: pubkey failed '
        res = self.sendByteArray(self.iMsgAddr,msg)
        if res == 0:
            print 'send: msg failed '
        """
        #change message
        self.iRegVal.value = 0xFFFF;
        self.iJLink.JLINKARM_ReadMemU16(self.iMsgLenAddr, 1, byref(self.iRegVal), None)
        print (self.iRegVal.value)
        self.iJLink.JLINKARM_ReadMemU16(self.iMsgLenAddr + 2, 1, byref(self.iRegVal), None)
        print (self.iRegVal.value)
        """
        self.iJLink.JLINKARM_WriteU32(self.iMsgLenAddr, length)
        self.iJLink.JLINKARM_Go()
        return signature,pubKey,msg,priv
    def dumpSRAM(self):
        #pause it
        self.iJLink.JLINKARM_Halt()
        #get the 2MB SRAM
        count = (2*1024*1024)/4
        buftype = ctypes.c_uint32 * int(count)
        buf = buftype()
        self.iJLink.JLINKARM_ReadMemU32(0x20000000, count, buf, 0)
        self.iJLink.JLINKARM_Go()
        fp = open('sram.bin','wb')
        fp.write(bytearray(buf))
        fp.close()
    def reset(self):
        self.iJLink.JLINKARM_WriteU32(0xe000ed0c, 0x05fa0004)
        time.sleep(1)
        self.iRegVal.value = 0xFFFF;
        self.iJLink.JLINKARM_ReadMemU16(self.iMsgLenAddr, 1, byref(self.iRegVal), None)
        print (self.iRegVal.value)
        self.iJLink.JLINKARM_ReadMemU16(self.iMsgLenAddr + 2, 1, byref(self.iRegVal), None)
        print (self.iRegVal.value)


class SerialPort:
    def __init__(self):
        self.com_port = ''
        self.ser = None

    def connect(self, com_port, baudrate=115200):
        self.com_port = com_port
        self.ser = serial.Serial(com_port,
                          baudrate=baudrate,  # baudrate
                          bytesize=serial.EIGHTBITS,  # number of databits
                          parity=serial.PARITY_NONE,  # enable parity checking
                          stopbits=serial.STOPBITS_ONE,  # number of stopbits
                          timeout=10,  # set a timeout value, None for waiting forever
                          xonxoff=0,  # enable software flow control
                          rtscts=0,  # enable RTS/CTS flow control
                          )
        self.ser.isOpen()
        return self.ser

    def disconnect(self):
        self.ser.close()

    def write(self, buffer):
        self.ser.flushOutput()
        buffer += str('\r')
        self.ser.write(buffer.encode('utf-8'))
        st_time = time.time()
        while self.ser.out_waiting and time.time() - st_time < 10:
            time.sleep(0.01)

    def read_until_match(self, match):
        out = ''
        found = False
        while not found:
            out_local = ''
            while self.ser.inWaiting() > 0:
                try:
                    out_local += ((self.ser.read(1)).decode('utf-8', errors='ignore'))
                except:
                    print('Got exception %s' % sys.exc_info()[0])
            out += out_local
            if match in out:
                found = True
                break
        else:
            found = True
            out += match
        return found, out.strip()

    def read(self):
        out = ''
        while self.ser.inWaiting() > 0:
            out += ((self.ser.read(1)).decode('utf-8'))
        return out

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result

def Test_run(serPort):
    count = 0
    while count < 10:
        serPort.write('test at1')
        found,result = serPort.read_until_match('=>')
        if 'Started' in result:
            break
        else:
            if count == 0:
                print('send failed. Retrying')
            count +=1
    if count < 10:
        count = 0
        found = False
        while count < 100:
            serPort.write('getresult at1')
            found1,result = serPort.read_until_match('=>')
            if 'Pass' in result:
                found = True
                break
            elif 'Fail' in result:
                print 'verification failed'
                logfile = open('logfile.txt','a')
                logfile.write('\nverification failed')
                logfile.close()
                break
            else:
                count += 1
    else:
        count = -1
        print 'Send failed. exiting'
    return found,count

if __name__ == '__main__':
    try:
        os.environ["BRCM_FLASHER_SEGGER_PATH"]
    except KeyError:
        print "Please set the environment variable BRCM_FLASHER_SEGGER_PATH"
        sys.exit()

    if not os.path.isdir('./test_cases_rom'):
        os.mkdir('test_cases_rom')
    logfile = open('logfile.txt','w')
    logfile.write('%s\n\n'%datetime.datetime.now())
    logfile.close()
    jlinkClient = RAMClient()
    jlinkClient.listDevices(None)
    jlinkClient.TestSetup('../../../../out/app/bcm89103_evk/rsa_qt_obj/images',)
    ports = serial_ports()
    print 'Available ports'
    for i in range(len(ports)):
        print(str(i)+' : '+ports[i])
    print 'Enter port number to use'
    input = int(raw_input(">> "))
    serPort = SerialPort()
    ser = serPort.connect(serial_ports()[input])
    num_tests = 10
    if len(sys.argv) != 1:
        num_tests = int(sys.argv[1])
    start = time.time()
    pass_count = 0
    load_from_file = False
    save_all_tests = False
    save_sram = False
    msg_loop_count = 10000
    for i in range(num_tests):
        if i%msg_loop_count == 0:
            print 'running' + str(i)
            print 'Changing key'
            jlinkClient.updateKey()
        (sig,pub,msg,priv) = jlinkClient.GenTestData(128,load_from_file)
        if save_sram:
            jlinkClient.dumpSRAM()
        result,count = Test_run(serPort)
        if count != -1:
            save_anyway = False
            if result:
                pass_count += 1
            else:
                #save data
                print('test failed: '+ str(i))
                logfile = open('logfile.txt','a')
                logfile.write('\ntest failed: '+ str(i))
                logfile.write('\nsaved it as ' + str(fail_test_case_num) +' in test_cases_rom')
                logfile.close()
                save_anyway = True
            if ((not load_from_file) and (save_all_tests)) or save_anyway:
                fp = open('test_cases_rom/test_'+str(fail_test_case_num)+'_sig.bin','wb')
                fp.write(sig)
                fp.close()

                fp = open('test_cases_rom/test_'+str(fail_test_case_num)+'_pub.bin','wb')
                fp.write(pub)
                fp.close()

                fp = open('test_cases_rom/test_'+str(fail_test_case_num)+'_msg.bin','wb')
                fp.write(msg)
                fp.close()

                fp = open('test_cases_rom/test_'+str(fail_test_case_num)+'_priv.bin','wb')
                fp.write(priv)
                fp.close()
                fail_test_case_num += 1
        else:
            #reset
            jlinkClient.reset()
    print 'completed ' + str(num_tests) + ' tests'
    print 'pass count ' + str(pass_count)
    print 'fail count ' + str(num_tests - pass_count)
    print 'Time ' + str(time.time() - start)
    serPort.disconnect()

