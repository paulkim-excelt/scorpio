/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * MDIO serdes(Internal PHY) access verification
 *
 */

#include <common.h>
#include <post.h>
#include "mdio_wrappers.h"

#if CONFIG_POST & CONFIG_SYS_POST_MDIO
#define PCIE_PLL_AFE1_100MHZ_EP_CTRL0 0x2100
#define PCIE_PLL_AFE1_100MHZ_EP_CTRL0_DEFAULT0 0x556c
#define PCIE_PLL_AFE1_100MHZ_EP_CTRL0_DEFAULT1 0x5174

#define PCIE_PLL_AFE1_100MHZ_EP_CTRL1 0x2101
#define PCIE_PLL_AFE1_100MHZ_EP_CTRL1_DEFAULT 0x01d0

#define PCIE_PLL_AFE1_100MHZ_EP_CTRL2 0x2102
#define PCIE_PLL_AFE1_100MHZ_EP_CTRL2_DEFAULT 0x19f8

#define USB20_UTMI_BLOCK 0x80C0
#define USB20_PLL_DIVIDER_INT 0x8
#define USB20_PLL_DIVIDER_INT_DEFAULT 0x0012
#define USB20_PLL_DIVIDER_FRAC0 0x9
#define USB20_PLL_DIVIDER_FRAC0_DEFAULT 0x6276
#define USB20_PLL_DIVIDER_FRAC1 0xA
#define USB20_PLL_DIVIDER_FRAC1_DEFAULT 0x0007

#define USB30_PIPE_BLOCK 0x8060
#define USB30_PIPE_REG0 0x0
#define USB30_PIPE_REG0_DEFAULT 0x0C07
#define USB30_PIPE_REG1 0x1
#define USB30_PIPE_REG1_DEFAULT 0x00007
#define USB30_PIPE_REG2 0x2
#define USB30_PIPE_REG2_DEFAULT 0xF30D

static void help(void)
{
	post_log("\n ---------------------\n");
	post_log("| MDIO DIAG HELP MENU |\n");
	post_log(" ---------------------\n");
}

int MDIO_post_test (int flags)
{
	uint16_t val;
	int result = 0;
	int idx;

	void (*mdio_write_pcie[2])(uint32_t, uint16_t) = {
		 mdio_write_pcie_a, mdio_write_pcie_b};
	uint16_t (*mdio_read_pcie[2])(uint32_t) = {
		 mdio_read_pcie_a, mdio_read_pcie_b};
	void (*mdio_write_usb20[3])(uint32_t, uint16_t) = {
		 mdio_write_usb20_hs0, mdio_write_usb20_hs1,
		 mdio_write_usb20_drd};
	uint16_t (*mdio_read_usb20[3])(uint32_t) = {
		 mdio_read_usb20_hs0, mdio_read_usb20_hs1,
		 mdio_read_usb20_drd};

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	post_log("\n******** 1. MDIO External PHY test:*********\n");

	val = mdio_read_extphy (0x2);
	post_log("BCM54810 PHY_Identifier_MSB_Register: 0x%x\n", val);
	if (val != 0x0362)
		result = -1;

	val = mdio_read_extphy (0x3);
	post_log("BCM54810 PHY_Identifier_LSB_Register: 0x%x\n", val);
	if ((val & 0xFFF0) != 0x5D00)
		result = -1;

	if (result == 0)
		post_log("MDIO External PHY test:....Passed\n");
	else
		post_log("MDIO External PHY test:....Failed\n");

	for (idx = 0; idx < 2; idx++) {
		post_log("\n******** %d. MDIO PCIE%d PHY test:*********\n",
			 2 + idx, idx);
		result = 0;
		/* Access
		 * 1. PCIE_PLL_AFE1_100MHZ_EP_CTRL0 @ 0x2100: Default: 0x556c
		 * 2. PCIE_PLL_AFE1_100MHZ_EP_CTRL1 @ 0x2101: Default: 0x01d0
		 * 3. PCIE_PLL_AFE1_100MHZ_EP_CTRL2 @ 0x2102: Default: 0x19f8
		 */

		mdio_write_pcie[idx](0x1F,
				     PCIE_PLL_AFE1_100MHZ_EP_CTRL0 & 0xFFE0);
		val = mdio_read_pcie[idx](PCIE_PLL_AFE1_100MHZ_EP_CTRL0 & 0x1F);
		post_log("PCIE%d PCIE_PLL_AFE1_100MHZ_EP_CTRL0: 0x%x\n",
			 idx, val);
		if (!((val == PCIE_PLL_AFE1_100MHZ_EP_CTRL0_DEFAULT0) ||
		      (val == PCIE_PLL_AFE1_100MHZ_EP_CTRL0_DEFAULT1)))
			result = -1;

		mdio_write_pcie[idx](0x1F,
				     PCIE_PLL_AFE1_100MHZ_EP_CTRL1 & 0xFFE0);
		val = mdio_read_pcie[idx](PCIE_PLL_AFE1_100MHZ_EP_CTRL1 & 0x1F);
		post_log("PCIE%d PCIE_PLL_AFE1_100MHZ_EP_CTRL1: 0x%x\n",
			 idx, val);
		if (val != PCIE_PLL_AFE1_100MHZ_EP_CTRL1_DEFAULT)
			result = -1;

		mdio_write_pcie[idx](0x1F,
				     PCIE_PLL_AFE1_100MHZ_EP_CTRL2 & 0xFFE0);
		val = mdio_read_pcie[idx](PCIE_PLL_AFE1_100MHZ_EP_CTRL2 & 0x1F);
		post_log("PCIE%d PCIE_PLL_AFE1_100MHZ_EP_CTRL2: 0x%x\n",
			 idx, val);
		if (val != PCIE_PLL_AFE1_100MHZ_EP_CTRL2_DEFAULT)
			result = -1;

		if (result == 0)
			post_log("MDIO PCIE%d PHY test:....Passed\n", idx);
		else
			post_log("MDIO PCIE%d PHY test:....Failed\n", idx);
	}

	for (idx = 0; idx < 3;  idx++) {
		post_log("\n******** %d. MDIO USB20.%d PHY test:*********\n",
			 4 + idx, idx);
		post_log("*** ATTN: Make sure to run usb start first***\n");
		result = 0;
		/* Access: UTMI block : 0x80C0
		 * 0xA PLL divider Default 16'b0007
		 *	b19:16 ndiv frac
		 *	b31:20 Not Used
		 * 0x8 PLL divider	Default 16'h0012
		 *	b9:0 -> ndiv int PLL integer N-divider;52MHz ref-clock
		 *	b11:10 -> not used
		 *	b15:12 -> PLL integer P-divider: 0000 double: *2
		 */

		mdio_write_usb20[idx](0x1F, USB20_UTMI_BLOCK);
		val = mdio_read_usb20[idx](USB20_PLL_DIVIDER_INT);
		post_log("USB20.%d USB20_PLL_DIVIDER_INT 0x%x\n", idx, val);
		if (val != USB20_PLL_DIVIDER_INT_DEFAULT) {
			post_log("Mismatch val=%x with expected %x\n",
				 val, USB20_PLL_DIVIDER_INT_DEFAULT);
			result = -1;
		}

		val = mdio_read_usb20[idx](USB20_PLL_DIVIDER_FRAC0);
		post_log("USB20.%d USB20_PLL_DIVIDER_FRAC0: 0x%x\n", idx, val);
		if (val != USB20_PLL_DIVIDER_FRAC0_DEFAULT) {
			post_log("Mismatch val=%x with expected %x\n",
				 val, USB20_PLL_DIVIDER_FRAC0_DEFAULT);
			result = -1;
		}

		val = mdio_read_usb20[idx](USB20_PLL_DIVIDER_FRAC1);
		post_log("USB20.%d USB20_PLL_DIVIDER_FRAC1: 0x%x\n", idx, val);
		if (val != USB20_PLL_DIVIDER_FRAC1_DEFAULT) {
			post_log("Mismatch val=%x with expected %x\n",
				 val, USB20_PLL_DIVIDER_FRAC1_DEFAULT);
			result = -1;
		}

		if (result == 0)
			post_log("MDIO USB20.%d PHY test:....Passed\n", idx);
		else
			post_log("MDIO USB20.%d PHY test:....Failed\n", idx);
	}
	/* Access: USB30 PIPE BLOCK @ 0x8060
	 *	REG0 Default 0x0C07
	 *	REG1 Default 0x0007
	 *	REG2 Default 0xF30D
	 */
	post_log("\n******** 7. MDIO USB30 PHY test::*********\n");
	result = 0;
	mdio_write_usb30(0x1F, USB30_PIPE_BLOCK);

	val = mdio_read_usb30(USB30_PIPE_REG0);
	post_log("USB30 USB30_PIPE_REG0: 0x%x\n", val);
	if (val != USB30_PIPE_REG0_DEFAULT) {
		post_log("Mismatch val=%x with expected %x\n",
			 val, USB30_PIPE_REG0_DEFAULT);
		result = -1;
	}

	val = mdio_read_usb30(USB30_PIPE_REG1);
	post_log("USB30 USB30_PIPE_REG1: 0x%x\n", val);
	if (val != USB30_PIPE_REG1_DEFAULT) {
		post_log("Mismatch val=%x with expected %x\n",
			 val, USB30_PIPE_REG1_DEFAULT);
		result = -1;
	}

	val = mdio_read_usb30(USB30_PIPE_REG2);
	post_log("USB30 USB30_PIPE_REG2: 0x%x\n", val);
	if (val != USB30_PIPE_REG2_DEFAULT) {
		post_log("Mismatch val=%x with expected %x\n",
			 val, USB30_PIPE_REG2_DEFAULT);
		result = -1;
	}

	if (result == 0)
		post_log("MDIO USB30 PHY test:....Passed\n");
	else
		post_log("MDIO USB30 PHY test:....Failed\n");

	return result;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_MDIO */
