/*
* mhal_audio_datatype.h- Sigmastar
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

#ifndef __MHAL_AUDIO_DATATYPE_H__

#define __MHAL_AUDIO_DATATYPE_H__
#include "mhal_common.h"

/**
*  @file mhal_audio_datatype.h
*  @brief audio driver data structure
*/

// DEV ID, refer to hal_audio_sys.h "AudAiDev_e" & "AudAoDev_e"
typedef MHAL_AUDIO_DEV  MHAL_AUDIO_AiDev_e;
typedef MHAL_AUDIO_DEV  MHAL_AUDIO_AoDev_e;

typedef enum
{
    E_MHAL_AUDIO_RATE_8K = 8000,
    E_MHAL_AUDIO_RATE_11K = 11000,
    E_MHAL_AUDIO_RATE_12K = 12000,
    E_MHAL_AUDIO_RATE_16K = 16000,
    E_MHAL_AUDIO_RATE_22K = 22000,
    E_MHAL_AUDIO_RATE_24K = 24000,
    E_MHAL_AUDIO_RATE_32K = 32000,
    E_MHAL_AUDIO_RATE_44K = 44000,
    E_MHAL_AUDIO_RATE_48K = 48000,
}MHAL_AUDIO_Rate_e;

typedef enum
{
    E_MHAL_AUDIO_BITWIDTH_16,
    E_MHAL_AUDIO_BITWIDTH_32,
}MHAL_AUDIO_BitWidth_e;

typedef enum
{
    E_MHAL_AUDIO_I2S_FMT_I2S,
    E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY,
}MHAL_AUDIO_I2sFmt_e;

typedef enum
{
    E_MHAL_AUDIO_MODE_I2S_MASTER,
    E_MHAL_AUDIO_MODE_I2S_SLAVE,
    E_MHAL_AUDIO_MODE_TDM_MASTER,
    E_MHAL_AUDIO_MODE_TDM_SLAVE
}MHAL_AUDIO_I2sMode_e;

typedef enum
{
    E_MHAL_AUDIO_MCK_NULL,      // Disable
    E_MHAL_AUDIO_MCK_12_288M,   // 12.288MHz
    E_MHAL_AUDIO_MCK_16_384M,   // 16.384MHz
    E_MHAL_AUDIO_MCK_18_432M,   // 18.432MHz
    E_MHAL_AUDIO_MCK_24_576M,   // 24.576MHz
    E_MHAL_AUDIO_MCK_24M,       // 24MHz
    E_MHAL_AUDIO_MCK_48M,       // 48MHz
}MHAL_AUDIO_I2sMck_e;

typedef enum
{
    E_MHAL_AUDIO_I2STX_DMARD1,
    E_MHAL_AUDIO_I2STX_DMAWR1,
    E_MHAL_AUDIO_I2STX_SRC,
    E_MHAL_AUDIO_I2STX_DMARD2,
    E_MHAL_AUDIO_I2STX_NUM
}MHAL_AUDIO_I2sTxMux_e;

typedef enum
{
    E_MHAL_AUDIO_4WIRE_OFF,
    E_MHAL_AUDIO_4WIRE_ON,
}MHAL_AUDIO_I2s4WireMode_e;

typedef enum
{
    E_MHAL_AUDIO_GAIN_FADING_0, // OFF
    E_MHAL_AUDIO_GAIN_FADING_1, // 1 samples
    E_MHAL_AUDIO_GAIN_FADING_2, // 2 samples
    E_MHAL_AUDIO_GAIN_FADING_3, // 4 samples
    E_MHAL_AUDIO_GAIN_FADING_4, // 8 samples
    E_MHAL_AUDIO_GAIN_FADING_5, // 16 samples
    E_MHAL_AUDIO_GAIN_FADING_6, // 32 samples
    E_MHAL_AUDIO_GAIN_FADING_7, // 64 samples
    E_MHAL_AUDIO_GAIN_FADING_NUM,

}MHAL_AUDIO_GainFading_e;

typedef struct MHAL_AUDIO_I2sCfg_s
{
    MHAL_AUDIO_I2sMode_e eMode;
    MHAL_AUDIO_BitWidth_e eWidth;
    MHAL_AUDIO_I2sFmt_e eFmt;
    MHAL_AUDIO_Rate_e eRate;
    MHAL_AUDIO_I2sMck_e eMck;
    MHAL_AUDIO_I2s4WireMode_e e4WireMode;
    MS_U16 u16Channels;
}MHAL_AUDIO_I2sCfg_t;

typedef struct MHAL_AUDIO_PcmCfg_s
{
    MHAL_AUDIO_Rate_e eRate;
    MHAL_AUDIO_BitWidth_e eWidth;
    MS_U16 u16Channels;
    MS_BOOL bInterleaved;
    MS_U8 *pu8DmaArea;	/* DMA area */
    MS_PHYADDR phyDmaAddr; //dma buffer size should be nPeriodSize*nPeriodCount
    MS_U32 u32BufferSize;
    MS_U32 u32PeriodSize; //bytes
    MS_U32 u32StartThres;
}MHAL_AUDIO_PcmCfg_t;

typedef struct MHAL_AUDIO_MmapCfg_s
{
    MS_U8     u8Gid;                         // Mmap ID
    MS_U8     u8Layer;                       // Memory Layer
    MS_U8     u8MiuNo;                       // 0: MIU0 / 1: MIU1 / 2: MIU2
    MS_U8     u8CMAHid;                      // Memory CMAHID
    MS_U32    u32Addr;                       // Memory Address
    MS_U32    u32Size;                       // Memory Size
    MS_U32    u32Align;                      // Memory Align
    MS_U32    u32MemoryType;                 // Memory Type
}MHAL_AUDIO_MmapCfg_t;

typedef struct MHAL_AUDIO_SineGenCfg_t
{
    MS_BOOL   bEnable;
    MS_U8     u8Freq;                        // 2: 1000Hz for 48K sample rate
    MS_U8     u8Gain;                        // 0: 0dB(maximum), -6dB per step
}MHAL_AUDIO_SineGenCfg_t;

#endif //__MHAL_AUDIO_DATATYPE_H__