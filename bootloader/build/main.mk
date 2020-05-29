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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################
# File Name: main.mk
# DESCRIPTION: makefile to trigger bootloader build.
#####################################################################################

##############################################
# Derive variables
##############################################
BUILD_TIME	:= $(shell date)
SRC_DIR = $(BRCM_SDK_ROOT)/bootloader
TARGET_DIR ?= $(SRC_DIR)/out/$(board)

C_DEFS += -D$(CPU)

INCS += -I$(BRCM_SDK_ROOT)/system/nvm/inc
INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/inc
INCS += -I$(BRCM_SDK_ROOT)/system/common/inc
INCS += -I$(BRCM_SDK_ROOT)/system/imgl/inc
ifeq ($(BL_ENABLE_IPC_DWNLD_MODE), TRUE)
INCS += -I$(BRCM_SDK_ROOT)/ethernet/common/rpc/inc
INCS += -I$(BRCM_SDK_ROOT)/system/common/inc
INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc
INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/osil
endif
ifeq ($(CHIP_FAMILY), bcm8910x)
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8910x/inc
endif
ifeq ($(CHIP_FAMILY), bcm8953x)
    INCS += -I$(BRCM_SDK_ROOT)/system/$(CHIP_FAMILY)/inc/rdb/b1
    INCS += -I$(BRCM_SDK_ROOT)/system/$(CHIP_FAMILY)/inc/
endif
INCS += -I$(SRC_DIR)/include
INCS += -I$(SRC_DIR)/comms/inet/include
INCS += -I$(SRC_DIR)/$(CHIP_FAMILY)/
INCS += -I$(SRC_DIR)/$(CHIP_FAMILY)/chip/
INCS += -I$(SRC_DIR)/$(CHIP_FAMILY)/chip/common
INCS += -I$(BRCM_SDK_ROOT)/drivers/mcu/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/mcu/inc/osil
INCS += -I$(BRCM_SDK_ROOT)/system/utils/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/timer/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/timer/inc/osil
INCS += -I$(BRCM_SDK_ROOT)/system/time/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/gpio/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/pinmux/inc
INCS += -I$(BRCM_SDK_ROOT)/drivers/pinmux/inc/osil
INCS += -I$(BRCM_SDK_ROOT)/cpu/arm/inc/
INCS += -I$(BRCM_SDK_ROOT)/system/console/inc/
INCS += -I$(BRCM_SDK_ROOT)/system/queue/inc/
ifeq ($(BL_ENABLE_DDR), TRUE)
	INCS += -I$(BRCM_SDK_ROOT)/drivers/memc/inc
endif
ifeq ($(CHIP_FAMILY), bcm8910x)
	INCS += -I$(BRCM_SDK_ROOT)/drivers/avt/inc
endif
INCS += -I$(BRCM_SDK_ROOT)/drivers/otp/inc

ifeq ($(CHIP_FAMILY), bcm8910x)
    INCS += -I$(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8910x
endif

ifeq ($(CHIP_FAMILY), bcm8953x)
    INCS += -I$(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8953x
ifeq ($(BL_ENABLE_ETH_SWITCH), TRUE)
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8953x/chip/common
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8953x/chip
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8953x/inc
endif
endif

ifeq ($(CHIP_FAMILY), bcm8956x)
    INCS += -I$(BRCM_SDK_ROOT)/drivers/mcu/inc/bcm8956x
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/ipc/inc/bcm8956x
    INCS += -I$(BRCM_SDK_ROOT)/system/$(CHIP_FAMILY)/inc/rdb/a0
ifeq ($(BL_ENABLE_ETH_SWITCH), TRUE)
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8956x/chip/common
    INCS += -I$(BRCM_SDK_ROOT)/system/bcm8956x/inc
endif
ifeq ($(BL_ENABLE_SLAVE_BOOT), TRUE)
	C_SRCS += $(SRC_DIR)/bcm8956x/chip/$(chip)/bl_slave_boot.c
endif
endif

LD_FLAGS += -L$(SRC_DIR)/$(CHIP_FAMILY)/
LD_FLAGS += -L$(SRC_DIR)/$(CHIP_FAMILY)/chip

C_EXT_SRCS += $(BRCM_SDK_ROOT)/system/utils/lib/bcm_memtest.c
C_EXT_SRCS += $(BRCM_SDK_ROOT)/system/utils/lib/crc.c
C_EXT_SRCS += $(BRCM_SDK_ROOT)/system/utils/lib/error_log.c
C_EXT_SRCS += $(BRCM_SDK_ROOT)/system/utils/lib/svc_request.c

C_SRCS += $(SRC_DIR)/utils/bl_utils.c
C_SRCS += $(SRC_DIR)/utils/bl_edc.c

C_SRCS += $(SRC_DIR)/control/bl_main.c
C_SRCS += $(SRC_DIR)/control/bl_updater.c
C_SRCS += $(SRC_DIR)/control/bl_xcvr_cfg.c
C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/mcu/os/none/mcu_osil.c
C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/timer/os/none/gpt_osil.c
ifeq ($(CHIP_FAMILY), bcm8910x)
C_SRCS += $(SRC_DIR)/control/bl_swr_cfg.c
	C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/avt/os/none/avt_osil.c
endif
ifeq ($(BL_ENABLE_GPIO), TRUE)
	C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/pinmux/os/none/pinmux_osil.c
endif
C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/otp/os/none/otp_osil.c
ifeq ($(CHIP_FAMILY), bcm8910x)
C_EXT_SRCS += $(BRCM_SDK_ROOT)/cpu/arm/os/none/cache_osil.c
endif

ifneq ($(BL_NUM_COPIES),)
	CCFLAGS += -DBL_NUM_COPIES=$(BL_NUM_COPIES)
endif

ifeq ($(CPU), BL_CORTEX_M7)
	ASM_SRCS += $(SRC_DIR)/$(CHIP_FAMILY)/chip/common/bl_startup.S
	C_SRCS += $(SRC_DIR)/$(CHIP_FAMILY)/chip/common/bl_early_init.c
endif

ifeq ($(CHIP_FAMILY), bcm8910x)
	C_SRCS += $(SRC_DIR)/bcm8910x/chip/common/bl_piocfg.c
	C_SRCS += $(SRC_DIR)/bcm8910x/chip/common/bl_xcvr_cfg.c
	C_DEFS += -DBL_ENABLE_BOOT_PROFILING
endif

ifeq ($(CHIP_FAMILY), bcm8908x)
	C_SRCS += $(SRC_DIR)/bcm8908x/chip/common/bl_mcu.c
	C_SRCS += $(SRC_DIR)/bcm8908x/chip/common/bl_piocfg.c
	C_SRCS += $(SRC_DIR)/bcm8908x/chip/common/bl_xcvr_cfg.c
	C_DEFS += -DBL_ENABLE_BOOT_PROFILING
endif

ifeq ($(CPU), CORTEX_R4)
	ASM_SRCS += $(SRC_DIR)/$(CHIP_FAMILY)/chip/common/bl_startup.S
	C_SRCS += $(SRC_DIR)/$(CHIP_FAMILY)/chip/common/bl_early_init.c
endif

ifeq ($(CHIP_FAMILY), bcm8953x)
	C_DEFS += -DBL_ENABLE_BOOT_PROFILING
endif

include $(SRC_DIR)/$(CHIP_FAMILY)/board/board.mk

ifeq ($(BL_ENABLE_RAM_DWNLD_MODE), TRUE)
    BL_ENABLE_TAR_PROCESSING := TRUE
    C_DEFS += -DBL_ENABLE_RAM_DWNLD_MODE
    C_SRCS += $(SRC_DIR)/control/bl_ram_downloader.c
endif

ifeq ($(BL_ENABLE_IPC_DWNLD_MODE), TRUE)
    C_DEFS += -DBL_ENABLE_IPC_DWNLD_MODE
	C_DEFS += -DENABLE_IPC
    C_SRCS += $(SRC_DIR)/control/bl_ipc_downloader.c
	C_EXT_SRCS += $(BRCM_SDK_ROOT)/ethernet/drivers/ipc/os/none/ipc_osil.c
endif

ifeq ($(BL_ENABLE_INET), TRUE)
	C_DEFS += -DBL_ENABLE_INET
include $(BRCM_SDK_ROOT)/bootloader/comms/inet/default.mk
# Porting files
	C_SRCS += $(SRC_DIR)/comms/inet/src/port/bl_ethif.c
	C_SRCS += $(SRC_DIR)/comms/inet/src/port/bl_inet_cfg.c
endif

ifeq ($(BL_ENABLE_TFTP_DWNLD_MODE), TRUE)
    BL_ENABLE_TAR_PROCESSING := TRUE
	C_DEFS += -DBL_ENABLE_TFTP_DWNLD_MODE
	C_SRCS += $(SRC_DIR)/bcm8910x/bl_tftp_downloader.c
	C_SRCS += $(SRC_DIR)/comms/inet/src/app/bl_command.c
	C_SRCS += $(SRC_DIR)/comms/inet/src/app/bl_tftp.c
endif

ifeq ($(BL_ENABLE_TAR_PROCESSING), TRUE)
	C_SRCS += $(SRC_DIR)/utils/bl_tar.c
endif

ifeq ($(BL_ENABLE_TIME), TRUE)
endif

ifeq ($(BL_ENABLE_DBGMEM), TRUE)
    INCS += -I$(BRCM_SDK_ROOT)/drivers/dbgmem/inc
    INCS += -I$(BRCM_SDK_ROOT)/drivers/dbgmem/inc/osil
    C_EXT_SRCS += $(BRCM_SDK_ROOT)/drivers/dbgmem/os/none/dbgmem_osil.c
    C_DEFS += -DBL_ENABLE_DBGMEM
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_dbgmem.a
endif

ifeq ($(BL_ENABLE_ETH_SWITCH), TRUE)
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/common/comms/inc/
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/switch/lib/
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/switch/inc/
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/controller/inc/
    INCS += -I$(BRCM_SDK_ROOT)/ethernet/drivers/mdio/inc/
    C_SRCS += $(SRC_DIR)/control/bl_switch_access.c
    C_DEFS += -DBL_ENABLE_ETHSWT
    C_DEFS += -DCOMMS_MAX_STREAM_POLICER_ENTRIES=256UL
endif

ifeq ($(BL_ENABLE_AVT), TRUE)
	C_DEFS += -DBL_ENABLE_AVT
endif

ifeq ($(BL_ENABLE_GPT), TRUE)
	C_DEFS += -DBL_ENABLE_GPT
endif

ifeq ($(BL_BCM_TIME_USES_GPT), TRUE)
	C_DEFS += -DBL_BCM_TIME_USES_GPT
endif

ifeq ($(BL_ENABLE_PTM), TRUE)
	C_SRCS += $(SRC_DIR)/utils/bl_ptm.c
	C_DEFS += -DBL_ENABLE_PTM
	C_DEFS += -DITBL_MAX_NUM_ENTRIES=$(ITBL_MAX_NUM_ENTRIES)UL
endif

ifeq ($(BL_ENABLE_ICACHE), TRUE)
C_DEFS += -DBL_ENABLE_ICACHE
endif

ifeq ($(BL_ENABLE_DCACHE), TRUE)
C_DEFS += -DBL_ENABLE_DCACHE
endif

ifeq ($(BL_ENABLE_SWR_CONFIG), TRUE)
C_DEFS += -DBL_ENABLE_SWR_CONFIG
endif

ifeq ($(BL_ENABLE_UART_PL011), TRUE)
	C_SRCS += $(SRC_DIR)/drivers/uart/bl_pl011.c
	C_DEFS += -DBL_ENABLE_UART_PL011
endif

ifeq ($(BL_ENABLE_LOG), TRUE)
	C_SRCS += $(SRC_DIR)/utils/bl_log.c
	C_DEFS += -DBL_ENABLE_LOG
endif

ifeq ($(BL_ENABLE_FLASH), TRUE)
    C_SRCS += $(SRC_DIR)/drivers/flash/bl_qspi.c
	C_SRCS += $(SRC_DIR)/drivers/flash/bl_flash.c
	C_DEFS += -DBL_ENABLE_FLASH
endif

ifeq ($(BL_ENABLE_ETH_AMAC), TRUE)
	C_SRCS += $(SRC_DIR)/drivers/ethernet/bl_eth_amac.c
	C_SRCS += $(SRC_DIR)/drivers/ethernet/bl_eth_unimac.c
	C_DEFS += -DBL_ENABLE_ETH_AMAC
endif

ifeq ($(BL_ENABLE_MMI), TRUE)
	C_SRCS += $(SRC_DIR)/drivers/mmi/bl_mmi.c
ifeq ($(CHIP_FAMILY), bcm8953x)
ifeq ($(chip), bcm89559g)
	C_SRCS += $(SRC_DIR)/drivers/ethernet/89881_phy.c
endif
endif
endif

ifeq ($(BL_ENABLE_GPIO), TRUE)
	C_DEFS += -DBL_ENABLE_GPIO
endif

ifeq ($(BL_ENABLE_DDR), TRUE)
	C_DEFS += -DBL_ENABLE_DDR
endif

ifeq ($(BL_ENABLE_TEST), TRUE)
	C_SRCS += $(SRC_DIR)/tests/test.c
	C_DEFS += -DBL_ENABLE_TEST
ifeq ($(BL_ENABLE_FLASH_TEST), TRUE)
	C_SRCS += $(SRC_DIR)/tests/flash_test.c
	C_DEFS += -DBL_ENABLE_FLASH_TEST
endif
ifeq ($(BL_ENABLE_ETH_TEST), TRUE)
	C_SRCS += $(SRC_DIR)/tests/eth_test.c
	C_DEFS += -DBL_ENABLE_ETH_TEST
endif
ifeq ($(BL_ENABLE_PTM_TEST), TRUE)
	C_SRCS += $(SRC_DIR)/tests/ptm_test.c
	C_DEFS += -DBL_ENABLE_PTM_TEST
endif
endif
ifeq ($(BL_ENABLE_PCIE_GEN1), TRUE)
	C_DEFS += -DBL_ENABLE_PCIE_GEN1
endif
ifeq ($(BL_ENABLE_SECURE_BOOT), TRUE)
	C_DEFS += -DBL_ENABLE_SECURE_BOOT
    INCS += -I$(BRCM_SDK_ROOT)/security/rsa/inc
    INCS += -I$(BRCM_SDK_ROOT)/security/sha/inc
    C_SRCS += $(SRC_DIR)/control/bl_security.c
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_rsa.a
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_sha.a
endif

ifeq ($(BL_ENABLE_FLASHLESS_BOOT), TRUE)
	C_DEFS += -DBL_ENABLE_FLASHLESS_BOOT
	C_SRCS += $(SRC_DIR)/control/bl_fw_execute.c
endif

LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_mcudrv.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_arm.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_timdrv.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_time.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_otpdrv.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_console.a
LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_queue.a

ifeq ($(CHIP_FAMILY), bcm8910x)
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_avtdrv.a
endif
ifeq ($(BL_ENABLE_GPIO), TRUE)
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_pinmuxdrv.a
endif
ifeq ($(BL_ENABLE_DDR), TRUE)
    LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_memcdrv.a
endif
ifeq ($(BL_ENABLE_IPC_DWNLD_MODE), TRUE)
	LIBS += $(TARGET_DIR)/../../../out/lib/$(chip)/lib$(chip)_ipcdrv.a
endif

ifeq ($(BL_SECURE_KEY_FROM_PT), TRUE)
	C_DEFS += -DBL_SECURE_KEY_FROM_PT
endif

BRCM_BL_OBJ_DIR ?= $(TARGET_DIR)/obj
C_EXT_OBJS = ${C_EXT_SRCS:$(BRCM_SDK_ROOT)/%.c=$(BRCM_BL_OBJ_DIR)/%.o}
C_OBJS	= ${C_SRCS:$(SRC_DIR)/%.c=$(BRCM_BL_OBJ_DIR)/%.o}
ASM_OBJS = ${ASM_SRCS:$(SRC_DIR)/%.S=$(BRCM_BL_OBJ_DIR)/%.o}
OBJS = $(ASM_OBJS) $(C_OBJS) $(C_EXT_OBJS)
#$(info C_SRCS: $(C_SRCS))
TARGET := $(addprefix $(TARGET_DIR)/,$(TARGET))
#$(info OBJS: $(OBJS))
#$(info TARGET: $(TARGET))
##############################################
# Rules
##############################################

.SECONDARY:
.PHONY: bootloader clean

#.PRECIOUS: $(TARGET_DIR)/%.o $(TARGET_DIR)/%.d
make-depend = mv $1_tmp $1


#$(TARGET_DIR)/%.img: $(TARGET_DIR)/%.bin
include $(BRCM_SDK_ROOT)/bootloader/$(family)/lib.mk

libs: $(BRCM_BL_LIB_TARGET)

bootloader : $(BRCM_BL_LIB_TARGET) $(TARGET)
	$(info Built ${TARGET} at from ${SRC_DIR})

$(TARGET_DIR)/%.bin: $(TARGET_DIR)/%.elf
	$(QUIET)$(OBJCOPY) -O binary $< $@

$(TARGET_DIR)/%.elf : $(BRCM_BL_LIB_TARGET) $(OBJS) $(LD_SCRIPT)
	$(QUIET)$(LD) $(LD_FLAGS) -T $(LD_SCRIPT) $(OBJS) $(BRCM_BL_LIB_TARGET) --start-group $(LIBS) --end-group $(LD_LIBS) -o $@ -Map=$(TARGET_DIR)/$*.map

$(BRCM_BL_OBJ_DIR)/%.o: $(BRCM_SDK_ROOT)/%.c $(BRCM_BL_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) -c $(C_FLAGS) $(CCFLAGS) $(C_DEFS) $(INCS) $(BRCM_DEPFLAGS) -o $@ $<
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

$(BRCM_BL_OBJ_DIR)/%.o: $(SRC_DIR)/%.S $(BRCM_BL_OBJ_DIR)/%.d
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(CC) -c  $(C_DEFS) $(C_FLAGS) $(INCS) $(BRCM_DEPFLAGS) $< -o $@
	$(QUIET)$(call make-depend, $(subst .o,.d,$@))

$(BRCM_BL_OBJ_DIR)/%.d: ;


clean :
	rm -rf $(TARGET_DIR)

dependencies=$(subst .o,.d,$(OBJS))
ifneq ($(MAKECMDGOALS),bl_clean)
-include $(dependencies)
endif
