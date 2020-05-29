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
####################################################################################

##  @defgroup grp_eth_cntlrdrv Ethernet Controller Driver
#   @ingroup grp_drivers
#
#   @addtogroup grp_eth_cntlrdrv
#   @{
#   @file ethernet/drivers/controller/comp.mk
#   @brief Makefile for Ethernet Controller Driver
#   @version 0.1 Initial version
#   @}

# Ethernet controller & time build configuration items
ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_CONFIGURATION_ITEM := comp.mk
BRCM_CONFIGURATION_ITEM += doc/ethernet_req.c
BRCM_CONFIGURATION_ITEM += inc/ethernet.h
BRCM_CONFIGURATION_ITEM += inc/ether_ipc.h
BRCM_CONFIGURATION_ITEM += inc/osil/eth_osil.h
BRCM_CONFIGURATION_ITEM += lib/eth_cntlr_switch.h
BRCM_CONFIGURATION_ITEM += lib/ethernet.c
BRCM_CONFIGURATION_ITEM += inc/eth_cntlr.h
BRCM_CONFIGURATION_ITEM += lib/eth_cntlr_buf.h
BRCM_CONFIGURATION_ITEM += lib/eth_cntlr_buf.c
BRCM_CONFIGURATION_ITEM += doc/amac_req.c
BRCM_CONFIGURATION_ITEM += lib/amac.c
BRCM_CONFIGURATION_ITEM += lib/eth_time_v1.c
endif
ifeq ($(CHIP_FAMILY),bcm8953x)
#BRCM_CONFIGURATION_ITEM += lib/eth_time_v2.c
endif

ETH_CNTLR_DIR := $(BRCM_SDK_ROOT)/ethernet/drivers/controller

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=

ifeq ($(CHIP_FAMILY),bcm8910x)
BRCM_CFLAGS += -DETHER_TIMEHWIDMAX=1UL -DETHER_HW_ID_MAX=1UL
BRCM_CFLAGS += -DETHER_RXPKTSTARTALIGN=32UL
BRCM_CFLAGS += -DETHER_RXBUFFCOUNT=8UL
BRCM_CFLAGS += -DETHER_TXCH0BUFFCOUNT=5UL
BRCM_CFLAGS += -DETHER_TXCH1BUFFCOUNT=5UL
BRCM_CFLAGS += -DETHER_TXCH2BUFFCOUNT=5UL
BRCM_CFLAGS += -DETHER_TXCH3BUFFCOUNT=12UL
BRCM_CFLAGS += -DENABLE_ETH_AMAC
BRCM_CFLAGS += -DAMAC_DMA_MEM_VIEW_BASE=0x0
endif

ifeq ($(CHIP_FAMILY),bcm8953x)
BRCM_CFLAGS += -DETHER_TIMEHWIDMAX=1UL -DETHER_HW_ID_MAX=1UL
BRCM_CFLAGS += -DETHER_RXPKTSTARTALIGN=64UL
BRCM_CFLAGS += -DENABLE_ETH_GMAC
endif

ifeq ($(CHIP_FAMILY),bcm8956x)
BRCM_CFLAGS += -DETHER_TIMEHWIDMAX=1UL -DETHER_HW_ID_MAX=1UL
BRCM_CFLAGS += -DETHER_RXPKTSTARTALIGN=4UL
BRCM_CFLAGS += -DETHER_RXBUFFCOUNT=12UL
BRCM_CFLAGS += -DETHER_TXCH0BUFFCOUNT=12UL
BRCM_CFLAGS += -DENABLE_ETH_AMAC
BRCM_CFLAGS += -DAMAC_DMA_MEM_VIEW_BASE=0x10000
endif

ifeq ($(ENABLE_ETH), TRUE)
BRCM_SRC += $(ETH_CNTLR_DIR)/lib/ethernet.c
BRCM_SRC += $(ETH_CNTLR_DIR)/lib/eth_cntlr_switch.c
BRCM_SRC += $(ETH_CNTLR_DIR)/lib/eth_cntlr_buf.c
endif

ifeq ($(ENABLE_ETH_AMAC), TRUE)
    BRCM_SRC += $(ETH_CNTLR_DIR)/lib/amac.c
endif

ifeq ($(ENABLE_ETH_GMAC), TRUE)
    BRCM_SRC += $(ETH_CNTLR_DIR)/lib/gmac.c
endif

ifeq ($(ENABLE_ETH_TIME), TRUE)
ifeq ($(CHIP_FAMILY),bcm8910x)
    BRCM_SRC += $(ETH_CNTLR_DIR)/lib/eth_time_v1.c
endif
ifeq ($(CHIP_FAMILY),bcm8953x)
    BRCM_SRC += $(ETH_CNTLR_DIR)/lib/eth_time_v2.c
endif
ifeq ($(CHIP_FAMILY),bcm8956x)
    BRCM_SRC += $(ETH_CNTLR_DIR)/lib/eth_time_v2.c
endif
endif

BRCM_COMP_NAME := ethcntlr
BRCM_COMP_DEPENDS := mdiodrv
BRCM_COMP_TYPE := lib

