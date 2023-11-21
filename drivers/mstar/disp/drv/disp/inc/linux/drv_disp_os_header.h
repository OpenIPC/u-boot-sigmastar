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

#include <linux/pfn.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/vmalloc.h> /* seems do not need this */
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/delay.h>
#include <linux/device.h>

#include <asm/io.h>
#include <asm/string.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>

#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/blkdev.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/wait.h>

#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#include "cam_os_wrapper.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#define DISP_IO_OFFSET MS_IO_OFFSET

#define DISP_IO_ADDRESS(x) ((x) + DISP_IO_OFFSET)
//#define __io_address(n)       ((void __iomem __force *)DISP_IO_ADDRESS(n))

/* read register by byte */
#define disp_readb(a) (*(volatile u8 *)DISP_IO_ADDRESS(a))

/* read register by word */
#define disp_readw(a) (*(volatile u16 *)DISP_IO_ADDRESS(a))

/* read register by long */
#define disp_readl(a) (*(volatile u32 *)DISP_IO_ADDRESS(a))

/* write register by byte */
#define disp_writeb(v, a) (*(volatile u8 *)DISP_IO_ADDRESS(a) = (v))

/* write register by word */
#define disp_writew(v, a) (*(volatile u16 *)DISP_IO_ADDRESS(a) = (v))

/* write register by long */
#define disp_writel(v, a) (*(volatile u32 *)DISP_IO_ADDRESS(a) = (v))

#define READ_BYTE(x)     disp_readb(x)
#define READ_WORD(x)     disp_readw(x)
#define READ_LONG(x)     disp_readl(x)
#define WRITE_BYTE(x, y) disp_writeb((u8)(y), x)
#define WRITE_WORD(x, y) disp_writew((u16)(y), x)
#define WRITE_LONG(x, y) disp_writel((u32)(y), x)

#define DISP_TIMEZONE_ISR_SUPPORT HAL_DISP_TIMEZONE_ISR_SUPPORT_LINUX
#define DISP_VGA_HPD_ISR_SUPPORT  HAL_DISP_VGA_HPD_ISR_SUPPORT_LINUX
#define DISP_REG_ACCESS_MODE      HAL_DISP_REG_ACCESS_MD_LINUX

#define DISP_OS_STRING_MAX 128

#define DISP_IRQNUM_Default 0

#define DISP_OS_VIR_RIUBASE (DISP_IO_OFFSET + 0x1F000000)
#define DISPDEBUG_SPRINTF(str, size,_fmt, _args...) CamOsSnprintf((char*)str, size, _fmt, ## _args)

//-------------------------------------------------------------------------------------------------
//  Structure & Emu
//-------------------------------------------------------------------------------------------------

// Task
typedef void *(*DISP_TASK_ENTRY_CB)(void *argv);

typedef struct
{
    s32         s32Id;
    CamOsThread stThreadCfg;
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
    s32          s32Id;
    CamOsMutex_t stMutxCfg;
} DrvDispOsMutexConfig_t;

#endif
