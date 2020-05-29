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
#include <asm/arch/bcm_tamper.h>
#include <asm/arch/smc_call.h>
#include <asm/arch/bcm_secure_reg_ops.h>

int32_t tamper_mem_read32(u32 addr)
{
	int ret_status;
	u32 data0;

	register int smc_retdata asm("x1");

	ret_status = __invoke_fn_smc(OEM_SECURE_REG_OPS,
				     OEM_SECURE_REG_OPS_READ, addr, 0);
	/* EL3 service will store read data in x1 reg */
	data0 = smc_retdata;
	if (ret_status < 0)
		goto err;

	/*printf("read addr:%x :: status:%x smc_retdata:%x\n", addr,
	       ret_status, data0);*/
	return data0;
err:
	printf("tamper_mem_read32() failed\n");
	return ret_status;
}

void tamper_mem_write32(u32 addr, u32 data0)
{
	int ret;

	/*printf("write .....addr: %x :: data: %x\n", addr, data0);*/
	ret = __invoke_fn_smc(OEM_SECURE_REG_OPS,
			      OEM_SECURE_REG_OPS_WRITE, addr, data0);
	if (ret < 0)
		printf("tamper_mem_write32() failed\n");
	ret = tamper_mem_read32(addr);
}
