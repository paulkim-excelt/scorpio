/*
 * (C) Copyright 2010,2011
 * NVIDIA Corporation <www.nvidia.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/socregs.h>
#include <common.h>
#include <linux/types.h>
#include <linux/ctype.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch-iproc/sys_proto.h>

void iproc_get_soc_id(u32 *id, u32 *rev)
{
	u32 soc_id, soc_rev;


	soc_id = reg32_read((u32 *)ICFG_CHIP_ID_REG);
	soc_id = soc_id & ICFG_CHIP_ID_REG_READMASK;

	soc_rev = reg32_read((u32 *)ICFG_CHIP_REVISION_ID);
	soc_rev = soc_rev & ICFG_CHIP_REVISION_ID_READMASK;

	switch (soc_id) {
	case 0xd300:
	case 0xe711:
	case 0xd712:
	case 0xd713:
	case 0xd715:
		*id = IPROC_SOC_NS2;
		switch (soc_rev) {
		case 0x01:
			*rev = IPROC_SOC_NS2_A0;
			break;
		case 0x02:
			*rev = IPROC_SOC_NS2_A1;
			break;
		case 0x11:
			*rev = IPROC_SOC_NS2_B0;
			break;
		default:
			*rev = 0;
		}
		break;
	case 0x2025340:
		*id = IPROC_SOC_PEGASUS;
	default:
		*id = 0;
		*rev = 0;

	}
}

/* Print CPU information */
#if defined(CONFIG_DISPLAY_CPUINFO)
int print_cpuinfo(void)
{
	char soc_info[12];

	memset(soc_info, 0, sizeof(soc_info));
	strncpy(soc_info, CONFIG_SYS_SOC, 11);
	printf("%s\n", soc_info);

	return 0;
}
#endif
