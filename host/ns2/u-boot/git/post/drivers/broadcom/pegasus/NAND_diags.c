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
#include <asm/arch/bcm_otp.h>
#include <asm/arch/socregs.h>
#include <asm/arch/bcm_secure_reg_ops.h>

#if CONFIG_POST & CONFIG_SYS_POST_NAND
#define NAND_READ_BUF_ADDR   0x81000000ULL
#define NAND_WRITE_BUF_ADDR  0x81200000ULL
#define NAND_DIAG_TEST_OFFS	 0x03f00000
#define NAND_DIAG_TEST_SIZE	 0x100000

#define NAND_WRITE_ADDR_STR  "81000000"
#define NAND_READ_ADDR_STR   "81200000"
#define NAND_DIAG_TEST_OFFS_STR	 "3f00000"
#define NAND_DIAG_TEST_SIZE_STR	 "100000"

#define NAND_EMMC_IOMUX_MASK 0xc00
#define NAND_EMMC_IOMUX_SHIFT 0xa

extern int do_nand(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);

int NAND_post_test(int flags)
{
	char argv_buf[7 * 10];
	char *cmd_argv[7];
	int i, rc = 0;
	u32 rd, wr;
	u32 sku_id = 0;
	int ret = 0;
	uint reg, iomux_status = 0;

#ifdef BCM_OTP_CHECK_ENABLED
	/* Checking sku id with OTP read */
	ret = otp_read_sku_id(&sku_id);
	if (ret != 0) {
		post_log("\nOTP read failed !! Exiting from this Diag !!\n");
		return BCM_NO_IP;
	}

	/* check if sku is 17x17 */
	if ((sku_id == 0x1) || (sku_id == 0x2) || (sku_id == 0x4)) {
		/* Checking IP iomuxing status with eMMC */
		reg = smc_mem_read32(ICFG_IOMUX_CTRL_REG);
		iomux_status = (reg & NAND_EMMC_IOMUX_MASK) >>
			NAND_EMMC_IOMUX_SHIFT;
		if (iomux_status == 0x1 || iomux_status == 0x2) {
			post_log("\nIP is not iomuxed to NAND !! Exiting!!\n");
			return BCM_NO_IP;
		}
	}
#endif

	memset(&argv_buf, 0, 7 * 10);
	for (i = 0; i < 7; i++)
		cmd_argv[i] = &argv_buf[i * 10];

	/* NAND Info */
	printf("\n\nNAND Info\n");
	strcpy(cmd_argv[0], "nand");
	strcpy(cmd_argv[1], "info");
	do_nand(0, 0, 2, cmd_argv);
	udelay(200 * 1000);

	/* Memory Fill */
	post_log("\n\nFill memory with incremental data\n");
	for (i = 0; i < NAND_DIAG_TEST_SIZE / 4; i++)
		*(u32 *)(NAND_WRITE_BUF_ADDR + i * 4) = i;

	/* NAND erase */
	post_log("\n\nNAND Erase, 1Mbyte\n");
	post_log("NAND erase: from NAND offset=0x3f00000 cnt=1MByte\n");
	strcpy(cmd_argv[0], "nand");
	strcpy(cmd_argv[1], "erase");
	strcpy(cmd_argv[2], NAND_DIAG_TEST_OFFS_STR);
	strcpy(cmd_argv[3], NAND_DIAG_TEST_SIZE_STR);
	rc = do_nand(0, 0, 4, cmd_argv);
	if (rc != 0)
		goto failed;

	/* NAND write */
	post_log("\n\nNAND data write, 1Mbyte\n");
	post_log
	    ("NAND write: from ddr_addr=0x%x to NAND offset=0x3f00000 cnt=1MByte\n",
	     NAND_WRITE_BUF_ADDR);
	strcpy(cmd_argv[0], "nand");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], NAND_WRITE_ADDR_STR);
	strcpy(cmd_argv[3], NAND_DIAG_TEST_OFFS_STR);
	strcpy(cmd_argv[4], NAND_DIAG_TEST_SIZE_STR);
	rc = do_nand(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed;

	/* NAND read */
	post_log("\n\nNAND data read, 1Mbyte\n");
	post_log
	    ("NAND read: from NAND offset=0x3f00000 to ddr_addr=0x%x cnt=1MByte\n",
	     NAND_READ_BUF_ADDR);
	strcpy(cmd_argv[0], "nand");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], NAND_READ_ADDR_STR);
	strcpy(cmd_argv[3], NAND_DIAG_TEST_OFFS_STR);
	strcpy(cmd_argv[4], NAND_DIAG_TEST_SIZE_STR);
	rc = do_nand(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed;

	/* NAND Verify */
	for (i = 0; i < NAND_DIAG_TEST_SIZE / 4; i++) {
		rd = *(u32 *)(NAND_READ_BUF_ADDR + i * 4);
		wr = *(u32 *)(NAND_WRITE_BUF_ADDR + i * 4);

		if (rd != wr) {
			post_log
			    ("\n !! Compare failed at address <0x%08x>, write=0x%08x, read=0x%08x\n",
			     NAND_READ_BUF_ADDR + i * 4, wr, rd);
			break;
		}
	}

	if (i == NAND_DIAG_TEST_SIZE / 4) {
		post_log("\n\nRead back and compare OK\n");
		post_log("\n\nNAND test passed\n");
		return 0;
	}

failed:
	post_log("\nNAND test failed");
	return -1;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_NAND */
