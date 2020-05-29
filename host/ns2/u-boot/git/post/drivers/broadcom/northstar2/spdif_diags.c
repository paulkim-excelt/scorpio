/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * This file contains routines to perform diags
 * for SPDIF interface.
 *
 */

#include <common.h>
#include <post.h>
#include <malloc.h>

#if CONFIG_POST & CONFIG_SYS_POST_SPDIF

#include "../../../../drivers/sound/audio_data.h"
#include "../../../../drivers/sound/northstar2/audio_apis.h"
#include <asm/arch/socregs.h>

#define I2S_DEBUG 0

#define STEREO_INTERLEAVED_L 0x87000000
#define STEREO_INTERLEAVED_R 0x88000000

int audio_spdif_out_test(void)
{
	uint32_t *stereo_interleaved_l, *stereo_interleaved_r;
	uint32_t spdif_en = 1;
	int i;
	char ch;

	/* Buffer addresses to be configured */
	stereo_interleaved_l = (uint32_t *)STEREO_INTERLEAVED_L;

	for (i = 0; i < 480; i++) {
		*stereo_interleaved_l = left_channel_samples[i];
		stereo_interleaved_l++;
	}
	stereo_interleaved_l = (uint32_t *)STEREO_INTERLEAVED_L;

	stereo_interleaved_r = (uint32_t *)STEREO_INTERLEAVED_R;

	for (i = 0; i < 480; i++) {
		*stereo_interleaved_r = right_channel_samples[i];
		stereo_interleaved_r++;
	}
	stereo_interleaved_r = (uint32_t *)STEREO_INTERLEAVED_R;

	/* This test uses Stereo interleaved data to be played back from DDR */
	/* 1. Audio Soft reset and other Init */
	audio_sw_reset();

	/* 2. PLL Power ON and Configuration */

	/* Configuring CRMU_PLL_AON_CTRL */
	audio_gen_pll_pwr_on(1);

	/* user macro set to 48kHz clock */
	audio_gen_pll_group_id_config(1, 0x00000046, 0x000C75FF,
				      0x000000D8, 4, 0, 8, 2, 0, 0);

	post_log("Audio PLL configuration done\n");

	/* 3. Source Channel Configuration */

	/* Audio BF control source channel configuration
	   AUD_FMM_BF_CTRL_SOURCECH_CFG3 */
	post_log("Source Channel 3 under configuration\n");
	audio_fmm_bf_ctrl_source_cfg(spdif_en, 1, 0, 0, 0, 0, 0, 0, 0,
				     1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0);
	post_log("Source Channel 3 configuration done\n");

	/* 3a. Configure the Source Channel Ring Buffer Addresses */
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR,
		      0x87000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR,
		      0x8700077F, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR,
		      0x8700077F, 4);

#if I2S_DEBUG
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR + I2S_ROOT));
#endif

	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR,
		      0x88000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR,
		      0x8800077F, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR,
		      0x88000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR,
		      0x8800077F, 4);

#if I2S_DEBUG
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR + I2S_ROOT));
	post_log("AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR: %x\n",
		 readl(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR + I2S_ROOT));
#endif
	post_log("Source Channel Ring Buffer Addresses Configured\n");

	/* 4. SPDIF registers configuration */

	/* SPDIF output stream config AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0 */
	audio_spdif_stream_config(0, 1, 0, 8, 0, 1, 0, 0, 3);

	/* SPDIF CTRL config AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL */
	audio_spdif_ctrl(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 8, 1, 0, 0);

	/* SPDIF channel status config for 48KHz linear PCM audio
	   AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0/1/2 */
	audio_spdif_ch_status(0x00002000, 0x00000000, 0x00000000);

	/* SPDIF format config AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG */
	audio_spdif_format_config(0, 0, 1, 1, 1);

	/* SPDIF MCLK, PLL clock select AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0 */
	audio_spdif_mclk_config(2, 1);

	/* SPDIF stream enable AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0 */
	audio_spdif_stream_config(1, 1, 0, 8, 0, 1, 0, 0, 3);

	/* Source FIFO enable AUD_FMM_BF_CTRL_SOURCECH_CFG3 */
	audio_fmm_bf_ctrl_source_cfg(spdif_en, 1, 0, 0, 0, 0, 0, 0, 0, 1,
				     0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0);

	/* 5. Play audio */
	/* PLAY RUN AUD_FMM_BF_CTRL_SOURCECH_CTRL3__PLAY_RUN */
	audio_start_dma_read(1, 1, 0);

	post_log("\nEnd of test: check for 1KHz audio on the left channel and");
	post_log(" 2KHz audio on the right channel. (audio_spdif_out_test)\n");

	post_log("\nConfirm audio status (Y/N) to exit the test.\n");
	do {
		ch = (char) serial_getc();
	} while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));

	if ((ch == 'n') || (ch == 'N')) {
		post_log("\nError.. exiting the test.\n");
		/* Stop play_run and Reset the audio subsystem */
		writel(0x0, (AUD_FMM_BF_CTRL_SOURCECH_CTRL3 + I2S_ROOT));
		writel(0xffffffff, (AUD_MISC_INIT + I2S_ROOT));
		writel(0x0, (AUD_MISC_INIT + I2S_ROOT));
		return -1;
	} else {
		/* Stop play_run and Reset the audio subsystem */
		writel(0x0, (AUD_FMM_BF_CTRL_SOURCECH_CTRL3 + I2S_ROOT));
		writel(0xffffffff, (AUD_MISC_INIT + I2S_ROOT));
		writel(0x0, (AUD_MISC_INIT + I2S_ROOT));
	}
	return 0;
}

static void help(void)
{
	post_log("\n ----------------------\n");
	post_log("| SPDIF DIAG HELP MENU |\n");
	post_log(" ----------------------\n");
}

int spdif_post_test(int flags)
{
	int status = 0;

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	post_log("Running SPDIF diags...\n");
	status = audio_spdif_out_test();
	return status;
}
#endif /* CONFIG_POST && CONFIG_SYS_POST_SPDIF */
