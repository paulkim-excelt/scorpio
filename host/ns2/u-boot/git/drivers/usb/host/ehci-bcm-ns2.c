/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <asm/arch/socregs.h>
#include <usb/ehci-ci.h>
#include <asm/io.h>
#include <common.h>
#include "ehci.h"
#include <usb.h>

#define ICFG_USB2H_PHY_MISC_STATUS_PLLLOCK_R		0
#define USB2H_PHY_CTRL_P0_PHY_HARD_RESET		9
#define USB2H_PHY_CTRL_P0_CORE_RESET			8
#define USB2H_PHY_CTRL_P0_PHY_SOFT_RESET		6
#define USB2H_PHY_CTRL_P0_PHY_TEST_PORT_PWR_DN		4
#define USB2H_PHY_CTRL_P0_PHY_TEST_PORT_UTMI_PWR_DN	2
#define USB2H_PHY_CTRL_P0_PHY_PLL_PWR_DN		0

/* USB2H Clock init */
int ns2_usb2_host_init(u32 host)
{
	int i, count = 100;
	u32 afe;
	u32 reg_data;
	u32 *reset_ctrl;
	u32 *io_control;
	u32 *status;
	u32 *phy_ctrl;

	switch (host) {
	case 0:
		reset_ctrl = (u32 *)USB2_DRD_IDM_IDM_RESET_CONTROL;
		io_control = (u32 *)USB2_DRD_IDM_IDM_IO_CONTROL_DIRECT;
		status = (u32 *)ICFG_USB2_DRD_PHY_MISC_STATUS_REG;
		afe = CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2_DRD_PHY_AFE_CORERDY_VDDC_I;
		phy_ctrl = (u32 *)USB2H_DRD_Phy_Ctrl_P0;
		break;
	case 1:
		reset_ctrl = (u32 *)USB2H_M0_IDM_IDM_RESET_CONTROL;
		io_control = (u32 *)USB2H_M0_IDM_IDM_IO_CONTROL_DIRECT;
		status = (u32 *)ICFG_USB2H_M0_PHY_MISC_STATUS_CONFIG_REG;
		afe = CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2H_PHY_M0_AFE_CORERDY_VDDC_I;
		phy_ctrl = (u32 *)USB2H_M0_Phy_Ctrl_P0;
		break;
	case 2:
		reset_ctrl = (u32 *)USB2H_M1_IDM_IDM_RESET_CONTROL;
		io_control = (u32 *)USB2H_M1_IDM_IDM_IO_CONTROL_DIRECT;
		status = (u32 *)ICFG_USB2H_M1_PHY_MISC_STATUS_CONFIG_REG;
		afe = CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2H_PHY_M1_AFE_CORERDY_VDDC_I;
		phy_ctrl = (u32 *)USB2H_M1_Phy_Ctrl_P0;
		break;
	default:
		return -EINVAL;
	}

	if (host == 0) {
		/* Configure the DRD in Host mode */
		reg_data = readl(ICFG_USB2DRD_FSM_CONTROL);
		reg_data |= 0xC;
		writel(reg_data, ICFG_USB2DRD_FSM_CONTROL);
	}
	/* give hardware time to settle */
	udelay(1000);

	/* reset USB controller */
	reg_data = readl(reset_ctrl);
	reg_data |= (1 << USB2_DRD_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, reset_ctrl);

	if (host == 0) {
		/* Phy bring up is done with USBH controller in reset */
		reg_data = readl((DMU_COMMON_ROOT + CRMU_USB_PHY_AON_CTRL));
		reg_data |= (1 << CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P0_AFE_CORERDY_VDDC);
		writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB_PHY_AON_CTRL));
	}

	/* Disable USB2 controller clock */
	reg_data = readl(io_control);
	reg_data &= ~(1 << USB2_DRD_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	writel(reg_data, io_control);

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB2_CONTROL));
	reg_data |= 1 << afe;
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB2_CONTROL));

	/* give hardware time to settle */
	udelay(1000);

	i = 0;
	do {
		i++;
		reg_data = readl(status);
		if (i >= count) {
			printf("failed to get PLL lock for USB2H_PHY\n");
			return -ETIMEDOUT;
		}
		udelay(10);
	} while (!(reg_data & (1 << ICFG_USB2H_PHY_MISC_STATUS_PLLLOCK_R)));

	printf("\nInitializing USB2 Host\n");

	/* USB Host clock enable */
	reg_data = readl(io_control);
	reg_data |= (1 << USB2H_M0_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	writel(reg_data, io_control);

	/* Enter reset */
	reg_data = readl(reset_ctrl);
	reg_data |= (1 << USB2H_M0_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, reset_ctrl);

	udelay(100);

	/* Exit reset */
	reg_data &= ~(1 << USB2H_M0_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, reset_ctrl);

	udelay(1000);
	board_ehci_hcd_init(host);
	/* This controller (root hub) we have only one port */
	/* Pull these fields out of reset */
	writel(((1 << USB2H_PHY_CTRL_P0_PHY_HARD_RESET) |
		(1 << USB2H_PHY_CTRL_P0_CORE_RESET) |
		(0x3 << USB2H_PHY_CTRL_P0_PHY_SOFT_RESET) |
		(0x3 << USB2H_PHY_CTRL_P0_PHY_TEST_PORT_PWR_DN) |
		(0x3 << USB2H_PHY_CTRL_P0_PHY_TEST_PORT_UTMI_PWR_DN) |
		(0x3 << USB2H_PHY_CTRL_P0_PHY_PLL_PWR_DN)),
		phy_ctrl);
	return 0;
}

void usb2_drd_clock_disable(void)
{
	u32 reg_data;

	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB2_CONTROL));
	reg_data &= ~(1 << CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2_DRD_PHY_AFE_CORERDY_VDDC_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB2_CONTROL));

	/* Disable USB2_DRD controller clock */
	reg_data = readl(USB2_DRD_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data |= (1 << USB2_DRD_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	writel(reg_data, USB2_DRD_IDM_IDM_IO_CONTROL_DIRECT);

	/* Phy bring up is done with USBH controller in reset */
	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB_PHY_AON_CTRL));
	reg_data &= ~(1 <<
		      CRMU_USB_PHY_AON_CTRL__CRMU_USBPHY_P0_AFE_CORERDY_VDDC);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB_PHY_AON_CTRL));

	/* Configure the DRD in Host mode */
	reg_data = readl(ICFG_USB2DRD_FSM_CONTROL);
	reg_data &= ~(0xC);
	writel(reg_data, ICFG_USB2DRD_FSM_CONTROL);

	/* give hardware time to settle */
	udelay(1000);

	/* reset USB2_DRD controller */
	reg_data = readl(USB2_DRD_IDM_IDM_RESET_CONTROL);
	reg_data |= (1 << USB2_DRD_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, USB2_DRD_IDM_IDM_RESET_CONTROL);
}

void usbh_clock_disable_m1(void)
{
	u32 reg_data;

	/* Phy bring up is done with USBH controller in reset */
	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB2_CONTROL));
	reg_data &= ~(1 << CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2H_PHY_M1_AFE_CORERDY_VDDC_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB2_CONTROL));

	/* give hardware time to settle */
	udelay(100);

	/* Disable USBH controller clock */
	reg_data = readl(USB2H_M1_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data &= ~(1 << USB2H_M1_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	writel(reg_data, USB2H_M1_IDM_IDM_IO_CONTROL_DIRECT);

	/* reset USBH controller */
	reg_data = readl(USB2H_M1_IDM_IDM_RESET_CONTROL);
	reg_data |= (1 << USB2H_M1_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, USB2H_M1_IDM_IDM_RESET_CONTROL);
}

void usbh_clock_disable_m0(void)
{
	u32 reg_data;

	/* Phy bring up is done with USBH controller in reset */
	reg_data = readl((DMU_COMMON_ROOT + CRMU_USB2_CONTROL));
	reg_data &= ~(1 << CRMU_USB2_CONTROL__CRMU_PERIPHERALS_USB2H_PHY_M0_AFE_CORERDY_VDDC_I);
	writel(reg_data, (DMU_COMMON_ROOT + CRMU_USB2_CONTROL));

	/* give hardware time to settle */
	udelay(100);

	/* Disable USBH controller clock */
	reg_data = readl(USB2H_M0_IDM_IDM_IO_CONTROL_DIRECT);
	reg_data &= ~(1 << USB2H_M0_IDM_IDM_IO_CONTROL_DIRECT__clk_enable);
	writel(reg_data, USB2H_M0_IDM_IDM_IO_CONTROL_DIRECT);

	/* reset USBH controller */
	reg_data = readl(USB2H_M0_IDM_IDM_RESET_CONTROL);
	reg_data |= (1 << USB2H_M0_IDM_IDM_RESET_CONTROL__RESET);
	writel(reg_data, USB2H_M0_IDM_IDM_RESET_CONTROL);
}

/*
 * Function to initialize USB host related low level hardware including PHY,
 * clocks, etc.
 */
int ehci_hcd_init(int index, enum usb_init_type init,
		  struct ehci_hccr **hccr, struct ehci_hcor **hcor)
{
	int ret = 0;

	/* For now we don't support device mode, only host mode support */
	if (init == USB_INIT_DEVICE)
		return -1;

	ret = ns2_usb2_host_init(index);

	/* map registers */
	switch (index) {
	case 0:
		*hccr = (struct ehci_hccr *)(USB2H_DRD_HCCAPBASE);
		break;
	case 1:
		*hccr = (struct ehci_hccr *)(USB2H_M0_HCCAPBASE);
		break;
	case 2:
		*hccr = (struct ehci_hccr *)(USB2H_M1_HCCAPBASE);
		break;
	default:
		return -EINVAL;
	}

	if (hcor)
		*hcor = (struct ehci_hcor *)((phys_addr_t)(*hccr) +
			HC_LENGTH(ehci_readl(&(*hccr)->cr_capbase)));

	return ret;
}

/*
 * Function to terminate USB host related low level hardware including PHY,
 * clocks, etc.
 */
int ehci_hcd_stop(int index)
{
	switch (index) {
	case 0:
		usb2_drd_clock_disable();
		break;
	case 1:
		usbh_clock_disable_m0();
		break;
	case 2:
		usbh_clock_disable_m1();
		break;
	default:
		printf("%s unexpected controller index=%d\n", __func__, index);
		return -EINVAL;
	}

	return 0;
}

