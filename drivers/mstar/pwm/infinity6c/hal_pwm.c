/*
* hal_pwm.c- Sigmastar
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
#include "MsTypes.h"
#include <common.h>
#include "hal_pwm.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "../drivers/mstar/gpio/infinity6c/gpio.h"
#include "../drivers/mstar/gpio/infinity6c/padmux.h"

static U8 _pwmEnSatus[PWM_NUM] = { 0 };
static U32 _pwmDiv[PWM_NUM] = { 0 };
static U32 _pwmPeriod_ns[PWM_NUM] = { 0 };
static U32 _pwmDuty_ns[PWM_NUM] = { 0 };
static U16 clk_pwm_div[7] = {1, 2, 4, 8, 32, 64, 128};
static U8 _pwmInit = 0;

extern S32 HalPadSetVal(U32 u32PadID, U32 u32Mode);

static pwmPadTbl_t padTbl_0[] =
{
    { PAD_PWM0, PINMUX_FOR_PWM0_MODE_1},
    { PAD_GPIO14, PINMUX_FOR_PWM0_MODE_2},
    { PAD_FUART_RX, PINMUX_FOR_PWM0_MODE_3},
    { PAD_GPIO0, PINMUX_FOR_PWM0_MODE_4},
};

static pwmPadTbl_t padTbl_1[] =
{
    { PAD_PWM1, PINMUX_FOR_PWM1_MODE_1},
    { PAD_GPIO15, PINMUX_FOR_PWM1_MODE_2},
    { PAD_FUART_TX, PINMUX_FOR_PWM1_MODE_3},
    { PAD_GPIO1, PINMUX_FOR_PWM1_MODE_4},
};

static pwmPadTbl_t padTbl_2[] =
{
    { PAD_I2C0_SCL, PINMUX_FOR_PWM2_MODE_1},
    { PAD_FUART_CTS, PINMUX_FOR_PWM2_MODE_2},
    { PAD_PM_GPIO7, PINMUX_FOR_PWM2_MODE_3},
    { PAD_GPIO2, PINMUX_FOR_PWM2_MODE_4},
};

static pwmPadTbl_t padTbl_3[] =
{
    { PAD_I2C0_SDA, PINMUX_FOR_PWM3_MODE_1},
    { PAD_FUART_RTS, PINMUX_FOR_PWM3_MODE_2},
    { PAD_PM_GPIO8, PINMUX_FOR_PWM3_MODE_3},
    { PAD_GPIO3, PINMUX_FOR_PWM3_MODE_4},
};

static pwmPadTbl_t padTbl_4[] =
{
    { PAD_PM_GPIO0, PINMUX_FOR_PWM4_MODE_1},
    { PAD_SPI0_CZ, PINMUX_FOR_PWM4_MODE_2},
    { PAD_GPIO4, PINMUX_FOR_PWM4_MODE_3},
};

static pwmPadTbl_t padTbl_5[] =
{
    { PAD_PM_GPIO1, PINMUX_FOR_PWM5_MODE_1},
    { PAD_SPI0_CK, PINMUX_FOR_PWM5_MODE_2},
    { PAD_GPIO5, PINMUX_FOR_PWM5_MODE_3},
};

static pwmPadTbl_t padTbl_6[] =
{
    { PAD_PM_GPIO2, PINMUX_FOR_PWM6_MODE_1},
    { PAD_SPI0_DI, PINMUX_FOR_PWM6_MODE_2},
    { PAD_GPIO6, PINMUX_FOR_PWM6_MODE_3},
};

static pwmPadTbl_t padTbl_7[] =
{
    { PAD_PM_GPIO3, PINMUX_FOR_PWM7_MODE_1},
    { PAD_SPI0_DO, PINMUX_FOR_PWM7_MODE_2},
    { PAD_GPIO7, PINMUX_FOR_PWM7_MODE_3},
};

static pwmPadTbl_t padTbl_8[] =
{
    { PAD_GPIO0, PINMUX_FOR_PWM8_MODE_1},
    { PAD_GPIO2, PINMUX_FOR_PWM8_MODE_2},
    { PAD_SR_IO14, PINMUX_FOR_PWM8_MODE_3},
};

static pwmPadTbl_t padTbl_9[] =
{
    { PAD_GPIO1, PINMUX_FOR_PWM9_MODE_1},
    { PAD_PWM0, PINMUX_FOR_PWM9_MODE_2},
    { PAD_GPIO14, PINMUX_FOR_PWM9_MODE_3},
    { PAD_SR_IO15, PINMUX_FOR_PWM9_MODE_4},
};

static pwmPadTbl_t padTbl_10[] =
{
    { PAD_GPIO3, PINMUX_FOR_PWM10_MODE_1},
    { PAD_PWM1, PINMUX_FOR_PWM10_MODE_2},
    { PAD_GPIO15, PINMUX_FOR_PWM10_MODE_3},
    {PAD_SR_IO16, PINMUX_FOR_PWM10_MODE_4},
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

void Hal_PWMGetGrpAddr(U32 *u32addr, U32 *u32PwmOffs, U8 u8Id)
{
    if (u8Id >= PWM_NUM)
        return;

    *u32addr    = BASE_REG_PWM_PA;
    *u32PwmOffs = (u8Id < 4) ? (u8Id * 0x80) : ((4 * 0x80) + ((u8Id - 4) * 0x40));
}

void HalPWMEnableGet(U8 u8Id, U8* pu8Val)
{
    *pu8Val = 0;
    if (PWM_NUM <= u8Id)
        return;
    *pu8Val = _pwmEnSatus[u8Id];
}

void HalPWMEnable(U8 u8Id, U8 u8Val, U32 u32Pad)
{
    if (PWM_NUM <= u8Id)
        return;
    HalPWMSetDben(u8Id, 1);
    HalPWMPadSet(u8Id, u32Pad);

    if(u8Val)
    {
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1 << u8Id);
    }
    else
    {
        SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1 << u8Id);
    }
    _pwmEnSatus[u8Id] = u8Val;
}

void HalPWMSetConfig(U8 u8Id, U32 duty,U32 period)
{
    U8 i;
    U16 u16Div = 0;
    U32 common = 0;
    U32 pwmclk = 0;
    U32 periodmax = 0;
    U32 u32Period = 0x00000000;
    U32 u32Duty = 0x00000000;
    U32 U32PwmAddr = 0;
    U32 u32PwmOffs = 0;
    if(!_pwmInit)
    {
        HalPWMAllGrpEnable();
        OUTREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 0x03FF);
        _pwmInit = 1;
    }

    if(u8Id >= PWM_NUM)
        return;

    Hal_PWMGetGrpAddr(&U32PwmAddr, &u32PwmOffs, u8Id);
    pwmclk = PWM_CLK;

    switch(pwmclk)
    {
        case PWM_CLK:
            pwmclk = 3;
            common = 250;
            break;
        default:
            pwmclk = 3;
            common = 250;
    }

    /*      select   div       */
    for(i = 0;i<(sizeof(clk_pwm_div)/sizeof(U16));i++){
        periodmax = (clk_pwm_div[i] * 262144 / pwmclk) * common;
        if(period < periodmax)
        {
            u16Div = clk_pwm_div[i];
            _pwmDiv[u8Id] = clk_pwm_div[i];
            break;
        }
    }

    /*      select   period       */
    if(period < (0xFFFFFFFF / pwmclk))
    {
        u32Period= (pwmclk * period) / (u16Div * common);
        if(((pwmclk * period) % (u16Div * common)) > (u16Div * common / 2))
        {
            u32Period++;
        }
        _pwmPeriod_ns[u8Id] = (u32Period * u16Div * common) / pwmclk;
    }
    else
    {
        u32Period= (period / u16Div) * pwmclk / common;
        u32Period++;
        _pwmPeriod_ns[u8Id] = (u32Period * common / pwmclk) * u16Div;
    }

    /*      select   duty       */
    if(duty == 0)
    {
        if(_pwmEnSatus[u8Id])
        {
            SETREG16(U32PwmAddr + u16REG_SW_RESET, BIT0 << u8Id);
        }
    }
    else
    {
        if(_pwmEnSatus[u8Id])
        {
            CLRREG16(U32PwmAddr + u16REG_SW_RESET, BIT0 << u8Id);
        }
    }

    if(duty < (0xFFFFFFFF / pwmclk))
    {
        u32Duty= (pwmclk * duty) / (u16Div * common);
        if((((pwmclk * duty) % (u16Div * common)) > (u16Div * common / 2)) || (u32Duty == 0))
        {
            u32Duty++;
        }
        _pwmDuty_ns[u8Id] = (u32Duty * u16Div * common) / pwmclk;
    }
    else
    {
        u32Duty= (duty / u16Div) * pwmclk / common;
        u32Duty++;
        _pwmPeriod_ns[u8Id] = (u32Duty * common / pwmclk) * u16Div;
    }

    /*      set  div period duty       */
    u16Div--;
    u32Period--;
    u32Duty--;
    printf("clk=12M, u16Div=%d u32Duty=0x%x u32Period=0x%x\n", u16Div, u32Duty, u32Period);
    OUTREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DIV, (u16Div & 0xFFFF));
    OUTREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_PERIOD_L, (u32Period & 0xFFFF));
    OUTREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_PERIOD_H, ((u32Period >> 16) & 0x3));
    OUTREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DUTY_L, (u32Duty & 0xFFFF));
    OUTREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DUTY_H, ((u32Duty >> 16) & 0x3));
}


void HalPWMGetConfig(U8 u8Id, U32* Duty,U32* Period)
{
    U16 u16Div = 0;
    U32 u32Duty = 0;
    U32 u32Period = 0;
    U32 pwmclk = 0;
    U32 common = 0;
    U32 U32PwmAddr = 0;
    U32 u32PwmOffs = 0;
    Hal_PWMGetGrpAddr(&U32PwmAddr, &u32PwmOffs, u8Id);
    if(u8Id >= PWM_NUM)
        return;

    pwmclk = 12000000;

    switch(pwmclk)
    {
        case 12000000:
            pwmclk = 3;
            common = 250;
            break;
        default:
            pwmclk = 3;
            common = 250;
    }
    u16Div = INREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DIV);
    u16Div++;

    if(Period != NULL)
    {
        if(_pwmPeriod_ns[u8Id] == 0)
        {
            u32Period = INREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_PERIOD_L) | ((INREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_PERIOD_H) & 0x3) << 16);
            if(u32Period)
            {
                u32Period++;
            }
            _pwmPeriod_ns[u8Id] = (u32Period * u16Div * common) / pwmclk;
        }
        *Period = _pwmPeriod_ns[u8Id];
    }

    if(Duty != NULL)
    {
        if(_pwmDuty_ns[u8Id] == 0)
        {
            u32Duty = INREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DUTY_L) | ((INREG16(U32PwmAddr + (u32PwmOffs) + u16REG_PWM_DUTY_H) & 0x3) << 16);
            if(u32Duty)
            {
                u32Duty++;
            }
            _pwmDuty_ns[u8Id] = (u32Duty * u16Div * common) / pwmclk;
        }
        *Duty = _pwmDuty_ns[u8Id];
    }

}


void HalPWMSetDben(U8 u8Id, U8 u8Val)
{
    U32 U32PwmAddr = 0;
    U32 u32PwmOffs = 0;
    Hal_PWMGetGrpAddr(&U32PwmAddr, &u32PwmOffs, u8Id);

    OUTREGMSK16(U32PwmAddr + u32PwmOffs + u16REG_PWM_CTRL, (u8Val<<DBEN_BIT), (0x1<<DBEN_BIT));
    OUTREGMSK16(U32PwmAddr + u32PwmOffs + u16REG_PWM_CTRL, (u8Val<<VDBEN_SW_BIT), (0x1<<VDBEN_SW_BIT));
}

void HalPWMSetPolarity(U8 u8Id, U8 u8Val)
{
    U32 U32PwmAddr = 0;
    U32 u32PwmOffs = 0;
    Hal_PWMGetGrpAddr(&U32PwmAddr, &u32PwmOffs, u8Id);
    OUTREGMSK16(U32PwmAddr + u32PwmOffs + u16REG_PWM_CTRL, (u8Val<<POLARITY_BIT), (0x1<<POLARITY_BIT));
    while((INREG16(U32PwmAddr + u32PwmOffs + u16REG_PWM_CTRL) & (0x1<<POLARITY_BIT)) != (u8Val<<POLARITY_BIT));
}

void HalPWMGetPolarity(U8 u8Id, U8* pu8Val)
{
    U32 U32PwmAddr = 0;
    U32 u32PwmOffs = 0;
    Hal_PWMGetGrpAddr(&U32PwmAddr, &u32PwmOffs, u8Id);
    *pu8Val = (INREG16(U32PwmAddr + u32PwmOffs + u16REG_PWM_CTRL) & (0x1<<POLARITY_BIT)) ? 1 : 0;
}

void HalPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    printf("[%s L%d] grp:%d x%x(%d)\n", __FUNCTION__, __LINE__, u8GroupId, u8Val, u8Val);
    if (u8Val)
        SETREG16(BASE_REG_PWM_PA + u16REG_PWM_DUTY_QE0, (1 << (u8GroupId + u16REG_PWM_DUTY_QE0_SHFT)));
    else
        CLRREG16(BASE_REG_PWM_PA + u16REG_PWM_DUTY_QE0, (1 << (u8GroupId + u16REG_PWM_DUTY_QE0_SHFT)));
}

void HalPWMInit(U8 u8Id)
{
    U32 reset, u32Period;

    if (PWM_NUM <= u8Id)
        return;

    reset = INREG16(BASE_REG_PWM_PA + u16REG_SW_RESET) & (BIT0<<u8Id);
    HalPWMGetConfig(u8Id, NULL ,&u32Period);
    HalPWMDutyQE0(0, 0);

    if ((0 == reset) && (u32Period))
    {
        _pwmEnSatus[u8Id] = 1;
    }
    else
    {
        HalPWMEnable(u8Id, 0, 0xFFFF);//PAD_UNKNOWN
    }
}


void HalPWMPadSet(U8 u8Id, U32 u32Val)
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

int HalGroupEnable(U8 u8GroupId, U8 u8Val)
{
    U16 u32JoinMask = 0X0000;
    if (PWM_GROUP_NUM <= u8GroupId)
        return 0;
    u32JoinMask |= 1 << (u8GroupId + PWM_NUM);

    if (u8Val)
    {
        SETREG16(BASE_REG_PWM_PA+ u16REG_GROUP_ENABLE, (1 << (u8GroupId + u16REG_GROUP_ENABLE_SHFT)));
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, u32JoinMask);
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

void HalPWMAllGrpEnable(void)
{

    if ((INREG16(BASE_REG_CLKGEN_PA + (0x38<<2)) & BIT8)) {
        OUTREGMSK16( BASE_REG_CLKGEN_PA + (0x38<<2), 0x00, BIT8|BIT9|BIT10|BIT11|BIT12);
        printf("[PWM]CfgClk(12M)\r\n");
    }
    #if 0 //pwm group
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
    #endif
    OUTREG16(BASE_REG_CHIPTOP_PA + (0xF*4), 0x0);//close ej_mode
}


