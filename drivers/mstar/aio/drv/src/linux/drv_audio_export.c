/*
* drv_audio_export.c- Sigmastar
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

#include <linux/kernel.h>
#include "mhal_audio.h"

EXPORT_SYMBOL(MHAL_AUDIO_Init);
EXPORT_SYMBOL(MHAL_AUDIO_DeInit);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigI2sOut);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigI2sIn);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ConfigPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_OpenPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_OpenPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_ClosePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ClosePcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_StartPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_StartPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_StopPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_StopPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_PausePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ResumePcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_ReadDataIn);
EXPORT_SYMBOL(MHAL_AUDIO_WriteDataOut);
EXPORT_SYMBOL(MHAL_AUDIO_IsPcmOutXrun);
EXPORT_SYMBOL(MHAL_AUDIO_IsPcmInXrun);
EXPORT_SYMBOL(MHAL_AUDIO_PrepareToRestartPcmOut);
EXPORT_SYMBOL(MHAL_AUDIO_PrepareToRestartPcmIn);
EXPORT_SYMBOL(MHAL_AUDIO_SetGainOut);
EXPORT_SYMBOL(MHAL_AUDIO_SetGainIn);
EXPORT_SYMBOL(MHAL_AUDIO_SetAdcGain);
EXPORT_SYMBOL(MHAL_AUDIO_SetDmicGain);
EXPORT_SYMBOL(MHAL_AUDIO_SetSrcGain);
EXPORT_SYMBOL(MHAL_AUDIO_GetPcmOutCurrDataLen);
EXPORT_SYMBOL(MHAL_AUDIO_GetPcmInCurrDataLen);
EXPORT_SYMBOL(MHAL_AUDIO_GetPcmOutStartPts);
EXPORT_SYMBOL(MHAL_AUDIO_DmaSineGenOut);
EXPORT_SYMBOL(MHAL_AUDIO_DmaSineGenIn);
EXPORT_SYMBOL(MHAL_AUDIO_AmpEnable);
