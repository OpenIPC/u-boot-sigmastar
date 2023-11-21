/*
* hal_audio_common.h- Sigmastar
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

#ifndef __HAL_AUD_COMMON_H__
#define __HAL_AUD_COMMON_H__

//=============================================================================
// Include files
//=============================================================================
#include <common.h>

// errno
#define EPERM       1
#define EIO         5
#define EAGAIN      11
#define ENOMEM      12
#define EFAULT      14
#define EINVAL      22
#define ENOTTY      25
#define EPIPE       32
#define EOVERFLOW   75
#define EBADFD      77

//
#if !defined(U8)
typedef unsigned char   U8;
typedef signed   char   S8;
typedef unsigned short  U16;
typedef signed   short  S16;
typedef unsigned int    U32;
typedef signed   int    S32;
typedef unsigned long long  U64;
typedef signed   long long  S64;
#endif

//
#if !defined(u8)
typedef unsigned char   u8;
typedef signed   char   s8;
typedef unsigned short  u16;
typedef signed   short  s16;
typedef unsigned int    u32;
typedef signed   int    s32;
typedef unsigned long long  u64;
typedef signed   long long  s64;
#endif

//
#if !defined(BOOL)
typedef unsigned int    BOOL;
#endif

//
#if !defined(TRUE) && !defined(FALSE)
#define TRUE            1
#define FALSE           0
#endif

//
#if !defined(NULL)
#define NULL (void*)0
#endif


#endif //__HAL_AUD_COMMON_H__