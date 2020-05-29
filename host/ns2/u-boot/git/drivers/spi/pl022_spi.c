/*
 * $Copyright Open Broadcom Corporation$
 */

#include <common.h>
#include <malloc.h>
#include <spi.h>
#include <asm/io.h>
#include <asm/arch/socregs.h>
/*#include <asm/arch/configs.h>*/

#define EINVAL          22
/* base address must be address of SPI0, spi: 0-5 */
#define SPI_ADDR(base_addr, spi) ((const void *)((unsigned long)\
			(base_addr + ((spi) * (CHIPCOMMONG_SPI1_SSPCR0 -\
					       CHIPCOMMONG_SPI0_SSPCR0)))))

void input_param_check(struct spi_slave *slave)
{
	if (!slave) {
		printf("%s %d Invalid param slave:%p\n",
		       __func__, __LINE__, slave);
		return;
	}
	if ((slave->bus > CONFIG_PL022_SPI_BUS) ||
	    (slave->cs != CONFIG_PL022_SPI_CS)) {
		printf("%s %d Invalid param bus:%d cs %d\n",
		       __func__, __LINE__, slave->bus, slave->cs);
		return;
	}
}

/* setup spi io multiplex */
/*extern void iproc_spi_iomux(int spi, int op);*/

#define SCR_MAX		0xFF
#define CPSR_MAX	0xFE

void pl022_spi_init(void)
{
}

struct spi_slave *pl022_spi_setup_slave(unsigned int bus, unsigned int cs,
					unsigned int max_hz, unsigned int mode)
{
	struct spi_slave *slave;
	u16 scr = 1, prescaler, cr0 = 0, cpsr = 0;
	u16 reg_val = 0;

	if (cs != CONFIG_PL022_SPI_CS || bus > CONFIG_PL022_SPI_BUS ||
	    mode > SPI_MODE_3) {
		printf("%s %d: unsupported bus %d cs:%d mode:%d\n", __func__,
		       __LINE__, bus, cs, mode);
		return NULL;
	}

	slave = malloc(sizeof(struct spi_slave));
	if (!slave)
		return NULL;
	slave->bus = bus;
	slave->cs = cs;

	/* PID should be 0x00041022 */
	if ((readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPPERIPHID0, bus)) == 0x22) &&
	    (readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPPERIPHID1, bus))
	     == 0x10) &&
	    ((readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPPERIPHID2, bus)) &
	      0xf) == 0x04) &&
	    (readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPPERIPHID3, bus))
	     == 0x00)) {
		/*pr_info("Function %s: Valid Bus %d Line %d\n",
		 * __func__, bus, __LINE__);
		 */
	} else {
		free(slave);
		printf("After free slave");
		/*pr_info("Function %s: Line %d\n", __func__, __LINE__); */
		return NULL;
	}

	/* Disable SSP */
	writel(0x0, SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, bus));

	/* Set requested polarity and 8bit mode */
	cr0 = 7;		/* 8 bits; */
	cr0 |= (mode & SPI_CPHA) ? (1 << CHIPCOMMONG_SPI0_SSPCR0__SPH) : 0;
	cr0 |= (mode & SPI_CPOL) ? (1 << CHIPCOMMONG_SPI0_SSPCR0__SPO) : 0;

	writel(cr0, SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR0, bus));
	/* Program the SSPClk frequency */
	prescaler = CONFIG_SYS_SPI_CLK / max_hz;

	if (prescaler <= 0xFF) {
		cpsr = prescaler;
	} else {
		for (scr = 1; scr <= SCR_MAX; scr++) {
			if (!(prescaler % scr)) {
				cpsr = prescaler / scr;
				if (cpsr <= CPSR_MAX)
					break;
			}
		}
		if (scr > SCR_MAX) {
			scr = SCR_MAX;
			cpsr = prescaler / scr;
			cpsr &= CPSR_MAX;
		}
	}

	if (cpsr & 0x1)
		cpsr++;

	writel(cpsr, SPI_ADDR(CHIPCOMMONG_SPI0_SSPCPSR, bus));
	cr0 = readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR0, bus));
	writel(cr0 | (scr - 1) << CHIPCOMMONG_SPI0_SSPCR0__SCR_R,
	       SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR0, bus));

	/* Enable ssp */
	reg_val = 1 << CHIPCOMMONG_SPI0_SSPCR1__SSE;
	writel(reg_val, SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, bus));

	return slave;
}

/*EXPORT_SYMBOL(pl022_spi_setup_slave);*/

int pl022_spi_loopback_enable(struct spi_slave *slave)
{
	u16 reg_val = 0;
	u8 value;

	input_param_check(slave);
	reg_val = readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));
	/* Enable looop back */
	writel((reg_val | (1 << CHIPCOMMONG_SPI0_SSPCR1__LBM)),
	       SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));
	value = readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));

	return 0;
}

/*EXPORT_SYMBOL(pl022_spi_loopback_enable);*/

void pl022_spi_free_slave(struct spi_slave *slave)
{
	free(slave);
}

/*EXPORT_SYMBOL(pl022_spi_free_slave);*/

int pl022_spi_claim_bus(struct spi_slave *slave)
{
	u8 value;

	input_param_check(slave);
	/* Enable the SPI hardware */
	writel(1 << CHIPCOMMONG_SPI0_SSPCR1__SSE,
	       SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));
	value = readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));

	return 0;
}

/*EXPORT_SYMBOL(pl022_spi_claim_bus);*/

int pl022_spi_release_bus(struct spi_slave *slave)
{
	input_param_check(slave);
	/* Disable SPI */
	writel(0x0, SPI_ADDR(CHIPCOMMONG_SPI0_SSPCR1, slave->bus));
	return 0;
}

/*EXPORT_SYMBOL(pl022_spi_release_bus);*/

int pl022_spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		   const void *dout, void *din, unsigned long flags)
{
	u32 len_tx = 0, len_rx = 0, len;
	u32 ret = 0;
	const u8 *txp = dout;
	u8 *rxp = din, value;

	input_param_check(slave);
	if (bitlen == 0)
		return ret;

	if (bitlen % 8) {
		ret = -1;
		return ret;
	}

	len = bitlen / 8;

	while (len_tx < len) {
		if (readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPSR, slave->bus)) &
		    (1 << CHIPCOMMONG_SPI0_SSPSR__TNF)) {
			/* transmit io not full */
			/*value = (txp != NULL) ? *txp++ : 0; */
			if (txp)
				value = *txp++;
			else
				value = 0;
			/*printf("Transmit Value:%d", txp); */
			writel(value,
			       SPI_ADDR(CHIPCOMMONG_SPI0_SSPDR, slave->bus));
			len_tx++;
		}

		if (readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPSR, slave->bus)) &
		    (1 << CHIPCOMMONG_SPI0_SSPSR__RNE)) {
			/* receive fifo not empty */
			value =
			    readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPDR, slave->bus));
			if (rxp) {
				*rxp++ = value;
			}
			len_rx++;
		}
	}

	while (len_rx < len_tx) {
		if (readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPSR, slave->bus)) &
		    (1 << CHIPCOMMONG_SPI0_SSPSR__RNE)) {
			/* receive fifo not empty */
			value =
			    readl(SPI_ADDR(CHIPCOMMONG_SPI0_SSPDR, slave->bus));
			if (rxp)
				*rxp++ = value;
			len_rx++;
		}
	}
	return ret;
}

/*EXPORT_SYMBOL(pl022_spi_xfer);
 * EXPORT_SYMBOL(pl022_spi_release_bus);
 * EXPORT_SYMBOL(pl022_spi_claim_bus);
 * EXPORT_SYMBOL(pl022_spi_free_slave);
 * EXPORT_SYMBOL(pl022_spi_setup_slave);
 * EXPORT_SYMBOL(pl022_spi_loopback_enable);
 */
