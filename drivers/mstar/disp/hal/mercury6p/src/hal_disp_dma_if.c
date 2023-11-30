/*
* hal_disp_dma_if.c- Sigmastar
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

#define _HAL_DISP_DMA_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_clk.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "hal_disp_op2.h"
#include "hal_disp_irq.h"
#include "drv_disp_ctx.h"
#include "hal_disp_dma.h"
#include "hal_disp_dma_if.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DISP_CHECK_DMA_SRC_DST_CORRECT(pstSrcContain, pstDestContain) \
    (((pstSrcContain->u32DevId == HAL_DISP_DEVICE_ID_0) || (pstSrcContain->u32DevId == HAL_DISP_DEVICE_ID_1)))

#define DISP_CHECK_FPLL_NEED_TO_SETUP(eMode, SrcInterface, DstInterface) \
    ((DISP_DMA_IS_RING_MODE(eMode) && DISP_OUTDEV_IS_CVBS(DstInterface) && DISP_OUTDEV_IS_HDMI(SrcInterface)))
#define DISP_SET_RING_BUF_HEIGHT(height) ((height) ? (height - 1) : 15)
#define DISP_ALIGN_4(x)                  (((x + 0x3) >> 2) << 2)
#define DISP_ALIGN_8(x)                  (((x + 0x7) >> 3) << 3)
#define DISP_ALIGN_16(x)                 (((x + 0xF) >> 4) << 4)
#define DISP_ALIGN_32(x)                 (((x + 0x1F) >> 5) << 5)
#define DISP_SET_BUF_WIDTH(width, comp)  ((comp) ? (DISP_ALIGN_4(width)) / 4 * 3 : width)
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
extern HalDispInternalConfig_t g_stInterCfg[HAL_DISP_DEVICE_MAX];

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------

void HalDispDmaIfWaitDone(void *pCtx)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;
    DrvDispCtxConfig_t *         pstDispCtxCfg = pCtx;
    DrvDispCtxDmaContain_t *     ptDmaContain  = NULL;

    if (pstDispCtxCfg)
    {
        HalDispGetCmdqByCtx((void *)pstDispCtxCfg, (void *)&pstCmdqCtx);
        ptDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    }

    if (pstCmdqCtx)
    {
        HalDispDmaOnOffCmdqIrqWriteDone(pstDispCtxCfg, 1);
        if (pstCmdqCtx->pvCmdqInf)
        {
            HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait even
        }
        else
        {
            HalDispUtilityPollWait(pstCmdqCtx->pvCmdqInf, DISP_CVBS_REG_CMDQ_INTR_STATUS_GRP_A,
                                   HAL_DISP_CMDQIRQ_DMA_REALDONE_BIT, HAL_DISP_CMDQIRQ_DMA_REALDONE_BIT, 1000000, 1);
            DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d,Dma DONE\n", __FUNCTION__, __LINE__);
        }
        HalDispIrqEnableDoubleVsync(ptDmaContain->u8RingModeEn, pstDispCtxCfg);
        HalDispDmaClearIrqWriteDone((void *)pstDispCtxCfg);

        HalDispUtilityW2BYTEMSK(REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(pstCmdqCtx->s32UtilityId),
                                REG_CMDQ_IN_PROCESS_OFF, REG_CMDQ_IN_PROCESS_MSK, (void *)pstCmdqCtx);

        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }
}
void HalDispDmaIfRegFlip(void *pCtx)
{
    DrvDispCtxConfig_t *         pstDispCtxCfg = pCtx;
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;
    DrvDispCtxDmaContain_t *     ptDmaContain  = NULL;
    u16                          u16CmdqInProcess;

    if (pstDispCtxCfg)
    {
        HalDispGetCmdqByCtx((void *)pstDispCtxCfg, (void *)&pstCmdqCtx);
        ptDmaContain  = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CmdqCtx:%px, FlipCnt:%d, Dma MODE\n", __FUNCTION__, __LINE__,
                 pstCmdqCtx, pstCmdqCtx->u32DirectCmdqCnt);
    }
    if (pstCmdqCtx)
    {
        u16CmdqInProcess =
            HalDispUtilityR2BYTEMaskDirect(REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(pstCmdqCtx->s32UtilityId),
                                           REG_CMDQ_IN_PROCESS_MSK | REG_CMDQ_DEV_TIMEGEN_START_MSK);
        if (((u16CmdqInProcess & REG_CMDQ_IN_PROCESS_MSK) == REG_CMDQ_IN_PROCESS_OFF)
            && (ptDmaContain->u8RingModeEn || ptDmaContain->u16RingMdChange))
        {
            HalDispDmaCheckIdle(pstDispCtxCfg, E_DISP_UTILITY_REG_ACCESS_CMDQ_DIRECT);
        }
        HalDispDmaOnOffCmdqIrqWriteDone(pstDispCtxCfg, 1);
        HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16RegFlipCnt, 1);
        HalDispUtilityW2BYTEMSK(
            REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(pstCmdqCtx->s32UtilityId),
            REG_CMDQ_IN_PROCESS_ON | REG_CMDQ_DEV_TIMEGEN_START_MSK
                | (pstCmdqCtx->u16RegFlipCnt & REG_CMDQ_PROCESS_FENCE_MSK), // set cmdq process = 1 and fence
            REG_CMDQ_IN_PROCESS_MSK | REG_CMDQ_PROCESS_FENCE_MSK | REG_CMDQ_DEV_TIMEGEN_START_MSK, (void *)pstCmdqCtx);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }
}
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HalDispDmaIfSetFpll(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *   pstDmaContain     = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain  = NULL;

    pstDmaContain     = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDestDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
    pstSrcDevContain  = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;

    if (pstDestDevContain
        && DISP_CHECK_FPLL_NEED_TO_SETUP(pstDmaContain->stOutputCfg.eMode, pstSrcDevContain->u32Interface,
                                         pstDestDevContain->u32Interface))
    {
        HalDispDmaSetTvencSynthSel(1, (void *)pstDispCtxCfg);
        if (DISP_OUTDEV_IS_HDMI(pstSrcDevContain->u32Interface) && DISP_OUTDEV_IS_VGA(pstSrcDevContain->u32Interface))
        {
            HalDispSetDacAffReset(1, pstDispCtxCfg);
            HalDispSetDacReset(1, pstDispCtxCfg);
        }

        HalDispSetFpllGain(HAL_DISP_FPLL_1_CVBS_DAC, 0x88, (void *)pstDispCtxCfg);
        HalDispSetFpllThd(HAL_DISP_FPLL_1_CVBS_DAC, 0x90, (void *)pstDispCtxCfg);
        HalDispSetFpllAbsLimit(HAL_DISP_FPLL_1_CVBS_DAC, 0x10000, (void *)pstDispCtxCfg);
        HalDispSetFpllRst(HAL_DISP_FPLL_1_CVBS_DAC, 0, (void *)pstDispCtxCfg);
        HalDispSetFpllEn(HAL_DISP_FPLL_1_CVBS_DAC, 1, (void *)pstDispCtxCfg);
        HalDispSetFpllUsedFlag(HAL_DISP_FPLL_1_CVBS_DAC, pstSrcDevContain->u32DevId, (void *)pstDispCtxCfg);
        HalDispIrqSetFpllEn(HAL_DISP_FPLL_1_CVBS_DAC, 1);
        HalDispPollFpllLock(HAL_DISP_FPLL_1_CVBS_DAC, (void *)pstDispCtxCfg);

        if (DISP_OUTDEV_IS_HDMI(pstSrcDevContain->u32Interface) && DISP_OUTDEV_IS_VGA(pstSrcDevContain->u32Interface))
        {
            HalDispSetDacReset(0, pstDispCtxCfg);
            HalDispSetDacAffReset(0, pstDispCtxCfg);
        }
    }
}
static void _HalDispSetDataStructByFormat(DrvDispCtxConfig_t *pstDispCtxCfg, HalDispFormatRela_t *pstForCfg)
{
    DrvDispCtxDmaContain_t *   pstDmaContain = NULL;
    HalDispDmaPixelFormat_e    ePixFmt;
    HalDispPixelCompressMode_e enComp = E_HAL_DISP_COMPRESS_MODE_NONE;

    u8 u8Ret = 1;

    if (pstDispCtxCfg)
    {
        pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
        if (pstDmaContain)
        {
            u8Ret = 0;
        }
    }
    if (u8Ret)
    {
        return;
    }
    memset(pstForCfg, 0, sizeof(HalDispFormatRela_t));
    ePixFmt = pstDmaContain->stOutputCfg.ePixelFormat;
    enComp  = pstDmaContain->stOutputCfg.eCompressMode;
    switch (ePixFmt)
    {
        case E_HAL_DISP_DMA_PIX_FMT_RGB565:
        case E_HAL_DISP_DMA_PIX_FMT_YUYV:
        case E_HAL_DISP_DMA_PIX_FMT_YVYU:
        case E_HAL_DISP_DMA_PIX_FMT_UYVY:
        case E_HAL_DISP_DMA_PIX_FMT_VYUY:
            pstForCfg->u8bEn[0]       = 1;
            pstForCfg->u8fifosize[0]  = 2;
            pstForCfg->u8fifostart[0] = 0;
            pstForCfg->u16Width[0]  = DISP_SET_BUF_WIDTH(DISP_ALIGN_8(pstDmaContain->stOutputCfg.u16Width) / 8, enComp);
            pstForCfg->u16Height[0] = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Pixel[0]  = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Stride[0] = DISP_ALIGN_8(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_SP420:
        case E_HAL_DISP_DMA_PIX_FMT_SP420NV21:
            pstForCfg->u8bEn[0]       = 1;
            pstForCfg->u8bEn[1]       = 1;
            pstForCfg->u8fifosize[0]  = 1;
            pstForCfg->u8fifosize[1]  = 1;
            pstForCfg->u8fifostart[0] = 0;
            pstForCfg->u8fifostart[1] = 1;
            pstForCfg->u16Width[0] =
                DISP_SET_BUF_WIDTH(DISP_ALIGN_16(pstDmaContain->stOutputCfg.u16Width) / 16, enComp);
            pstForCfg->u16Width[1]      = pstForCfg->u16Width[0];
            pstForCfg->u16Height[0]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Height[1]     = pstDmaContain->stOutputCfg.u16Height / 2;
            pstForCfg->u16Pixel[0]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Pixel[1]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Stride[0]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16Stride[1]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[1]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            pstForCfg->u16BuffHeight[1] =
                DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight) / 2;
            break;
        case E_HAL_DISP_DMA_PIX_FMT_SP422:
            pstForCfg->u8bEn[0]       = 1;
            pstForCfg->u8bEn[1]       = 1;
            pstForCfg->u8fifosize[0]  = 1;
            pstForCfg->u8fifosize[1]  = 1;
            pstForCfg->u8fifostart[0] = 0;
            pstForCfg->u8fifostart[1] = 1;
            pstForCfg->u16Width[0] =
                DISP_SET_BUF_WIDTH(DISP_ALIGN_16(pstDmaContain->stOutputCfg.u16Width) / 16, enComp);
            pstForCfg->u16Width[1]      = pstForCfg->u16Width[0];
            pstForCfg->u16Height[0]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Height[1]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Pixel[0]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Pixel[1]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Stride[0]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16Stride[1]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[1]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            pstForCfg->u16BuffHeight[1] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_P420:
            pstForCfg->u8bEn[0]       = 1;
            pstForCfg->u8bEn[1]       = 1;
            pstForCfg->u8bEn[2]       = 1;
            pstForCfg->u8fifosize[0]  = 1;
            pstForCfg->u8fifosize[1]  = 0;
            pstForCfg->u8fifosize[2]  = 0;
            pstForCfg->u8fifostart[0] = 0;
            pstForCfg->u8fifostart[1] = 2;
            pstForCfg->u8fifostart[2] = 3;
            pstForCfg->u16Width[0] =
                DISP_SET_BUF_WIDTH(DISP_ALIGN_32(pstDmaContain->stOutputCfg.u16Width) / 16, enComp);
            pstForCfg->u16Width[1]      = pstForCfg->u16Width[0] / 2;
            pstForCfg->u16Width[2]      = pstForCfg->u16Width[1];
            pstForCfg->u16Height[0]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Height[1]     = pstDmaContain->stOutputCfg.u16Height / 2;
            pstForCfg->u16Height[2]     = pstDmaContain->stOutputCfg.u16Height / 2;
            pstForCfg->u16Pixel[0]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Pixel[1]      = pstDmaContain->stOutputCfg.u16Width / 2;
            pstForCfg->u16Pixel[2]      = pstDmaContain->stOutputCfg.u16Width / 2;
            pstForCfg->u16Stride[0]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16Stride[1]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[1]) / 16;
            pstForCfg->u16Stride[2]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[2]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            pstForCfg->u16BuffHeight[1] =
                DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight) / 2;
            pstForCfg->u16BuffHeight[2] =
                DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight) / 2;
            break;
        case E_HAL_DISP_DMA_PIX_FMT_P422:
            pstForCfg->u8bEn[0]       = 1;
            pstForCfg->u8bEn[1]       = 1;
            pstForCfg->u8bEn[2]       = 1;
            pstForCfg->u8fifosize[0]  = 1;
            pstForCfg->u8fifosize[1]  = 0;
            pstForCfg->u8fifosize[2]  = 0;
            pstForCfg->u8fifostart[0] = 0;
            pstForCfg->u8fifostart[1] = 2;
            pstForCfg->u8fifostart[2] = 3;
            pstForCfg->u16Width[0] =
                DISP_SET_BUF_WIDTH(DISP_ALIGN_32(pstDmaContain->stOutputCfg.u16Width) / 16, enComp);
            pstForCfg->u16Width[1]      = pstForCfg->u16Width[0] / 2;
            pstForCfg->u16Width[2]      = pstForCfg->u16Width[1];
            pstForCfg->u16Height[0]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Height[1]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Height[2]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Pixel[0]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Pixel[1]      = pstDmaContain->stOutputCfg.u16Width / 2;
            pstForCfg->u16Pixel[2]      = pstDmaContain->stOutputCfg.u16Width / 2;
            pstForCfg->u16Stride[0]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16Stride[1]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[1]) / 16;
            pstForCfg->u16Stride[2]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[2]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            pstForCfg->u16BuffHeight[1] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            pstForCfg->u16BuffHeight[2] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_ARGB:
        case E_HAL_DISP_DMA_PIX_FMT_BGRA:
        case E_HAL_DISP_DMA_PIX_FMT_ABGR:
        case E_HAL_DISP_DMA_PIX_FMT_RGBA:
            pstForCfg->u8bEn[0]         = 1;
            pstForCfg->u8fifosize[0]    = 2;
            pstForCfg->u8fifostart[0]   = 0;
            pstForCfg->u16Width[0]      = DISP_SET_BUF_WIDTH(pstDmaContain->stOutputCfg.u16Width / 4, enComp);
            pstForCfg->u16Height[0]     = pstDmaContain->stOutputCfg.u16Height;
            pstForCfg->u16Pixel[0]      = pstDmaContain->stOutputCfg.u16Width;
            pstForCfg->u16Stride[0]     = DISP_ALIGN_16(pstDmaContain->stBufferAttrCfg.u32Stride[0]) / 16;
            pstForCfg->u16BuffHeight[0] = DISP_SET_RING_BUF_HEIGHT(pstDmaContain->stBufferAttrCfg.u16RingBuffHeight);
            break;
        default:
            break;
    }
}
static void _HalDispDmaIfSetColorFormat(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    u16                     u16PixFmt[2];
    u16                     u16Cfilter = 0x0004;
    HalDispDmaPixelFormat_e ePixFmt;
    u8                      u8bY2R = 0;

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    ePixFmt       = pstDmaContain->stOutputCfg.ePixelFormat;
    u16PixFmt[0]  = 0;
    u16PixFmt[1]  = 0;
    switch (ePixFmt)
    {
        case E_HAL_DISP_DMA_PIX_FMT_YUYV:
        case E_HAL_DISP_DMA_PIX_FMT_YVYU:
        case E_HAL_DISP_DMA_PIX_FMT_UYVY:
        case E_HAL_DISP_DMA_PIX_FMT_VYUY:
            u16PixFmt[0] = (mask_of_disp_wdma_reg_sc_frm_w_422_pack);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_SP420NV21:
        case E_HAL_DISP_DMA_PIX_FMT_SP420:
            u16PixFmt[0] = (mask_of_disp_wdma_reg_sc_frm_w_422to420_md);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_SP422:
            u16PixFmt[0] = (mask_of_disp_wdma_reg_sc_frm_w_422_yc_sep);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_P420:
            u16PixFmt[0] = (mask_of_disp_wdma_reg_sc_frm_w_422to420_md | mask_of_disp_wdma_reg_sc_frm_w_420_planer);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_P422:
            u16PixFmt[0] = (mask_of_disp_wdma_reg_sc_frm_w_422_planer);
            break;
        case E_HAL_DISP_DMA_PIX_FMT_ARGB:
        case E_HAL_DISP_DMA_PIX_FMT_BGRA:
        case E_HAL_DISP_DMA_PIX_FMT_ABGR:
        case E_HAL_DISP_DMA_PIX_FMT_RGBA:
            u16PixFmt[0] = mask_of_disp_wdma_reg_sc_frm_w_mdma_argb_mode;
            u16PixFmt[1] = mask_of_disp_wdma_reg_sc_frm_w_444_pack;
            u8bY2R       = 1;
            break;
        case E_HAL_DISP_DMA_PIX_FMT_RGB565:
            u16PixFmt[0] = mask_of_disp_wdma_reg_sc_frm_w_422_pack | mask_of_disp_wdma_reg_sc_frm_w_rgb565
                           | mask_of_disp_wdma_reg_sc_frm_w_mdma_argb_mode;
            u8bY2R = 1;
        default:
            break;
    }
    if (ePixFmt >= E_HAL_DISP_DMA_PIX_FMT_ARGB && ePixFmt <= E_HAL_DISP_DMA_PIX_FMT_BGRA)
    {
        if (ePixFmt == E_HAL_DISP_DMA_PIX_FMT_ARGB)
        {
            u16PixFmt[1] |= (1 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) | (2 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT)
                            | (3 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else if (ePixFmt == E_HAL_DISP_DMA_PIX_FMT_ABGR)
        {
            u16PixFmt[1] |= (3 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) | (2 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT)
                            | (1 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else if (ePixFmt == E_HAL_DISP_DMA_PIX_FMT_RGBA)
        {
            u16PixFmt[1] |= (0 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) | (1 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT)
                            | (2 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else
        {
            u16PixFmt[1] |= (2 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) | (1 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT)
                            | (0 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
    }

    u16Cfilter |= (HAL_DISP_IS_YCSWAP(ePixFmt)) ? 0x2 : 0;
    u16Cfilter |= (HAL_DISP_IS_UVSWAP(ePixFmt)) ? 0x1 : 0;
    HalDispDmaSetCscY2RMode(0, pstDispCtxCfg);
    HalDispDmaSetCscY2RMatrix(0, pstDispCtxCfg);
    HalDispDmaSetCscY2RPortEn(u8bY2R, pstDispCtxCfg);
    HalDispDmaSetFrmWPixelFormat(u16PixFmt[0], u16PixFmt[1], (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWCFilter(u16Cfilter, (void *)pstDispCtxCfg);
}

static void _HalDispDmaIfSetSize(DrvDispCtxConfig_t *pstDispCtxCfg, HalDispFormatRela_t *pstForCfg)
{
    DrvDispCtxDmaContain_t *  pstDmaContain = NULL;
    HalDispDmaOutputConfig_t *pstOutCfg     = NULL;
    HalDispDmaInputConfig_t * pstInputCfg   = NULL;
    u8                        bHScalingEn, bVScalingEn;
    u32                       u32Hratio, u32Vratio;
    u16                       au16CvbsWidth[2]     = {0, 0};
    u16                       au16CvbsBufWidth[2]  = {0, 0};
    u16                       au16CvbsHeight[2]    = {0, 0};
    u16                       au16CvbsBufHeight[2] = {0, 0};

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstOutCfg     = &pstDmaContain->stOutputCfg;
    pstInputCfg   = &pstDmaContain->stInputCfg;

    if (pstInputCfg->u16Width != pstOutCfg->u16Width)
    {
        bHScalingEn = 1;
        u32Hratio   = HAL_DISP_DMA_SCALING_RATIO(pstInputCfg->u16Width, pstOutCfg->u16Width);
    }
    else
    {
        bHScalingEn = 0;
        u32Hratio   = 0;
    }

    if (pstInputCfg->u16Height != pstOutCfg->u16Height)
    {
        bVScalingEn = 1;
        u32Vratio   = HAL_DISP_DMA_SCALING_RATIO(pstInputCfg->u16Height, pstOutCfg->u16Height);
    }
    else
    {
        bVScalingEn = 0;
        u32Vratio   = 0;
    }

    if (DISP_DMA_IS_RING_MODE(pstOutCfg->eMode))
    {
        au16CvbsWidth[0] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
        au16CvbsWidth[1] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
        ;
        au16CvbsHeight[0] = pstOutCfg->u16Height;
        au16CvbsHeight[1] = pstDmaContain->stOutputCfg.ePixelFormat == E_HAL_DISP_DMA_PIX_FMT_SP420
                                ? pstOutCfg->u16Height / 2
                                : pstOutCfg->u16Height;

        au16CvbsBufWidth[0]  = 0;
        au16CvbsBufWidth[1]  = 0;
        au16CvbsBufHeight[0] = 0;
        au16CvbsBufHeight[1] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
    }

    HalDispDmaSetDdaInSize(pstInputCfg->u16Width, pstInputCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaOutSize(pstOutCfg->u16Width, pstOutCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaHratio(bHScalingEn, u32Hratio, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaVratio(bVScalingEn, u32Vratio, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsSize(au16CvbsWidth, au16CvbsHeight, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsBufSize(au16CvbsBufWidth, au16CvbsBufHeight, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWFifoSizeByFormat(pstForCfg->u8fifosize, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWFifoStartByFormat(pstForCfg->u8fifostart, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWWidthByFormat(pstForCfg->u16Width, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWHeightByFormat(pstForCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWPixelByFormat(pstForCfg->u16Pixel, (void *)pstDispCtxCfg);
}

static void _HalDispDmaIfSetFrmWConfig(DrvDispCtxConfig_t *pstDispCtxCfg, HalDispFormatRela_t *pstForCfg)
{
    DrvDispCtxDmaContain_t *   pstDmaContain     = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain  = NULL;
    HalDispDmaOutputConfig_t * pstOutCfg         = NULL;
    HalDispDmaInputConfig_t *  pstInCfg          = NULL;
    HalDispPixelCompressMode_e enComp            = E_HAL_DISP_COMPRESS_MODE_NONE;
    u16 u16CvbsDest, u16CvbsRingEn, u16CvbsMopMux, u16MiuMux, u16R2Y;

    pstDmaContain     = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDestDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
    pstSrcDevContain  = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstOutCfg         = &pstDmaContain->stOutputCfg;
    pstInCfg          = &pstDmaContain->stInputCfg;
    enComp            = pstOutCfg->eCompressMode;

    if (DISP_DMA_IS_RING_MODE(pstOutCfg->eMode))
    {
        u16CvbsRingEn  = 1;
    }
    else
    {
        u16CvbsRingEn  = 0;
    }

    if (pstOutCfg->eAccessType == E_HAL_DISP_DMA_ACCESS_TYPE_IMI)
    {
        u16CvbsDest = HAL_DISP_DMA_CVBS_DEST_IMI;
    }
    else
    {
        u16CvbsDest = HAL_DISP_DMA_CVBS_DEST_EMI;
    }

    if (pstSrcDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
    {
        u16MiuMux = pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_BACK ? HAL_DISP_DMA_MIU_MUX_BACK_DISP1
                                                                        : HAL_DISP_DMA_MIU_MUX_FRONT_DISP1;
    }
    else
    {
        u16MiuMux = pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_BACK ? HAL_DISP_DMA_MIU_MUX_BACK_DISP0
                                                                        : HAL_DISP_DMA_MIU_MUX_FRONT_DISP0;
    }
    u16R2Y =
        (pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_BACK
         || (g_stInterCfg[pstSrcDevContain->u32DevId].bDispPat && pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_FRONT))
            ? 1
            : 0;
    if (pstDestDevContain)
    {
        if (pstDmaContain->u8RingModeEn != u16CvbsRingEn)
        {
            pstDmaContain->u16RingMdChange = 1;
        }
        else
        {
            pstDmaContain->u16RingMdChange = 0;
        }
        if (u16CvbsRingEn || pstDmaContain->u16RingMdChange)
        {
            if (!DISP_OUTDEV_IS_CVBS(pstDestDevContain->u32Interface))
            {
                HalDispDmaSetTgenExtFrmResetMode(u16CvbsRingEn, (void *)pstDispCtxCfg);
                HalDispDmaEnsureIdle(pstDispCtxCfg, E_DISP_UTILITY_REG_ACCESS_CMDQ);
            }
            HalDispDmaSetFrmWTriggerModeEn(1, pstDispCtxCfg);
            u16CvbsMopMux = (pstDestDevContain->u32DevId == HAL_DISP_DEVICE_ID_0) ? HAL_DISP_DMA_CVBS_MOP_MUX_PATH_0
                                                                                  : HAL_DISP_DMA_CVBS_MOP_MUX_PATH_1;
            HalDispClkSetFpllOdclk(1, pstSrcDevContain->u32DevId, HAL_DISP_FPLL_0_VGA_HDMI);  // src_clk
            HalDispClkSetFpllOdclk(1, pstDestDevContain->u32DevId, HAL_DISP_FPLL_1_CVBS_DAC); // dst_clk
            HalDispDmaSetCvbsMopMux(u16CvbsMopMux, (void *)pstDispCtxCfg);
        }
    }
    HalDispDmaSetR2Y(u16R2Y, (void *)pstDispCtxCfg);
    HalDispDmaSetDispToMiuMux(u16MiuMux, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsDest(u16CvbsDest, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsOffsetEn(u16CvbsRingEn, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsRingEn(u16CvbsRingEn, pstForCfg->u8bEn, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWReqLen((u16CvbsDest == HAL_DISP_DMA_CVBS_DEST_IMI) ? 0 : 5, pstDispCtxCfg);
    HalDispDmaSetFrmWCompessEn(enComp ? 1 : 0, pstDispCtxCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *    pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    void *                  pvCmdqCtx     = NULL;
    s32                     u32UtilityId;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];

    if (HalDispGetUtilityIdByDmaId(pstDmaContain->u32DmaId, &u32UtilityId))
    {
        if (HalDispUtilityInit((u32)u32UtilityId) == 0)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, Utitiy Init Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            if (HalDispUtilityGetCmdqContext(&pvCmdqCtx, (u32)u32UtilityId) == 0)
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
            }
            else
            {
                if (pvCmdqCtx)
                {
                    pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[(u32)u32UtilityId] = pvCmdqCtx;
                }
                else
                {
                    enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                    DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
                }
            }
        }
    }
    else
    {
        DISP_ERR("%s %d Get Cmdq Id Fail\n", __FUNCTION__, __LINE__);
    }

    return enRet;
}

static void _HalDispIfSetDmaInit(void *pCtx, void *pCfg)
{
    HalDispDmaHwInit(pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaDeInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

static void _HalDispIfSetDmaDeInit(void *pCtx, void *pCfg)
{
    HalDispClkSetDma(0, HAL_DISP_DEVICE_ID_0);
    HalDispIrqSetDmaTriggerCount(1, HAL_DISP_DMA_ID_0);
    HalDispDmaHwDeinit(pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *    pstDispCtxCfg = NULL;
    DrvDispCtxConfig_t *    pstSrcCtxCfg  = NULL;
    DrvDispCtxConfig_t *    pstDestCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBindConfig_t *pstDmaBindCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDmaBindCfg = (HalDispDmaBindConfig_t *)pCfg;

    if (pstDmaBindCfg->pDestDevCtx && pstDmaContain->pDestDevContain)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Dest DevContain Already Bind\n", __FUNCTION__, __LINE__);
    }
    else if (pstDmaBindCfg->pSrcDevCtx && pstDmaContain->pSrcDevContain)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Src DevContain Already Bind\n", __FUNCTION__, __LINE__);
    }
    else
    {
        pstDestCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pDestDevCtx;
        pstSrcCtxCfg  = (DrvDispCtxConfig_t *)pstDmaBindCfg->pSrcDevCtx;

        DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, ContainId:%d, DmaId:%d\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                 pstDmaContain->u32DmaId);

        if (pstSrcCtxCfg)
        {
            pstDmaContain->pSrcDevContain  = pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32ContainIdx];
            pstDmaContain->pDestDevContain = NULL;
            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, ContainId:%d DmaId:%d, SrcDevId:%d\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId);
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, SrcCtxCfg NULL\n", __FUNCTION__, __LINE__);
        }
        if (pstDestCtxCfg)
        {
            pstDmaContain->pDestDevContain = pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32ContainIdx];

            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, ContainId:%d DmaId:%d, DestDevId:%d\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     ((DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain)->u32DevId);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, DestCtxCfg NULL\n", __FUNCTION__, __LINE__);
        }
    }

    return enRet;
}

static void _HalDispIfSetDmaBind(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg  = NULL;
    DrvDispCtxDmaContain_t *   pstDmaContain  = NULL;
    DrvDispCtxDeviceContain_t *pstSrcContain  = NULL;
    DrvDispCtxDeviceContain_t *pstDestContain = NULL;

    pstDispCtxCfg  = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain  = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstSrcContain  = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstDestContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
    if (DISP_CHECK_DMA_SRC_DST_CORRECT(pstSrcContain, pstDestContain))
    {
        HalDispClkSetDma(1, pstSrcContain->u32DevId);
        HalDispDmaSetFpll1SrcMux(1, pstSrcContain->u32DevId, pCtx);
        HalDispDmaSetVideoPathFpllRefSel(0x03, pCtx);
        HalDispDmaSetVideoPathFpllDly(HAL_DISP_VIDEO_PATH_FPLL_DLY, pCtx);
        HalDispDmaSetVsSel(1, pCtx);
        HalDispDmaSetFrmWTriggerModeCfg(1, pCtx);
        HalDispDmaSetFrmWTriggerModeEn(1, pCtx);
        HalDispDmaSetFrmWEn(1, pCtx);
        HalDispDmaClearIrqWriteDone(pCtx);
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8Dmaused = 1;
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8DmaBindDevSrc= pstSrcContain->u32DevId;
        if(pstDestContain)
        {
            g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8DmaBindDevSrc= pstDestContain->u32DevId;
        }
    }
    else
    {
        DISP_ERR("%s %d, SrcDevId:%d, DestDevId:%d, is not legal\n", __FUNCTION__, __LINE__, pstSrcContain->u32DevId,
                 pstDestContain->u32DevId);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaUnBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *    pstDispCtxCfg = NULL;
    DrvDispCtxConfig_t *    pstSrcCtxCfg  = NULL;
    DrvDispCtxConfig_t *    pstDestCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBindConfig_t *pstDmaBindCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDmaBindCfg = (HalDispDmaBindConfig_t *)pCfg;

    pstSrcCtxCfg  = (DrvDispCtxConfig_t *)pstDmaBindCfg->pSrcDevCtx;
    pstDestCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pDestDevCtx;

    if (pstSrcCtxCfg)
    {
        if (pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32ContainIdx] != pstDmaContain->pSrcDevContain)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, ContainId:%d DmaId:%d, SrcContain Not Match %px != %px\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32ContainIdx], pstDmaContain->pSrcDevContain);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, ContainId:%d, DmaId:%d, SrcDevId:%d\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId);
        }
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8Dmaused = 0;
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8DmaBindDevSrc= 0xFF;
    }
    else
    {
        DISP_ERR("%s %d, NULL Ctx, SrcCtx:%px\n", __FUNCTION__, __LINE__, pstSrcCtxCfg);
    }
    if (pstDestCtxCfg)
    {
        if (pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32ContainIdx] != pstDmaContain->pDestDevContain)
        {
            DISP_ERR("%s %d, ContainId:%d DmaId:%d, DestContain Not Match %px != %px\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32ContainIdx],
                     pstDmaContain->pDestDevContain);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, ContainId:%d, DmaId:%d, DestDevId:%d\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDmaContain->u32DmaId,
                     ((DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain)->u32DevId);
        }
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[pstDmaContain->u32DmaId].u8DmaBindDevSrc= 0xFF;
    }
    else
    {
        DISP_ERR("%s %d, NULL Ctx, DestCtx:%px \n", __FUNCTION__, __LINE__, pstDestCtxCfg);
    }
    return enRet;
}

static void _HalDispIfSetDmaUnBind(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDmaContain_t *   pstDmaContain    = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstDstDevContain = NULL;

    pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain    = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstSrcDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstDstDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;

    HalDispDmaEnsureIdle(pCtx, E_DISP_UTILITY_REG_ACCESS_CPU);
    HalDispDmaSetFrmWEn(0, pCtx);
    HalDispClkSetDma(0, HAL_DISP_DEVICE_ID_0);
    HalDispDmaSetFpll1SrcMux(0, HAL_DISP_DEVICE_ID_0, pCtx);

    if (DISP_CHECK_FPLL_NEED_TO_SETUP(pstDmaContain->stOutputCfg.eMode, pstSrcDevContain->u32Interface,
                                      pstDstDevContain->u32Interface))
    {
        HalDispSetFpllEn(HAL_DISP_FPLL_1_CVBS_DAC, 0, (void *)pstDispCtxCfg);
        HalDispSetFpllRst(HAL_DISP_FPLL_1_CVBS_DAC, 1, (void *)pstDispCtxCfg);
        HalDispSetFpllUsedFlag(HAL_DISP_FPLL_1_CVBS_DAC, HAL_DISP_DEVICE_MAX, (void *)pstDispCtxCfg);
        HalDispIrqSetFpllEn(HAL_DISP_FPLL_1_CVBS_DAC, 0);
        HalDispDmaSetTvencSynthSel(0, (void *)pstDispCtxCfg);
    }
    HalDispDmaSetTgenExtFrmResetMode(0, (void *)pstDispCtxCfg);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(0), REG_CMDQ_IN_PROCESS_OFF,
                                             0xFFFF);
    pstDmaContain->pDestDevContain = NULL;
    pstDmaContain->pSrcDevContain  = NULL;
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *    pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaAttrConfig_t *pstDmaAttrCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDmaAttrCfg = (HalDispDmaAttrConfig_t *)pCfg;

    if (pstDmaContain->pSrcDevContain == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContain NULL, SrcDevContain:%px, DestDevContain:%px \n", __FUNCTION__, __LINE__,
                 pstDmaContain->pSrcDevContain, pstDmaContain->pDestDevContain);
    }
    else if (pstDmaContain->pDestDevContain == NULL
             && pstDmaAttrCfg->stOutputCfg.eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DestDevContain is NULL, Can't use Ring Mode \n", __FUNCTION__, __LINE__);
    }
    else
    {
        memcpy(&pstDmaContain->stInputCfg, &pstDmaAttrCfg->stInputCfg, sizeof(HalDispDmaInputConfig_t));
        memcpy(&pstDmaContain->stOutputCfg, &pstDmaAttrCfg->stOutputCfg, sizeof(HalDispDmaOutputConfig_t));
        if (pstDmaAttrCfg->stOutputCfg.eCompressMode
            && pstDmaAttrCfg->stOutputCfg.u16Width % HAL_DISP_COMPRESS_DMA_WIDTH_ALIGN)
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d,Compress Enable, Width %u not align %u  \n", __FUNCTION__, __LINE__,
                     pstDmaAttrCfg->stOutputCfg.u16Width, HAL_DISP_COMPRESS_DMA_WIDTH_ALIGN);
        }
        DISP_DBG(DISP_DBG_LEVEL_HAL_DMA, "%s %d, SrcDevId:%d, In(%s %s %s, %d %d) Out(%s %s %s %s %d %d)\n", __FUNCTION__,
                 __LINE__, ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId,
                 PARSING_HAL_DMA_INPUT_TYPE(pstDmaContain->stInputCfg.eType),
                 PARSING_HAL_DMA_PIXEL_FMT(pstDmaContain->stInputCfg.ePixelFormat),
                 PARSING_HAL_COMPRESS_MD(pstDmaContain->stInputCfg.eCompressMode), pstDmaContain->stInputCfg.u16Width,
                 pstDmaContain->stInputCfg.u16Height,
                 PARSING_HAL_DMA_ACCESS_TYPE(pstDmaContain->stOutputCfg.eAccessType),
                 PARSING_HAL_DMA_OUTPUT_MODE(pstDmaContain->stOutputCfg.eMode),
                 PARSING_HAL_DMA_PIXEL_FMT(pstDmaContain->stOutputCfg.ePixelFormat),
                 PARSING_HAL_COMPRESS_MD(pstDmaContain->stOutputCfg.eCompressMode), pstDmaContain->stOutputCfg.u16Width,
                 pstDmaContain->stOutputCfg.u16Height);
    }
    return enRet;
}

static void _HalDispIfSetDmaAttr(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispFormatRela_t     stForCfg;

    if (pstDispCtxCfg && pstDispCtxCfg->pstCtxContain)
    {
        pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    }
    if (pstDmaContain)
    {
        _HalDispSetDataStructByFormat(pstDispCtxCfg, &stForCfg);
        _HalDispDmaIfSetFrmWConfig(pstDispCtxCfg, &stForCfg);
        _HalDispDmaIfSetColorFormat(pstDispCtxCfg);
        _HalDispDmaIfSetSize(pstDispCtxCfg, &stForCfg);
        _HalDispDmaIfSetFpll(pstDispCtxCfg);
        HalDispIrqEnableVsync(1, pstDmaContain->u32DmaId, (void *)pstDispCtxCfg);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDmaBufferAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e             enRet               = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *          pstDispCtxCfg       = NULL;
    DrvDispCtxDmaContain_t *      pstDmaContain       = NULL;
    HalDispDmaBufferAttrConfig_t *pstDmaBufferAttrCfg = NULL;

    pstDispCtxCfg       = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain       = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDmaBufferAttrCfg = (HalDispDmaBufferAttrConfig_t *)pCfg;

    if (pstDmaContain->pSrcDevContain == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContain NULL, SrcDevContain:%px, DestDevContain:%px \n", __FUNCTION__, __LINE__,
                 pstDmaContain->pSrcDevContain, pstDmaContain->pDestDevContain);
    }
    else
    {
        memcpy(&pstDmaContain->stBufferAttrCfg, pstDmaBufferAttrCfg, sizeof(HalDispDmaBufferAttrConfig_t));
        if (pstDmaContain->stBufferAttrCfg.u32Stride[0]
            % HAL_DISP_SUPPORT_DMA_STRIDE_ALIGN(pstDmaContain->stOutputCfg.ePixelFormat,
                                                pstDmaContain->stOutputCfg.eCompressMode))
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, Stride %u not align %u  \n", __FUNCTION__, __LINE__,
                     pstDmaContain->stBufferAttrCfg.u32Stride[0],
                     HAL_DISP_SUPPORT_DMA_STRIDE_ALIGN(pstDmaContain->stOutputCfg.ePixelFormat,
                                                       pstDmaContain->stOutputCfg.eCompressMode));
        }
        DISP_DBG(DISP_DBG_LEVEL_HAL_DMA,
                 "%s %d, En:%d, PhyAddr(%x %llx %llx) Stride(%d %d %d), RingBuffHeight:%d, RingStartLine:%d, "
                 "FrameIdx:%d\n",
                 __FUNCTION__, __LINE__, pstDmaContain->stBufferAttrCfg.bEn,
                 pstDmaContain->stBufferAttrCfg.paPhyAddr[0], pstDmaContain->stBufferAttrCfg.paPhyAddr[1],
                 pstDmaContain->stBufferAttrCfg.paPhyAddr[2], pstDmaContain->stBufferAttrCfg.u32Stride[0],
                 pstDmaContain->stBufferAttrCfg.u32Stride[1], pstDmaContain->stBufferAttrCfg.u32Stride[2],
                 pstDmaContain->stBufferAttrCfg.u16RingBuffHeight, pstDmaContain->stBufferAttrCfg.u16RingStartLine,
                 pstDmaContain->stBufferAttrCfg.u16FrameIdx);
    }

    return enRet;
}

static void _HalDispIfSetDmaBufferAttr(void *pCtx, void *pCfg)
{
    u8                            i;
    u32                           au32PhyAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32                           au32RingStartAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32                           au32RingEndAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32                           au32ImiOffset[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u16                           u16FrameIdx;
    DrvDispCtxConfig_t *          pstDispCtxCfg       = NULL;
    DrvDispCtxDmaContain_t *      pstDmaContain       = NULL;
    HalDispDmaBufferAttrConfig_t *pstDmaBufferAttrCfg = NULL;
    HalDispFormatRela_t           stForCfg;

    pstDispCtxCfg       = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain       = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32ContainIdx];
    pstDmaBufferAttrCfg = &pstDmaContain->stBufferAttrCfg;
    _HalDispSetDataStructByFormat(pstDispCtxCfg, &stForCfg);
    u16FrameIdx = DISP_DMA_IS_RING_MODE(pstDmaContain->stOutputCfg.eMode) ? pstDmaBufferAttrCfg->u16FrameIdx : 0;

    for (i = 0; i < HAL_DISP_DMA_PIX_FMT_PLANE_MAX; i++)
    {
        au32PhyAddr[i] = pstDmaBufferAttrCfg->paPhyAddr[i] >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
        if (DISP_DMA_IS_RING_MODE(pstDmaContain->stOutputCfg.eMode))
        {
            au32RingStartAddr[i] = (pstDmaBufferAttrCfg->u16RingStartLine * pstDmaBufferAttrCfg->u32Stride[i])
                                   >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;

            au32RingEndAddr[i] = i == 1
                                     ? ((pstDmaBufferAttrCfg->u16RingBuffHeight / 2 * pstDmaBufferAttrCfg->u32Stride[i])
                                        >> HAL_DISP_DMA_BUFFER_SHIFT_BIT)
                                           - 1
                                     : ((pstDmaBufferAttrCfg->u16RingBuffHeight * pstDmaBufferAttrCfg->u32Stride[i])
                                        >> HAL_DISP_DMA_BUFFER_SHIFT_BIT)
                                           - 1;

            au32ImiOffset[i] = au32RingEndAddr[i] + 1;
        }
        else
        {
            au32RingStartAddr[i] = 0;
            au32RingEndAddr[i]   = 0;
            au32ImiOffset[i]     = 0;
        }
    }
    DISP_DBG(DISP_DBG_LEVEL_HAL_DMA,
             "%s %d, En:%d, Base(%08x, %08x), Pixel(%d, %d), Burst Width(%d, %d), Stirde(%d, %d), Burst Stirde(%d, "
             "%d), RingSt(%08x, %08x) RingEnd(%08x, %08x), CvbsImiSt(%08x, %08x) CvbsImiOffset(%08x, %08x)\n",
             __FUNCTION__, __LINE__, pstDmaBufferAttrCfg->bEn, au32PhyAddr[0], au32PhyAddr[1], stForCfg.u16Pixel[0],
             stForCfg.u16Pixel[1], stForCfg.u16Width[0], stForCfg.u16Width[1], pstDmaBufferAttrCfg->u32Stride[0],
             pstDmaBufferAttrCfg->u32Stride[1], stForCfg.u16Stride[0], stForCfg.u16Stride[1], au32RingStartAddr[0],
             au32RingStartAddr[1], au32RingEndAddr[0], au32RingEndAddr[1], au32PhyAddr[0], au32PhyAddr[1],
             au32ImiOffset[0], au32ImiOffset[1]);
    HalDispDmaSetRegMonitor(1, pCtx);
    if (pstDmaBufferAttrCfg->u16RingBuffHeight > 8)
    {
        HalDispDmaSetVideoPathFpllDly((HAL_DISP_VIDEO_PATH_FPLL_DLY > pstDmaBufferAttrCfg->u16RingBuffHeight)
                                          ? pstDmaBufferAttrCfg->u16RingBuffHeight - 8
                                          : HAL_DISP_VIDEO_PATH_FPLL_DLY,
                                      pCtx);
    }
    HalDispDmaSetFrmWBaseAddr(au32PhyAddr, u16FrameIdx, pCtx);
    HalDispDmaSetCvbsImiStartAddr(au32PhyAddr, pCtx);
    HalDispDmaSetCvbsImiOffset(au32ImiOffset, pCtx);
    HalDispDmaSetFrmWPitchByFormat(stForCfg.u16Stride, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWEnByFormat(stForCfg.u8bEn, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWCrc16(stForCfg.u8bEn, pstDmaBufferAttrCfg->enCrcTpye, pCtx);
    HalDispDmaSwRst(pCtx, E_HAL_DISP_DMA_RST_WOPATH_L, E_DISP_UTILITY_REG_ACCESS_CMDQ);
    HalDispDmaClearIrqWriteDone((void *)pstDispCtxCfg);
    HalDispIrqEnableDoubleVsync(pstDmaBufferAttrCfg->bEn, pCtx);
    if (DISP_DMA_IS_RING_MODE(pstDmaContain->stOutputCfg.eMode))
    {
        pstDmaContain->u8RingModeEn = 1;
        HalDispDmaSetFrmWRingBuffHeight(stForCfg.u16BuffHeight, pCtx);
        HalDispDmaSetFrmWTriggerModeEn(pstDmaBufferAttrCfg->bEn ? 0 : 1, pCtx);
        HalDispDmaSetFrmWTrigger(pstDmaBufferAttrCfg->bEn, pCtx);
        HalDispDmaSetFrmWEn(1, pCtx);
    }
    else
    {
        pstDmaContain->u8RingModeEn = 0;
        HalDispDmaSetFrmWTriggerModeEn(1, pCtx);
        HalDispDmaSetFrmWTrigger(pstDmaBufferAttrCfg->bEn, pCtx);
    }
    if (pstDmaBufferAttrCfg->bEn)
    {
        HalDispIrqSetDmaTriggerCount(0, pstDmaContain->u32DmaId);
    }
    HalDispDmaSetRegMonitor(0, pCtx);
    DISP_DBG_VAL(au32RingStartAddr[0]);
}

u8 HalDispDmaIfInit(void *pCtx)
{
    HalDispQueryCallBackFunc_t *pDispCbTbl = pCtx;

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].pGetInfoFunc = _HalDispIfGetInfoDmaInit;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].pSetFunc     = _HalDispIfSetDmaInit;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].u16CfgSize   = 0;

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].pGetInfoFunc = _HalDispIfGetInfoDmaDeInit;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].pSetFunc     = _HalDispIfSetDmaDeInit;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].u16CfgSize   = 0;

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].pGetInfoFunc = _HalDispIfGetInfoDmaBind;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].pSetFunc     = _HalDispIfSetDmaBind;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].u16CfgSize   = sizeof(HalDispDmaBindConfig_t);

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].pGetInfoFunc = _HalDispIfGetInfoDmaUnBind;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].pSetFunc     = _HalDispIfSetDmaUnBind;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].u16CfgSize   = sizeof(HalDispDmaBindConfig_t);

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].pGetInfoFunc = _HalDispIfGetInfoDmaAttr;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].pSetFunc     = _HalDispIfSetDmaAttr;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].u16CfgSize   = sizeof(HalDispDmaAttrConfig_t);

    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].pGetInfoFunc = _HalDispIfGetInfoDmaBufferAttr;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].pSetFunc     = _HalDispIfSetDmaBufferAttr;
    pDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].u16CfgSize   = sizeof(HalDispDmaBufferAttrConfig_t);
    return 1;
}
u8 HalDispDmaIfDeInit(void *pCtx)
{
    return 1;
}
