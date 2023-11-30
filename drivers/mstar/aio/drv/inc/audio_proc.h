/*
* audio_proc.h- Sigmastar
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

#ifndef __AUDIO_PROC_H__
#define __AUDIO_PROC_H__

typedef struct
{
    int enable;
    int channels;
    int bit_width;

} AudProcInfoAi_t;

typedef struct
{
    int enable;
    int channels;
    int bit_width;

} AudProcInfoAo_t;

extern AudProcInfoAi_t gAudProInfoAiList[];
extern AudProcInfoAo_t gAudProInfoAoList[];

void AudioProcInit(void);
void AudioProcDeInit(void);

#endif
