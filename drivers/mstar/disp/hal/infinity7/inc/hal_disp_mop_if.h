/*
* hal_disp_mop_if.h- Sigmastar
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

#ifndef _HAL_DISP_MOP_IF_H_
#define _HAL_DISP_MOP_IF_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_MOP_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoVidLayerInit(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortInit(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortInit(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortEnable(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortEnable(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortAttr(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortAttr(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortFlip(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortFlip(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortRotate(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortRotate(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortCrop(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortCrop(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoStretchWinSize(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInfoStretchWinSize(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInfoHextSize(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfInputPortDeinit(void);
INTERFACE void              HalDispMopIfSetInfoStretchWinMop0Auto(void *pCtx, u8 bEnAuto);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoInputPortRingBuffAttr(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetInputPortRingBuffAttr(void *pCtx, void *pCfg);
INTERFACE HalDispQueryRet_e HalDispMopIfGetInfoImiStartAddr(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetImiStartAddr(void *pCtx, void *pCfg);
INTERFACE void              HalDispMopIfSetDdbfWr(void *pCtx);
INTERFACE void              HalDispMopIfSetForceWr(void *pCtx, u8 bEn);
INTERFACE void              HalDispMopIfSetYDmaThreshold(void *pCtx, HalDispMopId_e enMopId, u8 u8Threshold);
INTERFACE void              HalDispMopIfSetCDmaThreshold(void *pCtx, HalDispMopId_e enMopId, u8 u8Threshold);


#undef INTERFACE
#endif
