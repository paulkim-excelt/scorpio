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



##  @defgroup grp_pinmuxdrv PINMUX Driver
#   @ingroup grp_drivers
#
#   @addtogroup grp_pinmuxdrv
#   @{
#   @file drivers/pinmux/comp.mk
#   @brief Makefile for PINMUX Driver
#   @version 0.1 Initial version
#   @}


BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

DRIVERS_DIR := $(BRCM_SDK_ROOT)/drivers

BRCM_SRC += $(DRIVERS_DIR)/pinmux/lib/pinmux.c

ifeq ($(ENABLE_GPIO_GIO_V2), TRUE)
  BRCM_CONFIGURATION_ITEM := comp.mk
  BRCM_CONFIGURATION_ITEM += inc/pinmux.h
  BRCM_CONFIGURATION_ITEM += lib/pinmux.c
  BRCM_CONFIGURATION_ITEM += lib/pinmux_drv.c
  BRCM_CONFIGURATION_ITEM += inc/osil/pinmux_osil.h
  BRCM_CONFIGURATION_ITEM += os/erika/pinmux_osil.c
  BRCM_CONFIGURATION_ITEM += doc/pinmux_req.c
  BRCM_SRC += $(DRIVERS_DIR)/pinmux/lib/pinmux_drv.c
  BRCM_CFLAGS += -DENABLE_GPIO_GIO_V2
endif

ifeq ($(CHIP_FAMILY),bcm8910x)
	BRCM_SRC += $(DRIVERS_DIR)/pinmux/lib/bcm8910x/pinmux_table.c
endif
ifeq ($(CHIP_FAMILY),bcm8908x)
	BRCM_SRC += $(DRIVERS_DIR)/pinmux/lib/bcm8908x/pinmux_table.c
endif

BRCM_COMP_NAME := pinmuxdrv
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

