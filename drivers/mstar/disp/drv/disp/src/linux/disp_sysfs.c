/*
* disp_sysfs.c- Sigmastar
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

#define _DISP_SYSFS_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
//#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/delay.h>
#include <linux/device.h>

#include "ms_msys.h"
#include "ms_platform.h"

#include "cam_sysfs.h"
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "drv_disp_if.h"
#include "hal_disp_if.h"
#include "hal_disp_irq.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "hal_disp_dma.h"
#include "drv_disp_ctx.h"
#include "hal_disp.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _DispSysFsParsingCommand(u8 *p8Str, DrvDispOsStrConfig_t *pstStrCfg)
{
    u8  a8Del[] = " ";
    u32 u32len;

    pstStrCfg->argc                                  = DrvDispOsSplit(pstStrCfg->argv, p8Str, a8Del);
    u32len                                           = strlen(pstStrCfg->argv[pstStrCfg->argc - 1]);
    pstStrCfg->argv[pstStrCfg->argc - 1][u32len - 1] = '\0';
}
//------------------------------------------------------------------------------
ssize_t DrvDispSysfsCheckDbgmgStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugSetDbgmgFlag(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckDbgmgShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugGetDbgmgFlag(buf);
}

static DEVICE_ATTR(dbgmg, 0644, DrvDispSysfsCheckDbgmgShow, DrvDispSysfsCheckDbgmgStore);

ssize_t DrvDispSysfsCheckPqStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);

        DrvDispDebugSetPq(&stStringCfg);

        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckPqShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugGetPqBinName(buf);
}

static DEVICE_ATTR(pq, 0644, DrvDispSysfsCheckPqShow, DrvDispSysfsCheckPqStore);

ssize_t DrvDispSysfsCheckDispClkStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugSetClk(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckClkShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugGetClk(buf);
}

static DEVICE_ATTR(clk, 0644, DrvDispSysfsCheckClkShow, DrvDispSysfsCheckDispClkStore);

ssize_t DrvDispSysfsCheckTurnDrvStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugSetTurnDrv(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckTurnDrvShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugGetTurnDrvInfo(buf);
}

static DEVICE_ATTR(turndrv, 0644, DrvDispSysfsCheckTurnDrvShow, DrvDispSysfsCheckTurnDrvStore);
#if DISP_STATISTIC_EN
ssize_t DrvDispSysfsCheckCfgStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugCfgStore(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckCfgShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugCfgShow(buf);
}

static DEVICE_ATTR(cfg, 0644, DrvDispSysfsCheckCfgShow, DrvDispSysfsCheckCfgStore);

ssize_t DrvDispSysfsCheckDispintsStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugIntsStore(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckDispintsShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugIntsShow(buf);
}

static DEVICE_ATTR(ints, 0644, DrvDispSysfsCheckDispintsShow, DrvDispSysfsCheckDispintsStore);
#endif
ssize_t DrvDispSysfsCheckDispFuncStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvDispOsStrConfig_t stStringCfg;

        _DispSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvDispDebugSetFunc(&stStringCfg);
        return n;
    }
    return 0;
}

ssize_t DrvDispSysfsCheckDispfuncShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return (ssize_t)DrvDispDebugFuncShow(buf);
}

static DEVICE_ATTR(func, 0644, DrvDispSysfsCheckDispfuncShow, DrvDispSysfsCheckDispFuncStore);

void DrvDispSysfsInit(struct device *device)
{
    CamDeviceCreateFile(device, &dev_attr_dbgmg);
    CamDeviceCreateFile(device, &dev_attr_pq);
    CamDeviceCreateFile(device, &dev_attr_clk);
    CamDeviceCreateFile(device, &dev_attr_func);
    CamDeviceCreateFile(device, &dev_attr_turndrv);
#if DISP_STATISTIC_EN
    CamDeviceCreateFile(device, &dev_attr_cfg);
    CamDeviceCreateFile(device, &dev_attr_ints);
#endif
}
