# /*****************************************************************************
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
#******************************************************************************/

ifeq ($(call iseeopt, ENABLE_INET), yes)
# Max number of fragments we expect an incoming datagram to consist 2^6 = 64
CFLAGS += -DINET_LOG2_MAX_REASS_FRAGS=6U
# Maximum size of reassembled datagram that we expect
CFLAGS += -DINET_MAX_REASS_BUF_SIZE=1600U
# Maximum duration in micro seconds to wait before freeing a reassembly buffer
# in case all fragments of a IP packet are not received. Currently a value
# of 50 milli seconds is configured i.e. 3125 microsecs * 16 = 50 ms
CFLAGS += -DINET_MAX_REASS_BUF_TIMEOUT=\(SYS_TICK_US*16UL\)
# Maximum duration in micro seconds to wait before freeing a reassembly buffer
# in case the notified client does not read the buffer. Currently a value
# of 6.125ms is configured i.e. 3125 microsecs * 2 = 6.125ms
CFLAGS += -DINET_MAX_CLIENT_READ_TIMEOUT=\(SYS_TICK_US*2UL\)
# The configured alarm interval for INET in microseconds in the INET OIL file
# <repo>/ethernet/tcp_ip/inet/os/erika/inet.oil
# Currently in the OIL file, the alarm is configured to 1 SYSTICK interval
CFLAGS += -DINET_ALARM_EVENT_INTERVAL=\(SYS_TICK_US*1UL\)
# Size of the ARP cache
CFLAGS += -DINET_MAX_ENTRIES_ARPCACHE=20U
# Lifetime of a resolved ARP address in the cache in seconds
CFLAGS += -DINET_ARP_COMPLETE_ENTRY_LIFETIME=7200UL
# Interval in seconds after which an ARP request needs to be resent
# in case of an un-resolved ARP address in the cache
CFLAGS += -DINET_ARP_RETRY_PERIOD=1UL
INCLUDE_PATH += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/inet/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/inet/inc/osil
EE_SRCS += $(BRCM_SDK_ROOT)/ethernet/common/nif/os/erika/inet_cfg.c
endif   # ENABLE_INET

ifeq ($(call iseeopt, ENABLE_NIF), yes)
ifneq ($(call iseeopt, ENABLE_ETHER_TEST), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/ethernet/common/nif/os/common/nif.c
endif # !ENABLE_ETHER_TEST
endif # ENABLE_NIF

ifeq ($(call iseeopt, ENABLE_WLAN), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/ethernet/common/nif/os/common/nif_wireless.c
endif # ENABLE_WLAN

ifeq ($(call iseeopt, ENABLE_NIF), yes)
ifeq ($(call iseeopt, ENABLE_ETH_SWITCH), yes)
ifneq ($(call iseeopt, ENABLE_ETHER_TEST), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/ethernet/common/nif/os/common/nif_switch.c
endif # !ENABLE_ETHER_TEST
endif   # ENABLE_ETH_SWITCH
endif # ENABLE_NIF
