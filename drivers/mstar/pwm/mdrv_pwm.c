/*
* mdrv_pwm.c- Sigmastar
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
//#include "MsTypes.h"

#include <common.h>
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "asm/arch/mach/ms_types.h"
#include "hal_pwm.h"

void DrvPWMSetPolarity(U8 u8Id, U8 u8Val)
{
    HalPWMSetPolarity(u8Id, u8Val);
}

void DrvPWMGetPolarity(U8 u8Id, U8* pu8Val)
{
    HalPWMGetPolarity(u8Id, pu8Val);
}

void DrvPWMSetDben(U8 u8Id, U8 u8Val)
{
    HalPWMSetDben(u8Id, u8Val);
}

void DrvPWMEnableGet(U8 u8Id, U8* pu8Val)
{
    HalPWMEnableGet(u8Id, pu8Val);
}

void DrvPWMPadSet(U8 u8Id, U32 u32Val)
{
    HalPWMPadSet(u8Id, u32Val);
}

void DrvPWMEnable(U8 u8Id, U8 u8Val, U32 u32Pad)
{
    HalPWMEnable(u8Id, u8Val, u32Pad);
}
void DrvPWMSetConfig(U8 u8Id, U32 duty,U32 period)
{
    HalPWMSetConfig(u8Id, duty,period);
}

void DrvPWMGetConfig(U8 u8Id, U32* Duty,U32* Period)
{
    HalPWMGetConfig(u8Id, Duty,Period);
}

void DrvPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    HalPWMDutyQE0(u8GroupId, u8Val);
}

void DrvPWMInit(U8 u8Id)
{
    HalPWMInit(u8Id);
}
