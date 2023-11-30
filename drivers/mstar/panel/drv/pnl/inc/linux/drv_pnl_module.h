/*
* drv_pnl_module.h- Sigmastar
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

#ifndef _DRV_PNL_MODULE_H_
#define _DRV_PNL_MODULE_H_

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_PNL_DEVICE_COUNT    1
#define DRV_PNL_DEVICE_NAME     "mpnl"

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    dev_t tDevNumber;
    int refCnt;
    struct cdev cdev;
    struct file_operations fops;
    struct device *devicenode;
}DrvPnlModuleDevice_t;


//-------------------------------------------------------------------------------------------------
// Varialbe
//-------------------------------------------------------------------------------------------------
#ifdef _PNL_MODULE_C_

DrvPnlModuleDevice_t _tPnlDevice =
{
    .tDevNumber = 0,
    .refCnt = 0,
    .devicenode = NULL,
    .cdev =
    {
        .kobj = {.name= DRV_PNL_DEVICE_NAME, },
        .owner = THIS_MODULE,
    },
    /*
    .fops =
    {
        .open = DrvPnlModuleOpen,
        .release = DrvPnlModuleRelease,
        .unlocked_ioctl = DrvPnlModuleIoctl,
        .poll = DrvPnlModulePoll,
    },*/
};

struct class * _tPnlClass = NULL;
char * PnlClassName = "m_pnl_class";


u64 u64Pnl_DmaMask = 0xffffffffUL;

struct platform_device stDrvPnlPlatformDevice =
{
    .name = DRV_PNL_DEVICE_NAME,
    .id = 0,
    .dev =
    {
        .of_node = NULL,
        .dma_mask = &u64Pnl_DmaMask,
        .coherent_dma_mask = 0xffffffffUL
    }
};


#endif

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _DRV_PNL_MODULE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif



#undef INTERFACE

#endif
