/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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

 2. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE COMP_MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_svc Supervisor Call Interface
    @ingroup grp_utils

    @addtogroup grp_svc
    @{

    @file svc.h
    @brief This file contains Supervisor Call Interface functions


    @version 0.1 Initial Version
*/

#ifndef SVC_H
#define SVC_H

/**
    @name SVC Request Architecture IDs
    @{
    @brief Architecture IDs for SVC Requests
*/
#define BRCM_SWARCH_SVC_MACRO         (0x00U)   /**< @brief #SVC_CMD_START      */
#define BRCM_SWARCH_SVC_MAGIC_MACRO   (0x01U)   /**< @brief #SVC_MAGIC_MCU_ID   */
#define BRCM_SWARCH_SVC_REQUEST_TYPE  (0x02U)   /**< @brief #SVC_RequestType    */
#define BRCM_SWARCH_SVC_REQUEST_PROC  (0x03U)   /**< @brief #SVC_Request        */
#define BRCM_SWARCH_SVC_HANDLER_TYPE  (0x04U)   /**< @brief #SVC_ReqHandlerType */
#define BRCM_SWARCH_SVC_HANDLER_PROC  (0x05U)   /**< @brief #SVC_CmdHandler     */
/** @} */

/**
    @name SVC IDs for components
    @{
    @brief SVC number to be used by different components

    @trace #BRCM_SWREQ_SVC_REQUEST
*/
#define SVC_CMD_START   (0x80UL)   /**< @brief SVC start     */
#define SVC_MCU_ID      (0x80UL)   /**< @brief MCU           */
#define SVC_SPT_ID      (0x81UL)   /**< @brief SP804         */
#define SVC_CCT_ID      (0x82UL)   /**< @brief CCT           */
#define SVC_URT_ID      (0x83UL)   /**< @brief UART          */
#define SVC_IIC_ID      (0x84UL)   /**< @brief IIC           */
#define SVC_SPI_ID      (0x85UL)   /**< @brief SPI           */
#define SVC_QSP_ID      (0x86UL)   /**< @brief QSPI          */
#define SVC_WDT_ID      (0x87UL)   /**< @brief Watchdog      */
#define SVC_GIO_ID      (0x88UL)   /**< @brief GPIO          */
#define SVC_PMX_ID      (0x89UL)   /**< @brief PinMux        */
#define SVC_SIO_ID      (0x8AUL)   /**< @brief SDIO          */
#define SVC_PWM_ID      (0x8BUL)   /**< @brief PWM           */
#define SVC_ADC_ID      (0x8CUL)   /**< @brief ADC           */
#define SVC_DMA_ID      (0x8DUL)   /**< @brief DMA330        */
#define SVC_OTP_ID      (0x8EUL)   /**< @brief OTP           */
#define SVC_VTM_ID      (0x8FUL)   /**< @brief VTMON         */
#define SVC_IPC_ID      (0x90UL)   /**< @brief IPC           */
#define SVC_PPR_ID      (0xA0UL)   /**< @brief PixelProcessor*/
#define SVC_IIS_ID      (0xA1UL)   /**< @brief I2S           */
#define SVC_UCM_ID      (0xA3UL)   /**< @brief UNICAM        */
#define SVC_ISP_ID      (0xA4UL)   /**< @brief ISP           */
#define SVC_VGC_ID      (0xA7UL)   /**< @brief VideoGraphicsComposer */
#define SVC_AVT_ID      (0xA9UL)   /**< @brief AVT           */
#define SVC_AMC_ID      (0xB0UL)   /**< @brief AMAC          */
#define SVC_CAN_ID      (0xB1UL)   /**< @brief CAN           */
#define SVC_LIN_ID      (0xB2UL)   /**< @brief LIN           */
#define SVC_MIO_ID      (0xB3UL)   /**< @brief MDIO          */
#define SVC_SWT_ID      (0xB4UL)   /**< @brief SWITCH        */
#define SVC_XVR_ID      (0xB5UL)   /**< @brief XVR           */
#define SVC_ETT_ID      (0xB6UL)   /**< @brief Ethernet time */
#define SVC_MSG_ID      (0xC0UL)   /**< @brief MsgQ (IPC)    */
#define SVC_SHL_ID      (0xC1UL)   /**< @brief Shell         */
#define SVC_LOG_ID      (0xC2UL)   /**< @brief Log           */
#define SVC_MEM_ID      (0xC3UL)   /**< @brief MEMORY        */
#define SVC_OSI_ID      (0xC4UL)   /**< @brief OSIL          */
#define SVC_DCA_ID      (0xC5UL)   /**< @brief DCache        */
#define SVC_CMD_END     (0xCFUL)   /**< @brief SVC end       */
/** @} */

/**
    @name Magic IDs
    @{
    @brief 32-bit magic IDs for all components

    @trace #BRCM_SWREQ_SVC_REQUEST
*/
#define SVC_MAGIC_MCU_ID    (0x4D435530UL)      /**< @brief MCU            */
#define SVC_MAGIC_SPT_ID    (0x53505430UL)      /**< @brief SP804          */
#define SVC_MAGIC_CCT_ID    (0x43435430UL)      /**< @brief CCT            */
#define SVC_MAGIC_URT_ID    (0x55415254UL)      /**< @brief UART           */
#define SVC_MAGIC_IIC_ID    (0x49324330UL)      /**< @brief IIC            */
#define SVC_MAGIC_SPI_ID    (0x53504930UL)      /**< @brief SPI            */
#define SVC_MAGIC_QSP_ID    (0x51535049UL)      /**< @brief QSPI           */
#define SVC_MAGIC_WDT_ID    (0x57444F47UL)      /**< @brief Watchdog       */
#define SVC_MAGIC_GIO_ID    (0x4750494FUL)      /**< @brief GPIO           */
#define SVC_MAGIC_PMX_ID    (0x504D5558UL)      /**< @brief PinMux         */
#define SVC_MAGIC_SIO_ID    (0x5344494FUL)      /**< @brief SDIO           */
#define SVC_MAGIC_PWM_ID    (0x50574D30UL)      /**< @brief PWM            */
#define SVC_MAGIC_ADC_ID    (0x41444330UL)      /**< @brief ADC            */
#define SVC_MAGIC_DMA_ID    (0x444D4143UL)      /**< @brief DMAC           */
#define SVC_MAGIC_OTP_ID    (0x4F545030UL)      /**< @brief OTP            */
#define SVC_MAGIC_VTM_ID    (0x5626544DUL)      /**< @brief VTMON          */
#define SVC_MAGIC_PPR_ID    (0x50695072UL)      /**< @brief PixelProcessor */
#define SVC_MAGIC_IIS_ID    (0x49325330UL)      /**< @brief I2S            */
#define SVC_MAGIC_UCM_ID    (0x55434D30UL)      /**< @brief UNICAM         */
#define SVC_MAGIC_ISP_ID    (0x49535030UL)      /**< @brief ISP            */
#define SVC_MAGIC_VGC_ID    (0x5669436FUL)      /**< @brief VGC            */
#define SVC_MAGIC_AVT_ID    (0x41565430UL)      /**< @brief AVT            */
#define SVC_MAGIC_AMC_ID    (0x414D4143UL)      /**< @brief AMAC           */
#define SVC_MAGIC_CAN_ID    (0x43414E30UL)      /**< @brief CAN            */
#define SVC_MAGIC_LIN_ID    (0x4C494E30UL)      /**< @brief LIN            */
#define SVC_MAGIC_MIO_ID    (0x4D44494FUL)      /**< @brief MDIO           */
#define SVC_MAGIC_SWT_ID    (0x53575448UL)      /**< @brief SWITCH         */
#define SVC_MAGIC_MSG_ID    (0x4D534751UL)      /**< @brief MsgQ (IPC)     */
#define SVC_MAGIC_SHL_ID    (0x5348454CUL)      /**< @brief Shell          */
#define SVC_MAGIC_LOG_ID    (0x554C4F47UL)      /**< @brief Log            */
#define SVC_MAGIC_PTM_ID    (0x50544D30UL)      /**< @brief PTM            */
#define SVC_MAGIC_IPC_ID    (0x49504330UL)      /**< @brief IPC            */
#define SVC_MAGIC_XVR_ID    (0x58565230UL)      /**< @brief XVR            */
#define SVC_MAGIC_ETT_ID    (0x45545430UL)      /**< @brief ETT            */
#define SVC_MAGIC_MEM_ID    (0x444D454DUL)      /**< @brief DBGMEM         */
#define SVC_MAGIC_OSI_ID    (0x4F53494CUL)      /**< @brief OSIL           */
#define SVC_MAGIC_DDR_ID    (0x44445230UL)      /**< @brief DDR            */
#define SVC_MAGIC_DCA_ID    (0x44434143UL)      /**<@brief DCache   */
/** @} */

/**
    @brief System command request

    SVC Request parameters

    @trace #BRCM_SWREQ_SVC_REQUEST
 */
typedef struct _SVC_RequestType {
    uint32_t sysReqID;      /**< @brief 8-bit request ID (LSB of 32-bit)                */
    uint32_t magicID;       /**< @brief 32-bit magic number unique to each component    */
    uint32_t cmd;           /**< @brief 32-bit Command ID specific to component         */
    uint8_t *svcIO;         /**< @brief Request Info (I/O type)                         */
    int32_t response;       /**< @brief 32-bit response updated by System SVC handler   */
} SVC_RequestType;

/** @brief Function pointer for component's SVC handler.

    Handles the command requested by the unpriviledged software and executes in
    the priviledged context. These functions are to be defined by each compoenent
    or driver and hooked on to the SVC.

    @behavior Sync, Non-reentrant

    @pre describe any pre-condition (expectation before calling this function)

    @param[in]      aMagicID    Magic ID of the driver  to whom the SVC handled
                                belongs to.
    @param[in]      aCmd        Command ID specific to component.
    @param[inout]   aRequest    Request I/O type specific to the component

    @return void

    @post None

    @trace  #BRCM_SWREQ_SVC_REQUEST

    @limitations None
*/
typedef void (*SVC_ReqHandlerType)(uint32_t aMagicID, uint32_t aCmd, uint8_t* aRequest);

/** @brief Raise an SVC with sysReqID

    This API raises an SVC with the given sysReqID.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aSvcReq             SVC parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_INVAL_PARAMS   svcRequestID in svcReqIO is not within
                                        the allowed range.

    @post None

    @trace  #BRCM_SWREQ_SVC_REQUEST

    @limitations None
*/
extern int32_t SVC_Request(SVC_RequestType *const aSVCReq);


/** @brief Handle requests raised by @ref SVC_Request

    This API handles SVC Request with the given sysReqID.

    @behavior Sync, Non-reentrant

    @pre CPU shall be in SVC Exception mode

    @param[inout]   aSysReqIO             SVC parameters
    @param[in]      aSysReqID             SVC Request ID

    @return     void

    @post None

    @trace  #BRCM_SWREQ_SVC_REQUEST

    @limitations None
*/
extern void SVC_CmdHandler(SVC_RequestType *aSysReqIO, uint32_t aSysReqID);

#endif /* SVC_H */

/** @} */
