/*
 * Configuration for Broadcom NS2.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __BCM_NORTHSTAR2_H
#define __BCM_NORTHSTAR2_H

/* PCI/PCIE */
#define CONFIG_PCI
#define CONFIG_BCM_NS2_PCIE
#define CONFIG_CMD_PCI
#define CONFIG_PCI_PNP
#define CONFIG_PCI_SCAN_SHOW

#define CONFIG_BCM_SDHCI
#define CONFIG_SDHCI
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_EMMC_INIT

/* RTC SUPPORT */
#define CONFIG_CMD_DATE
#define CONFIG_RTC_NS2

#define CONFIG_CMD_SCSI
#define CONFIG_SCSI_DEV_LIST
#define CONFIG_LIBATA
#define CONFIG_SCSI_AHCI
#define CONFIG_SCSI_AHCI_PLAT
#define CONFIG_DOS_PARTITION

#ifndef CONFIG_SYS_SCSI_MAX_SCSI_ID
#define CONFIG_SYS_SCSI_MAX_SCSI_ID 2
#endif
#ifndef CONFIG_SYS_SCSI_MAX_LUN
#define CONFIG_SYS_SCSI_MAX_LUN 1
#endif
#ifndef CONFIG_SYS_SCSI_MAX_DEVICE
#define CONFIG_SYS_SCSI_MAX_DEVICE	(CONFIG_SYS_SCSI_MAX_SCSI_ID *	\
						CONFIG_SYS_SCSI_MAX_LUN)
#endif
#ifndef CONFIG_BOARD_LATE_INIT
#define CONFIG_BOARD_LATE_INIT
#endif
#ifndef CONFIG_SYS_64BIT_LBA
#define CONFIG_SYS_64BIT_LBA
#endif
#ifndef CONFIG_PARTITION_UUIDS
#define CONFIG_PARTITION_UUIDS
#endif

/* ethernet */
#define CONFIG_NET_API
#define CONFIG_PHYLIB
#define CONFIG_PHY_BCM5481
#define CONFIG_BCM_CMIC_MDIO
#define CONFIG_BCMIPROC_ETH
#define CONFIG_GMAC_NUM				0
#define IPROC_ETH_MALLOC_BASE			0x83000000
#define CONFIG_EXTERNAL_PHY_BUS_ID		0x0
#define CONFIG_EXTERNAL_PHY_DEV_ID		0x10

#define CONFIG_BCM_CMIC_MDIO
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_HOSTNAME				northstar2
#define CONFIG_UBI_PART				ubi0
#define CONFIG_UBIFS_VOLUME			rootfs
#define CONFIG_LZO

/* BSC1 */
#define I2C_IOMUX_ADDR_U73			0x70  /* SVK */
#define I2C_IOMUX_ADDR_U7			0x70  /* XMC */
#define I2C_SFP_CNTL_U74			0x24  /* SVK */
#define I2C_TLV32X_I2S_DC			0x30  /* un-used */
#define CONFIG_SYS_I2C_PCA9554_U8		0x21  /* XMC */
/* BSC0 */
#define I2C_IOMUX_ADDR_U68			0x70
#define I2C_SFP_ADDR				0x50
#define CONFIG_SYS_I2C_PCF8574_U86		0x20
#define CONFIG_SYS_I2C_PCF8574_U92		0x21
#define CONFIG_SYS_I2C_PCF8574_U100		0x22
#define CONFIG_SYS_I2C_PCF9505_U41		0x23
#define CONFIG_SYS_I2C_PCF9505_U40		0x24

/* USB */
#define CONFIG_USB_GADGET
#define CONFIG_USB_GADGET_DUALSPEED
#define CONFIG_USB_GADGET_VBUS_DRAW		0
#define CONFIG_USBDOWNLOAD_GADGET
#define CONFIG_G_DNL_VENDOR_NUM			0x18D1/*google*/
#define CONFIG_G_DNL_PRODUCT_NUM		0x0D02/*nexus one*/
#define CONFIG_G_DNL_MANUFACTURER		"Broadcom"
#define CONFIG_USB_EHCI_BCM_NS2
/*For now enable only 2 USB2.0 ports*/
#define CONFIG_USB_MAX_CONTROLLER_COUNT         3
#define CONFIG_USB_EHCI
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION

/* USB3 Stack for Diags */
#define CONFIG_USB_XHCI_IPROC

#ifndef CONFIG_EFI_PARTITION
#define CONFIG_EFI_PARTITION
#endif /* ndef CONFIG_EFI_PARTITION */
#ifndef CONFIG_EXT2
#define CONFIG_EXT2
#endif /* ndef CONFIG_EXT2 */
#ifndef CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT2
#endif /* ndef CONFIG_CMD_EXT2 */
#ifndef CONFIG_FS_EXT4
#define CONFIG_FS_EXT4
#endif /* ndef CONFIG_FS_EXT4 */
#ifndef CONFIG_EXT4_WRITE
#define CONFIG_EXT4_WRITE
#endif /* ndef CONFIG_EXT4_WRITE */
#ifndef CONFIG_CMD_EXT4
#define CONFIG_CMD_EXT4
#endif /* ndef CONFIG_CMD_EXT4 */

/* /\* Fastboot *\/ */
/* #define CONFIG_CMD_FASTBOOT */
/* #define CONFIG_FASTBOOT_FLASH */
/* #define CONFIG_USB_FASTBOOT_BUF_ADDR           CONFIG_SYS_LOAD_ADDR */
/* #define CONFIG_USB_FASTBOOT_BUF_SIZE           0x12C00000 */
/* #define CONFIG_SYS_CACHELINE_SIZE                      64 */
/* #define CONFIG_DFU_FUNCTION */

#define BL0_PARTITION_SIZE                        0x00080000
#define FIP_PARTITION_SIZE                        0x00150000
#define UBOOT_ENV_PARTITION_SIZE                  0x00020000
#define DTB_PARTITION_SIZE                        0x00010000
#define KERNEL_PARTITION_SIZE                     0x00E00000
#define ROOTFS_PARTITION_SIZE                     0x01000000

#define BL0_PARTITION_OFFSET                      0x00000000
#define FIP_PARTITION_OFFSET                      0x00080000
#define UBOOT_ENV_PARTITION_OFFSET                0x001E0000
#define DTB_PARTITION_OFFSET                      0x001F0000
#define KERNEL_PARTITION_OFFSET                   0x00200000
#define ROOTFS_PARTITION_OFFSET                   0x01000000
#define CONFIG_BOARD_LATE_INIT

#define CONFIG_SYS_NO_FLASH

/* Environment variables for NAND flash */
#define CONFIG_NAND_FLASH
#define CONFIG_CMD_NAND
#define CONFIG_IPROC_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE		1
#define CONFIG_SYS_NAND_BASE			0xdeadbeef
#define CONFIG_SYS_ONENAND_BASE			CONFIG_SYS_NAND_BASE
#define IPROC_NAND_MEM_BASE			0x78000000
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_INITRD_TAG			1
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_SUPPORT_RAW_INITRD
#ifndef CONFIG_BCM_NS2_CUPS_DDR4
#define BRCM_NAND_CONTROLLER_16_BIT_WIDTH_ISSUE_FIX
#endif
#define IPROC_NAND_DEBUG_CALLS

/* Cache Definitions */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_ICACHE_OFF

#define CONFIG_IDENT_STRING			" Broadcom Northstar2"
#define CONFIG_BOOTP_VCI_STRING			"U-boot.armv8.bcm_ns2"

/* Flat Device Tree Definitions */
#define CONFIG_CMD_FDT
#define CONFIG_OF_LIBFDT
#define CONFIG_SYS_BOOTM_LEN			0x4000000

/* SMP Spin Table Definition */
#define CPU_RELEASE_ADDR			(0x84B00000)

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY			(0x17D7840) /* 25MHz */

/* Generic Interrupt Controller Definitions */
#define CONFIG_GICV2
#define GICD_BASE				(0x65210000)
#define GICC_BASE				(0x65220000)

/* Pen-Holder Locations for CPUs */
/* CPU-0 (Primary) */
#define PEN_HOLDER_ADDR_CPU0			0x67D60FC0
/* CPU (Secondary) */
#define PEN_HOLDER_ADDR_CPU1			0x67D60FC8
#define PEN_HOLDER_ADDR_CPU2			0x67D60FD0
#define PEN_HOLDER_ADDR_CPU3			0x67D60FD8

/******** Physical Memory Map **************/
#ifdef CONFIG_PALLADIUM_EMULATION
#define V2M_BASE				0x0000000085000000
#define CONFIG_NR_DRAM_BANKS			1
#define PHYS_SDRAM_1				V2M_BASE	/*SDRAM Bank 1*/
#define PHYS_SDRAM_1_SIZE			0x2000000	/* 32 MB */
#define CONFIG_SYS_SDRAM_BASE			PHYS_SDRAM_1

/* define text_base for U-boot image */
#define CONFIG_SYS_TEXT_BASE			0x0000000085000000
#define CONFIG_SYS_INIT_SP_ADDR			(PHYS_SDRAM_1 + 0x7fff0)
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN			(CONFIG_ENV_SIZE + (8 << 20))
/* Miscellaneous download areas */
#define CONFIG_SYS_LOAD_ADDR			(PHYS_SDRAM_1 + 0x0080000)
#endif

#ifdef CONFIG_TARGET_NS2_SVK
#ifdef CONFIG_NS2_SRAM_BOOT	/* debug purpose */
/* All we use is SRAM here */
#define V2M_BASE		0x0000000065100000
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1			V2M_BASE
#define PHYS_SDRAM_1_SIZE		(512 * 1024) /* portion of SRAM */
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
/* define text_base for U-boot image */
#define CONFIG_SYS_TEXT_BASE	0x0000000065100000
/* QSPI XIP mode - experimental! */
#define CONFIG_SYS_INIT_SP_ADDR (PHYS_SDRAM_1 + 0x7fff0)
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		(16 * 1024)	/*Minimal heap*/
/* Miscellaneous download areas */
#define CONFIG_SYS_LOAD_ADDR    (PHYS_SDRAM_1 + 0x0080000)

#else	/***********Execute from DDR****************/

/*
 *  UBoot does not init memory controller so does not need physical memory
 *  description. Instead, it does try to tell the OS what memory there is, so
 *  it needs a more use-oriented description.
 *
 *  Memory listed below is reserved for the stated purpose.  Split memory into
 *  multiple regions to avoid giving the OS any of these areas, since sometimes
 *  the '/memreserve/' or 'reserved memory' node in the DT is not handled as
 *  well as we would hope.  This also prevents UBoot automatically doing things
 *  with memory that should not be used, but it does not prevent the user from
 *  doing so.  Hard to prevent that.
 *
 *    0x00000000_81000000 through 0x00000000_811FFFFF -- ChiMP/Nitro
 *    0x00000000_82200000 through 0x00000000_828FFFFF -- Trusted FW
 */
#define CONFIG_NR_DRAM_BANKS	3
#define PHYS_SDRAM_1		0x0000000080000000
#define PHYS_SDRAM_1_SIZE	0x0000000001000000
#define PHYS_SDRAM_2		0x0000000081200000
#define PHYS_SDRAM_2_SIZE	0x0000000000600000
#define PHYS_SDRAM_3		0x0000000085000000
#if !defined(CONFIG_BCM_NS2_SVK_DDR4) && !defined(CONFIG_BCM_NS2_SVK_XMC)
/* DRAM ends at 8GiB from base (10GiB physical) */
#define PHYS_SDRAM_3_SIZE	(0x0000000280000000 - PHYS_SDRAM_3)
#else
/* DRAM ends at 14GiB from base (16GiB physical) */
#define PHYS_SDRAM_3_SIZE	(0x0000000400000000 - PHYS_SDRAM_3)
#endif
#define V2M_BASE			PHYS_SDRAM_1
#define CONFIG_SYS_SDRAM_BASE		PHYS_SDRAM_1
/* define text_base for U-boot image */
#define CONFIG_SYS_TEXT_BASE		0x0000000085000000
#define CONFIG_SYS_INIT_SP_ADDR		(PHYS_SDRAM_1 + 0x7ff00)
#define CONFIG_SYS_LOAD_ADDR		0x0000000090000000
#define CONFIG_SYS_MALLOC_LEN		(20 * 1024 * 1024)
#endif
#endif

/* Serial Configuration */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	 (-4)
#define CONFIG_SYS_NS16550_CLK		(25 * 1000 * 1000)
#define CONFIG_SYS_NS16550_COM1		 0x66100000
#define CONFIG_SYS_NS16550_COM2		 0x66110000
#define CONFIG_SYS_NS16550_COM3		 0x66120000
#define CONFIG_SYS_NS16550_COM4		 0x66130000

#define CONFIG_BAUDRATE			115200

#define CONFIG_CONS_INDEX               4
/* I2C */
#define CONFIG_SYS_I2C_BRCM
#define CONFIG_SYS_I2C_SPEED    0       /* Default on 100KHz */
#define CONFIG_SYS_I2C_SLAVE    0xff    /* No slave address */
#define CONFIG_I2C_MULTI_BUS    1

#define CONFIG_CMD_MEMINFO

/* Initial environment variables */
#undef CONFIG_CMD_IMLS
#define CONFIG_EXTRA_ENV_SETTINGS	\
		"ethaddr=00:10:19:D0:B2:A4\0"		\
		"ipaddr=10.31.5.228\0"			\
		"netmask=255.255.255.0\0"		\
		"console=ttyS0\0"           \
		"initrd_high=0xFFFFFFFF\0" \
"loglevel=7\0"              \
"nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs\0"        \
"fdt_high=0xFFFFFFFFFFFFFFFF\0"   \
"mtd_nand=brcmnand.0:2560K(nboot),256K(nenv),256K(ndtb),61M(nsystem)," \
"960M(nrootfs)\0"	\
"mtd_qspi=spi32766.0:512K(bl1),1344K(fip),128K(env),64K(dtb),14M(kernel),"\
"16M(rootfs)\0"	\
"flashfsargs=setenv bootargs loglevel=${loglevel} console=${console},${baudrate}n8 "  \
"ubi.mtd=nrootfs root=ubi0 rootfstype=ubifs mtdparts=${mtd_nand}\0"   \
"nfsargs=setenv bootargs "  \
"loglevel=${loglevel} " \
"console=${console},${baudrate}n8 "   \
"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:northstar2:: " \
"maxcpus=1 "            \
"root=${nfsroot}\0"      \
"mmcbootargs=setenv bootargs "\
"loglevel=${loglevel} " \
"console=${console},${baudrate}n8 "   \
"root=/dev/mmcblk1p1 rw noinitrd rootwait"	\
"ethaddr=${ethaddr}\0"	\
"mmcboot=run mmcbootargs; "	\
"ext2load mmc 1 90000000 /boot/uImage-brcm-linux.img; "	\
"ext2load mmc 1 88000000 /boot/Image-ns2-svk.dtb; " \
"bootm 90000000 - 88000000\0"

#define CONFIG_BOOTCOMMAND		"sf probe;sf read 0x90000000 0x200000 0x1600000;bootm 0x90000000"
#define CONFIG_BOOTDELAY		30

#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#define CONFIG_BCM_QSPI
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0
#define IPROC_QSPI_MEM_BASE                     0x68000000

#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 104000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3
#define CONFIG_SPI_FLASH_BAR

/*SPI*/
#define CONFIG_CMD_SPI                          1
/* SPI flash configuration, 2 flashes connected to SPI2 and SPI3 */
#define CONFIG_PL022_SPI
#define CONFIG_PL022_SPI_BUS                   1
#define CONFIG_PL022_SPI_CS                    1
#define CONFIG_SYS_SPI_CLK                     100000000 /* 100MHz */

/* current SPI flash layout is 0x0 to 0x80000 is ATF, 0x80000 to 0x1E0000
 * is fip.bin, 0x1E0000 to 0x200000 is SPI flash ENV sector
 * and kernel is from 0x200000 till 0xc00000, DTB is 0xC00000 to 1000000
 */
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_OFFSET                       0x1E0000
#define CONFIG_ENV_SPI_MAX_HZ                   104000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#define CONFIG_ENV_SIZE                         0x10000     /* 64KB */
#define CONFIG_ENV_OVERWRITE


#define CONFIG_DIAG_VER "Northstar2 Diagnostics Version: 2.3"
#define CONFIG_HAS_POST
#define CONFIG_CMD_DIAG
#define CONFIG_POST     (CONFIG_SYS_POST_MEMORY | CONFIG_SYS_POST_I2C |	\
			 CONFIG_SYS_POST_QSPI | CONFIG_SYS_POST_UART |	\
			 CONFIG_SYS_POST_NAND | CONFIG_SYS_POST_LED |	\
			 CONFIG_SYS_POST_USB20 | CONFIG_SYS_POST_GMAC |	\
			 CONFIG_SYS_POST_MDIO | CONFIG_SYS_POST_SATA |	\
			 CONFIG_SYS_POST_USB20_DEVICE_MODE |		\
			 CONFIG_SYS_POST_PCIE | CONFIG_SYS_POST_USB30 | \
			 CONFIG_SYS_POST_I2S | CONFIG_SYS_POST_EMMC |	\
			 CONFIG_SYS_POST_BCM_SPI | CONFIG_SYS_POST_TAMPER | \
			 CONFIG_SYS_POST_SPDIF)
#endif /* __BCM_NORTHSTAR2_H */
