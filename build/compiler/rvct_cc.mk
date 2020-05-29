#
# Copyright 2016 Broadcom Limited.  All rights reserved.
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
TOOLCHAIN_PATH := /tools/arm/DS-5_5.26.2/Linux-64/sw/ARMCompiler6.6
#TOOLCHAIN_PATH := /tools/arm/DS-5_5.28/Linux-64/sw/ARMCompiler6.9
#export USEARMLINUX64=1
#export RVDSVER=DS-5_5.25
COMPILER_PREFIX=arm
COMPILER_PATH = $(TOOLCHAIN_PATH)/bin/$(COMPILER_PREFIX)

CCFLAGS += --target=arm-arm-none-eabi -march=armv7-m -mcpu=cortex-m7 -mthumb  -fshort-enums -fno-builtin
CCFLAGS += -I$(TOOLCHAIN_PATH)/include -I$(TOOLCHAIN_PATH)/../gcc/arm-linux-gnueabihf/libc/usr/include
CCFLAGS += -fomit-frame-pointer -fno-unwind-tables
CCFLAGS += -D__ARMCC__ -DARMCC
CCFLAGS += -std=gnu11
CCFLAGS += -Oz
CCFLAGS += -fno-data-sections
CCFLAGS += -fdata-sections -ffunction-sections

ifeq ($(CPU),cortex-m7)
CCFLAGS += -D__CORTEX_MX__ -D__CORTEX_M7__
ifeq ($(ENABLE_FPU),TRUE)
CCFLAGS += -mfpu=fpv5-sp-d16 -mfloat-abi=softfp
endif
endif

BRCM_DEPFLAGS = -MMD -MF $(subst .o,.d_tmp,$@) -MP -MT $@

ifneq ($(cust),1)
CCFLAGS += -g
endif

CC = $(COMPILER_PATH)clang
AR = $(COMPILER_PATH)ar
LD = $(COMPILER_PATH)link
ARFLAGS = -rcs
#Disable DEBUG build option
#ifdef DEBUG
#CCFLAGS += -O0
#else
#CCFLAGS += -Ospace
#endif

