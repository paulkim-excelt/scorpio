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


#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <stdint.h>
#include <utils.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <bcm_err.h>
#include <sys/mman.h>
#include <stdio.h>
#include <gpio.h>
#include <hlog.h>
#include <sys/ioctl.h>
#include <linux/gpio.h>
/* include ns2 kernel export include directory in Makefile */

#define BCM58712_GPIO_NUM_MAX   (31UL)

#define GPIO_CHRDEV_NAME     "/dev/gpiochip0"
#define GPIO_CONSUMER_NAME      "bcmutild"

static int GPIO_Fd;
static uint32_t LineOffsetsOutput[BCM58712_GPIO_NUM_MAX];
static struct gpiohandle_data LineValuesOutput;

int GPIO_SetValues(struct gpiohandle_data *data)
{
    int retVal;

    retVal = ioctl(GPIO_Fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, data);
    if (retVal == -1) {
        HOST_Log("%s IOCTL failed err:0x%x\n", __func__, retVal);
        retVal = BCM_ERR_UNKNOWN;
    }  else {
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

/* GET is not tested */
int GPIO_GetValues(struct gpiohandle_data *data)
{
    int retVal;

    retVal = ioctl(GPIO_Fd, GPIOHANDLE_GET_LINE_VALUES_IOCTL, data);
    if (retVal == -1) {
        HOST_Log("%s IOCTL failed err:0x%x\n", __func__, retVal);
        retVal = BCM_ERR_UNKNOWN;
    }  else {
        retVal = BCM_ERR_OK;
    }

    return retVal;
}

int32_t GPIO_Get(uint32_t pin, GPIO_LevelType *level)
{
    int32_t retVal;
    struct gpiohandle_data data;
    int i;

    for (i=0; i < BCM58712_GPIO_NUM_MAX; i++) {
        if (LineOffsetsOutput[i] == pin) {
            break;
        }
    }
    
    if (i == BCM58712_GPIO_NUM_MAX) {
        HOST_Log("%s pin:%u is not configured\n", __func__, pin);
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    }

    retVal = GPIO_GetValues(&data);
    if (BCM_ERR_OK == retVal) {
        if (data.values[i] == 0) {
            *level = GPIO_LEVEL_LOW;
        } else {
            *level = GPIO_LEVEL_HIGH;
        }
    } else {
        HOST_Log("%s Failed to get values\n", __func__);
    }
    
done:
    return retVal;
}

int32_t GPIO_Set(uint32_t pin, GPIO_LevelType level)
{
    int32_t retVal;
    int i;

    for (i=0; i < BCM58712_GPIO_NUM_MAX; i++) {
        if (LineOffsetsOutput[i] == pin) {
            break;
        }
    }
    
    if (i == BCM58712_GPIO_NUM_MAX) {
        HOST_Log("%s pin:%u is not configured\n", __func__, pin);
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    }

    if (level == GPIO_LEVEL_LOW) {
        LineValuesOutput.values[i] = 0;
    } else {
        LineValuesOutput.values[i] = 1;
    }

    retVal = GPIO_SetValues(&LineValuesOutput);
    if (retVal != BCM_ERR_OK) {
        HOST_Log("%s Failed to set GPIO %u to level %u", __func__, pin, level);
    }

done:
    return retVal;
}

static int32_t GPIO_ConfigureOutput(uint32_t *lineoffsets, uint32_t lines)
{
    struct gpiohandle_request req;
    int32_t retVal;

    req.flags = GPIOHANDLE_REQUEST_OUTPUT;
    strcpy(req.consumer_label, GPIO_CONSUMER_NAME);
    memcpy(&req.lineoffsets[0], lineoffsets, sizeof(uint32_t)*lines);
    req.lines = lines;
    memcpy(&req.default_values[0], &LineValuesOutput.values[0],
        sizeof(LineValuesOutput.values[0])*lines);

    retVal = ioctl(GPIO_Fd, GPIO_GET_LINEHANDLE_IOCTL, &req);
    if (retVal == -1) {
        HOST_Log("%s IOCTL failed err:%d\n", retVal);
        retVal = BCM_ERR_UNKNOWN;
    }

    /* close existing file handle and use the line handle returned by IOCTL */
    if (close(GPIO_Fd) == -1) {
        HOST_Log("Failed to close GPIO character device file");
    } else {
        GPIO_Fd = req.fd;
    }

    return retVal;
}

int32_t GPIO_Init()
{
    int32_t retVal;

    GPIO_Fd = open(GPIO_CHRDEV_NAME, 0);
    if (GPIO_Fd == -1) {
        HOST_Log("Failed to open %s\n", GPIO_CHRDEV_NAME);
        if (close(GPIO_Fd) == -1) {
            HOST_Log("Failed to close GPIO character device file");
        }
        retVal = BCM_ERR_UNKNOWN;
        goto done;
    }

    /* Ensure that the list is invalid when we start */
    memset(&LineOffsetsOutput[0], 0xff, sizeof(LineOffsetsOutput));

    /* Drive low by default */
    memset(&LineValuesOutput.values[0], 0, sizeof(LineValuesOutput));

    /* List all lines to be configured */
    LineOffsetsOutput[0] = GPIO_SPI_SS;

    retVal = GPIO_ConfigureOutput(&LineOffsetsOutput[0], 1UL);
    if (BCM_ERR_OK != retVal) {
        HOST_Log("%s :: Could not configure GPIO pin %lu as out\n",
                __FUNCTION__, GPIO_SPI_SS);
    }

done:
    return retVal;
}

void GPIO_Deinit()
{
    if (GPIO_Fd != -1) {
        if (close(GPIO_Fd) == -1) {
            HOST_Log("Failed to close GPIO character device file");
        }
        GPIO_Fd = -1;
    }
}

