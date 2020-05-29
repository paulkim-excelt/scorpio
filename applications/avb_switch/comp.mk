#
# Copyright 2017-2019 Broadcom Limited.  All rights reserved.
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

##  @defgroup grp_avb_switch AVB Switch
#   @ingroup grp_applications
#
#   @addtogroup grp_avb_switch
#   @{
#   AVB Switch application provides ethernet L2 switching functionality
#   with gPTP protocol for low latency streaming
#
#   @section sec_avb_switch_config Configuration
#
#   @subsection subsec_avb_switch_ls_config Light-Stacking Configuration
#
#   BCM8953X allows devices to be connected in light stacking mode. Following
#   configuration is used for Light stacking variant of the AVB switch
#   application:
#   -# On the slave switch, IMP0 is connected to the master switch.
#   -# Slave switch 1 can use any port except IMP0 to connect to slave switch 2.
#   (valid only in case of 3 stacking)
#   -# On the master switch, IMP0 is connected to the external CPU
#   (BCM53003 in bcm89530_ls_evk case)
#   -# The master switch can use any ports except IMP0 to connect to slave
#   switch 1 and slave switch 2. (Only 5/6 can be used, BR-ports cannot be
#   used)
#   -# Following configurations are valid:
#   + 1 x master switch + 1 x slave switch.
#   + 1 x master switch + 2 x slave switches.
#
#   The bcm89530_ls_evk board has 3 BCM8953X in stacking topology. However, due
#   to board limitation (incorrect port configuration), it can only be used in 2
#   stacking topology. Below is the configuration 2 stacking topology on the
#   bcm89530_ls_evk board:
#   @image html avb_switch_2_stack_topo.jpg "2 stacking topology"
#
#   @file applications/avb_switch/comp.mk
#   @brief Makefile for AVB Switch application
#   @version 0.1 Initial version
#   @}
#

BRCM_CFLAGS += -DENABLE_SHELL_APP=1

BRCM_SRC += $(BRCM_SDK_ROOT)/applications/avb_switch/avb_switch.c

BRCM_COMP_NAME := avb_switch
BRCM_COMP_DEPENDS := ets
BRCM_COMP_TYPE := app

BRCM_APP_CFG_XML := bcm8953x
ifeq ($(board),bcm89531_ls_evk)
BRCM_APP_CFG_XML := bcm8953x_ls
endif

ifeq ($(chip),bcm89564g)
BRCM_APP_CFG_XML := bcm89564g
endif
