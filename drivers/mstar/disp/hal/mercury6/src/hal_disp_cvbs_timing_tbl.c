/*
* hal_disp_cvbs_timing_tbl.c- Sigmastar
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
// Excel update date    : 2020/10/27 09:32
//****************************************************

#ifndef _HAL_DISP_CVBS_TIMING_TBL_C_
#define _HAL_DISP_CVBS_TIMING_TBL_C_

#include "hal_disp_cvbs_timing_tbl.h" 

//****************************************************
// CVBS_IDAC
//****************************************************
MS_U8 ST_CVBS_IDAC_COMMON_TBL[CVBS_IDAC_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] =
{
 { DRV_REG(REG_DAC_ATOP_14_L), 0x08, 0x08,
 },
 { DRV_REG(REG_DAC_ATOP_15_L), 0x08, 0x08,
 },
 { DRV_REG(REG_DAC_ATOP_16_L), 0xFF, 0x01,
 },
 { DRV_REG(REG_DAC_ATOP_1F_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_00_L), 0xF0, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_36_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DAC_ATOP_1D_L), 0xFF, 0x01,
 },
 { DRV_REG(REG_DAC_ATOP_29_L), 0xFF, 0x03,
 },
};

//****************************************************
// CVBS_PLL
//****************************************************
MS_U8 ST_CVBS_PLL_COMMON_TBL[CVBS_PLL_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] =
{
 { DRV_REG(REG_DISP_PLL_48_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_48_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_49_L), 0xFF, 0x40,
 },
 { DRV_REG(REG_DISP_PLL_40_L), 0xFF, 0x81,
 },
 { DRV_REG(REG_DISP_PLL_40_H), 0xFF, 0x22,
 },
 { DRV_REG(REG_DISP_PLL_41_L), 0xFF, 0x20,
 },
 { DRV_REG(REG_DISP_PLL_41_H), 0xFF, 0x04,
 },
 { DRV_REG(REG_DISP_PLL_42_L), 0xFF, 0x03,
 },
 { DRV_REG(REG_DISP_PLL_42_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_43_L), 0xFF, 0x03,
 },
 { DRV_REG(REG_DISP_PLL_43_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_4A_L), 0xFF, 0x01,
 },
 { DRV_REG(REG_DISP_PLL_4A_H), 0xFF, 0x58,
 },
 { DRV_REG(REG_DISP_PLL_4B_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_4B_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_6F_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_6F_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_4C_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_DISP_PLL_4C_H), 0xFF, 0x00,
 },
};

//****************************************************
// CVBS_TVENC
//****************************************************
MS_U8 ST_CVBS_TVENC_COMMON_TBL[CVBS_TVENC_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] =
{
 { DRV_REG(REG_TVENC_11_H), 0xFF, 0x03,
 },
 { DRV_REG(REG_TVENC_12_H), 0xFF, 0x02,
 },
 { DRV_REG(REG_TVENC_15_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_15_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_16_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_23_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_23_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_24_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_27_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_28_H), 0xFF, 0x03,
 },
 { DRV_REG(REG_TVENC_29_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_2A_H), 0xFF, 0x02,
 },
 { DRV_REG(REG_TVENC_3C_L), 0xFF, 0x0C,
 },
 { DRV_REG(REG_TVENC_3C_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_52_H), 0xFF, 0x41,
 },
 { DRV_REG(REG_TVENC_53_L), 0xFF, 0x06,
 },
 { DRV_REG(REG_TVENC_53_H), 0xFF, 0x08,
 },
 { DRV_REG(REG_TVENC_56_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_57_L), 0xFF, 0x04,
 },
 { DRV_REG(REG_TVENC_57_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_5C_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_5C_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_5D_L), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_5D_H), 0xFF, 0x00,
 },
 { DRV_REG(REG_TVENC_66_L), 0xFF, 0x80,
 },
 { DRV_REG(REG_TVENC_40_H), 0xFF, 0x00,
 },
};

MS_U8 ST_CVBS_TVENC_TBL[CVBS_TVENC_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + HAL_DISP_CVBS_ID_NUM]=
{
 { DRV_REG(REG_TVENC_11_L), 0xFF, 0x59, /*NTSC*/
                    0x5F, /*PAL*/
 },
 { DRV_REG(REG_TVENC_12_L), 0xFF, 0x0C, /*NTSC*/
                    0x70, /*PAL*/
 },
 { DRV_REG(REG_TVENC_16_L), 0xFF, 0x2C, /*NTSC*/
                    0x30, /*PAL*/
 },
 { DRV_REG(REG_TVENC_24_L), 0xFF, 0x89, /*NTSC*/
                    0x8F, /*PAL*/
 },
 { DRV_REG(REG_TVENC_27_L), 0xFF, 0x8A, /*NTSC*/
                    0x90, /*PAL*/
 },
 { DRV_REG(REG_TVENC_28_L), 0xFF, 0x59, /*NTSC*/
                    0x5F, /*PAL*/
 },
 { DRV_REG(REG_TVENC_29_L), 0xFF, 0x2D, /*NTSC*/
                    0x31, /*PAL*/
 },
 { DRV_REG(REG_TVENC_2A_L), 0xFF, 0x0C, /*NTSC*/
                    0x70, /*PAL*/
 },
 { DRV_REG(REG_TVENC_52_L), 0xFF, 0xA4, /*NTSC*/
                    0xA0, /*PAL*/
 },
 { DRV_REG(REG_TVENC_56_L), 0xFF, 0x60, /*NTSC*/
                    0x66, /*PAL*/
 },
 { DRV_REG(REG_TVENC_58_L), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_58_H), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_59_L), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_59_H), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_5A_L), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_5A_H), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_5B_L), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_5B_H), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_6E_L), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_6E_H), 0xFF, 0x40, /*NTSC*/
                    0x45, /*PAL*/
 },
 { DRV_REG(REG_TVENC_6F_L), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_6F_H), 0xFF, 0x5A, /*NTSC*/
                    0x62, /*PAL*/
 },
 { DRV_REG(REG_TVENC_66_H), 0xFF, 0x00, /*NTSC*/
                    0x10, /*PAL*/
 },
 { DRV_REG(REG_TVENC_40_L), 0xFF, 0xC3, /*NTSC*/
                    0xE3, /*PAL*/
 },
};

HAL_DISP_CVBS_INFO stHAL_DISP_CVBS_TIMING_TBL[HAL_DISP_CVBS_TAB_NUM]=
{
    {*ST_CVBS_IDAC_COMMON_TBL,CVBS_IDAC_COMMON_REG_NUM, HAL_DISP_CVBS_IP_COMMON},
    {*ST_CVBS_PLL_COMMON_TBL,CVBS_PLL_COMMON_REG_NUM, HAL_DISP_CVBS_IP_COMMON},
    {*ST_CVBS_TVENC_TBL,CVBS_TVENC_REG_NUM, HAL_DISP_CVBS_IP_NORMAL},
    {*ST_CVBS_TVENC_COMMON_TBL,CVBS_TVENC_COMMON_REG_NUM, HAL_DISP_CVBS_IP_COMMON},
};
#endif

