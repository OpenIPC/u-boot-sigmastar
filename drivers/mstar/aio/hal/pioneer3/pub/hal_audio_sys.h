/*
* hal_audio_sys.h- Sigmastar
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

#ifndef __HAL_AUDIO_SYS_H__
#define __HAL_AUDIO_SYS_H__

////////////////////////////////////////////////////////////////////////////
//
// Config by platform
//
////////////////////////////////////////////////////////////////////////////

// Padmux Defualt
#define AUD_DEF_DIG_PMUX        3
#define AUD_DEF_I2S_TX_PMUX     6
#define AUD_DEF_I2S_RX_PMUX     6
#define AUD_DEF_I2S_MCK_PMUX    7
#define AUD_DEF_I2S_RXTX_PMUX   0


// Dev ID
typedef enum
{
    AUD_AI_DEV_AMIC,
    AUD_AI_DEV_DMIC,
    AUD_AI_DEV_I2S_RX,
    AUD_AI_DEV_LINE_IN,
    AUD_AI_DEV_AMIC_D2,
    AUD_AI_DEV_NUM,

} AudAiDev_e;

typedef enum
{
    AUD_AO_DEV_LINE_OUT,
    AUD_AO_DEV_I2S_TX,
    AUD_AO_DEV_LINE_OUT_D1_L,
    AUD_AO_DEV_LINE_OUT_D2_L,
    AUD_AO_DEV_LINE_OUT_EP,
    AUD_AO_DEV_NUM,

} AudAoDev_e;

// ------------------------
// Common Fucntion
// ------------------------

// No exhaust space for write AO dma buffer, No exhaust data for read AI dma buffer.
#define NO_EXHAUST_DMA_BUF          ( 0 )
#define DMA_BUF_REMAINDER           ( 128 ) // 128 Bytes ( 8 MIU Line )

#define MIU_WORD_BYTE_SIZE          ( 16 )    //bytes
#define DMA_LOCALBUF_SIZE           ( 64 )    //bytes
#define DMA_LOCALBUF_LINE           ( DMA_LOCALBUF_SIZE/MIU_WORD_BYTE_SIZE )

#define BACH_DPGA_GAIN_MAX_DB       ( 34 )
#define BACH_DPGA_GAIN_MIN_DB       ( -64 )   //actually -63.5 dB
#define BACH_DPGA_GAIN_MIN_IDX      ( 0x7F )

#endif  // __HAL_AUDIO_SYS_H__
