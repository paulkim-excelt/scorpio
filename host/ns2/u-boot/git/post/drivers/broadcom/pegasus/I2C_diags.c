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
extern int i2c_write_mux_ctrl_byte(u8 devaddr, u8 ctrl_byte);
extern int i2c_write_ctrl_bytes(u8 devaddr, u8 *data, u8 size);
/*extern int i2c_read_mux_ctrl_byte(u8 devaddr);*/

/* Pegasus I2C Map */
struct pegasus_i2c_map {
	char name[30];	/*Slave name*/
	unsigned char addr; /*7-bit addr*/
	unsigned char found; /*flag*/
};

static struct pegasus_i2c_map bsc0_map_default[] = {
	{ "PCA9544APW U10", CONFIG_SYS_I2C_PCA9544APW_U10, 0 },
};

static struct pegasus_i2c_map bsc1_map_default[] = {
	{ "PCA9544APW U13", CONFIG_SYS_I2C_PCA9544APW_U13, 0 },
	{ "PCA9555APW U15", CONFIG_SYS_I2C_PCA9555APW_U15, 0 },
};
static struct pegasus_i2c_map bsc1_map_17mm_svk[] = {
	{ "PCA9555APW U15", CONFIG_SYS_I2C_PCA9555APW_U15, 0 },
};

static struct pegasus_i2c_map bsc2_map_default[] = {
	{ "PCA9544APW U11", CONFIG_SYS_I2C_PCA9544APW_U11, 0 },
	{ "PCA9555APW U34", CONFIG_SYS_I2C_PCA9555APW_U34, 0 },
};

static struct pegasus_i2c_map bsc2_map_mux_exp[] = {
	{ "SFP I2C", I2C_SFP_ADDR, 0 },
};

static struct pegasus_i2c_map bsc3_map_default[] = {
	{ "PCA9544APW U35", CONFIG_SYS_I2C_PCA9544APW_U35, 0 },
	{ "PCA9555APW U47", CONFIG_SYS_I2C_PCA9555APW_U47, 0 },
};

static int i2c_post_probe_addr(struct pegasus_i2c_map *map, int count)
{
	unsigned int i;
	int ret  = 0;

	for (i = 0; i < count; i++)	{
		if (i2c_probe(map[i].addr) == 0)	{
			map[i].found = 1;
			post_log("I2C slave:%s,addr:0x%02x responded\n",
				 map[i].name, map[i].addr);
		} else {
			map[i].found = 0;
			post_log("I2C slave:%s,addr:0x%02x did not respond!\n",
				 map[i].name, map[i].addr);
			ret = -1;
		}
	}

	return ret;
}

int I2C_post_test(int flags)
{
	unsigned int ret  = 0;
	char *s = getenv("board_name");

	if (s) {
		if (!strcmp(s, PEGASUS_XMC_BOARD)) {
			post_log("I2C interface is not available on XMC\n");
			return BCM_NO_IP;
		}
	} else	{
		post_log("Unable to get board name\n");
		ret = -1;
		return ret;
	}

	post_log("\n****BSC3 - I2C devices test****\n");
	i2c_set_bus_num(3);
	if (!strcmp(s, PEGASUS_23MM_BOARD)) {
		if (i2c_post_probe_addr(&bsc3_map_default[0],
				ARRAY_SIZE(bsc3_map_default)) == -1)
			ret =  -1;
	}

	post_log("\n****BSC2 - I2C devices test****\n");
	i2c_set_bus_num(2);

	if (i2c_post_probe_addr(&bsc2_map_default[0],
				ARRAY_SIZE(bsc2_map_default)) == -1)
		ret =  -1;

	/* Programming MUX */
	i2c_write_mux_ctrl_byte(0x70, 0x0);

	if (!strcmp(s, PEGASUS_23MM_BOARD)) {
		post_log("Selecting MUX channel 3 for WAN SFP+3\n");
		i2c_write_mux_ctrl_byte(0x70, 0x7);
		udelay(10000);
		if (i2c_post_probe_addr(&bsc2_map_mux_exp[0],
					ARRAY_SIZE(bsc2_map_mux_exp)) == -1)
			ret =  -1;

		i2c_write_mux_ctrl_byte(0x70, 0x0);

		post_log("Selecting MUX channel 2 for WAN SFP+2\n");
		i2c_write_mux_ctrl_byte(0x70, 0x6);
		udelay(10000);
		if (i2c_post_probe_addr(&bsc2_map_mux_exp[0],
					ARRAY_SIZE(bsc2_map_mux_exp)) == -1)
			ret =  -1;
	}

	i2c_write_mux_ctrl_byte(0x70, 0x0);

	post_log("Selecting MUX channel 1 for WAN SFP+1\n");
	i2c_write_mux_ctrl_byte(0x70, 0x5);
	udelay(10000);
	if (i2c_post_probe_addr(&bsc2_map_mux_exp[0],
				ARRAY_SIZE(bsc2_map_mux_exp)) == -1)
		ret =  -1;

	i2c_write_mux_ctrl_byte(0x70, 0x0);

	post_log("Selecting MUX channel 0 for WAN SFP+0\n");
	i2c_write_mux_ctrl_byte(0x70, 0x4);
	udelay(10000);

	if (i2c_post_probe_addr(&bsc2_map_mux_exp[0],
				ARRAY_SIZE(bsc2_map_mux_exp)) == -1)
		ret =  -1;

	post_log("\n****BSC1 - I2C devices test****\n");
	i2c_set_bus_num(1);
	if (!strcmp(s, PEGASUS_17MM_BOARD)) {
		if (i2c_post_probe_addr(&bsc1_map_17mm_svk[0],
				ARRAY_SIZE(bsc1_map_17mm_svk)) == -1)
			ret =  -1;
	}

	else {
		if (i2c_post_probe_addr(&bsc1_map_default[0],
				ARRAY_SIZE(bsc1_map_default)) == -1)
			ret =  -1;
	}

	post_log("\n****BSC0 - I2C devices test****\n");
	i2c_set_bus_num(0);
	if (!strcmp(s, PEGASUS_23MM_BOARD)) {
		if (i2c_post_probe_addr(&bsc0_map_default[0],
				ARRAY_SIZE(bsc0_map_default)) == -1)
			ret =  -1;
	}

	if (ret == 0)
		post_log("\nI2C Diagnostics Passed\n");
	else
		post_log("\nI2C Diagnostics Failed\n");

	return ret;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_I2C */
