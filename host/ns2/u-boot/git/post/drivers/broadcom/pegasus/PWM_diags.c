/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 */

#include <common.h>
#include <post.h>
#include <command.h>
#include <cli.h>
#include "../../../drivers/pwm/pegasus_pwm.h"

#if CONFIG_SYS_POST_PWM

int PWM_post_test(int flags)
{
	uint32_t iomux_val, gpio_val;
	char *s = getenv("board_name");
	char ch;

	if (s) {
		if (!strcmp(s, PEGASUS_XMC_BOARD)) {
			post_log("\nPWM module is not available on XMC\n");
			return BCM_NO_IP;
		}
	} else {
		post_log("\n board_name env not set\n");
		return -1;
	}

	post_log("\nPWM diag starts ..\n");
	if (!strcmp(s, PEGASUS_17MM_BOARD))
		post_log("\nMake sure SW3.1 Switch to ON position.\n");

	else
		post_log("\nMake sure SW3.1 and 3.2 Switch to ON position.\n");

	post_log("\nAnd Make sure SW15.7 and SW15.8 Switch to OFF position.\n");
	post_getConfirmation("\n(Y/N)\n");

	/* iomux spi0 to GPIO mode to drive PWM LEDs, write mux reg[1:0]=01 */
	iomux_val = readl(ICFG_IOMUX_CTRL_REG);
	iomux_val |= (1 << 0);
	writel(iomux_val, ICFG_IOMUX_CTRL_REG);

	/* make GPIO 0,1,2 output */
	gpio_val = readl(ChipcommonG_GP_OUT_EN);
	gpio_val |= (1 << 0) | (1 << 1) | (1 << 2);
	writel(gpio_val, ChipcommonG_GP_OUT_EN);

	/* drive GPIO 0,1,2 high */
	gpio_val = readl(ChipcommonG_GP_DATA_OUT);
	gpio_val |= (1 << 0) | (1 << 1) | (1 << 2);
	writel(gpio_val, ChipcommonG_GP_DATA_OUT);

	post_log("\nObserve flashing LED-D35,LED Matrix and Buzzer sound on the Board for 10 seconds\n");

	/* Configuring all 4 PWM channels */
	pegasus_pwm_cfg_all();

	/* Enabling all 4 PWM channels */
	pegasus_pwm_enable_all();

	/* Wait for 10 seconds and Disable all Channels */
	 udelay(10000000);
	 pegasus_pwm_shutdown_all();

	 /* restore iomux from GPIO to spi0 */
	iomux_val = readl(ICFG_IOMUX_CTRL_REG);
	iomux_val &= ~(1 << 0);
	writel(iomux_val, ICFG_IOMUX_CTRL_REG);

	post_log("\nDo you observe flashing LED-D35, LED Matrix and Buzzer sound? (Y/N)\n");
	do {
		ch = (char)serial_getc();
	} while ((ch != 'y') && (ch != 'Y') && (ch != 'n') && (ch != 'N'));

	if ((ch == 'n') || (ch == 'N')) {
		post_log("\nError. Exiting the test\n");
		return -1;
	}
	post_log("\nTurn back SW3.1 and SW3.2 to Off position.");
	post_getConfirmation("(Y/N)\n");
	return 0;
}
#endif
