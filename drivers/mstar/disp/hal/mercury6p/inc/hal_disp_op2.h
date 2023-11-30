/*
* hal_disp_op2.h- Sigmastar
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

#ifndef _HAL_DISP_OP2_H_
#define _HAL_DISP_OP2_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_OP2_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void HalDispSetSwReset(u8 u8Val, void *pCtx, u32 u32DevId);
INTERFACE void HalDispSetHdmitxRst(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetVgaDtopDumpByDiv(u32 u32Mulit, u32 u32Div, void *pCtx);
INTERFACE void HalDispSetHdmitxAtopDumpByDiv(u32 u32Mulit, u32 u32Div, void *pCtx);
INTERFACE void HalDispClearDisp0SwReset(void *pCtx);
INTERFACE void HalDispSetPatGenMd(u8 u8Val, void *pCtx, u32 u32DevId);
INTERFACE void HalDispSetTgenHtt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVtt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHsyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHsyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVsyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVsyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHfdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHfdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVfdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVfdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenHdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenVdeEnd(u16 u16Val, void *pCtx);
;
INTERFACE void HalDispSetTgenDacHsyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenDacHsyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenDacHdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenDacHdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenDacVdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenDacVdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetTgenExtHsEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetRgbOrder(void *pCtx, u8 u8R, u8 u8G, u8 u8B);
INTERFACE void HalDispSetRgbMode(void *pCtx, HaldispDeviceRgbType_e enMode, u8 bMlSwap);
INTERFACE void HalDispSetDacMux(void *pCtx, u8 u8Val);
INTERFACE u16  HalDispGetTgenHtt(void *pCtx);
INTERFACE u16  HalDispGetTgenVtt(void *pCtx);
INTERFACE u16  HalDispGetTgenHsyncSt(void *pCtx);
INTERFACE u16  HalDispGetTgenHsyncEnd(void *pCtx);
INTERFACE u16  HalDispGetTgenHfdeSt(void *pCtx);
INTERFACE u16  HalDispGetTgenHfdeEnd(void *pCtx);
INTERFACE u16  HalDispGetTgenHdeSt(void *pCtx);
INTERFACE u16  HalDispGetTgenHdeEnd(void *pCtx);
INTERFACE void HalDispSetFrameColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx);
INTERFACE void HalDispSetFrameColorForce(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetDispWinColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx);
INTERFACE void HalDispSetDispWinColorForce(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetDacTrimming(u16 u16R, u16 u16G, u16 u16B, void *pCtx);
INTERFACE void HalDispSetDacAffReset(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetDacReset(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetVgaHpdInit(void *pCtx);
INTERFACE void HalDispSetHdmitxSsc(u16 u16Step, u16 u16Span, void *pCtx);
INTERFACE void HalDispGetDacTriming(u16 *pu16R, u16 *pu16G, u16 *pu16B, void *pCtx);

// DAC
INTERFACE void HalDispSetDacFpllDly(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDacFpllRefSel(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDacExtFrmRstEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetVgaHVsyncPad(void *pCtx, bool bEn);
INTERFACE void HalDispSetDacSynthSetSel(u16 u16Val, void *pCtx);
INTERFACE void HaldispSetDacFpllSynthSwRst(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetHdmitxSynth(u32 u32Val, void *pCtx);
INTERFACE void HalDispSetVgaSynth(u32 u32Val, void *pCtx);

INTERFACE void HalDispSetDacRgbHpdInit(void);
INTERFACE void HalDispSetFpllEn(u16 u16FpllId, u8 u8Val, void *pCtx);
INTERFACE void HalDispSetFpllRst(u16 u16FpllId, u8 u8Val, void *pCtx);
INTERFACE void HalDispClearFpllUnLock(u16 u16FpllId, void *pCtx);
INTERFACE void HalDispIrqClearFpllUnLock(u32 u32DevId);
INTERFACE void HalDispPollFpllLock(u16 u16FpllId, void *pCtx);
INTERFACE void HalDispSetFpllGain(u16 u16FpllId, u16 u16Val, void *pCtx);
INTERFACE void HalDispSetFpllThd(u16 u16FpllId, u16 u16Val, void *pCtx);
INTERFACE void HalDispSetFpllAbsLimit(u16 u16FpllId, u32 u32Val, void *pCtx);
INTERFACE void HalDispSetDacPllPowerDown(u16 u16Val, void *pCtx);
INTERFACE void HaldispSetDacSwRst(HalDispInternalConfigType_e enType, u16 u16Val);
INTERFACE void HalDispIrqFpllSwRst(u32 u32FpllId, u16 u16Val);
INTERFACE void HalDispIrqEnableDoubleVsync(u8 u8bEn, void *pCtx);
INTERFACE void HalDispIrqEnableVsync(u8 u8bEn, u32 u32DmaId, void *pCtx);

// LCD
INTERFACE void HalDispSetLcdAffReset(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetLcdFpllDly(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetLcdFpllRefSel(u16 u16Val, void *pCtx);

// Dsi
INTERFACE void HalDispSetDsiClkGateEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDsiAff(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDsiRst(u16 u16Val, void *pCtx);

INTERFACE void HalDispSetHdmi2OdClkRate(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDacSrcMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetCvbsSrcMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetHdmiSrcMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetLcdSrcMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetDsiSrcMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetCmdqIntMask(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetCmdqIntClear(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetMaceSrcSel(u16 u16Val, void *pCtx, u32 u32DevId);
INTERFACE void HalDispSetMopWinMerge(u16 u16Val, void *pCtx);
INTERFACE void HalDispSetCvbsSwRst(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetCmdqMux(u8 u8Val, void *pCtx);
INTERFACE void HalDispSetCvbsOnOff(u8 u8Val, void *pCtx);

// MCU LCD used
INTERFACE void HalDispSetClkDsiGateEn(u8 u8Val, void *pCtx);
INTERFACE void HalDispLcdAffGateEn(u8 u8Val, void *pCtx);
INTERFACE void HalDispMcuLcdRefresh(u8 u8Val, void *pCtx);
INTERFACE void HalDispMcuLcdRest(u8 u8Val, void *pCtx);
INTERFACE void HalDispMcuLcdCmdqInit(void);
INTERFACE void HalDispKeepMcuReset(u8 u8Val, void *pCtx);

INTERFACE void HalDispDumpRegTab(void *pCtx, u16 *pData, u16 u16RegNum, u16 u16DataSize, u8 u8DataOffset);
INTERFACE void HalDispSetFpllUsedFlag(u16 u16FpllId, u16 u16Dev, void *pCtx);
INTERFACE u16  HalDispGetFpllUsedByDev(u16 u16FpllId, u16 u16Dev);
INTERFACE void HalDispSetFpllLockedStableFlag(u16 u16FpllId, u16 u16Val, void *pCtx);
INTERFACE u16  HalDispGetFpllLockedStableFlag(u16 u16FpllId);
INTERFACE void HalDispSetTimeGenStartFlag(u16 u16Val, void *pCtx, u32 u32DevId);
INTERFACE void HalDispSetInterCfgRstMop(u16 u16Val, u32 u32DevId);
INTERFACE void HalDispSetInterCfgRstLcd(u16 u16Val);
INTERFACE void HalDispSetInterCfgRstHdmitx(u16 u16Val);
INTERFACE void HalDispOp2SetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode);

#undef INTERFACE
#endif
