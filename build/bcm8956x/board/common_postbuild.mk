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
# File Name: Makefile
# DESCRIPTION: Makefile to be invoked while running "make". This would do initial
#              pre-requisite which is required to build products and invoke
#              product makefile to build accordingly.
#####################################################################################

BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/bcm8956x_$(BRCM_SYSCFG_XCVR_CFG).bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/userdata.txt
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_bl_signed.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_bl.elf
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_bl.map
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_bl.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).cmm
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_flash_erase.cmm
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).text.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.privileged.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.bin
BRCM_APP_TARGET_DEPS += $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).eth_ro.bin

BRCM_PT_CREATOR_OPTS  = --sector_sz=$(BRCM_FLASH_IMAGE_ALIGN_SIZE) --pt_base=$(PT_BASE_ADDR) --pla 0x80 --edc 1
BRCM_PT_CREATOR_OPTS += --repo $(BRCM_SDK_ROOT) --outdir $(BRCM_APP_OBJ_DIR)/images --out $(board)_$(comp)_$(OS)
BRCM_PT_CREATOR_OPTS += --max_num_img_entries $(ITBL_MAX_NUM_ENTRIES)
BRCM_PT_CREATOR_OPTS += --bl_cnt $(BRCM_BL_NUM_COPIES) --bl_cfg $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt --bl_entry_pt $(BRCM_BL_EP)
BRCM_PT_CREATOR_OPTS += --fw_cnt $(BRCM_FW_NUM_COPIES) --fw_cfg $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt --fw_entry_pt $(BRCM_FW_EP)
BRCM_PT_CREATOR_OPTS += --syscfg_cnt $(BRCM_SYSCFG_NUM_COPIES) --sys_cfg $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
BRCM_PT_CREATOR_OPTS += --userdata $(BRCM_APP_OBJ_DIR)/images/userdata.txt
ifeq ($(ROM_SUPPORTS_SECURE_BOOT),TRUE)
BRCM_PT_CREATOR_OPTS += --bl_sign 1
endif

ifeq ($(BL_SUPPORTS_SECURE_BOOT),TRUE)
BRCM_PT_CREATOR_OPTS += --pub_key $(BRCM_SDK_ROOT)/build/keys/public_key.bin
BRCM_PT_CREATOR_OPTS += --priv_key $(BRCM_SDK_ROOT)/build/keys/private_key.dat
endif

BRCM_APP_CFG_DEP :=
ifneq ($(wildcard $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml),)
BRCM_APP_CFG_DEP += $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml
endif

$(BRCM_APP_OBJ_DIR)/images/bcm8956x_$(BRCM_SYSCFG_XCVR_CFG).bin: $(BRCM_SDK_ROOT)/ethernet/drivers/transceiver/config/bcm8956x/bcm8956x_$(BRCM_SYSCFG_XCVR_CFG).bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $< $@

%/images/bl_cfg.txt %/images/sys_cfg.txt %/images/app_cfg.txt: $(BRCM_APP_CFG_DEP) $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf $(BRCM_APP_OBJ_DIR)/images/$(board)_bl.map
	$(QUIET)mkdir -p $(dir $@)
	$(eval BRCM_BL_LA := $(word 1,$(shell grep -R __text_start__ $(BRCM_APP_OBJ_DIR)/images/$(board)_bl.map)))
	$(QUIET)echo "0x0 $(BL_MAX_SIZE) $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_bl_signed.bin) $(BRCM_BL_LA)" > $(BRCM_APP_OBJ_DIR)/images/bl_cfg.txt
	$(QUIET)echo "0x9000 0x1000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/bcm8956x_$(BRCM_SYSCFG_XCVR_CFG).bin)" > $(BRCM_APP_OBJ_DIR)/images/sys_cfg.txt
	$(QUIET)echo "0x0 0x30000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).text.bin) $(BRCM_FW_EXE0_LA)" > $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)echo "0x1 0x10000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.privileged.bin) $(BRCM_FW_EXE1_LA)" >> $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)echo "0x2 0x30000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).data.bin) $(BRCM_FW_EXE2_LA)" >> $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)echo "0x3 0x1000 $(subst $(BRCM_SDK_ROOT)/,,$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).eth_ro.bin) $(BRCM_FW_EXE3_LA)" >> $(BRCM_APP_OBJ_DIR)/images/app_cfg.txt
	$(QUIET)$(BRCM_SDK_ROOT)/build/common/xmlparse.py -a -b $(BRCM_APP_OBJ_DIR)/application.xml -c $(BRCM_COMP_DIR)/$(BRCM_APP_CFG_XML).xml -s $(BRCM_SDK_ROOT)/build/common/schema.xsd -o $(BRCM_APP_OBJ_DIR)/images -r $(BRCM_SDK_ROOT)

$(BRCM_APP_OBJ_DIR)/images/userdata.txt:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)echo "0xBC10" >> $@

FORCE:

$(BRCM_APP_OBJ_DIR)/images/$(board)_bl.elf: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(board)_bl.elf $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_bl.map: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(board)_bl.map $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_bl.bin: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_BOOTLOADER_DIR)/$(board)_bl.bin $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_bl_signed.bin: $(BRCM_APP_PREBUILDS)
	$(QUIET)mkdir -p $(dir $@)
	python $(BRCM_SDK_ROOT)/build/common/sign_img.py -i $(BRCM_BOOTLOADER_DIR)/$(board)_bl.bin -o $@ -k $(BRCM_SDK_ROOT)/build/keys/private_key.dat -f 0


$(BRCM_APP_OBJ_DIR)/images/$(board)_flash_erase.cmm: $(BRCM_APP_PREBUILDS) $(BRCM_SDK_ROOT)/build/bcm8956x/bcm8956x_flash_erase.cmm
	$(eval BL_DWNLD_ADDR := $(word 1,$(shell grep -R BL_DWNLD_StartAddr $(BRCM_BOOTLOADER_DIR)/$(board)_bl.map)))
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_SDK_ROOT)/build/bcm8956x/bcm8956x_flash_erase.cmm $@
	$(QUIET)sed -i 's/BOOTLOADER_ELF_NAME/$(board)_bl.elf/g' $@
	$(QUIET)sed -i 's/BOOTLOADER_IMG_DWNLD_ADDR/$(BL_DWNLD_ADDR)/g' $@
	$(QUIET)sed -i 's/APPLICATION_IMG_NAME/$(board)_$(comp)_$(OS).img/g' $@

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).cmm: $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf $(BRCM_SDK_ROOT)/build/bcm8956x/bcm8956x_app_debug.cmm
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_SDK_ROOT)/build/bcm8956x/bcm8956x_app_debug.cmm $@
	$(QUIET)sed -i 's/APP_ELF_NAME/$(board)_$(comp)_$(OS).elf/g' $@


$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf: $(BRCM_APP_OBJ_DIR)/c_rX.bin
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(BRCM_APP_OBJ_DIR)/c_rX.elf $@
	$(QUIET)cp $(BRCM_APP_OBJ_DIR)/c_rX.map $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).map
	$(eval BRCM_FW_EXE0_LA := $(word 1,$(shell grep -R __text_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE1_LA := $(word 1,$(shell grep -R __stack_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE2_LA := $(word 1,$(shell grep -R __data_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_FW_EXE3_LA := $(word 1,$(shell grep -R __eth_ro_start__ $(BRCM_APP_OBJ_DIR)/c_rX.map)))

$(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).%.bin: $(BRCM_APP_OBJ_DIR)/images/$(board)_$(comp)_$(OS).elf
	$(QUIET)$(eval BRCM_TMP_VAR= $(subst .bin,,$(subst $(board)_$(comp)_$(OS),, $(shell basename $@ ))))
	$(QUIET)$(OBJCOPY) --output-target binary --only-section $(BRCM_TMP_VAR) $< $@

$(BRCM_APP_TARGET): prebuilds $(BRCM_APP_TARGET_DEPS)
	$(QUIET)mkdir -p $(dir $@)
	$(eval BRCM_FW_EP := $(word 1,$(shell grep -R EE_cortex_mx_default_reset_ISR $(BRCM_APP_OBJ_DIR)/c_rX.map)))
	$(eval BRCM_BL_EP := $(word 1,$(shell grep -R CORTEX_MX_RESET_HANDLER $(BRCM_BOOTLOADER_DIR)/$(board)_bl.map)))
	python $(BRCM_SDK_ROOT)/build/common/pt_creator.py $(BRCM_PT_CREATOR_OPTS)
	$(QUIET)cd $(BRCM_APP_OBJ_DIR)/images;tar -czf $@ *;cd -

