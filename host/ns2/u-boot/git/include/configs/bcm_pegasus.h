/*
 * Configuration for Broadcom NS2.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __BCM_PEGASUS_H
#define __BCM_PEGASUS_H

#include <asm/arch/socregs.h>

/*#define CONFIG_PALLADIUM_EMULATION*/
#define PEG_A0_BRING_UP
/*#define CONFIG_UBOOT_AT_EL3*/
#define CONFIG_ARMV8_MULTIENTRY

#define CONFIG_BOARD_LATE_INIT

#define CONFIG_REMAKE_ELF

#define CONFIG_SYS_NO_FLASH

/* Environment variables for NAND flash */
#define CONFIG_NAND_FLASH
/*#define NAND_INFO*/
#define CONFIG_CMD_NAND
#define CONFIG_IPROC_NAND
#define CONFIG_SYS_MAX_NAND_DEVICE                          1
#define CONFIG_SYS_NAND_BASE                        0xdeadbeef
#define CONFIG_SYS_ONENAND_BASE                     CONFIG_SYS_NAND_BASE
#define IPROC_NAND_MEM_BASE                         0x10000000
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_INITRD_TAG                       1   /* send initrd params */
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_SUPPORT_RAW_INITRD
/*This can be enabled if u-boot doesn't want to mark the sectors as bad */
/*#define NAND_MARK_BAD_SECTOR_OPERATION_NOT_SUPPORTED*/
/*#define BRCM_NAND_CONTROLLER_16_BIT_WIDTH_ISSUE_FIX*/
/*#define CONFIG_MTD_DEBUG*/
/*#define CONFIG_MTD_DEBUG_VERBOSE 7*/
/* Debug  purpose*/
/*#define CONFIG_DRIV_DEBUG*/
/*#define CONFIG_DRIV_DEBUG_VERBOSE 7*/

/* Cache Definitions */
#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_SYS_ICACHE_OFF

#define CONFIG_BOARD_LATE_INIT
/* Lets enter the kernel at EL2 for all the cores */
/*#define CONFIG_ARMV8_SWITCH_TO_EL1*/

#define CONFIG_IDENT_STRING		" Broadcom Pegasus"
#define CONFIG_BOOTP_VCI_STRING		"U-boot.armv8.bcm_pegasus"

/* Flat Device Tree Definitions */
#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_FDT

/* SMP Spin Table Definition */
#define CPU_RELEASE_ADDR			(0x84B00000)

/* Generic Timer Definitions */
#define COUNTER_FREQUENCY       (0x17D7840) /* 25MHz */

/* Generic Interrupt Controller Definitions */
//#ifdef CONFIG_UBOOT_AT_EL3
#define CONFIG_GICV2
#define GICD_BASE			(0x2A001000)
#define GICC_BASE			(0x2A002000)

/******** Physical Memory Map **************/
#define V2M_BASE			0x80000000
#define CONFIG_NR_DRAM_BANKS	2
#define PHYS_SDRAM_1			V2M_BASE	/* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE		(2 * 1024 * 1024 * 1024UL)
#define PHYS_SDRAM_2			(0x880000000UL)
#define PHYS_SDRAM_2_23MM_SIZE		(6 * 1024 * 1024 * 1024UL)
#define PHYS_SDRAM_2_17MM_SIZE		(2 * 1024 * 1024 * 1024UL)
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1

/* define text_base for U-boot image */
#define CONFIG_SYS_TEXT_BASE		0x0000000085000000
#define CONFIG_SYS_INIT_SP_ADDR     (PHYS_SDRAM_1 + 0x7fff0)
/* Size of malloc() pool */
#define CONFIG_SYS_MALLOC_LEN		 (1 * 1024 *1024)
/* Miscellaneous download areas */
#define CONFIG_SYS_LOAD_ADDR    (PHYS_SDRAM_1 +0x0080000)

/* Serial Configuration */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE (-4)
#define CONFIG_SYS_NS16550_COM1 ChipcommonG_UART0_UART_RBR_THR_DLL
#define CONFIG_SYS_NS16550_COM2 ChipcommonG_UART1_UART_RBR_THR_DLL
#define CONFIG_SYS_NS16550_COM3 ChipcommonG_UART2_UART_RBR_THR_DLL
#define CONFIG_SYS_NS16550_COM4 ChipcommonG_UART3_UART_RBR_THR_DLL

#ifdef CONFIG_PALLADIUM_EMULATION
/*Palladium baud rate*/
#define CONFIG_BAUDRATE			19200
#define CONFIG_SYS_NS16550_CLK	(307200)
#else
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_NS16550_CLK	(25 * 1000 * 1000)
#endif

#define CONFIG_CONS_INDEX               1

/* I2C */
#define CONFIG_SYS_I2C_BRCM
#define CONFIG_SYS_I2C_SPEED    0       /* Default on 100KHz */
#define CONFIG_SYS_I2C_SLAVE    0xff    /* No slave address */
#define CONFIG_I2C_MULTI_BUS    1

#define CONFIG_BCM_CMIC_MDIO

#ifdef CONFIG_BCM_CMIC_MDIO
#define CONFIG_CMD_MIIM
#endif

/*PCIE SECURITY PROGRAMMING*/
/*#define PCIE_NIC_ENABLE*/

/* Command line configuration */
//#define CONFIG_MENU
/*#define CONFIG_MENU_SHOW*/

//#define CONFIG_CMD_RUN
#if 0
#define CONFIG_CMD_PXE
#define CONFIG_CMD_ENV
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MII
#define CONFIG_CMD_PING
#define CONFIG_CMD_BOOTD
#define CONFIG_CMD_ECHO
#define CONFIG_CMD_SOURCE
#define CONFIG_CMD_NFS

/* BOOTP options */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME
#define CONFIG_BOOTP_PXE
#define CONFIG_BOOTP_PXE_CLIENTARCH	0x100
#endif

#if 0
/* UBI info */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_HOSTNAME                         northstar2
#define CONFIG_UBI_PART                         ubi0
#define CONFIG_UBIFS_VOLUME                     rootfs
#define CONFIG_LZO
#endif

//#define CONFIG_CMD_MEMORY
//#define CONFIG_CMD_MEMINFO
#define CONFIG_CMD_I2C
/*#define CONFIG_CMD_SAVEENV*/

/* Initial environment variables */
//10.31.4.86
#undef CONFIG_CMD_IMLS
/*#define CONFIG_ENV_SIZE                         0x10000*/     /* 64KB */
/*#define CONFIG_ENV_IS_NOWHERE*/
#if 1
#define CONFIG_EXTRA_ENV_SETTINGS	\
					"ethaddr=00:10:19:D0:B2:A4\0"		\
					"ipaddr=10.31.5.228\0"			\
					"netmask=255.255.255.0\0"		\
                    "console=ttyS0\0"           \
                    "loglevel=7\0"              \
                    "nfsroot=/dev/nfs rw nfsroot=/nfs/rootfs maxcpus=4\0"	\
                    "fdt_high=0xFFFFFFFFFFFFFFFF\0"   \
                    "flashfsargs=setenv bootargs loglevel=${loglevel} console=${console},${baudrate}n8 "  \
		    "ubi.mtd=nrootfs root=ubi0 rootfstype=ubifs"\
		    " maxcpus=4\0"	\
                    "nfsargs=setenv bootargs "  \
                    "loglevel=${loglevel} " \
                    "console=${console},${baudrate}n8 "   \
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:pegasus:: " \
                    "root=${nfsroot} maxcpus=4\0"	\
		"ubibootargs=setenv bootargs loglevel=${loglevel} "	\
		"console=${console},${baudrate}n8 "	\
		"ubi.mtd=nrootfs root=ubi0 "	\
		"rootfstype=ubifs rw maxcpus=4\0"\
		"ubiboot=run ubibootargs; "	\
		"nand read 0x8007ffb0 0x300000 0x01400000; "	\
		"nand read 0x90000000 0x2c0000 0x00040000; "	\
		"bootm 0x8007FFb0 - 0x90000000\0"

/* Note on mtd partition*/
/* mtd partitions can be provided through u-boot bootargs that takes precedence
 * over Linux dts specified mtd partition entries.example:
 * setenv bootargs console=ttyS0,115200n8 ubi.mtd=nrootfs root=ubi0
 * "rootfstype=ubifs rw loglevel=7 mtdparts=brcmnand.0:0x280000@0x0(nboot)ro,
 * 0x40000@0x280000(nenv),0x40000@0x2c0000(ndtb),0x3d00000@0x300000(nsystem),
 * 0x3c000000@0x4000000(nrootfs)"
 */

#ifdef CONFIG_PALLADIUM_EMULATION
//#define CONFIG_BOOTCOMMAND		"bootm 0x8007FFC0 - 0x84000000"
#define CONFIG_BOOTCOMMAND		"bootm 0x8007FFC0"
#else
#define CONFIG_BOOTCOMMAND		"sf probe;sf read 0x8007FFB0 0x200000 0xE00000;bootm 0x8007FFB0"
#endif
#endif
#define CONFIG_BOOTDELAY		-1

#define PEGASUS_23MM_BOARD  "pegasus_bcm958543k"
#define PEGASUS_17MM_BOARD  "pegasus_bcm958541k"

/* use 64MByte as max gunzip size for kernel image */
#define CONFIG_SYS_BOOTM_LEN		0x4000000

/* Monitor Command Prompt */
#define CONFIG_SYS_CBSIZE		512	/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE
#define CONFIG_SYS_LONGHELP
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_MAXARGS		64	/* max command args */

#if 1
#define CONFIG_BCM_QSPI
#define CONFIG_CMD_SF
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_STMICRO_NS
#define CONFIG_SPI_FLASH_MACRONIX_NS
#define CONFIG_SPI_FLASH_SPANSION
#define CONFIG_SPI_FLASH_SST
#define CONFIG_SPI_FLASH_WINBOND
#define CONFIG_SPI_FLASH_ATMEL

/* SPI flash configurations */
#define CONFIG_IPROC_QSPI
#define CONFIG_IPROC_QSPI_BUS                   0
#define CONFIG_IPROC_QSPI_CS                    0
#define IPROC_QSPI_MEM_BASE                     0x08000000

#define CONFIG_IPROC_BSPI_DATA_LANES            1
#define CONFIG_IPROC_BSPI_ADDR_LANES            1
#define CONFIG_IPROC_BSPI_READ_CMD              0x0b
#define CONFIG_IPROC_BSPI_READ_DUMMY_CYCLES     8
#define CONFIG_SF_DEFAULT_SPEED                 104000000
#define CONFIG_SF_DEFAULT_MODE                  SPI_MODE_3
#define CONFIG_SPI_FLASH_BAR

#define CONFIG_CMD_SF_TEST
#endif

/*SPI*/
#define CONFIG_CMD_SPI                          1
/* SPI flash configuration, 2 flashes connected to SPI2 and SPI3 */
#define CONFIG_PL022_SPI
#define CONFIG_PL022_SPI_BUS                   1
#define CONFIG_PL022_SPI_CS                    1
#define CONFIG_SYS_SPI_CLK                     100000000 /* 100MHz */

#define CONFIG_BCMIPROC_ETH 1
#define CONFIG_PHY_EGPHY28	1

#define CONFIG_GMAC_NUM	3
#ifdef PEG_A0_BRING_UP
#define CONFIG_PHY_BROADCOM 1
#endif

#define CONFIG_EXTERNAL_PHY_BUS_ID	0x3
#define CONFIG_EXTERNAL_PHY_DEV_ID	0x4

#ifdef PEG_A0_BRING_UP
#undef CONFIG_CMD_NET
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#endif

#ifdef CONFIG_NAND_IPROC_BOOT	//Untested - Boot configurations
/* NAND/QSPI */
#define CONFIG_ENV_IS_IN_NAND                   1
#define CONFIG_ENV_OFFSET                       0x200000
#define CONFIG_ENV_RANGE                        0x200000
#define CONFIG_ENV_SIZE                         0x10000     /* 64KB */
#else	/* Bootloader,ENV is in SPI Flash */
/* current SPI flash layout is 0x0 to 0x80000 is ATF, 0x80000 to 0x1E0000 is fip.bin, 0x1E0000 to 0x200000 is SPI flash ENV sector
 * and kernel is from 0x200000 till 0xc00000, DTB is 0xC00000 to 1000000 */
#define CONFIG_ENV_IS_IN_SPI_FLASH              1
#define CONFIG_ENV_OFFSET                       0x1E0000
#define CONFIG_ENV_SPI_MAX_HZ                   104000000
#define CONFIG_ENV_SPI_MODE                     SPI_MODE_3
#define CONFIG_ENV_SPI_BUS                      CONFIG_IPROC_QSPI_BUS
#define CONFIG_ENV_SPI_CS                       CONFIG_IPROC_QSPI_CS
#define CONFIG_ENV_SECT_SIZE                    0x10000     /* 64KB */
#define CONFIG_ENV_SIZE                         0x20000     /* 128KB */
#define CONFIG_ENV_OVERWRITE
#endif

/* configs for GMAC driver */
#define CONFIG_PHYLIB

/*USB*/
#ifdef PEG_A0_BRING_UP
#define CONFIG_USB_XHCI_BCM
#define CONFIG_SYS_USB_XHCI_MAX_ROOT_PORTS 3
#define CONFIG_USB_MAX_CONTROLLER_COUNT 1
#define CONFIG_CMD_USB
#define CONFIG_USB_XHCI
#define CONFIG_USB_STORAGE
#define CONFIG_CMD_FAT
#define CONFIG_DOS_PARTITION
#define CONFIG_SYS_CACHELINE_SIZE       64
#endif

#if 1
/*MMC/SD Card*/
#define CONFIG_PEGASUS 1
#define CONFIG_BCM_SDHCI
#define CONFIG_SDHCI
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_EMMC_INIT
#endif

/* SATA and company */
/* Though Pegasus has #2 controllers, AHCI driver can handle
 * only one controller at a time. So changing the SATA_CONTROLLERS_MAX to 1
 * In SATA_diags, we call ahci_init() with different controllers base for
 * different ports.
 */
#define SATA_CONTROLLERS_MAX (1)
#define CONFIG_CMD_SCSI
#define CONFIG_LIBATA
#define CONFIG_SCSI_AHCI
#define CONFIG_SCSI_AHCI_PLAT
#ifndef CONFIG_DOS_PARTITION
#define CONFIG_DOS_PARTITION
#endif /* ndef CONFIG_DOS_PARTITION */
#ifndef CONFIG_EFI_PARTITION
#define CONFIG_EFI_PARTITION
#endif /* ndef CONFIG_EFI_PARTITION */
#ifndef CONFIG_EXT2
#define CONFIG_EXT2
#endif /* ndef CONFIG_EXT2 */
#ifndef CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT2
#endif /* ndef CONFIG_CMD_EXT2 */
#ifndef CONFIG_EXT4
#define CONFIG_EXT4
#endif /* ndef CONFIG_EXT4 */
#ifndef CONFIG_CMD_EXT4
#define CONFIG_CMD_EXT4
#endif /* ndef CONFIG_CMD_EXT4 */
#ifndef CONFIG_SYS_SCSI_MAX_SCSI_ID
#define CONFIG_SYS_SCSI_MAX_SCSI_ID 1
#endif /* ndef CONFIG_SYS_SCSI_MAX_SCSI_ID */
#ifndef CONFIG_SYS_SCSI_MAX_LUN
#define CONFIG_SYS_SCSI_MAX_LUN 1
#endif /* ndef CONFIG_SYS_SCSI_MAX_LUN */
#ifndef CONFIG_SYS_SCSI_MAX_DEVICE
#define CONFIG_SYS_SCSI_MAX_DEVICE (SATA_CONTROLLERS_MAX *\
		CONFIG_SYS_SCSI_MAX_SCSI_ID * CONFIG_SYS_SCSI_MAX_LUN)
#endif /* ndef CONFIG_SYS_SCSI_MAX_DEVICE */
#ifndef CONFIG_SYS_64BIT_LBA
#define CONFIG_SYS_64BIT_LBA
#endif /* ndef CONFIG_SYS_64BIT_LBA */
#ifndef CONFIG_PARTITION_UUIDS
#define CONFIG_PARTITION_UUIDS
#endif /* ndef CONFIG_PARTITION_UUIDS */

/* OTP read support */
#define CONFIG_BCM_OTP
#define CONFIG_CMD_BCM_OTP
/* OTP check in driver-diags */
#define BCM_OTP_CHECK_ENABLED

/* RTC support */
#define CONFIG_CMD_DATE
#define CONFIG_RTC_BCM

#define CONFIG_BCM_TAMPER

#define CONFIG_BCM_SECURE_REG_OPS

#define CONFIG_CMD_BCM_PVTMON

/* I2C slaves */
/*BSC0*/
#define CONFIG_SYS_I2C_PCA9544APW_U10	0x70

/*BSC1*/
#define CONFIG_SYS_I2C_PCA9544APW_U13	0x70
#define CONFIG_SYS_I2C_PCA9555APW_U15   0x20

/*BSC2*/
#define CONFIG_SYS_I2C_PCA9544APW_U11	0x70
#define CONFIG_SYS_I2C_PCA9555APW_U34	0x20
#define I2C_SFP_ADDR			0x50

/*BSC3*/
#define CONFIG_SYS_I2C_PCA9544APW_U35	0x70
#define CONFIG_SYS_I2C_PCA9555APW_U47	0x20

/* PWM driver support*/
#define CONFIG_PWM_PEGASUS_DRIVER

/* DIAG support */
#ifdef PEG_A0_BRING_UP
#define CONFIG_DIAG_VER "Pegasus Diagnostics Version: 1.1"
#define CONFIG_HAS_POST
#define CONFIG_CMD_DIAG
#define CONFIG_NET_API

#define CONFIG_POST     (CONFIG_SYS_POST_MEMORY | CONFIG_SYS_POST_UART |\
			CONFIG_SYS_POST_QSPI | CONFIG_SYS_POST_BCM_SPI |\
			CONFIG_SYS_POST_USB | CONFIG_SYS_POST_LED |\
			CONFIG_SYS_POST_I2C | CONFIG_SYS_POST_PWM |\
			CONFIG_SYS_POST_NAND | CONFIG_SYS_POST_EMMC |\
			CONFIG_SYS_POST_PCIE | CONFIG_SYS_POST_SATA |\
			CONFIG_SYS_POST_TAMPER | CONFIG_SYS_POST_SPDIF |\
			CONFIG_SYS_POST_I2S)
#endif

/* Planned to Enable in post-silicon 2nd-phase */
#if 0
#define CONFIG_POST	CONFIG_SYS_POST_GMAC
#endif

#endif /* __BCM_PEGASUS_H */
