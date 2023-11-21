/*
* drv_disp_os_header.h- Sigmastar
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

#ifndef _DRV_DISP_OS_HEADER_H_
#define _DRV_DISP_OS_HEADER_H_

#include "cam_os_wrapper.h"
#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CamOsPrintf printf

#define WRITE_SCL_REG(addr, type, data) ((*(volatile type *)(addr)) = (data))
#define READ_SCL_REG(addr, type)        ((*(volatile type *)(addr)))

#define READ_BYTE(x)     READ_SCL_REG(x, u8)
#define READ_WORD(x)     READ_SCL_REG(x, u16)
#define READ_LONG(x)     READ_SCL_REG(x, u32)
#define WRITE_BYTE(x, y) WRITE_SCL_REG(x, u8, y)
#define WRITE_WORD(x, y) WRITE_SCL_REG(x, u16, y)
#define WRITE_LONG(x, y) WRITE_SCL_REG(x, u32, y)

#define DISP_TIMEZONE_ISR_SUPPORT HAL_DISP_TIMEZONE_ISR_SUPPORT_UBOOT
#define DISP_VGA_HPD_ISR_SUPPORT  HAL_DISP_VGA_HPD_ISR_SUPPORT_UBOOT
#define DISP_REG_ACCESS_MODE      HAL_DISP_REG_ACCESS_MD_UBOOT

#define DISP_OS_STRING_MAX 128

#define DISP_IRQNUM_Default 0

#define DISP_OS_VIR_RIUBASE 0x1F000000
#define DISPDEBUG_SPRINTF(str, size,_fmt, _args...) snprintf((char*)str, size, _fmt, ## _args)
#define CamOsChipRevision() (0)
//-------------------------------------------------------------------------------------------------
//  Structure & Emu
//-------------------------------------------------------------------------------------------------

// Task
typedef void *(*DISP_TASK_ENTRY_CB)(void *argv);

typedef struct
{
    s32 s32Id;
} DrvDispOsTaskConfig_t;

typedef struct
{
    void *pFile;
} DrvDispOsFileConfig_t;

typedef struct
{
    s32 argc;
    u8 *argv[DISP_OS_STRING_MAX];
} DrvDispOsStrConfig_t;

typedef struct
{
    u8 pu8Name[64];
    u8 pu8Value[64];
} DrvDispOsTextItemConfig_t;

typedef struct
{
    DrvDispOsTextItemConfig_t *pstItem;
    u32                        u32Size;
} DrvDispOsTextConfig_t;

typedef struct
{
    s32 s32Id;
} DrvDispOsMutexConfig_t;

#endif
