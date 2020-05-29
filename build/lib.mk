#
# Copyright 2016-2019 Broadcom Limited.  All rights reserved.
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
# File Name: Makefile
# DESCRIPTION: Makefile to be invoked while running "make". This would do initial
#				pre-requisite which is required to build products and invoke
#				product makefile to build accordingly.
#####################################################################################

include common.mk

MAKEFLAGS += -j 8

ifeq ($(LIB_GNU_CC),1)
LIB_TYPE := gcc
include $(BRCM_SDK_ROOT)/build/compiler/gnu_cc.mk
else
ifeq ($(LIB_RVCT_CC),1)
$(info Building with ARMCC...)
LIB_TYPE := rvct
include $(BRCM_SDK_ROOT)/build/compiler/rvct_cc.mk
endif
endif

$(eval $(call brcm_full_clear_vars))
$(eval $(call brcm_chip_include_deps,$(comp),$(BRCM_CHIP_BASIC_DEPENDS)))

$(eval $(call brcm_comp_clear_vars))
include $($(comp)_config_mk)

BRCM_LIB_CFLAGS := $(BRCM_COMP_LIB_CFLAGS)
BRCM_LIB_CFLAGS += $(BRCM_CFLAGS)
BRCM_PREBUILT_DIR = $(BRCM_SDK_ROOT)/prebuilt/$(chip)/lib/$(LIB_TYPE)
BRCM_LIB_TARGET := $(call brcm_chip_lib_from_comp,$(comp))

.PHONY: all clean help

BRCM_LIB_OBJS := $(patsubst %.c,%.o,$(addprefix $(BRCM_LIB_OBJ_DIR)/,$(filter %.c,$(BRCM_COMP_LIB_SRC))))
BRCM_LIB_OBJS += $(patsubst %.S,%.o,$(addprefix $(BRCM_LIB_OBJ_DIR)/,$(filter %.S,$(BRCM_COMP_LIB_SRC))))
BRCM_LIB_OBJS += $(sort $(BRCM_SRC:$(BRCM_COMP_DIR)/%.c=$(BRCM_LIB_OBJ_DIR)/%.o)\
       $(patsubst $(BRCM_COMP_DIR)/%.S, $(BRCM_LIB_OBJ_DIR)/%.o, $(BRCM_ASM_SRC)))
BRCM_LIB_OBJS += $(BRCM_LIB_OBJ_DIR)/$(comp)_version.o

INCLUDES := $(addprefix -I,$(BRCM_INC))
INCLUDES += -I$(dir $($(comp)_config_mk))/inc
INCLUDES += $(addprefix -I,$(dir $(BRCM_FULL_EXPORT_HDR))) $(addprefix -I,$(dir $(BRCM_FULL_LIB_INC))) $(addprefix -I,$(BRCM_CHIP_RDB_INC))

ifeq ($(BRCM_COMP_LIB_SRC),)
INCLUDES += $(addprefix -I,$(DEFAULT_BRCM_INC))
endif

#Mark intermediate object files as PRECIOUS so that it is not deleted once lib.a is created.
.PRECIOUS: $(BRCM_LIB_OBJ_DIR)/%.o $(BRCM_LIB_OBJ_DIR)/%.d

make-depend = mv $1_tmp $1

ifeq (,$(wildcard $(BRCM_PREBUILT_DIR)/lib$(chip)_$(comp).a))
%.a: $(BRCM_LIB_OBJS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(AR) -rcs $(BRCM_LIB_TARGET) $(BRCM_LIB_OBJS)
else
%.a:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_PREBUILT_DIR)/lib$(chip)_$(comp).a $@
endif

$(BRCM_LIB_OBJ_DIR)/%.o : $(BRCM_COMP_DIR)/%.c $(BRCM_LIB_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) -c $(CCFLAGS) $(BRCM_LIB_CFLAGS) $(INCLUDES) $(BRCM_DEPFLAGS)  $< -o $@
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

$(BRCM_LIB_OBJ_DIR)/%.o : $(BRCM_COMP_DIR)/%.S $(BRCM_LIB_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) -c $(CCFLAGS) $(BRCM_LIB_CFLAGS) $(INCLUDES) $(BRCM_DEPFLAGS) $< -o $@
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

# Handling deleted dependency file.
# If dependency file exists nothing will be done since there’s no prerequisites for the dependency file.
# If it doesn’t exist, it will be marked out of date since it has an empty recipe,
# which will force the object target to be rebuilt (creating a new dependency file).
$(BRCM_LIB_OBJ_DIR)/%.d: ;

ifneq ($(wildcard $(BRCM_SDK_ROOT)/build/.git/.),)
$(BRCM_LIB_OBJ_DIR)/$(comp)_version.o :
	$(QUIET)sh $(BRCM_SDK_ROOT)/build/scripts/generate_version_file.sh $(BRCM_LIB_OBJ_DIR) $(comp)
	$(QUIET)$(eval REVISION=$(shell  cd $(BRCM_COMP_DIR) && git log -1 --pretty=format:"%h"))
	$(QUIET)$(eval REPO=$(shell  cd $(BRCM_COMP_DIR) && git remote -v  |sed -e 's/.*\///' |head -n 1|sed -e 's/ /_/g' -e 's/(.*)//'))
	$(QUIET)$(CC) -c $(CCFLAGS) $(BRCM_CFLAGS) -D__BRCM_REVISION__="\"$(comp)_$(REPO)$(REVISION)_$(LIB_TYPE)\"" $(INCLUDES) $(BRCM_LIB_OBJ_DIR)/$(comp)_version.c -o $@
else
include $(BRCM_BOARD_DIR)/../../sw_version.txt
$(BRCM_LIB_OBJ_DIR)/$(comp)_version.o :
	$(QUIET)sh $(BRCM_SDK_ROOT)/build/scripts/generate_version_file.sh $(BRCM_LIB_OBJ_DIR) $(comp)
	$(QUIET)$(CC) -c $(CCFLAGS) $(BRCM_CFLAGS) -D__BRCM_REVISION__="\"$(comp)_$(SW_IMAGE_VERSION)_$(LIB_TYPE)\"" $(INCLUDES) $(BRCM_LIB_OBJ_DIR)/$(comp)_version.c -o $@
endif

all: $(BRCM_LIB_TARGET)
	@echo "Lib $(BRCM_COMP_NAME) for $(chip) Compilation Successful!"

clean:
	rm -rf $(BRCM_LIB_OBJS) $(BRCM_LIB_TARGET)

dependencies=$(subst .o,.d,$(BRCM_LIB_OBJS))
ifneq ($(MAKECMDGOALS),clean)
-include $(dependencies)
endif
