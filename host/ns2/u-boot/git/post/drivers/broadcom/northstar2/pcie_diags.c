/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 */

#include <common.h>
#include <environment.h>
#include <post.h>
#include <linux/types.h>
#include <asm/io.h>
#include "mdio_wrappers.h"

#define MAX_RETRY_COUNT			1
#define PCIE_A_HWADDR			0x20020000
#define PCIE_B_HWADDR			0x50020000
#define EP_PERST_SOURCE_SELECT_SHIFT	2
#define EP_PERST_SOURCE_SELECT		(1 << EP_PERST_SOURCE_SELECT_SHIFT)
#define EP_MODE_SURVIVE_PERST_SHIFT	1
#define EP_MODE_SURVIVE_PERST		(1 << EP_MODE_SURVIVE_PERST_SHIFT)
#define RC_PCIE_RST_OUTPUT_SHIFT	0
#define RC_PCIE_RST_OUTPUT		(1 << RC_PCIE_RST_OUTPUT_SHIFT)

/* X3
 * PCIE serdes registers
 * General PCIE blocks
 */
#define PCIE1_BLK_ADR		0x1100
#define PCIE3_BLK_ADR		0x1300
#define PCIE4_BLK_ADR		0x1400
#define PCIE5_BLK_ADR		0x1500

/* RX DFE Broadcast */
#define RX_DFE0_BC_BLK_ADR	0xf700

/* PCIE1 registers */
#define XGXS_STATUS_ADR		1
#define XGXS_STATUS_A		(XGXS_STATUS_ADR | (1<<4))

/* PCIE3 registers */
#define PCIE3_GEN2_CTRL0_ADR	0
#define PCIE3_GEN2_CTRL1_ADR	1
#define PCIE3_GEN_CTRL2_ADR	2

#define GEN2_CTRL0_ADR		PCIE3_GEN2_CTRL0_ADR
#define GEN2_CTRL1_ADR		PCIE3_GEN2_CTRL1_ADR
#define GEN2_CTRL2_ADR		PCIE3_GEN_CTRL2_ADR

#define GEN2_CTRL0_A		(GEN2_CTRL0_ADR | (1<<4))
#define GEN2_CTRL1_A		(GEN2_CTRL1_ADR | (1<<4))
#define GEN2_CTRL2_A		(GEN2_CTRL2_ADR | (1<<4))

/* PCIE4 registers */
#define PCIE4_LANE_CTRL2_ADR	2
#define LANE_CTRL2_ADR		PCIE4_LANE_CTRL2_ADR
#define LANE_CTRL2_A		(LANE_CTRL2_ADR | (1<<4))

/* PCIE5 registers */
#define LANE_PRBS0_A		(1 | (1<<4))
#define LANE_PRBS1_A		(2 | (1<<4))
#define LANE_PRBS2_A		(3 | (1<<4))
#define LANE_PRBS3_A		(4 | (1<<4))
#define LANE_PRBS4_A		(5 | (1<<4))
#define LANE_TEST0_A		(6 | (1<<4))

/* RX_DFE0_BC_BLK_ADR registers */
#define RX_DFE0_STATUS_ADR	0
#define RX_DFE0_STATUS_A	(RX_DFE0_STATUS_ADR | (1<<4))
#define RX_DFE0_PRBS_PASS_VAL	0x8000

/* RX_DFE0_BC_BLK_ADR registers */
#define RX_DFE0_CONTROL_ADR	1
#define RX_DFE0_CONTROL_A	(RX_DFE0_CONTROL_ADR | (1<<4))

/* RX_DFE0_LN1_BLK_ADR registers */
#define RX_DFE0_LN1_BLK_ADR   0x7010
#define RX_DFE0_LN1_control_pci 0x18

struct env_variables {
	unsigned long pciespeed;
	unsigned long pciepolynomial;
	unsigned long testdelay;
	char *boardname;
	int verbosity;
};

static struct env_variables env;

static void set_speed_a(void)
{
	/*
	 * [PCIe-A] : Configuring PCIE3_BLK_ADDR - Gen2Ctrl1_A, Gen2Ctrl2_A:
	 *Software override pipe_RateSelect
	 * [PCIe-A] : Configuring PCIE3_BLK_ADDR - Gen2Ctrl1_A:
	 * Set RateSelect_mdio_sel
	 */
	mdio_write_pcie_a(GEN2_CTRL1_A, 0xffff);

	/* [BRCM ENG] SPEED SETTING
	 * Gen-1 = 0x0000
	 * Gen-2 = 0xffff  << default
	 */

	if (env.pciespeed == 1) {
		debug_cond(env.verbosity, "Setting PCIe-A speed to Gen-1.\n");
		mdio_write_pcie_a(GEN2_CTRL2_A, 0x0000);
	} else {
		debug_cond(env.verbosity, "Setting PCIe speed to Gen-2.\n");
		mdio_write_pcie_a(GEN2_CTRL2_A, 0xffff);
	}
}

static void set_speed_b(void)
{
	/*
	 * [PCIe-B] : Configuring PCIE3_BLK_ADDR - Gen2Ctrl1_A, Gen2Ctrl2_A:
	 * Software override pipe_RateSelect
	 * [PCIe-B] : Configuring PCIE3_BLK_ADDR - Gen2Ctrl1_A:
	 * Set RateSelect_mdio_sel
	 */
	mdio_write_pcie_b(GEN2_CTRL1_A, 0xffff);

	/* [BRCM ENG] SPEED SETTING
	 * Gen-1 = 0x0000
	 * Gen-2 = 0xffff  << default
	 */
	if (env.pciespeed == 1) {
		debug_cond(env.verbosity, "Setting PCIe-B speed to Gen-1.\n");
		mdio_write_pcie_b(GEN2_CTRL2_A, 0x0000);
	} else {
		debug_cond(env.verbosity, "Setting PCIe-B speed to Gen-2.\n");
		mdio_write_pcie_b(GEN2_CTRL2_A, 0xffff);
	}
}

static void set_polynomial_a(void)
{
	/*
	 * [PCIe-A] : Configuring PCIE5_BLK_ADDR - LaneTest0_A
	 * [PCIe-A] : Configuring PCIE5_BLK_ADDR - LaneTest0_A:
	 * Set MdioStandAloneMode
	 */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);

	/* [PCIe-A] : Configuring PCIE5_BLK_ADDR - : Setup the PRBS test */

	switch (env.pciepolynomial) {
	case 7:
		debug_cond(env.verbosity, "PCIe-A: PRBS 7\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0x0000);
		mdio_write_pcie_a(LANE_PRBS4_A, 0x0000);
		break;
	case 15:
		debug_cond(env.verbosity, "PCIe-A: PRBS 15\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0x5555);
		mdio_write_pcie_a(LANE_PRBS4_A, 0x5555);
		break;
	case 23:
		debug_cond(env.verbosity, "PCIe-A: PRBS 23\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0xAAAA);
		mdio_write_pcie_a(LANE_PRBS4_A, 0xAAAA);
		break;
	case 31:
		debug_cond(env.verbosity, "PCIe-A: PRBS 31\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0xFFFF);
		mdio_write_pcie_a(LANE_PRBS4_A, 0xFFFF);
		break;
	case 9:
		debug_cond(env.verbosity, "PCIe-A: PRBS 9\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0x0000);
		mdio_write_pcie_a(LANE_PRBS4_A, 0x0000);
		break;
	case 10:
		debug_cond(env.verbosity, "PCIe-A: PRBS 10\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0x5555);
		mdio_write_pcie_a(LANE_PRBS4_A, 0x5555);
		break;
	case 11:
		debug_cond(env.verbosity, "PCIe-A: PRBS 11\n");
		mdio_write_pcie_a(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_a(LANE_PRBS3_A, 0xAAAA);
		mdio_write_pcie_a(LANE_PRBS4_A, 0xAAAA);
		break;
	}

	mdio_write_pcie_a(LANE_PRBS0_A, 0xffff);
}

static void set_polynomial_b(void)
{
	/*
	 * [PCIe-B] : Configuring PCIE5_BLK_ADDR - LaneTest0_A
	 * [PCIe-B] : Configuring PCIE5_BLK_ADDR - LaneTest0_A:
	 * Set MdioStandAloneMode
	 */
	mdio_write_pcie_b(0x1f, PCIE5_BLK_ADR);

	/* [PCIe-B] : Configuring PCIE5_BLK_ADDR - : Setup the PRBS test */

	switch (env.pciepolynomial) {
	case 7:
		debug_cond(env.verbosity, "PCIe-B: PRBS 7\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0x0000);
		mdio_write_pcie_b(LANE_PRBS4_A, 0x0000);
		break;
	case 15:
		debug_cond(env.verbosity, "PCIe-B: PRBS 15\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0x5555);
		mdio_write_pcie_b(LANE_PRBS4_A, 0x5555);
		break;
	case 23:
		debug_cond(env.verbosity, "PCIe-B: PRBS 23\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0xAAAA);
		mdio_write_pcie_b(LANE_PRBS4_A, 0xAAAA);
		break;
	case 31:
		debug_cond(env.verbosity, "PCIe-B: PRBS 31\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0x0228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0xFFFF);
		mdio_write_pcie_b(LANE_PRBS4_A, 0xFFFF);
		break;
	case 9:
		debug_cond(env.verbosity, "PCIe-B: PRBS 9\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0x0000);
		mdio_write_pcie_b(LANE_PRBS4_A, 0x0000);
		break;
	case 10:
		debug_cond(env.verbosity, "PCIe-B: PRBS 10\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0x5555);
		mdio_write_pcie_b(LANE_PRBS4_A, 0x5555);
		break;
	case 11:
		debug_cond(env.verbosity, "PCIe-B: PRBS 11\n");
		mdio_write_pcie_b(LANE_TEST0_A, 0xF228);
		mdio_write_pcie_b(LANE_PRBS3_A, 0xAAAA);
		mdio_write_pcie_b(LANE_PRBS4_A, 0xAAAA);
		break;
	}

	mdio_write_pcie_b(LANE_PRBS0_A, 0xffff);
}

static void pcie_reset_a(void)
{
	unsigned int val;

	debug_cond(env.verbosity, "Resetting PCIe-A...");
	mdio_write_pcie_a(0x1F, 0x2100);
	mdio_write_pcie_a(0x13, 0x2b18);
	udelay(20000);

	/*
	 * Select perst_b signal as reset source, and put the device in
	 * reset
	 */
	val = readl(PCIE_A_HWADDR);

	val &= ~EP_PERST_SOURCE_SELECT & ~EP_MODE_SURVIVE_PERST &
		~RC_PCIE_RST_OUTPUT;

	writel(val, PCIE_A_HWADDR);
	udelay(250);

	/* now bring it out of reset */
	val |= RC_PCIE_RST_OUTPUT;
	writel(val, PCIE_A_HWADDR);
	mdelay(250);
	debug_cond(env.verbosity, " OK\n");
}

static void pcie_reset_b(void)
{
	unsigned int val;

	debug_cond(env.verbosity, "Resetting PCIe-B...");
	mdio_write_pcie_b(0x1F, 0x2100);
	mdio_write_pcie_b(0x13, 0x2b18);
	udelay(20000);

	/*
	 * Select perst_b signal as reset source, and put the device in
	 * reset
	 */
	val = readl(PCIE_B_HWADDR);
	val &= ~EP_PERST_SOURCE_SELECT & ~EP_MODE_SURVIVE_PERST &
		~RC_PCIE_RST_OUTPUT;
	writel(val, PCIE_B_HWADDR);
	udelay(250);

	/* now bring it out of reset */
	val |= RC_PCIE_RST_OUTPUT;
	writel(val, PCIE_B_HWADDR);
	mdelay(250);
	debug_cond(env.verbosity, " OK\n");
}

static void pcie_clear(void)
{
	/* [PCIe-A] :Clear and read status registers */
	debug_cond(env.verbosity, "Clearing and reading status registers...");
	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_read_pcie_a(RX_DFE0_STATUS_A);

	udelay(1000);

	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_read_pcie_a(RX_DFE0_STATUS_A);

	/* [PCIe-B] :Clear and read status registers */
	mdio_write_pcie_b(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_read_pcie_b(RX_DFE0_STATUS_A);

	udelay(1000);

	mdio_write_pcie_b(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_read_pcie_b(RX_DFE0_STATUS_A);
	debug_cond(env.verbosity, " OK\n");
}


static void pcie_bert_setup_a(void)
{
	uint16_t data_rd;
	int do_pol_flip = 1;

	/* [PCIe-A] : Configuring PCIE3_BLK_ADDR - LaneTest0_A:
	 * Set StandAloneMode_mdio_sel & Rloop */
	mdio_write_pcie_a(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_a(GEN2_CTRL0_A, 0x00c0);

	set_speed_a();

	/*
	 * [PCIe-A] : Configuring PCIE4_BLK_ADDR - LANE_CTRL2_A, Gen2Ctrl2_A:
	 * Disable 8b10b decoder/encoder
	 * [PCIe-A] : Configuring PCIE4_BLK_ADDR - LANE_CTRL2_A, Gen2Ctrl2_A:
	 * reset eden
	 */
	mdio_write_pcie_a(0x1f, PCIE4_BLK_ADR);
	mdio_write_pcie_a(LANE_CTRL2_A, 0x0000);

	set_polynomial_a();

	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_write_pcie_a(RX_DFE0_CONTROL_A, 0x1c47);

	/*
	 * Rx polarity hardware is flipped for BCM958712K boards, so we
	 * adjust the polarity within the software. Do NOT flip polarity
	 * for XMC boards or BCM958710K boards.
	 * RX_DFE0_LN1_control_pci: set b[3:2] = 2'b11
	 */

	env.boardname = getenv("board_name");
	if (env.boardname != NULL) {
		if ((strcmp(env.boardname, "BCM958710K_0000") == 0) ||
		    (strcmp(env.boardname, "BCM958710K_0010") == 0) ||
		    (strcmp(env.boardname, "BCM958710K") == 0)	)
			do_pol_flip = 0;
	}

#if (CONFIG_BCM_NS2_SVK_XMC)
	do_pol_flip = 0;
#endif

	if (do_pol_flip) {
		mdio_write_pcie_a(0x1F, RX_DFE0_LN1_BLK_ADR);
		data_rd = mdio_read_pcie_a(RX_DFE0_LN1_control_pci);
		data_rd |= (1 << 2);
		data_rd |= (1 << 3);
		mdio_write_pcie_a(RX_DFE0_LN1_control_pci, data_rd);
		data_rd = mdio_read_pcie_a(RX_DFE0_LN1_control_pci);
	} else {
		mdio_write_pcie_a(0x1F, RX_DFE0_LN1_BLK_ADR);
		data_rd = mdio_read_pcie_a(RX_DFE0_LN1_control_pci);
		data_rd &= (0 << 2);
		data_rd &= (0 << 3);
		mdio_write_pcie_a(RX_DFE0_LN1_control_pci, data_rd);
		data_rd = mdio_read_pcie_a(RX_DFE0_LN1_control_pci);
	}

	/* [PCIe-A] :Clear and read status registers */
	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);

	udelay(1000);
}

static void pcie_bert_setup_b(void)
{
	uint16_t data_rd;

	/* [PCIe-B] : Configuring PCIE3_BLK_ADDR - LaneTest0_A:
	 * Set StandAloneMode_mdio_sel & Rloop */
	mdio_write_pcie_b(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_b(GEN2_CTRL0_A, 0x00c0);

	set_speed_b();

	/*
	 * [PCIe-B] : Configuring PCIE4_BLK_ADDR - LANE_CTRL2_A, Gen2Ctrl2_A:
	 * Disable 8b10b decoder/encoder
	 * [PCIe-B] : Configuring PCIE4_BLK_ADDR - LANE_CTRL2_A, Gen2Ctrl2_A:
	 * reset eden
	 */
	mdio_write_pcie_b(0x1f, PCIE4_BLK_ADR);
	mdio_write_pcie_b(LANE_CTRL2_A, 0x0000);

	set_polynomial_b();

	mdio_write_pcie_b(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_write_pcie_b(RX_DFE0_CONTROL_A, 0x1c47);

	/* [PCIe-B] :Clear and read status registers */
	mdio_write_pcie_b(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_b(RX_DFE0_STATUS_A);

	udelay(1000);
}

static int pcie_begin_test_ab(void)
{
	int lane, retry;
	uint16_t data_rd;
	int ret = 0;

	post_log("\n==================> Reading [A->B] <==================\n");
	if (env.testdelay)
		post_log("Running each lane for %d second(s)...\n",
			 env.testdelay);

	for (lane = 0; lane <= 3; lane = lane+1) {
		retry = 0;
		/* Block to read from = B */
		mdio_write_pcie_b(0x1f, 0x7000 | lane << 4);

		if (env.testdelay)
			udelay(env.testdelay * 1000000);

		do {
			/* Register withing block B to read from */
			data_rd = mdio_read_pcie_b(RX_DFE0_STATUS_A);
			retry++;
		} while ((data_rd != RX_DFE0_PRBS_PASS_VAL) &&
				(retry < MAX_RETRY_COUNT));

		post_log("[B]:read data:(%0x), lane: %0x, block addr: %0x\n",
			 data_rd, lane, 0x7000 | lane << 4);

		if (data_rd == RX_DFE0_PRBS_PASS_VAL) {
			post_log("[B] PASSED|lane: %0x, tries:%d\n",
				 lane, retry);
		} else {
			post_log("[B] **FAILED**|lane: %0x, tries:%d\n",
				 lane, retry);
			ret = -1;
		}
	}
	post_log("\n");
	pcie_reset_a();
	pcie_bert_setup_a();

	pcie_reset_b();
	pcie_bert_setup_b();

	pcie_clear();

	post_log("\n==================> Reading [B->A] <==================\n");
	if (env.testdelay)
		post_log("Running each lane for %d second(s)...\n",
			 env.testdelay);

	for (lane = 0; lane <= 3; lane = lane+1) {
		retry = 0;
		/* Block to read from = A */
		mdio_write_pcie_a(0x1f, 0x7000 | lane << 4);

		if (env.testdelay)
			udelay(env.testdelay * 1000000);

		do {
			/* Register withing block A to read from */
			data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
			retry++;
		} while ((data_rd != RX_DFE0_PRBS_PASS_VAL) &&
				(retry < MAX_RETRY_COUNT));

		post_log("[A]:read data:(%0x), lane: %0x, block addr: %0x\n",
			 data_rd, lane, 0x7000 | lane << 4);

		if (data_rd == RX_DFE0_PRBS_PASS_VAL) {
			post_log("[A] PASSED lane: %d, tries:%d\n",
				 lane, retry);
		} else {
			post_log("[A] **FAILED**|lane: %0x, tries:%d\n",
				 lane, retry);
			ret = -1;
		}
	}
	return ret;
}


static int pcie_begin_test_aa(void)
{
	int lane, retry;
	uint16_t data_rd;
	int ret = 0;

	post_log("\n==================> Reading [A->A] <==================\n");

	if (env.testdelay)
		post_log("Running each lane for %d second(s)...\n",
			 env.testdelay);

	for (lane = 0; lane <= 3; lane++) {
		retry = 0;
		mdio_write_pcie_a(0x1f, 0x7000 | lane << 4);

		if (env.testdelay)
			udelay(env.testdelay * 1000000);

		do {
			data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
			retry++;
		} while ((data_rd != RX_DFE0_PRBS_PASS_VAL) &&
			 (retry < MAX_RETRY_COUNT));

		post_log("[a]:read data:(%04x), lane: %0x, block addr: %0x\n",
			 data_rd, lane, 0x7000 | lane << 4);

		if (data_rd == RX_DFE0_PRBS_PASS_VAL) {
			post_log("[a] PASSED lane: %d, tries:%d\n",
				 lane, retry);
		} else {
			post_log("[a] **FAILED**|lane: %0x, tries:%d\n",
				 lane, retry);
			ret = -1;
		}
	}
	return ret;
}


static int pcie_begin_test_bb(void)
{
	int lane, retry;
	uint16_t data_rd;
	int ret = 0;

	post_log("\n==================> Reading [B->B] <==================\n");
	if (env.testdelay)
		post_log("Running each lane for %d second(s)...\n",
			 env.testdelay);

	for (lane = 0; lane <= 3; lane++) {
		retry = 0;
		mdio_write_pcie_b(0x1f, 0x7000 | lane << 4);

		if (env.testdelay)
			udelay(env.testdelay * 1000000);

		do {
			data_rd = mdio_read_pcie_b(RX_DFE0_STATUS_A);
			retry++;
		} while ((data_rd != RX_DFE0_PRBS_PASS_VAL) &&
			 (retry < MAX_RETRY_COUNT));

		post_log("[b]:DFE data:(%04x), lane: %0x, block addr: %0x\n",
			 data_rd, lane, 0x7000 | lane << 4);

		if (data_rd == RX_DFE0_PRBS_PASS_VAL) {
			post_log("[b] PASSED lane: %d, tries:%d\n",
				 lane, retry);
		} else {
			post_log("[b] **FAILED**|lane: %0x, tries:%d\n",
				 lane, retry);
			ret = -1;
		}
	}
	return ret;
}

static bool patternexists(int val, int *arr, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		if (arr[i] == val)
			return true;
	}
	return false;
}

static void help(void)
{
	post_log("\n ---------------------\n");
	post_log("| PCIE DIAG HELP MENU |\n");
	post_log(" ---------------------\n");

	post_log("pciespeed options: 1, 2");
	post_log(" (default 2)\n");
	post_log("EX: to set PRBS SERDES speed to Gen-1:\n");
	post_log("\tu-boot> setenv pciespeed 1\n\n");

	post_log("pciepolynomial options: 7, 15, 23, 31, 9, 10, 11");
	post_log(" (default 7)\n");
	post_log("EX: to set PRBS polynomial speed to PRBS31:\n");
	post_log("\tu-boot> setenv pciepolynomial 31\n\n");

	post_log("testdelay: number of seconds the test will run for");
	post_log(" (default 0)\n");
	post_log("EX: to run the test for 5 seconds before checking status\n");
	post_log("\tu-boot> setenv testdelay 5\n\n");

	post_log("verbosity: show debug printouts\n");
	post_log("\tu-boot> setenv verbosity 1\n\n");
}


int PCIE_post_test(int flags)
{
	int result = -1, test = 0;
	int pattern[7] = {7, 15, 23, 31, 9, 10, 11};

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	post_log("\nPCIE lanes PRBS diags\n");

	if (getenv_yesno("verbosity") == 1)
		env.verbosity = true;
	else
		env.verbosity = false;

	env.pciespeed = getenv_ulong("pciespeed", 10, 2);
	env.pciepolynomial = getenv_ulong("pciepolynomial", 10, 7);
	env.testdelay = getenv_ulong("testdelay", 10, 0);
	env.boardname = getenv("board_name");

	debug_cond(env.verbosity, "pciespeed: Gen-%u\n", env.pciespeed);
	if (env.pciespeed < 1 || env.pciespeed > 2) {
		post_log("Invalid speed chosen, default to Gen-2\n");
		env.pciespeed = 2;
	}
	debug_cond(env.verbosity,
		   "pciepolynomial: PRBS%u\n", env.pciepolynomial);
	if (patternexists(env.pciepolynomial, pattern, 7) == 0) {
		post_log("Invalid polynomial chosen, default to PRBS7\n");
		env.pciepolynomial = 7;
	}

	debug_cond(env.verbosity, "testdelay: %u sec.\n", env.testdelay);

	/*Initialize and Reset PCIE*/
	pcie_reset_a();
	pcie_bert_setup_a();
	pcie_reset_b();
	pcie_bert_setup_b();
	pcie_clear();

	/* AUTO MODE: only run test A->B for SVK, A->A for XMC */
	if (flags & POST_AUTO) {
#if CONFIG_TARGET_NS2_SVK
		result = pcie_begin_test_ab();
		return result;
#elif (CONFIG_BCM_NS2_SVK_DDR4 || CONFIG_BCM_NS2_SVK_XMC)
		result = pcie_begin_test_aa();
		return result;
#endif
	}

#if CONFIG_TARGET_NS2_SVK
	while (1) {
		post_log("\nTEST CHOICES:\n1. [A->A]\n2. [B->B]\n");
		post_log("3. [A->B]\n");
		test = post_getUserInput("Test type? (1, 2, 3): ");
		if (test < 1 || test > 4)
			post_log("Error, out of range.\n");
		else
			break;
	}

#elif (CONFIG_BCM_NS2_SVK_DDR4 || CONFIG_BCM_NS2_SVK_XMC)
	test = 1;
#endif

	switch (test) {
	case 1:
		result = pcie_begin_test_aa();
		break;
	case 2:
		result = pcie_begin_test_bb();
		break;
	case 3:
		result = pcie_begin_test_ab();
		break;
	}
	return result;
}
