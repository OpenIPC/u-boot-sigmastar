/*
* rtk_audio_dts_st.h- Sigmastar
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

#ifndef __RTK_AUDIO_DTS_ST_H__
#define __RTK_AUDIO_DTS_ST_H__
// ------------------------
// DTS define
// ------------------------

// Macro
#define AUD_ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))

typedef struct
{
    const char *str;
    const int pin;
    const int value;

} AudRtkDtsGpio_t;

typedef struct
{
    const char *str;
    const int value;

} AudRtkDtsPadMux_t;

typedef struct
{
    const char *str;
    const int value;

} AudRtkDtsKeepI2sClk_t;

typedef struct
{
    const char *str;
    const int value;

} AudRtkDtsKeepAdcPowerOn_t;

typedef struct
{
    const char *str;
    const int value;

} AudRtkDtsKeepDacPowerOn_t;

typedef struct
{
    const char *str;
    const int value;

} AudRtkDtsI2sPcmMode_t;

typedef struct
{
    const char *AudRtkDtsCompatibleStr;
    const int AudRtkDtsIrqId;
    const AudRtkDtsGpio_t *AudRtkDtsGpioList;
    const int AudRtkDtsNumOfGpio;
    const AudRtkDtsPadMux_t *AudRtkDtsPadmuxList;
    const int AudRtkDtsNumOfPadmux;
    const AudRtkDtsKeepI2sClk_t *AudRtkDtsKeepI2sClkList;
    const AudRtkDtsKeepAdcPowerOn_t *AudRtkDtsKeepAdcPowerOn;
    const AudRtkDtsKeepDacPowerOn_t *AudRtkDtsKeepDacPowerOn;
    const int AudRtkDtsNumOfKeepI2sClk;
    const AudRtkDtsI2sPcmMode_t *AudRtkDtsI2sPcmMode;

} AudRtkDts_t;

// ------------------------
// DTS API
// ------------------------
BOOL _AudRtkDtsFindCompatibleNode( const char *compatible_str );
int _AudRtkDtsGetIrqId( void );
BOOL _AudRtkDtsGetGpio( const char *amp_gpio_str, U32 *aGpio );
BOOL _AudRtkDtsGetPadMux( const char *padmux_str, S32 *padmux );
BOOL _AudRtkDtsGetKeepI2sClk(const char *keep_i2s_clk_str, S32 *keepI2sClk);
BOOL _AudRtkDtsGetKeepDacPowerOn(const char *keep_dac_power_on, S32 *keepDacPowerOn);
BOOL _AudRtkDtsGetKeepAdcPowerOn(const char *keep_adc_power_on, S32 *keepAdcPowerOn);
BOOL _AudRtkDtsGetI2sPcmMode(const char *i2s_pcm_mode_str, S32 *I2sPcmMode);

#endif
