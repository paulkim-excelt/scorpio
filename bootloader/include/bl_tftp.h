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
    @defgroup grp_bl_tftp TFTP Downloader
    @ingroup grp_bl_downloader

    @addtogroup grp_bl_tftp
    @{
    @file bl_tftp.h
    @brief TFTP APIs

    @version 0.30 Imported from docx
*/

#ifndef BL_TFTP_H
#define BL_TFTP_H

#include <stdint.h>
#include <bl_udp.h>

/**
    @name TFTP API IDs
    @{
    @brief API IDs for TFTP
*/
#define BRCM_ARCH_BL_TFTP_INIT_PROC         (0x00U) /**< @brief #BL_TFTP_Init */
#define BRCM_ARCH_BL_TFTP_RX_DATA_IND_PROC  (0x01U) /**< @brief #BL_TFTP_RxDataInd */
#define BRCM_ARCH_BL_TFTP_END_IND_PROC      (0x02U) /**< @brief #BL_TFTP_EndInd */
#define BRCM_ARCH_BL_TFTP_START_IND_PROC    (0x03U) /**< @brief #BL_TFTP_StartInd */
/** @} */

#define UINT uint32_t
#define USHORT uint16_t
#define TFTP_DEBUG

#define BL_TFTP_TIMER_INTERVAL             (4000UL)

/* TFTP Payload ID for binding with Endpoints */
#define BL_TFTP_PAYLOAD_RCV_REQ            (0x54465252UL)
#define BL_TFTP_PAYLOAD_RCV_FILE           (0x54465246UL)

/* The number of timeouts before a connection is Killed */
#define BL_TFTP_MAX_TIMEOUTS               (5UL)

/* Connection state */
typedef uint16_t BL_TFTP_StateType;
#define BL_TFTP_STATE_NONE                 (0U)
#define BL_TFTP_STATE_RRQ                  (1U)
#define BL_TFTP_STATE_WRQ                  (2U)
#define BL_TFTP_STATE_DALLY                (3U)

/* TFTP error codes */
typedef uint16_t BL_TFTP_ErrType;
#define BL_TFTP_ERR_NOT_DEFINED            (0U)
#define BL_TFTP_ERR_FILE_NOT_FOUND         (1U)
#define BL_TFTP_ERR_ACCESS_VIOLATION       (2U)
#define BL_TFTP_ERR_DISK_FULL_ALLOC_EXCEED (3U)
#define BL_TFTP_ERR_ILLEGAL_OPERATION      (4U)
#define BL_TFTP_ERR_UNKNOWN_TRNSFR_ID      (5U)
#define BL_TFTP_ERR_FILE_ALRDY_EXISTS      (6U)
#define BL_TFTP_ERR_NO_SUCH_USER           (7U)

#define BL_TFTP_ERR_MSG_MAX_LEN            (128)
#define BL_TFTP_START_BLOCK	            (1U)
#define BL_TFTP_FILE_NAME_OFFSET           (2)
#define BL_TFTP_BLOCK_NUM_OFFSET           (2)

/* TFTP opcodes */
typedef uint16_t BL_TFTP_OpcodeType;
#define BL_TFTP_OP_RRQ                     (1U)
#define BL_TFTP_OP_WRQ                     (2U)
#define BL_TFTP_OP_DATA                    (3U)
#define BL_TFTP_OP_ACK                     (4U)
#define BL_TFTP_OP_ERROR                   (5U)

/* Parameters for getRandPort() */
#define BL_LOWEST_PORT                     (2000U)
#define BL_NUM_PORTS                       (1024U)

/* Misc TFTP defines */
#define BL_TFTP_PKT_SIZE                   (512UL)
#define BL_TFTP_DATA_HDR_SIZE              (4)
#define BL_TFTP_LISTEN_PORT                (69)
#define BL_TFTP_ACK_PKT_SIZE               (4)
#define BL_TFTP_ERR_PKT_SIZE               (5UL)

typedef struct {
  uint16_t blockNo;
  BL_TFTP_StateType state;
  /* Number of timeouts that have occurred (used to kill connection). */
  uint32_t timeoutCnt;
  uint32_t enableTimeOut;
  BL_INET_IPAddress rmtIP;
  BL_INET_Port rmtPort;
  uint32_t reTransmit;
} BL_TFTP_CompType;

/** @brief This API gives an RX data indication to downloader

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aBuf    data buffer
    @param[in]      aSize   size of the buffer

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successful
    @retval     #BL_BCM_ERR_INVAL_PARAMS   aBuf is NULL

    @post None

    @trace  #BRCM_ARCH_BL_TFTP_RX_DATA_IND_PROC  #BRCM_REQ_BL_TFTP_RX_DATA_IND

    @limitations None
*/
extern int32_t BL_TFTP_RxDataInd(const void *const aBuf,
                                 uint32_t aSize);

/** @brief This API gives an TFTP transfer end indication to downloader

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIsErr  flag to indicate when connection is lost

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_TFTP_END_IND_PROC  #BRCM_REQ_BL_TFTP_END_IND

    @limitations None
*/
extern void BL_TFTP_EndInd(uint32_t aIsErr);

/** @brief This API gives a start indication to downloader when a request is received from TFTP client

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aRmtIP  IP of the TFTP client from which request is received

    Return values are documented in reverse-chronological order
    @retval     #BL_BCM_ERR_OK             Successful
    @retval     #BL_BCM_ERR_BUSY           Downloader is busy

    @post None

    @trace  #BRCM_ARCH_BL_TFTP_START_IND_PROC  #BRCM_REQ_BL_TFTP_START_IND

    @limitations None
*/
extern int32_t BL_TFTP_StartInd(BL_INET_IPAddress aRmtIP);

/** @brief Initialize the TFTP framework

    API to initialize TFTP framework. Ethernet stack is assumed to initialized before TFTP_Init.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_TFTP_INIT_PROC  #BRCM_REQ_BL_TFTP_INIT

    @limitations None
*/
extern void BL_TFTP_Init(void);

extern void BL_TFTP_StateHandler(void);

extern void BL_TFTP_RecvRequestCb(const uint8_t* const aBuf,
                                  uint16_t aLen,
                                  BL_INET_IPAddress aSrcIPAddr,
                                  BL_INET_Port aSrcPort);

extern void BL_TFTP_RecvFileCb(const uint8_t* const aBuf,
                               uint16_t aLen,
                               BL_INET_IPAddress aSrcIPAddr,
                               BL_INET_Port aSrcPort);
#endif /* BL_TFTP_H */

/** @} */
