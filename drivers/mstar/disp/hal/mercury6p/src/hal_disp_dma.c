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
        u32Reg        = DISP_CVBS_REG_INTR_RAW_STATUS_GRP_A;
        u16Msk        = HAL_DISP_IRQ_DMA_VDE_MSK;
        u16Val        = 0;
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
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_DMA_EN, bEn ? mask_of_disp_wdma_reg_sc_frm_w_dma_en : 0x0000,
                                mask_of_disp_wdma_reg_sc_frm_w_dma_en, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWEnByFormat(u8 *pbEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WDMA_EN, pbEn[0] ? mask_of_disp_sgwdma_reg_wdma_en : 0x0000,
                                mask_of_disp_sgwdma_reg_wdma_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WDMA_EN + 0x40, pbEn[1] ? mask_of_disp_sgwdma_reg_wdma_en : 0x0000,
                                mask_of_disp_sgwdma_reg_wdma_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WDMA_EN + 0x80, pbEn[2] ? mask_of_disp_sgwdma_reg_wdma_en : 0x0000,
                                mask_of_disp_sgwdma_reg_wdma_en, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWCrc16(u8 *pbEn, HalDispCrc16Type_e enCrcTpye, void *pCtx)
{
    void *pCmdqCtx = NULL;

    if (enCrcTpye == E_HAL_DISP_CRC16_OVERWRITE)
    {
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md = HAL_DISP_REG_CRC_MD_HEAD;
    }
    else if (enCrcTpye == E_HAL_DISP_CRC16_EXT)
    {
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md = HAL_DISP_REG_CRC_MD_TAIL;
    }
    else
    {
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md = g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md;
    }
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_CRC_MODE,
                                ((pbEn[0]) ? g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md : 0) << shift_of_disp_sgwdma_reg_crc_mode,
                                mask_of_disp_sgwdma_reg_crc_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WINFO_SEL,
                                ((pbEn[0] && g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md == HAL_DISP_REG_CRC_MD_RIU) ? 0x10 : 0)
                                    << shift_of_disp_sgwdma_reg_winfo_sel,
                                mask_of_disp_sgwdma_reg_winfo_sel, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_CRC_MODE + 0x40,
                                ((pbEn[1]) ? g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md : 0) << shift_of_disp_sgwdma_reg_crc_mode,
                                mask_of_disp_sgwdma_reg_crc_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WINFO_SEL + 0x40,
                                ((pbEn[1] && g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md == HAL_DISP_REG_CRC_MD_RIU) ? 0x10 : 0)
                                    << shift_of_disp_sgwdma_reg_winfo_sel,
                                mask_of_disp_sgwdma_reg_winfo_sel, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_CRC_MODE + 0x80,
                                ((pbEn[2]) ? g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md : 0) << shift_of_disp_sgwdma_reg_crc_mode,
                                mask_of_disp_sgwdma_reg_crc_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WINFO_SEL + 0x80,
                                ((pbEn[2] && g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md == HAL_DISP_REG_CRC_MD_RIU) ? 0x10 : 0)
                                    << shift_of_disp_sgwdma_reg_winfo_sel,
                                mask_of_disp_sgwdma_reg_winfo_sel, pCmdqCtx);
    }
}
void HalDispDmaGetFrmWCrc16(u32 *p32Val)
{
    u32 u32Val;
    u8  i;

    if (p32Val)
    {
        for (i = 0; i < 3; i++)
        {
            u32Val = HalDispUtilityR2BYTEDirect(DISP_SGWDMA_REG_WINFO_0 + i * 0x40);
            u32Val |= (HalDispUtilityR2BYTEDirect(DISP_SGWDMA_REG_WINFO_1 + i * 0x40) << 16);
            p32Val[i] = u32Val;
        }
    }
}
u16 HalDispDmaGetFrmVCnt(u32 u32DmaId, u8 u8bImmediate)
{
    u16 u16Val;
    if (u8bImmediate)
    {
        u16Val = HalDispUtilityR2BYTEDirect(DISP_WDMA_REG_SC_FRM_W_V_CNT);
    }
    else
    {
        u16Val = HalDispUtilityR2BYTEDirect(DISP_WDMA_REG_SC_FRM_W_V_CNT_Y);
    }
    return u16Val;
}

void HalDispDmaSetFrmWRingBuffHeight(u16 *pu16Height, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_LINESM1, pu16Height[0],
                                mask_of_disp_sgwdma_reg_ring_buf_linesm1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_LINESM1 + 0x40, pu16Height[1],
                                mask_of_disp_sgwdma_reg_ring_buf_linesm1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_LINESM1 + 0x80, pu16Height[2],
                                mask_of_disp_sgwdma_reg_ring_buf_linesm1, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWFifoSizeByFormat(u8 *p8Size, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSIZE, p8Size[0] << shift_of_disp_sgwdma_reg_wfifosize,
                                mask_of_disp_sgwdma_reg_wfifosize, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSIZE + 0x40, p8Size[1] << shift_of_disp_sgwdma_reg_wfifosize,
                                mask_of_disp_sgwdma_reg_wfifosize, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSIZE + 0x80, p8Size[2] << shift_of_disp_sgwdma_reg_wfifosize,
                                mask_of_disp_sgwdma_reg_wfifosize, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWFifoStartByFormat(u8 *p8Start, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSTART, p8Start[0] << shift_of_disp_sgwdma_reg_wfifostart,
                                mask_of_disp_sgwdma_reg_wfifostart, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSTART + 0x40, p8Start[1] << shift_of_disp_sgwdma_reg_wfifostart,
                                mask_of_disp_sgwdma_reg_wfifostart, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WFIFOSTART + 0x80, p8Start[2] << shift_of_disp_sgwdma_reg_wfifostart,
                                mask_of_disp_sgwdma_reg_wfifostart, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWWidthByFormat(u16 *p16Width, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH, p16Width[0] << shift_of_disp_sgwdma_reg_wwidth,
                                mask_of_disp_sgwdma_reg_wwidth, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH + 0x40, p16Width[1] << shift_of_disp_sgwdma_reg_wwidth,
                                mask_of_disp_sgwdma_reg_wwidth, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH + 0x80, p16Width[2] << shift_of_disp_sgwdma_reg_wwidth,
                                mask_of_disp_sgwdma_reg_wwidth, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWHeightByFormat(u16 *p16Height, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WHEIGHT, p16Height[0] << shift_of_disp_sgwdma_reg_wheight,
                                mask_of_disp_sgwdma_reg_wheight, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WHEIGHT + 0x40, p16Height[1] << shift_of_disp_sgwdma_reg_wheight,
                                mask_of_disp_sgwdma_reg_wheight, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WHEIGHT + 0x80, p16Height[2] << shift_of_disp_sgwdma_reg_wheight,
                                mask_of_disp_sgwdma_reg_wheight, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWPixelByFormat(u16 *p16Pixel, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH_PIXELS, p16Pixel[0] << shift_of_disp_sgwdma_reg_wwidth_pixels,
                                mask_of_disp_sgwdma_reg_wwidth_pixels, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH_PIXELS + 0x40,
                                p16Pixel[1] << shift_of_disp_sgwdma_reg_wwidth_pixels,
                                mask_of_disp_sgwdma_reg_wwidth_pixels, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WWIDTH_PIXELS + 0x80,
                                p16Pixel[2] << shift_of_disp_sgwdma_reg_wwidth_pixels,
                                mask_of_disp_sgwdma_reg_wwidth_pixels, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWPitchByFormat(u16 *p16Pitch, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WPITCH, p16Pitch[0] << shift_of_disp_sgwdma_reg_wpitch,
                                mask_of_disp_sgwdma_reg_wpitch, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WPITCH + 0x40, p16Pitch[1] << shift_of_disp_sgwdma_reg_wpitch,
                                mask_of_disp_sgwdma_reg_wpitch, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WPITCH + 0x80, p16Pitch[2] << shift_of_disp_sgwdma_reg_wpitch,
                                mask_of_disp_sgwdma_reg_wpitch, pCmdqCtx);
    }
}
void HalDispDmaSetFrmWTriggerModeCfg(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER_MODE,
                                bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_mode : 0x0000,
                                mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger_mode, pCmdqCtx);
    }
}
void HalDispDmaSetRingDbfEn(u8 bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_LOAD_REG, bEn ? 0 : mask_of_disp_cvbs_reg_load_reg,
                                mask_of_disp_cvbs_reg_load_reg, pCmdqCtx);
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

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR_FIFO_PUSH, mask_of_disp_sgwdma_reg_waddr_fifo_push,
                            mask_of_disp_sgwdma_reg_waddr_fifo_push, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR_FIFO_PUSH + 0x40, mask_of_disp_sgwdma_reg_waddr_fifo_push,
                            mask_of_disp_sgwdma_reg_waddr_fifo_push, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR_FIFO_PUSH + 0x80, mask_of_disp_sgwdma_reg_waddr_fifo_push,
                            mask_of_disp_sgwdma_reg_waddr_fifo_push, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_SC_FRM_W_DMA_TRIGGER,
                            bEn ? mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger : 0x0000,
                            mask_of_disp_cvbs_reg_sc_frm_w_dma_trigger, pCmdqCtx);
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

void HalDispDmaSetFrmWCFilter(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_C_FILTER, u16Val, mask_of_disp_wdma_reg_sc_frm_w_c_filter,
                                pCmdqCtx);
    }
}

void HalDispDmaSetFrmWPixelFormat(u16 u16Val0, u16 u16Val1, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_422TO420_MD, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_422to420_md,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_420_PLANER, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_420_planer,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_422_YC_SEP, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_422_yc_sep,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_422_PLANER, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_422_planer,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_422_PACK, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_422_pack,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_ARGB_MODE, u16Val0,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_argb_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_RGB565, u16Val0, mask_of_disp_wdma_reg_sc_frm_w_rgb565,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(
            DISP_WDMA_REG_SC_FRM_W_DITH_EN,
            (u16Val0 & mask_of_disp_wdma_reg_sc_frm_w_rgb565) ? mask_of_disp_wdma_reg_sc_frm_w_dith_en : 0,
            mask_of_disp_wdma_reg_sc_frm_w_dith_en, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_444_A_VALUE, u16Val1, mask_of_disp_wdma_reg_sc_frm_w_444_a_value,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_444_Y_LOCATION, u16Val1,
                                mask_of_disp_wdma_reg_sc_frm_w_444_y_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_444_U_LOCATION, u16Val1,
                                mask_of_disp_wdma_reg_sc_frm_w_444_u_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_444_V_LOCATION, u16Val1,
                                mask_of_disp_wdma_reg_sc_frm_w_444_v_location, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_444_PACK, u16Val1, mask_of_disp_wdma_reg_sc_frm_w_444_pack,
                                pCmdqCtx);
    }
}
void HalDispDmaSetCscY2RMatrix(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_Y_OFSET8, 0x1000,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_y_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_CR_OFSET8, 0x8000,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_cr_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_CB_OFSET8, 0x0080,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_cb_ofset8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A11, u16Val ? 0x707 : 0x0cc4,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a11, pCmdqCtx); // reg_mdma_csc_y2r_a11
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A12, u16Val ? 0x0 : 0x0950,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a12, pCmdqCtx); // reg_mdma_csc_y2r_a12
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A13, u16Val ? 0x0 : 0x3ffc,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a13, pCmdqCtx); // reg_mdma_csc_y2r_a13
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A21, u16Val ? 0x0 : 0x397e,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a21, pCmdqCtx); // reg_mdma_csc_y2r_a21
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A22, u16Val ? 0x6de : 0x0950,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a22, pCmdqCtx); // reg_mdma_csc_y2r_a22
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A23, u16Val ? 0x0 : 0x3cde,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a23, pCmdqCtx); // reg_mdma_csc_y2r_a23
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A31, u16Val ? 0x0 : 0x3ffe,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a31, pCmdqCtx); // reg_mdma_csc_y2r_a31
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A32, u16Val ? 0x0 : 0x0950,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a32, pCmdqCtx); // reg_mdma_csc_y2r_a32
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_A33, u16Val ? 0x707 : 0x1024,
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_a33, pCmdqCtx); // reg_mdma_csc_y2r_a33
    }
}

void HalDispDmaSetCscY2RMode(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_MODE,
                                (u16Val ? mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_mode : 0),
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_mode, pCmdqCtx);
    }
}

void HalDispDmaSetCscY2RPortEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_MDMA_CSC_Y2R_EN,
                                (u16Val ? mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_en : 0),
                                mask_of_disp_wdma_reg_sc_frm_w_mdma_csc_y2r_en, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWVflip(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_V_FLIP, u16Val ? mask_of_disp_wdma_reg_sc_frm_w_v_flip : 0x0000,
                                mask_of_disp_wdma_reg_sc_frm_w_v_flip, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_FLIP, u16Val ? mask_of_disp_sgwdma_reg_flip : 0x0000,
                                mask_of_disp_sgwdma_reg_flip, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_FLIP + 0x40, u16Val ? mask_of_disp_sgwdma_reg_flip : 0x0000,
                                mask_of_disp_sgwdma_reg_flip, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_FLIP + 0x80, u16Val ? mask_of_disp_sgwdma_reg_flip : 0x0000,
                                mask_of_disp_sgwdma_reg_flip, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWHmirror(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_H_MIRROR,
                                u16Val ? mask_of_disp_wdma_reg_sc_frm_w_h_mirror : 0x0000,
                                mask_of_disp_wdma_reg_sc_frm_w_h_mirror, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_H_MIRROR_MD,
                                u16Val ? mask_of_disp_wdma_reg_sc_frm_w_h_mirror_md : 0x0000,
                                mask_of_disp_wdma_reg_sc_frm_w_h_mirror_md, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_MIRROR, u16Val ? mask_of_disp_sgwdma_reg_mirror : 0x0000,
                                mask_of_disp_sgwdma_reg_mirror, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_MIRROR + 0x40, u16Val ? mask_of_disp_sgwdma_reg_mirror : 0x0000,
                                mask_of_disp_sgwdma_reg_mirror, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_MIRROR + 0x80, u16Val ? mask_of_disp_sgwdma_reg_mirror : 0x0000,
                                mask_of_disp_sgwdma_reg_mirror, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWBaseAddr(u32 *pu32BaseAddr, u16 u16FrameIdx, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16BaseAddrYHi, u16BaseAddrYLo;
    u16   u16BaseAddrCHi, u16BaseAddrCLo;
    u16   u16BaseAddrVHi, u16BaseAddrVLo;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u16BaseAddrYLo = (pu32BaseAddr[0] & 0x0000FFFF);
        u16BaseAddrYHi = (pu32BaseAddr[0] & 0xFFFF0000) >> 16;
        u16BaseAddrCLo = (pu32BaseAddr[1] & 0x0000FFFF);
        u16BaseAddrCHi = (pu32BaseAddr[1] & 0xFFFF0000) >> 16;
        u16BaseAddrVLo = (pu32BaseAddr[2] & 0x0000FFFF);
        u16BaseAddrVHi = (pu32BaseAddr[2] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0, u16BaseAddrYLo, mask_of_disp_sgwdma_reg_waddr0_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0_1, u16BaseAddrYHi, mask_of_disp_sgwdma_reg_waddr0_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0 + 0x40, u16BaseAddrCLo, mask_of_disp_sgwdma_reg_waddr0_0,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0_1 + 0x40, u16BaseAddrCHi, mask_of_disp_sgwdma_reg_waddr0_1,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0 + 0x80, u16BaseAddrVLo, mask_of_disp_sgwdma_reg_waddr0_0,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WADDR0_1 + 0x80, u16BaseAddrVHi, mask_of_disp_sgwdma_reg_waddr0_1,
                                pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WREQTH, (u16Val << shift_of_disp_sgwdma_reg_wreqth),
                                mask_of_disp_sgwdma_reg_wreqth, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WREQTH + 0x40, (u16Val << shift_of_disp_sgwdma_reg_wreqth),
                                mask_of_disp_sgwdma_reg_wreqth, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WREQTH + 0x80, (u16Val << shift_of_disp_sgwdma_reg_wreqth),
                                mask_of_disp_sgwdma_reg_wreqth, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqLen(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WSIZE, (u16Val << shift_of_disp_sgwdma_reg_wsize),
                                mask_of_disp_sgwdma_reg_wsize, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WSIZE + 0x40, (u16Val << shift_of_disp_sgwdma_reg_wsize),
                                mask_of_disp_sgwdma_reg_wsize, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_WSIZE + 0x80, (u16Val << shift_of_disp_sgwdma_reg_wsize),
                                mask_of_disp_sgwdma_reg_wsize, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWCompessEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_WDMA_REG_SC_FRM_W_COMPRESSION_EN,
                                u16Val ? mask_of_disp_wdma_reg_sc_frm_w_compression_en : 0,
                                mask_of_disp_wdma_reg_sc_frm_w_compression_en, pCmdqCtx);
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

void HalDispDmaSetCvbsRingEn(u16 u16Val, u8 *pbEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_CVBS_REG_CVBS_RING_EN, u16Val ? mask_of_disp_cvbs_reg_cvbs_ring_en : 0x0000,
                                mask_of_disp_cvbs_reg_cvbs_ring_en, pCmdqCtx);

        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_MODE,
                                (pbEn[0] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_buf_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_buf_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_MODE + 0x40,
                                (pbEn[1] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_buf_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_buf_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_BUF_MODE + 0x80,
                                (pbEn[2] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_buf_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_buf_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_HOLDEXT_MODE,
                                (pbEn[0] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_holdext_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_holdext_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_HOLDEXT_MODE + 0x40,
                                (pbEn[1] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_holdext_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_holdext_mode, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_SGWDMA_REG_RING_HOLDEXT_MODE + 0x80,
                                (pbEn[2] && u16Val) ? (1 << shift_of_disp_sgwdma_reg_ring_holdext_mode) : 0x0000,
                                mask_of_disp_sgwdma_reg_ring_holdext_mode, pCmdqCtx);
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
    if ((enRst == E_HAL_DISP_DMA_RST_WOPATH_H))
    {
        HalDispUtilityW2BYTEMSKByType(REG_SGDMA_RESET, 1, 1, pCmdqCtx, enRiuMode);
        // polling 0x166d h30 = 0
        HalDispUtilityWritePollCmdByType(REG_SGDMA_RESETING, 0, 0x0200, 2000000000, 1, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(REG_SGDMA_RESET, 0, 1, pCmdqCtx, enRiuMode);
    }
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
        HalDispUtilityW2BYTEMSK(u32Reg, u8bEn ? 0 : HAL_DISP_CMDQIRQ_DMA_REALDONE_BIT,
                                HAL_DISP_CMDQIRQ_DMA_REALDONE_MSK, pCmdqCtx);
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
        HalDispUtilityW2BYTEMSK(u32Reg, HAL_DISP_CMDQIRQ_DMA_REALDONE_BIT, HAL_DISP_CMDQIRQ_DMA_REALDONE_MSK, pCmdqCtx);
    }
}
