/*
* hal_disp_hpq.h- Sigmastar
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

#ifndef _HAL_DISP_HPQ_H_
#define _HAL_DISP_HPQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_HPQ_H_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void                       HalDispHpqLoadBin(void *pCtx, void *pCfg);
INTERFACE void                       HalDispHpqFreeBin(void *pCtx, void *pCfg);
INTERFACE void                       HalDispHqpLoadSetting(void *pCtx);
INTERFACE void                       HalDispHpqSetSrcId(void *pCtx, u16 u16DevId, u16 u16SrcId);
INTERFACE void                       HalDispHpqSetPnlId(void *pCtx, u16 u16PnlId);
INTERFACE void                       HalDispHpqLoadData(void *pCtx, u8 *pu8Data, u32 u32DataSize);
INTERFACE u8                         HalDispHpqGetSourceIdx(void *pCtx, u16 *pu16SrcIdx);
INTERFACE void                       HalDispHpqSetLoadSettingType(HalDispPqLoadSettingType_e eType);
INTERFACE HalDispPqLoadSettingType_e HalDispHpqGetLoadSettingType(void);
INTERFACE void                       HalDispHpqSetPeakingCoef(void *pCtx, u16 u16Coef1, u16 u16Coef2);
INTERFACE void HalDispHpqSetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode);

#undef INTERFACE
#endif
