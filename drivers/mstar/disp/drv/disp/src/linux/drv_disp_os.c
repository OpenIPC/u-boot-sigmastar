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
#include "cam_sysfs.h"
#include "cam_clkgen.h"
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"
#include "cam_fs_wrapper.h"

#include "imi_heap.h"
#include "mdrv_padmux.h"
#include "hal_disp_chip.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
struct device_node *g_pstDispDeviceNode = NULL;

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
    s32 s32Ret = 0;
    // s32Ret = ImiHeapMalloc(0, u32Size, ppAddr);
    return s32Ret;
}

void DrvDispOsImiHeapFree(void *pAddr)
{
    // ImiHeapFree(0, pAddr);
}

u8 DrvDispOsPadMuxActive(void)
{
    u8 bRet = mdrv_padmux_active() ? 1 : 0;
    return bRet;
}

u8 DrvDispOsCreateTask(DrvDispOsTaskConfig_t *pstTaskCfg, DISP_TASK_ENTRY_CB pTaskEntry, void *pDataPtr, u8 *pTaskName,
                       u8 bAuotStart)
{
    CamOsThreadAttrb_t stThreadAttrCfg = {0};
    u8                 bRet            = TRUE;

    stThreadAttrCfg.nPriority  = 50;
    stThreadAttrCfg.nStackSize = 0;
    stThreadAttrCfg.szName     = pTaskName;

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
    g_pstDispDeviceNode = ((struct platform_device *)pPlatFormDev)->dev.of_node;

    if (!g_pstDispDeviceNode)
    {
        return FALSE;
    }
    return TRUE;
}
#ifdef CONFIG_CAM_CLK
#include "drv_camclk_Api.h"
void **g_pvDispclk        = NULL;
u32    g_u32DispParentCnt = 1;

u8 DrvDispOsClkRegister(struct device_node *device_node)
{
    u32 u32clknum;
    u32 u32DispClk;
    u8  a8str[16];

    if (of_find_property(device_node, "camclk", &g_u32DispParentCnt))
    {
        g_u32DispParentCnt /= sizeof(u32);
        // printk( "[%s] Number : %d\n", __func__, num_parents);
        if (g_u32DispParentCnt < 0)
        {
            printk("[%s] Fail to get parent count! Error Number : %d\n", __func__, g_u32DispParentCnt);
            return 0;
        }
        g_pvDispclk = CamOsMemAlloc((sizeof(void *) * g_u32DispParentCnt));
        if (!g_pvDispclk)
        {
            return 0;
        }
        for (u32clknum = 0; u32clknum < g_u32DispParentCnt; u32clknum++)
        {
            u32DispClk = 0;
            msys_of_property_read_u32_index(device_node, "camclk", u32clknum, &(u32DispClk));
            if (!u32DispClk)
            {
                printk("[%s] Fail to get clk!\n", __func__);
            }
            else
            {
                CamOsSnprintf(a8str, 16, "Disp_%d ", u32clknum);
                CamClkRegister(a8str, u32DispClk, &(g_pvDispclk[u32clknum]));
            }
        }
    }
    else
    {
        printk("[%s] W/O Camclk \n", __func__);
    }
    return 1;
}
u8 DrvDispOsClkUnregister(void)
{
    u32 u32clknum;

    for (u32clknum = 0; u32clknum < g_u32DispParentCnt; u32clknum++)
    {
        if (g_pvDispclk[u32clknum])
        {
            printk(KERN_DEBUG "[%s] %px\n", __func__, g_pvDispclk[u32clknum]);
            CamClkUnregister(g_pvDispclk[u32clknum]);
            g_pvDispclk[u32clknum] = NULL;
        }
    }
    CamOsMemRelease(g_pvDispclk);

    return 1;
}
#endif
u8 DrvDispOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    u8   bRet = TRUE;
    u32  u32Clkrate;
    u32 *p32ClkRatebuf = (u32 *)pClkRate;
    u8 * p8Clkenbuf    = (u8 *)pClkEn;
#ifdef CONFIG_CAM_CLK
    u32                  u32clknum = 0;
    CAMCLK_Set_Attribute stSetCfg;
    CAMCLK_Get_Attribute stGetCfg;

    if (g_pstDispDeviceNode && pClkRate && u32ClkRateSize == E_HAL_DISP_CLK_NUM)
    {
        DrvDispOsClkRegister(g_pstDispDeviceNode);
        for (u32clknum = 0; u32clknum < g_u32DispParentCnt; u32clknum++)
        {
            if (g_pvDispclk[u32clknum])
            {
                u32Clkrate = p32ClkRatebuf[u32clknum];
                if (HAL_DISP_CLK_GET_MUX_ATTR(u32clknum) == 1)
                {
                    CamClkAttrGet(g_pvDispclk[u32clknum], &stGetCfg);
                    CAMCLK_SETPARENT(stSetCfg, stGetCfg.u32Parent[u32Clkrate]);
                    CamClkAttrSet(g_pvDispclk[u32clknum], &stSetCfg);
                }
                else
                {
                    CAMCLK_SETRATE_ROUNDUP(stSetCfg, u32Clkrate);
                    CamClkAttrSet(g_pvDispclk[u32clknum], &stSetCfg);
                }
                if (p8Clkenbuf[u32clknum] == 0)
                {
                    CamClkSetOnOff(g_pvDispclk[u32clknum], 1);
                }
            }
        }
    }
#else
    u32            u32Numparents, u32Idx;
    struct clk **  pstDispclks;
    struct clk_hw *pstHwparent;

    if (g_pstDispDeviceNode && pClkRate && u32ClkRateSize == E_HAL_DISP_CLK_NUM)
    {
        u32Numparents = CamOfClkGetParentCount(g_pstDispDeviceNode);

        DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d u32Numparents:%d \n", __FUNCTION__, __LINE__, u32Numparents);
        for (u32Idx = 0; u32Idx < u32ClkRateSize; u32Idx++)
        {
            DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, CLK_%d En:%d, Rate:%d\n", __FUNCTION__, __LINE__, u32Idx,
                     p8Clkenbuf[u32Idx], p32ClkRatebuf[u32Idx]);
        }

        if (u32Numparents == E_HAL_DISP_CLK_NUM)
        {
            pstDispclks = CamOsMemAlloc((sizeof(struct clk *) * u32Numparents));

            if (pstDispclks == NULL)
            {
                DISP_ERR("%s %d Alloc pstDispclks is NULL\n", __FUNCTION__, __LINE__);
                return 0;
            }

            for (u32Idx = 0; u32Idx < u32Numparents; u32Idx++)
            {
                pstDispclks[u32Idx] = of_clk_get(g_pstDispDeviceNode, u32Idx);
                if (IS_ERR(pstDispclks[u32Idx]))
                {
                    DISP_ERR("%s %d, Fail to get [Disp] %s\n", __FUNCTION__, __LINE__,
                             CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx));
                    CamOsMemRelease(pstDispclks);
                    return 0;
                }

                if (HAL_DISP_CLK_GET_MUX_ATTR(u32Idx) == 1)
                {
                    u32Clkrate  = p32ClkRatebuf[u32Idx];
                    pstHwparent = CamClkGetParentByIndex(__CamClkGetHw(pstDispclks[u32Idx]), u32Clkrate);
                    CamClkSetParent(pstDispclks[u32Idx], pstHwparent->clk);
                }
                else
                {
                    u32Clkrate = CamClkRoundRate(pstDispclks[u32Idx], p32ClkRatebuf[u32Idx]);
                    CamClkSetRate(pstDispclks[u32Idx], u32Clkrate + 1000000);
                }

                if (!p8Clkenbuf[u32Idx])
                {
                    CamClkPrepareEnable(pstDispclks[u32Idx]);
                    DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d Set %s:enable \n", __FUNCTION__, __LINE__, CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx),p8Clkenbuf[u32Idx]);
                }
                DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, [Disp] %d-%d %15s En:%d, Rate:%d\n", __FUNCTION__, __LINE__,
                         u32Numparents, u32Idx + 1, CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx),
                         p8Clkenbuf[u32Idx], u32Clkrate);

                clk_put(pstDispclks[u32Idx]);
            }

            CamOsMemRelease(pstDispclks);
        }
        else
        {
            bRet = FALSE;
            DISP_ERR("%s %d, u32Numparents %d != %d\n", __FUNCTION__, __LINE__, u32Numparents, E_HAL_DISP_CLK_NUM);
        }
    }
    else
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Param Null, DeviceNode:%x, ClkRate:%x, ClkSize:%d\n", __FUNCTION__, __LINE__,
                 g_pstDispDeviceNode, pClkRate, u32ClkRateSize);
    }
#endif
    return bRet;
}

u8 DrvDispOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    u8  bRet       = TRUE;
    u8 *p8Clkenbuf = (u8 *)pClkEn;
#ifdef CONFIG_CAM_CLK
    u32 u32clknum = 0;

    for (u32clknum = 0; u32clknum < g_u32DispParentCnt; u32clknum++)
    {
        if (g_pvDispclk[u32clknum] && p8Clkenbuf[u32clknum])
        {
            CamClkSetOnOff(g_pvDispclk[u32clknum], 0);
        }
    }
    DrvDispOsClkUnregister();
#else
    u32          u32Numparents, u32Idx;
    struct clk **pstDispclks;

    if (g_pstDispDeviceNode)
    {
        u32Numparents = CamOfClkGetParentCount(g_pstDispDeviceNode);

        DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d u32Numparents:%d\n", __FUNCTION__, __LINE__, u32Numparents);
        if (u32Numparents == E_HAL_DISP_CLK_NUM)
        {
            pstDispclks = CamOsMemAlloc((sizeof(struct clk *) * u32Numparents));

            if (pstDispclks == NULL)
            {
                DISP_ERR("%s %d Alloc pstDispclks is NULL\n", __FUNCTION__, __LINE__);
                return 0;
            }

            for (u32Idx = 0; u32Idx < u32Numparents; u32Idx++)
            {
                pstDispclks[u32Idx] = of_clk_get(g_pstDispDeviceNode, u32Idx);
                if (!IS_ERR(pstDispclks[u32Idx]))
                {
                    if (p8Clkenbuf[u32Idx])
                    {
                        DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d Set %s:disable \n", __FUNCTION__, __LINE__, CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx));
                        CamClkDisableUnprepare(pstDispclks[u32Idx]);
                    }

                    DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, [Disp] %d-%d, %15s En:%d\n", __FUNCTION__, __LINE__,
                             u32Numparents, u32Idx + 1, CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx),
                             p8Clkenbuf[u32Idx]);
                }
                else
                {
                    DISP_ERR("%s %d, Fail to get [Disp] %s\n", __FUNCTION__, __LINE__,
                             CamOfClkGetParentName(g_pstDispDeviceNode, u32Idx));
                    CamOsMemRelease(pstDispclks);
                    return 0;
                }
                clk_put(pstDispclks[u32Idx]);
            }
            CamOsMemRelease(pstDispclks);
        }
        else
        {
            bRet = FALSE;
        }
    }
    else
    {
        bRet = FALSE;
    }
#endif
    return bRet;
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
    char *p8cur   = str;
    u8 *  p8token = NULL;
    u32   u32Cnt  = 0;

    p8token = strsep(&p8cur, del);
    while (p8token)
    {
        if (u32Cnt >= DISP_OS_STRING_MAX)
        {
            DISP_ERR("%s %d, Out of Size\n", __FUNCTION__, __LINE__);
            break;
        }
        arr[u32Cnt] = p8token;
        p8token     = strsep(&p8cur, del);
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

CamFsRet_e DrvDispOsOpenFile(u8 *path, s32 flag, s32 mode, DrvDispOsFileConfig_t *pstFileCfg)
{
    CamFsRet_e bRet = 1;

    pstFileCfg->pFile = NULL;
    bRet              = CamFsOpen(&pstFileCfg->pFile, path, flag, mode);
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
                    len = strlen(stFileStrCfg.argv[i]);
                    memcpy(a8LinePattern, stFileStrCfg.argv[i], len);
                    DrvDispOsSplitByToken(&stLineStrCfg, a8LinePattern, linedel);

                    len = strlen(stLineStrCfg.argv[0]);
                    if ((len == 0) || (stLineStrCfg.argv[0][0] == '#'))
                    {
                        continue;
                    }
                    pstTxtItemCfg = &pstTxtCfg->pstItem[pstTxtCfg->u32Size++];
                    len           = strlen(stLineStrCfg.argv[0]);
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

                        len = strlen(stLineStrCfg.argv[j]);

                        if (len != 0 && (strcmp(stLineStrCfg.argv[j], "=") != 0))
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
    ret = strcmp(str, dstr);
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
    *pValue = CamOsStrtol(str, NULL, u16Base);
    return 1;
}
u32 DrvDispOsMathDivU64(u64 nDividend, u64 nDivisor, u64 *pRemainder)
{
    return CamOsMathDivU64(nDividend, nDivisor, pRemainder);
}
EXPORT_SYMBOL(MHAL_DISP_DeviceSetBackGroundColor);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerUnBind);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerCreateInstance);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerDestoryInstance);
EXPORT_SYMBOL(MHAL_DISP_DeviceCreateInstance);
EXPORT_SYMBOL(MHAL_DISP_InputPortSetAttr);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetHdmiParam);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerBind);
EXPORT_SYMBOL(MHAL_DISP_DeviceEnable);
EXPORT_SYMBOL(MHAL_DISP_InputPortShow);
EXPORT_SYMBOL(MHAL_DISP_InputPortAttrEnd);
EXPORT_SYMBOL(MHAL_DISP_ClearDevInterrupt);
EXPORT_SYMBOL(MHAL_DISP_ClearLcdInterrupt);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerEnable);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetOutputTiming);
EXPORT_SYMBOL(MHAL_DISP_DeviceDestroyInstance);
EXPORT_SYMBOL(MHAL_DISP_InputPortCreateInstance);
EXPORT_SYMBOL(MHAL_DISP_InputPortEnable);
EXPORT_SYMBOL(MHAL_DISP_InitPanelConfig);
EXPORT_SYMBOL(MHAL_DISP_GetDevIrq);
EXPORT_SYMBOL(MHAL_DISP_GetDevIrqFlag);
EXPORT_SYMBOL(MHAL_DISP_GetLcdIrq);
EXPORT_SYMBOL(MHAL_DISP_GetLcdIrqFlag);
EXPORT_SYMBOL(MHAL_DISP_InitMmapConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceAttach);
EXPORT_SYMBOL(MHAL_DISP_InputPortAttrBegin);
EXPORT_SYMBOL(MHAL_DISP_InputPortFlip);
EXPORT_SYMBOL(MHAL_DISP_DeviceDetach);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetCvbsParam);
EXPORT_SYMBOL(MHAL_DISP_InputPortDestroyInstance);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerCheckBufferFired);
EXPORT_SYMBOL(MHAL_DISP_InputPortHide);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetVgaParam);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerSetAttr);
EXPORT_SYMBOL(MHAL_DISP_DeviceAddOutInterface);
EXPORT_SYMBOL(MHAL_DISP_EnableDevIrq);
EXPORT_SYMBOL(MHAL_DISP_EnableLcdIrq);
EXPORT_SYMBOL(MHAL_DISP_DbgLevel);
EXPORT_SYMBOL(MHAL_DISP_InputPortRotate);
EXPORT_SYMBOL(MHAL_DISP_InputPortSetCropAttr);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetColorTempeture);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetLcdParam);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetGammaParam);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetTimeZone);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetTimeZoneConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetDisplayInfo);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetInstance);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetPqConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceSetConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetHdmiParam);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetVgaParam);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetLcdParam);
EXPORT_SYMBOL(MHAL_DISP_SetRegAccessConfig);
EXPORT_SYMBOL(MHAL_DISP_SetRegFlipConfig);
EXPORT_SYMBOL(MHAL_DISP_SetRegWaitDoneConfig);
EXPORT_SYMBOL(MHAL_DISP_SetCmdqInterface);
EXPORT_SYMBOL(MHAL_DISP_DmaCreateInstance);
EXPORT_SYMBOL(MHAL_DISP_DmaDestoryInstance);
EXPORT_SYMBOL(MHAL_DISP_DmaBind);
EXPORT_SYMBOL(MHAL_DISP_DmaUnBind);
EXPORT_SYMBOL(MHAL_DISP_DmaSetAttr);
EXPORT_SYMBOL(MHAL_DISP_DmaSetBufferAttr);
EXPORT_SYMBOL(MHAL_DISP_EnableDmaIrq);
EXPORT_SYMBOL(MHAL_DISP_ClearDmaInterrupt);
EXPORT_SYMBOL(MHAL_DISP_GetDmaIrqFlag);
EXPORT_SYMBOL(MHAL_DISP_InputPortSetRingBuffAttr);
EXPORT_SYMBOL(MHAL_DISP_GetDmaIrq);
EXPORT_SYMBOL(MHAL_DISP_GetDevIrqById);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetHwCount);
EXPORT_SYMBOL(MHAL_DISP_InputPortSetImiAddr);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetCapabilityConfig);
EXPORT_SYMBOL(MHAL_DISP_VideoLayerGetCapabilityConfig);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetIntefaceCapabilityConfig);
EXPORT_SYMBOL(MHAL_DISP_DmaGetCapabilityConfig);
EXPORT_SYMBOL(MHAL_DISP_DmaGetHwCount);
EXPORT_SYMBOL(MHAL_DISP_DeviceGetCvbsParam);
EXPORT_SYMBOL(DrvDispOsStrCmp);
EXPORT_SYMBOL(DrvDispOsStrToL);
EXPORT_SYMBOL(DrvDispOsMemRelease);
EXPORT_SYMBOL(DrvDispOsMemAlloc);
EXPORT_SYMBOL(DrvDispOsCreateTask);
EXPORT_SYMBOL(DrvDispOsDestroyTask);
EXPORT_SYMBOL(DrvDispOsMsSleep);
EXPORT_SYMBOL(DrvDispOsGetSystemTime);
EXPORT_SYMBOL(DrvDispOsGetSystemTimeStamp);
EXPORT_SYMBOL(DrvDispOsGetFileSize);
EXPORT_SYMBOL(DrvDispOsOpenFile);
EXPORT_SYMBOL(DrvDispOsWriteFile);
EXPORT_SYMBOL(DrvDispOsReadFile);
EXPORT_SYMBOL(DrvDispOsCloseFile);
EXPORT_SYMBOL(DrvDispOsSetFileSeek);
