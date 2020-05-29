/*
 * (C) Copyright 2016 Broadcom Ltd.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/ns2_nvram.h>
#include <common.h>
#include <errno.h>
#include <asm/types.h>
#include "ns2_nvram_store.h"
#include <spi.h>
#include <spi_flash.h>

/*
 *  Open access to the backing store
 */
int ns2_nvram_backing_store_open(struct ns2_nvram_backing_store_info_s *info)
{
	struct spi_flash *flash = NULL;
	int ret = 0;

	/* try to gain access to the SPI flash */
	flash = spi_flash_probe(CONFIG_IPROC_QSPI_BUS, CONFIG_IPROC_QSPI_CS,
				CONFIG_SF_DEFAULT_SPEED,
				CONFIG_SF_DEFAULT_MODE);
	if (!flash) {
		printf("%s: unable to open SPI flash\n", __func__);
		ret = -ENODEV;
		goto error;
	}
	info->backing_store_priavte = flash;
	/* make sure the NVRAM size is an integral number of erase pages */
	if (NS2_NVRAM_SIZE !=
	    (flash->erase_size *
	     (NS2_NVRAM_SIZE / flash->erase_size))) {
		printf("NVRAM size %u is not aligned to erase page %u\n",
		       NS2_NVRAM_SIZE, flash->erase_size);
		ret = -EINVAL;
	}
	/* make sure NVRAM aligns to pages */
	if (NS2_NVRAM_PRIMARY_OFFSET !=
	    (flash->erase_size *
	     (NS2_NVRAM_PRIMARY_OFFSET / flash->erase_size))) {
		printf("NVRAM primary at %08X is un-aligned to erase page %u\n",
		       NS2_NVRAM_PRIMARY_OFFSET, flash->erase_size);
		ret = -EINVAL;
	}
#if NS2_NVRAM_HAS_BACKUP
	if (NS2_NVRAM_BACKUP_OFFSET !=
	    (flash->erase_size *
	     (NS2_NVRAM_BACKUP_OFFSET / flash->erase_size))) {
		printf("NVRAM backup at %08X is not aligned to erase page %u\n",
		       NS2_NVRAM_BACKUP_OFFSET, flash->erase_size);
		ret = -EINVAL;
	}
#endif
	if (!ret) {
		/* only commit open if no error */
		info->backing_store_priavte = flash;
	}
error:
	if (ret)
		/* only do cleanup if error */
		if (flash)
			spi_flash_free(flash);
	return ret;
}

/*
 *  Close access to the backing store
 */
int ns2_nvram_backing_store_close(struct ns2_nvram_backing_store_info_s *info)
{
	struct spi_flash *flash = info->backing_store_priavte;

	if (flash) {
		spi_flash_free(flash);
		info->backing_store_priavte = NULL;
	}
	return 0;
}

/*
 *  Read from the backing store.
 *
 *  Set index = 0 for primary copy; set index = 1 for backup copy
 */
int ns2_nvram_backing_store_read(struct ns2_nvram_backing_store_info_s *info,
				 int index, u8 *data_buffer)
	{
	struct spi_flash *flash = info->backing_store_priavte;
	u32 offset;
	int ret;

	if (!flash) {
		printf("%s: SPI flash not open\n", __func__);
		return -EIO;
	}
	if (index == 0) {
		offset = NS2_NVRAM_PRIMARY_OFFSET;
#if NS2_NVRAM_HAS_BACKUP
	} else if (index == 1) {
		offset = NS2_NVRAM_BACKUP_OFFSET;
#endif /* NS2_NVRAM_HAS_BACKUP */
	} else {
		printf("%s: invalid index %u\n", __func__, index);
		return -ENOENT;
	}
	ret = spi_flash_read(flash, offset, NS2_NVRAM_SIZE, data_buffer);
	if (ret) {
		printf("NVRAM: unable to read SPI flash %08X for %u bytes\n",
		       offset, NS2_NVRAM_SIZE);
		ret = -EIO;
	}
	return ret;
}

/*
 *  Write to the backing store (erase first if necessary)
 *
 *  Set index = 0 for primary copy; set index = 1 for backup copy
 */
int ns2_nvram_backing_store_write(struct ns2_nvram_backing_store_info_s *info,
				  int index, void *data_buffer)
{
	struct spi_flash *flash = info->backing_store_priavte;
	u32 offset;
	int ret;

	if (!flash) {
		printf("%s: SPI flash not open\n", __func__);
		return -EIO;
	}
	if (index == 0) {
		offset = NS2_NVRAM_PRIMARY_OFFSET;
#if NS2_NVRAM_HAS_BACKUP
	} else if (index == 1) {
		offset = NS2_NVRAM_BACKUP_OFFSET;
#endif /* NS2_NVRAM_HAS_BACKUP */
	} else {
		printf("%s: invalid index %u\n", __func__, index);
		return -ENOENT;
	}
	ret = spi_flash_erase(flash, offset, NS2_NVRAM_SIZE);
	if (ret) {
		printf("%s: unable to erase SPI flash %08X for %u bytes\n",
		       __func__, offset, NS2_NVRAM_SIZE);
		ret = -EIO;
		goto error;
	}
	ret = spi_flash_write(flash, offset, NS2_NVRAM_SIZE, data_buffer);
	if (ret) {
		printf("%s: unable to write SPI flash %08X for %u bytes\n",
		       __func__, offset, NS2_NVRAM_SIZE);
		ret = -EIO;
		goto error;
	}
error:
	return ret;
}
