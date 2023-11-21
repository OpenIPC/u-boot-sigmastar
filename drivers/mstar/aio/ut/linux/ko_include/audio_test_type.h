/*
* audio_test_type.h- Sigmastar
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

#ifndef _AUDIO_TEST_TYPE_H_
#define _AUDIO_TEST_TYPE_H_

typedef struct AUD_I2sCfg_s
{
    int nTdmMode;
    int nMsMode;
    int nBitWidth;
    int nFmt;
    int u16Channels;
    int e4WireMode;
    int eMck;
}_AUD_I2sCfg_s;

typedef struct AUD_PcmCfg_s
{
    int nRate;
    int nBitWidth;
    int n16Channels;
    int nInterleaved;
    int n32PeriodSize; //bytes
    int n32StartThres;
    int nTimeMs;
    int nI2sConfig;
    _AUD_I2sCfg_s sI2s;
    int nSpecialDumpChannels;
    char aoUseFile;
    char file_name[100];
}_AUDPcmCfg_t;

// Bench
typedef struct AUD_BenchRead_s
{
    void *aBufAddr;
    unsigned int nLen;
}_AUD_BenchRead_s;

typedef struct AUD_SineGenCfg_s
{
    unsigned char bEnable;
    unsigned char u8Freq;
    unsigned char u8Gain;
}_AUD_SineGenCfg_s;

typedef struct AUD_GainCfg_s
{
    int s16Gain;
    char s8Ch;
    
}_AUD_GainCfg_s;

#endif /* _AUDIO_TEST_TYPE_H_ */
