/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef _bcm_iproc_phy5481_h_
#define _bcm_iproc_phy5481_h_

#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/bcmutils.h>
#include "../ethHw_data.h"

/* Indirect PHY register address flags */
#define SOC_PHY_REG_RESERVE_ACCESS	0x20000000
#define SOC_PHY_REG_1000X		0x40000000
#define SOC_PHY_REG_INDIRECT		0x80000000
#define _SOC_PHY_REG_DIRECT		((SOC_PHY_REG_1000X << 1) |	\
						(SOC_PHY_REG_1000X >> 1))

void phy5481_ge_reset(bcm_eth_t *eth_data, uint phyaddr);

int phy5481_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 *data);
int phy5481_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 *data);
int phy5481_mod_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		    uint16 reg_bank, uint8 reg_addr, uint16 data,
		    uint16 mask);
int phy5481_init(bcm_eth_t *eth_data, uint phyaddr);
int phy5481_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link);
void phy54810_config_laneswap(bcm_eth_t *eth_data, int ext, uint phyaddr);
#ifdef BCMINTERNAL
int phy5481_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
int phy5481_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed);
#endif /* BCMINTERNAL */
int phy5481_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed,
		      int *duplex);
#ifdef BCMINTERNAL
int phy5481_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
void phy548_disp_status(bcm_eth_t *eth_data, uint phyaddr);
#endif /* BCMINTERNAL */

#endif	/* _bcm_iproc_phy5481_h_ */
