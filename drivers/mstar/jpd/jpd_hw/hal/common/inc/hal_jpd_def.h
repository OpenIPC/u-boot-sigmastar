/*
 * hal_jpd_def.h- Sigmastar
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

#ifndef _HAL_JPD_DEF_H_
#define _HAL_JPD_DEF_H_

#include "mhal_common.h"
#include "cam_os_wrapper.h"
#include <stdlib.h>

#if defined(CHIP_M6)
#define MAX_JPD_DEVICE_NUM (1)
#elif defined(CHIP_I7)
#define MAX_JPD_DEVICE_NUM (4)
#elif defined(CHIP_M6P)
#define MAX_JPD_DEVICE_NUM (2)
#else
// do nothing
#endif

//-------------------------------------------------------------------------------------------------
//  Local Compiler Options
//-------------------------------------------------------------------------------------------------
#define JPEG_STATIC static

#define SUPPORT_HIGH_LOW_REVERSE FALSE // NJPD not support high-low reverse, just reserved the code in api layer

// NJPD IP supporting feature

#define ENABLE_TEST_09_NJPEGWriteProtectTest        FALSE
#define ENABLE_TEST_11_NJPEGScaleDownFunctionTest_2 FALSE
#define ENABLE_TEST_11_NJPEGScaleDownFunctionTest_4 FALSE
#define ENABLE_TEST_11_NJPEGScaleDownFunctionTest_8 FALSE
#define ENABLE_TEST_16_NJPEGEnablePsaveModeTest     FALSE
#define ENABLE_TEST_18_miu_sel_128M                 FALSE
#define ENABLE_TEST_18_miu_sel_64M                  FALSE
#define ENABLE_TEST_18_miu_sel_32M                  FALSE
#define ENABLE_TEST_22_AutoProtectFailTest          FALSE
#define ENABLE_TEST_NJPD_01_table_read_write_test   FALSE
#define ENABLE_TEST_NJPD_13_ROI_Test                FALSE
#define ENABLE_TEST_NJPD_17_Obuf_Output_Format      FALSE
#define ENABLE_TEST_NJPD_18_Ibuf_Burst_Length_Test  FALSE
#define ENABLE_TEST_NJPD_21_No_Reset_Table_Test     FALSE

#define ENABLE_TEST_NJPD_IMI_RING FALSE
#define ENABLE_TEST_NJPD_EMI_RING FALSE

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#define JPEG_BLOCK_TYPE MS_S16
//------------------------------------------------------------------------------
#define JPEG_QUANT_TYPE MS_S16
//------------------------------------------------------------------------------
// May need to be adjusted if support for other colorspaces/sampling factors is added
#define JPEG_MAXBLOCKSPERMCU 10
//------------------------------------------------------------------------------
#define JPEG_MAXHUFFTABLES 8
//------------------------------------------------------------------------------
#define JPEG_MAXQUANTTABLES 4
//------------------------------------------------------------------------------
#define JPEG_MAXCOMPONENTS 4
//------------------------------------------------------------------------------
#define JPEG_MAXCOMPSINSCAN 4
//------------------------------------------------------------------------------
// Increase this if you increase the max width!
#define JPEG_MAXBLOCKSPERROW 6144

#define STRING_NAME_LENGTH     (16)
#define JPD_MIN_ALLOC_CMA_SIZE (8192)

#define SCALE_DOWN_ALIGNMENT_W (8)
#define SCALE_DOWN_ALIGNMENT_H (2)
#define JPD_ALIGN_UP(x, y)     (((x) + (y)-1) & (~((y)-1)))

#define JPEG_memcpy(pDstAddr, pSrcAddr, u32Size) memcpy((pDstAddr), (pSrcAddr), (u32Size))
#define JPEG_memset(pDstAddr, u8value, u32Size)  memset((pDstAddr), (u8value), (u32Size))

#ifdef USE_PHYSICAL_ADDR
#define JPD_MIU_ADDR(addr) (addr - 0x20000000)
#define JPD_PHY_ADDR(addr) (addr - 0x20000000)
#else
#define JPD_MIU_ADDR(addr) (addr)
#define JPD_PHY_ADDR(addr) (addr)
#endif

#ifndef true
#define true 1
#define false 0
#endif

#define JPEG_AtoU32(pData, u32value)                             \
    do                                                           \
    {                                                            \
        u32value = 0;                                            \
        while (('0' <= *pData) && ('9' >= *pData))               \
        {                                                        \
            u32value = (10 * u32value) + (MS_U32)(*pData - '0'); \
            pData++;                                             \
        }                                                        \
    } while (0)

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#define NJPD_BIT(_bit_) (1 << (_bit_))
#define NJPD_BITMASK(_bits_) (NJPD_BIT(((1)?_bits_)+1)-NJPD_BIT(((0)?_bits_)))
#define NJPD_OFFSET(x) ((x) << 2)
#ifndef READ_BYTE
#define READ_WORD(_reg) (*(volatile MS_U16 *)(_reg))
#define WRITE_WORD(_reg, _val)                           \
    {                                                    \
        (*((volatile MS_U16 *)(_reg))) = (MS_U16)(_val); \
    }
#endif

#define NJPD_READ_WORD(addr) READ_WORD((addr))

#define NJPD_WRITE_WORD(id, addr, val) WRITE_WORD((g_njpdregisterBase[id] + (addr)), (val))
#define NJPD_WRITE_CMD(id, addr, val, buf, cnt)                                                                  \
    (*(MS_U64 *)(buf + (*cnt)++) = ((MS_U64)(((g_njpdregisterBase[id] & 0xFFFFFFFFL) + addr) & 0x00FFFFFF) >> 1) \
                                   | ((MS_U64)val & 0x000000000000FFFF) << 32)

#define MhalJpdRead2Byte(id, u32Reg) (NJPD_READ_WORD(g_njpdregisterBase[id] + ((u32Reg))))
#define MhalJpdWrite2Byte(id, u32Reg, u16Val) \
    do                                        \
    {                                         \
        MhalJpdWrite(id, ((u32Reg)), u16Val); \
    } while (0)

//-------------------------------------------------------------------------------------------------
//  Jpd Log Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    JPD_LOG_TAG_DEFAULT = 0x0,
    JPD_LOG_TAG_SYS,
    JPD_LOG_TAG_VDI,
    JPD_LOG_TAG_CTX,
    JPD_LOG_TAG_OS,
    JPD_LOG_TAG_DEV,
    JPD_LOG_TAG_OPS,
    JPD_LOG_TAG_PARSE,
    JPD_LOG_TAG_IOS,
    JPD_LOG_TAG_UT,
    JPD_LOG_TAG_MAX_NUM,
} E_JPD_TAG;

typedef enum
{
    JPD_LOG_LEVEL_ERROR = 0,
    JPD_LOG_LEVEL_WARNING,
    JPD_LOG_LEVEL_KMSG,
    JPD_LOG_LEVEL_INFO,
    JPD_LOG_LEVEL_KMSG_DEBUG,
    JPD_LOG_LEVEL_DEBUG,
    JPD_LOG_LEVEL_DEFAULT = JPD_LOG_LEVEL_WARNING,
} E_JPD_LOG_LEVEL;

typedef struct
{
    char *          pu8TagStr;
    E_JPD_TAG       eTagIdx;
    E_JPD_LOG_LEVEL eLogLevel;
} ST_JpdTagEntry;

#define JPD_CHECK_LOG_LEVEL(level) (1 >= (level))

#define JPD_LOGI(_fmt, _args...)                                            \
    do                                                                      \
    {                                                                       \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_INFO))                        \
        {                                                                   \
            printf("[jpd-i][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                   \
    } while (0)

#define JPD_LOGD(_fmt, _args...)                                            \
    do                                                                      \
    {                                                                       \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_DEBUG))                       \
        {                                                                   \
            printf("[jpd-d][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                   \
    } while (0)

#if defined(__I_SW__)
#define JPD_LOGD_KMSG(_fmt, _args...)
#else
#define JPD_LOGD_KMSG(_fmt, _args...)                                                  \
    do                                                                                 \
    {                                                                                  \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_KMSG_DEBUG))                             \
        {                                                                              \
            printf(KERN_DEBUG "[jpd-d][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                              \
    } while (0)
#endif
#define JPD_LOGE(_fmt, _args...)                                            \
    do                                                                      \
    {                                                                       \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_ERROR))                       \
        {                                                                   \
            printf("[jpd-e][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                   \
    } while (0)

#define JPD_LOGW(_fmt, _args...)                                            \
    do                                                                      \
    {                                                                       \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_WARNING))                     \
        {                                                                   \
            printf("[jpd-w][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                   \
    } while (0)

#define JPD_LOGKMSG(_fmt, _args...)                                                    \
    do                                                                                 \
    {                                                                                  \
        if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_KMSG))                                   \
        {                                                                              \
            printf("[jpd-w][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args);            \
        }                                                                              \
        else if (JPD_CHECK_LOG_LEVEL(JPD_LOG_LEVEL_WARNING))                           \
        {                                                                              \
            printf(KERN_DEBUG "[jpd-k][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                              \
    } while (0)

#define JPD_LOGOUT(_fmt, _args...)                                          \
    do                                                                      \
    {                                                                       \
        {                                                                   \
            printf("[jpd-m][%s:%d] "_fmt, __FUNCTION__, __LINE__, ##_args); \
        }                                                                   \
    } while (0)

#define LOGE_REUTRN(ret)          \
    do                            \
    {                             \
        JPD_LOGE("RETURN ERR\n"); \
        return ret;               \
    } while (0);

#define JPD_CHECK_VALID_POINTER(p)             \
    if (!p)                                    \
    {                                          \
        JPD_LOGE("Check pointer is NULL !\n"); \
        return E_MHAL_ERR_NULL_PTR;            \
    }

#define JPD_CHECK_OVER_BUFFER(addr, buf_addr, buf_size)                                     \
    do                                                                                      \
    {                                                                                       \
        if (((unsigned long)(addr) < (unsigned long)(buf_addr))                             \
            || ((unsigned long)(addr) >= ((unsigned long)(buf_addr) + (MS_U32)(buf_size)))) \
        {                                                                                   \
            JPD_LOGE("%s [%d] invalid address 0x%tx\n", (ptrdiff_t)(addr));                 \
            return FALSE;                                                                   \
        }                                                                                   \
    } while (0)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_NJPD00_NONE,
    E_NJPD01_TABLE_READ_WRITE,
    E_NJPD13_ROI,
    E_NJPD17_OBUF_OUTPUT_FORMAT_YC_SWAP,
    E_NJPD17_OBUF_OUTPUT_FORMAT_UV_SWAP,
    E_NJPD17_OBUF_OUTPUT_FORMAT_UV_7BIT,
    E_NJPD17_OBUF_OUTPUT_FORMAT_UV_MSB,
    E_NJPD18_IBUF_BURST_LENGTH,
    E_NJPD21_NO_RESET_TABLE,
    E_NJPD23_WRITE_PROTECT,
    E_NJPD25_DOWNSCALE_1_2,
    E_NJPD25_DOWNSCALE_1_4,
    E_NJPD25_DOWNSCALE_1_8,
    E_NJPD26_IMI_RING,
    E_NJPD27_EMI_RING

} NJPD_VERIFICATION_MODE;

//-----------------------------------------------------------------------------
// Description: JPEG scan type
typedef enum
{
    E_JPEG_GRAYSCALE = 0,
    E_JPEG_YH1V1     = 1 // 444
    ,
    E_JPEG_YH2V1 = 2 // 422
    ,
    E_JPEG_YH1V2 = 3 // 440
    ,
    E_JPEG_YH2V2 = 4 // 420
    ,
    E_JPEG_YH4V1 = 5 // 411
    //#if SW_JPD_RGB_CMYK
    ,
    E_JPEG_CMYK = 6,
    E_JPEG_RGB  = 7
    //#endif
} JPEG_SCAN_TYPE;

typedef enum
{
    E_NJPD_MIU_LAST_Z_PATCH,
    E_NJPD_EAGLE_SW_PATCH,
    E_NJPD_EIFFEL_REDUCE_CLK
} NJPD_PATCH_INDEX;

//-----------------------------------------------------------------------------
/// @brief \b Struct \b Name: JPEG_QUANT_TBL
/// @brief \b Struct \b Description: Structure for JPEG Quantization Table
//-----------------------------------------------------------------------------
typedef struct // same as JPD_QuanTbl in drvJPD.h
{
    JPEG_QUANT_TYPE s16Value[64]; ///< value of Q table
    MS_BOOL         bValid;       ///< has Q table or not.
} JPEG_QUANT_TBL;

//-----------------------------------------------------------------------------
/// @brief \b Struct \b Name: JPEG_HUFF_INFO
/// @brief \b Struct \b Description: Structure for JPEG huffman information
//-----------------------------------------------------------------------------
typedef struct
{
    MS_U8 u8HuffNum[17];  ///< number of Huffman codes per bit size
    MS_U8 u8HuffVal[256]; ///< Huffman codes per bit size

    MS_U8   u8Symbol[17]; ///< u8HuffNum in reverse order
    MS_U16  u16Code[17];  ///< Minimun code word
    MS_U8   u8Valid[17];  ///< Valid bit for NJPD
    MS_U8   u8SymbolCnt;
    MS_BOOL bValid; ///< has huffman table or not
} JPEG_HUFF_INFO;
//-----------------------------------------------------------------------------
/// @brief \b Struct \b Name: JPEG_HUFF_TBL
/// @brief \b Struct \b Description: Structure for JPEG huffman table
//-----------------------------------------------------------------------------
typedef struct
{
    MS_S16 s16Look_up[256]; ///< the lookup of huffman code
    // FIXME: Is 512 tree entries really enough to handle _all_ possible
    // code sets? I think so but not 100% positive.
    MS_S16 s16Tree[512];     ///< huffman tree of huffman code
    MS_U8  u8Code_size[256]; ///< code size of huffman code
} JPEG_HUFF_TBL;

#endif //_HAL_JPD_DEF_H_
