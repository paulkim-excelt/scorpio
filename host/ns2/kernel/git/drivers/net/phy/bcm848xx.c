/*
 * Copyright (C) 2017 Broadcom
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include "bcm-phy-lib.h"
#include <linux/brcmphy.h>
#include <linux/module.h>
#include <linux/phy.h>
#include <linux/of.h>
#include <linux/delay.h>
#include <linux/firmware.h>

#define PHY_BCM8488X_FEATURES	(SUPPORTED_10000baseR_FEC | \
				 PHY_BASIC_FEATURES)

#define BCM848XX_EXEC_FW_SUCCESSFULLY_VAL 0x2040

#define BCM848XX_10G_RPMD_CTRL_REG		(MII_ADDR_C45 | 0x10000)
#define BCM848XX_DEVICE_IDENTIFIER_REG   (MII_ADDR_C45 | 0x10002)
#define BCM848XX_PMD_RX_SIGNAL_DETECT	(MII_ADDR_C45 | 0x1000a)

#define BCM848XX_MAILBOX_REG				(MII_ADDR_C45 | 0x1A008)

#define BCM848XX_DOWNLOAD_CTRL_REG		(MII_ADDR_C45 | 0x1A817)
#define BCM848XX_DOWNLOAD_STATUS_REG		(MII_ADDR_C45 | 0x1A818)
#define BCM848XX_DOWNLOAD_ADDR_LOW_REG	(MII_ADDR_C45 | 0x1A819)
#define BCM848XX_DOWNLOAD_ADDR_HIGH_REG	(MII_ADDR_C45 | 0x1A81A)
#define BCM848XX_DOWNLOAD_DATA_LOW_REG	(MII_ADDR_C45 | 0x1A81B)
#define BCM848XX_DOWNLOAD_DATA_HIGH_REG	(MII_ADDR_C45 | 0x1A81C)

#define BCM848XX_1G_100M_MII_CTRL_REG	(MII_ADDR_C45 | 0x7FFE0)
#define BCM848XX_1G_100M_AUX_STATUS_REG	(MII_ADDR_C45 | 0x7FFF9)

#define BCM848XX_USER_CONTROL_REG	(MII_ADDR_C45 | 0x1E4005)
#define BCM848XX_USER_STATUS_REG		(MII_ADDR_C45 | 0x1E400D)
#define BCM848XX_USER_FW_REV_REG		(MII_ADDR_C45 | 0x1E400F)
#define BCM848XX_USER_FW_DATE_REG	(MII_ADDR_C45 | 0x1E4010)

#define BCM848XX_USER_DEFINE_REG_4181	(MII_ADDR_C45 | 0x1E4181)
#define BCM848XX_USER_DEFINE_REG_4186	(MII_ADDR_C45 | 0x1E4186)
#define BCM848XX_USER_DEFINE_REG_4188	(MII_ADDR_C45 | 0x1E4188)
#define BCM848XX_USER_DEFINE_REG_418C	(MII_ADDR_C45 | 0x1E418C)

#define BCM848XX_CONTROL_REG_8004	(MII_ADDR_C45 | 0x1E8004)

#define BCM848XX_10GBASER_PCS_STATUS	(MII_ADDR_C45 | 0x30020)

#define PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT   9
#define PHY848XX_1G_100M_AUX_HCD_MASK					 0x700
#define PHY848XX_USER_STATUS_CRC_CHK_MASK				 0xC000
#define PHY848XX_USER_STATUS_CRC_GOOD_VAL				 0x4000
#define PHY848XX_USER_STATUS_MAC_LINK_STATUS_MASK        0x2000
#define PHY848XX_USER_STATUS_MAC_LINK_STATUS_SHIFT       13
#define PHY848XX_USER_STATUS_COP_LINK_STATUS_MASK        0x20
#define PHY848XX_USER_STATUS_COP_LINK_STATUS_SHIFT       5
#define PHY848XX_USER_STATUS_COP_SPEED_MASK              0x1c
#define PHY848XX_USER_STATUS_COP_SPEED_SHIFT             2
#define PHY848XX_USER_STATUS_COP_DETECT_MASK             0x2
#define PHY848XX_USER_STATUS_COP_DETECT_SHIFT            1

const struct firmware *fw;
#define FIRMWARE_BCM848XX   "BCM848xx_FW.bin"
MODULE_FIRMWARE(FIRMWARE_BCM848XX);

#if IS_ENABLED(CONFIG_OF_MDIO)
/* Set and/or override some configuration registers based on the
 * brcm,c45-reg-init property stored in the of_node for the phydev.
 *
 * brcm,c45-reg-init = <devid reg mask value>,...;
 *
 * There may be one or more sets of <devid reg mask value>:
 *
 * devid: which sub-device to use.
 * reg: the register.
 * mask: if non-zero, ANDed with existing register value.
 * value: ORed with the masked value and written to the regiser.
 *
 */
static int bcm848XX_of_reg_init(struct phy_device *phydev)
{
	const __be32 *paddr;
	const __be32 *paddr_end;
	int len, ret;

	if (!phydev->mdio.dev.of_node)
		return 0;

	paddr = of_get_property(phydev->mdio.dev.of_node,
				"brcm,c45-reg-init", &len);
	if (!paddr)
		return 0;

	paddr_end = paddr + (len /= sizeof(*paddr));

	ret = 0;

	while (paddr + 3 < paddr_end) {
		u16 devid	= be32_to_cpup(paddr++);
		u16 reg		= be32_to_cpup(paddr++);
		u16 mask	= be32_to_cpup(paddr++);
		u16 val_bits	= be32_to_cpup(paddr++);
		int val;
		u32 regnum = MII_ADDR_C45 | (devid << 16) | reg;

		val = 0;
		if (mask) {
			val = phy_read(phydev, regnum);
			if (val < 0) {
				ret = val;
				goto err;
			}
			val &= mask;
		}
		val |= val_bits;

		ret = phy_write(phydev, regnum, val);
		if (ret < 0)
			goto err;
	}
err:
	return ret;
}
#else
static int bcm848XX_of_reg_init(struct phy_device *phydev)
{
	return -EINVAL;
}
#endif /* CONFIG_OF_MDIO */

static void bcm848XX_write_reg32(struct phy_device *phydev, u32 ad, u32 value)
{
	u16 val = 0;

	val = (u16)(ad & 0xFFFF);
	phy_write(phydev, BCM848XX_DOWNLOAD_ADDR_LOW_REG, val);

	val = (u16)(ad >> 16);
	phy_write(phydev, BCM848XX_DOWNLOAD_ADDR_HIGH_REG, val);

	val = (u16)(value & 0xFFFF);
	phy_write(phydev, BCM848XX_DOWNLOAD_DATA_LOW_REG, val);

	val = (u16)(value >> 16);
	phy_write(phydev, BCM848XX_DOWNLOAD_DATA_HIGH_REG, val);

	phy_write(phydev, BCM848XX_DOWNLOAD_CTRL_REG, 0x0009);
}

static void bcm848XX_halt_system(struct phy_device *phydev)
{
	/* This procedure of halt system was described in AN with pseudocode.
	 * Both BCM8486x & BCM8488x use the same procedure.
	 * These regs are hidden registers in data sheet but necessary
	 * to get the PHY functioning
	 */
	phy_write(phydev, BCM848XX_USER_DEFINE_REG_418C, 0x0000);
	phy_write(phydev, BCM848XX_USER_DEFINE_REG_4188, 0x48f0);
	phy_write(phydev, BCM848XX_USER_DEFINE_REG_4186, 0x8000);
	phy_write(phydev, BCM848XX_USER_DEFINE_REG_4181, 0x017c);
	phy_write(phydev, BCM848XX_USER_DEFINE_REG_4181, 0x0040);

	/* There are 10 sets of cmd for set shading resiter with specified val.
	 * Here wrapping a set sequence by function: bcm848XX_write_reg32.
	 */
	bcm848XX_write_reg32(phydev, 0xC3000000, 0x00000010);
	bcm848XX_write_reg32(phydev, 0xFFFF0000, 0xE59F1018);
	bcm848XX_write_reg32(phydev, 0xFFFF0004, 0xEE091F11);
	bcm848XX_write_reg32(phydev, 0xFFFF0008, 0xE3A00000);
	bcm848XX_write_reg32(phydev, 0xFFFF000C, 0xE3A01806);
	bcm848XX_write_reg32(phydev, 0xFFFF0010, 0xE8A00002);
	bcm848XX_write_reg32(phydev, 0xFFFF0014, 0xE1500001);
	bcm848XX_write_reg32(phydev, 0xFFFF0018, 0x3AFFFFFC);
	bcm848XX_write_reg32(phydev, 0xFFFF001C, 0xEAFFFFFE);
	bcm848XX_write_reg32(phydev, 0xFFFF0020, 0x00040021);

	phy_write(phydev, BCM848XX_USER_DEFINE_REG_4181, 0x0000);
}

static int bcm848XX_load_firmware(struct phy_device *phydev)
{
	u16 val_h, val_l;
	int rc, i;

	/* This procedure of loading fw was described in AN with pseudocode.
	 * At the beginning, clear value of register for download addr
	 * and set specified value for download ctrl reg.
	 */
	phy_write(phydev, BCM848XX_DOWNLOAD_ADDR_LOW_REG, 0x0000);
	phy_write(phydev, BCM848XX_DOWNLOAD_ADDR_HIGH_REG, 0x0000);
	phy_write(phydev, BCM848XX_DOWNLOAD_CTRL_REG, 0x0038);

	rc = request_firmware(&fw, FIRMWARE_BCM848XX, &phydev->mdio.dev);
	if (rc) {
		phydev_err(phydev, "Fail load %s(%d)\n", FIRMWARE_BCM848XX, rc);
		return -EINVAL;
	}
	phydev_dbg(phydev, "Downloaded firmware size: %lu.\n", fw->size);

	for (i = 0; i < fw->size; i += 4) {
		val_l = *((u16 *)(fw->data + i));
		val_h = *((u16 *)(fw->data + i + 2));

		phy_write(phydev, BCM848XX_DOWNLOAD_DATA_HIGH_REG, val_h);
		phy_write(phydev, BCM848XX_DOWNLOAD_DATA_LOW_REG, val_l);
	}
	/* At the end, we clear shading regiter:0xc3000000 as AN described.
	 * The register can't be found in datasheet but necessary step in AN.
	 */
	bcm848XX_write_reg32(phydev, 0xC3000000, 0x00000000);

	return fw->size;
}

static int bcm848XX_soft_reset(struct phy_device *phydev)
{
	int timeout = 1000; /* wait for 1000ms */
	u16 val;

	phy_write(phydev, BCM848XX_10G_RPMD_CTRL_REG, 0x8000);
	do {
		val = phy_read(phydev, BCM848XX_10G_RPMD_CTRL_REG);
		if (val == BCM848XX_EXEC_FW_SUCCESSFULLY_VAL)
			break;

		usleep_range(1000, 2000);
	} while (timeout--);

	if (timeout == 0) {
		phydev_err(phydev, "Verify PHY status timeout!\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int bcm848XX_verify_crc(struct phy_device *phydev)
{
	int timeout = 1000; /* wait for 1000ms */
	int rc = 0;

	do {
		rc = phy_read(phydev, BCM848XX_USER_STATUS_REG);
		if ((rc & PHY848XX_USER_STATUS_CRC_CHK_MASK) != 0)
			break;
		usleep_range(1000, 2000);
	} while (timeout--);

	if (timeout == 0) {
		phydev_err(phydev, "Verify firmware CRC timeout!\n");
		return -ETIMEDOUT;
	}

	if ((rc & PHY848XX_USER_STATUS_CRC_CHK_MASK) !=
		  PHY848XX_USER_STATUS_CRC_GOOD_VAL) {
		phydev_err(phydev, "Verify firmware CRC wrong!\n");
		return -EINVAL;
	}

	/* Firmware version */
	rc = phy_read(phydev, BCM848XX_DEVICE_IDENTIFIER_REG);
	phydev_dbg(phydev, "PHY Identifier=0x%x\n", rc);

	rc = phy_read(phydev, BCM848XX_USER_FW_REV_REG);
	phydev_dbg(phydev, "Firmware REV=0x%x\n", rc);
	rc = phy_read(phydev, BCM848XX_USER_FW_DATE_REG);
	phydev_dbg(phydev, "Firmware Date=0x%x\n", rc);

	return 0;
}

static int bcm848XX_init(struct phy_device *phydev)
{
	int rc = 0;

	/* Step 0: Check if the FW is already loaded&run on the PHY device */
	rc = phy_read(phydev, BCM848XX_USER_FW_REV_REG);
	if (rc != 0) {
		pr_info("FW REV=0x%x detected, no re-download\n", rc);
		return 0;
	}

	/* Step 1: Halting BCM848xx system */
	bcm848XX_halt_system(phydev);

	/* Step 2: download firmware */
	rc = bcm848XX_load_firmware(phydev);
	if (rc <= 0)
		return rc;

	/* Setp 3: reset system */
	phy_write(phydev, BCM848XX_MAILBOX_REG, 0x0000);
	phy_write(phydev, BCM848XX_CONTROL_REG_8004, 0x5555);

	/* soft reset */
	rc = bcm848XX_soft_reset(phydev);
	if (rc != 0)
		return rc;

	/* Setp 5: verify CRC */
	rc = bcm848XX_verify_crc(phydev);

	return rc;
}

static int bcm848XX_config_init(struct phy_device *phydev)
{
	int rc;

	phydev->supported = PHY_BCM8488X_FEATURES;
	phydev->advertising = ADVERTISED_10000baseR_FEC;
	phydev->state = PHY_NOLINK;
	phydev->autoneg = AUTONEG_ENABLE;

	rc = bcm848XX_init(phydev);
	if (rc != 0)
		return rc;

	bcm848XX_of_reg_init(phydev);

	return 0;
}

static int bcm848XX_config_aneg(struct phy_device *phydev)
{
	int timeout = 1000; /* wait for 1000ms */
	u16 val = 0;
	u16 chk_restart;

	val = phy_read(phydev, BCM848XX_1G_100M_MII_CTRL_REG);
	val |= (1 << PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT);
	phy_write(phydev, BCM848XX_1G_100M_MII_CTRL_REG, val);

	do {
		val = phy_read(phydev, BCM848XX_1G_100M_MII_CTRL_REG);
		chk_restart = val &
			(1 << PHY848XX_MII_R00_CONTROL_RESTART_AUTONEG_SHIFT);
		if (chk_restart == 0)
			break;
		usleep_range(500, 1000);
	} while (timeout--);

	if (timeout <= 0) {
		phydev_err(phydev, "AutoNeg timeout!\n");
		return -ETIMEDOUT;
	}

	return 0;
}

static int bcm848XX_read_status(struct phy_device *phydev)
{
	int link_status, cop_link, mac_link, cop_detect;
	int link_speed;
	int duplex_status;

	link_status = phy_read(phydev, BCM848XX_USER_STATUS_REG);
	cop_link = (link_status & PHY848XX_USER_STATUS_COP_LINK_STATUS_MASK) >>
				PHY848XX_USER_STATUS_COP_LINK_STATUS_SHIFT;
	if (cop_link == 0)
		goto no_link;

	mac_link = (link_status & PHY848XX_USER_STATUS_MAC_LINK_STATUS_MASK) >>
				PHY848XX_USER_STATUS_MAC_LINK_STATUS_SHIFT;
	cop_detect = (link_status & PHY848XX_USER_STATUS_COP_DETECT_MASK) >>
				PHY848XX_USER_STATUS_COP_DETECT_SHIFT;
	link_speed = (link_status & PHY848XX_USER_STATUS_COP_SPEED_MASK) >>
			 PHY848XX_USER_STATUS_COP_SPEED_SHIFT;

	switch (link_speed) {
	case 2:
		phydev->speed = SPEED_100;
		break;
	case 4:
		phydev->speed = SPEED_1000;
		break;
	case 6:
		phydev->speed = SPEED_10000;
		break;
	case 1:
		phydev->speed = SPEED_2500;
		break;
	case 3:
		phydev->speed = SPEED_5000;
		break;
	default:
		phydev_err(phydev, "Unknown link speed:%d\n", link_speed);
		break;
	}

	phydev->link = 1;

	phydev->duplex = DUPLEX_FULL;
	if (phydev->speed <= SPEED_1000) {
		duplex_status = phy_read(phydev,
					 BCM848XX_1G_100M_AUX_STATUS_REG);
		duplex_status &= PHY848XX_1G_100M_AUX_HCD_MASK;
		switch (duplex_status) {
		case 0x110:
		case 0x011:
			phydev->duplex = DUPLEX_HALF;
			break;
		default:
			break;
		}
	}
	return 0;

no_link:
	phydev->link = 0;
	return 0;
}

static int bcm848XX_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int bcm848XX_did_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int bcm848XX_ack_interrupt(struct phy_device *phydev)
{
	/* Reading the LASI status clears it. */
	bcm848XX_did_interrupt(phydev);
	return 0;
}

int bcm8486x_match_phy_device(struct phy_device *phydev)
{
	return phydev->c45_ids.device_ids[1] == PHY_ID_BCM84861;
}

int bcm8488x_match_phy_device(struct phy_device *phydev)
{
	return phydev->c45_ids.device_ids[1] == PHY_ID_BCM84881;
}

static struct phy_driver BCM848XX_driver[] = {
{
	.phy_id		= PHY_ID_BCM84861,
	.phy_id_mask	= 0xfffffff0,
	.name		= "Broadcom BCM84861",
	.features	= PHY_BCM8488X_FEATURES,
	.config_init	= bcm848XX_config_init,
	.config_aneg	= bcm848XX_config_aneg,
	.read_status	= bcm848XX_read_status,
	.ack_interrupt	= bcm848XX_ack_interrupt,
	.config_intr	= bcm848XX_config_intr,
	.did_interrupt	= bcm848XX_did_interrupt,
	.match_phy_device = bcm8486x_match_phy_device,
	.soft_reset = bcm848XX_soft_reset,
}, {
	.phy_id		= PHY_ID_BCM84881,
	.phy_id_mask	= 0xfffffff0,
	.name		= "Broadcom BCM84881",
	.features	= PHY_BCM8488X_FEATURES,
	.config_init	= bcm848XX_config_init,
	.config_aneg	= bcm848XX_config_aneg,
	.read_status	= bcm848XX_read_status,
	.ack_interrupt	= bcm848XX_ack_interrupt,
	.config_intr	= bcm848XX_config_intr,
	.did_interrupt	= bcm848XX_did_interrupt,
	.match_phy_device = bcm8488x_match_phy_device,
	.soft_reset = bcm848XX_soft_reset,
} };

module_phy_driver(BCM848XX_driver);
MODULE_LICENSE("GPL v2");

static struct mdio_device_id __maybe_unused BCM848XX_tbl[] = {
	{ PHY_ID_BCM84861, 0xfffffff0 },
	{ PHY_ID_BCM84881, 0xfffffff0 },
	{ }
};

MODULE_DEVICE_TABLE(mdio, BCM848XX_tbl);

