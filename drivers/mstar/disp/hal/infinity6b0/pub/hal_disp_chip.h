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

#ifndef _HAL_DISP_CHIP_H_
#define _HAL_DISP_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_CTX_MAX_INST   2
//------------------------------------------------------------------------------
// Device Ctx
#define HAL_DISP_DEVICE_MAX     1
//------------------------------------------------------------------------------
// VideoLayer Ctx
#define HAL_DISP_VIDLAYER_MAX   1
//------------------------------------------------------------------------------
// InputPort Ctx
#define HAL_DISP_INPUTPORT_NUM  1
#define HAL_DISP_INPUTPORT_MAX  (HAL_DISP_VIDLAYER_MAX  * HAL_DISP_INPUTPORT_NUM)
//------------------------------------------------------------------------------
// DMA  Ctx
#define HAL_DISP_DMA_MAX                    0
#define HAL_DISP_DMA_PIX_FMT_PLANE_MAX      3
//------------------------------------------------------------------------------
// IRQ CTX
#define HAL_DISP_DEVICE_IRQ_MAX 1

#define HAL_DISP_IRQ_CFG \
{ \
    {0, 0}, \
}

// TimeZone
#define HAL_DISP_TIMEZONE_ISR_SUPPORT           0
#define HAL_DISP_DEVICE_IRQ_TIMEZONE_ISR_IDX    1

#define E_HAL_DISP_IRQ_TYPE_TIMEZONE            (1)
#define E_HAL_DISP_IRQ_TYPE_INTERNAL_TIMEZONE   (E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)


// Vga HPD Isr
#define HAL_DISP_VGA_HPD_ISR_SUPPORT            0
#define HAL_DISP_DEVICE_IRQ_VGA_HPD_ISR_IDX     2
#define E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF      (E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON | E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)

#define CLK_MHZ(x)                  (x*1000000)
#define HAL_DISP_CLK_MOP_RATE       CLK_MHZ(320)
#define HAL_DISP_CLK_HDMI_RATE      0
#define HAL_DISP_CLK_DAC_RATE       0
#define HAL_DISP_CLK_DISP_432_RATE  CLK_MHZ(432)
#define HAL_DISP_CLK_DISP_216_RATE  CLK_MHZ(216)

#define HAL_DISP_CLK_NUM            1

#define HAL_DISP_CLK_ON_SETTING \
{ \
    1,\
}

#define HAL_DISP_CLK_OFF_SETTING \
{ \
    0, \
}


#define HAL_DISP_CLK_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
}

#define HAL_DISP_CLK_MUX_ATTR \
{ \
    0, \
}

#define HAL_DISP_CLK_NAME \
{   \
    "mop", \
}
//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif

