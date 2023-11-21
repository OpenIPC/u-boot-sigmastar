/*
* MsVersion.h- Sigmastar
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

#ifndef _MS_VERSION_H_
#define _MS_VERSION_H_

//-------------------------------------------------------------------------------------------------
//  Version String Definition
//-------------------------------------------------------------------------------------------------
#define MSIF_TAG           \
    {                      \
        'M', 'S', 'I', 'F' \
    } // MSIF
#define MSIF_CLASS \
    {              \
        '0', '0'   \
    }                    // DRV/API (DDI)
#define MSIF_CUS  0x0000 // CAMDRIVER Common library
#define MSIF_MOD  0x0000 // CAMDRIVER Common library
#define MSIF_CHIP 0x000F
#define MSIF_CPU  '1'
#define MSIF_OS   '0'

#endif // _MS_VERSION_H_
