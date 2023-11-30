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
#include "drv_disp_ctx.h"
#include "hal_disp_dma.h"
#include "hal_disp_dma_if.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 u32FpllHdmiSynthSet[E_HAL_DISP_OUTPUT_MAX] =
{
    0x00000000, // PAL
    0x00000000, // NTSC
    0x00000000, // 960H_PAL
    0x00000000, // 960H_NTSC
    0x00000000, // 1080P24
    0x00000000, // 1080P25
    0x00000000, // 1080P30
    0x002E8BA2, // 720P50
    0x002E978D, // 720P60
    0x00000000, // 1080I50
    0x00000000, // 1080I60
    0x002E8BA2, // 1080P50
    0x002E978D, // 1080P60
    0x00400000, // 576P50
    0x00400000, // 480P60
    0x00000000, // 640x480_60
    0x00000000, // 800x600_60
    0x003539CE, // 1024x768_60
    0x00200AE3, // 1280x1024_60
    0x002851CD, // 1366x768_60
    0x00206C16, // 1440x900_60
    0x00294CB5, // 1280x800_60
    0x0017A2E8, // 1680x1050_60
    0x00000000, // 1920x2160_30
    0x002AB596, // 1600x1200_60
    0x00000000, // 1920x1200_60
    0x00000000, // 2560x1440_30
    0x00000000, // 2560x1600_60
    0x00000000, // 3840x2160_30
    0x00000000, // 3840x2160_60
    0x00000000, // USER
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void _HalDispDmaIfSetFpll(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain = NULL;
    HalDispDmaOutputConfig_t *pstOutCfg = NULL;

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDestDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
    pstSrcDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstOutCfg = &pstDmaContain->stOutputCfg;


    if((pstDestDevContain->u32Interface ==  HAL_DISP_INTF_CVBS) &&
       (pstSrcDevContain->u32Interface & HAL_DISP_INTF_HDMI) &&
        pstOutCfg->eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
    {
        if(u32FpllHdmiSynthSet[pstSrcDevContain->eTimeType])
        {
            HalDispDmaSetHdmiSynthSet(u32FpllHdmiSynthSet[pstSrcDevContain->eTimeType], (void *)pstDispCtxCfg);
            HalDispDmaSetTvencSynthSet(0x00400000, (void *)pstDispCtxCfg);
            HalDispDmaSetTvencSynthSel(1, (void *)pstDispCtxCfg);

            HalDispDmaSetFpllGain(0x88, (void *)pstDispCtxCfg);
            HalDispDmaSetFpllThd(0x90, (void *)pstDispCtxCfg);
            HalDispDmaSetFpllAbsLimit(0x10000, (void *)pstDispCtxCfg);
            HalDispDmaSetFpllSwRst(0, (void *)pstDispCtxCfg);
            HalDispDmaSetFpllEn(1, (void *)pstDispCtxCfg);
            HalDispDmaWaitFpllLock((void *)pstDispCtxCfg);
        }
    }
}

void _HalDispDmaIfSetColorFormat(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    bool bFrmW32b;
    u16 u16PixFmt[2];
    u16 u16Val;
    HalDispDmaPixelFormat_e ePixFmt;

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    ePixFmt = pstDmaContain->stOutputCfg.ePixelFormat;

    u16PixFmt[0] = 0;
    u16PixFmt[1] = 0;

    //u16PixFmt[0] |= ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YUV420    ? HAL_DISP_DMA_FRM_W_422_TO_420 :
    //                ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YUVSep420 ? HAL_DISP_DMA_FRM_W_422_TO_420 :
    //                                                              0;

    u16PixFmt[0] |= ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YUVSep420 ? HAL_DISP_DMA_FRM_W_420_PLANER :
                    ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YCSep422  ? HAL_DISP_DMA_FRM_W_422_SEP:
                    ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YUVSep422 ? HAL_DISP_DMA_FRM_W_422_PLANER:
                                                                  0;

    bFrmW32b = ((ePixFmt == E_HAL_DISP_DMA_PIX_FMT_YUV422) ||
                (ePixFmt >= E_HAL_DISP_DMA_PIX_FMT_ARGB &&  ePixFmt <= E_HAL_DISP_DMA_PIX_FMT_BGRA)) ? 1 : 0;

    u16PixFmt[0] |= bFrmW32b ? (HAL_DISP_DMA_FRM_W_32B | HAL_DISP_DMA_FRM_W_32B_EXT) : 0;

    if(ePixFmt >= E_HAL_DISP_DMA_PIX_FMT_ARGB && ePixFmt <= E_HAL_DISP_DMA_PIX_FMT_BGRA)
    {
        u16PixFmt[0] |= HAL_DISP_DMA_FRM_W_ARGB_MD_ARGB;
        u16PixFmt[1] |= HAL_DISP_DMA_FRM_W_444_PACKET_MD_444;
        u16PixFmt[0] |= HAL_DISP_DMA_FRM_W_422_PACKET_OFF;

        if(ePixFmt == E_HAL_DISP_DMA_PIX_FMT_ARGB)
        {
            u16PixFmt[1] |=  (1 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) |
                             (2 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT) |
                             (3 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else if(ePixFmt == E_HAL_DISP_DMA_PIX_FMT_ABGR)
        {
            u16PixFmt[1] |=  (3 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) |
                             (2 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT) |
                             (1 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else if(ePixFmt == E_HAL_DISP_DMA_PIX_FMT_RGBA)
        {
            u16PixFmt[1] |=  (0 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) |
                             (1 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT) |
                             (2 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
        }
        else
        {
            u16PixFmt[1] |=  (2 << HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT) |
                             (1 << HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT) |
                             (0 << HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT);
         }
    }
    else
    {
        u16PixFmt[0] |= HAL_DISP_DMA_FRM_W_ARGB_MD_YUV;
        u16PixFmt[1] |= HAL_DISP_DMA_FRM_W_444_PACKET_MD_422_420;
        u16PixFmt[0] |=
            ePixFmt ==  E_HAL_DISP_DMA_PIX_FMT_YUV422 ?
            HAL_DISP_DMA_FRM_W_422_PACKET_ON :
            HAL_DISP_DMA_FRM_W_422_PACKET_OFF;
    }

    HalDispDmaSetFrmWPixelFormat(u16PixFmt[0], u16PixFmt[1], (void *)pstDispCtxCfg);

    u16Val = bFrmW32b ? 0x7E : 0x30;
    HalDispDmaSetFrmWReqLen(u16Val,  (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWMaxLenghtR0(u16Val,  (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWMaxLenghtR1(u16Val,  (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWMaxLenghtAll(u16Val,  (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWReqTh(u16Val,  (void *)pstDispCtxCfg);
    //HalDispDmaSetFrmWCFilter(0x0004,  (void *)pstDispCtxCfg);

    HalDispDmaSetFrmWReqLenC(0x18,  (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWReqThC(0x18,  (void *)pstDispCtxCfg);
}

void _HalDispDmaIfSetSize(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaOutputConfig_t *pstOutCfg = NULL;
    HalDispDmaInputConfig_t *pstInputCfg = NULL;
    bool bHScalingEn, bVScalingEn;
    u32 u32Hratio, u32Vratio;
    u16 au16CvbsWidth[2] = {0, 0};
    u16 au16CvbsBufWidth[2] = {0, 0};
    u16 au16CvbsHeight[2] = {0, 0};
    u16 au16CvbsBufHeight[2] = {0, 0};

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstOutCfg = &pstDmaContain->stOutputCfg;
    pstInputCfg = &pstDmaContain->stInputCfg;

     if(pstInputCfg->u16Width != pstOutCfg->u16Width)
    {
        bHScalingEn = 1;
        u32Hratio = HAL_DISP_DMA_SCALING_RATIO(pstInputCfg->u16Width, pstOutCfg->u16Width);
    }
    else
    {
        bHScalingEn = 0;
        u32Hratio = 0;
    }

    if(pstInputCfg->u16Height != pstOutCfg->u16Height)
    {
        bVScalingEn = 1;
        u32Vratio = HAL_DISP_DMA_SCALING_RATIO(pstInputCfg->u16Height, pstOutCfg->u16Height);
    }
    else
    {
        bVScalingEn = 0;
        u32Vratio = 0;
    }


    if(pstOutCfg->eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
    {
        au16CvbsWidth[0] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
        au16CvbsWidth[1] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;;
        au16CvbsHeight[0] = pstOutCfg->u16Height;
        au16CvbsHeight[1] =
            pstDmaContain->stOutputCfg.ePixelFormat == E_HAL_DISP_DMA_PIX_FMT_YUV420 ?
           pstOutCfg->u16Height/2 :
           pstOutCfg->u16Height;

        au16CvbsBufWidth[0] = 0;
        au16CvbsBufWidth[1] = 0;
        au16CvbsBufHeight[0] = 0;
        au16CvbsBufHeight[1] = pstOutCfg->u16Width >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;

    }

    HalDispDmaSetDdaInSize(pstInputCfg->u16Width, pstInputCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaOutSize(pstOutCfg->u16Width, pstOutCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaHratio(bHScalingEn, u32Hratio, (void *)pstDispCtxCfg);
    HalDispDmaSetDdaVratio(bVScalingEn, u32Vratio, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWSize(pstOutCfg->u16Width, pstOutCfg->u16Height, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsSize(au16CvbsWidth, au16CvbsHeight, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsBufSize(au16CvbsBufWidth, au16CvbsBufHeight, (void *)pstDispCtxCfg);
}

void _HalDispDmaIfSetFrmWConfig(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain = NULL;
    HalDispDmaOutputConfig_t *pstOutCfg = NULL;
    HalDispDmaInputConfig_t *pstInCfg = NULL;
    u16 u16VencModeSel, u16FrmWIdxMd, u16CvbsDest, u16CvbsRingEn, u16CvbsMopMux, u16MiuMux;

    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDestDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
    pstSrcDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstOutCfg = &pstDmaContain->stOutputCfg;
    pstInCfg = &pstDmaContain->stInputCfg;

    if(pstOutCfg->eMode == E_HAL_DISP_DMA_OUTPUT_MODE_FRAME)
    {
        u16VencModeSel = HAL_DISP_DMA_VEN_MODE_SEL_NO_HANDSHAKE;
        u16FrmWIdxMd = 0x0000;
        u16CvbsRingEn = 0;
    }
    else
    {
        u16VencModeSel = HAL_DISP_DMA_VEN_MODE_SEL_RING;
        u16FrmWIdxMd = 0x0000;
        u16CvbsRingEn = 1;
    }

    if(pstOutCfg->eAccessType == E_HAL_DISP_DMA_ACCESS_TYPE_IMI)
    {
        u16CvbsDest = HAL_DISP_DMA_CVBS_DEST_IMI;

    }
    else
    {
        u16CvbsDest = HAL_DISP_DMA_CVBS_DEST_EMI;
    }

    u16CvbsMopMux = (pstDestDevContain->u32DevId == HAL_DISP_DEVICE_ID_0) ?
                     HAL_DISP_DMA_CVBS_MOP_MUX_PATH_0 :
                     HAL_DISP_DMA_CVBS_MOP_MUX_PATH_1;

    if(pstSrcDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
    {
        u16MiuMux = pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_BACK ?
                    HAL_DISP_DMA_MIU_MUX_BACK_DISP1 :
                    HAL_DISP_DMA_MIU_MUX_FRONT_DISP1 ;
    }
    else
    {
        u16MiuMux = pstInCfg->eType == E_HAL_DISP_DMA_INPUT_DEVICE_BACK ?
                    HAL_DISP_DMA_MIU_MUX_BACK_DISP0 :
                    HAL_DISP_DMA_MIU_MUX_FRONT_DISP0 ;
    }

    HalDispDmaSetDispToMiuMux(u16MiuMux, (void *)pstDispCtxCfg);
    HalDispDmaSetFrmWIdxMd(u16FrmWIdxMd, (void *)pstDispCtxCfg);
    HalDispDmaSetVenModeSel(u16VencModeSel, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsDest(u16CvbsDest, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsMopMux(u16CvbsMopMux,  (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsOffsetEn(u16CvbsRingEn, (void *)pstDispCtxCfg);
    HalDispDmaSetCvbsRingEn(u16CvbsRingEn, (void *)pstDispCtxCfg);

    if(pstDestDevContain->u32Interface != HAL_DISP_INTF_CVBS)
    {
        HalDispDmaSetTgenExtFrmResetMode(u16CvbsRingEn, (void *)pstDispCtxCfg);
    }

}

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
bool HalDispDmaIfInit(void *pCtx)
{
    return 1;
}

bool HalDispDmaIfDeInit(void *pCtx)
{
    HalDispClkSetDma(0, HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0);
    return 1;
}

bool HalDispDmaIfBind(void *pCtx)
{
    bool bRet = 1;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcContain = NULL;
    DrvDispCtxDeviceContain_t *pstDestContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstSrcContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstDestContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;

    if(pstSrcContain->u32DevId == HAL_DISP_DEVICE_ID_0 &&
       pstDestContain->u32DevId == HAL_DISP_DEVICE_ID_1 )
    {
        HalDispClkSetDma(1, HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0);
        HalDispDmaSetFpll1SrcMux(1, HAL_DISP_DEVICE_ID_0, pCtx);

        HalDispClkSetFpllOdclk0(1, HAL_DISP_CLK_FPLL_SEL_ODCLK0); //src_clk
        HalDispClkSetFpllOdclk1(1, HAL_DISP_CLK_FPLL_SEL_ODCLK1); //dst_clk

    }
    else if(pstSrcContain->u32DevId == HAL_DISP_DEVICE_ID_1 &&
            pstDestContain->u32DevId == HAL_DISP_DEVICE_ID_0)
    {
        HalDispClkSetDma(1, HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK1);
        HalDispDmaSetFpll1SrcMux(1, HAL_DISP_DEVICE_ID_1, pCtx);

        HalDispClkSetFpllOdclk0(1, HAL_DISP_CLK_FPLL_SEL_ODCLK1); //src_clk
        HalDispClkSetFpllOdclk1(1, HAL_DISP_CLK_FPLL_SEL_ODCLK0); //dst_clk
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, SrcDevId:%ld, DestDevId:%ld, is not legal\n",
            __FUNCTION__, __LINE__,
            pstSrcContain->u32DevId, pstDestContain->u32DevId);
    }

    HalDispDmaSetVideoPathFpllRefSel(0x03, pCtx);
    HalDispDmaSetVideoPathFpllDly(0x10, pCtx);

    return bRet;
}

bool HalDispDmaIfUnBind(void *pCtx)
{
    bool bRet = 1;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstSrcDevContain = NULL;
    DrvDispCtxDeviceContain_t *pstDstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstSrcDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
    pstDstDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;

    HalDispDmaSetFrmWDbEn(0, pCtx);
    HalDispDmaSetFrmWEn(0, pCtx);

    HalDispClkSetDma(0, HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0);
    HalDispClkSetFpllOdclk1(0, HAL_DISP_CLK_FPLL_SEL_ODCLK0);
    HalDispDmaSetFpll1SrcMux(0, HAL_DISP_DEVICE_ID_0, pCtx);

    if(pstDstDevContain->u32Interface == HAL_DISP_INTF_CVBS &&
       pstSrcDevContain->u32Interface & HAL_DISP_INTF_HDMI)
    {
        HalDispDmaSetFpllEn(0, (void *)pstDispCtxCfg);
        HalDispDmaSetFpllSwRst(1, (void *)pstDispCtxCfg);
        HalDispDmaSetTvencSynthSel(0, (void *)pstDispCtxCfg);
        HalDispDmaSetTvencSynthSet(0x0000, (void *)pstDispCtxCfg);
    }
    return bRet;
}


bool HalDispDmaIfSetAttr(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;

    _HalDispDmaIfSetColorFormat(pstDispCtxCfg);
    _HalDispDmaIfSetSize(pstDispCtxCfg);
    _HalDispDmaIfSetFrmWConfig(pstDispCtxCfg);
    _HalDispDmaIfSetFpll(pstDispCtxCfg);

    return 1;
}

bool HalDispDmaIfSetBufferAttr(void *pCtx)
{
    u8 i;
    u32 au32PhyAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32 au32RingStartAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32 au32RingEndAddr[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u16 au16LineOffset[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u32 au32ImiOffset[HAL_DISP_DMA_PIX_FMT_PLANE_MAX];
    u16 u16FrameIdx;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBufferAttrConfig_t *pstDmaBufferAttrCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDmaBufferAttrCfg = &pstDmaContain->stBufferAttrCfg;

    u16FrameIdx = pstDmaContain->stOutputCfg.eMode == E_HAL_DISP_DMA_OUTPUT_MODE_FRAME ? 0 : pstDmaBufferAttrCfg->u16FrameIdx;

    for(i=0; i< HAL_DISP_DMA_PIX_FMT_PLANE_MAX; i++)
    {
        au32PhyAddr[i] = pstDmaBufferAttrCfg->u64PhyAddr[i] >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;
        au16LineOffset[i] = pstDmaBufferAttrCfg->u32Stride[i];

        if(pstDmaContain->stOutputCfg.eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
        {
            au32RingStartAddr[i] =
                (pstDmaBufferAttrCfg->u16RingStartLine * pstDmaBufferAttrCfg->u32Stride[i]) >> HAL_DISP_DMA_BUFFER_SHIFT_BIT;

            au32RingEndAddr[i]  =
                 i==1 ?
                ((pstDmaBufferAttrCfg->u16RingBuffHeight / 2* pstDmaBufferAttrCfg->u32Stride[i]) >> HAL_DISP_DMA_BUFFER_SHIFT_BIT) - 1 :
                ((pstDmaBufferAttrCfg->u16RingBuffHeight * pstDmaBufferAttrCfg->u32Stride[i]) >> HAL_DISP_DMA_BUFFER_SHIFT_BIT) - 1;

            au32ImiOffset[i] = au32RingEndAddr[i] + 1;
        }
        else
        {
            au32RingStartAddr[i] = 0;
            au32RingEndAddr[i] = 0;
            au32ImiOffset[i] = 0;
        }
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d, Base(%08lx, %08lx), Stirde(%d, %d), RingSt(%08lx, %08lx) RingEnd(%08lx, %08lx), CvbsImiSt(%08lx, %08lx) CvbsImiOffset(%08lx, %08lx)\n",
        __FUNCTION__, __LINE__,
        pstDmaBufferAttrCfg->bEn,
        au32PhyAddr[0], au32PhyAddr[1], au16LineOffset[0], au16LineOffset[1],
        au32RingStartAddr[0], au32RingStartAddr[1], au32RingEndAddr[0], au32RingEndAddr[1],
        au32PhyAddr[0], au32PhyAddr[1], au32ImiOffset[0], au32ImiOffset[1]);

    HalDispDmaSetFrmWBaseAddr(au32PhyAddr, u16FrameIdx, pCtx);
    HalDispDmaSetFrmWLineOffset(1, au16LineOffset, pCtx);
    HalDispDmaSetVenRingStarAddr(au32RingStartAddr, pCtx);
    HalDispDmaSetVenRingEndAddr(au32RingEndAddr, pCtx);
    HalDispDmaSetCvbsImiStartAddr(au32PhyAddr, pCtx);
    HalDispDmaSetCvbsImiOffset(au32ImiOffset, pCtx);
    HalDispDmaSetFrmWDbEn(pstDmaContain->stOutputCfg.eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING ? 1 : 0, pCtx);
    HalDispDmaSetFrmWEn(pstDmaBufferAttrCfg->bEn, pCtx);
    return 1;
}
