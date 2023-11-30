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
#define  bool   unsigned char
extern bool gbrgndbglv;
#define DRV_RGN_DEBUG   (gbrgndbglv&DRVRGNDBGLV)
#define DRV_RGN_HLDEBUG   (gbrgndbglv&HLRGNDBGLV)

#define DRVRGNERR(fmt, arg...)    CamOsPrintf(KERN_WARNING fmt, ##arg)
#define DRVRGNDBG(fmt, arg...)    (DRV_RGN_DEBUG ? CamOsPrintf(KERN_DEBUG fmt, ##arg): DRV_RGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define DRVRGNHLDBG(fmt, arg...)    (DRV_RGN_HLDEBUG ? CamOsPrintf(KERN_DEBUG fmt, ##arg): DRV_RGN_HLDEBUG) //CamOsDebug(fmt, ##arg)
#define DRVRGNDBGERR(fmt, arg...) CamOsPrintf(KERN_DEBUG fmt, ##arg)
#define HALRGNDBGLV 0x1
#define DRVRGNDBGLV 0x2
#define HLRGNDBGLV 0x4  //high level

#define HAL_RGN_DEBUG   (gbrgndbglv&HALRGNDBGLV)
#define HAL_HLRGN_DEBUG   (gbrgndbglv&HLRGNDBGLV)
#define HALRGNERR(fmt, arg...) CamOsPrintf(KERN_WARNING fmt, ##arg) //CamOsDebug(fmt, ##arg)

#define HALRGNDBG(fmt, arg...) (HAL_RGN_DEBUG ? CamOsPrintf(KERN_DEBUG fmt, ##arg): HAL_RGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define HALHLRGNDBG(fmt, arg...) (HAL_HLRGN_DEBUG ? CamOsPrintf(KERN_DEBUG fmt, ##arg): HAL_HLRGN_DEBUG) //CamOsDebug(fmt, ##arg)
#define HALRGNDBGERR(fmt, arg...) CamOsPrintf(KERN_DEBUG fmt, ##arg)

//=============================================================================

//=============================================================================
#define RGN_IO_OFFSET 0xDE000000

#endif //
/** @} */ // end of hvsp_group
