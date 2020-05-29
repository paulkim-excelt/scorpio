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
#include <linux/types.h>
#include <asm/arch/socregs.h>

/*J new*/
#include <div64.h>
#include <dm.h>
#include <malloc.h>
#include <spi.h>
#include <spi_flash.h>
#include <asm/io.h>
#include <dm/device-internal.h>

/* Test block within 1M offset
 * BSPI area, qspi_s0 = 0x68000000 */
#define FLASH_START_ADDR	(0x68000000)
#define TEST_BLOCK_OFFSET	(0x001FF0000)
#define TEST_BLOCK_OFFSET_STR	"0x001FF0000"

#define TEST_BLOCK_SIZE	4096
#define TEST_PATTERN		0x5566778899aabbcc

#define TEST_BLK_CNT 1024
#define BLK_SIZE 64
#define SPI_DATA_BUF_SIZE (BLK_SIZE * TEST_BLK_CNT)
#define SPI_DATA_BUF_SIZE_STR "0x10000"

#define SPI_SAVE_DATA_ADDR		0x82000000ULL
#define SPI_SAVE_DATA_ADDR_STR		"0x82000000"
#define SPI_PATTERN_DATA_ADDR		0x83000000ULL
#define SPI_PATTERN_DATA_ADDR_STR	"0x83000000"
#define SPI_COMPARE_DATA_ADDR		0x84000000ULL
#define SPI_COMPARE_DATA_ADDR_STR	"0x84000000"

uint8_t JEDEC[6];

/*J global struct */
static struct spi_flash *flash;


/*
 * Wait until certain bits of the register become what we want
 */
static void wait_for_complete(uint64_t reg, uint32_t mask, uint32_t value)
{
	for (;;) {
		if (((*(uint32_t *)reg) & mask) == value) {
			/* post_log("Wait: reg(0x%x)&mask(0x%x)=value(0x%x)\n",
			 *(uint32_t *)reg, mask, value); */
			break;
		}
		__udelay(10);
	}
}


/*
 * Generic MSPI translation (max 4 bytes tx)
 *   Return: address to read data back (with +8 increment)
 */
static uint64_t mspi_translation(uint32_t tx_data, uint32_t tx_len, uint32_t rx_len)
{
	int32_t i;
	uint64_t dst;
	uint32_t total;

	// Fill TXRAM
	*(uint32_t *)QSPI_mspi_TXRAM00 = tx_data & 0xff;
	*(uint32_t *)QSPI_mspi_TXRAM02 = (tx_data >> 8) & 0xff;
	*(uint32_t *)QSPI_mspi_TXRAM04 = (tx_data >> 16) & 0xff;
	*(uint32_t *)QSPI_mspi_TXRAM06 = (tx_data >> 24) & 0xff;

	// Fill CDRAM
	total = tx_len + rx_len;
	dst = QSPI_mspi_CDRAM00;

	for (i = 0; i < total; i++, dst += 4) {
		*(uint32_t *)dst = 0x82;
		if (i == total - 1) {
			*(uint32_t *)dst = 0x02;
		}
	}

	// Queue pointers
	*(uint32_t *)QSPI_mspi_NEWQP = 0;
	*(uint32_t *)QSPI_mspi_ENDQP = total - 1;

	// Start it
	*(uint32_t *)QSPI_mspi_MSPI_STATUS = 0x00;
	*(uint32_t *)QSPI_mspi_SPCR2 = 0xc0;

	// Wait for complete
	wait_for_complete(QSPI_mspi_MSPI_STATUS, 1, 1);

	// Return the address of the first byte to read from
	return QSPI_mspi_RXRAM01 + 8 * tx_len;
}

void read_jedec_id(void)
{
	uint64_t rxaddr, i;

	for (i = 0; i < 5; i++)
		JEDEC[i] = 0;

	post_log("\nMSPI: reading JEDEC Device ID");
	rxaddr = mspi_translation(0x9f, 1, 5);

	JEDEC[0] = *(uint8_t *)rxaddr;
	JEDEC[1] = *(uint8_t *)(rxaddr + 8);
	JEDEC[2] = *(uint8_t *)(rxaddr + 16);
	JEDEC[3] = *(uint8_t *)(rxaddr + 24);
	JEDEC[4] = *(uint8_t *)(rxaddr + 32);

	post_log("\nJEDEC-ID read from Serial Flash\n");
	post_log("\n ID0, MANUFACTURER ID=%02x", JEDEC[0]);
	post_log("\n ID1, Memory Type    =%02x", JEDEC[1]);
	post_log("\n ID2, Memory Capacity=%02x", JEDEC[2]);
	post_log("\n ID3, UniqueID Length=%02x", JEDEC[3]);
	post_log("\n ID4, UniqueID Byte-1=%02x", JEDEC[4]);
}

/*
 * Initialize QSPI and serial flash
 */
void qspi_mspi_init(void)
{
	// Switch to MSPI if not yet
	if (*(uint32_t *)QSPI_bspi_registers_MAST_N_BOOT_CTRL == 0) {
		// Wait for BSPI ready
		post_log("\nWait for BSPI ready");
		wait_for_complete(QSPI_bspi_registers_BUSY_STATUS, 1, 0);

		// Switch to MSPI
		post_log("\nSwitch to MSPI");
		*(uint32_t *)QSPI_bspi_registers_MAST_N_BOOT_CTRL = 1;
	}
	// Basic MSPI configuration
	*(uint32_t *)QSPI_mspi_SPCR0_LSB = 0x8;	//baud rate = system_clock/(2 * 62)
	*(uint32_t *)QSPI_mspi_SPCR0_MSB = 0xa3;	//SPI master, no waiting before transaction, SCK=1.
}

static int j_spi_flash_probe(int argc, char * const argv[])
{
	unsigned int bus = CONFIG_SF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SF_DEFAULT_CS;
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
#ifdef CONFIG_DM_SPI_FLASH
	struct udevice *new, *bus_dev;
	int ret;
#else
	struct spi_flash *new;
#endif

#ifdef CONFIG_DM_SPI_FLASH
	/* Remove the old device, otherwise probe will just be a nop */
	ret = spi_find_bus_and_cs(bus, cs, &bus_dev, &new);
	if (!ret) {
		device_remove(new);
		device_unbind(new);
	}

	flash = NULL;
	ret = spi_flash_probe_bus_cs(bus, cs, speed, mode, &new);
	if (ret) {
		printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
		       bus, cs, ret);
		return 1;
	}

	flash = new->uclass_priv;
#else
	new = spi_flash_probe(bus, cs, speed, mode);
	if (!new) {
		printf("Failed to initialize SPI flash at %u:%u\n", bus, cs);
		return 1;
	}

	if (flash)
		spi_flash_free(flash);
	flash = new;
#endif

	return 0;
}

static int j_spi_flash_read_write(int argc, char * const argv[])
{
	unsigned long addr;
	unsigned long offset;
	unsigned long len;
	void *buf;
	char *endp;
	int ret = 1;

	if (argc < 4)
		return -1;

	addr = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		return -1;

	offset = simple_strtoul(argv[2], &endp, 16);
	if (*argv[2] == 0 || *endp != 0)
		return -1;

	len = simple_strtoul(argv[3], &endp, 16);
	if (*argv[3] == 0 || *endp != 0)
		return -1;
	/* Consistency checking */
	if (offset + len > flash->size) {
		printf("ERROR: attempting %s past flash size (%#x)\n",
		       argv[0], flash->size);
		return 1;
	}

	buf = map_physmem(addr, len, MAP_WRBACK);
	if (!buf) {
		puts("Failed to map physical memory\n");
		return 1;
	}

	if (strncmp(argv[0], "read", 4) == 0 ||
	    strncmp(argv[0], "write", 5) == 0) {
		int read;

		read = strncmp(argv[0], "read", 4) == 0;
		if (read)
			ret = spi_flash_read(flash, offset, len, buf);
		else
			ret = spi_flash_write(flash, offset, len, buf);

		post_log("- %zu bytes @ %#x %s: %s\n", (size_t)len, (u32)offset,
			 read ? "Read" : "Written", ret ? "ERROR" : "OK");
	}

	unmap_physmem(buf, len);

	return ret == 0 ? 0 : 1;
}

/*
 * This function computes the length argument for the erase command.
 * The length on which the command is to operate can be given in two forms:
 * 1. <cmd> offset len  - operate on <'offset',  'len')
 * 2. <cmd> offset +len - operate on <'offset',  'round_up(len)')
 * If the second form is used and the length doesn't fall on the
 * sector boundary, than it will be adjusted to the next sector boundary.
 * If it isn't in the flash, the function will fail (return -1).
 * Input:
 *    arg: length specification (i.e. both command arguments)
 * Output:
 *    len: computed length for operation
 * Return:
 *    1: success
 *   -1: failure (bad format, bad address).
 */
static int sf_parse_len_arg(char *arg, ulong *len)
{
	char *ep;
	char round_up_len; /* indicates if the "+length" form used */
	ulong len_arg;

	round_up_len = 0;
	if (*arg == '+') {
		round_up_len = 1;
		++arg;
	}

	len_arg = simple_strtoul(arg, &ep, 16);
	if (ep == arg || *ep != '\0')
		return -1;

	if (round_up_len && flash->sector_size > 0)
		*len = ROUND(len_arg, flash->sector_size);
	else
		*len = len_arg;

	return 1;
}

static int j_spi_flash_erase(int argc, char * const argv[])
{
	unsigned long offset;
	unsigned long len;
	char *endp;
	int ret;

	if (argc < 3)
		return -1;

	offset = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
		/* broken */
		return -1;

	ret = sf_parse_len_arg(argv[2], &len);
	if (ret != 1)
		return -1;

	/* Consistency checking */
	if (offset + len > flash->size) {
		printf("ERROR: attempting %s past flash size (%#x)\n",
		       argv[0], flash->size);
		return 1;
	}

	ret = spi_flash_erase(flash, offset, len);
	printf("%zu bytes @ %#x Erased: %s\n", (size_t)len, (u32)offset,
	       ret ? "ERROR" : "OK");

	return ret == 0 ? 0 : 1;
}

static void help(void)
{
	post_log("\n ---------------------\n");
	post_log("| QSPI DIAG HELP MENU |\n");
	post_log(" ---------------------\n");
}

int qspi_post_test(int flags)
{
	int status = 0;
	uint32_t i;

	static char argv_buf[4 * 15];
	static char *cmd_argv[4];
	int ret;

	if (flags & POST_HELP) {
		help();
		return 0;
	}


	memset(&argv_buf, 0, 4 * 15);
	for (i = 0; i < 4; i++)
		cmd_argv[i] = &argv_buf[i * 15];

	/* MSPI init & Read JEDEC info */
	qspi_mspi_init();
	read_jedec_id();

	post_log("\nQSPI DIAG TEST BEGIN\n\n");
	strcpy(cmd_argv[0], "probe");
	strcpy(cmd_argv[1], "0");
	ret = j_spi_flash_probe(2, cmd_argv);

	post_log("Clear area for pattern (dest. 0x83000000): ");
	for (i = 0; i < SPI_DATA_BUF_SIZE/8; i++)
		*(unsigned long *)(SPI_PATTERN_DATA_ADDR + i * 8) = 0;
	post_log("OK\n");

	/* sf read 0x82000000 0x1ff0000 0x10000
	 * take (0x10000) amount of data from 0x1ff0000 and put it in SAVE */
	post_log("Read in save data (dest. 0x82000000): ");
	strcpy(cmd_argv[0], "read");
	strcpy(cmd_argv[1], SPI_SAVE_DATA_ADDR_STR);
	strcpy(cmd_argv[2], TEST_BLOCK_OFFSET_STR);
	strcpy(cmd_argv[3], SPI_DATA_BUF_SIZE_STR);
	j_spi_flash_read_write(4, cmd_argv);

	post_log("Create a pattern in DDR (dest. 0x83000000): ");
	for (i = 0; i < SPI_DATA_BUF_SIZE/8; i++)
		*(unsigned long *)
			(SPI_PATTERN_DATA_ADDR + i * 8) = TEST_PATTERN;
	post_log("OK\n");

	/* sf write 0x83000000 0x1ff0000 0x10000
	 * write PATTERN (size 0x10000) located at 0x83000000 into 0x1ff0000 */
	post_log("Writing pattern into SPI (dest 0x1ff0000): ");
	strcpy(cmd_argv[0], "write");
	strcpy(cmd_argv[1], SPI_PATTERN_DATA_ADDR_STR);
	strcpy(cmd_argv[2], TEST_BLOCK_OFFSET_STR);
	strcpy(cmd_argv[3], SPI_DATA_BUF_SIZE_STR);
	j_spi_flash_read_write(4, cmd_argv);

	/* sf read 0x84000000 0x1ff0000 0x10000
	 * read 0x10000 amount of SPI data from 0x1ff0000
	 * to COMPARE area (0x83000000)*/
	post_log("Reading SPI (dest. 0x84000000): ");
	strcpy(cmd_argv[0], "read");
	strcpy(cmd_argv[1], SPI_COMPARE_DATA_ADDR_STR);
	strcpy(cmd_argv[2], TEST_BLOCK_OFFSET_STR);
	strcpy(cmd_argv[3], SPI_DATA_BUF_SIZE_STR);
	j_spi_flash_read_write(4, cmd_argv);

	/* compare 0x83000000 and 0x84000000 */
	post_log("Comparing SPI data with original pattern: ");
	for (i = 0; i < SPI_DATA_BUF_SIZE/8; i++) {
		if (*(unsigned long *)(SPI_PATTERN_DATA_ADDR + i * 8) !=
		    *(unsigned long *)(SPI_COMPARE_DATA_ADDR + i * 8)) {
			post_log("Compare failed at address <0x%lx> = 0x%lx\n",
				 SPI_COMPARE_DATA_ADDR + i * 8,
				  *(unsigned long *)
				 (SPI_COMPARE_DATA_ADDR + i * 8));
			status = -1;
			goto end;
		}
	}
	post_log("OK\n");

	/* sf erase 0x1ff0000 0x10000 */
end:	post_log("Erasing SPI (0x1ff0000): ");
	strcpy(cmd_argv[0], "erase");
	strcpy(cmd_argv[1], TEST_BLOCK_OFFSET_STR);
	strcpy(cmd_argv[2], SPI_DATA_BUF_SIZE_STR);
	j_spi_flash_erase(3, cmd_argv);


	post_log("\n");
	return status;

}
