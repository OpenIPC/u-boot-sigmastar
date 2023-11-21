/*
* drv_disp_os.c- Sigmastar
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

#define _DRV_DISP_OS_C_
#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

#include "drv_disp_os.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
void *DrvDispOsMemAlloc(u32 u32Size)
{
    void *pBuf = NULL;
    pBuf       = malloc(u32Size);
    if (pBuf)
    {
        memset(pBuf, 0, u32Size);
    }
    return pBuf;
}

void DrvDispOsMemRelease(void *pPtr)
{
    free(pPtr);
}

void DrvDispOsMsSleep(u32 u32Msec)
{
    mdelay(u32Msec);
}

void DrvDispOsUsSleep(u32 u32Usec)
{
    udelay(u32Usec);
}

void DrvDispOsMsDelay(u32 u32Msec)
{
    mdelay(u32Msec);
}

void DrvDispOsUsDelay(u32 u32Usec)
{
    udelay(u32Usec);
}

s32 DrvDispOsImiHeapAlloc(u32 u32Size, void **ppAddr)
{
    return 0;
}

void DrvDispOsImiHeapFree(void *pAddr) {}

u8 DrvDispOsPadMuxActive(void)
{
    return 0;
}

u8 DrvDispOsCreateTask(DrvDispOsTaskConfig_t *pstTaskCfg, DISP_TASK_ENTRY_CB pTaskEntry, void *pDataPtr, u8 *pTaskName,
                       u8 bAuotStart)
{
    return 0;
}

u8 DrvDispOsDestroyTask(DrvDispOsTaskConfig_t *pstTaskCfg)
{
    return 0;
}

u8 DrvDispOsSetDeviceNode(void *pPlatFormDev)
{
    return 1;
}

u8 DrvDispOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    return 0;
}

u8 DrvDispOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    return 0;
}

u32 DrvDispOsGetSystemTime(void)
{
    return 0;
}

u64 DrvDispOsGetSystemTimeStamp(void)
{
    return 0;
}

s32 DrvDispOsSplit(u8 **arr, u8 *str, u8 *del)
{
    return 0;
}

u32 DrvDispOsGetFileSize(DrvDispOsFileConfig_t *pFileCfg)
{
    return 0;
}

s32 DrvDispOsSetFileSeek(DrvDispOsFileConfig_t *pFileCfg, u32 u32Flag)
{
    return 0;
}

s32 DrvDispOsOpenFile(u8 *path, s32 flag, s32 mode, DrvDispOsFileConfig_t *pstFileCfg)
{
    return 0;
}

s32 DrvDispOsWriteFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 writelen)
{
    return 0;
}

s32 DrvDispOsReadFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 readlen)
{
    return 0;
}

s32 DrvDispOsCloseFile(DrvDispOsFileConfig_t *pFileCfg)
{
    return 0;
}

void DrvDispOsSplitByToken(DrvDispOsStrConfig_t *pstStrCfg, u8 *pBuf, u8 *token) {}

void DrvDispOsParsingTxt(u8 *pFileName, DrvDispOsTextConfig_t *pstTxtCfg) {}

u8 DrvDispOsStrCmp(u8 *str,const char *dstr)
{
    u8 ret;
    ret = strcmp((char *)str, dstr);
    return ret;
}

u8 DrvDispOsCreateMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

u8 DrvDispOsDestroyMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

u8 DrvDispOsObtainMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}

u8 DrvDispOsReleaseMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    return 1;
}
u32 DrvDispOsStrToL(u8 *str, u16 u16Base, u32 *pValue)
{
    *pValue = ustrtoul((char *)str, NULL, u16Base);
    return 0;
}
u32 DrvDispOsMathDivU64(u64 nDividend, u64 nDivisor, u64 *pRemainder)
{
    u64 u64idx = 0;
    while(1)
    {
        if(nDivisor * u64idx <nDividend)
        {
            u64idx++;
        }
        else
        {
            *pRemainder = nDividend - (nDivisor * (u64idx-1));
            break;
        }
    }
    return (u32)(u64idx-1);
}

