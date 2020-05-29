/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <config.h>
#include <i2c.h>
#include <asm/io.h>

#include <brcm_i2c_regs.h>
#include <brcm_i2c_defs.h>
#include <brcm_i2c.h>
#include <asm/arch/socregs.h>
#include "errno.h"

/*#define BCM_I2C_DIAG*/
#undef debug
#ifdef BCM_I2C_DIAG
#define debug(fmt, args...)	printf(fmt, ##args)
#else
#define debug(fmt, args...)
#endif

/* we have 2 instances */
#define NS2_I2C_MAX	4
int i2c_init_done[NS2_I2C_MAX] = { 0, 0, 0, 0 };

static u64 i2c_base = (u64) CHIPCOMMONG_SMBUS0_SMBUS_CONFIG;

#ifdef CONFIG_I2C_MULTI_BUS
static unsigned int current_bus;
#endif

/**
 * i2c_set_bus_num - change active I2C bus
 *	@bus: bus index, zero based
 *	@returns: 0 on success, non-0 on failure
 */
int i2c_set_bus_num(unsigned int bus)
{
	if ((bus < 0) || (bus >= NS2_I2C_MAX)) {
		printf("Bad bus: %d\n", bus);
		return -1;
	}

	switch (bus) {
	case 0:
		i2c_base = (u64)CHIPCOMMONG_SMBUS0_SMBUS_CONFIG;
		break;
	case 1:
		i2c_base = (u64)CHIPCOMMONG_SMBUS1_SMBUS_CONFIG;
		break;
#ifdef CHIPCOMMONG_SMBUS2_SMBUS_CONFIG
	case 2:
		i2c_base = (u64)CHIPCOMMONG_SMBUS2_SMBUS_CONFIG;
		break;
#endif

#ifdef CHIPCOMMONG_SMBUS3_SMBUS_CONFIG
	case 3:
		i2c_base = (u64)CHIPCOMMONG_SMBUS3_SMBUS_CONFIG;
		break;
#endif
	default:
		return -1;
	}
	current_bus = bus;
	return 0;
}

/**
 * i2c_get_bus_num - returns index of active I2C bus
 */
unsigned int i2c_get_bus_num(void)
{
	return current_bus;
}

/* Function to read a value from specified register. */
static unsigned int iproc_i2c_reg_read(unsigned long reg_addr)
{
	unsigned int val;

	val = __raw_readl((void *)(i2c_base + reg_addr));
	debug("i2c %d: reg 0x%p read 0x%x\n", current_bus,
	      (void *)(i2c_base + reg_addr), val);
	return cpu_to_le32(val);
}

/* Function to write a value ('val') in to a specified register. */
static int iproc_i2c_reg_write(unsigned long reg_addr, unsigned int val)
{
	val = cpu_to_le32(val);
	__raw_writel(val, (void *)(i2c_base + reg_addr));
	debug("i2c %d: reg 0x%p wrote 0x%x\n", current_bus,
	      (void *)(i2c_base + reg_addr), val);
	return 0;
}

#ifdef BCM_I2C_DIAG
static int iproc_dump_i2c_regs(void)
{
	unsigned int regval;

	debug(fmt, args...)("\n-------------------------------------------\n");
	debug("%s: Dumping SMBus registers...\n", __func__);

	regval = iproc_i2c_reg_read(CCB_SMB_CFG_REG);
	debug("CCB_SMB_CFG_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);
	debug("CCB_SMB_TIMGCFG_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_ADDR_REG);
	debug("CCB_SMB_ADDR_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_MSTRFIFOCTL_REG);
	debug("CCB_SMB_MSTRFIFOCTL_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_SLVFIFOCTL_REG);
	debug("CCB_SMB_SLVFIFOCTL_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_BITBANGCTL_REG);
	debug("CCB_SMB_BITBANGCTL_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	debug("CCB_SMB_MSTRCMD_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_SLVCMD_REG);
	debug(fmt, args...)("CCB_SMB_SLVCMD_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_EVTEN_REG);
	debug("CCB_SMB_EVTEN_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);
	debug("CCB_SMB_EVTSTS_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATAWR_REG);
	debug("CCB_SMB_MSTRDATAWR_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
	debug("CCB_SMB_MSTRDATARD_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_SLVDATAWR_REG);
	debug("CCB_SMB_SLVDATAWR_REG=0x%08X\n", regval);

	regval = iproc_i2c_reg_read(CCB_SMB_SLVDATARD_REG);
	debug("CCB_SMB_SLVDATARD_REG=0x%08X\n", regval);

	debug("----------------------------------------------\n\n");
	return 0;
}
#endif

/*
 * Function to ensure that the previous transaction was completed before
 * initiating a new transaction. It can also be used in polling mode to
 * check status of completion of a command
 */
static int iproc_i2c_startbusy_wait(void)
{
	unsigned int regval;

	regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);

	/* Check if an operation is in progress. During probe it won't be.
	 * But when shutdown/remove was called we want to make sure that
	 * the transaction in progress completed
	 */
	if (regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) {
		unsigned int i = 0;

		do {
			udelay(10000);	/* Wait for 1 msec */
			i++;
			regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
			/* If start-busy bit cleared, exit the loop */
		} while ((regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) &&
			 (i < IPROC_SMB_MAX_RETRIES));
		if (i >= IPROC_SMB_MAX_RETRIES) {
			printf("%s: START_BUSY bit didn't clear, exiting\n",
			       __func__);
			return -ETIMEDOUT;
		}
	}
	return 0;
}

/*
 * This function copies data to SMBus's Tx FIFO. Valid for write transactions
 * base_addr: Mapped address of this SMBus instance
 * dev_addr:  SMBus (I2C) device address. We are assuming 7-bit addresses
 * initially
 * info:   Data to copy in to Tx FIFO. For read commands, the size should be
 * set to zero by the caller
 *
 */
static void iproc_i2c_write_trans_data(unsigned short dev_addr,
				       struct iproc_xact_info *info)
{
	unsigned int regval;
	unsigned int i;
	unsigned int num_data_bytes = 0;

#ifdef BCM_I2C_DIAG
	debug("\n%s:dev_addr=0x%X,cmd_valid=%d, cmd=0x%02x, size=%u proto=%d\n",
	      __func__, dev_addr, info->cmd_valid, info->command,
	      info->size, info->smb_proto);
#endif /* BCM_I2C_DIAG */

    /* Depending on the SMBus protocol, we need to write additional transaction
     * data in to Tx FIFO. Refer to section 5.5 of SMBus spec for sequence for a
     * transaction
	 */
	switch (info->smb_proto) {
	case SMBUS_PROT_RECV_BYTE:
		/* No additional data to be written */
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG,
				    dev_addr | 0x1 | CCB_SMB_MSTRWRSTS_MASK);
		num_data_bytes = 0;
		break;
	case SMBUS_PROT_SEND_BYTE:
		num_data_bytes = info->size;
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr);
		break;
	case SMBUS_PROT_RD_BYTE:

	case SMBUS_PROT_RD_WORD:
	case SMBUS_PROT_BLK_RD:
		/* Write slave address with R/W~ clear(bit #0) */
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr);
		num_data_bytes = 0;
		break;
	case SMBUS_PROT_BLK_WR_BLK_RD_PROC_CALL:
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr | 0x1 |
				    CCB_SMB_MSTRWRSTS_MASK);
		num_data_bytes = 0;
		break;
	case SMBUS_PROT_WR_BYTE:
	case SMBUS_PROT_WR_WORD:
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, dev_addr);
		/* No additional bytes to be written. Data portion is written
		 * in the 'for' loop below
		 */
		num_data_bytes = info->size;
		break;
	case SMBUS_PROT_BLK_WR:
		/* 3rd byte is byte count */
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->size);
		num_data_bytes = info->size;
		break;
	default:
		break;
	}

	/* If the protocol needs command code, copy it */
	if (info->cmd_valid) {
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, info->command);
		/* add the read portion of the transaction to the tx fifo */
		switch (info->smb_proto) {
		case SMBUS_PROT_RD_BYTE:
		case SMBUS_PROT_RD_WORD:
		case SMBUS_PROT_BLK_RD:
			/* Write slave address with R/W~ set (bit #0) */
			iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG,
					    dev_addr | 0x1);
			break;
		}
	}

/* Copy actual data from caller, next. In general, for reads, no data is
 * copied
 */
	for (i = 0; num_data_bytes; --num_data_bytes, i++) {
		/* For the last byte, set MASTER_WR_STATUS bit */
		regval = (num_data_bytes == 1) ?
		    info->data[i] | CCB_SMB_MSTRWRSTS_MASK : info->data[i];
		iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, regval);
	}
}

static int iproc_i2c_data_send(unsigned short addr,
			       struct iproc_xact_info *info)
{
	int rc, retry = 3;
	unsigned int regval;

	/* Make sure the previous transaction completed */
	rc = iproc_i2c_startbusy_wait();

	if (rc < 0) {
		printf("%s: Send: bus is busy, exiting\n", __func__);
		return rc;
	}
	/* Write transaction bytes to Tx FIFO */
	iproc_i2c_write_trans_data(addr, info);
	/* Program master command register (0x30) with protocol type and set
	 * start_busy_command bit to initiate the write transaction
	 */
	regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
	    CCB_SMB_MSTRSTARTBUSYCMD_MASK;
	iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);
	/* Check for Master status */
	regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	while (regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) {
		udelay(10000);
		if (retry-- <= 0)
			break;
		regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	}
	/* If start_busy bit cleared, check if there are any errors */
	if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {
		/* start_busy bit cleared, check master_status field now */
		regval &= CCB_SMB_MSTRSTS_MASK;
		regval >>= CCB_SMB_MSTRSTS_SHIFT;
		if (regval != MSTR_STS_XACT_SUCCESS) {
			/* Error We can flush Tx FIFO here */
#ifdef BCM_I2C_DIAG
			printf("%s: ERROR: %u exiting", __func__, regval);
#endif
			return -EREMOTEIO;
		}
	}
	return 0;
}

static int iproc_i2c_data_recv(unsigned short addr,
			       struct iproc_xact_info *info,
			       unsigned int *num_bytes_read)
{
	int rc, retry = 3;
	unsigned int regval;

	/* Make sure the previous transaction completed */
	rc = iproc_i2c_startbusy_wait();

	if (rc < 0) {
		printf("%s: Receive: Bus is busy, exiting\n", __func__);
		return rc;
	}

	/* Program all transaction bytes into master Tx FIFO */
	iproc_i2c_write_trans_data(addr, info);

	/* Program master command register (0x30) with protocol type and set
	 * start_busy_command bit to initiate the write transaction
	 */
	regval = (info->smb_proto << CCB_SMB_MSTRSMBUSPROTO_SHIFT) |
	    CCB_SMB_MSTRSTARTBUSYCMD_MASK | info->size;

	iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);
	/* Check for Master status */
	regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	while (regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK) {
		udelay(1000);
		if (retry-- <= 0)
			break;
		regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	}

	/* If start_busy bit cleared, check if there are any errors */
	if (!(regval & CCB_SMB_MSTRSTARTBUSYCMD_MASK)) {
		/* start_busy bit cleared, check master_status field now */
		regval &= CCB_SMB_MSTRSTS_MASK;
		regval >>= CCB_SMB_MSTRSTS_SHIFT;
		if (regval != MSTR_STS_XACT_SUCCESS) {
			/* We can flush Tx FIFO here */
			printf("%s: Error in transaction %d, exiting",
			       __func__, regval);
			return -EREMOTEIO;
		}
	}
	/* Read received byte(s), after TX out address etc */
	regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
	/* For block read, protocol (hw) returns byte count,as the first byte */
	if (info->smb_proto == SMBUS_PROT_BLK_RD) {
		int i;
		*num_bytes_read = regval & CCB_SMB_MSTRRDDATA_MASK;
		/* Limit to reading a max of 32 bytes only; just a safeguard. If
		 * # bytes read is a number > 32, check transaction set up, and
		 * contact hw engg. Assumption: PEC is disabled
		 */
		for (i = 0; (i < *num_bytes_read) &&
		     (i < I2C_SMBUS_BLOCK_MAX); i++) {
			/* Read Rx FIFO for data bytes */
			regval = iproc_i2c_reg_read(CCB_SMB_MSTRDATARD_REG);
			info->data[i] = regval & CCB_SMB_MSTRRDDATA_MASK;
		}
	} else {
		/* 1 Byte data */
		*info->data = regval & CCB_SMB_MSTRRDDATA_MASK;
		*num_bytes_read = 1;
	}

	return 0;
}

/*
 * This function set clock frequency for SMBus block. As per hardware
 * engineering, the clock frequency can be changed dynamically.
 */
static int iproc_i2c_set_clk_freq(smb_clk_freq_t freq)
{
	unsigned int regval;
	unsigned int val;

	switch (freq) {
	case I2C_SPEED_100KHz:
		val = 0;
		break;
	case I2C_SPEED_400KHz:
		val = 1;
		break;
	default:
		return -EINVAL;
	}

	regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);

	SETREGFLDVAL(regval, val, CCB_SMB_TIMGCFG_MODE400_MASK,
		     CCB_SMB_TIMGCFG_MODE400_SHIFT);

	iproc_i2c_reg_write(CCB_SMB_TIMGCFG_REG, regval);
	return 0;
}

static void iproc_i2c_init(int speed, int slaveadd)
{
	unsigned int regval;

#ifdef BCM_I2C_DIAG
	debug("\n%s: Entering iproc_i2c_init\n", __func__);
#endif /* BCM_I2C_DIAG */

	/* Flush Tx, Rx FIFOs. Note we are setting the Rx FIFO threshold to 0.
	 * May be OK since we are setting RX_EVENT and RX_FIFO_FULL interrupts
	 */
	regval = CCB_SMB_MSTRRXFIFOFLSH_MASK | CCB_SMB_MSTRTXFIFOFLSH_MASK;
	iproc_i2c_reg_write(CCB_SMB_MSTRFIFOCTL_REG, regval);

	/* Enable SMbus block. Note, we are setting MASTER_RETRY_COUNT to zero
	 * since there will be only one master
	 */

	regval = iproc_i2c_reg_read(CCB_SMB_CFG_REG);
	regval |= CCB_SMB_CFG_SMBEN_MASK;
	iproc_i2c_reg_write(CCB_SMB_CFG_REG, regval);
	/* Wait a minimum of 50 Usec, as per SMB hw doc. But we wait longer */
	udelay(10000);

	/* Set default clock frequency */
	iproc_i2c_set_clk_freq(I2C_SPEED_100KHz);

	/* Disable intrs */
	regval = 0x0;
	iproc_i2c_reg_write(CCB_SMB_EVTEN_REG, regval);

	/* Clear intrs (W1TC) */
	regval = iproc_i2c_reg_read(CCB_SMB_EVTSTS_REG);
	iproc_i2c_reg_write(CCB_SMB_EVTSTS_REG, regval);

	i2c_init_done[current_bus] = 1;

#ifdef BCM_I2C_DIAG
	iproc_dump_i2c_regs(void);

	debug("%s: Init successfulllyy\n", __func__);
#endif /* BCM_I2C_DIAG */
}

void i2c_init(int speed, int slaveadd)
{
	iproc_i2c_init(speed, slaveadd);
}

int i2c_probe(uchar chip)
{
	u32 slave_addr = chip, regval;
#ifdef BCM_I2C_DIAG
	debug("\n%s: Entering probe\n", __func__);
#endif /* BCM_I2C_DIAG */

	/* Init internal regs, disable intrs (and then clear intrs), set fifo
	 * thresholds, etc.
	 */
	if (!i2c_init_done[current_bus])
		iproc_i2c_init(0, 0);

	regval = (slave_addr << 1);
	iproc_i2c_reg_write(CCB_SMB_MSTRDATAWR_REG, regval);
	regval = ((0 << 9) | (1 << 31));
	iproc_i2c_reg_write(CCB_SMB_MSTRCMD_REG, regval);
	do {
		udelay(1000);
		regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
		regval &= CCB_SMB_MSTRSTARTBUSYCMD_MASK;
	} while (regval);
	regval = iproc_i2c_reg_read(CCB_SMB_MSTRCMD_REG);
	if (((regval >> 25) & 7) == 0)
		/* printf("i2c slave address:%x\n", slave_addr); */
		;
	else
		return -1;

#ifdef BCM_I2C_DIAG
	iproc_dump_i2c_regs();
	debug("%s: probe successful\n", __func__);
#endif /* BCM_I2C_DIAG */
	return 0;
}

static int i2c_read_byte(u8 devaddr, u8 regoffset, u8 *value)
{
	int rc;
	struct iproc_xact_info info;
	unsigned int num_bytes_read = 0;

	devaddr <<= 1;

	info.cmd_valid = 1;
	info.command = (unsigned char)regoffset;
	info.data = value;
	info.size = 1;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_RD_BYTE;
	/* Refer to i2c_smbus_read_byte for params passed. */
	rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

	if (rc < 0) {
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Read", devaddr);
		return -EREMOTEIO;
	}
	return 0;
}

int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int i;

	if (alen > 1) {
		printf("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_read_byte(chip, addr + i, &buffer[i])) {
			printf("I2C read: I/O error\n");
			iproc_i2c_init(CONFIG_SYS_I2C_SPEED,
				       CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}

	return 0;
}

static int i2c_write_byte(u8 devaddr, u8 regoffset, u8 value)
{
	int rc;
	struct iproc_xact_info info;

	devaddr <<= 1;

	info.cmd_valid = 1;
	info.command = (unsigned char)regoffset;
	info.data = &value;
	info.size = 1;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_WR_BYTE;
	/* Refer to i2c_smbus_write_byte params passed. */
	rc = iproc_i2c_data_send(devaddr, &info);

	if (rc < 0) {
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Write", devaddr);
		return -EREMOTEIO;
	}

	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	int i;

	if (alen > 1) {
		printf("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (addr + len > 256) {
		printf("I2C read: address out of range\n");
		return 1;
	}

	for (i = 0; i < len; i++) {
		if (i2c_write_byte(chip, addr + i, buffer[i])) {
			printf("I2C read: I/O error\n");
			iproc_i2c_init(CONFIG_SYS_I2C_SPEED,
				       CONFIG_SYS_I2C_SLAVE);
			return 1;
		}
	}
	return 0;
}

/**
 * i2c_set_bus_speed - set i2c bus speed
 *  @speed: bus speed (in HZ)
 * This function returns invalid or 0
 */
int i2c_set_bus_speed(unsigned int speed)
{
	switch (speed) {
	case 100000:
		iproc_i2c_set_clk_freq(I2C_SPEED_100KHz);
		break;

	case 400000:
		iproc_i2c_set_clk_freq(I2C_SPEED_400KHz);
		break;

	default:
		return -EINVAL;
	}
	return 0;
}

/**
 * i2c_get_bus_speed - get i2c bus speed
 * This function returns the speed of operation in Hz
 */
unsigned int i2c_get_bus_speed(void)
{
	unsigned int regval;
	unsigned int val;

	regval = iproc_i2c_reg_read(CCB_SMB_TIMGCFG_REG);
	val = GETREGFLDVAL(regval, CCB_SMB_TIMGCFG_MODE400_MASK,
			   CCB_SMB_TIMGCFG_MODE400_SHIFT);
	switch (val) {
	case I2C_SPEED_100KHz:
		return 100000;

	case I2C_SPEED_400KHz:
		return 400000;

	default:
		return 0;
	}
	return 0;
}

int i2c_write_mux_ctrl_byte(u8 devaddr, u8 ctrl_byte)
{
	int rc;
	struct iproc_xact_info info;
	u8 value = ctrl_byte;

	devaddr <<= 1;

	info.cmd_valid = 0;
	info.command = 0;
	info.data = &value;
	info.size = 1;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_WR_BYTE;
	/* Refer to i2c_smbus_write_byte params passed. */
	rc = iproc_i2c_data_send(devaddr, &info);

	if (rc < 0) {
#ifdef BCM_I2C_DIAG
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Write", devaddr);
#endif
		return -EREMOTEIO;
	}

	return 0;
}

int i2c_read_mux_ctrl_byte(u8 devaddr)
{
	int rc;
	struct iproc_xact_info info;
	unsigned int num_bytes_read = 0;
	unsigned int value = 0;

	devaddr <<= 1;

	info.cmd_valid = 0;
	info.data = (unsigned char *)&value;
	info.size = 1;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_RD_BYTE;
	/* Refer to i2c_smbus_read_byte for params passed. */
	rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

	if (rc < 0) {
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Read", devaddr);
		return -EREMOTEIO;
	}
	printf("devaddr:0x%x, read value:0x%x numbytes:%d\n", devaddr, value,
	       num_bytes_read);
	return value;
}

int i2c_write_ctrl_bytes(u8 devaddr, u8 *data, u8 size)
{
	int rc;
	struct iproc_xact_info info;

	devaddr <<= 1;

	info.cmd_valid = 0;
	info.command = 0;
	info.data = data;
	info.size = size;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_WR_BYTE;
	/* Refer to i2c_smbus_write_byte params passed. */
	rc = iproc_i2c_data_send(devaddr, &info);

	if (rc < 0) {
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Write", devaddr);
		return -EREMOTEIO;
	}
	return 0;
}

/*
 * I2C read/write routines for PCA9673 (16-bit IO expander).
 * 2 bytes in a transaction for port 0 and port 1 respectively
 * before generating stop condition.
 */

int i2c_pca9673_read(u8 devaddr, u8 *value)
{
	int rc;
	struct iproc_xact_info info;
	unsigned int num_bytes_read = 0;

	devaddr <<= 1;

	info.cmd_valid = 0;
	info.command = 0;
	info.data = value;
	info.size = 2;
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_BLK_RD;
	/* Refer to i2c_smbus_read_byte for params passed. */
	rc = iproc_i2c_data_recv(devaddr, &info, &num_bytes_read);

	if (rc < 0) {
#ifdef BCM_I2C_DIAG
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Read", devaddr);
#endif
		return -EREMOTEIO;
	}
	return 0;
}

int i2c_pca9673_write(u8 devaddr, u8 *buffer)
{
	int rc;
	struct iproc_xact_info info;

	devaddr <<= 1;

	info.cmd_valid = 0;	/*No command required */
	info.command = 0;
	info.data = buffer;
	info.size = 2;		/*For PCA9673 2-bytes in a write for port 0 and 1. */
	info.flags = 0;
	info.smb_proto = SMBUS_PROT_BLK_WR;
	/* Refer to i2c_smbus_write_byte params passed. */
	rc = iproc_i2c_data_send(devaddr, &info);

	if (rc < 0) {
#ifdef BCM_I2C_DIAG
		printf("%s: %s error accessing device 0x%X",
		       __func__, "Write", devaddr);
#endif
		return -EREMOTEIO;
	}
	return 0;
}
