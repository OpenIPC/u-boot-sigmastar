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
#include "drv_disp_os.h"
#include "disp_debug.h"
#include "cam_fs_wrapper.h"
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
    return CamOsMemAlloc(u32Size);
}

void DrvDispOsMemRelease(void *pPtr)
{
    CamOsMemRelease(pPtr);
}

void DrvDispOsMsSleep(u32 u32Msec)
{
    CamOsMsSleep(u32Msec);
}

void DrvDispOsUsSleep(u32 u32Usec)
{
    CamOsUsSleep(u32Usec);
}

void DrvDispOsMsDelay(u32 u32Msec)
{
    CamOsMsDelay(u32Msec);
}

void DrvDispOsUsDelay(u32 u32Usec)
{
    CamOsUsDelay(u32Usec);
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
    CamOsThreadAttrb_t stThreadAttrCfg = {0};
    u8                 bRet            = TRUE;

    stThreadAttrCfg.nPriority  = 50;
    stThreadAttrCfg.nStackSize = 0;
    stThreadAttrCfg.szName     = (char *)pTaskName;

    if (CamOsThreadCreate(&pstTaskCfg->stThreadCfg, &stThreadAttrCfg, pTaskEntry, pDataPtr) == CAM_OS_OK)
    {
        if (bAuotStart)
        {
            if (CamOsThreadWakeUp(pstTaskCfg->stThreadCfg) == CAM_OS_OK)
            {
                bRet              = TRUE;
                pstTaskCfg->s32Id = 1;
            }
            else
            {
                bRet              = FALSE;
                pstTaskCfg->s32Id = -1;
                DISP_ERR("%s %d, CamOsThreadWakeUp Fail\n", __FUNCTION__, __LINE__);
            }
        }
    }
    else
    {
        pstTaskCfg->s32Id = -1;
        bRet              = FALSE;
        DISP_ERR("%s %d, CamOsThreadCreate Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvDispOsDestroyTask(DrvDispOsTaskConfig_t *pstTaskCfg)
{
    if (CamOsThreadStop(pstTaskCfg->stThreadCfg) == CAM_OS_OK)
    {
        pstTaskCfg->s32Id = -1;
    }
    else
    {
        DISP_ERR("%s %d, CamOsThreadStop Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
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
    CamOsTimespec_t ts;
    CamOsGetTimeOfDay(&ts);
    return ts.nSec * 1000 + ts.nNanoSec / 1000000;
}

u64 DrvDispOsGetSystemTimeStamp(void)
{
    u64             u64TimeStamp = 0;
    CamOsTimespec_t stRes;

    CamOsGetMonotonicTime(&stRes);
    u64TimeStamp = (u64)stRes.nSec * 1000000ULL + (u64)(stRes.nNanoSec / 1000LL);
    return u64TimeStamp;
}

s32 DrvDispOsSplit(u8 **arr, u8 *str, u8 *del)
{
    u8 *p8cur   = str;
    u8 *  p8token = NULL;
    u32   u32Cnt  = 0;

    p8token = (u8 *)strsep((char **)&p8cur, (const char *)del);
    while (p8token)
    {
        if (u32Cnt >= DISP_OS_STRING_MAX)
        {
            DISP_ERR("%s %d, Out of Size\n", __FUNCTION__, __LINE__);
            break;
        }
        arr[u32Cnt] = p8token;
        p8token     = (u8 *)strsep((char **)&p8cur,(const char *) del);
        u32Cnt++;
    }
    return u32Cnt;
}

s32 DrvDispOsSetFileSeek(DrvDispOsFileConfig_t *pFileCfg, u32 u32Flag)
{
    return CamFsSeek(pFileCfg->pFile, 0, u32Flag);
}

u32 DrvDispOsGetFileSize(DrvDispOsFileConfig_t *pFileCfg)
{
    u32 u32Filesize;

    u32Filesize = CamFsSeek(pFileCfg->pFile, 0, SEEK_END);
    CamFsSeek(pFileCfg->pFile, 0, SEEK_SET);
    return u32Filesize;
}

s32 DrvDispOsOpenFile(u8 *path, s32 flag, s32 mode, DrvDispOsFileConfig_t *pstFileCfg)
{
    CamFsRet_e bRet = 1;

    pstFileCfg->pFile = NULL;
    bRet              = CamFsOpen(&pstFileCfg->pFile, (const char *)path, (u32)flag,(u32) mode);
    return bRet;
}

s32 DrvDispOsWriteFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 writelen)
{
    s32 ret;

    ret = CamFsWrite(pFileCfg->pFile, buf, writelen);
    return ret;
}

s32 DrvDispOsReadFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 readlen)
{
    s32 ret;

    ret = CamFsRead(pFileCfg->pFile, buf, readlen);
    return ret;
}

s32 DrvDispOsCloseFile(DrvDispOsFileConfig_t *pFileCfg)
{
    CamFsClose(pFileCfg->pFile);
    return 0;
}

void DrvDispOsSplitByToken(DrvDispOsStrConfig_t *pstStrCfg, u8 *pBuf, u8 *token)
{
    pstStrCfg->argc = DrvDispOsSplit(pstStrCfg->argv, pBuf, token);
}

void DrvDispOsParsingTxt(u8 *pFileName, DrvDispOsTextConfig_t *pstTxtCfg)
{
    DrvDispOsFileConfig_t      stReadFileCfg;
    u8 *                       pBuf;
    u32                        u32FileSize;
    u8                         filedel[] = "\n";
    u8                         linedel[] = " ";
    u32                        i, j, len;
    u8                         a8LinePattern[1024];
    DrvDispOsStrConfig_t       stLineStrCfg;
    DrvDispOsStrConfig_t       stFileStrCfg;
    DrvDispOsTextItemConfig_t *pstTxtItemCfg;
    u32                        u32LineCnt;

    DrvDispOsOpenFile(pFileName, O_RDONLY, 0, &stReadFileCfg);

    if (stReadFileCfg.pFile)
    {
        u32FileSize = DrvDispOsGetFileSize(&stReadFileCfg);

        pBuf = DrvDispOsMemAlloc(u32FileSize + 1);

        if (pBuf)
        {
            DrvDispOsReadFile(&stReadFileCfg, pBuf, u32FileSize);
            DrvDispOsCloseFile(&stReadFileCfg);
            pBuf[u32FileSize] = '\0';

            DrvDispOsSplitByToken(&stFileStrCfg, pBuf, filedel);
            u32LineCnt = stFileStrCfg.argc;

            pstTxtCfg->pstItem = DrvDispOsMemAlloc(sizeof(DrvDispOsTextItemConfig_t) * u32LineCnt);
            pstTxtCfg->u32Size = 0;

            if (pstTxtCfg->pstItem)
            {
                for (i = 0; i < u32LineCnt; i++)
                {
                    if (i >= DISP_OS_STRING_MAX)
                    {
                        DISP_ERR("%s %d, Out Of Max String Size\n", __FUNCTION__, __LINE__);
                        break;
                    }

                    if (stFileStrCfg.argv[i] == NULL)
                    {
                        DISP_ERR("NULL Point, %d\n", i);
                        continue;
                    }

                    memset(a8LinePattern, 0, 1024);
                    len = strlen((char *)stFileStrCfg.argv[i]);
                    memcpy(a8LinePattern, stFileStrCfg.argv[i], len);
                    DrvDispOsSplitByToken(&stLineStrCfg, a8LinePattern, linedel);

                    len = strlen((char *)stLineStrCfg.argv[0]);
                    if ((len == 0) || (stLineStrCfg.argv[0][0] == '#'))
                    {
                        continue;
                    }
                    pstTxtItemCfg = &pstTxtCfg->pstItem[pstTxtCfg->u32Size++];
                    len           = strlen((char *)stLineStrCfg.argv[0]);
                    memcpy(pstTxtItemCfg->pu8Name, stLineStrCfg.argv[0], len);
                    pstTxtItemCfg->pu8Name[len] = '\0';

                    for (j = 1; j < stLineStrCfg.argc; j++)
                    {
                        if (j >= DISP_OS_STRING_MAX)
                        {
                            DISP_ERR("%s %d, Out Of Max String Size\n", __FUNCTION__, __LINE__);
                            break;
                        }

                        if (stLineStrCfg.argv[j][0] == '#')
                        {
                            break;
                        }

                        len = strlen((char *)stLineStrCfg.argv[j]);

                        if (len != 0 && (strcmp((char *)stLineStrCfg.argv[j], "=") != 0))
                        {
                            memcpy(pstTxtItemCfg->pu8Value, stLineStrCfg.argv[j], len);
                            pstTxtItemCfg->pu8Value[len] = '\0';
                        }
                    }
                    // printk("line:%d, %s = %s\n", i, pstTxtItemCfg->pu8Name, pstTxtItemCfg->pu8Value);
                }
            }

            CamOsMemRelease(pBuf);
        }
    }
}

u8 DrvDispOsStrCmp(u8 *str,const char *dstr)
{
    u8 ret;
    ret = strcmp((char *)str, dstr);
    return ret;
}

u8 DrvDispOsCreateMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    u8 bRet = TRUE;

    if (CamOsMutexInit(&pstMutexCfg->stMutxCfg) == CAM_OS_OK)
    {
        pstMutexCfg->s32Id = 1;
        bRet               = TRUE;
    }
    else
    {
        pstMutexCfg->s32Id = -1;

        DISP_ERR("%s %d, MutexCreate Fail\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvDispOsDestroyMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    u8 bRet;
    if (CamOsMutexDestroy(&pstMutexCfg->stMutxCfg) == CAM_OS_OK)
    {
        pstMutexCfg->s32Id = -1;
        bRet               = TRUE;
    }
    else
    {
        DISP_ERR("%s %d, Mutex Destroy Fail\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvDispOsObtainMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexLock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        DISP_ERR("%s %d, Mutex Lock Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}

u8 DrvDispOsReleaseMutex(DrvDispOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexUnlock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        DISP_ERR("%s %d, Mutex UnLock Fail\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}
u32 DrvDispOsStrToL(u8 *str, u16 u16Base, u32 *pValue)
{
    *pValue = CamOsStrtol((const char *)str, NULL, u16Base);
    return 1;
}
u32 DrvDispOsMathDivU64(u64 nDividend, u64 nDivisor, u64 *pRemainder)
{
    return CamOsMathDivU64(nDividend, nDivisor, pRemainder);
}

