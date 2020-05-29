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
# File Name: main.mk
# DESCRIPTION: Makefile to be invoked while running "make". This would do initial
#				pre-requisite which is required to build products and invoke
#				product makefile to build accordingly.
#####################################################################################

include common.mk

ifneq ($(board),)
.PHONY: list_comp all clean prebuilds libs apps
ifeq ($($(board)_config_mk),)
$(error Unknown Board:$(board))
endif
MAKEFLAGS += -j 8

ifneq ($(comp),)
BRCM_BOARD_APPS_LIST := $(call brcm_board_app_from_comp,$(comp),$(OS))
else
BRCM_BOARD_APPS_LIST := $(call brcm_board_app_from_comp,$(BRCM_BOARD_ENABLED_APP),$(OS))
endif

BRCM_CHIP_LIBS :=
$(eval $(call brcm_calc_chip_libs,BRCM_CHIP_LIBS))
$(info BRCM_CHIP_LIBS: $(BRCM_CHIP_LIBS))
#BRCM_CHIP_HOSTAPPS :=
#$(eval $(call brcm_calc_chip_hostapps,BRCM_CHIP_HOSTAPPS))
#$(info BRCM_CHIP_HOSTAPPS: $(BRCM_CHIP_HOSTAPPS))

BRCM_CHIP_LIBS_LIST := $(call brcm_chip_lib_from_comp,$(BRCM_CHIP_LIBS))
#BRCM_CHIP_HOSTAPPS_LIST := $(call brcm_chip_hostapp_from_comp,$(BRCM_CHIP_HOSTAPPS))
#$(info BRCM_CHIP_HOSTAPPS_LIST: $(BRCM_CHIP_HOSTAPPS_LIST))

all: libs prebuilds apps
	@echo "Build for $(board) Completed"

list_comp:
	@echo "Supported Components: $(BRCM_COMP_LIST)"

clean:
	@echo "Cleaning for $(board)"
	rm -rf $(BRCM_APP_DIR) $(BRCM_APP_OBJ_DIR)

include $(BRCM_SDK_ROOT)/build/$(CHIP_FAMILY)/board/prebuild.mk

prebuilds: libs $(BRCM_APP_PREBUILDS)

libs: $(BRCM_CHIP_LIBS_LIST)

apps: $(BRCM_BOARD_APPS_LIST)

%.a: FORCE
	$(QUIET)$(MAKE) all -f lib.mk chip=$(chip) comp=$(call brcm_chip_comp_from_lib,$@) VERBOSE=$(VERBOSE) BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP)

#$(BRCM_BOARD_OBJ_DIR)/board.xml:
#	$(shell mkdir -p $(dir $@))
#	$(eval $(call brcm_write_board_xml,$@))

%.$(BRCM_APP_EXTENSION): $(BRCM_APP_PREBUILDS) $(BRCM_CHIP_LIBS_LIST) FORCE
	@echo "Apps for $(board)"
	$(QUIET)$(MAKE) all -f app.mk board=$(board) comp=$(call brcm_chip_comp_from_app,$@,$(OS)) VERBOSE=$(VERBOSE) OS=$(OS) BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP)

FORCE:

else ifneq ($(chip),)

.PHONY: all clean doc prebuilds libs list_comp
ifeq ($($(chip)_config_mk),)
$(error Unknown Chip:$(chip))
endif
MAKEFLAGS += -j 8

ifneq ($(comp),)
BRCM_CHIP_LIBS_LIST := $(call brcm_chip_lib_from_comp,$(comp))
else
BRCM_CHIP_LIBS :=
$(eval $(call brcm_calc_chip_libs,BRCM_CHIP_LIBS))
BRCM_CHIP_LIBS_LIST := $(call brcm_chip_lib_from_comp,$(BRCM_CHIP_LIBS))
endif

all:$(BRCM_CHIP_LIBS_LIST)
	@echo "Build for $(chip) Completed"

list_comp:
	@echo "Supported Components: $(BRCM_CHIP_ENABLED_LIBS)"

%.a:
	$(QUIET)$(MAKE) all -f lib.mk chip=$(chip) comp=$(call brcm_chip_comp_from_lib,$@) VERBOSE=$(VERBOSE) BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP)

clean:
	@echo "Cleaning for $(chip)"
	rm -rf $(BRCM_LIB_DIR) $(BRCM_LIB_OBJ_DIR)

else ifneq ($(family),)
.PHONY: $(BRCM_BOARD_LIST) doc

ifeq ($(wildcard $(BRCM_SDK_ROOT)/tools/common/map_parser/config/memory_size_extractor.py),)
all: $(BRCM_BOARD_LIST)
	@echo "Build completed for $(family)"
else
all: $(BRCM_BOARD_LIST)
	$(QUIET)python $(BRCM_SDK_ROOT)/tools/common/map_parser/config/memory_size_extractor.py -o $(BRCM_SDK_ROOT)/out/obj/
	@echo "Build completed for $(family)"
endif

$(BRCM_BOARD_LIST):
	$(QUIET)echo "Starting Build for $@"
	$(QUIET)$(MAKE) board=$@

list_board:
	@echo "Listing all boards: $(BRCM_BOARD_LIST)"

clean:
	rm -rf $(BRCM_SDK_ROOT)/out/app $(BRCM_SDK_ROOT)/out/lib $(BRCM_SDK_ROOT)/out/obj $(BRCM_SDK_ROOT)/out/bootloader

doc:
	$(QUIET)$(MAKE) all -f doc.mk family=$(family)

else
BRCM_ALL_CHIP_FAMILY := bcm8910x bcm8953x bcm8956x
.PHONY: $(BRCM_ALL_CHIP_FAMILY)

all: $(BRCM_ALL_CHIP_FAMILY)
	@echo "All Chip Families Build completed"

$(BRCM_ALL_CHIP_FAMILY):
	$(QUIET)echo "Starting Build for $@"
	$(QUIET)$(MAKE) family=$@

list_family:
	@echo "Listing all families: $(BRCM_ALL_CHIP_FAMILY)"

clean:
	rm -rf $(BRCM_SDK_ROOT)/out

endif

help:
	@echo "LIB Build: make [chip=<bcm8910x> [comp=<utils>]]"
	@echo "APP Build: make [board=<bcm8910x> [comp=<vidxmtr>]]"
	@echo "Doc BUILD: make [family=<bcm8910x>]"
	@echo "Chip List: make [family=<bcm8910x>] list_chip"
	@echo "Board List: make [family=<bcm8910x>] list_board"
	@echo "Component List: make board=<bcm8910x> list_comp"


