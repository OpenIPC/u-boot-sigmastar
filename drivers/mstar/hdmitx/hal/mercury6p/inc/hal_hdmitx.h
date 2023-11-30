/*
* hal_hdmitx.h- Sigmastar
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

#ifndef _HAL_HDMITX_H_
#define _HAL_HDMITX_H_
#include "halHDMIUtilTx.h"

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
void                HalHdmitxPowerOnOff(u8 bEnable, u32 u32Dev);
void                HalHdmitxDeviceOnOff(u8 bEnable, u32 u32Dev);
void                HalHdmitxIrqDisable(u32 u32Int, u32 u32Dev);
void                HalHdmitxIrqEnable(u32 u32Int, u32 u32Dev);
void                HalHdmitxIrqClear(u32 u32Int, u32 u32Dev);
u32                 HalHdmitxIrqGetStatus(u32 u32Dev);
void                HalHdmitxInitSeq(u32 u32Dev);
u8                  HalHdmitxAdjustDDCFreq(u32 u32Dev, u32 u32Speed_K);
void                HalHdmitxInitPadTop(u32 u32Dev);
HalHdmitxRxStatus_e HalHdmitxGetRXStatus(u8 u8bHpd, u32 u32Dev);
u16                 HalHdmitxGetTMDS(u32 u32Dev);
void                HalHdmitxSetTMDSOnOff(u32 u32Dev, u8 bRB_Swap, u8 bTMDS);
void HalHdmitxSetVideoOutputMode(u32 u32Dev, HalHdmitxTimingResType_e Timing, HalHdmitxAnaloDrvCurConfig_t *pInfo);
void HalHdmitxSetAnalogDrvCur(u32 u32Dev, HalHdmitxAnaloDrvCurConfig_t *pCfg);
void HalHdmitxSetHDMImode(u32 u32Dev, u8 bflag, HalHdmitxColorDepthType_e cd_val);
u8   HalHdmitxColorandRangeTransform(u32 u32Dev, HalHdmitxColorType_e incolor, HalHdmitxColorType_e outcolor,
                                     HalHdmitxYccQuantRange_e inrange, HalHdmitxYccQuantRange_e outrange);
void HalHdmitxSetAudioFrequency(u32 u32Dev, HalHdmitxAudioFreqType_e afidx, HalHdmitxAudioChannelType_e achidx,
                                HalHdmitxAudioCodingType_e actidx, HalHdmitxTimingResType_e vidtiming);
void HalHdmitxSetVideoOnOff(u32 u32Dev, u8 bVideo, u8 bCSC, u8 b709format);
void HalHdmitxSetAudioOnOff(u32 u32Dev, u8 bflag);
u8   HalHdmitxGetClk(u8 *pbEn, u32 *pu32ClkRate, u32 u32Size);
u8   HalHdmitxSetClk(u8 *pbEn, u32 *pu32ClkRate, u32 u32Size);
void HalHdmitxSendInfoFrame(HalHdmitxResourcePrivate_t *psHDMITXResPri, HalHdmitxInfoFrameType_e enPktType,
                            HalHdmitxPacketProcess_e packet_process);
void HalHdmitxSendPacket(HalHdmitxResourcePrivate_t *psHDMITXResPri, HalHdmitxPacketType_e enPktType,
                         HalHdmitxPacketProcess_e packet_process);
u8   HalHdmitxInfoFrameContentDefine(u32 u32Dev, HalHdmitxInfoFrameType_e packet_type, u8 *data, u8 length);
HalHdmitxAviVic_e HalHdmitxMapVideoTimingToVIC(HalHdmitxTimingResType_e timing);

#endif // _HAL_HDMITX_H_
