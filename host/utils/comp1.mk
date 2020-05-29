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

BRCM_SRC :=
BRCM_CFLAGS :=
BRCM_INC :=


ifeq ($(OS),LINUX)
BRCM_CFLAGS += -DLINUX
endif

BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_imgl.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_system.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_comms.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_etherswt.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_ether.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_ets.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/lib/host_cfp.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/hipc.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/spi_xfer.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/server.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hlog.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/crc.c

BRCM_CFLAGS += -DENABLE_DBGMEM
BRCM_CFLAGS += -DENABLE_HOST_COMMS_CMD_HANDLER
BRCM_CFLAGS += -DENABLE_HOST_ETS_CMD_HANDLER
BRCM_CFLAGS += -DENABLE_HOST_CFP_CMD_HANDLER
BRCM_CFLAGS += -DSPIUTIL_IMAGE_VERSION=\"$(SPIUTIL_IMAGE_VERSION)\"
BRCM_CFLAGS += -DHIPC_ENABLE_SPI

BRCM_INC := $(BRCM_SDK_ROOT)/host/utils/include
BRCM_INC += $(BRCM_SDK_ROOT)/host/utils/hipc

BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc/osil
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/include/
BRCM_INC += $(BRCM_SDK_ROOT)/system/common/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/utils/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/time/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/rpc/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/comms/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/queue/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/imgl/inc
BRCM_INC += $(BRCM_SDK_ROOT)/syscfg/include
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/inc/
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/802.1as/inc/osil/
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/nif/inc
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc ## form paul NXP build
#BRCM_INC += $(BRCM_SDK_ROOT)/sysroots/aarch64-fsl-linux/usr/include ## form paul NXP build


#Conditional compilation based on host/target chipset
$(info CHIP_FAMILY: $(CHIP_FAMILY))
ifeq ($(CHIP_FAMILY),bcm8953x)
BRCM_CFLAGS += -DBE_HOST
BRCM_CFLAGS += -D__BCM8953X__
BRCM_CFLAGS += -D__BCM5300X__
BRCM_CFLAGS += -D__CORTEX_RX__
BRCM_CFLAGS += -D__CORTEX_R4__
BRCM_CFLAGS += -DENABLE_RECORD_NOTIFICATION

BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm8953x/bcm8953x_hipc.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm5300x/bcm5300x_spi.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm5300x/bcm5300x_gpio.c

BRCM_INC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm5300x
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8953x
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8953x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8953x/chip/common
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8953x
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8953x/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8953x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/system/include/bcm8953x

else ifeq ($(CHIP_FAMILY),bcm8956x)
CCFLAGS += -D__BCM89560__
CCFLAGS += -D__BCM8956X__
CCFLAGS += -D__BCM58712__
CCFLAGS += -D__CORTEX_MX__
CCFLAGS += -D__CORTEX_M7__
BRCM_CFLAGS += -DHIPC_ENABLE_PCIE

BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm8956x/bcm8956x_hipc.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm58712/bcm58712_spi.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm58712/bcm58712_gpio.c
BRCM_SRC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm58712/bcm58712_pcie_xfer.c

BRCM_INC += $(BRCM_SDK_ROOT)/host/utils/hipc/bcm58712
BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8956x
BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/bcm8956x
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8956x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/bootloader/bcm8956x/chip/common
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x/inc
BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x/chip
BRCM_INC += $(BRCM_SDK_ROOT)/system/include/bcm8956x
BRCM_INC += $(BRCM_SDK_ROOT)/host/ns2/kernel/git/drivers/net/ethernet/broadcom/xgbe
endif

BRCM_COMP_NAME := bcmutild
BRCM_COMP_DEPENDS := bcmutil
BRCM_COMP_TYPE := hostapp



