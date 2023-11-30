/*
* drv_hdmitx_if.h- Sigmastar
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

#ifndef _DRV_HDMITX_IF_H_
#define _DRV_HDMITX_IF_H_

#ifdef _DRV_HDMITX_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE MhalHdmitxRet_e DrvHdmitxIfCreateInstance(void **pCtx, u32 u32Id);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfGetInstance(void **pCtx, u32 u32Id);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfDestoryInstance(void *pCtx);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetAttrBegin(void *pCtx);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetAttr(void *pCtx, MhalHdmitxAttrConfig_t *pstAttrCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetAttrEnd(void *pCtx);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetMute(void *pCtx, MhalHdmitxMuteConfig_t *pstMuteCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetSignal(void *pCtx, MhalHdmitxSignalConfig_t *pstSignalCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfGetSinkInfo(void *pCtx, MhalHdmitxSinkInfoConfig_t *pstSinkInfo);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetInfoFrame(void *pCtx, MhalHdmitxInfoFrameConfig_t *pstInfoFrameCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetAnalogDrvCur(void *pCtx, MhalHdmitxAnaloDrvCurConfig_t *pstDrvCurCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetDebugLevel(void *pCtx, u32 u32DbgLevel);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetHpdConfig(void *pCtx, MhalHdmitxHpdConfig_t *pstHpdCfg);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfSetClk(void *pCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
INTERFACE MhalHdmitxRet_e DrvHdmitxIfGetClk(void *pCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
INTERFACE u32 DrvHdmitxDebugForceCfgShow(u8 *DstBuf);
INTERFACE u32 DrvHdmitxDebugCfgShow(u8 *DstBuf);
INTERFACE u32 DrvHdmitxDebugEdidShow(u8 *DstBuf);
INTERFACE void DrvHdmitxDebugForceCfgStore(DrvHdmitxOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvHdmitxDebugClkShow(u8 *DstBuf);
INTERFACE void DrvHdmitxDebugClkStore(DrvHdmitxOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvHdmitxDebugPtGenShow(u8 *DstBuf);
INTERFACE void DrvHdmitxDebugPtGenStore(DrvHdmitxOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvHdmitxDebugDbgmgShow(u8 *DstBuf);
INTERFACE void DrvHdmitxDebugDbgmgStore(DrvHdmitxOsStrConfig_t *pstStringCfg);
#undef INTERFACE

#endif
