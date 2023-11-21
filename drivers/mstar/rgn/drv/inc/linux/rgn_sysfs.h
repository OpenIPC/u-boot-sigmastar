/*
* rgn_sysfs.h- Sigmastar
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

#ifndef __RGN_SYSFS_H__
#define __RGN_SYSFS_H__
//=============================================================================
// enum
//=============================================================================
//=============================================================================
// struct
//=============================================================================
//=============================================================================
// Defines
//=============================================================================
//=============================================================================

//=============================================================================
#ifndef __RGN_SYSFS_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE void RgnSysfsInit(void);
INTERFACE void RgnSysfsDeInit(void);


#undef INTERFACE
#endif /* __HAL_SYSFS_H__ */
