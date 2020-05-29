/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <malloc.h>
#include <errno.h>
#include <netdev.h>
#include <asm/io.h>
#include <linux/compiler.h>
#include <configs/bcm_northstar2.h>
#include <asm/arch/socregs.h>
#include <spi.h>
#include <spi_flash.h>
#include <i2c.h>
#include <asm/arch/ns2_nvram.h>

/*****************************************************************************
 *
 * Various definitions
 *
 */

#define CHIMP_FLASH_OFFSET 0x50000
#define CHIMP_FLASH_SIZE 0x30000
#define CHIMP_FLASH_EXTRA_OFFSET 0x40000
#define CHIMP_FLASH_EXTRA_SIZE 0x10000

#define CHIMP_HEADR_MAGIC 0xdeadbeef
#define STATUS_JUMP_TO_FW_ENTRY 0x6000000
#define CHIMP_STAGE_1_OFFSET	0x07000
#define CHIMP_STAGE_2_OFFSET	0x14000
#define CHIMP_SOC_MEMORY_OFFSET	0x81000000
#define CHIMP_SOC_CONFIG_OFFSET 0x811F7000
#define CHIMP_A0_CONFIG_OFFSET  0xB000
#define CHIMP_VERSION_OFFSET	0x18

#define NS2_CHIMP_SFPCTL_I2C_BUS 1
#define NS2_CHIMP_SFPCTL_I2C_MUX_DEV 0x70
#define NS2_CHIMP_SFPCTL_I2C_MUX_REG 0X00
#define NS2_CHIMP_SFPCTL_I2C_MUX_REG_LEN 0x01
#define NS2_CHIMP_SFPCTL_I2C_MUX_VAL 0x07
#define NS2_CHIMP_SFPCTL_I2C_MUX_VAL_LEN 0x01
#define NS2_CHIMP_SFPCTL_I2C_CTRL_DEV 0x24
#define NS2_CHIMP_SFPCTL_I2C_CTRL_REG 0xFF
#define NS2_CHIMP_SFPCTL_I2C_CTRL_REG_LEN 0x01
#define NS2_CHIMP_SFPCTL_I2C_CTRL_VAL 0X00
#define NS2_CHIMP_SFPCTL_I2C_CTRL_VAL_LEN 0X01

#ifdef CONFIG_BCM_NS2_CUPS_DDR4
#define MASK_IOMUX_PAD_FUNCTION_1_BIT20_OFF 0xFFFFBFFF
#define MASK_IOMUX_PAD_FUNCTION_1_BIT20_ON 0x00100000
#endif

/*
 *  Support for debugging output (very verbose)
 */
#undef DEBUG_DETAIL
#ifdef DEBUG_DETAIL
#define DEBUG_OUT(_info) printf _info
#else				/* def DEBUG_DETAIL */
#define DEBUG_OUT(_info)
#endif				/* def DEBUG_DETAIL */

/*
 *  Support for diagnostic output on errors
 */
#undef DEBUG_DETAIL
#define DEBUG_ERRORS 1
#ifdef DEBUG_ERRORS
#define ERROR_OUT(_info) printf _info
#else				/* def DEBUG_ERRORS */
#define ERROR_OUT(_info)
#endif				/* def DEBUG_ERRORS */

/*
 *  This is used by several functions for SPI flash handling.
 *
 *  It is opened by the one exported function, load_chimp_firmware.
 */
static struct spi_flash *flash;

/*****************************************************************************
 *
 * 'Extra' blob handling
 *
 * At this point, there are two supported types of 'extra' blobs --
 * 'default configuration', and 'opaque data', but this can easily expand.
 *
 */

/*
 *  This describes the add-on blobs that are stored in the 'extra' section.
 *
 *  Each one has one of these, followed by additional data about the particular
 *  blob itself (so each type could have a different header beyond this part).
 */
struct opaque_blob {
	u32 offset;
	u32 length;
	u64 target_address;
};

struct chimp_extra_blob_header_s {
	u32 magic;		/* magic number to ID type */
	u16 version;		/* version */
	u16 reserved0;		/* reserved */
	u32 size;		/* size of the whole thing */
	u32 hdrsize;		/* size of the header only */
};

/*
 *  Verify the CRC32 on an extra blob; returns zero if match, else nonzero.
 *
 *  Assumes the caller has already read the entire blob into memory, starting
 *  with the header and running to the final quadbyte, which is expected to be
 *  the CRC32 for the blob.
 */
static u32 chimp_extra_blob_check(struct chimp_extra_blob_header_s *header)
{
	u32 result;
	u32 crcval;
	u32 *crcptr;

	DEBUG_OUT(("%s[%u]: verify CRC32 on the blob\n", __func__, __LINE__));
	DEBUG_OUT(("%s[%u]: hdr size 0x%x, blob size 0x%x(%u)\n",
		   __func__,
		   __LINE__, header->hdrsize, header->size, header->size));

	crcptr = (u32 *)(&(((u8 *)header)[header->size - sizeof(u32)]));
	crcval = crc32(0, (void *)header, header->size - sizeof(u32));
	result = (crcval - (le32_to_cpu(*crcptr)));

	DEBUG_OUT(("%s[%u]: blob CRC32 is 0x%x with resultant check 0x%x\n",
		   __func__, __LINE__, crcval, result));

	return result;
}

/*
 *  Read the blob at the requested offset in the 'extra' space into memory and
 *  verify its check value.
 *
 *  This will use the provided header buffer as scratchpad -- it will contain
 *  the header for the found item if it is found, but it will otherwise be
 *  overwritten with garbage.
 *
 *  This will allocate a memory cell and load the entire blob into it as part
 *  of checking the CRC32.  If the caller provides a non-NULL 'data' pointer,
 *  the pointer at the end of that will be updated to reflect this cell and the
 *  caller must free the cell; if not, the cell is freed before return.
 *
 *  If an error occurs, no alloc cell will be returned and the header may be
 *  overwritten with garbage.
 */
static int
chimp_extra_blob_read(u32 offset,
		      struct chimp_extra_blob_header_s *header, void **data)
{
	void *temp = NULL;
	int result;
	u32 size;

	if (!flash) {
		ERROR_OUT(("%s[%u]: flash device not opened\n",
			   __func__, __LINE__));
		result = -EIO;
		goto abort;
	}

	if (!header) {
		ERROR_OUT(("%s[%u]: caller must provide header buffer\n",
			   __func__, __LINE__));
		result = -EINVAL;
		goto abort;
	}
	if (offset >
	    (CHIMP_FLASH_EXTRA_SIZE -
	     sizeof(struct chimp_extra_blob_header_s))) {
		ERROR_OUT(("%s[%u]: extra space is %u bytes; offs %u invalid\n",
			   __func__, __LINE__, CHIMP_FLASH_EXTRA_SIZE, offset));
		result = -EINVAL;
		goto abort;
	}

	DEBUG_OUT(("%s[%u]: reading blob header of size %u at offset %u\n",
		   __func__,
		   __LINE__,
		   (int)sizeof(struct chimp_extra_blob_header_s), offset));

	result = spi_flash_read(flash,
				offset + CHIMP_FLASH_EXTRA_OFFSET,
				sizeof(struct chimp_extra_blob_header_s),
				header);

	if (result) {
		ERROR_OUT(("%s[%u]: error %d reading %u at %u in extra space\n",
			   __func__, __LINE__, result, (unsigned int)
			   (sizeof(struct chimp_extra_blob_header_s)), offset));
		goto abort;
	}

	if ((header->magic == 0xFFFFFFFF /* same BE or LE */) ||
	    (header->magic == 0x00000000 /* same BE or LE */)) {
		/* erased space or zeroed space; no more blobs */
		result = -ENOENT;
		goto abort;
	}

	size = le32_to_cpu(header->size);
	DEBUG_OUT(("%s[%u]: blob header magic %u with hdr size of 0x%x\n",
		   __func__, __LINE__, header->magic, size));

	if (size + offset > CHIMP_FLASH_EXTRA_SIZE) {
		ERROR_OUT(("%s[%u]: claims to be %u bytes by only %u left\n",
			   __func__,
			   __LINE__,
			   size, CHIMP_FLASH_EXTRA_SIZE - (offset + size)));

		result = -EIO;
		goto abort;
	}

	DEBUG_OUT(("%s[%u]: reading blob %08X of size %u at offset %u\n",
		   __func__,
		   __LINE__, be32_to_cpu(header->magic), size, offset));

	temp = malloc(size);
	if (!temp) {
		ERROR_OUT(("%s[%u]: unable to allocate %u bytes for data\n",
			   __func__, __LINE__, size));
		result = -ENOMEM;
		goto abort;
	}

	result = spi_flash_read(flash,
				offset + CHIMP_FLASH_EXTRA_OFFSET, size, temp);

	if (result) {
		ERROR_OUT(("%s[%u]: error %d reading %u at %u in extra space\n",
			   __func__, __LINE__, result, size, offset));
		goto abort;
	}

	if (chimp_extra_blob_check(temp)) {
		ERROR_OUT(("%s[%u]: CRC of %u bytes blob at %u is invalid\n",
			   __func__, __LINE__, size, offset));
		result = -EIO;
		goto abort;
	}

 abort:
	if (result) {
		/* clean up if something went wrong */
		if (temp)
			free(temp);
	} else {
		/* return buffer if caller wanted it */
		if (data)
			*data = temp;
		else if (temp)
			free(temp);
	}
	return result;
}

/*
 *  Look for a particular blob by its magic number.
 *
 *  Note that this will read and verify every blob up to the desired one.  This
 *  is to make sure it is not following an invalid pointer or similar.
 *
 *  If the caller provides a location to put the pointer to the buffer, this
 *  will provide the address of a buffer containing the entire requested blob
 *  to the caller.  The entire blob, including the header is included, and the
 *  caller must dispose of the alloc cell when done.  If the caller does not
 *  provide a place to put the pointer, the buffer will be discarded before
 *  this returns.
 *
 *  On error, the pointer at the caller's location is not updated.
 */
static int chimp_extra_blob_find(u32 magic, void **data)
{
	struct chimp_extra_blob_header_s header;
	void *temp = NULL;
	int result;
	u32 offset = 0;

	DEBUG_OUT(("%s[%u]: search extra blobs for %08X\n",
		   __func__, __LINE__, magic));

	do {
		result = chimp_extra_blob_read(offset, &header, &temp);
		if (result)
			break;
		if (be32_to_cpu(header.magic) == magic)
			break;
		offset += le32_to_cpu(header.size);
	} while (1);

	if (result) {
		/* clean up if something went wrong */
		if (temp)
			free(temp);
	} else {
		/* return buffer if caller wanted it */
		if (data)
			*data = temp;
		else if (temp)
			free(temp);
	}
	return result;
}

/*****************************************************************************
 *
 * Configuration handling
 *
 */

/*
 *  In order to express things more compactly, we can omit the lower bits of
 *  the size and offset.  This indicates how many bits we throw out.  Since
 *  everything is based upon quadbytes, values up to two should be pretty safe.
 */
#define CFG_OFFS_SIZE_OMIT 0

/*
 *  The settings are kept by the NVRAM APIs.  So, we need an element name.
 *
 *  Note that since the name is not NUL-terminated, we use character instead of
 *  string quoting.  This avoids encoding the NUL as part of the element name.
 */
const char const *chimp_nvram_name = "ChiMP config";

enum nvm_cfg_sections {
	NVM_CFG_SECTION_BOARD = 0,
	NVM_CFG_SECTION_BOARD_IO,
	NVM_CFG_SECTION_PCIE,
	NVM_CFG_SECTION_FEATURES,
	NVM_CFG_SECTION_LINK_SETTINGS,
	NVM_CFG_SECTION_MGMT,
	NVM_CFG_SECTION_PHY,
	NVM_CFG_SECTION_PRE_BOOT,
	NVM_CFG_SECTION_VF,
	NVM_CFG_SECTION_DUAL_PHY,
	NVM_CFG_SECTION_NPAR,
	NVM_CFG_SECTION_TEMPERATURE_CONTROL,
	NVM_CFG_SECTION_EMBEDDED_BMC,
	NVM_CFG_SECTION_MAX = 32
};

enum nvm_cfg_glob_part_e {
	NVM_CFG_GLOB_PART_SHARED = 0,
	NVM_CFG_GLOB_PART_COUNT	/* MUST BE LAST */
};

enum nvm_cfg_port_part_e {
	NVM_CFG_PORT_PART_PORT = 0,
	NVM_CFG_PORT_PART_COUNT	/* MUST BE LAST */
};

enum nvm_cfg_func_part_e {
	NVM_CFG_FUNC_PART_FUNC = 0,
	NVM_CFG_FUNC_PART_COUNT	/* MUST BE LAST */
};

enum nvm_cfg_path_type_e {
	NVM_CFG_PATH_SHARED = 0,
	NVM_CFG_PATH_PORT,
	NVM_CFG_PATH_FUNC,
	NVM_CFG_PATH_COUNT	/* MUST BE LAST */
};

const char const *path_idx_mode_name[] = {
	"shared",
	"port",
	"function"
};

#define NVM_CFG_HEADER_VERSION 0x0020
#define NVM_CFG_HEADER_MAGIC 0x42636667
struct nvm_cfg_header_s {
	struct chimp_extra_blob_header_s hdr;	/* common header */
	u8 offs_len;		/* number of bits in offsets */
	u8 port_count;		/* number of ports */
	u8 func_count;		/* number of funcs per port */
	u8 reserved1;		/* reserved */
	u8 glob_part_count;	/* number of glob parts below */
	u8 port_part_count;	/* number of port parts below */
	u8 func_part_count;	/* number of func parts below */
	u8 reserved2;		/* reserved */
	u32 parts[1];		/* offs+size for parts */
};

/*
 *  To locate a particular datum in the configuration, we need to know the part
 *  index from above, possibly the port number (0..header.port_count), possibly
 *  the function number, the section (see enum nvm_config_sections), the offset
 *  (which we shall consider to be in quadbytes since that's the granularity in
 *  nvm_cfg.h), the bit offset within that quadbyte, and the number of bits.
 *
 *  Within the indices, bits below NVM_CFG_SECTION_LEN_SHIFT indicate the
 *  offset from the start of the index count to the item (expressed as a number
 *  of bytes), and bits above it indicate the length of the item. Note that
 *  offsets and lengths are in bytes.
 *
 *  Find the part by looking up the offset of the part in question in the
 *  chimp_config_header_s -- shared, ports, funcs.  Note that the offset in
 *  this index is for the first such part and the length is for each one of the
 *  part (there will be one shared, but there will be CHIMP_PORT_COUNT ports
 *  and CHIMP_PORT_COUNT funcs).  For shared, the port number is not used, but
 *  for ports multiply the port number by the length of one of the appropriate
 *  part and add that to the offset in the table, and for funcs, multiply the
 *  func number by the number of ports, add the port number, then multiply that
 *  by the size of each func. That is the base of the part you want.
 *
 *  To find a section in that part is basically the same process, but there can
 *  be only zero or one of a specific section in a given part.  If a section
 *  does not exist in a part, both its offset and length will be zero.
 *  Otherwise, use the offset from the index to find the desired section and
 *  the length from the index to be sure you are not trying to go beyond it.
 *
 *  Get/Set the value by looking the appropriate number of quadbytes in and
 *  masking the value as indicated by the lower bit and bit count.
 *
 *  Because ChiMP firmware appears to expect the data in little endian format
 *  with 8b grains, that is the format of the entire configuration blob,
 *  including the indices.  Our internal tables are in native format, however.
 */
struct nvm_cfg_item_path_s {
	const char const *item_name;
	unsigned int index_mode;
	unsigned int item_part;
	unsigned int item_section;
	u16 item_offset;
	u16 item_lower_bit;
	u16 item_bit_count;
};

typedef struct nvm_cfg_item_path_s nvm_cfg_item_path_t;

#define NVM_CFG_GLOB_ITEM(_name, _desc, _part, _section, _offset, _lsb, _len) \
{	\
	_name, \
	NVM_CFG_PATH_SHARED, \
	_part, \
	_section, \
	_offset, \
	_lsb, \
	_len \
}

#define NVM_CFG_PORT_ITEM(_name, _desc, _part, _section, _offset, _lsb, _len) \
{	\
	_name, \
	NVM_CFG_PATH_PORT, \
	_part, \
	_section, \
	_offset, \
	_lsb, \
	_len \
}

#define NVM_CFG_FUNC_ITEM(_name, _desc, _part, _section, _offset, _lsb, _len) \
{	\
	_name, \
	NVM_CFG_PATH_FUNC, \
	_part, \
	_section, \
	_offset, \
	_lsb, \
	_len \
}

#define NVM_CONFIG_DONE \
{ /* this must be last */ \
	NULL, \
	0, \
	0, \
	0, \
	0, \
	0, \
	0 \
}

/*
 *  This is the callback function for the iterators below.
 *
 *  The index argument indicates the index within the paths list (internal).
 *
 *  The counter argument indicates how many times this has been called during
 *  this particular iteration (so the 'counter'th match for the path type).
 *  This starts with zero.
 *
 *  The path argument points to the current path (so can be used to get the
 *  appropriate value).
 *
 *  The extra argument is the pointer that was provided by the caller to the
 *  particular iterator request.
 *
 *  The name argument points to a string naming the path.
 *
 *  The desc argument points to a string describing the path (this may be an
 *  empty string).
 *
 *  The part argument indicates which part within the global parts is involved.
 *
 *  The section argument indicates which section within that part is involved.
 *
 *  The offset argument indicates the number of quadbytes into the section.
 *
 *  The lsb argument indicates the least significant bit of the value within
 *  the particular quadbyte.
 *
 *  The bits argument indicates the number of bits in the value.
 *
 *  Note that everything after 'extra' is merely informative, intended for
 *  diagnostic or debugging purposes.  To get/set the actual value, a call to
 *  nvm_cfg_item_set/get must be made.
 *
 *  To abort the traversal, return a nonzero result.  Any nonzero result will
 *  be returned by the iterator function.
 */
typedef int (*nvm_cfg_path_iterator_callback) (unsigned int index,
					       unsigned int counter,
					       const nvm_cfg_item_path_t *path,
					       void *extra,
					       const char const *name,
					       unsigned int part,
					       unsigned int section,
					       unsigned int offset,
					       unsigned int lsb,
					       unsigned int bits);

static const nvm_cfg_item_path_t chimp_config_paths[] = {
/*
 *  This section is used anywhere else.  It should contain all of the items
 *  that are available for editing.  Note that anything not in this list will
 *  not be available for editing through these APIs but the current value will
 *  be maintained across edits.
 *
 *  Ideally, this would cover everything in the configuration.  More
 *  practically, customers will probably find themselves removing bits here in
 *  order to ensure settings specific to their devices are not changed.
 */
/* FIXME: add descriptive comments (need to get details from ChiMP team) */
	/* SHARED.BOARD */
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_PORT_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_VERSION",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  1,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_REVSION",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  9,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_HIDE_PORT1",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  17,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_PORT_LAYOUT",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  21,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_DEVIATION_CONFIG",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  25,
			  4),
	/* SHARED.BOARD_IO */
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_FAN_FAILURE_ENFORCEMENT",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_FRU_VPD_PRESENT",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  1,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_FRU_VPD_SELECTION",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  2,
			  3),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_FRU_VPD_SLAVE_ADDR",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  5,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_EXT_THERM_SEN_PRESENT",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  13,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_EXT_THERM_SEN_SEL",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  14,
			  3),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_IO_EXT_THERM_SEN_SLAVE_ADDR",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD_IO,
			  0,
			  17,
			  8),
	/* SHARED.PCIE */
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_PCI_GEN",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  0,
			  2),
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_PCI_ASPM_SUPPORT",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  2,
			  2),
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_PREVENT_PCIE_L1_MENTRY",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  4,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_GEN1_PREENPHASIS",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  5,
			  3),
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_GEN2_PREEMPHASIS",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  8,
			  3),
	NVM_CFG_GLOB_ITEM("SHARED_PCIE_MSIX_MAX_NUM",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  11,
			  10),
	/* SHARED.FEATURES */
	NVM_CFG_GLOB_ITEM("SHARED_FEATURES_LLDP_TRANSMIT_INTERVAL",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  0,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_FEATURES_LLDP_DEVICE_TYPE_ID",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  8,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_FEATURES_WIDE_DCBX_FEATURE",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_FEATURES,
			  1,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_FEATURES_FLR_CAPABILITY",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_FEATURES,
			  1,
			  1,
			  1),
	/* SHARED.MGMT */
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_SMBUS_TIMING",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_SMBUS_ADDRESS",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  1,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_SMBUS_ARB",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  9,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_SMBUS_SELECTIONG",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  10,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_NC_MSI_OVER_RMII",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  18,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_NC_MSI_OVER_SMBUS",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  19,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_NC_MSI_OVER_PCIEVDM",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  20,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_MGMT_SMBUS_TIMING",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  1,
			  0,
			  3),
	NVM_CFG_GLOB_ITEM("SHARED_NCSI_ID_METHOD",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  2,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_NCSI_ID",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_MGMT,
			  2,
			  1,
			  2),
	/* SHARED.PHY */
	NVM_CFG_GLOB_ITEM("SHARED_PHY_NETWORK_PORT_MODE",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  0,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_ENFORCE_PREEMPHASIS_CFG",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  8,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_2_5_VCO",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  24,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_2_5_10_SPARAM_TUNABLE",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  25,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_STRICT_PREAMBLE_CHECK",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  26,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_MAC_LEN_CHECK",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  0,
			  27,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE0_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  0,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE1_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  4,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE2_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  8,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE3_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  12,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE0_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  16,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE1_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  20,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE2_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  24,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE3_SWAP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  1,
			  28,
			  4),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE0_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE1_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  1,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE2_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  2,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_RX_LANE3_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  3,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE0_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  4,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE1_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  5,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE2_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  6,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_TX_LANE3_POL_FLIP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  2,
			  7,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE0_PREEMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  3,
			  0,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE1_PREEMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  3,
			  8,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE2_PREEMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  3,
			  16,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE3_PREEMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  3,
			  24,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE0_AMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  4,
			  0,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE1_AMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  4,
			  8,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE2_AMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  4,
			  16,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_PHY_LANE3_AMP",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PHY,
			  4,
			  24,
			  8),
	/* SHARED.PRE_BOOT */
	NVM_CFG_GLOB_ITEM("SHARED_PRE_BOOT_EXPANSION_ROM_SIZE",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  0,
			  8),
	/* SHARED.VF */
	NVM_CFG_GLOB_ITEM("SHARED_VF_ENABLE_SRIOV",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_VF,
			  0,
			  0,
			  1),
	NVM_CFG_GLOB_ITEM("SHARED_VF_MSI_MX_AUTO_MASK",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_VF,
			  0,
			  1,
			  1),
	/* SHARED.TEMPERATURE_CONTROL */
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_FAN_THRESH",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  0,
			  0,
			  7),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_SHUT_THRESH",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  0,
			  7,
			  7),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_FAN_GPIO",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  0,
			  14,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_SHUT_GPIO",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  0,
			  22,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_TEMP_PERIOD",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  1,
			  0,
			  16),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_TEMP_SMBUS_ADDR",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  1,
			  16,
			  8),
	NVM_CFG_GLOB_ITEM("SHARED_TEMPERATURE_CONTROL_RESERVED",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_TEMPERATURE_CONTROL,
			  2,
			  0,
			  1),
	/* PORT.BOARD */
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_DIS_D0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_DIS_D1",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_DIS_D2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  16,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_DIS_D3",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  24,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_CONS_D0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_CONS_D1",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_CONS_D2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  16,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_POWER_CONS_D3",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  24,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_VENDOR_ID",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  2,
			  0,
			  16),
	NVM_CFG_PORT_ITEM("PORT_BOARD_VENDOR_DEVICE_ID",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  2,
			  16,
			  16),
	NVM_CFG_PORT_ITEM("PORT_BOARD_SUBSYSTEM_VENDOR_ID",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  3,
			  0,
			  16),
	NVM_CFG_PORT_ITEM("PORT_BOARD_SUBSYSTEM_VENDOR_DEVICE_ID",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  3,
			  16,
			  16),
	NVM_CFG_PORT_ITEM("PORT_BOARD_LED_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  4,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_LED_SPEED_0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  4,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_BOARD_LED_SPEED_1",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_BOARD,
			  4,
			  16,
			  8),
	/* PORT.PCIE */
	NVM_CFG_PORT_ITEM("PORT_PCIE_BAR_1_SIZE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  0,
			  4),
	NVM_CFG_PORT_ITEM("PORT_PCIE_BAR_2_SIZE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  4,
			  4),
	NVM_CFG_PORT_ITEM("PORT_PCIE_BAR_3_SIZE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  8,
			  4),
	/* PORT.FEATURES */
	NVM_CFG_PORT_ITEM("PORT_FEATURES_ENABLE_WOL_ON_ACPI_PATTERN",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  0,
			  1),
	NVM_CFG_PORT_ITEM("PORT_FEATURES_MAGIC_PACKET_WOL",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  1,
			  1),
	NVM_CFG_PORT_ITEM("PORT_FEATURES_DCBX_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  2,
			  4),
	NVM_CFG_PORT_ITEM("PORT_FEATURES_MF_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_FEATURES,
			  0,
			  6,
			  5),
	/* PORT.LINK_SETTINGS */
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_SPEED_CAPABILITY_DRV",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  0,
			  0,
			  16),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_SPEED_CAPABILITY_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  0,
			  16,
			  16),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_SPEED_DRV",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  0,
			  4),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_FLOW_CONTROL_DRV",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  4,
			  3),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_SPEED_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  7,
			  4),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_FLOW_CONTROL_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  11,
			  3),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_OPTIC_MDL_VNDR_ENF",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  14,
			  2),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_D3_LINK_SPEED_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  16,
			  4),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_D3_FLOW_CONTROL_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  1,
			  20,
			  3),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_EEE_POWER_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  2,
			  0,
			  2),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_D3_SPEED_CAPABILITY_FW",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  2,
			  2,
			  16),
	NVM_CFG_PORT_ITEM("PORT_LINK_SETTINGS_EEE_POWER_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_LINK_SETTINGS,
			  2,
			  0,
			  2),
	/* PORT.MGMT */
	NVM_CFG_PORT_ITEM("PORT_MGMT_NC_MSI_OVER_RMII__M_DEPRECATED",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  0,
			  1),
	NVM_CFG_PORT_ITEM("PORT_MGMT_NC_MSI_OVER_SMBUS__M_DEPRECATED",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  1,
			  1),
	NVM_CFG_PORT_ITEM("PORT_MGMT_NC_MSI_OVER_PCIEVDM__M_DEPRECATED",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_MGMT,
			  0,
			  2,
			  1),
	/* PORT.PHY */
	NVM_CFG_PORT_ITEM("PORT_PHY_EXTERNAL_PHY_TYPE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  0,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_EXTERNAL_PHY_ADDRESS",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  0,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_SERDES_NET_INTERFACE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  1,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_AN_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  1,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_OPTICAL_MODULE_I2C_SELECTION",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  1,
			  16,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_SELECTION",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  2,
			  0,
			  3),
	NVM_CFG_PORT_ITEM("PORT_PHY_SWAPPED",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  2,
			  3,
			  1),
	NVM_CFG_PORT_ITEM("PORT_PHY_SWAP_PHY_POLARITY",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  3,
			  0,
			  1),
	NVM_CFG_PORT_ITEM("PORT_PHY_PHY_RESET",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  4,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_TX_FAULT",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  4,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_COPPER_PAIR_SWAP",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  5,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_ENABLE_CMS",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  5,
			  8,
			  1),
	NVM_CFG_PORT_ITEM("PORT_PHY_TX_LASER",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  6,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_MOD_ABS",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  6,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_PWR_DIS",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  6,
			  16,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_WRONG_MOD_TYPE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  7,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_CURRENT_FAULT",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  7,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_QSFP_LP_MODE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  7,
			  16,
			  8),
	NVM_CFG_PORT_ITEM("PORT_PHY_QSFP_RESET",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_PHY,
			  7,
			  24,
			  8),
	/* PORT.DUAL_PHY */
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_EXT_PHY2_TYPE",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  0,
			  0,
			  8),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_EXT_PHY2_ADDR",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  0,
			  8,
			  8),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SEC_TX_PRE_EMPH_COEF_0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  1,
			  0,
			  32),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SEC_TX_PRE_EMPH_COEF_1",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  2,
			  0,
			  32),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SEC_RX_EQ_COEF_0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  3,
			  0,
			  32),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SEC_RX_EQ_COEF_1",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  4,
			  0,
			  32),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SPEED_CAPABILITY2_D0",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  5,
			  0,
			  16),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_SPEED_CAPABILITY2_D3",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  5,
			  16,
			  16),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_LINK_SPEED2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  6,
			  0,
			  4),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_FLOW_CONTROL2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  6,
			  4,
			  3),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_FW_LINK_SPEED2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  7,
			  0,
			  4),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_FW_FLOW_CONTROL2",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  7,
			  4,
			  3),
	NVM_CFG_PORT_ITEM("PORT_DUAL_PHY_NET_SERDES_IF",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_DUAL_PHY,
			  8,
			  0,
			  4),
	/* PORT.NPAR */
	NVM_CFG_PORT_ITEM("PORT_NPAR_NUMBER_OF_PARTITIONS_PER_PORT",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_NPAR,
			  0,
			  0,
			  8),
	/* PORT.EMBEDDED_BMC */
	NVM_CFG_PORT_ITEM("PORT_EMBEDDED_BMC_MAC_ADDR_HI",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_EMBEDDED_BMC,
			  0,
			  0,
			  16),
	NVM_CFG_PORT_ITEM("PORT_EMBEDDED_BMC_MAC_ADDR_LO",
			  "",
			  NVM_CFG_PORT_PART_PORT,
			  NVM_CFG_SECTION_EMBEDDED_BMC,
			  1,
			  0,
			  32),
	/* FUNC.BOARD */
	NVM_CFG_FUNC_ITEM("FUNC_BOARD_MAC_ADDR_HI",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_BOARD,
			  0,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_BOARD_MAC_ADDR_LO",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  0,
			  32),
	/* FUNC.PCIE */
	NVM_CFG_FUNC_ITEM("FUNC_PCIE_MF_VENDOR_DEVICE_ID",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PCIE,
			  0,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_PCIE_KCS_DEVICE_ID",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PCIE,
			  1,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_PCIE_KCS_MODE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PCIE,
			  1,
			  16,
			  1),
	NVM_CFG_FUNC_ITEM("FUNC_PCIE_UART_DEVICE_ID",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PCIE,
			  2,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_PCIE_UART_MODE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PCIE,
			  2,
			  16,
			  1),
	/* FUNC.PRE_BOOT */
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  0,
			  1),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_BOOT_TYPE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  1,
			  2),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_DELAY_TIME",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  3,
			  4),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_SETUP_HOT_KEY",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  7,
			  1),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_HIDE_SETUP_PROMPT",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  8,
			  1),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_LINK_SPEED",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  9,
			  4),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_BOOT_RETRY_COUNT",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  13,
			  3),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_BOOT_PROTOCOL",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  16,
			  3),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_FORCE_EXPANSION_ROM_ADV",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  0,
			  19,
			  1),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_VLAN_VALUE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  1,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_PRE_BOOT_MBA_VLAN",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_PRE_BOOT,
			  1,
			  16,
			  1),
	/* FUNC.VF */
	NVM_CFG_FUNC_ITEM("FUNC_VF_PCI_DEVICE_ID",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  0,
			  0,
			  16),
	NVM_CFG_FUNC_ITEM("FUNC_VF_BAR1_SIZE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  0,
			  16,
			  4),
	NVM_CFG_FUNC_ITEM("FUNC_VF_BAR2_SIZE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  0,
			  20,
			  4),
	NVM_CFG_FUNC_ITEM("FUNC_VF_BAR3_SIZE",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  0,
			  24,
			  4),
	NVM_CFG_FUNC_ITEM("FUNC_VF_NUMBER_OF_VFS_PER_PF",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  1,
			  0,
			  8),
	NVM_CFG_FUNC_ITEM("FUNC_VF_NUMBER_OF_VNICS_PER_PF",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  1,
			  8,
			  9),
	NVM_CFG_FUNC_ITEM("FUNC_VF_MSI_MX_VECTORS_PER_VF",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_VF,
			  1,
			  17,
			  10),
	/* FUNC.NPAR */
	NVM_CFG_FUNC_ITEM("FUNC_NPAR_BW_WEIGHT",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_NPAR,
			  0,
			  0,
			  8),
	NVM_CFG_FUNC_ITEM("FUNC_NPAR_BW_MAX",
			  "",
			  NVM_CFG_FUNC_PART_FUNC,
			  NVM_CFG_SECTION_NPAR,
			  0,
			  8,
			  16),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_SPARE_0",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  1,
			  0,
			  32),
	NVM_CFG_GLOB_ITEM("SHARED_BOARD_SPARE_1",
			  "",
			  NVM_CFG_GLOB_PART_SHARED,
			  NVM_CFG_SECTION_BOARD,
			  2,
			  0,
			  32),
/*
 *  The following item must always be last, and it must always be included.
 */
	NVM_CONFIG_DONE		/* MUST BE LAST */
};

/*
 *  Given a pointer to our configuration image, and necessary bits from the
 *  path, provide a pointer to the beginning of the configuration part and the
 *  size of the particular configuraiton part.
 *
 *  This assumes the configuration is valid and a supported version.
 */
#define NCPPGPFNZ "%s[%u]: port %u and function %u must be zero for %s_part\n"
#define NCPPGFNZ "%s[%u]: function %u must be zero for %s_part\n"
#define NCPPGNPA "%s[%u]: offset %08X and size %08X must be quadbyte aligned\n"
static int
nvm_cfg_part_pointer_get(struct nvm_cfg_header_s *header,
			 unsigned int index_mode,
			 unsigned int part,
			 u32 port, u32 function, u32 **address, u32 *size)
{
	u8 *temp;
	u32 next_offs;
	u32 next_size;
	u32 offs_mask;
	u32 size_mask;
	u32 offs_len;
	u32 part_num;

	/* start from the header */
	DEBUG_OUT(("%s[%u]: verify header and arguments\n",
		   __func__, __LINE__));
	DEBUG_OUT(("index_mode %d, part %d, port %d, func %d\n", index_mode,
		   part, port, function));
	offs_len = le32_to_cpu(header->offs_len);
	offs_mask = (1 << offs_len) - 1;
	size_mask = (1 << (32 - offs_len)) - 1;
	DEBUG_OUT(("header offs_len %d, offs_mask 0x%x, size_mask 0x%x\n",
		   offs_len, offs_mask, size_mask));

	temp = (u8 *)header;
	if (port >= header->port_count) {
		ERROR_OUT(("%s[%u]: invalid port number %u\n",
			   __func__, __LINE__, port));
		return -EINVAL;
	}
	if (function >= header->func_count) {
		ERROR_OUT(("%s[%u]: invalid function number %u\n",
			   __func__, __LINE__, function));
		return -EINVAL;
	}
	switch (index_mode) {
	case NVM_CFG_PATH_SHARED:
		DEBUG_OUT(("%s[%u]: find %s_part %u\n",
			   __func__,
			   __LINE__, path_idx_mode_name[index_mode], part));
		if (part >= header->glob_part_count) {
			ERROR_OUT(("%s[%u]: invalid %s_part number %u\n",
				   __func__,
				   __LINE__,
				   path_idx_mode_name[index_mode], part));
			return -EINVAL;
		}
		if (port || function) {
			ERROR_OUT((NCPPGPFNZ,
				   __func__,
				   __LINE__,
				   port,
				   function, path_idx_mode_name[index_mode]));
			return -EINVAL;
		}
		part_num = part;
		break;
	case NVM_CFG_PATH_PORT:
		DEBUG_OUT(("%s[%u]: find %s_part %u, port %u\n",
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode], part, port));
		if (part >= header->port_part_count) {
			ERROR_OUT(("%s[%u]: invalid %s_part number %u\n",
				   __func__,
				   __LINE__,
				   path_idx_mode_name[index_mode], part));
			return -EINVAL;
		}
		if (function) {
			ERROR_OUT((NCPPGFNZ,
				   __func__,
				   __LINE__,
				   function, path_idx_mode_name[index_mode]));
			return -EINVAL;
		}
		part_num = header->glob_part_count + part;
		break;
	case NVM_CFG_PATH_FUNC:
		DEBUG_OUT(("%s[%u]: find %s_part %u, port %u func %u\n",
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode],
			   part, port, function));
		if (part >= header->func_part_count) {
			ERROR_OUT(("%s[%u]: invalid %s_part number %u\n",
				   __func__,
				   __LINE__,
				   path_idx_mode_name[index_mode], part));
			return -EINVAL;
		}
		part_num = (header->glob_part_count +
			    header->port_part_count + part);
		break;
	default:
		ERROR_OUT(("%s[%u]: invalid index_mode %d\n",
			   __func__, __LINE__, index_mode));
		return -EINVAL;
	}
	next_offs = (le32_to_cpu(header->parts[part_num]) & offs_mask);
	next_size = ((le32_to_cpu(header->parts[part_num]) >> offs_len) &
		     size_mask);
	DEBUG_OUT(("%s[%u]: p[%d]: r=%08X, o=%08X, sz=%08X, b4 scaling\n",
		   __func__, __LINE__, part_num,
		   le32_to_cpu(header->parts[part_num]),
		   next_offs, next_size));

	next_offs <<= CFG_OFFS_SIZE_OMIT;
	next_size <<= CFG_OFFS_SIZE_OMIT;
	DEBUG_OUT(("%s[%u]: offs=%08X, size=%08X, after scaling\n",
		   __func__, __LINE__, next_offs, next_size));

	next_offs += (((function * header->port_count) + port) * next_size);
	DEBUG_OUT(("%s[%u]: offs=%08X, size=%08X, after selection\n",
		   __func__, __LINE__, next_offs, next_size));

	if ((next_offs & 3) || (next_size & 3)) {
		ERROR_OUT((NCPPGNPA, __func__, __LINE__, next_offs, next_size));
		return -EINVAL;
	}
	if (address)
		*address = (void *)(&(temp[next_offs]));
	if (size)
		*size = next_size;
	return 0;
}

/*
 *  Given a pointer to and size of a specific part, adjust that pointer so it
 *  points to the requested section and offset.  We still pass all the locator
 *  information in here just for diagnostics; this function only uses the
 *  header, section, and offset for its work.
 *
 *  The initial arguments are kept in the same order as above in hopes of
 *  letting the optimiser do a little better job with stack work.
 */
#define NCIPGNSI "%s[%u]: section %u invalid in %s_part %u port %u func %u\n"
#define NCIPGNSS "%s[%u]: no section %u in %s_part %u of port %u func %u\n"
#define NCIPGOI "%s[%u]: offs %u invalid sec %u %s_part %u port %u func %u\n"
#define NCIPGUA \
	"%s[%u]: %s_part %u port %u func %u sec %u offs %u len %u unaligned\n"
#define NCIPGFI "%s[%u]: %s_part %u port %u func %u sec %u offs %u invalid\n"
#define NCIPGF "%s[%u]: %s_part %u port %u func %u sec %u: offs %08X ln %08X\n"
#define NCIPGFOB "%s[%u]: %s_part %u port %u func %u sec %u offs %u, %p\n"
static int
nvm_cfg_item_pointer_get(struct nvm_cfg_header_s *header,
			 unsigned int index_mode,
			 unsigned int part,
			 u32 port,
			 u32 function,
			 u32 *base,
			 u32 size,
			 unsigned int section,
			 unsigned int offset, u32 **address)
{
	u8 *temp;
	u32 next_offs;
	u32 next_size;
	u32 offs_mask;
	u32 size_mask;

	offs_mask = ((1 << header->offs_len) - 1);
	size_mask = ((1 << (32 - header->offs_len)) - 1);

	DEBUG_OUT(("%s[%u]:\n\tb 0x%x, s %d, p %d, f %d, pr %d,idx %d\n",
		   __func__, __LINE__, base, size, port,
		   function, part, index_mode));

	DEBUG_OUT(("\tsection %d, offs 0x%x, offs_mask 0x%x,sz_mask 0x%x\n",
		   section, offset, offs_mask, size_mask));

	DEBUG_OUT(("\tbase: 0x%08x, 0x%08x, 0x%08x, 0x%08x\n",
		   base[0], base[1], base[2], base[3]));

	/* within each part is the number of sections, then offs+len pairs */
	if (section >= le32_to_cpu(base[0])) {
		/* section is not valid */
		ERROR_OUT((NCIPGNSI,
			   __func__,
			   __LINE__,
			   section,
			   path_idx_mode_name[index_mode],
			   part, port, function));
		return -EINVAL;
	}
	/* get the offset and size of the requested section */
	next_offs = (le32_to_cpu(base[1 + section])) & offs_mask;
	next_size = ((le32_to_cpu(base[1 + section]) >> header->offs_len) &
		     size_mask);
	DEBUG_OUT(("%s[%u]: next_offs 0x%x, next_size 0x%x, offs_len %d\n",
		   __func__, __LINE__, next_offs, next_size, header->offs_len));
	if ((!next_offs) || (!next_size)) {
		/* section is not used */
		ERROR_OUT((NCIPGNSS,
			   __func__,
			   __LINE__,
			   section,
			   path_idx_mode_name[index_mode],
			   part, port, function));
		return -ENOENT;
	}
	next_offs <<= CFG_OFFS_SIZE_OMIT;
	next_size <<= CFG_OFFS_SIZE_OMIT;
	if ((next_offs & 3) || (next_size & 3)) {
		/* it's not correctly aligned */
		ERROR_OUT((NCIPGUA,
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode],
			   part,
			   port, function, section, next_offs, next_size));
		return -EINVAL;
	}
	DEBUG_OUT((NCIPGF,
		   __func__,
		   __LINE__,
		   path_idx_mode_name[index_mode],
		   part, port, function, section, next_offs, next_size));

	/* see whether the offset fits in the section */
	if ((offset << 2 /* quadbytes */) >= next_size /* bytes */) {
		ERROR_OUT((NCIPGOI,
			   __func__,
			   __LINE__,
			   offset,
			   section,
			   path_idx_mode_name[index_mode],
			   part, port, function));
		return -ENOENT;
	}
	/* calculate the final offset and verify it is in the part */
	next_offs += (offset << 2);
	if (next_offs > size - 3) {
		ERROR_OUT((NCIPGFI,
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode],
			   part, port, function, section, offset));
		return -EINVAL;
	}
	/* compute the final pointer and provide it if requested */
	if (address) {
		temp = (u8 *)base;
		*address = (u32 *)(&(temp[next_offs]));
		DEBUG_OUT((NCIPGFOB,
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode],
			   part,
			   port,
			   function, section, offset, (void *)(*address)));
	}
	return 0;
}

/*
 *  Get the path to a configuration item by name.
 */
#define NCIPGBNF "%s[%u]: found path for \"%s\" at index %u in paths list\n"
static int
nvm_cfg_item_path_get_by_name(const char *const name,
			      const nvm_cfg_item_path_t **path)
{
	unsigned int index;

	if ((!name) || (!path)) {
		ERROR_OUT(("%s[%u]: must provide name and pointer for path\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	DEBUG_OUT(("%s[%u]: search paths list for \"%s\"\n",
		   __func__, __LINE__, name));
	for (index = 0; chimp_config_paths[index].item_name; index++) {
		if (!strcasecmp(name, chimp_config_paths[index].item_name)) {
			/* found it; stop looking */
			break;
		}
	}
	if (chimp_config_paths[index].item_name) {
		DEBUG_OUT((NCIPGBNF, __func__, __LINE__, name, index));
		*path = &(chimp_config_paths[index]);
		return 0;
	} else {
		ERROR_OUT(("%s[%u]: unable to find \"%s\" path\n",
			   __func__, __LINE__, name));
		return -ENOENT;
	}
}

/*
 *  Update the CRC32 value on a configuration section
 */
static void nvm_cfg_section_checksum_update(u32 *base, u32 size)
{
	u8 *temp;

	/* figure out where the CRC32 value goes */
	temp = (u8 *)base;
	base = (u32 *)(&(temp[size - sizeof(u32)]));
	/*
	 *  The CRC32 here includes the space under where we put the value, and
	 *  assumes the underlying value was zero.  Therefore, we need to zero
	 *  the present value of this quadbyte, calculate the CRC32, then
	 *  finally we can populate the updated value.
	 */
	*base = 0;
	*base = cpu_to_le32(crc32(0, (const unsigned char *)temp, size));
}

/*
 *  Get the value for a particular configuration item.
 */
#define NCIVGIBF "%s[%u]: the bitfield specified in the path is not valid\n"
#define NCIVGIP "%s[%u]: must provide config, path, and where to put value\n"
static int
nvm_cfg_item_value_get(struct nvm_cfg_header_s *header,
		       const nvm_cfg_item_path_t *path,
		       unsigned int port, unsigned int function, u32 *value)
{
	u32 *part_base;
	u32 *item_ptr;
	u32 part_size;
	u32 temp;
	u32 mask;
	int result;

	if ((!header) || (!path) || (!value)) {
		ERROR_OUT((NCIVGIP, __func__, __LINE__));
		return -EINVAL;
	}
	if (path->item_bit_count + path->item_lower_bit > 32) {
		ERROR_OUT((NCIVGIBF, __func__, __LINE__));
		return -EFAULT;
	}
	result = nvm_cfg_part_pointer_get(header,
					  path->index_mode,
					  path->item_part,
					  port,
					  function, &part_base, &part_size);
	if (result)
		return result;
	result = nvm_cfg_item_pointer_get(header,
					  path->index_mode,
					  path->item_part,
					  port,
					  function,
					  part_base,
					  part_size,
					  path->item_section,
					  path->item_offset, &item_ptr);
	if (result)
		return result;
	if (path->item_bit_count > 31)
		mask = ~0;
	else
		mask = ((1 << path->item_bit_count) - 1);
	temp = le32_to_cpu(*item_ptr);
	DEBUG_OUT(("%s[%u]: *(%p) == %08X, select bits %u..%u\n",
		   __func__,
		   __LINE__,
		   (void *)item_ptr,
		   temp,
		   path->item_lower_bit + path->item_bit_count - 1,
		   path->item_lower_bit));
	temp >>= path->item_lower_bit;
	temp &= mask;
	DEBUG_OUT(("%s[%u]: get value of \"%s\": %08X\n",
		   __func__, __LINE__, path->item_name, temp));
	*value = temp;
	return 0;
}

/*
 *  Set the value for a particular configuraiton item.  This also updates the
 *  checksum for the section being updated, whether it was correct before
 *  updating the value or not.
 */
#define NCIVSVNR "%s[%u]: %u is not representable on the path (limit 0..%u)\n"
#define NCIVSBFI "%s[%u]: the bitfield specified in the path is not valid\n"
static int
nvm_cfg_item_value_set(struct nvm_cfg_header_s *header,
		       const nvm_cfg_item_path_t *path,
		       unsigned int port, unsigned int function, u32 value)
{
	u32 *part_base;
	u32 *item_ptr;
	u8 *base;
	u32 part_size;
	u32 temp;
	u32 mask;
	int result;

	if ((!header) || (!path)) {
		ERROR_OUT(("%s[%u]: must provide config, and path\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	if (path->item_bit_count + path->item_lower_bit > 32) {
		ERROR_OUT((NCIVSBFI, __func__, __LINE__));
		return -EFAULT;
	}
	result = nvm_cfg_part_pointer_get(header,
					  path->index_mode,
					  path->item_part,
					  port,
					  function, &part_base, &part_size);
	if (result)
		return result;
	result = nvm_cfg_item_pointer_get(header,
					  path->index_mode,
					  path->item_part,
					  port,
					  function,
					  part_base,
					  part_size,
					  path->item_section,
					  path->item_offset, &item_ptr);
	if (result)
		return result;
	if (path->item_bit_count > 31)
		mask = ~0;
	else
		mask = ((1 << path->item_bit_count) - 1);
	temp = le32_to_cpu(*item_ptr);
	if (value > mask) {
		ERROR_OUT((NCIVSVNR, __func__, __LINE__, value, mask));
		return -ERANGE;
	}
	DEBUG_OUT(("%s[%u]: *(%p) == %08X, replace bits %u..%u\n",
		   __func__,
		   __LINE__,
		   (void *)item_ptr,
		   temp,
		   path->item_lower_bit + path->item_bit_count - 1,
		   path->item_lower_bit));
	temp &= (~(mask << path->item_lower_bit));
	temp |= (value << path->item_lower_bit);
	DEBUG_OUT(("%s[%u]: set \"%s\" to %08X: %08X -> %08X\n",
		   __func__,
		   __LINE__,
		   path->item_name, value, le32_to_cpu(*item_ptr), temp));

	(*item_ptr) = cpu_to_le32(temp);

	/* recalculate the check value for the section */
	nvm_cfg_section_checksum_update(part_base, part_size);

	/* recalculate the entire blob's check value */
	base = (u8 *)header;
	item_ptr = (u32 *)(&(base[(le32_to_cpu(header->hdr.size) -
				    sizeof(u32))]));

	*item_ptr = cpu_to_le32(crc32(0,
				      base,
				      le32_to_cpu(header->hdr.size) -
				      sizeof(u32)));
	return 0;
}

/*
 *  Verify the CRC32 value on a configuration section
 */
#define NCSCVNE "%s[%u]: %s_part %u port %u func %u CRC %08X expected %08X\n"
static int
nvm_cfg_sec_chk_vfy(struct nvm_cfg_header_s *header,
		    unsigned int index_mode,
		    unsigned int part, u32 port, u32 function)
{
	u32 *base;
	u8 *temp;
	u32 size;
	u32 crc;
	int ret;

	ret = nvm_cfg_part_pointer_get(header,
				       index_mode,
				       part, port, function, &base, &size);
	if (ret)
		return ret;
	DEBUG_OUT(("%s[%u]: verify %s_part %u port %u func %u check value\n",
		   __func__,
		   __LINE__,
		   path_idx_mode_name[index_mode], part, port, function));

	/* figure out where the CRC32 value goes */
	temp = (u8 *)base;
	base = (u32 *)(&(temp[size - sizeof(u32)]));

	/*
	 *  The CRC32 here includes the space under where we put the value, and
	 *  assumes the underlying value was zero.  Therefore, we need to keep
	 *  the current value, zero the present value of this quadbyte,
	 *  calculate the CRC32, then finally we can check the value (and put
	 *  back what was there before returning).
	 */
	crc = *base;
	*base = 0;
	*base = cpu_to_le32(crc32(0, (const unsigned char *)temp, size));
	if (crc != (*base)) {
		/* mismatched: flag as error and restore old value */
		ERROR_OUT((NCSCVNE,
			   __func__,
			   __LINE__,
			   path_idx_mode_name[index_mode],
			   part,
			   port,
			   function, le32_to_cpu(*base),
			   le32_to_cpu(crc)));
		ret = -EIO;
		*base = crc;
	} /* else no error (inherited), no need to rewrite equal old val */
	return ret;
}

/*
 *  Verify the integrity of a configuration blob
 */
static int nvm_cfg_checksum_validate(struct nvm_cfg_header_s *header)
{
	u8 *temp;
	u32 *crc;
	u32 tot_size;
	u32 hdr_size;
	u32 crc_val;
	unsigned int part;
	unsigned int port;
	unsigned int func;
	int result = 0;

	DEBUG_OUT(("%s[%u]: verify header and overall check value\n",
		   __func__, __LINE__));
	if (!header) {
		ERROR_OUT(("%s[%u]: must provide configuration\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	/* start from the header */
	if (be32_to_cpu(header->hdr.magic) != NVM_CFG_HEADER_MAGIC) {
		ERROR_OUT(("%s[%u]: invalid header magic %08X, expected %08X\n",
			   __func__,
			   __LINE__,
			   be32_to_cpu(header->hdr.magic),
			   NVM_CFG_HEADER_MAGIC));
		return -EIO;
	}
	if ((le16_to_cpu(header->hdr.version) & 0xFFF0) >
	    (NVM_CFG_HEADER_VERSION & 0xFFF0)) {
		ERROR_OUT(("%s[%u]: unknown version %04X\n",
			   __func__,
			   __LINE__, le16_to_cpu(header->hdr.version)));
		return -EIO;
	}
	hdr_size = le32_to_cpu(header->hdr.hdrsize);
	tot_size = le32_to_cpu(header->hdr.size);
	if (tot_size <= hdr_size) {
		ERROR_OUT(("%s[%u]: claimed size %u <= claimed hdr size %u\n",
			   __func__, __LINE__, tot_size, hdr_size));
		return -EIO;
	}
	if ((hdr_size & 3) || (tot_size & 3)) {
		ERROR_OUT(("%s[%u]: hdr size %u or tot size %u not aligned\n",
			   __func__, __LINE__, tot_size, hdr_size));
		return -EIO;
	}
	if ((header->port_count == 0) ||
	    (header->func_count == 0) ||
	    (header->offs_len < 6) ||
	    (header->offs_len > 26) ||
	    ((header->glob_part_count == 0) &&
	     (header->port_part_count == 0) &&
	     (header->func_part_count == 0))) {
		ERROR_OUT(("%s[%u]: the configuration makes no sense\n",
			   __func__, __LINE__));
		return -EIO;
	}
	/* check the claimed checksum */
	/* this does not include the space under the CRC in the CRC */
	temp = (u8 *)header;
	crc = (u32 *)(&(temp[tot_size - 4]));
	crc_val = crc32(0, temp, tot_size - 4);
	if (le32_to_cpu(*crc) != crc_val) {
		ERROR_OUT(("%s[%u]: overall CRC32 %08X but expected %08X\n",
			   __func__, __LINE__, crc_val, le32_to_cpu(*crc)));
		return -EIO;
	}
	/* check all of the global parts */
	for (part = 0; part < header->glob_part_count; part++) {
		result = nvm_cfg_sec_chk_vfy(header,
					     NVM_CFG_PATH_SHARED,
					     part, 0 /* port */,
					     0 /* function */);
		if (result)
			return result;
	}
	/* then check all of the port-indexed parts */
	for (part = 0; part < header->port_part_count; part++) {
		for (port = 0; port < header->port_count; port++) {
			result = nvm_cfg_sec_chk_vfy(header,
						     NVM_CFG_PATH_PORT,
						     part,
						     port, 0 /* function */);
			if (result)
				return result;
		}
	}
	/* then check all the function-indexed parts */
	for (part = 0; part < header->port_part_count; part++) {
		for (port = 0; port < header->port_count; port++) {
			for (func = 0; func < header->func_count; func++) {
				result =
				    nvm_cfg_sec_chk_vfy(header,
							NVM_CFG_PATH_FUNC,
							part, port, func);
				if (result)
					return result;
			}
		}
	}
	return result;
}

/*
 *  Get number of supported ports on the device
 */
#define NCPCGE "%s[%u]: must provide config and place to put port count\n"
static int
nvm_cfg_port_count_get(struct nvm_cfg_header_s *header, u32 *num_ports)
{
	if (header && num_ports) {
		*num_ports = header->port_count;
		return 0;
	} else {
		ERROR_OUT((NCPCGE, __func__, __LINE__));
		return -EINVAL;
	}
}

/*
 *  Get number of supported ports on the device
 */
#define NCFCGE "%s[%u]: must provide config and place to put func count\n"
static int
nvm_cfg_func_count_get(struct nvm_cfg_header_s *header, u32 *num_funcs)
{
	if (header && num_funcs) {
		*num_funcs = header->func_count;
		return 0;
	} else {
		ERROR_OUT((NCFCGE, __func__, __LINE__));
		return -EINVAL;
	}
}

/*
 *  Get the pointer and size to copy to the ChiMP buffer
 */
#define NCPSGA "%s[%u]: must provide config, place for buff ptr and len\n"
static int
nvm_cfg_ptr_size_get(struct nvm_cfg_header_s *header,
		     void **buffer, u32 *length)
{
	u8 *temp;
	int result;

	if (header && buffer && length) {
		result = nvm_cfg_checksum_validate(header);
		if (result)
			return result;
		*length = (le32_to_cpu(header->hdr.size) -
			   sizeof(u32) - le32_to_cpu(header->hdr.hdrsize));
		temp = (u8 *)header;
		*buffer = (void *)(&(temp[le32_to_cpu(header->hdr.hdrsize)]));
		return 0;
	} else {
		ERROR_OUT((NCPSGA, __func__, __LINE__));
		return -EINVAL;
	}
}

/*
 *  Iterate the list of items that are global (shared between all ports).
 *
 *  This calls a provided function with each entry from chimp_config_paths
 *  above that is marked as being a 'glob' type.
 */
static int
nvm_cfg_glob_iterate(nvm_cfg_path_iterator_callback callback, void *extra)
{
	unsigned int index;
	unsigned int counter;
	int result = 0;

	for (index = 0, counter = 0;
	     (!result) && (chimp_config_paths[index].item_name); index++) {
		if (chimp_config_paths[index].index_mode !=
		    NVM_CFG_PATH_SHARED) {
			/* don't want to include these */
			continue;
		}
		result = callback(index,
				  counter,
				  &(chimp_config_paths[index]),
				  extra,
				  chimp_config_paths[index].item_name,
				  chimp_config_paths[index].item_part,
				  chimp_config_paths[index].item_section,
				  chimp_config_paths[index].item_offset,
				  chimp_config_paths[index].item_lower_bit,
				  chimp_config_paths[index].item_bit_count);
		counter++;
	}
	return result;
}

/*
 *  Iterate the list of items that are specific to a given port.
 *
 *  This calls a provided function with each entry from chimp_config_paths
 *  above that is marked as being a 'port' type.*
 */
static int
nvm_cfg_port_iterate(nvm_cfg_path_iterator_callback callback, void *extra)
{
	unsigned int index;
	unsigned int counter;
	int result = 0;

	for (index = 0, counter = 0;
	     (!result) && (chimp_config_paths[index].item_name); index++) {
		if (chimp_config_paths[index].index_mode != NVM_CFG_PATH_PORT)
			/* don't want to include these */
			continue;
		result = callback(index,
				  counter,
				  &(chimp_config_paths[index]),
				  extra,
				  chimp_config_paths[index].item_name,
				  chimp_config_paths[index].item_part,
				  chimp_config_paths[index].item_section,
				  chimp_config_paths[index].item_offset,
				  chimp_config_paths[index].item_lower_bit,
				  chimp_config_paths[index].item_bit_count);
		counter++;
	}
	return result;
}

/*
 *  Iterate the list of items that are specific to a given port function.
 *
 *  This calls a provided function with each entry from chimp_config_paths
 *  above that is marked as being a 'func' type.
 */
static int
nvm_cfg_func_iterate(nvm_cfg_path_iterator_callback callback, void *extra)
{
	unsigned int index;
	unsigned int counter;
	int result = 0;

	for (index = 0, counter = 0;
	     (!result) && (chimp_config_paths[index].item_name); index++) {
		if (chimp_config_paths[index].index_mode != NVM_CFG_PATH_FUNC) {
			/* don't want to include these */
			continue;
		}
		result = callback(index,
				  counter,
				  &(chimp_config_paths[index]),
				  extra,
				  chimp_config_paths[index].item_name,
				  chimp_config_paths[index].item_part,
				  chimp_config_paths[index].item_section,
				  chimp_config_paths[index].item_offset,
				  chimp_config_paths[index].item_lower_bit,
				  chimp_config_paths[index].item_bit_count);
		counter++;
	}
	return result;
}

/*
 *  Use an iterator callback aginst a single path.
 *
 *  This is intended for use in displaying a single path or a small set of
 *  known paths (such as a 'get' command in a UI).  It assumes the path is
 *  valid, so it must have been acquired using  nvm_cfg_item_path_get_by_name
 *  or similar.
 */
static int
nvm_cfg_single_iterate(const nvm_cfg_item_path_t *path,
		       nvm_cfg_path_iterator_callback callback, void *extra)
{
	return callback(0,
			0,
			path,
			extra,
			path->item_name,
			path->item_part,
			path->item_section,
			path->item_offset,
			path->item_lower_bit, path->item_bit_count);
}

/*
 *  Determine whether a path is to a 'shared' parameter
 *
 *  Returns nonzero if the path is for a 'shared' parameter; zero otherwise.
 */
static int nvm_cfg_path_is_shared(const nvm_cfg_item_path_t *path)
{
	return path->index_mode == NVM_CFG_PATH_SHARED;
}

/*
 *  Determine whether a path is to a 'port' parameter
 *
 *  Returns nonzero if the path is for a 'port' parameter; zero if 'global'.
 */
static int nvm_cfg_path_is_port(const nvm_cfg_item_path_t *path)
{
	return path->index_mode == NVM_CFG_PATH_PORT;
}

/*
 *  Determines whether a path is to a 'func' parameter
 *
 *  Retruns nonzero if the path is for a 'func' paramter; zero otherwise.
 */
static int nvm_cfg_path_is_func(const nvm_cfg_item_path_t *path)
{
	return path->index_mode == NVM_CFG_PATH_FUNC;
}

/*
 *  Tries to fetch a configuration from a particular place in SPI flash..
 *  Normally this would be used to fetch a default attached to the end of a
 *  firmware image or something of that nature.
 */
static int nvm_cfg_default_fetch(void **config_data, u32 *config_len)
{
	struct nvm_cfg_header_s *header;
	void *temp;
	int result;

	DEBUG_OUT(("%s[%u]: look for default config in extra blobs\n",
		   __func__, __LINE__));
	result = chimp_extra_blob_find(NVM_CFG_HEADER_MAGIC, &temp);
	if (result)
		goto abort;
	header = temp;
	DEBUG_OUT(("%s[%u]: make sure the config looks valid\n",
		   __func__, __LINE__));
	DEBUG_OUT(("%s[%u]: offsbits=%u ports=%u funcs=%u\n",
		   __func__,
		   __LINE__,
		   header->offs_len, header->port_count, header->func_count));
	DEBUG_OUT(("%s[%u]: types: glob=%u port=%u func=%u\n",
		   __func__,
		   __LINE__,
		   header->glob_part_count,
		   header->port_part_count, header->func_part_count));
	if ((header->port_count == 0) ||
	    (header->func_count == 0) ||
	    (header->offs_len < 6) ||
	    (header->offs_len > 26) ||
	    ((header->glob_part_count == 0) &&
	     (header->port_part_count == 0) &&
	     (header->func_part_count == 0))) {
		ERROR_OUT(("%s[%u]: the configuration makes no sense\n",
			   __func__, __LINE__));
		result = -EIO;
		goto abort;
	}
	result = nvm_cfg_checksum_validate(temp);
	if (result)
		goto abort;
	if (config_data)
		*config_data = temp;
	else
		free(temp);
	if (config_len)
		*config_len = le32_to_cpu(header->hdr.size);
 abort:
	return result;
}

/*
 *  Get byte (assumed hex) from string and find following character
 */
static int hex_str_to_byte(const char *str, u8 *byte, const char **next)
{
	int valid = 0;
	u8 value = 0;
	if (!str) {
		if (next)
			next = NULL;
		return 0;
	}
	/* find the first valid digit */
	while ((*str) &&
	       (!((((*str) >= '0') && ((*str) <= '9')) ||
		  (((*str) >= 'A') && ((*str) <= 'F')) ||
		  (((*str) >= 'a') && ((*str) <= 'f'))))) {
		/* as long as it's not valid hex digit or NUL, keep going */
		str++;
	}
	/* keep going as long as not at end of string */
	while (*str) {
		if (((*str) >= '0') && ((*str) <= '9')) {
			/* this is a valid digit */
			value <<= 4;
			value += ((*str) - '0');
			valid = 1;
		} else if (((*str) >= 'A') && ((*str) <= 'F')) {
			/* this is a valid hex digit (uppercase) */
			value <<= 4;
			value += ((*str) - 'A' + 10);
			valid = 1;
		} else if ((str[0] >= 'a') && ((*str) <= 'f')) {
			/* this is a valid hex digit (lowercase) */
			value <<= 4;
			value += ((*str) - 'a' + 10);
			valid = 1;
		} else
			/* not a valid digit; stop here */
			break;
		str++;
	}
	/* report the location of the next character */
	if (next)
		*next = str;
	/* report the value if we got something that looked like hex */
	if (valid)
		*byte = value;
	/* return 0 if valid, nonzero if not valid */
	return !valid;
}

/*
 *  Dump a configuration element
 */
struct chimp_nvm_dump_state_s {
	void *config_ptr;
	unsigned int port;
	unsigned int func;
};
static int dump_config_item_by_path(unsigned int index,
				    unsigned int counter,
				    const nvm_cfg_item_path_t *path,
				    void *extra,
				    const char const *name,
				    unsigned int part,
				    unsigned int section,
				    unsigned int offset,
				    unsigned int lsb, unsigned int bits)
{
	char *format = "  %50s =%1s%08X\n";	/* deliberately not const */
	/* WARNING: do not change format above; it is edited inline */
	struct chimp_nvm_dump_state_s *state = extra;
	u32 value;
	int result;

	result = nvm_cfg_item_value_get(state->config_ptr,
					path, state->port, state->func, &value);
	if (!result) {
		format[9] = (9 - ((bits + 3) / 4)) + '0';
		format[13] = ((bits + 3) / 4) + '0';
		printf(format, name, " ", value);
	} else {
		printf("  %50s - [unable to retrieve: %d]\n", name, result);
	}
	return 0;
}

/*
 *  Dump the current configuration (in the volatile copy of 'NVRAM')
 */
static int dump_chimp_configuration(void *config_data, u32 ports, u32 funcs)
{
	struct chimp_nvm_dump_state_s state;
	unsigned int i;
	unsigned int j;
	int result;

	state.config_ptr = config_data;
	state.port = 0;
	state.func = 0;
	printf("Current global configuration:\n");
	result = nvm_cfg_glob_iterate(dump_config_item_by_path, &state);
	for (i = 0; (!result) && (i < ports); i++) {
		printf("Current port %u configuration:\n", i);
		state.port = i;
		result = nvm_cfg_port_iterate(dump_config_item_by_path, &state);
		for (j = 0; (!result) && (j < funcs); j++) {
			printf("Current port %u func %u configuration:\n",
			       i, j);
			state.func = j;
			result = nvm_cfg_func_iterate(dump_config_item_by_path,
						      &state);
		}
	}
	return result;
}

/*
 *  If the U-Boot environment specifies eth1addr..eth4addr, this will modify
 *  the MAC addresses in the ChiMP configuration to match the ones in the
 *  U-Boot environment.
 *
 *  If the U-Boot environment does not specify eth1addr..eth4addr, this will
 *  set them to reflect the MAC addresses in the ChiMP configuration. If it
 *  changes the U-Boot environment, this will save the U-Boot environment.
 *
 *  Note that each eth#addr variable is operated upon independently.  It is
 *  thus possible to import some in one direction and others in the other.
 */
#define SCFGCDPLF "Unable to get configuration data pointer and length: %d\n"
#define SCFFMAHF0 "Unable to find FUNC_BOARD_MAC_ADDR_HI for port %u: %d\n"
#define SCFFMALF0 "Unable to find FUNC_BOARD_MAC_ADDR_LO for port %u: %d\n"
#define SCFIMA "WARNING: invalid MAC address specified for port %d\n"
#define SCFSMAHF0 "Unable to set FUNC_BOARD_MAC_ADDR_HI for port %u: %d\n"
#define SCFSMALF0 "Unable to set FUNC_BOARD_MAC_ADDR_LO for port %u: %d\n"
#define SCFFMAHF1 "Unable to get FUNC_BOARD_MAC_ADDR_LO for port %u: %d\n"
#define SCFFMALF1 "Unable to get FUNC_BOARD_MAC_ADDR_HI for port %u: %d\n"
#define SCFPMAD "Port %u using MAC address %s\n"
#define SCF_STR_LIM 32
static int setup_chimp_firmware(void **config_ptr,
				u32 *config_len,
				void **config_data_ptr, u32 *config_data_len)
{
	void *config_data = (*config_ptr);
	int result = 0;
	int created = 0;
	unsigned int i;
	unsigned int j;
	u32 mac_addr_high = 0x000018C0;
	u32 mac_addr_low = 0x86DEAD00;
	u32 ports;
	u32 funcs;
	const char *mac_string_pointer;
	const char *byte_from_string;
	u8 mac_address[6];
	char environment_variable_name[SCF_STR_LIM];
	char environment_variable_value[SCF_STR_LIM];
	const nvm_cfg_item_path_t *path_mac_high;
	const nvm_cfg_item_path_t *path_mac_low;

	if (!config_data) {
		printf("No ChiMP configuration\n");
		result = -EINVAL;
		goto abort;
	}
	result = nvm_cfg_ptr_size_get(config_data,
				      config_data_ptr, config_data_len);
	if (result) {
		printf(SCFGCDPLF, result);
		goto abort;
	}
	result = nvm_cfg_port_count_get(config_data, &ports);
	if (result) {
		printf("Unable to get number of ports in config: %d\n", result);
		goto abort;
	}
	result = nvm_cfg_func_count_get(config_data, &funcs);
	printf("Updating MAC addresses to reflect U-Boot settings\n");
	for (i = 0; i < ports; i++) {
		snprintf(environment_variable_name,
			 SCF_STR_LIM - 1, "eth%uaddr", i + 1);
		mac_string_pointer = getenv(environment_variable_name);
		/* find the port's function information */
		result = nvm_cfg_item_path_get_by_name("FUNC_BOARD_MAC_ADDR_HI",
						       &path_mac_high);
		if (result) {
			printf(SCFFMAHF0, i, result);
			continue;
		}
		result = nvm_cfg_item_path_get_by_name("FUNC_BOARD_MAC_ADDR_LO",
						       &path_mac_low);
		if (result) {
			printf(SCFFMALF0, i, result);
			continue;
		}
		/* parse the MAC address string */
		if (mac_string_pointer) {
			/* U-Boot has this port's MAC address */
			byte_from_string = mac_string_pointer;
			for (j = 0; j < 6; j++) {
				result = hex_str_to_byte(byte_from_string,
							 &(mac_address[j]),
							 &byte_from_string);
				if (result) {
					/* something went wrong */
					break;
				}
			}
			if (j < 6) {
				printf(SCFIMA, i);
				continue;
			}
			mac_addr_high = ((mac_address[0] << 8) |
					 (mac_address[1]));
			mac_addr_low = ((mac_address[2] << 24) |
					(mac_address[3] << 16) |
					(mac_address[4] << 8) |
					(mac_address[5]));
			result = nvm_cfg_item_value_set(config_data,
							path_mac_high,
							i /* port */ ,
							0 /* function */ ,
							mac_addr_high);
			if (result)
				printf(SCFSMAHF0, i, result);
			result = nvm_cfg_item_value_set(config_data,
							path_mac_low,
							i /* port */ ,
							0 /* function */ ,
							mac_addr_low);
			if (result)
				printf(SCFSMALF0, i, result);
		}
		result = nvm_cfg_item_value_get(config_data,
						path_mac_low, i /* port */ ,
						0 /* function */ ,
						&mac_addr_low);
		if (result)
			printf(SCFFMAHF1, i, result);
		result = nvm_cfg_item_value_get(config_data,
						path_mac_high, i /* port */ ,
						0 /* function */ ,
						&mac_addr_high);
		if (result)
			printf(SCFFMALF1, i, result);
		snprintf(environment_variable_value,
			 SCF_STR_LIM - 1,
			 "%02X:%02X:%02X:%02X:%02X:%02X",
			 (mac_addr_high >> 8) & 0xFF,
			 (mac_addr_high) & 0xFF,
			 (mac_addr_low >> 24) & 0xFF,
			 (mac_addr_low >> 16) & 0xFF,
			 (mac_addr_low >> 8) & 0xFF, (mac_addr_low) & 0xFF);
		if (!mac_string_pointer) {
			/* U-Boot does not have this port's MAC address */
			result = setenv(environment_variable_name,
					environment_variable_value);
			if (result) {
				printf("Error %d setting environment %s=%s\n",
				       result,
				       environment_variable_name,
				       environment_variable_value);
				/* don't propagate U-Boot env error */
				result = 0;
			} else {
				created = 1;
			}
		}
		printf(SCFPMAD, i, environment_variable_value);
	}
	if (created) {
		/* updated U-Boot environment, so save it now */
		saveenv();
	}
 abort:
	return result;
}

/*****************************************************************************
 *
 * Opaque blob handling
 *
 */

#define OPAQ_HEADER_VERSION 0x0001
#define OPAQ_HEADER_MAGIC 0x4F706171

struct opaq_info_s {
	u32 offset;		/* offset from header start */
	u32 length;		/* length */
	u64 address;		/* where to stick it */
};

struct opaq_header_s {
	struct chimp_extra_blob_header_s hdr;	/* common header */
	u16 count;		/* number of blobs */
	u16 reserved0;		/* reserved */
	u32 reserved1;		/* reserved */
	struct opaq_info_s info[1];	/* info about blobs */
};

/*
 *  Scan and load the opaque blobs
 *
 *  There is a single 'big blob' that contains all of the 'opaque blobs' to
 *  load, and specifies where to load them.  Load that big blob, then iterate
 *  through its list of blobs and where to put them.
 *
 *  If there is no 'big blob', then just return success.
 *
 *  If something else goes wrong, pass it back to the caller.
 */
static int opaque_blob_scan_and_load(void)
{
	struct opaq_header_s *header;
	void *data = NULL;
	u8 *temp;
	void *dst;
	void *src;
	int result;
	u16 index;
	u16 count;
	u32 offset;
	u32 length;
	u32 bias;
	u64 address;

	result = chimp_extra_blob_find(OPAQ_HEADER_MAGIC, &data);
	if (result == -ENOENT) {
		/* nothing to load; skip it */
		DEBUG_OUT(("%s[%u]: no extra blobs to load; skipping\n",
			   __func__, __LINE__));
		result = 0;
		goto abort;
	}
	if (result)
		goto abort;
	header = data;
	temp = data;
	count = le16_to_cpu(header->count);
	bias = le32_to_cpu(header->hdr.hdrsize);
	DEBUG_OUT(("%s[%u]: loading %u 'opaque' blob%s to memory\n",
		   __func__,
		   __LINE__, (unsigned int)count, (count != 1) ? "s" : ""));
	for (index = 0; index < count; index++) {
		offset = le32_to_cpu(header->info[index].offset) + bias;
		length = le32_to_cpu(header->info[index].length);
		address = le64_to_cpu(header->info[index].address);
		DEBUG_OUT(("%s[%u]: blob %u load %u bytes at %u to %016llX\n",
			   __func__, __LINE__, index, length, offset, address));
		src = (void *)(&(temp[offset]));
		dst = (void *)address;
		memcpy(dst, src, length);
	}
 abort:
	if (data)
		free(data);
	return result;
}

/*****************************************************************************
 *
 * Firmware handling
 *
 */

/*
 *  Firmware bundle header
 */
struct chimpfw_header {
	u32 magic;
	u32 image1_size;
	u32 image2_size;
	u32 crc32;
};

static int start_chimpfw(void *config_data, void *cfg_space)
{
	u32 *address;
	u32 wr_data;
	u32 rd_data = 0;
	u32 spare0_val;
	u32 spare1_val;
	u32 delay;
	int result;
	u32 retries = 5;
	const nvm_cfg_item_path_t *path_spare0_val;
	const nvm_cfg_item_path_t *path_spare1_val;

	result = nvm_cfg_item_path_get_by_name("SHARED_BOARD_SPARE_0",
					       &path_spare0_val);
	if (result) {
		printf(SCFGCDPLF, result);
		return result;
	}

	result = nvm_cfg_item_path_get_by_name("SHARED_BOARD_SPARE_1",
					       &path_spare1_val);
	if (result) {
		printf(SCFGCDPLF, result);
		return result;
	}

	result = nvm_cfg_item_value_get(config_data,
					path_spare0_val, 0 /* port */ ,
					0 /* function */ ,
					&spare0_val);
	if (result)
		printf(SCFGCDPLF, result);

	result = nvm_cfg_item_value_get(config_data,
					path_spare1_val, 0 /* port */ ,
					0 /* function */ ,
					&spare1_val);
	if (result)
		printf(SCFGCDPLF, result);

	delay = min(5000, (int)(spare1_val & 0xFFFF));

	/*
	   SPARE0 is a byte offset into the config space to a flag that is
	   init'd in cfg to indicate the ChiMP is in init phase by default
	   and cleared by ChiMP after init completion. SPARE1 is used to
	   determine how long u-boot should wait before determining that
		 chimp failed to load & init properly.
	 */
	DEBUG_OUT(("%s[%u]: config_data @ 0x%p, cfg_space @ 0x%p\n",
		   __func__, __LINE__, config_data, cfg_space));
	DEBUG_OUT(("%s[%u]: spare0 = 0x%x, spare1 = 0x%x\n",
		   __func__, __LINE__, spare0_val, spare1_val));
	address = (u32 *)((u32)cfg_space + (spare0_val >> 16));
	DEBUG_OUT(("%s[%u]: spare0_val @ 0x%p = 0x%x\n",
		   __func__, __LINE__, address, readl(address)));
	DEBUG_OUT(("%s[%u]: spare0_val @ 0x%p = 0x%x\n",
		   __func__, __LINE__, address + 1, readl(address + 1)));

	/*Start chimp */
	address = (u32 *)(NS2_CHIMP_ROOT + CHIMP_REG_CTRL_BPE_MODE_REG);
	wr_data = 0x5;
	writel(wr_data, address);

	/*Make sure chimp has jumped to start of firmware entry point */
	address = (u32 *)(NS2_CHIMP_ROOT + CHIMP_REG_CTRL_BPE_STAT_REG);
	do {
		rd_data = readl(address);
		mdelay(1000);
		retries--;
	} while (((rd_data & STATUS_JUMP_TO_FW_ENTRY) !=
		  STATUS_JUMP_TO_FW_ENTRY) && retries);

	if ((rd_data & STATUS_JUMP_TO_FW_ENTRY) != STATUS_JUMP_TO_FW_ENTRY) {
		printf("Timed out fastbooting Chimp BPE_STAT_REG : 0x%x\n",
		       rd_data);
		return 1;
	}

	/* wait till chimp has informed us that is has
	   completed initialization
	 */
	retries = min(20, (int)(spare1_val >> 16));
	address = (u32 *)((u32)cfg_space + (spare0_val >> 16));
	DEBUG_OUT(("%s[%u]: rd_data @ 0x%p = 0x%x\n",
		   __func__, __LINE__, address, rd_data));

	do {
		mdelay(delay);
		retries--;
		rd_data = readl(address);
		DEBUG_OUT(("%s[%u]: rd_data 0x%x\n",
			   __func__, __LINE__, rd_data));
		printf("rd_data 0x%x\n", rd_data);
	} while ((rd_data != 0) && retries);

	return retries == 0;
}

static void setup_fastboot(u32 data)
{
	u32 *address;
	u32 wr_data;

	/*Take out M3 processor block out of reset */
	address = (u32 *)(NS2_CHIMP_ROOT + CHIMP_REG_CTRL_BPE_MODE_REG);
	wr_data = 0x2;
	writel(wr_data, address);

	/*Clear the status register */
	address = (u32 *)(NS2_CHIMP_ROOT + CHIMP_REG_CTRL_BPE_STAT_REG);
	wr_data = 0x0;
	writel(wr_data, address);

	/*Enable fast boot mode and update start of firmware */
	address = (u32 *)(NS2_CHIMP_ROOT + CHIMP_REG_CTRL_FSTBOOT_PTR_REG);
	wr_data = data;
	writel(wr_data, address);
}

#define LCFNVSF "Unable to save NVRAM to backing store: %d\n"
#define LCFCNFI "Unable to get deflt ChiMP cfg from fw: %d. Blding config.\n"
int check_cfg_match(u32 new, u32 old)
{
	int ret = 0;

	/* check to make sure the cfg revs match */
	if (old != new) {
		printf("Cfg rev's don't match - 0x%x != 0x%x, rebuilding!\n",
		       old, new);
		ret = -1;
	}

	return ret;
}

int load_chimp_firmware(void)
{
	int ret;
	char *buffer;
	unsigned int *address;
	unsigned int wr_data;
	char *fw_phase1, *fw_phase2;
	struct chimpfw_header *hdr;
	int hdr_len = 0;
	u32 crc32_orig = 0;
	u32 calc_crc = 0;
	u32 chip_rev;
	u32 nitro_core_b0 = 0;
	u32 version;
	void *config_data = NULL;
	void *chimpcfg_data = NULL;
	void *fw_config;
	u32 chimpcfg_len = -1;
	u32 config_len = -1;
	int new_cfg = 0;

	chip_rev = readl(ICFG_CHIP_REVISION_ID);
	nitro_core_b0 = (chip_rev & 0xf0) >> 4;

	if (nitro_core_b0) {
		fw_phase1 = (char *)(CHIMP_SOC_MEMORY_OFFSET);
		fw_config = (void *)(CHIMP_SOC_CONFIG_OFFSET);
	} else {
		fw_phase1 = (char *)(NS2_CHIMP_ROOT + CHIMP_REG_SCPAD +
				     CHIMP_STAGE_1_OFFSET);
		fw_phase2 = (char *)(NS2_CHIMP_ROOT + CHIMP_REG_SCPAD +
				     CHIMP_STAGE_2_OFFSET);
		fw_config = (void *)(NS2_CHIMP_ROOT + CHIMP_REG_SCPAD +
				     CHIMP_A0_CONFIG_OFFSET);
	}

	flash = spi_flash_probe(CONFIG_IPROC_QSPI_BUS,
				CONFIG_IPROC_QSPI_CS, CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);

	hdr_len = sizeof(struct chimpfw_header);

	hdr = malloc(hdr_len);
	if (!hdr) {
		printf("Not able to allocate memory\n");
		return -ENOMEM;
	}

	ret = spi_flash_read(flash, CHIMP_FLASH_OFFSET, hdr_len, hdr);
	if (ret) {
		printf("Flash: Not able to read the Chimp image header!\n");
		free(hdr);
		return -EIO;
	}

	if (hdr->magic != CHIMP_HEADR_MAGIC) {
		printf("Chimp image header not found, skip firmware load\n");
		free(hdr);
		return -EIO;
	}

	buffer = malloc(hdr->image1_size + hdr->image2_size);
	if (!buffer) {
		free(hdr);
		printf("Not able to allocate memory\n");
		return -ENOMEM;
	}

	ret = spi_flash_read(flash, CHIMP_FLASH_OFFSET + hdr_len,
			     hdr->image1_size + hdr->image2_size, buffer);
	if (ret) {
		printf("Flash: Not able to read the Chimp image!\n");
		goto err_out;
	}

	/* Compute crc32 and validate image */
	crc32_orig = hdr->crc32;
	hdr->crc32 = 0;
	calc_crc = crc32(0, (const unsigned char *)hdr, hdr_len);
	calc_crc = crc32(calc_crc, (const unsigned char *)buffer,
			 hdr->image1_size + hdr->image2_size);
	if (crc32_orig != calc_crc) {
		printf("Chimp crc mismatch found: %x expected: %x\n",
		       crc32_orig, calc_crc);
		ret = -EIO;
		goto err_out;
	}

	if (nitro_core_b0 && hdr->image2_size) {
		printf("Chimp FW Ax image found aborting...\n");
		ret = -EIO;
		goto err_out;
	}

	if (!nitro_core_b0 && !hdr->image2_size) {
		printf("Chimp FW B0 image found aborting...\n");
		ret = -EIO;
		goto err_out;
	}

	/* get the configuration from NVRAM or build default configuration */
	ret = ns2_nvram_element_get((u8 *)chimp_nvram_name,
				    strlen(chimp_nvram_name),
				    NULL, 0, &config_len);
	if (ret) {
		printf("Unable to find ChiMP configuration in NVRAM: %d\n",
		       ret);
		goto chimp_not_nvram;
	}
	config_data = malloc(config_len);
	if (!config_data) {
		printf("Unable to allocate %u bytes for config\n", config_len);
		ret = -ENOMEM;
		goto err_out;
	}
	ret = ns2_nvram_element_get((u8 *)chimp_nvram_name,
				    strlen(chimp_nvram_name),
				    (u8 *)config_data, config_len, NULL);
	if (ret) {
		printf("Unable to load ChiMP configuration fron NVRAM: %d\n",
		       ret);
		goto chimp_not_nvram;
	}
	ret = nvm_cfg_checksum_validate(config_data);
 chimp_not_nvram:
	/* failed to load ChiMP config from NVRAM; look elsewhere */
	if (ret) {
		if (config_data) {
			/* something went wrong; don't keep the buffer */
			free(config_data);
			config_data = NULL;
		}
		new_cfg = 1;
	}
	if (ret) {
		/* not able to load current; look for a default config */
		ret = nvm_cfg_default_fetch(&config_data, &config_len);
		if (ret) {
			printf(LCFCNFI, ret);
		}
	}

	/*
	 *  Set or adjust the configuration according to the U-Boot
	 *  environment, maybe edit the U-Boot environment a little.
	 *
	 *  Also retrieve a pointer to the beginning of what the firmware
	 *  expects in its configuration spot, and how long that part is.
	 */
	ret = setup_chimp_firmware(&config_data,
				   &config_len, &chimpcfg_data, &chimpcfg_len);
	if (ret) {
		printf("Chimp firmware configuration failed\n");
		goto err_out;
	} else if (!new_cfg) {
		u32 ver = 0;
		u32 s0_val = 0;
		u32 tmp_cfg_len = -1;
		void *cfg = config_data;
		void *tmp_cfg_data = NULL;
		const nvm_cfg_item_path_t *path_s0;

		ret = nvm_cfg_default_fetch(&tmp_cfg_data, &tmp_cfg_len);
		if (ret) {
			printf(LCFCNFI, ret);
		} else {
			const char *vname = "SHARED_BOARD_SPARE_0";
			ret = nvm_cfg_item_path_get_by_name(vname,
							    &path_s0);
			if (!ret) {
				ret = nvm_cfg_item_value_get(tmp_cfg_data,
							     path_s0,
							     0,
							     0,
							     &ver);
				if (!ret) {
					ret = nvm_cfg_item_value_get(cfg,
								     path_s0,
								     0,
								     0,
								     &s0_val);

					/* make sure the cfg revs match */
					if (check_cfg_match(ver, s0_val)) {
						/* free up old, upd cfg */
						free(config_data);
						config_len = tmp_cfg_len;
						config_data = tmp_cfg_data;
						new_cfg = 1;
					}
				}
			}

			if (!new_cfg)
				free(tmp_cfg_data);
		}
	}

	if (new_cfg) {
		/* created new config; save NVRAM to backing store */
		ret = ns2_nvram_element_set((u8 *)chimp_nvram_name,
					    strlen(chimp_nvram_name),
					    (u8 *)config_data, config_len);
		if (ret)
			printf("Unable to put default config into NVRAM: %d\n",
			       ret);
		/* but it's not fatal, so keep going */
		else {
			/* keep the new config in NVRAM */
			ret = ns2_nvram_commit();
			if (ret)
				printf(LCFNVSF, ret);
			/* still not fatal, so keep going */
		}
	}

	ret = opaque_blob_scan_and_load();
	if (ret) {
		printf("Unable to perform opaque blob load: %d\n", ret);
		goto err_out;
	}

	/* Major, Minor, Build versions */
	version = *(u32 *)(buffer + CHIMP_VERSION_OFFSET);

	/*Get the chimp and related blocks out of reset */
	address = (unsigned int *)(PAXC_ROOT + NIC_CLK_CONTROL);
	wr_data = 0x7f;
	writel(wr_data, address);

	/*
	 *  Firmware does not exepct additional config_data header, so
	 *  look past that when copying data to firmware location.
	 */
	memcpy(fw_config, chimpcfg_data, chimpcfg_len);

	if (nitro_core_b0) {
		printf("loading Chimp FW v:%d.%d.%d.%d for Nitro B0\n",
		       version >> 24, (version >> 16) & 0xff,
		       (version >> 8) & 0xff, version & 0xff);
		memcpy(fw_phase1, buffer, hdr->image1_size);

		setup_fastboot(0x61000002);
	} else {
		printf("loading Chimp FW v:%d.%d.%d.%d for Nitro A0\n",
		       version >> 24, (version >> 16) & 0xff,
		       (version >> 8) & 0xff, version & 0xff);
		memcpy(fw_phase1, buffer, hdr->image1_size);
		memcpy(fw_phase2, buffer + hdr->image1_size, hdr->image2_size);
		*(u32 *)(fw_phase2 - 4) = hdr->image2_size;
		setup_fastboot(1 << 20 | CHIMP_STAGE_1_OFFSET | 0x2);
	}

	ret = start_chimpfw(config_data, fw_config);
	if (ret) {
		printf("Error starting Chimp FW stage 1\n");
		ret = -EFAULT;
	} else {
#ifdef CONFIG_BCM_NS2_CUPS_DDR4
		/* now we should turn on the power to the connectors */
		/* select the proper channel on the mux */
		/* on CUPS board, we use GPIO instead of I2C on SVK */
		unsigned int *addr;

		addr = (unsigned int *)(DMU_COMMON_ROOT + IOMUX_PAD_FUNCTION_1);
		wr_data = readl(addr);

		/* Step1. Set MFIO45 function as GPIO */
		/* Bit20: ctrl bit of MFIO45 */
		wr_data &= MASK_IOMUX_PAD_FUNCTION_1_BIT20_OFF;
		writel(wr_data, addr);

		/* Step2. Set GPIO20 in output mode with val:0 */
		/* Since the gpio default val = 0, only need set it in output */
		writel(MASK_IOMUX_PAD_FUNCTION_1_BIT20_ON,
		       CHIPCOMMONG_GP_OUT_EN);
#else
		u8 datum;
		/* now we should turn on the power to the connectors */
		/* select the proper channel on the mux */
		if (i2c_set_bus_num(NS2_CHIMP_SFPCTL_I2C_BUS))
			goto sfpctl_error;
		if (i2c_probe(NS2_CHIMP_SFPCTL_I2C_MUX_DEV))
			goto sfpctl_error;
		datum = NS2_CHIMP_SFPCTL_I2C_MUX_VAL;
		if (i2c_write(NS2_CHIMP_SFPCTL_I2C_MUX_DEV,
			      NS2_CHIMP_SFPCTL_I2C_MUX_REG,
			      NS2_CHIMP_SFPCTL_I2C_MUX_REG_LEN,
			      &datum, NS2_CHIMP_SFPCTL_I2C_MUX_VAL_LEN))
			goto sfpctl_error;
		/* turn the SFP module power on */
		if (i2c_probe(NS2_CHIMP_SFPCTL_I2C_CTRL_DEV))
			goto sfpctl_error;
		datum = NS2_CHIMP_SFPCTL_I2C_CTRL_VAL;
		if (i2c_write(NS2_CHIMP_SFPCTL_I2C_CTRL_DEV,
			      NS2_CHIMP_SFPCTL_I2C_CTRL_REG,
			      NS2_CHIMP_SFPCTL_I2C_CTRL_REG_LEN,
			      &datum, NS2_CHIMP_SFPCTL_I2C_CTRL_VAL_LEN))
			goto sfpctl_error;
#endif
		printf("Enabled SFP module power");
		goto sfpctl_done;
 sfpctl_error:
		printf("Failed to enable SFP module power");
 sfpctl_done:
		printf("\n");
	}

 err_out:
	free(buffer);
	free(hdr);
	return ret;
}

/*****************************************************************************
 *
 * User interface
 *
 */

static int ns2_chimp_cmd(cmd_tbl_t *cmdtp,
			 int flag, int argc, char *const argv[])
{
	struct chimp_nvm_dump_state_s state;
	const nvm_cfg_item_path_t *path;
	int result;
	void *config_data = NULL;
	u32 config_len;
	u32 ports;
	u32 funcs;
	u32 value;

	if (argc < 2)
		return CMD_RET_USAGE;
	/* for every subcommand, we need to access the configuration */
	result = ns2_nvram_element_get((u8 *)chimp_nvram_name,
				       strlen(chimp_nvram_name),
				       NULL, 0, &config_len);
	if (result) {
		printf("Unable to access the ChiMP configuration: %d\n",
		       result);
		return CMD_RET_FAILURE;
	}
	config_data = malloc(config_len);
	if (!config_data) {
		printf("Unable to allocate %u bytes for ChiMP configuration\n",
		       config_len);
		return CMD_RET_FAILURE;
	}
	result = ns2_nvram_element_get((u8 *)chimp_nvram_name,
				       strlen(chimp_nvram_name),
				       config_data, config_len, &value);
	if (result) {
		printf("Unable to read the ChiMP configuration: %d\n", result);
		result = CMD_RET_FAILURE;
		goto abort;
	}
	if (config_len != value) {
		printf("ChiMP configuration changed size while reading\n");
		result = CMD_RET_FAILURE;
	}
	result = nvm_cfg_port_count_get(config_data, &ports);
	if (result) {
		printf("Unable to get number of ports in ChiMP config: %d\n",
		       result);
		goto abort;
	}
	result = nvm_cfg_func_count_get(config_data, &funcs);
	if (result) {
		printf("Unable to get number of funcs in ChiMP config: %d\n",
		       result);
		goto abort;
	}
	state.config_ptr = config_data;
	state.port = 0;
	state.func = 0;
	if (((!strcasecmp(argv[1], "set")) ||
	     (!strcasecmp(argv[1], "get"))) && (argc > 2)) {
		/* these commands have the same early format; prep them */
		result = nvm_cfg_item_path_get_by_name(argv[2], &path);
		if (result) {
			printf("Unable to get path for \"%s\": %d\n",
			       argv[2], result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		if (!nvm_cfg_path_is_shared(path)) {
			printf("Item \"%s\" is not 'shared'.\n", argv[2]);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	}
	if (((!strcasecmp(argv[1], "setport")) ||
	     (!strcasecmp(argv[1], "getport"))) && (argc > 3)) {
		/* these commands have the same early format; prep them */
		state.port = simple_strtoul(argv[2], NULL, 10);
		if (state.port >= ports) {
			printf("Port number %u is invalid; range 0..%u\n",
			       state.port, ports - 1);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = nvm_cfg_item_path_get_by_name(argv[3], &path);
		if (result) {
			printf("Unable to get path for \"%s\": %d\n",
			       argv[3], result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		if (!nvm_cfg_path_is_port(path)) {
			printf("Item \"%s\" is not 'port'.\n", argv[3]);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	}
	if (((!strcasecmp(argv[1], "setfunc")) ||
	     (!strcasecmp(argv[1], "getfunc"))) && (argc > 4)) {
		/* these commands have the same early format; prep them */
		state.port = simple_strtoul(argv[2], NULL, 10);
		state.func = simple_strtoul(argv[3], NULL, 10);
		if (state.port >= ports) {
			printf("Port number %u is invalid; range 0..%u\n",
			       state.port, ports - 1);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		if (state.func >= funcs) {
			printf("Func number %u is inavlid; range 0..%u\n",
			       state.func, funcs - 1);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = nvm_cfg_item_path_get_by_name(argv[4], &path);
		if (result) {
			printf("Unable to get path for \"%s\": %d\n",
			       argv[4], result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		if (!nvm_cfg_path_is_func(path)) {
			printf("Item \"%s\" is not 'func'.\n", argv[4]);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	}
	if (!strcasecmp(argv[1], "dump")) {
		if (argc != 2) {
			printf("chimp dump\n");
			printf("  takes no additonal arguments\n");
			result = CMD_RET_FAILURE;
		}
		result = dump_chimp_configuration(config_data, ports, funcs);
		if (result)
			result = CMD_RET_FAILURE;
	} else if (!strcasecmp(argv[1], "get")) {
		if (argc != 3) {
			printf("chimp get <item>\n");
			printf("  get the value of <item>, which must be 'shared'\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = nvm_cfg_single_iterate(path,
						dump_config_item_by_path,
						&state);
		if (result) {
			printf("Unable to display value for \"%s\": %d\n",
			       argv[2], result);
			result = CMD_RET_FAILURE;
		}
	} else if (!strcasecmp(argv[1], "getport")) {
		if (argc != 4) {
			printf("chimp getport <port> <item>\n");
			printf("  get the value of <item>, which must be 'port', onport <port>\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = nvm_cfg_single_iterate(path,
						dump_config_item_by_path,
						&state);
		if (result) {
			printf("Unable to display value for \"%s\" on port %u: %d\n",
			       argv[3], state.port, result);
			result = CMD_RET_FAILURE;
		}
	} else if (!strcasecmp(argv[1], "getfunc")) {
		if (argc != 5) {
			printf("chimp getfunc <port> <func> <item>\n");
			printf("  get the value of <item>, which must be 'func', on port <port>, func <func>\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = nvm_cfg_single_iterate(path,
						dump_config_item_by_path,
						&state);
		if (result) {
			printf("Unable to display value for \"%s\" on port %u func %u: %d\n",
			       argv[4], state.port, state.func, result);
			result = CMD_RET_FAILURE;
		}
	} else if (!strcasecmp(argv[1], "set")) {
		if (argc != 4) {
			printf("chimp set <item> <value>\n");
			printf("  set <item>, which must be 'shared', to <value>\n");
			printf("  Does not commit NVRAM to backing store; must do so and then reset\n");
			printf("  before the change will take effect.\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		value = simple_strtoul(argv[3], NULL, 10);
		result = nvm_cfg_item_value_set(config_data,
						path, 0 /* port */ ,
						0 /* func */ ,
						value);
		if (result) {
			printf("Unable to set\"%s\" to %u: %d\n",
			       argv[2], value, result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = ns2_nvram_element_set((u8 *)chimp_nvram_name,
					       strlen(chimp_nvram_name),
					       config_data, config_len);
		if (result) {
			printf("Unable to update the ChiMP configuration: %d\n",
			       result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	} else if (!strcasecmp(argv[1], "setport")) {
		if (argc != 5) {
			printf("chimp setport <port> <item> <value>\n");
			printf("  set <item>, which must be 'port', to <value> on port <port>\n");
			printf("  Does not commit NVRAM to backing store; must do so and then reset\n");
			printf("  before the change will take effect.\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		value = simple_strtoul(argv[4], NULL, 10);
		result = nvm_cfg_item_value_set(config_data,
						path,
						state.port,
						0, /* func */
						value);
		if (result) {
			printf("Unable to set port %u \"%s\" to %u: %d\n",
			       state.port, argv[3], value, result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = ns2_nvram_element_set((u8 *)chimp_nvram_name,
					       strlen(chimp_nvram_name),
					       config_data, config_len);
		if (result) {
			printf("Unable to update the ChiMP cfg: %d\n", result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	} else if (!strcasecmp(argv[1], "setfunc")) {
		if (argc != 6) {
			printf("chimp setport <port> <func> <item> <value>\n");
			printf("  set <item>, which must be 'func', to <value> on <port> <func>\n");
			printf("  Does not commit NVRAM to backing store; must do so and then reset\n");
			printf("  before the change will take effect.\n");
			result = CMD_RET_FAILURE;
			goto abort;
		}
		value = simple_strtoul(argv[4], NULL, 10);
		result = nvm_cfg_item_value_set(config_data,
						path,
						state.port,
						state.func,
						value);
		if (result) {
			printf("Unable to set port %u func %u \"%s\" to %u: %d\n",
			       state.port, state.func, argv[4], value, result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
		result = ns2_nvram_element_set((u8 *)chimp_nvram_name,
					       strlen(chimp_nvram_name),
					       config_data, config_len);
		if (result) {
			printf("Unable to update the ChiMP cfg: %d\n", result);
			result = CMD_RET_FAILURE;
			goto abort;
		}
	} else {
		result = CMD_RET_USAGE;
	}
 abort:
	if (config_data)
		free(config_data);
	return result;
}

U_BOOT_CMD(chimp,
	   20,
	   0,
	   ns2_chimp_cmd,
	   "ChiMP subsystem",
	   "subcommands:\n"
	   "  dump -- dump the entire configuration\n"
	   "  get <item> -- get global configuration <item> value\n"
	   "  set <item> <value> -- set global configuration <item> to <value>\n"
	   "  getport <port> <item> -- get port <port> specific config item <item>\n"
	   "  setport <port> <item> <value> -- set port specific config item to <value>\n"
	   "  getfunc <port> <func> <item> -- get <port> <func> config item <item>\n"
	   "  setfunc <port> <func> <item> <value> -- set <port> <func> <item> to <value>\n"
	   "Note that changes are not saved to the NVRAM backing store unless you issue\n"
	   "an 'nvram save' command, and that changes only take effect once you reset\n"
	   "after the 'nvram save' command.  To revert to the configuration at startup\n"
	   "you can use 'nvram close force;nvram open' sequence, though this might also\n"
	   "revert anything else you have done in NVRAM since the last 'nvram save'.\n"
	   "Get help for any subcommand that needs arguments by issuing it without args.\n"
	   "WARNING: U-Boot overrides MAC addresses if they are set in U-Boot env.\n");
