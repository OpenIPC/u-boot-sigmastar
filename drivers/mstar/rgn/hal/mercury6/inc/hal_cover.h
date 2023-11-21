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
    E_HAL_ISPSCL4_COVER,
    E_HAL_ISPSCL5_COVER,
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
    E_HAL_COVER_CMDQ_ID_NUM = 1,
}HalCoverCmdqIdType_e;

#include "hal_cover_if.h"
#endif /* __HAL_COVER_H__ */
