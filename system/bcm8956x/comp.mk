#
# Copyright 2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################


##  @defchapter grp_introduction Introduction
#
#   @addtogroup grp_introduction
#   @{
#   This document covers Requirements, Architecture, Design and Test specification for BCM8956X.
#   <BR>BCM8956X software architecture follows a modular and layered approach to address
#   -# portability across operating systems
#   -# reduced memory footprint for each use-case
#   -# Safety and Security at component and system level
#   -# Optimized power for each use-case
#   .
#
#   The word “Shall/Should” is used to indicate that the feature is mandatory.
#   <BR>The word “Will/Would” is used to indicate that the feature is mandatory to be implemented/followed.
#   <BR>The word “May/Might” is used to indicate that the feature is optional.
#
#   @}
#
#   @defchapter grp_bcm8956x_qsg Quick Start Guide
#
#   @addtogroup grp_bcm8956x_qsg
#   @{
#   This chapter describes how to build and run the ERIKA based release on
#   BCM8956X based EVK platforms. Details include
#       -# Build & Environment setup
#       -# Flashing and Debugging environment setup
#       -# Running and Debugging an application
#
#   @}
#
#   @defchapter grp_bcm8956x References & Examples
#
#   @addtogroup grp_bcm8956x
#   @{
#   This chapter describes
#       -# Example configurations
#       -# External references
#   @}
#
#   @defchapter grp_bcm8956x_init Initialization
#
#   @addtogroup grp_bcm8956x_init
#   @{
#   This chapter describes initialization sequence for BCM8953X EVKs including
#       -# Early Init
#           -# Cortex-M7 processor initialization including stack pointers setup,
#               MPU configuration, exception/interrupt vector installation etc
#           -# BSS initialization
#           -# Starting OS
#       -# Late Init
#           -# Idle Task creation (for WFI and profiling)
#           -# NVM subsystem initialization
#           -# Communication subsystem initialization
#
#   @file system/bcm8956x/comp.mk
#   @brief Makefile for BCM8956X Initialization
#   @version 0.1 Initial version
#   @}
#

BRCM_COMP_REQ_SRC += doc/bcm8956x_req.c
BRCM_COMP_REQ_SRC += doc/bcm8956x_rdb_req.c
BRCM_COMP_DOC_SRC += doc/introduction_page.c
BRCM_COMP_DOC_SRC += doc/qsg_page.c
BRCM_COMP_DOC_SRC += doc/board_page.c
BRCM_COMP_DOC_SRC += doc/references_page.c
BRCM_COMP_DOC_SRC += doc/example_netconf_page.c
BRCM_COMP_DOC_SRC += doc/architecture.vsd
#BRCM_COMP_DOC_SRC += os/docref/qsg.vsd
BRCM_COMP_DOC_SRC += doc/images/bcm8956x_arch_tcm_map.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8956x_arch_non_autosar.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8956x_arch_autosar.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm89561_evk_frontview.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm89561_evk_pcie_rework.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8956x_flash_layout.jpg
BRCM_COMP_DOC_SRC += doc/images/bcm8956x_arch_secure_boot.jpg

ifeq ($(board),bcm89561_evk)
BRCM_COMP_APP_OPT += ENABLE_FLASH0
else ifeq ($(board),bcm89564g_evk)
BRCM_COMP_APP_OPT += ENABLE_FLASH0
endif

BRCM_COMP_APP_OPT += __BCM8956X__
BRCM_COMP_APP_OPT += ENABLE_DBGMEM
BRCM_COMP_APP_OPT += ENABLE_MSG_QUEUE
BRCM_COMP_APP_OPT += ENABLE_OTP
BRCM_COMP_APP_OPT += ENABLE_GPIO_GIO_V1
BRCM_COMP_APP_OPT += ENABLE_PINMUX
BRCM_COMP_APP_OPT += ENABLE_TIMER_SP804
BRCM_COMP_APP_OPT += BCM_TIME_USES_SP804
BRCM_COMP_APP_OPT += ENABLE_HRTIMER
BRCM_COMP_APP_OPT += ENABLE_SYSTEM_MONITOR
BRCM_COMP_APP_OPT += ENABLE_WATCHDOG_SP805
BRCM_COMP_APP_OPT += ENABLE_THREAD_PROTECTION

BRCM_COMP_APP_OPT += ENABLE_UART0

BRCM_COMP_APP_OPT += ENABLE_RPC
BRCM_COMP_APP_OPT += ENABLE_IPC
BRCM_COMP_APP_OPT += IPC_MAX_CHANNELS=2

BRCM_COMP_APP_OPT += P1588_PORT_MASK=0x01FFUL
BRCM_COMP_APP_OPT += ETHXCVR_HW_ID_MAX=9UL
BRCM_COMP_APP_OPT += ETHER_TX_CHAN_CNT=1UL
BRCM_COMP_APP_OPT += ENABLE_ETH_AMAC
BRCM_COMP_APP_OPT += ENABLE_ETH_BRPHY
BRCM_COMP_APP_OPT += ENABLE_ETH_SGMII
BRCM_COMP_APP_OPT += ENABLE_ETH_SWITCH
BRCM_COMP_APP_OPT += ENABLE_CFP

BRCM_COMP_APP_INTR += TIMER0 TIMER1
TIMER0.function = TIM_IRQHandler0
TIMER1.function = TIM_IRQHandler1

BRCM_COMP_APP_INTR += GPIO1
GPIO1.function = GPIO_IRQ1Handler

BRCM_COMP_APP_EVENT += SystemEvent0
BRCM_COMP_APP_EVENT += SystemEvent1
BRCM_COMP_APP_EVENT += SystemEvent2
BRCM_COMP_APP_EVENT += SystemEvent3
BRCM_COMP_APP_EVENT += SystemEvent4
BRCM_COMP_APP_EVENT += SystemEvent5
BRCM_COMP_APP_EVENT += SystemEvent6
BRCM_COMP_APP_EVENT += SystemEvent7
BRCM_COMP_APP_EVENT += SystemEvent8
BRCM_COMP_APP_EVENT += SystemEvent9
BRCM_COMP_APP_EVENT += SystemEvent10
BRCM_COMP_APP_EVENT += SystemEvent11
BRCM_COMP_APP_EVENT += SystemEvent12
BRCM_COMP_APP_EVENT += SystemEvent13
BRCM_COMP_APP_EVENT += SystemEvent14
BRCM_COMP_APP_EVENT += ShutdownEvent

BRCM_COMP_APP_TASK += BCM8956X_SystemTask
BCM8956X_SystemTask.priority = 16
BCM8956X_SystemTask.autostart = true
BCM8956X_SystemTask.stack = 2048
BCM8956X_SystemTask.event += SystemEvent0
BCM8956X_SystemTask.event += SystemEvent1
BCM8956X_SystemTask.event += SystemEvent4
BCM8956X_SystemTask.event += SystemEvent6 # Subsystem Notifications to Sytem Task(MsgQ)
BCM8956X_SystemTask.event += SystemEvent10
BCM8956X_SystemTask.event += SystemEvent11

BRCM_COMP_APP_TASK += TaskSerialIO
TaskSerialIO.priority = 13
TaskSerialIO.stack = 2048
TaskSerialIO.event += SystemEvent0
TaskSerialIO.event += SystemEvent1
TaskSerialIO.event += SystemEvent2
TaskSerialIO.event += SystemEvent3
TaskSerialIO.event += SystemEvent4
TaskSerialIO.event += SystemEvent5
TaskSerialIO.event += SystemEvent6
TaskSerialIO.event += SystemEvent7
TaskSerialIO.event += SystemEvent8
TaskSerialIO.event += SystemEvent9
TaskSerialIO.event += SystemEvent10
TaskSerialIO.event += SystemEvent11
TaskSerialIO.event += SystemEvent12

BRCM_COMP_APP_COUNTER += SystemTimer
SystemTimer.mincycle = 1
SystemTimer.maxallowedvalue = 2147483647
SystemTimer.ticksperbase = 1
SystemTimer.secondspertick = 1

BRCM_COMP_APP_COUNTER += HRTimer
HRTimer.mincycle = 1
HRTimer.maxallowedvalue = 2147483647
HRTimer.ticksperbase = 1
HRTimer.secondspertick = 1

BRCM_COMP_APP_ALARM += SystemMonitorTimer
SystemMonitorTimer.counter = SystemTimer
SystemMonitorTimer.callback = SYS_NotfnAlarmCb
SystemMonitorTimer.autostart = true
SystemMonitorTimer.alarmtime = 320
SystemMonitorTimer.cycletime = 320

BRCM_COMP_APP_ALARM += SYS_REBOOT_Alarm
SYS_REBOOT_Alarm.counter = SystemTimer
SYS_REBOOT_Alarm.callback = SYS_REBOOT_AlarmCb
SYS_REBOOT_Alarm.autostart = false
SYS_REBOOT_Alarm.alarmtime = 0
SYS_REBOOT_Alarm.cycletime = 0


BRCM_COMP_NAME := bcm8956x
BRCM_COMP_DEPENDS := comms nvm console
BRCM_COMP_DEPENDS += mcudrv utils arm
BRCM_COMP_TYPE := doc

