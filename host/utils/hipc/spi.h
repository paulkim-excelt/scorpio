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

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

/** @brief SPI init

    This API is used to initialise SPI interface

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      dev_id          Device identification
    @param[in]      speed_hz        Desired frequency of operation

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_NODEV          No device
    @return     #BCM_ERR_INVAL_STATE    Not in a state to initalise
    @return     #BCM_ERR_INVAL_PARAMS   Invalid speed_hz

    @post None

    @limitations None
*/
extern int32_t SPI_Init(uint32_t dev_id, uint32_t speed_hz);

/** @brief SPI Deinit

    This API is used to de-initialise SPI interface

    @behavior Sync, Non Re-entrant

    @pre None

    @param None

    @return None

    @post None

    @limitations None
*/
extern void SPI_Deinit(void);

/** @brief SPI Read

    This API is used to send data over SPI bus interface and read response

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      dev_id          Device identification
    @param[in]      wr_buf          Pointer to write data buffer
    @param[in]      wr_len          Length of the write buffer in bytes
    @param[out]     rd_buf          Pointer to read data buffer
    @param[in]      rd_len          Length of the read buffer in bytes

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   dev_id in invalid
    @return     #BCM_ERR_INVAL_PARAMS   wr_buf in invalid
    @return     #BCM_ERR_INVAL_PARAMS   wr_len is invalid
    @return     #BCM_ERR_INVAL_PARAMS   rd_buf in invalid
    @return     #BCM_ERR_INVAL_PARAMS   rd_len is invalid

    @post None

    @limitations None
*/
extern int32_t SPI_Read(int32_t dev_id, uint8_t *wr_buf, int32_t wr_len, uint8_t *rd_buf, int32_t rd_len);

/** @brief SPI Write

    This API is used to send data over SPI bus interface

    @behavior Sync, Non Re-entrant

    @pre None

    @param[in]      dev_id          Device identification
    @param[in]      buf             Pointer to data buffer
    @param[in]      len             Length of the buffer in bytes

    @return     #BCM_ERR_OK             Success
    @return     #BCM_ERR_INVAL_PARAMS   dev_id in invalid
    @return     #BCM_ERR_INVAL_PARAMS   buf in invalid
    @return     #BCM_ERR_INVAL_PARAMS   len is invalid

    @post None

    @limitations None
*/
extern int32_t SPI_Write(int32_t dev_id, uint8_t *buf, int32_t len);

#endif /* SPI_H */
