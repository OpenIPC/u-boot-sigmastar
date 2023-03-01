/*
 * mdrv_pwm.c- Sigmastar
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
//#include "MsTypes.h"

#include <common.h>
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "hal_pwm.h"

static U8 _pwmEnSatus[PWM_NUM] = { 0 };
//static U32 _pwmPeriod[PWM_NUM] = { 0 };
static U32 _pwmDiv[PWM_NUM] = { 0 };
static U32 _pwmPeriod_ns[PWM_NUM] = { 0 };
static U32 _pwmDuty_ns[PWM_NUM] = { 0 };
static U16 clk_pwm_div[7] = {1, 2, 4, 8, 32, 64, 128};
static U8 _pwmInit = 0;

//------------------------------------------------------------------------------
//
//  Function:   DrvPWMSetPolarity
//
//  Description
//      Set Polarity value
//
//  Parameters
//      u8Id:   [in] PWM ID
//      u8Val:  [in] Polarity value
//
//  Return Value
//      None
//
void DrvPWMSetPolarity(U8 u8Id, U8 u8Val)
{
    OUTREGMSK16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_CTRL, (u8Val<<POLARITY_BIT), (0x1<<POLARITY_BIT));
}

void DrvPWMGetPolarity(U8 u8Id, U8* pu8Val)
{
    *pu8Val = (INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_CTRL) & (0x1<<POLARITY_BIT)) ? 1 : 0;
}

//------------------------------------------------------------------------------
//
//  Function:   DrvPWMSetDben
//
//  Description
//      Enable/Disable Dben function
//
//  Parameters
//      u8Id:   [in] PWM ID
//      u8Val:  [in] On/Off value
//
//  Return Value
//      None
//
void DrvPWMSetDben(U8 u8Id, U8 u8Val)
{
    OUTREGMSK16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_CTRL, (u8Val<<DBEN_BIT), (0x1<<DBEN_BIT));
    OUTREGMSK16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_CTRL, (u8Val<<VDBEN_SW_BIT), (0x1<<VDBEN_SW_BIT));
}

void DrvPWMEnableGet(U8 u8Id, U8* pu8Val)
{
    *pu8Val = 0;
    if (PWM_NUM <= u8Id)
        return;
    *pu8Val = _pwmEnSatus[u8Id];
}

void DrvPWMPadSet(U8 u8Id, U32 u32Val)
{
    halPWMPadSet(u8Id, u32Val);
}

#ifdef CONFIG_INFINITY6E
void DrvPWMEnable(U8 u8Id, U8 u8Val, U32 u32Pad)
{
    if (PWM_NUM <= u8Id)
        return;
    DrvPWMSetDben(u8Id, 1);
    DrvPWMPadSet(u8Id, u32Pad);

    if(u8Val)
    {
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1<<(u8Id==10?0:u8Id));
    }
    else
    {
        SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1<<(u8Id==10?0:u8Id));
    }
    _pwmEnSatus[u8Id] = u8Val;
}
#else
void DrvPWMEnable(U8 u8Id, U8 u8Val, U32 u32Pad)
{
    if (PWM_NUM <= u8Id)
        return;
    DrvPWMSetDben(u8Id, 1);
    DrvPWMPadSet(u8Id, u32Pad);

    if(u8Val)
    {
        CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1<<u8Id);
    }
    else
    {
        SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 1<<u8Id);
    }
    _pwmEnSatus[u8Id] = u8Val;
}
#endif

#ifdef CONFIG_INFINITY6E
void DrvPWMSetConfig(U8 u8Id, U32 duty,U32 period)
{
    U8 i;
    U16 u16Div = 0;
    U32 common = 0;
    U32 pwmclk = 0;
    U32 periodmax = 0;
    U32 u32Period = 0x00000000;
    U32 u32Duty = 0x00000000;
    if(!_pwmInit)
    {
        halPWMAllGrpEnable();
        OUTREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, 0x03FF);
        _pwmInit = 1;
    }

    if(u8Id >= PWM_NUM)
        return;

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
             SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, BIT0<<((u8Id==10)?0:u8Id));
        }
    }
    else
    {
        if(_pwmEnSatus[u8Id])
        {
            CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, BIT0<<((u8Id==10)?0:u8Id));
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
    OUTREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_DIV, (u16Div & 0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_PERIOD_L, (u32Period&0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_PERIOD_H, ((u32Period>>16)&0x3));
    OUTREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_DUTY_L, (u32Duty&0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_DUTY_H, ((u32Duty>>16)&0x3));
}
#else
void DrvPWMSetConfig(U8 u8Id, U32 duty,U32 period)
{
    U8 i;
    U16 u16Div = 0;
    U32 common = 0;
    U32 pwmclk = 0;
    U32 periodmax = 0;
    U32 u32Period = 0x00000000;
    U32 u32Duty = 0x00000000;
    if(!_pwmInit)
    {
        halPWMAllGrpEnable();
        _pwmInit = 1;
    }

    halPWMAllGrpEnable();

    if(u8Id >= PWM_NUM)
        return;

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
             SETREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, BIT0<<((u8Id==10)?0:u8Id));
        }
    }
    else
    {
        if(_pwmEnSatus[u8Id])
        {
            CLRREG16(BASE_REG_PWM_PA + u16REG_SW_RESET, BIT0<<((u8Id==10)?0:u8Id));
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
    OUTREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DIV, (u16Div & 0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_PERIOD_L, (u32Period&0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_PERIOD_H, ((u32Period>>16)&0x3));
    OUTREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DUTY_L, (u32Duty&0xFFFF));
    OUTREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DUTY_H, ((u32Duty>>16)&0x3));
}
#endif

#ifdef CONFIG_INFINITY6E
void DrvPWMGetConfig(U8 u8Id, U32* Duty,U32* Period)
{
    U16 u16Div = 0;
    U32 u32Duty = 0;
    U32 u32Period = 0;
    U32 pwmclk = 0;
    U32 common = 0;

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
    u16Div = INREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_DIV);
    u16Div++;

    if(Period != NULL)
    {
        if(_pwmPeriod_ns[u8Id] == 0)
        {
            u32Period = INREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_PERIOD_L) | ((INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_PERIOD_H) & 0x3) << 16);
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
            u32Duty = INREG16(BASE_REG_PWM_PA + ((u8Id < 4) ? (u8Id*0x80) : (u8Id*0x40 + 4*0x40)) + u16REG_PWM_DUTY_L) | ((INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DUTY_H) & 0x3) << 16);
            if(u32Duty)
            {
                u32Duty++;
            }
            _pwmDuty_ns[u8Id] = (u32Duty * u16Div * common) / pwmclk;
        }
        *Duty = _pwmDuty_ns[u8Id];
    }

}
#else
void DrvPWMGetConfig(U8 u8Id, U32* Duty,U32* Period)
{
    U16 u16Div = 0;
    U32 u32Duty = 0;
    U32 u32Period = 0;
    U32 pwmclk = 0;
    U32 common = 0;

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
    u16Div = INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DIV);
    u16Div++;

    if(Period != NULL)
    {
        if(_pwmPeriod_ns[u8Id] == 0)
        {
            u32Period = INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_PERIOD_L) | ((INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_PERIOD_H) & 0x3) << 16);
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
            u32Duty = INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DUTY_L) | ((INREG16(BASE_REG_PWM_PA + (u8Id*0x80) + u16REG_PWM_DUTY_H) & 0x3) << 16);
            if(u32Duty)
            {
                u32Duty++;
            }
            _pwmDuty_ns[u8Id] = (u32Duty * u16Div * common) / pwmclk;
        }
        *Duty = _pwmDuty_ns[u8Id];
    }

}
#endif

int DrvPWMDutyQE0(U8 u8GroupId, U8 u8Val)
{
    halPWMDutyQE0(u8GroupId, u8Val);
    return 1;
}

void DrvPWMInit(U8 u8Id)
{
    U32 reset, u32Period;

    if (PWM_NUM <= u8Id)
        return;

    reset = INREG16(BASE_REG_PWM_PA + u16REG_SW_RESET) & (BIT0<<u8Id);
    DrvPWMGetConfig(u8Id, NULL ,&u32Period);
    DrvPWMDutyQE0(0, 0);

    if ((0 == reset) && (u32Period))
    {
        _pwmEnSatus[u8Id] = 1;
    }
    else
    {
        DrvPWMEnable(u8Id, 0, 0xFFFF);//PAD_UNKNOWN
    }
}
