/*
* cmd_bootlogo.c- Sigmastar
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


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <stdlib.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"

#include <ubi_uboot.h>
#include <cmd_osd.h>

#if defined(CONFIG_SSTAR_DISP)
#include "mhal_common.h"
#include "cam_os_wrapper.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"
#endif

#if defined(CONFIG_SSTAR_PNL)
#include "mhal_pnl_datatype.h"
#include "mhal_pnl.h"
#include <../drivers/mstar/panel/PnlTbl.h>
#endif

#if defined(CONFIG_SSTAR_HDMITX)
#include "mhal_hdmitx_datatype.h"
#include "mhal_hdmitx.h"
#endif

#if defined(CONFIG_SSTAR_JPD)
#define JPD_TIME_DEBUG 0
#define ALIGNMENT_NEED 64

#ifndef ALIGN_UP
#define ALIGN_UP(val, alignment) ((((val) + (alignment)-1) / (alignment)) * (alignment))
#endif

#if defined(CONFIG_JPD_SW)
#include "jinclude.h"
#include "jpeglib.h"
#elif defined(CONFIG_JPD_HW)
#include "mhal_jpd.h"

#define MAX_DEC_HEIGHT 8640
#define MAX_DEC_WIDTH  10000
typedef struct MHal_JPD_Addr_s
{
    void *       InputAddr;
    void *       InputpVirAddr;
    unsigned int InputSize;

    void *       OutputAddr;
    void *       OutpVirAddr;
    unsigned int OutputSize;

    void *       InterAddr;
    void *       InterVirAddr;
    unsigned int InterSize;

} MHal_JPD_Addr_t;

#endif
#endif

#if defined(CONFIG_MS_PARTITION)
#include "part_mxp.h"
#endif

#if defined(CONFIG_SSTAR_RGN)
#include "mhal_rgn_datatype.h"
#include "mhal_rgn.h"
#endif

#if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
#define blit_pixel unsigned char
#endif
#include "blit32.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PNL_TEST_MD_EN             0


#define BOOTLOGO_DBG_LEVEL_ERR     0x01
#define BOOTLOGO_DBG_LEVEL_INFO    0x02
#define BOOTLOGO_DBG_LEVEL_JPD     0x04

#define FLAG_DELAY            0xFE
#define FLAG_END_OF_TABLE     0xFF   // END OF REGISTERS MARKER


#define REGFLAG_DELAY                                         0xFFFE
#define REGFLAG_END_OF_TABLE                                  0xFFFF   // END OF REGISTERS MARKER

#define BOOTLOGO_DBG_LEVEL          0 // BOOTLOGO_DBG_LEVEL_INFO


#define BOOTLOGO_DBG(dbglv, _fmt, _args...)    \
    do                                          \
    if(dbglv & u32BootlogDvgLevel)              \
    {                                           \
            printf(_fmt, ## _args);             \
    }while(0)

#define DISP_DEVICE_NULL     0
#define DISP_DEVICE_HDMI     1
#define DISP_DEVICE_VGA      2
#define DISP_DEVICE_LCD      4

#define BOOTLOGO_TIMING_NUM  16


#define BOOTLOGO_NOT_ZOOM     0

#define BOOTLOGO_VIRTUAL_ADDRESS_OFFSET 0x20000000

#define MEASURE_BOOT_LOGO_TIME 0
//-------------------------------------------------------------------------------------------------
//  structure & Enu
//-------------------------------------------------------------------------------------------------
/*Base*/
typedef struct
{
    u8 au8Tittle[8];
    u32 u32DataInfoCnt;
}SS_HEADER_Desc_t;
typedef struct
{
    u8 au8DataInfoName[32];
    u32 u32DataTotalSize;
    u32 u32SubHeadSize;
    u32 u32SubNodeCount;
}SS_SHEADER_DataInfo_t;

/*Disp*/
typedef enum
{
    EN_DISPLAY_DEVICE_NULL,
    EN_DISPLAY_DEVICE_LCD,
    EN_DISPLAY_DEVICE_HDMI,
    EN_DISPLAY_DEVICE_VGA,
    EN_DISPLAY_DEVICE_CVBS,
}SS_SHEADER_DisplayDevice_e;
typedef struct
{
    SS_SHEADER_DataInfo_t stDataInfo;
    u32 u32FirstUseOffset;
    u32 u32DispBufSize;
    u32 u32DispBufStart;
}SS_SHEADER_DispInfo_t;

/*HDMI & VGA*/
typedef struct
{
    SS_SHEADER_DisplayDevice_e enDevice;
    u8 au8ResName[32];
    u32 u32Width;
    u32 u32Height;
    u32 u32Clock;
}SS_SHEADER_DispConfig_t;

#if defined(CONFIG_SSTAR_PNL)
/*Panel*/
typedef struct
{
    SS_SHEADER_DisplayDevice_e enDevice;
    u8 au8PanelName[32];
    MhalPnlParamConfig_t stPnlParaCfg;
    MhalPnlMipiDsiConfig_t stMipiDsiCfg;
}SS_SHEADER_PnlPara_t;
#endif

typedef union
{
    SS_SHEADER_DispConfig_t stDispOut;
#if defined(CONFIG_SSTAR_PNL)
    SS_SHEADER_PnlPara_t stPnlPara;
#endif
}SS_SHEADER_DispPnl_u;

typedef struct
{
    SS_SHEADER_DataInfo_t stDataInfo;
}SS_SHEADER_PictureDataInfo_t;


typedef enum
{
    EN_ASPECT_RATIO_ZOOM,
    EN_ASPECT_RATIO_CENTER,
    EN_ASPECT_RATIO_USER
}DisplayOutAspectRatio_e;

typedef enum
{
    EN_DISPLAY_OUT_DEVICE_NULL,
    EN_DISPLAY_OUT_DEVICE_LCD,
    EN_DISPLAY_OUT_DEVICE_HDMI,
    EN_DISPLAY_OUT_DEVICE_VGA,
    EN_DISPLAY_OUT_DEVICE_CVBS,
}DisplayOutDevice_e;
typedef struct
{
    DisplayOutAspectRatio_e enAspectRatio;
    u32 u32DstX;
    u32 u32DstY;
}DispOutAspectRatio;
typedef struct
{
    DisplayOutDevice_e enDev;
    u32 u32DevId;
    u64 phyAddr;
    u32 u32Size;
    u32 u32Width;
    u32 u32Height;
    u32 u32Clock;
    u32 u32ImgWidth;
    u32 u32ImgHeight;
#if defined(CONFIG_SSTAR_PNL)
    MhalPnlParamConfig_t *pstPnlParaCfg;
    MhalPnlMipiDsiConfig_t *pstMipiDsiCfg;
    MhalPnlUnifiedParamConfig_t *pstUniPnlParaCfg;
    struct spi_attr stSpiCfg;
    u16 *pSpiCmdbuf;
    u32 PanelCmdArrayCnt;
#endif
}DispOutCfg;
typedef enum
{
    EN_LOGO_ROTATE_NONE,
    EN_LOGO_ROTATE_90,
    EN_LOGO_ROTATE_180,
    EN_LOGO_ROTATE_270
}LogoRotation_e;

#if defined(CONFIG_SSTAR_DISP)
typedef struct
{
    MHAL_DISP_DeviceTiming_e enTiminId;
    u16 u16HsyncWidht;
    u16 u16HsyncBacPorch;

    u16 u16VsyncWidht;
    u16 u16VsyncBacPorch;

    u16 u16Hstart;
    u16 u16Vstart;
    u16 u16Hactive;
    u16 u16Vactive;

    u16 u16Htotal;
    u16 u16Vtotal;
    u16 u16DclkMhz;
}DisplayLogoTimingConfig_t;
#endif

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 u32BootlogDvgLevel = BOOTLOGO_DBG_LEVEL_ERR;// | BOOTLOGO_DBG_LEVEL_INFO | BOOTLOGO_DBG_LEVEL_JPD;

#if defined(CONFIG_SSTAR_DISP)
DisplayLogoTimingConfig_t stTimingTable[BOOTLOGO_TIMING_NUM] =
{
    {   E_MHAL_DISP_OUTPUT_PAL,
        64, 68,  5,  39, 132, 0, 720, 576, 864, 625, 27 },

    {   E_MHAL_DISP_OUTPUT_NTSC,
        62, 60,  6,  30, 122, 0, 720, 480, 858, 525, 27 },

    {   E_MHAL_DISP_OUTPUT_1080P60,
        44, 148,  5,  36, 192, 41, 1920, 1080, 2200, 1125, 148 },

    {   E_MHAL_DISP_OUTPUT_1080P50,
        44,  148, 5, 36, 192, 41, 1920, 1080, 2640, 1125, 148 },

    {   E_MHAL_DISP_OUTPUT_720P50,
        40, 220, 5, 20, 260, 25, 1280, 720, 1980, 750, 74},

    {   E_MHAL_DISP_OUTPUT_720P60,
        40, 220,5,20, 260, 25, 1280, 720, 1650, 750, 74},

    {   E_MHAL_DISP_OUTPUT_480P60,
        62, 60, 6, 30, 122, 36, 720, 480, 858, 525, 27},

    {   E_MHAL_DISP_OUTPUT_576P50,
        64, 68, 4, 39, 132, 44, 720, 576, 864, 625, 27},

    {   E_MHAL_DISP_OUTPUT_1024x768_60,
        136, 160, 6, 29, 296, 35, 1024, 768, 1344, 806, 65},

    {   E_MHAL_DISP_OUTPUT_1366x768_60,
        143, 215, 3, 24, 358, 27, 1366, 768, 1792, 798, 86},

    {   E_MHAL_DISP_OUTPUT_1440x900_60,
        152, 232, 6, 25, 384, 31, 1440, 900, 1904, 934, 106},

    {   E_MHAL_DISP_OUTPUT_1280x800_60,
        128, 200, 6, 22, 328, 28, 1280, 800, 1680, 831, 84},

    {   E_MHAL_DISP_OUTPUT_1280x1024_60,
        112, 248, 3, 38, 360, 41, 1280, 1024, 1688, 1066, 108},

    {   E_MHAL_DISP_OUTPUT_1680x1050_60,
        176, 280, 6, 30, 456, 36, 1680, 1050, 2240, 1089, 146},

    {   E_MHAL_DISP_OUTPUT_1600x1200_60,
        192, 304, 3, 46, 496, 49, 1600, 1200, 2160, 1250, 162},

    {   E_MHAL_DISP_OUTPUT_USER,
        48, 46,  4,  23, 98, 27, 800, 480, 928, 525, 43},
};
#endif

#if PNL_TEST_MD_EN
MhalPnlParamConfig_t stTtl00x480Param =
{
    "TTL_800x480_60",         // m_pPanelName
    0,                        //
    0,    // m_bPanelDither
    0,    // m_ePanelLinkType  0:TTL,1:LVDS,8:DAC_P

    1,    // m_bPanelDualPort
    0,    // m_bPanelSwapPort          //
    0,    // m_bPanelSwapOdd_ML
    0,    // m_bPanelSwapEven_ML
    0,    // m_bPanelSwapOdd_RB
    0,    // m_bPanelSwapEven_RB


    0,    // u8SwapLVDS_POL;          ///<  Swap LVDS Channel Polarity
    0,    // u8SwapLVDS_CH;           ///<  Swap LVDS channel
    0,    // u8PDP10BIT;              ///<  PDP 10bits on/off
    0,    // u8LVDS_TI_MODE;          ///<  Ti Mode On/Off

    0,    // m_ucPanelDCLKDelay
    0,    // m_bPanelInvDCLK
    0,    // m_bPanelInvDE
    0,    // m_bPanelInvHSync
    0,    // m_bPanelInvVSync
          //
    1,    // m_ucPanelDCKLCurrent
    1,    // m_ucPanelDECurrent
    1,    // m_ucPanelODDDataCurrent
    1,    // _ucPanelEvenDataCurrent
          //
    30,   // m_wPanelOnTiming1
    400,  // m_wPanelOnTiming2
    80,   // _wPanelOffTiming1
    30,   // m_wPanelOffTiming2
          //
    48,   // m_ucPanelHSyncWidth
    46,   // m_ucPanelHSyncBackPorch
          //
    4,    // m_ucPanelVSyncWidth
    23,   // m_ucPanelVBackPorch
          //
    98,   // m_wPanelHStart
    27,   // m_wPanelVStart
          //
    800,  // m_wPanelWidth
    480,  // m_wPanelHeight
          //
    978,  // m_wPanelMaxHTotal
    928,  // m_wPanelHTotal
    878,  // m_wPanelMinHTotal
          //
    818,  // m_wPanelMaxVTotal
    525,  // m_wPanelVTotal
    718,  // m_wPanelMinVTotal
          //
    49,   // m_dwPanelMaxDCLK
    29,   // m_dwPanelDCLK
    37,   // m_dwPanelMinDCLK
          //
    25,   // m_wSpreadSpectrumStep
    192,  // m_wSpreadSpectrumSpan
          //
    160,  // m_ucDimmingCtl
    255,  // m_ucMaxPWMVal
    80,   // m_ucMinPWMVal
          //
    0,    // m_bPanelDeinterMode

    1,    // m_ucPanelAspectRatio

    0,   //u16LVDSTxSwapValue
    2,   // m_ucTiBitMode      TI_10BIT_MODE = 0    TI_8BIT_MODE  = 2    TI_6BIT_MODE  = 3

    2, // m_ucOutputFormatBitMode  10BIT_MODE = 0  6BIT_MODE  = 1   8BIT_MODE  = 2 565BIT_MODE =3

    0, // m_bPanelSwapOdd_RG
    0, // m_bPanelSwapEven_RG
    0, // m_bPanelSwapOdd_GB
    0, // m_bPanelSwapEven_GB

    1, //m_bPanelDoubleClk
    0x001c848e,  //m_dwPanelMaxSET
    0x0018eb59,  //m_dwPanelMinSET

    2, // m_ucOutTimingMode   DCLK=0, HTOTAL=1, VTOTAL=2

    0, // m_bPanelNoiseDith

    0, // m_bPanelChannelSwap0
    1, // m_bPanelChannelSwap1
    2, // m_bPanelChannelSwap2
    3, // m_bPanelChannelSwap3
    4, // m_bPanelChannelSwap4
};


MhalPnlParamConfig_t stRm6820Param =
{
    "Rm6820",         // m_pPanelName
    0,                        //
    0,    // m_bPanelDither
    11,    // m_ePanelLinkType  0:TTL,1:LVDS,8:DAC_P, 11 MIPI_DSI

    1,    // m_bPanelDualPort
    0,    // m_bPanelSwapPort          //
    0,    // m_bPanelSwapOdd_ML
    0,    // m_bPanelSwapEven_ML
    0,    // m_bPanelSwapOdd_RB
    0,    // m_bPanelSwapEven_RB


    0,    // u8SwapLVDS_POL;          ///<  Swap LVDS Channel Polarity
    0,    // u8SwapLVDS_CH;           ///<  Swap LVDS channel
    0,    // u8PDP10BIT;              ///<  PDP 10bits on/off
    0,    // u8LVDS_TI_MODE;          ///<  Ti Mode On/Off

    0,    // m_ucPanelDCLKDelay
    0,    // m_bPanelInvDCLK
    0,    // m_bPanelInvDE
    0,    // m_bPanelInvHSync
    0,    // m_bPanelInvVSync
          //
    1,    // m_ucPanelDCKLCurrent
    1,    // m_ucPanelDECurrent
    1,    // m_ucPanelODDDataCurrent
    1,    // _ucPanelEvenDataCurrent

    30,   // m_wPanelOnTiming1
    400,  // m_wPanelOnTiming2
    80,   // _wPanelOffTiming1
    30,   // m_wPanelOffTiming2

    6,    // m_ucPanelHSyncWidth
    60,  // m_ucPanelHSyncBackPorch

    40,    // m_ucPanelVSyncWidth
    220,   // m_ucPanelVBackPorch
          //
    66,   // m_wPanelHStart
    260,   // m_wPanelVStart
          //
    720,  // m_wPanelWidth
    1280,  // m_wPanelHeight
          //
    850,  // m_wPanelMaxHTotal
    830,  // m_wPanelHTotal
    750,  // m_wPanelMinHTotal
          //
    1750,  // m_wPanelMaxVTotal
    1650,  // m_wPanelVTotal
    1550,  // m_wPanelMinVTotal
          //
    89,   // m_dwPanelMaxDCLK
    79,   // m_dwPanelDCLK
    69,   // m_dwPanelMinDCLK
          //
    25,   // m_wSpreadSpectrumStep
    192,  // m_wSpreadSpectrumSpan
          //
    160,  // m_ucDimmingCtl
    255,  // m_ucMaxPWMVal
    80,   // m_ucMinPWMVal
          //
    0,    // m_bPanelDeinterMode

    1,    // m_ucPanelAspectRatio

    0,   //u16LVDSTxSwapValue
    2,   // m_ucTiBitMode      TI_10BIT_MODE = 0    TI_8BIT_MODE  = 2    TI_6BIT_MODE  = 3

    2, // m_ucOutputFormatBitMode  10BIT_MODE = 0  6BIT_MODE  = 1   8BIT_MODE  = 2 565BIT_MODE =3

    0, // m_bPanelSwapOdd_RG
    0, // m_bPanelSwapEven_RG
    0, // m_bPanelSwapOdd_GB
    0, // m_bPanelSwapEven_GB

    1, //m_bPanelDoubleClk
    0x001c848e,  //m_dwPanelMaxSET
    0x0018eb59,  //m_dwPanelMinSET

    2, // m_ucOutTimingMode   DCLK=0, HTOTAL=1, VTOTAL=2

    0, // m_bPanelNoiseDith

    2, // m_bPanelChannelSwap0
    4, // m_bPanelChannelSwap1
    3, // m_bPanelChannelSwap2
    1, // m_bPanelChannelSwap3
    0, // m_bPanelChannelSwap4
};

u8 Rm6820TestCmd[] =
{
    0xFE, 1, 0x01,
    0x27, 1, 0x0A,
    0x29, 1, 0x0A,
    0x2B, 1, 0xE5,
    0x24, 1, 0xC0,
    0x25, 1, 0x53,
    0x26, 1, 0x00,
    0x16, 1, 0x52, //wrong
    0x2F, 1, 0x54,
    0x34, 1, 0x57,
    0x1B, 1, 0x00,
    0x12, 1, 0x0A,
    0x1A, 1, 0x06,
    0x46, 1, 0x4D,
    0x52, 1, 0x90,
    0x53, 1, 0x00,
    0x54, 1, 0x90,
    0x55, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x03,
    0x00, 1, 0x05,
    0x01, 1, 0x16,
    0x02, 1, 0x09,
    0x03, 1, 0x0D,
    0x04, 1, 0x00,
    0x05, 1, 0x00,
    0x06, 1, 0x50,
    0x07, 1, 0x05,
    0x08, 1, 0x16,
    0x09, 1, 0x0B,
    0x0A, 1, 0x0F,
    0x0B, 1, 0x00,
    0x0C, 1, 0x00,
    0x0D, 1, 0x50,
    0x0E, 1, 0x03,
    0x0F, 1, 0x04,
    0x10, 1, 0x05,
    0x11, 1, 0x06,
    0x12, 1, 0x00,
    0x13, 1, 0x54,
    0x14, 1, 0x00,
    0x15, 1, 0xC5,
    0x16, 1, 0x08,
    0x17, 1, 0x07,
    0x18, 1, 0x08,
    0x19, 1, 0x09,
    0x1A, 1, 0x0A,
    0x1B, 1, 0x00,
    0x1C, 1, 0x54,
    0x1D, 1, 0x00,
    0x1E, 1, 0x85,
    0x1F, 1, 0x08,
    0x20, 1, 0x00,
    0x21, 1, 0x00,
    0x22, 1, 0x03,
    0x23, 1, 0x1F,
    0x24, 1, 0x00,
    0x25, 1, 0x28,
    0x26, 1, 0x00,
    0x27, 1, 0x1F,
    0x28, 1, 0x00,
    0x29, 1, 0x28,
    0x2A, 1, 0x00,
    0x2B, 1, 0x00,
    0x2D, 1, 0x00,
    0x2F, 1, 0x00,
    0x30, 1, 0x00,
    0x31, 1, 0x00,
    0x32, 1, 0x00,
    0x33, 1, 0x00,
    0x34, 1, 0x00,
    0x35, 1, 0x00,
    0x36, 1, 0x00,
    0x37, 1, 0x00,
    0x38, 1, 0x00,
    0x39, 1, 0x00,
    0x3A, 1, 0x00,
    0x3B, 1, 0x00,
    0x3D, 1, 0x00,
    0x3F, 1, 0x00,
    0x40, 1, 0x00,
    0x3F, 1, 0x00,
    0x41, 1, 0x00,
    0x42, 1, 0x00,
    0x43, 1, 0x00,
    0x44, 1, 0x00,
    0x45, 1, 0x00,
    0x46, 1, 0x00,
    0x47, 1, 0x00,
    0x48, 1, 0x00,
    0x49, 1, 0x00,
    0x4A, 1, 0x00,
    0x4B, 1, 0x00,
    0x4C, 1, 0x00,
    0x4D, 1, 0x00,
    0x4E, 1, 0x00,
    0x4F, 1, 0x00,
    0x50, 1, 0x00,
    0x51, 1, 0x00,
    0x52, 1, 0x00,
    0x53, 1, 0x00,
    0x54, 1, 0x00,
    0x55, 1, 0x00,
    0x56, 1, 0x00,
    0x58, 1, 0x00,
    0x59, 1, 0x00,
    0x5A, 1, 0x00,
    0x5B, 1, 0x00,
    0x5C, 1, 0x00,
    0x5D, 1, 0x00,
    0x5E, 1, 0x00,
    0x5F, 1, 0x00,
    0x60, 1, 0x00,
    0x61, 1, 0x00,
    0x62, 1, 0x00,
    0x63, 1, 0x00,
    0x64, 1, 0x00,
    0x65, 1, 0x00,
    0x66, 1, 0x00,
    0x67, 1, 0x00,
    0x68, 1, 0x00,
    0x69, 1, 0x00,
    0x6A, 1, 0x00,
    0x6B, 1, 0x00,
    0x6C, 1, 0x00,
    0x6D, 1, 0x00,
    0x6E, 1, 0x00,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x00,
    0x72, 1, 0x00,
    0x73, 1, 0x00,
    0x74, 1, 0x04,
    0x75, 1, 0x04,
    0x76, 1, 0x04,
    0x77, 1, 0x04,
    0x78, 1, 0x00,
    0x79, 1, 0x00,
    0x7A, 1, 0x00,
    0x7B, 1, 0x00,
    0x7C, 1, 0x00,
    0x7D, 1, 0x00,
    0x7E, 1, 0x86,
    0x7F, 1, 0x02,
    0x80, 1, 0x0E,
    0x81, 1, 0x0C,
    0x82, 1, 0x0A,
    0x83, 1, 0x08,
    0x84, 1, 0x3F,
    0x85, 1, 0x3F,
    0x86, 1, 0x3F,
    0x87, 1, 0x3F,
    0x88, 1, 0x3F,
    0x89, 1, 0x3F,
    0x8A, 1, 0x3F,
    0x8B, 1, 0x3F,
    0x8C, 1, 0x3F,
    0x8D, 1, 0x3F,
    0x8E, 1, 0x3F,
    0x8F, 1, 0x3F,
    0x90, 1, 0x00,
    0x91, 1, 0x04,
    0x92, 1, 0x3F,
    0x93, 1, 0x3F,
    0x94, 1, 0x3F,
    0x95, 1, 0x3F,
    0x96, 1, 0x05,
    0x97, 1, 0x01,
    0x98, 1, 0x3F,
    0x99, 1, 0x3F,
    0x9A, 1, 0x3F,
    0x9B, 1, 0x3F,
    0x9C, 1, 0x3F,
    0x9D, 1, 0x3F,
    0x9E, 1, 0x3F,
    0x9F, 1, 0x3F,
    0xA0, 1, 0x3F,
    0xA2, 1, 0x3F,
    0xA3, 1, 0x3F,
    0xA4, 1, 0x3F,
    0xA5, 1, 0x09,
    0xA6, 1, 0x0B,
    0xA7, 1, 0x0D,
    0xA9, 1, 0x0F,
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAA, 1, 0x03, // wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAB, 1, 0x07, //wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAC, 1, 0x01,
    0xAD, 1, 0x05,
    0xAE, 1, 0x0D,
    0xAF, 1, 0x0F,
    0xB0, 1, 0x09,
    0xB1, 1, 0x0B,
    0xB2, 1, 0x3F,
    0xB3, 1, 0x3F,
    0xB4, 1, 0x3F,
    0xB5, 1, 0x3F,
    0xB6, 1, 0x3F,
    0xB7, 1, 0x3F,
    0xB8, 1, 0x3F,
    0xB9, 1, 0x3F,
    0xBA, 1, 0x3F,
    0xBB, 1, 0x3F,
    0xBC, 1, 0x3F,
    0xBD, 1, 0x3F,
    0xBE, 1, 0x07,
    0xBF, 1, 0x03,
    0xC0, 1, 0x3F,
    0xC1, 1, 0x3F,
    0xC2, 1, 0x3F,
    0xC3, 1, 0x3F,
    0xC4, 1, 0x02,
    0xC5, 1, 0x06,
    0xC6, 1, 0x3F,
    0xC7, 1, 0x3F,
    0xC8, 1, 0x3F,
    0xC9, 1, 0x3F,
    0xCA, 1, 0x3F,
    0xCB, 1, 0x3F,
    0xCC, 1, 0x3F,
    0xCD, 1, 0x3F,
    0xCE, 1, 0x3F,
    0xCF, 1, 0x3F,
    0xD0, 1, 0x3F,
    0xD1, 1, 0x3F,
    0xD2, 1, 0x0A,
    0xD3, 1, 0x08,
    0xD4, 1, 0x0E,
    0xD5, 1, 0x0C,
    0xD6, 1, 0x04,
    0xD7, 1, 0x00,
    0xDC, 1, 0x02,
    0xDE, 1, 0x10,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x04,
    0x60, 1, 0x00,
    0x61, 1, 0x0C,
    0x62, 1, 0x14,
    0x63, 1, 0x0F,
    0x64, 1, 0x08,
    0x65, 1, 0x15,
    0x66, 1, 0x0F,
    0x67, 1, 0x0B,
    0x68, 1, 0x17,
    0x69, 1, 0x0D,
    0x6A, 1, 0x10,
    0x6B, 1, 0x09,
    0x6C, 1, 0x0F,
    0x6D, 1, 0x11,
    0x6E, 1, 0x0B,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x0C,
    0x72, 1, 0x14,
    0x73, 1, 0x0F,
    0x74, 1, 0x08,
    0x75, 1, 0x15,
    0x76, 1, 0x0F,
    0x77, 1, 0x0B,
    0x78, 1, 0x17,
    0x79, 1, 0x0D,
    0x7A, 1, 0x10,
    0x7B, 1, 0x09,
    0x7C, 1, 0x0F,
    0x7D, 1, 0x11,
    0x7E, 1, 0x0B,
    0x7F, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x0E,
    0x01, 1, 0x75,
    0x49, 1, 0x56,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x00,
    0x58, 1, 0xA9,
    0x11, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0x29, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x0E,
    0x35, 1, 0x80,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};


u8 Rm6820Cmd[] =
{
    0xFE, 1, 0x01,
    0x27, 1, 0x0A,
    0x29, 1, 0x0A,
    0x2B, 1, 0xE5,
    0x24, 1, 0xC0,
    0x25, 1, 0x53,
    0x26, 1, 0x00,
    0x16, 1, 0x52, //wrong
    0x2F, 1, 0x54,
    0x34, 1, 0x57,
    0x1B, 1, 0x00,
    0x12, 1, 0x0A,
    0x1A, 1, 0x06,
    0x46, 1, 0x4D,
    0x52, 1, 0x90,
    0x53, 1, 0x00,
    0x54, 1, 0x90,
    0x55, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x03,
    0x00, 1, 0x05,
    0x01, 1, 0x16,
    0x02, 1, 0x09,
    0x03, 1, 0x0D,
    0x04, 1, 0x00,
    0x05, 1, 0x00,
    0x06, 1, 0x50,
    0x07, 1, 0x05,
    0x08, 1, 0x16,
    0x09, 1, 0x0B,
    0x0A, 1, 0x0F,
    0x0B, 1, 0x00,
    0x0C, 1, 0x00,
    0x0D, 1, 0x50,
    0x0E, 1, 0x03,
    0x0F, 1, 0x04,
    0x10, 1, 0x05,
    0x11, 1, 0x06,
    0x12, 1, 0x00,
    0x13, 1, 0x54,
    0x14, 1, 0x00,
    0x15, 1, 0xC5,
    0x16, 1, 0x08,
    0x17, 1, 0x07,
    0x18, 1, 0x08,
    0x19, 1, 0x09,
    0x1A, 1, 0x0A,
    0x1B, 1, 0x00,
    0x1C, 1, 0x54,
    0x1D, 1, 0x00,
    0x1E, 1, 0x85,
    0x1F, 1, 0x08,
    0x20, 1, 0x00,
    0x21, 1, 0x00,
    0x22, 1, 0x03,
    0x23, 1, 0x1F,
    0x24, 1, 0x00,
    0x25, 1, 0x28,
    0x26, 1, 0x00,
    0x27, 1, 0x1F,
    0x28, 1, 0x00,
    0x29, 1, 0x28,
    0x2A, 1, 0x00,
    0x2B, 1, 0x00,
    0x2D, 1, 0x00,
    0x2F, 1, 0x00,
    0x30, 1, 0x00,
    0x31, 1, 0x00,
    0x32, 1, 0x00,
    0x33, 1, 0x00,
    0x34, 1, 0x00,
    0x35, 1, 0x00,
    0x36, 1, 0x00,
    0x37, 1, 0x00,
    0x38, 1, 0x00,
    0x39, 1, 0x00,
    0x3A, 1, 0x00,
    0x3B, 1, 0x00,
    0x3D, 1, 0x00,
    0x3F, 1, 0x00,
    0x40, 1, 0x00,
    0x3F, 1, 0x00,
    0x41, 1, 0x00,
    0x42, 1, 0x00,
    0x43, 1, 0x00,
    0x44, 1, 0x00,
    0x45, 1, 0x00,
    0x46, 1, 0x00,
    0x47, 1, 0x00,
    0x48, 1, 0x00,
    0x49, 1, 0x00,
    0x4A, 1, 0x00,
    0x4B, 1, 0x00,
    0x4C, 1, 0x00,
    0x4D, 1, 0x00,
    0x4E, 1, 0x00,
    0x4F, 1, 0x00,
    0x50, 1, 0x00,
    0x51, 1, 0x00,
    0x52, 1, 0x00,
    0x53, 1, 0x00,
    0x54, 1, 0x00,
    0x55, 1, 0x00,
    0x56, 1, 0x00,
    0x58, 1, 0x00,
    0x59, 1, 0x00,
    0x5A, 1, 0x00,
    0x5B, 1, 0x00,
    0x5C, 1, 0x00,
    0x5D, 1, 0x00,
    0x5E, 1, 0x00,
    0x5F, 1, 0x00,
    0x60, 1, 0x00,
    0x61, 1, 0x00,
    0x62, 1, 0x00,
    0x63, 1, 0x00,
    0x64, 1, 0x00,
    0x65, 1, 0x00,
    0x66, 1, 0x00,
    0x67, 1, 0x00,
    0x68, 1, 0x00,
    0x69, 1, 0x00,
    0x6A, 1, 0x00,
    0x6B, 1, 0x00,
    0x6C, 1, 0x00,
    0x6D, 1, 0x00,
    0x6E, 1, 0x00,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x00,
    0x72, 1, 0x00,
    0x73, 1, 0x00,
    0x74, 1, 0x04,
    0x75, 1, 0x04,
    0x76, 1, 0x04,
    0x77, 1, 0x04,
    0x78, 1, 0x00,
    0x79, 1, 0x00,
    0x7A, 1, 0x00,
    0x7B, 1, 0x00,
    0x7C, 1, 0x00,
    0x7D, 1, 0x00,
    0x7E, 1, 0x86,
    0x7F, 1, 0x02,
    0x80, 1, 0x0E,
    0x81, 1, 0x0C,
    0x82, 1, 0x0A,
    0x83, 1, 0x08,
    0x84, 1, 0x3F,
    0x85, 1, 0x3F,
    0x86, 1, 0x3F,
    0x87, 1, 0x3F,
    0x88, 1, 0x3F,
    0x89, 1, 0x3F,
    0x8A, 1, 0x3F,
    0x8B, 1, 0x3F,
    0x8C, 1, 0x3F,
    0x8D, 1, 0x3F,
    0x8E, 1, 0x3F,
    0x8F, 1, 0x3F,
    0x90, 1, 0x00,
    0x91, 1, 0x04,
    0x92, 1, 0x3F,
    0x93, 1, 0x3F,
    0x94, 1, 0x3F,
    0x95, 1, 0x3F,
    0x96, 1, 0x05,
    0x97, 1, 0x01,
    0x98, 1, 0x3F,
    0x99, 1, 0x3F,
    0x9A, 1, 0x3F,
    0x9B, 1, 0x3F,
    0x9C, 1, 0x3F,
    0x9D, 1, 0x3F,
    0x9E, 1, 0x3F,
    0x9F, 1, 0x3F,
    0xA0, 1, 0x3F,
    0xA2, 1, 0x3F,
    0xA3, 1, 0x3F,
    0xA4, 1, 0x3F,
    0xA5, 1, 0x09,
    0xA6, 1, 0x0B,
    0xA7, 1, 0x0D,
    0xA9, 1, 0x0F,
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAA, 1, 0x03, // wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAB, 1, 0x07, //wrong
    FLAG_DELAY, FLAG_DELAY, 10,
    0xAC, 1, 0x01,
    0xAD, 1, 0x05,
    0xAE, 1, 0x0D,
    0xAF, 1, 0x0F,
    0xB0, 1, 0x09,
    0xB1, 1, 0x0B,
    0xB2, 1, 0x3F,
    0xB3, 1, 0x3F,
    0xB4, 1, 0x3F,
    0xB5, 1, 0x3F,
    0xB6, 1, 0x3F,
    0xB7, 1, 0x3F,
    0xB8, 1, 0x3F,
    0xB9, 1, 0x3F,
    0xBA, 1, 0x3F,
    0xBB, 1, 0x3F,
    0xBC, 1, 0x3F,
    0xBD, 1, 0x3F,
    0xBE, 1, 0x07,
    0xBF, 1, 0x03,
    0xC0, 1, 0x3F,
    0xC1, 1, 0x3F,
    0xC2, 1, 0x3F,
    0xC3, 1, 0x3F,
    0xC4, 1, 0x02,
    0xC5, 1, 0x06,
    0xC6, 1, 0x3F,
    0xC7, 1, 0x3F,
    0xC8, 1, 0x3F,
    0xC9, 1, 0x3F,
    0xCA, 1, 0x3F,
    0xCB, 1, 0x3F,
    0xCC, 1, 0x3F,
    0xCD, 1, 0x3F,
    0xCE, 1, 0x3F,
    0xCF, 1, 0x3F,
    0xD0, 1, 0x3F,
    0xD1, 1, 0x3F,
    0xD2, 1, 0x0A,
    0xD3, 1, 0x08,
    0xD4, 1, 0x0E,
    0xD5, 1, 0x0C,
    0xD6, 1, 0x04,
    0xD7, 1, 0x00,
    0xDC, 1, 0x02,
    0xDE, 1, 0x10,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x04,
    0x60, 1, 0x00,
    0x61, 1, 0x0C,
    0x62, 1, 0x14,
    0x63, 1, 0x0F,
    0x64, 1, 0x08,
    0x65, 1, 0x15,
    0x66, 1, 0x0F,
    0x67, 1, 0x0B,
    0x68, 1, 0x17,
    0x69, 1, 0x0D,
    0x6A, 1, 0x10,
    0x6B, 1, 0x09,
    0x6C, 1, 0x0F,
    0x6D, 1, 0x11,
    0x6E, 1, 0x0B,
    0x6F, 1, 0x00,
    0x70, 1, 0x00,
    0x71, 1, 0x0C,
    0x72, 1, 0x14,
    0x73, 1, 0x0F,
    0x74, 1, 0x08,
    0x75, 1, 0x15,
    0x76, 1, 0x0F,
    0x77, 1, 0x0B,
    0x78, 1, 0x17,
    0x79, 1, 0x0D,
    0x7A, 1, 0x10,
    0x7B, 1, 0x09,
    0x7C, 1, 0x0F,
    0x7D, 1, 0x11,
    0x7E, 1, 0x0B,
    0x7F, 1, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x0E,
    0x01, 1, 0x75,
    0x49, 1, 0x56,
    FLAG_DELAY, FLAG_DELAY, 200,
    0xFE, 1, 0x00,
    0x58, 1, 0xA9,
    0x11, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    0x29, 0, 0x00,
    FLAG_DELAY, FLAG_DELAY, 200,
    FLAG_END_OF_TABLE, FLAG_END_OF_TABLE,
};


MhalPnlMipiDsiConfig_t stRm6820MipiCfg =
{
    5,     // HsTrail
    3,     // HsPrpr
    5,     // HsZero
    10,    // ClkHsPrpr
    14,    // ClkHsExit
    3,     // ClkTrail
    12,    // ClkZero
    10,    // ClkHsPost
    5,     // DaHsExit
    0,     // ContDet

    16,    // Lpx
    26,    // TaGet
    24,    // TaSure
    50,    // TaGo

    720,   // Hactive
    6,     // Hpw
    60,    // Hbp
    44,    // Hfp
    1280,  // Vactive
    40,    // Vpw
    220,   // Vbp
    110,   // Vfp
    0,     // Bllp
    60,    // Fps

    4,     // LaneNum
    3,     // Format   0:RGB565, 1:RGB666, 2:Loosely_RGB666, 3:RGB888
    1,     // CtrlMode 1:Sync pulse 2:Sync_event, 3:Burst
    Rm6820TestCmd,
    sizeof(Rm6820TestCmd),
};

MhalPnlParamConfig_t stPanel_LX50FWB4001=
{
        "LX50FWB4001_RM68172_480x854", // const char *m_pPanelName;                ///<  PanelName
        0, //u16 u32AlignmentDummy0;
        0,                    //u8             u8Dither;         ///<  Diether On?off
        E_MHAL_PNL_LINK_TTL_SPI_IF, //MHAL_DISP_ApiPnlLinkType_e m_ePanelLinkType   :4;  ///<  PANEL_LINK
        0,        //u8 u8DualPort      :1;          ///<  DualPort on/off
        0,        //u8 u8SwapPort      :1;          ///<  Swap Port on/off
        0,        //u8 u8SwapOdd_ML    :1;          ///<  Swap Odd ML
        0,        //u8 u8SwapEven_ML   :1;          ///<  Swap Even ML
        0,        //u8 u8SwapOdd_RB    :1;          ///<  Swap Odd RB
        0,        //u8 u8SwapEven_RB   :1;          ///<  Swap Even RB

        0,        //u8 u8SwapLVDS_POL  :1;          ///<  Swap LVDS Channel Polairyt
        0,        //u8 u8SwapLVDS_CH   :1;          ///<  Swap LVDS channel
        0,        //u8 u8PDP10BIT      :1;          ///<  PDP 10bits on/off
        0,        //u8 u8LVDS_TI_MODE  :1;          ///<  Ti Mode On/Off

        ///////////////////////////////////////////////
        // For TTL Only
        ///////////////////////////////////////////////
        0,        //u8 u8DCLKDelay;                 ///<  DCLK Delay
        1,        //u8 u8InvDCLK   :1;              ///<  CLK Invert
        0,        //u8 u8InvDE     :1;              ///<  DE Invert
        1,        //u8 u8InvHSync  :1;              ///<  HSync Invert
        1,        //u8 u8InvVSync  :1;              ///<  VSync Invert

        ///////////////////////////////////////////////
        // Output driving current setting
        ///////////////////////////////////////////////
        // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
        0x00,     //u8 u8DCKLCurrent;              ///< PANEL_DCLK_CURRENT
        0x00,     //u8 u8DECurrent;                ///< PANEL_DE_CURRENT
        0x00,     //u8 u8ODDDataCurrent;           ///< PANEL_ODD_DATA_CURRENT
        0x00,     //u8 u8EvenDataCurrent;          ///< PANEL_EVEN_DATA_CURRENT
        ///////////////////////////////////////////////
        // panel on/off timing
        ///////////////////////////////////////////////
        0,       //u16 u16OnTiming1;                ///<  time between panel & data while turn on power
        0,      //u16 u16OnTiming2;                ///<  time between data & back light while turn on power
        0,       //u16 u16OffTiming1;               ///<  time between back light & data while turn off power
        0,       //u16 u16OffTiming2;               ///<  time between data & panel while turn off power
        ///////////////////////////////////////////////
        // panel timing spec.
        ///////////////////////////////////////////////
        // sync related
        20,       //u16 u16HSyncWidth;                  ///<  Hsync Width
        4,       //u16 u16HSyncBackPorch;               ///<  Hsync back porch
                                                        ///<  not support Manuel VSync Start/End now
                                                        ///<  VOP_02[10:0] VSync start = Vtt - VBackPorch - VSyncWidth
                                                        ///<  VOP_03[10:0] VSync end = Vtt - VBackPorch
        3,  //MS_U8 m_ucPanelVSyncWidth;                ///<  define PANEL_VSYNC_WIDTH
        16,  //MS_U8 m_ucPanelVBackPorch;               ///<  define PANEL_VSYNC_BACK_PORCH

        // DE related
        28,  //MS_U16 m_wPanelHStart;                   ///<  VOP_04[11:0], PANEL_HSTART, DE H Start (PANEL_HSYNC_WIDTH + PANEL_HSYNC_BACK_PORCH)
        20,  //MS_U16 m_wPanelVStart;                   ///<  VOP_06[11:0], PANEL_VSTART, DE V Start
        480,  //MS_U16 m_wPanelWidth;                    ///< PANEL_WIDTH, DE width (VOP_05[11:0] = HEnd = HStart + Width - 1)
        854,  //MS_U16 m_wPanelHeight;                   ///< PANEL_HEIGHT, DE height (VOP_07[11:0], = Vend = VStart + Height - 1)

        // DClk related
        0,  //MS_U16 m_wPanelMaxHTotal;                ///<  PANEL_MAX_HTOTAL. Reserved for future using.
        508,  //MS_U16 m_wPanelHTotal;                   ///<  VOP_0C[11:0], PANEL_HTOTAL
        0,  //MS_U16 m_wPanelMinHTotal;                ///<  PANEL_MIN_HTOTAL. Reserved for future using.

        0,  //MS_U16 m_wPanelMaxVTotal;                ///<  PANEL_MAX_VTOTAL. Reserved for future using.
        953,  //MS_U16 m_wPanelVTotal;                   ///<  VOP_0D[11:0], PANEL_VTOTAL
        0,  //MS_U16 m_wPanelMinVTotal;                ///<  PANEL_MIN_VTOTAL. Reserved for future using.

        0,  //MS_U8 m_dwPanelMaxDCLK;                  ///<  PANEL_MAX_DCLK. Reserved for future using.
        36,  //MS_U8 m_dwPanelDCLK;                     ///<  LPLL_0F[23:0], PANEL_DCLK          ,{0x3100_10[7:0], 0x3100_0F[15:0]}
        0,  //MS_U8 m_dwPanelMinDCLK;                  ///<  PANEL_MIN_DCLK. Reserved for future using.
                                                 ///<  spread spectrum
        0,  //MS_U16 m_wSpreadSpectrumStep;            ///<  move to board define, no use now.
        0,  //MS_U16 m_wSpreadSpectrumSpan;            ///<  move to board define, no use now.

        0,  //MS_U8 m_ucDimmingCtl;                    ///<  Initial Dimming Value
        0,  //MS_U8 m_ucMaxPWMVal;                     ///<  Max Dimming Value
        0,  //MS_U8 m_ucMinPWMVal;                     ///<  Min Dimming Value

        0,  //MS_U8 m_bPanelDeinterMode   :1;          ///<  define PANEL_DEINTER_MODE,  no use now
        E_MHAL_PNL_ASPECT_RATIO_OTHER,  //MHAL_DISP_PnlAspectRatio_e m_ucPanelAspectRatio; ///<  Panel Aspect Ratio, provide information to upper layer application for aspect ratio setting.
      /*
        *
        * Board related params
        *
        *  If a board ( like BD_MST064C_D01A_S ) swap LVDS TX polarity
        *    : This polarity swap value =
        *      (LVDS_PN_SWAP_H<<8) | LVDS_PN_SWAP_L from board define,
        *  Otherwise
        *    : The value shall set to 0.
        */
        0,  //MS_U16 m_u16LVDSTxSwapValue;
        0,  //MHAL_DISP_ApiPnlTiBitMode_e m_ucTiBitMode;                         ///< MOD_4B[1:0], refer to "LVDS output app note"
        0,  //MHAL_DISP_ApiPnlOutPutFormatBitMode_e m_ucOutputFormatBitMode;

        0,  //MS_U8 m_bPanelSwapOdd_RG    :1;          ///<  define PANEL_SWAP_ODD_RG
        0,  //MS_U8 m_bPanelSwapEven_RG   :1;          ///<  define PANEL_SWAP_EVEN_RG
        0,  //MS_U8 m_bPanelSwapOdd_GB    :1;          ///<  define PANEL_SWAP_ODD_GB
        0,  //MS_U8 m_bPanelSwapEven_GB   :1;          ///<  define PANEL_SWAP_EVEN_GB

        /**
        *  Others
        */
        0,  //MS_U8 m_bPanelDoubleClk     :1;             ///<  LPLL_03[7], define Double Clock ,LVDS dual mode
        0,  //MS_U32 m_dwPanelMaxSET;                     ///<  define PANEL_MAX_SET
        0,  //MS_U32 m_dwPanelMinSET;                     ///<  define PANEL_MIN_SET
        0,  //MHAL_DISP_ApiPnlOutTimingMode_e m_ucOutTimingMode;   ///<Define which panel output timing change mode is used to change VFreq for same panel
        0,  //MS_U8 m_bPanelNoiseDith     :1;             ///<  PAFRC mixed with noise dither disable
        0,
        0,
        0,
        0,
        0,

};

#if 0
//infinity5
#define LSCE_GPIO_PIN   (107)
#define LSCK_GPIO_PIN   (108)
#define LSDA_GPIO_PIN   (109)
#define LSRST_GPIO_PIN   (15)
#else
//infinity6e
#define LSCE_GPIO_PIN   (59)
#define LSCK_GPIO_PIN   (60)
#define LSDA_GPIO_PIN   (61)
#define LSRST_GPIO_PIN   (6)
#endif
unsigned short LX50FWB4001Cmd[] =
{
    2,                      //spi wordlen  ex: unsigned char = 1, unsigned short =2, unsigned int =4
    9,                      //spi header count exclude current field
    0,                      //latch edge 0:rising edge 1:falling edge                       (offset 1)
    27000,                  // max spi clock kHz                                            (offset 2)
    1,                      // gpio mode on                                                 (offset 3)
    LSDA_GPIO_PIN,          // mosi gpio, SDA data pin                                      (offset 4)
    109,                    // miso gpio                                                    (offset 5)
    LSCE_GPIO_PIN,          // cs gpio                                                      (offset 6)
    LSCK_GPIO_PIN,          // clk gpio                                                     (offset 7)
    LSRST_GPIO_PIN,         // rst gpio                                                     (offset 8)
    3,                      //command num, ex: {0x2001, 0 , 0},  command num is set to 3    (offset 9)
    0x2001, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20f0, 0, 0,
    0x0000, 0, 0,
    0x4055, 0, 0,
    0x20f0, 0, 0,
    0x0001, 0, 0,
    0x40aa, 0, 0,
    0x20f0, 0, 0,
    0x0002, 0, 0,
    0x4052, 0, 0,
    0x20f0, 0, 0,
    0x0003, 0, 0,
    0x4008, 0, 0,
    0x20f0, 0, 0,
    0x0004, 0, 0,
    0x4002, 0, 0,
    0x20f6, 0, 0,
    0x0000, 0, 0,
    0x4060, 0, 0,
    0x20f6, 0, 0,
    0x0001, 0, 0,
    0x4040, 0, 0,
    0x20fe, 0, 0,
    0x0000, 0, 0,
    0x4001, 0, 0,
    0x20fe, 0, 0,
    0x0001, 0, 0,
    0x4080, 0, 0,
    0x20fe, 0, 0,
    0x0002, 0, 0,
    0x4009, 0, 0,
    0x20fe, 0, 0,
    0x0003, 0, 0,
    0x4018, 0, 0,
    0x20fe, 0, 0,
    0x0004, 0, 0,
    0x40a0, 0, 0,
    0x20f0, 0, 0,
    0x0000, 0, 0,
    0x4055, 0, 0,
    0x20f0, 0, 0,
    0x0001, 0, 0,
    0x40aa, 0, 0,
    0x20f0, 0, 0,
    0x0002, 0, 0,
    0x4052, 0, 0,
    0x20f0, 0, 0,
    0x0003, 0, 0,
    0x4008, 0, 0,
    0x20f0, 0, 0,
    0x0004, 0, 0,
    0x4001, 0, 0,
    0x20b0, 0, 0,
    0x0000, 0, 0,
    0x400a, 0, 0,
    0x20b1, 0, 0,
    0x0000, 0, 0,
    0x400a, 0, 0,
    0x20b5, 0, 0,
    0x0000, 0, 0,
    0x4008, 0, 0,
    0x20b6, 0, 0,
    0x0000, 0, 0,
    0x4054, 0, 0,
    0x20b7, 0, 0,
    0x0000, 0, 0,
    0x4044, 0, 0,
    0x20b8, 0, 0,
    0x0000, 0, 0,
    0x4024, 0, 0,
    0x20b9, 0, 0,
    0x0000, 0, 0,
    0x4034, 0, 0,
    0x20ba, 0, 0,
    0x0000, 0, 0,
    0x4014, 0, 0,
    0x20bc, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20bc, 0, 0,
    0x0001, 0, 0,
    0x4098, 0, 0,
    0x20bc, 0, 0,
    0x0002, 0, 0,
    0x4013, 0, 0,
    0x20bd, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20bd, 0, 0,
    0x0001, 0, 0,
    0x4090, 0, 0,
    0x20bd, 0, 0,
    0x0002, 0, 0,
    0x401a, 0, 0,
    0x20be, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20be, 0, 0,
    0x0001, 0, 0,
    0x401a, 0, 0,
    0x20d1, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d1, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d1, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d1, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d1, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d1, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d1, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d1, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d1, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d1, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d1, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d1, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d1, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d1, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d1, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d1, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d1, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d1, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d1, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d1, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d1, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d1, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d1, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d1, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d1, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d1, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d1, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d1, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d1, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d1, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d1, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d1, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d1, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d1, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d1, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d1, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d1, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20d2, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d2, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d2, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d2, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d2, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d2, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d2, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d2, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d2, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d2, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d2, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d2, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d2, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d2, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d2, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d2, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d2, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d2, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d2, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d2, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d2, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d2, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d2, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d2, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d2, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d2, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d2, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d2, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d2, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d2, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d2, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d2, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d2, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d2, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d2, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d2, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d2, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20d3, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d3, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d3, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d3, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d3, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d3, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d3, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d3, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d3, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d3, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d3, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d3, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d3, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d3, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d3, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d3, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d3, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d3, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d3, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d3, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d3, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d3, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d3, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d3, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d3, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d3, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d3, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d3, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d3, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d3, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d3, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d3, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d3, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d3, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d3, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d3, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d3, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20d4, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d4, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d4, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d4, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d4, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d4, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d4, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d4, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d4, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d4, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d4, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d4, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d4, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d4, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d4, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d4, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d4, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d4, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d4, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d4, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d4, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d4, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d4, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d4, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d4, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d4, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d4, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d4, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d4, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d4, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d4, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d4, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d4, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d4, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d4, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d4, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d4, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20d5, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d5, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d5, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d5, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d5, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d5, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d5, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d5, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d5, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d5, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d5, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d5, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d5, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d5, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d5, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d5, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d5, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d5, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d5, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d5, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d5, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d5, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d5, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d5, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d5, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d5, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d5, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d5, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d5, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d5, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d5, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d5, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d5, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d5, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d5, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d5, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d5, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20d6, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0003, 0, 0,
    0x4017, 0, 0,
    0x20d6, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0005, 0, 0,
    0x403e, 0, 0,
    0x20d6, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0007, 0, 0,
    0x405e, 0, 0,
    0x20d6, 0, 0,
    0x0008, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x0009, 0, 0,
    0x407b, 0, 0,
    0x20d6, 0, 0,
    0x000a, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x000b, 0, 0,
    0x40a9, 0, 0,
    0x20d6, 0, 0,
    0x000c, 0, 0,
    0x4000, 0, 0,
    0x20d6, 0, 0,
    0x000d, 0, 0,
    0x40ce, 0, 0,
    0x20d6, 0, 0,
    0x000e, 0, 0,
    0x4001, 0, 0,
    0x20d6, 0, 0,
    0x000f, 0, 0,
    0x400a, 0, 0,
    0x20d6, 0, 0,
    0x0010, 0, 0,
    0x4001, 0, 0,
    0x20d6, 0, 0,
    0x0011, 0, 0,
    0x4037, 0, 0,
    0x20d6, 0, 0,
    0x0012, 0, 0,
    0x4001, 0, 0,
    0x20d6, 0, 0,
    0x0013, 0, 0,
    0x407c, 0, 0,
    0x20d6, 0, 0,
    0x0014, 0, 0,
    0x4001, 0, 0,
    0x20d6, 0, 0,
    0x0015, 0, 0,
    0x40b0, 0, 0,
    0x20d6, 0, 0,
    0x0016, 0, 0,
    0x4001, 0, 0,
    0x20d6, 0, 0,
    0x0017, 0, 0,
    0x40ff, 0, 0,
    0x20d6, 0, 0,
    0x0018, 0, 0,
    0x4002, 0, 0,
    0x20d6, 0, 0,
    0x0019, 0, 0,
    0x403d, 0, 0,
    0x20d6, 0, 0,
    0x001a, 0, 0,
    0x4002, 0, 0,
    0x20d6, 0, 0,
    0x001b, 0, 0,
    0x403f, 0, 0,
    0x20d6, 0, 0,
    0x001c, 0, 0,
    0x4002, 0, 0,
    0x20d6, 0, 0,
    0x001d, 0, 0,
    0x407c, 0, 0,
    0x20d6, 0, 0,
    0x001e, 0, 0,
    0x4002, 0, 0,
    0x20d6, 0, 0,
    0x001f, 0, 0,
    0x40c4, 0, 0,
    0x20d6, 0, 0,
    0x0020, 0, 0,
    0x4002, 0, 0,
    0x20d6, 0, 0,
    0x0021, 0, 0,
    0x40f6, 0, 0,
    0x20d6, 0, 0,
    0x0022, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0023, 0, 0,
    0x403a, 0, 0,
    0x20d6, 0, 0,
    0x0024, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0025, 0, 0,
    0x4068, 0, 0,
    0x20d6, 0, 0,
    0x0026, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0027, 0, 0,
    0x40a0, 0, 0,
    0x20d6, 0, 0,
    0x0028, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0029, 0, 0,
    0x40bf, 0, 0,
    0x20d6, 0, 0,
    0x002a, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x002b, 0, 0,
    0x40e0, 0, 0,
    0x20d6, 0, 0,
    0x002c, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x002d, 0, 0,
    0x40ec, 0, 0,
    0x20d6, 0, 0,
    0x002e, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x002f, 0, 0,
    0x40f5, 0, 0,
    0x20d6, 0, 0,
    0x0030, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0031, 0, 0,
    0x40fa, 0, 0,
    0x20d6, 0, 0,
    0x0032, 0, 0,
    0x4003, 0, 0,
    0x20d6, 0, 0,
    0x0033, 0, 0,
    0x40ff, 0, 0,
    0x20f0, 0, 0,
    0x0000, 0, 0,
    0x4055, 0, 0,
    0x20f0, 0, 0,
    0x0001, 0, 0,
    0x40aa, 0, 0,
    0x20f0, 0, 0,
    0x0002, 0, 0,
    0x4052, 0, 0,
    0x20f0, 0, 0,
    0x0003, 0, 0,
    0x4008, 0, 0,
    0x20f0, 0, 0,
    0x0004, 0, 0,
    0x4003, 0, 0,
    0x20b0, 0, 0,
    0x0000, 0, 0,
    0x4005, 0, 0,
    0x20b0, 0, 0,
    0x0001, 0, 0,
    0x4017, 0, 0,
    0x20b0, 0, 0,
    0x0002, 0, 0,
    0x40f9, 0, 0,
    0x20b0, 0, 0,
    0x0003, 0, 0,
    0x4053, 0, 0,
    0x20b0, 0, 0,
    0x0004, 0, 0,
    0x4053, 0, 0,
    0x20b0, 0, 0,
    0x0005, 0, 0,
    0x4000, 0, 0,
    0x20b0, 0, 0,
    0x0006, 0, 0,
    0x4030, 0, 0,
    0x20b1, 0, 0,
    0x0000, 0, 0,
    0x4005, 0, 0,
    0x20b1, 0, 0,
    0x0001, 0, 0,
    0x4017, 0, 0,
    0x20b1, 0, 0,
    0x0002, 0, 0,
    0x40fb, 0, 0,
    0x20b1, 0, 0,
    0x0003, 0, 0,
    0x4055, 0, 0,
    0x20b1, 0, 0,
    0x0004, 0, 0,
    0x4053, 0, 0,
    0x20b1, 0, 0,
    0x0005, 0, 0,
    0x4000, 0, 0,
    0x20b1, 0, 0,
    0x0006, 0, 0,
    0x4030, 0, 0,
    0x20b2, 0, 0,
    0x0000, 0, 0,
    0x40fb, 0, 0,
    0x20b2, 0, 0,
    0x0001, 0, 0,
    0x40fc, 0, 0,
    0x20b2, 0, 0,
    0x0002, 0, 0,
    0x40fd, 0, 0,
    0x20b2, 0, 0,
    0x0003, 0, 0,
    0x40fe, 0, 0,
    0x20b2, 0, 0,
    0x0004, 0, 0,
    0x40f0, 0, 0,
    0x20b2, 0, 0,
    0x0005, 0, 0,
    0x4053, 0, 0,
    0x20b2, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20b2, 0, 0,
    0x0007, 0, 0,
    0x40c5, 0, 0,
    0x20b2, 0, 0,
    0x0008, 0, 0,
    0x4008, 0, 0,
    0x20b3, 0, 0,
    0x0000, 0, 0,
    0x405b, 0, 0,
    0x20b3, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20b3, 0, 0,
    0x0002, 0, 0,
    0x40fb, 0, 0,
    0x20b3, 0, 0,
    0x0003, 0, 0,
    0x405a, 0, 0,
    0x20b3, 0, 0,
    0x0004, 0, 0,
    0x405a, 0, 0,
    0x20b3, 0, 0,
    0x0005, 0, 0,
    0x400c, 0, 0,
    0x20b4, 0, 0,
    0x0000, 0, 0,
    0x40ff, 0, 0,
    0x20b4, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20b4, 0, 0,
    0x0002, 0, 0,
    0x4001, 0, 0,
    0x20b4, 0, 0,
    0x0003, 0, 0,
    0x4002, 0, 0,
    0x20b4, 0, 0,
    0x0004, 0, 0,
    0x40c0, 0, 0,
    0x20b4, 0, 0,
    0x0005, 0, 0,
    0x4040, 0, 0,
    0x20b4, 0, 0,
    0x0006, 0, 0,
    0x4005, 0, 0,
    0x20b4, 0, 0,
    0x0007, 0, 0,
    0x4008, 0, 0,
    0x20b4, 0, 0,
    0x0008, 0, 0,
    0x4053, 0, 0,
    0x20b5, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20b5, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20b5, 0, 0,
    0x0002, 0, 0,
    0x40ff, 0, 0,
    0x20b5, 0, 0,
    0x0003, 0, 0,
    0x4083, 0, 0,
    0x20b5, 0, 0,
    0x0004, 0, 0,
    0x405f, 0, 0,
    0x20b5, 0, 0,
    0x0005, 0, 0,
    0x405e, 0, 0,
    0x20b5, 0, 0,
    0x0006, 0, 0,
    0x4050, 0, 0,
    0x20b5, 0, 0,
    0x0007, 0, 0,
    0x4050, 0, 0,
    0x20b5, 0, 0,
    0x0008, 0, 0,
    0x4033, 0, 0,
    0x20b5, 0, 0,
    0x0009, 0, 0,
    0x4033, 0, 0,
    0x20b5, 0, 0,
    0x000a, 0, 0,
    0x4055, 0, 0,
    0x20b6, 0, 0,
    0x0000, 0, 0,
    0x40bc, 0, 0,
    0x20b6, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20b6, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20b6, 0, 0,
    0x0003, 0, 0,
    0x4000, 0, 0,
    0x20b6, 0, 0,
    0x0004, 0, 0,
    0x402a, 0, 0,
    0x20b6, 0, 0,
    0x0005, 0, 0,
    0x4080, 0, 0,
    0x20b6, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0001, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0003, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0005, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0006, 0, 0,
    0x4000, 0, 0,
    0x20b7, 0, 0,
    0x0007, 0, 0,
    0x4000, 0, 0,
    0x20b8, 0, 0,
    0x0000, 0, 0,
    0x4011, 0, 0,
    0x20b8, 0, 0,
    0x0001, 0, 0,
    0x4060, 0, 0,
    0x20b8, 0, 0,
    0x0002, 0, 0,
    0x4000, 0, 0,
    0x20b9, 0, 0,
    0x0000, 0, 0,
    0x4090, 0, 0,
    0x20ba, 0, 0,
    0x0000, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x0001, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x0002, 0, 0,
    0x4008, 0, 0,
    0x20ba, 0, 0,
    0x0003, 0, 0,
    0x40ac, 0, 0,
    0x20ba, 0, 0,
    0x0004, 0, 0,
    0x40e2, 0, 0,
    0x20ba, 0, 0,
    0x0005, 0, 0,
    0x4064, 0, 0,
    0x20ba, 0, 0,
    0x0006, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x0007, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x0008, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x0009, 0, 0,
    0x4044, 0, 0,
    0x20ba, 0, 0,
    0x000a, 0, 0,
    0x4047, 0, 0,
    0x20ba, 0, 0,
    0x000b, 0, 0,
    0x403f, 0, 0,
    0x20ba, 0, 0,
    0x000c, 0, 0,
    0x40db, 0, 0,
    0x20ba, 0, 0,
    0x000d, 0, 0,
    0x4091, 0, 0,
    0x20ba, 0, 0,
    0x000e, 0, 0,
    0x4054, 0, 0,
    0x20ba, 0, 0,
    0x000f, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x0000, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x0001, 0, 0,
    0x4043, 0, 0,
    0x20bb, 0, 0,
    0x0002, 0, 0,
    0x4079, 0, 0,
    0x20bb, 0, 0,
    0x0003, 0, 0,
    0x40fd, 0, 0,
    0x20bb, 0, 0,
    0x0004, 0, 0,
    0x40b5, 0, 0,
    0x20bb, 0, 0,
    0x0005, 0, 0,
    0x4014, 0, 0,
    0x20bb, 0, 0,
    0x0006, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x0007, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x0008, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x0009, 0, 0,
    0x4044, 0, 0,
    0x20bb, 0, 0,
    0x000a, 0, 0,
    0x4040, 0, 0,
    0x20bb, 0, 0,
    0x000b, 0, 0,
    0x404a, 0, 0,
    0x20bb, 0, 0,
    0x000c, 0, 0,
    0x40ce, 0, 0,
    0x20bb, 0, 0,
    0x000d, 0, 0,
    0x4086, 0, 0,
    0x20bb, 0, 0,
    0x000e, 0, 0,
    0x4024, 0, 0,
    0x20bb, 0, 0,
    0x000f, 0, 0,
    0x4044, 0, 0,
    0x20bc, 0, 0,
    0x0000, 0, 0,
    0x40e0, 0, 0,
    0x20bc, 0, 0,
    0x0001, 0, 0,
    0x401f, 0, 0,
    0x20bc, 0, 0,
    0x0002, 0, 0,
    0x40f8, 0, 0,
    0x20bc, 0, 0,
    0x0003, 0, 0,
    0x4007, 0, 0,
    0x20bd, 0, 0,
    0x0000, 0, 0,
    0x40e0, 0, 0,
    0x20bd, 0, 0,
    0x0001, 0, 0,
    0x401f, 0, 0,
    0x20bd, 0, 0,
    0x0002, 0, 0,
    0x40f8, 0, 0,
    0x20bd, 0, 0,
    0x0003, 0, 0,
    0x4007, 0, 0,
    0x20f0, 0, 0,
    0x0000, 0, 0,
    0x4055, 0, 0,
    0x20f0, 0, 0,
    0x0001, 0, 0,
    0x40aa, 0, 0,
    0x20f0, 0, 0,
    0x0002, 0, 0,
    0x4052, 0, 0,
    0x20f0, 0, 0,
    0x0003, 0, 0,
    0x4008, 0, 0,
    0x20f0, 0, 0,
    0x0004, 0, 0,
    0x4000, 0, 0,
    0x20b0, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x20b0, 0, 0,
    0x0001, 0, 0,
    0x4010, 0, 0,
    0x20b5, 0, 0,
    0x0000, 0, 0,
    0x406b, 0, 0,
    0x20bc, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x2035, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    0x2036, 0, 0,
    0x0000, 0, 0,
    0x4003, 0, 0,
    0x2011, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    REGFLAG_DELAY, REGFLAG_DELAY, 120,
    0x2029, 0, 0,
    0x0000, 0, 0,
    0x4000, 0, 0,
    REGFLAG_DELAY, REGFLAG_DELAY, 100,
    REGFLAG_END_OF_TABLE, REGFLAG_END_OF_TABLE, 0
};



MhalPnlMipiDsiConfig_t stLX50FWB4001CmdCfg =
{
    5,     // HsTrail
    3,     // HsPrpr
    5,     // HsZero
    10,    // ClkHsPrpr
    14,    // ClkHsExit
    3,     // ClkTrail
    12,    // ClkZero
    10,    // ClkHsPost
    5,     // DaHsExit
    0,     // ContDet

    16,    // Lpx
    26,    // TaGet
    24,    // TaSure
    50,    // TaGo

    720,   // Hactive
    6,     // Hpw
    60,    // Hbp
    44,    // Hfp
    1280,  // Vactive
    40,    // Vpw
    220,   // Vbp
    110,   // Vfp
    0,     // Bllp
    60,    // Fps

    4,     // LaneNum
    3,     // Format   0:RGB565, 1:RGB666, 2:Loosely_RGB666, 3:RGB888
    1,     // CtrlMode 1:Sync pulse 2:Sync_event, 3:Burst
    (u8 *)LX50FWB4001Cmd,
    sizeof(LX50FWB4001Cmd),
};

#endif

#if defined(CONFIG_CMD_MTDPARTS)
#include <jffs2/jffs2.h>
/* partition handling routines */
int mtdparts_init(void);
int find_dev_and_part(const char *id, struct mtd_device **dev,
        u8 *part_num, struct part_info **part);
#endif


//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_SSTAR_DISP)
MS_S32 BootLogoMemAlloc(MS_U8 *pu8Name, MS_U32 size, unsigned long long *pu64PhyAddr)
{
    return 0;
}

MS_S32 BootLogoMemRelease(unsigned long long u64PhyAddr)
{
    return 0;
}


MHAL_DISP_DeviceTiming_e _BootLogoGetTiminId(u16 u16Width, u16 u16Height, u8 u8Rate)
{
    MHAL_DISP_DeviceTiming_e enTiming;
    enTiming =  ((u16Width) == 720 && (u16Height) == 480 && (u8Rate) == 30) ? E_MHAL_DISP_OUTPUT_NTSC :
                ((u16Width) == 720 && (u16Height) == 576 && (u8Rate) == 25) ? E_MHAL_DISP_OUTPUT_PAL :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 24) ? E_MHAL_DISP_OUTPUT_1080P24 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 25) ? E_MHAL_DISP_OUTPUT_1080P25 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 30) ? E_MHAL_DISP_OUTPUT_1080P30 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 50) ? E_MHAL_DISP_OUTPUT_1080P50 :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1080P60 :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 50) ? E_MHAL_DISP_OUTPUT_720P50  :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_720P60  :
                ((u16Width) == 720  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_480P60  :
                ((u16Width) == 720  && (u16Height) == 576  && (u8Rate) == 50) ? E_MHAL_DISP_OUTPUT_576P50  :
                ((u16Width) == 640  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_640x480_60   :
                ((u16Width) == 800  && (u16Height) == 600  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_800x600_60   :
                ((u16Width) == 1280 && (u16Height) == 1024 && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1280x1024_60 :
                ((u16Width) == 1366 && (u16Height) == 768  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1366x768_60  :
                ((u16Width) == 1440 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1440x900_60  :
                ((u16Width) == 1280 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_DISP_OUTPUT_1280x800_60  :
                ((u16Width) == 3840 && (u16Height) == 2160 && (u8Rate) == 30) ? E_MHAL_DISP_OUTPUT_3840x2160_30 :
                                                                                E_MHAL_DISP_OUTPUT_MAX;
    return enTiming;
}
#endif

#if defined(CONFIG_SSTAR_HDMITX)
MhaHdmitxTimingResType_e _BootLogoGetHdmitxTimingId(u16 u16Width, u16 u16Height, u8 u8Rate)
{
    MhaHdmitxTimingResType_e enTiming;
    enTiming =  ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 24) ? E_MHAL_HDMITX_RES_1920X1080P_24HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 25) ? E_MHAL_HDMITX_RES_1920X1080P_25HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 30) ? E_MHAL_HDMITX_RES_1920X1080P_30HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 50) ? E_MHAL_HDMITX_RES_1920X1080P_50HZ :
                ((u16Width) == 1920 && (u16Height) == 1080 && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1920X1080P_60HZ :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 50) ? E_MHAL_HDMITX_RES_1280X720P_50HZ  :
                ((u16Width) == 1280 && (u16Height) == 720  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X720P_60HZ  :
                ((u16Width) == 720  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_720X480P_60HZ  :
                ((u16Width) == 720  && (u16Height) == 576  && (u8Rate) == 50) ? E_MHAL_HDMITX_RES_720X576P_50HZ  :
                ((u16Width) == 640  && (u16Height) == 480  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_MAX   :
                ((u16Width) == 800  && (u16Height) == 600  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_MAX   :
                ((u16Width) == 1280 && (u16Height) == 1024 && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X1024P_60HZ :
                ((u16Width) == 1366 && (u16Height) == 768  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1366X768P_60HZ  :
                ((u16Width) == 1440 && (u16Height) == 900  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1440X900P_60HZ  :
                ((u16Width) == 1280 && (u16Height) == 800  && (u8Rate) == 60) ? E_MHAL_HDMITX_RES_1280X800P_60HZ  :
                ((u16Width) == 3840 && (u16Height) == 2160 && (u8Rate) == 30) ? E_MHAL_HDMITX_RES_MAX :
                                                                                E_MHAL_HDMITX_RES_MAX;
    return enTiming;
}
#endif

void _BootLogoDispPnlInit(void)
{
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_PanelConfig_t stPnlCfg[BOOTLOGO_TIMING_NUM];
    u16 i;


    if( sizeof(stTimingTable)/sizeof(DisplayLogoTimingConfig_t) > BOOTLOGO_TIMING_NUM)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Timing Talbe is bigger than %d\n",
            __FUNCTION__, __LINE__, BOOTLOGO_TIMING_NUM);
        return;
    }
    memset(stPnlCfg, 0, sizeof(MHAL_DISP_PanelConfig_t)*BOOTLOGO_TIMING_NUM);
    for(i=0; i<BOOTLOGO_TIMING_NUM; i++)
    {
        stPnlCfg[i].bValid = 1;
        stPnlCfg[i].eTiming = stTimingTable[i].enTiminId;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncWidth     = stTimingTable[i].u16HsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncBackPorch = stTimingTable[i].u16HsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_ucPanelVSyncWidth     = stTimingTable[i].u16VsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelVBackPorch     = stTimingTable[i].u16VsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_wPanelHStart          = stTimingTable[i].u16Hstart;
        stPnlCfg[i].stPanelAttr.m_wPanelVStart          = stTimingTable[i].u16Vstart;
        stPnlCfg[i].stPanelAttr.m_wPanelWidth           = stTimingTable[i].u16Hactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHeight          = stTimingTable[i].u16Vactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHTotal          = stTimingTable[i].u16Htotal;
        stPnlCfg[i].stPanelAttr.m_wPanelVTotal          = stTimingTable[i].u16Vtotal;
        stPnlCfg[i].stPanelAttr.m_dwPanelDCLK           = stTimingTable[i].u16DclkMhz;
    }


    MHAL_DISP_InitPanelConfig(stPnlCfg, BOOTLOGO_TIMING_NUM);
#endif
}
#define YUV444_TO_YUV420_PIXEL_MAPPING(y_dst_addr, uv_dst_addr, dst_x, dst_y, dst_stride, src_addr, src_x, src_y, src_w, src_h) do {   \
        for (src_y = 0; src_y < src_h; src_y++) \
        {   \
            for (src_x = 0; src_x < src_w; src_x++) \
            {   \
                *((char *)((char *)(y_dst_addr) + (dst_y) * (dst_stride) + (dst_x)))    \
                    = *((char *)((char *)(src_addr) + (src_y) * (src_w * 3) + (src_x * 3)));  \
                if ((src_y & 0x01) && (src_x & 0x01))   \
                {   \
                    *((short *)((char *)(uv_dst_addr) + ((dst_y - 1) >> 1) * (dst_stride) + (dst_x - 1)))    \
                        = *((short *)((char *)(src_addr) + (src_y) * (src_w * 3) + (src_x * 3) + 1));  \
                }   \
            }   \
        }   \
    }while(0)

void _BootLogoYuv444ToYuv420(u8 *pu8InBuf, u8 *pu8OutBuf, u16 *pu16Width, u16 *pu16Height, LogoRotation_e eRot)
{
    u16 x, y;

    u8 *pu8DesY = NULL, *pu8DesUV = NULL;;
    u8 *pu8SrcYUV = NULL;

    pu8SrcYUV = pu8InBuf;

    pu8DesY = pu8OutBuf;
    pu8DesUV = pu8DesY + (*pu16Width) * (*pu16Height);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: 444 To 420, In:%x, Out:%x, Width:%d, Height:%d\n",
        __FUNCTION__, __LINE__,
        (u32)pu8InBuf, (u32)pu8OutBuf, *pu16Width, *pu16Height);

    switch (eRot)
    {
        case EN_LOGO_ROTATE_NONE:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, x, y, *pu16Width, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_90:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, *pu16Height - y, x, *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        case EN_LOGO_ROTATE_180:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, (*pu16Width - x), (*pu16Height - y - 1), *pu16Width, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_270:
        {
            YUV444_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, y, (*pu16Width - x - 1), *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        default:
            return;
    }
}

#if defined(CONFIG_JPD_HW)
#define YUV422_TO_YUV420_PIXEL_MAPPING(y_dst_addr, uv_dst_addr, dst_x, dst_y, dst_stride, src_addr, src_x, src_y, src_w, src_h)  \
do  {                                                                                                                            \
        for (src_y = 0; src_y < src_h; src_y++)                                                                                  \
        {                                                                                                                        \
            for (src_x = 0; src_x < src_w; src_x++)                                                                              \
            {                                                                                                                    \
                *(((y_dst_addr) + (dst_y) * (dst_stride) + (dst_x)))                                                             \
                    = *(((src_addr) + (src_y) * (src_w * 2) + (src_x * 2)));                                                     \
                if (dst_stride == src_w)                                                                                         \
                {                                                                                                                \
                    if ((src_y & 0x01))                                                                                          \
                    {                                                                                                            \
                        *(((uv_dst_addr) + ((dst_y-1)>>1) * (dst_stride) + (dst_x)))                                             \
                            = *(((src_addr) + (src_y) * (src_w * 2) + (src_x * 2) + 1));                                         \
                    }                                                                                                            \
                }                                                                                                                \
                else                                                                                                             \
                {                                                                                                                \
                    if ((src_y & 0x01))                                                                                          \
                    {                                                                                                            \
                        if(src_x & 0x01)                                                                                         \
                        {                                                                                                        \
                            *(((uv_dst_addr) + ((dst_y)>>1) * (dst_stride) + (dst_x)))                                           \
                                = *(((src_addr) + (src_y) * (src_w * 2) + (src_x * 2) + 1));                                     \
                        }                                                                                                        \
                    }                                                                                                            \
                    else                                                                                                         \
                    {                                                                                                            \
                        if((src_x %2) == 0)                                                                                      \
                        {                                                                                                        \
                            *(((uv_dst_addr) + ((dst_y)>>1) * (dst_stride) + (dst_x)))                                           \
                                = *(((src_addr) + (src_y) * (src_w * 2) + (src_x * 2) + 1));                                     \
                        }                                                                                                        \
                    }                                                                                                            \
                }                                                                                                                \
            }                                                                                                                    \
        }                                                                                                                        \
    }while(0)

void _BootLogoYuv422ToYuv420(u8 *pu8InBuf, u8 *pu8OutBuf, u16 *pu16Width, u16 *pu16Height, LogoRotation_e eRot)
{
    u16 x=0, y=0;

    u8 *pu8DesY = NULL, *pu8DesUV = NULL;
    u8 *pu8SrcYUV = NULL;

    pu8SrcYUV = pu8InBuf;

    pu8DesY = pu8OutBuf;
    pu8DesUV = pu8DesY + (*pu16Width) * (*pu16Height);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD,"%s %d:: 422 To 420, In:%x, Out:%x, Width:%d, Height:%d\n",
        __FUNCTION__, __LINE__,
        (u32)pu8InBuf, (u32)pu8OutBuf, *pu16Width, *pu16Height);

    switch (eRot)
    {
        case EN_LOGO_ROTATE_NONE:
        {
            YUV422_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, x, y, *pu16Width , pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_90:
        {
            YUV422_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, (*pu16Height - y), x, *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        case EN_LOGO_ROTATE_180:
        {
            YUV422_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, (*pu16Width - x), (*pu16Height - y - 1), *pu16Width, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
        }
        break;
        case EN_LOGO_ROTATE_270:
        {
            YUV422_TO_YUV420_PIXEL_MAPPING(pu8DesY, pu8DesUV, y, (*pu16Width - x - 1), *pu16Height, pu8SrcYUV, x, y, *pu16Width, *pu16Height);
            *pu16Width ^= *pu16Height;
            *pu16Height ^= *pu16Width;
            *pu16Width ^= *pu16Height;
        }
        break;
        default:
            return;
    }
}
#endif

#if defined(CONFIG_SSTAR_RGN)
#define RGB_PIXEL_MAPPING(dst_addr, dst_x, dst_y, dst_stride, src_addr, src_x, src_y, src_stride, src_w, src_h, type) do { \
        for (src_y = 0; src_y < src_h; src_y++) \
        {   \
            for (src_x = 0; src_x < src_w; src_x++) \
            {   \
                *((type *)((char *)(dst_addr) + (dst_y) * (dst_stride) + (dst_x) * sizeof(type)))    \
                    = *((type *)((char *)(src_addr) + (src_y) * (src_stride) + (src_x) * sizeof(type)));  \
            }   \
        }   \
    }while(0)

static void _BootLogoRgbRotate(u8 *pDstBuf, u8 *pSrcBuf, u16 u16Width, u16 u16Height, LogoRotation_e eRot, u8 u8BytePerPixel)
{
    u16 x = 0, y = 0;
    switch (eRot)
    {
        case EN_LOGO_ROTATE_90:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, u16Height - y - 1, x, u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, u16Height - y - 1, x, u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        case EN_LOGO_ROTATE_180:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, (u16Width - x - 1), (u16Height - y - 1), u8BytePerPixel * u16Width, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, (u16Width - x - 1), (u16Height - y - 1), u8BytePerPixel * u16Width, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        case EN_LOGO_ROTATE_270:
        {
            if (u8BytePerPixel == 2)
            {
                RGB_PIXEL_MAPPING(pDstBuf, y, (u16Width - x - 1), u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u16);
            }
            else if (u8BytePerPixel == 4)
            {
                RGB_PIXEL_MAPPING(pDstBuf, y, (u16Width - x - 1), u16Height * u8BytePerPixel, pSrcBuf, x, y, u8BytePerPixel * u16Width, u16Width, u16Height, u32);
            }
        }
        break;
        default:
            return;
    }
}
static void _BootJpdArgbCtrl(u32 u32InBufSzie, u32 u32InBuf, u32 u32OutBufSize, u32 u32OutBuf, u16 *pu16OutWidth, u16 *pu16OutHeight, LogoRotation_e eRot)
{
#if defined(CONFIG_SSTAR_JPD)
    u32 u32JpgSize;
    u8 *pu8JpgBuffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    u8* dstbuffer = NULL;
    u8 *framebuffer;
    u8* linebuffer;
    u8* optbuffer;
    u16 u16RowStride, u16Width, u16Height, u16PixelSize, u16FbPixleSize;
    int rc; //, i, j;

    u32JpgSize = u32OutBufSize;
    pu8JpgBuffer = (unsigned char *)(u32InBuf);

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, pu8JpgBuffer, u32JpgSize);
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1)
    {
        return;
    }

    cinfo.out_color_space = JCS_RGB;

    jpeg_start_decompress(&cinfo);

    u16Width = cinfo.output_width;
    u16Height = cinfo.output_height;
    u16PixelSize = cinfo.output_components;
    *pu16OutWidth = u16Width;
    *pu16OutHeight = u16Height;

    framebuffer = (unsigned char *)(u32OutBuf + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET);

    u16RowStride = u16Width * u16PixelSize;
    linebuffer = malloc(u16RowStride);
    if(!linebuffer)
        return;


#if 1 //ARGB8888
    u16FbPixleSize = 4;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((int*)optbuffer)+i) = 0xFF<<24|(*(pixel))<<16|(*(pixel+1))<<8|(*(pixel+2));
        }
        optbuffer+=u16Width*4;
    }
#endif
#if 0 //ARGB1555
    u16FbPixleSize = 2;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((u16*)optbuffer)+i) = 0x1<<15|(*(pixel)&0xF8)<<7|(*(pixel+1)&0xF8)<<2|(*(pixel+2)&0xF8)>>3;
        }
        optbuffer+=u16Width*2;
    }
#endif
#if 0 //ARGB4444
    u16FbPixleSize = 2;
    if (eRot != EN_LOGO_ROTATE_NONE)
    {
        dstbuffer = (unsigned char *)malloc(u16Width * u16Height * u16FbPixleSize);
        if (!dstbuffer)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not alloc opt buffer.\n", __FUNCTION__, __LINE__);
            free(linebuffer);

            return;
        }
        optbuffer = dstbuffer;
    }
    else
    {
        optbuffer = framebuffer;
    }
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = linebuffer ;
        u8* pixel=linebuffer;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
        for(int i = 0;i<u16Width;i++,pixel+=cinfo.output_components)
        {
            *(((u16*)optbuffer)+i) = 0xF<<12|(*(pixel)&0xF0)<<4|(*(pixel+1)&0xF0)|(*(pixel+2)&0xF0)>>4;
        }
        optbuffer+=u16Width*2;
    }
#endif

    if (dstbuffer != NULL && eRot != EN_LOGO_ROTATE_NONE)
    {
        _BootLogoRgbRotate(framebuffer, dstbuffer, u16Width, u16Height, eRot, u16FbPixleSize);
        free(dstbuffer);
        if (eRot == EN_LOGO_ROTATE_90 || eRot == EN_LOGO_ROTATE_270)
        {
            *pu16OutWidth = u16Height;
            *pu16OutHeight = u16Width;
        }
    }
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);
    free(linebuffer);
#endif
}
#else
static void _BootJpdYuvCtrl(unsigned long u32InBufSize, unsigned long u32InBuf, unsigned long u32OutBufSize,
                            unsigned long u32OutBuf, u16 *pu16OutWidth, u16 *pu16OutHeight, LogoRotation_e eRot)
{
#if defined(CONFIG_SSTAR_JPD)

#if defined(CONFIG_JPD_SW)

#if JPD_TIME_DEBUG
    unsigned long u32TimerStart    = 0;
    unsigned long u32TimerDecode   = 0;
    unsigned long u32Timer444To420 = 0;
    unsigned long u32TotalTime     = 0;

    u32TimerStart = get_timer(0);
#endif

    // Variables for the source jpg
    unsigned long u32JpgSize;
    u8 *          pu8JpgBuffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr         jerr;

    // Variables for the output buffer, and how long each row is
    unsigned long u32BmpSize;
    u8 *          pu8BmpBuffer;

    unsigned long u32Yuv420Size;
    u8 *          pu8Yuv420Buffer;

    u16 u16RowStride, u16Width, u16Height, u16PixelSize;

    int rc; //i, j;

    u32JpgSize = u32InBufSize;

    pu8JpgBuffer = (unsigned char *)u32InBuf;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Create Decompress struct\n", __FUNCTION__, __LINE__);
    // Allocate a new decompress struct, with the default error handler.
    // The default error handler will exit() on pretty much any issue,
    // so it's likely you'll want to replace it or supplement it with
    // your own.
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Set memory buffer as source\n", __FUNCTION__, __LINE__);
    // Configure this decompressor to read its data from a memory
    // buffer starting at unsigned char *pu8JpgBuffer, which is u32JpgSize
    // long, and which must contain a complete jpg already.
    //
    // If you need something fancier than this, you must write your
    // own data source manager, which shouldn't be too hard if you know
    // what it is you need it to do. See jpeg-8d/jdatasrc.c for the
    // implementation of the standard jpeg_mem_src and jpeg_stdio_src
    // managers as examples to work from.
    jpeg_mem_src(&cinfo, pu8JpgBuffer, u32JpgSize);

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Read the JPEG header\n", __FUNCTION__, __LINE__);
    // Have the decompressor scan the jpeg header. This won't populate
    // the cinfo struct output fields, but will indicate if the
    // jpeg is valid.
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: File does not seem to be a normal JPEG\n", __FUNCTION__,
                     __LINE__);
        return;
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Initiate JPEG decompression\n", __FUNCTION__, __LINE__);

    // output color space is yuv444 packet
    cinfo.out_color_space = JCS_YCbCr;

    jpeg_start_decompress(&cinfo);

    u16Width     = cinfo.output_width;
    u16Height    = cinfo.output_height;
    u16PixelSize = cinfo.output_components;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d::  Image is %d by %d with %d components\n", __FUNCTION__, __LINE__,
                 u16Width, u16Height, u16PixelSize);

    u32BmpSize   = u16Width * u16Height * u16PixelSize;
    pu8BmpBuffer = (u8 *)malloc(u32BmpSize);

    if (pu8BmpBuffer == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: malloc fail\n", __FUNCTION__, __LINE__);
        return;
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: BmpBuffer: 0x%lx\n", __FUNCTION__, __LINE__,
                 (unsigned long)pu8BmpBuffer);
    u32Yuv420Size   = u16Width * u16Height * 3 / 2;
    pu8Yuv420Buffer = (unsigned char *)(u32OutBuf + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET);

    if (u32Yuv420Size > u32OutBufSize)
    {
        free(pu8BmpBuffer);
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d::  Yuv420 OutBufSize not enough! expected=%lx, but get=%lx\n",
                     __FUNCTION__, __LINE__, u32Yuv420Size, u32OutBufSize);
        return;
    }

    u16RowStride = u16Width * u16PixelSize;

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: Start reading scanlines\n", __FUNCTION__, __LINE__);
    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = pu8BmpBuffer + (cinfo.output_scanline) * u16RowStride;

        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: Done reading scanlines\n", __FUNCTION__, __LINE__);
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

#if JPD_TIME_DEBUG
    u32TimerDecode = get_timer(0);
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: End of decompression\n", __FUNCTION__, __LINE__);
    _BootLogoYuv444ToYuv420(pu8BmpBuffer, pu8Yuv420Buffer, &u16Width, &u16Height, eRot);
    *pu16OutWidth  = u16Width;
    *pu16OutHeight = u16Height;

#if JPD_TIME_DEBUG
    u32Timer444To420 = get_timer(0);
    u32TotalTime     = get_timer(u32TimerStart);
    // print the time consuming of JPD_SW
    // get_timer(base) return how many HZ passed since input base
    // here CONFIG_SYS_HZ = 1000, and time measurement unit is ms.
    printf(
        "==================JPD_SW statics==================\n  \
        decode time used    = %ld ms\n  \
        444To420 time used  = %ld ms\n  \
        total time used     = %ld ms\n",
        (u32TimerDecode - u32TimerStart) * 1000 / CONFIG_SYS_HZ,
        (u32Timer444To420 - u32TimerDecode) * 1000 / CONFIG_SYS_HZ, (u32TotalTime)*1000 / CONFIG_SYS_HZ);
#endif

    free(pu8BmpBuffer);

#elif defined(CONFIG_JPD_HW)

    MS_U32 u32DevId            = 0;
    MS_U16 u16Width            = 0;
    MS_U16 u16Height           = 0;
    MS_U32 u32Yuv420Size       = 0;

#if JPD_TIME_DEBUG
    MS_U32 u32TimerStart       = 0;
    MS_U32 u32TimerPrepare     = 0;
    MS_U32 u32TimerExtraHeader = 0;
    MS_U32 u32TimerDecode      = 0;
    MS_U32 u32Timer422To420    = 0;
    MS_U32 u32TotalTime        = 0;

    u32TimerStart = get_timer(0);
#endif

    void *pYuv422OutputAddr = NULL;
    u8 *  pu8Yuv420Buffer   = NULL;

    MHAL_JPD_DEV_HANDLE  pstJpdDevHandle = NULL; ///< might be void* or MS_U32
    MHAL_JPD_INST_HANDLE pstJpdInstCtx   = NULL;

    MHAL_JPD_InputBuf_t  stInputBuf;
    MHAL_JPD_JpgInfo_t   stJpgInfo;
    MHAL_JPD_JpgFrame_t  stJpgFrame;
    MHal_JPD_Addr_t      stJpdAddr;
    MHAL_JPD_MaxDecRes_t stJpdMaxinfo;
    MHAL_JPD_Status_e    eJpdStatusFlag;

    memset(&stInputBuf, 0x00, sizeof(stInputBuf));
    memset(&stJpgInfo, 0x00, sizeof(stJpgInfo));
    memset(&stJpgFrame, 0x00, sizeof(stJpgFrame));
    memset(&stJpdAddr, 0x00, sizeof(stJpdAddr));
    memset(&stJpdMaxinfo, 0x00, sizeof(stJpdMaxinfo));
    memset(&eJpdStatusFlag, 0x00, sizeof(eJpdStatusFlag));

    // InterBuffer allocate when create device

    // InputBuffer
    stJpdAddr.InputAddr     = (void *)u32InBuf;
    stJpdAddr.InputpVirAddr = stJpdAddr.InputAddr;
    stJpdAddr.InputSize     = u32InBufSize;

    // YUV422Buffer
    // We allocate YUV422Buffer when extra the width and height of image.

    if (0 != MHAL_JPD_CreateDevice(u32DevId, &pstJpdDevHandle))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: MHAL_JPD_CreateDevice fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    if (0 != MHAL_JPD_CreateInstance(pstJpdDevHandle, &pstJpdInstCtx))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: MHAL_JPD_CreateInstance fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    stJpdMaxinfo.u16MaxHeight    = MAX_DEC_HEIGHT;
    stJpdMaxinfo.u16MaxWidth     = MAX_DEC_WIDTH;
    stJpdMaxinfo.u16ProMaxHeight = MAX_DEC_HEIGHT;
    stJpdMaxinfo.u16ProMaxWidth  = MAX_DEC_WIDTH;

    if (0 != MHAL_JPD_SetParam(pstJpdInstCtx, E_JPD_PARAM_MAX_DEC_RES, &stJpdMaxinfo)) // set msg to sysinfo
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: MHAL_JPD_SetParam MAX_DEC_RES fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    // We do not use Addr2 now
    stInputBuf.pVirtBufAddr1  = stJpdAddr.InputpVirAddr;
    stInputBuf.pVirtBufAddr2  = stJpdAddr.InputpVirAddr + stJpdAddr.InputSize;
    stInputBuf.u64PhyBufAddr1 = (MS_PHY)stJpdAddr.InputAddr;
    stInputBuf.u64PhyBufAddr2 = (MS_PHY)(stJpdAddr.InputAddr + stJpdAddr.InputSize);
    stInputBuf.u32BufSize1    = stJpdAddr.InputSize;
    stInputBuf.u32BufSize2    = 0;

    flush_dcache_range((unsigned long)(stInputBuf.pVirtBufAddr1),
                       (unsigned long)(stInputBuf.pVirtBufAddr1 + stJpdAddr.InputSize));

#if JPD_TIME_DEBUG
    u32TimerPrepare = get_timer(0);
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: Start Extract Jpg Header Info.\n", __FUNCTION__, __LINE__);
    if (0 != MHAL_JPD_ExtractJpgInfo(pstJpdInstCtx, &stInputBuf, &stJpgInfo))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: MHAL_JPD_Extract Jpg Info fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: Get JpgInfo w = %d , h = %d.\n", __FUNCTION__, __LINE__,
                 stJpgInfo.u16AlignedWidth, stJpgInfo.u16AlignedHeight);
    stJpgFrame.stInputBuf.pVirtBufAddr1  = stJpdAddr.InputpVirAddr;
    stJpgFrame.stInputBuf.pVirtBufAddr2  = stJpdAddr.InputpVirAddr + stJpdAddr.InputSize;
    stJpgFrame.stInputBuf.u64PhyBufAddr1 = (MS_PHY)stJpdAddr.InputAddr;
    stJpgFrame.stInputBuf.u64PhyBufAddr2 = (MS_PHY)(stJpdAddr.InputAddr + stJpdAddr.InputSize);
    stJpgFrame.stInputBuf.u32BufSize1    = stJpdAddr.InputSize;
    stJpgFrame.stInputBuf.u32BufSize2    = 0;

    // Get aligned width and height
    u16Width       = stJpgInfo.u16AlignedWidth;
    u16Height      = stJpgInfo.u16AlignedHeight;
    *pu16OutWidth  = stJpgInfo.u16AlignedWidth;
    *pu16OutHeight = stJpgInfo.u16AlignedHeight;
    u32Yuv420Size  = u16Width * u16Height * 3 / 2;

    // YUV422Buffer
    stJpdAddr.OutputSize = stJpgInfo.u16AlignedWidth * stJpgInfo.u16AlignedHeight * 2;
    pYuv422OutputAddr    = malloc(stJpdAddr.OutputSize + ALIGNMENT_NEED);
    if (!pYuv422OutputAddr)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Can not allocate pYuv422OutputAddr.\n", __FUNCTION__, __LINE__);
        free(pYuv422OutputAddr);
        return;
    }
    memset(pYuv422OutputAddr, 0x0, stJpdAddr.OutputSize);
    // Align output buffer, we use 32 bytes alignment
    stJpdAddr.OutputAddr  = (void *)ALIGN_UP((unsigned long)pYuv422OutputAddr, ALIGNMENT_NEED);
    stJpdAddr.OutpVirAddr = stJpdAddr.OutputAddr;

    stJpgFrame.stOutputBuf.pVirtBufAddr[0]  = stJpdAddr.OutpVirAddr;
    stJpgFrame.stOutputBuf.u64PhyBufAddr[0] = (MS_PHY)stJpdAddr.OutputAddr;
    stJpgFrame.stOutputBuf.u32BufSize       = stJpdAddr.OutputSize;

    stJpgFrame.stOutputBuf.eScaleDownMode  = E_MHAL_JPD_SCALE_DOWN_ORG;
    stJpgFrame.stOutputBuf.eOutputFmt      = E_MHAL_JPD_OUTPUT_FMT_YUV422;
    stJpgFrame.stOutputBuf.u32BufStride[0] = stJpgInfo.u16AlignedWidth * 2;
    stJpgFrame.stOutputBuf.eOutputMode     = E_MHAL_JPD_OUTPUT_FRAME;
    stJpgFrame.stOutputBuf.u32LineNum      = 0;
    stJpgFrame.stOutputBuf.bProtectEnable  = 0;
    stJpgFrame.stOutputBuf.pCmdQ           = NULL;

#if JPD_TIME_DEBUG
    u32TimerExtraHeader                    = get_timer(0);
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s %d:: Start Decode One Frame.\n", __FUNCTION__, __LINE__);
    if (0 != MHAL_JPD_StartDecodeOneFrame(pstJpdInstCtx, &stJpgFrame))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: MHAL_JPD_StartDecodeOneFrame fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Input buffer addr         = 0x%lx \n", u32InBuf);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "u32InBufSize              = 0x%lx \n", u32InBufSize);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Yuv420Buffer addr         = 0x%lx \n", u32OutBuf);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Yuv420Buffer size in need = 0x%x  \n", u32Yuv420Size);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Get u32OutBufSize         = 0x%lx \n", u32OutBufSize);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Yuv422 Output buffer addr = 0x%lx \n", (unsigned long)stJpdAddr.OutputAddr);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "Yuv422 OutBufSize         = 0x%x  \n", stJpdAddr.OutputSize);

    if (MHAL_JPD_CheckDecodeStatus(pstJpdInstCtx, &eJpdStatusFlag) == 0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_JPD, "%s:%d JPD Done.\n", __FUNCTION__, __LINE__);
    }
    else
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s:%d JPD Fail!!!\n", __FUNCTION__, __LINE__);
    }

#if JPD_TIME_DEBUG
    u32TimerDecode = get_timer(0);
#endif

    if (u32Yuv420Size > u32OutBufSize)
    {
        free(pYuv422OutputAddr);
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d:: Yuv420 OutBufSize not enough! expected=%x, but get=%lx\n",
                     __FUNCTION__, __LINE__, u32Yuv420Size, u32OutBufSize);
        return;
    }
    pu8Yuv420Buffer = (u8 *)(u32OutBuf + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET);
    _BootLogoYuv422ToYuv420(pYuv422OutputAddr, pu8Yuv420Buffer, &u16Width, &u16Height, eRot);

    flush_dcache_range((unsigned long)(pu8Yuv420Buffer),
                       (unsigned long)(pu8Yuv420Buffer + u16Width * u16Height * 3 / 2));

    MHAL_JPD_DestroyInstance(pstJpdInstCtx);
    MHAL_JPD_DestroyDevice(pstJpdDevHandle);
    free(pYuv422OutputAddr);

#if JPD_TIME_DEBUG
    u32Timer422To420 = get_timer(0);
    u32TotalTime     = get_timer(u32TimerStart);

    // print the time consuming of JPD_HW
    // get_timer(base) return how many HZ passed since input base
    // here CONFIG_SYS_HZ = 1000, and time measurement unit is ms.
    printf(
        "==================JPD_HW statics==================\n  \
        prepare time used       = %d ms\n  \
        extra header time used  = %d ms\n  \
        decode time used        = %d ms\n  \
        422To420 time used      = %d ms\n  \
        total time used         = %d ms\n",
        (u32TimerPrepare - u32TimerStart) * 1000 / CONFIG_SYS_HZ,
        (u32TimerExtraHeader - u32TimerPrepare) * 1000 / CONFIG_SYS_HZ,
        (u32TimerDecode - u32TimerExtraHeader) * 1000 / CONFIG_SYS_HZ,
        (u32Timer422To420 - u32TimerDecode) * 1000 / CONFIG_SYS_HZ, (u32TotalTime) * 1000 / CONFIG_SYS_HZ);
#endif

#endif
#endif
}

#endif

#if 1//defined(CONFIG_SSTAR_RGN)
static u32 gu32FrameBuffer = 0;
static u32 gu32DispWidth = 0;
static u32 gu32DispHeight = 0;
#endif
void _BootDispCtrl(DispOutCfg *pstDispOut, DispOutAspectRatio *pstAspectRatio, u32 u32Shift)
{
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_AllocPhyMem_t stPhyMem;
    MHAL_DISP_DeviceTimingInfo_t stTimingInfo;
    void *pDevCtx = NULL;
    void *pVidLayerCtx = NULL;
    void *pInputPortCtx = NULL;
    u32 u32Interface = 0;
    u32 u32DispDbgLevel;
    u16 u16DispOutWidht = 0;
    u16 u16DispOutHeight = 0;

    stPhyMem.alloc = BootLogoMemAlloc;
    stPhyMem.free  = BootLogoMemRelease;

    u32DispDbgLevel = 0;//0x1F;
    MHAL_DISP_DbgLevel(&u32DispDbgLevel);

    //Inint Pnl Tbl
    _BootLogoDispPnlInit();

    if(pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_LCD)
    {
        u32Interface = MHAL_DISP_INTF_LCD;
    }
    else if (pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_CVBS)
    {
        u32Interface = MHAL_DISP_INTF_CVBS;
    }
    else
    {
        if (pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_HDMI)
            u32Interface = MHAL_DISP_INTF_HDMI;
        else if (pstDispOut->enDev == EN_DISPLAY_OUT_DEVICE_VGA)
            u32Interface = MHAL_DISP_INTF_VGA;
    }

    if(pDevCtx == NULL)
    {
        if(MHAL_DISP_DeviceCreateInstance(&stPhyMem, pstDispOut->u32DevId, &pDevCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreateDevice Fail\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    if(pVidLayerCtx == NULL)
    {
        if(MHAL_DISP_VideoLayerCreateInstance(&stPhyMem, (pstDispOut->u32DevId?pstDispOut->u32DevId *2:0), &pVidLayerCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreateVideoLayer Fail\n", __FUNCTION__, __LINE__);
            return;

        }
        MHAL_DISP_VideoLayerBind(pVidLayerCtx, pDevCtx);
    }

    if(pInputPortCtx == NULL)
    {
        if(MHAL_DISP_InputPortCreateInstance(&stPhyMem, pVidLayerCtx, 0, &pInputPortCtx) == FALSE)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, CreaetInputPort Fail\n", __FUNCTION__, __LINE__);
            return;
        }
    }
    MHAL_DISP_DeviceSetBackGroundColor(pDevCtx, 0x800080);
    MHAL_DISP_DeviceEnable(pDevCtx, 0);
#if defined(CONFIG_SSTAR_PNL)
    if (pstDispOut->pstPnlParaCfg)
    {
        u32 u32DispInterface = u32Interface;
        if((pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL)||
            (pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF))
            u32DispInterface = MHAL_DISP_INTF_TTL;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            u32DispInterface = MHAL_DISP_INTF_MIPIDSI;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_SRGB)
            u32DispInterface = MHAL_DISP_INTF_SRGB;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MCU_TYPE)
            u32DispInterface = MHAL_DISP_INTF_MCU;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT1120)
            u32DispInterface = MHAL_DISP_INTF_BT1120;
        else if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT656)
            u32DispInterface = MHAL_DISP_INTF_BT656;
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32DispInterface);
    }
    else if (pstDispOut->pstUniPnlParaCfg)
    {
        u32 u32DispInterface = u32Interface;
        if((pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL)||
            (pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF))
            u32DispInterface = MHAL_DISP_INTF_TTL;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            u32DispInterface = MHAL_DISP_INTF_MIPIDSI;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_SRGB)
            u32DispInterface = MHAL_DISP_INTF_SRGB;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MCU_TYPE)
            u32DispInterface = MHAL_DISP_INTF_MCU;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT1120)
            u32DispInterface = MHAL_DISP_INTF_BT1120;
        else if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_BT656)
            u32DispInterface = MHAL_DISP_INTF_BT656;
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32DispInterface);
    }
    else
#endif
        MHAL_DISP_DeviceAddOutInterface(pDevCtx, u32Interface);
#if defined(CONFIG_SSTAR_PNL)
    if(u32Interface == MHAL_DISP_INTF_LCD)
    {
        MHAL_DISP_SyncInfo_t stSyncInfo;
        if (pstDispOut->pstPnlParaCfg)
        {
            stSyncInfo.u16Vact = pstDispOut->pstPnlParaCfg->u16Height;
            stSyncInfo.u16Vbb  = pstDispOut->pstPnlParaCfg->u16VSyncBackPorch;
            stSyncInfo.u16Vpw  = pstDispOut->pstPnlParaCfg->u16VSyncWidth;
            stSyncInfo.u16Vfb  = pstDispOut->pstPnlParaCfg->u16VTotal - stSyncInfo.u16Vact - stSyncInfo.u16Vbb - stSyncInfo.u16Vpw;
            stSyncInfo.u16Hact = pstDispOut->pstPnlParaCfg->u16Width;
            stSyncInfo.u16Hbb  = pstDispOut->pstPnlParaCfg->u16HSyncBackPorch;
            stSyncInfo.u16Hpw  = pstDispOut->pstPnlParaCfg->u16HSyncWidth;
            stSyncInfo.u16Hfb  = pstDispOut->pstPnlParaCfg->u16HTotal - stSyncInfo.u16Hact - stSyncInfo.u16Hbb - stSyncInfo.u16Hpw;
            u16DispOutWidht = pstDispOut->pstPnlParaCfg->u16Width;
            u16DispOutHeight = pstDispOut->pstPnlParaCfg->u16Height;
        }
        else if (pstDispOut->pstUniPnlParaCfg)
        {
            if (pstDispOut->pstUniPnlParaCfg->u8TgnTimingFlag)
            {
                stSyncInfo.u16Vact = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VActive;
                stSyncInfo.u16Vbb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VSyncBackPorch;
                stSyncInfo.u16Vpw  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VSyncWidth;
                stSyncInfo.u16Vfb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VTotal - stSyncInfo.u16Vact - stSyncInfo.u16Vbb - stSyncInfo.u16Vpw;
                stSyncInfo.u16Hact = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HActive;
                stSyncInfo.u16Hbb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HSyncBackPorch;
                stSyncInfo.u16Hpw  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HSyncWidth;
                stSyncInfo.u16Hfb  = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HTotal - stSyncInfo.u16Hact - stSyncInfo.u16Hbb - stSyncInfo.u16Hpw;
                u16DispOutWidht = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16HActive;
                u16DispOutHeight = pstDispOut->pstUniPnlParaCfg->stTgnTimingInfo.u16VActive;
                stSyncInfo.u32FrameRate = pstDispOut->pstUniPnlParaCfg->stMpdInfo.u16Fps;

            }
            else
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "PNL PARA u8TgnTimingFlag 0\n");

                return;
            }
        }
        else
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "PNL PARA is NULL\n");

            return;
        }
        if (pstDispOut->pstMipiDsiCfg)
        {
            stSyncInfo.u32FrameRate = pstDispOut->pstMipiDsiCfg->u16Fps;
        }
        else
        {
            //TTL panel has no mipi para.
        }
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, H(%d %d %d %d) V(%d %d %d %d) Fps:%d\n",
            __FUNCTION__, __LINE__,
            stSyncInfo.u16Hfb, stSyncInfo.u16Hpw, stSyncInfo.u16Hbb, stSyncInfo.u16Hact,
            stSyncInfo.u16Vfb, stSyncInfo.u16Vpw, stSyncInfo.u16Vbb, stSyncInfo.u16Vact,
            stSyncInfo.u32FrameRate);

        stTimingInfo.eTimeType = E_MHAL_DISP_OUTPUT_USER;
        stTimingInfo.pstSyncInfo = &stSyncInfo;
        MHAL_DISP_DeviceSetOutputTiming(pDevCtx, u32Interface, &stTimingInfo);
    }
    else
#endif
    {
        stTimingInfo.eTimeType = _BootLogoGetTiminId(pstDispOut->u32Width,
                                                     pstDispOut->u32Height,
                                                     pstDispOut->u32Clock);
        stTimingInfo.pstSyncInfo = NULL;
        MHAL_DISP_DeviceSetOutputTiming(pDevCtx, u32Interface, &stTimingInfo);
        u16DispOutWidht = pstDispOut->u32Width;
        u16DispOutHeight = pstDispOut->u32Height;
    }
    MHAL_DISP_DeviceEnable(pDevCtx, 1);
#if !defined(CONFIG_SSTAR_RGN)
    MHAL_DISP_VideoFrameData_t stVideoFrameBuffer;
    MHAL_DISP_InputPortAttr_t stInputAttr;

    memset(&stInputAttr, 0, sizeof(MHAL_DISP_InputPortAttr_t));
    memset(&stVideoFrameBuffer, 0, sizeof(MHAL_DISP_VideoFrameData_t));
    stInputAttr.u16SrcWidth = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
    stInputAttr.u16SrcHeight = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
    switch (pstAspectRatio->enAspectRatio)
    {
        case EN_ASPECT_RATIO_ZOOM:
        {
            stInputAttr.stDispWin.u16X = 0;
            stInputAttr.stDispWin.u16Y = 0;
            stInputAttr.stDispWin.u16Width = u16DispOutWidht;
            stInputAttr.stDispWin.u16Height = u16DispOutHeight;
        }
        break;
        case EN_ASPECT_RATIO_CENTER:
        {
            stInputAttr.stDispWin.u16X = (u16DispOutWidht - stInputAttr.u16SrcWidth) / 2;
            stInputAttr.stDispWin.u16Y = (u16DispOutHeight - stInputAttr.u16SrcHeight) / 2;
            stInputAttr.stDispWin.u16Width = stInputAttr.u16SrcWidth;
            stInputAttr.stDispWin.u16Height = stInputAttr.u16SrcHeight;
        }
        break;
        case EN_ASPECT_RATIO_USER:
        {
            stInputAttr.stDispWin.u16X = pstAspectRatio->u32DstX;
            stInputAttr.stDispWin.u16Y = pstAspectRatio->u32DstY;
            stInputAttr.stDispWin.u16Width = stInputAttr.u16SrcWidth;
            stInputAttr.stDispWin.u16Height = stInputAttr.u16SrcHeight;
        }
        break;
        default:
            return;
    }
    stVideoFrameBuffer.ePixelFormat = E_MHAL_PIXEL_FRAME_YUV_MST_420;
    stVideoFrameBuffer.aPhyAddr[0] = (MS_PHYADDR)(pstDispOut->phyAddr + (u64)u32Shift);
    stVideoFrameBuffer.aPhyAddr[1] = (MS_PHYADDR)(pstDispOut->phyAddr + (u64)(u32Shift + pstDispOut->u32ImgWidth * pstDispOut->u32ImgHeight));
    stVideoFrameBuffer.au32Stride[0] = pstDispOut->u32ImgWidth;
    MHAL_DISP_InputPortSetAttr(pInputPortCtx, &stInputAttr);
    MHAL_DISP_InputPortFlip(pInputPortCtx, &stVideoFrameBuffer);
    MHAL_DISP_InputPortEnable(pInputPortCtx, TRUE);

    MHAL_DISP_RegFlipConfig_t stRegFlipCfg;

    stRegFlipCfg.bEnable = 1;
    stRegFlipCfg.pCmdqInf = NULL;

    MHAL_DISP_SetRegFlipConfig(pDevCtx, &stRegFlipCfg);
#endif
#if defined(CONFIG_SSTAR_PNL)
    static void *pPnlDev = NULL;
    u32 u32DbgLevel;

    if(u32Interface == MHAL_DISP_INTF_LCD)
    {
        if (pPnlDev == NULL)
        {
            u32DbgLevel = 0; //0x0F;
            MhalPnlSetDebugLevel((void *)&u32DbgLevel);
            if (pstDispOut->pstPnlParaCfg)
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, PnlLink:%d\n",
                         __FUNCTION__, __LINE__, pstDispOut->pstPnlParaCfg->eLinkType);
                if(MhalPnlCreateInstance(&pPnlDev, pstDispOut->pstPnlParaCfg->eLinkType) == FALSE)
                {
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, PnlCreateInstance Fail\n", __FUNCTION__, __LINE__);
                    return;
                }
                MhalPnlSetParamConfig(pPnlDev,pstDispOut->pstPnlParaCfg);
                if(pstDispOut->pstPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
                {
                    MhalPnlSetMipiDsiConfig(pPnlDev, pstDispOut->pstMipiDsiCfg);
                }
            }
            else if (pstDispOut->pstUniPnlParaCfg)
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s %d, PnlLink:%d\n",
                         __FUNCTION__, __LINE__, pstDispOut->pstUniPnlParaCfg->eLinkType);
                if(MhalPnlCreateInstance(&pPnlDev, pstDispOut->pstUniPnlParaCfg->eLinkType) == FALSE)
                {
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "%s %d, PnlCreateInstance Fail\n", __FUNCTION__, __LINE__);
                    return;
                }
                MhalPnlSetUnifiedParamConfig(pPnlDev,pstDispOut->pstUniPnlParaCfg);

                if(pstDispOut->pstUniPnlParaCfg->eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF)
                {
                    Lcm_init_Ini(pstDispOut->pSpiCmdbuf,pstDispOut->PanelCmdArrayCnt,pstDispOut->stSpiCfg);
                }
            }
            else
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "PNL PARA is NULL");

                return;
            }

        }
    }
#elif defined(CONFIG_SSTAR_HDMITX)
    static void *pHdmitxCtx = NULL;
    MhalHdmitxAttrConfig_t stAttrCfg;
    MhalHdmitxSignalConfig_t stSignalCfg;
    MhalHdmitxMuteConfig_t stMuteCfg;
    MhalHdmitxHpdConfig_t stHpdCfg;

    if(u32Interface == MHAL_DISP_INTF_HDMI)
    {
        if(pHdmitxCtx == NULL)
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO,  "interface %d w %d h %d clock %d\n", u32Interface, pstDispOut->u32Width, pstDispOut->u32Height, pstDispOut->u32Clock);
            if(MhalHdmitxCreateInstance(&pHdmitxCtx, 0) != E_MHAL_HDMITX_RET_SUCCESS)
            {
                printf("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
                return;
            }
            //MhalHdmitxSetDebugLevel(pHdmitxCtx, 0x3F);

            stHpdCfg.u8GpioNum = 26;
            MhalHdmitxSetHpdConfig(pHdmitxCtx, &stHpdCfg);

            stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
            stMuteCfg.bMute = 1;
            MhalHdmitxSetMute(pHdmitxCtx, &stMuteCfg);

            stSignalCfg.bEn = 0;
            MhalHdmitxSetSignal(pHdmitxCtx, &stSignalCfg);

            stAttrCfg.bVideoEn = 1;
            stAttrCfg.enInColor    = E_MHAL_HDMITX_COLOR_RGB444;
            stAttrCfg.enOutColor   = E_MHAL_HDMITX_COLOR_RGB444;
            stAttrCfg.enOutputMode = E_MHAL_HDMITX_OUTPUT_MODE_HDMI;
            stAttrCfg.enColorDepth = E_MHAL_HDMITX_CD_24_BITS;
            stAttrCfg.enTiming     = _BootLogoGetHdmitxTimingId(pstDispOut->u32Width, pstDispOut->u32Height, pstDispOut->u32Clock);

            stAttrCfg.bAudioEn = 1;
            stAttrCfg.enAudioFreq = E_MHAL_HDMITX_AUDIO_FREQ_48K;
            stAttrCfg.enAudioCh   = E_MHAL_HDMITX_AUDIO_CH_2;
            stAttrCfg.enAudioFmt  = E_MHAL_HDMITX_AUDIO_FORMAT_PCM;
            stAttrCfg.enAudioCode = E_MHAL_HDMITX_AUDIO_CODING_PCM;
            MhalHdmitxSetAttr(pHdmitxCtx, &stAttrCfg);

            stSignalCfg.bEn = 1;
            MhalHdmitxSetSignal(pHdmitxCtx, &stSignalCfg);

            stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
            stMuteCfg.bMute = 0;
            MhalHdmitxSetMute(pHdmitxCtx, &stMuteCfg);
        }
    }
#endif
#if defined(CONFIG_SSTAR_RGN)
    MHAL_RGN_GopType_e eGopId = E_MHAL_GOP_DISP0_UI;
    MHAL_RGN_GopGwinId_e eGwinId = E_MHAL_GOP_GWIN_ID_0;
    MHAL_RGN_GopWindowConfig_t stSrcWinCfg;
    MHAL_RGN_GopWindowConfig_t stDstWinCfg;
    u8 bInitRgn = 0;

    switch (pstAspectRatio->enAspectRatio)
    {
        case EN_ASPECT_RATIO_ZOOM:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = 0;
            stDstWinCfg.u32Y = 0;
            stDstWinCfg.u32Width  = u16DispOutWidht;
            stDstWinCfg.u32Height = u16DispOutHeight;
        }
        break;
        case EN_ASPECT_RATIO_CENTER:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = (u16DispOutWidht - stSrcWinCfg.u32Width) / 2;
            stDstWinCfg.u32Y = (u16DispOutHeight - stSrcWinCfg.u32Height) / 2;
            stDstWinCfg.u32Width = stSrcWinCfg.u32Width;
            stDstWinCfg.u32Height = stSrcWinCfg.u32Height;
        }
        break;
        case EN_ASPECT_RATIO_USER:
        {
            stSrcWinCfg.u32X = 0;
            stSrcWinCfg.u32Y = 0;
            stSrcWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stSrcWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
            stDstWinCfg.u32X = pstAspectRatio->u32DstX;
            stDstWinCfg.u32Y = pstAspectRatio->u32DstY;
            stDstWinCfg.u32Width = min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth);
            stDstWinCfg.u32Height = min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight);
        }
        break;
        default:
            return;
    }
    if (!bInitRgn)
    {
        MHAL_RGN_GopInit();
        bInitRgn = 1;
    }
    MHAL_RGN_GopSetBaseWindow(eGopId, &stSrcWinCfg, &stDstWinCfg);

    MHAL_RGN_GopGwinSetPixelFormat(eGopId, eGwinId, E_MHAL_RGN_PIXEL_FORMAT_ARGB8888);

    MHAL_RGN_GopGwinSetWindow(eGopId, eGwinId, min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth), min(u16DispOutHeight, (u16)pstDispOut->u32ImgHeight), min(u16DispOutWidht, (u16)pstDispOut->u32ImgWidth) * 4, 0, 0);

    MHAL_RGN_GopGwinSetBuffer(eGopId, eGwinId, (MS_PHYADDR)pstDispOut->phyAddr);

    MHAL_RGN_GopSetAlphaZeroOpaque(eGopId, FALSE, FALSE, E_MHAL_RGN_PIXEL_FORMAT_ARGB8888);

    MHAL_RGN_GopSetAlphaType(eGopId, eGwinId, E_MHAL_GOP_GWIN_ALPHA_PIXEL, 0xFF);

    MHAL_RGN_GopGwinEnable(eGopId,eGwinId);
#endif
#if 1 //defined(CONFIG_SSTAR_RGN)
    gu32FrameBuffer = (u32)pstDispOut->phyAddr + BOOTLOGO_VIRTUAL_ADDRESS_OFFSET;
    gu32DispWidth = pstDispOut->u32ImgWidth;
    gu32DispHeight = pstDispOut->u32ImgHeight;
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Framebuffer addr 0x%x width %d height %d\n", gu32FrameBuffer, gu32DispWidth, gu32DispHeight);
#endif

#endif
}
#ifndef ALIGN_UP
#define ALIGN_UP(val, alignment) ((( (val)+(alignment)-1)/(alignment))*(alignment))
#endif
SS_SHEADER_DispPnl_u *_BootDbTable(SS_SHEADER_DispInfo_t *pHeadInfo, SS_SHEADER_DispPnl_u *pDispPnl, u8 *pbNeedRestorePartition)
{
    char *pDispTable = NULL;
    SS_SHEADER_DisplayDevice_e enDevice = EN_DISPLAY_DEVICE_NULL;
    SS_SHEADER_DispPnl_u *puDispPnlLoop = NULL;
    u32 u32Idx = 0;
    char tmp[64];

    pDispTable = getenv("dispout");
    enDevice = *(SS_SHEADER_DisplayDevice_e *)pDispPnl;
    *pbNeedRestorePartition = 0;
    switch (enDevice)
    {
#if defined(CONFIG_SSTAR_PNL)
        case EN_DISPLAY_DEVICE_LCD:
        {
            if (!pDispTable)
            {
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache off");
                run_command(tmp, 0);
                setenv("dispout", (const char *)pDispPnl->stPnlPara.au8PanelName);
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "dispout is empty, set %s as default.\n", pDispPnl->stPnlPara.au8PanelName);
                saveenv();
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache on");
                run_command(tmp, 0);

                return pDispPnl;
            }
            if (!strcmp((const char *)pDispPnl->stPnlPara.au8PanelName, pDispTable))
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "DB Table and setting match.\n");

                return pDispPnl;
            }
            else
            {
                for (u32Idx = 0, puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)pHeadInfo + pHeadInfo->stDataInfo.u32SubHeadSize);
                      u32Idx < pHeadInfo->stDataInfo.u32SubNodeCount; u32Idx++)
                {
                    if (!strcmp((const char *)puDispPnlLoop->stPnlPara.au8PanelName, pDispTable))
                    {
                        pHeadInfo->u32FirstUseOffset = (u32)puDispPnlLoop - (u32)pHeadInfo + sizeof(SS_HEADER_Desc_t);
                        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Find dispout: %s, first offset 0x%x\n", pDispTable, pHeadInfo->u32FirstUseOffset);
                        *pbNeedRestorePartition = 1;

                        return puDispPnlLoop;
                    }
                    puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)puDispPnlLoop + ALIGN_UP(sizeof(SS_SHEADER_PnlPara_t) + puDispPnlLoop->stPnlPara.stMipiDsiCfg.u32CmdBufSize, 4));
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Disp header size (%d) mhal pnl para size(%d) spi cmd size(%d) offset 0x%x\n", sizeof(SS_SHEADER_PnlPara_t), sizeof(MhalPnlParamConfig_t), puDispPnlLoop->stPnlPara.stMipiDsiCfg.u32CmdBufSize, (u32)puDispPnlLoop - (u32)pHeadInfo + (u32)sizeof(SS_HEADER_Desc_t));

                }
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found dispout: %s\n", pDispTable);

                return NULL;
            }
        }
        break;
#endif
        case EN_DISPLAY_DEVICE_HDMI:
        case EN_DISPLAY_DEVICE_VGA:
        {
            if (!pDispTable)
            {
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache off");
                run_command(tmp, 0);
                setenv("dispout", (const char *)pDispPnl->stDispOut.au8ResName);
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "dispout is empty, set %s as default.\n", pDispPnl->stDispOut.au8ResName);
                saveenv();
                memset(tmp,0,sizeof(tmp));
                snprintf(tmp, sizeof(tmp) - 1,"dcache on");
                run_command(tmp, 0);

                return pDispPnl;
            }
            if (!strcmp((const char *)pDispPnl->stDispOut.au8ResName, pDispTable))
            {
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "DB Table and setting match.\n");

                return pDispPnl;
            }
            else
            {
                for (u32Idx = 0, puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)pHeadInfo + pHeadInfo->stDataInfo.u32SubHeadSize);
                      u32Idx < pHeadInfo->stDataInfo.u32SubNodeCount; u32Idx++)
                {
                    if (!strcmp((const char *)puDispPnlLoop->stDispOut.au8ResName, pDispTable))
                    {
                        pHeadInfo->u32FirstUseOffset = (u32)puDispPnlLoop - (u32)pHeadInfo + sizeof(SS_HEADER_Desc_t);
                        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Find dispout: %s, first offset 0x%x\n", pDispTable, pHeadInfo->u32FirstUseOffset);
                        *pbNeedRestorePartition = 1;

                        return puDispPnlLoop;
                    }
                    puDispPnlLoop = (SS_SHEADER_DispPnl_u *)((u8 *)puDispPnlLoop + sizeof(SS_SHEADER_DispConfig_t));
                }
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found dispout: %s\n", pDispTable);

                return NULL;
            }
        }
        break;
        default:
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Device fail\n");

            return NULL;
        }
    }
}
#if (MEASURE_BOOT_LOGO_TIME == 1)
static u64 _gettime(void)
{
    u64 cval;

    asm volatile("mrrc p15, 0, %Q0, %R0, c14" : "=r" (cval));
    return cval;
}
#endif
#if defined(CONFIG_SSTAR_INI_PARSER)
#include "vsprintf.h"
#include "iniparser.h"

#if defined(CONFIG_FS_LITTLEFS)
#include "littlefs.h"
#elif defined(CONFIG_FS_FIRMWAREFS)
#include "firmwarefs.h"
#else
#error "CONFIG_FS_LITTLEFS or CONFIG_FS_FIRMWAREFS must be defined"
#endif

static S32 inifs_mount(char *partition, char *mnt_path)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_mount(partition, mnt_path);
#else
    return firmwarefs_mount(partition, mnt_path);
#endif
}

static void inifs_unmount(void)
{
#if defined(CONFIG_FS_LITTLEFS)
    littlefs_unmount();
#else
    firmwarefs_unmount();
#endif
}

static void *inifs_open(char *filename, U32 flags, U32 mode)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_open(filename, flags, mode);
#else
    return firmwarefs_open(filename, flags, mode);
#endif
}

static S32 inifs_close(void *fd)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_close(fd);
#else
    return firmwarefs_close(fd);
#endif
}

static S32 inifs_read(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_read(fd, buf, count);
#else
    return firmwarefs_read(fd, buf, count);
#endif
}

static S32 inifs_write(void *fd, void *buf, U32 count)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_write(fd, buf, count);
#else
    return firmwarefs_write(fd, buf, count);
#endif
}

static S32 inifs_lseek(void *fd, S32 offset, S32 whence)
{
#if defined(CONFIG_FS_LITTLEFS)
    return littlefs_lseek(fd, offset, whence);
#else
    return firmwarefs_lseek(fd, offset, whence);
#endif
}

static int _GetLogoReservedAddr(u32 *pu32Addr, u32 *pu32Size)
{
    char *pEnv = NULL;
    char *pHandle = NULL;
    int miu = 0;
    char sz[128];
    char start[128];
    char end[128];

    *pu32Addr = 0;
    *pu32Size = 0;
    pEnv = getenv("bootargs");
    if (!pEnv)
        return -1;
    pHandle = strstr(pEnv, "mmap_reserved=fb");
    if (!pHandle)
        return -1;
    sscanf(pHandle, "mmap_reserved=fb,miu=%d,sz=%[^,],max_start_off=%[^,],max_end_off=%[^ ]", &miu, sz, start, end);
    *pu32Size = simple_strtoul(sz, NULL, 16);
    *pu32Addr = simple_strtoul(start, NULL, 16);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Parsing bootargs size 0x%x addr 0x%x\n", *pu32Size, *pu32Addr);

    return 0;
}

#define INI_GET_INT(ret, key) do{  \
            ret = (typeof(ret))iniparser_getint(d, key, -1); \
            if (ret == -1)  \
            {   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_INT: Get %s error\n", key);   \
                return -1;  \
            }   \
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%d\n", key, ret);   \
        }while(0);
#define INI_GET_STR(ret, key) do{  \
            ret = (typeof(ret))iniparser_getstring(d, key, NULL); \
            if (ret == NULL)  \
            {   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_STR: Get %s error\n", key);   \
                return -1;  \
            }   \
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%s\n", key, ret);   \
        }while(0);
#define INI_GET_PNL_INT(ret, section, key) do{  \
            char section_key[128];  \
            snprintf(section_key, 128, "%s:%s", section, key); \
            ret = (typeof(ret))iniparser_getint(d, section_key, -1); \
            if (ret == -1)  \
            {   \
                /*iniparser_freedict(d);*/  \
                /*inifs_unmount();*/ \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_INT: Get %s error\n", section_key);   \
                ret = 0; \
                /*return -1;*/  \
            }   \
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%d\n", section_key, ret);   \
        }while(0);
#define INI_GET_PNL_STR(ret, section, key) do{  \
            char section_key[128];  \
            snprintf(section_key, 128, "%s:%s", section, key); \
            ret = (typeof(ret))iniparser_getstring(d, section_key, NULL); \
            if (ret == NULL)  \
            {   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_STR: Get %s error\n", section_key);   \
                return -1;  \
            }   \
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s=%s\n", section_key, ret);   \
        }while(0);
#define INI_GET_PNL_ARRAY(ret, cnt, section, key) do{  \
            char section_key[128];  \
            char *holder = NULL;    \
            char *array_str = NULL; \
            char **array_str_holder = NULL; \
            int array_count;    \
            int array_loop_idx = 0; \
            snprintf(section_key, 128, "%s:%s", section, key); \
            array_str= iniparser_getstring(d, section_key, NULL); \
            if (array_str == NULL)  \
            {   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: Get %s error\n", section_key);   \
                return -1;  \
            }   \
            holder = iniparser_parser_string_to_array(array_str, &array_str_holder, &array_count); \
            if (holder == NULL) \
            {   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: Ret %s array error\n", section_key);   \
                return -1;  \
            }   \
            ret = (typeof(ret))malloc(sizeof(typeof(*ret)) * array_count);  \
            if (ret == NULL)    \
            {   \
                free(array_str_holder); \
                free(holder);   \
                iniparser_freedict(d);  \
                inifs_unmount(); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "INI_GET_PNL_ARRAY: malloc %s error\n", section_key);   \
                return -1;  \
            }   \
            cnt = array_count;  \
            for (array_loop_idx = 0; array_loop_idx < array_count; array_loop_idx++)    \
            {   \
                ret[array_loop_idx] = (typeof(*ret))simple_strtoul(array_str_holder[array_loop_idx], NULL, -1); \
                BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s[%d]=%x\n", section_key, array_loop_idx, ret[array_loop_idx]);   \
            }   \
            free(array_str_holder); \
            free(holder);   \
        }while(0);

static INI_FILE *ini_file_open(const char *path, const char *opt);
static int ini_file_close(INI_FILE *pfile_desc);
static char *ini_file_fgets(char *str_buf, int size, INI_FILE *file_desc);

static int _MiscPartiton(char * pstSectionName, u32 u32LogoId, DispOutAspectRatio *pstAspectRatio, LogoRotation_e eRot)
{
    char strENVName[] = "MISC";
    char strLogoName[50] = {0};
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64 u64Time0 = 0;
    u64 u64Time1 = 0;
    u32 u32TimeDiff = 0;

    u64Time0 = _gettime();
#endif
    memset(strLogoName, 0, sizeof(strLogoName));
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;
    int ret;

    ret = mtdparts_init();
    if (ret)
        return -1;

    ret = find_dev_and_part(strENVName, &dev, &pnum, &part);
    if (ret)
    {
        return -1;
    }

//disable for nor flash bootlogo
#if 0
    if (dev->id->type != MTD_DEV_TYPE_NAND)
    {
        puts("not a NAND device\n");
        return -1;
    }
#endif

#elif defined(CONFIG_MS_PARTITION)
    mxp_record rec;
    u32 idx;
    mxp_load_table();
    idx=mxp_get_record_index(strENVName);
    if(idx<0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "can not found mxp record: %s\n", strENVName);
        return -1;
    }

    if(0 != mxp_get_record_by_index(idx,&rec))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "failed to get MXP record with name: %s\n", strENVName);
        return -1;
    }
#else
    return -1;
#endif
    static u32 u32Shift = 0;
    DispOutCfg stDispOut;
    iniparser_file_opt ini_opt;
    dictionary *d;
    u32 u32Addr = 0;
    u32 u32Size = 0;
    u32 u32FileSize = 0;
    char *pFileName = NULL;
    char *inBuf = NULL;
#if defined(CONFIG_SSTAR_PNL)
    MhalPnlUnifiedParamConfig_t stPnlPara;
    char *pTargetPnlName = NULL;
#endif
    if (inifs_mount(strENVName, NULL) < 0)
        return -1;
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load("config.ini", &ini_opt);
    memset(&stDispOut, 0, sizeof(DispOutCfg));
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_eDeviceType", pstSectionName);
    INI_GET_INT(stDispOut.enDev, strLogoName);
    _GetLogoReservedAddr(&u32Addr, &u32Size);
    stDispOut.phyAddr = (u64)u32Addr;
    stDispOut.u32Size = u32Size;
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispWidth", pstSectionName);
    INI_GET_INT(stDispOut.u32Width, strLogoName);
    INI_GET_PNL_INT(stDispOut.u32DevId, pstSectionName,"m_wDevId");
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispHeight", pstSectionName);
    INI_GET_INT(stDispOut.u32Height, strLogoName);
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispFps", pstSectionName);
    INI_GET_INT(stDispOut.u32Clock, strLogoName);
    switch (stDispOut.enDev)
    {
        case EN_DISPLAY_DEVICE_NULL:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
        case EN_DISPLAY_DEVICE_LCD:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_LCD;
#if defined(CONFIG_SSTAR_PNL)
            memset(&stPnlPara, 0, sizeof(MhalPnlUnifiedParamConfig_t));
            snprintf(strLogoName, sizeof(strLogoName), "%s:m_wDispWidth", pstSectionName);
            INI_GET_INT(stDispOut.u32Width, strLogoName);
            snprintf(strLogoName, sizeof(strLogoName), "%s:m_sParaTarget", pstSectionName);
            INI_GET_STR(pTargetPnlName, strLogoName);
            INI_GET_PNL_STR(stPnlPara.pPanelName, pTargetPnlName, "m_pPanelName");
            INI_GET_PNL_INT(stPnlPara.eLinkType, pTargetPnlName, "m_ePanelIntfType");
            stPnlPara.u8TgnTimingFlag = 1;
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16HSyncWidth, pTargetPnlName, "m_wPanelHSyncWidth");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16HSyncBackPorch, pTargetPnlName, "m_wPanelHSyncBackPorch");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16VSyncWidth, pTargetPnlName, "m_wPanelVSyncWidth");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16VSyncBackPorch, pTargetPnlName, "m_wPanelVBackPorch");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16HStart, pTargetPnlName, "m_wPanelHStart");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16VStart, pTargetPnlName, "m_wPanelVStart");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16HActive, pTargetPnlName, "m_wPanelWidth");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16VActive, pTargetPnlName, "m_wPanelHeight");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16HTotal, pTargetPnlName, "m_wPanelHTotal");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u16VTotal, pTargetPnlName, "m_wPanelVTotal");
            INI_GET_PNL_INT(stPnlPara.stTgnTimingInfo.u32Dclk, pTargetPnlName, "m_wPanelDCLK");
            stPnlPara.u8TgnPolarityFlag = 1;
            INI_GET_PNL_INT(stPnlPara.stTgnPolarityInfo.u8InvDCLK, pTargetPnlName, "m_bPanelInvDCLK");
            INI_GET_PNL_INT(stPnlPara.stTgnPolarityInfo.u8InvDE, pTargetPnlName, "m_bPanelInvDE");
            INI_GET_PNL_INT(stPnlPara.stTgnPolarityInfo.u8InvHSync, pTargetPnlName, "m_bPanelInvHSync");
            INI_GET_PNL_INT(stPnlPara.stTgnPolarityInfo.u8InvVSync, pTargetPnlName, "m_bPanelInvVSync");
            stPnlPara.u8TgnRgbSwapFlag = 1;
            INI_GET_PNL_INT(stPnlPara.stTgnRgbSwapInfo.u8SwapChnR, pTargetPnlName, "m_ucPanelSwapChnR");
            INI_GET_PNL_INT(stPnlPara.stTgnRgbSwapInfo.u8SwapChnG, pTargetPnlName, "m_ucPanelSwapChnG");
            INI_GET_PNL_INT(stPnlPara.stTgnRgbSwapInfo.u8SwapChnB, pTargetPnlName, "m_ucPanelSwapChnB");
            INI_GET_PNL_INT(stPnlPara.stTgnRgbSwapInfo.u8SwapRgbML, pTargetPnlName, "m_ucPanelSwapRgbML");
            stPnlPara.stTgnRgbSwapInfo.u8SwapChnR =
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnR == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_B;
            stPnlPara.stTgnRgbSwapInfo.u8SwapChnG =
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnG == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_G;
            stPnlPara.stTgnRgbSwapInfo.u8SwapChnB =
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_R ? E_MHAL_PNL_RGB_SWAP_R:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_G ? E_MHAL_PNL_RGB_SWAP_G:
                        stPnlPara.stTgnRgbSwapInfo.u8SwapChnB == E_MHAL_PNL_RGB_SWAP_B ? E_MHAL_PNL_RGB_SWAP_B:
                        E_MHAL_PNL_RGB_SWAP_R;
            stPnlPara.u8TgnOutputBitMdFlag = 1;
            INI_GET_PNL_INT(stPnlPara.eOutputFormatBitMode, pTargetPnlName, "m_eOutputFormatBitMode");
            INI_GET_PNL_INT(stPnlPara.u16PadMux, pTargetPnlName, "m_wPadmux");
            if(stPnlPara.u16PadMux)
                stPnlPara.u8TgnPadMuxFlag = 1;
            INI_GET_PNL_INT(stPnlPara.u32FDclk, pTargetPnlName, "m_wPanelFixedDCLK");
            if(stPnlPara.u32FDclk)
                stPnlPara.u8TgnFixedDclkFlag = 1;
            stPnlPara.u8TgnSscFlag = 1;
            INI_GET_PNL_INT(stPnlPara.stTgnSscInfo.u16SpreadSpectrumStep, pTargetPnlName, "m_wSpreadSpectrumFreq");
            INI_GET_PNL_INT(stPnlPara.stTgnSscInfo.u16SpreadSpectrumSpan, pTargetPnlName, "m_wSpreadSpectrumRatio");
            if (stPnlPara.eLinkType == E_MHAL_PNL_LINK_TTL_SPI_IF)
            {
                INI_GET_PNL_ARRAY(stDispOut.pSpiCmdbuf, stDispOut.PanelCmdArrayCnt, pTargetPnlName, "m_pSpiCmdTable");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.cs_gpio_num, pTargetPnlName, "m_wGpioSpiSCE");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.clk_gpio_num, pTargetPnlName, "m_wGpioSpiCLK");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.mosi_gpio_num, pTargetPnlName, "m_wGpioSpiSDA");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.rst_gpio_num, pTargetPnlName, "m_wGpioSpiRST");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.bl_gpio_num, pTargetPnlName, "m_wGpioBLEn");
                INI_GET_PNL_INT(stDispOut.stSpiCfg.cmd_mode, pTargetPnlName, "m_wSpiBitsMode");
            }else if (stPnlPara.eLinkType == E_MHAL_PNL_LINK_MIPI_DSI)
            {
                stPnlPara.u8MpdFlag = 1;
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8HsTrail, pTargetPnlName, "m_wHsTrail");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8HsPrpr, pTargetPnlName, "m_wHsPrpr");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8HsZero, pTargetPnlName, "m_wHsZero");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ClkHsPrpr, pTargetPnlName, "m_wClkHsPrpr");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ClkHsExit, pTargetPnlName, "m_wClkHsExit");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ClkTrail, pTargetPnlName, "m_wClkTrail");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ClkZero, pTargetPnlName, "m_wClkZero");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ClkHsPost, pTargetPnlName, "m_wClkHsPost");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8DaHsExit, pTargetPnlName, "m_wDaHsExit");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8ContDet, pTargetPnlName, "m_wContDet");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8Lpx, pTargetPnlName, "m_wLpx");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8TaGet, pTargetPnlName, "m_wTaGet");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8TaSure, pTargetPnlName, "m_wTaSure");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8TaGo, pTargetPnlName, "m_wTaGo");

                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Hactive, pTargetPnlName, "m_wHactive");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Hpw, pTargetPnlName, "m_wHpw");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Hbp, pTargetPnlName, "m_wHbp");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Hfp, pTargetPnlName, "m_wHfp");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Vactive, pTargetPnlName, "m_wVactive");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Vpw, pTargetPnlName, "m_wVpw");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Vbp, pTargetPnlName, "m_wVbp");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Vfp, pTargetPnlName, "m_wVfp");

                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Bllp, pTargetPnlName, "m_wBllp");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u16Fps, pTargetPnlName, "m_wFps");

                INI_GET_PNL_INT(stPnlPara.stMpdInfo.enLaneNum, pTargetPnlName, "m_eLaneNum");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.enFormat, pTargetPnlName, "m_eFormat");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.enCtrl, pTargetPnlName, "m_eCtrlMode");

                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8PolCh0, pTargetPnlName, "m_ucPolCh0");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8PolCh1, pTargetPnlName, "m_ucPolCh1");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8PolCh2, pTargetPnlName, "m_ucPolCh2");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8PolCh3, pTargetPnlName, "m_ucPolCh3");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8PolCh4, pTargetPnlName, "m_ucPolCh4");

                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8SwapCh0, pTargetPnlName, "m_ucClkLane");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8SwapCh1, pTargetPnlName, "m_ucDataLane0");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8SwapCh2, pTargetPnlName, "m_ucDataLane1");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8SwapCh3, pTargetPnlName, "m_ucDataLane2");
                INI_GET_PNL_INT(stPnlPara.stMpdInfo.u8SwapCh4, pTargetPnlName, "m_ucDataLane3");
                INI_GET_PNL_ARRAY(stPnlPara.stMpdInfo.pu8CmdBuf, stPnlPara.stMpdInfo.u32CmdBufSize, pTargetPnlName, "m_pCmdBuff");
            }
            stDispOut.pstUniPnlParaCfg = &stPnlPara;
#endif
            break;
        case EN_DISPLAY_DEVICE_HDMI:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_HDMI;
            stDispOut.u32Width = stDispOut.u32Width;
            stDispOut.u32Height = stDispOut.u32Height;
            stDispOut.u32Clock = stDispOut.u32Clock;
            break;
        case EN_DISPLAY_DEVICE_VGA:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_VGA;
            stDispOut.u32Width = stDispOut.u32Width;
            stDispOut.u32Height = stDispOut.u32Height;
            stDispOut.u32Clock = stDispOut.u32Clock;
            break;
        case EN_DISPLAY_DEVICE_CVBS:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_CVBS;
            stDispOut.u32Width = stDispOut.u32Width;
            stDispOut.u32Height = stDispOut.u32Height;
            stDispOut.u32Clock = stDispOut.u32Clock;
            break;
        default:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
    }
    snprintf(strLogoName, sizeof(strLogoName), "%s:m_sLogoFile%d", pstSectionName, u32LogoId);
    INI_GET_STR(pFileName, strLogoName);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Get file %s!\n", pFileName);
    void *fd = inifs_open(pFileName, O_RDONLY, 0);
    if (fd == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Open file %s error!\n", pFileName);
        iniparser_freedict(d);
        inifs_unmount();
        return -1;

    }
    u32FileSize = inifs_lseek(fd, 0, SEEK_END);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "File size 0x%x!\n", u32FileSize);
    inBuf = (char *)malloc(u32FileSize);
    if (inBuf == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Can not alloc buffer! size %d\n", u32FileSize);
        inifs_close(fd);
        iniparser_freedict(d);
        inifs_unmount();
        return -1;
    }
    u32FileSize = inifs_read(fd, inBuf, u32FileSize);
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Read size 0x%x!\n", u32FileSize);
    inifs_close(fd);
    iniparser_freedict(d);
    inifs_unmount();
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time1 = _gettime();
    u32TimeDiff = ((u32)(u64Time1 - u64Time0)) / 6;
    printf("MISC config parsering time diff %d us\n", u32TimeDiff);
#endif
#if defined(CONFIG_SSTAR_RGN)
    _BootJpdArgbCtrl(u32FileSize, (u32)inBuf, stDispOut.u32Size-u32Shift, (u32)stDispOut.phyAddr+u32Shift, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#else
    _BootJpdYuvCtrl(u32FileSize, (u32)inBuf, stDispOut.u32Size-u32Shift, (u32)stDispOut.phyAddr+u32Shift, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#endif
    flush_dcache_all();
    free(inBuf);
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time0 = _gettime();
    u32TimeDiff = ((u32)(u64Time0 - u64Time1)) / 6;
    printf("Jpeg decode Time diff %d us\n", u32TimeDiff);
#endif
    _BootDispCtrl(&stDispOut, pstAspectRatio, u32Shift);
    u32Shift += (ALIGN((stDispOut.u32ImgWidth*stDispOut.u32ImgHeight*3/2), 32)) ;
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "bootlogo shift :0x%x    -----\n", u32Shift);
    if(u32Shift >  u32Size)
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "bootlogo shift:0x%x out of size:0x%x  \n", u32Shift, u32Size);
#if defined(CONFIG_SSTAR_PNL)
    if (stPnlPara.eLinkType == E_MHAL_PNL_LINK_MIPI_DSI
        && stPnlPara.stMpdInfo.pu8CmdBuf)
    {
        free(stPnlPara.stMpdInfo.pu8CmdBuf);
    }
#endif
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time1 = _gettime();
    u32TimeDiff = ((u32)(u64Time1 - u64Time0)) / 6;
    printf("Display Time diff %d us\n", u32TimeDiff);
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "bootlogo exit. %s:%d\n",__FUNCTION__, __LINE__);
    return 0;
}
#endif

static int _LogoPartiton(u32 u32LogoId, DispOutAspectRatio *pstAspectRatio, LogoRotation_e eRot)
{
    #define LOGO_FLAHS_BASE     0x14000000

    u32     start, size;
    char strENVName[] = "LOGO";
    u32 idx;
    char strHeaderName[] = "SSTAR";
    void *pRawData = NULL;
    SS_HEADER_Desc_t *pHeader = NULL;
    SS_SHEADER_DataInfo_t *pDataHead = NULL;
    SS_SHEADER_DispInfo_t *pDispInfo = NULL;
    SS_SHEADER_DisplayDevice_e *penDevice = NULL;
    SS_SHEADER_DispPnl_u *puDispPnl = NULL;
    SS_SHEADER_PictureDataInfo_t *pstPictureInfo = NULL;
    u32 u32LogoCount = 0;
    u8 bResorePartition = 0;
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64 u64Time0 = 0;
    u64 u64Time1 = 0;
    u32 u32TimeDiff = 0;

    u64Time0 = _gettime();
#endif
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;
    int ret;

    ret = mtdparts_init();
    if (ret)
        return -1;

    ret = find_dev_and_part(strENVName, &dev, &pnum, &part);
    if (ret)
    {
        return -1;
    }

#if !defined(CONFIG_MS_NOR_ONEBIN) && !defined(CONFIG_MS_NAND_ONEBIN)
    if (dev->id->type != MTD_DEV_TYPE_NAND)
    {
        puts("not a NAND device\n");
        return -1;
    }
#endif

    start = part->offset;
    size = part->size;
#elif defined(CONFIG_MS_PARTITION) && !defined(CONFIG_MS_NOR_ONEBIN) && !defined(CONFIG_MS_NAND_ONEBIN)
    mxp_record rec;
    mxp_load_table();
    idx=mxp_get_record_index(strENVName);
    if(idx<0)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "can not found mxp record: %s\n", strENVName);
        return -1;
    }

    if(0 != mxp_get_record_by_index(idx,&rec))
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "failed to get MXP record with name: %s\n", strENVName);
        return -1;
    }
    start = rec.start;
    size = rec.size;
#else
    start = 0;
    size = 0;
    return -1;
#endif
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "%s in flash offset=0x%x size=0x%x\n",strENVName , start, size);

    pRawData = malloc(size);
    if(pRawData == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "allocate buffer fail\n");
        return -1;
    }
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
    char  cmd_str[128];
    sprintf(cmd_str, "nand read.e 0x%p %s", pRawData, strENVName);
    run_command(cmd_str, 0);
#else
    //sprintf(cmd_str, "sf probe; sf read 0x%p 0x%p 0x%p", pRawData, start, size);
    //run_command(cmd_str, 0);
    memcpy(pRawData, (void*)(U32)(start | LOGO_FLAHS_BASE), size);
#endif
    flush_cache((U32)pRawData, size);

    pHeader = pRawData;
    //Parsing Header
    for(idx = 0; idx < 5; idx++)
    {
        if( strHeaderName[idx] != *((U8 *)(pHeader->au8Tittle + idx)))
        {
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Header check fail\n");

            free(pRawData);
            return -1;
        }
    }
    pDataHead = (SS_SHEADER_DataInfo_t *)(pRawData + sizeof(SS_HEADER_Desc_t));
    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Header count %d\n", pHeader->u32DataInfoCnt);
    for (idx = 0; idx < pHeader->u32DataInfoCnt; idx++)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Name %s Sub head sz %d total sz %d node cnt %d\n", pDataHead->au8DataInfoName, pDataHead->u32SubHeadSize,
                                              pDataHead->u32DataTotalSize, pDataHead->u32SubNodeCount);
        if (!strcmp((char *)pDataHead->au8DataInfoName, "DISP"))
        {
            pDispInfo = (SS_SHEADER_DispInfo_t *)pDataHead;
            penDevice = (SS_SHEADER_DisplayDevice_e *)(pDispInfo->u32FirstUseOffset + pRawData);
            puDispPnl = (SS_SHEADER_DispPnl_u *)penDevice;
            puDispPnl = _BootDbTable(pDispInfo, puDispPnl, &bResorePartition);
            if (!puDispPnl)
            {
                free(pRawData);
                return -1;
            }
            if (bResorePartition)
            {
                flush_dcache_all();
#if defined(CONFIG_CMD_MTDPARTS) || defined(CONFIG_MS_SPINAND)
                char  cmd_str[128];
                sprintf(cmd_str, "dcache off");
                run_command(cmd_str, 0);
                sprintf(cmd_str, "nand erase.part %s", strENVName);
                run_command(cmd_str, 0);
                sprintf(cmd_str, "nand write.e 0x%p %s 0x%x", pRawData, strENVName, size);
                run_command(cmd_str, 0);
                sprintf(cmd_str, "dcache on");
                run_command(cmd_str, 0);
#else
                char  cmd_str[128];
                sprintf(cmd_str, "dcache off");
                run_command(cmd_str, 0);
                sprintf(cmd_str, "sf probe 0; sf erase 0x%x 0x%x", start, size);
                run_command(cmd_str, 0);
                sprintf(cmd_str, "sf write 0x%p 0x%x 0x%x", pRawData, start, size);
                run_command(cmd_str, 0);
                sprintf(cmd_str, "dcache on");
                run_command(cmd_str, 0);
#endif
            }
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "First offset %d out buf size 0x%x out buf addr 0x%x en %d\n", pDispInfo->u32FirstUseOffset, pDispInfo->u32DispBufSize, pDispInfo->u32DispBufStart, *penDevice);
            switch (*penDevice)
            {
#if defined(CONFIG_SSTAR_PNL)
                case EN_DISPLAY_DEVICE_LCD:
                {
                    puDispPnl->stPnlPara.stMipiDsiCfg.pu8CmdBuf = (u8 *)puDispPnl + sizeof(SS_SHEADER_PnlPara_t);
                }
                break;
#endif
                case EN_DISPLAY_DEVICE_HDMI:
                case EN_DISPLAY_DEVICE_VGA:
                {
                }
                break;
                default:
                {
                    BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Device fail\n");
                    free(pRawData);

                    return -1;
                }
            }
            break;
        }
        pDataHead = (SS_SHEADER_DataInfo_t *)((u8 *)pDataHead + pDataHead->u32SubHeadSize + pDataHead->u32DataTotalSize);
    }
    if (idx == pHeader->u32DataInfoCnt || puDispPnl == NULL || pDispInfo == NULL || penDevice == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found DISP header\n");
        free(pRawData);

        return -1;
    }
    pDataHead = (SS_SHEADER_DataInfo_t *)(pRawData + sizeof(SS_HEADER_Desc_t));
    for (idx = 0; idx < pHeader->u32DataInfoCnt; idx++)
    {
        if (!strcmp((char *)pDataHead->au8DataInfoName, "LOGO"))
        {
            pstPictureInfo = (SS_SHEADER_PictureDataInfo_t *)pDataHead;
            BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Total sz %d, Node cnt %d\n", pstPictureInfo->stDataInfo.u32DataTotalSize,
                                                 pstPictureInfo->stDataInfo.u32SubNodeCount);

            if (u32LogoId == u32LogoCount)
            {
                break;
            }
            u32LogoCount++;
        }
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_INFO, "Total size %d\n", pDataHead->u32SubHeadSize + pDataHead->u32DataTotalSize);
        pDataHead = (SS_SHEADER_DataInfo_t *)((u8 *)pDataHead + pDataHead->u32SubHeadSize + pDataHead->u32DataTotalSize);
    }
    if (idx == pHeader->u32DataInfoCnt || puDispPnl == NULL)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "Not found LOGO header\n");
        free(pRawData);

        return -1;
    }
    static u32 u32Shift = 0;
    DispOutCfg stDispOut;
    memset(&stDispOut, 0, sizeof(DispOutCfg));

    stDispOut.phyAddr = (u64)pDispInfo->u32DispBufStart;
    stDispOut.u32Size = pDispInfo->u32DispBufSize;
    switch (*penDevice)
    {
        case EN_DISPLAY_DEVICE_NULL:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
        case EN_DISPLAY_DEVICE_LCD:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_LCD;
#if defined(CONFIG_SSTAR_PNL)
            stDispOut.pstPnlParaCfg = &puDispPnl->stPnlPara.stPnlParaCfg;
            stDispOut.pstMipiDsiCfg = &puDispPnl->stPnlPara.stMipiDsiCfg;
#endif
            break;
        case EN_DISPLAY_DEVICE_HDMI:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_HDMI;
            stDispOut.u32Width = puDispPnl->stDispOut.u32Width;
            stDispOut.u32Height = puDispPnl->stDispOut.u32Height;
            stDispOut.u32Clock = puDispPnl->stDispOut.u32Clock;
            break;
        case EN_DISPLAY_DEVICE_VGA:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_VGA;
            stDispOut.u32Width = puDispPnl->stDispOut.u32Width;
            stDispOut.u32Height = puDispPnl->stDispOut.u32Height;
            stDispOut.u32Clock = puDispPnl->stDispOut.u32Clock;
            break;
        default:
            stDispOut.enDev = EN_DISPLAY_OUT_DEVICE_NULL;
            break;
    }
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time1 = _gettime();
    u32TimeDiff = ((u32)(u64Time1 - u64Time0)) / 6;
    printf("LOGO config parsering time diff %d us\n", u32TimeDiff);
#endif
#if defined(CONFIG_SSTAR_RGN)
    _BootJpdArgbCtrl(pstPictureInfo->stDataInfo.u32DataTotalSize, (u32)((s8 *)pstPictureInfo + pstPictureInfo->stDataInfo.u32SubHeadSize),
                     stDispOut.u32Size, (u32)stDispOut.phyAddr, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#else
    _BootJpdYuvCtrl(pstPictureInfo->stDataInfo.u32DataTotalSize, (u32)((s8 *)pstPictureInfo + pstPictureInfo->stDataInfo.u32SubHeadSize),
                     stDispOut.u32Size, (u32)stDispOut.phyAddr, (u16 *)&stDispOut.u32ImgWidth, (u16 *)&stDispOut.u32ImgHeight, eRot);
#endif
    flush_dcache_all();
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time0 = _gettime();
    u32TimeDiff = ((u32)(u64Time0 - u64Time1)) / 6;
    printf("Jpeg decode Time diff %d us\n", u32TimeDiff);
#endif
    _BootDispCtrl(&stDispOut, pstAspectRatio, u32Shift);
    free(pRawData);
#if (MEASURE_BOOT_LOGO_TIME == 1)
    u64Time1 = _gettime();
    u32TimeDiff = ((u32)(u64Time1 - u64Time0)) / 6;
    printf("Display Time diff %d us\n", u32TimeDiff);
#endif
    return 0;
}
int do_display (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    DispOutAspectRatio stAspectRatio;
    LogoRotation_e enRot = EN_LOGO_ROTATE_NONE;
    char *pRot = NULL;
    char *pSection_name = NULL;
    u32 u32LogoId = 0;

    if (argc != 5 && argc != 6)
    {
        BOOTLOGO_DBG(BOOTLOGO_DBG_LEVEL_ERR, "usage: bootlogo [logo_id] [aspect ratio '0: zoom' '1: center' '2: usr'] [x] [y]\n or\nbootlogo [logo_id] [aspect ratio '0: zoom' '1: center' '2: usr'] [x] [y] [section_name]\n");

        return 0;
    }
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "do_display++");
    u32LogoId = simple_strtoul(argv[1], NULL, 0);
    memset(&stAspectRatio, 0, sizeof(DispOutAspectRatio));
    pRot =  getenv("logo_rot");
    if (pRot)
        enRot = (LogoRotation_e)simple_strtoul(pRot, NULL, 0);
    stAspectRatio.enAspectRatio = simple_strtoul(argv[2], NULL, 0);
    stAspectRatio.u32DstX = simple_strtoul(argv[3], NULL, 0);
    stAspectRatio.u32DstY = simple_strtoul(argv[4], NULL, 0);
    pSection_name = (argc == 6 && argv[5] != NULL)? argv[5]: "LOGO";
#if defined(CONFIG_SSTAR_INI_PARSER)
    if (_MiscPartiton(pSection_name, u32LogoId, &stAspectRatio, enRot) == -1)
        _LogoPartiton(u32LogoId, &stAspectRatio, enRot);
#else
    _LogoPartiton(u32LogoId, &stAspectRatio, enRot);
#endif
    bootstage_mark_name(BOOTSTAGE_ID_ALLOC, "do_display--");

    return 0;
}
/**
 * draw Rectangle. the colormat of Framebuffer is ARGB8888
 */
void drawRect_rgb32 (int x0, int y0, int width, int height, int stride,
    int color,unsigned char* frameBuffer )
{
    int *dest = (int *)((char *) (frameBuffer)
        + y0 * stride + x0 * 4);

    int x, y;
    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color;
        }
        dest = (int *)((char *)dest + stride);
    }
}
void  drawRect_rgb12(int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel =2;
    const int red = (color & 0xff0000) >> (16 + 4);
    const int green = (color & 0xff00) >> (8 + 4);
    const int blue = (color & 0xff) >> 4;
    const short color16 = blue | (green << 4) | (red << (4+4)) |0xf000;
    short *dest = NULL;
    int x, y;

    dest = (short *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            dest[x] = color16;
        }
        dest = (short *)((char *)dest + stride);
    }
}

void  drawRect_Yuv_Y(int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel =1;

    const char color8 = color & 0xff;
    char *dest = NULL;
    int x, y;

    dest = (char *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            dest[x] = color8;
        }
        dest = (char *)((char *)dest + stride);
    }
}

void drawRect_rgb15 (int x0, int y0, int width, int height, int stride, int color, unsigned char* frameBuffer)
{
    const int bytesPerPixel = 2;
    const int red = (color & 0xff0000) >> (16 + 3);
    const int green = (color & 0xff00) >> (8 + 3);
    const int blue = (color & 0xff) >> 3;
    const short color15 = blue | (green << 5) | (red << (5 + 5)) | 0x8000;
    short *dest = NULL;
    int x = 0, y = 0;

    dest = (short *)((char *)(frameBuffer)
        + y0 * stride + x0 * bytesPerPixel);

    for (y = 0; y < height; ++y)
    {
        for (x = 0; x < width; ++x)
        {
            dest[x] = color15;
        }
        dest = (short *)((char *)dest + stride);
    }
}

#if defined(CONFIG_SSTAR_RGN)
#define BGCOLOR 0xFFFFFFFF
#define BARCOLOR 0xFF00FF00
#define BARCOLOR_DARK 0xFF00AEEF
#else
#define BGCOLOR 0x000000FF
#define BARCOLOR 0x000000A0
#define BARCOLOR_DARK 0x00000000
#endif

void do_bootfb_bar(u8 progress,char* message, u32 width, u32 height, u32 u32Framebuffer)
{

    int total_bar_width = 0;
    int bar_x0 = 0;
    int bar_y0 = 0;
    int bar_width = 0;
    int bar_height = 0;
    #if defined(CONFIG_SSTAR_RGN)
    int stride = width * 4;
    #else
    int stride = width;//YUV, y only
    #endif

    total_bar_width = width * 618 / 1000;
    if (progress > 0 && progress < 100)
    {
        bar_x0 = (width - total_bar_width) / 2;
        bar_y0 = height * 750 / 1000;
        bar_width = total_bar_width * progress / 100;
        bar_height = 8;
        #if defined(CONFIG_SSTAR_RGN)
        drawRect_rgb32(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR, (unsigned char *)u32Framebuffer);
        #else
        drawRect_Yuv_Y(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR, (unsigned char *)u32Framebuffer);
        #endif
        //printf("w %d h %d x %d y %d\n", bar_width, bar_height, bar_x0, bar_y0);
    }
    if (progress < 100)
    {
        bar_x0 = (width - total_bar_width) / 2 + bar_width;
        bar_y0 = height * 750 / 1000;
        bar_width = total_bar_width - bar_width;
        bar_height = 8;
        #if defined(CONFIG_SSTAR_RGN)
        drawRect_rgb32(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR_DARK, (unsigned char *)u32Framebuffer);
        #else
        drawRect_Yuv_Y(bar_x0, bar_y0, bar_width, bar_height, stride, BARCOLOR_DARK, (unsigned char *)u32Framebuffer);
        #endif
        //printf("w %d h %d x %d y %d\n", bar_width, bar_height, bar_x0, bar_y0);
    }
    flush_dcache_all();
}

#if !defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
void do_bootfb_progress(u8 progress,char* message, u16 width, u16 height, u32 u32Framebuffer)
{
    static u8 bInited = 0;
    static u8 u8Completed = 0;
    static u8 u8PreMsgLen = 0;
    blit_props props;
    int completed = progress/10;
    unsigned char* framebuffer = (unsigned char*)(u32Framebuffer);
    int stride = width * 4;
    int i = 0;

    props.Buffer = (blit_pixel *)(framebuffer);
    props.BufHeight = height;
    props.BufWidth= width;
    props.Value = 0xFF000000;
    props.Wrap = 0;
    props.Scale = 4;


    int offset = 10+blit32_ADVANCE*props.Scale*strlen("[progress][100%]");
    int w = (width-offset)/20;
    if(!bInited)
    {
        drawRect_rgb32(0,0,width,height,stride,BGCOLOR,(unsigned char *)framebuffer);
        char str[]="[progess]";
        blit32_TextProps(props,10,height/3,str);

        for(i=0;i<completed;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
        for(i=completed;i<10;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR,framebuffer);
        }
        u8Completed = completed;
        bInited = 1;
    }
    else
    {
        for(i=u8Completed;i<completed;i++)
        {
            drawRect_rgb32(offset+w*i*2,height/3,w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
    }

    char str[] = "[   %]";
    str[3] = '0'+progress%10;
    if(progress>=10)
    {
        str[2] = '0'+(progress%100)/10;
    }
    if(progress>=100)
    {
        str[1] = '0'+progress/100;
    }
    offset = 10+blit32_ADVANCE*props.Scale*strlen("[progress]");
    drawRect_rgb32(offset,height/3,blit32_ADVANCE*props.Scale*strlen(str),blit32_ADVANCE*props.Scale,stride,BGCOLOR,framebuffer);
    blit32_TextProps(props,offset,height/3,str);
    if(message!=NULL)
    {
        props.Scale = 8;
        if(u8PreMsgLen>0)
        {
            offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
            drawRect_rgb32(offset,height*2/3,blit32_ADVANCE*props.Scale*u8PreMsgLen,(blit32_ADVANCE+1)*props.Scale,stride,BGCOLOR,framebuffer);
        }
        u8PreMsgLen  = strlen(message);
        offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
        blit32_TextProps(props,offset,height*2/3,message);
    }
    flush_dcache_all();
}
#else
void do_bootfb_progress_Yonly(u8 progress,char* message, u16 width, u16 height, u32 u32Framebuffer)
{
    static u8 bInited = 0;
    static u8 u8Completed = 0;
    static u8 u8PreMsgLen = 0;
    blit_props props;
    int completed = progress/10;
    unsigned char* framebuffer = (unsigned char*)(u32Framebuffer);
    int stride = width;
    int i = 0;

    props.Buffer = (blit_pixel *)(framebuffer);
    props.BufHeight = height;
    props.BufWidth= width;
    props.Value = 0x80;
    props.Wrap = 0;
    props.Scale = 4;

    int offset = 70+blit32_ADVANCE*props.Scale*strlen("[prog][100%]");
    int w = 18;

    if(!bInited)
    {
        //drawRect_Yuv_Y(0,0,width,height,stride,BGCOLOR,(unsigned char *)framebuffer);
        char str[]="[prog]";
        blit32_TextProps(props,60,height*2/3,str);

        for(i=0;i<completed;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
        for(i=completed;i<10;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR,framebuffer);
        }
        u8Completed = completed;
        bInited = 1;
    }
    else
    {
        for(i=u8Completed;i<completed;i++)
        {
            drawRect_Yuv_Y(70+w*i*2,(height*2/3 + blit32_HEIGHT*props.Scale*2),w,blit32_ADVANCE*props.Scale,stride,BARCOLOR_DARK,framebuffer);
        }
    }

    char str[] = "[   %]";
    str[3] = '0'+progress%10;
    if(progress>=10)
    {
        str[2] = '0'+(progress%100)/10;
    }
    if(progress>=100)
    {
        str[1] = '0'+progress/100;
    }
    offset = 60+blit32_ADVANCE*props.Scale*strlen("[prog]");
    drawRect_Yuv_Y(offset,height*2/3,blit32_ADVANCE*props.Scale*strlen(str),blit32_ADVANCE*props.Scale,stride,BGCOLOR,framebuffer);
    blit32_TextProps(props,offset,height*2/3,str);

    if(message!=NULL)
    {
        props.Scale = 4;
        if(u8PreMsgLen>0)
        {
            offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
            drawRect_Yuv_Y(offset,height*4/5,blit32_ADVANCE*props.Scale*u8PreMsgLen,(blit32_ADVANCE+1)*props.Scale,stride,BGCOLOR,framebuffer);
        }
        u8PreMsgLen  = strlen(message);
        offset = (width-blit32_ADVANCE*props.Scale*u8PreMsgLen)>>1;
        blit32_TextProps(props,offset,height*4/5,message);
    }
    flush_dcache_all();
}
#endif

void do_bootfb_blank(u16 width, u16 height, u32 u32Framebuffer)
{
    #if defined(CONFIG_SSTAR_RGN)
    memset((unsigned char*)(u32Framebuffer), 0, width * height * 4);
    #else
    memset((unsigned char*)(u32Framebuffer), 0x00, (width * height));
    memset((unsigned char*)(u32Framebuffer + (width * height)), 0x80, (width * height)/2);
    #endif
}

int do_bootfb (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 1//defined(CONFIG_SSTAR_RGN)
    if(argc < 2)
    {
        printf("usage\n");
        printf("1.bootframebuffer progressbar <percentage> [message]\n");
        printf("2.bootframebuffer blank\n");
    }
    else
    {
        if(strcmp("progressbar",argv[1])==0)
        {
            u8 progress = simple_strtoul(argv[2], NULL, 10);

            if(argc>=4)
            {
                #if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
                do_bootfb_progress_Yonly(progress,argv[3], gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #else
                do_bootfb_progress(progress,argv[3], gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #endif
            }
            else
            {
                #if defined(CONFIG_SSTAR_UPGRADE_UI_DRAW_YUV)
                do_bootfb_progress_Yonly(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #else
                do_bootfb_progress(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
                #endif
            }
        }
        else if(strcmp("bar",argv[1])==0)
        {
            u8 progress = simple_strtoul(argv[2], NULL, 10);

            do_bootfb_bar(progress,NULL, gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
        }
        else if(strcmp("blank",argv[1])==0)
        {
            do_bootfb_blank(gu32DispWidth, gu32DispHeight, gu32FrameBuffer);
        }
    }
#endif
    return 0;
}

U_BOOT_CMD(
    bootlogo, CONFIG_SYS_MAXARGS, 1,    do_display,
    "show bootlogo",
    NULL
);

U_BOOT_CMD(
    bootframebuffer, CONFIG_SYS_MAXARGS, 1,    do_bootfb,
    "boot framebuffer \n" \
    "                 1.bootframebuffer progressbar <percentage> [message]\n" \
    "                 2.bootframebuffer blank\n",
    NULL
);
#if defined(CONFIG_SSTAR_INI_PARSER)
static INI_FILE *ini_file_open(const char *path, const char *opt)
{
    U32 flags;

    if (!strcmp("r", opt))
        flags = O_RDONLY;
    else if (!strcmp("rw", opt))
        flags = O_RDWR;
    else if (!strcmp("w", opt))
        flags = O_WRONLY;
    else
        flags = O_RDONLY;

    return (INI_FILE *)inifs_open((char *)path, flags, 0);
}

static char *ini_file_fgets(char *str_buf, int size, INI_FILE *file_desc)
{
    char c;
    int read_cnt = 0;

    if (!size)
        return NULL;

    while(inifs_read((void *)file_desc, &c, 1))
    {
        str_buf[read_cnt] = c;
        read_cnt++;
        if (read_cnt > size)
            break;
        if (c == '\n')
        {
            if (read_cnt >= 1 && str_buf[read_cnt - 1] == '\r')
            {
                /*Windows format a line end with '\r' '\n'*/
                read_cnt--;
            }
            break;
        }
    }
    if (read_cnt)
    {
        str_buf[read_cnt] = 0;
    }
    else
    {
        return NULL;
    }

    return str_buf;
}

static int ini_file_close(INI_FILE *pfile_desc)
{
    return inifs_close((void *)pfile_desc);
}

static int ini_get_int(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    int val = 0;
    char parse[128];

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }
    memset(parse, 0, 128);
    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getint(d, parse, -1);
    printf("Partition %s file %s, section %s, key %s=%d\n", argv[1], argv[2], argv[3], argv[4], val);
    iniparser_freedict(d);
    inifs_unmount();
    return 0;
}

U_BOOT_CMD(
    ini_get_int, CONFIG_SYS_MAXARGS, 1,    ini_get_int,
    "ini_test\n",
    NULL
);
static int ini_get_str(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    char *val = 0;
    char parse[128];

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getstring(d, parse, "");
    printf("Partition %s file %s, section %s, key %s=%s\n", argv[1], argv[2], argv[3], argv[4], val);
    iniparser_freedict(d);
    inifs_unmount();

    return 0;
}

U_BOOT_CMD(
    ini_get_str, CONFIG_SYS_MAXARGS, 1,    ini_get_str,
    "ini_test\n",
    NULL
);
static int ini_get_str_array(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    iniparser_file_opt ini_opt;
    dictionary *d;
    char parse[128];
    char *val = NULL;
    char *ret = NULL;
    char **str_array = NULL;
    int str_array_count = 0;
    int i = 0;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }
    memset(parse, 0, 128);
    inifs_mount(argv[1], NULL);
    ini_opt.ini_open = ini_file_open;
    ini_opt.ini_close = ini_file_close;
    ini_opt.ini_fgets = ini_file_fgets;
    d = iniparser_load(argv[2], &ini_opt);
    snprintf(parse, 128, "%s:%s", argv[3], argv[4]);
    val = iniparser_getstring(d, parse, "");
    ret = iniparser_parser_string_to_array((const char *)val, &str_array, &str_array_count);
    printf("Partition %s file %s, section %s count %d\n", argv[1], argv[2], argv[3], str_array_count);
    for (i = 0; i < str_array_count; i++)
    {
        printf("idx[%d]=%s(%d)\n", i, str_array[i], (int)simple_strtoul(str_array[i], NULL, 16));
    }
    if (str_array)
    {
        free(str_array);
    }
    if (ret)
    {
        free(ret);
    }
    iniparser_freedict(d);
    inifs_unmount();
    return 0;
}

U_BOOT_CMD(
    ini_get_str_array, CONFIG_SYS_MAXARGS, 1,    ini_get_str_array,
    "ini_test\n",
    NULL
);

static int atoi(char *str)
{
    int value = 0;

    while(*str >= '0' && *str <= '9')
    {
        value *= 10;
        value += *str - '0';
        str++;
    }

    return value;
}

static int ini_get_filesize(cmd_tbl_t *cmdtp, int flag, int argc,
                            char * const argv[])
{
    void *fd = NULL;
    int ret;

    if (argc != 3)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDONLY, 0);
    if (fd != NULL)
    {
        int nFileLen = inifs_lseek(fd, 0, SEEK_END);

        printf("file len is %d\n", nFileLen);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_get_filesize, CONFIG_SYS_MAXARGS, 1,    ini_get_filesize,
    "ini_get_filesize\n",
    NULL
);

static int ini_get_buf(cmd_tbl_t *cmdtp, int flag, int argc,
                       char * const argv[])
{
    char value[32];
    void *fd = NULL;
    int ret;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDONLY, 0);
    if (fd != NULL)
    {
        int pos = atoi(argv[3]);
        int size = atoi(argv[4]);

        if (size > sizeof(value))
            size = sizeof(value);

        inifs_lseek(fd, pos, SEEK_SET);
        printf("seek is %d\n", pos);

        inifs_read(fd, value, size);
        printf("value is:");
        for(int i = 0; i < size; i++)
        {
            printf("0x%.2x,", value[i]);
        }
        printf("\n");
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_get_buf, CONFIG_SYS_MAXARGS, 1,    ini_get_buf,
    "ini_get_buf\n",
    NULL
);

static int ini_put_buf(cmd_tbl_t *cmdtp, int flag, int argc,
                       char * const argv[])
{
    char value[32];
    void *fd = NULL;
    int ret;

    if (argc != 5)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_RDWR, 0);
    if (fd != NULL)
    {
        int pos = atoi(argv[3]);
        int size = strlen(argv[4]);
        char *content = argv[4];
        unsigned char lo, hi;

        if ((size % 2) != 0)
        {
            fprintf(stderr, "ini_put_buf:invalid length %d ((%d %% 2) != 0)\n",
                    size, size);
            inifs_close(fd);
            inifs_unmount();
            return 0;
        }

        printf("ini_put_buf: put %s at %d, size=%d\n",
               content, pos, size);

        printf("fd=%p\n", fd);
        if (size > (sizeof(value)*2))
            size = sizeof(value)*2;

        int j;
        int i;
        for (i = 0, j = 0; i < size; i += 2, j++)
        {
            if (content[i] >= 'A' && content[i] <= 'Z')
            {
                hi = content[i] - 'A' + 10;
            } else if (content[i] >= 'a' && content[i] <= 'z')
            {
                hi = content[i] - 'a' + 10;
            } else if (content[i] >= '0' && content[i] <= '9')
            {
                hi = content[i] - '0';
            } else
            {
                fprintf(stderr, "ini_put_buf:invalid value 0x%x at %d\n",
                        (u32)content[i], i);
                inifs_close(fd);
                inifs_unmount();
                return 0;
            }

            if (content[i + 1] >= 'A' && content[i + 1] <= 'Z')
            {
                lo = content[i + 1] - 'A' + 10;
            } else if (content[i + 1] >= 'a' && content[i + 1] <= 'z')
            {
                lo = content[i + 1] - 'a' + 10;
            } else if (content[i + 1] >= '0' && content[i + 1] <= '9')
            {
                lo = content[i + 1] - '0';
            } else
            {
                fprintf(stderr, "ini_put_buf:invalid value 0x%x at %d\n",
                        (u32)content[i + 1], i + 1);
                inifs_close(fd);
                inifs_unmount();
                return 0;
            }
            value[j] = (unsigned char)(hi << 4 | lo);
        }
        printf("fd=%p,%d,%d\n", fd, j, size);

        inifs_lseek(fd, pos, SEEK_SET);
        printf("seek is %d\n", pos);

        ret = inifs_write(fd, value, j);
        printf("write:%d\n", ret);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_put_buf, CONFIG_SYS_MAXARGS, 1,    ini_put_buf,
    "ini_put_buf\n",
    NULL
);

static int ini_create_file(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    int len;
    void *fd = NULL;
    int ret;

    if (argc != 4)
    {
        printf("arg error!\n");
        return 0;
    }

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], O_CREAT|O_RDWR, 0);
    if (fd != NULL)
    {
        len = inifs_write(fd, argv[3], strlen(argv[3]));
        printf("write len is %d!", len);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_create_file, CONFIG_SYS_MAXARGS, 1,    ini_create_file,
    "ini_create_file\n",
    NULL
);

static int ini_write_file(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    void *fd = NULL;
    int ret;
    int flags;
    int offset;

    if (argc != 6)
    {
        printf("arg error!\n");
        return 0;
    }
    /**
     * argv[3] is mode
     * O_RDONLY = 00000000 (Open a file as read only)
     * O_WRONLY = 00000001 (Open a file as write only)
     * O_RDWR   = 00000002 (Open a file as read and write)
     * O_CREAT  = 00000100 (Create a file if it does not exist)
     * O_TRUNC  = 00001000 (Truncate the existing file to zero size)
     * O_APPEND = 00002000 (Move to end of file before every write)
     *
     * append + write is 1025
     * create + write is 65
     */
    flags = atoi(argv[3]);
    offset = atoi(argv[5]);

    ret = inifs_mount(argv[1], NULL);
    if (ret)
    {
        fprintf(stderr, "inifs_mount return error %d\n", ret);
        return ret;
    }

    fd = inifs_open(argv[2], flags, 0);
    if (fd != NULL)
    {
        if (offset > 0)
            inifs_lseek(fd, offset, SEEK_SET);
        ret = inifs_write(fd, argv[4], strlen(argv[4]));
        printf("write return %d\n", ret);
    } else
    {
        fprintf(stderr, "inifs_open return NULL\n");
        inifs_unmount();
        return 0;
    }

    ret = inifs_close(fd);
    if (ret)
    {
        fprintf(stderr, "inifs_close return error %d\n", ret);
    }
    inifs_unmount();
    return ret;
}

U_BOOT_CMD(
    ini_write_file, CONFIG_SYS_MAXARGS, 1,    ini_write_file,
    "ini_write_file\n",
    NULL
);

#endif
