/*
* mhal_hdmitx_datatype.h- Sigmastar
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

#ifndef __MHAL_HDMITX_DATATYPE_H__
#define __MHAL_HDMITX_DATATYPE_H__

//-------------------------------------------------------------------------------------------------
//  enum
//-------------------------------------------------------------------------------------------------

///
/// @brief retval for HDMITX moudle
///
typedef enum
{
    E_MHAL_HDMITX_RET_SUCCESS    = 0, ///< Function is success
    E_MHAL_HDMITX_RET_CFGERR     = 1, ///< Configuration error
    E_MHAL_HDMITX_RET_NOTSUPPORT = 2, ///< Function is not support
    E_MHAL_HDMITX_RET_NONEED     = 3, ///< Function is unnecessary
    E_MHAL_HDMITX_RET_HPDERR     = 4, ///< Hot pulg error
    E_MHAL_HDMITX_RET_ERR        = 5, ///< Function is fail
} MhalHdmitxRet_e;

///
/// @brief HDMITx output color space
///
typedef enum
{
    E_MHAL_HDMITX_COLOR_RGB444 = 0x01, ///< Output color: RGB444
    E_MHAL_HDMITX_COLOR_YUV444 = 0x02, ///< Output color: YUV444
    E_MHAL_HDMITX_COLOR_YUV422 = 0x04, ///< Output color: YUV422
    E_MHAL_HDMITX_COLOR_YUV420 = 0x08, ///< Output color: YUV420
    E_MHAL_HDMITX_COLOR_AUTO   = 0x10, ///< Output color is determined by EDID
} MhalHdmitxColorType_e;

///
/// @brief HDMITx output mode
///
typedef enum
{
    E_MHAL_HDMITX_OUTPUT_MODE_DVI       = 0, ///< Output mode is DVI
    E_MHAL_HDMITX_OUTPUT_MODE_DVI_HDCP  = 1, ///< Output mode is DVI with HDCP
    E_MHAL_HDMITX_OUTPUT_MODE_HDMI      = 2, ///< Output mode is HDMI
    E_MHAL_HDMITX_OUTPUT_MODE_HDMI_HDCP = 3, ///< Output mode is HDMI with HDCP
} MhalHdmitxOutpuModeType_e;

///
/// @brief HDMITx output color Depth
///
typedef enum
{
    E_MHAL_HDMITX_CD_NO_ID   = 0, ///< NO Indefinty
    E_MHAL_HDMITX_CD_24_BITS = 1, ///< Output color depth is 24 bits
    E_MHAL_HDMITX_CD_30_BITS = 2, ///< Output color depth is 30 bits
    E_MHAL_HDMITX_CD_36_BITS = 3, ///< Output color depth is 36 bits
    E_MHAL_HDMITX_CD_48_BITS = 4, ///< Output color depth is 48 bits
} MhalHdmitxColorDepthType_e;

///
/// @brief The identity of HDMITx output resultuion
///
typedef enum
{
    E_MHAL_HDMITX_RES_1280X720P_24HZ    , ///< 1280x720@24Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_25HZ    , ///< 1280x720@25Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_2997HZ  , ///< 1280x720@29.97Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_30HZ    , ///< 1280x720@30Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_50HZ    ,  ///< 1280x720@50Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_5994HZ  , ///< 1280x720@59.94Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X720P_60HZ    ,  ///< 1280x720@60Hz, progressive mode

    E_MHAL_HDMITX_RES_1920X1080P_2398HZ , ///< 1920x1080@23.98Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_24HZ   ,  ///< 1920x1080@24Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_25HZ   ,  ///< 1920x1080@25Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_2997HZ , ///< 1920x1080@29.97Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_30HZ   ,  ///< 1920x1080@30Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_50HZ   ,  ///< 1920x1080@50Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_5994HZ , ///< 1920x1080@59.94Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1080P_60HZ   ,  ///< 1920x1080@60Hz, progressive mode

    E_MHAL_HDMITX_RES_2560X1440P_30HZ   , ///< 2560x1440@30Hz, progressive mode
    E_MHAL_HDMITX_RES_2560X1440P_50HZ   , ///< 2560x1440@50Hz, progressive mode
    E_MHAL_HDMITX_RES_2560X1440P_60HZ   , ///< 2560x1440@60Hz, progressive mode

    E_MHAL_HDMITX_RES_3840X2160P_24HZ   , ///< 3840x21600@24Hz, progressive mode
    E_MHAL_HDMITX_RES_3840X2160P_25HZ   , ///< 3840x21600@25Hz, progressive mode
    E_MHAL_HDMITX_RES_3840X2160P_2997HZ , ///< 3840x21600@29.97Hz, progressive mode
    E_MHAL_HDMITX_RES_3840X2160P_30HZ   , ///< 3840x21600@30Hz, progressive mode
    E_MHAL_HDMITX_RES_3840X2160P_50HZ   , ///< 3840x21600@50Hz, progressive mode
    E_MHAL_HDMITX_RES_3840X2160P_60HZ   , ///< 3840x21600@60Hz, progressive mode

    E_MHAL_HDMITX_RES_640X480P_60HZ     ,  ///< 720x408@60Hz, progressive mode
    E_MHAL_HDMITX_RES_720X480P_60HZ     ,  ///< 720x408@60Hz, progressive mode
    E_MHAL_HDMITX_RES_720X576P_50HZ     ,  ///< 720x576@50Hz, progressive mode
    E_MHAL_HDMITX_RES_800X600P_60HZ     , ///< 800x600@60Hz, progressive mode
    E_MHAL_HDMITX_RES_848X480P_60HZ     , ///< 848x480@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1024X768P_60HZ    , ///< 1024x768@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X768P_60HZ    , ///< 1280x768@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X800P_60HZ    , ///< 1280x800@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X960P_60HZ    , ///< 1280x960@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1280X1024P_60HZ   , ///< 1280x1024@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1360X768P_60HZ    , ///< 1360x768@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1366X768P_60HZ    , ///< 1366x768@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1400X1050P_60HZ   , ///< 1400x1050@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1440X900P_60HZ    , ///< 1440x900@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1600X900P_60HZ    , ///< 1600x900@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1600X1200P_60HZ   , ///< 1600x1200@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1680X1050P_60HZ   , ///< 1600x1050@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1200P_60HZ   , ///< 1920x1200@60Hz, progressive mode
    E_MHAL_HDMITX_RES_1920X1440P_60HZ   , ///< 1980x1440@60Hz, progressive mode
    E_MHAL_HDMITX_RES_2560X1600P_60HZ   , ///< 2560x1600@60Hz, progressive mode
    E_MHAL_HDMITX_RES_MAX               ,
}MhaHdmitxTimingResType_e;

///
/// @brief Hdmitx Audio output frequency
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_FREQ_NO_SIG = 0, ///< No signal
    E_MHAL_HDMITX_AUDIO_FREQ_32K    = 1, ///< Output audio frequency is 32K
    E_MHAL_HDMITX_AUDIO_FREQ_44K    = 2, ///< Output audio frequency is 44K
    E_MHAL_HDMITX_AUDIO_FREQ_48K    = 3, ///< Output audio frequency is 48K
    E_MHAL_HDMITX_AUDIO_FREQ_88K    = 4, ///< Output audio frequency is 88K
    E_MHAL_HDMITX_AUDIO_FREQ_96K    = 5, ///< Output audio frequency is 96K
    E_MHAL_HDMITX_AUDIO_FREQ_176K   = 6, ///< Output audio frequency is 176K
    E_MHAL_HDMITX_AUDIO_FREQ_192K   = 7, ///< Output audio frequency is 192K
    E_MHAL_HDMITX_AUDIO_FREQ_NUM    = 8, ///< Total number of audio frequency
} MhalHdmitxAudioFreqType_e;

///
/// @brief Hdmitx Audio output channel
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_CH_2, ///< Ouptut audio channel is 2
    E_MHAL_HDMITX_AUDIO_CH_8, ///< Ouptut audio channel is 8
} MhalHdmitxAudioChannelType_e;

///
/// @brief Hdmitx Audio output configuration
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_CODING_PCM,    ///< Output audio configuration is PCM
    E_MHAL_HDMITX_AUDIO_CODING_NONPCM, ///< Output audio configuration is NON-PCM
} MhalHdmitxAudioCodingType_e;

///
/// @brief Hdmitx Audio output format
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_FORMAT_PCM, ///< Ouptut audio format is PCM
    E_MHAL_HDMITX_AUDIO_FORMAT_DSD, ///< Ouptut audio format is DSD
    E_MHAL_HDMITX_AUDIO_FORMAT_HBR, ///< Ouptut audio format is HBR
    E_MHAL_HDMITX_AUDIO_FORMAT_NA,  ///< Ouptut audio format is no defined
} MhalHdmitxAudioSourceFormat_e;

///
/// @brief Hdmitx mute type
///
typedef enum
{
    E_MHAL_HDMITX_MUTE_NONE   = 0x00, ///< No mute
    E_MHAL_HDMITX_MUTE_VIDEO  = 0x01, ///< Video mute only
    E_MHAL_HDMITX_MUTE_AUDIO  = 0x02, ///< Audio mute only
    E_MHAL_HDMITX_MUTE_AVMUTE = 0x04, ///< Both vidoe and audio mute
} MhalHdmitxMuteType_e;

///
/// @brief Query information of sink
///
typedef enum
{
    E_MHAL_HDMITX_SINK_INFO_EDID_DATA = 0, ///< Query sink EDID
    E_MHAL_HDMITX_SINK_INFO_HDMI_SUPPORT,  ///< Query HDMI mode supported of sink
    E_MHAL_HDMITX_SINK_INFO_COLOR_FORMAT,  ///< Query supported color space of sink
    E_MHAL_HDMITX_SINK_INFO_HPD_STATUS,    ///< Query Hog plug status
    E_MHAL_HDMITX_SINK_INFO_NUM,           ///< Number of sink infomation type
} MhalHdmitxSinkInfoType_e;

///
/// @brief Hdmitx info frame type
///
typedef enum
{
    E_MHAL_HDMITX_INFOFRAM_TYPE_AVI   = 0, ///< AVI Info frame
    E_MHAL_HDMITX_INFOFRAM_TYPE_SPD   = 1, ///< SPD Info frame
    E_MHAL_HDMITX_INFOFRAM_TYPE_AUDIO = 2, ///< AUDIO Info frame
    E_MHAL_HDMITX_INFOFRAM_TYPE_MAX   = 3, ///< Max number of Info frame
} MhalHdmitxInfoFrameType_e;

///
/// @brief Colorimetry data for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_COLORIMETRY_NO_DATA = 0, ///< Colorimetry is no data
    E_MHAL_HDMITX_COLORIMETRY_SMPTE170M,   ///< Colorimetry is SMPTE170M
    E_MHAL_HDMITX_COLORIMETRY_ITUR709,     ///< Colorimetry is ITUR709
    E_MHAL_HDMITX_COLORIMETRY_EXTEND,      ///< Extened Colorimetry is valid
    E_MHAL_HDMITX_COLORIMETRY_MAX,
} MhalHdmitxColorimetry_e;

///
/// @brief Extened Colorimetry data for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_EXT_COLORIMETRY_XVYCC601 = 0, ///< Colorimetry is XVYCC601
    E_MHAL_HDMITX_EXT_COLORIMETRY_XVYCC709,     ///< Colorimetry is XVYCC709
    E_MHAL_HDMITX_EXT_COLORIMETRY_SYCC601,      ///< Colorimetry is SYCC601
    E_MHAL_HDMITX_EXT_COLORIMETRY_ADOBEYCC601,  ///< Colorimetry is ADOBE YCC601
    E_MHAL_HDMITX_EXT_COLORIMETRY_ADOBERGB,     ///< Colorimetry is ADOBE RGB
    E_MHAL_HDMITX_EXT_COLORIMETRY_BT2020CYCC,   ///< Colorimetry is BT2020 CYCC
    E_MHAL_HDMITX_EXT_COLORIMETRY_BT2020YCC,    ///< Colorimetry is BT2020 YCC
    E_MHAL_HDMITX_EXT_COLORIMETRY_BT2020RGB,    ///< Colorimetry is BT2020 RGB
    E_MHAL_HDMITX_EXT_COLORIMETRY_MAX,          ///< Max number of extended colorimetry
} MhalHdmitxExtColorimetry_e;

///
/// @brief Aspec Ratio data for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_ASPECT_RATIO_INVALID = 0, ///< Unknown aspect ratio
    E_MHAL_HDMITX_ASPECT_RATIO_4TO3,        ///< Aspect ratio 4:3
    E_MHAL_HDMITX_ASPECT_RATIO_16TO9,       ///< Aspect ratio 16:9
    E_MHAL_HDMITX_ASPECT_RATIO_21TO9,       ///< Aspect ratio 21:9
    E_MHAL_HDMITX_ASPECT_RATIO_MAX          ///< Max number of aspect ratio
} MhalHdmitxAspectRatio_e;

///
/// @brief YCC quantization data for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_YCC_QUANTIZATION_LIMITED_RANGE = 0, ///< YCC quantization is limited range
    E_MHAL_HDMITX_YCC_QUANTIZATION_FULL_RANGE,        ///< YCC quantization is full range
    E_MHAL_HDMITX_YCC_QUANTIZATION_MAX                ///< Max number of ycc quantization
} MhalHdmitxYccQuantRange_e;

///
/// @brief AFD for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_VIDEO_AFD_SameAsPictureAR = 8,  ///< Same as coded frame aspect ratio
    E_MHAL_HDMITX_VIDEO_AFD_4_3_Center      = 9,  ///< 4:3 (cenger)
    E_MHAL_HDMITX_VIDEO_AFD_16_9_Center     = 10, ///< 16:9 (cenger)
    E_MHAL_HDMITX_VIDEO_AFD_14_9_Center     = 11, ///< 14:9 (cenger)
    E_MHAL_HDMITX_VIDEO_AFD_Others          = 15, ///< Other definition.
} MhalHdmitxVideoAfdRatio_e;

///
/// @brief Scan information for AVI Info frame
///
typedef enum
{
    E_MHAL_HDMITX_SCAN_INFO_NO_DATA = 0,  ///< No Scan information*
    E_MHAL_HDMITX_SCAN_INFO_OVERSCANNED,  ///< Scan information, Overscanned
    E_MHAL_HDMITX_SCAN_INFO_UNDERSCANNED, ///< Scan information, Underscanned
    E_MHAL_HDMITX_SCAN_INFO_FUTURE,       ///< Scan information, future definition
    E_MHAL_HDMITX_SCAN_INFO_MAX           ///< Max number of scan information
} MhalHdmitxScanInfo_e;

///
/// @brief Hdmitx audio code type
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_CODE_PCM = 0, ///< Audio code type is PCM
    E_MHAL_HDMITX_AUDIO_CODE_NON_PCM, ///< Audio code type is NON-PCM
    E_MHAL_HDMITX_AUDIO_CODE_MAX      ///< Max number of audio code type
} MhalHdmitxAudioCodeType_e;

///
/// @brief Hdmitx audio sampling rate type
///
typedef enum
{
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_UNKNOWN = 0, ///< Audio sampling rate is unknonw
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_32K     = 1, ///< Audio sampling rate is 32K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_44K     = 2, ///< Audio sampling rate is 44K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_48K     = 3, ///< Audio sampling rate is 48K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_88K     = 4, ///< Audio sampling rate is 88K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_96K     = 5, ///< Audio sampling rate is 96K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_176K    = 6, ///< Audio sampling rate is 176K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_192K    = 7, ///< Audio sampling rate is 192K
    E_MHAL_HDMITX_AUDIO_SAMPLERATE_MAX,         ///< Max number of sampling rate
} MhalHdmitxSampleRate_e;

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
///
/// @brief The attribute of HDMITX
///
typedef struct
{
    u8                         bVideoEn;     ///< 1:eanble video, 0: disalbe video
    MhalHdmitxColorType_e      enInColor;    ///< Input color space
    MhalHdmitxColorType_e      enOutColor;   ///< Output color space
    MhalHdmitxColorDepthType_e enColorDepth; ///< Output color depth
    MhalHdmitxOutpuModeType_e  enOutputMode; ///< Output mode
    MhaHdmitxTimingResType_e   enTiming;     ///< Output resolution

    u8                            bAudioEn;    ///< 1: enable audio, 0: disable audio
    MhalHdmitxAudioFreqType_e     enAudioFreq; ///< Output audio frequency
    MhalHdmitxAudioChannelType_e  enAudioCh;   ///< Output audio channel
    MhalHdmitxAudioCodingType_e   enAudioCode; ///< Output audio code type
    MhalHdmitxAudioSourceFormat_e enAudioFmt;  ///< Output audio format
} MhalHdmitxAttrConfig_t;

///
/// @brief The configuration of mute
///
typedef struct
{
    u8                   bMute;  ///< 1: enable mute, 0: disable mute
    MhalHdmitxMuteType_e enType; ///< mute type
} MhalHdmitxMuteConfig_t;

///
/// @brief The signal configuration
///
typedef struct
{
    u8 bEn; ///< 1 : enable signal, 0 : disable signal
} MhalHdmitxSignalConfig_t;

///
/// @brief The configuration of sink EDID
///
typedef struct
{
    u8 u8BlockId;        ///< block ID of EDID
    u8 au8EdidData[128]; ///< EDIDdata
} MhalHdmitxSinkEdidDataConfig_t;

///
/// @brief The HDMI supported of sink
///
typedef struct
{
    u8 bSupported; ///< 1: HDMI supported, 0 : HDMI nont supported
} MhalHdmitxSinkSupportHdmiConfig_t;

///
/// @brief The supported color format of specific timing resolution from sink
///
typedef struct
{
    MhaHdmitxTimingResType_e enTiming; ///< Specific timing resolution
    MhalHdmitxColorType_e    enColor;  ///< Supported color format
} MhalHdmiSinkColorForamtConfig_t;

///
/// @brief The hot plug status of sink
///
typedef struct
{
    u8 bHpd; ///<
} MhalHdmitxSinkHpdStatusConfig_t;

///
/// @brief The union structure for sink information
///
typedef union
{
    MhalHdmitxSinkEdidDataConfig_t    stEdidData;      ///< EDID configuration
    MhalHdmitxSinkSupportHdmiConfig_t stSupportedHdmi; ///< Hdmi Supported
    MhalHdmiSinkColorForamtConfig_t   stColoFmt;       ///< Color format
    MhalHdmitxSinkHpdStatusConfig_t   stHpdStatus;     ///< Hot plug status
} MhalHdmitxSinkInfoUnit_t;

///
/// @brief The configuration of sink information
///
typedef struct
{
    MhalHdmitxSinkInfoType_e enType;     ///< Query type of sink information
    MhalHdmitxSinkInfoUnit_t stInfoUnit; ///< Data of sink information
} MhalHdmitxSinkInfoConfig_t;

///
/// @brief The configuration of analog driving current
///
typedef struct
{
    u8 u8DrvCurTap1Ch0;       ///< TAP1 Post Driver Current Control of HDMI Channel 0
    u8 u8DrvCurTap1Ch1;       ///< TAP1 Post Driver Current Control of HDMI Channel 1
    u8 u8DrvCurTap1Ch2;       ///< TAP1 Post Driver Current Control of HDMI Channel 2
    u8 u8DrvCurTap1Ch3;       ///< TAP1 Post Driver Current Control of HDMI Channel 3
    u8 u8DrvCurTap2Ch0;       ///< TAP2 Post Driver Current Control of HDMI Channel 0
    u8 u8DrvCurTap2Ch1;       ///< TAP2 Post Driver Current Control of HDMI Channel 1
    u8 u8DrvCurTap2Ch2;       ///< TAP2 Post Driver Current Control of HDMI Channel 2
    u8 u8DrvCurTap2Ch3;       ///< TAP2 Post Driver Current Control of HDMI Channel 3
    u8 u8DrvCurPdRtermCh;     ///< 50 ohm Rterm Disable signal
    u8 u8DrvCurPdLdoPreDrvCh; ///< Pre-driver LDO siable control signal of HDMI channels
    u8 u8DrvCurPdLdoMuxCh;    ///< Seiralizer LDO siable control signal of HDMI channels
} MhalHdmitxAnaloDrvCurConfig_t;

///
/// @brief The configuration of AVI info frame
///
typedef struct
{
    MhalHdmitxScanInfo_e  enScanInfo;  ///< Scan information
    u8                    A0Value;     ///< Active format information
    MhalHdmitxColorType_e enColorType; ///< Output color information

    u8                        bEnableAfdOverWrite; ///< AFD information
    MhalHdmitxVideoAfdRatio_e enAfdRatio;          ///< Aspec ratio informaiton
    MhalHdmitxAspectRatio_e   enAspectRatio;

    MhalHdmitxColorimetry_e    enColorimetry;    ///< Colorimetry information
    MhalHdmitxExtColorimetry_e enExtColorimetry; ///< Extended colorimetry information
    MhaHdmitxTimingResType_e   enTimingType;     ///< Active timing resolution
    MhalHdmitxYccQuantRange_e  enYccQuantRange;  ///< Ycc quantization range information
} MhalHdmitxAviInfoFrameConfig_t;

///
/// @brief The configuration of Audio info frame
///
typedef struct
{
    u32                       u32ChannelCount; ///< Channel count
    MhalHdmitxAudioCodeType_e enAudioCodeType; ///< Code type, PCM or NON-PCM
    MhalHdmitxSampleRate_e    enSampleRate;    ///< Audio sampling rate
} MhalHdmitxAudInfoFrameConfig_t;

///
/// @brief The configuration of SPI info frame
///
typedef struct
{
    u8 au8VendorName[8];          ///< Vendor Name
    u8 au8ProductDescription[16]; ///< Description of product
} MhalHdmitxSpdInfoFrameConfig_t;

///
/// @brief The union structure of info frame
///
typedef union
{
    MhalHdmitxAviInfoFrameConfig_t stAviInfoFrame; ///< AVI info frame
    MhalHdmitxAudInfoFrameConfig_t stAudInfoFrame; ///< Audio Info frame
    MhalHdmitxSpdInfoFrameConfig_t stSpdInfoFrame; ///< SPI info frame
} MhalHdmitxInfoFrameUint_u;

///
/// @brief The configuration of info frame
///
typedef struct
{
    u8                        bEn;
    MhalHdmitxInfoFrameType_e enType;      ///< Info frame type
    MhalHdmitxInfoFrameUint_u stInfoFrame; ///< Data of info frame
} MhalHdmitxInfoFrameConfig_t;

///
/// @brief The configuration for GPIO number of hot plug
///
typedef struct
{
    u8 u8GpioNum; ///< GPIO number
} MhalHdmitxHpdConfig_t;
#endif
