/*
* drv_hdmitx_module.c- Sigmastar
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

#define _DRV_HDMITX_MODULE_C_

#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include "ms_msys.h"
#include "cam_os_wrapper.h"
#include "cam_sysfs.h"
#include "hdmitx_debug.h"
#include "drv_hdmitx_os.h"
#include "hdmitx_sysfs.h"
#include "mhal_common.h"
#include "drv_hdmitx_module.h"

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------
static int _DrvHdmitxModuleProbe(struct platform_device *pdev);
static int _DrvHdmitxModuleRemove(struct platform_device *pdev);
static int _DrvHdmitxModuleSuspend(struct platform_device *dev, pm_message_t state);
static int _DrvHdmitxModuleResume(struct platform_device *dev);
//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------
extern DrvHdmitxModuleDevice_t g_tHdmitxDevice;
extern struct class *          g_tHdmitxClass;
extern u8 *                    g_u8HdmitxClassName;
extern struct platform_device  g_stDrvHdmitxPlatformDevice;

struct of_device_id g_tHdmitxMatchTable[] = {{.compatible = "sstar,hdmitx"}, {}};

struct platform_driver g_stDrvHdmitxPlatformDriver = {
    .probe   = _DrvHdmitxModuleProbe,
    .remove  = _DrvHdmitxModuleRemove,
    .suspend = _DrvHdmitxModuleSuspend,
    .resume  = _DrvHdmitxModuleResume,
    .driver =
        {
            .name           = DRV_HDMITX_DEVICE_NAME,
            .owner          = THIS_MODULE,
            .of_match_table = of_match_ptr(g_tHdmitxMatchTable),
        },
};

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------

//==============================================================================
static int _DrvHdmitxModuleSuspend(struct platform_device *dev, pm_message_t state)
{
    int ret = 0;
    return ret;
}

static int _DrvHdmitxModuleResume(struct platform_device *dev)
{
    int ret = 0;
    return ret;
}
/*
static void _DrvHdmitxGetIrqNum(struct platform_device *pDev, u8 u8Idx, u8 u8DevId)
{
    u32 u32HdmitxIrqId = 0; //INT_IRQ_AU_SYSTEM;
    u32HdmitxIrqId  = CamOfIrqToResource(pDev->dev.of_node, u8Idx, NULL);

    if (!u32HdmitxIrqId)
    {
        printf(PRINT_RED "%s %d Can't Get SCL_IRQ\n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
    else
    {
        //DrvHdmitxIrqSetIsrNum(u8DevId, u32HdmitxIrqId);
    }
}
*/

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
static int _DrvHdmitxModuleProbe(struct platform_device *pdev)
{
    g_tHdmitxClass = msys_get_sysfs_class();

    if (IS_ERR(g_tHdmitxClass))
    {
        HDMITX_ERR( "%s %d class_create() fail. Please exec [mknod] before operate the device\n" ,
               __FUNCTION__, __LINE__);
    }

    g_stDrvHdmitxPlatformDevice.dev.of_node = pdev->dev.of_node;

    // create device
    DrvHdmitxDeviceInit();

    return 0;
}

static int _DrvHdmitxModuleRemove(struct platform_device *pdev)
{
    DrvHdmitxDeviceDeInit();

    CamDeviceUnregister(g_tHdmitxDevice.devicenode);
    return 0;
}

int DrvHdmitxModuleInit(void)
{
    int ret = 0;

    ret = CamPlatformDriverRegister(&g_stDrvHdmitxPlatformDriver);
    if (!ret)
    {
    }
    else
    {
        HDMITX_ERR( "%s %d CamPlatformDriverRegister failed\n" , __FUNCTION__, __LINE__);
        CamPlatformDriverUnregister(&g_stDrvHdmitxPlatformDriver);
    }

    return ret;
}
void DrvHdmitxModuleExit(void)
{
    /*de-initial the who GFLIPDriver */
    CamPlatformDriverUnregister(&g_stDrvHdmitxPlatformDriver);
}

module_init(DrvHdmitxModuleInit);
module_exit(DrvHdmitxModuleExit);

MODULE_AUTHOR("CAMDRIVER");
MODULE_DESCRIPTION("camdriver sclhvsp ioctrl driver");
#ifdef SUPPORT_GPL_SYMBOL
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("PROPRIETARY");
#endif
