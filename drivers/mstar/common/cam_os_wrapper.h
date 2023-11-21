/*
* cam_os_wrapper.h- Sigmastar
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

///////////////////////////////////////////////////////////////////////////////
/// @file      cam_os_wrapper.h
/// @brief     Cam OS Wrapper Header File for
///            1. RTK OS
///            2. Linux User Space
///            3. Linux Kernel Space
///////////////////////////////////////////////////////////////////////////////

#ifndef __CAM_OS_WRAPPER_H__
#define __CAM_OS_WRAPPER_H__

#define CAM_OS_WRAPPER_VERSION "v2.0.42"

#if defined(__aarch64__)
#define CAM_OS_BITS_PER_LONG 64
#else
#define CAM_OS_BITS_PER_LONG 32
#endif

#ifndef NULL
#define NULL 0
#endif

typedef unsigned char      u8;
typedef signed char        s8;
typedef unsigned short     u16;
typedef signed short       s16;
typedef unsigned int       u32;
typedef signed int         s32;
typedef unsigned long long u64;
typedef signed long long   s64;


typedef unsigned long long ss_phys_addr_t;
typedef unsigned long long ss_miu_addr_t;



#define CAM_OS_MAX_TIMEOUT ((u32)(~0U))
#define CAM_OS_MAX_INT     ((s32)(~0U >> 1))
typedef enum
{
    CAM_OS_OK            = 0,
    CAM_OS_FAIL          = -1,
    CAM_OS_PARAM_ERR     = -2,
    CAM_OS_ALLOCMEM_FAIL = -3,
    CAM_OS_TIMEOUT       = -4,
    CAM_OS_RESOURCE_BUSY = -5,
    CAM_OS_INTERRUPTED   = -6,
} CamOsRet_e;

#endif /* __CAM_OS_WRAPPER_H__ */
