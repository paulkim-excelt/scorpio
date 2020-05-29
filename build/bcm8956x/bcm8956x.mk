##############################################################################
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
################################################################################

BRCM_CPU_MODEL := CORTEX_M7
BRCM_CPU_FAMILY := CORTEX_MX
BRCM_CPU_FREQUENCY := 400000000
BRCM_CPU_IRQ_STACK := 2048

LIB_GNU_CC ?= 0
LIB_RVCT_CC ?= 0

ifeq ($(LIB_GNU_CC),0)
ifeq ($(LIB_RVCT_CC),0)
LIB_GNU_CC = 1
endif
endif

BRCM_APP_EXTENSION := tar.gz

CHIP_FAMILY := bcm8956x

CPU := cortex-m7

BRCM_CHIP_BASIC_DEPENDS := mcudrv utils arm

BRCM_CHIP_RDB_INC := $(BRCM_SDK_ROOT)/system/bcm8956x/inc/rdb/a0

DEFAULT_COMP_DEPENDENCIES := mcudrv flashdrv uartdrv timdrv ethcntlr ethxcvr
DEFAULT_COMP_DEPENDENCIES += ethswitch ipcdrv otpdrv gpiodrv pinmuxdrv mdiodrv
DEFAULT_COMP_DEPENDENCIES += utils time console queue arm dbgmem rsa sha
DEFAULT_COMP_DEPENDENCIES += wdtdrv

DEFAULT_BRCM_INC := $(BRCM_SDK_ROOT)/system/utils/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/time/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/console/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/queue/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/rpc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/rpc/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/nvm/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/nvm/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/imgl/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/cpu/arm/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/common/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/flash/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/flash/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/gpio/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/gpio/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/uart/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/uart/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8956x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mcu/inc/osil/bcm8956x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pinmux/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/pinmux/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/spi/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/spi/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/timer/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/timer/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/otp/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/otp/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mdio/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/mdio/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x/chip
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/bcm8956x/chip
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/security/rsa/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/security/sha/inc

DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/include/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/comms/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/nif/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/ethernet/common/fqtss/inc

DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/dbgmem/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/wds/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/system/wds/inc/osil
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/watchdog/inc
DEFAULT_BRCM_INC += $(BRCM_SDK_ROOT)/drivers/watchdog/inc/osil

CCFLAGS += -D__BCM__  -D__BCM8956X__

# Flash max identifier
CCFLAGS += -DFLASH_MAX_HW_ID=1

# BCM8956X family has Switch with 9 port connected to 8 Ethernet transceiver
# and 1 CPU port (dummy Ethernet transceiver)
CCFLAGS += -DETHXCVR_HW_ID_MAX=9UL
CCFLAGS += -DENABLE_ETH_SWITCH
CCFLAGS += -DENABLE_CFP

CCFLAGS += -DETS_MAX_INTERFACES=14UL

CCFLAGS += -DCOMMS_MAX_STREAM_POLICER_ENTRIES=256UL
# BCM8956X family uses heart beat timer for Ethernet time
CCFLAGS += -DETHER_TIME_USES_HB_TIME

ITBL_MAX_NUM_ENTRIES ?= 20
CCFLAGS += -DITBL_MAX_NUM_ENTRIES=$(ITBL_MAX_NUM_ENTRIES)UL

# maximum size of the IPC message payload = 512 - IPC header size
CCFLAGS += -DRPC_MSG_PAYLOAD_SZ=496

ENABLE_UART_PL011 := TRUE
ENABLE_ULOG := TRUE
ENABLE_SPI_PL022 := TRUE
ENABLE_FLASH := TRUE
ENABLE_FLSMGR := TRUE
ENABLE_MSG_QUEUE := TRUE
ENABLE_UART_CONSOLE := TRUE
ENABLE_PTM := TRUE
ENABLE_TIMER_SP804 := TRUE
ENABLE_GPIO_GIO_V1 := TRUE
BCM_TIME_USES_SP804 := TRUE
ENABLE_ETH := TRUE
ENABLE_ETH_AMAC := TRUE
ENABLE_ETH_TIME := TRUE
ENABLE_ETH_BRPHY := TRUE
ENABLE_ETH_SGMII := TRUE
ENABLE_IPC := TRUE
ENABLE_RPC := TRUE
ENABLE_ETH_SWITCH := TRUE
ENABLE_CFP := TRUE
ENABLE_ETS := TRUE
ENABLE_DBGMEM := TRUE
ENABLE_OTP := TRUE
ENABLE_WATCHDOG_SP805 := TRUE

#Architecture Documentation
BRCM_ENABLED_COMP += bcm8956x drivers autosw comms applications bootloader tools

BRCM_ENABLED_COMP += arm mcudrv flashdrv timdrv uartdrv queue utils console time dbgmem
BRCM_ENABLED_COMP += ipcdrv ethcntlr ethxcvr ethswitch ets rpc_server
BRCM_ENABLED_COMP += rsa sha otpdrv gpiodrv pinmuxdrv mdiodrv wdtdrv
BRCM_ENABLED_COMP += security secureheap

BRCM_ENABLED_COMP += wds
BRCM_ENABLED_COMP += imgl nvm
BRCM_ENABLED_COMP += map_parser

ifneq ($(cust),1)
BRCM_ENABLED_COMP += bcmutil bcmutild eth_xcvr_ut
endif

BRCM_ENABLED_COMP += configurator

