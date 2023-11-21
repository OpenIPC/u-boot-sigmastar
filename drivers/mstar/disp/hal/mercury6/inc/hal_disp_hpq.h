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

INTERFACE void HalDispHpqLoadBin(void *pCtx, void *pCfg);
INTERFACE void HalDispHpqLoadData(u16 u16Id, u8 *pu8Data, u32 u32DataSize);

#undef INTERFACE
#endif
