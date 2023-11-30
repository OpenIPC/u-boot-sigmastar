/*
* rtk_bach_board_cfg.c- Sigmastar
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
#include "rtk_audio_dts_st.h"
#include "gpio.h"

// ------------------------
// DTS Config
// ------------------------

// Gpio ( now no use this, now set GPIO by hard code method !)
static const AudRtkDtsGpio_t AudRtkDtsGpioArray[] =
{
    {
        .str = "amp-gpio",
        .pin = PAD_UNKNOWN,
        .value = 1,
    },

    {
        .str = "amp-gpio",
        .pin = PAD_UNKNOWN,
        .value = 1,
    },
};

// PadMux
static const AudRtkDtsPadMux_t AudRtkDtsPadMuxArray[] =
{
    {
        .str = "i2s-padmux",
        .value = 1,
    },

    {
        .str = "digmic-padmux",
        .value = 1,
    },
};

//
static const AudRtkDtsKeepI2sClk_t AudRtkDtsKeepI2sClkArray[] =
{
    {
        .str = "keep-i2s-clk",
        .value = FALSE,
    },
};

static const AudRtkDtsKeepAdcPowerOn_t AudRtkDtsKeepAdcPowerOnArray[] =
{
    {
        .str = "keep_adc_power_on",
        .value = FALSE,
    },
};

static const AudRtkDtsKeepDacPowerOn_t AudRtkDtsKeepDacPowerOnArray[] =
{
    {
        .str = "keep_dac_power_on",
        .value = FALSE,
    },
};

static const AudRtkDtsI2sPcmMode_t AudRtkDtsI2sPcmModeArray[] =
{
    {
        .str = "i2s-pcm",
        .value = FALSE,
    },
};

static const AudRtkDts_t AudRtkDtsCfg[] =
{
    {
    .AudRtkDtsCompatibleStr = "sstar,audio",
    .AudRtkDtsIrqId = MS_INT_NUM_IRQ_BACH,
    .AudRtkDtsGpioList = AudRtkDtsGpioArray,
    .AudRtkDtsNumOfGpio = AUD_ARRAY_SIZE(AudRtkDtsGpioArray),
    .AudRtkDtsPadmuxList = AudRtkDtsPadMuxArray,
    .AudRtkDtsNumOfPadmux = AUD_ARRAY_SIZE(AudRtkDtsPadMuxArray),
    .AudRtkDtsKeepI2sClkList = AudRtkDtsKeepI2sClkArray,
    .AudRtkDtsNumOfKeepI2sClk = AUD_ARRAY_SIZE(AudRtkDtsKeepI2sClkArray),
    .AudRtkDtsKeepAdcPowerOn = AudRtkDtsKeepAdcPowerOnArray,
    .AudRtkDtsKeepDacPowerOn = AudRtkDtsKeepDacPowerOnArray,
    .AudRtkDtsI2sPcmMode = AudRtkDtsI2sPcmModeArray,
    }
};

