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
# File Name: hostapp.mk
# DESCRIPTION: Makefile for hostapps compilation
#####################################################################################

include common.mk

OS = LINUX
ifeq ($(CHIP_FAMILY),bcm8953x)
#HOST_TOOLCHAIN?=/projects/nwsoft-toolchains/brl_2.0/keystone_uclibc/bin/mips-broadcom-linux-uclibc-
TARGET = BCM8953X
else ifeq ($(CHIP_FAMILY),bcm8956x)
#HOST_TOOLCHAIN?=/local_tools/compiler/gcc-linaro-7.2.1-2017.11-i686_aarch64-linux-gnu/bin/aarch64-linux-gnu-
TARGET = BCM8956X
endif

$(eval $(call brcm_full_clear_vars))
include $($(comp)_config_mk)

CCFLAGS += -D__GNU__
CCFLAGS += -DGNU


ifeq ($(OS),LINUX)
CCFLAGS += -DLINUX
endif

BRCM_DEPFLAGS = -MMD -MF $(subst .o,.d_tmp,$@) -MP -MT $@

.PHONY: all clean help

BRCM_HOSTAPP_CFLAGS := $(BRCM_COMP_HOSTAPP_CFLAGS)
BRCM_HOSTAPP_CFLAGS += $(BRCM_CFLAGS)
BRCM_HOSTAPP_TARGET := $(call brcm_chip_hostapp_from_comp,$(comp))
$(info BRCM_HOSTAPP_TARGET: $(BRCM_HOSTAPP_TARGET))

BRCM_HOSTAPP_OBJS += $(sort $(BRCM_SRC:$(BRCM_COMP_DIR)/%.c=$(BRCM_HOSTAPP_OBJ_DIR)/%.o)\
       $(patsubst $(BRCM_COMP_DIR)/%.S, $(BRCM_HOSTAPP_OBJ_DIR)/%.o, $(BRCM_ASM_SRC)))

INCLUDES := $(addprefix -I,$(BRCM_INC))
#INCLUDES += -I$(dir $($(comp)_config_mk))/inc
INCLUDES += $(addprefix -I,$(dir $(BRCM_FULL_EXPORT_HDR))) $(addprefix -I,$(BRCM_CHIP_RDB_INC))
INCLUDES += $(addprefix -I,$(BRCM_HOSTAPP_INC))

.PRECIOUS: $(BRCM_HOSTAPP_OBJ_DIR)/%.o $(BRCM_HOSTAPP_OBJ_DIR)/%.d

make-depend = mv $1_tmp $1

$(BRCM_HOST_DIR)/bcm%: $(BRCM_HOSTAPP_OBJS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(HOST_TOOLCHAIN)gcc -o $@ $(BRCM_HOSTAPP_OBJS) -lrt
	$(QUIET)$(HOST_TOOLCHAIN)strip $@

$(BRCM_HOST_DIR)/eth_xcvr_ut: $(BRCM_HOSTAPP_OBJS)
	$(QUIET)$(HOST_TOOLCHAIN)gcc -lpthread -o $@ $(BRCM_HOSTAPP_OBJS)

$(BRCM_HOSTAPP_OBJ_DIR)/%.o : $(BRCM_COMP_DIR)/%.c $(BRCM_HOSTAPP_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(HOST_TOOLCHAIN)gcc -c -Werror -Wall $(CCFLAGS) $(BRCM_CFLAGS) $(INCLUDES) $(BRCM_DEPFLAGS) $< -o $@
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

$(BRCM_HOSTAPP_OBJ_DIR)/%.d: ;

all: $(BRCM_HOSTAPP_TARGET)
	@echo "Hostapp $(BRCM_COMP_NAME) for $(CHIP_FAMILY) Compilation Successful!"

clean:
	rm -rf $(BRCM_HOSTAPP_OBJS) $(BRCM_HOSTAPP_TARGET)

dependencies=$(subst .o,.d,$(BRCM_HOSTAPP_OBJS))
ifneq ($(MAKECMDGOALS),clean)
-include $(dependencies)
endif
