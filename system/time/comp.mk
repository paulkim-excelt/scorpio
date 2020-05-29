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
################################################################################


##  @defgroup grp_time_utils Time Utilities
#   @ingroup grp_system_sw
#
#   @addtogroup grp_time_utils
#   @{
#   Broadcom SDK provides wrappers for Time APIs
#
#   @limitations None
#   @file system/time/comp.mk
#   @brief Makefile for BCM Time Wrappers
#   @version 0.1 Initial version
#   @}
#

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_CONFIGURATION_ITEM := inc/bcm_time.h
BRCM_CONFIGURATION_ITEM += doc/bcm_time_req.c


BRCM_INC += $(BRCM_SDK_ROOT)/system/include

ifeq ($(ENABLE_AVT), TRUE)
    BRCM_CFLAGS += -DBCM_TIME_USES_AVT=1
endif

ifeq ($(ENABLE_TIMER_SP804), TRUE)
ifeq ($(BCM_TIME_USES_SP804), TRUE)
    BRCM_CFLAGS += -DBCM_TIME_USES_SP804=1
endif
endif

ifeq ($(CHIP_FAMILY), bcm8953x)
	BRCM_CFLAGS += -DBCM_HRTIMER_CHANID=1UL
	BRCM_CFLAGS += -DBCM_HRTIMER_EXPIRY_TICKS=62UL
else ifeq ($(CHIP_FAMILY), bcm8956x)
	BRCM_CFLAGS += -DBCM_HRTIMER_CHANID=1UL
	BRCM_CFLAGS += -DBCM_HRTIMER_EXPIRY_TICKS=62UL
else ifeq ($(CHIP_FAMILY), bcm8910x)
	BRCM_CFLAGS += -DBCM_HRTIMER_CHANID=0UL
	BRCM_CFLAGS += -DBCM_HRTIMER_EXPIRY_TICKS=500UL
endif #CHIP_FAMILY

BRCM_SRC += $(BRCM_SDK_ROOT)/system/time/lib/bcm_time.c

BRCM_COMP_NAME := time
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

