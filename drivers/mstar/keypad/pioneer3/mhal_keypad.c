/*
* mhal_keypad.c- Sigmastar
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

#include "mhal_keypad.h"
#include <common.h>

//#include "asm/arch/mach/platform.h"
//#include "asm/arch/mach/io.h"

U32 mhal_KeyPad_SetColNum(U32 num)
{
    U32 tmp;
    if(num >KEYPAD_MAXCOLNUM)
        return KEYPAD_FAILED;
    else
    {
        tmp = KEYPAD_READ(KEYPAD_KEY_NUM);
        tmp &= ~(KEYPAD_COL);
        tmp |= num;
        KEYPAD_WRITE(KEYPAD_KEY_NUM, tmp);
    }
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetRowNum(U32 num)
{
    U32 tmp;
    if(num >KEYPAD_MAXROWNUM)
        return KEYPAD_FAILED;
    else
    {
        tmp = KEYPAD_READ(KEYPAD_KEY_NUM);
        tmp &= ~(KEYPAD_ROW);
        tmp |= (num<<4);

        KEYPAD_WRITE(KEYPAD_KEY_NUM, tmp);
    }
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetGlhrNum(U32 num)
{
    KEYPAD_WRITE(KEYPAD_GLHRM_NUM, num);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetGlhrEN(U32 num,U32 enable)
{
    U32 Remainder,value,tmp;

    if(num >= KEPYAD_MAXKEYNUM)
        return KEYPAD_FAILED;

    Remainder = num%REGBYTENUM;
    value = num/REGBYTENUM;

    if(enable)
    {
        tmp = KEYPAD_READ(KEYPAD_GLHRM_ENABLE0+value);
        tmp |= BIT(Remainder);
        KEYPAD_WRITE(KEYPAD_GLHRM_ENABLE0+value, tmp);
    }
    else
    {
        tmp = KEYPAD_READ(KEYPAD_GLHRM_ENABLE0+value);
        tmp &= ~(BIT(Remainder));
        KEYPAD_WRITE(KEYPAD_GLHRM_ENABLE0+value, tmp);
    }
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetForce(U32 num,U32 enable)
{
    U32 Remainder,value,tmp;

    if(num >= KEPYAD_MAXKEYNUM_TWOSTATUS)
        return KEYPAD_FAILED;

    Remainder = num%REGBYTENUM;
    value = num/REGBYTENUM;

    if(enable)
    {
        tmp = KEYPAD_READ(KEYPAD_FORCE_ENABLE0+value);
        tmp |= BIT(Remainder);

        KEYPAD_WRITE(KEYPAD_FORCE_ENABLE0+value, tmp);
    }
    else
    {
        tmp = KEYPAD_READ(KEYPAD_FORCE_ENABLE0+value);
        tmp &= ~(BIT(Remainder));

        KEYPAD_WRITE(KEYPAD_FORCE_ENABLE0+value, tmp);
    }
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetMask(U32 num,U32 enable)
{
    U32 Remainder,value,tmp;

    if(num >= KEPYAD_MAXKEYNUM_TWOSTATUS)
        return KEYPAD_FAILED;

    Remainder = num%REGBYTENUM;
    value = num/REGBYTENUM;

    if(enable)
    {
        tmp = KEYPAD_READ(KEYPAD_MASK0+value);
        tmp |= BIT(Remainder);

        KEYPAD_WRITE(KEYPAD_MASK0+value, tmp);
    }
    else
    {
        tmp = KEYPAD_READ(KEYPAD_MASK0+value);
        tmp &= ~(BIT(Remainder));

        KEYPAD_WRITE(KEYPAD_MASK0+value, tmp);
    }
    return KEYPAD_SUCCESS;
}

U32 mhal_keypad_pinmux(u32 value)
{
    KEYPAD_GPIO_WRITE(KEYPAD_GPIO_PAMUX, value);
    //KEYPAD_GPIO_WRITE(0x60, 0);//jlink padmux
    //KEYPAD_GPIO_WRITE(0x6F, 0); //i2c padmux
    return 0;
}

U32 mhal_KeyPad_ClearIrq(void)
{
    KEYPAD_WRITE(KEYPAD_CLEANIRQ, TRUE);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_GetFinal_Stage(U32 stage,U32* value)
{
    U32 tmp;
    if( stage >= KEYPAD_MAXROWNUM )
    {
        *value = 0;
        return KEYPAD_FAILED;
    }
    tmp = KEYPAD_READ(KEYPAD_FINAL_STATUS0+stage);
    *value = tmp;
    return KEYPAD_SUCCESS;
}


U32 mhal_KeyPad_GetRaw_Stage(U32 stage,U32* value)
{
    U32 tmp;
    if( stage >= KEYPAD_MAXROWNUM )
    {
        *value = 0;
        return KEYPAD_FAILED;
    }
    tmp = KEYPAD_READ(KEYPAD_RAW_STATUS0+stage);
    *value = tmp;

    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetRawSel(KEYPAD_RAW_MODE num)
{
    KEYPAD_WRITE(KEYPAD_RAW_SEL, num);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_EnableScan(bool Enable)
{
    if(Enable)
        KEYPAD_WRITE(KEYPAD_SCAN_EN, TRUE);
    else
        KEYPAD_WRITE(KEYPAD_SCAN_EN, FALSE);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_RST(bool Enable)
{
    if(Enable)
        KEYPAD_WRITE(KEYPAD_SW_RST, FALSE);
    else
        KEYPAD_WRITE(KEYPAD_SW_RST, TRUE);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetScanDfs(U32 ScanClk)
{
    KEYPAD_WRITE(KEYPAD_SCAN_DFS_CFG, ScanClk);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_SetGlhrmDfs(U32 GlhClk)
{
    KEYPAD_WRITE(KEYPAD_GLHRM_DFS_CFG, GlhClk);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_ModeSel(KEYPAD_STATUS_MODE mode)
{
    KEYPAD_WRITE(KEYPAD_MODE_SEL, (U32)mode);
    return KEYPAD_SUCCESS;

}

U32 mhal_KeyPad_Mode3_sel(KEYPAD_MODE3_SEL mode)
{
    U32 tmp;
    tmp = KEYPAD_READ(KEYPAD_MODE_SEL);
    tmp |= (mode<<4);

    KEYPAD_WRITE(KEYPAD_MODE_SEL, (U32)mode);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_Get_Mode3_sel(void)
{
    return (KEYPAD_READ(KEYPAD_MODE_SEL)>>4)&0xf;
}
U32 mhal_KeyPad_Dummy(bool dummy)
{
    KEYPAD_WRITE(KEYPAD_DUMMY, dummy);
    return KEYPAD_SUCCESS;
}

U32 mhal_KeyPad_GetTwoStatusFlag(void)
{
    U32 tmp;
    tmp = KEYPAD_READ(KEYPAD_MODE_STATUS_FLAG) & BIT(0);
    return tmp;
}

U32 mhal_KeyPad_KEYPAD_ALIGN_NUM(U32 num)
{
    KEYPAD_WRITE(KEYPAD_ALIGN_NUM,num);
    return KEYPAD_SUCCESS;
}



void mhal_KeyPad_CLK(bool isEnable)
{
    U16 u16tmp;

    u16tmp = KEYPAD_CLK_READ(KEYPAD_CLK);

    if(isEnable)
        u16tmp &=0xFFF0;
    else
        u16tmp |=0x0001;

    KEYPAD_CLK_WRITE(KEYPAD_CLK,u16tmp);
    return;
}


