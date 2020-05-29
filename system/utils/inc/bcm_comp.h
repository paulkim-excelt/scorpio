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
    @defgroup grp_utils_comp BCM Component Identifiers
    @ingroup grp_utils

    @addtogroup grp_utils_comp
    @{
    @section sec_comp_overview Overview
    Architecture defines 16-bit component IDs (for error reporting and 32-bit magic IDs). It also defines SVC numbers
    for drivers and other miscellaneous software components.
    <BR>Architecture limits critical resource access to privileged software only. All hardware register accesses are
    allowed only by privileged software. Architecture also provides memory protection at task level (and hence, to a
    component level, to be implemented). Inter Task communication requires privileged software permissions as the source
    component/requestor component might not have permission to access the destination component/service provider
    component.
    <BR>Architecture defines SVC interface layer and System SVC handler to provide privileged access to different
    software components and Inter Task communication (and Inter component communication).
    <BR>Switching to Privileged mode is performed using SVC instruction in ARM Processor. Each SVC request is mapped to
    a unique 7-bit number (lower 7-bits of 8-bit number used for request and top 1-bit is always set). SVC interface
    layer provides functions for different SVC requests. Details explained in table below.
    <BR>Components can use these identifiers to report errors.

    @file bcm_comp.h
    @brief BCM Component Identifiers
    This header file contains the component identifiers
    @version 1.0 Initial Version
*/


#ifndef BCM_COMP_H
#define BCM_COMP_H

#include <stdint.h>
/**
    @name BCM Component API IDs
    @{
    @brief API IDs for BCM Component APIs
*/
#define BRCM_SWARCH_BCM_TYPE          (0x00U) /**< @brief #BCM_CompIDType */
#define BRCM_SWARCH_BCM_GROUPID_TYPE  (0x01U) /**< @brief #BCM_GroupIDType */
/** @} */

/**
    @name Component IDs
    @{
    @brief 16-bit component IDs for all the components in the system

    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
typedef uint16_t BCM_CompIDType;   /**< @brief 16-bit Unique Component ID for
                                    error reporting       */
#define BCM_RSV_ID (0x0000U)   /**< @brief Reserved       */
#define BCM_MCU_ID (0xB054U)   /**< @brief MCU            */
#define BCM_SPT_ID (0xC9F3U)   /**< @brief SP804          */
#define BCM_CCT_ID (0x8853U)   /**< @brief CCT            */
#define BCM_URT_ID (0xD233U)   /**< @brief UART           */
#define BCM_IIC_ID (0xA102U)   /**< @brief IIC            */
#define BCM_SPI_ID (0xC9E8U)   /**< @brief SPI            */
#define BCM_QSP_ID (0xC24FU)   /**< @brief QSPI           */
#define BCM_WDG_ID (0xD866U)   /**< @brief Watchdog       */
#define BCM_GIO_ID (0x990EU)   /**< @brief GPIO           */
#define BCM_PMX_ID (0xBD97U)   /**< @brief PinMux         */
#define BCM_SIO_ID (0xC90EU)   /**< @brief SDIO           */
#define BCM_PWM_ID (0xBECCU)   /**< @brief PWM            */
#define BCM_ADC_ID (0x8062U)   /**< @brief ADC            */
#define BCM_DMA_ID (0x8D80U)   /**< @brief DMA330         */
#define BCM_OTP_ID (0xBA6FU)   /**< @brief OTP            */
#define BCM_VTM_ID (0xDA8DU)   /**< @brief VTMON          */
#define BCM_PPR_ID (0xBDF1U)   /**< @brief PixelProcessor */
#define BCM_IIS_ID (0xA112U)   /**< @brief I2S            */
#define BCM_UCM_ID (0xD04CU)   /**< @brief UNICAM         */
#define BCM_ISP_ID (0xA24FU)   /**< @brief ISP            */
#define BCM_IPC_ID (0x21E2U)   /**< @brief IPC            */
#define BCM_RPC_ID (0x45E2U)   /**< @brief RPC            */
#define BCM_DDR_ID (0x8C71U)   /**< @brief DDR            */

#define BCM_MFD_ID (0xB0A3U)   /**< @brief MPEGFeeder     */
#define BCM_GFD_ID (0x98A3U)   /**< @brief GraphicsFeeder */
#define BCM_PV0_ID (0xBD15U)   /**< @brief PixelValve     */
#define BCM_CMP_ID (0x898FU)   /**< @brief Compositor     */
#define BCM_BTV_ID (0x8675U)   /**< @brief BT656 Controller */
#define BCM_DSI_ID (0x8E48U)   /**< @brief DSI Controller */
#define BCM_AFE_ID (0x80A4U)   /**< @brief AFE Controller */
#define BCM_MWB_ID (0xB2C1U)   /**< @brief Memory Writeback */
#define BCM_DMS_ID (0x8D92U)   /**< @brief MISC Controller */
#define BCM_VGC_ID (0xD4C2U)   /**< @brief Video & Graphics Composer*/
#define BCM_DIS_ID (0x0D12U)   /**< @brief Display Service */
#define BCM_VRC_ID (0x5622U)   /**< @brief Video Renderer Component */
#define BCM_GRC_ID (0x1A22U)   /**< @brief Graphics Renderer Component */

#define BCM_AVT_ID (0x82B3U)   /**< @brief AVT            */
#define BCM_AMC_ID (0x8182U)   /**< @brief AMAC           */
#define BCM_WLC_ID (0xD962U)   /**< @brief Wireless LAN   */
#define BCM_XVR_ID (0x5EB1U)   /**< @brief XVR            */
#define BCM_SWT_ID (0xCAD3U)   /**< @brief SWITCH         */
#define BCM_CFP_ID (0x88AFU)   /**< @brief CFP            */
#define BCM_CAN_ID (0x880DU)   /**< @brief CAN            */
#define BCM_LIN_ID (0xAD0DU)   /**< @brief LIN            */
#define BCM_MIO_ID (0xB10EU)   /**< @brief MDIO           */
#define BCM_MSG_ID (0xB246U)   /**< @brief MsgQ (IPC)     */
#define BCM_SHL_ID (0xC8EBU)   /**< @brief UART Console   */
#define BCM_UTL_ID (0xD26BU)   /**< @brief UTILS          */

#define BCM_FLM_ID (0x156CU)   /**< @brief Flash Manager  */
#define BCM_PTM_ID (0x3E6CU)   /**< @brief PTM            */
#define BCM_NIF_ID (0x3505U)   /**< @brief NIF            */
#define BCM_SYS_ID (0x4B12U)   /**< @brief SYS            */
#define BCM_COM_ID (0x09CCU)   /**< @brief COM            */

#define BCM_MSF_ID (0x3245U)   /**< @brief Media System & Framework */

#define BCM_CAM_ID (0x080CU)   /**< @brief Camera         */
#define BCM_CFW_ID (0x08B6U)   /**< @brief Camera Fmwk    */
#define BCM_CTN_ID (0x0A6DU)   /**< @brief Camera Tuners  */
#define BCM_CSN_ID (0x0A4DU)   /**< @brief Camera Sensor  */

#define BCM_AVL_ID (0x02ABU)   /**< @brief AVB Library    */
#define BCM_AVB_ID (0x02A1U)   /**< @brief AVB Component  */
#define BCM_ETL_ID (0x126BU)   /**< @brief ETS Library   */
#define BCM_ETS_ID (0x1272U)   /**< @brief ETS Component */
#define BCM_NET_ID (0x21B3U)   /**< @brief Internet Stack */

#define BCM_AVE_ID (0x02A4U)   /**< @brief AVC Encoder Component   */
#define BCM_AVD_ID (0x02A3U)   /**< @brief AVC Decoder Component   */
#define BCM_VEN_ID (0x548DU)   /**< @brief Video Encoder Component */
#define BCM_VDE_ID (0x5464U)   /**< @brief Video Decoder Component */
#define BCM_VXS_ID (0x56F2U)   /**< @brief Openvx Server           */
#define BCM_OPF_ID (0x39E5U)   /**< @brief Opticalflow Component   */
#define BCM_LDC_ID (0x2C62U)   /**< @brief LDC Component           */
#define BCM_STC_ID (0x4A62U)   /**< @brief Stitch Component        */

#define BCM_APY_ID (0x81F8U)   /**< @brief Audio play component   */
#define BCM_ARC_ID (0x8222U)   /**< @brief Audio record component */
#define BCM_RTL_ID (0x466BU)   /**< @brief RTP Library            */
#define BCM_RTS_ID (0x4672U)   /**< @brief RTP Sender Component   */
#define BCM_RTR_ID (0x4671U)   /**< @brief RTP Receiver Component */
#define BCM_MTL_ID (0x326BU)   /**< @brief MPEG2TS Library        */
#define BCM_RFS_ID (0x44B2U)   /**< @brief RFS                    */
#define BCM_OSI_ID (0x3E69U)   /**< @brief OSI                    */
#define BCM_DCA_ID (0x1061U)   /**< @brief DCache                 */

#define BCM_CTL_ID (0x8A6BU)   /**< @brief BL Control             */
/** @} */

/**
    @name Group IDs
    @{
    @brief 6-bit group IDs

    @limitation Do not change the group ID of the IMGL group. This is being
    shared with BOOTROM

    @trace #BRCM_SWREQ_BCM_ERR_REPORT
*/
typedef uint8_t BCM_GroupIDType;
#define BCM_GROUPID_IMGL        (0x00U) /**< @brief Image Loader group  */
#define BCM_GROUPID_SYS         (0x01U) /**< @brief System group        */
#define BCM_GROUPID_NVM         (0x02U) /**< @brief NVM group           */
#define BCM_GROUPID_COMMS       (0x03U) /**< @brief Communications group*/
#define BCM_GROUPID_APP         (0x04U) /**< @brief Application Group   */
#define BCM_GROUPID_CONSOLE     (0x05U) /**< @brief Console Group       */
#define BCM_GROUPID_AVCE        (0x20U) /**< @brief AVCE Group          */
#define BCM_GROUPID_AVCD        (0x21U) /**< @brief AVCD Group          */
#define BCM_GROUPID_CAMERA      (0x22U) /**< @brief Camera Group        */
#define BCM_GROUPID_OPENVX      (0x23U) /**< @brief OPENVX Group        */
#define BCM_GROUPID_LDC         (0x24U) /**< @brief LDC Group           */
#define BCM_GROUPID_DISPLAY     (0x25U) /**< @brief Display Group       */
#define BCM_GROUPID_GRAPHICS    (0x26U) /**< @brief Graphics Group      */
#define BCM_GROUPID_STITCH      (0x27U) /**< @brief Stitch Group        */
#define BCM_GROUPID_AUDIO       (0x28U) /**< @brief Audio Group         */
#define BCM_GROUPID_INVALID     (0xFFU) /**< @brief Invalid group       */
/** @} */

#endif /* BCM_COMP_H */
/** @} */

