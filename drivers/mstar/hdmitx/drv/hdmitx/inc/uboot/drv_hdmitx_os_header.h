/*
* drv_hdmitx_os_header.h- Sigmastar
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

#ifndef _DRV_HDMITX_OS_HEADER_H_
#define _DRV_HDMITX_OS_HEADER_H_

#include "cam_os_wrapper.h"
#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#ifndef u64
#define u64 unsigned long long
#endif
#ifndef s64
#define s64 signed long long
#endif
#ifndef u32
#define u32 unsigned int
#endif
#ifndef s32
#define s32 signed int
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef u8
#define u8 unsigned char
#endif
#ifndef s8
#define s8 signed char
#endif
#ifndef ss_phys_addr_t
#define ss_phys_addr_t unsigned long long
#endif
#ifndef ss_miu_addr_t
#define ss_miu_addr_t unsigned long long
#endif
#ifndef bool
#define bool unsigned char
#endif
#define CamOsPrintf printf

#define HDMITX_SNPRINTF(str, size,_fmt, _args...) snprintf((char*)str, size, _fmt, ## _args)

#define TICK_PER_ONE_MS        (1) // Note: confirm Kernel fisrt
#define HDMITX_OS_WAIT_FOREVER (0xffffff00 / TICK_PER_ONE_MS)

#define HDMITX_OS_MMIO_NONEPM_BASE 0x1F000000UL
#define HDMITX_OS_MMIO_NONEPM_SIZE 0x00200000UL

#define HDMITX_OS_MMIO_PM_BASE 0x1F000000UL
#define HDMITX_OS_MMIO_PM_SIZE 0x00400000UL
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

// Timer
typedef struct DrvHdmitxOsTimerConfig_s
{
    s32   s32Id;
    u8    bCycle;
    u8    bStop;
    u32   u32TimerOut;
    void *pCtx;
    void (*pfnFunc)(u32 stTimer, void *u32Data);
} DrvHdmitxOsTimerConfig_t;

// Mutex

typedef struct DrvHdmitxOsMutexConfig_s
{
    s32 s32Id;
} DrvHdmitxOsMutexConfig_t;

// Semaphore

typedef struct DrvHdmitxOsSemConfig_s
{
    s32 s32Id;
} DrvHdmitxOsSemConfig_t;
typedef struct
{
    s32 argc;
    u8 *argv[100];
} DrvHdmitxOsStrConfig_t;

// Event
typedef enum
{
    E_DRV_HDMITX_OS_EVENT_MD_AND,       ///< Specify all of the requested events are require.
    E_DRV_HDMITX_OS_EVENT_MD_OR,        ///< Specify any of the requested events are require.
    E_DRV_HDMITX_OS_EVENT_MD_AND_CLEAR, ///< Specify all of the requested events are require. If the request are
                                        ///< successful, clear the event.
    E_DRV_HDMITX_OS_EVENT_MD_OR_CLEAR,  ///< Specify any of the requested events are require. If the request are
                                        ///< successful, clear the event.
} DrvHdmitxOsEventWaitMoodeType_e;

typedef struct DrvHdmitxOsEventGroupInfoConfig_s
{
    u8  bUsed;
    u32 u32Waiting;
    u32 u32EventGroup;
    // pthread_cond_t              stSemaphore; // ?????????????
} DrvHdmitxOsEventGroupInfoConfig_t;

// Interrupt

typedef void (*INTERRUPT_CALLBACK)(u32 u32IntNum);
typedef void (*INTERRUPT_CB)(u32 irq, void *dev_instance);

// Task

typedef void *(*TASK_ENTRY_CB)(void *argv);

typedef struct DrvHdmitxOsTaskConfig_s
{
    s32 s32Id;
} DrvHdmitxOsTaskConfig_t;

// MMIO
typedef enum
{
    E_HDMITX_OS_MMIO_PM,
    E_HDMITX_OS_MMIO_NONEPM,
} DrvHdmitxOsMmioType_e;

#endif
