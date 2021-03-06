/*
 * (C) Copyright 2000-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#ifndef _PART_H
#define _PART_H

#include <blk.h>
#include <ide.h>

typedef struct block_dev_desc {
	int		if_type;	/* type of the interface */
	int		dev;		/* device number */
	unsigned char	part_type;	/* partition type */
	unsigned char	target;		/* target SCSI ID */
	unsigned char	lun;		/* target LUN */
	unsigned char	type;		/* device type */
	unsigned char	removable;	/* removable device */
#ifdef CONFIG_LBA48
	unsigned char	lba48;
	/* device can use 48bit addr (ATA/ATAPI v7) */
#endif
	lbaint_t	lba;		/* number of blocks */
	unsigned long	blksz;		/* block size */
	int		log2blksz;	/* for convenience: log2(blksz) */
	char		vendor[40+1];	/* IDE model, SCSI Vendor */
	char		product[20+1];	/* IDE Serial no, SCSI product */
	char		revision[8+1];	/* firmware revision */
	unsigned long	(*block_read)(int dev,
				      lbaint_t start,
				      lbaint_t blkcnt,
				      void *buffer);
	unsigned long	(*block_write)(int dev,
				       lbaint_t start,
				       lbaint_t blkcnt,
				       const void *buffer);
	unsigned long	(*block_erase)(int dev,
				       lbaint_t start,
				       lbaint_t blkcnt);
	void		*priv;		/* driver private struct pointer */
} block_dev_desc_t;

struct block_drvr {
	char *name;
	struct blk_desc* (*get_dev)(int dev);
	int (*select_hwpart)(int dev_num, int hwpart);
};

#define LOG2(x) (((x & 0xaaaaaaaa) ? 1 : 0) + ((x & 0xcccccccc) ? 2 : 0) + \
		 ((x & 0xf0f0f0f0) ? 4 : 0) + ((x & 0xff00ff00) ? 8 : 0) + \
		 ((x & 0xffff0000) ? 16 : 0))
#define LOG2_INVALID(type) ((type)((sizeof(type)<<3)-1))

/* Part types */
#define PART_TYPE_UNKNOWN	0x00
#define PART_TYPE_MAC		0x01
#define PART_TYPE_DOS		0x02
#define PART_TYPE_ISO		0x03
#define PART_TYPE_AMIGA		0x04
#define PART_TYPE_EFI		0x05

/*
 * Type string for U-Boot bootable partitions
 */
#define BOOT_PART_TYPE	"U-Boot"	/* primary boot partition type	*/
#define BOOT_PART_COMP	"PPCBoot"	/* PPCBoot compatibility type	*/

/* device types */
#define DEV_TYPE_UNKNOWN	0xff	/* not connected */
#define DEV_TYPE_HARDDISK	0x00	/* harddisk */
#define DEV_TYPE_TAPE		0x01	/* Tape */
#define DEV_TYPE_CDROM		0x05	/* CD-ROM */
#define DEV_TYPE_OPDISK		0x07	/* optical disk */

typedef struct disk_partition {
	lbaint_t	start;	/* # of first block in partition	*/
	lbaint_t	size;	/* number of blocks in partition	*/
	ulong	blksz;		/* block size in bytes			*/
	uchar	name[32];	/* partition name			*/
	uchar	type[32];	/* string type description		*/
	int	bootable;	/* Active/Bootable flag is set		*/
#ifdef CONFIG_PARTITION_UUIDS
	char	uuid[37];	/* filesystem UUID as string, if exists	*/
#endif
#ifdef CONFIG_PARTITION_TYPE_GUID
	char	type_guid[37];	/* type GUID as string, if exists	*/
#endif
} disk_partition_t;

/* Misc _get_dev functions */
#ifdef CONFIG_PARTITIONS
/**
 * blk_get_dev() - get a pointer to a block device given its type and number
 *
 * Each interface allocates its own devices and typically struct blk_desc is
 * contained with the interface's data structure. There is no global
 * numbering for block devices, so the interface name must be provided.
 *
 * @ifname:	Interface name (e.g. "ide", "scsi")
 * @dev:	Device number (0 for first device on that interface, 1 for
 *		second, etc.
 * @return pointer to the block device, or NULL if not available, or an
 *	   error occurred.
 */
struct blk_desc *blk_get_dev(const char *ifname, int dev);
struct blk_desc *ide_get_dev(int dev);
struct blk_desc *sata_get_dev(int dev);
struct blk_desc *scsi_get_dev(int dev);
struct blk_desc *usb_stor_get_dev(int dev);
struct blk_desc *mmc_get_dev(int dev);

/**
 * mmc_select_hwpart() - Select the MMC hardware partiion on an MMC device
 *
 * MMC devices can support partitioning at the hardware level. This is quite
 * separate from the normal idea of software-based partitions. MMC hardware
 * partitions must be explicitly selected. Once selected only the region of
 * the device covered by that partition is accessible.
 *
 * The MMC standard provides for two boot partitions (numbered 1 and 2),
 * rpmb (3), and up to 4 addition general-purpose partitions (4-7).
 *
 * @dev_num:	Block device number (struct blk_desc->dev value)
 * @hwpart:	Hardware partition number to select. 0 means the raw device,
 *		1 is the first partition, 2 is the second, etc.
 * @return 0 if OK, other value for an error
 */
int mmc_select_hwpart(int dev_num, int hwpart);
struct blk_desc *systemace_get_dev(int dev);
struct blk_desc *mg_disk_get_dev(int dev);
struct blk_desc *host_get_dev(int dev);
int host_get_dev_err(int dev, struct blk_desc **blk_devp);

/* disk/part.c */
int part_get_info(struct blk_desc *dev_desc, int part, disk_partition_t *info);
void part_print(struct blk_desc *dev_desc);
void part_init(struct blk_desc *dev_desc);
void dev_print(struct blk_desc *dev_desc);

/**
 * blk_get_device_by_str() - Get a block device given its interface/hw partition
 *
 * Each interface allocates its own devices and typically struct blk_desc is
 * contained with the interface's data structure. There is no global
 * numbering for block devices, so the interface name must be provided.
 *
 * The hardware parition is not related to the normal software partitioning
 * of a device - each hardware partition is effectively a separately
 * accessible block device. When a hardware parition is selected on MMC the
 * other hardware partitions become inaccessible. The same block device is
 * used to access all hardware partitions, but its capacity may change when a
 * different hardware partition is selected.
 *
 * When a hardware partition number is given, the block device switches to
 * that hardware partition.
 *
 * @ifname:	Interface name (e.g. "ide", "scsi")
 * @dev_str:	Device and optional hw partition. This can either be a string
 *		containing the device number (e.g. "2") or the device number
 *		and hardware partition number (e.g. "2.4") for devices that
 *		support it (currently only MMC).
 * @dev_desc:	Returns a pointer to the block device on success
 * @return block device number (local to the interface), or -1 on error
 */
int blk_get_device_by_str(const char *ifname, const char *dev_str,
			  struct blk_desc **dev_desc);

/**
 * blk_get_device_part_str() - Get a block device and partition
 *
 * This calls blk_get_device_by_str() to look up a device. It also looks up
 * a partition and returns information about it.
 *
 * @dev_part_str is in the format:
 *	<dev>.<hw_part>:<part> where <dev> is the device number,
 *	<hw_part> is the optional hardware partition number and
 *	<part> is the partition number
 *
 * If ifname is "hostfs" then this function returns the sandbox host block
 * device.
 *
 * If ifname is ubi, then this function returns 0, with @info set to a
 * special UBI device.
 *
 * If @dev_part_str is NULL or empty or "-", then this function looks up
 * the "bootdevice" environment variable and uses that string instead.
 *
 * If the partition string is empty then the first partition is used. If the
 * partition string is "auto" then the first bootable partition is used.
 *
 * @ifname:	Interface name (e.g. "ide", "scsi")
 * @dev_part_str:	Device and partition string
 * @dev_desc:	Returns a pointer to the block device on success
 * @info:	Returns partition information
 * @allow_whole_dev:	true to allow the user to select partition 0
 *		(which means the whole device), false to require a valid
 *		partition number >= 1
 * @return partition number, or -1 on error
 *
 */
int blk_get_device_part_str(const char *ifname, const char *dev_part_str,
			    struct blk_desc **dev_desc,
			    disk_partition_t *info, int allow_whole_dev);
extern const struct block_drvr block_drvr[];
#else
static inline struct blk_desc *blk_get_dev(const char *ifname, int dev)
{ return NULL; }
static inline struct blk_desc *ide_get_dev(int dev) { return NULL; }
static inline struct blk_desc *sata_get_dev(int dev) { return NULL; }
static inline struct blk_desc *scsi_get_dev(int dev) { return NULL; }
static inline struct blk_desc *usb_stor_get_dev(int dev) { return NULL; }
static inline struct blk_desc *mmc_get_dev(int dev) { return NULL; }
static inline int mmc_select_hwpart(int dev_num, int hwpart) { return -1; }
static inline struct blk_desc *systemace_get_dev(int dev) { return NULL; }
static inline struct blk_desc *mg_disk_get_dev(int dev) { return NULL; }
static inline struct blk_desc *host_get_dev(int dev) { return NULL; }

static inline int part_get_info(struct blk_desc *dev_desc, int part,
				disk_partition_t *info) { return -1; }
static inline void part_print(struct blk_desc *dev_desc) {}
static inline void part_init(struct blk_desc *dev_desc) {}
static inline void dev_print(struct blk_desc *dev_desc) {}
static inline int blk_get_device_by_str(const char *ifname, const char *dev_str,
					struct blk_desc **dev_desc)
{ return -1; }
static inline int blk_get_device_part_str(const char *ifname,
					   const char *dev_part_str,
					   struct blk_desc **dev_desc,
					   disk_partition_t *info,
					   int allow_whole_dev)
{ *dev_desc = NULL; return -1; }
#endif

/*
 * We don't support printing partition information in SPL and only support
 * getting partition information in a few cases.
 */
#ifdef CONFIG_SPL_BUILD
# define part_print_ptr(x)	NULL
# if defined(CONFIG_SPL_EXT_SUPPORT) || defined(CONFIG_SPL_FAT_SUPPORT) || \
	defined(CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION)
#  define part_get_info_ptr(x)	x
# else
#  define part_get_info_ptr(x)	NULL
# endif
#else
#define part_print_ptr(x)	x
#define part_get_info_ptr(x)	x
#endif


struct part_driver {
	const char *name;
	int part_type;

	/**
	 * get_info() - Get information about a partition
	 *
	 * @dev_desc:	Block device descriptor
	 * @part:	Partition number (1 = first)
	 * @info:	Returns partition information
	 */
	int (*get_info)(struct blk_desc *dev_desc, int part,
			disk_partition_t *info);

	/**
	 * print() - Print partition information
	 *
	 * @dev_desc:	Block device descriptor
	 */
	void (*print)(struct blk_desc *dev_desc);

	/**
	 * test() - Test if a device contains this partition type
	 *
	 * @dev_desc:	Block device descriptor
	 * @return 0 if the block device appears to contain this partition
	 *	   type, -ve if not
	 */
	int (*test)(struct blk_desc *dev_desc);
};

/* Declare a new U-Boot partition 'driver' */
#define U_BOOT_PART_TYPE(__name)					\
	ll_entry_declare(struct part_driver, __name, part_driver)

#ifdef CONFIG_EFI_PARTITION
#include <part_efi.h>
/* disk/part_efi.c */
/**
 * part_get_info_efi_by_name() - Find the specified GPT partition table entry
 *
 * @param dev_desc - block device descriptor
 * @param gpt_name - the specified table entry name
 * @param info - returns the disk partition info
 *
 * @return - '0' on match, '-1' on no match, otherwise error
 */
int part_get_info_efi_by_name(struct blk_desc *dev_desc,
			      const char *name, disk_partition_t *info);

/**
 * write_gpt_table() - Write the GUID Partition Table to disk
 *
 * @param dev_desc - block device descriptor
 * @param gpt_h - pointer to GPT header representation
 * @param gpt_e - pointer to GPT partition table entries
 *
 * @return - zero on success, otherwise error
 */
int write_gpt_table(struct blk_desc *dev_desc,
		  gpt_header *gpt_h, gpt_entry *gpt_e);

/**
 * gpt_fill_pte(): Fill the GPT partition table entry
 *
 * @param gpt_h - GPT header representation
 * @param gpt_e - GPT partition table entries
 * @param partitions - list of partitions
 * @param parts - number of partitions
 *
 * @return zero on success
 */
int gpt_fill_pte(gpt_header *gpt_h, gpt_entry *gpt_e,
		disk_partition_t *partitions, int parts);

/**
 * gpt_fill_header(): Fill the GPT header
 *
 * @param dev_desc - block device descriptor
 * @param gpt_h - GPT header representation
 * @param str_guid - disk guid string representation
 * @param parts_count - number of partitions
 *
 * @return - error on str_guid conversion error
 */
int gpt_fill_header(struct blk_desc *dev_desc, gpt_header *gpt_h,
		char *str_guid, int parts_count);

/**
 * gpt_restore(): Restore GPT partition table
 *
 * @param dev_desc - block device descriptor
 * @param str_disk_guid - disk GUID
 * @param partitions - list of partitions
 * @param parts - number of partitions
 *
 * @return zero on success
 */
int gpt_restore(struct blk_desc *dev_desc, char *str_disk_guid,
		disk_partition_t *partitions, const int parts_count);

/**
 * is_valid_gpt_buf() - Ensure that the Primary GPT information is valid
 *
 * @param dev_desc - block device descriptor
 * @param buf - buffer which contains the MBR and Primary GPT info
 *
 * @return - '0' on success, otherwise error
 */
int is_valid_gpt_buf(struct blk_desc *dev_desc, void *buf);

/**
 * write_mbr_and_gpt_partitions() - write MBR, Primary GPT and Backup GPT
 *
 * @param dev_desc - block device descriptor
 * @param buf - buffer which contains the MBR and Primary GPT info
 *
 * @return - '0' on success, otherwise error
 */
int write_mbr_and_gpt_partitions(struct blk_desc *dev_desc, void *buf);

/**
 * gpt_verify_headers() - Function to read and CRC32 check of the GPT's header
 *                        and partition table entries (PTE)
 *
 * As a side effect if sets gpt_head and gpt_pte so they point to GPT data.
 *
 * @param dev_desc - block device descriptor
 * @param gpt_head - pointer to GPT header data read from medium
 * @param gpt_pte - pointer to GPT partition table enties read from medium
 *
 * @return - '0' on success, otherwise error
 */
int gpt_verify_headers(struct blk_desc *dev_desc, gpt_header *gpt_head,
		       gpt_entry **gpt_pte);

/**
 * gpt_verify_partitions() - Function to check if partitions' name, start and
 *                           size correspond to '$partitions' env variable
 *
 * This function checks if on medium stored GPT data is in sync with information
 * provided in '$partitions' environment variable. Specificially, name, start
 * and size of the partition is checked.
 *
 * @param dev_desc - block device descriptor
 * @param partitions - partition data read from '$partitions' env variable
 * @param parts - number of partitions read from '$partitions' env variable
 * @param gpt_head - pointer to GPT header data read from medium
 * @param gpt_pte - pointer to GPT partition table enties read from medium
 *
 * @return - '0' on success, otherwise error
 */
int gpt_verify_partitions(struct blk_desc *dev_desc,
			  disk_partition_t *partitions, int parts,
			  gpt_header *gpt_head, gpt_entry **gpt_pte);
#endif

#endif /* _PART_H */
