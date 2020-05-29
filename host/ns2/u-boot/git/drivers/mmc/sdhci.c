/*
 * Copyright 2011, Marvell Semiconductor Inc.
 * Lei Wen <leiwen@marvell.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Back ported to the 8xx platform (from the 8260 platform) by
 * Murray.Jensen@cmst.csiro.au, 27-Jan-01.
 */

#include <common.h>
#include <malloc.h>
#include <mmc.h>
#include <sdhci.h>
#include <asm/arch/socregs.h>

void *aligned_buffer;
void chal_sd_dump_reg(struct sdhci_host *host);
/* Define a 200Mhz Base Clock */
#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
#define NS2_MMC_BASE_CLOCK 200000000
#define SDHCI_PCF8574TS_U92	0x21
#define SEL_VOLT_1V8		0
#define SEL_VOLT_3V3		1

/* To support 3v3 <-> 1v8 */
extern int i2c_recv_byte(u8 devaddr, u8 *value);
extern int i2c_send_byte(u8 devaddr, u8 value);
extern int i2c_probe(uchar chip);
#endif

static void sdhci_reset(struct sdhci_host *host, u8 mask)
{
	unsigned long timeout;

	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("%s: Reset 0x%x never completed.\n",
			       __func__, (int)mask);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static void sdhci_cmd_done(struct sdhci_host *host, struct mmc_cmd *cmd)
{
	int i;
	if (cmd->resp_type & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			cmd->response[i] = sdhci_readl(host,
						       SDHCI_RESPONSE + (3 -
									 i) *
						       4) << 8;
			if (i != 3)
				cmd->response[i] |= sdhci_readb(host,
								SDHCI_RESPONSE +
								(3 - i) * 4 -
								1);
		}
	} else {
		cmd->response[0] = sdhci_readl(host, SDHCI_RESPONSE);
	}
}

static void sdhci_transfer_pio(struct sdhci_host *host, struct mmc_data *data)
{
	int i;
	char *offs;
	for (i = 0; i < data->blocksize; i += 4) {
		offs = data->dest + i;
		if (data->flags == MMC_DATA_READ)
			*(u32 *)offs = sdhci_readl(host, SDHCI_BUFFER);
		else
			sdhci_writel(host, *(u32 *)offs, SDHCI_BUFFER);
	}
}

static int sdhci_transfer_data(struct sdhci_host *host, struct mmc_data *data,
			       unsigned int start_addr)
{
	unsigned int stat, rdy, mask, timeout, block = 0;
#ifdef CONFIG_MMC_SDMA
	unsigned char ctrl;
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
#endif

	timeout = 1000000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			return -1;
		}
		if (stat & rdy) {
			if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & mask))
				continue;
			sdhci_writel(host, rdy, SDHCI_INT_STATUS);
			sdhci_transfer_pio(host, data);
			data->dest += data->blocksize;
			if (++block >= data->blocks)
				break;
		}
#ifdef CONFIG_MMC_SDMA
		if (stat & SDHCI_INT_DMA_END) {
			sdhci_writel(host, SDHCI_INT_DMA_END, SDHCI_INT_STATUS);
			start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
			start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
			sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		}
#endif
		if (timeout-- > 0)
			udelay(10);
		else {
			printf("%s: Transfer data timeout\n", __func__);
			return -1;
		}
	} while (!(stat & SDHCI_INT_DATA_END));

	return 0;
}

/*
 * No command will be sent by driver if card is busy, so driver must wait
 * for card ready state.
 * Every time when card is busy after timeout then (last) timeout value will be
 * increased twice but only if it doesn't exceed global defined maximum.
 * Each function call will use last timeout value. Max timeout can be redefined
 * in board config file.
 */
#ifndef CONFIG_SDHCI_CMD_MAX_TIMEOUT
#define CONFIG_SDHCI_CMD_MAX_TIMEOUT		3200
#endif
#define CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT	100

static int sdhci_send_command(struct mmc *mmc, struct mmc_cmd *cmd,
			      struct mmc_data *data)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int stat = 0;
	int ret = 0;
	int i = 0;
	int trans_bytes = 0, is_aligned = 1;
	u32 mask, flags, mode;
	unsigned int time = 0, start_addr = 0;
	int mmc_dev = mmc->block_dev.devnum;
	unsigned start = get_timer(0);
	unsigned int retry = 10000;
	unsigned int cmd_complete_retry = 100;

	/* Timeout unit - ms */
	static unsigned int cmd_timeout = CONFIG_SDHCI_CMD_DEFAULT_TIMEOUT;

	stat = sdhci_readl(host, SDHCI_INT_STATUS);
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~SDHCI_DATA_INHIBIT;

	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
		if (time >= cmd_timeout) {
			printf("%s: MMC: %d busy ", __func__, mmc_dev);
			if (2 * cmd_timeout <= CONFIG_SDHCI_CMD_MAX_TIMEOUT) {
				cmd_timeout += cmd_timeout;
				chal_sd_dump_reg(host);
				printf("timeout increasing to: %u ms.\n",
				       cmd_timeout);
			} else {
				chal_sd_dump_reg(host);
				puts("timeout.\n");
				return COMM_ERR;
			}
		}
		time++;
		udelay(1000);
	}

	if (cmd->cmdidx == MMC_CMD_TUNING) {
		mask &= ~SDHCI_INT_RESPONSE;
		mask &= SDHCI_INT_DATA_AVAIL;
	} else {
		mask = SDHCI_INT_RESPONSE;
	}

	if (!(cmd->resp_type & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (cmd->resp_type & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (cmd->resp_type & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		mask |= SDHCI_INT_DATA_END;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (cmd->resp_type & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (cmd->resp_type & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;
	if (data)
		flags |= SDHCI_CMD_DATA;

	mode = 0;
	/* Set Transfer mode regarding to data flag */
	if (data != 0) {
		/* writel((int)data->dest, &host->reg->sysad); */
		sdhci_writel(host, (int)(unsigned long)data->dest,
			     SDHCI_DMA_ADDRESS);

		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = data->blocks * data->blocksize;
		if (data->blocks > 1)
			mode |= SDHCI_TRNS_MULTI;

		if (data->flags == MMC_DATA_READ)
			mode |= SDHCI_TRNS_READ;
#ifdef CONFIG_MMC_SDMA
		if (data->flags == MMC_DATA_READ)
			start_addr = (unsigned int)data->dest;
		else
			start_addr = (unsigned int)data->src;
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
		    (start_addr & 0x7) != 0x0) {
			is_aligned = 0;
			start_addr = (unsigned int)aligned_buffer;
			if (data->flags != MMC_DATA_READ)
				memcpy(aligned_buffer, data->src, trans_bytes);
		}

		sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		mode |= SDHCI_TRNS_DMA;
#endif

#if 0
		sdhci_writew(host, SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
						    data->blocksize),
			     SDHCI_BLOCK_SIZE);
#endif

		/* For CMD6 i.e. Switch Function, try out something.
		 */
		if (cmd->cmdidx == SD_CMD_SWITCH_FUNC) {
			sdhci_writew(host, SDHCI_MAKE_BLKSZ(0, data->blocksize),
				     SDHCI_BLOCK_SIZE);
		} else {
			sdhci_writew(host, SDHCI_MAKE_BLKSZ(0, data->blocksize),
				     SDHCI_BLOCK_SIZE);
		}
		sdhci_writew(host, data->blocks, SDHCI_BLOCK_COUNT);
		/* NOTE:
		 * Setup the mode, but do not write it out yet. There might be a
		 * problem when this particular register is written to, to
		 * affect only 16 bits instead of the full 32bits
		 * Seems like the AXI to APB translation writes the full 32bit
		 * word in each case i.e.
		 * whether one tries to access the upper or lower word.
		 */
		/*sdhci_writew(host, mode, SDHCI_TRANSFER_MODE); */
	}

	sdhci_writel(host, cmd->cmdarg, SDHCI_ARGUMENT);
#ifdef CONFIG_MMC_SDMA
	flush_cache(start_addr, trans_bytes);
#endif
	/*sdhci_writew(host,SDHCI_MAKE_CMD(cmd->cmdidx, flags),SDHCI_COMMAND);*/
	sdhci_writel(host, ((SDHCI_MAKE_CMD(cmd->cmdidx, flags) << 16) | mode),
		     SDHCI_TRANSFER_MODE);
	udelay(10000);
#if 0
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR)
			break;
		if (--retry == 0)
			break;
	} while ((stat & mask) != mask);
#endif

	/* For CMD6 i.e. Switch Function, try out something else.
	 */
	cmd_complete_retry = 100;
	if (cmd->cmdidx == SD_CMD_SWITCH_FUNC) {
		for (i = 0; i < cmd_complete_retry; i++) {
			stat = sdhci_readl(host, SDHCI_INT_STATUS);
			/* Command Complete */
			if (stat & (1 << 0)) {
				if (!data) {
					sdhci_writel(host, stat,
						     SDHCI_INT_STATUS);
				}
				break;
			}
			udelay(1000);	/* Needed for cmd0 */
		}
	} else {
		do {
			/* sdhci_readl addr=0x66420030 val=0x00000021reg=0x30 */
			stat = sdhci_readl(host, SDHCI_INT_STATUS);
			/* SDHCI_INT_ERROR      0x00008000 */
			if (stat & SDHCI_INT_ERROR) {
				if (cmd->cmdidx != 0x6) {
					break;
				} else {
					break;
				}
			}

			if (--retry == 0)
				break;

			/*
			 * execution time for this loop can be different for
			 * various cpu frueq. because it is just checking
			 * register for 'retry' count without any fix time.
			 * Adding a fix delay so that emmc/sd card can completed
			 * command execution. and hence avoiding false timeout
			 * condition.
			 */
			udelay(100);
		} while ((stat & mask) != mask);
	}

	if (retry == 0) {
		if (host->quirks & SDHCI_QUIRK_BROKEN_R1B)
			return 0;
		else {
			printf("%s: Timeout for status update!\n", __func__);
			return TIMEOUT;
		}
	}

	if ((stat & (SDHCI_INT_ERROR | mask)) == mask) {
		sdhci_cmd_done(host, cmd);
		chal_sd_dump_reg(host);
		sdhci_writel(host, mask, SDHCI_INT_STATUS);
	} else {
		ret = -1;
	}

	if (!ret && data) {
		chal_sd_dump_reg(host);
		ret = sdhci_transfer_data(host, data, start_addr);
	}

	if (host->quirks & SDHCI_QUIRK_WAIT_SEND_CMD)
		udelay(1000);

	stat = sdhci_readl(host, SDHCI_INT_STATUS);
	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	if (!ret) {
		chal_sd_dump_reg(host);
		if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) &&
		    !is_aligned && (data->flags == MMC_DATA_READ))
			memcpy(data->dest, aligned_buffer, trans_bytes);
		return 0;
	}

	sdhci_reset(host, SDHCI_RESET_CMD);
	sdhci_reset(host, SDHCI_RESET_DATA);
	if (stat & SDHCI_INT_TIMEOUT)
		return TIMEOUT;
	else
		return COMM_ERR;
}

static int sdhci_set_clock(struct mmc *mmc, unsigned int clock)
{
	struct sdhci_host *host = mmc->priv;
	unsigned int div, clk, timeout, reg;
	unsigned int max_clk;

#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
	max_clk = NS2_MMC_BASE_CLOCK;
#else
	max_clk = mmc->cfg->f_max;
#endif

	/* Wait max 20 ms */
	timeout = 200;
	while (sdhci_readl(host, SDHCI_PRESENT_STATE) &
			   (SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT)) {
		if (timeout == 0) {
			printf("%s: Timeout to wait cmd & data inhibit\n",
			       __func__);
			return -1;
		}

		timeout--;
		udelay(100);
	}

	reg = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
	reg &= ~SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, reg, SDHCI_CLOCK_CONTROL);

	if (clock == 0)
		return 0;

	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		/* Version 3.00 divisors must be a multiple of 2. */
		if (max_clk <= clock)
			div = 1;
		else {
			for (div = 2; div < SDHCI_MAX_DIV_SPEC_300; div += 2) {
				if ((max_clk / div) <= clock)
					break;
			}
		}
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((max_clk / div) <= clock)
				break;
		}
	}
	div >>= 1;

	if (host->set_clock)
		host->set_clock(host->index, div);

	clk = (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
	    << SDHCI_DIVIDER_HI_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL))
		 & SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("%s: Internal clock never stabilised.\n",
			       __func__);
			return -1;
		}
		timeout--;
		udelay(1000);
	}

	clk |= SDHCI_CLOCK_CARD_EN;
#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
	host->clock = clock;
	if (clock <= 52000000)
		host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;
	else
		host->quirks &= ~(SDHCI_QUIRK_NO_HISPD_BIT);
#endif
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
	chal_sd_dump_reg(host);
	return 0;
}

static void sdhci_set_power(struct sdhci_host *host, unsigned short power)
{
	u8 pwr = 0;

	printf("power.....\n");
	if (power != (unsigned short)-1) {
		switch (1 << power) {
		case MMC_VDD_165_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_VDD_29_30:
		case MMC_VDD_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_VDD_32_33:
		case MMC_VDD_33_34:
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	if (pwr == 0) {
		sdhci_writeb(host, 0, SDHCI_POWER_CONTROL);
		return;
	}

	if (host->quirks & SDHCI_QUIRK_NO_SIMULT_VDD_AND_POWER)
		sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);

	pwr |= SDHCI_POWER_ON;

	sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);
	chal_sd_dump_reg(host);
}

static void sdhci_set_ios(struct mmc *mmc)
{
	u32 ctrl;
	u32 errstat;
	struct sdhci_host *host = mmc->priv;

	if (host->set_control_reg)
		host->set_control_reg(host);

	if (mmc->clock != host->clock)
		sdhci_set_clock(mmc, mmc->clock);

	/* Set bus width */
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (mmc->bus_width == 8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) ||
		    (host->quirks & SDHCI_QUIRK_USE_WIDE8))
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (mmc->bus_width == 4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}

	if (mmc->clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;

	if (host->quirks & SDHCI_QUIRK_NO_HISPD_BIT)
		ctrl &= ~SDHCI_CTRL_HISPD;

	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);

	if (mmc->ddr_mode == 1) {
		errstat = sdhci_readl(host, SDHCI_ACMD12_ERR);
		errstat |= 0x40000;
		sdhci_writel(host, errstat, SDHCI_ACMD12_ERR);
	}
}

#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
static void sdhci_set_taps(struct mmc *mmc)
{
	u32 ctrl;

	/* Need to set the correct TAP settings based on what speed we are
	 * running...
	 */
	if (mmc->cfg->host_caps & MMC_MODE_HS_200) {
		debug("--> Clear IP and OP Settings for HS200 use...\n");
		if (mmc->block_dev.devnum == 0) {
			ctrl = readl(SDIO_IDM0_IO_CONTROL_DIRECT);
			debug("--> Read SDIO_IDM0_IO_CONTROL_DIRECT == %08X\n",
			      ctrl);
			ctrl &= 0xFFFE0001;
			debug
			    ("->AbouttowriteSDIO_IDM0_IO_CONTROL_DIRECT=%08X\n",
			     ctrl);
			writel(ctrl, SDIO_IDM0_IO_CONTROL_DIRECT);
		} /*else {
		   * ctrl = readl(SDIO_IDM1_IO_CONTROL_DIRECT);
		   * debug("--> Read SDIO_IDM1_IO_CONTROL_DIRECT
		   * == %08X\n",ctrl);
		   * ctrl &= 0xFFFE0001;
		   * debug("--> About to write
		   * SDIO_IDM1_IO_CONTROL_DIRECT == %08X\n",ctrl);
		   * writel(ctrl,SDIO_IDM1_IO_CONTROL_DIRECT);
		   * }
		   */
	}
}

static int sdhci_set_tuning(struct mmc *mmc)
{

	struct sdhci_host *host = mmc->priv;
	u32 ctrl;

	/* Set sampling clock to 0 */
	ctrl = sdhci_readl(host, SDHCI_ACMD12_ERR);
	ctrl &= ~(1 << 23);
	sdhci_writel(host, ctrl, SDHCI_ACMD12_ERR);

	/* set exectune */
	ctrl = sdhci_readl(host, SDHCI_ACMD12_ERR);
	debug("--> About to hit exec tune, ERRSTAT == %08X\n", ctrl);
	ctrl |= (1 << 22);
	sdhci_writel(host, ctrl, SDHCI_ACMD12_ERR);
	ctrl = sdhci_readl(host, SDHCI_ACMD12_ERR);
	debug("--> After hit exec tune, ERRSTAT == %08X\n", ctrl);

	return 0;
}

static int sdhci_return_tuning_status(struct mmc *mmc)
{

	struct sdhci_host *host = mmc->priv;
	u32 ctrl;

	ctrl = sdhci_readl(host, SDHCI_ACMD12_ERR);
	debug("--> Tuning returned as ... %08X\n", ctrl);

	if (ctrl & (1 << 22)) {
		return 2;
	} else {
		if (ctrl & (1 << 23)) {
			printf("%s: Tuning Successful for device %d\n",
			       __func__, mmc->block_dev.devnum);
			return 1;
		} else {
			printf("%s: Tuning Failed for device %d\n", __func__,
			       mmc->block_dev.devnum);
			return -1;
		}
	}
}

#if 0
static int sdhci_dump_regs(struct mmc *mmc)
{
	u32 read;

	debug("\nDumping SDIO0 Regs...\n");
	read = readl(SDIO_IDM0_IO_CONTROL_DIRECT);
	debug("Read SDIO_IDM0_IO_CONTROL_DIRECT (%08X) == %08X\n",
	      SDIO_IDM0_IO_CONTROL_DIRECT, read);
	read = readl(SDIO_IDM0_IDM_RESET_CONTROL);
	debug("Read SDIO_IDM0_IDM_RESET_CONTROL (%08X) == %08X\n",
	      SDIO_IDM0_IDM_RESET_CONTROL, read);
	read = readl(SDIO_IDM0_IDM_RESET_STATUS);
	debug("Read SDIO_IDM0_IDM_RESET_STATUS (%08X) == %08X\n",
	      SDIO_IDM0_IDM_RESET_STATUS, read);
	read = readl(SDIO_IDM0_IDM_INTERRUPT_STATUS);
	debug("Read SDIO_IDM0_IDM_INTERRUPT_STATUS (%08X) == %08X\n",
	      SDIO_IDM0_IDM_INTERRUPT_STATUS, read);

	read = readl(ICFG_SDIO_CONFIG_0);
	debug("Read ICFG_SDIO_CONFIG_0 (%08X) == %08X\n", ICFG_SDIO_CONFIG_0,
	      read);
	read = readl(ICFG_SDIO0_STRAPSTATUS_0);
	debug("Read ICFG_SDIO0_STRAPSTATUS_0 (%08X) == %08X\n",
	      ICFG_SDIO0_STRAPSTATUS_0, read);
	read = readl(ICFG_SDIO0_STRAPSTATUS_1);
	debug("Read ICFG_SDIO0_STRAPSTATUS_1 (%08X) == %08X\n",
	      ICFG_SDIO0_STRAPSTATUS_1, read);
	read = readl(ICFG_SDIO0_STRAPSTATUS_2);
	debug("Read ICFG_SDIO0_STRAPSTATUS_2 (%08X) == %08X\n",
	      ICFG_SDIO0_STRAPSTATUS_2, read);
	read = readl(ICFG_SDIO0_STRAPSTATUS_3);
	debug("Read ICFG_SDIO0_STRAPSTATUS_3 (%08X) == %08X\n",
	      ICFG_SDIO0_STRAPSTATUS_3, read);
	read = readl(ICFG_SDIO0_STRAPSTATUS_4);
	debug("Read ICFG_SDIO0_STRAPSTATUS_4 (%08X) == %08X\n",
	      ICFG_SDIO0_STRAPSTATUS_4, read);
	read = readl(ICFG_SDIO0_CAP0);
	debug("Read ICFG_SDIO0_CAP0 (%08X) == %08X\n", ICFG_SDIO0_CAP0, read);
	read = readl(ICFG_SDIO0_CAP1);
	debug("Read ICFG_SDIO0_CAP1 (%08X) == %08X\n", ICFG_SDIO0_CAP1, read);
	read = readl(ICFG_SDIO0_DPMEM_TM);
	debug("Read ICFG_SDIO0_DPMEM_TM (%08X) == %08X\n", ICFG_SDIO0_DPMEM_TM,
	      read);

	read = readl(SDIO0_eMMCSDXC_SYSADDR);
	debug("Read SDIO0_eMMCSDXC_SYSADDR (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_SYSADDR, read);
	read = readl(SDIO0_eMMCSDXC_BLOCK);
	debug("Read SDIO0_eMMCSDXC_BLOCK (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_BLOCK, read);
	read = readl(SDIO0_eMMCSDXC_ARG);
	debug("Read SDIO0_eMMCSDXC_ARG (%08X) == %08X\n", SDIO0_eMMCSDXC_ARG,
	      read);
	read = readl(SDIO0_eMMCSDXC_CMD);
	debug("Read SDIO0_eMMCSDXC_CMD (%08X) == %08X\n", SDIO0_eMMCSDXC_CMD,
	      read);
	read = readl(SDIO0_eMMCSDXC_RESP0);
	debug("Read SDIO0_eMMCSDXC_RESP0 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_RESP0, read);
	read = readl(SDIO0_eMMCSDXC_RESP2);
	debug("Read SDIO0_eMMCSDXC_RESP2 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_RESP2, read);
	read = readl(SDIO0_eMMCSDXC_RESP4);
	debug("Read SDIO0_eMMCSDXC_RESP4 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_RESP4, read);
	read = readl(SDIO0_eMMCSDXC_RESP6);
	debug("Read SDIO0_eMMCSDXC_RESP6 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_RESP6, read);
	read = readl(SDIO0_eMMCSDXC_BUFDAT);
	debug("Read SDIO0_eMMCSDXC_BUFDAT(%08X) == %08X\n",
	      SDIO0_eMMCSDXC_BUFDAT, read);
	read = readl(SDIO0_eMMCSDXC_PSTATE);
	debug("Read SDIO0_eMMCSDXC_PSTATE (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_PSTATE, read);
	read = readl(SDIO0_eMMCSDXC_CTRL);
	debug("Read SDIO0_eMMCSDXC_CTRL (%08X) == %08X\n", SDIO0_eMMCSDXC_CTRL,
	      read);
	read = readl(SDIO0_eMMCSDXC_CTRL1);
	debug("Read SDIO0_eMMCSDXC_CTRL1 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_CTRL1, read);
	read = readl(SDIO0_eMMCSDXC_INTR);
	debug("Read SDIO0_eMMCSDXC_INTR (%08X) == %08X\n", SDIO0_eMMCSDXC_INTR,
	      read);
	read = readl(SDIO0_eMMCSDXC_INTREN1);
	debug("Read SDIO0_eMMCSDXC_INTREN1 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_INTREN1, read);
	read = readl(SDIO0_eMMCSDXC_INTREN2);
	debug("Read SDIO0_eMMCSDXC_INTREN2 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_INTREN2, read);
	read = readl(SDIO0_eMMCSDXC_ERRSTAT);
	debug("Read SDIO0_eMMCSDXC_ERRSTAT (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_ERRSTAT, read);
	read = readl(SDIO0_eMMCSDXC_CAPABILITIES1);
	debug("Read SDIO0_eMMCSDXC_CAPABILITIES1 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_CAPABILITIES1, read);
	read = readl(SDIO0_eMMCSDXC_CAPABILITIES2);
	debug("Read SDIO0_eMMCSDXC_CAPABILITIES2 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_CAPABILITIES2, read);
	read = readl(SDIO0_eMMCSDXC_MAX_A1);
	debug("Read SDIO0_eMMCSDXC_MAX_A1 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_MAX_A1, read);
	read = readl(SDIO0_eMMCSDXC_MAX_A2);
	debug("Read SDIO0_eMMCSDXC_MAX_A2 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_MAX_A2, read);
	read = readl(SDIO0_eMMCSDXC_CMDENTSTAT);
	debug("Read SDIO0_eMMCSDXC_CMDENTSTAT (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_CMDENTSTAT, read);
	read = readl(SDIO0_eMMCSDXC_ADMAERR);
	debug("Read SDIO0_eMMCSDXC_ADMAERR (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_ADMAERR, read);
	read = readl(SDIO0_eMMCSDXC_ADMAADDR0);
	debug("Read SDIO0_eMMCSDXC_ADMAADDR0 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_ADMAADDR0, read);
	read = readl(SDIO0_eMMCSDXC_PRESETVAL1);
	debug("Read SDIO0_eMMCSDXC_PRESETVAL1 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_PRESETVAL1, read);
	read = readl(SDIO0_eMMCSDXC_PRESETVAL2);
	debug("Read SDIO0_eMMCSDXC_PRESETVAL2 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_PRESETVAL2, read);
	read = readl(SDIO0_eMMCSDXC_PRESETVAL3);
	debug("Read SDIO0_eMMCSDXC_PRESETVAL3 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_PRESETVAL3, read);
	read = readl(SDIO0_eMMCSDXC_PRESETVAL4);
	debug("Read SDIO0_eMMCSDXC_PRESETVAL4 (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_PRESETVAL4, read);
	read = readl(SDIO0_eMMCSDXC_BOOTTIMEOUT);
	debug("Read SDIO0_eMMCSDXC_BOOTTIMEOUT (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_BOOTTIMEOUT, read);
	read = readl(SDIO0_eMMCSDXC_DBGSEL);
	debug("Read SDIO0_eMMCSDXC_DBGSEL (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_DBGSEL, read);
	read = readl(SDIO0_eMMCSDXC_SBUSCTRL);
	debug("Read SDIO0_eMMCSDXC_SBUSCTRL (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_SBUSCTRL, read);
	read = readl(SDIO0_eMMCSDXC_SPI_INT);
	debug("Read SDIO0_eMMCSDXC_SPI_INT (%08X) == %08X\n",
	      SDIO0_eMMCSDXC_SPI_INT, read);
	read = readl(SDIO0_eMMCSDXC_HCVERSIRQ);
	debug("Read SDIO0_eMMCSDXC_HCVERSIRQ (%08X) == %08X\n\n",
	      SDIO0_eMMCSDXC_HCVERSIRQ, read);

	/* Do SDIO1 Now... */

	debug("\nDumping SDIO1 Regs...\n");
	read = readl(SDIO_IDM1_IO_CONTROL_DIRECT);
	debug("Read SDIO_IDM1_IO_CONTROL_DIRECT (%08X) == %08X\n",
	      SDIO_IDM1_IO_CONTROL_DIRECT, read);
	read = readl(SDIO_IDM1_IDM_RESET_CONTROL);
	debug("Read SDIO_IDM1_IDM_RESET_CONTROL (%08X) == %08X\n",
	      SDIO_IDM1_IDM_RESET_CONTROL, read);
	read = readl(SDIO_IDM1_IDM_RESET_STATUS);
	debug("Read SDIO_IDM1_IDM_RESET_STATUS (%08X) == %08X\n",
	      SDIO_IDM1_IDM_RESET_STATUS, read);
	read = readl(SDIO_IDM1_IDM_INTERRUPT_STATUS);
	debug("Read SDIO_IDM1_IDM_INTERRUPT_STATUS (%08X) == %08X\n",
	      SDIO_IDM1_IDM_INTERRUPT_STATUS, read);

	read = readl(ICFG_SDIO_CONFIG_1);
	debug("Read ICFG_SDIO_CONFIG_1 (%08X) == %08X\n", ICFG_SDIO_CONFIG_1,
	      read);
	read = readl(ICFG_SDIO1_STRAPSTATUS_0);
	debug("Read ICFG_SDIO1_STRAPSTATUS_0 (%08X) == %08X\n",
	      ICFG_SDIO1_STRAPSTATUS_0, read);
	read = readl(ICFG_SDIO1_STRAPSTATUS_1);
	debug("Read ICFG_SDIO1_STRAPSTATUS_1 (%08X) == %08X\n",
	      ICFG_SDIO1_STRAPSTATUS_1, read);
	read = readl(ICFG_SDIO1_STRAPSTATUS_2);
	debug("Read ICFG_SDIO1_STRAPSTATUS_2 (%08X) == %08X\n",
	      ICFG_SDIO1_STRAPSTATUS_2, read);
	read = readl(ICFG_SDIO1_STRAPSTATUS_3);
	debug("Read ICFG_SDIO1_STRAPSTATUS_3 (%08X) == %08X\n",
	      ICFG_SDIO1_STRAPSTATUS_3, read);
	read = readl(ICFG_SDIO1_STRAPSTATUS_4);
	debug("Read ICFG_SDIO1_STRAPSTATUS_4 (%08X) == %08X\n",
	      ICFG_SDIO1_STRAPSTATUS_4, read);
	read = readl(ICFG_SDIO1_CAP0);
	debug("Read ICFG_SDIO1_CAP0 (%08X) == %08X\n", ICFG_SDIO1_CAP0, read);
	read = readl(ICFG_SDIO1_CAP1);
	debug("Read ICFG_SDIO1_CAP1 (%08X) == %08X\n", ICFG_SDIO1_CAP1, read);
	read = readl(ICFG_SDIO1_DPMEM_TM);
	debug("Read ICFG_SDIO1_DPMEM_TM (%08X) == %08X\n", ICFG_SDIO1_DPMEM_TM,
	      read);

	read = readl(SDIO1_eMMCSDXC_SYSADDR);
	debug("Read SDIO1_eMMCSDXC_SYSADDR (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_SYSADDR, read);
	read = readl(SDIO1_eMMCSDXC_BLOCK);
	debug("Read SDIO1_eMMCSDXC_BLOCK (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_BLOCK, read);
	read = readl(SDIO1_eMMCSDXC_ARG);
	debug("Read SDIO1_eMMCSDXC_ARG (%08X) == %08X\n", SDIO1_eMMCSDXC_ARG,
	      read);
	read = readl(SDIO1_eMMCSDXC_CMD);
	debug("Read SDIO1_eMMCSDXC_CMD (%08X) == %08X\n", SDIO1_eMMCSDXC_CMD,
	      read);
	read = readl(SDIO1_eMMCSDXC_RESP0);
	debug("Read SDIO1_eMMCSDXC_RESP0 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_RESP0, read);
	read = readl(SDIO1_eMMCSDXC_RESP2);
	debug("Read SDIO1_eMMCSDXC_RESP2 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_RESP2, read);
	read = readl(SDIO1_eMMCSDXC_RESP4);
	debug("Read SDIO1_eMMCSDXC_RESP4 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_RESP4, read);
	read = readl(SDIO1_eMMCSDXC_RESP6);
	debug("Read SDIO1_eMMCSDXC_RESP6 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_RESP6, read);
	read = readl(SDIO1_eMMCSDXC_BUFDAT);
	debug("Read SDIO1_eMMCSDXC_BUFDAT (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_BUFDAT, read);
	read = readl(SDIO1_eMMCSDXC_PSTATE);
	debug("Read SDIO1_eMMCSDXC_PSTATE (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_PSTATE, read);
	read = readl(SDIO1_eMMCSDXC_CTRL);
	debug("Read SDIO1_eMMCSDXC_CTRL (%08X) == %08X\n", SDIO1_eMMCSDXC_CTRL,
	      read);
	read = readl(SDIO1_eMMCSDXC_CTRL1);
	debug("Read SDIO1_eMMCSDXC_CTRL1 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_CTRL1, read);
	read = readl(SDIO1_eMMCSDXC_INTR);
	debug("Read SDIO1_eMMCSDXC_INTR (%08X) == %08X\n", SDIO1_eMMCSDXC_INTR,
	      read);
	read = readl(SDIO1_eMMCSDXC_INTREN1);
	debug("Read SDIO1_eMMCSDXC_INTREN1 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_INTREN1, read);
	read = readl(SDIO1_eMMCSDXC_INTREN2);
	debug("Read SDIO1_eMMCSDXC_INTREN2 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_INTREN2, read);
	read = readl(SDIO1_eMMCSDXC_ERRSTAT);
	debug("Read SDIO1_eMMCSDXC_ERRSTAT (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_ERRSTAT, read);
	read = readl(SDIO1_eMMCSDXC_CAPABILITIES1);
	debug("Read SDIO1_eMMCSDXC_CAPABILITIES1 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_CAPABILITIES1, read);
	read = readl(SDIO1_eMMCSDXC_CAPABILITIES2);
	debug("Read SDIO1_eMMCSDXC_CAPABILITIES2 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_CAPABILITIES2, read);
	read = readl(SDIO1_eMMCSDXC_MAX_A1);
	debug("Read SDIO1_eMMCSDXC_MAX_A1 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_MAX_A1, read);
	read = readl(SDIO1_eMMCSDXC_MAX_A2);
	debug("Read SDIO1_eMMCSDXC_MAX_A2 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_MAX_A2, read);
	read = readl(SDIO1_eMMCSDXC_CMDENTSTAT);
	debug("Read SDIO1_eMMCSDXC_CMDENTSTAT (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_CMDENTSTAT, read);
	read = readl(SDIO1_eMMCSDXC_ADMAERR);
	debug("Read SDIO1_eMMCSDXC_ADMAERR (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_ADMAERR, read);
	read = readl(SDIO1_eMMCSDXC_ADMAADDR0);
	debug("Read SDIO1_eMMCSDXC_ADMAADDR0 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_ADMAADDR0, read);
	read = readl(SDIO1_eMMCSDXC_PRESETVAL1);
	debug("Read SDIO1_eMMCSDXC_PRESETVAL1 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_PRESETVAL1, read);
	read = readl(SDIO1_eMMCSDXC_PRESETVAL2);
	debug("Read SDIO1_eMMCSDXC_PRESETVAL2 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_PRESETVAL2, read);
	read = readl(SDIO1_eMMCSDXC_PRESETVAL3);
	debug("Read SDIO1_eMMCSDXC_PRESETVAL3 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_PRESETVAL3, read);
	read = readl(SDIO1_eMMCSDXC_PRESETVAL4);
	debug("Read SDIO1_eMMCSDXC_PRESETVAL4 (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_PRESETVAL4, read);
	read = readl(SDIO1_eMMCSDXC_BOOTTIMEOUT);
	debug("Read SDIO1_eMMCSDXC_BOOTTIMEOUT (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_BOOTTIMEOUT, read);
	read = readl(SDIO1_eMMCSDXC_DBGSEL);
	debug("Read SDIO1_eMMCSDXC_DBGSEL (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_DBGSEL, read);
	read = readl(SDIO1_eMMCSDXC_SBUSCTRL);
	debug("Read SDIO1_eMMCSDXC_SBUSCTRL (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_SBUSCTRL, read);
	read = readl(SDIO1_eMMCSDXC_SPI_INT);
	debug("Read SDIO1_eMMCSDXC_SPI_INT (%08X) == %08X\n",
	      SDIO1_eMMCSDXC_SPI_INT, read);
	read = readl(SDIO1_eMMCSDXC_HCVERSIRQ);
	debug("Read SDIO1_eMMCSDXC_HCVERSIRQ (%08X) == %08X\n\n",
	      SDIO1_eMMCSDXC_HCVERSIRQ, read);

	return 0;
}
#endif
int sdhci_sel_voltage(struct mmc *mmc, int sel)
{
	return 0;
}

#if 0
int sdhci_sel_voltage(struct mmc *mmc, int sel)
{
	u8 devaddr;
	u8 value;
	int ret;

	debug("#### %s:%s. Begin : %d...\n", __FILE__, __func__,
	      mmc->block_dev.dev);
	/*
	 * This is a hardcoded value for the Remote 8-bit I/O expander on
	 * the NS2 SVK.
	 */
	devaddr = SDHCI_PCF8574TS_U92;

	/*
	 * First call i2c_probe() to ensure that all relevant data-structures
	 * are setup.
	 */
	ret = i2c_probe(devaddr);
	if (ret) {
		printf
		    ("ERROR: Problem probing the PCF8574 Device at Addr:
		     0x%x!!!\n",
		     devaddr);
		return ret;
	}

	ret = i2c_recv_byte(devaddr, &value);
	debug("#### PCF8574 read : 0x%08x\n", value);
	if (ret) {
		printf
		    ("ERROR: Problem reading the PCF8574 Device at Addr:
		     0x%x!!!\n",
		     devaddr);
		return ret;
	}

	/* Undriving the port sets the voltage to 3.3V. */
	if (sel == SEL_VOLT_1V8) {
		if (mmc->block_dev.dev == 0)
			value |= (1 << 6);
		else
			value |= (1 << 7);
	} else {

		if (mmc->block_dev.dev == 0)
			value &= ~(1 << 6);
		else
			value &= ~(1 << 7);
	}

	debug("#### PCF8574 write: 0x%08x\n", value);
	ret = i2c_send_byte(devaddr, value);
	if (ret) {
		printf
		    ("ERROR: Problem writing to the PCF8574 Device at Addr:
		     0x%x!!!\n",
		     devaddr);
		return ret;
	}

	mdelay(3000);
	return 0;
}
#endif
#endif /* CONFIG_PEGASUS*/

static int sdhci_init(struct mmc *mmc)
{
	struct sdhci_host *host = mmc->priv;
#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
	/* set 3v3 on init */
	sdhci_sel_voltage(mmc, SEL_VOLT_3V3);
#endif
	if ((host->quirks & SDHCI_QUIRK_32BIT_DMA_ADDR) && !aligned_buffer) {
		aligned_buffer = memalign(8, 512 * 1024);
		if (!aligned_buffer) {
			printf("%s: Aligned buffer alloc failed!!!\n",
			       __func__);
			return -1;
		}
	}

	sdhci_set_power(host, fls(mmc->cfg->voltages) - 1);

	if (host->quirks & SDHCI_QUIRK_NO_CD) {
#if defined(CONFIG_PIC32_SDHCI)
		/* PIC32 SDHCI CD errata:
		 * - set CD_TEST and clear CD_TEST_INS bit
		 */
		sdhci_writeb(host, SDHCI_CTRL_CD_TEST, SDHCI_HOST_CONTROL);
#else
		unsigned int status;

		sdhci_writel(host, SDHCI_CTRL_CD_TEST_INS | SDHCI_CTRL_CD_TEST,
			     SDHCI_HOST_CONTROL);

		status = sdhci_readl(host, SDHCI_PRESENT_STATE);
		while ((!(status & SDHCI_CARD_PRESENT)) ||
		       (!(status & SDHCI_CARD_STATE_STABLE)) ||
		       (!(status & SDHCI_CARD_DETECT_PIN_LEVEL)))
			status = sdhci_readl(host, SDHCI_PRESENT_STATE);
#endif
	}

	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK,
		     SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, 0x0, SDHCI_SIGNAL_ENABLE);

	return 0;
}

static const struct mmc_ops sdhci_ops = {
	.send_cmd = sdhci_send_command,
	.set_ios = sdhci_set_ios,
	.init = sdhci_init,
#if defined(CONFIG_PEGASUS) || defined(CONFIG_NS2)
	.set_taps = sdhci_set_taps,
	.set_tuning = sdhci_set_tuning,
	.return_tuning_status = sdhci_return_tuning_status,
	/*.dump_regs = sdhci_dump_regs,*/
	.set_voltage = sdhci_sel_voltage,
#endif
};

int add_sdhci(struct sdhci_host *host, u32 max_clk, u32 min_clk)
{
	unsigned int caps;

	host->cfg.name = host->name;
	host->cfg.ops = &sdhci_ops;

	caps = sdhci_readl(host, SDHCI_CAPABILITIES);
#ifdef CONFIG_MMC_SDMA
	if (!(caps & SDHCI_CAN_DO_SDMA)) {
		printf("%s: Your controller doesn't support SDMA!!\n",
		       __func__);
		return -1;
	}
#endif

	if (max_clk)
		host->cfg.f_max = max_clk;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->cfg.f_max = (caps & SDHCI_CLOCK_V3_BASE_MASK)
			    >> SDHCI_CLOCK_BASE_SHIFT;
		else
			host->cfg.f_max = (caps & SDHCI_CLOCK_BASE_MASK)
			    >> SDHCI_CLOCK_BASE_SHIFT;
		host->cfg.f_max *= 1000000;
	}
	if (host->cfg.f_max == 0) {
		printf("%s: Hardware doesn't specify base clock frequency\n",
		       __func__);
		return -1;
	}
	if (min_clk)
		host->cfg.f_min = min_clk;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->cfg.f_min = host->cfg.f_max /
			    SDHCI_MAX_DIV_SPEC_300;
		else
			host->cfg.f_min = host->cfg.f_max /
			    SDHCI_MAX_DIV_SPEC_200;
	}

	host->cfg.voltages = 0;
	if (caps & SDHCI_CAN_VDD_330)
		host->cfg.voltages |= MMC_VDD_32_33 | MMC_VDD_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		host->cfg.voltages |= MMC_VDD_29_30 | MMC_VDD_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		host->cfg.voltages |= MMC_VDD_165_195;

	if (host->quirks & SDHCI_QUIRK_BROKEN_VOLTAGE)
		host->cfg.voltages |= host->voltages;

	host->cfg.host_caps = MMC_MODE_HS | MMC_MODE_HS_52MHz | MMC_MODE_4BIT;
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		if (caps & SDHCI_CAN_DO_8BIT)
			host->cfg.host_caps |= MMC_MODE_8BIT;
	}

	if (host->quirks & SDHCI_QUIRK_NO_HISPD_BIT)
		host->cfg.host_caps &= ~(MMC_MODE_HS | MMC_MODE_HS_52MHz);

	if (host->host_caps)
		host->cfg.host_caps |= host->host_caps;

	host->cfg.b_max = CONFIG_SYS_MMC_MAX_BLK_COUNT;

	sdhci_reset(host, SDHCI_RESET_ALL);

	host->mmc = mmc_create(&host->cfg, host);
	if (host->mmc == NULL) {
		printf("%s: mmc create fail!\n", __func__);
		return -1;
	}

	return 0;
}

void chal_sd_dump_reg(struct sdhci_host *host)
{
#if 0
	uint32_t i = 0;
	for (i = 0; i < 0x80; i = i + 4) {
		if (i % 16 == 0)
			printf("\n");
		printf("%x ", sdhci_readl(host, i));
	}
#endif
}
