/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * This file contains routines to perform diags
 * for I2S interface.
 *
 */

#include <common.h>
#include <post.h>
#include <i2c.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2S
#include "../../../../drivers/sound/audio_data.h"
#include "../../../../drivers/sound/northstar2/audio_apis.h"
#include "../../../../include/brcm_i2c.h"

#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

#define I2S_DEBUG 0
extern int i2c_init_done[2];

/* Default Register Value */
/* extern uint32_t right_channel_samples[480]; */
/* extern uint32_t left_channel_samples[480]; */
/* extern uint32_t stereo_interleaved_data[960]; */
/* extern uint32_t right_channel_samples_0_1[480]; */
/* extern uint32_t left_channel_samples_0_1[480]; */
/* extern uint32_t left_data[1574888]; */
/* extern uint32_t right_data[1574888]; */

static inline void probe_sfp_device(unsigned char ctrl_byte)
{
	i2c_write_mux_ctrl_byte(0x70, 0x0);

	i2c_write_mux_ctrl_byte(0x70, ctrl_byte);
	udelay(10000);
	i2c_probe(I2C_SFP_ADDR);
}

static void probe_i2c_devices(void)
{
	u8 data[2];

	i2c_set_bus_num(1);

	i2c_probe(I2C_IOMUX_ADDR_U73);

	i2c_write_mux_ctrl_byte(0x70, 0x7);
	udelay(10000);
	i2c_probe(I2C_SFP_CNTL_U74);

	/* programming SFP CNTL */
	data[0] = 0x00;
	data[1] = 0x00;
	i2c_write_ctrl_bytes(0x24, data, 2);

	i2c_set_bus_num(0);

	i2c_probe(CONFIG_SYS_I2C_PCF8574_U86);
	i2c_probe(CONFIG_SYS_I2C_PCF8574_U92);
	i2c_probe(CONFIG_SYS_I2C_PCF8574_U100);
	i2c_probe(CONFIG_SYS_I2C_PCF9505_U41);
	i2c_probe(CONFIG_SYS_I2C_PCF9505_U40);
	i2c_probe(I2C_IOMUX_ADDR_U68);

	probe_sfp_device(0x7);
	probe_sfp_device(0x6);
	probe_sfp_device(0x5);
	probe_sfp_device(0x4);
}

/* For I2S0 on daughter card BCM9I2SDDC  */
int audio_i2s0_output_to_stereo_codec_test_0(void)
{
	uint32_t i;
	uint32_t smbus_slave_pca9673_address, smbus_slave_ti_codec_address;
	int slave_present;
	uint32_t control_register;
	uint32_t control_byte;
	int smbus_status;
	uint32_t *stereo_interleaved_l, *stereo_interleaved_r;
	uint8_t wbuf[2];
#if I2S_DEBUG
	uint8_t buf;
#endif
	char ch;

/* extern uint32_t right_channel_samples[480]; */
/* extern uint32_t left_channel_samples[480]; */

	stereo_interleaved_l = (uint32_t *)0x87000000;
	for (i = 0; i < 480; i++) {
		*stereo_interleaved_l = left_channel_samples[i];
		stereo_interleaved_l++;
	}

	stereo_interleaved_l = (uint32_t *)0x87000000;

	stereo_interleaved_r = (uint32_t *)0x88000000;
	for (i = 0; i < 480; i++) {
		*stereo_interleaved_r = right_channel_samples[i];
		stereo_interleaved_r++;
	}

	stereo_interleaved_r = (uint32_t *)0x88000000;

	/* This test uses Stereo interleaved data to be played back from DDR */
	audio_sw_reset();

	/* Configuring CRMU PLL CONTROL REGISTER */
	audio_gen_pll_pwr_on(1);
	/* user macro set to 48kHz clock */
	audio_gen_pll_group_id_config(1, 0x00000046, 0x000C75FF, 0x000000D8,
				      4, 0, 8, 2, 0, 0);
	post_log("Audio PLL configuration done\n");

	/* Audio BF control source channel configuration */
	post_log("Source Channel 0 under configuration\n");
	audio_fmm_bf_ctrl_source_cfg(0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
				     0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1);
	post_log("Source Channel 0 configuration done\n");

	/* Audio I2S0 Source Buffer Addr Config for Stereo Interleaved data */
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR,
		      0x87000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR,
		      0x8700077F, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR,
		      0x8700077F, 4);

	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR,
		      0x88000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_WRADDR,
		      0x8800077F, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_BASEADDR,
		      0x88000000, 4);
	cpu_wr_single(AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_ENDADDR,
		      0x8800077F, 4);

	post_log("Source Channel Ring Buffer Addresses Configured\n");

	/* Audio BF control source channel 0 configuration
	I2S0 Stream output configuration */
	audio_i2s_stream_config_samp_count(0, 1, 0, 8, 0, 1, 0, 0, 0, 1);

	audio_mclk_cfg(1, 2, 1);
	post_log("MCLK and PLLCLKSEL programming done\n");

/* I2S0 Out Config */
	audio_i2s_out_tx_config(1, 1, 0, 0, 1, 0, 24, 0, 0, 0, 2, 0, 0, 1);
	post_log("Done with I2S Out Configuration\n");

/* I2S Legacy Enable */
	audio_i2s_out_legacy_enable(1, 1);
	post_log("I2S port 1 (I2S channel 0) Enabled\n");

/* SFIFO Enable */
	audio_fmm_bf_ctrl_source_cfg(0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
				     0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1);

	audio_i2s_stream_config_samp_count(1, 1, 0, 8, 0, 1, 0, 0, 0, 1);

/* Configuring Codec registers on Daughter Card */

	smbus_slave_pca9673_address = I2C_SFP_CNTL_U74;
	smbus_slave_ti_codec_address = 0x18; /* I2C_TLV32X_I2S_DC */

	/* SOC-2564: Probing all i2c devices inorder to avoid
	 * intermittent TI codec not getting detected and
	 * noise issue
	 */
	if (!i2c_init_done[1])
		probe_i2c_devices();

	/* Speed 400KHz */
	smbus1_init(1);
	i2c_set_bus_num(1);

	/* Mux Channel 3 for I2S_DC_GPIO0/1/2 */
	i2c_probe(0x70);
	udelay(10000);
	i2c_write_mux_ctrl_byte(0x70, 0x7);
	udelay(10000);

	slave_present = smbus1_slave_presence(smbus_slave_pca9673_address);
	if (slave_present == 0) {
		post_log("I2C 16b IO (PCA9673PW) recognized as Slave\n");
	} else {
		post_log("I2C 16b IO (PCA9673PW) not recognized as Slave\n");
		return -1;
	}

	/* Drive the I2S_DC_GPIOs*/
	/*
	   I2S_DC_GPIO1, I2S_DC_GPIO2, I2S_DC_GPIO3, SLIC_RESET_L are connected
	   to Pins 17, 18, 19, 20 of 16-bit IO expander PCA9673PW respectively.
	   Drive these pins to enable external headset, handsfree speaker and
	   to reset the Codec.

	   I2S_DC_GPIO1 connects to EXT_HEADSET_EN pin on daughter card
	   I2S_DC_GPIO2 connects to CODEC_RST_B pin on daughter card
	   I2S_DC_GPIO3 connects to HANDSFREE_SPEAKER_EN pin on daughter card
	*/

/* Drive U24 Port2 bits 5 to 7 for enabling I2S_DC_GPIOs */
	wbuf[0] = 0xFF;
	wbuf[1] = 0x7F;
	smbus_status = smbus_write(smbus_slave_pca9673_address,
				   wbuf[0], wbuf[1]);

	/* Mux Channel 0 for I2S DC */
	/* Now check for the TI codec slave & program the codec regs */

	i2c_write_mux_ctrl_byte(0x70, 0x4);
	udelay(10000);

	slave_present = smbus1_slave_presence(smbus_slave_ti_codec_address);

	if (slave_present == 0) {
		post_log("TI Codec in daughter card recognized as Slave\n");
	} else {
		post_log("TI Codec in daughter card not recognized as Slave\n");
		return -1;
	}


/* start writing to the Stereo Codec registers - Page 0
   chosen for the following register accesses */
	control_register = 0x00;
	control_byte = 0x00 | (1 << 31);

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
			   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - Page Select Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x00, 1, &buf, 1);
	post_log("\nPage Select Register: 0x%x\n", buf);
#endif

/* page 0 - Register 7 - Codec Datapath Setup Register */
	control_register = 0x07;
	control_byte = 0x0A | (1 << 31); /* 00001010 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
			   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - Codec Datapath reg write\n");
	} else {
		post_log("SMBUS TRANS. FAIL - Codec Datapath reg write\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x07, 1, &buf, 1);
	post_log("\nCodec Datapath Register: 0x%x\n", buf);
#endif

/* this needs to be verified twice */
/* page 0 - register 9 - Audio Serial Interface Ctrl Register B
- program the audio data bit to be 24 bits and 256 clock mode */
	control_register = 0x09;
	control_byte = 0x20 | (1 << 31); /* 00101000 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - Serial Int. Ctrl Register B\n");
	} else {
		post_log("SMBUS TRANS. FAIL - Serial Int. Ctrl Register B\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x09, 1, &buf, 1);
	post_log("\nAudio Serial Interface Ctrl Register: 0x%x\n", buf);
#endif

/* page 0 - Register 13  - Headset Detection Register */
	control_register = 0x0d;
	control_byte = 0x82 | (1 << 31);

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - Headset Detection Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - Headset Detection Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0d, 1, &buf, 1);
	post_log("\nHeadset Detection Register 0x%x\n", buf);
#endif

/* page 0 - Register 14  - Make the Stereo Output driver - fully differential */
	control_register = 0x0E;
	control_byte = 0x40 | (1 << 31);

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - Stereo Output driver\n");
	} else {
		post_log("SMBUS TRANS. FAIL - Stereo Output driver\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0E, 1, &buf, 1);
	post_log("\nStereo Output Driver Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 37 - Power up Left and right DACs */
	control_register = 37;
	control_byte = 0xC0 | (1 << 31); /* 11000000 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
			   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - DACs Powered Up\n");
	} else {
		post_log("SMBUS TRANS. FAIL - DACs not powered up\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 37, 1, &buf, 1);
	post_log("\nDACs Power Ctrl Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 40 - VCM setting */
	control_register = 40;
	control_byte = 0x80 | (1 << 31); /* 11000000 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - VCM Setting Done\n");
	} else {
		post_log("SMBUS TRANS. FAIL - VCM Setting Done\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 40, 1, &buf, 1);
	post_log("\nVCM Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 41 - Route DAC to */
	control_register = 41;
	control_byte = 0x50 | (1 << 31); /* 01010000 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - DAC Routing Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - DAC Routing Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 41, 1, &buf, 1);
	post_log("\nDAC Routing Register: 0x%x\n", buf);
#endif

/* page0 - Register 43 - Left DAC Volume Ctrl Register */
	control_register = 43;
	control_byte = 0x00 | (1 << 31);

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - L DAC Vol Ctrl Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - L DAC Vol Ctrl Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 43, 1, &buf, 1);
	post_log("\nLEFT DAC Volume Ctrl Register: 0x%x\n", buf);
#endif

/* page0 - Register 44 - Right DAC Volume Ctrl Register */
	control_register = 44;
	control_byte = 0x00 | (1 << 31);

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - R DAC Vol Ctrl Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - R DAC Vol Ctrl Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 44, 1, &buf, 1);
	post_log("\nRight DAC Volume Ctrl Register: 0x%x\n", buf);
#endif

/* Left LOP/M - Register 86 */
	control_register = 86;
	control_byte = 0x0B | (1 << 31); /* 00001010 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - L LOP Vol Ctrl Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - L LOP Vol Ctrl Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 86, 1, &buf, 1);
	post_log("\nLeft LOP Volume Ctrl Register: 0x%x\n", buf);
#endif

/* Right LOP/M - Register 93 */
	control_register = 93;
	control_byte = 0x0B | (1 << 31); /* 00001010 */

	smbus_status = smbus_write(smbus_slave_ti_codec_address,
				   control_register, control_byte);
	if (smbus_status == 0) {
		post_log("SMBUS TRANS. PASS - R LOM Vol Ctrl Register\n");
	} else {
		post_log("SMBUS TRANS. FAIL - R LOM Vol Ctrl Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 93, 1, &buf, 1);
	post_log("\nRight LOM Volume Ctrl Register: 0x%x\n", buf);
#endif

/* PLAY RUN */
	audio_start_dma_read(0, 1, 1);
	post_log("Play Run Enabled\n");
	post_log("End of test: check for 1 KHz audio on the left channel and ");
	post_log("2 KHz audio on the right channel - audio_i2s0_output_test\n");

	post_log("\nConfirm audio status (Y/N) to exit the test\n");

	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y') &&
		 (ch != 'n') && (ch != 'N'));
	if ((ch == 'n') || (ch == 'N')) {
		post_log("\nError. Exiting the test\n");
		writel(0xffffffff, (AUD_MISC_INIT + I2S_ROOT));
		writel(0x0, (AUD_MISC_INIT + I2S_ROOT));
		return -1;
	} else {
		/* Stop play_run and Reset the audio subsystem */
		writel(0x0, (AUD_FMM_BF_CTRL_SOURCECH_CTRL0 + I2S_ROOT));
		writel(0xffffffff, (AUD_MISC_INIT + I2S_ROOT));
		writel(0x0, (AUD_MISC_INIT + I2S_ROOT));
	}

	return TEST_PASS;
}

int audio_i2s_test(void)
{
	int status;
	status = 0;
	char input;

	post_log("\nTesting I2S interface w/ TI codec on BCM9I2SDDC\n");
	post_log("Press 0 to select Audio Output;");

	do {
		input = (char)serial_getc();
	} while ((input != '0') && (input != '1'));

	if (input == '0')
		status = audio_i2s0_output_to_stereo_codec_test_0();

	if (input == '1')
		status = 0;

	return status;
}

static void help(void)
{
	post_log("\n ----------------------\n");
	post_log("| AUDIO DIAG HELP MENU |\n");
	post_log(" ----------------------\n");
}

/******* Internal test function start ********/
int AUDIO_post_test(int flags)
{
	int status = 0;
	status = TEST_PASS;

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	if (TEST_FAIL == audio_i2s_test())
		status = TEST_FAIL;

	return status;
}

#endif /* CONFIG_POST && CONFIG_SYS_POST_AUDIO */
