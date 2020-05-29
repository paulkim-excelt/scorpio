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
#include <usb/xhci-iproc.h>

#if CONFIG_POST & CONFIG_SYS_POST_USB30

/* Size in byte */
#define USB_DATA_BUF_SIZE	2*1024*1024
#define USB_DATA_BUF_ADDR   0x89000000

#define  XHCI_MSC_READ_BUF      (0x84200000)
#define  XHCI_MSC_WRITE_BUF     (0x84800000)

/* DISK write starting block address = 16K */
/* 16K * blk_size 512 = 8192K, so disk must be at least 10M */
#define  XHCI_DISK_WR_LBA       0x4000
#define  BLK_CNT_PER_XFER       32

static void help(void)
{
	post_log("\n -----------------------\n");
	post_log("| USB3.0 DIAG HELP MENU |\n");
	post_log(" -----------------------\n");
}

int USB30_post_test(int flags)
{
	int  i, j, rc = 0;
	int cnt, total;
	unsigned int *wr_ptr = (unsigned int *)XHCI_MSC_WRITE_BUF;
	unsigned int *rd_ptr = (unsigned int *)XHCI_MSC_READ_BUF;

#if CONFIG_TARGET_NS2_SVK == 1
	/* SVK supports 2 USB3.0 ports */
	total = 2;
#else
	/* XMC only supports 1 USB3.0 ports */
	total = 1;
#endif

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	for (cnt = 0; cnt < total; cnt++) {
		post_log("\nUSB 3.0 diags starts port %d...", cnt);
		/* Memory Fill */
		memset((void *)USB_DATA_BUF_ADDR, 0, USB_DATA_BUF_SIZE);

		/* Incremental data pattern for USB write buffer */
		for (i = 0; i < ((1024*1024)/4); i++)
			wr_ptr[i] = i;

		/* Enumerate USB3, retry three times if failed */
		for (i = 0; i < 3; i++) {
			xhci_hcd_init(cnt);
			rc = xhci_hcd_enum();
			if (rc) {
				post_log("\nLink connection failed,retry...\n");
			} else {
				post_log("\nLink setup OK. Enumeration done\n");
				break;
			}

			/* Memory Fill */
			memset((void *)USB_DATA_BUF_ADDR, 0, USB_DATA_BUF_SIZE);
			/* Incremental data pattern for USB write buffer */
			for (j = 0; j < ((1024*1024)/4); j++)
				wr_ptr[j] = j;
		}

		if (i == 3) {
			post_log("\nUSB3 device enumeration failed on Port %d",
				 cnt);
			xhci_core_phy_in_reset();
			return -1;
		}
		post_log("\n\nStart USB read/write test\n");

		/* Write 1M data, 16KB per command, 64x commands */
		post_log("\n\nWrite 1M data to disk, ");
		for (i = 0; i < 64; i++) {
			/* incr 32 blks per run */
			/* blk_cnt = 32 * 512 = 16k */
			/* incr 16k per run */
			xhci_write_test(XHCI_DISK_WR_LBA + i * BLK_CNT_PER_XFER,
					BLK_CNT_PER_XFER,
					XHCI_MSC_WRITE_BUF + i*16*1024);
			udelay(10000);
			post_log(".");
		}

		/* Read 1M data, 16KB per command, 64x commands */
		post_log("\n\nRead 1M data from disk, ");
		for (i = 0; i < 64; i++) {
			xhci_read_test(XHCI_DISK_WR_LBA + i * BLK_CNT_PER_XFER,
				       BLK_CNT_PER_XFER,
				       XHCI_MSC_READ_BUF + i*16*1024
				);
			udelay(10000);
			post_log(".");
		}

		post_log("\n\nVerify data : ");
		/* Incremental data pattern for USB write buffer */
		for (i = 0; i < ((1024*1024)/4); i++) {
			if (wr_ptr[i] != rd_ptr[i]) {
				post_log("Data err at 0x%x, wr=0x%x, rd=0x%x\n",
					 XHCI_MSC_WRITE_BUF + i*4,
					 wr_ptr[i],
					 rd_ptr[i]);
				return -1;
			}
			if ((i % 1024) == 0)
				post_log(".");
		}

		xhci_core_phy_in_reset();
		post_log("\n\nData verified OK on Port %d, test passed\n", cnt);
	}
	return 0;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_USB30 */
