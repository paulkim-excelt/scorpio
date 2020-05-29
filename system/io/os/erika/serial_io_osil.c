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

#include <msg_queue.h>
#include <bcm_err.h>
#if defined(ENABLE_VTMON)
#include <osil/vtmon_osil.h>
#endif
#if defined(ENABLE_SPI_PL022)
#include <osil/spi_osil.h>
#endif
#include <serial_io_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"

//! [Usage of SPI_IRQEventHandler]
//! [Usage of VTMON_StateHandler]
TASK(TaskSerialIO)
{
    int32_t status = BCM_ERR_OK;
    BCM_EventMaskType mask;

    while (BCM_ERR_OK == status) {
        status = BCM_WaitEvent(SERIAL_IO_EVENTS);
        if (BCM_ERR_OK != status) {
            break;
        }
        status = BCM_GetEvent(TaskSerialIO, &mask);
        if (BCM_ERR_OK != status) {
            break;
        }

#if defined(ENABLE_SPI_PL022)
        if ((mask & SPI0_IRQ_EVENT) == SPI0_IRQ_EVENT) {
            status = BCM_ClearEvent(SPI0_IRQ_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
#if defined(ENABLE_SPI0)
            SPI_IRQEventHandler(0U);
#endif
        }
        if ((mask & SPI1_IRQ_EVENT) == SPI1_IRQ_EVENT) {
            status = BCM_ClearEvent(SPI1_IRQ_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
#if defined(ENABLE_SPI1)
            SPI_IRQEventHandler(1U);
#endif
        }
        if ((mask & SPI2_IRQ_EVENT) == SPI2_IRQ_EVENT) {
            status = BCM_ClearEvent(SPI2_IRQ_EVENT);
            if (BCM_ERR_OK != status) {
                break;
            }
#if defined(ENABLE_SPI2)
            SPI_IRQEventHandler(2U);
#endif
        }
#endif
    }

    (void)BCM_TerminateTask();
}
//! [Usage of VTMON_StateHandler]
//! [Usage of SPI_IRQEventHandler]
