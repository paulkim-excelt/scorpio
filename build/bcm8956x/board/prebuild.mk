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
# File Name: prebuild.mk
# DESCRIPTION: makefile to generate prebuild files required for build.
#####################################################################################

BRCM_APP_PREBUILDS = $(BRCM_BOOTLOADER_DIR)/$(board)_bl.bin
BRCM_APP_PREBUILDS += $(BRCM_BOOTLOADER_DIR)/$(board)_bl.elf
ifneq ($(cust),1)
BRCM_CHIP_HOSTAPPS :=
$(eval $(call brcm_calc_chip_hostapps,BRCM_CHIP_HOSTAPPS))
$(info BRCM_CHIP_HOSTAPPS: $(BRCM_CHIP_HOSTAPPS))
BRCM_CHIP_HOSTAPPS_LIST := $(call brcm_chip_hostapp_from_comp,$(BRCM_CHIP_HOSTAPPS))
$(info BRCM_CHIP_HOSTAPPS_LIST: $(BRCM_CHIP_HOSTAPPS_LIST))
#BRCM_APP_PREBUILDS += $(BRCM_CHIP_HOSTAPPS_LIST)
BRCM_APP_PREBUILDS += $(BRCM_HOST_DIR)/ns2/uImage
BRCM_APP_PREBUILDS += $(BRCM_HOST_DIR)/ns2/ns2-xmc.dtb
BRCM_APP_PREBUILDS += $(BRCM_HOST_DIR)/ns2/ramdisk.cpio.gz.u-boot
BRCM_APP_PREBUILDS += $(BRCM_HOST_DIR)/ns2/rootfs.ubi

HOST_LINUX_SRC_DIR = $(BRCM_SDK_ROOT)/host/ns2/kernel/git
HOST_ROOTFS_SRC_DIR = $(BRCM_SDK_ROOT)/host/ns2/rootfs
UTIL_SRC_DIR = $(BRCM_SDK_ROOT)/host/utils
#TEST_SRC_DIR = $(UTIL_SRC_DIR)/tests 
UBOOT_SRC_DIR := $(BRCM_SDK_ROOT)/host/ns2/u-boot/git

HOST_LINUX_OBJ_DIR = $(BRCM_HOST_OBJ_DIR)/ns2/kernel
HOST_ROOTFS_OBJ_DIR = $(BRCM_HOST_OBJ_DIR)/ns2/rootfs
UTIL_OBJ_DIR = $(BRCM_HOST_OBJ_DIR)/utils
#TEST_OBJ_DIR = $(UTIL_OBJ_DIR)/tests
ETH_XCVR_OBJ_DIR = $(TEST_OBJ_DIR)/eth_xcvr
UBOOT_OBJ_DIR := $(BRCM_HOST_OBJ_DIR)/ns2/u-boot
TOOLS_COMMON_DIR := $(BRCM_SDK_ROOT)/tools/common

HOST_TEST_TARGETS =
HOST_TEST_TARGETS += $(ETH_XCVR_OBJ_DIR)/eth_xcvr

BRCM_APP_PREBUILDS += $(BRCM_CHIP_HOSTAPPS_LIST)

HOST_TOOLCHAIN?=/local_tools/compiler/gcc-linaro-7.2.1-2017.11-i686_aarch64-linux-gnu/bin/aarch64-linux-gnu-
endif #($(cust),1)
#$(info BRCM_APP_PREBUILDS: $(BRCM_APP_PREBUILDS))
PYTHON = $(QUIET)python

# Align bootloader to 4k boundary
BRCM_BL_ALIGN = 0x1000
BL_IMG_CREATOR_ARGS = --edc 1 --la 0x6c000 --bl_cnt $(BRCM_BOARD_BL_NUM_COPIES) --align $(BRCM_BL_ALIGN)

$(BRCM_BOOTLOADER_DIR)/$(board)_bl.bin: $(BRCM_CHIP_LIBS_LIST)
	$(QUIET)$(MAKE) -C $(BRCM_SDK_ROOT)/bootloader/build TARGET_DIR=$(BRCM_BOOTLOADER_DIR) VERBOSE=$(VERBOSE) BL_NUM_COPIES=$(BRCM_BOARD_BL_NUM_COPIES) ITBL_MAX_NUM_ENTRIES=$(ITBL_MAX_NUM_ENTRIES)

$(BRCM_BOOTLOADER_DIR)/$(board)_bl.elf: $(BRCM_BOOTLOADER_DIR)/$(board)_bl.bin

ifneq ($(cust),1)
$(BRCM_HOST_DIR)/bcmutil: FORCE
	$(QUIET)$(MAKE) all -f hostapp.mk chip=$(chip) comp=$(call brcm_chip_comp_from_hostapp,$@) VERBOSE=$(VERBOSE) \
	BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP) SPIUTIL_IMAGE_VERSION=$(SW_IMAGE_VERSION) HOST_TOOLCHAIN=$(HOST_TOOLCHAIN)

$(BRCM_HOST_DIR)/bcmutild: $(BRCM_HOST_DIR)/bcmutil
	$(QUIET)$(MAKE) all -f hostapp.mk chip=$(chip) comp=$(call brcm_chip_comp_from_hostapp,$@) VERBOSE=$(VERBOSE) \
	BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP) SPIUTIL_IMAGE_VERSION=$(SW_IMAGE_VERSION) HOST_TOOLCHAIN=$(HOST_TOOLCHAIN)

$(BRCM_HOST_DIR)/eth_xcvr_ut: FORCE
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(MAKE) all -f hostapp.mk chip=$(chip) comp=$(call brcm_chip_comp_from_hostapp,$@) VERBOSE=$(VERBOSE) \
	BRCM_ALL_COMP_MAP=$(BRCM_ALL_COMP_MAP) SPIUTIL_IMAGE_VERSION=$(SW_IMAGE_VERSION) HOST_TOOLCHAIN=$(HOST_TOOLCHAIN)

$(HOST_LINUX_OBJ_DIR)/.config:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(MAKE) -C $(HOST_LINUX_SRC_DIR) ARCH=arm64 CROSS_COMPILE=$(HOST_TOOLCHAIN) KBUILD_OUTPUT=$(dir $@) iproc_defconfig

#Once the kernel is built, even if there is a change in sources, it won't be
#built since we have not imposed FORCE target for vmlinux. Reason for not doing
#so is that when the build is executed in parallel for multiple applications,
#kernel build is invoked multiple times and each one of them tries to
#remove/replace certain files and that causes build failure.
$(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/Image: $(HOST_LINUX_OBJ_DIR)/.config
	$(QUIET)$(MAKE) -C $(HOST_LINUX_SRC_DIR) ARCH=arm64 CROSS_COMPILE=$(HOST_TOOLCHAIN) KBUILD_OUTPUT=$(HOST_LINUX_OBJ_DIR) -j32


$(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/bcmutild: $(BRCM_CHIP_HOSTAPPS_LIST)
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)
	$(QUIET)cp -Rf $(HOST_ROOTFS_SRC_DIR)/cpio $(HOST_ROOTFS_OBJ_DIR)
	$(QUIET)cp $(BRCM_HOST_DIR)/bcmutil $(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/
	$(QUIET)cp $(BRCM_HOST_DIR)/bcmutild $(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/
	$(QUIET)cp $(UTIL_SRC_DIR)/tests/iperf/bin/iperf3-arm $(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/dev
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/media
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/mnt/.psplash
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/proc
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/run
	$(QUIET)mkdir -p $(HOST_ROOTFS_OBJ_DIR)/cpio/sys

$(HOST_ROOTFS_OBJ_DIR)/rootfs.ubi: $(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/bcmutild
	$(QUIET)cp $(HOST_ROOTFS_SRC_DIR)/ubinize-rootfs.cfg $(dir $@)
	$(QUIET)echo "image="$(HOST_ROOTFS_OBJ_DIR)"/rootfs.ubifs" >> $(dir $@)/ubinize-rootfs.cfg
	$(QUIET)$(TOOLS_COMMON_DIR)/mkfs.ubifs -q -r $(dir $@)/cpio -m 4096 -e 0x3E000 -c 4096 -o $(dir $@)/rootfs.ubifs
	$(QUIET)$(TOOLS_COMMON_DIR)/ubinize -o $@ -m 4096 -p 0x40000 $(dir $@)/ubinize-rootfs.cfg

$(HOST_ROOTFS_OBJ_DIR)/ramdisk.cpio.gz.u-boot: $(UBOOT_OBJ_DIR)/tools/mkimage $(HOST_ROOTFS_OBJ_DIR)/cpio/home/root/bcmutild
	$(QUIET)cd $(HOST_ROOTFS_OBJ_DIR)/cpio; find . | fakeroot cpio -o -H newc | fakeroot gzip -9 > ../ramdisk.cpio.gz
	$(QUIET)$(UBOOT_OBJ_DIR)/tools/mkimage -A arm64 -O linux -T ramdisk -C none -a 0x00000000 -n "Image Name" -d $(HOST_ROOTFS_OBJ_DIR)/ramdisk.cpio.gz $@

$(UBOOT_OBJ_DIR)/.config:
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(MAKE) -C $(UBOOT_SRC_DIR) CROSS_COMPILE=$(HOST_TOOLCHAIN) KBUILD_OUTPUT=$(UBOOT_OBJ_DIR) northstar2_defconfig

$(UBOOT_OBJ_DIR)/tools/mkimage: $(UBOOT_OBJ_DIR)/.config
	$(QUIET)$(MAKE) -C $(UBOOT_SRC_DIR) CROSS_COMPILE=$(HOST_TOOLCHAIN) KBUILD_OUTPUT=$(UBOOT_OBJ_DIR) -j32

$(BRCM_HOST_DIR)/ns2/uImage: $(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/Image $(UBOOT_OBJ_DIR)/tools/mkimage
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)$(UBOOT_OBJ_DIR)/tools/mkimage -A arm64 -O linux -T multi -n iProcLDK_src_4.0.0 -e 0x80080000 -a 0x80080000 -C none -d $(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/Image $@

$(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/dts/broadcom/northstar2/ns2-xmc.dtb: $(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/Image

$(BRCM_HOST_DIR)/ns2/ns2-xmc.dtb: $(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/dts/broadcom/northstar2/ns2-xmc.dtb
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(HOST_LINUX_OBJ_DIR)/arch/arm64/boot/dts/broadcom/northstar2/ns2-xmc.dtb $(dir $@)

$(BRCM_HOST_DIR)/ns2/rootfs.ubi: $(HOST_ROOTFS_OBJ_DIR)/rootfs.ubi
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(HOST_ROOTFS_OBJ_DIR)/rootfs.ubi $(dir $@)

$(BRCM_HOST_DIR)/ns2/ramdisk.cpio.gz.u-boot: $(HOST_ROOTFS_OBJ_DIR)/ramdisk.cpio.gz.u-boot
	$(QUIET)mkdir -p $(dir $@)
	$(QUIET)cp $(HOST_ROOTFS_OBJ_DIR)/ramdisk.cpio.gz.u-boot $(dir $@)

endif #($(cust),1)

