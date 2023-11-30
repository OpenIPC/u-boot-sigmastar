/*
* rgn_debug.h- Sigmastar
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

#ifndef __RGN_DEBUG_H__
#define __RGN_DEBUG_H__

//=============================================================================
// Defines
//=============================================================================
//=============================================================================
// struct
//=============================================================================
#ifndef bool
#define  bool   unsigned char
#endif

extern bool gbrgndbglv;
#define DRV_RGN_DEBUG   (gbrgndbglv&DRVRGNDBGLV)
#define DRV_RGN_HLDEBUG   (gbrgndbglv&HLRGNDBGLV)

#define DRVRGNERR(fmt, arg...)    printf(fmt, ##arg)
#define DRVRGNDBG(fmt, arg...)    (DRV_RGN_DEBUG ? printf(fmt, ##arg): DRV_RGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define DRVRGNHLDBG(fmt, arg...)    (DRV_RGN_HLDEBUG ? printf(fmt, ##arg): DRV_RGN_HLDEBUG) //CamOsDebug(fmt, ##arg)
#define DRVRGNDBGERR(fmt, arg...) printf(fmt, ##arg)
#define HALRGNDBGLV 0x1
#define DRVRGNDBGLV 0x2
#define HLRGNDBGLV 0x4  //high level
#define REGRGNDBGLV 0x8 // for write register

#define HAL_RGN_DEBUG   (gbrgndbglv&HALRGNDBGLV)
#define HAL_HLRGN_DEBUG   (gbrgndbglv&HLRGNDBGLV)
#define HAL_REGRGN_DEBUG (gbrgndbglv&REGRGNDBGLV)
#define HALRGNERR(fmt, arg...) printf(fmt, ##arg) //CamOsDebug(fmt, ##arg)

#define HALRGNDBG(fmt, arg...) (HAL_RGN_DEBUG ? printf(fmt, ##arg): HAL_RGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define HALHLRGNDBG(fmt, arg...) (HAL_HLRGN_DEBUG ? printf(fmt, ##arg): HAL_HLRGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define HALREGRGNDGB(fmt, arg...) (HAL_REGRGN_DEBUG ? printf(fmt, ##arg): HAL_REGRGN_DEBUG)
#define HALRGNDBGERR(fmt, arg...) printf(fmt, ##arg)

//=============================================================================

//=============================================================================
#define RGN_IO_OFFSET 0x00000000

#endif //
/** @} */ // end of hvsp_group
