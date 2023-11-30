/*
* mhal_audio.h- Sigmastar
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

#ifndef __MHAL_AUDIO_H__

#define __MHAL_AUDIO_H__
#include "mhal_audio_datatype.h"

/**
*  @file mhal_audio.h
*  @brief audio driver APIs
*/

/**
* \brief Init audio HW
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_Init(void *pdata);

MS_S32 MHAL_AUDIO_DeInit(void);

/**
* \brief Config audio I2S output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig);

/**
* \brief Config audio I2S input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig);

/**
* \brief Config audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig);

/**
* \brief Config audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig);


/**
* \brief Open audio PCM output device, should be called after configPcmOut
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Open audio PCM input device, should be called after configPcmIn
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmIn(MHAL_AUDIO_AiDev_e AinDevId);


/**
* \brief Close audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Close audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmIn(MHAL_AUDIO_AiDev_e AinDevId);

/**
* \brief Start audio PCM output device, start playback
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Start audio PCM input device, start recording
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmIn(MHAL_AUDIO_AiDev_e AinDevId);

/**
* \brief Stop audio PCM output device, stop playback
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Stop audio PCM input device, stop recording
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmIn(MHAL_AUDIO_AiDev_e AinDevId);

/**
* \brief Pause audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_PausePcmOut(MHAL_AUDIO_AoDev_e AoutDevId);


/**
* \brief Resume audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ResumePcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Read audio raw data from PCM input device(period unit)
* \return value>0 => read data bytes, value<0 => error number, -2 => empty
*/
//MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_AiDev_e AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock, MS_U64 *pu64AiPts );
MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_AiDev_e AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock );

/**
* \brief Write audio raw data to PCM output device(period unit)
* \return value>0 => write data bytes, value<0 => error number, -2 => full
*/
MS_S32 MHAL_AUDIO_WriteDataOut(MHAL_AUDIO_AoDev_e AoutDevId, VOID *pWrBuffer, MS_U32 u32Size, MS_BOOL bBlock);

/**
* \brief Get PCM device status(empty or not, if empty, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmOutXrun(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief Get PCM device status(full or not, if full, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmInXrun(MHAL_AUDIO_AiDev_e AinDevId);

/**
* \brief you can restart PcmOut (call MHAL_AUDIO_StartPcmOut), after this function is called.
* \return 0 => success, <0 => error number
*/
MS_BOOL MHAL_AUDIO_PrepareToRestartPcmOut(MHAL_AUDIO_AoDev_e AoutDevId);

/**
* \brief you can restart PcmIn (call MHAL_AUDIO_StartPcmIn), after this function is called.
* \return 0 => success, <0 => error number
*/
MS_BOOL MHAL_AUDIO_PrepareToRestartPcmIn(MHAL_AUDIO_AiDev_e AinDevId);

/**
* \brief Set output path gain
* \max: 30    min: -60
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainOut(MHAL_AUDIO_AoDev_e AoutDevId, MS_S16 s16Gain, MS_S8 s8Ch, MHAL_AUDIO_GainFading_e eFading);

/**
* \brief Set input path gain
* \max: 30    min: -60
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainIn(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch);

/**
* \brief Set ADC gain
* \
* \E_MHAL_AUDIO_AI_DEV_MIC
* \Range: 0 ~ 21 => -6dB ~ 57dB
* \Step: 3dB
* \
* \E_MHAL_AUDIO_AI_DEV_LINE_IN
* \Range: 0 ~ 7 => -6dB ~ 15dB
* \Step: 3dB
* \
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetAdcGain(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch);

/**
* \brief Set DMIC gain
* \
* \Platform: I5
* \Range: 0 ~ 4 => 0dB ~ 24dB
* \Step: 6dB
* \return 0 => success, <0 => error number
* \
* \Platform: I6
* \Range: -60 ~ 10 => -60dB ~ 10dB
* \Step: 1dB
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetDmicGain(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch);

/**
* \brief Set src path gain
* \max: 30    min: -60
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetSrcGain(MHAL_AUDIO_AoDev_e AoutDevId, MS_S16 s16Gain, MS_S8 s8Ch);

/**
* \brief Get Ao dma buf current data length ( bytes ).
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmOutCurrDataLen( MHAL_AUDIO_AoDev_e AoutDevId, MS_U32 *len );

/**
* \brief Get Ai dma buf current data length ( bytes ).
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmInCurrDataLen( MHAL_AUDIO_AiDev_e AinDevId, MS_U32 *len );

/**
* \brief Get Ao Start Pts.
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmOutStartPts(MHAL_AUDIO_AoDev_e AoutDevId, MS_U64 *pu64AoStartPts );

/**
* \enable AO SINEGEN, SampleRate: 48K.
*/
MS_S32 MHAL_AUDIO_DmaSineGenOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_SineGenCfg_t SineGen );

/**
* \enable AI SINEGEN, SampleRate: 48K.
*/
MS_S32 MHAL_AUDIO_DmaSineGenIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_SineGenCfg_t SineGen );

/**
* \enable Ao Amp.
*/
MS_BOOL MHAL_AUDIO_AmpEnable(MS_BOOL bEnable, MS_S8 s8Ch);

#endif //__MHAL_AUDIO_H__
