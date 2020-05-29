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
#include <asm/arch-bcm_pegasus/bcm_otp.h>
#include <asm/arch/smc_call.h>
#include <asm/arch/bcm_secure_reg_ops.h>

#if CONFIG_POST & CONFIG_SYS_POST_USB

/* Size in byte */
#define USB_DATA_BUF_SIZE	(0x40 * 0x4)

#define USB_DATA_BUF_ADDR		0x80000000ULL
#define USB_DATA_BUF_ADDR_STR		"80000000"
#define USB_DST_DATA_BUF_ADDR		0x81000000ULL
#define USB_DST_DATA_BUF_ADDR_STR	"81000000"

static inline void usb2_port_on_off(int enable)
{
	u32 reg_data = 0x802;

	if (enable)
		smc_mem_write32(CDRU_USB20PHY_P1_CTRL, reg_data);
	else
		smc_mem_write32(CDRU_USB20PHY_P1_CTRL, 0x0);
}

static inline void usb3_usb2_port_on_off(int enable)
{
	u32 reg_data = 0x802;

	if (enable)
		smc_mem_write32(CDRU_USB20PHY_P2_CTRL, reg_data);
	else
		smc_mem_write32(CDRU_USB20PHY_P2_CTRL, 0x0);
}

static inline void usb3_port_on_off(int enable)
{
	u32 reg_data = 0x2;

	if (enable)
		smc_mem_write32(CDRU_USB30PHY_P0_CTRL, reg_data);
	else
		smc_mem_write32(CDRU_USB30PHY_P0_CTRL, 0x0);
}

/* Returns the number of the attached Devices */
static int usb_get_attach_dev_count(void)
{
	int devno;
	int count = 0;
#ifdef CONFIG_USB_STORAGE
	struct blk_desc *stor_dev;
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
	char argv_buf[7*10];
	char *usb_argv[7];
	int  i, rc = 0, val;

	memset(&argv_buf, 0, 7*10);

	for (i = 0; i < 7; i++)
		usb_argv[i] = &argv_buf[i*10];

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
	post_log("\n\nFill mem with incremental data, 32K bytes, addr=0x%x\n",
		 USB_DATA_BUF_ADDR);
	for (i = 0; i < (USB_DATA_BUF_SIZE / 4); i++)
		*(unsigned int *)(USB_DATA_BUF_ADDR + (i * 4)) = i;

#ifdef USB_DIAGS_DEBUG
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

	for (i = 0; i < USB_DATA_BUF_SIZE/4; i++) {
		val = *(unsigned int *)(USB_DST_DATA_BUF_ADDR + (i * 4));
		if (*(unsigned int *)(USB_DST_DATA_BUF_ADDR + (i * 4)) != i) {
			post_log("\nCompare failed at addr <0x%08x> = 0x%08x\n",
				 USB_DATA_BUF_ADDR + (i * 4), val);
			break;
		}
	}

	if (i == (USB_DATA_BUF_SIZE/4)) {
		post_log("\n\nRead back and compare OK\n");
		post_log("\n\nUSB test done\n");
	}
	return 0;
failed:
	post_log("\n\nUSB test failed\n");
	rc = -1;
	return rc;
}

int USB_test(int flags)
{
	char argv_buf[7*10];
	char *usb_argv[7];
	int  i, rc = 0;
	int num_devices;
	int retry = 1;

	memset(&argv_buf, 0, 7*10);

	for (i = 0; i < 7; i++)
		usb_argv[i] = &argv_buf[i*10];

	if (!is_usb_subsys_supported()) {
		post_log("USB subsystem not supported\n");
		return BCM_NO_IP;
	}

usb_diag_start:
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
		if (retry) {
			/* In case USB3 port got detected as USB2 port
			 * initially (like some lexar USB3 pendrives),
			 * a reset here will resolve the issue.
			 */
			retry--;
			printf("Retrying.. USB3 Port detected as USB2 ??\n");
			goto usb_diag_start;
		}
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
			printf("**** ERROR ****: USB dev failed\n");
			return rc;
		}

		rc = usb_read_write();
	}
	return rc;
}

int USB_post_test(int flags)
{
	u32 reg_val;
	u32 p1_speed; /* USB 3 port */
	u32 p2_speed = 1; /* USB 2 port */
	u32 p3_speed; /* USB 3 USB 2 port */
	u32 ret = 0;
	char *s = getenv("board_name");
	char *speed[] = {"UnKnown", "Full", "Low", "High", "Super",
			 "Super Plus"};

#define DEV_SPEED_MASK          (0xf << 10)
#define XDEV_FS                 (0x1 << 10)
#define XDEV_LS                 (0x2 << 10)
#define XDEV_HS                 (0x3 << 10)
#define XDEV_SS                 (0x4 << 10)
#define XDEV_SSP                (0x5 << 10)
#define DEV_UNDEFSPEED(p)       (((p) & DEV_SPEED_MASK) == (0x0<<10))
#define DEV_FULLSPEED(p)        (((p) & DEV_SPEED_MASK) == XDEV_FS)
#define DEV_LOWSPEED(p)         (((p) & DEV_SPEED_MASK) == XDEV_LS)
#define DEV_HIGHSPEED(p)        (((p) & DEV_SPEED_MASK) == XDEV_HS)
#define DEV_SUPERSPEED(p)       (((p) & DEV_SPEED_MASK) == XDEV_SS)
#define DEV_SUPERSPEEDPLUS(p)   (((p) & DEV_SPEED_MASK) == XDEV_SSP)
#define DEV_SUPERSPEED_ANY(p)   (((p) & DEV_SPEED_MASK) >= XDEV_SS)
#define DEV_PORT_SPEED(p)       (((p) >> 10) & 0x0f)

	/* XHCI has #3 ports.
	 * Port_1 is USB3 port
	 * Port_2 is USB2 port (Dedicated USB2 port)
	 * Port_3 is USB3 USB2 port */
	printf("Testing USB_3 port\n");
	/* Enable USB port P1 and Disable P2 and P3 */
	udelay(100);
	usb2_port_on_off(0);
	udelay(100);
	usb3_usb2_port_on_off(0);
	udelay(100);
	usb3_port_on_off(1);
	udelay(100);
	ret = USB_test(flags);
	if (ret == BCM_NO_IP) {
		printf("BOO !!!! Chip does not support USB\n");
		return ret;
	} else if (ret) { /* Other values which are returned from do_usb() */
		printf("P1 USB3 port test failed\n");
		ret = -1;
	}

	reg_val = readl(XHC_XHC_PORTSC1);

	p1_speed = DEV_PORT_SPEED(reg_val);

	if (p1_speed > XDEV_SSP) {
		printf("**** ERR: USB3 port(P1) USB device speed is unknown\n");
		p1_speed = 0;
	}

	if ((s) && (strcmp(s, PEGASUS_XMC_BOARD))) {
		printf("Testing USB2 port\n");
		/* Enable USB port P2 and Disable P1 and P3 */
		usb2_port_on_off(1);
		udelay(100);
		usb3_usb2_port_on_off(0);
		udelay(100);
		usb3_port_on_off(0);
		udelay(100);
		if (USB_test(flags)) {
			printf("P2 USB2 port test failed\n");
			ret = -1;
		}

		reg_val = readl(XHC_XHC_PORTSC2);

		p2_speed = DEV_PORT_SPEED(reg_val);
		if (p2_speed > XDEV_SSP) {
			printf("**** ERR: ");
			printf("USB2 port(P2) USB device speed is unknown\n");
			p2_speed = 0;
		}
	}

	printf("Testing USB_3 USB2 port\n");
	/* Enable USB port P3 and Disable P1 and P2 */
	usb3_usb2_port_on_off(1);
	udelay(100);
	usb2_port_on_off(0);
	udelay(100);
	usb3_port_on_off(0);
	udelay(100);
	if (USB_test(flags)) {
		printf("P3 USB3 USB2 port test failed\n");
		ret = -1;
	}

	reg_val = readl(XHC_XHC_PORTSC3);

	p3_speed = DEV_PORT_SPEED(reg_val);

	if (p3_speed > XDEV_SSP) {
		printf("**** ERR: USB2 port(P3) USB device speed is unknown\n");
		p3_speed = 0;
	}

	printf("P1 port %s speed\n", speed[p1_speed]);
	if ((s) && (strcmp(s, PEGASUS_XMC_BOARD)))
		printf("P2 port %s speed\n", speed[p2_speed]);
	printf("P3 port %s speed\n", speed[p3_speed]);

	usb2_port_on_off(1);
	usb3_usb2_port_on_off(1);
	return ret;
}

#endif /* CONFIG_POST & CONFIG_SYS_POST_USB20 */
