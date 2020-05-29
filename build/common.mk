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

define brcm_comp_clear_vars
BRCM_COMP_DISABLE_TRACE:=
BRCM_COMP_DOC_SRC:=
BRCM_COMP_EXPORT_HDR:=
BRCM_COMP_LIB_INC:=
BRCM_COMP_LIB_SRC:=
BRCM_COMP_LIB_CFLAGS :=
BRCM_COMP_APP_INC:=
BRCM_COMP_APP_SRC:=
BRCM_COMP_APP_CFLAGS :=
BRCM_COMP_APP_TASK :=
BRCM_COMP_APP_EVENT :=
BRCM_COMP_APP_OPT :=
BRCM_COMP_APP_INTR :=
BRCM_COMP_APP_COUNTER :=
BRCM_COMP_APP_ALARM :=
BRCM_COMP_APP_CFG :=
BRCM_COMP_CONFIG_SCRIPT:=
BRCM_COMP_CONFIG_SRC:=
BRCM_COMP_QT_SRC:=
BRCM_COMP_IT_SRC:=
BRCM_COMP_UT_SRC:=
BRCM_COMP_REQ_SRC:=
BRCM_COMP_NAME:=
BRCM_COMP_DEPENDS:=
BRCM_COMP_TYPE:=
BRCM_COMP_PREBUILD_SRC:=
BRCM_CONFIGURATION_ITEM:=
BRCM_SRC:=
BRCM_ASM_SRC:=
BRCM_CFLAGS:=
BRCM_APP_CFG_XML:=
BRCM_ERIKA_CONF_INC:=
BRCM_SYSCFG_XCVR_CFG:=
BRCM_SYSCFG_MAC_CFG:=
BRCM_INC:=
BRCM_FW_NUM_COPIES:=
endef

define brcm_full_clear_vars
BRCM_FULL_DOC_SRC:=
BRCM_FULL_EXPORT_HDR:=
BRCM_FULL_LIB_INC:=
BRCM_FULL_LIB_SRC:=
BRCM_FULL_LIB_CFLAGS :=
BRCM_FULL_APP_INC:=
BRCM_FULL_APP_SRC:=
BRCM_FULL_APP_CFLAGS :=
BRCM_FULL_APP_TASK :=
BRCM_FULL_APP_EVENT :=
BRCM_FULL_APP_OPT :=
BRCM_FULL_APP_INTR :=
BRCM_FULL_APP_COUNTER :=
BRCM_FULL_APP_ALARM :=
BRCM_FULL_APP_CFG :=
BRCM_FULL_CONFIG_SCRIPT:=
BRCM_FULL_CONFIG_SRC:=
BRCM_FULL_QT_SRC:=
BRCM_FULL_IT_SRC:=
BRCM_FULL_UT_SRC:=
BRCM_FULL_REQ_SRC:=
BRCM_FULL_ALARM:=
BRCM_FULL_COUNTER:=
BRCM_FULL_NAME:=
BRCM_FULL_DEPENDS:=
BRCM_FULL_PREBUILD_SRC:=
BRCM_FULL_CONFIGURATION_ITEM:=
endef

define brcm_board_clear_vars
BRCM_BOARD_ENABLED_APP:=
BRCM_BOARD_OPTIONS:=
endef

define brcm_chip_clear_vars
BRCM_ENABLED_COMP:=
BRCM_CHIP_BASIC_DEPENDS:=
BRCM_CHIP_RDB_INC :=
DEFAULT_COMP_DEPENDENCIES:=
endef

define brcm_calc_chip_libs
$(foreach LOOP, $(BRCM_ENABLED_COMP), \
			$(eval $(call brcm_comp_clear_vars))\
			$(eval include $($(LOOP)_config_mk))\
			$(if $(filter $(BRCM_COMP_TYPE),lib),$(eval $(1) += $(LOOP)))\
			$(eval $(call brcm_comp_clear_vars)))
endef

define brcm_calc_chip_hostapps
$(foreach LOOP, $(BRCM_ENABLED_COMP), \
			$(eval $(call brcm_comp_clear_vars))\
			$(eval include $($(LOOP)_config_mk))\
			$(if $(filter $(BRCM_COMP_TYPE),hostapp),$(eval $(1) += $(LOOP)))\
			$(eval $(call brcm_comp_clear_vars)))
endef

define brcm_chip_lib_from_comp
$(addprefix $(BRCM_LIB_DIR)/lib$(chip)_,$(addsuffix .a,$(1)))
endef

define brcm_chip_hostapp_from_comp
$(addprefix $(BRCM_HOST_DIR)/,$(1))
endef

define brcm_chip_comp_from_hostapp
$(subst $(BRCM_HOST_DIR)/,,$(1))
endef

define brcm_chip_comp_from_lib
$(subst .a,,$(subst $(BRCM_LIB_DIR)/lib$(chip)_,,$(1)))
endef

define brcm_board_app_from_comp
$(addprefix $(BRCM_APP_DIR)/$(board)_,$(addsuffix _$(2).$(BRCM_APP_EXTENSION),$(1)))
endef

define brcm_chip_comp_from_app
$(subst _$(2).$(BRCM_APP_EXTENSION),,$(subst $(BRCM_APP_DIR)/$(board)_,,$(1)))
endef

define brcm_remove_duplicate
$(eval BRCM_FULL_CONFIGURATION_ITEM += $(filter-out $(BRCM_FULL_CONFIGURATION_ITEM),$(1)))
endef

define brcm_chip_include_docs
$(if $(filter $(BRCM_COMP_DISABLE_TRACE),TRUE),\
		$(eval BRCM_FULL_DOC_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(1))),\
		$(eval $(call brcm_remove_duplicate, $(addprefix $(dir $($(LOOP)_config_mk)),$(1)))))
endef

define brcm_chip_include_comp
$(if $(filter $(BRCM_COMP_DISABLE_TRACE),TRUE),\
		$(eval BRCM_FULL_DOC_SRC+=$(1)),\
		$(eval $(call brcm_remove_duplicate, $(1))))
endef

# 1 - list of dep comps
define brcm_chip_include_deps_internal
$(foreach LOOP,$(1),\
	$(if $(filter $(LOOP),$(brcm_comp_var_list_int)),,\
		$(eval $(call brcm_comp_clear_vars))\
		$(eval include $($(LOOP)_config_mk))\
		$(call brcm_chip_include_comp,$($(LOOP)_config_mk))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_DOC_SRC))\
		$(eval BRCM_FULL_EXPORT_HDR+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_EXPORT_HDR)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_EXPORT_HDR))\
		$(eval BRCM_FULL_LIB_INC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_LIB_INC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_LIB_INC))\
		$(eval BRCM_FULL_LIB_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_LIB_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_LIB_SRC))\
		$(eval BRCM_FULL_LIB_CFLAGS+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_LIB_CFLAGS)))\
		$(eval BRCM_FULL_APP_INC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_APP_INC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_APP_INC))\
		$(eval BRCM_FULL_APP_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_APP_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_APP_SRC))\
		$(eval BRCM_FULL_APP_CFLAGS+=$(BRCM_COMP_APP_CFLAGS))\
		$(eval BRCM_FULL_APP_TASK+=$(BRCM_COMP_APP_TASK))\
		$(eval BRCM_FULL_APP_EVENT+=$(BRCM_COMP_APP_EVENT))\
		$(eval BRCM_FULL_APP_OPT+=$(BRCM_COMP_APP_OPT))\
		$(eval BRCM_FULL_APP_INTR+=$(BRCM_COMP_APP_INTR))\
		$(eval BRCM_FULL_APP_COUNTER+=$(BRCM_COMP_APP_COUNTER))\
		$(eval BRCM_FULL_APP_ALARM+=$(BRCM_COMP_APP_ALARM))\
		$(eval BRCM_FULL_APP_CFG+=$(BRCM_COMP_APP_CFG))\
		$(eval BRCM_FULL_CONFIG_SCRIPT+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_CONFIG_SCRIPT)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_CONFIG_SCRIPT))\
		$(eval BRCM_FULL_CONFIG_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_CONFIG_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_CONFIG_SRC))\
		$(eval BRCM_FULL_QT_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_QT_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_QT_SRC))\
		$(eval BRCM_FULL_IT_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_IT_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_IT_SRC))\
		$(eval BRCM_FULL_UT_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_UT_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_UT_SRC))\
		$(eval BRCM_FULL_REQ_SRC+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_REQ_SRC)))\
		$(call brcm_chip_include_docs,$(BRCM_COMP_REQ_SRC))\
		$(eval $(call brcm_remove_duplicate, $(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_CONFIGURATION_ITEM))))\
		$(eval BRCM_FULL_COUNTER+=$(BRCM_COUNTER))\
		$(eval BRCM_FULL_ALARM+=$(BRCM_ALARM))\
		$(eval BRCM_FULL_DEPENDS+=$(addprefix $(dir $($(LOOP)_config_mk)),$(BRCM_COMP_DEPENDS)))\
		$(eval brcm_comp_var_list_int+=$(LOOP))\
		$(if $(BRCM_LIB_SRC),$(eval BRCM_FULL_COMP_DEP_LIBS += $(LOOP)))\
		$(if $(filter $(BRCM_COMP_TYPE),lib),$(eval BRCM_FULL_COMP_DEP_LIBS += $(LOOP)))\
		$(eval $(call brcm_chip_include_deps_internal,$(BRCM_COMP_DEPENDS)))))
BRCM_FULL_DOC_SRC=$(sort $(BRCM_FULL_DOC_SRC))
endef


# 1 - comp , 2 - default depends
define brcm_chip_include_deps
$(eval brcm_comp_var_list_int:=)
$(eval $(call brcm_chip_include_deps_internal,$(1) $(2)))
endef


EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
COMMA := ,
COLON := :
SLASH := /
DOT := .

OS ?= erika

ifdef VERBOSE
QUIET=
else
QUIET=@
endif


BRCM_SDK_ROOT := $(realpath $(dir $(lastword $(MAKEFILE_LIST)))../)
BRCM_COMP_SEARCH_DIR ?= $(BRCM_SDK_ROOT)
BRCM_ABS_SEARCH_DIR = $(shell readlink -f $(BRCM_COMP_SEARCH_DIR))


ifeq ($(BRCM_ALL_COMP_MAP),)
BRCM_ALL_COMP_MK_FILES := $(shell find $(BRCM_ABS_SEARCH_DIR) -name comp.mk -o -name comp[0-9].mk)
# Iterate $BRCM_ALL_COMP_MK_FILES which contains list of "comp.mk" files, and retreive component list.
# reset the variables before and after using brcm_comp_clear_vars.
$(foreach LOOP, $(BRCM_ALL_COMP_MK_FILES), \
			$(eval $(call brcm_comp_clear_vars))\
			$(eval include $(LOOP))\
			$(if $(filter-out ,$(BRCM_COMP_NAME)),$(eval BRCM_ALL_COMP_MAP_LIST +=  $(BRCM_COMP_NAME)$(COLON)$(LOOP)),)\
			$(eval $(call brcm_comp_clear_vars)))
BRCM_ALL_COMP_MAP := $(subst $(SPACE),$(COMMA),$(BRCM_ALL_COMP_MAP_LIST))
else
BRCM_ALL_COMP_MAP_LIST := $(subst $(COMMA),$(SPACE),$(BRCM_ALL_COMP_MAP))
endif


# Export all comp.mk locations with the corresponding component name as <comp_name>_config_mk=<filename>
BRCM_COMP_LIST:=
$(foreach LOOP, $(BRCM_ALL_COMP_MAP_LIST),\
			$(eval $(word 1,$(subst $(COLON),$(SPACE),$(LOOP)))_config_mk:=$(word 2,$(subst $(COLON),$(SPACE),$(LOOP))))\
			$(eval BRCM_COMP_LIST+=$(word 1,$(subst $(COLON),$(SPACE),$(LOOP)))))

# Export all board directories
BRCM_BOARD_LIST:=
ifeq ($(family),)
$(foreach LOOP, $(shell find $(BRCM_SDK_ROOT)/build/ -name board.mk),\
        $(eval $(lastword $(subst $(SLASH),$(SPACE),$(subst /board.mk,,$(LOOP))))_config_mk:=$(LOOP))\
		$(eval BRCM_BOARD_LIST+=$(lastword $(subst $(SLASH),$(SPACE),$(subst /board.mk,,$(LOOP))))))
else
$(foreach LOOP, $(shell find $(BRCM_SDK_ROOT)/build/$(family) -name board.mk),\
        $(eval $(lastword $(subst $(SLASH),$(SPACE),$(subst /board.mk,,$(LOOP))))_config_mk:=$(LOOP))\
		$(eval BRCM_BOARD_LIST+=$(lastword $(subst $(SLASH),$(SPACE),$(subst /board.mk,,$(LOOP))))))
endif


# Export all chip directories
BRCM_CHIP_LIST:=
ifeq ($(family),)
$(foreach LOOP, $(shell find $(BRCM_SDK_ROOT)/build/*/chip/ -name \*.mk),\
        $(eval $(firstword $(subst $(DOT),$(SPACE),$(lastword $(subst $(SLASH),$(SPACE),$(LOOP)))))_config_mk:=$(LOOP))\
		$(eval BRCM_CHIP_LIST+=$(firstword $(subst $(DOT),$(SPACE),$(lastword $(subst $(SLASH),$(SPACE),$(LOOP)))))))
else
$(foreach LOOP, $(shell find $(BRCM_SDK_ROOT)/build/$(family)/chip -name \*.mk),\
        $(eval $(firstword $(subst $(DOT),$(SPACE),$(lastword $(subst $(SLASH),$(SPACE),$(LOOP)))))_config_mk:=$(LOOP))\
		$(eval BRCM_CHIP_LIST+=$(firstword $(subst $(DOT),$(SPACE),$(lastword $(subst $(SLASH),$(SPACE),$(LOOP)))))))
endif


BRCM_LIB_DIR = $(BRCM_SDK_ROOT)/out/lib/$(chip)
BRCM_CHIP_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(chip)
BRCM_LIB_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(chip)/$(comp)
BRCM_APP_DIR = $(BRCM_SDK_ROOT)/out/app/$(board)
BRCM_BOARD_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(board)
BRCM_APP_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(board)/$(comp)
BRCM_BOOTLOADER_DIR = $(BRCM_SDK_ROOT)/out/bootloader/$(board)
BRCM_BOOTLOADER_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(board)/bootloader
BRCM_HOST_DIR = $(BRCM_SDK_ROOT)/out/host/$(CHIP_FAMILY)
BRCM_HOSTAPP_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(CHIP_FAMILY)/host/$(comp)
BRCM_HOST_OBJ_DIR = $(BRCM_SDK_ROOT)/out/obj/$(CHIP_FAMILY)/host
BRCM_DOC_DIR = $(BRCM_SDK_ROOT)/out/doc/$(family)
BRCM_DOC_OBJ_DIR = $(BRCM_SDK_ROOT)/out/doc/$(family)

BRCM_PREBUILT_DIR = $(BRCM_SDK_ROOT)/prebuilt/$(chip)/lib


ifneq ($(board),)
$(eval $(call brcm_board_clear_vars))
BRCM_BOARD_DIR := $(shell dirname $($(board)_config_mk))
include $($(board)_config_mk)
ifeq ($(BRCM_BOOTLOADER_VARIANT),)
BRCM_BOOTLOADER_VARIANT := $(board)
endif
endif


ifneq ($(chip),)
$(eval $(call brcm_chip_clear_vars))
BRCM_CHIP_DIR := $(shell dirname $($(chip)_config_mk))
include $($(chip)_config_mk)
BRCM_CHIP_FAMILY_UPPERCASE := $(shell echo $(CHIP_FAMILY) | tr a-z A-Z)
else
BRCM_CHIP_FAMILY_UPPERCASE := $(shell echo $(family) | tr a-z A-Z)
endif

ifneq ($(comp),)
BRCM_COMP_DIR := $(shell dirname $($(comp)_config_mk))
endif

BRCM_CHIP_UPPERCASE := $(shell echo $(chip) | tr a-z A-Z)
BRCM_BOARD_UPPERCASE := $(shell echo $(board) | tr a-z A-Z)

