/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited. All rights reserved.

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
    @defgroup grp_bl_utils_edc EDC
    @ingroup grp_bl_utils

    @addtogroup grp_bl_utils_edc
    @{
    @file bl_edc.h
    @brief Error Detection Code APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_EDC_H
#define BL_EDC_H

/**
    @name Control Software API IDs
    @{
    @brief API IDs for Control Software
*/
#define BRCM_ARCH_BL_EDC_CRC32_POLY_MACRO       (0x00U) /**< @brief #BL_CRC32_POLY */
#define BRCM_ARCH_BL_EDC_CRC_INIT_SEED_MACRO    (0x01U) /**< @brief #BL_EDC_CRC_INIT_SEED */
#define BRCM_ARCH_BL_EDC_VALIDATE_CRC_PROC      (0x02U) /**< @brief #BL_EDC_ValidateCRC */
#define BRCM_ARCH_BL_EDC_CALCULATE_CRC_PROC     (0x03U) /**< @brief #BL_EDC_CalculateCRC */
#define BRCM_ARCH_BL_EDC_VALIDATE_CHCKSM_PROC   (0x04U) /**< @brief #BL_EDC_ValidateChcksm */
/** @} */

/**
    @brief CRC polynomial used for generating partition table related CRCs

    @trace #BRCM_ARCH_BL_EDC_CRC32_POLY_MACRO #BRCM_REQ_BL_EDC_CRC32_POLY
*/
#define BL_CRC32_POLY              (0x04C11DB7UL)

/**
    @brief CRC intial seed value

    @trace #BRCM_ARCH_BL_EDC_CRC_INIT_SEED_MACRO #BRCM_REQ_BL_EDC_CRC_INIT_SEED
*/
#define BL_EDC_CRC_INIT_SEED       (~0x0UL)

/** @brief CRC validation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  aAddr   Starting address from which CRC has to be computed
    @param[in]  aLen    Size in Bytes for which CRC has to be computed
    @param[in]  aCRC    CRC against which the computed CRC has to be validated

    @retval     #BCM_ERR_OK         CRC matches
    @retval     #BCM_ERR_DATA_INTEG Computed CRC is different from aCRC

    @post None

    @trace  #BRCM_ARCH_BL_EDC_VALIDATE_CRC_PROC  #BRCM_REQ_BL_EDC_VALIDATE_CRC

    @limitations None
*/
extern int32_t BL_EDC_ValidateCRC(const uint8_t *const aAddr,
                                  uint32_t aLen,
                                  uint64_t aCRC);

/** @brief CRC calculation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  aAddr       Starting address from which CRC has to be computed
    @param[in]  aLen        Size in Bytes for which CRC has to be computed
    @param[in]  aSeedCRC    Seed value that should be used for current CRC calculation

    @retval     uint32_t    CRC

    @post None

    @trace  #BRCM_ARCH_BL_EDC_CALCULATE_CRC_PROC  #BRCM_REQ_BL_EDC_CALCULATE_CRC

    @limitations None
*/
extern uint32_t BL_EDC_CalculateCRC(const uint8_t *const aAddr,
                                    uint32_t aLen,
                                    uint32_t aSeedCRC);

/** @brief Checksum validation

    @behavior Sync, Re-entrant

    @pre None

    @param[in]  aAddr       Starting address from which Checksum has to be computed
    @param[in]  aLen        Size in Bytes for which Checksum has to be computed
    @param[in]  aChcksm     Checksum against which the computed Checksum has to be validated

    @retval     #BCM_ERR_OK         Checksum matches
    @retval     #BCM_ERR_DATA_INTEG Computed Checksum is different from aChcksm

    @post None

    @trace  #BRCM_ARCH_BL_EDC_VALIDATE_CHCKSM_PROC  #BRCM_REQ_BL_EDC_VALIDATE_CHCKSM

    @limitations None
*/
extern int32_t BL_EDC_ValidateChcksm(uint8_t *const aAddr,
                                     uint32_t aLen,
                                     uint64_t aChcksm);

#endif /* BL_EDC_H */

/** @} */
