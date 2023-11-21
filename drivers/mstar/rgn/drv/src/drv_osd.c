/*
* drv_osd.c- Sigmastar
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

#define __DRV_OSD_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_osd.h"

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
_DrvOsdLocalSettingsConfig_t _tOsdpLocSettings[E_HAL_OSD_ID_MAX];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

bool _DrvOsdTransId(DrvOsdId_e eOsdId, HalOsdIdType_e *pHalId)
{
    bool bRet = TRUE;
    bool str[16];

    if(eOsdId == E_DRV_OSD_PORT0)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT0);
    }
    else if(eOsdId == E_DRV_OSD_PORT1)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT1);
    }
    else if(eOsdId == E_DRV_OSD_PORT2)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT2);
    }
    else if(eOsdId == E_DRV_OSD_PORT3)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT3);
    }
    else if(eOsdId == E_DRV_OSD_PORT4)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT4);
    }
    else if(eOsdId == E_DRV_OSD_PORT5)
    {
        strcpy((char *)str,RGN_DEVICE_SC_PORT5);
    }
    else if(eOsdId == E_DRV_OSD_DIP)
    {
        strcpy((char *)str,RGN_DEVICE_DIP);
    }
    else if(eOsdId == E_DRV_OSD_LDC)
    {
        strcpy((char *)str,RGN_DEVICE_LDC);
    }
    else if(eOsdId == E_DRV_OSD_DISP0_CUR)
    {
        strcpy((char *)str,RGN_DEVICE_DISP0_CUR);
    }
    else if(eOsdId == E_DRV_OSD_DISP0_UI)
    {
        strcpy((char *)str,RGN_DEVICE_DISP0_UI);
    }
    else if(eOsdId == E_DRV_OSD_DISP1_CUR)
    {
        strcpy((char *)str,RGN_DEVICE_DISP1_CUR);
    }
    else if(eOsdId == E_DRV_OSD_DISP1_UI)
    {
        strcpy((char *)str,RGN_DEVICE_DISP1_UI);
    }
    else
    {
        return FALSE;
    }
    bRet = HalOsdTransId((const char*)str,pHalId);
    return bRet;
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
static bool _DrvOsdExecQueryFunc(HalOsdQueryFuncConfig_t *pstQ)
{
    HalOsdQuery((void *)pstQ);
    
    if (pstQ->stOutQ.enQueryRet == E_HAL_QUERY_OSD_OK)
    {
        if (pstQ->stOutQ.pfForSet)
        {
            pstQ->stOutQ.pfForSet(pstQ->stInQ.eOsdId ,pstQ->stInQ.pCfg);
        }
        if (pstQ->stOutQ.pfForPatch)
        {
            pstQ->stOutQ.pfForPatch(pstQ->stInQ.eOsdId ,pstQ->stOutQ.pPatch);
        }
    }
    
    return pstQ->stOutQ.enQueryRet;
}
bool DrvOsdInit(void)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    DrvOsdId_e eOsdId;

    HalOsdInit();
    bRet = HalRgnCheckBindRation((HalRgnOsdFlag_e*)&geGopOsdFlag);
    for(eOsdId=E_DRV_OSD_PORT0;eOsdId<E_DRV_OSD_ID_MAX;eOsdId++)
    {
        if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
        {    
            _tOsdpLocSettings[eHalOsdId].bBypassEn = 1;
            _tOsdpLocSettings[eHalOsdId].bOsdEn = 0;
            HalOsdSetOsdBypassForScEnable(eHalOsdId,TRUE);
            HalOsdSetOsdEnable(eHalOsdId,0);
        }
    }
    HalOsdSetBindGop(geGopOsdFlag);
    return bRet;
}
bool DrvOsdSetBindGop(u32 u32GopOsdFlag)
{
    bool bRet = TRUE;
    HalOsdSetBindGop(u32GopOsdFlag);
    return bRet;
}
bool DrvOsdSetEnable(DrvOsdId_e eOsdId, bool bEn)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
    {
        if(_tOsdpLocSettings[eHalOsdId].bOsdEn != bEn)
        {
            _tOsdpLocSettings[eHalOsdId].bOsdEn = bEn;
            HalOsdSetOsdEnable(eHalOsdId,bEn);
        }
        else
        {
            DRVRGNHLDBG("%s %d: Not need to exec\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

bool DrvOsdSetColorInverseEn(DrvOsdId_e eOsdId, bool bEn) {
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    HalOsdQueryFuncConfig_t stQ;
    
    if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
    {    
        _tOsdpLocSettings[eHalOsdId].bColorInv = bEn;
        stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_EN;
        stQ.stInQ.eOsdId = eHalOsdId;
        stQ.stInQ.pCfg = &_tOsdpLocSettings[eHalOsdId].bColorInv;
        stQ.stInQ.u32CfgSize = sizeof(bEn);
        _DrvOsdExecQueryFunc(&stQ);
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvOsdSetColorInverseParam(DrvOsdId_e eOsdId, DrvOsdColorInvWindowIdType_e eAeId, DrvOsdColorInvParamConfig_t *ptColInvCfg)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    HalOsdColorInvParamConfig_t tHalColInvCfg;
    HalOsdQueryFuncConfig_t stQ;
    
    memcpy(&tHalColInvCfg,ptColInvCfg,sizeof(DrvOsdColorInvParamConfig_t));
    if(_DrvOsdTransId(eOsdId, &eHalOsdId))
    {
        stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_PARAM_CHECK;
        stQ.stInQ.eOsdId = eHalOsdId;
        stQ.stInQ.pCfg = &tHalColInvCfg;
        stQ.stInQ.u32CfgSize = sizeof(tHalColInvCfg);
        if(_DrvOsdExecQueryFunc(&stQ)==E_HAL_QUERY_OSD_OK)
        {
            memcpy(&_tOsdpLocSettings[eHalOsdId].stColorInvCfg[eAeId],ptColInvCfg,sizeof(DrvOsdColorInvParamConfig_t));
            stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_PARAM;
            tHalColInvCfg.enCiWin = eAeId;
            _DrvOsdExecQueryFunc(&stQ);
        }
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvOsdSetColorInverseUpdate(DrvOsdId_e eOsdId)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    HalOsdQueryFuncConfig_t stQ;
    
    if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
    {
        stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_UPDATE;
        stQ.stInQ.eOsdId = eHalOsdId;
        stQ.stInQ.pCfg = NULL;
        stQ.stInQ.u32CfgSize = 0;
        _DrvOsdExecQueryFunc(&stQ);
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvOsdWriteColorInverseData(DrvOsdId_e eOsdId, u16 addr, u32 wdata)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    HalOsdQueryFuncConfig_t stQ;
    HalOsdColorInvRWConfig_t stRw;
    
    if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
    {
        stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_WRITEDATA;
        stQ.stInQ.eOsdId = eHalOsdId;
        stRw.addr = addr;
        stRw.wdata = wdata;
        stQ.stInQ.pCfg = &stRw;
        stQ.stInQ.u32CfgSize = sizeof(stRw);
        _DrvOsdExecQueryFunc(&stQ);
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

bool DrvOsdReadColorInverseData(DrvOsdId_e eOsdId, u16 addr, u32 *rdata)
{
    bool bRet = TRUE;
    HalOsdIdType_e eHalOsdId;
    HalOsdQueryFuncConfig_t stQ;
    HalOsdColorInvRWConfig_t stRw;
    
    if(_DrvOsdTransId(eOsdId, &eHalOsdId) == TRUE)
    {
        stQ.stInQ.enQF = E_HAL_QUERY_OSD_CI_READDATA;
        stQ.stInQ.eOsdId = eHalOsdId;
        stRw.addr = addr;
        stRw.rdata = rdata;
        stQ.stInQ.pCfg = &stRw;
        stQ.stInQ.u32CfgSize = sizeof(stRw);
        _DrvOsdExecQueryFunc(&stQ);
    }
    else
    {
        DRVRGNERR("%s %d: Fail ID\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}
