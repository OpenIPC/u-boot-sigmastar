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
//****************************************************

#ifndef __HAL_DISP_CVBS_TIMING_TBL_C__
#define __HAL_DISP_CVBS_TIMING_TBL_C__

#include "hal_disp_cvbs_timing_tbl.h"
#include "reg_disp_top.h"
#include "drv_disp_os.h"
//****************************************************
// CVBS_IDAC
//****************************************************
u16 g_stcvbsidaccommontbl[CVBS_IDAC_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] = {
    {
        DRV_REG(REG_DAC_ATOP_14_L), /* DAC_ATOP_REG_EN_HD_DAC_C_DET */
        0x08,
        0x08,
    },
    {
        DRV_REG(REG_DAC_ATOP_15_L), /* DAC_ATOP_REG_EN_IDAC_C */
        0x08,
        0x08,
    },
    {
        DRV_REG(REG_DAC_ATOP_16_L), /* DAC_ATOP_REG_EN_IDAC_REF */
        0xFF,
        0x01,
    },
    {
        DRV_REG(REG_DAC_ATOP_1F_L), /* DAC_ATOP_REG_PD_IDAC_LDO */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_00_L), /* DAC_ATOP_REG_CKG_CVBS_ATOP */
        0xF0,
        0x00,
    },
    {
        DRV_REG(REG_DAC_ATOP_36_L), /* DAC_ATOP_REG_EMMCPLL_PD */
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
        DRV_REG(REG_DAC_ATOP_29_L), /* DAC_ATOP_REG_GCR_MODE_RGB, DAC_ATOP_REG_GCR_MODE_CVBS */
        0xFF,
        0x03,
    },
};

//****************************************************
// CVBS_PLL
//****************************************************
u16 g_stcvbspllcommontbl[CVBS_PLL_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] = 
{
    { DRV_REG(REG_DISP_PLL_49_L),/*  */
   0xFF, 0x3d,
    },
    { DRV_REG(REG_DISP_PLL_40_L),/*  */
   0xffff, 0x2a81,
    },
    { DRV_REG(REG_DISP_PLL_50_L),/*  */
   0xffff, 0x1000,
    }, 
    { DRV_REG(REG_DISP_PLL_53_L),/*  */
   0x2ff, 0xc,
    }, 
    { DRV_REG(REG_DISP_PLL_54_L),/*  */
   0xc0, 0x00,
    }, 
    { DRV_REG(REG_DISP_PLL_59_L),/*  */
   0xFF, 0xbc,
    },
};

#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
u16 g_stcvbsplltbl[CVBS_PLL_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_CVBS_ID_NUM] = 
{
    { DRV_REG(REG_DISP_PLL_48_L)/*  */,
   0xffff,           0x70a8,/*NTSC*/
           0x70a4,/*PAL*/
    }, 
};
#endif 
//****************************************************
// CVBS_TVENC
//****************************************************
u16 g_stcvbstvenccommontbl[CVBS_TVENC_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + REG_TBL_DATA_SIZE] = {
    {
        DRV_REG(REG_TVENC_15_L), /* TVENC_ */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_TVENC_23_L), /* TVENC_ */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_TVENC_3C_L), /* TVENC_, TVENC_REG_TVE_DAC_CLK_DIV2_EN, TVENC_REG_TVE_CLK_EN, TVENC_ */
        0xffff,
        0xc,
    },
    {
        DRV_REG(REG_TVENC_53_L), /* TVENC_ */
        0xffff,
        0x806,
    },
    {
        DRV_REG(REG_TVENC_57_L), /* TVENC_ */
        0xffff,
        0x4,
    },
    {
        DRV_REG(REG_TVENC_5C_L), /* TVENC_ */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_TVENC_5D_L), /* TVENC_ */
        0xffff,
        0x0,
    },
};

#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
u16 g_stcvbstvenctbl[CVBS_TVENC_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_DISP_CVBS_ID_NUM] = {
    {
        DRV_REG(REG_TVENC_11_L) /* TVENC_ */, 0xffff, 0x359, /*NTSC*/
        0x35f,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_12_L) /* TVENC_ */, 0xffff, 0x20c, /*NTSC*/
        0x270,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_16_L) /* TVENC_ */, 0xffff, 0x2c, /*NTSC*/
        0x30,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_24_L) /* TVENC_ */, 0xffff, 0x89, /*NTSC*/
        0x8f,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_27_L) /* TVENC_ */, 0xffff, 0x8a, /*NTSC*/
        0x90,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_28_L) /* TVENC_ */, 0xffff, 0x359, /*NTSC*/
        0x35f,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_29_L) /* TVENC_ */, 0xffff, 0x2d, /*NTSC*/
        0x31,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_2A_L) /* TVENC_ */, 0xffff, 0x20c, /*NTSC*/
        0x270,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_52_L) /* TVENC_ */, 0xffff, 0x41a4, /*NTSC*/
        0x41a0,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_56_L) /* TVENC_ */, 0xffff, 0x60, /*NTSC*/
        0x66,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_58_L) /* TVENC_ */, 0xffff, 0x4040, /*NTSC*/
        0x4545,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_59_L) /* TVENC_ */, 0xffff, 0x4040, /*NTSC*/
        0x4545,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_5A_L) /* TVENC_ */, 0xffff, 0x5a5a, /*NTSC*/
        0x6262,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_5B_L) /* TVENC_ */, 0xffff, 0x5a5a, /*NTSC*/
        0x6262,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_6E_L) /* TVENC_ */, 0xffff, 0x4040, /*NTSC*/
        0x4545,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_6F_L) /* TVENC_ */, 0xffff, 0x5a5a, /*NTSC*/
        0x6262,                                               /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_66_L) /* TVENC_ */, 0xffff, 0x80, /*NTSC*/
        0x1080,                                             /*PAL*/
    },
    {
        DRV_REG(REG_TVENC_40_L) /* TVENC_REG_TVIF_EN, TVENC_REG_TVE_R2Y_EN, TVENC_REG_TVE_POSITION_EN,
                                   TVENC_REG_UV_422_MODE, TVENC_REG_TVE_PAL_MODE, TVENC_REG_TVE_DAC_YOUT_EN,
                                   TVENC_REG_TVE_DAC_COUT_EN, TVENC_ */
        ,
        0xffff, 0xc3, /*NTSC*/
        0xe3,         /*PAL*/
    },
};
#endif 

//****************************************************
HAL_DISP_CVBS_INFO g_sthaldispcvbstimingtbl[E_HAL_DISP_CVBS_TAB_NUM]=
{
    {*g_stcvbsidaccommontbl, CVBS_IDAC_COMMON_REG_NUM, E_HAL_DISP_CVBS_IP_COMMON},
#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
    {*g_stcvbsplltbl, CVBS_PLL_REG_NUM, E_HAL_DISP_CVBS_IP_NORMAL},
#endif
    {*g_stcvbspllcommontbl, CVBS_PLL_COMMON_REG_NUM, E_HAL_DISP_CVBS_IP_COMMON},
#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
    {*g_stcvbstvenctbl, CVBS_TVENC_REG_NUM, E_HAL_DISP_CVBS_IP_NORMAL},
#endif
    {*g_stcvbstvenccommontbl, CVBS_TVENC_COMMON_REG_NUM, E_HAL_DISP_CVBS_IP_COMMON},
};

#endif
