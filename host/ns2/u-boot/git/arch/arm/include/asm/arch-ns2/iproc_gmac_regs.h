/*
 * $Copyright Open Broadcom Corporation$
 */

#ifndef	__IPROC_GMAC_REGS_H
#define	__IPROC_GMAC_REGS_H

#include "bcmgmacmib.h"

/* cpp contortions to concatenate w/arg prescan */
#ifndef PAD
#define	_PADLINE(line)	pad ## line
#define	_XSTR(line)		_PADLINE(line)
#define	PAD				_XSTR(__LINE__)
#endif /* PAD */

/* We have 4 DMA TX channels */
#define	GMAC_NUM_DMA_TX		4

/* dma registers per channel(xmt or rcv) */
typedef struct {
	u32 control;	/* 0x00 enable, et al */
	u32 ptr;	/* 0x04 last descriptor posted to chip */
	u32 addrlow;	/* 0x08 descr ring base addr low 32-bits (8K aligned) */
	u32 addrhigh;	/* 0x0c descr ring base addr bits 63:32 (8K aligned) */
	u32 status0;	/* 0x10 current descriptor, xmt state */
	u32 status1;	/* 0x14 active descriptor, xmt error */
	u32 PAD[2];	/* 0x18-0x1c */
} dma64regs_t;

typedef struct {
	dma64regs_t	dmaxmt;		/* dma tx */
	dma64regs_t	dmarcv;		/* dma rx */
} dma64_t;

/*
 * Host Interface Registers
 */
typedef struct _gmacregs {
	u32 dev_ctl;			/* 0x000 */
	u32 dev_status;			/* 0x004 */
	u32 PAD;			/* 0x008 */
	u32 bist_status;		/* 0x00c */
	u32 PAD[4];			/* 0x010-0x01c */
	u32 int_status;			/* 0x020 */
	u32 int_mask;			/* 0x024 */
	u32 gp_timer;			/* 0x028 */
	u32 PAD[53];			/* 0x02c-0x0fc */
	u32 int_recv_lazy;		/* 0x100 */
	u32 flow_ctl_thresh;		/* 0x104 */
	u32 tx_arb_wrr_thresh;		/* 0x108 */
	u32 gmac_idle_cnt_thresh;	/* 0x10c */
	u32 PAD[4];			/* 0x110-0x11c */
	u32 fifo_access_addr;		/* 0x120 */
	u32 fifo_access_byte;		/* 0x124 */
	u32 fifo_access_data;		/* 0x128 */
	u32 PAD[21];			/* 0x12c-0x17c */
	u32 phy_access;			/* 0x180 */
	u32 PAD;			/* 0x184 */
	u32 phy_ctl;			/* 0x188 */
	u32 txq_ctl;			/* 0x18c */
	u32 rxq_ctl;			/* 0x190 */
	u32 gpio_select;		/* 0x194 */
	u32 gpio_output_en;		/* 0x198 */
	u32 PAD;			/* 0x19c */
	u32 txq_rxq_mem_ctl;		/* 0x1a0 */
	u32 memory_ecc_status;		/* 0x1a4 */
	u32 PAD[14];			/* 0x1a8-1dc */
	u32 clk_ctl_status;		/* 0x1e0 */
	u32 PAD;			/* 0x1e4 */
	u32 pwr_ctl;			/* 0x1e8 */
	u32 PAD[5];			/* 0x1ec-0x1fc */
	dma64_t	dma_regs[GMAC_NUM_DMA_TX]; /* 0x200-0x2fc */

	/* GAMC MIB counters */
	struct gmacmib mib;		/* 0x300-0x428 */
	u32 PAD[245];			/* 0x42c-0x7fc */

	/* UNIMAC registers*/
	u32 unimac_version;		/* 0x800 */
	u32 hd_bk_pctl;			/* 0x804 */
	u32 cmd_cfg;			/* 0x808 */
	u32 mac_addr_high;		/* 0x80c */
	u32 mac_addr_low;		/* 0x810 */
	u32 rx_max_length;		/* 0x814 */
	u32 pause_quanta;		/* 0x818 */
	u32 PAD[9];			/* 0x81c-x83c */
	u32 sfd_offset;			/* 0x840 */
	u32 mac_mode;			/* 0x844 */
	u32 outer_tag;			/* 0x848 */
	u32 inner_tag;			/* 0x84c */
	u32 rx_pause_quanta_scale;	/* 0x850 */
	u32 tx_preamble;		/* 0x854 */
	u32 PAD;			/* 0x858 */
	u32 tx_ipg_len;			/* 0x85c */
	u32 PAD[168];			/* 0x860-0xafc */
	u32 mac_pfc_type;		/* 0xb00 */
	u32 mac_pfc_opcode;		/* 0xb04 */
	u32 mac_pfc_da_0;		/* 0xb08 */
	u32 mac_pfc_da_1;		/* 0xb0c */
	u32 macsec_prog_tx_crc;		/* 0xb10 */
	u32 macsec_ctl;			/* 0xb14 */
	u32 ts_status_ctl;		/* 0xb18 */
	u32 tx_ts_data;			/* 0xb1c */
	u32 PAD[4];			/* 0xb20-0xb2c */
	u32 pause_ctl;			/* 0xb30 */
	u32 flush_ctl;			/* 0xb34 */
	u32 rx_fifo_status;		/* 0xb38 */
	u32 tx_fifo_status;		/* 0xb3c */
	u32 mac_pfc_ctl;		/* 0xb40 */
	u32 mac_pfc_refresh_ctl;	/* 0xb44 */
} gmacregs_t;

#define	GM_MIB_BASE			0x300
/* #define	GM_MIB_LIMIT	0x800 */
#define	GM_MIB_LIMIT	0x42c

/*
 * register-specific flag definitions
 */

/* device control */
#define	DC_TSM				0x00000001
#define	DC_ROCS				0x00000002
#define	DC_CFCO				0x00000004
#define	DC_RLSS				0x00000008
#define	DC_MROR				0x00000010
#define	DC_FCM_MASK			0x00000060
#define	DC_FCM_SHIFT			5
#define	DC_NAE				0x00000080
#define	DC_TF				0x00000100
#define	DC_RDS_MASK			0x00030000
#define	DC_RDS_SHIFT			16
#define	DC_TDS_MASK			0x000c0000
#define	DC_TDS_SHIFT			18

/* device status */
#define	DS_RBF				0x00000001
#define	DS_RDF				0x00000002
#define	DS_RIF				0x00000004
#define	DS_TBF				0x00000008
#define	DS_TDF				0x00000010
#define	DS_TIF				0x00000020
#define	DS_PO				0x00000040
#define	DS_MM_MASK			0x00000300
#define	DS_MM_SHIFT			8

/* bist status */
#define	BS_MTF				0x00000001
#define	BS_MRF				0x00000002
#define	BS_TDB				0x00000004
#define	BS_TIB				0x00000008
#define	BS_TBF				0x00000010
#define	BS_RDB				0x00000020
#define	BS_RIB				0x00000040
#define	BS_RBF				0x00000080
#define	BS_SF				0x00000800

/* interrupt status and mask registers */
#define	I_MRO				0x00000001
#define	I_MTO				0x00000002
#define	I_TFD				0x00000004
#define	I_LS				0x00000008
#define	I_MDIO				0x00000010
#define	I_MR				0x00000020
#define	I_MT				0x00000040
#define	I_TO				0x00000080
#define	I_SLSC				0x00000100
#define	I_PDEE				0x00000400
#define	I_PDE				0x00000800
#define	I_DE				0x00001000
#define	I_RDU				0x00002000
#define	I_RFO				0x00004000
#define	I_XFU				0x00008000
#define	I_RI				0x00010000
#define	I_XI0				0x01000000
#define	I_XI1				0x02000000
#define	I_XI2				0x04000000
#define	I_XI3				0x08000000
#define	I_INTMASK			0x0f01fcff
#define	I_ERRMASK			0x0000fc00

/* interrupt receive lazy */
#define	IRL_TO_MASK			0x00ffffff
#define	IRL_FC_MASK			0xff000000
#define	IRL_FC_SHIFT		24

/* flow control thresholds */
#define	FCT_TT_MASK			0x00000fff
#define	FCT_RT_MASK			0x0fff0000
#define	FCT_RT_SHIFT		16

/* txq aribter wrr thresholds */
#define	WRRT_Q0T_MASK		0x000000ff
#define	WRRT_Q1T_MASK		0x0000ff00
#define	WRRT_Q1T_SHIFT		8
#define	WRRT_Q2T_MASK		0x00ff0000
#define	WRRT_Q2T_SHIFT		16
#define	WRRT_Q3T_MASK		0xff000000
#define	WRRT_Q3T_SHIFT		24

/* phy access */
#define	PA_DATA_MASK		0x0000ffff
#define	PA_ADDR_MASK		0x001f0000
#define	PA_ADDR_SHIFT		16
#define	PA_REG_MASK		0x1f000000
#define	PA_REG_SHIFT		24
#define	PA_WRITE		0x20000000
#define	PA_START		0x40000000

/* phy control */
#define	PC_EPA_MASK		0x0000001f
#define	PC_MCT_MASK		0x007f0000
#define	PC_MCT_SHIFT		16
#define	PC_MTE			0x00800000

/* rxq control */
#define	RC_DBT_MASK		0x00000fff
#define	RC_DBT_SHIFT		0
#define	RC_PTE			0x00001000
#define	RC_MDP_MASK		0x3f000000
#define	RC_MDP_SHIFT		24

#define RC_MAC_DATA_PERIOD	9

/* txq control */
#define	TC_DBT_MASK		0x00000fff
#define	TC_DBT_SHIFT		0

/* gpio select */
#define	GS_GSC_MASK		0x0000000f
#define	GS_GSC_SHIFT		0

/* gpio output enable */
#define	GS_GOE_MASK		0x0000ffff
#define	GS_GOE_SHIFT		0

/* clk control status */
#define CS_FA			0x00000001
#define CS_FH			0x00000002
#define CS_FI			0x00000004
#define CS_AQ			0x00000008
#define CS_HQ			0x00000010
#define CS_FC			0x00000020
#define CS_ER			0x00000100
#define CS_AA			0x00010000
#define CS_HA			0x00020000
#define CS_BA			0x00040000
#define CS_BH			0x00080000
#define CS_ES			0x01000000

/* command config */
#define	CC_TE			0x00000001
#define	CC_RE			0x00000002
#define	CC_ES_MASK		0x0000000c
#define	CC_ES_SHIFT		2
#define	CC_PROM			0x00000010
#define	CC_PAD_EN		0x00000020
#define	CC_CF			0x00000040
#define	CC_PF			0x00000080
#define	CC_RPI			0x00000100
#define	CC_TAI			0x00000200
#define	CC_HD			0x00000400
#define	CC_HD_SHIFT		10
#define	CC_SR			0x00002000
#define	CC_ML			0x00008000
#define	CC_AE			0x00400000
#define	CC_CFE			0x00800000
#define	CC_NLC			0x01000000
#define	CC_RL			0x02000000
#define	CC_RED			0x04000000
#define	CC_PE			0x08000000
#define	CC_TPI			0x10000000
#define	CC_AT			0x20000000

/* mac addr high */
#define	MH_HI_MASK		0xffff
#define	MH_HI_SHIFT		16
#define	MH_MID_MASK		0xffff
#define	MH_MID_SHIFT		0

/* mac addr low */
#define	ML_LO_MASK		0xffff
#define	ML_LO_SHIFT		0

/* Core specific control flags */
#define SICF_SWCLKE		0x0004
#define SICF_SWRST		0x0008

/* Core specific status flags */
#define SISF_SW_ATTACHED	0x0800

#endif /*__IPROC_GMAC_REGS_H */
