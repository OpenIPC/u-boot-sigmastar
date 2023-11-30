/*
* hal_audio_os_api_linux.c- Sigmastar
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
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#include "padmux.h"
#include "mdrv_padmux.h"
#include "mdrv_puse.h"
#include "hal_audio_common.h"
#include "hal_audio_pri_api.h"



BOOL HalAudOsApiChangeAmpGpio(U32 *nGpioPad, S8 s8Ch)
{
    if(s8Ch == 0)
        *nGpioPad = mdrv_padmux_getpad(MDRV_PUSE_AIO_AMP_PWR); // It may be PAD_UNKNOWN.

    return TRUE;
}

BOOL HalAudOsApiPreSetAllPadmux(void)
{
    S32 nMode, nPadMuxMode;

    // I2S
    nMode = mdrv_padmux_getmode(MDRV_PUSE_I2S_BCK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    if (nMode == PINMUX_FOR_I2S_MODE_1)
    {
        nPadMuxMode = 1;
    }
    else if (nMode == PINMUX_FOR_I2S_MODE_2)
    {
        nPadMuxMode = 2;
    }
    else if (nMode == PINMUX_FOR_I2S_MODE_3)
    {
        nPadMuxMode = 3;
    }
    else
    {
        nPadMuxMode = 0;
    }

    HalAudPriSetPadMuxI2s(nPadMuxMode);

    // DMIC
    nMode = mdrv_padmux_getmode(MDRV_PUSE_DMIC_CLK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    if (nMode == PINMUX_FOR_DMIC_MODE_1)
    {
        nPadMuxMode = 1;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_2)
    {
        nPadMuxMode = 2;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_3)
    {
        nPadMuxMode = 3;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_4)
    {
        nPadMuxMode = 4;
    }
    else
    {
        nPadMuxMode = 0;
    }

    HalAudPriSetPadMuxDmic(nPadMuxMode);

    return TRUE;
}
