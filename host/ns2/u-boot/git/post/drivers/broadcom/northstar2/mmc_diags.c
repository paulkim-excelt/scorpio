/*
 * Copyright 2016 Broadcom Limited.  All rights reserved.
 *
 * Unless you and Broadcom execute a separate written software license
 * agreement governing use of this software, this software is licensed to you
 * under the terms of the GNU General Public License version 2, available at
 * http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
 */

#include <common.h>
#include <post.h>
#include <mmc.h>

#if CONFIG_POST & CONFIG_SYS_POST_EMMC

#define SD_BLK_SIZE         512

/* Test memory */
#define MMC_DATA_SRC_BUF_ADDR_0      0x81500000ULL
#define MMC_DATA_SRC_BUF_ADDR_STR_0  "0x81500000"
#define MMC_DATA_DST_BUF_ADDR_0  0x81600000ULL
#define MMC_DATA_DST_BUF_ADDR_STR_0  "0x81600000"
#define MMC_DATA_SRC_BUF_ADDR_1      0x81700000ULL
#define MMC_DATA_SRC_BUF_ADDR_STR_1  "0x81700000"
#define MMC_DATA_DST_BUF_ADDR_1  0x81800000ULL
#define MMC_DATA_DST_BUF_ADDR_STR_1  "0x81800000"
#define TEST_BLK_CNT        1024	/* aligned to erase blocks size */
#define TEST_BLK_CNT_STR	"0x400"
#define ERASE_BLK_CNT_STR	"0x100"
#define MMC_DATA_BUF_SIZE	(SD_BLK_SIZE * TEST_BLK_CNT)	/* 512K */
#define SD_START_BLK        1024	/* Second erase block */
#define SD_START_BLK_STR	"0x400"
#define SD_START_BLK_STR_1	"0x500"
#define SD_START_BLK_STR_2	"0x600"
#define SD_START_BLK_STR_3	"0x700"

struct env_variables {
	int verbosity;
};

static struct env_variables env;

extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);

static void help(void)
{
	post_log("\n -------------------------\n");
	post_log("| MMC/SDIO DIAG HELP MENU |\n");
	post_log(" -------------------------\n");

	post_log("verbosity: show debug printouts\n"
		 "\tu-boot> setenv verbosity 1\n\n");
}

int MMC_post_test(int flags)
{
	struct mmc *mmc_0;
	struct mmc *mmc_1;

	static char argv_buf[7 * 15];
	static char *cmd_argv[7];
	int i, diag_ret = 0, ret_0 = 0, ret_1 = 0;
	char p0_type[] = "NONE",
		p1_type[] = "NONE",
		p0_result[] = " NA ",
		p1_result[] = " NA ";

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	if (getenv_yesno("verbosity") == 1)
		env.verbosity = true;
	else
		env.verbosity = false;

	memset(&argv_buf, 0, 7 * 15);
	for (i = 0; i < 7; i++)
		cmd_argv[i] = &argv_buf[i * 15];

	post_log("\nMMC device test for Port 0");
	post_log("\nPlease Ensure an SDIO or EMMC DC is ");
	post_log("installed on SDIO0 [J35 connector]\n\n");

	mdelay(2000);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "dev");
	strcpy(cmd_argv[2], "0");
	do_mmcops(0, 0, 3, cmd_argv);
	mdelay(1000);

	mmc_0 = find_mmc_device(0);
	if (!mmc_0) {
		post_log("\nMMC device not found on Port 0");
		goto failed_0;
	}

	if (IS_SD(mmc_0)) {
		p0_type[0] = 'S';
		p0_type[1] = 'D';
		p0_type[2] = 'I';
		p0_type[3] = 'O';
	} else {
		p0_type[0] = 'e';
		p0_type[1] = 'M';
		p0_type[2] = 'M';
		p0_type[3] = 'C';
	}

	debug_cond(env.verbosity, "\nMMC Info\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "info");
	do_mmcops(0, 0, 2, cmd_argv);
	mdelay(1000);

	/* Memory Fill */
	debug_cond(env.verbosity,
		   "Fill source memory with incremental data...");
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++)
		*(unsigned long *)(MMC_DATA_SRC_BUF_ADDR_0 + i * 8) = i;
	debug_cond(env.verbosity, " OK\n");

	/* Memory Clear */
	debug_cond(env.verbosity, "Clear destinaton memory...");
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++)
		*(unsigned long *)(MMC_DATA_DST_BUF_ADDR_0 + i * 8) = 0;
	debug_cond(env.verbosity, " OK\n");

	/* MMC erase */
	post_log("Erasing Test blocks on device...\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "erase");
	strcpy(cmd_argv[2], SD_START_BLK_STR);
	strcpy(cmd_argv[3], TEST_BLK_CNT_STR);
	ret_0 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_0 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_0 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_0 = do_mmcops(0, 0, 4, cmd_argv);
	if (ret_0 != 0)
		goto failed_0;

	mdelay(2000);

	/* MMC write */
	post_log("\nWrite data to device");
	debug_cond(env.verbosity,
		   "mmc write 0 ddr_addr=0x%x, blk_addr=%d cnt=%d\n",
		   MMC_DATA_SRC_BUF_ADDR_0, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], MMC_DATA_SRC_BUF_ADDR_STR_0);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret_0 = do_mmcops(0, 0, 5, cmd_argv);
	if (ret_0 != 0)
		goto failed_0;

	mdelay(2000);

	/* MMC read back */
	post_log("\nRead back from device and compare");
	debug_cond(env.verbosity,
		   "mmc read 0 ddr_addr=0x%x, blk_addr=%d cnt=%d\n",
		   MMC_DATA_DST_BUF_ADDR_0, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], MMC_DATA_DST_BUF_ADDR_STR_0);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret_0 = do_mmcops(0, 0, 5, cmd_argv);
	if (ret_0 != 0)
		goto failed_0;

	/* Compare result */
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++) {
		if (*(unsigned long *)(MMC_DATA_DST_BUF_ADDR_0 + i * 8) !=
		    *(unsigned long *)(MMC_DATA_SRC_BUF_ADDR_0 + i * 8)) {
			post_log("Compare failed at address <0x%lx> = 0x%lx\n",
				 MMC_DATA_DST_BUF_ADDR_0 + i * 8,
				 *(unsigned long *)
				 (MMC_DATA_DST_BUF_ADDR_0 + i * 8));
			break;
		}
	}

	if (i == MMC_DATA_BUF_SIZE / 8) {
		post_log("\nRead back and compare : OK\n");
		if (mmc_0->clock == 200000000) {
			post_log("Clock Check : OK\n");
		} else {
			post_log("Clock Check : FAILED\n");
			post_log("Expected 200000000 ; Got %d\n",
				 mmc_0->clock);
			goto failed_0;
		}
		if (IS_SD(mmc_0)) {
			if (mmc_0->bus_width == 4) {
				post_log("Bus Width Check : OK\n");
			} else {
				post_log("Bus Width Check : FAILED\n");
				post_log("Expected 4 ; Got %d\n",
					 mmc_0->bus_width);
				goto failed_0;
			}
		} else {
			if (mmc_0->bus_width == 8) {
				post_log("Bus Width Check : OK\n");
			} else {
				post_log("Bus Width Check : FAILED\n");
				post_log("Expected 8 ; Got %d\n",
					 mmc_0->bus_width);
				goto failed_0;
			}
		}
		debug_cond(env.verbosity,
			   "\nSDIO / eMMC device test on Port 0 Passed");
		p0_result[0] = 'P';
		p0_result[1] = 'A';
		p0_result[2] = 'S';
		p0_result[3] = 'S';
	} else {
failed_0:
		debug_cond(env.verbosity,
			   "\nSDIO / eMMC device test on Port 0 Failed");
		p0_result[0] = 'F';
		p0_result[1] = 'A';
		p0_result[2] = 'I';
		p0_result[3] = 'L';
		ret_0 = -1;
	}

	post_log("\nMMC device test for Port 1");
	post_log("\nPlease Ensure an SDIO or EMMC DC is ");
	post_log("installed on SDIO1[J36 connector]\n\n");

	mdelay(2000);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "dev");
	strcpy(cmd_argv[2], "1");
	do_mmcops(0, 0, 3, cmd_argv);
	mdelay(1000);

	mmc_1 = find_mmc_device(1);
	if (!mmc_1) {
		post_log("\nMMC device not found on Port 1");
		goto failed_1;
	}

	if (IS_SD(mmc_1)) {
		p1_type[0] = 'S';
		p1_type[1] = 'D';
		p1_type[2] = 'I';
		p1_type[3] = 'O';
	} else {
		p1_type[0] = 'e';
		p1_type[1] = 'M';
		p1_type[2] = 'M';
		p1_type[3] = 'C';
	}

	debug_cond(env.verbosity, "\nMMC Info\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "info");
	do_mmcops(0, 0, 2, cmd_argv);
	mdelay(1000);

	/* Memory Fill */
	debug_cond(env.verbosity, "Fill memory with incremental data...");
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++)
		*(unsigned long *)(MMC_DATA_SRC_BUF_ADDR_1 + i * 8) = i;
	debug_cond(env.verbosity, " OK\n");

	/* Memory Clear */
	debug_cond(env.verbosity, "\nClear destinaton memory...");
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++)
		*(unsigned long *)(MMC_DATA_DST_BUF_ADDR_1 + i * 8) = 0;
	debug_cond(env.verbosity, " OK\n");

	/* MMC erase */
	post_log("Erasing Test blocks on device...\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "erase");
	strcpy(cmd_argv[2], SD_START_BLK_STR);
	strcpy(cmd_argv[3], TEST_BLK_CNT_STR);
	ret_1 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_1 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_1 = do_mmcops(0, 0, 4, cmd_argv);
	mdelay(1000);
	ret_1 = do_mmcops(0, 0, 4, cmd_argv);
	if (ret_1 != 0)
		goto failed_1;

	mdelay(2000);

	/* MMC write */
	post_log("\nWrite data to device...");
	debug_cond(env.verbosity,
		   "mmc write 0 ddr_addr=0x%x,blk_addr=%d cnt=%d\n",
		   MMC_DATA_SRC_BUF_ADDR_1, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], MMC_DATA_SRC_BUF_ADDR_STR_1);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret_1 = do_mmcops(0, 0, 5, cmd_argv);
	if (ret_1 != 0)
		goto failed_1;

	mdelay(2000);

	/* MMC read back */
	post_log("\nRead back from device and compare...");
	debug_cond(env.verbosity,
		   "mmc read 0 ddr_addr=0x%x, blk_addr=%d cnt=%d\n",
		   MMC_DATA_DST_BUF_ADDR_1, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], MMC_DATA_DST_BUF_ADDR_STR_1);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret_1 = do_mmcops(0, 0, 5, cmd_argv);
	if (ret_1 != 0)
		goto failed_1;

	/* Compare result */
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++) {
		if (*(unsigned long *)(MMC_DATA_DST_BUF_ADDR_1 + i * 8) !=
		    *(unsigned long *)(MMC_DATA_SRC_BUF_ADDR_1 + i * 8)) {
			post_log("Compare failed at address <0x%lx> = 0x%lx\n",
				 MMC_DATA_DST_BUF_ADDR_1 + i * 8,
				 *(unsigned long *)
				 (MMC_DATA_DST_BUF_ADDR_1 + i * 8));
			break;
		}
	}

	if (i == MMC_DATA_BUF_SIZE / 8) {
		post_log("\nRead back and compare : OK\n");
		if (mmc_1->clock == 200000000) {
			post_log("Clock Check : OK\n");
		} else {
			post_log("Clock Check : FAILED\n");
			post_log("Expected 200000000 ; Got %d\n",
				 mmc_1->clock);
			goto failed_1;
		}
		if (IS_SD(mmc_1)) {
			if (mmc_1->bus_width == 4) {
				post_log("Bus Width Check : OK\n");
			} else {
				post_log("Bus Width Check : FAILED\n");
				post_log("Expected 4 ; Got %d\n",
					 mmc_1->bus_width);
				goto failed_1;
			}
		} else {
			if (mmc_1->bus_width == 8) {
				post_log("Bus Width Check : OK\n");
			} else {
				post_log("Bus Width Check : FAILED\n");
				post_log("Expected 8 ; Got %d\n",
					 mmc_1->bus_width);
				goto failed_1;
			}
		}
		debug_cond(env.verbosity,
			   "\nSDIO / eMMC device test on Port 1 Passed");
		p1_result[0] = 'P';
		p1_result[1] = 'A';
		p1_result[2] = 'S';
		p1_result[3] = 'S';
	} else {
failed_1:
		debug_cond(env.verbosity,
			   "\nSDIO / eMMC device test on Port 1 Failed");
		p1_result[0] = 'F';
		p1_result[1] = 'A';
		p1_result[2] = 'I';
		p1_result[3] = 'L';
		ret_1 = -1;
	}

	post_log("\nResults ...\n");
	post_log("\n|---------------------------------------------|\n");
	post_log("| Port  | Device | Clock     | Width | Result |\n");
	post_log("|---------------------------------------------|\n");
	if (mmc_0) {
		post_log("|   0   |  %04s  | %09d |  %db   |  %04s  |\n",
			 p0_type, mmc_0->clock, mmc_0->bus_width, p0_result);
		post_log("|---------------------------------------------|\n");
	}
	if (mmc_1) {
		post_log("|   1   |  %04s  | %09d |  %db   |  %04s  |\n",
			 p1_type, mmc_1->clock, mmc_1->bus_width, p1_result);
		post_log("|---------------------------------------------|\n\n");
	}

	if ((ret_0 == -1) || (ret_1 == -1))
		diag_ret = -1;

	return diag_ret;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_EMMC */
