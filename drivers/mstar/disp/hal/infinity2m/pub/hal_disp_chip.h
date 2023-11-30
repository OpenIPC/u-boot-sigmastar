/*
* hal_disp_chip.h- Sigmastar
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
// VideoLayer  Ctx
#define HAL_DISP_VIDLAYER_MAX   2 // MOPG & MOPS
//------------------------------------------------------------------------------
// InputPort Ctx
#define HAL_DISP_MOPG_GWIN_NUM  16 //MOPG 16 Gwins
#define HAL_DISP_MOPS_GWIN_NUM  1  //MOPS 1 Gwin

#define HAL_DISP_INPUTPORT_NUM  (HAL_DISP_MOPG_GWIN_NUM+HAL_DISP_MOPS_GWIN_NUM) // MOP: 16Gwin_MOPG + 1Gwin_MOPS
#define HAL_DISP_INPUTPORT_MAX  (HAL_DISP_VIDLAYER_MAX  * HAL_DISP_INPUTPORT_NUM)
//------------------------------------------------------------------------------
// Dma Ctx
#define HAL_DISP_DMA_MAX                        1
#define HAL_DISP_DMA_PIX_FMT_PLANE_MAX          3
//------------------------------------------------------------------------------
// IRQ CTX
#define HAL_DISP_IRQ_ID_MAX     3
#define HAL_DISP_IRQ_CFG \
{ \
    {0, 0}, \
    {0, 0}, \
    {0, 0}, \
}
//------------------------------------------------------------------------------
// TIMEZONE Isr
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_LINUX     0
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_UBOOT     0

#define HAL_DISP_DEVICE_IRQ_TIMEZONE_ISR_IDX    1
#define E_HAL_DISP_IRQ_TYPE_TIMEZONE            (E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
//------------------------------------------------------------------------------
// Vga HPD Isr
#define HAL_DISP_VGA_HPD_ISR_SUPPORT_LINUX      0
#define HAL_DISP_VGA_HPD_ISR_SUPPORT_UBOOT      0

#define HAL_DISP_DEVICE_IRQ_VGA_HPD_ISR_IDX     2
#define E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF      (E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON | E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)


#define CLK_MHZ(x)                  (x*1000000)
#define HAL_DISP_CLK_MOP_RATE       CLK_MHZ(320)
#define HAL_DISP_CLK_HDMI_RATE      0
#define HAL_DISP_CLK_DAC_RATE       0
#define HAL_DISP_CLK_DISP_432_RATE  CLK_MHZ(432)
#define HAL_DISP_CLK_DISP_216_RATE  CLK_MHZ(216)

#if defined(HDMITX_VGA_SUPPORTED)

#define HAL_DISP_CLK_NUM            5

#define HAL_DISP_CLK_ON_SETTING \
{ \
    1, 1, 1, 1, 1, \
}

#define HAL_DISP_CLK_OFF_SETTING \
{ \
    0, 0, 0, 0, 0,\
}


#define HAL_DISP_CLK_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_HDMI_RATE, \
    HAL_DISP_CLK_DAC_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
}

#define HAL_DISP_CLK_OFF_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_HDMI_RATE, \
    HAL_DISP_CLK_DAC_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
}

#define HAL_DISP_CLK_MUX_ATTR \
{ \
    0, 1, 1, 0, 0, \
}

#define HAL_DISP_CLK_NAME \
{   \
    "mop", \
    "hdmi", \
    "dac", \
    "disp432", \
    "disp216", \
}
#else

#define HAL_DISP_CLK_NUM            3

#define HAL_DISP_CLK_ON_SETTING \
{ \
    1, 1, 1,\
}

#define HAL_DISP_CLK_OFF_SETTING \
{ \
    0, 0, 0,\
}


#define HAL_DISP_CLK_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
}

#define HAL_DISP_CLK_OFF_RATE_SETTING \
{ \
    HAL_DISP_CLK_MOP_RATE, \
    HAL_DISP_CLK_DISP_432_RATE, \
    HAL_DISP_CLK_DISP_216_RATE, \
}

#define HAL_DISP_CLK_MUX_ATTR \
{ \
    0, 0, 0, \
}
#define HAL_DISP_CLK_NAME \
{   \
    "mop", \
    "disp432", \
    "disp216", \
}

#endif
//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif

