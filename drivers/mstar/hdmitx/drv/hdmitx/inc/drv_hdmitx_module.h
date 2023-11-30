/*
* drv_hdmitx_module.h- Sigmastar
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

#ifndef _DRV_HDMITX_MODULE_H
#define _DRV_HDMITX_MODULE_H

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_HDMITX_DEVICE_COUNT 1
#define DRV_HDMITX_DEVICE_NAME  "mhdmitx"

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------
typedef struct DrvHdmitxModuleDevice_s
{
    dev_t                  tDevNumber;
    int                    refCnt;
    struct cdev            cdev;
    struct file_operations fops;
    struct device *        devicenode;
} DrvHdmitxModuleDevice_t;

//-------------------------------------------------------------------------------------------------
// Varialbe
//-------------------------------------------------------------------------------------------------
#ifdef _HDMITX_MODULE_C_
DrvHdmitxModuleDevice_t g_tHdmitxDevice = {
    .tDevNumber = 0,
    .refCnt     = 0,
    .devicenode = NULL,
    .cdev =
        {
            .kobj =
                {
                    .name = DRV_HDMITX_DEVICE_NAME,
                },
            .owner = THIS_MODULE,
        },
    /*
    .fops =
    {
        .open = DrvHdmitxModuleOpen,
        .release = DrvHdmitxModuleRelease,
        .unlocked_ioctl = DrvHdmitxModuleIoctl,
        .poll = DrvHdmitxModulePoll,
    },*/
};

struct class *g_tHdmitxClass      = NULL;
u8 *          g_u8HdmitxClassName = "m_hdmitx_class";

u64 u64Hdmitx_DmaMask = 0xffffffffUL;

struct platform_device g_stDrvHdmitxPlatformDevice = {
    .name = DRV_HDMITX_DEVICE_NAME,
    .id   = 0,
    .dev  = {.of_node = NULL, .dma_mask = &u64Hdmitx_DmaMask, .coherent_dma_mask = 0xffffffffUL}};

#endif

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------
#ifdef _DRV_HDMITX_MODULE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void DrvHdmitxDeviceInit(void);
INTERFACE void DrvHdmitxDeviceDeInit(void);

#undef INTERFACE

#endif
