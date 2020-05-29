# /*****************************************************************************
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#******************************************************************************/

EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
COMMA := ,
COLON := :

CFLAGS += -DGNU

include $(BRCM_SDK_ROOT)/build/common.mk

isappopt = $(if $(filter $1,$(EEOPT)),yes,)

$(eval $(call brcm_full_clear_vars))
$(eval $(call brcm_chip_include_deps,$(comp),$(CHIP_FAMILY)))

CFLAGS += $(BRCM_FULL_APP_CFLAGS)
INCLUDE_PATH += $(addprefix -I,$(dir $(BRCM_FULL_EXPORT_HDR)))
INCLUDE_PATH += $(addprefix -I,$(dir $(BRCM_FULL_APP_INC)))
EE_SRCS += $(BRCM_FULL_APP_SRC)

$(eval $(call brcm_comp_clear_vars))
include $($(comp)_config_mk)
CFLAGS += $(BRCM_CFLAGS)
INCLUDE_PATH += $(BRCM_INC)
EE_SRCS += $(BRCM_SRC)


INCLUDE_PATH += $(DEFAULT_BRCM_INC)

include $(BRCM_SDK_ROOT)/build/compiler/gnu_cc.mk

CFLAGS += $(CCFLAGS)

ifneq ($(BRCM_COMP_PREBUILDS_LIST),)
BRCM_COMP_PREBUILD_SRC := $(subst $(COMMA),$(SPACE),$(BRCM_COMP_PREBUILDS_LIST))
EE_SRCS += $(addprefix $(BRCM_APP_DIR)/$(comp)_obj/,$(BRCM_COMP_PREBUILD_SRC))
endif

INCLUDE_PATH += $(BRCM_SDK_ROOT)/
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/utils/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/time/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/console/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/cpu/arm/include
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/
INCLUDE_PATH += $(BRCM_SDK_ROOT)/system/include
INCLUDE_PATH += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/drivers/include
INCLUDE_PATH += $(BRCM_SDK_ROOT)/security/rsa/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/security/sha/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/applications/framework/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/applications/framework/inc/examples
INCLUDE_PATH += $(BRCM_SDK_ROOT)/applications/framework/inc/os/erika
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/avcd/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/avce/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/dup/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/ldc/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/openvx/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/pgx/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/pplib/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/ppr/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/stitch/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/pp/venc/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/camera/isys/inc
INCLUDE_PATH += $(BRCM_SDK_ROOT)/multimedia/camera/isys/inc/osil


include $(utils_config_mk)

include $(BRCM_SDK_ROOT)/drivers/common/os/erika/cfg.mk

ifeq ($(call iseeopt, __BCM8910X__), yes)
include $(BRCM_SDK_ROOT)/system/bcm8910x/os/erika/cfg.mk
endif # __BCM8910X__

ifeq ($(call iseeopt, __BCM8953X__), yes)
include $(BRCM_SDK_ROOT)/system/bcm8953x/os/erika/cfg.mk
endif # __BCM8953X__

ifeq ($(call iseeopt, __BCM8956X__), yes)
include $(BRCM_SDK_ROOT)/system/bcm8956x/os/erika/cfg.mk
endif # __BCM8956X__

ifeq ($(call iseeopt, ENABLE_MSG_QUEUE), yes)
include $(BRCM_SDK_ROOT)/system/queue/os/erika/cfg.mk
endif   # ENABLE_MSG_QUEUE

ifeq ($(or $(call iseeopt, ENABLE_KLOG), $(call iseeopt, ENABLE_ULOG)), yes)
include $(BRCM_SDK_ROOT)/system/utils/os/erika/cfg.mk
endif   # ENABLE_KLOG or ENABLE_ULOG

ifeq ($(call iseeopt, ENABLE_MEDIA_FRAMEWORK), yes)
include $(BRCM_SDK_ROOT)/applications/framework/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, __CORTEX_MX__),yes)
include $(BRCM_SDK_ROOT)/cpu/arm/os/erika/cfg.mk
endif
##################### audio components ##########################
ifeq ($(call iseeopt, ENABLE_I2S), yes)
include $(BRCM_SDK_ROOT)/multimedia/audio/controller/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AUDIO_PLAY), yes)
include $(BRCM_SDK_ROOT)/multimedia/audio/play/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AUDIO_RECORD), yes)
include $(BRCM_SDK_ROOT)/multimedia/audio/record/os/erika/cfg.mk
endif

##################### camera components ##########################
ifeq ($(call iseeopt, ENABLE_CAM), yes)
include $(BRCM_SDK_ROOT)/multimedia/camera/component/os/erika/cfg.mk
endif

##################### display components ##########################
ifeq ($(call iseeopt, ENABLE_VGC), yes)
include $(BRCM_SDK_ROOT)/multimedia/display/vgc/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_GRAPHICS_SOURCE), yes)
include $(BRCM_SDK_ROOT)/multimedia/display/graphics/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_SGFX_ICLUSTER), yes)
include $(BRCM_SDK_ROOT)/demo/sgfx/os/erika/cfg.mk
endif

##################### pp git components ##########################
ifeq ($(call iseeopt, ENABLE_PP), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/pplib/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_LDC), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/ldc/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_PPR), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/ppr/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_VENC), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/venc/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AVCE), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/avce/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AVCD), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/avcd/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_OPTICALFLOW), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/opticalflow/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_OPENVX), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/openvx/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_VDUP), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/dup/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_PALLET_GRAPHICS), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/pgx/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_STITCH), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/stitch/os/erika/cfg0.mk
endif

ifeq ($(call iseeopt, ENABLE_SCALE_PLACE), yes)
include $(BRCM_SDK_ROOT)/multimedia/pp/stitch/os/erika/cfg1.mk
endif


##################### ethernet components ##########################
ifeq ($(call iseeopt, ENABLE_ETH), yes)
include $(BRCM_SDK_ROOT)/ethernet/drivers/controller/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_ETH), yes)
include $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/os/erika/cfg.mk
include $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_ETH_SWITCH), yes)
include $(BRCM_SDK_ROOT)/ethernet/drivers/switch/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AVB_TALKER), yes)
include $(BRCM_SDK_ROOT)/ethernet/1722/os/erika/talker/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_AVB_LISTENER), yes)
include $(BRCM_SDK_ROOT)/ethernet/1722/os/erika/listener/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_INET), yes)
include $(BRCM_SDK_ROOT)/ethernet/tcp_ip/inet/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_RTP_SENDER), yes)
include $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/os/erika/sender/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_RTP_RECEIVER), yes)
include $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rtp/os/erika/receiver/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_RFS), yes)
include $(BRCM_SDK_ROOT)/ethernet/tcp_ip/rfs/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_ETS), yes)
include $(BRCM_SDK_ROOT)/ethernet/802.1as/os/erika/cfg.mk
endif

ifeq ($(call iseeopt, ENABLE_RPC), yes)
include $(BRCM_SDK_ROOT)/ethernet/common/rpc/os/erika/cfg.mk
endif

include $(BRCM_SDK_ROOT)/ethernet/common/comms/os/erika/cfg.mk

include $(BRCM_SDK_ROOT)/ethernet/common/nif/os/erika/cfg.mk
##################### wireless components ##########################
ifeq ($(call iseeopt, ENABLE_WLAN), yes)
include $(BRCM_SDK_ROOT)/ethernet/wireless/os/erika/cfg.mk
endif
