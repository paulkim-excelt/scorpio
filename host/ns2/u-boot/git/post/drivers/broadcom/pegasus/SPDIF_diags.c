/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * Ported from NS2 SPDIF Diags.
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
#include "../../../../drivers/sound/pegasus/audio_apis.h"
#include <asm/arch-bcm_pegasus/socregs.h>
#include <asm/arch/bcm_otp.h>
#define I2S_DEBUG 0

#define STEREO_INTERLEAVED_L 0x87000000
#define STEREO_INTERLEAVED_R 0x88000000

static void enable_i2s(void)
{
	cpu_wr_single(ICFG_PD_MEM_CTRL,0x0020000f,4);
	cpu_wr_single(ICFG_PD_MEM_CTRL,0x0020000e,4);
	cpu_wr_single(ICFG_PD_MEM_CTRL,0xffdffff0,4);
	cpu_wr_single(ICFG_PD_MEM_CTRL,0x0020000f,4);
	cpu_wr_single(ICFG_PD_MEM_CTRL,0x0020000e,4);

	cpu_wr_single(ICFG_SP_MEM_CTRL,0x0004000f,4);
	cpu_wr_single(ICFG_SP_MEM_CTRL,0x0004000e,4);
	cpu_wr_single(ICFG_SP_MEM_CTRL,0xfffbfff0,4);
	cpu_wr_single(ICFG_SP_MEM_CTRL,0x0004000f,4);
	cpu_wr_single(ICFG_SP_MEM_CTRL,0x0004000e,4);

	cpu_wr_single(CRYPTO_SS_SPU0_MDE_RCVPFCCTRL_0,0x0000000f,4);
	cpu_wr_single(CRYPTO_SS_SPU0_MDE_RCVPFCCTRL_0,0x0000000e,4);
	cpu_wr_single(CRYPTO_SS_SPU0_MDE_RCVPFCCTRL_0,0xfffffff0,4);
	cpu_wr_single(CRYPTO_SS_SPU0_MDE_RCVPFCCTRL_0,0x0000000f,4);
	cpu_wr_single(CRYPTO_SS_SPU0_MDE_RCVPFCCTRL_0,0x0000000e,4);

	cpu_wr_single(ICFG_RF_MEM_CTRL,0x0000000f,4);
	cpu_wr_single(ICFG_RF_MEM_CTRL,0x0000000e,4);
	cpu_wr_single(ICFG_RF_MEM_CTRL,0xfffffff0,4);
	cpu_wr_single(ICFG_RF_MEM_CTRL,0x0000000f,4);
	cpu_wr_single(ICFG_RF_MEM_CTRL,0x0000000e,4);
}

int audio_spdif_out_test(void)
{
	uint32_t *stereo_interleaved_l, *stereo_interleaved_r;
	uint32_t spdif_en = 1;
	int i;
	/*uint32_t *temp; */
	char ch;

	/*Buffer addresses to be configured */
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

	enable_i2s();

/* 1. Audio Soft reset and other Init */

	audio_sw_reset();

/* 2. PLL Power ON and Configuration */
	/*Configuring CRMU PLL CONTROL REGISTER */
	audio_gen_pll_pwr_on(1);/*CRMU_PLL_AON_CTRL*/

	/*user macro set to 48kHz clock */
	audio_gen_pll_group_id_config(1, 0x00000046, 0x000C75FF, 0x000000D8, 4,
				      0, 8, 2, 0, 0);

	post_log("Audio PLL configuration done \n");

/* 3. Source Channel Configuration */

	/*Audio BF control source channel configuration */
	post_log("Source Channel 3 under configuration \n");
	/*AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3 */
	audio_fmm_bf_ctrl_source_cfg(spdif_en, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
				     0, 0, 0, 0, 1, 0, 0);
	post_log("Source Channel 3 configuration done \n");

/* 3a. Configure the Source Channel Ring Buffer Addresses */

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR,
		      0x8700077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR,
		      0x8700077F, 4);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_MI_VALID, 0, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_START_WRPOINT, 0, 4);


	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR,
		      0x8800077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR,
		      0x8800077F, 4);

	post_log("Source Channel Ring Buffer Addresses Configured \n");


/* 3b. enable i2s */

	enable_i2s();

/* 4. SPDIF registers configuration */

	/*SPDIF OUTPUT STREAM CONFIG */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0 */
	audio_spdif_stream_config(0, 1, 0, 8, 0, 1, 0, 0, 3);

	/*SPDIF CTRL CONFIG */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL */
	audio_spdif_ctrl(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 8, 1, 0, 0);

	/*SPDIF CHANNEL STATUS CONFIGURATION for 48 KHz linear pcm audio */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0/1/2 */
	audio_spdif_ch_status(0x00002000, 0x00000000, 0x00000000);

	/*SPDIF FORMAT CONFIG */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG */
	audio_spdif_format_config(0, 0, 1, 1, 1);

	/*SPDIF MCLK AND PLL CLOCK SELECT, MCLK,PLL */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0 */
	audio_spdif_mclk_config(2, 1);

	/*SPDIF STREAM ENABLE */
	/*AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0 */
	audio_spdif_stream_config(1, 1, 0, 8, 0, 1, 0, 0, 3);


	/*SOURCE FIFO ENABLE */
	/*AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3 */
	audio_fmm_bf_ctrl_source_cfg(spdif_en, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
				     1, 0, 0, 0, 0, 0, 1, 1, 0);

/* 5. Play audio */
	/*PLAY RUN */
	/*AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3__PLAY_RUN */
	audio_start_dma_read(1, 1, 0);

	post_log
	    ("End of test : check for 1 KHz audio on the left channel and 2 KHz audio on the right channel - audio_spdif_out_test \n");

	post_log("\nConfirm audio status (Y/N) to exit the test \n");
	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y')
		 && (ch != 'n') && (ch != 'N'));
	if ((ch == 'n') || (ch == 'N')) {
		post_log("\nError. Exiting the test\n");
		return -1;
	} else {
		/* Stop play_run and Reset the audio subsystem */
		writel(0x0, (AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3));
		writel(0xffffffff, (AUDIO_AUD_MISC_INIT));
		writel(0x0, (AUDIO_AUD_MISC_INIT));

	}

	return 0;

}

int spdif_post_test(int flags)
{
	int status = 0;
#ifdef BCM_OTP_CHECK_ENABLED
	u32 ip_enable_status = 0;
#endif
	int ret = 0;
	char *s = getenv("board_name");

	if (s) {
		if ((!strcmp(s, PEGASUS_XMC_BOARD)) ||
				(!strcmp(s, PEGASUS_17MM_BOARD))) {
			post_log("SPDIF interface is not available on %s\n", s);
			return BCM_NO_IP;
		}
	} else  {
		post_log("Unable to get board name\n");
		ret = -1;
		return ret;
	}

#ifdef BCM_OTP_CHECK_ENABLED
	/* Checking IP status with OTP read */
	ret = otp_read_audio_stat(&ip_enable_status);
	if (ret != 0) {
		post_log("\nOTP read failed !! Exiting from this Diag !!\n");
		return BCM_NO_IP;
	}

	if (!ip_enable_status) {
		post_log("\nIP is disabled !! Exiting from this Diag !!\n");
		return BCM_NO_IP;
	}
#endif

	post_log("Running SPDIF diags...\n");

	status = audio_spdif_out_test();

	return status;
}
#endif /* CONFIG_POST && CONFIG_SYS_POST_SPDIF */
