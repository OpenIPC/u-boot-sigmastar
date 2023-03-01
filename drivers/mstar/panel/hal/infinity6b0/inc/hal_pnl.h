/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
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
INTERFACE void HalPnlSetChipTop(u32 u32Mode);
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
INTERFACE void HalPnlEnableBT656(u8 u8En);
INTERFACE void HalPnlSetY2RMatrix(bool bEn);
INTERFACE void HalPnlSetLSSC(u8 u8SscEn, u32 u32SSCStep, u32 u32SSCSpan);
INTERFACE void HalPnlSetBt656ClkInv(bool bEn);
INTERFACE void HalPnlUnifiedSetMLSwap(bool bSwap);
INTERFACE void HalPnlUnifiedSetFixedClk(u32 u32FixedClkRate);

#undef INTERFACE
#endif
