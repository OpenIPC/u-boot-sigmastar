/*
* drv_rgn_os.c- Sigmastar
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

#include "drv_rgn_os.h"
#include "hal_gop.h"


unsigned long gGopDmaThd = 0;

bool _DrvGopTransId(DrvGopIdType_e eGopId, HalGopIdType_e *pHalId);
u8 DrvGopClkDisable(DrvGopIdType_e eGopId,bool bEn);
u8 DrvGopClkEnable(DrvGopIdType_e eGopId);
bool DrvRgnOsSetGopClkEnable(DrvGopIdType_e eGopId)
{
#ifndef CONFIG_CAM_CLK

    HalGopIdType_e enType;

    if(!_DrvGopTransId(eGopId,&enType))
    {
        return 0;
    }
    return HalGopSetClkEnable(enType);
#else
    return DrvGopClkEnable(eGopId);
#endif
}

bool DrvRgnOsSetGopClkDisable(DrvGopIdType_e eGopId,bool bEn)
{
#ifndef CONFIG_CAM_CLK
    HalGopIdType_e enType;

    DrvGopClkDisable(eGopId,bEn);
    if(!_DrvGopTransId(eGopId,&enType))
    {
        return 0;
    }
    return HalGopSetClkDisable(enType,bEn);
#else
    return DrvGopClkDisable(eGopId,bEn);
#endif
}
