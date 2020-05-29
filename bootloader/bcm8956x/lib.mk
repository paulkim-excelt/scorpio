#
# Copyright 2018 Broadcom Limited.  All rights reserved.
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
# File Name: lib.mk
# DESCRIPTION: makefile to generate prebuild files required for build.
#####################################################################################

BRCM_BL_PREBUILT_DIR = $(BRCM_SDK_ROOT)/prebuilt/$(chip)/bootloader_lib/$(board)
BRCM_BL_LIB_TARGET_XCVR := $(TARGET_DIR)/lib/lib$(chip)_xcvr.a
BRCM_BL_LIB_XCVR_SRC := $(SRC_DIR)/bcm8956x/chip/common/bl_xcvr_cfg.c
BRCM_BL_LIB_TARGET := $(BRCM_BL_LIB_TARGET_XCVR)

BRCM_BL_LIB_XCVR_OBJS := ${BRCM_BL_LIB_XCVR_SRC:$(SRC_DIR)/%.c=$(BRCM_BL_OBJ_DIR)/%.o}

.PRECIOUS: $(BRCM_BL_OBJ_DIR)/%.o $(BRCM_BL_OBJ_DIR)/%.d

ifeq (,$(wildcard $(BRCM_BL_PREBUILT_DIR)/lib$(chip)_xcvr.a))
%xcvr.a: $(BRCM_BL_LIB_XCVR_OBJS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(AR) rcs $@ $(BRCM_BL_LIB_XCVR_OBJS)
else
%xcvr.a:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BL_PREBUILT_DIR)/lib$(chip)_xcvr.a $@
endif

$(BRCM_BL_OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(BRCM_BL_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) -c $(C_FLAGS) $(CCFLAGS) $(C_DEFS) $(INCS) $(BRCM_DEPFLAGS) -o $@ $<
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

$(BRCM_BL_OBJ_DIR)/%.d: ;

bootloader:

libs: $(BRCM_BL_LIB_TARGET)
	@echo "Lib $(BRCM_BL_LIB_TARGET) for $(chip) Compilation Successful!"


dependencies=$(subst .o,.d,$(OBJS))
ifneq ($(MAKECMDGOALS),bl_clean)
-include $(dependencies)
endif
