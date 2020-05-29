/*
 * Copyright (C) 2016 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __SMC_CALL_H__
#define __SMC_CALL_H__
#include <linux/linkage.h>

asmlinkage int __invoke_fn_smc(u64, u64, u64, u64);
#endif
