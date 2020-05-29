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

#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/arch/bcm_otp.h>

static int do_bcmotp(cmd_tbl_t *cmdtp, int flag, int argc,
		     char *const argv[])
{
	char ch;
	int ret = 0;
	u32 data0;
	u32 addr;
	int len, i;

	if (argc < 2) {
		printf("bcmotp: incomplete arguments\n");
		return -1;
	}

	ch = argv[1][0];

	switch (ch) {
	case 'i':
		ret = otp_init();
		break;

	case 'r':
		if (argc < 3) {
			printf("bcmotp: incomplete arguments\n");
			ret = -1;
			break;
		}

		if (argc < 4)
			len = 1;
		else
			len = simple_strtoul(argv[3], NULL, 16);

		addr = simple_strtoul(argv[2], NULL, 16);
		for (i = 0; i < len; i++) {
			ret = otp_mem_read32(addr + i, &data0);
			if (ret) {
				printf("bcmotp:read@0x%x failed\n", addr + i);
				break;
			}
			printf("bcmotp:data@0x%0x 0x%x\n", addr + i, data0);
		}
		break;

	case 'k':
		data0 = 0;
		ret = otp_read_sku_id(&data0);
		if (!ret)
			printf("bcmotp: skuid: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_revision_id(&data0);
		if (!ret)
			printf("bcmotp: revid: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_device_id(&data0);
		if (!ret)
			printf("bcmotp: devid: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_pcie_stat(&data0);
		if (!ret)
			printf("bcmotp: pcie_stat: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_sata_stat(&data0);
		if (!ret)
			printf("bcmotp: sata_stat: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_usb_stat(&data0);
		if (!ret)
			printf("bcmotp: usb_stat: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_raidxor_stat(&data0);
		if (!ret)
			printf("bcmotp: raidxor_stat: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_audio_stat(&data0);
		if (!ret)
			printf("bcmotp: audio_stat: 0x%x\n", data0);

		data0 = 0;
		ret = otp_read_pcie_config(&data0);
		if (!ret)
			printf("bcmotp: pcie_config: 0x%x\n", data0);

		break;
	}

	return ret;
}

U_BOOT_CMD(bcmotp, 4, 0, do_bcmotp,
	   "Broadcom OTP command",
	   "bcmotp read <start> <len>\n"
	   "bcmotp knownvals\n"
	   "bcmotp init\n"
);
