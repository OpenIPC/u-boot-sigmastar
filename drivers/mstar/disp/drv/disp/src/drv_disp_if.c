/*
* drv_disp_if.c- Sigmastar
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

#define _DRV_DISP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_if.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "drv_disp_irq.h"
#include "drv_disp_if.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct DrvDispIfTimingData_s
{
    u8                          bValid;
    HalDispDeviceTiming_e       enDevTimingType;
    HalDispDeviceTimingConfig_t stDevTiming;
} DrvDispIfTimingData_t;

typedef struct DrvDispIfTimingConfig_s
{
    u32                    u32DataSize;
    DrvDispIfTimingData_t *pstData;
} DrvDispIfTimingConfig_t;
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispIfTimingConfig_t g_stDispDevTimingCfg = {0, NULL};

u32 g_u32DispDbgLevel = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static HalDispDeviceTiming_e _DrvDispIfTransDevTimingTypeToHal(MHAL_DISP_DeviceTiming_e eMhalTiming, u16 *u16Fps, u16 *u16Fpsdot)
{
    HalDispDeviceTiming_e eHalTiming;

    switch(eMhalTiming)
    {
        case E_MHAL_DISP_OUTPUT_PAL:
            eHalTiming = E_HAL_DISP_OUTPUT_PAL;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_NTSC:
            eHalTiming = E_HAL_DISP_OUTPUT_NTSC;
            *u16Fps = 59;
            *u16Fpsdot = 94;
            break;
        case E_MHAL_DISP_OUTPUT_960H_PAL:
            eHalTiming = E_HAL_DISP_OUTPUT_960H_PAL;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_960H_NTSC:
            eHalTiming = E_HAL_DISP_OUTPUT_960H_NTSC;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_720P24:
            eHalTiming = E_HAL_DISP_OUTPUT_720P24;
            *u16Fps = 24;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_720P25:
            eHalTiming = E_HAL_DISP_OUTPUT_720P25;
            *u16Fps = 25;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x720_2997:
            eHalTiming = E_HAL_DISP_OUTPUT_720P29D97;
            *u16Fps = 29;
            *u16Fpsdot = 97;
            break;
        case E_MHAL_DISP_OUTPUT_720P30:
            eHalTiming = E_HAL_DISP_OUTPUT_720P30;
            *u16Fps = 30;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_720P50:
            eHalTiming = E_HAL_DISP_OUTPUT_720P50;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x720_5994:
            eHalTiming = E_HAL_DISP_OUTPUT_720P59D94;
            *u16Fps = 59;
            *u16Fpsdot = 94;
            break;
        case E_MHAL_DISP_OUTPUT_720P60:
            eHalTiming = E_HAL_DISP_OUTPUT_720P60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x1080_2398:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P23D98;
            *u16Fps = 23;
            *u16Fpsdot = 98;
            break;
        case E_MHAL_DISP_OUTPUT_1080P24:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P24;
            *u16Fps = 24;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1080P25:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P25;
            *u16Fps = 25;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x1080_2997:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P29D97;
            *u16Fps = 29;
            *u16Fpsdot = 97;
            break;
        case E_MHAL_DISP_OUTPUT_1080P30:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P30;
            *u16Fps = 30;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1080P50:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P50;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x1080_5994:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P59D94;
            *u16Fps = 59;
            *u16Fpsdot = 94;
            break;
        case E_MHAL_DISP_OUTPUT_1080P60:
            eHalTiming = E_HAL_DISP_OUTPUT_1080P60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1080I50:
            eHalTiming = E_HAL_DISP_OUTPUT_1080I50;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1080I60:
            eHalTiming = E_HAL_DISP_OUTPUT_1080I60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_2560x1440_30:
            eHalTiming = E_HAL_DISP_OUTPUT_2560X1440_30;
            *u16Fps = 30;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_2560x1440_50:
            eHalTiming = E_HAL_DISP_OUTPUT_2560X1440_50;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_2560x1440_60:
            eHalTiming = E_HAL_DISP_OUTPUT_2560X1440_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_3840x2160_24:
            eHalTiming = E_HAL_DISP_OUTPUT_3840X2160_24;
            *u16Fps = 24;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_3840x2160_25:
            eHalTiming = E_HAL_DISP_OUTPUT_3840X2160_25;
            *u16Fps = 25;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_3840x2160_2997:
            eHalTiming = E_HAL_DISP_OUTPUT_3840X2160_29D97;
            *u16Fps = 29;
            *u16Fpsdot = 97;
            break;
        case E_MHAL_DISP_OUTPUT_3840x2160_30:
            eHalTiming = E_HAL_DISP_OUTPUT_3840X2160_30;
            *u16Fps = 30;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_3840x2160_60:
            eHalTiming = E_HAL_DISP_OUTPUT_3840X2160_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_640x480_60:
            eHalTiming = E_HAL_DISP_OUTPUT_640X480_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_576P50:
            eHalTiming = E_HAL_DISP_OUTPUT_576P50;
            *u16Fps = 50;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_480P60:
            eHalTiming = E_HAL_DISP_OUTPUT_480P60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_800x600_60:
            eHalTiming = E_HAL_DISP_OUTPUT_800X600_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_848x480_60:
            eHalTiming = E_HAL_DISP_OUTPUT_848X480_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1024x768_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1024X768_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x768_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1280X768_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x800_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1280X800_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x960_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1280X960_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1280x1024_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1280X1024_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1360x768_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1360X768_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1366x768_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1366X768_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1400x1050_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1400X1050_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1440x900_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1440X900_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1600x900_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1600X900_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1600x1200_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1600X1200_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1680x1050_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1680X1050_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x1200_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1920X1200_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x1440_60:
            eHalTiming = E_HAL_DISP_OUTPUT_1920X1440_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_1920x2160_30:
            eHalTiming = E_HAL_DISP_OUTPUT_1920X2160_30;
            *u16Fps = 30;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_2560x1600_60:
            eHalTiming = E_HAL_DISP_OUTPUT_2560X1600_60;
            *u16Fps = 60;
            *u16Fpsdot = 0;
            break;
        case E_MHAL_DISP_OUTPUT_USER:
            eHalTiming = E_HAL_DISP_OUTPUT_USER;
            *u16Fps = 0;
            *u16Fpsdot = 0;
            break;
        default:
            eHalTiming = E_HAL_DISP_OUTPUT_MAX;
            *u16Fps = 0;
            *u16Fpsdot = 0;
            break;
    }

    return eHalTiming;
}

static HalDispCscmatrix_e _DrvDispIfTransCscMatrixToHal(MHAL_DISP_CscMattrix_e eMhalCscMatrixType)
{
    HalDispCscmatrix_e eHalCscMatrixType;

    eHalCscMatrixType =
        eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_BYPASS            ? E_HAL_DISP_CSC_MATRIX_BYPASS
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_BT601_TO_BT709  ? E_HAL_DISP_CSC_MATRIX_BT601_TO_BT709
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_BT709_TO_BT601  ? E_HAL_DISP_CSC_MATRIX_BT709_TO_BT601
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ? E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC ? E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_USER ? E_HAL_DISP_CSC_MATRIX_USER
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC ? E_HAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC
        : eMhalCscMatrixType == E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC ? E_HAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC
                                                                       : E_HAL_DISP_CSC_MATRIX_MAX;
    return eHalCscMatrixType;
}

static MHAL_DISP_CscMattrix_e _DrvDispIfTransCscMatrixToMhal(HalDispCscmatrix_e eHalCscMatrixType)
{
    MHAL_DISP_CscMattrix_e eMhalCscMatrixType;

    eMhalCscMatrixType =
        eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_BYPASS            ? E_MHAL_DISP_CSC_MATRIX_BYPASS
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_BT601_TO_BT709  ? E_MHAL_DISP_CSC_MATRIX_BT601_TO_BT709
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_BT709_TO_BT601  ? E_MHAL_DISP_CSC_MATRIX_BT709_TO_BT601
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ? E_MHAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC ? E_MHAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_USER ? E_MHAL_DISP_CSC_MATRIX_USER
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC ? E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC
        : eHalCscMatrixType == E_HAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC ? E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC
                                                                     : E_MHAL_DISP_CSC_MATRIX_MAX;
    return eMhalCscMatrixType;
}

static HalDispPixelCompressMode_e _DrvDispIfTransCompressToHal(MHalPixelCompressMode_e eMhalCompress)
{
    HalDispPixelCompressMode_e eHalCompress;

    eHalCompress = eMhalCompress == E_MHAL_COMPRESS_MODE_NONE      ? E_HAL_DISP_COMPRESS_MODE_NONE
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_SEG     ? E_HAL_DISP_COMPRESS_MODE_SEG
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_LINE    ? E_HAL_DISP_COMPRESS_MODE_LINE
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_FRAME   ? E_HAL_DISP_COMPRESS_MODE_FRAME
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_TO_8BIT ? E_HAL_DISP_COMPRESS_MODE_TO_8BIT
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_TO_6BIT ? E_HAL_DISP_COMPRESS_MODE_TO_6BIT
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_AFBC    ? E_HAL_DISP_COMPRESS_MODE_AFBC
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_SFBC0   ? E_HAL_DISP_COMPRESS_MODE_SFBC0
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_SFBC1   ? E_HAL_DISP_COMPRESS_MODE_SFBC1
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_SFBC2   ? E_HAL_DISP_COMPRESS_MODE_SFBC2
                   : eMhalCompress == E_MHAL_COMPRESS_MODE_BUTT    ? E_HAL_DISP_COMPRESS_MODE_BUTT
                                                                   : E_HAL_DISP_COMPRESS_MODE_MAX;
    return eHalCompress;
}

static HalDispTileMode_e _DrvDispIfTransTileModeToHal(MHAL_DISP_TileMode_e eMhalTile)
{
    HalDispTileMode_e eHalTile;

    eHalTile = eMhalTile == E_MHAL_DISP_TILE_MODE_16x16   ? E_HAL_DISP_TILE_MODE_16X16
               : eMhalTile == E_MHAL_DISP_TILE_MODE_16x32 ? E_HAL_DISP_TILE_MODE_16X32
               : eMhalTile == E_MHAL_DISP_TILE_MODE_32x16 ? E_HAL_DISP_TILE_MODE_32X16
               : eMhalTile == E_MHAL_DISP_TILE_MODE_32x32 ? E_HAL_DISP_TILE_MODE_32X32
                                                          : E_HAL_DISP_TILE_MODE_NONE;
    return eHalTile;
}

static HalDispPixelFormat_e _DrvDispIfTransPixelFormatToHal(MHalPixelFormat_e eMhalPixelFormat)
{
    HalDispPixelFormat_e eHalPixelFormat;

    eHalPixelFormat =
        eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_YUYV          ? E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ARGB8888           ? E_HAL_DISP_PIXEL_FRAME_ARGB8888
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ABGR8888           ? E_HAL_DISP_PIXEL_FRAME_ABGR8888
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_BGRA8888           ? E_HAL_DISP_PIXEL_FRAME_BGRA8888
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_RGB565             ? E_HAL_DISP_PIXEL_FRAME_RGB565
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ARGB1555           ? E_HAL_DISP_PIXEL_FRAME_ARGB1555
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ARGB4444           ? E_HAL_DISP_PIXEL_FRAME_ARGB4444
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_422 ? E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420 ? E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_420
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_MST_420        ? E_HAL_DISP_PIXEL_FRAME_YUV_MST_420
                                                                    : E_HAL_DISP_PIXEL_FRAME_FORMAT_MAX;
    return eHalPixelFormat;
}

static HalDispDmaPixelFormat_e _DrvDispIfTransDmaPixelFmtToHal(MHalPixelFormat_e eMhalPixelFormat)
{
    HalDispDmaPixelFormat_e eHalPixelFormat;

    eHalPixelFormat =
        eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_YUYV                 ? E_HAL_DISP_DMA_PIX_FMT_YUYV
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_422        ? E_HAL_DISP_DMA_PIX_FMT_SP422
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420        ? E_HAL_DISP_DMA_PIX_FMT_SP420
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV420_PLANAR             ? E_HAL_DISP_DMA_PIX_FMT_P420
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_PLANAR             ? E_HAL_DISP_DMA_PIX_FMT_P422
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_MST_420               ? E_HAL_DISP_DMA_PIX_FMT_SP420
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ABGR8888                  ? E_HAL_DISP_DMA_PIX_FMT_RGBA
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_BGRA8888                  ? E_HAL_DISP_DMA_PIX_FMT_ARGB
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_ARGB8888                  ? E_HAL_DISP_DMA_PIX_FMT_BGRA
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_YVYU               ? E_HAL_DISP_DMA_PIX_FMT_YVYU
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_UYVY               ? E_HAL_DISP_DMA_PIX_FMT_UYVY
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV422_VYUY               ? E_HAL_DISP_DMA_PIX_FMT_VYUY
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420_NV21   ? E_HAL_DISP_DMA_PIX_FMT_SP420NV21
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420_SSTILE ? E_HAL_DISP_DMA_PIX_FMT_SP420TILE
        : eMhalPixelFormat == E_MHAL_PIXEL_FRAME_RGB565                    ? E_HAL_DISP_DMA_PIX_FMT_RGB565
                                                                           : E_HAL_DISP_DMA_PIX_FMT_NUM;
    return eHalPixelFormat;
}

static HalDispDeviceTimingConfig_t *_DrvDispIfGetTimingFromTbl(HalDispDeviceTiming_e enHalTiminId)
{
    u32                          i;
    HalDispDeviceTimingConfig_t *pstTimingCfg  = NULL;
    DrvDispIfTimingData_t *      pstTimingData = NULL;

    for (i = 0; i < g_stDispDevTimingCfg.u32DataSize; i++)
    {
        pstTimingData = &g_stDispDevTimingCfg.pstData[i];
        if (pstTimingData->bValid && pstTimingData->enDevTimingType == enHalTiminId)
        {
            pstTimingCfg = &pstTimingData->stDevTiming;
            break;
        }
    }

    return pstTimingCfg;
}

static u8 _DrvDispIfTransDeviceOutpuTimingInfoToHal(MHAL_DISP_DeviceTimingInfo_t *pstMhalCfg,
                                                    HalDispDeviceTimingInfo_t *   pstHalCfg)
{
    u8                           bRet               = 1;
    HalDispDeviceTimingConfig_t *pstDeviceTimingCfg = NULL;
    u32                          u32Tmp;
    u16 u16FpsDot;
    u16 u16Fps;

    pstHalCfg->eTimeType = _DrvDispIfTransDevTimingTypeToHal(pstMhalCfg->eTimeType, &u16Fps, &u16FpsDot);

    if (pstHalCfg->eTimeType == E_HAL_DISP_OUTPUT_USER && pstMhalCfg->pstSyncInfo)
    {
        memset(&pstHalCfg->stDeviceTimingCfg, 0, sizeof(HalDispDeviceTimingConfig_t));
        pstHalCfg->stDeviceTimingCfg.u16HsyncWidth     = pstMhalCfg->pstSyncInfo->u16Hpw;
        pstHalCfg->stDeviceTimingCfg.u16HsyncBackPorch = pstMhalCfg->pstSyncInfo->u16Hbb;
        pstHalCfg->stDeviceTimingCfg.u16Hactive        = pstMhalCfg->pstSyncInfo->u16Hact;
        pstHalCfg->stDeviceTimingCfg.u16Hstart         = pstMhalCfg->pstSyncInfo->u16HStart;
        pstHalCfg->stDeviceTimingCfg.u16Htotal = pstMhalCfg->pstSyncInfo->u16Hpw + pstMhalCfg->pstSyncInfo->u16Hbb
                                                 + pstMhalCfg->pstSyncInfo->u16Hact
                                                 + pstMhalCfg->pstSyncInfo->u16Hfb;
        pstHalCfg->stDeviceTimingCfg.u16VsyncWidth     = pstMhalCfg->pstSyncInfo->u16Vpw;
        pstHalCfg->stDeviceTimingCfg.u16VsyncBackPorch = pstMhalCfg->pstSyncInfo->u16Vbb;
        pstHalCfg->stDeviceTimingCfg.u16Vactive        = pstMhalCfg->pstSyncInfo->u16Vact;
        pstHalCfg->stDeviceTimingCfg.u16Vstart         = pstMhalCfg->pstSyncInfo->u16VStart;
        pstHalCfg->stDeviceTimingCfg.u16Vtotal = pstMhalCfg->pstSyncInfo->u16Vpw + pstMhalCfg->pstSyncInfo->u16Vbb
                                                 + pstMhalCfg->pstSyncInfo->u16Vact
                                                 + pstMhalCfg->pstSyncInfo->u16Vfb;
        pstHalCfg->stDeviceTimingCfg.u16Fps = pstMhalCfg->pstSyncInfo->u32FrameRate;
        if(pstMhalCfg->pstSyncInfo->u32FrameRateDot)
        {
            pstHalCfg->stDeviceTimingCfg.u16Fps++;
        }
        if(pstMhalCfg->pstSyncInfo->u32Dclk)
        {
            pstHalCfg->stDeviceTimingCfg.u32Dclk = pstMhalCfg->pstSyncInfo->u32Dclk;
        }
        else
        {
        pstHalCfg->stDeviceTimingCfg.u32Dclk = 
            pstHalCfg->stDeviceTimingCfg.u16Vtotal * pstHalCfg->stDeviceTimingCfg.u16Htotal * pstHalCfg->stDeviceTimingCfg.u16Fps;
            if(pstMhalCfg->pstSyncInfo->u32FrameRateDot)
            {
                pstHalCfg->stDeviceTimingCfg.u32Dclk += 
                    (pstHalCfg->stDeviceTimingCfg.u16Vtotal *
                    pstHalCfg->stDeviceTimingCfg.u16Htotal *
                    pstMhalCfg->pstSyncInfo->u32FrameRateDot / 100);
            }
        }
        u32Tmp = 1000000UL / (u32)pstHalCfg->stDeviceTimingCfg.u16Fps;
        pstHalCfg->stDeviceTimingCfg.u32VSyncPeriod =
            u32Tmp * (u32)pstMhalCfg->pstSyncInfo->u16Vpw / (u32)pstHalCfg->stDeviceTimingCfg.u16Vtotal;
        pstHalCfg->stDeviceTimingCfg.u32VBackPorchPeriod =
            u32Tmp * (u32)pstMhalCfg->pstSyncInfo->u16Vbb / (u32)pstHalCfg->stDeviceTimingCfg.u16Vtotal;
        pstHalCfg->stDeviceTimingCfg.u32VActivePeriod =
            u32Tmp * (u32)pstMhalCfg->pstSyncInfo->u16Vact / (u32)pstHalCfg->stDeviceTimingCfg.u16Vtotal;
        pstHalCfg->stDeviceTimingCfg.u32VFrontPorchPeriod =
            u32Tmp * (u32)pstMhalCfg->pstSyncInfo->u16Vfb / (u32)pstHalCfg->stDeviceTimingCfg.u16Vtotal;
    }
    else
    {
        pstDeviceTimingCfg = _DrvDispIfGetTimingFromTbl(pstHalCfg->eTimeType);
        if (pstDeviceTimingCfg)
        {
            memcpy(&pstHalCfg->stDeviceTimingCfg, pstDeviceTimingCfg, sizeof(HalDispDeviceTimingConfig_t));
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Can't find Timing(%s) in Table\n", __FUNCTION__, __LINE__,
                     PARSING_HAL_TMING_ID(pstHalCfg->eTimeType));
        }
    }
    return bRet;
}

static void _DrvDispIfTransDeviceCvbsParamToHal(MHAL_DISP_CvbsParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstHalCfg->bEnable = pstMhalCfg->bEnable;
    pstHalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToHal(pstMhalCfg->stCsc.eCscMatrix);
    pstHalCfg->stCsc.u32Luma       = pstMhalCfg->stCsc.u32Luma;
    pstHalCfg->stCsc.u32Hue        = pstMhalCfg->stCsc.u32Hue;
    pstHalCfg->stCsc.u32Contrast   = pstMhalCfg->stCsc.u32Contrast;
    pstHalCfg->stCsc.u32Saturation = pstMhalCfg->stCsc.u32Saturation;
    pstHalCfg->u32Sharpness        = pstMhalCfg->u32Sharpness;
    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, En:%d ,Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat:%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             pstHalCfg->bEnable, PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceVgaParamToHal(MHAL_DISP_VgaParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstHalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToHal(pstMhalCfg->stCsc.eCscMatrix);
    pstHalCfg->stCsc.u32Luma       = pstMhalCfg->stCsc.u32Luma;
    pstHalCfg->stCsc.u32Hue        = pstMhalCfg->stCsc.u32Hue;
    pstHalCfg->stCsc.u32Contrast   = pstMhalCfg->stCsc.u32Contrast;
    pstHalCfg->stCsc.u32Saturation = pstMhalCfg->stCsc.u32Saturation;

    pstHalCfg->u32Gain      = pstMhalCfg->u32Gain;
    pstHalCfg->u32Sharpness = pstMhalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat%d, Gain:%d Sharp:%d\n", __FUNCTION__,
             __LINE__, PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma,
             pstHalCfg->stCsc.u32Hue, pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Gain,
             pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceVgaParamToMhal(MHAL_DISP_VgaParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstMhalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToMhal(pstHalCfg->stCsc.eCscMatrix);
    pstMhalCfg->stCsc.u32Luma       = pstHalCfg->stCsc.u32Luma;
    pstMhalCfg->stCsc.u32Hue        = pstHalCfg->stCsc.u32Hue;
    pstMhalCfg->stCsc.u32Contrast   = pstHalCfg->stCsc.u32Contrast;
    pstMhalCfg->stCsc.u32Saturation = pstHalCfg->stCsc.u32Saturation;

    pstMhalCfg->u32Gain      = pstHalCfg->u32Gain;
    pstMhalCfg->u32Sharpness = pstHalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat%d, Gain:%d Sharp:%d\n", __FUNCTION__,
             __LINE__, PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma,
             pstHalCfg->stCsc.u32Hue, pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Gain,
             pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceHdmiParamToHal(MHAL_DISP_HdmiParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstHalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToHal(pstMhalCfg->stCsc.eCscMatrix);
    pstHalCfg->stCsc.u32Luma       = pstMhalCfg->stCsc.u32Luma;
    pstHalCfg->stCsc.u32Hue        = pstMhalCfg->stCsc.u32Hue;
    pstHalCfg->stCsc.u32Contrast   = pstMhalCfg->stCsc.u32Contrast;
    pstHalCfg->stCsc.u32Saturation = pstMhalCfg->stCsc.u32Saturation;
    pstHalCfg->u32Sharpness        = pstMhalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat:%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceHdmiParamToMhal(MHAL_DISP_HdmiParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstMhalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToMhal(pstHalCfg->stCsc.eCscMatrix);
    pstMhalCfg->stCsc.u32Luma       = pstHalCfg->stCsc.u32Luma;
    pstMhalCfg->stCsc.u32Hue        = pstHalCfg->stCsc.u32Hue;
    pstMhalCfg->stCsc.u32Contrast   = pstHalCfg->stCsc.u32Contrast;
    pstMhalCfg->stCsc.u32Saturation = pstHalCfg->stCsc.u32Saturation;
    pstMhalCfg->u32Sharpness        = pstHalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat:%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}
static void _DrvDispIfTransDeviceCvbsParamToMhal(MHAL_DISP_CvbsParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstMhalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToMhal(pstHalCfg->stCsc.eCscMatrix);
    pstMhalCfg->stCsc.u32Luma       = pstHalCfg->stCsc.u32Luma;
    pstMhalCfg->stCsc.u32Hue        = pstHalCfg->stCsc.u32Hue;
    pstMhalCfg->stCsc.u32Contrast   = pstHalCfg->stCsc.u32Contrast;
    pstMhalCfg->stCsc.u32Saturation = pstHalCfg->stCsc.u32Saturation;
    pstMhalCfg->u32Sharpness        = pstHalCfg->u32Sharpness;
    pstMhalCfg->bEnable             = pstHalCfg->bEnable;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat:%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceLcdParamToHal(MHAL_DISP_LcdParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstHalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToHal(pstMhalCfg->stCsc.eCscMatrix);
    pstHalCfg->stCsc.u32Luma       = pstMhalCfg->stCsc.u32Luma;
    pstHalCfg->stCsc.u32Hue        = pstMhalCfg->stCsc.u32Hue;
    pstHalCfg->stCsc.u32Contrast   = pstMhalCfg->stCsc.u32Contrast;
    pstHalCfg->stCsc.u32Saturation = pstMhalCfg->stCsc.u32Saturation;

    pstHalCfg->u32Sharpness = pstMhalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransDeviceLcdParamToMhal(MHAL_DISP_LcdParam_t *pstMhalCfg, HalDispDeviceParam_t *pstHalCfg)
{
    pstMhalCfg->stCsc.eCscMatrix    = _DrvDispIfTransCscMatrixToMhal(pstHalCfg->stCsc.eCscMatrix);
    pstMhalCfg->stCsc.u32Luma       = pstHalCfg->stCsc.u32Luma;
    pstMhalCfg->stCsc.u32Hue        = pstHalCfg->stCsc.u32Hue;
    pstMhalCfg->stCsc.u32Contrast   = pstHalCfg->stCsc.u32Contrast;
    pstMhalCfg->stCsc.u32Saturation = pstHalCfg->stCsc.u32Saturation;

    pstMhalCfg->u32Sharpness = pstHalCfg->u32Sharpness;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Csc:%s, Luma:%d, Hue:%d, Con:%d, Sat%d, Sharp:%d\n", __FUNCTION__, __LINE__,
             PARSING_HAL_CSC_MATRIX(pstHalCfg->stCsc.eCscMatrix), pstHalCfg->stCsc.u32Luma, pstHalCfg->stCsc.u32Hue,
             pstHalCfg->stCsc.u32Contrast, pstHalCfg->stCsc.u32Saturation, pstHalCfg->u32Sharpness);
}

static void _DrvDispIfTransdeviceGammaParamToHal(MHAL_DISP_GammaParam_t *pstMhalCfg, HalDispGammaParam_t *pstHalCfg)
{
    pstHalCfg->bEn         = pstMhalCfg->bEn;
    pstHalCfg->u16EntryNum = pstMhalCfg->u16EntryNum;
    pstHalCfg->pu8ColorR   = pstMhalCfg->pu8ColorR;
    pstHalCfg->pu8ColorG   = pstMhalCfg->pu8ColorG;
    pstHalCfg->pu8ColorB   = pstMhalCfg->pu8ColorB;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, En:%d, EntryNum:%d\n", __FUNCTION__, __LINE__, pstHalCfg->bEn,
             pstHalCfg->u16EntryNum);
}

static void _DrvDispIfTransDeviceColorTempToHal(MHAL_DISP_ColorTempeture_t *pstMhalCfg, HalDispColorTemp_t *pstHalCfg)
{
    pstHalCfg->u16RedColor    = pstMhalCfg->u16RedColor;
    pstHalCfg->u16GreenColor  = pstMhalCfg->u16GreenColor;
    pstHalCfg->u16BlueColor   = pstMhalCfg->u16BlueColor;
    pstHalCfg->u16RedOffset   = pstMhalCfg->u16RedOffset;
    pstHalCfg->u16GreenOffset = pstMhalCfg->u16GreenOffset;
    pstHalCfg->u16BlueOffset  = pstMhalCfg->u16BlueOffset;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Color(%d %d %d), Offset(%d %d %d)\n", __FUNCTION__, __LINE__,
             pstHalCfg->u16RedColor, pstHalCfg->u16GreenColor, pstHalCfg->u16BlueColor, pstHalCfg->u16RedOffset,
             pstHalCfg->u16GreenOffset, pstHalCfg->u16BlueOffset);
}

static void _DrvDispIfTransVidLayerAttrToHal(MHAL_DISP_VideoLayerAttr_t *pstMhalCfg, HalDispVideoLayerAttr_t *pstHalCfg)
{
    pstHalCfg->stVidLayerDispWin.u16X      = pstMhalCfg->stVidLayerDispWin.u16X;
    pstHalCfg->stVidLayerDispWin.u16Y      = pstMhalCfg->stVidLayerDispWin.u16Y;
    pstHalCfg->stVidLayerDispWin.u16Width  = pstMhalCfg->stVidLayerDispWin.u16Width;
    pstHalCfg->stVidLayerDispWin.u16Height = pstMhalCfg->stVidLayerDispWin.u16Height;

    pstHalCfg->stVidLayerSize.u32Height = pstMhalCfg->stVidLayerSize.u32Height;
    pstHalCfg->stVidLayerSize.u32Width  = pstMhalCfg->stVidLayerSize.u32Width;

    pstHalCfg->ePixFormat = _DrvDispIfTransPixelFormatToHal(pstMhalCfg->ePixFormat);

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Pixel:%s, Disp(%d %d %d %d) Size(%d %d)\n", __FUNCTION__, __LINE__,
             PARSING_HAL_PIXEL_FMT(pstHalCfg->ePixFormat), pstHalCfg->stVidLayerDispWin.u16X,
             pstHalCfg->stVidLayerDispWin.u16Y, pstHalCfg->stVidLayerDispWin.u16Width,
             pstHalCfg->stVidLayerDispWin.u16Height, pstHalCfg->stVidLayerSize.u32Width,
             pstHalCfg->stVidLayerSize.u32Height);
}

static void _DrvDispIfTransDeviceTimeZoneToMhal(MHAL_DISP_TimeZone_t *pstMhalCfg, HalDispTimeZone_t *pstHalCfg)
{
    pstMhalCfg->enType = pstHalCfg->enType == E_HAL_DISP_TIMEZONE_SYNC         ? E_MHAL_DISP_TIMEZONE_SYNC
                         : pstHalCfg->enType == E_HAL_DISP_TIMEZONE_BACKPORCH  ? E_MHAL_DISP_TIMEZONE_BACKPORCH
                         : pstHalCfg->enType == E_HAL_DISP_TIMEZONE_ACTIVE     ? E_MHAL_DISP_TIMEZONE_ACTIVE
                         : pstHalCfg->enType == E_HAL_DISP_TIMEZONE_FRONTPORCH ? E_MHAL_DISP_TIMEZONE_FRONTPORCH
                                                                               : E_MHAL_DISP_TIMEZONE_UNKONWN;

    // DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, TimeZone=%s\n",
    //     __FUNCTION__, __LINE__,
    //     PARSING_HAL_TIMEZONE(pstHalCfg->enType));
}

static void _DrvDispIfTransDeviceTimeZoneCfgToHal(MHAL_DISP_TimeZoneConfig_t *pstMhalCfg,
                                                  HalDispTimeZoneConfig_t *   pstHalCfg)
{
    u8 i;

    for (i = 0; i < E_MHAL_DISP_TIMEZONE_NUM; i++)
    {
        pstHalCfg->u16InvalidArea[i] = pstMhalCfg->u16InvalidArea[i];
    }
}

static void _DrvDispIfTransDeviceDisplayInfoToMhal(MHAL_DISP_DisplayInfo_t *pstMhalCfg, HalDispDisplayInfo_t *pstHalCfg)
{
    pstMhalCfg->u16Htotal      = pstHalCfg->u16Htotal;
    pstMhalCfg->u16Vtotal      = pstHalCfg->u16Vtotal;
    pstMhalCfg->u16HdeStart    = pstHalCfg->u16HdeStart;
    pstMhalCfg->u16VdeStart    = pstHalCfg->u16VdeStart;
    pstMhalCfg->u16Width       = pstHalCfg->u16Width;
    pstMhalCfg->u16Height      = pstHalCfg->u16Height;
    pstMhalCfg->bInterlaceMode = pstHalCfg->bInterlace;
    pstMhalCfg->bYuvOutput     = pstHalCfg->bYuvOutput;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d: Htt:%d, Vtt:%d, Hst:%d Vst:%d, Interlace:%d, Yuv:%d\n", __FUNCTION__, __LINE__,
             pstMhalCfg->u16Htotal, pstMhalCfg->u16Vtotal, pstMhalCfg->u16HdeStart, pstMhalCfg->u16VdeStart,
             pstMhalCfg->bInterlaceMode, pstMhalCfg->bYuvOutput);
}

static void _DrvDispIfTransVidLayerCompressToHal(MHAL_DISP_CompressAttr_t *       pstMhalCfg,
                                                 HalDispVideoLayerCompressAttr_t *pstHalCfg)
{
    pstHalCfg->bEnbale = pstMhalCfg->bEnbale;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, bEn:%d\n", __FUNCTION__, __LINE__, pstHalCfg->bEnbale);
}

static void _DrvDispIfTransInputPortFrameDataToHal(MHAL_DISP_VideoFrameData_t *pstMhalCfg,
                                                   HalDispVideoFrameData_t *   pstHalCfg)
{
    pstHalCfg->ePixelFormat  = _DrvDispIfTransPixelFormatToHal(pstMhalCfg->ePixelFormat);
    pstHalCfg->eCompressMode = _DrvDispIfTransCompressToHal(pstMhalCfg->eCompressMode);
    pstHalCfg->eTileMode     = _DrvDispIfTransTileModeToHal(pstMhalCfg->eTileMode);
    pstHalCfg->au32Stride[0] = pstMhalCfg->au32Stride[0];
    pstHalCfg->au32Stride[1] = pstMhalCfg->au32Stride[1];
    pstHalCfg->au32Stride[2] = pstMhalCfg->au32Stride[2];

    pstHalCfg->au64PhyAddr[0] = pstMhalCfg->aPhyAddr[0];
    pstHalCfg->au64PhyAddr[1] = pstMhalCfg->aPhyAddr[1];
    pstHalCfg->au64PhyAddr[2] = pstMhalCfg->aPhyAddr[2];

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Pixel:%s, Compress:%s, Title:%s, (%llx, %llx, %llx), (%d %d %d)\n",
             __FUNCTION__, __LINE__, PARSING_HAL_PIXEL_FMT(pstHalCfg->ePixelFormat),
             PARSING_HAL_COMPRESS_MD(pstHalCfg->eCompressMode), PARSING_HAL_TILE_MD(pstHalCfg->eTileMode),
             pstHalCfg->au64PhyAddr[0], pstHalCfg->au64PhyAddr[1], pstHalCfg->au64PhyAddr[2], pstHalCfg->au32Stride[0],
             pstHalCfg->au32Stride[1], pstHalCfg->au32Stride[2]);
}

static void _DrvDispIfTransInputPortRingBufAttrToHal(MHAL_DISP_RingBufferAttr_t *pstMhalCfg,
                                                     HalDispRingBuffAttr_t *     pstHalCfg)
{
    pstHalCfg->bEn              = pstMhalCfg->bEn;
    pstHalCfg->u16BuffHeight    = pstMhalCfg->u16BuffHeight;
    pstHalCfg->u16BuffStartLine = pstMhalCfg->u16BuffStartLine;

    pstHalCfg->eAccessType = pstMhalCfg->eAccessType == E_MHAL_DISP_DMA_ACCESS_TYPE_EMI ? E_HAL_DISP_DMA_ACCESS_TYPE_EMI
                             : pstMhalCfg->eAccessType == E_MHAL_DISP_DMA_ACCESS_TYPE_IMI
                                 ? E_HAL_DISP_DMA_ACCESS_TYPE_IMI
                                 : E_HAL_DISP_DMA_ACCESS_TYPE_NUM;
    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, En:%d, BuffHeight:%d, StartLine:%d, Access:%s\n", __FUNCTION__, __LINE__,
             pstHalCfg->bEn, pstHalCfg->u16BuffHeight, pstHalCfg->u16BuffStartLine,
             PARSING_HAL_DMA_ACCESS_TYPE(pstHalCfg->eAccessType));
}

static void _DrvDispIfTransInputPortRotateToHal(MHAL_DISP_RotateConfig_t *pstMhalCfg,
                                                HalDispInputPortRotate_t *pstHalCfg)
{
    pstHalCfg->enRotate = pstMhalCfg->enRotate == E_MHAL_DISP_ROTATE_NONE  ? E_HAL_DISP_ROTATE_NONE
                          : pstMhalCfg->enRotate == E_MHAL_DISP_ROTATE_90  ? E_HAL_DISP_ROTATE_90
                          : pstMhalCfg->enRotate == E_MHAL_DISP_ROTATE_180 ? E_HAL_DISP_ROTATE_180
                          : pstMhalCfg->enRotate == E_MHAL_DISP_ROTATE_270 ? E_HAL_DISP_ROTATE_270
                                                                           : E_HAL_DISP_ROTATE_NUM;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Rotate:%s\n", __FUNCTION__, __LINE__, PARSING_HAL_ROTATE(pstHalCfg->enRotate));
}

static void _DrvDispIfTransInputPortAttrToHal(MHAL_DISP_InputPortAttr_t *pstMhalCfg, HalDispInputPortAttr_t *pstHalCfg)
{
    pstHalCfg->stDispWin.u16X      = pstMhalCfg->stDispWin.u16X;
    pstHalCfg->stDispWin.u16Y      = pstMhalCfg->stDispWin.u16Y;
    pstHalCfg->stDispWin.u16Width  = pstMhalCfg->stDispWin.u16Width;
    pstHalCfg->stDispWin.u16Height = pstMhalCfg->stDispWin.u16Height;
    pstHalCfg->u16SrcHeight        = pstMhalCfg->u16SrcHeight;
    pstHalCfg->u16SrcWidth         = pstMhalCfg->u16SrcWidth;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Rect(%d %d %d %d) Src(%d %d)\n", __FUNCTION__, __LINE__,
             pstHalCfg->stDispWin.u16X, pstHalCfg->stDispWin.u16Y, pstHalCfg->stDispWin.u16Width,
             pstHalCfg->stDispWin.u16Height, pstHalCfg->u16SrcWidth, pstHalCfg->u16SrcHeight);
}
static void _DrvDispIfTransInputPortCropAttrToHal(MHAL_DISP_VidWinRect_t *pstMhalCfg, HalDispVidWinRect_t *pstHalCfg)
{
    pstHalCfg->u16X      = pstMhalCfg->u16X;
    pstHalCfg->u16Y      = pstMhalCfg->u16Y;
    pstHalCfg->u16Width  = pstMhalCfg->u16Width;
    pstHalCfg->u16Height = pstMhalCfg->u16Height;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, crop Rect(%d %d %d %d)\n", __FUNCTION__, __LINE__, pstHalCfg->u16X,
             pstHalCfg->u16Y, pstHalCfg->u16Width, pstHalCfg->u16Height);
}

static void _DrvDispIfTransRegAccessToHal(MHAL_DISP_RegAccessConfig_t *pstMhalCfg, HalDispRegAccessConfig_t *pstHalCfg)
{
    pstHalCfg->enType   = pstMhalCfg->enType == E_MHAL_DISP_REG_ACCESS_CPU    ? E_DISP_UTILITY_REG_ACCESS_CPU
                          : pstMhalCfg->enType == E_MHAL_DISP_REG_ACCESS_CMDQ ? E_DISP_UTILITY_REG_ACCESS_CMDQ
                                                                              : E_DISP_UTILITY_REG_ACCESS_NUM;
    pstHalCfg->pCmdqInf = (void *)pstMhalCfg->pCmdqInf;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Access:%s, CmdqIntf:%px \n", __FUNCTION__, __LINE__,
             PARSING_HAL_REG_ACCESS_TYPE(pstHalCfg->enType), pstHalCfg->pCmdqInf);
}

static void _DrvDispIfTransRegFlipToHal(MHAL_DISP_RegFlipConfig_t *pstMhalCfg, HalDispRegFlipConfig_t *pstHalCfg)
{
    pstHalCfg->bEnable  = pstMhalCfg->bEnable;
    pstHalCfg->pCmdqInf = (void *)pstMhalCfg->pCmdqInf;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, Enable:%d, CmdqIntf:%px \n", __FUNCTION__, __LINE__, pstHalCfg->bEnable,
             pstHalCfg->pCmdqInf);
}

static void _DrvDispIfTransRegWaitDoneToHal(MHAL_DISP_RegWaitDoneConfig_t *pstMhalCfg,
                                            HalDispRegWaitDoneConfig_t *   pstHalCfg)
{
    pstHalCfg->u32WaitType = pstMhalCfg->u32WaitType;
    pstHalCfg->pCmdqInf    = (void *)pstMhalCfg->pCmdqInf;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, WaitType:%x, CmdqIntf:%px \n", __FUNCTION__, __LINE__, pstHalCfg->u32WaitType,
             pstHalCfg->pCmdqInf);
}

static void _DrvDispIfTransPqCfgToHal(MHAL_DISP_PqConfig_t *pstMhalCfg, HalDispPqConfig_t *pstHalCfg)
{
    pstHalCfg->u32PqFlags  = pstMhalCfg->u32PqFlags;
    pstHalCfg->u32DataSize = pstMhalCfg->u32DataSize;
    pstHalCfg->pData       = pstMhalCfg->pData;
    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, PqFlag=%x, Size=%d, pData=%px \n", __FUNCTION__, __LINE__,
             pstHalCfg->u32PqFlags, pstHalCfg->u32DataSize, pstHalCfg->pData);
}

static void _DrvDispIfTransDmaBindToHal(MHAL_DISP_DmaBindConfig_t *pstMhalCfg, HalDispDmaBindConfig_t *pstHalCfg)
{
    pstHalCfg->pSrcDevCtx  = pstMhalCfg->pSrcDevCtx;
    pstHalCfg->pDestDevCtx = pstMhalCfg->pDestDevCtx;
}

static void _DrvDispIfTransDmaAttrToHal(MHAL_DISP_DmaAttrConfig_t *pstMhalCfg, HalDispDmaAttrConfig_t *pstHalCfg)
{
    // In Config
    pstHalCfg->stInputCfg.eType =
        pstMhalCfg->stInputCfg.eType == E_MHAL_DISP_DMA_INPUT_DEVICE_FRONT  ? E_HAL_DISP_DMA_INPUT_DEVICE_FRONT
        : pstMhalCfg->stInputCfg.eType == E_MHAL_DISP_DMA_INPUT_DEVICE_BACK ? E_HAL_DISP_DMA_INPUT_DEVICE_BACK
                                                                            : E_HAL_DISP_DMA_INPUT_NUM;

    pstHalCfg->stInputCfg.ePixelFormat  = _DrvDispIfTransDmaPixelFmtToHal(pstMhalCfg->stInputCfg.ePixelFormat);
    pstHalCfg->stInputCfg.eCompressMode = _DrvDispIfTransCompressToHal(pstMhalCfg->stInputCfg.eCompressMode);

    pstHalCfg->stInputCfg.u16Width  = pstMhalCfg->stInputCfg.u16Width;
    pstHalCfg->stInputCfg.u16Height = pstMhalCfg->stInputCfg.u16Height;

    // Out Config
    pstHalCfg->stOutputCfg.eAccessType =
        pstMhalCfg->stOutputCfg.eAccessType == E_MHAL_DISP_DMA_ACCESS_TYPE_EMI   ? E_HAL_DISP_DMA_ACCESS_TYPE_EMI
        : pstMhalCfg->stOutputCfg.eAccessType == E_MHAL_DISP_DMA_ACCESS_TYPE_IMI ? E_HAL_DISP_DMA_ACCESS_TYPE_IMI
                                                                                 : E_HAL_DISP_DMA_ACCESS_TYPE_NUM;
    pstHalCfg->stOutputCfg.eMode =
        pstMhalCfg->stOutputCfg.eMode == E_MHAL_DISP_DMA_OUTPUT_MODE_FRAME  ? E_HAL_DISP_DMA_OUTPUT_MODE_FRAME
        : pstMhalCfg->stOutputCfg.eMode == E_MHAL_DISP_DMA_OUTPUT_MODE_RING ? E_HAL_DISP_DMA_OUTPUT_MODE_RING
                                                                            : E_HAL_DISP_DMA_OUTPUT_MODE_NUM;

    pstHalCfg->stOutputCfg.ePixelFormat  = _DrvDispIfTransDmaPixelFmtToHal(pstMhalCfg->stOutputCfg.ePixelFormat);
    pstHalCfg->stOutputCfg.eCompressMode = _DrvDispIfTransCompressToHal(pstMhalCfg->stOutputCfg.eCompressMode);

    pstHalCfg->stOutputCfg.u16Width  = pstMhalCfg->stOutputCfg.u16Width;
    pstHalCfg->stOutputCfg.u16Height = pstMhalCfg->stOutputCfg.u16Height;

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, IN: Type:%s, PixFmt%s, Compress%s, Size(%d %d)\n", __FUNCTION__, __LINE__,
             PARSING_HAL_DMA_INPUT_TYPE(pstHalCfg->stInputCfg.eType),
             PARSING_HAL_DMA_PIXEL_FMT(pstHalCfg->stInputCfg.ePixelFormat),
             PARSING_HAL_COMPRESS_MD(pstHalCfg->stInputCfg.eCompressMode), pstHalCfg->stInputCfg.u16Width,
             pstHalCfg->stInputCfg.u16Height);

    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, OUT:Access:%s, Mode:%s, PixFmt:%s, Compress:%s, Width:%d, Heigh:%d\n",
             __FUNCTION__, __LINE__, PARSING_HAL_DMA_ACCESS_TYPE(pstHalCfg->stOutputCfg.eAccessType),
             PARSING_HAL_DMA_OUTPUT_MODE(pstHalCfg->stOutputCfg.eMode),
             PARSING_HAL_DMA_PIXEL_FMT(pstHalCfg->stOutputCfg.ePixelFormat),
             PARSING_HAL_COMPRESS_MD(pstHalCfg->stOutputCfg.eCompressMode), pstHalCfg->stOutputCfg.u16Width,
             pstHalCfg->stOutputCfg.u16Height);
}

static void _DrvDispIfTransDmaBufferAttrToHal(MHAL_DISP_DmaBufferAttrConfig_t *pstMhalCfg,
                                              HalDispDmaBufferAttrConfig_t *   pstHalCfg)
{
    u8 j;

    pstHalCfg->bEn = pstMhalCfg->bEn;

    for (j = 0; j < HAL_DISP_DMA_PIX_FMT_PLANE_MAX; j++)
    {
        pstHalCfg->paPhyAddr[j] = pstMhalCfg->paPhyAddr[j];
        pstHalCfg->u32Stride[j] = pstMhalCfg->u32Stride[j];
    }

    pstHalCfg->u16RingBuffHeight = pstMhalCfg->u16RingBuffHeight;
    pstHalCfg->u16RingStartLine  = pstMhalCfg->u16RingStartLine;
    pstHalCfg->u16FrameIdx       = pstMhalCfg->u16FrameIdx;
    pstHalCfg->enCrcTpye         = pstMhalCfg->enCrcType;

    DISP_DBG(DISP_DBG_LEVEL_DRV,
             "%s %d, PhyAddr(%llx %llx %llx), Stride(%d %d %d), RingBufHeight:%d, RingStartLine:%d FrameIdx:%d\n",
             __FUNCTION__, __LINE__, pstHalCfg->paPhyAddr[0], pstHalCfg->paPhyAddr[1], pstHalCfg->paPhyAddr[2],
             pstHalCfg->u32Stride[0], pstHalCfg->u32Stride[1], pstHalCfg->u32Stride[2], pstHalCfg->u16RingBuffHeight,
             pstHalCfg->u16RingStartLine, pstHalCfg->u16FrameIdx);
}

static void _DrvDispIfTransDeviceConfigToHal(MHAL_DISP_DeviceConfig_t *pstMhalCfg, HalDispInternalConfig_t *pstHalCfg)
{
    pstHalCfg->eType        = pstMhalCfg->eType;
    pstHalCfg->bBootlogoEn  = pstMhalCfg->bBootlogoEn;
    pstHalCfg->u8ColorId    = pstMhalCfg->u8ColorId;
    pstHalCfg->u8GopBlendId = pstMhalCfg->u8GopBlendId;
    pstHalCfg->bDispPat     = pstMhalCfg->bDispPat;
    pstHalCfg->bRstMop      = pstMhalCfg->bRstMop;
    pstHalCfg->bRstDisp     = pstMhalCfg->bRstDisp;
    pstHalCfg->bRstDac      = pstMhalCfg->bRstDac;
    pstHalCfg->bRstHdmitx   = pstMhalCfg->bRstHdmitx;
    pstHalCfg->bRstLcd      = pstMhalCfg->bRstLcd;
    pstHalCfg->bRstDacAff   = pstMhalCfg->bRstDacAff;
    pstHalCfg->bRstDacSyn   = pstMhalCfg->bRstDacSyn;
    pstHalCfg->bRstFpll     = pstMhalCfg->bRstFpll;
    pstHalCfg->u8PatMd      = pstMhalCfg->u8PatMd;
    pstHalCfg->bCsCEn       = pstMhalCfg->bCscEn;
    pstHalCfg->u8CsCMd      = pstMhalCfg->u8CscMd;
    pstHalCfg->bCtx         = pstMhalCfg->bCtx;
    pstHalCfg->u8MopYThrd   = pstMhalCfg->u8MopYThrd;
    pstHalCfg->u8MopCThrd   = pstMhalCfg->u8MopCThrd;
    DISP_DBG(DISP_DBG_LEVEL_DRV,
             "%s %d, Type:%x, Bootlogo:%hhd, ColorId:%hhd GopBlendId:%hhd, DispPat:%hhd, DispMd:%hhd, RstMop:%hhd, RstDisp:%hhd "
             "RstDac:%hhd, RstHdmitx:%hhd, RstLcd:%hhd, DacAff:%hhd, DacSyn:%hhd, Fpll:%hhd, CscEn:%hhd, CscMd:%hhd, Ctx:%hhd, "
             "MopYThrd:%hhd, MopCThrd:%hhd\n",
             __FUNCTION__, __LINE__, pstHalCfg->eType, pstHalCfg->bBootlogoEn, pstHalCfg->u8ColorId,
             pstHalCfg->u8GopBlendId, pstHalCfg->bDispPat, pstHalCfg->u8PatMd, pstHalCfg->bRstMop, pstHalCfg->bRstDisp,
             pstHalCfg->bRstDac, pstHalCfg->bRstHdmitx, pstHalCfg->bRstLcd, pstHalCfg->bRstDacAff,
             pstHalCfg->bRstDacSyn, pstHalCfg->bRstFpll, pstHalCfg->bCsCEn, pstHalCfg->u8CsCMd, pstHalCfg->bCtx,
             pstHalCfg->u8MopYThrd, pstHalCfg->u8MopCThrd);
}

void DrvDispIfTransDeviceConfigToMHal(MHAL_DISP_DeviceConfig_t *pstMhalCfg, HalDispInternalConfig_t *pstHalCfg)
{
    pstMhalCfg->eType        = pstHalCfg->eType;
    pstMhalCfg->bBootlogoEn  = pstHalCfg->bBootlogoEn;
    pstMhalCfg->u8ColorId    = pstHalCfg->u8ColorId;
    pstMhalCfg->u8GopBlendId = pstHalCfg->u8GopBlendId;
    pstMhalCfg->bDispPat     = pstHalCfg->bDispPat;
    pstMhalCfg->bRstMop      = pstHalCfg->bRstMop;
    pstMhalCfg->bRstDisp     = pstHalCfg->bRstDisp;
    pstMhalCfg->bRstDac      = pstHalCfg->bRstDac;
    pstMhalCfg->bRstHdmitx   = pstHalCfg->bRstHdmitx;
    pstMhalCfg->bRstLcd      = pstHalCfg->bRstLcd;
    pstMhalCfg->bRstDacAff   = pstHalCfg->bRstDacAff;
    pstMhalCfg->bRstDacSyn   = pstHalCfg->bRstDacSyn;
    pstMhalCfg->bRstFpll     = pstHalCfg->bRstFpll;
    pstMhalCfg->u8PatMd      = pstHalCfg->u8PatMd;
    pstMhalCfg->bCscEn       = pstHalCfg->bCsCEn;
    pstMhalCfg->u8CscMd      = pstHalCfg->u8CsCMd;
    pstMhalCfg->bCtx         = pstHalCfg->bCtx;
    pstMhalCfg->u8MopYThrd   = pstHalCfg->u8MopYThrd;
    pstMhalCfg->u8MopCThrd   = pstHalCfg->u8MopCThrd;

    DISP_DBG(DISP_DBG_LEVEL_DRV,
             "%s %d, Type:%x, Bootlogo:%hhd, ColorId:%hhd GopBlendId:%hhd, DispPat:%hhd, DispMd:%hhd, RstMop:%hhd, RstDisp:%hhd "
             "RstDac:%hhd, RstHdmitx:%hhd, RstLcd:%hhd, DacAff:%hhd, DacSyn:%hhd, Fpll:%hhd, CscEn:%hhd, CscMd:%hhd, Ctx:%hhd, "
             "MopYThrd:%hhd, MopCThrd:%hhd\n",
             __FUNCTION__, __LINE__, pstMhalCfg->eType, pstMhalCfg->bBootlogoEn, pstMhalCfg->u8ColorId,
             pstMhalCfg->u8GopBlendId, pstMhalCfg->bDispPat, pstMhalCfg->u8PatMd, pstMhalCfg->bRstMop, pstMhalCfg->bRstDisp,
             pstMhalCfg->bRstDac, pstMhalCfg->bRstHdmitx, pstMhalCfg->bRstLcd, pstMhalCfg->bRstDacAff,
             pstMhalCfg->bRstDacSyn, pstMhalCfg->bRstFpll, pstMhalCfg->bCscEn, pstMhalCfg->u8CscMd, pstMhalCfg->bCtx,
             pstMhalCfg->u8MopYThrd, pstMhalCfg->u8MopCThrd);
}

static void _DrvDispIfTransDeviceCapabilityToMhal(MHAL_DISP_DeviceCapabilityConfig_t *pstMhalCfg,
                                                  HalDispDeviceCapabilityConfig_t *   pstHalCfg)
{
    pstMhalCfg->ePqType =
        pstHalCfg->ePqType == E_HAL_DISP_DEV_PQ_HPQ ? E_MHAL_DISP_DEV_PQ_HPQ : E_MHAL_DISP_DEV_PQ_MACE;

    pstMhalCfg->bWdmaSupport  = pstHalCfg->bWdmaSupport;
    pstMhalCfg->u32VideoLayerHwCnt = pstHalCfg->u32VidLayerCnt;
    pstMhalCfg->u32VideoLayerStartOffset = pstHalCfg->u32VidLayerStartNumber;
    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, PqType:%d, VidLayerCnt:%x, WdmaSupport:%d\n", __FUNCTION__, __LINE__,
             pstMhalCfg->ePqType, pstMhalCfg->u32VideoLayerHwCnt, pstMhalCfg->bWdmaSupport);
}

static void _DrvDispIfTransVideoLayerCapabilityToMhal(MHAL_DISP_VideoLayerCapabilityConfig_t *pstMhalCfg,
                                                      HalDispVideoLayerCapabilityConfig_t *   pstHalCfg)
{
    pstMhalCfg->bRotateSupport             = pstHalCfg->bRotateSupport;
    pstMhalCfg->u32InputPortHwCnt          = pstHalCfg->u32InputPortHwCnt;
    pstMhalCfg->bCompressFmtSupport        = pstHalCfg->bCompressFmtSupport;
    pstMhalCfg->u32InputPortPitchAlignment = pstHalCfg->u32InputPortPitchAlignment;
    pstMhalCfg->u32RotateHeightAlighment   = pstHalCfg->u32RotateHeightAlig;
    DISP_DBG(DISP_DBG_LEVEL_DRV,
             "%s %d, VidLayerId:%d, Rotate:%d, Compress:%d, InputPortHWCnt:%d, PitchAlign:%d, RotateHeightAligh:%d\n",
             __FUNCTION__, __LINE__, pstHalCfg->u32VidLayerId, pstMhalCfg->bRotateSupport,
             pstMhalCfg->bCompressFmtSupport, pstMhalCfg->u32InputPortHwCnt, pstMhalCfg->u32InputPortPitchAlignment,
             pstMhalCfg->u32RotateHeightAlighment);
}

static void _DrvDispIfTransDeviceInterfaceCapabilityToMhal(MHAL_DISP_InterfaceCapabilityConfig_t *pstMhalCfg,
                                                           HalDispInterfaceCapabilityConfig_t *   pstHalCfg)
{
    u8 u8Idx    = 0;
    u8 u8HalIdx = 0;
    u16 u16FpsDot = 0;

    pstMhalCfg->u32HwCount = pstHalCfg->u32HwCount;
    for (u8Idx = 0; u8Idx < E_MHAL_DISP_OUTPUT_MAX; u8Idx++)
    {
        u8HalIdx                          = _DrvDispIfTransDevTimingTypeToHal(u8Idx, &u16FpsDot, &u16FpsDot);
        if(u8HalIdx != E_HAL_DISP_OUTPUT_MAX)
        {
            pstMhalCfg->bSupportTiming[u8Idx] = pstHalCfg->bSupportTiming[u8HalIdx];
        }
    }
}

static void _DrvDispIfTransDmaCapabilityToMhal(MHAL_DISP_DmaCapabiliytConfig_t *pstMhalCfg,
                                               HalDispDmaCapabilitytConfig_t *  pstHalCfg)
{
    u8 u8Idx    = 0;
    u8 u8HalIdx = 0;

    pstMhalCfg->bCompressSupport = pstHalCfg->bCompressSupport;
    for (u8Idx = 0; u8Idx < E_MHAL_COMPRESS_MODE_MAX; u8Idx++)
    {
        u8HalIdx = _DrvDispIfTransCompressToHal(u8Idx);
        if (u8HalIdx < E_HAL_DISP_COMPRESS_MODE_MAX)
        {
            pstMhalCfg->bSupportCompress[u8Idx] = pstHalCfg->bSupportCompress[u8HalIdx];
        }
    }
    for (u8Idx = 0; u8Idx < E_MHAL_PIXEL_FRAME_FORMAT_MAX; u8Idx++)
    {
        u8HalIdx = _DrvDispIfTransPixelFormatToHal(u8Idx);
        if (u8HalIdx < E_HAL_DISP_PIXEL_FRAME_FORMAT_MAX)
        {
            pstMhalCfg->u8FormatWidthAlign[u8Idx] = pstHalCfg->u8FormatWidthAlign[u8HalIdx];
        }
    }
    pstMhalCfg->eDeviceType = E_MHAL_DISP_DEVICE_NONE;

    pstMhalCfg->eDeviceType = (pstHalCfg->eDeviceType & E_HAL_DISP_DEVICE_0)
                                  ? pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_0
                                  : pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_NONE;

    pstMhalCfg->eDeviceType = (pstHalCfg->eDeviceType & E_HAL_DISP_DEVICE_1)
                                  ? pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_1
                                  : pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_NONE;

    pstMhalCfg->eDeviceType = (pstHalCfg->eDeviceType & E_HAL_DISP_DEVICE_2)
                                  ? pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_2
                                  : pstMhalCfg->eDeviceType | E_MHAL_DISP_DEVICE_NONE;
    DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, bCompress:%d, DeviceType:%x\n", __FUNCTION__, __LINE__,
             pstMhalCfg->bCompressSupport, pstMhalCfg->eDeviceType);
}

static u8 _DrvDispIfSetClkOn(void *pCtx)
{
    u8   bRet               = 1;
    u32 *pu32ClkDefaultRate = NULL;
    u8 * pbClkDefaultEn     = NULL;
    u32 *pu32ClkCurRate     = NULL;
    u8 * pbClkCurEn         = NULL;
    u8   i;

    if (E_HAL_DISP_CLK_NUM)
    {
        pu32ClkDefaultRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);
        pbClkDefaultEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);

        pu32ClkCurRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);
        pbClkCurEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);

        if (pu32ClkDefaultRate == NULL || pbClkDefaultEn == NULL || pu32ClkCurRate == NULL || pbClkCurEn == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Allocate Memory Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
            {
                pbClkDefaultEn[i]     = HAL_DISP_CLK_GET_ON_SETTING(i);
                pu32ClkDefaultRate[i] = HAL_DISP_CLK_GET_RATE_ON_SETTING(i);
            }
            DrvDispIfGetClk(pCtx, pbClkCurEn, pu32ClkCurRate, E_HAL_DISP_CLK_NUM);

            if (DrvDispOsSetClkOn(pbClkCurEn, pu32ClkDefaultRate, E_HAL_DISP_CLK_NUM) == 0)
            {
                if (DrvDispIfSetClk(pCtx, pbClkDefaultEn, pu32ClkDefaultRate, E_HAL_DISP_CLK_NUM) == 0)
                {
                    DISP_ERR("%s %d:: SetClk Fail\n", __FUNCTION__, __LINE__);
                    bRet = 0;
                }
                else
                {
                    bRet = 1;
                }
            }
            else
            {
                bRet = 1;
            }
        }
    }

    if (pu32ClkDefaultRate)
    {
        DrvDispOsMemRelease(pu32ClkDefaultRate);
    }
    if (pu32ClkCurRate)
    {
        DrvDispOsMemRelease(pu32ClkCurRate);
    }

    if (pbClkDefaultEn)
    {
        DrvDispOsMemRelease(pbClkDefaultEn);
    }

    if (pbClkCurEn)
    {
        DrvDispOsMemRelease(pbClkCurEn);
    }

    return bRet;
}

static u8 _DrvDispIfSetClkOff(void *pCtx)
{
    u8   bRet               = 1;
    u32 *pu32ClkDefaultRate = NULL;
    u8 * pbClkDefaultEn     = NULL;
    u32 *pu32ClkCurRate     = NULL;
    u8 * pbClkCurEn         = NULL;
    u8   i;

    if (E_HAL_DISP_CLK_NUM)
    {
        pu32ClkDefaultRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);
        pbClkDefaultEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);

        pu32ClkCurRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);
        pbClkCurEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);

        if (pu32ClkDefaultRate == NULL || pbClkDefaultEn == NULL || pu32ClkCurRate == NULL || pbClkCurEn == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Allocate Memory Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
            {
                pbClkDefaultEn[i]     = HAL_DISP_CLK_GET_OFF_SETTING(i);
                pu32ClkDefaultRate[i] = HAL_DISP_CLK_GET_RATE_OFF_SETTING(i);
            }
            DrvDispIfGetClk(pCtx, pbClkCurEn, pu32ClkCurRate, E_HAL_DISP_CLK_NUM);

            if (DrvDispOsSetClkOff(pbClkCurEn, pu32ClkDefaultRate, E_HAL_DISP_CLK_NUM) == 0)
            {
                if (DrvDispIfSetClk(pCtx, pbClkDefaultEn, pu32ClkDefaultRate, E_HAL_DISP_CLK_NUM) == 0)
                {
                    DISP_ERR("%s %d:: SetClk Fail\n", __FUNCTION__, __LINE__);
                    bRet = 0;
                }
                else
                {
                    bRet = 1;
                }
            }
            else
            {
                bRet = 1;
            }
        }
    }

    if (pu32ClkDefaultRate)
    {
        DrvDispOsMemRelease(pu32ClkDefaultRate);
    }
    if (pu32ClkCurRate)
    {
        DrvDispOsMemRelease(pu32ClkCurRate);
    }

    if (pbClkDefaultEn)
    {
        DrvDispOsMemRelease(pbClkDefaultEn);
    }

    if (pbClkCurEn)
    {
        DrvDispOsMemRelease(pbClkCurEn);
    }

    return bRet;
}

static u8 _DrvDispIfExecuteQueryWithoutCtx(HalDispQueryConfig_t *pstQueryCfg)
{
    u8                 bRet = 1;
    DrvDispCtxConfig_t stFakeDispCtxCfg;

    memset(&stFakeDispCtxCfg, 0,sizeof(DrvDispCtxConfig_t));
    if (HalDispIfQuery(&stFakeDispCtxCfg, pstQueryCfg))
    {
        if (pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_OK
            || pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_NONEED)
        {
            if (pstQueryCfg->stOutCfg.pSetFunc)
            {
                pstQueryCfg->stOutCfg.pSetFunc(&stFakeDispCtxCfg, pstQueryCfg->stInCfg.pInCfg);
            }
        }
        else if (pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_IMPLICIT_ERR)
        {
            bRet = 0;
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Query:%s, Ret:%s\n", __FUNCTION__, __LINE__,
                     PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType),
                     PARSING_HAL_QUERY_RET(pstQueryCfg->stOutCfg.enQueryRet));
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Query Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

static u8 _DrvDispIfExecuteQuery(void *pCtx, HalDispQueryConfig_t *pstQueryCfg)
{
    u8 bRet = 1;

    if (HalDispIfQuery(pCtx, pstQueryCfg))
    {
        if (pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_OK
            || pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_NONEED)
        {
            if (pstQueryCfg->stOutCfg.pSetFunc)
            {
                pstQueryCfg->stOutCfg.pSetFunc(pCtx, pstQueryCfg->stInCfg.pInCfg);
            }
        }
        else if (pstQueryCfg->stOutCfg.enQueryRet == E_HAL_DISP_QUERY_RET_IMPLICIT_ERR)
        {
            bRet = 0;
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Query:%s, Ret:%s\n", __FUNCTION__, __LINE__,
                     PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType),
                     PARSING_HAL_QUERY_RET(pstQueryCfg->stOutCfg.enQueryRet));
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Query Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 DrvDispIfGetClk(void *pDevCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispClkConfig_t   stHalClkCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    memset(&stHalClkCfg, 0, sizeof(HalDispClkConfig_t));
    stHalClkCfg.u32Num = u32ClkNum;

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_CLK_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(stHalClkCfg);
    stQueryCfg.stInCfg.pInCfg      = &stHalClkCfg;

    if (_DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg))
    {
        if (stHalClkCfg.u32Num == u32ClkNum)
        {
            memcpy(pu32ClkRate, stHalClkCfg.u32Rate, sizeof(stHalClkCfg.u32Rate));
            memcpy(pbEn, stHalClkCfg.bEn, sizeof(stHalClkCfg.bEn));
            bRet = 1;
        }
    }
    else
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvDispIfSetClk(void *pDevCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispClkConfig_t   stHalClkCfg;

    if (sizeof(stHalClkCfg.u32Rate) != sizeof(u32) * u32ClkNum || sizeof(stHalClkCfg.bEn) != sizeof(u8) * u32ClkNum)
    {
        bRet = 0;
        DISP_ERR("%s %d, Clk Num is not correct: Rate:%d != %d, En:%d != %d", __FUNCTION__, __LINE__,
                 sizeof(stHalClkCfg.u32Rate), sizeof(u32) * u32ClkNum, sizeof(stHalClkCfg.bEn), sizeof(u8) * u32ClkNum);
    }
    else
    {
        HalDispIfInit();

        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_CLK_SET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispClkConfig_t);
        stQueryCfg.stInCfg.pInCfg      = &stHalClkCfg;

        stHalClkCfg.u32Num = u32ClkNum;
        memcpy(stHalClkCfg.u32Rate, pu32ClkRate, sizeof(u32) * u32ClkNum);
        memcpy(stHalClkCfg.bEn, pbEn, sizeof(u8) * u32ClkNum);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfInitPanelConfig(MHAL_DISP_PanelConfig_t *pstPanelConfig, u8 u8Size)
{
    u8                           i;
    u8                           bRet = TRUE;
    u32                          u32Mod, u32HttVtt, u32DClkhz;
    u32                          u32Tmp;
    u16                          u16FrontPorch;
    u16                          u16FpsDot = 0;
    u16                          u16Fps = 0;
    HalDispDeviceTimingConfig_t *pstDevTiming  = NULL;
    DrvDispIfTimingData_t *      pstTimingData = NULL;

    if (u8Size > E_HAL_DISP_OUTPUT_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, The size (%d) is bigger than %d\n", __FUNCTION__, __LINE__, u8Size, E_HAL_DISP_OUTPUT_MAX);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, u8Size:%d\n", __FUNCTION__, __LINE__, u8Size);

        if (g_stDispDevTimingCfg.pstData)
        {
            DrvDispOsMemRelease(g_stDispDevTimingCfg.pstData);
            g_stDispDevTimingCfg.pstData     = NULL;
            g_stDispDevTimingCfg.u32DataSize = 0;
        }

        g_stDispDevTimingCfg.pstData     = DrvDispOsMemAlloc(sizeof(DrvDispIfTimingData_t) * u8Size);
        g_stDispDevTimingCfg.u32DataSize = u8Size;

        if (g_stDispDevTimingCfg.pstData == NULL)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Allocate Memory Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            for (i = 0; i < u8Size; i++)
            {
                pstTimingData                  = &g_stDispDevTimingCfg.pstData[i];
                pstTimingData->bValid          = pstPanelConfig[i].bValid;
                pstTimingData->enDevTimingType = _DrvDispIfTransDevTimingTypeToHal(pstPanelConfig[i].eTiming, &u16Fps, &u16FpsDot);

                pstDevTiming                    = &pstTimingData->stDevTiming;
                pstDevTiming->u16HsyncWidth     = pstPanelConfig[i].stPanelAttr.m_ucPanelHSyncWidth;
                pstDevTiming->u16HsyncBackPorch = pstPanelConfig[i].stPanelAttr.m_ucPanelHSyncBackPorch;
                pstDevTiming->u16Hstart         = pstPanelConfig[i].stPanelAttr.m_wPanelHStart;
                pstDevTiming->u16Hactive        = pstPanelConfig[i].stPanelAttr.m_wPanelWidth;
                pstDevTiming->u16Htotal         = pstPanelConfig[i].stPanelAttr.m_wPanelHTotal;

                pstDevTiming->u16VsyncWidth     = pstPanelConfig[i].stPanelAttr.m_ucPanelVSyncWidth;
                pstDevTiming->u16VsyncBackPorch = pstPanelConfig[i].stPanelAttr.m_ucPanelVBackPorch;
                pstDevTiming->u16Vstart         = pstPanelConfig[i].stPanelAttr.m_wPanelVStart;
                pstDevTiming->u16Vactive        = pstPanelConfig[i].stPanelAttr.m_wPanelHeight;
                pstDevTiming->u16Vtotal         = pstPanelConfig[i].stPanelAttr.m_wPanelVTotal;
                pstDevTiming->u32Dclk           = pstPanelConfig[i].stPanelAttr.m_dwPanelDCLK;

                u32HttVtt = ((u32)pstPanelConfig[i].stPanelAttr.m_wPanelHTotal
                             * (u32)pstPanelConfig[i].stPanelAttr.m_wPanelVTotal);
                if(pstTimingData->enDevTimingType == E_HAL_DISP_OUTPUT_USER)
                {
                    u32DClkhz = (pstPanelConfig[i].stPanelAttr.m_dwPanelDCLK);
                    if(u32DClkhz < (u32HttVtt/100))
                    {
                        DISP_DBG(DISP_DBG_LEVEL_DRV,"%s %d, The Dclk (%d) is Too small than vtthtt %d\n", __FUNCTION__, __LINE__, u32DClkhz, u32HttVtt);
                        u32DClkhz *= HAL_DISP_1M;
                    }
                    u32Mod    = u32DClkhz % u32HttVtt;

                    if (u32Mod > (u32HttVtt / 2))
                    {
                        pstDevTiming->u16Fps = (u32DClkhz + u32HttVtt - 1) / u32HttVtt;
                    }
                    else
                    {
                        pstDevTiming->u16Fps = u32DClkhz / u32HttVtt;
                    }
                }
                else
                {
                    u32DClkhz = u32HttVtt * u16Fps;
                    if(u16FpsDot)
                    {
                        u32DClkhz += ((u32HttVtt * u16FpsDot) /100);
                        u16Fps++;
                    }
                    pstDevTiming->u16Fps = u16Fps;
                    pstDevTiming->u32Dclk = u32DClkhz;
                }
                pstDevTiming->u16SscSpan = pstPanelConfig[i].stPanelAttr.m_wSpreadSpectrumSpan;
                pstDevTiming->u16SscStep = pstPanelConfig[i].stPanelAttr.m_wSpreadSpectrumStep;

                u32Tmp        = 1000000UL / (u32)pstDevTiming->u16Fps;
                u16FrontPorch = (pstDevTiming->u16Vtotal - pstDevTiming->u16VsyncWidth - pstDevTiming->u16VsyncBackPorch
                                 - pstDevTiming->u16Vactive);
                pstDevTiming->u32VSyncPeriod = u32Tmp * (u32)pstDevTiming->u16VsyncWidth / (u32)pstDevTiming->u16Vtotal;
                pstDevTiming->u32VBackPorchPeriod =
                    u32Tmp * (u32)pstDevTiming->u16VsyncBackPorch / (u32)pstDevTiming->u16Vtotal;
                pstDevTiming->u32VActivePeriod = u32Tmp * (u32)pstDevTiming->u16Vactive / (u32)pstDevTiming->u16Vtotal;
                pstDevTiming->u32VFrontPorchPeriod = u32Tmp * (u32)u16FrontPorch / (u32)pstDevTiming->u16Vtotal;
            }
        }
    }
    return bRet;
}

u8 DrvDispIfSetRegAccessConfig(void *pDevCtx, MHAL_DISP_RegAccessConfig_t *pstRegAccessCfg)
{
    u8                       bRet = 1;
    HalDispQueryConfig_t     stQueryCfg;
    HalDispRegAccessConfig_t stHalRegAccessCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_REG_ACCESS;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispRegAccessConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalRegAccessCfg;

        _DrvDispIfTransRegAccessToHal(pstRegAccessCfg, &stHalRegAccessCfg);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfSetRegFlipConfig(void *pDevCtx, MHAL_DISP_RegFlipConfig_t *pstRegFlipCfg)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispRegFlipConfig_t stHalRegFlipCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_REG_FLIP;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(stHalRegFlipCfg);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalRegFlipCfg;

        _DrvDispIfTransRegFlipToHal(pstRegFlipCfg, &stHalRegFlipCfg);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfSetRegWaitDoneConfig(void *pDevCtx, MHAL_DISP_RegWaitDoneConfig_t *pstRegWaitDoneCfg)
{
    u8                         bRet = 1;
    HalDispQueryConfig_t       stQueryCfg;
    HalDispRegWaitDoneConfig_t stHalRegWaitDoneCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_REG_WAITDONE;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispRegWaitDoneConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalRegWaitDoneCfg;

        _DrvDispIfTransRegWaitDoneToHal(pstRegWaitDoneCfg, &stHalRegWaitDoneCfg);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfSetCmdqInterfaceConfig(u32 u32DevId, MHAL_DISP_CmdqInfConfig_t *pstCmdqInfCfg)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispCmdqInfConfig_t stHalCmdqIfCfg;
    DrvDispCtxConfig_t     stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_CMDQINF;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispCmdqInfConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stHalCmdqIfCfg;

    stHalCmdqIfCfg.pCmdqInf = (void *)pstCmdqInfCfg->pCmdqInf;
    stHalCmdqIfCfg.u32DevId = u32DevId;

    bRet = _DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg);
    return bRet;
}

u8 DrvDispIfDeviceCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DeviceId, void **pDevCtx)
{
    u8                      bRet = TRUE;
    DrvDispCtxAllocConfig_t stCtxAllocCfg;
    HalDispQueryConfig_t    stQueryCfg;
    DrvDispCtxConfig_t *    pstDispCtx = NULL;

    DrvDispCtxInit();

    stCtxAllocCfg.enType             = E_DISP_CTX_TYPE_DEVICE;
    stCtxAllocCfg.u32DevId           = u32DeviceId;
    stCtxAllocCfg.pstBindCtx         = NULL;
    stCtxAllocCfg.stMemAllcCfg.alloc = (pDispCtxMemAlloc)pstAlloc->alloc;
    stCtxAllocCfg.stMemAllcCfg.free  = (pDispCtxMemFree)pstAlloc->free;

    if (DrvDispCtxAllocate(&stCtxAllocCfg, &pstDispCtx) == FALSE)
    {
        bRet     = 0;
        *pDevCtx = NULL;
        DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        *pDevCtx = (void *)pstDispCtx;

        if (_DrvDispIfSetClkOn((void *)pstDispCtx))
        {
            memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
            stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_INIT;
            stQueryCfg.stInCfg.pInCfg      = NULL;
            stQueryCfg.stInCfg.u32CfgSize  = 0;

            if (_DrvDispIfExecuteQuery(pstDispCtx, &stQueryCfg))
            {
                DrvDispCtxSetCurCtx(pstDispCtx, pstDispCtx->u32ContainIdx);
                DrvDispIrqCreateInternalIsr((void *)pstDispCtx);
            }
            else
            {
                DISP_ERR("%s %d:: Device Init Fail\n", __FUNCTION__, __LINE__);
                bRet = 0;
            }
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Set Clk On Fail\n", __FUNCTION__, __LINE__);
        }
    }

    return bRet;
}

u8 DrvDispIfDeviceDestroyInstance(void *pDevCtx)
{
    u8                   bRet = TRUE;
    HalDispQueryConfig_t stQueryCfg;
    DrvDispCtxConfig_t * pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;

    if (DrvDispCtxSetCurCtx(pstDispCtx, pstDispCtx->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqDestroyInternalIsr((void *)pstDispCtx))
        {
            if(DrvDispCtxIsLastDeviceCtx(pstDispCtx))
            {
                memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
                stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_DEINIT;
                stQueryCfg.stInCfg.pInCfg      = NULL;
                stQueryCfg.stInCfg.u32CfgSize  = 0;
                
                if (_DrvDispIfExecuteQuery(pstDispCtx, &stQueryCfg))
                {
                    if (!_DrvDispIfSetClkOff(pDevCtx))
                    {
                        bRet = 0;
                        DISP_ERR("%s %d, Set Clk Off Fail\n", __FUNCTION__, __LINE__);
                    }
                }
            }
            if (!DrvDispCtxFree(pstDispCtx))
            {
                bRet = 0;
                DISP_ERR("%s %d, DestroyInstance Fail\n", __FUNCTION__, __LINE__);
            }
            if (DrvDispCtxIsAllFree())
            {
                DrvDispCtxDeInit();
            }
        }
    }
    return bRet;
}

u8 DrvDispIfDeviceEnable(void *pDevCtx, u8 bEnable)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_ENABLE;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(u8);
        stQueryCfg.stInCfg.pInCfg      = (void *)&bEnable;

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceSetBackGroundColor(void *pDevCtx, u32 u32BgColor)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(u32);
        stQueryCfg.stInCfg.pInCfg      = (void *)&u32BgColor;

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceAddOutInterface(void *pDevCtx, u32 u32Interface)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_INTERFACE;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(u32);
        stQueryCfg.stInCfg.pInCfg      = (void *)&u32Interface;

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceSetOutputTiming(void *pDevCtx, u32 u32Interface, MHAL_DISP_DeviceTimingInfo_t *pstTimingInfo)
{
    u8                        bRet = 1;
    HalDispQueryConfig_t      stQueryCfg;
    HalDispDeviceTimingInfo_t stHalTimingIfo;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceTimingInfo_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalTimingIfo;

        if (_DrvDispIfTransDeviceOutpuTimingInfoToHal(pstTimingInfo, &stHalTimingIfo))
        {
            bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
        }
        else
        {
            bRet = 0;
        }
    }

    return bRet;
}

u8 DrvDispIfDeviceSetColortemp(void *pDevCtx, MHAL_DISP_ColorTempeture_t *pstcolorTemp)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispColorTemp_t   stHalColorTemp;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_COLORTEMP;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispColorTemp_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalColorTemp;

        _DrvDispIfTransDeviceColorTempToHal(pstcolorTemp, &stHalColorTemp);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceSetCvbsParam(void *pDevCtx, MHAL_DISP_CvbsParam_t *pstCvbsInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t stHalCvbsParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalCvbsParam, 0, sizeof(HalDispDeviceParam_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalCvbsParam;

        _DrvDispIfTransDeviceCvbsParamToHal(pstCvbsInfo, &stHalCvbsParam);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}
u8 DrvDispIfDeviceGetCvbsParam(void *pDevCtx, MHAL_DISP_CvbsParam_t *pstCvbsInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t   stHalCvbsParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalCvbsParam, 0, sizeof(HalDispDeviceParam_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM_GET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalCvbsParam;

        if (_DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg))
        {
            _DrvDispIfTransDeviceCvbsParamToMhal(pstCvbsInfo, &stHalCvbsParam);
        }
        else
        {
            bRet = 0;
        }
    }
    return bRet;
}

u8 DrvDispIfDeviceSetHdmiParam(void *pDevCtx, MHAL_DISP_HdmiParam_t *pstHdmiInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t stHalHdmiParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalHdmiParam, 0, sizeof(HalDispDeviceParam_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalHdmiParam;

        _DrvDispIfTransDeviceHdmiParamToHal(pstHdmiInfo, &stHalHdmiParam);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetHdmiParam(void *pDevCtx, MHAL_DISP_HdmiParam_t *pstHdmiInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t stHalHdmiParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalHdmiParam, 0, sizeof(HalDispDeviceParam_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalHdmiParam;

        if (_DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg))
        {
            _DrvDispIfTransDeviceHdmiParamToMhal(pstHdmiInfo, &stHalHdmiParam);
        }
        else
        {
            bRet = 0;
        }
    }
    return bRet;
}

u8 DrvDispIfDeviceSetLcdParam(void *pDevCtx, MHAL_DISP_LcdParam_t *pstLcdInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t stHalLcdParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalLcdParam, 0, sizeof(HalDispDeviceParam_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_LCD_PARAM;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalLcdParam;

        _DrvDispIfTransDeviceLcdParamToHal(pstLcdInfo, &stHalLcdParam);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetLcdParam(void *pDevCtx, MHAL_DISP_LcdParam_t *pstLcdInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t stHalLcdParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalLcdParam, 0, sizeof(HalDispDeviceParam_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalLcdParam;

        if (_DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg))
        {
            _DrvDispIfTransDeviceLcdParamToMhal(pstLcdInfo, &stHalLcdParam);
        }
        else
        {
            bRet = 0;
        }
    }
    return bRet;
}

u8 DrvDispIfDeviceSetGammaParam(void *pDevCtx, MHAL_DISP_GammaParam_t *pstGammaInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispGammaParam_t  stHalGammaaram;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispGammaParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalGammaaram;

        _DrvDispIfTransdeviceGammaParamToHal(pstGammaInfo, &stHalGammaaram);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceSetVgaParam(void *pDevCtx, MHAL_DISP_VgaParam_t *pstVgaInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t    stHalVgaParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalVgaParam, 0, sizeof(HalDispDeviceParam_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_VGA_PARAM;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalVgaParam;

        _DrvDispIfTransDeviceVgaParamToHal(pstVgaInfo, &stHalVgaParam);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetVgaParam(void *pDevCtx, MHAL_DISP_VgaParam_t *pstVgaInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDeviceParam_t    stHalVgaParam;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        memset(&stHalVgaParam, 0, sizeof(HalDispDeviceParam_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceParam_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalVgaParam;

        if (_DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg))
        {
            _DrvDispIfTransDeviceVgaParamToMhal(pstVgaInfo, &stHalVgaParam);
        }
        else
        {
            bRet = 0;
        }
    }
    return bRet;
}

u8 DrvDispIfDeviceAttach(void *pSrcDevCtx, void *pDstDevCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pSrcDevCtx, ((DrvDispCtxConfig_t *)pSrcDevCtx)->u32ContainIdx) == FALSE
        || DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDstDevCtx, ((DrvDispCtxConfig_t *)pDstDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_ATTACH;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(pDstDevCtx);
        stQueryCfg.stInCfg.pInCfg      = pDstDevCtx;

        bRet = _DrvDispIfExecuteQuery(pSrcDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceDetach(void *pSrcDevCtx, void *pDstDevCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pSrcDevCtx, ((DrvDispCtxConfig_t *)pSrcDevCtx)->u32ContainIdx) == FALSE
        || DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDstDevCtx, ((DrvDispCtxConfig_t *)pDstDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_DETACH;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(pDstDevCtx);
        stQueryCfg.stInCfg.pInCfg      = pDstDevCtx;

        bRet = _DrvDispIfExecuteQuery(pSrcDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetTimeZone(void *pDevCtx, MHAL_DISP_TimeZone_t *pstTimeZone)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispTimeZone_t    stHalTimeZone;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_TIME_ZONE;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispTimeZone_t);
        stQueryCfg.stInCfg.pInCfg      = &stHalTimeZone;

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);

        if (bRet)
        {
            _DrvDispIfTransDeviceTimeZoneToMhal(pstTimeZone, &stHalTimeZone);
        }
        else
        {
            pstTimeZone->enType = E_MHAL_DISP_TIMEZONE_UNKONWN;
        }
    }

    return bRet;
}

u8 DrvDispIfDeviceSetTimeZoneConfig(void *pDevCtx, MHAL_DISP_TimeZoneConfig_t *pstTimeZoneCfg)
{
    u8                      bRet = 1;
    HalDispQueryConfig_t    stQueryCfg;
    HalDispTimeZoneConfig_t stTimeZoneCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDevCtx, ((DrvDispCtxConfig_t *)pDevCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispTimeZoneConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stTimeZoneCfg;

        _DrvDispIfTransDeviceTimeZoneCfgToHal(pstTimeZoneCfg, &stTimeZoneCfg);

        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetDisplayInfo(void *pDevCtx, MHAL_DISP_DisplayInfo_t *pstDisplayInfo)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDisplayInfo_t stHalDisplayInfo;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDisplayInfo_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalDisplayInfo;

    bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);

    if (bRet)
    {
        _DrvDispIfTransDeviceDisplayInfoToMhal(pstDisplayInfo, &stHalDisplayInfo);
    }
    else
    {
        memset(pstDisplayInfo, 0, sizeof(MHAL_DISP_DisplayInfo_t));
    }

    return bRet;
}

u8 DrvDispIfDeviceGetInstance(u32 u32DeviceId, void **pDevCtx)
{
    u8                  bRet       = TRUE;
    DrvDispCtxConfig_t *pstDispCtx = NULL;

    if (DrvDispCtxGetCurCtx(E_DISP_CTX_TYPE_DEVICE, u32DeviceId, &pstDispCtx))
    {
        *pDevCtx = (void *)pstDispCtx;
    }
    else
    {
        bRet     = FALSE;
        *pDevCtx = NULL;
    }
    return bRet;
}

u8 DrvDispIfDeviceSetPqConfig(void *pDevCtx, MHAL_DISP_PqConfig_t *pstPqCfg)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispPqConfig_t    stPqCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_PQ_SET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispPqConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stPqCfg;

    _DrvDispIfTransPqCfgToHal(pstPqCfg, &stPqCfg);

    if (pDevCtx == NULL
        && (stPqCfg.u32PqFlags == E_MHAL_DISP_PQ_FLAG_LOAD_BIN || stPqCfg.u32PqFlags == E_MHAL_DISP_PQ_FLAG_FREE_BIN
            || stPqCfg.u32PqFlags == E_MHAL_DISP_PQ_FLAG_SET_SRC_ID
            || stPqCfg.u32PqFlags == E_MHAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE))
    {
        HalDispIfInit();
        bRet = _DrvDispIfExecuteQueryWithoutCtx(&stQueryCfg);
    }
    else
    {
        bRet = _DrvDispIfExecuteQuery(pDevCtx, &stQueryCfg);
    }

    return bRet;
}

u8 DrvDispIfDeviceGetHwCount(u32 *pu32Count)
{
    u8                    bRet = 1;
    HalDispQueryConfig_t  stQueryCfg;
    HalDispHwInfoConfig_t stHalHwInfoCfg;
    DrvDispCtxConfig_t    stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_HW_INFO;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispHwInfoConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalHwInfoCfg;

    memset(&stHalHwInfoCfg, 0, sizeof(HalDispHwInfoConfig_t));
    stHalHwInfoCfg.eType = E_HAL_DISP_HW_INFO_DEVICE;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        *pu32Count = stHalHwInfoCfg.u32Count;
        bRet       = 1;
    }
    else
    {
        *pu32Count = 0;
        bRet       = 0;
    }
    return bRet;
}

u8 DrvDispIfSetDeviceConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg)
{
    HalDispQueryConfig_t    stQueryCfg;
    HalDispInternalConfig_t stInterCfg;
    DrvDispCtxConfig_t      stDispCtxCfg;
    DrvDispCtxConfig_t *    pstDispCtx;
    u8                      bRet;

    HalDispIfInit();

    memset(&stInterCfg, 0, sizeof(HalDispInternalConfig_t));
    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INTERCFG_SET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispInternalConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stInterCfg;

    stDispCtxCfg.u32ContainIdx    = u32DevId;
    stDispCtxCfg.enCtxType = E_DISP_CTX_TYPE_DEVICE;
    if (pstDevCfg->bCtx)
    {
        DrvDispIfDeviceGetInstance(u32DevId, (void *)&pstDispCtx);
        stDispCtxCfg.pstCtxContain = (pstDispCtx) ? pstDispCtx->pstCtxContain : NULL;
    }
    else
    {
        stDispCtxCfg.pstCtxContain = NULL;
    }
    _DrvDispIfTransDeviceConfigToHal(pstDevCfg, &stInterCfg);

    bRet = _DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg);
    return bRet;
}

u8 DrvDispIfGetDeviceConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg)
{
    HalDispQueryConfig_t    stQueryCfg;
    HalDispInternalConfig_t stInterCfg;
    DrvDispCtxConfig_t      stDispCtxCfg;
    u8                      bRet = 1;

    HalDispIfInit();

    memset(&stInterCfg, 0, sizeof(HalDispInternalConfig_t));
    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INTERCFG_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispInternalConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stInterCfg;

    stDispCtxCfg.u32ContainIdx        = u32DevId;
    stDispCtxCfg.enCtxType     = E_DISP_CTX_TYPE_DEVICE;
    stDispCtxCfg.pstCtxContain = NULL;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        DrvDispIfTransDeviceConfigToMHal(pstDevCfg, &stInterCfg);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d Get Config Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvDispIfDeviceGetCapabilityConfig(u32 u32DevId, MHAL_DISP_DeviceCapabilityConfig_t *pstDevCapCfg)
{
    u8                              bRet = 1;
    HalDispQueryConfig_t            stQueryCfg;
    HalDispDeviceCapabilityConfig_t stDevCapCfg;
    DrvDispCtxConfig_t              stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDeviceCapabilityConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stDevCapCfg;

    stDevCapCfg.u32DevId = u32DevId;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        _DrvDispIfTransDeviceCapabilityToMhal(pstDevCapCfg, &stDevCapCfg);
        bRet = 1;
    }
    else
    {
        bRet = 0;
    }
    return bRet;
}

u8 DrvDispIfDeviceGetInterfaceCapabilityConfig(u32                                    u32Interface,
                                               MHAL_DISP_InterfaceCapabilityConfig_t *pstInterfaceCapCfg)
{
    u8                                 bRet = 1;
    HalDispQueryConfig_t               stQueryCfg;
    HalDispInterfaceCapabilityConfig_t stInterfaceCapCfg;
    DrvDispCtxConfig_t                 stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispInterfaceCapabilityConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stInterfaceCapCfg;

    stInterfaceCapCfg.u32Interface = u32Interface;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        _DrvDispIfTransDeviceInterfaceCapabilityToMhal(pstInterfaceCapCfg, &stInterfaceCapCfg);
        bRet = 1;
    }
    else
    {
        bRet = 0;
    }
    return bRet;
}

// VideoLayer
u8 DrvDispIfVideoLayerCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32LayerId, void **pVidLayerCtx)
{
    u8                      bRet = TRUE;
    DrvDispCtxAllocConfig_t stCtxAllocCfg;
    HalDispQueryConfig_t    stQueryCfg;
    DrvDispCtxConfig_t *    pstDispCtx = NULL;

    DrvDispCtxInit();

    stCtxAllocCfg.enType             = E_DISP_CTX_TYPE_VIDLAYER;
    stCtxAllocCfg.u32DevId              = u32LayerId;
    stCtxAllocCfg.pstBindCtx         = NULL;
    stCtxAllocCfg.stMemAllcCfg.alloc = NULL;
    stCtxAllocCfg.stMemAllcCfg.free  = NULL;

    if (DrvDispCtxAllocate(&stCtxAllocCfg, &pstDispCtx) == FALSE)
    {
        bRet          = 0;
        *pVidLayerCtx = NULL;
        DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        *pVidLayerCtx = (void *)pstDispCtx;

        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_INIT;
        stQueryCfg.stInCfg.pInCfg      = NULL;
        stQueryCfg.stInCfg.u32CfgSize  = 0;
        bRet                           = _DrvDispIfExecuteQuery(pstDispCtx, &stQueryCfg);
    }

    return bRet;
}

u8 DrvDispIfVideoLayerDestoryInstance(void *pVidLayerCtx)
{
    u8                  bRet       = TRUE;
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pVidLayerCtx;

    if (DrvDispCtxFree(pstDispCtx))
    {
        if (DrvDispCtxIsAllFree())
        {
            DrvDispCtxDeInit();
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, DestroyInstance Fail\n", __FUNCTION__, __LINE__);
    }

    return bRet;
}

u8 DrvDispIfVideoLayerEnable(void *pVidLayerCtx, u8 bEnable)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(u8);
    stQueryCfg.stInCfg.pInCfg      = &bEnable;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerBind(void *pVidLayerCtx, void *pDevCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_BIND;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(pDevCtx);
    stQueryCfg.stInCfg.pInCfg      = pDevCtx;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerUnBind(void *pVidLayerCtx, void *pDevCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(pDevCtx);
    stQueryCfg.stInCfg.pInCfg      = pDevCtx;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerSetAttr(void *pVidLayerCtx, MHAL_DISP_VideoLayerAttr_t *pstAttr)
{
    u8                      bRet = 1;
    HalDispQueryConfig_t    stQueryCfg;
    HalDispVideoLayerAttr_t stHalAttrCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_ATTR;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispVideoLayerAttr_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalAttrCfg;

    _DrvDispIfTransVidLayerAttrToHal(pstAttr, &stHalAttrCfg);

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerBufferFire(void *pVidLayerCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerCheckBufferFired(void *pVidLayerCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerSetCompress(void *pVidLayerCtx, MHAL_DISP_CompressAttr_t *pstCompressAttr)
{
    u8                              bRet = 1;
    HalDispQueryConfig_t            stQueryCfg;
    HalDispVideoLayerCompressAttr_t stHalCompressCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispVideoLayerCompressAttr_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalCompressCfg;

    _DrvDispIfTransVidLayerCompressToHal(pstCompressAttr, &stHalCompressCfg);
    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerSetPriority(void *pVidLayerCtx, u32 u32Priority)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(u32);
    stQueryCfg.stInCfg.pInCfg      = &u32Priority;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfVideoLayerGetCapabilityConfig(u32 u32LayerId,
                                          MHAL_DISP_VideoLayerCapabilityConfig_t *pstVidLayerCapCfg)
{
    u8                                  bRet = 1;
    HalDispQueryConfig_t                stQueryCfg;
    HalDispVideoLayerCapabilityConfig_t stVidLayerCapCfg;
    DrvDispCtxConfig_t                  stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispVideoLayerCapabilityConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stVidLayerCapCfg;

    memset(&stVidLayerCapCfg, 0, sizeof(HalDispVideoLayerCapabilityConfig_t));
    stVidLayerCapCfg.u32VidLayerId = u32LayerId;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        _DrvDispIfTransVideoLayerCapabilityToMhal(pstVidLayerCapCfg, &stVidLayerCapCfg);
        bRet = 1;
    }
    else
    {
        bRet = 0;
    }
    return bRet;
}

// InputPort
u8 DrvDispIfInputPortCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, void *pVidLayerCtx, u32 u32PortId, void **pCtx)
{
    u8                      bRet = TRUE;
    DrvDispCtxAllocConfig_t stCtxAllocCfg;
    HalDispQueryConfig_t    stQueryCfg;
    DrvDispCtxConfig_t *    pstDispCtx = NULL;

    DrvDispCtxInit();

    stCtxAllocCfg.enType             = E_DISP_CTX_TYPE_INPUTPORT;
    stCtxAllocCfg.u32DevId              = u32PortId;
    stCtxAllocCfg.pstBindCtx         = (DrvDispCtxConfig_t *)pVidLayerCtx;
    stCtxAllocCfg.stMemAllcCfg.alloc = NULL;
    stCtxAllocCfg.stMemAllcCfg.free  = NULL;

    if (DrvDispCtxAllocate(&stCtxAllocCfg, &pstDispCtx) == FALSE)
    {
        bRet  = 0;
        *pCtx = NULL;
        DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        *pCtx = (void *)pstDispCtx;

        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_INIT;
        stQueryCfg.stInCfg.pInCfg      = NULL;
        stQueryCfg.stInCfg.u32CfgSize  = 0;
        bRet                           = _DrvDispIfExecuteQuery(pstDispCtx, &stQueryCfg);
    }

    return bRet;
}

u8 DrvDispIfInputPortDestroyInstance(void *pInputPortCtx)
{
    u8                  bRet       = TRUE;
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pInputPortCtx;

    if (DrvDispCtxFree(pstDispCtx))
    {
        if (DrvDispCtxIsAllFree())
        {
            DrvDispCtxDeInit();
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, DestroyInstance Fail\n", __FUNCTION__, __LINE__);
    }

    return bRet;
}

u8 DrvDispIfInputPortRotate(void *pInputPortCtx, MHAL_DISP_RotateConfig_t *pstRotateCfg)
{
    u8                       bRet = 1;
    HalDispQueryConfig_t     stQueryCfg;
    HalDispInputPortRotate_t stRotate;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_ROTATE;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispInputPortRotate_t);
    stQueryCfg.stInCfg.pInCfg      = &stRotate;

    _DrvDispIfTransInputPortRotateToHal(pstRotateCfg, &stRotate);
    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortSetCropAttr(void *pInputPortCtx, MHAL_DISP_VidWinRect_t *pstCropAttr)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;
    HalDispVidWinRect_t  stHalCropAttr;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_CROP;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispVidWinRect_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalCropAttr;

    _DrvDispIfTransInputPortCropAttrToHal(pstCropAttr, &stHalCropAttr);
    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}
u8 DrvDispIfInputPortFlip(void *pInputPortCtx, MHAL_DISP_VideoFrameData_t *pstVideoFrameData)
{
    u8                      bRet = 1;
    HalDispQueryConfig_t    stQueryCfg;
    HalDispVideoFrameData_t stHalFrameData;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_FLIP;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispVideoFrameData_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalFrameData;

    _DrvDispIfTransInputPortFrameDataToHal(pstVideoFrameData, &stHalFrameData);
    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortSetRingBuffAttr(void *pInputPortCtx, MHAL_DISP_RingBufferAttr_t *pstRingBufAttr)
{
    u8                    bRet = 1;
    HalDispQueryConfig_t  stQueryCfg;
    HalDispRingBuffAttr_t stRingBuffAttr;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispRingBuffAttr_t);
    stQueryCfg.stInCfg.pInCfg      = &stRingBuffAttr;

    _DrvDispIfTransInputPortRingBufAttrToHal(pstRingBufAttr, &stRingBuffAttr);
    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortEnable(void *pInputPortCtx, u8 bEnable)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_ENABLE;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(u8);
    stQueryCfg.stInCfg.pInCfg      = &bEnable;

    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortSetAttr(void *pInputPortCtx, MHAL_DISP_InputPortAttr_t *pstAttr)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispInputPortAttr_t stHalAttr;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_ATTR;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispInputPortAttr_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalAttr;

    _DrvDispIfTransInputPortAttrToHal(pstAttr, &stHalAttr);
    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortShow(void *pInputPortCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_SHOW;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortHide(void *pInputPortCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_HIDE;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortAttrBegin(void *pVidLayerCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_BEGIN;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortAttrEnd(void *pVidLayerCtx)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_END;
    stQueryCfg.stInCfg.u32CfgSize  = 0;
    stQueryCfg.stInCfg.pInCfg      = NULL;

    bRet = _DrvDispIfExecuteQuery(pVidLayerCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfInputPortSetImiAddr(void *pInputPortCtx, ss_phys_addr_t paImiAddr)
{
    u8                   bRet = 1;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_INPUTPORT_IMIADDR;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(ss_phys_addr_t);
    stQueryCfg.stInCfg.pInCfg      = &paImiAddr;

    bRet = _DrvDispIfExecuteQuery(pInputPortCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfSetDbgLevel(void *p)
{
    g_u32DispDbgLevel = *((u32 *)p);
    return TRUE;
}

u8 DrvDispIfDmaCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DmaId, void **pDmaCtx)
{
    u8                      bRet = TRUE;
    DrvDispCtxAllocConfig_t stCtxAllocCfg;
    HalDispQueryConfig_t    stQueryCfg;
    DrvDispCtxConfig_t *    pstDispCtx = NULL;

    DrvDispCtxInit();

    stCtxAllocCfg.enType             = E_DISP_CTX_TYPE_DMA;
    stCtxAllocCfg.u32DevId              = u32DmaId;
    stCtxAllocCfg.pstBindCtx         = NULL;
    stCtxAllocCfg.stMemAllcCfg.alloc = NULL;
    stCtxAllocCfg.stMemAllcCfg.free  = NULL;

    if (DrvDispCtxAllocate(&stCtxAllocCfg, &pstDispCtx) == FALSE)
    {
        bRet     = 0;
        *pDmaCtx = NULL;
        DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        *pDmaCtx = (void *)pstDispCtx;

        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_INIT;
        stQueryCfg.stInCfg.pInCfg      = NULL;
        stQueryCfg.stInCfg.u32CfgSize  = 0;
        bRet                           = _DrvDispIfExecuteQuery((void *)pstDispCtx, &stQueryCfg);
    }

    return bRet;
}

u8 DrvDispIfDmaDestoryInstance(void *pDmaCtx)
{
    u8                   bRet       = TRUE;
    DrvDispCtxConfig_t * pstDispCtx = (DrvDispCtxConfig_t *)pDmaCtx;
    HalDispQueryConfig_t stQueryCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_DEINIT;
    stQueryCfg.stInCfg.pInCfg      = NULL;
    stQueryCfg.stInCfg.u32CfgSize  = 0;

    if (_DrvDispIfExecuteQuery(pstDispCtx, &stQueryCfg))
    {
        if (DrvDispCtxFree(pstDispCtx))
        {
            if (DrvDispCtxIsAllFree())
            {
                DrvDispCtxDeInit();
            }
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, DestroyInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Dma DeInit Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 DrvDispIfDmaBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispDmaBindConfig_t stDmaBindCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_BIND;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDmaBindConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stDmaBindCfg;

    _DrvDispIfTransDmaBindToHal(pstDmaBindCfg, &stDmaBindCfg);
    bRet = _DrvDispIfExecuteQuery(pDmaCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfDmaUnBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispDmaBindConfig_t stDmaBindCfg;

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_UNBIND;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDmaBindConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stDmaBindCfg;

    _DrvDispIfTransDmaBindToHal(pstDmaBindCfg, &stDmaBindCfg);
    bRet = _DrvDispIfExecuteQuery(pDmaCtx, &stQueryCfg);

    return bRet;
}

u8 DrvDispIfDmaSetAttr(void *pDmaCtx, MHAL_DISP_DmaAttrConfig_t *pstDmaAttrCfg)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispDmaAttrConfig_t stHalDmaAttrCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDmaCtx, ((DrvDispCtxConfig_t *)pDmaCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_ATTR;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(MHAL_DISP_DmaAttrConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalDmaAttrCfg;

        _DrvDispIfTransDmaAttrToHal(pstDmaAttrCfg, &stHalDmaAttrCfg);

        bRet = _DrvDispIfExecuteQuery(pDmaCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDmaSetBufferAttr(void *pDmaCtx, MHAL_DISP_DmaBufferAttrConfig_t *pstDmaBufferAttrCfg)
{
    u8                           bRet = 1;
    HalDispQueryConfig_t         stQueryCfg;
    HalDispDmaBufferAttrConfig_t stHalDmaBufferAttrCfg;

    if (DrvDispCtxSetCurCtx((DrvDispCtxConfig_t *)pDmaCtx, ((DrvDispCtxConfig_t *)pDmaCtx)->u32ContainIdx) == FALSE)
    {
        bRet = FALSE;
    }
    else
    {
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_BUFFERATTR;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(MHAL_DISP_DmaBufferAttrConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stHalDmaBufferAttrCfg;

        _DrvDispIfTransDmaBufferAttrToHal(pstDmaBufferAttrCfg, &stHalDmaBufferAttrCfg);

        bRet = _DrvDispIfExecuteQuery(pDmaCtx, &stQueryCfg);
    }
    return bRet;
}

u8 DrvDispIfDmaGetHwCount(u32 *pu32Count)
{
    u8                    bRet = 1;
    HalDispQueryConfig_t  stQueryCfg;
    HalDispHwInfoConfig_t stHalHwInfoCfg;
    DrvDispCtxConfig_t    stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_HW_INFO;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispHwInfoConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalHwInfoCfg;

    memset(&stHalHwInfoCfg, 0, sizeof(HalDispHwInfoConfig_t));
    stHalHwInfoCfg.eType = E_HAL_DISP_HW_INFO_DMA;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        *pu32Count = stHalHwInfoCfg.u32Count;
        bRet       = 1;
    }
    else
    {
        *pu32Count = 0;
        bRet       = 0;
    }
    return bRet;
}

u8 DrvDispIfDmaGetCapabilityConfig(u32 u32DmaId, MHAL_DISP_DmaCapabiliytConfig_t *pstDmaCapCfg)
{
    u8                            bRet = 1;
    HalDispQueryConfig_t          stQueryCfg;
    HalDispDmaCapabilitytConfig_t stDmaCapCfg;
    DrvDispCtxConfig_t            stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DMA_CAPABILITY_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDmaCapabilitytConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stDmaCapCfg;

    memset(&stDmaCapCfg, 0, sizeof(HalDispDmaCapabilitytConfig_t));
    stDmaCapCfg.u32DmaId = u32DmaId;

    if (_DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg))
    {
        _DrvDispIfTransDmaCapabilityToMhal(pstDmaCapCfg, &stDmaCapCfg);
        bRet = 1;
    }
    else
    {
        bRet = 0;
    }
    return bRet;
}

u8 DrvDispIfClkOn(void)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispClkInitConfig_t stHalClkInitCfg;
    DrvDispCtxConfig_t     stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_CLK_INIT;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispClkInitConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalClkInitCfg;

    stHalClkInitCfg.bEn = 1;

    bRet = _DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg);
    return bRet;
}

u8 DrvDispIfClkOff(void)
{
    u8                     bRet = 1;
    HalDispQueryConfig_t   stQueryCfg;
    HalDispClkInitConfig_t stHalClkInitCfg;
    DrvDispCtxConfig_t     stDispCtxCfg;

    HalDispIfInit();

    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_CLK_INIT;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispClkInitConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stHalClkInitCfg;

    stHalClkInitCfg.bEn = 0;

    bRet = _DrvDispIfExecuteQuery(&stDispCtxCfg, &stQueryCfg);
    return bRet;
}

//-------------------------------------------------------------------------------------------------
