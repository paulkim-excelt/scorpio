/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
#include <spi_osil.h>
#include "ee.h"


//! [Usage of SPI_IRQHandler]
ISR2(SPI0_IRQHandler)
{
    StatusType status = E_OK;

    SPI_IRQHandler(0U);
    status = SetEvent(SPI_IRQ_PROCESS_TASK, SPI0_IRQ_EVENT);
    if (E_OK != status) {
    }
}
//! [Usage of SPI_IRQHandler]

ISR2(SPI1_IRQHandler)
{
    StatusType status = E_OK;

    SPI_IRQHandler(1U);
    status = SetEvent(SPI_IRQ_PROCESS_TASK, SPI1_IRQ_EVENT);
    if (E_OK != status) {
    }
}

ISR2(SPI2_IRQHandler)
{
    StatusType status = E_OK;

    SPI_IRQHandler(2U);
    status = SetEvent(SPI_IRQ_PROCESS_TASK, SPI2_IRQ_EVENT);
    if (E_OK != status) {
    }
}

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    Build SVC command structure #SVC_RequestType
    sysReqIO.sysReqID = BCM_SPI_SVC_ID
    sysReqIO.magicID = SVC_MAGIC_SPI_ID
    sysReqIO.cmd = aCmd
    sysReqIO.sysIO.adcIO = aIO
    sysReqIO.response = BCM_ERR_UNKNOWN

    Send SVC command
    ret = SVC_Request(&sysReqIO)
    if ret is not BCM_ERR_OK
        ret = BCM_ERR_UNKNOWN
    else if sysReqIO.response is not BCM_ERR_OK
        ret = sysReqIO.response
    else
        ret = aIO->retVal
    return ret
    @endcode
*/
int32_t SPI_SysCmdReq(SPI_CmdType aCmd,
                      SPIIO *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_SPI_ID;
        sysReqIO.magicID = SVC_MAGIC_SPI_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == sysReqIO.response) {
                retVal = aIO->retVal;
            } else {
                retVal = sysReqIO.response;
            }
        }
    }

    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _SPI_SVCIOType {
    uint8_t *data;
    SPIIO *io;
} SPI_SVCIOType;

/**
    @code{.c}
    if aSysIO.spiIO is not NULL
        if aMagicID is SVC_MAGIC_SPI_ID
            aSysIO.spiIO.retVal = SPI_CmdHandler(aCmd, aSysIO.spiIO)
        else
            aSysIO.spiIO.retVal = BCM_ERR_INVAL_MAGIC
    @endcode
*/
//! [Usage of SPI_CmdHandler]
void SPI_SysCmdHandler(uint32_t aMagicID,
                       uint32_t aCmd,
                       uint8_t * aSysIO)
{
    SPI_SVCIOType spi;
    spi.data = aSysIO;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_SPI_ID) {
            spi.io->retVal = SPI_CmdHandler(aCmd, spi.io);
        } else {
            spi.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}
//! [Usage of SPI_CmdHandler]
