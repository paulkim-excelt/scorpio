/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef __IPROC_REGS_H
#define __IPROC_REGS_H __FILE__
#include <linux/types.h>
#include "socregs.h"

#define IPROC_NUM_CPUS			(2)
#define IPROC_NUM_IRQS			(256) /* Number of interrupts */
#define IPROC_CPU0_MIN_INT_PRIORITY	(0)
#define IPROC_CPU1_MIN_INT_PRIORITY	(0)

#define IPROC_DDR_MEM_BASE1		(0x0)
#define IPROC_DDR_MEM_BASE2		(0x60000000)

#define IPROC_CCA_REG_BASE		(CHIPCOMMONA_CHIPID)
#define IPROC_CCB_GPIO_REG_BASE		(CHIPCOMMONB_GP_DATA_IN)
#define IPROC_CCB_PWM_REG_BASE		(CHIPCOMMONB_PWMCTL)
#define IPROC_CCB_MDIO_REG_BASE		(CHIPCOMMONB_MII_MANAGEMENT_CTRL)
#define IPROC_CCB_RNG_REG_BASE		(CHIPCOMMONB_rng_CTRL)
#define IPROC_CCB_TIM0_REG_BASE		(CHIPCOMMONB_TIM0_TIM_TIMER1LOAD)
#define IPROC_CCB_TIM1_REG_BASE		(CHIPCOMMONB_TIM1_TIM_TIMER1LOAD)
#define IPROC_CCB_SRAU_REG_BASE		(0x18036000)
#define IPROC_CCB_UART0_REG_BASE	(CHIPCOMMONB_UART0_RBR_THR_DLL)
#define IPROC_DDRC_REG_BASE		(DDR_DENALI_CTL_00)
#define IPROC_DMAC_REG_BASE		(DMAC_pl330_non_DS)
#define IPROC_PCIE_AXIB0_REG_BASE	(PAXB_0_PCIE_CONTROL)
#define IPROC_PCIE_AXIB1_REG_BASE	(PAXB_1_PCIE_CONTROL)
#define IPROC_PCIE_AXIB2_REG_BASE	(PAXB_2_PCIE_CONTROL)
#define IPROC_SDIO3_REG_BASE		(SDIO_eMMCSDXC_SYSADDR)
#define IPROC_USB20_REG_BASE		(0x18021000)
#define IPROC_USB30_REG_BASE		(0x18022000)
#define IPROC_USB20_PHY_REG_BASE	(0x18023000)
#define IPROC_GMAC0_REG_BASE		(GMAC_PM_ROOT + DEVCONTROL_BASE)
#define AMAC_IDM_RESET_CONTROL (AMAC_M_IDM_ROOT + AMAC_M_IDM_RESET_CONTROL)
#define AMAC_IDM2_IDM_RESET_CONTROL (AMAC_M_IDM_ROOT + AMAC_M_IDM_RESET_CONTROL)
#define AMAC_IDM3_IDM_RESET_CONTROL (AMAC_IDM2_IDM_RESET_CONTROL)

/* ARM9 Private memory region */
#define IPROC_PERIPH_BASE		(0x19020000)
#define IPROC_PERIPH_INT_CTRL_REG_BASE	(IPROC_PERIPH_BASE + 0x100)
#define IPROC_PERIPH_GLB_TIM_REG_BASE	(IPROC_PERIPH_BASE + 0x200)
#define IPROC_PERIPH_PVT_TIM_REG_BASE	(IPROC_PERIPH_BASE + 0x600)
#define IPROC_PERIPH_PVT_WDT_REG_BASE	(IPROC_PERIPH_BASE + 0x620)
#define IPROC_PERIPH_INT_DISTR_REG_BASE (IPROC_PERIPH_BASE + 0x1000)
#define IPROC_L2CC_REG_BASE		(IPROC_PERIPH_BASE + 0x2000)

typedef struct iproc_clk_struct_t {
	u32 arm_clk; /* A9 core clock */
	u32 arm_periph_clk;
	u32 axi_clk;
	u32 apb_clk;
} iproc_clk_struct;

/* Structures and bit definitions */
/* SCU Control register */
#define IPROC_SCU_CTRL_SCU_EN		(0x00000001)
#define IPROC_SCU_CTRL_ADRFLT_EN	(0x00000002)
#define IPROC_SCU_CTRL_PARITY_EN	(0x00000004)
#define IPROC_SCU_CTRL_SPEC_LNFL_EN	(0x00000008)
#define IPROC_SCU_CTRL_FRC2P0_EN	(0x00000010)
#define IPROC_SCU_CTRL_SCU_STNDBY_EN	(0x00000020)
#define IPROC_SCU_CTRL_IC_STNDBY_EN	(0x00000040)

typedef struct scu_reg_struct_t {
	u32 control;
	u32 config;
	u32 cpupwrstatus;
	u32 invalidate;
	u32 rsvd1[4];
	u32 rsvd2[4];
	u32 rsvd3[4];
	u32 filtstart;
	u32 filtend;
	u32 rsvd4[2];
	u32 sac;
	u32 snsac;
} scu_reg_struct, *scu_reg_struct_ptr;

/* ARM A9 Private Timer */
#define IPROC_PVT_TIM_CTRL_TIM_EN		(0x00000001)
#define IPROC_PVT_TIM_CTRL_AUTO_RELD		(0x00000002)
#define IPROC_PVT_TIM_CTRL_INT_EN		(0x00000004)
#define IPROC_PVT_TIM_CTRL_PRESC_MASK		(0x0000FF00)
#define IPROC_PVT_TIM_INT_STATUS_SET		(0x00000001)

typedef struct pvt_tim_reg_struct_t {
	u32 load;
	u32 counter;
	u32 control;
	u32 intstatus;
} pvt_tim_reg_struct, *pvt_tim_reg_struct_ptr;

/* Global timer */
#define IPROC_GLB_TIM_CTRL_TIM_EN		(0x00000001)
#define IPROC_GLB_TIM_CTRL_COMP_EN		(0x00000002)
#define IPROC_GLB_TIM_CTRL_INT_EN		(0x00000004)
#define IPROC_GLB_TIM_CTRL_AUTO_INC		(0x00000008)
#define IPROC_GLB_TIM_CTRL_PRESC_MASK		(0x0000FF00)
#define IPROC_GLB_TIM_INT_STATUS_SET		(0x00000001)

typedef struct glb_tim_reg_struct_t {
	u32 counter_l;
	u32 counter_h;
	u32 control;
	u32 intstatus;
	u32 cmp_l;
	u32 cmp_h;
	u32 reload;
} glb_tim_reg_struct, *glb_tim_reg_struct_ptr;

/* GIC(Generic Interrupt controller) definitions */

typedef struct intr_data_struct_t {
	u32 cpuid;
	u32 intid;
	void *data;
} intr_data_struct, *intr_data_struct_ptr;

/* GIC(Generic Interrupt controller) CPU interface registers */
#define IPROC_GIC_CI_CTRL_EN			(0x00000001)
#define IPROC_GIC_CI_PMR_PRIO_MASK		(0x000000FF)
#define IPROC_GIC_CI_BPR_BP_MASK		(0x00000003)
#define IPROC_GIC_CI_IAR_INTID_MASK		(0x000003FF)
#define IPROC_GIC_CI_IAR_CPUID_MASK		(0x00001C00)
#define IPROC_GIC_CI_IAR_CPUID_OFFSET		(10)
#define IPROC_GIC_CI_EOIR_INTID_MASK		(0x000003FF)
#define IPROC_GIC_CI_EOIR_CPUID_MASK		(0x00001C00)
#define IPROC_GIC_CI_EOIR_CPUID_OFFSET		(10)
#define IPROC_GIC_CI_RPR_PRIO_MASK		(0x000000FF)
#define IPROC_GIC_CI_HPIR_PENDID_MASK		(0x000003FF)
#define IPROC_GIC_CI_HPIR_CPUID_MASK		(0x00001C00)
#define IPROC_GIC_CI_HPIR_CPUID_OFFSET		(10)
#define IPROC_GIC_CI_ABPR_BP_MASK		(0x00000003)

typedef struct gic_ci_reg_struct_t {
	u32 control;	/* Control reg */
	u32 pmr;	/* Interrupt Priority mask reg */
	u32 bpr;	/* Binary point reg */
	u32 iar;	/* interrupt acknowledge reg */
	u32 eoir;	/* End of interrupt reg */
	u32 rpr;	/* running priority register */
	u32 hpir;	/* Highest pending interrupt register */
	u32 abpr;	/* Aliased Non-Secure Binary point register */
	u32 rsvd[55];
	u32 idr;	/* CPU Interface Implementer Identification register */
} gic_ci_reg_struct, *gic_ci_reg_struct_ptr;

#define IPROC_GIC_DIST_CTRL_S_EN_S		(0x00000001)
#define IPROC_GIC_DIST_CTRL_S_EN_NS		(0x00000002)
#define IPROC_GIC_DIST_CTRL_NS_EN_NS		(0x00000001)
#define IPROC_GIC_DIST_ISR_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ISER_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ICER_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ISPR_BIT_SIZE		(1)
#define IPROC_GIC_DIST_ISPR_SECURE		(1)
#define IPROC_GIC_DIST_ISPR_NON_SECURE		(0)
#define IPROC_GIC_DIST_ICPR_BIT_SIZE		(1)
#define IPROC_GIC_DIST_IPR_BIT_SIZE		(8)
#define IPROC_GIC_DIST_IPTR_BIT_SIZE		(8)
#define IPROC_GIC_DIST_IPTR_CPU0		(0x01)
#define IPROC_GIC_DIST_IPTR_CPU1		(0x02)
#define IPROC_GIC_DIST_SGIR_ID_MASK		(0xF)
#define IPROC_GIC_DIST_SGIR_TR_LIST_MASK	(0x00FF0000)
#define IPROC_GIC_DIST_SGIR_TR_LIST_BOFFSET	(16)
#define IPROC_GIC_DIST_SGIR_TR_FILT_MASK	(0x03000000)
#define IPROC_GIC_DIST_SGIR_TR_FILT_BOFFSET	(24)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_LIST		(0)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_ALL_EX_ME	(0x01)
#define IPROC_GIC_DIST_SGIR_TR_FILT_FW_ME_ONLY		(0x02)

#define IPROC_INTR_LEVEL_SENSITIVE			(1)
#define IPROC_INTR_EDGE_TRIGGERED			(2)

typedef struct gic_dist_reg_struct_t {
	u32 control;	/* Control reg */
	u32 ictr;	/* Interrupt controller type reg */
	u32 idr;	/* Distributor Implementer Identification register */
	u32 rsvd1[1];
	u32 rsvd2[28];
	u32 isr[8];	/* interrupt security reg */
	u32 rsvd3[24];
	u32 iser[8];	/* interrupt set-enable reg */
	u32 rsvd4[24];
	u32 icer[8];	/* interrupt clear-enable reg */
	u32 rsvd5[24];
	u32 ispr[8];	/* interrupt set-pending reg */
	u32 rsvd6[24];
	u32 icpr[8];	/* interrupt clear-pending reg */
	u32 rsvd7[24];
	u32 abr[8];	/* Active bit reg */
	u32 rsvd8[24];
	u32 rsvd9[32];
	u32 ipr[64];	/* Interrupt priority reg */
	u32 rsvd10[192];
	u32 iptr[64];	/* Interrupt processor targets reg */
	u32 rsvd11[192];
	u32 icfr[16];	/* Interrupt configuration reg */
	u32 rsvd12[48];
	u32 ppi_status; /* PPI status register */
	u32 spi_status[7]; /* SPI status register */
	u32 rsvd13[24];
	u32 rsvd14[32];
	u32 rsvd15[64];
	u32 sgir;	/* Software generated interrupt reg */
	u32 rsvd16[51];
	u32 periph_id[8];
	u32 component_id[4];
} gic_dist_reg_struct, *gic_dist_reg_struct_ptr;

typedef struct l2cc_reg_struct_t {
	u32 cache_id;
	u32 cache_type;
	u32 rsvd1[62];
	u32 control;	/* 0x100 */
	u32 aux_control;
	u32 tag_ram_control;
	u32 data_ram_control;
	u32 rsvd2[60];
	u32 ev_counter_ctrl;	/* 0x200 */
	u32 ev_counter1_cfg;
	u32 ev_counter0_cfg;
	u32 ev_counter1;
	u32 ev_counter0;
	u32 int_mask;
	u32 int_mask_status;
	u32 int_raw_status;
	u32 int_clear;
	u32 rsvd3[55];
	u32 rsvd4[64]; /* 0x300 */
	u32 rsvd5[64]; /* 0x400 */
	u32 rsvd6[64]; /* 0x500 */
	u32 rsvd7[64]; /* 0x600 */
	u32 rsvd8[12]; /* 0x700 - 0x72F */
	u32 cache_sync; /* 0x730 */
	u32 rsvd9[15];
	u32 inv_pa; /* 0x770 */
	u32 rsvd10[2];
	u32 inv_way; /* 0x77C */
	u32 rsvd11[12];
	u32 clean_pa; /* 0x7B0 */
	u32 rsvd12[1];
	u32 clean_index; /* 0x7B8 */
	u32 clean_way;
	u32 rsvd13[12];
	u32 clean_inv_pa; /* 0x7F0 */
	u32 rsvd14[1];
	u32 clean_inv_index;
	u32 clean_inv_way;
	u32 rsvd15[64]; /* 0x800 - 0x8FF*/
	u32 d_lockdown0; /* 0x900 */
	u32 i_lockdown0;
	u32 d_lockdown1;
	u32 i_lockdown1;
	u32 d_lockdown2;
	u32 i_lockdown2;
	u32 d_lockdown3;
	u32 i_lockdown3;
	u32 d_lockdown4;
	u32 i_lockdown4;
	u32 d_lockdown5;
	u32 i_lockdown5;
	u32 d_lockdown6;
	u32 i_lockdown6;
	u32 d_lockdown7;
	u32 i_lockdown7;
	u32 rsvd16[4]; /* 0x940 */
	u32 lock_line_en; /* 0x950 */
	u32 unlock_way;
	u32 rsvd17[42];
	u32 rsvd18[64]; /* 0xA00 */
	u32 rsvd19[64]; /* 0xB00 */
	u32 addr_filtering_start; /* 0xC00 */
	u32 addr_filtering_end;
	u32 rsvd20[62];
	u32 rsvd21[64]; /* 0xD00 */
	u32 rsvd22[64]; /* 0xE00 */
	u32 rsvd23[16]; /* 0xF00 - 0xF3F */
	u32 debug_ctrl; /* 0xF40 */
	u32 rsvd24[7];
	u32 prefetch_ctrl; /* 0xF60 */
	u32 rsvd25[7];
	u32 power_ctrl; /* 0xF80 */
} l2cc_reg_struct, *l2cc_reg_struct_ptr;

#endif /*__IPROC_REGS_H */
