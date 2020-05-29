/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <configs/bcm_northstar2.h>
#include <asm/arch/socregs.h>
#include <ahci.h>
#include <scsi.h>

#undef debug
#ifdef DEBUG
#define debug(x)  printf x
#else /* def DEBUG */
#define debug(x)
#endif /* def DEBUG */

/*
 *  Mostly this is to make sure a particular write is committed instead of
 *  deferred, but it's also for certain bits of paranoia.
 */
static void sata_writel(u32 value, u64 address)
{
#ifdef DEBUG
	u32 reg;
#endif /* def DEBUG */
	void *addr = (void *)(address + SATA_ROOT);

#ifdef DEBUG
	reg = readl(addr);
	debug(("SATA: reg %016llX was %08X, write %08X",
	       address + SATA_ROOT, reg, value));
#endif /* def DEBUG */
	writel(value, addr);
#ifdef DEBUG
	debug((", is now %08X\n", readl(addr)));
#endif /* def DEBUG */
}

static u32 sata_readl(u64 address)
{
	void *addr = (void *)(address + SATA_ROOT);

	return readl(addr);
}

#define msleep(a) udelay(a * 1000)

/*
 *  Set up the SATA host PHY registers
 */
int config_sata_phy(void)
{
	unsigned int cnt;
	u32 regval;

	debug(("enter config_sata_phy\n"));

	printf("set up port 0\n");
	/* Access PLL register bank 1 */
	sata_writel(0x00000060, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* Set intN_fb_en bit */
	sata_writel(0x00001DF8, SATA_PORT0_SATA3_PCB_REG2);
	/* Select integer divide mode (instead of fractional) */
	sata_writel(0x00002B00, SATA_PORT0_SATA3_PCB_REG3);
	/* Set PLL divider to 60 */
	sata_writel(0x00008824, SATA_PORT0_SATA3_PCB_REG4);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* Set oob_clk_sel to refclk/2 */
	sata_writel(0x00000001, SATA_PORT0_SATA3_PCB_REG13);
	/* Access SATA AEQ register bank */
	sata_writel(0x000000D0, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* switch to EQ_reg for equaliser control */
	regval = sata_readl(SATA_PORT0_SATA3_PCB_REG2);
	regval |= (1 << 12);
	regval &= (~(1 << 15));	/* must write this bit zeroed */
	sata_writel(regval, SATA_PORT0_SATA3_PCB_REG2);
	sata_writel(3 | (3 << 4), SATA_PORT0_SATA3_PCB_REG3);
	/* Access SATA RXPMD register bank */
	sata_writel(0x000001C0, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* Set a limit on how far the CDR integral loop can go, ~5K ppm */
	regval = (149 | (1 << 8));
	sata_writel(regval, SATA_PORT0_SATA3_PCB_REG7);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* strobe phy 0 reset */
	sata_writel(0x00000001, SATA_SATA_TOP_CTRL_PHY_CTRL_1);
	sata_writel(0x0000000E, SATA_SATA_TOP_CTRL_PHY_CTRL_2);
	sata_writel(0x00000000, SATA_SATA_TOP_CTRL_PHY_CTRL_1);
	sata_writel(0x00000000, SATA_SATA_TOP_CTRL_PHY_CTRL_2);
	/* wait for it to settle */
	msleep(100);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* poll pll_lock for the port */
	debug(("wait for pll lock on port 0\n"));
	for (cnt = 0;
	     ((cnt < 30) &&
	      (0 == (sata_readl(SATA_PORT0_SATA3_PCB_REG1) & 0x00001000)));
	     cnt++) {
		msleep(1000);
		debug(("register = %08X\n",
		       sata_readl(SATA_PORT0_SATA3_PCB_REG1)));
	}
	if (cnt >= 30) {
		/* PLL did not lock; give up */
		printf("SATA port 0 PLL did not lock\n");
		return 0;
	}

	printf("set up port 1\n");
	/* Access PLL register bank 1 */
	sata_writel(0x00000060, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* Set intN_fb_en bit */
	sata_writel(0x00001DF8, SATA_PORT1_SATA3_PCB_REG2);
	/* Select integer divide mode (instead of fractional) */
	sata_writel(0x00002B00, SATA_PORT1_SATA3_PCB_REG3);
	/* Set PLL divider to 60 */
	sata_writel(0x00008824, SATA_PORT1_SATA3_PCB_REG4);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* Set oob_clk_sel to refclk/2 */
	sata_writel(0x00000001, SATA_PORT1_SATA3_PCB_REG13);
	/* Access SATA AEQ register bank */
	sata_writel(0x000000D0, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* switch to EQ_reg for equaliser control */
	regval = sata_readl(SATA_PORT1_SATA3_PCB_REG2);
	regval |= (1 << 12);
	regval &= (~(1 << 15));	/* must write this bit zeroed */
	sata_writel(regval, SATA_PORT1_SATA3_PCB_REG2);
	sata_writel(3 | (3 << 4), SATA_PORT1_SATA3_PCB_REG3);
	/* Access SATA RXPMD register bank */
	sata_writel(0x000001C0, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	regval = (149 | (1 << 8));
	sata_writel(regval, SATA_PORT1_SATA3_PCB_REG7);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* strobe phy 1 reset */
	sata_writel(0x00000001, SATA_SATA_TOP_CTRL_PHY_CTRL_3);
	sata_writel(0x0000000E, SATA_SATA_TOP_CTRL_PHY_CTRL_4);
	sata_writel(0x00000000, SATA_SATA_TOP_CTRL_PHY_CTRL_3);
	sata_writel(0x00000000, SATA_SATA_TOP_CTRL_PHY_CTRL_4);
	/* wait for it to settle */
	msleep(100);
	/* Access BLOCK_0 register bank */
	sata_writel(0x00000000, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* poll pll_lock for the port */
	debug(("wait for pll lock on port 1\n"));
	for (cnt = 0;
	     ((cnt < 30) &&
	      (0 == (sata_readl(SATA_PORT1_SATA3_PCB_REG1) & 0x00001000)));
	     cnt++) {
		msleep(1000);
		debug(("register = %08X\n",
		       sata_readl(SATA_PORT1_SATA3_PCB_REG1)));
	}
	if (cnt >= 30) {
		/* PLL did not lock; give up */
		printf("SATA port 1 PLL did not lock\n");
		return 0;
	}

	debug(("leave config_sata_phy\n"));
	/* SATA phys are up and PLLs locked */
	return !0;
}

int config_sata_host(void)
{
	u32 reg;

	debug(("enter config_sata_host\n"));

	debug(("make sure SATA host not in reset\n"));
	writel((0 << SATA_M0_IDM_IDM_RESET_CONTROL__RESET_L),
	       SATA_M0_IDM_IDM_RESET_CONTROL);
	msleep(10);

	debug(("set port 0 OOB reference clock\n"));
	/* Set port 0 OOB control for 100MHz reference clock */
	/* Access port 0 OOB0 register bank */
	sata_writel(0x00000150, SATA_PORT0_SATA3_PCB_BLOCK_ADDR);
	/* ??? */
	sata_writel(0x0000C493, SATA_PORT0_SATA3_PCB_REG0);
	/* ??? */
	sata_writel(0x00001B89, SATA_PORT0_SATA3_PCB_REG1);

	debug(("set port 1 OOB reference clock\n"));
	/* Set port 1 OOB control for 100MHz reference clock */
	/* Access port 1 OOB0 register bank */
	sata_writel(0x00000150, SATA_PORT1_SATA3_PCB_BLOCK_ADDR);
	/* ??? */
	sata_writel(0x0000C493, SATA_PORT1_SATA3_PCB_REG0);
	/* ??? */
	sata_writel(0x00001B89, SATA_PORT1_SATA3_PCB_REG1);

	debug(("change 'hardware' parmeters\n"));
	/* Change exposed parameters for the device */
	/* Allow override of hardware defaults */
	reg = sata_readl(SATA_SATA_TOP_CTRL_BUS_CTRL);
	reg |= (1 << 16);
	sata_writel(reg, SATA_SATA_TOP_CTRL_BUS_CTRL);
	/* Adjust some values in the capabilities */
	reg = sata_readl(SATA_SATA_AHCI_GHC_HBA_CAP);
	reg |= (1 << 16);	/* support FBS on PMs */
	sata_writel(reg, SATA_SATA_AHCI_GHC_HBA_CAP);
	/* Adjsut some values in PxFBS */
	reg = sata_readl(SATA_SATA_PORT0_AHCI_S1_PxCLB + 0x40);
	reg &= (~(0xF << 12));
	reg |= (0x2 << 12);
	sata_writel(reg, SATA_SATA_PORT0_AHCI_S1_PxCLB + 0x40);
	reg = sata_readl(SATA_SATA_PORT1_AHCI_S1_PxCLB + 0x40);
	reg &= (~(0xF << 12));
	reg |= (0x2 << 12);
	sata_writel(reg, SATA_SATA_PORT1_AHCI_S1_PxCLB + 0x40);
	/* disable further changes and set endianness */
	/* FIXME: probably need to find core endiannes at compile/run time */
	reg = sata_readl(SATA_SATA_TOP_CTRL_BUS_CTRL);
	reg &= (~(1 << 16));
	sata_writel(reg, SATA_SATA_TOP_CTRL_BUS_CTRL);

	reg = sata_readl(SATA_SATA_PORT0_CTRL_PCTRL5);
	reg |= 0x01400810;
	sata_writel(reg, SATA_SATA_PORT0_CTRL_PCTRL5);
	reg = sata_readl(SATA_SATA_PORT1_CTRL_PCTRL5);
	reg |= 0x01400810;
	sata_writel(reg, SATA_SATA_PORT1_CTRL_PCTRL5);

	debug(("avoid unsolicited cominit\n"));
	/* avoid unsolicited COMINIT??? */
	reg = sata_readl(SATA_SATA_PORT0_CTRL_PCTRL3);
	reg &= 0xFFFF0000;
	sata_writel(reg, SATA_SATA_PORT0_CTRL_PCTRL3);
	reg = sata_readl(SATA_SATA_PORT1_CTRL_PCTRL3);
	reg &= 0xFFFF0000;
	sata_writel(reg, SATA_SATA_PORT1_CTRL_PCTRL3);

	debug(("???\n"));
	/* ??? */
	reg = sata_readl(SATA_SATA_PORT0_CTRL_PCTRL4);
	reg |= 0x01024000;
	sata_writel(reg, SATA_SATA_PORT0_CTRL_PCTRL4);
	reg = sata_readl(SATA_SATA_PORT1_CTRL_PCTRL4);
	reg |= 0x01024000;
	sata_writel(reg, SATA_SATA_PORT1_CTRL_PCTRL4);

	debug(("leave config_sata_host\n"));
	/* finished successfully */
	return !0;
}
