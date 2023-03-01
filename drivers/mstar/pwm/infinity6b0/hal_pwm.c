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
#include "../drivers/mstar/gpio/infinity6b0/gpio.h"
#include "../drivers/mstar/gpio/infinity6b0/padmux.h"
extern S32 HalPadSetVal(U32 u32PadID, U32 u32Mode);

static pwmPadTbl_t padTbl_0[] =
{
    { PAD_PWM0, PINMUX_FOR_PWM0_MODE},
    { PAD_GPIO14, PINMUX_FOR_PWM0_MODE},
    { PAD_FUART_RX, PINMUX_FOR_PWM0_MODE},
    { PAD_GPIO0, PINMUX_FOR_PWM0_MODE},
    { PAD_SD1_IO0,  PINMUX_FOR_PWM0_MODE},
    { PAD_PM_GPIO0, PINMUX_FOR_PM_PWM0_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_1[] =
{
    { PAD_PWM1, PINMUX_FOR_PWM1_MODE},
    { PAD_GPIO15, PINMUX_FOR_PWM1_MODE},
    { PAD_FUART_TX, PINMUX_FOR_PWM1_MODE},
    { PAD_GPIO1,  PINMUX_FOR_PWM1_MODE},
    { PAD_SD1_IO1, PINMUX_FOR_PWM1_MODE},
    { PAD_PM_GPIO1, PINMUX_FOR_PM_PWM1_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_2[] =
{
    { PAD_GPIO14, PINMUX_FOR_PWM2_MODE},
    { PAD_FUART_CTS, PINMUX_FOR_PWM2_MODE},
    { PAD_SD1_IO0, PINMUX_FOR_PWM2_MODE},
    { PAD_GPIO2, PINMUX_FOR_PWM2_MODE},
    { PAD_SD1_IO2,  PINMUX_FOR_PWM2_MODE},
    { PAD_PM_GPIO2, PINMUX_FOR_PM_PWM2_MODE},
    { PAD_PM_GPIO9, PINMUX_FOR_PM_PWM2_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_3[] =
{
    { PAD_GPIO15,  PINMUX_FOR_PWM3_MODE},
    { PAD_FUART_RTS, PINMUX_FOR_PWM3_MODE},
    { PAD_SD1_IO1, PINMUX_FOR_PWM3_MODE},
    { PAD_GPIO3, PINMUX_FOR_PWM3_MODE},
    { PAD_SD1_IO3, PINMUX_FOR_PWM3_MODE},
    { PAD_PM_GPIO3, PINMUX_FOR_PM_PWM3_MODE},
    { PAD_PM_GPIO7, PINMUX_FOR_PM_PWM3_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_4[] =
{
    { PAD_SD1_IO2, PINMUX_FOR_PWM4_MODE},
    { PAD_SPI0_CZ, PINMUX_FOR_PWM4_MODE},
    { PAD_GPIO4, PINMUX_FOR_PWM4_MODE},
    { PAD_SD1_IO4,  PINMUX_FOR_PWM4_MODE},
    { PAD_PM_LED0, PINMUX_FOR_PM_PWM9_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_5[] =
{
    { PAD_SD1_IO3, PINMUX_FOR_PWM5_MODE},
    { PAD_SPI0_CK,  PINMUX_FOR_PWM5_MODE},
    { PAD_GPIO5, PINMUX_FOR_PWM5_MODE},
    { PAD_SD1_IO5,  PINMUX_FOR_PWM5_MODE},
    { PAD_PM_LED1, PINMUX_FOR_PM_PWM5_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_6[] =
{
    { PAD_SD1_IO4,  PINMUX_FOR_PWM6_MODE},
    { PAD_SPI0_DI,  PINMUX_FOR_PWM6_MODE},
    { PAD_GPIO6, PINMUX_FOR_PWM6_MODE},
    { PAD_SD1_IO6,  PINMUX_FOR_PWM6_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_7[] =
{
    { PAD_SD1_IO5, PINMUX_FOR_PWM7_MODE},
    { PAD_SPI0_DO, PINMUX_FOR_PWM7_MODE},
    { PAD_GPIO7,  PINMUX_FOR_PWM7_MODE},
    { PAD_SD1_IO7, PINMUX_FOR_PWM7_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_8[] =
{
    { PAD_GPIO0, PINMUX_FOR_PWM8_MODE},
    { PAD_GPIO8, PINMUX_FOR_PWM8_MODE},
    { PAD_SD1_IO8, PINMUX_FOR_PWM8_MODE},
    { PAD_SR_IO14,  PINMUX_FOR_PWM8_MODE},
    { PAD_PM_GPIO9, PINMUX_FOR_PM_PWM2_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_9[] =
{
    { PAD_GPIO1, PINMUX_FOR_PWM9_MODE},
    { PAD_PWM0, PINMUX_FOR_PWM9_MODE},
    { PAD_GPIO14, PINMUX_FOR_PWM9_MODE},
    { PAD_SR_IO15, PINMUX_FOR_PWM9_MODE},
    { PAD_PM_LED0, PINMUX_FOR_PM_PWM9_MODE},
    { PAD_UNKNOWN, BASE_REG_NULL},
};

static pwmPadTbl_t padTbl_10[] =
{
    { PAD_PWM1, PINMUX_FOR_PWM10_MODE},
    { PAD_SD1_IO2, PINMUX_FOR_PWM10_MODE},
    { PAD_GPIO15, PINMUX_FOR_PWM10_MODE},
    { PAD_SR_IO16, PINMUX_FOR_PWM10_MODE},
    { PAD_PM_LED1, PINMUX_FOR_PM_PWM10_MODE}, 
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
    padTbl_10,
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
        SETREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    else
        CLRREG16(BASE_REG_CHIPTOP_PA + u16REG_GROUP_JOIN, (1 << (u8PWMId + u16REG_GROUP_JOIN_SHFT)));
    return 1;
}
void halPWMAllGrpEnable(void)
{
    //dummy
}
void halPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    //dummy
}

