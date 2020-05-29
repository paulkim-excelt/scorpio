/*
 * Copyright 2016 Broadcom
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation (the "GPL").
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License version 2 (GPLv2) for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 (GPLv2) along with this source code.
 */

#ifndef _BCM_OTP_
#define _BCM_OTP_

#define OEM_OTP_OPS			0xC300FF09

enum otp_ops {
	OEM_OTP_OPS_INIT = 0,
	OEM_OTP_OPS_READ,
	OEM_OTP_OPS_READ_SKUID,
	OEM_OTP_OPS_READ_REVID,
	OEM_OTP_OPS_READ_DEVID,
	OEM_OTP_OPS_READ_PCIE_STAT,
	OEM_OTP_OPS_READ_SATA_STAT,
	OEM_OTP_OPS_READ_USB_STAT,
	OEM_OTP_OPS_READ_RAIDXOR_STAT,
	OEM_OTP_OPS_READ_AUDIO_STAT,
	OEM_OTP_OPS_READ_PCIE_CONFIG,
};

int32_t otp_mem_read32(u32 addr, u32 *data0);
int32_t otp_init(void);
/* data0[2:0] = sku id data */
int32_t otp_read_sku_id(u32 *data0);
/* data0[7:0] = revision id */
int32_t otp_read_revision_id(u32 *data0);
/* data0[31:0] = device id*/
int32_t otp_read_device_id(u32 *data0);
/*
 * data0[4:0] = pcie ip disable status
 * bit[0] = otp_strap_ip_disable_pcie0
 * bit[1] = otp_strap_ip_disable_pcie1
 * bit[2] = otp_strap_ip_disable_pcie2
 * bit[3] = otp_strap_ip_disable_pcie3
 * bit[4] = otp_strap_ip_disable_pcie4
 */
int32_t otp_read_pcie_stat(u32 *data0);
/*
 * data0[1:0] = sata ip disable status
 * bit[0] = otp_strap_ip_disable_sata0
 * bit[1] = otp_strap_ip_disable_sata1
 */
int32_t otp_read_sata_stat(u32 *data0);
/*
 * data0[2:0] = usb ip disable status
 * bit[0] = otp_strap_ip_usb_subsystem
 * bit[1] = otp_strap_ip_usb2phy_lane0
 * bit[2] = otp_strap_ip_usb2phy_lane1
 */
int32_t otp_read_usb_stat(u32 *data0);
/* data0[0:0] = raid xor ip disable status */
int32_t otp_read_raidxor_stat(u32 *data0);
/* data0[0:0] = audio ip disable status */
int32_t otp_read_audio_stat(u32 *data0);
/* data0[1:0] = otp_strap_pcie_configuration */
int32_t otp_read_pcie_config(u32 *data0);

u32 is_sata_port_enabled(u32 port);

/* Return the number of SATA ports */
u32 get_num_stat_ports(void);

u32 is_usb_subsys_supported(void);
#endif /* _BCM_OTP_ */
