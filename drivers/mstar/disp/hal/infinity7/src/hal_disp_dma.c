/*
* hal_disp_dma.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/

#define _HAL_DISP_DMA_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "hal_disp_irq.h"
#include "drv_disp_ctx.h"
#include "hal_disp_dma.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC,
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST,
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_NUM,
} HalDispDmaCmdqCtxType_e;
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

#define DISP_CHIP_REVISION_U01       0x01
#define DISP_CHIP_REVISION_U02       0x02

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
static u8 _HalDispDmaCheckDone(void *pCtx, HalDispUtilityRegAccessMode_e enRiuMode)
{
    HalDispUtilityCmdqContext_t *pCmdqCtx      = NULL;
    DrvDispCtxConfig_t *         pstDispCtxCfg = pCtx;
    DrvDispCtxDmaContain_t *     pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *  pstDevContain = NULL;
    u32                          u32Reg;
    u16                          u16Msk;
    u16                          u16Val = 0;
    u16                          u16CmdqInProcess;
    u8                           u8Ret = 1;

    HalDispGetCmdqByCtx(pCtx, (void *)&pCmdqCtx);
    if (pCmdqCtx)
    {
        u16CmdqInProcess =
            HalDispUtilityR2BYTEMaskDirect(REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(pCmdqCtx->s32UtilityId),
                                           REG_CMDQ_IN_PROCESS_MSK | REG_CMDQ_DEV_TIMEGEN_START_MSK);
        pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
        pstDevContain = pstDmaContain->pSrcDevContain;
        if (pstDevContain == NULL)
        {
            return 0;
        }
        u32Reg = HAL_DISP_IRQ_TOP_RAWFLAG_REG(pstDevContain->u32DevId);
        u16Msk = HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVE_MSK;
        u16Val = 0;
        if (enRiuMode == E_DISP_UTILITY_REG_ACCESS_CPU)
        {
            if (pstDmaContain->u8RingModeEn || ((u16CmdqInProcess & REG_CMDQ_IN_PROCESS_MSK) == REG_CMDQ_IN_PROCESS_ON))
            {
                if (HalDispUtilityR2BYTEMaskDirect(u32Reg, u16Msk) != u16Val)
                {
                    // wait for end
                    DISP_ERR("%s Check DMA, But DMA not frame done\n", __FUNCTION__);
                    u8Ret = 0;
                }
            }
        }
        else
        {
            u8Ret = 0;
        }
        if (!u8Ret)
        {
            u8Ret = HalDispUtilityWritePollCmdByType(u32Reg, u16Val, u16Msk, 2000000000, 1, pCmdqCtx, enRiuMode);
        }
    }
    return u8Ret;
}
void HalDispDmaEnsureIdle(void *pCtx, HalDispUtilityRegAccessMode_e enRiuMode)
{
    HalDispDmaSwRst(pCtx, E_HAL_DISP_DMA_RST_WOPATH_L, enRiuMode);
    _HalDispDmaCheckDone(pCtx, enRiuMode);
    HalDispDmaSwRst(pCtx, E_HAL_DISP_DMA_RST_WOPATH_H, enRiuMode);
}
u8 HalDispDmaCheckIdle(void *pCtx, HalDispUtilityRegAccessMode_e enRiuMode)
{
    return _HalDispDmaCheckDone(pCtx, enRiuMode);
}
void HalDispDmaSetFrmWEn(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_EN, bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_en, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWTriggerModeCfg(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIG_FRM_END_MASK_EN,
                                bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_trig_frm_end_mask_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trig_frm_end_mask_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER_MODE,
                                bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_mode : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_mode, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWTriggerModeEn(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER_DBF_EN,
                                bEn ? 0 : mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_dbf_en,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_dbf_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER_EN,
                                bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_en, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWTrigger(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;
    //u8   u8Cnt = 0;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx && bEn)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger, pCmdqCtx);
        /*
        if(CamOsChipRevision() == DISP_CHIP_REVISION_U01)
        {
            for(u8Cnt = 0;u8Cnt < HAL_DISP_RETRIGGER_CNT;u8Cnt++)
            {
                HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER,
                                        mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger,
                                        mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger, pCmdqCtx);
            }
        }*/
    }
}

u8 HalDispDmaCheckFrmWEn(void *pCtx)
{
    return HalDispUtilityR2BYTEDirect(DISP_CVBS_REG_SC_FRM_W_DMA_EN) & mask_of_disp_cvbs_reg_sc_frm_w_dma_en ? 1 : 0;
}
void HalDispDmaSetVsSel(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_VS_SEL, bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_vs_sel : 0x0000,
                            mask_of_disp_cvbs_reg_sc_frm_w_dma_vs_sel, pCmdqCtx);
}
void HalDispDmaSetRegMonitor(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_REGISTER_SETTING_MONITOR,
                                bEn ? mask_of_disp_cvbs_reg_register_setting_monitor : 0x0000,
                                mask_of_disp_cvbs_reg_register_setting_monitor, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWDbEn(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_DBF_EN,
                                bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_dbf_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_dbf_en, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWCFilter(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_C_FILTER, u16Val, mask_of_disp_cvbs_reg_sc_frm_w_c_filter,
                                pCmdqCtx);
    }
}

void HalDispDmaSetFrmWPixelFormat(u16 u16Val0, u16 u16Val1, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_422TO420_MD, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_422to420_md,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_420_PLANER_MD, u16Val0,
                                mask_of_disp_cvbs_reg_sc_frm_w_420_planer_md, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_422_SEP, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_422_sep,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_422_PLANER, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_422_planer,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_422_PACK, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_422_pack,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_32B_MD, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_32b_md,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_32B_MD_EXT, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_32b_md_ext,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_RGB565, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_rgb565,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(
            DISP_CVBS_REG_SC_FRM_W_DITH_EN,
            (u16Val0 & mask_of_disp_cvbs_reg_sc_frm_w_rgb565) ? mask_of_disp_cvbs_reg_sc_frm_w_dith_en : 0,
            mask_of_disp_cvbs_reg_sc_frm_w_dith_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_ARGB_MODE, u16Val0, mask_of_disp_cvbs_reg_sc_frm_w_argb_mode,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_A_VALUE, u16Val1, mask_of_disp_cvbs_reg_sc_frm_w_444_a_value,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_Y_LOCATION, u16Val1,
                                mask_of_disp_cvbs_reg_sc_frm_w_444_y_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_U_LOCATION, u16Val1,
                                mask_of_disp_cvbs_reg_sc_frm_w_444_u_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_V_LOCATION, u16Val1,
                                mask_of_disp_cvbs_reg_sc_frm_w_444_v_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_PACK, u16Val1, mask_of_disp_cvbs_reg_sc_frm_w_444_pack,
                                pCmdqCtx);
    }
}
void HalDispDmaSetCscY2RMatrix(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_Y_OFSET8, 0x1000,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_y_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_CR_OFSET8, 0x8000,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_cr_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_CB_OFSET8, 0x0080,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_cb_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A11, u16Val ? 0x707 : 0x0cc4,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a11, pCmdqCtx); // reg_mdma_csc_y2r_a11
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A12, u16Val ? 0x0 : 0x0950,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a12, pCmdqCtx); // reg_mdma_csc_y2r_a12
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A13, u16Val ? 0x0 : 0x3ffc,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a13, pCmdqCtx); // reg_mdma_csc_y2r_a13
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A21, u16Val ? 0x0 : 0x397e,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a21, pCmdqCtx); // reg_mdma_csc_y2r_a21
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A22, u16Val ? 0x6de : 0x0950,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a22, pCmdqCtx); // reg_mdma_csc_y2r_a22
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A23, u16Val ? 0x0 : 0x3cde,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a23, pCmdqCtx); // reg_mdma_csc_y2r_a23
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A31, u16Val ? 0x0 : 0x3ffe,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a31, pCmdqCtx); // reg_mdma_csc_y2r_a31
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A32, u16Val ? 0x0 : 0x0950,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a32, pCmdqCtx); // reg_mdma_csc_y2r_a32
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_A33, u16Val ? 0x707 : 0x1024,
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_a33, pCmdqCtx); // reg_mdma_csc_y2r_a33
    }
}

void HalDispDmaSetCscY2RMode(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_MODE, (u16Val ? mask_of_disp_cvbs_reg_mdma_csc_y2r_mode : 0),
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_mode, pCmdqCtx);
    }
}

void HalDispDmaSetCscY2RPortEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_MDMA_CSC_Y2R_EN, (u16Val ? mask_of_disp_cvbs_reg_mdma_csc_y2r_en : 0),
                                mask_of_disp_cvbs_reg_mdma_csc_y2r_en, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWVflip(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_V_FLIP, u16Val ? mask_of_disp_cvbs_reg_sc_frm_w_v_flip : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_v_flip, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWHmirror(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_H_MIRROR,
                                u16Val ? mask_of_disp_cvbs_reg_sc_frm_w_h_mirror : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_h_mirror, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_H_MIRROR_MD,
                                u16Val ? mask_of_disp_cvbs_reg_sc_frm_w_h_mirror_md : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_h_mirror_md, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWSize(u16 u16Width, u16 u16Height, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_HSIZE, u16Width, mask_of_disp_cvbs_reg_sc_frm_w_hsize, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_VSIZE, u16Height, mask_of_disp_cvbs_reg_sc_frm_w_vsize,
                                pCmdqCtx);
    }
}

void HalDispDmaSetFrmW444Packet(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_444_PACK, u16Val ? mask_of_disp_cvbs_reg_sc_frm_w_444_pack : 0,
                                mask_of_disp_cvbs_reg_sc_frm_w_444_pack, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWBaseAddr(u32 *pu32BaseAddr, u16 u16FrameIdx, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32RegOffset;
    u16   u16BaseAddrYHi, u16BaseAddrYLo;
    u16   u16BaseAddrCHi, u16BaseAddrCLo;
    u16   u16BaseAddrVHi, u16BaseAddrVLo;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u32RegOffset = ((u16FrameIdx % 4) << 2);

        u16BaseAddrYLo = (pu32BaseAddr[0] & 0x0000FFFF);
        u16BaseAddrYHi = (pu32BaseAddr[0] & 0xFFFF0000) >> 16;
        u16BaseAddrCLo = (pu32BaseAddr[1] & 0x0000FFFF);
        u16BaseAddrCHi = (pu32BaseAddr[1] & 0xFFFF0000) >> 16;
        u16BaseAddrVLo = (pu32BaseAddr[2] & 0x0000FFFF);
        u16BaseAddrVHi = (pu32BaseAddr[2] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_Y_ADR0, u16BaseAddrYLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_y_adr0_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_Y_ADR0 + 2, u16BaseAddrYHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_y_adr0_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_C_ADR0, u16BaseAddrCLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_c_adr0_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_C_ADR0 + 2, u16BaseAddrCHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_c_adr0_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_V_ADR0, u16BaseAddrVLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_v_adr0_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_V_ADR0 + 2, u16BaseAddrVHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_base_v_adr0_1, pCmdqCtx);

        if (u16FrameIdx != 0)
        {
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_Y_ADR0 + u32RegOffset, u16BaseAddrYLo,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_y_adr0_0, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_Y_ADR0 + 2 + u32RegOffset, u16BaseAddrYHi,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_y_adr0_1, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_C_ADR0 + u32RegOffset, u16BaseAddrCLo,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_c_adr0_0, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_C_ADR0 + 2 + u32RegOffset, u16BaseAddrCHi,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_c_adr0_1, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_V_ADR0 + u32RegOffset, u16BaseAddrVLo,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_v_adr0_0, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_BASE_V_ADR0 + 2 + u32RegOffset, u16BaseAddrVHi,
                                    mask_of_disp_cvbs_reg_sc_frm_w_base_v_adr0_1, pCmdqCtx);
        }
    }
}

void HalDispDmaSetFrmWReqTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_REQ_TH, u16Val, mask_of_disp_cvbs_reg_sc_frm_w_req_th, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqLen(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_REQ_LEN, (u16Val << shift_of_disp_cvbs_reg_sc_frm_w_req_len),
                                mask_of_disp_cvbs_reg_sc_frm_w_req_len, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqThC(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_REQ_TH_C, u16Val, mask_of_disp_cvbs_reg_sc_frm_w_req_th_c,
                                pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqLenC(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_REQ_LEN_C, (u16Val << shift_of_disp_cvbs_reg_sc_frm_w_req_len_c),
                                mask_of_disp_cvbs_reg_sc_frm_w_req_len_c, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtR0(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_MAX_LENGTH_R0, u16Val,
                                mask_of_disp_cvbs_reg_sc_frm_w_max_length_r0, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtR1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_MAX_LENGTH_R1,
                                (u16Val << shift_of_disp_cvbs_reg_sc_frm_w_max_length_r1),
                                mask_of_disp_cvbs_reg_sc_frm_w_max_length_r1, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtAll(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_MAX_LENGTH_ALL, u16Val,
                                mask_of_disp_cvbs_reg_sc_frm_w_max_length_all, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWLineOffset(u8 bEn, u16 *pu16LineOffset, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_LINEOFFSET_Y, pu16LineOffset[0],
                                mask_of_disp_cvbs_reg_sc_frm_w_lineoffset_y, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_LINEOFFSET_C, pu16LineOffset[1],
                                mask_of_disp_cvbs_reg_sc_frm_w_lineoffset_c, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_LF_EN, bEn ? mask_of_disp_cvbs_reg_sc_frm_w_lf_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_lf_en, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_EN, u16Val ? mask_of_disp_cvbs_reg_sc_frm_w_imi_en : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_en, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWIdxMd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IDX_MD, ((u16Val) << shift_of_disp_cvbs_reg_sc_frm_w_idx_md),
                                mask_of_disp_cvbs_reg_sc_frm_w_idx_md, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWIdxW(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_IDX_W, ((u16Val) << shift_of_disp_cvbs_reg_sc_frm_idx_w),
                                mask_of_disp_cvbs_reg_sc_frm_idx_w, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiStarAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16StarAddrYLo, u16StarAddrYHi;
    u16   u16StarAddrCLo, u16StarAddrCHi;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16StarAddrYLo = pu32StartAddr[0] & 0x0000FFFF;
        u16StarAddrYHi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;

        u16StarAddrCLo = pu32StartAddr[1] & 0x0000FFFF;
        u16StarAddrCHi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_Y, u16StarAddrYLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_Y + 2, u16StarAddrYHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_C, u16StarAddrCLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_C + 2, u16StarAddrCHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_c_1, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiOffset(u32 *pu32Offset, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16OffsetYLo, u16OffsetYHi;
    u16   u16OffsetCLo, u16OffsetCHi;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16OffsetYLo = pu32Offset[0] & 0x0000FFFF;
        u16OffsetYHi = (pu32Offset[0] & 0xFFFF0000) >> 16;

        u16OffsetCLo = pu32Offset[1] & 0x0000FFFF;
        u16OffsetCHi = (pu32Offset[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_Y, u16OffsetYLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_Y + 2, u16OffsetYHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_C, u16OffsetCLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_C + 2, u16OffsetCHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_c_1, pCmdqCtx);
    }
}

void HalDispDmaSetVenModeSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_VEN_MODE_SEL, u16Val, mask_of_disp_cvbs_reg_ven_mode_sel, pCmdqCtx);
    }
}

void HalDispDmaSetVenRingStarAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16StartAddrYHi, u16StartAddrYLo;
    u16   u16StartAddrCHi, u16StartAddrCLo;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16StartAddrYHi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;
        u16StartAddrYLo = (pu32StartAddr[0] & 0x0000FFFF);
        u16StartAddrCHi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;
        u16StartAddrCLo = (pu32StartAddr[1] & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_Y, u16StartAddrYLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_Y + 2, u16StartAddrYHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_C, u16StartAddrCLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_STA_ADR_C + 2, u16StartAddrCHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_sta_adr_c_1, pCmdqCtx);
    }
}
u16 HalDispDmaGetFrmVCnt(u32 u32DmaId, u8 u8bImmediate)
{
    u16 u16Val;
    if (u8bImmediate)
    {
        u16Val = HalDispUtilityR2BYTEDirect(DISP_CVBS_REG_SC_FRM_W_V_CNT);
    }
    else
    {
        u16Val = HalDispUtilityR2BYTEDirect(DISP_CVBS_REG_SC_FRM_W_V_CNT_Y);
    }
    return u16Val;
}

void HalDispDmaSetVenRingEndAddr(u32 *pu32EndAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16EndAddrYHi, u16EndAddrYLo;
    u16   u16EndAddrCHi, u16EndAddrCLo;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16EndAddrYHi = (pu32EndAddr[0] & 0xFFFF0000) >> 16;
        u16EndAddrYLo = (pu32EndAddr[0] & 0x0000FFFF);
        u16EndAddrCHi = (pu32EndAddr[1] & 0xFFFF0000) >> 16;
        u16EndAddrCLo = (pu32EndAddr[1] & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_Y, u16EndAddrYLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_Y + 2, u16EndAddrYHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_C, u16EndAddrCLo,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_IMI_OFFSET_C + 2, u16EndAddrCHi,
                                mask_of_disp_cvbs_reg_sc_frm_w_imi_offset_c_1, pCmdqCtx);
    }
}

void HalDispDmaSetDdaHratio(u8 bEn, u16 u16Ratio, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_H_ENABLE, bEn ? mask_of_disp_cvbs_reg_dda_h_enable : 0x0000,
                                mask_of_disp_cvbs_reg_dda_h_enable, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_H_RATIO, u16Ratio, mask_of_disp_cvbs_reg_dda_h_ratio, pCmdqCtx);
    }
}

void HalDispDmaSetDdaVratio(u8 bEn, u16 u16Ratio, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_V_ENABLE, bEn ? mask_of_disp_cvbs_reg_dda_v_enable : 0x0000,
                                mask_of_disp_cvbs_reg_dda_v_enable, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_V_RATIO, u16Ratio, mask_of_disp_cvbs_reg_dda_v_ratio, pCmdqCtx);
    }
}

void HalDispDmaSetDdaInSize(u16 u16Hsize, u16 u16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_H_SIZE, u16Hsize, mask_of_disp_cvbs_reg_dda_h_size, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_V_SIZE, u16Vsize, mask_of_disp_cvbs_reg_dda_v_size, pCmdqCtx);
    }
}

void HalDispDmaSetDdaOutSize(u16 u16Hsize, u16 u16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_SCL_H_SIZE, u16Hsize, mask_of_disp_cvbs_reg_dda_scl_h_size, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_DDA_SCL_V_SIZE, u16Vsize, mask_of_disp_cvbs_reg_dda_scl_v_size, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsRingEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_RING_EN, u16Val ? mask_of_disp_cvbs_reg_cvbs_ring_en : 0x0000,
                                mask_of_disp_cvbs_reg_cvbs_ring_en, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsMopMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_TO_MOP_MUX, u16Val, mask_of_disp_cvbs_reg_cvbs_to_mop_mux, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsDest(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_TO_DST, u16Val, mask_of_disp_cvbs_reg_cvbs_to_dst, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsOffsetEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_OFFSET_ENABLE,
                                u16Val ? mask_of_disp_cvbs_reg_cvbs_offset_enable : 0x0000,
                                mask_of_disp_cvbs_reg_cvbs_offset_enable, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsImiStartAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16StarAddrYLo, u16StarAddrYHi;
    u16   u16StarAddrCLo, u16StarAddrCHi;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16StarAddrYLo = pu32StartAddr[0] & 0x0000FFFF;
        u16StarAddrYHi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;

        u16StarAddrCLo = pu32StartAddr[1] & 0x0000FFFF;
        u16StarAddrCHi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_STA_ADR_Y, u16StarAddrYLo,
                                mask_of_disp_cvbs_reg_cvbs_imi_sta_adr_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_STA_ADR_Y + 2, u16StarAddrYHi,
                                mask_of_disp_cvbs_reg_cvbs_imi_sta_adr_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_STA_ADR_C, u16StarAddrCLo,
                                mask_of_disp_cvbs_reg_cvbs_imi_sta_adr_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_STA_ADR_C + 2, u16StarAddrCHi,
                                mask_of_disp_cvbs_reg_cvbs_imi_sta_adr_c_1, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsImiOffset(u32 *pu32Offset, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16OffsetYLo, u16OffsetYHi;
    u16   u16OffsetCLo, u16OffsetCHi;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16OffsetYLo = pu32Offset[0] & 0x0000FFFF;
        u16OffsetYHi = (pu32Offset[0] & 0xFFFF0000) >> 16;

        u16OffsetCLo = pu32Offset[1] & 0x0000FFFF;
        u16OffsetCHi = (pu32Offset[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_OFFSET_Y, u16OffsetYLo,
                                mask_of_disp_cvbs_reg_cvbs_imi_offset_y_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_OFFSET_Y + 2, u16OffsetYHi,
                                mask_of_disp_cvbs_reg_cvbs_imi_offset_y_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_OFFSET_C, u16OffsetCLo,
                                mask_of_disp_cvbs_reg_cvbs_imi_offset_c_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_IMI_OFFSET_C + 2, u16OffsetCHi,
                                mask_of_disp_cvbs_reg_cvbs_imi_offset_c_1, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_WIDTH_Y, pu16Hsize[0], mask_of_disp_cvbs_reg_cvbs_width_y, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_HEIGHT_Y, pu16Vsize[0], mask_of_disp_cvbs_reg_cvbs_height_y,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_WIDTH_C, pu16Hsize[1], mask_of_disp_cvbs_reg_cvbs_width_c, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_HEIGHT_C, pu16Vsize[1], mask_of_disp_cvbs_reg_cvbs_height_c,
                                pCmdqCtx);
    }
}

void HalDispDmaSetCvbsBufSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_H_BUF_Y, pu16Hsize[0], mask_of_disp_cvbs_reg_cvbs_h_buf_y, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_V_BUF_Y, pu16Vsize[0], mask_of_disp_cvbs_reg_cvbs_v_buf_y, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_H_BUF_C, pu16Hsize[1], mask_of_disp_cvbs_reg_cvbs_h_buf_c, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_V_BUF_C, pu16Vsize[1], mask_of_disp_cvbs_reg_cvbs_v_buf_c, pCmdqCtx);
    }
}

void HalDispDmaSetVideoPathFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_VIDEO_PATH_FPLL_DLY, u16Val,
                                mask_of_disp_top_op20_reg_video_path_fpll_dly, pCmdqCtx);
    }
}
void HalDispDmaSwRst(void *pCtx, HalDispDmaRstType_e enRst, HalDispUtilityRegAccessMode_e enRiuMode)
{
    void *pCmdqCtx = NULL;
    
    g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u8bRstWdma = (enRst == E_HAL_DISP_DMA_RST_WOPATH_H) ? 1 : 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSKByType(DISP_CVBS_REG_SW_RESET_SRC_MUX, mask_of_disp_cvbs_reg_sw_reset_src_mux,
                                  mask_of_disp_cvbs_reg_sw_reset_src_mux, pCmdqCtx, enRiuMode);
    HalDispUtilityW2BYTEMSKByType(HAL_DISP_UTILITY_DISP_TOP_RST(HAL_DISP_UTILITY_CMDQ_ID_DEVICE0),
                                  (enRst == E_HAL_DISP_DMA_RST_WOPATH_H) ? 0x1000 : 0, 0x1000, pCmdqCtx, enRiuMode);
}
void HalDispDmaHwInit(void *pCtx)
{
    HalDispDmaSetDispToMiuMux(HAL_DISP_DMA_MIU_MUX_FRONT_DISP0, NULL);
    HalDispDmaSetVsSel(1, NULL);
    HalDispDmaSwRst(pCtx, E_HAL_DISP_DMA_RST_WOPATH_H, E_DISP_UTILITY_REG_ACCESS_CPU);
}
void HalDispDmaHwDeinit(void *pCtx)
{
    HalDispDmaSetDispToMiuMux(HAL_DISP_DMA_MIU_MUX_FRONT_DISP0, NULL);
    HalDispDmaSwRst(pCtx, E_HAL_DISP_DMA_RST_WOPATH_H, E_DISP_UTILITY_REG_ACCESS_CPU);
}

void HalDispDmaSetVideoPathFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_VIDEO_PATH_FPLL_REF_SEL, u16Val,
                                mask_of_disp_top_op20_reg_video_path_fpll_ref_sel, pCmdqCtx);
    }
}

void HalDispDmaSetTgenExtFrmResetMode(u16 u16Val, void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg     = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    void *                     pCmdqCtx          = NULL;
    u32                        u32Reg;
    u16                        u16RegVal;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
        pstDestDevContain =
            (DrvDispCtxDeviceContain_t *)pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx]
                ->pDestDevContain;
        if (pstDestDevContain)
        {
            u32Reg    = pstDestDevContain->u32DevId == HAL_DISP_DEVICE_ID_1 ? DISP_TOP_OP21_REG_TGEN_EXT_FRM_RESET_EN
                                                                            : DISP_TOP_OP20_REG_TGEN_EXT_FRM_RESET_EN;
            u16RegVal = u16Val ? mask_of_disp_top_op21_reg_tgen_ext_frm_reset_en : 0;

            HalDispUtilityW2BYTEMSK(u32Reg, u16RegVal, mask_of_disp_top_op21_reg_tgen_ext_frm_reset_en, pCmdqCtx);
        }
    }
}

void HalDispDmaSetFpll1SrcMux(u8 bEn, u16 u16Mux, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Val;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16Val = bEn ? 0x0200 : 0x0000;
        u16Val |= u16Mux == HAL_DISP_DEVICE_ID_1 ? 0x0100 : 0x0000;
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_FPLL1_SRC_MUX, u16Val, mask_of_disp_top0_reg_fpll1_src_mux, pCmdqCtx);
    }
}

void HalDispDmaSetDispToMiuMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DISP_TO_MIU_MUX, u16Val << shift_of_disp_top0_reg_disp_to_miu_mux,
                            mask_of_disp_top0_reg_disp_to_miu_mux, pCmdqCtx);
}
u32 HalDispDmaGetDispToMiuMuxDev(void *pCtx)
{
    u32 u32DevId = 0;
    u16 u16Val;
    
    u16Val = HalDispUtilityR2BYTEMaskDirect(DISP_TOP0_REG_DISP_TO_MIU_MUX, mask_of_disp_top0_reg_disp_to_miu_mux);
    u16Val = u16Val >> shift_of_disp_top0_reg_disp_to_miu_mux;
    if(0 == u16Val || 1 == u16Val)
    {
        u32DevId = HAL_DISP_DEVICE_ID_0;
    }
    else if(2 == u16Val || 3 == u16Val)
    {
        u32DevId = HAL_DISP_DEVICE_ID_1;
    }

    return u32DevId;
}

void HalDispDmaSetR2Y(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_R2Y_EN, u16Val ? mask_of_disp_cvbs_reg_cvbs_r2y_en : 0x0000,
                                mask_of_disp_cvbs_reg_cvbs_r2y_en, pCmdqCtx);
    }
}

void HalDispDmaSetTvencSynthSet(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Val;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        u16Val = u32Val & 0x0000FFFF;
        HalDispUtilityW2BYTEMSK(DISP_TOP1_REG_SYN_SET, u16Val, mask_of_disp_top1_reg_syn_set_0, pCmdqCtx);
        u16Val = (u32Val & 0xFFFF0000) >> 16;
        HalDispUtilityW2BYTEMSK(DISP_TOP1_REG_SYN_SET + 2, u16Val, mask_of_disp_top1_reg_syn_set_1, pCmdqCtx);
    }
}

void HalDispDmaSetTvencSynthSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_LPLL_4C_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_TOP1_REG_FPLL1_SET_SEL,
            u16Val ? mask_of_disp_top1_reg_fpll1_set_sel : 0x0000, mask_of_disp_top1_reg_fpll1_set_sel, pCmdqCtx);
    }
}

void HalDispDmaOnOffCmdqIrqWriteDone(void *pCtx, u8 u8bEn)
{
    void *                     pCmdqCtx      = NULL;
    u32                        u32Reg;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u32Reg = DISP_CVBS_REG_CMDQ_INTR_MASK_GRP_A;
        HalDispUtilityW2BYTEMSK(u32Reg, u8bEn ? 0 : HAL_DISP_CMDQIRQ_DMA_DONE_BIT, HAL_DISP_CMDQIRQ_DMA_DONE_BIT,
                                pCmdqCtx);
    }
}

void HalDispDmaClearIrqWriteDone(void *pCtx)
{
    void *                     pCmdqCtx      = NULL;
    u32                        u32Reg;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u32Reg = DISP_CVBS_REG_CMDQ_INTR_STATUS_GRP_A;
        HalDispUtilityW2BYTEMSK(u32Reg, HAL_DISP_CMDQIRQ_DMA_DONE_BIT, HAL_DISP_CMDQIRQ_DMA_DONE_BIT, pCmdqCtx);
    }
}

void HalDispDmaPollWriteDone(void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *   pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *                     pCmdqCtx      = NULL;
    u32                        u32Reg;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u32Reg = pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0 ? DISP_TOP0_REG_INTR_STATUS_GRP_A
                                                                 : DISP_TOP1_REG_INTR_STATUS_GRP_A;
        HalDispUtilityWritePollCmd(u32Reg, HAL_DISP_DMA_IRQ_WRITEDONE, HAL_DISP_DMA_IRQ_WRITEDONE, 2000000000, 1,
                                   pCmdqCtx);
    }
}
void HalDispDmaPollVWriteCnt(void *pCtx, u16 u16LineCnt)
{
    void *pCmdqCtx = NULL;

    DISP_ERR("%s %d, LineCnt:%d\n", __FUNCTION__, __LINE__, u16LineCnt);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityWritePollCmd(DISP_CVBS_REG_SC_FRM_W_V_CNT, u16LineCnt, mask_of_disp_cvbs_reg_sc_frm_w_v_cnt,
                                   2000000000, 1, pCmdqCtx);
    }
}
