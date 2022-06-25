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

#define _HAL_DISP_C_
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

void HalDispSetY2RMatrix(void)
{
    W2BYTE(REG_SCL2_10_L, 0x1000);  //disable frame color

    W2BYTE(REG_SCL2_19_L, 0x1001);  // cm_en
    W2BYTE(REG_SCL2_1A_L, 0x8080);  // cm_en
    W2BYTE(REG_SCL2_1B_L, 0x0cc4);  // cm_11
    W2BYTE(REG_SCL2_1C_L, 0x0950);  // cm_12
    W2BYTE(REG_SCL2_1D_L, 0x3ffc);  // cm_13
    W2BYTE(REG_SCL2_1E_L, 0x397e);  // cm_21
    W2BYTE(REG_SCL2_1F_L, 0x0950);  // cm_22
    W2BYTE(REG_SCL2_20_L, 0x3cde);  // cm_23
    W2BYTE(REG_SCL2_21_L, 0x3ffe);  // cm_31
    W2BYTE(REG_SCL2_22_L, 0x0950);  // cm_32
    W2BYTE(REG_SCL2_23_L, 0x1024);  // cm_33
}

void HalDispSetOutputWidth(u16 u16Val)
{
    W2BYTE(REG_SCL_DMA0_32_L, u16Val);
}

void HalDispSetOutputHeight(u16 u16Val)
{
    W2BYTE(REG_SCL_DMA0_33_L, u16Val);
}

void HalDispSetBaseAddr(u32 u32YBase, u32 u32CBase, u16 u16FrameIdx)
{
    u16 u16Base_Lo, u16Base_Hi;
    u32YBase = u32YBase >> 3;
    u32CBase = u32CBase >> 3;

    u16Base_Lo = u32YBase & 0xFFFF;
    u16Base_Hi = u32YBase >> 16;

    W2BYTE(REG_SCL_DMA0_20_L + (u16FrameIdx * 4), u16Base_Lo);
    W2BYTE(REG_SCL_DMA0_21_L + (u16FrameIdx * 4), u16Base_Hi);

    u16Base_Lo = u32CBase & 0xFFFF;
    u16Base_Hi = u32CBase >> 16;
    W4BYTE(REG_SCL_DMA0_28_L + (u16FrameIdx * 4), u16Base_Lo);
    W4BYTE(REG_SCL_DMA0_29_L + (u16FrameIdx * 4), u16Base_Hi);
}

void HalDipSetBaseNum(u16 u16BaseNum)
{
    W2BYTEMSK(REG_SCL_DMA0_60_L, (u16BaseNum-1)<<3, DISP_BIT5|DISP_BIT4|DISP_BIT3);
}

void HalDispSetOutPutOnOff(bool bEn)
{
    W2BYTEMSK(REG_SCL_DMA0_02_L, bEn ? DISP_BIT15 : 0, DISP_BIT15);
}

void HalDispSetColorFormat(HalDispPixelFormat_e enPixeFmt)
{
    bool bYuv422;

    bYuv422 = (enPixeFmt == E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV ||
               enPixeFmt == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422) ? 1 : 0;

    W2BYTEMSK(REG_SCL_DMA0_02_L, bYuv422 ? DISP_BIT14 : 0 , DISP_BIT14);
    W2BYTEMSK(REG_SCL_DMA0_36_L, bYuv422 ? 0x00C6 : 0x0046, 0xFFFF);
}

void HalDispSetMaceSrc(bool bExtVideo)
{

}
void HalDispSetPatGenMd(u8 u8Val)
{

}

