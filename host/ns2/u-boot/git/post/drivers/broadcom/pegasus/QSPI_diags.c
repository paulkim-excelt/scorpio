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

/*Test block within 1M offset*/
#define FLASH_START_ADDR	0x8000000
#define TEST_BLOCK_OFFSET       (0x00100000)
#define TEST_BLOCK_SIZE		4096
#define TEST_PATTERN_BASE	0x40

u8 pu8DataPattern[16];	/*To store the data pattern for testing */
const int iBlockSize = TEST_BLOCK_SIZE;
const u32 u32TestBlockOffset = TEST_BLOCK_OFFSET;
u8 JEDEC[6];

/*
 * #define TEST_BLOCK_OFFSET	(0x00FF0000)
 * Wait until certain bits of the register become what we want
 */
static void wait_for_complete(u64 reg, u32 mask, u32 value)
{
	for (;;) {
		if (((*(u32 *)reg) & mask) == value) {
			/*post_log(" Waiting for: reg(0x%x)&mask(0x%x)=
			 * value(0x%x)\n", *(u32 *)reg, mask, value);
			 */
			break;
		}
		__udelay(10);
	}
}

/*
 * Generic MSPI translation (max 4 bytes tx)
 *   Return: address to read data back (with +8 increment)
 */
static u64 mspi_translation(u32 tx_data, u32 tx_len,
			    u32 rx_len)
{
	s32 i;
	u64 dst;
	u32 total;

	/*Fill TXRAM */
	*(u32 *)QSPI_mspi_TXRAM00 = tx_data & 0xff;
	*(u32 *)QSPI_mspi_TXRAM02 = (tx_data >> 8) & 0xff;
	*(u32 *)QSPI_mspi_TXRAM04 = (tx_data >> 16) & 0xff;
	*(u32 *)QSPI_mspi_TXRAM06 = (tx_data >> 24) & 0xff;

	/*Fill CDRAM */
	total = tx_len + rx_len;
	dst = QSPI_mspi_CDRAM00;
	for (i = 0; i < total; i++, dst += 4) {
		*(u32 *)dst = 0x82;
		if (i == total - 1)
			*(u32 *)dst = 0x02;
	}

	/*Queue pointers */
	*(u32 *)QSPI_mspi_NEWQP = 0;
	*(u32 *)QSPI_mspi_ENDQP = total - 1;

	/*Start it */
	*(u32 *)QSPI_mspi_MSPI_STATUS = 0x00;
	*(u32 *)QSPI_mspi_SPCR2 = 0xc0;

	/*Wait for complete */
	wait_for_complete(QSPI_mspi_MSPI_STATUS, 1, 1);

	/*Return the address of the first byte to read from */
	return QSPI_mspi_RXRAM01 + 8 * tx_len;
}

/*This is a hack version for Program command*/

static u64 mspi_translation_w(u32 tx_data, u32 tx_len,
			      u32 rx_len, u8 *pchar)
{
	u32 i;
	u64 dst, _v;
	u32 total;

	/*Fill TXRAM */
	*(u32 *)QSPI_mspi_TXRAM00 = tx_data & 0xff;
	*(u32 *)QSPI_mspi_TXRAM02 = (tx_data >> 8) & 0xff;
	*(u32 *)QSPI_mspi_TXRAM04 = (tx_data >> 16) & 0xff;
	*(u32 *)QSPI_mspi_TXRAM06 = (tx_data >> 24) & 0xff;

	if (pchar) {
		for (i = 0; i < 8; i++) {
			_v = QSPI_mspi_TXRAM08 + 8 * i;
			*(u8 *)_v = *(pchar + i);
			printf("0x%x ", *(pchar + i));
		}
	}
	/*Fill CDRAM */
	total = tx_len + rx_len;
	dst = QSPI_mspi_CDRAM00;
	for (i = 0; i < total; i++, dst += 4) {
		*(u32 *)dst = 0x82;
		if (i == total - 1)
			*(u32 *)dst = 0x02;
	}

	/*Queue pointers */
	*(u32 *)QSPI_mspi_NEWQP = 0;
	*(u32 *)QSPI_mspi_ENDQP = total - 1;

	/*Start it */
	*(u32 *)QSPI_mspi_MSPI_STATUS = 0x00;
	*(u32 *)QSPI_mspi_SPCR2 = 0xc0;

	/*Wait for complete */
	wait_for_complete(QSPI_mspi_MSPI_STATUS, 1, 1);

	/*Return the address of the first byte to read from */
	return QSPI_mspi_RXRAM01 + 8 * tx_len;
}

void read_jedec_id(void)
{
	u64 rxaddr, i;

	for (i = 0; i < 5; i++)
		JEDEC[i] = 0;

	post_log("\nMSPI: reading JEDEC Device ID");
	rxaddr = mspi_translation(0x9f, 1, 5);

	JEDEC[0] = *(u8 *)rxaddr;
	JEDEC[1] = *(u8 *)(rxaddr + 8);
	JEDEC[2] = *(u8 *)(rxaddr + 16);
	JEDEC[3] = *(u8 *)(rxaddr + 24);
	JEDEC[4] = *(u8 *)(rxaddr + 32);

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
	/*Switch to MSPI if not yet */
	if (*(u32 *)QSPI_bspi_registers_MAST_N_BOOT_CTRL == 0) {
		/*post_log("IOMUX Function 1 : 0x%x\n",
		 * *(volatile unsigned int *) 0x6501D134);
		 */
		/*Wait for BSPI ready */
		post_log("\nWait for BSPI ready");
		wait_for_complete(QSPI_bspi_registers_BUSY_STATUS, 1, 0);

		/*Switch to MSPI */
		post_log("\nSwitch to MSPI");
		*(u32 *)QSPI_bspi_registers_MAST_N_BOOT_CTRL = 1;
	}
	/*Basic MSPI configuration */
	/*baud rate = system_clock/(2 * 62) */
	*(u32 *)QSPI_mspi_SPCR0_LSB = 0x8;
	/*SPI master, no waiting before transaction, SCK=1. */
	*(u32 *)QSPI_mspi_SPCR0_MSB = 0xa3;
}

void qspi_testblock_program(void)
{
	u64 rxaddr;
	u32 cmd, u32FlashAddress;
	int i;

	post_log("\nqspi_testblock_program!\n");

	/*Clear software protection bits if any
	 * Write enable (precondition for erase)
	 */
	post_log("\nWrite Enable!");
	mspi_translation(0x06, 1, 0);
	__udelay(100000);
	/*clear write prtect bits in flash status
	 * In real chip: enable this line
	 * mspi_translation(0xa001, 2, 0);
	 */
	/*__udelay(100000);*/

	post_log("\nErase test sector\n");
	/*Erase one block/sector
	 * 4KB sector erase
	 */
	if (iBlockSize == 4096)
		cmd = 0x20;
	else
		cmd = 0xd8;

	u32FlashAddress = u32TestBlockOffset;
	cmd |= ((u32FlashAddress >> 8) & 0xff00);
	cmd |= ((u32FlashAddress << 8) & 0xff0000);
	cmd |= ((u32FlashAddress << 24) & 0xff000000);
	mspi_translation(cmd, 4, 0);	/*erase the contenet */
	__udelay(100000);

	post_log("\nWaiting for erase complete!");
	/*Wait for erase complete */
	for (;;) {
		rxaddr = mspi_translation(0x05, 1, 1);
		if ((*(u32 *)rxaddr & 1) == 0)
			break;

		__udelay(10000);
	}
	mspi_translation(0x04, 1, 0);

	__udelay(10000);

	/*Generate a data pattern. */
	for (i = 0; i < 16; i++)
		*(pu8DataPattern + i) = (TEST_PATTERN_BASE + i);

	mspi_translation(0x06, 1, 0);

	/*write 16-byte data pattern 8 */
	post_log("\nWriting an 16-byte data pattern!");

	cmd = 0x02;
	u32FlashAddress = u32TestBlockOffset;
	cmd |= ((u32FlashAddress >> 8) & 0xff00);
	cmd |= ((u32FlashAddress << 8) & 0xff0000);
	cmd |= ((u32FlashAddress << 24) & 0xff000000);

	/*post_log("cmd word = 0x%x\n", cmd); */
	mspi_translation_w(cmd, (4 + 8), 0, pu8DataPattern);

	/*Wait for program complete */
	/*post_log("\nWaiting for program complete!"); */
	for (;;) {
		rxaddr = mspi_translation(0x05, 1, 1);
		if ((*(u32 *)rxaddr & 1) == 0)
			break;
	}

	mspi_translation(0x04, 1, 0);
	__udelay(10000);
	mspi_translation(0x06, 1, 0);

	cmd = 0x02;
	u32FlashAddress = u32TestBlockOffset + 8;
	cmd |= ((u32FlashAddress >> 8) & 0xff00);
	cmd |= ((u32FlashAddress << 8) & 0xff0000);
	cmd |= ((u32FlashAddress << 24) & 0xff000000);

	/*post_log("cmd word = 0x%x\n", cmd); */
	mspi_translation_w(cmd, (4 + 8), 0, pu8DataPattern + 8);

	/*Wait for program complete */
	for (;;) {
		rxaddr = mspi_translation(0x05, 1, 1);
		if ((*(u32 *)rxaddr & 1) == 0)
			break;
	}
	mspi_translation(0x04, 1, 0);

	__udelay(10000);
}

int qspi_post_test(int flags)
{
	int status = 0;
	u32 i;
	u8 chTmp;
	u32 u32FailureCounter = 0;
	u32 cmd, u32FlashAddress;
	u64 rxaddr, _v;

	/* MSPI init & Read JEDEC info */
	qspi_mspi_init();

	read_jedec_id();

	post_log("\n******Test Block offset 0x%x, block size:%d******\n",
		 u32TestBlockOffset, iBlockSize);

	qspi_testblock_program();

	/*----------------------------------------------------------
	 * **Step 3**: Switch back to BSPI, single lane mode
	 * Read out the data pattern and verify the result
	 * ----------------------------------------------------------
	 */

	/*SPI bus is driven by BSPI */
	*(u32 *)QSPI_bspi_registers_MAST_N_BOOT_CTRL = 0;
	__udelay(10000);
	/*Invalidate bspi buffer */

	/*clear data in buffer 0 */
	*(u32 *)QSPI_bspi_registers_B0_CTRL = 1;
	/*clear data in buffer 1 */
	*(u32 *)QSPI_bspi_registers_B1_CTRL = 1;

	/*Read back Serial Flash through QSPI-BSPI */
	post_log("\n\nQSPI: Switch to BSPI, verify data:\n");
	for (i = 0; i < 16; i++) {
		_v = (FLASH_START_ADDR + TEST_BLOCK_OFFSET + i);
		chTmp = *(u8 *)_v;
		printf("0x%x ", chTmp);
		if (chTmp != (TEST_PATTERN_BASE + i))
			u32FailureCounter++;
	}
	post_log("\n");

	if (u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI (Single lane) Read: Test Failed\n");
		u32FailureCounter = 0;
		status = -1;
	} else {
		post_log("\n\nQSPI BSPI (Single lane) Read: Test Passed\n");
	}

	__udelay(1000);

	/*------------------------------------------------------------------
	 * **Step 4**: Configure QSPI BSPI (dual lane mode).
	 * Then read out data and compare with data pattern.
	 * ------------------------------------------------------------------
	 */
	/*Invalidate bspi buffer */
	*(u32 *)QSPI_bspi_registers_B0_CTRL = 1;/*clear data in buffer0 */
	*(u32 *)QSPI_bspi_registers_B1_CTRL = 1;/*clear data in buffer1 */

	/*now test BSPI read in dual mode */
	/*3-byte address, dual mode, override strap */
	*(u32 *)QSPI_bspi_registers_STRAP_OVERRIDE_CTRL = 3;
	/*enable flex mode */
	*(u32 *)QSPI_bspi_registers_FLEX_MODE_ENABLE = 1;
	/*dual mode to recv data, 2 mode bits/cycle, 2 address bits/cycle,
	 * 2 cmd bits/cycle
	 */
	*(u32 *)QSPI_bspi_registers_BITS_PER_CYCLE = 0x1010101;
	/*dummy cycle=8, 0 mode bits, 24-byte address */
	*(u32 *)QSPI_bspi_registers_BITS_PER_PHASE = 0x8;
	/*default command is fast read */
	*(u32 *)QSPI_bspi_registers_CMD_AND_MODE_BYTE = 0x3;

	/*Read back Serial Flash through QSPI-BSPI */
	post_log("\n\nQSPI: Switch to BSPI dual lane mode, verify data:\n");

	for (i = 0; i < 16; i++) {
		_v = (FLASH_START_ADDR + TEST_BLOCK_OFFSET + i);
		chTmp = *(u8 *)_v;
		printf("0x%x ", chTmp);
		if (chTmp != (TEST_PATTERN_BASE + i))
			u32FailureCounter++;
	}

	if (u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI Dual lane Read Test: Test Failed\n");
		u32FailureCounter = 0;
		status = -1;
	} else {
		post_log("\n\nQSPI_Dual dual lane Read Test: Test Passed\n");
	}

	__udelay(1000);

	/*---------------------------------------------------------------------
	 * **Step 5**: Configure QSPI BSPI (quad-lane mode).
	 * Then read out data and compare with data pattern.
	 * --------------------------------------------------------------------
	 */
	/*Invalidate bspi buffer */
	*(u32 *)QSPI_bspi_registers_B0_CTRL = 1;
	*(u32 *)QSPI_bspi_registers_B1_CTRL = 1;

	/*now test BSPI read in quad mode */
	/*3-byte address, quad mode, override strap */
	*(u32 *)QSPI_bspi_registers_STRAP_OVERRIDE_CTRL = 9;
	/*enable flex mode */
	*(u32 *)QSPI_bspi_registers_FLEX_MODE_ENABLE = 1;
	/*quad mode to recv data, 4 mode bits/cycle, 4 address bits/cycle,
	 * 4 cmd bits/cycle
	 */
	*(u32 *)QSPI_bspi_registers_BITS_PER_CYCLE = 0x2020202;
	/*dummy cycle=8, 0 mode bits, 24-byte address */
	*(u32 *)QSPI_bspi_registers_BITS_PER_PHASE = 8;
	/*default command is fast read */
	*(u32 *)QSPI_bspi_registers_CMD_AND_MODE_BYTE = 0x3;

	/*Read back Serial Flash through QSPI-BSPI */
	post_log("\n\nQSPI: Switch to BSPI quad lane mode, verify data:\n");
	for (i = 0; i < 16; i++) {
		_v = (FLASH_START_ADDR + TEST_BLOCK_OFFSET + i);
		chTmp = *(u8 *)_v;
		printf("0x%x ", chTmp);
		if (chTmp != (TEST_PATTERN_BASE + i))
			u32FailureCounter++;	/*return -1 */
	}

	if (u32FailureCounter > 0) {
		post_log("\n\nQSPI BSPI-Quad Read: Test Failed\n");
		u32FailureCounter = 0;
		status = -1;
	} else {
		post_log("\n\nQSPI_Quad Read: Test Passed\n");
	}

	/* switch back to the single mode BSPI  */
	*(u32 *)QSPI_bspi_registers_MAST_N_BOOT_CTRL = 0;
	/*3-byte address, single mode, override strap */
	*(u32 *)QSPI_bspi_registers_STRAP_OVERRIDE_CTRL = 1;
	*(u32 *)QSPI_bspi_registers_FLEX_MODE_ENABLE = 0;
	*(u32 *)QSPI_bspi_registers_BITS_PER_CYCLE = 0;
	/*dummy cycle=8, 0 mode bits, 24-byte address */
	*(u32 *)QSPI_bspi_registers_BITS_PER_PHASE = 8;
	/*default command is read */
	*(u32 *)QSPI_bspi_registers_CMD_AND_MODE_BYTE = 0x0B;

	/*----------------------------------------------------------------------
	 * **Step 6**: Erase the test block; flash the back-up data to the block
	 * ---------------------------------------------------------------------
	 */
	post_log("\nErasing the test sector\n");
	qspi_mspi_init();

	mspi_translation(0x06, 1, 0);
	mspi_translation(0xa001, 2, 0);
	/*Erase one block/sector */
	/*4KB sector erase */
	if (iBlockSize == 4096)
		cmd = 0x20;
	else
		cmd = 0xd8;

	u32FlashAddress = u32TestBlockOffset;
	cmd |= ((u32FlashAddress >> 8) & 0xff00);
	cmd |= ((u32FlashAddress << 8) & 0xff0000);
	cmd |= ((u32FlashAddress << 24) & 0xff000000);
	mspi_translation(cmd, 4, 0);	/*erase the contenet */

	/*Wait for erase complete */
	/*post_log("\nWaiting for erase complete!"); */
	for (;;) {
		rxaddr = mspi_translation(0x05, 1, 1);
		if ((*(u32 *)rxaddr & 1) == 0)
			break;
	}
	mspi_translation(0x04, 1, 0);
	/*Back to BSPI before exiting */
	*(u32 *)QSPI_bspi_registers_MAST_N_BOOT_CTRL = 0;

	return status;
}
