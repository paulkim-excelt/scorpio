/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_uartdrv_drv_impl UART Driver Design
    @ingroup grp_uartdrv

    @addtogroup grp_uartdrv_drv_impl
    @{

    @file drivers/uart/lib/uart_drv.c
    @brief UART Driver Design

    @version 0.1 Initial version
*/

#include <uart_osil.h>
#include <uart_rdb.h>
#include <bcm_err.h>
#include <stddef.h>
#include <string.h>
#include <compiler.h>

/**
    @name UART Driver Design IDs
    @{
    @brief Driver Design IDs for UART

*/
#define BRCM_SWDSGN_UART_DRV_INIT_PROC          (0x90U) /**< @brief #UART_DrvInit        */
#define BRCM_SWDSGN_UART_DRV_SEND_PROC          (0x91U) /**< @brief #UART_DrvSend        */
#define BRCM_SWDSGN_UART_DRV_GET_TX_STATUS_PROC (0x92U) /**< @brief #UART_DrvGetTxStatus */
#define BRCM_SWDSGN_UART_DRV_DEINIT_PROC        (0x93U) /**< @brief #UART_DrvDeInit      */
#define BRCM_SWDSGN_UART_DRV_GET_STATS_PROC     (0x94U) /**< @brief #UART_DrvGetStats    */
#define BRCM_SWDSGN_UART_DRV_SEND_BREAK_PROC    (0x95U) /**< @brief #UART_DrvSendBreak   */
#define BRCM_SWDSGN_UART_DRV_CLR_BREAK_PROC     (0x96U) /**< @brief #UART_DrvClrBreak    */
#define BRCM_SWDSGN_UART_DRV_GET_STATE_PROC     (0x97U) /**< @brief #UART_DrvGetState    */
#define BRCM_SWDSGN_UART_SVCIO_TYPE             (0x98U) /**< @brief #UART_SVCIOType      */
#define BRCM_SWDSGN_UART_SYS_CMD_HANDLER_PROC   (0x99U) /**< @brief #UART_SysCmdHandler  */
#define BRCM_SWDSGN_UART_IRQ_HANDLER_PROC       (0x9AU) /**< @brief #UART_IRQHandler     */
#define BRCM_SWDSGN_UART_ERR_MASK_MACRO         (0x9BU) /**< @brief #UART_ERR_MASK       */
#define BRCM_SWDSGN_UART_RWDEV_TYPE             (0x9CU) /**< @brief #UART_RWDevType      */
#define BRCM_SWDSGN_UART_RWDEV_GLOBAL           (0x9DU) /**< @brief #UART_RWDev          */
#define BRCM_SWDSGN_UART_REGS_GLOBAL            (0x9EU) /**< @brief #UART_Regs          */
#define BRCM_SWDSGN_UART_DRV_RX_HANDLER_PROC    (0x9FU) /**< @brief #UART_DrvRxHandler   */
#define BRCM_SWDSGN_UART_DRV_TX_HANDLER_PROC    (0xA0U) /**< @brief #UART_DrvTxHandler   */
/** @} */

/**
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER0_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER1_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER2_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
#define UART_ERR_MASK (UART_UARTDR_OE_MASK | UART_UARTDR_BE_MASK | UART_UARTDR_PE_MASK | UART_UARTDR_FE_MASK)

/**
    @trace #BRCM_SWARCH_UART_INIT_PROC
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWARCH_UART_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_GET_STATS_PROC
    @trace #BRCM_SWARCH_UART_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_GET_STATE_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
typedef struct _UART_RWDevType {
    UART_StateType state;               /**< State of the UART */
    UART_StatsType stats;               /**< UART Statistics counter structure */
    uint32_t timeout;                   /**< Timeout for uartfr busy loop */
    const uint8_t *data;
    uint32_t size;
    uint32_t curIdx;
} UART_RWDevType;

/**
    @trace #BRCM_SWARCH_UART_INIT_PROC
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWARCH_UART_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_GET_STATS_PROC
    @trace #BRCM_SWARCH_UART_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
static UART_RWDevType UART_RWDev[UART_MAX_HW_ID] COMP_SECTION(".data.drivers")=
{
    #if (UART_MAX_HW_ID == 0)
    #error "UART_MAX_HW_ID == 0"
    #endif
    {
     .state = UART_STATE_UNINIT,
     .data = NULL,
     .size = 0UL,
     .curIdx = 0UL,
    },
    #if (UART_MAX_HW_ID > 1)
    {
     .state = UART_STATE_UNINIT,
     .data = NULL,
     .size = 0UL,
     .curIdx = 0UL,
    },
    #endif
    #if (UART_MAX_HW_ID > 2)
    {
     .state = UART_STATE_UNINIT,
     .data = NULL,
     .size = 0UL,
     .curIdx = 0UL,
    },
    #endif
    #if (UART_MAX_HW_ID > 3)
    #error "UART_MAX_HW_ID > 3 is not supported by the driver"
    #endif
};

/**
    @trace #BRCM_SWARCH_UART_INIT_PROC
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWARCH_UART_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_GET_STATS_PROC
    @trace #BRCM_SWARCH_UART_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
static UART_RDBType * const UART_Regs[UART_MAX_HW_ID] =
{
#if (UART_MAX_HW_ID == 0)
#error "UART_MAX_HW_ID == 0"
#endif
    (UART_RDBType *)URT0_BASE,
#if (UART_MAX_HW_ID > 1)
    (UART_RDBType *)URT1_BASE,
#endif
#if (UART_MAX_HW_ID > 2)
    (UART_RDBType *)URT2_BASE,
#endif
#if (UART_MAX_HW_ID > 3)
#error "UART_MAX_HW_ID > 3 is not supported by the driver"
#endif
};


/** @brief  Handle RX Interrupt
 *
 * This API is used to handle RX interrupt and give a callback to the
 * registered client
 *
 * param[in]    Id  Index of the UART port
 *
 * return       void
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER0_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER1_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER2_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
static void UART_DrvRxHandler(UART_HwIDType aId)
{
    UART_ErrorType dataErr;
    uint8_t data;
    uint32_t dataReg;
    uint32_t maxCount = 2UL * 16UL; /* Twice RX Fifo depth */
    uint32_t flag = UART_Regs[aId]->uartfr;

    UART_RWDev[aId].stats.rxIsrCnt++;
    /* Mask the RX Interrupt */
    UART_Regs[aId]->uartimsc &= ~UART_UARTIMSC_RXIM_MASK;
    while ((flag & UART_UARTFR_RXFE_MASK) != UART_UARTFR_RXFE_MASK) {
        if(maxCount == 0UL) {
            break;
        }
        dataReg = UART_Regs[aId]->uartdr;
        /* Read the data register. If there is any error (Overrun, Break,
           Parity or Framing) ignore the byte otherwise copy the byte to
           internal buffer(rxBuff) */
        if ((dataReg & UART_ERR_MASK) != 0UL) {
            if ((dataReg & UART_UARTDR_OE_MASK) == UART_UARTDR_OE_MASK) {
                UART_RWDev[aId].stats.oeErr++;
            }
            if ((dataReg & UART_UARTDR_BE_MASK) == UART_UARTDR_BE_MASK) {
                UART_RWDev[aId].stats.beErr++;
            }
            if ((dataReg & UART_UARTDR_PE_MASK) == UART_UARTDR_PE_MASK) {
                UART_RWDev[aId].stats.peErr++;
            }
            if ((dataReg & UART_UARTDR_FE_MASK) == UART_UARTDR_FE_MASK) {
                UART_RWDev[aId].stats.feErr++;
            }
            dataErr = (UART_ErrorType)((dataReg & UART_ERR_MASK) >> UART_UARTDR_FE_SHIFT);
            data = (char)(((uint8_t)dataReg) & ((uint8_t)UART_UARTDR_DATA_MASK));
            UART_Config[aId].errCb(dataErr, data);
        } else {
            data = (char)(((uint8_t)dataReg) & ((uint8_t)UART_UARTDR_DATA_MASK));
            UART_Config[aId].rxCb(&data, 1UL);
        }
        maxCount--;
        flag = UART_Regs[aId]->uartfr;
    }
    /* Unmask the RX Interrupt */
    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RXIM_MASK;
}


/**
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER0_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER1_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER2_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
static void UART_DrvTxHandler(UART_HwIDType aId)
{
    /* Mask the TX Interrupt */
    UART_Regs[aId]->uartimsc &= ~UART_UARTIMSC_TXIM_MASK;
    while ((UART_Regs[aId]->uartfr & UART_UARTFR_TXFF_MASK) == 0UL) {
        if (UART_RWDev[aId].curIdx < UART_RWDev[aId].size) {
            UART_Regs[aId]->uartdr = (uint32_t)UART_RWDev[aId].data[UART_RWDev[aId].curIdx++];
        } else {
            UART_RWDev[aId].curIdx = 0UL;
            UART_RWDev[aId].size = 0UL;
            UART_RWDev[aId].data = NULL;
            UART_Config[aId].txCb();
            break;
        }
    }
    /* Unmask the TX Interrupt */
    UART_Regs[aId]->uartimsc |= UART_UARTIMSC_TXIM_MASK;
}

/**
    @trace #BRCM_SWARCH_UART_SEND_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_DrvSend(UART_HwIDType aId, const uint8_t* aData, uint32_t aSize)
{
    int32_t ret = BCM_ERR_OK;
    if ((UART_MAX_HW_ID <= aId)
        || (NULL == aData)
        || (0UL == aSize)) {
        ret = BCM_ERR_INVAL_PARAMS;
    } else if (UART_STATE_UNINIT == UART_RWDev[aId].state) {
        ret = BCM_ERR_INVAL_STATE;
    } else if (UART_RWDev[aId].curIdx != UART_RWDev[aId].size) {
        ret = BCM_ERR_BUSY;
    } else {
        UART_RWDev[aId].curIdx = 0UL;
        UART_RWDev[aId].size = aSize;
        UART_RWDev[aId].data = aData;
        UART_DrvTxHandler(aId);
    }

    return ret;
}
/** @brief  UART IRQ Handler
 *
 * This API is called when PL011 interrupt is asserted
 *
 * param[in]    Id  Index of the UART port. Passed by osil.
 *
 * return       void
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER0_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER1_PROC
    @trace #BRCM_SWARCH_UART_IRQ_HANDLER2_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
 */
void UART_IRQHandler(UART_HwIDType aId)
{
    uint32_t intr;

    if (aId >= UART_MAX_HW_ID) {
        goto err;
    }
    intr = UART_Regs[aId]->uartmis;
    /* Clear the interrupt */
    UART_Regs[aId]->uarticr = intr;
    if (UART_RWDev[aId].state != UART_STATE_INIT) {
        goto err;
    }
    if (intr != 0UL) {
        UART_RWDev[aId].stats.isrCnt++;
        if ((intr & UART_UARTMIS_RXMIS_MASK) == UART_UARTMIS_RXMIS_MASK) {
            /* Clear the interrupt */
            UART_Regs[aId]->uarticr |= UART_UARTICR_RXIC_MASK;
            UART_DrvRxHandler(aId);
        }
        if ((intr & UART_UARTMIS_RTMIS_MASK) == UART_UARTMIS_RTMIS_MASK) {
            /* Clear the interrupt */
            UART_Regs[aId]->uarticr |= UART_UARTICR_RTIC_MASK;
            UART_Config[aId].errCb((UART_ErrorType)UART_ERROR_TIMEOUT, 0U);
            UART_DrvRxHandler(aId);
        }
        if ((intr & UART_UARTMIS_TXMIS_MASK) == UART_UARTMIS_TXMIS_MASK) {
            /* Clear the interrupt */
            UART_Regs[aId]->uarticr |= UART_UARTICR_TXIC_MASK;
            UART_RWDev[aId].stats.txIsrCnt++;
            UART_DrvTxHandler(aId);
        }
    }

err:
    return;
}


/** @brief  Initialize PL011
 *
 *  This API is used to initialize the UART hardware with given parameters
 *
 *  param[in]   Id      Index of the UART port
 *  param[in]   Config  Pointer to the UART Configuration structure
 *
 *  return      void
    @trace #BRCM_SWARCH_UART_INIT_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static void UART_DrvInit(UART_HwIDType aId)
{
    uint32_t baud;
    uint32_t stopBits;
    uint32_t parity;
    uint32_t divisor;
    uint32_t timeout;

    if (aId < UART_MAX_HW_ID) {
        const UART_ConfigType *const config = &UART_Config[aId];
        if (UART_RWDev[aId].state == UART_STATE_UNINIT) {
            switch(config->baud) {
            case UART_BAUD_9600:
                baud = 9600UL;
            break;
            case UART_BAUD_19200:
                baud = 19200UL;
            break;
            case UART_BAUD_115200:
                baud = 115200UL;
            break;
            default:
                baud = 115200UL;
            break;
            }
            if (config->stopBits == UART_STOP_BITS1) {
                stopBits = 0UL;
            } else if (config->stopBits == UART_STOP_BITS2) {
                stopBits = UART_UARTLCR_STP2_MASK;
            } else {
                goto err;
            }
            if (config->parity == UART_PARITY_NONE) {
                parity = 0UL;
            } else if (config->parity == UART_PARITY_EVEN) {
                parity = UART_UARTLCR_PEN_MASK | UART_UARTLCR_EPS_MASK;
            } else if (config->parity == UART_PARITY_ODD) {
                parity = UART_UARTLCR_PEN_MASK;
            } else {
                goto err;
            }
            timeout = 100000UL;
            UART_RWDev[aId].timeout = timeout;
            /* Disable FIFO */
            UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_FEN_MASK;
            /* Wait for uart to finish the last transmit before
             * programming the control registers. */
            while (((UART_Regs[aId]->uartfr) & UART_UARTFR_BUSY_MASK)
                        == UART_UARTFR_BUSY_MASK) {
                if (timeout == 0UL) {
                    break;
                }
                timeout--;
            }
            /* Disable UART */
            UART_Regs[aId]->uartcr &= ~UART_UARTCR_UARTEN_MASK;
            /* Set TX and RX FIFO threshold level */
            UART_Regs[aId]->uartifls = (config->txFifoLvl << UART_UARTIFLS_TXIFLSEL_SHIFT)
                                        | (config->rxFifoLvl << UART_UARTIFLS_RXIFLSEL_SHIFT);
            /* Configure Baud Rate */
            divisor = (UART_CLOCK / 16UL) / baud;
            UART_Regs[aId]->uartibrd = divisor;
            UART_Regs[aId]->uartfbrd =
                ((UART_CLOCK - (divisor * 16UL * baud)) * 64UL) / (16UL * baud);
            /* Set the number of stop bits, parity, word length
               and enable FIFO */
            UART_Regs[aId]->uartlcr = stopBits | parity
                | UART_UARTLCR_FEN_MASK | UART_UARTLCR_WLEN_MASK;
            /* Enable Loop back */
            if (config->loopBackMode == 1UL) {
                UART_Regs[aId]->uartcr |= UART_UARTCR_LBE_MASK;
            }
            /* Set the state to Init */
            UART_RWDev[aId].state = UART_STATE_INIT;
            /* Unmask RX, RX Timeout, TX interrupts */
            UART_Regs[aId]->uartimsc |= UART_UARTIMSC_TXIM_MASK;
            UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RXIM_MASK;
            UART_Regs[aId]->uartimsc |= UART_UARTIMSC_RTIM_MASK;
            /* Enable TX, RX, UART */
            UART_Regs[aId]->uartcr |= (UART_UARTCR_TXE_MASK | UART_UARTCR_RXE_MASK
                | UART_UARTCR_UARTEN_MASK);
        }
    }

err:
    return;
}

/** @brief  Deinitialize PL011
 *
 *  This API is used to deinitialize the UART. Reset the controller and put
 *  back to its original state
 *
 *  param[in]   Id      Index of the UART port
 *
 *  return      BCM_ERR_OK              Success
 *              BCM_ERR_INVALID_PARAMS  Invalid hardware index
    @trace #BRCM_SWARCH_UART_DEINIT_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static int32_t UART_DrvDeInit(UART_HwIDType aId)
{
    uint32_t timeout;
    int32_t err = BCM_ERR_OK;

    if (aId < UART_MAX_HW_ID) {
        if (UART_RWDev[aId].state == UART_STATE_INIT) {
            timeout = UART_RWDev[aId].timeout;
            /* Mask all the interrupts */
            UART_Regs[aId]->uartimsc &= ~(UART_UARTIMSC_OEIM_MASK | UART_UARTIMSC_BEIM_MASK
                | UART_UARTIMSC_PEIM_MASK | UART_UARTIMSC_FEIM_MASK | UART_UARTIMSC_RTIM_MASK | UART_UARTIMSC_TXIM_MASK
                | UART_UARTIMSC_RXIM_MASK | UART_UARTIMSC_DSRMIM_MASK | UART_UARTIMSC_DCDMIM_MASK
                | UART_UARTIMSC_CTSMIM_MASK | UART_UARTIMSC_RIMIM_MASK);
            /* Clear all the interrupts */
            UART_Regs[aId]->uarticr = UART_UARTICR_RTIC_MASK | UART_UARTICR_TXIC_MASK
                | UART_UARTICR_RXIC_MASK | UART_UARTICR_DSRMIC_MASK | UART_UARTICR_DCDMIC_MASK | UART_UARTICR_CTSMIC_MASK
                | UART_UARTICR_RIMIC_MASK;
            /* Disable the FIFO mode */
            UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_FEN_MASK;
            /* Wait for UART to become idle */
            while (((UART_Regs[aId]->uartfr) & UART_UARTFR_BUSY_MASK)
                        == UART_UARTFR_BUSY_MASK) {
               if (timeout == 0UL) {
                    break;
                }
               timeout--;
            }
            /* Disable UART */
            UART_Regs[aId]->uartcr &= ~UART_UARTCR_UARTEN_MASK;
            /* Set the state to uninit */
            UART_RWDev[aId].state = UART_STATE_UNINIT;
        }
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

/** @brief  Send Break
 *
 *  This API is used to send Break on the respective hardware ID
 *
 *  param[in]   Id      Index of the UART port
 *
 *  return      void
    @trace #BRCM_SWARCH_UART_SEND_BREAK_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
int32_t UART_DrvSendBreak(UART_HwIDType aId)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (aId < UART_MAX_HW_ID) {
        UART_Regs[aId]->uartlcr |= UART_UARTLCR_BRK_MASK;
        ret = BCM_ERR_OK;
    }
    return ret;
}


/** @brief  Clear Break
 *
 *  This API is used to clear Break on the respective hardware ID
 *
 *  param[in]   Id      Index of the UART port
 *
 *  return      void
    @trace #BRCM_SWARCH_UART_CLR_BREAK_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
 */
int32_t UART_DrvClrBreak(UART_HwIDType aId)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (aId < UART_MAX_HW_ID) {
        UART_Regs[aId]->uartlcr &= ~UART_UARTLCR_BRK_MASK;
        ret = BCM_ERR_OK;
    }
    return ret;
}

/** @brief  Get the state of the PL011
 *
 *  This API is returns the current state of the UART in State argument
 *
 *  param[in]   Id      Index of the UART port
 *  param[out]  State   Current state of the UART
 *
 *  return      BCM_ERR_OK              Success
 *              BCM_ERR_INVALID_PARAMS  Invalid hardware index
    @trace #BRCM_SWARCH_UART_GET_STATE_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
 */
static int32_t UART_DrvGetState(UART_HwIDType aId, UART_StateType *aState)
{
    int32_t err = BCM_ERR_OK;

    if ((aId < UART_MAX_HW_ID) && (aState != NULL)) {
        *aState = UART_RWDev[aId].state;
    } else {
        err = BCM_ERR_INVAL_PARAMS;
    }

    return err;
}

/**
    @trace #BRCM_SWARCH_UART_GET_STATS_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_INIT
*/
int32_t UART_DrvGetStats(UART_HwIDType aId, UART_StatsType *const aStats)
{
    int32_t err = BCM_ERR_INVAL_PARAMS;
    if ((aId < UART_MAX_HW_ID) && (aStats != NULL)) {
        aStats->isrCnt = UART_RWDev[aId].stats.isrCnt;
        aStats->txIsrCnt = UART_RWDev[aId].stats.txIsrCnt;
        aStats->rxIsrCnt = UART_RWDev[aId].stats.rxIsrCnt;
        aStats->oeErr = UART_RWDev[aId].stats.oeErr;
        aStats->beErr = UART_RWDev[aId].stats.beErr;
        aStats->peErr = UART_RWDev[aId].stats.peErr;
        aStats->feErr = UART_RWDev[aId].stats.feErr;
        err = BCM_ERR_OK;
    }
    return err;
}

/**
    @trace #BRCM_SWARCH_UART_GET_TX_STATUS_PROC
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_COMMUNICATION
*/
int32_t UART_DrvGetTxStatus(UART_HwIDType aId, UART_TxStatusType *aStatus)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if ((aId < UART_MAX_HW_ID) && (aStatus != NULL)) {
        *aStatus = 0UL;
        if (0UL == UART_RWDev[aId].size) {
            *aStatus |= UART_TX_STATUS_BUFFER_IDLE;
        }
        if (0UL != (UART_Regs[aId]->uartfr & UART_UARTFR_TXFE_MASK)) {
            *aStatus |= UART_TX_STATUS_FIFO_IDLE;
        }
        ret = BCM_ERR_OK;
    }
    return ret;
}


/**
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
*/
typedef union _UART_SVCIOType {
    uint8_t *data;
    UART_IOType *io;
} UART_SVCIOType;

/**
    @trace #BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWREQ_UART_KERNEL_HANDLER
*/
void UART_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    UART_SVCIOType uartio;
    uartio.data = aSysIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_URT_ID == aMagicID) {
            retVal = BCM_ERR_OK;
            switch (aCmd) {
            case UART_CMD_INIT:
                UART_DrvInit(uartio.io->id);
                break;
            case UART_CMD_SEND:
                retVal = UART_DrvSend(uartio.io->id, uartio.io->data, uartio.io->size);
                break;
            case UART_CMD_TX_STATUS:
                retVal = UART_DrvGetTxStatus(uartio.io->id, &uartio.io->status);
                break;
            case UART_CMD_DEINIT:
                retVal = UART_DrvDeInit(uartio.io->id);
                break;
            case UART_CMD_SEND_BREAK:
                retVal = UART_DrvSendBreak(uartio.io->id);
                break;
            case UART_CMD_CLR_BREAK:
                retVal = UART_DrvClrBreak(uartio.io->id);
                break;
            case UART_CMD_GET_STATE:
                retVal = UART_DrvGetState(uartio.io->id, &uartio.io->state);
                break;
            case UART_CMD_GET_STATS:
                retVal = UART_DrvGetStats(uartio.io->id, &uartio.io->stats);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        uartio.io->retVal = retVal;
    }
    if (BCM_ERR_OK != retVal) {
        const uint32_t values[4] = {aMagicID, aCmd, (uint32_t)aSysIO, 0UL};
        BCM_ReportError(BCM_URT_ID, 0U, BRCM_SWARCH_UART_SYS_CMD_HANDLER_PROC, retVal,
                4UL, values);
    }
}

/** @} */


