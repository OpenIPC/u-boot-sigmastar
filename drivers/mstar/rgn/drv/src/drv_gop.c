/*
* drv_gop.c- Sigmastar
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

#define __DRV_GOP_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_gop.h"
#include "hal_gop.h"
/*#ifdef CONFIG_CAM_CLK
#include "camclk.h"
#include "drv_camclk_Api.h"

#ifndef CAMCLK_gop0
#define CAMCLK_gop0 0
#endif
#ifndef CAMCLK_fclk1
#define CAMCLK_fclk1 0
#endif
#ifndef CAMCLK_dip
#define CAMCLK_dip 0
#endif
#ifndef CAMCLK_ldcfeye
#define CAMCLK_ldcfeye 0
#endif
#define MAX_GOPPARENTCNT 4
void *pvgophandler[MAX_GOPPARENTCNT] = {0,0,0,0};

u32 gGopCID[MAX_GOPPARENTCNT] = {CAMCLK_gop0,CAMCLK_fclk1,CAMCLK_dip,CAMCLK_ldcfeye};
#endif*/
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define IsDrvGOPStretchRatioType_1(x)          (x == E_DRV_GOP_STRETCH_RATIO_1)

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

_DrvGopLocalSettingsConfig_t _tGopLocSettings[HAL_RGN_GOP_NUM];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static bool _DrvGopExecQueryFunc(HalGopQueryFuncConfig_t *pstQ)
{
    HalGopQuery((void *)pstQ);

    if (pstQ->stOutQ.enQueryRet == E_HAL_QUERY_GOP_OK)
    {
        if (pstQ->stOutQ.pfForSet)
        {
            pstQ->stOutQ.pfForSet(pstQ->stInQ.eGopId ,pstQ->stInQ.pCfg);
        }
        if (pstQ->stOutQ.pfForPatch)
        {
            pstQ->stOutQ.pfForPatch(pstQ->stInQ.eGopId ,pstQ->stOutQ.pPatch);
        }
    }

    return pstQ->stOutQ.enQueryRet;
}

bool _DrvGopTransId(DrvGopIdType_e eGopId, HalGopIdType_e *pHalId)
{
    bool bRet = TRUE;
    *pHalId = HAL_GOP_TRANS_ID(eGopId);
    if(!HalGopCheckIdSupport(*pHalId))
    {
        bRet = 0;
    }
    return bRet;
}

bool _DrvGopTransGwinId(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, HalGopIdType_e *pHalId)
{
    bool bRet = TRUE;
    *pHalId = HAL_GOP_GET_GWIN_ID(eGopId,eGwinId);
    if(!HalGopCheckIdSupport(*pHalId))
    {
        bRet = 0;
    }
    else if(eGwinId >= HAL_RGN_GOP_GWIN_NUM)
    {
        bRet = 0;
    }
    return bRet;
}

HalGopGwinSrcFormat_e _DrvGopTranSrcFmt(DrvGopGwinSrcFmtType_e enType)
{
    HalGopGwinSrcFormat_e enHalType = E_HAL_GOP_GWIN_SRC_NOTSUPPORT;
    switch(enType)
    {
        case E_DRV_GOP_GWIN_SRC_FMT_RGB1555:
            enHalType = E_HAL_GOP_GWIN_SRC_RGB1555;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_RGB565:
            enHalType = E_HAL_GOP_GWIN_SRC_RGB565;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_ARGB4444:
            enHalType = E_HAL_GOP_GWIN_SRC_ARGB4444;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_2266:
            enHalType = E_HAL_GOP_GWIN_SRC_2266;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_I8_PALETTE:
            enHalType = E_HAL_GOP_GWIN_SRC_I8_PALETTE;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_ARGB8888:
            enHalType = E_HAL_GOP_GWIN_SRC_ARGB8888;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_ARGB1555:
            enHalType = E_HAL_GOP_GWIN_SRC_ARGB1555;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_ABGR8888:
            enHalType = E_HAL_GOP_GWIN_SRC_ABGR8888;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_UV7Y8:
            enHalType = E_HAL_GOP_GWIN_SRC_UV7Y8;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_UV8Y8:
            enHalType = E_HAL_GOP_GWIN_SRC_UV8Y8;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_RGBA5551:
            enHalType = E_HAL_GOP_GWIN_SRC_RGBA5551;
        break;
        case E_DRV_GOP_GWIN_SRC_FMT_RGBA4444:
            enHalType = E_HAL_GOP_GWIN_SRC_RGBA4444;
        break;
        case E_DRV_GOP_GWIN_SRC_I4_PALETTE:
            enHalType = E_HAL_GOP_GWIN_SRC_I4_PALETTE;
        break;
        case E_DRV_GOP_GWIN_SRC_I2_PALETTE:
            enHalType = E_HAL_GOP_GWIN_SRC_I2_PALETTE;
        break;
        default:
            DRVRGNERR("[%s]Wrong enType=%d",__FUNCTION__,enType);
            CamOsPanic("");
    }
    if(enHalType==E_HAL_GOP_GWIN_SRC_NOTSUPPORT)
    {
        DRVRGNERR("%s %d: SRC NOTSUPPORT\n", __FUNCTION__, __LINE__);
    }
    return enHalType;
}

bool _DrvGopGwinInit(u8 u8GopIdx)
{
    u8 u8GwinIdx;
    HalGopIdType_e eHalGopId;
    HalGopGwinParamConfig_t tGwinParamCfg;
    for(u8GwinIdx = E_DRV_GOP_GWIN_ID_0; u8GwinIdx < HAL_RGN_GOP_GWIN_NUM; u8GwinIdx++)
    {
        if(_DrvGopTransGwinId((DrvGopIdType_e)u8GopIdx, (DrvGopGwinIdType_e)u8GwinIdx, &eHalGopId))
        {
            HalGopSetAlphaPointVal(E_HAL_GOP_GWIN_ALPHA_DEFINE_CONST,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinAblSet.tAlphaVal,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8ConstantAlphaVal);
            HalGopSetAlphaPointVal(E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA0,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinAblSet.tAlphaVal,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha0Val);
            HalGopSetAlphaPointVal(E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA1,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinAblSet.tAlphaVal,
                &_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha1Val);
            _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].bGwinEn = FALSE;
            HalGopSetEnableGwin(eHalGopId, (HalGopGwinIdType_e)u8GwinIdx, _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].bGwinEn);
            _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.eGwinId = (HalGopGwinIdType_e)u8GwinIdx;
            _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.eSrcFmt = E_DRV_GOP_GWIN_SRC_FMT_I8_PALETTE;
            tGwinParamCfg.eSrcFmt = _DrvGopTranSrcFmt(_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.eSrcFmt);
            tGwinParamCfg.u32BaseAddr = _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.u32BaseAddr = 0;
            tGwinParamCfg.u16Base_XOffset = _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.u16Base_XOffset = 0;
            tGwinParamCfg.u32Base_YOffset = _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.u32Base_YOffset = 0;
            tGwinParamCfg.tDispWindow.u16X = _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.tDisplayWin.u16X = 0;
            tGwinParamCfg.tDispWindow.u16Y = _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.tDisplayWin.u16Y = 0;
            tGwinParamCfg.tDispWindow.u16Width= _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.tDisplayWin.u16Width = 0;
            tGwinParamCfg.tDispWindow.u16Height= _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinGenSet.tDisplayWin.u16Height = 0;
            HalGopUpdateGwinParam(eHalGopId, (HalGopGwinIdType_e)u8GwinIdx, &tGwinParamCfg);
            *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha0Val=0x0;
            HalGopSetArgb1555Alpha(eHalGopId, (HalGopGwinIdType_e)u8GwinIdx, E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA0,
                *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha0Val);
            *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha1Val=0xFF;
            HalGopSetArgb1555Alpha(eHalGopId, (HalGopGwinIdType_e)u8GwinIdx, E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA1,
                *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8Argb1555Alpha1Val);
            _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].u32MemPitch = 0;
            _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinAblSet.eAlphaType = E_DRV_GOP_GWIN_ALPHA_CONSTANT;
            *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8ConstantAlphaVal = 0xFF;
            HalGopSetAlphaBlending(eHalGopId, (HalGopGwinIdType_e)u8GwinIdx,
                _tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].tGwinAblSet.eAlphaType == E_DRV_GOP_GWIN_ALPHA_CONSTANT ? TRUE : FALSE,
                *_tGopLocSettings[u8GopIdx].tGwinLocSet[u8GwinIdx].p8ConstantAlphaVal);
        }
    }
    return 1;
}
bool _DrvGopInit(u8 u8GopIdx,HalGopIdType_e eHalGopId)
{
    HalGopParamConfig_t tParamCfg;
    //Gop settings

    tParamCfg.eDisplayMode = _tGopLocSettings[u8GopIdx].eGopDispModeSet = E_DRV_GOP_DISPLAY_MD_PROGRESS;
    tParamCfg.tStretchWindow.u16X = _tGopLocSettings[u8GopIdx].tGopSrcStrWinSet.u16X = 0;
    tParamCfg.tStretchWindow.u16Y = _tGopLocSettings[u8GopIdx].tGopSrcStrWinSet.u16Y = 0;
    tParamCfg.tStretchWindow.u16Width= _tGopLocSettings[u8GopIdx].tGopSrcStrWinSet.u16Width= 0;
    tParamCfg.tStretchWindow.u16Height= _tGopLocSettings[u8GopIdx].tGopSrcStrWinSet.u16Height= 0;
    _tGopLocSettings[u8GopIdx].tGopDstStrWinSet.u16X = 0;
    _tGopLocSettings[u8GopIdx].tGopDstStrWinSet.u16Y = 0;
    _tGopLocSettings[u8GopIdx].tGopDstStrWinSet.u16Width= 0;
    _tGopLocSettings[u8GopIdx].tGopDstStrWinSet.u16Height= 0;
    HalGopUpdateParam(eHalGopId, &tParamCfg);
    _tGopLocSettings[u8GopIdx].eGopOutFmtSet = tParamCfg.eOutFormat;
    _tGopLocSettings[u8GopIdx].bMirror = FALSE;
    _tGopLocSettings[u8GopIdx].bFlip = FALSE;

    _tGopLocSettings[u8GopIdx].tGopColorKeySet.bEn = FALSE;
    _tGopLocSettings[u8GopIdx].tGopColorKeySet.u8R = 0;
    _tGopLocSettings[u8GopIdx].tGopColorKeySet.u8G = 0;
    _tGopLocSettings[u8GopIdx].tGopColorKeySet.u8B = 0;
    HalGopSetColorKey(eHalGopId, (HalGopColorKeyConfig_t *)&_tGopLocSettings[u8GopIdx].tGopColorKeySet,FALSE);
    HalGopSetColorKey(eHalGopId, (HalGopColorKeyConfig_t *)&_tGopLocSettings[u8GopIdx].tGopColorKeySet,TRUE);

    //Gwin settings
    _tGopLocSettings[u8GopIdx].u8GWinNum = HAL_RGN_GOP_GWIN_NUM;
    _DrvGopGwinInit(u8GopIdx);
    return 1;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 DrvGopClkDisable(DrvGopIdType_e eGopId,bool bEn)
{
#ifdef CONFIG_CAM_CLK
    u32 u32clknum;
    HalGopIdType_e enType;

    if(!_DrvGopTransId(eGopId,&enType))
    {
        return 0;
    }
    if(!bEn)
    {
        return 0;
    }
    HalGopGetClkNum(enType,&u32clknum);
    if(u32clknum<MAX_GOPPARENTCNT)
    {
        if(pvgophandler[u32clknum])
        {
            CamClkSetOnOff(pvgophandler[u32clknum],0);
            DRVRGNDBG( "[%s] %p\n", __func__,pvgophandler[u32clknum]);
        }
    }

#endif
    return 1;
}
u8 DrvGopClkEnable(DrvGopIdType_e eGopId)
{
#ifdef CONFIG_CAM_CLK
    u32 u32clknum;
    HalGopIdType_e enType;

    if(!_DrvGopTransId(eGopId,&enType))
    {
        return 0;
    }
    HalGopGetClkNum(enType,&u32clknum);
    if(u32clknum<MAX_GOPPARENTCNT)
    {
        if(pvgophandler[u32clknum])
        {
            CamClkSetOnOff(pvgophandler[u32clknum],1);
            DRVRGNDBG( "[%s] %p\n", __func__,pvgophandler[u32clknum]);
        }
    }

#endif
    return 1;
}
u8 DrvGopClkRegister(void)
{
#ifdef CONFIG_CAM_CLK
    u32 u32clknum;
    u32 gopip_clks = 0;

    for(u32clknum=0;u32clknum<MAX_GOPPARENTCNT;u32clknum++)
    {
        gopip_clks = gGopCID[u32clknum];
        DRVRGNDBG( "[%s] %d\n", __func__,gopip_clks);
        if(pvgophandler[u32clknum])
        {
            continue;
        }
        if (gopip_clks == 0)
        {
            DRVRGNDBG( "[%s] Fail to get clk:%d!\n", __func__,u32clknum);
            pvgophandler[u32clknum] = NULL;
            continue;
        }
        else
        {
            CamClkRegister((u8*)"gop",gopip_clks,&pvgophandler[u32clknum]);
            DRVRGNDBG( "[%s] %p\n", __func__,pvgophandler[u32clknum]);
        }
        DRVRGNDBG( "[RGN] CID:%d\n"
            ,gopip_clks
            );
    }
#endif
    return 1;
}
u8 DrvGopClkUnregister(void)
{
#ifdef CONFIG_CAM_CLK
    u32 u32clknum;

    for(u32clknum=0;u32clknum<MAX_GOPPARENTCNT;u32clknum++)
    {
        if(pvgophandler[u32clknum])
        {
            DRVRGNDBG( "[%s] %p\n", __func__,pvgophandler[u32clknum]);
            CamClkUnregister(pvgophandler[u32clknum]);
            pvgophandler[u32clknum] = NULL;
        }
    }
#endif
    return 1;
}

bool DrvGopRgnInit(void)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    u16 u16GopCnt;
    HalGopInit();
    for(u16GopCnt=E_DRV_GOP_ID_0;u16GopCnt<HAL_RGN_GOP_NUM;u16GopCnt++)
    {
        if(_DrvGopTransId(u16GopCnt, &eHalGopId))
        {
            _DrvGopInit(u16GopCnt,eHalGopId);
        }
    }
    return bRet;
}
bool DrvGopSetBindOsd(void)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    u16 u16GopCnt;
    for(u16GopCnt=E_DRV_GOP_ID_0;u16GopCnt<HAL_RGN_GOP_NUM;u16GopCnt++)
    {
        if(_DrvGopTransId(u16GopCnt, &eHalGopId))
        {
            HalGopBindOsd(eHalGopId);
        }
    }
    return bRet;
}
u8 DrvGopGetGwinEnableCount(DrvGopIdType_e eGopId, DrvGopGwinIdType_e *eGwinId)
{
    u8 u8GwinEnCnt = 0;
    u8 u8GwinIdx;
    for(u8GwinIdx = E_DRV_GOP_GWIN_ID_0; u8GwinIdx < HAL_RGN_GOP_GWIN_NUM; u8GwinIdx++)
    {
        if(_tGopLocSettings[eGopId].tGwinLocSet[u8GwinIdx].bGwinEn == TRUE)
        {
            u8GwinEnCnt++;
            *eGwinId = u8GwinIdx;
        }
    }
    return u8GwinEnCnt;
}
bool DrvGopRgnSetEnable(DrvGopIdType_e eGopId,  DrvGopGwinIdType_e eGwinId, bool bEn)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    DrvGopGwinIdType_e eDrvGwinId;

    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].bGwinEn!=bEn)
        {
            if(DrvGopGetGwinEnableCount(eGopId,&eDrvGwinId)==0 && bEn)
            {
                HalGopEnsureIdle(eHalGopId);
            }
            _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].bGwinEn = bEn;
            HalGopSetEnableGwin(eHalGopId, (HalGopGwinIdType_e)eGwinId, bEn);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = 1;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    return bRet;
}

bool DrvGopUpdateBufferXoffset(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, u16 u16Hoffset)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
        {
            if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.u16Base_XOffset != u16Hoffset)
            {
                _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.u16Base_XOffset = u16Hoffset;
                HalGopUpdateBaseXoffset(eHalGopId, (HalGopGwinIdType_e)eGwinId, u16Hoffset);
            }
            else
            {
                DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
            }
            bRet = TRUE;
        }
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
        return bRet;
    }
    return bRet;
}
bool DrvGopUpdateBase(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, u32 u32BaseAddr)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;

    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        HalGopMiuSelect(eHalGopId, &u32BaseAddr);
    }
    else
    {
        DRVRGNDBG("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
        return bRet;
    }
    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.u32BaseAddr != u32BaseAddr)
        {
            _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.u32BaseAddr = u32BaseAddr;
            HalGopUpdateBase(eHalGopId, (HalGopGwinIdType_e)eGwinId, u32BaseAddr);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetMirror(DrvGopIdType_e eGopId, bool bEn)
{
    bool bRet = TRUE;
    //HalGopIdType_e eHalGopId;

    DRVRGNHLDBG("%s %d: Not support\n", __FUNCTION__, __LINE__);
    /*if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].bMirror!= bEn)
        {
            _tGopLocSettings[eGopId].bMirror = bEn;
            HalGopSetMirror(eHalGopId, bEn);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }*/
    return bRet;

}

bool DrvGopSetFlip(DrvGopIdType_e eGopId, bool bEn)
{
    bool bRet = TRUE;
    //HalGopIdType_e eHalGopId;

    DRVRGNHLDBG("%s %d: Not support\n", __FUNCTION__, __LINE__);
    /*if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].bFlip!= bEn)
        {
            _tGopLocSettings[eGopId].bFlip = bEn;
            HalGopSetFlip(eHalGopId, bEn);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }*/
    return bRet;

}

bool DrvGopSetMemPitchDirect(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, u32 u32MemPitch)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;

    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].u32MemPitch!= u32MemPitch)
        {
            _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].u32MemPitch = u32MemPitch;
            HalGopSetGwinMemPitchDirect(eHalGopId, (HalGopGwinIdType_e)eGwinId,
                _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].u32MemPitch);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}
bool DrvGopSetSrcFmt(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, DrvGopGwinSrcFmtType_e eSrcFmt)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    HalGopGwinSrcFormat_e eHalSrcFmt;
    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.eSrcFmt != eSrcFmt)
        {
            _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.eSrcFmt = eSrcFmt;
            eHalSrcFmt = _DrvGopTranSrcFmt(eSrcFmt);
            if(eHalSrcFmt==E_HAL_GOP_GWIN_SRC_NOTSUPPORT)
            {
                return FALSE;
            }
            HalGopSetGwinSrcFmt(eHalGopId, (HalGopGwinIdType_e)eGwinId, eHalSrcFmt);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetAlphaBlending(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, DrvGopGwinAlphaConfig_t *ptAlphaCfg)
{
    bool bRet = 0;
    HalGopIdType_e eHalGopId;

    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        if(ptAlphaCfg->ePixelAlphaType==E_DRV_GOP_GWIN_ARGB1555_DEFINE_NOUSE)
        {
            if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinAblSet.eAlphaType != ptAlphaCfg->eAlphaType ||
                *_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8ConstantAlphaVal != ptAlphaCfg->tAlphaVal.u8ConstantAlpahValue)
            {
                _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinAblSet.eAlphaType = ptAlphaCfg->eAlphaType;
                if(ptAlphaCfg->eAlphaType == E_DRV_GOP_GWIN_ALPHA_CONSTANT)
                {
                    *_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8ConstantAlphaVal = ptAlphaCfg->tAlphaVal.u8ConstantAlpahValue;
                }
                HalGopSetAlphaBlending(eHalGopId, (HalGopGwinIdType_e)eGwinId,
                                       ptAlphaCfg->eAlphaType == E_DRV_GOP_GWIN_ALPHA_CONSTANT ? TRUE : FALSE,
                                       ptAlphaCfg->tAlphaVal.u8ConstantAlpahValue);
            }
        }
        else
        {
            if(ptAlphaCfg->ePixelAlphaType==E_DRV_GOP_GWIN_ARGB1555_DEFINE_ALPHA0 &&
                (*_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8Argb1555Alpha0Val != ptAlphaCfg->tAlphaVal.u8Argb1555Alpha0Val))
            {
                *_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8Argb1555Alpha0Val=ptAlphaCfg->tAlphaVal.u8Argb1555Alpha0Val;
                HalGopSetArgb1555Alpha(eHalGopId, (HalGopGwinIdType_e)eGwinId, E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA0,
                    ptAlphaCfg->tAlphaVal.u8Argb1555Alpha0Val);
            }
            else if(ptAlphaCfg->ePixelAlphaType==E_DRV_GOP_GWIN_ARGB1555_DEFINE_ALPHA1 &&
                (*_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8Argb1555Alpha1Val != ptAlphaCfg->tAlphaVal.u8Argb1555Alpha1Val))
            {
                *_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].p8Argb1555Alpha1Val=ptAlphaCfg->tAlphaVal.u8Argb1555Alpha1Val;
                HalGopSetArgb1555Alpha(eHalGopId, (HalGopGwinIdType_e)eGwinId, E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA1,
                    ptAlphaCfg->tAlphaVal.u8Argb1555Alpha1Val);
            }
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetColorKey(DrvGopIdType_e eGopId, DrvGopColorKeyConfig_t *ptColorKeyCfg, bool bVYU)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;

    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGopColorKeySet.bEn != ptColorKeyCfg->bEn ||
        _tGopLocSettings[eGopId].tGopColorKeySet.u8R != ptColorKeyCfg->u8R ||
        _tGopLocSettings[eGopId].tGopColorKeySet.u8G != ptColorKeyCfg->u8G ||
        _tGopLocSettings[eGopId].tGopColorKeySet.u8B != ptColorKeyCfg->u8B)
        {
            _tGopLocSettings[eGopId].tGopColorKeySet.bEn = ptColorKeyCfg->bEn;
            _tGopLocSettings[eGopId].tGopColorKeySet.u8R = ptColorKeyCfg->u8R;
            _tGopLocSettings[eGopId].tGopColorKeySet.u8G = ptColorKeyCfg->u8G;
            _tGopLocSettings[eGopId].tGopColorKeySet.u8B = ptColorKeyCfg->u8B;
            HalGopSetColorKey(eHalGopId,(HalGopColorKeyConfig_t *)ptColorKeyCfg, bVYU);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetPaletteRiuOneMem(DrvGopIdType_e eGopId, DrvGopPaletteConfig_t *ptCfg)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;

    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        HalGopSetPaletteRiu(eHalGopId,(HalGopPaletteConfig_t *)ptCfg);
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetGwinSizeWithoutFmt(DrvGopIdType_e eGopId, DrvGopGwinIdType_e eGwinId, DrvGopWindowConfig_t *ptWinCfg)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    HalGopWindowType_t tHalGwinCfg;
    HalGopGwinSrcFormat_e enType;
    if(_DrvGopTransGwinId(eGopId, eGwinId, &eHalGopId) == TRUE)
    {
        //if(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16X != ptWinCfg->u16X ||
        //_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Y != ptWinCfg->u16Y ||
        //_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Width != ptWinCfg->u16Width ||
        //_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Height != ptWinCfg->u16Height)
        //{
            tHalGwinCfg.u16X = _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16X = ptWinCfg->u16X;
            tHalGwinCfg.u16Y = _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Y = ptWinCfg->u16Y;
            tHalGwinCfg.u16Width = _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Width = ptWinCfg->u16Width;
            tHalGwinCfg.u16Height = _tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.tDisplayWin.u16Height = ptWinCfg->u16Height;
            enType = _DrvGopTranSrcFmt(_tGopLocSettings[eGopId].tGwinLocSet[eGwinId].tGwinGenSet.eSrcFmt);
            HalGopSetGwinSize(eHalGopId, (HalGopGwinIdType_e)eGwinId, &tHalGwinCfg,enType);
        //}
        //else
        //{
        //    DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        //}
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetStretchWindowSize(DrvGopIdType_e eGopId, DrvGopWindowConfig_t *ptSrcWinCfg, DrvGopWindowConfig_t *ptDstWinCfg)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    HalGopWindowType_t tHalSrcWinCfg, tHalDstWinCfg;

    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        if(_tGopLocSettings[eGopId].tGopSrcStrWinSet.u16X != ptSrcWinCfg->u16X ||
        _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Y != ptSrcWinCfg->u16Y ||
        _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Width != ptSrcWinCfg->u16Width ||
        _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Height != ptSrcWinCfg->u16Height ||
        _tGopLocSettings[eGopId].tGopDstStrWinSet.u16X != ptDstWinCfg->u16X ||
        _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Y != ptDstWinCfg->u16Y ||
        _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Width != ptDstWinCfg->u16Width ||
        _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Height != ptDstWinCfg->u16Height)
        {
            tHalSrcWinCfg.u16X = _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16X = ptSrcWinCfg->u16X;
            tHalSrcWinCfg.u16Y = _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Y = ptSrcWinCfg->u16Y;
            tHalSrcWinCfg.u16Width = _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Width = ptSrcWinCfg->u16Width;
            tHalSrcWinCfg.u16Height = _tGopLocSettings[eGopId].tGopSrcStrWinSet.u16Height = ptSrcWinCfg->u16Height;
            tHalDstWinCfg.u16X = _tGopLocSettings[eGopId].tGopDstStrWinSet.u16X = ptDstWinCfg->u16X;
            tHalDstWinCfg.u16Y = _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Y = ptDstWinCfg->u16Y;
            tHalDstWinCfg.u16Width = _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Width = ptDstWinCfg->u16Width;
            tHalDstWinCfg.u16Height = _tGopLocSettings[eGopId].tGopDstStrWinSet.u16Height = ptDstWinCfg->u16Height;
            HalGopSetStretchWindowSize(eHalGopId, &tHalSrcWinCfg, &tHalDstWinCfg);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}
bool DrvGopSetAlphaZeroOpaque(DrvGopIdType_e eGopId, bool bEn,bool bConAlpha,DrvGopGwinSrcFmtType_e eSrcFmt)
{
    bool bRet = TRUE;
    bool binv = TRUE;
    HalGopGwinSrcFormat_e eHalSrcFmt;
    HalGopIdType_e eHalGopId;
    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {
        eHalSrcFmt = _DrvGopTranSrcFmt(eSrcFmt);
        binv = HalGopCheckAlphaZeroOpaque(eHalGopId,bEn,bConAlpha,eHalSrcFmt);
        HalGopSetAlphaInvert(eHalGopId, binv);
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvGopSetVideoTimingInfo(DrvGopIdType_e eGopId, DrvGopVideoTimingInfoConfig_t *ptTimingCfg)
{
    bool bRet = TRUE;
    HalGopIdType_e eHalGopId;
    HalGopVideoTimingInfoConfig_t tHalVdTmgCfg;
    HalGopQueryFuncConfig_t stQ;

    if(_DrvGopTransId(eGopId, &eHalGopId) == TRUE)
    {

            tHalVdTmgCfg.u16Htotal      = _tGopLocSettings[eGopId].tVideoTiming.u16Htotal      = ptTimingCfg->u16Htotal;
            tHalVdTmgCfg.u16Vtotal      = _tGopLocSettings[eGopId].tVideoTiming.u16Vtotal      = ptTimingCfg->u16Vtotal;
            tHalVdTmgCfg.u16HdeStart    = _tGopLocSettings[eGopId].tVideoTiming.u16HdeStart    = ptTimingCfg->u16HdeStart;
            tHalVdTmgCfg.u16VdeStart    = _tGopLocSettings[eGopId].tVideoTiming.u16VdeStart    = ptTimingCfg->u16VdeStart;
            tHalVdTmgCfg.u16Width       = _tGopLocSettings[eGopId].tVideoTiming.u16Width       = ptTimingCfg->u16Width;
            tHalVdTmgCfg.u16Height      = _tGopLocSettings[eGopId].tVideoTiming.u16Height      = ptTimingCfg->u16Height;
            tHalVdTmgCfg.bInterlaceMode = _tGopLocSettings[eGopId].tVideoTiming.bInterlaceMode = ptTimingCfg->bInterlaceMode;
            tHalVdTmgCfg.bYuvOutput     = _tGopLocSettings[eGopId].tVideoTiming.bYuvOutput     = ptTimingCfg->bYuvOutput;
            stQ.stInQ.eGopId = eHalGopId;
            stQ.stInQ.enQF = E_HAL_QUERY_SETVIDEOTIMINGINFO;
            stQ.stInQ.pCfg = &tHalVdTmgCfg;
            stQ.stInQ.u32CfgSize = sizeof(tHalVdTmgCfg);
            _DrvGopExecQueryFunc(&stQ);

        bRet = TRUE;
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}
