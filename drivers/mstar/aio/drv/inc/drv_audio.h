/*
* drv_audio.h- Sigmastar
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

#ifndef __DRV_AUDIO_H__
#define __DRV_AUDIO_H__

typedef struct
{
    u8 *pDmaBuf;
    u32 nPhysDmaAddr;
    u32 nBufferSize;
    u16 nChannels;
    u16 nBitWidth;
    u32 nSampleRate;
    u32 nPeriodSize;
    u32 nStartThres;
    u16 nInterleaved;
    void *private;
} DmaParam_t;

typedef struct
{
    u16 nChannels;
    u16 nBitWidth;
    u32 nSampleRate;

} DigMicParam_t;

typedef struct
{
    u32 nSampleRate;

} AecParam_t;



void DrvAudInit(void);
void DrvAudDeInit(void);

BOOL DrvAudConfigI2s(AudI2sIf_e eI2sIf, AudI2sCfg_t *ptI2sCfg);
BOOL DrvAudDmaInitChannel(AudDmaChn_e eDma,
                                 u32 nPhysDmaAddr,
                                 u32 nBufferSize,
                                 u32 nChannels,
                                 AudBitWidth_e eBitWidth,
                                 u32 nThreshold);

BOOL DrvAudSetMux(AudMux_e eMux, u8 nChoice);
BOOL DrvAudMonoEnable(AudDmaChn_e eDmaChannel, BOOL bEnable);
BOOL DrvAudEnableI2s(AudI2sIf_e eI2sIf, BOOL bEnable);
BOOL DrvAudEnableAtop(AudAtopPath_e eAtop, BOOL bEnable);
BOOL DrvAudEnableHdmi(BOOL bEnable);
BOOL DrvAudEnableAmp(BOOL bEnable, S8 s8Ch);

BOOL DrvAudConfigDmaParam(AudDmaChn_e eDmaChannel, DmaParam_t *ptParam);
BOOL DrvAudOpenDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudPrepareDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudCloseDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudStartDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudStopDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudPauseDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudResumeDma(AudDmaChn_e eDmaChannel);
BOOL DrvAudIsXrun(AudDmaChn_e eDmaChannel);
BOOL DrvAudSetAdcGain(AudAdcSel_e eAdc, U16 nSel, S8 s8Ch);
BOOL DrvAudSetMicAmpGain(AudAdcSel_e eAdc, U16 nSel, S8 s8Ch);
BOOL DrvAudAdcSetMux(AudAdcSel_e eAdcSel);
BOOL DrvAudDpgaSetGain(AudDpga_e eDpga, S8 s8Gain, S8 s8Ch);
BOOL DrvAudDpgaSetGainFading(AudDpga_e eDpga, U8 nFading, S8 s8Ch);
BOOL DrvAudConfigDigMicParam(DigMicParam_t *ptDigMicParam);
BOOL DrvAudDigMicEnable(BOOL bEn);
BOOL DrvAudSetDigMicGain(S8 s8Gain, S8 s8Ch);
BOOL DrvAudGetBufCurrDataLen( AudDmaChn_e eDmaChannel, u32 *len );
BOOL DrvAudConfigAecParam(AecParam_t *ptAecParam);
BOOL DrvAudGetStartPts( AudDmaChn_e eDmaChannel, u64 *pnPts );
BOOL DrvAudDmaSineGen(AudDmaChn_e eDmaChannel, BOOL bEnable, U8 freq, U8 gain);

s32 DrvAudWritePcm(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock);
s32 DrvAudReadPcm(AudDmaChn_e eDmaChannel, void *pRdBuffer, u32 nSize, BOOL bBlock, u64 *puAiPts);

#endif //__DRV_AUDIO_H__
