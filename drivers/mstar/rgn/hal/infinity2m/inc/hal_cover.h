/*
* hal_cover.h- Sigmastar
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


#ifndef __HAL_COVER_H__
#define __HAL_COVER_H__
#include "hal_rgn_common.h"
//=============================================================================
// Defines
//=============================================================================
#ifndef bool
#define  bool   unsigned char
#endif

//=============================================================================
// enum
//=============================================================================
typedef enum
{
    E_HAL_ISPSCL0_COVER = 0,
    E_HAL_ISPSCL1_COVER,
    E_HAL_ISPSCL2_COVER,
    E_HAL_ISPSCL3_COVER,
    E_HAL_DIP_COVER_ID,
    E_HAL_COVER_ID_NUM,
} HalCoverIdType_e;

typedef enum
{
    E_HAL_COVER_BWIN_ID_0 = 0,
    E_HAL_COVER_BWIN_ID_1 = 1,
    E_HAL_COVER_BWIN_ID_2 = 2,
    E_HAL_COVER_BWIN_ID_3 = 3,
    E_HAL_COVER_BWIN_ID_NUM = 4,
} HalCoverWinIdType_e;

typedef enum
{
    E_HAL_COVER_CMDQ_VPE_ID_0 = 0,
    E_HAL_COVER_CMDQ_DIVP_ID_1 = 1,
    E_HAL_COVER_CMDQ_ID_NUM = 1, // I5 not support DIVP cover
}HalCoverCmdqIdType_e;

//=============================================================================
// struct
//=============================================================================
typedef struct
{
    u16 u16X;
    u16 u16Y;
    u16 u16Width;
    u16 u16Height;
} HalCoverWindowType_t;

typedef struct
{
    u8 u8R;
    u8 u8G;
    u8 u8B;
} HalCoverColorType_t;

//=============================================================================

//=============================================================================
#ifndef __HAL_COVER_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool HalCoverCheckIdSupport(HalCoverIdType_e eCoverId);
INTERFACE void HalCoverSetWindowSize(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverWindowType_t *ptWinCfg);
INTERFACE void HalCoverSetColor(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverColorType_t *ptType);
INTERFACE void HalCoverSetEnableWin(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, bool bEn);

#undef INTERFACE
#endif /* __HAL_COVER_H__ */
