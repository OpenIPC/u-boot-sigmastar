/*
* drv_disp_module.c- Sigmastar
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

#define _DRV_DISP_MODULE_C_

#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include "ms_msys.h"
#include "drv_disp_os.h"
#include "cam_sysfs.h"
#include "disp_sysfs.h"
#include "disp_debug.h"

#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "drv_disp_irq.h"
#include "drv_disp_module.h"
//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------
static int _DrvDispModuleProbe(struct platform_device *pdev);
static int _DrvDispModuleRemove(struct platform_device *pdev);
static int _DrvDispModuleSuspend(struct platform_device *dev, pm_message_t state);
static int _DrvDispModuleResume(struct platform_device *dev);

extern void DrvDispModuleDeviceInit(void);
extern void DrvDispModuleDeviceDeInit(void);
//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------
extern DrvDispModuleDevice_t  g_tDispDevice;
extern struct class *         g_stDispClass;
extern u8 *                   g_p8DispClassName;
extern struct platform_device g_stDrvDispPlatformDevice;

struct of_device_id g_stDispMatchTable[] = {{.compatible = "sstar,disp"}, {}};

struct platform_driver g_stDrvDispPlatformDriver = {
    .probe   = _DrvDispModuleProbe,
    .remove  = _DrvDispModuleRemove,
    .suspend = _DrvDispModuleSuspend,
    .resume  = _DrvDispModuleResume,
    .driver =
        {
            .name           = DRV_DISP_DEVICE_NAME,
            .owner          = THIS_MODULE,
            .of_match_table = of_match_ptr(g_stDispMatchTable),
        },
};

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------

//==============================================================================
static int _DrvDispModuleSuspend(struct platform_device *dev, pm_message_t state)
{
    u32 ret = 0;
    return ret;
}

static int _DrvDispModuleResume(struct platform_device *dev)
{
    u32 ret = 0;
    DISP_DBG(DISP_DBG_LEVEL_IO, "%s %d\n", __FUNCTION__, __LINE__);
    return ret;
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
static int _DrvDispModuleProbe(struct platform_device *pdev)
{
    g_stDispClass = msys_get_sysfs_class();

    if (IS_ERR(g_stDispClass))
    {
        DISP_ERR("%s %d class_create() fail. Please exec [mknod] before operate the device\n", __FUNCTION__, __LINE__);
    }

    g_stDrvDispPlatformDevice.dev.of_node = pdev->dev.of_node;

    // create device
    DrvDispModuleDeviceInit();

    return 0;
}

static int _DrvDispModuleRemove(struct platform_device *pdev)
{
    DISP_DBG(DISP_DBG_LEVEL_IO, "[DISP] %s\n", __FUNCTION__);

    DrvDispModuleDeviceDeInit();
    CamDeviceUnregister(g_tDispDevice.devicenode);
    return 0;
}

int DrvDispModuleInit(void)
{
    u32 ret = 0;

    DISP_DBG(DISP_DBG_LEVEL_IO, "%s:%d\n", __FUNCTION__, __LINE__);

    ret = CamPlatformDriverRegister(&g_stDrvDispPlatformDriver);
    if (!ret)
    {
        DISP_DBG(DISP_DBG_LEVEL_IO, "%s %d CamPlatformDriverRegister success\n", __FUNCTION__, __LINE__);
    }
    else
    {
        DISP_ERR("%s %d CamPlatformDriverRegister failed\n", __FUNCTION__, __LINE__);
        CamPlatformDriverUnregister(&g_stDrvDispPlatformDriver);
    }

    return ret;
}
void DrvDispModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    DISP_DBG(DISP_DBG_LEVEL_IO, "[DISP] %s\n", __FUNCTION__);
    CamPlatformDriverUnregister(&g_stDrvDispPlatformDriver);
}

module_init(DrvDispModuleInit);
module_exit(DrvDispModuleExit);

MODULE_AUTHOR("CAMDRIVER");
MODULE_DESCRIPTION("camdriver disp ioctrl driver");
#ifdef SUPPORT_GPL_SYMBOL
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("PROPRIETARY");
#endif
