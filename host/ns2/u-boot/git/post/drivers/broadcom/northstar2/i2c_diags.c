/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * I2C test
 */

#include <common.h>
#include <post.h>
#include <i2c.h>

#if CONFIG_POST & CONFIG_SYS_POST_I2C

extern int i2c_set_bus_num(unsigned int bus);
extern int i2c_write_mux_ctrl_byte (u8 devaddr, u8 ctrl_byte);
extern int i2c_write_ctrl_bytes (u8 devaddr, u8 *data, u8 size);
//extern int i2c_read_mux_ctrl_byte (u8 devaddr);

/* NS2 I2C Map */
struct ns2_i2c_map {
	char name[30];	//Slave name
	unsigned char addr; //7-bit addr
	unsigned char found; //flag
};

#if CONFIG_BCM_NS2_SVK_XMC
static struct ns2_i2c_map bsc1_map_default[] = {
	{ "PCA9554 U8", CONFIG_SYS_I2C_PCA9554_U8, 0 },
	{ "PCA9544 U7", I2C_IOMUX_ADDR_U7, 0 },
};

static struct ns2_i2c_map bsc1_map_mux_exp[] = {
	{ "QSFP I2C", I2C_SFP_ADDR, 0 },
};
#define I2C_DIAG_BSC1_MUX_SFP 0x4
#else /* config for SVK diags */
static struct ns2_i2c_map bsc0_map_default[] = {
	{ "PCF8574 U86", CONFIG_SYS_I2C_PCF8574_U86, 0 },
	{ "PCF8574 U92", CONFIG_SYS_I2C_PCF8574_U92, 0 },
	{ "PCF8574 U100", CONFIG_SYS_I2C_PCF8574_U100, 0 },
	{ "PCF9505 U41", CONFIG_SYS_I2C_PCF9505_U41, 0 },
	{ "PCF9505 U40", CONFIG_SYS_I2C_PCF9505_U40, 0 },
	{ "PCA9544 U68", I2C_IOMUX_ADDR_U68, 0 },
};

static struct ns2_i2c_map bsc0_map_mux_exp[] = {
	{ "SFP I2C", I2C_SFP_ADDR, 0 },
};

static struct ns2_i2c_map bsc1_map_default[] = {
	{ "PCA9544 U73", I2C_IOMUX_ADDR_U73, 0 },
};

static struct ns2_i2c_map bsc1_map_mux_exp[] = {
	{ "SFP CNTL U74", I2C_SFP_CNTL_U74, 0 },
};
#define I2C_DIAG_BSC1_MUX_SFP 0x7
#endif /* CONFIG_BCM_NS2_SVK_XMC */

static int i2c_post_probe_addr (struct ns2_i2c_map *map, int count)
{
	unsigned int i;
	int ret  = 0;

	for(i = 0; i < count; i++)	{
		if (i2c_probe(map[i].addr) == 0)	{
			map[i].found = 1;
			post_log ("I2C slave: %s, addr:0x%02x responded\n", map[i].name, map[i].addr);
		} else {
			map[i].found = 0;
			post_log ("I2C slave: %s, addr:0x%02x did not respond!\n", map[i].name, map[i].addr);
			ret = -1;
		}
	}

	return ret;
}

static void help(void)
{
	post_log("\n --------------------\n");
	post_log("| I2C DIAG HELP MENU |\n");
	post_log(" --------------------\n");
}

int I2C_post_test (int flags)
{
	unsigned int ret  = 0;
#if CONFIG_TARGET_NS2_SVK
	u8 data[2];
#endif

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	post_log("\n****BSC1 - I2C devices test****\n");
	i2c_set_bus_num(1);

	if(i2c_post_probe_addr(&bsc1_map_default[0], ARRAY_SIZE(bsc1_map_default)) == -1)
		ret =  -1;

	post_log("Selecting MUX channel for SFP CNTL\n");
	/* SVK=0x7, XMC=0x4 */
	i2c_write_mux_ctrl_byte(0x70, I2C_DIAG_BSC1_MUX_SFP);
	udelay(10000);
	if(i2c_post_probe_addr(&bsc1_map_mux_exp[0], ARRAY_SIZE(bsc1_map_mux_exp)) == -1)
		ret =  -1;

#ifndef CONFIG_BCM_NS2_SVK_XMC
	/* programming SFP CNTL */
	data[0] = 0x00;
	data[1] = 0x00;
	i2c_write_ctrl_bytes(0x24, data, 2);

	post_log("\n****BSC0 - I2C devices test****\n");
	i2c_set_bus_num(0);

	if(i2c_post_probe_addr(&bsc0_map_default[0], ARRAY_SIZE(bsc0_map_default)) == -1)
		ret =  -1;

	/* Programming MUX */
	i2c_write_mux_ctrl_byte (0x70, 0x0);

	post_log("Selecting MUX channel 3 for WAN SFP+3\n");
	i2c_write_mux_ctrl_byte (0x70, 0x7);
	udelay(10000);
	if(i2c_post_probe_addr(&bsc0_map_mux_exp[0], ARRAY_SIZE(bsc0_map_mux_exp)) == -1)
		ret =  -1;

	i2c_write_mux_ctrl_byte (0x70, 0x0);

	post_log("Selecting MUX channel 2 for WAN SFP+2\n");
	i2c_write_mux_ctrl_byte (0x70, 0x6);
	udelay(10000);
	if(i2c_post_probe_addr(&bsc0_map_mux_exp[0], ARRAY_SIZE(bsc0_map_mux_exp)) == -1)
		ret =  -1;

	i2c_write_mux_ctrl_byte (0x70, 0x0);

	post_log("Selecting MUX channel 1 for WAN SFP+1\n");
	i2c_write_mux_ctrl_byte (0x70, 0x5);
	udelay(10000);
	if(i2c_post_probe_addr(&bsc0_map_mux_exp[0], ARRAY_SIZE(bsc0_map_mux_exp)) == -1)
		ret =  -1;

	i2c_write_mux_ctrl_byte (0x70, 0x0);

	post_log("Selecting MUX channel 0 for WAN SFP+0\n");
	i2c_write_mux_ctrl_byte (0x70, 0x4);
	udelay(10000);
	if(i2c_post_probe_addr(&bsc0_map_mux_exp[0], ARRAY_SIZE(bsc0_map_mux_exp)) == -1)
		ret =  -1;
#endif /* CONFIG_BCM_NS2_SVK_XMC */

	if (ret == 0)
		post_log("\nI2C Diagnostics Passed\n");
	else
		post_log("\nI2C Diagnostics Failed\n");

	return ret;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_I2C */
