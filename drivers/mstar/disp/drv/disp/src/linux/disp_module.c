/*
* disp_module.c- Sigmastar
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

#define _DISP_MODULE_C_

#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>

#include "ms_msys.h"
#include "cam_sysfs.h"
#include "drv_disp_os.h"
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
#include "drv_disp_if.h"
//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------

//==============================================================================
static void _DrvDispGetIrqNum(struct platform_device *pDev, u8 u8Idx, u8 u8DevId)
{
    u32 u32DispIrqId = 0; // INT_IRQ_AU_SYSTEM;

    u32DispIrqId = CamOfIrqToResource(pDev->dev.of_node, u8Idx, NULL);

    if (!u32DispIrqId)
    {
        DISP_ERR("[DISPMODULE] Can't Get SCL_IRQ, Idx=%d, DevId=%d\n", u8Idx, u8DevId);
    }
    else
    {
        DrvDispIrqSetIsrNum(u8DevId, u32DispIrqId);
    }
}

void DrvDispModuleDeviceInit(void)
{
    u32 s32Ret;
    u8  i, u8IrqCnt;

    if (g_tDispDevice.refCnt == 0)
    {
        g_tDispDevice.refCnt++;

        s32Ret = alloc_chrdev_region(&g_tDispDevice.tDevNumber, 0, 1, DRV_DISP_DEVICE_NAME);

        if (!g_stDispClass)
        {
            g_stDispClass = msys_get_sysfs_class();
            if (!g_stDispClass)
            {
                g_stDispClass = CamClassCreate(THIS_MODULE, g_p8DispClassName);
            }
        }
        else
        {
            cdev_init(&g_tDispDevice.cdev, &g_tDispDevice.fops);
            if (0 != (s32Ret = cdev_add(&g_tDispDevice.cdev, g_tDispDevice.tDevNumber, DRV_DISP_DEVICE_COUNT)))
            {
                DISP_ERR("[DISP] Unable add a character device\n");
            }
        }

        if (g_tDispDevice.devicenode == NULL && g_stDispClass)
        {
            // coverity[array_free]
            g_tDispDevice.devicenode =
                CamDeviceCreate(g_stDispClass, NULL, g_tDispDevice.tDevNumber, NULL, DRV_DISP_DEVICE_NAME);
            DrvDispSysfsInit(g_tDispDevice.devicenode);
        }

        if (g_stDrvDispPlatformDevice.dev.of_node == NULL)
        {
            g_stDrvDispPlatformDevice.dev.of_node = of_find_compatible_node(NULL, NULL, "sstar,disp");
        }
        if (g_stDrvDispPlatformDevice.dev.of_node == NULL)
        {
            DISP_ERR("[DISP INIT] Get Device mode Fail!!\n");
        }

        // Get IRQ
    }
    u8IrqCnt = DrvDispIrqGetIrqCount();
    for (i = 0; i < u8IrqCnt; i++)
    {
        _DrvDispGetIrqNum(&g_stDrvDispPlatformDevice, i, i);
    }

    DrvDispOsSetDeviceNode(&g_stDrvDispPlatformDevice);

    DrvDispIfClkOn();
    DrvDispCtxInit();
}

void DrvDispModuleDeviceDeInit(void)
{
    if (g_tDispDevice.refCnt)
    {
        g_tDispDevice.refCnt--;
    }

    if (g_tDispDevice.refCnt == 0)
    {
        if (g_tDispDevice.cdev.count)
        {
            // Remove a cdev from the system
            cdev_del(&g_tDispDevice.cdev);
        }

        // CamDeviceDestroy(m_pstDispClass, g_tDispDevice.tDevNumber);
        unregister_chrdev_region(g_tDispDevice.tDevNumber, 1);

        g_stDrvDispPlatformDevice.dev.of_node = NULL;
        g_stDispClass                         = NULL;
    }

    DrvDispIfClkOff();
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
