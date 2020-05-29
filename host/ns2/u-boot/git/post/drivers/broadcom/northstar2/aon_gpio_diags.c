/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * This test verifies LEDs driven by AON GPIOs.
 *
 * Note: Modify the DIP switch position before for
 *       successfully executing the test. 
 */

#include <common.h>
#include <post.h>
#include <linux/types.h>

#if CONFIG_POST & CONFIG_SYS_POST_LED
#include <asm/arch/socregs.h>

#define MAX_AON_GPIO	6
#define M0_BASE	0x62000000
#define LED_DEBUG 0

/* Configure all the #6 AON GPIO as output */
static void aon_gpio_cfg_output(void)
{
	writel(0x3F, (GP_OUT_EN + M0_BASE));
}

static void aon_gpio_data_high(int gpio)
{
	unsigned int reg_val;
	reg_val = readl(GP_DATA_OUT + M0_BASE);
	reg_val = reg_val | (1 << gpio);

	writel(reg_val, (GP_DATA_OUT + M0_BASE));
#if LED_DEBUG
	post_log("AON GPIO: %d is set high. \n", gpio);
#endif
}

static void aon_gpio_data_low(int gpio)
{
	unsigned int reg_val;
	reg_val = readl(GP_DATA_OUT + M0_BASE);
	reg_val = reg_val & (~((1 << gpio)));

	writel(reg_val, (GP_DATA_OUT + M0_BASE));
#if LED_DEBUG
	post_log("AON GPIO: %d is set low. \n", gpio);
#endif
}

static void help(void)
{
	post_log("\n -------------------------\n");
	post_log("| AON GPIO DIAG HELP MENU |\n");
	post_log(" -------------------------\n");
}
int AON_GPIO_post_test(int flags)
{
	unsigned int def_reg_val;
	unsigned int val, def_aon_control3_val;
	int gpio;
	char ch;

	if (flags & POST_HELP) {
		help();
		return 0;
	}


	post_log("\n\nAON GPIO LED test\n");

	/* Default Register value */
	def_reg_val = readl(GP_DATA_OUT + M0_BASE);
#if LED_DEBUG
	post_log("Default value of GP_DATA_OUT:  %x\n",def_reg_val);
#endif

	/* Configure drive_sel0 */
	def_aon_control3_val = readl(AON_GPIO_CONTROL3 + M0_BASE);
#if LED_DEBUG
	post_log("\nDefault value of AON_GPIO_CONTROL3:  %x\n",def_aon_control3_val);
#endif
	if (def_aon_control3_val != 0x3f)
	{
		writel(0x3F, (AON_GPIO_CONTROL3 + M0_BASE));
	}

	/* Configure all AON GPIO as output */
	val = readl(GP_OUT_EN + M0_BASE);
#if LED_DEBUG
	post_log("Status of GP_OUT_EN: %x\n",val);
#endif
	if (val != 0x3F)
	{
		aon_gpio_cfg_output(); //writel(GP_OUT_EN, 0x3F);
		val = readl(GP_OUT_EN + M0_BASE);
#if LED_DEBUG
		post_log("GP_OUT_EN value after o/p config: %x\n", val);
#endif
	}

	/* Clear all GPIOs before starting the test*/
	writel(0x0, (GP_DATA_OUT + M0_BASE));
#if LED_DEBUG
	post_log("Status of GP_DATA_OUT before test: %x\n",val);
#endif
	/* Check for DIP switch turn ON */	
        post_log("\nEnsure DIP switch SW4 is ON for all LEDs. Confirm to start the test (Y/N):\n");
        do
        {
                ch = (char)serial_getc();
        } while ((ch !='y') && (ch !='Y')
                        && (ch !='n') && (ch!='N'));
        if ((ch == 'n') || (ch == 'N'))
        {
                post_log("\nError. Exiting the test\n");
                return -1;
        }

	/* Turn ON LEDs*/
	post_log("\nTurning ON the LED's sequentially ...\n");

	for (gpio = 0; gpio < MAX_AON_GPIO; gpio++) {
		aon_gpio_data_high(gpio);
		__udelay(1000000);
	}

	post_log("\nAre the #6 AON GPIO Led's ON? (Y/N)\n");
        do
        {
                ch = (char)serial_getc();
        } while ((ch !='y') && (ch !='Y')
                        && (ch !='n') && (ch!='N'));
	if ((ch == 'n') || (ch == 'N'))
	{
		post_log("\nError. Exiting the test\n");
		return -1;
	}

#if LED_DEBUG
        val = readl(GP_DATA_OUT + M0_BASE);
        post_log("Status of GP_DATA_OUT after ON: %x\n",val);
#endif

	/* Turn OFF LEDs */
	post_log("\nTurning OFF the LED's sequentially ...\n");
	for (gpio = 0; gpio < MAX_AON_GPIO; gpio++) {
		aon_gpio_data_low(gpio);
		__udelay(1000000);
	}

        post_log("\nAre the #6 AON GPIO Led's OFF? (Y/N)\n");
        do
        {
                ch = (char)serial_getc();
        } while ((ch !='y') && (ch !='Y')
                        && (ch !='n') && (ch!='N'));
        if ((ch == 'n') || (ch == 'N'))
        {
                post_log("\nError. Exiting the test\n");
                return -1;
        }

#if LED_DEBUG
        val = readl(GP_DATA_OUT + M0_BASE);
        post_log("Status of GP_DATA_OUT after OFF: %x\n",val);
#endif
	writel(def_reg_val, (GP_DATA_OUT + M0_BASE));
	writel(def_aon_control3_val, (AON_GPIO_CONTROL3 + M0_BASE));
	post_log("\nLEDs restored to the state prior to test. \n");

	return 0;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_LED */
