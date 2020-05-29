#
# Copyright 2016-2018 Broadcom Limited.  All rights reserved.
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
TOOLCHAIN_PATH?=/home/paulkim/work/toolchains/gcc-arm-none-eabi-4_9-2015q3
COMPILER_PREFIX=arm-none-eabi
TOOLCHAIN_VERSION=4.9.3
COMPILER_PATH = $(TOOLCHAIN_PATH)/bin/$(COMPILER_PREFIX)-

C_FLAGS += -Wall -Werror -fno-builtin -I$(TOOLCHAIN_PATH)/arm-none-eabi/include
C_FLAGS += -D__GNU__ -D__GCC__ -DGNU

# build function as sections and strip-off unused functions
C_FLAGS += -fdata-sections -ffunction-sections

ifeq ($(CPU), BL_CORTEX_M7)
C_FLAGS += -mcpu=cortex-m7 -mthumb -mthumb-interwork -std=gnu99
C_FLAGS += -D__CORTEX_MX__ -D__BL_CORTEX_M7__
LD_FLAGS += -L$(TOOLCHAIN_PATH)/lib/gcc/$(COMPILER_PREFIX)/$(TOOLCHAIN_VERSION)/armv7-m
LD_FLAGS += -L$(TOOLCHAIN_PATH)/$(COMPILER_PREFIX)/lib/thumb
LD_FLAGS += -gc-sections -static -nostdlib -EL
LD_LIBS += -lc -lnosys -lgcc
endif

ifeq ($(CPU), CORTEX_R4)
CCFLAGS += -mcpu=cortex-r4 -mthumb -mthumb-interwork -std=gnu99
C_FLAGS += -D__CORTEX_RX__ -D__CORTEX_R4__
LD_FLAGS += -L$(TOOLCHAIN_PATH)/lib/gcc/$(COMPILER_PREFIX)/$(TOOLCHAIN_VERSION)/armv7-ar/thumb
LD_FLAGS += -L$(TOOLCHAIN_PATH)/$(COMPILER_PREFIX)/lib/thumb
LD_FLAGS += -gc-sections -static -nostdlib -EL
LD_LIBS += -lc -lnosys -lgcc
endif

BRCM_DEPFLAGS = -MMD -MF $(subst .o,.d_tmp,$@) -MP -MT $@

ifneq ($(cust),1)
C_FLAGS += -ggdb
endif

CC = $(COMPILER_PATH)gcc
AR = $(COMPILER_PATH)ar
LD = $(COMPILER_PATH)ld
OBJCOPY = $(COMPILER_PATH)objcopy
OBJDUMP = $(COMPILER_PATH)objdump

ifdef DEBUG
C_FLAGS += -O0
else
C_FLAGS += -Os
endif
