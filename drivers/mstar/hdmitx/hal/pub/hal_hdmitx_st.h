/*
* hal_hdmitx_st.h- Sigmastar
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

#ifndef _HAL_HDMITX_ST_H_
#define _HAL_HDMITX_ST_H_

#include "hal_hdmitx_enum.h"
#include "hal_hdmitx_chip.h"
#include "halHDMITx.h"

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct HalHdmitxQueryInConfig_s
{
    HalHdmitxQueryType_e enQueryType;
    void *               pInCfg;
    u32                  u32CfgSize;
} HalHdmitxQueryInConfig_t;

typedef struct HalHdmitxQueryOutConfig_s
{
    HalHdmitxQueryRet_e enQueryRet;
    void (*pSetFunc)(void *, void *);
} HalHdmitxQueryOutConfig_t;

typedef struct HalHdmitxQueryConfig_s
{
    HalHdmitxQueryInConfig_t  stInCfg;
    HalHdmitxQueryOutConfig_t stOutCfg;
} HalHdmitxQueryConfig_t;

typedef struct HalHdmitxAttrConfig_s
{
    u8                        bVideoEn;
    HalHdmitxColorType_e      enInColor;
    HalHdmitxColorType_e      enOutColor;
    HalHdmitxColorDepthType_e enColorDepth;
    HalHdmitxOutpuModeType_e  enOutputMode;
    HalHdmitxTimingResType_e  enTiming;

    u8                           bAudioEn;
    HalHdmitxAudioFreqType_e     enAudioFreq;
    HalHdmitxAudioChannelType_e  enAudioCh;
    HalHdmitxAudioCodingType_e   enAudioCode;
    HalHdmitxAudioSourceFormat_e enAudioFmt;
} HalHdmitxAttrConfig_t;

typedef struct HalHdmitxMuteConfig_s
{
    u8                  bMute;
    HalHdmitxMuteType_e enType;
} HalHdmitxMuteConfig_t;

typedef struct HalHdmitxSignalConfig_s
{
    u8 bEn;
} HalHdmitxSignalConfig_t;

typedef struct HalHdmitxSinkEdidDataConfig_s
{
    u8 u8BlockId;
    u8 au8EdidData[128];
} HalHdmitxSinkEdidDataConfig_t;

typedef struct HalHdmitxSinkSupportHdmiConfig_s
{
    u8 bSupported;
} HalHdmitxSinkSupportHdmiConfig_t;

typedef struct HalHdmitxSinkColorFormatConfig_s
{
    HalHdmitxTimingResType_e enTiming;
    u32                      u32Color;
} HalHdmitxSinkColorFormatConfig_t;

typedef struct HalHdmitxSinkHpdStatusConfig_s
{
    u8 bHpd;
} HalHdmitxSinkHpdStatusConfig_t;

typedef union HalHdmitxSinkInfoUnit_s
{
    HalHdmitxSinkEdidDataConfig_t    stEdidData;
    HalHdmitxSinkSupportHdmiConfig_t stSupportedHdmi;
    HalHdmitxSinkColorFormatConfig_t stColoFmt;
    HalHdmitxSinkHpdStatusConfig_t   stHpdStatus;
} HalHdmitxSinkInfoUnit_t;

typedef struct HalHdmitxSinkInfoConfig_s
{
    HalHdmitxSinkInfoType_e enType;
    HalHdmitxSinkInfoUnit_t stInfoUnit;
} HalHdmitxSinkInfoConfig_t;

typedef struct HalHdmitxAnaloDrvCurConfig_s
{
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
} HalHdmitxAnaloDrvCurConfig_t;

typedef struct HalHdmitxInfoFrameConfig_s
{
    u8                       bEn;
    HalHdmitxInfoFrameType_e enType;
    u8                       au8Data[128];
    u8                       u8DataLen;
} HalHdmitxInfoFrameConfig_t;

typedef struct HalHdmitxHpdConfig_s
{
    u8 u8GpioNum;
} HalHdmitxHpdConfig_t;

typedef struct HalHdmitxClkConfig_s
{
    u8  bEn[HAL_HDMITX_CLK_NUM];
    u32 u32Rate[HAL_HDMITX_CLK_NUM];
    u32 u32Num;
} HalHdmitxClkConfig_t;

typedef struct HalHdmitxForceAttrConfig_s
{
    HalHdmitxForceFlag_e     eFlag;
    u8                       bOutputMode;
    HalHdmitxOutpuModeType_e enOutputMode;
    u8                       bHpdDetect;
} HalHdmitxForceAttrConfig_t;
#ifndef HDMITX_OS_TYPE_UBOOT
typedef struct __attribute__((packed)) HalHdmitxRxEDIDVideoInfo_s
#else
typedef struct HalHdmitxRxEDIDVideoInfo_s
#endif
{
    HalHdmitxAviVic_e             VideoTiming;
    HalHdmitxEDID3DStructuteAll_e Video3DInfo;
    u32                           u32AlignDummy;
} HalHdmitxRxEDIDVideoInfo_t; // wilson@kano

#ifndef HDMITX_OS_TYPE_UBOOT
typedef struct __attribute__((packed)) HalHdmitxRxEDIDInfo_s
#else
typedef struct HalHdmitxRxEDIDInfo_s
#endif
{
    u8 EdidBlk0[HDMITX_EDID_BLK_SIZE];
    u8 EdidBlk1[HDMITX_EDID_BLK_SIZE];
    u8 ManufacturerID[3];
    u8 CEADataBlkLen[HDMITX_CEA_DAT_BLK_TYPE_NUM];
    u8 PhyAddr[2];
    u8 SupportHdmi;

    // HF-VSDB, scdc relative
    u8 HF_VSDBVerInfo;
    u8 MaxTmdsCharRate;
    u8 SupportIndependView;
    u8 SupportDualView;
    u8 Support3DOsdDisparity;
    u8 SupportLTEScramble;
    u8 SupportSCDC;
    u8 SupportRR; // read request
    u8 YUV420DeepColorInfo;

    u8                        AudSupportAI;
    u8                        b3DPresent;
    u8                        b3dMultiPresent;
    u8                        Hdmi3DLen;
    u8                        Support2D_50Hz;
    u8                        Support2D_60Hz;
    u8                        Support3D_50Hz;
    u8                        Support3D_60Hz;
    HalHdmitxColorDepthType_e ColorDepthInfo;

    // data block content
    u8                         AudioDataBlk[32];
    u8                         VideoDataBlk[32];
    u8                         VendorDataBlk[32];
    u8                         SpeakerAllocDataBlk[32];
    u8                         VESA_DTCDataBlk[32];
    u8                         HdmiVICList[7]; // this field has only 3 bits
    u8                         HdmiVICLen;
    HalHdmitxRxEDIDVideoInfo_t SupVidTiming[32];

    u8 HDMI_VSDB[32];
    u8 HDMI_VSDB_Len;
    u8 HF_VSDB[32];
    u8 HF_VSDB_Len;

    // for 420
    u8 Support420ColorFmt;
    u8 YCbCr420VidDataBlk[32];
    u8 YCbCr420CapMapDataBlk[32];

    // for colorimetry
    u8 ExtColorimetry;

    // for HDR
    u8 bSupportHDR;
    u8 HDRStaticDataBlk[32];

    // for VCDB
    u8 VideoCapDataBlk;

    u8 VSVideoBlk[32];

    u32 u32AlignDummy;

} HalHdmitxRxEDIDInfo_t; // wilson@kano

typedef struct HalHdmitxParameterList_s
{
    u8                           bHDMITxTaskIdCreated;
    u8                           bHDMITxEventIdCreated;
    u8                           bCheckRxTimerIdCreated;
    u8                           hdmitx_enable_flag;        ///< hdmitx module actived
    u8                           hdmitx_tmds_flag;          ///< hdmitx tmds on/off
    u8                           hdmitx_tmds_locked;        ///< hdmitx tmds on/off
    u8                           hdmitx_video_flag;         ///< hdmitx video on/off
    u8                           hdmitx_audio_flag;         ///< hdmitx audio on/off
    u8                           hdmitx_csc_flag;           ///< hdmitx csc on/off
    u8                           hdmitx_RB_swap_flag;       ///< hdmitx R/B swap
    u8                           hdmitx_force_mode;         ///< hdmitx output force mode: auto/force
    u8                           hdmitx_force_output_color; ///< hdmitx output force color format: auto/force
    u8                           hdmitx_AFD_override_mode;  ///< hdmitx AFD override mode: auto/override
    u8                           hdmitx_edid_ready;         ///< hdmitx get ready to Rx's EDID
    u8                           hdmitx_avmute_flag;        ///< hdmitx AVMUTE status
    u8                           hdmitx_CECEnable_flag;     ///< hdmitx CEC enable flag
    volatile HalHdmitxFsmType_e  hdmitx_fsm_state;          ///< hdmitx fsm state
    HalHdmitxFsmType_e           hdmitx_fsm_prestate;       ///< hdmitx fsm pre-state
    HalHdmitxRxStatus_e          hdmitx_preRX_status;       ///< hdmitx previous Rx status
    HalHdmitxRxStatus_e          hdmitx_curRX_status;       ///< hdmitx cur Rx status
    HalHdmitxOutpuModeType_e     output_mode;               ///< output DVI / HDMI mode
    HalHdmitxOutpuModeType_e     force_output_mode;         ///< output DVI / HDMI mode
    HalHdmitxColorimetry_e       colorimetry;
    HalHdmitxExtColorimetry_e    ext_colorimetry;
    HalHdmitxYccQuantRange_e     enYCCQuantRange;
    HalHdmitxRgbQuantRange_e     enRGBQuantRange;
    HalHdmitxColorDepthType_e    output_colordepth_val;   // output video color depth
    HalHdmitxTimingResType_e     output_video_timing;     ///< output video timing
    HalHdmitxTimingResType_e     output_video_prevtiming; ///< output video previous timing
    HalHdmitxColorType_e         input_color;             ///< RGB444 / YUV444
    HalHdmitxColorType_e         output_color;            ///< RGB444 / YUV444
    HalHdmitxYccQuantRange_e     input_range;
    HalHdmitxYccQuantRange_e     output_range;
    HalHdmitxColorType_e         force_output_color;          ///< RGB444 / YUV444
    HalHdmitxVideoAspectRatio_e  output_aspect_ratio;         // Aspect ratio
    HalHdmitxVideoScanInfo_e     output_scan_info;            // overscan / underscan
    HalHdmitxVideoAfdRatio_e     output_afd_ratio;            // AFD
    u8                           output_activeformat_present; // Active format information present
    HalHdmitxAudioFreqType_e     output_audio_frequncy;       ///< audio sampling frequency
    HalHdmitxAudioChannelType_e  output_audio_channel;        // audio channel count
    HalHdmitxAudioCodingType_e   output_audio_type;           // audio coding type
    HalHdmitxAnaloDrvCurConfig_t analog_setting;              // HDMI Tx Pre-emphasis and Double termination
    u8                           ubSSCEn;
    void *                       pvPMRIUBaseAddress;
    u32                          u32PMBankSize;
    void *                       pvCoproRIUBase;
    u8                           hdmitx_bypass_flag; // RxTxBypass
    u32                          u32HpdOnTime;
    u32                          u32TransmitTime;
    u32                          u32ValidateEdidTime;
    u32                          bHpdRestart;
    // EDID
    HalHdmitxRxEDIDInfo_t RxEdidInfo; // wilson@kano
} HalHdmitxParameterList_t;           // wilson@kano

typedef struct HalHdmitxPacketParameter_s
{
    u8                       User_Define;
    HalHdmitxPacketProcess_e Define_Process;
    u8                       Define_FCnt;
    u8                       bForceDisable;
} HalHdmitxPacketParameter_t;

typedef struct HalHdmitxResourcePrivate_s
{
    HalHdmitxParameterList_t   stHDMITxInfo;
    HalHdmitxPacketParameter_t stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_MAX];
    HalHdmitxPacketParameter_t stInfoFrameCfg[E_HAL_HDMITX_INFOFRAM_TYPE_MAX];
    HalHdmitxGCPktPara_t       stGCPara;
    HalHdmitxVSInfoPktPara_t   stVSInfoPara;
    HalHdmitxAVIInfoPktPara_t  stAVIInfoPara;
    HalHdmitxAUDInfoPktPara_t  stAUDInfoPara;
    u8                         bEnableHDMITxTask;
    u8                         bSetHPD;
    u8                         bResPriInit;
    u32                        u32AlignDummy;
    u32                        u32DeviceID;
    u8                         bResPriUsed;
} HalHdmitxResourcePrivate_t;

// debug mask definition
/**
 *   @brief HDMI Status
 */
typedef struct HalHdmitxStatus_s
{
    u8 bIsInitialized;
    u8 bIsRunning;
} HalHdmitxStatus_t;
typedef struct HalHdmitxAudioCtsNType_s
{
    u32 u32N;
    u32 u32CTS;
} HalHdmitxAudioCtsNType_t;

typedef struct HalHdmitxAudioFreqType_s
{
    u8                       CH_Status3;
    HalHdmitxAudioCtsNType_t stCtsN;
} HalHdmitxAudioFreqType_t;
typedef struct HalHdmitxVideoModeInfoType_s
{
    HalHdmitxVideoMode_e     i_p_mode;   // interlace / progressive mode
    HalHdmitxVideoPolarity_e h_polarity; // Hsync polarity
    HalHdmitxVideoPolarity_e v_polarity; // Vsync polarity
    u32                      pixel_clk;  // pixel clock
} HalHdmitxVideoModeInfoType_t;

#endif
