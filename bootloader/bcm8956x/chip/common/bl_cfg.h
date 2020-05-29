/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
/*TODO: Update the new cfg structure */

#ifndef BL_CFG_H
#define BL_CFG_H

/* Includes */
#include <stdint.h>

typedef volatile struct {
    uint32_t SR;
    uint32_t CFG_CPUSYS_MISC;
    uint32_t TM_0;
    uint32_t TM_1;
    uint32_t DEBUG_EN;
    uint32_t RSVD[3];
    uint32_t PARITY_DISABLE;
    uint32_t RSVD1[2];
    uint32_t SRAB_CMDSTAT;
    uint32_t SRAB_WDH;
    uint32_t SRAB_WDL;
    uint32_t SRAB_RDH;
    uint32_t SRAB_RDL;
    uint32_t SW_IF;
    uint32_t SW_INTR_CLR;
    uint32_t QSPI_IO_STATUS;
    uint32_t QSPI_IO_CONTROL;
    uint32_t QSPI_IP_REVID;
    uint32_t SPI_CRC_CONTROL;
    uint32_t SPI_CRC_STATUS;
    uint32_t CPU_INTR_RAW;
    uint32_t CPU_INTR_STAT;
    uint32_t CPU_INTR_MASK;
    uint32_t CPU_INTR_FORCE;
    uint32_t CPU_INTR_CFG;
    uint32_t SPI_CRC_IDLE_CYCLE_COUNT;
    uint32_t AHB2RDB_TIMEOUT;
} BL_CFG_RegsType;

#endif /* BL_CFG_H */
