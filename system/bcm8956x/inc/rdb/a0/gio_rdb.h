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
    @file gio_rdb.h
    @brief RDB File for GIO

    @version 2019Jan21_rdb
*/

#ifndef GIO_RDB_H
#define GIO_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint8_t GIO_RESERVED_TYPE;




typedef uint32_t GIO_GPIO_G1_DIN_TYPE;
#define GIO_GPIO_G1_DIN_DATA_IN_MASK (0xfUL)
#define GIO_GPIO_G1_DIN_DATA_IN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_DOUT_TYPE;
#define GIO_GPIO_G1_DOUT_DATA_OUT_MASK (0xfUL)
#define GIO_GPIO_G1_DOUT_DATA_OUT_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_DRV_EN_TYPE;
#define GIO_GPIO_G1_DRV_EN_EN_MASK (0xfUL)
#define GIO_GPIO_G1_DRV_EN_EN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_TYP_TYPE;
#define GIO_GPIO_G1_INT_TYP_TYP_MASK (0xfUL)
#define GIO_GPIO_G1_INT_TYP_TYP_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_DU_EDG_TYPE;
#define GIO_GPIO_G1_INT_DU_EDG_EDGE_MASK (0xfUL)
#define GIO_GPIO_G1_INT_DU_EDG_EDGE_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_EDG_LVL_SEL_TYPE;
#define GIO_GPIO_G1_INT_EDG_LVL_SEL_EDGE_MASK (0xfUL)
#define GIO_GPIO_G1_INT_EDG_LVL_SEL_EDGE_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_MSK_TYPE;
#define GIO_GPIO_G1_INT_MSK_MSK_MASK (0xfUL)
#define GIO_GPIO_G1_INT_MSK_MSK_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_STS_TYPE;
#define GIO_GPIO_G1_INT_STS_STS_MASK (0xfUL)
#define GIO_GPIO_G1_INT_STS_STS_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_MSK_STS_TYPE;
#define GIO_GPIO_G1_INT_MSK_STS_STS_MASK (0xfUL)
#define GIO_GPIO_G1_INT_MSK_STS_STS_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_CLR_TYPE;
#define GIO_GPIO_G1_INT_CLR_CLR_MASK (0xfUL)
#define GIO_GPIO_G1_INT_CLR_CLR_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_PWM_SEL_TYPE;
#define GIO_GPIO_G1_PWM_SEL_MUX_SEL_MASK (0xfUL)
#define GIO_GPIO_G1_PWM_SEL_MUX_SEL_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_SEC_CFG_TYPE;
#define GIO_GPIO_G1_SEC_CFG_CFG_MASK (0x1fUL)
#define GIO_GPIO_G1_SEC_CFG_CFG_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INIT_TYPE;
#define GIO_GPIO_G1_INIT_VAL_MASK (0xfUL)
#define GIO_GPIO_G1_INIT_VAL_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_PAD_RES_TYPE;
#define GIO_GPIO_G1_PAD_RES_RES_MASK (0xfUL)
#define GIO_GPIO_G1_PAD_RES_RES_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_PAD_RESIS_EN_TYPE;
#define GIO_GPIO_G1_PAD_RESIS_EN_EN_MASK (0xfUL)
#define GIO_GPIO_G1_PAD_RESIS_EN_EN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_TST_IN_TYPE;
#define GIO_GPIO_G1_TST_IN_IN_MASK (0xfUL)
#define GIO_GPIO_G1_TST_IN_IN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_TST_OUT_TYPE;
#define GIO_GPIO_G1_TST_OUT_OUT_MASK (0xfUL)
#define GIO_GPIO_G1_TST_OUT_OUT_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_TST_IN_EN_TYPE;
#define GIO_GPIO_G1_TST_IN_EN_EN_MASK (0xfUL)
#define GIO_GPIO_G1_TST_IN_EN_EN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_PWR_FAIL_TRI_STS_TYPE;
#define GIO_GPIO_G1_PWR_FAIL_TRI_STS_STS_MASK (0xfUL)
#define GIO_GPIO_G1_PWR_FAIL_TRI_STS_STS_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_PWR_FAIL_TRI_EN_TYPE;
#define GIO_GPIO_G1_PWR_FAIL_TRI_EN_EN_MASK (0x1fUL)
#define GIO_GPIO_G1_PWR_FAIL_TRI_EN_EN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_HYSTER_EN_TYPE;
#define GIO_GPIO_G1_HYSTER_EN_EN_MASK (0xfUL)
#define GIO_GPIO_G1_HYSTER_EN_EN_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_SLEW_CTRL_TYPE;
#define GIO_GPIO_G1_SLEW_CTRL_CTRL_MASK (0xfUL)
#define GIO_GPIO_G1_SLEW_CTRL_CTRL_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_DRV_SEL_0_TYPE;
#define GIO_GPIO_G1_DRV_SEL_0_0_MASK (0xfUL)
#define GIO_GPIO_G1_DRV_SEL_0_0_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_DRV_SEL_1_TYPE;
#define GIO_GPIO_G1_DRV_SEL_1_1_MASK (0xfUL)
#define GIO_GPIO_G1_DRV_SEL_1_1_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_DRV_SEL_2_TYPE;
#define GIO_GPIO_G1_DRV_SEL_2_2_MASK (0xfUL)
#define GIO_GPIO_G1_DRV_SEL_2_2_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_AUX_SEL_TYPE;
#define GIO_GPIO_G1_AUX_SEL_AUX01_SEL_MASK (0xfUL)
#define GIO_GPIO_G1_AUX_SEL_AUX01_SEL_SHIFT (0UL)




typedef uint32_t GIO_GPIO_G1_INT_POL_TYPE;
#define GIO_GPIO_G1_INT_POL_G1_RESERVED_1_MASK (0x1cUL)
#define GIO_GPIO_G1_INT_POL_G1_RESERVED_1_SHIFT (2UL)
#define GIO_GPIO_G1_INT_POL_POL_MASK (0x2UL)
#define GIO_GPIO_G1_INT_POL_POL_SHIFT (1UL)


typedef volatile struct COMP_PACKED _GIO_CHAN_GRP_RDBType {
    GIO_GPIO_G1_DIN_TYPE din; /* OFFSET: 0x100 */
    GIO_GPIO_G1_DOUT_TYPE dout; /* OFFSET: 0x104 */
    GIO_GPIO_G1_DRV_EN_TYPE drv_en; /* OFFSET: 0x108 */
    GIO_GPIO_G1_INT_TYP_TYPE int_typ; /* OFFSET: 0x10c */
    GIO_GPIO_G1_INT_DU_EDG_TYPE int_du_edg; /* OFFSET: 0x110 */
    GIO_GPIO_G1_INT_EDG_LVL_SEL_TYPE int_edg_lvl_sel; /* OFFSET: 0x114 */
    GIO_GPIO_G1_INT_MSK_TYPE int_msk; /* OFFSET: 0x118 */
    GIO_GPIO_G1_INT_STS_TYPE int_sts; /* OFFSET: 0x11c */
    GIO_GPIO_G1_INT_MSK_STS_TYPE int_msk_sts; /* OFFSET: 0x120 */
    GIO_GPIO_G1_INT_CLR_TYPE int_clr; /* OFFSET: 0x124 */
    GIO_GPIO_G1_PWM_SEL_TYPE pwm_sel; /* OFFSET: 0x128 */
    GIO_GPIO_G1_SEC_CFG_TYPE sec_cfg; /* OFFSET: 0x12c */
    GIO_GPIO_G1_INIT_TYPE init; /* OFFSET: 0x130 */
    GIO_GPIO_G1_PAD_RES_TYPE pad_res; /* OFFSET: 0x134 */
    GIO_GPIO_G1_PAD_RESIS_EN_TYPE pad_resis_en; /* OFFSET: 0x138 */
    GIO_GPIO_G1_TST_IN_TYPE tst_in; /* OFFSET: 0x13c */
    GIO_GPIO_G1_TST_OUT_TYPE tst_out; /* OFFSET: 0x140 */
    GIO_GPIO_G1_TST_IN_EN_TYPE tst_in_en; /* OFFSET: 0x144 */
    GIO_RESERVED_TYPE rsvd1[8]; /* OFFSET: 0x148 */
    GIO_GPIO_G1_PWR_FAIL_TRI_STS_TYPE pwr_fail_tri_sts; /* OFFSET: 0x150 */
    GIO_GPIO_G1_PWR_FAIL_TRI_EN_TYPE pwr_fail_tri_en; /* OFFSET: 0x154 */
    GIO_GPIO_G1_HYSTER_EN_TYPE hyster_en; /* OFFSET: 0x158 */
    GIO_GPIO_G1_SLEW_CTRL_TYPE slew_ctrl; /* OFFSET: 0x15c */
    GIO_GPIO_G1_DRV_SEL_0_TYPE drv_sel_0; /* OFFSET: 0x160 */
    GIO_GPIO_G1_DRV_SEL_1_TYPE drv_sel_1; /* OFFSET: 0x164 */
    GIO_GPIO_G1_DRV_SEL_2_TYPE drv_sel_2; /* OFFSET: 0x168 */
    GIO_GPIO_G1_AUX_SEL_TYPE aux_sel; /* OFFSET: 0x16c */
    GIO_GPIO_G1_INT_POL_TYPE int_pol; /* OFFSET: 0x170 */
} GIO_CHAN_GRP_RDBType;



typedef volatile struct COMP_PACKED _GIO_RDBType {
    GIO_RESERVED_TYPE rsvd0[256]; /* OFFSET: 0x0 */
    GIO_CHAN_GRP_RDBType gio_chan_grp_regs; /* OFFSET: 0x100 */
    GIO_RESERVED_TYPE rsvd2[256]; /* OFFSET: 0x174 */
} GIO_RDBType;


#define GIO_BASE                        (0x4014C000UL)

#define GIO_CHAN_GRP_RDBTYPE_OFFSET     (0x4014c100UL)



#define GIO_MAX_HW_ID                   (1UL)


#define GIO1_BASE                       (0x4014C100UL)


#define GIO2_BASE                       (0x4014C200UL)


#define GIO_DRV_OUT                     (1UL)


#define GIO_INT_TYP_LVL                 (1UL)


#define GIO_AUXSEL_AUX                  (1UL)


#define GIO_PAD_RES_PULL_UP             (1UL)


#define GIO_PAD_RES_EN                  (1UL)


#define GIO_SLEW_NORMAL_SPEED           (1UL)


#define GIO_MAX_CHANNELS                (9UL)


#define GIO_MAX_PORTS                   (3UL)


#define GIO_PORT_SHIFT                  (2UL)


#define GIO_PINS_PER_PORT               (4UL)


#define GIO_PORT_LEVEL_MAX              (0xFFFFFFFFUL)


#define PINMUX_PIN_RES_EN_BIT_MASK      (0x00000001UL)


#define PINMUX_PIN_RES_SEL_BIT_MASK     (0x00000002UL)


#define PINMUX_PIN_DRV_PWR_BIT_MASK     (0x0000001CUL)


#define PINMUX_PIN_SLEW_RATE_BIT_MASK   (0x00000020UL)

#endif /* GIO_RDB_H */
