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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
################################################################################


##  @defgroup grp_console Debug Console
#   @ingroup grp_system_sw
#
#   @addtogroup grp_console
#   @{
#   Broadcom SDK provides debug console with
#       -# Logging
#       -# Profiling
#       -# UART/UDP Console
#       -# Debug IPC
#
#   @limitations None
#   @file system/console/comp.mk
#   @brief Makefile for Logging/Profiling/Debug Console
#   @version 0.1 Initial version
#   @}
#

BRCM_COMP_DISABLE_TRACE := TRUE

BRCM_COMP_LIB_SRC += lib/log.c
BRCM_COMP_LIB_SRC += lib/console.c

ifeq ($(ENABLE_ULOG), TRUE)
    BRCM_COMP_LIB_SRC +=  lib/ulog.c
    BRCM_COMP_LIB_CFLAGS += -DENABLE_ULOG
endif
BRCM_COMP_LIB_CFLAGS += -DENABLE_UART_CONSOLE

BRCM_COMP_APP_OPT += ENABLE_SHELL
BRCM_COMP_APP_OPT += ENABLE_UART_PL011
BRCM_COMP_APP_OPT += ENABLE_UART_CONSOLE
BRCM_COMP_APP_OPT += ENABLE_KLOG
BRCM_COMP_APP_OPT += ENABLE_ULOG


ifeq ($(call isappopt,ENABLE_UART0),yes)
BRCM_COMP_APP_INTR += UART0
UART0.function = UART_IRQHandler0
endif

ifeq ($(call isappopt,ENABLE_UART1),yes)
BRCM_COMP_APP_INTR += UART1
UART1.function = UART_IRQHandler1
endif

BRCM_COMP_APP_TASK += TaskShell
TaskShell.priority = 12
TaskShell.stack = 2048
TaskShell.event += SystemEvent0
TaskShell.event += SystemEvent1
TaskShell.event += SystemEvent2
ifeq ($(CHIP_FAMILY),bcm8910x)
TaskShell.event += SystemEvent3
endif
TaskShell.event += SystemEvent11

BRCM_COMP_APP_ALARM += Profile_Task CONSOLE_Alarm
Profile_Task.counter = SystemTimer
Profile_Task.callback = Profile_AlarmCb
Profile_Task.autostart = true
Profile_Task.alarmtime = 320
Profile_Task.cycletime = 320
CONSOLE_Alarm.counter = SystemTimer
CONSOLE_Alarm.callback = CONSOLE_AlarmCb
CONSOLE_Alarm.autostart = true
CONSOLE_Alarm.alarmtime = 4
CONSOLE_Alarm.cycletime = 4

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_COMP_APP_ALARM += CONSOLE_LogAlarm
CONSOLE_LogAlarm.counter = SystemTimer
CONSOLE_LogAlarm.callback = CONSOLE_MsysLogAlarmCb
CONSOLE_LogAlarm.autostart = true
CONSOLE_LogAlarm.alarmtime = 4
CONSOLE_LogAlarm.cycletime = 960
endif

BRCM_COMP_NAME := console
BRCM_COMP_DEPENDS := uartdrv
BRCM_COMP_TYPE := lib

