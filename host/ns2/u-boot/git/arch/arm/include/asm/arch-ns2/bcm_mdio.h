/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

/*
 * MDIO routines for CMIC internal/external MDIO bus
 */
enum {
	INTERNAL,
	EXTERNAL
};

enum {
	CLAUS22,
	CLAUS45
};

/* NS2 internal MDIO chains */
#define PCIEA_BUSID		0x0
#define PCIEA_PHYID		0x0

#define USB3_SS0_BUSID		0x1
#define USB3_SS0_PHYID		0x0

#define USB2_HS0_BUSID		0x2
#define USB2_HS0_PHYID		0x0

#define USB2_HS1_BUSID		0x3
#define USB2_HS1_PHYID		0x0

#define USB2_DRD_BUSID		0x4
#define USB2_DRD_PHYID		0x0

#define EAGLE_BUSID		0x5
#define EAGLE_PHYID		0x0

#define SATA_BUSID		0x6
#define SATA_PORT0_PHYID	0x1
#define SATA_PORT1_PHYID	0x2

#define PCIEB_BUSID		0x7
#define PCIEB_PHYID		0x0

/* External PHY: BCM54810 */
#define PHY54810_BUSID		0x0
#define PHY54810_PHYID		0x10

/*  API */
void bcm_mdio_init(void);
void bcm_mdio_write(u32 ext, u32 c45, u32 busid,
		    u32 phyaddr, u32 reg, u16 v);
u16 bcm_mdio_read(u32 ext, u32 c45, u32 busid,
		  u32 phyaddr, u32 reg);
