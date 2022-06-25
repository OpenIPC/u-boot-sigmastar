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

#ifndef _HAL_DISP_CHIP_H_
#define _HAL_DISP_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_CTX_MAX_INST   2

// Device Ctx
#define HAL_DISP_DEVICE_MAX     1

#define HAL_DISP_VIDLAYER_MAX   1

#define HAL_DISP_INPUTPORT_NUM  1
#define HAL_DISP_INPUTPORT_MAX  (HAL_DISP_VIDLAYER_MAX  * HAL_DISP_INPUTPORT_NUM)


// Vga HPD Isr
#define HAL_DISP_VGA_HPD_ISR_SUPPORT            1
#define HAL_DISP_DEVICE_IRQ_VGA_HPD_ISR_IDX     2
#define E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF      (E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON | E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)

#define HAL_DISP_MOPG_GWIN_NUM  16 //MOPG 16 Gwins
#define HAL_DISP_MOPS_GWIN_NUM  1  //MOPS 1 Gwin



// IRQ CTX
#define HAL_DISP_DEVICE_IRQ_MAX 1

// Inernal Isr
#define HAL_DISP_INTERNAL_ISR_SUPPORT           0
#define HAL_DISP_DEVICE_IRQ_INTERNAL_ISR_IDX    1
#define E_HAL_DISP_IRQ_TYPE_INTERNAL_TIMEZONE   (E_HAL_DISP_IRQ_TYPE_INTERNAL_VSYNC_POSITIVE | E_HAL_DISP_IRQ_TYPE_INTERNAL_VDE_POSITIVE | E_HAL_DISP_IRQ_TYPE_INTERNAL_VDE_NEGATIVE)


#define HAL_DISP_CLK_ODCLK    3  //clk_odclk=3 (lpll_clk)

#define HAL_DISP_CLK_ON_SETTING \
{ \
    1,\
}

#define HAL_DISP_CLK_OFF_SETTING \
{ \
    0,\
}


#define HAL_DISP_CLK_RATE_SETTING \
{ \
    HAL_DISP_CLK_ODCLK, \
}

#define 	HAL_DISP_CLK_NUM										1
#define 	HAL_DISP_CLK_MUX_ATTR \
{ \
    0, \
}


#define HAL_DISP_CLK_NAME \
{   \
    "CLK_odclk",         \
}


//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif

