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


INTERFACE bool HalPnlGetLpllIdx(u32 u32Dclk, u16 *pu16Idx, bool bDsi);
INTERFACE u16  HalPnlGetLpllGain(u16 u16Idx, bool bDsi);
INTERFACE u16  HalPnlGetLpllDiv(u16 u16Idx, bool bDsi);
INTERFACE void HalPnlSetAllPadIn(void);
INTERFACE void HalPnlSetTTLPadMux(u16 u16Val);
INTERFACE void HalPnlSetBTPadMux(HalPnlLinkType_e eLinkType, u16 u16Val);
INTERFACE void HalPnlSetRGBPadMux(HalPnlOutputFormatBitMode_e enFmt, u16 u16Val);
INTERFACE void HalPnlSetMipiDisPadMux(HalPnlMipiDsiLaneMode_e enLaneNum);
INTERFACE void HalPnlSetTtlPadCotnrol(u32 u32Ie, u32 u32Pe, u32 u32Ps, u32 u32Drv);
INTERFACE void HalPnlDumpLpllSetting(u16 u16Idx);
INTERFACE void HalPnlSetLpllSet(u32 u32LpllSet);
INTERFACE void HalPnlSetFrameColorEn(bool bEn, void *pCtx);
INTERFACE void HalPnlSetFrameColor(u16 u16R, u16 u16G, u16 u16B, void *pCtx);
INTERFACE void HalPnlSetVSyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVSyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVfdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVfdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetVtt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHSyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHSyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHfdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHfdeEnd(u16 u16Va, void *pCtxl);
INTERFACE void HalPnlSetHdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetHtt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDeInv(bool bEn);
INTERFACE void HalPnlSetVsynRefMd(bool bEn);
INTERFACE void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk);
INTERFACE void HalPnlSetTgenExtHsEn(u8 u8Val, void *pCtx);
INTERFACE void HalPnlSetSwReste(u8 u8Val, void *pCtx);
INTERFACE void HalPnlSetDacHsyncSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDacHsyncEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDacHdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDacHdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDacVdeSt(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDacVdeEnd(u16 u16Val, void *pCtx);
INTERFACE void HalPnlSetDitherEn(bool bEn, void *pCtx);
INTERFACE void HalPnlSetRgbMode(HalPnlOutputFormatBitMode_e enFmt, void *pCtx);
INTERFACE void HalPnlSetDispToDsiMd(u8 u8Val, void *pCtx);
INTERFACE void HalPnlSetLpllSkew(u16 u16Val);
INTERFACE void HalPnlSetRgbSwap(HalPnlRgbSwapType_e enChR, HalPnlRgbSwapType_e enChG, HalPnlRgbSwapType_e enChB, void *pCtx);
INTERFACE void HalPnlSetRgbMlSwap(u8 u8Val, void *pCtx);
INTERFACE void HalPnlSetSscEn(u8 u8Val);
INTERFACE void HalPnlSetSscSpan(u16 u16Val);
INTERFACE void HalPnlSetSscStep(u16 u16Val);
INTERFACE u32  HalPnlGetLpllSet(void);
INTERFACE u32  HalPnlGetRevision(void);

//CLK
INTERFACE u32  HalPnlClkMapMipiTxDsiToIdx(u32 u32ClkRate);
INTERFACE u32  HalPnlClkMapMipiTxDsiToRate(u32 u32ClkIdx);
INTERFACE void HalPnlClkSetMipiTxDsi(bool bEn, u32 u32ClkRate);
INTERFACE void HalPnlClkGetMipiTxDsi(bool *pbEn, u32 *pu32ClkRate);


// LCD / LCD_MISC
INTERFACE void HalPnlSetLcdRgbVSyncWidth(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbVBp(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbVPorchTotal(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbVActive(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHSyncWidth(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHBp(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHPorchTotal(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHActive(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbFramePixelCnt(u32 u32Cnt);
INTERFACE void HalPnlSetLcdRgbDeltaMode(HalPnlRgbDeltaMode_e eOddLine, HalPnlRgbDeltaMode_e eEvenLine);
INTERFACE void HalPnlSetLcdRgbDswap(u8 u8Val);
INTERFACE void HalPnlSetLcdRgbDtype(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbEn(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbVSyncPol(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHSyncPol(u16 u16Val);
INTERFACE void HalPnlSetLcdMiscTgenSyncEn(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbVsyncPol(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbHsyncPol(u16 u16Val);
INTERFACE void HalPnlSetLcdRgbClkPol(u16 u16Val);
INTERFACE void HalPnlSetLcdDisp1Used(u16 u16Val);
INTERFACE void HalPnlSetLcdDisp1Type(u16 u16Val);
INTERFACE void HalPnlSetLcdBtEncMode(u16 u16Val);
INTERFACE void HalPnlSetLcdBtClip(u16 u16Val);
INTERFACE void HalPnlSetLcdBtEncModeEn(u16 u16Val);
INTERFACE void HalPnlSetLcdBt601En(u16 u16Val);
INTERFACE void HalPnlSetDsiGateEn(u16 u16Val);
INTERFACE void HalPnlSetLcdMiscDacMux(u16 u16Val);


//MCU LCD (intel8080 or Motorola 8068)
INTERFACE void HalPnlSetMCUPadMux(HalPnlMcuDataBusCfg databus, u16 u16Val);
INTERFACE void HalPnlMcuSetBus(HalPnlMcuDataBusCfg databus);
INTERFACE void HalPnlMcuCtl(void);
INTERFACE void HalPnlSetMcuIDXCmdNum(u8 u8TXNum);
INTERFACE void HalPnlSetMcuAutoConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd);
INTERFACE void HalPnlSetMcuType(HalPnlMcuType enMcuSys);
INTERFACE void HalPnlSetMcuPhase(bool bPHA);
INTERFACE void HalPnlSetMcuPolar(bool bPOR);
INTERFACE void HalPnlSetMcuRsPolarity(bool bRsPor);

INTERFACE void HalPnlSetMCUIdxCmdTotalNum(u8 u8Num);
INTERFACE void HalPnlSetMCUCycleCnt(u8 u8RSLeadCs, u8 u8CSLeadRW, u8 u8RW_Cnt);
INTERFACE void HalPnlSetMcuUnifiedAutoCmdBuff(u32 *pu32CmdBuff, u32 u32CmdNum);
INTERFACE void HalPnlSetMcuUnifiedInitCmdBuff(u32 *pu32CmdBuff, u32 u32CmdNum);

// Mipi Dsi
INTERFACE void HalPnlInitMipiDsiDphy(void);
INTERFACE void HalPnlSetMipiDsiPadOutSel(HalPnlMipiDsiLaneMode_e enLaneMode);
INTERFACE void HalPnlResetMipiDsi(void);
INTERFACE void HalPnlEnableMipiDsiClk(void);
INTERFACE void HalPnlDisableMipiDsiClk(void);
INTERFACE void HalPnlSetMipiDsiLaneNum(HalPnlMipiDsiLaneMode_e enLaneMode);
INTERFACE void HalPnlSetMipiDsiCtrlMode(HalPnlMipiDsiCtrlMode_e enCtrlMode);
INTERFACE bool HalPnlGetMipiDsiClkHsMode(void);
INTERFACE void HalPnlSetMpiDsiClkHsMode(bool bEn);
INTERFACE bool HalPnlGetMipiDsiShortPacket(u8 u8ReadBackCount, u8 u8RegAddr);
INTERFACE bool HalPnlSetMipiDsiShortPacket(HalPnlMipiDsiPacketType_e enPacketType, u8 u8Count, u8 u8Cmd, u8 *pu8ParamList);
INTERFACE bool HalPnlSetMipiDsiLongPacket(HalPnlMipiDsiPacketType_e enPacketType, u8 u8Count, u8 u8Cmd, u8 *pu8ParamList);
INTERFACE bool HalPnlSetMipiDsiPhyTimConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg);
INTERFACE bool HalPnlSetMipiDsiVideoTimingConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg, HalPnlHwMipiDsiConfig_t *pstHwCfg);
INTERFACE void HalPnlGetMipiDsiReg(u32 u32Addr, u32 *pu32Val);
INTERFACE void HalPnlSetMipiDsiChPolarity(u8 *pu8ChPol);
INTERFACE void HalPnlSetMipiDsiChSel(HalPnlMipiDsiConfig_t *pstMipiDisCfg);
INTERFACE void HalPnlSetMipiDsiPatGen(void);

#undef INTERFACE
#endif
