/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

/****************************************************************************
 * Copyright(c) 2016 Broadcom Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0
 *
 * Name:        ns2_nvram_store.h
 *
 * Description: Backing store for 'NVRAM' support on NS2 using SPI flash
 *
 ****************************************************************************/
#ifndef NS2_NVRAM_STORE_H
#define NS2_NVRAM_STORE_H

/* describe the backing store as a string */
#define NS2_NVRAM_BACKING_STORE		"SPI"

/*
 *  If you want to configure a backup copy, that will be used if the primary
 *  copy is corrupted or otherwise unreadable for some reason, set this to a
 *  nonzero value.
 *
 *  If you do not want (or can not find the space for) a backup copy, set this
 *  to zero.
 */
#define NS2_NVRAM_HAS_BACKUP		0

/*
 *  This is where the primary copy starts in the flash device.
 *
 *  This must align to an erase page.
 */
/* FIXME: U-Boot env alloc 001D0000(128KiB) but only uses 001E0000(64KiB) */
#define NS2_NVRAM_PRIMARY_OFFSET	0x001D0000

/*
 *  This is where the backup copy starts in the flash device.
 *
 *  This must align to an erase page.
 *
 *  This has no effect if the backup option is not enabled above.
 */
/* FIXME: set backup page if we want to use it */
#define NS2_NVRAM_BACKUP_OFFSET		0x00040000

/*
 *  This is the total size of *each* NVRAM copy.  There is some overhead for
 *  the NVRAM and each element in it, so the effective capacity will be
 *  slightly less.
 *
 *  This must be an integral number of erase pages.
 */
#define NS2_NVRAM_SIZE			0x00010000

/*
 *  If the backup copy is enabled, and you want to be paranoid about it being
 *  written before any update to the primary copy, set this nonzero.  If it's
 *  okay to not update the backup copy first, set this to zero.
 *
 *  If zero, a failure to update the backup copy displays error messages but
 *  does not abort committing changes to the backing store.
 *
 *  If nonzero, a failure to update the backup copy displays error messages and
 *  also aborts the commit of changes to the backing store.
 */
#define NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID 1

/****************************************************************************
 *
 *  Exports
 *
 */

/*
 *  Generate a checksum of the backing store.
 *
 *  This may be called upon to generate a checksum for validation or in
 *  preparation of a new NVRAM image, so it needs to be able to put the
 *  generated checksum at a specific place.
 *
 *  Should always return zero unless something went wrong; if something goes
 *  wrong, it should return a nonzero error code.
 */
typedef int (*ns2_nvram_backing_store_checksum_f)
	(const u8 *buffer_to_checksum,
	 unsigned int bytes_to_checksum,
	 u8 *place_to_put_checksum,
	 unsigned int size_of_checksum);

/*
 *  This structure describes and provides access to the NVRAM backing store.
 *
 *  Offsets are within the backing store; both offsets and sizes must be filled
 *  in by the open function, as well as the buffers.  The close function frees
 *  the buffers and replaces their pointers with NULL.  If the backing store
 *  code wants to pre-allocate the third buffer for commits that involve
 *  shifting the 'primary copy' to the 'backup copy', it can use temp_copy
 *  (which is not used by the NVRAM code).
 */
struct ns2_nvram_backing_store_info_s {
	ns2_nvram_backing_store_checksum_f checksum;/* checksum function */
	void *backing_store_priavte;		/* private data */
	u32 nvram_flags;			/* NVRAM flags */
	u32 nvram_size;				/* NVRAM size in bytes */
	u32 nvram_checksum_size;		/* NVRAM cksum size in bytes */
	u8 *working_copy;			/* working copy buffer */
	u8 *edit_copy;				/* edit copy buffer */
	u8 *temp_copy;				/* temp copy buffer */
};

/*
 *  Open access to the backing store
 */
extern int
ns2_nvram_backing_store_open(struct ns2_nvram_backing_store_info_s *info);

/*
 *  Close access to the backing store
 */
extern int
ns2_nvram_backing_store_close(struct ns2_nvram_backing_store_info_s *info);

/*
 *  Read from the backing store.
 *
 *  Set index = 0 for primary copy; set index = 1 for backup copy
 */
extern int
ns2_nvram_backing_store_read(struct ns2_nvram_backing_store_info_s *info,
			     int index,
			     u8 *data_buffer);

/*
 *  Write to the backing store (erase first if necessary)
 *
 *  Set index = 0 for primary copy; set index = 1 for backup copy
 */
extern int
ns2_nvram_backing_store_write(struct ns2_nvram_backing_store_info_s *info,
			      int index,
			      void *data_buffer);

#endif /* ndef NS2_NVRAM_STORE_H */
