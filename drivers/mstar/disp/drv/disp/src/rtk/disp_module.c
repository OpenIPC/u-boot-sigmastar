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

#include "ms_msys.h"
#include "drv_disp_os.h"
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
#include "drv_disp_if.h"

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

typedef struct DrvDispModuleDevice_s
{
    u32     refCnt;
    void    *devicenode;
} DrvDispModuleDevice_t;

static DrvDispModuleDevice_t g_stDispDevice =
{
    .refCnt = 0,
    .devicenode = NULL,
};

//------------------------------------------------------------------------------------------------
// Variable
//-------------------------------------------------------------------------------------------------

static u32 g_stDrvDispPlatformDevice = 0;

//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------


//==============================================================================
static void _DrvDispGetIrqNum(MSOS_ST_PLATFORMDEVICE *pDev, u8 u8Idx, u8 u8DevId)
{
    u8 *p8Addr = NULL;
    u8 u8IrqCnt;
    u16 u16IrqId;
    u16 u16Cnt, u16Rem;
    u32 u32Val;

    u8IrqCnt = DrvDispIrqGetIrqCount();

    MDrv_SysDesc_GetElementCount(SYSDESC_DEV_disp, SYSDESC_PRO_interrupts_u8, &u16Cnt, &u16Rem);
    p8Addr = DrvDispOsMemAlloc(u16Cnt * sizeof(u8));
    if(p8Addr)
    {
        if(E_SYS_DESC_PASS == MDrv_SysDesc_Read_U8_Array(SYSDESC_DEV_disp, SYSDESC_PRO_interrupts_u8, p8Addr, u16Cnt))
        {
            for(u16IrqId=0; u16IrqId<u8IrqCnt; u16IrqId++)
            {
                if(u16IrqId < u16Cnt)
                {
                    u32Val = (u32)(*(p8Addr+u16IrqId));
                    DrvDispIrqSetIsrNum(u16IrqId, u32Val);
                    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Get disp_%d number = 0x%x\n", __FUNCTION__, __LINE__, u16IrqId, u32Val);
                }
            }
        }
        else
        {
            DISP_ERR("[DISPMODULE] Can't Get DISP_IRQ, Idx=%d, DevId=%d\n", u8Idx, u8DevId);
        }
        DrvDispOsMemRelease(p8Addr);
    }
}

void DrvDispModuleDeviceInit(void)
{

    if(g_stDispDevice.refCnt == 0)
    {
        g_stDispDevice.refCnt++;
    }

    //Get IRQ
    _DrvDispGetIrqNum((void *)&g_stDrvDispPlatformDevice, 0, 0);

    DrvDispOsSetDeviceNode((void *)&g_stDrvDispPlatformDevice);

    DrvDispIfClkOn();
}


void DrvDispModuleDeviceDeInit(void)
{
    if (g_stDispDevice.refCnt)
    {
        g_stDispDevice.refCnt--;
    }

     DrvDispIfClkOff();
}

//-------------------------------------------------------------------------------------------------
// Module functions
//-------------------------------------------------------------------------------------------------

