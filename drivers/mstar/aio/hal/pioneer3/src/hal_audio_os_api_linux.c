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

    /* reuse when padmux table ready
    // I2S TDM TX
    nMode = mdrv_padmux_getmode(MDRV_PUSE_I2S_TX_BCK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    switch(nMode)
    {
        case PINMUX_FOR_I2S_TX_MODE_1:
            nPadMuxMode = 1;
            break;
        case PINMUX_FOR_I2S_TX_MODE_2:
            nPadMuxMode = 2;
            break;
        case PINMUX_FOR_I2S_TX_MODE_3:
            nPadMuxMode = 3;
            break;
        case PINMUX_FOR_I2S_TX_MODE_4:
            nPadMuxMode = 4;
            break;
        case PINMUX_FOR_I2S_TX_MODE_5:
            nPadMuxMode = 5;
            break;
        case PINMUX_FOR_I2S_TX_MODE_6:
            nPadMuxMode = 6;
            break;
        default:
            nPadMuxMode = 0;
            break;
    }

    HalAudPriSetPadMuxI2sTdmTx(nPadMuxMode);

    // I2S TDM RX
    nMode = mdrv_padmux_getmode(MDRV_PUSE_I2S_RX_BCK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    switch(nMode)
    {
        case PINMUX_FOR_I2S_RX_MODE_1:
            nPadMuxMode = 1;
            break;
        case PINMUX_FOR_I2S_RX_MODE_2:
            nPadMuxMode = 2;
            break;
        case PINMUX_FOR_I2S_RX_MODE_3:
            nPadMuxMode = 3;
            break;
        case PINMUX_FOR_I2S_RX_MODE_4:
            nPadMuxMode = 4;
            break;
        case PINMUX_FOR_I2S_RX_MODE_5:
            nPadMuxMode = 5;
            break;
        case PINMUX_FOR_I2S_RX_MODE_6:
            nPadMuxMode = 6;
            break;
        default:
            nPadMuxMode = 0;
            break;
    }

    HalAudPriSetPadMuxI2sTdmRx(nPadMuxMode);
    */
    // DMIC
    nMode = mdrv_padmux_getmode(MDRV_PUSE_DMIC_CLK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    switch(nMode)
    {
        case PINMUX_FOR_DMIC_MODE_1:
            nPadMuxMode = 1;
            break;
        case PINMUX_FOR_DMIC_MODE_2:
            nPadMuxMode = 2;
            break;
        case PINMUX_FOR_DMIC_MODE_3:
            nPadMuxMode = 3;
            break;
        case PINMUX_FOR_DMIC_MODE_4:
            nPadMuxMode = 4;
            break;
        case PINMUX_FOR_DMIC_MODE_5:
            nPadMuxMode = 5;
            break;
        case PINMUX_FOR_DMIC_MODE_6:
            nPadMuxMode = 6;
            break;
        case PINMUX_FOR_DMIC_MODE_7:
            nPadMuxMode = 7;
            break;
        case PINMUX_FOR_DMIC_MODE_8:
            nPadMuxMode = 8;
            break;
        case PINMUX_FOR_DMIC_MODE_9:
            nPadMuxMode = 9;
            break;
        case PINMUX_FOR_DMIC_MODE_10:
            nPadMuxMode = 10;
            break;
        case PINMUX_FOR_DMIC_MODE_11:
            nPadMuxMode = 11;
            break;
        case PINMUX_FOR_DMIC_MODE_12:
            nPadMuxMode = 12;
            break;
        case PINMUX_FOR_DMIC_MODE_13:
            nPadMuxMode = 13;
            break;
        default:
            nPadMuxMode = 0;
            break;
    }

    HalAudPriSetPadMuxDmic(nPadMuxMode);

    // I2S MCK
    nMode = mdrv_padmux_getmode(MDRV_PUSE_AIO_MCK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    switch(nMode)
    {
        case PINMUX_FOR_I2S_MCK_MODE_1:
            nPadMuxMode = 1;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_2:
            nPadMuxMode = 2;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_3:
            nPadMuxMode = 3;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_4:
            nPadMuxMode = 4;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_5:
            nPadMuxMode = 5;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_6:
            nPadMuxMode = 6;
            break;
        case PINMUX_FOR_I2S_MCK_MODE_7:
            nPadMuxMode = 7;
            break;
        default:
            nPadMuxMode = 0;
            break;
    }

    HalAudPriSetPadMuxMck(nPadMuxMode);

    // I2S normal mode
    nMode = mdrv_padmux_getmode(MDRV_PUSE_I2S_BCK); // It may be PINMUX_FOR_UNKNOWN_MODE.

    switch(nMode)
    {
        case PINMUX_FOR_I2S_RXTX_MODE_1:
            nPadMuxMode = 1;
            break;
        case PINMUX_FOR_I2S_RXTX_MODE_2:
            nPadMuxMode = 2;
            break;
        case PINMUX_FOR_I2S_RXTX_MODE_3:
            nPadMuxMode = 3;
            break;
        case PINMUX_FOR_I2S_RXTX_MODE_4:
            nPadMuxMode = 4;
            break;
        case PINMUX_FOR_I2S_RXTX_MODE_5:
            nPadMuxMode = 5;
            break;
        case PINMUX_FOR_I2S_RXTX_MODE_6:
            nPadMuxMode = 6;
            break;
        default:
            nPadMuxMode = 0;
            break;
    }

    HalAudPriSetPadMuxI2sMode(nPadMuxMode);

    return TRUE;
}
