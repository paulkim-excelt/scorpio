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

define brcm_erika_generate_xml
$(eval BRCM_FULL_APP_CFG_XML +=  <brcm_cfg>)
$(eval BRCM_FULL_APP_CFG_XML +=    <app>)
$(eval BRCM_FULL_APP_CFG_XML +=      <board>$(BRCM_BOARD_UPPERCASE)</board>)
$(eval BRCM_FULL_APP_CFG_XML +=      <mcu name="$(BRCM_CHIP_UPPERCASE)" family="$(BRCM_CHIP_FAMILY_UPPERCASE)">)
$(eval BRCM_FULL_APP_CFG_XML +=        <cpu model="$(BRCM_CPU_MODEL)" family="$(BRCM_CPU_FAMILY)">)
$(eval BRCM_FULL_APP_CFG_XML +=            <frequency>$(BRCM_CPU_FREQUENCY)</frequency>)
$(eval BRCM_FULL_APP_CFG_XML +=        </cpu>)
$(eval BRCM_FULL_APP_CFG_XML +=      </mcu>)
$(eval BRCM_FULL_APP_CFG_XML +=      <irq_stack>$(BRCM_CPU_IRQ_STACK)</irq_stack>)
$(eval BRCM_FULL_APP_CFG_XML +=      <compiler>GNU</compiler>)
$(eval BRCM_FULL_APP_CFG_XML +=      <ee_cfg>)
$(eval BRCM_FULL_APP_CFG_XML +=       <status>EXTENDED</status>)
$(eval BRCM_FULL_APP_CFG_XML +=       <kernel>ECC2</kernel>)
$(eval BRCM_FULL_APP_CFG_XML +=       <startuphook>true</startuphook>)
$(eval BRCM_FULL_APP_CFG_XML +=       <pretaskhook>true</pretaskhook>)
$(eval BRCM_FULL_APP_CFG_XML +=       <posttaskhook>true</posttaskhook>)
$(eval BRCM_FULL_APP_CFG_XML +=      </ee_cfg>)
$(foreach LOOP, $(BRCM_FULL_APP_OPT), $(eval BRCM_FULL_APP_CFG_XML += <opt>$(LOOP)</opt>))
$(foreach LOOP, $(BRCM_FULL_APP_INTR), $(eval BRCM_FULL_APP_CFG_XML += <interrupt name="$(LOOP)" function="$($(LOOP).function)"/>))
$(foreach LOOP, $(BRCM_FULL_APP_EVENT), $(eval BRCM_FULL_APP_CFG_XML += <event>$(LOOP)</event>))
$(foreach LOOP, $(BRCM_FULL_APP_TASK), $(eval BRCM_FULL_APP_CFG_XML += <task name="$(LOOP)">)\
						    $(eval BRCM_FULL_APP_CFG_XML += <priority>$($(LOOP).priority)</priority>)\
						    $(if $($(LOOP).autostart), $(eval BRCM_FULL_APP_CFG_XML += <autostart>$($(LOOP).autostart)</autostart>), $(eval BRCM_FULL_APP_CFG_XML += <autostart>false</autostart>))\
						    $(eval BRCM_FULL_APP_CFG_XML += <stack>$($(LOOP).stack)</stack>)\
                            $(foreach EVENT, $($(LOOP).event), $(eval BRCM_FULL_APP_CFG_XML += <event>$(EVENT)</event>))
                            $(foreach WDS, $($(LOOP).wds), $(eval BRCM_FULL_APP_CFG_XML += <wdsSe name="$(WDS)">)\
							                                $(eval BRCM_FULL_APP_CFG_XML += <maxAllowedErrors>$($(WDS).maxAllowedErrors)</maxAllowedErrors>)\
							                                $(eval BRCM_FULL_APP_CFG_XML += <maxState>$($(WDS).maxState)</maxState>)\
							                                $(eval BRCM_FULL_APP_CFG_XML += <maxTransitionsPerState>$($(WDS).maxTransitionsPerState)</maxTransitionsPerState>)\
							                                $(eval BRCM_FULL_APP_CFG_XML += <errorCallback>$($(WDS).errorCallback)</errorCallback>)\
							                                $(eval BRCM_FULL_APP_CFG_XML += </wdsSe>))\
						    $(if $($(LOOP).group), $(eval BRCM_FULL_APP_CFG_XML += <group>$($(LOOP).group)</group>),)\
                        $(eval BRCM_FULL_APP_CFG_XML += </task>))
$(foreach LOOP, $(BRCM_FULL_APP_COUNTER), $(eval BRCM_FULL_APP_CFG_XML += <counter name="$(LOOP)">)\
						    $(eval BRCM_FULL_APP_CFG_XML += <mincycle>$($(LOOP).mincycle)</mincycle>)\
						    $(eval BRCM_FULL_APP_CFG_XML += <maxallowedvalue>$($(LOOP).maxallowedvalue)</maxallowedvalue>)\
						    $(eval BRCM_FULL_APP_CFG_XML += <ticksperbase>$($(LOOP).ticksperbase)</ticksperbase>)\
						    $(eval BRCM_FULL_APP_CFG_XML += <secondspertick>$($(LOOP).secondspertick)</secondspertick>)\
                        $(eval BRCM_FULL_APP_CFG_XML += </counter>))
$(foreach LOOP, $(BRCM_FULL_APP_ALARM), $(eval BRCM_FULL_APP_CFG_XML += <alarm name="$(LOOP)" counter="$($(LOOP).counter)" callback="$($(LOOP).callback)">)\
						    $(eval BRCM_FULL_APP_CFG_XML += <autostart>$($(LOOP).autostart)</autostart>)\
						    $(eval BRCM_FULL_APP_CFG_XML += <alarmtime>$($(LOOP).alarmtime)</alarmtime>)\
						    $(eval BRCM_FULL_APP_CFG_XML += <cycletime>$($(LOOP).cycletime)</cycletime>)\
                        $(eval BRCM_FULL_APP_CFG_XML += </alarm>))
$(foreach LOOP, $(BRCM_FULL_APP_CFG), $(eval BRCM_FULL_APP_CFG_XML += <cfg pid="$($(LOOP).pid)" imgid="$($(LOOP).imgid)" maxsz="$($(LOOP).maxsz)" file="$($(LOOP).file)"/>))
$(eval BRCM_FULL_APP_CFG_XML +=      </app>)
$(eval BRCM_FULL_APP_CFG_XML +=    </brcm_cfg>)
endef

$(eval $(call brcm_erika_generate_xml))

include $(BRCM_SDK_ROOT)/build/compiler/gnu_cc.mk
BRCM_ECLIPSE_HOME?=/projects/BLR_SW_AUTOMOTIVE/tools/erika_tools/eclipse/eclipse_2.4.0.17
# Includes valid only for 8910X platform
ifeq ($(CHIP_FAMILY), bcm8910x)
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/drivers/sd/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/camera/component/config

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/ldc/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/avce/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/venc/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/avcd/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/stitch/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/dup/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/display/vgc/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/display/graphics/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/audio/play/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/audio/record/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/multimedia/audio/controller/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/1722/os/erika/talker
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/1722/os/erika/listener
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/os/erika/sender
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/os/erika/receiver
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/inet/os/erika
BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rfs/os/erika

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/cpu/arm/os/erika
endif

BRCM_ERIKA_CONF_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/os/erika

BRCM_ERIKA_CONF_INC+=$(BRCM_BOARD_DIR)
BRCM_ERIKA_CONF_INC+=$(BRCM_BOARD_DIR)/../../
BRCM_ERIKA_CONF_INC_LIST = $(subst $(SPACE),:,$(BRCM_ERIKA_CONF_INC))
BRCM_ERIKA_CONF_DIRS := $(subst :, ,$(BRCM_ERIKA_CONF_INC))
$(foreach DIR, $(BRCM_ERIKA_CONF_DIRS), \
                    $(if  $(filter-out ,$(DIR)),$(eval BRCM_COMP_OIL_FILES += $(shell find $(DIR) -name *.oil)),))
$(foreach DIR, $(BRCM_ERIKA_CONF_DIRS), \
                    $(if  $(filter-out ,$(DIR)),$(eval BRCM_COMP_XML_FILES += $(shell find $(DIR) -name *.xml)),))

BRCM_ERIKA_ENV = export ERIKA_FILES=$(BRCM_SDK_ROOT)/erika-sdk;
BRCM_ERIKA_ENV += export HOST_MACHINE_TYPE=Linux;
BRCM_ERIKA_ENV += export COMPILER_PREFIX=${COMPILER_PREFIX};
BRCM_ERIKA_ENV += export RTDRUID_TEMPLATES_PATH=$(BRCM_SDK_ROOT)/erika-sdk/applications;
BRCM_ERIKA_ENV += export preference_cortex_mx__path_for_gnu_compiler=${TOOLCHAIN_PATH};
BRCM_ERIKA_ENV += export preference_cortex_rx__path_for_gnu_compiler=${TOOLCHAIN_PATH};
BRCM_ERIKA_ENV += export ECLIPSE_HOME=$(BRCM_ECLIPSE_HOME);
BRCM_ERIKA_ENV += export SDK_TOOLS=$(BRCM_SDK_ROOT)/erika-sdk/tools;
BRCM_ERIKA_ENV += export TOOLCHAIN_PATH=${TOOLCHAIN_PATH};


BRCM_RTDRUID_OUTPUT = $(addprefix $(BRCM_APP_OBJ_DIR)/, eecfg.h eecfg.c makefile)
ifneq ($(wildcard $(BRCM_SDK_ROOT)/build/.git/.),)
SW_IMAGE_VERSION := $(shell git for-each-ref --sort=-taggerdate --format=%\(refname:short\) refs/tags --merged | grep -im1 $(CHIP_FAMILY)_REL | awk -F"_REL_" '{print $$NF}')
else
include $(BRCM_BOARD_DIR)/../../sw_version.txt
endif
BRCM_ERIKA_ENV += export SW_IMAGE_VERSION=$(SW_IMAGE_VERSION);

BRCM_APP_LD_FILE := $(BRCM_APP_OBJ_DIR)/app_gnu.ld
$(shell rm -f $(BRCM_APP_LD_FILE))

ifeq ($(shell test -e $(BRCM_COMP_DIR)/cfg.mk && echo -n yes),yes)
app_cfg=$(BRCM_COMP_DIR)/cfg.mk
endif

ifneq ($(wildcard $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml),)
BRCM_COMP_XML_FILES += $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml
endif

$(info $(BRCM_COMP_XML_FILES))

$(shell mkdir -p $(BRCM_APP_OBJ_DIR))
$(file >$(BRCM_APP_OBJ_DIR)/application.xml,<?xml version="1.0"?>)
$(foreach loop, $(BRCM_FULL_APP_CFG_XML), $(file >>$(BRCM_APP_OBJ_DIR)/application.xml, $(loop)))

$(BRCM_APP_OBJ_DIR)/c_rX.bin: $(BRCM_DEP_LIBS) $(BRCM_RTDRUID_OUTPUT) $(BRCM_APP_LD_FILE)
	$(QUIET)$(BRCM_ERIKA_ENV)	\
	$(MAKE) -C $(BRCM_APP_OBJ_DIR) all app_cfg=$(app_cfg) prebuilt=$(BRCM_LIB_DIR)/lib BRCM_SDK_ROOT=$(BRCM_SDK_ROOT) board=$(board) comp=$(comp)

$(BRCM_APP_LD_FILE): $(BRCM_RTDRUID_OUTPUT)
	$(QUIET)printf 'EXTERN(%s)\n' $(addprefix brcm_revision_, $(BRCM_FULL_COMP_DEP_LIBS)) > $@
	$(QUIET)echo "SEARCH_DIR($(BRCM_LIB_DIR)/lib)" >> $@
	$(QUIET)echo "SEARCH_DIR($(BRCM_APP_OBJ_DIR)/)" >> $@
	$(QUIET)echo "GROUP($(BRCM_DEP_LIBS) libee.a)" >> $@

%eecfg.c %eecfg.h %makefile: $(BRCM_COMP_XML_FILES)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(BRCM_SDK_ROOT)/build/common/xmlparse.py -b $(BRCM_APP_OBJ_DIR)/application.xml -c $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml -s $(BRCM_SDK_ROOT)/build/common/schema.xsd -o $(BRCM_APP_OBJ_DIR) -r $(BRCM_SDK_ROOT)

