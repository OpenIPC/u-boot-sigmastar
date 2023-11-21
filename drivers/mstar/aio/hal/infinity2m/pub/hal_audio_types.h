/*
* hal_audio_types.h- Sigmastar
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

#ifndef __HAL_AUD_TYPES_H__
#define __HAL_AUD_TYPES_H__

typedef enum
{
    AUD_RATE_SLAVE,
    AUD_RATE_8K,
    AUD_RATE_11K,
    AUD_RATE_12K,
    AUD_RATE_16K,
    AUD_RATE_22K,
    AUD_RATE_24K,
    AUD_RATE_32K,
    AUD_RATE_44K,
    AUD_RATE_48K,
    AUD_RATE_NUM,
    AUD_RATE_NULL = 0xff,
} AudRate_e;

typedef enum
{
    AUD_BITWIDTH_16,
    AUD_BITWIDTH_32
}AudBitWidth_e;

typedef enum
{
    AUD_I2S_IF_CODEC_RX,
    AUD_I2S_IF_CODEC_TX,
    AUD_I2S_IF_NUM,
    AUD_I2S_IF_NULL = 0xff,
}AudI2sIf_e;

typedef enum
{
    AUD_I2S_CODEC_RX,
    AUD_I2S_CODEC_TX,
    AUD_I2S_NUM
}AudI2s_e;

typedef enum
{
    AUD_I2S_MODE_I2S,
    AUD_I2S_MODE_TDM,
}AudI2sMode_e;

typedef enum
{
    AUD_I2S_MSMODE_MASTER,
    AUD_I2S_MSMODE_SLAVE,
}AudI2sMsMode_e;

typedef enum
{
    AUD_I2S_FMT_I2S,
    AUD_I2S_FMT_LEFT_JUSTIFY
}AudI2sFmt_e;

typedef enum
{
    AUD_I2S_MCK_NULL,   // Disable
    AUD_I2S_MCK_12_288M,// 12.288MHz
    AUD_I2S_MCK_16_384M,// 16.384MHz
    AUD_I2S_MCK_18_432M,// 18.432MHz
    AUD_I2S_MCK_24_576M,// 24.576MHz
    AUD_I2S_MCK_24M,    // 24MHz
    AUD_I2S_MCK_48M,    // 48MHz
}AudI2sMck_e;

typedef enum
{
    AUD_I2S_WIRE_4,
    AUD_I2S_WIRE_6,
    AUD_I2S_WIRE_NULL = 0xff
}AudWireMode_e;

typedef enum
{
    AUD_TDM_CHN_MAP_NULL = 0xff
}AudTdmChnMap_e;

typedef enum
{
    AUD_DMA_WRITER1,
    AUD_DMA_READER1,
    AUD_DMA_NUM,
    AUD_DMA_NULL = 0xff
}AudDmaChn_e;

typedef enum
{
    AUD_ADC_LINEIN,
    AUD_ADC_MICIN,
    AUD_DAC_LINEOUT,
    AUD_ATOP_NUM,
} AudAtopPath_e;

typedef enum
{
    AUD_ADC_SEL_LINEIN,
    AUD_ADC_SEL_MICIN,
    AUD_ADC_SEL_MICIN2,
    AUD_ADC_SEL_MICIN_4CH,
    AUD_ADC_SEL_NUM,
    AUD_ADC_SEL_NULL = 0xFF,
} AudAdcSel_e;

/**
 * \brief Audio Synthesizer
 */
typedef struct
{
    AudI2sMode_e eMode;
    AudI2sMsMode_e eMsMode;
    AudI2sFmt_e eFormat;
    AudBitWidth_e eWidth;
    AudI2sMck_e eMck;
    U16 nChannelNum;
    AudRate_e eRate;
    AudWireMode_e eWireMode;
    void *pPriData;
}AudI2sCfg_t;

/**
 * \brief Audio MUX
 */
typedef enum
{
    AUD_MUX_MMC1,
    AUD_MUX_DMAWR1,
    AUD_MUX_DMAWR1_MCH,
    AUD_MUX_I2S_TX,
    AUD_MUX_AMIC_DMIC,
    AUD_MUX_DAAD_LOOP,
    AUD_MUX_NUM,
    AUD_MUX_NULL = 0xff
}AudMux_e;

typedef enum
{
    AUD_DPGA_MMC1,
    AUD_DPGA_ADC,
    AUD_DPGA_AEC1,
    AUD_DPGA_DEC1,
    AUD_DPGA_NUM,
    AUD_DPGA_NULL = 0xff,
} AudDpga_e;

typedef enum
{
    AUD_PAD_MUX_DMIC,
    AUD_PAD_MUX_I2S_TX,
    AUD_PAD_MUX_I2S_RX,
    AUD_PAD_MUX_NUM,
    AUD_PAD_MUX_NULL = 0xff,
} AudPadMux_e;

typedef enum
{
    AUD_MCH_SEL_DMIC,
    AUD_MCH_SEL_ADC,
    AUD_MCH_SEL_I2S_RX,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_NUM,
    AUD_MCH_SEL_NULL = 0xff,
} AudMchSel_e;

typedef enum
{
    AUD_MCH_CH_BIND_01,
    AUD_MCH_CH_BIND_23,
    AUD_MCH_CH_BIND_4567,
    AUD_MCH_CH_BIND_NUM,
} AudMchChBind_e;

typedef enum
{
    AUD_MCH_CLK_REF_DMIC,
    AUD_MCH_CLK_REF_ADC,
    AUD_MCH_CLK_REF_NUM,
} AudMchClkRef_e;

typedef enum
{
    AUD_MCH_SET_W1,
    AUD_MCH_SET_NUM,
} AudMchSet_e;

#endif //__HAL_AUD_TYPES_H__