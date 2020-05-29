/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <asm/io.h>
#include <scsi.h>
#include <usb.h>

#include <usb/xhci-iproc.h>
#include <asm/arch/socregs.h>

#include <asm/arch/bcm_mdio.h>

#define IPROC_USB3_BASE_ADDR		 (0x66300000)

#define  XHCI_BASE               0x81000000
#define  XHCI_DCB_TABLE         (XHCI_BASE)
#define  DCB_SLOT0_SCATCHBU     (XHCI_BASE + 0x110000)
#define  DCB_SLOT1_DEVICE_0     (XHCI_BASE  + 0x120000)
#define  DCB_SLOT2_DEVICE_1     (XHCI_BASE  + 0x130000)
#define  XHCI_EVENT_SEGMENT     (XHCI_BASE  + 0x00C000)

#define  XHCI_SCRATCH_BUF0      (XHCI_BASE  + 0x180000)
#define  XHCI_SCRATCH_BUF1      (XHCI_BASE  + 0x1c0000)
#define  XHCI_INPUT_CONTEXT     (XHCI_BASE  + 0x100000)

#define  XHCI_COMMAND_RING      (XHCI_BASE +  0x200000)
#define  XHCI_EVENT_RING        (XHCI_BASE +  0x300000)
#define  XHCI_EP0_RING          (XHCI_BASE +  0x600000)
#define  XHCI_BULK_IN_RING      (XHCI_BASE +  0x800000)
#define  XHCI_BULK_OUT_RING     (XHCI_BASE +  0xa00000)

#define XHCI_CHAP9_BASE         (0x82000000)
#define  XHCI_CH9_DESC          (XHCI_CHAP9_BASE)
#define  XHCI_CH9_CONFIG        (XHCI_CHAP9_BASE + 0x000020)
#define  XHCI_CH9_POOL          (XHCI_CHAP9_BASE + 0x001000)

#define  XHCI_MSC_CMD_BUF       (XHCI_CHAP9_BASE + 0x004000)
#define  XHCI_MSC_STATUS_BUF    (XHCI_CHAP9_BASE + 0x008000)
#define  XHCI_MSC_REPLY_BUF     (XHCI_CHAP9_BASE + 0x00c000)

#define USB3_RUNTIME            (0x4A0)
#define USB3_DBOFF              (0x8c0)
#define XHCI_DB_CMD_RING        (USB3_DBOFF)
#define XHCI_DB_DEV_1           (USB3_DBOFF + 4)

#define USB3_SS0_BUSID  0x1
#define USB3_SS0_PHYID  0x0

#define USB3H_CMIC_MIIM_PARAM                   (0x6602023c)
#define USB3H_CMIC_MIIM_ADDRESS                 (0x66020244)
#define USB3H_CMIC_MIIM_STAT                    (0x6602024c)
#define USB3H_CMIC_MIIM_CTRL                    (0x66020248)
#define USB3H_CMIC_MIIM_READ_DATA               (0x66020240)

#define USB3H_VBUS_PPC_NANDNOR_SELECT		24

static uint32_t g_port_offset;
static uint32_t g_port_number;

/* Board work arround function.
 * This should take argument as USB3H instance */
static void usb3h_board_workaround(void)
{
	uint32_t reg_data;

	writel(0xA200, USB3H_M0_usb3h_IRAADR);

	reg_data = readl(USB3H_M0_usb3h_IRADAT);
	reg_data |= 0x4; /* bypass vbus inputs. AIUCTL0[2]=1 */
	reg_data |= 0x8; /* override vbus present. AIUCTL[3]=1 */
	reg_data &= 0xFFFFFFEF; /* override oca. AIUCTL[4]=0 */

	/* Indirect write */
	writel(0xA200, USB3H_M0_usb3h_IRAADR);
	writel(reg_data, USB3H_M0_usb3h_IRADAT);

	writel(0xA200, USB3H_M1_usb3h_IRAADR);
	reg_data = readl(USB3H_M1_usb3h_IRADAT);
	reg_data |= 0x4; /* bypass vbus inputs. AIUCTL0[2]=1 */
	reg_data |= 0x8; /* override vbus present. AIUCTL[3]=1 */
	reg_data &= 0xFFFFFFEF; /* override oca. AIUCTL[4]=0 */

	/* Indirect write */
	writel(0xA200, USB3H_M1_usb3h_IRAADR);
	writel(reg_data, USB3H_M1_usb3h_IRADAT);
}

/* USB3H Board polarity */
static void usb3h_board_polarity(void)
{
	uint32_t reg_data;

	reg_data = readl(USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data |=
		1 <<
		USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT__usb3_vbus_ppc_NANDNOR_select;
	reg_data &=
		~(1 <<
		  USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT__usb3_vbus_ppc_polarity);
	writel(reg_data, USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT);

	reg_data = readl(USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data |=
		1 <<
		USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT__usb3_vbus_ppc_NANDNOR_select;
	reg_data &=
		~(1 <<
		  USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT__usb3_vbus_ppc_polarity);
	writel(reg_data, USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT);
}

static int usb3h_mdio_write(uint32_t addr, uint32_t val)
{
	uint32_t reg_data;
	uint32_t count;

	reg_data = val & 0xFFFF; /* Phy register val is 16bits */
	reg_data |= 0x02400000;
	writel(reg_data, USB3H_CMIC_MIIM_PARAM);
	udelay(1000);

	addr = addr & 0xFF;

	writel(addr, USB3H_CMIC_MIIM_ADDRESS);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		reg_data = readl(USB3H_CMIC_MIIM_STAT);
		if (reg_data == 0)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x0\n",
		       reg_data);
		return -1;
	}

	writel(0x1, USB3H_CMIC_MIIM_CTRL);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		reg_data = readl(USB3H_CMIC_MIIM_STAT);
		if (reg_data == 1)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x1\n",
		       reg_data);
		return -1;
	}

	udelay(1000);

	writel(0x0, USB3H_CMIC_MIIM_CTRL);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		reg_data = readl(USB3H_CMIC_MIIM_STAT);
		if (reg_data == 0)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x0\n",
		       reg_data);
		return -1;
	}

	return 0;
}

#ifdef DEBUG
static int usb3h_mdio_read(uint32_t addr)
{
	uint32_t reg_data;
	uint32_t count;

	reg_data = 0x02400000;
	writel(reg_data, USB3H_CMIC_MIIM_PARAM);
	udelay(1000);

	addr &= 0xFF;

	writel(addr, USB3H_CMIC_MIIM_ADDRESS);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		reg_data = readl(USB3H_CMIC_MIIM_STAT);
		if (reg_data == 0)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x0\n",
		       reg_data);
		return -1;
	}

	writel(0x2, USB3H_CMIC_MIIM_CTRL);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		reg_data = readl(USB3H_CMIC_MIIM_STAT);
		if (reg_data == 1)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x1\n",
		       reg_data);
		return -1;
	}
	reg_data = readl(USB3H_CMIC_MIIM_READ_DATA);

	udelay(1000);

	writel(0x0, USB3H_CMIC_MIIM_CTRL);

	udelay(100);
	count = 20;
	do {
		udelay(1);
		if (readl(USB3H_CMIC_MIIM_STAT) == 0)
			break;
	} while (--count);

	if (!count) {
		printf("ERROR: USB3H_CMIC_MIIM_STAT read data(0x%08x) is not 0x0\n",
		       reg_data);
		return -1;
	}
	return reg_data;
}
#endif


void assert_phy_reset(void)
{
	uint32_t reg_data;

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
	reg_data &=
		~(1 <<
		  CRMU_USB3_CONTROL__CRMU_PERIPHERALS_USB3H_PHY_RESETB_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
}


void deassert_phy_reset(void)
{
	uint32_t reg_data;

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
	reg_data |=
		(1 <<
		 CRMU_USB3_CONTROL__CRMU_PERIPHERALS_USB3H_PHY_RESETB_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
}

void assert_phy_pll_reset(void)
{
	uint32_t reg_data;

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
	reg_data &=
		~(1 <<
		  CRMU_USB3_CONTROL__CRMU_PERIPHERALS_USB3H_PHY_PLL_RESETB_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
}

void deassert_phy_pll_reset(void)
{
	uint32_t reg_data;

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
	reg_data |=
		(1 <<
		 CRMU_USB3_CONTROL__CRMU_PERIPHERALS_USB3H_PHY_PLL_RESETB_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
}

void assert_phy_mdio_reset(void)
{
	uint32_t mdio_reg_data;

	mdio_reg_data = readl((DMU_COMMON_ROOT + CDRU_MISC_RESET_CONTROL));
	mdio_reg_data &=
		~(1 <<
		  CDRU_MISC_RESET_CONTROL__CDRU_PERIPHERALS_USB3H_MDIO_RESET_N);
	writel(mdio_reg_data, (DMU_COMMON_ROOT + CDRU_MISC_RESET_CONTROL));
}

void deassert_phy_mdio_reset(void)
{
	uint32_t mdio_reg_data;

	mdio_reg_data = readl((DMU_COMMON_ROOT + CDRU_MISC_RESET_CONTROL));
	mdio_reg_data |=
		(1 <<
		 CDRU_MISC_RESET_CONTROL__CDRU_PERIPHERALS_USB3H_MDIO_RESET_N);
	writel(mdio_reg_data, (DMU_COMMON_ROOT + CDRU_MISC_RESET_CONTROL));
}

void assert_phy_soft_reset(void)
{
	uint32_t reg_data;

	reg_data = readl(ICFG_USB3H_PHY_P0CTL_I);
	reg_data &= ~(0x2);
	writel(reg_data, ICFG_USB3H_PHY_P0CTL_I);

	reg_data = readl(ICFG_USB3H_PHY_P1CTL_I);
	reg_data &= ~(0x2);
	writel(reg_data, ICFG_USB3H_PHY_P1CTL_I);
}

void deassert_phy_soft_reset(void)
{
	uint32_t reg_data;

	reg_data = readl(ICFG_USB3H_PHY_P0CTL_I);
	reg_data |= 0x2;
	writel(reg_data, ICFG_USB3H_PHY_P0CTL_I);

	reg_data = readl(ICFG_USB3H_PHY_P1CTL_I);
	reg_data |= 0x2;
	writel(reg_data, ICFG_USB3H_PHY_P1CTL_I);
}

void assert_phy_pipe_reset(void)
{
	uint32_t reg_data;

	/*reg_data = readl(USB3H_M0_IDM_IDM_RESET_CONTROL);*/
	reg_data = 0x1;
	writel(reg_data, USB3H_M0_IDM_IDM_RESET_CONTROL);

	/*reg_data = readl(USB3H_M1_IDM_IDM_RESET_CONTROL);*/
	reg_data = 0x1;
	writel(reg_data, USB3H_M1_IDM_IDM_RESET_CONTROL);
}

void deassert_phy_pipe_reset(void)
{
	uint32_t reg_data;

	/*reg_data = readl(USB3H_M0_IDM_IDM_RESET_CONTROL);*/
	reg_data = 0x0;
	writel(reg_data, USB3H_M0_IDM_IDM_RESET_CONTROL);

	/*reg_data = readl(USB3H_M1_IDM_IDM_RESET_CONTROL);*/
	reg_data = 0x0;
	writel(reg_data, USB3H_M1_IDM_IDM_RESET_CONTROL);
}

void program_phy_ref_clock(void)
{
	uint32_t reg_data;

	reg_data = readl(ICFG_USB3H_PHY_CONFIG_CTRL);
	reg_data &= 0xFFFFFFC7; /* Make sure bit 3, 4 and 5 are 0 */
	writel(reg_data, ICFG_USB3H_PHY_CONFIG_CTRL);
}

void assert_phy_pll_seq_start(void)
{
	uint32_t reg_data;

	reg_data = readl(ICFG_USB3H_PHY_CONFIG_CTRL);
	reg_data |= 0x40;
	writel(reg_data, ICFG_USB3H_PHY_CONFIG_CTRL);
}

void xhci_phy_init(void)
{
	uint32_t reg_data;
	uint32_t count;

	deassert_phy_reset();
	deassert_phy_mdio_reset();
	deassert_phy_pll_reset();
	assert_phy_soft_reset();
	assert_phy_pipe_reset();

	/* Use CML ref clock. */
	program_phy_ref_clock();

	/* Assert pll, mdio and phy resets. */
	assert_phy_pll_reset();
	assert_phy_mdio_reset();
	assert_phy_reset();

	/* Deassert phy, mdio resets. */
	deassert_phy_reset();
	deassert_phy_mdio_reset();

	/* PLL programming. */
	usb3h_mdio_write(0x1F, 0x8000);

	#ifdef DEBUG
	printf("\nmdio_read_usb30(0x14) = 0x%08x\n", usb3h_mdio_read(0x14));
	#endif

	usb3h_mdio_write(0x14, 0x0023);

#ifdef DEBUG
	printf("After writing 0x0023 mdio_read_usb30(0x14) = 0x%08x\n",
	       usb3h_mdio_read(0x14));
#endif

	/* Program PLL Start Sequence and deassert pipe reset.*/
	assert_phy_pll_seq_start();
	deassert_phy_pipe_reset();
	udelay(10000);

	/* Deassert soft reset. */
	deassert_phy_soft_reset();

	/* Deassert pll reset. */
	deassert_phy_pll_reset();
	mdelay(1000);

#ifdef DEBUG
	printf("Waiting for PLL Lock\n");
#endif
	count = 100;
	/* Wait for PLL lock. */
	do {
		udelay(1);
		if (readl(ICFG_USB3H_PHY_MISC_STATUS_REG) == 1)
			break;
	} while (--count);
	if (!count) {
		printf("ERROR: PLL Lock Failed\n");
		return;
	}

#ifdef DEBUG
	printf("USB3H_M0_CAPLENGTH = 0x%x\n", readl(USB3H_M0_CAPLENGTH));
#endif
	/* Check for Controller Not Ready (CNR) */
#if 1
	do {
		udelay(1);
		reg_data = readl(USB3H_M0_USBSTS);
	} while (reg_data & (1 << USB3H_M0_USBSTS__CNR));
#endif

	usb3h_board_workaround();
	usb3h_board_polarity();
}

/* TODO Can optmize these Delays */
void xhci_core_phy_in_reset(void)
{
	uint32_t reg_data;

	writel(0xA200, USB3H_M0_usb3h_IRAADR);
	udelay(10);
	writel(0x0, USB3H_M0_usb3h_IRADAT);
	udelay(10);
	writel(0xA200, USB3H_M1_usb3h_IRAADR);
	udelay(10);
	writel(0x0, USB3H_M1_usb3h_IRADAT);
	udelay(10);

	reg_data = readl(USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data &= ~(1 << USB3H_VBUS_PPC_NANDNOR_SELECT);
	writel(reg_data, USB3H_M0_IDM_IDM_IO_CONTROL_DIRECT);
	udelay(2);

	reg_data = readl(USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data &= ~(1 << USB3H_VBUS_PPC_NANDNOR_SELECT);
	writel(reg_data, USB3H_M1_IDM_IDM_IO_CONTROL_DIRECT);
	udelay(2);

	writel(0x0, ICFG_USB3H_PHY_CONFIG_CTRL);
	udelay(10);
	assert_phy_soft_reset();
	udelay(100);
	writel(0x0, (DMU_COMMON_ROOT + CRMU_USB3_CONTROL));
	udelay(100);
	assert_phy_pipe_reset();
	udelay(100);
}

struct xhci_event_cmd_trb   *p_event_trb, *p_event_root;
struct xhci_event_cmd_trb   *p_cmd_trb, *p_cmd_root;

struct xhci_slot_ctx           *p_input_slot_ctx;
struct xhci_ep_ctx             *p_input_ep0_ctx;
struct xhci_ep_ctx             *p_input_ep_bulk_in_ctx;
struct xhci_ep_ctx             *p_input_ep_bulk_out_ctx;
struct xhci_input_control_ctx  *p_input_inputctrl_ctx;

struct xhci_slot_ctx        *p_output_slot_ctx;
struct xhci_ep_ctx          *p_output_ep0_ctx;
struct xhci_ep_ctx          *p_output_ep_bulk_in_ctx;

struct xhci_transfer_event  *p_ep0_ring;
struct xhci_transfer_event  *p_ep_in_ring;
struct xhci_transfer_event  *p_ep_out_ring;

umass_bbb_cbw_t    *p_cbw_cmd;
umass_bbb_csw_t    *p_csw_status;
char               *p_cbw_data;

static u32 event_ring_dequeue_ptr;
static u32 cbw_tag;
static u32 link_state;
static u32 ep_halt_state;
static u32 ep_in_num, ep_out_num, DCI_ep_in, DCI_ep_out;

/* 64K entries of TRB, * (16 bytes per TRB), = 1MByte, 0x10_0000 */
#define  MAX_EVENT_RING_SIZE    64*1024*16
#define  MAX_CMD_RING_SIZE      64*1024*16

/* WAIT_TIME_OUT, 5s */
#define  EVT_TIME_OUT           (500*1000)

/* #define debug printf */

static inline void xhci_writel(u32 addr, unsigned int val)
{
	debug("Write [0x%08x] = 0x%08x\n", (u32)addr, val);
	*(uint32_t *)(u64)
		(IPROC_USB3_BASE_ADDR + addr + g_port_offset) =
		(uint32_t)val;
}

static inline u32 xhci_readl(u32 addr)
{
	uint32_t val;

	val = *(uint32_t *)(u64)
		(IPROC_USB3_BASE_ADDR + addr + g_port_offset);
	return (u32)val;
}

void xhci_hcd_reset(void)
{
	u32 tmp;
	debug("\nHCD Hard Reset\n");
	xhci_writel(USB3_USBCMD, CMD_RESET);
	do {
		udelay(1);
		tmp = xhci_readl(USB3_USBCMD);
	} while (tmp & CMD_RESET);
}

void xhci_port_warm_reset(void)
{
	u32 tmp;
	debug("\nPort Warm Reset\n");

	tmp = xhci_readl(USB3_PORTSC);
	/* clear reset status bits first */
	tmp |= (1<<19) | (1<<21);
	xhci_writel(USB3_PORTSC, tmp);

	tmp = xhci_readl(USB3_PORTSC);
	xhci_writel(USB3_PORTSC, tmp | PORT_WR);
	xhci_writel(USB3_PORTSC, tmp | PORT_RESET | PORT_WR);

	/* Wait Warm Reset Complete, */
	/* bit19 warm completed, */
	/* bit21 reset completed */
	do {
		udelay(10);
		tmp = xhci_readl(USB3_PORTSC);
	} while ((tmp & (1<<19)) && (tmp & (1<<21)));

	/* clear reset status bits */
	tmp |= (1<<19) | (1<<21);
	xhci_writel(USB3_PORTSC, tmp);
	debug("\nPort Warm Reset Done\n");
}

/****************************************************
    USB Init, memory init, data structure build
*****************************************************/
int xhci_hcd_init(uint32_t port_num)
{
	int usb3_cnr;

	g_port_number = port_num;
	if (port_num)
		g_port_offset = 0x10000;
	else
		g_port_offset = 0;

	xhci_phy_init();

	/* Wait CNR */
	debug("\n xhci_hcd_init: Polling CNR\n");
	do {
		usb3_cnr = xhci_readl(USB3_USBSTS);
	} while (usb3_cnr & STS_CNR);

	/* HCD Hard Reset */
	xhci_hcd_reset();

	/* Wait CNR again */
	debug("\n xhci_hcd_init: Polling CNR\n");
	do {
		usb3_cnr = xhci_readl(USB3_USBSTS);
	} while (usb3_cnr & STS_CNR);

	debug("\n malloc critical structures\n");
	xhci_hcd_mem_init();

	xhci_writel(USB3_CONFIG, MAX_DEVS(0x1));

	/* DCB context */
	/* Add Scatch Buffer to Scatch Buffer Array */
	writel(XHCI_SCRATCH_BUF0,
	       DCB_SLOT0_SCATCHBU); /* Scatch Buffer Array item 0 */
	writel(XHCI_SCRATCH_BUF1,
	       DCB_SLOT0_SCATCHBU + 8); /* Scatch Buffer Array item 1 */


	/* Populate DCB Array */
	writel(DCB_SLOT0_SCATCHBU,
	       XHCI_DCB_TABLE); /* Item 0, reserved to Scatch buffer array */
	writel(DCB_SLOT1_DEVICE_0,
	       XHCI_DCB_TABLE + 0x8); /* Item 1, first Slot/device */
	writel(DCB_SLOT2_DEVICE_1, XHCI_DCB_TABLE + 0x10);

	xhci_writel(USB3_DCBAAPL, XHCI_DCB_TABLE);
	xhci_writel(USB3_DCBAAPH, 0);

	/* Command Ring */
	xhci_writel(USB3_CRCRL,
		    XHCI_COMMAND_RING | 0x1); /* RCS, Ring Cycle State = 1 */
	memset((void *)XHCI_COMMAND_RING, 0, 0x200000);
	xhci_writel(USB3_CRCRH, 0);

	/* Event Ring Seg Table Size = 1 */
	xhci_writel(USB3_RUNTIME + 0x28 + 0, 1);

	/* Populate Segment item, only 1 event segment, 2K event TRB entries */
	/* Event_Segment_table(0) = XHCI_EVENT_RING (first TRB entry) */
	writel(XHCI_EVENT_RING,  XHCI_EVENT_SEGMENT); /* Address, u64 */
	writel(2048,             XHCI_EVENT_SEGMENT + 8); /* Size, u32 */

	/* Event Ring Dequeue PTR */
	xhci_writel(USB3_RUNTIME + 0x38 + 0, XHCI_EVENT_RING);
	xhci_writel(USB3_RUNTIME + 0x3C + 0, 0);

	/* Event Segment Table, ESTBA */
	xhci_writel(USB3_RUNTIME + 0x30 + 0, XHCI_EVENT_SEGMENT);
	xhci_writel(USB3_RUNTIME + 0x34 + 0, 0);

	/* Cmd Run */
	xhci_writel(USB3_USBCMD, CMD_RUN);

	link_state = 0;
	return 0;
}

int xhci_hcd_mem_init(void)
{
	memset((void *)XHCI_BASE, 0, 0x2000000);
	printf("memset form XHCI_BASE(0x%08x) to 0x2000000\n", XHCI_BASE);

	/* Build all rings */
	p_event_trb   = (struct xhci_event_cmd_trb *)(XHCI_EVENT_RING);
	p_cmd_trb     = (struct xhci_event_cmd_trb *)(XHCI_COMMAND_RING);
	p_event_root  = p_event_trb;
	p_cmd_root    = p_cmd_trb;
	event_ring_dequeue_ptr = XHCI_EVENT_RING;

	p_ep0_ring       = (struct xhci_transfer_event *)(XHCI_EP0_RING);
	p_ep_in_ring     = (struct xhci_transfer_event *)(XHCI_BULK_IN_RING);
	p_ep_out_ring    = (struct xhci_transfer_event *)(XHCI_BULK_OUT_RING);

	/* Create Input DCB context */
	p_input_inputctrl_ctx = (struct xhci_input_control_ctx *)
		(XHCI_INPUT_CONTEXT);
	p_input_slot_ctx = (struct xhci_slot_ctx *)
		(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE); /* 16|32 Dwords of CTX */
	p_input_ep0_ctx = (struct xhci_ep_ctx *)
		(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * 2);

	xhci_create_input_ctx();

	/* Create Output DCB context */
	/* Since Slot_0 context init as 0, the dev_state = 0 (disabled) */
	p_output_slot_ctx       = (struct xhci_slot_ctx *)(DCB_SLOT1_DEVICE_0);
	p_output_ep0_ctx         = (struct xhci_ep_ctx *)
		(DCB_SLOT1_DEVICE_0 + DCB_ENTRY_SZIE);

	/* USB MSC CMD buffers */
	p_cbw_cmd    = (umass_bbb_cbw_t *)(XHCI_MSC_CMD_BUF);
	p_csw_status = (umass_bbb_csw_t *)(XHCI_MSC_STATUS_BUF);
	p_cbw_data   = (char *)(XHCI_MSC_REPLY_BUF);

	return 0;
}

void xhci_create_input_ctx(void)
{
	u32 temp;

	p_input_inputctrl_ctx->add_flags = 0x3; /* Add A0 and A1 */

	temp = SLOT_SPEED_SS;
	temp |= 0; /* ROUTE_STRING_MASK, Find out route string */
	temp |= LAST_CTX(1); /* Add EP0 for now */

	p_input_slot_ctx->dev_info     = temp;
	p_input_slot_ctx->dev_info2    = ROOT_HUB_PORT(1);
	p_input_slot_ctx->tt_info      = 0;
	p_input_slot_ctx->dev_state    = 0;

	/* EP0 Context */
	p_input_ep0_ctx->ep_info    = 0;
	p_input_ep0_ctx->ep_info2   = EP_TYPE(CTRL_EP) |
		MAX_BURST(0) | MAX_PACKET(512) | ERROR_COUNT(3);
	p_input_ep0_ctx->deq_l      = XHCI_EP0_RING | 1;
	p_input_ep0_ctx->deq_h      = 0;
	p_input_ep0_ctx->tx_info    = 0;
}

void xhci_update_input_ctx(void)
{
	u32 DCI_last_ctx;

	/* find out which bulk endpoint from Get_Config */
	/* Item 0, input_ctrl; Item 1, Slot_ctx; Itme 2, EP0_ctx */
	/* Item 3, EP1_OUT; Item 4, EP1_IN; Item 5, EP2_OUT */
	/* Get_Config, EP-1, IN, item 4; EP-2, OUT, item 5 */

	DCI_ep_in   = (ep_in_num * 2) + 1; /* IN plus 1 */
	DCI_ep_out  = (ep_out_num * 2); /* OUT plus 0 */
	debug("\n Update DCI index, ep_in=%d, ep_out=%d, ",
	      DCI_ep_in, DCI_ep_out);

	/* ICI = DCI + 1 */
	p_input_ep_bulk_in_ctx = (struct xhci_ep_ctx *)(u64)
		(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * (DCI_ep_in + 1));
	p_input_ep_bulk_out_ctx = (struct xhci_ep_ctx *)(u64)
		(XHCI_INPUT_CONTEXT + DCB_ENTRY_SZIE * (DCI_ep_out + 1));

	p_input_ep_bulk_in_ctx->ep_info    = 0;
	p_input_ep_bulk_in_ctx->ep_info2   = EP_TYPE(BULK_IN_EP) |
		MAX_BURST(0) | MAX_PACKET(1024) | ERROR_COUNT(3);
	p_input_ep_bulk_in_ctx->deq_l      = XHCI_BULK_IN_RING | 1;
	p_input_ep_bulk_in_ctx->deq_h      = 0;
	p_input_ep_bulk_in_ctx->tx_info    = 0;

	p_input_ep_bulk_out_ctx->ep_info    = 0;
	p_input_ep_bulk_out_ctx->ep_info2   = EP_TYPE(BULK_OUT_EP) |
		MAX_BURST(0) | MAX_PACKET(1024) | ERROR_COUNT(3);
	p_input_ep_bulk_out_ctx->deq_l      = XHCI_BULK_OUT_RING | 1;
	p_input_ep_bulk_out_ctx->deq_h      = 0;
	p_input_ep_bulk_out_ctx->tx_info    = 0;

	/* Change Slot_ctx entries */
	if (DCI_ep_in > DCI_ep_out)
		DCI_last_ctx = DCI_ep_in;
	else
		DCI_last_ctx = DCI_ep_out;

	p_input_slot_ctx->dev_info   &= ~LAST_CTX_MASK;
	p_input_slot_ctx->dev_info   |= LAST_CTX(DCI_last_ctx);

	/* Add Bulk EP into Context, Set A0, Ingore A1 */
	/* Add A3 (EP1-IN) and A4 (EP2-OUT) */
	p_input_inputctrl_ctx->add_flags = (1<<0) |
		(1<<DCI_ep_in) | (1<<DCI_ep_out);

	/* Get hold of Bulk_in Context, DCB index = 3 */
	p_output_ep_bulk_in_ctx = (struct xhci_ep_ctx *)(u64)
		(DCB_SLOT1_DEVICE_0 + DCB_ENTRY_SZIE * DCI_ep_in);
}
void xhci_ring_doorbell(u32 ep_num, u32 direction)
{
	if (ep_num == 0)
		xhci_writel(XHCI_DB_DEV_1, 0x1);
	else
		xhci_writel(XHCI_DB_DEV_1, (ep_num*2)+direction);
}

/***************************************
    USB Mass Storage functions,
    Work on Bulk In and Out EP.
****************************************/
void xhci_queue_bulk_tx(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
	/* Bulk-out data */
	p_ep_out_ring->buffer_l     = buf_ptr;
	p_ep_out_ring->buffer_h     = 0;
	p_ep_out_ring->transfer_len = xfer_len;
	p_ep_out_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | 0x1;
	p_ep_out_ring++;
}

void xhci_queue_bulk_data_write(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
	u32 td_count;

	td_count = (xfer_len+1023)/1024;
	if (td_count == 0)
		td_count = 1;

	debug("\nWrite_10 : xfer_len=%d, td_cnt=%d, ", xfer_len, td_count);
	/* Bulk-out data */
	p_ep_out_ring->buffer_l     = buf_ptr;
	p_ep_out_ring->buffer_h     = 0;
	p_ep_out_ring->transfer_len = xfer_len | (td_count<<17);
	p_ep_out_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit |
		(1<<2) | 0x1;
	p_ep_out_ring++;
}

void xhci_queue_bulk_rx(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
	/* Bulk-in data */
	p_ep_in_ring->buffer_l     = buf_ptr;
	p_ep_in_ring->buffer_h     = 0;
	p_ep_in_ring->transfer_len = xfer_len;
	p_ep_in_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit | 0x1;
	p_ep_in_ring++;
}

void xhci_queue_bulk_data_read(u32 buf_ptr, u32 xfer_len, u32 IOC_bit)
{
	u32 td_count;

	td_count = (xfer_len+1023)/1024;
	debug("\nRead_10 : xfer_len=%d, td_cnt=%d, ", xfer_len, td_count);
	/* Bulk-in data */
	p_ep_in_ring->buffer_l     = buf_ptr;
	p_ep_in_ring->buffer_h     = 0;
	p_ep_in_ring->transfer_len = xfer_len | (td_count<<17);
	/* ISP short packet is on */
	p_ep_in_ring->flags        = TRB_TYPE(TRB_NORMAL) | IOC_bit |
		(1<<2) | 0x1;
	p_ep_in_ring++;
}

void xhci_msc_inquiry_cmd(void)
{
	/* INQUIRY */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = 36; /* 36 byte vendor ID etc. */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_IN;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN =0, get from GET_LUN */
	p_cbw_cmd->bCDBLength            = 6; /* Inquiry cmd_len = 6 */

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_INQUIRY;
	p_cbw_cmd->CBWCDB[4]  = 36;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_read_capacity(void)
{
	/* READ_CAPACITY */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = 8; /* 8 byte capacity info */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_IN;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 10; /* ReadCap cmd_len = 10 */

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_RD_CAPAC;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_mode_sense(void)
{
	/* MODE_SENSE */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = 0xC0; /* 8 byte capacity info */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_IN;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 6;

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_MODE_SEN6;
	p_cbw_cmd->CBWCDB[2]  = 0x3f; /* Page Code */
	p_cbw_cmd->CBWCDB[4]  = 0xC0;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_request_sense(void)
{
	/* MODE_SENSE */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = 0x60;
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_IN;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 6;

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_REQ_SENSE;
	p_cbw_cmd->CBWCDB[4]  = 0x60;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_test_unit(void)
{
	/* TEST_UNIT_READY */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = 0; /* No return */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_OUT;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 6; /* cmd_len = 6 */

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_TST_U_RDY;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_read_10(u32 xfer_len, u32 lba, u32 block_len)
{
	/* READ_10 */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = xfer_len; /* Total bytes read */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_IN;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 10; /* cmd_len = 10 */

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_READ10;
	p_cbw_cmd->CBWCDB[5]  = lba & 0xff; /* Disk logical blkAddr, l-endian */
	p_cbw_cmd->CBWCDB[4]  = (lba>>8) & 0xff;
	p_cbw_cmd->CBWCDB[3]  = (lba>>16) & 0xff;
	p_cbw_cmd->CBWCDB[2]  = (lba>>24) & 0xff;
	p_cbw_cmd->CBWCDB[8]  = block_len & 0xff; /* Disk blk len to transfer */
	p_cbw_cmd->CBWCDB[7]  = (block_len>>8) & 0xff;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

void xhci_msc_write_10(u32 xfer_len, u32 lba, u32 block_len)
{
	/* READ_10 */
	p_cbw_cmd->dCBWSignature         = CBWSIGNATURE;
	p_cbw_cmd->dCBWTag               = cbw_tag++;
	p_cbw_cmd->dCBWDataTransferLength    = xfer_len; /* Total bytes read */
	p_cbw_cmd->bCBWFlags             = CBWFLAGS_OUT;
	p_cbw_cmd->bCBWLUN               = 0; /* WDC LUN = 0 */
	p_cbw_cmd->bCDBLength            = 10; /* cmd_len = 10 */

	memset((u8 *)p_cbw_cmd->CBWCDB, 0, CBWCDBLENGTH);
	p_cbw_cmd->CBWCDB[0]  = SCSI_WRITE10;
	p_cbw_cmd->CBWCDB[5]  = lba & 0xff; /* Disk logical block Addr */
	p_cbw_cmd->CBWCDB[4]  = (lba>>8) & 0xff;
	p_cbw_cmd->CBWCDB[3]  = (lba>>16) & 0xff;
	p_cbw_cmd->CBWCDB[2]  = (lba>>24) & 0xff;
	p_cbw_cmd->CBWCDB[8]  = block_len & 0xff; /* Disk blk len to transfer */
	p_cbw_cmd->CBWCDB[7]  = (block_len>>8) & 0xff;

	xhci_queue_bulk_tx((u32)(u64)p_cbw_cmd, UMASS_BBB_CBW_SIZE, 0);

	/* Move to next Command */
	p_cbw_cmd++;
}

/***************************************
    USB CH9 Enumeration functions,
    Work on Control EP0.
****************************************/
void xhci_queue_ctrl_setup(u32 field_1, u32 field_2, u32 trt)
{
	/* Control Setup Stage, always 8 bytes Immediate data */
	p_ep0_ring->buffer_l     = field_1;
	p_ep0_ring->buffer_h     = field_2;
	p_ep0_ring->transfer_len = 8;
	p_ep0_ring->flags        = TRB_TYPE(TRB_SETUP) | TRB_IDT |
		TRB_TX_TYPE(trt) | 0x1; /* Cycle bit */
	p_ep0_ring++;
}

void xhci_queue_ctrl_data(u32 buf_ptr, u32 xfer_len, u32 direction)
{
	/* Contrl Data Stage, DIR=IN or OUT */
	p_ep0_ring->buffer_l     = buf_ptr;
	p_ep0_ring->buffer_h     = 0;
	p_ep0_ring->transfer_len = xfer_len;
	p_ep0_ring->flags        = TRB_TYPE(TRB_DATA) | direction | 0x1;
	p_ep0_ring++;
}

void xhci_queue_ctrl_status(u32 direction)
{
	/* Contrl Data Stage, DIR=IN or OUT, IOC - trigger an event */
	p_ep0_ring->buffer_l     = 0;
	p_ep0_ring->buffer_h     = 0;
	p_ep0_ring->transfer_len = 0;
	p_ep0_ring->flags        = TRB_TYPE(TRB_STATUS) | direction |
		TRB_IOC | 0x1;
	p_ep0_ring++;
}

void xhci_ch9_clear_feature(void)
{
	u32 f1, f2;

	/* 2 stage */
	f1 = 0x0102; f2 = 0x80 | ep_in_num;
	xhci_queue_ctrl_setup(f1, f2, 0);
	xhci_queue_ctrl_status(TRB_DIR_IN);
}

void xhci_ch9_get_descriptor(void)
{
	u32 f1, f2, data_buf;

	/* Get_Descriptor, len = 18 bytes */
	f1 = 0x1000680; f2 = 0x120000;
	xhci_queue_ctrl_setup(f1, f2, TRB_DATA_IN);

	data_buf = XHCI_CH9_DESC;
	xhci_queue_ctrl_data(data_buf, 18, TRB_DIR_IN);

	xhci_queue_ctrl_status(TRB_DIR_OUT);
}

void xhci_ch9_get_configuration(u32 len)
{
	u32 f1, f2, data_buf;

	/* Get_Descriptor, len = 9 or xx bytes */
	f1 = 0x2000680; f2 = (len&0xff)<<16;
	xhci_queue_ctrl_setup(f1, f2, TRB_DATA_IN);

	data_buf = XHCI_CH9_CONFIG;
	xhci_queue_ctrl_data(data_buf, len, TRB_DIR_IN);

	xhci_queue_ctrl_status(TRB_DIR_OUT);
}

void xhci_ch9_set_configuration(void)
{
	u32 f1, f2;

	/* Per Spec 4.3.5, Only Need Setup stage is needed, Config_id=1  */
	f1 = 0x00010900; f2 = 0;
	xhci_queue_ctrl_setup(f1, f2, 0);

	xhci_queue_ctrl_status(TRB_DIR_IN);
}

void xhci_ch9_get_lun(void)
{
	u32 f1, f2, data_buf;

	/* Get_LUN, len = 1 */
	f1 = 0xfea1; f2 = (0x1)<<16;
	xhci_queue_ctrl_setup(f1, f2, TRB_DATA_IN);

	data_buf = XHCI_CH9_POOL;
	xhci_queue_ctrl_data(data_buf, 1, TRB_DIR_IN);

	xhci_queue_ctrl_status(TRB_DIR_OUT);
}


void xhci_queue_cmd(void)
{
	/* Toggle Cycle bit */
	p_cmd_trb->flags |= 1;

	/* Ring Command Queue DoorBell, = 0 */
	xhci_writel(XHCI_DB_CMD_RING, 0);

	/* Move the pointer of Cmd_Ring, and wrap around */
	p_cmd_trb++;
	if (p_cmd_trb >= p_cmd_root + MAX_CMD_RING_SIZE)
		p_cmd_trb = p_cmd_root;
}

void xhci_queue_enable_slot(void)
{
	/* Create Address Device Cmd */
	p_cmd_trb->cmd_trb_l = 0;
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	p_cmd_trb->flags = TRB_TYPE(TRB_ENABLE_SLOT);
	xhci_queue_cmd();
}

void xhci_queue_address_device(u32 slot_id)
{
	/* Create Address Device Cmd */
	p_cmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	p_cmd_trb->flags = TRB_TYPE(TRB_ADDR_DEV) | (slot_id<<24);
	xhci_queue_cmd();
}

void xhci_queue_evaluate_context(u32 slot_id)
{
	p_cmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	p_cmd_trb->flags = TRB_TYPE(TRB_EVAL_CONTEXT) | (slot_id<<24);
	xhci_queue_cmd();
}

void xhci_queue_configure_endpoint(u32 slot_id)
{
	p_cmd_trb->cmd_trb_l = (XHCI_INPUT_CONTEXT);
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	p_cmd_trb->flags = TRB_TYPE(TRB_CONFIG_EP) | (slot_id<<24);
	xhci_queue_cmd();
}

void xhci_queue_reset_endpoint(u32 slot_id, u32 ep)
{
	p_cmd_trb->cmd_trb_l = 0;
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	/* bit9 TSP =0, No perserve ep state */
	p_cmd_trb->flags = TRB_TYPE(TRB_RESET_EP) | (slot_id<<24) |
		(ep<<16) | (0<<9);
	xhci_queue_cmd();
}

void xhci_queue_tr_dequeue(u32 slot_id, u32 ep, u32 address)
{
	p_cmd_trb->cmd_trb_l = address | (1<<0);
	p_cmd_trb->cmd_trb_h = 0;
	p_cmd_trb->status = 0;
	/* bit9 TSP =0, No perserve ep state */
	p_cmd_trb->flags = TRB_TYPE(TRB_SET_DEQ) | (slot_id<<24) | (ep<<16);
	xhci_queue_cmd();
}

/**************************************
    USB Interrupt processing.
    Event coming from interrupt
****************************************/
void xhci_check_slot_state(void)
{
	/* Add a timeout later!! */
	while (1) {
		/* break when Slot is addressed */
		if (GET_SLOT_STATE(p_output_slot_ctx->dev_state) == 0x2) {
			printf("\nUSB Slot Addressed\n");
			return;
		}
		udelay(1);
	}
}

int xhci_wait_event_queue(u32 trb_type)
{
	u32 event, event_ring_dequeue_ptr, tmp;
	u32 slot_state, ep_state, i;

	while (1) {
		/* Polling event queue, C bit */
		for (i = EVT_TIME_OUT; i > 0; i--) {
			if (p_event_trb->flags & 0x1)
				break;
			udelay(10);
		}

	if (i == 0) {
		printf("\nEvent Queue time-out\n");
		return -1;
	}

	/* Get an event in the queue */
	event = TRB_FIELD_TO_TYPE(p_event_trb->flags);
	debug("\nRecv Event #%d ", event);

	if (event == TRB_COMPLETION) {
		debug("\nCompleted cmd_trb: prt=%x, status=%d, slot_id=%d, ",
		      p_event_trb->cmd_trb_l,
		      GET_COMP_CODE(p_event_trb->status),
		      TRB_TO_SLOT_ID(p_event_trb->flags));
	} else if (event == TRB_PORT_STATUS) {
		debug("\nPORT_STATUS_Change: port_id=%d, ",
		      GET_PORT_ID(p_event_trb->cmd_trb_l));
	} else if (event == TRB_TRANSFER) {
		debug("\nXfer_Done, EP-ID %d: EP-state=%d, ",
		      TRB_TO_EP_INDEX(p_event_trb->flags),
		      (p_output_ep0_ctx->ep_info)&EP_STATE_MASK);
		debug("trb_ptr=%x, xfer_status=%d, xfer_len=%d, ",
		      p_event_trb->cmd_trb_l,
		      GET_COMP_CODE(p_event_trb->status),
		      p_event_trb->status & 0xFFFFFF);
	} else {
		debug("\nNot Processed Event, Check this!!!!!!!!!!!!!\n");
	}

	/* Monitor Port Status */
	tmp = xhci_readl(USB3_PORTSC);
	debug("Port_SC: Port_state=%d, PP/Power=%d, CCS/Cont=%d, ",
	      (tmp>>5)&0xf, (tmp>>9)&0x1, tmp&0x1);
	debug("PED/Enabled=%d, PR/Reset=%d, CAS=%d, ",
	      (tmp>>1)&0x1, (tmp>>4)&0x1, (tmp>>24)&0x1);

	/* After Port connected, report slot_state */
	if (link_state) {
		slot_state = GET_SLOT_STATE(p_output_slot_ctx->dev_state);
		debug("\nSlot_state=%d, ", slot_state);

		/* Configured Slot */
		if (slot_state == 3) {
			ep_state = p_output_ep_bulk_in_ctx->ep_info &
				EP_STATE_MASK;
			debug("\nEP-IN<%d>_state=%d, ", ep_in_num, ep_state);

			/* Halted EP need clear */
			if (ep_state == EP_STATE_HALTED) {
				ep_halt_state = 1;
				debug("\nEP Halted, Take care.....\n");
			}
		}
	}

	/* Move Event index, move de-queue pointer */
	p_event_trb++;
	event_ring_dequeue_ptr = (u32)(u64)p_event_trb;
	xhci_writel(USB3_RUNTIME + 0x38 + 0, event_ring_dequeue_ptr);

	/* Check trb_type, if match, return; no match, discard, continue. */
	if (event == trb_type)
		return 0;
	}
}


void xhci_reset_halt(void)
{
	u32 ret;

	/* Reset halt endpoint, slot_id=1, DCI=3 */
	debug("\n*** Reset Halted Endpoint : ");
	xhci_queue_reset_endpoint(1, DCI_ep_in);
	xhci_wait_event_queue(TRB_COMPLETION);

	/* Clear Halt */
	debug("\n*** Send Clear_feature : ");
	xhci_ch9_clear_feature();
	xhci_ring_doorbell(EP_0, 0);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(100);

	/* Set TR_DEqueue */
	ret = p_output_ep_bulk_in_ctx->deq_l & 0xfffffff0;
	debug("\n*** Set TR_Dequeue Ptr 0x%x : ", ret);
	xhci_queue_tr_dequeue(1, DCI_ep_in, ret);
	xhci_wait_event_queue(TRB_COMPLETION);

	/* Retry last xfer */
	debug("\n*** Retry last transfer : ");
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(100);
}


void xhci_queue_msc_test_unit(void)
{
	/* MSC_TEST_UNIT_READY */
	debug("\n\n*** MSC_Test_Unit_Ready : ");
	xhci_msc_test_unit();

	p_csw_status += 0x20;
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(1000);
}

void xhci_queue_msc_read_capacity(void)
{
	/* Read Capacity */
	debug("\n\n*** MSC_Read_Capacity : ");
	xhci_msc_read_capacity();

	p_cbw_data   += 0x40;
	p_csw_status += 0x20;
	xhci_queue_bulk_rx((u32)(u64)p_cbw_data, 8, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);
	if (ep_halt_state) {
		xhci_reset_halt();
		ep_halt_state = 0;
	}
	udelay(1000);
}

void xhci_queue_msc_mode_sense(void)
{
	/* Mode Sense 6 */
	debug("\n\n*** MSC_Mode_Sense_6 : ");
	xhci_msc_mode_sense();

	p_cbw_data   += 0x40;
	p_csw_status += 0x20;
	xhci_queue_bulk_rx((u32)(u64)p_cbw_data, 0xc0, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);

	if (ep_halt_state) {
		xhci_reset_halt();
		ep_halt_state = 0;
	}
	udelay(3000);
}

void xhci_queue_msc_request_sense(void)
{
	/* Request Sense */
	debug("\n\n*** MSC_Request_Sense : ");
	xhci_msc_request_sense();

	p_cbw_data   += 0x40;
	p_csw_status += 0x20;
	xhci_queue_bulk_rx((u32)(u64)p_cbw_data, 0x60, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);

	if (ep_halt_state) {
		xhci_reset_halt();
		ep_halt_state = 0;
	}
	udelay(3000);
}


/***************************************
    USB Enumeration flow
****************************************/
int xhci_hcd_link_setup(void)
{
	u32 ret, port_state;
	xhci_port_warm_reset();

	debug("\n==>Port Reset is done !!!");
	xhci_wait_event_queue(TRB_PORT_STATUS);
	/* Query Speed_id */
	ret = xhci_readl(USB3_PORTSC) & DEV_SPEED_MASK;
	debug("\nPort speed is %d\n", ret>>10);
	printf("\n****************************************");
	printf("\n\nPlug in USB 3.0 Cable now on Port %d...", g_port_number);
	printf("\n\n****************************************");

	/* Wait for Port Status Change (Device Attach) */
	/* Check compliance mode recovery quirk */
	do {
		ret = (xhci_readl(USB3_PORTSC) >> 5) & 0xF;
		if (ret == 0xA) {
			printf("\nDetected port in <%d> Compliance mode\n",
			       ret);
			debug("Disconnect and reset USB disk power\n");
			return -1;
		}
		udelay(1);
		ret = xhci_readl(USB3_PORTSC) & PORT_CONNECT;
	} while (!ret);

	/* Port Reset */
	port_state = xhci_readl(USB3_PORTSC);
	port_state = (port_state>>5) & 0xf;
	if (port_state != 0) {
		debug("\nPort State is not in U0 state, state %d\n!!!",
		      port_state);
	}

	/* Confirm CCS connected */
	ret = xhci_readl(USB3_PORTSC) & PORT_CONNECT;
	if (ret != 0x1) {
		printf("\nBad connection, failed\n");
		return -1;
	}

	/* Query Speed_id */
	ret = xhci_readl(USB3_PORTSC) & DEV_SPEED_MASK;
	printf("\nDevice connected, speed is %d\n", ret>>10);
	udelay(1000000);

	printf("\nPort State is in %d state!!!\n", (port_state>>5) & 0xf);
	/* Mark link up */
	link_state = 1;
	return 0;
}


int xhci_hcd_enum(void)
{
	u32 ret, slot_id, block_cnt;

	/* Wait for good link */
	if (xhci_hcd_link_setup() < 0)
		return -1;

	/* Enable Slot Command */
	printf("\n\n*** Enable_Slot ");
	xhci_queue_enable_slot();
	/* Wait for event, retrieve slot_id (we know = 1, but have to do it) */
	if (xhci_wait_event_queue(TRB_COMPLETION))
		return -1;
	udelay(10000);
	slot_id = 1;

	printf("\nAdd more timeouts!!!!\n");

	udelay(1000);

	printf("\ntimeouts are done!!!!\n");
	/* Set Address Command */
	printf("\n\n*** Set_Address ");
	xhci_queue_address_device(slot_id);
	udelay(5000);
	/* Use slot_state as indicator */

	if (xhci_wait_event_queue(TRB_COMPLETION))
		return -2;
	udelay(5000);


	/* Get_Descriptor, first Xfer_trb */
	/* Ring Device_1 DoorBell, DB_target = EP0 update = 0x1 */
	debug("\n\n*** Get_Descriptor:");
	xhci_ch9_get_descriptor();
	xhci_ring_doorbell(EP_0, 0);
	if (xhci_wait_event_queue(TRB_TRANSFER))
		return -1;
	udelay(10000);

	/* Get_Config (9) */
	debug("\n\n*** Get_Configuration:");
	xhci_ch9_get_configuration(9);
	xhci_ring_doorbell(EP_0, 0);
	if (xhci_wait_event_queue(TRB_TRANSFER))
		return -1;
	udelay(100);

	/* Get_Config (44) */
	debug("\n\n*** Get_Configuration:");
	xhci_ch9_get_configuration(44);
	xhci_ring_doorbell(EP_0, 0);
	if (xhci_wait_event_queue(TRB_TRANSFER))
		return -1;
	udelay(1000);

	/* Decode EP assignment */
	debug("Get_config <20>=0x%x, <33>=0x%x, ",
	      *(u8 *)(XHCI_CH9_CONFIG + 20), *(u8 *)(XHCI_CH9_CONFIG + 33));
	ret =  *(u8 *)(XHCI_CH9_CONFIG + 20);
	if ((ret>>4) == 0x8) {
		ep_in_num   = ret & 0xf;
		ret  = *(u8 *)(XHCI_CH9_CONFIG + 33);
		ep_out_num  = ret & 0xf;
	} else {
		ep_out_num  = ret & 0xf;
		ret  = *(u8 *)(XHCI_CH9_CONFIG + 33);
		ep_in_num   = ret & 0xf;
	}
	printf("\n\n---> Get EP number: bulk-in <%d>, bulk-out <%d>, ",
	       ep_in_num, ep_out_num);

	/* Populate EP-Bulk IN/OUT into Device Context */
	debug("\n\n*** Update Bulk-EPs and evaluate context:");
	xhci_update_input_ctx();
	xhci_queue_evaluate_context(slot_id);
	xhci_wait_event_queue(TRB_COMPLETION);
	udelay(10000);

	/* Set_Config */
	debug("\n\n*** Set_Configuration_1 :");
	xhci_ch9_set_configuration();
	xhci_queue_configure_endpoint(slot_id);
	xhci_wait_event_queue(TRB_COMPLETION);
	udelay(100000);

	/* Bulk-only command on EP0 */
	debug("\n\n*** Get LUN :");
	xhci_ch9_get_lun();
	xhci_ring_doorbell(EP_0, 0);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(100);

	/* Take out an extra transfer event */
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(1000);

	/* Queue first MSC_INQUIRY Command */
	debug("\n\n*** MSC_Inquiry : ");
	xhci_msc_inquiry_cmd();

	xhci_queue_bulk_rx((u32)(u64)p_cbw_data, 36, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(1000);

	xhci_queue_msc_test_unit();
	xhci_queue_msc_read_capacity();

	xhci_queue_msc_mode_sense();
	xhci_queue_msc_test_unit();
	xhci_queue_msc_request_sense();
	xhci_queue_msc_test_unit();
	xhci_queue_msc_read_capacity();
	xhci_queue_msc_test_unit();

	block_cnt = 8;
	/* Read_10, Load FAT table */
	debug("\n\n^^^ MSC_Read_10 : ");
	/* Read first 8 block, start from lba=0 */
	xhci_msc_read_10(MSC_BLK_SIZE*block_cnt, 0, block_cnt);
	/* Real data start from second 4K boundary */
	p_cbw_data   = (char *)(XHCI_MSC_REPLY_BUF + 0x1000);
	p_csw_status += 0x20;
	xhci_queue_bulk_data_read((u32)(u64)p_cbw_data,
				  MSC_BLK_SIZE*block_cnt, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);

	if (ep_halt_state) {
		xhci_reset_halt();
		ep_halt_state = 0;
	}

	udelay(1000);
	xhci_queue_msc_test_unit();
	xhci_queue_msc_test_unit();

	printf("\n\nUSB 3.0 enumeration done\n");

	return 0;
}

void xhci_write_test(u32 lba, u32 blk_cnt, u32 write_data_buf)
{
	/* Support 16Kbyte (16packets) in one TRB */
	xhci_queue_msc_test_unit();
	xhci_queue_msc_test_unit();

	/* Write_10 */
	debug("\n\n^^^ MSC_Write_test : lba=0x%x, blk_cnt=%d\n", lba, blk_cnt);
	xhci_msc_write_10(MSC_BLK_SIZE * blk_cnt, lba, blk_cnt);

	p_cbw_data = (char *)(u64)(write_data_buf);
	p_csw_status += 0x20;
	xhci_queue_bulk_data_write((u32)(u64)p_cbw_data,
				   MSC_BLK_SIZE * blk_cnt, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);
	udelay(10000);
}


void xhci_read_test(u32 lba, u32 blk_cnt, u32 read_data_buf)
{
	/* Read_10 */
	debug("\n\n^^^ MSC_Read_test : lba=0x%x, blk_cnt=%d\n", lba, blk_cnt);
	xhci_msc_read_10(MSC_BLK_SIZE * blk_cnt, lba, blk_cnt);

	p_cbw_data   = (char *)(u64)(read_data_buf);
	p_csw_status += 0x20;
	xhci_queue_bulk_data_read((u32)(u64)p_cbw_data,
				  MSC_BLK_SIZE * blk_cnt, 0);
	xhci_queue_bulk_rx((u32)(u64)p_csw_status, UMASS_BBB_CSW_SIZE, TRB_IOC);

	xhci_ring_doorbell(ep_out_num, EP_OUT);
	xhci_ring_doorbell(ep_in_num, EP_IN);
	xhci_wait_event_queue(TRB_TRANSFER);

	if (ep_halt_state) {
		xhci_reset_halt();
		ep_halt_state = 0;
	}

	udelay(3000);
}
