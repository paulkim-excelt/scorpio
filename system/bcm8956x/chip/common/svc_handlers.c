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

#define GetModuleLogLevel()     KLOG_LVL_ERROR

/* include files */
#include <stdlib.h>
#include <system.h>
#include <bcm_time.h>
#include <svc.h>
#include <osil/mcu_osil.h>

#include <uart_osil.h>

#if defined(ENABLE_FLASH)
#include <osil/flash_osil.h>
#endif  /* ENABLE_FLASH */

#if defined(ENABLE_GPIO_GIO_V1)
#include <osil/gpio_osil.h>
#include <osil/pinmux_osil.h>
#endif  /* ENABLE_GPIO_GIO_V1 */

#if defined(ENABLE_UART_CONSOLE)
#include <osil/uconsole_osil.h>
#endif  /* ENABLE_UART_CONSOLE */

#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#include <osil/ethxcvr_osil.h>
#endif  /* ENABLE_ETH */

#if defined(ENABLE_ETH_SWITCH)
#include <osil/eth_switch_osil.h>
#endif

#if defined(ENABLE_MSG_QUEUE)
#include <osil/msg_queue_osil.h>
#endif  /* ENABLE_MSG_QUEUE */

#if defined(ENABLE_ULOG)
#include <osil/log_osil.h>
#endif  /* ENABLE_ULOG */

#if defined(ENABLE_TIMER_SP804)
#include <osil/gpt_osil.h>
#endif  /* ENABLE_TIMER_SP804 */
#include "ee_internal.h"

#if defined(ENABLE_SPI_PL022)
#include <osil/spi_osil.h>
#endif  /* ENABLE_SPI_PL022 */

#if defined(ENABLE_IPC)
#include <osil/ipc_osil.h>
#endif

#ifdef ENABLE_DBGMEM
#include <osil/dbgmem_osil.h>
#endif
#if defined(ENABLE_OTP)
#include <osil/otp_osil.h>
#endif /* ENABLE_OTP */

#include <osil/bcm_osil_svc.h>
#include <osil/cache_osil.h>

#if defined(ENABLE_WATCHDOG_SP805)
#include <osil/wdt_osil.h>
#endif  /* ENABLE_WATCHDOG_SP805 */

static void DefaultSVCHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    (void)aMagicID;
    (void)aCmd;
    (void)aSysIO;
}

static const SVC_ReqHandlerType SVCHandlerFuncTbl[(SVC_CMD_END - SVC_CMD_START) + 1UL] = {
    MCU_SysCmdHandler,      /* 0x80 */
#if defined(ENABLE_TIMER_SP804)
    TIM_SysCmdHandler,          /* 0x81 */
#else
    DefaultSVCHandler,          /* 0x81 */
#endif
    DefaultSVCHandler,          /* 0x82 */
    UART_SysCmdHandler,         /* 0x83 */
#if defined(ENABLE_IIC_BSC)
    IIC_SysCmdHandler,          /* 0x84 */
#else
    DefaultSVCHandler,          /* 0x84 */
#endif
#if defined(ENABLE_SPI_PL022)
    SPI_SysCmdHandler,          /* 0x85 */
#else
    DefaultSVCHandler,          /* 0x85 */
#endif
#if defined(ENABLE_FLASH)
    FLASH_SysCmdHandler,        /* 0x86 */
#else
    DefaultSVCHandler,          /* 0x86 */
#endif
#if defined(ENABLE_WATCHDOG_SP805)
    WDT_SysCmdHandler,          /* 0x87 */
#else
    DefaultSVCHandler,          /* 0x87 */
#endif
#if defined(ENABLE_GPIO_GIO_V1)
    GPIO_SysCmdHandler,         /* 0x88 */
    PINMUX_SysCmdHandler,       /* 0x89 */
#else
    DefaultSVCHandler,          /* 0x88 */
    DefaultSVCHandler,          /* 0x89 */
#endif
    DefaultSVCHandler,          /* 0x8A */
    DefaultSVCHandler,          /* 0x8B */
    DefaultSVCHandler,          /* 0x8C */
    DefaultSVCHandler,          /* 0x8D */
#if defined(ENABLE_OTP)
    OTP_SysCmdHandler,          /* 0x8E */
#else
    DefaultSVCHandler,          /* 0x8E */
#endif
    DefaultSVCHandler,          /* 0x8F */
#if defined(ENABLE_IPC)
    IPC_SysCmdHandler,       /* 0x90 */
#else
    DefaultSVCHandler,          /* 0x90 */
#endif
    DefaultSVCHandler,          /* 0x91 */
    DefaultSVCHandler,          /* 0x92 */
    DefaultSVCHandler,          /* 0x93 */
    DefaultSVCHandler,          /* 0x94 */
    DefaultSVCHandler,          /* 0x95 */
    DefaultSVCHandler,          /* 0x96 */
    DefaultSVCHandler,          /* 0x97 */
    DefaultSVCHandler,          /* 0x98 */
    DefaultSVCHandler,          /* 0x99 */
    DefaultSVCHandler,          /* 0x9A */
    DefaultSVCHandler,          /* 0x9B */
    DefaultSVCHandler,          /* 0x9C */
    DefaultSVCHandler,          /* 0x9D */
    DefaultSVCHandler,          /* 0x9E */
    DefaultSVCHandler,          /* 0x9F */
    DefaultSVCHandler,          /* 0xA0 */
    DefaultSVCHandler,          /* 0xA1 */
    DefaultSVCHandler,          /* 0xA2 */
    DefaultSVCHandler,          /* 0xA3 */
    DefaultSVCHandler,          /* 0xA4 */
    DefaultSVCHandler,          /* 0xA5 */
    DefaultSVCHandler,          /* 0xA6 */
    DefaultSVCHandler,          /* 0xA7 */
    DefaultSVCHandler,          /* 0xA8 */
    DefaultSVCHandler,          /* 0xA9 */
    DefaultSVCHandler,          /* 0xAA */
    DefaultSVCHandler,          /* 0xAB */
    DefaultSVCHandler,          /* 0xAC */
    DefaultSVCHandler,          /* 0xAD */
    DefaultSVCHandler,          /* 0xAE */
    DefaultSVCHandler,          /* 0xAF */
#if defined(ENABLE_ETH)
    ETHER_SysCmdHandler,        /* 0xB0 */
#else
    DefaultSVCHandler,          /* 0xB0 */
#endif
    DefaultSVCHandler,          /* 0xB1 */
    DefaultSVCHandler,          /* 0xB2 */
    DefaultSVCHandler,          /* 0xB3 */
#if defined(ENABLE_ETH_SWITCH)
    ETHERSWT_SysCmdHandler,        /* 0xB4 */
#else
    DefaultSVCHandler,          /* 0xB4 */
#endif
#if defined(ENABLE_ETH)
    ETHXCVR_SysCmdHandler,      /* 0xB5 */
#else
    DefaultSVCHandler,          /* 0xB5 */
#endif
#if defined(ENABLE_ETH_TIME)
    ETHER_TimeSysCmdHandler,    /* 0xB6 */
#else
    DefaultSVCHandler,          /* 0xB6 */
#endif
    DefaultSVCHandler,          /* 0xB7 */
    DefaultSVCHandler,          /* 0xB8 */
    DefaultSVCHandler,          /* 0xB9 */
    DefaultSVCHandler,          /* 0xBA */
    DefaultSVCHandler,          /* 0xBB */
    DefaultSVCHandler,          /* 0xBC */
    DefaultSVCHandler,          /* 0xBD */
    DefaultSVCHandler,          /* 0xBE */
    DefaultSVCHandler,          /* 0xBF */
#if defined(ENABLE_MSG_QUEUE)
    MSGQ_CmdHandler,            /* 0xC0 */
#else
    DefaultSVCHandler,          /* 0xC0 */
#endif  /* ENABLE_MSG_QUEUE */
#if defined(ENABLE_UART_CONSOLE)
    UCONSOLE_SysCmdHandler,      /* 0xC1 */
#else
    SVC_DefaultHandler,          /* 0xC1 */
#endif /*ENABLE_UART_CONSOLE */
    DefaultSVCHandler,          /* 0xC2 */
#ifdef ENABLE_DBGMEM
    DBGMEM_SysCmdHandler,       /* 0xC3 */
#else
    DefaultSVCHandler,          /* 0xC3 */
#endif
    BCM_OsSysCmdHandler,        /* 0xC4 */
    DCACHE_SysCmdHandler,       /* 0xC5 */
    DefaultSVCHandler,          /* 0xC6 */
    DefaultSVCHandler,          /* 0xC7 */
    DefaultSVCHandler,          /* 0xC8 */
    DefaultSVCHandler,          /* 0xC9 */
    DefaultSVCHandler,          /* 0xCA */
    DefaultSVCHandler,          /* 0xCB */
    DefaultSVCHandler,          /* 0xCC */
    DefaultSVCHandler,          /* 0xCD */
    DefaultSVCHandler,          /* 0xCE */
    DefaultSVCHandler,          /* 0xCF */
};

/*
 * Handle the command requested by unprivileged software.
 */
void SVC_CmdHandler(SVC_RequestType *aSysReqIO, uint32_t aSysReqID)
{
    if (NULL == aSysReqIO) {
        /* TODO: Force crash */
    } else if ((SVC_CMD_START > aSysReqID)
               || (SVC_CMD_END < aSysReqID)
               || (aSysReqID != aSysReqIO->sysReqID)){
        aSysReqIO->response = BCM_ERR_INVAL_PARAMS;
    } else {
        SVCHandlerFuncTbl[aSysReqIO->sysReqID - SVC_CMD_START](aSysReqIO->magicID,
                                                                    aSysReqIO->cmd,
                                                                    aSysReqIO->svcIO);
        aSysReqIO->response = BCM_ERR_OK;
    }
}
