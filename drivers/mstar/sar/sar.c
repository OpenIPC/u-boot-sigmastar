/*
* sar.c- Sigmastar
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

#include "sar.h"
static u32 _gMIO_MapBase = 0x1F002800;
static u8 m_u8Init = 0;
static u8 m_u8First = 1;

bool HAL_SAR_Write2Byte(u32 u32RegAddr, u16 u16Val)
{
    (*(volatile u32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2))) = u16Val;
    return TRUE;
}

u16 HAL_SAR_Read2Byte(u32 u32RegAddr)
{
    return (*(volatile u32*)(_gMIO_MapBase+((u32RegAddr & 0xFFFFFF00ul) << 1) + (((u32RegAddr & 0xFF)/ 2) << 2)));
}

bool HAL_SAR_Write2ByteMask(u32 u32RegAddr, u16 u16Val, u16 u16Mask)
{
    u16Val = (HAL_SAR_Read2Byte(u32RegAddr) & ~u16Mask) | (u16Val & u16Mask);
    //sarDbg("sar IOMap base:%16llx u16Val:%4x\n", _gMIO_MapBase, u16Val);
    HAL_SAR_Write2Byte(u32RegAddr, u16Val);
    return TRUE;
}

void ms_sar_hw_init(void)
{
    if (!m_u8Init) {

        HAL_SAR_Write2Byte(REG_SAR_CTRL0,0x0a20);
        //HAL_SAR_Write2Byte(REG_SAR_CKSAMP_PRD,0x0005);
        //HAL_SAR_Write2ByteMask(REG_SAR_CTRL0,0x4000,0x4000);
        m_u8Init = 1;
    }
}
int ms_sar_get(int ch)
{
    u16 value=0;
    u32 count=0;
    RETRY:
    HAL_SAR_Write2ByteMask(REG_SAR_CTRL0,BIT14, 0x4000);
    while(HAL_SAR_Read2Byte(REG_SAR_CTRL0)&BIT14 && count<100000)
    {
        udelay(1);
        count++;
    }
    switch(ch)
    {
    case 0:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT0, BIT0);
        value=HAL_SAR_Read2Byte(REG_SAR_CH1_DATA);
        break;
    case 1:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT1, BIT1);
        value=HAL_SAR_Read2Byte(REG_SAR_CH2_DATA);
        break;
    case 2:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT2, BIT2);
        value=HAL_SAR_Read2Byte(REG_SAR_CH3_DATA);
        break;
    case 3:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT3, BIT3);
        value=HAL_SAR_Read2Byte(REG_SAR_CH4_DATA);
        break;
    case 4:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT4, BIT4);
        value=HAL_SAR_Read2Byte(REG_SAR_CH5_DATA);
        break;
    case 5:
        HAL_SAR_Write2ByteMask(REG_SAR_AISEL_CTRL, BIT5, BIT5);
        value=HAL_SAR_Read2Byte(REG_SAR_CH6_DATA);
        break;
    default:
        printf("error channel,support SAR0,SAR1,SAR2,SAR3,SAR4,SAR5\n");
        return -1;
    }
    if(m_u8First)
    {
        m_u8First = 0;
        udelay(1000);
        goto RETRY;
    }
    return  value;
}
