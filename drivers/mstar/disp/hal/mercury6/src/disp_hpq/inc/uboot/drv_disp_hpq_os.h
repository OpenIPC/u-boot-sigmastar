/*
* drv_disp_hpq_os.h- Sigmastar
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

#ifndef __DRV_SCL_OS_H__
#define __DRV_SCL_OS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#if 0
//-------------------------------------------------------------------------------------------------
// Defines
//-------------------------------------------------------------------------------------------------
#define SCLOS_TASK_MAX           32//(32+120)
#define SCLOS_WORKQUEUE_MAX      8//(8)
#define SCLOS_WORK_MAX           32//(8)
#define SCLOS_TASKLET_MAX        32//(8)
#define SCLOS_MUTEX_MAX          32//(64+240)
#define SCLOS_SPINLOCK_MAX       8
#define SCLOS_EVENTGROUP_MAX     32//(64)
#define SCLOS_TIMER_MAX          (32)
#define SCLOS_CLK_MAX            (E_DRV_SCLOS_CLK_ID_NUM)

#define SCL_DELAY2FRAMEINDOUBLEBUFFERMode 0
#define SCL_DELAYFRAME (DrvSclOsGetSclFrameDelay())

#define _Phys2Miu(phys) ((phys&MIU0_BASE) ? (u32)(phys - MIU0_BASE) :(u32)(phys))

#define SCOS_DRIVER_MEMORY_BASE   0xC0000000
//-------------------------------------------------------------------------------------------------
// Macros
//-------------------------------------------------------------------------------------------------
//time and clock macros
#define TICK_PER_ONE_MS     (1) //Note: confirm Kernel fisrt
#define SCLOS_WAIT_FOREVER   (0xffffff00/TICK_PER_ONE_MS)

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
#define  u64    unsigned long long
#define  s64    signed long long
#define  u32    unsigned long
#define  s32    signed long
#define  u16    unsigned short
#define  s16    signed short
#define  u8     unsigned char
#define  s8     signed char
#define  bool   unsigned char


#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif

//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
typedef int (*TaskEntry)(void *argv);
typedef irqreturn_t ( *InterruptCb ) (u32 eIntNum, void* dev_id);
typedef void ( *SignalCb ) (u32 u32Signals);        ///< Signal callback function
typedef void ( *TimerCb ) (u32 u32StTimer, u32 u32TimerID);  ///< Timer callback function  u32StTimer: not used; u32TimerID: Timer ID

typedef  dma_addr_t DrvSclOsDmemBusType_t;

/// Task priority
typedef enum
{
    E_TASK_PRI_SYS      = 0,    ///< System priority task   ( interrupt level driver, e.g. TSP, SMART )
    E_TASK_PRI_HIGHEST  = 4,    ///< Highest priority task  ( background monitor driver, e.g. DVBC, HDMI )
    E_TASK_PRI_HIGH     = 8,    ///< High priority task     ( service task )
    E_TASK_PRI_MEDIUM   = 12,   ///< Medium priority task   ( application task )
    E_TASK_PRI_LOW      = 16,   ///< Low priority task      ( nonbusy application task )
    E_TASK_PRI_LOWEST   = 24,   ///< Lowest priority task   ( idle application task )
} TaskPriority;

/// Suspend type
typedef enum
{
    E_DRV_SCLOS_PRIORITY,            ///< Priority-order suspension
    E_DRV_SCLOS_FIFO,                ///< FIFO-order suspension
} DrvSclOsAttributeType_e;


typedef enum
{
    E_DRV_SCLOS_INIT_NONE   = 0x00000000,
    E_DRV_SCLOS_INIT_HVSP_1 = 0x00000001,
    E_DRV_SCLOS_INIT_HVSP_2 = 0x00000002,
    E_DRV_SCLOS_INIT_HVSP_3 = 0x00000004,
    E_DRV_SCLOS_INIT_DMA_1  = 0x00000008,
    E_DRV_SCLOS_INIT_DMA_2  = 0x00000010,
    E_DRV_SCLOS_INIT_DMA_3  = 0x00000020,
    E_DRV_SCLOS_INIT_DMA_4  = 0x00000040,
    E_DRV_SCLOS_INIT_PNL    = 0x00000080,
    E_DRV_SCLOS_INIT_VIP    = 0x00000100,
    E_DRV_SCLOS_INIT_HVSP   = (E_DRV_SCLOS_INIT_HVSP_1 | E_DRV_SCLOS_INIT_HVSP_2 | E_DRV_SCLOS_INIT_HVSP_3),
    E_DRV_SCLOS_INIT_DMA    = (E_DRV_SCLOS_INIT_DMA_1 | E_DRV_SCLOS_INIT_DMA_2 | E_DRV_SCLOS_INIT_DMA_3 | E_DRV_SCLOS_INIT_DMA_4),
    E_DRV_SCLOS_INIT_ALL    = (E_DRV_SCLOS_INIT_HVSP | E_DRV_SCLOS_INIT_DMA | E_DRV_SCLOS_INIT_PNL | E_DRV_SCLOS_INIT_VIP),
} DrvSclosProbeType_e;

/// Message size type
typedef enum
{
    E_DRV_SCLOS_MSG_FIXED_SIZE,           ///< Fixed size message
    E_DRV_SCLOS_MSG_VAR_SIZE,             ///< Variable size message
} DrvSclOsMessageType_e;

/// Event mode
typedef enum
{
    E_DRV_SCLOS_EVENT_MD_AND,                      ///< Specify all of the requested events are require.
    E_DRV_SCLOS_EVENT_MD_OR,                       ///< Specify any of the requested events are require.
    E_DRV_SCLOS_EVENT_MD_AND_CLEAR,                ///< Specify all of the requested events are require. If the request are successful, clear the event.
    E_DRV_SCLOS_EVENT_MD_OR_CLEAR,                 ///< Specify any of the requested events are require. If the request are successful, clear the event.
} DrvSclOsEventWaitMoodeType_e;

typedef struct
{
    s32                          iId;
    TaskPriority                    ePriority;
    void                            *pStack;
    u32                          u32StackSize;
    char szName[16];
} DrvSclOsTaskInfo_t, *PDrvSclOsTaskInfo_t;

///////////paul include for RTOS and LINUX
typedef struct
{
    struct task_struct *pThread;
}DrvSclOsTaskStruct_t;

// CLK
typedef struct clk DrvSclOsClkStruct_t;

typedef enum
{
    E_DRV_SCLOS_CLK_ID_HVSP1,
    E_DRV_SCLOS_CLK_ID_HVSP2,
    E_DRV_SCLOS_CLK_ID_HVSP3,
    E_DRV_SCLOS_CLK_ID_DMA1,
    E_DRV_SCLOS_CLK_ID_DMA2,
    E_DRV_SCLOS_CLK_ID_DMA3,
    E_DRV_SCLOS_CLK_ID_DMA4,
    E_DRV_SCLOS_CLK_ID_NUM,
}DrvSclOsClkIdType_e;

typedef struct
{
    void *ptIdclk;
    void *ptFclk1;
    void *ptFclk2;
    void *ptOdclk;
}DrvSclOsClkConfig_t;
typedef struct
{
    void *filp;
    void *pWaitQueueHead;
    void *pstPollQueue;
}DrvSclOsPollWaitConfig_t;

typedef struct workqueue_struct DrvSclOsWoorkQueueConfig_t;
typedef struct work_struct DrvSclOsWorkConfig_t;
typedef struct tasklet_struct DrvSclOsTaskletConfig_t;
typedef enum
{
    E_DRV_SCLOS_VIPSETRUle_DEFAULT  = 0,
    E_DRV_SCLOS_VIPSETRUle_CMDQACT  = 0x1,
    E_DRV_SCLOS_VIPSETRUle_CMDQCHECK  = 0x2,
    E_DRV_SCLOS_VIPSETRUle_CMDQALL  = 0x4,
    E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck  = 0x8,
    E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck  = 0x10,
} DrvSclOsVipSetRuleType_e;

#define VIPSETRULE() (DrvSclOsGetVipSetRule())
#define VIPDEFAULTSETRULE E_DRV_SCLOS_VIPSETRUle_CMDQALL
//for OSD bug, need to set same clk freq with fclk1


#define OSDinverseBug 1
#define CLKDynamic 0
#define USE_Utility 1
#define VIR_RIUBASE 0xFD000000

//-------------------------------------------------------------------------------------------------
// Extern Functions
//-------------------------------------------------------------------------------------------------
//
// Init
//
bool DrvSclOsInit (void);


//
// Memory management
//



//
// Task
//
s32 DrvSclOsCreateTask (TaskEntry pTaskEntry, u32 u32TaskEntryData, bool bAutoStart,char *pTaskName);
DrvSclOsTaskStruct_t DrvSclOsGetTaskinfo(s32 s32Id);
bool DrvSclOsDeleteTask (s32 s32TaskId);
bool DrvSclOsSetTaskWork(s32 s32TaskId);
bool DrvSclOsSleepTaskWork(s32 s32TaskId);
void DrvSclOsDelayTask (u32 u32Ms);
void DrvSclOsDelayTaskUs (u32 u32Us);
void DrvSclOsSetProbeInformation(DrvSclosProbeType_e enProbe);
u32  DrvSclOsGetProbeInformation(DrvSclosProbeType_e enProbe);
void DrvSclOsClearProbeInformation(DrvSclosProbeType_e enProbe);

//-------------------------------------------------------------------------------------------------
/// Get thread ID of current thread/process in OS
/// @return : current thread ID
//-------------------------------------------------------------------------------------------------
//
// Mutex
//
#define SCLOS_PROCESS_PRIVATE    0x00000000
#define SCLOS_PROCESS_SHARED     0x00000001
#define SCLOS_MUTEX_NAME_LENGTH   16
s32 DrvSclOsCreateMutex ( DrvSclOsAttributeType_e eAttribute, char *pMutexName, u32 u32Flag);
s32 DrvSclOsCreateSpinlock ( DrvSclOsAttributeType_e eAttribute, char *pMutexName1, u32 u32Flag);

bool DrvSclOsDeleteMutex (s32 s32MutexId);
bool DrvSclOsDeleteSpinlock (s32 s32MutexId);
bool DrvSclOsObtainMutexIrq(s32 s32MutexId);
bool DrvSclOsObtainMutex (s32 s32MutexId, u32 u32WaitMs);
bool DrvSclOsReleaseMutexIrq (s32 s32MutexId);
char * DrvSclOsCheckMutex(char *str,char *end);
bool DrvSclOsReleaseMutex (s32 s32MutexId);
bool DrvSclOsReleaseMutexAll (void);
bool DrvSclOsInfoMutex (s32 s32MutexId, DrvSclOsAttributeType_e *peAttribute, char *pMutexName);

s32  DrvSclOsTsemInit(u32 nVal);
bool DrvSclOsReleaseTsem(s32 s32Id);
bool DrvSclOsObtainTsem(s32 s32Id);
s32  DrvSclOsTsemDeinit(s32 s32Id);


//
// Semaphore
//
s32 DrvSclOsCreateSemaphore (u32 u32InitCnt, DrvSclOsAttributeType_e eAttribute, char *pName);
bool DrvSclOsDeleteSemaphore (s32 s32SemaphoreId);
bool DrvSclOsObtainSemaphore (s32 s32SemaphoreId, u32 u32WaitMs);
bool DrvSclOsReleaseSemaphore (s32 s32SemaphoreId);
bool DrvSclOsInfoSemaphore (s32 s32SemaphoreId, u32 *pu32Cnt, DrvSclOsAttributeType_e *peAttribute, char *pSemaphoreName);


//
// Event management
//
s32 DrvSclOsCreateEventGroup (char *pName);
bool DrvSclOsCreateEventGroupRing (u8 u8Id);
bool DrvSclOsDeleteEventGroup (s32 s32EventGroupId);
bool DrvSclOsDeleteEventGroupRing (s32 s32EventGroupId);
bool DrvSclOsSetEventIrq (s32 s32EventGroupId, u32 u32EventFlag);
bool DrvSclOsSetEvent (s32 s32EventGroupId, u32 u32EventFlag);
bool DrvSclOsSetEventRing (s32 s32EventGroupId);
u32 DrvSclOsGetEvent(s32 s32EventGroupId);
u32 DrvSclOsGetandClearEventRing(u32 u32EventGroupId);
bool DrvSclOsClearEventIRQ (s32 s32EventGroupId, u32 u32EventFlag);
bool DrvSclOsClearEvent (s32 s32EventGroupId, u32 u32EventFlag);
bool DrvSclOsWaitEvent (s32  s32EventGroupId, u32  u32WaitEventFlag, u32  *pu32RetrievedEventFlag, DrvSclOsEventWaitMoodeType_e eWaitMode, u32  u32WaitMs);
wait_queue_head_t* DrvSclOsGetEventQueue (s32 s32EventGroupId);

//
// Timer management
//
s32   DrvSclOsCreateTimer (TimerCb pTimerCb, u32 u32FirstTimeMs, u32 u32PeriodTimeMs,  bool bStartTimer, char *pName);
bool  DrvSclOsDeleteTimer (s32 s32TimerId);
bool  DrvSclOsStartTimer (s32 s32TimerId);
bool  DrvSclOsStopTimer (s32 s32TimerId);
bool  DrvSclOsResetTimer (s32 s32TimerId, u32 u32FirstTimeMs, u32 u32PeriodTimeMs, bool bStartTimer);
void* DrvSclOsMemalloc(size_t size, gfp_t flags);
bool  DrvSclOsFlushWorkQueue(bool bTask, s32 s32TaskId);
bool  DrvSclOsGetQueueExist(s32 s32QueueId);
bool  DrvSclOsQueueWork(bool bTask, s32 s32TaskId, s32 s32QueueId, u32 u32WaitMs);
bool  DrvSclOsQueueWait(s32 s32QueueId);
s32   DrvSclOsCreateWorkQueueTask(char *pTaskName);
bool  DrvSclOsestroyWorkQueueTask(s32 s32Id);
s32   DrvSclOsCreateWorkQueueEvent(void * pTaskEntry);
bool  DrvSclOsTaskletWork(s32 s32TaskId);
bool  DrvSclOsDisableTasklet (s32 s32Id);
bool  DrvSclOsEnableTasklet (s32 s32Id);
bool  DrvSclOsDestroyTasklet(s32 s32Id);
s32   DrvSclOsCreateTasklet(void * pTaskEntry,u32 u32data);
void  DrvSclOsMemFree(void *pVirAddr);
void* DrvSclOsVirMemalloc(size_t size);
void  DrvSclOsVirMemFree(void *pVirAddr);
void* DrvSclOsMemcpy(void *pstCfg,const void *pstInformCfg,__kernel_size_t size);
void* DrvSclOsMemset(void *pstCfg,int val,__kernel_size_t size);

bool  DrvSclOsClkSetConfig(DrvSclOsClkIdType_e enClkId, DrvSclOsClkConfig_t stClk);
DrvSclOsClkConfig_t *DrvSclOsClkGetConfig(DrvSclOsClkIdType_e enClkId);
u32   DrvSclOsClkGetEnableCount(DrvSclOsClkStruct_t * clk);
DrvSclOsClkStruct_t * DrvSclOsClkGetParentByIndex(DrvSclOsClkStruct_t * clk,u8 index);
u32   DrvSclOsClkSetParent(DrvSclOsClkStruct_t *clk, DrvSclOsClkStruct_t *parent);
u32   DrvSclOsClkPrepareEnable(DrvSclOsClkStruct_t *clk);
void  DrvSclOsClkDisableUnprepare(DrvSclOsClkStruct_t *clk);
u32   DrvSclOsClkGetRate(DrvSclOsClkStruct_t *clk);

u32   DrvSclOsCopyFromUser(void *to, const void *from, u32 n);
u32   DrvSclOsCopyToUser(void *to, const void *from, u32 n);
void  DrvSclOsWaitForCpuWriteToDMem(void);
u8    DrvSclOsGetSclFrameDelay(void);
DrvSclOsVipSetRuleType_e DrvSclOsGetVipSetRule(void);
void  DrvSclOsSetVipSetRule(DrvSclOsVipSetRuleType_e EnSetRule);
void  DrvSclOsSetSclFrameDelay(u8 u8delay);

//
// System time
//
u32  DrvSclOsGetSystemTime (void);
bool DrvSclOsSetPollWait(DrvSclOsPollWaitConfig_t stPollWait);
u64  DrvSclOsGetSystemTimeStamp (void);
u32 DrvSclOsTimerDiffTimeFromNow(u32 u32TaskTimer); ///[OBSOLETE]
u32 DrvSclOsTimerDiffTime(u32 u32Timer, u32 u32TaskTimer); ///[OBSOLETE]
//
// Interrupt management
//
bool DrvSclOsAttachInterrupt (u32 eIntNum, InterruptCb pIntCb,u32 flags,const char *name);
bool DrvSclOsDetachInterrupt (u32 eIntNum);
bool DrvSclOsEnableInterrupt (u32 eIntNum);
bool DrvSclOsDisableInterrupt (u32 eIntNum);
bool DrvSclOsInInterrupt (void);
u32  DrvSclOsDisableAllInterrupts(void);
bool DrvSclOsRestoreAllInterrupts(void);
bool DrvSclOsEnableAllInterrupts(void);

// direct memory
u32 DrvSclOsPa2Miu(DrvSclOsDmemBusType_t addr);
void* DrvSclOsDirectMemAlloc(const char* name, u32 size, DrvSclOsDmemBusType_t *addr);
void DrvSclOsDirectMemFree(const char* name, u32 size, void *virt, DrvSclOsDmemBusType_t addr);

//
// OS Dependent Macro
//

// Worldwide thread safe macro
// Usage:
//     s32 os_X_MutexID;
//     os_X_MutexID = OS_CREATE_MUTEX(_M_);
//     if (os_X_MutexID < 0) {  return FALSE; }
//     if (OS_OBTAIN_MUTEX(os_X_MutexID, 1000) == FALSE) { return FALSE; }
//     ...
//     OS_RELEASE_MUTEX(os_X_MutexID);
//     return X;
//

#define OS_CREATE_MUTEX(_M_)        DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, "OS_"#_M_"_Mutex", SCLOS_PROCESS_SHARED)
#define OS_OBTAIN_MUTEX(_mx, _tm)   DrvSclOsObtainMutex(_mx, _tm)
#define OS_RELEASE_MUTEX(_mx)       DrvSclOsReleaseMutex(_mx)
#define OS_DELETE_MUTEX(_mx)        DrvSclOsDeleteMutex(_mx)
#define OS_DELAY_TASK(_msec)        DrvSclOsDelayTask(_msec)
#define OS_SYSTEM_TIME()            DrvSclOsGetSystemTime()
//#define OS_ENTER_CRITICAL()         DrvSclOsDisableAllInterrupts();
//#define OS_EXIT_CRITICAL()          DrvSclOsEnableAllInterrupts();

//-------------------------------------------------------------------------------------------------
// Virutal/Physial address operation
//-------------------------------------------------------------------------------------------------
u32 DrvSclOsVa2Pa(u32 addr);

#define OS_VA2PA(_addr_)                (u32)DrvSclOsVa2Pa((_addr_))
#endif

//-------------------------------------------------------------------------------------------------
// Debug message
//-------------------------------------------------------------------------------------------------
#define DispPqBinPrintf(_fmt, _args...)       printf(_fmt, ## _args)
#define DrvSclOsScnprintf(buf, size, _fmt, _args...)
//#define printf printk



#ifdef __cplusplus
}
#endif

#endif // __DRV_SCL_OS_H__
