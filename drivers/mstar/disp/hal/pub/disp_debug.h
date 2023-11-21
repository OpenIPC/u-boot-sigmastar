/*
* disp_debug.h- Sigmastar
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

#ifndef _DISP_DEBUG_H_
#define _DISP_DEBUG_H_

//-----------------------------------------------------------------------------------------------------
// Variable Prototype
//-----------------------------------------------------------------------------------------------------
#ifndef _DRV_DISP_IF_C_
extern u32 g_u32DispDbgLevel;
#endif

//-----------------------------------------------------------------------------------------------------
// Debug Level
//-----------------------------------------------------------------------------------------------------

#define DISP_DBG_LEVEL_NONE              0x00000000
#define DISP_DBG_LEVEL_DRV               0x00000001
#define DISP_DBG_LEVEL_HAL               0x00000002
#define DISP_DBG_LEVEL_IO                0x00000004
#define DISP_DBG_LEVEL_CTX               0x00000008
#define DISP_DBG_LEVEL_COLOR             0x00000010
#define DISP_DBG_LEVEL_IRQ               0x00000020
#define DISP_DBG_LEVEL_IRQ_TIMEZONE      0x00000040
#define DISP_DBG_LEVEL_IRQ_VGA_HPD       0x00000080
#define DISP_DBG_LEVEL_CLK               0x00000100
#define DISP_DBG_LEVEL_UTILITY_CMDQ      0x00000200
#define DISP_DBG_LEVEL_UTILITY_FLIP      0x00000400
#define DISP_DBG_LEVEL_IRQ_DMA           0x00000800
#define DISP_DBG_LEVEL_MCU               0x00001000
#define DISP_DBG_LEVEL_IRQ_FPLL          0x00002000
#define DISP_DBG_LEVEL_HAL_MOP_ORDER     0x00004000
#define DISP_DBG_LEVEL_IRQ_TIMEZONE_RUN  0x00008000
#define DISP_DBG_LEVEL_HPQ               0x00010000
#define DISP_DBG_LEVEL_HPQHL             0x00020000
#define DISP_DBG_LEVEL_MACE              0x00040000
#define DISP_DBG_LEVEL_HPQREG            0x00080000
#define DISP_DBG_LEVEL_UTILITY_FLIP_HWRD 0x00100000
#define DISP_DBG_LEVEL_UTILITY_DW_HWRD   0x00200000
#define DISP_DBG_LEVEL_HAL_DMA           0x00400000
#define DISP_DBG_LEVEL_HAL_MOP           0x00800000
#define DISP_DBG_LEVEL_DEBUG2WARNING     0x80000000
//-----------------------------------------------------------------------------------------------------
// Debug Macro
//-----------------------------------------------------------------------------------------------------
#ifdef DISP_DBG_DISABLE
#define DISP_DBG_EN 0
#define DISP_PRINTF(_fmt, _args...)
#else
#define DISP_DBG_EN 1
#define DISP_PRINTF(_fmt, _args...)       CamOsPrintf(_fmt, ##_args)
#endif
// for uboot using.
#ifndef KERN_WARNING
#define KERN_WARNING
#define PRINT_NONE
#define PRINT_RED
#define PRINT_YELLOW
#define PRINT_GREEN
#else
#define PRINT_NONE   "\33[m"
#define PRINT_RED    "\33[1;31m"
#define PRINT_YELLOW "\33[1;33m"
#define PRINT_GREEN  "\33[1;32m"

#endif
#ifndef KERN_DEBUG
#define KERN_DEBUG
#endif

#ifdef DISP_STATISTIC_DISABLE
#define DISP_STATISTIC_EN 0
#define DISP_DBG_STAT(dbglv, _fmt, _args...)
#define DISP_STATISTIC_VAL(val)
#else
#define DISP_STATISTIC_EN 1
#define DISP_STATISTIC_VAL(val) ((val))
#define DISP_DBG_STAT(dbglv, _fmt, _args...)                           \
    do{                                                            \
        if (g_u32DispDbgLevel & dbglv)                            \
        {                                                         \
            if (g_u32DispDbgLevel & DISP_DBG_LEVEL_DEBUG2WARNING) \
                DISP_PRINTF(KERN_WARNING _fmt, ##_args);          \
            else                                                  \
                DISP_PRINTF(KERN_DEBUG _fmt, ##_args);            \
        }                                                         \
    }while (0)
#endif

#if DISP_DBG_EN

#define DISP_ASSERT(_con)                                                        \
    do                                                                           \
    {                                                                            \
        if (!(_con))                                                             \
        {                                                                        \
            DISP_PRINTF("BUG at %s:%d assert(%s)\n", __FILE__, __LINE__, #_con); \
            BUG();                                                               \
        }                                                                        \
    } while (0)

#define DISP_DBG(dbglv, _fmt, _args...)                           \
    do{                                                            \
        if (g_u32DispDbgLevel & dbglv)                            \
        {                                                         \
            if (g_u32DispDbgLevel & DISP_DBG_LEVEL_DEBUG2WARNING) \
                DISP_PRINTF(KERN_WARNING _fmt, ##_args);          \
            else                                                  \
                DISP_PRINTF(KERN_DEBUG _fmt, ##_args);            \
        }                                                         \
    }while (0)

#define DISP_ERR(_fmt, _args...)                         \
    do                                                   \
    {                                                    \
        DISP_PRINTF(PRINT_RED _fmt PRINT_NONE, ##_args); \
    } while (0)

#define DISP_MSG(_fmt, _args...)                           \
    do                                                     \
    {                                                      \
        DISP_PRINTF(PRINT_GREEN _fmt PRINT_NONE, ##_args); \
    } while (0)

#else
#define DISP_ASSERT(arg)
#define DISP_DBG(dbglv, _fmt, _args...)
#define DISP_ERR(_fmt, _args...)\
    do                                                   \
    {                                                    \
        DISP_PRINTF(PRINT_RED _fmt PRINT_NONE, ##_args); \
    } while (0)

#define DISP_MSG(_fmt, _args...)\
    do                                                     \
    {                                                      \
        DISP_PRINTF(PRINT_GREEN _fmt PRINT_NONE, ##_args); \
    } while (0)
#endif
#if (DISP_DBG_EN && DISP_STATISTIC_EN)
#define DISP_DBG_VAL(val)

#else
#define DISP_DBG_VAL(val)   ((val)=(val))

#endif
#define DISP_DBG_CHECK(dbglv) (g_u32DispDbgLevel & dbglv)
//-----------------------------------------------------------------------------------------------------
// Parsing String
//-----------------------------------------------------------------------------------------------------
#define PARSING_CTX_TYPE(x)                         \
    (x == E_DISP_CTX_TYPE_DEVICE      ? "DEVICE"    \
     : x == E_DISP_CTX_TYPE_VIDLAYER  ? "VIDLAYER"  \
     : x == E_DISP_CTX_TYPE_INPUTPORT ? "INPUTPORT" \
     : x == E_DISP_CTX_TYPE_DMA       ? "DMA"       \
                                      : "UNKNOWN")

#define PARSING_HAL_QUERY_TYPE(x)                                                  \
    (x == E_HAL_DISP_QUERY_DEVICE_INIT                ? "DEVICE_INIT"              \
     : x == E_HAL_DISP_QUERY_DEVICE_DEINIT            ? "DEVICE_DEINIT"            \
     : x == E_HAL_DISP_QUERY_DEVICE_ENABLE            ? "DEVICE_ENABLE"            \
     : x == E_HAL_DISP_QUERY_DEVICE_ATTACH            ? "DEVICE_ATTACH"            \
     : x == E_HAL_DISP_QUERY_DEVICE_DETACH            ? "DEVICE_DETACH"            \
     : x == E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR  ? "DEVICE_BACKGROUND_COLOR"  \
     : x == E_HAL_DISP_QUERY_DEVICE_INTERFACE         ? "DEVICE_INTERFACE"         \
     : x == E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING      ? "DEVICE_OUTPUTTIMING"      \
     : x == E_HAL_DISP_QUERY_DEVICE_VGA_PARAM         ? "DEVICE_VGA_PARAM"         \
     : x == E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM        ? "DEVICE_CVBS_PARAM"        \
     : x == E_HAL_DISP_QUERY_DEVICE_COLORTEMP         ? "DEVICE_COLORTEMP"         \
     : x == E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM        ? "DEVICE_HDMI_PARAM"        \
     : x == E_HAL_DISP_QUERY_DEVICE_LCD_PARAM         ? "DEVICE_LCD_PARAM"         \
     : x == E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM       ? "DEVICE_GAMMA_PARAM"       \
     : x == E_HAL_DISP_QUERY_DEVICE_TIME_ZONE         ? "DEVICE_TIME_ZONE"         \
     : x == E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG     ? "DEVICE_TIME_ZONE_CFG"     \
     : x == E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO      ? "DEVICE_DISPLAY_INFO"      \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_INIT          ? "VIDEOLAYER_INIT"          \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE        ? "VIDEOLAYER_ENABLE"        \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_BIND          ? "VIDEOLAYER_BIND"          \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND        ? "VIDEOLAYER_UNBIND"        \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_ATTR          ? "VIDEOLAYER_ATTR"          \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS      ? "VIDEOLAYER_COMPRESS"      \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY      ? "VIDEOLAYER_PRIORITY"      \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE   ? "VIDEOLAYER_BUFFER_FIRE"   \
     : x == E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE    ? "VIDEOLAYER_CHECK_FIRE"    \
     : x == E_HAL_DISP_QUERY_INPUTPORT_INIT           ? "INPUTPORT_INIT"           \
     : x == E_HAL_DISP_QUERY_INPUTPORT_ENABLE         ? "INPUTPORT_ENABLE"         \
     : x == E_HAL_DISP_QUERY_INPUTPORT_ATTR           ? "INPUTPORT_ATTR"           \
     : x == E_HAL_DISP_QUERY_INPUTPORT_SHOW           ? "INPUTPORT_SHOW"           \
     : x == E_HAL_DISP_QUERY_INPUTPORT_HIDE           ? "INPUTPORT_HIDEN"          \
     : x == E_HAL_DISP_QUERY_INPUTPORT_BEGIN          ? "INPUTPORT_BEGIN"          \
     : x == E_HAL_DISP_QUERY_INPUTPORT_END            ? "INPUTPORT_END"            \
     : x == E_HAL_DISP_QUERY_INPUTPORT_FLIP           ? "INPUTPORT_FLIP"           \
     : x == E_HAL_DISP_QUERY_INPUTPORT_ROTATE         ? "INPUTPORT_ROTATE"         \
     : x == E_HAL_DISP_QUERY_INPUTPORT_CROP           ? "INPUTPORT_CROP"           \
     : x == E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR ? "INPUTPORT_RING_BUFF_ATTR" \
     : x == E_HAL_DISP_QUERY_INPUTPORT_IMIADDR        ? "INPUTPORT_IMIADDR"        \
     : x == E_HAL_DISP_QUERY_CLK_SET                  ? "CLK_SET"                  \
     : x == E_HAL_DISP_QUERY_CLK_GET                  ? "CLK_GET"                  \
     : x == E_HAL_DISP_QUERY_PQ_SET                   ? "PQ_SET"                   \
     : x == E_HAL_DISP_QUERY_PQ_GET                   ? "PQ_GET"                   \
     : x == E_HAL_DISP_QUERY_DRVTURNING_SET           ? "DRVTURNING_SET"           \
     : x == E_HAL_DISP_QUERY_DRVTURNING_GET           ? "DRVTURNING_GET"           \
     : x == E_HAL_DISP_QUERY_DBGMG_GET                ? "DBGMG_GET"                \
     : x == E_HAL_DISP_QUERY_REG_ACCESS               ? "REG_ACESS"                \
     : x == E_HAL_DISP_QUERY_REG_FLIP                 ? "REG_FLIP"                 \
     : x == E_HAL_DISP_QUERY_REG_WAITDONE             ? "REG_WAITDONE"             \
     : x == E_HAL_DISP_QUERY_DMA_INIT                 ? "DMA_INIT"                 \
     : x == E_HAL_DISP_QUERY_DMA_DEINIT               ? "DMA_DEINIT"               \
     : x == E_HAL_DISP_QUERY_DMA_BIND                 ? "DMA_BIND"                 \
     : x == E_HAL_DISP_QUERY_DMA_UNBIND               ? "DMA_UNBIND"               \
     : x == E_HAL_DISP_QUERY_DMA_ATTR                 ? "DMA_ATTR"                 \
     : x == E_HAL_DISP_QUERY_DMA_BUFFERATTR           ? "DMA_BUFFERATTR"           \
     : x == E_HAL_DISP_QUERY_HW_INFO                  ? "HW_INFO"                  \
     : x == E_HAL_DISP_QUERY_CLK_INIT                 ? "CLK_INIT"                 \
     : x == E_HAL_DISP_QUERY_CMDQINF                  ? "CMDQINF"                  \
     : x == E_HAL_DISP_QUERY_INTERCFG_SET             ? "INTERCFG_SET"             \
     : x == E_HAL_DISP_QUERY_INTERCFG_GET             ? "INTERCFG_GET"             \
     : x == E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET     ? "VGA_PARAM_GET"            \
     : x == E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET    ? "HDMI_PARAM_GET"           \
     : x == E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET     ? "LCD_PARAM_GET"            \
     : x == E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET    ? "DEVICE_CAPABILITY"        \
     : x == E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET  ? "VIDLAYER_CAPABILITY"      \
     : x == E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET ? "INTERFACE_CAPABILITY"     \
     : x == E_HAL_DISP_QUERY_DMA_CAPABILITY_GET       ? "DMA_CAPABILITY"           \
                                                      : "UNKNOWN")

#define PARSING_HAL_QUERY_RET(x)                                \
    (x == E_HAL_DISP_QUERY_RET_OK           ? "RET_OK"          \
     : x == E_HAL_DISP_QUERY_RET_CFGERR     ? "RET_CFGERR"      \
     : x == E_HAL_DISP_QUERY_RET_NONEED     ? "RET_NO_NEED"     \
     : x == E_HAL_DISP_QUERY_RET_NOTSUPPORT ? "RET_NOT_SUPPORT" \
                                            : "UNKNOWN")

#define PARSING_HAL_PIXEL_FMT(x)                                             \
    (x == E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV          ? "YUV422_YUYV"        \
     : x == E_HAL_DISP_PIXEL_FRAME_ARGB8888           ? "ARGB8888"           \
     : x == E_HAL_DISP_PIXEL_FRAME_ABGR8888           ? "ABGR8888"           \
     : x == E_HAL_DISP_PIXEL_FRAME_BGRA8888           ? "BGRA8888"           \
     : x == E_HAL_DISP_PIXEL_FRAME_RGB565             ? "RGB565"             \
     : x == E_HAL_DISP_PIXEL_FRAME_ARGB1555           ? "ARGB1555"           \
     : x == E_HAL_DISP_PIXEL_FRAME_ARGB4444           ? "ARGB4444"           \
     : x == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422 ? "YUV_SEMIPLANAR_422" \
     : x == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_420 ? "YUV_SEMIPLANAR_420" \
     : x == E_HAL_DISP_PIXEL_FRAME_YUV_MST_420        ? "YUV_MST_420"        \
                                                      : "UNKNOWN")

#define PARSING_HAL_DMA_PIXEL_FMT(x)                    \
    (x == E_HAL_DISP_DMA_PIX_FMT_YUYV        ? "YUYV"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_YVYU      ? "YVYU"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_UYVY      ? "UYVY"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_VYUY      ? "VYUY"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_SP420     ? "SP420"  \
     : x == E_HAL_DISP_DMA_PIX_FMT_SP420NV21 ? "NV21"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_SP422     ? "SP422"  \
     : x == E_HAL_DISP_DMA_PIX_FMT_P422      ? "P422"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_P420      ? "P420"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_ARGB      ? "ARGB"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_ABGR      ? "ABGR"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_RGBA      ? "RGBA"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_BGRA      ? "BGRA"   \
     : x == E_HAL_DISP_DMA_PIX_FMT_RGB565    ? "RGB565" \
                                             : "UNKNOWN")

#define PARSING_FPLL_STM(x)                                     \
    (x == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_DISABLE   ? "DISABLE" \
     : x == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE    ? "FREE"    \
     : x == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING ? "LOCKING" \
     : x == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED  ? "LOCKED"  \
     : x == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE ? "STATBLE" \
                                                    : "UNKNOWN")

#define PARSING_HAL_COMPRESS_MD(x)                   \
    (x == E_HAL_DISP_COMPRESS_MODE_NONE    ? "NONE"  \
     : x == E_HAL_DISP_COMPRESS_MODE_SEG   ? "SEG"   \
     : x == E_HAL_DISP_COMPRESS_MODE_LINE  ? "LINE"  \
     : x == E_HAL_DISP_COMPRESS_MODE_FRAME ? "FRAME" \
                                           : "UNKNOWN")

#define PARSING_HAL_TILE_MD(x)                   \
    (x == E_HAL_DISP_TILE_MODE_NONE    ? "NONE"  \
     : x == E_HAL_DISP_TILE_MODE_16X16 ? "16x16" \
     : x == E_HAL_DISP_TILE_MODE_16X32 ? "16x32" \
     : x == E_HAL_DISP_TILE_MODE_32X16 ? "32x16" \
     : x == E_HAL_DISP_TILE_MODE_32X32 ? "32x31" \
                                       : "UNKNOWN")

#define PARSING_HAL_CSC_MATRIX(x)                                     \
    (x == E_HAL_DISP_CSC_MATRIX_BYPASS            ? "BYPASS"          \
     : x == E_HAL_DISP_CSC_MATRIX_BT601_TO_BT709  ? "BT601_TO_BT709 " \
     : x == E_HAL_DISP_CSC_MATRIX_BT709_TO_BT601  ? "BT709_TO_BT601 " \
     : x == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ? "BT601_TO_RGB_PC" \
     : x == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC ? "BT709_TO_RGB_PC" \
     : x == E_HAL_DISP_CSC_MATRIX_USER            ? "USER" \
     : x == E_HAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC ? "RGB_TO_BT601_PC" \
     : x == E_HAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC ? "RGB_TO_BT709_PC" \
                                                  : "UNKNOWN")

#define PARSING_HAL_TMING_ID(x)                                \
    (x == E_HAL_DISP_OUTPUT_PAL               ? "PAL"          \
     : x == E_HAL_DISP_OUTPUT_NTSC            ? "NTSC"         \
     : x == E_HAL_DISP_OUTPUT_960H_PAL        ? "960H_PAL"     \
     : x == E_HAL_DISP_OUTPUT_960H_NTSC       ? "960H_NTSC"    \
     : x == E_HAL_DISP_OUTPUT_720P24          ? "720P24"       \
     : x == E_HAL_DISP_OUTPUT_720P25          ? "720P25"       \
     : x == E_HAL_DISP_OUTPUT_720P29D97  ? "720P29D97"    \
     : x == E_HAL_DISP_OUTPUT_720P30          ? "720P30"       \
     : x == E_HAL_DISP_OUTPUT_720P50          ? "720P50"       \
     : x == E_HAL_DISP_OUTPUT_720P59D94  ? "720P59D94"    \
     : x == E_HAL_DISP_OUTPUT_720P60          ? "720P60"       \
     : x == E_HAL_DISP_OUTPUT_1080P23D98      ? "1080P23D98"      \
     : x == E_HAL_DISP_OUTPUT_1080P24         ? "1080P24"      \
     : x == E_HAL_DISP_OUTPUT_1080P25         ? "1080P25"      \
     : x == E_HAL_DISP_OUTPUT_1080P29D97 ? "1080P29D97"   \
     : x == E_HAL_DISP_OUTPUT_1080P30         ? "1080P30"      \
     : x == E_HAL_DISP_OUTPUT_1080P50         ? "1080P50"      \
     : x == E_HAL_DISP_OUTPUT_1080P59D94 ? "1080P59D94"   \
     : x == E_HAL_DISP_OUTPUT_1080P60         ? "1080P60"      \
     : x == E_HAL_DISP_OUTPUT_1080I50         ? "1080I50"      \
     : x == E_HAL_DISP_OUTPUT_1080I60         ? "1080I60"      \
     : x == E_HAL_DISP_OUTPUT_2560X1440_30    ? "2560x1440_30" \
     : x == E_HAL_DISP_OUTPUT_2560X1440_50    ? "2560x1440_50" \
     : x == E_HAL_DISP_OUTPUT_2560X1440_60    ? "2560x1440_60" \
     : x == E_HAL_DISP_OUTPUT_3840X2160_24    ? "3840x2160_24" \
     : x == E_HAL_DISP_OUTPUT_3840X2160_25    ? "3840x2160_25" \
     : x == E_HAL_DISP_OUTPUT_3840X2160_29D97 ? "3840x2160_29D97" \
     : x == E_HAL_DISP_OUTPUT_3840X2160_30    ? "3840x2160_30" \
     : x == E_HAL_DISP_OUTPUT_3840X2160_60    ? "3840x2160_60" \
     : x == E_HAL_DISP_OUTPUT_640X480_60      ? "640x480_60"   \
     : x == E_HAL_DISP_OUTPUT_480P60          ? "480P60"       \
     : x == E_HAL_DISP_OUTPUT_576P50          ? "576P50"       \
     : x == E_HAL_DISP_OUTPUT_800X600_60      ? "800x600_60"   \
     : x == E_HAL_DISP_OUTPUT_848X480_60      ? "848x480_60"   \
     : x == E_HAL_DISP_OUTPUT_1024X768_60     ? "1024x768_60"  \
     : x == E_HAL_DISP_OUTPUT_1280X768_60     ? "1280x768_60"  \
     : x == E_HAL_DISP_OUTPUT_1280X800_60     ? "1280x800_60"  \
     : x == E_HAL_DISP_OUTPUT_1280X960_60     ? "1280x960_60"  \
     : x == E_HAL_DISP_OUTPUT_1280X1024_60    ? "1280x1024_60" \
     : x == E_HAL_DISP_OUTPUT_1360X768_60     ? "1360x768_60"  \
     : x == E_HAL_DISP_OUTPUT_1366X768_60     ? "1366x768_60"  \
     : x == E_HAL_DISP_OUTPUT_1400X1050_60    ? "1400x1050_60"  \
     : x == E_HAL_DISP_OUTPUT_1440X900_60     ? "1440x900_60"  \
     : x == E_HAL_DISP_OUTPUT_1600X900_60     ? "1600x900_60" \
     : x == E_HAL_DISP_OUTPUT_1600X1200_60    ? "1600x1200_60" \
     : x == E_HAL_DISP_OUTPUT_1680X1050_60    ? "1680x1050_60" \
     : x == E_HAL_DISP_OUTPUT_1920X1200_60    ? "1920x1200_60" \
     : x == E_HAL_DISP_OUTPUT_1920X1440_60    ? "1920x1440_60" \
     : x == E_HAL_DISP_OUTPUT_1920X2160_30    ? "1920x2160_30" \
     : x == E_HAL_DISP_OUTPUT_2560X1600_60    ? "2560x1600_60" \
     : x == E_HAL_DISP_OUTPUT_USER            ? "USER"         \
                                              : "UNKNOWN")

#define PARSING_HAL_INTERFACE(x)                                    \
    (x == HAL_DISP_INTF_HDMI                         ? "HDMI"       \
     : x == HAL_DISP_INTF_CVBS                       ? "CVBS"       \
     : x == HAL_DISP_INTF_VGA                        ? "VGA"        \
     : x == HAL_DISP_INTF_LCD                        ? "LCD"        \
     : x == HAL_DISP_INTF_TTL                        ? "TTL"        \
     : x == HAL_DISP_INTF_MIPIDSI                    ? "MIPIDSI"    \
     : x == HAL_DISP_INTF_BT656                      ? "BT656"      \
     : x == HAL_DISP_INTF_BT601                      ? "BT601"      \
     : x == HAL_DISP_INTF_BT1120                     ? "BT1120"     \
     : x == HAL_DISP_INTF_MCU                        ? "MCU"        \
     : x == HAL_DISP_INTF_MCU_NOFLM                  ? "MCU_NO_FLM" \
     : x == HAL_DISP_INTF_SRGB                       ? "SRGB"       \
     : x == (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA) ? "HDMI_VGA"   \
     : x == HAL_DISP_INTF_BT1120_DDR                 ? "BT1120_DDR" \
                                                     : "UNKNOWN")

#define PARSING_HAL_ROTATE(x)              \
    (x == E_HAL_DISP_ROTATE_NONE  ? "NONE" \
     : x == E_HAL_DISP_ROTATE_90  ? "90"   \
     : x == E_HAL_DISP_ROTATE_180 ? "180"  \
     : x == E_HAL_DISP_ROTATE_270 ? "270"  \
                                  : "UNKNOWN")

#define PARSING_HAL_TIMEZONE(x)                           \
    (x == E_HAL_DISP_TIMEZONE_SYNC         ? "SYNC"       \
     : x == E_HAL_DISP_TIMEZONE_BACKPORCH  ? "BACKPORCH"  \
     : x == E_HAL_DISP_TIMEZONE_ACTIVE     ? "ACTIVE"     \
     : x == E_HAL_DISP_TIMEZONE_FRONTPORCH ? "FRONTPORCH" \
                                           : "UNKNOWN")

#define PARSING_HAL_IRQ_IOCTL(x)                                           \
    (x == E_HAL_DISP_IRQ_IOCTL_ENABLE               ? "ENABLE"             \
     : x == E_HAL_DISP_IRQ_IOCTL_GET_FLAG           ? "GET_FLAG"           \
     : x == E_HAL_DISP_IRQ_IOCTL_CLEAR              ? "CLEAR"              \
     : x == E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED ? "TIMEZONE_SUPPORTED" \
     : x == E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID    ? "TIMEZONE_GET_ID"    \
     : x == E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE    ? "TIMEZONE_ENABLE"    \
     : x == E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG  ? "TIMEZONE_GET_FLAG"  \
     : x == E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR     ? "TIMEZONE_CLEAR"     \
     : x == E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED  ? "VGA_HPD_SUPPORTED"  \
     : x == E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID     ? "VGA_HPD_GET_ID"     \
     : x == E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE     ? "VGA_HPD_ENABLE"     \
     : x == E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_FLAG   ? "VGA_HPD_GET_FLAG"   \
     : x == E_HAL_DISP_IRQ_IOCTL_VGA_HPD_CLEAR      ? "VGA_HPD_CLEAR"      \
     : x == E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE         ? "DMA_ENABLE"         \
     : x == E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG       ? "DMA_GET_FLAG"       \
     : x == E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR          ? "DMA_CLEAR"          \
     : x == E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID         ? "DMA_GET_ID"         \
     : x == E_HAL_DISP_IRQ_IOCTL_LCD_ENABLE         ? "LCD_ENABLE"         \
     : x == E_HAL_DISP_IRQ_IOCTL_LCD_GET_FLAG       ? "LCD_GET_FLAG"       \
     : x == E_HAL_DISP_IRQ_IOCTL_LCD_CLEAR          ? "LCD_CLEAR"          \
     : x == E_HAL_DISP_IRQ_IOCTL_LCD_GET_ID         ? "LCD_GET_ID"         \
                                                    : "UNKNOWN")

#define PARSING_HAL_IRQ_TYPE(x)                                                 \
    (x == E_HAL_DISP_IRQ_TYPE_NONE                    ? "NONE"                  \
     : x == E_HAL_DISP_IRQ_TYPE_MOP_VSYNC             ? "MOP_VSYNC"             \
     : x == E_HAL_DISP_IRQ_TYPE_MOP_VDE               ? "MOP_VDE"               \
     : x == E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE ? "TIMEZONE_VDE_NEGATIVE" \
     : x == E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_UNLOCK  ? "TIMEZONE_FPLL_UNLOCK"  \
     : x == E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_LOCKED  ? "TIMEZONE_FPLL_LOCKED"  \
     : x & E_HAL_DISP_IRQ_TYPE_TIMEZONE               ? "TIMEZONE_TIME_ZONE"    \
     : x == E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON            ? "VGP_HPD_ON"            \
     : x == E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF           ? "VGP_HPD_OFF"           \
     : x == E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF        ? "VGA_HPD_ON_OFF"        \
     : x == E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON         ? "DMA_ACTIVE_ON"         \
     : x == E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF        ? "DMA_ACTIVE_OFF"        \
     : x == E_HAL_DISP_IRQ_TYPE_DMA_REALDONE          ? "DMA_DONE"              \
     : x == E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL          ? "DMA_FIFOFULL"          \
     : x == E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL        ? "DMA_REG_SET_FAIL"      \
     : x & E_HAL_DISP_IRQ_TYPE_DMA                    ? "DMA_INT"               \
                                                      : "UNKNOWN")

#define PARSING_HAL_LCD_IRQ_TYPE(x)                     \
    (x == E_HAL_DISP_IRQ_TYPE_LCD_NONE      ? "NONE"    \
     : x == E_HAL_DISP_IRQ_TYPE_LCD_FRM_END ? "FRM_END" \
     : x == E_HAL_DISP_IRQ_TYPE_LCD_IDX_RDY ? "IDX_RDY" \
     : x == E_HAL_DISP_IRQ_TYPE_LCD_CMD_RDY ? "CMD_RDY" \
     : x == E_HAL_DISP_IRQ_TYPE_LCD_FLM     ? "FLM"     \
                                            : "UNKNOWN")

#define PARSING_HAL_REG_ACCESS_TYPE(x) \
    (x == E_DISP_UTILITY_REG_ACCESS_CPU ? "CPU" : x == E_DISP_UTILITY_REG_ACCESS_CMDQ ? "CMDQ" : "UNKNOWN")

#define PARSING_HAL_DMA_INPUT_TYPE(x) \
    (x == E_HAL_DISP_DMA_INPUT_DEVICE_FRONT ? "FRONT" : x == E_HAL_DISP_DMA_INPUT_DEVICE_BACK ? "BACK" : "UNKNOWN")

#define PARSING_HAL_DMA_ACCESS_TYPE(x) \
    (x == E_HAL_DISP_DMA_ACCESS_TYPE_EMI ? "EMI" : x == E_HAL_DISP_DMA_ACCESS_TYPE_IMI ? "IMI" : "UNKNOWN")

#define PARSING_HAL_DMA_OUTPUT_MODE(x) \
    (x == E_HAL_DISP_DMA_OUTPUT_MODE_FRAME ? "FRAME" : x == E_HAL_DISP_DMA_OUTPUT_MODE_RING ? "RING" : "UNKNOWN")

#define PARSING_HAL_CLK_GP_CTRL_TYPE(x)                  \
    (x == E_HAL_DISP_CLK_GP_CTRL_HDMI       ? "HDMI"     \
     : x == E_HAL_DISP_CLK_GP_CTRL_LCD      ? "LCD"      \
     : x == E_HAL_DISP_CLK_GP_CTRL_DAC      ? "DAC"      \
     : x == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI  ? "MIPIDSI"  \
     : x == E_HAL_DISP_CLK_GP_CTRL_CVBS     ? "CVBS"     \
     : x == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC ? "HDMI_DAC" \
     : x == E_HAL_DISP_CLK_GP_CTRL_NONE     ? "NONE"     \
                                            : "UNKNOWN")
#define PARSING_HAL_PQ_FLAG_TYPE(x)                                   \
    (x == E_HAL_DISP_PQ_FLAG_LOAD_BIN                ? "LOAD BIN"     \
     : x == E_HAL_DISP_PQ_FLAG_FREE_BIN              ? "FREE BIN"     \
     : x == E_HAL_DISP_PQ_FLAG_BYPASS                ? "BYPASS"       \
     : x == E_HAL_DISP_PQ_FLAG_SET_SRC_ID            ? "SET SRC ID"   \
     : x == E_HAL_DISP_PQ_FLAG_PROCESS               ? "PROCESS"      \
     : x == E_HAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE ? "LOAD_SETTING" \
                                                     : "UNKNOWN")

#define PARSING_HAL_DEV_PQ_TYPE(x) \
    (x == E_HAL_DISP_DEV_PQ_MACE ? "MACE" : x == E_HAL_DISP_DEV_PQ_HPQ ? "HPQ" : "UNKNOW")
#endif // #ifndef
