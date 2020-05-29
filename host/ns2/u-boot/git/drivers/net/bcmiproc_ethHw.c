/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <config.h>
#include <common.h>
#include <asm/io.h>
#include <net.h>
#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>
#include <asm/arch/bcmgmacmib.h>
#include <asm/arch/bcmutils.h>
#include "ethHw_data.h"
#include "bcmiproc_phy.h"
#ifdef CONFIG_PHY_EGPHY28
#include <asm/arch-bcm_pegasus/bcm_mdio.h>
#include "phy/bcmiproc_egphy28.h"
#endif
#ifdef CONFIG_PHY_BCM5481
#include "phy/bcm5481.h"
#endif
#ifdef CONFIG_NS2
#include <asm/arch-iproc/sys_proto.h>
#endif

#define ChipID_Reg ICFG_CHIP_REVISION_ID

#define BCMDBG 0
#define BCMDBG_ERR

#if BCMDBG
#define DBG_PRN printf("[AT:%s:%d]\n", __FILE__, __LINE__)
#define ET_ERROR(args) DBG_PRN;printf args
#define ET_TRACE(args) DBG_PRN;printf args
#define BCMIPROC_ETH_DEBUG  (1)
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#endif /* BCMDBG */

bcm_eth_t g_eth_data;
u32 reg_debug;
u32 rxDescBuf;
u32 rxDescAlignedBuf;
u32 rxDataBuf;
u32 txDescBuf;
u32 txDescAlignedBuf;
u32 txDataBuf;

#ifndef ASSERT
#define ASSERT(exp)
#endif

/* protypes */
int ethHw_chipAttach(bcm_eth_t *eth_data);
void ethHw_chipDetach(bcm_eth_t *eth_data);
int ethHw_dmaInit(bcm_eth_t *eth_data);
int ethHw_dmaRxInit(bcm_eth_t *eth_data);
int ethHw_dmaTxInit(bcm_eth_t *eth_data);
int ethHw_dmaAttach(bcm_eth_t *eth_data);
int ethHw_dmaDetach(dma_info_t *di);
int ethHw_portLinkUp(void);
void ethHw_checkPortSpeed(void);

#ifdef BCMIPROC_ETH_DEBUG
static void txDump(u8 *buf, int len);
static void dmaTxDump(bcm_eth_t *eth_data);
static void dmaRxDump(bcm_eth_t *eth_data);
static void gmacRegDump(bcm_eth_t *eth_data);
static void gmac_mibTxDump(bcm_eth_t *eth_data);
static void gmac_mibRxDump(bcm_eth_t *eth_data);
#endif
static uint dma_ctrlflags(dma_info_t *di, uint mask, uint flags);
static int dma_rxenable(dma_info_t *di);
static void dma_txinit(dma_info_t *di);
static void dma_rxinit(dma_info_t *di);
static bool dma_txreset(dma_info_t *di);
static bool dma_rxreset(dma_info_t *di);
static int dma_txload(int index, size_t len, u8 *tx_buf);
static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len,
			    u32 *stat0, u32 *stat1);
static void dma_rxrefill(dma_info_t *di, int index);
static void gmac_init_reset(bcm_eth_t *eth_data);
static void gmac_clear_reset(bcm_eth_t *eth_data);
void gmac_loopback(bcm_eth_t *eth_data, bool enable);
static void gmac_reset(bcm_eth_t *eth_data);
static void gmac_clearmib(bcm_eth_t *eth_data);
static int gmac_speed(bcm_eth_t *eth_data, u32 speed);
static void gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on);
static void gmac_promisc(bcm_eth_t *eth_data, bool mode);
static void gmac_enable(bcm_eth_t *eth_data, bool en);
static void gmac_core_reset(bcm_eth_t *eth_data);
#if !EMULATION_TEST
static void cmicd_miim_initialize(void);
#endif
void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg,
		 u16 v);
u16 chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg);
int chip_phy_auto_negotiate_gcd(bcm_eth_t *eth_data, uint phyaddr, int *speed,
				int *duplex);
int chip_phy_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link);
static void chip_reset(bcm_eth_t *eth_data);
static void chip_init(bcm_eth_t *eth_data, uint options);
static u32 chip_getintr_events(bcm_eth_t *eth_data, bool in_isr);

static u32 ethHw_readl(u32 addr);
static void ethHw_writel(u32 val, u32 addr);

#ifndef CONFIG_PEGASUS
void gmac_set_amac_rgmii(int en)
{
	u32 tmp;

	if (en) {
		/* SET RGMII IO CONFIG */
		/* Get register base address */
		tmp = readl(NICPM_ROOT + NICPM_PADRING_CFG);
		ET_TRACE(("NICPM_PADRING_CFG:%u, default 0x%x\n",
			  (NICPM_ROOT + NICPM_PADRING_CFG), tmp));
#if 0
		/* reset RGMII_AMP_EN & RGMII_PDN */
		tmp &= ~(1 << NICPM_PADRING_CFG__RGMII_AMP_EN);
		tmp &= ~(1 << NICPM_PADRING_CFG__RGMII_PDN);

		/* Set RGMII_PUP & RGMII_SEL = 3'b001 */
		tmp |= (1 << NICPM_PADRING_CFG__RGMII_PUP);
		tmp |= (1 << NICPM_PADRING_CFG__RGMII_SEL_R);
		writel(tmp, NICPM_ROOT + NICPM_PADRING_CFG);
#endif
		writel(0x74000000, NICPM_ROOT + NICPM_PADRING_CFG);
		ET_TRACE(("NICPM_PADRING_CFG:%u, value 0x%x\n",
			  (NICPM_ROOT + NICPM_PADRING_CFG),
		       readl(NICPM_ROOT + NICPM_PADRING_CFG)));
		mdelay(100);
		/* SET IO MUX CONTROL */
		/* Get register base address */
		tmp = readl(NICPM_ROOT + NICPM_IOMUX_CTRL);
		/* IOMUX CTRL should be written in board file */
		ET_TRACE(("NICPM_IOMUX_CTRL:%u, default 0x%x\n",
			  (NICPM_ROOT + NICPM_IOMUX_CTRL), tmp));
#if 0
		tmp |= (2 << NICPM_IOMUX_CTRL__IOMUX_CTL_G_R);
		/* Reset NICPM_IOMUX_CTRL__IOMUX_DEL_G_R[27:20] */
		tmp &= ~(0xFF << NICPM_IOMUX_CTRL__IOMUX_DEL_G_R);
		/* reset iddq */
		tmp &= ~(1 << NICPM_IOMUX_CTRL__IOMUX_IDDQ);

		/* set bypass_dll_2ns_del */
		tmp |= (1 << NICPM_IOMUX_CTRL__IOMUX_BYPASS_DEL);
		writel(0x80080000, (NICPM_ROOT + NICPM_IOMUX_CTRL));
#endif
		mdelay(100);
	}
}
#endif

#ifdef CONFIG_PEGASUS
static void gmac_set_ctf_pass_thru_mode(bcm_eth_t *eth_data)
{
	u32 ctf_port_val = 0x10000;

	writel(ctf_port_val, CTF_PASS_TRU_PORT0_OFFSET);
	writel(ctf_port_val, CTF_PASS_TRU_PORT1_OFFSET);
	writel(ctf_port_val, CTF_PASS_TRU_PORT2_OFFSET);
	writel(ctf_port_val, CTF_PASS_TRU_PORT3_OFFSET);

	/* writel(0x0400000F, CTF_CONTROL_OFFSET);*/
}

void gmac_setup_unimac_port(bcm_eth_t *eth_data)
{
	u32 tmp;

	tmp = readl(CTF_PORTS_CONFIG_OFFSET);
	tmp |= CTF_PORT3_UNIMAC_ENABLE;
	writel(tmp, CTF_PORTS_CONFIG_OFFSET);
}
#endif

int iproc_get_chipid(void)
{
	u32 val;

	val = reg32_read((unsigned int *)ChipID_Reg);
	return (val & 0xFFFF);
}

int iproc_get_chiprev(void)
{
	u32 val;

	val = reg32_read((unsigned int *)ChipID_Reg);
	return ((val >> 16) & 0xF);
}

int iproc_get_chipsku(void)
{
	u32 val;

	val = reg32_read((unsigned int *)ChipID_Reg);
	return ((val >> 20) & 0xF);
}

/* ==== Public Functions ================================================= */

/*****************************************************************************
* See ethHw.h for API documentation
*****************************************************************************/

int ethHw_Init(void)
{
	bcm_eth_t *eth_data = &g_eth_data;
	int stat;

	ET_TRACE(("%s enter\n", __func__));

	/* clear out g_eth_data */
	memset(&g_eth_data, 0, sizeof(bcm_eth_t));

	/* load mac number  */
	eth_data->mac = CONFIG_GMAC_NUM;

	printf("|eth_data->mac:%d\n", eth_data->mac);
	/* load mac address */
	switch (eth_data->mac) {
#ifdef IPROC_GMAC0_REG_BASE
	case (ETHHW_MAC_0):
		eth_data->regs = (gmacregs_t *)IPROC_GMAC0_REG_BASE;
		break;
#endif
#ifdef IPROC_GMAC3_REG_BASE
	case (ETHHW_MAC_3):
		eth_data->regs = (gmacregs_t *)IPROC_GMAC3_REG_BASE;
		break;
#endif

	default:
		ET_ERROR(("ERROR: invalid GMAC specified\n"));
	}

	printf("Using GMAC%d (%p)\n", eth_data->mac, eth_data->regs);

	/* load options */
	eth_data->loopback = false;

	/* copy mac addr */
	if (getenv("ethaddr")) {
		if (!eth_getenv_enetaddr("ethaddr", eth_data->enetaddr)) {
			ET_ERROR(("ERROR: could not get env ethaddr\n"));
		}
	} else {
		ET_ERROR(("ERROR: could not get env ethaddr\n"));
	}
	stat = ethHw_dmaInit(eth_data);
	if (stat < 0) {
		printf("ethHw_dmaInit ret: %d\n", stat);
		return -1;
	}
#ifndef CONFIG_PEGASUS
	/* Enable RGMII data path from GMAC --> PHY */
	gmac_set_amac_rgmii(1);
#endif

	/* reset cores */
	gmac_core_reset(eth_data);

	stat = ethHw_chipAttach(eth_data);
	if (stat) {
		printf("ethHw_chipAttach ret: %d\n", stat);
		return -1;
	}

	/* init chip  */
	chip_init(eth_data, ET_INIT_FULL);

#ifdef BCMIPROC_ETH_DEBUG
	dmaTxDump(eth_data);
	dmaRxDump(eth_data);
	gmacRegDump(eth_data);
#endif

	return ETHHW_RC_NONE;
}

int ethHw_Exit(void)
{
	ET_TRACE(("%s enter\n", __func__));

	/* free rx descriptors buffer */
	if (rxDescBuf) {
		MFREE(0, (void *)(unsigned long)rxDescBuf, 0);
		rxDescBuf = 0;
		rxDescAlignedBuf = 0;
	}

	/* allocate rx data buffer */
	if (rxDataBuf) {
		MFREE(0, (void *)(unsigned long)rxDataBuf, 0);
		rxDataBuf = 0;
	}

	/* free tx descriptors buffer */
	if (txDescBuf) {
		MFREE(0, (void *)(unsigned long)txDescBuf, 0);
		txDescBuf = 0;
		txDescAlignedBuf = 0;
	}

	/* allocate tx data buffer */
	if (txDataBuf) {
		MFREE(0, (void *)(unsigned long)txDataBuf, 0);
		txDataBuf = 0;
	}

	/* Application code will normally control shutdown of the driver */
	return ETHHW_RC_NONE;
}

int ethHw_arlEntrySet(struct eth_device *dev)
{
	bcm_eth_t *eth_data = (bcm_eth_t *)dev->priv;
	gmacregs_t *regs = eth_data->regs;
	int i, rc = ETHHW_RC_NONE;

	/* copy mac addr */
	for (i = 0; i < ETH_ADDR_LEN; i++)
		eth_data->enetaddr[i] = dev->enetaddr[i];

	/* put mac in reset */
	gmac_init_reset(eth_data);

	reg32_write(&regs->mac_addr_high, htonl((eth_data->enetaddr[0] << 24)
	 | (eth_data->enetaddr[1] << 16) | (eth_data->enetaddr[2] << 8) | (eth_data->enetaddr[3])));
	reg32_write(&regs->mac_addr_low,  htons(eth_data->enetaddr[4] << 8 | eth_data->enetaddr[5]));

	/* bring mac out of reset */
	gmac_clear_reset(eth_data);

	return rc;
}

int ethHw_macEnableSet(int port, int en)
{
	bcm_eth_t *eth_data = &g_eth_data;
	gmacregs_t *regs = eth_data->regs;

	gmac_enable(eth_data, en);

	if (en) {
		/* clear interrupts */
		reg32_write(&regs->int_status, I_INTMASK);
	}

	/* reset phy: reset it once now */

	return ETHHW_RC_NONE;
}

#ifdef CONFIG_PEGASUS
void ethHw_gphyEnableSet(int port, int en)
{
	uint16 val;

	val = bcm_mdio_read(INTERNAL, CLAUS22, EGPHY28_BUSID,
			    EGPHY28_PHYID, PHY_1000X_MII_CTRLR_ADDR);
	if (en)
		val &= ~(MII_CTRL_PD | MII_CTRL_RESET);
	else
		val |= MII_CTRL_PD;

	bcm_mdio_write(INTERNAL, CLAUS22, EGPHY28_BUSID, EGPHY28_PHYID,
		       PHY_1000X_MII_CTRLR_ADDR, val);
}
#endif

int ethHw_macEnableGet(int port, int *txp, int *rxp)
{
	bcm_eth_t *eth_data = &g_eth_data;
	gmacregs_t *regs = eth_data->regs;
	u32 cmdcfg;

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	*txp = ((cmdcfg | CC_TE) ? 1 : 0);
	*rxp = ((cmdcfg | CC_RE) ? 1 : 0);

	return ETHHW_RC_NONE;
}

void ethHw_writel(u32 val, u32 addr)
{
	debug("Write [0x%08x] = 0x%08x\n", (u32)addr, val);
	reg32_write((u32 *)(unsigned long)addr, val);
}

u32 ethHw_readl(u32 addr)
{
	u32 val = reg32_read((u32 *)(unsigned long)addr);

	debug("Read  [0x%08x] = 0x%08x\n", (u32)addr, val);
	return val;
}

int ethHw_chipAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int stat;
	int i, link;
#if !EMULATION_TEST
	int chipid;
#endif
	char name[16];
	char *strptr;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	BZERO_SM((char *)ch, sizeof(eth_data->bcmgmac));

	/* get our phyaddr value */
	ch->phyaddr = CONFIG_EXTERNAL_PHY_DEV_ID;

	/* get our phyaddr value */
	sprintf(name, "et%dphyaddr", eth_data->unit);
	strptr = getenv(name);
	if (strptr != NULL)
		ch->phyaddr = simple_strtoul(strptr, &strptr, 10) & 0x1f;

	stat = ethHw_dmaAttach(eth_data);
	if (stat) {
		ET_ERROR(("et%d: ethHw_dmaAttach failed\n", eth_data->unit));
		printf("ethHw_dmaAttach failed: %d\n", stat);
		goto fail;
	}

	/* reset the gmac core */
	chip_reset(eth_data);

	ethHw_dmaTxInit(eth_data);
	ethHw_dmaRxInit(eth_data);

	/* set default sofware intmask */
	ch->def_intmask = DEF_INTMASK;
	ch->intmask = ch->def_intmask;

#if !EMULATION_TEST
	/* reset phy: reset it once now */
	chipid = iproc_get_chipid();
	printf("et%d: %s: Chip ID: 0x%x; phyaddr: 0x%x\n",
	       eth_data->unit, __func__, chipid, eth_data->bcmgmac.phyaddr);

	cmicd_miim_initialize();
#endif

#ifdef CONFIG_PHY_EGPHY28
	egphy28_init(eth_data, EGPHY28_PHYID);
#endif

#ifdef CONFIG_PHY_BCM5481
	phy54810_config_laneswap(eth_data, 1, ch->phyaddr);
#endif
	mdelay(1000);

	/* Give sufficient retries to detect link */
	for (i = 0; i < 5; i++) {
		link = ethHw_portLinkUp();
		if (link)
			break;

		mdelay(1000);
	}

	if (link == 0) {
		printf
		    ("ethHw_portLinkUp failed : Ethernet external port not connected\n");
		goto fail;
	}

	printf("ethHw_portLinkUp: Ethernet external port connected\n");
#ifdef CONFIG_NS2
	ethHw_checkPortSpeed();
#endif
	return 0;

fail:
	return -1;
}

void ethHw_chipDetach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	ethHw_dmaDetach(ch->di[0]);
	ch->di[0] = NULL;
}

int ethHw_dmaInit(bcm_eth_t *eth_data)
{
	int buflen;

	/* allocate rx descriptors buffer */
#ifdef CONFIG_NS2
	u32 alloc_ptr = IPROC_ETH_MALLOC_BASE;

	buflen = RX_DESC_LEN + DESC_ALIGNMENT;
	rxDescBuf = alloc_ptr;
	alloc_ptr += buflen;
#endif
	buflen = RX_DESC_LEN + DESC_ALIGNMENT;

	rxDescBuf = (u32)(unsigned long)MALLOC(0, buflen);
	if (rxDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Descriptor memory\n",
			  __func__));
		return -1;
	}
	rxDescAlignedBuf = rxDescBuf;
	ET_TRACE(("RX Descriptor Buffer: 0x%x; length: 0x%x\n", rxDescBuf,
		  buflen));
	/* align buffer */
	rxDescAlignedBuf = (rxDescBuf + DESC_ALIGNMENT - 1) &
	    (~(DESC_ALIGNMENT - 1));
	ET_TRACE(("RX Descriptor Buffer (aligned): %#X; length: 0x%lx\n",
		  rxDescAlignedBuf, RX_DESC_LEN));
	//printf("RX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", rxDescAlignedBuf, RX_DESC_LEN);

	/* allocate rx data buffer */
	buflen = RX_BUF_LEN;
#ifdef CONFIG_NS2
	rxDataBuf = alloc_ptr;
	alloc_ptr += buflen;
#endif
	rxDataBuf = (u32)(unsigned long)MALLOC(0, buflen);
	if (rxDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate RX Data Buffer memory\n",
			  __func__));
		return -1;
	}
	ET_TRACE(("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen));
	//printf("RX Data Buffer: 0x%x; length: 0x%x\n", rxDataBuf, buflen);

	/* allocate tx descriptors buffer */
	buflen = TX_DESC_LEN + DESC_ALIGNMENT;
#ifdef CONFIG_NS2
	txDescBuf = alloc_ptr;
	alloc_ptr += buflen;
#endif
	txDescBuf = (u32)(unsigned long)MALLOC(0, buflen);
	if (txDescBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Descriptor memory\n",
			  __func__));
		return -1;
	}
	txDescAlignedBuf = txDescBuf;
	ET_TRACE(("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf,
		  buflen));
	//printf("TX Descriptor Buffer: 0x%x; length: 0x%x\n", txDescBuf, buflen);
	/* align buffer */
	txDescAlignedBuf = (txDescBuf + DESC_ALIGNMENT - 1) &
	    (~(DESC_ALIGNMENT - 1));
	ET_TRACE(("TX Descriptor Buffer (aligned): %#x; length: 0x%lx\n",
		  txDescAlignedBuf, TX_DESC_LEN));
	//printf("TX Descriptor Buffer (aligned): 0x%x; length: 0x%x\n", txDescAlignedBuf, TX_DESC_LEN);

	/* allocate tx data buffer */
	buflen = TX_BUF_LEN;
#ifdef CONFIG_NS2
	txDataBuf = alloc_ptr;
	alloc_ptr += buflen;
#endif
	txDataBuf = (u32)(unsigned long)MALLOC(0, buflen);
	if (txDataBuf == 0) {
		ET_ERROR(("%s: Failed to allocate TX Data Buffer memory\n",
			  __func__));
		return -1;
	}
	ET_TRACE(("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen));
	//printf("TX Data Buffer: 0x%x; length: 0x%x\n", txDataBuf, buflen);

	return 0;
}

int ethHw_dmaRxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	u32 lastDscr;
	dma64dd_t *descp = NULL;
	u8 *bufp;
	int i;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	/* clear descriptor memory */
	BZERO_SM((void *)(unsigned long)RX_DESC_BASE, RX_DESC_LEN);
	/* clear buffer memory */
	BZERO_SM((void *)(unsigned long)RX_BUF_BASE, RX_BUF_LEN);

	/* Initialize RX DMA descriptor table */
	for (i = 0; i < RX_BUF_NUM; i++) {
		u32 ctrl;

		bufp = (u8 *)RX_BUF(i);
		descp = (dma64dd_t *) RX_DESC(i);
		ctrl = 0;
		/* if last descr set endOfTable */
		if (i == RX_BUF_NUM - 1)
			ctrl = D64_CTRL1_EOT;
		descp->ctrl1 = cpu_to_le32(ctrl);
		descp->ctrl2 = cpu_to_le32(RX_BUF_SIZE);
		descp->addrlow = cpu_to_le32((u32)(unsigned long)bufp);
		descp->addrhigh = 0;
		/* flush descriptor */
		//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));

		lastDscr = ((u32)(unsigned long)(descp) & D64_XP_LD_MASK) +
				sizeof(dma64dd_t);
	}

	/* initailize the DMA channel */
	reg32_write(&dma->d64rxregs->addrlow, RX_DESC_BASE);
	reg32_write(&dma->d64rxregs->addrhigh, 0);

	/* now update the dma last descriptor */
	reg32_write(&dma->d64rxregs->ptr, lastDscr);

	dma->rcvptrbase = RX_DESC_BASE;

	return 0;
}

int ethHw_dmaTxInit(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	u8 *bufp;
	int i;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	/* clear descriptor memory */
	BZERO_SM((void *)(unsigned long)TX_DESC_BASE, TX_DESC_LEN);
	/* clear buffer memory */
	BZERO_SM((void *)(unsigned long)TX_BUF_BASE, TX_BUF_LEN);

	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		u32 ctrl;

		bufp = (u8 *)TX_BUF(i);
		descp = (dma64dd_t *) TX_DESC(i);
		ctrl = 0;
		/* if last descr set endOfTable */
		if (i == TX_BUF_NUM - 1)
			ctrl = D64_CTRL1_EOT;
		descp->ctrl1 = cpu_to_le32(ctrl);
		descp->ctrl2 = 0;
		descp->addrlow = cpu_to_le32((u32)(unsigned long)bufp);
		descp->addrhigh = 0;
		/* flush descriptor */
		//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
	}

	/* initailize the DMA channel */
	reg32_write(&dma->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&dma->d64txregs->addrhigh, 0);

	dma->xmtptrbase = TX_DESC_BASE;

	/* now update the dma last descriptor */
	reg32_write(&dma->d64txregs->ptr, TX_DESC_BASE & D64_XP_LD_MASK);

	return 0;
}

int ethHw_dmaTx(size_t len, u8 *tx_buf)
{
	/* kick off the dma */
	bcm_eth_t *eth_data = &g_eth_data;
	dma_info_t *di = eth_data->bcmgmac.di[0];
	int txout = di->txout;
	int ntxd = di->ntxd;
	u32 flags;
	dma64dd_t *descp = NULL;
	u8 *bufp;
	u32 ctrl2;
	u32 lastDscr = ((u32)(unsigned long)(TX_DESC(NEXTTXD(txout)))
			     & (D64_XP_LD_MASK));
	size_t buflen;
#if BCMIPROC_ETH_DEBUG
	int i, dump_len = 0x40;
#endif

	REG_DEBUG(1);
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	if (txout > (ntxd - 1)) {
		ET_ERROR(("%s: ERROR - invalid txout 0x%x\n", __func__,
			  txout));
		REG_DEBUG(0);
		return -1;
	}

	/* load the buffer */
	buflen = dma_txload(txout, len, tx_buf);

	ctrl2 = (buflen & D64_CTRL2_BC_MASK);

	/* the transmit will only be one frame or set SOF, EOF */
	/* also set int on completion */
	flags = D64_CTRL1_SOF | D64_CTRL1_IOC | D64_CTRL1_EOF;

	/* txout points to the descriptor to uset */
	/* if last descriptor then set EOT */
	if (txout == (ntxd - 1)) {
		flags |= D64_CTRL1_EOT;
		lastDscr = ((u32)(unsigned long)(TX_DESC(0)) & D64_XP_LD_MASK);
	}

	/* write the descriptor */
	bufp = (u8 *)TX_BUF(txout);
	descp = (dma64dd_t *)TX_DESC(txout);
	descp->addrlow = cpu_to_le32((u32)(unsigned long)bufp);
	descp->addrhigh = 0;
	descp->ctrl1 = cpu_to_le32(flags);
	descp->ctrl2 = cpu_to_le32(ctrl2);

#if BCMIPROC_ETH_DEBUG
	if (buflen < 0x40)
		dump_len = buflen;

	printf("Tx Buf: len = 0x%lx\n", buflen);
	for (i = 0; i < dump_len; i++)
		printf("%02X ", bufp[i]);

	printf("\n");
#endif

	/* flush descriptor and buffer */
	//gmac_cache_flush((u32)descp, sizeof(dma64dd_t));
	//gmac_cache_flush((u32)bufp, buflen);

	/* now update the dma last descriptor */
	reg32_write(&di->d64txregs->ptr, lastDscr);

	/* tx dma should be enabled so packet should go out */

	/* update txout */
	di->txout = NEXTTXD(txout);
	REG_DEBUG(0);

	return 0;
}

int ethHw_dmaTxWait(void)
{
	/* wait for tx to complete */
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	u32 intstatus;
	int xfrdone = false;
	int i = 0;
#if defined(CHK_ETH_ERRS)
	dma_info_t *di = ch->di[0];
	u32 stat1
#endif
	 ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	/* clear stored intstatus */
	ch->intstatus = 0;

	intstatus = chip_getintr_events(eth_data, true);
	ET_TRACE(("int(0x%x)\n", intstatus));
	if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3))
		xfrdone = true;

	//printf("Waiting for TX to complete");

#if defined(CHK_ETH_ERRS)
	stat1 = reg32_read(&di->d64txregs->status1);
	if (stat1 & 0xf0000000)
		printf("%s stat1 (0x%x)\n", __func__, stat1);
#endif

	while (!xfrdone) {
		udelay(100);
		i++;
		if (i > 20) {
			printf
			    ("\nbcm iproc ethernet Tx failure! 20 retries\n");
			//REG_DEBUG(0);
			return -1;
		}
		intstatus = chip_getintr_events(eth_data, true);
		ET_TRACE(("int(0x%x)", intstatus));
		if (intstatus & (I_XI0 | I_XI1 | I_XI2 | I_XI3))
			xfrdone = true;
		else if (intstatus) {
			printf("int(0x%x)", intstatus);
		}
	}

#ifdef BCMIPROC_ETH_DEBUG
	dmaTxDump(eth_data);
	gmacRegDump(eth_data);
	gmac_mibTxDump(eth_data);
#endif

	return 0;
}

#if defined(CHK_ETH_ERRS)
/* get current and pending interrupt events */
static void check_errs(gmacregs_t *regs)
{
	static uint32 crserrs;
	uint32 err;

	err = reg32_read(&regs->mib.tx_jabber_pkts);
	if (err)
		printf("%s tx_jabber_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_oversize_pkts);
	if (err)
		printf("%s tx_oversize_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_fragment_pkts);
	if (err)
		printf("%s tx_fragment_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_underruns);
	if (err)
		printf("%s tx_underruns (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_total_cols);
	if (err)
		printf("%s tx_total_cols (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_single_cols);
	if (err)
		printf("%s tx_single_cols (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_multiple_cols);
	if (err)
		printf("%s tx_multiple_cols (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_excessive_cols);
	if (err)
		printf("%s tx_excessive_cols (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_late_cols);
	if (err)
		printf("%s tx_late_cols (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_defered);
	if (err)
		printf("%s tx_defered (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.tx_carrier_lost);
	crserrs += err;
	err = reg32_read(&regs->mib.tx_pause_pkts);
	if (err)
		printf("%s tx_pause_pkts (0x%x)\n", __func__, err);

	err = reg32_read(&regs->mib.rx_jabber_pkts);
	if (err)
		printf("%s rx_jabber_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_oversize_pkts);
	if (err)
		printf("%s rx_oversize_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_fragment_pkts);
	if (err)
		printf("%s rx_fragment_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_missed_pkts);
	if (err)
		printf("%s rx_missed_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_crc_align_errs);
	if (err)
		printf("%s rx_crc_align_errs (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_undersize);
	if (err)
		printf("%s rx_undersize (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_crc_errs);
	if (err)
		printf("%s rx_crc_errs (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_align_errs);
	if (err)
		printf("%s rx_align_errs (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_symbol_errs);
	if (err)
		printf("%s rx_symbol_errs (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_pause_pkts);
	if (err)
		printf("%s rx_pause_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_nonpause_pkts);
	if (err)
		printf("%s rx_nonpause_pkts (0x%x)\n", __func__, err);
	err = reg32_read(&regs->mib.rx_sachanges);
	if (err)
		printf("%s rx_sachanges (0x%x)\n", __func__, err);
}
#endif

int ethHw_dmaRx(void)
{
	u8 *buf;
	u8 rx_buf[1518];
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di = ch->di[0];
	void *bufp, *datap;
	int index;
	size_t rcvlen, buflen;
	u32 stat0, stat1;
	bool rxdata = false;
	int rc = 0;
	u32 control, offset;
	u32 statbuf[HWRXOFF];
	u32 *rcv_pkt_info = statbuf;
#if BCMDBG
	u32 ctf_miss_cnt, ctf_hit_cnt, ctf_fifo_high_wmark;
#endif

	//REG_DEBUG(1);
	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

#if BCMDBG
	ctf_miss_cnt = readl(0x20430000 + 0xcac);
	ctf_hit_cnt = readl(0x20430000 + 0xca8);
	ctf_fifo_high_wmark = readl(0x20430000 + 0xe48);

	ET_TRACE(("| ctf_hit_cnt=%#X, ctf_miss_cnt=%#X, ctf_fifo_high_wmark= %#X|\n", ctf_hit_cnt, ctf_miss_cnt, ctf_fifo_high_wmark));
#endif

#if defined(CHK_ETH_ERRS)
	check_errs(eth_data->regs);
#endif

	while (1) {
		bufp = dma_getnextrxp(ch->di[0], &index, &buflen, &stat0, &stat1);
#if defined(CHK_ETH_ERRS)
		if (stat1 & 0xf0000000)
			printf("%s stat1 (0x%x)\n", __func__, stat1);
#endif
		if (bufp != NULL) {
			ET_TRACE(("received packet\n"));
			ET_TRACE(("bufp(%p) index(0x%x) buflen(0x%lx) stat0(0x%x) stat1(0x%x)\n", bufp, index, buflen, stat0, stat1));

			// get buffer offset
			control = reg32_read(&di->d64rxregs->control);
			offset = (control & D64_RC_RO_MASK) >> D64_RC_RO_SHIFT;
			rcvlen = ltoh16(*(uint16 *) bufp);

			rcvlen = le16_to_cpu(rcvlen);
			if ((rcvlen == 0) || (rcvlen > RX_BUF_SIZE)) {
				ET_ERROR(("Wrong RX packet size 0x%x drop it\n",
					  (u32)rcvlen));
				/* refill buffre & descriptor */
				dma_rxrefill(ch->di[0], index);
				break;
			}

			ET_TRACE(("Received %ld bytes\n", rcvlen));
			/* cp into temp buf (need to copy data out of buf) */
			memcpy(statbuf, bufp, offset);
			datap = (void *)(unsigned long)((u32)(unsigned long)
						    bufp + offset);
#ifdef CONFIG_PEGASUS
			datap += 4;
#endif
			buf = rx_buf;
			memcpy(buf, datap, rcvlen);

			if ((*rcv_pkt_info) &
			    (1 << RCV_PKT_INFO_CRC_ERR_OFFSET)) {
				printf("CRC error...discarding Rx\n");
				rc = -2;
				break;
			}

			if ((*rcv_pkt_info) & (1 << RCV_PKT_INFO_RX_OVERFLOW)) {
				printf
				    ("Rx packet overflow error...discarding\n");
				rc = -2;
				break;
			}
#if BCMIPROC_ETH_DEBUG
			printf("Rx Buf: len = 0x%lx\n", rcvlen);
			int i, dump_len = 0x40;
			if (rcvlen < 0x40)
				dump_len = rcvlen;

			for (i = 0; i < dump_len; i++) {
				printf("%02X ", buf[i]);
			}
			printf("\n");
#endif

#ifdef UBOOT_MDK
			/* Send to MDK handler instead */
			mdk_rcv_handler(buf, rcvlen);
#else
			/* packet received, forward to uboot network handler */
			net_process_received_packet(buf, rcvlen);
#endif

			/* refill buffre & descriptor */
			dma_rxrefill(ch->di[0], index);
			rxdata = true;
			break;
		} else {
			if (!rxdata) {
				ET_TRACE(("No Rx Pkt available!\n"));
				/* Tell caller that no packet was received when Rx queue was polled */
				rc = -1;
				break;
			}
			/* at leasted received one packet */
			break;
		}
	}
	//REG_DEBUG(0);

#ifdef BCMIPROC_ETH_DEBUG
	dmaRxDump(eth_data);
	gmacRegDump(eth_data);
	gmac_mibRxDump(eth_data);
#endif

	return rc;
}

int ethHw_dmaAttach(bcm_eth_t *eth_data)
{
	bcmgmac_t *ch = &eth_data->bcmgmac;
	dma_info_t *di = NULL;
	char name[16];

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	/* allocate private info structure */
	di = MALLOC(0, sizeof(dma_info_t));
	if (!di) {
		ET_ERROR(("%s: out of memory, malloced %d bytes\n",
			  __func__, MALLOCED(0)));
		return -1;
	}

	BZERO_SM(di, sizeof(dma_info_t));

	di->dma64 = 1;
	di->d64txregs =
	    (dma64regs_t *)&eth_data->regs->dma_regs[TX_Q0].dmaxmt;
	di->d64rxregs =
	    (dma64regs_t *)&eth_data->regs->dma_regs[RX_Q0].dmarcv;
	/* Default flags: For backwards compatibility both Rx Overflow Continue
	 * and Parity are DISABLED.
	 * supports it.
	 */
	dma_ctrlflags(di, DMA_CTRL_ROC | DMA_CTRL_PEN, 0);
	di->rxburstlen = MAX_BURST_LEN;
	di->txburstlen = MAX_BURST_LEN;

	ET_TRACE(("rx burst len 0x%x\n", di->rxburstlen));
	ET_TRACE(("tx burst len 0x%x\n", di->txburstlen));

	di->ntxd = NTXD;
	di->nrxd = NRXD;

	di->rxbufsize = RX_BUF_SIZE;

	di->nrxpost = NRXBUFPOST;
	di->rxoffset = HWRXOFF;

	sprintf(name, "et%d", eth_data->unit);
	strncpy(di->name, name, MAXNAMEL);

	/* load dma struc addr */
	ch->di[0] = di;

	return 0;
}

int ethHw_dmaDetach(dma_info_t *di)
{

	ET_TRACE(("%s enter\n", __func__));

	if (di)
		/* free our private info structure */
		MFREE(0, (void *)di, sizeof(dma_info_t));

	return 0;
}

int ethHw_dmaDisable(int dir)
{
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	int stat;

	ET_TRACE(("%s enter\n", __func__));

	if (dir == DMA_TX)
		stat = dma_txreset(ch->di[0]);
	else
		stat = dma_rxreset(ch->di[0]);

	return stat;
}

int ethHw_dmaEnable(int dir)
{
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("et%d: %s enter\n", eth_data->unit, __func__));

	if (dir == DMA_TX)
		dma_txinit(ch->di[0]);
	else
		dma_rxinit(ch->di[0]);

	return 0;
}

int ethHw_portLinkUp(void)
{
	int link = 0;
	bcm_eth_t *eth_data = &g_eth_data;
	bcmgmac_t *ch = &eth_data->bcmgmac;
#ifdef CONFIG_PHY_BCM5481
	phy5481_link_get(eth_data, ch->phyaddr, &link);
#endif

#ifdef CONFIG_PHY_EGPHY28
	egphy28_link_get(eth_data, ch->phyaddr, &link);
#endif

	return link;
}

void ethHw_checkPortSpeed(void)
{
	int speed = 0, duplex = 0, speedcfg;
	bcm_eth_t *eth_data = &g_eth_data;
	static int orgspd, orgdpx;
#ifndef CONFIG_PEGASUS
	bcmgmac_t *ch = &eth_data->bcmgmac;

	phy5481_speed_get(eth_data, ch->phyaddr, &speed, &duplex);
#endif

	if (speed) {
		if (speed == 1000) {
			if (duplex)
				speedcfg = ET_1000FULL;
			else
				speedcfg = ET_1000HALF;
		} else if (speed == 100) {
			if (duplex)
				speedcfg = ET_100FULL;
			else
				speedcfg = ET_100HALF;
		} else if (speed == 10) {
			if (duplex)
				speedcfg = ET_10FULL;
			else
				speedcfg = ET_10HALF;
		} else {
			printf("ERROR: unknown speed %d\n", speed);
			return;
		}
#ifdef CONFIG_NS2
		{
			u32 id, rev, data;

			iproc_get_soc_id(&id, &rev);
			if ((id == IPROC_SOC_NS2) &&
			    (rev == IPROC_SOC_NS2_B0)) {
				switch (speed) {
				case 1000:
					data = 0x0196e800;
					break;
				case 100:
					data = 0x3196e400;
					break;
				case 10:
					data = 0x3196e000;
					break;
				default:
					return;
				}
				writel(data, (NICPM_ROOT + NICPM_IOMUX_CTRL));
			}
		}
#endif
		gmac_speed(eth_data, speedcfg);
	}
	if (orgspd != speed || orgdpx != duplex) {
		printf("ETH LINK UP: %d%s\n", speed, duplex ? "FD" : "HD");
		orgspd = speed;
		orgdpx = duplex;
	}
}

/* ==== Private Functions ================================================ */

#ifdef BCMIPROC_ETH_DEBUG
static void txDump(u8 *buf, int len)
{
	u8 *bufp;
	int i;

	bufp = (u8 *)buf;

	printf("Tx Buf: %p, %d\n", bufp, len);
	for (i = 0; i < len; i++)
		printf("%02X ", bufp[i]);

	printf("\n");
}

static void dmaTxDump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	u8 *bufp;
	int i;

	printf("TX DMA Register:\n");
	printf
	    ("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
	     reg32_read(&dma->d64txregs->control),
	     reg32_read(&dma->d64txregs->ptr),
	     reg32_read(&dma->d64txregs->addrlow),
	     reg32_read(&dma->d64txregs->addrhigh),
	     reg32_read(&dma->d64txregs->status0),
	     reg32_read(&dma->d64txregs->status1));

	printf("TX Descriptors:\n");
	for (i = 0; i < TX_BUF_NUM; i++) {
		descp = (dma64dd_t *) TX_DESC(i);
		printf
		    ("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
		     descp->ctrl1, descp->ctrl2, descp->addrhigh,
		     descp->addrlow);
	}

	printf("TX Buffers:\n");
	/* Initialize TX DMA descriptor table */
	for (i = 0; i < TX_BUF_NUM; i++) {
		bufp = (u8 *)TX_BUF(i);
		printf("buf%d:%p; ", i, bufp);
	}
	printf("\n");
}

static void dmaRxDump(bcm_eth_t *eth_data)
{
	dma_info_t *dma = eth_data->bcmgmac.di[0];
	dma64dd_t *descp = NULL;
	u8 *bufp;
	int i;

	printf("RX DMA Register:\n");
	printf
	    ("control:0x%x; ptr:0x%x; addrl:0x%x; addrh:0x%x; stat0:0x%x, stat1:0x%x\n",
	     reg32_read(&dma->d64rxregs->control),
	     reg32_read(&dma->d64rxregs->ptr),
	     reg32_read(&dma->d64rxregs->addrlow),
	     reg32_read(&dma->d64rxregs->addrhigh),
	     reg32_read(&dma->d64rxregs->status0),
	     reg32_read(&dma->d64rxregs->status1));

	printf("RX Descriptors:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		descp = (dma64dd_t *) RX_DESC(i);
		printf
		    ("ctrl1:0x%08x; ctrl2:0x%08x; addrhigh:0x%x; addrlow:0x%08x\n",
		     descp->ctrl1, descp->ctrl2, descp->addrhigh,
		     descp->addrlow);
	}

	printf("RX Buffers:\n");
	for (i = 0; i < RX_BUF_NUM; i++) {
		bufp = (u8 *)RX_BUF(i);
		printf("buf%d:%p; ", i, bufp);
	}
	printf("\n");
}

static void gmacRegDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	printf("GMAC Registers:\n");
	printf
	    ("dev_ctl:0x%x; dev_status:0x%x; int_status:0x%x; int_mask:0x%x; int_recv_lazy:0x%x\n",
	     reg32_read(&regs->dev_ctl), reg32_read(&regs->dev_status),
	     reg32_read(&regs->int_status), reg32_read(&regs->int_mask),
#ifdef CONFIG_PEGASUS
	     reg32_read((u32 *)(((u8 *)regs) + GMAC_INTR_RECV_LAZY_OFFSET)));
#else
	     reg32_read(&regs->int_recv_lazy);
#endif
	printf("UNIMAC Registers:\n");
	printf("cmd_cfg:0x%x; mac_h:0x%x; mac_l:0x%x; rx_max_len:0x%x\n",
	       reg32_read(&regs->cmd_cfg),
	       reg32_read(&regs->mac_addr_high),
	       reg32_read(&regs->mac_addr_low),
	       reg32_read(&regs->rx_max_length));
}

static void gmac_mibTxDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	u32 *ptr;
	u32 tmp;

	printf("GMAC TX MIB:\n");

	for (ptr = &regs->mib.tx_good_octets;
	     ptr <= &regs->mib.tx_q3_octets_high; ptr++) {
		tmp = reg32_read(ptr);
		printf("%p:0x%x; ", ptr, tmp);
	}
	printf("\n");

	return;
}

static void gmac_mibRxDump(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	u32 *ptr;
	u32 tmp;

	printf("GMAC RX MIB:\n");

	for (ptr = &regs->mib.rx_good_octets; ptr <= &regs->mib.rx_uni_pkts;
	     ptr++) {
		tmp = reg32_read(ptr);
		printf("%p:0x%x; ", ptr, tmp);
	}
	printf("\n");

	return;
}
#endif

static uint dma_ctrlflags(dma_info_t *di, uint mask, uint flags)
{
	uint dmactrlflags;

	ET_TRACE(("%s enter\n", __func__));

	dmactrlflags = di->hnddma.dmactrlflags;
	ASSERT((flags & ~mask) == 0);

	dmactrlflags &= ~mask;
	dmactrlflags |= flags;

	/* If trying to enable parity, check if parity is actually supported */
	if (dmactrlflags & DMA_CTRL_PEN) {
		uint32 control;

		control = reg32_read(&di->d64txregs->control);
		reg32_write(&di->d64txregs->control, control | D64_XC_PD);
		if (reg32_read(&di->d64txregs->control) & D64_XC_PD) {
			/* We *can* disable it so it is supported,
			 * restore control register
			 */
			reg32_write(&di->d64txregs->control, control);
		} else {
			/* Not supported, don't allow it to be enabled */
			dmactrlflags &= ~DMA_CTRL_PEN;
		}
	}

	di->hnddma.dmactrlflags = dmactrlflags;

	return dmactrlflags;
}

static int dma_rxenable(dma_info_t *di)
{
	u32 dmactrlflags = di->hnddma.dmactrlflags;
	u32 rxoffset = di->rxoffset;
	u32 rxburstlen = di->rxburstlen;

	ET_TRACE(("%s enter\n", __func__));

	uint32 control =
	    (reg32_read(&di->d64rxregs->control) & D64_RC_AE) | D64_RC_RE;

	if ((dmactrlflags & DMA_CTRL_PEN) == 0)
		control |= D64_RC_PD;

	if (dmactrlflags & DMA_CTRL_ROC)
		control |= D64_RC_OC;

	/*
	 *  These bits 20:18 (burstLen) of control register can be written but
	 * will take effect only if these bits are valid. So this will not
	 * affect previous versions of the DMA. They will continue to have
	 * those bits set to 0.
	 */
	control &= ~D64_RC_BL_MASK;
	control |= (rxburstlen << D64_RC_BL_SHIFT);
	control |= (rxoffset << D64_RC_RO_SHIFT);
	control |= (D64_RC_PC_4_DESCRIPTORS << D64_RC_PC_SHIFT);

	reg32_write(&di->d64rxregs->control, control);
	return 0;
}

static void dma_txinit(dma_info_t *di)
{
	uint32 control;
	u32 txburstlen = di->txburstlen;

	ET_TRACE(("%s enter\n", __func__));

	if (di->ntxd == 0)
		return;

	di->txin = di->txout = 0;
	di->hnddma.txavail = di->ntxd - 1;

	/* These bits 20:18 (burstLen) of control register can be written but will take
	 * effect only if these bits are valid. So this will not affect previous versions
	 * of the DMA. They will continue to have those bits set to 0.
	 */
	control = reg32_read(&di->d64txregs->control);

	control |= D64_XC_XE;
	if ((di->hnddma.dmactrlflags & DMA_CTRL_PEN) == 0)
		control |= D64_XC_PD;

	control |= (txburstlen << D64_XC_BL_SHIFT);
	control |= (D64_XC_PC_4_DESCRIPTORS << D64_XC_PC_SHIFT);
	reg32_write(&di->d64txregs->control, control);

	/* initailize the DMA channel */
	reg32_write(&di->d64txregs->addrlow, TX_DESC_BASE);
	reg32_write(&di->d64txregs->addrhigh, 0);
}

static void dma_rxinit(dma_info_t *di)
{
	ET_TRACE(("%s enter\n", __func__));

	di->rxin = di->rxout = 0;

	dma_rxenable(di);

	/* the rx descriptor ring should have the addresses set properly */
	/* set the lastdscr for the rx ring */
	reg32_write(&di->d64rxregs->ptr,
		    ((u32)(unsigned long)RX_DESC(RX_BUF_NUM) &
		     D64_XP_LD_MASK));
}

static bool dma_txreset(dma_info_t *di)
{
	uint32 status;

	ET_TRACE(("%s enter\n", __func__));

	if (di->ntxd == 0)
		return TRUE;

	/* address PR8249/PR7577 issue */
	/* suspend tx DMA first */
	reg32_write(&di->d64txregs->control, D64_XC_SE);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
		   D64_XS0_XS_DISABLED) &&
		   (status != D64_XS0_XS_IDLE) &&
		   (status != D64_XS0_XS_STOPPED), 10000);

	/* PR2414 WAR: DMA engines are not disabled until transfer finishes */
	reg32_write(&di->d64txregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64txregs->status0) & D64_XS0_XS_MASK)) !=
		   D64_XS0_XS_DISABLED), 10000);

	/* wait for the last transaction to complete */
	udelay(300);

	return (status == D64_XS0_XS_DISABLED);
}

static bool dma_rxreset(dma_info_t *di)
{
	uint32 status;

	ET_TRACE(("%s enter\n", __func__));

	if (di->nrxd == 0)
		return TRUE;

	/* PR2414 WAR: DMA engines are not disabled until transfer finishes */
	reg32_write(&di->d64rxregs->control, 0);
	SPINWAIT(((status = (reg32_read(&di->d64rxregs->status0) & D64_RS0_RS_MASK)) !=
		  D64_RS0_RS_DISABLED), 10000);

	return (status == D64_RS0_RS_DISABLED);
}

static int dma_txload(int index, size_t len, u8 *tx_buf)
{
#ifdef BCMIPROC_ETH_DEBUG
	int tlen = len;
#endif
	u8 *bufp = (u8 *)tx_buf;

	ET_TRACE(("%s enter\n", __func__));

#ifdef BCMIPROC_ETH_DEBUG
	printf("TX IN buf:%p; len:%ld\n", tx_buf, len);
	if (tlen > 64)
		tlen = 64;
	txDump(tx_buf, tlen);
#endif

	/* copy buffer */
	memcpy((u8 *)TX_BUF(index), bufp, len);

	/*
	 * The Ethernet packet has to be >= 64 bytes required by switch
	 * padding it with zeros FIXME
	 */
	if (len < 64) {
		memset((u8 *)TX_BUF(index) + len, 0, 64 - len);
		len = 64;
	}

	/* Add 4 bytes for Ethernet FCS/CRC */
	len += 4;

	/* Flush data, config, and descriptors to external memory */
	TX_FLUSH_CACHE();

#ifdef BCMIPROC_ETH_DEBUG
	printf("TX DMA buf:%p; len:%ld\n", TX_BUF(index), len);
	txDump((u8 *)TX_BUF(index), tlen);
#endif

	return len;
}

/*
 * this api will check if a packet has been received.  If so it will return the
 * address of the buffer and enter a bogus address into the descriptor table
 * rxin will be incremented to the next descriptor.
 * Once done with the frame the buffer should be added back onto the descriptor
 * and the lastdscr should be updated to this descriptor.
 */
static void *dma_getnextrxp(dma_info_t *di, int *index, size_t *len,
			    u32 *stat0, u32 *stat1)
{
	dma64dd_t *descp = NULL;
	uint i, curr, active;
	void *rxp;

	/* initialize return parameters */
	*index = 0;
	*len = 0;
	*stat0 = 0;
	*stat1 = 0;

	i = di->rxin;

	curr = B2I(((reg32_read(&di->d64rxregs->status0) & D64_RS0_CD_MASK) -
		    di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);
	active = B2I(((reg32_read(&di->d64rxregs->status1) & D64_RS0_CD_MASK) -
		      di->rcvptrbase) & D64_RS0_CD_MASK, dma64dd_t);

	/* check if any frame */
	if (i == curr)
		return NULL;

	ET_TRACE(("rxin(0x%x) curr(0x%x) active(0x%x)\n", i, curr, active));
	/* remove warning */
	if (i == active);

	/* get the packet pointer that corresponds to the rx descriptor */
	rxp = (void *)RX_BUF(i);

	descp = (dma64dd_t *)RX_DESC(i);

	descp->addrlow = 0xdeadbeef;
	descp->addrhigh = 0xdeadbeef;

	*index = i;
	*len = (descp->ctrl2 & D64_CTRL2_BC_MASK);
	*stat0 = reg32_read(&di->d64rxregs->status0);
	*stat1 = reg32_read(&di->d64rxregs->status1);

	di->rxin = NEXTRXD(i);

	ET_TRACE(("Rx ptr=%p", rxp));
	return rxp;
}

/*
 * Restore the buffer back on to the descriptor and
 * then lastdscr should be updated to this descriptor.
 */
static void dma_rxrefill(dma_info_t *di, int index)
{
	dma64dd_t *descp = NULL;
	void *bufp;

	/* get the packet pointer that corresponds to the rx descriptor */
	bufp = (void *)RX_BUF(index);
	descp = (dma64dd_t *)RX_DESC(index);

	/* update descriptor that is being added back on ring */
	descp->ctrl2 = cpu_to_le32(RX_BUF_SIZE);
	descp->addrlow = cpu_to_le32((u32)(unsigned long)bufp);
	descp->addrhigh = 0;
	/* flush descriptor */
}

static void gmac_core_reset(bcm_eth_t *eth_data)
{
#ifdef CONFIG_PEGASUS
	u32 tmp;
	tmp = readl(AMAC_IDM3_IDM_RESET_CONTROL);
	tmp &= 0xFFFFFFFE;
	writel(tmp, AMAC_IDM3_IDM_RESET_CONTROL);
	/*reg32_write((u32*)(AMAC_IDM3_IDM_RESET_CONTROL),0); */

	tmp = readl(AMAC_IDM3_IDM_IO_CONTROL);
#define IDM_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN 3
#define ETH_SS_0_AMAC_IDM_M3_IO_CONTROL_DIRECT__CLK_250_SEL 6
	tmp &= ~(1 << ETH_SS_0_AMAC_IDM_M3_IO_CONTROL_DIRECT__CLK_250_SEL);
	tmp |= (1 << IDM_IO_CONTROL_DIRECT__DEST_SYNC_MODE_EN);
	writel(tmp, AMAC_IDM3_IDM_IO_CONTROL);

	gmac_set_ctf_pass_thru_mode(eth_data);
	gmac_setup_unimac_port(eth_data);
#else
	reg32_write((u32 *)(AMAC_IDM_RESET_CONTROL), 0);
#endif
}

static void gmac_init_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	ET_TRACE(("%s enter\n", __func__));

	/* set command config reg CC_SR */
	reg32_set_bits(&regs->cmd_cfg, CC_SR);
	udelay(GMAC_RESET_DELAY);
}

static void gmac_clear_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;

	ET_TRACE(("%s enter\n", __func__));

	/* clear command config reg CC_SR */
	reg32_clear_bits(&regs->cmd_cfg, CC_SR);
	udelay(GMAC_RESET_DELAY);
}

void unimac_loopback(bcm_eth_t *eth_data, bool enable)
{
	gmacregs_t *regs = eth_data->regs;
	u32 ocmdcfg, cmdcfg;

	ET_TRACE(("%s enter\n", __func__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* set/clear the mac loopback mode */
	if (enable)
		cmdcfg |= CC_ML;
	else
		cmdcfg &= ~CC_ML;

	/* check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}

void gmac_loopback(bcm_eth_t *eth_data, bool enable)
{
	gmacregs_t *regs = eth_data->regs;
	u32 xmitctrl;

	ET_TRACE(("%s enter\n", __func__));

	/* read command cenablefig reg */
	xmitctrl = reg32_read((u32 *)&regs->dma_regs[0]);

	/* set/clear the mac loopback mode */
	if (enable)
		xmitctrl |= D64_XC_LE;
	else
		xmitctrl &= ~D64_XC_LE;

	reg32_write((u32 *)&regs->dma_regs[0], xmitctrl);
}

static void gmac_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	u32 ocmdcfg, cmdcfg;

	ET_TRACE(("%s enter\n", __func__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	cmdcfg &= ~(CC_RPI | CC_TAI | CC_HD | CC_ML |
		    CC_CFE | CC_RL | CC_RED | CC_PE | CC_TPI | CC_PAD_EN |
		    CC_PF);
	cmdcfg |= (CC_PROM | CC_NLC | CC_CFE);
#ifdef CONFIG_PEGASUS
	cmdcfg = UNIMAC_CMD_CFG_VAL;
#endif

	/* check if need to write register back */
	if (cmdcfg != ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write register */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}

static void gmac_clearmib(bcm_eth_t *eth_data)
{
#if 0
	gmacregs_t *regs = eth_data->regs;
	u32 *ptr;

	ET_TRACE(("%s enter\n", __func__));

	/* enable clear on read */
	reg32_set_bits(&regs->dev_ctl, DC_MROR);

	for (ptr = &regs->mib.tx_good_octets; ptr <= &regs->mib.rx_uni_pkts;
	     ptr++) {
		reg32_read(ptr);
		if (ptr == &regs->mib.tx_q3_octets_high)
			ptr++;
	}
#endif
	return;
}

static int gmac_speed(bcm_eth_t *eth_data, u32 speed)
{
	gmacregs_t *regs = eth_data->regs;
	u32 cmdcfg, ocmdcfg;
	u32 hd_ena = 0;

	ET_TRACE(("%s enter\n", __func__));

	switch (speed) {
	u32 id, rev;

	iproc_get_soc_id(&id, &rev);

	case ET_10HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_10FULL:
		speed = 0;
		if ((id == IPROC_SOC_NS2) &&
		    (rev == IPROC_SOC_NS2_B0)) {
			writel(NICPM_IOMUX_CTRL_VAL_B0_10M,
			       (NICPM_ROOT + NICPM_IOMUX_CTRL));
		}
		break;

	case ET_100HALF:
		hd_ena = CC_HD;
		/* FALLTHRU */

	case ET_100FULL:
		speed = 1;
		if ((id == IPROC_SOC_NS2) &&
		    (rev == IPROC_SOC_NS2_B0)) {
			writel(NICPM_IOMUX_CTRL_VAL_B0_100M,
			       (NICPM_ROOT + NICPM_IOMUX_CTRL));
		}
		break;

	case ET_1000FULL:
		speed = 2;
		break;

	case ET_1000HALF:
		ET_ERROR(("et%d: gmac_speed: supports 1000 mbps full duplex only\n", eth_data->unit));
		return FAILURE;

	case ET_2500FULL:
		speed = 3;
		break;

	default:
		ET_ERROR(("et%d: gmac_speed: speed %d not supported\n",
			  eth_data->unit, speed));
		return FAILURE;
	}

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* set the speed */
	cmdcfg &= ~(CC_ES_MASK | CC_HD);
	cmdcfg |= ((speed << CC_ES_SHIFT) | hd_ena);

	ET_TRACE(("%s(): cmdcfg=%#X ocmdcfg = %#X, speed=%d\n", __func__,
		  cmdcfg, ocmdcfg, speed));
	if (cmdcfg != ocmdcfg) {
		/* put mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
	return SUCCESS;
}

static void gmac_tx_flowcontrol(bcm_eth_t *eth_data, bool on)
{
	gmacregs_t *regs = eth_data->regs;
	u32 cmdcfg, ocmdcfg;

	ET_TRACE(("%s enter\n", __func__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* to enable tx flow control clear the rx pause ignore bit */
	if (on)
		cmdcfg &= ~CC_RPI;
	else
		cmdcfg |= CC_RPI;

	if (cmdcfg != ocmdcfg) {
		/* put the mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}

static void gmac_promisc(bcm_eth_t *eth_data, bool mode)
{
	gmacregs_t *regs = eth_data->regs;
	u32 cmdcfg, ocmdcfg;

	ET_TRACE(("%s enter\n", __func__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);
	ocmdcfg = cmdcfg;

	/* enable or disable promiscuous mode */
	if (mode)
		cmdcfg |= CC_PROM;
	else
		cmdcfg &= ~CC_PROM;

	if (cmdcfg != ocmdcfg) {
		/* put the mac in reset */
		gmac_init_reset(eth_data);
		/* write command config reg */
		reg32_write(&regs->cmd_cfg, cmdcfg);
		/* bring mac out of reset */
		gmac_clear_reset(eth_data);
	}
}

static void gmac_enable(bcm_eth_t *eth_data, bool en)
{
	gmacregs_t *regs = eth_data->regs;
	u32 cmdcfg;

	ET_TRACE(("%s enter\n", __func__));

	/* read command config reg */
	cmdcfg = reg32_read(&regs->cmd_cfg);

	/* put mac in reset */
	gmac_init_reset(eth_data);

	cmdcfg |= CC_SR;

	/* first deassert rx_ena and tx_ena while in reset */
	/* write command config reg */
	reg32_write(&regs->cmd_cfg, cmdcfg);

	/* bring mac out of reset */
	gmac_clear_reset(eth_data);

	/* enable the mac transmit and receive paths now */
	udelay(2);
	cmdcfg &= ~CC_SR;

	if (en)
		cmdcfg |= (CC_RE | CC_TE);
	else
		cmdcfg &= ~(CC_RE | CC_TE);

	/* assert rx_ena and tx_ena when out of reset to enable the mac */
	reg32_write(&regs->cmd_cfg, cmdcfg);
}

#define CMICD_BASE_ADDRESS		CMIC_RATE_ADJUST_EXT_MDIO
#define CMC2_OFFSET			    (0x00)

#define MIIM_PARAM_REG 			(0x23c)
#define MIIM_PARAM__MIIM_CYCLE_SHIFT	CMIC_MIIM_PARAM__MIIM_CYCLE_R
#define MIIM_PARAM__MIIM_CYCLE_MASK	((1 << CMIC_MIIM_PARAM__MIIM_CYCLE_WIDTH) - 1)
#define MIIM_PARAM__INTERNAL_SEL_SHIFT CMIC_MIIM_PARAM__INTERNAL_SEL
#define MIIM_PARAM__INTERNAL_SEL_MASK	((1 << CMIC_MIIM_PARAM__INTERNAL_SEL_WIDTH) - 1)
#define MIIM_PARAM__BUS_ID_SHIFT CMIC_MIIM_PARAM__BUS_ID_R
#define MIIM_PARAM__BUS_ID_MASK		((1 << CMIC_MIIM_PARAM__BUS_ID_WIDTH) - 1)
#define MIIM_PARAM__C45_SEL_SHIFT	CMIC_MIIM_PARAM__C45_SEL
#define MIIM_PARAM__C45_SEL_MASK	((1 << CMIC_MIIM_PARAM__INTERNAL_SEL_WIDTH) - 1)
#define MIIM_PARAM__PHY_ID_SHIFT CMIC_MIIM_PARAM__PHY_ID_R
#define MIIM_PARAM__PHY_ID_MASK		((1 << CMIC_MIIM_PARAM__PHY_ID_WIDTH) - 1)
#define MIIM_PARAM__PHY_DATA_SHIFT CMIC_MIIM_PARAM__PHY_DATA_R
#define MIIM_PARAM__PHY_DATA_MASK	((1 << CMIC_MIIM_PARAM__PHY_DATA_WIDTH) - 1)

#define MIIM_READ_DATA_REG 		CMIC_MIIM_READ_DATA_BASE
#define MIIM_READ_DATA__DATA_SHIFT	CMIC_MIIM_READ_DATA__DATA_R
#define MIIM_READ_DATA__DATA_MASK	((1 << CMIC_MIIM_READ_DATA__DATA_WIDTH) - 1)

#define MIIM_ADDRESS_REG 			CMIC_MIIM_ADDRESS_BASE
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT CMIC_MIIM_ADDRESS__CLAUSE_45_DTYPE_R
#define MIIM_ADDRESS__CLAUSE_45_DTYPE_MASK	((1 << CMIC_MIIM_ADDRESS__CLAUSE_45_DTYPE_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT  CMIC_MIIM_ADDRESS__CLAUSE_45_REGADR_R
#define MIIM_ADDRESS__CLAUSE_45_REGADR_MASK	((1 << CMIC_MIIM_ADDRESS__CLAUSE_45_REGADR_WIDTH) - 1)
#define MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT	0
#define MIIM_ADDRESS__CLAUSE_22_REGADR_MASK	(0x1F)

#define MIIM_CTRL_REG			CMIC_MIIM_CTRL_BASE
#define MIIM_CTRL__MIIM_RD_START_SHIFT  CMIC_MIIM_CTRL__MIIM_RD_START
#define MIIM_CTRL__MIIM_RD_START_MASK	((1 << CMIC_MIIM_CTRL__MIIM_RD_START_WIDTH) - 1)
#define MIIM_CTRL__MIIM_WR_START_SHIFT CMIC_MIIM_CTRL__MIIM_WR_START
#define MIIM_CTRL__MIIM_WR_START_MASK	((1 << CMIC_MIIM_CTRL__MIIM_WR_START_WIDTH) - 1)

#define MIIM_STAT_REG 			CMIC_MIIM_STAT_BASE
#define MIIM_STAT__MIIM_OPN_DONE_SHIFT CMIC_MIIM_STAT__MIIM_OPN_DONE
#define MIIM_STAT__MIIM_OPN_DONE_MASK	((1 << CMIC_MIIM_STAT__MIIM_OPN_DONE_WIDTH) - 1)

#define SET_REG_FIELD(reg_value, fshift, fmask, fvalue)	\
	(reg_value) = ((reg_value) & ~((fmask) << (fshift))) |  \
			(((fvalue) & (fmask)) << (fshift))
#define ISET_REG_FIELD(reg_value, fshift, fmask, fvalue) \
		(reg_value) = (reg_value) | (((fvalue) & (fmask)) << (fshift))
#define GET_REG_FIELD(reg_value, fshift, fmask)	\
	(((reg_value) & ((fmask) << (fshift))) >> (fshift))

#define MIIM_OP_MAX_HALT_USEC	500

enum {
	MIIM_OP_MODE_READ,
	MIIM_OP_MODE_WRITE,
	MIIM_OP_MODE_MAX
};

struct cmicd_miim_cmd {
	int bus_id;
	int int_sel;
	int phy_id;
	int regnum;
	int c45_sel;
	u16 op_mode;
	u16 val;
};

#if !EMULATION_TEST
static void cmicd_miim_initialize(void)
{
	u32 val;

	val = ethHw_readl(ICFG_IPROC_IOPAD_CTRL_11);
	val &= ~(3 << 11);
	val |= (2 << 11);	/*pull down*/
	ethHw_writel(val, ICFG_IPROC_IOPAD_CTRL_11);
	mdelay(10);
	val = ethHw_readl(ICFG_IPROC_IOPAD_CTRL_11);
	ET_TRACE(("***ICFG_IPROC_IOPAD_CTRL_11 %x val:%#x\n",
		  ICFG_IPROC_IOPAD_CTRL_11, val));

	val = ethHw_readl(CMIC_MIIM_SCAN_CTRL);
	val |= (1 << 28);
	ethHw_writel(val, CMIC_MIIM_SCAN_CTRL);
	mdelay(10);
	val = ethHw_readl(CMIC_MIIM_SCAN_CTRL);
	ET_TRACE(("***Read CMIC_MIIM_SCAN_CTRL %x val:%#x\n",
		  CMIC_MIIM_SCAN_CTRL, val));

	val = ethHw_readl(CMIC_RATE_ADJUST_EXT_MDIO);
	val = 0x00010008;
	ethHw_writel(val, CMIC_RATE_ADJUST_EXT_MDIO);
	mdelay(100);
	val = ethHw_readl(CMIC_RATE_ADJUST_EXT_MDIO);
	ET_TRACE(("***Read CMIC_RATE_ADJUST_EXT_MDIO %x val:%#x\n",
		  CMIC_RATE_ADJUST_EXT_MDIO, val));
}
#endif

static inline u32 cmicd_miim_reg_read(u32 reg)
{
	u32 val;

	val = ethHw_readl((CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg);
#if BCMDBG
	printf("***Read CMIC Reg. at: %x val:%#x\n",
	       (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg, val);
#endif
	return val;
}

static inline void cmicd_miim_reg_write(u32 reg, u32 data)
{
	ethHw_writel(data, (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg);
#if BCMDBG
	printf("Read to verify Wr: CMIC Reg. at: %x val:%#x\n",
	       (CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg,
	       ethHw_readl((CMICD_BASE_ADDRESS + CMC2_OFFSET) + reg));
#endif
}

static inline void cmicd_miim_set_op_read(u32 *data, u32 set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_RD_START_SHIFT,
		      MIIM_CTRL__MIIM_RD_START_MASK, set);
}

static inline void cmicd_miim_set_op_write(u32 *data, u32 set)
{
	SET_REG_FIELD(*data, MIIM_CTRL__MIIM_WR_START_SHIFT,
		      MIIM_CTRL__MIIM_WR_START_MASK, set);
}

static inline int do_cmicd_miim_op(u32 op, u32 param, u32 addr,
				   u16 *reg_val)
{
	u32 val, op_done;
	int ret = 0;
	int usec = MIIM_OP_MAX_HALT_USEC;

	if (op >= MIIM_OP_MODE_MAX) {
		ET_ERROR(("%s : invalid op code %d\n", __func__, op));
		return SOC_E_INIT;
	}

	/* Stop the on going operations if any before setting other regs */
	cmicd_miim_reg_write(MIIM_CTRL_REG, 0);

	cmicd_miim_reg_write(MIIM_PARAM_REG, param);
	cmicd_miim_reg_write(MIIM_ADDRESS_REG, addr);
	val = cmicd_miim_reg_read(MIIM_CTRL_REG);
	if (op == MIIM_OP_MODE_READ)
		cmicd_miim_set_op_read(&val, 1);
	else
		cmicd_miim_set_op_write(&val, 1);
	cmicd_miim_reg_write(MIIM_CTRL_REG, val);

	do {
		cmicd_miim_reg_read(MIIM_STAT_REG);
		op_done = GET_REG_FIELD(cmicd_miim_reg_read(MIIM_STAT_REG),
					MIIM_STAT__MIIM_OPN_DONE_SHIFT,
					MIIM_STAT__MIIM_OPN_DONE_MASK);
		if (op_done)
			break;

		udelay(1);
		usec--;
	} while (usec > 0);

	/* Need delay after DONE before reading */
	mdelay(100);

	if (op_done) {
		if (op == MIIM_OP_MODE_READ)
			*reg_val = cmicd_miim_reg_read(MIIM_READ_DATA_REG);
	}
	else
		ret = SOC_E_TIMEOUT;

	cmicd_miim_reg_write(MIIM_CTRL_REG, 0);

	return ret;
}

static int cmicd_miim_op(struct cmicd_miim_cmd *cmd)
{
	u32 miim_param = 0, miim_addr = 0;
	int rv = 0;

	ISET_REG_FIELD(miim_param, MIIM_PARAM__BUS_ID_SHIFT,
		       MIIM_PARAM__BUS_ID_MASK, cmd->bus_id);

	if (cmd->int_sel)
		ISET_REG_FIELD(miim_param, MIIM_PARAM__INTERNAL_SEL_SHIFT,
			       MIIM_PARAM__INTERNAL_SEL_MASK, 1);

	ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_ID_SHIFT,
		       MIIM_PARAM__PHY_ID_MASK, cmd->phy_id);

	if (cmd->op_mode == MIIM_OP_MODE_WRITE)
		ISET_REG_FIELD(miim_param, MIIM_PARAM__PHY_DATA_SHIFT,
			       MIIM_PARAM__PHY_DATA_MASK, cmd->val);

	if (cmd->c45_sel) {
		ISET_REG_FIELD(miim_param, MIIM_PARAM__C45_SEL_SHIFT,
			       MIIM_PARAM__C45_SEL_MASK, 1);

		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_REGADR_SHIFT,
			       MIIM_ADDRESS__CLAUSE_45_REGADR_MASK,
			       cmd->regnum);
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_45_DTYPE_SHIFT,
			       MIIM_ADDRESS__CLAUSE_45_REGADR_MASK,
			       cmd->regnum >> 16);
	} else {
		ISET_REG_FIELD(miim_addr, MIIM_ADDRESS__CLAUSE_22_REGADR_SHIFT,
			       MIIM_ADDRESS__CLAUSE_22_REGADR_MASK,
			       cmd->regnum);
	}

	rv = do_cmicd_miim_op(cmd->op_mode, miim_param, miim_addr, &cmd->val);

	return rv;
}

void chip_phy_wr(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg, u16 v)
{
	struct cmicd_miim_cmd cmd = { 0 };
	int rv;

	ET_TRACE(("%s enter\n", __func__));

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);
#ifdef CONFIG_NS2
	cmd.bus_id = 0;
#else
	cmd.bus_id = 3;
#endif
	cmd.c45_sel = 0;

	if (ext)
		cmd.int_sel = 0;
	else
		cmd.int_sel = 1;

	cmd.phy_id = phyaddr;
	cmd.regnum = reg;
	cmd.val = v;

	cmd.op_mode = MIIM_OP_MODE_WRITE;

	rv = cmicd_miim_op(&cmd);
	if (rv < 0)
		ET_ERROR(("%s : PHY register write is failed! error code is %d\n",
			  __func__, rv));
}

u16 chip_phy_rd(bcm_eth_t *eth_data, uint ext, uint phyaddr, uint reg)
{
	struct cmicd_miim_cmd cmd = { 0 };
	int rv;

	ET_TRACE(("%s enter\n", __func__));

	ASSERT(phyaddr < MAXEPHY);
	ASSERT(reg < MAXPHYREG);

#ifdef CONFIG_NS2
	cmd.bus_id = 0;
#else
	cmd.bus_id = 3;
#endif
	if (ext)
		cmd.int_sel = 0;
	else
		cmd.int_sel = 1;

	cmd.phy_id = phyaddr;
	cmd.regnum = reg;
	cmd.c45_sel = 0;
	cmd.op_mode = MIIM_OP_MODE_READ;

	rv = cmicd_miim_op(&cmd);
	if (rv < 0)
		ET_ERROR(("%s : PHY register read is failed!  %d\n",
			  __func__, rv));

	return cmd.val;
}

#ifndef CONFIG_PEGASUS
int chip_phy_auto_negotiate_gcd(bcm_eth_t *eth_data, uint phyaddr, int *speed,
				int *duplex)
{
	int ext = 0;
	int t_speed, t_duplex;
	uint16 mii_ana, mii_anp, mii_stat;
	uint16 mii_gb_stat, mii_esr, mii_gb_ctrl;

	mii_gb_stat = 0;	/* Start off 0 */
	mii_gb_ctrl = 0;	/* Start off 0 */

	mii_ana = chip_phy_rd(eth_data, ext, phyaddr, 0x04);
	mii_anp = chip_phy_rd(eth_data, ext, phyaddr, 0x05);
	mii_stat = chip_phy_rd(eth_data, ext, phyaddr, 0x01);

	if (mii_stat & MII_STAT_ES) {	/* Supports extended status */
		/*
		 * If the PHY supports extended status, check if it is 1000MB
		 * capable.  If it is, check the 1000Base status register to see
		 * if 1000MB negotiated.
		 */
		mii_esr = chip_phy_rd(eth_data, ext, phyaddr, 0x0f);

		if (mii_esr & (MII_ESR_1000_X_FD | MII_ESR_1000_X_HD |
			       MII_ESR_1000_T_FD | MII_ESR_1000_T_HD)) {
			mii_gb_stat = chip_phy_rd(eth_data, ext, phyaddr, 0x0a);
			mii_gb_ctrl = chip_phy_rd(eth_data, ext, phyaddr, 0x09);
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

int chip_phy_link_get(bcm_eth_t *eth_data, uint phyaddr, int *link)
{
	int ext = 0;
	uint16 mii_ctrl, mii_stat;
	unsigned long init_time;

	*link = FALSE;		/* Default */

	mii_stat = chip_phy_rd(eth_data, ext, phyaddr, 0x01);
	/*
	 * the first read of status register will not show link up,
	 * second read will show
	 */
	if (!(mii_stat & MII_STAT_LA))
		mii_stat = chip_phy_rd(eth_data, ext, phyaddr, 0x01);

	if (!(mii_stat & MII_STAT_LA) || (mii_stat == 0xffff)) {
		/* mii_stat == 0xffff check is to handle removable PHY cards */
		return SOC_E_NONE;
	}

	/* Link appears to be up; we are done if autoneg is off. */

	mii_ctrl = chip_phy_rd(eth_data, ext, phyaddr, 0x00);

	if (!(mii_ctrl & MII_CTRL_AE)) {
		*link = TRUE;
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
		mii_stat = chip_phy_rd(eth_data, ext, phyaddr, 0x01);

		if (!(mii_stat & MII_STAT_LA)) {
			/* link is down */
			return SOC_E_NONE;
		}

		if (mii_stat & MII_STAT_AN_DONE) {
			/* AutoNegotiation done */
			break;
		}

		if (get_timer(init_time) > 1) {
			/* timeout */
			return SOC_E_BUSY;
		}
	}

	/* Return link state at end of polling */
	*link = ((mii_stat & MII_STAT_LA) != 0);

	return SOC_E_NONE;
}
#endif

static void chip_reset(bcm_eth_t *eth_data)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("%s enter\n", __func__));

	/* reset the tx dma engines */
	dma_txreset(ch->di[TX_Q0]);

	/* set eth_data into loopback mode to ensure no rx traffic */
	udelay(1);

	/* reset the rx dma engine */
	dma_rxreset(ch->di[RX_Q0]);

	/* reset gmac */
	gmac_reset(eth_data);

	/* clear mib */
	gmac_clearmib(eth_data);

#ifndef CONFIG_PEGASUS
	/* set smi_master to drive mdc_clk */
	reg32_set_bits(&eth_data->regs->phy_ctl, PC_MTE);
#endif

#if UNIMAC_LOOPBACK
	unimac_loopback(eth_data, TRUE);
#endif

#if AMAC_DMA_LOOPBACK
	gmac_loopback(eth_data, eth_data->loopback);
#endif

#ifdef CONFIG_NS2
	ethHw_checkPortSpeed();
#endif

	/* clear persistent sw intstatus */
	reg32_write(&regs->int_status, 0);
	/* reg32_write(&regs->int_mask, 0); */
}

/*
 * Initialize all the chip registers.  If dma mode, init tx and rx dma engines
 * but leave the devcontrol tx and rx (fifos) disabled.
 */
static void chip_init(bcm_eth_t *eth_data, uint options)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;

	ET_TRACE(("%s enter\n", __func__));

	/* enable one rx interrupt per received frame */
#ifdef CONFIG_PEGASUS
	/* OFFSET 0x30 for Pegasus */
	reg32_write((u32 *)(((u8 *)regs) + GMAC_INTR_RECV_LAZY_OFFSET),
		    (1 << IRL_FC_SHIFT));
#else
	/* OFFSET 0x100 for NS2 */
	reg32_write(&regs->int_recv_lazy, (1 << IRL_FC_SHIFT));
#endif

#ifdef CONFIG_NS2
	/* enable 802.3x tx flow control (honor received PAUSE frames) */
	gmac_tx_flowcontrol(eth_data, TRUE);

	/* disable promiscuous mode */
	gmac_promisc(eth_data, TRUE);
#endif

	/* set our local address */
	ET_TRACE(("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		  (u32)eth_data->enetaddr[0], (u32)eth_data->enetaddr[1],
		  (u32)eth_data->enetaddr[2], (u32)eth_data->enetaddr[3],
		  (u32)eth_data->enetaddr[4], (u32)eth_data->enetaddr[5]));

	reg32_write(&regs->mac_addr_high, htonl((eth_data->enetaddr[0] << 24)
	 | (eth_data->enetaddr[1] << 16) | (eth_data->enetaddr[2] << 8) | (eth_data->enetaddr[3])));
	reg32_write(&regs->mac_addr_low,  htons(eth_data->enetaddr[4] << 8 | eth_data->enetaddr[5]));

	/* optionally enable mac-level loopback */
	/*gmac_loopback(eth_data, eth_data->loopback); */

	/* set max frame lengths - account for possible vlan tag */
	reg32_write(&regs->rx_max_length, PKTSIZE + 32);

#ifdef CONFIG_NS2
	ethHw_checkPortSpeed();
#endif

	/* enable the overflow continue feature and disable parity */
	dma_ctrlflags(ch->di[0], DMA_CTRL_ROC | DMA_CTRL_PEN /* mask */,
		      DMA_CTRL_ROC /* value */);
}

/* get current and pending interrupt events */
static u32 chip_getintr_events(bcm_eth_t *eth_data, bool in_isr)
{
	gmacregs_t *regs = eth_data->regs;
	bcmgmac_t *ch = &eth_data->bcmgmac;
	u32 intstatus;

	/* read the interrupt status register */
	intstatus = reg32_read(&regs->int_status);
	/* clear the int bits */
	reg32_write(&regs->int_status, intstatus);
	reg32_read(&regs->int_status);

	/* defer unsolicited interrupts */
	intstatus &= (in_isr ? ch->intmask : ch->def_intmask);

	/* or new bits into persistent intstatus */
	intstatus = (ch->intstatus |= intstatus);

	/* return intstatus */
	return intstatus;
}
