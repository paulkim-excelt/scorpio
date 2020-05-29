/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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

#ifndef PL022_H
#define PL022_H

#include <stdint.h>

typedef volatile struct {
    uint32_t SSPCR0;    /**< Control Register 0 */
#define SSPCR0_SCR_MASK         (0xFF00UL)
#define SSPCR0_SCR_SHIFT        (8UL)
#define SSPCR0_SPH_MASK         (0x80UL)
#define SSPCR0_SPH_SHIFT        (7UL)
#define SSPCR0_SPO_MASK         (0x40UL)
#define SSPCR0_SPO_SHIFT        (6UL)
#define SSPCR0_FRF_MASK         (0x30UL)
#define SSPCR0_FRF_SHIFT        (4UL)
#define SSPCR0_FRF_MOTOROLA     (0UL)
#define SSPCR0_FRF_TI           (1UL)
#define SSPCR0_FRF_NATIONAL     (2UL)
    uint32_t SSPCR1;    /**< Control Register 1 */
#define SSPCR1_MS_MASK          (0x4UL)
#define SSPCR1_MS_SHIFT         (2UL)
#define SSPCR1_SSE_MASK         (0x2UL)
#define SSPCR1_SSE_SHIFT        (1UL)
    uint32_t SSPDR;     /**< Receive FIFO (read) and transmit FIFO (write) data register */
    uint32_t SSPSR;     /**< Status register */
#define SSPSR_TFE_MASK          (0x1UL)
#define SSPSR_TFE_SHIFT         (0UL)
#define SSPSR_TNF_MASK          (0x2UL)
#define SSPSR_TNF_SHIFT         (1UL)
#define SSPSR_RNE_MASK          (0x4UL)
#define SSPSR_RNE_SHIFT         (2UL)
    uint32_t SSPCPSR;   /**< Clock prescale register */
    uint32_t SSPIMSC;   /**< Interrupt mask set and clear register */
#define SSPIMSC_TXIM_MASK       (0x8UL)
#define SSPIMSC_TXIM_SHIFT      (3UL)
#define SSPIMSC_RXIM_MASK       (0x4UL)
#define SSPIMSC_RXIM_SHIFT      (2UL)
#define SSPIMSC_RTIM_MASK       (0x2UL)
#define SSPIMSC_RTIM_SHIFT      (1UL)
#define SSPIMSC_RORIM_MASK      (0x1UL)
#define SSPIMSC_RORIM_SHIFT     (0UL)
    uint32_t SSPRIS;    /**< Raw interrupt status register */
    uint32_t SSPMIS;    /**< Masked interrupt status register */
    uint32_t SSPICR;    /**< Interrupt clear register */
    uint32_t SSPDMACR;  /**< DMA control register */
    uint32_t SSPTCR;    /**< Test control register */
    uint32_t SSPITIP;   /**< Integration test input register */
    uint32_t SSPITOP;   /**< Integration test output register */
    uint32_t SSPTDR;    /**< Test data register */
} PL022_RegsType;

#endif /* PL022_H */
