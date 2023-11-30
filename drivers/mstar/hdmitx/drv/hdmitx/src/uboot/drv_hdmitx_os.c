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
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HDMITX_OS_ID_PREFIX      0x76540000
#define HDMITX_OS_ID_PREFIX_MASK 0xFFFF0000
#define HDMITX_OS_ID_MASK        0x0000FFFF //~HDMITX_OS_ID_PREFIX_MASK

#define HAS_FLAG(flag, bit)   ((flag) & (bit))
#define SET_FLAG(flag, bit)   ((flag) |= (bit))
#define RESET_FLAG(flag, bit) ((flag) &= (~(bit)))

#define EVENT_MUTEX_LOCK()
#define EVENT_MUTEX_UNLOCK()

#define EN_EVENT_GROUP 0
//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    void (*pfnFunc)(void *nDataAddr);
} DrvHdmitxOsTimerNotifyFunc_t;

typedef struct
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

u16 g_u16I2cId     = 0;
u16 g_u16I2cUsed   = 1;
u8  g_u8HpdGpioPin = 0;

extern u8 g_u8SwI2cSdaPin;
extern u8 g_u8SwI2cSclPin;
extern u8 g_bSwI2cEn;
//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------
u8 DrvHdmitxOsInit(void)
{
    if (g_bHdmitxOsInit == 0)
    {
        g_bHdmitxOsInit = 1;
    }
    else
    {
    }
    return TRUE;
}

void *DrvHdmitxOsMemAlloc(u32 u32Size)
{
    return malloc(u32Size);
}

void DrvHdmitxOsMemRelease(void *pPtr)
{
    free(pPtr);
}

u32 DrvHdmitxOsGetSystemTime(void)
{
    return 0;
}

u64 DrvHdmitxOsGetSystemTimeStamp(void)
{
    return 0;
}

u32 DrvHdmitxOsTimerDiffTimeFromNow(u32 u32TaskTimer) // unit = ms
{
    return (DrvHdmitxOsGetSystemTime() - u32TaskTimer);
}

u8 DrvHdmitxOsCreateTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsDeleteTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsStopTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsStartTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsCreateSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg, u32 u32InitVal)
{
    return TRUE;
}

u8 DrvHdmitxOsDestroySemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg)
{
    return TRUE;
}

void DrvHdmitxOsObtainSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg) {}

void DrvHdmitxOsReleaseSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg) {}

u8 DrvHdmitxOsCreateMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsDestroyMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsObtainMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    return TRUE;
}

u8 DrvHdmitxOsReleaseMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg)
{
    return TRUE;
}

s32 DrvHdmitxOsCreateEventGroup(u8 *pEventName)
{
    return 1;
}

u8 DrvHdmitxOsDeleteEventGroup(s32 s32EventGroupId)
{
    return TRUE;
}

u8 DrvHdmitxOsSetEvent(s32 s32EventGroupId, u32 u32EventFlag)
{
    return TRUE;
}

u32 DrvHdmitxOsGetEvent(s32 s32EventGroupId)
{
    return 1;
}

u8 DrvHdmitxOsWaitEvent(s32 s32EventGroupId, u32 u32WaitEventFlag, u32 *pu32RetrievedEventFlag,
                        DrvHdmitxOsEventWaitMoodeType_e eWaitMode, u32 u32WaitMs)
{
    return TRUE;
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
    return TRUE;
}

u8 DrvHdmitxOsDestroyTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg)
{
    return TRUE;
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
    return bRet;
}

u8 DrvHdmitxOsGpioRequestOutput(u8 u8GpioNum)
{
    return TRUE;
}

u8 DrvHdmitxOsGpioRequestInput(u8 u8GpioNum)
{
    return TRUE;
}

u8 DrvHdmitxOsGetGpioValue(u8 u8GpioNum)
{
    return TRUE;
}

u8 DrvHdmitxOsSetGpioValue(u8 u8GpioNum, u8 u8Val)
{
    return TRUE;
}

u8 DrvHdmitxOsGpioIrq(u8 u8GpioNum, INTERRUPT_CB pIsrCb, u8 bEn, void *pData)
{
    return TRUE;
}

void DrvHdmitxOsSetHpdIrq(u8 bEn) {}

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
    mdelay(u32Msec);
}

void DrvHdmitxOsUsSleep(u32 u32Usec)
{
    udelay(u32Usec);
}

void DrvHdmitxOsMsDelay(u32 u32Msec)
{
    mdelay(u32Msec);
}

void DrvHdmitxOsUsDelay(u32 u32Usec)
{
    udelay(u32Usec);
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
u32 DrvHdmitxOsStrToL(u8 *str, u16 u16Base, u32 *pValue)
{
    *pValue = ustrtoul((char *)str, NULL, u16Base);
    return 0;
}
u8 DrvHdmitxOsStrCmp(u8 *str,const char *dstr)
{
    u8 ret;
    ret = strcmp((char *)str, dstr);
    return ret;
}
u8 DrvHdmitxOsTaskShouldStop(void)
{
    u8 bRet = CAM_OS_RESOURCE_BUSY;
    return bRet;
}

void DrvHdmitxOsSetDebugEnable(u8 bEn) {}
