/*
* hdmitx_cli.c- Sigmastar
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
#define HDMITX_CLI_C
#include "drv_hdmitx_os.h"
#include "hdmitx_debug.h"
#include "mhal_hdmitx.h"
#include "drv_hdmitx_if.h"
#include "hdmitx_ut.h"
#include "hdmitx_sysfs.h"
#include "sys_sys_isw_cli.h"

static u8 _DrvHdmitxCliRunUt(DrvHdmitxOsStrConfig_t *pstStrCfg)
{
    u8 u8Ret = 0;
#ifndef HDMITX_RTOS_UT_DISABLE
    u8Ret = HdmitxTestStore(pstStrCfg);
#endif
    return u8Ret;
}

void DrvHdmitxCliRun(DrvHdmitxOsStrConfig_t *pstStrCfg)
{
    if(NULL!=pstStrCfg)
    {
        if(0 ==_DrvHdmitxCliRunUt(pstStrCfg))  //Test
        {
            DrvHdmitxSysfsGetInfo(pstStrCfg);
        }
    }
}
static char g_szHdmitxTestAHelpTxt[] = "hdmitx test ut";
static char g_szHdmitxTestAUsageTxt[] = "TODO";
/*=============================================================*/
// Global Variable definition
/*=============================================================*/

int DrvHdmitxCliParser(CLI_t *pvCli, char *p)
{
    u32 idx;
    CLI_t *pCli = pvCli;
    DrvHdmitxOsStrConfig_t stTest;

    stTest.argc = CliTokenCount(pCli);
    if(stTest.argc > 0)
    {
        for(idx=0;idx<stTest.argc;idx++)
        {
            pCli->tokenLvl++;
            stTest.argv[idx] = (u8 *)CliTokenPop(pCli);
        }
        DrvHdmitxCliRun(&stTest);
    }
    else
    {
        HDMITX_ERR("print hdmitx xxx \n");
        HDMITX_ERR("ex. hdmitx ints \n");
        HDMITX_ERR("ex. hdmitx cfg \n");
    }
    return 0;
}
SS_RTOS_CLI_CMD(hdmitx,
        g_szHdmitxTestAHelpTxt,
        g_szHdmitxTestAUsageTxt,
        DrvHdmitxCliParser);
