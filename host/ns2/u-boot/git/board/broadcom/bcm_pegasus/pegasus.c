/*
 * (C) Copyright 2013
 * David Feng <fenghua@phytium.com.cn>
 * Sharma Bhupesh <bhupesh.sharma@freescale.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <configs/bcm_pegasus.h>
#include <asm/arch/socregs.h>
#ifdef CONFIG_SCSI_AHCI_PLAT
#include <asm/arch-bcm_pegasus/bcm_otp.h>
#include <ahci.h>
#include <scsi.h>
#endif /* CONFIG_SCSI_AHCI_PLAT */
#include <asm/arch/smc_call.h>
#include <i2c.h>

DECLARE_GLOBAL_DATA_PTR;

extern int bcm_mdio_init(void);
extern int bcmiproc_eth_register(u8 dev_num);

#define PSCI_0_2_FN_SYSTEM_RESET  0x84000009

#define XHC_PORTSC1	0x28330420
#define XHC_PORTSC3	0x28330440
#define PORTSC_PR	4
#define PORTSC_PLS	5
#define PORTSC_PP	9
#define PORTSC_WRC	19
#define PORTSC_PRC	21
#define PORTSC_WPR	31

int board_init(void)
{
	u32 val;
#ifdef CONFIG_UBOOT_AT_EL3
 	uint i;
 	int *ptr;
 	unsigned int *ptr2;
#define DDR_s0_TZASC_REGION_SETUP_LOW_1_OFFSET 0x110
#define DDR_s0_TZASC_REGION_SETUP_HIGH_1_OFFSET 0x114
#define DDR_s0_TZASC_REGION_ATTRIBUTES_1_OFFSET 0x118

#define DDR_s0_TZASC_REGION_SETUP_LOW_2_OFFSET 0x120
#define DDR_s0_TZASC_REGION_SETUP_HIGH_2_OFFSET 0x124
#define DDR_s0_TZASC_REGION_ATTRIBUTES_2_OFFSET 0x128

#define TZAC_DDR_S0_BASE 0x28130000
#define TZAC_DDR_S1_BASE 0x28140000
#define TZAC_DDR_S2_BASE 0x28150000
#define AXIIC_APBY_s0_security 0x0010002c

#define APBY_IDM_IDM_IO_CONTROL_DIRECT 0x00990408
 	writel(0xFFFFFFFF, AXIIC_APBY_s0_security);
	writel(0xFFFFFFFF, AXIIC_APBX_s0_security);
	writel(0xFFFFFFFF, AXIIC_APBZ_s0_security);
	writel(0xFFFFFFFF, AXIIC_APBR_s0_security);

#ifdef PCIE_NIC_ENABLE
#define SATA_NIC_pcie4_s0_security 0x5000000c
#define PCIE_NIC_pcie0_s0_security 0x5800000c
#define PCIE_NIC_pcie1_s0_security 0x58000010
#define PCIE_NIC_pcie2_s0_security 0x58000014
#define PCIE_NIC_APBT_s0_security 0x5800001c
	writel(0xFFFFFFFF, SATA_NIC_pcie4_s0_security);
	writel(0xFFFFFFFF, PCIE_NIC_pcie0_s0_security);
	writel(0xFFFFFFFF, PCIE_NIC_pcie1_s0_security);
	writel(0xFFFFFFFF, PCIE_NIC_pcie2_s0_security);
	writel(0xFFFFFFFF, PCIE_NIC_APBT_s0_security);
#endif

 	/*For 2.13 onwards*/
 	i = readl(APBY_IDM_IDM_IO_CONTROL_DIRECT);
 	i |= 0x4;
 	writel(i, APBY_IDM_IDM_IO_CONTROL_DIRECT);

 	i = readl(0x2800001C);
 	i = i | 1;
 	writel(i, 0x2800001C);

 	ptr = (int*)0x2e194000;
 	*ptr = 0xc0000003;

 	ptr = (int*)0x2e195000;
 	*ptr = 0xc0000003;

 	ptr2 = (unsigned int*) IHOST_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1;
 	*ptr2 |= (1<<IHOST_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__BROADCASTOUTER);
 	*ptr2 |= (1<<IHOST_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__BROADCASTINNER);
 	*ptr2 &= ~(1<<IHOST_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__SYSBARDISABLE);

 	ptr2 = (unsigned int*) PAE_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1;
 	*ptr2 |= (1<<PAE_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__BROADCASTOUTER);
 	*ptr2 |= (1<<PAE_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__BROADCASTINNER);
 	*ptr2 &= ~(1<<PAE_CONFIG_A53_CRM_SUBSYSTEM_CONFIG_1__SYSBARDISABLE);

 	printf("Configuring DDR as non-secure\n");
 	writel(0x0,TZAC_DDR_S0_BASE+ DDR_s0_TZASC_REGION_SETUP_LOW_2_OFFSET);
 	writel(0x0,TZAC_DDR_S0_BASE+ DDR_s0_TZASC_REGION_SETUP_HIGH_2_OFFSET);
 	writel(0xF000003D,TZAC_DDR_S0_BASE+ DDR_s0_TZASC_REGION_ATTRIBUTES_2_OFFSET);
 	writel(0x3,TZAC_DDR_S0_BASE+0x4);

 	writel(0x0,TZAC_DDR_S1_BASE+ DDR_s0_TZASC_REGION_SETUP_LOW_2_OFFSET);
 	writel(0x0,TZAC_DDR_S1_BASE+ DDR_s0_TZASC_REGION_SETUP_HIGH_2_OFFSET);
 	writel(0xF000003D,TZAC_DDR_S1_BASE+ DDR_s0_TZASC_REGION_ATTRIBUTES_2_OFFSET);
 	writel(0x3,TZAC_DDR_S1_BASE+0x4);

 	writel(0x0,TZAC_DDR_S2_BASE+ DDR_s0_TZASC_REGION_SETUP_LOW_2_OFFSET);
 	writel(0x0,TZAC_DDR_S2_BASE+ DDR_s0_TZASC_REGION_SETUP_HIGH_2_OFFSET);
 	writel(0xF000003D,TZAC_DDR_S2_BASE+ DDR_s0_TZASC_REGION_ATTRIBUTES_2_OFFSET);
	writel(0x3,TZAC_DDR_S2_BASE+0x4);

	/* Move the below line to board_late_init later */
	/* Bring DMAC out of reset */
	writel(0x0, DMAC_M0_IDM_RESET_CONTROL);
#endif
	if (is_usb_subsys_supported()) {
		/* Reset xHC Port 3.
		 * For Pegasus, Port 1 is USB3. Port 3 is USB2. The below reset
		 * is required to overcome problem of detecting USB3 pendrives
		 * as USB2 initially and get to an indeterminate state during
		 * enumeration.
		 */
		val = readl(XHC_PORTSC3);
		writel((val | (1 << PORTSC_PR)), XHC_PORTSC3);
		mdelay(200);
	}
	return 0;
}

int dram_init(void)
{
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

void dram_init_banksize(void)
{
	char *s = getenv("board_name");

	gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	/*
	 * 17mmx17mm sku has 2GB in rank1 bank, we decide the sku
	 * based on "board_name" u-boot env variable. If value is
	 * is not set then 23mmx23mm configuration will be assumed.
	 * reset is required after variable is set and saved.
	 * FIXME: This should be replaced with OTP based detection
	 * once ready.
	 */
	if (s != NULL && !strcmp(s, PEGASUS_17MM_BOARD)) {
		printf("DDR size adjusted for 17mm\n");
		gd->bd->bi_dram[1].size = PHYS_SDRAM_2_17MM_SIZE;
	} else {
		gd->bd->bi_dram[1].size = PHYS_SDRAM_2_23MM_SIZE;
	}
}

/*
 * Board specific reset that is system reset.
 */
void reset_cpu(ulong addr)
{
	/* Invoke SMC call for CRMU soft reset */
	__invoke_fn_smc(PSCI_0_2_FN_SYSTEM_RESET, 0, 0, 0);
}

/* USB Reset for XHCI -Board specific implementation as specified
 * in usb_hub_configure().
 */
void usb_hub_reset_devices(int port)
{
	u32 val;
	int i = 0;
	int timeout = 10;

	if (!is_usb_subsys_supported()) {
		printf("USB subsystem not supported\n");
		return;
	}
	/* Port 1 is USB3 port. Since it's not in good state during
	 * power on, it requires a warm reset.
	 */
	if (port == 1) {
		val = readl(XHC_PORTSC1);
		debug("PORTSC1: 0x%x\n", val);
		val |= (1 << PORTSC_WRC) | (1 << PORTSC_PRC);
		writel(val, XHC_PORTSC1);

		val = readl(XHC_PORTSC1);
		val |= (1 << PORTSC_WPR);
		writel(val, XHC_PORTSC1);

		debug("waiting for warm reset to complete\n");
		do {
			timeout--;
			mdelay(100);
			val = readl(XHC_PORTSC1);
			debug("PORTSC1: 0x%x.. loop: %d\n", val, ++i);
		} while ((val & 0x10) && (!(val & 0x200000)) && timeout);

		if (!timeout)
			printf("%s: Warm reset error for Port %d\n",
			       __func__, port);
		val = readl(XHC_PORTSC1);
		debug("Setting PRC, WRC bits.. PORTSC1: 0x%x", val);
		val |= (1 << PORTSC_WRC) | (1 << PORTSC_PRC);
		writel(val, XHC_PORTSC1);

		val = readl(XHC_PORTSC1);
		debug("PORTSC1: 0x%x\n", val);
		val &= ~(1 << PORTSC_PP);
		writel(val, XHC_PORTSC1);
		/* Providing delay between Port power off and on */
		mdelay(100);

		val = readl(XHC_PORTSC1);
		debug("PORTSC1: 0x%x\n", val);
		val |= (1 << PORTSC_PP);
		writel(val, XHC_PORTSC1);

		val = readl(XHC_PORTSC1);
		debug("PORTSC1: 0x%x\n", val);
		val |= (0x5 << PORTSC_PLS);
		writel(val, XHC_PORTSC1);

		mdelay(500);
		val = readl(XHC_PORTSC1);
		debug("PORTSC1: 0x%x\n", val);
	}
	return;
}

#ifdef CONFIG_BOARD_LATE_INIT

#ifdef CONFIG_SCSI_AHCI_PLAT
#undef debug
#ifdef DEBUG
#define debug(x)  printf x
#else /* def DEBUG */
#define debug(x)
#endif /* def DEBUG */

#define SATA_CFG_BASE	(SATA_SUBSYSTEM_CFG_SATA_SUBSYSTEM_REV_ID)

/* SATA Combo PHY initialization */
static int sata_combo_phy_init(uint32_t instance)
{
#define SATA_COMBO_PHY_PLL_LOCK_TIMEOUT		(1000)
	uint32_t val;
	uint32_t mask;
	uint32_t *addr;
	uint8_t *sata_base;
	uint8_t *pll30_base;
	uint8_t *txpmd_base;
	uint32_t count = SATA_COMBO_PHY_PLL_LOCK_TIMEOUT;

	if (instance > CONFIG_SYS_SCSI_MAX_DEVICE) {
		printf("%s(): Error: Invalid SATA conroller ID %d\n",
			__func__, instance);
		return -EINVAL;
	}

	/* Set PHY access uisng APB mode in Combo Phy control Status register */
	addr = (uint32_t *)(SATA_CFG_BASE +
			SATA_SUBSYSTEM_CFG_COMBOPHY_0_CONTOL_STATUS_OFFSET);
	val = readl(addr + instance);

	/* Clear Power control override */
	mask = 0x3 <<
	SATA_SUBSYSTEM_CFG_COMBOPHY_0_CONTOL_STATUS__POWER_CONTROL_OVERRIDE_I_R;
	val &= ~mask;

	/* Set APB mode */
	val |= 1 << SATA_SUBSYSTEM_CFG_COMBOPHY_0_CONTOL_STATUS__APB_MODE_R;

	/* Combo Phy control status register */
	writel(val, (addr + instance));

	if (instance == 0)
		sata_base = (uint8_t *)SATA_0_SATA_GRB_REVISION;
	else if (instance == 1)
		sata_base = (uint8_t *)SATA_1_SATA_GRB_REVISION;
	/*
	 * APB based access to PHY registers.
	 * All PHY registers are 16 bit wide.
	 * Using APB(32 bit wide), can access 2 PHY registers at same time */
#define SATA_APB_MDIO_PLL30	(0x10000)
#define SATA_APB_MDIO_TXPMD	(0x10100)

#define SATA_APB_MDIO_OFF_00_01	(0x00)
#define SATA_APB_MDIO_OFF_02_03	(0x04)
#define SATA_APB_MDIO_OFF_04_05	(0x08)
#define SATA_APB_MDIO_OFF_06_07	(0x0c)
#define SATA_APB_MDIO_OFF_08_09	(0x10)
#define SATA_APB_MDIO_OFF_0A_0B	(0x14)
#define SATA_APB_MDIO_OFF_0C_0D	(0x18)
#define SATA_APB_MDIO_OFF_0E_0F	(0x1c)
#define SATA_APB_MDIO_OFF_10_11	(0x20)
#define SATA_APB_MDIO_OFF_12_13	(0x24)
#define SATA_APB_MDIO_OFF_14_15	(0x28)
#define SATA_APB_MDIO_OFF_16_17	(0x2c)
#define SATA_APB_MDIO_OFF_18_19	(0x30)

#define CLEAR_UPPER_16		(0xFFFF)
#define CLEAR_LOWER_16		(0xFFFF0000)

#define ANA_CAL_CONTROL1_CAP_CHARGE_TIME	(0x32)
#define ANA_CAL_CONTROL3_CALIB_SETUP_TIME	(0xa)
#define ANA_PLL_A_CONTROL_0_1	(0x01D05744)
#define ANA_PLL_A_CONTROL_2_3	(0xAA801DE8)
#define ANA_PLL_A_CONTROL_4_5	(0x00448827)
#define ANA_PLL_A_CONTROL_6_7	(0x0830c000)
#define ANA_PLL_A_CONTROL_8	(0x00000000)

#define TXPMD_GEN_CONTROL	(0x10000000)

#define SATA_PHY_PLL_LOCK	(1 << \
		SATA_SUBSYSTEM_CFG_COMBOPHY_0_CONTOL_STATUS__PLL_SS_LOCK_O)

	/* PHY PLL30 Block */
	pll30_base = sata_base + SATA_APB_MDIO_PLL30;

	/* anaCalControl1 offset 0xA */
	val = readl(pll30_base + SATA_APB_MDIO_OFF_0A_0B);
	val &= CLEAR_LOWER_16;
	val |= ANA_CAL_CONTROL1_CAP_CHARGE_TIME;
	writel(val, (pll30_base + SATA_APB_MDIO_OFF_0A_0B));

	/* anaCalControl3 offset 0xC */
	val = readl(pll30_base + SATA_APB_MDIO_OFF_0C_0D);
	val &= CLEAR_LOWER_16;
	val |= ANA_CAL_CONTROL3_CALIB_SETUP_TIME;
	writel(val, (pll30_base + SATA_APB_MDIO_OFF_0C_0D));

	/* anaAmpControl offset 0x6 No longer used.
	 * anaFreqDetCntr offset 0x7, should be 0x0 */
	writel(0, (pll30_base + SATA_APB_MDIO_OFF_06_07));

	/* anaPllAControl0 offset 0x10, val 0x5744
	 * anaPllAControl1 offset 0x11, val 0x01D0 */
	writel(ANA_PLL_A_CONTROL_0_1, (pll30_base + SATA_APB_MDIO_OFF_10_11));

	/* anaPllAControl2 offset 0x12, val 0x1DE8
	 * anaPllAControl3 offset 0x13, val 0xAA80 */
	writel(ANA_PLL_A_CONTROL_2_3, (pll30_base + SATA_APB_MDIO_OFF_12_13));

	/* anaPllAControl4 offset 0x14, val 0x8827
	 * anaPllAControl5 offset 0x15, val 0x0044 */
	writel(ANA_PLL_A_CONTROL_4_5, (pll30_base + SATA_APB_MDIO_OFF_14_15));

	/* anaPllAControl6 offset 0x16, val 0xc000
	 * anaPllAControl7 offset 0x17, val 0x0830 */
	writel(ANA_PLL_A_CONTROL_6_7, (pll30_base + SATA_APB_MDIO_OFF_16_17));

	/* anaCalControl8 offset 0x18, value 0x0 */
	writel(ANA_PLL_A_CONTROL_8, (pll30_base + SATA_APB_MDIO_OFF_18_19));

	/* TXPMD block registers */
	txpmd_base = sata_base + SATA_APB_MDIO_TXPMD;

	/* General control Register offset 0x1, val 0x1000 */
	val = readl(txpmd_base + SATA_APB_MDIO_OFF_00_01);
	val &= CLEAR_UPPER_16;
	val |= TXPMD_GEN_CONTROL;
	writel(val, (txpmd_base + SATA_APB_MDIO_OFF_00_01));

	/* Check fpr PHY PLL Lock in Combo Phy control Status register */
	addr = (uint32_t *)(SATA_CFG_BASE +
			SATA_SUBSYSTEM_CFG_COMBOPHY_0_CONTOL_STATUS_OFFSET);
	do {
		udelay(10);
		val = readl(addr + instance);
		if (val & SATA_PHY_PLL_LOCK)
			break;
	} while (--count);

	if (!count) {
		printf("ERR: SATA PHY PLL Lock Failed\n");
		return -EAGAIN;
	}

	printf("SATA Controller %d PHY PLL Lock Success\n", instance);
	return 0;
}

/* Sata driver initializations. Bringing out of Resets.
 * instance can either 0 or 1 */
static int sata_driver_init(uint32_t instance)
{
	uint32_t val;
	uint8_t *addr;
#define AXI_SATA_M0_M1_IDM_OFFSET	(0x10000)
#define AXI_SATA_IDM_IO_CTRL_AWCACHE	(0xe)
#define AXI_SATA_IDM_IO_CTRL_ARCACHE	(0xf)
#define AXI_SATA_IDM_IO_CTRL_AWUSER	(0x7)
#define AXI_SATA_IDM_IO_CTRL_ARUSER	(0x1e)

#define CDRU_BASE	(CDRU_GENPLL_CONTROL0)

	if (instance > CONFIG_SYS_SCSI_MAX_DEVICE) {
		printf("%s(): Error: Invalid SATA conroller ID %d\n",
			__func__, instance);
		return -EINVAL;
	}

	/*
	 * TODO PLL programming is missing.
	 * According to clock tree diagram program PLL values
	 * According to SV team, M0 will program the PLLs.
	 */

	addr = (uint8_t *)AXI_SATA_M0_IDM_IO_CONTROL_DIRECT;

	/* AXI_SATA_M0/M1_IDM_IO_CONTROL_DIRECT */
	val = 1 << AXI_SATA_M0_IDM_IO_CONTROL_DIRECT__CLK_ENABLE_R;
	val |= AXI_SATA_IDM_IO_CTRL_AWCACHE <<
		AXI_SATA_M0_IDM_IO_CONTROL_DIRECT__AWCACHE_R;
	val |= AXI_SATA_IDM_IO_CTRL_ARCACHE <<
		AXI_SATA_M0_IDM_IO_CONTROL_DIRECT__ARCACHE_R;
	val |= AXI_SATA_IDM_IO_CTRL_AWUSER <<
		AXI_SATA_M0_IDM_IO_CONTROL_DIRECT__AWUSER_R;
	val |= AXI_SATA_IDM_IO_CTRL_ARUSER <<
		AXI_SATA_M0_IDM_IO_CONTROL_DIRECT__ARUSER_R;
	/* TODO Get the info about bits 20, 21, 22 and 23. Set these bits */
	val |= 0xF << 20;

	writel(val, (addr + (instance * AXI_SATA_M0_M1_IDM_OFFSET)));

	val = readl(CDRU_BASE + CDRU_PCIE);
	if (instance == 0)
		val |= 1 << CDRU_PCIE__combophy0_is_sata_R;
	else if (instance == 1)
		val |= 1 << CDRU_PCIE__combophy1_is_sata_R;
	writel(val, (CDRU_BASE + CDRU_PCIE));

	/* TODO Not needed: AXI_SATA_M0/M1_IDM_IO_CONTROL_DIRECT write */

	/* TODO Not Needed: Assert Reset */
	/*addr = AXI_SATA_M0_IDM_M_IDM_RESET_CONTROL;
	writel(0x1, (addr + (instance * AXI_SATA_M0_M1_IDM_OFFSET)));*/

	/* Release Reset */
	addr = (uint8_t *)AXI_SATA_M0_IDM_M_IDM_RESET_CONTROL;
	writel(0x0, (addr + (instance * AXI_SATA_M0_M1_IDM_OFFSET)));

	printf("SATA controller %d out of RESET\n", instance);
	return 0;
}

static void sata_combo_phy_pwron (void)
{
	uint8_t i;

	for (i = 0; i < CONFIG_SYS_SCSI_MAX_DEVICE; i++) {
#ifdef BCM_OTP_CHECK_ENABLED
		if (!is_sata_port_enabled(i)) {
			printf("SATA_%d not supported\n", i);
			continue;
		}
#endif
		if (sata_combo_phy_init(i)) {
			printf("ERR: SATA %d: Combo PHY Init FAILED\n", i);
			return;
		}
	}
}

/* Offset between SATA_0 and SATA_1 controllers */
#define SATA_1_CONTROLLER_OFFSET	(0x20000)
void scsi_init(void)
{
	uint32_t offset;
	uint8_t i = 0;

#ifdef BCM_OTP_CHECK_ENABLED
	if (!is_sata_port_enabled(i))
		return;
#endif
	/* Default only SATA controller #0 */
	offset = i * SATA_1_CONTROLLER_OFFSET;
	ahci_init((void __iomem *)((uint64_t) (offset +
				SATA_0_SATA_AHCI_GHC_HBA_CAP)));
	scsi_scan(1);
}

void scsi_bus_reset(void)
{
	uint32_t offset;
	uint8_t i;

	for (i = 0; i < CONFIG_SYS_SCSI_MAX_DEVICE; i++) {
		offset = i * SATA_1_CONTROLLER_OFFSET;
#ifdef BCM_OTP_CHECK_ENABLED
		if (!is_sata_port_enabled(i))
			continue;
#endif
		ahci_init((void __iomem *)((uint64_t) (offset +
					SATA_0_SATA_AHCI_GHC_HBA_CAP)));
	}
}

#endif /* CONFIG_SCSI_AHCI_PLAT */

static void reset_card(void)
{
	unsigned char sl_addr = 0x20;
	unsigned char reg_offset;
	unsigned char buf;

	/*
	 * We first need to select I2C2 to toggle SDIO_EN
	 * to 'low', then toggle VDD_SDIO connected on I2C1
	 * and then finally toggle back SDIO_EN to 'high'
	 */

	i2c_set_bus_num(2);

	if (!i2c_probe(sl_addr)) {
		reg_offset = 0x6;
		buf = 0xef;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
		reg_offset = 0x2;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
	} else {
		printf("failed to probe i2c device on I2C2\n");
	}
	i2c_set_bus_num(1);
	if (!i2c_probe(sl_addr)) {
		reg_offset = 0x7;
		buf = 0xfe;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
		reg_offset = 0x3;
		buf = 0xff;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
		mdelay(200);
		buf = 0xfe;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
	} else {
		printf("failed to probe i2c device on I2C1\n");
	}
	i2c_set_bus_num(2);
	if (!i2c_probe(sl_addr)) {
		reg_offset = 0x2;
		buf = 0xff;
		i2c_write(sl_addr, reg_offset, 1, &buf, 1);
	}
}

int board_late_init(void)
{
	/*
	 * Calling dram_init_banksize again during late init
	 * to set ddr parameter according to board_name variable
	 * TODO: remove it once OTP based detection is available.
	 */
	dram_init_banksize();
	sata_combo_phy_pwron();

	reset_card();
	return 0;
}
#endif /* CONFIG_BOARD_LATE_INIT */


int board_eth_init(bd_t *bis)
{
    int rc = -1;

#ifdef CONFIG_BCM_CMIC_MDIO
	printf("Initializing CMIC MDIO controller.....\n");
	rc = bcm_mdio_init();
#endif
#ifdef CONFIG_BCMIPROC_ETH
	printf("Registering GMAC ethernet.....\n");
	rc = bcmiproc_eth_register(0);
#endif
    return rc;
}
