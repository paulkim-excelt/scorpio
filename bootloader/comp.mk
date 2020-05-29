#
# Copyright 2019 Broadcom Limited.  All rights reserved.
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

##  @defchapter grp_bootloader Bootloader
#
#   @addtogroup grp_bootloader
#   @{
#   This chapter describes Bootloader with below features (some of them may not be enabled for
#   specific platform)
#   -# Faster Transceiver configuration for quick linkup
#   -# Partition validation for loading a valid firmware
#   -# Firmware update process
#   .
#
#   Automotive SoCs come with inbuilt BootROM which searches for an executable binary in Flash, loads and executes it.
#   BootROM typically executes with Caches and interrupts disabled, low speed settings (example: Flash is accessed at
#   25MHz) etc.
#
#   Before actual application software, Bootloader boosts startup performance by enabling caches and configuring with
#   added supports for features like
#   - Quick Link up on BroadR-Reach / Ethernet
#   - Failsafe Software Update (as BootROM typically does not support writing to Flash)
#   - Support for Crash dump
#   - Initialization of volatile memories
#   - Boot over Ethernet/CAN etc
#   - Early Diagnostics
#
#   Below figure depicts Bootloader software architecture.
#   @image html bl_overview.jpg "Bootloader Block Diagram"
#
#   Bootloader is an OS less firmware loaded by BootRom from Flash and executed. Bootloader continues to execute without
#   Interrupts. Some configuration of Bootloader is specific to the board as it needs to be aware of external
#   peripherals available on the board. Hence, Bootloader image is specific to board.
#
#   Bootloader consists of basic drivers, middleware and Bootloader Control Software (BCS). BCS initializes basic
#   drivers, components and controls complete flow of bootloader. BCS interacts with middleware consisting of Partition
#   Manager and various types of downloaders. Middleware interacts with various drivers including Flash, Ethernet, UART,
#   Timer etc.
#
#   @file bootloader/comp.mk
#   @brief Makefile for Bootloader
#   @version 0.1 Initial version
#   @}
#
#    @defgroup grp_bl_bootstrap BootStrapping
#    @ingroup grp_bootloader
#
#    @addtogroup grp_bl_bootstrap
#    @{
#    When the BL enters main(), it looks for a valid FW image on the flash. If it is found, BL loads it and jumps to its
#    entry point else it enters download mode.
#    Before giving control to FW, BL cleans, invalidates and disables D-Cache and I-Cache.
#    Below figure depicts the startup sequence of BL before entering main().
#    @image html bl_startup_sequence.jpg "BL Startup Sequence Diagram"
#    @}
#

BRCM_COMP_NAME := bootloader
BRCM_COMP_DEPENDS :=
BRCM_COMP_TYPE := doc

