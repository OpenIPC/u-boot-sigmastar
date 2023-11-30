/*
* audio_proc.c- Sigmastar
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

#include "audio_proc.h"

//
#include "hal_audio_common.h"
#include "hal_audio_config.h"
#include "hal_audio_sys.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"

AudProcInfoAi_t gAudProInfoAiList[AUD_AI_DEV_NUM] = {0};
AudProcInfoAo_t gAudProInfoAoList[AUD_AO_DEV_NUM] = {0};

void AudioProcInit(void)
{
}

void AudioProcDeInit(void)
{
}
