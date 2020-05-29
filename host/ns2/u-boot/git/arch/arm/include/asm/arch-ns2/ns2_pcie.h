/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef NS2_PCIE_H
#define NS2_PCIE_H

#include <pci.h>
enum ns2_pcie_type {
	IPROC_PCI_GEN,
	IPROC_PCI_NITRO,
};

struct ns2_pcie {
	void __iomem *reg;

	struct pci_controller hose;
	enum ns2_pcie_type type;
};

#define GEN_CFG_IND_ADDR_OFFSET		0x120
#define GEN_CFG_IND_DATA_OFFSET		0x124

#define NITRO_CFG_IND_ADDR_OFFSET	0x1f0
#define NITRO_CFG_IND_DATA_OFFSET	0x1f4

#define CLK_CONTROL_OFFSET		0x000
#define EP_PERST_SOURCE_SELECT_SHIFT	2
#define EP_PERST_SOURCE_SELECT		(1 << EP_PERST_SOURCE_SELECT_SHIFT)
#define EP_MODE_SURVIVE_PERST_SHIFT	1
#define EP_MODE_SURVIVE_PERST		(1 << EP_MODE_SURVIVE_PERST_SHIFT)
#define RC_PCIE_RST_OUTPUT_SHIFT	0
#define RC_PCIE_RST_OUTPUT		(1 << RC_PCIE_RST_OUTPUT_SHIFT)

#define CFG_ADDR_OFFSET			0x1F8
#define CFG_ADDR_BUS_NUM_SHIFT		20
#define CFG_ADDR_BUS_NUM_MASK		0x0FF00000
#define CFG_ADDR_DEV_NUM_SHIFT		15
#define CFG_ADDR_DEV_NUM_MASK		0x000F8000
#define CFG_ADDR_FUNC_NUM_SHIFT		12
#define CFG_ADDR_FUNC_NUM_MASK		0x00007000
#define CFG_ADDR_REG_NUM_SHIFT		2
#define CFG_ADDR_REG_NUM_MASK		0x00000FFC
#define CFG_ADDR_CFG_TYPE_SHIFT		0
#define CFG_ADDR_CFG_TYPE_MASK		0x00000003

#define CFG_DATA_OFFSET			0x1FC
#define CFG_IND_ADDR_MASK		0x00001FFC

#define PCIE_LINK_STATUS_OFFSET		0xF0C
#define PCIE_PHYLINKUP_SHITF		3
#define PCIE_PHYLINKUP			(1 << PCIE_PHYLINKUP_SHITF)
#define PCIE_DL_ACTIVE_SHIFT		2
#define PCIE_DL_ACTIVE			(1 << PCIE_DL_ACTIVE_SHIFT)

#define INVALID_ACCESS_OFFSET		0xFFFFFFFF

int ns2_pcie_read_config(struct pci_controller *hose, pci_dev_t d, int where,
			 u32 *val);

int ns2_pcie_write_config(struct pci_controller *hose, pci_dev_t d, int where,
			  u32 val);

int ns2_pcie_init_hose(struct ns2_pcie *pcie);

void ns2_pcie_reset(struct ns2_pcie *pcie);
#endif
