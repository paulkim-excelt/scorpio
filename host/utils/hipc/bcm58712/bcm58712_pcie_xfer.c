
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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <bcm_err.h>
#include <hlog.h>
#include <xgbe-ioctl.h>
#include "bus_xfer.h"
#include "pcie_xfer.h"


#define BCM_XGBE_DEV        ("/dev/net/bcm")

static int32_t HIPC_PcieFd;

int32_t PCIE_XferRead(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr, uint8_t *data, uint32_t width)
{
    int32_t ret = BCM_ERR_OK;
    int err;
    /* Note that lenght parameters is not populated in the iocall,
     * since hipc layer does not provide it as of now
     */
    struct xgbe_ioctlcmd cmd = {
        .addr = addr,
        .width = width,
        .data = data,
    };

    if (HIPC_PcieFd > 0) {
        err = ioctl(HIPC_PcieFd, XGBE_IOCTL_RDMEM, &cmd);
        if (err) {
            ret = BCM_ERR_UNKNOWN;
        }
    } else {
        ret = BCM_ERR_NODEV;
    }
    return ret;
}

int32_t PCIE_XferWrite(uint32_t slaveId, HIPC_BusAccessType access, uint32_t addr, uint8_t *data, uint32_t width)

{
    int32_t ret = BCM_ERR_OK;
    int err;

    /* Note that lenght parameters is not populated in the iocall,
     * since hipc layer does not provide it as of now
     */
    struct xgbe_ioctlcmd cmd = {
        .addr = addr,
        .width = width,
        .data = data,
    };

    if (HIPC_PcieFd > 0) {
        err = ioctl(HIPC_PcieFd, XGBE_IOCTL_WRMEM, &cmd);
        if (err) {
            ret = BCM_ERR_UNKNOWN;
        }
    } else {
        ret = BCM_ERR_NODEV;
    }
    return ret;
}

int32_t PCIE_Init(uint32_t busID, uint32_t speed)
{
    int32_t ret = BCM_ERR_OK;
    HIPC_PcieFd = open(BCM_XGBE_DEV, O_RDWR);
    if (HIPC_PcieFd < 0) {
        perror("open");
        ret = BCM_ERR_NODEV;
    }
    return ret;
}

int32_t PCIE_DeInit(uint32_t busID)
{
    if (HIPC_PcieFd > 0) {
        close(HIPC_PcieFd);
    }
    return BCM_ERR_OK;
}
