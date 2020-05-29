/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 *
 * This file contains routines to perform diags
 * for SPI interface.
 *
 */

#include <common.h>
#include <post.h>
#include <malloc.h>
#include <dm.h>
#include <errno.h>
#include <spi.h>
#include <pl022_spi.h>

#if CONFIG_POST & CONFIG_SYS_POST_BCM_SPI

#define MAX_SPI_BYTES	(16 + 4) /* Maximum number of bytes we can handle */
#define CMD_READSTAT	0x5
#define CMD_WEN		0x6
#define CMD_WRITE	0x2
#define CMD_READ	0x3
#define CMD_CHIPERASE	0xc7
#define CMD_READID	0x9f

/*
 * Values from last command.
 */
static unsigned int mode;
static int bitlen;
static uchar dout[MAX_SPI_BYTES];
static uchar din[MAX_SPI_BYTES];

static int do_spi_xfer(int bus, int cs)
{
	struct spi_slave *slave;
	int ret = 0;

	slave = pl022_spi_setup_slave(bus, cs, 1000000, mode);
	if (!slave) {
		printf("Invalid device %d:%d\n", bus, cs);
		return -EINVAL;
	}

	ret = pl022_spi_claim_bus(slave);
	if (ret)
		goto done;

	ret = pl022_spi_xfer(slave, bitlen, dout, din,
			     SPI_XFER_BEGIN | SPI_XFER_END);
	if (ret)
		printf("Error %d during SPI transaction\n", ret);

done:
	pl022_spi_release_bus(slave);
	pl022_spi_free_slave(slave);
	return ret;
}

char read_status(int bus, int cs)
{
	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_READSTAT;
	bitlen = 8 * 2;

	do_spi_xfer(bus, cs);

	return din[1];
}

void read_id(int bus, int cs)
{
	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_READID;
	bitlen = 8 * 4;

	do_spi_xfer(bus, cs);
	printf("%d:%d: Slave chip id:0x%x-0x%x-0x%x\n",
	       bus, cs, din[1], din[2], din[3]);
}

void send_wle(int bus, int cs)
{
	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_WEN;
	bitlen = 8 * 1;

	printf("Write Enable for SPI:%d:%d\n", bus, cs);
	do_spi_xfer(bus, cs);
}

int erase_chip(int bus, int cs)
{
	int timeout = 30000;
	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_CHIPERASE;

	bitlen = 8 * 1;

	do_spi_xfer(bus, cs);

	while (read_status(bus, cs) & (1 << 0)) {
		if (!timeout--)
			break;
		udelay(100);
	}

	printf("%d:%d: Chip erase %s\n", bus, cs,
	       (timeout <= 0) ? "failed" : "successful");

	if (timeout > 0)
		return 0;
	else
		return 1;
}

void send_test_pattern(int bus, int cs, uint16_t address)
{
	int i;

	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_WRITE;
	dout[1] = 0x00;
	dout[2] = (address >> 8) & 0xFF;
	dout[3] = address & 0xFF;

	for (i = 4; i < MAX_SPI_BYTES; i++)
		dout[i] = i - 4;

	bitlen = 8 * MAX_SPI_BYTES;

	printf("%d:%d: Write@%0x: ", bus, cs, address);
	for (i = 4; i < MAX_SPI_BYTES; i++)
		printf(" 0x%x", dout[i]);
	printf("\n");

	do_spi_xfer(bus, cs);
}

int recv_test_pattern(int bus, int cs, uint16_t address)
{
	int i, pass;

	memset(dout, 0 , sizeof(dout));
	memset(din, 0, sizeof(din));

	mode = 3;
	dout[0] = CMD_READ;
	dout[1] = 0x00;
	dout[2] = (address >> 8) & 0xFF;
	dout[3] = address & 0xFF;
	bitlen = 8 * MAX_SPI_BYTES;

	do_spi_xfer(bus, cs);
	pass = 1;
	printf("%d:%d: Read@%0x: ", bus, cs, address);
	for (i = 4; i < MAX_SPI_BYTES; i++) {
		printf(" 0x%x", din[i]);
		if (din[i] != (i - 4))
			pass = 0;
	}
	printf("\n");

	return pass;
}

int SPI_post_test(int flags)
{
	int ret = 0;
	int i = 0;
	int count;
	int result = 0;
	int chip_select = 1;
	uint16_t address = 0x00;
	char *s = getenv("board_name");

	if (s) {
		if (!strcmp(s, PEGASUS_XMC_BOARD))
			count = 1;
		else
			count = 2;
	} else {
		post_log("Unable to get board name\n");
		ret = -1;
		return ret;
	}

	printf("SPI Diags start\n");
	printf("This diag version supports EEPROM tests!\n");

	for (i = 0; i < count; i++) {
		printf("===========================================\n");
		printf("\t\tSPI %d:%d TEST\n", i, chip_select);

		read_id(i, chip_select);
		send_wle(i , chip_select);
		if (erase_chip(i, chip_select))
			continue;

		send_wle(i, 1);
		printf("%d:%d: Slave chip status:0x%x\n", i, chip_select,
		       read_status(i, chip_select));
		send_test_pattern(i, chip_select, address);
		udelay(10000);

		ret = recv_test_pattern(i, chip_select, address);
		printf("SPI %d:%d Test %s.\n", i, chip_select,
		       ret ? "Passed" : "Failed");
		if (!result && !ret)
			result = 1;
		printf("===========================================\n");
		udelay(10);
	}

	return result;
}
#endif
