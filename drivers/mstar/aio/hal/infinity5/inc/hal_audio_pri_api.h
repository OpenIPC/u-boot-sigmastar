/*
* hal_audio_pri_api.h- Sigmastar
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

#ifndef __HAL_AUD_PRI_API_H__
#define __HAL_AUD_PRI_API_H__



BOOL HalAudPriSetPadMuxI2sTdm(S32 nPadMuxMode);
BOOL HalAudPriSetPadMuxDmic(S32 nPadMuxMode);
BOOL HalAudPriSetPadMuxMck(S32 nPadMuxMode);
BOOL HalAudPriSetPadMuxI2sMode(S32 nPadMuxMode);



#endif //__HAL_AUD_PRI_API_H__
