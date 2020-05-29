#/****************************************************************************
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
################################################################################


##  @defgroup grp_ets Ethernet TimeSync (ETS)
#   @ingroup grp_comms_sw
#
#   @addtogroup grp_ets
#   @{
#   @section sec_ets_overview Overview
#   @image html etslib_overview.png "Component Overview"
#
#   This Module contains requirements, Interface, Design and implementation
#   details of ETS
#   <BR>
#   The Ethernet TimeSync (ETS) library provides time-of-day synchronization
#   over Ethernet. It supports operation in both end point (single port) and
#   bridge (multi-port) modes with the following features:
#       - PDELAY operation
#           -# Computation of peer delay and neighbor rate ratio.
#           -# Response to requests from the peer
#           -# Transition from initial to operational PDELAY intervals
#       - SYNC operation:
#           -# Distribution of time in grandmaster mode
#           -# Synchronization to received time in end point slave mode
#           -# Propagation of time in switch slave mode
#           -# Computation of grandmaster rate ratio in slave mode
#           -# Transition from initial to operational SYNC intervals in end
#              point slave mode
#       - SIGNALING operation:
#           -# Transmission of signaling message in slave mode, to request for
#              change in interval
#           -# Honoring received signaling messages in master mode and changing
#              transmission interval accordingly
#           -# Propagation of signaling requests from master to slave ports in
#              switch slave mode
#       - NETWORK CLOCK operation:
#           -# Provision of network time to clients in both master and slave
#              modes
#           -# Setting of global time in master mode
#   @limitations None
#   @trace #BRCM_SWREQ_ETS_CONFIG
#
#   @}
#
#   @file ethernet/802.1as/comp.mk
#   @brief Makefile for ETS
#   @version 0.1 Initial version
#

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

ifeq ($(ENABLE_ETS), TRUE)
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/include

BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/ets_api.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/ets_common.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/ets_pdelay.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/ets_timesync.c
BRCM_SRC += $(BRCM_SDK_ROOT)/ethernet/802.1as/lib/ets_signaling.c
endif

BRCM_COMP_NAME := ets
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := lib

