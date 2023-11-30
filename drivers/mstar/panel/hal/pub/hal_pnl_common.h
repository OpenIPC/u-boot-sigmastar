/*
* hal_pnl_common.h- Sigmastar
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

#ifndef _HAL_PNL_COMMON_H_
#define _HAL_PNL_COMMON_H_


//-------------------------------------------------------------------------------------------------
// Type and Structure Declaration
//-------------------------------------------------------------------------------------------------
#define  u64    unsigned long long
#define  s64    signed long long
#define  u32    unsigned long
#define  s32    signed long
#define  u16    unsigned short
#define  s16    signed short
#define  u8     unsigned char
#define  s8     signed char

#ifndef bool
#define  bool   unsigned char
#endif

#ifndef NULL
#define NULL    (0)
#endif

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif


#endif
