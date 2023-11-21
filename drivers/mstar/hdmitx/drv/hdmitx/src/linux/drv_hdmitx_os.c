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
//#include <linux/sched.h>
#include <linux/gpio.h>
#include <linux/i2c.h>

#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "cam_sysfs.h"
#include "cam_clkgen.h"
#include "mdrv_gpio_io.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "hal_hdmitx_enum.h"
#include "regHDMITx.h"
#include "mhal_hdmitx.h"
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
    u32         u32Dummy;
    CamOsTsem_t EventGroup;
} DrvHdmitxOsMutext_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8                                g_bHdmitxOsInit = 0;
DrvHdmitxOsEventGroupInfoConfig_t g_hdmitxOsEventGroupInfo[HDMITX_OS_EVENTGROUP_MAX];

CamOsSpinlock_t g_eventGroupMutex;

struct i2c_adapter *g_stI2cAdpa = NULL;
u16                 g_u16I2cId  = 0;
#ifdef CONFIG_HDMITX_NONUSEGPIO
u16 g_u16I2cUsed = 0;
#else
u16 g_u16I2cUsed = 1;
#endif
u8                  g_u8HpdGpioPin      = 0;
struct device_node *g_pHdmitxDeviceNode = NULL;

u8 g_bHdmitxOsDbgEn = 0;

extern u8 g_u8SwI2cSdaPin;
extern u8 g_u8SwI2cSclPin;
extern u8 g_bSwI2cEn;
extern u8 g_bHpdIrqEn;
//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------
extern u8  HalHdmitxUtilityI2cBurstReadBytes(u8 ucSlaveAdr, u8 ucSubAdr, u8 *pBuf, u16 ucBufLen);
extern u8  HalHdmitxUtilityEdidSetSegment(u8 value);
extern u8  HalHdmitxUtilityAdjustDDCFreq(u32 u32Speed_K);
extern u32 HalHdmitxUtilityGetDDCDelayCount(void);

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
        if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, stNotify.pfnFunc)
            != CAM_OS_OK)
        {
            HDMITX_ERR( "%s %d: CamOsTimerAdd Fail\n" , __FUNCTION__, __LINE__);
        }
    }

    CamOsSpinUnlock(&pTimerCfg->sem_lock);

    if (pTimerCfg->pfnFunc)
    {
        pTimerCfg->pfnFunc(0, pTimerCfg->pCtx);
    }
}

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
u8 DrvHdmitxOsInit(void)
{
    static u32 u32I = 0;

    if (g_bHdmitxOsInit == 0 && u32I < HDMITX_OS_EVENTGROUP_MAX)
    {
        g_hdmitxOsEventGroupInfo[u32I].bUsed = FALSE;
        g_bHdmitxOsInit                      = 1;
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

    TIMER_MUTEX_LOCK(&pTimerCfg->sem_lock);

    if (CamOsTimerInit(&pTimerCfg->stTimerCfg) == CAM_OS_OK)
    {
        pTimerCfg->s32Id  = s32TimerId++;
        pTimerCfg->bCycle = 1;
        if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, stNotify.pfnFunc)
            == CAM_OS_OK)
        {
            bRet = TRUE;
        }
        else
        {
            pTimerCfg->s32Id = 0;
            bRet             = FALSE;
            HDMITX_ERR( "%s %d, TimerAdd Fail\n" , __FUNCTION__, __LINE__);
        }
    }
    else
    {
        pTimerCfg->s32Id = -1;
        bRet             = FALSE;
        HDMITX_ERR( "%s %d, TimerInit Fail\n" , __FUNCTION__, __LINE__);
    }

    TIMER_MUTEX_UNLOCK(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsDeleteTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    u8 bRet;

    TIMER_MUTEX_LOCK(&pTimerCfg->sem_lock);

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
        HDMITX_ERR( "%s %d, TimerDelete Fail\n" , __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    TIMER_MUTEX_UNLOCK(&pTimerCfg->sem_lock);

    return bRet;
}

u8 DrvHdmitxOsStopTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    u8 bRet;

    TIMER_MUTEX_LOCK(&pTimerCfg->sem_lock);

    if (pTimerCfg->bStop)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF, "%s %d, TimerStop Already Stop\n" , __FUNCTION__, __LINE__);
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

    TIMER_MUTEX_LOCK(&pTimerCfg->sem_lock);

    pTimerCfg->bCycle = 1;
    pTimerCfg->bStop  = 0;
    if (CamOsTimerAdd(&pTimerCfg->stTimerCfg, pTimerCfg->u32TimerOut, (void *)pTimerCfg, stNotify.pfnFunc) == CAM_OS_OK)
    {
        bRet = TRUE;
    }
    else
    {
        HDMITX_ERR( "%s %d, TimerStart Fail\n" , __FUNCTION__, __LINE__);
        bRet = FALSE;
    }

    TIMER_MUTEX_UNLOCK(&pTimerCfg->sem_lock);

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
        HDMITX_ERR( "%s %d, Semaphore Create Fail\n" , __FUNCTION__, __LINE__);
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
        HDMITX_ERR( "%s %d, Semaphore Destroy Fail\n" , __FUNCTION__, __LINE__);
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

        HDMITX_ERR( "%s %d, MutexCreate Fail\n" , __FUNCTION__, __LINE__);
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
        HDMITX_ERR( "%s %d, Mutex Destroy Fail\n" , __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 DrvHdmitxOsObtainMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexLock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        HDMITX_ERR( "%s %d, Mutex Lock Fail\n" , __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}

u8 DrvHdmitxOsReleaseMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    if (CamOsMutexUnlock(&pstMutexCfg->stMutxCfg) != CAM_OS_OK)
    {
        HDMITX_ERR( "%s %d, Mutex UnLock Fail\n" , __FUNCTION__, __LINE__);
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

#if 0
    if (0 < g_hdmitxOsEventGroupInfo[s32EventGroupId].u32Waiting)
    {
        EVENT_MUTEX_UNLOCK();
        CamOsPrintf("\033[35m[%s][%d] EventGroup Delete FAIL : Event was still waiting.\033[m\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
#endif
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
                HDMITX_ERR( "%s %d, CamOsThreadWakeUp Fail\n" , __FUNCTION__, __LINE__);
            }
        }
    }
    else
    {
        pstTaskCfg->s32Id = -1;
        bRet              = FALSE;
        HDMITX_ERR( "%s %d, CamOsThreadCreate Fail\n" , __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvHdmitxOsDestroyTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg)
{
    if (CamOsThreadStop(pstTaskCfg->stThreadCfg) == CAM_OS_OK)
    {
        pstTaskCfg->s32Id = -1;
    }
    else
    {
        HDMITX_ERR( "%s %d, CamOsThreadStop Fail\n" , __FUNCTION__, __LINE__);
        return FALSE;
    }
    return TRUE;
}

u8 DrvHdmitxOsTaskShouldStop(void)
{
    return CamOsThreadShouldStop();
}
u8 DrvHdmitxOsGetMmioBase(void **ppaBaseAddr, u32 *pu32BaseSize, DrvHdmitxOsMmioType_e enType)
{
    u8 bRet = TRUE;

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
            bRet = FALSE;
            break;
    }
    return TRUE;
}

u8 DrvHdmitxOsGpioRequestOutput(u8 u8GpioNum)
{
    u32 ret = 0;
#ifndef CONFIG_HDMITX_NONUSEGPIO
    ret = camdriver_gpio_request(NULL, u8GpioNum);

    if (ret < 0)
    {
        HDMITX_ERR( "%s %d, gpio_request(%d) fail\n" , __FUNCTION__, __LINE__);
    }
    else
    {
        ret = camdriver_gpio_direction_output(NULL, u8GpioNum, 0);

        if (ret < 0)
        {
            HDMITX_ERR( "%s %d, gpio_direction_input (%d) fail\n" , __FUNCTION__, __LINE__);
        }
    }
#endif
    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsGpioRequestInput(u8 u8GpioNum)
{
    u32 ret = 0;
#ifndef CONFIG_HDMITX_NONUSEGPIO
    ret = camdriver_gpio_request(NULL, u8GpioNum);

    if (ret < 0)
    {
        HDMITX_ERR( "%s %d, gpio_request(%d) fail\n" , __FUNCTION__, __LINE__);
    }
    else
    {
        ret = camdriver_gpio_direction_input(NULL, u8GpioNum);

        if (ret < 0)
        {
            HDMITX_ERR( "%s %d, gpio_direction_input (%d) fail\n" , __FUNCTION__, __LINE__);
        }
    }
#endif
    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsSetGpioValue(u8 u8GpioNum, u8 u8Val)
{
    u32 ret = 0;
#ifndef CONFIG_HDMITX_NONUSEGPIO
    ret = camdriver_gpio_direction_output(NULL, u8GpioNum, u8Val);
#endif
    return ret < 0 ? FALSE : TRUE;
}

u8 DrvHdmitxOsGetGpioValue(u8 u8GpioNum)
{
    u32 ret = 0;
#ifndef CONFIG_HDMITX_NONUSEGPIO
    camdriver_gpio_direction_input(NULL, u8GpioNum);
    ret = camdriver_gpio_get(NULL, u8GpioNum);
#endif
    return ret ? TRUE : FALSE;
}

u8 DrvHdmitxOsGpioIrq(u8 u8GpioNum, INTERRUPT_CB pIsrCb, u8 bEn, void *pData)
{
    u8 bRet = 1;
#ifndef CONFIG_HDMITX_NONUSEGPIO
    u32 u32IrqNum;

    if (bEn)
    {
        if (camdriver_gpio_request(NULL, u8GpioNum) < 0)
        {
            HDMITX_ERR( "%s %d, gpio(%d) request fail\n" , __FUNCTION__, __LINE__, u8GpioNum);
            bRet = 0;
        }
        else
        {
            if (camdriver_gpio_direction_input(NULL, u8GpioNum) < 0)
            {
                HDMITX_ERR( "%s %d, gpio(%d) direction input fail\n" , __FUNCTION__, __LINE__,
                            u8GpioNum);
                bRet = 0;
            }
            else
            {
                u32IrqNum = camdriver_gpio_to_irq(NULL, u8GpioNum);

                if (CamOsIrqRequest(u32IrqNum, (CamOsIrqHandler)pIsrCb, "HPD_ISR", pData))
                {
                    HDMITX_ERR( "%s %d, giop(%d), can't allocate irq (%d)\n" , __FUNCTION__,
                                __LINE__, u8GpioNum, u32IrqNum);

                    bRet = 0;
                }
            }
        }
    }
    else
    {
        u32IrqNum = camdriver_gpio_to_irq(NULL, u8GpioNum);
        free_irq(u32IrqNum, NULL);
    }
#endif
    return bRet;
}

void DrvHdmitxOsSetHpdIrq(u8 bEn)
{
    g_bHpdIrqEn = bEn;
}

u8 DrvHdmitxOsSetI2cAdapter(u16 u16Id)
{
    g_stI2cAdpa = i2c_get_adapter(u16Id);

    if (g_stI2cAdpa == NULL)
    {
        HDMITX_ERR("%s %d,id:%d Fail\n", __FUNCTION__, __LINE__, u16Id);
        return FALSE;
    }
    return TRUE;
}

u8 DrvHdmitxOsSetI2cReadBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen)
{
    struct i2c_msg msg[2];
    u8             u8Data[4];

    u8Data[0]    = u8SubAdr;
    msg[0].addr  = u8SlaveAdr >> 1;
    msg[0].flags = 0;
    msg[0].buf   = u8Data;
    msg[0].len   = 1;

    msg[1].addr  = u8SlaveAdr >> 1;
    msg[1].flags = 1;
    msg[1].buf   = pBuf;
    msg[1].len   = u16BufLen;

    if (i2c_transfer(g_stI2cAdpa, msg, 2) < 0)
    {
        HDMITX_ERR("%s %d, SlaveId:%x, Len:%d Fail\n", __FUNCTION__, __LINE__, u8SlaveAdr, u16BufLen);
        return FALSE;
    }
    return TRUE;
}

u8 DrvHdmitxOsSetI2cWriteBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen, u8 bSendStop)
{
    struct i2c_msg msg[1];
    u8 *           pu8Data;
    u8             bRet;

    pu8Data = (u8 *)DrvHdmitxOsMemAlloc(u16BufLen + 1);

    if (pu8Data == NULL)
    {
        HDMITX_ERR("%s %d, SlaveId:%x, Len:%d Fail\n", __FUNCTION__, __LINE__, u8SlaveAdr, u16BufLen);
        bRet = FALSE;
    }
    else
    {
        pu8Data[0] = u8SubAdr;
        memcpy(&pu8Data[1], pBuf, u16BufLen);

        msg[0].addr  = u8SlaveAdr >> 1;
        msg[0].flags = bSendStop ? 0 : 2;
        msg[0].buf   = pu8Data;
        msg[0].len   = u16BufLen + 1;

        if (i2c_transfer(g_stI2cAdpa, msg, 1) < 0)
        {
            HDMITX_ERR("%s %d, SlaveId:%x, Len:%d Fail\n", __FUNCTION__, __LINE__, u8SubAdr, u16BufLen);
            bRet = FALSE;
        }
        else
        {
            bRet = TRUE;
        }

        CamOsMemRelease(pu8Data);
    }
    return bRet;
}

void DrvHdmitxOsMsSleep(u32 u32Msec)
{
#ifndef CONFIG_HDMITX_NONUSEGPIO
    CamOsMsSleep(u32Msec);
#endif
}

void DrvHdmitxOsUsSleep(u32 u32Usec)
{
#ifndef CONFIG_HDMITX_NONUSEGPIO
    CamOsUsSleep(u32Usec);
#endif
}

void DrvHdmitxOsMsDelay(u32 u32Msec)
{
#ifndef CONFIG_HDMITX_NONUSEGPIO
    CamOsMsDelay(u32Msec);
#endif
}

void DrvHdmitxOsUsDelay(u32 u32Usec)
{
#ifndef CONFIG_HDMITX_NONUSEGPIO
    CamOsUsDelay(u32Usec);
#endif
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
    g_pHdmitxDeviceNode = ((struct platform_device *)pPlatFormDev)->dev.of_node;

    if (g_pHdmitxDeviceNode)
    {
        return TRUE;
    }
    return FALSE;
}
#ifdef CONFIG_CAM_CLK
#include "drv_camclk_Api.h"
void **pvHdmitxclk     = NULL;
u32    HdmitxParentCnt = 1;

u8 Hdmitx_ClkRegister(struct device_node *device_node)
{
    u32 u32clknum;
    u32 HdmitxClk;
    u8  str[16];

    if (of_find_property(device_node, "camclk", &HdmitxParentCnt))
    {
        HdmitxParentCnt /= sizeof(u32);
        // printk( "[%s] Number : %d\n", __FUNCTION__, num_parents);
        if (HdmitxParentCnt < 0)
        {
            HDMITX_ERR("[%s] Fail to get parent count! Error Number : %d\n", __FUNCTION__, HdmitxParentCnt);
            return 0;
        }
        pvHdmitxclk = CamOsMemAlloc((sizeof(void *) * HdmitxParentCnt));
        if (!pvHdmitxclk)
        {
            return 0;
        }
        for (u32clknum = 0; u32clknum < HdmitxParentCnt; u32clknum++)
        {
            HdmitxClk = 0;
            msys_of_property_read_u32_index(device_node, "camclk", u32clknum, &(HdmitxClk));
            if (!HdmitxClk)
            {
                HDMITX_ERR("[%s] Fail to get clk!\n", __FUNCTION__);
            }
            else
            {
                CamOsSnprintf(str, 16, "Hdmitx_%d ", u32clknum);
                CamClkRegister(str, HdmitxClk, &(pvHdmitxclk[u32clknum]));
            }
        }
    }
    else
    {
        HDMITX_ERR("[%s] W/O Camclk \n", __FUNCTION__);
    }
    return 1;
}
u8 Hdmitx_ClkUnregister(void)
{
    u32 u32clknum;

    for (u32clknum = 0; u32clknum < HdmitxParentCnt; u32clknum++)
    {
        if (pvHdmitxclk[u32clknum])
        {
            printk(KERN_DEBUG "[%s] %p\n", __FUNCTION__, pvHdmitxclk[u32clknum]);
            CamClkUnregister(pvHdmitxclk[u32clknum]);
            pvHdmitxclk[u32clknum] = NULL;
        }
    }
    CamOsMemRelease(pvHdmitxclk);

    return 1;
}
#endif
u8 DrvHdmitxOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    u8             bRet = FALSE;
    u32            u32NumParents;
    u32            u32idx;
    struct clk **  pstHdmitxClks;
    struct clk_hw *pstHwParent;
    u32            u32Clkrate;
    u32 *          p32clkratebuf                    = (u32 *)pClkRate;
    u8 *           p8clkenbuf                       = (u8 *)pClkEn;
    u8             abClkMuxAttr[HAL_HDMITX_CLK_NUM] = HAL_HDMITX_CLK_MUX_ATTR;
#ifdef CONFIG_CAM_CLK
    u32                  u32clknum = 0;
    CAMCLK_Set_Attribute stSetCfg;
    CAMCLK_Get_Attribute stGetCfg;

    if (g_pHdmitxDeviceNode && pClkRate && u32ClkRateSize == HAL_HDMITX_CLK_NUM)
    {
        bRet = TRUE;
        Hdmitx_ClkRegister(g_pHdmitxDeviceNode);
        for (u32clknum = 0; u32clknum < HdmitxParentCnt; u32clknum++)
        {
            if (pvHdmitxclk[u32clknum])
            {
                u32Clkrate = p32clkratebuf[u32clknum];
                if (abClkMuxAttr[u32clknum] == 1)
                {
                    CamClkAttrGet(pvHdmitxclk[u32clknum], &stGetCfg);
                    CAMCLK_SETPARENT(stSetCfg, stGetCfg.u32Parent[u32Clkrate]);
                    CamClkAttrSet(pvHdmitxclk[u32clknum], &stSetCfg);
                }
                else
                {
                    CAMCLK_SETRATE_ROUNDUP(stSetCfg, u32Clkrate);
                    CamClkAttrSet(pvHdmitxclk[u32clknum], &stSetCfg);
                }
                if (p8clkenbuf[u32clknum] == 0)
                {
                    CamClkSetOnOff(pvHdmitxclk[u32clknum], 1);
                }
            }
        }
    }
#else
    if (g_pHdmitxDeviceNode && pClkRate && u32ClkRateSize == HAL_HDMITX_CLK_NUM)
    {
        bRet          = TRUE;
        u32NumParents = CamOfClkGetParentCount(g_pHdmitxDeviceNode);

        HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF,"%s %d u32NumParents:%d \n", __FUNCTION__, __LINE__, u32NumParents);
        for (u32idx = 0; u32idx < u32ClkRateSize; u32idx++)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF,"%s %d, CLK_%d = %d\n", __FUNCTION__, __LINE__, u32idx, p32clkratebuf[u32idx]);
        }

        if (u32NumParents == HAL_HDMITX_CLK_NUM)
        {
            pstHdmitxClks = CamOsMemAlloc((sizeof(struct clk *) * u32NumParents));

            if (pstHdmitxClks == NULL)
            {
                HDMITX_ERR("%s %d Alloc pstHdmitxClks is NULL\n", __FUNCTION__, __LINE__);
                return 0;
            }

            for (u32idx = 0; u32idx < u32NumParents; u32idx++)
            {
                pstHdmitxClks[u32idx] = of_clk_get(g_pHdmitxDeviceNode, u32idx);
                if (IS_ERR(pstHdmitxClks[u32idx]))
                {
                    HDMITX_ERR("%s %d, Fail to get [Hdmitx] %s\n", __FUNCTION__, __LINE__,
                                CamOfClkGetParentName(g_pHdmitxDeviceNode, u32idx));
                    CamOsMemRelease(pstHdmitxClks);
                    return 0;
                }

                if (abClkMuxAttr[u32idx] == 1)
                {
                    u32Clkrate  = p32clkratebuf[u32idx];
                    pstHwParent = CamClkGetParentByIndex(__CamClkGetHw(pstHdmitxClks[u32idx]), u32Clkrate);
                    CamClkSetParent(pstHdmitxClks[u32idx], pstHwParent->clk);
                }
                else
                {
                    u32Clkrate = CamClkRoundRate(pstHdmitxClks[u32idx], p32clkratebuf[u32idx]);
                    CamClkSetRate(pstHdmitxClks[u32idx], u32Clkrate + 1000000);
                }

                HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF,
                    "%s %d, [Hdmitx] u32NumParents:%d-%d %15s En:%d, Rate:%d\n", __FUNCTION__, __LINE__, u32NumParents,
                    u32idx + 1, CamOfClkGetParentName(g_pHdmitxDeviceNode, u32idx), p8clkenbuf[u32idx], u32Clkrate);

                if (p8clkenbuf[u32idx] == 0)
                {
                    CamClkPrepareEnable(pstHdmitxClks[u32idx]);
                }
                clk_put(pstHdmitxClks[u32idx]);
            }

            CamOsMemRelease(pstHdmitxClks);
        }
        else
        {
            bRet = FALSE;
        }
    }
#endif
    return bRet;
}

u8 DrvHdmitxOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize)
{
    u8           bRet = FALSE;
    u32          u32NumParents;
    u32          u32idx;
    struct clk **pstHdmitxClks;
    u8 *         p32clkenbuf = (u8 *)pClkEn;
#ifdef CONFIG_CAM_CLK
    u32 u32clknum = 0;

    bRet = TRUE;
    for (u32clknum = 0; u32clknum < HdmitxParentCnt; u32clknum++)
    {
        if (pvHdmitxclk[u32clknum] && p32clkenbuf[u32clknum])
            ;
        {
            CamClkSetOnOff(pvHdmitxclk[u32clknum], 0)
        }
    }
    Hdmitx_ClkUnregister();
#else
    if (g_pHdmitxDeviceNode)
    {
        bRet          = TRUE;
        u32NumParents = CamOfClkGetParentCount(g_pHdmitxDeviceNode);

        HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF,"%s %d u32NumParents:%d\n", __FUNCTION__, __LINE__, u32NumParents);
        if (u32NumParents == HAL_HDMITX_CLK_NUM)
        {
            pstHdmitxClks = CamOsMemAlloc((sizeof(struct clk *) * u32NumParents));

            if (pstHdmitxClks == NULL)
            {
                HDMITX_ERR("%s %d Alloc pstHdmitxClks is NULL\n", __FUNCTION__, __LINE__);
                return 0;
            }

            for (u32idx = 0; u32idx < u32NumParents; u32idx++)
            {
                pstHdmitxClks[u32idx] = of_clk_get(g_pHdmitxDeviceNode, u32idx);
                if (!IS_ERR(pstHdmitxClks[u32idx]))
                {
                    HDMITX_DBG(HDMITX_DBG_LEVEL_DRV_IF,"%s %d, [Hdmitx] %d-%d, %15s En:%d, \n", __FUNCTION__, __LINE__,
                                              u32NumParents, u32idx + 1,
                                              CamOfClkGetParentName(g_pHdmitxDeviceNode, u32idx), p32clkenbuf[u32idx]);
                    if (p32clkenbuf[u32idx])
                    {
                        CamClkDisableUnprepare(pstHdmitxClks[u32idx]);
                    }
                }
                else
                {
                    HDMITX_ERR("%s %d, Fail to get [Hdmitx] %s\n", __FUNCTION__, __LINE__,
                                CamOfClkGetParentName(g_pHdmitxDeviceNode, u32idx));
                    CamOsMemRelease(pstHdmitxClks);
                    return 0;
                }
                clk_put(pstHdmitxClks[u32idx]);
            }
            CamOsMemRelease(pstHdmitxClks);
        }
        else
        {
            bRet = FALSE;
        }
    }
#endif
    return bRet;
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
    *pValue = CamOsStrtol(str, NULL, u16Base);
    return 1;
}
u8 DrvHdmitxOsStrCmp(u8 *str,const char *dstr)
{
    u8 ret;
    ret = strcmp(str, dstr);
    return ret;
}

EXPORT_SYMBOL(MhalHdmitxCreateInstance);
EXPORT_SYMBOL(MhalHdmitxGetInstance);
EXPORT_SYMBOL(MhalHdmitxDestroyInstance);
EXPORT_SYMBOL(MhalHdmitxSetAttrBegin);
EXPORT_SYMBOL(MhalHdmitxSetAttr);
EXPORT_SYMBOL(MhalHdmitxSetAttrEnd);
EXPORT_SYMBOL(MhalHdmitxSetMute);
EXPORT_SYMBOL(MhalHdmitxSetSignal);
EXPORT_SYMBOL(MhalHdmitxSetAnalogDrvCur);
EXPORT_SYMBOL(MhalHdmitxSetInfoFrame);
EXPORT_SYMBOL(MhalHdmitxGetSinkInfo);
EXPORT_SYMBOL(MhalHdmitxSetDebugLevel);
EXPORT_SYMBOL(MhalHdmitxSetHpdConfig);

EXPORT_SYMBOL(DrvHdmitxOsStrToL);
EXPORT_SYMBOL(DrvHdmitxOsStrCmp);
EXPORT_SYMBOL(DrvHdmitxOsMemRelease);
EXPORT_SYMBOL(DrvHdmitxOsGetSystemTime);
EXPORT_SYMBOL(DrvHdmitxOsMemAlloc);

EXPORT_SYMBOL(HalHdmitxUtilityEdidSetSegment);
EXPORT_SYMBOL(HalHdmitxUtilityI2cBurstReadBytes);
EXPORT_SYMBOL(HalHdmitxUtilityGetDDCDelayCount);
EXPORT_SYMBOL(HalHdmitxUtilityAdjustDDCFreq);
