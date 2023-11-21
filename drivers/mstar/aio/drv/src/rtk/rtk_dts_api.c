/*
* rtk_dts_api.c- Sigmastar
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
#include "cam_os_wrapper.h"
#include "sys_MsWrapper_cus_os_int_ctrl.h"
#include "hal_int_ctrl_pub.h"
// Hal
#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"
#include "hal_audio_reg.h"

// Drv
#include "drv_audio_dbg.h"
#include "drv_audio_api.h"
#include "drv_audio_pri.h"

#include "rtk_audio_dts_st.h"
#include "rtk_bach_board_cfg.c"





BOOL _AudRtkDtsFindCompatibleNode( const char *compatible_str )
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsCompatibleStr, compatible_str) != 0 )
    {
        return FALSE;
    }

    return TRUE;
}

int _AudRtkDtsGetIrqId( void )
{
    return AudRtkDtsCfg[0].AudRtkDtsIrqId;
}

BOOL _AudRtkDtsGetGpio( const char *amp_gpio_str, U32 *aGpio )
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsGpioList[0].str, amp_gpio_str) != 0 ||
          strcmp(AudRtkDtsCfg[0].AudRtkDtsGpioList[1].str, amp_gpio_str) != 0)
    {
        return FALSE;
    }

    aGpio[0] = AudRtkDtsCfg[0].AudRtkDtsGpioList[0].pin;
    aGpio[1] = AudRtkDtsCfg[0].AudRtkDtsGpioList[0].value;

    aGpio[2] = AudRtkDtsCfg[0].AudRtkDtsGpioList[1].pin;
    aGpio[3] = AudRtkDtsCfg[0].AudRtkDtsGpioList[1].value;

    return TRUE;
}

BOOL _AudRtkDtsGetPadMux( const char *padmux_str, S32 *padmux )
{
    int i = 0;

    for ( i = 0; i < AudRtkDtsCfg[0].AudRtkDtsNumOfPadmux; i++ )
    {
        if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsPadmuxList[i].str, padmux_str) == 0 )
        {
            *padmux = AudRtkDtsCfg[0].AudRtkDtsPadmuxList[i].value;
            return TRUE;
        }
    }

    return FALSE;
}

BOOL _AudRtkDtsGetKeepI2sClk(const char *keep_i2s_clk_str, S32 *keepI2sClk)
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsKeepI2sClkList[0].str, keep_i2s_clk_str) != 0 )
    {
        return FALSE;
    }

    *keepI2sClk = AudRtkDtsCfg[0].AudRtkDtsKeepI2sClkList[0].value;

    return TRUE;
}

BOOL _AudRtkDtsGetKeepAdcPowerOn(const char *keep_adc_power_on, S32 *keepAdcPowerOn)
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsKeepAdcPowerOn[0].str, keep_adc_power_on) != 0 )
    {
        return FALSE;
    }

    *keepAdcPowerOn = AudRtkDtsCfg[0].AudRtkDtsKeepAdcPowerOn[0].value;

    return TRUE;
}

BOOL _AudRtkDtsGetKeepDacPowerOn(const char *keep_dac_power_on, S32 *keepDacPowerOn)
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsKeepDacPowerOn[0].str, keep_dac_power_on) != 0 )
    {
        return FALSE;
    }

    *keepDacPowerOn = AudRtkDtsCfg[0].AudRtkDtsKeepDacPowerOn[0].value;

    return TRUE;
}

BOOL _AudRtkDtsGetI2sPcmMode(const char *i2s_pcm_mode_str, S32 *I2sPcmMode)
{
    if ( strcmp(AudRtkDtsCfg[0].AudRtkDtsI2sPcmMode[0].str, i2s_pcm_mode_str) != 0 )
    {
        return FALSE;
    }

    *I2sPcmMode = AudRtkDtsCfg[0].AudRtkDtsI2sPcmMode[0].value;

    return TRUE;
}