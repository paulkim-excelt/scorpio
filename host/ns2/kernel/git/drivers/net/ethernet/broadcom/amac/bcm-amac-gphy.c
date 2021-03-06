/*
 * Copyright (C) 2015 Broadcom Corporation
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

#include <linux/types.h>
#include <linux/mdio.h>
#include <linux/mii.h>
#include <linux/netdevice.h>
#include <linux/of_mdio.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/kfifo.h>
#include <linux/brcmphy.h>

#include "bcm-amac-regs.h"
#include "bcm-amac-enet.h"
#include "bcm-amac-core.h"

static void amac_gphy_handle_link_change(struct net_device *ndev)
{
	struct bcm_amac_priv *privp = netdev_priv(ndev);
	struct phy_device *phydev = privp->port.ext_port.phydev;
	struct port_status *port_stat =
		&privp->port.ext_port.stat;

	/* Act on link, speed, duplex status changes */
	if ((phydev->link !=  port_stat->link) ||
	    (phydev->speed !=  port_stat->speed) ||
	    (phydev->duplex !=  port_stat->duplex)) {
		/* Update the new status */
		port_stat->link = phydev->link;
		port_stat->speed = phydev->speed;
		port_stat->duplex = phydev->duplex;

		if (!privp->switch_mode) {
			if (port_stat->link == 0)
				bcm_amac_enet_set_speed(privp,
							privp->port.
							imp_port_speed,
							DUPLEX_FULL);
			else
				bcm_amac_enet_set_speed(privp,
							port_stat->speed,
							port_stat->duplex);
		}
	}
}

static int amac_gphy_enable(struct phy_device *phy_dev, bool enable)
{
	int val;

	/* Read current value */
	val = phy_read(phy_dev, GPHY_MII_CTRL_REG);

	/* Set or Clear the Power and Reset bits */
	if (enable)
		val &= (~(GPHY_MII_CTRL_REG_RST_MASK |
			GPHY_MII_CTRL_REG_PWR_MASK));
	else
		val |= (GPHY_MII_CTRL_REG_PWR_MASK);

	return phy_write(phy_dev, GPHY_MII_CTRL_REG, val);
}

static int amac_gphy_lswap(struct phy_device *phy_dev)
{
	int rc = 0;
	u16 val;

	rc = phy_write(phy_dev, GPHY_EXP_SELECT_REG, 0x0F09);
	if (rc < 0)
		return rc;

	rc = phy_read(phy_dev, GPHY_EXP_DATA_REG);
	if (rc < 0)
		return rc;

	val = 0x5193; /* laneswap PHY 0 */

	if (val != (u16)rc) {
		/* Apply the laneswap setting */
		rc = phy_write(phy_dev, GPHY_EXP_SELECT_REG, 0x0F09);
		if (rc < 0)
			return rc;

		rc = phy_write(phy_dev, GPHY_EXP_DATA_REG, val);
		if (rc < 0)
			return rc;
	}

	return 0;
}

void amac_gphy_rgmii_init(struct bcm_amac_priv *privp, bool enable)
{
	u32 val;
	struct platform_device *pdev = privp->pdev;
	struct device_node *np = pdev->dev.of_node;
	struct net_device *ndev;
	void __iomem *rgmii_regs;

	if (!privp)
		return;

	rgmii_regs = privp->hw.reg.rgmii_regs;
	ndev = privp->ndev;

	if (enable) {
		/* SET RGMII IO CONFIG */
		/* Get register base address */
		val = readl(rgmii_regs + NICPM_PADRING_CFG);
		dev_dbg(&ndev->dev, "NICPM_PADRING_CFG:%u, default 0x%x\n",
			(NICPM_ROOT + NICPM_PADRING_CFG), val);
		writel(NICPM_PADRING_CFG_INIT_VAL,
		       rgmii_regs + NICPM_PADRING_CFG);
		dev_dbg(&ndev->dev, "NICPM_PADRING_CFG:%u, value 0x%x\n",
			(NICPM_ROOT + NICPM_PADRING_CFG),
			readl(rgmii_regs + NICPM_PADRING_CFG));
		/* Give some time so that values take effect */
		usleep_range(10, 100);

		/* SET IO MUX CONTROL */
		/* Get register base address */
		val = readl(rgmii_regs + NICPM_IOMUX_CTRL);
		dev_dbg(&ndev->dev, "NICPM_IOMUX_CTRL:%u, default 0x%x\n",
			(NICPM_ROOT + NICPM_IOMUX_CTRL), val);
		if (of_device_is_compatible(np, "brcm,amac-enet"))
			writel(NICPM_IOMUX_CTRL_INIT_VAL,
			       (rgmii_regs + NICPM_IOMUX_CTRL));
		else if (of_device_is_compatible(np, "brcm,amac-enet-v2"))
			writel(NICPM_IOMUX_CTRL_INIT_VAL_BX,
			       (rgmii_regs + NICPM_IOMUX_CTRL));
		else
			dev_err(&ndev->dev, "device tree node for unknown device\n");
		dev_dbg(&ndev->dev, "NICPM_IOMUX_CTRL:%u, value 0x%x\n",
			(NICPM_ROOT + NICPM_IOMUX_CTRL),
			readl(rgmii_regs + NICPM_IOMUX_CTRL));
		usleep_range(10, 100);
	}
}

int bcm_amac_gphy_init(struct bcm_amac_priv *privp)
{
	struct port_info *port = &privp->port.ext_port;
	int rc;

	/* No PHY handling in switch mode */
	if (privp->switch_mode)
		return 0;

	if (privp->hw.reg.rgmii_regs)
		amac_gphy_rgmii_init(privp, true);

	/* Register PHY Fix-ups related to lane-swap */
	if (privp->port.ext_port.lswap) {
		rc = phy_register_fixup_for_uid(PHY_ID_BCM_CYGNUS,
						PHY_BCM_OUI_MASK,
						amac_gphy_lswap);
		if (rc)
			return rc;
	}

	/* Connect and configure the PHY */
	port->phydev = of_phy_connect(privp->ndev,
				      port->phy_node,
				      amac_gphy_handle_link_change,
				      0,
				      (phy_interface_t)port->phy_mode);
	if (IS_ERR_OR_NULL(port->phydev)) {
		dev_err(&privp->pdev->dev, "Failed to connect phy\n");
		port->phydev = NULL;
		return -ENODEV;
	}

	if (privp->port.ext_port.pause_disable)
		/* Override the PAUSE frame setting of the PHY */
		port->phydev->advertising &= ~(ADVERTISED_Pause |
					       ADVERTISED_Asym_Pause);

	rc = phy_start_aneg(port->phydev);
	if (rc < 0) {
		phy_disconnect(port->phydev);
		port->phydev = NULL;
		dev_err(&privp->pdev->dev, "Cannot start PHY\n");
		return -ENODEV;
	}

	return 0;
}

void bcm_amac_gphy_exit(struct bcm_amac_priv *privp)
{
	/* No PHY handling in switch mode */
	if (privp->switch_mode)
		return;

	if (privp->port.ext_port.phydev) {
		phy_disconnect(privp->port.ext_port.phydev);
		privp->port.ext_port.phydev = NULL;
	}
}

int bcm_amac_gphy_powerup(struct bcm_amac_priv *privp, bool powerup)
{
	int rc = 0;

	/* No PHY handling in switch mode */
	if (privp->switch_mode)
		return 0;

	/* Power up the PHY(s) */
	rc = amac_gphy_enable(
		privp->port.ext_port.phydev,
		powerup);

	return rc;
}

void bcm_amac_gphy_start(struct bcm_amac_priv *privp, bool start)
{
	/* No PHY handling in switch mode */
	if (privp->switch_mode)
		return;

	if (start)
		phy_start(privp->port.ext_port.phydev);
	else
		phy_stop(privp->port.ext_port.phydev);
}

