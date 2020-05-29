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


##  @defgroup grp_utils Utilities
#   @ingroup grp_system_sw
#
#   @addtogroup grp_utils
#   @{
#   Broadcom SDK provides utilities like
#       -# Pre-defined error codes for all component use
#       -# Wrappers for Memory Copy/Compare operations
#       -# Wrappers for Time APIs
#       -# Error Reporting ability
#       -# Logging
#
#   @limitations None
#   @file system/utils/comp.mk
#   @brief Makefile for BCM Utils/Logging/Error Reporting
#   @version 0.1 Initial version
#   @}
#
#   @defgroup grp_circq Circular Queue
#   @ingroup grp_system_sw
#
#   @addtogroup grp_circq
#   @{
#   @section sec_circq_overview Overview
#   This Module contains requirements, Interface, Design
#   and implementation details of Circular Queues.
#   <BR>Circular Queues are intented to be used by logging,
#   media framework and media components extensively.
#
#   Circular Queue provides the following features:
#       -# Macros to define and initialize a CIRCQ (overwrite/non-overwrite mode in #CIRCQ)
#       -# APIs to check if CIRCQ is empty or full
#       -# APIs to push/pop to/from the CIRCQ
#       -# API to peek into the CIRCQ
#       .
#   @todo update state machine and sequence diagram
#   @limitations Maximum number of objects in the queue is limited to 65536 and
#                shall be in powers of 2.
#
#   @}
#
#   @defgroup grp_memtest Memory Test
#   @ingroup grp_system_sw
#


BRCM_COMP_REQ_SRC += doc/utils_req.c
BRCM_COMP_REQ_SRC += doc/circ_queue_req.c

BRCM_COMP_EXPORT_HDR += inc/bcm_memtest.h
BRCM_COMP_EXPORT_HDR += inc/bcm_err.h
BRCM_COMP_EXPORT_HDR += inc/bcm_comp.h
BRCM_COMP_EXPORT_HDR += inc/bcm_utils.h
BRCM_COMP_EXPORT_HDR += inc/compiler.h
BRCM_COMP_EXPORT_HDR += inc/svc.h
BRCM_COMP_EXPORT_HDR += inc/circ_queue.h
BRCM_COMP_EXPORT_HDR += inc/crc.h
BRCM_COMP_EXPORT_HDR += inc/osil/bcm_osil.h
BRCM_COMP_EXPORT_HDR += inc/idle.h

BRCM_COMP_LIB_SRC += lib/error.c
BRCM_COMP_LIB_SRC += lib/svc_request.c
BRCM_COMP_LIB_SRC += lib/crc.c
BRCM_COMP_LIB_SRC += lib/circ_queue.c
BRCM_COMP_LIB_SRC += lib/bcm_memtest.c

BRCM_COMP_QT_SRC += tests/qualification/circ_queue_qt.h
BRCM_COMP_QT_SRC += tests/qualification/memtest_qt.h

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_COMP_EXPORT_HDR += inc/osil/bcm_osil_svc.h
endif

BRCM_COMP_APP_OPT += ENABLE_WFI

BRCM_COMP_APP_SRC += os/erika/idle.c

BRCM_COMP_APP_TASK += IDLE_Task
IDLE_Task.priority = 1
IDLE_Task.stack = 512

BRCM_COMP_NAME := utils
BRCM_COMP_DEPENDS := arm
BRCM_COMP_TYPE := lib

