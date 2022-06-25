/*
* mhal_gpio.h- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: karl.xiao <karl.xiao@sigmastar.com.tw>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/
#ifndef _MHAL_GPIO_H_
#define _MHAL_GPIO_H_

#include <MsTypes.h>
#include "gpio.h"


//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

extern void MHal_GPIO_Init(void);
extern void MHal_GPIO_Pad_Set(MS_GPIO_NUM u8IndexGPIO);
extern int  MHal_GPIO_PadGroupMode_Set(U32 u32PadMode);
extern int  MHal_GPIO_PadVal_Set(MS_GPIO_NUM u8IndexGPIO, U32 u32PadMode);
extern void MHal_GPIO_Pad_Oen(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Pad_Odn(MS_GPIO_NUM u8IndexGPIO);
extern int MHal_GPIO_Pad_Level(MS_GPIO_NUM u8IndexGPIO);
extern U8 MHal_GPIO_Pad_InOut(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Pull_High(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Pull_Low(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Set_High(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Set_Low(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_Enable_GPIO_INT(MS_GPIO_NUM u8IndexGPIO);
extern int MHal_GPIO_To_Irq(MS_GPIO_NUM u8IndexGPIO);
extern void MHal_GPIO_Set_POLARITY(MS_GPIO_NUM u8IndexGPIO, U8 reverse);
extern void MHal_GPIO_PAD_32K_OUT(U8 u8Enable);
void MHal_GPIO_Set_Input(MS_GPIO_NUM u32IndexGPIO);
void MHal_GPIO_Set_Output(MS_GPIO_NUM u32IndexGPIO);
int MHal_GPIO_Get_InOut(MS_GPIO_NUM u32IndexGPIO);

#endif // _MHAL_GPIO_H_
