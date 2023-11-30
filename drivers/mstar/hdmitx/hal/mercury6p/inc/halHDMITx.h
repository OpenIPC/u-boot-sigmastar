/*
* halHDMITx.h- Sigmastar
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

#ifndef _HALHDMITX_H_
#define _HALHDMITX_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define HAL_HDMITX_SET_ASPECTRATIOFROMTIMING(Timing)                                               \
    ((Timing == E_HAL_HDMITX_RES_720X480P_60HZ ||\
      Timing == E_HAL_HDMITX_RES_720X576P_50HZ || \
      Timing == E_HAL_HDMITX_RES_640X480P_60HZ || \
      Timing == E_HAL_HDMITX_RES_1600X1200P_60HZ ||\
      Timing == E_HAL_HDMITX_RES_1280X1024P_60HZ) \
         ? E_HDMITX_VIDEO_AR_4_3                                                                   \
         : E_HDMITX_VIDEO_AR_16_9)
#define HAL_HDMITX_SET_COLORMETRYFROMTIMING(Timing)                                                \
    ((Timing == E_HAL_HDMITX_RES_720X480P_60HZ ||\
    Timing == E_HAL_HDMITX_RES_720X576P_50HZ || \
    Timing == E_HAL_HDMITX_RES_640X480P_60HZ || \
    Timing == E_HAL_HDMITX_RES_1600X1200P_60HZ || \
    Timing == E_HAL_HDMITX_RES_1280X1024P_60HZ) \
         ? E_HDMITX_COLORIMETRY_SMPTE170M                                                          \
         : E_HDMITX_COLORIMETRY_ITUR709)

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HDMITX_CSC_SEL_BYPASS      = 0,
    E_HDMITX_CSC_SEL_422_10_TO_8 = 0x1,
    E_HDMITX_CSC_SEL_Y_TO_R      = 0x2,
    E_HDMITX_CSC_SEL_R_TO_Y      = 0x4,
    E_HDMITX_CSC_SEL_444_TO_422  = 0x8,
    E_HDMITX_CSC_SEL_MAX,
} HalHdmitxCscType_e;
typedef enum
{
    E_HDMITX_DVICLOCK_L_HPD_L = 0,
    E_HDMITX_DVICLOCK_L_HPD_H = 1,
    E_HDMITX_DVICLOCK_H_HPD_L = 2,
    E_HDMITX_DVICLOCK_H_HPD_H = 3,
    E_HDMITX_DVICLOCK_MAX,
} HalHdmitxRxStatus_e;

typedef enum
{
    E_HDMITX_SEND_PACKET   = 0x00, // send packet
    E_HDMITX_CYCLIC_PACKET = 0x04, // cyclic packet by frame count
    E_HDMITX_STOP_PACKET   = 0x80, // stop packet
    E_HDMITX_PACKET_MAX,           //
} HalHdmitxPacketProcess_e;
typedef enum
{
    E_HDMITX_GCP_NO_CMD       = 0,
    E_HDMITX_GCP_SET_AVMUTE   = 1,
    E_HDMITX_GCP_CLEAR_AVMUTE = 2,
    E_HDMITX_GCP_NA           = 3,
    E_HDMITX_GCP_MAX,
} HalHdmitxPacketGcpStatus_e;

//*********************//
//             Video   //
//*********************//

typedef enum // wilson@kano
{
    E_HDMITX_COLORIMETRY_NO_DATA   = 0,
    E_HDMITX_COLORIMETRY_SMPTE170M = 1,
    E_HDMITX_COLORIMETRY_ITUR709   = 2,
    E_HDMITX_COLORIMETRY_EXTEND    = 3,
    E_HDMITX_COLORIMETRY_MAX,
} HalHdmitxColorimetry_e;

typedef enum // wilson@kano
{
    E_HDMITX_EXT_COLORIMETRY_XVYCC601    = 0,
    E_HDMITX_EXT_COLORIMETRY_XVYCC709    = 1,
    E_HDMITX_EXT_COLORIMETRY_SYCC601     = 2,
    E_HDMITX_EXT_COLORIMETRY_ADOBEYCC601 = 3,
    E_HDMITX_EXT_COLORIMETRY_ADOBERGB    = 4,
    E_HDMITX_EXT_COLORIMETRY_BT2020CYCC  = 5, // mapping to ext. colorimetry format BT2020Y'cC'bcC'rc
    E_HDMITX_EXT_COLORIMETRY_BT2020YCC   = 6, // mapping to ext. colorimetry format BT2020 RGB or YCbCr
    E_HDMITX_EXT_COLORIMETRY_BT2020RGB   = 7, // mapping to ext. colorimetry format BT2020 RGB or YCbCr
    E_HDMITX_EXT_COLORIMETRY_MAX,
} HalHdmitxExtColorimetry_e;

typedef enum // wilson@kano
{
    E_HDMITX_YCC_QUANT_LIMIT    = 0x00,
    E_HDMITX_YCC_QUANT_FULL     = 0x01,
    E_HDMITX_YCC_QUANT_RESERVED = 0x10,
    E_HDMITX_YCC_QUANT_MAX,
} HalHdmitxYccQuantRange_e;

typedef enum
{
    E_HDMITX_RGB_QUANT_DEFAULT  = 0x00,
    E_HDMITX_RGB_QUANT_LIMIT    = 0x01,
    E_HDMITX_RGB_QUANT_FULL     = 0x02,
    E_HDMITX_RGB_QUANT_RESERVED = 0x03,
    E_HDMITX_RGB_QUANT_MAX,
} HalHdmitxRgbQuantRange_e;

typedef enum
{
    E_HDMITX_VIC_NOT_AVAILABLE                 = 0,
    E_HDMITX_VIC_640X480P_60_4_3               = 1,
    E_HDMITX_VIC_720X480P_60_4_3               = 2,
    E_HDMITX_VIC_720X480P_60_16_9              = 3,
    E_HDMITX_VIC_1280X720P_60_16_9             = 4,
    E_HDMITX_VIC_1920X1080I_60_16_9            = 5,
    E_HDMITX_VIC_720X480I_60_4_3               = 6,
    E_HDMITX_VIC_720X480I_60_16_9              = 7,
    E_HDMITX_VIC_720X240P_60_4_3               = 8,
    E_HDMITX_VIC_720X240P_60_16_9              = 9,
    E_HDMITX_VIC_2880X480I_60_4_3              = 10,
    E_HDMITX_VIC_2880X480I_60_16_9             = 11,
    E_HDMITX_VIC_2880X240P_60_4_3              = 12,
    E_HDMITX_VIC_2880X240P_60_16_9             = 13,
    E_HDMITX_VIC_1440X480P_60_4_3              = 14,
    E_HDMITX_VIC_1440X480P_60_16_9             = 15,
    E_HDMITX_VIC_1920X1080P_60_16_9            = 16,
    E_HDMITX_VIC_720X576P_50_4_3               = 17,
    E_HDMITX_VIC_720X576P_50_16_9              = 18,
    E_HDMITX_VIC_1280X720P_50_16_9             = 19,
    E_HDMITX_VIC_1920X1080I_50_16_9            = 20,
    E_HDMITX_VIC_720X576I_50_4_3               = 21,
    E_HDMITX_VIC_720X576I_50_16_9              = 22,
    E_HDMITX_VIC_720X288P_50_4_3               = 23,
    E_HDMITX_VIC_720X288P_50_16_9              = 24,
    E_HDMITX_VIC_2880X576I_50_4_3              = 25,
    E_HDMITX_VIC_2880X576I_50_16_9             = 26,
    E_HDMITX_VIC_2880X288P_50_4_3              = 27,
    E_HDMITX_VIC_2880X288P_50_16_9             = 28,
    E_HDMITX_VIC_1440X576P_50_4_3              = 29,
    E_HDMITX_VIC_1440X576P_50_16_9             = 30,
    E_HDMITX_VIC_1920X1080P_50_16_9            = 31,
    E_HDMITX_VIC_1920X1080P_24_16_9            = 32,
    E_HDMITX_VIC_1920X1080P_25_16_9            = 33,
    E_HDMITX_VIC_1920X1080P_30_16_9            = 34,
    E_HDMITX_VIC_2880X480P_60_4_3              = 35,
    E_HDMITX_VIC_2880X480P_60_16_9             = 36,
    E_HDMITX_VIC_2880X576P_50_4_3              = 37,
    E_HDMITX_VIC_2880X576P_50_16_9             = 38,
    E_HDMITX_VIC_1920X1080I_50_16_9_1250_TOTAL = 39,
    E_HDMITX_VIC_1920X1080I_100_16_9           = 40,
    E_HDMITX_VIC_1280X720P_100_16_9            = 41,
    E_HDMITX_VIC_720X576P_100_4_3              = 42,
    E_HDMITX_VIC_720X576P_100_16_9             = 43,
    E_HDMITX_VIC_720X576I_100_4_3              = 44,
    E_HDMITX_VIC_720X576I_100_16_9             = 45,
    E_HDMITX_VIC_1920X1080I_120_16_9           = 46,
    E_HDMITX_VIC_1280X720P_120_16_9            = 47,
    E_HDMITX_VIC_720X480P_120_4_3              = 48,
    E_HDMITX_VIC_720X480P_120_16_9             = 49,
    E_HDMITX_VIC_720X480I_120_4_3              = 50,
    E_HDMITX_VIC_720X480I_120_16_9             = 51,
    E_HDMITX_VIC_720X576P_200_4_3              = 52,
    E_HDMITX_VIC_720X576P_200_16_9             = 53,
    E_HDMITX_VIC_720X576I_200_4_3              = 54,
    E_HDMITX_VIC_720X576I_200_16_9             = 55,
    E_HDMITX_VIC_720X480P_240_4_3              = 56,
    E_HDMITX_VIC_720X480P_240_16_9             = 57,
    E_HDMITX_VIC_720X480I_240_4_3              = 58,
    E_HDMITX_VIC_720X480I_240_16_9             = 59,
    E_HDMITX_VIC_1280X720P_24_16_9             = 60,
    E_HDMITX_VIC_1280X720P_25_16_9             = 61,
    E_HDMITX_VIC_1280X720P_30_16_9             = 62,
    E_HDMITX_VIC_1920X1080P_120_16_9           = 63,
    E_HDMITX_VIC_1920X1080P_100_16_9           = 64,
    // VVV------------------------------------------- HDMI 2.0 :: 21:9 ASPECT RATIO
    E_HDMITX_VIC_1280X720P_24_21_9   = 65,
    E_HDMITX_VIC_1280X720P_25_21_9   = 66,
    E_HDMITX_VIC_1280X720P_30_21_9   = 67,
    E_HDMITX_VIC_1280X720P_50_21_9   = 68,
    E_HDMITX_VIC_1280X720P_60_21_9   = 69,
    E_HDMITX_VIC_1280X720P_100_21_9  = 70,
    E_HDMITX_VIC_1280X720P_120_21_9  = 71,
    E_HDMITX_VIC_1920X1080P_24_21_9  = 72,
    E_HDMITX_VIC_1920X1080P_25_21_9  = 73,
    E_HDMITX_VIC_1920X1080P_30_21_9  = 74,
    E_HDMITX_VIC_1920X1080P_50_21_9  = 75,
    E_HDMITX_VIC_1920X1080P_60_21_9  = 76,
    E_HDMITX_VIC_1920X1080P_100_21_9 = 77,
    E_HDMITX_VIC_1920X1080P_120_21_9 = 78,
    E_HDMITX_VIC_1680X720P_24_21_9   = 79,
    E_HDMITX_VIC_1680X720P_25_21_9   = 80,
    E_HDMITX_VIC_1680X720P_30_21_9   = 81,
    E_HDMITX_VIC_1680X720P_50_21_9   = 82,
    E_HDMITX_VIC_1680X720P_60_21_9   = 83,
    E_HDMITX_VIC_1680X720P_100_21_9  = 84,
    E_HDMITX_VIC_1680X720P_120_21_9  = 85,
    E_HDMITX_VIC_2560X1080P_24_21_9  = 86,
    E_HDMITX_VIC_2560X1080P_25_21_9  = 87,
    E_HDMITX_VIC_2560X1080P_30_21_9  = 88,
    E_HDMITX_VIC_2560X1080P_50_21_9  = 89,
    E_HDMITX_VIC_2560X1080P_60_21_9  = 90,
    E_HDMITX_VIC_2560X1080P_100_21_9 = 91,
    E_HDMITX_VIC_2560X1080P_120_21_9 = 92,
    //^^^------------------------------------------- HDMI 2.0 :: 21:9 ASPECT RATIO
    E_HDMITX_VIC_3840X2160P_24_16_9    = 93,
    E_HDMITX_VIC_3840X2160P_25_16_9    = 94,
    E_HDMITX_VIC_3840X2160P_30_16_9    = 95,
    E_HDMITX_VIC_3840X2160P_50_16_9    = 96,
    E_HDMITX_VIC_3840X2160P_60_16_9    = 97,
    E_HDMITX_VIC_4096X2160P_24_256_135 = 98,
    E_HDMITX_VIC_4096X2160P_25_256_135 = 99,
    E_HDMITX_VIC_4096X2160P_30_256_135 = 100,
    E_HDMITX_VIC_4096X2160P_50_256_135 = 101,
    E_HDMITX_VIC_4096X2160P_60_256_135 = 102,
    // VVV------------------------------------------- HDMI 2.0 :: 21:9 ASPECT RATIO
    E_HDMITX_VIC_3840X2160P_24_64_27 = 103,
    E_HDMITX_VIC_3840X2160P_25_64_27 = 104,
    E_HDMITX_VIC_3840X2160P_30_64_27 = 105,
    E_HDMITX_VIC_3840X2160P_50_64_27 = 106,
    E_HDMITX_VIC_3840X2160P_60_64_27 = 107,
    //^^^------------------------------------------- HDMI 2.0 :: 21:9 ASPECT RATIO
    E_HDMITX_VIC_MAX,
} HalHdmitxAviVic_e;

typedef enum
{
    E_HDMITX_VIDEO_AR_RESERVED = 0,
    E_HDMITX_VIDEO_AR_4_3      = 1,
    E_HDMITX_VIDEO_AR_16_9     = 2,
    E_HDMITX_VIDEO_AR_21_9     = 3, // new feature in HDMI 2.0
    E_HDMITX_VIDEO_AR_MAX,
} HalHdmitxVideoAspectRatio_e;

typedef enum
{
    E_HDMITX_VIDEO_SI_NODATA       = 0,
    E_HDMITX_VIDEO_SI_OVERSCANNED  = 1,
    E_HDMITX_VIDEO_SI_UNDERSCANNED = 2,
    E_HDMITX_VIDEO_SI_RESERVED     = 3,
    E_HDMITX_VIDEO_SI_MAX,
} HalHdmitxVideoScanInfo_e;

typedef enum
{
    E_HDMITX_VIDEO_AFD_SAMEASPICTUREAR = 8,  // 1000
    E_HDMITX_VIDEO_AFD_4_3_CENTER      = 9,  // 1001
    E_HDMITX_VIDEO_AFD_16_9_CENTER     = 10, // 1010
    E_HDMITX_VIDEO_AFD_14_9_CENTER     = 11, // 1011
    E_HDMITX_VIDEO_AFD_OTHERS          = 15, // 0000~ 0111, 1100 ~ 1111
    E_HDMITX_VIDEO_AFD_MAX,
} HalHdmitxVideoAfdRatio_e;

typedef enum
{
    E_HDMITX_VIDEO_VS_NO_ADDITION = 0, // 000
    E_HDMITX_VIDEO_VS_4K_2K       = 1, // 001
    E_HDMITX_VIDEO_VS_3D          = 2, // 010
    E_HDMITX_VIDEO_VS_RESERVED    = 7, // 011~ 111
    E_HDMITX_VIDEO_VS_MAX,
} HalHdmitxVideoVsFormat_e;

typedef enum
{
    E_HDMITX_VIDEO_3D_FRAMEPACKING      = 0,  // 0000
    E_HDMITX_VIDEO_3D_FIELDALTERNATIVE  = 1,  // 0001
    E_HDMITX_VIDEO_3D_LINEALTERNATIVE   = 2,  // 0010
    E_HDMITX_VIDEO_3D_SIDEBYSIDE_FULL   = 3,  // 0011
    E_HDMITX_VIDEO_3D_L_DEP             = 4,  // 0100
    E_HDMITX_VIDEO_3D_L_DEP_GRAPHIC_DEP = 5,  // 0101
    E_HDMITX_VIDEO_3D_TOPANDBOTTOM      = 6,  // 0110
    E_HDMITX_VIDEO_3D_SIDEBYSIDE_HALF   = 8,  // 1000
    E_HDMITX_VIDEO_3D_NOT_IN_USE        = 15, // 1111
    E_HDMITX_VIDEO_3D_MAX,                    // 1111
} HalHdmitxVideo3DStructure_e;

typedef enum
{
    E_HDMITX_EDID_3D_FRAMEPACKING               = 1,     // 3D_STRUCTURE_ALL_0
    E_HDMITX_EDID_3D_FIELDALTERNATIVE           = 2,     // 3D_STRUCTURE_ALL_1
    E_HDMITX_EDID_3D_LINEALTERNATIVE            = 4,     // 3D_STRUCTURE_ALL_2
    E_HDMITX_EDID_3D_SIDEBYSIDE_FULL            = 8,     // 3D_STRUCTURE_ALL_3
    E_HDMITX_EDID_3D_L_DEP                      = 16,    // 3D_STRUCTURE_ALL_4
    E_HDMITX_EDID_3D_L_DEP_GRAPHIC_DEP          = 32,    // 3D_STRUCTURE_ALL_5
    E_HDMITX_EDID_3D_TOPANDBOTTOM               = 64,    // 3D_STRUCTURE_ALL_6
    E_HDMITX_EDID_3D_SIDEBYSIDE_HALF_HORIZONTAL = 256,   // 3D_STRUCTURE_ALL_8
    E_HDMITX_EDID_3D_SIDEBYSIDE_HALF_QUINCUNX   = 32768, // 3D_STRUCTURE_ALL_15
    E_HDMITX_EDID_3D_MAX,                                // 3D_STRUCTURE_ALL_15
} HalHdmitxEDID3DStructuteAll_e;

typedef enum
{
    E_HDMITX_VIDEO_4K2K_RESERVED   = 0, // 0x00
    E_HDMITX_VIDEO_4K2K_30HZ       = 1, // 0x01
    E_HDMITX_VIDEO_4K2K_25HZ       = 2, // 0x02
    E_HDMITX_VIDEO_4K2K_24HZ       = 3, // 0x03
    E_HDMITX_VIDEO_4K2K_24HZ_SMPTE = 4, // 0x04
    E_HDMITX_VIDEO_4K2K_MAX,            // 0x04
} HalHdmitxVideo4k2kVic_e;

typedef enum // check EDID support color range
{
    E_HDMITX_EDID_QUANT_LIMIT = 0x00,
    E_HDMITX_EDID_QUANT_FULL  = 0x01,
    E_HDMITX_EDID_QUANT_BOTH  = 0x10,
    E_HDMITX_EDID_QUANT_MAX,
} HalHdmitxEdidQuantRange_e;

#ifndef HDMITX_OS_TYPE_UBOOT
typedef struct __attribute__((packed)) HalHdmitxAnalogTuning_s
#else
typedef struct HalHdmitxAnalogTuning_s
#endif
{
    // HDMI Tx Current, Pre-emphasis and Double termination
    u8  tm_txcurrent; // TX current control(U4: 0x11302B[13:12], K1: 0x11302B[13:11])
    u8  tm_pren2;     // pre-emphasis mode control, 0x11302D[5]
    u8  tm_precon;    // TM_PRECON, 0x11302E[7:4]
    u8  tm_pren;      // pre-emphasis enable, 0x11302E[11:8]
    u8  tm_tenpre;    // Double termination pre-emphasis enable, 0x11302F[3:0]
    u8  tm_ten;       // Double termination enable, 0x11302F[7:4]
    u32 u32AlignDummy;

    // HDMI Tx Current
    u8 u8DrvCurTap1Ch0;
    u8 u8DrvCurTap1Ch1;
    u8 u8DrvCurTap1Ch2;
    u8 u8DrvCurTap1Ch3;
    u8 u8DrvCurTap2Ch0;
    u8 u8DrvCurTap2Ch1;
    u8 u8DrvCurTap2Ch2;
    u8 u8DrvCurTap2Ch3;
    u8 u8DrvCurPdRtermCh;
    u8 u8DrvCurPdLdoPreDrvCh;
    u8 u8DrvCurPdLdoMuxCh;
} HalHdmitxAnalogTuning_t;

typedef struct HalHdmitxEdid3DSupportTiming_s
{
    HalHdmitxAviVic_e             support_timing;
    HalHdmitxEDID3DStructuteAll_e support_3D_structure;
} HalHdmitxEdid3DSupportTiming_t;

typedef struct HalHdmitxGCPktPara_s // wilson@kano
{
    HalHdmitxColorDepthType_e  enColorDepInfo;
    HalHdmitxPacketGcpStatus_e enAVMute;
} HalHdmitxGCPktPara_t;

typedef struct HalHdmitxVSInfoPktPara_s // wilson@kano
{
    HalHdmitxVideoVsFormat_e    enVSFmt;
    HalHdmitxVideo3DStructure_e en3DStruct;
    HalHdmitxVideo4k2kVic_e     en4k2kVIC;
} HalHdmitxVSInfoPktPara_t;

typedef struct HalHdmitxAVIInfoPktPara_s // wilson@kano
{
    u8                          enableAFDoverWrite;
    u8                          A0Value;
    HalHdmitxColorType_e        enColorFmt;
    HalHdmitxColorimetry_e      enColorimetry;
    HalHdmitxExtColorimetry_e   enExtColorimetry;
    HalHdmitxYccQuantRange_e    enYCCQuantRange;
    HalHdmitxRgbQuantRange_e    enRGBQuantRange;
    HalHdmitxTimingResType_e    enVidTiming;
    HalHdmitxVideoAfdRatio_e    enAFDRatio;
    HalHdmitxVideoScanInfo_e    enScanInfo;
    HalHdmitxVideoAspectRatio_e enAspectRatio;
} HalHdmitxAVIInfoPktPara_t;

typedef struct HalHdmitxAUDInfoPktPara_s // wilson@kano
{
    HalHdmitxAudioChannelType_e enAudChCnt;
    HalHdmitxAudioCodingType_e  enAudType;
    HalHdmitxAudioFreqType_e    enAudFreq;
    // u8                         bChStatus;
} HalHdmitxAUDInfoPktPara_t;

typedef struct HalHdmitxPktAttribute_s // wilson@kano
{
    u8                       EnableUserDef;
    u8                       FrmCntNum;
    HalHdmitxPacketProcess_e enPktCtrl;
    union
    {
        HalHdmitxGCPktPara_t      GCPktPara;
        HalHdmitxVSInfoPktPara_t  VSInfoPktPara;
        HalHdmitxAVIInfoPktPara_t AVIInfoPktPara;
        HalHdmitxAUDInfoPktPara_t AUDInfoPktPara;
    } PktPara;
} HalHdmitxPktAttribute_t; // wilson@kano

typedef enum
{
    E_HDMITX_TMDS_OFF     = 0,
    E_HDMITX_TMDS_FULL_ON = 1,
    E_HDMITX_TMDS_MAX
} HalHdmitxTmdsStatus_e;

// *************  For customer NDS **************//
typedef enum
{
    E_HDMITX_AVI_PIXEL_FROMAT = 0, // Y[1:0]
    E_HDMITX_AVI_ASPECT_RATIO = 1, // AR, AFD, TBEL, BBSL, LBEP, RBSP, BIValid, AFDValid, ScanInfo, ScalingInfo
    E_HDMITX_AVI_COLORIMETRY  = 2, // Colormetry and extended colorimetry
    E_HDMITX_AVI_MAX               // Colormetry and extended colorimetry
} HalHdmitxAviContentType_e;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------
#endif // _HAL_HDMITX_H_
