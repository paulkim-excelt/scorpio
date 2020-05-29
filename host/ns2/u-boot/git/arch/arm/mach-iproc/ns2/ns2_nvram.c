/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <linux/crc32.h>
#include <malloc.h>
#include <errno.h>
#include <compiler.h>
#include <asm/arch/ns2_nvram.h>
#include "ns2_nvram_store.h"

/*
 *  Support for debugging output (very verbose)
 */
#undef DEBUG_DETAIL
#define DEBUG_ERRORS 1
#ifdef DEBUG_DETAIL
#define DEBUG_OUT(_info) printf _info
#else				/* def DEBUG_DETAIL */
#define DEBUG_OUT(_info)
#endif				/* def DEBUG_DETAIL */
#ifdef DEBUG_ERRORS
#define ERROR_OUT(_info) printf _info
#else				/* def DEBUG_ERRORS */
#define ERROR_OUT(_info)
#endif				/* def DEBUG_ERRORS */

/******************************************************************************
 *
 *  Internal functionality
 *
 */

/*
 *  NVRAM and backing store state.
 *
 *  Assign at least one field so the rest of it gets zeroed.
 */
static struct ns2_nvram_backing_store_info_s ns2_nvram_info = {
	.backing_store_priavte = NULL
};

/*
 *  Checksum function is provided to the NVRAM support code so it can keep the
 *  entire NVRAM checked and also deal with the additional bytes imposed by
 *  adding the checksum to the NVRAM space.
 */

/*
 *  We choose CRC32 for the checksumming.  This could be changed later by
 *  plugging in a new one, but then we would want to keep track of both
 *  versions of this to accommodate older copies loading properly.
 *
 *  Basically, this is only called to checksum the entire NVRAM space except
 *  for the size of the checksum.  The last size_of_checksum bytes of the NVRAM
 *  space is where the expected checksum value is kept.
 */
#define NNCWL "%s[%u]: expected checksum value to be 4 bytes, not %u\n"
static int ns2_nvram_crc32(const u8 *buffer_to_checksum,
			   unsigned int bytes_to_checksum,
			   u8 *place_to_put_checksum,
			   unsigned int size_of_checksum)
{
	u32 crc_val;

	if (size_of_checksum != 4) {
		ERROR_OUT((NNCWL, __func__, __LINE__, size_of_checksum));
		return -EINVAL;
	}
	crc_val = crc32(0, buffer_to_checksum, bytes_to_checksum);
	(*((u32 *)(place_to_put_checksum))) = cpu_to_le32(crc_val);
	return 0;
}

/*
 *  The backing store gets a header.  Not much of one, since each element in
 *  the backing store has its own header, but this is here just for searching
 *  and some sanity work.
 *
 *  The version is here in case underlying things change, such as another
 *  checksum mechanism or rearrangement.  But as long as that does not happen,
 *  it should be possible for backward of forward compatibility of the structs.
 *  Even if we change checksum method, the version may not need to change as
 *  long as the new method can be distinguished by the size of its value,
 *  though older code would not know to try the newer method...
 */
#define NVRAM_HEADER_MAGIC 0x42434D4E5652414Dull
#define NVRAM_HEADER_VERSION 0x00010000
struct nvram_header_s {
	u64 nvram_magic;	/* magic number for search/check */
	u32 nvram_version;	/* version of the mechanism */
	u32 nvram_size;		/* total size */
	u32 nvram_header_size;	/* size of this header */
	u32 elem_header_size;	/* size of element headers */
	u32 nvram_checksum_size;	/* size of checksum */
	u32 nvram_elements;	/* number of active elements */
	/* new fields must only be added to the end */
};

/*
 *  Each element in the backing store gets a header.  Ideally this would be
 *  packed (so byte aligned) for space reasons, but some devices care about
 *  alignment.  Thus, it gets quadbyte aligned (the previous element is padded
 *  if necessary but its size is not adjusted to consider the padding).
 *
 *  Immediately after this is the name.
 *
 *  Immediately after the name is the data.
 *
 *  Note that both name and data are considered binary, so no termination or
 *  escapes (this is why both sizes are necessary).
 */
struct nvram_elem_header_s {
	u32 elem_name_size;	/* bytes in name */
	u32 elem_data_size;	/* bytes in data */
	/* new fields must only be added to the end */
};

/*
 *  Note that the header is always included, as are the element headers.  They
 *  add some overhead to the NVRAM driver, but also give some flexibility in
 *  what is stored and how it is referenced.  Also there is a checksum at the
 *  end of the NVRAM; this covers all but the last (size of the checksum)
 *  bytes, which is where the resulting checksum is placed.  An element can not
 *  have a zero byte name, but it is permissible to have zero length content.
 *
 *  This storage method expects a zero-length-name element header to signify
 *  the end of the list of elements.  Thus, when making changes, the entire
 *  'edit copy' space is zeroed, a new header built, and all elements except
 *  the one to be modified will be copied from the 'working copy' to the 'edit
 *  copy', and, if we are changing or adding an element, the new element is
 *  added.  Finally, the checksum is generated and if all went well, the 'edit
 *  copy' is copied back to the 'working copy'.
 */

/*
 *  Validate an NVRAM copy (both format and checksum)
 *
 *  This allows checksum handler and size to be specified in the expectation
 *  that someone may change their mind about checksum type later.  The caller
 *  is responsible for retrying with multiple types if they choose to do this.
 *
 *  This is expected to be called from within the 'open' code, and other
 *  places, so if the provided checksum function requires information from the
 *  'info' struct, that information must already be in place before this is
 *  called from the 'open' code.
 */
static int nvram_copy_validate(struct ns2_nvram_backing_store_info_s *info,
			       const u8 *buffer,
			       u32 buffer_size,
			       ns2_nvram_backing_store_checksum_f checksum,
			       u32 checksum_size)
{
	u8 chksum_val[checksum_size];
	struct nvram_header_s *nvram_header;
	struct nvram_elem_header_s *elem_header;
	u32 offset;
	u32 elems;
	u32 limit;
	int ret = 0;

	DEBUG_OUT(("%s[%u]: check header\n", __func__, __LINE__));
	if ((!info) ||
	    (!buffer) || (!buffer_size) || (!checksum) || (!checksum_size)) {
		ERROR_OUT(("%s[%u]: invalid argument(S)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	nvram_header = (struct nvram_header_s *)&buffer[0];
	if (le64_to_cpu(nvram_header->nvram_magic) != NVRAM_HEADER_MAGIC) {
		ERROR_OUT(("%s[%u]: header magic %016llX != %016llX\n",
			   __func__,
			   __LINE__,
			   le64_to_cpu(nvram_header->nvram_magic),
			   NVRAM_HEADER_MAGIC));
		return -EINVAL;
	}
	if (le32_to_cpu(nvram_header->nvram_size) > buffer_size) {
		ERROR_OUT(("%s[%u]: header claimed size %u > buffer %u\n",
			   __func__,
			   __LINE__,
			   le32_to_cpu(nvram_header->nvram_size), buffer_size));
		return -EINVAL;
	}
	if ((le32_to_cpu(nvram_header->nvram_version) & 0xFFFF0000) >
	    (NVRAM_HEADER_VERSION & 0xFFFF0000)) {
		ERROR_OUT(("%s[%u]: header version %08X too far from %08X\n",
			   __func__,
			   __LINE__,
			   le32_to_cpu(nvram_header->nvram_version),
			   NVRAM_HEADER_VERSION));
		return -EINVAL;
	}
	DEBUG_OUT(("%s[%u]: verify checksum\n", __func__, __LINE__));
	ret = checksum(buffer,
		       buffer_size - checksum_size, chksum_val, checksum_size);
	if (ret) {
		ERROR_OUT(("%s[%u]: checksum function failed %d\n",
			   __func__, __LINE__, ret));
		return ret;
	}
	if (memcmp(chksum_val, &buffer[buffer_size - checksum_size],
		   checksum_size)) {
		ERROR_OUT(("%s[%u]: checksum disagrees with expected value\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	/* scan the elements */
	DEBUG_OUT(("%s[%u]: scan elements\n", __func__, __LINE__));
	limit = buffer_size - (le32_to_cpu(nvram_header->elem_header_size) +
			       le32_to_cpu(nvram_header->nvram_checksum_size));
	offset = le32_to_cpu(nvram_header->nvram_header_size);
	elems = 0;
	do {
		/* element headers are always quadbyte aligned */
		offset = ((offset + 3) & (~3));
		elem_header = (struct nvram_elem_header_s *)&buffer[offset];
		if (le32_to_cpu(elem_header->elem_name_size)) {
			/* this should be a valid element */
			offset += (le32_to_cpu(nvram_header->elem_header_size) +
				   le32_to_cpu(elem_header->elem_name_size) +
				   le32_to_cpu(elem_header->elem_data_size));
			elems++;
		} else {
			/* there are no more elements */
			break;
		}
	} while (offset < limit);
	if (offset > limit) {
		ERROR_OUT(("%s[%u]: elements kept going after expected end\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	if (elems != le32_to_cpu(nvram_header->nvram_elements)) {
		ERROR_OUT(("%s[%u]: expected %u elements but counted %u\n",
			   __func__,
			   __LINE__,
			   le32_to_cpu(nvram_header->nvram_elements), elems));
		return -EINVAL;
	}
	/*
	 *  If we made it down to here, then all the tests looked good.  We can
	 *  probably assume this NVRAM image is valid.
	 */
	return 0;
}

/*
 *  Find an element by index (starting from zero) in the NVRAM, copy its name
 *  and data to caller's buffers, and return the actual length of the name and
 *  data in bytes.  Because this does not clear the caller's buffer(s), the
 *  caller should make sure they are clear if such is necessary or expected.
 *
 *  if name_buffer is NULL or name_max_len zero, no name will be copied out.
 *
 *  if name_len is NULL, the actual length of the name will not be provided.
 *  If it is not NULL, the actual length of the name will be written at the
 *  provided address.  Note that this value may be larger than the name_max_len
 *  provided by the caller (but if name_buffer was provided, the amount of the
 *  name copied to it will be limited by name_max_len).
 *
 *  If data_buffer is NULL or data_max_len zero, no data will be copied out.
 *
 *  If data_len is NULL, the actual length of the element will not be provided.
 *  If it is not NULL, the actual length of the data will be written at the
 *  provided address.  Note that this value may be larger than the data_max_len
 *  provided by the caller (but if data_buffer was provided, the amount of the
 *  data copied to it will be limited by data_max_len).
 *
 *  This assumes the NVRAM image provided is valid.
 *
 *  This probably is useless except for traversals -- there is no order
 *  guarantee of elements within the NVRAM space.
 */
static int
nvram_element_get_by_index(struct ns2_nvram_backing_store_info_s *info,
			   u32 index,
			   u8 *name_buffer,
			   u32 name_max_len,
			   u32 *name_len,
			   u8 *data_buffer, u32 data_max_len, u32 *data_len)
{
	struct nvram_header_s *nvram_header;
	struct nvram_elem_header_s *elem_header;
	u8 *buffer;
	u8 *data = NULL;
	u8 *name = NULL;
	u32 offset;
	u32 limit;
	u32 elem_header_size;
	u32 elem_name_size;
	u32 elem_data_size;

	if ((!info) || (!info->working_copy)) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	DEBUG_OUT(("%s[%u]: scan elements\n", __func__, __LINE__));
	buffer = info->working_copy;
	nvram_header = (struct nvram_header_s *)(buffer);
	elem_header_size = le32_to_cpu(nvram_header->elem_header_size);
	/* scan the elements */
	limit = (le32_to_cpu(nvram_header->nvram_size) -
		 le32_to_cpu(nvram_header->nvram_checksum_size));
	offset = le32_to_cpu(nvram_header->nvram_header_size);
	do {
		/* element headers are always quadbyte aligned */
		offset = ((offset + 3) & (~3));
		/* look at this element */
		elem_header = (struct nvram_elem_header_s *)&buffer[offset];
		elem_name_size = le32_to_cpu(elem_header->elem_name_size);
		elem_data_size = le32_to_cpu(elem_header->elem_data_size);
		if (!elem_name_size)
			/* zero name length is past end of list */
			break;
		if (!index) {
			name = &buffer[offset + elem_header_size];
			data = &buffer[(offset + elem_header_size +
					elem_name_size)];
			break;
		}
		offset += (elem_header_size + elem_name_size + elem_data_size);
		index--;
	} while (offset < limit);
	if (data) {
		/* we found it; copy it out if applicable */
		DEBUG_OUT(("%s[%u]: found requested element\n",
			   __func__, __LINE__));
		if (name_buffer && name_max_len) {
			if (name_max_len > elem_name_size)
				name_max_len = elem_name_size;
			memcpy(name_buffer, name, name_max_len);
		}
		if (name_len)
			*name_len = elem_name_size;
		if (data_buffer && data_max_len) {
			if (data_max_len > elem_data_size)
				data_max_len = elem_data_size;
			memcpy(data_buffer, data, data_max_len);
		}
		if (data_len)
			*data_len = elem_data_size;
		return 0;
	} else {
		/* did not find it */
		ERROR_OUT(("%s[%u]: did not find requested element\n",
			   __func__, __LINE__));
		return -ENOENT;
	}
}

/*
 *  Find an element by name in the NVRAM, copy its data to caller's buffer, and
 *  return the actual length of the data in bytes.  Because this does not clear
 *  the caller's buffer, the caller should make sure it is clear if such is
 *  necessary or expected.
 *
 *  If databuff is NULL or datamax zero, nothing will be copied out.
 *
 *  If datalen is NULL, the actual length of the element will not be provided.
 *
 *  NOTE: this assumes the NVRAM image provided is valid.
 */
static int nvram_element_get(struct ns2_nvram_backing_store_info_s *info,
			     const u8 *name_buffer,
			     u32 name_len,
			     u8 *data_buffer, u32 data_max_len, u32 *data_len)
{
	struct nvram_header_s *nvram_header;
	struct nvram_elem_header_s *elem_header;
	u8 *buffer;
	u8 *name = NULL;
	u8 *data = NULL;
	u32 offset;
	u32 limit;
	u32 elem_header_size;
	u32 elem_name_size;
	u32 elem_data_size;

	if ((!info) || (!info->working_copy) || (!name_buffer) || (!name_len)) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	DEBUG_OUT(("%s[%u]: scan elements looking for '%s'\n", __func__,
		   __LINE__, name_buffer));
	buffer = info->working_copy;
	nvram_header = (struct nvram_header_s *)(buffer);
	elem_header_size = le32_to_cpu(nvram_header->elem_header_size);
	/* scan the elements */
	limit = (le32_to_cpu(nvram_header->nvram_size) -
		 le32_to_cpu(nvram_header->nvram_checksum_size));
	offset = le32_to_cpu(nvram_header->nvram_header_size);
	do {
		/* element headers are always quadbyte aligned */
		offset = ((offset + 3) & (~3));
		/* look at this element */
		elem_header = (struct nvram_elem_header_s *)&buffer[offset];
		elem_name_size = le32_to_cpu(elem_header->elem_name_size);
		elem_data_size = le32_to_cpu(elem_header->elem_data_size);
		DEBUG_OUT(("\telement[%04d]: name_sz %d, data_sz %d", offset,
			   elem_name_size, elem_data_size));
		if (!elem_name_size) {
			DEBUG_OUT(("\n"));
			/* zero name length is past end of list */
			break;
		}
		DEBUG_OUT((", name '%s'\n",
			   &buffer[offset + elem_header_size]));
		if (name_len == elem_name_size) {
			/* name is same length; check it */
			name = &buffer[offset + elem_header_size];
			if (!memcmp(name_buffer, name, name_len)) {
				/* it matches; get data pointer */
				data = &(buffer[(offset +
						 elem_header_size + name_len)]);
				break;
			}
		}
		/* look at the next element */
		offset += (elem_header_size + elem_name_size + elem_data_size);
	} while (offset < limit);
	if (data) {
		/* we found it; copy it out if applicable */
		DEBUG_OUT(("%s[%u]: found requested element\n",
			   __func__, __LINE__));
		if (data_buffer && data_max_len) {
			if (data_max_len > elem_data_size)
				data_max_len = elem_data_size;
			memcpy(data_buffer, data, data_max_len);
		}
		if (data_len)
			*data_len = elem_data_size;
		return 0;
	} else {
		/* did not find it */
		ERROR_OUT(("%s[%u]: did not find requested element\n",
			   __func__, __LINE__));
		return -ENOENT;
	}
}

/*
 *  Copy all elements except the specified one from the 'working' copy to the
 *  'edit' copy.  This basically builds a new copy in the 'edit' copy space but
 *  without the selected element.
 *
 *  It is not an error if the requested element does not exit; in that case,
 *  this merely copies what is in the 'working' copy to the 'edit' copy using
 *  the current headers and checksum mechanism.
 *
 *  This returns the next byte that can be used for an element (properly
 *  aligned) so a new element can be more easily added (if, for example,
 *  replacing the value of the one that this omitted in the copy).
 *
 *  This neither updates the checksum for the 'edit' copy, nor moves it back to
 *  the 'working' copy.  It is possible (indeed, mandatory) that something else
 *  be done first -- even if there is no other edit operation, the checksum
 *  needs to be filled in.  That is omitted here because there may be more to
 *  do before calling the operation complete.
 *
 *  Multiple calls to this will not remove multiple elements; the final result
 *  will be that the 'edit' copy contains all elements from the 'working' copy
 *  except the one specified in the most recent call to this.  In order to
 *  remove multiple elements, the changes must be committed after each one.
 *
 *  In the case of multiple elements of the same name existing and matching the
 *  one to be removed, this will remove all of them.
 */
#define CEENNF "%s[%u]: remaining elems don't fit in the available space\n"
#define CEENWC "%s[%u]: copied %u elem%s, but expected to copy %u elem%s\n"
static int
nvram_copy_elements_except_by_name(struct ns2_nvram_backing_store_info_s *info,
				   const u8 *name_buffer,
				   u32 name_len, u32 *next_byte)
{
	struct nvram_header_s *old_nvram_header;
	struct nvram_header_s *new_nvram_header;
	struct nvram_elem_header_s *old_elem_header;
	struct nvram_elem_header_s *new_elem_header;
	u8 *old_nvram;
	u8 *new_nvram;
	u8 *old_name;
	u8 *new_name;
	u32 old_offset;
	u32 new_offset;
	u32 old_limit;
	u32 new_limit;
	u32 old_elem_header_size;
	u32 new_elem_header_size;
	u32 elem_name_size;
	u32 elem_data_size;
	u32 old_nvram_elements;
	u32 new_nvram_elements;
	int found = 0;

	/* set up 'working' copy references */
	DEBUG_OUT(("%s[%u]: prep 'working' copy\n", __func__, __LINE__));
	old_nvram = info->working_copy;
	old_nvram_header = (struct nvram_header_s *)old_nvram;
	old_offset = le32_to_cpu(old_nvram_header->nvram_header_size);
	old_limit = (le32_to_cpu(old_nvram_header->nvram_size) -
		     le32_to_cpu(old_nvram_header->nvram_checksum_size));
	old_elem_header_size = le32_to_cpu(old_nvram_header->elem_header_size);
	old_nvram_elements = le32_to_cpu(old_nvram_header->nvram_elements);

	/* prepare 'edit' copy space */
	DEBUG_OUT(("%s[%u]: prep 'edit' copy\n", __func__, __LINE__));
	new_nvram = info->edit_copy;
	memset(new_nvram, 0x00, info->nvram_size);
	new_nvram_header = (struct nvram_header_s *)new_nvram;
	new_nvram_header->nvram_magic = cpu_to_le64(NVRAM_HEADER_MAGIC);
	new_nvram_header->nvram_size = cpu_to_le32(info->nvram_size);
	new_nvram_header->nvram_header_size =
	    cpu_to_le32(sizeof(*new_nvram_header));
	new_elem_header_size = sizeof(*new_elem_header);
	new_nvram_header->elem_header_size = cpu_to_le32(new_elem_header_size);
	new_nvram_header->nvram_checksum_size =
	    cpu_to_le32(info->nvram_checksum_size);
	new_offset = le32_to_cpu(new_nvram_header->nvram_header_size);
	new_limit = (le32_to_cpu(new_nvram_header->nvram_size) -
		     (new_elem_header_size +
		      le32_to_cpu(new_nvram_header->nvram_checksum_size)));
	new_nvram_elements = 0;

	/* copy all elements except the specified one */
	DEBUG_OUT(("%s[%u]: copy elements\n", __func__, __LINE__));
	do {
		old_offset = (old_offset + 3) & (~3);
		new_offset = (new_offset + 3) & (~3);
		old_elem_header =
		    (struct nvram_elem_header_s *)&old_nvram[old_offset];
		new_elem_header =
		    (struct nvram_elem_header_s *)&new_nvram[new_offset];
		elem_name_size = le32_to_cpu(old_elem_header->elem_name_size);
		elem_data_size = le32_to_cpu(old_elem_header->elem_data_size);
		if (!elem_name_size) {
			/* there are no more elements */
			break;
		}
		old_name = &old_nvram[old_offset + old_elem_header_size];
		if ((name_len != elem_name_size) ||
		    (memcmp(old_name, name_buffer, name_len))) {
			/* this is not the one to skip; copy it */
			if ((new_offset +
			     new_elem_header_size +
			     elem_name_size + elem_data_size) > new_limit) {
				/* it's too big for the new copy */
				return -ENOMEM;
			}
			new_elem_header->elem_name_size =
			    old_elem_header->elem_name_size;
			new_elem_header->elem_data_size =
			    old_elem_header->elem_data_size;
			new_name = &(new_nvram[(new_offset +
						new_elem_header_size)]);
			memcpy(new_name,
			       old_name, elem_name_size + elem_data_size);
			/* point to next space in 'edit' copy */
			new_offset += (new_elem_header_size +
				       elem_name_size + elem_data_size);
			new_nvram_elements++;
		} else {
			/*
			 *  There should never be two copies of an element by
			 *  name, but just in case we try to be graceful.
			 */
			found++;
		}
		/* point to next element in 'working' copy */
		old_offset += (old_elem_header_size +
			       elem_name_size + elem_data_size);
	} while ((old_offset < old_limit) && (new_offset < new_limit));
	/* make sure things came out valid */
	if ((old_offset > old_limit) || (new_offset > new_limit)) {
		ERROR_OUT((CEENNF, __func__, __LINE__));
		return -ENOMEM;
	}
	new_nvram_header->nvram_elements = cpu_to_le32(new_nvram_elements);
	if ((old_nvram_elements - found) != new_nvram_elements) {
		ERROR_OUT((CEENWC,
			   __func__,
			   __LINE__,
			   new_nvram_elements,
			   (new_nvram_elements != 1) ? "s" : "",
			   old_nvram_elements - found,
			   ((old_nvram_elements - found) != 1) ? "s" : ""));
		return -EINVAL;
	}
	/* pass this back to caller for future reference */
	DEBUG_OUT(("%s[%u]: tidy state\n", __func__, __LINE__));
	if (next_byte)
		*next_byte = (new_offset + 3) & (~3);
	/* mark the 'edit' copy as active */
	info->nvram_flags |= NVRAM_FLAG_EDITING;
	return 0;
}

/*
 *  Add a new element to an existing 'edit' copy.  If there is no 'edit' copy
 *  yet, nvram_copy_elems_except_by_name should be called first to copy
 *  anything except the one about to be added here.
 *
 *  This must not be called to add a new element of the same name as an
 *  existing one; the existing one should be first removed by the
 *  nvram_copy_elems_except_by_name function (which is used to prepare the
 *  'edit' copy even if an element of the given name does not yet exist).
 *
 *  In general, nvram_copy_elems_except_by_name will be called first, then
 *  this, then nvram_edit_commit.  The call to nvram_edit_commit will generate
 *  the checksum, copy the 'edit' buffer back to the 'working' buffer, clear
 *  the edit flag, and set the dirty flag.
 *
 *  It is obligatory that 'next_byte' be the value passed back by either this
 *  function or nvram_copy_elems_except_by_name.  In order to facilitate
 *  multiple adds in a single commit, the value at 'next_byte' will be updated
 *  on success.
 */
#define NANENF "%s[%u]: elem needs %u bytes, more than the %u remaining\n"
static int nvram_add_new_element(struct ns2_nvram_backing_store_info_s *info,
				 const u8 *name_buffer,
				 u32 name_len,
				 const u8 *data_buffer,
				 u32 data_len, u32 *next_byte)
{
	struct nvram_header_s *nvram_header;
	struct nvram_elem_header_s *elem_header;
	u8 *nvram;
	u8 *name;
	u8 *data;
	u32 offset;
	u32 limit;
	u32 elem_header_size;

	if (!(info->nvram_flags & NVRAM_FLAG_EDITING)) {
		ERROR_OUT(("%s[%u]: not currently editing\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	/* refer to the 'edit' copy */
	nvram = info->edit_copy;
	nvram_header = (struct nvram_header_s *)(info->edit_copy);
	elem_header_size = le32_to_cpu(nvram_header->elem_header_size);
	limit = (le32_to_cpu(nvram_header->nvram_size) -
		 (elem_header_size +
		  le32_to_cpu(nvram_header->nvram_checksum_size)));
	/* ensure quadbyte alignment for element header */
	DEBUG_OUT(("%s[%u]: verify new element will fit\n", __func__,
		   __LINE__));
	offset = ((*next_byte) + 3) & (~3);
	elem_header = (struct nvram_elem_header_s *)&nvram[offset];
	/* make sure this element will fit */
	if ((offset + elem_header_size + name_len + data_len) > limit) {
		ERROR_OUT((NANENF, __func__, __LINE__,
			   elem_header_size + name_len + data_len,
			   limit - offset));
		return -ENOMEM;
	}
	/* looks okay, so add it */
	DEBUG_OUT(("%s[%u]: add new element\n", __func__, __LINE__));
	elem_header->elem_name_size = cpu_to_le32(name_len);
	elem_header->elem_data_size = cpu_to_le32(data_len);
	name = &nvram[offset + elem_header_size];
	data = &nvram[offset + elem_header_size + name_len];
	memcpy(name, name_buffer, name_len);
	memcpy(data, data_buffer, data_len);
	nvram_header->nvram_elements =
	    cpu_to_le32(le32_to_cpu(nvram_header->nvram_elements) + 1);
	/* adjust pointer to prepare for the next one */
	*next_byte = (offset + elem_header_size + name_len + data_len);
	return 0;
}

/*
 *  Commit changes in the 'edit' copy back to the 'working' copy.  Basically,
 *  this adds the checksum, and if that is successful, copies the entire
 *  contents of the 'edit' copy to the 'working' copy, then sets the 'dirty'
 *  flag and clears the 'edit' flag.
 *
 *  This does nothing (successfully) if not in edit mode.
 */
static int nvram_edit_commit(struct ns2_nvram_backing_store_info_s *info)
{
	struct nvram_header_s *nvram_header;
	u8 *nvram;
	u32 limit;
	int ret;

	if (!(info->nvram_flags & NVRAM_FLAG_EDITING)) {
		DEBUG_OUT(("%s[%u]: not editing; nothing to do\n",
			   __func__, __LINE__));
		/*
		 *  We do not return an error here because we want this to be
		 *  construed as NOP instead of fault.
		 */
		return 0;
	}
	/* was editing, so generate the checksum */
	DEBUG_OUT(("%s[%u]: generate checksum\n", __func__, __LINE__));
	nvram = info->edit_copy;
	nvram_header = (struct nvram_header_s *)nvram;
	limit = (le32_to_cpu(nvram_header->nvram_size) -
		 le32_to_cpu(nvram_header->nvram_checksum_size));
	ret = info->checksum(nvram, limit, &nvram[limit],
			     le32_to_cpu(nvram_header->nvram_checksum_size));
	if (ret) {
		ERROR_OUT(("%s[%u]: checksum function failed %d\n",
			   __func__, __LINE__, ret));
		return ret;
	}
	/* now the 'edit' buffer is a complete NVRAM image */
	DEBUG_OUT(("%s[%u]: move 'edit' copy to 'working' copy\n",
		   __func__, __LINE__));
	memcpy(info->working_copy,
	       info->edit_copy, le32_to_cpu(nvram_header->nvram_size));
	info->nvram_flags &= (~(NVRAM_FLAG_EDITING | NVRAM_FLAG_CLEARED));
	info->nvram_flags |= NVRAM_FLAG_DIRTY;
	return 0;
}

/*
 *  This sets an element in NVRAM.  If the element already exists, it will be
 *  replaced; if not, it will be added.
 *
 *  It is valid to provide zero-length data (and a NULL data_buffer value is
 *  okay in this case), but the name must be at least one byte long (so must
 *  also have a valid name_buffer).
 */
static int nvram_element_set(struct ns2_nvram_backing_store_info_s *info,
			     const u8 *name_buffer,
			     u32 name_len, const u8 *data_buffer, u32 data_len)
{
	u32 next_byte;
	int ret;

	if ((!info) ||
	    (!info->working_copy) ||
	    (!info->edit_copy) ||
	    (!name_buffer) || (!name_len) || (data_len && (!data_buffer))) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	ret = nvram_copy_elements_except_by_name(info,
						 name_buffer,
						 name_len, &next_byte);
	if (ret)
		/* called emitted diagnostics */
		return ret;
	ret = nvram_add_new_element(info,
				    name_buffer,
				    name_len,
				    data_buffer, data_len, &next_byte);
	if (ret)
		/* called emitted diagnostics */
		return ret;
	/* called will emit diagnostics */
	return nvram_edit_commit(info);
}

/*
 *  This removes an element in NVRAM.  If the element does not exist, it will
 *  not complain (but it will be considered as updating).
 *
 *  It is valid to provide zero-length data (and a NULL data_buffer value is
 *  okay in this case), but the name must be at least one byte long (so must
 *  also have a valid name_buffer).
 */
static int nvram_element_unset(struct ns2_nvram_backing_store_info_s *info,
			       const u8 *name_buffer, u32 name_len)
{
	int ret;

	if ((!info) ||
	    (!info->working_copy) ||
	    (!info->edit_copy) || (!name_buffer) || (!name_len)) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	ret = nvram_copy_elements_except_by_name(info,
						 name_buffer, name_len, NULL);
	if (ret)
		/* called emitted diagnostics */
		return ret;
	/* called will emit diagnostics */
	return nvram_edit_commit(info);
}

/*
 *  Build an empty NVRAM image (no elements but valid).
 */
static int nvram_purge_all(struct ns2_nvram_backing_store_info_s *info)
{
	struct nvram_header_s *nvram_header;

	if ((!info) || (!info->working_copy) || (!info->edit_copy)) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	DEBUG_OUT(("%s[%u]: clearing NVRAM of all elements\n",
		   __func__, __LINE__));
	nvram_header = (struct nvram_header_s *)info->edit_copy;
	info->nvram_flags |= NVRAM_FLAG_EDITING;
	memset(info->edit_copy, 0x00, info->nvram_size);
	nvram_header->nvram_magic = cpu_to_le64(NVRAM_HEADER_MAGIC);
	nvram_header->nvram_version = cpu_to_le32(NVRAM_HEADER_VERSION);
	nvram_header->nvram_size = cpu_to_le32(info->nvram_size);
	nvram_header->nvram_header_size = cpu_to_le32(sizeof(*nvram_header));
	nvram_header->elem_header_size =
	    cpu_to_le32(sizeof(struct nvram_elem_header_s));
	nvram_header->nvram_checksum_size =
	    cpu_to_le32(info->nvram_checksum_size);
	/* called will emit diagnostics */
	return nvram_edit_commit(info);
}

/*
 *  Get information about NVRAM
 *
 *  Any outbound arguments that are NULL will not be provided.
 *
 *  The 'used' byte count includes overhead and padding.
 *
 *  NOTE: this assumes the NVRAM image provided is valid.
 */
static int nvram_info_get(struct ns2_nvram_backing_store_info_s *info,
			  u32 *nvram_flags,
			  u32 *nvram_size,
			  u32 *nvram_elements, u32 *nvram_used)
{
	struct nvram_header_s *nvram_header;
	struct nvram_elem_header_s *elem_header;
	u8 *nvram;
	u32 offset;
	u32 limit;
	u32 nvram_checksum_size;
	u32 elem_header_size;
	u32 elem_name_size;
	u32 elem_data_size;

	if (!info) {
		ERROR_OUT(("%s[%u]: invalid argument(s)\n",
			   __func__, __LINE__));
		return -EINVAL;
	}
	nvram_header = (struct nvram_header_s *)(info->working_copy);
	elem_header_size = le32_to_cpu(nvram_header->elem_header_size);
	nvram_checksum_size = le32_to_cpu(nvram_header->nvram_checksum_size);
	if (nvram_flags)
		*nvram_flags = info->nvram_flags;
	if (nvram_size)
		*nvram_size = le32_to_cpu(nvram_header->nvram_size);
	if (nvram_elements)
		*nvram_elements = le32_to_cpu(nvram_header->nvram_elements);
	if (nvram_used) {
		DEBUG_OUT(("%s[%u]: scan elements\n", __func__, __LINE__));
		nvram = info->working_copy;
		nvram_header = (struct nvram_header_s *)nvram;
		/* scan the elements */
		offset = le32_to_cpu(nvram_header->nvram_header_size);
		limit = (le32_to_cpu(nvram_header->nvram_size) -
			 nvram_checksum_size);
		do {
			/* element headers are always quadbyte aligned */
			offset = ((offset + 3) & (~3));
			elem_header =
			    (struct nvram_elem_header_s *)&nvram[offset];
			elem_name_size =
			    le32_to_cpu(elem_header->elem_name_size);
			elem_data_size =
			    le32_to_cpu(elem_header->elem_data_size);
			if (!elem_name_size)
				/* zero name length is past end of list */
				break;
			/* look at the next element */
			offset += (elem_header_size +
				   elem_name_size + elem_data_size);
		} while (offset < limit);
		/* account for checksum */
		offset += nvram_checksum_size;
		*nvram_used = offset;
	}
	return 0;
}

/******************************************************************************
 *
 *  Exposed functionality
 *
 */

/*
 *  Open the NVRAM backing store
 *
 *  If fallback is nonzero, and backup copy is supported, this will prefer the
 *  backup copy to the working copy (so it will try the backup copy first and
 *  fall back to the working copy if it is not valid).  If backup copy is not
 *  supported or not valid, the working copy will be used.
 */
#define NNOBSRF0 "%s[%u]: can't read NVRAM %s copy from backup store: %d\n"
#define NNOBSRF1 "%s[%u]: can't read NVRAM working copy from storage: %d\n"
#define NNOBSC0 "%s[%u]: NVRAM0 %s copy backup store corrupt: %d\n"
#define NNOBSC1 "%s[%u]: NVRAM working copy backup store corrupt: %d\n"
#define NNOBSRF2 "%s[%u]: can't read NVRAM %s copy from backup store: %d\n"
#define NNOBSC2 "%s[%u]: NVRAM2 %s copy backup store corrupt: %d\n"
#define NNOBSC3 "%s[%u]: NVRAM3 backup corrupt; considering NVRAM clear\n"
#define NNOBSBCF0 "%s[%u]: unable to build clear NVRAM state: %d\n"
#define NNOBSC4 "%s[%u]: NVRAM4 backup corrupt; considering NVRAM clear\n"
#define NNOBSAF "%s[%u]: unable to obtain access to NVRAM backup store: %d\n"
#define NNOBSBCF1 "%s[%u]: unable to build clear NVRAM state: %d\n"
int ns2_nvram_open(int fallback)
{
	int ret = 0;
	int tmp = -ENOENT;
	int pri = 0;
#if NS2_NVRAM_HAS_BACKUP
	int sec = 1;
#endif				/* NS2_NVRAM_HAS_BACKUP */

	if (ns2_nvram_info.backing_store_priavte) {
		/* but it's already open! */
		DEBUG_OUT(("%s[%u]: NVRAM is already open\n",
			   __func__, __LINE__));
		/*
		 *  We do not set an error code here because we want this to be
		 *  construed as NOP instead of fault.
		 */
		goto error;
	}

	DEBUG_OUT(("%s[%u]: prepare buffer\n", __func__, __LINE__));
	/* make sure it is completely clear */
	memset(&ns2_nvram_info, 0x00, sizeof(ns2_nvram_info));
	/* but set up the necessary fields */
	ns2_nvram_info.checksum = ns2_nvram_crc32;
	ns2_nvram_info.nvram_checksum_size = sizeof(u32);
	ns2_nvram_info.nvram_size = NS2_NVRAM_SIZE;
	/* make sure we can have enough memory */
	ns2_nvram_info.working_copy = malloc(NS2_NVRAM_SIZE);
	if (!ns2_nvram_info.working_copy) {
		ERROR_OUT(("%s[%u]: unable to allocate working copy buffer\n",
			   __func__, __LINE__));
		ret = ENOMEM;
		goto error;
	}
	ns2_nvram_info.edit_copy = malloc(NS2_NVRAM_SIZE);
	if (!ns2_nvram_info.edit_copy) {
		ERROR_OUT(("%s[%u]: unable to allocate edit copy buffer\n",
			   __func__, __LINE__));
		ret = ENOMEM;
		goto error;
	}
#if NS2_NVRAM_HAS_BACKUP
	ns2_nvram_info.temp_copy = malloc(NS2_NVRAM_SIZE);
	if (!ns2_nvram_info.temp_copy) {
		ERROR_OUT(("%s[%u]: unable to allocate temp copy buffer\n",
			   __func__, __LINE__));
		ret = ENOMEM;
		goto error;
	}
	ns2_nvram_info.nvram_flags |= NVRAM_FLAG_HAS_BACKUP_COPY;
	if (fallback) {
		DEBUG_OUT(("%s[%u]: will prefer NVRAM backup copy\n",
			   __func__, __LINE__));
		pri = 1;
		sec = 0;
	}
#endif				/* NS2_NVRAM_HAS_BACKUP */
	ret = ns2_nvram_backing_store_open(&ns2_nvram_info);
	if (ret) {
		ERROR_OUT((NNOBSAF, __func__, __LINE__, ret));
		goto error;
	}
	ret = ns2_nvram_backing_store_read(&ns2_nvram_info,
					   pri, ns2_nvram_info.working_copy);
	if (ret)
#if NS2_NVRAM_HAS_BACKUP
		ERROR_OUT((NNOBSRF0,
			   __func__,
			   __LINE__, pri ? "backup" : "working", ret));
#else				/* NS2_NVRAM_HAS_BACKUP */
		ERROR_OUT((NNOBSRF1, __func__, __LINE__, ret));
#endif				/* NS2_NVRAM_HAS_BACKUP */
	/* but don't bail out just yet */
	else {
		ret = nvram_copy_validate(&ns2_nvram_info,
					  ns2_nvram_info.working_copy,
					  NS2_NVRAM_SIZE,
					  ns2_nvram_info.checksum,
					  ns2_nvram_info.nvram_checksum_size);
		if (ret) {
#if NS2_NVRAM_HAS_BACKUP
			ERROR_OUT((NNOBSC0,
				   __func__,
				   __LINE__, pri ? "backup" : "working", ret));
#else				/* NS2_NVRAM_HAS_BACKUP */
			ERROR_OUT((NNOBSC1, __func__, __LINE__, ret));
#endif				/* NS2_NVRAM_HAS_BACKUP */
			ret = -EIO;
		}
	}
	if (ret) {
		/* preferred copy is bad */
#if NS2_NVRAM_HAS_BACKUP
		if (pri)
			/* preferred copy was the backup copy */
			ns2_nvram_info.nvram_flags |=
			    NVRAM_FLAG_BACKUP_COPY_BAD;
		else
#endif				/* NS2_NVRAM_HAS_BACKUP */
			/* preferred copy was the working copy */
			ns2_nvram_info.nvram_flags |=
			    NVRAM_FLAG_PRIMARY_COPY_BAD;
#if NS2_NVRAM_HAS_BACKUP
#endif				/* NS2_NVRAM_HAS_BACKUP */
	}
#if NS2_NVRAM_HAS_BACKUP
	tmp = ns2_nvram_backing_store_read(&ns2_nvram_info,
					   sec, ns2_nvram_info.edit_copy);
	if (tmp)
		ERROR_OUT((NNOBSRF2,
			   __func__,
			   __LINE__, sec ? "backup" : "working", tmp));
	/* but don't bail out just yet */
	else {
		/* the read went okay; but check that it's valid */
		tmp = nvram_copy_validate(&ns2_nvram_info,
					  ns2_nvram_info.edit_copy,
					  NS2_NVRAM_SIZE,
					  ns2_nvram_info.checksum,
					  ns2_nvram_info.nvram_checksum_size);
		if (tmp) {
			ERROR_OUT((NNOBSC2,
				   __func__,
				   __LINE__, sec ? "backup" : "working", tmp));
			tmp = -EIO;
		}
	}
	if (tmp) {
		/* fallback copy is bad */
		if (sec) {
			/* fallback copy is the backup copy */
			ns2_nvram_info.nvram_flags |=
			    NVRAM_FLAG_BACKUP_COPY_BAD;
		} else {
			/* fallback copy is the working copy */
			ns2_nvram_info.nvram_flags |=
			    NVRAM_FLAG_PRIMARY_COPY_BAD;
		}
	}
#endif				/* NS2_NVRAM_HAS_BACKUP */
#if NS2_NVRAM_HAS_BACKUP
	if (ret) {
		/* the preferred copy was bad */
		if (tmp) {
			/* both copies are bad */
			ERROR_OUT((NNOBSC3, __func__, __LINE__));
			ret = nvram_purge_all(&ns2_nvram_info);
			if (ret) {
				ERROR_OUT((NNOBSBCF0, __func__, __LINE__, ret));
				goto error;
			}
			ns2_nvram_info.nvram_flags |= NVRAM_FLAG_CLEARED;
		} else {
			/* preferred copy bad but the other was okay; use it */
			DEBUG_OUT(("%s[%u]: using %s copy of NVRAM\n",
				   __func__,
				   __LINE__, sec ? "backup" : "working"));
			memcpy(ns2_nvram_info.working_copy,
			       ns2_nvram_info.edit_copy, NS2_NVRAM_SIZE);
			if (sec)
				/* we're using the backup copy */
				ns2_nvram_info.nvram_flags |=
				    NVRAM_FLAG_USING_BACKUP_COPY;
		}
	} else {
		/* the preferred copy was good */
		if (pri)
			/* ...but it was the backup copy */
			ns2_nvram_info.nvram_flags |=
			    NVRAM_FLAG_USING_BACKUP_COPY;
	}
#else				/* NVRAM_NS2_SPI_FLASH_HAS_BACKUP */
	if (ret) {
		/* primary copy was bad an there is no backup */
		ERROR_OUT((NNOBSC4, __func__, __LINE__));
		ret = nvram_purge_all(&ns2_nvram_info);
		if (ret) {
			ERROR_OUT((NNOBSBCF1, __func__, __LINE__, ret));
			goto error;
		}
		ns2_nvram_info.nvram_flags |= NVRAM_FLAG_CLEARED;
	}
#endif				/* NVRAM_NS2_SPI_FLASH_HAS_BACKUP */
	/* even if we fell back to cleared, it's not really an error, is it? */
	ns2_nvram_info.nvram_flags |= NVRAM_FLAG_OPEN;
	ret = 0;
	/* make sure the function pointers are for the proper handlers */
 error:
	if (ret) {
		/* something went wrong; clean up */
		if (ns2_nvram_info.backing_store_priavte)
			ns2_nvram_backing_store_open(&ns2_nvram_info);
		if (ns2_nvram_info.temp_copy)
			free(ns2_nvram_info.temp_copy);
		if (ns2_nvram_info.edit_copy)
			free(ns2_nvram_info.edit_copy);
		if (ns2_nvram_info.working_copy)
			free(ns2_nvram_info.working_copy);
		memset(&ns2_nvram_info, 0x00, sizeof(ns2_nvram_info));
	}
	return ret;
}

/*
 *  Commit the current NVRAM state to the backing store
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
#define NNCRPCF "%s[%u]: unable to read current primary copy from NVRAM: %d\n"
#define NNCSPABF "%s[%u]: can't save curr primary copy as new backup: %d\n"
#define NNCSCABF "%s[%u]: can't save curr settings over corrupt backup: %d\n"
#define NNCSCAPF "%s[%u]: can't save current settings to primary copy: %d\n"
int ns2_nvram_commit(void)
{
	int ret = 0;

#if NS2_NVRAM_HAS_BACKUP
	/*
	 *  We expect to have a backup copy.
	 *
	 *  If the working copy in backing store is valid, copy it to the
	 *  backup copy in backing store first.  That way, there should be a
	 *  good copy (even if a little older) in backing store as fallback.
	 */
	if (ns2_nvram_info.nvram_flags & NVRAM_FLAG_HAS_BACKUP_COPY) {
		if (!(ns2_nvram_info.nvram_flags &
		      NVRAM_FLAG_PRIMARY_COPY_BAD)) {
			/*
			 *  We have a backup copy, and the working copy is
			 *  good, so copy the working copy over the current
			 *  backup copy so the fallback position is the most
			 *  recent one that we tried.
			 */
			ret = ns2_nvram_backing_store_read(&ns2_nvram_info,
							   0 /* primary */ ,
							   ns2_nvram_info.
							   temp_copy);
			if (ret) {
				ERROR_OUT((NNCRPCF, __func__, __LINE__, ret));
#if NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID
				return ret;
#endif				/* NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID */
			} else {
				ret =
				    ns2_nvram_backing_store_write
				    (&ns2_nvram_info, 1 /* backup */ ,
				     ns2_nvram_info.temp_copy);
				if (ret) {
					ERROR_OUT((NNCSPABF,
						   __func__, __LINE__, ret));
					ns2_nvram_info.nvram_flags |=
					    NVRAM_FLAG_BACKUP_COPY_BAD;
#if NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID
					return ret;
#endif				/* NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID */
				} else
					ns2_nvram_info.nvram_flags &=
					    (~NVRAM_FLAG_BACKUP_COPY_BAD);
			}
		} else if (ns2_nvram_info.nvram_flags &
			   NVRAM_FLAG_BACKUP_COPY_BAD) {
			/*
			 *  We have a backup copy, but it is bad and the
			 *  working copy is bad.  There's no point in copying a
			 *  bad working copy to the backup copy, so write the
			 *  current state to the backup copy instead.
			 */
			ret = ns2_nvram_backing_store_write(&ns2_nvram_info,
							    1 /* backup */ ,
							    ns2_nvram_info.
							    working_copy);
			if (ret) {
				ERROR_OUT((NNCSCABF, __func__, __LINE__, ret));
#if NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID
				return ret;
#endif				/* NS2_NVRAM_BACKING_STORE_BACKUP_PARANOID */
			} else
				ns2_nvram_info.nvram_flags &=
				    (~NVRAM_FLAG_BACKUP_COPY_BAD);
		}
	}
#endif				/* NS2_NVRAM_HAS_BACKUP */
	/* Write the current settings to the working copy in backing store */
	ret = ns2_nvram_backing_store_write(&ns2_nvram_info, 0 /* primary */ ,
					    ns2_nvram_info.working_copy);
	if (ret) {
		ERROR_OUT((NNCSCAPF, __func__, __LINE__, ret));
		ns2_nvram_info.nvram_flags |= NVRAM_FLAG_PRIMARY_COPY_BAD;
	} else {
		/*
		 *  The working copy in backing store is good and the working
		 *  copy in memory has been committed to backing store.
		 */
		ns2_nvram_info.nvram_flags &= (~(NVRAM_FLAG_PRIMARY_COPY_BAD |
						 NVRAM_FLAG_DIRTY));
	}
	return ret;
}

/*
 *  Close the NVRAM backing store
 */
#define NNCD "%s[%u]: NVRAM has been modified since commit\n"
int ns2_nvram_close(int force)
{
	int ret = 0;

	if (!ns2_nvram_info.backing_store_priavte) {
		/* but it's not open! */
		DEBUG_OUT(("%s[%u]: NVRAM not open\n", __func__, __LINE__));
		/*
		 *  The error code is not set here because we want to consider
		 *  this as NOP instead of fault.
		 */
		goto error;
	}
	if (ns2_nvram_info.nvram_flags & (NVRAM_FLAG_EDITING |
					  NVRAM_FLAG_DIRTY)) {
		/* only a problem if not forcing close */
		if (!force) {
			ERROR_OUT((NNCD, __func__, __LINE__));
			ret = -EBUSY;
			goto error;
		}
	}
	/* it looks okay; go ahead and close it */
	ret = ns2_nvram_backing_store_close(&ns2_nvram_info);
	if (ret) {
		ERROR_OUT(("%s[%u]: error closing backing store: %d\n",
			   __func__, __LINE__, ret));
	}
	/* clean up so we know it's closed and no longer valid */
	if (ns2_nvram_info.temp_copy)
		free(ns2_nvram_info.temp_copy);
	if (ns2_nvram_info.edit_copy)
		free(ns2_nvram_info.edit_copy);
	if (ns2_nvram_info.working_copy)
		free(ns2_nvram_info.working_copy);
	memset(&ns2_nvram_info, 0x00, sizeof(ns2_nvram_info));
 error:
	return ret;
}

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
				   u32 data_max_len, u32 *data_len)
{
	return nvram_element_get_by_index(&ns2_nvram_info,
					  index,
					  name_buffer,
					  name_max_len,
					  name_len,
					  data_buffer, data_max_len, data_len);
}

/*
 *  Find an element in the NVRAM by name
 */
int ns2_nvram_element_get(const u8 *name_buffer,
			  u32 name_len,
			  u8 *data_buffer, u32 data_max_len, u32 *data_len)
{
	return nvram_element_get(&ns2_nvram_info,
				 name_buffer,
				 name_len, data_buffer, data_max_len, data_len);
}

/*
 *  Set an element in the NVRAM by name
 */
int ns2_nvram_element_set(const u8 *name_buffer,
			  u32 name_len, const u8 *data_buffer, u32 data_len)
{
	return nvram_element_set(&ns2_nvram_info,
				 name_buffer, name_len, data_buffer, data_len);
}

/*
 *  Remove an element fron NVRAM by name
 */
int ns2_nvram_element_unset(const u8 *name_buffer, u32 name_len)
{
	return nvram_element_unset(&ns2_nvram_info, name_buffer, name_len);
}

/*
 *  Purge the NVRAM completely (remove all elements).
 */
int ns2_nvram_purge_all(void)
{
	return nvram_purge_all(&ns2_nvram_info);
}

/*
 *  Get information about the NVRAM state
 *
 *  This fills in all zeroes in the case of the NVRAM not being open, but it
 *  also returns -ENODATA in that case.
 */
int ns2_nvram_info_get(u32 *nvram_flags,
		       u32 *nvram_size, u32 *nvram_elements, u32 *nvram_used)
{
	if (ns2_nvram_info.backing_store_priavte)
		return nvram_info_get(&ns2_nvram_info,
				      nvram_flags,
				      nvram_size, nvram_elements, nvram_used);
	else {
		if (nvram_flags)
			*nvram_flags = 0;
		if (nvram_size)
			*nvram_size = 0;
		if (nvram_elements)
			*nvram_elements = 0;
		if (nvram_used)
			*nvram_used = 0;
		return -ENODATA;
	}
}
