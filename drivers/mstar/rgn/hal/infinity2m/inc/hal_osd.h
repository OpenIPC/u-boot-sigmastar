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

//=============================================================================
// struct
//=============================================================================

typedef struct
{
    u16 u16Th1;
    u16 u16Th2;
    u16 u16Th3;
    u16 u16X;
    u16 u16Y;
    u16 u16W;
    u16 u16H;
    u16 u16BlkNumX;
    u16 u16BlkNumY;
} HalOsdColorInvParamConfig_t;

//=============================================================================

//=============================================================================
#ifndef __HAL_OSD_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool HalOsdColInvParamCheck(HalOsdColorInvParamConfig_t *ptColInvCfg);
INTERFACE void HalOsdSetOsdBypassForScEnable(HalOsdIdType_e eGopId, bool bEn);
INTERFACE void HalOsdSetOsdEnable(HalOsdIdType_e eOsdId, bool bEn);
INTERFACE bool HalOsdSetBindGop(u32 u32GopOsdFlag);
INTERFACE bool HalOsdCheckIdSupport(HalOsdIdType_e eOsdId);
INTERFACE void HalOsdSetColorInverseEn(HalOsdIdType_e eGopId, bool bEn);
INTERFACE void HalOsdSetColorInverseParam(HalOsdIdType_e eGopId, HalOsdColorInvWindowIdType_e eAeId, HalOsdColorInvParamConfig_t *ptColInvCfg);
INTERFACE void HalOsdSetColorInverseUpdate(HalOsdIdType_e eGopId);
INTERFACE void HalOsdWriteColorInverseData(HalOsdIdType_e eGopId, u16 addr, u32 wdata);
INTERFACE void HalOsdReadColorInverseData(HalOsdIdType_e eGopId, u16 addr, u32 *rdata);
#undef INTERFACE
#endif /* __HAL_GOP_H__ */
