/*
 * $Copyright Open Broadcom Corporation$
 */
/* debug/trace */
#define BCMDBG_ERR
//#define BCMDBG
#ifdef BCMDBG
#define	ET_ERROR(args) printf args
#define	ET_TRACE(args) printf args
#define BCMIPROC_ETH_DEBUG
#elif defined(BCMDBG_ERR)
#define	ET_ERROR(args) printf args
#define ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#else
#define	ET_ERROR(args)
#define	ET_TRACE(args)
#undef BCMIPROC_ETH_DEBUG
#endif /* BCMDBG */

#include <common.h>
#include <malloc.h>
#include <net.h>
#include <config.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>

#define BCMIPROC_ETH_DEV_NAME          "bcmiproc_eth"

#define BCM_NET_MODULE_DESCRIPTION    "Broadcom BCM IPROC Ethernet driver"
#define BCM_NET_MODULE_VERSION        "0.1"

static const char banner[] = BCM_NET_MODULE_DESCRIPTION " " BCM_NET_MODULE_VERSION "\n";
extern int ethHw_miiphy_read(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);
extern int ethHw_miiphy_write(unsigned int const phyaddr, 
		   unsigned int const reg, unsigned short *const value);


/******************************************************************
 * u-boot net functions
 */
static int bcmiproc_eth_send(struct eth_device *dev, void *packet, int length)
{
	u8 *buf = (uint8_t *)packet;
	int rc;

	ET_TRACE(("%s enter\n", __func__));

	/* load buf and start transmit */
	rc = ethHw_dmaTx( length, buf );
	if (rc) {
		ET_ERROR(("ERROR - Tx failed\n"));
		return rc;
	}

	rc = ethHw_dmaTxWait();
	if (rc) {
		ET_ERROR(("ERROR - no Tx notice\n"));
		return rc;
	}

	ET_TRACE(("%s exit rc(0x%x)\n", __func__, rc));
	return rc;
}

static int
bcmiproc_eth_rcv(struct eth_device *dev)
{
	int rc;

	ET_TRACE(("%s enter\n", __func__));

	rc = ethHw_dmaRx();

	ET_TRACE(("%s exit rc(0x%x)\n", __func__, rc));
	return rc;
}


static int
bcmiproc_eth_write_hwaddr(struct eth_device* dev)
{
	int rc=0;

	ET_TRACE(("%s enter\n", __func__));

	printf("\nMAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
		dev->enetaddr[0], dev->enetaddr[1], dev->enetaddr[2],
		dev->enetaddr[3], dev->enetaddr[4], dev->enetaddr[5]);

	ET_TRACE(("%s exit rc(0x%x)\n", __func__, rc));
	return rc;
}


static int
bcmiproc_eth_open( struct eth_device *dev, bd_t * bt )
{
	int rc=0;
	ET_TRACE(("%s enter\n", __func__));

	/* enable tx and rx DMA */
	ethHw_dmaEnable(DMA_RX);
	ethHw_dmaEnable(DMA_TX);

	/* Enable forwarding to internal port */
	ethHw_macEnableSet(ETHHW_PORT_INT, 1);
#ifdef CONFIG_PEGASUS
	ethHw_gphyEnableSet(ETHHW_PORT_INT, 1);
#endif

	ET_TRACE(("%s exit rc(0x%x)\n", __func__, rc));
	return rc;
}

static void
bcmiproc_eth_close(struct eth_device *dev)
{
	ET_TRACE(("%s enter\n", __func__));

	/* Disable forwarding to internal port */
	/* Disable Rx from source order */
#ifdef CONFIG_PEGASUS
	ethHw_gphyEnableSet(ETHHW_PORT_INT, 0);
#endif
	ethHw_macEnableSet(ETHHW_PORT_INT, 0);

	mdelay(300); /* Allow time for Rx pipeline to flush fully */

	/* disable DMA */
	ethHw_dmaDisable(DMA_RX);
	ethHw_dmaDisable(DMA_TX);

	ethHw_macEnableSet(ETHHW_PORT_INT, 0);

	ET_TRACE(("%s exit\n", __func__));
}


int
bcmiproc_miiphy_read(char *devname, unsigned int const addr, 
   unsigned int const reg, unsigned short *const value)
{
	return ethHw_miiphy_read(addr, reg, value);
}


int
bcmiproc_miiphy_write(char *devname, unsigned int const addr, 
   unsigned int const reg, unsigned short *const value)
{
	return ethHw_miiphy_write(addr, reg, value);
}


int
bcmiproc_eth_register(u8 dev_num)
{
	struct eth_device *dev;
	int rc;

	ET_TRACE(("%s enter\n", __func__));

	dev = (struct eth_device *) malloc(sizeof(struct eth_device));
	if (dev == NULL) {
		return -1;
	}
	memset(dev, 0, sizeof(*dev));
	sprintf(dev->name, "%s-%hu", BCMIPROC_ETH_DEV_NAME, dev_num);

	printf(banner);

	/* Initialization */
	ET_TRACE(("Ethernet initialization...\n"));
	rc = ethHw_Init();	
	if (rc) {;}
	ET_TRACE(("Ethernet initialization %s (rc=%i)\n",
			(rc>=0) ? "successful" : "failed", rc));

	dev->iobase = 0;

	dev->init = bcmiproc_eth_open;
	dev->halt = bcmiproc_eth_close;
	dev->send = bcmiproc_eth_send;
	dev->recv = bcmiproc_eth_rcv;
	dev->write_hwaddr = bcmiproc_eth_write_hwaddr;

	eth_register(dev);

	ET_TRACE(("Ethernet Driver registered...\n"));

#ifdef CONFIG_CMD_MII
   miiphy_register(dev->name, bcmiproc_miiphy_read, bcmiproc_miiphy_write);
#endif

	ET_TRACE(("%s exit\n", __func__));
	return 1;
}

