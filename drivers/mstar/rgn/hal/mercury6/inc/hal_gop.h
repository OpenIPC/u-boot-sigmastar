/*
* hal_gop.h- Sigmastar
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


#ifndef __HAL_GOP_H__
#define __HAL_GOP_H__
#include "hal_rgn_common.h"

//=============================================================================
// Defines
//=============================================================================
#ifndef bool
#define  bool   unsigned char
#endif

#define GOP_BANK_ALLBANK_DOUBLE_WR 0x0100
#define GOP_BANK_FORCE_WR 0x0200
#define GOP_BANK_ONEBANK_DOUBLE_WR 0x0400
#define HAL_GOP_GET_GWIN_ID(eGopId,eGwinId) (eGopId)
#define HAL_GOP_TRANS_ID(eGopId) (eGopId)
//=============================================================================
// enum
//=============================================================================
typedef enum
{
    E_HAL_GOP_ID_0 = 0, // SCL0
    E_HAL_GOP_ID_1,     // SCL1
    E_HAL_GOP_ID_2,     // SCL2
    E_HAL_GOP_ID_3,     // SCL3
    E_HAL_GOP_ID_4,     // SCL4
    E_HAL_GOP_ID_5,     // DIVP/LDC/SCL5
    E_HAL_GOP_ID_6,     // DISP0_CUR
    E_HAL_GOP_ID_7,     // DISP0_UI
    E_HAL_GOP_ID_8,     // DISP1_CUR
    E_HAL_GOP_ID_9,     // DISP1_UI
    E_HAL_GOP_ID_MAX,
} HalGopIdType_e;
typedef enum
{
    E_HAL_GOP_GWIN_ID_0 = 0,
    E_HAL_GOP_GWIN_ID_1,
    E_HAL_GOP_GWIN_ID_2,
    E_HAL_GOP_GWIN_ID_3,
    E_HAL_GOP_GWIN_ID_4,
    E_HAL_GOP_GWIN_ID_5,
    E_HAL_GOP_GWIN_ID_6,
    E_HAL_GOP_GWIN_ID_7,
    E_HAL_GOP_GWIN_ID_NUM,
} HalGopGwinIdType_e;
typedef enum
{
    E_HAL_GOP_GWIN_SRC_I8_PALETTE = 0,
    E_HAL_GOP_GWIN_SRC_I4_PALETTE = 0x01,
    E_HAL_GOP_GWIN_SRC_I2_PALETTE = 0x02,
    E_HAL_GOP_GWIN_SRC_ARGB1555   = 0x03,
    E_HAL_GOP_GWIN_SRC_ARGB4444   = 0x04,
    E_HAL_GOP_GWIN_SRC_RGB1555    = 0x05,
    E_HAL_GOP_GWIN_SRC_RGB565     = 0x06,
    E_HAL_GOP_GWIN_SRC_2266       = 0x07,
    E_HAL_GOP_GWIN_SRC_ARGB8888   = 0x08,
    E_HAL_GOP_GWIN_SRC_ABGR8888   = 0x09,
    E_HAL_GOP_GWIN_SRC_UV7Y8      = 0x0A,
    E_HAL_GOP_GWIN_SRC_UV8Y8      = 0x0B,
    E_HAL_GOP_GWIN_SRC_RGBA5551   = 0x0C,
    E_HAL_GOP_GWIN_SRC_RGBA4444   = 0x0D,
    E_HAL_GOP_GWIN_SRC_NOTSUPPORT   = 0xFF,
} HalGopGwinSrcFormat_e;
typedef enum
{
    // old version
    E_HAL_GOP_OLD_GWIN_SRC_RGB1555    = 0x00,
    E_HAL_GOP_OLD_GWIN_SRC_RGB565     = 0x01,
    E_HAL_GOP_OLD_GWIN_SRC_ARGB4444   = 0x02,
    E_HAL_GOP_OLD_GWIN_SRC_2266       = 0x03,
    E_HAL_GOP_OLD_GWIN_SRC_I8_PALETTE = 0x04,
    E_HAL_GOP_OLD_GWIN_SRC_ARGB8888   = 0x05,
    E_HAL_GOP_OLD_GWIN_SRC_ARGB1555   = 0x06,
    E_HAL_GOP_OLD_GWIN_SRC_ABGR8888   = 0x07,
    E_HAL_GOP_OLD_GWIN_SRC_UV7Y8      = 0x08,
    E_HAL_GOP_OLD_GWIN_SRC_UV8Y8      = 0x09,
    E_HAL_GOP_OLD_GWIN_SRC_RGBA5551   = 0x0A,
    E_HAL_GOP_OLD_GWIN_SRC_RGBA4444   = 0x0B,
    E_HAL_GOP_OLD_GWIN_SRC_I4_PALETTE = 0x0D,
    E_HAL_GOP_OLD_GWIN_SRC_I2_PALETTE = 0x0E,
    // new version
    E_HAL_GOP_NEW_GWIN_SRC_I8_PALETTE = 0x00,
    E_HAL_GOP_NEW_GWIN_SRC_I4_PALETTE = 0x01,
    E_HAL_GOP_NEW_GWIN_SRC_I2_PALETTE = 0x02,
    E_HAL_GOP_NEW_GWIN_SRC_ARGB1555   = 0x03,
    E_HAL_GOP_NEW_GWIN_SRC_ARGB4444   = 0x04,
    E_HAL_GOP_GWIN_SRC_VAL_NOTSUPPORT = 0xFF,
} HalGopGwinSrcFormatRealVal_e;

typedef enum
{
    E_HAL_GOP_SHADOW_h11 = 0,
    E_HAL_GOP_SHADOW_h12 = 1,
    E_HAL_GOP_SHADOW_h20 = 2,
    E_HAL_GOP_SHADOW_h24 = 3,
    E_HAL_GOP_SHADOW_h28 = 4,
    E_HAL_GOP_SHADOW_Num = 5,
}HalGopShadow_e;
typedef enum
{
    E_HAL_GWIN_SHADOW_h0 = 0,
    E_HAL_GWIN_SHADOW_h8 = 1,
    E_HAL_GWIN_SHADOW_Num = 2,
}HalGwinShadow_e;
#include "hal_gop_if.h"

#endif /* __HAL_GOP_H__ */
