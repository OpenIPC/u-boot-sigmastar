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
    
    { DRV_REG(REG_DAC_ATOP_15_L),/* DAC_ATOP_REG_EN_IDAC_B, DAC_ATOP_REG_EN_IDAC_G, DAC_ATOP_REG_EN_IDAC_R */
   0x07, 0x07,
    },
    { DRV_REG(REG_DAC_ATOP_16_L),/* DAC_ATOP_REG_EN_IDAC_REF */
   0x01, 0x01,
    },
    { DRV_REG(REG_DAC_ATOP_1F_L),/* DAC_ATOP_REG_PD_IDAC_LDO */
   0xFF, 0x00,
    },
    { DRV_REG(REG_DAC_ATOP_1D_L),/* DAC_ATOP_REG_GCR_IHALF_RGB, DAC_ATOP_REG_GCR_IQUART_HD, DAC_ATOP_REG_GCR_IHALF_CVBS */
   0xFF, 0x01,
    },
    { DRV_REG(REG_DAC_ATOP_29_L),/* DAC_ATOP_REG_GCR_MODE_RGB */
   0x01, 0x01,
    },
    { DRV_REG(REG_DAC_ATOP_00_L),/* DAC_ATOP_REG_CKG_DAC_ATOP, DAC_ATOP_REG_CKG_CVBS_ATOP */
   0xFF, 0x00,
    },
    { DRV_REG(REG_DAC_ATOP_3D_L),/* DAC_ATOP_REG_SDIOPLL_TEST_SEL, DAC_ATOP_REG_SDIOPLL_IBIAS, DAC_ATOP_REG_SDIOPLL_TESTCLK_SEL, DAC_ATOP_REG_SDIOPLL_EN_D2S */
   0xffff, 0xf30,
    },
    { DRV_REG(REG_DAC_ATOP_3B_L),/* DAC_ATOP_REG_SDIOPLL_DLL_SEL, DAC_ATOP_REG_SDIOPLL_EN_TEST_CLOCK, DAC_ATOP_REG_SDIOPLL_EN_VCO, DAC_ATOP_REG_SDIOPLL_LO_FREQ_EN, DAC_ATOP_REG_SDIOPLL_INPUT_DIV, DAC_ATOP_REG_SDIOPLL_INPUT_SEL */
   0xFF, 0x06,
    },
    { DRV_REG(REG_DAC_ATOP_38_L),/* DAC_ATOP_REG_SDIOPLL_TEST_0 */
   0xffff, 0x63,
    },
    { DRV_REG(REG_DAC_ATOP_39_L),/* DAC_ATOP_REG_SDIOPLL_TEST_1 */
   0xffff, 0x180,
    },
    { DRV_REG(REG_DAC_ATOP_32_L),/* DAC_ATOP_REG_SDIOPLL_ICTRL */
   0xFF, 0x02,
    },
    { DRV_REG(REG_DAC_ATOP_36_L),/* DAC_ATOP_REG_SDIOPLL_PD */
   0xFF, 0x00,
    },
};

#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
u16 g_stidacatoptbl[IDAC_ATOP_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_VGA_ID_NUM] = 
{
    { DRV_REG(REG_DAC_ATOP_3E_L)/* DAC_ATOP_REG_SDIOPLL_LOOP_DIV */,
   0xFF,           0x09,/*720_24P*/
           0x09,/*720_25P*/
           0x09,/*720_29D97P*/
           0x09,/*720_30P*/
           0x09,/*720_50P*/
           0x09,/*720_59D94P*/
           0x09,/*720_60P*/
           0x09,/*1080_23D98P*/
           0x09,/*1080_24P*/
           0x09,/*1080_25P*/
           0x09,/*1080_29D97P*/
           0x09,/*1080_30P*/
           0x12,/*1080_50P*/
           0x12,/*1080_59D94P*/
           0x12,/*1080_60P*/
           0x12,/*2560x1440_30P*/
           0x24,/*2560x1440_50P*/
           0x24,/*2560x1440_60P*/
           0x24,/*3840x2160_24P*/
           0x24,/*3840x2160_25P*/
           0x24,/*3840x2160_29D97P*/
           0x24,/*3840x2160_30P*/
           0x24,/*3840x2160_60P*/
           0x12,/*640x480_60P*/
           0x12,/*480_60P*/
           0x12,/*576_50P*/
           0x12,/*800x600_60P*/
           0x12,/*848x480_60P*/
           0x12,/*1024x768_60P*/
           0x09,/*1280x768_60P*/
           0x09,/*1280x800_60P*/
           0x09,/*1280x960_60P*/
           0x09,/*1280x1024_60P*/
           0x09,/*1360x768_60P*/
           0x09,/*1366x768_60P*/
           0x09,/*1400x1050_60P*/
           0x09,/*1440x900_60P*/
           0x12,/*1600x900_60P*/
           0x12,/*1600x1200_60P*/
           0x09,/*1680x1050_60P*/
           0x12,/*1920x1200_60P*/
           0x12,/*1920x1440_60P*/
           0x24,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DAC_ATOP_37_L)/* DAC_ATOP_REG_SDIOPLL_PDIV */,
   0xFF,           0x02,/*720_24P*/
           0x02,/*720_25P*/
           0x02,/*720_29D97P*/
           0x02,/*720_30P*/
           0x00,/*720_50P*/
           0x00,/*720_59D94P*/
           0x00,/*720_60P*/
           0x00,/*1080_23D98P*/
           0x00,/*1080_24P*/
           0x00,/*1080_25P*/
           0x00,/*1080_29D97P*/
           0x00,/*1080_30P*/
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
           0x00,/*1280x768_60P*/
           0x00,/*1280x800_60P*/
           0x00,/*1280x960_60P*/
           0x00,/*1280x1024_60P*/
           0x00,/*1360x768_60P*/
           0x00,/*1366x768_60P*/
           0x00,/*1400x1050_60P*/
           0x00,/*1440x900_60P*/
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
   0xffff,           0x4743,/*720_24P*/
           0x45d1,/*720_25P*/
           0x4bc3,/*720_29D97P*/
           0x45d1,/*720_30P*/
           0x45d1,/*720_50P*/
           0x4bc3,/*720_59D94P*/
           0x45d1,/*720_60P*/
           0x4ac9,/*1080_23D98P*/
           0x45d1,/*1080_24P*/
           0x45d1,/*1080_25P*/
           0x4bc3,/*1080_29D97P*/
           0x45d1,/*1080_30P*/
           0x45d1,/*1080_50P*/
           0x4bc3,/*1080_59D94P*/
           0x45d1,/*1080_60P*/
           0x45d1,/*2560x1440_30P*/
           0x45d1,/*2560x1440_50P*/
           0x45d1,/*2560x1440_60P*/
           0x45d1,/*3840x2160_24P*/
           0x45d1,/*3840x2160_25P*/
           0x4bc8,/*3840x2160_29D97P*/
           0x45d1,/*3840x2160_30P*/
           0xa2e8,/*3840x2160_60P*/
           0x4924,/*640x480_60P*/
           0x0,/*480_60P*/
           0x0,/*576_50P*/
           0xb6c5,/*800x600_60P*/
           0x99a5,/*848x480_60P*/
           0x95a9,/*1024x768_60P*/
           0xb057,/*1280x768_60P*/
           0xa160,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x0,/*1280x1024_60P*/
           0x3733,/*1360x768_60P*/
           0x23cd,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cf,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x4487,/*1600x1200_60P*/
           0x9cee,/*1680x1050_60P*/
           0xd963,/*1920x1200_60P*/
           0x89d9,/*1920x1440_60P*/
           0xd442,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DISP_TOP0_11_L)/* DISP_TOP0_REG_SYN_SET_1 */,
   0xFF,           0x17,/*720_24P*/
           0x17,/*720_25P*/
           0x17,/*720_29D97P*/
           0x17,/*720_30P*/
           0x17,/*720_50P*/
           0x17,/*720_59D94P*/
           0x17,/*720_60P*/
           0x17,/*1080_23D98P*/
           0x17,/*1080_24P*/
           0x17,/*1080_25P*/
           0x17,/*1080_29D97P*/
           0x17,/*1080_30P*/
           0x17,/*1080_50P*/
           0x17,/*1080_59D94P*/
           0x17,/*1080_60P*/
           0x17,/*2560x1440_30P*/
           0x17,/*2560x1440_50P*/
           0x17,/*2560x1440_60P*/
           0x17,/*3840x2160_24P*/
           0x17,/*3840x2160_25P*/
           0x17,/*3840x2160_29D97P*/
           0x17,/*3840x2160_30P*/
           0x0B,/*3840x2160_60P*/
           0x22,/*640x480_60P*/
           0x20,/*480_60P*/
           0x20,/*576_50P*/
           0x15,/*800x600_60P*/
           0x19,/*848x480_60P*/
           0x1A,/*1024x768_60P*/
           0x15,/*1280x768_60P*/
           0x14,/*1280x800_60P*/
           0x10,/*1280x960_60P*/
           0x20,/*1280x1024_60P*/
           0x14,/*1360x768_60P*/
           0x14,/*1366x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x20,/*1600x900_60P*/
           0x15,/*1600x1200_60P*/
           0x17,/*1680x1050_60P*/
           0x11,/*1920x1200_60P*/
           0x1D,/*1920x1440_60P*/
           0x13,/*2560x1600_60P*/
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
   0xffff,           0x8e86,/*720_24P*/
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
           0x0,/*480_60P*/
           0x0,/*576_50P*/
           0x6d8b,/*800x600_60P*/
           0x334b,/*848x480_60P*/
           0x2b52,/*1024x768_60P*/
           0x60af,/*1280x768_60P*/
           0x4225,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x0,/*1280x1024_60P*/
           0x6e67,/*1360x768_60P*/
           0x477e,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cf,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x890e,/*1600x1200_60P*/
           0x9cee,/*1680x1050_60P*/
           0xb2c6,/*1920x1200_60P*/
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
           0x2E,/*3840x2160_29D97P*/
           0x2E,/*3840x2160_30P*/
           0x17,/*3840x2160_60P*/
           0x44,/*640x480_60P*/
           0x40,/*480_60P*/
           0x40,/*576_50P*/
           0x2B,/*800x600_60P*/
           0x33,/*848x480_60P*/
           0x35,/*1024x768_60P*/
           0x2B,/*1280x768_60P*/
           0x29,/*1280x800_60P*/
           0x20,/*1280x960_60P*/
           0x20,/*1280x1024_60P*/
           0x28,/*1360x768_60P*/
           0x28,/*1366x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x40,/*1600x900_60P*/
           0x2A,/*1600x1200_60P*/
           0x17,/*1680x1050_60P*/
           0x23,/*1920x1200_60P*/
           0x1D,/*1920x1440_60P*/
           0x27,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_DISP_TOP0_12_L)/* DISP_TOP0_REG_SYN_SET_SELF_0 */,
   0xffff,           0x8e86,/*720_24P*/
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
           0x0,/*480_60P*/
           0x0,/*576_50P*/
           0x6d8b,/*800x600_60P*/
           0x334b,/*848x480_60P*/
           0x2b52,/*1024x768_60P*/
           0x60af,/*1280x768_60P*/
           0x4225,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x0,/*1280x1024_60P*/
           0x6e67,/*1360x768_60P*/
           0x477e,/*1366x768_60P*/
           0x6036,/*1400x1050_60P*/
           0x63cf,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x890e,/*1600x1200_60P*/
           0x9cee,/*1680x1050_60P*/
           0xb2c6,/*1920x1200_60P*/
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
           0x2E,/*3840x2160_29D97P*/
           0x2E,/*3840x2160_30P*/
           0x17,/*3840x2160_60P*/
           0x44,/*640x480_60P*/
           0x40,/*480_60P*/
           0x40,/*576_50P*/
           0x2B,/*800x600_60P*/
           0x33,/*848x480_60P*/
           0x35,/*1024x768_60P*/
           0x2B,/*1280x768_60P*/
           0x29,/*1280x800_60P*/
           0x20,/*1280x960_60P*/
           0x20,/*1280x1024_60P*/
           0x28,/*1360x768_60P*/
           0x28,/*1366x768_60P*/
           0x1C,/*1400x1050_60P*/
           0x20,/*1440x900_60P*/
           0x40,/*1600x900_60P*/
           0x2A,/*1600x1200_60P*/
           0x17,/*1680x1050_60P*/
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
