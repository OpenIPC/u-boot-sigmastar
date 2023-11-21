/*
* hal_disp_vga_timing_tbl.h- Sigmastar
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

#ifndef __HAL_DISP_VGA_TIMING_TBL__
#define __HAL_DISP_VGA_TIMING_TBL__

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

#define CLK_COMMON_REG_NUM         4
#define IDAC_ATOP_REG_NUM          1
#define IDAC_ATOP_COMMON_REG_NUM   10
#define SYNTH_SET_HDMITX_REG_NUM   2
#define SYNTH_SET_VGA_REG_NUM      4
#define SCALER_TGEN_COMMON_REG_NUM 23

    //****************************************************

    //****************************************************

typedef enum 
{
    E_HAL_DISP_VGA_ID_720_24P = 0,
    E_HAL_DISP_VGA_ID_720_25P = 1,
    E_HAL_DISP_VGA_ID_720_29D97P = 2,
    E_HAL_DISP_VGA_ID_720_30P = 3,
    E_HAL_DISP_VGA_ID_720_50P = 4,
    E_HAL_DISP_VGA_ID_720_59D94P = 5,
    E_HAL_DISP_VGA_ID_720_60P = 6,
    E_HAL_DISP_VGA_ID_1080_23D98P = 7,
    E_HAL_DISP_VGA_ID_1080_24P = 8,
    E_HAL_DISP_VGA_ID_1080_25P = 9,
    E_HAL_DISP_VGA_ID_1080_29D97P = 10,
    E_HAL_DISP_VGA_ID_1080_30P = 11,
    E_HAL_DISP_VGA_ID_1080_50P = 12,
    E_HAL_DISP_VGA_ID_1080_59D94P = 13,
    E_HAL_DISP_VGA_ID_1080_60P = 14,
    E_HAL_DISP_VGA_ID_2560X1440_30P = 15,
    E_HAL_DISP_VGA_ID_2560X1440_50P = 16,
    E_HAL_DISP_VGA_ID_2560X1440_60P = 17,
    E_HAL_DISP_VGA_ID_3840X2160_24P = 18,
    E_HAL_DISP_VGA_ID_3840X2160_25P = 19,
    E_HAL_DISP_VGA_ID_3840X2160_29D97P = 20,
    E_HAL_DISP_VGA_ID_3840X2160_30P = 21,
    E_HAL_DISP_VGA_ID_3840X2160_60P = 22,
    E_HAL_DISP_VGA_ID_640X480_60P = 23,
    E_HAL_DISP_VGA_ID_480_60P = 24,
    E_HAL_DISP_VGA_ID_576_50P = 25,
    E_HAL_DISP_VGA_ID_800X600_60P = 26,
    E_HAL_DISP_VGA_ID_848X480_60P = 27,
    E_HAL_DISP_VGA_ID_1024X768_60P = 28,
    E_HAL_DISP_VGA_ID_1280X768_60P = 29,
    E_HAL_DISP_VGA_ID_1280X800_60P = 30,
    E_HAL_DISP_VGA_ID_1280X960_60P = 31,
    E_HAL_DISP_VGA_ID_1280X1024_60P = 32,
    E_HAL_DISP_VGA_ID_1360X768_60P = 33,
    E_HAL_DISP_VGA_ID_1366X768_60P = 34,
    E_HAL_DISP_VGA_ID_1400X1050_60P = 35,
    E_HAL_DISP_VGA_ID_1440X900_60P = 36,
    E_HAL_DISP_VGA_ID_1600X900_60P = 37,
    E_HAL_DISP_VGA_ID_1600X1200_60P = 38,
    E_HAL_DISP_VGA_ID_1680X1050_60P = 39,
    E_HAL_DISP_VGA_ID_1920X1200_60P = 40,
    E_HAL_DISP_VGA_ID_1920X1440_60P = 41,
    E_HAL_DISP_VGA_ID_2560X1600_60P = 42,
    E_HAL_DISP_VGA_ID_NUM = 43,
} E_HAL_DISP_VGA_ID_TYPE;

    typedef enum
    {
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
        E_HAL_DISP_VGA_TAB_IDAC_ATOP,
#endif
        E_HAL_DISP_VGA_COMMON_TAB_IDAC_ATOP,
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
        E_HAL_DISP_VGA_TAB_SYNTH_SET_HDMITX,
#endif
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF 
        E_HAL_DISP_VGA_TAB_SYNTH_SET_VGA,
#endif
        E_HAL_DISP_VGA_TAB_NUM,
    } E_HAL_DISP_VGA_TAB_TYPE;

    typedef enum
    {
        E_HAL_DISP_VGA_IP_NORMAL,
        E_HAL_DISP_VGA_IP_COMMON,
    } E_HAL_DISP_VGA_IP_TYPE;

    typedef struct
    {
        u16 *                  pData;
        u16                    u16RegNum;
        E_HAL_DISP_VGA_IP_TYPE enIPType;
    } HAL_DISP_VGA_INFO;

    extern HAL_DISP_VGA_INFO g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_TAB_NUM];

#ifdef __cplusplus
}
#endif

#endif
