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


##  @defgroup grp_mcudrv MCU Driver
#   @ingroup grp_drivers
#
#   @addtogroup grp_mcudrv
#   @{
#
#   @file drivers/mcu/comp.mk
#   @brief Makefile for MCU Driver
#   @version 0.1 Initial version
#   @}


BRCM_CONFIGURATION_ITEM += comp.mk

BRCM_CONFIGURATION_ITEM += inc/mcu.h
BRCM_CONFIGURATION_ITEM += inc/osil/mcu_osil.h
BRCM_CONFIGURATION_ITEM += lib/mcu.c
BRCM_CONFIGURATION_ITEM += doc/mcu_req.c
BRCM_CONFIGURATION_ITEM += os/erika/mcu_osil.c

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_CONFIGURATION_ITEM += inc/bcm8910x/clk.h
BRCM_CONFIGURATION_ITEM += inc/bcm8910x/mcu_ext.h
BRCM_CONFIGURATION_ITEM += inc/osil/bcm8910x/mcu_osil.h
BRCM_CONFIGURATION_ITEM += doc/bcm8910x/mcu_req.c
BRCM_CONFIGURATION_ITEM += lib/bcm8910x/mcu_drv.c
BRCM_CONFIGURATION_ITEM += lib/bcm8910x/mcu_ext.c
endif

ifeq ($(CHIP_FAMILY),bcm8953x)
BRCM_CONFIGURATION_ITEM += inc/osil/bcm8953x/mcu_osil.h
BRCM_CONFIGURATION_ITEM += inc/bcm8953x/clk.h
endif

ifeq ($(CHIP_FAMILY),bcm8956x)
BRCM_CONFIGURATION_ITEM += inc/bcm8956x/clk.h
BRCM_CONFIGURATION_ITEM += inc/bcm8956x/mcu_ext.h
BRCM_CONFIGURATION_ITEM += inc/osil/bcm8956x/mcu_osil.h
BRCM_CONFIGURATION_ITEM += doc/bcm8956x/mcu_req.c
BRCM_CONFIGURATION_ITEM += lib/bcm8956x/mcu_drv.c
BRCM_CONFIGURATION_ITEM += lib/bcm8956x/mcu_ext.c
endif

ifeq ($(CHIP_FAMILY),bcm8908x)
BRCM_CONFIGURATION_ITEM += inc/bcm8908x/clk.h
BRCM_CONFIGURATION_ITEM += inc/bcm8908x/mcu_ext.h
BRCM_CONFIGURATION_ITEM += inc/osil/bcm8908x/mcu_osil.h
BRCM_CONFIGURATION_ITEM += doc/bcm8908x/mcu_req.c
BRCM_CONFIGURATION_ITEM += lib/bcm8908x/mcu_drv.c
BRCM_CONFIGURATION_ITEM += lib/bcm8908x/mcu_ext.c
endif

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/mcu.c

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8910x/mcu_drv.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8910x/mcu_ext.c
endif

ifeq ($(CHIP_FAMILY),bcm8953x)
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8953x/mcu_drv.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8953x/cfg.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8953x/dmu.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8953x/mcu_ext.c
endif

ifeq ($(CHIP_FAMILY),bcm8956x)
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8956x/mcu_drv.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8956x/mcu_ext.c
endif

ifeq ($(CHIP_FAMILY),bcm8908x)
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8908x/mcu_drv.c
BRCM_SRC += $(BRCM_SDK_ROOT)/drivers/mcu/lib/bcm8908x/mcu_ext.c
endif

BRCM_COMP_NAME := mcudrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

