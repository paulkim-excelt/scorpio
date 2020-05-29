/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
#include <bcm_err.h>
#include <utils.h>
#include <switch_rdb.h>
#include "cfp_drv.h"
#include "switch_drv.h"
#include "ulog.h"

#define GetModuleLogLevel()     (ULOG_LVL_ERROR)

/**
    @name CFP Design IDs
    @{
    @brief Design IDs for CFP
*/
#define BRCM_SWDSGN_CFP_DRVPROGRAMTCAM_PART_PROC            (0x80U) /**< @brief #CFP_DrvProgramTCAM */
#define BRCM_SWDSGN_CFP_DRVPROGRAMACTIONPOLICYRAM_PART_PROC (0x81U) /**< @brief #CFP_DrvProgramActionPolicyRAM */
#define BRCM_SWDSGN_CFP_DRVPROGRAMRATEMETERRAM_PART_PROC    (0x82U) /**< @brief #CFP_DrvProgramRateMeterRAM */
#define BRCM_SWDSGN_CFP_DRVPROGRAMUDFS_PART_PROC            (0x83U) /**< @brief #CFP_DrvProgramUDFs */
#define BRCM_SWDSGN_CFP_DRVGETFORMAT_PART_PROC              (0x84U) /**< @brief #CFP_DrvGetFormat */
#define BRCM_SWDSGN_CFP_DRVWAITFORACCESS_PART_PROC          (0x85U) /**< @brief #CFP_DrvWaitForAccess */
#define BRCM_SWDSGN_CFP_DRVCHECKIFRULEEXISTS_PART_PROC      (0x86U) /**< @brief #CFP_DrvCheckIfRuleExists */
#define BRCM_SWDSGN_CFP_DRVINSERTRULE_PART_PROC             (0x87U) /**< @brief #CFP_DrvInsertRule */
#define BRCM_SWDSGN_CFP_DRVCOPYRULE_PART_PROC               (0x88U) /**< @brief #CFP_DrvCopyRule */
#define BRCM_SWDSGN_CFP_DRVALLOCATEROW_PART_PROC            (0x89U) /**< @brief #CFP_DrvAllocateRow */
#define BRCM_SWDSGN_CFP_DRVSETSTATS_PART_PROC               (0x8AU) /**< @brief #CFP_DrvSetStats */
#define BRCM_SWDSGN_CFP_DRVSETRULEVALID_PART_PROC           (0x8BU) /**< @brief #CFP_DrvSetRuleValid */
#define BRCM_SWDSGN_CFP_DRVVALIDATERULE_PART_PROC           (0x8CU) /**< @brief #CFP_DrvValidateRule */
#define BRCM_SWDSGN_CFP_DRVVALIDATEDSTMAPFLAGS_PART_PROC    (0x8DU) /**< @brief #CFP_DrvValidateDstMapFlags */
#define BRCM_SWDSGN_CFP_DRVVALIDATECOLOR_PART_PROC          (0x8EU) /**< @brief #CFP_DrvValidateColor */
#define BRCM_SWDSGN_CFP_DRVVALIDATEUDFBASE_PART_PROC        (0x8FU) /**< @brief #CFP_DrvValidateUDFBase */
#define BRCM_SWDSGN_CFP_DRVVALIDATEPKTLENCORR_PART_PROC     (0x90U) /**< @brief #CFP_DrvValidatePktLenCorr */
#define BRCM_SWDSGN_CFP_DRVCHECKACCEPTABLEFRAME_PART_PROC   (0x91U) /**< @brief #CFP_DrvCheckAcceptableFrame */
#define BRCM_SWDSGN_CFP_DRVVALIDATEL2L3FRAMING_PART_PROC    (0x92U) /**< @brief #CFP_DrvValidateL2L3Framing */
#define BRCM_SWDSGN_CFP_DRVRESETANDCLEARTCAM_PART_PROC      (0x93U) /**< @brief #CFP_DrvResetAndClearTCAM */
#define BRCM_SWDSGN_CFP_DRVPROGRAMRULE_PART_PROC            (0x94U) /**< @brief #CFP_DrvProgramRule */
#define BRCM_SWDSGN_CFP_DRVCHECKIFRULEISSTATIC_PART_PROC    (0x95U) /**< @brief #CFP_DrvCheckIfRuleIsStatic */
#define BRCM_SWDSGN_CFP_DRVREPORTERROR_PART_PROC            (0x96U) /**< @brief #CFP_DrvReportError */
#define BRCM_SWDSGN_CFP_DRVINIT_PROC                        (0x9AU) /**< @brief #CFP_DrvInit */
#define BRCM_SWDSGN_CFP_DRVADDRULE_PROC                     (0x9BU) /**< @brief #CFP_DrvAddRule */
#define BRCM_SWDSGN_CFP_DRVREMOVERULE_PROC                  (0x9CU) /**< @brief #CFP_DrvRemoveRule */
#define BRCM_SWDSGN_CFP_DRVUPDATERULE_PROC                  (0x9DU) /**< @brief #CFP_DrvUpdateRule */
#define BRCM_SWDSGN_CFP_DRVGETSTATS_PROC                    (0x9EU) /**< @brief #CFP_DrvGetStats */
#define BRCM_SWDSGN_CFP_DRVGETROWCONFIG_PART_PROC           (0x9FU) /**< @brief #CFP_DrvGetRowConfig */
#define BRCM_SWDSGN_CFP_DRVGETSNAPSHOT_PART_PROC            (0xA0U) /**< @brief #CFP_DrvGetSnapshot */
#define BRCM_SWDSGN_CFP_DRVENABLEPORT_PROC                  (0xA1U) /**< @brief #CFP_DrvEnablePort */
#define BRCM_SWDSGN_CFP_DRVDISABLEPORT_PROC                 (0xA2U) /**< @brief #CFP_DrvDisablePort */
#define BRCM_SWDSGN_CFP_DRVDEINIT_PROC                      (0xA3U) /**< @brief #CFP_DrvDeInit */
#define BRCM_SWDSGN_CFP_RDWR_TIMEOUT_MACRO                  (0xA4U) /**< @brief #CFP_RDWR_TIMEOUT */
#define BRCM_SWDSGN_CFP_HANDLE_GLOBAL                       (0xA5U) /**< @brief #CFP_Handle */
#define BRCM_SWDSGN_CFP_NUM_FORMATS_MACRO                   (0xA6U) /**< @brief #CFP_NUM_FORMATS */
#define BRCM_SWDSGN_CFP_STATE_TYPE                          (0xA7U) /**< @brief #CFP_StateType */
#define BRCM_SWDSGN_CFP_UDFQUEUEENTRY_TYPE                  (0xA8U) /**< @brief #CFP_UDFQueueEntryType */
#define BRCM_SWDSGN_CFP_RULEDATA_TYPE                       (0xA9U) /**< @brief #CFP_RuleDataType */
#define BRCM_SWDSGN_CFP_CONTEXT_TYPE                        (0xAAU) /**< @brief #CFP_ContextType */
#define BRCM_SWDSGN_CFP_ACCEPTABLEFRAME_TYPE                (0xABU) /**< @brief #CFP_AcceptableFrameType */
#define BRCM_SWDSGN_CFP_DRVAFTTOTAGSTATUS_PART_PROC         (0xACU) /**< @brief #CFP_DrvAFTtoTagStatus */
#define BRCM_SWDSGN_CFP_DRVTAGSTATUSTOAFT_PART_PROC         (0xADU) /**< @brief #CFP_DrvTagStatustoAFT */
/** @} */

/**
    @name CFP_AcceptableFrameType
    @{
    @brief Definitions for CFP Acceptable Frame Type.
           Combination of Un-Tagged and VLAN-Tagged type is not possible
           due to hardware constraint.

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
typedef uint32_t CFP_AcceptableFrameType;                        /**< @brief typedef for Acceptable Frame */
#define CFP_ACCEPTABLEFRAME_UNTAGGED                      (1UL)  /**< @brief Un-Tagged type, Tag status value = 0UL, Tag status mask = 3UL*/
#define CFP_ACCEPTABLEFRAME_PRIO_TAGGED                   (2UL)  /**< @brief Priority-Tagged type, Tag status value = 1UL, Tag status mask = 3UL */
#define CFP_ACCEPTABLEFRAME_UNTAGGED_OR_PRIO_TAGGED       (3UL)  /**< @brief Un-Tagged or Priority-Tagged type, Tag status value = 0UL,
                                                                             Tag status mask = 2UL */
#define CFP_ACCEPTABLEFRAME_VLAN_TAGGED                   (4UL)  /**< @brief VLAN-Tagged type, Tag status value = 3UL, Tag status mask = 3UL */
#define CFP_ACCEPTABLEFRAME_VLAN_OR_PRIO_TAGGED           (6UL)  /**< @brief VLAN-Tagged or Priority-Tagged type, Tag status value = 1UL,
                                                                             Tag status mask = 1UL */
#define CFP_ACCEPTABLEFRAME_ALL                           (7UL)  /**< @brief Un-Tagged, VLAN-Tagged or Priority-Tagged type,
                                                                             Tag status value = 0UL, Tag status mask =  0UL */
#define CFP_ACCEPTABLEFRAME_INVALID                       (5UL)  /**< @brief Invalid Tagged Frame*/
/** @} */

/**
    @brief Macro to acceptable-Frame-bit-map mask
    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_KEY_TAG_ACCPT_FRAME_MASK    ((CFP_KEY_TAG_UN_TAGGED_MASK)  | \
                                        (CFP_KEY_TAG_PRIO_TAGGED_MASK) | \
                                        (CFP_KEY_TAG_VLAN_TAGGED_MASK)) /**< @brief Mask for the status field in the tag parameter */


/**
    @brief Timeout for CFP RAM access
    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
#define CFP_RDWR_TIMEOUT             (8000UL)

/**
    @brief Macro to check a function's return value and jump to the err_exit
    label in case of error return

    @trace #BRCM_SWREQ_CFP
*/
#define CFP_ERR_EXIT(fn_ret)                \
    if (((ret) = (fn_ret)) != BCM_ERR_OK) { line = __LINE__; goto err_exit;}


/**
    @name CFP_State
    @{
    @brief State for the CFP module

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
typedef uint32_t CFP_StateType;      /**< @brief typedef for CFP state */
#define CFP_STATE_RESET        (0UL) /**< @brief Reset state           */
#define CFP_STATE_INITIALIZED  (1UL) /**< @brief Initialized state     */
/** @} */

/**
    @name CFP_RuleDataType
    @{
    @brief Cache per rule
    Stores information required per rule at run time in the following format
    enable (1) | static (1) | slice (2) | format (2) |  resv (1) | udfMap (9)

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
typedef uint16_t CFP_RuleDataType;           /**< @brief typedef for CFP rule data */
#define CFP_RULEDATA_ENABLE_SHIFT  (15U)     /**< @brief Shift for enable field    */
#define CFP_RULEDATA_ENABLE_MASK   (0x8000U) /**< @brief Mask for enable field     */
#define CFP_RULEDATA_STATIC_SHIFT  (14U)     /**< @brief Shift for static field    */
#define CFP_RULEDATA_STATIC_MASK   (0x4000U) /**< @brief Mask for static field     */
#define CFP_RULEDATA_SLICE_SHIFT   (12U)     /**< @brief Shift for slice field     */
#define CFP_RULEDATA_SLICE_MASK    (0x3000U) /**< @brief Mask for slice field      */
#define CFP_RULEDATA_FORMAT_SHIFT  (10U)     /**< @brief Shift for format field    */
#define CFP_RULEDATA_FORMAT_MASK   (0x0C00U) /**< @brief Mask for format field     */
#define CFP_RULEDATA_UDFMAP_SHIFT  (0U)      /**< @brief Shift for UDF map field   */
#define CFP_RULEDATA_UDFMAP_MASK   (0x1FFU)  /**< @brief Mask for UDF map field    */
/** @} */

/**
    @brief Structure for UDF Scratch Queue
    This stores the temporary information during UDF allocation for a rule in
    a particular slice.

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_UDFQueueEntryType {
    uint16_t value;   /**< @brief Value of the UDF */
    uint16_t mask;    /**< @brief Mask of the UDF */
    uint8_t enable;  /**< @brief Flag indicating whether the UDF is enabled or
                           not */
    uint8_t created; /**< @brief Flag indicating whether the UDF is already
                           allocated or not */
    uint8_t  address; /**< @brief Address (base and offset) of the UDF */
} CFP_UDFQueueEntryType;

/**
    @brief Structure for CFP context

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
typedef struct _CFP_ContextType {
    CFP_StateType     state;                     /**< @brief State of the
                                                      module */
    uint32_t          numRules;                  /**< @brief Number of rules
                                                      currently programmed */
    CFP_UDFQueueEntryType udfScratchQ[CFP_MAX_UDFS]; /**< @brief Temporary list of
                                                     UDFs allocated on a
                                                     slice for a rule */
    CFP_UDFAllocListType  udfList[CFP_NUM_FORMATS];  /**< @brief Global list of
                                                      UDF allocation across
                                                      all formats and slices*/
    CFP_RuleDataType      rules[CFP_MAX_RULES];      /**< @brief Cached context
                                                      for each rule */
} CFP_ContextType;

static SWITCH_RDBType *const SWITCH_REGS = (SWITCH_RDBType *)SWITCH_BASE;

/**
    @brief CFP Handle

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP
*/
static CFP_ContextType COMP_SECTION(".data.cfp") CFP_Handle =
{
    .state       = CFP_STATE_RESET,
    .numRules    = 0UL,
    .udfScratchQ = {{0U}},
    .udfList     = {{{{{0U}}}}},
    .rules       = {0U}
};

void CFP_DrvReportError(uint16_t aCompID, uint32_t aInstanceID,
                     uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                     uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, (uint8_t)aInstanceID, aApiID, aErr, 4UL, values);
}

/** @brief Reset and Clear TCAM

    @behavior Sync, Re-entrant

    @pre None

    This API disables the CFP lookup on all ports. It then resets and
    clears the TCAM.

    @param[in]     aID               Switch index

    @retval  #BCM_ERR_OK             TCAM reset and cleared successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    disable CFP
    reset and clear CFP
    wait for operation to complete
    @endcode
*/
static int32_t CFP_DrvResetAndClearTCAM(ETHERSWT_HwIDType aID)
{
    uint64_t regVal;
    uint32_t line    = 0UL;
    uint32_t timeout = 0UL;
    int32_t  ret     = BCM_ERR_OK;

    /* First disable the CFP on all ports: Software is not allowed to reset */
    /* TCAM in the middle of CFP lookup                                     */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, &regVal));
    regVal &= ~(SWITCH_PAGE_A1_CFP_CTL_REG_EN_MAP_MASK);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, regVal));

    /* Reset & Clear the CFP RAM */
    regVal = SWITCH_PAGE_A0_CFP_ACC_RAM_CLEAR_MASK
                | SWITCH_PAGE_A0_CFP_ACC_TCAM_RST_MASK;
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc, regVal));

    /* Poll for the Reset/Clear to complete */
    while (timeout < CFP_RDWR_TIMEOUT) {
        ret = SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc, &regVal);
        if ((ret != BCM_ERR_OK) ||
            ((regVal & (SWITCH_PAGE_A0_CFP_ACC_TCAM_RST_MASK
                        |SWITCH_PAGE_A0_CFP_ACC_RAM_CLEAR_MASK)) == 0ULL)) {
            break;
        }
        timeout++;
    }

    if (timeout == CFP_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
        line = __LINE__;
    }

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVRESETANDCLEARTCAM_PART_PROC, ret,
                        line, (uint32_t)CFP_Handle.state, 0UL, 0UL);
    }
    return ret;
}


/** @brief Get format from L3 framing type

    @behavior Sync, Re-entrant

    @pre None

    This API converts the L3 framing type to CFP format type

    @param[in]     aL3Framing        L3 framing type
    @param[out]    aFormat           CFP format type

    @retval  #BCM_ERR_OK             CFP format returned successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid aL3Framing

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Map L3framing values to format as follows:
        CFP_L3FRAMING_IPV4 -> 0
        CFP_L3FRAMING_IPV6 -> 1
        CFP_L3FRAMING_NONIP -> 2
        others -> error
    @endcode
*/
static int32_t CFP_DrvGetFormat(CFP_L3FramingType aL3Framing,
                                uint32_t* const aFormat)
{
    int32_t ret = BCM_ERR_OK;

    switch(aL3Framing) {
        case CFP_L3FRAMING_IPV4:
            *aFormat = 0UL;
            break;
        case CFP_L3FRAMING_IPV6:
            *aFormat = 1UL;
            break;
        case CFP_L3FRAMING_NONIP:
            *aFormat = 2UL;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }
    return ret;
}

/** @brief Validate destination map flags value

    @behavior Sync, Re-entrant

    @pre None

    This API validates the input destination map flags value

    @param[in]     aID               Switch index
    @param[in]     aFlags            Destination map flags value

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if fwdMap is in [CFP_CHANGEFWDMAP_NON, CFP_CHANGEFWDMAP_REP]
        return SUCCESS
    else if fwdMap is in [CFP_CHANGEFWDMAP_REM, CFP_CHANGEFWDMAP_ADD]
        if dstMap is not 0
            return SUCCESS
    return FAIL
    @endcode
*/
static int32_t CFP_DrvValidateDstMapFlags(ETHERSWT_HwIDType aID,
                                          uint32_t aFlags)
{
    int32_t  ret         = BCM_ERR_INVAL_PARAMS;
    uint32_t dstMap      = (aFlags & CFP_ACTION_DSTMAP_MASK) >> CFP_ACTION_DSTMAP_SHIFT;
    uint32_t changFwdMap = (aFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >> CFP_ACTION_CHANGE_FWDMAP_SHIFT;

    if ((CFP_CHANGEFWDMAP_NON == changFwdMap) ||
        (CFP_CHANGEFWDMAP_REP == changFwdMap)) {
        ret = BCM_ERR_OK;
    } else {
        if (0U != dstMap) {
            ret = BCM_ERR_OK;
        }
    }
    return ret;
}

/** @brief Validate UDF base value

    @behavior Sync, Re-entrant

    @pre None

    This API validates the input UDF base value

    @param[in]     aID               Switch index
    @param[in]     aBase             UDF base value

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if value is not in [CFP_UDFBASE_SOP, CFP_UDFBASE_ENDL2HDR, CFP_UDFBASE_ENDL3HDR]
        return error
    @endcode
*/
static int32_t CFP_DrvValidateUDFBase(ETHERSWT_HwIDType aID,
                                      CFP_UDFBaseType aBase)
{
    int32_t ret;

    switch (aBase) {
        case CFP_UDFBASE_SOP:
        case CFP_UDFBASE_ENDL2HDR:
        case CFP_UDFBASE_ENDL3HDR:
            ret = BCM_ERR_OK;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVVALIDATEUDFBASE_PART_PROC,
                        ret, (uint32_t)aBase, 0UL, 0UL, 0UL);
    }
    return ret;
}

/** @brief Validate Acceptable Frame

    @behavior Sync, Re-entrant

    @pre None

    This API Validate Acceptable Frame

    @param[in]     aID               Switch index
    @param[in]     frame             Acceptable Frame

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if value is not in [CFP_ACCEPTABLEFRAME_UNTAGGED, CFP_ACCEPTABLEFRAME_PRIO_TAGGED,
                        CFP_ACCEPTABLEFRAME_UNTAGGED_OR_PRIO_TAGGED,CFP_ACCEPTABLEFRAME_VLAN_TAGGED,CFP_ACCEPTABLEFRAME_VLAN_OR_PRIO_TAGGED,CFP_ACCEPTABLEFRAME_ALL]
        return error
    @endcode
*/
static int32_t CFP_DrvCheckAcceptableFrame(ETHERSWT_HwIDType aID,
                                        CFP_AcceptableFrameType frame)
{
    int32_t ret = BCM_ERR_OK;

    if ((CFP_ACCEPTABLEFRAME_INVALID == frame) || (0U == frame)) {
        ret = BCM_ERR_INVAL_PARAMS;
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVCHECKACCEPTABLEFRAME_PART_PROC,
                        ret, (uint32_t)frame, 0UL, 0UL, 0UL);
    }
    return ret;
}

/** @brief Convert Acceptable Frame Type to tagstatus value and tagstatus mask

    @behavior Sync, Re-entrant

    @pre None

    This API Convert Acceptable Frame Type to tagstatus value and tagstatus mask

    @param[in]     aID                Switch index
    @param[in]     frame              Acceptable Frame
    @param[out]    tagstatusVal       Tag Status Value
    @param[out]    tagstatusMask      Tag Status Mask

    @retval  None

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    @endcode
*/
static void CFP_DrvAFTtoTagStatus(ETHERSWT_HwIDType aID, CFP_AcceptableFrameType frame, uint32_t *tagStatusVal, uint32_t *tagStatusMask)
{
    switch(frame) {
        case CFP_ACCEPTABLEFRAME_UNTAGGED:
            *tagStatusVal  = 0UL;
            *tagStatusMask = 3UL;
            break;
        case CFP_ACCEPTABLEFRAME_PRIO_TAGGED:
            *tagStatusVal  = 1UL;
            *tagStatusMask = 3UL;
            break;
        case CFP_ACCEPTABLEFRAME_UNTAGGED_OR_PRIO_TAGGED:
            *tagStatusVal  = 0UL;
            *tagStatusMask = 2UL;
            break;
        case CFP_ACCEPTABLEFRAME_VLAN_TAGGED:
            *tagStatusVal  = 3UL;
            *tagStatusMask = 3UL;
            break;
        case CFP_ACCEPTABLEFRAME_VLAN_OR_PRIO_TAGGED:
            *tagStatusVal  = 1UL;
            *tagStatusMask = 1UL;
            break;
        default:
            /* case CFP_ACCEPTABLEFRAME_ALL */
            *tagStatusVal  = 0UL;
            *tagStatusMask = 0UL;
            break;
    }
}

/** @brief Convert tagstatus value and tagstatus mask to Acceptable Frame

    @behavior Sync, Re-entrant

    @pre None

    This API Convert tagstatus value and tagstatus mask to Acceptable Frame

    @param[in]    aID                 Switch index
    @param[in]    tagstatusVal        Tag Status Value
    @param[in]    tagstatusMask       Tag Status Mask

    @retval  #CFP_AcceptableFrameType Acceptable Frame

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    @endcode
*/
static CFP_AcceptableFrameType CFP_DrvTagStatustoAFT(ETHERSWT_HwIDType aID, uint32_t tagStatusVal, uint32_t tagStatusMask)
{
    CFP_AcceptableFrameType frame;

    if((0UL == tagStatusVal) && (3UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_UNTAGGED;
    }
    else if((1UL == tagStatusVal) && (3UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_PRIO_TAGGED;
    }
    else if((0UL == tagStatusVal) && (2UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_UNTAGGED_OR_PRIO_TAGGED;
    }
    else if((3UL == tagStatusVal) && (3UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_VLAN_TAGGED;
    }
    else if((1UL == tagStatusVal) && (1UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_VLAN_OR_PRIO_TAGGED;
    }
    else if((0UL == tagStatusVal) && (0UL == tagStatusMask)) {
        frame = CFP_ACCEPTABLEFRAME_ALL;
    }
    else {
        frame = CFP_ACCEPTABLEFRAME_INVALID;
        CFP_DrvReportError(BCM_CFP_ID, aID,
                BRCM_SWDSGN_CFP_DRVTAGSTATUSTOAFT_PART_PROC,
                BCM_ERR_DATA_INTEG, (uint32_t)tagStatusVal, (uint32_t)tagStatusMask, 0UL, 0UL);
    }
    return frame;
}

/** @brief Validate L2 and L3 framing values

    @behavior Sync, Re-entrant

    @pre None

    This API validates the input L2 and L3 framing values

    @param[in]     aID               Switch index
    @param[in]     aL2Framing        L2 framing value
    @param[in]     aL3Framing        L3 framing value

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if L3 framing == Non-IP
        if L2 framing is not in [CFP_L2FRAMING_DIXV2, CFP_L2FRAMING_SNAP_PUB,
                            CFP_L2FRAMING_SNAP_PVT, CFP_L2FRAMING_LLC]
        return error
    elseif L3 framing == IPv6 or IPv4
        if L2 framing is not in [CFP_L2FRAMING_DIXV2, CFP_L2FRAMING_SNAP_PUB]
        return error
    else
        return error
    @endcode
*/
static int32_t CFP_DrvValidateL2L3Framing(ETHERSWT_HwIDType aID,
                                          CFP_L2FramingType aL2Framing,
                                          CFP_L3FramingType aL3Framing)
{
    int32_t ret;

    if (CFP_L3FRAMING_NONIP == aL3Framing) {
        switch (aL2Framing) {
            case CFP_L2FRAMING_DIXV2:
            case CFP_L2FRAMING_SNAP_PUB:
            case CFP_L2FRAMING_LLC:
            case CFP_L2FRAMING_SNAP_PVT:
                ret = BCM_ERR_OK;
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
    } else if ((CFP_L3FRAMING_IPV4 == aL3Framing) ||
               (CFP_L3FRAMING_IPV6 == aL3Framing)) {
        switch (aL2Framing) {
            case CFP_L2FRAMING_DIXV2:
            case CFP_L2FRAMING_SNAP_PUB:
                ret = BCM_ERR_OK;
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                break;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVVALIDATEL2L3FRAMING_PART_PROC,
                        ret, (uint32_t)aL2Framing, (uint32_t)aL3Framing,
                        0UL, 0UL);
    }
    return ret;
}

/** @brief Validate packet length correction value

    @behavior Sync, Re-entrant

    @pre None

    This API validates the input packet length correction value

    @param[in]     aID               Switch index
    @param[in]     aCorr             Packet length correction value

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if value is not in
        [CFP_PKTLENCORR_NONE, CFP_PKTLENCORR_ADD_PRE_SFD, CFP_PKTLENCORR_ADD_PRE_SFD_IFG]
        return error
    @endcode
*/
static int32_t CFP_DrvValidatePktLenCorr(ETHERSWT_HwIDType aID,
                                         CFP_PktLenCorrType aCorr)
{
    int32_t ret;

    switch (aCorr) {
        case CFP_PKTLENCORR_NONE:
        case CFP_PKTLENCORR_ADD_PRE_SFD:
        case CFP_PKTLENCORR_ADD_PRE_SFD_IFG:
            ret = BCM_ERR_OK;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    return ret;
}

/** @brief Validate color value

    @behavior Sync, Re-entrant

    @pre None

    This API validates the input color value

    @param[in]     aID               Switch index
    @param[in]     aColor            Color value

    @retval  #BCM_ERR_OK             Valid value
    @retval  #BCM_ERR_INVAL_PARAMS   Invalid value

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if value is not in [CFP_COLOR_GREEN, CFP_COLOR_YELLOW, CFP_COLOR_RED]
        return error
    @endcode
*/
static int32_t CFP_DrvValidateColor(ETHERSWT_HwIDType aID, CFP_ColorType aColor)
{
    int32_t ret;

    switch (aColor) {
        case CFP_COLOR_GREEN:
        case CFP_COLOR_RED:
        case CFP_COLOR_YELLOW:
            ret = BCM_ERR_OK;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVVALIDATECOLOR_PART_PROC,
                        ret, (uint32_t)aColor, 0UL, 0UL, 0UL);
    }
    return ret;
}

/** @brief Validate rule configuration

    @behavior Sync, Re-entrant

    @pre None

    This API checks the input parameters in the rule configuration and reports
    any errors

    @param[in]     aID               Switch index
    @param[in]     aKey              CFP Key
    @param[in]     aAction           CFP Action

    @retval  #BCM_ERR_OK             Rule is valid
    @retval  #BCM_ERR_INVAL_PARAMS   Returned if aRule or aRow is NULL
                                     Returned if any of the following fields are
                                     invalid
                                         - cTagFlags/sTagFlags
                                         - l2Framing
                                         - l3Framing
                                         - numEnabledUDFs
                                         - UDF base
                                         - dstMapIBFlags/- dstMapOBFlags
                                         - colorFlags
                                         - cirRefCnt/eirRefCnt
                                         - cirBktSize
                                         - cirTkBkt/eirTkBkt
    @retval  #BCM_ERR_NOMEM           CFP TCAM is full

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if ingressPortBitmap is not 0
        if numRules is not CFP_MAX_RULES
            validate CTagStatus
            validate STagStatus
            validate L2 and L3 framing
            validate numEnabledUDFs
            for all UDFs in Key: 0->numEnabledUDFs
                validate UDF base
            validate dstMapIBFlags
            validate dstMapOBFlags
            validate colorFlags
            if policer mode is not DISABLED
                if CIRRefCnt > 0 and CIRRefCnt <= Max rate
                    validate CIRBktSize and CIRTkBkt
                if EIRRefCnt != 0
                    validate EIRBktSize and EIRTkBkt
    @endcode
*/
static int32_t CFP_DrvValidateRule(ETHERSWT_HwIDType aID,
                                   uint32_t* const aRow,
                                   const CFP_KeyType* const aKey,
                                   const CFP_ActionType* const aAction)
{
    int32_t              ret     = BCM_ERR_OK;
    const CFP_MeterType  *meter  = &aAction->meter;
    uint32_t             line    = 0UL;
    uint32_t             dbgVal1 = 0UL;
    uint32_t             dbgVal2 = 0UL;
    uint32_t             i,j;

    /* Validate input row number */
    if (CFP_MAX_RULES < *aRow) {
        ret     = BCM_ERR_INVAL_PARAMS;
        line    = __LINE__;
        dbgVal1 = *aRow;
        goto err_exit;
    }

    /* Use the ingress port bitmap to determine if a rule is enabled or */
    /* not                                                              */
    if (0UL == (aKey->ingressPortBitmap & CFP_KEY_INGPORTBMP_MASK)) {
        ret     = BCM_ERR_INVAL_PARAMS;
        line    = __LINE__;
        dbgVal1 = (uint32_t)aKey->ingressPortBitmap;
        goto err_exit;
    }

    if (CFP_Handle.numRules >= CFP_MAX_RULES) {
        ret     = BCM_ERR_NOMEM;
        line    = __LINE__;
        dbgVal1 = (uint32_t)CFP_Handle.numRules;
        goto err_exit;
    }

    /* Check Acceptable Frame */
    ret = CFP_DrvCheckAcceptableFrame(aID,
            (CFP_AcceptableFrameType)((aKey->cTagFlags & CFP_KEY_TAG_ACCPT_FRAME_MASK) >> CFP_KEY_TAG_UN_TAGGED_SHIFT));
    if (BCM_ERR_OK != ret) {
        line    = __LINE__;
        dbgVal1 = (uint32_t)aKey->cTagFlags;
        goto err_exit;
    }

    /* Check Acceptable Frame */
    ret = CFP_DrvCheckAcceptableFrame(aID,
            (CFP_AcceptableFrameType)((aKey->sTagFlags & CFP_KEY_TAG_ACCPT_FRAME_MASK) >> CFP_KEY_TAG_UN_TAGGED_SHIFT));
    if (BCM_ERR_OK != ret) {
        line  = __LINE__;
        dbgVal1 = (uint32_t)aKey->sTagFlags;
        goto err_exit;
    }

    /* Validate L2 and L3 framing */
    ret = CFP_DrvValidateL2L3Framing(aID, aKey->l2Framing, aKey->l3Framing);
    if (BCM_ERR_OK != ret) {
        line    = __LINE__;
        dbgVal1 = (uint32_t)aKey->l2Framing;
        dbgVal2 = (uint32_t)aKey->l3Framing;
        goto err_exit;
    }

    /* Validate numEnabledUDFs */
    if (aKey->numEnabledUDFs > CFP_MAX_UDFS) {
        ret     = BCM_ERR_INVAL_PARAMS;
        line    = __LINE__;
        dbgVal1 = (uint32_t)aKey->numEnabledUDFs;
        goto err_exit;
    }

    /* Validate UDF base */
    for (i = 0UL; i < aKey->numEnabledUDFs; ++i) {
        ret = CFP_DrvValidateUDFBase(aID,
                (aKey->udfList[i].baseAndOffset & CFP_UDF_BASE_MASK) >> CFP_UDF_BASE_SHIFT);
        if (BCM_ERR_OK != ret) {
            line    = __LINE__;
            dbgVal1 = (uint32_t)aKey->udfList[i].baseAndOffset;
            dbgVal2 = i;
            break;
        }
        /* Store the address in the scratch queue */
        CFP_Handle.udfScratchQ[i].address = aKey->udfList[i].baseAndOffset;

        /* Check for duplicate UDFs */
        for (j = 0UL; j < i; ++j) {
            if (CFP_Handle.udfScratchQ[j].address == CFP_Handle.udfScratchQ[i].address) {
                line    = __LINE__;
                dbgVal1 = i;
                dbgVal2 = j;
                ret = BCM_ERR_INVAL_PARAMS;
                break;
            }
        }
        if (ret != BCM_ERR_OK) {
            break;
        }
    }

    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }
    /* Clean up the udf scratch queue */
    BCM_MemSet(&CFP_Handle.udfScratchQ, 0U, sizeof(CFP_Handle.udfScratchQ));

    /* Validate dstMapIBFlags */
    ret = CFP_DrvValidateDstMapFlags(aID, aAction->dstMapIBFlags);
    if (BCM_ERR_OK != ret) {
        line    = __LINE__;
        dbgVal1 = aAction->dstMapIBFlags;
        goto err_exit;
    }

    /* Validate dstMapOBFlags */
    ret = CFP_DrvValidateDstMapFlags(aID, aAction->dstMapOBFlags);
    if (BCM_ERR_OK != ret) {
        line    = __LINE__;
        dbgVal1 = aAction->dstMapOBFlags;
        goto err_exit;
    }

    /* Validate colorFlags */
    ret = CFP_DrvValidateColor(aID, (aAction->colorFlags & CFP_ACTION_COLOR_MASK) >>
                               CFP_ACTION_COLOR_SHIFT);
    if (BCM_ERR_OK != ret) {
        line    = __LINE__;
        dbgVal1 = aAction->colorFlags;
        goto err_exit;
    }

    if (CFP_POLICERMODE_DISABLED !=
       ((meter->policerFlags & CFP_METER_MODE_MASK) >> CFP_METER_MODE_SHIFT)) {
        /* Validate CIR parameters */
        if ((0UL < meter->cirRefCnt) &&
            (CFP_METER_REFCNT_MASK >= meter->cirRefCnt)) {

            if ((meter->cirBktSize > CFP_METER_BKTSZ_MASK) ||
                (0UL == meter->cirBktSize) ||
                (meter->cirTkBkt > CFP_METER_TKBKT_MASK)) {
                ret     = BCM_ERR_INVAL_PARAMS;
                line    = __LINE__;
                dbgVal1 = meter->cirBktSize;
                dbgVal2 = meter->cirTkBkt;
                goto err_exit;
            }

        } else {
            ret     = BCM_ERR_INVAL_PARAMS;
            line    = __LINE__;
            dbgVal1 = meter->cirRefCnt;
            goto err_exit;
        }

        /* Validate EIR parameters */
        if (0UL != meter->eirRefCnt) {
            if ((meter->eirBktSize > CFP_METER_BKTSZ_MASK) ||
                (0UL == meter->eirBktSize) ||
                (CFP_METER_REFCNT_MASK < meter->eirRefCnt) ||
                (meter->eirTkBkt > CFP_METER_TKBKT_MASK)) {
                ret     = BCM_ERR_INVAL_PARAMS;
                line    = __LINE__;
                dbgVal1 = meter->eirBktSize;
                dbgVal2 = meter->eirTkBkt;
                goto err_exit;
            }
        }
    }

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVVALIDATERULE_PART_PROC,
                        ret, line, (uint32_t)aKey, dbgVal1, dbgVal2);
    }
    return ret;
}

/** @brief Wait for CFP access operation to complete

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API waits for a CFP operation to complete within a stipulated time.

    @param[in]     aID               Switch index

    @retval  #BCM_ERR_OK             Operation completed successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    while timeout has not occurred
        check if done bit in CFP access register is cleared
    @endcode
*/
static int32_t CFP_DrvWaitForAccess(ETHERSWT_HwIDType aID)
{
    uint64_t regVal;
    int32_t  ret     = BCM_ERR_OK;
    uint32_t timeout = 0UL;

    while (timeout < CFP_RDWR_TIMEOUT) {
        ret = SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc, &regVal);
        if ((ret != BCM_ERR_OK) ||
            ((regVal & SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK) == 0ULL)) {
            break;
        }
        timeout++;
    }

    if (timeout == CFP_RDWR_TIMEOUT) {
        ret = BCM_ERR_TIME_OUT;
    }
    return ret;
}

/** @brief Program the CFP key in the TCAM

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API programs the row in the TCAM for the given config

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number
    @param[in]     aSlice            Slice number
    @param[in]     aKey              Pointer to key configuration

    @retval  #BCM_ERR_OK             Row programmed successfully in TCAM
    @retval  #BCM_ERR_INVAL_PARAMS   Register or L3 framing type is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Construct DATA0 and MASK0 values from UDF0 and UDF1
    Write DATA0 and MASK0 registers

    Construct DATA1 and MASK1 values from UDF1, UDF2 and UDF3
    Write DATA1 and MASK1 registers

    Construct DATA2 and MASK2 values from UDF3, UDF4 and UDF5
    Write DATA2 and MASK2 registers

    Construct DATA3 and MASK3 values from UDF5, UDF6 and UDF7
    Write DATA3 and MASK3 registers

    Construct DATA4 and MASK4 values from UDF7, UDF8 and CTag
    Write DATA4 and MASK4 registers

    Construct DATA5 and MASK5 values from CTag, STag and UDFValid
    Write DATA5 and MASK5 registers

    Construct DATA6 and MASK6 values from UDFValid, flags, l3Framing, l2Framing,
    CTagStatus and STagStatus
    Write DATA6 and MASK6 registers

    Construct DATA7 and MASK7 values from ingressPortBitmap
    Write DATA7 and MASK7 registers

    Start write access at input row to TCAM
    Wait for write to TCAM to complete
    @endcode
*/
static int32_t CFP_DrvProgramTCAM(ETHERSWT_HwIDType aID,
                                  uint32_t aRow,
                                  uint32_t aSlice,
                                  const CFP_KeyType* const aKey)
{
    uint32_t  regVal;
    uint32_t  cTagAFT;
    uint32_t  sTagAFT;
    uint32_t  cTagStatusMask;
    uint32_t  sTagStatusMask;
    uint32_t  cTagStatusValue;
    uint32_t  sTagStatusValue;
    uint32_t  tagId;
    uint32_t  tagMask;
    uint32_t  value;
    uint32_t  udfValid = 0UL;
    uint32_t  valid;
    int32_t   ret = BCM_ERR_OK;
    uint32_t  line;

    /* DATA0 */
    regVal = SWITCH_CFP_DATA0_VALID                                                                    |
             ((aSlice << SWITCH_CFP_DATA0_SLICEID_SHIFT) & SWITCH_CFP_DATA0_SLICEID_MASK)                     |
             ((CFP_Handle.udfScratchQ[0U].value << SWITCH_CFP_DATA0_UDF0_SHIFT) & SWITCH_CFP_DATA0_UDF0_MASK) |
             ((CFP_Handle.udfScratchQ[1U].value << SWITCH_CFP_DATA0_UDF1_SHIFT) & SWITCH_CFP_DATA0_UDF1_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0,
                               (uint64_t)regVal));

    /* MASK0 */
    regVal = SWITCH_CFP_DATA0_SLICEID_MASK;
    if (1UL == CFP_Handle.udfScratchQ[0U].enable) {
        regVal   |= (CFP_Handle.udfScratchQ[0U].mask << SWITCH_CFP_DATA0_UDF0_SHIFT);
        udfValid |= 0x1UL;
    }
    if (1UL == CFP_Handle.udfScratchQ[1U].enable) {
        regVal   |= ((CFP_Handle.udfScratchQ[1U].mask & (SWITCH_CFP_DATA0_UDF1_MASK >> SWITCH_CFP_DATA0_UDF1_SHIFT))
                    << SWITCH_CFP_DATA0_UDF1_SHIFT);
        udfValid |= 0x2UL;
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask0,
                               (uint64_t)regVal));

    /* DATA1 */
    value = CFP_Handle.udfScratchQ[1U].value >> 8UL;
    regVal = ((value << SWITCH_CFP_DATA1_UDF1_SHIFT) & SWITCH_CFP_DATA1_UDF1_MASK) |
             ((CFP_Handle.udfScratchQ[2U].value << SWITCH_CFP_DATA1_UDF2_SHIFT) & SWITCH_CFP_DATA1_UDF2_MASK) |
             ((CFP_Handle.udfScratchQ[3U].value << SWITCH_CFP_DATA1_UDF3_SHIFT) & SWITCH_CFP_DATA1_UDF3_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data1,
                               (uint64_t)regVal));

    /* MASK1 */
    regVal = 0UL;
    if (1UL == CFP_Handle.udfScratchQ[1U].enable) {
        regVal |= (CFP_Handle.udfScratchQ[1U].mask >> 8U);
    }
    if (1UL == CFP_Handle.udfScratchQ[2U].enable) {
        regVal   |= (CFP_Handle.udfScratchQ[2U].mask << SWITCH_CFP_DATA1_UDF2_SHIFT);
        udfValid |= 0x4UL;
    }
    if (1UL == CFP_Handle.udfScratchQ[3U].enable) {
        regVal   |= ((CFP_Handle.udfScratchQ[3U].mask & (SWITCH_CFP_DATA1_UDF3_MASK >> SWITCH_CFP_DATA1_UDF3_SHIFT))
                    << SWITCH_CFP_DATA1_UDF3_SHIFT);
        udfValid |= 0x8UL;
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask1,
                               (uint64_t)regVal));

    /* DATA2 */
    value = CFP_Handle.udfScratchQ[3U].value >> 8UL;
    regVal = ((value << SWITCH_CFP_DATA2_UDF3_SHIFT) & SWITCH_CFP_DATA2_UDF3_MASK) |
             ((CFP_Handle.udfScratchQ[4U].value << SWITCH_CFP_DATA2_UDF4_SHIFT) & SWITCH_CFP_DATA2_UDF4_MASK) |
             ((CFP_Handle.udfScratchQ[5U].value << SWITCH_CFP_DATA2_UDF5_SHIFT) & SWITCH_CFP_DATA2_UDF5_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data2,
                               (uint64_t)regVal));

    /* MASK2 */
    regVal = 0UL;
    if (1UL == CFP_Handle.udfScratchQ[3U].enable) {
        regVal |= (CFP_Handle.udfScratchQ[3U].mask >> 8U);
    }
    if (1UL == CFP_Handle.udfScratchQ[4U].enable) {
        regVal   |= (CFP_Handle.udfScratchQ[4U].mask << SWITCH_CFP_DATA2_UDF4_SHIFT);
        udfValid |= 0x10UL;
    }
    if (1UL == CFP_Handle.udfScratchQ[5U].enable) {
        regVal   |= ((CFP_Handle.udfScratchQ[5U].mask & (SWITCH_CFP_DATA2_UDF5_MASK >> SWITCH_CFP_DATA2_UDF5_SHIFT))
                    << SWITCH_CFP_DATA2_UDF5_SHIFT);;
        udfValid |= 0x20UL;
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask2,
                               (uint64_t)regVal));

    /* DATA3 */
    value = CFP_Handle.udfScratchQ[5U].value >> 8UL;
    regVal = ((value << SWITCH_CFP_DATA3_UDF5_SHIFT) & SWITCH_CFP_DATA3_UDF5_MASK) |
             ((CFP_Handle.udfScratchQ[6U].value << SWITCH_CFP_DATA3_UDF6_SHIFT) & SWITCH_CFP_DATA3_UDF6_MASK) |
             ((CFP_Handle.udfScratchQ[7U].value << SWITCH_CFP_DATA3_UDF7_SHIFT) & SWITCH_CFP_DATA3_UDF7_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data3,
                               (uint64_t)regVal));

    /* MASK3 */
    regVal = 0UL;
    if (1UL == CFP_Handle.udfScratchQ[5U].enable) {
        regVal |= (CFP_Handle.udfScratchQ[5U].mask >> 8U);
    }
    if (1UL == CFP_Handle.udfScratchQ[6U].enable) {
        regVal   |= (CFP_Handle.udfScratchQ[6U].mask << SWITCH_CFP_DATA3_UDF6_SHIFT);
        udfValid |= 0x40UL;
    }
    if (1UL == CFP_Handle.udfScratchQ[7U].enable) {
        regVal   |= ((CFP_Handle.udfScratchQ[7U].mask & (SWITCH_CFP_DATA3_UDF7_MASK >> SWITCH_CFP_DATA3_UDF7_SHIFT))
                    << SWITCH_CFP_DATA3_UDF7_SHIFT);
        udfValid |= 0x80UL;
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask3,
                               (uint64_t)regVal));

    /* DATA4 */
    value = CFP_Handle.udfScratchQ[7U].value >> 8UL;
    regVal = ((value << SWITCH_CFP_DATA4_UDF7_SHIFT) & SWITCH_CFP_DATA4_UDF7_MASK) |
             ((CFP_Handle.udfScratchQ[8U].value << SWITCH_CFP_DATA4_UDF8_SHIFT) & SWITCH_CFP_DATA4_UDF8_MASK);

    valid      = ((aKey->cTagFlags & CFP_KEY_TAG_ID_VALID_MASK) >>
                 CFP_KEY_TAG_ID_VALID_SHIFT);

    if (1UL == valid) {
        tagId = ((aKey->cTagFlags & CFP_KEY_TAG_ID_MASK) >>
                 CFP_KEY_TAG_ID_SHIFT);
        regVal |= ((tagId << SWITCH_CFP_DATA4_CTAG_SHIFT) & SWITCH_CFP_DATA4_CTAG_MASK);
    }

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data4,
                               (uint64_t)regVal));

    /* MASK4 */
    regVal = 0UL;
    if (1UL == CFP_Handle.udfScratchQ[7U].enable) {
        regVal |= (CFP_Handle.udfScratchQ[7U].mask >> 8U);
    }
    if (1UL == CFP_Handle.udfScratchQ[8U].enable) {
        regVal   |= (CFP_Handle.udfScratchQ[8U].mask << SWITCH_CFP_DATA4_UDF8_SHIFT);
        udfValid |= 0x100UL;
    }
    if (1UL == valid) {
        tagMask = (aKey->cTagMask & CFP_KEY_TAG_IDMASK_MASK) >> CFP_KEY_TAG_IDMASK_SHIFT;
        regVal |= (SWITCH_CFP_DATA4_CTAG_MASK & (tagMask << SWITCH_CFP_DATA4_CTAG_SHIFT));
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask4,
                               (uint64_t)regVal));

    /* DATA5 */
    regVal = 0UL;
    /* ID */
    if (1UL == valid) {
        regVal |= (tagId >> 8UL);
    }

    /* PCP */
    valid = ((aKey->cTagFlags & CFP_KEY_TAG_PCP_VALID_MASK) >>
            CFP_KEY_TAG_PCP_VALID_SHIFT);

    if (1UL == valid) {
        regVal |= (((aKey->cTagFlags & CFP_KEY_TAG_PCP_MASK) >>
                  CFP_KEY_TAG_PCP_SHIFT) << 5UL);
    }

    /* DEI */
    valid = ((aKey->cTagFlags & CFP_KEY_TAG_DEI_VALID_MASK) >>
            CFP_KEY_TAG_DEI_VALID_SHIFT);

    if (1UL == valid) {
        regVal |= (((aKey->cTagFlags & CFP_KEY_TAG_DEI_MASK) >>
                  CFP_KEY_TAG_DEI_SHIFT) << 4UL);
    }

    regVal &= SWITCH_CFP_DATA5_CTAG_MASK;

    /* PCP */
    valid = ((aKey->sTagFlags & CFP_KEY_TAG_PCP_VALID_MASK) >>
            CFP_KEY_TAG_PCP_VALID_SHIFT);

    if (1UL == valid) {
        regVal |= (((aKey->sTagFlags & CFP_KEY_TAG_PCP_MASK) >>
                  CFP_KEY_TAG_PCP_SHIFT) << (SWITCH_CFP_DATA5_STAG_SHIFT + 13UL));
    }

    /* DEI */
    valid = ((aKey->sTagFlags & CFP_KEY_TAG_DEI_VALID_MASK) >>
            CFP_KEY_TAG_DEI_VALID_SHIFT);

    if (1UL == valid) {
        regVal |= (((aKey->sTagFlags & CFP_KEY_TAG_DEI_MASK) >>
                  CFP_KEY_TAG_DEI_SHIFT) << (SWITCH_CFP_DATA5_STAG_SHIFT + 12UL));
    }

    /* ID */
    valid      = ((aKey->sTagFlags & CFP_KEY_TAG_ID_VALID_MASK) >>
                 CFP_KEY_TAG_ID_VALID_SHIFT);

    if (1UL == valid) {
        tagId = ((aKey->sTagFlags & CFP_KEY_TAG_ID_MASK) >>
                CFP_KEY_TAG_ID_SHIFT);
        regVal |= ((tagId << SWITCH_CFP_DATA5_STAG_SHIFT) & SWITCH_CFP_DATA5_STAG_MASK);
    }

    regVal |= ((udfValid << SWITCH_CFP_DATA5_UDFVALID_7_0_SHIFT) & SWITCH_CFP_DATA5_UDFVALID_7_0_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data5,
                               (uint64_t)regVal));

    /* MASK5 */
    regVal = 0UL;
    if ((CFP_KEY_TAG_ID_VALID_MASK == (aKey->cTagFlags &
         CFP_KEY_TAG_ID_VALID_MASK))) {
        tagMask = (aKey->cTagMask & 0xF00UL) >> 8UL;
        regVal |= (0xFUL & (tagMask << SWITCH_CFP_DATA5_CTAG_SHIFT));
    }

    if (CFP_KEY_TAG_PCP_VALID_MASK == (aKey->cTagFlags &
        CFP_KEY_TAG_PCP_VALID_MASK)) {
        tagMask = (aKey->cTagMask & CFP_KEY_TAG_PCPMASK_MASK) >> CFP_KEY_TAG_PCPMASK_SHIFT;
        regVal |= (0xE0UL &  (tagMask << 5UL));
    }
    if (CFP_KEY_TAG_DEI_VALID_MASK == (aKey->cTagFlags &
        CFP_KEY_TAG_DEI_VALID_MASK)) {
        regVal |= 0x10UL;
    }

    if (1UL == valid) {
        tagMask = (aKey->sTagMask & CFP_KEY_TAG_IDMASK_MASK) >> CFP_KEY_TAG_IDMASK_SHIFT;
        regVal |= (tagMask << SWITCH_CFP_DATA5_STAG_SHIFT);
    }

    if (CFP_KEY_TAG_PCP_VALID_MASK == (aKey->sTagFlags &
        CFP_KEY_TAG_PCP_VALID_MASK)) {
        tagMask = (aKey->sTagMask & CFP_KEY_TAG_PCPMASK_MASK) >> CFP_KEY_TAG_PCPMASK_SHIFT;
        regVal |= (tagMask << (SWITCH_CFP_DATA5_STAG_SHIFT + 13UL));
    }
    if (CFP_KEY_TAG_DEI_VALID_MASK == (aKey->sTagFlags &
        CFP_KEY_TAG_DEI_VALID_MASK)) {
        regVal |= (0x1000UL << SWITCH_CFP_DATA5_STAG_SHIFT);
    }

    regVal |= ((udfValid << SWITCH_CFP_DATA5_UDFVALID_7_0_SHIFT) &
              SWITCH_CFP_DATA5_UDFVALID_7_0_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask5,
                               (uint64_t)regVal));

    /* DATA6 */
    regVal = (((udfValid >> 8UL) << SWITCH_CFP_DATA6_UDFVALID_8_SHIFT) &
             SWITCH_CFP_DATA6_UDFVALID_8_MASK);

    switch(aKey->l3Framing) {
        case CFP_L3FRAMING_IPV4:
            /* Fall through intentional */
        case CFP_L3FRAMING_IPV6:
            regVal |= (aKey->flags & CFP_KEY_IP_MASK) <<
                      SWITCH_CFP_DATA6_TTL_SHIFT;
            break;
        case CFP_L3FRAMING_NONIP:
            regVal |= (((aKey->flags & CFP_KEY_NONIP_MASK) <<
                        SWITCH_CFP_DATA6_ETHTYPE_SHIFT) & SWITCH_CFP_DATA6_ETHTYPE_MASK);
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            line = __LINE__;
            break;
    }
    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }

    regVal |= ((aKey->l3Framing << SWITCH_CFP_DATA6_L3FRAMING_SHIFT) & SWITCH_CFP_DATA6_L3FRAMING_MASK);
    regVal |= ((aKey->l2Framing << SWITCH_CFP_DATA6_L2FRAMING_SHIFT) & SWITCH_CFP_DATA6_L2FRAMING_MASK);

    cTagAFT = ((aKey->cTagFlags & CFP_KEY_TAG_ACCPT_FRAME_MASK) >>
                 CFP_KEY_TAG_UN_TAGGED_SHIFT);
    CFP_DrvAFTtoTagStatus(aID, (CFP_AcceptableFrameType)cTagAFT, &cTagStatusValue, &cTagStatusMask);
    regVal |= ((cTagStatusValue << SWITCH_CFP_DATA6_CTAGSTATUS_SHIFT) & SWITCH_CFP_DATA6_CTAGSTATUS_MASK);

    sTagAFT = ((aKey->sTagFlags & CFP_KEY_TAG_ACCPT_FRAME_MASK) >>
                 CFP_KEY_TAG_UN_TAGGED_SHIFT);
    CFP_DrvAFTtoTagStatus(aID, (CFP_AcceptableFrameType)sTagAFT, &sTagStatusValue, &sTagStatusMask);
    regVal |= ((sTagStatusValue << SWITCH_CFP_DATA6_STAGSTATUS_SHIFT) & SWITCH_CFP_DATA6_STAGSTATUS_MASK);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data6,
                               (uint64_t)regVal));

    /* MASK6 */
    regVal = (((udfValid >> 8UL) << SWITCH_CFP_DATA6_UDFVALID_8_SHIFT) &
             SWITCH_CFP_DATA6_UDFVALID_8_MASK);

    switch(aKey->l3Framing) {
        case CFP_L3FRAMING_IPV4:
            /* Fall through intentional */
        case CFP_L3FRAMING_IPV6:
            regVal |= (aKey->flagsMask & CFP_KEY_IP_MASK) <<
                      SWITCH_CFP_DATA6_TTL_SHIFT;
            break;
        case CFP_L3FRAMING_NONIP:

            regVal |= (((aKey->flagsMask & CFP_KEY_NONIP_MASK) <<
                        SWITCH_CFP_DATA6_ETHTYPE_SHIFT) & SWITCH_CFP_DATA6_ETHTYPE_MASK);
            break;
        default:
            break;
    }

    /* L3 Framing is mandatorily enabled */
    regVal |= SWITCH_CFP_DATA6_L3FRAMING_MASK;

    if (aKey->l2Framing != CFP_L2FRAMING_DIXV2) {
        regVal |= SWITCH_CFP_DATA6_L2FRAMING_MASK;
    }

    regVal |= cTagStatusMask << SWITCH_CFP_DATA6_CTAGSTATUS_SHIFT;
    regVal |= sTagStatusMask << SWITCH_CFP_DATA6_STAGSTATUS_SHIFT;

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask6,
                               (uint64_t)regVal));

    /* DATA7 */
    regVal = (((aKey->ingressPortBitmap) & SWITCH_CFP_DATA7_SRCPRTMAP_MASK) <<
              SWITCH_CFP_DATA7_SRCPRTMAP_SHIFT);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data7,
                               (uint64_t)regVal));

    /* MASK7 */
    regVal = (((~aKey->ingressPortBitmap) & SWITCH_CFP_DATA7_SRCPRTMAP_MASK) <<
              SWITCH_CFP_DATA7_SRCPRTMAP_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask7,
                               (uint64_t)regVal));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVPROGRAMTCAM_PART_PROC,
                        ret, line, aRow, aSlice, (uint32_t) aKey->l3Framing);
    }
    return ret;
}


/** @brief Program the CFP Action Policy RAM

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API programs the row in the action policy RAM for the given config

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number
    @param[in]     aAction           Pointer to action configuration

    @retval  #BCM_ERR_OK             Row programmed successfully in Action
                                     Policy RAM
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Construct ACTPOL0 value from otherFlags, reasonCode, dstMapIBFlags and
    tosIBFlags
    Write ACTPOL0 register

    Construct ACTPOL1 value from tosIBFlags, dstMapOBFlags, tosOBFlags, chainID,
    colorFlags and otherFlags
    Write ACTPOL1 register

    Write 0 to ACTPOL2 register

    Start write access at input row to Action RAM
    Wait for write to Action RAM to complete
    @endcode
*/
static int32_t CFP_DrvProgramActionPolicyRAM(ETHERSWT_HwIDType aID,
                                             uint32_t aRow,
                                             const CFP_ActionType* const aAction)
{
    uint32_t  regVal;
    int32_t   ret = BCM_ERR_OK;
    uint32_t  line;
    uint32_t  dstMapFlags = 0UL;

    /* ACT_POL_DATA0 */
    dstMapFlags = (aAction->dstMapIBFlags & (SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_DST_MAP_IB_MASK >> SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_DST_MAP_IB_SHIFT)) <<
                   SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_DST_MAP_IB_SHIFT;
    dstMapFlags |= ((aAction->dstMapIBFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >>
                   CFP_ACTION_CHANGE_FWDMAP_SHIFT) << SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_CHANGE_FWRD_MAP_IB_SHIFT;
    regVal = ((aAction->otherFlags & SWITCH_CFP_ACT_POL_DATA0_BYPASS_MASK) <<
             SWITCH_CFP_ACT_POL_DATA0_BYPASS_SHIFT) |
             ((aAction->reasonCode << SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_REASON_CODE_SHIFT) &
             SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_REASON_CODE_MASK) |
            (((aAction->otherFlags >> CFP_ACTION_LPBK_EN_SHIFT) << SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_LOOP_BK_EN_SHIFT) &
            SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_LOOP_BK_EN_MASK) |
            ((aAction->tcFlags << SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_TC_SHIFT) &
            SWITCH_CFP_ACT_POL_DATA0_TC_MASK) |
            (dstMapFlags) |
            ((aAction->tosIBFlags << SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_DSCP_IB_SHIFT) &
            SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_DSCP_IB_MASK);

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data0,
                (uint64_t)regVal));

    /* ACT_POL_DATA1 */
    dstMapFlags = (aAction->dstMapOBFlags & (SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_DST_MAP_OB_MASK >> SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_DST_MAP_OB_SHIFT)) <<
                   SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_DST_MAP_OB_SHIFT;
    dstMapFlags |= ((aAction->dstMapOBFlags & CFP_ACTION_CHANGE_FWDMAP_MASK) >>
                   CFP_ACTION_CHANGE_FWDMAP_SHIFT) << SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_FWRD_MAP_OB_SHIFT;
    regVal = ((((aAction->tosIBFlags & CFP_ACTION_CHANGE_TOS_MASK) >>
              CFP_ACTION_CHANGE_TOS_SHIFT) << SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_DSCP_IB_SHIFT) &
              SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_DSCP_IB_MASK) |
              (dstMapFlags) |
              ((aAction->tosOBFlags << SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_NEW_DSCP_OB_SHIFT) &
              SWITCH_CFP_ACT_POL_DATA1_TOS_MASK) |
              ((aAction->chainID << SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHAIN_ID_SHIFT) &
              SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHAIN_ID_MASK) |
              ((aAction->colorFlags << SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_COLOR_SHIFT) &
              SWITCH_CFP_ACT_POL_DATA1_COLOR_MASK) |
              ((((aAction->otherFlags & CFP_ACTION_USE_DFLT_RED_MASK) >>
              CFP_ACTION_USE_DFLT_RED_SHIFT) << SWITCH_CFP_ACT_POL_DATA1_REDDFLT_SHIFT) &
              SWITCH_CFP_ACT_POL_DATA1_REDDFLT_MASK);


    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data1,
                               (uint64_t)regVal));

    /* ACT_POL_DATA2 */
    regVal = 0UL;
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data2,
                               (uint64_t)regVal));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
            ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
            ((SWITCH_CFP_ACC_RAM_SEL_ACT_POL << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
            SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
            ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)|
            SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVPROGRAMACTIONPOLICYRAM_PART_PROC,
                        ret, line, aRow, 0UL, 0UL);
    }
    return ret;
}


/** @brief Program the CFP Rate Meter RAM

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API programs the row in the metering RAM for the given config

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number
    @param[in]     aMeter            Pointer to meter configuration

    @retval  #BCM_ERR_OK             Row programmed successfully in Rate Meter
                                     RAM
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Write policerFlags to RATE_METER0 register
    Write eirTkBkt to RATE_METER1 register
    Write eirBktSize to RATE_METER2 register
    Write eirRefCnt to RATE_METER3 register
    Write cirTkBkt to RATE_METER4 register
    Write cirBktSize to RATE_METER5 register
    Write cirRefCnt to RATE_METER6 register

    Start write access at input row to Rate Meter RAM
    Wait for write to Rate Meter RAM to complete
    @endcode
*/
static int32_t CFP_DrvProgramRateMeterRAM(ETHERSWT_HwIDType aID,
                                          uint32_t aRow,
                                          const CFP_MeterType* const aMeter)
{
    uint32_t  regVal;
    int32_t   ret = BCM_ERR_OK;
    uint32_t  line;

    /* RATE_METER0*/
    regVal = (aMeter->policerFlags & SWITCH_CFP_RATE_METER0_MASK);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter0,
                               (uint64_t)regVal));

    /* RATE_METER1*/
    regVal = ((aMeter->eirTkBkt & SWITCH_PAGE_A0_RATE_METER1_EIR_TK_BKT_MASK) <<
                SWITCH_PAGE_A0_RATE_METER1_EIR_TK_BKT_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter1,
                               (uint64_t)regVal));

    /* RATEMETER2*/
    regVal = ((aMeter->eirBktSize & SWITCH_PA0RM2_PAGE_A0_RATE_METER2_EIR_BKT_SIZE_MASK) <<
                SWITCH_PA0RM2_PAGE_A0_RATE_METER2_EIR_BKT_SIZE_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter2,
                               (uint64_t)regVal));

    /* RATEMETER3*/
    regVal = ((aMeter->eirRefCnt & SWITCH_PA0RM3_PAGE_A0_RATE_METER3_EIR_REF_CNT_MASK) <<
                SWITCH_PA0RM3_PAGE_A0_RATE_METER3_EIR_REF_CNT_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter3,
                               (uint64_t)regVal));

    /* RATEMETER4*/
    regVal = ((aMeter->cirTkBkt & SWITCH_PAGE_A0_RATE_METER4_CIR_TK_BKT_MASK) <<
                SWITCH_PAGE_A0_RATE_METER4_CIR_TK_BKT_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter4,
                               (uint64_t)regVal));

    /* RATEMETER5*/
    regVal = ((aMeter->cirBktSize & SWITCH_PA0RM5_PAGE_A0_RATE_METER5_CIR_BKT_SIZE_MASK) <<
                SWITCH_PA0RM5_PAGE_A0_RATE_METER5_CIR_BKT_SIZE_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter5,
                               (uint64_t)regVal));

    /* RATEMETER6*/
    regVal = ((aMeter->cirRefCnt & SWITCH_PA0RM6_PAGE_A0_RATE_METER6_CIR_REF_CNT_MASK) <<
                SWITCH_PA0RM6_PAGE_A0_RATE_METER6_CIR_REF_CNT_SHIFT);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter6,
                               (uint64_t)regVal));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RATE_METER << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID,
                        BRCM_SWDSGN_CFP_DRVPROGRAMRATEMETERRAM_PART_PROC,
                        ret, line, aRow, 0UL, 0UL);
    }
    return ret;
}


/** @brief Program the global UDFs

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API programs the newly allocated global UDFs for a particular slice and
    format.

    @param[in]     aID               Switch index
    @param[in]     aFormat           CFP Format
    @param[in]     aSlice            Slice number

    @retval  #BCM_ERR_OK             UDFs programmed successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    for all UDFs in scratch Q: 0->CFP_MAX_UDFS
        if UDF enabled but not created
            Write UDF register in H/W
    @endcode
*/
static int32_t CFP_DrvProgramUDFs(ETHERSWT_HwIDType aID,
                                  uint32_t aFormat,
                                  uint32_t aSlice)
{
    uint32_t          i;
    int32_t           ret = BCM_ERR_OK;

    for (i = 0UL ; i < CFP_MAX_UDFS; ++i) {
        if ((TRUE == CFP_Handle.udfScratchQ[i].enable) &&
            (FALSE == CFP_Handle.udfScratchQ[i].created)) {
            ret = SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_udf_0_a_0 +
                                     (aFormat * 0x30UL) +
                                     (aSlice * 0x10UL) + i,
                                     CFP_Handle.udfScratchQ[i].address);
            if (BCM_ERR_OK != ret) {
                break;
            }
        }
    }

    return ret;
}

/** @brief Enable/Disable rule

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API enables/disables the rule at the input row

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number of the rule
    @param[in]     aEnable           Flag indicating whether the rule needs
                                     to be enabled or disabled

    @retval  #BCM_ERR_OK             Rule enabled/disabled successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    @endcode
*/
static int32_t CFP_DrvSetRuleValid(ETHERSWT_HwIDType aID,
                                   uint32_t aRow, uint32_t aEnable)
{
    uint64_t regVal;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line= 0UL;

    /* Read the TCAM */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* DATA0 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0, &regVal));

    if (TRUE == aEnable) {
        /* Set the slice valid */
        regVal |= SWITCH_CFP_DATA0_VALID;
    } else {
        /* Reset the slice valid */
        regVal &= ~(SWITCH_CFP_DATA0_VALID);
    }
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0, regVal));

    /* Write to the TCAM */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVSETRULEVALID_PART_PROC,
                           ret, line, aRow, aEnable, 0UL);
    }
    return ret;
}

/** @brief Set statistics counters

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API sets the statistics counters to the input values

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number of the rule
    @param[in]     aGreenCntr        Value to set the green counter to
    @param[in]     aYellowCntr       Value to set the yellow counter to
    @param[in]     aRedCntr          Value to set the red counter to

    @retval  #BCM_ERR_OK             Counters set successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Set Green counter in Green statistics RAM
    Set Yellow counter in Yellow statistics RAM
    Set Red counter in Red statistics RAM
    @endcode
*/
static int32_t CFP_DrvSetStats(ETHERSWT_HwIDType aID,
                               uint32_t aRow,
                               const CFP_StatsType *const aStats)
{
    int32_t  ret = BCM_ERR_OK;
    uint32_t line= 0UL;

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_green_cntr,
                                (uint64_t)aStats->green));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_GREEN_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_yellow_cntr,
                                (uint64_t)aStats->yellow));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_YELLOW_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_red_cntr,
                                (uint64_t)aStats->red));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RED_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVSETSTATS_PART_PROC,
                        ret, line, aRow, 0UL, 0UL);
    }
    return ret;
}

/** @brief Check if input row is occupied

    @behavior Sync, Re-entrant

    @pre None

    This API checks the S/W cache to decide if a rule is already programed
    at the input row

    @param[in]     aRow              Row number

    @retval  #BCM_ERR_OK             Row is occupied
    @retval  #BCM_ERR_NOT_FOUND      Row is not occupied

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if row < CFP_MAX_RULES and row is enabled
        return SUCCESS
    else
        return FAIL
    @endcode
*/
static int32_t CFP_DrvCheckIfRuleExists(uint32_t aRow)
{
    int32_t ret = BCM_ERR_NOT_FOUND;

    if (aRow < CFP_MAX_RULES) {
        if (0U != (CFP_RULEDATA_ENABLE_MASK & CFP_Handle.rules[aRow])) {
            ret = BCM_ERR_OK;
        }
    }

    return ret;
}

/** @brief Check if input row is occupied by a static rule

    @behavior Sync, Re-entrant

    @pre None

    This API checks the S/W cache to decide if a row is programed
    with a static rule

    @param[in]     aRow              Row number

    @retval  #BCM_ERR_OK             Rule is static
    @retval  #BCM_ERR_UNKNOWN        Rule is not static

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if row < CFP_MAX_RULES and row is enabled
        return SUCCESS
    else
        return FAIL
    @endcode
*/
static int32_t CFP_DrvCheckIfRuleIsStatic(uint32_t aRow)
{
    int32_t ret = BCM_ERR_UNKNOWN;

    if (aRow < CFP_MAX_RULES) {
        if (0U != (CFP_RULEDATA_STATIC_MASK & CFP_Handle.rules[aRow])) {
            ret = BCM_ERR_OK;
        }
    }

    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        if rule exists
            read Green counter
            read Yellow counter
            read Red counter
    @endcode
*/
int32_t CFP_DrvGetStats(ETHERSWT_HwIDType aID,
                        uint32_t aRow,
                        CFP_StatsType *const aStats)
{
    uint64_t regVal;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line;

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(CFP_DrvCheckIfRuleExists(aRow));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_GREEN_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_green_cntr,
                               &regVal));
    aStats->green = (uint32_t)regVal;

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_YELLOW_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_yellow_cntr,
                                &regVal));
    aStats->yellow = (uint32_t)regVal;

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RED_STATS << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_stat_red_cntr,
                                &regVal));
    aStats->red = (uint32_t)regVal;

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVGETSTATS_PROC, ret,
                        line, aRow, (uint32_t)CFP_Handle.state, 0UL);
    }
    return ret;
}

/** @brief Copy rule from one row to another

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API copies a rule, register by register from one row to another

    @param[in]     aID               Switch index
    @param[in]     aDestRow          Destination row number
    @param[in]     aSrcRow           Source row number

    @retval  #BCM_ERR_OK             Rule copied successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    Load Rate Meter registers from source row
    Write Rate Meter registers to destination row
    Load Action Policy registers from source row
    Write Action Policy registers to destination row
    Load TCAM registers from source row
    Write TCAM registers to destination row
    Load statistics from the source row
    Write statistics to the destination row
    @endcode
*/
static int32_t CFP_DrvCopyRule(ETHERSWT_HwIDType aID,
                               uint32_t aDestRow,
                               uint32_t aSrcRow)
{
    uint64_t      regVal;
    uint32_t      line;
    CFP_StatsType stats;
    int32_t       ret = BCM_ERR_OK;

    /* RATE METER */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aSrcRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RATE_METER << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter0,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter1,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter2,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter3,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter4,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter5,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter6,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aDestRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RATE_METER << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* ACT POL */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
            ((aSrcRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
            ((SWITCH_CFP_ACC_RAM_SEL_ACT_POL << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
            SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
            ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)|
            SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data0,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data1,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data2,
                               &regVal));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
            ((aDestRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
            ((SWITCH_CFP_ACC_RAM_SEL_ACT_POL << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
            SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
            ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)|
            SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* TCAM */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aSrcRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask0, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data1, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask1, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data2, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask2, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data3, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask3, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data4, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask4, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data5, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask5, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data6, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask6, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data7, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask7, &regVal));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aDestRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* Statistics */
    CFP_ERR_EXIT(CFP_DrvGetStats(aID, aSrcRow, &stats));
    CFP_ERR_EXIT(CFP_DrvSetStats(aID, aDestRow, &stats));
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVCOPYRULE_PART_PROC,
                        ret, line, aDestRow, aSrcRow, 0UL);
    }
    return ret;
}

/** @brief Insert rule at a particular row

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API copies a rule, register by register from one row to another

    @param[in]     aID               Switch index
    @param[in]     aRow              Row number to insert the rule at

    @retval  #BCM_ERR_OK             Rule inserted successfully
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out
    @retval  #BCM_ERR_NOMEM          No free space to insert

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    search for a higher numbered free row
    if found
        shift up all rules from input row onwards
        update rulesAssociated bitmap to reflect new row number
    @endcode
*/
static int32_t CFP_DrvInsertRule(ETHERSWT_HwIDType aID,
                                 uint32_t aRow)
{
    uint32_t             row   = 0UL;
    int32_t              ret   = BCM_ERR_OK;
    uint32_t             srcRow;
    uint32_t             destRow;
    uint32_t             line = 0UL;

    /* Find the last entry to be moved */
    for (row = aRow + 1UL; row < CFP_MAX_RULES; ++row) {
        if (BCM_ERR_OK != CFP_DrvCheckIfRuleExists(row)) {
            break;
        }
    }

    if (row >= CFP_MAX_RULES) {
        ret = BCM_ERR_NOMEM;
        line = __LINE__;
        goto err_exit;
    }

    /* Now shift up all rules from the input row number onwards */
    for (; row > aRow; row--) {
        srcRow  = row - 1UL;
        destRow = row;

        if (BCM_ERR_OK != CFP_DrvCopyRule(aID, destRow, srcRow)) {
            line = __LINE__;
            break;
        }
        /* Update the S/W data structures:                           */
        /*  - rulesAssociated with all the UDFs that srcRow is using */
        /*    to be associated with destRow                          */
        /*  - cached rule data associated with destRow to be updated */
        BCM_MemCpy(&CFP_Handle.rules[destRow],
                   &CFP_Handle.rules[srcRow],
                   sizeof(CFP_RuleDataType));
    }
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVINSERTRULE_PART_PROC,
                        ret, line, aRow, row, 0UL);
    }
    return ret;
}

/** @brief Allocate row in CFP table

    @behavior Sync, Re-entrant (for different switch index)

    @pre None

    This API allocates a row (if required) in the CFP table. If a valid
    row number is not provided, a free row is returned. If the input row
    number is already occupied, this routine tries to create space by pushing
    all higher numbered rows down.

    @param[in]     aID               Switch index
    @param[inout]  aRow              Pointer to row number

    @retval  #BCM_ERR_OK             Row is allocated
    @retval  #BCM_ERR_NOMEM          No free row
    @retval  #BCM_ERR_INVAL_PARAMS   Register is invalid
    @retval  #BCM_ERR_TIME_OUT       CFP/Switch Register access timed out

    @post None

    @trace #BRCM_SWARCH_CFP_CMDHANDLER_PROC
    @trace #BRCM_SWREQ_CFP

    @limitations None

    @code{.unparsed}
    if valid row number is provided
        if row is already occupied
            if existing rule is not static
                insert the rule
    else
        search for a free row
    @endcode
*/
static int32_t CFP_DrvAllocateRow(ETHERSWT_HwIDType aID,
                                  uint32_t* const aRow)
{
    int32_t  ret = BCM_ERR_OK;
    uint32_t row;
    uint32_t line;

    if (*aRow >= CFP_MAX_RULES) {
        /* There can be holes due to deletion so find out a free slot */
        for (row = 0UL; row < CFP_MAX_RULES; ++row) {
            if (BCM_ERR_OK != CFP_DrvCheckIfRuleExists(row)) {
                *aRow = row;
                break;
            }
        }

        if (*aRow == CFP_MAX_RULES) {
            ret  = BCM_ERR_NOMEM;
            line = __LINE__;
        }
    } else {
        /* Row number is provided - make sure it does not belong to a static rule */
        if (BCM_ERR_OK == CFP_DrvCheckIfRuleExists(*aRow)) {
            if (BCM_ERR_OK == CFP_DrvCheckIfRuleIsStatic(*aRow)) {
                ret  = BCM_ERR_NOPERM;
                line = __LINE__;
            } else {
                CFP_ERR_EXIT(CFP_DrvInsertRule(aID, *aRow));
            }
        }
    }

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVALLOCATEROW_PART_PROC,
                        ret, line, *aRow, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    validate rule
    for all slices: CFP_MAX_SLICES->0
        for all valid UDFs in config: 0->numEnabledUDFs
            for all UDFs in current slice: 0->CFP_MAX_UDFS
                if slice UDF == config UDF
                    UDFAccomodated = true
                    break
                else if slice UDF free
                    store UDF number
                    UDFAccomodated = true
                    slice UDF = config UDF
                else
                    UDFAccomodated = false
            if true == UDFAccomodated
                push to UDF scratch queue
            else
                slice UDF = free
        if all UDFs in config accomodated
            if row allocation succeeds
                program action policy RAM
                program rate meter RAM
                program UDFs
                program TCAM
                for all slice UDFs: 0->CFP_MAX_UDFS
                    if slice UDF == config UDF
                        mark slice UDF having current row
                update S/W cache
                increment rule count
                clear stats in H/W
    @endcode
*/
static int32_t CFP_DrvProgramRule(ETHERSWT_HwIDType aID,
                                  const CFP_KeyType* const aKey,
                                  const CFP_ActionType* const aAction,
                                  uint32_t* const aRow,
                                  uint32_t* const aSlice)
{
    uint32_t             format;
    uint32_t             slot;
    int32_t              ret            = BCM_ERR_OK;
    uint32_t             ruleProgrammed = FALSE;
    CFP_UDFAllocListType *list          = NULL;
    uint32_t             line           = 0UL;
    uint32_t             row            = *aRow;
    int32_t              slice          = CFP_MAX_SLICES;
    int32_t              startSlice;
    int32_t              endSlice;
    uint32_t             udfMap         = 0UL;
    CFP_StatsType        stats          = {0UL, 0UL, 0UL};

    CFP_ERR_EXIT(CFP_DrvValidateRule(aID, aRow, aKey, aAction));
    CFP_ERR_EXIT(CFP_DrvGetFormat(aKey->l3Framing, &format));

    list = &CFP_Handle.udfList[format];

    if (CFP_MAX_SLICES <= *aSlice) {
        startSlice = (int32_t)CFP_MAX_SLICES - 1L;
        endSlice = 0L;
    } else {
        startSlice = endSlice = (int32_t) *aSlice;
    }

    /* Traverse the highest priority slice and see if all UDFs */
    /* can fit. If yes , then move on to the next rule. If not,*/
    /* then move on to the next slice                          */
    for (slice = startSlice ; slice >= endSlice; --slice) {
        uint32_t ruleFitsInCurrSlice = TRUE;
        uint32_t udf;

        /* Clean up the udf allocation queue to start fresh for every slice */
        BCM_MemSet(&CFP_Handle.udfScratchQ, 0U, sizeof(CFP_Handle.udfScratchQ));

        for (udf = 0UL; udf < aKey->numEnabledUDFs; ++udf) {

            uint8_t currentUDFAddress = aKey->udfList[udf].baseAndOffset;

            uint32_t UDFAccomodated = FALSE;
            uint32_t freeSlot = CFP_MAX_UDFS;

            /* There can be holes in the allocated UDFs so search through */
            /* the complete list. Also store any free slots just in case  */
            /* the search fails completely                                */
            for (slot = 0UL ; slot < CFP_MAX_UDFS; ++slot) {
                if(1U == list->udfs[slice][slot].enable) {

                    if (list->udfs[slice][slot].address ==
                            currentUDFAddress) {

                        /* No need to look at any more UDF slots */
                        UDFAccomodated = TRUE;
                        break;
                    }
                } else {
                    /* Store any free slot so that it is easy to */
                    /* allocate                                  */
                    if (CFP_MAX_UDFS == freeSlot) {
                        freeSlot = slot;
                    }
                }
            } /* All UDF slots in current slice */

            if (TRUE == UDFAccomodated) {
                CFP_UDFQueueEntryType* entry = &CFP_Handle.udfScratchQ[slot];
                /* Insert into the queue so that it can */
                /* be unrolled if required              */
                entry->enable  = 1U;
                entry->address = currentUDFAddress;
                entry->value   = aKey->udfList[udf].value;
                entry->mask    = aKey->udfList[udf].mask;
                entry->created = 1U;
            } else if (CFP_MAX_UDFS > freeSlot) {
                CFP_UDFQueueEntryType* entry = &CFP_Handle.udfScratchQ[freeSlot];
                /* Use this free UDF */
                list->udfs[slice][freeSlot].enable = 1U;
                list->udfs[slice][freeSlot].address = currentUDFAddress;

                /* Insert into the queue so that it can */
                /* be unrolled if required              */
                entry->enable  = 1U;
                entry->address = currentUDFAddress;
                entry->value   = aKey->udfList[udf].value;
                entry->mask    = aKey->udfList[udf].mask;
                entry->created = 0U;
            } else {
                /* If even a single UDF is not accomodated, */
                /* unroll and hope that the next slice will */
                /* be able to accomodate                    */
                /* Unroll: Deallocate all the allocated UDFs */
                for (slot = 0UL ; slot < CFP_MAX_UDFS; ++slot) {
                    if ((TRUE == CFP_Handle.udfScratchQ[slot].enable) &&
                        (FALSE == CFP_Handle.udfScratchQ[slot].created)) {
                        list->udfs[slice][slot].enable = 0U;
                        list->udfs[slice][slot].address = 0U;
                    }
                }
                /* The rule does not fit into the current slice */
                ruleFitsInCurrSlice = FALSE;
                break;
            }
        } /* All UDFs in the UDFList in the config */

        if (TRUE == ruleFitsInCurrSlice) {
            /* Generate the row number if required */
            ret = CFP_DrvAllocateRow(aID, &row);
            if (BCM_ERR_OK == ret) {
                CFP_Handle.numRules++;

                /* Write to HW: First disable any existing rule at the row. Once this is done */
                /* we can safely clear the statistics and then proceed with programming the   */
                /* new rule                                                                   */
                CFP_ERR_EXIT(CFP_DrvSetRuleValid(aID, row, FALSE));
                /* Clear the stats in HW */
                CFP_ERR_EXIT(CFP_DrvSetStats(aID, row, &stats));
                CFP_ERR_EXIT(CFP_DrvProgramTCAM(aID, row, slice, aKey));
                CFP_ERR_EXIT(CFP_DrvProgramActionPolicyRAM(aID, row, aAction));
                CFP_ERR_EXIT(CFP_DrvProgramRateMeterRAM(aID, row, &aAction->meter));
                CFP_ERR_EXIT(CFP_DrvProgramUDFs(aID, format, slice));

                /* Store in cache */
                CFP_Handle.rules[row] = (CFP_RULEDATA_ENABLE_MASK) |
                                        ((slice << CFP_RULEDATA_SLICE_SHIFT) & CFP_RULEDATA_SLICE_MASK) |
                                        ((format << CFP_RULEDATA_FORMAT_SHIFT) & CFP_RULEDATA_FORMAT_MASK);

                /* Once the row number is allocated, mark all the UDFs */
                /* associated with this rule                           */
                udfMap = 0UL;
                for (slot = 0UL ; slot < CFP_MAX_UDFS; ++slot) {
                    if (TRUE == CFP_Handle.udfScratchQ[slot].enable) {
                        udfMap |= (1UL << slot);
                    }
                }

                CFP_Handle.rules[row] |= ((udfMap << CFP_RULEDATA_UDFMAP_SHIFT) & CFP_RULEDATA_UDFMAP_MASK);

                ruleProgrammed = TRUE;

                *aRow   = row;
                *aSlice = slice;
                break;
            } else {
                /* Clean up if row number allocation fails */
                for (slot = 0UL ; slot < CFP_MAX_UDFS; ++slot) {
                    if ((TRUE == CFP_Handle.udfScratchQ[slot].enable) &&
                        (FALSE == CFP_Handle.udfScratchQ[slot].created)) {
                        list->udfs[slice][slot].enable = 0U;
                        list->udfs[slice][slot].address = 0U;
                    }
                }
                line = __LINE__;
                break;
            }
        }
    } /* All slices */

    if ((FALSE == ruleProgrammed) && (BCM_ERR_OK == ret)) {
        ret = BCM_ERR_NOMEM;
        line = __LINE__;
    }
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVPROGRAMRULE_PART_PROC, ret,
                        line, row, slice, ruleProgrammed);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == RESET
        reset and clear CFP
        enable CFP
        validate packet length correction
        set the packet length correction in rate meter global control register
        enable global rate meter
        set state = INITIALIZED
        for all rules in config: 0->ruleListSz
            add rule
    @endcode
*/
int32_t CFP_DrvInit(ETHERSWT_HwIDType aID,
                    const CFP_ConfigType* const aConfig)
{
    uint64_t regVal;
    uint32_t line;
    uint32_t row   = CFP_MAX_RULES;
    uint32_t slice = CFP_MAX_SLICES;
    int32_t  ret   = BCM_ERR_OK;
    uint32_t i     = 0UL;

    if (CFP_STATE_RESET != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    if (aConfig->ruleListSz > CFP_MAX_RULES) {
        ret     = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto err_exit;
    }

    ret = CFP_DrvResetAndClearTCAM(aID);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        goto err_exit;
    }

    ret = CFP_DrvValidatePktLenCorr(aID, aConfig->pktLenCorr);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        goto err_exit;
    }

    /* Rate Meter Global Ctrl */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter_global_ctl,
                               &regVal));
    regVal |= (aConfig->pktLenCorr << SWITCH_PA0RMGC_PAGE_A0_RATE_METER_GLOBAL_CTL_PKT_LEN_CORR_SHIFT) &
               SWITCH_PA0RMGC_PAGE_A0_RATE_METER_GLOBAL_CTL_PKT_LEN_CORR_MASK;

    regVal |= SWITCH_PA0RMGC_PA0RMGCRRE_MASK;

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter_global_ctl,
                                regVal));

    for (; i < aConfig->ruleListSz; ++i) {
        row   = (aConfig->ruleList[i].rowAndSlice & CFP_ROWANDSLICE_ROW_MASK) >> CFP_ROWANDSLICE_ROW_SHIFT;
        slice = (aConfig->ruleList[i].rowAndSlice & CFP_ROWANDSLICE_SLICE_MASK) >> CFP_ROWANDSLICE_SLICE_SHIFT;
        ret = CFP_DrvProgramRule(aID, &aConfig->ruleList[i].key, &aConfig->ruleList[i].action, &row, &slice);
        if (BCM_ERR_OK != ret) {
            line = __LINE__;
            break;
        }
        /* Rules received during initialization are treated as RO */
        CFP_Handle.rules[row] |= CFP_RULEDATA_STATIC_MASK;
    }

    if (BCM_ERR_OK == ret) {
        /* Enable CFP feature on ports enabled in the config */
        regVal = SWITCH_PAGE_A1_CFP_CTL_REG_EN_MAP_MASK & aConfig->portEnableMask;
        CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, regVal));

        /* Initialize the state */
        CFP_Handle.state = CFP_STATE_INITIALIZED;
    }

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVINIT_PROC, ret,
                           line, i, row, slice);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        program rule
    @endcode
*/
int32_t CFP_DrvAddRule(ETHERSWT_HwIDType aID,
                       const CFP_KeyType* const aKey,
                       const CFP_ActionType* const aAction,
                       uint32_t* const aRow,
                       uint32_t* const aSlice)
{
    int32_t  ret  = BCM_ERR_OK;
    uint32_t line = 0UL;

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret  = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(CFP_DrvProgramRule(aID, aKey, aAction, aRow, aSlice));
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVADDRULE_PROC, ret,
                           line, *aRow, *aSlice, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        if rule exists
            invalidate TCAM entry
            clear the green statistics counter
            clear the yellow statistics counter
            clear the green statistics counter
            for all UDFs in the slice: 0->CFP_MAX_UDFS
                if rulesAssociated bitmap has rowNum set
                    clear rowNum from rulesAssociated bitmap
                    if rulesAssociated bitmap is zero
                        remove UDF from H/W
                        remove UDF from S/W cache
            remove rule from S/W cache
            decrement rule count
    @endcode
*/
int32_t CFP_DrvRemoveRule(ETHERSWT_HwIDType aID,
                          uint32_t aRow)
{
    int32_t              ret   = BCM_ERR_OK;
    CFP_UDFAllocListType *list = NULL;
    uint32_t             format;
    uint32_t             udf;
    uint32_t             udfMap;
    uint8_t              slice;
    uint32_t             i;
    uint32_t             total;
    uint32_t             line;
    CFP_StatsType        stats = {0UL, 0UL, 0UL};

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(CFP_DrvCheckIfRuleExists(aRow));

    if (BCM_ERR_OK == CFP_DrvCheckIfRuleIsStatic(aRow)) {
        ret = BCM_ERR_NOPERM;
        line = __LINE__;
        goto err_exit;
    }

    /* First remove from the HW */
    /* According to ASIC team, only need to invalidate the entry */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0, 0ULL));
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask0, 0ULL));

    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_WR << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));
    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* Clear the statistics RAM so that the counters can be reused if the */
    /* row gets enabled in the future                                     */
    CFP_ERR_EXIT(CFP_DrvSetStats(aID, aRow, &stats));

    /* Check if any UDFs can be freed up. Need to restrict the search to the */
    /* particular format and slice                                           */
    format = (CFP_Handle.rules[aRow] & CFP_RULEDATA_FORMAT_MASK) >> CFP_RULEDATA_FORMAT_SHIFT;
    slice  = (CFP_Handle.rules[aRow] & CFP_RULEDATA_SLICE_MASK)  >> CFP_RULEDATA_SLICE_SHIFT;
    udfMap = (CFP_Handle.rules[aRow] & CFP_RULEDATA_UDFMAP_MASK) >> CFP_RULEDATA_UDFMAP_SHIFT;
    list   = &CFP_Handle.udfList[format];

    /* Remove rule from SW */
    CFP_Handle.rules[aRow] = 0U;
    CFP_Handle.numRules--;

    /* Check if any UDFs can be freed */
    for (udf = 0UL ; udf < CFP_MAX_UDFS; ++udf) {
        if (0UL != (udfMap & (1UL << udf))) {
            /* Check if there are any leftover rules using this UDF */
            total = 0UL;
            for (i = 0UL; i < CFP_MAX_RULES; ++i) {
                if ((BCM_ERR_OK == CFP_DrvCheckIfRuleExists(i)) &&
                    (0UL != (((CFP_Handle.rules[i] & CFP_RULEDATA_UDFMAP_MASK) >> CFP_RULEDATA_UDFMAP_SHIFT) & (1UL << udf)))) {
                    total++;
                    break;
                }
            }
            if (0UL == total) {
                /* Remove UDF from HW */
                CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_udf_0_a_0 +
                            (format  * 0x30UL) +
                            (slice * 0x10UL) + udf, 0ULL));

                /* Remove UDF from SW */
                list->udfs[slice][udf].enable  = 0U;
                list->udfs[slice][udf].address = 0U;
            }
        }
    }
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVREMOVERULE_PROC, ret,
                        line, aRow, (uint32_t)CFP_Handle.state, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        if rule exists and is not static
            program action policy RAM
    @endcode
*/
int32_t CFP_DrvUpdateRule(ETHERSWT_HwIDType aID,
                          uint32_t aRow,
                          const CFP_ActionType* const aAction)
{
    CFP_StatsType stats = {0UL, 0UL, 0UL};
    int32_t       ret = BCM_ERR_OK;
    uint32_t      line;

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(CFP_DrvCheckIfRuleExists(aRow));

    if (BCM_ERR_OK == CFP_DrvCheckIfRuleIsStatic(aRow)) {
        ret = BCM_ERR_NOPERM;
        line = __LINE__;
        goto err_exit;
    }

    /* First disable the rule */
    CFP_ERR_EXIT(CFP_DrvSetRuleValid(aID, aRow, FALSE));
    /* Reset the statistics */
    CFP_ERR_EXIT(CFP_DrvSetStats(aID, aRow, &stats));
    CFP_ERR_EXIT(CFP_DrvProgramActionPolicyRAM(aID, aRow, aAction));
    CFP_ERR_EXIT(CFP_DrvProgramRateMeterRAM(aID, aRow, &aAction->meter));
    /* Re-enable the rule */
    CFP_ERR_EXIT(CFP_DrvSetRuleValid(aID, aRow, TRUE));
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVUPDATERULE_PROC, ret,
                        line, aRow, (uint32_t)CFP_Handle.state, 0UL);
    }
    return ret;
}

static int32_t CFP_DrvGetSnapshot(ETHERSWT_HwIDType aID,
                                  CFP_TableSnapshotType *const aSnapshot)
{
    uint64_t regVal;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line;
    uint32_t i;

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    aSnapshot->numValidEntries = CFP_Handle.numRules;
    for (i = 0UL; i < CFP_MAX_RULES; ++i) {
        aSnapshot->entry[i] = (CFP_EntrySnapshotType)((CFP_Handle.rules[i] >>
                               CFP_RULEDATA_FORMAT_SHIFT) << CFP_ENTRYSNAPSHOT_FORMAT_SHIFT);
    }

    BCM_MemCpy(&aSnapshot->udfList, &CFP_Handle.udfList, sizeof(aSnapshot->udfList));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, &regVal));
    aSnapshot->portEnableMask = (uint16_t)regVal & SWITCH_PAGE_A1_CFP_CTL_REG_EN_MAP_MASK;

err_exit:
    if (BCM_ERR_OK != ret)  {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVGETSNAPSHOT_PART_PROC, ret,
                           line, (uint32_t)CFP_Handle.state, 0UL, 0UL);
    }
    return ret;

}

static int32_t CFP_DrvGetRowConfig(ETHERSWT_HwIDType aID,
                                   uint32_t aRow,
                                   CFP_RuleType *const aConfig)
{
    uint64_t regVal;
    uint64_t regVal2;
    uint32_t counter;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line;
    uint32_t i;
    uint32_t format;
    uint16_t udfValid;
    uint32_t slice = CFP_MAX_SLICES;
    CFP_AcceptableFrameType frame;

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(CFP_DrvCheckIfRuleExists(aRow));

    /* Initialize the data structures */
    BCM_MemSet(aConfig, 0U, sizeof(CFP_RuleType));

    /* Read the Key */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_TCAM << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK) |
                SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK
                ));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* DATA0 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data0, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask0, &regVal2));

    if (((uint32_t)regVal2 & SWITCH_CFP_DATA0_SLICEID_MASK) >> SWITCH_CFP_DATA0_SLICEID_SHIFT) {
        slice = ((uint32_t)regVal & SWITCH_CFP_DATA0_SLICEID_MASK) >> SWITCH_CFP_DATA0_SLICEID_SHIFT;
        aConfig->rowAndSlice = (slice <<CFP_ROWANDSLICE_SLICE_SHIFT) & CFP_ROWANDSLICE_SLICE_MASK;;
    } else {
        goto err_exit;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA0_UDF0_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA0_UDF0_MASK) >> SWITCH_CFP_DATA0_UDF0_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA0_UDF0_MASK) >> SWITCH_CFP_DATA0_UDF0_SHIFT;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA0_UDF1_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA0_UDF1_MASK) >> SWITCH_CFP_DATA0_UDF1_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA0_UDF1_MASK) >> SWITCH_CFP_DATA0_UDF1_SHIFT;
    }

    /* DATA1 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data1, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask1, &regVal2));

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF1_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value |= (((uint32_t)regVal & SWITCH_CFP_DATA1_UDF1_MASK) >> SWITCH_CFP_DATA1_UDF1_SHIFT) << 8UL;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  |= (((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF1_MASK) >> SWITCH_CFP_DATA1_UDF1_SHIFT) << 8UL;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF2_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA1_UDF2_MASK) >> SWITCH_CFP_DATA1_UDF2_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF2_MASK) >> SWITCH_CFP_DATA1_UDF2_SHIFT;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF3_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA1_UDF3_MASK) >> SWITCH_CFP_DATA1_UDF3_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA1_UDF3_MASK) >> SWITCH_CFP_DATA1_UDF3_SHIFT;
    }

    /* DATA2 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data2, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask2, &regVal2));

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF3_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value |= (((uint32_t)regVal & SWITCH_CFP_DATA2_UDF3_MASK) >> SWITCH_CFP_DATA2_UDF3_SHIFT) << 8UL;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  |= (((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF3_MASK) >> SWITCH_CFP_DATA2_UDF3_SHIFT) << 8UL;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF4_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA2_UDF4_MASK) >> SWITCH_CFP_DATA2_UDF4_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF4_MASK) >> SWITCH_CFP_DATA2_UDF4_SHIFT;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF5_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA2_UDF5_MASK) >> SWITCH_CFP_DATA2_UDF5_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA2_UDF5_MASK) >> SWITCH_CFP_DATA2_UDF5_SHIFT;
    }

    /* DATA3 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data3, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask3, &regVal2));

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF5_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value |= (((uint32_t)regVal & SWITCH_CFP_DATA3_UDF5_MASK) >> SWITCH_CFP_DATA3_UDF5_SHIFT) << 8UL;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  |= (((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF5_MASK) >> SWITCH_CFP_DATA3_UDF5_SHIFT) << 8UL;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF6_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA3_UDF6_MASK) >> SWITCH_CFP_DATA3_UDF6_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF6_MASK) >> SWITCH_CFP_DATA3_UDF6_SHIFT;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF7_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA3_UDF7_MASK) >> SWITCH_CFP_DATA3_UDF7_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA3_UDF7_MASK) >> SWITCH_CFP_DATA3_UDF7_SHIFT;
    }

    /* DATA4 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data4, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask4, &regVal2));

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA4_UDF7_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value |= (((uint32_t)regVal & SWITCH_CFP_DATA4_UDF7_MASK) >> SWITCH_CFP_DATA4_UDF7_SHIFT) << 8UL;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  |= (((uint32_t)regVal2 & SWITCH_CFP_DATA4_UDF7_MASK) >> SWITCH_CFP_DATA4_UDF7_SHIFT) << 8UL;
        aConfig->key.numEnabledUDFs++;
    }

    if (0U != ((uint32_t)regVal2 & SWITCH_CFP_DATA4_UDF8_MASK)) {
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].value = ((uint32_t)regVal & SWITCH_CFP_DATA4_UDF8_MASK) >> SWITCH_CFP_DATA4_UDF8_SHIFT;
        aConfig->key.udfList[aConfig->key.numEnabledUDFs].mask  = ((uint32_t)regVal2 & SWITCH_CFP_DATA4_UDF8_MASK) >> SWITCH_CFP_DATA4_UDF8_SHIFT;
        aConfig->key.numEnabledUDFs++;
    }

    if (0UL != ((uint32_t)regVal2 & SWITCH_CFP_DATA4_CTAG_MASK)) {
        aConfig->key.cTagFlags = (((uint32_t)regVal & SWITCH_CFP_DATA4_CTAG_MASK) >> SWITCH_CFP_DATA4_CTAG_SHIFT) << CFP_KEY_TAG_ID_SHIFT;
        aConfig->key.cTagMask = (((uint32_t)regVal2 & SWITCH_CFP_DATA4_CTAG_MASK) >> SWITCH_CFP_DATA4_CTAG_SHIFT) << CFP_KEY_TAG_IDMASK_SHIFT;
        aConfig->key.cTagFlags |= CFP_KEY_TAG_ID_VALID_MASK;
    }

    /* DATA5 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data5, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask5, &regVal2));

    if (0x10UL == ((uint32_t)regVal2 & 0x10UL)) {
        aConfig->key.cTagFlags |= CFP_KEY_TAG_DEI_VALID_MASK;
        aConfig->key.cTagFlags |= ((uint32_t) regVal & 0x10UL) << 10UL;
    }
    if (0UL != ((uint32_t)regVal2 & 0xE0UL)) {
        aConfig->key.cTagFlags |= CFP_KEY_TAG_PCP_VALID_MASK;
        aConfig->key.cTagFlags |= ((uint32_t) regVal & 0xE0UL) << 10UL;
        aConfig->key.cTagMask  |= (((uint32_t) regVal2 & 0xE0UL) >> 5UL) << CFP_KEY_TAG_PCPMASK_SHIFT;
    }
    if (0UL != ((uint32_t)regVal2 & 0x0FUL)) {
        aConfig->key.cTagFlags |= (((uint32_t)regVal & 0x0FUL) >> SWITCH_CFP_DATA5_CTAG_SHIFT) << 10UL;
        aConfig->key.cTagMask  |= (((uint32_t)regVal2 & 0x0FUL) >> SWITCH_CFP_DATA5_CTAG_SHIFT) << 8UL;
        aConfig->key.cTagFlags |= CFP_KEY_TAG_ID_VALID_MASK;
    }

    if (0UL != ((uint32_t)regVal2 & 0x100000UL)) {
        aConfig->key.sTagFlags  = CFP_KEY_TAG_DEI_VALID_MASK;
        aConfig->key.sTagFlags |= (((uint32_t) regVal & 0x100000UL) >> 20UL) << CFP_KEY_TAG_DEI_SHIFT;
    }

    if (0UL != ((uint32_t)regVal2 & 0xE00000UL)) {
        aConfig->key.sTagFlags |= CFP_KEY_TAG_PCP_VALID_MASK;
        aConfig->key.sTagFlags |= (((uint32_t) regVal & 0xE00000UL) >> 21UL) << CFP_KEY_TAG_PCP_SHIFT;
        aConfig->key.sTagMask   = (((uint32_t) regVal2 & 0xE00000UL) >> 21UL) << CFP_KEY_TAG_PCPMASK_SHIFT;
    }

    if (0UL != ((uint32_t)regVal2 & 0xFFF00UL)) {
        aConfig->key.sTagFlags |= (((uint32_t)regVal & SWITCH_CFP_DATA5_STAG_MASK) >> SWITCH_CFP_DATA5_STAG_SHIFT) << CFP_KEY_TAG_ID_SHIFT;
        aConfig->key.sTagMask  |= (((uint32_t)regVal2 & SWITCH_CFP_DATA5_STAG_MASK) >> SWITCH_CFP_DATA5_STAG_SHIFT) << CFP_KEY_TAG_IDMASK_SHIFT;
        aConfig->key.sTagFlags |= CFP_KEY_TAG_ID_VALID_MASK;
    }

    udfValid = ((uint32_t)regVal & SWITCH_CFP_DATA5_UDFVALID_7_0_MASK) >> SWITCH_CFP_DATA5_UDFVALID_7_0_SHIFT;
    udfValid &= (((uint32_t)regVal2 & SWITCH_CFP_DATA5_UDFVALID_7_0_MASK) >> SWITCH_CFP_DATA5_UDFVALID_7_0_SHIFT);

    /* DATA6 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_data6, &regVal));
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask6, &regVal2));

    if (SWITCH_CFP_DATA6_UDFVALID_8_MASK == ((uint32_t)regVal2 & SWITCH_CFP_DATA6_UDFVALID_8_MASK)) {
        udfValid |= ((((uint32_t)regVal & SWITCH_CFP_DATA6_UDFVALID_8_MASK) >> SWITCH_CFP_DATA6_UDFVALID_8_SHIFT) << 8UL);
    }

    aConfig->key.l2Framing  = ((uint32_t)regVal & SWITCH_CFP_DATA6_L2FRAMING_MASK) >> SWITCH_CFP_DATA6_L2FRAMING_SHIFT;
    aConfig->key.l3Framing  = ((uint32_t)regVal & SWITCH_CFP_DATA6_L3FRAMING_MASK) >> SWITCH_CFP_DATA6_L3FRAMING_SHIFT;

    switch(aConfig->key.l3Framing) {
        case CFP_L3FRAMING_IPV4:
            /* Fall through intentional */
        case CFP_L3FRAMING_IPV6:
            aConfig->key.flagsMask = ((uint32_t)regVal2 >> SWITCH_CFP_DATA6_TTL_SHIFT) & CFP_KEY_IP_MASK;
            aConfig->key.flags     = ((uint32_t)regVal >> SWITCH_CFP_DATA6_TTL_SHIFT) & CFP_KEY_IP_MASK;
            break;
        case CFP_L3FRAMING_NONIP:
            aConfig->key.flagsMask = ((uint32_t)regVal2 >> SWITCH_CFP_DATA6_ETHTYPE_SHIFT) & CFP_KEY_NONIP_MASK;
            aConfig->key.flags     = ((uint32_t)regVal >> SWITCH_CFP_DATA6_ETHTYPE_SHIFT) & CFP_KEY_NONIP_MASK;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            line = __LINE__;
            break;
    }
    if (ret != BCM_ERR_OK) {
        goto err_exit;
    }

    frame = CFP_DrvTagStatustoAFT(aID,(((uint32_t)regVal & SWITCH_CFP_DATA6_CTAGSTATUS_MASK) >> SWITCH_CFP_DATA6_CTAGSTATUS_SHIFT),
             (((uint32_t)regVal2 & SWITCH_CFP_DATA6_CTAGSTATUS_MASK) >> SWITCH_CFP_DATA6_CTAGSTATUS_SHIFT));

    aConfig->key.cTagFlags |= frame << CFP_KEY_TAG_UN_TAGGED_SHIFT;

    frame = CFP_DrvTagStatustoAFT(aID, (((uint32_t)regVal & SWITCH_CFP_DATA6_STAGSTATUS_MASK) >> SWITCH_CFP_DATA6_STAGSTATUS_SHIFT),
             (((uint32_t)regVal2 & SWITCH_CFP_DATA6_STAGSTATUS_MASK) >> SWITCH_CFP_DATA6_STAGSTATUS_SHIFT));

    aConfig->key.sTagFlags |= frame << CFP_KEY_TAG_UN_TAGGED_SHIFT;

    /* DATA7 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_mask7, &regVal2));
    aConfig->key.ingressPortBitmap = (~((uint32_t)regVal2 >> SWITCH_CFP_DATA7_SRCPRTMAP_SHIFT) & SWITCH_CFP_DATA7_SRCPRTMAP_MASK);

    CFP_ERR_EXIT(CFP_DrvGetFormat(aConfig->key.l3Framing, &format));

    /* Get the UDFs */
    counter = 0UL;
    for (i = 0UL; i < CFP_MAX_UDFS; ++i) {
        if (0UL != ((1UL << i) & udfValid)) {
            ret = SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a1_udf_0_a_0 +
                                     (format * 0x30UL) +
                                     (slice * 0x10UL) + i, &regVal);
            if (BCM_ERR_OK == ret) {
                aConfig->key.udfList[counter++].baseAndOffset = (uint8_t)regVal;
            }
        }
    }

    /* Read the action policy RAM */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
            ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
            ((SWITCH_CFP_ACC_RAM_SEL_ACT_POL << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
            SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
            ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)|
            SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* ACT_POL_DATA0 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data0, &regVal));
    aConfig->action.otherFlags = ((uint32_t)regVal & SWITCH_CFP_ACT_POL_DATA0_BYPASS_MASK) >> SWITCH_CFP_ACT_POL_DATA0_BYPASS_SHIFT;
    aConfig->action.otherFlags |= ((((uint32_t)regVal & SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_LOOP_BK_EN_MASK) >>
                                  SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_LOOP_BK_EN_SHIFT) << CFP_ACTION_LPBK_EN_SHIFT) &
                                  CFP_ACTION_LPBK_EN_MASK;

    aConfig->action.reasonCode = ((uint32_t)regVal & SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_REASON_CODE_MASK) >> SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_REASON_CODE_SHIFT;
    aConfig->action.tcFlags = ((uint32_t)regVal & SWITCH_CFP_ACT_POL_DATA0_TC_MASK) >> SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_TC_SHIFT;
    aConfig->action.dstMapIBFlags = ((uint32_t)regVal & SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_DST_MAP_IB_MASK) >> SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_DST_MAP_IB_SHIFT;
    aConfig->action.dstMapIBFlags |= (((uint32_t)regVal & SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_CHANGE_FWRD_MAP_IB_MASK) >>
                                     SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_CHANGE_FWRD_MAP_IB_SHIFT) << CFP_ACTION_CHANGE_FWDMAP_SHIFT;
    aConfig->action.tosIBFlags = ((uint32_t)regVal & SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_DSCP_IB_MASK) >> SWITCH_PA0APD0_PAGE_A0_ACT_POL_DATA0_NEW_DSCP_IB_SHIFT;

    /* ACT_POL_DATA1 */
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_act_pol_data1, &regVal));

    aConfig->action.tosIBFlags |= ((((uint32_t)regVal & SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_DSCP_IB_MASK) >>
                                   SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_DSCP_IB_SHIFT) << CFP_ACTION_CHANGE_TOS_SHIFT) &
                                   CFP_ACTION_CHANGE_TOS_MASK;
    aConfig->action.dstMapOBFlags = ((uint32_t)regVal & SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_DST_MAP_OB_MASK) >> SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_DST_MAP_OB_SHIFT;
    aConfig->action.dstMapOBFlags |= (((uint32_t)regVal & SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_FWRD_MAP_OB_MASK) >>
                                     SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_FWRD_MAP_OB_SHIFT) << CFP_ACTION_CHANGE_FWDMAP_SHIFT;
    aConfig->action.tosOBFlags = ((uint32_t)regVal & SWITCH_CFP_ACT_POL_DATA1_TOS_MASK) >> SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_NEW_DSCP_OB_SHIFT;
    aConfig->action.chainID = ((uint32_t)regVal & SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHAIN_ID_MASK) >> SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHAIN_ID_SHIFT;
    aConfig->action.colorFlags = ((uint32_t)regVal & SWITCH_CFP_ACT_POL_DATA1_COLOR_MASK) >> SWITCH_PA0APD1_PAGE_A0_ACT_POL_DATA1_CHANGE_COLOR_SHIFT;
    aConfig->action.otherFlags |= ((((uint32_t)regVal & SWITCH_CFP_ACT_POL_DATA1_REDDFLT_MASK) >>
                                   SWITCH_CFP_ACT_POL_DATA1_REDDFLT_SHIFT) << CFP_ACTION_USE_DFLT_RED_SHIFT) &
                                   CFP_ACTION_USE_DFLT_RED_MASK;

    /* Read rate meter */
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a0_cfp_acc,
                ((aRow << SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_SHIFT) & SWITCH_PAGE_A0_CFP_ACC_XCESS_ADDR_MASK)|
                ((SWITCH_CFP_ACC_RAM_SEL_RATE_METER << SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_RAM_SEL_MASK) |
                ((SWITCH_CFP_ACC_OP_SEL_RD << SWITCH_PAGE_A0_CFP_ACC_OP_SEL_SHIFT) &
                SWITCH_PAGE_A0_CFP_ACC_OP_SEL_MASK)| SWITCH_PAGE_A0_CFP_ACC_OP_STR_DONE_MASK));

    CFP_ERR_EXIT(CFP_DrvWaitForAccess(aID));

    /* RATE_METER0*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter0, &regVal));
    aConfig->action.meter.policerFlags = (uint32_t)regVal & SWITCH_CFP_RATE_METER0_MASK;

    /* RATE_METER1*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter1, &regVal));
    aConfig->action.meter.eirTkBkt = (uint32_t)regVal & SWITCH_PAGE_A0_RATE_METER1_EIR_TK_BKT_MASK;

    /* RATEMETER2*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter2, &regVal));
    aConfig->action.meter.eirBktSize = (uint32_t)regVal & SWITCH_PA0RM2_PAGE_A0_RATE_METER2_EIR_BKT_SIZE_MASK;

    /* RATEMETER3*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter3, &regVal));
    aConfig->action.meter.eirRefCnt = (uint32_t)regVal & SWITCH_PA0RM3_PAGE_A0_RATE_METER3_EIR_REF_CNT_MASK;

    /* RATEMETER4*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter4, &regVal));
    aConfig->action.meter.cirTkBkt = (uint32_t)regVal & SWITCH_PAGE_A0_RATE_METER4_CIR_TK_BKT_MASK;

    /* RATEMETER5*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter5, &regVal));
    aConfig->action.meter.cirBktSize = (uint32_t)regVal & SWITCH_PA0RM5_PAGE_A0_RATE_METER5_CIR_BKT_SIZE_MASK;

    /* RATEMETER6*/
    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a0_rate_meter6, &regVal));
    aConfig->action.meter.cirRefCnt = (uint32_t)regVal & SWITCH_PA0RM6_PAGE_A0_RATE_METER6_CIR_REF_CNT_MASK;

err_exit:
    if ((BCM_ERR_OK != ret) && (BCM_ERR_NOT_FOUND != ret)) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVGETROWCONFIG_PART_PROC, ret,
                           line, aRow, (uint32_t)CFP_Handle.state, slice);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        set bit corresponding to port in CFP Control register
    @endcode
*/
int32_t CFP_DrvEnablePort(ETHERSWT_HwIDType aID,
                          uint32_t aPort)
{
    uint64_t regVal;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line;

    if (aPort >= ETHERSWT_PORT_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto err_exit;
    }

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, &regVal));
    regVal |= (1UL << aPort);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, regVal));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVENABLEPORT_PROC, ret,
                           line, aPort, (uint32_t)CFP_Handle.state, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state == INITIALIZED
        clear bit corresponding to port in CFP Control register
    @endcode
*/
int32_t CFP_DrvDisablePort(ETHERSWT_HwIDType aID,
                           uint32_t aPort)
{
    uint64_t regVal;
    int32_t  ret = BCM_ERR_OK;
    uint32_t line;

    if (aPort >= ETHERSWT_PORT_ID_MAX) {
        ret = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
        goto err_exit;
    }

    if (CFP_STATE_INITIALIZED != CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    CFP_ERR_EXIT(SwitchDrv_ReadReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, &regVal));
    regVal &= ~(1UL << aPort);
    CFP_ERR_EXIT(SwitchDrv_WriteReg(aID, (uint32_t)&SWITCH_REGS->a1_cfp_ctl_reg, regVal));

err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVDISABLEPORT_PROC, ret,
                        line, aPort, (uint32_t)CFP_Handle.state, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if state != RESET
        reset and clear CFP
        set state = RESET
    @endcode
*/
int32_t CFP_DrvDeInit(ETHERSWT_HwIDType aID)
{
    uint32_t line;
    int32_t  ret     = BCM_ERR_OK;

    if (CFP_STATE_RESET == CFP_Handle.state) {
        ret = BCM_ERR_INVAL_STATE;
        line = __LINE__;
        goto err_exit;
    }

    ret = CFP_DrvResetAndClearTCAM(aID);
    if (BCM_ERR_OK != ret) {
        line = __LINE__;
        goto err_exit;
    }

    /* Update the state */
    BCM_MemSet(&CFP_Handle, 0U, sizeof(CFP_Handle));
    CFP_Handle.state = CFP_STATE_RESET;
err_exit:
    if (BCM_ERR_OK != ret) {
        CFP_DrvReportError(BCM_CFP_ID, aID, BRCM_SWDSGN_CFP_DRVDEINIT_PROC, ret,
                        line, (uint32_t)CFP_Handle.state, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    return &CFP_Handle
    @endcode
*/
const CFP_ContextType* CFP_DrvGetHandle(void)
{
    return &CFP_Handle;
}

/**
    @code{.unparsed}
    if aID is valid and aConfig is non-NULL
        if aConfig magic is valid
            Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_INIT_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_Init(ETHERSWT_HwIDType aID,
                 const CFP_ConfigType* const aConfig)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;
    uint32_t magic = 0UL;
    if ((aID < SWITCH_MAX_HW_ID) && (NULL != aConfig)){
        if (CFP_CONFIG_MAGIC_ID == aConfig->magicId) {
            swtIO.retVal  = BCM_ERR_UNKNOWN;
            swtIO.swtHwID = aID;
            swtIO.cfpCfg  = aConfig;
            ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_INIT, &swtIO);
        } else {
            ret = BCM_ERR_INVAL_MAGIC;
            magic = aConfig->magicId;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_INIT_PROC, ret,
                         __LINE__, (uint32_t) aConfig, magic, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aKey, aAction, aRow and aSlice are non-NULL
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_ADDRULE_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_AddRule(ETHERSWT_HwIDType aID,
                    const CFP_KeyType* const aKey,
                    const CFP_ActionType* const aAction,
                    uint32_t* const aRow,
                    uint32_t* const aSlice)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (NULL != aKey) &&
        (NULL != aAction) && (NULL != aRow) && (NULL != aSlice)) {
        swtIO.retVal    = BCM_ERR_UNKNOWN;
        swtIO.swtHwID   = aID;
        swtIO.keyCfg    = aKey;
        swtIO.actionCfg = aAction;
        swtIO.row       = aRow;
        swtIO.slice     = aSlice;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_ADDRULE, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_ADDRULE_PROC, ret,
                         (uint32_t) aKey, (uint32_t) aAction,
                         (uint32_t) *aRow, (uint32_t) *aSlice);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aRow is within limits
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_REMOVERULE_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_RemoveRule(ETHERSWT_HwIDType aID,
                       uint32_t aRow)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aRow < CFP_MAX_RULES)) {
        swtIO.retVal  = BCM_ERR_UNKNOWN;
        swtIO.swtHwID = aID;
        swtIO.row     = &aRow;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_REMOVERULE, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_REMOVERULE_PROC, ret,
                         __LINE__, aRow, 0UL, 0uL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aRow is within limits and aAction is non-NULL
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_UPDATERULE_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_UpdateRule(ETHERSWT_HwIDType aID,
                       uint32_t aRow,
                       const CFP_ActionType* const aAction)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aRow < CFP_MAX_RULES) &&
        (NULL != aAction)) {
        swtIO.retVal    = BCM_ERR_UNKNOWN;
        swtIO.swtHwID   = aID;
        swtIO.row       = &aRow;
        swtIO.actionCfg = aAction;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_UPDATERULE, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_UPDATERULE_PROC, ret,
                         __LINE__, aRow, (uint32_t) aAction, 0uL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aRow is within limits and aStats is non-NULL
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_GETSTATS_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_GetStats(ETHERSWT_HwIDType aID,
                     uint32_t aRow,
                     CFP_StatsType *const aStats)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aRow < CFP_MAX_RULES) && (NULL != aStats)) {
        swtIO.retVal     = BCM_ERR_UNKNOWN;
        swtIO.swtHwID    = aID;
        swtIO.row        = &aRow;
        swtIO.cfpStats   = aStats;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_GETSTATS, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_GETSTATS_PROC, ret,
                         aRow, (uint32_t) aStats, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aRow is within limits and aConfig is non-NULL
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_GETROWCONFIG_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_GetRowConfig(ETHERSWT_HwIDType aID,
                         uint32_t aRow,
                         CFP_RuleType *const aConfig)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aRow < CFP_MAX_RULES) && (NULL != aConfig)) {
        swtIO.retVal     = BCM_ERR_UNKNOWN;
        swtIO.swtHwID    = aID;
        swtIO.row        = &aRow;
        swtIO.ruleCfg    = aConfig;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_GETROWCONFIG, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if ((ret != BCM_ERR_OK) && (ret != BCM_ERR_NOT_FOUND))  {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_GETROWCONFIG_PROC, ret,
                         aRow, (uint32_t) aConfig, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aRow is within limits and aConfig is non-NULL
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_GETSNAPSHOT_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_GetSnapshot(ETHERSWT_HwIDType aID,
                        CFP_TableSnapshotType *const aSnapshot)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (NULL != aSnapshot)) {
        swtIO.retVal     = BCM_ERR_UNKNOWN;
        swtIO.swtHwID    = aID;
        swtIO.snapShot   = aSnapshot;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_GETSNAPSHOT, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if ((ret != BCM_ERR_OK) && (ret != BCM_ERR_NOT_FOUND))  {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_GETSNAPSHOT_PROC, ret,
                         (uint32_t) aSnapshot, 0UL, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aPortNum is valid
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_ENABLEPORT_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_EnablePort(ETHERSWT_HwIDType aID,
                       uint32_t aPortNum)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aPortNum < ETHERSWT_PORT_ID_MAX)) {
        swtIO.retVal    = BCM_ERR_UNKNOWN;
        swtIO.swtHwID   = aID;
        swtIO.portHwID  = aPortNum;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_ENABLEPORT, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_ENABLEPORT_PROC, ret,
                         aPortNum, 0UL, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid and aPortNum is valid
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_DISABLEPORT_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_DisablePort(ETHERSWT_HwIDType aID,
                       uint32_t aPortNum)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if ((aID < SWITCH_MAX_HW_ID) && (aPortNum < ETHERSWT_PORT_ID_MAX)) {
        swtIO.retVal    = BCM_ERR_UNKNOWN;
        swtIO.swtHwID   = aID;
        swtIO.portHwID  = aPortNum;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_DISABLEPORT, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_DISABLEPORT_PROC, ret,
                         aPortNum, 0UL, 0UL, 0UL);
    }
    return ret;
}

/**
    @code{.unparsed}
    if aID is valid
        Invoke SVC
    @endcode

    @trace #BRCM_SWARCH_CFP_DEINIT_PROC
    @trace #BRCM_SWREQ_CFP
*/
int32_t CFP_DeInit(ETHERSWT_HwIDType aID)
{
    EthSwtIO swtIO;
    int32_t  ret = BCM_ERR_INVAL_PARAMS;

    if (aID < SWITCH_MAX_HW_ID) {
        swtIO.retVal  = BCM_ERR_UNKNOWN;
        swtIO.swtHwID = aID;
        ret = ETHERSWT_SysCmdReq(SWT_IO_CMD_CFP_DEINIT, &swtIO);
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    if (ret != BCM_ERR_OK) {
        CFP_DrvReportError((BCM_CFP_ID & BCM_LOGMASK_USER), aID,
                         BRCM_SWARCH_CFP_DEINIT_PROC, ret,
                         __LINE__, 0UL, 0UL, 0UL);
    }
    return ret;
}

int32_t CFP_CmdHandler(ETHERSWT_CmdType aCmd, EthSwtIO *const aIO)
{
    int32_t ret = BCM_ERR_UNKNOWN;
    if (NULL != aIO) {
        switch (aCmd) {
            case SWT_IO_CMD_CFP_INIT:
                ret = CFP_DrvInit(aIO->swtHwID, aIO->cfpCfg);
                break;
            case SWT_IO_CMD_CFP_ADDRULE:
                ret = CFP_DrvAddRule(aIO->swtHwID, aIO->keyCfg, aIO->actionCfg, aIO->row, aIO->slice);
                break;
            case SWT_IO_CMD_CFP_REMOVERULE:
                ret = CFP_DrvRemoveRule(aIO->swtHwID, *aIO->row);
                break;
            case SWT_IO_CMD_CFP_UPDATERULE:
                ret = CFP_DrvUpdateRule(aIO->swtHwID, *aIO->row, aIO->actionCfg);
                break;
            case SWT_IO_CMD_CFP_GETSTATS:
                ret = CFP_DrvGetStats(aIO->swtHwID, *aIO->row, aIO->cfpStats);
                break;
            case SWT_IO_CMD_CFP_GETSNAPSHOT:
                ret = CFP_DrvGetSnapshot(aIO->swtHwID, aIO->snapShot);
                break;
            case SWT_IO_CMD_CFP_GETROWCONFIG:
                ret = CFP_DrvGetRowConfig(aIO->swtHwID, *aIO->row, aIO->ruleCfg);
                break;
            case SWT_IO_CMD_CFP_ENABLEPORT:
                ret = CFP_DrvEnablePort(aIO->swtHwID, aIO->portHwID);
                break;
            case SWT_IO_CMD_CFP_DISABLEPORT:
                ret = CFP_DrvDisablePort(aIO->swtHwID, aIO->portHwID);
                break;
            case SWT_IO_CMD_CFP_DEINIT:
                ret = CFP_DrvDeInit(aIO->swtHwID);
                break;
            default:
                ret = BCM_ERR_NOSUPPORT;
                break;
        }
    }
    return ret;
}
