/*
* hal_disp_cvbs_timing_tbl.h- Sigmastar
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

#ifndef __HAL_DISP_CVBS_TIMING_TBL__
#define __HAL_DISP_CVBS_TIMING_TBL__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mhal_common.h"


#define DRV_REG(reg)      ((reg >> 16) & 0xFF), (reg & 0xFFFF)
#define REG_TBL_ADDR_SIZE 2
#define REG_TBL_MASK_SIZE 1
#define REG_TBL_DATA_SIZE 1
#define REG_TBL_TABLE_END 0xFFFFFF
    //****************************************************

#define CVBS_IDAC_COMMON_REG_NUM   8
#define CVBS_PLL_REG_NUM     1
#define CVBS_PLL_COMMON_REG_NUM     6
#define CVBS_TVENC_REG_NUM         18
#define CVBS_TVENC_COMMON_REG_NUM  7
#define SCALER_TGEN_COMMON_REG_NUM 23

    //****************************************************

#define REG_DISP_PLL_BASE (0x103700)

    //****************************************************

    typedef enum
    {
        E_HAL_DISP_CVBS_ID_NTSC = 0,
        E_HAL_DISP_CVBS_ID_PAL  = 1,
        E_HAL_DISP_CVBS_ID_NUM  = 2,
    } E_HAL_DISP_CVBS_ID_TYPE;

    typedef enum
    {
        E_HAL_DISP_CVBS_COMMON_TAB_CVBS_IDAC,
#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
    E_HAL_DISP_CVBS_TAB_CVBS_PLL,
#endif
        E_HAL_DISP_CVBS_COMMON_TAB_CVBS_PLL,
#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
        E_HAL_DISP_CVBS_TAB_CVBS_TVENC,
#endif
    E_HAL_DISP_CVBS_COMMON_TAB_CVBS_TVENC,
        E_HAL_DISP_CVBS_TAB_NUM,
    } E_HAL_DISP_CVBS_TAB_TYPE;

    typedef enum
    {
        E_HAL_DISP_CVBS_IP_NORMAL,
        E_HAL_DISP_CVBS_IP_COMMON,
    } E_HAL_DISP_CVBS_IP_TYPE;

    typedef struct
    {
        u16 *                   pData;
        u16                     u16RegNum;
        E_HAL_DISP_CVBS_IP_TYPE enIPType;
    } HAL_DISP_CVBS_INFO;

    extern HAL_DISP_CVBS_INFO g_sthaldispcvbstimingtbl[E_HAL_DISP_CVBS_TAB_NUM];

#define REG_DISP_PLL_00_L (REG_DISP_PLL_BASE + 0x0)
#define REG_DISP_PLL_01_L (REG_DISP_PLL_BASE + 0x2)
#define REG_DISP_PLL_02_L (REG_DISP_PLL_BASE + 0x4)
#define REG_DISP_PLL_03_L (REG_DISP_PLL_BASE + 0x6)
#define REG_DISP_PLL_04_L (REG_DISP_PLL_BASE + 0x8)
#define REG_DISP_PLL_05_L (REG_DISP_PLL_BASE + 0xa)
#define REG_DISP_PLL_06_L (REG_DISP_PLL_BASE + 0xc)
#define REG_DISP_PLL_07_L (REG_DISP_PLL_BASE + 0xe)
#define REG_DISP_PLL_08_L (REG_DISP_PLL_BASE + 0x10)
#define REG_DISP_PLL_09_L (REG_DISP_PLL_BASE + 0x12)
#define REG_DISP_PLL_0A_L (REG_DISP_PLL_BASE + 0x14)
#define REG_DISP_PLL_0B_L (REG_DISP_PLL_BASE + 0x16)
#define REG_DISP_PLL_0C_L (REG_DISP_PLL_BASE + 0x18)
#define REG_DISP_PLL_0D_L (REG_DISP_PLL_BASE + 0x1a)
#define REG_DISP_PLL_0E_L (REG_DISP_PLL_BASE + 0x1c)
#define REG_DISP_PLL_0F_L (REG_DISP_PLL_BASE + 0x1e)
#define REG_DISP_PLL_10_L (REG_DISP_PLL_BASE + 0x20)
#define REG_DISP_PLL_11_L (REG_DISP_PLL_BASE + 0x22)
#define REG_DISP_PLL_12_L (REG_DISP_PLL_BASE + 0x24)
#define REG_DISP_PLL_13_L (REG_DISP_PLL_BASE + 0x26)
#define REG_DISP_PLL_14_L (REG_DISP_PLL_BASE + 0x28)
#define REG_DISP_PLL_15_L (REG_DISP_PLL_BASE + 0x2a)
#define REG_DISP_PLL_16_L (REG_DISP_PLL_BASE + 0x2c)
#define REG_DISP_PLL_17_L (REG_DISP_PLL_BASE + 0x2e)
#define REG_DISP_PLL_18_L (REG_DISP_PLL_BASE + 0x30)
#define REG_DISP_PLL_19_L (REG_DISP_PLL_BASE + 0x32)
#define REG_DISP_PLL_1A_L (REG_DISP_PLL_BASE + 0x34)
#define REG_DISP_PLL_1B_L (REG_DISP_PLL_BASE + 0x36)
#define REG_DISP_PLL_1C_L (REG_DISP_PLL_BASE + 0x38)
#define REG_DISP_PLL_1D_L (REG_DISP_PLL_BASE + 0x3a)
#define REG_DISP_PLL_1E_L (REG_DISP_PLL_BASE + 0x3c)
#define REG_DISP_PLL_1F_L (REG_DISP_PLL_BASE + 0x3e)
#define REG_DISP_PLL_20_L (REG_DISP_PLL_BASE + 0x40)
#define REG_DISP_PLL_21_L (REG_DISP_PLL_BASE + 0x42)
#define REG_DISP_PLL_22_L (REG_DISP_PLL_BASE + 0x44)
#define REG_DISP_PLL_23_L (REG_DISP_PLL_BASE + 0x46)
#define REG_DISP_PLL_24_L (REG_DISP_PLL_BASE + 0x48)
#define REG_DISP_PLL_25_L (REG_DISP_PLL_BASE + 0x4a)
#define REG_DISP_PLL_26_L (REG_DISP_PLL_BASE + 0x4c)
#define REG_DISP_PLL_27_L (REG_DISP_PLL_BASE + 0x4e)
#define REG_DISP_PLL_28_L (REG_DISP_PLL_BASE + 0x50)
#define REG_DISP_PLL_29_L (REG_DISP_PLL_BASE + 0x52)
#define REG_DISP_PLL_2A_L (REG_DISP_PLL_BASE + 0x54)
#define REG_DISP_PLL_2B_L (REG_DISP_PLL_BASE + 0x56)
#define REG_DISP_PLL_2C_L (REG_DISP_PLL_BASE + 0x58)
#define REG_DISP_PLL_2D_L (REG_DISP_PLL_BASE + 0x5a)
#define REG_DISP_PLL_2E_L (REG_DISP_PLL_BASE + 0x5c)
#define REG_DISP_PLL_2F_L (REG_DISP_PLL_BASE + 0x5e)
#define REG_DISP_PLL_30_L (REG_DISP_PLL_BASE + 0x60)
#define REG_DISP_PLL_31_L (REG_DISP_PLL_BASE + 0x62)
#define REG_DISP_PLL_32_L (REG_DISP_PLL_BASE + 0x64)
#define REG_DISP_PLL_33_L (REG_DISP_PLL_BASE + 0x66)
#define REG_DISP_PLL_34_L (REG_DISP_PLL_BASE + 0x68)
#define REG_DISP_PLL_35_L (REG_DISP_PLL_BASE + 0x6a)
#define REG_DISP_PLL_36_L (REG_DISP_PLL_BASE + 0x6c)
#define REG_DISP_PLL_37_L (REG_DISP_PLL_BASE + 0x6e)
#define REG_DISP_PLL_38_L (REG_DISP_PLL_BASE + 0x70)
#define REG_DISP_PLL_39_L (REG_DISP_PLL_BASE + 0x72)
#define REG_DISP_PLL_3A_L (REG_DISP_PLL_BASE + 0x74)
#define REG_DISP_PLL_3B_L (REG_DISP_PLL_BASE + 0x76)
#define REG_DISP_PLL_3C_L (REG_DISP_PLL_BASE + 0x78)
#define REG_DISP_PLL_3D_L (REG_DISP_PLL_BASE + 0x7a)
#define REG_DISP_PLL_3E_L (REG_DISP_PLL_BASE + 0x7c)
#define REG_DISP_PLL_3F_L (REG_DISP_PLL_BASE + 0x7e)
#define REG_DISP_PLL_40_L (REG_DISP_PLL_BASE + 0x80)
#define REG_DISP_PLL_41_L (REG_DISP_PLL_BASE + 0x82)
#define REG_DISP_PLL_42_L (REG_DISP_PLL_BASE + 0x84)
#define REG_DISP_PLL_43_L (REG_DISP_PLL_BASE + 0x86)
#define REG_DISP_PLL_44_L (REG_DISP_PLL_BASE + 0x88)
#define REG_DISP_PLL_45_L (REG_DISP_PLL_BASE + 0x8a)
#define REG_DISP_PLL_46_L (REG_DISP_PLL_BASE + 0x8c)
#define REG_DISP_PLL_47_L (REG_DISP_PLL_BASE + 0x8e)
#define REG_DISP_PLL_48_L (REG_DISP_PLL_BASE + 0x90)
#define REG_DISP_PLL_49_L (REG_DISP_PLL_BASE + 0x92)
#define REG_DISP_PLL_4A_L (REG_DISP_PLL_BASE + 0x94)
#define REG_DISP_PLL_4B_L (REG_DISP_PLL_BASE + 0x96)
#define REG_DISP_PLL_4C_L (REG_DISP_PLL_BASE + 0x98)
#define REG_DISP_PLL_4D_L (REG_DISP_PLL_BASE + 0x9a)
#define REG_DISP_PLL_4E_L (REG_DISP_PLL_BASE + 0x9c)
#define REG_DISP_PLL_4F_L (REG_DISP_PLL_BASE + 0x9e)
#define REG_DISP_PLL_50_L (REG_DISP_PLL_BASE + 0xa0)
#define REG_DISP_PLL_51_L (REG_DISP_PLL_BASE + 0xa2)
#define REG_DISP_PLL_52_L (REG_DISP_PLL_BASE + 0xa4)
#define REG_DISP_PLL_53_L (REG_DISP_PLL_BASE + 0xa6)
#define REG_DISP_PLL_54_L (REG_DISP_PLL_BASE + 0xa8)
#define REG_DISP_PLL_55_L (REG_DISP_PLL_BASE + 0xaa)
#define REG_DISP_PLL_56_L (REG_DISP_PLL_BASE + 0xac)
#define REG_DISP_PLL_57_L (REG_DISP_PLL_BASE + 0xae)
#define REG_DISP_PLL_58_L (REG_DISP_PLL_BASE + 0xb0)
#define REG_DISP_PLL_59_L (REG_DISP_PLL_BASE + 0xb2)
#define REG_DISP_PLL_5A_L (REG_DISP_PLL_BASE + 0xb4)
#define REG_DISP_PLL_5B_L (REG_DISP_PLL_BASE + 0xb6)
#define REG_DISP_PLL_5C_L (REG_DISP_PLL_BASE + 0xb8)
#define REG_DISP_PLL_5D_L (REG_DISP_PLL_BASE + 0xba)
#define REG_DISP_PLL_5E_L (REG_DISP_PLL_BASE + 0xbc)
#define REG_DISP_PLL_5F_L (REG_DISP_PLL_BASE + 0xbe)
#define REG_DISP_PLL_60_L (REG_DISP_PLL_BASE + 0xc0)
#define REG_DISP_PLL_61_L (REG_DISP_PLL_BASE + 0xc2)
#define REG_DISP_PLL_62_L (REG_DISP_PLL_BASE + 0xc4)
#define REG_DISP_PLL_63_L (REG_DISP_PLL_BASE + 0xc6)
#define REG_DISP_PLL_64_L (REG_DISP_PLL_BASE + 0xc8)
#define REG_DISP_PLL_65_L (REG_DISP_PLL_BASE + 0xca)
#define REG_DISP_PLL_66_L (REG_DISP_PLL_BASE + 0xcc)
#define REG_DISP_PLL_67_L (REG_DISP_PLL_BASE + 0xce)
#define REG_DISP_PLL_68_L (REG_DISP_PLL_BASE + 0xd0)
#define REG_DISP_PLL_69_L (REG_DISP_PLL_BASE + 0xd2)
#define REG_DISP_PLL_6A_L (REG_DISP_PLL_BASE + 0xd4)
#define REG_DISP_PLL_6B_L (REG_DISP_PLL_BASE + 0xd6)
#define REG_DISP_PLL_6C_L (REG_DISP_PLL_BASE + 0xd8)
#define REG_DISP_PLL_6D_L (REG_DISP_PLL_BASE + 0xda)
#define REG_DISP_PLL_6E_L (REG_DISP_PLL_BASE + 0xdc)
#define REG_DISP_PLL_6F_L (REG_DISP_PLL_BASE + 0xde)
#define REG_DISP_PLL_70_L (REG_DISP_PLL_BASE + 0xe0)
#define REG_DISP_PLL_71_L (REG_DISP_PLL_BASE + 0xe2)
#define REG_DISP_PLL_72_L (REG_DISP_PLL_BASE + 0xe4)
#define REG_DISP_PLL_73_L (REG_DISP_PLL_BASE + 0xe6)
#define REG_DISP_PLL_74_L (REG_DISP_PLL_BASE + 0xe8)
#define REG_DISP_PLL_75_L (REG_DISP_PLL_BASE + 0xea)
#define REG_DISP_PLL_76_L (REG_DISP_PLL_BASE + 0xec)
#define REG_DISP_PLL_77_L (REG_DISP_PLL_BASE + 0xee)
#define REG_DISP_PLL_78_L (REG_DISP_PLL_BASE + 0xf0)
#define REG_DISP_PLL_79_L (REG_DISP_PLL_BASE + 0xf2)
#define REG_DISP_PLL_7A_L (REG_DISP_PLL_BASE + 0xf4)
#define REG_DISP_PLL_7B_L (REG_DISP_PLL_BASE + 0xf6)
#define REG_DISP_PLL_7C_L (REG_DISP_PLL_BASE + 0xf8)
#define REG_DISP_PLL_7D_L (REG_DISP_PLL_BASE + 0xfa)
#define REG_DISP_PLL_7E_L (REG_DISP_PLL_BASE + 0xfc)
#define REG_DISP_PLL_7F_L (REG_DISP_PLL_BASE + 0xfe)

#ifdef __cplusplus
}
#endif

#endif
