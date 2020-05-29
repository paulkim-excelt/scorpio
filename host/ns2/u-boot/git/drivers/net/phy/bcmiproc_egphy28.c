/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

/* ---- Include Files ---------------------------------------------------- */
#include "bcmiproc_phy.h"
#include "bcmiproc_egphy28.h"
#include <asm/arch-bcm_pegasus/bcm_mdio.h>
#include <asm/io.h>

/* ---- External Variable Declarations ----------------------------------- */
/* ---- External Function Prototypes ------------------------------------- */
/* ---- Public Variables ------------------------------------------------- */
/* ---- Private Constants and Types -------------------------------------- */
/* ---- Private Variables ------------------------------------------------ */

/* debug/trace */
#undef BCMDBG
#define BCMDBG_ERR
#ifdef BCMDBG
#define	NET_ERROR(args) printf args
#define	NET_TRACE(args) printf args
#elif defined(BCMDBG_ERR)
#define	NET_ERROR(args) printf args
#define NET_TRACE(args)
#else
#define	NET_ERROR(args)
#define	NET_TRACE(args)
#endif /* BCMDBG */
#define	NET_REG_TRACE(args)

#ifndef ASSERT
#define ASSERT(exp)
#endif

/* ==== Public Functions ================================================= */

int egphy28_wr_reg(bcm_eth_t *eth_data, uint phyaddr,
		   uint32 flags, uint16 reg_bank, uint8 reg_addr, uint16 *data)
{
	int rv = SOC_E_NONE;
	uint16 wr_data = *data;

	NET_TRACE(("%s enter\n", __func__));

	NET_REG_TRACE(("%s line%d, reg_bank(0x%x) reg_addr(0x%x) data(0x%x)\n",
		       __func__, __LINE__, reg_bank, reg_addr, wr_data));

	if (flags & SOC_PHY_REG_1000X) {
		if (reg_addr <= 0x000f) {
			uint16 blk_sel;

			/* Map 1000X page */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, 0x7c00);
			blk_sel =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, 0x1c);
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, blk_sel | 0x8001);

			/* write 1000X IEEE register */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, reg_addr, wr_data);

			/* Restore IEEE mapping */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c,
				       (blk_sel & 0xfffe) | 0x8000);
		} else if (flags & _SOC_PHY_REG_DIRECT) {
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, reg_addr, wr_data);
		} else {
			rv = SOC_E_PARAM;
		}
	} else {
		switch (reg_addr) {
			/* Map shadow registers */
#ifdef BCMINTERNAL
		case 0x15:
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x17, reg_bank);
			break;
#endif /* BCMINTERNAL */
		case 0x18:
			if (reg_bank <= 0x0007) {
				if (reg_bank == 0x0007)
					wr_data |= 0x8000;
				wr_data = (wr_data & ~(0x0007)) | reg_bank;
			} else {
				rv = SOC_E_PARAM;
			}
			break;
		case 0x1C:
			if (reg_bank <= 0x001F)
				wr_data =
				    0x8000 | (reg_bank << 10) | (wr_data &
								 0x03FF);
			else
				rv = SOC_E_PARAM;
			break;
#ifdef BCMINTERNAL
		case 0x1D:
			if (reg_bank == 0x0000)
				wr_data = wr_data & 0x07FFF;
			else
				rv = SOC_E_PARAM;
			break;
#endif /* BCMINTERNAL */
		default:
			if (!(flags & SOC_PHY_REG_RESERVE_ACCESS))
				/* Must not write to reserved registers */
				if (reg_addr > 0x001e)
					rv = SOC_E_PARAM;
			break;
		}
		if (SOC_SUCCESS(rv))
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, reg_addr, wr_data);
	}
	if (SOC_FAILURE(rv))
		NET_ERROR(("%s line%d, reg_bank(0x%x) reg_addr(0x%x) rv(%d)\n",
			   __func__, __LINE__, reg_bank, reg_addr, rv));
	return rv;
}

int egphy28_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 *data)
{
	int rv = SOC_E_NONE;

	NET_TRACE(("%s enter\n", __func__));

	NET_REG_TRACE(("%s line%d reg_bank(0x%x) reg_addr(0x%x)\n",
		       __func__, __LINE__, reg_bank, reg_addr));
	if (flags & SOC_PHY_REG_1000X) {
		if (reg_addr <= 0x000f) {
			uint16 blk_sel;

			/* Map 1000X page */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, 0x7c00);
			blk_sel =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, 0x1c);
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, blk_sel | 0x8001);

			/* Read 1000X IEEE register */
			*data =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, reg_addr);
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n",
				       __func__, __LINE__, reg_addr, *data));

			/* Restore IEEE mapping */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c,
				       (blk_sel & 0xfffe) | 0x8000);
		} else {
			rv = SOC_E_PARAM;
		}
	} else {
		switch (reg_addr) {
			/* Map shadow registers */
#ifdef BCMINTERNAL
		case 0x15:
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x17, reg_bank);
			break;
#endif /* BCMINTERNAL */
		case 0x18:
			if (reg_bank <= 0x0007)
				bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
					       phyaddr, reg_addr,
					       (reg_bank << 12) | 0x7);
			else
				rv = SOC_E_PARAM;
			break;
		case 0x1C:
			if (reg_bank <= 0x001F)
				bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
					       phyaddr, reg_addr,
					       (reg_bank << 10));
			else
				rv = SOC_E_PARAM;
			break;
#ifdef BCMINTERNAL
		case 0x1D:
			if (reg_bank <= 0x0001)
				bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
					       phyaddr, reg_addr,
					       (reg_bank << 15));
			else
				rv = SOC_E_PARAM;
			break;
#endif /* BCMINTERNAL */
		default:
			if (!(flags & SOC_PHY_REG_RESERVE_ACCESS))
				/* Must not read from reserved registers */
				if (reg_addr > 0x001e)
					rv = SOC_E_PARAM;
			break;
		}
		if (SOC_SUCCESS(rv)) {
			*data =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, reg_addr);
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n"
				      __func__, __LINE__, reg_addr, *data));
		}
	}
	if (SOC_FAILURE(rv))
		NET_ERROR(("%s line%d reg_bank(0x%x) reg_addr(0x%x) rv(%d)\n",
			   __func__, __LINE__, reg_bank, reg_addr, rv));
	return rv;
}

int egphy28_mod_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 data, uint16 mask)
{
	int rv = SOC_E_NONE;
	uint16 o_data, rd_data;

	NET_TRACE(("%s enter\n", __func__));

	NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x) mask(0x%x)\n",
		       __func__, __LINE__, reg_addr, data, mask));

	if (flags & SOC_PHY_REG_1000X) {
		if (reg_addr <= 0x000f) {
			uint16 blk_sel;

			/* Map 1000X page */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, 0x7c00);
			blk_sel =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, 0x1c);
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c, blk_sel | 0x8001);

			/* Modify 1000X IEEE register */
			o_data =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, reg_addr);
			rd_data = o_data;
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n",
				       __func__, __LINE__, reg_addr, o_data));
			rd_data = o_data & (~(mask));
			rd_data |= data;
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, reg_addr, rd_data);
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n",
				       __func__, __LINE__, reg_addr, rd_data));

			/* Restore IEEE mapping */
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x1c,
				       (blk_sel & 0xfffe) | 0x8000);
		} else {
			rv = SOC_E_PARAM;
		}
	} else {
		switch (reg_addr) {
			/* Map shadow registers */
#ifdef BCMINTERNAL
		case 0x15:
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, 0x17, reg_bank);
			break;
#endif /* BCMINTERNAL */
		case 0x18:
			if (reg_bank <= 0x0007) {
				bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
					       phyaddr, reg_addr,
					       (reg_bank << 12) | 0x7);

				if (reg_bank == 0x0007) {
					data |= 0x8000;
					mask |= 0x8000;
				}
				mask &= ~(0x0007);
			} else {
				rv = SOC_E_PARAM;
			}
			break;
		case 0x1C:
			if (reg_bank <= 0x001F) {
				bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
					       phyaddr, reg_addr,
					       (reg_bank << 10));
				data |= 0x8000;
				mask |= 0x8000;
				mask &= ~(0x1F << 10);
			} else {
				rv = SOC_E_PARAM;
			}
			break;
#ifdef BCMINTERNAL
		case 0x1D:
			if (reg_bank == 0x0000)
				mask &= 0x07FFF;
			else
				rv = SOC_E_PARAM;
			break;
#endif /* BCMINTERNAL */
		default:
			if (!(flags & SOC_PHY_REG_RESERVE_ACCESS))
				/* Must not write to reserved registers */
				if (reg_addr > 0x001e)
					rv = SOC_E_PARAM;
			break;
		}
		if (SOC_SUCCESS(rv)) {
			o_data =
			    bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
					  phyaddr, reg_addr);
			rd_data = o_data;
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n",
				       __func__, __LINE__, reg_addr, o_data));
			rd_data &= ~(mask);
			rd_data |= data;
			bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID,
				       phyaddr, reg_addr, rd_data);
			NET_REG_TRACE(("%s line%d reg_addr(0x%x) data(0x%x)\n",
				       __func__, __LINE__, reg_addr, rd_data));
		}
	}

	if (SOC_FAILURE(rv))
		NET_ERROR(("%s line%d reg_addr(0x%x) rv(%d)\n",
			   __func__, __LINE__, reg_addr, rv));
	return rv;
}

void egphy28_ge_reset(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16 ctrl;
	unsigned long init_time;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __func__,
		   phyaddr));

	/* set reset flag */
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &ctrl);
	ctrl |= MII_CTRL_RESET;
	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &ctrl);
	init_time = get_timer(0);
	for (;;) {
		udelay(100);

		/* check if out of reset */
		if (!
		    (egphy28_rd_reg
		     (eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		      PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR,
		      &ctrl) & MII_CTRL_RESET)) {
			NET_TRACE(("et%d: %s reset complete\n", eth_data->unit,
				   __func__));
			return;
		}

		if (get_timer(init_time) > 10) {
			/* timeout */
			NET_ERROR(("et%d: %s reset not complete\n",
				   eth_data->unit, __func__));
			return;
		}
	}
}

/*
 * Function:
 *  egphy28_ge_interface_set
 * Purpose:
 *  Set the current operating mode of the PHY.
 *  (Pertaining to the MAC/PHY interface, not the line interface).
 *      For example: TBI or MII/GMII.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  pif - one of SOC_PORT_IF_*
 * Returns:
 *  SOC_E_XXX
 */
int egphy28_ge_interface_set(bcm_eth_t *eth_data, uint phyaddr,
			     soc_port_if_t pif)
{
	uint16 mii_ecr;
	int mii;		/* MII if true, TBI otherwise */

	switch (pif) {
	case SOC_PORT_IF_MII:
	case SOC_PORT_IF_GMII:
	case SOC_PORT_IF_RGMII:
		mii = TRUE;
		break;
	case SOC_PORT_IF_NOCXN:
		return SOC_E_NONE;
	case SOC_PORT_IF_TBI:
	case SOC_PORT_IF_SGMII:
		mii = FALSE;
		break;
	default:
		return SOC_E_UNAVAIL;
	}

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ECRR_FLAGS, PHY_MII_ECRR_BANK,
		       PHY_MII_ECRR_ADDR, &mii_ecr);

	if (mii)
		mii_ecr &= ~MII_ECR_10B;
	else
		mii_ecr |= MII_ECR_10B;

	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_ECRR_FLAGS, PHY_MII_ECRR_BANK,
		       PHY_MII_ECRR_ADDR, &mii_ecr);

	return SOC_E_NONE;
}

/*
 * Function:
 *  egphy28_ge_init
 * Purpose:
 *  Initialize the PHY (MII mode) to a known good state.
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 * Returns:
 *  SOC_E_XXX

 * Notes:
 *  No synchronization performed at this level.
 */
int egphy28_ge_init(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16 mii_ctrl, mii_gb_ctrl;
	uint16 mii_ana;

	/* Reset PHY */
	egphy28_ge_reset(eth_data, phyaddr);

	/* set advertized bits */
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANAR_FLAGS, PHY_MII_ANAR_BANK,
		       PHY_MII_ANAR_ADDR, &mii_ana);
	mii_ana |= MII_ANA_FD_100 | MII_ANA_FD_10;
	mii_ana |= MII_ANA_HD_100 | MII_ANA_HD_10;
	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_ANAR_FLAGS, PHY_MII_ANAR_BANK,
		       PHY_MII_ANAR_ADDR, &mii_ana);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANAR_FLAGS, PHY_MII_ANAR_BANK,
		       PHY_MII_ANAR_ADDR, &mii_ana);
	mii_ctrl = MII_CTRL_FD | MII_CTRL_SS_1000 | MII_CTRL_AE | MII_CTRL_RAN;
	mii_gb_ctrl = MII_GB_CTRL_ADV_1000FD | MII_GB_CTRL_PT;

	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_GB_CTRLR_FLAGS,
		       PHY_MII_GB_CTRLR_BANK, PHY_MII_GB_CTRLR_ADDR,
		       &mii_gb_ctrl);
	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);

	return SOC_E_NONE;
}

#ifdef BCMINTERNAL
/*
 * Function:
 *  egphy28_ge_speed_set
 * Purpose:
 *  Set the current operating speed (forced).
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  duplex - (OUT) Boolean, true indicates full duplex, false
 *      indicates half.
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  No synchronization performed at this level. Autonegotiation is
 *  not manipulated.
 */
int egphy28_ge_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed)
{
	uint16 mii_ctrl;

	if (speed == 0)
		return SOC_E_NONE;

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);

	mii_ctrl &= ~(MII_CTRL_SS_LSB | MII_CTRL_SS_MSB);

	switch (speed) {
	case 10:
		mii_ctrl |= MII_CTRL_SS_10;
		break;
	case 100:
		mii_ctrl |= MII_CTRL_SS_100;
		break;
	case 1000:
		mii_ctrl |= MII_CTRL_SS_1000;
		break;
	default:
		return SOC_E_CONFIG;
	}

	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);

	return SOC_E_NONE;
}
#endif /* BCMINTERNAL */

void egphy28_reset_setup(bcm_eth_t *eth_data, uint phyaddr)
{
	NET_TRACE(("%s enter\n", __func__));

	egphy28_ge_init(eth_data, phyaddr);

	/* remove power down */
	egphy28_mod_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
			PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, 0, MII_CTRL_PD);
	/* Disable super-isolate */
	egphy28_mod_reg(eth_data, phyaddr, PHY_MII_POWER_CTRLR_FLAGS,
			PHY_MII_POWER_CTRLR_BANK, PHY_MII_POWER_CTRLR_ADDR, 0,
			1U << 5);
	/* Enable extended packet length */
	egphy28_mod_reg(eth_data, phyaddr, PHY_MII_AUX_CTRLR_FLAGS,
			PHY_MII_AUX_CTRLR_BANK, PHY_MII_AUX_CTRLR_ADDR, 0x4000,
			0x4000);

	return;
}

/*
 * Function:
 *	egphy28_init
 * Purpose:
 *	Initialize EGphy28
 * Parameters:
 *	eth_data - ethernet data
 *	phyaddr - physical address
 * Returns:
 *	0
 */
int egphy28_init(bcm_eth_t *eth_data, uint phyaddr)
{
	bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID, phyaddr, 0x17,
		       0x6032);
	bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID, phyaddr, 0x15, 0x0A0);

	return 0;
}

/*
 * Function:
 *  egphy28_link_get
 * Purpose:
 *  Determine the current link up/down status
 * Parameters:
 *  unit - StrataSwitch unit #.
 *  port - StrataSwitch port #.
 *  link - (OUT) Boolean, true indicates link established.
 * Returns:
 *  SOC_E_XXX
 * Notes:
 *  No synchronization performed at this level.
 */
int egphy28_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link)
{
	int rc;
	uint16 mii_ctrl, mii_stat;
	unsigned long init_time;

	*link = 0;		/* Default */

	rc = egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
			    PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR, &mii_stat);
	if (rc)
		printf("at %d egphy28_rd_reg failed:%d\n", __LINE__, rc);
	/* the first read of status register will not show link up,
	 * second read will show link up
	 */
	if (!(mii_stat & MII_STAT_LA)) {
		rc = egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
				    PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR,
				    &mii_stat);
		if (rc)
			printf("at %d egphy28_rd_reg failed:%d\n", __LINE__,
			       rc);
	}

	if ((!(mii_stat & MII_STAT_LA)) || (mii_stat == 0xffff)) {
		/* mii_stat == 0xffff check is to handle removable PHY
		 * daughter cards
		 */
		printf
		    ("at  bcmiproc_egphy28.c:%d return mii_stat:%d| link:%d\n",
		     __LINE__, mii_stat, *link);
		return SOC_E_NONE;
	}

	/* Link appears to be up; we are done if autoneg is off. */
	rc = egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
			    PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);
	if (rc)
		printf("at %d egphy28_rd_reg failed:%d\n", __LINE__, rc);

	if (!(mii_ctrl & MII_CTRL_AE)) {
		*link = 1;
		printf("at %d return link:%d\n", __LINE__, *link);
		return SOC_E_NONE;
	}

	/*
	 * If link appears to be up but autonegotiation is still in
	 * progress, wait for it to complete.  For BCM5228, autoneg can
	 * still be busy up to about 200 usec after link is indicated.  Also
	 * continue to check link state in case it goes back down.
	 */
	init_time = get_timer(0);
	for (;;) {
		rc = egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
				    PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR,
				    &mii_stat);
		if (rc)
			printf("at %d egphy28_rd_reg failed:%d\n", __LINE__,
			       rc);
		if (!(mii_stat & MII_STAT_LA)) {
			/* link is down */
			printf("at %d return link:%d\n", __LINE__, *link);
			return SOC_E_NONE;
		}

		if (mii_stat & MII_STAT_AN_DONE) {
			/* AutoNegotiation done */
			break;
		}

		if (get_timer(init_time) > 1) {
			/* timeout */
			printf("at %d return SOC_E_BUSY link:%d\n", __LINE__,
			       *link);
			return SOC_E_BUSY;
		}
	}
	/* Return link state at end of polling */
	*link = ((mii_stat & MII_STAT_LA) != 0);

	printf("at %d return link:%d\n", __LINE__, *link);
	return SOC_E_NONE;
}

#ifdef BCMINTERNAL
/*
 * Function:
 *      egphy28_enable_set
 * Purpose:
 *      Enable/Disable phy
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      enable - on/off state to set
 * Returns:
 *      0
 */
int egphy28_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16 power_down;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __func__,
		   phyaddr));

	power_down = (enable) ? 0 : MII_CTRL_PD;
	egphy28_mod_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
			PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, power_down,
			MII_CTRL_PD);

	return SOC_E_NONE;
}

/*
 * Function:
 *      egphy28_speed_set
 * Purpose:
 *      Set PHY speed
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      speed - link speed in Mbps
 * Returns:
 *      0
 */
int egphy28_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed)
{
	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __func__,
		   phyaddr));

	egphy28_ge_speed_set(eth_data, phyaddr, speed);

	return 0;
}
#endif /* BCMINTERNAL */

/*
 * Function:
 *    egphy28_auto_negotiate_gcd (greatest common denominator).
 * Purpose:
 *    Determine the current greatest common denominator between
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:
 *    SOC_E_XXX
 * Notes:
 *    No synchronization performed at this level.
 */
static int egphy28_auto_negotiate_gcd(bcm_eth_t *eth_data, uint phyaddr,
				      int *speed, int *duplex)
{
	int t_speed, t_duplex;
	uint16 mii_ana, mii_anp, mii_stat;
	uint16 mii_gb_stat, mii_esr, mii_gb_ctrl;

	mii_gb_stat = 0;	/* Start off 0 */
	mii_gb_ctrl = 0;	/* Start off 0 */

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANAR_FLAGS, PHY_MII_ANAR_BANK,
		       PHY_MII_ANAR_ADDR, &mii_ana);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANPR_FLAGS, PHY_MII_ANPR_BANK,
		       PHY_MII_ANPR_ADDR, &mii_anp);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
		       PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR, &mii_stat);

	if (mii_stat & MII_STAT_ES) {	/* Supports extended status */
		/*
		 * If the PHY supports extended status, check if it is 1000MB
		 * capable.  If it is, check the 1000Base status register to see
		 * if 1000MB negotiated.
		 */
		egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ESRR_FLAGS,
			       PHY_MII_ESRR_BANK, PHY_MII_ESRR_ADDR, &mii_esr);

		if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD |
			       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
			egphy28_rd_reg(eth_data, phyaddr,
				       PHY_MII_GB_STATR_FLAGS,
				       PHY_MII_GB_STATR_BANK,
				       PHY_MII_GB_STATR_ADDR, &mii_gb_stat);
			egphy28_rd_reg(eth_data, phyaddr,
				       PHY_MII_GB_CTRLR_FLAGS,
				       PHY_MII_GB_CTRLR_BANK,
				       PHY_MII_GB_CTRLR_ADDR, &mii_gb_ctrl);
		}
	}

	/*
	 * At this point, if we did not see Gig status, one of mii_gb_stat or
	 * mii_gb_ctrl will be 0. This will cause the first 2 cases below to
	 * fail and fall into the default 10/100 cases.
	 */

	mii_ana &= mii_anp;

	if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000FD) &&
	    (mii_gb_stat & MII_GB_STAT_LP_1000FD)) {
		t_speed = 1000;
		t_duplex = 1;
	} else if ((mii_gb_ctrl & MII_GB_CTRL_ADV_1000HD) &&
		   (mii_gb_stat & MII_GB_STAT_LP_1000HD)) {
		t_speed = 1000;
		t_duplex = 0;
	} else if (mii_ana & MII_ANA_FD_100) {	/* [a] */
		t_speed = 100;
		t_duplex = 1;
	} else if (mii_ana & MII_ANA_T4) {	/* [b] */
		t_speed = 100;
		t_duplex = 0;
	} else if (mii_ana & MII_ANA_HD_100) {	/* [c] */
		t_speed = 100;
		t_duplex = 0;
	} else if (mii_ana & MII_ANA_FD_10) {	/* [d] */
		t_speed = 10;
		t_duplex = 1;
	} else if (mii_ana & MII_ANA_HD_10) {	/* [e] */
		t_speed = 10;
		t_duplex = 0;
	} else {
		return SOC_E_FAIL;
	}

	if (speed)
		*speed = t_speed;
	if (duplex)
		*duplex = t_duplex;

	return SOC_E_NONE;
}

/*
 * Function:
 *    egphy28_auto_negotiate_ew (Autoneg-ed mode with E@W on).
 * Purpose:
 *    Determine autoneg-ed mode between
 *    two ends of a link
 * Parameters:
 *    unit - StrataSwitch unit #.
 *    port - StrataSwitch port #.
 *    speed - (OUT) greatest common speed.
 *    duplex - (OUT) greatest common duplex.
 *    link - (OUT) Boolean, true indicates link established.
 * Returns:
 *    SOC_E_XXX
 * Notes:
 *    No synchronization performed at this level.
 */
static int egphy28_auto_negotiate_ew(bcm_eth_t *eth_data, uint phyaddr,
				     int *speed, int *duplex)
{
	int t_speed, t_duplex;
	uint16 mii_assr;

	egphy28_rd_reg(eth_data, phyaddr, 0x00, 0x0000, 0x19, &mii_assr);

	switch ((mii_assr >> 8) & 0x7) {
	case 0x7:
		t_speed = 1000;
		t_duplex = TRUE;
		break;
	case 0x6:
		t_speed = 1000;
		t_duplex = FALSE;
		break;
	case 0x5:
		t_speed = 100;
		t_duplex = TRUE;
		break;
	case 0x3:
		t_speed = 100;
		t_duplex = FALSE;
		break;
	case 0x2:
		t_speed = 10;
		t_duplex = TRUE;
		break;
	case 0x1:
		t_speed = 10;
		t_duplex = FALSE;
		break;
	default:
		t_speed = 0;	/* 0x4 is 100BASE-T4 which is not supported */
		t_duplex = FALSE;
		break;
	}

	if (speed)
		*speed = t_speed;
	if (duplex)
		*duplex = t_duplex;

	return SOC_E_NONE;
}

/*
 * Function:
 *      egphy28_speed_get
 * Purpose:
 *      Get PHY speed
 * Parameters:
 *      eth_data - ethernet data
 *      phyaddr - physical address
 *      speed - current link speed in Mbps
 * Returns:
 *      0
 */
int egphy28_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed,
		      int *duplex)
{
	int rv;
	uint16 mii_ctrl, mii_stat, misc_ctrl;

	NET_TRACE(("et%d: %s: phyaddr %d\n", eth_data->unit, __func__,
		   phyaddr));

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
		       PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR, &mii_stat);

	*speed = 0;
	*duplex = 0;
	if (mii_ctrl & MII_CTRL_AE) {	/* Auto-negotiation enabled */
		 /* Auto-neg NOT complete */
		if (!(mii_stat & MII_STAT_AN_DONE)) {
			rv = SOC_E_NONE;
		} else {
			/* First check for Ethernet@Wirespeed */
			egphy28_rd_reg(eth_data, phyaddr, 0x0, 0x0007, 0x18,
				       &misc_ctrl);
			/* Ethernet@Wirespeed enabled */
			if (misc_ctrl & (1U << 4)) {
				rv = egphy28_auto_negotiate_ew(eth_data,
							       phyaddr, speed,
							       duplex);
			} else {
				rv = egphy28_auto_negotiate_gcd(eth_data,
								phyaddr, speed,
								duplex);
			}
		}
	} else {		/* Auto-negotiation disabled */
		/* Simply pick up the values we force in CTRL register */
		if (mii_ctrl & MII_CTRL_FD)
			*duplex = 1;

		switch (MII_CTRL_SS(mii_ctrl)) {
		case MII_CTRL_SS_10:
			*speed = 10;
			break;
		case MII_CTRL_SS_100:
			*speed = 100;
			break;
		case MII_CTRL_SS_1000:
			*speed = 1000;
			break;
		default:	/* Just pass error back */
			return SOC_E_UNAVAIL;
		}
		rv = SOC_E_NONE;
	}

	return rv;
}

#ifdef BCMINTERNAL
int egphy28_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable)
{
	uint16 mii_ctrl;

	/* set reset flag */
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);
	mii_ctrl &= ~MII_CTRL_LE;
	mii_ctrl |= enable ? MII_CTRL_LE : 0;
	egphy28_wr_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &mii_ctrl);

	return 0;
}

void egphy28_disp_status(bcm_eth_t *eth_data, uint phyaddr)
{
	uint16 tmp0, tmp1, tmp2;
	int speed, duplex;

	printf("et%d: %s: phyaddr:%d\n", eth_data->unit, __func__, phyaddr);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_CTRLR_FLAGS,
		       PHY_MII_CTRLR_BANK, PHY_MII_CTRLR_ADDR, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_STATR_FLAGS,
		       PHY_MII_STATR_BANK, PHY_MII_STATR_ADDR, &tmp1);
	printf("  MII-Control: 0x%x; MII-Status: 0x%x\n", tmp0, tmp1);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID0R_FLAGS,
		       PHY_MII_PHY_ID0R_BANK, PHY_MII_PHY_ID0R_ADDR, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_PHY_ID1R_FLAGS,
		       PHY_MII_PHY_ID1R_BANK, PHY_MII_PHY_ID1R_ADDR, &tmp1);
	printf("  Phy ChipID: 0x%04x:0x%04x\n", tmp0, tmp1);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANAR_FLAGS, PHY_MII_ANAR_BANK,
		       PHY_MII_ANAR_ADDR, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ANPR_FLAGS, PHY_MII_ANPR_BANK,
		       PHY_MII_ANPR_ADDR, &tmp1);
	egphy28_speed_get(eth_data, phyaddr, &speed, &duplex);
	printf
	    ("  AutoNeg Ad: 0x%x; AutoNeg Partner: 0x%x; speed:%d; duplex:%d\n",
	     tmp0, tmp1, speed, duplex);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_GB_CTRLR_FLAGS,
		       PHY_MII_GB_CTRLR_BANK, PHY_MII_GB_CTRLR_ADDR, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_GB_STATR_FLAGS,
		       PHY_MII_GB_STATR_BANK, PHY_MII_GB_STATR_ADDR, &tmp1);
	printf("  MII GB ctrl: 0x%x; MII GB stat: 0x%x\n", tmp0, tmp1);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ESRR_FLAGS, PHY_MII_ESRR_BANK,
		       PHY_MII_ESRR_ADDR, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_MII_ECRR_FLAGS, PHY_MII_ECRR_BANK,
		       PHY_MII_ECRR_ADDR, &tmp1);
	egphy28_rd_reg(eth_data, phyaddr, 0x00, 0x0000, 0x11, &tmp2);
	printf
	    ("  IEEE Ext stat: 0x%x; PHY Ext ctrl: 0x%x; PHY Ext stat: 0x%x\n",
	     tmp0, tmp1, tmp2);

	egphy28_rd_reg(eth_data, phyaddr, PHY_MODE_CTRLR_FLAGS,
		       PHY_MODE_CTRLR_BANK, PHY_MODE_CTRLR_ADDR, &tmp0);
	printf("  Mode Control (Addr 1c shadow 1f): 0x%x\n", tmp0);

	egphy28_rd_reg(eth_data, phyaddr, PHY_1000X_MII_CTRLR_FLAGS,
		       PHY_1000X_MII_CTRLR_BANK, PHY_1000X_MII_CTRLR_ADDR,
		       &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_1000X_MII_CTRLR_FLAGS,
		       PHY_1000X_MII_CTRLR_BANK, 0x01, &tmp1);
	printf("  1000-x MII ctrl: 0x%x; 1000-x MII stat: 0x%x\n", tmp0, tmp1);

	egphy28_rd_reg(eth_data, phyaddr, PHY_1000X_MII_CTRLR_FLAGS,
		       PHY_1000X_MII_CTRLR_BANK, 0x04, &tmp0);
	egphy28_rd_reg(eth_data, phyaddr, PHY_1000X_MII_CTRLR_FLAGS,
		       PHY_1000X_MII_CTRLR_BANK, 0x05, &tmp1);
	printf("  1000-x AutoNeg Ad: 0x%x; 1000-x AutoNeg Partner: 0x%x\n",
	       tmp0, tmp1);
}
#endif /* BCMINTERNAL */
