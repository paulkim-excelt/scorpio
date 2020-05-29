/*
 * (C) Copyright 2016  Broadcom Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _SYS_PROTO_H_
#define _SYS_PROTO_H_

/* SOC ids */
#define IPROC_SOC_NS2		0xd712
#define IPROC_SOC_PEGASUS	0x25340

/* SOC revision ids */
#define IPROC_SOC_NS2_A0	0xd7120001
#define IPROC_SOC_NS2_A1	0xd7120002
#define IPROC_SOC_NS2_B0	0xd7120011

/*
 * iproc_soc_id() - Get the soc and revision id
 */
void iproc_get_soc_id(u32 *id, u32 *rev);
#endif
