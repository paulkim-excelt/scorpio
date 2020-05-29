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

##  @defchapter grp_drivers Drivers
#   @addtogroup grp_drivers
#   @{
#   Although most drivers are tightly coupled to one/few components, the
#   architecture defines a Device Driver Layer consisting of all the drivers
#   which interact with the hardware. All the needed hardware interactions
#   are implemented through corresponding drivers.
#   Figure below depicts the Device Driver architecture to be followed by
#   all drivers [some names are given for reference purposes only].
#   @image html grp_drivers_overview.jpg
#   Each driver follows the Component design specified in Generic Design
#   Methodology section. It consists of two layers, namely HW Specific logic
#   and OS specific integration layer. Architecture also specifies a chip
#   specific layer which defines the number of instances of each device and
#   also the location of the device in SOC. Architecture/SLD highly
#   recommends reuse of the HW Specific implementation across operating
#   systems both in source and binary formats.
#   <BR>To meet Low Latency requirements in the system, each device driver
#   registers its Interrupt Service Routine (ISR). Interrupt Service
#   Routines perform minimal necessary operations to acknowledge the
#   hardware and to take note of the updates from the hardware. Further
#   processing is deferred to pre-defined group of high priority tasks
#   called Driver Group Tasks.
#   <BR>Each driver is mapped statically to a predefined task in Driver Task
#   Group to process the bottom half of the interrupt and other user
#   requests as necessary [in case of asynchronous requests].  Refer to Task
#   Map for the list of all tasks.
#   <BR>To reduce context switching overheads between privileged and
#   unprivileged task (unprivileged task cannot write to drivers/peripheral
#   memory), System Command Request (SVC_Request as described in Privileged
#   Software section) is used to quickly process without switching context
#   to relevant driver task.
#   @image html grp_drivers_sys_cmd_sync.jpg
#   @image html grp_drivers_sys_cmd_async.jpg
#   HW Specific Implementation layer (Core Logic layer) uses necessary
#   Interrupt Suspend/Resume APIs to protect the resources from interrupts.
#   This layer contains minimal or no global variables. OS specific
#   integration layer software uses OS specific calls as necessary.
#   All RW data that is private to this layer shall be put under
#   .data.drivers. System ensures access to this section is restricted to
#   privileged mode only. All polling loops are implemented using time based
#   fixed loop iterations [for example, try for 8 times with a delay of X
#   ns each].
#
#   @limitations Add component limitations here
#   @file process/templates/comp/comp.mk
#   @brief Makefile for Template Component
#   @version 0.1 Initial version
#   @}

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

BRCM_COMP_NAME := drivers
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

