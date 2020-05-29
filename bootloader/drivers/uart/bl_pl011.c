/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_bl_uart_impl Implementation
    @ingroup grp_bl_uart

    @addtogroup grp_bl_uart_impl
    @{
    @file bl_pl011.c
    @brief UART Driver Implementation
    This source file contains the implementation of UART driver

    @version 0.30 Imported from docx
*/

/* Includes */
#include <stddef.h>
#include <string.h>
#include <bl_chip_config.h>
#include <bl_bcm_err.h>
#include <bl_compiler.h>
#include "bl_pl011.h"

/* Macros */
#define BL_UART_REGS(id)           ((BL_PL011_RODev[id]).REGS)

/* Declarations */
static BL_PL011_RWDevType BL_PL011_RWDev[BL_UART_MAX_HW_ID] =
{
    #if (BL_UART_MAX_HW_ID == 0)
    #error "BL_UART_MAX_HW_ID == 0"
    #endif
    {.state = BL_UART_UNINIT},
    #if (BL_UART_MAX_HW_ID > 1)
    {.state = BL_UART_UNINIT},
    #endif
    #if (BL_UART_MAX_HW_ID > 2)
    {.state = BL_UART_UNINIT},
    #endif
    #if (BL_UART_MAX_HW_ID > 3)
    #error "BL_UART_MAX_HW_ID > 3 is not supported by the driver"
    #endif
};

static const BL_PL011_RODevType BL_PL011_RODev[BL_UART_MAX_HW_ID] =
{
    #if (BL_UART_MAX_HW_ID == 0)
    #error "BL_UART_MAX_HW_ID == 0"
    #endif
    {.REGS = (BL_PL011_RegsType *)BL_URT0_BASE},
    #if (BL_UART_MAX_HW_ID > 1)
    {.REGS = (BL_PL011_RegsType *)BL_URT1_BASE},
    #endif
    #if (BL_UART_MAX_HW_ID > 2)
    {.REGS = (BL_PL011_RegsType *)BL_URT2_BASE}
    #endif
    #if (BL_UART_MAX_HW_ID > 3)
    #error "BL_UART_MAX_HW_ID > 3 is not supported by the driver"
    #endif
};

static void BL_IntrDisableAll(BL_UART_HwIDType aId)
{
    BL_UART_REGS(aId)->UARTIMSC &= ~(UARTIMSC_OEIM | UARTIMSC_BEIM
        | UARTIMSC_PEIM | UARTIMSC_FEIM | UARTIMSC_RTIM | UARTIMSC_TXIM
        | UARTIMSC_RXIM | UARTIMSC_DSRMIM | UARTIMSC_DCDMIM
        | UARTIMSC_CTSMIM | UARTIMSC_RIMIM);
}

static void BL_IntrClearAll(BL_UART_HwIDType aId)
{
    BL_UART_REGS(aId)->UARTICR = UARTICR_RTIC | UARTICR_TXIC
        | UARTICR_RXIC | UARTICR_DSRMIC | UARTICR_DCDMIC | UARTICR_CTSMIC
        | UARTICR_RIMIC;
}

static uint32_t BL_CalcBusyTimeout(uint32_t baud)
{
    uint32_t timePerCharNS;
    uint32_t timeout;

    timePerCharNS = (1000000000UL / baud);
    timeout = (timePerCharNS / BL_NS_PER_CPU_CYCLE) * 8UL;

    return timeout;
}

static void BL_PL011_PutChar(BL_UART_HwIDType aId,
                             char aCh)
{
    uint32_t timeout = BL_PL011_RWDev[aId].timeout;

    if (aId < BL_UART_MAX_HW_ID) {
        if (BL_PL011_RWDev[aId].state == BL_UART_INIT) {
            BL_UART_REGS(aId)->UARTDR = (uint32_t)aCh;
            while (((timeout--) != 0UL) && (BL_UART_REGS(aId)->UARTFR & UARTFR_TXFF) != 0UL) {
            }
        }
    }
}

/** @brief  Initialize PL011
 *
 *  This API is used to initialize the UART hardware with given parameters
 *
 *  param[in]   Id      Index of the UART port
 *  param[in]   Config  Pointer to the UART Configuration structure
 *
 *  return      void
 */
static void BL_PL011_Init(BL_UART_HwIDType aId,
                          const BL_UART_ConfigType *const aConfig)
{
    uint32_t baud;
    uint32_t stopBits;
    uint32_t parity;
    uint32_t divisor;
    uint32_t timeout;

    if ((aId < BL_UART_MAX_HW_ID) && (aConfig != NULL)) {
        if (BL_PL011_RWDev[aId].state != BL_UART_INIT) {
            if (aConfig->baud == BL_UART_BAUD_9600) {
                baud = 9600UL;
            } else if (aConfig->baud == BL_UART_BAUD_115200) {
                baud = 115200UL;
            } else {
                goto err;
            }
            if (aConfig->stopBits == BL_UART_STOP1) {
                stopBits = UARTLCR_STOP1;
            } else if (aConfig->stopBits == BL_UART_STOP2) {
                stopBits = UARTLCR_STOP2;
            } else {
                goto err;
            }
            if (aConfig->parity == BL_UART_PARITY_NONE) {
                parity = UARTLCR_NOPAR;
            } else if (aConfig->parity == BL_UART_PARITY_EVEN) {
                parity = UARTLCR_PAREN | UARTLCR_PEVEN;
            } else if (aConfig->parity == BL_UART_PARITY_ODD) {
                parity = UARTLCR_PAREN | UARTLCR_PODD;
            } else {
                goto err;
            }
            timeout = BL_CalcBusyTimeout(baud);
            BL_PL011_RWDev[aId].timeout = timeout;
            /* Disable FIFO mode (work in character mode) */
            BL_UART_REGS(aId)->UARTLCR &= ~UARTLCR_FEN;
            /* Wait for uart to finish the last transmit before
             * programming the control registers. */
            while (((BL_UART_REGS(aId)->UARTFR) & UARTFR_BUSY)
                        == UARTFR_BUSY) {
                if (timeout == 0UL) {
                    break;
                }
                timeout--;
            }
            /* Disable UART */
            BL_UART_REGS(aId)->UARTCR &= ~UARTCR_UARTEN;
            /* Configure Baud Rate */
            divisor = (BL_UART_CLOCK / 16UL) / baud;
            BL_UART_REGS(aId)->UARTIBRD = divisor;
            BL_UART_REGS(aId)->UARTFBRD =
                ((BL_UART_CLOCK - (divisor * 16UL * baud)) * 64UL) / (16UL * baud);

            /* Set the number of stop bits, parity, word length
               and enable FIFO */
            BL_UART_REGS(aId)->UARTLCR = stopBits | parity | UARTLCR_WLEN8;
            /* Set the state to Init */
            BL_PL011_RWDev[aId].state = BL_UART_INIT;

            /* clear all interrupts and keep
             * interrupts disabled
             */
            BL_IntrClearAll(aId);
            BL_IntrDisableAll(aId);
            /* Enable TX, UART */
            BL_UART_REGS(aId)->UARTCR = (UARTCR_TXE | UARTCR_UARTEN);
        }
    }
err:
    return;
}

/**
    @image html bl_uart_flowchart_put_char.jpg
*/
void BL_UART_PutChar(BL_UART_HwIDType aId,
                     char aCh)
{
    BL_PL011_PutChar(aId, aCh);
}

/**
    @image html bl_uart_flowchart_init.jpg
*/
void BL_UART_Init(BL_UART_HwIDType aId,
                  const BL_UART_ConfigType *const aConfig)
{
    BL_PL011_Init(aId, aConfig);
}

/** @} */
