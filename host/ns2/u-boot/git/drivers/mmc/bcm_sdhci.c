/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <mmc.h>
#include <asm/io.h>
#include <malloc.h>
#include "bcm_sdhci.h"
#include <asm/arch/socregs.h>
#include <sdhci.h>

#ifdef CONFIG_NS2
/*2 SDIO instances*/
#define MAX_CONTROLLER_COUNT	2
#else
#define MAX_CONTROLLER_COUNT	1
#endif

#define FREQ_MIN 400000
#define FREQ_MAX 52000000

#define NAND_EMMC_IOMUX_MASK 0xc00
#define NAND_EMMC_IOMUX_SHIFT 0xa

/*Define to use standard open source SDHCI driver*/
#define USE_STD_SDHCI_DRIVER

#ifndef USE_STD_SDHCI_DRIVER
static void mmc_prepare_data(struct mmc_host *host, struct mmc_data *data)
{
	unsigned int temp = 0;

	debug("Enter %s\n", __func__);

	debug("data->dest: 0x%p, size: %d\n", data->dest, data->blocksize);
	writel((int)data->dest, &host->reg->sysad);
	debug("dest read back: %08x\n", readl(&host->reg->sysad));

	/*temp = ( 7 << SDIO0_eMMCSDXC_BLOCK__HSBS_R) | data->blocksize |
	 * ( data->blocks << SDIO0_eMMCSDXC_BLOCK__BCNT_R);
	 */
	temp =
	    (0 << SDIO0_eMMCSDXC_BLOCK__HSBS_R) | data->blocksize |
	    (data->blocks << SDIO0_eMMCSDXC_BLOCK__BCNT_R);
	writel(temp, &host->reg->blkcnt_sz);
}

static void iproc_mmc_clear_all_intrs(struct mmc_host *host)
{
	/* Clear all interrupts. */
	writel(0xFFFFFFFF, &host->reg->norintsts);
	udelay(1000);
}

static void iproc_mmc_decode_cmd(unsigned int cmd)
{
	debug("Cmd=%08x,", cmd);
	debug("Cidx=%d,", cmd >> 24);
	debug("ctyp=%d,", (cmd >> 22) & 3);
	debug("dps=%d,", (cmd >> 21) & 1);
	debug("cchk=%d,", (cmd >> 20) & 1);
	debug("crc=%d,", (cmd >> 19) & 1);
	debug("rtsel=%d,", (cmd >> 16) & 3);
	debug("msbs=%d,", (cmd >> 5) & 1);
	debug("r/w=%d,", (cmd >> 4) & 1);
	debug("bcen=%d,", (cmd >> 1) & 1);
	debug("dma=%d\n", (cmd >> 0) & 1);
}

static void iproc_mmc_change_clock(struct mmc_host *host, uint clock)
{
	int div = 0;
	unsigned int clk;
	unsigned long timeout;

	debug("Enter %s clock : %d\n", __func__, clock);

	clk = readl(&host->reg->ctrl1_clkcon_timeout_swrst);
	clk = clk & 0xFFFF0000;	/* Clean up all bits related to clock.*/
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);
	clk = 0;

	/*Check the below dividers once */

	if (clock == 0)
		return;
	if (clock <= 400000)
		div = 500;
	else if (clock <= 20000000)
		div = 10;
	else if (clock <= 26000000)
		div = 8;
	else
		div = 4;	/*50MHz*/
	debug("div: %d\n", div);

	div >>= 1;

	/* Write divider value, and enable internal clock. */
	clk =
	    readl(&host->reg->
		  ctrl1_clkcon_timeout_swrst) |
	    (div << SDIO0_eMMCSDXC_CTRL1__SDCLKSEL_R) |
	    (1 << SDIO0_eMMCSDXC_CTRL1__ICLKEN_R);
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);

	/* Wait for clock to stabilize */
	/* Wait max 10 ms */
	timeout = 20;
	while (!
	       (readl(&host->reg->ctrl1_clkcon_timeout_swrst) &
		(1 << SDIO0_eMMCSDXC_CTRL1__ICLKSTB_R))) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return;
		}
		timeout--;
		udelay(10000);
	}

	/* Enable sdio clock now.*/
	clk |=
	    (1 << SDIO0_eMMCSDXC_CTRL1__SDCLKEN_R) |
	    readl(&host->reg->ctrl1_clkcon_timeout_swrst);
	writel(clk, &host->reg->ctrl1_clkcon_timeout_swrst);
	host->clock = clock;
}

static int iproc_mmc_send_cmd(struct mmc *mmc, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	struct mmc_host *host = (struct mmc_host *)mmc->priv;
	int flags = 0;
	int i = 0;
	unsigned int timeout;
	unsigned int mask;
	unsigned int cmd_complete_retry = 100;
	unsigned int resp_retry = 10000;

	debug("Send_Cmd: ");

	/* Wait max 10 ms */
	timeout = 10;

	/*
	 * PRNSTS
	 * CMDINHDAT[1] : Command Inhibit (DAT)
	 * CMDINHCMD[0] : Command Inhibit (CMD)
	 */
	/* Set command inhibit. */
	mask = (1 << SDIO0_eMMCSDXC_PSTATE__CMDINH_R);
	/* Set dat inhibit. */
	if ((data) || (cmd->resp_type & MMC_RSP_BUSY))
		mask |= (1 << SDIO0_eMMCSDXC_PSTATE__DATINH_R);

	/*
	 * We shouldn't wait for data inihibit for stop commands, even
	 * though they might use busy signaling
	 */
	if (data)
		mask &= ~(1 << SDIO0_eMMCSDXC_PSTATE__DATINH_R);

	while (readl(&host->reg->prnsts) & mask) {
		if (timeout == 0) {
			printf("%s : timeout error\n", __func__);
			return TIMEOUT;
		}
		timeout--;
		udelay(10000);
	}

	/* Set up block cnt, and block size.*/
	if (data)
		mmc_prepare_data(host, data);

	debug("cmd->arg: %08x\n", cmd->cmdarg);
	if (cmd->cmdidx == 17 || cmd->cmdidx == 24)
		printf(".");
	writel(cmd->cmdarg, &host->reg->argument);

	flags = 0;
	if (data) {
		flags = (1 << SDIO0_eMMCSDXC_CMD__BCEN_R);
		debug("Data_Send: PIO, BCEN=1\n");

		/* Multiple block select.*/
		if (data->blocks > 1)
			flags |= (1 << SDIO0_eMMCSDXC_CMD__MSBS_R);
		/* 1= read, 0=write.*/
		if (data->flags & MMC_DATA_READ)
			flags |= (1 << SDIO0_eMMCSDXC_CMD__DTDS_R);
	}

	if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
		return -1;

	/*
	 * CMDREG
	 * CMDIDX[29:24]: Command index
	 * DPS[21]      : Data Present Select
	 * CCHK_EN[20]  : Command Index Check Enable
	 * CRC_EN[19]   : Command CRC Check Enable
	 * RTSEL[1:0]
	 *      00 = No Response
	 *      01 = Length 136
	 *      10 = Length 48
	 *      11 = Length 48 Check busy after response
	 */
	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags |= (0 << SDIO0_eMMCSDXC_CMD__RTSEL_R);
	else if (cmd->resp_type & MMC_RSP_136)
		flags |= (1 << SDIO0_eMMCSDXC_CMD__RTSEL_R);
	else if (cmd->resp_type & MMC_RSP_BUSY)
		flags |= (3 << SDIO0_eMMCSDXC_CMD__RTSEL_R);
	else
		flags |= (2 << SDIO0_eMMCSDXC_CMD__RTSEL_R);

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= (1 << SDIO0_eMMCSDXC_CMD__CRC_EN_R);
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= (1 << SDIO0_eMMCSDXC_CMD__CCHK_EN_R);
	if (data)
		flags |= (1 << SDIO0_eMMCSDXC_CMD__DPS_R);

	debug("cmd: %d\n", cmd->cmdidx);
	flags |= (cmd->cmdidx << SDIO0_eMMCSDXC_CMD__CIDX_R);
	iproc_mmc_decode_cmd(flags);

	writel(flags, &host->reg->cmdreg);

	iproc_mmc_decode_cmd(readl(&host->reg->cmdreg));
	udelay(10000);

	for (i = 0; i < cmd_complete_retry; i++) {
		mask = readl(&host->reg->norintsts);
		/* Command Complete */
		if (mask & (1 << 0)) {
			if (!data)
				writel(mask, &host->reg->norintsts);
			break;
		}
		udelay(1000);	/* Needed for cmd0 */
	}

	if (i == cmd_complete_retry) {
		debug("%s: waiting for status update for cmd %d\n", __func__,
		      cmd->cmdidx);
		/* Set CMDRST and DATARST bits.
		 * Problem :
		 * -------
		 *  When a command 8 is sent in case of MMC card, it will not
		 *  respond, and CMD INHIBIT bit of PRSTATUS register will be
		 *  set to 1, causing no more commands to be sent from host
		 *  controller. This causes things to stall.
		 *  Solution :
		 *  ---------
		 *  In order to avoid this situation, we clear the CMDRST and
		 *  DATARST bits in the case when card
		 *  doesn't respond back to a command sent by host controller.
		 */
		writel(((0x3 << SDIO0_eMMCSDXC_CTRL1__CMDRST_R) |
			(readl(&host->reg->ctrl1_clkcon_timeout_swrst))),
		       &host->reg->ctrl1_clkcon_timeout_swrst);
		iproc_mmc_clear_all_intrs(host);
		printf("\niproc_mmc_send_cmd() returned timeout\n");
		return TIMEOUT;
	}

	if (mask & (1 << 16)) {
		/* Timeout Error */
		printf("timeout: %08x cmd %d\n", mask, cmd->cmdidx);
		/* Clear up the CMD inhibit and DATA inhibit bits.*/
		return TIMEOUT;

	} else if (mask & (1 << 15)) {
		/* Error Interrupt */

		if (mask & (1 << 20))
			printf("error: DTOERROR //Data Time out error\n");

		if (mask & (1 << 21))
			printf("error: DCRCERR //Data CRC error\n");

		if (mask & (1 << 22))
			printf("error: DEBERR //Data end bit error\n");

		printf("error: %08x cmd %d\n", mask, cmd->cmdidx);
		return -1;
	}

	if (cmd->resp_type & MMC_RSP_PRESENT) {
		if (cmd->resp_type & MMC_RSP_136) {
			/* CRC is stripped so we need to do some shifting. */
			for (i = 0; i < 4; i++) {
				unsigned int offset =
				    (unsigned int)(&host->reg->rspreg3 - i);
				cmd->response[i] = readl(offset) << 8;

				if (i != 3)
					cmd->response[i] |= readb(offset - 1);
				debug("cmd->resp[%d]: %08x\n", i,
				      cmd->response[i]);
			}
		} else if (cmd->resp_type & MMC_RSP_BUSY) {
			for (i = 0; i < resp_retry; i++) {
				/* PRNTDATA[23:20] : DAT[3:0] Line Signal */
				if (readl(&host->reg->prnsts)
				    & (1 << 20))	/* DAT[0] */
					break;
			}

			if (i == resp_retry) {
				printf("%s: card is still busy resp_retry=%u\n",
				       __func__, resp_retry);
				return TIMEOUT;
			}

			cmd->response[0] = readl(&host->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		} else {
			cmd->response[0] = readl(&host->reg->rspreg0);
			debug("cmd->resp[0]: %08x\n", cmd->response[0]);
		}
	}

	/* Read PIO */
	if (data && (data->flags & MMC_DATA_READ)) {
		unsigned int len = data->blocksize * data->blocks;
		unsigned int chunk, scratch;
		unsigned char *buf;

		while (1) {
			mask = readl(&host->reg->norintsts);

			if (mask & (1 << 0))
				/*debug("cmd completed\n");*/
				if (mask & (1 << 5)) {
					debug("Buf Rd Ready\n");
					break;
				}
			if (mask & (1 << 15)) {
				/* Error Interrupt */
				writel(mask, &host->reg->norintsts);
				printf("%s: error during transfer: 0x%08x\n",
				       __func__, mask);
				return -1;
			}
		}

		/* Loop read */
		buf = (unsigned char *)data->dest;
		chunk = 0;
		scratch = 0;
		while (len) {
			if (chunk == 0) {
				scratch = readl(&host->reg->bdata);
				chunk = 4;
				debug("bdata=%08x\t", scratch);
			}

			*buf = scratch & 0xFF;

			buf++;
			scratch >>= 8;
			chunk--;
			len--;
		}
		debug("\n");

		while (1) {
			mask = readl(&host->reg->norintsts);

			if (mask & (1 << 1)) {
				debug("Rx Completed\n");
				break;
			}
			if (mask & (1 << 15)) {
				/* Error Interrupt */
				writel(mask, &host->reg->norintsts);
				printf("%s: error during transfer: 0x%08x\n",
				       __func__, mask);
				return -1;
			}
		}
	}

	/* Write PIO */
	else if (data && !(data->flags & MMC_DATA_READ)) {
		unsigned int len = data->blocksize * data->blocks;
		unsigned int chunk, scratch, blksize = 512;
		unsigned char *buf;

		while (1) {
			mask = readl(&host->reg->norintsts);

			if (mask & (1 << 4)) {
				debug("Buf Wr Ready\n");
				break;
			}
			if (mask & (1 << 15)) {
				/* Error Interrupt */
				writel(mask, &host->reg->norintsts);
				printf("%s: error during transfer: 0x%08x\n",
				       __func__, mask);
				return -1;
			}
		}

		buf = (unsigned int *)data->dest;
		chunk = 0;
		scratch = 0;
		while (len) {
			scratch |= (u32)*buf << (chunk * 8);

			buf++;
			chunk++;
			len--;

			if ((chunk == 4) || ((len == 0) && (blksize == 0))) {
				writel(scratch, &host->reg->bdata);
				debug("bdata=%08x\n", scratch);
				chunk = 0;
				scratch = 0;
			}
		}
		debug("\n");

		while (1) {
			mask = readl(&host->reg->norintsts);
			debug("INT_sts: 0x%08x\n", mask);

			if (mask & (1 << 1)) {
				debug("Tx Completed\n");
				break;
			}
			if (mask & (1 << 15)) {
				/* Error Interrupt */
				writel(mask, &host->reg->norintsts);
				printf("%s: error during transfer: 0x%08x\n",
				       __func__, mask);
				return -1;
			}
		}

	}			/* if PIO*/

	/* Clear all interrupts as per FPGA code.*/
	writel(0xFFFFFFFF, &host->reg->norintsts);
	udelay(1000);

	debug("\n");
	return 0;
}

static void iproc_mmc_set_ios(struct mmc *mmc)
{
	struct mmc_host *host = mmc->priv;
	unsigned long ctrl;

	debug("Enter %s bus : %x\n", __func__, mmc->bus_width);

	if (mmc->clock != host->clock) {
		iproc_mmc_change_clock(host, mmc->clock);
		return;
	}

	ctrl = readl(&host->reg->ctrl_host_pwr_blk_wak);
	/*  1 = 4-bit mode , 0 = 1-bit mode */
	if (mmc->bus_width == 8)
		ctrl |= (1 << SDIO0_eMMCSDXC_CTRL__SDB_R);
	else if (mmc->bus_width == 4) {
		ctrl |= (1 << SDIO0_eMMCSDXC_CTRL__DXTW_R);
		/*HS mode.*/
		ctrl |= (1 << SDIO0_eMMCSDXC_CTRL__HSEN_R);
	} else
		ctrl &= ~(1 << SDIO0_eMMCSDXC_CTRL__DXTW_R);

	ctrl |= 0x4;
	ctrl |= 0xd00;

	writel(ctrl, &host->reg->ctrl_host_pwr_blk_wak);
}

static void iproc_mmc_reset(struct mmc_host *host)
{
	unsigned int timeout;

	debug("Enter %s\n", __func__);

	/* Software reset for all * 1 = reset * 0 = work */
	writel((1 << SDIO0_eMMCSDXC_CTRL1__RST_R),
	       &host->reg->ctrl1_clkcon_timeout_swrst);

	host->clock = 0;

	/* Wait max 100 ms */
	timeout = 100;

	/* hw clears the bit when it's done */
	while (readl(&host->reg->ctrl1_clkcon_timeout_swrst) &
	       (1 << SDIO0_eMMCSDXC_CTRL1__RST_R)) {
		if (timeout == 0) {
			printf("%s: timeout error\n", __func__);
			return;
		}
		timeout--;
		udelay(1000);
	}
	printf("Reset Done\n");
}

static int iproc_mmc_core_init(struct mmc *mmc)
{
	struct mmc_host *host = (struct mmc_host *)mmc->priv;
	unsigned int mask;

	debug("Enter %s\n", __func__);

	iproc_mmc_reset(host);
	mdelay(500);

	/* Set power now.*/
	mask =
	    readl(&host->reg->
		  ctrl_host_pwr_blk_wak) | (5 << SDIO0_eMMCSDXC_CTRL__SDVSEL_R)
	    | (1 << SDIO0_eMMCSDXC_CTRL__SDPWR_R);
	writel(mask, &host->reg->ctrl_host_pwr_blk_wak);

	mdelay(500);

#define HCVERSIRQ_SPECVER_MASK 0x00FF0000
	host->version = readl(SDIO0_eMMCSDXC_HCVERSIRQ);
	host->version =
	    (host->
	     version & (HCVERSIRQ_SPECVER_MASK)) >>
	    SDIO0_eMMCSDXC_HCVERSIRQ__SPECVER_R;

	/* mask all */
	writel(0xffffffff, &host->reg->norintstsen);
	writel(0xffffffff, &host->reg->norintsigen);

	/*DATA line timeout*/
	/* TMCLK * 2^26 */
	writel(((0xd << SDIO0_eMMCSDXC_CTRL1__DTCNT_R) |
			(readl(&host->reg->ctrl1_clkcon_timeout_swrst))),
			&host->reg->ctrl1_clkcon_timeout_swrst);

	/*
	 * Interrupt Status Enable Register init
	 * bit 5 : Buffer Read Ready Status Enable
	 * bit 4 : Buffer write Ready Status Enable
	 * bit 1 : Transfre Complete Status Enable
	 * bit 0 : Command Complete Status Enable
	 */
	mask = readl(&host->reg->norintstsen);
	mask &= ~(0xffff);
	mask |=
	    (1 << SDIO0_eMMCSDXC_INTREN1__BUFRREN_R) |
	    (1 << SDIO0_eMMCSDXC_INTREN1__BUFWREN_R)
	    | (1 << SDIO0_eMMCSDXC_INTREN1__TXDONEEN_R) |
	    (1 << SDIO0_eMMCSDXC_INTREN1__CMDDONEEN_R)
	    | (1 << SDIO0_eMMCSDXC_INTREN1__DMAIRQEN_R);
	writel(mask, &host->reg->norintstsen);

	/*
	 * Interrupt Signal Enable Register init
	 * bit 1 : Transfer Complete Signal Enable
	 */
	mask = readl(&host->reg->norintsigen);
	mask &= ~(0xffff);
	mask |= (1 << SDIO0_eMMCSDXC_INTREN2__TXDONE_R);
	writel(mask, &host->reg->norintsigen);

	debug("MMC_Init Done\n");
	return 0;
}

static struct mmc_ops bcm_ops = {
	.send_cmd = iproc_mmc_send_cmd,
	.set_ios = iproc_mmc_set_ios,
	.init = iproc_mmc_core_init,
};

static struct mmc_config cfg = {
	.name = "bcm_sdhci",
	.ops = &bcm_ops,
	.host_caps = MMC_MODE_8BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS,
	.voltages = MMC_VDD_165_195 | MMC_VDD_27_28 | MMC_VDD_28_29
	    | MMC_VDD_29_30 | MMC_VDD_30_31 | MMC_VDD_31_32
	    | MMC_VDD_32_33 | MMC_VDD_33_34 | MMC_VDD_34_35 | MMC_VDD_35_36,
	.f_min = FREQ_MIN,
	.f_max = FREQ_MAX,
};

#endif /* !USE_STD_SDHCI_DRIVER */

int board_mmc_init(bd_t *bis)
{
	int cnt = 0;

#ifdef USE_STD_SDHCI_DRIVER
	struct sdhci_host *host = NULL;
#else
	struct mmc_host *mmc_host = NULL;
#endif

	debug("Enter %s\n", __func__);

	for (cnt = 0; cnt < MAX_CONTROLLER_COUNT; cnt++) {
#ifndef USE_STD_SDHCI_DRIVER
		mmc_host = malloc(sizeof(struct mmc_host));
		if (!mmc_host) {
			printf("mmc_host malloc fail!\n");
			return -1;
		}

		mmc_host->clock = 0;
		if (cnt == 0)
			mmc_host->reg =
			    (volatile struct iproc_mmc *)SDIO0_eMMCSDXC_SYSADDR;
#ifdef SDIO1_eMMCSDXC_SYSADDR
		else if (cnt == 1)
			mmc_host->reg = (struct iproc_mmc *)
					 SDIO1_eMMCSDXC_SYSADDR;
#endif
		mmc_create(&cfg, mmc_host);
#else
		host = (struct sdhci_host *)malloc(sizeof(struct sdhci_host));
		if (!host) {
			printf("%s: sdhci host malloc fail!\n", __func__);
			return -1;
		}

		host->name = "bcm-sdhci";

		if (cnt == 0)
			host->ioaddr = (void *)SDIO0_eMMCSDXC_SYSADDR;
#ifdef SDIO1_eMMCSDXC_SYSADDR
		else if (cnt == 1)
			host->ioaddr = (void *)SDIO1_eMMCSDXC_SYSADDR;
#endif
		/*Add appropriate quirks here */
		host->quirks = 0;

		if (FREQ_MAX <= 52000000)
			host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;

		host->host_caps =
		    MMC_MODE_8BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS |
		    MMC_MODE_HC | MMC_MODE_4BIT | MMC_MODE_HS_200;

		host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

		/*If it does not work straight away consider setting set_clock
		 * here host->set_clock
		 */
		if (add_sdhci(host, FREQ_MAX, FREQ_MIN)) {
			printf("Init failed for SDIO%d\n", cnt);
			return -1;
		}
#endif
	}
	return 0;
}
