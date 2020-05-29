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



##  @defgroup grp_nvm Non-Volatile Memory
#   @ingroup grp_system_sw
#
#   @addtogroup grp_nvm
#   @{
#   This module abstracts/processes requests for read/write from/into
#   Non-Volatile Memories (Flash and OTP)
#
#   @file system/nvm/comp.mk
#   @brief Makefile for Non-Volatile Memory
#   @version 0.1 Initial version
#   @}
#

BRCM_COMP_EXPORT_HDR += inc/nvm_pt.h
BRCM_COMP_REQ_SRC += doc/nvm_req.c


BRCM_COMP_APP_OPT += ENABLE_FLASH
BRCM_COMP_APP_OPT += ENABLE_FLSMGR
BRCM_COMP_APP_OPT += ENABLE_PTM

ifeq ($(call isappopt,ENABLE_FLASH0),yes)
BRCM_COMP_APP_INTR += QSPI0
QSPI0.function = FLASH_IRQHandler0
endif
ifeq ($(call isappopt,ENABLE_FLASH1),yes)
BRCM_COMP_APP_INTR += QSPI1
QSPI1.function = FLASH_IRQHandler1
endif

BRCM_COMP_APP_TASK += FLSMGR_Server
FLSMGR_Server.priority = 8
FLSMGR_Server.stack = 2048
FLSMGR_Server.event += SystemEvent0
FLSMGR_Server.event += SystemEvent1
FLSMGR_Server.event += SystemEvent2
FLSMGR_Server.event += SystemEvent3
FLSMGR_Server.event += SystemEvent4
FLSMGR_Server.event += SystemEvent5
FLSMGR_Server.event += SystemEvent6
FLSMGR_Server.event += SystemEvent7
FLSMGR_Server.event += SystemEvent8
FLSMGR_Server.event += ShutdownEvent


BRCM_COMP_NAME := nvm
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

