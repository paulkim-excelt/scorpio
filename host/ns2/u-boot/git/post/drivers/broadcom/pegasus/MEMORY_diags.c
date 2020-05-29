/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * Copied from drivers/post/memory.c
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include "memtest.h"
#include <post.h>
#include <watchdog.h>

#if CONFIG_POST & (CONFIG_SYS_POST_MEMORY | CONFIG_SYS_POST_MEM_REGIONS)

DECLARE_GLOBAL_DATA_PTR;

#define DDR_BANK0				0x80000000ULL
#define DDR_BANK0_SZ			0x80000000ULL
#define PART2_RANK0_START		0x880000000ULL
#define PART2_RANK0_SIZE		0x80000000ULL
#define DDR_BANK1				0x900000000ULL
#define DDR_BANK1_SZ			0x100000000ULL

/*
 * !! this is only valid, if you have contiguous memory banks !!
 */
__attribute__ ((weak))
int arch_memory_test_prepare(unsigned long *vstart, unsigned long *size,
			     phys_addr_t *phys_offset)
{
	bd_t *bd = gd->bd;

	*vstart = DDR_BANK0;
	/* Leave top 64MB for U-boot */
	*size = DDR_BANK0_SZ - (64 * 1024 * 1024);

	/* Limit area to be tested with the board info struct */
	if ((*vstart) + (*size) > (ulong)bd)
		*size = (ulong)bd - *vstart;

	post_log("Memory Range selected: start:0x%lx, end:0x%lx\n", *vstart,
		 *vstart + *size - 1);

	return 0;
}

/*
 * test regions not exposed to U-boot
 */

int arch_memory_test_prepare_high(unsigned long *vstart, unsigned long *size,
				  phys_addr_t *phys_offset)
{
	*vstart = DDR_BANK1;
	*size = DDR_BANK1_SZ;	/* RANK-1 */

	post_log("Memory Range selected: start:0x%lx, end:0x%lx\n", *vstart,
		 *vstart + *size - 1);

	return 0;
}

__attribute__ ((weak))
int arch_memory_test_advance(u32 *vstart, u32 *size,
			     phys_addr_t *phys_offset)
{
	return 1;
}

__attribute__ ((weak))
int arch_memory_test_cleanup(u32 *vstart, u32 *size,
			     phys_addr_t *phys_offset)
{
	return 0;
}

__attribute__ ((weak))
void arch_memory_failure_handle(void)
{
	return;
}


int memory_post_test(int flags)
{
	datum retval1 = 0, *retval2 = NULL;
	phys_addr_t phys_offset = 0, phys_offset_high = 0;
	unsigned long memsize, vstart, memsize_high, vstart_high;
	char *s = getenv("board_name");

	arch_memory_test_prepare(&vstart, &memsize, &phys_offset);
	arch_memory_test_prepare_high(&vstart_high, &memsize_high,
				      &phys_offset_high);

	post_log("vstart : 0x%lx, memsize : 0x%lx\n", vstart, memsize);

	retval1 = memTestDataBus((volatile datum *)vstart);
	if (retval1 != 0)
		return -1;

	retval1 = memTestDataBus((volatile datum *)PART2_RANK0_START);
	if (retval1 != 0)
		return -1;

	retval1 = memTestDataBus((volatile datum *)vstart_high);
	if (retval1 != 0)
		return -1;

	post_log("\n\n**** DDR WALK TEST: RANK-0 ****\n\n");
	retval2 = memTestAddressBus((volatile datum *)vstart, memsize);
	if (retval2)
		return -1;

	retval2 = memTestAddressBus((volatile datum *)PART2_RANK0_START, PART2_RANK0_SIZE);
	if (retval2)
		return -1;

	if ((!strcmp(s, PEGASUS_23MM_BOARD)) ||
			(!strcmp(s, PEGASUS_XMC_BOARD))) {
		post_log("\n\n**** DDR WALK TEST: RANK-1 ****\n\n");
		retval2 = memTestAddressBus((volatile datum *)vstart_high,
				memsize_high);
		if (retval2)
			return -1;
	}

	/* Auto mode will not run slow tests */
	if (flags & POST_AUTO)
		return 0;

	post_log("\n\n**** DDR FULL TEST: RANK-0 ****\n\n");
	retval2 = memTestDevice((volatile datum *)vstart, memsize);
	if (retval2)
		return -1;

	retval2 = memTestDevice((volatile datum *)PART2_RANK0_START, PART2_RANK0_SIZE);
	if (retval2)
		return -1;

	if ((!strcmp(s, PEGASUS_23MM_BOARD)) ||
			(!strcmp(s, PEGASUS_XMC_BOARD))) {
		post_log("\n\n**** DDR FULL TEST: RANK-1 ****\n\n");
		retval2 = memTestDevice((volatile datum *)vstart_high,
				memsize_high);
		if (retval2)
			return -1;
	}

	return (unsigned long int)retval2;
}

#endif /* CONFIG_POST&(CONFIG_SYS_POST_MEMORY|CONFIG_SYS_POST_MEM_REGIONS) */
