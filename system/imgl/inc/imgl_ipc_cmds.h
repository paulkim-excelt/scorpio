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
    @defgroup grp_imgl_ipc_cmds_ifc IPC Command Interface
    @ingroup grp_imgl

    @addtogroup grp_imgl_ifc
    @{

    @file imgl_ipc_cmds.h
    @brief Image Loader IPC Commands

    @version 0.1 Initial Version
*/

#ifndef IMGL_IPC_CMDS_H
#define IMGL_IPC_CMDS_H

#include <stdint.h>
#include <stdlib.h>

/**
    @name Image Loader Architecture IDs
    @{
    @brief Architecture IDs for Image Loader
*/
#define BRCM_SWARCH_IMGL_CMDID_TYPE         (0x10U)   /**< @brief #IMGL_CmdIdType       */
#define BRCM_SWARCH_IMGL_IMGHDR_TYPE        (0x11U)   /**< @brief #IMGL_ImgHdrType      */
/** @} */


/**
    @name Image Loader Command IDs
    @{
    @brief Image Loader Command IDs

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
*/
typedef uint8_t IMGL_CmdIdType;
#define IMGL_CMDID_PING                     (0x01U)
#define IMGL_CMDID_DWNLD_IMG_HDR            (0x02U)
#define IMGL_CMDID_DWNLD_IMG                (0x03U)
#define IMGL_CMDID_IMAGE_FETCH              (0x10U)
/** @} */

#pragma pack(push, 4)

/**
    @brief Image header identifying an image

    @trace #BRCM_SWREQ_IMGL_ABSTRACTION
 */
typedef struct _IMGL_ImgHdrType {
    uint8_t  isImgHdr;  /**< @brief set to TRUE if image is only entires and optional signature */
    uint8_t  pid;       /**< @brief Partition ID                                                */
    uint16_t imgID;     /**< @brief Image ID                                                    */
    uint32_t imgSize;   /**< @brief Image Size                                                  */
} IMGL_ImgHdrType;

#pragma pack(pop)


#endif /* IMGL_IPC_CMDS_H */
/** @} */
