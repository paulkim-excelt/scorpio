#
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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


##  @defgroup grp_cortex ARM Cortex CPUs
#   @ingroup grp_system_sw
#
#   @addtogroup grp_cortex
#   @{
#   @section sec_cortex_overview Overview
#	ARM library contains low level functions for various ARM architectures (like
# 	Cortex-M7, Cortex-R4 etc). Low level functions include
#		-# Cache Maintenance
#		-# MPU/MMU Setup and configuration
#		-# Any architecture spcific peripherals (like SysTick in M7)
#		-# Interrupt Controller configuration and control
#		-# Reset Control
#   This library also contains abstractions for	special instructions in ARM
# 	processors, however, user may choose to use them on specific variants based
# 	on availability and behavior of the instruction.
#	<BR>This also contains functions for Atomic operations and other utility
# 	functions.
#
#   @trace #BRCM_SWREQ_CORTEX_COMMON
#
#   @limitations None
#
#   @file cpu/arm/comp.mk
#   @brief Makefile for ARM CPU Library
#   @version 0.1 Initial version
#   @}
#


BRCM_COMP_REQ_SRC += doc/cortex_req.c

BRCM_COMP_EXPORT_HDR += inc/atomic.h
BRCM_COMP_LIB_SRC += lib/atomic.c
BRCM_COMP_EXPORT_HDR += inc/cortex.h
BRCM_COMP_EXPORT_HDR += inc/interrupt.h
BRCM_COMP_EXPORT_HDR += inc/cache.h
ifeq ($(CPU),cortex-m7)
BRCM_COMP_REQ_SRC += doc/cortex_mx_req.c
BRCM_COMP_EXPORT_HDR += inc/cortex_mx.h
BRCM_COMP_LIB_SRC += lib/cortex_mx/mpu.c
BRCM_COMP_LIB_SRC += lib/cortex_mx/scs.c
BRCM_COMP_LIB_SRC += lib/cortex_mx/interrupt.c
BRCM_COMP_LIB_SRC += lib/cortex_mx/cache.c
BRCM_COMP_EXPORT_HDR += inc/cortex_fpu.h
BRCM_INC += $(BRCM_SDK_ROOT)/cpu/arm/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/system/include
BRCM_CONFIGURATION_ITEM += os/erika/cache_osil.c
BRCM_CONFIGURATION_ITEM += inc/osil/cache_osil.h
endif

ifeq ($(CPU),cortex-r4)
BRCM_SRC += $(BRCM_SDK_ROOT)/cpu/arm/lib/cortex_rx/cache.c
BRCM_SRC += $(BRCM_SDK_ROOT)/cpu/arm/lib/cortex_rx/mpu.c
BRCM_SRC += $(BRCM_SDK_ROOT)/cpu/arm/lib/cortex_rx/interrupt.c
endif #ifeq ($(CPU),cortex-r4)

BRCM_COMP_NAME := arm
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

