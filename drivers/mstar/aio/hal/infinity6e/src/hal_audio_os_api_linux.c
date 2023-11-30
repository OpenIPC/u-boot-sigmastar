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
    nMode = mdrv_padmux_getmode(MDRV_PUSE_AIO_MCK); // It may be PINMUX_FOR_UNKNOWN_MODE.
    if( nMode == PINMUX_FOR_I2S_MCK_MODE_1 ||
        nMode == PINMUX_FOR_I2S_MCK_MODE_2 ||
        nMode == PINMUX_FOR_I2S_MCK_MODE_3 ||
        nMode == PINMUX_FOR_I2S_MCK_MODE_4
        ) {
        if (nMode == PINMUX_FOR_I2S_MCK_MODE_1)
        {
            nPadMuxMode = 1;
        }
        else if (nMode == PINMUX_FOR_I2S_MCK_MODE_2)
        {
            nPadMuxMode = 2;
        }
        else if (nMode == PINMUX_FOR_I2S_MCK_MODE_3)
        {
            nPadMuxMode = 3;
        }
        else if (nMode == PINMUX_FOR_I2S_MCK_MODE_4)
        {
            nPadMuxMode = 4;
        }
        else
        {
            nPadMuxMode = 0;
        }
        HalAudPriSetPadMuxI2sTX(nPadMuxMode);
    }

    // I2S
    nMode = mdrv_padmux_getmode(MDRV_PUSE_I2S_BCK); // It may be PINMUX_FOR_UNKNOWN_MODE.
    //for 4-wire mode or 6-wire mode used
    if( nMode == PINMUX_FOR_I2S_TX_MODE_1 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_2 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_3 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_4 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_5 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_6 ||
        nMode == PINMUX_FOR_I2S_TX_MODE_7
        ) {
        if (nMode == PINMUX_FOR_I2S_TX_MODE_1)
        {
            nPadMuxMode = 1;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_2)
        {
            nPadMuxMode = 2;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_3)
        {
            nPadMuxMode = 3;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_4)
        {
            nPadMuxMode = 4;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_5)
        {
            nPadMuxMode = 5;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_6)
        {
            nPadMuxMode = 6;
        }
        else if (nMode == PINMUX_FOR_I2S_TX_MODE_7)
        {
            nPadMuxMode = 7;
        }
        else
        {
            nPadMuxMode = 0;
        }
        HalAudPriSetPadMuxI2sTX(nPadMuxMode);
    }

    //for 4-wire mode used
    if( nMode == PINMUX_FOR_I2S_RXTX_MODE_1 ||
        nMode == PINMUX_FOR_I2S_RXTX_MODE_2 ||
        nMode == PINMUX_FOR_I2S_RXTX_MODE_3 ||
        nMode == PINMUX_FOR_I2S_RXTX_MODE_4 ||
        nMode == PINMUX_FOR_I2S_RXTX_MODE_5 ||
        nMode == PINMUX_FOR_I2S_RXTX_MODE_6
        ) {
        if (nMode == PINMUX_FOR_I2S_RXTX_MODE_1)
        {
            nPadMuxMode = 1;
        }
        else if (nMode == PINMUX_FOR_I2S_RXTX_MODE_2)
        {
            nPadMuxMode = 2;
        }
        else if (nMode == PINMUX_FOR_I2S_RXTX_MODE_3)
        {
            nPadMuxMode = 3;
        }
        else if (nMode == PINMUX_FOR_I2S_RXTX_MODE_4)
        {
            nPadMuxMode = 4;
        }
        else if (nMode == PINMUX_FOR_I2S_RXTX_MODE_5)
        {
            nPadMuxMode = 5;
        }
        else if (nMode == PINMUX_FOR_I2S_RXTX_MODE_6)
        {
            nPadMuxMode = 6;
        }
        else
        {
            nPadMuxMode = 0;
        }
        HalAudPriSetPadMuxI2sTXRX(nPadMuxMode);
    }

    if( nMode == PINMUX_FOR_I2S_RX_MODE_1 ||
        nMode == PINMUX_FOR_I2S_RX_MODE_2 ||
        nMode == PINMUX_FOR_I2S_RX_MODE_3 ||
        nMode == PINMUX_FOR_I2S_RX_MODE_4
        ) {
        if (nMode == PINMUX_FOR_I2S_RX_MODE_1)
        {
            nPadMuxMode = 1;
        }
        else if (nMode == PINMUX_FOR_I2S_RX_MODE_2)
        {
            nPadMuxMode = 2;
        }
        else if (nMode == PINMUX_FOR_I2S_RX_MODE_3)
        {
            nPadMuxMode = 3;
        }
        else if (nMode == PINMUX_FOR_I2S_RX_MODE_4)
        {
            nPadMuxMode = 4;
        }
        else
        {
            nPadMuxMode = 0;
        }
        HalAudPriSetPadMuxI2sRX(nPadMuxMode);
    }

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
    else if (nMode == PINMUX_FOR_DMIC_MODE_5)
    {
        nPadMuxMode = 5;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_6)
    {
        nPadMuxMode = 6;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_7)
    {
        nPadMuxMode = 7;
    }
    else if (nMode == PINMUX_FOR_DMIC_MODE_8)
    {
        nPadMuxMode = 8;
    }
    else
    {
        nPadMuxMode = 0;
    }

    HalAudPriSetPadMuxDmic(nPadMuxMode);

    return TRUE;
}
