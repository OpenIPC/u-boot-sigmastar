/*
* hal_disp_st.h- Sigmastar
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

#ifndef _HAL_DISP_ST_H_
#define _HAL_DISP_ST_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_INTF_HDMI  (0x00000001)
#define HAL_DISP_INTF_CVBS  (0x00000002)
#define HAL_DISP_INTF_VGA   (0x00000004)
#define HAL_DISP_INTF_YPBPR (0x00000008)
#define HAL_DISP_INTF_LCD   (0x00000010)

#define HAL_DISP_INTF_TTL             (0x00010000)
#define HAL_DISP_INTF_MIPIDSI         (0x00020000)
#define HAL_DISP_INTF_BT656           (0x00040000)
#define HAL_DISP_INTF_BT601           (0x00080000)
#define HAL_DISP_INTF_BT1120          (0x00100000)
#define HAL_DISP_INTF_MCU             (0x00200000)
#define HAL_DISP_INTF_SRGB            (0x00400000)
#define HAL_DISP_INTF_MCU_NOFLM       (0x00800000)
#define HAL_DISP_INTF_BT1120_DDR      (0x01000000)
#define HAL_DISP_INTF_PNL             (0x01FF0000)
#define HAL_DISP_INTF_HW_LCD          (0x01FD0000)
#define DISP_DMA_IS_FRAME_MODE(eMode) ((eMode) == E_HAL_DISP_DMA_OUTPUT_MODE_FRAME)
#define DISP_DMA_IS_RING_MODE(eMode)  ((eMode) == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
#define DISP_OUTDEV_IS_CVBS(eMode)    ((eMode)&HAL_DISP_INTF_CVBS)
#define DISP_OUTDEV_IS_PNL(eMode)    ((eMode)&HAL_DISP_INTF_PNL)
#define DISP_OUTDEV_IS_HDMI(eMode)    ((eMode)&HAL_DISP_INTF_HDMI)
#define DISP_OUTDEV_IS_VGA(eMode)     ((eMode)&HAL_DISP_INTF_VGA)
#define DISP_FPLL_DEVICE_FLAG(Dev)    ((Dev < HAL_DISP_DEVICE_MAX) ? (1 << Dev) : 0)

#define HAL_DISP_DAC_RESET_OFF              0
#define HAL_DISP_DAC_RESET_ON               1
#define HAL_DISP_DAC_RESET_FORCE            2

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_QUERY_RET_OK = 0,
    E_HAL_DISP_QUERY_RET_CFGERR,
    E_HAL_DISP_QUERY_RET_NOTSUPPORT,
    E_HAL_DISP_QUERY_RET_NONEED,
    E_HAL_DISP_QUERY_RET_IMPLICIT_ERR,
} HalDispQueryRet_e;

typedef enum
{
    E_HAL_DISP_QUERY_DEVICE_INIT,
    E_HAL_DISP_QUERY_DEVICE_DEINIT,
    E_HAL_DISP_QUERY_DEVICE_ENABLE,
    E_HAL_DISP_QUERY_DEVICE_ATTACH,
    E_HAL_DISP_QUERY_DEVICE_DETACH,
    E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR,
    E_HAL_DISP_QUERY_DEVICE_INTERFACE,
    E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING,
    E_HAL_DISP_QUERY_DEVICE_VGA_PARAM,
    E_HAL_DISP_QUERY_DEVICE_COLORTEMP,
    E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM,
    E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM_GET,
    E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM,
    E_HAL_DISP_QUERY_DEVICE_LCD_PARAM,
    E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM,
    E_HAL_DISP_QUERY_DEVICE_TIME_ZONE,
    E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO,
    E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG,
    E_HAL_DISP_QUERY_VIDEOLAYER_INIT,
    E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE,
    E_HAL_DISP_QUERY_VIDEOLAYER_BIND,
    E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND,
    E_HAL_DISP_QUERY_VIDEOLAYER_ATTR,
    E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS,
    E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY,
    E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE,
    E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE,
    E_HAL_DISP_QUERY_INPUTPORT_INIT,
    E_HAL_DISP_QUERY_INPUTPORT_ENABLE,
    E_HAL_DISP_QUERY_INPUTPORT_ATTR,
    E_HAL_DISP_QUERY_INPUTPORT_SHOW,
    E_HAL_DISP_QUERY_INPUTPORT_HIDE,
    E_HAL_DISP_QUERY_INPUTPORT_BEGIN,
    E_HAL_DISP_QUERY_INPUTPORT_END,
    E_HAL_DISP_QUERY_INPUTPORT_FLIP,
    E_HAL_DISP_QUERY_INPUTPORT_ROTATE,
    E_HAL_DISP_QUERY_INPUTPORT_CROP,
    E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR,
    E_HAL_DISP_QUERY_INPUTPORT_IMIADDR,
    E_HAL_DISP_QUERY_CLK_SET,
    E_HAL_DISP_QUERY_CLK_GET,
    E_HAL_DISP_QUERY_PQ_SET,
    E_HAL_DISP_QUERY_PQ_GET,
    E_HAL_DISP_QUERY_DRVTURNING_SET,
    E_HAL_DISP_QUERY_DRVTURNING_GET,
    E_HAL_DISP_QUERY_DBGMG_GET,
    E_HAL_DISP_QUERY_REG_ACCESS,
    E_HAL_DISP_QUERY_REG_FLIP,
    E_HAL_DISP_QUERY_REG_WAITDONE,
    E_HAL_DISP_QUERY_DMA_INIT,
    E_HAL_DISP_QUERY_DMA_DEINIT,
    E_HAL_DISP_QUERY_DMA_BIND,
    E_HAL_DISP_QUERY_DMA_UNBIND,
    E_HAL_DISP_QUERY_DMA_ATTR,
    E_HAL_DISP_QUERY_DMA_BUFFERATTR,
    E_HAL_DISP_QUERY_HW_INFO,
    E_HAL_DISP_QUERY_CLK_INIT,
    E_HAL_DISP_QUERY_CMDQINF,
    E_HAL_DISP_QUERY_INTERCFG_SET,
    E_HAL_DISP_QUERY_INTERCFG_GET,
    E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET,
    E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET,
    E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET,
    E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET,
    E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET,
    E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET,
    E_HAL_DISP_QUERY_DMA_CAPABILITY_GET,
    E_HAL_DISP_QUERY_MAX,
} HalDispQueryType_e;
typedef enum
{
    E_HAL_DISP_OUTPUT_PAL = 0,   ///< PAL, 720X576  i 50Hz
    E_HAL_DISP_OUTPUT_NTSC,      ///< NTSC 704X480 P 60Hz
    E_HAL_DISP_OUTPUT_960H_PAL,  ///< ITU-R BT.1302 960 X 576  50 Hz (interlaced)
    E_HAL_DISP_OUTPUT_960H_NTSC, ///< ITU-R BT.1302 960 X 480  60 Hz (interlaced)

    E_HAL_DISP_OUTPUT_720P24,  ///< 1280 X 720 24Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_720P25,  ///< 1280 X 720 25Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_720P29D97,  ///< 1280 X  720 29.97Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_720P30,  ///< 1280 X 720 30Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_720P50,  ///< 1280 X 720 50Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_720P59D94,  ///< 1280 X  720 59.94Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_720P60,  ///< 1280 X 720 60Hz (non-interflaced)

    E_HAL_DISP_OUTPUT_1080P23D98, ///< 1920 X 1080 23.98Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1080P24, ///< 1920 X 1080 24Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1080P25, ///< 1920 X 1080 25Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1080P29D97, ///< 1920 X 1080 29.97Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1080P30, ///< 1920 X 1080 30Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1080P50, ///< 1920 X 1080 50Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1080P59D94, ///< 1920 X 1080 59.94Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1080P60, ///< 1920 X 1080 60Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1080I50, ///< 1920 X 1080 50Hz (interflaced)
    E_HAL_DISP_OUTPUT_1080I60, ///< 1920 X 1080 60Hz (interflaced)

    E_HAL_DISP_OUTPUT_2560X1440_30,   ///< 2560 X 1440 30Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_2560X1440_50,   ///< 2560 X 1440 50Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_2560X1440_60,   ///< 2560 X 1440 60Hz (non-interlaced)

    E_HAL_DISP_OUTPUT_3840X2160_24,   ///< 3840 X 2160 24Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_3840X2160_25,   ///< 3840 X 2160 25Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_3840X2160_29D97, ///< 3840 X 2160 29.97Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_3840X2160_30,   ///< 3840 X 2160 30Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_3840X2160_60,   ///< 3840 X 2160 60Hz (non-interlaced)


    E_HAL_DISP_OUTPUT_640X480_60,     ///<  640 X 480 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_480P60,         ///<  720 X 480 60Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_576P50,         ///<  720 X 576 50Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_800X600_60,     ///<  800 X 480 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_848X480_60,     ///<  848 X 480 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1024X768_60,    ///< 1920 X 768 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1280X768_60,    ///< 1280 X 768 60Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1280X800_60,    ///< 1280 X  800 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1280X960_60,    ///< 1280 X 960 60Hz (non-interflaced)
    E_HAL_DISP_OUTPUT_1280X1024_60,   ///< 1280 X 1024 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1360X768_60,    ///< 1360 X 768 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1366X768_60,    ///< 1366 X  768 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1400X1050_60,   ///< 1400 X 1050 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1440X900_60,    ///< 1440 X  900 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1600X900_60,    ///< 1600 X 900 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1600X1200_60,   ///< 1600 X 1200 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1680X1050_60,   ///< 1680 X 1050 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1920X1200_60,   ///< 1920 X 1200 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1920X1440_60,   ///< 1920 X 1440 60Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_1920X2160_30,   ///< 1920 X 2160 30Hz (non-interlaced)
    E_HAL_DISP_OUTPUT_2560X1600_60,   ///< 2560 X 1600 30Hz (non-interlaced)

    E_HAL_DISP_OUTPUT_USER,           ///< User defined tiing
    E_HAL_DISP_OUTPUT_MAX,            ///< MaX number of type

} HalDispDeviceTiming_e;

typedef enum
{
    E_HAL_DISP_CSC_MATRIX_BYPASS = 0, /* do not change color space */

    E_HAL_DISP_CSC_MATRIX_BT601_TO_BT709, /* change color space from BT.601 to BT.709 */
    E_HAL_DISP_CSC_MATRIX_BT709_TO_BT601, /* change color space from BT.709 to BT.601 */

    E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC, /* change color space from BT.601 to RGB */
    E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC, /* change color space from BT.709 to RGB */

    E_HAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC, /* change color space from RGB to BT.601 */
    E_HAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC, /* change color space from RGB to BT.709 */

    E_HAL_DISP_CSC_MATRIX_USER,

    E_HAL_DISP_CSC_MATRIX_MAX
} HalDispCscmatrix_e;

typedef enum
{
    E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV = 0,
    E_HAL_DISP_PIXEL_FRAME_ARGB8888,
    E_HAL_DISP_PIXEL_FRAME_ABGR8888,
    E_HAL_DISP_PIXEL_FRAME_BGRA8888,

    E_HAL_DISP_PIXEL_FRAME_RGB565,
    E_HAL_DISP_PIXEL_FRAME_ARGB1555,
    E_HAL_DISP_PIXEL_FRAME_ARGB4444,

    E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422,
    E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_420,
    // mdwin/mgwin
    E_HAL_DISP_PIXEL_FRAME_YUV_MST_420,

    E_HAL_DISP_PIXEL_FRAME_FORMAT_MAX,
} HalDispPixelFormat_e;

typedef enum
{
    E_HAL_DISP_COMPRESS_MODE_NONE,  // no compress
    E_HAL_DISP_COMPRESS_MODE_SEG,   // compress unit is 256 bytes as a segment
    E_HAL_DISP_COMPRESS_MODE_LINE,  // compress unit is the whole line
    E_HAL_DISP_COMPRESS_MODE_FRAME, // compress unit is the whole frame
    E_HAL_DISP_COMPRESS_MODE_TO_8BIT,
    E_HAL_DISP_COMPRESS_MODE_TO_6BIT,
    E_HAL_DISP_COMPRESS_MODE_AFBC,
    E_HAL_DISP_COMPRESS_MODE_SFBC0, // DSC
    E_HAL_DISP_COMPRESS_MODE_SFBC1, // Encoder Fbc
    E_HAL_DISP_COMPRESS_MODE_SFBC2, // Decoder Fbc
    E_HAL_DISP_COMPRESS_MODE_BUTT,  // number
    E_HAL_DISP_COMPRESS_MODE_MAX,   // number
} HalDispPixelCompressMode_e;

typedef enum
{
    E_HAL_DISP_TILE_MODE_NONE  = 0x00,
    E_HAL_DISP_TILE_MODE_16X16 = 0x01,
    E_HAL_DISP_TILE_MODE_16X32 = 0x02,
    E_HAL_DISP_TILE_MODE_32X16 = 0x03,
    E_HAL_DISP_TILE_MODE_32X32 = 0x04,
} HalDispTileMode_e;

typedef enum
{
    E_HAL_DISP_MMAP_XC_MAIN = 0, /* XC Main buffer */
    E_HAL_DISP_MMAP_XC_MENULOAD, /* XC Menuload buffer */
    E_HAL_DISP_MMAP_MAX,
} HalDispMmapType_e;

typedef enum
{
    E_HAL_DISP_ROTATE_NONE,
    E_HAL_DISP_ROTATE_90,
    E_HAL_DISP_ROTATE_180,
    E_HAL_DISP_ROTATE_270,
    E_HAL_DISP_ROTATE_NUM,
} HalDispRotateMode_e;

typedef enum
{
    E_HAL_DISP_TIMEZONE_NONE       = 0x00,
    E_HAL_DISP_TIMEZONE_SYNC       = 0x01,
    E_HAL_DISP_TIMEZONE_BACKPORCH  = 0x02,
    E_HAL_DISP_TIMEZONE_ACTIVE     = 0x03,
    E_HAL_DISP_TIMEZONE_FRONTPORCH = 0x04,
    E_HAL_DISP_TIMEZONE_NUM        = 0x05,
} HalDispTimeZoneType_e;

typedef enum
{
    E_HAL_DISP_DRV_TURNING_RGB,
    E_HAL_DISP_DRV_TRUNING_NUM,
} HalDispDrvTruningType_e;

typedef enum
{
    E_HAL_DISP_DMA_INPUT_DEVICE_FRONT,
    E_HAL_DISP_DMA_INPUT_DEVICE_BACK,
    E_HAL_DISP_DMA_INPUT_NUM,
} HalDispDmaInputType_e;

typedef enum
{
    E_HAL_DISP_DMA_ACCESS_TYPE_IMI,
    E_HAL_DISP_DMA_ACCESS_TYPE_EMI,
    E_HAL_DISP_DMA_ACCESS_TYPE_NUM,
} HalDispDmaAccessType_e;

typedef enum
{
    E_HAL_DISP_DMA_OUTPUT_MODE_FRAME,
    E_HAL_DISP_DMA_OUTPUT_MODE_RING,
    E_HAL_DISP_DMA_OUTPUT_MODE_NUM,
} HalDispDmaOutputMode_e;

typedef enum
{
    E_HAL_DISP_DMA_PORT0    = 0,
    E_HAL_DISP_DMA_PORT_NUM = 1,
} HalDispDmaPortType_e;

typedef enum
{
    E_HAL_DISP_DMA_PIX_FMT_YUYV,      ///< pixel format: 422Pack  : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_YVYU,      ///< pixel format: 422Pack  : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_UYVY,      ///< pixel format: 422Pack  : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_VYUY,      ///< pixel format: 422Pack  : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_SP420,     ///< pixel format: YCSep420 : 2 plane
    E_HAL_DISP_DMA_PIX_FMT_SP420NV21, ///< pixel format: YCSep420 : 2 plane
    E_HAL_DISP_DMA_PIX_FMT_SP420TILE,
    E_HAL_DISP_DMA_PIX_FMT_SP422,  ///< pixel format: YC422    : 2 plane
    E_HAL_DISP_DMA_PIX_FMT_P422,   ///< pixel format: YUVSep422: 3 plane
    E_HAL_DISP_DMA_PIX_FMT_P420,   ///< pixel format: YUVSep420: 3 plane
    E_HAL_DISP_DMA_PIX_FMT_ARGB,   ///< pixel format: ARGB8888 : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_ABGR,   ///< pixel format: ABGR8888 : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_RGBA,   ///< pixel format: RGBA8888 : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_BGRA,   ///< pixel format: BGRA8888 : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_RGB565, ///< pixel format:RGB565 : 1 plane
    E_HAL_DISP_DMA_PIX_FMT_NUM
} HalDispDmaPixelFormat_e;

typedef enum
{
    E_HAL_DISP_HW_INFO_DEVICE,
    E_HAL_DISP_HW_INFO_VIDEOLAYER,
    E_HAL_DISP_HW_INFO_INPUTPORT,
    E_HAL_DISP_HW_INFO_DMA,
    E_HAL_DISP_HW_INFO_NUM,
} HalDispHwInfoType_e;

typedef enum
{
    E_HAL_DISP_INTER_CFG_NONE       = 0x00000000,
    E_HAL_DISP_INTER_CFG_BOOTLOGO   = 0x00000001,
    E_HAL_DISP_INTER_CFG_COLORID    = 0x00000002,
    E_HAL_DISP_INTER_CFG_GOPBLENDID = 0x00000004,
    E_HAL_DISP_INTER_CFG_DISP_PAT   = 0x00000008,
    E_HAL_DISP_INTER_CFG_RST_MOP    = 0x00000010,
    E_HAL_DISP_INTER_CFG_RST_DISP   = 0x00000020,
    E_HAL_DISP_INTER_CFG_RST_DAC    = 0x00000040,
    E_HAL_DISP_INTER_CFG_RST_HDMITX = 0x00000080,
    E_HAL_DISP_INTER_CFG_RST_LCD    = 0x00000100,
    E_HAL_DISP_INTER_CFG_RST_DACAFF = 0x00000200,
    E_HAL_DISP_INTER_CFG_RST_DACSYN = 0x00000400,
    E_HAL_DISP_INTER_CFG_RST_FPLL   = 0x00000800,
    E_HAL_DISP_INTER_CFG_CSC_EN     = 0x00001000,
    E_HAL_DISP_INTER_CFG_CSC_MD     = 0x00002000,
    E_HAL_DISP_INTER_CFG_MOP_Y_THRD = 0x00004000,
    E_HAL_DISP_INTER_CFG_MOP_C_THRD = 0x00008000,
    E_HAL_DISP_INTER_CFG_RST_DACALL = 0x00000640,
} HalDispInternalConfigType_e;

typedef enum
{
    E_HAL_DISP_PQ_FLAG_NONE                  = 0x0000,
    E_HAL_DISP_PQ_FLAG_LOAD_BIN              = 0x0001,
    E_HAL_DISP_PQ_FLAG_SET_SRC_ID            = 0x0002,
    E_HAL_DISP_PQ_FLAG_PROCESS               = 0x0003,
    E_HAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE = 0x0004,
    E_HAL_DISP_PQ_FLAG_FREE_BIN              = 0x0005,
    E_HAL_DISP_PQ_FLAG_BYPASS                = 0x0006,
} HalDispPqFlagType_e;
typedef enum
{
    E_HAL_DISP_PQ_LOAD_SETTING_AUTO,
    E_HAL_DISP_PQ_LOAD_SETTING_MANUAL,
} HalDispPqLoadSettingType_e;

typedef enum
{
    E_HAL_DISP_GAMMA_R,
    E_HAL_DISP_GAMMA_G,
    E_HAL_DISP_GAMMA_B,
    E_HAL_DISP_GAMMA_TYPE,
} HalDispGammaType_e;

typedef enum
{
    E_HAL_DISP_DEV_PQ_MACE,
    E_HAL_DISP_DEV_PQ_HPQ,
} HalDispDevicePqType_e;

typedef enum
{
    E_HAL_DISP_VIDLAYER_NONE  = 0x0000,
    E_HAL_DISP_VIDLAYER_MOPG0 = 0x0001,
    E_HAL_DISP_VIDLAYER_MOPS0 = 0x0002,
    E_HAL_DISP_VIDLAYER_MOPG1 = 0x0004,
    E_HAL_DISP_VIDLAYER_MOPS1 = 0x0008,
    E_HAL_DISP_VIDLAYER_MOPG2 = 0x0010,
    E_HAL_DISP_VIDLAYER_MOPS2 = 0x0020,
} HalDispVideoLayerType_e;
typedef enum
{
    E_HAL_DISP_DEVICE_RGB888,
    E_HAL_DISP_DEVICE_RGB666,
    E_HAL_DISP_DEVICE_RGB565,
    E_HAL_DISP_DEVICE_BGR565,
    E_HAL_DISP_DEVICE_RGB_TYPE,
} HaldispDeviceRgbType_e;

typedef enum
{
    E_HAL_DISP_DEVICE_NONE = 0x0000,
    E_HAL_DISP_DEVICE_0    = 0x0001,
    E_HAL_DISP_DEVICE_1    = 0x0002,
    E_HAL_DISP_DEVICE_2    = 0x0004,
} HaldispDeviceType_e;
typedef enum
{
    E_HAL_DISP_CRC16_OFF,
    E_HAL_DISP_CRC16_EXT,
    E_HAL_DISP_CRC16_OVERWRITE,
} HalDispCrc16Type_e;

typedef enum
{
    E_HAL_DISP_DEV_SYNTH_HDMI,
    E_HAL_DISP_DEV_SYNTH_VGA,
    E_HAL_DISP_DEV_SYNTH_CVBS,
    E_HAL_DISP_DEV_SYNTH_LCD,
    E_HAL_DISP_DEV_SYNTH_MAX,
} HalDispDeviceSynthType_e;

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalDispQueryType_e enQueryType;
    void *             pInCfg;
    u32                u32CfgSize;
} HalDispQueryInConfig_t;

typedef struct
{
    u8  u8bEn[3];
    u8  u8fifosize[3];
    u8  u8fifostart[3];
    u16 u16Width[3];
    u16 u16Height[3];
    u16 u16Pixel[3];
    u16 u16Stride[3];
    u16 u16BuffHeight[3];
} HalDispFormatRela_t;

typedef struct
{
    HalDispQueryRet_e enQueryRet;
    void (*pSetFunc)(void *, void *);
} HalDispQueryOutConfig_t;

typedef struct
{
    HalDispQueryInConfig_t  stInCfg;
    HalDispQueryOutConfig_t stOutCfg;
} HalDispQueryConfig_t;

typedef struct
{
    u16 u16HsyncWidth;
    u16 u16HsyncBackPorch;
    u16 u16Hstart;
    u16 u16Hactive;
    u16 u16Htotal;

    u16 u16VsyncWidth;
    u16 u16VsyncBackPorch;
    u16 u16Vstart;
    u16 u16Vactive;
    u16 u16Vtotal;
    u16 u16Fps;

    u16 u16SscStep;
    u16 u16SscSpan;
    u32 u32VSyncPeriod;
    u32 u32VBackPorchPeriod;
    u32 u32VActivePeriod;
    u32 u32VFrontPorchPeriod;
    u32 u32Dclk;
} HalDispDeviceTimingConfig_t;

typedef struct
{
    u8 bSynm;   /* sync mode(0:timing,as BT.656; 1:signal,as LCD) */
    u8 bIop;    /* interlaced or progressive display(0:i; 1:p) */
    u8 u8Intfb; /* interlace bit width while output */

    u16 u16Vact; /* vertical active area */
    u16 u16Vbb;  /* vertical back blank porch */
    u16 u16Vfb;  /* vertical front blank porch */

    u16 u16Hact; /* herizontal active area */
    u16 u16Hbb;  /* herizontal back blank porch */
    u16 u16Hfb;  /* herizontal front blank porch */
    u16 u16Hmid; /* bottom herizontal active area */

    u16 u16Bvact; /* bottom vertical active area */
    u16 u16Bvbb;  /* bottom vertical back blank porch */
    u16 u16Bvfb;  /* bottom vertical front blank porch */

    u16 u16Hpw; /* horizontal pulse width */
    u16 u16Vpw; /* vertical pulse width */

    u8  bIdv; /* inverse data valid of output */
    u8  bIhs; /* inverse horizontal synch signal */
    u8  bIvs; /* inverse vertical synch signal */
    u32 u32FrameRate;
} HalDispSyncInfo_t;

typedef struct
{
    HalDispDeviceTiming_e       eTimeType;
    HalDispDeviceTimingConfig_t stDeviceTimingCfg;
} HalDispDeviceTimingInfo_t;

typedef struct
{
    HalDispCscmatrix_e eCscMatrix;
    u32                u32Luma;       /* luminance:   0 ~ 100 default: 50 */
    u32                u32Contrast;   /* contrast :   0 ~ 100 default: 50 */
    u32                u32Hue;        /* hue      :   0 ~ 100 default: 50 */
    u32                u32Saturation; /* saturation:  0 ~ 100 default: 50 */
} HalDispCsc_t;

typedef struct
{
    u8 bEnable; /* color space */
    u32          u32Gain; /* current gain of VGA signals. [0, 64). default:0x30 */
    HalDispCsc_t stCsc; /* color space */
    u32          u32Sharpness;
} HalDispDeviceParam_t;

typedef struct
{
    u8  bEn;
    u16 u16EntryNum;
    u8 *pu8ColorR;
    u8 *pu8ColorG;
    u8 *pu8ColorB;
} HalDispGammaParam_t;

typedef struct
{
    u16 u16RedOffset;
    u16 u16GreenOffset;
    u16 u16BlueOffset;

    u16 u16RedColor;   // 00~FF, 0x80 is no change
    u16 u16GreenColor; // 00~FF, 0x80 is no change
    u16 u16BlueColor;  // 00~FF, 0x80 is no change
} HalDispColorTemp_t;

typedef struct
{
    u16 u16X;
    u16 u16Y;
    u16 u16Width;
    u16 u16Height;
} HalDispVidWinRect_t;

typedef struct
{
    u32 u32Width;
    u32 u32Height;
} HalDispVideoLyaerSize_t;

typedef struct
{
    HalDispVidWinRect_t     stVidLayerDispWin; /* Display resolution */
    HalDispVideoLyaerSize_t stVidLayerSize;    /* Canvas size of the video layer */
    HalDispPixelFormat_e    ePixFormat;        /* Pixel format of the video layer */
} HalDispVideoLayerAttr_t;

typedef struct
{
    u8 bEnbale; /* Enable/Disable Compress */
} HalDispVideoLayerCompressAttr_t;

typedef struct
{
    HalDispVidWinRect_t stDispWin; /* rect of video out chn */
    u16                 u16SrcWidth;
    u16                 u16SrcHeight;
} HalDispInputPortAttr_t;

typedef struct
{
    HalDispPixelFormat_e       ePixelFormat;
    HalDispPixelCompressMode_e eCompressMode;
    HalDispTileMode_e          eTileMode;

    ss_phys_addr_t au64PhyAddr[3];
    u32            au32Stride[3];
} HalDispVideoFrameData_t;

typedef struct
{
    u8                     bEn;
    u16                    u16BuffHeight;
    u16                    u16BuffStartLine;
    HalDispDmaAccessType_e eAccessType;
} HalDispRingBuffAttr_t;

typedef struct
{
    HalDispPixelFormat_e enPixelFmt;
    u16                  u16Width;
    u16                  u16Height;
    u8                   bEn;
    u64                  au64PhyAddr[3];
    u32                  au32Stride[3];
} HalDispHwDmaConfig_t;

typedef struct
{
    HalDispRotateMode_e enRotate;
} HalDispInputPortRotate_t;

typedef struct
{
    HalDispTimeZoneType_e enType;
} HalDispTimeZone_t;

typedef struct
{
    u16 u16InvalidArea[E_HAL_DISP_TIMEZONE_NUM];
} HalDispTimeZoneConfig_t;

typedef struct
{
    u16 u16Htotal;
    u16 u16Vtotal;
    u16 u16HdeStart;
    u16 u16VdeStart;
    u16 u16Width;
    u16 u16Height;
    u8  bInterlace;
    u8  bYuvOutput;
} HalDispDisplayInfo_t;

typedef struct
{
    u8  bEn[E_HAL_DISP_CLK_NUM];
    u32 u32Rate[E_HAL_DISP_CLK_NUM];
    u32 u32Num;
} HalDispClkConfig_t;

typedef struct
{
    u32   u32PqFlags;
    u32   u32DataSize;
    void *pData;
} HalDispPqConfig_t;

typedef struct
{
    HalDispDrvTruningType_e enType;
    u16                     u16Trim[3];
} HalDispDrvTurningConfig_t;

typedef struct
{
    u32   u32DbgmgFlags;
    void *pData;
} HalDispDbgmgConfig_t;

typedef struct
{
    void *pCmdqInf;
    u32   enType;
} HalDispRegAccessConfig_t;

typedef struct
{
    void *pCmdqInf;
    u8    bEnable;
} HalDispRegFlipConfig_t;

typedef struct
{
    u32   u32WaitType;
    void *pCmdqInf;
} HalDispRegWaitDoneConfig_t;

typedef struct
{
    u32   u32DevId;
    void *pCmdqInf;
} HalDispCmdqInfConfig_t;

typedef struct
{
    void *pSrcDevCtx;
    void *pDestDevCtx;
} HalDispDmaBindConfig_t;

typedef struct
{
    HalDispDmaInputType_e      eType;
    HalDispDmaPixelFormat_e    ePixelFormat;
    HalDispPixelCompressMode_e eCompressMode;
    u16                        u16Width;
    u16                        u16Height;
} HalDispDmaInputConfig_t;

typedef struct
{
    HalDispDmaAccessType_e     eAccessType;
    HalDispDmaOutputMode_e     eMode;
    HalDispDmaPixelFormat_e    ePixelFormat;
    HalDispPixelCompressMode_e eCompressMode;
    u16                        u16Width;
    u16                        u16Height;
} HalDispDmaOutputConfig_t;

typedef struct
{
    HalDispDmaInputConfig_t  stInputCfg;
    HalDispDmaOutputConfig_t stOutputCfg;
} HalDispDmaAttrConfig_t;

typedef struct
{
    u8                 bEn;
    ss_phys_addr_t     paPhyAddr[3];
    u32                u32Stride[3];
    u16                u16RingBuffHeight;
    u16                u16RingStartLine;
    u16                u16FrameIdx;
    HalDispCrc16Type_e enCrcTpye;
} HalDispDmaBufferAttrConfig_t;

typedef struct
{
    HalDispHwInfoType_e eType;
    u32                 u32Id;
    u32                 u32Count;
} HalDispHwInfoConfig_t;

typedef struct
{
    u8 bEn;
} HalDispClkInitConfig_t;

typedef struct
{
    HalDispInternalConfigType_e eType;
    u8                          bBootlogoEn;
    u8                          u8ColorId;
    u8                          u8GopBlendId;
    u8                          bDispPat;
    u8                          u8PatMd;
    u8                          bRstMop;
    u8                          bRstDisp;
    u8                          bRstDac;
    u8                          bRstHdmitx;
    u8                          bRstLcd;
    u8                          bRstDacAff;
    u8                          bRstDacSyn;
    u8                          bRstFpll;
    u8                          bCsCEn;
    u8                          u8CsCMd;
    u8                          bCtx;
    u8                          u8MopYThrd;
    u8                          u8MopCThrd;
} HalDispInternalConfig_t;

typedef struct
{
    u32                     u32DevId;
    HalDispDevicePqType_e   ePqType;
    u8                      bWdmaSupport;
    u32                     u32VidLayerStartNumber;
    u32                     u32VidLayerCnt;
} HalDispDeviceCapabilityConfig_t;

typedef struct
{
    u32                     u32VidLayerId;
    u8                      bRotateSupport;
    u32                     u32InputPortHwCnt;
    u8                      bCompressFmtSupport;
    u32                     u32InputPortPitchAlignment;
    u32                     u32RotateHeightAlig;
} HalDispVideoLayerCapabilityConfig_t;

typedef struct
{
    u32 u32Interface;
    u32 u32HwCount;
    u8  bSupportTiming[E_HAL_DISP_OUTPUT_MAX];
} HalDispInterfaceCapabilityConfig_t;

typedef struct
{
    u32                 u32DmaId;
    u8                  bCompressSupport;
    HaldispDeviceType_e eDeviceType;
    u8                  u8FormatWidthAlign[E_HAL_DISP_DMA_PIX_FMT_NUM]; ///<  FormatWidthAlign, 0:non-supported
    u8                  bSupportCompress[E_HAL_DISP_COMPRESS_MODE_MAX]; ///< 1: supported, 0:non-supported
} HalDispDmaCapabilitytConfig_t;
#endif
