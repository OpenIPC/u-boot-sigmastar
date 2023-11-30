/*
* mhal_pnl_datatype.h- Sigmastar
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


/**
 * \defgroup HAL_PNL_group  HAL_PNL driver
 * @{
 */
#ifndef __MHAL_PNL_DATATYPE_H__
#define __MHAL_PNL_DATATYPE_H__



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
/// Define PANEL Signaling Type
typedef enum
{
    E_MHAL_PNL_LINK_TTL,                              ///< TTL  type
    E_MHAL_PNL_LINK_LVDS,                             ///< LVDS type
    E_MHAL_PNL_LINK_RSDS,                             ///< RSDS type
    E_MHAL_PNL_LINK_MINILVDS,                         ///< TCON
    E_MHAL_PNL_LINK_ANALOG_MINILVDS,                  ///< Analog TCON
    E_MHAL_PNL_LINK_DIGITAL_MINILVDS,                 ///< Digital TCON
    E_MHAL_PNL_LINK_MFC,                              ///< Ursa (TTL output to Ursa)
    E_MHAL_PNL_LINK_DAC_I,                            ///< DAC output
    E_MHAL_PNL_LINK_DAC_P,                            ///< DAC output
    E_MHAL_PNL_LINK_PDPLVDS,                          ///< For PDP(Vsync use Manually MODE)
    E_MHAL_PNL_LINK_EXT,                              ///< EXT LPLL TYPE
    E_MHAL_PNL_LINK_MIPI_DSI,                         ///< Mipi DSI
    E_MHAL_PNL_LINK_BT656,                            ///< BT656
    E_MHAL_PNL_LINK_BT601,                            ///< BT601
    E_MHAL_PNL_LINK_BT1120,                           ///< BT1120
    E_MHAL_PNL_LINK_MCU_TYPE,                         ///< MCU Type
    E_MHAL_PNL_LINK_SRGB,                             ///< sRGB
    E_MHAL_PNL_LINK_TTL_SPI_IF,                       ///< TTL with SPI init interface
}MhalPnlLinkType_e;

typedef enum
{
    E_MHAL_PNL_ASPECT_RATIO_4_3    = 0,         ///< set aspect ratio to 4 : 3
    E_MHAL_PNL_ASPECT_RATIO_WIDE,               ///< set aspect ratio to 16 : 9
    E_MHAL_PNL_ASPECT_RATIO_OTHER,              ///< resvered for other aspect ratio other than 4:3/ 16:9
}MhalPnlAspectRatio_e;

/// Define TI bit mode
typedef enum
{
    E_MHAL_PNL_TI_10BIT_MODE = 0,
    E_MHAL_PNL_TI_8BIT_MODE = 2,
    E_MHAL_PNL_TI_6BIT_MODE = 3,
} MhalPnlTiBitMode_e;

/// Define which panel output timing change mode is used to change VFreq for same panel
typedef enum
{
    E_MHAL_PNL_CHG_DCLK   = 0,      ///<change output DClk to change Vfreq.
    E_MHAL_PNL_CHG_HTOTAL = 1,      ///<change H total to change Vfreq.
    E_MHAL_PNL_CHG_VTOTAL = 2,      ///<change V total to change Vfreq.
} MhalPnlOutputTimingMode_e;

/// Define panel output format bit mode
typedef enum
{
    E_MHAL_PNL_OUTPUT_10BIT_MODE  = 0, //default is 10bit, becasue 8bit panel can use 10bit config and 8bit config.
    E_MHAL_PNL_OUTPUT_6BIT_MODE   = 1, //but 10bit panel(like PDP panel) can only use 10bit config.
    E_MHAL_PNL_OUTPUT_8BIT_MODE   = 2, //and some PDA panel is 6bit.
    E_MHAL_PNL_OUTPUT_565BIT_MODE = 3, // 565
} MhalPnlOutputFormatBitMode_e;

/// Define RGB data type and LCD data bus
typedef enum
{
	E_MHAL_PNL_RGB_DTYPE_RGB888,
	E_MHAL_PNL_RGB_DTYPE_RGB666,
	E_MHAL_PNL_RGB_DTYPE_RGB565,
	E_MHAL_PNL_RGB_DTYPE_RGB444,
	E_MHAL_PNL_RGB_DTYPE_RGB333,
	E_MHAL_PNL_RGB_DTYPE_RGB332,
	E_MHAL_PNL_RGB_DTYPE_BGR888,
	E_MHAL_PNL_RGB_DTYPE_BGR666,
	E_MHAL_PNL_RGB_DTYPE_BGR565,
	E_MHAL_PNL_RGB_DTYPE_BGR444,
	E_MHAL_PNL_RGB_DTYPE_BGR333,
	E_MHAL_PNL_RGB_DTYPE_BGR332,
	E_MHAL_PNL_RGB_DTYPE_YUV422_UY0VY1,
	E_MHAL_PNL_RGB_DTYPE_YUV422_VY0UY1,
	E_MHAL_PNL_RGB_DTYPE_YUV422_UY1VY0,
	E_MHAL_PNL_RGB_DTYPE_YUV422_VY1UY0,
	E_MHAL_PNL_RGB_DTYPE_YUV422_Y0UY1V,
	E_MHAL_PNL_RGB_DTYPE_YUV422_Y0VY1U,
	E_MHAL_PNL_RGB_DTYPE_YUV422_Y1UY0V,
	E_MHAL_PNL_RGB_DTYPE_YUV422_Y1VY0U,
} MhalPnlRgbDataType_e;

/// Define RGB delta mode
typedef enum
{
	E_MHAL_PNL_RGB_Delta_RGB_MODE,
	E_MHAL_PNL_RGB_Delta_RBG_MODE,
	E_MHAL_PNL_RGB_Delta_GRB_MODE,
	E_MHAL_PNL_RGB_Delta_GBR_MODE,
	E_MHAL_PNL_RGB_Delta_BRG_MODE,
	E_MHAL_PNL_RGB_Delta_BGR_MODE,
} MhalPnlRgbDeltaMode_e;

typedef enum
{
    E_MHAL_PNL_CH_SWAP_0,
    E_MHAL_PNL_CH_SWAP_1,
    E_MHAL_PNL_CH_SWAP_2,
    E_MHAL_PNL_CH_SWAP_3,
    E_MHAL_PNL_CH_SWAP_4,
} MhalPnlChannelSwapType_e;

typedef enum
{
    E_MHAL_PNL_REG_SWAP_NONE = 0,
    E_MHAL_PNL_RGB_SWAP_R    = 1,
    E_MHAL_PNL_RGB_SWAP_G    = 2,
    E_MHAL_PNL_RGB_SWAP_B    = 3,
} MhalPnlRgbSwapType_e;

typedef enum
{
    E_MHAL_PNL_MIPI_DSI_CMD_MODE   = 0,
    E_MHAL_PNL_MIPI_DSI_SYNC_PULSE = 1,
    E_MHAL_PNL_MIPI_DSI_SYNC_EVENT = 2,
    E_MHAL_PNL_MIPI_DSI_BURST_MODE = 3,
} MhalPnlMipiDsiCtrlMode_e;

typedef enum
{
    E_MHAL_PNL_MIPI_DSI_RGB565         = 0,
    E_MHAL_PNL_MIPI_DSI_RGB666         = 1,
    E_MHAL_PNL_MIPI_DSI_LOOSELY_RGB666 = 2,
    E_MHAL_PNL_MIPI_DSI_RGB888         = 3,
} MhalPnlMipiDsiFormat_e;

typedef enum
{
    E_MHAL_PNL_MIPI_DSI_LANE_NONE   = 0,
    E_MHAL_PNL_MIPI_DSI_LANE_1      = 1,
    E_MHAL_PNL_MIPI_DSI_LANE_2      = 2,
    E_MHAL_PNL_MIPI_DSI_LANE_3      = 3,
    E_MHAL_PNL_MIPI_DSI_LANE_4      = 4,
} MhalPnlMipiDsiLaneMode_e;

typedef enum
{
    E_MHAL_PNL_MIPI_DSI_PACKET_TYPE_DCS     = 0,
    E_MHAL_PNL_MIPI_DSI_PACKET_TYPE_GENERIC = 1,
} MhalPnlMipiDsiPacketType_e;


/* MCU I/F system */
typedef enum
{
    E_MHAL_LCD_MCU_68SYS,
    E_MHAL_LCD_MCU_80SYS
} MhalPnlMcuType;

typedef enum
{
    //16bit bus
    E_MHAL_MCU_RGB565_BUS16_CFG,                  //0x0000
    E_MHAL_MCU_RGB444_BUS16_CFG,                  //0x0001
    E_MHAL_MCU_RGB666_BUS16_CFG,                  //0x0002
    //8 bit bus
    E_MHAL_MCU_RGB888_BUS8_CFG,                   //0x0003
    E_MHAL_MCU_RGB332_BUS8_CFG,                   //0x0004
    E_MHAL_MCU_RGB444_BUS8_CFG,                   //0x0005
    E_MHAL_MCU_RGB666_BUS8_CFG,                   //0x0006
    E_MHAL_MCU_RGB565_BUS8_CFG,                   //0x0007
    //18 bit bus
    E_MHAL_MCU_RGB666_BUS18_CFG,                  //0x0008
    E_MHAL_MCU_RGB666_BUS18_9_9_CFG,              //0x0009
    E_MHAL_MCU_RGB666_BUS18_2_16_CFG,             //0x000A
    E_MHAL_MCU_RGB666_BUS18_16_2_CFG,             //0x000B
    E_MHAL_MCU_RGB24_BUS18_16_8_CFG,              //0x000C
    E_MHAL_MCU_RGB24_BUS18_8_16_CFG,              //0x000D
    //8 bit
    E_MHAL_MCU_RGB18_BUS8_2_8_8_CFG,              //0x000E
    //TBD
    E_MHAL_MCU_RGB666_BUS8_2_7_CFG,               //
    E_MHAL_MCU_RGB444_B12_EXT_B16_CFG,            //
    E_MHAL_MCU_RGB444_B15_4_CFG,
    E_MHAL_MCU_RGBB9_9_17_CFG,
    E_MHAL_MCU_CFG_NOT_SUPPORT
} MhalPnlMcuDataBusCfg;

typedef struct
{
    const char *pPanelName; ///<  PanelName

#if !defined (__aarch64__)
    u16 u32AlignmentDummy0;
#endif

    u8             u8Dither;         ///<  Diether On?off
    MhalPnlLinkType_e eLinkType;     ///<  Panel LinkType

    ///////////////////////////////////////////////
    // Board related setting
    ///////////////////////////////////////////////
    u8 u8DualPort;              ///<  DualPort on/off
    u8 u8SwapPort;              ///<  Swap Port on/off
    u8 u8SwapOdd_ML;            ///<  Swap Odd ML
    u8 u8SwapEven_ML;           ///<  Swap Even ML
    u8 u8SwapOdd_RB;            ///<  Swap Odd RB
    u8 u8SwapEven_RB;           ///<  Swap Even RB

    u8 u8SwapLVDS_POL;          ///<  Swap LVDS Channel Polarity
    u8 u8SwapLVDS_CH;           ///<  Swap LVDS channel
    u8 u8PDP10BIT;              ///<  PDP 10bits on/off
    u8 u8LVDS_TI_MODE;          ///<  Ti Mode On/Off

    ///////////////////////////////////////////////
    // For TTL Only
    ///////////////////////////////////////////////
    u8 u8DCLKDelay;                 ///<  DCLK Delay
    u8 u8InvDCLK;                   ///<  CLK Invert
    u8 u8InvDE;                     ///<  DE Invert
    u8 u8InvHSync;                  ///<  HSync Invert
    u8 u8InvVSync;                  ///<  VSync Invert

    ///////////////////////////////////////////////
    // Output driving current setting
    ///////////////////////////////////////////////
    // driving current setting (0x00=4mA, 0x01=6mA, 0x02=8mA, 0x03=12mA)
    u8 u8DCKLCurrent;              ///< PANEL_DCLK_CURRENT
    u8 u8DECurrent;                ///< PANEL_DE_CURRENT
    u8 u8ODDDataCurrent;           ///< PANEL_ODD_DATA_CURRENT
    u8 u8EvenDataCurrent;          ///< PANEL_EVEN_DATA_CURRENT

    ///////////////////////////////////////////////
    // panel on/off timing
    ///////////////////////////////////////////////
    u16 u16OnTiming1;                ///<  time between panel & data while turn on power
    u16 u16OnTiming2;                ///<  time between data & back light while turn on power
    u16 u16OffTiming1;               ///<  time between back light & data while turn off power
    u16 u16OffTiming2;               ///<  time between data & panel while turn off power

    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    u16 u16HSyncWidth;               ///<  Hsync Width
    u16 u16HSyncBackPorch;           ///<  Hsync back porch

    u16 u16VSyncWidth;               ///<  Vsync width
    u16 u16VSyncBackPorch;           ///<  Vsync back porch

    // DE related
    u16 u16HStart;                   ///<  HDe start
    u16 u16VStart;                   ///<  VDe start
    u16 u16Width;                    ///<  Panel Width
    u16 u16Height;                   ///<  Panel Height

    // DClk related
    u16 u16MaxHTotal;                ///<  Max H Total
    u16 u16HTotal;                   ///<  H Total
    u16 u16MinHTotal;                ///<  Min H Total

    u16 u16MaxVTotal;                ///<  Max V Total
    u16 u16VTotal;                   ///<  V Total
    u16 u16MinVTotal;                ///<  Min V Total

    u16 u16MaxDCLK;                  ///<  Max DCLK
    u16 u16DCLK;                     ///<  DCLK ( Htt * Vtt * Fps)
    u16 u16MinDCLK;                  ///<  Min DCLK

    ///<  SSC
    u16 u16SpreadSpectrumStep;       ///<  Step of SSC
    u16 u16SpreadSpectrumSpan;       ///<  Span of SSC

    ///< PWM
    u8 u8PwmPeriodL;                 ///<
    u8 u8PwmPeriodH;                 ///<  Period of Pwm, 2 ~ 262143 Hz
    u8 u8PwmDuty;                    ///<  Duty of Pwm 0 ~ 100

    u8 u8DeinterMode;                ///<  DeInter Mode
    MhalPnlAspectRatio_e ePanelAspectRatio; ///<  Aspec Ratio

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
    u16 u16LVDSTxSwapValue;         // LVDS Swap Value
    MhalPnlTiBitMode_e eTiBitMode;  // Ti Bit Mode
    MhalPnlOutputFormatBitMode_e eOutputFormatBitMode;

    u8 u8SwapOdd_RG;                ///<  Swap Channel R
    u8 u8SwapEven_RG;               ///<  Swap Channel G
    u8 u8SwapOdd_GB;                ///<  Swap Channel B
    u8 u8SwapEven_GB;               ///<  Swap Rgb MSB/LSB

    /**
    *  Others
    */
    u8 u8DoubleClk;                             ///<  Double CLK On/off
    u32 u32MaxSET;                              ///<  Max Lpll Set
    u32 u32MinSET;                              ///<  Min Lpll Set
    MhalPnlOutputTimingMode_e eOutTimingMode;   ///<  Define which panel output timing change mode is used to change VFreq for same panel
    u8 u8NoiseDith;                             ///<  Noise Dither On/Off
    MhalPnlChannelSwapType_e eCh0;              ///<  Channel swap for CH0
    MhalPnlChannelSwapType_e eCh1;              ///<  Channel swap for CH1
    MhalPnlChannelSwapType_e eCh2;              ///<  Channel swap for CH2
    MhalPnlChannelSwapType_e eCh3;              ///<  Channel swap for CH3
    MhalPnlChannelSwapType_e eCh4;              ///<  Channel swap for CH4

    MhalPnlMipiDsiPacketType_e enPacketType;    ///<  Packet Type
} MhalPnlParamConfig_t;

typedef struct
{
    u8 u8HsTrail;
    u8 u8HsPrpr;
    u8 u8HsZero;
    u8 u8ClkHsPrpr;
    u8 u8ClkHsExit;
    u8 u8ClkTrail;
    u8 u8ClkZero;
    u8 u8ClkHsPost;
    u8 u8DaHsExit;
    u8 u8ContDet;

    u8 u8Lpx;
    u8 u8TaGet;
    u8 u8TaSure;
    u8 u8TaGo;

    u16 u16Hactive;
    u16 u16Hpw;
    u16 u16Hbp;
    u16 u16Hfp;

    u16 u16Vactive;
    u16 u16Vpw;
    u16 u16Vbp;
    u16 u16Vfp;

    u16 u16Bllp;
    u16 u16Fps;

    MhalPnlMipiDsiLaneMode_e enLaneNum;
    MhalPnlMipiDsiFormat_e enFormat;
    MhalPnlMipiDsiCtrlMode_e enCtrl;

    u8  *pu8CmdBuf;
    u32 u32CmdBufSize;

    u8  u8SyncCalibrate;
    u16 u16VirHsyncSt;
    u16 u16VirHsyncEnd;
    u16 u16VsyncRef;
    u16 u16DataClkSkew;

    u8  u8PolCh0;  // channel 0 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh1;  // channel 1 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh2;  // channel 2 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh3;  // channel 3 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh4;  // channel 4 polarity, 0:HW default, 1:positive, 2:negative

    MhalPnlMipiDsiPacketType_e enPacketType;
}MhalPnlMipiDsiConfig_t;

typedef struct
{
    bool bEn;
    u16  u16Step;
    u16  u16Span;
    u16  u16Modulation;// Khz
    u16  u16Deviation; // 1/1000
}MhalPnlSscConfig_t;

typedef struct
{
    u16 u16HSyncWidth;
    u16 u16HSyncBackPorch;
    u16 u16HSyncFrontPorch;

    u16 u16VSyncWidth;
    u16 u16VSyncBackPorch;
    u16 u16VSyncFrontPorch;

    u16 u16HStart;
    u16 u16VStart;

    u16 u16HActive;
    u16 u16VActive;

    u16 u16HTotal;
    u16 u16VTotal;

    u16 u16Dclk;
}MhalPnlTimingConfig_t;

typedef struct
{
    bool bEn;
}MhalPnlPowerConfig_t;

typedef struct
{
    bool bEn;
}MhalPnlBackLightOnOffConfig_t;

typedef struct
{
    u16  u16Duty;
    u16  u16Period;
}MhalPnlBackLightLevelConfig_t;

typedef struct
{
    u16 u16Val;
}MhalPnlDrvCurrentConfig_t;


typedef struct
{
    bool bEn;
    u16  u16R;
    u16  u16G;
    u16  u16B;
}MhalPnlTestPatternConfig_t;


//new add structs
typedef struct
{
    ///////////////////////////////////////////////
    // panel timing spec.
    ///////////////////////////////////////////////
    // sync related
    u16 u16HSyncWidth;               ///<  Hsync Width
    u16 u16HSyncBackPorch;           ///<  Hsync back porch

    u16 u16VSyncWidth;               ///<  Vsync width
    u16 u16VSyncBackPorch;           ///<  Vsync back porch

    // DE related
    u16 u16HStart;                   ///<  HDe start
    u16 u16VStart;                   ///<  VDe start
    u16 u16HActive;                  ///<  H Active = Panel Width
    u16 u16VActive;                  ///<  V Active = Panel Height

    // DClk related
    u16 u16HTotal;                   ///<  H Total
    u16 u16VTotal;                   ///<  V Total
    u32 u32Dclk;                     ///<  DCLK(Htt * Vtt * Fps)

}MhalPnlUnifiedTgnTimingConfig_t;

typedef struct
{
    ///////////////////////////////////////////////
    // ttl polarity
    ///////////////////////////////////////////////
    u8 u8InvDCLK;                   ///<  CLK Invert
    u8 u8InvDE;                     ///<  DE Invert
    u8 u8InvHSync;                  ///<  HSync Invert
    u8 u8InvVSync;                  ///<  VSync Invert
}MhalPnlUnifiedTgnPolarityConfig_t;

typedef struct
{
    ///////////////////////////////////////////////
    // ttl rgb output swap spec.
    ///////////////////////////////////////////////
    u8 u8SwapChnR;                ///<  Swap Channel R
    u8 u8SwapChnG;                ///<  Swap Channel G
    u8 u8SwapChnB;                ///<  Swap Channel B
    u8 u8SwapRgbML;               ///<  Swap Rgb MSB/LSB
}MhalPnlUnifiedTgnRgbOutputSwapConfig_t;

typedef struct
{
    ///////////////////////////////////////////////
    // ttl spread spectrum spec.
    ///////////////////////////////////////////////
    u16 u16SpreadSpectrumStep;    ///<  Swap Channel R
    u16 u16SpreadSpectrumSpan;    ///<  Swap Channel G
}MhalPnlUnifiedTgnSpreadSpectrumConfig_t;


typedef struct
{
    u32      *pu32CmdBuf;
    u32     u32CmdBufSize;
}MhalPnlUnifiedMcuAutoCmdConfig_t;

typedef struct
{
    u32     *pu32CmdBuf;
    u32     u32CmdBufSize;
}MhalPnlUnifiedMcuInitCmdConfig_t;

typedef struct
{
    u32     u32HActive;
    u32     u32VActive;
    u8      u8WRCycleCnt;
    u8      u8CSLeadWRCycleCnt;
    u8      u8RSLeadCSCycleCnt;
    MhalPnlMcuType       enMcuType;              ///<  McuType
    MhalPnlMcuDataBusCfg enMcuDataBusCfg;        ///<  McuDataBusCfg
    MhalPnlUnifiedMcuInitCmdConfig_t stMcuInitCmd;
    MhalPnlUnifiedMcuAutoCmdConfig_t stMcuAutoCmd;
}MhalPnlUnifiedMcuConfig_t;

typedef struct
{
    u8                   u8RgbDswap;
    MhalPnlRgbDataType_e eRgbDtype;
}MhalPnlUnifiedRgbDataConfig_t;

typedef struct
{
    MhalPnlRgbDeltaMode_e eOddLine;
    MhalPnlRgbDeltaMode_e eEvenLine;
}MhalPnlUnifiedRgbDeltaConfig_t;

typedef struct
{
    u8 u8HsTrail;
    u8 u8HsPrpr;
    u8 u8HsZero;
    u8 u8ClkHsPrpr;
    u8 u8ClkHsExit;
    u8 u8ClkTrail;
    u8 u8ClkZero;
    u8 u8ClkHsPost;
    u8 u8DaHsExit;
    u8 u8ContDet;

    u8 u8Lpx;
    u8 u8TaGet;
    u8 u8TaSure;
    u8 u8TaGo;

    u16 u16Hactive;
    u16 u16Hpw;
    u16 u16Hbp;
    u16 u16Hfp;

    u16 u16Vactive;
    u16 u16Vpw;
    u16 u16Vbp;
    u16 u16Vfp;

    u16 u16Bllp;
    u16 u16Fps;

    MhalPnlMipiDsiLaneMode_e enLaneNum;
    MhalPnlMipiDsiFormat_e enFormat;
    MhalPnlMipiDsiCtrlMode_e enCtrl;

    u8  *pu8CmdBuf;
    u32 u32CmdBufSize;

    u8  u8SyncCalibrate;
    u16 u16VirHsyncSt;
    u16 u16VirHsyncEnd;
    u16 u16VsyncRef;
    u16 u16DataClkSkew;

    u8  u8PolCh0;  // channel 0 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh1;  // channel 1 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh2;  // channel 2 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh3;  // channel 3 polarity, 0:HW default, 1:positive, 2:negative
    u8  u8PolCh4;  // channel 4 polarity, 0:HW default, 1:positive, 2:negative

    u8  u8SwapCh0;  // swap channel 0
    u8  u8SwapCh1;  // swap channel 1
    u8  u8SwapCh2;  // swap channel 2
    u8  u8SwapCh3;  // swap channel 3
    u8  u8SwapCh4;  // swap channel 4

    MhalPnlMipiDsiPacketType_e enPacketType;
}MhalPnlUnifiedMipiDsiConfig_t;


typedef struct
{
    const char *pPanelName;          ///<  PanelName
    MhalPnlLinkType_e eLinkType;     ///<  Panel LinkType

    ///////////////////////////////////////////////
    // Flags
    ///////////////////////////////////////////////
    //Tgen related
    u8 u8TgnTimingFlag;              ///<  1: stTgnTimingInfo is available;0 not available
    u8 u8TgnPolarityFlag;            ///<  1: stTgnPolarityInfo is available;0 not available
    u8 u8TgnRgbSwapFlag;             ///<  1: stTgnRgbSwapInfo is available;0 not available
    u8 u8TgnOutputBitMdFlag;         ///<  1: eOutputFormatBitMode is available;0 not available
    u8 u8TgnFixedDclkFlag;           ///<  1: u16FDclk is available;0 not available
    u8 u8TgnSscFlag;                 ///<  1: stTgnSscInfo is available;0 not available
    u8 u8TgnPadMuxFlag;              ///<  1: u16PadMux is available;0 not available

    //Mcu related
    u8 u8McuPhaseFlag;               ///<  1 u8McuPhase is available;0 not available
    u8 u8McuPolarityFlag;            ///<  1 u8McuPolarity is available;0 not available
    u8 u8McuRsPolarityFlag;          ///<  1 u8RsPolarity is available;0 not available
    u8 u8McuConfigFlag;              ///<  1 stMcuInfo is available;0 not available

    u8 u8RgbDataFlag;                ///<  1: stRgbDataInfo is available;0 not available
    u8 u8RgbDeltaMdFlag;             ///<  1: stRgbDeltaInfo is available;0 not available
    //mipi dsi related
    u8 u8MpdFlag;                    ///<  1: stMpdInfo is available;0 not available

    ///////////////////////////////////////////////
    // Parameters
    ///////////////////////////////////////////////
    //Tgen related
    MhalPnlUnifiedTgnTimingConfig_t stTgnTimingInfo;
    MhalPnlUnifiedTgnPolarityConfig_t stTgnPolarityInfo;
    MhalPnlUnifiedTgnRgbOutputSwapConfig_t stTgnRgbSwapInfo;
    MhalPnlOutputFormatBitMode_e eOutputFormatBitMode;
    u32 u32FDclk;                    ///<  Fixed DClk
    MhalPnlUnifiedTgnSpreadSpectrumConfig_t stTgnSscInfo;
    u16 u16PadMux;                   ///<  Pad Mux


    //Mcu related
    u8 u8McuPhase;                                  ///<  McuPhase
    u8 u8McuPolarity;                               ///<  McuPolarity
    u8 u8McuRsPolarity;                             ///<  McuRsPolarity
    MhalPnlUnifiedMcuConfig_t                       stMcuInfo;

    MhalPnlUnifiedRgbDataConfig_t  stRgbDataInfo;
    MhalPnlUnifiedRgbDeltaConfig_t stRgbDeltaInfo;

    //mipi dsi related
    MhalPnlUnifiedMipiDsiConfig_t stMpdInfo;

} MhalPnlUnifiedParamConfig_t;


#endif//
/** @} */ // end of HAL_PNL_group

