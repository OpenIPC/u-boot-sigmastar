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
#define HAL_DISP_IRQ_MOP_VSYNC_BIT (0x0001) // BIT0
#define HAL_DISP_IRQ_MOP_VSYNC_MSK (0x0001)

#define HAL_DISP_RETRIGGER_CNT 5
#define HAL_DISP_IRQ_DMAMISC_DOUBLEVS_CNT  (5)
#define HAL_DISP_IRQ_DMA_DONE_BIT           (0x0020) // BIT5
#define HAL_DISP_IRQ_DMA_DONE_MSK           (0x0020)
#define HAL_DISP_IRQ_DMA_REALDONE_BIT       (0x0010) // BIT4
#define HAL_DISP_IRQ_DMA_REALDONE_MSK       (0x0010)
#define HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT      (0x0008) // BIT3
#define HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK      (0x0008)
#define HAL_DISP_IRQ_DMA_ACTIVE_OFF_BIT     (0x0004) // BIT2
#define HAL_DISP_IRQ_DMA_ACTIVE_OFF_MSK     (0x0004)
#define HAL_DISP_IRQ_DMA_FIFOFULL_BIT       (0x0002) // BIT1
#define HAL_DISP_IRQ_DMA_FIFOFULL_MSK       (0x0002)
#define HAL_DISP_IRQ_DMA_REGFAIL_BIT        (0x0001) // BIT0
#define HAL_DISP_IRQ_DMA_REGFAIL_MSK        (0x0001)
#define HAL_DISP_CMDQIRQ_DMA_DONE_BIT       (0x0008) // BIT3
#define HAL_DISP_CMDQIRQ_DMA_DONE_MSK       (0x0008)
#define HAL_DISP_CMDQIRQ_DMA_REALDONE_BIT   (0x0004) // BIT2
#define HAL_DISP_CMDQIRQ_DMA_REALDONE_MSK   (0x0004)
#define HAL_DISP_CMDQIRQ_DMA_ACTIVE_ON_BIT  (0x0002) // BIT1
#define HAL_DISP_CMDQIRQ_DMA_ACTIVE_ON_MSK  (0x0002)
#define HAL_DISP_CMDQIRQ_DMA_ACTIVE_OFF_BIT (0x0001) // BIT0
#define HAL_DISP_CMDQIRQ_DMA_ACTIVE_OFF_MSK (0x0001)

#define HAL_DISP_CMDQIRQ_DEV_CVBS_BIT       (0x0040)
#define HAL_DISP_CMDQIRQ_DEV_CVBS_MSK       (0x0040)
#define HAL_DISP_CMDQIRQ_DEV_LCD_BIT        (0x0080)
#define HAL_DISP_CMDQIRQ_DEV_LCD_MSK        (0x0080)
#define HAL_DISP_CMDQIRQ_DEV_OP2VDE_ON_BIT  (0x0008) // BIT3
#define HAL_DISP_CMDQIRQ_DEV_OP2VDE_ON_MSK  (0x0008)
#define HAL_DISP_CMDQIRQ_DEV_OP2VDE_OFF_BIT (0x0004) // BIT2
#define HAL_DISP_CMDQIRQ_DEV_OP2VDE_OFF_MSK (0x0004)
#define HAL_DISP_CMDQIRQ_DEV_VSYNC_ON_BIT   (0x0002) // BIT1
#define HAL_DISP_CMDQIRQ_DEV_VSYNC_ON_MSK   (0x0002)
#define HAL_DISP_CMDQIRQ_DEV_VSYNC_OFF_BIT  (0x0001) // BIT0
#define HAL_DISP_CMDQIRQ_DEV_VSYNC_OFF_MSK  (0x0001)

#define HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_BIT (0x8000) // BIT15
#define HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_MSK (0x8000)
#define HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_BIT (0x4000) // BIT14
#define HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_MSK (0x4000)
#define HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVEN_BIT (0x0010) // BIT4
#define HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVEN_MSK (0x0010)
#define HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVE_BIT  (0x0020) // BIT5
#define HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVE_MSK  (0x0020)
#define HAL_DISP_IRQ_TIMEZONE_VS_ACTIVE_BIT   (0x0080) // BIT7
#define HAL_DISP_IRQ_TIMEZONE_VS_ACTIVE_MSK   (0x0080)

#define HAL_DISP_IRQ_VGA_AFF_BIT     (0x0001) // BIT0
#define HAL_DISP_IRQ_VGA_AFF_MSK     (0x0001)
#define HAL_DISP_IRQ_VGA_AFE_BIT     (0x0002) // BIT1
#define HAL_DISP_IRQ_VGA_AFE_MSK     (0x0002)
#define HAL_DISP_IRQ_VGA_HPD_ON_BIT  (0x0004) // BIT2
#define HAL_DISP_IRQ_VGA_HPD_ON_MSK  (0x0004)
#define HAL_DISP_IRQ_VGA_HPD_OFF_BIT (0x0008) // BIT3
#define HAL_DISP_IRQ_VGA_HPD_OFF_MSK (0x0008)
//------------------------------------------------------------------------------
// Fpll
#define HAL_DISP_FPLL_0_VGA_HDMI 0
#define HAL_DISP_FPLL_1_CVBS_DAC 1
#define HAL_DISP_FPLL_CNT        2

//------------------------------------------------------------------------------
// Ctx
#define HAL_DISP_CTX_MAX_INST 1

//------------------------------------------------------------------------------
// Device Ctx
#define HAL_DISP_DEVICE_ID_0 0
#define HAL_DISP_DEVICE_ID_1 1
#define HAL_DISP_DEVICE_ID_2 2
#define HAL_DISP_DEVICE_MAX  3

#define HAL_DISP_DEVICE_0_VID_CNT 2
#define HAL_DISP_DEVICE_1_VID_CNT 4
#define HAL_DISP_DEVICE_2_VID_CNT 2

#define HAL_DISP_DEVICE_0_SUPPORT_DMA 1
#define HAL_DISP_DEVICE_1_SUPPORT_DMA 1
#define HAL_DISP_DEVICE_2_SUPPORT_DMA 0

//------------------------------------------------------------------------------
// VideoLayer
#define HAL_DISP_VIDLAYER_ID_0 0
#define HAL_DISP_VIDLAYER_ID_1 1
#define HAL_DISP_VIDLAYER_ID_2 2
#define HAL_DISP_VIDLAYER_ID_3 3
#define HAL_DISP_VIDLAYER_ID_4 4
#define HAL_DISP_VIDLAYER_ID_5 5
#define HAL_DISP_VIDLAYER_MAX  6 // (MOPG & MOPS) * 2 Device
//------------------------------------------------------------------------------
// InputPort
#define HAL_DISP_MOPG_GWIN_NUM 32                                                // MOPG 16 Gwins
#define HAL_DISP_MOPS_GWIN_NUM 1                                                 // MOPS 1 Gwin
#define HAL_DISP_INPUTPORT_NUM (HAL_DISP_MOPG_GWIN_NUM + HAL_DISP_MOPS_GWIN_NUM) // MOP: 32Gwin_MOPG + 1Gwin_MOPS
#define HAL_DISP_INPUTPORT_MAX (3 * HAL_DISP_INPUTPORT_NUM)
//------------------------------------------------------------------------------
// DMA  Ctx
#define HAL_DISP_DMA_ID_0              0
#define HAL_DISP_DMA_MAX               1
#define HAL_DISP_DMA_PIX_FMT_PLANE_MAX 3
//------------------------------------------------------------------------------
// IRQ CTX
#define HAL_DISP_IRQ_ID_DEVICE_0   0
#define HAL_DISP_IRQ_ID_DEVICE_1   2
#define HAL_DISP_IRQ_ID_DEVICE_2   4
#define HAL_DISP_IRQ_ID_TIMEZONE_0 1
#define HAL_DISP_IRQ_ID_TIMEZONE_1 3
#define HAL_DISP_IRQ_ID_TIMEZONE_2 5
#define HAL_DISP_IRQ_ID_VGAHPD     6
#define HAL_DISP_IRQ_ID_CVBS       7
#define HAL_DISP_IRQ_ID_LCD        8
#define HAL_DISP_IRQ_ID_DMA        9
#define HAL_DISP_IRQ_ID_MAX        10

#define HAL_DISP_MAPPING_DEVICEID_FROM_MI(x)  ((x==0) ? HAL_DISP_DEVICE_ID_1 :\
                                            (x==1) ? HAL_DISP_DEVICE_ID_0 :\
                                            (x==2) ? HAL_DISP_DEVICE_ID_2 :HAL_DISP_DEVICE_MAX)

#define HAL_DISP_MAPPING_VIDLAYERID_FROM_MI(x)  ((x==0) ? HAL_DISP_VIDLAYER_ID_2 :\
                                                (x==1) ? HAL_DISP_VIDLAYER_ID_3 :\
                                                (x==2) ? HAL_DISP_VIDLAYER_ID_0 :\
                                                (x==3) ? HAL_DISP_VIDLAYER_ID_1 :\
                                                (x==4) ? HAL_DISP_VIDLAYER_ID_4 :\
                                                (x==5) ? HAL_DISP_VIDLAYER_ID_5 :\
                                                HAL_DISP_VIDLAYER_MAX)

#define HAL_DISP_IRQ_CFG                                                        \
    {                                                                           \
        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, \
    }
//------------------------------------------------------------------------------
// TimeZone
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_LINUX 1
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_UBOOT 0
#define HAL_DISP_TIMEZONE_ISR_SUPPORT_RTK   0

#define E_HAL_DISP_IRQ_TYPE_TIMEZONE (E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)

//------------------------------------------------------------------------------
// Vga HPD Isr
#define HAL_DISP_VGA_HPD_ISR_SUPPORT_LINUX 0
#define HAL_DISP_VGA_HPD_ISR_SUPPORT_UBOOT 0
#define HAL_DISP_VGA_HPD_ISR_SUPPORT_RTK   0

#define E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF (E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON | E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
//------------------------------------------------------------------------------
// DMA Isr

#define E_HAL_DISP_IRQ_TYPE_DMA                                                                                \
    (E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON | E_HAL_DISP_IRQ_TYPE_DMA_REALDONE | E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL \
     | E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL)
#define E_HAL_DISP_IRQ_MSK_DMA                                                                     \
    (HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK | HAL_DISP_IRQ_DMA_REGFAIL_MSK | HAL_DISP_IRQ_DMA_FIFOFULL_MSK \
     | HAL_DISP_IRQ_DMA_REALDONE_MSK)

//------------------------------------------------------------------------------
// CLK
#define CLK_MHZ(x)                 (x * 1000000)
#define HAL_DISP_CLK_MOP_RATE      CLK_MHZ(384)
#define HAL_DISP_CLK_DISP_432_RATE CLK_MHZ(432)
#define HAL_DISP_CLK_DISP_216_RATE CLK_MHZ(216)
//------------------------------------------------------------------------------
// MACE
#define HAL_DISP_HPQ_BANKOFFSET_VAL       (REG_ACE2_2_BASE - REG_ACE2_BASE)
#define HAL_DISP_DEV_MACE_SUPPORT(dev)    ((dev == HAL_DISP_DEVICE_ID_0) ? 1 : 0)
#define HAL_DISP_DEV_HPQ_SUPPORT(dev)     ((dev == HAL_DISP_DEVICE_ID_0) ? 0 : 1)
#define HAL_DISP_DEV_HPQ_NEED_OFFSET(dev) ((dev == HAL_DISP_DEVICE_ID_2) ? 1 : 0)
#define HAL_DISP_REG_IS_OP2(Reg)          (((Reg & 0xFFFF00) == REG_DISP_TOP_OP20_BASE))
#define HAL_DISP_REG_IS_GAMMA(Reg)        (((Reg & 0xFFFF00) == REG_DISP_TOP_GAMMA0_BASE))
#define HAL_DISP_REG_IS_MACE(Reg)         (((Reg & 0xFFFF00) == REG_DISP_TOP_MACE_BASE))
#define HAL_DISP_REG_IS_HPQ(Reg)          (((Reg & 0xFFFF00) >= REG_ACE2_BASE && (Reg & 0xFFFF00) <= REG_VPS_BASE) ? 1 : 0)
#define HAL_DISP_HPQ_BANKOFFSET(dev)      (HAL_DISP_DEV_HPQ_NEED_OFFSET(dev) ? HAL_DISP_HPQ_BANKOFFSET_VAL : 0)
#define HAL_DISP_GAMMA_BANKOFFSET(dev)                                                       \
    ((dev == HAL_DISP_DEVICE_ID_1)   ? (REG_DISP_TOP_GAMMA1_BASE - REG_DISP_TOP_GAMMA0_BASE) \
     : (dev == HAL_DISP_DEVICE_ID_2) ? (REG_DISP_TOP_GAMMA2_BASE - REG_DISP_TOP_GAMMA0_BASE) \
                                     : 0)
#define HAL_DISP_OP2_BANKOFFSET(dev)                                                     \
    ((dev == HAL_DISP_DEVICE_ID_1)   ? (REG_DISP_TOP_OP21_BASE - REG_DISP_TOP_OP20_BASE) \
     : (dev == HAL_DISP_DEVICE_ID_2) ? (REG_DISP_TOP_OP22_BASE - REG_DISP_TOP_OP20_BASE) \
                                     : 0)
#define HAL_DISP_GET_BANKOFFSET(dev, Reg)                        \
    (HAL_DISP_REG_IS_GAMMA(Reg) ? HAL_DISP_GAMMA_BANKOFFSET(dev) \
     : HAL_DISP_REG_IS_OP2(Reg) ? HAL_DISP_OP2_BANKOFFSET(dev)   \
     : HAL_DISP_REG_IS_HPQ(Reg) ? HAL_DISP_HPQ_BANKOFFSET(dev)   \
                                : 0)
#define HAL_DISP_GET_APPLY(dev, Reg)                                   \
    (((HAL_DISP_DEV_HPQ_SUPPORT(dev) && HAL_DISP_REG_IS_MACE(Reg))     \
      || (HAL_DISP_DEV_MACE_SUPPORT(dev) && HAL_DISP_REG_IS_HPQ(Reg))) \
         ? 0                                                           \
         : 1)

#define HAL_DISP_PKSRAM_DELAYCNT   2
#define HAL_DISP_REG_IS_PKSRAM(Reg, msk)          (((Reg) == VPS_REG_VPS_SRAM_ACT && \
    (msk & mask_of_vps_reg_vps_sram_act)) ? 1 : 0)
    
#define HAL_DISP_IS_PKSRAM_REG(u32Dev, Reg, msk)                             \
        ((HAL_DISP_DEV_HPQ_SUPPORT(u32Dev) && \
        (u32Dev < HAL_DISP_DEVICE_MAX) && \
        (HAL_DISP_REG_IS_PKSRAM(Reg, msk))))


#define HAL_DISP_CLK_
typedef enum
{
    E_HAL_DISP_CLK_MOP0,
    E_HAL_DISP_CLK_MOP1,
    E_HAL_DISP_CLK_MOP2,
    E_HAL_DISP_CLK_DISP_432,
    E_HAL_DISP_CLK_DISP_216,
    E_HAL_DISP_CLK_NUM,
} HalDispClkType_e;

#define HAL_DISP_COLOR_CSCM_TO_Y2RM(matrix) \
    ((matrix == E_HAL_DISP_CSC_MATRIX_BYPASS) ? E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS :\
    (matrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC) ? E_DISP_COLOR_YUV_2_RGB_MATRIX_SDTV :\
    (matrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC) ? E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV :\
    (matrix == E_HAL_DISP_CSC_MATRIX_USER) ? E_DISP_COLOR_YUV_2_RGB_MATRIX_USER :\
    E_DISP_COLOR_YUV_2_RGB_MATRIX_MAX)
#define HAL_DISP_COLOR_Y2RM_TO_CSCM(matrix) \
        ((matrix == E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS) ? E_HAL_DISP_CSC_MATRIX_BYPASS:\
        (matrix == E_DISP_COLOR_YUV_2_RGB_MATRIX_SDTV) ? E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC :\
        (matrix == E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV) ? E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC :\
        (matrix == E_DISP_COLOR_YUV_2_RGB_MATRIX_USER) ? E_HAL_DISP_CSC_MATRIX_USER :\
        E_HAL_DISP_CSC_MATRIX_MAX)

#define HAL_DISP_CLK_GET_ON_SETTING(idx)  \
    (idx == E_HAL_DISP_CLK_MOP0       ? 1 \
     : idx == E_HAL_DISP_CLK_MOP1     ? 1 \
     : idx == E_HAL_DISP_CLK_MOP2     ? 1 \
     : idx == E_HAL_DISP_CLK_DISP_432 ? 1 \
                                      : 1)

#define HAL_DISP_CLK_GET_OFF_SETTING(idx) \
    (idx == E_HAL_DISP_CLK_MOP0       ? 0 \
     : idx == E_HAL_DISP_CLK_MOP1     ? 0 \
     : idx == E_HAL_DISP_CLK_MOP2     ? 0 \
     : idx == E_HAL_DISP_CLK_DISP_432 ? 0 \
                                      : 0)

#define HAL_DISP_CLK_GET_RATE_ON_SETTING(idx)                      \
    (idx == E_HAL_DISP_CLK_MOP0       ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_MOP1     ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_MOP2     ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_DISP_432 ? HAL_DISP_CLK_DISP_432_RATE \
                                      : HAL_DISP_CLK_DISP_216_RATE)

#define HAL_DISP_CLK_GET_RATE_OFF_SETTING(idx)                     \
    (idx == E_HAL_DISP_CLK_MOP0       ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_MOP1     ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_MOP2     ? HAL_DISP_CLK_MOP_RATE      \
     : idx == E_HAL_DISP_CLK_DISP_432 ? HAL_DISP_CLK_DISP_432_RATE \
                                      : HAL_DISP_CLK_DISP_216_RATE)

// Mux_ATTR = 1 : clk idx
// Mux_ATTR = 0 : clk rate
#define HAL_DISP_CLK_GET_MUX_ATTR(idx)    \
    (idx == E_HAL_DISP_CLK_MOP0       ? 0 \
     : idx == E_HAL_DISP_CLK_MOP1     ? 0 \
     : idx == E_HAL_DISP_CLK_MOP2     ? 0 \
     : idx == E_HAL_DISP_CLK_DISP_432 ? 0 \
                                      : 0)

#define HAL_DISP_CLK_GET_NAME(idx)                \
    (idx == E_HAL_DISP_CLK_MOP0       ? "mop0"    \
     : idx == E_HAL_DISP_CLK_MOP1     ? "mop1"    \
     : idx == E_HAL_DISP_CLK_MOP2     ? "mop2"    \
     : idx == E_HAL_DISP_CLK_DISP_432 ? "disp432" \
                                      : "disp216")
#define HAL_DISP_CLK_ON_SETTING \
    {                           \
        1, 1, 1, 1, 1,          \
    }

#define HAL_DISP_CLK_OFF_SETTING \
    {                            \
        0, 0, 0, 0, 0,           \
    }

#define HAL_DISP_CLK_RATE_SETTING                                                                        \
    {                                                                                                    \
        HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_DISP_432_RATE, \
            HAL_DISP_CLK_DISP_216_RATE,                                                                  \
    }

#define HAL_DISP_CLK_OFF_RATE_SETTING                                                                    \
    {                                                                                                    \
        HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_MOP_RATE, HAL_DISP_CLK_DISP_432_RATE, \
            HAL_DISP_CLK_DISP_216_RATE,                                                                  \
    }

// Mux_ATTR = 1 : clk idx
// Mux_ATTR = 0 : clk rate
#define HAL_DISP_CLK_MUX_ATTR \
    {                         \
        0, 0, 0, 0, 0,        \
    }

#define HAL_DISP_CLK_NAME                             \
    {                                                 \
        "mop0", "mop1", "mop2", "disp432", "disp216", \
    }

#define IsYUV422PackFormat(enPortFormat)                                                         \
    ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_YUYV || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_YVYU \
      || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_UYVY || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_VYUY))
#define IsRGB565Format(enPortFormat) ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_RGB565))
#define IsYUV444PackFormat(enPortFormat)                                                         \
    ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_ARGB || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_ABGR \
      || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_RGBA || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_BGRA))
#define IsYUV420Format(enPortFormat)                                                                   \
    ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_SP420 || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_SP420NV21 \
      || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_P420 || enPortFormat == E_HAL_DISP_DMA_PIX_FMT_SP420TILE))
#define IsYUV420TileFormat(enPortFormat) ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_SP420TILE))
#define IsYUVPlannarFormat(enPortFormat) \
    ((enPortFormat == E_HAL_DISP_DMA_PIX_FMT_P420) || (enPortFormat == E_HAL_DISP_DMA_PIX_FMT_P422))

#define HAL_DISP_COMPRESS_DMA_WIDTH_ALIGN 64

#define HAL_DISP_SUPPORT_DMA_STRIDE_ALIGN(fmt, comp) \
    ((IsYUV420TileFormat(fmt))   ? 0                 \
     : (IsYUVPlannarFormat(fmt)) ? 32                \
     : (IsYUV422PackFormat(fmt)) ? 8                 \
     : (IsYUV444PackFormat(fmt)) ? 4                 \
     : IsRGB565Format(fmt)       ? 8                 \
                                 : 16)
#define HAL_DISP_IS_YCSWAP(fmt) ((fmt == E_HAL_DISP_DMA_PIX_FMT_UYVY) || (fmt == E_HAL_DISP_DMA_PIX_FMT_VYUY))
#define HAL_DISP_IS_UVSWAP(fmt)                                                   \
    ((fmt == E_HAL_DISP_DMA_PIX_FMT_YVYU) || (fmt == E_HAL_DISP_DMA_PIX_FMT_VYUY) \
     || (fmt == E_HAL_DISP_DMA_PIX_FMT_SP420NV21))

//------------------------------------------------------------------------------
// reg_disp_to_miu_mux
#define HAL_DISP_DMA_MIU_MUX_FRONT_DISP0 0x0000
#define HAL_DISP_DMA_MIU_MUX_BACK_DISP0  0x0001
#define HAL_DISP_DMA_MIU_MUX_FRONT_DISP1 0x0002
#define HAL_DISP_DMA_MIU_MUX_BACK_DISP1  0x0003

#define CMDQ_POLL_EQ_MODE 1

//
#define HAL_DISP_REG_ACCESS_MD_UBOOT E_DISP_UTILITY_REG_ACCESS_CPU
#define HAL_DISP_REG_ACCESS_MD_LINUX E_DISP_UTILITY_REG_ACCESS_CMDQ
#define HAL_DISP_REG_ACCESS_MD_RTK   E_DISP_UTILITY_REG_ACCESS_CPU

#define HAL_DISP_REG_FPLL_LOCK_DUMMY   REG_DISP_MOPG_BK02_3F_L
#define HAL_DISP_REG_FPLL0_LOCKED_FLAG 0x80
#define HAL_DISP_REG_FPLL1_LOCKED_FLAG 0x8000

//-------------------------------------------------------------------------------------------------
//  Maybe Diff in ASIC/FPGA
//-------------------------------------------------------------------------------------------------

#define HAL_DISP_VIDEO_PATH_FPLL_DLY 64
// PLL cnt
#define HAL_DISP_PLL_LPLL    (0)
#define HAL_DISP_PLL_DISPPLL (1)
#define HAL_DISP_PLL_CNT     (2)

//------
// CVBS Timegen
//------
#define HAL_DISP_CVBS_VS_ST(eTimeType)    ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0 : 0)
#define HAL_DISP_CVBS_VS_END(eTimeType)   ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x30 : 0x2c)
#define HAL_DISP_CVBS_VFDE_ST(eTimeType)  ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x31 : 0x2d)
#define HAL_DISP_CVBS_VFDE_END(eTimeType) ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x270 : 0x20C)
#define HAL_DISP_CVBS_VDE_ST(eTimeType)   ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x31 : 0x2d)
#define HAL_DISP_CVBS_VDE_END(eTimeType)  ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x270 : 0x20C)
#define HAL_DISP_CVBS_VTT(eTimeType)      ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x270 : 0x20C)
#define HAL_DISP_CVBS_HS_ST(eTimeType)    ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0 : 0)
#define HAL_DISP_CVBS_HS_END(eTimeType)   ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x8f : 0x3D)
#define HAL_DISP_CVBS_HFDE_ST(eTimeType)  ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x80 : 0x7A)
#define HAL_DISP_CVBS_HFDE_END(eTimeType) ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x355 : 0x349)
#define HAL_DISP_CVBS_HDE_ST(eTimeType)   ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x80 : 0x7A)
#define HAL_DISP_CVBS_HDE_END(eTimeType)  ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x34f : 0x349)
#define HAL_DISP_CVBS_HTT(eTimeType)      ((eTimeType == E_HAL_DISP_OUTPUT_PAL) ? 0x35f : 0x359)
//------
// Patgen
//------
#define HAL_DISP_PATGEN_DEFAULT     (0x7)

//----------
// CMDQ Test
//----------
#define HAL_DISP_CMDQ_TEST_EVENT    \
        {\
            E_MHAL_CMDQEVE_DISP_0_INT,\
            E_MHAL_CMDQEVE_DISP_1_INT,\
            E_MHAL_CMDQEVE_DISP_2_INT,\
            E_MHAL_CMDQEVE_DISP_WDMA_0_INT\
        }

#define HAL_DISP_CMDQ_TEST_REG    \
        {\
            DISP_GP_CTRL_REG_RQ_TYPE_SEL,\
            DISP_TOP0_REG_INTR_MASK_GRP_A,\
            DISP_TOP1_REG_INTR_MASK_GRP_A,\
            DISP_TOP_OP20_REG_HISTOGRAM_THRD_0,\
            DISP_TOP_OP21_REG_HISTOGRAM_THRD_0,\
            DISP_TOP_MACE_REG_DD_CORING_TABLE_0,\
            DISP_TOP_GAMMA0_REG_TBL_R_00,\
            DISP_TOP_GAMMA1_REG_TBL_R_00,\
            TVENC_REG_BG_Y_COLOR,\
            DISP_CVBS_REG_CVBS_IMI_OFFSET_Y_1,\
            DAC_ATOP_REG_DAC_SYN_SET,\
            ACE2_REG_MAIN_IHC_ICC_Y_0,\
            ACE3_REG_MAIN_IHC_ICC_COLOR2_Y_2,\
            ACE_REG_FCC_CR_T1_Y_20,\
            DLC_REG_HISTOGRAM_RANGE6,\
            LCE_REG_MAIN_COLOR_G0_SELECT_LCE,\
            MWE_REG_VIP_STAT_BASE0,\
            VPS_REG_BAND1_UNDERSHOOT_LIMIT,\
            HDMI_LTP_REG_LTP123_DATA_0,\
            HDMI_TX_REG_RESERVED_1E,\
            HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD_0\
        }

#define HAL_DISP_MOP_Y_DMA_THRESHOLD 0x30
#define HAL_DISP_MOP_C_DMA_THRESHOLD 0x30

#define HAL_DISP_1M         1000000
#define HAL_DISP_1K         1000
#define HAL_DISP_100K         100000
#define HAL_DISP_BASIC_CLK_DIV   ((u64)((u64)432* HAL_DISP_1M * 524288))
#define HAL_DISP_HDMI_VCO_MAX   (6000)
#define HAL_DISP_HDMI_VCO_MIN   (2130)
#define HAL_DISP_HDMI_VCO_RECOMMEND   (3200)
#define HAL_DISP_HDMI_PLLPOSTDIVSELCNT 6   // 1 2 4 8 16 32
#define HAL_DISP_GET_HDMIPLLPOSTDIVVAL(u32idx) ((1<<u32idx))
#define HAL_DISP_HDMI_DCLKTOBITRATE 10
#define HAL_DISP_HDMI_PLLLOOPDIV 20
#define HAL_DISP_HDMI_DCLKTOHWDCLKMULTIPLE (HAL_DISP_HDMI_DCLKTOBITRATE/2)

#define HAL_DISP_VGA_VCO_MAX   (5000)
#define HAL_DISP_VGA_VCO_MIN   (1200)
#define HAL_DISP_VGA_VCO_RECOMMEND   (2100)
#define HAL_DISP_VGA_PLLPOSTDIVSELCNT 6     // 2 4 6 8 10 16
#define HAL_DISP_GET_VGAPLLPOSTDIVVAL(u32idx) (((u32idx)==(HAL_DISP_VGA_PLLPOSTDIVSELCNT-1)) ? 16 : ((u32idx+1)<<(1)))
#define HAL_DISP_GET_VGAPLLPOSTDIVIDX(val) (((val)==(16)) ? 0x6 : (val/2))
#define HAL_DISP_VGA_DCLKTOHWDCLKMULTIPLE (9)


#define HAL_DISP_CVBS_DCLKTOHWDCLKMULTIPLE ((2))
#define HAL_DISP_CVBS_PLLLOOPDIV 24
#define HAL_DISP_GET_CVBSPLLPOSTDIVVAL 25
#define HAL_DISP_CVBS_PLLPOSTDIVSELCNT 1
#define HAL_DISP_CVBS_VCO_RECOMMEND 2000
#define HAL_DISP_CVBS_VCO_MAX   (3000)
#define HAL_DISP_CVBS_VCO_MIN   (1000)
#define HAL_DISP_GET_VCO_MAX(u32interface) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_VCO_MAX :\
                                             (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_VGA_VCO_MAX :\
                                            (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_VCO_MAX:0)
#define HAL_DISP_GET_VCO_MIN(u32interface) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_VCO_MIN :\
                                             (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_VGA_VCO_MIN :\
                                             (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_VCO_MIN:0)
                                             
#define HAL_DISP_GET_VCO_RECOMMEND(u32interface) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_VCO_RECOMMEND :\
                                                 (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_VGA_VCO_RECOMMEND :\
                                                 (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_VCO_RECOMMEND: 0)
                                                         
#define HAL_DISP_GET_PLLPOSTDIVSELCNT(u32interface) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_PLLPOSTDIVSELCNT :\
                                                     (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_VGA_PLLPOSTDIVSELCNT :\
                                                     (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_PLLPOSTDIVSELCNT : 0)

#define HAL_DISP_GET_PLLPOSTDIVVAL(u32interface,u32idx) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_GET_HDMIPLLPOSTDIVVAL(u32idx) :\
                                                         (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_GET_VGAPLLPOSTDIVVAL(u32idx) :\
                                                         (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_GET_CVBSPLLPOSTDIVVAL : 0)

#define HAL_DISP_DCLKTOHWDCLKMULTIPLE(u32interface) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_DCLKTOHWDCLKMULTIPLE :\
                                                     (DISP_OUTDEV_IS_VGA(u32interface))   ? HAL_DISP_VGA_DCLKTOHWDCLKMULTIPLE :\
                                                     (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_DCLKTOHWDCLKMULTIPLE :0)

#define HAL_DISP_GET_PLLLOOPDIVVAL(u32interface,u32idx) ((DISP_OUTDEV_IS_HDMI(u32interface)) ? HAL_DISP_HDMI_PLLLOOPDIV :\
                                                         (DISP_OUTDEV_IS_VGA(u32interface))   ? (u32idx*2) :\
                                                         (DISP_OUTDEV_IS_CVBS(u32interface))   ? HAL_DISP_CVBS_PLLLOOPDIV :0)

#define HAL_HDMITX_TOP_TBL_BYSELF
#define HAL_DISP_VGA_TIMING_TBL_BYSELF

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif
