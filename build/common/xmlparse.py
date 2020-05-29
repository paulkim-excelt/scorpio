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
import lxml
import math

from lxml import etree
from lxml import objectify


# ******************************************************************************
RTDRUID_CONFIGURATOR_NUMBER = 1278
license_txt = """/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/"""

#******************************************************************************
class CPU:
    def __init__(self):
        self.model = None
        self.family = None
        self.frequency = None
class CfgImage:
    def __init__(self, imgid, maxsz, file):
        self.imgid = int(imgid)
        self.maxsz = int(maxsz)
        self.file = str(file)
class CfgMode:
    def __init__(self, mode):
        self.mode = mode
        self.blCfg = []
        self.sysCfg = []
        self.appCfg = []
    def addCfg(self, pid, imgid, maxsz, file):
        if (pid == 1):
            if imgid not in [temp.imgid for temp in self.blCfg]:
                self.blCfg.append(CfgImage(imgid, maxsz, file))
            elif file != self.blCfg[[temp.imgid for temp in self.blCfg].index(imgid)].file:
                print '\n\nError: Same imgID with different filename'
                raise Exception
        elif (pid == 3):
            if imgid not in [temp.imgid for temp in self.sysCfg]:
                self.sysCfg.append(CfgImage(imgid, maxsz, file))
            elif file != self.sysCfg[[temp.imgid for temp in self.sysCfg].index(imgid)].file:
                print '\n\nError: Same imgID with different filename'
                raise Exception
        elif (pid == 5):
            if imgid not in [temp.imgid for temp in self.appCfg]:
                self.appCfg.append(CfgImage(imgid, maxsz, file))
            elif file != self.appCfg[[temp.imgid for temp in self.appCfg].index(imgid)].file:
                print '\n\nError: Same imgID with different filename'
                raise Exception
    def generateCfg(self, outdir):
        fp = open(outdir +'/' + 'app_cfg.txt','a')
        for each in self.appCfg:
            fp.write('0x' + format(each.imgid, '04X') +' 0x' + format(each.maxsz, '08X') + ' '+ each.file + '\n')
        fp.close()
        fp = open(outdir +'/' + 'bl_cfg.txt','a')
        for each in self.blCfg:
            fp.write('0x' + format(each.imgid, '04X') +' 0x' + format(each.maxsz, '08X') + ' '+ each.file + '\n')
        fp.close()
        fp = open(outdir +'/' + 'sys_cfg.txt','a')
        for each in self.sysCfg:
            fp.write('0x' + format(each.imgid, '04X') +' 0x' + format(each.maxsz, '08X') + ' '+ each.file + '\n')
        fp.close()
class AppCfg:
    def __init__(self):
        self.board = None
        self.mcu = None
        self.cpu = CPU()
        self.irq_stack = None
        self.compiler = None
        self.opt = ['__BCM__']
        self.userCfg = []
        self.modes = {}
    def addOpt(self, opt):
        if opt not in self.opt:
            self.opt.append(opt)
    def generateAppCfg(self, outdir):
        if 'default' in self.modes:
            self.modes['default'].generateCfg(outdir)
        iterator = list(set(self.modes.keys()) - set(['default']))
        for each in iterator:
            if 'default' in self.modes:
                self.modes['default'].generateCfg(outdir + '/' + each +'/')
            self.modes[each].generateCfg(outdir + '/' + each +'/')
    def addCfg(self, cfg):
        pid = int(cfg.attrib['pid'], 16)
        imgid = int(cfg.attrib['imgid'], 16)
        maxsz = int(cfg.attrib['maxsz'], 16)
        mode = 'default'
        if 'mode' in cfg.attrib:
            mode = cfg.attrib['mode']
        file = None
        if 'file' in cfg.attrib:
            file = str(cfg.attrib['file'])
        if mode not in self.modes:
            self.modes[mode] = CfgMode(mode)
        self.modes[mode].addCfg(pid, imgid, maxsz, file)
    def setIRQStack(self, stack):
        self.irq_stack = stack
    def addCPU(self, model, family, frequency):
        self.cpu.model = model
        self.cpu.family = family
        self.cpu.frequency = frequency
        self.addOpt('__'+model+'__')
        self.addOpt('__'+family+'__')
    def setBoard(self, board):
        self.board = board
        self.addOpt('__'+board+'__')
    def setMCU(self, mcu):
        self.mcu = mcu
        self.addOpt('__'+mcu+'__')
    def setCompiler(self, compiler):
        self.compiler = compiler
        self.addOpt('__'+compiler+'__')
#******************************************************************************
class WDS_SupervisionEntity:
    def __init__(self,wdsSE):
        self.name = str(wdsSE.attrib['name'])
        self.maxAllowedErrors = int(wdsSE.maxAllowedErrors)
        self.maxState = int(wdsSE.maxState)
        self.maxTransitionsPerState = int(wdsSE.maxTransitionsPerState)
        self.callback = None
        if hasattr(wdsSE, 'errorCallback'):
            self.callback = str(wdsSE.errorCallback)

class EETask:
    def __init__(self, task):
        self.name = str(task.attrib['name'])
        self.priority = int(task.priority)
        self.autostart = bool(task.autostart)
        self.stack = int(task.stack)
        self.group = 0
        self.is_ext = 0
        self.event = []
        self.wdsSE = {}
        self.groupID = 'BCM_GROUPID_INVALID'
        if hasattr(task, 'group'):
            self.groupID = str(task.group)
        if hasattr(task, 'event'):
            self.is_ext = 1
            for each in task.event:
                if each not in self.event:
                    self.event.append(str(each))
        if hasattr(task, 'wdsSE'):
            for each in task.wdsSE:
                self.wdsSE[str(each.attrib['name'])] = WDS_SupervisionEntity(each)

class EEInterrupt:
    def __init__(self, name, function):
        self.name = name
        self.function = function
class EECounter:
    def __init__(self, counter):
        self.name = str(counter.attrib['name'])
        self.mincycle = int(counter.mincycle)
        self.maxallowedvalue = int(counter.maxallowedvalue)
        self.ticksperbase = int(counter.ticksperbase)
        self.secondspertick = int(counter.secondspertick)
class EEAlarm:
    def __init__(self, alarm):
        self.name = str(alarm.attrib['name'])
        self.counter = str(alarm.attrib['counter'])
        self.callback = str(alarm.attrib['callback'])
        self.autostart = bool(alarm.autostart)
        self.alarmtime = int(alarm.alarmtime)
        self.cycletime = int(alarm.cycletime)
class EECfg(AppCfg):
    def __init__(self):
        AppCfg.__init__(self)
        self.group_max = 0
        self.status = None
        self.kernel = None
        self.startuphook = None
        self.pretaskhook = None
        self.posttaskhook = None
        self.event = []
        self.task = []
        self.interrupt = []
        self.counter = []
        self.alarm = []
        self.wdsSEcount = 0
        self.stackSizeDict = {128:'128', 256:'256', 512:'512', 1024:'1k', 2048:'2k', 4096:'4k', 8192:'8k', 16384:'16k'}
    def addEECfg(self, ee_cfg):
        self.status = str(ee_cfg.status)
        self.kernel = str(ee_cfg.kernel)
        self.startuphook = bool(ee_cfg.startuphook)
        self.pretaskhook = bool(ee_cfg.pretaskhook)
        self.posttaskhook = bool(ee_cfg.posttaskhook)
        self.event = []
    def addEvent(self, event):
        if event not in self.event:
            self.event.append(event)
    def addInterrupt(self, name, function):
        intr = EEInterrupt(name,function)
        self.interrupt.append(intr)
    def addCounter(self, counter):
        self.counter.append(counter)
    def addAlarm(self, alarm):
        self.alarm.append(alarm)
    def addTask(self, task):
        self.task.append(task)
    def generateEECfg(self, outdir):
        prio_list = []
        for each in self.task:
            if each.priority not in prio_list:
                prio_list.append(each.priority)
            self.wdsSEcount += len(each.wdsSE.keys())
        prio_list.sort()
        group = 0
        for each in prio_list:
            for task in self.task:
                if prio_list[group] == task.priority:
                    task.group = group
            self.group_max = group
            group += 1
        self.generateMakefile(outdir)
        self.generateHeader(outdir)
        self.generateSource(outdir)
    def generateStackSize(self, aStack):
        if aStack in self.stackSizeDict:
            return self.stackSizeDict[aStack]
        else:
            print ("Invalid Size: " + str(aStack))
            sys.exit(1)
    def generateMakefile(self, outdir):
        fp = open(outdir + '/makefile','w')
        fp.write('\nEEOPT += ' +'__'+self.board+ '__\n')
        fp.write('EEOPT += ' +'__'+self.mcu+ '__\n')
        fp.write('EEOPT += ' +'__'+self.compiler+ '__\n')
        fp.write('EEOPT += ' +'__'+self.cpu.model+ '__\n')
        fp.write('EEOPT += ' +'__'+self.cpu.family+ '__\n')
        fp.write('APPBASE := ..\n')
        fp.write('OUTBASE := Debug\n')
        fp.write('\n')
        fp.write('RTDRUID_CONFIGURATOR_NUMBER:=0x' + format(16384 + RTDRUID_CONFIGURATOR_NUMBER,'04x') + '\n')
        fp.write('\n')
        for each in self.opt:
            fp.write('EEOPT += ' + each + '\n')
        fp.write('EEOPT += __RTD_LINUX__\n')
        fp.write('EEOPT += __OO_ECC2__\n')
        fp.write('EEOPT += __OO_EXTENDED_STATUS__\n')
        if self.startuphook == True:
            fp.write('EEOPT += __OO_HAS_STARTUPHOOK__\n')
        if self.pretaskhook == True:
            fp.write('EEOPT += __OO_HAS_PRETASKHOOK__\n')
        if self.posttaskhook == True:
            fp.write('EEOPT += __OO_HAS_POSTTASKHOOK__\n')
        fp.write('EEOPT += __IRQ_STACK_NEEDED__\n')
        fp.write('EEOPT += __MULTI__\n')
        fp.write('EEOPT += __OO_HAS_USERESSCHEDULER__\n')
        fp.write('EEOPT += __OO_AUTOSTART_TASK__\n')
        fp.write('EEOPT += __OO_AUTOSTART_ALARM__\n')
        fp.write('EEOPT += __ALLOW_NESTED_IRQ__\n')
        fp.write('EEOPT += __DISABLE_EEOPT_DEFINES__\n')
        fp.write('EEBASE := ${ERIKA_FILES}\n')
        fp.write('ERIKA_FILES :=\n')
        fp.write('$(info Using erika files in $(EEBASE))\n')
        fp.write('-include $(EEBASE)/pkg/cfg/path_helper.mk\n')
        fp.write('APPBASE := ..\n')
        fp.write('OUTBASE := ' + outdir + '\n')
        fp.write('$(eval $(call check_and_set_linux_compiler_path,GNU_ARM_ROOT,$(TOOLCHAIN_PATH)))\n')
        fp.write('MCU_MODEL := ' + self.mcu + '\n')
        fp.write('CFLAGS  += -Wall -Werror -Wno-unused-function -ggdb\n')
        fp.write('APP_SRCS += eecfg.c\n')
        fp.write('include $(EEBASE)/pkg/cfg/rules.mk\n')
        fp.close()
    def generateHeader(self, outdir):
        fp = open(outdir + '/eecfg.h','w')
        fp.write(license_txt)
        fp.write('\n#ifndef RTDH_EECFG_H\n#define RTDH_EECFG_H\n\n')
        fp.write('#define RTDRUID_CONFIGURATOR_NUMBER (0x' + format(16384 + RTDRUID_CONFIGURATOR_NUMBER,'04x') + ')\n\n')
        fp.write('#include "wds.h"\n\n')
        fp.write('\n/* TASK definition */\n')
        fp.write('#define EE_MAX_TASK (' + str(len(self.task)) + ')\n')
        index = 0
        for value in self.task:
            fp.write('#define ' + value.name +' ' + str(index) + '\n')
            index += 1
        fp.write('\n/* MUTEX definition */\n')
        fp.write('#define EE_MAX_RESOURCE (1U)\n')
        fp.write('#define RES_SCHEDULER (0U)\n')
        fp.write('\n/* EVENT definition */\n')
        if (len(self.event) > 32):
            raise ValueError('Too many events declared')
        index = 1
        for value in self.event:
            fp.write('#define ' + value +' (0x' + format(index, '02X') + 'UL)\n')
            index = index << 1
        fp.write('\n/* ALARM definition */\n')
        fp.write('#define EE_MAX_ALARM (' + str(len(self.alarm)) + 'U)\n')
        index = 0
        for value in self.alarm:
            fp.write('#define ' + value.name +' ' + str(index) + '\n')
            index += 1
        fp.write('\n/* SCHEDULING TABLE definition */\n')
        fp.write('#define EE_MAX_SCHEDULETABLE (0U)\n')
        fp.write('\n/* COUNTER OBJECTS definition */\n')
        fp.write('#define EE_MAX_COUNTER_OBJECTS (EE_MAX_ALARM + EE_MAX_SCHEDULETABLE)\n')
        fp.write('\n/* CPU CLOCK definition */\n')
        fp.write('#define EE_CPU_CLOCK (' + str(self.cpu.frequency) +'UL)\n')
        fp.write('\n/* COUNTER definition */\n')
        fp.write('#define EE_MAX_COUNTER (' + str(len(self.counter)) + 'U)\n')
        index = 0
        for value in self.counter:
            fp.write('#define ' + value.name +' ' + str(index) + '\n')
            index += 1
        fp.write('\n/* APPMODE definition */\n')
        fp.write('#define EE_MAX_APPMODE (1U)\n')
        fp.write('\n/* CPUs */\n')
        fp.write('#define EE_MAX_CPU (1U)\n')
        fp.write('#define EE_CURRENTCPU (0U)\n')
        fp.write('\n#ifndef __DISABLE_EEOPT_DEFINES__\n')
        for each in self.opt:
            fp.write('#define '+ each + '\n')
        fp.write('#define __RTD_LINUX__\n')
        fp.write('#define __OO_ECC2__\n')
        fp.write('#define __OO_EXTENDED_STATUS__\n')
        if self.startuphook==True:
            fp.write('#define __OO_HAS_STARTUPHOOK__\n')
        if self.pretaskhook==True:
            fp.write('#define __OO_HAS_PRETASKHOOK__\n')
        if self.posttaskhook==True:
            fp.write('#define __OO_HAS_POSTTASKHOOK__\n')
        fp.write('#define __IRQ_STACK_NEEDED__\n')
        fp.write('#define __MULTI__\n')
        fp.write('#define __OO_HAS_USERESSCHEDULER__\n')
        fp.write('#define __OO_AUTOSTART_TASK__\n')
        fp.write('#define __OO_AUTOSTART_ALARM__\n')
        fp.write('#define __ALLOW_NESTED_IRQ__\n')
        fp.write('#endif')
        fp.write('\n/* Number of isr ' + str(len(self.interrupt)) + '*/\n')
        fp.write('#define EE_MAX_ISR2 (' + str(len(self.interrupt) + 1) + ')\n')
        fp.write('#define EE_MAX_ISR_ID (' + str(len(self.interrupt) + 1) + ')\n')
        fp.write('\n/* ISR definition */\n')
        for value in self.interrupt:
            fp.write('#define EE_' + self.cpu.family + '_' + value.name +'_ISR ' + value.function + '\n')
            if self.cpu.family == "CORTEX_RX":
                fp.write('#define EE_' + self.cpu.family + '_' + value.name + '_ISR_PRI ' + ' (EE_ISR_PRI_15)\n')
            else:
                fp.write('#define EE_' + self.cpu.family + '_' + value.name +'_ISR_PRI ' + ' (EE_ISR_PRI_1)\n')
        fp.write('\n\n/* HW counter definition */\n')
        fp.write('#define EE_MAX_COUNTER_HW (' + str(len(self.counter)) + ')\n')
        for each in self.counter:
            fp.write('#define OSMAXALLOWEDVALUE_' + each.name +' (' + str(each.maxallowedvalue) + 'U)\n')
            fp.write('#define OSTICKSPERBASE_' + each.name +' (' + str(each.ticksperbase) + 'U)\n')
            fp.write('#define OSMINCYCLE_' + each.name +' (' + str(each.mincycle) + 'U)\n')
        fp.write('\n/* Vector size defines */\n')
        fp.write('#define EE_ACTION_ROM_SIZE ('+ str(len(self.alarm)) +'U)\n')
        fp.write('#define EE_ALARM_ROM_SIZE ('+ str(len(self.alarm)) +'U)\n')
        if self.cpu.family == "CORTEX_MX":
            fp.write('#define EE_CORTEX_MX_SYSTEM_TOS_SIZE ('+ str(len(self.task) + 1) +'U)\n')
        elif self.cpu.family == "CORTEX_RX":
            fp.write('#define EE_CORTEX_RX_SYSTEM_TOS_SIZE ('+ str(len(self.task) + 1) +'U)\n')
        fp.write('#define EE_BRCM_SYSTEM_TOS_SIZE ('+ str(len(self.task)) +'U)\n')
        fp.write('#define EE_COUNTER_HW_ROM_SIZE ('+ str(len(self.counter)) +'U)\n')
        fp.write('#define EE_COUNTER_OBJECTS_ROM_SIZE ('+ str(len(self.alarm)) +'U)\n')
        fp.write('#define EE_COUNTER_ROM_SIZE ('+ str(len(self.counter)) +'U)\n')
        auto = 0
        for each in self.alarm:
            if each.autostart==True:
                auto += 1
        fp.write('#define EE_OO_AUTOSTART_ALARM_MODE_OSDEFAULTAPPMODE_SIZE ('+ str(auto) +'U)\n')
        auto = 0
        for each in self.task:
            if each.autostart==True:
                auto += 1
        fp.write('#define EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE ('+ str(auto) +'U)\n')
        fp.write('#define EE_RQ_PAIRS_NEXT_SIZE ('+ str(len(self.task)) +'U)\n')
        fp.write('#define EE_RQ_PAIRS_TID_SIZE ('+ str(len(self.task)) +'U)\n')
        fp.write('#define EE_RQ_QUEUES_HEAD_SIZE (16U)\n');
        fp.write('#define EE_RQ_QUEUES_TAIL_SIZE (16U)\n');
        if self.cpu.family == "CORTEX_RX":
            fp.write('#define EE_STD_SYSTEM_TOS_SIZE (' + str(len(self.task) + 1) + 'U)\n')
        fp.write('\n')
        fp.write('/* Functions */\n')
        for each in self.alarm:
            fp.write('void ' + each.callback +'(void);\n')
        fp.write('\n/* WDS SE Declarations */\n')
        counter = 0
        fp.write('extern const WDS_SEType WDS_SupervisionEntities[' + str(self.wdsSEcount) + '];\n')
        for each in self.task:
            for se in each.wdsSE.values():
                fp.write('#define ' +se.name + ' (WDS_SupervisionEntities[' + str(counter) + '])\n')
                counter += 1
        fp.write('#define WDS_SUPERVISION_ENTITY_CNT  ('+ str(counter) + 'UL)\n')
        for each in self.task:
            for se in each.wdsSE.values():
                if se.callback != None:
                    fp.write('void ' +se.callback + ' (const char* aName, int32_t aError, const WDS_MonitorType *aMonitor, uint32_t aMaxAllowedErrors);\n')
        fp.write('\n/* Task declarations */\n')
        for each in self.task:
            fp.write('void Func' + each.name + '(void);\n')
        fp.write('\n\nstruct BRCM_TaskInfoType {\n    char *name;\n    int *base;\n    int size;\n    int fillSize;\n    BCM_GroupIDType group;\n};\n')
        fp.write('\nextern const struct BRCM_TaskInfoType BRCM_TaskInfo[EE_BRCM_SYSTEM_TOS_SIZE];\n')
        fp.write('\n#endif /*RTDH_EECFG_H*/\n')
        fp.close()
    def generateSource(self, outdir):
        fp = open(outdir + '/eecfg.c','w')
        fp.write(license_txt)
        fp.write('\n#include "ee.h"\n')
        fp.write('#include <bcm_comp.h>\n')
        fp.write('#include <compiler.h>\n')
        fp.write('\n/* HW counter definition */\n')
        fp.write('const EE_oo_counter_hw_ROM_type EE_counter_hw_ROM[EE_COUNTER_HW_ROM_SIZE] = {\n')
        for each in self.counter:
            fp.write('{'+ str(each.secondspertick*1000000) +'U},  /*' + each.name +'*/\n')
        fp.write('};\n')
        fp.write('\n\n/* Stack definition for ' + self.cpu.family +' */\n')
        index = 1
        for each in self.task:
            fp.write('#define STACK_' + str(index) + '_SIZE ' + str(each.stack/4) + '   /* size = ' + str(each.stack) +' bytes */\n')
            index += 1
        if self.cpu.family == "CORTEX_RX":
            fp.write('#define STACK_' + str(index) + '_SIZE ' + str(self.irq_stack/4) + '   /* size = ' + str(self.irq_stack) +' bytes */\n')
        fp.write('\n')
        index = 0
        for each in self.task:
            fp.write('static int COMP_SECTION(".stack.' + self.generateStackSize(each.stack) + '.' + str(index) +'.'+each.name + '") EE_' + self.cpu.family.lower() + '_stack_' + str(index+1))
            fp.write('[STACK_'+ str(index+1) +'_SIZE]; ')
            fp.write('/* Task '+ str(index) +' (' + each.name +') */\n')
            index += 1
        if self.cpu.family == "CORTEX_RX":
            fp.write('static int COMP_SECTION(".stack.' + self.generateStackSize(self.irq_stack) + '.irq") EE_' + self.cpu.family.lower() + '_stack_'+ str(index+1))
            fp.write('[STACK_'+ str(index+1) +'_SIZE]; ')
            fp.write('/* irq stack */\n')
            fp.write('\n')
        index = 0
        fp.write('const EE_UREG EE_std_thread_tos[EE_MAX_TASK+1] = {\n')
        fp.write(str(index) + 'U,  /* dummy */\n')
        index += 1
        for each in self.task:
            fp.write(str(index) + 'U,  /* '+ each.name + '*/\n')
            index += 1
        fp.write('};\n')
        fp.write('\n')
        index = 1
        if self.cpu.family == "CORTEX_MX":
            fp.write('struct EE_TOS EE_cortex_mx_system_tos[EE_CORTEX_MX_SYSTEM_TOS_SIZE] COMP_SECTION(".data.os.EE_cortex_mx_system_tos") = {\n')
        elif self.cpu.family == "CORTEX_RX":
            fp.write('struct EE_TOS EE_std_system_tos[EE_STD_SYSTEM_TOS_SIZE]  COMP_SECTION(".data.os.EE_std_system_tos") = {\n')
        fp.write('{0},	/* Task   (dummy) */\n')
        for each in self.task:
            fp.write('{(EE_ADDR)(&EE_' + self.cpu.family.lower() + '_stack_' + str(index) +'[STACK_' + str(index) +'_SIZE - ' +\
                     self.cpu.family + '_INIT_TOS_OFFSET])},')
            fp.write('	/* Task ' + str(index-1) +' (' + each.name+ ') */\n')
            index += 1
        fp.write('};\n')
        fp.write('\n')
        if self.cpu.family == "CORTEX_MX":
            fp.write('EE_UREG EE_cortex_mx_active_tos COMP_SECTION(".data.os.EE_cortex_mx_active_tos") = 0U; /* dummy */\n')
        elif self.cpu.family == "CORTEX_RX":
            fp.write('EE_UREG EE_std_active_tos COMP_SECTION(".data.os.EE_std_active_tos") = 0U; /* dummy */\n')
        fp.write('\n')
        fp.write('/* stack used only by IRQ handlers */\n')
        if self.cpu.family == "CORTEX_RX":
            fp.write('struct EE_TOS EE_std_IRQ_tos COMP_SECTION(".data.os.EE_std_IRQ_tos") = {\n')
            fp.write('(EE_ADDR)(&EE_' + self.cpu.family.lower() + '_stack_' + str(len(self.task)+1) +'[STACK_' + str(len(self.task)+1) +'_SIZE])\n')
            fp.write('};\n')
        fp.write('\n')
        fp.write('const struct BRCM_TaskInfoType BRCM_TaskInfo[EE_BRCM_SYSTEM_TOS_SIZE] = {\n')
        index = 0
        for each in self.task:
            fp.write('{"' + str(index)+ ':' + each.name +'", ')
            fp.write('EE_' + self.cpu.family.lower() + '_stack_' + str(index+1) + ', ')
            fp.write('STACK_'+ str(index+1) +'_SIZE,')
            fp.write(' STACK_'+ str(index+1) +'_SIZE - ' + self.cpu.family + '_INIT_TOS_OFFSET, ')
            fp.write(str(each.groupID) + '},    ')
            fp.write('/* Task ' + str(index) + '(' + each.name +') */\n')
            index += 1
        fp.write('};\n')
        for each in self.task:
            for se in each.wdsSE.values():
                fp.write('WDS_ContextType ' +se.name + 'Context COMP_SECTION(".data.' + each.name+'");\n')
                fp.write('WDS_MonitorType ' +se.name + 'Monitor COMP_SECTION(".data.wds_monitor");\n')
                fp.write('WDS_StateType ' +se.name + 'GraphBuffer['+ str(se.maxState*se.maxTransitionsPerState)+'] COMP_SECTION(".data.' + each.name+'");\n')
        fp.write('const WDS_SEType WDS_SupervisionEntities[' + str(self.wdsSEcount) + '] = {\n')
        for each in self.task:
            for se in each.wdsSE.values():
                seCb = 'NULL'
                if se.callback != None:
                    seCb = se.callback
                fp.write('{0x57445330UL, "'+se.name+'", &'+se.name + 'Context, &' + se.name + 'Monitor, ' + se.name + 'GraphBuffer, '
                                + seCb + ' , ' + str(se.maxAllowedErrors) + ', ' + str(se.maxState) + ', ' + str(se.maxTransitionsPerState) + '},\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('\n')
        fp.write('const EE_THREAD_PTR EE_hal_thread_body[EE_MAX_TASK] = {\n')
        for each in self.task:
            fp.write('&EE_oo_thread_stub,		 /* thread ' + each.name +' */\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('EE_UINT32 EE_terminate_data[EE_MAX_TASK] COMP_SECTION(".data.os.EE_terminate_data");\n')
        fp.write('/* ip of each thread body (ROM) */\n')
        fp.write('const EE_THREAD_PTR EE_terminate_real_th_body[EE_MAX_TASK] = {\n')
        for each in self.task:
            fp.write('&Func'+each.name +',\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* ready priority */\n')
        fp.write('const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK] = {\n')
        for each in self.task:
            fp.write('0x' + format(1 << each.group, '04X')+'U,    /* thread ' + each.name +'*/\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK] = {\n')
        for each in self.task:
            fp.write('0x' + format(1 << each.group, '04X')+'U,    /* thread ' + each.name +'*/\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* thread status */\n')
        fp.write('EE_TYPESTATUS EE_th_status[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_status") = {\n')
        for each in self.task:
            fp.write('SUSPENDED,\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* next thread */')
        fp.write('EE_TID EE_th_next[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_next") = {\n')
        for each in self.task:
            fp.write('EE_NIL,\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* The first stacked task */\n')
        fp.write('EE_TID EE_stkfirst COMP_SECTION(".data.os.EE_stkfirst") = EE_NIL;\n')
        fp.write('/* system ceiling */\n')
        fp.write('EE_TYPEPRIO EE_sys_ceiling COMP_SECTION(".data.os.EE_sys_ceiling") = 0x0000U;\n')
        fp.write('/* The priority queues: (16 priorities maximum!) */\n')
        fp.write('EE_TYPEPAIR EE_rq_queues_head[EE_RQ_QUEUES_HEAD_SIZE] COMP_SECTION(".data.os.EE_rq_queues_head") =\n')
        fp.write('{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};\n')
        fp.write('EE_TYPEPAIR EE_rq_queues_tail[EE_RQ_QUEUES_TAIL_SIZE] COMP_SECTION(".data.os.EE_rq_queues_tail") =\n')
        fp.write('{ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};\n')
        fp.write('EE_TYPE_RQ_MASK EE_rq_bitmask COMP_SECTION(".data.os.EE_rq_bitmask") = 0U;\n')
        fp.write('\n')
        fp.write('/* remaining nact: init= maximum pending activations of a Task */\n')
        fp.write('EE_TYPENACT EE_th_rnact[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_rnact") = {\n')
        for each in self.task:
            fp.write('1U,    /* thread '+ each.name + '*/\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('const EE_TYPENACT EE_th_rnact_max[EE_MAX_TASK] = {\n')
        for each in self.task:
            fp.write('1U,    /* thread '+ each.name + '*/\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('EE_TYPEPRIO EE_rq_link[EE_MAX_TASK] COMP_SECTION(".data.os.EE_rq_link") =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write(str(each.group) + 'U, ')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* The pairs that are enqueued into the priority queues (' + str(len(self.task)) +' is the total number of task activations) */')
        fp.write('EE_TYPEPAIR EE_rq_pairs_next[EE_RQ_PAIRS_NEXT_SIZE] COMP_SECTION(".data.os.EE_rq_pairs_next") =\n')
        fp.write('{ ')
        for each in range(1,len(self.task)):
            fp.write(str(each) + ', ')
        fp.write(' -1};')
        fp.write('\n')
        fp.write('/* no need to be initialized */\n')
        fp.write('EE_TID EE_rq_pairs_tid[EE_RQ_PAIRS_TID_SIZE] COMP_SECTION(".data.os.EE_rq_pairs_tid");\n')
        fp.write('/* a list of unused pairs */\n')
        fp.write('EE_TYPEPAIR EE_rq_free COMP_SECTION(".data.os.EE_rq_free") = 0; /* pointer to a free pair */\n')
        fp.write('#ifndef __OO_NO_CHAINTASK__\n')
        fp.write('/* The next task to be activated after a ChainTask. initvalue=all EE_NIL */\n')
        fp.write('EE_TID EE_th_terminate_nextask[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_terminate_nextask") = {\n')
        for each in self.task:
            fp.write('EE_NIL,\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('#endif\n')
        fp.write('/* Event handling */\n')
        fp.write('EE_TYPEEVENTMASK EE_th_event_active[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_event_active") =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write('0U, ');
        fp.write(' };    /* thread event already active */\n')
        fp.write('EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_event_waitmask") =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write('0U, ');
        fp.write(' };    /* thread wait mask */\n')
        fp.write('EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_waswaiting") =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write('0U, ');
        fp.write(' };\n')
        fp.write('const EE_TYPEPRIO EE_th_is_extended[EE_MAX_TASK] =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write(str(each.is_ext) + 'U, ');
        fp.write(' };\n')
        fp.write('/*\n')
        fp.write(' * This is the last mutex that the task has locked. This array\n')
        fp.write(' *    contains one entry for each task. Initvalue= all -1. at runtime,\n')
        fp.write(' *    it points to the first item in the EE_resource_stack data structure.\n')
        fp.write('*/\n')
        fp.write('EE_UREG EE_th_resource_last[EE_MAX_TASK] COMP_SECTION(".data.os.EE_th_resource_last") =\n')
        fp.write('{ ')
        for each in self.task:
            fp.write('(EE_UREG) -1, ');
        fp.write(' };\n')
        fp.write('/* \n')
        fp.write(' * This array is used to store a list of resources locked by a\n')
        fp.write(' *    task. there is one entry for each resource, initvalue = -1. the\n')
        fp.write(' *    list of resources locked by a task is ended by -1.\n')
        fp.write(' */\n')
        fp.write('EE_UREG EE_resource_stack[EE_MAX_RESOURCE] COMP_SECTION(".data.os.EE_resource_stack") =\n')
        fp.write('{ (EE_UREG) -1};\n')
        fp.write('/*\n')
        fp.write(' * Only in extended status or when using ORTI with resources; for\n')
        fp.write(' *    each resource, a flag is allocated to see if the resource is locked or\n')
        fp.write(' *    not.  Note that this information cannot be easily knew from the previous\n')
        fp.write(' *    two data structures. initvalue=0\n')
        fp.write(' */\n')
        fp.write('EE_TYPEBOOL EE_resource_locked[EE_MAX_RESOURCE] COMP_SECTION(".data.os.EE_resource_locked") =\n')
        fp.write('{ 0U};\n')
        fp.write('const EE_TYPEPRIO EE_resource_ceiling[EE_MAX_RESOURCE]= {\n')
        fp.write('0x' + format(1 << self.group_max, '04X') + 'U 		/* resource RES_SCHEDULER */\n')
        fp.write('};\n')
        fp.write('EE_TYPEPRIO EE_resource_oldceiling[EE_MAX_RESOURCE] COMP_SECTION(".data.os.EE_resource_oldceiling");\n')
        fp.write('/* Counters */\n')
        fp.write('const EE_oo_counter_ROM_type EE_counter_ROM[EE_COUNTER_ROM_SIZE] = {\n')
        for each in self.counter:
            fp.write('{'+ str(each.maxallowedvalue) +'U, ' + str(each.ticksperbase) +'U, '+ str(each.mincycle) +'U}, ')
            fp.write('/* ' + each.name + ' */\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('EE_oo_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER] COMP_SECTION(".data.os.EE_counter_RAM") = {\n')
        for each in self.counter:
            fp.write('{0U, (EE_TYPECOUNTEROBJECT)-1},\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* Alarms */\n')
        fp.write('const EE_oo_alarm_ROM_type EE_alarm_ROM[EE_ALARM_ROM_SIZE] = {\n')
        index = 0
        for each in self.alarm:
            fp.write('{'+ str(index)+'U},\n')
            index += 1
        fp.write('};\n')
        fp.write('/* Counter Objects */\n')
        fp.write('const EE_oo_counter_object_ROM_type   EE_oo_counter_object_ROM[EE_COUNTER_OBJECTS_ROM_SIZE] = {\n')
        for each in self.alarm:
            fp.write('{' + each.counter +', '+ each.name +', EE_ALARM },\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('EE_oo_counter_object_RAM_type EE_oo_counter_object_RAM[EE_COUNTER_OBJECTS_ROM_SIZE] COMP_SECTION(".data.os.EE_oo_counter_object_RAM");\n')
        fp.write('/* Alarms and Scheduling Tables actions */\n')
        fp.write('const EE_oo_action_ROM_type   EE_oo_action_ROM[EE_ACTION_ROM_SIZE] = {\n')
        for each in self.alarm:
            fp.write('{EE_ACTION_CALLBACK, 0, 0U, '+ each.callback +', (EE_TYPECOUNTER)-1 },\n')
        fp.write('};\n')
        fp.write('\n')
        fp.write('/* AppMode */\n')
        fp.write('EE_TYPEAPPMODE EE_ApplicationMode COMP_SECTION(".data.os.EE_ApplicationMode");\n')
        fp.write('/* Auto Start (TASK) */\n')
        fp.write('static const EE_TID EE_oo_autostart_task_mode_OSDEFAULTAPPMODE[EE_OO_AUTOSTART_TASK_MODE_OSDEFAULTAPPMODE_SIZE] = \n')
        fp.write('{ ')
        count = 0
        for each in self.task:
            if each.autostart == True:
                fp.write(each.name +', ')
                count += 1
        fp.write(' };\n')
        fp.write('\n')
        fp.write('const struct EE_oo_autostart_task_type EE_oo_autostart_task_data[EE_MAX_APPMODE] = {\n')
        fp.write('{ ' + str(count)+'U, EE_oo_autostart_task_mode_OSDEFAULTAPPMODE}\n')
        fp.write(' };\n')
        fp.write('\n')
        fp.write('/* Auto Start (ALARM) */\n')
        fp.write('static const EE_TYPEALARM EE_oo_autostart_alarm_mode_OSDEFAULTAPPMODE[EE_OO_AUTOSTART_ALARM_MODE_OSDEFAULTAPPMODE_SIZE] =\n')
        fp.write('{ ')
        count = 0
        for each in self.alarm:
            if each.autostart == True:
                fp.write(each.name +', ')
                count += 1
        fp.write(' };\n')
        fp.write('\n')
        fp.write('const struct EE_oo_autostart_alarm_type EE_oo_autostart_alarm_data[EE_MAX_APPMODE] = {\n')
        fp.write('{ '+ str(count)+'U, EE_oo_autostart_alarm_mode_OSDEFAULTAPPMODE}\n')
        fp.write(' };\n')
        fp.write('\n')
        fp.write('/* Init alarms parameters (ALARM) */\n')
        fp.write('const EE_TYPETICK EE_oo_autostart_alarm_increment[EE_MAX_ALARM] =\n')
        fp.write('{ ')
        for each in self.alarm:
            fp.write(str(each.alarmtime) +'U, ')
        fp.write(' };\n')
        fp.write('\n')
        fp.write('const EE_TYPETICK EE_oo_autostart_alarm_cycle[EE_MAX_ALARM] =\n')
        fp.write('{ ')
        for each in self.alarm:
            fp.write(str(each.cycletime) +'U, ')
        fp.write(' };\n')
        fp.write('\n')
        fp.close()

#******************************************************************************

class XMLParser:
    def __init__(self, schemafile):
        self.repo = None
        self.dirs = []
        self.outdir = None
        self.schema = etree.XMLSchema(file = schemafile)
        self.parser = objectify.makeparser(schema = self.schema)
        self.loadedfiles = []
    def loadXML(self, file):
        fp = open(str(file),'r')
        xml = fp.read()
        fp.close
        brcm_cfg = objectify.fromstring(xml, self.parser)
        return brcm_cfg
    def processCompType(self, cfg, appCfg):
        if hasattr(cfg, 'include'):
            for each in cfg.include:
                file = repo + '/' + str(each)
                self.processCompXML(file, repo, appCfg)
        if hasattr(cfg, 'opt'):
            for each in cfg.opt:
                appCfg.addOpt(str(each))
        if hasattr(cfg, 'interrupt'):
            for each in cfg.interrupt:
                appCfg.addInterrupt(str(each.attrib['name']), str(each.attrib['function']))
        if hasattr(cfg, 'event'):
            for each in cfg.event:
                appCfg.addEvent(str(each))
        if hasattr(cfg, 'task'):
            for each in cfg.task:
                task = EETask(each)
                appCfg.addTask(task)
        if hasattr(cfg, 'counter'):
            for each in cfg.counter:
                cntr = EECounter(each)
                appCfg.addCounter(cntr)
        if hasattr(cfg, 'alarm'):
            for each in cfg.alarm:
                alarm = EEAlarm(each)
                appCfg.addAlarm(alarm)
        if hasattr(cfg, 'cfg'):
            for each in cfg.cfg:
                appCfg.addCfg(each)
        self.loadedfiles.extend(compfile)
    def processCompXML(self, compfile, repo, appCfg):
        if compfile in self.loadedfiles:
            return
        cfg = self.loadXML(compfile)
        self.processCompType(cfg.comp, appCfg)
        self.loadedfiles.extend(compfile)
    def processMCUType(self, mcu, appCfg):
        appCfg.setMCU(str(mcu.attrib['name']))
        appCfg.addOpt('__'+mcu.attrib['family']+'__')
        cpu = mcu.cpu
        appCfg.addCPU(str(cpu.attrib['model']),str(cpu.attrib['family']), int(cpu.frequency))
    def processApplicationXML(self, boardfile, repo, appCfg):
        cfg = self.loadXML(boardfile)
        appCfg.setBoard(str(cfg.app.board))
        self.processMCUType(cfg.app.mcu, appCfg)
        appCfg.setIRQStack(int(cfg.app.irq_stack))
        appCfg.setCompiler(str(cfg.app.compiler))
        appCfg.addEECfg(cfg.app.ee_cfg)
        self.processCompType(cfg.app, appCfg)

# ******************************************************************************
def usage():
    print("./xmlparse.py --repo <repo root> --dirs <dirs>  --input <input>")
    sys.exit(1)

compfile = None
boardfile = None
schemafile = None
dirs = None
repo = None
outdir = None
app_cfg = False
try:
    opts, args = getopt.getopt(sys.argv[1:], "hab:c:r:o:s:",
                                ["help=", "app=", "board=", "dirs=", "repo=", "out=", "schema="])
except:
    print("Error in parsing arguments")
    usage()
for opt, arg in opts:
    if opt in ("-h", "--help"):
        usage()
    elif opt in ("-a", "--app_cfg"):
        app_cfg = True
    elif opt in ("-c", "--comp"):
        compfile = arg
    elif opt in ("-b", "--board"):
        boardfile = arg
    elif opt in ("-r", "--repo"):
        repo = arg
    elif opt in ("-o", "--out"):
        outdir = arg
    elif opt in ("-s", "--schema"):
        schemafile = arg
    else:
        print("Unknown argument %s" % opt)
        usage()
#    if ((self.repo == None) or (self.sector_sz == None) or (self.pt_base == None) or (self.pla == None) or (self.out == None)
#        or (self.bl_cnt == None) or (self.bl_la == None) or (self.bl_entry_pt == None) or (self.bl_cfg == None)
#        or (self.fw_cnt == None) or (self.fw_la == None) or (self.fw_entry_pt == None) or (self.fw_cfg == None)):
#        self.usage()
#        sys.exit(1)
eeCfg = EECfg()
xmlParser = XMLParser(schemafile)
if os.path.isfile(boardfile) == True:
    xmlParser.processApplicationXML(boardfile, repo, eeCfg)
if os.path.isfile(compfile) == True:
    xmlParser.processCompXML(compfile, repo, eeCfg)
if app_cfg == False:
    eeCfg.generateEECfg(outdir)
else:
    eeCfg.generateAppCfg(outdir)
