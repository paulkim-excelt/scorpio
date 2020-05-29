/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_nvm_pt Partition Table
    @ingroup grp_nvm

    @addtogroup grp_nvm_pt
    @{
    @section sec_nvm_pt_overview Overview
    @image html partition_table.png Flash Partition Layout
    Above figure describes a typical layout in flash with Partition Table (four copies always), three Bootloader
    Partitions, three Firmware Partitions and two User Data Partitions. On the left, Flash layout with incrementing
    address going down is described. On the right, an expanded view of Partition Table and a typical Partition layout is
    presented.

    @subsection subsec_pt_ptable Partition Table
    There are four copies of Partition Table located at the beginning of Flash. Partition Table contains one Partition
    Table Header (#PTBL_HdrType) followed by a maximum of Eleven Partition Table Entries (#PTBL_EntryType). Each PT copy is
    256 bytes (36 bytes of PT Header and 11 x 20 bytes of PT Entry). All four copies fit into first four pages of the
    Flash.
    <BR>Content of all PT Entries together can be validated with a CRC field in PT Header. However, PT Header is not
    validated with any CRC (only magic in PT Header is validated). The “magicNumber” field in PT Header is validated
    before using it. The “errDetCode” in PT Header is validated with PT Entries before using PT Entries. Each valid PT
    Entry points to a unique partition in flash.
    @subsection subsec_pt_ptable Image Partition
    Each Partition pointed by PT Entry contains Image Header, ‘N’ Image Entries and ‘N’ Images. Images can be left blank
    if it needs to be updated by Bootloader/Firmware. For BootROM to load and execute Bootloader/Firmware, Executable
    binary (bin extracted from ELF) shall be placed in the first Image Entry.
    Image Header must be located (in Flash) at the address specified in “addr” field (Flash offset) in #PTBL_EntryType.
    Image Entries must immediately follow Image Header. Images need not be located immediately after last Image Entry.
    <BR>Each Image can be validated with a CRC field present in Image Entry. All Image Entries together can be validated
    with CRC field present in Image Header. However, Image Header cannot be validated with CRC (Only magic is checked in
    Image Header). BootROM validates Image Header, Image Entries and executable image before using it.
    <BR>For Flash Sector/Subsector alignment requirements, refer to Service Update Process.
    @subsection subsec_pt_blpart Bootloader Partition
    Bootloader (Code+Data) binary shall be the only entry in the Bootloader partitions.
    @subsection subsec_syscfg_part System Config Partition
    All System configuration images are placed in this Partition. This partition is shared between Bootloader and Firmware.
    This partition also contains Partition Table Backup Copy. In case of failed service update, PT Backup Copy helps to
    recover Image Header and Entries of different partitions in Flash. Refer to Service Update Process for safely
    updating this partition.
    @subsection subsec_fw_part Firmware Partition
    Firmware executable images are placed in the first set of Image Entries of Firmware Partitions. All relevant
    configurations needed for a safe execution of application/usecase must be placed in this Partition. Refer to Service
    Update Process for safely updating this partition.
    @subsection subsec_ud_part Userdata Partition
    Design limits Firmware to write only to this partition. User profile information, auxiliary information, and other
    miscellaneous information that is not critical for basic application/usecase can be placed here. For example,
    changes in Propagation delay on BroadR-Reach can be preserved here. This partition is not marked for read-only
    access in PT Entry.
    @subsection subsec_crash_part Crash Partition
    This partition is used by Bootloader to preserve critical memory regions and CPU status for debugging later.
    Bootloader shall look for an empty image entry (actualSize = 0 in Image Entry), update the partition with crash
    logs/dumps for debugging.

    @limitation All these structure are supposed to be 4-byte aligned

    @file nvm_pt.h
    @brief Partition table interface
    This file defines interface for Partition table
    @version 1.0 Import from SADS
*/

#ifndef NVM_PT_H
#define NVM_PT_H

#include <stdint.h>

/**
    @name Partition Table & Image Table Architecture IDs
    @{
    @brief Architecture IDs for Partition & Image Tables

*/
#define BRCM_SWARCH_PTBL_ID_TYPE        (0x00U)   /**< @brief #PTBL_IdType          */
#define BRCM_SWARCH_PTBL_MACRO          (0x01U)   /**< @brief #PTBL_MAGIC           */
#define BRCM_SWARCH_PTBL_ERR_DET_MACRO  (0x02U)   /**< @brief #PTBL_ERR_DET_NOCODE  */
#define BRCM_SWARCH_PTBL_HDR_TYPE       (0x03U)   /**< @brief #PTBL_HdrType         */
#define BRCM_SWARCH_PTBL_ENTRY_TYPE     (0x04U)   /**< @brief #PTBL_EntryType       */
#define BRCM_SWARCH_PTBL_TYPE           (0x05U)   /**< @brief #PTBL_Type            */
#define BRCM_SWARCH_ITBL_MACRO          (0x06U)   /**< @brief #ITBL_MAGIC           */
#define BRCM_SWARCH_ITBL_ERR_DET_MACRO  (0x07U)   /**< @brief #ITBL_ERR_DET_HDR_MASK*/
#define BRCM_SWARCH_ITBL_HDR_TYPE       (0x08U)   /**< @brief #ITBL_HdrType         */
#define BRCM_SWARCH_ITBL_ENTRY_TYPE     (0x09U)   /**< @brief #ITBL_EntryType       */
#define BRCM_SWARCH_ITBL_TYPE           (0x0AU)   /**< @brief #ITBL_Type            */
/** @} */

/**
    @name Partition IDs
    @{
    @brief Partition ID

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
typedef uint16_t PTBL_IdType;               /**< @brief 16-bit Partition ID     */
#define PTBL_ID_BL              (1U)        /**< @brief Bootloader Partition ID */
#define PTBL_ID_SYSCFG          (3U)        /**< @brief SysCfg Partition ID     */
#define PTBL_ID_FW              (5U)        /**< @brief Firmware Partition ID   */
#define PTBL_ID_CRASH_DUMP      (7U)        /**< @brief Crash Partition ID      */
#define PTBL_ID_SECURE_KEY      (8U)        /**< @brief Secure Key Partition ID */
#define PTBL_ID_USER_DATA       (9U)        /**< @brief User Data Partition ID  */
#define PTBL_ID_TYPE_MASK       (0xFFU)     /**< @brief Partition ID mask       */
#define PTBL_ID_TYPE_SHIFT      (0U)        /**< @brief Partition ID shift      */
#define PTBL_ID_COPY_MASK       (0xF00UL)   /**< @brief Partition ID Copy mask  */
#define PTBL_ID_COPY_SHIFT      (8UL)       /**< @brief Partition ID Copy shift */
#define PTBL_ID_SKIP_MASK       (0x01U)     /**< @brief Partition ID skip mask  */
#define PTBL_ID_SKIP_SHIFT      (0U)        /**< @brief Partition ID skip shift */
/** @} */


/**
    @name Partition Table related Macros
    @{
    @brief Partition Table related Macros

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
#define PTBL_MAGIC            (0x5054424CUL)        /**< @brief PT magic number 'P''T''B''L' */
#define PTBL_VERSION          (0x00000001UL)        /**< @brief PT format version */
#define PTBL_HDR_SIZE    (sizeof(PTBL_HdrType))     /**< @brief PT header struct size */
#define PTBL_ENTRY_SIZE  (sizeof(PTBL_EntryType))   /**< @brief PT entry struct size */
#define PTBL_SIZE        (sizeof(PTBL_Type))        /**< @brief Parition Table (PT) size (equal to 256,
                                                        same as FLASH_PAGE_SIZE for most of the flashes */
#define PTBL_MAX_NUM_ENTRIES  (11UL)                /**< @brief Max Number of entries in Parition Table */
#define PTBL_NUM_COPIES       (4UL)                 /**< @brief Number of PT copies in RAM */
#define PTBL_MAX_NUM_COPIES   (3UL)                 /**< @brief Max Number of PT entries for same partition copy */
#define PTBL_CRC32_POLY       (0x04C11DB7UL)        /**< @brief CRC polynomial used for generating
                                                        partition table related CRCs */
/** @} */

/**
    @name Partition Table Error Detection Code types
    @{
    @brief Partition Table Error Detection Code Macros

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
#define PTBL_ERR_DET_NOCODE         (0UL)   /**< @brief No error detection code         */
#define PTBL_ERR_DET_CRC            (1UL)   /**< @brief Error detection code CRC        */
#define PTBL_ERR_DET_CHECKSUM       (2UL)   /**< @brief Error detection code Checksum   */
#define PTBL_ERR_DET_MASK        (0x07UL)   /**< @brief PT header flag error detection code mask */
#define PTBL_ERR_DET_SHIFT          (0UL)   /**< @brief PT header flag error detection code shift */
/** @} */

/**
    @name Image Table related macros
    @{
    @brief Image Table related macros

    @trace #BRCM_SWREQ_ITBL_LAYOUT
*/
#define ITBL_MAGIC           (0x494D4748UL)         /**< @brief Image magic number 'I''M''G''H' */
#define ITBL_VERSION         (0x00000001UL)         /**< @brief Image Table format verison      */
#define ITBL_HDR_SIZE   (sizeof(ITBL_HdrType))      /**< @brief Image header struct size        */
#define ITBL_ENTRY_SIZE (sizeof(ITBL_EntryType))    /**< @brief Image entry struct size         */
#define ITBL_CRC32_POLY      (0x04C11DB7UL)         /**< @brief CRC polynomial used for generating
                                                        partition table related CRCs */
#define ITBL_ID_EXE_MAX      (0xFU)                 /**< @brief Image ID for the executable image
                                                        ranging from 0 - 0xF */
#define ITBL_SIGNATURE_SIZE   (256UL)               /**< @brief Signature size */
/** @} */

/**
    @name Image Table Error Detection Code types
    @{
    @brief Image Table Error Detection Code Macros

    @trace #BRCM_SWREQ_ITBL_LAYOUT
*/
#define ITBL_ERR_DET_HDR_MASK      (0x07UL)     /**< @brief Image header flag error detection code mask */
#define ITBL_ERR_DET_HDR_SHIFT     (0UL)        /**< @brief Image header flag error detection code shift */
#define ITBL_ERR_DET_ENTRY_MASK    (0x1CUL)     /**< @brief Image entry flag error detection code mask */
#define ITBL_ERR_DET_ENTRY_SHIFT   (2UL)        /**< @brief Image entry flag error detection code shift */
/** @} */

#pragma pack(push, 4)

/**
    @brief Partition Table Header

    @note hdrErrDetCode member is not used by BootROM of all the chipsets (Refer table below).
    Where as hdrErrDetCode is always used by Bootloader and Firmware.
    |  Chipset          | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | BCM8910x          |   Not used by BootROM                             |
    | BCM89531          |   Not used by BootROM                             |
    | BCM89541          |   Not used by BootROM                             |
    | BCM89551          |   Not used by BootROM                             |
    | BCM89559          |   Used by BootROM                                 |
    | BCM89559G         |   Used by BootROM                                 |

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
typedef struct _PTBL_HdrType {
    uint32_t magicNumber;           /**< @brief Must be set to 0x5054424C (ASCII representation of ‘PTBL’) */
    uint32_t version;               /**< @brief Table format version */
    uint64_t errDetCode;            /**< @brief 32-bit CRC (upper 32-bit must be zero) */
    uint32_t flags;                 /**< @brief Bit[0-2] – Type of Error detection mechanism. Must be set to 0x1 */
    uint32_t totalSize;             /**< @brief Must be set to sizeof(PTBL_HdrType) + numEntries * sizeof(PTBL_EntryType) */
    uint32_t loadAddr;              /**< @brief Address in RAM where PT (Hdr + Entries) has to be loaded to.
                                        Must be set to a valid address for BootROM to load it. */
    uint32_t numEntries;            /**< @brief Number of partition entries in PT */
    uint32_t hdrErrDetCode;         /**< @brief PT header error detection code */
} PTBL_HdrType;


/**
    @brief PT entry structure type

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
typedef struct _PTBL_EntryType {
    uint32_t flags;                 /**< @brief Bit[0] – BootRom will skip this PT Entry if set to 1
                                            <BR>Bit[1] – If set, Partition Shall be treated as Read only
                                            [Can be used to notify Factory image, never update]
                                            <BR>Bit[2] – QSPI Dual Lane mode [Used by BootROM)
                                            <BR>    0- Single Lane mode
                                            <BR>    1- Dual Lane mode
                                            <BR> Bootloader shall ignore this bit.
                                            <BR>Other bits are reserved */
    uint32_t addr;                  /**< @brief Absolute Flash Address/Offset where the Partition is located */
    uint32_t maxSize;               /**< @brief Set to Zero. Ignored by BootRom */
    PTBL_IdType pid;                 /**< @brief Partition ID
                                            <BR>Bit[0-7] – Partition Type
                                            <BR>Bit[8-15] – Partition Index
                                                [Valid values: 1,2.. numEntries in PT Header] */
    uint16_t resvd1;                /**< @brief Reserved fields */
    uint32_t resvd2;                /**< @brief Reserved fields */
} PTBL_EntryType;

/**
    @brief Partition Table structure type

    @trace #BRCM_SWREQ_PTBL_LAYOUT
*/
typedef struct _PTBL_Type {
    PTBL_HdrType    hdr;                        /**< @brief Partition Table Header  */
    PTBL_EntryType  entry[PTBL_MAX_NUM_ENTRIES];/**< @brief Partition Table Entries */
} PTBL_Type;


/**
    @brief Image Header

    @note hdrErrDetCode member is not used by BootROM of all the chipsets (Refer table below).
    Where as hdrErrDetCode is always used by Bootloader and Firmware.
    |  Chipset          | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | BCM8910x          |   Not used by BootROM                             |
    | BCM89531          |   Not used by BootROM                             |
    | BCM89541          |   Not used by BootROM                             |
    | BCM89551          |   Not used by BootROM                             |
    | BCM89559          |   Used by BootROM                                 |
    | BCM89559G         |   Used by BootROM                                 |


    @trace #BRCM_SWREQ_ITBL_LAYOUT
*/
typedef struct _ITBL_HdrType {
    uint32_t magicNumber;           /**< @brief Must be set to 0x494D4748 (ASCII representation of ‘IMGH’) */
    uint32_t version;               /**< @brief Table version */
    uint64_t timeStamp;             /**< @brief Build time information */
    uint32_t imgVersion;            /**< @brief Image version */
    uint64_t errDetCode;            /**< @brief 32-bit CRC (upper 32-bit must be zero) */
    uint32_t flags;                 /**< @brief Must be set to 0x1 */
    uint32_t entryPoint;            /**< @brief Address to which BootROM/Bootloader shall execute “Branch to” to execute
                                        Firmware/Bootloader Must be set appropriately. Refer to Image Entry to know
                                        where the executable is loaded to. */
    uint32_t numImgs;               /**< @brief Number of valid Image Entries (or Images, each Image Entry can point to
                                        only one Image) */
    uint32_t hdrErrDetCode;         /**< @brief Image header error detection code */
} ITBL_HdrType;


/**
    @brief Image Entry

    @trace #BRCM_SWREQ_ITBL_LAYOUT
*/
typedef struct _ITBL_EntryType {
    uint32_t flags;                 /**< @brief Bit[0] – Reserved. Must be zero
                                        <BR>Bit[1] – Reserved. Must be zero
                                        <BR>Bit[2-4] – Type of Error detection mechanism. Must be set 0x1.
                                        <BR>Other bits are reserved. Must be set to Zero. */
    uint64_t errDetCode;            /**< @brief 32-bit CRC (upper 32-bit must be zero) */
    uint32_t flashOffset;           /**< @brief Offset within Partition to the beginning of Image.
                                        <BR>For flash offset, add this offset to “addr” field in PT Entry. */
    uint32_t actualSize;            /**< @brief Actual size of image to be loaded.
                                        <BR>(Must be less than or equal to maxSize) */
    uint16_t imgType;               /**< @brief Unique Image ID in the partition.
                                        <BR>Each image is uniquely identified with (pid, imgID) pair */
    uint16_t compType;              /**< @brief Compression type (Unused) */
    uint32_t loadAddr;              /**< @brief Address to which the image needs to be loaded to.
                                        <BR>Used by BootROM to load bootloader/firmware to appropriate address.
                                        <BR>Used by Bootloader to load firmware to appropriate address.
                                        <BR>Ignored by Firmware.
                                        <BR>BootROM needs this address (for Bootloader Partition) to be outside BootROM’s
                                        <BR>memory usage. Refer to TRM [15] for details on BootROM memory usage/map. */
    uint32_t maxSize;               /**< @brief Maximum allowed image size.
                                        <BR>Flash memory shall be reserved for maxSize for this image */
    uint32_t resvd2;                /**< @brief Reserved fields. Used for CRC validation on Image Header by Bootloader */
} ITBL_EntryType;

/**
    @brief Image Table structure type

    @trace #BRCM_SWREQ_ITBL_LAYOUT
*/
typedef struct _ITBL_Type {
    ITBL_HdrType    hdr;                                /**< @brief Image Table Header  */
    ITBL_EntryType  entry[ITBL_MAX_NUM_ENTRIES];        /**< @brief Image Table Entries */
    uint8_t         signature[ITBL_SIGNATURE_SIZE];     /**< @brief Partition signature */
} ITBL_Type;


#pragma pack(pop)

#endif /* NVM_PT_H */
/** @} */
