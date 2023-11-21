/*
* hal_pnl_if.c- Sigmastar
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

#define _HAL_PNL_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_pnl_os.h"
#include "hal_pnl_common.h"
#include "pnl_debug.h"
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl.h"
#include "drv_pnl_ctx.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define SC_PIX_CLK_240M     (240000000)
#define SC_PIX_CLK_216M     (216000000)
#define SC_PIX_CLK_192M     (192000000)
#define SC_PIX_CLK_172M     (172000000)
#define SC_PIX_CLK_144M     (144000000)
#define SC_PIX_CLK_123M     (123000000)
#define SC_PIX_CLK_108M     (108000000)
#define SC_PIX_CLK_86M      (86000000)
#define SC_PIX_CLK_72M      (72000000)
#define SC_PIX_CLK_54M      (54000000)


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
void __HalPnlIfSetPolarityConfig(DrvPnlCtxConfig_t *pPnlCtx, HalPnlParamConfig_t *pParamCfg)
{
    HalPnlHwPolarityConfig_t *pstPolarityCfg;

    pstPolarityCfg = &pPnlCtx->pstHwCtx->stPolarityConfig;

    pstPolarityCfg->bClkInv = pParamCfg->u8InvDCLK;
    pstPolarityCfg->bDeInv  = pParamCfg->u8InvDE;
    pstPolarityCfg->bHsyncInv = pParamCfg->u8InvHSync;
    pstPolarityCfg->bVsyncInv = pParamCfg->u8InvVSync;

    pstPolarityCfg->enChR = pParamCfg->u8SwapOdd_RG;
    pstPolarityCfg->enChG = pParamCfg->u8SwapEven_RG;
    pstPolarityCfg->enChB = pParamCfg->u8SwapOdd_GB;
    pstPolarityCfg->bRgbMlSwap = pParamCfg->u8SwapEven_GB ? 1 : 0;

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: ClkInv:%d, DeInv:%d, HsyncInv:%d, VsyncInv:%d\n",
        __FUNCTION__, __LINE__,
        pstPolarityCfg->bClkInv, pstPolarityCfg->bDeInv,
        pstPolarityCfg->bHsyncInv, pstPolarityCfg->bVsyncInv
        );

    HalPnlSetClkInv(pstPolarityCfg->bClkInv);
    HalPnlSetDeInv(pstPolarityCfg->bDeInv);
    HalPnlSetVsyncInv(pstPolarityCfg->bVsyncInv);
    HalPnlSetHsyncInv(pstPolarityCfg->bHsyncInv);
    HalPnlSetVsynRefMd(1);


}

void __HalPnlIfSetLpllConfig(DrvPnlCtxConfig_t *pPnlCtx, bool bLpMode)
{
    HalPnlHwLpllConfig_t *pstHwLpllCfg;
    HalPnlParamConfig_t *pstParamCfg;
    u16 u16LpllIdx = 0;
    u32 u32Dividen, u32Divisor, u32LplLSet;
    u32 u32DclkHz, u32HttVtt, u32Mod;

    u16 u16Htotal, u16Vtotal;
    bool bParamSet = 0;

    pstHwLpllCfg = &pPnlCtx->pstHwCtx->stLplLConfig;
    pstParamCfg = &pPnlCtx->pstHalCtx->stParamCfg;

    if(pPnlCtx->pstHalCtx->enFlag & E_HAL_PNL_CTX_FLAG_PARAM)
    {
        //u16BitPerPixel = 1;
        u16Htotal = pstParamCfg->u16HTotal;
        u16Vtotal = pstParamCfg->u16VTotal;
        u32DclkHz = (u32)pstParamCfg->u16DCLK* 1000000;
        PNL_ERR("%s %d, DCLK:%ld\n", __FUNCTION__, __LINE__, u32DclkHz);

        u32HttVtt = (u32)(u16Vtotal * u16Htotal);
        u32Mod = u32DclkHz % u32HttVtt;

        if(u32Mod > (u32HttVtt/2))
        {
            pstHwLpllCfg->u32Fps = (u32DclkHz + u32HttVtt -1) / u32HttVtt;
        }
        else
        {
            pstHwLpllCfg->u32Fps  = u32DclkHz/u32HttVtt;
        }
        bParamSet = 1;
    }
    else
    {

        bParamSet = 0;
        PNL_ERR("%s %d, ParamConfig or MipiDisConfig not Init\n", __FUNCTION__, __LINE__);
    }

    if(bParamSet)
    {
        bool bDsi = (pPnlCtx->pstHalCtx->enLinkType == E_HAL_PNL_LINK_MIPI_DSI) ? 1 : 0;

        pstHwLpllCfg->u32Dclk = u32DclkHz ;

        PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: DclKhz:%ld\n",
            __FUNCTION__, __LINE__, u32DclkHz);

        if(HalPnlGetLpllIdx(pstHwLpllCfg->u32Dclk, &u16LpllIdx, bDsi))
        {
            pstHwLpllCfg->u16LoopDiv = HalPnlGetLpllDiv(u16LpllIdx, bDsi);
            pstHwLpllCfg->u16LoopGain = HalPnlGetLpllGain(u16LpllIdx, bDsi);

            u32Dividen = ((u32)432 * (u32)524288 * (u32)pstHwLpllCfg->u16LoopGain) / (u32)pstHwLpllCfg->u16LoopDiv;
            u32Divisor = pstHwLpllCfg->u32Dclk / 1000000;
            u32LplLSet= u32Dividen / u32Divisor;

            PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: Idx:%d, LoopGain:%d, LoopDiv:%d, fps=%ld, dclk=%ld, Divden:0x%lx, Divisor:0x%lx, LpllSe:0x%lx\n",
                __FUNCTION__, __LINE__, u16LpllIdx,
                pstHwLpllCfg->u16LoopGain, pstHwLpllCfg->u16LoopDiv, pstHwLpllCfg->u32Fps, pstHwLpllCfg->u32Dclk,
                u32Dividen, u32Divisor, u32LplLSet);          
            HalPnlDumpLpllSetting(u16LpllIdx);
            HalPnlSetLpllSet(u32LplLSet);
        }
        else
        {
            PNL_ERR("%s %d, DCLK Out of Range:%ld\n", __FUNCTION__, __LINE__, pstHwLpllCfg->u32Dclk);
        }
    }
}

void __HalPnlIfSetMipiDisTimeGenCOnfig(DrvPnlCtxConfig_t *pPnlCtx, HalPnlTimingConfig_t *pstTimingCfg)
{


}

void _HalPnlIfSetClkSetConfig(void *pCtx, void *pCfg)
{
    
}

HalPnlQueryRet_e _HalPnlIfGetInfoClkSetConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    return enRet;
}



void __HalPnlIfSetTimeGenConfig(DrvPnlCtxConfig_t *pPnlCtx, HalPnlTimingConfig_t *pstTimingCfg)
{
    HalPnlHwTimeGenConfig_t *pstHwTimeGenCfg;
    u16 u16VsyncSt, u16HsyncSt;
    u16 u16VdeSt, u16HdeSt;

    pstHwTimeGenCfg = &pPnlCtx->pstHwCtx->stTimeGenCfg;
    pstHwTimeGenCfg->u16VsyncWidth = pstTimingCfg->u16VSyncWidth;
    pstHwTimeGenCfg->u16VsyncBackPorch = pstTimingCfg->u16VSyncBackPorch;
    pstHwTimeGenCfg->u16Vactive  = pstTimingCfg->u16VActive;
    pstHwTimeGenCfg->u16Vstart = pstTimingCfg->u16VStart;
    pstHwTimeGenCfg->u16VTotal = pstTimingCfg->u16VTotal;

    pstHwTimeGenCfg->u16HsyncWidth = pstTimingCfg->u16HSyncWidth;
    pstHwTimeGenCfg->u16HsyncBackPorch = pstTimingCfg->u16HSyncBackPorch;
    pstHwTimeGenCfg->u16Hactive  = pstTimingCfg->u16HActive;
    pstHwTimeGenCfg->u16Hstart = pstTimingCfg->u16HStart;
    pstHwTimeGenCfg->u16HTotal = pstTimingCfg->u16HTotal;


    u16VdeSt = pstHwTimeGenCfg->u16Vstart;
    u16HdeSt = pstHwTimeGenCfg->u16Hstart;

    if(u16VdeSt)
    {
        u16VsyncSt = u16VdeSt - pstHwTimeGenCfg->u16VsyncWidth -  pstHwTimeGenCfg->u16VsyncBackPorch;
    }
    else
    {
        u16VsyncSt = pstHwTimeGenCfg->u16VTotal - pstHwTimeGenCfg->u16VsyncWidth -  pstHwTimeGenCfg->u16VsyncBackPorch;
    }

    if(u16HdeSt)
    {
        u16HsyncSt = u16HdeSt - pstHwTimeGenCfg->u16HsyncWidth -  pstHwTimeGenCfg->u16HsyncBackPorch;
    }
    else
    {
        u16HsyncSt = pstHwTimeGenCfg->u16HTotal - pstHwTimeGenCfg->u16HsyncWidth -  pstHwTimeGenCfg->u16HsyncBackPorch;
    }

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: LinkType:%s, Hsync(%d %d), Vsync(%d %d) SyncStart(%d %d), DeStart(%d, %d) Size(%d %d), Total(%d %d)\n",
        __FUNCTION__, __LINE__,
        PARSING_HAL_LINKTYPE(pPnlCtx->pstHalCtx->stParamCfg.eLinkType),
        pstHwTimeGenCfg->u16HsyncWidth, pstHwTimeGenCfg->u16HsyncBackPorch,
        pstHwTimeGenCfg->u16VsyncWidth, pstHwTimeGenCfg->u16VsyncBackPorch,
        u16HsyncSt, u16VsyncSt, u16HdeSt, u16VdeSt,
        pstHwTimeGenCfg->u16Hactive, pstHwTimeGenCfg->u16Vactive,
        pstHwTimeGenCfg->u16HTotal, pstHwTimeGenCfg->u16VTotal);

    HalPnlSetVtt(pstHwTimeGenCfg->u16VTotal - 1);
    mdelay(33);
    HalPnlSetVSyncSt(u16VsyncSt);
    HalPnlSetVSyncEnd(u16VsyncSt + pstHwTimeGenCfg->u16VsyncWidth - 1);

    HalPnlSetVfdeSt(u16VdeSt);
    HalPnlSetVfdeEnd(u16VdeSt + pstHwTimeGenCfg->u16Vactive - 1);
    HalPnlSetVdeSt(u16VdeSt);
    HalPnlSetVdeEnd(u16VdeSt + pstHwTimeGenCfg->u16Vactive - 1);

    HalPnlSetHSyncSt(u16HsyncSt);
    HalPnlSetHSyncEnd(u16HsyncSt + pstHwTimeGenCfg->u16HsyncWidth - 1);

    HalPnlSetHfdeSt(u16HdeSt);
    HalPnlSetHfdeEnd(u16HdeSt + pstHwTimeGenCfg->u16Hactive- 1);
    HalPnlSetHdeSt(u16HdeSt);
    HalPnlSetHdeEnd(u16HdeSt + pstHwTimeGenCfg->u16Hactive  - 1);


    HalPnlSetHtt(pstHwTimeGenCfg->u16HTotal - 1);
    //HalPnlSetDacHsyncSt(u16HsyncSt);
    //HalPnlSetDacHsyncEnd(u16HsyncSt + pstHwTimeGenCfg->u16HsyncWidth - 1);
    //HalPnlSetDacHdeSt(u16HdeSt);
    //HalPnlSetDacHdeEnd(u16HdeSt + pstHwTimeGenCfg->u16Hactive- 1);
    //HalPnlSetDacVdeSt(u16VdeSt);
    //HalPnlSetDacVdeEnd(u16VdeSt + pstHwTimeGenCfg->u16Vactive - 1);

    HalPnlSetSwReste(1);
    HalPnlSetSwReste(0);
}

void __HalPnlIfSetTestPatConfig(DrvPnlCtxConfig_t *pPnlCtx,  HalPnlTestPatternConfig_t *pstTestPatCfg)
{

    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d:: En:%d, (%x %x %x)\n",
        __FUNCTION__, __LINE__,
        pstTestPatCfg->bEn, pstTestPatCfg->u16R, pstTestPatCfg->u16G, pstTestPatCfg->u16B);

    HalPnlSetFrameColorEn(pstTestPatCfg->bEn);
    HalPnlSetFrameColor(pstTestPatCfg->u16R, pstTestPatCfg->u16G, pstTestPatCfg->u16B);
}

bool __HalPnlIfSetMipiDsiPacket(u8 *pu8PacketData, u32 u32PacketLen)
{

    return 1;
}


HalPnlQueryRet_e _HalPnlIfGetInfoParamConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlParamConfig_t *pParamCfg = (HalPnlParamConfig_t *)pCfg;

    pPnlCtxCfg->pstHalCtx->enFlag |= E_HAL_PNL_CTX_FLAG_PARAM;
    memcpy(&pPnlCtxCfg->pstHalCtx->stParamCfg, pParamCfg, sizeof(HalPnlParamConfig_t));

    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HSyncWidth = pParamCfg->u16HSyncWidth;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HSyncBackPorch = pParamCfg->u16HSyncBackPorch;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HStart = pParamCfg->u16HStart;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HTotal = pParamCfg->u16HTotal;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HActive = pParamCfg->u16Width;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16HSyncFrontPorch =
        pParamCfg->u16HTotal - pParamCfg->u16HSyncWidth - pParamCfg->u16HSyncBackPorch - pParamCfg->u16Width;

    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VSyncWidth = pParamCfg->u16VSyncWidth;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VSyncBackPorch = pParamCfg->u16VSyncBackPorch;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VStart = pParamCfg->u16VStart;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VTotal = pParamCfg->u16VTotal;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VActive = pParamCfg->u16Height;
    pPnlCtxCfg->pstHalCtx->stTimingCfg.u16VSyncFrontPorch =
        pParamCfg->u16VTotal - pParamCfg->u16VSyncWidth - pParamCfg->u16VSyncBackPorch - pParamCfg->u16Height;

    if(pPnlCtxCfg->pstHalCtx->enFlag & E_HAL_PNL_CTX_FLAG_MIPIDSI)
    {
        pPnlCtxCfg->pstHalCtx->stMipiDisCfg.enCh[0] = pPnlCtxCfg->pstHalCtx->stParamCfg.eCh0;
        pPnlCtxCfg->pstHalCtx->stMipiDisCfg.enCh[1] = pPnlCtxCfg->pstHalCtx->stParamCfg.eCh1;
        pPnlCtxCfg->pstHalCtx->stMipiDisCfg.enCh[2] = pPnlCtxCfg->pstHalCtx->stParamCfg.eCh2;
        pPnlCtxCfg->pstHalCtx->stMipiDisCfg.enCh[3] = pPnlCtxCfg->pstHalCtx->stParamCfg.eCh3;
        pPnlCtxCfg->pstHalCtx->stMipiDisCfg.enCh[4] = pPnlCtxCfg->pstHalCtx->stParamCfg.eCh4;
    }

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
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlParamConfig_t *pstParamCfg = (HalPnlParamConfig_t *)pCfg;

    HalPnlSetClk(1, pPnlCtxCfg->pstHalCtx->enLinkType == E_HAL_PNL_LINK_MIPI_DSI ? 1 : 0); // CLK
    __HalPnlIfSetTimeGenConfig((DrvPnlCtxConfig_t *)pCtx, &pPnlCtxCfg->pstHalCtx->stTimingCfg);
    __HalPnlIfSetPolarityConfig((DrvPnlCtxConfig_t *)pCtx, pstParamCfg);
    memcpy(&pPnlCtxCfg->pstHalCtx->stParamCfg, pstParamCfg, sizeof(HalPnlParamConfig_t));

    __HalPnlIfSetLpllConfig((DrvPnlCtxConfig_t *)pCtx, 0);

    HalPnlSetRgbMode(pstParamCfg->eOutputFormatBitMode); // 565 or 888

    if(DrvPnlOsPadMuxActive() == 0 && (pstParamCfg->eLinkType == E_HAL_PNL_LINK_TTL ||  pstParamCfg->eLinkType == E_HAL_PNL_LINK_TTL_SPI_IF )  )
    {
        HalPnlSetTTLPadMux(pstParamCfg->eOutputFormatBitMode);
    }
}

HalPnlQueryRet_e _HalPnlIfGetInfoMipiDsiParamConfig(void *pCtx, void *pCfg)
{
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;

    return enRet;
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
    HalPnlQueryRet_e enRet = E_HAL_PNL_QUERY_RET_OK;
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlTimingConfig_t *pTimingCfg = (HalPnlTimingConfig_t *)pCfg;

    pPnlCtxCfg->pstHalCtx->enFlag |= E_HAL_PNL_CTX_FLAG_TIMING;
    memcpy(&pPnlCtxCfg->pstHalCtx->stTimingCfg, pTimingCfg, sizeof(HalPnlTimingConfig_t));

    return enRet;
}

void _HalPnlIfSetTimingConfig(void *pCtx, void *pCfg)
{
    DrvPnlCtxConfig_t  *pPnlCtxCfg = (DrvPnlCtxConfig_t *)pCtx;
    HalPnlTimingConfig_t *pstTimingCfg = (HalPnlTimingConfig_t *)pCfg;

    __HalPnlIfSetTimeGenConfig(pPnlCtxCfg, pstTimingCfg);
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
    gpPnlCbTbl[E_HAL_PNL_QUERY_CLK_SET].u16CfgSize   = sizeof(HalPnlClkConfig_t);

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



