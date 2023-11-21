/*
* hal_disp.h- Sigmastar
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

#ifndef _HAL_DISP_H_
#define _HAL_DISP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef struct
{
    bool bUsed;
}HalDispHwContain_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE void HalDispSetY2RMatrix(void);
INTERFACE void HalDispSetOutputWidth(u16 u16Val);
INTERFACE void HalDispSetOutputHeight(u16 u16Val);
INTERFACE void HalDispSetBaseAddr(u32 u32YBase, u32 u32CBase, u16 u16FrameIdx);
INTERFACE void HalDispSetOutPutOnOff(bool bEn);
INTERFACE void HalDispSetColorFormat(HalDispPixelFormat_e enPixeFmt);
INTERFACE void HalDipSetBaseNum(u16 u16BaseNum);


#undef INTERFACE
#endif
