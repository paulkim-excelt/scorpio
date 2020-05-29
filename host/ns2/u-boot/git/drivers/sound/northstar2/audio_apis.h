/*
 * $Copyright Open Broadcom Corporation$
 *
 * audio_apis.h
 *
 * This file contains definitions for routines in audio_apis.c
 * for Northstar2.
 *
 */

#ifndef _AUDIO_APIS_H
#define _AUDIO_APIS_H

#include <asm/arch/socregs.h>
#include <common.h>
#include <linux/compiler.h>
#include <post.h>

#define BIT8     0x1
#define BIT16    0x2
#define BIT32    0x4
#define BIT64    0x8

#define cpu_rd_single_ns2(addr, size) *((volatile uint32_t*)addr)

#define cpu_wr_single_ns2(addr, data, size) \
	do { \
		*((volatile uint32_t *)(addr)) = (uint32_t) (data); \
	} while (0)

#define I2S_ADDR(addr)	(I2S_ROOT+addr)
#define cpu_rd_single(addr, size) cpu_rd_single_ns2(I2S_ADDR(addr), size)
#define cpu_wr_single(addr, data, size)  \
	cpu_wr_single_ns2(I2S_ADDR(addr), data, size)

int smbus_wm8750_write(uint32_t slave_addr, uint32_t control_byte1,
		       uint32_t control_byte2);
int smbus_write(uint32_t slave_addr, uint32_t control_byte1,
		uint32_t control_byte2);
void smbus_init(uint32_t speed_mode);
int smbus_slave_presence(uint32_t slave_address);
void smbus1_init(uint32_t speed_mode);
int smbus1_slave_presence(uint32_t slave_address);
unsigned int smbus1_slave_read(uint32_t slave_address, int reg_add);

void crmu_reset_related(void);

void audio_sw_reset(void);
void audio_io_mux_select(void);
void audio_clock_gating_disable(void);
void audio_pad_enable(void);

void dte_register_write(uint32_t addr, uint32_t value);
uint32_t dte_register_read(uint32_t addr);

void audio_dte_lts_src_en(uint32_t src_en_data,
			  uint32_t audio_dte_both_edge_en);
void audio_dte_lts_div_54(uint32_t i2s0_bit_clk_div, uint32_t i2s1_bit_clk_div);
void audio_dte_lts_div_76(uint32_t i2s2_bit_clk_div, uint32_t ws_i2s0_clk_div);
void audio_dte_lts_div_98(uint32_t ws_i2s0_clk_div, uint32_t ws_i2s1_clk_div);
void audio_dte_lts_div_1110(uint32_t lts_ext_1_div, uint32_t lts_ext_2_div);
void audio_dte_lts_div_1312(uint32_t lts_ext_3_div, uint32_t lts_ext_4_div);
void audio_dte_lts_div_1514(uint32_t lts_ext_5_div, uint32_t lts_ext_6_div);
void audio_dte_intr_config(uint32_t soi, uint32_t interval_length);

void audio_spdif_stream_config(uint32_t ena, uint32_t channel_grouping,
			       uint32_t group_id,
			       uint32_t stream_bit_resolution,
			       uint32_t wait_for_valid,
			       uint32_t ignore_first_underflow,
			       uint32_t init_sm, uint32_t ins_inval,
			       uint32_t fci_id);

void audio_spdif_ctrl(uint32_t dither_value, uint32_t wait_pcm_to_comp,
		      uint32_t length_code, uint32_t overwrite_data,
		      uint32_t comp_or_linear, uint32_t flush_on_uflow,
		      uint32_t insert_on_uflow, uint32_t insert_when_disa,
		      uint32_t spdif_bypass, uint32_t cp_toggle_rate,
		      uint32_t dither_width, uint32_t dither_ena,
		      uint32_t hold_cstat, uint32_t validity);

void audio_spdif_ch_status(uint32_t ch_status_0, uint32_t ch_status_1,
			   uint32_t ch_status_2);
void audio_spdif_ramp_burst(uint32_t stepsize, uint32_t rampup_steps,
			    uint32_t stop_bit, uint32_t burst_type,
			    uint32_t rep_period);
void audio_spdif_format_config(uint32_t lr_select, uint32_t limit_to_16_bits,
			       uint32_t pream_pol, uint32_t data_enable,
			       uint32_t clock_enable);
void audio_spdif_mclk_config(uint32_t mclk_rate, uint32_t pllclksel);
void audio_spdif_sample_count(uint32_t spdif_count_start,
			      uint32_t spdif_count_clear);

void audio_i2s_in_rx_config(uint32_t cap_ena, uint32_t cap_group_id,
			    uint32_t ignore_first_overflow,
			    uint32_t lrck_polarity, uint32_t sclk_polarity,
			    uint32_t data_alignment,
			    uint32_t data_justification,
			    uint32_t bits_per_sample, uint32_t bits_per_slot,
			    uint32_t valid_slot, uint32_t slave_mode,
			    uint32_t tdm_mode, uint32_t i2s_port_num);

void audio_fmm_bf_ctrl_dest_cfg(uint32_t process_seq_id_valid,
				uint32_t blocked_access_disable,
				uint32_t process_id_high,
				uint32_t dma_block_cnt, uint32_t reverse_endian,
				uint32_t capture_mode, uint32_t fci_cap_id,
				uint32_t not_pause_when_full,
				uint32_t source_fifo_id,
				uint32_t input_frm_sourcefifo,
				uint32_t capture_to_sourcefifo,
				uint32_t play_from_capture,
				uint32_t destfifo_size_double,
				uint32_t buffer_pair_enable,
				uint32_t capture_enable, uint32_t i2s_port_num);

void audio_i2s_out_tx_config(uint32_t clock_enable, uint32_t data_enable,
			     uint32_t lrck_polarity, uint32_t sclk_polarity,
			     uint32_t data_alignment,
			     uint32_t data_justification,
			     uint32_t bits_per_sample, uint32_t bits_per_slot,
			     uint32_t valid_slot, uint32_t fsync_width,
			     uint32_t sclk_per_1fs, uint32_t slave_mode,
			     uint32_t tdm_mode, uint32_t i2s_port_num);

void audio_i2s_stream_config_samp_count(uint32_t ena, uint32_t channel_grouping,
					uint32_t group_id,
					uint32_t stream_bit_resolution,
					uint32_t wait_for_valid,
					uint32_t ignore_first_underflow,
					uint32_t init_sm, uint32_t ins_inval,
					uint32_t fci_id, uint32_t i2s_port_num);

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
				  uint32_t i2s_port_num);

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
				  uint32_t spdif_en);

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
				uint32_t audio_address_difference);

void audio_start_dma_read(uint32_t spdif_en, uint32_t play_run,
			  uint32_t i2s_port_num);
void audio_start_capture_inputs(uint32_t capture_run, uint32_t i2s_port_num);
void audio_i2s_out_legacy_enable(uint32_t legacy_en, uint32_t i2s_port_num);
void audio_i2s_out_sample_count(uint32_t count_start, uint32_t count_clear,
				uint32_t i2s_port_num);
void audio_i2s_in_sample_count(uint32_t in_count_start, uint32_t in_count_clear,
			       uint32_t i2s_port_num);
void audio_i2s_in_legacy_enable(uint32_t legacy_en, uint32_t i2s_port_num);
void audio_mclk_cfg(uint32_t pllclksel, uint32_t mclk_rate,
		    uint32_t i2s_port_num);
void audio_gen_pll_pwr_on(uint32_t crmu_pll_pwr_on);

void audio_gen_pll_group_id_config(uint32_t pdiv, uint32_t ndiv_int,
				   uint32_t ndiv_frac, uint32_t mdiv,
				   uint32_t gain_ki, uint32_t gain_ka,
				   uint32_t gain_kp, uint32_t user_macro,
				   uint32_t i2s_port_num,
				   uint32_t audio_ext_test_clock_en);

void add_delay(uint32_t delay);
void audio_WRITE_REG_api(uint32_t addr, uint32_t data);
uint32_t audio_READ_REG_api(uint32_t addr);
void audio_source_fifo_intr_set(uint32_t i2s_port_number);
void audio_source_rbf_intr_set(uint32_t i2s_port_number);
void audio_source_rbf_freemark_intr_set(uint32_t i2s_port_number,
					uint32_t freemark_bytes);
void dte_iomux_select_external_inputs(void);
int test_captured_samples(uint32_t *src_addr,
			  uint32_t *dest_addr,
			  uint32_t samples);
uint32_t iomux_select_i2s_0_if_en_interface(void);

#endif
