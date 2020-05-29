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
#include <mmc.h>
#include <asm/arch/bcm_otp.h>
#include <asm/arch/socregs.h>
#include <asm/arch/bcm_secure_reg_ops.h>

#if CONFIG_POST & CONFIG_SYS_POST_EMMC

#define SD_BLK_SIZE         512

/* Test memory */
#define MMC_DATA_SRC_BUF_ADDR      0x80000000ULL
#define MMC_DATA_SRC_BUF_ADDR_STR  "0x80000000"
#define MMC_DATA_DST_BUF_ADDR	0x85000000ULL
#define MMC_DATA_DST_BUF_ADDR_STR  "0x85000000"
#define TEST_BLK_CNT        16	/*aligned to erase blocks size */
#define TEST_BLK_CNT_STR	"0x10"
#define MMC_DATA_BUF_SIZE	(SD_BLK_SIZE * TEST_BLK_CNT)	/*512K */
#define SD_START_BLK        0	/*Second erase block */
#define SD_START_BLK_STR	"0x00"

#define NAND_eMMC_IOMUX_MASK 0xc00
#define NAND_eMMC_IOMUX_SHIFT 0xa

extern int do_mmcops(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[]);

int MMC_post_test(int flags)
{
	static char argv_buf[7 * 15];
	static char *cmd_argv[7];
	int i, ret = 0;
	u32 sku_id = 0;
	uint reg, iomux_status = 0;
	char *s = getenv("board_name");

	if (s) {
		if (!strcmp(s, PEGASUS_XMC_BOARD)) {
			post_log("\neMMC interface is not available on XMC\n");
			return BCM_NO_IP;
		}
	}

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
		iomux_status = (reg & NAND_eMMC_IOMUX_MASK) >>
			NAND_eMMC_IOMUX_SHIFT;
		if (iomux_status != 0x2) {
			post_log("\nIP is not iomuxed to eMMC !! Exiting!!\n");
			return BCM_NO_IP;
		}
	}
#endif

	memset(&argv_buf, 0, 7 * 15);
	for (i = 0; i < 7; i++)
		cmd_argv[i] = &argv_buf[i * 15];

	post_log("\nMMC device test for SDIO1\n\n");
	post_log("\nPlease Ensure EMMC DC is installed on\
			SDIO1[J36 connector]\n\n");

	udelay(2000000);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "dev");
	strcpy(cmd_argv[2], "0");
	do_mmcops(0, 0, 3, cmd_argv);
	udelay(1000000);

	printf("\nMMC Info\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "info");
	do_mmcops(0, 0, 2, cmd_argv);
	udelay(1000000);

	/* Memory Fill */
	post_log("\nFill memory with incremental data\n");
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++)
		*(unsigned long *)(MMC_DATA_SRC_BUF_ADDR + i * 8) = i;

	/* MMC erase */
	post_log("\nErasing Test blocks on eMMC device\n");
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "erase");
	strcpy(cmd_argv[2], SD_START_BLK_STR);
	strcpy(cmd_argv[3], TEST_BLK_CNT_STR);
	ret = do_mmcops(0, 0, 4, cmd_argv);
	udelay(1000000);
	ret = do_mmcops(0, 0, 4, cmd_argv);
	udelay(1000000);
	ret = do_mmcops(0, 0, 4, cmd_argv);
	udelay(1000000);
	ret = do_mmcops(0, 0, 4, cmd_argv);
	udelay(1000000);
	ret = do_mmcops(0, 0, 4, cmd_argv);
	if (ret != 0)
		goto failed_0;

	udelay(2000000);

	/* MMC write */
	post_log("\nWrite data to eMMC device\n");
	post_log("mmc write 0 ddr_addr=0x%x,blk_addr=%d cnt=%d\n",
		 MMC_DATA_SRC_BUF_ADDR, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], MMC_DATA_SRC_BUF_ADDR_STR);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret = do_mmcops(0, 0, 5, cmd_argv);
	if (ret != 0)
		goto failed_0;

	udelay(2000000);

	/* MMC read back */
	post_log("\nRead back from eMMC device and compare\n");
	post_log("mmc read 0 ddr_addr=0x%x, blk_addr=%d cnt=%d\n",
		 MMC_DATA_DST_BUF_ADDR, SD_START_BLK, TEST_BLK_CNT);
	strcpy(cmd_argv[0], "mmc");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], MMC_DATA_DST_BUF_ADDR_STR);
	strcpy(cmd_argv[3], SD_START_BLK_STR);
	strcpy(cmd_argv[4], TEST_BLK_CNT_STR);
	ret = do_mmcops(0, 0, 5, cmd_argv);
	if (ret != 0)
		goto failed_0;

	/* Compare result */
	for (i = 0; i < MMC_DATA_BUF_SIZE / 8; i++) {
		if (*(unsigned long *)(MMC_DATA_DST_BUF_ADDR + i * 8) !=
		    *(unsigned long *)(MMC_DATA_SRC_BUF_ADDR + i * 8)) {
			post_log("\n!Compare failed at address<0x%lx>=0x%lx\n",
				 MMC_DATA_DST_BUF_ADDR + i * 8,
				 *(unsigned long *)
				 (MMC_DATA_DST_BUF_ADDR + i * 8));
			break;
		}
	}

	if (i == MMC_DATA_BUF_SIZE / 8) {
		post_log("Read back and compare OK\n");
		post_log("\neMMC device test on SDIO1 Passed\n");
	} else {
failed_0:
		post_log("\neMMC device test on SDIO1 Failed\n");
		ret = -1;
	}
	return ret;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_EMMC */
