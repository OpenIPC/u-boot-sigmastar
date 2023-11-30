/*
* hal_disp_vga_timing_tbl.c- Sigmastar
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

//****************************************************
// Drive Chip           : I2
// Excel CodeGen Version: 1.05
// Excel SW      Version: 1.01
//****************************************************

#ifndef __HAL_DISP_VGA_TIMING_TBL_C__
#define __HAL_DISP_VGA_TIMING_TBL_C__

#include "hal_disp_vga_timing_tbl.h"
#include "reg_disp_top.h"
#include "drv_disp_os.h"
//****************************************************
// IDAC_ATOP
//****************************************************
u16 g_stidacatopcommontbl[IDAC_ATOP_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] = {
    {
        DRV_REG(REG_DAC_ATOP_15_L), /* DAC_ATOP_REG_EN_IDAC_B, DAC_ATOP_REG_EN_IDAC_G, DAC_ATOP_REG_EN_IDAC_R */
        0x07,
        0x07,
    },
    {
        DRV_REG(REG_DAC_ATOP_16_L), /* DAC_ATOP_REG_EN_IDAC_REF */
        0x01,
        0x01,
    },
    {
        DRV_REG(REG_DAC_ATOP_1F_L), /* DAC_ATOP_REG_PD_IDAC_LDO */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_1D_L), /* DAC_ATOP_REG_GCR_IHALF_RGB, DAC_ATOP_REG_GCR_IQUART_HD,
                                       DAC_ATOP_REG_GCR_IHALF_CVBS */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_29_L), /* DAC_ATOP_REG_GCR_MODE_RGB */
        0x01,
        0x03,
    },
    {
        DRV_REG(REG_DAC_ATOP_00_L), /* DAC_ATOP_REG_CKG_DAC_ATOP, DAC_ATOP_REG_CKG_CVBS_ATOP */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_33_L), /* DAC_ATOP_REG_EMMCPLL_INPUT_DIV_FIRST */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_34_L), /* DAC_ATOP_REG_EMMCPLL_LOOP_DIV_FIRST */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_35_L), /* DAC_ATOP_REG_EMMCPLL_LOOP_DIV_SECOND */
        0xFF,
        0x04,
    },
    {
        DRV_REG(REG_DAC_ATOP_36_L), /* DAC_ATOP_REG_EMMCPLL_PD */
        0xFF,
        0x00,
    },
};

#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
u16 g_stidacatoptbl[IDAC_ATOP_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_VGA_ID_NUM] = 
{
    { DRV_REG(REG_DAC_ATOP_37_L)/* DAC_ATOP_REG_EMMCPLL_PDIV */,
   0xFF,           0x04,/*720_24P*/
           0x04,/*720_25P*/
           0x04,/*720_29D97P*/
           0x04,/*720_30P*/
           0x02,/*720_50P*/
           0x02,/*720_59D94P*/
           0x02,/*720_60P*/
           0x02,/*1080_23D98P*/
           0x02,/*1080_24P*/
           0x02,/*1080_25P*/
           0x02,/*1080_29D97P*/
           0x02,/*1080_30P*/
           0x00,/*1080_50P*/
           0x00,/*1080_59D94P*/
           0x00,/*1080_60P*/
           0x00,/*2560x1440_30P*/
           0x00,/*2560x1440_50P*/
           0x00,/*2560x1440_60P*/
           0x00,/*3840x2160_24P*/
           0x00,/*3840x2160_25P*/
           0x00,/*3840x2160_29D97P*/
           0x00,/*3840x2160_30P*/
           0x00,/*3840x2160_60P*/
           0x04,/*640x480_60P*/
           0x04,/*480_60P*/
           0x04,/*576_50P*/
           0x04,/*800x600_60P*/
           0x04,/*848x480_60P*/
           0x02,/*1024x768_60P*/
           0x02,/*1280x768_60P*/
           0x02,/*1280x800_60P*/
           0x02,/*1280x960_60P*/
           0x00,/*1280x1024_60P*/
           0x02,/*1360x768_60P*/
           0x02,/*1366x768_60P*/
           0x02,/*1400x1050_60P*/
           0x02,/*1440x900_60P*/
           0x00,/*1600x900_60P*/
           0x00,/*1600x1200_60P*/
           0x00,/*1680x1050_60P*/
           0x00,/*1920x1200_60P*/
           0x00,/*1920x1440_60P*/
           0x00,/*2560x1600_60P*/
    }, 
};
#endif 

//****************************************************
// SYNTH_SET_HDMITX
//****************************************************
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
u16 g_stsynthsethdmitxtbl[SYNTH_SET_HDMITX_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_VGA_ID_NUM] = 
{
    { DRV_REG(REG_DISP_TOP0_10_L)/* DISP_TOP0_REG_SYN_SET_0 */,
   0xffff,           0x8e86,/*720_24P*/
           0x8ba2,/*720_25P*/
           0x2f20,/*720_29D97P*/
           0x8ba2,/*720_30P*/
           0x8ba2,/*720_50P*/
           0x9790,/*720_59D94P*/
           0x8ba2,/*720_60P*/
           0x9593,/*1080_23D98P*/
           0x8ba2,/*1080_24P*/
           0x8ba2,/*1080_25P*/
           0x9790,/*1080_29D97P*/
           0x8ba2,/*1080_30P*/
           0x8ba2,/*1080_50P*/
           0x9790,/*1080_59D94P*/
           0x8ba2,/*1080_60P*/
           0x8ba2,/*2560x1440_30P*/
           0x8ba2,/*2560x1440_50P*/
           0x8ba2,/*2560x1440_60P*/
           0x8ba2,/*3840x2160_24P*/
           0x8ba2,/*3840x2160_25P*/
           0x9790,/*3840x2160_29D97P*/
           0x8ba2,/*3840x2160_30P*/
           0x45d1,/*3840x2160_60P*/
           0x9249,/*640x480_60P*/
           0xedd0,/*480_60P*/
           0xedd0,/*576_50P*/
           0x6d8b,/*800x600_60P*/
           0x334b,/*848x480_60P*/
           0x95a9,/*1024x768_60P*/
           0x60af,/*1280x768_60P*/
           0x1e74,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x1e74,/*1280x1024_60P*/
           0x6e67,/*1360x768_60P*/
           0x479b,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cb,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x890e,/*1600x1200_60P*/
           0x9cdd,/*1680x1050_60P*/
           0xb2c6,/*1920x1200_60P*/
           0x89d9,/*1920x1440_60P*/
           0xa884,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DISP_TOP0_11_L)/* DISP_TOP0_REG_SYN_SET_1 */,
   0xFF,           0x2e,/*720_24P*/
           0x2e,/*720_25P*/
           0x5d,/*720_29D97P*/
           0x2e,/*720_30P*/
           0x2e,/*720_50P*/
           0x2e,/*720_59D94P*/
           0x2e,/*720_60P*/
           0x2E,/*1080_23D98P*/
           0x2e,/*1080_24P*/
           0x2e,/*1080_25P*/
           0x2e,/*1080_29D97P*/
           0x2e,/*1080_30P*/
           0x2e,/*1080_50P*/
           0x2e,/*1080_59D94P*/
           0x2e,/*1080_60P*/
           0x2e,/*2560x1440_30P*/
           0x2e,/*2560x1440_50P*/
           0x2e,/*2560x1440_60P*/
           0x2e,/*3840x2160_24P*/
           0x2e,/*3840x2160_25P*/
           0x2e,/*3840x2160_29D97P*/
           0x2e,/*3840x2160_30P*/
           0x17,/*3840x2160_60P*/
           0x44,/*640x480_60P*/
           0x3f,/*480_60P*/
           0x3f,/*576_50P*/
           0x2B,/*800x600_60P*/
           0x33,/*848x480_60P*/
           0x1A,/*1024x768_60P*/
           0x2B,/*1280x768_60P*/
           0x20,/*1280x800_60P*/
           0x20,/*1280x960_60P*/
           0x20,/*1280x1024_60P*/
           0x28,/*1280x1024_60P*/
           0x28,/*1360x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x40,/*1600x900_60P*/
           0x2a,/*1600x1200_60P*/
           0x17,/*1680x1050_60P*/
           0x23,/*1920x1200_60P*/
           0x1D,/*1920x1440_60P*/
           0x27,/*2560x1600_60P*/
    }, 
};
#endif 

//****************************************************
// SYNTH_SET_VGA
//****************************************************
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
u16 g_stsynthsetvgatbl[SYNTH_SET_VGA_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_VGA_ID_NUM] = 
{
    { DRV_REG(REG_DAC_ATOP_0E_L)/* DAC_ATOP_REG_DAC_SYN_SET_0 */,
   0xffff,           0x8b86,/*720_24P*/
           0x8ba2,/*720_25P*/
           0x978f,/*720_29D97P*/
           0x8ba2,/*720_30P*/
           0x8ba2,/*720_50P*/
           0x978f,/*720_59D94P*/
           0x8ba2,/*720_60P*/
           0x9593,/*1080_23D98P*/
           0x8ba2,/*1080_24P*/
           0x8ba2,/*1080_25P*/
           0x978f,/*1080_29D97P*/
           0x8ba2,/*1080_30P*/
           0x8ba2,/*1080_50P*/
           0x978f,/*1080_59D94P*/
           0x8ba2,/*1080_60P*/
           0x8ba2,/*2560x1440_30P*/
           0x8ba2,/*2560x1440_50P*/
           0x8ba2,/*2560x1440_60P*/
           0x8ba2,/*3840x2160_24P*/
           0x8ba2,/*3840x2160_25P*/
           0x9790,/*3840x2160_29D97P*/
           0x8ba2,/*3840x2160_30P*/
           0x45d1,/*3840x2160_60P*/
           0x9249,/*640x480_60P*/
           0xefa1,/*480_60P*/
           0x0,/*576_50P*/
           0x6d8b,/*800x600_60P*/
           0x334b,/*848x480_60P*/
           0x2b52,/*1024x768_60P*/
           0x60af,/*1280x768_60P*/
           0x4225,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x563,/*1280x1024_60P*/
           0x6e67,/*1360x768_60P*/
           0x477e,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cb,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x890e,/*1600x1200_60P*/
           0x39ba,/*1680x1050_60P*/
           0xb2e3,/*1920x1200_60P*/
           0x89d9,/*1920x1440_60P*/
           0xa884,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DAC_ATOP_0F_L)/* DAC_ATOP_REG_DAC_SYN_SET_1 */,
   0xFF,           0x2E,/*720_24P*/
           0x2E,/*720_25P*/
           0x2E,/*720_29D97P*/
           0x2E,/*720_30P*/
           0x2E,/*720_50P*/
           0x2E,/*720_59D94P*/
           0x2E,/*720_60P*/
           0x2E,/*1080_23D98P*/
           0x2E,/*1080_24P*/
           0x2E,/*1080_25P*/
           0x2E,/*1080_29D97P*/
           0x2E,/*1080_30P*/
           0x2E,/*1080_50P*/
           0x2E,/*1080_59D94P*/
           0x2E,/*1080_60P*/
           0x2E,/*2560x1440_30P*/
           0x2E,/*2560x1440_50P*/
           0x2E,/*2560x1440_60P*/
           0x2E,/*3840x2160_24P*/
           0x2E,/*3840x2160_25P*/
           0x2e,/*3840x2160_29D97P*/
           0x2E,/*3840x2160_30P*/
           0x17,/*3840x2160_60P*/
           0x44,/*640x480_60P*/
           0x3f,/*480_60P*/
           0x40,/*576_50P*/
           0x2B,/*800x600_60P*/
           0x33,/*848x480_60P*/
           0x35,/*1024x768_60P*/
           0x2B,/*1280x768_60P*/
           0x29,/*1280x800_60P*/
           0x20,/*1280x960_60P*/
           0x40,/*1280x1024_60P*/
           0x28,/*1360x768_60P*/
           0x28,/*1366x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x40,/*1600x900_60P*/
           0x2A,/*1600x1200_60P*/
           0x2F,/*1680x1050_60P*/
           0x23,/*1920x1200_60P*/
           0x1D,/*1920x1440_60P*/
           0x27,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DISP_TOP0_12_L)/* DISP_TOP0_REG_SYN_SET_SELF_0 */,
   0xffff,           0x8b86,/*720_24P*/
           0x8ba2,/*720_25P*/
           0x978f,/*720_29D97P*/
           0x8ba2,/*720_30P*/
           0x8ba2,/*720_50P*/
           0x978f,/*720_59D94P*/
           0x8ba2,/*720_60P*/
           0x9593,/*1080_23D98P*/
           0x8ba2,/*1080_24P*/
           0x8ba2,/*1080_25P*/
           0x978f,/*1080_29D97P*/
           0x8ba2,/*1080_30P*/
           0x8ba2,/*1080_50P*/
           0x978f,/*1080_59D94P*/
           0x8ba2,/*1080_60P*/
           0x8ba2,/*2560x1440_30P*/
           0x8ba2,/*2560x1440_50P*/
           0x8ba2,/*2560x1440_60P*/
           0x8ba2,/*3840x2160_24P*/
           0x8ba2,/*3840x2160_25P*/
           0x9790,/*3840x2160_29D97P*/
           0x8ba2,/*3840x2160_30P*/
           0x45d1,/*3840x2160_60P*/
           0x9249,/*640x480_60P*/
           0xefa1,/*480_60P*/
           0x0,/*576_50P*/
           0x6d8b,/*800x600_60P*/
           0x334b,/*848x480_60P*/
           0x2b52,/*1024x768_60P*/
           0x60af,/*1280x768_60P*/
           0x4225,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x563,/*1280x1024_60P*/
           0x6e67,/*1360x768_60P*/
           0x477e,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cb,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x890e,/*1600x1200_60P*/
           0x39ba,/*1680x1050_60P*/
           0xb2e3,/*1920x1200_60P*/
           0x89d9,/*1920x1440_60P*/
           0xa884,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DISP_TOP0_13_L)/* DISP_TOP0_REG_SYN_SET_SELF_1 */,
   0xFF,           0x2E,/*720_24P*/
           0x2E,/*720_25P*/
           0x2E,/*720_29D97P*/
           0x2E,/*720_30P*/
           0x2E,/*720_50P*/
           0x2E,/*720_59D94P*/
           0x2E,/*720_60P*/
           0x2E,/*1080_23D98P*/
           0x2E,/*1080_24P*/
           0x2E,/*1080_25P*/
           0x2E,/*1080_29D97P*/
           0x2E,/*1080_30P*/
           0x2E,/*1080_50P*/
           0x2E,/*1080_59D94P*/
           0x2E,/*1080_60P*/
           0x2E,/*2560x1440_30P*/
           0x2E,/*2560x1440_50P*/
           0x2E,/*2560x1440_60P*/
           0x2E,/*3840x2160_24P*/
           0x2E,/*3840x2160_25P*/
           0x2e,/*3840x2160_29D97P*/
           0x2E,/*3840x2160_30P*/
           0x17,/*3840x2160_60P*/
           0x44,/*640x480_60P*/
           0x3f,/*480_60P*/
           0x40,/*576_50P*/
           0x2B,/*800x600_60P*/
           0x33,/*848x480_60P*/
           0x35,/*1024x768_60P*/
           0x2B,/*1280x768_60P*/
           0x29,/*1280x800_60P*/
           0x20,/*1280x960_60P*/
           0x40,/*1280x1024_60P*/
           0x28,/*1360x768_60P*/
           0x28,/*1366x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x40,/*1600x900_60P*/
           0x2A,/*1600x1200_60P*/
           0x2F,/*1680x1050_60P*/
           0x23,/*1920x1200_60P*/
           0x1D,/*1920x1440_60P*/
           0x27,/*2560x1600_60P*/
    }, 
};
#endif 

//****************************************************
HAL_DISP_VGA_INFO g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_TAB_NUM] = {
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
    {*g_stidacatoptbl, IDAC_ATOP_REG_NUM, E_HAL_DISP_VGA_IP_NORMAL},
#endif
    {*g_stidacatopcommontbl, IDAC_ATOP_COMMON_REG_NUM, E_HAL_DISP_VGA_IP_COMMON},
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
    {*g_stsynthsethdmitxtbl, SYNTH_SET_HDMITX_REG_NUM, E_HAL_DISP_VGA_IP_NORMAL},
#endif
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
    {*g_stsynthsetvgatbl, SYNTH_SET_VGA_REG_NUM, E_HAL_DISP_VGA_IP_NORMAL},
#endif
};

#endif
