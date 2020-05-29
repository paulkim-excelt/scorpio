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
#include <asm/arch/smc_call.h>

#define OEM_PVTMON_OPS			0xC300FF0C

enum oem_pvtmon_ops {
	/*
	 * ops aligned with M0 user handler's opcodes
	 * however, only few are supported now.
	 */
	OEM_PVTMON_SET_POWER_MAP = 0,
	OEM_PVTMON_ENABLE_SYS_AVS,
	OEM_PVTMON_SET_FREQ_MAP,
	OEM_PVTMON_PREDICT_AVS,
	OEM_PVTMON_CONFIG_PLL_VCO,
	OEM_PVTMON_CONFIG_PLL_MDIV,
	OEM_PVTMON_RESET_BLOCK,
	OEM_PVTMON_RESET_PROCESSOR,
	OEM_PVTMON_ENABLE_POWER,
	OEM_PVTMON_CONFIG_WDT,
	OEM_PVTMON_CONFIG_BIMC,
	OEM_PVTMON_POLL_BIMC,
	OEM_PVTMON_GET_PVT_TEMP,
	OEM_PVTMON_GET_PVT_PROCESS,
	OEM_PVTMON_GET_PVT_VOLTAGE,
	OEM_PVTMON_GET_CENTRAL_RO_FREQ,
	OEM_PVTMON_GET_REMOTE_RO_FREQ,
	OEM_PVTMON_GET_MIN_RO_FREQ,
	OEM_PVTMON_GET_MAX_RO_FREQ,
	OEM_PVTMON_END
};

enum avs_pvt_t {
	PVT_TEMPERATURE = 0, /* Bit 0 - Temperature measurement */
	PVT_0P85V_0     = 1, /* Bit 1 - Voltage 0p85V<0> measurement */
	PVT_0P85V_1     = 2, /* Bit 2 - Voltage 0p85V<1> measurement */
	PVT_1V_0        = 3, /* Bit 3 - Voltage 1V<0> measurement */
	PVT_1V_1        = 4, /* Bit 4 - Voltage 1V<1> measurement */
	PVT_1P8V        = 5, /* Bit 5 - Voltage 1p8V measurement */
	PVT_3P3V        = 6, /* Bit 6 - Voltage 3p3V measurement */
	PVT_TESTMODE    = 7  /* Bit 7 - Testmode measurement */
};

static int get_process(u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_PVTMON_OPS, OEM_PVTMON_GET_PVT_PROCESS, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

static int get_temp(int *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_PVTMON_OPS, OEM_PVTMON_GET_PVT_TEMP, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

static int get_voltage(u32 domain, u32 *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_PVTMON_OPS, OEM_PVTMON_GET_PVT_VOLTAGE,
			      domain, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("%s() failed\n", __func__);
	return ret;
}

static int do_get_pvt(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = 0;
	u32 process, voltage = 0;
	int celsius = 0;
	int i, count;

	if (argc != 2)
		return CMD_RET_USAGE;

	count = simple_strtoul(argv[1], NULL, 16);

	if (get_process(&process)) {
		printf("pvt: failed to get temperature\n");
		ret = -1;
		goto out;
	}

	for (i = 0; i < count; i++) {
		if (!get_temp(&celsius)) {
			/* Milli celsius to celsius with round up */
			celsius = (celsius + 500) / 1000;
		} else {
			printf("pvt: failed to get temperature\n");
			ret = -1;
			break;
		}

		if (!get_voltage(PVT_1V_0, &voltage)) {
			/* 1/10 milli volts to volts with round up */
			voltage = (voltage + 55) / 10;
		} else {
			printf("pvt: failed to get voltage\n");
			ret = -1;
			break;
		}

		printf("pvt:\t P: %d\t| V: %d.%02dV\t| T: %dC\n", process,
		       (voltage / 1000), ((voltage % 1000) / 10), celsius);

		mdelay(1000);
	}
out:
	return ret;
}

U_BOOT_CMD(pvt,	2, 0, do_get_pvt,
	   "Broadcom pvt monitor - gets temperature and voltage details",
	   "<count>\n"
);
