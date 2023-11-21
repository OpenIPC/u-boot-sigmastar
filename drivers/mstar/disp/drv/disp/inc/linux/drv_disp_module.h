/*
* drv_disp_module.h- Sigmastar
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

#ifndef _DRV_DISP_MODULE_H_
#define _DRV_DISP_MODULE_H_

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_DISP_DEVICE_COUNT 1
#define DRV_DISP_DEVICE_NAME  "mdisp"

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    dev_t                  tDevNumber;
    int                    refCnt;
    struct cdev            cdev;
    struct file_operations fops;
    struct device *        devicenode;
} DrvDispModuleDevice_t;

//-------------------------------------------------------------------------------------------------
// Varialbe
//-------------------------------------------------------------------------------------------------
#ifdef _DISP_MODULE_C_
DrvDispModuleDevice_t g_tDispDevice = {
    .tDevNumber = 0,
    .refCnt     = 0,
    .devicenode = NULL,
    .cdev =
        {
            .kobj =
                {
                    .name = DRV_DISP_DEVICE_NAME,
                },
            .owner = THIS_MODULE,
        },
    /*
    .fops =
    {
        .open = DrvDispModuleOpen,
        .release = DrvDispModuleRelease,
        .unlocked_ioctl = DrvDispModuleIoctl,
        .poll = DrvDispModulePoll,
    },*/
};

struct class *g_stDispClass     = NULL;
u8 *          g_p8DispClassName = "m_disp_class";

u64 u64Disp_DmaMask = 0xffffffffUL;

struct platform_device g_stDrvDispPlatformDevice = {
    .name = DRV_DISP_DEVICE_NAME,
    .id   = 0,
    .dev  = {.of_node = NULL, .dma_mask = &u64Disp_DmaMask, .coherent_dma_mask = 0xffffffffUL}};

#endif

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _DRV_DISP_MODULE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

#undef INTERFACE

#endif
