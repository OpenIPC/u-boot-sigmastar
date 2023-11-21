/*
* drv_hdmitx_os.c- Sigmastar
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

#define _DRV_HDMITX_OS_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "regHDMITx.h"
#include "mdrv_gpio_io.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HDMITX_OS_ID_PREFIX      0x76540000
#define HDMITX_OS_ID_PREFIX_MASK 0xFFFF0000
#define HDMITX_OS_ID_MASK        0x0000FFFF //~HDMITX_OS_ID_PREFIX_MASK

#define HAS_FLAG(flag, bit)   ((flag) & (bit))
#define SET_FLAG(flag, bit)   ((flag) |= (bit))
#define RESET_FLAG(flag, bit) ((flag) &= (~(bit)))

#define EVENT_MUTEX_LOCK()   CamOsSpinLock(&g_eventGroupMutex)
#define EVENT_MUTEX_UNLOCK() CamOsSpinUnlock(&g_eventGroupMutex)

#define EN_EVENT_GROUP 0

#define TIMER_MUTEX_LOCK(x)   CamOsSpinLockIrqSave(x)
#define TIMER_MUTEX_UNLOCK(x) CamOsSpinUnlockIrqRestore(x)

#define DBG_OS_HDMITX(_f)     \
    do                        \
    {                         \
        if (g_bHdmitxOsDbgEn) \
        {                     \
            (_f);             \
        }                     \
    } while (0)

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------
typedef struct DrvHdmitxOsTimerNotifyFunc_s
{
    void (*pfnFunc)(void *nDataAddr);
} DrvHdmitxOsTimerNotifyFunc_t;

typedef struct DrvHdmitxOsMutext_s
{
    u32 u32Dummy;
#if defined(CAM_OS)
    CamOsTsem_t EventGroup;
#endif
} DrvHdmitxOsMutext_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8                                g_bHdmitxOsInit = 0;
DrvHdmitxOsEventGroupInfoConfig_t g_hdmitxOsEventGroupInfo[HDMITX_OS_EVENTGROUP_MAX];
u16 g_u16I2cUsed = 1;

CamOsSpinlock_t g_eventGroupMutex;

bool g_bHdmitxOsDbgEn = 0;

u16 g_u16I2cId     = 0;
u8  g_u8HpdGpioPin = 0;

extern u8 g_u8SwI2cSdaPin;
extern u8 g_u8SwI2cSclPin;
extern u8 g_bSwI2cEn;
extern u8 g_bHpdIrqEn;
//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
static void _DrvHdmitxOsTimerNotify(void *nDataAddr)
{
    DrvHdmitxOsTimerConfig_t *   pTimerCfg = (DrvHdmitxOsTimerConfig_t *)nDataAddr;
    DrvHdmitxOsTimerNotifyFunc_t stNotify;

    CamOsSpinLock(&pTimerCfg->sem_lock);

    if (pTimerCfg->s32Id < 0)
    {
        CamOsSpinUnlock(&pTimerCfg->sem_lock);
        return;
    }

    stNotify.pfnFunc = _DrvHdmitxOsTimerNotify;

    // DBG_OS_HDMITX(CamOsPrintf( PRINT_RED "%s %d: bStop:%d Cycle:%d\n" PRINT_NONE, __FUNCTION__, __LINE__,
    // pTimerCfg->bStop, pTimerCfg->bCycle));

    if (pTimerCfg->bCycle)
    {
        if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, (CamOsTimerCallback)stNotify.pfnFunc)
            != CAM_OS_OK)
        {
            CamOsPrintf(PRINT_RED "%s %d: CamOsTimerAdd Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        }
    }

    CamOsSpinUnlock(&pTimerCfg->sem_lock);

    if (pTimerCfg->pfnFunc)
    {
        pTimerCfg->pfnFunc(0, pTimerCfg->pCtx);
    }
}

u8 DrvHdmitxOsInit(void)
{
    if (g_bHdmitxOsInit == 0)
    {
        CamOsSpinInit(&g_eventGroupMutex);

        g_bHdmitxOsInit = 1;
    }
    else
    {
    }
    return TRUE;
}

void *DrvHdmitxOsMemAlloc(u32 u32Size)
{
    return CamOsMemAlloc(u32Size);
}

void DrvHdmitxOsMemRelease(void *pPtr)
{
    CamOsMemRelease(pPtr);
}

u32 DrvHdmitxOsGetSystemTime(void)
{
    CamOsTimespec_t ts;
    CamOsGetTimeOfDay(&ts);
    return ts.nSec * 1000 + ts.nNanoSec / 1000000;
}
u64 DrvHdmitxOsGetSystemTimeStamp(void)
{
    u64             u64TimeStamp;
    CamOsTimespec_t stRes;
    CamOsGetMonotonicTime(&stRes);
    u64TimeStamp = (u64)stRes.nSec * 1000000ULL + (u64)(stRes.nNanoSec / 1000LL);
    return u64TimeStamp;
}

u32 DrvHdmitxOsTimerDiffTimeFromNow(u32 u32TaskTimer) // unit = ms
{
    return (DrvHdmitxOsGetSystemTime() - u32TaskTimer);
}

u8 DrvHdmitxOsCreateTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    static s32                   s32TimerId = 1;
    u8                           bRet;
    DrvHdmitxOsTimerNotifyFunc_t stNotify;

    stNotify.pfnFunc = _DrvHdmitxOsTimerNotify;
    CamOsSpinInit(&pTimerCfg->sem_lock);

    CamOsSpinLock(&pTimerCfg->sem_lock);

    if (CamOsTimerInit(&pTimerCfg->stTimerCfg) == CAM_OS_OK)
    {
        pTimerCfg->s32Id  = s32TimerId++;
        pTimerCfg->bCycle = 1;
        if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, (CamOsTimerCallback)stNotify.pfnFunc)
            == CAM_OS_OK)
        {
            bRet = TRUE;
        }
        else
        {
            pTimerCfg->s32Id = 0;
            bRet             = FALSE;
            CamOsPrintf(PRINT_RED "%s %d, TimerAdd Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        }
    }
    else
    {
        pTimerCfg->s32Id = -1;
        bRet             = FALSE;
        CamOsPrintf(PRINT_RED "%s %d, TimerInit Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }

    CamOsSpinUnlock(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsDeleteTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    u8 bRet;

    CamOsSpinLock(&pTimerCfg->sem_lock);

    if (CamOsTimerDeleteSync(&pTimerCfg->stTimerCfg) == CAM_OS_OK)
    {
        pTimerCfg->bCycle      = 0;
        pTimerCfg->bStop       = 1;
        pTimerCfg->pfnFunc     = NULL;
        pTimerCfg->u32TimerOut = 0;
        pTimerCfg->s32Id       = -1;
        bRet                   = TRUE;
    }
    else
    {
        CamOsPrintf(PRINT_RED "%s %d, TimerDelete Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    CamOsSpinUnlock(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsStopTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    u8 bRet;

    TIMER_MUTEX_LOCK(&pTimerCfg->sem_lock);

    if (pTimerCfg->bStop)
    {
        DBG_OS_HDMITX(CamOsPrintf(PRINT_RED "%s %d, TimerStop Already Stop\n" PRINT_NONE, __FUNCTION__, __LINE__));
        bRet = TRUE;
    }
    else
    {
        pTimerCfg->bCycle = 0;
        pTimerCfg->bStop  = 1;
        CamOsTimerDeleteSync(&pTimerCfg->stTimerCfg);
        bRet = TRUE;
    }

    TIMER_MUTEX_UNLOCK(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsStartTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    u8                           bRet;
    DrvHdmitxOsTimerNotifyFunc_t stNotify;

    stNotify.pfnFunc = _DrvHdmitxOsTimerNotify;

    CamOsSpinLock(&pTimerCfg->sem_lock);

    pTimerCfg->bCycle = 1;
    pTimerCfg->bStop  = 0;
    if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, (CamOsTimerCallback)stNotify.pfnFunc) == CAM_OS_OK)
    {
        bRet = TRUE;
    }
    else
    {
        CamOsPrintf(PRINT_RED "%s %d, TimerStart Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    CamOsSpinUnlock(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsCreateSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg, u32 u32InitVal)
{
    u8 bRet = TRUE;

    if (CamOsTsemInit(&pstSemaphoreCfg->stSemCfg, u32InitVal) == CAM_OS_OK)
    {
        pstSemaphoreCfg->s32Id = 1;
        bRet                   = TRUE;
    }
    else
    {
        pstSemaphoreCfg->s32Id = -1;
        bRet                   = FALSE;
        CamOsPrintf(PRINT_RED "%s %d, Semaphore Create Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvHdmitxOsDestroySemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg)
{
    u8 bRet = TRUE;

    if (CamOsTsemDeinit(&pstSemaphoreCfg->stSemCfg) == CAM_OS_OK)
    {
        pstSemaphoreCfg->s32Id = -1;
        bRet                   = TRUE;
    }
    else
    {
        bRet = FALSE;
        CamOsPrintf(PRINT_RED "%s %d, Semaphore Destroy Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    return bRet;
}

void DrvHdmitxOsObtainSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg)
{
    CamOsTsemDown(&pstSemaphoreCfg->stSemCfg);
}

void DrvHdmitxOsReleaseSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg)
{
    CamOsTsemUp(&pstSemaphoreCfg->stSemCfg);
}

u8 DrvHdmitxOsCreateMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
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

        CamOsPrintf(PRINT_RED "%s %d, MutexCreate Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvHdmitxOsDestroyMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    u8 bRet;

    if (CamOsMutexDestroy(&pstMutexCfg->stMutxCfg) == CAM_OS_OK)
    {
        pstMutexCfg->s32Id = -1;
        bRet               = TRUE;
    }
    else
    {
        CamOsPrintf(PRINT_RED "%s %d, Mutex Destroy Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvHdmitxOsObtainMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexLock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        CamOsPrintf(PRINT_RED "%s %d, Mutex Lock Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}

u8 DrvHdmitxOsReleaseMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexUnlock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        CamOsPrintf(PRINT_RED "%s %d, Mutex UnLock Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}

s32 DrvHdmitxOsCreateEventGroup(u8 *pEventName)
{
    s32 s32Id;

    EVENT_MUTEX_LOCK();

    for (s32Id = 0; s32Id < HDMITX_OS_EVENTGROUP_MAX; s32Id++)
    {
        if (g_hdmitxOsEventGroupInfo[s32Id].bUsed == FALSE)
        {
            break;
        }
    }

    if (s32Id >= HDMITX_OS_EVENTGROUP_MAX)
    {
        EVENT_MUTEX_UNLOCK();
        return -1;
    }

    CamOsSpinInit(&g_hdmitxOsEventGroupInfo[s32Id].stMutexEvent);
    g_hdmitxOsEventGroupInfo[s32Id].bUsed      = TRUE;
    g_hdmitxOsEventGroupInfo[s32Id].u32Waiting = 0;
    CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32Id].stMutexEvent);
    g_hdmitxOsEventGroupInfo[s32Id].u32EventGroup = 0;
    CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32Id].stMutexEvent);

    EVENT_MUTEX_UNLOCK();

    CamOsConditionInit(&g_hdmitxOsEventGroupInfo[s32Id].stEventWaitQueue);
    s32Id |= HDMITX_OS_ID_PREFIX;
    return s32Id;
}

u8 DrvHdmitxOsDeleteEventGroup(s32 s32EventGroupId)
{
    if ((s32EventGroupId & HDMITX_OS_ID_PREFIX_MASK) == HDMITX_OS_ID_PREFIX)
    {
        s32EventGroupId &= HDMITX_OS_ID_MASK;
    }
    else
    {
        return FALSE;
    }

    EVENT_MUTEX_LOCK();
    if (!g_hdmitxOsEventGroupInfo[s32EventGroupId].bUsed)
    {
        EVENT_MUTEX_UNLOCK();
        return FALSE;
    }

    CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
    g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup = 0;
    CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
    g_hdmitxOsEventGroupInfo[s32EventGroupId].bUsed      = FALSE;
    g_hdmitxOsEventGroupInfo[s32EventGroupId].u32Waiting = 0;
    EVENT_MUTEX_UNLOCK();

    CamOsConditionInit(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue);
    return TRUE;
}

u8 DrvHdmitxOsSetEvent(s32 s32EventGroupId, u32 u32EventFlag)
{
    if ((s32EventGroupId & HDMITX_OS_ID_PREFIX_MASK) == HDMITX_OS_ID_PREFIX)
    {
        s32EventGroupId &= HDMITX_OS_ID_MASK;
    }
    else
    {
        return FALSE;
    }

    EVENT_MUTEX_LOCK();
    if (!g_hdmitxOsEventGroupInfo[s32EventGroupId].bUsed)
    {
        EVENT_MUTEX_UNLOCK();
        return FALSE;
    }
    EVENT_MUTEX_UNLOCK();

    CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
    SET_FLAG(g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup, u32EventFlag);
    CamOsConditionWakeUpAll(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue);
    CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);

    return TRUE;
}

u32 DrvHdmitxOsGetEvent(s32 s32EventGroupId)
{
    u32 u32Event = 0;
    if ((s32EventGroupId & HDMITX_OS_ID_PREFIX_MASK) == HDMITX_OS_ID_PREFIX)
    {
        s32EventGroupId &= HDMITX_OS_ID_MASK;
    }
    else
    {
        return FALSE;
    }
    u32Event = HAS_FLAG(g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup, 0xFFFFFFFF);

    return u32Event;
}

u8 DrvHdmitxOsWaitEvent(s32 s32EventGroupId, u32 u32WaitEventFlag, u32 *pu32RetrievedEventFlag,
                        DrvHdmitxOsEventWaitMoodeType_e eWaitMode, u32 u32WaitMs)
{
    u8 bRet = FALSE;
    u8 bAnd;
    u8 bClear;

    *pu32RetrievedEventFlag = 0;

    if (!u32WaitEventFlag)
    {
        return FALSE;
    }

    if ((s32EventGroupId & HDMITX_OS_ID_PREFIX_MASK) == HDMITX_OS_ID_PREFIX)
    {
        s32EventGroupId &= HDMITX_OS_ID_MASK;
    }
    else
    {
        return FALSE;
    }

    EVENT_MUTEX_LOCK();
    if (!g_hdmitxOsEventGroupInfo[s32EventGroupId].bUsed)
    {
        EVENT_MUTEX_UNLOCK();
        return FALSE;
    }
    g_hdmitxOsEventGroupInfo[s32EventGroupId].u32Waiting++;
    EVENT_MUTEX_UNLOCK();

    bClear = ((E_DRV_HDMITX_OS_EVENT_MD_AND_CLEAR == eWaitMode) || (E_DRV_HDMITX_OS_EVENT_MD_OR_CLEAR == eWaitMode))
                 ? TRUE
                 : FALSE;
    bAnd   = ((E_DRV_HDMITX_OS_EVENT_MD_AND == eWaitMode) || (E_DRV_HDMITX_OS_EVENT_MD_AND_CLEAR == eWaitMode)) ? TRUE
                                                                                                                : FALSE;

    CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);

    do
    {
        if (u32WaitMs == HDMITX_OS_WAIT_FOREVER) // blocking wait
        {
            if (bAnd)
            {
                CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
                CamOsConditionWait(
                    &g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue,
                    ((g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup & u32WaitEventFlag) == u32WaitEventFlag));
                CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
            }
            else
            {
                CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
                CamOsConditionWait(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue,
                                   ((g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup & u32WaitEventFlag) != 0));
                CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
            }
        }
        else if (u32WaitMs == 0)
        {
            *pu32RetrievedEventFlag =
                HAS_FLAG(g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup, u32WaitEventFlag);
            break;
        }
        else
        {
            u32WaitMs = CamOsMsToJiffies(u32WaitMs);

            if (bAnd)
            {
                CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
                CamOsConditionTimedWait(
                    &g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue,
                    ((g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup & u32WaitEventFlag) == u32WaitEventFlag),
                    u32WaitMs);
                CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
            }
            else
            {
                CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
                CamOsConditionTimedWait(
                    &g_hdmitxOsEventGroupInfo[s32EventGroupId].stEventWaitQueue,
                    ((g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup & u32WaitEventFlag) != 0), u32WaitMs);
                CamOsSpinLock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
            }
        }
        *pu32RetrievedEventFlag = HAS_FLAG(g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup, u32WaitEventFlag);
    } while (0);

    // spin_lock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);
    bRet = (bAnd) ? (*pu32RetrievedEventFlag == u32WaitEventFlag) : (0 != *pu32RetrievedEventFlag);
    if (bRet && bClear)
    {
        RESET_FLAG(g_hdmitxOsEventGroupInfo[s32EventGroupId].u32EventGroup, *pu32RetrievedEventFlag);
    }
    CamOsSpinUnlock(&g_hdmitxOsEventGroupInfo[s32EventGroupId].stMutexEvent);

    EVENT_MUTEX_LOCK();
    g_hdmitxOsEventGroupInfo[s32EventGroupId].u32Waiting--;
    EVENT_MUTEX_UNLOCK();

    return bRet;
}

u8 DrvHdmitxOsAttachInterrupt(u32 u32IntNum, INTERRUPT_CALLBACK pIntCb)
{
    return TRUE;
}

u8 DrvHdmitxOsDetachInterrupt(u32 u32IntNum)
{
    return TRUE;
}

u8 DrvHdmitxOsEnableInterrupt(u32 u32IntNum)
{
    return TRUE;
}

u8 DrvHdmitxOsDisableInterrupt(u32 u32IntNum)
{
    return TRUE;
}

u32 DrvHdmitxOsDisableAllInterrupts(void)
{
    return 0;
}

u8 DrvHdmitxOsRestoreAllInterrupts(u32 u32OldInterrupts)
{
    return TRUE;
}

u8 DrvHdmitxOsCreateTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg, TASK_ENTRY_CB pTaskEntry, void *pDataPtr, u8 *pTaskName,
                         u8 bAuotStart)
{
    CamOsThreadAttrb_t stThreadAttrCfg = {0};
    u8                 bRet            = TRUE;

    stThreadAttrCfg.nPriority  = 99;
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
                CamOsPrintf(PRINT_RED "%s %d, CamOsThreadWakeUp Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
            }
        }
    }
    else
    {
        pstTaskCfg->s32Id = -1;
        bRet              = FALSE;
        CamOsPrintf(PRINT_RED "%s %d, CamOsThreadCreate Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvHdmitxOsDestroyTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg)
{
    u8 bRet = TRUE;

    if (CamOsThreadStop(pstTaskCfg->stThreadCfg) != CAM_OS_OK)
    {
        pstTaskCfg->s32Id = -1;
        CamOsPrintf(PRINT_RED "%s %d, CamOsThreadStop Fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvHdmitxOsTaskShouldStop(void)
{
    u8 bRet = TRUE;

    if (CamOsThreadShouldStop() != CAM_OS_OK)
    {
        bRet = FALSE;
    }
    return bRet;
}
u8 DrvHdmitxOsGetMmioBase(void **ppaBaseAddr, u32 *pu32BaseSize, DrvHdmitxOsMmioType_e enType)
{
    switch (enType)
    {
        case E_HDMITX_OS_MMIO_PM:
            *ppaBaseAddr  = (void *)HDMITX_OS_MMIO_PM_BASE;
            *pu32BaseSize = HDMITX_OS_MMIO_PM_SIZE;
            break;

        case E_HDMITX_OS_MMIO_NONEPM:
            *ppaBaseAddr  = (void *)HDMITX_OS_MMIO_NONEPM_BASE;
            *pu32BaseSize = HDMITX_OS_MMIO_NONEPM_SIZE;
            break;

        default:
            break;
    }
    return TRUE;
}

u8 DrvHdmitxOsGpioRequestOutput(u8 u8GpioNum)
{
    int ret = camdriver_gpio_request(NULL, u8GpioNum);

    if (ret < 0)
    {
        CamOsPrintf(PRINT_RED "%s %d, gpio_request(%d) fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    else
    {
        ret = camdriver_gpio_direction_output(NULL, u8GpioNum, 0);

        if (ret < 0)
        {
            CamOsPrintf(PRINT_RED "%s %d, gpio_direction_input (%d) fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        }
    }

    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsGpioRequestInput(u8 u8GpioNum)
{
    int ret = camdriver_gpio_request(NULL, u8GpioNum);

    if (ret < 0)
    {
        CamOsPrintf(PRINT_RED "%s %d, gpio_request(%d) fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    else
    {
        ret = camdriver_gpio_direction_input(NULL, u8GpioNum);

        if (ret < 0)
        {
            CamOsPrintf(PRINT_RED "%s %d, gpio_direction_input (%d) fail\n" PRINT_NONE, __FUNCTION__, __LINE__);
        }
    }

    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsSetGpioValue(u8 u8GpioNum, u8 u8Val)
{
    int ret;

    ret = camdriver_gpio_direction_output(NULL, u8GpioNum, u8Val);
    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsGetGpioValue(u8 u8GpioNum)
{
    int ret;

    camdriver_gpio_direction_input(NULL, u8GpioNum);
    ret = camdriver_gpio_get(NULL, u8GpioNum);
    return ret ? TRUE : FALSE;
}

u8 DrvHdmitxOsGpioIrq(u8 u8GpioNum, INTERRUPT_CB pIsrCb, u8 bEn, void *pData)
{
    return TRUE;
}

void DrvHdmitxOsSetHpdIrq(u8 bEn)
{
    g_bHpdIrqEn = bEn;
}

u8 DrvHdmitxOsSetI2cAdapter(u16 u16Id)
{
    return TRUE;
}

u8 DrvHdmitxOsSetI2cReadBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen)
{
    return TRUE;
}

u8 DrvHdmitxOsSetI2cWriteBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen, u8 bSendStop)
{
    return TRUE;
}

void DrvHdmitxOsMsSleep(u32 u32Msec)
{
    CamOsMsSleep(u32Msec);
}

void DrvHdmitxOsUsSleep(u32 u32Usec)
{
    CamOsUsSleep(u32Usec);
}

void DrvHdmitxOsMsDelay(u32 u32Msec)
{
    CamOsMsDelay(u32Msec);
}

void DrvHdmitxOsUsDelay(u32 u32Usec)
{
    CamOsUsDelay(u32Usec);
}

void DrvHdmitxOsSetI2cId(u16 u16Id)
{
    g_u16I2cId = u16Id;
}

u16 DrvHdmitxOsGetI2cId(void)
{
    return g_u16I2cId;
}

u8 DrvHdmitxOsSetDeviceNode(void *pPlatFormDev)
{
    return FALSE;
}

u8 DrvHdmitxOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    return FALSE;
}

u8 DrvHdmitxOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    return FALSE;
}

u8 DrvHdmitxOsSetHpdGpinPin(u8 u8Pin)
{
    g_u8HpdGpioPin = u8Pin;
    return TRUE;
}

u8 DrvHdmitxOsGetHpdGpinPin(void)
{
    return g_u8HpdGpioPin;
}

void DrvHdmitxOsSetSwI2cPin(u8 u8SdaPin, u8 u8SclPin)
{
    g_u8SwI2cSdaPin = u8SdaPin;
    g_u8SwI2cSclPin = u8SclPin;
}

void DrvHdmitxOsSetSwI2cEn(u8 bEn)
{
    g_bSwI2cEn = bEn;
}

u32 DrvHdmitxOsGetCpuSpeedMhz(void)
{
    return 1000;
}

void DrvHdmitxOsSetDebugEnable(u8 bEn)
{
    g_bHdmitxOsDbgEn = bEn;
}
u32 DrvHdmitxOsStrToL(u8 *str, u16 u16Base, u32 *pValue)
{
    *pValue = CamOsStrtol((char *)str, NULL, u16Base);
    return 1;
}
u8 DrvHdmitxOsStrCmp(u8 *str,const char *dstr)
{
    u8 ret;
    ret = strcmp((char *)str, dstr);
    return ret;
}
