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
#include <linux/compat.h>
#include <asm/arch/ns2_nvram.h>
#include <asm/arch/chimp.h>
#include <asm/arch/ns2_sata.h>
#include <asm/arch/socregs.h>
#include <asm/arch/ns2_pcie.h>
#include <asm/arch/bcm_mdio.h>
#include <asm/arch-iproc/sys_proto.h>
#include <usb/ehci-ci.h>
#include <usb/xhci-iproc.h>
#include <usb/ehci-bcm-ns2.h>
#include <ahci.h>
#include <scsi.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

#define USB2H_OHCI_EHCI_STRAP__PPC_INVERSION		12

extern int bcmiproc_eth_register(u8 dev_num);

int board_init(void)
{
#ifdef CONFIG_BCM_CMIC_MDIO
	bcm_mdio_init();
#endif
	return 0;
}

int dram_init(void)
{
	gd->ram_size = 2 * 1024 * 1024 * 1024LL;
	return 0;
}

void dram_init_banksize(void)
{
	unsigned int i;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

#if (CONFIG_NR_DRAM_BANKS > 1)
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
#endif /* (CONFIG_NR_DRAM_BANKS > 1) */

#if (CONFIG_NR_DRAM_BANKS > 2)
	gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
	gd->bd->bi_dram[2].size = PHYS_SDRAM_3_SIZE;
#endif /* (CONFIG_NR_DRAM_BANKS > 2) */

	printf("Memory regions list (%u regions total)\n",
	       CONFIG_NR_DRAM_BANKS);
	for (i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		printf("Memory region %u: %016llX..%016llX (%016llX)\n",
		       i,
		       gd->bd->bi_dram[i].start,
		       gd->bd->bi_dram[i].start + gd->bd->bi_dram[i].size - 1,
		       gd->bd->bi_dram[i].size);
	}
}

#ifdef CONFIG_LMB
/*
 *  Instead of reserving memory we don't want touched, we only admit to having
 *  memory that can be touched.  Unhappily, this means there isn't enough
 *  memory to deal with certain things (such as a Linux kernel and initrd) in
 *  the first region of memory (and, honestly, probably not the second too).
 *
 *  Add memory regions that we have up to the 4GiB mark, in hopes it is enough.
 *  Skip the initial memory region because it was automatically added.
 */
void board_lmb_reserve(struct lmb *lmb)
{
	int i;
	u64 end;
	for (i = 1; i < CONFIG_NR_DRAM_BANKS; i++) {
		end = gd->bd->bi_dram[i].size + gd->bd->bi_dram[i].start;
		if (end > 0x0000000100000000ull) {
			/* don't go beyond 4GiB physical */
			end = 0x0000000100000000ull;
		}
		if (end < gd->bd->bi_dram[i].start) {
			/* this one is entirely out of range; skip it */
			continue;
		}
		lmb_add(lmb,
			(phys_addr_t)gd->bd->bi_dram[1].start,
			(phys_addr_t)(end - gd->bd->bi_dram[i].start));
	}
}
#endif /* def CONFIG_LMB */

void reset_cpu(ulong addr)
{
	uint reg;

	reg = readl(DMU_COMMON_ROOT + CRMU_SOFT_RESET_CTRL);
	writel(reg & ~(1 << CRMU_SOFT_RESET_CTRL__SOFT_SYS_RST_L),
	       DMU_COMMON_ROOT + CRMU_SOFT_RESET_CTRL);
}

int board_eth_init(bd_t *bis)
{
	int rc = 0;
	u32 id = 0, rev = 0;

#ifdef CONFIG_BCMIPROC_ETH
	iproc_get_soc_id(&id, &rev);
	if ((id == IPROC_SOC_NS2) && (rev == IPROC_SOC_NS2_B0))
		writel(0x0196e800, (NICPM_ROOT + NICPM_IOMUX_CTRL));
	else
		writel(0x21880000, (NICPM_ROOT + NICPM_IOMUX_CTRL));

	printf("Registering Northstar2 GMAC ethernet.....\n");
	rc = bcmiproc_eth_register(0);
#endif

#ifdef CONFIG_E1000
	rc = e1000_initialize(0);
	printf("%d e1000 interfaces found\n", rc);
#endif
	return rc;
}

#define PCA9506_I2C_SLAVE_ADDR 0x24
#define PCA9506_BANK3_IO_CFG_REG 0x1B
#define PCA9506_BANK3_PIN2_SHIFT 2
#define PCA9506_BANK3_PIN2 BIT(PCA9506_BANK3_PIN2_SHIFT)
#define PCA9506_BANK3_OUTPUT_CFG_REG 0x0B
#define SDIO_EN_SHIFT 2
#define SDIO_EN BIT(SDIO_EN_SHIFT)

#define PCF8574_I2C_SLAVE_ADDR 0x21
#define SDIO0_VDD_CTRL_SHIFT 6
#define SDIO0_VDD_CTRL BIT(SDIO0_VDD_CTRL_SHIFT)
#define SDIO1_VDD_CTRL_SHIFT 7
#define SDIO1_VDD_CTRL BIT(SDIO1_VDD_CTRL_SHIFT)
static void reset_card(void)
{
	unsigned char reg_offset;
	unsigned char value;

	/*
	 * We need to toggle power for SDIO card in graceful way for proper
	 * functioning.
	 * 1) Disable SDIO_EN (Bank3:Pin2 of PCA9506 I2C exander on I2C0
	 *    slave addr 0x24)
	 * 2) Toggle SDIO0_VDD_CTRL,SDIO1_VDD_CTRL(Pin6,7 of PCF8574TS I2C
	 *    exander on I2C0 slave addr 0x21 and then
	 * 3) Enable SDIO_EN (Bank3:Pin2 of PCA9506 I2C exander on I2C0
	 *    slave addr 0x24)
	 */

	i2c_set_bus_num(0);

	if (!i2c_probe(PCA9506_I2C_SLAVE_ADDR)) {
		printf("Toggling power for SDIO cards\n");

		reg_offset = PCA9506_BANK3_IO_CFG_REG;
		i2c_read(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		value &= ~PCA9506_BANK3_PIN2; /* Make pin 2 writable */
		i2c_write(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);

		reg_offset = PCA9506_BANK3_OUTPUT_CFG_REG;
		i2c_read(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		value &= ~SDIO_EN; /* Power Down SDIO_EN */
		i2c_write(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		mdelay(100);

		value = 0xff; /* set all bits */
		/* Reset to 1.8V */
		value &= (SDIO0_VDD_CTRL_SHIFT | SDIO1_VDD_CTRL_SHIFT);
		i2c_write_ctrl_bytes(PCF8574_I2C_SLAVE_ADDR, &value, 1);
		mdelay(100);

		/* Switch to 3.3V */
		value |= (SDIO0_VDD_CTRL_SHIFT | SDIO1_VDD_CTRL_SHIFT);
		i2c_write_ctrl_bytes(PCF8574_I2C_SLAVE_ADDR, &value, 1);
		mdelay(100);

		reg_offset = PCA9506_BANK3_OUTPUT_CFG_REG;
		i2c_read(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		value |= SDIO_EN; /* Power Up SDIO */
		i2c_write(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);

		reg_offset = PCA9506_BANK3_IO_CFG_REG;
		i2c_read(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		value |= PCA9506_BANK3_PIN2; /* Make pin 2 readable */
		i2c_write(PCA9506_I2C_SLAVE_ADDR, reg_offset, 1, &value, 1);
		printf("Toggled\n");
	} else {
		printf("failed to probe i2c device on I2C0\n");
	}
}

#ifdef CONFIG_BOARD_LATE_INIT
int board_late_init(void)
{
	int ret = 0;

#ifdef CONFIG_BCM_NS2_CHIMP
	printf("Preparing NVRAM access...\n");
	ret = ns2_nvram_open(0);
		if (ret < 0)
			printf("NVRAM access failed\n");

	printf("Loading CHIMP firmware ..\n");
	ret = load_chimp_firmware();
	if (ret < 0)
		printf("Loading CHIMP firmware ...failed with rc %d\n", ret);
	else
		printf("Loading CHIMP firmware ...done\n");
#endif
#ifdef CONFIG_SCSI_AHCI_PLAT
	printf("Enabling onboard SATA host\n");
	if (config_sata_host() && config_sata_phy()) {
		ahci_init((void __iomem *)
			  (SATA_SATA_AHCI_GHC_HBA_CAP + SATA_ROOT));
		scsi_scan(1);
	}
#endif
	reset_card();
	return 0;
}
#endif

int board_ehci_hcd_init(host)
{
	u32 reg_data = 0;
	char *var = 0;

	/* Reverse over current polarity  for drd port */
	if (host == 0) {
		reg_data = readl(USB2H_DRD_OHCI_EHCI_STRAP);
		reg_data |=
			(1 << USB2H_DRD_OHCI_EHCI_STRAP__OHCI_APP_PORT_OVC_POL);
		writel(reg_data, USB2H_DRD_OHCI_EHCI_STRAP);
	}
#ifndef CONFIG_BCM_NS2_CUPS_DDR4
	var = getenv("board_name");
	if (var && ((strcmp(var, "BCM958710K_0000") == 0) ||
		    (strcmp(var, "BCM958712K_0000") == 0))) {
#else
	{
#endif
		if (host == 1) {
			reg_data = readl(USB2H_M0_OHCI_EHCI_STRAP);
			reg_data |=
			(1 << USB2H_M0_OHCI_EHCI_STRAP__OHCI_APP_PORT_OVC_POL);
			reg_data |= (1 << USB2H_OHCI_EHCI_STRAP__PPC_INVERSION);
			writel(reg_data, USB2H_M0_OHCI_EHCI_STRAP);
		}

		if (host == 2) {
			reg_data = readl(USB2H_M1_OHCI_EHCI_STRAP);
			reg_data |=
			(1 << USB2H_M1_OHCI_EHCI_STRAP__OHCI_APP_PORT_OVC_POL);
			reg_data |= (1 << USB2H_OHCI_EHCI_STRAP__PPC_INVERSION);
			writel(reg_data, USB2H_M1_OHCI_EHCI_STRAP);
		}
	}

	return 0;
}

void pci_init_board(void)
{
	struct ns2_pcie *pcie;
	int i, rc;

	printf("PCIe Init\n");
	/* Do PCI slots first */
	for (i = 0; i < 2; i++) {
		pcie = kzalloc(sizeof(*pcie), GFP_KERNEL);
		if (!pcie)
			return;

		pcie->type = IPROC_PCI_GEN;
		pcie->hose.first_busno = 0;
		pcie->hose.domain = i * 4;

		/* map registers */
		pcie->reg = (void *)PAXB_0_CLK_CONTROL + (0x30000000 * i);

		/* turn on the phys. NOTE: Only for NS2. */
		bcm_mdio_write(INTERNAL, CLAUS22, i * 7, 0, 0x1F, 0x2100);
		bcm_mdio_write(INTERNAL, CLAUS22, i * 7, 0, 0x3, 0x2b18);

		ns2_pcie_reset(pcie);

		rc = ns2_pcie_init_hose(pcie);
		if (rc)
			kfree(pcie);
	}

	/* Now do Nitro */
	pcie = kzalloc(sizeof(*pcie), GFP_KERNEL);
	if (!pcie)
		return;

	pcie->type = IPROC_PCI_NITRO;
	pcie->hose.first_busno = 0;
	pcie->hose.domain = i * 4;
	pcie->reg = (void *)PAXC_ROOT;

	ns2_pcie_reset(pcie);

	rc = ns2_pcie_init_hose(pcie);
	if (rc)
		kfree(pcie);
}

void arch_preboot_os(void)
{
	/* Do USB3 phy init to program XHCI histogram register*/
	xhci_phy_init();
	/* Put PHY back in reset */
	xhci_core_phy_in_reset();

#ifdef CONFIG_USB_EHCI
	usbh_clock_disable_m0();
	usbh_clock_disable_m1();
#endif
}
