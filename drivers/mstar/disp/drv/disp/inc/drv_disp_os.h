/*
* drv_disp_os.h- Sigmastar
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

#ifndef _DRV_DISP_OS_H_
#define _DRV_DISP_OS_H_

#include "drv_disp_os_header.h"

#ifdef _DRV_DISP_OS_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

INTERFACE void *DrvDispOsMemAlloc(u32 u32Size);
INTERFACE void  DrvDispOsMemRelease(void *pPtr);
INTERFACE void  DrvDispOsMsSleep(u32 u32Msec);
INTERFACE void  DrvDispOsUsSleep(u32 u32Usec);
INTERFACE void  DrvDispOsMsDelay(u32 u32Msec);
INTERFACE void  DrvDispOsUsDelay(u32 u32Usec);
INTERFACE s32   DrvDispOsImiHeapAlloc(u32 u32Size, void **ppAddr);
INTERFACE void  DrvDispOsImiHeapFree(void *pAddr);
INTERFACE u8    DrvDispOsPadMuxActive(void);
INTERFACE u8    DrvDispOsSetDeviceNode(void *pPlatFormDev);
INTERFACE u8    DrvDispOsSetClkOn(void *pClkEn, void *pClkRate, u32 u32ClkRateSize);
INTERFACE u8    DrvDispOsSetClkOff(void *pClkEn, void *pClkRate, u32 u32ClkRateSize);
INTERFACE u8    DrvDispOsCreateTask(DrvDispOsTaskConfig_t *pstTaskCfg, DISP_TASK_ENTRY_CB pTaskEntry, void *pDataPtr,
                                    u8 *pTaskName, u8 bAuotStart);
INTERFACE u8    DrvDispOsDestroyTask(DrvDispOsTaskConfig_t *pstTaskCfg);
INTERFACE u32   DrvDispOsGetSystemTime(void);
INTERFACE u64   DrvDispOsGetSystemTimeStamp(void);
INTERFACE u8    DrvDispOsCreateMutex(DrvDispOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvDispOsDestroyMutex(DrvDispOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvDispOsObtainMutex(DrvDispOsMutexConfig_t *pstMutexCfg);
INTERFACE u8    DrvDispOsReleaseMutex(DrvDispOsMutexConfig_t *pstMutexCfg);
INTERFACE u32   DrvDispOsStrToL(u8 *str, u16 u16Base, u32 *pValue);
INTERFACE s32   DrvDispOsSplit(u8 **arr, u8 *str, u8 *del);
INTERFACE u32   DrvDispOsGetFileSize(DrvDispOsFileConfig_t *pFileCfg);
INTERFACE s32   DrvDispOsWriteFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 writelen);
INTERFACE s32   DrvDispOsOpenFile(u8 *path, s32 flag, s32 mode, DrvDispOsFileConfig_t *pstFileCfg);
INTERFACE s32   DrvDispOsCloseFile(DrvDispOsFileConfig_t *pFileCfg);
INTERFACE s32   DrvDispOsReadFile(DrvDispOsFileConfig_t *pFileCfg, u8 *buf, s32 readlen);
INTERFACE void  DrvDispOsSplitByToken(DrvDispOsStrConfig_t *pstStrCfg, u8 *pBuf, u8 *token);
INTERFACE void  DrvDispOsParsingTxt(u8 *pFileName, DrvDispOsTextConfig_t *pstTxtCfg);
INTERFACE u8    DrvDispOsStrCmp(u8 *str,const char *dstr);
INTERFACE s32   DrvDispOsSetFileSeek(DrvDispOsFileConfig_t *pFileCfg, u32 u32Flag);
INTERFACE u32 DrvDispOsMathDivU64(u64 nDividend, u64 nDivisor, u64 *pRemainder);

#undef INTERFACE

#endif
