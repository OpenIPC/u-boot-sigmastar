/*
* drv_cover.c- Sigmastar
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

#define __DRV_COVER_C__
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_cover.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

_DrvCoverLocalSettingsConfig_t _tCoverLocSettings[E_HAL_COVER_ID_NUM];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

bool _DrvCoverTransId(DrvCoverIdType_e eCoverId, HalCoverIdType_e *pHalId)
{
    bool bRet = TRUE;
    bool str[8];
    
    if(eCoverId == E_DRV_ISPSC0_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT0);
    }
    else if(eCoverId == E_DRV_ISPSC1_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT1);
    }
    else if(eCoverId == E_DRV_ISPSC2_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT2);
    }
    else if(eCoverId == E_DRV_ISPSC3_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT3);
    }
    else if(eCoverId == E_DRV_DIP_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_DIP);
    }
    else if(eCoverId == E_DRV_ISPSC4_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT4);
    }
    else if(eCoverId == E_DRV_ISPSC5_COVER)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT5);
    }
    else
    {
        return FALSE;
    }
    bRet = HalCoverTransHalId((const char*)str,pHalId);
    return bRet;
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------


bool DrvCoverRgnInit(void)
{
    bool bRet = TRUE;
    u8 u8CoverId;
    u8 u8WinId;
    HalCoverIdType_e eHalCoverId;
    HalCoverWindowType_t tHalWinCfg;
    HalCoverColorType_t tHalType;
    for(u8CoverId = E_DRV_ISPSC0_COVER; u8CoverId < E_DRV_COVER_ID_NUM; u8CoverId++)
    {
        if(_DrvCoverTransId(u8CoverId, &eHalCoverId) == TRUE)
        {
            _tCoverLocSettings[eHalCoverId].bDbEn = FALSE;
            HalCoverInit();
            for(u8WinId = E_HAL_COVER_BWIN_ID_0; u8WinId < E_HAL_COVER_BWIN_ID_NUM; u8WinId++)
            {
                _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].bBwinEn = FALSE;
                tHalType.u8R =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinColorSet.u8R= 0;
                tHalType.u8G =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinColorSet.u8G= 0;
                tHalType.u8B =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinColorSet.u8B = 0;
                tHalWinCfg.u16X =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinSet.u16X = 0;
                tHalWinCfg.u16Y =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinSet.u16Y= 0;
                tHalWinCfg.u16Width=_tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinSet.u16Width= 0;
                tHalWinCfg.u16Height=_tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].tWinSet.u16Height= 0;

                HalCoverSetEnableWin(eHalCoverId, (HalCoverWinIdType_e)u8WinId,
                    _tCoverLocSettings[eHalCoverId].tCoverWinsSet[u8WinId].bBwinEn);
                HalCoverSetWindowSize(eHalCoverId, (HalCoverWinIdType_e)u8WinId, &tHalWinCfg);
                HalCoverSetColor(eHalCoverId, (HalCoverWinIdType_e)u8WinId,&tHalType);
            }
        }
    }

    return bRet;
}
bool DrvCoverSetWinSize(DrvCoverIdType_e eCoverId, DrvCoverWinIdType_e eWinId, DrvCoverWindowConfig_t *ptWinCfg)
{
    bool bRet = FALSE;
    HalCoverIdType_e eHalCoverId;
    HalCoverWindowType_t tHalWinCfg;

    if(_DrvCoverTransId(eCoverId, &eHalCoverId) == TRUE)
    {
        if(_tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16X != ptWinCfg->u16X ||
        _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Y != ptWinCfg->u16Y ||
        _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Width != ptWinCfg->u16Width ||
        _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Height != ptWinCfg->u16Height)
        {
            tHalWinCfg.u16X = _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16X = ptWinCfg->u16X;
            tHalWinCfg.u16Y = _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Y = ptWinCfg->u16Y;
            tHalWinCfg.u16Width = _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Width = ptWinCfg->u16Width;
            tHalWinCfg.u16Height = _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinSet.u16Height = ptWinCfg->u16Height;
            HalCoverSetWindowSize(eHalCoverId, (HalCoverWinIdType_e)eWinId, &tHalWinCfg);
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

bool DrvCoverSetColor(DrvCoverIdType_e eCoverId, DrvCoverWinIdType_e eWinId, DrvCoverColorConfig_t *ptColorCfg)
{
    bool bRet = FALSE;
    HalCoverIdType_e eHalCoverId;
    HalCoverColorType_t tHalType;
    if(_DrvCoverTransId(eCoverId, &eHalCoverId) == TRUE)
    {
        if(_tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8R != ptColorCfg->u8R ||
        _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8G != ptColorCfg->u8G ||
        _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8B != ptColorCfg->u8B)
        {
            tHalType.u8R =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8R= ptColorCfg->u8R;
            tHalType.u8G =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8G= ptColorCfg->u8G;
            tHalType.u8B =  _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].tWinColorSet.u8B = ptColorCfg->u8B;
            HalCoverSetColor(eHalCoverId, (HalCoverWinIdType_e)eWinId, &tHalType);
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

bool DrvCoverSetEnableWin(DrvCoverIdType_e eCoverId,  DrvCoverWinIdType_e eWinId, bool bEn)
{
    bool bRet = FALSE;
    HalCoverIdType_e eHalCoverId;
    if(_DrvCoverTransId(eCoverId, &eHalCoverId) == TRUE)
    {
        if(_tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].bBwinEn != bEn)
        {
            _tCoverLocSettings[eHalCoverId].tCoverWinsSet[eWinId].bBwinEn= bEn;
            HalCoverSetEnableWin(eHalCoverId, (HalCoverWinIdType_e)eWinId, bEn);
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
