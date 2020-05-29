/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#ifndef NS2_NVRAM_H
#define NS2_NVRAM_H

#include <asm/types.h>

/*
 *  The various flags for the NVRAM...
 */
/* indicates there is a backup copy in the backing store */
#define NVRAM_FLAG_HAS_BACKUP_COPY		0x00000001
/* indicates the primary copy in backing store is corrupt/unreadable */
#define NVRAM_FLAG_PRIMARY_COPY_BAD		0x00000002
/* indicates the backup copy in backing store is corrupt/unreadable */
#define NVRAM_FLAG_BACKUP_COPY_BAD		0x00000004
/* indicates the backup copy is the one that was loaded */
#define NVRAM_FLAG_USING_BACKUP_COPY		0x00000010
/* indicates using a cleared NVRAM due to corruption/unreadability */
#define NVRAM_FLAG_CLEARED			0x00000020
/* indicates using a default NVRAM due to corruption/unreadability */
#define NVRAM_FLAG_DEFAULTED			0x00000040
/* indicates an edit is in progress ('edit' copy in use) */
#define NVRAM_FLAG_EDITING			0x20000000
/* indicates NVRAM has been changed but not committed */
#define NVRAM_FLAG_DIRTY			0x40000000
/* indicates NVRAM is open */
#define NVRAM_FLAG_OPEN				0x80000000

#define NVRAM_FLAG_NAMES \
{\
	"HAS_BACKUP_COPY   ",\
	"PRIMARY_COPY_BAD  ",\
	"BACKUP_COPY_BAD   ",\
	"reserved03        ",\
	"USING_BACKUP_COPY ",\
	"CLEARED           ",\
	"DEFAULTED         ",\
	"reserved07        ",\
	"reserved08        ",\
	"reserved09        ",\
	"reserved0A        ",\
	"reserved0B        ",\
	"reserved0C        ",\
	"reserved0D        ",\
	"reserved0E        ",\
	"reserved0F        ",\
	"reserved10        ",\
	"reserved11        ",\
	"reserved12        ",\
	"reserved13        ",\
	"reserved14        ",\
	"reserved15        ",\
	"reserved16        ",\
	"reserved17        ",\
	"reserved18        ",\
	"reserved19        ",\
	"reserved1A        ",\
	"reserved1B        ",\
	"reserved1C        ",\
	"EDITING           ",\
	"DIRTY             ",\
	"OPEN              " \
}

/*
 *  Open the NVRAM storage.
 *
 *  If fallback is nonzero, and backup copy is supported, this will prefer the
 *  backup copy to the working copy (so it will try the backup copy first and
 *  fall back to the working copy if it is not valid).  If backup copy is not
 *  supported or not valid, the working copy will be used.
 */
int ns2_nvram_open(int fallback);

/*
 *  Commit the current state of the NVRAM to the backing store.
 *
 *  If backup copy is not supported, this merely writes the current state to
 *  the backing store.
 *
 *  If backup copy is supported, the behaviour is according to the state of the
 *  backing store.  If the working copy in the backing store is valid, this
 *  copies that to the backup copy in the backing store, then it writes the
 *  current state to the working copy in the backing store.  If the working
 *  copy in the backing store is not valid, but the backup copy is valid, this
 *  only writes the current state to the working copy in the backing store.  If
 *  both copies in the backing store are not valid, this writes the current
 *  state to both the working and backup copies in the backing store.
 */
int ns2_nvram_commit(void);

/*
 *  Close the NVRAM.  This does not automatically commit changes, and will
 *  error out if there are changes and the 'force' argument is zero.
 *
 *  If the 'force' argument is nonzero, this will close (without committing any
 *  changes that might be pending).
 */
int ns2_nvram_close(int force);

/*
 *  Find an element in the NVRAM by index
 *
 *  NOTE: Index is not stable; this function is only intended for use in
 *  enumerating the contents of the NVRAM, not accessing particular elements.
 */
int ns2_nvram_element_get_by_index(u32 index,
				   u8 *name_buffer,
				   u32 name_max_len,
				   u32 *name_len,
				   u8 *data_buffer,
				   u32 data_max_len,
				   u32 *data_len);

/*
 *  Find an element in the NVRAM by name
 */
int ns2_nvram_element_get(const u8 *name_buffer,
			  u32 name_len,
			  u8 *data_buffer,
			  u32 data_max_len,
			  u32 *data_len);

/*
 *  Set an element in the NVRAM by name
 */
int ns2_nvram_element_set(const u8 *name_buffer,
			  u32 name_len,
			  const u8 *data_buffer,
			  u32 data_len);

/*
 *  Remove an element fron NVRAM by name
 */
int ns2_nvram_element_unset(const u8 *name_buffer, u32 name_len);

/*
 *  Purge the NVRAM completely (remove all elements).
 */
int ns2_nvram_purge_all(void);

/*
 *  Get information about the NVRAM state
 *
 *  This fills in all zeroes in the case of the NVRAM not being open, but it
 *  also returns -ENODATA in that case.
 */
int ns2_nvram_info_get(u32 *nvram_flags,
		       u32 *nvram_size,
		       u32 *nvram_elements,
		       u32 *nvram_used);

#endif /* ndef NS2_NVRAM_H */
