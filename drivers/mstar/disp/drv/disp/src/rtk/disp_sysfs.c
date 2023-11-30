/*
* disp_sysfs.c- Sigmastar
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

#define __DISP_SYSFS_C__

//-------------------------------------------------------------------------------------------------
// Include files
//-------------------------------------------------------------------------------------------------

#include "drv_disp_os.h"
#include "disp_sysfs.h"
#include "disp_debug.h"
#include "mhal_disp.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "drv_disp_if.h"
#include "hal_disp_irq.h"
#include "hal_disp_util.h"
#include "hal_disp_dma.h"

#if DISP_STATISTIC_EN

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------
static void _DrvDispSysFsPrintBuf(u8 *buf)
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
static void _DrvDispSysFsShiftCmdBuffer(DrvDispOsStrConfig_t *pstStrCfg)
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
void DrvDispSysfsGetInfo(DrvDispOsStrConfig_t *pstStrCfg)
{
    u32 Reg;
    u32 u32idx;
    u32 u32RegVal;
    u8 *str = NULL;
    
    if(DrvDispOsStrCmp(pstStrCfg->argv[0], "dbgmg") == 0)
    {
        if(pstStrCfg->argc == 2)
        {
            _DrvDispSysFsShiftCmdBuffer(pstStrCfg);
            DrvDispDebugSetDbgmgFlag(pstStrCfg);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "riu_w") == 0)
    {
        if(pstStrCfg->argc >= 4)
        {
            DrvDispOsStrToL(pstStrCfg->argv[1], 16, (u32 *)&Reg);
            DrvDispOsStrToL(pstStrCfg->argv[2], 16, (u32 *)&u32idx);
            DrvDispOsStrToL(pstStrCfg->argv[3], 16, (u32 *)&u32RegVal);
            Reg = (Reg << 8);
            HalDispUtilityW2BYTEMSKDirect( Reg|(u32idx*2), (u16)u32RegVal,0xFFFF, NULL);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "cfg") == 0)
    {
        str = DrvDispOsMemAlloc(PAGE_SIZE);
        if(str)
        {
            DrvDispDebugCfgShow(str);
            _DrvDispSysFsPrintBuf((u8 *)str);
            DrvDispOsMemRelease(str);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "pqbin") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvDispOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvDispDebugGetPqBinName(str);
                _DrvDispSysFsPrintBuf((u8 *)str);
                DrvDispOsMemRelease(str);
            }
        }
        else
        {
            _DrvDispSysFsShiftCmdBuffer(pstStrCfg);
            DrvDispDebugSetPq(pstStrCfg);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "turndrv") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvDispOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvDispDebugGetTurnDrvInfo(str);
                _DrvDispSysFsPrintBuf((u8 *)str);
                DrvDispOsMemRelease(str);
            }
        }
        else
        {
            _DrvDispSysFsShiftCmdBuffer(pstStrCfg);
            DrvDispDebugSetTurnDrv(pstStrCfg);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "ints") == 0)
    {
        str = DrvDispOsMemAlloc(PAGE_SIZE);
        if(str)
        {
            DrvDispDebugIntsShow((u8 *)str);
            _DrvDispSysFsPrintBuf((u8 *)str);
            DrvDispOsMemRelease(str);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "func") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvDispOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvDispDebugFuncShow(str);
                _DrvDispSysFsPrintBuf((u8 *)str);
                DrvDispOsMemRelease(str);
            }
        }
        else
        {
            _DrvDispSysFsShiftCmdBuffer(pstStrCfg);
            DrvDispDebugSetFunc(pstStrCfg);
        }
    }
    else if(DrvDispOsStrCmp(pstStrCfg->argv[0], "clk") == 0)
    {
        if(pstStrCfg->argc == 1)
        {
            str = DrvDispOsMemAlloc(PAGE_SIZE);
            if(str)
            {
                DrvDispDebugGetClk(str);
                _DrvDispSysFsPrintBuf((u8 *)str);
                DrvDispOsMemRelease(str);
            }
        }
        else
        {
            _DrvDispSysFsShiftCmdBuffer(pstStrCfg);
            DrvDispDebugSetClk(pstStrCfg);
        }
    }

}
#endif
void DrvDispSysfsInit(void *pvDevice)
{
    // NOP
}

#undef __DISP_SYSFS_C__

