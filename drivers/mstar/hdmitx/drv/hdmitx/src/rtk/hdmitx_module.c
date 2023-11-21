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

#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "hdmitx_debug.h"
#include "mhal_hdmitx_datatype.h"

#include "drv_sysdesc.h"
#include "device_id.h"
#include "property_id.h"

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------

typedef void MSOS_ST_PLATFORMDEVICE;

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Structure
//-------------------------------------------------------------------------------------------------

typedef struct DrvHdmitxModuleDevice_s
{
    u32   refCnt;
    void *devicenode;
} DrvHdmitxModuleDevice_t;

static DrvHdmitxModuleDevice_t g_tHdmitxDevice = {
    .refCnt     = 0,
    .devicenode = NULL,
};

//------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------

//==============================================================================
static void _DrvHdmitxUpdateHpdGpio(void)
{
    u32 u32Val;

    if (MDrv_SysDesc_Read_U32(SYSDESC_DEV_hdmitx, SYSDESC_PRO_hpd_gpio_u32, &u32Val) == E_SYS_DESC_PASS)
    {
        u8 u8Pin = u32Val & 0xFF;
        DrvHdmitxOsSetHpdGpinPin(u8Pin);
        DrvHdmitxOsGpioRequestInput(u8Pin);
    }
    else
    {
        printf(PRINT_RED "%s %d, No hpd_gpio property \n" PRINT_NONE, __FUNCTION__, __LINE__);
    }
}
static void _DrvHdmixUpdateSWDdcGpio(void)
{
    u32 u32SwI2c  = 0;
    u32 u32I2cSda = 0;
    u32 u32I2cScl = 0;
    if (MDrv_SysDesc_Read_U32(SYSDESC_DEV_hdmitx, SYSDESC_PRO_i2c_sw_u32, &u32SwI2c) != E_SYS_DESC_PASS)
    {
        printf(PRINT_RED "%s %d, No sw i2c property \n" PRINT_NONE, __FUNCTION__, __LINE__);
        u32SwI2c = 0;
    }

    DrvHdmitxOsSetSwI2cEn(u32SwI2c ? TRUE : FALSE);

    if (u32SwI2c)
    {
        if (MDrv_SysDesc_Read_U32(SYSDESC_DEV_hdmitx, SYSDESC_PRO_i2c_sda_gpio_u32, &u32I2cSda) != E_SYS_DESC_PASS)
        {
            printf(PRINT_RED "%s %d, No i2c_sda_gpio property \n" PRINT_NONE, __FUNCTION__, __LINE__);
            u32I2cSda = 0;
        }

        if (MDrv_SysDesc_Read_U32(SYSDESC_DEV_hdmitx, SYSDESC_PRO_i2c_scl_gpio_u32, &u32I2cScl) != E_SYS_DESC_PASS)
        {
            printf(PRINT_RED "%s %d, No i2c_scl_gpio property \n" PRINT_NONE, __FUNCTION__, __LINE__);
            u32I2cScl = 0;
        }
        DrvHdmitxOsSetSwI2cPin(u32I2cSda, u32I2cScl);
    }
}

void DrvHdmitxDeviceInit(void)
{
    if (g_tHdmitxDevice.refCnt == 0)
    {
        g_tHdmitxDevice.refCnt++;
    }

    _DrvHdmitxUpdateHpdGpio();
    _DrvHdmixUpdateSWDdcGpio();
}

void DrvHdmitxDeviceDeInit(void)
{
    if (g_tHdmitxDevice.refCnt)
    {
        g_tHdmitxDevice.refCnt--;
    }
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------
