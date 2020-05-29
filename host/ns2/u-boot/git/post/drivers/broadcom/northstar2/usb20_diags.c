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

#if CONFIG_POST & CONFIG_SYS_POST_USB20

/* Size in byte */
#define USB_DATA_BUF_SIZE	(0x40 * 0x4)

#define USB_DATA_BUF_ADDR		0x80000000ULL
#define USB_DATA_BUF_ADDR_STR		"80000000"
#define USB_DST_DATA_BUF_ADDR		0x81000000ULL
#define USB_DST_DATA_BUF_ADDR_STR	"81000000"

extern int do_usb(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
/*
 * TODO If USB2_DIAGS_DEBUG macro is defined,
 * make sure do_mem_md(), common/cmd_mem.c file is *NOT* static
 */
#ifdef USB2_DIAGS_DEBUG
extern int do_mem_md(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);
#endif

/* Returns the number of the attached Devices */
static int usb_get_attach_dev_count(void)
{
	int devno;
	int count = 0;
#ifdef CONFIG_USB_STORAGE
	block_dev_desc_t *stor_dev;
#endif
	for (devno = 0; ; ++devno) {
		stor_dev = usb_stor_get_dev(devno);
		if (stor_dev == NULL)
			break;
		if (stor_dev->type != DEV_TYPE_UNKNOWN)
			count++;
	}
	printf("##########################################\n");
	printf("Number of USB devices attached are %d\n", count);
	printf("##########################################\n");
	return count;
}

static int usb_read_write(void)
{
	char argv_buf[7 * 10];
	char *usb_argv[7];
	int  i, rc = 0;

	memset(&argv_buf, 0, 7 * 10);

	for (i = 0; i < 7; i++)
		usb_argv[i] = &argv_buf[i * 10];

	/* USB read */
	post_log("\n\nUSB raw data read, 32K bytes\n");
	post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=64\n",
		 USB_DATA_BUF_ADDR);
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "read");
	strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR);
	strcpy(usb_argv[3], "8000");
	strcpy(usb_argv[4], "40");
	rc = do_usb(0, 0, 5, usb_argv);
	if (rc != 0)
		goto failed;

	/* Memory Fill */
	post_log("\n\nFill memory with incremental data, 32K bytes, addr=0x%x\n",
		 USB_DATA_BUF_ADDR);
	for (i = 0; i < (USB_DATA_BUF_SIZE / 4); i++)
		*(unsigned int *)(USB_DATA_BUF_ADDR + (i * 4)) = i;

#ifdef USB2_DIAGS_DEBUG
	/* Memory Display */
	post_log("\n\Addr 0x%x - Display filled memory\n", USB_DATA_BUF_ADDR);
	strcpy(usb_argv[0], "md");
	strcpy(usb_argv[1], USB_DATA_BUF_ADDR_STR);
	do_mem_md(0, 0, 2, usb_argv);
#endif

	/* USB write */
	post_log("\n\nUSB raw data write, 32K bytes\n");
	post_log("USB write ddr_addr=0x%x blk_addr=0x8000 cnt=64\n",
		 USB_DATA_BUF_ADDR);
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "write");
	strcpy(usb_argv[2], USB_DATA_BUF_ADDR_STR);
	strcpy(usb_argv[3], "8000");
	strcpy(usb_argv[4], "40");
	rc = do_usb(0, 0, 5, usb_argv);
	if (rc != 0)
		goto failed;

	/* USB read back and compare */
	post_log("\n\nUSB read back and compare\n");
	post_log("USB read ddr_addr=0x%x blk_addr=0x8000 cnt=64\n",
		 USB_DST_DATA_BUF_ADDR);
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "read");
	strcpy(usb_argv[2], USB_DST_DATA_BUF_ADDR_STR);
	strcpy(usb_argv[3], "8000");
	strcpy(usb_argv[4], "40");
	rc = do_usb(0, 0, 5, usb_argv);
	if (rc != 0)
		goto failed;

	for(i = 0; i < USB_DATA_BUF_SIZE / 4; i++) {
		if (*(unsigned int *)(USB_DST_DATA_BUF_ADDR + (i * 4)) != i) {
			post_log("\n !! Compare failed at address <0x%08x> = 0x%08x\n",
				 USB_DATA_BUF_ADDR + (i * 4),
				 *(unsigned int *)(USB_DST_DATA_BUF_ADDR + (i * 4)));
			break;
		}
	}

	if (i == (USB_DATA_BUF_SIZE / 4)) {
		post_log("\n\nRead back and compare OK\n");
		post_log("\n\nUSB test done\n");
	}
	return 0;
failed:
	post_log("\n\nUSB test failed\n");
	rc = -1;
	return rc;
}

static void help(void)
{
	post_log("\n -----------------------\n");
	post_log("| USB2.0 DIAG HELP MENU |\n");
	post_log(" -----------------------\n");
}

int USB20_post_test(int flags)
{
	char argv_buf[7 * 10];
	char *usb_argv[7];
	int  i, rc = 0;
	int num_devices;

	if (flags & POST_HELP) {
		help();
		return 0;
	}

	memset(&argv_buf, 0, 7 * 10);

	for (i = 0; i < 7; i++)
		usb_argv[i] = &argv_buf[i * 10];

	/* USB Reset */
	post_log("\n\nUSB reset - Reset/Enumerate USB device\n");
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "reset");
	rc = do_usb(0, 0, 2, usb_argv);
	if (rc != 0) {
		printf("**** ERROR ****: USB reset failed\n");
		return rc;
	}

	/* USB Tree */
	post_log("\n\nUSB tree - Display USB device\n");
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "tree");
	rc = do_usb(0, 0, 2, usb_argv);
	if (rc != 0) {
		printf("**** ERROR ****: USB tree failed\n");
		return rc;
	}

	/* USB Info */
	post_log("\n\nUSB info - Display USB device info\n");
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "info");
	rc = do_usb(0, 0, 2, usb_argv);
	if (rc != 0) {
		printf("**** ERROR ****: USB info failed\n");
		return rc;
	}

	/* USB Storage */
	post_log("\n\nUSB storage - Display USB disk info\n");
	strcpy(usb_argv[0], "usb");
	strcpy(usb_argv[1], "storage");
	rc = do_usb(0, 0, 2, usb_argv);
	if (rc != 0) {
		printf("**** ERROR ****: USB storage failed\n");
		return rc;
	}

	/* Get the number of device attached */
	num_devices = usb_get_attach_dev_count();
	for (i = 0; i < num_devices; i++) {
		/* Set the USB DEV */
		strcpy(usb_argv[0], "usb");
		strcpy(usb_argv[1], "dev");

		switch (i) {
		case 0:
			strcpy(usb_argv[2], "0");
			break;

		case 1:
			strcpy(usb_argv[2], "1");
			break;

		case 2:
			strcpy(usb_argv[2], "2");
			break;

		default:
			printf("Max number of devices can be connected are #3\n");
			break;
		}
		rc = do_usb(0, 0, 3, usb_argv);
		if (rc != 0) {
			printf("** ERROR **: USB dev failed. device %d\n", i);
			return rc;
		}

		rc = usb_read_write();
		if (rc != 0) {
			printf("** ERROR **: USB reaf/write test failed. device %d\n", i);
			return rc;
		}
		post_log("\n\nUSB2.0 test on port %d passed\n", i);
	}
	return rc;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_USB20 */
