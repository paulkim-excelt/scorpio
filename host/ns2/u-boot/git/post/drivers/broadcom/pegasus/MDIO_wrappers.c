/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 */

#include <linux/types.h>
#include <asm/arch-bcm_pegasus/bcm_mdio.h>

/* NS2 internal MDIO chains */
#define PCIEA_BUSID	0x0
/*#define PCIEA_PHYID	0x0*/
/* Pegasus phyID->0x1 */
#define PCIEA_PHYID	0x1

#if 0
#define USB3_SS0_BUSID	0x1
#define USB3_SS0_PHYID	0x0

#define USB2_HS0_BUSID	0x2
#define USB2_HS0_PHYID	0x0

#define USB2_HS1_BUSID	0x3
#define USB2_HS1_PHYID	0x0

#define USB2_DRD_BUSID	0x4
#define USB2_DRD_PHYID	0x0

#define	EAGLE_BUSID	0x5
#define EAGLE_PHYID	0x0
/**************************************************/

#define SATA_BUSID	0x6
#define SATA_PORT0_PHYID	0x1
#define SATA_PORT1_PHYID	0x2

#define PCIEB_BUSID	0x7
#define PCIEB_PHYID	0x0

/* External PHY: BCM54810 */
#define PHY54810_BUSID	0x0
#define PHY54810_PHYID	0x10
#endif

/* PCIE-A */
void mdio_write_pcie_a(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, PCIEA_BUSID, PCIEA_PHYID, reg, v);
}

uint16_t mdio_read_pcie_a(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, PCIEA_BUSID, PCIEA_PHYID, reg);
}

#if 0
/* PCIE-B */
void mdio_write_pcie_b(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, PCIEB_BUSID, PCIEB_PHYID, reg, v);
}

uint16_t mdio_read_pcie_b(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, PCIEB_BUSID, PCIEB_PHYID, reg);
}

/* USB3 */
void mdio_write_usb30(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, USB3_SS0_BUSID, USB3_SS0_PHYID, reg,
		       v);
}

uint16_t mdio_read_usb30(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, USB3_SS0_BUSID, USB3_SS0_PHYID,
			     reg);
}

/* USB2 HS0 */
void mdio_write_usb20_hs0(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, USB2_HS0_BUSID, USB2_HS0_PHYID, reg,
		       v);
}

uint16_t mdio_read_usb20_hs0(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, USB2_HS0_BUSID, USB2_HS0_PHYID,
			     reg);
}

/* USB2 HS1 */
void mdio_write_usb20_hs1(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, USB2_HS1_BUSID, USB2_HS1_PHYID, reg,
		       v);
}

uint16_t mdio_read_usb20_hs1(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, USB2_HS1_BUSID, USB2_HS1_PHYID,
			     reg);
}

/* USB2 DRD */
void mdio_write_usb20_drd(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, USB2_DRD_BUSID, USB2_DRD_PHYID, reg,
		       v);
}

uint16_t mdio_read_usb20_drd(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, USB2_DRD_BUSID, USB2_DRD_PHYID,
			     reg);
}

/* SATA 0 */
void mdio_write_sata_port0(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, SATA_BUSID, SATA_PORT0_PHYID, reg, v);
}

uint16_t mdio_read_sata_port0(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, SATA_BUSID, SATA_PORT0_PHYID,
			     reg);
}

/* SATA 1 */
void mdio_write_sata_port1(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, SATA_BUSID, SATA_PORT1_PHYID, reg, v);
}

uint16_t mdio_read_sata_port1(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, SATA_BUSID, SATA_PORT1_PHYID,
			     reg);
}

void mdio_write_extphy(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(EXTERNAL, CLAUS22, PHY54810_BUSID, PHY54810_PHYID, reg,
		       v);
}

uint16_t mdio_read_extphy(uint32_t reg)
{
	return bcm_mdio_read(EXTERNAL, CLAUS22, PHY54810_BUSID, PHY54810_PHYID,
			     reg);
}

void mdio_write_eagle(uint32_t reg, uint16_t v)
{
	bcm_mdio_write(INTERNAL, CLAUS22, EAGLE_BUSID, EAGLE_PHYID, reg, v);
}

uint16_t mdio_read_eagle(uint32_t reg)
{
	return bcm_mdio_read(INTERNAL, CLAUS22, EAGLE_BUSID, EAGLE_PHYID, reg);
}
#endif
