/*
* Pnl.c- Sigmastar
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
#define __PNL_C__
#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

#include "drv_pnl_os.h"
#include "mhal_pnl.h"
#include "mhal_pnl_datatype.h"
#include "PnlTblDef.h"
#include "PnlTbl.h"
//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 u32PnlParamCfgSize;
    u8 *pPnlParamCfg;
    u32 u32MipiDsiCfgSize;
    u8 *pMipiDsiCfg;
}PnlConfig_t;
//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

void *gpPnlCtx = NULL;

//-------------------------------------------------------------------------------------------------
// Pirvate Function
//-------------------------------------------------------------------------------------------------

void PnlInit(PnlConfig_t *pPnlCfg)
{
    MhalPnlParamConfig_t stPnlParmCfg;
    MhalPnlTestPatternConfig_t stTestPatternCfg={0,0xff,0xff,0xff};

    if(pPnlCfg->u32PnlParamCfgSize == sizeof(MhalPnlParamConfig_t))
    {
        memcpy(&stPnlParmCfg, pPnlCfg->pPnlParamCfg, sizeof(MhalPnlParamConfig_t));
    }
    else
    {
        if(pPnlCfg->u32PnlParamCfgSize == 0 || pPnlCfg->pPnlParamCfg == NULL)
        {
            memcpy(&stPnlParmCfg, stPanel_LX50FWB4001, sizeof(MhalPnlParamConfig_t));
        }
        else
        {
            printf("%s %d, size of ParamCfg is not correct\n", __FUNCTION__, __LINE__);
            return;
        }
    }


    if(MhalPnlCreateInstance(&gpPnlCtx, stPnlParmCfg.eLinkType) == 0)
    {
        printf("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
        return;
    }

    if(gpPnlCtx == 0)
    {
        printf("%s %d, PnlCtx is NULL\n", __FUNCTION__, __LINE__);
        return;
    }

    if(MhalPnlSetTestPatternConfig(gpPnlCtx , &stTestPatternCfg) == 0)
    {
        printf("%s %d, set test pattern Fail\n", __FUNCTION__, __LINE__);
        return;
    }
    if(MhalPnlSetParamConfig(gpPnlCtx , &stPnlParmCfg) == 0)
    {
        printf("%s %d, SetParamConfig Fail\n", __FUNCTION__, __LINE__);
        return;
    }


}

