/*
* mhal_disp_datatype.h- Sigmastar
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

#ifndef __MHAL_DISP_DATATYPE_H__
#define __MHAL_DISP_DATATYPE_H__

#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "cam_os_wrapper.h"
// Device: Set/Get output to Interface: HDMI/VGA
#define MHAL_DISP_INTF_HDMI  (0x00000001) ///< The Disp output interface : HDMI
#define MHAL_DISP_INTF_CVBS  (0X00000002) ///< The Disp output interface : CVBS
#define MHAL_DISP_INTF_VGA   (0x00000004) ///< The Disp output interface : VGA
#define MHAL_DISP_INTF_YPBPR (0x00000008) ///< The Disp output interface : YPBPR
#define MHAL_DISP_INTF_LCD   (0x00000010) ///< The Disp output interface : LCD
#define MHAL_DISP_INTF_DMA   (0x00000020) ///< The Disp output interface : DMA

#define MHAL_DISP_INTF_TTL       (0x00010000) ///< The Disp output interface : TTL
#define MHAL_DISP_INTF_MIPIDSI   (0x00020000) ///< The Disp output interface : MIPIDSI
#define MHAL_DISP_INTF_BT656     (0x00040000) ///< The Disp output interface : BT656
#define MHAL_DISP_INTF_BT601     (0x00080000) ///< The Disp output interface : BT601
#define MHAL_DISP_INTF_BT1120    (0x00100000) ///< The Disp output interface : BT1120
#define MHAL_DISP_INTF_MCU       (0x00200000) ///< The Disp output interface : MCU
#define MHAL_DISP_INTF_SRGB      (0x00400000) ///< The Disp output interface : SRGB
#define MHAL_DISP_INTF_MCU_NOFLM (0x00800000) ///< The Disp output interface : MCU_NO_FLM
#define MHAL_DISP_INTF_BT1120DDR (0x01000000) ///< The Disp output interface : BT1120 DDR mode

///< Define Disp output timing/resolution ID
typedef enum
{
    E_MHAL_DISP_OUTPUT_PAL = 0,   ///< PAL, 720x576  i 50Hz
    E_MHAL_DISP_OUTPUT_NTSC,      ///< NTSC 704x480 P 60Hz
    E_MHAL_DISP_OUTPUT_960H_PAL,  ///< ITU-R BT.1302 960 x 576  50 Hz (interlaced)
    E_MHAL_DISP_OUTPUT_960H_NTSC, ///< ITU-R BT.1302 960 x 480  60 Hz (interlaced)

    E_MHAL_DISP_OUTPUT_720P24,  ///< 1280 x 720 24Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_720P25,  ///< 1280 x 720 25Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1280x720_2997,  ///< 1280 x  720 29.97Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_720P30,  ///< 1280 x 720 30Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_720P50,  ///< 1280 x 720 50Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1280x720_5994,  ///< 1280 x  720 59.94Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_720P60,  ///< 1280 x 720 60Hz (non-interflaced)

    E_MHAL_DISP_OUTPUT_1920x1080_2398, ///< 1920 x 1080 23.98Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1080P24, ///< 1920 x 1080 24Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1080P25, ///< 1920 x 1080 25Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1920x1080_2997, ///< 1920 x 1080 29.97Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1080P30, ///< 1920 x 1080 30Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1080P50, ///< 1920 x 1080 50Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1920x1080_5994, ///< 1920 x 1080 59.94Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1080P60, ///< 1920 x 1080 60Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1080I50, ///< 1920 x 1080 50Hz (interflaced)
    E_MHAL_DISP_OUTPUT_1080I60, ///< 1920 x 1080 60Hz (interflaced)

    E_MHAL_DISP_OUTPUT_2560x1440_30,   ///< 2560 x 1440 30Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_2560x1440_50,   ///< 2560 x 1440 50Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_2560x1440_60,   ///< 2560 x 1440 60Hz (non-interlaced)

    E_MHAL_DISP_OUTPUT_3840x2160_24,   ///< 3840 x 2160 24Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_3840x2160_25,   ///< 3840 x 2160 25Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_3840x2160_2997, ///< 3840 x 2160 29.97Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_3840x2160_30,   ///< 3840 x 2160 30Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_3840x2160_60,   ///< 3840 x 2160 60Hz (non-interlaced)


    E_MHAL_DISP_OUTPUT_640x480_60,     ///<  640 x 480 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_480P60,         ///<  720 x 480 60Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_576P50,         ///<  720 x 576 50Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_800x600_60,     ///<  800 x 480 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_848x480_60,     ///<  848 x 480 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1024x768_60,    ///< 1920 x 768 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1280x768_60,    ///< 1280 x 768 60Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1280x800_60,    ///< 1280 x  800 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1280x960_60,    ///< 1280 x 960 60Hz (non-interflaced)
    E_MHAL_DISP_OUTPUT_1280x1024_60,   ///< 1280 x 1024 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1360x768_60,    ///< 1360 x 768 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1366x768_60,    ///< 1366 x  768 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1400x1050_60,   ///< 1400 x 1050 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1440x900_60,    ///< 1440 x  900 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1600x900_60,    ///< 1600 x 900 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1600x1200_60,   ///< 1600 x 1200 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1680x1050_60,   ///< 1680 x 1050 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1920x1200_60,   ///< 1920 x 1200 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1920x1440_60,   ///< 1920 x 1440 60Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_1920x2160_30,   ///< 1920 x 1050 30Hz (non-interlaced)
    E_MHAL_DISP_OUTPUT_2560x1600_60,   ///< 2560 x 1600 30Hz (non-interlaced)

    E_MHAL_DISP_OUTPUT_USER,           ///< User defined tiing
    E_MHAL_DISP_OUTPUT_MAX,            ///< Max number of type
} MHAL_DISP_DeviceTiming_e;

///< Define CSC matrix type
typedef enum
{
    E_MHAL_DISP_CSC_MATRIX_BYPASS = 0, ///< Do not change color space

    E_MHAL_DISP_CSC_MATRIX_BT601_TO_BT709, ///< Change color space from BT.601 to BT.709
    E_MHAL_DISP_CSC_MATRIX_BT709_TO_BT601, ///< Change color space from BT.709 to BT.601

    E_MHAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC, ///< Change color space from BT.601 to RGB
    E_MHAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC, ///< Change color space from BT.709 to RGB

    E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT601_PC, ///< Change color space from RGB to BT.601
    E_MHAL_DISP_CSC_MATRIX_RGB_TO_BT709_PC, ///< Change color space from RGB to BT.709
    E_MHAL_DISP_CSC_MATRIX_USER,///< Change color space fromPQbin

    E_MHAL_DISP_CSC_MATRIX_MAX
} MHAL_DISP_CscMattrix_e;

///< Define tile mode type
typedef enum
{
    E_MHAL_DISP_TILE_MODE_NONE  = 0x00, ///< No tile mode
    E_MHAL_DISP_TILE_MODE_16x16 = 0x01, ///< 16x16 tile mode
    E_MHAL_DISP_TILE_MODE_16x32 = 0x02, ///< 16x32 tile mode
    E_MHAL_DISP_TILE_MODE_32x16 = 0x03, ///< 32x16 tile mode
    E_MHAL_DISP_TILE_MODE_32x32 = 0x04, ///< 32 x32 tile mode
} MHAL_DISP_TileMode_e;

///< Define memory map type
typedef enum
{
    E_MHAL_DISP_MMAP_XC_MAIN = 0, ///< Main buffer
    E_MHAL_DISP_MMAP_XC_MENULOAD, ///< Menuload buffer
    E_MHAL_DISP_MMAP_MAX,         ///< Max number of memory map type
} MHAL_DISP_MmapType_e;

///< Define degree of ratate
typedef enum
{
    E_MHAL_DISP_ROTATE_NONE, ///< No rotate
    E_MHAL_DISP_ROTATE_90,   ///< rotate degree is 90
    E_MHAL_DISP_ROTATE_180,  ///< rotate degree is 180
    E_MHAL_DISP_ROTATE_270,  ///< rotate degree is 270
    E_MHAL_DISP_ROTATE_NUM,  ///< max number of rotate degree type
} MHAL_DISP_RorateMode_e;

///< Define time-zone type
typedef enum
{
    E_MHAL_DISP_TIMEZONE_UNKONWN    = 0x00, ///< unknown time zone
    E_MHAL_DISP_TIMEZONE_SYNC       = 0x01, ///< Sync area
    E_MHAL_DISP_TIMEZONE_BACKPORCH  = 0x02, ///< Back porch area
    E_MHAL_DISP_TIMEZONE_ACTIVE     = 0x03, ///< Data Eanble area
    E_MHAL_DISP_TIMEZONE_FRONTPORCH = 0x04, ///< Front porch area
    E_MHAL_DISP_TIMEZONE_NUM        = 0x05, ///< Total number of TimeZone type
} MHAL_DISP_TimeZoneType_e;

///< Define the register assess mode
typedef enum
{
    E_MHAL_DISP_REG_ACCESS_CPU,  ///< RIU mode
    E_MHAL_DISP_REG_ACCESS_CMDQ, ///< CMDQ mode
    E_MHAL_DISP_REG_ACCESS_NUM,  ///< Total number of register acess mode
} MHAL_DISP_RegAccessType_e;

///< Define the position of data fetching for DISP DMA
typedef enum
{
    E_MHAL_DISP_DMA_INPUT_DEVICE_FRONT, ///< The postion of data fetching is front
    E_MHAL_DISP_DMA_INPUT_DEVICE_BACK,  ///< The position of data fetching is back
    E_MHAL_DISP_DMA_INPUT_NUM,          ///< Number of type
} MHAL_DISP_DmaInputType_e;

///< Define the output memory type of DISP DMA
typedef enum
{
    E_MHAL_DISP_DMA_ACCESS_TYPE_IMI, ///< The output DRAM is interal memory
    E_MHAL_DISP_DMA_ACCESS_TYPE_EMI, ///< The output DRAM is external memory
    E_MHAL_DISP_DMA_ACCESS_TYPE_NUM, ///< Total number of tpe
} MHAL_DISP_DmaAccessType_e;

///< Define the output mode of DISP DMA
typedef enum
{
    E_MHAL_DISP_DMA_OUTPUT_MODE_FRAME, ///< The output is frame mode
    E_MHAL_DISP_DMA_OUTPUT_MODE_RING,  ///< The output is ring mode
    E_MHAL_DISP_DMA_OUTPUT_MODE_NUM,   ///< Total number of type
} MHAL_DISP_DmaOutputMode_e;

///< Define the port ID of DISP DMA
typedef enum
{
    E_MHAL_DISP_DMA_PORT0    = 0, ///< DMA port id = 0
    E_MHAL_DISP_DMA_PORT_NUM = 1, ///< Total number of port
} MHAL_DISP_DmaPortType_e;

///< Define the infomation of Device configuration
typedef enum
{
    E_MHAL_DISP_DEV_CFG_NONE       = 0x00000000, ///< No actition
    E_MHAL_DISP_DEV_CFG_BOOTLOGO   = 0x00000001, ///< bootlogo on/off
    E_MHAL_DISP_DEV_CFG_COLORID    = 0x00000002, ///< csc matrix ID
    E_MHAL_DISP_DEV_CFG_GOPBLENDID = 0x00000004, ///< GOP blending ID
    E_MHAL_DISP_DEV_CFG_DISP_PAT   = 0x00000008, ///< Test pattern on/off
    E_MHAL_DISP_DEV_CFG_RST_MOP    = 0x00000010, ///< MOP reset on/off
    E_MHAL_DISP_DEV_CFG_RST_DISP   = 0x00000020, ///< DISP reset on/off
    E_MHAL_DISP_DEV_CFG_RST_DAC    = 0x00000040, ///< HDMITX reset on/off
    E_MHAL_DISP_DEV_CFG_RST_HDMITX = 0x00000080, ///< LCD reset on/off
    E_MHAL_DISP_DEV_CFG_RST_LCD    = 0x00000100, ///< DAC_AFF reset on/off
    E_MHAL_DISP_DEV_CFG_RST_DACAFF = 0x00000200, ///< DAC Syn reset on/off
    E_MHAL_DISP_DEV_CFG_RST_DACSYN = 0x00000400, ///< FPLL reset on/off
    E_MHAL_DISP_DEV_CFG_RST_FPLL   = 0x00000800, ///< MOP reset on/off
    E_MHAL_DISP_DEV_CFG_CSC_EN     = 0x00001000, ///< CSC on/off
    E_MHAL_DISP_DEV_CFG_CSC_MD     = 0x00002000, ///< CSC mode
    E_MHAL_DISP_DEV_CFG_MOP_Y_THRD = 0x00004000, ///< MOP Y DMA Threshold
    E_MHAL_DISP_DEV_CFG_MOP_C_THRD = 0x00008000, ///< MOP C DMA Threshold
} MHAL_DISP_DeviceConfigType_e;

///< Define the flag of PQ
typedef enum
{
    E_MHAL_DISP_PQ_FLAG_NONE                  = 0x0000, ///< No action
    E_MHAL_DISP_PQ_FLAG_LOAD_BIN              = 0x0001, ///< Load binary file
    E_MHAL_DISP_PQ_FLAG_SET_SRC_ID            = 0x0002, ///< Set source ID
    E_MHAL_DISP_PQ_FLAG_PROCESS               = 0x0003, ///< Aplly PQ Setting
    E_MHAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE = 0x0004, ///< Set load type
    E_MHAL_DISP_PQ_FLAG_FREE_BIN              = 0x0005, ///< Free binary file
    E_MHAL_DISP_PQ_FLAG_BYPASS                = 0x0006, ///< OFF device pq setting.
} MHAL_DISP_PqFlagType_e;

///< Define PQ type
typedef enum
{
    E_MHAL_DISP_DEV_PQ_MACE, ///< PQ type is MACE
    E_MHAL_DISP_DEV_PQ_HPQ,  ///< PQ type is HPQ
} MHAL_DISP_DevicePqType_e;

///< Define Device ID
typedef enum
{
    E_MHAL_DISP_DEVICE_NONE = 0x0000, ///< No ID
    E_MHAL_DISP_DEVICE_0    = 0x0001, ///< Device0 (1 << 0)
    E_MHAL_DISP_DEVICE_1    = 0x0002, ///< Device1 (1 << 1)
    E_MHAL_DISP_DEVICE_2    = 0x0004, ///< Device2 (1 << 2)
} MHAL_DISP_DeviceType_e;

///< Define CRC type
typedef enum
{
    E_MHAL_DISP_CRC16_OFF,       ///< CRC off
    E_MHAL_DISP_CRC16_EXT,       ///< extend CRC
    E_MHAL_DISP_CRC16_OVERWRITE, ///< Overwrite CRC
} MHAL_DISP_Crc16Type_e;
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

///< Define memory map information
typedef struct MHAL_DISP_MmapInfo_s
{
    u8             u8Gid;         ///< Mmap ID
    u8             u8Layer;       ///< Memory Layer
    u8             u8MiuNo;       ///< 0: MIU0 / 1: MIU1 / 2: MIU2
    u8             u8CMAHid;      ///< Memory CMAHID
    ss_phys_addr_t u32Addr;       ///< Memory Address
    u32            u32Size;       ///< Memory Size
    u32            u32Align;      ///< Memory Align
    u32            u32MemoryType; ///< Memory Type
} MHAL_DISP_MmapInfo_t;

///< Define PANEL Signaling Type
typedef enum
{
    E_MHAL_LINK_TTL,              ///< TTL  type
    E_MHAL_LINK_LVDS,             ///< LVDS type
    E_MHAL_LINK_RSDS,             ///< RSDS type
    E_MHAL_LINK_MINILVDS,         ///< TCON
    E_MHAL_LINK_ANALOG_MINILVDS,  ///< Analog TCON
    E_MHAL_LINK_DIGITAL_MINILVDS, ///< Digital TCON
    E_MHAL_LINK_MFC,              ///< Ursa (TTL output to Ursa)
    E_MHAL_LINK_DAC_I,            ///< DAC interlace output
    E_MHAL_LINK_DAC_P,            ///< DAC prograssive output
    E_MHAL_LINK_PDPLVDS,          ///< For PDP(Vsync use Manually MODE)
    E_MHAL_LINK_EXT,              ///< EXT LPLL TYPE
} MHAL_DISP_ApiPnlLinkType_e;

///< Define PANEL aspect ratio
typedef enum
{
    E_MHAL_ASPECT_RATIO_4_3 = 0, ///< set aspect ratio to 4 : 3
    E_MHAL_ASPECT_RATIO_WIDE,    ///< set aspect ratio to 16 : 9
    E_MHAL_ASPECT_RATIO_OTHER,   ///< resvered for other aspect ratio other than 4:3/ 16:9
} MHAL_DISP_PnlAspectRatio_e;

///< Define TI bit mode
typedef enum
{
    E_MHAL_TI_10BIT_MODE = 0, ///< TI 10 bits
    E_MHAL_TI_8BIT_MODE  = 2, ///< TI 8  bits
    E_MHAL_TI_6BIT_MODE  = 3, ///< TI 6  bits
} MHAL_DISP_ApiPnlTiBitMode_e;

///< Define which panel output timing change mode is used to change VFreq for same panel
typedef enum
{
    E_MHAL_CHG_DCLK   = 0, ///< Change output DClk to change Vfreq.
    E_MHAL_CHG_HTOTAL = 1, ///< Change H total to change Vfreq.
    E_MHAL_CHG_VTOTAL = 2, ///< Change V total to change Vfreq.
} MHAL_DISP_ApiPnlOutTimingMode_e;

///< Define panel output format bit mode
typedef enum
{
    E_MHAL_OUTPUT_10BIT_MODE = 0, ///< 10 bits
    E_MHAL_OUTPUT_6BIT_MODE  = 1, ///<  6 bits
    E_MHAL_OUTPUT_8BIT_MODE  = 2, ///<  8 bits
} MHAL_DISP_ApiPnlOutPutFormatBitMode_e;

///< Define timing configuration
typedef struct __attribute__((packed))
{
    const u8 *m_pPanelName; ///<  PanelName
#if !(defined(UFO_PUBLIC_HEADER_212) || defined(UFO_PUBLIC_HEADER_300))
#if !defined(__aarch64__)
    u32 u32AlignmentDummy0;
#endif
#endif
    //
    //  Panel output
    //
    u8                         m_bPanelDither : 1;   ///<  dither on off (invalid)
    MHAL_DISP_ApiPnlLinkType_e m_ePanelLinkType : 4; ///<  panel link type (invalid)

    // ========================================================
    // Board related setting
    // ========================================================
    u8 m_bPanelDualPort : 1;    ///<  (Invalid)
    u8 m_bPanelSwapPort : 1;    ///<  (Invalid)
    u8 m_bPanelSwapOdd_ML : 1;  ///<  (Invalid)
    u8 m_bPanelSwapEven_ML : 1; ///<  (Invalid)
    u8 m_bPanelSwapOdd_RB : 1;  ///<  (Invalid)
    u8 m_bPanelSwapEven_RB : 1; ///<  (Invalid)

    u8 m_bPanelSwapLVDS_POL : 1; ///<  (Invalid) differential P/N swap
    u8 m_bPanelSwapLVDS_CH : 1;  ///<  (Invalid) pair swap
    u8 m_bPanelPDP10BIT : 1;     ///<  (Invalid)
    u8 m_bPanelLVDS_TI_MODE : 1; ///<  (Invalid)

    // ========================================================
    // For TTL Only
    // ========================================================
    u8 m_ucPanelDCLKDelay;   ///<  (Invalid)
    u8 m_bPanelInvDCLK : 1;  ///<  (Invalid)Invert clk
    u8 m_bPanelInvDE : 1;    ///<  (Invalid)Invert DE
    u8 m_bPanelInvHSync : 1; ///<  (Invalid)Invert horizontal sync
    u8 m_bPanelInvVSync : 1; ///<  (Invalid)

    // ========================================================
    // Output driving current setting
    // ========================================================
    u8 m_ucPanelDCKLCurrent;     ///<  (Invalid)
    u8 m_ucPanelDECurrent;       ///<  (Invalid)
    u8 m_ucPanelODDDataCurrent;  ///<  (Invalid)
    u8 m_ucPanelEvenDataCurrent; ///<  (Invalid)

    // ========================================================
    // panel on/off timing
    // ========================================================
    u16 m_wPanelOnTiming1;  ///<  (Invalid) time between panel & data while turn on power
    u16 m_wPanelOnTiming2;  ///<  (Invalid) time between data & back light while turn on power
    u16 m_wPanelOffTiming1; ///<  (Invalid) time between back light & data while turn off power
    u16 m_wPanelOffTiming2; ///<  (Invalid) time between data & panel while turn off power

    // ========================================================
    // panel timing spec.
    // ========================================================
    // sync related
    u16 m_ucPanelHSyncWidth;     ///<  Horizotnal sync with
    u16 m_ucPanelHSyncBackPorch; ///<  Horizontal back porch

    u16 m_ucPanelVSyncWidth; ///<  Vertical sync width
    u16 m_ucPanelVBackPorch; ///<  Vertical sync back porch

    // DE related
    u16 m_wPanelHStart; ///<  Horizontal DE start
    u16 m_wPanelVStart; ///<  Vertical DE start
    u16 m_wPanelWidth;  ///< Horizontal DE wdith
    u16 m_wPanelHeight; ///< Vertical DEwidth

    // DClk related
    u16 m_wPanelMaxHTotal; ///<  Max horizntal total
    u16 m_wPanelHTotal;    ///<  Horizontal total
    u16 m_wPanelMinHTotal; ///<  Min horizotnal total

    u16 m_wPanelMaxVTotal; ///<  Max vertical total
    u16 m_wPanelVTotal;    ///<  vertical total
    u16 m_wPanelMinVTotal; ///<  Min vertical total

    u32 m_dwPanelMaxDCLK; ///<  Max Dot clock
    u32 m_dwPanelDCLK;    ///<  Dot clock
    u32 m_dwPanelMinDCLK; ///<  Min dot clock
    u16 m_wPanelVFreqx10; ///<  Frame rate

    u16 m_wSpreadSpectrumStep; ///<  (Invalid) specturm of SSC
    u16 m_wSpreadSpectrumSpan; ///<  (Invalid) Span of SSC

    u8 m_ucDimmingCtl; ///<  (Invalid) Initial Dimming Value
    u8 m_ucMaxPWMVal;  ///<  (Invalid) Max Dimming Value
    u8 m_ucMinPWMVal;  ///<  (Invalid) Min Dimming Value

    u8                         m_bPanelDeinterMode : 1; ///<  (Invalid)
    MHAL_DISP_PnlAspectRatio_e m_ucPanelAspectRatio;    ///<  (Invalid)

    u16                                   m_u16LVDSTxSwapValue;
    MHAL_DISP_ApiPnlTiBitMode_e           m_ucTiBitMode; ///< (Invalid)
    MHAL_DISP_ApiPnlOutPutFormatBitMode_e m_ucOutputFormatBitMode;

    u8 m_bPanelSwapOdd_RG : 1;  ///<  (Invalid)
    u8 m_bPanelSwapEven_RG : 1; ///<  (Invalid)
    u8 m_bPanelSwapOdd_GB : 1;  ///<  (Invalid)
    u8 m_bPanelSwapEven_GB : 1; ///<  (Invalid)

    /**
     *  Others
     */
    u8                              m_bPanelDoubleClk : 1; ///<  (Invalid)
    u32                             m_dwPanelMaxSET;       ///<  (Invalid)
    u32                             m_dwPanelMinSET;       ///<  (Invalid)
    MHAL_DISP_ApiPnlOutTimingMode_e m_ucOutTimingMode;     ///<< (Invalid)
    u8                              m_bPanelNoiseDith : 1; ///<  PAFRC mixed with noise dither disable
} MHAL_DISP_PanelType_t;

///< Define timing configuration for timing table
typedef struct MHAL_DISP_PanelConfig_s
{
    MHAL_DISP_PanelType_t    stPanelAttr;  ///< Attribute of timing
    MHAL_DISP_DeviceTiming_e eTiming;      ///< Timing ID
    u32                      u32OutputDev; ///< Output Device
    u8                       bValid;       ///< 1: valiad, 0: non-valid
} MHAL_DISP_PanelConfig_t;

///< Define timing sync information
typedef struct MHAL_DISP_SyncInfo_s
{
    u8 bSynm;   ///< Sync mode(0:timing,as BT.656; 1:signal,as LCD)
    u8 bIop;    ///< Interlaced or progressive display(0:i; 1:p)
    u8 u8Intfb; ///< Interlace bit width while output

    u16 u16VStart; ///< Vertical DE start
    u16 u16Vact;   ///< Vertical active area
    u16 u16Vbb;    ///< Vertical back blank porch
    u16 u16Vfb;    ///< Vertical front blank porch

    u16 u16HStart; ///< Horizontal DE start
    u16 u16Hact;   ///< Herizontal active area
    u16 u16Hbb;    ///< Herizontal back blank porch
    u16 u16Hfb;    ///< Herizontal front blank porch
    u16 u16Hmid;   ///< Bottom herizontal active area

    u16 u16Bvact; ///< Bottom vertical active area
    u16 u16Bvbb;  ///< Bottom vertical back blank porch
    u16 u16Bvfb;  ///< Bottom vertical front blank porch

    u16 u16Hpw; ///< Horizontal pulse width
    u16 u16Vpw; ///< Vertical pulse width

    u8  bIdv; ///< Inverse data valid of output
    u8  bIhs; ///< Inverse horizontal synch signal
    u8  bIvs; ///< Inverse vertical synch signal
    u32 u32FrameRate;
    u32 u32FrameRateDot;
    u32 u32Dclk;
} MHAL_DISP_SyncInfo_t;

///< Define timing inforamtion of device output resolution
typedef struct
{
    MHAL_DISP_DeviceTiming_e eTimeType;   ///< output timing ID
    MHAL_DISP_SyncInfo_t *   pstSyncInfo; ///< timing sync information
} MHAL_DISP_DeviceTimingInfo_t;

///< Define color space transform inforamtion
typedef struct
{
    MHAL_DISP_CscMattrix_e eCscMatrix;    ///< CSC matrix type
    u32                    u32Luma;       ///< Adjustment of luminance, Range: 0 ~ 100
    u32                    u32Contrast;   ///< Adjustment of contrast, Range:   0 ~ 100
    u32                    u32Hue;        ///< Adjustment of Hue, Range: 0 ~ 100
    u32                    u32Saturation; ///< Adjustment of saturation, Range: 0 ~ 100
} MHAL_DISP_Csc_t;

///< Define adjustment of VGA parameter
typedef struct
{
    MHAL_DISP_Csc_t stCsc;        ///< Adjustment of CSC
    u32             u32Gain;      ///< Adjustment of driving current
    u32             u32Sharpness; ///< Adjustment of sharpness
} MHAL_DISP_VgaParam_t;

///</ Define adjustment of HDMI parameter
typedef struct
{
    MHAL_DISP_Csc_t stCsc;        ///< Adjustment of CSC
    u32             u32Sharpness; ///< Adjustment of sharpness
} MHAL_DISP_HdmiParam_t;

///</ Define adjustment of CVBS parameter
typedef struct
{
    u8 bEnable; ///< 1: enable, 0: disable
    MHAL_DISP_Csc_t stCsc;        ///< Adjustment of CSC
    u32             u32Sharpness; ///< Adjustment of sharpness
} MHAL_DISP_CvbsParam_t;

///</ Define windows configuration of video
typedef struct
{
    u16 u16X;      ///< Horizotnal start
    u16 u16Y;      ///< Vertical start
    u16 u16Width;  ///< Horizontal size
    u16 u16Height; ///< Vertical size
} MHAL_DISP_VidWinRect_t;

///< Define size of videolayer
typedef struct
{
    u32 u32Width;  ///< Horizontal size
    u32 u32Height; ///< Vertical size
} MHAL_DISP_VideoLayerSize_t;

///<  Define attribute of VideoLayer
typedef struct
{
    MHAL_DISP_VidWinRect_t     stVidLayerDispWin; ///< Display window configuration of Videolayer
    MHAL_DISP_VideoLayerSize_t stVidLayerSize;    ///< size of Videolayer
    MHalPixelFormat_e          ePixFormat;        ///< pixel foramt of Videolayer
} MHAL_DISP_VideoLayerAttr_t;

///<  Define attribute of Inputport
typedef struct
{
    MHAL_DISP_VidWinRect_t stDispWin;    ///< Display window configuraiton of Inputport
    u16                    u16SrcWidth;  ///< Source horizonal size
    u16                    u16SrcHeight; ///< Source vertical size
} MHAL_DISP_InputPortAttr_t;

///< Define video frame configuration
typedef struct MHAL_DISP_VideoFrameData_s
{
    MHalPixelFormat_e       ePixelFormat;  ///< Pixel format
    MHalPixelCompressMode_e eCompressMode; ///< Compress mode

    MHAL_DISP_TileMode_e eTileMode; ///< Tile mode

    ss_phys_addr_t aPhyAddr[3];   ///< Physical address
    u32            au32Stride[3]; ///< Memory stride
} MHAL_DISP_VideoFrameData_t;

///< Define ring buffer configuration of Disp DMA
typedef struct
{
    u8                        bEn;              ///< 1: enable, 0:disable
    u16                       u16BuffHeight;    ///< Ring buffer height
    u16                       u16BuffStartLine; ///< Start line of rign buffer
    MHAL_DISP_DmaAccessType_e eAccessType;      ///< output memory type
} MHAL_DISP_RingBufferAttr_t;

// MI_S32 mi_sys_MMA_Alloc(MI_U8 *u8MMAHeapName, MI_U32 u32blkSize ,MI_PHY *phyAddr);
// MI_S32 mi_sys_MMA_Free(MI_PHY phyAddr);

///< Define physical memory allocating configuration
typedef struct
{
    // Success return 0
    s32 (*alloc)(u8 *pu8Name, u32 size, u64 *pu64PhyAddr); ///< Allocate memory function point
    // Success return 0
    s32 (*free)(u64 u64PhyAddr); ///< Free memory function point
} MHAL_DISP_AllocPhyMem_t;

///< Define rotate configuration
typedef struct
{
    MHAL_DISP_RorateMode_e enRotate; ///< The degree of rotate
} MHAL_DISP_RotateConfig_t;

///< Define attribute of compress
typedef struct
{
    u8 bEnbale; ///< 1: enalbe, 0: disable
} MHAL_DISP_CompressAttr_t;

///< Define IRQ flag type
typedef struct
{
    u32 u32IrqMask; ///< Irq mask
    u32 u32IrqFlag; ///< Irq flag
} MHAL_DISP_IRQFlag_t;

///< Define color tempeture configuration
typedef struct
{
    u16 u16RedOffset;   ///< Offset of color R
    u16 u16GreenOffset; ///< Offset of color G
    u16 u16BlueOffset;  ///< Offset of color B

    u16 u16RedColor;   ///< Gain of Color R
    u16 u16GreenColor; ///< Gain of Color G
    u16 u16BlueColor;  ///< Gain of Color B
} MHAL_DISP_ColorTempeture_t;

///< Define adjustment of LCD parameter
typedef struct
{
    MHAL_DISP_Csc_t stCsc;        ///< Adjustment of CSC
    u32             u32Sharpness; ///< Adjustment of sharpness
} MHAL_DISP_LcdParam_t;

///< Define gamma configuration
typedef struct
{
    u8  bEn;         ///< 1:enable, 0:disable
    u16 u16EntryNum; ///< Total entry number of gamma table
    u8 *pu8ColorR;   ///< Data of Gamma R
    u8 *pu8ColorG;   ///< Data of Gamma G
    u8 *pu8ColorB;   ///< Data of Gamma B
} MHAL_DISP_GammaParam_t;

///< Define timezone query configuration
typedef struct
{
    MHAL_DISP_TimeZoneType_e enType; ///< timezone vlaue
} MHAL_DISP_TimeZone_t;

///< Define timezone configuration
typedef struct
{
    u16 u16InvalidArea[E_MHAL_DISP_TIMEZONE_NUM]; ///< Invalid area of echo timezone
} MHAL_DISP_TimeZoneConfig_t;

///< Define display output timing
typedef struct
{
    u16 u16Htotal;      ///< Hoizontal total size
    u16 u16Vtotal;      ///< Vertical total size
    u16 u16HdeStart;    ///< Horizontal DE start
    u16 u16VdeStart;    ///< Vertical DE start
    u16 u16Width;       ///< Horizontal size
    u16 u16Height;      ///< Vertical size
    u8  bInterlaceMode; ///< 1:interlace , 0: non-interlace
    u8  bYuvOutput;     ///< 1: YUV color, 0: RGB color
} MHAL_DISP_DisplayInfo_t;

///< Define register accessing configuration
typedef struct
{
    MHAL_DISP_RegAccessType_e  enType;   ///< register accessing type
    MHAL_CMDQ_CmdqInterface_t *pCmdqInf; ///< CMDQ interface hanlder
} MHAL_DISP_RegAccessConfig_t;

///< Define register flip configuration
typedef struct
{
    u8                         bEnable;  ///< 1: enable, 0: disable
    MHAL_CMDQ_CmdqInterface_t *pCmdqInf; ///< CMDQ interface handler
} MHAL_DISP_RegFlipConfig_t;

///< Define register waiting done configuration
typedef struct
{
    u32                        u32WaitType; ///< wait type
    MHAL_CMDQ_CmdqInterface_t *pCmdqInf;    ///< CMDQ interface handler
} MHAL_DISP_RegWaitDoneConfig_t;

///< Define CMDQ interface configuration
typedef struct
{
    MHAL_CMDQ_CmdqInterface_t *pCmdqInf; ///< CMDQ interface handler
} MHAL_DISP_CmdqInfConfig_t;

///< Define PQ configuration
typedef struct
{
    u32   u32PqFlags;  ///< The flag of pq action
    u32   u32DataSize; ///< size of PQ binary
    void *pData;       ///< Data of PQ binary
} MHAL_DISP_PqConfig_t;

///< Define  DMA binding configuration
typedef struct
{
    void *pSrcDevCtx;  ///< The context of source deivce
    void *pDestDevCtx; ///< The context of destination device
} MHAL_DISP_DmaBindConfig_t;

///< Define input configuraiton of Disp DMA
typedef struct
{
    MHAL_DISP_DmaInputType_e eType;         ///< The position of data fetching
    MHalPixelFormat_e        ePixelFormat;  ///< Pixel format of input data
    MHalPixelCompressMode_e  eCompressMode; ///< Compress mode of input data
    u16                      u16Width;      ///< Horizontal size
    u16                      u16Height;     ///< Vertical size
} MHAL_DISP_DmaInputConfig_t;

///< Define ouput configuraiton of Disp DMA
typedef struct
{
    MHAL_DISP_DmaAccessType_e eAccessType;   ///< Output memory type
    MHAL_DISP_DmaOutputMode_e eMode;         ///< Output mode
    MHalPixelFormat_e         ePixelFormat;  ///< Pixel fomrat of output data
    MHalPixelCompressMode_e   eCompressMode; ///< Compress mode type
    u16                       u16Width;      ///< Horizontal size
    u16                       u16Height;     ///< Vertical size
} MHAL_DISP_DmaOutputConfig_t;

///< Define attribue of Disp DMA
typedef struct
{
    MHAL_DISP_DmaInputConfig_t  stInputCfg;  ///< input data configuration
    MHAL_DISP_DmaOutputConfig_t stOutputCfg; ///< Output data configuration
} MHAL_DISP_DmaAttrConfig_t;

///< Define buffer attribute of Disp DMA
typedef struct
{
    u8                    bEn;               ///< 1:enable, 0:disable
    ss_phys_addr_t        paPhyAddr[3];      ///< Physical address
    u32                   u32Stride[3];      ///< Memory stirde
    u16                   u16RingBuffHeight; ///< height of Ring buffer
    u16                   u16RingStartLine;  ///< Starting line of ring buffer
    u16                   u16FrameIdx;       ///< frame idex
    MHAL_DISP_Crc16Type_e enCrcType;         ///< CRC type
} MHAL_DISP_DmaBufferAttrConfig_t;

///< Define Device configuraiton
typedef struct
{
    MHAL_DISP_DeviceConfigType_e eType;        ///< device config type
    u8                           bBootlogoEn;  ///< Value of Bootlogo
    u8                           u8ColorId;    ///< Value of CSC matrix ID
    u8                           u8GopBlendId; ///< Value of GOP blendig ID
    u8                           bDispPat;     ///< Value of disp test pattern on/off
    u8                           u8PatMd;      ///< Value of disp test pattern mode
    u8                           bRstMop;      ///< Value of MOP reset
    u8                           bRstDisp;     ///< Value of DISP reset
    u8                           bRstDac;      ///< Value of DAC reset
    u8                           bRstHdmitx;   ///< Value of HDMITX reset
    u8                           bRstLcd;      ///< Value of LCD reset
    u8                           bRstDacAff;   ///< Value of DAC AFF reset
    u8                           bRstDacSyn;   ///< Value of DAC SYN reset
    u8                           bRstFpll;     ///< Value of FPLL reset
    u8                           bCscEn;       ///< Value of CSC on/off
    u8                           u8CscMd;      ///< Value of CSC mode
    u8                           bCtx;         ///< Value of CTX
    u8                           u8MopYThrd;   ///< Value of Mop Y Dma Threshold
    u8                           u8MopCThrd;   ///< Value of Mop C Dma Threshold
} MHAL_DISP_DeviceConfig_t;

///< Define Device capability configuration
typedef struct
{
    MHAL_DISP_DevicePqType_e   ePqType;       ///< PQ type
    u8                         bWdmaSupport;  ///< 1: supported, 0:non-supported
    u32                        u32VideoLayerHwCnt; ///< count of videolayer
    u32                        u32VideoLayerStartOffset; ///< start number of videolayer,only for loop use.
} MHAL_DISP_DeviceCapabilityConfig_t;

///< Define VideoLayer capability configuration
typedef struct
{
    u8  bRotateSupport;             ///< 1: supported, 0:non-supported
    u32 u32InputPortHwCnt;          ///< Count of Inputport
    u8  bCompressFmtSupport;        ///< 1: supported, 0:non-supported
    u32 u32InputPortPitchAlignment; ///< Pitch alignmnet
    u32 u32RotateHeightAlighment;   ///< Height Alignment in ratate case
} MHAL_DISP_VideoLayerCapabilityConfig_t;

///< Define Inputport capability configuration
typedef struct
{
    u32 u32HwCount;                             ///< Count of Inputport
    u8  bSupportTiming[E_MHAL_DISP_OUTPUT_MAX]; ///< 1: supported, 0:non-supported
} MHAL_DISP_InterfaceCapabilityConfig_t;

///< Define DMA capability configuration
typedef struct
{
    u8                     bCompressSupport;                                  ///< 1: supported, 0:non-supported
    MHAL_DISP_DeviceType_e eDeviceType;                                       ///< Supported binding Device ID
    u8                     u8FormatWidthAlign[E_MHAL_PIXEL_FRAME_FORMAT_MAX]; ///<  FormatWidthAlign, 0:non-supported
    u8                     bSupportCompress[E_MHAL_COMPRESS_MODE_MAX];        ///< 1: supported, 0:non-supported
} MHAL_DISP_DmaCapabiliytConfig_t;

#endif
