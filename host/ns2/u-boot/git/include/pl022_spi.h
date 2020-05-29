/*
 *  * spi_pl022.h
 *
 *   * $Copyright Open Broadcom Corporation$
 *    */

#include <spi.h>

#ifndef SPI_PL022_H_
#define SPI_PL022_H_

struct spi_slave *pl022_spi_setup_slave(unsigned int bus,
					unsigned int cs,
					unsigned int max_hz, unsigned int mode);
int pl022_spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		   const void *dout, void *din, unsigned long flags);
int pl022_spi_claim_bus(struct spi_slave *slave);
void pl022_spi_release_bus(struct spi_slave *slave);
void pl022_spi_free_slave(struct spi_slave *slave);
int pl022_spi_loopback_enable(struct spi_slave *slave);

#endif /* SPI_PL022_H_ */
