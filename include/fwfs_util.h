/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
/*
 * fwfs utility functions
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef FWFS_UTIL_H
#define FWFS_UTIL_H

// Users can override fwfs_util.h with their own configuration by defining
// FWFS_CONFIG as a header file to include (-DFWFS_CONFIG=fwfs_config.h).
//
// If FWFS_CONFIG is used, none of the default utils will be emitted and must be
// provided by the config file. To start, I would suggest copying fwfs_util.h
// and modifying as needed.
#ifdef FWFS_CONFIG
#define FWFS_STRINGIZE(x) FWFS_STRINGIZE2(x)
#define FWFS_STRINGIZE2(x) #x
#include FWFS_STRINGIZE(FWFS_CONFIG)
#else

// System includes
#include <stdint.h>
#include <linux/string.h>
#include <stdarg.h>
#include <vsprintf.h>
#include <inttypes.h>

#ifndef FWFS_NO_MALLOC
#include <stdlib.h>
#endif
#ifndef FWFS_NO_ASSERT
//#include <assert.h>
#endif
#if !defined(FWFS_NO_DEBUG) || \
        !defined(FWFS_NO_WARN) || \
        !defined(FWFS_NO_ERROR) || \
        defined(FWFS_YES_TRACE)
//#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

extern int printf(const char *fmt, ...);

// Macros, may be replaced by system specific wrappers. Arguments to these
// macros must not have side-effects as the macros can be removed for a smaller
// code footprint

// Logging functions
#ifdef FWFS_YES_TRACE
#define FWFS_TRACE_(fmt, ...) \
    printf("%s:%d:trace: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define FWFS_TRACE(...) FWFS_TRACE_(__VA_ARGS__, "")
#else
#define FWFS_TRACE(...)
#endif

#ifndef FWFS_NO_DEBUG
#define FWFS_DEBUG_(fmt, ...) \
    printf("%s:%d:debug: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define FWFS_DEBUG(...) FWFS_DEBUG_(__VA_ARGS__, "")
#else
#define FWFS_DEBUG(...)
#endif

#ifndef FWFS_NO_WARN
#define FWFS_WARN_(fmt, ...) \
    printf("%s:%d:warn: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define FWFS_WARN(...) FWFS_WARN_(__VA_ARGS__, "")
#else
#define FWFS_WARN(...)
#endif

#ifndef FWFS_NO_INFO
#define FWFS_INFO_(fmt, ...) \
    printf("%s:%d:info: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define FWFS_INFO(...) FWFS_INFO_(__VA_ARGS__, "")
#else
#define FWFS_INFO(...)
#endif

#ifndef FWFS_NO_ERROR
#define FWFS_ERROR_(fmt, ...) \
    printf("%s:%d:error: " fmt "%s\n", __FILE__, __LINE__, __VA_ARGS__)
#define FWFS_ERROR(...) FWFS_ERROR_(__VA_ARGS__, "")
#else
#define FWFS_ERROR(...)
#endif

// Runtime assertions
#ifndef FWFS_NO_ASSERT
#define FWFS_ASSERT(test)                                               \
    do {                                                                \
        if (!(test)) {                                                  \
            printf("%s:%d:assert fail:%s\n",                            \
                        __FILE__, __LINE__, #test);                     \
        }                                                               \
    } while (0)
#else
#define FWFS_ASSERT(test)
#endif

#define FWFS_INDEX_NULL ((fwfs_off_t)-3)
#define FWFS_FID_NULL   ((fwfs_off_t)-4)

static inline size_t strcspn(const char *s, const char *reject)
{
    size_t count = 0;

    while (*s != '\0')
    {
        if (strchr(reject, *s++) == NULL)
        {
            ++count;
        } else
        {
            return count;
        }
    }
    return count;
}

// Builtin functions, these may be replaced by more efficient
// toolchain-specific implementations. FWFS_NO_INTRINSICS falls back to a more
// expensive basic C implementation for debugging purposes

// Min/max functions for unsigned 32-bit numbers
static inline uint32_t fwfs_max(uint32_t a, uint32_t b) {
    return (a > b) ? a : b;
}

static inline uint32_t fwfs_min(uint32_t a, uint32_t b) {
    return (a < b) ? a : b;
}

// Align to nearest multiple of a size
static inline uint32_t fwfs_aligndown(uint32_t a, uint32_t alignment) {
    return a - (a % alignment);
}

static inline uint32_t fwfs_alignup(uint32_t a, uint32_t alignment) {
    return fwfs_aligndown(a + alignment-1, alignment);
}

// Find the smallest power of 2 greater than or equal to a
static inline uint32_t fwfs_npw2(uint32_t a) {
#if !defined(FWFS_NO_INTRINSICS) && (defined(__GNUC__) || defined(__CC_ARM))
    return 32 - __builtin_clz(a-1);
#else
    uint32_t r = 0;
    uint32_t s;
    a -= 1;
    s = (a > 0xffff) << 4; a >>= s; r |= s;
    s = (a > 0xff  ) << 3; a >>= s; r |= s;
    s = (a > 0xf   ) << 2; a >>= s; r |= s;
    s = (a > 0x3   ) << 1; a >>= s; r |= s;
    return (r | (a >> 1)) + 1;
#endif
}

// Count the number of trailing binary zeros in a
// fwfs_ctz(0) may be undefined
static inline uint32_t fwfs_ctz(uint32_t a) {
#if !defined(FWFS_NO_INTRINSICS) && defined(__GNUC__)
    return __builtin_ctz(a);
#else
    return fwfs_npw2((a & -a) + 1) - 1;
#endif
}

// Count the number of binary ones in a
static inline uint32_t fwfs_popc(uint32_t a) {
/*
 * Disable __builtin_popcount because of below link error:
 * lib/gcc/arm-linux-gnueabihf/8.2.1/libgcc.a(_popcountsi2.o) uses VFP register arguments, u-boot does not
 *
 * Beacuse _popcountsi2.o use -mfloat-abi=hard but uboot use -mfloat-abi=soft
 */
#if (0) && (!defined(FWFS_NO_INTRINSICS) && (defined(__GNUC__) || defined(__CC_ARM)))
    return __builtin_popcount(a);
#else
    a = a - ((a >> 1) & 0x55555555);
    a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
    return (((a + (a >> 4)) & 0xf0f0f0f) * 0x1010101) >> 24;
#endif
}

// Find the sequence comparison of a and b, this is the distance
// between a and b ignoring overflow
static inline int fwfs_scmp(uint32_t a, uint32_t b) {
    return (int)(unsigned)(a - b);
}

// Convert between 32-bit little-endian and native order
static inline uint32_t fwfs_fromle32(uint32_t a) {
#if !defined(FWFS_NO_INTRINSICS) && ( \
    (defined(  BYTE_ORDER  ) && defined(  ORDER_LITTLE_ENDIAN  ) &&   BYTE_ORDER   ==   ORDER_LITTLE_ENDIAN  ) || \
    (defined(__BYTE_ORDER  ) && defined(__ORDER_LITTLE_ENDIAN  ) && __BYTE_ORDER   == __ORDER_LITTLE_ENDIAN  ) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
    return a;
#elif !defined(FWFS_NO_INTRINSICS) && ( \
    (defined(  BYTE_ORDER  ) && defined(  ORDER_BIG_ENDIAN  ) &&   BYTE_ORDER   ==   ORDER_BIG_ENDIAN  ) || \
    (defined(__BYTE_ORDER  ) && defined(__ORDER_BIG_ENDIAN  ) && __BYTE_ORDER   == __ORDER_BIG_ENDIAN  ) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    return __builtin_bswap32(a);
#else
    return (((uint8_t*)&a)[0] <<  0) |
           (((uint8_t*)&a)[1] <<  8) |
           (((uint8_t*)&a)[2] << 16) |
           (((uint8_t*)&a)[3] << 24);
#endif
}

static inline uint32_t fwfs_tole32(uint32_t a) {
    return fwfs_fromle32(a);
}

// Convert between 32-bit big-endian and native order
static inline uint32_t fwfs_frombe32(uint32_t a) {
#if !defined(FWFS_NO_INTRINSICS) && ( \
    (defined(  BYTE_ORDER  ) && defined(  ORDER_LITTLE_ENDIAN  ) &&   BYTE_ORDER   ==   ORDER_LITTLE_ENDIAN  ) || \
    (defined(__BYTE_ORDER  ) && defined(__ORDER_LITTLE_ENDIAN  ) && __BYTE_ORDER   == __ORDER_LITTLE_ENDIAN  ) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
    return __builtin_bswap32(a);
#elif !defined(FWFS_NO_INTRINSICS) && ( \
    (defined(  BYTE_ORDER  ) && defined(  ORDER_BIG_ENDIAN  ) &&   BYTE_ORDER   ==   ORDER_BIG_ENDIAN  ) || \
    (defined(__BYTE_ORDER  ) && defined(__ORDER_BIG_ENDIAN  ) && __BYTE_ORDER   == __ORDER_BIG_ENDIAN  ) || \
    (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
    return a;
#else
    return (((uint8_t*)&a)[0] << 24) |
           (((uint8_t*)&a)[1] << 16) |
           (((uint8_t*)&a)[2] <<  8) |
           (((uint8_t*)&a)[3] <<  0);
#endif
}

static inline uint32_t fwfs_tobe32(uint32_t a) {
    return fwfs_frombe32(a);
}

// Calculate CRC-32 with polynomial = 0x04c11db7
uint32_t fwfs_crc(uint32_t crc, const void *buffer, size_t size);

// Allocate memory, only used if buffers are not provided to firmwarefs
// Note, memory must be 64-bit aligned
static inline void *fwfs_malloc(size_t size) {
#ifndef FWFS_NO_MALLOC
    return memalign(64, size);
#else
    (void)size;
    return NULL;
#endif
}

// Deallocate memory, only used if buffers are not provided to firmwarefs
static inline void fwfs_free(void *p) {
#ifndef FWFS_NO_MALLOC
    free(p);
#else
    (void)p;
#endif
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif
