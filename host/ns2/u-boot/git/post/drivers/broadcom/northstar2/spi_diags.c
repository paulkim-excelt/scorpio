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
#include <malloc.h>
#include <spi.h>

#if CONFIG_POST & CONFIG_SYS_POST_BCM_SPI

#define MAX_SPI_BYTES 32
extern uchar dout[MAX_SPI_BYTES];
extern uchar din[MAX_SPI_BYTES];

#define TEST_BYTES 8

/*
 * First byte is the command,
 * bytes 2 and 3 are offsets
 * */
#define COMMAND_OFFSET 3

static void help(void)
{
	post_log("\n --------------------\n");
	post_log("| SPI DIAG HELP MENU |\n");
	post_log(" --------------------\n");
}

int SPI_post_test(int flags)
{
	int ret = 0;
	int count = 2;
	int i = 0;
	char *test_argv[4];
	char *temp_buff[8];

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	#if DEBUG
	int j = 0;
	#endif

	printf("SPI Diags start\n");
	printf("Please ensure SLIC DC is not installed, this diag version supports EEPROM tests!\n");

	for (i = 0; i <= 3; i++) {
		test_argv[i] = malloc(30);
		if (test_argv[i] == NULL) {
			printf("malloc error\n");
			return -1;
		}
	}


	for (i = 0; i < count; i++) {
		printf("Write Enable for SPI:%d\n", i);
		strcpy(test_argv[0], "sspi");
		if (i == 0)
			strcpy(test_argv[1], "0:1.3");
		else if (i == 1)
			strcpy(test_argv[1], "1:1.3");
		strcpy(test_argv[2], "8");
		strcpy(test_argv[3], "06");

		do_spi(0, 0, 4, test_argv);

		printf("Write Data for SPI:%d\n", i);
		strcpy(test_argv[2], "88");
		strcpy(test_argv[3], "0200001234567888667788");
		do_spi(0, 0, 4, test_argv);

#if DEBUG
		printf("Sent data:\n");
		for (j = 0; j < TEST_BYTES; j++)
			printf("%d\n", *(dout + j + COMMAND_OFFSET));
#endif

		memcpy(temp_buff, dout + COMMAND_OFFSET, TEST_BYTES);
		mdelay(500);

		printf("Read Data for SPI:%d\n", i);
		strcpy(test_argv[2], "88");
		strcpy(test_argv[3], "030000");
		do_spi(0, 0, 4, test_argv);

#if DEBUG
		printf("Recevied data:\n");
		for (j = 0; j < TEST_BYTES; j++)
			printf("%d\n", *(din + j + COMMAND_OFFSET));
#endif
		udelay(10);

		if (memcmp(temp_buff, din + COMMAND_OFFSET, TEST_BYTES)) {
			printf("Diag failed for SPI:%d\n", i);
			ret = -1;
		} else {
			printf("Data Compare : OK, SPI:%d\n", i);
		}
	}


	for (i = 0; i <= 3; i++)
		free(test_argv[i]);

	return ret;
}
#endif
