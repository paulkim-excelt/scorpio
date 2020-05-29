/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 */

#include <post.h>
#include <net.h>
#if CONFIG_POST & CONFIG_SYS_POST_GMAC
#include "bcmiproc_egphy28.h"


#define MAX_PACKET_LENGTH	1500
#define TEST_PACKET_LENGTH	500

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

	for (i = 6; i < length; i++) {
		if (packet[i] != (c++ % 256))
			return -1;
	}
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
	egphy28_set_internal_phy_loopback(&g_eth_data, 0,
					  EGPHY28_PHYID, TRUE);
}

void gmac_disable_int_phy_loopback(void)
{
	egphy28_set_internal_phy_loopback(&g_eth_data, 0,
					  EGPHY28_PHYID, FALSE);
}

void gmac_enable_ext_phy_loopback(void)
{
	egphy28_set_external_phy_loopback(&g_eth_data, 1,
					  EGPHY28_PHYID, TRUE);
}

void gmac_disable_ext_phy_loopback(void)
{
	egphy28_set_external_phy_loopback(&g_eth_data, 1,
					  EGPHY28_PHYID, FALSE);
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
	egphy28_ge_reset(&g_eth_data, EGPHY28_PHYID);
	udelay(5000);

	eth_init();

	gmac_enable_int_phy_loopback();

	post_log("Sending packet to ethernet!\n");
	eth_send(packet_send, TEST_PACKET_LENGTH);

	udelay(10000);
	egphy28_rd_reg(&g_eth_data, EGPHY28_PHYID, 0, 0, 0x1, &val);
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
	egphy28_ge_reset(&g_eth_data, EGPHY28_PHYID);
	udelay(5000);

	eth_init();

	gmac_enable_ext_phy_loopback();

	post_log("Sending packet to ethernet!\n");
	eth_send(packet_send, TEST_PACKET_LENGTH);

	udelay(10000);
	egphy28_rd_reg(&g_eth_data, EGPHY28_PHYID, 0, 0, 0x1, &val);
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

int GMAC_post_test(int flags)
{
	int ret;

	post_log("GMAC diagnostic test\n");

	/* Internal PHY loopback */
	post_log("\nGMAC PHY Internal Loopback test.\n");

	ret = gmac_int_loopback_test();
	if (ret == -1)
		post_log("\nGMAC PHY Internal Loopback test failed.\n");
	else
		post_log("\nGMAC PHY Internal Loopback test passed.\n");

	/* External PHY loopback */
	post_log("\nGMAC PHY External Loopback test.\n");

	ret = gmac_ext_loopback_test();
	if (ret == -1)
		post_log("\nGMAC PHY External Loopback test failed.\n");
	else
		post_log("\nGMAC PHY External Loopback test passed.\n");

	return ret;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_GMAC */
