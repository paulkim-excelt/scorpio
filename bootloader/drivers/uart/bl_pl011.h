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
    @addtogroup grp_bl_uart_impl
    @{
    @file bl_pl011.h
    @brief UART driver internal header

    @version 0.30 Imported from docx
*/

#ifndef BL_PL011_H
#define BL_PL011_H

#include <bl_uart.h>

/**
    @name UART Driver API IDs
    @{
    @brief API IDs for UART Driver
*/
#define BRCM_DSGN_BL_UART_PL011_RW_DEV_TYPE     (0x80U) /**< @brief #BL_PL011_RWDevType */
#define BRCM_DSGN_BL_UART_PL011_RO_DEV_TYPE     (0x81U) /**< @brief #BL_PL011_RODevType */
/** @} */

typedef volatile struct {
    uint32_t UARTDR;                        /**< Data Register */
#define UARTDR_OE           (0x00000800UL)  /**< Overrun Error */
#define UARTDR_BE           (0x00000400UL)  /**< Break Error */
#define UARTDR_PE           (0x00000200UL)  /**< Parity Error */
#define UARTDR_FE           (0x00000100UL)  /**< Framing Error */
#define UARTDR_DATA         (0x000000FFUL)
#define UARTDR_ERR          (UARTDR_OE | UARTDR_BE | UARTDR_PE | UARTDR_FE)

    uint32_t UARTRSR;                       /**< Receive Status Register */
#define UARTRSR_OE          (0x00000008UL)  /**< Overrun Error */
#define UARTRSR_BE          (0x00000004UL)  /**< Break Error */
#define UARTRSR_PE          (0x00000002UL)  /**< Parity Error */
#define UARTRSR_FE          (0x00000001UL)  /**< Framing Error */

    uint32_t RESERVED1;
    uint32_t RESERVED2;
    uint32_t RESERVED3;
    uint32_t RESERVED4;

    uint32_t UARTFR;                        /**< Flag Register */
#define UARTFR_RI           (0x00000100UL)  /**< Ring Indicator */
#define UARTFR_TXFE         (0x00000080UL)  /**< Transmit FIFO Empty */
#define UARTFR_RXFF         (0x00000040UL)  /**< Receive FIFO Full */
#define UARTFR_TXFF         (0x00000020UL)  /**< Transmit FIFO Full */
#define UARTFR_RXFE         (0x00000010UL)  /**< Receive FIFO Empty */
#define UARTFR_BUSY         (0x00000008UL)  /**< UART Busy */
#define UARTFR_DCD          (0x00000004UL)  /**< Data Carrier Detect */
#define UARTFR_DSR          (0x00000002UL)  /**< Data Set Ready */
#define UARTFR_CTS          (0x00000001UL)  /**< Clear To Send */

    uint32_t RESERVED5;

    uint32_t UARTILPR;                      /**< IrDA Low Power counter Register */
    uint32_t UARTIBRD;                      /**< Integer Baud Rate Divisor register */
    uint32_t UARTFBRD;                      /**< Fractional Baud Rate Divisor register */

    uint32_t UARTLCR;                       /**< Line Control Register */
#define UARTLCR_SPS         (0x00000080UL)  /**< Stick Parity Select */
#define UARTLCR_WLEN        (0x00000060UL)  /**< Word LENgth */
#define UARTLCR_WLEN8       (0x00000060UL)  /**< 8 bits */
#define UARTLCR_WLEN7       (0x00000040UL)  /**< 7 bits */
#define UARTLCR_WLEN6       (0x00000020UL)  /**< 6 bits */
#define UARTLCR_WLEN5       (0x00000000UL)  /**< 5 bits */
#define UARTLCR_FEN         (0x00000010UL)  /**< Enable FIFOs */
#define UARTLCR_STP2        (0x00000008UL)  /**< 2 SToP bits select */
#define UARTLCR_STOP1       (0x00000000UL)  /**< 1 Stop bit */
#define UARTLCR_STOP2       (0x00000008UL)  /**< 2 Stop bits */
#define UARTLCR_EPS         (0x00000004UL)  /**< Even Parity Select */
#define UARTLCR_PEVEN       (0x00000004UL)  /**< Even Parity */
#define UARTLCR_PODD        (0x00000000UL)  /**< Odd Parity */
#define UARTLCR_PEN         (0x00000002UL)  /**< Parity Enable */
#define UARTLCR_PAREN       (0x00000002UL)  /**< Parity Enabled */
#define UARTLCR_NOPAR       (0x00000000UL)  /**< Parity Disabled */
#define UARTLCR_BRK         (0x00000001UL)  /**< send BReaK */

    uint32_t UARTCR;                        /**< Control Register */
#define UARTCR_CTSEN        (0x00008000UL)  /**< CTS hardware flow control ENable */
#define UARTCR_OUT2         (0x00002000UL)  /**< nUARTOut2 modem status */
#define UARTCR_OUT1         (0x00001000UL)  /**< nUARTOut1 modem status */
#define UARTCR_RTS          (0x00000800UL)  /**< Request To Send */
#define UARTCR_DTR          (0x00000400UL)  /**< Data Transmit Ready */
#define UARTCR_RXE          (0x00000200UL)  /**< Receive Enable */
#define UARTCR_TXE          (0x00000100UL)  /**< Transmit Enable */
#define UARTCR_LBE          (0x00000080UL)  /**< Loop Back Enable */
#define UARTCR_SIRLP        (0x00000004UL)  /**< IrDA SIR Low Power mode */
#define UARTCR_SIREN        (0x00000002UL)  /**< SIR ENable */
#define UARTCR_UARTEN       (0x00000001UL)  /**< UART ENable */

    uint32_t UARTIFLS;                      /**< Interrupt FIFO Level Select register */
#define UARTIFLS_RXIFLSEL   (0x00000038UL)  /**< Recieve Interrupt FIFO Level SELect */
#define UARTIFLS_RX_1_DIV_8 (0X00000000UL)  /**< Receive FIFO becomes >= 1/8 full */
#define UARTIFLS_RX_1_DIV_4 (0X00000008UL)  /**< Receive FIFO becomes >= 1/4 full */
#define UARTIFLS_RX_1_DIV_2 (0X00000010UL)  /**< Receive FIFO becomes >= 1/2 full */
#define UARTIFLS_RX_3_DIV_4 (0X00000018UL)  /**< Receive FIFO becomes >= 3/4 full */
#define UARTIFLS_RX_7_DIV_8 (0X00000020UL)  /**< Receive FIFO becomes >= 7/8 full */
#define UARTIFLS_TXIFLSEL   (0x00000007UL)  /**< Transmit Interrupt FIFO Level SELect */
#define UARTIFLS_TX_1_DIV_8 (0X00000000UL)  /**< Transmit FIFO becomes <= 1/8 full */
#define UARTIFLS_TX_1_DIV_4 (0X00000001UL)  /**< Transmit FIFO becomes <= 1/4 full */
#define UARTIFLS_TX_1_DIV_2 (0X00000002UL)  /**< Transmit FIFO becomes <= 1/2 full */
#define UARTIFLS_TX_3_DIV_4 (0X00000003UL)  /**< Transmit FIFO becomes <= 3/4 full */
#define UARTIFLS_TX_7_DIV_8 (0X00000004UL)  /**< Transmit FIFO becomes <= 7/8 full */

    uint32_t UARTIMSC;                      /**< Interrupt Mask Set/Clear */
#define UARTIMSC_OEIM       (0x00000400UL)  /**< Overrun Error Interrupt Mask */
#define UARTIMSC_BEIM       (0x00000200UL)  /**< Break Error Interrupt Mask */
#define UARTIMSC_PEIM       (0x00000100UL)  /**< Parity Error Interrupt Mask */
#define UARTIMSC_FEIM       (0x00000080UL)  /**< Framing Error Interrupt Mask */
#define UARTIMSC_RTIM       (0x00000040UL)  /**< Receive Timeout Interrupt Mask */
#define UARTIMSC_TXIM       (0x00000020UL)  /**< Transmit Interrupt Mask */
#define UARTIMSC_RXIM       (0x00000010UL)  /**< Receive Interrupt Mask */
#define UARTIMSC_DSRMIM     (0x00000008UL)  /**< nUARTDSR Modem Interrupt Mask */
#define UARTIMSC_DCDMIM     (0x00000004UL)  /**< nUARTDCD Modem Interrupt Mask */
#define UARTIMSC_CTSMIM     (0x00000002UL)  /**< nUARTCTS Modem Interrupt Mask */
#define UARTIMSC_RIMIM      (0x00000001UL)  /**< nUARTRI Modem Interrupt Mask */

    uint32_t UARTRIS;                       /**< Raw Interrupt Status */
#define UARTRIS_OERIS       (0x00000400UL)  /**< Overrun Error Interrupt Status */
#define UARTRIS_BERIS       (0x00000200UL)  /**< Break Error Interrupt Status */
#define UARTRIS_PERIS       (0x00000100UL)  /**< Parity Error Interrupt Status */
#define UARTRIS_FERIS       (0x00000080UL)  /**< Framing Error Interrupt Status */
#define UARTRIS_RTRIS       (0x00000040UL)  /**< Receive Timeout Interrupt Status */
#define UARTRIS_TXRIS       (0x00000020UL)  /**< Transmit Interrupt Status */
#define UARTRIS_RXRIS       (0x00000010UL)  /**< Receive Interrupt Status */
#define UARTRIS_DSRRMIS     (0x00000008UL)  /**< nUARTDSR Modem Interrupt Status */
#define UARTRIS_DCDRMIS     (0x00000004UL)  /**< nUARTDCD Modem Interrupt Status */
#define UARTRIS_CTSRMIS     (0x00000002UL)  /**< nUARTCTS Modem Interrupt Status */
#define UARTRIS_RIRMIS      (0x00000001UL)  /**< nUARTRI Modem Interrupt Status */

    uint32_t UARTMIS;                       /**< Masked Interrupt Status */
#define UARTMIS_OEMIS       (0x00000400UL)  /**< Overrun Error Masked Interrupt Status */
#define UARTMIS_BEMIS       (0x00000200UL)  /**< Break Error Masked Interrupt Status */
#define UARTMIS_PEMIS       (0x00000100UL)  /**< Parity Error Masked Interrupt Status */
#define UARTMIS_FEMIS       (0x00000080UL)  /**< Framing Error Masked Interrupt Status */
#define UARTMIS_RTMIS       (0x00000040UL)  /**< Receive Timeout Masked Interrupt Status */
#define UARTMIS_TXMIS       (0x00000020UL)  /**< Transmit Masked Interrupt Status */
#define UARTMIS_RXMIS       (0x00000010UL)  /**< Receive Masked Interrupt Status */
#define UARTMIS_DSRMMIS     (0x00000008UL)  /**< nUARTDSR Modem Masked Interrupt Status */
#define UARTMIS_DCDMMIS     (0x00000004UL)  /**< nUARTDCD Modem Masked Interrupt Status */
#define UARTMIS_CTSMMIS     (0x00000002UL)  /**< nUARTCTS Modem Masked Interrupt Status */
#define UARTMIS_RIMMIS      (0x00000001UL)  /**< nUARTRI Modem Masked Interrupt Status */

    uint32_t UARTICR;                       /**< Interrupt Clear Register */
#define UARTICR_OEIC        (0x00000400UL)  /**< Overrun Error Interrupt Clear */
#define UARTICR_BEIC        (0x00000200UL)  /**< Break Error Interrupt Clear */
#define UARTICR_PEIC        (0x00000100UL)  /**< Parity Error Interrupt Clear */
#define UARTICR_FEIC        (0x00000080UL)  /**< Framing Error Interrupt Clear */
#define UARTICR_RTIC        (0x00000040UL)  /**< Receive Timeout Interrupt Clear */
#define UARTICR_TXIC        (0x00000020UL)  /**< Transmit Interrupt Clear */
#define UARTICR_RXIC        (0x00000010UL)  /**< Receive Interrupt Clear */
#define UARTICR_DSRMIC      (0x00000008UL)  /**< nUARTDSR Modem Interrupt Clear */
#define UARTICR_DCDMIC      (0x00000004UL)  /**< nUARTDCD Modem Interrupt Clear */
#define UARTICR_CTSMIC      (0x00000002UL)  /**< nUARTCTS Modem Interrupt Clear */
#define UARTICR_RIMIC       (0x00000001UL)  /**< nUARTRI Modem Interrupt Clear */

    uint32_t UARTDMACR;                     /**< DMA Control Register */
#define UARTDMACR_DMAONERR  (0x00000004UL)  /**< DMA ON ERRor */
#define UARTDMACR_TXDMAE    (0x00000002UL)  /**< Transmit DMA Enable */
#define UARTDMACR_RXDMAE    (0x00000001UL)  /**< Receive DMA Enable */

} BL_PL011_RegsType;

/**
    @brief PL011 RW device type

    @trace #BRCM_DSGN_BL_UART_PL011_RW_DEV_TYPE #BRCM_REQ_BL_UART_PL011_RW_DEV
 */
typedef struct {
    BL_UART_StateType state;               /**< @brief State of the UART */
    uint32_t timeout;                   /**< @brief Timeout for UARTFR busy loop */
} BL_PL011_RWDevType;

/**
    @brief PL011 RW device type

    @trace #BRCM_DSGN_BL_UART_PL011_RO_DEV_TYPE #BRCM_REQ_BL_UART_PL011_RO_DEV
 */
typedef struct {
    BL_PL011_RegsType *REGS;               /**< @brief Pointer to start address of PL011 controller */
} BL_PL011_RODevType;

#endif /* BL_PL011_H */

/** @} */