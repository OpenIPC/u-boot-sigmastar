/*
* hal_pnl.h- Sigmastar
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

#ifndef _HAL_PNL_H_
#define _HAL_PNL_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_PNL_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE u16  HalPnlGetLpllIdx(u32 u32Dclk);
INTERFACE u16  HalPnlGetLpllGain(u16 u16Idx);
INTERFACE u16  HalPnlGetLpllDiv(u16 u16Idx);
//INTERFACE void HalPnlSetChipTop(bool bEn);
INTERFACE void HalPnlSetChipTop(bool bEn, HalPnlOutputFormatBitMode_e enOutBitMode, u8 u8PadMode);
INTERFACE void HalPnlSetClkSrc(u8 u8ClkSrc);

INTERFACE void HalPnlDumpLpllSetting(u16 u16Idx);
INTERFACE void HalPnlSetLpllSet(u32 u32LpllSet);
INTERFACE void HalPnlSetOpenLpllCLK(u8 bLpllClk);
INTERFACE void HalPnlSetFrameColorEn(bool bEn);
INTERFACE void HalPnlSetFrameColor(u16 u16R, u16 u16G, u16 u16B);
INTERFACE void HalPnlSetVSyncSt(u16 u16Val);
INTERFACE void HalPnlSetVSyncEnd(u16 u16Val);
INTERFACE void HalPnlSetVfdeSt(u16 u16Val);
INTERFACE void HalPnlSetVfdeEnd(u16 u16Val);
INTERFACE void HalPnlSetVdeSt(u16 u16Val);
INTERFACE void HalPnlSetVdeEnd(u16 u16Val);
INTERFACE void HalPnlSetVtt(u16 u16Val);
INTERFACE void HalPnlSetHSyncSt(u16 u16Val);
INTERFACE void HalPnlSetHSyncEnd(u16 u16Val);
INTERFACE void HalPnlSetHfdeSt(u16 u16Val);
INTERFACE void HalPnlSetHfdeEnd(u16 u16Val);
INTERFACE void HalPnlSetHdeSt(u16 u16Val);
INTERFACE void HalPnlSetHdeEnd(u16 u16Val);
INTERFACE void HalPnlSetHtt(u16 u16Val);
INTERFACE void HalPnlSetClkInv(bool bEn);
INTERFACE void HalPnlSetVsyncInv(bool bEn);
INTERFACE void HalPnlSetHsyncInv(bool bEn);
INTERFACE void HalPnlSetDeInv(bool bEn);
INTERFACE void HalPnlSetVsynRefMd(bool bEn);
INTERFACE void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk);
INTERFACE void HalPnlResetOdclk(bool bEn);
INTERFACE void HalPnlIfSetOutFormatConfig(HalPnlOutputFormatBitMode_e eFormatMode);
INTERFACE void HalPnlUnifiedSetBT656PadMux(u8 u8Mode);
INTERFACE void HalPnlSetUnifiedChipTop(u32 u32Mode);
INTERFACE void HalPnlUnifiedSetTTLPadMux(HalPnlOutputFormatBitMode_e enOutputFormat, u32 u32Mode);

INTERFACE void HalPnlEnableBT656(u8 u8En);
INTERFACE void HalPnlSetY2RMatrix(bool bEn);
INTERFACE void HalPnlSetLSSC(u8 u8SscEn, u32 u32SSCStep, u32 u32SSCSpan);
INTERFACE void HalPnlSetBt656ClkInv(bool bEn);
INTERFACE void HalPnlUnifiedSetMLSwap(bool bSwap);
INTERFACE void HalPnlUnifiedSetFixedClk(u32 u32FixedClkRate);
#undef INTERFACE
#endif
