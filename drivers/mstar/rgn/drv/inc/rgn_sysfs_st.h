/*
* rgn_sysfs_st.h- Sigmastar
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
extern unsigned long geGopOsdFlag;
extern MS_U32 geGopIdFromOsd[E_DRV_OSD_ID_MAX];
extern _DrvGopLocalSettingsConfig_t _tGopLocSettings[HAL_RGN_GOP_NUM];
extern _DrvOsdLocalSettingsConfig_t _tOsdpLocSettings[E_HAL_OSD_ID_MAX];
extern _DrvCoverLocalSettingsConfig_t _tCoverLocSettings[E_HAL_COVER_ID_NUM];
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
#endif /* __HAL_SYSFS_H__ */
