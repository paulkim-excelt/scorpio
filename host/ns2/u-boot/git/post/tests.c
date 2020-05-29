/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 *
 * Be sure to mark tests to be run before relocation as such with the
 * CONFIG_SYS_POST_PREREL flag so that logging is done correctly if the
 * logbuffer support is enabled.
 */

#include <common.h>

#include <post.h>

#ifdef __PPC__
extern int ocm_post_test (int flags);
extern int cache_post_test (int flags);
extern int watchdog_post_test (int flags);
extern int i2c_post_test (int flags);
extern int rtc_post_test (int flags);
extern int memory_post_test (int flags);
extern int cpu_post_test (int flags);
extern int fpu_post_test (int flags);
extern int uart_post_test (int flags);
extern int ether_post_test (int flags);
extern int spi_post_test (int flags);
extern int usb_post_test (int flags);
extern int spr_post_test (int flags);
extern int sysmon_post_test (int flags);
extern int dsp_post_test (int flags);
extern int codec_post_test (int flags);
extern int ecc_post_test (int flags);
extern int flash_post_test(int flags);

extern int dspic_init_post_test (int flags);
extern int dspic_post_test (int flags);
extern int gdc_post_test (int flags);
extern int fpga_post_test (int flags);
extern int lwmon5_watchdog_post_test(int flags);
extern int sysmon1_post_test(int flags);
extern int coprocessor_post_test(int flags);
extern int led_post_test(int flags);
extern int button_post_test(int flags);
extern int memory_regions_post_test(int flags);
#else
/****** Broadcom Diagnostics *********/
extern int memory_post_test(int flags);
extern int UART_post_test(int flags);
extern int qspi_post_test(int flags);
extern int SPI_post_test(int flags);
extern int MMC_post_test(int flags);
extern int NAND_post_test(int flags);
extern int GMAC_post_test(int flags);
extern int PCIE_post_test(int flags);
extern int PWM_post_test(int flags);
extern int spdif_post_test(int flags);
extern int AUDIO_post_test(int flags);
extern int AON_GPIO_post_test(int flags);
extern int I2C_post_test(int flags);
extern int USB_post_test(int flags);
extern int TAMPER_post_test(int flags);
extern int SATA_post_test(int flags);
extern int USB20_post_test(int flags);
extern int USB30_post_test(int flags);
extern int MDIO_post_test(int flags);

#endif


extern int sysmon_init_f (void);

extern void sysmon_reloc (void);


struct post_test post_list[] =
{
#ifdef __PPC__
#if CONFIG_POST & CONFIG_SYS_POST_OCM
    {
	"OCM test",
	"ocm",
	"This test checks on chip memory (OCM).",
	POST_ROM | POST_ALWAYS | POST_PREREL | POST_CRITICAL | POST_STOP,
	&ocm_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_OCM
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CACHE
    {
	"Cache test",
	"cache",
	"This test verifies the CPU cache operation.",
	POST_RAM | POST_ALWAYS,
	&cache_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CACHE
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_WATCHDOG
#if defined(CONFIG_POST_WATCHDOG)
	CONFIG_POST_WATCHDOG,
#else
    {
	"Watchdog timer test",
	"watchdog",
	"This test checks the watchdog timer.",
	POST_RAM | POST_POWERON | POST_SLOWTEST | POST_MANUAL | POST_REBOOT,
	&watchdog_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_WATCHDOG
    },
#endif
#endif
#if CONFIG_POST & CONFIG_SYS_POST_I2C
    {
	"I2C test",
	"i2c",
	"This test verifies the I2C operation.",
	POST_RAM | POST_ALWAYS,
	&i2c_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_I2C
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_RTC
    {
	"RTC test",
	"rtc",
	"This test verifies the RTC operation.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&rtc_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_RTC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
    {
	"Memory test",
	"memory",
	"This test checks RAM.",
	POST_ROM | POST_POWERON | POST_SLOWTEST | POST_PREREL,
	&memory_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEMORY
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CPU
    {
	"CPU test",
	"cpu",
	"This test verifies the arithmetic logic unit of"
	" CPU.",
	POST_RAM | POST_ALWAYS,
	&cpu_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CPU
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_FPU
    {
	"FPU test",
	"fpu",
	"This test verifies the arithmetic logic unit of"
	" FPU.",
	POST_RAM | POST_ALWAYS,
	&fpu_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_FPU
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_UART
#if defined(CONFIG_POST_UART)
	CONFIG_POST_UART,
#else
    {
	"UART test",
	"uart",
	"This test verifies the UART operation.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&uart_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_UART
    },
#endif /* CONFIG_POST_UART */
#endif
#if CONFIG_POST & CONFIG_SYS_POST_ETHER
    {
	"ETHERNET test",
	"ethernet",
	"This test verifies the ETHERNET operation.",
	POST_RAM | POST_ALWAYS,
	&ether_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_ETHER
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPI
    {
	"SPI test",
	"spi",
	"This test verifies the SPI operation.",
	POST_RAM | POST_ALWAYS,
	&spi_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPI
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_USB
    {
	"USB test",
	"usb",
	"This test verifies the USB operation.",
	POST_RAM | POST_ALWAYS,
	&usb_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_USB
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SPR
    {
	"SPR test",
	"spr",
	"This test checks SPR contents.",
	POST_RAM | POST_ALWAYS,
	&spr_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_SPR
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_SYSMON
    {
	"SYSMON test",
	"sysmon",
	"This test monitors system hardware.",
	POST_RAM | POST_ALWAYS,
	&sysmon_post_test,
	&sysmon_init_f,
	&sysmon_reloc,
	CONFIG_SYS_POST_SYSMON
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_DSP
    {
	"DSP test",
	"dsp",
	"This test checks any connected DSP(s).",
	POST_RAM | POST_ALWAYS,
	&dsp_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_DSP
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_CODEC
    {
	"CODEC test",
	"codec",
	"This test checks any connected codec(s).",
	POST_RAM | POST_MANUAL,
	&codec_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_CODEC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_ECC
    {
	"ECC test",
	"ecc",
	"This test checks the ECC facility of memory.",
	POST_ROM | POST_ALWAYS | POST_PREREL,
	&ecc_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_ECC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC1
	CONFIG_POST_BSPEC1,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC2
	CONFIG_POST_BSPEC2,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC3
	CONFIG_POST_BSPEC3,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC4
	CONFIG_POST_BSPEC4,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_BSPEC5
	CONFIG_POST_BSPEC5,
#endif
#if CONFIG_POST & CONFIG_SYS_POST_COPROC
    {
	"Coprocessors communication test",
	"coproc_com",
	"This test checks communication with coprocessors.",
	POST_RAM | POST_ALWAYS | POST_CRITICAL,
	&coprocessor_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_COPROC
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_FLASH
    {
	"Parallel NOR flash test",
	"flash",
	"This test verifies parallel flash operations.",
	POST_RAM | POST_SLOWTEST | POST_MANUAL,
	&flash_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_FLASH
    },
#endif
#if CONFIG_POST & CONFIG_SYS_POST_MEM_REGIONS
    {
	"Memory regions test",
	"mem_regions",
	"This test checks regularly placed regions of the RAM.",
	POST_ROM | POST_SLOWTEST | POST_PREREL,
	&memory_regions_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEM_REGIONS
    },
#endif

#else /****** BROADCOM DIAGS *******/

#ifdef CONFIG_TARGET_BROADCOM_PEGASUS

#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
    {
	"Memory test",
	"memory",
	"This test checks RAM.",
	POST_RAM | POST_MANUAL | POST_AUTO,
	&memory_post_test,
	NULL,
	NULL,
	CONFIG_SYS_POST_MEMORY
    },
#endif

#if CONFIG_POST & CONFIG_SYS_POST_UART
    {
	    "UART test",
	    "uart",
	    "This test verifies the UART operation",
	    POST_RAM | POST_MANUAL | POST_AUTO,
	    &UART_post_test,
	    NULL,
	    NULL,
	    CONFIG_SYS_POST_UART
    },
#endif

#if CONFIG_POST & CONFIG_SYS_POST_QSPI
    {
	    "QSPI test",
	    "qspi",
	    "This test verifies the qspi operation.",
	    POST_RAM | POST_MANUAL | POST_AUTO,
	    &qspi_post_test,
	    NULL,
	    NULL,
	    CONFIG_SYS_POST_QSPI
    },
#endif

#if CONFIG_POST & CONFIG_SYS_POST_BCM_SPI
	{
	   "SPI test",
	   "spi",
	   "This test verifies the connectivity of SPI0/1 with EEPROM",
	   POST_RAM | POST_MANUAL | POST_AUTO,
	   &SPI_post_test,
	   NULL,
	   NULL,
	   CONFIG_SYS_POST_BCM_SPI
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_EMMC
	{
		"eMMC test",
		"emmc",
		"This test verifies eMMC device",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&MMC_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_EMMC
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_GMAC
	{
		"GMAC test",
		"gmac",
		"This test verifies the GMAC interface: EGPHY28",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&GMAC_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_GMAC
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_NAND
	{
		"NAND test",
		"nand",
		"This test verifies the NAND operation",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&NAND_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_NAND
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_PCIE
	{
		"PCIE PRBS loopback test",
		"pcie",
		"This test verifies the PCIE link via external loopback using PRBS test",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&PCIE_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_PCIE
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_PWM
	{
		"PWM test",
		"pwm",
		"This test verifies PWM channels 0,1,2 and 3",
		POST_RAM | POST_MANUAL,
		&PWM_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_PWM
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_SPDIF
	{
		"SPDIF test",
		"spdif",
		"This test verifies the SPDIF operation.",
		POST_RAM | POST_MANUAL,
		&spdif_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_SPDIF
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_I2S
	{
		"AUDIO test",
		"audio",
		"This test verifies the I2S interface with the BCM9I2SDDC daughter card.",
		POST_RAM | POST_MANUAL,
		&AUDIO_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_I2S
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_LED
	{
		"AON GPIO Led test",
		"led",
		"This test verifies LEDs connected to AON GPIO",
		POST_RAM | POST_MANUAL,
		&AON_GPIO_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_LED
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_I2C
	{
		"I2C test",
		"i2c",
		"This test verifies the connectivity of I2C interface",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&I2C_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_I2C
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_USB
	{
		"USB test",
		"usb",
		"This test verifies the connectivity of USB 2.0/3.0 links and functions",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&USB_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_USB
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_TAMPER
	{
		"TAMPER test",
		"tamper",
		"This test verifies tamper emesh features",
		POST_RAM | POST_MANUAL,
		&TAMPER_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_TAMPER
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_SATA
	{
		"SATA 0/1 test",
		"sata",
		"This test verifies the SATA disk connected to ports 0 and 1",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&SATA_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_SATA
	},
#endif
#endif /*ifdef CONFIG_TARGET_BROADCOM_PEGASUS*/

#ifdef CONFIG_TARGET_NS2_SVK

#if CONFIG_POST & CONFIG_SYS_POST_MEMORY
	{
		"Memory test",
		"memory",
		"This test checks RAM.",
		POST_RAM | POST_MANUAL,
		&memory_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_MEMORY
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_UART
	{
		"UART test",
		"uart",
		"This test verifies the UART operation",
		POST_RAM | POST_MANUAL,
		&UART_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_UART
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_QSPI
	{
		"QSPI test",
		"qspi",
		"This test verifies the qspi operation.",
		POST_RAM | POST_MANUAL,
		&qspi_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_QSPI
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_BCM_SPI
	{
		"SPI test",
		"spi",
		"This test verifies the connectivity of SPI0/1 with EEPROM",
		POST_RAM | POST_MANUAL,
		&SPI_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_BCM_SPI
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_EMMC
	{
		"eMMC test",
		"emmc",
		"This test verifies eMMC device",
		POST_RAM | POST_MANUAL,
		&MMC_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_EMMC
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_GMAC
	{
		"GMAC test",
		"gmac",
		"This test verifies the GMAC interface: EGPHY28",
		POST_RAM | POST_MANUAL,
		&GMAC_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_GMAC
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_NAND
	{
		"NAND test",
		"nand",
		"This test verifies the NAND operation",
		POST_RAM | POST_MANUAL,
		&NAND_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_NAND
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_PCIE
	{
		"PCIE PRBS loopback test",
		"pcie",
		"This test verifies the PCIE link via external loopback using PRBS test",
		POST_RAM | POST_MANUAL,
		&PCIE_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_PCIE
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_PWM
	{
		"PWM test",
		"pwm",
		"This test verifies PWM channels 0,1,2 and 3",
		POST_RAM | POST_MANUAL,
		&PWM_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_PWM
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_SPDIF
	{
		"SPDIF test",
		"spdif",
		"This test verifies the SPDIF operation.",
		POST_RAM | POST_MANUAL,
		&spdif_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_SPDIF
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_I2S
	{
		"AUDIO test",
		"audio",
		"This test verifies the I2S interface with the BCM9I2SDDC daughter card.",
		POST_RAM | POST_MANUAL,
		&AUDIO_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_I2S
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_LED
	{
		"AON GPIO Led test",
		"led",
		"This test verifies LEDs connected to AON GPIO",
		POST_RAM | POST_MANUAL,
		&AON_GPIO_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_LED
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_I2C
	{
		"I2C test",
		"i2c",
		"This test verifies the connectivity of I2C interface",
		POST_RAM | POST_MANUAL,
		&I2C_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_I2C
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_USB
	{
		"USB test",
		"usb",
		"This test verifies the connectivity of USB 2.0/3.0 links and functions",
		POST_RAM | POST_MANUAL,
		&USB_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_USB
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_TAMPER
	{
		"TAMPER test",
		"tamper",
		"This test verifies tamper emesh features",
		POST_RAM | POST_MANUAL,
		&TAMPER_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_TAMPER
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_SATA
	{
		"SATA 0/1 test",
		"sata",
		"This test verifies the SATA disk connected to ports 0 and 1",
		POST_RAM | POST_MANUAL,
		&SATA_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_SATA
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_USB20
	{
		"USB 2.0 test",
		"usb2",
		"This test verifies the connectivity of the USB 2.0 link & functions",
		POST_RAM | POST_MANUAL | POST_AUTO,
		&USB20_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_USB20
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_USB30
	{
		"USB 3.0 test",
		"usb3",
		"This test verifies the USB 3.0 link & operation on Port_0 and Port_1.",
		POST_RAM | POST_MANUAL,
		&USB30_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_USB30
	},
#endif

#if CONFIG_POST & CONFIG_SYS_POST_MDIO
	{
		"MDIO test",
		"mdio",
		"This test verifies the functionality of mdio access to available phy's",
		POST_RAM | POST_MANUAL,
		&MDIO_post_test,
		NULL,
		NULL,
		CONFIG_SYS_POST_MDIO
	},
#endif

#endif /*ifdef CONFIG_TARGET_NS2_SVK*/
#endif /*ifdef __PPC__*/
};

unsigned int post_list_size = ARRAY_SIZE(post_list);
