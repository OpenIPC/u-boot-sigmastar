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

#ifndef _HAL_DISP_H_
#define _HAL_DISP_H_

#include "hal_disp_chip.h"
#include "hal_disp_st.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------


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


INTERFACE void HalDispSetMaceSrc(bool bExtVideo);
INTERFACE void HalDispSetPatGenMd(u8 u8Val);

#undef INTERFACE
#endif
