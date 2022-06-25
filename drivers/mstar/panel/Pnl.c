////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
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

