/*
 * hal_pwm.c- Sigmastar
 *
 * Copyright (C) 2018 Sigmastar Technology Corp.
 *
 * Author: ken.chang <ken-ms.chang@sigmastar.com.tw>
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
#include "MsTypes.h"
#include <common.h>
#include "hal_pwm.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "../drivers/mstar/gpio/infinity6e/gpio.h"
#include "../drivers/mstar/gpio/infinity6e/padmux.h"
extern S32 HalPadSetVal(U32 u32PadID, U32 u32Mode);

static pwmPadTbl_t padTbl_0[] =
{
    { PAD_GPIO8, PINMUX_FOR_PWM0_MODE_1},
    { PAD_GPIO0, PINMUX_FOR_PWM0_MODE_2},
    { PAD_PM_GPIO0, PINMUX_FOR_PWM0_MODE_3},
    { PAD_SR1_IO00, PINMUX_FOR_PWM0_MODE_4},
    { PAD_I2C0_SCL, PINMUX_FOR_PWM0_MODE_5},
    { PAD_FUART_RX, PINMUX_FOR_PWM0_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_1[] =
{
    { PAD_GPIO9, PINMUX_FOR_PWM1_MODE_1},
    { PAD_GPIO1, PINMUX_FOR_PWM1_MODE_2},
    { PAD_PM_GPIO1, PINMUX_FOR_PWM1_MODE_3},
    { PAD_SR1_IO01, PINMUX_FOR_PWM1_MODE_4},
    { PAD_I2C0_SDA, PINMUX_FOR_PWM1_MODE_5},
    { PAD_FUART_TX, PINMUX_FOR_PWM1_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_2[] =
{
    { PAD_GPIO10, PINMUX_FOR_PWM2_MODE_1},
    { PAD_GPIO2, PINMUX_FOR_PWM2_MODE_2},
    { PAD_PM_GPIO2, PINMUX_FOR_PWM2_MODE_3},
    { PAD_SR1_IO02, PINMUX_FOR_PWM2_MODE_4},
    { PAD_ETH_LED0, PINMUX_FOR_PWM2_MODE_5},
    { PAD_FUART_CTS, PINMUX_FOR_PWM2_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_3[] =
{
    { PAD_GPIO11, PINMUX_FOR_PWM3_MODE_1},
    { PAD_GPIO3, PINMUX_FOR_PWM3_MODE_2},
    { PAD_PM_GPIO3, PINMUX_FOR_PWM3_MODE_3},
    { PAD_SR1_IO03, PINMUX_FOR_PWM3_MODE_4},
    { PAD_ETH_LED1, PINMUX_FOR_PWM3_MODE_5},
    { PAD_FUART_RTS, PINMUX_FOR_PWM3_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_4[] =
{
    { PAD_GPIO12, PINMUX_FOR_PWM4_MODE_1},
    { PAD_GPIO4, PINMUX_FOR_PWM4_MODE_2},
    { PAD_PM_UART_RX1, PINMUX_FOR_PWM4_MODE_3},
    { PAD_PM_GPIO4, PINMUX_FOR_PWM4_MODE_4},
    { PAD_I2S0_BCK, PINMUX_FOR_PWM4_MODE_5},
    { PAD_PM_SPI_CZ, PINMUX_FOR_PWM4_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_5[] =
{
    { PAD_GPIO13, PINMUX_FOR_PWM5_MODE_1},
    { PAD_GPIO5,  PINMUX_FOR_PWM5_MODE_2},
    { PAD_PM_UART_TX1, PINMUX_FOR_PWM5_MODE_3},
    { PAD_PM_GPIO5, PINMUX_FOR_PWM5_MODE_4},
    { PAD_I2S0_WCK, PINMUX_FOR_PWM5_MODE_5},
    { PAD_PM_SPI_CK, PINMUX_FOR_PWM5_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_6[] =
{
    { PAD_GPIO14,  PINMUX_FOR_PWM6_MODE_1},
    { PAD_GPIO6,  PINMUX_FOR_PWM6_MODE_2},
    { PAD_PM_I2CM_SCL,  PINMUX_FOR_PWM6_MODE_3},
    { PAD_PM_GPIO6,  PINMUX_FOR_PWM6_MODE_4},
    { PAD_I2S0_DI, PINMUX_FOR_PWM6_MODE_5},
    { PAD_PM_SPI_DI,  PINMUX_FOR_PWM6_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_7[] =
{
    { PAD_GPIO15, PINMUX_FOR_PWM7_MODE_1},
    { PAD_GPIO7, PINMUX_FOR_PWM7_MODE_2},
    { PAD_PM_I2CM_SDA,  PINMUX_FOR_PWM7_MODE_3},
    { PAD_PM_GPIO7,  PINMUX_FOR_PWM7_MODE_4},
    { PAD_I2S0_DO,  PINMUX_FOR_PWM7_MODE_5},
    { PAD_PM_SPI_DO,  PINMUX_FOR_PWM7_MODE_6},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_8[] =
{
    { PAD_PM_GPIO10, PINMUX_FOR_PWM8_MODE_1},
    { PAD_SD0_GPIO0, PINMUX_FOR_PWM8_MODE_2},
    { PAD_ETH_LED1, PINMUX_FOR_PWM8_MODE_3},
    { PAD_SR1_IO00, PINMUX_FOR_PWM8_MODE_4},
    { PAD_GPIO12,  PINMUX_FOR_PWM8_MODE_5},
    { PAD_I2C0_SCL, PINMUX_FOR_PWM8_MODE_6},
    { PAD_SR0_IO13, PINMUX_FOR_PWM8_MODE_7},
    { PAD_PM_GPIO0, PINMUX_FOR_PWM8_MODE_8},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_9[] =
{
    { PAD_FUART_CTS, PINMUX_FOR_PWM9_MODE_2},
    { PAD_PM_GPIO9, PINMUX_FOR_PWM9_MODE_1},
    { PAD_ETH_LED0, PINMUX_FOR_PWM9_MODE_3},
    { PAD_SR1_IO12, PINMUX_FOR_PWM9_MODE_4},
    { PAD_GPIO13, PINMUX_FOR_PWM9_MODE_5},
    { PAD_I2C0_SDA, PINMUX_FOR_PWM9_MODE_6},
    { PAD_I2S0_MCLK, PINMUX_FOR_PWM9_MODE_7},
    { PAD_PM_GPIO1, PINMUX_FOR_PWM9_MODE_8},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t* padTbl[] =
{
    padTbl_0,
    padTbl_1,
    padTbl_2,
    padTbl_3,
    padTbl_4,
    padTbl_5,
    padTbl_6,
    padTbl_7,
    padTbl_8,
    padTbl_9,
};

void halPWMPadSet(U8 u8Id, U32 u32Val)
{
    pwmPadTbl_t* pTbl = NULL;

    if (PWM_NUM <= u8Id)
    {
        return;
    }

    printf("[%s][%d] (pwmId, padId) = (%d, %d)\n", __FUNCTION__, __LINE__, u8Id, u32Val);

    pTbl = padTbl[u8Id];
    while (1)
    {
        if (u32Val == pTbl->u32PadId)
        {
            if (BASE_REG_NULL != pTbl->u32Mode)
            {
                HalPadSetVal(u32Val, pTbl->u32Mode);
            }
            break;
        }
        if (PAD_UNKNOWN == pTbl->u32PadId)
        {
            printf("[%s][%d] void DrvPWMEnable error!!!! (%x, %x)\r\n", __FUNCTION__, __LINE__, u8Id, u32Val);
            break;
        }
        pTbl++;
    }
}

int halGroupEnable(U8 u8GroupId, U8 u8Val)
{
    U16 u32JoinMask = 0X0000;
    if (PWM_GROUP_NUM <= u8GroupId)
        return 0;
    u32JoinMask |= 1 << (u8GroupId + PWM_NUM);

    if (u8Val)
    {
        SETREG16(BASE_REG_PWM_PA+ u16REG_GROUP_ENABLE, (1 << (u8GroupId + u16REG_GROUP_ENABLE_SHFT)));
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, u32JoinMask);
//        MDEV_PWM_SetSyncFlag(1); //dont need to sync until new data in
    }
    else
    {
        CLRREG16(BASE_REG_PWM_PA + u16REG_GROUP_ENABLE, (1 << (u8GroupId + u16REG_GROUP_ENABLE_SHFT)));
        SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, u32JoinMask);
    }
    return 1;
}

int halPWMGroupJoin(U8 u8PWMId, U8 u8Val)
{
    if (PWM_NUM <= u8PWMId)
        return 0;
    if(u8Val)
    {
        SETREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    }
    else
    {
        if((u8PWMId%PWM_PER_GROUP)==0)
        {
            printf("pwm0 pwm4 pwm8 must join group\n");
        }
        CLRREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    }
    return 1;
}

void halPWMAllGrpEnable(void)
{
    U8 i=0;

    if ((INREG16(BASE_REG_CLKGEN_PA + (0x38<<2)) & BIT8)) {
        OUTREGMSK16( BASE_REG_CLKGEN_PA + (0x38<<2), 0x00, BIT8|BIT9|BIT10|BIT11|BIT12);
        printf("[PWM]CfgClk(12M)\r\n");
    }
    for (i = 0; i < PWM_GROUP_NUM; i++) {
        //[interrupt function]
        //Each group bit0 must enable for interrupt function
        //please see sync mode description for detail
        //SW owner default need to enable h'74 bit0, bit4, bit8
        halPWMGroupJoin((i*PWM_PER_GROUP), 1);
        OUTREG16(BASE_REG_PWM_PA + u16REG_GROUP_JOIN,0x0111);
        //Suggest driver owner default open each group_enable(h'73) and
        //each group bit0 sync_mode for general mode.
        halGroupEnable(i, 1);
    //printk(KERN_NOTICE "[NOTICE]Each grp enable must be enabled!\r\n");
    }
    OUTREG16(BASE_REG_CHIPTOP_PA + (0xF*4), 0x0);//close ej_mode
}
void halPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    printf("[%s L%d] grp:%d x%x(%d)\n", __FUNCTION__, __LINE__, u8GroupId, u8Val, u8Val);
    if (u8Val)
        SETREG16(BASE_REG_PWM_PA + u16REG_PWM_DUTY_QE0, (1 << (u8GroupId + u16REG_PWM_DUTY_QE0_SHFT)));
    else
        CLRREG16(BASE_REG_PWM_PA + u16REG_PWM_DUTY_QE0, (1 << (u8GroupId + u16REG_PWM_DUTY_QE0_SHFT)));
}

