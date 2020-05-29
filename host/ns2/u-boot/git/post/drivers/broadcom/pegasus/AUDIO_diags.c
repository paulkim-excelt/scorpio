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
#include "../../../../drivers/sound/pegasus/audio_apis.h"
#include <asm/arch-bcm_pegasus/socregs.h>
#include <asm/arch/bcm_otp.h>

#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP 2

#define I2S_DEBUG 0

#define STEREO_INTERLEAVED_L 0x87000000
#define STEREO_INTERLEAVED_R 0x88000000

/*extern int i2c_init_done;*/
extern int i2c_init_done[4];
extern int i2c_pca9673_write(u8 devaddr, u8 * buffer);
extern int i2c_pca9673_read(u8 devaddr, u8 * buffer);
extern int i2c_write_mux_ctrl_byte(u8 devaddr, u8 ctrl_byte);
extern int i2c_read_mux_ctrl_byte(u8 devaddr);
extern int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len);
extern int i2c_set_bus_num(unsigned int bus);
extern void i2c_init(int speed, int slaveadd);
extern int i2c_probe(uchar chip);
extern int smbus1_poll(void);
extern int smbus_poll(void);
extern int pca9555_write(uint32_t port, uint32_t slave_addr, uint32_t reg,
			 uint32_t byte1, uint32_t byte2);

unsigned int pca9555_slave_read(uint32_t slave_address, int reg_add);

/* Default Register Value */
#if 0
extern uint32_t right_channel_samples[480];
extern uint32_t left_channel_samples[480];
extern uint32_t stereo_interleaved_data[960];
extern uint32_t right_channel_samples_0_1[480];
extern uint32_t left_channel_samples_0_1[480];
/*extern uint32_t left_data[1574888];*/
/*extern uint32_t right_data[1574888];*/
#endif

static void probe_i2c_devices(void)
{
	/* Not yet implemented for Pegasus, if required need to implement */
#if 0
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

	/* Programming MUX */
	i2c_write_mux_ctrl_byte(0x70, 0x0);

	i2c_write_mux_ctrl_byte(0x70, 0x7);
	udelay(10000);
	i2c_probe(I2C_SFP_ADDR);

	i2c_write_mux_ctrl_byte(0x70, 0x0);

	i2c_write_mux_ctrl_byte(0x70, 0x6);
	udelay(10000);
	i2c_probe(I2C_SFP_ADDR);

	i2c_write_mux_ctrl_byte(0x70, 0x0);

	i2c_write_mux_ctrl_byte(0x70, 0x5);
	udelay(10000);
	i2c_probe(I2C_SFP_ADDR);

	i2c_write_mux_ctrl_byte(0x70, 0x0);

	i2c_write_mux_ctrl_byte(0x70, 0x4);
	udelay(10000);
	i2c_probe(I2C_SFP_ADDR);
#endif
}
	
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


/* For I2S0 on daughter card BCM9I2SDDC  */
int audio_i2s0_output_to_stereo_codec_test_0(int channel)
{
	uint32_t i;
	uint32_t smbus_slave_pca9555_address, smbus_slave_TIcodec_address;
	int slave_present;
	uint32_t control_register;
	uint32_t control_byte;
	int smbus_status;
	uint32_t *stereo_interleaved_l, *stereo_interleaved_r;
#if I2S_DEBUG
	uint8_t buf;
#endif
	char ch;
	int gpio_smbus;
	int gpio1_lo, gpio1_hi, gpio2_lo, gpio2_hi;
	int port = channel+1;

	/* port is either 1 or 2 equivalent to channels 0 & 1 */

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


	enable_i2s();

	//This test uses Stereo interleaved data to be played back from DDR
	audio_sw_reset();

	if (port == 1) {
		gpio_smbus = 3;
		gpio1_lo = 0xff;
		gpio1_hi = 0x8f;
		gpio2_lo = 0xff;
		gpio2_hi = 0xdf;
	} else {
		gpio_smbus = 1;
		gpio1_lo = 0x8f;
		gpio1_hi = 0xff;
		gpio2_lo = 0xdf;
		gpio2_hi = 0xff;
	}

	//user macro set to 48kHz clock
	audio_gen_pll_group_id_config(1, 0x00000046, 0x000C75FF, 0x000000D8, 4,
				      0, 8, 2, channel, 0);
#if I2S_DEBUG
	post_log("Audio PLL configuration done\n");
#endif

	//Audio BF control source channel configuration
#if I2S_DEBUG
	post_log("Source Channel 0 under configuration\n");
#endif
	audio_fmm_bf_ctrl_source_cfg(0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
				     0, 0, 0, 0, 1, 0, port);
#if I2S_DEBUG
	post_log("Source Channel 0 configuration done\n");
#endif

	/* Audio I2S0 Source Buffer Address Configuration for
	 * Stereo Interleaved data
	 */
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR,
		      0x8700077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR,
		      0x8700077F, 4);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_WRADDR,
		      0x8800077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_BASEADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_ENDADDR,
		      0x8800077F, 4);

	/* Audio I2S1 Source Buffer Address Configuration for
	 * Stereo Interleaved data
	 */
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_RDADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_WRADDR,
		      0x8700077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_ENDADDR,
		      0x8700077F, 4);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_RDADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_WRADDR,
		      0x8800077F, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_BASEADDR,
		      0x88000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_ENDADDR,
		      0x8800077F, 4);

#if I2S_DEBUG
	post_log("Source Channel Ring Buffer Addresses Configured\n");
#endif

	//Audio BF control source channel 0 configuration
	// I2S0 Stream output configuration */
	audio_i2s_stream_config_samp_count(0, 1, 0, 8, 0, 1, 0, 0, 0, port);

	audio_mclk_cfg(1, 2, 1);
#if I2S_DEBUG
	post_log("MCLK and PLLCLKSEL programming done\n");
#endif

	/* I2S0 Out Config */
	audio_i2s_out_tx_config(1, 1, 0, 0, 1, 0, 24, 0, 0, 0, 2, 0, 0, port);
#if I2S_DEBUG
	post_log("Done with I2S Out Configuration\n");
#endif

	/* I2S Legacy Enable */
	audio_i2s_out_legacy_enable(1, port);
#if I2S_DEBUG
	post_log("I2S port %d (I2S channel %d) Enabled\n", port, channel);
#endif

	/* enable i2s */
	enable_i2s();

	/* SFIFO Enable */
	audio_fmm_bf_ctrl_source_cfg(0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0,
				     0, 0, 0, 0, 1, 1, port);

	audio_i2s_stream_config_samp_count(1, 1, 0, 8, 0, 1, 0, 0, 0, port);

/* Configuring Codec registers on Daughter Card */

	smbus_slave_pca9555_address = 0x20;
	smbus_slave_TIcodec_address = 0x18;	//I2C_TLV32X_I2S_DC

	if (port == 1) {
		/* SOC-2564: Probing all i2c devices inorder to avoid
		 * intermittent TI codec not getting detected and noise issue
		 */
		if (!i2c_init_done[3]) {
#if I2S_DEBUG
			post_log("probe_i2c_devices\n");
#endif
			probe_i2c_devices();
		}

		smbus3_init(1);		/* Speed 400KHz */

		if (i2c_set_bus_num(3))	/* Pegasus SVK uses i2c3 for I2S0 */
			post_log("Error selecting i2c bus 3 (port 1)\n");
#if I2S_DEBUG
		else
			post_log("selecting i2c bus 3 (port 1)\n");
#endif

		slave_present = smbus3_slave_presence(smbus_slave_pca9555_address);
		if (slave_present == 0) {
#if I2S_DEBUG
			post_log
			 ("I2C3 16-bit IO expander (PCA9555PW) recognized as a Slave\n\n");
#endif
		} else {
			post_log
				("I2C3 16-bit IO expander (PCA9555PW) not recognized as a Slave\n\n");
			return -1;
		}
	} else {
		/* SOC-2564: Probing all i2c devices inorder to avoid
		 * intermittent TI codec not getting detected and noise issue
		 */
		if (!i2c_init_done[1]) {
#if I2S_DEBUG
			post_log("probe_i2c_devices\n");
#endif
			probe_i2c_devices();
		}

		smbus1_init(1);		/* Speed 400KHz */

		if (i2c_set_bus_num(1))	/* Pegasus SVK uses i2c1 for I2S0 */
			post_log("Error selecting i2c bus 1 (port 2)\n");
#if I2S_DEBUG
		else
			post_log("selecting i2c bus 1 (port 2)\n");
#endif

		slave_present = smbus1_slave_presence(smbus_slave_pca9555_address);
		if (slave_present == 0) {
#if I2S_DEBUG
			post_log
			 ("I2C1 16-bit IO expander (PCA9555PW) recognized as a Slave\n\n");
#endif
		} else {
			post_log
				("I2C1 16-bit IO expander (PCA9555PW) not recognized as a Slave\n\n");
			return -1;
		}
	}

	/*
	 * reset the TI codec using a GPIO from u47 pca9555, i2c3 addr 0x20
	 *
	 * The following GPIOs are used by the TI codec daughter card:
	 * I/O1_4 I2S0_DC_GPIO0   External headset enable
	 * I/O1_5 I2S0_DC_GPIO1   TI codec reset
	 * I/O1_6 I2S0_DC_GPIO2   Handsfree speaker enable
	 */


	/* set all GPIOa reset to 1 */
	pca9555_write(gpio_smbus, smbus_slave_pca9555_address, 2, 0xff, 0xff);

	/* set audio GPIOs to output */
	pca9555_write(gpio_smbus, smbus_slave_pca9555_address, 6,
		      gpio1_lo, gpio1_hi);

	/* set audio GPIOs outputs high */
#if I2S_DEBUG
	post_log("writing all gpios to 1\n");
#endif
	pca9555_write(gpio_smbus, smbus_slave_pca9555_address, 2, 0xff, 0xff);
	udelay(1000);

	/* set reset GPIO to 0 */
#if I2S_DEBUG
	post_log("writing codec reset gpio to 0\n");
#endif
	pca9555_write(gpio_smbus, smbus_slave_pca9555_address, 2,
		      gpio2_lo, gpio2_hi);
	udelay(100000);

	/* set audio GPIOs to output */
#if I2S_DEBUG
	post_log("writing all gpios to 1\n");
#endif
	pca9555_write(gpio_smbus, smbus_slave_pca9555_address, 2, 0xff, 0xff);

	/* Mux Channel 0 for I2S DC */
	/* Now check for the TI codec slave presence and program the codec regs */

	/* both ports 1 & 2 use i2c3 */

	/* SOC-2564: Probing all i2c devices inorder to avoid
	 * intermittent TI codec not getting detected and noise issue
	 */
	if (!i2c_init_done[3])
		probe_i2c_devices();

	smbus3_init(1);		/* Speed 400KHz */

	if (i2c_set_bus_num(3))	/* Pegasus SVK uses i2c bus:3 for I2S0 */
		post_log("Error selecting i2c bus 3 (port 1)\n");
#if I2S_DEBUG
	else
		post_log("selecting i2c bus 3 (port 1)\n");
#endif

	if (i2c_set_bus_num(3))	/* Pegasus SVK uses i2c bus:3 for I2S0 */
		post_log("Error selecting i2c bus 3\n");
#if I2S_DEBUG
	else
		post_log("selecting i2c bus 3\n");
#endif

#if I2S_DEBUG
	post_log("i2c probe 0x70\n");
#endif
	if (i2c_probe(0x70))
		post_log("Error probing i2c 0x70\n");
#if I2S_DEBUG
	else
		post_log("probed i2c 0x70 OK\n");
#endif
	udelay(10000);

	if (port == 1)
		i2c_write_mux_ctrl_byte(0x70, 0x4);
	else
		i2c_write_mux_ctrl_byte(0x70, 0x5);

	udelay(10000);

#if I2S_DEBUG
	post_log("smbus3_slave_presence\n");
#endif
	slave_present = smbus3_slave_presence(smbus_slave_TIcodec_address);

	if (slave_present == 0) {
#if I2S_DEBUG
		post_log
		    ("TI Stereo Codec in daughter card recognized as a Slave\n\n");
#endif
	} else {
		post_log
		    ("TI Stereo Codec in daughter card not recognized as a Slave\n\n");
		return -1;
	}

/* start writing to the Stereo Codec registers - Page 0 chosen for the following register accesses */
	control_register = 0x00;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - Page Select Register\n");
#endif
	} else {
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x00, 1, &buf, 1);
	post_log("\nPage Select Register: 0x%x\n", buf);
#endif

/* page 0 - Register 7 - Codec Datapath Setup Register */
	control_register = 0x07;
	control_byte = 0x0A | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Codec Datapath register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Codec Datapath register write not succesful\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x07, 1, &buf, 1);
	post_log("\nCodec Datapath Register: 0x%x\n", buf);
#endif

// this needs to be verified twice
/* page 0 - register 9 - Audio Serial Interface Control Register B - program the audio data bit to be 24 bits and 256 clock mode */
	control_register = 0x09;
	control_byte = 0x20 | (1 << 31);	//00101000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Audio Serial Interface Control Register B\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Audio Serial Interface Control Register B\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x09, 1, &buf, 1);
	post_log("\nAudio Serial Interface Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 13  - Headset Detection Register */
	control_register = 0x0d;
	control_byte = 0x82 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Headset Detection Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Headset Detection Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0d, 1, &buf, 1);
	post_log("\nHeadset Detection Register  0x%x\n", buf);
#endif

/* page 0 - Register 14  - Make the Stereo Output driver - fully differential */
	control_register = 0x0E;
	control_byte = 0x40 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Stereo Output driver - fully differential\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Stereo Output driver - fully differential\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0E, 1, &buf, 1);
	post_log("\nStereo Output Driver Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 37 - Power up Left and right DACs */
	control_register = 37;
	control_byte = 0xC0 | (1 << 31);	//11000000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - DACs Powered Up\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - DACs not powered up\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 37, 1, &buf, 1);
	post_log("\nDACs Power Control Register: 0x%x\n", buf);
#endif

/* Page 0 - REgister 40 - VCM setting */
	control_register = 40;
	control_byte = 0x80 | (1 << 31);	//11000000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - VCM Setting Done\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - VCM Setting Done\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 40, 1, &buf, 1);
	post_log("\nVCM Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 41 - Route DAC to */
	control_register = 41;
	control_byte = 0x50 | (1 << 31);	//01010000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - DAC Routing Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - DAC Routing Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 41, 1, &buf, 1);
	post_log("\nDAC Routing Register: 0x%x\n", buf);
#endif

/* page0 - Register 43 - Left DAC Volume Control Register */
	control_register = 43;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Left DAC Volume Control REgister\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Left DAC Volume Control REgister\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 43, 1, &buf, 1);
	post_log("\nLEFT DAC Volume Control Register: 0x%x\n", buf);
#endif

/* page0 - Register 44 - Right DAC Volume Control Register */
	control_register = 44;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Right DAC Volume Control REgister\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Right DAC Volume Control REgister\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 44, 1, &buf, 1);
	post_log("\nRight DAC Volume Control Register: 0x%x\n", buf);
#endif

/* Left LOP/M - Register 86 */
	control_register = 86;
	control_byte = 0x0B | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Left LOPP Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Left LOP Volume Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 86, 1, &buf, 1);
	post_log("\nLeft LOP Volume Control Register: 0x%x\n", buf);
#endif

/* Right LOP/M - Register 93 */
	control_register = 93;
	control_byte = 0x0B | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Right LOM Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Right LOM Volume Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 93, 1, &buf, 1);
	post_log("\nRight LOM Volume Control Register: 0x%x\n", buf);
#endif

/* These register control settings are commented out in cygnus * /

//page0 - Register 57 - DAC_R1 to HPLCOM Volume Control Register
	control_register = 57;
	control_byte = 0x80;

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_R1 to HPLCOM Volume Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_R1 to HPLCOM Volume Control Register\n");
        	return -1;
	}

//page0 - REgister 61 - DAC_L1 to HPROUT Volume Control Register
      	control_register = 61;
	control_byte = 0x80;

	smbus_status = smbus4_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_L1 to HPROUT Volume Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_L1 to HPROUT Volume Control Register\n");
        	return -1;
	}

//page0 - REgister 71 - DAC_R1 to HPRCOM Volume Control Register
	control_register = 71;
	control_byte = 0x80;

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_R1 to HPRCOM Volume Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_R1 to HPRCOM Volume Control Register\n");
        	return -1;
	}

//HPLOUT and HPROUT Level Control Register Programming
//HPLOUT - Register 51
	control_register = 51;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - HPLOUT Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -HPLOUT Volume Control Register\n");
        	return -1;
	}

//HPLCOM - Register 58
	control_register = 58;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - HPLCOM Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -HPLCOM Volume Control Register\n");
        	return -1;
	}

//HPLOUT and HPROUT Level Control Register Programming
//HPROUT - Register 65
	control_register = 65;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - HPROUT Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -HPROUT Volume Control Register\n");
	        return -1;
	}

//HPRCOM - Register 72
	control_register = 72;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - HPRCOM Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -HPRCOM Volume Control Register\n");
        	return -1;
	}

//DAC_L1 to MONO LOP
	control_register = 75;
	control_byte = 0x80; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_L1 to MONO LOPr\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_L1 to MONO LOP\n");
        	return -1;
	}

//MONO LOP - Register 79
	control_register = 79;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - MONO LOP Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -MONO LOP Volume Control Register\n");
        	return -1;
	}

//DAC_L1 to LEFT_LOP
	control_register = 82;
	control_byte = 0x80; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_L1 to LEFT_LOP\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_L1 to LEFT_LOP\n");
 	       return -1;
	}

 //Left LOP - Register 86
	control_register = 86;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - Left LOPP Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -Left LOP Volume Control Register\n");
        	return -1;
	}

//DAC_R1 to Right_LOM
      	control_register = 92;
	control_byte = 0x80; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - DAC_R1 to Right_LOM\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -DAC_R1 to Right_LOM\n");
        	return -1;
	}

//Right LOM - Register 93
	control_register = 93;
	control_byte = 0x0b; //00001011

	smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);
	if (smbus_status == 0)
	{
		post_log("SMBUS TRANSACTION SUCCESSFUL - Right LOM Control Register\n");
	}
	else
	{
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL -Right LOM Volume Control Register\n");
       		return -1;
	}
//Just have an eye for the page 0 - REgister 102 - clock division register
*/
/* PLAY RUN */
	audio_start_dma_read(0, 1, port);
	post_log("Play Run Enabled for port %d\n", channel);
	post_log
	    ("End of test : check for 1 KHz audio on the left channel and 2 KHz audio on the right channel - audio_i2s0_output_test\n");

	post_log("\nConfirm audio status (Y/N) to exit the test\n");

	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y')
		 && (ch != 'n') && (ch != 'N'));
	if ((ch == 'n') || (ch == 'N')) {
		post_log("\nError. Exiting the test\n");
		writel(0xffffffff, (AUDIO_AUD_MISC_INIT));
		writel(0x0, (AUDIO_AUD_MISC_INIT));
		return -1;
	} else {
		/* Stop play_run and Reset the audio subsystem */
		writel(0x0, (AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL0));
		writel(0xffffffff, (AUDIO_AUD_MISC_INIT));
		writel(0x0, (AUDIO_AUD_MISC_INIT));

	}

	return TEST_PASS;
}

#if 0
int audio_i2s0_mic_in(void)
{
	uint32_t i;
	uint32_t smbus_slave_pca9673_address, smbus_slave_TIcodec_address;
	int slave_present, status;
	uint32_t control_register;
	uint32_t control_byte;
	int smbus_status;
	uint8_t rbuf[2], wbuf[2];	// buf;
	char ch;

	post_log("\nPlease provide audio input via J4000 (top port ,3.5 mm jack) and hear the same from the bottom port 3.5 mm jack (J4000 connector) Ready(Y/N): ");
	post_log
	    ("\nPlease provide audio input and hear the same from the 3.5 mm jack (J400 connector) Ready(Y/N): ");
	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y'));

	//This test uses Stereo interleaved data to be played back from DDR
	audio_sw_reset();
	//Configuring CRMU PLL CONTROL REGISTER
	audio_gen_pll_pwr_on(1);
	//user macro set to 48kHz clock
	audio_gen_pll_group_id_config(1, 0x00000046, 0x000C75FF, 0x000000D8, 4,
				      0, 8, 2, 0, 0);
	post_log("Audio PLL configuration done\n");

	audio_mclk_cfg(1, 2, 1);
	post_log("MCLK and PLLCLKSEL programming done\n");

	//I2S0 Out Config
	//audio_i2s_out_tx_config(1,1,0,0,1,0,24,0,0,0,2,0,0,4);
	audio_i2s_out_tx_config(1, 1, 0, 0, 1, 0, 24, 0, 0, 0, 2, 0, 0, 2);
	post_log("Done with I2S Out Configuration\n");

/* Configuring Codec registers on Daughter Card */

	smbus1_init(1);		//Speed 400KHz
	smbus_slave_pca9673_address = I2C_SFP_CNTL_U74;
	smbus_slave_TIcodec_address = 0x18;	//I2C_TLV32X_I2S_DC

	i2c_set_bus_num(1);
	i2c_probe(0x70);

	/* Check if i2c_init is done. Check for presence of slaves */
	if (!i2c_init_done[1]) {
		post_log("I2C Init..");
		i2c_init(0, 0);
		udelay(10000);
		i2c_probe(0x70);
		udelay(10000);
		if (!i2c_init_done[1]) {
			post_log("Error: I2C not initialized\n");
			return -1;
		}
	}
#if I2S_DEBUG
	post_log("I2C_INIT: %d\n", i2c_init_done[1]);
#endif

	/* Mux Channel 3 for I2S_DC_GPIO0/1/2 */
	//      post_log("Selecting MUX channel 3 for SFP CNTL\n");
	//      udelay(1000000);
	//      i2c_write_mux_ctrl_byte (0x70, 0x0);
	//      udelay(1000000);
	i2c_write_mux_ctrl_byte(0x70, 0x7);
	udelay(10000);
	post_log("Selecting MUX channel 3 for I2S GPIO\n");

	slave_present = smbus1_slave_presence(smbus_slave_pca9673_address);
	if (slave_present == 0) {
		post_log
		    ("I2C 16-bit IO expander (PCA9673PW) recognized as a Slave\n\n");
	} else {
		post_log
		    ("I2C 16-bit IO expander (PCA9673PW) not recognized as a Slave\n\n");
		return -1;
	}

	/* Drive the I2S_DC_GPIOs */
	/* 
	   For Pegasus:
	   I2S_DC_GPIO1, I2S_DC_GPIO2, I2S_DC_GPIO3, SLIC_RESET_L are connected 
	   to Pins 17, 18, 19, 20 of 16-bit IO expander PCA9673PW respectively.
	   Drive these pins to enable external headset, handsfree speaker and 
	   to reset the Codec.

	   I2S_DC_GPIO1 connects to EXT_HEADSET_EN pin on daughter card
	   I2S_DC_GPIO2 connects to CODEC_RST_B pin on daughter card
	   I2S_DC_GPIO3 connects to HANDSFREE_SPEAKER_EN pin on daughter card
	 */
	/* Active low Reset --drive to 0, delay and drive to 1 */
	status = i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
#if I2S_DEBUG
	post_log("STATUS: %d PCA9673 READ START: %x..%x\n", status, rbuf[0],
		 rbuf[1]);
#endif
	wbuf[0] = rbuf[0];
	wbuf[1] = rbuf[1] & (~(1 << 5));	//drive reset to 0 
	status = i2c_pca9673_write((uint8_t) smbus_slave_pca9673_address, wbuf);
	udelay(100000);
	status = i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	wbuf[0] = rbuf[0];
	wbuf[1] = rbuf[1] | (1 << 5);	//drive reset to 1
	status = i2c_pca9673_write((uint8_t) smbus_slave_pca9673_address, wbuf);

#if I2S_DEBUG
	post_log("TI stereo codec on the daughter card has been reset\n");
	i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	post_log("STATUS: %d I2S_DC_GPIO2: RESET pin: %x..%x\n", status,
		 rbuf[0], rbuf[1]);
#endif
	status = i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	wbuf[0] = rbuf[0];
	wbuf[1] = rbuf[1] | (1 << 4);	//External Headset Enable
	//wbuf[1] = rbuf[1] & (~(1 << 4)); //External Headset Enable
	status = i2c_pca9673_write((uint8_t) smbus_slave_pca9673_address, wbuf);

#if I2S_DEBUG
	post_log("External Headset Enable has been asserted 1\n");
	i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	post_log("STATUS: %d I2S_DC_GPIO1: EXT Head Set En pin: %x..%x\n",
		 status, rbuf[0], rbuf[1]);
#endif
	status = i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	wbuf[0] = rbuf[0];
	wbuf[1] = rbuf[1] | (1 << 6);	//Handsfree Speaker Enable
	//wbuf[1] = rbuf[1] & (~(1 << 6)); //Handsfree Speaker Enable
	status = i2c_pca9673_write((uint8_t) smbus_slave_pca9673_address, wbuf);

#if I2S_DEBUG
	post_log("Handsfree Speaker Enable has been asserted 1\n");
	i2c_pca9673_read((uint8_t) smbus_slave_pca9673_address, rbuf);
	post_log("STATUS: %d I2S_DC_GPIO3: Handsfree speaker en pin: %x..%x\n",
		 status, rbuf[0], rbuf[1]);
#endif

	/* Mux Channel 0 for I2S DC */
	/* Now check for the TI codec slave presence and program the codec regs */

	//        i2c_write_mux_ctrl_byte (0x70, 0x0);
	//        udelay(100000);
	post_log("Selecting MUX channel 0 for I2S DC\n");

	i2c_write_mux_ctrl_byte(0x70, 0x4);
	udelay(100000);

	slave_present = smbus1_slave_presence(smbus_slave_TIcodec_address);

	if (slave_present == 0) {
		post_log
		    ("TI Stereo Codec in daughter card recognized as a Slave\n\n");
	} else {
		post_log
		    ("TI Stereo Codec in daughter card not recognized as a Slave\n\n");
		return -1;
	}

/* start writing to the Stereo Codec registers - Page 0 chosen for the following register accesses */
	control_register = 0x00;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Page Select Register\n");
#endif
	} else {
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x00, 1, &buf, 1);
	post_log("\nPage Select Register: 0x%x\n", buf);
#endif

/* page 0 - Register 7 - Codec Datapath Setup Register */
	control_register = 0x07;
	control_byte = 0x0A | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Codec Datapath register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Codec Datapath register write not succesful\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x07, 1, &buf, 1);
	post_log("\nCodec Datapath Register: 0x%x\n", buf);
#endif

/* page 0 - Register 8 -Audio Serial Data Interface Control Register A (for Mic)*/
	control_register = 0x08;
	control_byte = 0x03 | (1 << 31);	//

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Audio Serial Data Interface Control Register A\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Audio Serial Data Interface Control Register A\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x08, 1, &buf, 1);
	post_log
	    ("\nAudio Serial Data Interface Control Register A Register: 0x%x\n",
	     buf);
#endif

// this needs to be verified twice
/* page 0 - register 9 - Audio Serial Interface Control Register B - program the audio data bit to be 24 bits and 256 clock mode */
	control_register = 0x09;
	control_byte = 0x20 | (1 << 31);	//00101000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Audio Serial Interface Control Register B\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Audio Serial Interface Control Register B\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x09, 1, &buf, 1);
	post_log("\nAudio Serial Interface Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 12  - Digital filter control Register */
	control_register = 12;
	control_byte = 0xa0 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Digital filter control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Digital filter control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 12, 1, &buf, 1);
	post_log("\nDigital filter control Register  0x%x\n", buf);
#endif

/* page 0 - Register 13  - Headset Detection Register */
	control_register = 0x0d;
	control_byte = 0x82 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Headset Detection Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Headset Detection Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0d, 1, &buf, 1);
	post_log("\nHeadset Detection Register  0x%x\n", buf);
#endif

/* page 0 - Register 14  - Make the Stereo Output driver - fully differential */
	control_register = 0x0E;
	control_byte = 0x40 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Stereo Output driver - fully differential\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Stereo Output driver - fully differential\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 0x0E, 1, &buf, 1);
	post_log("\nStereo Output Driver Register: 0x%x\n", buf);
#endif

/* Configuring Right and Left ADC */
/* page 0 - Register 15, 16 Left/Right ADC PGA Gain Control Register */
	control_register = 15;
	control_byte = 0x0 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);

	control_register = 16;
	control_byte = 0x0 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);

	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Left/Right ADC PGA Gain Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - Left/Right ADC PGA Gain Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 15, 1, &buf, 1);
	post_log("\nLeft ADC PGA Gain Control Register: 0x%x\n", buf);
	i2c_read(0x18, 16, 1, &buf, 1);
	post_log("\nRight ADC PGA Gain Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 19 LINE1L to Left ADC Control Register */
	control_register = 19;
	control_byte = 0x7c | (1 << 31);	//0x84

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - LINE1L to Left ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - LINE1L to Left ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 19, 1, &buf, 1);
	post_log("\nLINE1L to Left ADC Control Register: 0x%x\n", buf);
#endif
#if 0
/* page 0 - Register 24 LINE1L to Right ADC Control Register */
	control_register = 24;
	control_byte = 0x80 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - LINE1L to Right ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - LINE1L to Right ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 24, 1, &buf, 1);
	post_log("\nLINE1L to Right ADC Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 21 LINE1R to Left ADC Control Register */
	control_register = 21;
	control_byte = 0x80 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - LINE1R to Left ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - LINE1R to Left ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 21, 1, &buf, 1);
	post_log("\nLINE1R to Left ADC Control Register: 0x%x\n", buf);
#endif
#endif
/* page 0 - Register 22 LINE1R to Right ADC Control Register */
	control_register = 22;
	control_byte = 0x7c | (1 << 31);	//0x84

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - LINE1R to Right ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - LINE1R to Right ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 22, 1, &buf, 1);
	post_log("\nLINE1R to Right ADC Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 17 MIC3L/R to Left ADC Control Register */
	control_register = 17;
	control_byte = 0x0F | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - MIC3L/R to Left ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - MIC3L/R to Left ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 17, 1, &buf, 1);
	post_log("\nMIC3L/R to Left ADC Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 18 MIC3L/R to Right ADC Control Register */
	control_register = 18;
	control_byte = 0xF0 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - MIC3L/R to Right ADC Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - MIC3L/R to Right ADC Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 18, 1, &buf, 1);
	post_log("\nMIC3L/R to Right ADC Control Register: 0x%x\n", buf);
#endif

/* page 0 - Register 25 -MICBIAS Control Register (for Mic)*/
	control_register = 25;
	control_byte = 0x80 | (1 << 31);	//0xC0, 0x00, 0x80

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - MICBIAS Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - MICBIAS Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 25, 1, &buf, 1);
	post_log("\nMICBIAS Control Register: 0x%x\n", buf);
#endif
/* Done with DAC and MIC configuration */

/* Page 0 - Register 37 - Power up Left and right DACs */
	control_register = 37;
	control_byte = 0x0 | (1 << 31);	//11000000 //C0 for power up

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - DACs Powered down\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - DACs not powered down\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 37, 1, &buf, 1);
	post_log("\nDACs Power Control Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 40 - VCM setting */
	control_register = 40;
	control_byte = 0x80 | (1 << 31);	//11000000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - VCM Setting Done\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - VCM Setting Done\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 40, 1, &buf, 1);
	post_log("\nVCM Register: 0x%x\n", buf);
#endif

/* Page 0 - Register 41 - Route DAC to */
	control_register = 41;
	control_byte = 0x50 | (1 << 31);	//01010000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - DAC Routing Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - DAC Routing Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 41, 1, &buf, 1);
	post_log("\nDAC Routing Register: 0x%x\n", buf);
#endif

/* page0 - Register 43 - Left DAC Volume Control Register */
	control_register = 43;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Left DAC Volume Control REgister\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Left DAC Volume Control REgister\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 43, 1, &buf, 1);
	post_log("\nLEFT DAC Volume Control Register: 0x%x\n", buf);
#endif

/* page0 - Register 44 - Right DAC Volume Control Register */
	control_register = 44;
	control_byte = 0x00 | (1 << 31);

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Right DAC Volume Control REgister\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Right DAC Volume Control REgister\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 44, 1, &buf, 1);
	post_log("\nRight DAC Volume Control Register: 0x%x\n", buf);
#endif

/* Left LOP - Register 86 */
	control_register = 86;
	control_byte = 0x0B | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Left LOPP Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Left LOP Volume Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 86, 1, &buf, 1);
	post_log("\nLeft LOP Volume Control Register: 0x%x\n", buf);
#endif

/* Right LOM - Register 93 */
	control_register = 93;
	control_byte = 0x0B | (1 << 31);	//00001010

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - Right LOM Control Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -Right LOM Volume Control Register\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 93, 1, &buf, 1);
	post_log("\nRight LOM Volume Control Register: 0x%x\n", buf);
#endif

/* ADC Digital Path and I2C Bus Condition Register - Register 107 */
	control_register = 107;
	control_byte = 0x38 | (1 << 31);	//Left and Right Analog Microphones are used 0x30, 0 for digital mic

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);
	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log
		    ("SMBUS TRANSACTION SUCCESSFUL - ADC Digital Path Register\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL -ADC Digital Path\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 107, 1, &buf, 1);
	post_log("\nADC Digital Path and I2C Bus Condition Register: 0x%x\n",
		 buf);
#endif
	//Configure i2s for capture
	audio_i2s_in_rx_config(0, 0, 1, 0, 0, 1, 0, 24, 0, 0, 0, 0, 1);

	audio_fmm_bf_ctrl_dest_cfg(1, 0, 0, 0, 0, 0, 0x182, 0, 0, 0, 0, 0, 0, 1,
				   0, 1);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_RDADDR, 0x87000000,
		      4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_WRADDR, 0x87000000,
		      4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_BASEADDR,
		      0x87000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_ENDADDR,
		      0x8900077F, 4);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_RDADDR, 0x97000000,
		      4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_WRADDR, 0x97000000,
		      4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_BASEADDR,
		      0x97000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_ENDADDR,
		      0x9900077F, 4);

	//i2s_0 input legacy enable
	cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_LEGACY, 0, 4);

	//PLAY RUN
	//audio_i2s_in_rx_config(1,2,1,0,0,1,0,24,0,0,0,0,1);
	audio_i2s_in_rx_config(1, 0, 1, 0, 0, 1, 0, 24, 0, 0, 0, 0, 1);
	audio_fmm_bf_ctrl_dest_cfg(1, 0, 0, 0, 0, 0, 0x182, 0, 0, 0, 0, 1, 0, 1,
				   1, 1);
	audio_start_capture_inputs(1, 1);

	post_log("Capture via I2S_0 input\n");

	//Disable ADC
	/*0x18 : 8 */

/* Page 0 - Register 19, 22 - Power down Left and right ADCs */
#if 0
	control_register = 19;
	control_byte = 0x0 | (1 << 31);	//11000000

	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);

	control_register = 22;
	control_byte = 0x0 | (1 << 31);	//11000000
	smbus_status =
	    smbus3_write(smbus_slave_TIcodec_address, control_register,
			control_byte);

	if (smbus_status == 0) {
#if I2S_DEBUG
		post_log("SMBUS TRANSACTION SUCCESSFUL - ADCs Powered down\n");
#endif
	} else {
		post_log
		    ("SMBUS TRANSACTION NOT SUCCESSFUL - ADCs not powered down\n");
		return -1;
	}
#if I2S_DEBUG
	i2c_read(0x18, 19, 1, &buf, 1);
	post_log("\nADCs Power Control Register 19: 0x%x\n", buf);
	i2c_read(0x18, 22, 1, &buf, 1);
	post_log("\nADCs Power Control Register 22: 0x%x\n", buf);
#endif
#endif
/*      control_register = 0x30;
        control_byte = 0x8 | (1 << 31);
        smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);

        if (smbus_status == 0)
        {
		post_log("SMBUS TRANSACTION SUCCESSFUL\n");
        }
        else
        {
		post_log("SMBUS TRANSACTION NOT SUCCESSFUL\n");
		return -1;
        }
*/
	post_log
	    ("End of test : You should hear whatever is input to top jack\n");

	//Playback recorded Audio
	post_log("Enter Y/N to quit\n");

	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y')
		 && (ch != 'n') && (ch != 'N'));
	if ((ch == 'n') || (ch == 'N')) {
		//       writel(0x0, (AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3));
		writel(0xffffffff, (AUDIO_AUD_MISC_INIT));
		writel(0x0, (AUDIO_AUD_MISC_INIT));
		post_log("\nError. Exiting the test\n");
		return -1;
	} else {
		/* Stop play_run and Reset the audio subsystem */
		//       writel(0x0, (AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3));
		writel(0xffffffff, (AUDIO_AUD_MISC_INIT));
		writel(0x0, (AUDIO_AUD_MISC_INIT));

	}

//      control_register = 0x32;
//      control_byte = 0x0 | (1 << 31);
//      smbus_status = smbus3_write(smbus_slave_TIcodec_address,control_register,control_byte);

	return 0;
}
#endif

int audio_i2s_test(void)
{
	int status;
	status = 0;
	char input;

	post_log
	    ("\n\nTesting I2S interface with TI codec on Daughter card model BCM9I2SDDC\n");
	post_log
	    ("  card 0 in slot I2S0 (J40)\n");
	post_log
	    ("  card 1 in slot I2S1 (J41)\n");

	do {
		post_log("Select audio card (0 or 1) for audio output test:\n");
		input = (char)serial_getc();
	} while ((input != '0') && (input != '1'));

	status = audio_i2s0_output_to_stereo_codec_test_0(input-'0');

	/****** input not supported ******
	if (input == '2') {
		status = 0;
		status = audio_i2s0_mic_in();
	}
	*********************/

	return status;
}

/******* Internal test function start ********/
int AUDIO_post_test(int flags)
{
	int status = 0;
#ifdef BCM_OTP_CHECK_ENABLED
	u32 ip_enable_status = 0;
#endif
	int ret = 0;
	status = TEST_PASS;
	char *s = getenv("board_name");

	if (s) {
		if ((!strcmp(s, PEGASUS_XMC_BOARD)) ||
				(!strcmp(s, PEGASUS_17MM_BOARD))) {
			post_log("AUDIO interface is not available on %s\n", s);
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

	if (TEST_FAIL == audio_i2s_test())
		status = TEST_FAIL;

	return status;
}

#endif /* CONFIG_POST && CONFIG_SYS_POST_AUDIO */
