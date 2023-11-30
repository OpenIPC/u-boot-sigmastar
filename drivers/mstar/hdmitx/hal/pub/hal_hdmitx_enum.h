/*
* hal_hdmitx_enum.h- Sigmastar
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

#ifndef _HAL_HDMITX_ENUM_H_
#define _HAL_HDMITX_ENUM_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HDMITX_EDID_BLK_SIZE        128U
#define HDMITX_CEA_DAT_BLK_TYPE_NUM 8U
#define HDMITX_ISR_ENABLE           0U
#define HDMITX_DBG_PQ               0x01U ///< Debug PQ Table
#define HDMITX_DBG_UTILTX           0x04U ///< Debug S RULE
#define HDMITX_DBG_EDID             0x08U ///< Debug
#define HDMITX_DBG_HAL              0x10U
#define HDMITX_DBG_OS               0x20U
#define HDMITX_GET_CSC_SEL_VAL(incolor, outcolor)                                                                  \
    ((incolor == outcolor)                                                             ? (E_HDMITX_CSC_SEL_BYPASS) \
     : (incolor == E_HAL_HDMITX_COLOR_RGB444 && outcolor == E_HAL_HDMITX_COLOR_YUV444) ? E_HDMITX_CSC_SEL_R_TO_Y   \
     : (incolor == E_HAL_HDMITX_COLOR_YUV444 && outcolor == E_HAL_HDMITX_COLOR_RGB444) ? E_HDMITX_CSC_SEL_Y_TO_R   \
                                                                                       : E_HDMITX_CSC_SEL_MAX)
#define HDMITX_PKT_AVI_LEN           (13)
#define HDMITX_PKT_ADO_LEN           (6)
#define HDMITX_PKT_SPD_LEN           (25)
#define HDMITX_PKT_VS_LEN            (27)
#define HDMITX_PKT_VS_HD_VERSION_VAL (0x1)

#define HDMITX_FSM_IS_PENDING(stm)       (stm == E_HAL_HDMITX_FSM_PENDING)
#define HDMITX_FSM_IS_VALIDATE_EDID(stm) (stm == E_HAL_HDMITX_FSM_VALIDATE_EDID)
#define HDMITX_FSM_IS_TRANSMIT(stm)      (stm == E_HAL_HDMITX_FSM_TRANSMIT)
#define HDMITX_FSM_IS_DONE(stm)          (stm == E_HAL_HDMITX_FSM_DONE)
#define HDMITX_FSM_SET(stm, val)         (stm = val)
#define HDMITX_CLK_DOUBLE                1

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_HDMITX_EVENT_RUN      = 0x00000001U,
    E_HAL_HDMITX_EVENT_IRQ      = 0x00000002U,
    E_HAL_HDMITX_EVENT_RXTIMER  = 0x00000004U,
    E_HAL_HDMITX_EVENT_CECRX    = 0x00000008U,
    E_HAL_HDMITX_EVENT_HPD_ISR  = 0x00000080U,
    E_HAL_HDMITX_EVENT_STOPTASK = 0x00008000U,
    E_HAL_HDMITX_EVENT_MAX,
} HalHdmitxEventType_e;

typedef enum
{
    E_HAL_HDMITX_CEATAG_RESERVED_0     = 0,
    E_HAL_HDMITX_CEATAG_AUDIOBLK       = 1,
    E_HAL_HDMITX_CEATAG_VIDEOBLK       = 2,
    E_HAL_HDMITX_CEATAG_VENDORSPECIFIC = 3,
    E_HAL_HDMITX_CEATAG_SPEAKERALLOC   = 4,
    E_HAL_HDMITX_CEATAG_VESA_DTCBLK    = 5,
    E_HAL_HDMITX_CEATAG_RESERVED_1     = 6,
    E_HAL_HDMITX_CEATAG_USEEXTTAG      = 7,
    E_HAL_HDMITX_CEATAG_MAX,
} HalHdmitxCEADatablkTagCode_e;

typedef enum
{
    E_HAL_HDMITX_CEAEXTTAG_VIDEOCAP_DB          = 0,
    E_HAL_HDMITX_CEAEXTTAG_VSVIDEO_DB           = 1,
    E_HAL_HDMITX_CEAEXTTAG_VESADISPALYDEV_DB    = 2,
    E_HAL_HDMITX_CEAEXTTAG_VESAVIDEOTIMINGBLK   = 3,
    E_HAL_HDMITX_CEAEXTTAG_RESVEREDHDMIVIDEODB  = 4,
    E_HAL_HDMITX_CEAEXTTAG_COLORIMETRYDB        = 5,
    E_HAL_HDMITX_CEAEXTTAG_HDRSTATICMETADB      = 6,
    E_HAL_HDMITX_CEAEXTTAG_YCBCR420VIDEODB      = 14,
    E_HAL_HDMITX_CEAEXTTAG_YCBCR420CAPMAPDB     = 15,
    E_HAL_HDMITX_CEAEXTTAG_RESERVEDCEAMISCAUDIO = 16,
    E_HAL_HDMITX_CEAEXTTAG_VSAUDIODB            = 17,
    E_HAL_HDMITX_CEAEXTTAG_RESERVEDHDMIAUDIODB  = 18,
    E_HAL_HDMITX_CEAEXTTAG_INFOFRAMEDB          = 32,
    E_HAL_HDMITX_CEAEXTTAG_MAX,
} HalHdmitxCEADatablkExtTagCode_e;

typedef enum
{
    E_HAL_HDMITX_QUERY_RET_OK = 0,
    E_HAL_HDMITX_QUERY_RET_CFGERR,
    E_HAL_HDMITX_QUERY_RET_NOTSUPPORT,
    E_HAL_HDMITX_QUERY_RET_NONEED,
    E_HAL_HDMITX_QUERY_RET_ERR,
    E_HAL_HDMITX_QUERY_RET_MAX,
} HalHdmitxQueryRet_e;

typedef enum
{
    E_HAL_HDMITX_QUERY_INIT,
    E_HAL_HDMITX_QUERY_DEINIT,
    E_HAL_HDMITX_QUERY_ATTR_BEGIN,
    E_HAL_HDMITX_QUERY_ATTR,
    E_HAL_HDMITX_QUERY_ATTR_END,
    E_HAL_HDMITX_QUERY_SIGNAL,
    E_HAL_HDMITX_QUERY_MUTE,
    E_HAL_HDMITX_QUERY_ANALOG_DRV_CUR,
    E_HAL_HDMITX_QUERY_SINK_INFO,
    E_HAL_HDMITX_QUERY_INFO_FRAME,
    E_HAL_HDMITX_QUERY_DEBUG_LEVEL,
    E_HAL_HDMITX_QUERY_HPD,
    E_HAL_HDMITX_QUERY_CLK_SET,
    E_HAL_HDMITX_QUERY_CLK_GET,
    E_HAL_HDMITX_QUERY_FORCE_CFG_SET,
    E_HAL_HDMITX_QUERY_FORCE_CFG_GET,
    E_HAL_HDMITX_QUERY_MAX,
} HalHdmitxQueryType_e;

typedef enum
{
    E_HAL_HDMITX_COLOR_RGB444,
    E_HAL_HDMITX_COLOR_YUV422,
    E_HAL_HDMITX_COLOR_YUV444,
    E_HAL_HDMITX_COLOR_YUV420,
    E_HAL_HDMITX_COLOR_AUTO,
    E_HAL_HDMITX_COLOR_MAX,
} HalHdmitxColorType_e;

typedef enum
{
    E_HAL_HDMITX_OUTPUT_MODE_DVI       = 0,
    E_HAL_HDMITX_OUTPUT_MODE_DVI_HDCP  = 1,
    E_HAL_HDMITX_OUTPUT_MODE_HDMI      = 2,
    E_HAL_HDMITX_OUTPUT_MODE_HDMI_HDCP = 3,
    E_HAL_HDMITX_OUTPUT_MODE_MAX,
} HalHdmitxOutpuModeType_e;

typedef enum
{
    E_HAL_HDMITX_CD_NO_ID   = 0,
    E_HAL_HDMITX_CD_24_BITS = 1,
    E_HAL_HDMITX_CD_30_BITS = 2,
    E_HAL_HDMITX_CD_36_BITS = 3,
    E_HAL_HDMITX_CD_48_BITS = 4,
    E_HAL_HDMITX_CD_MAX,
} HalHdmitxColorDepthType_e;

typedef enum
{
    E_HAL_HDMITX_RES_1280X720P_24HZ    , ///< 1280x720@24Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_25HZ    , ///< 1280x720@25Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_29D97HZ  , ///< 1280x720@29.97Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_30HZ    , ///< 1280x720@30Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_50HZ    ,  ///< 1280x720@50Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_59D94HZ  , ///< 1280x720@59.94Hz, progressive mode
    E_HAL_HDMITX_RES_1280X720P_60HZ    ,  ///< 1280x720@60Hz, progressive mode

    E_HAL_HDMITX_RES_1920X1080P_23D98HZ , ///< 1920x1080@23.98Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_24HZ   ,  ///< 1920x1080@24Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_25HZ   ,  ///< 1920x1080@25Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_29D97HZ , ///< 1920x1080@29.97Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_30HZ   ,  ///< 1920x1080@30Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_50HZ   ,  ///< 1920x1080@50Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_59D94HZ , ///< 1920x1080@59.94Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1080P_60HZ   ,  ///< 1920x1080@60Hz, progressive mode

    E_HAL_HDMITX_RES_2560X1440P_30HZ   , ///< 2560x1440@30Hz, progressive mode
    E_HAL_HDMITX_RES_2560X1440P_50HZ   , ///< 2560x1440@50Hz, progressive mode
    E_HAL_HDMITX_RES_2560X1440P_60HZ   , ///< 2560x1440@60Hz, progressive mode

    E_HAL_HDMITX_RES_3840X2160P_24HZ   , ///< 3840x21600@24Hz, progressive mode
    E_HAL_HDMITX_RES_3840X2160P_25HZ   , ///< 3840x21600@25Hz, progressive mode
    E_HAL_HDMITX_RES_3840X2160P_29D97HZ , ///< 3840x21600@29.97Hz, progressive mode
    E_HAL_HDMITX_RES_3840X2160P_30HZ   , ///< 3840x21600@30Hz, progressive mode
    E_HAL_HDMITX_RES_3840X2160P_50HZ   , ///< 3840x21600@50Hz, progressive mode
    E_HAL_HDMITX_RES_3840X2160P_60HZ   , ///< 3840x21600@60Hz, progressive mode

    E_HAL_HDMITX_RES_640X480P_60HZ     ,  ///< 720x408@60Hz, progressive mode
    E_HAL_HDMITX_RES_720X480P_60HZ     ,  ///< 720x408@60Hz, progressive mode
    E_HAL_HDMITX_RES_720X576P_50HZ     ,  ///< 720x576@50Hz, progressive mode
    E_HAL_HDMITX_RES_800X600P_60HZ     , ///< 800x600@60Hz, progressive mode
    E_HAL_HDMITX_RES_848X480P_60HZ     , ///< 848x480@60Hz, progressive mode
    E_HAL_HDMITX_RES_1024X768P_60HZ    , ///< 1024x768@60Hz, progressive mode
    E_HAL_HDMITX_RES_1280X768P_60HZ    , ///< 1280x768@60Hz, progressive mode
    E_HAL_HDMITX_RES_1280X800P_60HZ    , ///< 1280x800@60Hz, progressive mode
    E_HAL_HDMITX_RES_1280X960P_60HZ    , ///< 1280x960@60Hz, progressive mode
    E_HAL_HDMITX_RES_1280X1024P_60HZ   , ///< 1280x1024@60Hz, progressive mode
    E_HAL_HDMITX_RES_1360X768P_60HZ    , ///< 1360x768@60Hz, progressive mode
    E_HAL_HDMITX_RES_1366X768P_60HZ    , ///< 1366x768@60Hz, progressive mode
    E_HAL_HDMITX_RES_1400X1050P_60HZ   , ///< 1400x1050@60Hz, progressive mode
    E_HAL_HDMITX_RES_1440X900P_60HZ    , ///< 1440x900@60Hz, progressive mode
    E_HAL_HDMITX_RES_1600X900P_60HZ    , ///< 1600x900@60Hz, progressive mode
    E_HAL_HDMITX_RES_1600X1200P_60HZ   , ///< 1600x1200@60Hz, progressive mode
    E_HAL_HDMITX_RES_1680X1050P_60HZ   , ///< 1600x1050@60Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1200P_60HZ   , ///< 1920x1200@60Hz, progressive mode
    E_HAL_HDMITX_RES_1920X1440P_60HZ   , ///< 1980x1440@60Hz, progressive mode
    E_HAL_HDMITX_RES_2560X1600P_60HZ   , ///< 2560x1600@60Hz, progressive mode
    E_HAL_HDMITX_RES_MAX               ,
} HalHdmitxTimingResType_e;

typedef enum
{
    E_HAL_HDMITX_AUDIO_FREQ_NO_SIG = 0,
    E_HAL_HDMITX_AUDIO_FREQ_32K    = 1,
    E_HAL_HDMITX_AUDIO_FREQ_44K    = 2,
    E_HAL_HDMITX_AUDIO_FREQ_48K    = 3,
    E_HAL_HDMITX_AUDIO_FREQ_88K    = 4,
    E_HAL_HDMITX_AUDIO_FREQ_96K    = 5,
    E_HAL_HDMITX_AUDIO_FREQ_176K   = 6,
    E_HAL_HDMITX_AUDIO_FREQ_192K   = 7,
    E_HAL_HDMITX_AUDIO_FREQ_MAX    = 8,
} HalHdmitxAudioFreqType_e;

typedef enum
{
    E_HAL_HDMITX_AUDIO_CH_2,
    E_HAL_HDMITX_AUDIO_CH_8,
    E_HAL_HDMITX_AUDIO_CH_MAX,
} HalHdmitxAudioChannelType_e;

typedef enum
{
    E_HAL_HDMITX_AUDIO_CODING_PCM,
    E_HAL_HDMITX_AUDIO_CODING_NONPCM,
    E_HAL_HDMITX_AUDIO_CODING_MAX,
} HalHdmitxAudioCodingType_e;

typedef enum
{
    E_HAL_HDMITX_AUDIO_FORMAT_PCM,
    E_HAL_HDMITX_AUDIO_FORMAT_DSD,
    E_HAL_HDMITX_AUDIO_FORMAT_HBR,
    E_HAL_HDMITX_AUDIO_FORMAT_NA,
    E_HAL_HDMITX_AUDIO_FORMAT_MAX,
} HalHdmitxAudioSourceFormat_e;

typedef enum
{
    E_HAL_HDMITX_MUTE_NONE   = 0x00,
    E_HAL_HDMITX_MUTE_VIDEO  = 0x01,
    E_HAL_HDMITX_MUTE_AUDIO  = 0x02,
    E_HAL_HDMITX_MUTE_AVMUTE = 0x04,
    E_HAL_HDMITX_MUTE_MAX,
} HalHdmitxMuteType_e;

typedef enum
{
    E_HAL_HDMITX_SINK_INFO_EDID_DATA = 0,
    E_HAL_HDMITX_SINK_INFO_HDMI_SUPPORT,
    E_HAL_HDMITX_SINK_INFO_COLOR_FORMAT,
    E_HAL_HDMITX_SINK_INFO_HPD_STATUS,
    E_HAL_HDMITX_SINK_INFO_MAX,
} HalHdmitxSinkInfoType_e;

typedef enum
{
    E_HAL_HDMITX_INFOFRAM_TYPE_AVI,
    E_HAL_HDMITX_INFOFRAM_TYPE_SPD,
    E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO,
    E_HAL_HDMITX_INFOFRAM_TYPE_VS,
    E_HAL_HDMITX_INFOFRAM_TYPE_MAX,
} HalHdmitxInfoFrameType_e;
typedef enum
{
    E_HAL_HDMITX_PACKET_TYPE_NULL,
    E_HAL_HDMITX_PACKET_TYPE_ACR,
    E_HAL_HDMITX_PACKET_TYPE_AS,
    E_HAL_HDMITX_PACKET_TYPE_GC,
    E_HAL_HDMITX_PACKET_TYPE_ACP,
    E_HAL_HDMITX_PACKET_TYPE_ISRC1,
    E_HAL_HDMITX_PACKET_TYPE_ISRC2,
    E_HAL_HDMITX_PACKET_TYPE_DSD,
    E_HAL_HDMITX_PACKET_TYPE_HBR,
    E_HAL_HDMITX_PACKET_TYPE_GM,
    E_HAL_HDMITX_PACKET_TYPE_MAX,
} HalHdmitxPacketType_e;

typedef enum
{
    E_HAL_HDMITX_STATUS_FLAG_ATTR_BEGIN = 0x0001,
    E_HAL_HDMITX_STATUS_FLAG_ATTR       = 0x0002,
    E_HAL_HDMITX_STATUS_FLAG_ATTR_END   = 0x0004,
    E_HAL_HDMITX_STATUS_FLAG_SIGNAL     = 0x0008,
    E_HAL_HDMITX_STATUS_FLAG_AUDIO_MUTE = 0x0010,
    E_HAL_HDMITX_STATUS_FLAG_VIDEO_MUTE = 0x0020,
    E_HAL_HDMITX_STATUS_FLAG_AV_MUTE    = 0x0040,
    E_HAL_HDMITX_STATUS_FLAG_INFO_FRAME = 0x0080,
    E_HAL_HDMITX_STATUS_FLAG_MAX,
} HalHdmitxStatusFlag_e;
typedef enum
{
    E_HAL_HDMITX_FORCE_FLAG_NONE       = 0x0000,
    E_HAL_HDMITX_FORCE_FLAG_OUPUT_MODE = 0x0001,
    E_HAL_HDMITX_FORCE_FLAG_HPD_DETECT = 0x0002,
    E_HAL_HDMITX_FORCE_FLAG_MAX,
} HalHdmitxForceFlag_e;
typedef enum
{
    E_HAL_HDMITX_FSM_PENDING       = 0,
    E_HAL_HDMITX_FSM_VALIDATE_EDID = 1,
    E_HAL_HDMITX_FSM_TRANSMIT      = 2,
    E_HAL_HDMITX_FSM_DONE          = 3,
    E_HAL_HDMITX_FSM_MAX,
} HalHdmitxFsmType_e;
typedef enum
{
    E_HDMITX_VIDEO_INTERLACE_MODE   = 0,
    E_HDMITX_VIDEO_PROGRESSIVE_MODE = 1,
    E_HDMITX_VIDEO_MODE_MAX,
} HalHdmitxVideoMode_e;

typedef enum
{
    E_HDMITX_VIDEO_POLARITY_HIGH = 0,
    E_HDMITX_VIDEO_POLARITY_LOW  = 1,
    E_HDMITX_VIDEO_POLARITY_MAX,
} HalHdmitxVideoPolarity_e;

#endif
