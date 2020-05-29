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
#include <ahci.h>
#include <scsi.h>
#include <asm/arch-bcm_pegasus/bcm_otp.h>

#if CONFIG_POST & CONFIG_SYS_POST_SATA

#define SATA_READ_BUF_ADDR   0x81800000ULL
#define SATA_READ_BUF_STR   "81800000"
#define SATA_WRITE_BUF_ADDR  0x81900000ULL
#define SATA_WRITE_BUF_STR  "81900000"

#define SATA_TEST_BUF_SIZE	0x1000	/* 8 sectors */

int SATA_post_test(int flags)
{
	char argv_buf[7 * 10];
	char *cmd_argv[7];
	int i, rc = 0, ret = 0, ret_1 = 0;
	uint32_t rd, wr;
	char *s = getenv("board_name");

#define SATA_0_SATA_AHCI_GHC_HBA_CAP 0x50242000
#define SATA_1_SATA_AHCI_GHC_HBA_CAP 0x50262000

	post_log("\n");
	post_log
	    ("Make sure SATA disks connected to SATA ports\n");

	post_log("############ SATA_0 test ############\n");
#ifdef BCM_OTP_CHECK_ENABLED
	if (!is_sata_port_enabled(0)) {
		post_log("\nSATA Port_0 is Not enabled, Skipping...\n");
		goto port1;
	}
#endif
	ahci_init((void __iomem *)((uint64_t)SATA_0_SATA_AHCI_GHC_HBA_CAP));
	scsi_scan(1);

	memset(&argv_buf, 0, 7 * 10);
	for (i = 0; i < 7; i++)
		cmd_argv[i] = &argv_buf[i * 10];
#if 0
	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "info");
	do_scsi(0, 0, 2, cmd_argv);
	udelay(200 * 1000);

	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "scan");
	do_scsi(0, 0, 2, cmd_argv);
	udelay(200 * 1000);
#endif

	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "device");
	strcpy(cmd_argv[2], "0");
	rc = do_scsi(0, 0, 3, cmd_argv);
	if (rc != 0)
		goto failed0;

	udelay(200 * 1000);

	/* Memory Fill */
	post_log("\n\nFill memory with incremental data\n");
	for (i = 0; i < SATA_TEST_BUF_SIZE / 4; i++)
		*(u32 *)(SATA_WRITE_BUF_ADDR + i * 4) = i * 4;

	/* SCSI write */
	post_log("\n\nSATA write, 4k\n");
	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], SATA_WRITE_BUF_STR);
	strcpy(cmd_argv[3], "8");
	strcpy(cmd_argv[4], "8");
	rc = do_scsi(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed0;

	udelay(3000000);

	/* SCSI read */
	post_log("\n\nSATA read, 4k\n");
	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], SATA_READ_BUF_STR);
	strcpy(cmd_argv[3], "8");
	strcpy(cmd_argv[4], "8");
	rc = do_scsi(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed0;

	/* Verify */
	for (i = 0; i < SATA_TEST_BUF_SIZE / 4; i++) {
		rd = *(u32 *)(SATA_READ_BUF_ADDR + i * 4);
		wr = *(u32 *)(SATA_WRITE_BUF_ADDR + i * 4);

		if (rd != wr) {
			post_log("\n ERROR:\n");
			post_log("SATA_0: Compare Failed Address <0x%08x>\n",
					SATA_READ_BUF_ADDR + i * 4);
			post_log("write=0x%08x, read=0x%08x\n", wr, rd);
			break;
		}
	}

	if (i == SATA_TEST_BUF_SIZE / 4) {
		post_log("Read back and compare OK\n");
		post_log("\n\nSATA disk 0 test ................passed\n");
	} else {
failed0:
		post_log("\n\nSATA disk 0 test ................failed\n");
		ret = -1;
	}

#ifdef BCM_OTP_CHECK_ENABLED
port1:
	if (!is_sata_port_enabled(1)) {
		post_log("\nSATA Port_1 is Not enabled\n");
		return BCM_NO_IP;
	}
#endif
	/* Check for XMC and 17x17 boards. They have only one SATA port */
	if (s) {
		if ((!strcmp(s, PEGASUS_XMC_BOARD)) ||
				(!strcmp(s, PEGASUS_17MM_BOARD)))
			return rc;
	}

	post_log("############ SATA_1 test ############ \n");
	ahci_init((void __iomem *)((uint64_t)SATA_1_SATA_AHCI_GHC_HBA_CAP));
	scsi_scan(1);

	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "device");
	strcpy(cmd_argv[2], "0");
	rc = do_scsi(0, 0, 3, cmd_argv);
	if (rc != 0)
		goto failed1;

	udelay(200 * 1000);

	/* Memory Fill */
	post_log("\n\nFill memory with incremental data\n");
	for (i = 0; i < SATA_TEST_BUF_SIZE / 4; i++)
		*(u32 *)(SATA_WRITE_BUF_ADDR + i * 4) = i * 8;

	/* SCSI write */
	post_log("\n\nSATA write, 4k\n");
	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "write");
	strcpy(cmd_argv[2], SATA_WRITE_BUF_STR);
	strcpy(cmd_argv[3], "8");
	strcpy(cmd_argv[4], "8");
	rc = do_scsi(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed1;

	udelay(3000000);

	/* SCSI read */
	post_log("\n\nSATA read, 4k\n");
	strcpy(cmd_argv[0], "scsi");
	strcpy(cmd_argv[1], "read");
	strcpy(cmd_argv[2], SATA_READ_BUF_STR);
	strcpy(cmd_argv[3], "8");
	strcpy(cmd_argv[4], "8");
	rc = do_scsi(0, 0, 5, cmd_argv);
	if (rc != 0)
		goto failed1;

	/* Verify */
	for (i = 0; i < SATA_TEST_BUF_SIZE / 4; i++) {
		rd = *(u32 *)(SATA_READ_BUF_ADDR + i * 4);
		wr = *(u32 *)(SATA_WRITE_BUF_ADDR + i * 4);

		if (rd != wr) {
			post_log("\n ERROR:\n");
			post_log("SATA_1: Compare Failed Address <0x%08x>\n",
					SATA_READ_BUF_ADDR + i * 4);
			post_log("write=0x%08x, read=0x%08x\n", wr, rd);
			break;
		}
	}

	if (i == SATA_TEST_BUF_SIZE / 4) {
		post_log("Read back and compare OK\n");
		post_log("\n\nSATA disk 1 test ................passed\n");
		/* Auto mode, both disk should detect, else diag failure */
		if (!(flags & POST_AUTO))
			ret_1 = 0;
	} else {
failed1:
		post_log("\n\nSATA disk 1 test ................failed\n");
		ret_1 = -1;
	}

	if (ret)
		post_log("\n\n SATA port_0 test FAILED ......\n");

	if (ret_1) {
		post_log("\n\n SATA port_1 test FAILED ......\n");
		ret = ret_1;
	}

	return ret;
}
#endif /* CONFIG_POST & CONFIG_SYS_POST_SATA */
