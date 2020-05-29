/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * This file contains routines to perform diags
 * for GMAC.
 *
 */

#include <common.h>
#include <linux/compiler.h>
#include <post.h>
#include <net.h>
#include <asm/arch/ethHw_dma.h>
#include <asm/arch/socregs.h>

#include <asm/arch/bcmenetphy.h>
#include <asm/arch/iproc_regs.h>
#include <asm/arch/iproc_gmac_regs.h>
#include <asm/arch/reg_utils.h>
#include <asm/arch/ethHw.h>
#include <asm/arch/ethHw_dma.h>
#include <asm/arch/bcmgmacmib.h>
#include <asm/arch/bcmutils.h>
#include <bcmiproc_phy.h>
#include "ethHw_data.h"
#include "phy/bcm5481.h"

#if CONFIG_POST & CONFIG_SYS_POST_GMAC

DECLARE_GLOBAL_DATA_PTR;

#define MIN_PACKET_LENGTH	47
#define MAX_PACKET_LENGTH	1500
#define TEST_PACKET_LENGTH	500

#define PHY54810_ADDR (0x10)

struct env_variables {
	int verbosity;
};

static struct env_variables env;


static void packet_fill(char *packet, int length)
{
	char c = (char) length;
	int i;

	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFF;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = 0xFF;

	for (i = 6; i < length; i++)
		packet[i] = c++ % 256;
}

static int packet_check(char *packet, int length)
{
	char c = (char) length;
	int i;

	for (i = 6; i < length; i++)
		if (packet[i] != (c++ % 256))
			return -1;
	return 0;
}


void gmac_enable_rgmii_loopback(void)
{
	return;
}

void gmac_disable_rgmii_loopback(void)
{
	return;
}


void gmac_enable_gmac_loopback(void)
{
	gmac_loopback(&g_eth_data, TRUE);
	return;
}

void gmac_disable_gmac_loopback(void)
{
	gmac_loopback(&g_eth_data, FALSE);
	return;
}

void gmac_enable_int_phy_loopback(void)
{
	phy54810_set_internal_phy_loopback(&g_eth_data, 1,
					   PHY54810_ADDR, TRUE);
}

void gmac_disable_int_phy_loopback(void)
{
	phy54810_set_internal_phy_loopback(&g_eth_data, 1,
					   PHY54810_ADDR, FALSE);
}

void gmac_enable_ext_phy_loopback(void)
{
	phy54810_set_external_phy_loopback(&g_eth_data, 1,
					   PHY54810_ADDR, TRUE);
}

void gmac_disable_ext_phy_loopback(void)
{
	phy54810_set_external_phy_loopback(&g_eth_data, 1,
					   PHY54810_ADDR, FALSE);
}

static int gmac_int_loopback_test(void)
{
	int rcvlen = 0;
	uint8_t packet_send[MAX_PACKET_LENGTH];
	uint8_t packet_recv[MAX_PACKET_LENGTH];
	uint16_t val;

	post_log("Fill packet with length = %d\n", TEST_PACKET_LENGTH);
	packet_fill((char *)packet_send, TEST_PACKET_LENGTH);

	eth_halt();
	phy5481_ge_reset(&g_eth_data, PHY54810_ADDR);
	udelay(5000);

	eth_init();

	gmac_enable_int_phy_loopback();

	post_log("Sending packet to ethernet!\n");
	eth_send(packet_send, TEST_PACKET_LENGTH);

	udelay(10000);
	phy5481_rd_reg(&g_eth_data, PHY54810_ADDR, 0, 0, 0x1, &val);
	post_log("Value of status reg 0x01 after sending: %#x\n", val);


	post_log("Trying to receive a packet from ethernet!\n");
	rcvlen = eth_receive(packet_recv, TEST_PACKET_LENGTH);

	/*  recv packet */
	if (rcvlen != TEST_PACKET_LENGTH) {
		post_log("Received packet of length %d (expected %d) - FAIL\n",
			 rcvlen, TEST_PACKET_LENGTH);
		return -1;
	} else {
		if (packet_check((char *)packet_recv, rcvlen) < 0) {
			post_log("Received packet (length %d) compare - FAIL\n",
				 rcvlen);
			return -1;
		} else {
			post_log("Received packet (length %d) compare - OK\n",
				 rcvlen);
		}
	}

	gmac_disable_int_phy_loopback();

	eth_halt();
	return 0;
}

static int gmac_ext_loopback_test(void)
{
	int rcvlen = 0;
	uint8_t packet_send[MAX_PACKET_LENGTH];
	uint8_t packet_recv[MAX_PACKET_LENGTH];
	uint16_t val;

	post_log("Fill packet with length = %d\n", TEST_PACKET_LENGTH);
	packet_fill((char *)packet_send, TEST_PACKET_LENGTH);

	eth_halt();
	phy5481_ge_reset(&g_eth_data, PHY54810_ADDR);
	udelay(5000);

	eth_init();

	gmac_enable_ext_phy_loopback();

	post_log("Sending packet to ethernet!\n");
	eth_send(packet_send, TEST_PACKET_LENGTH);

	udelay(10000);
	phy5481_rd_reg(&g_eth_data, PHY54810_ADDR, 0, 0, 0x1, &val);
	post_log("Value of status reg 0x01 after sending: %#x\n", val);


	post_log("Trying to receive a packet from ethernet!\n");
	rcvlen = eth_receive(packet_recv, TEST_PACKET_LENGTH);

	/*  recv packet */
	if (rcvlen != TEST_PACKET_LENGTH) {
		post_log("Received packet of length %d (expected %d) - FAIL\n",
			 rcvlen, TEST_PACKET_LENGTH);
		return -1;
	} else {
		if (packet_check((char *)packet_recv, rcvlen) < 0) {
			post_log("Received packet (length %d) compare - FAIL\n",
				 rcvlen);
			return -1;
		} else {
			post_log("Received packet (length %d) compare - OK\n",
				 rcvlen);
		}
	}

	gmac_disable_ext_phy_loopback();

	eth_halt();
	return 0;
}

static void help(void)
{
	post_log("\n ---------------------------\n");
	post_log("| GMAC/RGMII DIAG HELP MENU |\n");
	post_log(" ---------------------------\n");

	post_log("verbosity: show debug printouts\n");
	post_log("\tu-boot> setenv verbosity 1\n\n");
}

int GMAC_post_test(int flags)
{
	int result, test = 0;
	int ret;

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	if (getenv_yesno("verbosity") == 1)
		env.verbosity = true;
	else
		env.verbosity = false;

	post_log("GMAC/RGMII diagnostic test\n");

	while (1) {
		post_log("TEST CHOICES:\n");
		post_log("1. Internal Loopback\n2. External Loopback\n");
		test = post_getUserInput("Test type? (1 or 2): ");
		if (test < 1 || test > 2)
			post_log("Error, out of range.\n");
		else
			break;
	}
	switch (test) {
	case 1:
		/* Internal PHY loopback */
		post_log("\nGMAC PHY Internal Loopback test.\n");

		ret = gmac_int_loopback_test();
		if (ret == -1)
			result = -1;
		else
			result = 0;
		break;
	case 2:
		/* External PHY loopback */
		post_log("\nGMAC PHY External Loopback test.\n");

		ret = gmac_ext_loopback_test();
		if (ret == -1)
			result = -1;
		else
			result = 0;
		break;
	}

	return result;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_GMAC */
