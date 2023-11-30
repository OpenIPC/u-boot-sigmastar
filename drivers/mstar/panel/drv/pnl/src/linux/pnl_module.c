/*
* pnl_module.c- Sigmastar
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

#define _PNL_MODULE_C_

#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include "cam_os_wrapper.h"
#include "cam_sysfs.h"

#include "drv_pnl_os.h"
#include "ms_msys.h"
#include "pnl_sysfs.h"
#include "pnl_debug.h"
#include "drv_pnl_module.h"
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
void _DrvPnlModuleInit(void)
{
    int s32Ret;

    if(_tPnlDevice.refCnt == 0)
    {
        _tPnlDevice.refCnt++;

        s32Ret = alloc_chrdev_region(&_tPnlDevice.tDevNumber, 0, 1, DRV_PNL_DEVICE_NAME);

        if(!_tPnlClass)
        {
            _tPnlClass = msys_get_sysfs_class();
            if(!_tPnlClass)
            {
                _tPnlClass = CamClassCreate(THIS_MODULE, PnlClassName);
            }

        }
        else
        {
            cdev_init(&_tPnlDevice.cdev, &_tPnlDevice.fops);
            if (0 != (s32Ret= cdev_add(&_tPnlDevice.cdev, _tPnlDevice.tDevNumber, DRV_PNL_DEVICE_COUNT)))
            {
                PNL_ERR( "%s %d Unable add a character device\n", __FUNCTION__, __LINE__);
            }
        }

        if(_tPnlDevice.devicenode == NULL)
        {
            _tPnlDevice.devicenode = CamDeviceCreate(_tPnlClass, NULL, _tPnlDevice.tDevNumber, NULL, DRV_PNL_DEVICE_NAME);
            DrvPnlSysfsInit(_tPnlDevice.devicenode);
        }

        if(stDrvPnlPlatformDevice.dev.of_node == NULL)
        {
            stDrvPnlPlatformDevice.dev.of_node = of_find_compatible_node(NULL, NULL, "sstar,pnl");
        }
        if(stDrvPnlPlatformDevice.dev.of_node==NULL)
        {
            PNL_ERR("%s %d:: Get Device mode Fail!!\n", __FUNCTION__, __LINE__);
        }

        DrvPnlOsSetDeviceNode(&stDrvPnlPlatformDevice);
    }
    else
    {
        _tPnlDevice.refCnt++;
    }
}


void _DrvPnlModuleDeInit(void)
{
    if (_tPnlDevice.refCnt)
    {
        _tPnlDevice.refCnt--;
    }

    if (_tPnlDevice.refCnt == 0)
    {
        if (_tPnlDevice.cdev.count)
        {
            // Remove a cdev from the system
            cdev_del(&_tPnlDevice.cdev);
         }

         //CamDeviceDestroy(m_pstPnlClass, _tPnlDevice.tDevNumber);
         unregister_chrdev_region(_tPnlDevice.tDevNumber, 1);

         stDrvPnlPlatformDevice.dev.of_node = NULL;
         _tPnlClass = NULL;
     }
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------

