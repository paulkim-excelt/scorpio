/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <pci.h>
#include <asm/arch/bcm_mdio.h>
#include <asm/io.h>
#include <linux/sizes.h>
#include <linux/compat.h>
#include <asm/arch/socregs.h>
#include <asm/arch/ns2_pcie.h>

#ifdef DEBUG
#define pr_debug	printf
#else
#define pr_debug(...)	do {} while (0)
#endif

static u32 ns2_pcie_conf_access(struct pci_controller *hose, pci_dev_t d,
				int where)
{
	struct ns2_pcie *pcie = hose->priv_data;
	int bus, dev, func;
	u32 val;

	if (!pcie || !pcie->reg)
		return INVALID_ACCESS_OFFSET;

	bus = PCI_BUS(d);
	dev = PCI_DEV(d);
	func = PCI_FUNC(d);
	/* root complex access */
	if (PCI_BUS(d) == 0) {
		/* For RC, only device=0 and func=0 are applicable */
		if ((dev > 1) || (func > 0))
			return INVALID_ACCESS_OFFSET;

		if (pcie->type == IPROC_PCI_NITRO) {
			writel(where & CFG_IND_ADDR_MASK,
			       pcie->reg + NITRO_CFG_IND_ADDR_OFFSET);
			return NITRO_CFG_IND_DATA_OFFSET;
		}
		writel(where & CFG_IND_ADDR_MASK,
		       pcie->reg + GEN_CFG_IND_ADDR_OFFSET);
		return GEN_CFG_IND_DATA_OFFSET;
	}

	/* access of EP device */
	val = (bus << CFG_ADDR_BUS_NUM_SHIFT) |
	      (dev << CFG_ADDR_DEV_NUM_SHIFT) |
	      (func << CFG_ADDR_FUNC_NUM_SHIFT) |
	      (where & CFG_ADDR_REG_NUM_MASK) |
	      (1 & CFG_ADDR_CFG_TYPE_MASK);
	writel(val, pcie->reg + CFG_ADDR_OFFSET);

	return CFG_DATA_OFFSET;
}

/*
 *  By default, the PCIe RC propagates faults, even involved in enumeration, to
 *  the processor.  We don't want that when we are enumerating -- in this case
 *  we expect to sometimes try to access something that is not there (how else
 *  can we see whether something is there but to look?)...
 */
void ns2_pcie_apb_err_control(struct ns2_pcie *pcie, int enable)
{
	u32 val;

	if (pcie->type != IPROC_PCI_NITRO) {
		val = readl(pcie->reg + 0x0F40);
		if (enable)
			val |= 1;
		else
			val &= ~1;
		writel(val, pcie->reg + 0x0F40);
		readl(pcie->reg + 0x0F40);
	}
}

int ns2_pcie_read_config(struct pci_controller *hose, pci_dev_t d,
			 int where, u32 *val)
{
	struct ns2_pcie *pcie = hose->priv_data;
	u32 offset;
	unsigned short flags;
	unsigned short type;
	int     pcie_cap_pos;

	if (PCI_DEV(d) > 0) {
		pcie_cap_pos = pci_hose_find_capability(
					hose,
					PCI_BDF(PCI_BUS(d), 0, 0),
					PCI_CAP_ID_EXP);
		pci_hose_read_config_word(
					hose,
					PCI_BDF(PCI_BUS(d), 0, 0),
					pcie_cap_pos + 2, &flags);
		type = (flags & 0xf0) >> 4;
		if ((type == 0x5) || (type == 0x0)) {
			/* PCI_EXP_TYPE_UPSTREAM */
			/* Above is root port so simply return */
			pr_debug("UPSTREAM PORT so Skipping:%x:%x:%x\n",
				 PCI_BUS(d), PCI_DEV(d), PCI_FUNC(d));
			*val = 0xffffffff;
			return 0;
		}
	}
	offset = ns2_pcie_conf_access(hose, d, where);
	if (offset == INVALID_ACCESS_OFFSET) {
		*val = 0xffffffff;
		return 0;
	}

	if (pcie->type == IPROC_PCI_NITRO) {
		if ((PCI_BUS(d) == 1) &&
		    /* For NITRO, only four function are available on device=1*/
		    /* Next Access results in SError */
		    ((PCI_DEV(d) > 0) || (PCI_FUNC(d) > 3))) {
			pr_debug("Skipping request for dev and func\n");
			*val = 0xffffffff;
			return 0;
		}
	}

	ns2_pcie_apb_err_control(pcie, 0);
	*val = readl(pcie->reg + offset);
	ns2_pcie_apb_err_control(pcie, 1);

	if (pcie->type == IPROC_PCI_NITRO) {
		/* For Nitro bridge, need to report BRIDGE-CLASS forcefully */
		if ((PCI_BUS(d) == 0) && (where == 8)) {
			pr_debug("Received Class as:%x:\n", *val);
			*val = 0x06040011;
			pr_debug("Changed to pci-bridge:%x:\n", *val);
		}
	}

	return 0;
}

int ns2_pcie_write_config(struct pci_controller *hose, pci_dev_t d,
			  int where, u32 val)
{
	struct ns2_pcie *pcie = hose->priv_data;
	u32 offset;

	offset = ns2_pcie_conf_access(hose, d, where);
	if (offset == INVALID_ACCESS_OFFSET)
		return -EINVAL;

	ns2_pcie_apb_err_control(pcie, 0);
	writel(val, pcie->reg + offset);
	ns2_pcie_apb_err_control(pcie, 1);

	return 0;
}

int ns2_pcie_init_hose(struct ns2_pcie *pcie)
{
	u32 val32;

	/*
	 * Check for a device to be present.  Only proceed if one is present.
	 * If not, it will cause a spurious interrupt (which is found when
	 * booting Linux after the fact)
	 */
	printf("\n######\nPCI @ Domain:%x 0x%p\n######\n",
	       pcie->hose.domain, pcie->reg);
	if (pcie->type != IPROC_PCI_NITRO) {
		val32 = readl(pcie->reg + PCIE_LINK_STATUS_OFFSET);
		if (!(val32 & PCIE_PHYLINKUP) || !(val32 & PCIE_DL_ACTIVE)) {
			printf("LinkUp Not detected so exiting\n");
			return -ENODEV;
		}
	}

	pcie->hose.priv_data = pcie;

	/*
	 *  FIXME - Major hack below.  This should be more limited than saying
	 * all of memory
	 */
	pci_set_region(&pcie->hose.regions[0], 0, 0, SZ_2G * 2 - 1,
		       PCI_REGION_MEM);
	pcie->hose.region_count = 1;

	pci_set_ops(&pcie->hose,
		    pci_hose_read_config_byte_via_dword,
		    pci_hose_read_config_word_via_dword,
		    ns2_pcie_read_config,
		    pci_hose_write_config_byte_via_dword,
		    pci_hose_write_config_word_via_dword,
		    ns2_pcie_write_config);
	if (pcie->type != IPROC_PCI_NITRO) {
		/* Workaround to return BRIDGE-CLASS for non-nitro RCs */
		writel(0x43c, pcie->reg + 0x120);
		writel(0x11060400, pcie->reg + 0x124);
	}

	pci_register_hose(&pcie->hose);
	pciauto_config_init(&pcie->hose);
	pcie->hose.last_busno = pci_hose_scan(&pcie->hose);

	return 0;
}

void ns2_pcie_reset(struct ns2_pcie *pcie)
{
	if (pcie->type == IPROC_PCI_NITRO) {
		writel(0x0000007F, pcie->reg + CLK_CONTROL_OFFSET);
	} else {
		u32 val;

		/*
		 * Select perst_b signal as reset source, and put the device in
		 * reset
		 */
		val = readl(pcie->reg + CLK_CONTROL_OFFSET);
		val &= ~EP_PERST_SOURCE_SELECT & ~EP_MODE_SURVIVE_PERST &
			~RC_PCIE_RST_OUTPUT;
		writel(val, pcie->reg + CLK_CONTROL_OFFSET);
		udelay(250);

		/* now bring it out of reset*/
		val |= RC_PCIE_RST_OUTPUT;
		writel(val, pcie->reg + CLK_CONTROL_OFFSET);
		mdelay(1000);
	}
}

