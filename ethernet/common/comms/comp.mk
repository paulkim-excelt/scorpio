#
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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


##  @defchapter grp_comms_sw Communication Stack
#
#   @addtogroup grp_comms_sw
#   @{
#   This chapter describes communication stacks and components available for
#   Broadcom Automotive SDK, including
#       -# Network Interface (NIF)
#       -# Stream Shaping (FQTSS)
#       -# UDP/IP Stack (INET)
#       -# TFTP Client
#       -# AVB Stacks (1722 only)
#       -# RTP/RTCP
#       -# MPEG-TS
#       -# Ethernet Time Sync
#       -# Wireless LAN stack
#       -# Remote File Service (RFS)
#
#   @file ethernet/common/comms/comp.mk
#   @brief Makefile for Communication Stacks
#   @version 0.1 Initial version
#   @}
#
#

BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/comms_req.c
BRCM_CONFIGURATION_ITEM += inc/comms_stream.h
BRCM_CONFIGURATION_ITEM += tests/qualification/streams_qt/streams_qt.h

BRCM_COMP_APP_OPT += ENABLE_ETH
BRCM_COMP_APP_OPT += ENABLE_ETH_TIME
BRCM_COMP_APP_OPT += ENABLE_NIF
BRCM_COMP_APP_OPT += ENABLE_MDIO

BRCM_COMP_APP_INTR += ETH ETHTIME
ETH.function = ETHER_IRQHandler
ETHTIME.function = ETHER_1588_IRQHandler

ifeq ($(call isappopt,ENABLE_ETH_SWITCH),yes)
BRCM_COMP_APP_INTR += SWITCH_LINK0 SWITCH_LINK1 SWITCH_LINK2 SWITCH_LINK3
BRCM_COMP_APP_INTR += SWITCH_LINK4 SWITCH_LINK5 SWITCH_LINK6 SWITCH_LINK8
SWITCH_LINK0.function = ETHERSWT_Port0LinkIRQHandler
SWITCH_LINK1.function = ETHERSWT_Port1LinkIRQHandler
SWITCH_LINK2.function = ETHERSWT_Port2LinkIRQHandler
SWITCH_LINK3.function = ETHERSWT_Port3LinkIRQHandler
SWITCH_LINK4.function = ETHERSWT_Port4LinkIRQHandler
SWITCH_LINK5.function = ETHERSWT_Port5LinkIRQHandler
SWITCH_LINK6.function = ETHERSWT_Port6LinkIRQHandler
SWITCH_LINK8.function = ETHERSWT_Port8LinkIRQHandler
else
ETHXCVR.function = ETHXCVR_LinkIRQHandler0
endif

BRCM_COMP_APP_TASK += CommsTask
CommsTask.priority = 14
CommsTask.stack = 2048 #Ensure that atleast 2KB stack is allocated
CommsTask.event += SystemEvent0
CommsTask.event += SystemEvent1
CommsTask.event += SystemEvent2
CommsTask.event += SystemEvent3
CommsTask.event += SystemEvent4
CommsTask.event += SystemEvent5
CommsTask.event += SystemEvent6 #Used by Switch
CommsTask.event += SystemEvent7 #Used by Switch
CommsTask.event += SystemEvent8
CommsTask.event += SystemEvent9
CommsTask.event += SystemEvent10
CommsTask.event += SystemEvent11 # Subsystem Notifications to Comms Task(MsgQ)
CommsTask.event += SystemEvent12 # Stream monitoring timer
CommsTask.event += ShutdownEvent

ifeq ($(call isappopt,ENABLE_IPC),yes)
BRCM_COMP_APP_INTR += IPC
IPC.function = IPC_IRQChann0Handler
endif

ifeq ($(call isappopt,ENABLE_RPC),yes)
BRCM_COMP_APP_TASK += RPC_ServerTask
RPC_ServerTask.priority = 13
RPC_ServerTask.stack = 2048
RPC_ServerTask.event += SystemEvent0
RPC_ServerTask.event += SystemEvent1
RPC_ServerTask.event += SystemEvent2
RPC_ServerTask.event += SystemEvent3
RPC_ServerTask.event += SystemEvent4
RPC_ServerTask.event += SystemEvent6 # IPC loader event
RPC_ServerTask.event += SystemEvent7 # IPC command queue event-->
RPC_ServerTask.event += SystemEvent9
RPC_ServerTask.event += SystemEvent10
RPC_ServerTask.event += SystemEvent11
RPC_ServerTask.event += ShutdownEvent

ifeq ($(chip),bcm89564g)
ifneq ($(call isappopt,ENABLE_IPC_S2M_INTR),yes)
# If the slave to master interrupt is not enabled, timer is used
# in master to poll on the IPC messages from slave.
BRCM_COMP_APP_ALARM += RPC_Alarm
RPC_Alarm.counter = SystemTimer
RPC_Alarm.callback = RPC_AlarmCb
RPC_Alarm.autostart = true
RPC_Alarm.alarmtime = 1
RPC_Alarm.cycletime = 1
endif # ($(call isappopt,ENABLE_IPC_S2M_INTR),yes)
endif # ($(chip),bcm89564g)

BRCM_COMP_APP_ALARM += SwitchArlTimer
SwitchArlTimer.counter = SystemTimer
SwitchArlTimer.callback = COMMS_ARLRefreshAlarmCb
SwitchArlTimer.autostart = true
SwitchArlTimer.alarmtime = 100
SwitchArlTimer.cycletime = 320
endif

ifeq ($(call isappopt,ENABLE_ETH_SWITCH),yes)
BRCM_COMP_APP_ALARM += StreamMonitor
StreamMonitor.counter = SystemTimer
StreamMonitor.callback = COMMS_StreamMonitorAlarmCb
StreamMonitor.autostart = false
StreamMonitor.alarmtime = 0
StreamMonitor.cycletime = 0
endif # ($(call isappopt,ENABLE_ETH_SWITCH),yes)

BRCM_COMP_NAME := comms
BRCM_COMP_DEPENDS := mdiodrv
BRCM_COMP_TYPE := doc

