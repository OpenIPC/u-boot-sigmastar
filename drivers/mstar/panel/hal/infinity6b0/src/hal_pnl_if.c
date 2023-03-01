/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#define _HAL_PNL_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/string.h>

#include "drv_pnl_os.h"
#include "hal_pnl_common.h"
#include "pnl_debug.h"
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl.h"
#include "drv_pnl_ctx.h"
//padmux
//#include "padmux.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalPnlQueryRet_e (*pGetInfoFunc) (void *, void *);
    void (*pSetFunc) (void * , void *);
    u16  u16CfgSize;
}HalPnlQueryCallBackFunc_t;



//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
bool gbHwIfInit = 0;
HalPnlQueryCallBackFunc_t gpPnlCbTbl[E_HAL_PNL_QUERY_MAX];


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
int __HalPnlIfGetPadMuxMode(u32 u32OutMode, HalPnlLinkType_e eLink, HalPnlOutputFormatBitMode_e eFormatMode)
{
    u32 u32Mode = 0xFF;//PINMUX_FOR_UNKNOWN_MODE;

/*    if (eLink == E_HAL_PNL_LINK_TTL)
    {
        if (eFormatMode == E_HAL_PNL_OUTPUT_565BIT_MODE)
        {
            u32Mode = (u32OutMode == 1) ? PINMUX_FOR_TTL16_MODE_1 :
                      (u32OutMode == 2) ? PINMUX_FOR_TTL16_MODE_2 :
                      (u32OutMode == 3) ? PINMUX_FOR_TTL16_MODE_3 :
                      (u32OutMode == 4) ? PINMUX_FOR_TTL16_MODE_4 :
                                          PINMUX_FOR_UNKNOWN_MODE;
        }
        else
        {
            u32Mode = (u32OutMode == 1) ? PINMUX_FOR_TTL24_MODE :
                                          PINMUX_FOR_UNKNOWN_MODE;
        }
    }
    else if(eLink == E_HAL_PNL_LINK_BT656)
    {
        u32Mode = (u32OutMode == 1) ? PINMUX_FOR_BT656_OUT_MODE_1 :
                  (u32OutMode == 2) ? PINMUX_FOR_BT656_OUT_MODE_2 :
                  (u32OutMode == 3) ? PINMUX_FOR_BT656_OUT_MODE_3 :
                                      PINMUX_FOR_UNKNOWN_MODE;
    }
    else
    {
        return u32Mode;
    }
*/
    return u32Mode;
}

void __HalPnlIfSetPolarityConfig(DrvPnlCtxConfig_t *pPnlCtx, HalPnlParamConfig_t *pParamCfg)
{
    HalPnlHwPolarityConfig_t *pstPolarityCfg;

    pstPolarityCfg = &pPnlCtx->pstHwCtx->stPolarityConfig;

    pstPolarityCfg->bClkInv = pParamCfg->u8InvDCLK;
    pstPolarityCfg->bDeInv  = pParamCfg->u8InvDE;
    pstPolarityCfg->bHsyncInv = pParamCfg->u8InvHSync;
    pstPolarityCfg->bVsyncInv = pParamCfg->u8InvVSync;

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: ClkInv:%d, DeInv:%d, HsyncInv:%d, VsyncInv:%d\n",
        __FUNCTION__, __LINE__,
        pstPolarityCfg->bClkInv, pstPolarityCfg->bDeInv,
        pstPolarityCfg->bHsyncInv, pstPolarityCfg->bVsyncInv);

    HalPnlSetClkInv(pstPolarityCfg->bClkInv);
    HalPnlSetDeInv(pstPolarityCfg->bDeInv);
    HalPnlSetVsyncInv(pstPolarityCfg->bVsyncInv);
    HalPnlSetHsyncInv(pstPolarityCfg->bHsyncInv);
    HalPnlSetVsynRefMd(1);
}

void __HalPnlIfSetLpllConfig(DrvPnlCtxConfig_t *pPnlCtx,  HalPnlParamConfig_t *pParamCfg)
{
    HalPnlHwLpllConfig_t *pstLpllCfg;
    u16 u16LpllIdx;
    u32 u32Dividen, u32Divisor, u32LplLSet;
    u32 u32DclkHz, u32HttVtt, u32Mod;

    pstLpllCfg = &pPnlCtx->pstHwCtx->stLplLConfig;

    u32DclkHz = (u32)(pParamCfg->u16DCLK) * 1000000;
    u32HttVtt = (u32)(pParamCfg->u16VTotal * pParamCfg->u16HTotal);
    u32Mod = u32DclkHz % u32HttVtt;

    if(u32Mod > (u32HttVtt/2))
    {
        pstLpllCfg->u32Fps = (u32DclkHz + u32HttVtt -1) / u32HttVtt;
    }
    else
    {
        pstLpllCfg->u32Fps  = u32DclkHz/u32HttVtt;
    }

    pstLpllCfg->u32Dclk = (u32)pParamCfg->u16DCLK * 1000000;
    u16LpllIdx = HalPnlGetLpllIdx(pstLpllCfg->u32Dclk);

    if(u16LpllIdx == 0xFF)
    {
        PNL_ERR( "[HALPNL]%s %d:: Set clock failed. Wrong lpll table index %d caused by unsupported clock range. Output clock rate(%ldHz) is less than 25Mhz \n",
                 __FUNCTION__, __LINE__, u16LpllIdx, pstLpllCfg->u32Dclk);
        return;
    }

    pstLpllCfg->u16LoopDiv = HalPnlGetLpllDiv(u16LpllIdx);
    pstLpllCfg->u16LoopGain = HalPnlGetLpllGain(u16LpllIdx);

    u32Dividen = (u32)432 * (u64)524288 * (u32)pstLpllCfg->u16LoopGain;
    u32Divisor = u32HttVtt * pstLpllCfg->u32Fps * (u32)pstLpllCfg->u16LoopDiv / 1000000;
    u32LplLSet= u32Dividen / u32Divisor;

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: Idx:%d, LoopGain:%d, LoopDiv:%d, fps=%ld, Divden:0x%lx, Divisor:0x%lx, LpllSe:0x%lx\n",
        __FUNCTION__, __LINE__, u16LpllIdx,
        pstLpllCfg->u16LoopGain, pstLpllCfg->u16LoopDiv, pstLpllCfg->u32Fps,
        u32Dividen, u32Divisor, u32LplLSet);

    HalPnlDumpLpllSetting(u16LpllIdx);
    HalPnlSetLpllSet(u32LplLSet);
}

void __HalPnlIfUnifiedSetLpllConfig(DrvPnlCtxConfig_t *pPnlCtx, u32 u32OutClkRate)
{
    HalPnlHwLpllConfig_t *pstLpllCfg;
    u16 u16LpllIdx;
    u32 u32Dividen, u32Divisor, u32LplLSet;
    //u64 u64Dividen;
    //u64 u64Divisor=0, u64LplLSet;
    pstLpllCfg = &pPnlCtx->pstHwCtx->stLplLConfig;

    pstLpllCfg->u32Dclk = u32OutClkRate;
    u16LpllIdx = HalPnlGetLpllIdx(pstLpllCfg->u32Dclk);
    if(u16LpllIdx == 0xFF)
    {
        PNL_ERR( "[HALPNL]%s %d:: Set clock failed. Wrong lpll table index %d caused by unsupported clock range. Output clock rate(%ldHz) is less than 25Mhz \n",
                 __FUNCTION__, __LINE__, u16LpllIdx, pstLpllCfg->u32Dclk);
        return;
    }
    pstLpllCfg->u16LoopDiv = HalPnlGetLpllDiv(u16LpllIdx);
    pstLpllCfg->u16LoopGain = HalPnlGetLpllGain(u16LpllIdx);


    /*u64Dividen  =   (u64)432 * (u64)524288 * (u64)pstLpllCfg->u16LoopGain;
    u64LplLSet  =   (((u64Dividen) * (10*1000000)) + (((u64)(pstLpllCfg->u32Dclk*10) * (u64)(pstLpllCfg->u16LoopDiv)) >> 1));
    u64LplLSet  =   u64LplLSet / (pstLpllCfg->u32Dclk*10);
    u64LplLSet  =   u64LplLSet / (u32)pstLpllCfg->u16LoopDiv;*/

    u32Dividen = ((u32)432 * (u32)524288 * (u32)pstLpllCfg->u16LoopGain) / (u32)pstLpllCfg->u16LoopDiv;
    u32Divisor = pstLpllCfg->u32Dclk / 1000000;
    u32LplLSet= u32Dividen / u32Divisor;

    /*if( (u16LpllIdx == 6) ||
        (u16LpllIdx == 5))
    {
        if(u64LplLSet < 0x228f5c || u64LplLSet > 0x451eb8)
        {
            PNL_ERR( "[HALPNL]%s %d:: wrong lpll range %x, idx %d\n",
                    __FUNCTION__, __LINE__, (int)u64LplLSet,u16LpllIdx);
            return;
        }
    }
    if(u16LpllIdx == 4)
    {
        if(u64LplLSet < 0x3c2fa6 || u64LplLSet > 0x451eb8)
        {
            PNL_ERR( "[HALPNL]%s %d:: wrong lpll range %x, idx %d\n",
                    __FUNCTION__, __LINE__, (int)u64LplLSet,u16LpllIdx);
            return;
        }
    }
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: Idx:%d, LoopGain:%d, LoopDiv:%d, fps=%ld, Divden:0x%llx, Divisor:0x%llx, LpllSe:0x%x\n",
                            __FUNCTION__, __LINE__, u16LpllIdx,
        pstLpllCfg->u16LoopGain, pstLpllCfg->u16LoopDiv, pstLpllCfg->u32Fps,
        u64Dividen, u64Divisor, (int)u64LplLSet);*/

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: Idx:%d, LoopGain:%d, LoopDiv:%d, fps=%ld, Divden:0x%lx, Divisor:0x%lx, LpllSe:0x%lx\n",
        __FUNCTION__, __LINE__, u16LpllIdx,
        pstLpllCfg->u16LoopGain, pstLpllCfg->u16LoopDiv, pstLpllCfg->u32Fps,
        u32Dividen, u32Divisor, u32LplLSet);
    HalPnlDumpLpllSetting(u16LpllIdx);
    HalPnlSetLpllSet(u32LplLSet);
}

void __HalPnlIfSetSSCConfig(HalPnlParamConfig_t *pParamCfg)
{
    u32 u32SscSpan, u32SscStep;
    if( (pParamCfg->u16SpreadSpectrumStep != 0)  && (pParamCfg->u16SpreadSpectrumSpan != 0) ) {
        //u32Dclk unit: Hz
        #if 0
        u32SscSpan = ( (u32Dclk*u32Lpll_Div)/u32Lpll_Gain)/(4*pCfg->u16SpreadSpectrumSpan);
        u32SscStep =  ((u64)(u64LpllSet*pCfg->u16SpreadSpectrumStep) ) / ((u64)(u32SscSpan*1000));
        #else
        u32SscSpan =  (pParamCfg->u16SpreadSpectrumSpan);
        u32SscStep =  (pParamCfg->u16SpreadSpectrumStep);
        #endif
        PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s::SscSpan=%x, SscStep:%x\n "
                                ,__FUNCTION__, (int)u32SscSpan,  (int)u32SscStep);
        HalPnlSetLSSC(1,u32SscStep, u32SscSpan);
    }
}

void __HalPnlIfSetTimeGenConfig(DrvPnlCtxConfig_t *pPnlCtx,  HalPnlParamConfig_t *pParamCfg)
{
    HalPnlHwTimeGenConfig_t *pstTimeGenCfg;
    u16 u16VsyncSt, u16HsyncSt;
    u16 u16VdeSt, u16HdeSt;

    pstTimeGenCfg = &pPnlCtx->pstHwCtx->stTimeGenCfg;
    pstTimeGenCfg->u16VsyncWidth = pParamCfg->u16VSyncWidth;
    pstTimeGenCfg->u16VsyncBackPorch = pParamCfg->u16VSyncBackPorch;
    pstTimeGenCfg->u16Vactive  = pParamCfg->u16Height;
    pstTimeGenCfg->u16Vstart = pParamCfg->u16VStart;
    pstTimeGenCfg->u16VTotal = pParamCfg->u16VTotal;

    pstTimeGenCfg->u16HsyncWidth = pParamCfg->u16HSyncWidth;
    pstTimeGenCfg->u16HsyncBackPorch = pParamCfg->u16HSyncBackPorch;
    pstTimeGenCfg->u16Hactive  = pParamCfg->u16Width;
    pstTimeGenCfg->u16Hstart = pParamCfg->u16HStart;
    pstTimeGenCfg->u16HTotal = pParamCfg->u16HTotal;


    u16VdeSt = pstTimeGenCfg->u16Vstart;
    u16HdeSt = pstTimeGenCfg->u16Hstart;

    if(u16VdeSt)
    {
        u16VsyncSt = u16VdeSt - pstTimeGenCfg->u16VsyncWidth -  pstTimeGenCfg->u16VsyncBackPorch;
    }
    else
    {
        u16VsyncSt = pstTimeGenCfg->u16VTotal - pstTimeGenCfg->u16VsyncWidth -  pstTimeGenCfg->u16VsyncBackPorch;
    }

    if(u16HdeSt)
    {
        u16HsyncSt = u16HdeSt - pstTimeGenCfg->u16HsyncWidth -  pstTimeGenCfg->u16HsyncBackPorch;
    }
    else
    {
        u16HsyncSt = pstTimeGenCfg->u16HTotal - pstTimeGenCfg->u16HsyncWidth -  pstTimeGenCfg->u16HsyncBackPorch;
    }

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: Hsync(%d %d), Vsync(%d %d) SyncStart(%d %d), DeStart(%d, %d) Size(%d %d), Total(%d %d)\n",
        __FUNCTION__, __LINE__,
        pstTimeGenCfg->u16HsyncWidth, pstTimeGenCfg->u16HsyncBackPorch,
        pstTimeGenCfg->u16VsyncWidth, pstTimeGenCfg->u16VsyncBackPorch,
        u16HsyncSt, u16VsyncSt, u16HdeSt, u16VdeSt,
        pstTimeGenCfg->u16Hactive, pstTimeGenCfg->u16Vactive,
        pstTimeGenCfg->u16HTotal, pstTimeGenCfg->u16VTotal);

    HalPnlSetVSyncSt(u16VsyncSt);
    HalPnlSetVSyncEnd(u16VsyncSt + pstTimeGenCfg->u16VsyncWidth - 1);
    HalPnlSetVfdeSt(u16VdeSt);
    HalPnlSetVfdeEnd(u16VdeSt + pstTimeGenCfg->u16Vactive - 1);
    HalPnlSetVdeSt(u16VdeSt);
    HalPnlSetVdeEnd(u16VdeSt + pstTimeGenCfg->u16Vactive - 1);
    HalPnlSetVtt(pstTimeGenCfg->u16VTotal - 1);

    HalPnlSetHSyncSt(u16HsyncSt);
    HalPnlSetHSyncEnd(u16HsyncSt + pstTimeGenCfg->u16HsyncWidth - 1);
    HalPnlSetHfdeSt(u16HdeSt);
    HalPnlSetHfdeEnd(u16HdeSt + pstTimeGenCfg->u16Hactive- 1);
    HalPnlSetHdeSt(u16HdeSt);
    HalPnlSetHdeEnd(u16HdeSt + pstTimeGenCfg->u16Hactive  - 1);
    HalPnlSetHtt(pstTimeGenCfg->u16HTotal - 1);
}

void __HalPnlIfSetTestPatConfig(DrvPnlCtxConfig_t *pPnlCtx,  HalPnlTestPatternConfig_t *pstTestPatCfg)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: En:%d, (%x %x %x)\n",
        __FUNCTION__, __LINE__,
        pstTestPatCfg->bEn, pstTestPatCfg->u16R, pstTestPatCfg->u16G, pstTestPatCfg->u16B);

    HalPnlSetFrameColorEn(pstTestPatCfg->bEn);
    HalPnlSetFrameColor(pstTestPatCfg->u16R, pstTestPatCfg->u16G, pstTestPatCfg->u16B);
}

HalPnlQueryRet_e _HalPnlIfGetInfoInitConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    return enRet;
}

void _HalPnlIfSetInitConfig(void *pCtx, void *pCfg)
{
    HalPnlSetFrameColorEn(0);
}

HalPnlQueryRet_e _HalPnlIfGetInfoParamConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlParamConfig_t *pParamCfg = (HalPnlParamConfig_t *)pCfg;

    memcpy(&pPnlCtxCfg->pstHalCtx->stParamCfg, pParamCfg, sizeof(HalPnlParamConfig_t));

    if(pPnlCtxCfg->pstHalCtx->enLinkType != pParamCfg->eLinkType)
    {
        enRet = E_HAL_PNL_QUERY_RET_CFGERR;
        PNL_ERR("%s %d, LinkType is not Correct (%s != %s)\n",
            __FUNCTION__, __LINE__,
            PARSING_HAL_LINKTYPE(pPnlCtxCfg->pstHalCtx->enLinkType),
            PARSING_HAL_LINKTYPE(pParamCfg->eLinkType));
    }
    return enRet;
}

void _HalPnlIfSetParamConfig(void *pCtx, void *pCfg)
{
    HalPnlSetOpenLpllCLK(1); // CLK

    __HalPnlIfSetTimeGenConfig((DrvPnlCtxConfig_t *)pCtx, (HalPnlParamConfig_t *)pCfg);
    __HalPnlIfSetPolarityConfig((DrvPnlCtxConfig_t *)pCtx, (HalPnlParamConfig_t *)pCfg);
    __HalPnlIfSetLpllConfig((DrvPnlCtxConfig_t *)pCtx, (HalPnlParamConfig_t *)pCfg);

    HalPnlResetOdclk(1); // Reset odck to avoid vertical abnormal
    HalPnlResetOdclk(0);

    HalPnlSetChipTop(1); // pad mux
}

HalPnlQueryRet_e _HalPnlIfGetInfoMipiDsiParamConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetMipiDsiParamConfig(void *pCtx, void *pCfg)
{
}

HalPnlQueryRet_e _HalPnlIfGetInfoSscConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetSscConfig(void *pCtx, void *pCfg)
{

}

HalPnlQueryRet_e _HalPnlIfGetInfoTimingConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetTimingConfig(void *pCtx, void *pCfg)
{

}

HalPnlQueryRet_e _HalPnlIfGetInfoPowerConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetPowerConfig(void *pCtx, void *pCfg)
{

}

HalPnlQueryRet_e _HalPnlIfGetInfoBackLightOnOffConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetBackLightOnOffConfig(void *pCtx, void *pCfg)
{

}

HalPnlQueryRet_e _HalPnlIfGetInfoBackLightLevelConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetBackLightLevelConfig(void *pCtx, void *pCfg)
{
}


HalPnlQueryRet_e _HalPnlIfGetInfoCurrentConfig(void *pCtx, void *pCfg)
{
    return E_HAL_PNL_QUERY_RET_NOTSUPPORT;
}

void _HalPnlIfSetCurrentConfig(void *pCtx, void *pCfg)
{

}

HalPnlQueryRet_e _HalPnlIfGetInfoTestPatConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlTestPatternConfig_t *pstTestPatCfg = (HalPnlTestPatternConfig_t *)pCfg;

    memcpy(&pPnlCtxCfg->pstHalCtx->stTestPatCfg, pstTestPatCfg, sizeof(HalPnlTestPatternConfig_t));
    return enRet;
}

void _HalPnlIfSetTestPatConfig(void *pCtx, void *pCfg)
{
    __HalPnlIfSetTestPatConfig((DrvPnlCtxConfig_t *)pCtx, (HalPnlTestPatternConfig_t*)pCfg);
}

HalPnlQueryRet_e _HalPnlIfGetInfoClkSetConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    HalPnlClkConfig_t *pstClkCfg = (HalPnlClkConfig_t *)pCfg;

    if(pstClkCfg->u32Num != HAL_PNL_CLK_NUM)
    {
        enRet = E_HAL_PNL_QUERY_RET_CFGERR;
        PNL_ERR("%s %d, Clk Num is not correct %ld != %d\n",
            __FUNCTION__, __LINE__, pstClkCfg->u32Num, HAL_PNL_CLK_NUM);
    }

    return enRet;
}

void _HalPnlIfSetClkSetConfig(void *pCtx, void *pCfg)
{
    u32 u32ClkNum = 0;
    HalPnlClkConfig_t *pstClkCfg = (HalPnlClkConfig_t *)pCfg;

    for (u32ClkNum=0; u32ClkNum<pstClkCfg->u32Num; u32ClkNum++)
    {
        PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d, Lpll(%d %ld)\n",
                __FUNCTION__, __LINE__, pstClkCfg->bEn[u32ClkNum], pstClkCfg->u32Rate[u32ClkNum]);

        HalPnlSetOpenLpllCLK(pstClkCfg->bEn[u32ClkNum]);

        PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d, Lpll(%d %ld)\n",
                __FUNCTION__, __LINE__, pstClkCfg->bEn[u32ClkNum], pstClkCfg->u32Rate[u32ClkNum]);
    }
}

//------------------------------------------------------------------------------

bool _HalPnlIfGetCallBack(DrvPnlCtxConfig_t *pstPnlCfg, HalPnlQueryConfig_t *pstQueryCfg)
{
    memset(&pstQueryCfg->stOutCfg, 0, sizeof(HalPnlQueryOutConfig_t));

    if(pstQueryCfg->stInCfg.u32CfgSize != gpPnlCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize)
    {
        pstQueryCfg->stOutCfg.enQueryRet = E_HAL_PNL_QUERY_RET_CFGERR;
        PNL_ERR("%s %d, Size %ld != %d\n",
            __FUNCTION__, __LINE__,
            pstQueryCfg->stInCfg.u32CfgSize,
            gpPnlCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize);
    }
    else
    {
        pstQueryCfg->stOutCfg.pSetFunc = gpPnlCbTbl[pstQueryCfg->stInCfg.enQueryType].pSetFunc;

        if(pstQueryCfg->stOutCfg.pSetFunc == NULL)
        {
            pstQueryCfg->stOutCfg.enQueryRet = E_HAL_PNL_QUERY_RET_NOTSUPPORT;
        }
        else
        {
            pstQueryCfg->stOutCfg.enQueryRet =
                gpPnlCbTbl[pstQueryCfg->stInCfg.enQueryType].pGetInfoFunc(pstPnlCfg, pstQueryCfg->stInCfg.pInCfg);

        }
    }

    return 1;
}


HalPnlQueryRet_e _HalPnlIfGetInfoUnifiedParamConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlUnifiedParamConfig_t *pParamCfg = (HalPnlUnifiedParamConfig_t *)pCfg;

    memcpy(&pPnlCtxCfg->pstHalUnfdCtx->stHalPnlUdParamCfg, pParamCfg, sizeof(HalPnlUnifiedParamConfig_t));

    return enRet;
}

void _HalPnlIfSetUnifiedParamConfig(void *pCtx, void *pCfg)
{

    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlParamConfig_t stParamCfg = {0};
    HalPnlUnifiedParamConfig_t *pstUfdParamCfg = (HalPnlUnifiedParamConfig_t *)pCfg;
    u32 u32Mode = 0xFF;//PINMUX_FOR_UNKNOWN_MODE;

    HalPnlSetOpenLpllCLK(1); // CLK

    //set timing
    if(pstUfdParamCfg->u8TgnTimingFlag)
    {
        stParamCfg.eLinkType        = pstUfdParamCfg->eLinkType;
        stParamCfg.u16VSyncWidth    = pstUfdParamCfg->stTgnTimingInfo.u16VSyncWidth;
        stParamCfg.u16VSyncBackPorch= pstUfdParamCfg->stTgnTimingInfo.u16VSyncBackPorch;
        stParamCfg.u16Height        = pstUfdParamCfg->stTgnTimingInfo.u16VActive;
        stParamCfg.u16VStart        = pstUfdParamCfg->stTgnTimingInfo.u16VStart;
        stParamCfg.u16VTotal        = pstUfdParamCfg->stTgnTimingInfo.u16VTotal;

        stParamCfg.u16HSyncWidth    = pstUfdParamCfg->stTgnTimingInfo.u16HSyncWidth;
        stParamCfg.u16HSyncBackPorch= pstUfdParamCfg->stTgnTimingInfo.u16HSyncBackPorch;
        stParamCfg.u16Width         = pstUfdParamCfg->stTgnTimingInfo.u16HActive;
        stParamCfg.u16HStart        = pstUfdParamCfg->stTgnTimingInfo.u16HStart;
        stParamCfg.u16HTotal        = pstUfdParamCfg->stTgnTimingInfo.u16HTotal;
        PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: LinkType is %s\n",__FUNCTION__, __LINE__, PARSING_HAL_LINKTYPE(stParamCfg.eLinkType));
        __HalPnlIfSetTimeGenConfig(pPnlCtxCfg, &stParamCfg);
    }

    //set polarity
    if(pstUfdParamCfg->u8TgnPolarityFlag)
    {
        stParamCfg.u8InvDCLK        =    pstUfdParamCfg->stTgnPolarityInfo.u8InvDCLK;
        stParamCfg.u8InvDE          =    pstUfdParamCfg->stTgnPolarityInfo.u8InvDE;
        stParamCfg.u8InvHSync       =    pstUfdParamCfg->stTgnPolarityInfo.u8InvHSync;
        stParamCfg.u8InvVSync       =    pstUfdParamCfg->stTgnPolarityInfo.u8InvVSync;
        //need refine
        __HalPnlIfSetPolarityConfig(pPnlCtxCfg, &stParamCfg);
    }

    //set output bit mode
    if(pstUfdParamCfg->u8TgnPadMuxFlag)
    {
        if(DrvPnlOsPadMuxActive() == 0)
        {
            PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d, use config setting mode %lx\n",
                    __FUNCTION__, __LINE__,u32Mode);
            u32Mode = __HalPnlIfGetPadMuxMode(pstUfdParamCfg->u16PadMux,
                                              pstUfdParamCfg->eLinkType,
                                              pstUfdParamCfg->eOutputFormatBitMode);
        }
        else
        {
            u32Mode = DrvPnlOsPadMuxGetMode(); // It may be PINMUX_FOR_UNKNOWN_MODE
            PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d, get mode (%lx) from padmux.dtsi\n",
                    __FUNCTION__, __LINE__,u32Mode);
        }
        HalPnlSetChipTop(u32Mode); // pad mux
    }
    //set DCLK
    if(pstUfdParamCfg->u8TgnFixedDclkFlag || pstUfdParamCfg->u8TgnTimingFlag)
    {
        if(pstUfdParamCfg->u8TgnFixedDclkFlag)
            HalPnlUnifiedSetFixedClk(pstUfdParamCfg->u32FDclk);
        else
            __HalPnlIfUnifiedSetLpllConfig(pPnlCtxCfg,pstUfdParamCfg->stTgnTimingInfo.u32Dclk);
        HalPnlResetOdclk(1); // Reset odck to avoid vertical abnormal
        HalPnlResetOdclk(0);
    }
    //set output bit
    if(pstUfdParamCfg->u8TgnOutputBitMdFlag)
    {
        HalPnlIfSetOutFormatConfig(pstUfdParamCfg->eOutputFormatBitMode);
    }
    //set ssc
    if(pstUfdParamCfg->u8TgnSscFlag)
    {
        stParamCfg.u16SpreadSpectrumStep = pstUfdParamCfg->stTgnSscInfo.u16SpreadSpectrumStep;
        stParamCfg.u16SpreadSpectrumSpan = pstUfdParamCfg->stTgnSscInfo.u16SpreadSpectrumSpan;
        __HalPnlIfSetSSCConfig(&stParamCfg);
    }
    //set swap function
    if(pstUfdParamCfg->u8TgnRgbSwapFlag)
    {
        HalPnlUnifiedSetMLSwap(pstUfdParamCfg->stTgnRgbSwapInfo.u8SwapRgbML);
    }

    HalPnlResetOdclk(1); // Reset odck to avoid vertical abnormal
    HalPnlResetOdclk(0);

    HalPnlSetChipTop(1); // pad mux
}
//

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool HalPnlIfInit(void)
{
    if(gbHwIfInit)
    {
        return 1;
    }

    memset(gpPnlCbTbl, 0, sizeof(HalPnlQueryCallBackFunc_t)* E_HAL_PNL_QUERY_MAX);
    gpPnlCbTbl[E_HAL_PNL_QUERY_PARAM].pSetFunc     = _HalPnlIfSetParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_PARAM].pGetInfoFunc = _HalPnlIfGetInfoParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_PARAM].u16CfgSize   = sizeof(HalPnlParamConfig_t);


    gpPnlCbTbl[E_HAL_PNL_QUERY_MIPIDSI_PARAM].pSetFunc     = _HalPnlIfSetMipiDsiParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_MIPIDSI_PARAM].pGetInfoFunc = _HalPnlIfGetInfoMipiDsiParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_MIPIDSI_PARAM].u16CfgSize   = sizeof(HalPnlMipiDsiConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_SSC].pSetFunc     = _HalPnlIfSetSscConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_SSC].pGetInfoFunc = _HalPnlIfGetInfoSscConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_SSC].u16CfgSize   =  sizeof(HalPnlSscConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_TIMING].pSetFunc     = _HalPnlIfSetTimingConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_TIMING].pGetInfoFunc = _HalPnlIfGetInfoTimingConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_TIMING].u16CfgSize   =  sizeof(HalPnlTimingConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_POWER].pSetFunc     = _HalPnlIfSetPowerConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_POWER].pGetInfoFunc = _HalPnlIfGetInfoPowerConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_POWER].u16CfgSize   =  sizeof(HalPnlPowerConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_ONOFF].pSetFunc     = _HalPnlIfSetBackLightOnOffConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_ONOFF].pGetInfoFunc = _HalPnlIfGetInfoBackLightOnOffConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_ONOFF].u16CfgSize   =  sizeof(HalPnlBackLightOnOffConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_LEVEL].pSetFunc     = _HalPnlIfSetBackLightLevelConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_LEVEL].pGetInfoFunc = _HalPnlIfGetInfoBackLightLevelConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_BACKLIGHT_LEVEL].u16CfgSize   =  sizeof(HalPnlBackLightLevelConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_CURRENT].pSetFunc     = _HalPnlIfSetCurrentConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_CURRENT].pGetInfoFunc = _HalPnlIfGetInfoCurrentConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_CURRENT].u16CfgSize   =  sizeof(HalPnlCurrentConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_TESTPAT].pSetFunc     = _HalPnlIfSetTestPatConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_TESTPAT].pGetInfoFunc = _HalPnlIfGetInfoTestPatConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_TESTPAT].u16CfgSize   =  sizeof(HalPnlTestPatternConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_SET].pSetFunc     = _HalPnlIfSetClkSetConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_SET].pGetInfoFunc = _HalPnlIfGetInfoClkSetConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_SET].u16CfgSize   =  sizeof(HalPnlClkConfig_t);

//    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_GET].pSetFunc     = _HalPnlIfSetClkGetConfig;
//    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_GET].pGetInfoFunc = _HalPnlIfGetInfoClkGetConfig;
//    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_GET].u16CfgSize   =  sizeof(HalPnlClkConfig_t);

    gpPnlCbTbl[E_HAL_PNL_QUERY_INIT].pSetFunc     = _HalPnlIfSetInitConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_INIT].pGetInfoFunc = _HalPnlIfGetInfoInitConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_INIT].u16CfgSize   = 0;

    gpPnlCbTbl[E_HAL_PNL_QUERY_UNIFIED_PARAM].pSetFunc     = _HalPnlIfSetUnifiedParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_UNIFIED_PARAM].pGetInfoFunc = _HalPnlIfGetInfoUnifiedParamConfig;
    gpPnlCbTbl[E_HAL_PNL_QUERY_UNIFIED_PARAM].u16CfgSize   = sizeof(HalPnlUnifiedParamConfig_t);
    gbHwIfInit = 1;
    return 1;
}

bool HalPnlIfDeInit(void)
{
    if(gbHwIfInit == 0)
    {
        PNL_ERR("%s %d, HalIf not init\n", __FUNCTION__, __LINE__);
        return 0;
    }
    gbHwIfInit = 0;
    memset(gpPnlCbTbl, 0, sizeof(HalPnlQueryCallBackFunc_t)* E_HAL_PNL_QUERY_MAX);

    return 1;
}

bool HalPnlIfQuery(void *pCtx,  void *pCfg)
{
    bool bRet = 1;

    if(pCtx == NULL || pCfg == NULL)
    {
        PNL_ERR("%s %d, Input Param is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = _HalPnlIfGetCallBack((DrvPnlCtxConfig_t *)pCtx, (HalPnlQueryConfig_t *)pCfg);
    }

    return bRet;
}



