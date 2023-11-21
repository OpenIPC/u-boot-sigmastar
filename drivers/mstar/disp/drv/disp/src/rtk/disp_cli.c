/*
* disp_cli.c- Sigmastar
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
/*=============================================================*/
// Variable definition
/*=============================================================*/
#define DISP_CLI_C
#include "drv_disp_os.h"
#include "disp_debug.h"
#include "mhal_disp.h"
#include "drv_disp_if.h"
#include "disp_ut.h"
#include "disp_sysfs.h"
#include "sys_sys_isw_cli.h"

static u8 _DrvDispCliRunUt(DrvDispOsStrConfig_t *pstStrCfg)
{
    u8 u8Ret = 0;
#ifndef DISP_RTOS_UT_DISABLE
    u8Ret = DispTestStore(pstStrCfg);
#endif
    return u8Ret;
}

void DrvDispCliRun(DrvDispOsStrConfig_t *pstStrCfg)
{
    if(NULL!=pstStrCfg)
    {
        if(0 ==_DrvDispCliRunUt(pstStrCfg))  //Test
        {
#if DISP_STATISTIC_EN
            DrvDispSysfsGetInfo(pstStrCfg);
#endif
        }
    }
}
static char g_szDispTestAHelpTxt[] = "disp test ut";
static char g_szDispTestAUsageTxt[] = "TODO";
/*=============================================================*/
// Global Variable definition
/*=============================================================*/

int DrvDispCliParser(CLI_t *pvCli, char *p)
{
    u32 idx;
    CLI_t *pCli = pvCli;
    DrvDispOsStrConfig_t stTest;

    stTest.argc = CliTokenCount(pCli);
    if(stTest.argc > 0)
    {
        for(idx=0;idx<stTest.argc;idx++)
        {
            pCli->tokenLvl++;
            stTest.argv[idx] = (u8 *)CliTokenPop(pCli);
        }
        DrvDispCliRun(&stTest);
    }
    else
    {
        DISP_ERR("print disp xxx \n");
        DISP_ERR("ex. disp ints \n");
        DISP_ERR("ex. disp cfg \n");
    }
    return 0;
}
SS_RTOS_CLI_CMD(disp,
        g_szDispTestAHelpTxt,
        g_szDispTestAUsageTxt,
        DrvDispCliParser);
