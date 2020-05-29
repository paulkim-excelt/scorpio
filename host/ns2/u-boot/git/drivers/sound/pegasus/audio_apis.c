/*
 * $Copyright Open Broadcom Corporation$
 *
 * audio_apis.c
 *
 * Ported from Northstar2.
 *
 * This file contains routines that interface with
 * audio subsystem registers for Pegasus.
 *
 */

#include "audio_apis.h"
#include <post.h>

#define AUDIO_DEBUG 0

void dte_iomux_select_external_inputs(void)
{
	/* Needs to implement for Pegasus if required */
	post_log("Not yet implenmented ..\n");
}

void audio_sw_reset(void)
{
	uint32_t data;

	/* Write 0 to bring AUDIO system out of reset */
#if AUDIO_DEBUG
	post_log("\n\nInitiating Audio SOFT RESET\n");
#endif
	writel(0x0, AUDIO_M0_IDM_IDM_RESET_CONTROL);

#if AUDIO_DEBUG
	post_log("\n\nASIU SOFT RESET INITIATED\n");
#endif
	data = cpu_rd_single(AUDIO_AUD_MISC_REVISION, 4);
	post_log("Audio module Revision: %x\n", data);

	cpu_wr_single(AUDIO_AUD_MISC_INIT, 0xc0000c03, 4);
	add_delay(10000);
	cpu_wr_single(AUDIO_AUD_MISC_INIT, 0x0, 4);
	data = cpu_rd_single(AUDIO_AUD_MISC_INIT, 4);

	add_delay(1000);

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP0, 0x0, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP1, 0x1, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP2, 0x2, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP3, 0x3, 4);

#if AUDIO_DEBUG
	post_log("ASIU SOFT RESET DONE\n");
#endif
}

void crmu_reset_related(void)
{
	/* Needs to implement for Pegasus if required */
	post_log("Not yet implenmented ..\n");
}

void audio_io_mux_select(void)
{
	/* Needs to implement for Pegasus if required */
	post_log("Not yet implenmented ..\n");
}

void audio_pad_enable(void)
{
	/* Needs to implement for Pegasus if required */
	post_log("Not yet implenmented ..\n");
}

void audio_clock_gating_disable(void)
{
/*cpu_wr_single(ASIU_TOP_CLK_GATING_CTRL,0x00000002,4);*/
/*post_log("Clock Gating Disabled for the ASIU - Audio block\n");*/

/*default Group ID configurations also*/
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP0, 0x0, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP1, 0x1, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP2, 0x2, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP3, 0x3, 4);
}

void audio_spdif_stream_config(uint32_t ena,
			       uint32_t channel_grouping,
			       uint32_t group_id,
			       uint32_t stream_bit_resolution,
			       uint32_t wait_for_valid,
			       uint32_t ignore_first_underflow,
			       uint32_t init_sm,
			       uint32_t ins_inval, uint32_t fci_id)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Stream Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data = (ena << AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__ENA |
		   channel_grouping <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__CHANNEL_GROUPING_R
		   | group_id <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__GROUP_ID_R |
		   stream_bit_resolution <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__STREAM_BIT_RESOLUTION_R
		   | wait_for_valid <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__WAIT_FOR_VALID |
		   ignore_first_underflow <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__IGNORE_FIRST_UNDERFLOW
		   | init_sm <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__INIT_SM |
		   ins_inval <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__INS_INVAL | 3 <<
		   AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0__FCI_ID_R);
#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0 = %0x\n", wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_STREAM_CFG_0, wr_data, 4);
}

/* ----------------------- SPDIF Control function -------------------------- */
void audio_spdif_ctrl(uint32_t dither_value,
		      uint32_t wait_pcm_to_comp,
		      uint32_t length_code,
		      uint32_t overwrite_data,
		      uint32_t comp_or_linear,
		      uint32_t flush_on_uflow,
		      uint32_t insert_on_uflow,
		      uint32_t insert_when_disa,
		      uint32_t spdif_bypass,
		      uint32_t cp_toggle_rate,
		      uint32_t dither_width,
		      uint32_t dither_ena,
		      uint32_t hold_cstat, uint32_t validity)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Control\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data =
		(dither_value <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__DITHER_VALUE |
		 wait_pcm_to_comp <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__WAIT_PCM_TO_COMP_R |
		 length_code <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__LENGTH_CODE |
		 overwrite_data <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__OVERWRITE_DATA |
		 comp_or_linear <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__COMP_OR_LINEAR |
		 flush_on_uflow <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__FLUSH_ON_UFLOW |
		 insert_on_uflow <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__INSERT_ON_UFLOW |
		 insert_when_disa <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__INSERT_WHEN_DISA |
		 spdif_bypass <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__SPDIF_BYPASS |
		 cp_toggle_rate <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__CP_TOGGLE_RATE_R |
		 dither_width <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__DITHER_WIDTH_R |
		 dither_ena <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__DITHER_ENA |
		 hold_cstat <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__HOLD_CSTAT |
		 validity <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL__VALIDITY);
#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL = %0x\n", wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CTRL, wr_data, 4);
}

/* --------------------- SPDIF Channel Status function ---------------------- */
void audio_spdif_ch_status(uint32_t ch_status_0,
			   uint32_t ch_status_1, uint32_t ch_status_2)
{
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Channel Status\n");
	post_log("--------------------------------------------------------\n");

	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0= %0x\n",
		 ch_status_0);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_0,
		      ch_status_0, 4);

#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_1= %0x\n",
		 ch_status_1);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_1,
		      ch_status_1, 4);

#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_2= %0x\n",
		 ch_status_2);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_CHANSTAT_2,
		      ch_status_2, 4);
}

/* -------------------------- SPDIF Ramp Burst ----------------------------- */
void audio_spdif_ramp_burst(uint32_t stepsize,
			    uint32_t rampup_steps,
			    uint32_t stop_bit,
			    uint32_t burst_type, uint32_t rep_period)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Ramp Burst\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data =
		(stepsize <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST__STEPSIZE_R |
		 rampup_steps <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST__RAMPUP_STEPS_R |
		 stop_bit <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST__STOP_BIT |
		 burst_type <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST__BURST_TYPE |
		 rep_period <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST__REP_PERIOD_R);
#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST= %0x\n",
		 wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_RAMP_BURST, wr_data,
		      4);
}

/* ------------------------ SPDIF Format Config -------------------------- */
void audio_spdif_format_config(uint32_t lr_select,
			       uint32_t limit_to_16_bits,
			       uint32_t pream_pol,
			       uint32_t data_enable, uint32_t clock_enable)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Format Config\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data =
		(lr_select <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG__LR_SELECT_R |
		 limit_to_16_bits <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG__LIMIT_TO_16_BITS |
		 pream_pol <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG__PREAM_POL |
		 data_enable <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG__DATA_ENABLE |
		 clock_enable <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG__CLOCK_ENABLE);

#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG = %0x\n",
		 wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SPDIF_FORMAT_CFG, wr_data,
		      4);
}

/* ---------------------- SPDIF MCLK Config ---------------------------- */
void audio_spdif_mclk_config(uint32_t mclk_rate, uint32_t pllclksel)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF MCLK Config\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data =
		(mclk_rate <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0__MCLK_RATE_R
		 | pllclksel <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0__PLLCLKSEL_R);

#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0 = %0x\n", wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_MCLK_CFG_0, wr_data, 4);
}

/* ------------------ SPDIF Sample Count Config ---------------------- */
void audio_spdif_sample_count(uint32_t spdif_count_start,
			      uint32_t spdif_count_clear)
{
	uint32_t wr_data = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : SPDIF Sample Count Config\n");
	post_log("--------------------------------------------------------\n");
#endif

	wr_data =
		(spdif_count_start <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SAMPLE_COUNT_CTRL__COUNT_START |
		 spdif_count_clear <<
		 AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SAMPLE_COUNT_CTRL__COUNT_CLEAR);

#if AUDIO_DEBUG
	post_log("AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SAMPLE_COUNT_CTRL= %0x\n",
		 wr_data);
#endif
	cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_SPDIF_0_SAMPLE_COUNT_CTRL, wr_data,
		      4);
}

/* ----------------- I2S In Configuration function ------------------------- */
void audio_i2s_in_rx_config(
	uint32_t cap_ena,
	uint32_t cap_group_id,
	uint32_t ignore_first_overflow,
	uint32_t lrck_polarity,
	uint32_t sclk_polarity,
	uint32_t data_alignment,
	uint32_t data_justification,
	uint32_t bits_per_sample,
	uint32_t bits_per_slot,
	uint32_t valid_slot,
	uint32_t slave_mode,
	uint32_t tdm_mode, uint32_t i2s_port_num
	)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S In Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif

/* Capture Stream */
/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   ignore_first_overflow <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0__IGNORE_FIRST_OVERFLOW
			   | cap_group_id <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0__CAP_GROUP_ID_R
			   | cap_ena <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0__CAP_ENA);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_CAP_STREAM_CFG_0,
			      wr_data, 4);

		rd_data = 0;
		wr_data = 0;
		mask = 0;
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   ignore_first_overflow <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0__IGNORE_FIRST_OVERFLOW
			   | cap_group_id <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0__CAP_GROUP_ID_R
			   | cap_ena <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0__CAP_ENA);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_CAP_STREAM_CFG_0,
			      wr_data, 4);

		rd_data = 0;
		wr_data = 0;
		mask = 0;
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   ignore_first_overflow <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0__IGNORE_FIRST_OVERFLOW
			   | cap_group_id <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0__CAP_GROUP_ID_R
			   | cap_ena <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0__CAP_ENA);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_CAP_STREAM_CFG_0,
			      wr_data, 4);

		rd_data = 0;
		wr_data = 0;
		mask = 0;
	}
/* I2S In Config */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__LRCK_POLARITY
			   | sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__SCLK_POLARITY
			   | data_alignment <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__DATA_ALIGNMENT
			   | data_justification <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__BITS_PER_SLOT
			   | valid_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__VALID_SLOT_R |
			   slave_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG__TDM_MODE);

		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_IN_CFG, wr_data,
			      4);
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__LRCK_POLARITY
			   | sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__SCLK_POLARITY
			   | data_alignment <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__DATA_ALIGNMENT
			   | data_justification <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__BITS_PER_SLOT
			   | valid_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__VALID_SLOT_R |
			   slave_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG__TDM_MODE);

		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_IN_CFG, wr_data,
			      4);
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__LRCK_POLARITY
			   | sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__SCLK_POLARITY
			   | data_alignment <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__DATA_ALIGNMENT
			   | data_justification <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__BITS_PER_SLOT
			   | valid_slot <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__VALID_SLOT_R |
			   slave_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG__TDM_MODE);

		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_IN_CFG, wr_data,
			      4);
	}
}

/* ------------ I2S Destination Buffer Configuration function ---------- */
void audio_fmm_bf_ctrl_dest_cfg(uint32_t process_seq_id_valid,
				uint32_t blocked_access_disable,
				uint32_t process_id_high,
				uint32_t dma_block_cnt,
				uint32_t reverse_endian,
				uint32_t capture_mode,
				uint32_t fci_cap_id,
				uint32_t not_pause_when_full,
				uint32_t source_fifo_id,
				uint32_t input_frm_sourcefifo,
				uint32_t capture_to_sourcefifo,
				uint32_t play_from_capture,
				uint32_t destfifo_size_double,
				uint32_t buffer_pair_enable,
				uint32_t capture_enable, uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S0 Dest Buffer Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__PROCESS_ID_HIGH |
			   dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__DMA_BLOCK_CNT_R |
			   reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__REVERSE_ENDIAN |
			   capture_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__CAPTURE_MODE |
			   0x180 <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__FCI_CAP_ID_R |
			   not_pause_when_full <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__NOT_PAUSE_WHEN_FULL
			   | source_fifo_id <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__SOURCE_FIFO_ID_R |
			   input_frm_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__INPUT_FRM_SOURCEFIFO
			   | capture_to_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__CAPTURE_TO_SOURCEFIFO
			   | play_from_capture <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__PLAY_FROM_CAPTURE
			   | destfifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__DESTFIFO_SIZE_DOUBLE
			   | buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__BUFFER_PAIR_ENABLE
			   | capture_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0__CAPTURE_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG0, wr_data, 4);
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__PROCESS_ID_HIGH |
			   dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__DMA_BLOCK_CNT_R |
			   reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__REVERSE_ENDIAN |
			   capture_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__CAPTURE_MODE |
			   0x181 <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__FCI_CAP_ID_R |
			   not_pause_when_full <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__NOT_PAUSE_WHEN_FULL
			   | source_fifo_id <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__SOURCE_FIFO_ID_R |
			   input_frm_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__INPUT_FRM_SOURCEFIFO
			   | capture_to_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__CAPTURE_TO_SOURCEFIFO
			   | play_from_capture <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__PLAY_FROM_CAPTURE
			   | destfifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__DESTFIFO_SIZE_DOUBLE
			   | buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__BUFFER_PAIR_ENABLE
			   | capture_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1__CAPTURE_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG1, wr_data, 4);
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__PROCESS_ID_HIGH |
			   dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__DMA_BLOCK_CNT_R |
			   reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__REVERSE_ENDIAN |
			   capture_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__CAPTURE_MODE |
			   0x182 <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__FCI_CAP_ID_R |
			   not_pause_when_full <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__NOT_PAUSE_WHEN_FULL
			   | source_fifo_id <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__SOURCE_FIFO_ID_R |
			   input_frm_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__INPUT_FRM_SOURCEFIFO
			   | capture_to_sourcefifo <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__CAPTURE_TO_SOURCEFIFO
			   | play_from_capture <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__PLAY_FROM_CAPTURE
			   | destfifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__DESTFIFO_SIZE_DOUBLE
			   | buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__BUFFER_PAIR_ENABLE
			   | capture_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2__CAPTURE_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CFG2, wr_data, 4);
	}
}

/* ---------------- I2S OUT Configuration function -------------------- */
void audio_i2s_out_tx_config(uint32_t clock_enable,
			     uint32_t data_enable,
			     uint32_t lrck_polarity,
			     uint32_t sclk_polarity,
			     uint32_t data_alignment,
			     uint32_t data_justification,
			     uint32_t bits_per_sample,
			     uint32_t bits_per_slot,
			     uint32_t valid_slot,
			     uint32_t fsync_width,
			     uint32_t sclk_per_1fs,
			     uint32_t slave_mode,
			     uint32_t tdm_mode,
			     uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S OUT Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   clock_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__CLOCK_ENABLE |
			   data_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__DATA_ENABLE |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__LRCK_POLARITY |
			   sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__SCLK_POLARITY |
			   data_alignment <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__DATA_ALIGNMENT |
			   data_justification <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__BITS_PER_SLOT |
			   valid_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__VALID_SLOT_R |
			   fsync_width <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__FSYNC_WIDTH_R |
			   sclk_per_1fs <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__SCLKS_PER_1FS_DIV32_R
			   | slave_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG__TDM_MODE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_CFG, wr_data, 4);
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   clock_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__CLOCK_ENABLE |
			   data_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__DATA_ENABLE |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__LRCK_POLARITY |
			   sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__SCLK_POLARITY |
			   data_alignment <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__DATA_ALIGNMENT |
			   data_justification <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__BITS_PER_SLOT |
			   valid_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__VALID_SLOT_R |
			   fsync_width <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__FSYNC_WIDTH_R |
			   sclk_per_1fs <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__SCLKS_PER_1FS_DIV32_R
			   | slave_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG__TDM_MODE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_CFG, wr_data, 4);
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   clock_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__CLOCK_ENABLE |
			   data_enable <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__DATA_ENABLE |
			   lrck_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__LRCK_POLARITY |
			   sclk_polarity <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__SCLK_POLARITY |
			   data_alignment <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__DATA_ALIGNMENT |
			   data_justification <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__DATA_JUSTIFICATION
			   | bits_per_sample <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__BITS_PER_SAMPLE_R
			   | bits_per_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__BITS_PER_SLOT |
			   valid_slot <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__VALID_SLOT_R |
			   fsync_width <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__FSYNC_WIDTH_R |
			   sclk_per_1fs <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__SCLKS_PER_1FS_DIV32_R
			   | slave_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__SLAVE_MODE |
			   tdm_mode <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG__TDM_MODE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_CFG, wr_data, 4);
	}
}

/* -------------- I2S Stream Configuration function ------------------ */
void audio_i2s_stream_config_samp_count(uint32_t ena,
					uint32_t channel_grouping,
					uint32_t group_id,
					uint32_t stream_bit_resolution,
					uint32_t wait_for_valid,
					uint32_t ignore_first_underflow,
					uint32_t init_sm,
					uint32_t ins_inval,
					uint32_t fci_id, uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Out Stream Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0s */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			ena <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__ENA
			| channel_grouping <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__CHANNEL_GROUPING_R
			| group_id <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__GROUP_ID_R
			| stream_bit_resolution <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__STREAM_BIT_RESOLUTION_R
			| wait_for_valid <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__WAIT_FOR_VALID
			| ignore_first_underflow <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__IGNORE_FIRST_UNDERFLOW
			| init_sm <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__INIT_SM
			| ins_inval <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__INS_INVAL
			|
			0 <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0__FCI_ID_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_STREAM_CFG_0, wr_data,
			      4);
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			ena <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__ENA
			| channel_grouping <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__CHANNEL_GROUPING_R
			| group_id <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__GROUP_ID_R
			| stream_bit_resolution <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__STREAM_BIT_RESOLUTION_R
			| wait_for_valid <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__WAIT_FOR_VALID
			| ignore_first_underflow <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__IGNORE_FIRST_UNDERFLOW
			| init_sm <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__INIT_SM
			| ins_inval <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__INS_INVAL
			|
			1 <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0__FCI_ID_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_STREAM_CFG_0, wr_data,
			      4);
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			ena <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__ENA
			| channel_grouping <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__CHANNEL_GROUPING_R
			| group_id <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__GROUP_ID_R
			| stream_bit_resolution <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__STREAM_BIT_RESOLUTION_R
			| wait_for_valid <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__WAIT_FOR_VALID
			| ignore_first_underflow <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__IGNORE_FIRST_UNDERFLOW
			| init_sm <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__INIT_SM
			| ins_inval <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__INS_INVAL
			|
			2 <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0__FCI_ID_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_STREAM_CFG_0, wr_data,
			      4);
	}
}

/* ----------- I2S Source Buffer Configuration function ----------------- */
void audio_fmm_bf_ctrl_source_cfg(uint32_t spdif_en,
				  uint32_t process_seq_id_valid,
				  uint32_t blocked_access_disable,
				  uint32_t process_id_high,
				  uint32_t dma_block_cnt,
				  uint32_t reverse_endian,
				  uint32_t bit_resolution,
				  uint32_t shared_sbuf_id,
				  uint32_t share_sbuf,
				  uint32_t sfifo_start_halffull,
				  uint32_t dma_read_disable,
				  uint32_t sample_repeat_enable,
				  uint32_t not_pause_when_empty,
				  uint32_t start_selection,
				  uint32_t retain_fci_tag,
				  uint32_t sourcefifo_size_double,
				  uint32_t lr_data_ctrl,
				  uint32_t sample_ch_mode,
				  uint32_t buffer_pair_enable,
				  uint32_t sourcefifo_enable,
				  uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Source Buffer Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif

	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__PROCESS_ID_HIGH
			   | dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__DMA_BLOCK_CNT_R
			   | reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__REVERSE_ENDIAN |
			   bit_resolution <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__BIT_RESOLUTION_R
			   | shared_sbuf_id <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SHARED_SBUF_ID_R
			   | share_sbuf <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SHARE_SBUF |
			   sfifo_start_halffull <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SFIFO_START_HALFFULL
			   | dma_read_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__DMA_READ_DISABLE
			   | sample_repeat_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SAMPLE_REPEAT_ENABLE
			   | not_pause_when_empty <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__NOT_PAUSE_WHEN_EMPTY
			   | start_selection <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__START_SELECTION
			   | retain_fci_tag <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__RETAIN_FCI_TAG |
			   sourcefifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SOURCEFIFO_SIZE_DOUBLE
			   | lr_data_ctrl <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__LR_DATA_CTRL_R |
			   sample_ch_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SAMPLE_CH_MODE |
			   buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__BUFFER_PAIR_ENABLE
			   | sourcefifo_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0__SOURCEFIFO_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0 = %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG0, wr_data, 4);
	}

	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__PROCESS_ID_HIGH
			   | dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__DMA_BLOCK_CNT_R
			   | reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__REVERSE_ENDIAN |
			   bit_resolution <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__BIT_RESOLUTION_R
			   | shared_sbuf_id <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SHARED_SBUF_ID_R
			   | share_sbuf <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SHARE_SBUF |
			   sfifo_start_halffull <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SFIFO_START_HALFFULL
			   | dma_read_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__DMA_READ_DISABLE
			   | sample_repeat_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SAMPLE_REPEAT_ENABLE
			   | not_pause_when_empty <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__NOT_PAUSE_WHEN_EMPTY
			   | start_selection <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__START_SELECTION
			   | retain_fci_tag <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__RETAIN_FCI_TAG |
			   sourcefifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SOURCEFIFO_SIZE_DOUBLE
			   | lr_data_ctrl <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__LR_DATA_CTRL_R |
			   sample_ch_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SAMPLE_CH_MODE |
			   buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__BUFFER_PAIR_ENABLE
			   | sourcefifo_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1__SOURCEFIFO_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1= %0x\n", wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG1, wr_data, 4);
	}

	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__PROCESS_ID_HIGH
			   | dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__DMA_BLOCK_CNT_R
			   | reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__REVERSE_ENDIAN |
			   bit_resolution <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__BIT_RESOLUTION_R
			   | shared_sbuf_id <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SHARED_SBUF_ID_R
			   | share_sbuf <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SHARE_SBUF |
			   sfifo_start_halffull <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SFIFO_START_HALFFULL
			   | dma_read_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__DMA_READ_DISABLE
			   | sample_repeat_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SAMPLE_REPEAT_ENABLE
			   | not_pause_when_empty <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__NOT_PAUSE_WHEN_EMPTY
			   | start_selection <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__START_SELECTION
			   | retain_fci_tag <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__RETAIN_FCI_TAG |
			   sourcefifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SOURCEFIFO_SIZE_DOUBLE
			   | lr_data_ctrl <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__LR_DATA_CTRL_R |
			   sample_ch_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SAMPLE_CH_MODE |
			   buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__BUFFER_PAIR_ENABLE
			   | sourcefifo_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2__SOURCEFIFO_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2 = %0x\n",
			 wr_data);
#endif

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG2, wr_data, 4);
	}
	/* SPDIF Enable */
	if (spdif_en) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;

		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   process_seq_id_valid <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__PROCESS_SEQ_ID_VALID
			   | blocked_access_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__BLOCKED_ACCESS_DISABLE
			   | process_id_high <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__PROCESS_ID_HIGH
			   | dma_block_cnt <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__DMA_BLOCK_CNT_R
			   | reverse_endian <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__REVERSE_ENDIAN |
			   bit_resolution <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__BIT_RESOLUTION_R
			   | shared_sbuf_id <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SHARED_SBUF_ID_R
			   | share_sbuf <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SHARE_SBUF |
			   sfifo_start_halffull <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SFIFO_START_HALFFULL
			   | dma_read_disable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__DMA_READ_DISABLE
			   | sample_repeat_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SAMPLE_REPEAT_ENABLE
			   | not_pause_when_empty <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__NOT_PAUSE_WHEN_EMPTY
			   | start_selection <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__START_SELECTION
			   | retain_fci_tag <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__RETAIN_FCI_TAG |
			   sourcefifo_size_double <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SOURCEFIFO_SIZE_DOUBLE
			   | lr_data_ctrl <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__LR_DATA_CTRL_R |
			   sample_ch_mode <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SAMPLE_CH_MODE |
			   buffer_pair_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__BUFFER_PAIR_ENABLE
			   | sourcefifo_enable <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3__SOURCEFIFO_ENABLE);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3 = %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CFG3, wr_data, 4);
	}
}

/* ---------------  I2S Source Buffer  Address Set function ----------------- */
void audio_set_source_buffer_addr(uint32_t i2s_port_num,
				  uint32_t single_buffer_en,
				  uint32_t first_ringbuf_rdaddr,
				  uint32_t first_ringbuf_wraddr,
				  uint32_t first_ringbuf_base_addr,
				  uint32_t first_ringbuf_end_address,
				  uint32_t first_ringbuf_start_wrpnt,
				  uint32_t second_ringbuf_rdaddr,
				  uint32_t second_ringbuf_wraddr,
				  uint32_t second_ringbuf_base_addr,
				  uint32_t second_ringbuf_end_address,
				  uint32_t second_ringbuf_start_wrpnt,
				  uint32_t audio_address_difference,
				  uint32_t spdif_en)
{
	uint32_t mi_valid_wr_data = 0x00000000;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Source Buffer Address Configuration\n");
	post_log("--------------------------------------------------------\n");

	char string[] = "AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF";
#endif

	/* For I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		if (single_buffer_en == 0) {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 1 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_RDADDR,
				 second_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_WRADDR,
				 second_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_BASEADDR,
				 second_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_ENDADDR,
				 second_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_0_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_0_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_0_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_0_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_0_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_0_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
			post_log("%s_1_RDADDR= %0x\n", string,
				 second_ringbuf_rdaddr);
			post_log("%s_1_WRADDR= %0x\n", string,
				 second_ringbuf_wraddr);
			post_log("%s_1_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr);
			post_log("%s_1_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address);
			post_log("%s_1_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);
			post_log("%s_1_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		} else {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_0_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_0_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_0_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_0_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_0_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_0_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		}
	}
	/* For I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		if (single_buffer_en == 0) {
			/* Buffer 2 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_RDADDR,
				 (first_ringbuf_rdaddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_WRADDR,
				 (first_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_BASEADDR,
				 (first_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_ENDADDR,
				 (first_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_START_WRPOINT,
				 (first_ringbuf_start_wrpnt), 4);
			/* Buffer 3 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_RDADDR,
				 (second_ringbuf_rdaddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_WRADDR,
				 (second_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_BASEADDR,
				 (second_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_ENDADDR,
				 (second_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_START_WRPOINT,
				 (second_ringbuf_start_wrpnt), 4);
#if AUDIO_DEBUG
			post_log("%s_2_RDADDR= %0x\n", string,
				 (first_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_2_WRADDR= %0x\n", string,
				 (first_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_2_BASEADDR= %0x\n", string,
				 (first_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_2_ENDADDR= %0x\n", string,
				 (first_ringbuf_end_address |
				  audio_address_difference));
			post_log("%s_2_START_WRPOINT= %0x\n", string,
				 (first_ringbuf_start_wrpnt));
			post_log("%s_2_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
			post_log("%s_3_RDADDR= %0x\n", string,
				 (second_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_3_WRADDR= %0x\n", string,
				 (second_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_3_BASEADDR= %0x\n", string,
				 (second_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_3_ENDADDR= %0x\n", string,
				 (second_ringbuf_end_address |
				  audio_address_difference));
			post_log("%s_3_START_WRPOINT= %0x\n", string,
				 (second_ringbuf_start_wrpnt));
			post_log("%s_3_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		} else {
			/* Buffer 2 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_RDADDR,
				 (first_ringbuf_rdaddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_WRADDR,
				 (first_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_BASEADDR,
				 (first_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_ENDADDR,
				 (first_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_START_WRPOINT,
				 (first_ringbuf_start_wrpnt), 4);
#if AUDIO_DEBUG
			post_log("%s_2_RDADDR= %0x\n", string,
				 (first_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_2_WRADDR= %0x\n", string,
				 (first_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_2_BASEADDR= %0x\n", string,
				 (first_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_2_ENDADDR= %0x\n", string,
				 (first_ringbuf_end_address |
				  audio_address_difference));
			post_log("%s_2_START_WRPOINT= %0x\n", string,
				 (first_ringbuf_start_wrpnt));
			post_log("%s_2_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		}
	}
	/* For I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		first_ringbuf_rdaddr =
			first_ringbuf_rdaddr |
			(audio_address_difference << 1);
		first_ringbuf_wraddr =
			first_ringbuf_wraddr |
			(audio_address_difference << 1);
		first_ringbuf_base_addr =
			first_ringbuf_base_addr |
			(audio_address_difference << 1);
		first_ringbuf_end_address =
			first_ringbuf_end_address |
			(audio_address_difference << 1);
		second_ringbuf_rdaddr =
			second_ringbuf_rdaddr |
			(audio_address_difference << 1);
		second_ringbuf_wraddr =
			second_ringbuf_wraddr |
			(audio_address_difference << 1);
		second_ringbuf_base_addr =
			second_ringbuf_base_addr |
			(audio_address_difference << 1);
		second_ringbuf_end_address =
			second_ringbuf_end_address |
			(audio_address_difference << 1);

		if (single_buffer_en == 0) {
			/* Buffer 4 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 5 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_RDADDR,
				 second_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_WRADDR,
				 second_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_BASEADDR,
				 second_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_ENDADDR,
				 second_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_4_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_4_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_4_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_4_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_4_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_4_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
			post_log("%s_5_RDADDR= %0x\n", string,
				 second_ringbuf_rdaddr);
			post_log("%s_5_WRADDR= %0x\n", string,
				 second_ringbuf_wraddr);
			post_log("%s_5_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr);
			post_log("%s_5_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address);
			post_log("%s_5_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);
			post_log("%s_5_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		} else {
			/* Buffer 4 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_4_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_4_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_4_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_4_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_4_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_4_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		}
	}
	/* For SPDIF */
	if (spdif_en) {
		if (single_buffer_en == 0) {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 1 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR,
				 second_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR,
				 second_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR,
				 second_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR,
				 second_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_6_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_6_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_6_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_6_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_6_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_6_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
			post_log("%s_7_RDADDR= %0x\n", string,
				 second_ringbuf_rdaddr);
			post_log("%s_7_WRADDR= %0x\n", string,
				 second_ringbuf_wraddr);
			post_log("%s_7_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr);
			post_log("%s_7_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address);
			post_log("%s_7_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);
			post_log("%s_7_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR,
				 first_ringbuf_rdaddr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR,
				 first_ringbuf_wraddr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR,
				 first_ringbuf_base_addr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR,
				 first_ringbuf_end_address |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 1 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_RDADDR,
				 second_ringbuf_rdaddr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_WRADDR,
				 second_ringbuf_wraddr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_BASEADDR,
				 second_ringbuf_base_addr |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_ENDADDR,
				 second_ringbuf_end_address |
				 audio_address_difference, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_7_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_6_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr |
				 audio_address_difference);
			post_log("%s_6_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr |
				 audio_address_difference);
			post_log("%s_6_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr |
				 audio_address_difference);
			post_log("%s_6_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address |
				 audio_address_difference);
			post_log("%s_6_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_6_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
			post_log("%s_7_RDADDR= %0x\n", string,
				 second_ringbuf_rdaddr |
				 audio_address_difference);
			post_log("%s_7_WRADDR= %0x\n", string,
				 second_ringbuf_wraddr |
				 audio_address_difference);
			post_log("%s_7_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr |
				 audio_address_difference);
			post_log("%s_7_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address |
				 audio_address_difference);
			post_log("%s_7_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);
			post_log("%s_7_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		} else {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_MI_VALID,
				 mi_valid_wr_data, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_6_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
#if AUDIO_DEBUG
			post_log("%s_6_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_6_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_6_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_6_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_6_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_6_MI_VALID= %0x\n", string,
				 mi_valid_wr_data);
#endif
		}
	}
}

/* ---------------  I2S Destination Buffer  Address Set function ------------ */
void audio_set_dest_buffer_addr(uint32_t i2s_port_num,
				uint32_t single_buffer_en,
				uint32_t first_ringbuf_rdaddr,
				uint32_t first_ringbuf_wraddr,
				uint32_t first_ringbuf_base_addr,
				uint32_t first_ringbuf_end_address,
				uint32_t first_ringbuf_start_wrpnt,
				uint32_t second_ringbuf_rdaddr,
				uint32_t second_ringbuf_wraddr,
				uint32_t second_ringbuf_base_addr,
				uint32_t second_ringbuf_end_address,
				uint32_t second_ringbuf_start_wrpnt,
				uint32_t audio_address_difference)
{
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Destination Buffer Address Config\n");
	post_log("--------------------------------------------------------\n");
#endif

	char string[] = "AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF";

	/* For I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		if (single_buffer_en == 0) {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 1 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_1_RDADDR,
				 second_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_1_WRADDR,
				 second_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_1_BASEADDR,
				 second_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_1_ENDADDR,
				 second_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_1_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);

			post_log("%s_0_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_0_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_0_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_0_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_0_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_1_RDADDR= %0x\n", string, string,
				 second_ringbuf_rdaddr);
			post_log("%s_1_WRADDR= %0x\n", string, string,
				 second_ringbuf_wraddr);
			post_log("%s_1_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr);
			post_log("%s_1_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address);
			post_log("%s_1_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);

		} else {
			/* Buffer 0 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_0_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);

			post_log("%s_0_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_0_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_0_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_0_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_0_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
		}
	}
	/* For I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		if (single_buffer_en == 0) {
			/* Buffer 2 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_RDADDR,
				 (first_ringbuf_rdaddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_WRADDR,
				 (first_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_BASEADDR,
				 (first_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_ENDADDR,
				 (first_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_START_WRPOINT,
				 (first_ringbuf_start_wrpnt), 4);
			/* Buffer 3 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_3_RDADDR,
				 (second_ringbuf_rdaddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_3_WRADDR,
				 (second_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_3_BASEADDR,
				 (second_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_3_ENDADDR,
				 (second_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_3_START_WRPOINT,
				 (second_ringbuf_start_wrpnt), 4);

			post_log("%s_2_RDADDR= %0x\n", string,
				 (first_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_2_WRADDR= %0x\n", string,
				 (first_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_2_BASEADDR= %0x\n", string,
				 (first_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_2_ENDADDR= %0x\n", string,
				 (first_ringbuf_end_address |
				  audio_address_difference));
			post_log("%s_2_START_WRPOINT= %0x\n", string,
				 (first_ringbuf_start_wrpnt));
			post_log("%s_3_RDADDR= %0x\n", string,
				 (second_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_3_WRADDR= %0x\n", string,
				 (second_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_3_BASEADDR= %0x\n", string,
				 (second_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_3_ENDADDR= %0x\n", string,
				 (second_ringbuf_end_address |
				  audio_address_difference));
			post_log("%s_3_START_WRPOINT= %0x\n", string,
				 (second_ringbuf_start_wrpnt));

		} else {
			/* Buffer 2 */
			cpu_wr_single(
				AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_RDADDR,
				(first_ringbuf_rdaddr |
				 audio_address_difference),
				4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_WRADDR,
				 (first_ringbuf_wraddr |
				  audio_address_difference),
				 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_BASEADDR,
				 (first_ringbuf_base_addr |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_ENDADDR,
				 (first_ringbuf_end_address |
				  audio_address_difference), 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_2_START_WRPOINT,
				 (first_ringbuf_start_wrpnt |
				  audio_address_difference), 4);
			post_log("%s_2_RDADDR= %0x\n", string,
				 (first_ringbuf_rdaddr |
				  audio_address_difference));
			post_log("%s_2_WRADDR= %0x\n", string,
				 (first_ringbuf_wraddr |
				  audio_address_difference));
			post_log("%s_2_BASEADDR= %0x\n", string,
				 (first_ringbuf_base_addr |
				  audio_address_difference));
			post_log("%s_2_ENDADDR= %0x\n", string,
				 (first_ringbuf_start_wrpnt |
				  audio_address_difference));
			post_log("%s_2_START_WRPOINT= %0x\n", string,
				 (first_ringbuf_start_wrpnt));
		}
	}
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		/* For I2S2 */
		first_ringbuf_rdaddr =
			first_ringbuf_rdaddr |
			(audio_address_difference << 2);
		first_ringbuf_wraddr =
			first_ringbuf_wraddr |
			(audio_address_difference << 2);
		first_ringbuf_base_addr =
			first_ringbuf_base_addr |
			(audio_address_difference << 2);
		first_ringbuf_end_address =
			first_ringbuf_end_address |
			(audio_address_difference << 2);
		second_ringbuf_rdaddr =
			second_ringbuf_rdaddr |
			(audio_address_difference << 2);
		second_ringbuf_wraddr =
			second_ringbuf_wraddr |
			(audio_address_difference << 2);
		second_ringbuf_base_addr =
			second_ringbuf_base_addr |
			(audio_address_difference << 2);
		second_ringbuf_end_address =
			second_ringbuf_end_address |
			(audio_address_difference << 2);

		if (single_buffer_en == 0) {
			/* Buffer 4 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			/* Buffer 5 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_RDADDR,
				 second_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_WRADDR,
				 second_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_BASEADDR,
				 second_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_ENDADDR,
				 second_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_5_START_WRPOINT,
				 second_ringbuf_start_wrpnt, 4);

			post_log("%s_4_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_4_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_4_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_4_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_4_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
			post_log("%s_5_RDADDR= %0x\n", string,
				 second_ringbuf_rdaddr);
			post_log("%s_5_WRADDR= %0x\n", string,
				 second_ringbuf_wraddr);
			post_log("%s_5_BASEADDR= %0x\n", string,
				 second_ringbuf_base_addr);
			post_log("%s_5_ENDADDR= %0x\n", string,
				 second_ringbuf_end_address);
			post_log("%s_5_START_WRPOINT= %0x\n", string,
				 second_ringbuf_start_wrpnt);
		} else {
			/* Buffer 4 */
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_RDADDR,
				 first_ringbuf_rdaddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_WRADDR,
				 first_ringbuf_wraddr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_BASEADDR,
				 first_ringbuf_base_addr, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_ENDADDR,
				 first_ringbuf_end_address, 4);
			cpu_wr_single
				(AUDIO_AUD_FMM_BF_CTRL_DESTCH_RINGBUF_4_START_WRPOINT,
				 first_ringbuf_start_wrpnt, 4);
			post_log("%s_4_RDADDR= %0x\n", string,
				 first_ringbuf_rdaddr);
			post_log("%s_4_WRADDR= %0x\n", string,
				 first_ringbuf_wraddr);
			post_log("%s_4_BASEADDR= %0x\n", string,
				 first_ringbuf_base_addr);
			post_log("%s_4_ENDADDR= %0x\n", string,
				 first_ringbuf_end_address);
			post_log("%s_4_START_WRPOINT= %0x\n", string,
				 first_ringbuf_start_wrpnt);
		}
	}
}

/* ------------------- Start DMA Read Enable function -----------------------*/
void audio_start_dma_read(uint32_t spdif_en, uint32_t play_run,
			  uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : Start DMA Read\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   play_run <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL0__PLAY_RUN);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL0 = %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL0, wr_data, 4);
	}
	/* I2S1 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL1, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   play_run <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL1__PLAY_RUN);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL1= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL1, wr_data, 4);
	}
	/* I2S2 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL2, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   play_run <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL2__PLAY_RUN);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL2 = %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL2, wr_data, 4);
	}
	/* SPDIF */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (spdif_en) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   play_run <<
			   AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3__PLAY_RUN);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3 = %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_CTRL3, wr_data, 4);
		cpu_wr_single(AUDIO_AUD_MISC_SEROUT_OE, 0x1fff, 4);
	}
}

/* --------------------- Start Capture Enable function --------------------- */
void audio_start_capture_inputs(uint32_t capture_run, uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : Start Capturing I2S Inputs\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL0, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   capture_run <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL0__CAPTURE_RUN);
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL0= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL0, wr_data, 4);
	}
	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL1, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   capture_run <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL1__CAPTURE_RUN);
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL1= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL1, wr_data, 4);
	}
	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data = 0;
		wr_data = 0;
		mask = 0;
		rd_data = cpu_rd_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL2, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   capture_run <<
			   AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL2__CAPTURE_RUN);
		post_log("AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL2= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_DESTCH_CTRL2, wr_data, 4);
	}
}

/* ---------------------- I2S Out Legacy Enable function ------------------- -*/
void audio_i2s_out_legacy_enable(uint32_t legacy_en, uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Out Legacy Enable\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_LEGACY__EN_LEGACY_I2S);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_LEGACY= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_I2S_LEGACY, wr_data,
			      4);
	}
	/* I2S1 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_LEGACY__EN_LEGACY_I2S);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_LEGACY= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_I2S_LEGACY, wr_data,
			      4);
	}
	/* I2S2 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_LEGACY__EN_LEGACY_I2S);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_LEGACY= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_I2S_LEGACY, wr_data,
			      4);
	}
}

/* ---------------------- I2S Out Sample Count -------------------------- */
void audio_i2s_out_sample_count(uint32_t count_start, uint32_t count_clear,
				uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S Out Sample Count Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_0_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   count_start <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_SAMPLE_COUNT_CTRL__COUNT_START |
			   count_clear <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_0_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
	/* I2S1 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_1_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   count_start <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_SAMPLE_COUNT_CTRL__COUNT_START |
			   count_clear <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_1_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
	/* I2S2 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_OUT_I2S_2_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   count_start <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_SAMPLE_COUNT_CTRL__COUNT_START |
			   count_clear <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_2_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
}

/* ------------------------- I2S In Sample Count --------------------------- */
void audio_i2s_in_sample_count(uint32_t in_count_start, uint32_t in_count_clear,
			       uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("------------------------------------------------------\n");
	post_log("AUDIO C API : I2S In Sample Count Configuration\n");
	post_log("------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_0_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   in_count_start <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_SAMPLE_COUNT_CTRL__COUNT_START |
			   in_count_clear <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_0_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_0_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
	/* I2S1 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_1_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   in_count_start <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_SAMPLE_COUNT_CTRL__COUNT_START |
			   in_count_clear <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_1_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_1_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
	/* I2S2 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(
				AUDIO_AUD_FMM_IOP_IN_I2S_2_SAMPLE_COUNT_CTRL,
				4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (wr_data |
			   in_count_start <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_SAMPLE_COUNT_CTRL__COUNT_START |
			   in_count_clear <<
			   AUDIO_AUD_FMM_IOP_IN_I2S_2_SAMPLE_COUNT_CTRL__COUNT_CLEAR);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_2_SAMPLE_COUNT_CTRL= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_SAMPLE_COUNT_CTRL,
			      wr_data, 4);
	}
}

/* --------------------- I2S In Legacy Enable function --------------------- */
void audio_i2s_in_legacy_enable(uint32_t legacy_en, uint32_t i2s_port_num)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : I2S In Legacy Enable\n");
	post_log("--------------------------------------------------------\n");
#endif
	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_LEGACY__EN_LEGACY_I2S);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_LEGACY= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_0_I2S_LEGACY, wr_data,
			      4);
	}
	/* I2S1 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_LEGACY__EN_LEGACY_I2S);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_LEGACY= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_1_I2S_LEGACY, wr_data,
			      4);
	}
	/* I2S2 */
	rd_data = 0;
	wr_data = 0;
	mask = 0;
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_LEGACY, 4);
		mask = 0x00000000;
		wr_data = rd_data & mask;

		wr_data = (
			wr_data |
			legacy_en <<
			AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_LEGACY__EN_LEGACY_I2S);
		post_log("AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_LEGACY= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_IOP_IN_I2S_2_I2S_LEGACY, wr_data,
			      4);
	}
}

/* --------------------- PLL Clock Select function -------------------------- */
void audio_mclk_cfg(uint32_t pllclksel, uint32_t mclk_rate,
		    uint32_t i2s_port_num)
{
	uint32_t wr_data = 0;

#if AUDIO_DEBUG
	post_log("---------------------------------------------------------\n");
	post_log("AUDIO C API : Mclk PLL Configuration\n");
	post_log("---------------------------------------------------------\n");
#endif

	/* I2S0 */
	if (i2s_port_num == 1 || i2s_port_num == 7) {
		wr_data = (wr_data |
			   mclk_rate <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0__MCLK_RATE_R |
			   pllclksel <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0__PLLCLKSEL_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_0_MCLK_CFG_0, wr_data,
			      4);
	}

	wr_data = 0;

	/* I2S1 */
	if (i2s_port_num == 2 || i2s_port_num == 7) {
		wr_data = (wr_data |
			   mclk_rate <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0__MCLK_RATE_R |
			   pllclksel <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0__PLLCLKSEL_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_1_MCLK_CFG_0, wr_data,
			      4);
	}

	wr_data = 0;

	/* I2S2 */
	if (i2s_port_num == 4 || i2s_port_num == 7) {
		wr_data = (wr_data |
			   mclk_rate <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_MCLK_CFG_0__MCLK_RATE_R |
			   pllclksel <<
			   AUDIO_AUD_FMM_IOP_OUT_I2S_2_MCLK_CFG_0__PLLCLKSEL_R);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_OUT_I2S_2_MCLK_CFG_0= %0x\n",
			 wr_data);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_OUT_I2S_2_MCLK_CFG_0, wr_data,
			      4);
	}
}

/* -----------------  Gen PLL Configuration function ----------------------- */
void audio_gen_pll_pwr_on(uint32_t crmu_pll_pwr_on)
{
	uint32_t wr_data = 0;
	uint32_t rd_data = 0;

#if AUDIO_DEBUG
	post_log("-------------------------------------------------------\n");
	post_log("AUDIO C API : CRMU Control Reg Wr -- Audio PLL Power On\n");
	post_log("-------------------------------------------------------\n");
#endif

	if (crmu_pll_pwr_on == 1) {
		rd_data = readl(CRMU_PLL_AON_CTRL);
		wr_data = rd_data | (1 << CRMU_PLL_AON_CTRL__GENPLL_PWRON_BG);
		writel(wr_data, (CRMU_PLL_AON_CTRL));
		add_delay(10000);

		rd_data = readl(CRMU_PLL_AON_CTRL);
		wr_data = rd_data | (1 << CRMU_PLL_AON_CTRL__GENPLL_PWRON_LDO)
			| (1 << CRMU_PLL_AON_CTRL__GENPLL_PWRON_PLL);
		writel(wr_data, (CRMU_PLL_AON_CTRL));
		add_delay(10000);

		rd_data = readl(CRMU_PLL_AON_CTRL);
		wr_data = rd_data & (~(1 << CRMU_PLL_AON_CTRL__GENPLL_ISO_IN));
		writel(wr_data, (CRMU_PLL_AON_CTRL));
#if AUDIO_DEBUG
		post_log("CRMU_PLL_AON_CTRL= %0x\n", readl(CRMU_PLL_AON_CTRL));
#endif
		add_delay(2000);
	}
}

/* ---------------------  Gen PLL Configuration function -------------------- */
void audio_gen_pll_group_id_config(uint32_t pdiv, uint32_t ndiv_int,
				   uint32_t ndiv_frac, uint32_t mdiv,
				   uint32_t gain_ki, uint32_t gain_ka,
				   uint32_t gain_kp, uint32_t user_macro,
				   uint32_t i2s_port_num,
				   uint32_t audio_ext_test_clock_en)
{
	uint32_t wr_data_user = 0;
	uint32_t reset_val = 0;
	uint32_t rd_data = 0;
	uint32_t timeout = 120;
	uint32_t bypass_disable = 0;
	uint32_t mask = 0;

#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API :  Gen. PLL Configuration\n");
	post_log("--------------------------------------------------------\n");
#endif

	if (audio_ext_test_clock_en != 1) {
		/* User Macro select */
		wr_data_user = wr_data_user |
			(user_macro <<
			 AUDIO_AUD_FMM_IOP_PLL_0_MACRO__MACRO_SELECT_R);

#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_MACRO= %0x\n", wr_data_user);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_MACRO, wr_data_user, 4);

		/* Bypass Disable Ch0 */
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0, 4);
		mask = AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0_DATAMASK;
		bypass_disable = rd_data & mask & 0xfffffcff;

#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0= %0x\n",
			 bypass_disable);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch0,
			      bypass_disable, 4);

		bypass_disable = 0;
		mask = 0;

		/* Bypass Disable Ch1 */
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1, 4);
		mask = AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1_DATAMASK;
		bypass_disable = rd_data & mask & 0xfffffcff;
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1= %0x\n",
			 bypass_disable);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch1,
			      bypass_disable, 4);

		bypass_disable = 0;
		mask = 0;

		/* Bypass Disable Ch2 */
		rd_data =
			cpu_rd_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2, 4);
		mask = AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2_DATAMASK;
		bypass_disable = rd_data & mask & 0xfffffcff;
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2= %0x\n",
			 bypass_disable);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_USER_MDIV_Ch2,
			      bypass_disable, 4);

		/* resetb & post_resetb Assertion */
		reset_val =
			reset_val | (1 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETD)
			| (1 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETD)
			| (1 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETA);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_RESET= %0x\n", reset_val);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_RESET, reset_val, 4);

		add_delay(1000);

		reset_val = 0;
		/* resetb de-assertion */
		reset_val = (reset_val |
			     1 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETD |
			     0 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETA);

#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_RESET= %0x\n", reset_val);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_RESET, reset_val, 4);

		add_delay(1000);

		reset_val = 0;
		/* post_resetb de-assertion */
		reset_val = (reset_val |
			     0 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETD |
			     0 << AUDIO_AUD_FMM_IOP_PLL_0_RESET__RESETA);
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_FMM_IOP_PLL_0_RESET= %0x\n", reset_val);
#endif
		cpu_wr_single(AUDIO_AUD_FMM_IOP_PLL_0_RESET, reset_val, 4);

		post_log("\nWaiting For PLL Lock\n");

		do {
			rd_data =
				cpu_rd_single(
					AUDIO_AUD_FMM_IOP_PLL_0_LOCK_STATUS, 4);
			timeout--;
		} while ((rd_data != 0x00000001) & (timeout > 0));

		if (rd_data == ((0x00000001 & timeout) != 0))
			post_log("PLL Lock Is Asserted\n");
		else if (timeout == 0)
			post_log("\nERROR: PLL Lock Timeout\n");

		/* enable mclk */
		if (i2s_port_num == 0) {
			reset_val = 0x40000f;
		} else if (i2s_port_num == 1) {
			reset_val = 0x8000f0;
		} else {
			reset_val = 0;
			post_log("invalid i2s_port_num number %d\n",
				 i2s_port_num);
		}
#if AUDIO_DEBUG
		post_log("AUDIO_AUD_MISC_SEROUT_OE= %0x\n", reset_val);
#endif
		cpu_wr_single(AUDIO_AUD_MISC_SEROUT_OE, reset_val, 4);
	} else {
		post_log("AUDIO TEST MODE : PLL Configuration Disabled\n");
	}

	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP0, 0x00000000, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP1, 0x00000001, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP2, 0x00000002, 4);
	cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_GRP3, 0x00000003, 4);
}

void add_delay(uint32_t delay)
{
	uint32_t i;
	for (i = 0; i < delay; i++)
		;
}

/* --------------- Source FIFO Interrupt Set function ----------------- */
void audio_source_fifo_intr_set(uint32_t i2s_port_number)
{
	uint32_t wr_data = 0;

#if AUDIO_DEBUG
	post_log("---------------------------------------------------------\n");
	post_log("AUDIO C API : Source FIFO Interrupt\n");
	post_log("---------------------------------------------------------\n");
#endif

	/* I2S 0, 1, 2 */
	if (i2s_port_number == 7) {
		wr_data = (
			wr_data |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR__SOURCE_FIFO_0_UNDERFLOW |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR__SOURCE_FIFO_1_UNDERFLOW |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR__SOURCE_FIFO_2_UNDERFLOW |
			0 <<
			AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR__SOURCE_FIFO_3_UNDERFLOW);

		post_log("AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_ESR_ESR0_MASK_CLEAR, wr_data, 4);

		wr_data = 0;

		wr_data = (wr_data |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF0 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF1 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF2 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF3 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF4 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_IOP);

		post_log("AUDIO_AUD_INTH_R5F_MASK_CLEAR= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_INTH_R5F_MASK_CLEAR, wr_data, 4);
	}
}

/* --------------- Source Ring Buffer Interrupt Set function ---------------- */
void audio_source_rbf_intr_set(uint32_t i2s_port_number)
{
	uint32_t wr_data = 0;

#if AUDIO_DEBUG
	post_log("----------------------------------------------------\n");
	post_log("AUDIO C API : Source Ring Buffer Interrupt\n");
	post_log("----------------------------------------------------\n");
#endif

	/* I2S 0, 1, 2 */
	if (i2s_port_number == 7) {
		wr_data = (
			wr_data |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR__SOURCE_RINGBUF_0_UNDERFLOW |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR__SOURCE_RINGBUF_1_UNDERFLOW |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR__SOURCE_RINGBUF_2_UNDERFLOW |
			0 <<
			AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR__SOURCE_RINGBUF_3_UNDERFLOW);

		post_log("AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_ESR_ESR1_MASK_CLEAR, wr_data, 4);

		wr_data = 0;

		wr_data = (wr_data |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF0 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF1 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF2 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF3 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF4 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_IOP);

		post_log("AUDIO_AUD_INTH_R5F_MASK_CLEAR= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_INTH_R5F_MASK_CLEAR, wr_data, 4);
	}
}

/* ----------------- Source Ring Buffer Freemark Interrupt ----------------- */
void audio_source_rbf_freemark_intr_set(uint32_t i2s_port_number,
					uint32_t freemark_bytes)
{
	uint32_t wr_data = 0;

#if AUDIO_DEBUG
	post_log("-------------------------------------------------------\n");
	post_log("AUDIO C API : Source Ring Buffer Freemark Interrupt\n");
	post_log("-------------------------------------------------------\n");
#endif

	/* I2S 0, 1, 2 */
	if (i2s_port_number == 7) {
		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_0_FREE_MARK,
			 freemark_bytes, 4);

		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_1_FREE_MARK,
			 freemark_bytes, 4);

		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_2_FREE_MARK,
			 freemark_bytes, 4);

		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_3_FREE_MARK,
			 freemark_bytes, 4);

		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_4_FREE_MARK,
			 freemark_bytes, 4);

		post_log("AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_FREE_MARK");
		post_log("= %0x\n", freemark_bytes);
		cpu_wr_single
			(AUDIO_AUD_FMM_BF_CTRL_SOURCECH_RINGBUF_5_FREE_MARK,
			 freemark_bytes, 4);

		wr_data = 0x00000007;

		post_log("AUDIO_AUD_FMM_BF_CTRL_REARM_FREE_MARK= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_CTRL_REARM_FREE_MARK, wr_data,
			      4);

		wr_data = 0;

		wr_data = (
			wr_data |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR__SOURCE_RINGBUF_0_EXCEED_FREEMARK |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR__SOURCE_RINGBUF_1_EXCEED_FREEMARK |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR__SOURCE_RINGBUF_2_EXCEED_FREEMARK |
			1 <<
			AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR__SOURCE_RINGBUF_3_EXCEED_FREEMARK);

		post_log("AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR= %0x\n",
			 wr_data);
		cpu_wr_single(AUDIO_AUD_FMM_BF_ESR_ESR3_MASK_CLEAR, wr_data, 4);

		wr_data = 0;

		wr_data = (wr_data |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF0 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF1 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF2 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF3 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_BF4 |
			   1 << AUDIO_AUD_INTH_R5F_MASK_CLEAR__AUD_IOP);

		post_log("AUDIO_AUD_INTH_R5F_MASK_CLEAR= %0x\n", wr_data);
		cpu_wr_single(AUDIO_AUD_INTH_R5F_MASK_CLEAR, wr_data, 4);
	}
}

/* -------------------------- DTE 54 DIV  Value --------------------------- */
void audio_dte_lts_div_54(uint32_t i2s0_bit_clk_div, uint32_t i2s1_bit_clk_div)
{
#if AUDIO_DEBUG
	post_log("--------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : I2S0 & 1 BIT CLOCK DIV VALUE\n");
	post_log("--------------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | i2s0_bit_clk_div << 16 | i2s1_bit_clk_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_54= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_54, wr_data, 4);
}

/*  ------------------------ DTE 76 DIV  Value -------------------------- */
void audio_dte_lts_div_76(uint32_t i2s2_bit_clk_div, uint32_t ws_i2s0_clk_div)
{
#if AUDIO_DEBUG
	post_log("-------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : I2S2 BIT & I2S0 LR CLOCK DIV VALUE\n");
	post_log("-------------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | i2s2_bit_clk_div << 16 | ws_i2s0_clk_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_76= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_76, wr_data, 4);
}

/*  ---------------------- DTE 98 DIV  Value ------------------------ */
void audio_dte_lts_div_98(uint32_t ws_i2s1_clk_div, uint32_t ws_i2s2_clk_div)
{
#if AUDIO_DEBUG
	post_log("------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : I2S1 & 2 LR CLOCK DIV VALUE\n");
	post_log("------------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | ws_i2s1_clk_div << 16 | ws_i2s2_clk_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_98= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_98, wr_data, 4);
}

/* ----------------------- DTE 1110 DIV  Value -------------------------- */
void audio_dte_lts_div_1110(uint32_t lts_ext_1_div, uint32_t lts_ext_2_div)
{
#if AUDIO_DEBUG
	post_log("-----------------------------------------------------\n");
	post_log("AUDIO C API : DTE : EXT. 1 & 2 DIV VALUE\n");
	post_log("-----------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | lts_ext_1_div << 16 | lts_ext_2_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_1110= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_1110, wr_data, 4);
}

/* ------------------------- DTE 1312 DIV  Value --------------------------- */
void audio_dte_lts_div_1312(uint32_t lts_ext_3_div, uint32_t lts_ext_4_div)
{
#if AUDIO_DEBUG
	post_log("-------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : EXT. 3 & 4 DIV VALUE\n");
	post_log("-------------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | lts_ext_3_div << 16 | lts_ext_4_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_1312= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_1312, wr_data, 4);
}

/* ------------------------- DTE 1514 DIV  Value ------------------------- */
void audio_dte_lts_div_1514(uint32_t lts_ext_5_div, uint32_t lts_ext_6_div)
{
#if AUDIO_DEBUG
	post_log("-------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : EXT. 5 & 6 DIV VALUE\n");
	post_log("-------------------------------------------------------\n");
#endif

	uint32_t wr_data = 0;

	wr_data = (wr_data | lts_ext_5_div << 16 | lts_ext_6_div << 0);

	post_log("AUDIO_EAV_DTE_DTE_LTS_DIV_14= %0x\n", wr_data);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_LTS_DIV_14, wr_data, 4);
}

/* --------------------- DTE Interrupt SOI  Value --------------------- */
void audio_dte_intr_config(uint32_t soi, uint32_t interval_length)
{
	uint32_t rd_data = 0;
	uint32_t wr_data = 0;

#if AUDIO_DEBUG
	post_log("---------------------------------------------------------\n");
	post_log("AUDIO C API : DTE : INTERRUPT CONFIGURATION\n");
	post_log("---------------------------------------------------------\n");
#endif

	post_log("AUDIO_EAV_DTE_DTE_ILEN= %0x\n", interval_length);
	cpu_wr_single(AUDIO_EAV_DTE_DTE_ILEN, interval_length, 4);

	rd_data = cpu_rd_single(AUDIO_EAV_DTE_DTE_NCO_TIME, 4);

	/* Multiplication of 16 is done with sum2 read data */
	/* as per the Figure "Figure 3 2 LTNCO Block Diagram", */
	/* the local time is left shift 4 of sum2 register */

	wr_data = (rd_data * 16) + soi;
	cpu_wr_single(AUDIO_EAV_DTE_DTE_NEXT_SOI, wr_data, 4);
#if AUDIO_DEBUG
	post_log("AUDIO_EAV_DTE_DTE_NEXT_SOI= %0x\n", wr_data);
#endif
}

int test_captured_samples(uint32_t *src_addr, uint32_t *dest_addr,
			  uint32_t samples)
{
	uint32_t i;
	uint32_t error;
	error = 0;
	for (i = 0; i < samples; i++) {
		if (*src_addr != *dest_addr)
			error++;
	}

	if (error == 0) {
		post_log("Test PASSED\n");
		return 0;
	} else {
		post_log("Test Failed\n");
		return -1;
	}
}

void smbus_init(uint32_t speed_mode)
{
	uint32_t data;

	data = readl(ChipcommonG_SMBus0_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus0_SMBus_Config = 0x%08X\n", data);
#endif
	data |= (1 << ChipcommonG_SMBus0_SMBus_Config__SMB_EN);
	writel(data, ChipcommonG_SMBus0_SMBus_Config);
	data = readl(ChipcommonG_SMBus0_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus0_SMBus_Config = 0x%08X\n", data);
#endif

	data = readl(ChipcommonG_SMBus0_SMBus_Config);
	data |= (1 << ChipcommonG_SMBus0_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus0_SMBus_Config);
	data &= ~(1 << ChipcommonG_SMBus0_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus0_SMBus_Config);

	if (speed_mode) {
		data = readl(ChipcommonG_SMBus0_SMBus_Timing_Config);
		data |= (1 << ChipcommonG_SMBus0_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus0_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS0 in 400KHz mode\n");
#endif
	} else {
		data = readl(ChipcommonG_SMBus0_SMBus_Timing_Config);
		data &=
			~(1 <<
			  ChipcommonG_SMBus0_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus0_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS0 in 100KHz mode\n");
#endif
	}
}

void smbus1_init(uint32_t speed_mode)
{
	uint32_t data;

	data = readl(ChipcommonG_SMBus1_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus1_SMBus_Config = 0x%08X\n", data);
#endif
	data |= (1 << ChipcommonG_SMBus1_SMBus_Config__SMB_EN);
	writel(data, ChipcommonG_SMBus1_SMBus_Config);
	data = readl(ChipcommonG_SMBus1_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus1_SMBus_Config = 0x%08X\n", data);
#endif

	data = readl(ChipcommonG_SMBus1_SMBus_Config);
	data |= (1 << ChipcommonG_SMBus1_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus1_SMBus_Config);
	data &= ~(1 << ChipcommonG_SMBus1_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus1_SMBus_Config);

	if (speed_mode) {
		data = readl(ChipcommonG_SMBus1_SMBus_Timing_Config);
		data |= (1 << ChipcommonG_SMBus1_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus1_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS1 in 400KHz mode\n");
#endif
	} else {
		data = readl(ChipcommonG_SMBus1_SMBus_Timing_Config);
		data &=
			~(1 <<
			  ChipcommonG_SMBus1_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus1_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS1 in 100KHz mode\n");
#endif
	}
}

int smbus_slave_presence(uint32_t slave_address)
{
	uint32_t data;

	writel((slave_address << 1),
	       ChipcommonG_SMBus0_SMBus_Master_Data_Write);
	writel(((0 << ChipcommonG_SMBus0_SMBus_Master_Command__SMBUS_PROTOCOL_R)
		| (1 << 31)), ChipcommonG_SMBus0_SMBus_Master_Command);

	do {
		data = readl(ChipcommonG_SMBus0_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	data = readl(ChipcommonG_SMBus0_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data == 0) {
#if AUDIO_DEBUG
		post_log("Slave present with address: 0x%08x\n", slave_address);
#endif
	} else {
		post_log("SMBUS Write tranasction error: 0x%08x - Addr : 0x%08X\n",
			 data, slave_address);
	}

	return 0;
}

void smbus3_init(uint32_t speed_mode)
{
	uint32_t data;

	data = readl(ChipcommonG_SMBus3_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus3_SMBus_Config = 0x%08X\n", data);
#endif
	data |= (1 << ChipcommonG_SMBus3_SMBus_Config__SMB_EN);
	writel(data, ChipcommonG_SMBus3_SMBus_Config);
	data = readl(ChipcommonG_SMBus3_SMBus_Config);
#if AUDIO_DEBUG
	post_log("ChipcommonG_SMBus3_SMBus_Config = 0x%08X\n", data);
#endif

	data = readl(ChipcommonG_SMBus3_SMBus_Config);
	data |= (1 << ChipcommonG_SMBus3_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus3_SMBus_Config);
	data &= ~(1 << ChipcommonG_SMBus3_SMBus_Config__RESET);
	writel(data, ChipcommonG_SMBus3_SMBus_Config);

	if (speed_mode) {
		data = readl(ChipcommonG_SMBus3_SMBus_Timing_Config);
		data |= (1 << ChipcommonG_SMBus3_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus3_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS3 in 400KHz mode\n");
#endif
	} else {
		data = readl(ChipcommonG_SMBus3_SMBus_Timing_Config);
		data &=
		    ~(1 << ChipcommonG_SMBus3_SMBus_Timing_Config__MODE_400);
		writel(data, ChipcommonG_SMBus3_SMBus_Timing_Config);
#if AUDIO_DEBUG
		post_log("Configuring SMBUS3 in 100KHz mode\n");
#endif
	}
}

int smbus1_slave_presence(uint32_t slave_address)
{
	uint32_t data;

	writel((slave_address << 1),
	       ChipcommonG_SMBus1_SMBus_Master_Data_Write);

	writel(((0 << ChipcommonG_SMBus1_SMBus_Master_Command__SMBUS_PROTOCOL_R)
		| (1 << 31)), ChipcommonG_SMBus1_SMBus_Master_Command);

	do {
		data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data == 0) {
#if AUDIO_DEBUG
		post_log("Slave present, address: 0x%08x\n", slave_address);
#endif
		return 0;
	} else {
		post_log("Write transaction error: 0x%08x - Addr : 0x%08X\n",
			 data, slave_address);
		return -1;
	}
}

int smbus3_slave_presence(uint32_t slave_address)
{
	uint32_t data;

#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, address: 0x%08x\n", slave_address);
#endif

	writel((slave_address << 1),
	       ChipcommonG_SMBus3_SMBus_Master_Data_Write);

#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, 1\n");
#endif
	writel(((0 << ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R)
		| (1 << 31)), ChipcommonG_SMBus3_SMBus_Master_Command);

#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, 2\n");
#endif
	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, data 0x%x\n", data);
#endif
	add_delay(1000);
	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, data 0x%x\n", data);
#endif
	add_delay(1000);
	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, data 0x%x\n", data);
#endif
	add_delay(1000);


	do {
		data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);
#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, 3\n");
#endif

	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

#if AUDIO_DEBUG
	post_log("smbus3_slave_presence, 4\n");
#endif
	if (data == 0) {
#if AUDIO_DEBUG
		post_log("Slave present, address: 0x%08x\n", slave_address);
#endif
		return 0;
	} else {
#if AUDIO_DEBUG
		post_log
		    ("Write transaction error: 0x%08x - Addr : 0x%08X\n",
		     data, slave_address);
#endif
		return -1;
	}
}

int smbus_poll(void)
{
	uint32_t data;

	do {
		data = readl(ChipcommonG_SMBus0_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	data = readl(ChipcommonG_SMBus0_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data == 0)
		return 0;
	else {
		post_log("Write transaction error: 0x%08x\n",
			 data);
		return -1;
	}
}
int smbus1_poll(void)
{
	uint32_t data;

	do {
		data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data == 0) {
		return 0;
	} else {
		post_log("Write transaction error: 0x%08x\n",
			 data);
		return -1;
	}
}

int smbus3_poll(void)
{
	uint32_t data;

	do {
		data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data == 0) {
		return 0;
	} else {
		post_log("Write transaction error: 0x%08x\n",
			 data);
		return -1;
	}
}

unsigned int smbus1_slave_read(uint32_t slave_address, int reg_add)
{
	unsigned int data_2 = 0;

	writel(((slave_address << 1) | 0),
	       ChipcommonG_SMBus1_SMBus_Master_Data_Write);
	writel(((reg_add) | 0x80000000),
	       ChipcommonG_SMBus1_SMBus_Master_Data_Write);
	writel(((0x5 <<
		 ChipcommonG_SMBus1_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)), ChipcommonG_SMBus1_SMBus_Master_Command);

	/* dummy read */
	(void)readl(ChipcommonG_SMBus1_SMBus_Master_Command);

	if (smbus1_poll())
		return -1;

	writel(((slave_address << 1) | 1),
	       ChipcommonG_SMBus1_SMBus_Master_Data_Write);
	writel(((0x4 <<
		 ChipcommonG_SMBus1_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)),
	       ChipcommonG_SMBus1_SMBus_Master_Command);

	if (!smbus1_poll())
		data_2 = readl(ChipcommonG_SMBus1_SMBus_Master_Data_Read);

	return data_2;
}

unsigned int smbus3_slave_read(uint32_t slave_address, int reg_add)
{
	unsigned int data_2 = 0;

	writel(((slave_address << 1) | 0),
	       ChipcommonG_SMBus3_SMBus_Master_Data_Write);
	writel(((reg_add) | 0x80000000),
	       ChipcommonG_SMBus3_SMBus_Master_Data_Write);
	writel(((0x5 <<
		 ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)),
	       ChipcommonG_SMBus3_SMBus_Master_Command);

	/* dummy read */
	(void)readl(ChipcommonG_SMBus3_SMBus_Master_Command);

	if (smbus3_poll())
		return -1;

	writel(((slave_address << 1) | 1),
	       ChipcommonG_SMBus3_SMBus_Master_Data_Write);
	writel(((0x4 <<
		 ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)),
	       ChipcommonG_SMBus3_SMBus_Master_Command);

	if (!smbus3_poll())
		data_2 = readl(ChipcommonG_SMBus3_SMBus_Master_Data_Read);

	return data_2;
}

int smbus_write(uint32_t slave_addr, uint32_t control_byte1,
		uint32_t control_byte2)
{
	uint32_t data;
	writel(((slave_addr << 1) | 0),
	       ChipcommonG_SMBus1_SMBus_Master_Data_Write);
	/* write to WM8750L */
	writel(control_byte1, ChipcommonG_SMBus1_SMBus_Master_Data_Write);
	writel(control_byte2, ChipcommonG_SMBus1_SMBus_Master_Data_Write);

	writel(((0x7 <<
		 ChipcommonG_SMBus1_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)), ChipcommonG_SMBus1_SMBus_Master_Command);

	/* dummy read */
	data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
	do {
		data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	/*check for smb_errors */
	data = readl(ChipcommonG_SMBus1_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data != 0) {
		if (slave_addr != 0x24)
			post_log("\n\rError occured: 0x%x\n\r",
				 data);
		return -1;
	} else {
#if AUDIO_DEBUG
		post_log("SMBUS PASSED - SLAVE ADDR: %08X ", slave_addr);
		post_log("WITH CTRL BYTE 1: %08X CTRL BYTE 2: %08X\n",
			 control_byte1, control_byte2);
#endif
		return 0;
	}
}

int smbus3_write(uint32_t slave_addr, uint32_t control_byte1,
		uint32_t control_byte2)
{
	uint32_t data;
	writel(((slave_addr << 1) | 0),
	       ChipcommonG_SMBus3_SMBus_Master_Data_Write);
	writel(control_byte1, ChipcommonG_SMBus3_SMBus_Master_Data_Write);
	writel(control_byte2, ChipcommonG_SMBus3_SMBus_Master_Data_Write);

	writel(((0x7 <<
		 ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)),
	       ChipcommonG_SMBus3_SMBus_Master_Command);

	/* dummy read */
	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
	do {
		data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
		data &= (1 << 31);
	} while (data);

	/*check for smb_errors */
	data = readl(ChipcommonG_SMBus3_SMBus_Master_Command);
	data = (data >> 25) & 0x7;

	if (data != 0) {
		if (slave_addr != 0x24)
			post_log("\n\rError occured ...!!!, error : 0x%x\n\r",
				 data);
		return -1;
	} else {
#if AUDIO_DEBUG
		post_log("SMBUS PASSED - SLAVE ADDR: %08X ", slave_addr);
		post_log("WITH CTRL BYTE 1: %08X CTRL BYTE 2: %08X\n",
			 control_byte1, control_byte2);
#endif
		return 0;
	}
}

int pca9555_write(uint32_t port, uint32_t slave_addr, uint32_t reg,
		  uint32_t byte1, uint32_t byte2)
{
	uint32_t data;
	uint32_t mdw_reg;
	uint32_t mc_reg;

#if AUDIO_DEBUG
	post_log("pca9555_write to port %d\n", port);
#endif

	switch (port) {
	case 0:
		mdw_reg = ChipcommonG_SMBus0_SMBus_Master_Data_Write;
		mc_reg = ChipcommonG_SMBus0_SMBus_Master_Command;
		break;
	case 1:
		mdw_reg = ChipcommonG_SMBus1_SMBus_Master_Data_Write;
		mc_reg = ChipcommonG_SMBus1_SMBus_Master_Command;
		break;
	case 2:
		mdw_reg = ChipcommonG_SMBus2_SMBus_Master_Data_Write;
		mc_reg = ChipcommonG_SMBus2_SMBus_Master_Command;
		break;
	case 3:
		mdw_reg = ChipcommonG_SMBus3_SMBus_Master_Data_Write;
		mc_reg = ChipcommonG_SMBus3_SMBus_Master_Command;
		break;
	default:
		post_log("ERROR port %d out of range\n", port);
		return -1;
	}
	writel((slave_addr << 1), mdw_reg);
#if AUDIO_DEBUG
	post_log("SMBus_Master_Data_Write 0x%x = 0x%x\n", mdw_reg, (slave_addr << 1));
#endif

	writel(reg, mdw_reg);
#if AUDIO_DEBUG
	post_log("SMBus_Master_Data_Write 0x%x = 0x%x\n",  mdw_reg, reg);
#endif

	writel(byte1,  mdw_reg);
#if AUDIO_DEBUG
	post_log("SMBus_Master_Data_Write 0x%x = 0x%x\n", mdw_reg, byte1);
#endif

	byte2 |= 0x80000000;
	writel(byte2,  mdw_reg);
#if AUDIO_DEBUG
	post_log("SMBus_Master_Data_Write 0x%x = 0x%x\n", mdw_reg, byte2);
#endif

	writel(((0x7 <<
		 ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		(1 << 31)), mc_reg);
#if AUDIO_DEBUG
	post_log("SMBus_Master_Data_Write 0x%x = 0x%x\n",
		 reg, (0x7 <<
		 ChipcommonG_SMBus3_SMBus_Master_Command__SMBUS_PROTOCOL_R) |
		 (1 << 31));
#endif

	/* dummy read */
	data = readl(mc_reg);
	do {
		data = readl(mc_reg);
		data &= (1 << 31);
	} while (data);

	/*check for smb_errors */
	data = readl(mc_reg);
	data = (data >> 25) & 0x7;

	if (data != 0) {
		if (slave_addr != 0x24)
			post_log("pca9555_write error occurred: 0x%x\n\r",
				 data);
		return -1;
	} else {
#if AUDIO_DEBUG
		post_log
		    ("pca9555_write - addr 0x%x reg 0x%x, data 0x%x 0x%x\n",
		     slave_addr, reg, byte1, (byte2 & 0xff));
#endif
		return 0;
	}
}
