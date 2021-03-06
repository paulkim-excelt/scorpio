/*
 * Copyright (C) 2014-2015 Broadcom Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dai.h>

#include "cygnus-ssp.h"
#include "iproc-pcm.h"

#define CAPTURE_FCI_ID_BASE 0x180
#define CYGNUS_SSP_TRISTATE_MASK 0x001fff
#define CYGNUS_PLLCLKSEL_MASK 0xf

/* Used with stream_on field to indicate which streams are active */
#define  PLAYBACK_STREAM_MASK   BIT(0)
#define  CAPTURE_STREAM_MASK    BIT(1)

#define CH_GRP_STEREO            0x1

/* Identifies which io map to the reg offsets is to be used with */
#define MAP1_FLAG  0x10000
#define MAP2_FLAG  0x20000
#define MAP3_FLAG  0x30000

/* Begin register offset defines */
#define IOMAP1_BASE_OFFSET      0x000

#define AUD_MISC_SEROUT_OE_REG_BASE     (0x01c | MAP1_FLAG)
#define AUD_MISC_SEROUT_SPDIF_OE  12
#define AUD_MISC_SEROUT_MCLK_OE    3
#define AUD_MISC_SEROUT_LRCK_OE    2
#define AUD_MISC_SEROUT_SCLK_OE    1
#define AUD_MISC_SEROUT_SDAT_OE    0

/* AUD_FMM_BF_CTRL_xxx regs */
#define BF_DST_CFG0_OFFSET              (0x100 | MAP1_FLAG)
#define BF_DST_CFG1_OFFSET              (0x104 | MAP1_FLAG)
#define BF_DST_CFG2_OFFSET              (0x108 | MAP1_FLAG)

#define BF_DST_CTRL0_OFFSET             (0x130 | MAP1_FLAG)
#define BF_DST_CTRL1_OFFSET             (0x134 | MAP1_FLAG)
#define BF_DST_CTRL2_OFFSET             (0x138 | MAP1_FLAG)

#define BF_SRC_CFG0_OFFSET              (0x148 | MAP1_FLAG)
#define BF_SRC_CFG1_OFFSET              (0x14c | MAP1_FLAG)
#define BF_SRC_CFG2_OFFSET              (0x150 | MAP1_FLAG)
#define BF_SRC_CFG3_OFFSET              (0x154 | MAP1_FLAG)

#define BF_SRC_CTRL0_OFFSET             (0x1c0 | MAP1_FLAG)
#define BF_SRC_CTRL1_OFFSET             (0x1c4 | MAP1_FLAG)
#define BF_SRC_CTRL2_OFFSET             (0x1c8 | MAP1_FLAG)
#define BF_SRC_CTRL3_OFFSET             (0x1cc | MAP1_FLAG)

#define BF_SRC_GRP0_OFFSET              (0x1fc | MAP1_FLAG)
#define BF_SRC_GRP1_OFFSET              (0x200 | MAP1_FLAG)
#define BF_SRC_GRP2_OFFSET              (0x204 | MAP1_FLAG)
#define BF_SRC_GRP3_OFFSET              (0x208 | MAP1_FLAG)

#define BF_SRC_GRP_EN_OFFSET            (0x320 | MAP1_FLAG)
#define BF_SRC_GRP_FLOWON_OFFSET        (0x324 | MAP1_FLAG)
#define BF_SRC_GRP_SYNC_DIS_OFFSET      (0x328 | MAP1_FLAG)

/* AUD_FMM_IOP_OUT_I2S_xxx regs */
#define OUT_I2S_0_STREAM_CFG_OFFSET     (0xa00 | MAP1_FLAG)
#define OUT_I2S_0_CFG_OFFSET            (0xa04 | MAP1_FLAG)
#define OUT_I2S_0_MCLK_CFG_OFFSET       (0xa0c | MAP1_FLAG)

#define OUT_I2S_1_STREAM_CFG_OFFSET     (0xa40 | MAP1_FLAG)
#define OUT_I2S_1_CFG_OFFSET            (0xa44 | MAP1_FLAG)
#define OUT_I2S_1_MCLK_CFG_OFFSET       (0xa4c | MAP1_FLAG)

#define OUT_I2S_2_STREAM_CFG_OFFSET     (0xa80 | MAP1_FLAG)
#define OUT_I2S_2_CFG_OFFSET            (0xa84 | MAP1_FLAG)
#define OUT_I2S_2_MCLK_CFG_OFFSET       (0xa8c | MAP1_FLAG)

/* AUD_FMM_IOP_OUT_SPDIF_xxx regs */
#define SPDIF_STREAM_CFG_OFFSET         (0xac0 | MAP1_FLAG)
#define SPDIF_CTRL_OFFSET               (0xac4 | MAP1_FLAG)
#define SPDIF_FORMAT_CFG_OFFSET         (0xad8 | MAP1_FLAG)
#define SPDIF_MCLK_CFG_OFFSET           (0xadc | MAP1_FLAG)

/*--------------------------------------------
 * Register offsets for i2s_in io space
 */
/* AUD_FMM_IOP_IN_I2S_xxx regs */
#define IOMAP3_BASE_OFFSET      0xc00

#define IN_I2S_0_STREAM_CFG_OFFSET      (0xc00 | MAP3_FLAG)
#define IN_I2S_0_CFG_OFFSET             (0xc04 | MAP3_FLAG)
#define IN_I2S_1_STREAM_CFG_OFFSET      (0xc40 | MAP3_FLAG)
#define IN_I2S_1_CFG_OFFSET             (0xc44 | MAP3_FLAG)
#define IN_I2S_2_STREAM_CFG_OFFSET      (0xc80 | MAP3_FLAG)
#define IN_I2S_2_CFG_OFFSET             (0xc84 | MAP3_FLAG)

/* AUD_FMM_IOP_MISC_xxx regs */
#define IOP_SW_INIT_LOGIC               (0xdc0 | MAP3_FLAG)

/* End register offset defines */


/* AUD_FMM_IOP_OUT_I2S_x_MCLK_CFG_0_REG */
#define I2S_OUT_MCLKRATE_SHIFT 16

/* AUD_FMM_IOP_OUT_I2S_x_MCLK_CFG_REG */
#define I2S_OUT_PLLCLKSEL_SHIFT  0

/* AUD_FMM_IOP_OUT_I2S_x_STREAM_CFG */
#define I2S_OUT_STREAM_CFG_ENA           31
#define I2S_OUT_STREAM_CFG_CH_GROUP      24
#define I2S_OUT_STREAM_CFG_CH_GROUP_MASK \
	GENMASK(I2S_OUT_STREAM_CFG_CH_GROUP + 3, I2S_OUT_STREAM_CFG_CH_GROUP)
#define I2S_OUT_STREAM_CFG_GROUP_ID      20
#define I2S_OUT_STREAM_CFG_GROUP_ID_MASK \
	GENMASK(I2S_OUT_STREAM_CFG_GROUP_ID + 3, I2S_OUT_STREAM_CFG_GROUP_ID)
#define I2S_OUT_STREAM_CFG_FCI_ID         0
#define I2S_OUT_STREAM_CFG_FCI_ID_MASK \
	GENMASK(I2S_OUT_STREAM_CFG_FCI_ID + 9, I2S_OUT_STREAM_CFG_FCI_ID)

/* AUD_FMM_IOP_IN_I2S_x_CAP */
#define I2S_IN_STREAM_CFG_CAP_ENA   31
#define I2S_IN_STREAM_CFG_GROUP_ID   4
#define I2S_IN_STREAM_CFG_GROUP_ID_MASK  \
	GENMASK(I2S_IN_STREAM_CFG_GROUP_ID + 3, I2S_IN_STREAM_CFG_GROUP_ID)

/* AUD_FMM_IOP_OUT_I2S_x_I2S_CFG_REG */
#define I2S_OUT_CFGX_CLK_ENA         0
#define I2S_OUT_CFGX_DATA_ENABLE     1
#define I2S_OUT_CFGX_LRCK_POLARITY   4
#define I2S_OUT_CFGX_SCLK_POLARITY   5
#define I2S_OUT_CFGX_DATA_ALIGNMENT  6
#define I2S_OUT_CFGX_BITS_PER_SAMPLE 8
#define I2S_OUT_CFGX_BIT_PER_SAMPLE_MASK \
	GENMASK(I2S_OUT_CFGX_BITS_PER_SAMPLE + 4, I2S_OUT_CFGX_BITS_PER_SAMPLE)
#define I2S_OUT_CFGX_BITS_PER_SLOT  13
#define I2S_OUT_CFGX_VALID_SLOT     14
#define I2S_OUT_CFGX_VALID_SLOT_MASK \
	GENMASK(I2S_OUT_CFGX_VALID_SLOT + 3, I2S_OUT_CFGX_VALID_SLOT)
#define I2S_OUT_CFGX_FSYNC_WIDTH    18
#define I2S_OUT_CFGX_FSYNC_WIDTH_MASK \
	GENMASK(I2S_OUT_CFGX_FSYNC_WIDTH + 7, I2S_OUT_CFGX_FSYNC_WIDTH)
#define I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32 26
#define I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32_MASK \
	GENMASK(I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32 + 3, \
		I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32)
#define I2S_OUT_CFGX_SLAVE_MODE     30
#define I2S_OUT_CFGX_TDM_MODE       31

#define I2S_IN_CFGX_DATA_ALIGNMENT   6
#define I2S_IN_CFGX_BITS_PER_SAMPLE  8
#define I2S_IN_CFGX_BIT_PER_SAMPLE_MASK \
	GENMASK(I2S_IN_CFGX_BITS_PER_SAMPLE + 4, I2S_IN_CFGX_BITS_PER_SAMPLE)
#define I2S_IN_CFGX_BITS_PER_SLOT   13
#define I2S_IN_CFGX_VALID_SLOT      14
#define I2S_IN_CFGX_SLAVE_MODE      30
#define I2S_IN_CFGX_TDM_MODE        31

/* AUD_FMM_BF_CTRL_SOURCECH_CFGx_REG */
#define BF_SRC_CFGX_SFIFO_ENA              0
#define BF_SRC_CFGX_BUFFER_PAIR_ENABLE     1
#define BF_SRC_CFGX_SAMPLE_CH_MODE         2
#define BF_SRC_CFGX_SFIFO_SZ_DOUBLE        5
#define BF_SRC_CFGX_NOT_PAUSE_WHEN_EMPTY  10
#define BF_SRC_CFGX_SAMPLE_REPEAT_ENABLE  11
#define BF_SRC_CFGX_BIT_RES               20
#define BF_SRC_CFGX_PROCESS_SEQ_ID_VALID  31
#define BF_SRC_CFGX_BITRES_MASK           0x1f

/* AUD_FMM_BF_CTRL_SOURCECH_CTRLx_REG */
#define BF_SOURCECH_CTRL_PLAY_RUN   0

/* AUD_FMM_BF_CTRL_SOURCECH_GRPX */
#define BF_SRC_GRPX_GROUP_ID 0
#define BF_SRC_GRPX_GROUP_ID_MASK \
	GENMASK(BF_SRC_GRPX_GROUP_ID + 2, BF_SRC_GRPX_GROUP_ID)

/* AUD_FMM_BF_CTRL_DESTCH_CFGx_REG */
#define BF_DST_CFGX_CAP_ENA              0
#define BF_DST_CFGX_BUFFER_PAIR_ENABLE   1
#define BF_DST_CFGX_DFIFO_SZ_DOUBLE      2
#define BF_DST_CFGX_NOT_PAUSE_WHEN_FULL 11
#define BF_DST_CFGX_FCI_ID              12
#define BF_DST_CFGX_CAP_MODE            24
#define BF_DST_CFGX_PROC_SEQ_ID_VALID   31
#define BF_DST_CFGX_BITRES_MASK         0x1f

/* AUD_FMM_BF_CTRL_DESTCH_CTRLX */
#define BF_DESTCH_CTRLX_CAP_RUN  0x1

/* AUD_FMM_IOP_OUT_SPDIF_STREAM_CFG */
#define SPDIF_STREAM_CFG_ENA            31
#define SPDIF_STREAM_CFG_FCI_ID_MASK    GENMASK(9, 0)

/* AUD_FMM_IOP_OUT_SPDIF_CTRL */
#define SPDIF_CTRL_DITHER_ENA     3

#define IOP_LOGIC_RESET_IN_OFFSET(x) ((x) + 7) /* Capture ports offset by 7 */

#define INIT_SSP_REGS(num) (struct cygnus_ssp_regs){ \
		.i2s_stream_cfg = OUT_I2S_ ##num## _STREAM_CFG_OFFSET, \
		.i2s_cap_stream_cfg = IN_I2S_ ##num## _STREAM_CFG_OFFSET, \
		.i2s_cfg = OUT_I2S_ ##num## _CFG_OFFSET, \
		.i2s_cap_cfg = IN_I2S_ ##num## _CFG_OFFSET, \
		.i2s_mclk_cfg = OUT_I2S_ ##num## _MCLK_CFG_OFFSET, \
		.bf_destch_ctrl = BF_DST_CTRL ##num## _OFFSET, \
		.bf_destch_cfg = BF_DST_CFG ##num## _OFFSET, \
		.bf_sourcech_ctrl = BF_SRC_CTRL ##num## _OFFSET, \
		.bf_sourcech_cfg = BF_SRC_CFG ##num## _OFFSET \
}

#define CYGNUS_RATE_MIN     8000
#define CYGNUS_RATE_MAX   384000

/* List of valid frame sizes for tdm mode */
static const int ssp_valid_tdm_framesize[] = {32, 64, 128, 256, 512};

static const unsigned int cygnus_rates[] = {
	 8000, 11025,  16000,  22050,  32000,  44100, 48000,
	88200, 96000, 176400, 192000, 352800, 384000
};

static const struct snd_pcm_hw_constraint_list cygnus_rate_constraint = {
	.count = ARRAY_SIZE(cygnus_rates),
	.list = cygnus_rates,
};

static void update_ssp_cfg(struct cygnus_aio_port *aio);


struct reg_desc {
	struct regmap *iomap;
	unsigned int io_offset;
};

static int audio_get_iomap(struct audio_io *io,
			    unsigned int reg_code, struct reg_desc *desc)
{
	unsigned int  map_idx;
	unsigned int  full_offset;

	map_idx     = reg_code & 0xFFFF0000;
	full_offset = reg_code & 0x0000FFFF;

	switch (map_idx) {
	case MAP1_FLAG:
		desc->iomap = io->audio;
		desc->io_offset = full_offset - IOMAP1_BASE_OFFSET;
		break;
	case MAP3_FLAG:
		desc->iomap = io->i2s_in;
		desc->io_offset = full_offset - IOMAP3_BASE_OFFSET;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int sspreg_update(struct audio_io *io, unsigned int offset,
			 u32 mask, u32 new_val)
{
	struct reg_desc desc;
	int ret;

	ret = audio_get_iomap(io, offset, &desc);
	if (ret)
		return ret;

	regmap_update_bits(desc.iomap, desc.io_offset, mask, new_val);

	return 0;
}

/* sspreg_clr_bits will set all the bits specified by "mask" to 0 */
static int sspreg_clr_bits(struct audio_io *io, unsigned int offset, u32 mask)
{
	return sspreg_update(io, offset, mask, 0);
}


/* sspreg_set_bits will set all the bits specified by "mask" to 1 */
static int sspreg_set_bits(struct audio_io *io, unsigned int offset, u32 mask)
{
	return sspreg_update(io, offset, mask, mask);
}

static struct cygnus_aio_port *cygnus_dai_get_portinfo(struct snd_soc_dai *dai)
{
	struct cygnus_audio *cygaud = snd_soc_dai_get_drvdata(dai);

	return &cygaud->portinfo[dai->id];
}

static int audio_ssp_init_portregs(struct cygnus_aio_port *aio)
{
	u32 val, fci_id, mask;
	int status = 0;

	/* Set Group ID */
	mask = BF_SRC_GRPX_GROUP_ID_MASK;
	sspreg_update(aio->io, BF_SRC_GRP0_OFFSET, mask, 0);
	sspreg_update(aio->io, BF_SRC_GRP1_OFFSET, mask, 1);
	sspreg_update(aio->io, BF_SRC_GRP2_OFFSET, mask, 2);
	sspreg_update(aio->io, BF_SRC_GRP3_OFFSET, mask, 3);

	switch (aio->port_type) {
	case PORT_TDM:
		/* Configure the AUD_FMM_IOP_OUT_I2S_x_STREAM_CFG reg */
		mask = I2S_OUT_STREAM_CFG_GROUP_ID_MASK;
		mask |= I2S_OUT_STREAM_CFG_FCI_ID_MASK;
		mask |= I2S_OUT_STREAM_CFG_CH_GROUP_MASK;
		val = aio->portnum << I2S_OUT_STREAM_CFG_GROUP_ID;
		/* FCI ID is the port num */
		val |= (aio->portnum << I2S_OUT_STREAM_CFG_FCI_ID);
		val |= (CH_GRP_STEREO << I2S_OUT_STREAM_CFG_CH_GROUP);
		sspreg_update(aio->io, aio->regs.i2s_stream_cfg, mask, val);

		/* Configure the AUD_FMM_BF_CTRL_SOURCECH_CFGX reg */
		mask = BIT(BF_SRC_CFGX_NOT_PAUSE_WHEN_EMPTY);
		mask |=	BIT(BF_SRC_CFGX_SAMPLE_REPEAT_ENABLE);
		mask |= BIT(BF_SRC_CFGX_SFIFO_SZ_DOUBLE);
		mask |= BIT(BF_SRC_CFGX_PROCESS_SEQ_ID_VALID);
		val = BIT(BF_SRC_CFGX_SFIFO_SZ_DOUBLE);
		val |= BIT(BF_SRC_CFGX_PROCESS_SEQ_ID_VALID);
		sspreg_update(aio->io, aio->regs.bf_sourcech_cfg, mask, val);

		/* Configure the AUD_FMM_IOP_IN_I2S_x_CAP_STREAM_CFG_0 reg */
		mask = I2S_IN_STREAM_CFG_GROUP_ID_MASK;
		val = aio->portnum << I2S_IN_STREAM_CFG_GROUP_ID;
		sspreg_update(aio->io, aio->regs.i2s_cap_stream_cfg, mask, val);

		/* Configure the AUD_FMM_BF_CTRL_DESTCH_CFGX_REG_BASE reg */
		mask = (0xf << BF_DST_CFGX_FCI_ID);
		mask |= BIT(BF_DST_CFGX_DFIFO_SZ_DOUBLE);
		mask |= BIT(BF_DST_CFGX_NOT_PAUSE_WHEN_FULL);
		mask |= BIT(BF_DST_CFGX_PROC_SEQ_ID_VALID);

		fci_id = CAPTURE_FCI_ID_BASE + aio->portnum;
		val = BIT(BF_DST_CFGX_DFIFO_SZ_DOUBLE);
		val |= (fci_id << BF_DST_CFGX_FCI_ID);
		val |= BIT(BF_DST_CFGX_PROC_SEQ_ID_VALID);
		sspreg_update(aio->io, aio->regs.bf_destch_cfg, mask, val);

		/* Enable the transmit pin for this port, logic low */
		mask = BIT((aio->portnum * 4) + AUD_MISC_SEROUT_SDAT_OE);
		sspreg_clr_bits(aio->io, AUD_MISC_SEROUT_OE_REG_BASE, mask);
		break;
	case PORT_SPDIF:
		mask = BIT(SPDIF_CTRL_DITHER_ENA);
		sspreg_set_bits(aio->io, SPDIF_CTRL_OFFSET, mask);

		/* Enable and set the FCI ID for the SPDIF channel */
		mask = SPDIF_STREAM_CFG_FCI_ID_MASK;
		mask |= BIT(SPDIF_STREAM_CFG_ENA);
		val = aio->portnum; /* FCI ID is the port num */
		val |= BIT(SPDIF_STREAM_CFG_ENA);
		sspreg_update(aio->io, SPDIF_STREAM_CFG_OFFSET, mask, val);

		mask = BIT(BF_SRC_CFGX_NOT_PAUSE_WHEN_EMPTY);
		mask |= BIT(BF_SRC_CFGX_SFIFO_SZ_DOUBLE);
		mask |= BIT(BF_SRC_CFGX_PROCESS_SEQ_ID_VALID);
		val = BIT(BF_SRC_CFGX_SFIFO_SZ_DOUBLE) |
			BIT(BF_SRC_CFGX_PROCESS_SEQ_ID_VALID);
		sspreg_update(aio->io, aio->regs.bf_sourcech_cfg, mask, val);

		/* Enable the spdif output pin */
		mask = BIT(AUD_MISC_SEROUT_SPDIF_OE);
		sspreg_clr_bits(aio->io, AUD_MISC_SEROUT_OE_REG_BASE, mask);
		break;
	default:
		dev_err(aio->dev, "Port not supported\n");
		status = -EINVAL;
	}

	return status;
}

static void audio_ssp_in_enable(struct cygnus_aio_port *aio)
{
	u32 mask;

	mask = BIT(BF_DST_CFGX_CAP_ENA);
	sspreg_set_bits(aio->io, aio->regs.bf_destch_cfg, mask);

	/*
	 * DATA_ENABLE need to be set even if doing capture.
	 * Subsequent Tx will fail if this is not done.
	 */
	if (!aio->streams_on) {
		mask = BIT(I2S_OUT_CFGX_CLK_ENA);
		mask |= BIT(I2S_OUT_CFGX_DATA_ENABLE);
		sspreg_set_bits(aio->io, aio->regs.i2s_cfg, mask);
	}

	mask = BIT(I2S_IN_STREAM_CFG_CAP_ENA);
	sspreg_set_bits(aio->io, aio->regs.i2s_cap_stream_cfg, mask);

	/* Enable input portion of block */
	udelay(10);

	/*
	 * The input port may or may not be held in reset. Always clear
	 * the reset. This will be benign if the port is not in reset.
	 */
	mask = BIT(IOP_LOGIC_RESET_IN_OFFSET(aio->portnum));
	sspreg_clr_bits(aio->io, IOP_SW_INIT_LOGIC, mask);

	mask = BF_DESTCH_CTRLX_CAP_RUN;
	sspreg_set_bits(aio->io, aio->regs.bf_destch_ctrl, mask);

	aio->streams_on |= CAPTURE_STREAM_MASK;
}

static void audio_ssp_in_disable(struct cygnus_aio_port *aio)
{
	u32 mask;

	mask = BIT(BF_DST_CFGX_CAP_ENA);
	sspreg_clr_bits(aio->io, aio->regs.bf_destch_cfg, mask);

	mask = BIT(I2S_IN_STREAM_CFG_CAP_ENA);
	sspreg_clr_bits(aio->io, aio->regs.i2s_cap_stream_cfg, mask);

	aio->streams_on &= ~CAPTURE_STREAM_MASK;

	mask = BF_DESTCH_CTRLX_CAP_RUN;
	sspreg_clr_bits(aio->io, aio->regs.bf_destch_ctrl, mask);

	/*
	 * Put input portion of port in reset.
	 * Clears residual data (32 bits) from internal formatter buffer
	 * BIT_CLOCK must be present for this to take effect.  For Cygnus
	 * in slave mode this means we must master after Cygnus port
	 */
	/*
	 * TDM Slave Rx needs to toggle this reset.
	 * See comment in cygnus_ssp_hw_params() about JIRA-1312.
	 * TDM Master Rx also needs this fix
	 *   32 bit transfers of fully populated TDM frames will have every
	 *   transfer after the first with misaligned channels.
	 *
	 */
	if (aio->mode == CYGNUS_SSPMODE_TDM) {
		mask = BIT(IOP_LOGIC_RESET_IN_OFFSET(aio->portnum));
		sspreg_set_bits(aio->io, IOP_SW_INIT_LOGIC, mask);
	}

	/* If both playback and capture are off */
	if (!aio->streams_on) {
		/*
		 * Add small delay before turning off clock
		 * Need 1 bit clock tick for INIT_LOGIC to activate
		 */
		udelay(10);
		mask = BIT(I2S_OUT_CFGX_CLK_ENA);
		mask |= BIT(I2S_OUT_CFGX_DATA_ENABLE);
		sspreg_clr_bits(aio->io, aio->regs.i2s_cfg, mask);
	}
}

static int audio_ssp_out_enable(struct cygnus_aio_port *aio)
{
	u32 value, mask;
	int status = 0;

	switch (aio->port_type) {
	case PORT_TDM:
		mask = I2S_OUT_STREAM_CFG_FCI_ID_MASK;
		value = aio->portnum;
		sspreg_update(aio->io, aio->regs.i2s_stream_cfg, mask, value);

		mask = BIT(BF_SRC_CFGX_SFIFO_ENA);
		sspreg_set_bits(aio->io, aio->regs.bf_sourcech_cfg, mask);

		mask = BIT(BF_SOURCECH_CTRL_PLAY_RUN);
		sspreg_set_bits(aio->io, aio->regs.bf_sourcech_ctrl, mask);

		mask = BIT(I2S_OUT_STREAM_CFG_ENA);
		sspreg_set_bits(aio->io, aio->regs.i2s_stream_cfg, mask);

		mask = BIT(I2S_OUT_CFGX_CLK_ENA) |
			BIT(I2S_OUT_CFGX_DATA_ENABLE);
		sspreg_set_bits(aio->io, aio->regs.i2s_cfg, mask);

		/*
		 * The output port may or may not be in reset. Always clear
		 * the reset. This will be benign if the port is not in reset.
		 */
		mask = BIT(aio->portnum);
		sspreg_clr_bits(aio->io, IOP_SW_INIT_LOGIC, mask);

		aio->streams_on |= PLAYBACK_STREAM_MASK;
		break;
	case PORT_SPDIF:
		mask = 0x3;
		sspreg_set_bits(aio->io, SPDIF_FORMAT_CFG_OFFSET, mask);

		mask = BIT(BF_SOURCECH_CTRL_PLAY_RUN);
		sspreg_set_bits(aio->io, aio->regs.bf_sourcech_ctrl, mask);

		mask = BIT(BF_SRC_CFGX_SFIFO_ENA);
		sspreg_set_bits(aio->io, aio->regs.bf_sourcech_cfg, mask);
		break;
	default:
		dev_err(aio->dev, "Port not supported %d\n", aio->portnum);
		status = -EINVAL;
	}

	return status;
}

static int audio_ssp_out_disable(struct cygnus_aio_port *aio)
{
	u32 mask, val;
	int status = 0;

	switch (aio->port_type) {
	case PORT_TDM:
		aio->streams_on &= ~PLAYBACK_STREAM_MASK;

		/* Set the FCI ID to INVALID */
		mask = I2S_OUT_STREAM_CFG_FCI_ID_MASK;
		val = 0x3ff << I2S_OUT_STREAM_CFG_FCI_ID;
		sspreg_update(aio->io, aio->regs.i2s_stream_cfg, mask, val);

		/*
		 * We want to wait enough time for 2 LRCLK.
		 * At 8 kHz framerate, this would be 250 us.
		 * Set delay to 300 us to be safe.
		 */
		udelay(300);

		/* set group_sync_dis = 1 */
		mask = BIT(aio->portnum);
		sspreg_set_bits(aio->io, BF_SRC_GRP_SYNC_DIS_OFFSET, mask);

		mask = BIT(BF_SOURCECH_CTRL_PLAY_RUN);
		sspreg_clr_bits(aio->io, aio->regs.bf_sourcech_ctrl, mask);

		mask = BIT(BF_SRC_CFGX_SFIFO_ENA);
		sspreg_clr_bits(aio->io, aio->regs.bf_sourcech_cfg, mask);

		/* set group_sync_dis = 0 */
		mask = BIT(aio->portnum);
		sspreg_clr_bits(aio->io, BF_SRC_GRP_SYNC_DIS_OFFSET, mask);

		/*
		 * We want to wait enough time for 1 LRCLK.
		 * At 8 kHz framerate, this would be 125 us.
		 * Set delay to 175 us to be safe.
		 */
		udelay(175);

		if (aio->is_slave && (aio->mode == CYGNUS_SSPMODE_TDM)) {
			mask = BIT(aio->portnum);
			sspreg_set_bits(aio->io, IOP_SW_INIT_LOGIC, mask);
		}

		/* If both playback and capture are off */
		if (aio->streams_on == 0) {
			mask = BIT(I2S_OUT_CFGX_DATA_ENABLE);
			mask |= BIT(I2S_OUT_CFGX_CLK_ENA);
			sspreg_clr_bits(aio->io, aio->regs.i2s_cfg, mask);
		}
		break;
	case PORT_SPDIF:
		mask = 0x3;
		sspreg_clr_bits(aio->io, SPDIF_FORMAT_CFG_OFFSET, mask);

		mask = BIT(BF_SOURCECH_CTRL_PLAY_RUN);
		sspreg_clr_bits(aio->io, aio->regs.bf_sourcech_ctrl, mask);

		mask = BIT(BF_SRC_CFGX_SFIFO_ENA);
		sspreg_clr_bits(aio->io, aio->regs.bf_sourcech_cfg, mask);
		break;
	default:
		dev_err(aio->dev, "Port not supported %d\n", aio->portnum);
		status = -EINVAL;
	}

	return status;
}

static int cygnus_ssp_set_clocks(struct cygnus_aio_port *aio)
{
	u32 value;
	u32 mask = 0xf;
	u32 sclk;
	u32 mclk_rate;
	unsigned int bits_per_frame;
	unsigned int bit_rate;
	unsigned int ratio;

	bits_per_frame = aio->slots_per_frame * aio->slot_width;
	bit_rate = bits_per_frame * aio->lrclk;

	/*
	 * Check if the bit clock can be generated from the given MCLK.
	 * MCLK must be a perfect multiple of bit clock and must be one of the
	 * following values... (2,4,6,8,10,12,14)
	 */
	if ((aio->mclk % bit_rate) != 0)
		return -EINVAL;

	ratio = aio->mclk / bit_rate;
	switch (ratio) {
	case 2:
	case 4:
	case 6:
	case 8:
	case 10:
	case 12:
	case 14:
		mclk_rate = ratio / 2;
		break;

	default:
		dev_err(aio->dev, "Invalid combination of MCLK and BCLK\n");
		dev_err(aio->dev, "lrclk = %u, bits/frame = %u, mclk = %u\n",
			aio->lrclk, bits_per_frame, aio->mclk);
		return -EINVAL;
	}

	/* Set sclk rate */
	switch (aio->port_type) {
	case PORT_TDM:
		sclk = bits_per_frame;
		if (sclk == 512)
			sclk = 0;

		/* sclks_per_1fs_div = sclk cycles/32 */
		sclk /= 32;

		/* Set number of bitclks per frame */
		mask = I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32_MASK;
		value = sclk << I2S_OUT_CFGX_SCLKS_PER_1FS_DIV32;
		sspreg_update(aio->io, aio->regs.i2s_cfg, mask, value);
		dev_dbg(aio->dev, "SCLKS_PER_1FS_DIV32 = 0x%x\n", value);
		break;
	case PORT_SPDIF:
		break;
	default:
		dev_err(aio->dev, "Unknown port type\n");
		return -EINVAL;
	}

	/* Set MCLK_RATE ssp port (spdif and ssp are the same) */
	mask = (0xf << I2S_OUT_MCLKRATE_SHIFT);
	value = (mclk_rate << I2S_OUT_MCLKRATE_SHIFT);
	sspreg_update(aio->io, aio->regs.i2s_mclk_cfg, mask, value);

	dev_dbg(aio->dev, "mclk cfg reg = 0x%x\n", value);
	dev_dbg(aio->dev, "bits per frame = %u, mclk = %u Hz, lrclk = %u Hz\n",
			bits_per_frame, aio->mclk, aio->lrclk);
	return 0;
}

static int cygnus_ssp_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_soc_dai *dai)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);
	int rate, bitres, bits_per_sample;
	u32 value;
	u32 mask;
	int ret = 0;

	dev_dbg(aio->dev, "%s port = %d\n", __func__, aio->portnum);
	dev_dbg(aio->dev, "params_channels %d\n", params_channels(params));
	dev_dbg(aio->dev, "rate %d\n", params_rate(params));
	dev_dbg(aio->dev, "format %d\n", params_format(params));

	rate = params_rate(params);

	switch (aio->mode) {
	case CYGNUS_SSPMODE_TDM:
		/* It's expected that set_dai_tdm_slot has been called */
		if ((rate == 192000) && (params_channels(params) > 4)) {
			dev_err(aio->dev, "Cannot run %d channels at %dHz\n",
				params_channels(params), rate);
			return -EINVAL;
		}
		break;
	case CYGNUS_SSPMODE_I2S:
		if (params_channels(params) != 2) {
			dev_err(aio->dev, "i2s mode must use 2 channels\n");
			return -EINVAL;
		}

		aio->active_slots = 2;
		aio->slots_per_frame = 2;
		aio->slot_width = 32; /* Use 64Fs */

		break;
	default:
		dev_err(aio->dev, "%s unknown mode\n", __func__);
		return -EINVAL;
	}

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		mask = BIT(BF_SRC_CFGX_SAMPLE_CH_MODE);
		sspreg_clr_bits(aio->io, aio->regs.bf_sourcech_cfg, mask);

		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			bitres = 16;
			bits_per_sample = 16;
			break;

		case SNDRV_PCM_FORMAT_S32_LE:
			bitres = 0; /* 32 bit mode is coded as 0 */
			bits_per_sample = 24; /* Only 24 valid bits */
			break;

		default:
			return -EINVAL;
		}

		mask = BF_SRC_CFGX_BITRES_MASK;
		value = (bitres << BF_SRC_CFGX_BIT_RES);
		sspreg_update(aio->io, aio->regs.bf_sourcech_cfg, mask, value);

		/* Only needed for LSB mode, ignored for MSB */
		mask = I2S_OUT_CFGX_BIT_PER_SAMPLE_MASK;
		value = (bits_per_sample << I2S_OUT_CFGX_BITS_PER_SAMPLE);
		sspreg_update(aio->io, aio->regs.i2s_cfg, mask, value);
	} else {

		switch (params_format(params)) {
		case SNDRV_PCM_FORMAT_S16_LE:
			bits_per_sample = 16;

			mask = BIT(BF_DST_CFGX_CAP_MODE);
			sspreg_set_bits(aio->io, aio->regs.bf_destch_cfg, mask);
			break;

		case SNDRV_PCM_FORMAT_S32_LE:
			bits_per_sample = 24; /* Only 24 valid bits */

			mask = BIT(BF_DST_CFGX_CAP_MODE);
			sspreg_clr_bits(aio->io, aio->regs.bf_destch_cfg, mask);
			break;

		default:
			return -EINVAL;
		}

		/* Used for both LSB and MSB modes */
		mask = I2S_IN_CFGX_BIT_PER_SAMPLE_MASK;
		value = (bits_per_sample << I2S_IN_CFGX_BITS_PER_SAMPLE);
		sspreg_update(aio->io, aio->regs.i2s_cap_cfg, mask, value);
	}

	/* Put output port into reset prior to configuring.
	 * This action is a workaround for couple situations:
	 *   1) JIRA-1312: 16-bit TDM Slave Tx problem
	 *      If the port is configured as 16-bit slave and
	 *      both CLK_ENA and DATA_ENABLE bits are off then the port will
	 *      fail to Tx.  Therefore, we hold port in reset until the
	 *      we are ready to enable
	 *   2) The TDM Slave Tx stream will be misaligned on the first
	 *      transfer after boot/reset (both 16 and 32 bit modes).
	 *      Applying reset will workaround this problem.
	 */
	if (aio->streams_on == 0) {
		if (aio->is_slave && (aio->mode == CYGNUS_SSPMODE_TDM)) {
			/*
			 * Need to do this LOGIC reset after boot/reset
			 * because it puts the logic in a slightly different
			 * than hard reset. In this way the chip logic will be
			 * in the same state for our first transfer as it is
			 * every transfer.
			 */
			mask = BIT(aio->portnum);
			mask |= BIT(IOP_LOGIC_RESET_IN_OFFSET(aio->portnum));
			sspreg_set_bits(aio->io, IOP_SW_INIT_LOGIC, mask);
		}

		if (aio->port_type != PORT_SPDIF)
			update_ssp_cfg(aio);

		aio->lrclk = rate;

		if (!aio->is_slave)
			ret = cygnus_ssp_set_clocks(aio);
	}

	return ret;
}

/*
 * Check that the actual mclk is within about 1% of the requested rate.
 * The check is rather loose and is intended to catch any big mistakes.
 * It is expected that the actual mclk rate may be a little different
 * than the requested rate because the clock from which the mclk is
 * derived (PLL) may not be an exact multiple of the mclk.
 */
static bool mclk_in_range(unsigned int target, unsigned int actual)
{
	unsigned int delta;

	/* Mclk is at least several MHz, so simple div by 100 will suffice */
	delta = target / 100;
	return (actual > (target - delta)) && (actual < (target + delta));
}

/*
 * This function sets the mclk frequency for pll clock
 */
static int cygnus_ssp_set_sysclk(struct snd_soc_dai *dai,
			int clk_id, unsigned int freq, int dir)
{
	int sel;
	int ret;
	u32 value, mask;
	long rate;
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);

	dev_dbg(aio->dev, "%s Enter port = %d\n", __func__, aio->portnum);

	/*
	 * This should not happen, but the machine file may inadvertently
	 * call set_sysclk without configuring a clock via the devicetree.
	 */
	if (!aio->clk_info.audio_clk) {
		dev_err(aio->dev, "%s Error. No clock assigned.\n", __func__);
		return -ENODEV;
	}

	rate = clk_round_rate(aio->clk_info.audio_clk, freq);
	if (rate < 0) {
		dev_err(aio->dev, "%s Error with with clock %ld.\n",
			__func__, rate);
		return rate;
	}

	if (!mclk_in_range(freq, rate)) {
		dev_err(aio->dev, "%s Can not set rate to %u  actual %ld.\n",
			__func__, freq, rate);
		return -EINVAL;
	}

	ret = clk_set_rate(aio->clk_info.audio_clk, freq);
	if (ret) {
		dev_err(aio->dev, "%s Set MCLK rate fail %d\n", __func__, ret);
		return ret;
	}

	aio->mclk = freq;
	sel = aio->clk_info.clk_mux;

	dev_dbg(aio->dev, "%s Setting MCLKSEL to %d\n", __func__, sel);

	mask = (0xf << I2S_OUT_PLLCLKSEL_SHIFT);
	value = (sel << I2S_OUT_PLLCLKSEL_SHIFT);
	sspreg_update(aio->io, aio->regs.i2s_mclk_cfg, mask, value);

	/* Clear bit for active */
	mask = BIT(AUD_MISC_SEROUT_MCLK_OE + (aio->portnum * 4));
	sspreg_clr_bits(aio->io, AUD_MISC_SEROUT_OE_REG_BASE, mask);

	return 0;
}

static int cygnus_ssp_startup(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct cygnus_audio *cygaud = snd_soc_dai_get_drvdata(dai);
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);
	struct iproc_pcm_dma_info *dma_info;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		dma_info = &cygaud->dma_info_play[aio->portnum];
	else
		dma_info = &cygaud->dma_info_cap[aio->portnum];

	dma_info->portnum = aio->portnum;
	dma_info->audio = aio->io->audio;
	dma_info->substream = substream;

	snd_soc_dai_set_dma_data(dai, substream, dma_info);

	substream->runtime->hw.rate_min = CYGNUS_RATE_MIN;
	substream->runtime->hw.rate_max = CYGNUS_RATE_MAX;

	snd_pcm_hw_constraint_list(substream->runtime, 0,
			SNDRV_PCM_HW_PARAM_RATE, &cygnus_rate_constraint);

	return clk_prepare_enable(aio->clk_info.audio_clk);
}

static void cygnus_ssp_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);

	clk_disable_unprepare(aio->clk_info.audio_clk);
}

int cygnus_ssp_set_custom_fsync_width(struct snd_soc_dai *cpu_dai, int len)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(cpu_dai);

	if ((len > 0) && (len < 256)) {
		aio->fsync_width = len;
		return 0;
	} else {
		return -EINVAL;
	}
}
EXPORT_SYMBOL_GPL(cygnus_ssp_set_custom_fsync_width);

static int cygnus_ssp_set_fmt(struct snd_soc_dai *cpu_dai, unsigned int fmt)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(cpu_dai);
	u32 val;
	u32 mask;

	dev_dbg(aio->dev, "%s Enter  fmt: %x\n", __func__, fmt);

	if (aio->port_type == PORT_SPDIF)
		return -EINVAL;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		aio->is_slave = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		aio->is_slave = 0;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		aio->fs_delay = 1;
		aio->mode = CYGNUS_SSPMODE_I2S;
		break;

	case SND_SOC_DAIFMT_DSP_A:
	case SND_SOC_DAIFMT_DSP_B:
		/* DSP_A = data after FS, DSP_B = data during FS */
		if ((fmt & SND_SOC_DAIFMT_FORMAT_MASK) == SND_SOC_DAIFMT_DSP_A)
			aio->fs_delay = 1;
		else {
			if (aio->is_slave) {
				dev_err(aio->dev,
				"%s DSP_B mode not supported while slave.\n",
					__func__);
				return -EINVAL;
			}
			aio->fs_delay = 0;
		}
		aio->mode = CYGNUS_SSPMODE_TDM;
		break;

	default:
		return -EINVAL;
	}

	/* We must be i2s master to invert any clock */
	if ((fmt & SND_SOC_DAIFMT_INV_MASK) != SND_SOC_DAIFMT_NB_NF) {
		if (aio->is_slave || (aio->mode == CYGNUS_SSPMODE_TDM)) {
			dev_err(aio->dev,
			"%s Can only invert clocks in i2s master mode\n",
				__func__);
			return -EINVAL;
		}
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_IB_NF:
		aio->invert_bclk = true;
		aio->invert_fs = false;
		break;
	case SND_SOC_DAIFMT_NB_IF:
		aio->invert_bclk = false;
		aio->invert_fs = true;
		break;
	case SND_SOC_DAIFMT_IB_IF:
		aio->invert_bclk = true;
		aio->invert_fs = true;
		break;
	case SND_SOC_DAIFMT_NB_NF:
		aio->invert_bclk = false;
		aio->invert_fs = false;
		break;
	default:
		return -EINVAL;
	}

	/*
	 * Configure the word clk and bit clk as output or tristate
	 * Each port has 4 bits for controlling its pins.
	 * Shift the mask based upon port number.
	 */
	mask = BIT(AUD_MISC_SEROUT_LRCK_OE) | BIT(AUD_MISC_SEROUT_SCLK_OE);
	mask = (mask << (aio->portnum * 4));
	if (aio->is_slave)
		val = mask;   /* Set bit for tri-state */
	else
		val = 0;  /* Clear bit for drive */

	dev_dbg(aio->dev, "%s  Set OE bits 0x%x\n", __func__, val);
	sspreg_update(aio->io, AUD_MISC_SEROUT_OE_REG_BASE, mask, val);

	return 0;
}

static int cygnus_ssp_trigger(struct snd_pcm_substream *substream, int cmd,
			       struct snd_soc_dai *dai)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);

	dev_dbg(aio->dev,
		"%s cmd %d at port = %d\n", __func__, cmd, aio->portnum);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			audio_ssp_out_enable(aio);
		else
			audio_ssp_in_enable(aio);

		break;

	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
			audio_ssp_out_disable(aio);
		else
			audio_ssp_in_disable(aio);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static int cygnus_set_dai_tdm_slot(struct snd_soc_dai *cpu_dai,
	unsigned int tx_mask, unsigned int rx_mask, int slots, int slot_width)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(cpu_dai);
	unsigned int active_slots;
	unsigned int bits_per_frame;
	bool found = false;
	unsigned int i;

	if (tx_mask != rx_mask) {
		dev_err(aio->dev, "%s tx_mask must equal rx_mask\n", __func__);
		return -EINVAL;
	}

	active_slots = hweight32(tx_mask);

	if ((active_slots == 0) || (active_slots > 16))
		return -EINVAL;

	/* Slot value must be even */
	if (active_slots % 2)
		return -EINVAL;

	if ((slot_width != 16) && (slot_width != 32))
		return -EINVAL;

	bits_per_frame = slots * slot_width;

	for (i = 0; i < ARRAY_SIZE(ssp_valid_tdm_framesize); i++) {
		if (ssp_valid_tdm_framesize[i] == bits_per_frame) {
			found = true;
			break;
		}
	}

	if (!found) {
		dev_err(aio->dev, "%s In TDM mode, frame bits INVALID (%d)\n",
			__func__, bits_per_frame);
		return -EINVAL;
	}

	aio->active_slots = active_slots;
	aio->slot_width = slot_width;
	aio->slots_per_frame = slots;

	dev_dbg(aio->dev, "%s active_slots %u, bits per frame %d\n",
			__func__, aio->active_slots, bits_per_frame);
	return 0;
}

static int cygnus_ssp_set_pll(struct snd_soc_dai *cpu_dai, int pll_id,
				 int source, unsigned int freq_in,
				 unsigned int freq_out)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(cpu_dai);
	struct clk *clk_pll;
	int ret = 0;

	if (!aio->clk_info.audio_clk) {
		dev_err(aio->dev,
			"%s: port %d does not have an assigned clock.\n",
			__func__, aio->portnum);
		return -ENODEV;
	}

	clk_pll = clk_get_parent(aio->clk_info.audio_clk);
	if (IS_ERR(clk_pll)) {
		dev_err(aio->dev,
			"%s: could not get audiopll clock.\n", __func__);
		return -ENODEV;
	}

	ret = clk_set_rate(clk_pll, freq_out);

	return ret;
}

/*
 * Bit    Update  Notes
 * 31     Yes     TDM Mode        (1 = TDM, 0 = i2s)
 * 30     Yes     Slave Mode      (1 = Slave, 0 = Master)
 * 29:26  No      Sclks per frame
 * 25:18  Yes     FS Width
 * 17:14  Yes     Valid Slots
 * 13     Yes     Bits            (1 = 16 bits, 0 = 32 bits)
 * 12:08  No      Bits per samp
 * 07     No      Justifcation    (1 = LSB, 0 = MSB)
 * 06     Yes     Alignment       (1 = Delay 1 clk, 0 = no delay
 * 05     Yes     SCLK polarity   (1 = Rising, 0 = Falling)
 * 04     Yes     LRCLK Polarity  (1 = High for left, 0 = Low for left)
 * 03:02  No      Reserved - write as zero
 * 01     No      Data Enable
 * 00     No      CLK Enable
 */
#define I2S_OUT_CFG_REG_UPDATE_MASK  \
	(BIT(I2S_OUT_CFGX_TDM_MODE)       | \
	BIT(I2S_OUT_CFGX_SLAVE_MODE)      | \
	I2S_OUT_CFGX_FSYNC_WIDTH_MASK     | \
	I2S_OUT_CFGX_VALID_SLOT_MASK      | \
	BIT(I2S_OUT_CFGX_BITS_PER_SLOT)   | \
	BIT(I2S_OUT_CFGX_DATA_ALIGNMENT)  | \
	BIT(I2S_OUT_CFGX_SCLK_POLARITY)   | \
	BIT(I2S_OUT_CFGX_LRCK_POLARITY))

/* Input cfg is same as output, but the FS width is not a valid field */
#define I2S_IN_CFG_REG_UPDATE_MASK  (I2S_OUT_CFG_REG_UPDATE_MASK & \
				     ~I2S_OUT_CFGX_FSYNC_WIDTH_MASK)

static void update_ssp_cfg(struct cygnus_aio_port *aio)
{
	u32 valid_slots;       /* reg val to program */
	int bits_per_slot_cmn;

	u32 ssp_newcfg;
	u32 ssp_outcfg;
	u32 ssp_incfg;
	u32 fsync_width;

	if (aio->port_type == PORT_SPDIF)
		return;

	ssp_newcfg = 0;

	/* We encode 16 slots as 0 in the reg */
	valid_slots = aio->active_slots;
	if (aio->active_slots == 16)
		valid_slots = 0;

	ssp_newcfg |= (valid_slots << I2S_OUT_CFGX_VALID_SLOT);

	/* Slot Width is either 16 or 32 */
	bits_per_slot_cmn = 0;     /* Default to 32 bits */
	if (aio->slot_width <= 16)
		bits_per_slot_cmn = 1;

	ssp_newcfg |= (bits_per_slot_cmn << I2S_OUT_CFGX_BITS_PER_SLOT);

	if (aio->mode == CYGNUS_SSPMODE_TDM) {
		ssp_newcfg |= BIT(I2S_OUT_CFGX_TDM_MODE);
		if (aio->fsync_width == -1)
			fsync_width = 1;
		else
			fsync_width = aio->fsync_width;

		ssp_newcfg |= (fsync_width << I2S_OUT_CFGX_FSYNC_WIDTH);
	}

	if (aio->is_slave)
		ssp_newcfg |= BIT(I2S_OUT_CFGX_SLAVE_MODE);
	else
		ssp_newcfg &= ~BIT(I2S_OUT_CFGX_SLAVE_MODE);

	if (aio->mode == CYGNUS_SSPMODE_I2S) {
		ssp_newcfg |= BIT(I2S_OUT_CFGX_DATA_ALIGNMENT);
		ssp_newcfg |= BIT(I2S_OUT_CFGX_FSYNC_WIDTH);
	} else {
		if (aio->fs_delay == 0)
			ssp_newcfg &= ~BIT(I2S_OUT_CFGX_DATA_ALIGNMENT);
		else
			ssp_newcfg |= BIT(I2S_OUT_CFGX_DATA_ALIGNMENT);
	}

	if (aio->invert_bclk)
		ssp_newcfg |= BIT(I2S_OUT_CFGX_SCLK_POLARITY);

	if (aio->invert_fs)
		ssp_newcfg |= BIT(I2S_OUT_CFGX_LRCK_POLARITY);

	/*
	 * SSP in cfg.
	 * Always set slave mode for Rx formatter.
	 * The Rx formatter's Slave Mode bit controls if it uses its own
	 * internal clock or the clock signal that comes from the Slave Mode
	 * bit set in the Tx formatter (which would be the Tx Formatters
	 * internal clock or signal from external pin).
	 */
	ssp_incfg = ssp_newcfg | BIT(I2S_OUT_CFGX_SLAVE_MODE);
	sspreg_update(aio->io, aio->regs.i2s_cap_cfg,
			I2S_IN_CFG_REG_UPDATE_MASK, ssp_incfg);

	ssp_outcfg = ssp_newcfg;
	sspreg_update(aio->io, aio->regs.i2s_cfg,
			I2S_OUT_CFG_REG_UPDATE_MASK, ssp_outcfg);
}

/*
 * This function is intended to assist in the situation where an external
 * codec is using the MCLK we generate.  The codec may need to have the
 * clock present earlier than we would normally enable it in the audio
 * driver.  This API give the machine driver control over when the clock
 * is enabled.
 */
int cygnus_ssp_get_clk(struct snd_soc_dai *dai, unsigned int freq)
{
	int error;
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);

	if (!aio->clk_info.audio_clk) {
		dev_err(aio->dev, "%s Clock was not provided\n", __func__);
		return -ENODEV;
	}

	error = clk_prepare_enable(aio->clk_info.audio_clk);
	if (error) {
		dev_err(aio->dev, "%s clk_prepare_enable failed %d\n",
					__func__, error);
		return error;
	}

	cygnus_ssp_set_sysclk(dai, 0, freq, SND_SOC_CLOCK_OUT);

	return 0;
}
EXPORT_SYMBOL_GPL(cygnus_ssp_get_clk);

int cygnus_ssp_put_clk(struct snd_soc_dai *dai)
{
	struct cygnus_aio_port *aio = cygnus_dai_get_portinfo(dai);

	if (aio->clk_info.audio_clk) {
		clk_disable_unprepare(aio->clk_info.audio_clk);
	} else {
		dev_err(aio->dev, "%s Clock was not provided\n", __func__);
		return -ENODEV;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(cygnus_ssp_put_clk);


static const struct snd_soc_dai_ops cygnus_ssp_dai_ops = {
	.startup	= cygnus_ssp_startup,
	.shutdown	= cygnus_ssp_shutdown,
	.trigger	= cygnus_ssp_trigger,
	.hw_params	= cygnus_ssp_hw_params,
	.set_fmt	= cygnus_ssp_set_fmt,
	.set_sysclk	= cygnus_ssp_set_sysclk,
	.set_tdm_slot	= cygnus_set_dai_tdm_slot,
	.set_pll	= cygnus_ssp_set_pll,
};

static const struct snd_soc_dai_ops cygnus_spdif_dai_ops = {
	.startup	= cygnus_ssp_startup,
	.shutdown	= cygnus_ssp_shutdown,
	.trigger	= cygnus_ssp_trigger,
	.hw_params	= cygnus_ssp_hw_params,
	.set_sysclk	= cygnus_ssp_set_sysclk,
};

#define INIT_CPU_DAI(num) { \
	.name = "cygnus-ssp" #num, \
	.playback = { \
		.channels_min = 2, \
		.channels_max = 16, \
		.rates = SNDRV_PCM_RATE_KNOT, \
		.formats = SNDRV_PCM_FMTBIT_S8 | \
				SNDRV_PCM_FMTBIT_S16_LE | \
				SNDRV_PCM_FMTBIT_S32_LE, \
	}, \
	.capture = { \
		.channels_min = 2, \
		.channels_max = 16, \
		.rates = SNDRV_PCM_RATE_KNOT, \
		.formats = SNDRV_PCM_FMTBIT_S16_LE | \
				SNDRV_PCM_FMTBIT_S32_LE, \
	}, \
	.ops = &cygnus_ssp_dai_ops, \
}

static const struct snd_soc_dai_driver cygnus_ssp_dai_info[] = {
	INIT_CPU_DAI(0),
	INIT_CPU_DAI(1),
	INIT_CPU_DAI(2),
};

static const struct snd_soc_dai_driver cygnus_spdif_dai_info = {
	.name = "cygnus-spdif",
	.playback = {
		.channels_min = 2,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE_KNOT,
		.formats = SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_S32_LE,
	},
	.ops = &cygnus_spdif_dai_ops,
};

static struct snd_soc_dai_driver cygnus_ssp_dai[CYGNUS_MAX_PORTS];

static const struct snd_soc_component_driver cygnus_ssp_component = {
	.name		= "cygnus-audio",
};

/*
 * Return < 0 if error
 * Return 0 if disabled
 * Return 1 if enabled and node is parsed successfully
 */
static int parse_ssp_child_node(struct platform_device *pdev,
				struct device_node *dn,
				struct cygnus_audio *cygaud,
				struct snd_soc_dai_driver *p_dai,
				struct cygnus_aio_port *aio)
{
	struct cygnus_ssp_regs ssp_regs[3];
	u32 rawval;
	int portnum = -1;
	enum cygnus_audio_port_type port_type;
	u32 muxval;
	struct clk *clk;

	if (of_property_read_u32(dn, "reg", &rawval)) {
		dev_err(&pdev->dev, "Missing reg property\n");
		return -EINVAL;
	}

	portnum = rawval;
	switch (rawval) {
	case 0:
		ssp_regs[0] = INIT_SSP_REGS(0);
		port_type = PORT_TDM;
		break;
	case 1:
		ssp_regs[1] = INIT_SSP_REGS(1);
		port_type = PORT_TDM;
		break;
	case 2:
		ssp_regs[2] = INIT_SSP_REGS(2);
		port_type = PORT_TDM;
		break;
	case 3:
		port_type = PORT_SPDIF;
		break;
	default:
		dev_err(&pdev->dev, "Bad value for reg %u\n", rawval);
		return -EINVAL;
	}

	aio->io = &cygaud->io;
	aio->portnum = portnum;
	aio->port_type = port_type;
	aio->fsync_width = -1;

	switch (port_type) {
	case PORT_TDM:
		aio->regs = ssp_regs[portnum];
		*p_dai = cygnus_ssp_dai_info[portnum];
		aio->mode = CYGNUS_SSPMODE_UNKNOWN;
		break;

	case PORT_SPDIF:
		aio->regs.bf_sourcech_cfg = BF_SRC_CFG3_OFFSET;
		aio->regs.bf_sourcech_ctrl = BF_SRC_CTRL3_OFFSET;
		aio->regs.i2s_mclk_cfg = SPDIF_MCLK_CFG_OFFSET;
		aio->regs.i2s_stream_cfg = SPDIF_STREAM_CFG_OFFSET;
		*p_dai = cygnus_spdif_dai_info;

		/* For the purposes of this code SPDIF can be I2S mode */
		aio->mode = CYGNUS_SSPMODE_I2S;
		break;
	default:
		dev_err(&pdev->dev, "Bad value for port_type %d\n", port_type);
		return -EINVAL;
	}

	dev_dbg(&pdev->dev, "%s portnum = %d\n", __func__, aio->portnum);
	aio->streams_on = 0;
	aio->dev = &pdev->dev;

	aio->clk_info.audio_clk = NULL;

	/*
	 * The default in the DT is to assign a clock. It is possible
	 * the user may not want a clock if the port is only used in slave
	 * mode.  In this case, they could override the default using this
	 * mechanism:    /delete-property/ clocks;
	 */
	if (of_property_read_bool(dn, "clocks")) {
		clk = devm_get_clk_from_child(&pdev->dev, dn, "ssp_clk");
		if (IS_ERR(clk)) {
			dev_err(&pdev->dev,
				"Port %d: devm_clk_get ssp-clk err %ld\n",
				portnum, PTR_ERR(clk));
			return PTR_ERR(clk);
		}

		aio->clk_info.audio_clk = clk;

		if (of_property_read_u32(dn, "brcm,ssp-clk-mux", &muxval)) {
			dev_err(&pdev->dev, "Missing property clock-mux\n");
			return -EINVAL;
		}
		aio->clk_info.clk_mux = muxval;
	} else {
		dev_dbg(&pdev->dev, "No clock provided for port %d\n", portnum);
	}

	return audio_ssp_init_portregs(aio);
}

static struct regmap_config cygnusaudio_audio_regmap_cfg = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,

	.cache_type = REGCACHE_NONE,
};

static struct regmap_config cygnusaudio_i2s_in_regmap_cfg = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,

	.cache_type = REGCACHE_NONE,
};

static int cygnus_ssp_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *child_node;
	struct resource *res = pdev->resource;
	struct cygnus_audio *cygaud;
	void __iomem *ioregs;
	int err = -EINVAL;
	int node_count;
	int active_port_count;
	int irq_num;
	u32 mask;

	cygaud = devm_kzalloc(dev, sizeof(struct cygnus_audio), GFP_KERNEL);
	if (!cygaud)
		return -ENOMEM;

	dev_set_drvdata(dev, cygaud);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "aud");
	ioregs = devm_ioremap_resource(dev, res);
	if (IS_ERR(ioregs))
		return PTR_ERR(ioregs);

	cygnusaudio_audio_regmap_cfg.max_register = resource_size(res);
	cygaud->io.audio = devm_regmap_init_mmio(&pdev->dev, ioregs,
					    &cygnusaudio_audio_regmap_cfg);
	if (IS_ERR(cygaud->io.audio)) {
		dev_err(&pdev->dev, "regmap failed\n");
		return PTR_ERR(cygaud->io.audio);
	}

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "i2s_in");
	ioregs = devm_ioremap_resource(dev, res);
	if (IS_ERR(ioregs))
		return PTR_ERR(ioregs);

	cygnusaudio_i2s_in_regmap_cfg.max_register = resource_size(res);
	cygaud->io.i2s_in = devm_regmap_init_mmio(&pdev->dev, ioregs,
					    &cygnusaudio_i2s_in_regmap_cfg);
	if (IS_ERR(cygaud->io.i2s_in)) {
		dev_err(&pdev->dev, "regmap failed\n");
		return PTR_ERR(cygaud->io.i2s_in);
	}

	/* Tri-state all controlable pins until we know that we need them */
	mask = CYGNUS_SSP_TRISTATE_MASK;
	sspreg_set_bits(&cygaud->io, AUD_MISC_SEROUT_OE_REG_BASE, mask);

	node_count = of_get_child_count(pdev->dev.of_node);
	if ((node_count < 1) || (node_count > CYGNUS_MAX_PORTS)) {
		dev_err(dev, "child nodes is %d.  Must be between 1 and %d\n",
			node_count, CYGNUS_MAX_PORTS);
		return -EINVAL;
	}

	active_port_count = 0;
	for_each_available_child_of_node(pdev->dev.of_node, child_node) {
		err = parse_ssp_child_node(pdev, child_node, cygaud,
					&cygnus_ssp_dai[active_port_count],
					&cygaud->portinfo[active_port_count]);

		/* negative is err, 0 is active and good, 1 is disabled */
		if (err < 0)
			return err;
		else if (!err) {
			dev_dbg(dev, "Activating DAI: %s\n",
				cygnus_ssp_dai[active_port_count].name);
			active_port_count++;
		}
	}

	cygaud->dev = dev;

	dev_dbg(dev, "Registering %d DAIs\n", active_port_count);
	err = snd_soc_register_component(dev, &cygnus_ssp_component,
				cygnus_ssp_dai, active_port_count);
	if (err) {
		dev_err(dev, "snd_soc_register_dai failed\n");
		return err;
	}

	irq_num = platform_get_irq(pdev, 0);
	if (irq_num <= 0) {
		dev_err(dev, "platform_get_irq failed\n");
		err = irq_num;
		goto err_irq;
	}

	cygaud->rb_info.dev = dev;
	cygaud->rb_info.audio = cygaud->io.audio;
	cygaud->rb_info.irq_num = irq_num;

	cygaud->rb_info.num_playback = CYGNUS_MAX_PLAYBACK_PORTS;
	cygaud->rb_info.rb_state_play = &cygaud->dma_info_play[0];

	cygaud->rb_info.num_capture = CYGNUS_MAX_CAPTURE_PORTS;
	cygaud->rb_info.rb_state_cap = &cygaud->dma_info_cap[0];

	err = iproc_pcm_platform_register(dev, &cygaud->rb_info);
	if (err) {
		dev_err(dev, "platform reg error %d\n", err);
		goto err_irq;
	}

	return 0;

err_irq:
	snd_soc_unregister_component(dev);
	return err;
}

static int cygnus_ssp_remove(struct platform_device *pdev)
{
	iproc_pcm_platform_unregister(&pdev->dev);
	snd_soc_unregister_component(&pdev->dev);

	return 0;
}

static const struct of_device_id cygnus_ssp_of_match[] = {
	{ .compatible = "brcm,cygnus-audio" },
	{},
};
MODULE_DEVICE_TABLE(of, cygnus_ssp_of_match);

static struct platform_driver cygnus_ssp_driver = {
	.probe		= cygnus_ssp_probe,
	.remove		= cygnus_ssp_remove,
	.driver		= {
		.name	= "cygnus-ssp",
		.of_match_table = cygnus_ssp_of_match,
	},
};

module_platform_driver(cygnus_ssp_driver);

MODULE_ALIAS("platform:cygnus-ssp");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Broadcom");
MODULE_DESCRIPTION("Cygnus ASoC SSP Interface");
