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
    @defgroup grp_mdiodrv_ifc Interface
    @ingroup grp_mdiodrv

    @addtogroup grp_mdiodrv_ifc
    @{
    @section sec_mdio_overview Overview
    @image html mdio_overview.jpg "MDIO driver overview"
    TBD

    @section sec_mdiodrv_req Requirements Assumptions
        -# Power-up and reset of MDIO controller is out of the scope of MDIO
           driver. This shall be done by MCU driver.
        -# Configuration of chip pins for MDIO functionality is out of the
           scope of MDIO driver. This shall be done by PINMUX driver.
        -# Configuration of input clock to MDIO controller is out of the scope
           of MDIO driver. This shall be done by MCU driver.

    @section sec_mdio_fsm State Machine
    TBD

    @section sec_mdio_seq Sequence Diagrams
    TBD

    @subsection subsec_mdio_sync_op_seq MDIO Synchronous Operation sequence
    TBD

    @subsection subsec_mdio_async_op_seq MDIO Asynchronous Operation sequence
    TBD

    @subsection subsec_mdio_cancel_op_seq MDIO Cancel Operation sequence
    TBD

    @subsection subsec_mdio_use_case_seq MDIO Driver Use Case sequence
    TBD

    @section sec_mdio_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   NA                                              |

    @includedoc drivers_mdio.dep

    @limitations None

    @file mdio.h
    @brief Interfaces for MDIO driver
    This header file  defines interface for MDIO driver.

    @version 0.1 Initial version
*/

#ifndef MDIO_H
#define MDIO_H

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>

/**
    @name MDIO driver API IDs
    @{
    @brief API IDs for MDIO drier
 */
#define BRCM_SWARCH_MDIO_HW_ID_TYPE             (0x00U) /**< @brief #MDIO_HwIDType */
#define BRCM_SWARCH_MDIO_SLAVEADDR_TYPE         (0x01U) /**< @brief #MDIO_SlaveAddrType */
#define BRCM_SWARCH_MDIO_DEVICE_TYPE            (0x02U) /**< @brief #MDIO_DeviceType */
#define BRCM_SWARCH_MDIO_REGACCESS_TYPE         (0x03U) /**< @brief #MDIO_RegAccessType */
#define BRCM_SWARCH_MDIO_REGADDR_TYPE           (0x04U) /**< @brief #MDIO_RegAddrType */
#define BRCM_SWARCH_MDIO_JOBID_TYPE             (0x05U) /**< @brief #MDIO_JobIDType */
#define BRCM_SWARCH_MDIO_SPEED_TYPE             (0x06U) /**< @brief #MDIO_SpeedType */
#define BRCM_SWARCH_MDIO_CTRLMODE_TYPE          (0x07U) /**< @brief #MDIO_CtlrModeType */
#define BRCM_SWARCH_MDIO_PKT_TYPE               (0x08U) /**< @brief #MDIO_PktType */
#define BRCM_SWARCH_MDIO_CFG_TYPE               (0x09U) /**< @brief #MDIO_CfgType */
#define BRCM_SWARCH_MDIO_INIT_PROC              (0x0AU) /**< @brief #MDIO_Init */
#define BRCM_SWARCH_MDIO_DEINIT_PROC            (0x0BU) /**< @brief #MDIO_DeInit */
#define BRCM_SWARCH_MDIO_READ_PROC              (0x0CU) /**< @brief #MDIO_Read */
#define BRCM_SWARCH_MDIO_WRITE_PROC             (0x0DU) /**< @brief #MDIO_Write */
#define BRCM_SWARCH_MDIO_GETJOBRESULT_PROC      (0x0EU) /**< @brief #MDIO_GetJobResult */
#define BRCM_SWARCH_MDIO_TIMERCMD_TYPE          (0x0FU) /**< @brief #MDIO_TimerCmdType */
#define BRCM_SWARCH_MDIO_GETJOBQFREELEN_PROC    (0x10U) /**< @brief #MDIO_GetJobQFreeLen */
/** @} */

/**
    @name MDIO_HwIDType
    @{
    @brief MDIO controller index type

    @trace #BRCM_SWREQ_MDIO_INIT
 */
typedef uint32_t MDIO_HwIDType;            /**< @brief MDIO controller index type */
/** @} */

/**
    @name MDIO_CtlrModeType
    @{
    @brief MDIO controller mode type

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef uint32_t MDIO_CtlrModeType;
#define MDIO_CTRLMODE_MASTER        (1UL)   /**< @brief MDIO controller master mode */
#define MDIO_CTRLMODE_SLAVE         (2UL)   /**< @brief MDIO controller slave mode */
/** @} */

/**
    @name MDIO_SlaveAddrType
    @{
    @brief MDIO Slave Address Type

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef uint8_t MDIO_SlaveAddrType;
#define MDIO_SLAVEADDR_MASK         (0x1FU) /**< @brief MDIO slave address field mask */
#define MDIO_SLAVEADDR_MAX          (31U)   /**< @brief MDIO slave address max value */
/** @} */

/**
    @name MDIO_DeviceType
    @{
    @brief MDIO Device Type

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef uint8_t MDIO_DeviceType;
#define MDIO_DEVICETYPE_MASK        (0x1FU) /**< @brief MDIO device type field mask */
#define MDIO_DEVICETYPE_MAX         (31U)   /**< @brief MDIO device type max value */
/** @} */

/**
    @name MDIO_RegAccessType
    @{
    @brief MDIO register access type

    @trace #BRCM_SWREQ_MDIO_INIT
 */
typedef uint8_t MDIO_RegAccessType;
#define MDIO_REGACCESS_CL22       (1UL)   /**< @brief MDIO slave CL22 register access */
#define MDIO_REGACCESS_CL45       (2UL)   /**< @brief MDIO slave CL45 register access */
/** @} */

/**
    @name MDIO_RegAddrType
    @{
    @brief MDIO register addres type

    @trace #BRCM_SWREQ_MDIO_INIT
 */
typedef uint16_t MDIO_RegAddrType;
#define MDIO_REGADDR_CL22_MASK     (0x1FUL)    /**< @brief MDIO register address mask for CL22 register access */
#define MDIO_REGADDR_CL45_MASK     (0xFFFFUL) /**< @brief MDIO register address mask for CL45 register access */
/** @} */

/**
    @name MDIO_SpeedType
    @{
    @brief MDIO speed type

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef uint32_t MDIO_SpeedType;    /**< @brief Type definition for mdio speed */
/** @} */

/**
    @name MDIO_JobIDType
    @{
    @brief MDIO job indentifier type

    @trace #BRCM_SWREQ_MDIO_OPERATIONS
*/
typedef uint32_t MDIO_JobIDType;    /**< @brief Type definition for MDIO job identifier */
/** @} */

/**
    @name MDIO_TimerCmdType
    @{
    @brief MDIO timer command type

    Typedef describing command for the periodic timer. This timer is used to
    invoke driver state handler.

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef uint32_t MDIO_TimerCmdType;
#define MDIO_TIMERCMD_NONE     (0UL) /**< @brief No action requied on periodic
                                       timer. i.e. timer remains in current
                                       state */
#define MDIO_TIMERCMD_START    (1UL) /**< @brief Start periodic timer */
#define MDIO_TIMERCMD_STOP     (2UL) /**< @brief Stop periodic timer */
/** @} */

/**
    @brief MDIO transfer packet

    MDIO transfer packet structure.

    @trace #BRCM_SWREQ_MDIO_INIT
*/
typedef struct _MDIO_PktType {
    MDIO_SlaveAddrType slaveAddr;   /**< @brief Device address of the MDIO slave.
                                        This slaveAddr is 5bit address */
    MDIO_DeviceType deviceType;     /**< @brief Device type */
    MDIO_RegAccessType regAccMode;  /**< @brief Slave register access mode type */
    MDIO_RegAddrType regAddr;       /**< @brief Slave register address */
    uint16_t regData;               /**< @brief Data read from slave register
                                          or data to write to slave register */
} MDIO_PktType;


/**
    @brief MDIO driver configuration

    @trace #BRCM_SWREQ_MDIO_INIT
 */
typedef struct _MDIO_CfgType {
    MDIO_CtlrModeType  mode;            /**< @brief MDIO controller mode */
    MDIO_SpeedType     speed;           /**< @brief SPI speed */
} MDIO_CfgType;

/**
    @brief Initialize MDIO driver

    This API initializes MDIO driver.

    @note This API if invoked after successful initialization of MDIO
    controller, it will silently return without reconfiguration. In
    case reconfiguration is required, deinitialise the driver by calling
    #MDIO_DeInit before calling #MDIO_Init.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the controller
    @param[in]      aConfig     Pointer to configuration structure

    Return values are documented in reverse-chronological order
    @return     void

    @post API to initialize MDIO driver. Up on successful initialization,
    MDIO driver state changes to #MDIO_STATUS_IDLE.

    @trace #BRCM_SWREQ_MDIO_INIT

    @limitations None
*/
extern void MDIO_Init(MDIO_HwIDType aID,
                       const MDIO_CfgType *const aConfig);

/** @brief De-initialize MDIO driver

    API to de-initialize mdio driver.
    @note If this API is called during an on-going mdio operation then:
    -# Completion of on-going operation is not guaranteed

    If there are any pending jobs in the driver queue, these jobs would be
    cancelled during de-initialisation of the driver. Client should not wait
    for completion of these jobs.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the controller

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_MDIO_INIT

    @limitations None
*/
extern void MDIO_DeInit(MDIO_HwIDType aID);

/** @brief MDIO read

    Priviledge mode API to queue a job to do slave register read.

    This register read job would be processed by driver asynchronously. Driver
    will notify client on completion of the job. Client should query the job
    result using #MDIO_GetJobResult on receiving a job completion notification.
    On job result query by client, driver would remove the completed job from
    driver job queue.
    This API should be accessed in priviledge software only.

    @behavior Async, Re-entrant

    @pre TBD

    @param[in]      aID         Controller ID
    @param[inout]   aPkt        Pointer to MDIO packet
    @param[out]     aJobID      Pointer to retrieve job identifier for read job.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully initiated mdio read operation
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aPkt is NULL) or
                                        (aJobID is NULL) or
                                        (aPkt->slaveAddr is invalid) or
                                        (aPkt->deviceType is invalid) or
                                        (aPkt->regAccType is invalide) or
                                        (aPkt->regAddr is invalid)
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_NOMEM          Driver job queue is full.

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern int32_t MDIO_Read(MDIO_HwIDType aID,
                          MDIO_PktType *const aPkt,
                          MDIO_JobIDType *const aJobID);

/** @brief MDIO write

    Priviledge mode API to queue a job to do slave register write.

    This register write job would be processed by driver asynchronously. Driver
    will notify client on completion of the job. Client should query the job
    result using #MDIO_GetJobResult on receiving a job completion notification.
    On job result query by client, driver would remove the completed job from
    driver job queue.
    This API should be accessed in priviledge software only.

    @behavior Async, Re-entrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]      aID         Controller ID
    @param[inout]   aPkt        Pointer to MDIO packet
    @param[out]     aJobID      Pointer to retrieve job identifier for write job

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully initiated mdio write operation
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aPkt is Null) or
                                        (aJobID is NULL) or
                                        (aPkt->slaveAddr is invalid) or
                                        (aPkt->deviceType is invalid) or
                                        (aPkt->regAccType is invalide) or
                                        (aPkt->regAddr is invalid)
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_NOMEM          Driver job queue is full.

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern int32_t MDIO_Write(MDIO_HwIDType aID,
                           MDIO_PktType *const aPkt,
                           MDIO_JobIDType *const aJobID);

/** @brief Get MDIO job results

    Priviledge  mode API to get result of a MDIO job result.

    API to query the job result for the job queued to the driver using
    #MDIO_Read and #MDIO_Write APIs. This API should be accessed in priviledge
    software only.

    @note Job result can be queried only once for a completed job.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID         Index of the controller
    @param[in]      aJobID      Job identifier
    @param[inout]   aPkt        Pointer to MDIO packet. aPkt->regData would
                                contain data read from register for a read job.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Job is successfully completed. For a
                                        read job, aPkt->regData contains data
                                        read from slave device register.
    @retval     #BCM_ERR_INVAL_PARAMS   (aID is invalid) or
                                        (aJobID is invalid) or
                                        (aPkt is NULL)
    @retval     #BCM_ERR_UNINIT         API called in uninitialized driver state
    @retval     #BCM_ERR_BUSY           Job is pending or in progress.
    @retval     #BCM_ERR_TIMEOUT        Timeout occured while process the job.
                                        This job and all pending job for this
                                        controller would not completed.

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern int32_t MDIO_GetJobResult(MDIO_HwIDType aID,
                                MDIO_JobIDType aJobID,
                                MDIO_PktType *const aPkt);

/** @brief Get MDIO job queue free length

    Priviledge mode API to retrieve MDIO driver job queue free length.
    So, client can queue upto free length number of MDIO requests.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aID                 Index of the controller

    Return values are documented in reverse-chronological order
    @return     Job queue free length. If aID is invalid, 0UL would be returned

    @post TBD

    @trace #BRCM_SWREQ_MDIO_OPERATIONS

    @limitations None
*/
extern uint32_t MDIO_GetJobQFreeLen(MDIO_HwIDType aID);

#endif /* MDIO_H */
/** @} */
