/*
* hdmitx_sysfs.c- Sigmastar
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
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/device.h>

#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "cam_sysfs.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "mhal_hdmitx_datatype.h"
#include "mhal_hdmitx.h"
#include "drv_hdmitx_if.h"
#include "hal_hdmitx_st.h"
#include "drv_hdmitx_ctx.h"
#include "hal_hdmitx_if.h"
#include "regHDMITx.h"

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
static u32 _DrvHdmitxSysFsSplit(u8 **arr, u8 *str, u8 *del)
{
    char *cur   = str;
    u8 *  token = NULL;
    u32   cnt   = 0;

    token = strsep(&cur, del);
    while (token)
    {
        arr[cnt] = token;
        token    = strsep(&cur, del);
        cnt++;
    }
    return cnt;
}

static void _DrvHdmitxSysFsParsingCommand(u8 *str, DrvHdmitxOsStrConfig_t *pstStrCfg)
{
    u8  del[] = " ";
    u32 len;

    pstStrCfg->argc                               = _DrvHdmitxSysFsSplit(pstStrCfg->argv, str, del);
    len                                           = strlen(pstStrCfg->argv[pstStrCfg->argc - 1]);
    pstStrCfg->argv[pstStrCfg->argc - 1][len - 1] = '\0';
}

static ssize_t _DrvHdmitxSysfsDbgmgStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvHdmitxOsStrConfig_t stStringCfg;

        _DrvHdmitxSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvHdmitxDebugDbgmgStore(&stStringCfg);
        return n;
    }
    return 0;
}

static ssize_t _DrvHdmitxSysfsDbgmgShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugDbgmgShow(buf);
}

static DEVICE_ATTR(dbgmg, 0644, _DrvHdmitxSysfsDbgmgShow, _DrvHdmitxSysfsDbgmgStore);

static ssize_t _DrvHdmitxSysfsPtgenStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvHdmitxOsStrConfig_t stStringCfg;

        _DrvHdmitxSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvHdmitxDebugPtGenStore(&stStringCfg);
        return n;
    }
    return 0;
}

static ssize_t _DrvHdmitxSysfsPtgenShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugPtGenShow(buf);
}

static DEVICE_ATTR(ptgen, 0644, _DrvHdmitxSysfsPtgenShow, _DrvHdmitxSysfsPtgenStore);

static ssize_t _DrvHdmitxSysfsClkStore(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if (NULL != buf)
    {
        DrvHdmitxOsStrConfig_t stStringCfg;

        _DrvHdmitxSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvHdmitxDebugClkStore(&stStringCfg);
        return n;
    }
    return 0;
}

static ssize_t _DrvHdmitxSysfsClkShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugClkShow(buf);
}

static DEVICE_ATTR(clk, 0644, _DrvHdmitxSysfsClkShow, _DrvHdmitxSysfsClkStore);

static ssize_t _DrvHdmitxSysfsForceCfgStore(struct device *dev, struct device_attribute *attr, const char *buf,
                                            size_t n)
{
    if (NULL != buf)
    {
        DrvHdmitxOsStrConfig_t stStringCfg;

        _DrvHdmitxSysFsParsingCommand((u8 *)buf, &stStringCfg);
        DrvHdmitxDebugForceCfgStore(&stStringCfg);
        return n;
    }
    return 0;
}

static ssize_t _DrvHdmitxSysfsForceCfgShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugForceCfgShow(buf);
}

static DEVICE_ATTR(forcecfg, 0644, _DrvHdmitxSysfsForceCfgShow, _DrvHdmitxSysfsForceCfgStore);

static ssize_t _DrvHdmitxSysfsCfgShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugCfgShow(buf);
}

static DEVICE_ATTR(cfg, 0444, _DrvHdmitxSysfsCfgShow, NULL);
static ssize_t _DrvHdmitxSysfsEdidShow(struct device *dev, struct device_attribute *attr, char *buf)
{
    return DrvHdmitxDebugEdidShow(buf);
}

static DEVICE_ATTR(edid, 0444, _DrvHdmitxSysfsEdidShow, NULL);

void DrvHdmitxSysfsInit(void *device)
{
    u32 ret;

    ret = CamDeviceCreateFile(device, &dev_attr_dbgmg);
    ret = CamDeviceCreateFile(device, &dev_attr_ptgen);
    ret = CamDeviceCreateFile(device, &dev_attr_clk);
    ret = CamDeviceCreateFile(device, &dev_attr_forcecfg);
    ret = CamDeviceCreateFile(device, &dev_attr_cfg);
    ret = CamDeviceCreateFile(device, &dev_attr_edid);
}
