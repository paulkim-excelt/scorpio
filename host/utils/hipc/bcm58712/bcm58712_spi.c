/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <bcm_err.h>
#include <gpio.h>
#include <stdlib.h>
#include <hlog.h>

int SPI_Fd;

int32_t SPI_Write(int32_t dev_id, uint8_t *buf, int32_t len)
{
	struct spi_ioc_transfer	xfer[1];
    int32_t retVal = BCM_ERR_NOSUPPORT;

	memset(xfer, 0, sizeof xfer);

	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = len;

    GPIO_Set(GPIO_SPI_SS, GPIO_LEVEL_LOW);

	retVal = ioctl(SPI_Fd, SPI_IOC_MESSAGE(1), xfer);
	if (retVal < 0) {
		perror("SPI_IOC_MESSAGE");
	} else if (retVal != len) {
        HOST_Log("%s completed %d/%d\n", __func__, retVal, len);
    } else {
        retVal = BCM_ERR_OK;
    }

    GPIO_Set(GPIO_SPI_SS, GPIO_LEVEL_HIGH);
    return retVal;
}

int32_t SPI_Read(int32_t dev_id, uint8_t *wr_buf, int32_t wr_len, uint8_t *rd_buf, int32_t rd_len)
{
	struct spi_ioc_transfer	xfer[2];
    int32_t retVal = BCM_ERR_NOSUPPORT;

	memset(xfer, 0, sizeof xfer);

	xfer[0].tx_buf = (unsigned long)wr_buf;
	xfer[0].len = wr_len;

	xfer[1].rx_buf = (unsigned long)rd_buf;
	xfer[1].len = rd_len;

    GPIO_Set(GPIO_SPI_SS, GPIO_LEVEL_LOW);

	retVal = ioctl(SPI_Fd, SPI_IOC_MESSAGE(2), xfer);
	if (retVal < 0) {
		perror("SPI_IOC_MESSAGE");
	} else if (retVal != (wr_len + rd_len)) {
        HOST_Log("%s completed %d/%d\n", __func__, retVal, wr_len + rd_len);
    } else {
        retVal = BCM_ERR_OK;
	}

    GPIO_Set(GPIO_SPI_SS, GPIO_LEVEL_HIGH);
    return retVal;
}

void SPI_Deinit()
{
    close(SPI_Fd);
    SPI_Fd = -1;
}

int32_t SPI_Init(uint32_t dev_id, uint32_t speed_hz)
{
    char name[] = "/dev/spidev1.0";
    int32_t retVal;

    SPI_Fd = open(name, O_RDWR);
    if (SPI_Fd < 0) {
        perror("open");
        retVal = BCM_ERR_NODEV;
        goto done;
    }

    retVal = GPIO_Set(GPIO_SPI_SS, GPIO_LEVEL_HIGH);
    if (BCM_ERR_OK != retVal) {
        goto done;
    }

done:
    return retVal;
}
