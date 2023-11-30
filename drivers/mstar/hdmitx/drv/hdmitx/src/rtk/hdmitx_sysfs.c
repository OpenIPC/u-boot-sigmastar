/*
* hdmitx_sysfs.c- Sigmastar
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

#define _HDMITX_SYSFS_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "mhal_hdmitx_datatype.h"
#include "mhal_hdmitx.h"
#include "drv_hdmitx_if.h"
#include "hal_hdmitx_st.h"
#include "drv_hdmitx_ctx.h"
#include "hal_hdmitx_if.h"
#include "regHDMITx.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _DrvHdmitxSysFsPrintBuf(u8 *buf)
{
    char *cur = (char *)buf;
    u8 *token = NULL;
    u8 del[] = "\n";
    do
    {
        token = (u8 *)strsep(&(cur),(const char *) del);
        CamOsPrintf("%s\n",token);
    }
    while(token);
}
static void _DrvHdmitxSysFsShiftCmdBuffer(DrvHdmitxOsStrConfig_t *pstStrCfg)
{
    u16 u16Idx = 0;

    if(pstStrCfg->argc >= 2)
    {
        pstStrCfg->argc = pstStrCfg->argc -1;
        for(u16Idx = 0;u16Idx < pstStrCfg->argc;u16Idx++)
        {
            pstStrCfg->argv[u16Idx] = pstStrCfg->argv[u16Idx+1];
        }
    }
}
void DrvHdmitxSysfsGetInfo(DrvHdmitxOsStrConfig_t *pstStrCfg)
{
    u32 Reg;
    u32 u32idx;
    u32 u32RegVal;
    u8 *str = NULL;
    
    if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "dbgmg") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugDbgmgShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
        if(pstStrCfg->argc == 2)
        {
            _DrvHdmitxSysFsShiftCmdBuffer(pstStrCfg);
            DrvHdmitxDebugDbgmgStore(pstStrCfg);
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "riu_w") == 0)
    {
        if(pstStrCfg->argc >= 4)
        {
            DrvHdmitxOsStrToL(pstStrCfg->argv[1], 16, (u32 *)&Reg);
            DrvHdmitxOsStrToL(pstStrCfg->argv[2], 16, (u32 *)&u32idx);
            DrvHdmitxOsStrToL(pstStrCfg->argv[3], 16, (u32 *)&u32RegVal);
            Reg = (Reg << 8);
            HalHdmitxUtilityW2BYTEMSK( Reg|(u32idx*2), (u16)u32RegVal,0xFFFF);
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "forcecfg") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugForceCfgShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
        else
        {
            _DrvHdmitxSysFsShiftCmdBuffer(pstStrCfg);
            DrvHdmitxDebugForceCfgStore(pstStrCfg);
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "cfg") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugCfgShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "edid") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugEdidShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "ptgen") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugPtGenShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
        else
        {
            _DrvHdmitxSysFsShiftCmdBuffer(pstStrCfg);
            DrvHdmitxDebugPtGenStore(pstStrCfg);
        }
    }
    else if(DrvHdmitxOsStrCmp(pstStrCfg->argv[0], "clk") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvHdmitxOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvHdmitxDebugClkShow(str);
                _DrvHdmitxSysFsPrintBuf((u8 *)str);
                DrvHdmitxOsMemRelease(str);
            }
        }
        else
        {
            _DrvHdmitxSysFsShiftCmdBuffer(pstStrCfg);
            DrvHdmitxDebugClkStore(pstStrCfg);
        }
    }

}
void DrvHdmitxSysfsInit(void *device)
{
}
