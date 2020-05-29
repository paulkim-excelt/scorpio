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
    @defgroup grp_bl_flash Flash
    @ingroup grp_bl_drivers

    @addtogroup grp_bl_flash
    @{
    @section sec_bl_flash_fsm State Machine
    @image html bl_flash_state_machine.jpg "Flash driver state machine"

    @file bl_flash.h
    @brief Interface for Flash driver
    This header file defines interface for Flash driver.

    @version 0.30 Imported from docx
*/

#ifndef BL_FLASH_H
#define BL_FLASH_H

#include <stddef.h>
#include <mcu.h>
#include <inttypes.h>

/**
    @name Flash driver API IDs
    @{
    @brief API IDs for Flash driver
 */
#define BRCM_ARCH_BL_FLASH_HW_ID_TYPE              (0x00U) /**< @brief #BL_FLASH_HwIDType */
#define BRCM_ARCH_BL_FLASH_ADDR_TYPE               (0x01U) /**< @brief #BL_FLASH_AddrType */
#define BRCM_ARCH_BL_FLASH_LEN_TYPE                (0x02U) /**< @brief #BL_FLASH_LenType */
#define BRCM_ARCH_BL_FLASH_READ_LANE_TYPE          (0x03U) /**< @brief #BL_FLASH_ReadLaneType */
#define BRCM_ARCH_BL_FLASH_SPI_MODE_TYPE           (0x04U) /**< @brief #BL_FLASH_SPIModeType */
#define BRCM_ARCH_BL_BL_FLASH_SPEED_TYPE              (0x05U) /**< @brief #BL_FLASH_SpeedType */
#define BRCM_ARCH_BL_FLASH_FLS_ID_TYPE             (0x06U) /**< @brief #BL_FLASH_FlsIDType */
#define BRCM_ARCH_BL_FLASH_CFG_TYPE                (0x07U) /**< @brief #BL_FLASH_CfgType */
#define BRCM_ARCH_BL_FLASH_INIT_PROC               (0x08U) /**< @brief #BL_FLASH_Init */
#define BRCM_ARCH_BL_FLASH_ERASE_SECTOR_PROC       (0x09U) /**< @brief #BL_FLASH_EraseSector */
#define BRCM_ARCH_BL_FLASH_ERASE_SUB_SECTOR_PROC   (0x0AU) /**< @brief #BL_FLASH_EraseSubSector */
#define BRCM_ARCH_BL_FLASH_WRITE_PROC              (0x0BU) /**< @brief #BL_FLASH_Write */
#define BRCM_ARCH_BL_FLASH_READ_PROC               (0x0CU) /**< @brief #BL_FLASH_Read */
#define BRCM_ARCH_BL_FLASH_READ_ID_PROC            (0x0DU) /**< @brief #BL_FLASH_ReadID */
/** @} */

/**
    @name BL_FLASH_HwIDType
    @{
    @brief Flash controller ID type

    @trace #BRCM_ARCH_BL_FLASH_HW_ID_TYPE #BRCM_REQ_FLASH_HW_ID
 */
typedef uint32_t BL_FLASH_HwIDType;            /**< @brief FLash controller ID type */
#define BL_FLASH_HW_ID_0               (0UL)   /**< @brief Flash controller 0 */
#define BL_FLASH_HW_ID_1               (1UL)   /**< @brief Flash controller 1 */
/** @} */

/**
    @brief Flash Address Type

    @trace #BRCM_ARCH_BL_FLASH_ADDR_TYPE #BRCM_REQ_FLASH_ADDR
*/
typedef uint32_t BL_FLASH_AddrType;

/**
    @brief Flash length type
    This type shall be same as Flash address type as these will be used for
    arithmetics.

    @trace #BRCM_ARCH_BL_FLASH_LEN_TYPE #BRCM_REQ_FLASH_LEN
 */
typedef uint32_t BL_FLASH_LenType;

/**
    @name BL_FLASH_ReadLaneType
    @{
    @brief Flash read lane type

    @trace #BRCM_ARCH_BL_FLASH_READ_LANE_TYPE #BRCM_REQ_FLASH_READ_LANE
*/
typedef uint32_t BL_FLASH_ReadLaneType;        /**< @brief typedef for flash read lane */
#define BL_FLASH_READ_LANE_SINGLE      (1UL)   /**< @brief Flash single lane read mode */
#define BL_FLASH_READ_LANE_DUAL        (2UL)   /**< @brief Flash dual lane read mode */
#define BL_FLASH_READ_LANE_QUAD        (3UL)   /**< @brief Flash quad lane read mode */
/** @} */

/**
    @name BL_FLASH_SPIModeType
    @{
    @brief Flash SPI mode type

    @trace #BRCM_ARCH_BL_FLASH_SPI_MODE_TYPE #BRCM_REQ_FLASH_SPI_MODE
 */
typedef uint32_t BL_FLASH_SPIModeType;         /**< @brief typedef for Flash SPI mode */
#define BL_FLASH_SPI_MODE_0            (1UL)   /**< @brief SPI mode-0 (CPOL = 0, CPHA = 0) */
#define BL_FLASH_SPI_MODE_1            (2UL)   /**< @brief SPI mode-1 (CPOL = 0, CPHA = 1) */
#define BL_FLASH_SPI_MODE_2            (3UL)   /**< @brief SPI mode-2 (CPOL = 1, CPHA = 0) */
#define BL_FLASH_SPI_MODE_3            (4UL)   /**< @brief SPI mode-3 (CPOL = 1, CPHA = 1) */
/** @} */

/**
    @name BL_FLASH_SpeedType
    @{
    @brief Flash speed type

    @trace #BRCM_ARCH_BL_BL_FLASH_SPEED_TYPE #BRCM_REQ_BL_FLASH_SPEED
*/
typedef uint32_t BL_FLASH_SpeedType;           /**< @brief typedef for flash speed */
#define BL_FLASH_SPI_SPEED_25M         (0UL)   /**< @brief SPI speed = 25MHz */
#define BL_FLASH_SPI_SPEED_50M         (1UL)   /**< @brief SPI speed = 50MHz */
#define BL_FLASH_SPI_SPEED_66M         (2UL)   /**< @brief SPI speed = 66MHz */
#define BL_FLASH_SPI_SPEED_80M         (3UL)   /**< @brief SPI speed = 80MHz */
/** @} */

/**
    @name BL_FLASH_FlsIDType
    @{
    @brief Flash ID type

    Flash ID type
    Bits 23 - 16: Manufacturer ID,
    Bits 15 - 8: Device ID15-8,
    Bits 7 - 0: Device ID7-0

    @trace #BRCM_ARCH_BL_FLASH_FLS_ID_TYPE #BRCM_REQ_FLASH_FLSID
*/
typedef uint32_t BL_FLASH_FlsIDType;           /**< @brief typedef for flash ID */
#define BL_WINBOND_W25Q16      (0xEF4015UL)    /**< @brief Winbond, W25Q16 flash ID */
#define BL_WINBOND_W25Q64CV    (0xEF4017UL)    /**< @brief Winbond, W25Q64CV flash ID */
#define BL_MICRON_N25Q512A     (0x20BA20UL)    /**< @brief Micron, N25Q512A flash ID */
#define BL_MACRONIX_MX25R32    (0xc22816UL)    /**< @brief Micronix, MX25R32F35F flash ID */
#define BL_MACRONIX_MX25L32    (0xc22016UL)    /**< @brief Micronix, MX25L3233F flash ID */
#define BL_MICRON_M25P16       (0x202015UL)    /**< @brief Micron, M25P16 flash ID */
#define BL_S25FL064P           (0x010216UL)    /**< @brief Cypress, S25FL064P flash ID */
/** @} */

/**
    @brief Flash configuration structure

    @trace #BRCM_ARCH_BL_FLASH_CFG_TYPE #BRCM_REQ_FLASH_CFG_STRUCTURE
 */
typedef struct {
    BL_FLASH_HwIDType hwID;           /**< @brief Flash hardware Index */
    BL_FLASH_LenType size;            /**< @brief Total flash size in bytes */
    BL_FLASH_LenType pageSize;        /**< @brief Flash page size in bytes */
    BL_FLASH_LenType sectorSize;      /**< @brief Flash sector size in bytes */
    uint32_t subSectorSupported;      /**< @brief #TRUE: Subsector supported, FALSE: not supported */
    BL_FLASH_LenType subSectorSize;   /**< @brief Flash sector size in bytes (if subSectorSupported is #TRUE) */
    BL_FLASH_SPIModeType SPIMode;     /**< @brief SPI mode */
    BL_FLASH_SpeedType speed;         /**< @brief SPI speed */
    BL_FLASH_ReadLaneType readLane;   /**< @brief Number of lane of read operation */
    MCU_ClkCfgIDType clkCfgID;        /**< @brief MCU clock configuration ID */
} BL_FLASH_CfgType;

/** @brief Initialize Flash driver

    This API initializes flash driver.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Index of the controller
    @param[in]      aConfig     Pointer to configuration structure

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_INIT_PROC  #BRCM_REQ_FLASH_INIT

    @limitations None
*/
extern void BL_FLASH_Init(BL_FLASH_HwIDType aID,
                          const BL_FLASH_CfgType *const aConfig);

/** @brief Erase flash sector

    API to erase flash sector/s. Number of sectors to be erased will be
    determined using aAddr and aLen parameters.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Flash start address for operation. It shall be
                                a Flash sector size aligned address
    @param[in]      aLen        Length in bytes. It shall be Flash
                                sector size aligned.

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK              Flash operation successfull
    @retval     #BL_BCM_ERR_INVAL_PARAMS    (aID is invalid) or
                                            (aLen is zero) or
                                            (aAddr + aLen > Total flash size) or
                                            Either aAddr or aLen is not aligned
                                            to sector boundary
    @retval     #BL_BCM_ERR_UNINIT          API called in uninitialized driver
                                            state

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_ERASE_SECTOR_PROC  #BRCM_REQ_FLASH_ERASE_SECTOR

    @limitations None
*/
extern int32_t BL_FLASH_EraseSector(BL_FLASH_HwIDType aID,
                                    BL_FLASH_AddrType aAddr,
                                    BL_FLASH_LenType aLen);

/** @brief Erase flash sub sector

    API to erase flash sub sector/s. Number of sub sectors to be erased will be
    determined using aAddr and aLen parameters.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Flash start address for operation. It shall be a Flash sub sector size aligned address.
    @param[in]      aLen        Length in bytes. It shall be Flash sub sector size aligned.

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK              Flash operation successfull
    @retval     #BL_BCM_ERR_INVAL_PARAMS    (aID is invalid) or
                                            (aLen is zero) or
                                            (aAddr + aLen > Total flash size) or
                                            Either aAddr or aLen is not aligned
                                            to sub sector boundary
    @retval     #BL_BCM_ERR_UNINIT          API called in uninitialized driver
                                            state

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_ERASE_SUB_SECTOR_PROC  #BRCM_REQ_FLASH_ERASE_SUB_SECTOR

    @limitations None
*/
extern int32_t BL_FLASH_EraseSubSector(BL_FLASH_HwIDType aID,
                                       BL_FLASH_AddrType aAddr,
                                       BL_FLASH_LenType aLen);

/** @brief Flash write

    API to write flash from aAddr for aLen bytes. Flash write will happen for
    one or more flash page sizes.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Flash start address for operation. It shall be
                                Flash page size aligned address.
    @param[in]      aBuf        Pointer to input data buffer
    @param[in]      aLen        Length in bytes. It shall be aligned to Flash
                                page size.

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK              Flash operation successfull
    @retval     #BL_BCM_ERR_INVAL_PARAMS    (aID is invalid) or
                                            (aLen is zero) or
                                            (aAddr + aLen > Total flash size) or
                                            (aBuf is NULL) or
                                            (aAddr unaligned to flash page boundary) or
                                            aLen is not multiple of flash page size
    @retval     #BL_BCM_ERR_UNINIT          API called in uninitialized driver state

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_WRITE_PROC  #BRCM_REQ_FLASH_WRITE

    @limitations None
*/
extern int32_t BL_FLASH_Write(BL_FLASH_HwIDType aID,
                              BL_FLASH_AddrType aAddr,
                              const uint8_t *const aBuf,
                              BL_FLASH_LenType aLen);

/** @brief Flash read

    API to read flash from aAddr for aLen bytes.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID
    @param[in]      aAddr       Flash address
    @param[out]     aBuf        Pointer to output data buffer
    @param[in]      aLen        Length in bytes

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK              Flash operation successfull
    @retval     #BL_BCM_ERR_INVAL_PARAMS    (aID is invalid) or
                                            (aLen is zero) or
                                            (aAddr + aLen > Total flash size) or
                                            (aBuf is NULL)
    @retval     #BL_BCM_ERR_UNINIT          API called in uninitialized driver state

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_READ_PROC  #BRCM_REQ_FLASH_READ

    @limitations None
*/
extern int32_t BL_FLASH_Read(BL_FLASH_HwIDType aID,
                             BL_FLASH_AddrType aAddr,
                             uint8_t *const aBuf,
                             BL_FLASH_LenType aLen);

/** @brief Flash Read ID

    API to read manufacturer and device ID.
    For manufacturer ID and device ID of the specific flash device, refer to
    flash device data sheet.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aID         Controller ID

    @retval     FLASH_FlsIDType Flash Manufacturer and device ID

    @post None

    @trace  #BRCM_ARCH_BL_FLASH_READ_ID_PROC  #BRCM_REQ_FLASH_READ_ID

    @limitations None
*/
extern BL_FLASH_FlsIDType BL_FLASH_ReadID(BL_FLASH_HwIDType aID);

#endif /* BL_FLASH_H */

/** @} */
