/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
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
