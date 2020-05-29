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
#include <post.h>
#include <linux/types.h>
#include <asm/io.h>
#include "MDIO_wrappers.h"
#include <asm/arch/bcm_otp.h>

#define MAX_RETRY_COUNT			10
#define PCIE_A_HWADDR			0x58220000
/*#define PCIE_B_HWADDR			0x50020000*/
#define EP_PERST_SOURCE_SELECT_SHIFT	2
#define EP_PERST_SOURCE_SELECT		(1 << EP_PERST_SOURCE_SELECT_SHIFT)
#define EP_MODE_SURVIVE_PERST_SHIFT	1
#define EP_MODE_SURVIVE_PERST		(1 << EP_MODE_SURVIVE_PERST_SHIFT)
#define RC_PCIE_RST_OUTPUT_SHIFT	0
#define RC_PCIE_RST_OUTPUT		(1 << RC_PCIE_RST_OUTPUT_SHIFT)

/*
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

static void pcie_bert_setup_p0(void)
{
	uint16_t data_rd;

	/* 1. Change device type */
	mdio_write_pcie_a(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_a(GEN2_CTRL0_A, 0x00c0);

	/* 2. Enable Stand-alone mode bit 3 */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(LANE_TEST0_A, 0x0228);

	/* 3. Set up PRBS patterns. */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(0x14, 0x0000);
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(0x15, 0x0000);

	/* 4. For PRBS patterns 9,10,11 set bits
	 * (15(lane3),14(lane2),13(lane1),12(lane0))
	 */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(0x16, 0xf228);

	/* 5. disable 8b10b */
	mdio_write_pcie_a(0x1f, PCIE4_BLK_ADR);
	mdio_write_pcie_a(LANE_CTRL2_A, 0x0000);

	/* 6. set speed */
	mdio_write_pcie_a(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_a(GEN2_CTRL1_A, 0xffff);

	/*[PCIe-A] : Configuring PCIE3_BLK_ADDR -
	 *Gen2Ctrl2_A: Set mdioRateSelect to gen2
	 */
	mdio_write_pcie_a(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_a(GEN2_CTRL2_A, 0xffff);

	/* 7. rx broadcast to setup rx status = prbs error register */
	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_write_pcie_a(RX_DFE0_CONTROL_A, 0x1c47);

	/* 8. enable PRBS */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(LANE_PRBS0_A, 0xffff);

	/* 9. clear error count for all lanes.PCIE_Serdes,read,F700,10,xxxx,
	 * at this point don't care what read data is, clearing previous errors
	 */
	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);

	udelay(1000);

	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
	post_log("Reading the STATUS registers: DFE reading\n");
	post_log("Read data: (x: %0x, d: %0d)\n", data_rd, data_rd);

	mdio_write_pcie_a(0x1f, PCIE1_BLK_ADR);
	data_rd = mdio_read_pcie_a(XGXS_STATUS_A);
	post_log("Reading the STATUS registers: PLL LOCK\n");
	post_log("PLL LOCK, Read data: (x: %0x, d: %0d)\n", data_rd, data_rd);

	udelay(1000);
}

/* NS2 type bert set-up */
#if 0
static void pcie_bert_setup_a(void)
{
	uint16_t data_rd;

	/*
	 * [PCIe-A] : Configuring PCIE5_BLK_ADDR - LaneTest0_A
	 * [PCIe-A] : Configuring PCIE5_BLK_ADDR - LaneTest0_A -
	 * Set MdioStandAloneMode
	 */
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(LANE_TEST0_A, 0x0228);

	/*[PCIe-A] : Configuring PCIE3_BLK_ADDR - LaneTest0_A:
	 * Set StandAloneMode_mdio_sel & Rloop
	 */
	mdio_write_pcie_a(0x1f, PCIE3_BLK_ADR);
	mdio_write_pcie_a(GEN2_CTRL0_A, 0x00c0);

	/*
	 * [PCIe-A] : Configuring PCIE3_BLK_ADDR -
	 * Gen2Ctrl1_A, Gen2Ctrl2_A: Software override pipe_RateSelect
	 * [PCIe-A] : Configuring PCIE3_BLK_ADDR -
	 * Gen2Ctrl1_A: Set RateSelect_mdio_sel
	 */
	mdio_write_pcie_a(GEN2_CTRL1_A, 0xffff);

	/*[PCIe-A] : Configuring PCIE3_BLK_ADDR -
	 * Gen2Ctrl2_A: Set mdioRateSelect to gen2
	 */
	mdio_write_pcie_a(GEN2_CTRL2_A, 0xffff);

	/*
	 * [PCIe-A] : Configuring PCIE4_BLK_ADDR -
	 * LANE_CTRL2_A, Gen2Ctrl2_A: Disable 8b10b decoder/encoder
	 * [PCIe-A] : Configuring PCIE4_BLK_ADDR -
	 * LANE_CTRL2_A, Gen2Ctrl2_A: reset eden
	 */
	mdio_write_pcie_a(0x1f, PCIE4_BLK_ADR);
	mdio_write_pcie_a(LANE_CTRL2_A, 0x0000);

	/*[PCIe-A] : Configuring PCIE5_BLK_ADDR - : Setup the PRBS test*/
	mdio_write_pcie_a(0x1f, PCIE5_BLK_ADR);
	mdio_write_pcie_a(LANE_PRBS3_A, 0xe4e4);
	mdio_write_pcie_a(LANE_PRBS4_A, 0xe4e4);
	mdio_write_pcie_a(LANE_PRBS0_A, 0xffff);

	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	mdio_write_pcie_a(RX_DFE0_CONTROL_A, 0x1c47);

	/*[PCIe-A] :Clear and read status registers\n*/
	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);

	udelay(1000);

	mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
	data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
	post_log("Reading the STATUS registers: DFE reading\n");
	post_log("Read data: (x: %0x, d: %0d)\n", data_rd, data_rd);

	mdio_write_pcie_a(0x1f, PCIE1_BLK_ADR);
	data_rd = mdio_read_pcie_a(XGXS_STATUS_A);
	post_log("Reading the STATUS registers: PLL LOCK\n");
	post_log("PLL LOCK, Read data: (x: %0x, d: %0d)\n", data_rd, data_rd);

	udelay(1000);
}
#endif


static int pcie_begin_test(void)
{
	int lane, retry;
	uint16_t data_rd;
	int ret = 0;
	u32 sku_id = 0;
	int otp_ops_status = 0;
	u32 lane_to_skip;
	u32 lane_count = 4;

#ifdef BCM_OTP_CHECK_ENABLED
	/* Checking sku id with OTP read */
	otp_ops_status = otp_read_sku_id(&sku_id);
	if (otp_ops_status != 0) {
		post_log("\nOTP read failed !! Exiting from this Diag !!\n");
		return BCM_NO_IP;
	}

	/* check if sku is BCM58541 */
	if (sku_id == 0x2)
		lane_to_skip = 2; /* Assuming 3rd lane to skip, need to check*/
#endif

	post_log("=================> Reading Port0 Lanes <=================\n");
	for (lane = 0; lane <= (lane_count - 1); lane = lane + 1) {
#ifdef BCM_OTP_CHECK_ENABLED
		if ((sku_id == 2) && (lane == lane_to_skip))
			continue;
#endif
		retry = 0;
		mdio_write_pcie_a(0x1f, 0x7000 | lane << 4);

		do {
			post_log("Clear & read status reg, retry:%d\n", retry);
			mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
			data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
			udelay(1000);
			mdio_write_pcie_a(0x1f, RX_DFE0_BC_BLK_ADR);
			data_rd = mdio_read_pcie_a(RX_DFE0_STATUS_A);
			retry++;
		} while ((data_rd != RX_DFE0_PRBS_PASS_VAL) &&
				(retry < MAX_RETRY_COUNT));

		post_log("DFE read data:%0x, lane: %0x, dfe block add: %0x\n",
				data_rd, lane, 0x7000 | lane << 4);

		if (data_rd == RX_DFE0_PRBS_PASS_VAL) {
			post_log("PASSED|lane:%0x,tries:%d\n", lane, retry);
		} else {
			post_log("*FAILED*|lane:%0x,tries:%d\n", lane, retry);
			ret = 1;
		}
	}

	return ret;
}

static void pcie_reset_p0(void)
{
	unsigned int val;

	post_log("Initializing PCIe-Port0 PHY...\n");
	mdio_write_pcie_a(0x1F, 0x2100);
	mdio_write_pcie_a(0x13, 0x2b18);
	udelay(20000);

	post_log("Reset PCIe-port0 ...\n");
	/*
	 * Select perst_b signal as reset source, and put the device in
	 * reset
	 */
	val = readl(PCIE_A_HWADDR);

	val &= ~EP_PERST_SOURCE_SELECT & ~EP_MODE_SURVIVE_PERST &
		~RC_PCIE_RST_OUTPUT;

	writel(val, PCIE_A_HWADDR);
	udelay(250);

	/* now bring it out of reset*/
	val |= RC_PCIE_RST_OUTPUT;
	writel(val, PCIE_A_HWADDR);
	mdelay(250);
}

int PCIE_post_test(int flags)
{
	int result = 0;
	post_log("\nPCIE lanes PRBS diags\n");

	/*Initialize and Reset PCIE*/
	pcie_reset_p0();
	/*pcie_reset_a();*/

	/*bert setup*/
	post_log("Setup PCIe-p0 for PRBS test\n");
	pcie_bert_setup_p0();

	result = pcie_begin_test();

	return result;
}
