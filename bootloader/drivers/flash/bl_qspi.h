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
    @addtogroup grp_bl_flash_impl
    @{
    @file bl_qspi.h
    @brief Flash driver internal header

    @version 0.30 Imported from docx
*/

#ifndef BL_QSPI_H
#define BL_QSPI_H

#include <stddef.h>
#include <inttypes.h>
#include <bl_flash.h>

/**
    @name Flash Driver API IDs
    @{
    @brief API IDs for Flash Driver
*/
#define BRCM_DSGN_BL_FLASH_MSPI_FIFO_LEN_MACRO  (0x80U) /**< @brief #BL_MSPI_FIFO_LEN */
#define BRCM_DSGN_BL_FLASH_RAF_FIFO_LEN_MACRO   (0x81U) /**< @brief #BL_RAF_FIFO_LEN */
#define BRCM_DSGN_BL_FLASH_STATE_TYPE           (0x82U) /**< @brief #BL_FLASH_StateType */
#define BRCM_DSGN_BL_FLASH_RW_DEV_TYPE          (0x83U) /**< @brief #BL_FLASH_RWDevType */
#define BRCM_DSGN_BL_FLASH_CMDS_MACRO           (0x84U) /**< @brief #BL_FLASH_CMD_READ */
#define BRCM_DSGN_BL_FLASH_QSPI_HW_ID_TYPE      (0x85U) /**< @brief #BL_QSPI_HwIDType */
#define BRCM_DSGN_BL_FLASH_QSPI_XFER_FLAG_TYPE  (0x86U) /**< @brief #BL_QSPI_XferFlagType */
#define BRCM_DSGN_BL_FLASH_QSPI_CONFIG_TYPE     (0x87U) /**< @brief #BL_QSPI_ConfigType */
#define BRCM_DSGN_BL_FLASH_QSPI_INIT_PROC       (0x88U) /**< @brief #BL_QSPI_Init */
#define BRCM_DSGN_BL_FLASH_QSPI_RAF_STOP_PROC   (0x89U) /**< @brief #BL_QSPI_RafStop */
#define BRCM_DSGN_BL_FLASH_QSPI_RAF_READ_PROC   (0x8AU) /**< @brief #BL_QSPI_RafRead */
#define BRCM_DSGN_BL_FLASH_QSPI_XFER_PROC       (0x8BU) /**< @brief #BL_QSPI_Xfer */
#define BRCM_DSGN_BL_FLASH_QSPI_READ_PROC       (0x8CU) /**< @brief #BL_QSPI_Read */
/** @} */

/**
    @name BL_QSPI_HwIDType
    @{
    @brief QSPI - Hardware ID type

    @trace #BRCM_DSGN_BL_FLASH_QSPI_HW_ID_TYPE #BRCM_REQ_BL_FLASH_QSPI_HW_ID
 */
#define BL_QSPI_HwIDType   BL_FLASH_HwIDType /**< @brief type for QSPI Hardware ID */
#define BL_QSPI_HW_ID_0    (0UL)          /**< @brief QSPI 0 controller ID */
#define BL_QSPI_HW_ID_1    (1UL)          /**< @brief QSPI 1 controller ID */
/** @} */

/**
    @name BL_QSPI_XferFlagType
    @{
    @brief Flag for transfer continuation

    @trace #BRCM_DSGN_BL_FLASH_QSPI_XFER_FLAG_TYPE #BRCM_REQ_BL_FLASH_BL_QSPI_XFER_FLAG
 */
typedef uint32_t BL_QSPI_XferFlagType; /**< @brief typedef for QSPI transfer continuation flag */
#define BL_QSPI_XFER_STOP      (0UL)   /**< @brief Transfer stop flag */
#define BL_QSPI_XFER_CONTINUE  (1UL)   /**< @brief Transfer continue flag */
/** @} */

/**
    @brief MSPI FIFO length in bytes

    @trace #BRCM_DSGN_BL_FLASH_MSPI_FIFO_LEN_MACRO #BRCM_REQ_BL_FLASH_BL_MSPI_FIFO_LEN
*/
#define BL_MSPI_FIFO_LEN                       (16UL)

/**
    @brief RAF FIFO length in bytes

    @trace #BRCM_DSGN_BL_FLASH_RAF_FIFO_LEN_MACRO #BRCM_REQ_BL_FLASH_BL_RAF_FIFO_LEN
*/
#define BL_RAF_FIFO_LEN                        (256UL)

/**
    @brief QSPI config structure

    @trace #BRCM_DSGN_BL_FLASH_QSPI_CONFIG_TYPE #BRCM_REQ_BL_FLASH_QSPI_CONFIG
 */
typedef struct {
    BL_FLASH_SPIModeType    SPIMode;    /**< @brief SPI mode */
    BL_FLASH_SpeedType      speed;      /**< @brief SPI speed */
    BL_FLASH_ReadLaneType   readLane;   /**< @brief Read lane config */
} BL_QSPI_ConfigType;

typedef volatile struct {
    uint32_t    BSPI_REVISION_ID;         /**< Revision ID */
    uint32_t    BSPI_SCRATCH;             /**< Scratch bits */
    uint32_t    BSPI_MAST_N_BOOT_CTRL;    /**< Master/Boot SPI ctrl Reg */
#define BSPI_MAST_N_BOOT_CTRL_BIT_MASK                  (0x00000001UL)
#define BSPI_MAST_N_BOOT_CTRL_BIT_SHIFT                 (0UL)
    uint32_t    BSPI_BUSY_STATUS;         /**< BSPI Busy Status Reg */
    uint32_t    BSPI_INTR_STATUS;         /**< Interrupt Status Reg */
    uint32_t    BSPI_B0_STATUS;           /**< Prefetch Buffer 0 Status Reg */
    uint32_t    BSPI_B0_CTRL;             /**< Prefetch Buffer 0 ctrl Reg */
    uint32_t    BSPI_B1_STATUS;           /**< Prefetch Buffer 1 Status Reg */
    uint32_t    BSPI_B1_CTRL;             /**< Prefetch Buffer 1 ctrl Reg */
    uint32_t    BSPI_STRAP_OVERRIDE_CTRL; /**< Dual/Single Recv Mode Ctrl Reg */
#define BSPI_STRAP_OR_CTRL_OR_MASK                      (0x00000001UL)
#define BSPI_STRAP_OR_CTRL_OR_SHIFT                     (0UL)
#define BSPI_STRAP_OR_CTRL_DUAL_SGL_MASK                (0x00000002UL)
#define BSPI_STRAP_OR_CTRL_DUAL_SGL_SHIFT               (1UL)
#define BSPI_STRAP_OR_CTRL_QUAD_MASK                    (0x00000008UL)
#define BSPI_STRAP_OR_CTRL_QUAD_SHIFT                   (3UL)
    uint32_t    BSPI_FLEX_MODE_ENABLE;    /**< Flexible Ctrl Mode Enable Reg */
    uint32_t    BSPI_BITS_PER_CYCLE;      /**< Bits per cycle b-p-c Ctrl Reg */
    uint32_t    BSPI_BITS_PER_PHASE;      /**< Bits per Phase b-p-p Ctrl Reg */
    uint32_t    BSPI_CMD_N_MODE_BYTE;     /**< Command and Mode Data Reg */
    uint32_t    BSPI_FLASH_UPPER_ADDR;    /**< Flash upper address byte Reg */
    uint32_t    BSPI_XOR_VALUE;           /**< Flash XOR Value Reg */
    uint32_t    BSPI_XOR_EN;              /**< Flash XOR Enable Reg */
    uint32_t    BSPI_PIO_MODE_EN;         /**< Programmed IO Mode Enable Reg */
    uint32_t    BSPI_PIO_IODIR;           /**< Programmed IO Mode Direction Reg */
    uint32_t    BSPI_PIO_DATA;            /**< Programmed IO Mode Data Reg */
    uint32_t    RSVD0[44];                /**< Reserved 1 */
    uint32_t    RAF_START_ADDR;           /**< Physical Starting Address in Flash */
    uint32_t    RAF_NUM_WORDS;            /**< Number of Words to be fetched */
    uint32_t    RAF_CTRL;                 /**< RAF Session Control Register */
#define RAF_CTRL_CLEAR_MASK                             (0x00000002UL)
#define RAF_CTRL_CLEAR_SHIFT                            (1UL)
#define RAF_CTRL_START_MASK                             (0x00000001UL)
#define RAF_CTRL_START_SHIFT                            (0UL)
    uint32_t    RAF_FULLNESS;             /**< Fullness indicator for RAF buffer */
    uint32_t    RAF_WATERMARK;            /**< RAF buffer interrupt trigger watermark level */
#define RAF_WATERMARK_FULLNESS_MASK                     (0x00000003UL)
#define RAF_WATERMARK_FULLNESS_SHIFT                    (0UL)
    uint32_t    RAF_STATUS;               /**< Linear Read Status Register */
#define RAF_STATUS_FIFO_FULL_MASK                       (0x00000004UL)
#define RAF_STATUS_FIFO_FULL_SHIFT                      (2UL)
#define RAF_STATUS_FIFO_EMPTY_MASK                      (0x00000002UL)
#define RAF_STATUS_FIFO_EMPTY_SHIFT                     (1UL)
#define RAF_STATUS_SESSION_BUSY_MASK                    (0x00000001UL)
#define RAF_STATUS_SESSION_BUSY_SHIFT                   (0UL)
    uint32_t    RAF_READ_DATA;            /**< Read data from RAF buffer */
    uint32_t    RAF_WORD_CNT;             /**< Current number of words fetched from Flash */
    uint32_t    RAF_CURR_ADDR;            /**< Current read address for linear read session */
    uint32_t    RSVD1[55];                 /**< Reserved 2 */
    uint32_t    MSPI_SPCR0_LSB;           /**< SPCR0_LSB REGISTER */
#define MSPI_SPCR0_LSB_SPBR_MASK                        (0x000000ffUL)
#define MSPI_SPCR0_LSB_SPBR_SHIFT                       (0UL)
    uint32_t    MSPI_SPCR0_MSB;           /**< SPCR0_MSB Register */
#define MSPI_SPCR0_MSB_MSTR_MASK                        (0x00000080UL)
#define MSPI_SPCR0_MSB_MSTR_SHIFT                       (7UL)
#define MSPI_SPCR0_MSB_CPOL_MASK                        (0x00000002UL)
#define MSPI_SPCR0_MSB_CPOL_SHIFT                       (1UL)
#define MSPI_SPCR0_MSB_CPHA_MASK                        (0x00000001UL)
#define MSPI_SPCR0_MSB_CPHA_SHIFT                       (0UL)
    uint32_t    MSPI_SPCR1_LSB;           /**< SPCR1_LSB REGISTER */
    uint32_t    MSPI_SPCR1_MSB;           /**< SPCR1_MSB REGISTER */
    uint32_t    MSPI_NEWQP;               /**< NEWQP REGISTER */
    uint32_t    MSPI_ENDQP;               /**< ENDQP REGISTER */
    uint32_t    MSPI_SPCR2;               /**< SPCR2 REGISTER */
#define MSPI_SPCR2_CONT_AFTER_CMD_MASK                  (0x00000080UL)
#define MSPI_SPCR2_CONT_AFTER_CMD_ALIGN                 (0UL)
#define MSPI_SPCR2_CONT_AFTER_CMD_BITS                  (1UL)
#define MSPI_SPCR2_CONT_AFTER_CMD_SHIFT                 (7UL)
#define MSPI_SPCR2_SPE_MASK                             (0x00000040UL)
#define MSPI_SPCR2_SPE_SHIFT                            (6UL)
#define MSPI_SPCR2_SPIFIE_MASK                          (0x00000020UL)
#define MSPI_SPCR2_SPIFIE_SHIFT                         (5UL)
#define MSPI_SPCR2_WREN_MASK                            (0x00000010UL)
#define MSPI_SPCR2_WREN_SHIFT                           (4UL)
#define MSPI_SPCR2_WRT0_MASK                            (0x00000008UL)
#define MSPI_SPCR2_WRT0_SHIFT                           (3UL)
#define MSPI_SPCR2_LOOPQ_MASK                           (0x00000004UL)
#define MSPI_SPCR2_LOOPQ_SHIFT                          (2UL)
#define MSPI_SPCR2_HIE_MASK                             (0x00000002UL)
#define MSPI_SPCR2_HIE_SHIFT                            (1UL)
#define MSPI_SPCR2_HALT_MASK                            (0x00000001UL)
#define MSPI_SPCR2_HALT_SHIFT                           (0UL)
    uint32_t    RSVD2[1];
    uint32_t    MSPI_STATUS;              /**< MSPI STATUS REGISTER */
#define MSPI_STATUS_HALTA_MASK                          (0x00000002UL)
#define MSPI_STATUS_HALTA_SHIFT                         (1UL)
#define MSPI_STATUS_SPIF_MASK                           (0x00000001UL)
#define MSPI_STATUS_SPIF_SHIFT                          (0UL)
    uint32_t    MSPI_CPTQP;               /**< CPTQP REGISTER */
    uint32_t    RSVD3[6];
    uint32_t    MSPI_TXRAM[32];           /**< Transmit RAM */
    uint32_t    MSPI_RXRAM[32];           /**< Receive RAM */
    uint32_t    MSPI_CDRAM[16];           /**< Command RAM */
#define MSPI_CDRAM_CDRAM_MASK                           (0x000000ffUL)
#define MSPI_CDRAM_CDRAM_SHIFT                          (0UL)
#define MSPI_CDRAM_CMD_CONTINUE_MASK                    (0x80UL)
#define MSPI_CDRAM_CMD_BITS_PER_TRANF_MASK              (0x40UL)
#define MSPI_CDRAM_CMD_DT_DELAY_MASK                    (0x20UL)
#define MSPI_CDRAM_CMD_DSCK_DELAY_MASK                  (0x10UL)
#define MSPI_CDRAM_CMD_PCS_MASK                         (0x03UL)
   uint32_t    MSPI_WRITE_LOCK;           /**< Ctrl bit to lock group of write commands */
#define MSPI_WRITE_LOCK_WRITELOCK_MASK                  (0x00000001UL)
#define MSPI_WRITE_LOCK_WRITELOCK_SHIFT                 (0UL)
    uint32_t    MSPI_DIS_FLUSH_GEN;       /**< Debug bit to mask MSPI flush signals */
#define MSPI_DIS_FLUSH_GEN_MASK                         (0x00000001UL)
#define MSPI_DIS_FLUSH_GEN_SHIFT                        (0UL)
    uint32_t    RSVD4[6];                 /**< Reserved 3 */
    uint32_t    RAF_LR_FULLNESS_REACHED;  /**< Interrupt from RAF sub-block */
#define RAF_LR_FULLNESS_REACHED_MASK                    (0x00000001UL)
#define RAF_LR_FULLNESS_REACHED_SHIFT                   (0UL)
    uint32_t    RAF_LR_TRUNCATED;         /**< Interrupt from RAF sub-block */
#define RAF_LR_TRUNCATED_MASK                           (0x00000001UL)
#define RAF_LR_TRUNCATED_SHIFT                          (0UL)
    uint32_t    RAF_LR_IMPATIENT;         /**< Interrupt from RAF sub-block */
#define RAF_LR_IMPATIENT_MASK                           (0x00000001UL)
#define RAF_LR_IMPATIENT_SHIFT                          (0UL)
    uint32_t    RAF_LR_SESSION_DONE;      /**< Interrupt from RAF sub-block */
#define RAF_LR_SESSION_DONE_MASK                        (0x00000001UL)
#define RAF_LR_SESSION_DONE_SHIFT                       (0UL)
    uint32_t    RAF_LR_OVERREAD;          /**< Interrupt from RAF sub-block */
#define RAF_LR_OVERREAD_MASK                            (0x00000001UL)
#define RAF_LR_OVERREAD_SHIFT                           (0UL)
    uint32_t    MSPI_DONE;                /**< Interrupt from MSPI sub-block */
#define MSPI_DONE_MASK                                  (0x00000001UL)
#define MSPI_DONE_SHIFT                                 (0UL)
    uint32_t    MSPI_HALT_SET_TRANS_DONE; /**< Interrupt from MSPI sub-block */
#define MSPI_HALT_SET_TRANS_DONE_MASK                   (0x00000001UL)
#define MSPI_HALT_SET_TRANS_DONE_ALIGN                  (0UL)
#define MSPI_HALT_SET_TRANS_DONE_BITS                   (1UL)
#define MSPI_HALT_SET_TRANS_DONE_SHIFT                  (0UL)
} BL_QSPI_RegsType;

extern void BL_QSPI_Init(BL_QSPI_HwIDType aID,
                         BL_QSPI_ConfigType *const aConfig);

extern int32_t BL_QSPI_Xfer(BL_QSPI_HwIDType aID,
                            const uint8_t *const aTxData,
                            uint8_t *const aRxData,
                            uint32_t aLen,
                            BL_QSPI_XferFlagType aFlag);

extern int32_t BL_QSPI_Read(BL_QSPI_HwIDType aID,
                            uint32_t aAddr,
                            uint8_t *const aData,
                            uint32_t aLen);

#endif /* BL_QSPI_H */

/** @} */
