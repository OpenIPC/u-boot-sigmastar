/*
* drv_hdmitx_os.h- Sigmastar
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

#ifndef _DRV_HDHMITX_OS_H_
#define _DRV_HDHMITX_OS_H_

#include "drv_hdmitx_os_header.h"
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define HDMITX_OS_EVENTGROUP_MAX 4 //(64)
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

//-------------------------------------------------------------------------------------------------
//  Debug Info Macro
//-------------------------------------------------------------------------------------------------
#define HDMITX_OS_CRITICAL_MSG(x)     x // for dump critical message
#define HDMITX_OS_FATAL_MSG(fmt, ...) printf("[MS_FATAL]: %s: %d \n" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)

#if defined(CAMDRV_DEBUG)
#define HDMITX_OS_DEBUG_MSG(x) x
#elif defined(MS_OPTIMIZE)
#define HDMITX_OS_DEBUG_MSG(x) // retail version remove debug message
#endif

// user-defined warning
#ifdef BLOCK_ASSERT
#define HDMITX_OS_ASSERT(_bool_)                                                                                      \
    {                                                                                                                 \
        if (!(_bool_))                                                                                                \
        {                                                                                                             \
            printf("\033[35m ASSERT FAIL: %s, %s %s %d\033[35m\n", #_bool_, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
            panic("camdriver panic\n");                                                                               \
            *(volatile u32 *)(ULONG_MAX) = 0;                                                                         \
        }                                                                                                             \
    }
#else
#define HDMITX_OS_ASSERT(_bool_)                                                                                      \
    {                                                                                                                 \
        if (!(_bool_))                                                                                                \
        {                                                                                                             \
            printf("\033[35m ASSERT FAIL: %s, %s %s %d\033[35m\n", #_bool_, __FILE__, __PRETTY_FUNCTION__, __LINE__); \
            panic("camdriver panic\n");                                                                               \
        }                                                                                                             \
    }
#endif

//-------------------------------------------------------------------------------------------------
//  MASK, BIT  BMASK Macro
//-------------------------------------------------------------------------------------------------

#define MASK(x) (((1 << (x##_BITS)) - 1) << x##_SHIFT)
#ifndef BIT // for Linux_kernel type, BIT redefined in <linux/bitops.h>
#define BIT(_bit_) (1 << (_bit_))
#endif
#define BIT_(x) BIT(x) //[OBSOLETED] //TODO: remove it later
#define BITS(_bits_, _val_)         ((BIT(((1)?_bits_)+1)-BIT(((0)?_bits_))) & (_val_<<((0)?_bits_)))
#define BMASK(_bits_)               (BIT(((1)?_bits_)+1)-BIT(((0)?_bits_)))


//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#ifdef _DRV_HDMITX_OS_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE u8    DrvHdmitxOsInit(void);
INTERFACE u32   DrvHdmitxOsGetSystemTime(void);
INTERFACE u64   DrvHdmitxOsGetSystemTimeStamp(void);
INTERFACE u32   DrvHdmitxOsTimerDiffTimeFromNow(u32 MS_U32TaskTimer);
INTERFACE u8    DrvHdmitxOsCreateTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg);
INTERFACE u8    DrvHdmitxOsDeleteTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg);
INTERFACE u8    DrvHdmitxOsStopTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg);
INTERFACE u8    DrvHdmitxOsStartTimer(DrvHdmitxOsTimerConfig_t *pTimerCfg);
INTERFACE u8    DrvHdmitxOsCreateMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvHdmitxOsDestroyMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvHdmitxOsObtainMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvHdmitxOsReleaseMutex(DrvHdmitxOsMutexConfig_t *pstMutexCfg);
INTERFACE s32   DrvHdmitxOsCreateEventGroup(u8 *pEventName);
INTERFACE u8    DrvHdmitxOsDeleteEventGroup(s32 s32EventGroupId);
INTERFACE u8    DrvHdmitxOsSetEvent(s32 s32EventGroupId, u32 u32EventFlag);
INTERFACE u32   DrvHdmitxOsGetEvent(s32 s32EventGroupId);
INTERFACE u8    DrvHdmitxOsWaitEvent(s32 s32EventGroupId, u32 u32WaitEventFlag, u32 *pu32RetrievedEventFlag,
                                     DrvHdmitxOsEventWaitMoodeType_e eWaitMode, u32 u32WaitMs);
INTERFACE u8    DrvHdmitxOsAttachInterrupt(u32 u32IntNum, INTERRUPT_CALLBACK pIntCb);
INTERFACE u8    DrvHdmitxOsDetachInterrupt(u32 u32IntNum);
INTERFACE u8    DrvHdmitxOsEnableInterrupt(u32 u32IntNum);
INTERFACE u8    DrvHdmitxOsDisableInterrupt(u32 u32IntNum);
INTERFACE u32   DrvHdmitxOsDisableAllInterrupts(void);
INTERFACE u8    DrvHdmitxOsRestoreAllInterrupts(u32 u32OldInterrupts);
INTERFACE u8    DrvHdmitxOsCreateSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg, u32 u32InitVal);
INTERFACE u8    DrvHdmitxOsDestroySemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg);
INTERFACE void  DrvHdmitxOsObtainSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg);
INTERFACE void  DrvHdmitxOsReleaseSemaphore(DrvHdmitxOsSemConfig_t *pstSemaphoreCfg);
INTERFACE u8    DrvHdmitxOsCreateTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg, TASK_ENTRY_CB pTaskEntry, void *pDataPtr,
                                      u8 *pTaskName, u8 bAuotStart);
INTERFACE u8    DrvHdmitxOsDestroyTask(DrvHdmitxOsTaskConfig_t *pstTaskCfg);
INTERFACE u8    DrvHdmitxOsGetMmioBase(void **ppaBaseAddr, u32 *pu32BaseSize, DrvHdmitxOsMmioType_e enType);
INTERFACE u8    DrvHdmitxOsSetGpioValue(u8 u8GpioNum, u8 u8Val);
INTERFACE u8    DrvHdmitxOsGetGpioValue(u8 u8GpioNum);
INTERFACE u8    DrvHdmitxOsGpioRequestOutput(u8 u8GpioNum);
INTERFACE u8    DrvHdmitxOsGpioRequestInput(u8 u8GpioNum);
INTERFACE u8    DrvHdmitxOsGpioIrq(u8 u8GpioNum, INTERRUPT_CB pIsrCb, u8 bEn, void *pData);
INTERFACE void  DrvHdmitxOsSetHpdIrq(u8 bEn);
INTERFACE u8    DrvHdmitxOsSetI2cAdapter(u16 u16Id);
INTERFACE u8    DrvHdmitxOsSetI2cWriteBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen, u8 bSendStop);
INTERFACE u8    DrvHdmitxOsSetI2cReadBytes(u8 u8SlaveAdr, u8 u8SubAdr, u8 *pBuf, u16 u16BufLen);
INTERFACE void *DrvHdmitxOsMemAlloc(u32 u32Size);
INTERFACE void  DrvHdmitxOsMemRelease(void *pPtr);
INTERFACE void  DrvHdmitxOsMsSleep(u32 u32Msec);
INTERFACE void  DrvHdmitxOsUsSleep(u32 u32Usec);
INTERFACE void  DrvHdmitxOsMsDelay(u32 u32Msec);
INTERFACE void  DrvHdmitxOsUsDelay(u32 u32Usec);
INTERFACE void  DrvHdmitxOsSetI2cId(u16 u16Id);
INTERFACE u16   DrvHdmitxOsGetI2cId(void);
INTERFACE u8    DrvHdmitxOsSetDeviceNode(void *pPlatFormDev);
INTERFACE u8    DrvHdmitxOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize);
INTERFACE u8    DrvHdmitxOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize);
INTERFACE u8    DrvHdmitxOsSetHpdGpinPin(u8 u8Pin);
INTERFACE u8    DrvHdmitxOsGetHpdGpinPin(void);
INTERFACE void  DrvHdmitxOsSetSwI2cPin(u8 u8SdaPin, u8 u8SclPin);
INTERFACE void  DrvHdmitxOsSetSwI2cEn(u8 bEn);
INTERFACE u32   DrvHdmitxOsGetCpuSpeedMhz(void);
INTERFACE void  DrvHdmitxOsSetDebugEnable(u8 bEn);
INTERFACE u32 DrvHdmitxOsStrToL(u8 *str, u16 u16Base, u32 *pValue);
INTERFACE u8 DrvHdmitxOsTaskShouldStop(void);
INTERFACE u8 DrvHdmitxOsStrCmp(u8 *str,const char *dstr);

#undef INTERFACE

#endif
