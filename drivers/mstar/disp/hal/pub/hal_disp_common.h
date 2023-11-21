/*
* hal_disp_common.h- Sigmastar
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

#ifndef _HAL_DISP_COMMON_H_
#define _HAL_DISP_COMMON_H_


//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
#ifndef u64
#define u64 unsigned long long
#endif
#ifndef s64
#define s64 signed long long
#endif
#ifndef u32
#define u32 unsigned int
#endif
#ifndef s32
#define s32 signed int
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef u8
#define u8 unsigned char
#endif
#ifndef s8
#define s8 signed char
#endif
#ifndef ss_phys_addr_t
#define ss_phys_addr_t unsigned long long
#endif
#ifndef ss_miu_addr_t
#define ss_miu_addr_t unsigned long long
#endif
#ifndef bool
#define bool unsigned char
#endif

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE 1
/// definition for FALSE
#define FALSE 0
#endif

#endif
