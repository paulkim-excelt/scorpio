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

#ifndef __BCM_SECURE_REG_OPS_H__
#define __BCM_SECURE_REG_OPS_H__

#define OEM_SECURE_REG_OPS			0xC300FF0A

enum secure_reg_ops {
	OEM_SECURE_REG_OPS_READ = 0,
	OEM_SECURE_REG_OPS_WRITE,
};

int32_t smc_mem_read32(u32 addr);
void smc_mem_write32(u32 addr, u32 data);

#endif /* __BCM_SECURE_REG_OPS_H__ */
