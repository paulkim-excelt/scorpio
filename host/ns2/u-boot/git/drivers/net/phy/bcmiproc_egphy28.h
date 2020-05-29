/*
 * $Copyright Open Broadcom Corporation$
 *
 * These routines provide access to the external phy
 *
 */

/* ---- Include Files ---------------------------------------------------- */
#include <config.h>
#include <common.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/bcmutils.h>
#include "ethHw_data.h"

/* Indirect PHY register address flags */
#define SOC_PHY_REG_RESERVE_ACCESS    0x20000000
#define SOC_PHY_REG_1000X             0x40000000
#define SOC_PHY_REG_INDIRECT          0x80000000
#define _SOC_PHY_REG_DIRECT ((SOC_PHY_REG_1000X << 1) | \
			     (SOC_PHY_REG_1000X >> 1))

#define EGPHY28_BUSID	0x3
#define EGPHY28_PHYID	0x4

/* ---- External Function Prototypes ------------------------------------- */
void egphy28_ge_reset(bcm_eth_t *eth_data, uint phyaddr);

int egphy28_wr_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 *data);
int egphy28_rd_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		   uint16 reg_bank, uint8 reg_addr, uint16 *data);
int egphy28_mod_reg(bcm_eth_t *eth_data, uint phyaddr, uint32 flags,
		    uint16 reg_bank, uint8 reg_addr, uint16 data,
		    uint16 mask);
int egphy28_init(bcm_eth_t *eth_data, uint phyaddr);
int egphy28_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link);
void egphy28_set_internal_phy_loopback(bcm_eth_t *eth_data,
				       int ext, uint phyaddr,
				       bool enable);
void egphy28_set_external_phy_loopback(bcm_eth_t *eth_data,
				       int ext, uint phyaddr,
				       bool enable);
#ifdef BCMINTERNAL
int egphy28_enable_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
int egphy28_speed_set(bcm_eth_t *eth_data, uint phyaddr, int speed);
#endif /* BCMINTERNAL */
int egphy28_speed_get(bcm_eth_t *eth_data, uint phyaddr, int *speed,
		      int *duplex);
#ifdef BCMINTERNAL
int egphy28_lb_set(bcm_eth_t *eth_data, uint phyaddr, int enable);
void egphy28_disp_status(bcm_eth_t *eth_data, uint phyaddr);
#endif /* BCMINTERNAL */
