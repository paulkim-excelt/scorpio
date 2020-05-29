/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <configs/bcm_northstar2.h>
#include <asm/arch/ns2_nvram.h>
#include <asm/arch/socregs.h>
#include <asm/arch/bcm_mdio.h>
#include "ns2_nvram_store.h"

#define CRMU_IPROC_MBOX0_REG (0x6502402c)
#define CRMU_IPROC_MBOX1_REG (0x65024030)
#define IPROC_CRMU_MBOX0_REG (0x65024024)
#define IPROC_CRMU_MBOX1_REG (0x65024028)

enum avs_pvt {
	PVT_TEMPERATURE = 0,	/* Bit 0 - Temperature measurement */
	PVT_0P85V_0 = 1,	/* Bit 1 - Voltage 0p85V<0> measurement */
	PVT_0P85V_1 = 2,	/* Bit 2 - Voltage 0p85V<1> measurement */
	PVT_1V_0 = 3,		/* Bit 3 - Voltage 1V<0> measurement */
	PVT_1V_1 = 4,		/* Bit 4 - Voltage 1V<1> measurement */
	PVT_1P8V = 5,		/* Bit 5 - Voltage 1p8V measurement */
	PVT_3P3V = 6,		/* Bit 6 - Voltage 3p3V measurement */
	PVT_TESTMODE = 7	/* Bit 7 - Testmode measurement */
};

enum m0_mbox_opcode {
	MBOX_SET_POWER_MAP = 0,	/* set power domain map */
	MBOX_ENABLE_SYS_AVS,	/* enable/disable AVS for a power domain */
	MBOX_SET_FREQ_MAP,	/* set system frequency */
	MBOX_PREDICT_AVS,	/* find Vavs for the given operation point */
	MBOX_CONFIG_PLL_VCO,	/* set PLL Vco */
	MBOX_CONFIG_PLL_MDIV,	/* set PLL Mdiv */
	MBOX_RESET_BLOCK,	/* trigger a CRMU reset condition L0/L1/L2/L3*/
	MBOX_RESET_PROCESSOR,	/* enable/disable a processor */
	MBOX_ENABLE_POWER,	/* power to a particular supply */
	MBOX_CONFIG_WDT,	/* enable/disable Watchdog timer */
	MBOX_CONFIG_BIMC,	/* enable/disable Chip-wide ECC */
	MBOX_POLL_BIMC,		/* check for ECC failure */
	MBOX_GET_PVT_TEMP,	/* get temperature from AVS PVT monitor */
	MBOX_GET_PVT_PROCESS,	/* get process corner from AVS PVT monitor */
	MBOX_GET_PVT_VOLTAGE,	/* get PD_SYS domain voltage from AVS PVT mon */
	MBOX_GET_CENTRAL_RO_FREQ,	/* get freq of AVS central ring osc */
	MBOX_GET_REMOTE_RO_FREQ,	/* get freq of AVS remote ring osc */
	MBOX_GET_MIN_RO_FREQ,	/* get minimu AVS ring oscillator frequency */
	MBOX_GET_MAX_RO_FREQ,	/* get maximum AVS ring oscillator frequency */
	MBOX_END
};

static uint reg_rd(ulong reg_addr)
{
	unsigned int val;

	val = __raw_readl((void *)(reg_addr));
	return cpu_to_le32(val);
}

static void reg_wr(ulong reg_addr, uint value)
{
	__raw_writel(value, (void *)(reg_addr));
}

static int get_temp(int *milli_celsius)
{
	int i;

	reg_wr(CRMU_IPROC_MBOX0_REG, 0xffffffff);
	reg_wr(IPROC_CRMU_MBOX0_REG, MBOX_GET_PVT_TEMP);
	reg_wr(IPROC_CRMU_MBOX1_REG, 0);

	for (i = 0; i < 1000; i++) {
		if (reg_rd(CRMU_IPROC_MBOX0_REG)) {
			udelay(1000);
		} else {
			*milli_celsius = reg_rd(CRMU_IPROC_MBOX1_REG);
			return 1;
		}
	}

	return 0;
}

static int get_voltage(uint domain, uint *milli_volts)
{
	int i;

	reg_wr(CRMU_IPROC_MBOX0_REG, 0xffffffff);
	reg_wr(IPROC_CRMU_MBOX0_REG, MBOX_GET_PVT_VOLTAGE);
	reg_wr(IPROC_CRMU_MBOX1_REG, domain);

	for (i = 0; i < 1000; i++) {
		if (reg_rd(CRMU_IPROC_MBOX0_REG)) {
			udelay(1000);
		} else {
			*milli_volts = reg_rd(CRMU_IPROC_MBOX1_REG);
			return 1;
		}
	}

	return 0;
}

static int do_get_pvt(cmd_tbl_t *cmdtp, int flag, int argc, char *const argv[])
{
	int rcode = 0;
	uint voltage = 0;
	int celsius = 0;
	int loop_count = 0, count = 0;

	if (argc != 2)
		return CMD_RET_USAGE;

	count = simple_strtoul(argv[1], NULL, 16);

	while (1) {
		if (get_temp(&celsius)) {
			/* Milli celsius to celsius with round up*/
			celsius = (celsius + 500) / 1000;
		} else {
			printf("Failed to get temperature\n");
		}

		if (get_voltage(PVT_1V_0, &voltage)) {
			/* 1/10 milli volts to volts with round up*/
			voltage = (voltage + 55) / 10;
		} else {
			printf("Failed to get voltage\n");
		}

		printf("---- V: %d.%02dV | T: %dC\n",
		       voltage / 1000, (voltage % 1000) / 10, celsius);

		/* delay 1 second */
		udelay(1000000);

		loop_count++;
		if (loop_count > count)
			break;
	}

	return rcode;
}

#define set_bits_32(in, out, parm)  set_reg_32(in, out, parm##_L, parm##_R,    \
						 parm##_WIDTH)

static void set_reg_32(u32 *in, u32 value,
		       u32 mask_left, u32 mask_right, u32 width)
{
	u32 value_mask = 0;

	/* Set bits on the "in" pointer using the mask information. */
	/* Create a mask. */
	value_mask = (u32)(~(~0 << width) << mask_right);

	/* Set the bits. */
	*in = ((~value_mask) & (*in)) | (value_mask & (value << mask_right));
}

static void ns2_mdio_write(unsigned int phy_id, unsigned int u_addr,
			   unsigned int u_wr_data)
{
	u32 timeout;
	u32 data;

	/* set the voltage */
	data = 0;
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_SB);
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_OP);
	set_bits_32(&data, 2, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_TA);
	set_bits_32(&data, phy_id,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_PA);
	set_bits_32(&data, u_addr,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_RA);
	set_bits_32(&data, u_wr_data,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_Data);
	reg_wr((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Command_Data), data);
	asm volatile ("dsb sy");

	timeout = 0x3FFFFF;
	data = reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
	while (data & (1 << CDRU_MDIO_Management_Control__CDRU_MDIO_BSY)) {
		data =
		    reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
		timeout--;
		if (!timeout) {
			printf("CDRU MDIO command timeout (a)\n");
			break;
		}
	}

	data = 0;
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_SB);
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_OP);
	set_bits_32(&data, 2, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_TA);
	set_bits_32(&data, phy_id,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_PA);
	set_bits_32(&data, 0, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_RA);
	set_bits_32(&data, 2,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_Data);
	reg_wr((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Command_Data), data);
	asm volatile ("dsb sy");

	timeout = 0x3FFFFF;
	data = reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
	while (data & (1 << CDRU_MDIO_Management_Control__CDRU_MDIO_BSY)) {
		data =
		    reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
		timeout--;
		if (!timeout) {
			printf("CDRU MDIO command timeout (b)\n");
			break;
		}
	}

	data = 0;
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_SB);
	set_bits_32(&data, 1, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_OP);
	set_bits_32(&data, 2, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_TA);
	set_bits_32(&data, phy_id,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_PA);
	set_bits_32(&data, 0, CDRU_MDIO_Management_Command_Data__CDRU_MDIO_RA);
	set_bits_32(&data, 0,
		    CDRU_MDIO_Management_Command_Data__CDRU_MDIO_Data);
	reg_wr((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Command_Data), data);
	asm volatile ("dsb sy");

	timeout = 0x3FFFFF;
	data = reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
	while (data & (1 << CDRU_MDIO_Management_Control__CDRU_MDIO_BSY)) {
		data =
		    reg_rd((SWREG_BSTI_ROOT) + (CDRU_MDIO_Management_Control));
		timeout--;
		if (!timeout) {
			printf("CDRU MDIO command timeout (c)\n");
			break;
		}
	}
}

static int do_set_volt(cmd_tbl_t *cmdtp, int flag, int argc,
		       char *const argv[])
{
	int rcode = 0;
	unsigned long rail, voltage;

	if (argc != 3)
		return CMD_RET_USAGE;

	rail = simple_strtoul(argv[1], NULL, 16);
	voltage = simple_strtoul(argv[2], NULL, 16);

	switch (rail) {
	case 0:
		ns2_mdio_write(1, 6, voltage);
		ns2_mdio_write(3, 6, voltage);
		ns2_mdio_write(4, 6, voltage);
		ns2_mdio_write(6, 6, voltage);
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		ns2_mdio_write(rail, 6, voltage);
		break;
	default:
		printf("rail must be one of: 0(all), 1(core), 2(ddr), ");
		printf("3(ihost0), 4(ihost1), 5(analog), 6(array)\n");
	}

	return rcode;
}

U_BOOT_CMD(pvt, 2, 1, do_get_pvt, "pvt monitor", "count");

U_BOOT_CMD(volt, 3, 1, do_set_volt,
	   "set voltage on a given rail or all of them",
	   "rail value - rail must be 0(all), 1(core), 2(ddr), 3(ihost0), 4(ihost1), 5(analog) or 6(array)");

#ifdef CONFIG_BCM_NS2_CHIMP
/*
 *  Decide whether all characters of a name are printable
 *
 *  This only works for base ASCII; not EBCDIC, unicode, or others. ..
 */
static int name_is_printable(u8 *name, u32 length)
{
	int result = 1;		/* assume it is all printable */
	unsigned int index;

	for (index = 0; index < length; index++)
		if ((name[index] < ' ') || (name[index] > '~'))
			/* this character is not 'printable' ASCII */
			result = 0;
	return result;
}

/*
 *  Names for enumerating the NVRAM flags
 */
const char const *nvram_flag_names[] = NVRAM_FLAG_NAMES;

/*
 *  Perform some commands dealing with NVRAM
 */
static int ns2_nvram_cmd(cmd_tbl_t *cmdtp,
			 int flag, int argc, char *const argv[])
{
	u8 *name;
	u8 *data;
	u32 name_len;
	u32 data_len;
	u32 data_len_max;
	u32 nvram_flags;
	u32 nvram_size;
	u32 nvram_elements;
	u32 nvram_used;
	u32 index;
	unsigned int offset;
	int force = 0;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;
	if (!strcasecmp(argv[1], "open")) {
		if (argc == 3) {
			force = !(strcasecmp(argv[2], "fallback"));
			if (!force) {
				printf("nvram open [fallback]\n  where ");
				printf("brackets indicate their contents ");
				printf("are optional\n  if 'fallback' is ");
				printf("specified, open will try to prefer ");
				printf("the backup copy\n");
				return CMD_RET_FAILURE;
			}
		} else if (argc > 2) {
			printf("nvram open [fallback]\n  where brackets ");
			printf("indicate their contents are optional\n");
			printf("  if 'fallback' is specified, open will ");
			printf("try to prefer the backup copy\n");
			return CMD_RET_FAILURE;
		}
		ret = ns2_nvram_open(force);
		if (ret) {
			printf("Error opening NVRAM: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			printf("NVRAM is now open.\n");
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "close")) {
		if (argc == 3) {
			force = !(strcasecmp(argv[2], "force"));
			if (!force) {
				printf("nvram close [force]\n  where ");
				printf("brackets indicate their contents ");
				printf("are optional\n  if 'force' is ");
				printf("specified, the close is forced, ");
				printf("otherwise it is normal\n");
				return CMD_RET_FAILURE;
			}
		} else if (argc > 2) {
			printf("nvram close [force]\n  where brackets ");
			printf("indicate their contents are optional\n");
			printf("  if 'force' is specified, the close is ");
			printf("forced, otherwise it is normal\n");
			return CMD_RET_FAILURE;
		}
		ret = ns2_nvram_close(force);
		if (ret) {
			printf("Error closing NVRAM: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			printf("NVRAM is now closed\n");
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "save")) {
		if (argc != 2) {
			printf("nvram save\n"
			       "  takes no additional arguments\n");
			return CMD_RET_FAILURE;
		}
		ret = ns2_nvram_commit();
		if (ret)
			return CMD_RET_FAILURE;
		else
			return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "info")) {
		if (argc != 2) {
			printf("nvram info\n"
			       "  takes no additional arguments\n");
			return CMD_RET_FAILURE;
		}
		ret = ns2_nvram_info_get(&nvram_flags,
					 &nvram_size,
					 &nvram_elements, &nvram_used);
		if (ret) {
			printf("Error getting NVRAM info: %d\n", ret);
			return CMD_RET_FAILURE;
		}
#if NS2_NVRAM_HAS_BACKUP
		printf("NVRAM located in %s, pri=%08X, sec=%08X, %u bytes\n",
		       NS2_NVRAM_BACKING_STORE,
		       NS2_NVRAM_PRIMARY_OFFSET,
		       NS2_NVRAM_BACKUP_OFFSET, NS2_NVRAM_SIZE);
#else /* NS2_NVRAM_HAS_BACKUP */
		printf
		    ("NVRAM located in %s, pri=%08X, sec=<<none>>, %u bytes\n",
		     NS2_NVRAM_BACKING_STORE, NS2_NVRAM_PRIMARY_OFFSET,
		     NS2_NVRAM_SIZE);
#endif /* NS2_NVRAM_HAS_BACKUP */
		printf("NVRAM status flags:\n");
		for (index = 0, offset = 0; index < 32; index++) {
			if (nvram_flags & (1 << index)) {
				printf("  %s", nvram_flag_names[index]);
				offset++;
				if ((offset & 3) == 0)
					printf("\n");
			}
		}
		if (offset) {
			/* printed at least one status flag */
			printf("\n");
		} else {
			/* printed no status flags */
			printf("  (none)\n");
		}
		printf("Currently, %u element%s use%s %u bytes of %u\n",
		       nvram_elements,
		       (nvram_elements != 1) ? "s" : "",
		       (nvram_elements == 1) ? "s" : "",
		       nvram_used, nvram_size);
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "list")) {
		u8 *name_buff = NULL;
		u8 *temp;
		u32 name_length;
		u32 curr_length = 0;
		u32 data_length;

		if (argc != 2) {
			printf("nvram list\n"
			       "  takes no additional arguments\n");
			return CMD_RET_FAILURE;
		}
		ret = ns2_nvram_info_get(&nvram_flags,
					 &nvram_size,
					 &nvram_elements, &nvram_used);
		if (ret) {
			printf("Error %d getting nvram information\n", ret);
			return CMD_RET_FAILURE;
		}
		printf("NVRAM contains %u element%s.\n",
		       nvram_elements, (nvram_elements != 1) ? "s" : "");
		for (index = 0; index < nvram_elements; index++) {
			/* find out how long this entry's name is */
			ret = ns2_nvram_element_get_by_index(index,
							     NULL,
							     0,
							     &name_length,
							     NULL,
							     0, &data_length);
			if (ret) {
				printf("Error %d getting element %u info\n",
				       ret, index);
				break;
			}
			if (name_length > curr_length) {
				temp = realloc(name_buff, name_length + 1);
				if (!temp) {
					printf("Unable to allocate memory ");
					printf("for element names\n");
					break;
				}
				name_buff = temp;
				curr_length = name_length;
			}
			ret = ns2_nvram_element_get_by_index(index,
							     name_buff,
							     curr_length,
							     &name_length,
							     NULL, 0, NULL);
			if (ret) {
				printf("Error %d getting element %u name\n",
				       ret, index);
				break;
			}
			printf("  Elem #%u: ", index);
			if (name_is_printable(name_buff, name_length)) {
				printf("\"");
				for (offset = 0; offset < name_length; offset++)
					printf("%c", name_buff[offset]);
				printf("\",");
			} else {
				for (offset = 0;
				     offset < name_length;
				     offset++) {
					if ((offset & 0xF) == 0)
						printf("\n   ");
					printf(" 0x%02X", name_buff[offset]);
				}
				printf("\n    name %u bytes, data",
				       name_length);
			}
			printf(" %u bytes\n", data_length);
		}
		if (name_buff) {
			free(name_buff);
			name_buff = NULL;
		}
		if (ret)
			return CMD_RET_FAILURE;
		else
			return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "set")) {
		if (argc != 5) {
			printf("nvram set <elemname> <data_addr> <data_length>");
			printf("\n  set element <elemname> (ASCII string) using ");
			printf("memory at <address> for\n  <data_length> bytes ");
			printf("as the value.\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)argv[2];
		name_len = strlen(argv[2]);
		data = (u8 *)simple_strtoull(argv[3], NULL, 10);
		data_len = simple_strtoul(argv[4], NULL, 10);
		printf("Setting \"%s\" to memory contents ", argv[2]);
		printf("at %016llX for %u bytes\n",
		       (u64)data, data_len);
		ret = ns2_nvram_element_set(name, name_len, data, data_len);
		if (ret) {
			printf("Unable to set element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "setbin")) {
		if (argc != 6) {
			printf("nvram setbin <name_addr> <name_length> ");
			printf("<data_addr> <data_length>\n  set element ");
			printf("specified by <name_length> bytes at ");
			printf("<name_addr> so its\n  value is specified ");
			printf("by <data_length> bytes at <data_addr>.\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)simple_strtoull(argv[2], NULL, 10);
		name_len = simple_strtoul(argv[3], NULL, 10);
		data = (u8 *)simple_strtoull(argv[4], NULL, 10);
		data_len = simple_strtoul(argv[5], NULL, 10);
		printf("Setting (name at %016llX, length %u)",
		       (u64)name, name_len);
		printf(" to memory contents at %016llX for %u bytes\n",
		       (u64)data, data_len_max);
		ret = ns2_nvram_element_set(name, name_len, data, data_len);
		if (ret) {
			printf("Unable to set element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "get")) {
		if (argc != 5) {
			printf("nvram get <elemname> <data_addr>");
			printf("<max_data_length>\n  get element");
			printf("<elemname> (ASCII string) using");
			printf("memory at <address> for\n  up to");
			printf(" <max_length> bytes as the value buffer.");
			printf("  You can use the data\n  directly or");
			printf("display it with 'md' commands.\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)argv[2];
		name_len = strlen(argv[2]);
		data = (u8 *)simple_strtoull(argv[3], NULL, 10);
		data_len_max = simple_strtoul(argv[4], NULL, 10);
		printf
		    ("Getting \"%s\" to memory at %016llX for up to %u bytes\n",
		     argv[2], (u64)data, data_len_max);
		ret =
		    ns2_nvram_element_get(name, name_len, data, data_len_max,
					  &data_len);
		if (ret) {
			printf("Unable to get element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			printf("Placed %u bytes into memory at %016llX\n",
			       (data_len_max > data_len) ?
			       data_len : data_len_max, (u64)data);
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "getbin")) {
		if (argc != 6) {
			printf("nvram getbin <name_addr> <name_length>");
			printf(" <data_addr> <max_data_length>\n");
			printf("  get element specified by <name_length>");
			printf(" bytes at <name_addr> so its\n");
			printf("  value is placed at <data_addr> for up");
			printf(" to <max_data_length> bytes.\n");
			printf("  You can use the value directly or display");
			printf(" it with the 'md' commands.\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)simple_strtoull(argv[2], NULL, 10);
		name_len = simple_strtoul(argv[3], NULL, 10);
		data = (u8 *)simple_strtoull(argv[4], NULL, 10);
		data_len_max = simple_strtoul(argv[5], NULL, 10);
		printf("Getting (name at %016llX, length %u)",
		       (u64)name, name_len);
		printf(" to memory at %016llX for up to %u bytes\n",
		       (u64)data, data_len_max);
		ret =
		    ns2_nvram_element_get(name, name_len, data, data_len_max,
					  &data_len);
		if (ret) {
			printf("Unable to get element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			printf("Placed %u bytes into memory at %016llX\n",
			       (data_len_max > data_len) ?
			       data_len : data_len_max, (u64)data);
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "getidx")) {
		if (argc != 5) {
			printf("nvram getidx <elem_index> <data_addr>");
			printf(" <max_data_length>\n");
			printf(" get element with index <index> using memory");
			printf(" at <address> for up to\n");
			printf(" <max_length> bytes as the value buffer. You");
			printf(" can use the data directly\n");
			printf(" or display it with 'md' commands.\n");
			printf(" WARNING: The index of a particular element");
			printf(" can change as it is updated\n");
			printf(" or other elements are updated or added.\n");
			return CMD_RET_FAILURE;
		}
		index = simple_strtoul(argv[2], NULL, 10);
		data = (u8 *)simple_strtoull(argv[3], NULL, 10);
		data_len_max = simple_strtoul(argv[4], NULL, 10);
		printf("Getting index %u to memory at %016llX for up to");
		printf(" %u bytes\n", index, (u64)data, data_len_max);
		ret =
		    ns2_nvram_element_get_by_index(index, NULL, 0, NULL, data,
						   data_len_max, &data_len);
		if (ret) {
			printf("Unable to get element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			printf("Placed %u bytes into memory at %016llX\n",
			       (data_len_max > data_len) ?
			       data_len : data_len_max, (u64)data);
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "unset")) {
		if (argc != 3) {
			printf("nvram unset <elemname>\n"
			       "  remove element <elemname> (ASCII string) from NVRAM\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)argv[2];
		name_len = strlen(argv[2]);
		printf("Removing \"%s\" from NVRAM\n", argv[2]);
		ret = ns2_nvram_element_unset(name, name_len);
		if (ret) {
			printf("Unable to remove element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			return CMD_RET_SUCCESS;
		}
	} else if (!strcasecmp(argv[1], "unsetbin")) {
		if (argc != 4) {
			printf("nvram unsetbin <name_addr> <name_length>\n"
			       "  remove element specified by <name_length> bytes at <name_addr>\n");
			return CMD_RET_FAILURE;
		}
		name = (u8 *)simple_strtoull(argv[2], NULL, 10);
		name_len = simple_strtoul(argv[3], NULL, 10);
		printf("Removing (name at %016llX, length %u) from NVRAM\n",
		       (u64)name, name_len);
		ret = ns2_nvram_element_unset(name, name_len);
		if (ret) {
			printf("Unable to set element: %d\n", ret);
			return CMD_RET_FAILURE;
		} else {
			return CMD_RET_SUCCESS;
		}
	}
	return CMD_RET_USAGE;
}

U_BOOT_CMD(nvram,
	   20,
	   0,
	   ns2_nvram_cmd,
	   "NVRAM subsystem",
	   "subcommands\n"
	   "  open [fallback] - open NVRAM for access\n"
	   "  close [force] - close NVRAM for access\n"
	   "  save - commit current NVRAM values to backing store\n"
	   "  info - dump NVRAM information\n"
	   "  list - list elements in NVRAM\n"
	   "  get <elemname> <address> <maxlen> - get an element from NVRAM\n"
	   "  getbin <nameaddr> <namelen> <addr> <len> - get an element from NVRAM\n"
	   "  getidx <index> <address> <maxlen> - get elem by index from NVRAM\n"
	   "  set <elemname> <address> <length> - set an element to NVRAM\n"
	   "  setbin <nameaddr> <namelen> <addr> <len> - set an element to NVRAM\n"
	   "  unset <elemname> - remove an element from NVRAM\n"
	   "  unsetbin <nameaddr> <namelen> - remove an element from NVRAM\n"
	   "Individual commands that have arguments offer some help if you do not\n"
	   "provide their arguments.  Changes are not automatically committed to\n"
	   "backing store; this is done with the save subcommand.  If you want to\n"
	   "revert changes since the last save, close and re-open.\n"
	   "Index is not stable and may change on updates; getidx should only be used\n"
	   "for enumerating elements and not for finding particular ones.\n");
#endif

/*
 *  Perform some commands dealing with MDIO
 */
#define SDI \
"Device: PCIE0, PCIE1, USB3, USB2A, USB2B, USB2D, SATA0, SATA1, EAGLE, EXTPHY"
static int ns2_mdio_cmd(cmd_tbl_t *cmdtp, int flag, int argc,
			char *const argv[])
{
	u16 val;
	u32 reg;

	if (argc < 2)
		return CMD_RET_USAGE;
	if (!strcasecmp(argv[1], "read")) {
		if (argc != 4) {
			printf("mdio read <device> <register>\n");
			printf("  Displays 16b value of MDIO <register>");
			printf(" on <device>\n");
			printf("  " SDI "\n");
			return CMD_RET_FAILURE;
		}
		reg = simple_strtoul(argv[3], NULL, 10);
		if (!strcasecmp(argv[2], "PCIE0")) {
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEA_BUSID,
				       PCIEA_PHYID, 0x1F, (reg & 0xFFE0));
			val = bcm_mdio_read(INTERNAL, CLAUS22, PCIEA_BUSID,
					    PCIEA_PHYID, (reg & 0x1F));
		} else if (!strcasecmp(argv[2], "PCIE1")) {
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEB_BUSID,
				       PCIEB_PHYID, 0x1F, (reg & 0xFFE0));
			val = bcm_mdio_read(INTERNAL, CLAUS22, PCIEB_BUSID,
					    PCIEB_PHYID, (reg & 0x1F));
		} else if (!strcasecmp(argv[2], "USB3")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, USB3_SS0_BUSID,
					    USB3_SS0_PHYID, reg);
		} else if (!strcasecmp(argv[2], "USB2A")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, USB2_HS0_BUSID,
					    USB2_HS0_PHYID, reg);
		} else if (!strcasecmp(argv[2], "USB2B")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, USB2_HS1_BUSID,
					    USB2_HS1_PHYID, reg);
		} else if (!strcasecmp(argv[2], "USB2D")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, USB2_DRD_BUSID,
					    USB2_DRD_PHYID, reg);
		} else if (!strcasecmp(argv[2], "SATA0")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, SATA_BUSID,
					    SATA_PORT0_PHYID, reg);
		} else if (!strcasecmp(argv[2], "SATA1")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, SATA_BUSID,
					    SATA_PORT1_PHYID, reg);
		} else if (!strcasecmp(argv[2], "EAGLE")) {
			val = bcm_mdio_read(INTERNAL, CLAUS22, EAGLE_BUSID,
					    EAGLE_PHYID, reg);
		} else if (!strcasecmp(argv[2], "EXTPHY")) {
			val = bcm_mdio_read(EXTERNAL, CLAUS22, PHY54810_BUSID,
					    PHY54810_PHYID, reg);
		} else {
			printf("mdio read <device> <register>\n");
			printf("  Displays 16b value of MDIO <register>");
			printf(" on <device>\n");
			printf("  " SDI "\n");
			return CMD_RET_FAILURE;
		}
		printf("%s MDIO register %u value is %u (0x%04X)\n",
		       argv[2], reg, val, val);
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "write")) {
		if (argc != 5) {
			printf("mdio write <device> <register> <value>\n");
			printf("  Writes 16b <value> to MDIO <register>");
			printf(" on <device>\n");
			printf("  " SDI "\n");
			return CMD_RET_FAILURE;
		}
		reg = simple_strtoul(argv[3], NULL, 10);
		val = simple_strtoul(argv[4], NULL, 10);
		if (!strcasecmp(argv[2], "PCIE0")) {
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEA_BUSID,
				       PCIEA_PHYID, 0x1F, (reg & 0xFFE0));
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEA_BUSID,
				       PCIEA_PHYID, (reg & 0x1F) , val);
		} else if (!strcasecmp(argv[2], "PCIE1")) {
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEB_BUSID,
				       PCIEB_PHYID, 0x1F, (reg & 0xFFE0));
			bcm_mdio_write(INTERNAL, CLAUS22, PCIEB_BUSID,
				       PCIEB_PHYID, (reg & 0x1F), val);
		} else if (!strcasecmp(argv[2], "USB3")) {
			bcm_mdio_write(INTERNAL, CLAUS22, USB3_SS0_BUSID,
				       USB3_SS0_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "USB2A")) {
			bcm_mdio_write(INTERNAL, CLAUS22, USB2_HS0_BUSID,
				       USB2_HS0_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "USB2B")) {
			bcm_mdio_write(INTERNAL, CLAUS22, USB2_HS1_BUSID,
				       USB2_HS1_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "USB2D")) {
			bcm_mdio_write(INTERNAL, CLAUS22, USB2_DRD_BUSID,
				       USB2_DRD_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "SATA0")) {
			bcm_mdio_write(INTERNAL, CLAUS22, SATA_BUSID,
				       SATA_PORT0_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "SATA1")) {
			bcm_mdio_write(INTERNAL, CLAUS22, SATA_BUSID,
				       SATA_PORT1_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "EAGLE")) {
			bcm_mdio_write(INTERNAL, CLAUS22, EAGLE_BUSID,
				       EAGLE_PHYID, reg, val);
		} else if (!strcasecmp(argv[2], "EXTPHY")) {
			bcm_mdio_write(EXTERNAL, CLAUS22, PHY54810_BUSID,
				       PHY54810_PHYID, reg, val);
		} else {
			printf("mdio write <device> <register> <value>\n");
			printf("  Writes 16b <value> to MDIO <register>  on <device>\n");
			printf("  " SDI "\n");
			return CMD_RET_FAILURE;
		}
		printf("Wrote %d (0x%04X) to %s MDIO register %u\n",
		       val, val, argv[2], reg);
		return CMD_RET_SUCCESS;
	} else {
		return CMD_RET_USAGE;
	}
}

U_BOOT_CMD(mdio,
	   20,
	   0,
	   ns2_mdio_cmd,
	   "MDIO access",
	   "subcommands\n"
	   "  read <device> <register> - read MDIO register <register> on device <device>\n"
	   "  write <device> <register> <value> - write <value> to <register> on <device>\n"
	   SDI "\n");


#define PCIELIST \
"Device: PCIE0, PCIE1"
static int ns2_pcie_config(cmd_tbl_t *cmdtp, int flag, int argc,
			char *const argv[])
{
	u32 val;
	u32 reg;
	uint32_t *PAXB_CONFIG_IND_ADDR;
	uint32_t *PAXB_CONFIG_IND_DATA;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcasecmp(argv[2], "PCIE0") || !strcasecmp(argv[2], "0")) {
		printf("Targeting PCIE0\n");
		PAXB_CONFIG_IND_ADDR = 0x20020120;
		PAXB_CONFIG_IND_DATA = 0x20020124;
	} else if (!strcasecmp(argv[2], "PCIE1") || !strcasecmp(argv[2], "1")) {
		printf("Targeting PCIE1\n");
		PAXB_CONFIG_IND_ADDR = 0x50020120;
		PAXB_CONFIG_IND_DATA = 0x50020124;
	} else {
		return CMD_RET_USAGE;
	}

	reg = simple_strtoul(argv[3], NULL, 10);
	*PAXB_CONFIG_IND_ADDR = reg;

	if (!strcasecmp(argv[1], "write")) {
		val = simple_strtoul(argv[4], NULL, 10);
		*PAXB_CONFIG_IND_DATA = val;
		return CMD_RET_SUCCESS;
	} else if (!strcasecmp(argv[1], "read")) {
		printf("%08x: %08x\n",  reg, *PAXB_CONFIG_IND_DATA);
		return CMD_RET_SUCCESS;
	} else {
		return CMD_RET_USAGE;
	}
}

U_BOOT_CMD(pcie,
	   4,
	   0,
	   ns2_pcie_config,
	   "PCIe access",
	   "subcommands\n"
	   "  read <device> <register> - read PCIe register <register> on device <device>\n"
	   "  write <device> <register> <value> - write <value> to <register> on <device>\n"
	   PCIELIST "\n");
