/*
* hal_osd.h- Sigmastar
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


#ifndef __HAL_OSD_H__
#define __HAL_OSD_H__
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
    E_HAL_OSD_PORT0,
    E_HAL_OSD_PORT1,
    E_HAL_OSD_PORT2,
    E_HAL_OSD_DIP,
    E_HAL_OSD_LDC,
    E_HAL_OSD_PORT3,
    E_HAL_OSD_PORT4,
    E_HAL_OSD_PORT5,
    E_HAL_OSD_DISP0_CUR,
    E_HAL_OSD_DISP0_UI,
    E_HAL_OSD_DISP1_CUR,
    E_HAL_OSD_DISP1_UI,
    E_HAL_OSD_ID_MAX
}HalOsdIdType_e;

typedef enum
{
    E_HAL_GOP_AE0 = 0,
    E_HAL_GOP_AE1,
    E_HAL_GOP_AE2,
    E_HAL_GOP_AE3,
    E_HAL_GOP_AE4,
    E_HAL_GOP_AE5,
    E_HAL_GOP_AE6,
    E_HAL_GOP_AE7
} HalOsdColorInvWindowIdType_e;
#include "hal_osd_if.h"
#endif /* __HAL_GOP_H__ */
