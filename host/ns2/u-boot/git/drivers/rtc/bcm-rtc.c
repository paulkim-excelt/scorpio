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
#include <rtc.h>
#include <asm/arch/smc_call.h>

#if defined(CONFIG_CMD_DATE)

#define OEM_RTC_BBL_INIT                0xC300FF04
#define OEM_RTC_TIME_REQ                0xC300FF06

enum oem_rtc_time_req {
	OEM_RTC_TIME_REQ_READ = 0,
	OEM_RTC_TIME_REQ_SET,
};

/*
 * Does the rtc_time represent a valid date/time?
 */
static int rtc_valid_tm(struct rtc_time *tm)
{
	if (tm->tm_year < 1970 ||
	    ((unsigned)tm->tm_mon) > 12 ||
	    tm->tm_mday < 1 ||
	    ((unsigned)tm->tm_hour) >= 24 ||
	    ((unsigned)tm->tm_min) >= 60 ||
	    ((unsigned)tm->tm_sec) >= 60
	   )
		return -1;
	return 0;
}

static int bbl_init(void)
{
	int ret;

	ret = __invoke_fn_smc(OEM_RTC_BBL_INIT, 0, 0, 0);
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("bcm-iproc-rtc: %s() failed\n", __func__);
	return ret;
}

static int32_t rtc_get_time(unsigned long long *data0)
{
	int ret;

	register int smc_retdata asm ("x1");

	ret = __invoke_fn_smc(OEM_RTC_TIME_REQ, OEM_RTC_TIME_REQ_READ, 0, 0);
	/* EL3 service will store read data in x1 reg */
	*data0 = smc_retdata;
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("bcm-iproc-rtc: %s() failed\n", __func__);
	return ret;
}

static int32_t rtc_set_time(unsigned long long data0)
{
	int ret;

	ret = __invoke_fn_smc(OEM_RTC_TIME_REQ, OEM_RTC_TIME_REQ_SET, data0, 0);
	if (ret < 0)
		goto err;

	return 0;
err:
	printf("bcm-iproc-rtc: %s() failed\n", __func__);
	return ret;
}

int rtc_get(struct rtc_time *tmp)
{
	unsigned long long seconds;
	int ret = 0;

	ret = bbl_init();
	if (ret < 0) {
		printf("bcm-iproc-rtc: rtc init failed\n");
		return ret;
	}

	ret = rtc_get_time(&seconds);
	if (ret < 0) {
		printf("bcm-iproc-rtc: rtc get time failed\n");
		return ret;
	}

	rtc_to_tm(seconds, tmp);
	ret = rtc_valid_tm(tmp);
	if (ret == -1) {
		printf("bcm-iproc-rtc: invalid data for date\n");
		return ret;
	}

	return ret;
}

int rtc_set(struct rtc_time *tmp)
{
	unsigned long long t;
	int ret;

	t = rtc_mktime(tmp);
	ret = rtc_set_time(t);
	if (ret < 0)
		printf("bcm-iproc-rtc: rtc set time failed\n");

	return ret;
}

void rtc_reset(void)
{
}

#endif
