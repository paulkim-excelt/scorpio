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

#include <common.h>
#include <asm/errno.h>
#include <asm/arch/bcm_otp.h>
#include <asm/arch/smc_call.h>

int32_t otp_mem_read32(u32 addr, u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ, addr, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("otp_mem_read32() failed\n");
	return ret;
}

int32_t otp_read_sku_id(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_SKUID, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_revision_id(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_REVID, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_device_id(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_DEVID, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_pcie_stat(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_PCIE_STAT, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_sata_stat(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_SATA_STAT, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

/* Port number starts with 0. Pegasus has #2 SATA controllers
 * returns 0 if not enabled else 1 if enabled.
 */
u32 is_sata_port_enabled(u32 port)
{
	u32 num_port;
	u32 ret;

	if (port >= 2) {
		printf("%s(): Invalid Port %d\n", __func__, port);
		return 0;
	}

	ret = otp_read_sata_stat(&num_port);
	if (ret < 0) {
		printf("%s(): SMC call otp_read_sata_stat failed\n", __func__);
		return 0;
	}

	if ((1 << port) & num_port)
		return 0;

	return 1;
}

/* According to the OTP values, returns the number of SATA ports */
u32 get_num_stat_ports(void)
{
	u32 count = 0;
	u32 num_port;
	u32 ret;

	ret = otp_read_sata_stat(&num_port);
	if (ret < 0) {
		printf("%s(): SMC call otp_read_sata_stat failed\n", __func__);
		return -EINVAL;
	}

	while (num_port) {
		num_port &= (num_port - 1);
		count++;
	}

	return count;
}

u32 is_usb_subsys_supported(void)
{
#ifdef BCM_OTP_CHECK_ENABLED
	u32 status;

	otp_read_usb_stat(&status);
	if (status & (1 << 0))
		return 0;
	else
		return 1;
#else
	return 1;
#endif
}

int32_t otp_read_usb_stat(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_USB_STAT, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_raidxor_stat(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_RAIDXOR_STAT, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_audio_stat(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_AUDIO_STAT, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_read_pcie_config(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_READ_PCIE_CONFIG, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

int32_t otp_init(void)
{
	int ret;

	ret = __invoke_fn_smc(OEM_OTP_OPS, OEM_OTP_OPS_INIT, 0, 0);
	if (ret < 0)
		printf("otp_init() failed\n");

	return ret;
}
