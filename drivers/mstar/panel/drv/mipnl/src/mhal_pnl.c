/*
* mhal_pnl.c- Sigmastar
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

#define __MHAL_PNL_C__
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_pnl_os.h"
#include "mhal_pnl_datatype.h"
#include "mhal_pnl.h"
#include "pnl_debug.h"
#include "drv_pnl_if.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structurs
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Private Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Pubic Functions
//-------------------------------------------------------------------------------------------------
bool MhalPnlCreateInstance(void **pCtx, MhalPnlLinkType_e enLinkType)
{
    return DrvPnlIfCreateInstance(pCtx, enLinkType, 0);
}

bool MhalPnlCreateInstanceEx(void **pCtx, MhalPnlLinkType_e enLinkType, u16 u16Id)
{
    return DrvPnlIfCreateInstance(pCtx, enLinkType, u16Id);
}

bool MhalPnlDestroyInstance(void *pCtx)
{
    bool bRet;

    if(pCtx == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet =  DrvPnlIfDestroyInstance(pCtx);
    }
    return bRet;
}


bool MhalPnlSetParamConfig(void *pCtx, MhalPnlParamConfig_t *pParamCfg)
{
    bool bRet = 1;

    if(pCtx == NULL || pParamCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetParamConfig(pCtx, pParamCfg);
    }
    return bRet;
}


bool MhalPnlGetParamConfig(void *pCtx, MhalPnlParamConfig_t *pParamCfg)
{
    bool bRet;

    if(pCtx == NULL || pParamCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetParamConfig(pCtx, pParamCfg);
    }
    return bRet;
}


bool MhalPnlSetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pMipiDsiCfg)
{
    bool bRet;

    if(pCtx == NULL || pMipiDsiCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetMipiDsiConfig(pCtx, pMipiDsiCfg);
    }
    return bRet;
}


bool MhalPnlGetMipiDsiConfig(void *pCtx, MhalPnlMipiDsiConfig_t *pMipiDsiCfg)
{
    bool bRet;

    if(pCtx == NULL || pMipiDsiCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetMipiDsiConfig(pCtx, pMipiDsiCfg);
    }
    return bRet;
}



bool MhalPnlSetSscConfig(void *pCtx, MhalPnlSscConfig_t *pSscCfg)
{
    bool bRet;

    if(pCtx == NULL || pSscCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetSscConfig(pCtx, pSscCfg);
    }
    return bRet;
}



bool MhalPnlSetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pTimingCfg)
{
    bool bRet;

    if(pCtx == NULL || pTimingCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetTimingConfig(pCtx, pTimingCfg);
    }
    return bRet;
}



bool MhalPnlGetTimingConfig(void *pCtx, MhalPnlTimingConfig_t *pTimingCfg)
{
    bool bRet;

    if(pCtx == NULL || pTimingCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetTimingConfig(pCtx, pTimingCfg);
    }
    return bRet;
}



bool MhalPnlSetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pPowerCfg)
{
    bool bRet;

    if(pCtx == NULL || pPowerCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetPowerConfig(pCtx, pPowerCfg);
    }
    return bRet;
}



bool MhalPnlGetPowerConfig(void *pCtx, MhalPnlPowerConfig_t *pPowerCfg)
{
    bool bRet;

    if(pCtx == NULL || pPowerCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetPowerConfig(pCtx, pPowerCfg);
    }
    return bRet;
}



bool MhalPnlSetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pBackLightOnOffCfg)
{
    bool bRet;

    if(pCtx == NULL || pBackLightOnOffCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetBackLightOnOffConfig(pCtx, pBackLightOnOffCfg);
    }
    return bRet;
}



bool MhalPnlGetBackLightOnOffConfig(void *pCtx, MhalPnlBackLightOnOffConfig_t *pBackLightOnOffCfg)
{
    bool bRet;

    if(pCtx == NULL || pBackLightOnOffCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetBackLightOnOffConfig(pCtx, pBackLightOnOffCfg);
    }
    return bRet;
}



bool MhalPnlSetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pBackLightLevelCfg)
{
    bool bRet;

    if(pCtx == NULL || pBackLightLevelCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetBackLightLevelConfig(pCtx, pBackLightLevelCfg);
    }
    return bRet;
}



bool MhalPnlGetBackLightLevelConfig(void *pCtx, MhalPnlBackLightLevelConfig_t *pBackLightLevelCfg)
{
    bool bRet;

    if(pCtx == NULL || pBackLightLevelCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetBackLightLevelConfig(pCtx, pBackLightLevelCfg);
    }
    return bRet;
}



bool MhalPnlSetDrvCurrentConfig(void *pCtx, MhalPnlDrvCurrentConfig_t *pDrvCurrentCfg)
{
    bool bRet;

    if(pCtx == NULL || pDrvCurrentCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetDrvCurrentConfig(pCtx, pDrvCurrentCfg);
    }
    return bRet;
}



bool MhalPnlSetTestPatternConfig(void *pCtx, MhalPnlTestPatternConfig_t *pTestPatternCfg)
{
    bool bRet;

    if(pCtx == NULL || pTestPatternCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetTestPatternConfig(pCtx, pTestPatternCfg);
    }
    return bRet;
}

bool MhalPnlSetDebugLevel(void *pDbgLevel)
{
    bool bRet;

    if(pDbgLevel == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetDbgLevel(pDbgLevel);
    }
    return bRet;
}

//new
bool MhalPnlSetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg)
{
    bool bRet = 1;

    if(pCtx == NULL || pUdParamCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfSetUnifiedParamConfig(pCtx, pUdParamCfg);
    }
    return bRet;
}
bool MhalPnlGetUnifiedParamConfig(void *pCtx, MhalPnlUnifiedParamConfig_t *pUdParamCfg)
{
    bool bRet;

    if(pCtx == NULL || pUdParamCfg == NULL)
    {
        PNL_ERR("%s %d, Param Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = DrvPnlIfGetUnifiedParamConfig(pCtx, pUdParamCfg);
    }
    return bRet;
}
