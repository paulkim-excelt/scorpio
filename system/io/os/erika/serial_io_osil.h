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

/**
 * @file serial_io_osil.h
 *
 * @brief This file specifes common interfaces for Serial IO
 */

#ifndef SERIAL_IO_H
#define SERIAL_IO_H

#include "ee.h"

/* IIC IRQ process task events */

/** IIC0 Event */
#define IIC_0_EVENT              (SystemEvent0)
/** IIC1 Event */
#define IIC_1_EVENT              (SystemEvent1)
/** IIC2 Event */
#define IIC_2_EVENT              (SystemEvent2)
/** IIC3 Event */
#define IIC_3_EVENT              (SystemEvent3)
/** IIC Events */
#define IIC_EVENTS                  (IIC_0_EVENT | IIC_1_EVENT |\
                                     IIC_2_EVENT | IIC_3_EVENT)

/** VTMON IRQ event */
#define VTMON_IRQ_EVENT             (SystemEvent5)

/** SPI Events */
#define SPI_EVENTS                  (SPI0_IRQ_EVENT | SPI1_IRQ_EVENT | \
                                     SPI2_IRQ_EVENT)

/* SystemEvent9 reserved for IPC Server */

/** Events for slow IO tasks */
#define SERIAL_IO_EVENTS            (IIC_EVENTS | VTMON_IRQ_EVENT | SPI_EVENTS)

/** Bottom-half task for IIC */
#define IIC_IRQ_PROCESS_TASK        (TaskSerialIO)
/** Bottom-half task for VTMON */
#define VTMON_IRQ_PROCESS_TASK      (TaskSerialIO)


#endif /* SERIAL_IO_H */
/* Nothing past this line */
