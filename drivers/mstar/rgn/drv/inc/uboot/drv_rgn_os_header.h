/*
* drv_rgn_os_header.h- Sigmastar
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

#ifndef __DRV_RGN_OS_HEADER_H__
#define __DRV_RGN_OS_HEADER_H__

#include <common.h>
#include <command.h>
#include <config.h>
#include <malloc.h>
#include <stdlib.h>


//=============================================================================
//  Defines & Macro
//=============================================================================
#ifndef NULL
#define NULL 0
#endif

typedef unsigned char       u8;
typedef signed   char       s8;
typedef unsigned short      u16;
typedef signed   short      s16;
typedef unsigned int        u32;
typedef signed   int        s32;
typedef unsigned long long  u64;
typedef signed   long long  s64;

#define MS_U8   u8
#define MS_S8   s8
#define MS_U16  u16
#define MS_S16  s16
#define MS_U32  u32
#define MS_S32  s32
#define MS_U64  u64
#define MS_S64  s64

#define CamOsMemAlloc       malloc
#define CamOsMemRelease     free
#define CamOsPanic(x)
#endif
