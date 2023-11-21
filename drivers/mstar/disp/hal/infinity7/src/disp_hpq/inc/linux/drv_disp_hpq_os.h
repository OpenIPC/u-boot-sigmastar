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

#include "ms_platform.h"
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/poll.h>

#ifndef __DRV_SCL_OS_H__
#define __DRV_SCL_OS_H__

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
// Debug message
//-------------------------------------------------------------------------------------------------
#define DispPqBinPrintf(_fmt, _args...)              CamOsPrintf(KERN_WARNING _fmt, ##_args)
#define DrvSclOsScnprintf(buf, size, _fmt, _args...) scnprintf(buf, size, _fmt, ##_args)
    //#define printf printk

#ifdef __cplusplus
}
#endif

#endif // __DRV_SCL_OS_H__
