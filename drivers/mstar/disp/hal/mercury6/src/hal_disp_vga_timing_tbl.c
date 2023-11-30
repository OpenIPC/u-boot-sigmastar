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
// Excel update date    : 2020/9/22 18:58
//****************************************************

#ifndef _HAL_DISP_VGA_TIMING_TBL_C_
#define _HAL_DISP_VGA_TIMING_TBL_C_

#include "hal_disp_vga_timing_tbl.h" 

//****************************************************
// IDAC_ATOP
//****************************************************
MS_U8 ST_IDAC_ATOP_COMMON_TBL[IDAC_ATOP_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] =
{
 { DRV_REG(REG_DAC_ATOP_15_L), 0x07, 0x07,
 },
 { DRV_REG(REG_DAC_ATOP_16_L), 0x01, 0x01,
 },
 { DRV_REG(REG_DAC_ATOP_1F_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_1D_L), 0xFF, 0x01,
 },
 { DRV_REG(REG_DAC_ATOP_29_L), 0x01, 0x01,
 },
 { DRV_REG(REG_DAC_ATOP_36_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_00_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_34_L), 0xFF, 0x02,
 },
 { DRV_REG(REG_DAC_ATOP_35_L), 0xFF, 0x00,
 },
};

MS_U8 ST_IDAC_ATOP_TBL[IDAC_ATOP_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + HAL_DISP_VGA_ID_NUM]=
{
 { DRV_REG(REG_DAC_ATOP_33_L), 0xFF, 0x00, /*480_60P*/
                    0x00, /*576_50P*/
                    0x00, /*720_50P*/
                    0x00, /*720_60P*/
                    0x00, /*1080_24P*/
                    0x00, /*1080_25P*/
                    0x00, /*1080_30P*/
                    0x00, /*1080_50P*/
                    0x00, /*1080_60P*/
                    0x00, /*2560x1440_30P*/
                    0x00, /*3840x2160_30P*/
                    0x00, /*1024x768_60P*/
                    0x00, /*1280x800_60P*/
                    0x01, /*1280x1024_60P*/
                    0x00, /*1366x768_60P*/
                    0x00, /*1440x900_60P*/
                    0x00, /*1680x1050_60P*/
                    0x00, /*1600x1200_60P*/
 },
 { DRV_REG(REG_DAC_ATOP_37_L), 0xFF, 0x04, /*480_60P*/
                    0x04, /*576_50P*/
                    0x02, /*720_50P*/
                    0x02, /*720_60P*/
                    0x02, /*1080_24P*/
                    0x02, /*1080_25P*/
                    0x02, /*1080_30P*/
                    0x01, /*1080_50P*/
                    0x01, /*1080_60P*/
                    0x01, /*2560x1440_30P*/
                    0x00, /*3840x2160_30P*/
                    0x02, /*1024x768_60P*/
                    0x02, /*1280x800_60P*/
                    0x01, /*1280x1024_60P*/
                    0x02, /*1366x768_60P*/
                    0x02, /*1440x900_60P*/
                    0x02, /*1680x1050_60P*/
                    0x01, /*1600x1200_60P*/
 },
};

//****************************************************
// SYNTH_SET_HDMITX
//****************************************************
MS_U8 ST_SYNTH_SET_HDMITX_TBL[SYNTH_SET_HDMITX_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + HAL_DISP_VGA_ID_NUM]=
{
 { DRV_REG(REG_DISP_TOP0_10_L), 0xFF, 0xA1, /*480_60P*/
                    0x00, /*576_50P*/
                    0xA2, /*720_50P*/
                    0xA2, /*720_60P*/
                    0xA2, /*1080_24P*/
                    0xA2, /*1080_25P*/
                    0xA2, /*1080_30P*/
                    0xA2, /*1080_50P*/
                    0xA2, /*1080_60P*/
                    0xA2, /*2560x1440_30P*/
                    0xA2, /*3840x2160_30P*/
                    0x31, /*1024x768_60P*/
                    0x25, /*1280x800_60P*/
                    0xB1, /*1280x1024_60P*/
                    0x7E, /*1366x768_60P*/
                    0xCB, /*1440x900_60P*/
                    0xDD, /*1680x1050_60P*/
                    0xAA, /*1600x1200_60P*/
 },
 { DRV_REG(REG_DISP_TOP0_10_H), 0xFF, 0xEF, /*480_60P*/
                    0x00, /*576_50P*/
                    0x8B, /*720_50P*/
                    0x8B, /*720_60P*/
                    0x8B, /*1080_24P*/
                    0x8B, /*1080_25P*/
                    0x8B, /*1080_30P*/
                    0x8B, /*1080_50P*/
                    0x8B, /*1080_60P*/
                    0x8B, /*2560x1440_30P*/
                    0x8B, /*3840x2160_30P*/
                    0x2C, /*1024x768_60P*/
                    0x42, /*1280x800_60P*/
                    0x02, /*1280x1024_60P*/
                    0x47, /*1366x768_60P*/
                    0x63, /*1440x900_60P*/
                    0x9C, /*1680x1050_60P*/
                    0xAA, /*1600x1200_60P*/
 },
 { DRV_REG(REG_DISP_TOP0_11_L), 0xFF, 0x3F, /*480_60P*/
                    0x40, /*576_50P*/
                    0x2E, /*720_50P*/
                    0x2E, /*720_60P*/
                    0x2E, /*1080_24P*/
                    0x2E, /*1080_25P*/
                    0x2E, /*1080_30P*/
                    0x2E, /*1080_50P*/
                    0x2E, /*1080_60P*/
                    0x2E, /*2560x1440_30P*/
                    0x2E, /*3840x2160_30P*/
                    0x35, /*1024x768_60P*/
                    0x29, /*1280x800_60P*/
                    0x20, /*1280x1024_60P*/
                    0x28, /*1366x768_60P*/
                    0x20, /*1440x900_60P*/
                    0x17, /*1680x1050_60P*/
                    0x2A, /*1600x1200_60P*/
 },
};

//****************************************************
// SYNTH_SET_VGA
//****************************************************
MS_U8 ST_SYNTH_SET_VGA_TBL[SYNTH_SET_VGA_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + HAL_DISP_VGA_ID_NUM]=
{
 { DRV_REG(REG_DAC_ATOP_0E_L), 0xFF, 0xA1, /*480_60P*/
                    0x00, /*576_50P*/
                    0xA2, /*720_50P*/
                    0xA2, /*720_60P*/
                    0xA2, /*1080_24P*/
                    0xA2, /*1080_25P*/
                    0xA2, /*1080_30P*/
                    0xA2, /*1080_50P*/
                    0xA2, /*1080_60P*/
                    0xA2, /*2560x1440_30P*/
                    0xA2, /*3840x2160_30P*/
                    0x31, /*1024x768_60P*/
                    0x25, /*1280x800_60P*/
                    0xB1, /*1280x1024_60P*/
                    0x7E, /*1366x768_60P*/
                    0xCB, /*1440x900_60P*/
                    0xDD, /*1680x1050_60P*/
                    0xAA, /*1600x1200_60P*/
 },
 { DRV_REG(REG_DAC_ATOP_0E_H), 0xFF, 0xEF, /*480_60P*/
                    0x00, /*576_50P*/
                    0x8B, /*720_50P*/
                    0x8B, /*720_60P*/
                    0x8B, /*1080_24P*/
                    0x8B, /*1080_25P*/
                    0x8B, /*1080_30P*/
                    0x8B, /*1080_50P*/
                    0x8B, /*1080_60P*/
                    0x8B, /*2560x1440_30P*/
                    0x8B, /*3840x2160_30P*/
                    0x2C, /*1024x768_60P*/
                    0x42, /*1280x800_60P*/
                    0x02, /*1280x1024_60P*/
                    0x47, /*1366x768_60P*/
                    0x63, /*1440x900_60P*/
                    0x9C, /*1680x1050_60P*/
                    0xAA, /*1600x1200_60P*/
 },
 { DRV_REG(REG_DAC_ATOP_0F_L), 0xFF, 0x3F, /*480_60P*/
                    0x40, /*576_50P*/
                    0x2E, /*720_50P*/
                    0x2E, /*720_60P*/
                    0x2E, /*1080_24P*/
                    0x2E, /*1080_25P*/
                    0x2E, /*1080_30P*/
                    0x2E, /*1080_50P*/
                    0x2E, /*1080_60P*/
                    0x2E, /*2560x1440_30P*/
                    0x2E, /*3840x2160_30P*/
                    0x35, /*1024x768_60P*/
                    0x29, /*1280x800_60P*/
                    0x20, /*1280x1024_60P*/
                    0x28, /*1366x768_60P*/
                    0x20, /*1440x900_60P*/
                    0x17, /*1680x1050_60P*/
                    0x2A, /*1600x1200_60P*/
 },
};

HAL_DISP_VGA_INFO stHAL_DISP_VGA_TIMING_TBL[HAL_DISP_VGA_TAB_NUM]=
{
    {*ST_IDAC_ATOP_TBL,IDAC_ATOP_REG_NUM, HAL_DISP_VGA_IP_NORMAL},
    {*ST_IDAC_ATOP_COMMON_TBL,IDAC_ATOP_COMMON_REG_NUM, HAL_DISP_VGA_IP_COMMON},
    {*ST_SYNTH_SET_HDMITX_TBL,SYNTH_SET_HDMITX_REG_NUM, HAL_DISP_VGA_IP_NORMAL},
    {*ST_SYNTH_SET_VGA_TBL,SYNTH_SET_VGA_REG_NUM, HAL_DISP_VGA_IP_NORMAL},
};
#endif

