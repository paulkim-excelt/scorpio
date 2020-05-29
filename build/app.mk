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
# File Name: app.mk
# DESCRIPTION: Rules to build an application
#####################################################################################

include common.mk

isappopt = $(if $(filter $1,$(BRCM_FULL_APP_OPT)),yes,)

OS ?= erika

MAKEFLAGS += -j 8
$(eval $(call brcm_full_clear_vars))
$(eval $(call brcm_chip_include_deps,$(comp),$(CHIP_FAMILY)))

$(eval $(call brcm_comp_clear_vars))
include $($(comp)_config_mk)

ifeq ($(BRCM_APP_CFG_XML),)
BRCM_APP_CFG_XML := application
endif

BRCM_APP_TARGET := $(call brcm_board_app_from_comp,$(comp),$(OS))

BRCM_FULL_COMP_DEP_LIBS += $(DEFAULT_COMP_DEPENDENCIES)
BRCM_DEP_LIBS := $(call brcm_chip_lib_from_comp,$(BRCM_FULL_COMP_DEP_LIBS))

$(shell touch $(BRCM_SDK_ROOT)/system/utils/os/erika/build_info.c)

ifeq ($(OS),erika)
include $(BRCM_SDK_ROOT)/build/os/$(OS)_rules.mk
endif

include $(BRCM_SDK_ROOT)/build/$(CHIP_FAMILY)/board/postbuild.mk

%.a:
	$(QUIET)$(eval dep= $(subst lib$(chip)_,, $(shell basename $* )))
	$(QUIET)$(MAKE) comp=$(dep) all

prebuilds: $(BRCM_DEP_LIBS) $(BRCM_APP_PREBUILDS)

all: $(BRCM_APP_TARGET)
	@echo "App $(BRCM_COMP_NAME) for $(OS) Compilation Successful!"

clean:
	rm -rf $(BRCM_APP_DIR)/$(comp)_obj  $(BRCM_APP_TARGET) $(BRCM_APP_DIR)/app/$(board)_$(comp).*

