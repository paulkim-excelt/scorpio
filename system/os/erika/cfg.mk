# /*****************************************************************************
# Copyright 2017-2019 Broadcom Limited.  All rights reserved.
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
# Build info variables
ifeq ($(HOST_MACHINE_TYPE), Linux)
SW_IMAGE_TYPE ?= Dev
BUILD_TIME	:= $(shell date +"%d-%b-%Y,%T%Z")
BUILDING_USER	:= $(shell whoami)
BUILD_SERVER	:= $(shell hostname)
COMPILER_INFO	:= $(COMPILER_PREFIX)-gcc
COMPILER_PATH_INFO := $(TOOLCHAIN_PATH)/bin/$(COMPILER_INFO)
COMPILER_VERSION:= $(shell $(COMPILER_PATH_INFO) -dumpversion)
endif

CFLAGS += -DSW_IMAGE_TYPE=\"$(SW_IMAGE_TYPE)\" -DSW_IMAGE_VERSION=\"$(SW_IMAGE_VERSION)\" -DBUILD_TIME=\"$(BUILD_TIME)\" -DBUILDING_USER=\"$(BUILDING_USER)\" -DBUILD_SERVER=\"$(BUILD_SERVER)\"
CFLAGS += -DEE_BUILD_INFO=\"Erika-$(comp)\ Ver:$(SW_IMAGE_TYPE)-$(SW_IMAGE_VERSION),$(BUILD_TIME)\" -DCOMPILER_VER_INFO=\"$(COMPILER_INFO)-$(COMPILER_VERSION)\"

ifneq ($(HOST_MACHINE_TYPE), Linux)
	@echo "Error: Only Linux host supported for building this SDK, use a Linux host."
endif

EE_SRCS += $(BRCM_SDK_ROOT)/system/common/os/common/sys_notify.c
CFLAGS += -DSYS_NOTIF_EVENT=SystemEvent6
EE_SRCS += $(BRCM_SDK_ROOT)/system/io/os/erika/serial_io_osil.c

CFLAGS += -DSPI0_IRQ_EVENT=SystemEvent6 -DSPI1_IRQ_EVENT=SystemEvent7 -DSPI2_IRQ_EVENT=SystemEvent8 -DSPI_IRQ_PROCESS_TASK=TaskSerialIO
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/io/os/erika/

ifeq ($(call iseeopt, ENABLE_VGCC_DISPLAY), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/system/display/os/erika/display.c
endif   # ENABLE_VGCC_DISPLAY

ifeq ($(call iseeopt, ENABLE_FLASH), yes)
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/include/osil
ifeq ($(call iseeopt, ENABLE_FLSMGR), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/system/nvm/os/erika/flsmgr_osil.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/nvm/lib/flsmgr.c
ifeq ($(call iseeopt, ENABLE_PTM), yes)
EE_SRCS += $(BRCM_SDK_ROOT)/system/nvm/os/erika/ptm_osil.c
EE_SRCS += $(BRCM_SDK_ROOT)/system/nvm/lib/ptm.c
CFLAGS += -DIMGL_MODE_FUNC_TABLE_PRIO0=PTM_ImageHandler
endif   # ENABLE_PTM
endif   # ENABLE_FLSMGR
endif   # ENABLE_FLASH

EE_SRCS += $(BRCM_SDK_ROOT)/system/imgl/os/common/imgl.c

CFLAGS += -DSYSTEM_MONITOR_ALARM_EVENT=SystemEvent11
