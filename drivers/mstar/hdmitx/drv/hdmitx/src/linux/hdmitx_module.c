/*
* hdmitx_module.c- Sigmastar
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

#define _HDMITX_MODULE_C_

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

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------
static void _DrvHdmiTxUpdateHpdIrq(struct platform_device *pDev)
{
    u32 u32HpdInt = 0;
    if (CamofPropertyReadU32(pDev->dev.of_node, "hpd_irq", &u32HpdInt))
    {
        // printf(PRINT_RED "%s %d, No hpd_irq property \n" PRINT_NONE, __FUNCTION__, __LINE__);
        u32HpdInt = 0;
    }
    DrvHdmitxOsSetHpdIrq(u32HpdInt ? 1 : 0);
}

static void _DrvHdmitxUpdateI2cId(struct platform_device *pDev)
{
    u32 u32Val;

    if (CamofPropertyReadU32(pDev->dev.of_node, "i2c_id", &u32Val))
    {
        u32Val = 0;
    }

    DrvHdmitxOsSetI2cId(u32Val);
}

static void _DrvHdmitxUpdateHpdGpio(struct platform_device *pDev)
{
    u32 u32Val;

    if (CamofPropertyReadU32(pDev->dev.of_node, "hpd_gpio", &u32Val))
    {
        HDMITX_ERR( "%s %d, No hpd_gpio property \n" , __FUNCTION__, __LINE__);
    }
    else
    {
        u8 u8Pin = u32Val & 0xFF;
        DrvHdmitxOsSetHpdGpinPin(u8Pin);
        DrvHdmitxOsGpioRequestInput(u8Pin);
    }
}

static void _DrvHdmixUpdateSWDdcGpio(struct platform_device *pDev)
{
    u32 u32SwI2c  = 0;
    u32 u32I2cSda = 0;
    u32 u32I2cScl = 0;
    if (CamofPropertyReadU32(pDev->dev.of_node, "i2c_sw", &u32SwI2c))
    {
        HDMITX_ERR( "%s %d, No sw i2c property \n" , __FUNCTION__, __LINE__);
        u32SwI2c = 0;
    }

    DrvHdmitxOsSetSwI2cEn(u32SwI2c ? TRUE : FALSE);

    if (u32SwI2c)
    {
        if (CamofPropertyReadU32(pDev->dev.of_node, "i2c_sda_gpio", &u32I2cSda))
        {
            HDMITX_ERR( "%s %d, No i2c_sda_gpio property \n" , __FUNCTION__, __LINE__);
            u32I2cSda = 0;
        }

        if (CamofPropertyReadU32(pDev->dev.of_node, "i2c_scl_gpio", &u32I2cScl))
        {
            HDMITX_ERR( "%s %d, No i2c_sda_gpio property \n" , __FUNCTION__, __LINE__);
            u32I2cSda = 0;
        }

        DrvHdmitxOsSetSwI2cPin(u32I2cSda, u32I2cScl);
    }
}

void DrvHdmitxDeviceInit(void)
{
    u32 s32Ret;

    if (g_tHdmitxDevice.refCnt == 0)
    {
        g_tHdmitxDevice.refCnt++;

        s32Ret = alloc_chrdev_region(&g_tHdmitxDevice.tDevNumber, 0, 1, DRV_HDMITX_DEVICE_NAME);

        if (s32Ret < 0)
        {
            HDMITX_ERR( "%s %d major number alloc fail\r\n", __FUNCTION__, __LINE__);
        }

        if (!g_tHdmitxClass)
        {
            g_tHdmitxClass = msys_get_sysfs_class();
            if (!g_tHdmitxClass)
            {
                g_tHdmitxClass = CamClassCreate(THIS_MODULE, g_u8HdmitxClassName);
            }
        }
        else
        {
            cdev_init(&g_tHdmitxDevice.cdev, &g_tHdmitxDevice.fops);
            if (0 != (s32Ret = cdev_add(&g_tHdmitxDevice.cdev, g_tHdmitxDevice.tDevNumber, DRV_HDMITX_DEVICE_COUNT)))
            {
                HDMITX_ERR( "%s %d Unable add a character device\n" , __FUNCTION__, __LINE__);
            }
        }

        if (g_tHdmitxDevice.devicenode == NULL && g_tHdmitxClass)
        {
            // coverity[array_free]
            g_tHdmitxDevice.devicenode =
                CamDeviceCreate(g_tHdmitxClass, NULL, g_tHdmitxDevice.tDevNumber, NULL, DRV_HDMITX_DEVICE_NAME);

            if (NULL == g_tHdmitxDevice.devicenode)
            {
                HDMITX_ERR( "%s %d device_create() fail\n" , __FUNCTION__, __LINE__);
            }

            DrvHdmitxSysfsInit((void *)g_tHdmitxDevice.devicenode);
        }

        if (g_stDrvHdmitxPlatformDevice.dev.of_node == NULL)
        {
            g_stDrvHdmitxPlatformDevice.dev.of_node = of_find_compatible_node(NULL, NULL, "sstar,hdmitx");
        }
        if (g_stDrvHdmitxPlatformDevice.dev.of_node == NULL)
        {
            HDMITX_ERR( "%s %d Get Device mode Fail!!\n" , __FUNCTION__, __LINE__);
        }

        _DrvHdmitxUpdateI2cId(&g_stDrvHdmitxPlatformDevice);
        _DrvHdmitxUpdateHpdGpio(&g_stDrvHdmitxPlatformDevice);
        _DrvHdmiTxUpdateHpdIrq(&g_stDrvHdmitxPlatformDevice);
        _DrvHdmixUpdateSWDdcGpio(&g_stDrvHdmitxPlatformDevice);
        DrvHdmitxOsSetDeviceNode(&g_stDrvHdmitxPlatformDevice);
    }
}

void DrvHdmitxDeviceDeInit(void)
{
    if (g_tHdmitxDevice.refCnt)
    {
        g_tHdmitxDevice.refCnt--;
    }

    if (g_tHdmitxDevice.refCnt == 0)
    {
        if (g_tHdmitxDevice.cdev.count)
        {
            // Remove a cdev from the system
            cdev_del(&g_tHdmitxDevice.cdev);
        }

        // CamDeviceDestroy(m_pstSclClass, g_tHdmitxDevice.tDevNumber);
        unregister_chrdev_region(g_tHdmitxDevice.tDevNumber, 1);

        g_stDrvHdmitxPlatformDevice.dev.of_node = NULL;
        g_tHdmitxClass                          = NULL;
    }
}
