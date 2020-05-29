/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_otpdrv_ifc Interface
    @ingroup grp_otpdrv

    @addtogroup grp_otpdrv_ifc
    @{
    @section sec_otp_overview Overview
    @todo image html otp_overview.jpg "OTP driver overview"
    OTP driver manages OTP controller in the system.

    OTP driver supports following operations on the OTP controller:
    - OTP read
    - OTP write

    @section sec_otp_fsm State Machine
    This section describes driver state machines for status transition during
    otp driver operation.
    @todo image html otp_state_machine.jpg "OTP driver state machine"
    - #OTP_DRVSTATE_UNINIT: OTP driver uninitialized or de-initialized after
      initialization.
    - #OTP_DRVSTATE_INIT: OTP driver is initialized

    @section sec_otp_seq Sequence Diagrams
    This section describes sequence diagrams for APIs to better understand OTP
    driver interface. Sequence diagrams are based on synchronous behavior of
    the interface.

    @subsection subsec_otp_use_case_seq OTP Driver Use Case sequence
    The following sequence diagram depicts the typical use case sequence for
    OTP APIs.

    @todo image html otp_seq_use_case.jpg "OTP use case sequence"
    @section sec_otp_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_otp.dep

    @limitations None

    @file otp.h
    @brief Interfaces for OTP driver
    This header file  defines interface for OTP driver.

    @version 1.31 Imported from docx
*/

#ifndef OTP_H
#define OTP_H

/**
    @name OTP driver API IDs
    @{
    @brief API IDs for OTP drier
 */
#define BRCM_SWARCH_OTP_HWID_TYPE               (0x00U) /**< @brief #OTP_HwIDType */
#define BRCM_SWARCH_OTP_INIT_PROC               (0x01U) /**< @brief #OTP_Init */
#define BRCM_SWARCH_OTP_READ_PROC               (0x02U) /**< @brief #OTP_Read */
#define BRCM_SWARCH_OTP_WRITE_PROC              (0x03U) /**< @brief #OTP_Write */
/** @} */

/**
    @brief OTP controller ID type

    @trace #BRCM_SWREQ_OTP_KERNEL_INTERFACE
 */
typedef uint32_t OTP_HwIDType;  /**< @brief OTP controller index type */

/** @brief Initialise OTP
 *
 * Intialise OTP driver
 *
 * @behavior Sync, Non re-entrant
 *
 * @pre TBD
 *
 * @param[in]   aID         Index of the OTP controller
 *
 * Return values are documented in reverse-chronological order
 *
 * @retval      #BCM_ERR_OK              OTP read was successful
 * @retval      #BCM_ERR_INVAL_PARAMS    aID is not valid
 * @retval      #BCM_ERR_INVAL_STATE     Invalid driver state
 *
 * @post API to initialize OTP driver. Up on successful initialization,
 * OTP driver state changes to #OTP_DRVSTATE_UNINIT
 *
 * @trace #BRCM_SWREQ_OTP_INIT
 *
 * @limitations None
 *
 */
int32_t OTP_Init(OTP_HwIDType aID);

/** @brief Read an OTP row
 *
 * Interface to read an OTP row
 *
 * @behavior Sync, Non re-entrant
 *
 * @pre TBD
 *
 * @param[in]       aID         Index of the OTP controller
 * @param[in]       aAddr       OTP address (row number) to read
 * @param[inout]    aData       Pointer to the memory where data will be copied
 *
 * Return values are documented in reverse-chronological order
 *
 * @retval      #BCM_ERR_OK              OTP read was successful
 * @retval      #BCM_ERR_INVAL_PARAMS    aID is not valid
 * @retval      #BCM_ERR_INVAL_PARAMS    aAddr is not within the range
 * @retval      #BCM_ERR_INVAL_PARAMS    aData is NULL
 * @retval      #BCM_ERR_UNINIT          OTP driver in uninitialised state
 * @retval      #BCM_ERR_TIME_OUT        timeout occurred while sending OTP command to controller
 * @retval      #BCM_ERR_DATA_INTEG      ECC error detected while reading the OTP row
 *
 * @post TBD
 *
 * @trace #BRCM_SWREQ_OTP_OPERATIONS
 *
 * @limitations None
 *
 */
int32_t OTP_Read(OTP_HwIDType aID,
                    uint32_t aAddr,
                    uint32_t *const aData);

/** @brief Write an OTP row
 *
 * Interface to write an OTP row
 *
 * @behavior Sync, None re-entrant
 *
 * @pre TBD
 *
 * @param[in]   aID         Index of the OTP controller
 * @param[in]   aAddr       OTP address (row number) to write
 * @param[in]   aData       Data to be written to the OTP address
 *
 * Return values are documented in reverse-chronological order
 *
 * @retval      #BCM_ERR_OK              OTP written successfully
 * @retval      #BCM_ERR_INVAL_PARAMS    aID is not valid
 * @retval      #BCM_ERR_INVAL_PARAMS    aAddr is not within the range
 * @retval      #BCM_ERR_INVAL_PARAMS    aData value bit-width greater than
 *                                       OTP row bit-width. Refer @sec_ref_trm
 *                                       for exact OTP row bit-width.
 * @retval      #BCM_ERR_UNINIT          OTP driver in uninitialised state
 * @retval      #BCM_ERR_TIME_OUT        Timeout occurred while sending OTP command to controller
 * @retval      #BCM_ERR_UNKNOWN         Unknown error occurred while sending OTP command to controller
 * @retval      #BCM_ERR_NOSUPPORT       Write is not supported
 *
 * @post TBD
 *
 * @trace #BRCM_SWREQ_OTP_OPERATIONS
 *
 * @limitations Currently OTP_Write is not supported by driver.
 *
 */
int32_t OTP_Write(OTP_HwIDType aID,
                     uint32_t aAddr,
                     uint32_t aData);

#endif /* OTP_H */

/** @} */