/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_svc_request_impl SVC Request Implementation
    @ingroup grp_svc

    @addtogroup grp_svc_request_impl
    @{

    @file svc_request.c

    @version 0.1 Initial version
*/

#include <stdlib.h>
#include <inttypes.h>
#include <compiler.h>
#include <bcm_err.h>
#include <svc.h>

/**
    @name SVC Request Architecture IDs
    @{
    @brief Architecture IDs for SVC Requests
*/
#define BRCM_SWDSGN_SVC_REQUEST_FUNC_TYPE       (0x90U)   /**< @brief #SVC_RequestFuncType         */
#define BRCM_SWDSGN_SVC_REQUEST_FUNC_ENTRY_PROC (0x91U)   /**< @brief #SVC_REQUEST_FUNC_ENTRY_0x80 */
#define BRCM_SWDSGN_SVC_REQ_FUNC_TABLE_GLOBAL   (0x92U)   /**< @brief #SVC_ReqFuncTbl              */
#define BRCM_SWDSGN_SVC_REQUEST_PROC            (0x93U)   /**< @brief #SVC_Request                 */
/** @} */

/**
    @trace #BRCM_SWREQ_SVC_REQUEST
    @trace #BRCM_SWARCH_SVC_REQUEST_PROC
*/
typedef void (*SVC_RequestFuncType)(const SVC_RequestType* aSysReqIO,
                                    const uint32_t aSysReqID);

/**
    @trace #BRCM_SWREQ_SVC_REQUEST
    @trace #BRCM_SWARCH_SVC_REQUEST_PROC
*/
static void SVC_REQUEST_FUNC_ENTRY_0x80(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x80");
}
static void SVC_REQUEST_FUNC_ENTRY_0x81(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x81");
}
static void SVC_REQUEST_FUNC_ENTRY_0x82(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x82");
}
static void SVC_REQUEST_FUNC_ENTRY_0x83(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x83");
}
static void SVC_REQUEST_FUNC_ENTRY_0x84(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x84");
}
static void SVC_REQUEST_FUNC_ENTRY_0x85(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x85");
}
static void SVC_REQUEST_FUNC_ENTRY_0x86(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x86");
}
static void SVC_REQUEST_FUNC_ENTRY_0x87(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x87");
}
static void SVC_REQUEST_FUNC_ENTRY_0x88(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x88");
}
static void SVC_REQUEST_FUNC_ENTRY_0x89(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x89");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8A(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8A");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8B(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8B");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8C(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8C");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8D(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8D");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8E(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8E");
}
static void SVC_REQUEST_FUNC_ENTRY_0x8F(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x8F");
}
static void SVC_REQUEST_FUNC_ENTRY_0x90(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x90");
}
static void SVC_REQUEST_FUNC_ENTRY_0x91(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x91");
}
static void SVC_REQUEST_FUNC_ENTRY_0x92(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x92");
}
static void SVC_REQUEST_FUNC_ENTRY_0x93(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x93");
}
static void SVC_REQUEST_FUNC_ENTRY_0x94(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x94");
}
static void SVC_REQUEST_FUNC_ENTRY_0x95(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x95");
}
static void SVC_REQUEST_FUNC_ENTRY_0x96(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x96");
}
static void SVC_REQUEST_FUNC_ENTRY_0x97(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x97");
}
static void SVC_REQUEST_FUNC_ENTRY_0x98(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x98");
}
static void SVC_REQUEST_FUNC_ENTRY_0x99(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x99");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9A(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9A");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9B(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9B");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9C(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9C");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9D(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9D");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9E(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9E");
}
static void SVC_REQUEST_FUNC_ENTRY_0x9F(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0x9F");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA0(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA0");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA1(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA1");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA2(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA2");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA3(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA3");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA4(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA4");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA5(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA5");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA6(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA6");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA7(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA7");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA8(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA8");
}
static void SVC_REQUEST_FUNC_ENTRY_0xA9(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xA9");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAA(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAA");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAB(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAB");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAC(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAC");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAD(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAD");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAE(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAE");
}
static void SVC_REQUEST_FUNC_ENTRY_0xAF(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xAF");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB0(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB0");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB1(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB1");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB2(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB2");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB3(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB3");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB4(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB4");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB5(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB5");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB6(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB6");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB7(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB7");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB8(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB8");
}
static void SVC_REQUEST_FUNC_ENTRY_0xB9(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xB9");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBA(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBA");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBB(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBB");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBC(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBC");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBD(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBD");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBE(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBE");
}
static void SVC_REQUEST_FUNC_ENTRY_0xBF(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xBF");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC0(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC0");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC1(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC1");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC2(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC2");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC3(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC3");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC4(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC4");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC5(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC5");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC6(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC6");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC7(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC7");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC8(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC8");
}
static void SVC_REQUEST_FUNC_ENTRY_0xC9(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xC9");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCA(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCA");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCB(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCB");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCC(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCC");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCD(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCD");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCE(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCE");
}
static void SVC_REQUEST_FUNC_ENTRY_0xCF(const SVC_RequestType *aSysReqIO,
                                        const uint32_t aSysReqID)
{
    COMP_ASM("SVC #0xCF");
}
/** @} */

/**
    @trace #BRCM_SWREQ_SVC_REQUEST
    @trace #BRCM_SWARCH_SVC_REQUEST_PROC
*/
static const SVC_RequestFuncType SVC_ReqFuncTbl[(SVC_CMD_END - SVC_CMD_START) + 1UL] =
{
    SVC_REQUEST_FUNC_ENTRY_0x80,
    SVC_REQUEST_FUNC_ENTRY_0x81,
    SVC_REQUEST_FUNC_ENTRY_0x82,
    SVC_REQUEST_FUNC_ENTRY_0x83,
    SVC_REQUEST_FUNC_ENTRY_0x84,
    SVC_REQUEST_FUNC_ENTRY_0x85,
    SVC_REQUEST_FUNC_ENTRY_0x86,
    SVC_REQUEST_FUNC_ENTRY_0x87,
    SVC_REQUEST_FUNC_ENTRY_0x88,
    SVC_REQUEST_FUNC_ENTRY_0x89,
    SVC_REQUEST_FUNC_ENTRY_0x8A,
    SVC_REQUEST_FUNC_ENTRY_0x8B,
    SVC_REQUEST_FUNC_ENTRY_0x8C,
    SVC_REQUEST_FUNC_ENTRY_0x8D,
    SVC_REQUEST_FUNC_ENTRY_0x8E,
    SVC_REQUEST_FUNC_ENTRY_0x8F,
    SVC_REQUEST_FUNC_ENTRY_0x90,
    SVC_REQUEST_FUNC_ENTRY_0x91,
    SVC_REQUEST_FUNC_ENTRY_0x92,
    SVC_REQUEST_FUNC_ENTRY_0x93,
    SVC_REQUEST_FUNC_ENTRY_0x94,
    SVC_REQUEST_FUNC_ENTRY_0x95,
    SVC_REQUEST_FUNC_ENTRY_0x96,
    SVC_REQUEST_FUNC_ENTRY_0x97,
    SVC_REQUEST_FUNC_ENTRY_0x98,
    SVC_REQUEST_FUNC_ENTRY_0x99,
    SVC_REQUEST_FUNC_ENTRY_0x9A,
    SVC_REQUEST_FUNC_ENTRY_0x9B,
    SVC_REQUEST_FUNC_ENTRY_0x9C,
    SVC_REQUEST_FUNC_ENTRY_0x9D,
    SVC_REQUEST_FUNC_ENTRY_0x9E,
    SVC_REQUEST_FUNC_ENTRY_0x9F,
    SVC_REQUEST_FUNC_ENTRY_0xA0,
    SVC_REQUEST_FUNC_ENTRY_0xA1,
    SVC_REQUEST_FUNC_ENTRY_0xA2,
    SVC_REQUEST_FUNC_ENTRY_0xA3,
    SVC_REQUEST_FUNC_ENTRY_0xA4,
    SVC_REQUEST_FUNC_ENTRY_0xA5,
    SVC_REQUEST_FUNC_ENTRY_0xA6,
    SVC_REQUEST_FUNC_ENTRY_0xA7,
    SVC_REQUEST_FUNC_ENTRY_0xA8,
    SVC_REQUEST_FUNC_ENTRY_0xA9,
    SVC_REQUEST_FUNC_ENTRY_0xAA,
    SVC_REQUEST_FUNC_ENTRY_0xAB,
    SVC_REQUEST_FUNC_ENTRY_0xAC,
    SVC_REQUEST_FUNC_ENTRY_0xAD,
    SVC_REQUEST_FUNC_ENTRY_0xAE,
    SVC_REQUEST_FUNC_ENTRY_0xAF,
    SVC_REQUEST_FUNC_ENTRY_0xB0,
    SVC_REQUEST_FUNC_ENTRY_0xB1,
    SVC_REQUEST_FUNC_ENTRY_0xB2,
    SVC_REQUEST_FUNC_ENTRY_0xB3,
    SVC_REQUEST_FUNC_ENTRY_0xB4,
    SVC_REQUEST_FUNC_ENTRY_0xB5,
    SVC_REQUEST_FUNC_ENTRY_0xB6,
    SVC_REQUEST_FUNC_ENTRY_0xB7,
    SVC_REQUEST_FUNC_ENTRY_0xB8,
    SVC_REQUEST_FUNC_ENTRY_0xB9,
    SVC_REQUEST_FUNC_ENTRY_0xBA,
    SVC_REQUEST_FUNC_ENTRY_0xBB,
    SVC_REQUEST_FUNC_ENTRY_0xBC,
    SVC_REQUEST_FUNC_ENTRY_0xBD,
    SVC_REQUEST_FUNC_ENTRY_0xBE,
    SVC_REQUEST_FUNC_ENTRY_0xBF,
    SVC_REQUEST_FUNC_ENTRY_0xC0,
    SVC_REQUEST_FUNC_ENTRY_0xC1,
    SVC_REQUEST_FUNC_ENTRY_0xC2,
    SVC_REQUEST_FUNC_ENTRY_0xC3,
    SVC_REQUEST_FUNC_ENTRY_0xC4,
    SVC_REQUEST_FUNC_ENTRY_0xC5,
    SVC_REQUEST_FUNC_ENTRY_0xC6,
    SVC_REQUEST_FUNC_ENTRY_0xC7,
    SVC_REQUEST_FUNC_ENTRY_0xC8,
    SVC_REQUEST_FUNC_ENTRY_0xC9,
    SVC_REQUEST_FUNC_ENTRY_0xCA,
    SVC_REQUEST_FUNC_ENTRY_0xCB,
    SVC_REQUEST_FUNC_ENTRY_0xCC,
    SVC_REQUEST_FUNC_ENTRY_0xCD,
    SVC_REQUEST_FUNC_ENTRY_0xCE,
    SVC_REQUEST_FUNC_ENTRY_0xCF,
};

/**
    @trace #BRCM_SWREQ_SVC_REQUEST
    @trace #BRCM_SWARCH_SVC_REQUEST_PROC
*/
int32_t SVC_Request(SVC_RequestType * const aSVCReq)
{
    int32_t retVal = BCM_ERR_OK;

    if ((NULL == aSVCReq)
        || (SVC_CMD_START > aSVCReq->sysReqID)
        || (SVC_CMD_END < aSVCReq->sysReqID)){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        SVC_ReqFuncTbl[aSVCReq->sysReqID - SVC_CMD_START](aSVCReq, aSVCReq->sysReqID);
    }
   return retVal;
}

/** @} */
