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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################



##  @defgroup grp_ipcdrv IPC Driver
#   @ingroup grp_drivers
#
#   @addtogroup grp_ipcdrv
#   @{
#   @file ethernet/drivers/ipc/comp.mk
#   @brief Makefile for IPC Driver
#   @version 0.1 Initial version
#   @}

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC := $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/lib/common

DRIVERS_DIR := $(BRCM_SDK_ROOT)/ethernet/drivers

ifeq ($(ENABLE_IPC), TRUE)
BRCM_INC += $(DRIVERS_DIR)/ipc/lib/$(CHIP_FAMILY)
BRCM_SRC += $(DRIVERS_DIR)/ipc/lib/common/ipc.c
BRCM_SRC += $(DRIVERS_DIR)/ipc/lib/common/ipc_drv.c
BRCM_SRC += $(DRIVERS_DIR)/ipc/lib/$(CHIP_FAMILY)/ipc_plat.c
endif

ifeq ($(CHIP_FAMILY), bcm8956x)
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/include
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8956x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8956x/chip/common
BRCM_INC += $(DRIVERS_DIR)/ipc/inc/bcm8956x
BRCM_SRC += $(DRIVERS_DIR)/ipc/lib/$(CHIP_FAMILY)/ipc_pl022.c
ifeq ($(chip), bcm89560)
BRCM_CFLAGS += -DIPC_MAX_CHANNELS=1
endif
ifeq ($(chip), bcm89564g)
BRCM_CFLAGS += -DIPC_MAX_CHANNELS=2
endif #
endif

ifeq ($(CHIP_FAMILY), bcm8953x)
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/include
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8953x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8953x/chip/common
BRCM_CFLAGS += -DIPC_MAX_CHANNELS=1
endif

BRCM_COMP_NAME := ipcdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

