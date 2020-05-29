/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_ipc_il
    @{

    @file ipc_osil.c
    @brief IPC driver Integration
    This source file contains the integration of IPC driver to system
    @version 0.20 Imported from docx
*/

#include <board.h>
#include <ipc.h>
#include <ipc_osil.h>
#include "ee.h"

#define IPC_ULOG_ID             (BCM_IPC_ID & BCM_LOGMASK_USER)
#define IPC_Osil_UErr(api, err, val0, val1, val2, val3)   \
                    IPC_Osil_Err(IPC_ULOG_ID, api, err, val0, val1, val2, val3)

#define IPC_KLOG_ID             (BCM_IPC_ID | BCM_LOGMASK_KERNEL)
#define IPC_Osil_KErr(api, err, val0, val1, val2, val3)   \
                    IPC_Osil_Err(IPC_KLOG_ID, api, err, val0, val1, val2, val3)


/******************************************************************************/

static void IPC_Osil_Err(uint16_t aCompID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3);

static void IPC_Osil_Err(uint16_t aCompID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @code{.c}
    Build SVC request
    ret = SVC_Request(&sysReqIO)
    if ret is not BCM_ERR_OK
        ret = sysReqIO.response;
    else if sysReqIO.response is not BCM_ERR_OK
        ret = sysReqIO.response
    else
        ret = aIO->retVal

    return ret
    @endcode
*/
int32_t IPC_SysCmdReq(const uint32_t aCmd, IPCIO *aIO)
{
    int32_t ret;
    SVC_RequestType sysReqIO;


    /* Build SVC command struct */
    sysReqIO.sysReqID = SVC_IPC_ID;
    sysReqIO.magicID = SVC_MAGIC_IPC_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t *)aIO;
    sysReqIO.response = BCM_ERR_UNKNOWN;

    ret = SVC_Request(&sysReqIO);
    if (BCM_ERR_OK != ret) {
        ret = BCM_ERR_UNKNOWN;
    } else if (BCM_ERR_OK != sysReqIO.response) {
        ret =  sysReqIO.response;
    } else {
        ret = aIO->status;
    }
    return ret;
}
//! [Usage of IPCDrv_IRQHandler]
ISR2(IPC_IRQChann0Handler)
{
    StatusType status;

    IPC_IRQHandler(0UL);

    status = SetEvent(IPC_SERVER_TASK, IPC_CHANN0_EVENT);
    if (status != E_OK) {
        IPC_Osil_KErr(BRCM_ARCH_IPC_IL_IRQ_HANDLER_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, (uint32_t)status, 0UL, 0UL);
    }
}
//! [Usage of IPCDrv_IRQHandler]

#if defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR)
void IPC_IRQChann1Handler (void)
{
    StatusType status;

    status = SetEvent(RPC_ServerTask, IPC_CHANN1_EVENT);
    if (status != E_OK) {
        IPC_Osil_KErr(BRCM_ARCH_IPC_IL_IRQ_HANDLER_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, (uint32_t)status, 0UL, 0UL);
    }
}
#endif /* defined(ENABLE_IPC) && defined(ENABLE_IPC_S2M_INTR) */
/** @} */
