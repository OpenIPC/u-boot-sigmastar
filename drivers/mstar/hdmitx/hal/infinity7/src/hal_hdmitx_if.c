/*
* hal_hdmitx_if.c- Sigmastar
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

#define _HAL_HDMITX_IF_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "hal_hdmitx_st.h"
#include "drv_hdmitx_ctx.h"
#include "hal_hdmitx.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_HDMITX_IF_SPD_INFOFRAME_DATA_SIZE 25
#define HAL_HDMITX_IF_AVI_INFOFRAME_DATA_SIZE 13
#define HAL_HDMITX_IF_AUD_INFOFRAME_DATA_SIZE 24
#define HAL_HDMITX_CTX_LOCK_SEM(x)            DrvHdmitxOsObtainSemaphore(x)
#define HAL_HDMITX_CTX_UNLOCK_SEM(x)          DrvHdmitxOsReleaseSemaphore(x)
#define HDMITX_EDID_DEBUG_CHECK()             (g_u32HdmitxDbgLevel & HDMITX_DBG_LEVEL_EDID)
#define HDMITX_DBG_PARSE_CEA_EDID()           (g_u32HdmitxDbgLevel & HDMITX_DBG_LEVEL_EDIDEXT)
#define HDMITX_DBG_HFVSDB_INFO()              (g_u32HdmitxDbgLevel & HDMITX_DBG_LEVEL_HFVSDB)
#define HDMITX_DBG_HDMIVSDB_INFO()            (g_u32HdmitxDbgLevel & HDMITX_DBG_LEVEL_HDMIVSDB)
#define HDMITX_EDID_CHECK_422                 0x10
#define HDMITX_EDID_CHECK_444                 0x20

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalHdmitxQueryRet_e (*pGetInfoFunc)(void *, void *);
    void (*pSetFunc)(void *, void *);
    u16 u16CfgSize;
} HalHdmitxQueryCallBackFunc_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8                              g_bHalIfInit = 0;
HalHdmitxQueryCallBackFunc_t    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_MAX];
HalHdmitxForceAttrConfig_t      g_stForceAttrCfg = {E_HAL_HDMITX_FORCE_FLAG_NONE, 0, E_HAL_HDMITX_OUTPUT_MODE_HDMI, 1};
u8                              g_bHpdIrqEn      = FALSE;
extern u16                      g_u16I2cUsed;
s32                             g_s32HDMITXMutex = -1;
static DrvHdmitxOsMutexConfig_t g_tHDMITXMutex;
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static u8 _HalHdmitxIfGetHwCtx(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxResourcePrivate_t **psHDMITXResPri)
{
    u8 u8Ret = 0;

    if (pInstance)
    {
        *psHDMITXResPri = pInstance->pstHwCtx;
        if (*psHDMITXResPri)
        {
            u8Ret = 1;
        }
    }
    return u8Ret;
}
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoClkSet(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e   enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxClkConfig_t *pstHalClkCfg = (HalHdmitxClkConfig_t *)pCfg;

    if (pstHalClkCfg->u32Num != HAL_HDMITX_CLK_NUM)
    {
        enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
        HDMITX_ERR("%s %d, Clk Num is not Correct, %d != %d\n", __FUNCTION__, __LINE__, pstHalClkCfg->u32Num,
                   HAL_HDMITX_CLK_NUM);
    }
    return enRet;
}

static void _HalHdmitxIfSetClkSet(void *pCtx, void *pCfg)
{
    HalHdmitxClkConfig_t *pstHalClkCfg = (HalHdmitxClkConfig_t *)pCfg;
    u8                    abEn[HAL_HDMITX_CLK_NUM];
    u32                   au32ClkRate[HAL_HDMITX_CLK_NUM];
    u32                   i;

    memcpy(abEn, pstHalClkCfg->bEn, sizeof(u8) * pstHalClkCfg->u32Num);
    memcpy(au32ClkRate, pstHalClkCfg->u32Rate, sizeof(u32) * pstHalClkCfg->u32Num);

    HalHdmitxSetClk(abEn, au32ClkRate, pstHalClkCfg->u32Num);
    for (i = 0; i < HAL_HDMITX_CLK_NUM; i++)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, En:%d, ClkRate:%d\n", __FUNCTION__, __LINE__, abEn[i],
                   au32ClkRate[i]);
    }
}

static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoClkGet(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e   enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxClkConfig_t *pstHalClkCfg = (HalHdmitxClkConfig_t *)pCfg;
    u8                    abEn[HAL_HDMITX_CLK_NUM];
    u32                   au32ClkRate[HAL_HDMITX_CLK_NUM];
    u32                   i;

    if (pstHalClkCfg->u32Num == HAL_HDMITX_CLK_NUM)
    {
        if (HalHdmitxGetClk(abEn, au32ClkRate, HAL_HDMITX_CLK_NUM))
        {
            for (i = 0; i < HAL_HDMITX_CLK_NUM; i++)
            {
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, En:%d, ClkRate:%d\n", __FUNCTION__, __LINE__, abEn[i],
                           au32ClkRate[i]);
            }
            memcpy(pstHalClkCfg->bEn, abEn, sizeof(u8) * HAL_HDMITX_CLK_NUM);
            memcpy(pstHalClkCfg->u32Rate, au32ClkRate, sizeof(u32) * HAL_HDMITX_CLK_NUM);
        }
        else
        {
            enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
            HDMITX_ERR("%s %d, Get CLK Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
        HDMITX_ERR("%s %d, Clk Num is not Correct, %d != %d\n", __FUNCTION__, __LINE__, pstHalClkCfg->u32Num,
                   HAL_HDMITX_CLK_NUM);
    }
    return enRet;
}

static void _HalHdmitxIfSetClkGet(void *pCtx, void *pCfg) {}

static void _HalHdmitxIfSetAVIInfoPktPara(DrvHdmitxCtxConfig_t *pInstance)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stAVIInfoPara.enableAFDoverWrite = psHDMITXResPri->stHDMITxInfo.hdmitx_AFD_override_mode;
    psHDMITXResPri->stAVIInfoPara.A0Value            = psHDMITXResPri->stHDMITxInfo.output_activeformat_present;
    psHDMITXResPri->stAVIInfoPara.enColorimetry      = psHDMITXResPri->stHDMITxInfo.colorimetry;
    psHDMITXResPri->stAVIInfoPara.enExtColorimetry   = psHDMITXResPri->stHDMITxInfo.ext_colorimetry;
    psHDMITXResPri->stAVIInfoPara.enYCCQuantRange    = psHDMITXResPri->stHDMITxInfo.enYCCQuantRange;
    psHDMITXResPri->stAVIInfoPara.enRGBQuantRange    = psHDMITXResPri->stHDMITxInfo.enRGBQuantRange;
    psHDMITXResPri->stAVIInfoPara.enVidTiming        = psHDMITXResPri->stHDMITxInfo.output_video_timing;
    psHDMITXResPri->stAVIInfoPara.enAFDRatio         = psHDMITXResPri->stHDMITxInfo.output_afd_ratio;
    psHDMITXResPri->stAVIInfoPara.enScanInfo         = psHDMITXResPri->stHDMITxInfo.output_scan_info;
    psHDMITXResPri->stAVIInfoPara.enAspectRatio      = psHDMITXResPri->stHDMITxInfo.output_aspect_ratio;

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag)
    {
        psHDMITXResPri->stAVIInfoPara.enColorFmt = psHDMITXResPri->stHDMITxInfo.output_color;
    }
    else
    {
        if (!HDMITX_CSC_SUPPORT_R2Y(psHDMITXResPri->u32DeviceID))
        {
            psHDMITXResPri->stAVIInfoPara.enColorFmt = E_HAL_HDMITX_COLOR_RGB444;
        }
        else
        {
            psHDMITXResPri->stAVIInfoPara.enColorFmt = psHDMITXResPri->stHDMITxInfo.output_color;
        }
    }
}
static void _HalHdmitxIfSetGCPParameter(DrvHdmitxCtxConfig_t *pInstance)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stGCPara.enAVMute =
        (psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag == TRUE) ? E_HDMITX_GCP_SET_AVMUTE : E_HDMITX_GCP_CLEAR_AVMUTE;
    psHDMITXResPri->stGCPara.enColorDepInfo = psHDMITXResPri->stHDMITxInfo.output_colordepth_val;
}
static void _HalHdmitxIfSetAudInfoPktPara(DrvHdmitxCtxConfig_t *pInstance)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stAUDInfoPara.enAudChCnt = psHDMITXResPri->stHDMITxInfo.output_audio_channel;
    psHDMITXResPri->stAUDInfoPara.enAudFreq  = psHDMITXResPri->stHDMITxInfo.output_audio_frequncy;
    psHDMITXResPri->stAUDInfoPara.enAudType  = psHDMITXResPri->stHDMITxInfo.output_audio_type;
}
static void _HalHdmitxIfEdidCheckInit(HalHdmitxResourcePrivate_t *psHDMITXResPri)
{
    // edid relative //assign initial value
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk0, 0x00, HDMITX_EDID_BLK_SIZE * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1, 0x00, HDMITX_EDID_BLK_SIZE * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ManufacturerID, 0x00, 3 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen, 0x00, HDMITX_CEA_DAT_BLK_TYPE_NUM * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr, 0x00, 2 * sizeof(u8));
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi = FALSE;

    // HF-VSDB, scdc relative
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDBVerInfo        = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate       = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportIndependView   = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportDualView       = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3DOsdDisparity = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportLTEScramble    = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportSCDC           = FALSE;

    // read request
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YUV420DeepColorInfo = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI        = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3DPresent          = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_50Hz      = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_60Hz      = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3D_50Hz      = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3D_60Hz      = FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ColorDepthInfo      = E_HAL_HDMITX_CD_NO_ID;

    // data block content
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudioDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VendorDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SpeakerAllocDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VESA_DTCDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HdmiVICList, 0x00, 7 * sizeof(u8)); // this field has only 3 bits
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming, 0x00, 32 * sizeof(HalHdmitxRxEDIDVideoInfo_t));

    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDMI_VSDB, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDB, 0x00, 32 * sizeof(u8));
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDMI_VSDB_Len   = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDB_Len     = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3dMultiPresent = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Hdmi3DLen       = 0x00;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HdmiVICLen      = 0x00;

    // for 420
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support420ColorFmt = FALSE;
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420VidDataBlk, 0x00, 32 * sizeof(u8));
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420CapMapDataBlk, 0x00, 32 * sizeof(u8));

    // for colorimetry
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ExtColorimetry = 0x00;

    // for HDR
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.bSupportHDR = FALSE;
    memset(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDRStaticDataBlk, 0x00, 32 * sizeof(u8));

    // VCDB
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoCapDataBlk = 0x00;
}
static void _HalHdmitxIfSwInit(DrvHdmitxCtxConfig_t *pInstance, u32 u32DevId)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated   = FALSE;
    psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated  = FALSE;
    psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_enable_flag     = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_flag       = TRUE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag      = TRUE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag      = TRUE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag        = FALSE;

    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_PENDING);
    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate, E_HAL_HDMITX_FSM_PENDING);
    psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status       = E_HDMITX_DVICLOCK_L_HPD_L;
    psHDMITXResPri->stHDMITxInfo.hdmitx_curRX_status       = E_HDMITX_DVICLOCK_L_HPD_L;
    psHDMITXResPri->stHDMITxInfo.hdmitx_RB_swap_flag       = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode         = g_stForceAttrCfg.bOutputMode;
    psHDMITXResPri->stHDMITxInfo.hdmitx_force_output_color = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_AFD_override_mode  = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready         = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag        = FALSE;
    psHDMITXResPri->stHDMITxInfo.hdmitx_CECEnable_flag     = TRUE; // CEC

    psHDMITXResPri->stHDMITxInfo.output_mode                 = E_HAL_HDMITX_OUTPUT_MODE_DVI;
    psHDMITXResPri->stHDMITxInfo.force_output_mode           = g_stForceAttrCfg.enOutputMode;
    psHDMITXResPri->stHDMITxInfo.ext_colorimetry             = E_HDMITX_EXT_COLORIMETRY_XVYCC601;
    psHDMITXResPri->stHDMITxInfo.colorimetry                 = E_HDMITX_COLORIMETRY_MAX;
    psHDMITXResPri->stHDMITxInfo.enYCCQuantRange             = E_HDMITX_YCC_QUANT_LIMIT;
    psHDMITXResPri->stHDMITxInfo.enRGBQuantRange             = E_HDMITX_RGB_QUANT_DEFAULT;
    psHDMITXResPri->stHDMITxInfo.output_colordepth_val       = E_HAL_HDMITX_CD_NO_ID; //;
    psHDMITXResPri->stHDMITxInfo.output_video_prevtiming     = E_HAL_HDMITX_RES_MAX;
    psHDMITXResPri->stHDMITxInfo.output_video_timing         = E_HAL_HDMITX_RES_720X480P_60HZ;
    psHDMITXResPri->stHDMITxInfo.force_output_color          = E_HAL_HDMITX_COLOR_RGB444;
    psHDMITXResPri->stHDMITxInfo.input_color                 = E_HAL_HDMITX_COLOR_RGB444;
    psHDMITXResPri->stHDMITxInfo.output_color                = E_HAL_HDMITX_COLOR_RGB444;
    psHDMITXResPri->stHDMITxInfo.input_range                 = E_HDMITX_YCC_QUANT_LIMIT;
    psHDMITXResPri->stHDMITxInfo.output_range                = E_HDMITX_YCC_QUANT_LIMIT;
    psHDMITXResPri->stHDMITxInfo.output_aspect_ratio         = E_HDMITX_VIDEO_AR_4_3;
    psHDMITXResPri->stHDMITxInfo.output_scan_info            = E_HDMITX_VIDEO_SI_NODATA;
    psHDMITXResPri->stHDMITxInfo.output_afd_ratio            = E_HDMITX_VIDEO_AFD_SAMEASPICTUREAR;
    psHDMITXResPri->stHDMITxInfo.output_activeformat_present = 1;
    psHDMITXResPri->stHDMITxInfo.output_audio_frequncy       = E_HAL_HDMITX_AUDIO_FREQ_48K;
    psHDMITXResPri->stHDMITxInfo.hdmitx_bypass_flag          = FALSE;
#ifdef OBERON
    psHDMITXResPri->stHDMITxInfo.output_audio_channel = E_HAL_HDMITX_AUDIO_CH_8;
#else // Uranus, Uranus4
    psHDMITXResPri->stHDMITxInfo.output_audio_channel = E_HAL_HDMITX_AUDIO_CH_2;
#endif
    psHDMITXResPri->stHDMITxInfo.output_audio_type = E_HAL_HDMITX_AUDIO_CODING_PCM;

    // HDMI Tx Pre-emphasis and Double termination
    memset(&psHDMITXResPri->stHDMITxInfo.analog_setting, 0, sizeof(psHDMITXResPri->stHDMITxInfo.analog_setting));
    _HalHdmitxIfEdidCheckInit(psHDMITXResPri);
    _HalHdmitxIfSetAVIInfoPktPara(pInstance);
    _HalHdmitxIfSetGCPParameter(pInstance);
    _HalHdmitxIfSetAudInfoPktPara(pInstance);
}
static void _HalHdmitxIfSetTMDSOnOff(HalHdmitxResourcePrivate_t *psHDMITXResPri, u8 state)
{
    psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_flag = state;

    if (!psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_locked)
    {
        HalHdmitxSetTMDSOnOff(psHDMITXResPri->u32DeviceID, psHDMITXResPri->stHDMITxInfo.hdmitx_RB_swap_flag,
                              psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_flag);
        HalHdmitxDeviceOnOff(state, psHDMITXResPri->u32DeviceID);
    }
}
static u8 _HalHdmitxIfParseHFVSDB(DrvHdmitxCtxConfig_t *pInstance, u8 *pu8BlkData, u8 u8BlkLen, u8 u8SupTimingNum)
{
    u8                          i                     = 0;
    const u8                    a8HfvsdbHdmiIeeeId[3] = {0xD8, 0x5D, 0xC4};
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    // check IEEE HDMI identifier //NOTE: current index is BYTE 1
    for (i = 0; i < sizeof(a8HfvsdbHdmiIeeeId); i++)
    {
        if (*(pu8BlkData + i) != a8HfvsdbHdmiIeeeId[i])
        {
            return FALSE;
        }
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "%s::IEEE HDMI Identifier PASS!\n", __FUNCTION__);

    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDBVerInfo = *(pu8BlkData + 3);
    // psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate = *(pu8BlkData + 4);
    if ((*(pu8BlkData + 4)) > psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate)
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate = *(pu8BlkData + 4);
    }

    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportSCDC           = (*(pu8BlkData + 5) & 0x80) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportRR             = (*(pu8BlkData + 5) & 0x40) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportLTEScramble    = (*(pu8BlkData + 5) & 0x08) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportIndependView   = (*(pu8BlkData + 5) & 0x04) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportDualView       = (*(pu8BlkData + 5) & 0x02) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3DOsdDisparity = (*(pu8BlkData + 5) & 0x01) ? TRUE : FALSE;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YUV420DeepColorInfo   = (*(pu8BlkData + 6) & 0x07);

    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] Version = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDBVerInfo);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] Max_TMDS_Character_Rate = %d Mhz\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate * 5);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] SCDC_Present = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportSCDC);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] RR_Capable = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportRR);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] LTF_340Mcsc_Scramble = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportLTEScramble);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] Independent_View = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportIndependView);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] Dual_View = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportDualView);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] 3D_OSD_Disparity = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3DOsdDisparity);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HFVSDB, "[HF-VSDB] DC_420_Info = %d\r\n",
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YUV420DeepColorInfo);
    return TRUE;
}
static void _HalHdmitxIfParseDeepColorInfo(DrvHdmitxCtxConfig_t *pInstance, u8 u8DataByte)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    // check Deep Color information
    if (u8DataByte & 0x40)
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ColorDepthInfo = E_HAL_HDMITX_CD_48_BITS;
        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::Support Color Depth 48-bit\r\n", __FUNCTION__);
    }
    else if (u8DataByte & 0x20)
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ColorDepthInfo = E_HAL_HDMITX_CD_36_BITS;
        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::Support Color Depth 36-bit\r\n", __FUNCTION__);
    }
    else if (u8DataByte & 0x10)
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ColorDepthInfo = E_HAL_HDMITX_CD_30_BITS;
        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::Support Color Depth 30-bit\r\n", __FUNCTION__);
    }
    else
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ColorDepthInfo = E_HAL_HDMITX_CD_24_BITS;
    }
}

static u8 _HalHdmitxIfParseHdmiVSDB(DrvHdmitxCtxConfig_t *pInstance, u8 *pu8BlkData, u8 u8BlkLen, u8 u8SupTimingNum)
{
    u8                          i = 0;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    u8                          u8IdxOffset = 0;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    // get Physical Address
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[0] = *(pu8BlkData + 3);
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[1] = *(pu8BlkData + 4);
    HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "Physcal Address = [%d, %d, %d, %d]\r\n",
               (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[0] & 0xF0) >> 4,
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[0] & 0x0F,
               (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[1] & 0xF0) >> 4,
               psHDMITXResPri->stHDMITxInfo.RxEdidInfo.PhyAddr[1] & 0x0F);

    if (u8BlkLen > 5)
    {
        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI = (*(pu8BlkData + 5) & 0x80) ? TRUE : FALSE;

        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::Support AI = %d !\n", __FUNCTION__,
                   psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI);
        // check Deep Color information
        _HalHdmitxIfParseDeepColorInfo(pInstance, *(pu8BlkData + 5));

        if (u8BlkLen > 7)
        {
            if (*(pu8BlkData + 7) & 0x20) // HDMI_Video_Present
            {
                if ((*(pu8BlkData + 7) & 0xC0) == 0xC0) // Latency_Field_present & I_Latency_Field_Present
                {
                    u8IdxOffset = 4;
                }
                else if ((*(pu8BlkData + 7) & 0xC0) == 0x40) // I_Latency_Field_Present
                {
                    u8IdxOffset = 2;
                }
                else
                {
                    u8IdxOffset = 0;
                }
                if ((*(pu8BlkData + 6)) > psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate)
                {
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.MaxTmdsCharRate = *(pu8BlkData + 6);
                }

                if (u8BlkLen > (u8IdxOffset + 9))
                {
                    if ((*(pu8BlkData + u8IdxOffset + 8) & 0x80)
                        || (((*(pu8BlkData + u8IdxOffset + 9) & 0xE0) >> 5) > 0)) // 3D present and VIC LEN
                    {
                        u8  u83dMultiPresent = 0;
                        u8  u8Hdmi3DLen      = 0;
                        u8  u8HdmiVICLen     = 0;
                        u16 u163dStructAll   = 0;
                        u16 u163dMask        = 0;

                        // psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3DPresent = TRUE;
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3DPresent =
                            (*(pu8BlkData + u8IdxOffset + 8) & 0x80) ? TRUE : FALSE;
                        u83dMultiPresent = (*(pu8BlkData + u8IdxOffset + 8) & 0x60) >> 5;
                        u8Hdmi3DLen      = (*(pu8BlkData + u8IdxOffset + 9) & 0x1F);
                        u8HdmiVICLen     = (*(pu8BlkData + u8IdxOffset + 9) & 0xE0) >> 5;
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3dMultiPresent = u83dMultiPresent;
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Hdmi3DLen       = u8Hdmi3DLen;
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HdmiVICLen      = u8HdmiVICLen;

                        if (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_60Hz
                            && psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3DPresent)
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3D_60Hz = TRUE;
                        }
                        if (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_50Hz
                            && psHDMITXResPri->stHDMITxInfo.RxEdidInfo.b3DPresent)
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support3D_50Hz = TRUE;
                        }
                        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::3dmulti_present=%x, 3DLen=%d, VicLen=%d !\n",
                                   __FUNCTION__, u83dMultiPresent, u8Hdmi3DLen, u8HdmiVICLen);
                        // fetch VIC list
                        if (u8HdmiVICLen > 0)
                        {
                            if (u8BlkLen > (u8IdxOffset + 9 + u8HdmiVICLen))
                            {
                                for (i = 0; i < u8HdmiVICLen; i++)
                                {
                                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HdmiVICList[i] =
                                        *(pu8BlkData + u8IdxOffset + 10 + i);
                                }
                                HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s::HDMI VIC list\r\n", __FUNCTION__);
                                for (i = 0; i < u8HdmiVICLen; i++)
                                {
                                    HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "0x%02hhx ", u83dMultiPresent);
                                }
                                HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "\n");

                                u8IdxOffset += u8HdmiVICLen; // HDMI_VIC_LEN
                            }
                        }

                        HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB, "%s 3D_MultiPresent = 0x%02x\r\n", __FUNCTION__,
                                   u83dMultiPresent);

                        if (u83dMultiPresent == 0x01) // 3D_Structure_ALL_15..0
                        {
                            // check length
                            if (u8BlkLen > (u8IdxOffset + 11))
                            {
                                u163dStructAll =
                                    (*(pu8BlkData + u8IdxOffset + 10) << 8) | (*(pu8BlkData + u8IdxOffset + 11));
                                for (i = 0; i < ((u8SupTimingNum > 16) ? 16 : u8SupTimingNum); i++)
                                {
                                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[i].Video3DInfo =
                                        u163dStructAll;
                                    HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB,
                                               "%s :SupVidTiming[%02d].Video3DInfo = 0x%02x\r\n", __FUNCTION__, i,
                                               u163dStructAll);
                                }
                                u8Hdmi3DLen -= 2;
                            }
                        }
                        else if (u83dMultiPresent == 0x02) // 3D_Structure_ALL_15..0 and 3D_Mask_ALL_15..0
                        {
                            // check Length
                            if (u8BlkLen > (u8IdxOffset + 13))
                            {
                                u163dStructAll =
                                    (*(pu8BlkData + u8IdxOffset + 10) << 8) | (*(pu8BlkData + u8IdxOffset + 11));
                                u163dMask =
                                    (*(pu8BlkData + u8IdxOffset + 12) << 8) | (*(pu8BlkData + u8IdxOffset + 13));

                                for (i = 0; i < ((u8SupTimingNum > 16) ? 16 : u8SupTimingNum); i++)
                                {
                                    if ((u163dMask >> i) & 0x01)
                                    {
                                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[i].Video3DInfo =
                                            u163dStructAll;
                                    }
                                    else
                                    {
                                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[i].Video3DInfo = 0;
                                    }
                                    HDMITX_DBG(HDMITX_DBG_LEVEL_HDMIVSDB,
                                               "%s :SupVidTiming[%02d].Video3DInfo = 0x%02x\r\n", __FUNCTION__, i,
                                               u163dStructAll);
                                }
                                u8Hdmi3DLen -= 4;
                            }
                        }

                        if (u8Hdmi3DLen > 1) // 3D structure remains
                        {
                            if (u8BlkLen > (u8IdxOffset + 11))
                            {
                                u8 u8VicOrder = 0;
                                u8 u83DStruct = 0;

                                for (i = 0; i < u8Hdmi3DLen; i++)
                                {
                                    if (u83dMultiPresent == 0x01)
                                    {
                                        u8VicOrder = (*(pu8BlkData + u8IdxOffset + 12 + i) & 0xF0) >> 4;
                                        u83DStruct = (*(pu8BlkData + u8IdxOffset + 12 + i) & 0x0F);
                                    }
                                    else if (u83dMultiPresent == 0x02)
                                    {
                                        u8VicOrder = (*(pu8BlkData + u8IdxOffset + 14 + i) & 0xF0) >> 4;
                                        u83DStruct = (*(pu8BlkData + u8IdxOffset + 14 + i) & 0x0F);
                                    }
                                    else
                                    {
                                        u8VicOrder = (*(pu8BlkData + u8IdxOffset + 10 + i) & 0xF0) >> 4;
                                        u83DStruct = (*(pu8BlkData + u8IdxOffset + 10 + i) & 0x0F);
                                    }

                                    switch (u83DStruct)
                                    {
                                        case 0:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_FRAMEPACKING;
                                            break;

                                        case 1:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_FIELDALTERNATIVE;
                                            break;

                                        case 2:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_LINEALTERNATIVE;
                                            break;

                                        case 3:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_SIDEBYSIDE_FULL;
                                            break;

                                        case 4:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_L_DEP;
                                            break;

                                        case 5:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_L_DEP_GRAPHIC_DEP;
                                            break;

                                        case 6:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_TOPANDBOTTOM;
                                            break;

                                        case 7:
                                            break;

                                        case 8:
                                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[u8VicOrder]
                                                .Video3DInfo |= E_HDMITX_EDID_3D_SIDEBYSIDE_HALF_HORIZONTAL;
                                            i++;
                                            break;

                                        default:
                                            i++;
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return TRUE;
}
static u8 _HalHdmitxIfParseCEAExtEdid(DrvHdmitxCtxConfig_t *pInstance, u8 *pu8BlkData) // wilson@kano
{
    u8                          u8DescOffset        = 0;
    u8                          u8CurIdx            = 0;
    u8                          i                   = 0;
    u8                          u8BlkLen            = 0;
    u8                          u8BlkTagCode        = 0;
    const u8                    u8VsdbHdmiIeeeId[3] = {0x03, 0x0C, 0x00};
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    // support CEA Extension Version 3
    if ((*(pu8BlkData) == 0x02) && (*(pu8BlkData + 1) < 0x04))
    {
        u8DescOffset = *(pu8BlkData + 2);

        if ((u8DescOffset < 0x05) || (u8DescOffset == 0xFF)) // invalid offset value;
        {
            return FALSE;
        }
        u8CurIdx = 0x04; // start of data block collection

        while (u8CurIdx < u8DescOffset)
        {
            u8BlkTagCode = (*(pu8BlkData + u8CurIdx) & 0xE0) >> 5;
            u8BlkLen     = *(pu8BlkData + u8CurIdx) & 0x1F;

            switch (u8BlkTagCode)
            {
                case E_HAL_HDMITX_CEATAG_AUDIOBLK:
                {
                    if (HDMITX_DBG_PARSE_CEA_EDID())
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Short Audio Block:%d \n", __FUNCTION__, u8BlkLen);
                    }

                    u8CurIdx++;

                    for (i = 0; i < u8BlkLen; i++)
                    {
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudioDataBlk[i] = *(pu8BlkData + u8CurIdx + i);
                    }
                    u8CurIdx += u8BlkLen;
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen[u8BlkTagCode] = u8BlkLen;
                }
                break;

                case E_HAL_HDMITX_CEATAG_VIDEOBLK:
                {
                    u8 ucOffset = 0; // for multiple video block present
                    if (HDMITX_DBG_PARSE_CEA_EDID())
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Short Video Block: %d\r\n", __FUNCTION__, u8BlkLen);
                    }
                    u8CurIdx++;
                    ucOffset = pInstance->u8SupTimingNum;
                    for (i = 0; i < u8BlkLen; i++)
                    {
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoDataBlk[ucOffset + i] =
                            *(pu8BlkData + u8CurIdx + i);
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum].VideoTiming =
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoDataBlk[ucOffset + i] & 0x7F;

                        if ((psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum].VideoTiming
                             <= E_HDMITX_VIC_1920X1080P_60_16_9)
                            | (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                   .VideoTiming
                               == E_HDMITX_VIC_2880X480P_60_4_3)
                            | (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                   .VideoTiming
                               == E_HDMITX_VIC_2880X480P_60_16_9))
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_60Hz = TRUE;
                        }
                        else if ((psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                      .VideoTiming
                                  <= E_HDMITX_VIC_1920X1080P_50_16_9)
                                 | (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                        .VideoTiming
                                    == E_HDMITX_VIC_2880X576P_50_4_3)
                                 | (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                        .VideoTiming
                                    == E_HDMITX_VIC_2880X576P_50_16_9)
                                 | (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupVidTiming[pInstance->u8SupTimingNum]
                                        .VideoTiming
                                    == E_HDMITX_VIC_1920X1080I_50_16_9_1250_TOTAL))
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.Support2D_50Hz = TRUE;
                        }
                        pInstance->u8SupTimingNum++;
                    }

                    u8CurIdx += u8BlkLen;
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen[u8BlkTagCode] = u8BlkLen;
                }
                break;
                case E_HAL_HDMITX_CEATAG_VENDORSPECIFIC:
                {
                    u8 bIsVSDB = TRUE;
                    if (HDMITX_DBG_PARSE_CEA_EDID())
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::VSDB\r\n", __FUNCTION__);
                    }
                    u8CurIdx++;

                    // check IEEE HDMI identifier
                    for (i = 0; i < sizeof(u8VsdbHdmiIeeeId); i++)
                    {
                        if (*(pu8BlkData + u8CurIdx + i) != u8VsdbHdmiIeeeId[i])
                        {
                            bIsVSDB = FALSE;
                            break;
                        }
                    }

                    if (bIsVSDB == FALSE)
                    {
                        // check if HF-VSDB
                        if (_HalHdmitxIfParseHFVSDB(pInstance, pu8BlkData + u8CurIdx, u8BlkLen,
                                                    pInstance->u8SupTimingNum))
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDB_Len = u8BlkLen;
                            memcpy(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HF_VSDB, pu8BlkData + u8CurIdx, u8BlkLen);
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi = TRUE;
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "HF-VSDB Presented!!\n");
                        }
                        else
                        {
                            HDMITX_ERR("HF-VSDB Parsing Failed!!\n");
                        }
                    }
                    else
                    {
                        psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi = TRUE;

                        if (_HalHdmitxIfParseHdmiVSDB(pInstance, pu8BlkData + u8CurIdx, u8BlkLen,
                                                      pInstance->u8SupTimingNum))
                        {
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDMI_VSDB_Len = u8BlkLen;
                            memcpy(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDMI_VSDB, pu8BlkData + u8CurIdx, u8BlkLen);

                            psHDMITXResPri->stHDMITxInfo.output_mode |= (u8)HDMITX_BIT1; // default is DVI mode

                            switch (*(pu8BlkData + 3) & 0x30)
                            {
                                case 0x00:
                                    if (psHDMITXResPri->stHDMITxInfo.input_color == E_HAL_HDMITX_COLOR_YUV444
                                        || psHDMITXResPri->stHDMITxInfo.input_color == E_HAL_HDMITX_COLOR_RGB444)
                                    {
                                        psHDMITXResPri->stHDMITxInfo.output_color = E_HAL_HDMITX_COLOR_RGB444;
                                        if (HDMITX_DBG_PARSE_CEA_EDID())
                                        {
                                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Not Support YCbCr!!\n",
                                                       __FUNCTION__);
                                        }
                                    }
                                    break;

                                case 0x20:
                                case 0x30:
                                    if (psHDMITXResPri->stHDMITxInfo.input_color == E_HAL_HDMITX_COLOR_YUV444
                                        || psHDMITXResPri->stHDMITxInfo.input_color == E_HAL_HDMITX_COLOR_RGB444)
                                    {
                                        psHDMITXResPri->stHDMITxInfo.output_color = E_HAL_HDMITX_COLOR_YUV444;
                                        if (HDMITX_DBG_PARSE_CEA_EDID())
                                        {
                                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Output YUV444!!\n", __FUNCTION__);
                                        }
                                    }
                                    break;

                                case 0x10:
                                    if (psHDMITXResPri->stHDMITxInfo.input_color == E_HAL_HDMITX_COLOR_YUV444)
                                    {
                                        psHDMITXResPri->stHDMITxInfo.output_color = E_HAL_HDMITX_COLOR_YUV422;
                                        if (HDMITX_DBG_PARSE_CEA_EDID())
                                        {
                                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Output YUV422!!\n", __FUNCTION__);
                                        }
                                    }
                                    break;

                                default:
                                    break;
                            }
                        }
                    }
                    u8CurIdx += u8BlkLen;
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen[u8BlkTagCode] = u8BlkLen;
                }
                break;

                case E_HAL_HDMITX_CEATAG_USEEXTTAG:
                {
                    u8 ucExtTagCode = 0x00;

                    ucExtTagCode = *(pu8BlkData + (++u8CurIdx));
                    u8CurIdx++;
                    HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "%s::Extend Tag Code = 0x%02X, block Len = 0x%X\r\n",
                               __FUNCTION__, ucExtTagCode, u8BlkLen);

                    switch (ucExtTagCode)
                    {
                        case E_HAL_HDMITX_CEAEXTTAG_VIDEOCAP_DB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "Video Capability Data Block::\n");
                            if (u8BlkLen != 0x02) // valid length
                            {
                                HDMITX_ERR("Invalid Length of Video Capability Data Block\r\n");
                                return FALSE;
                            }
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoCapDataBlk = *(pu8BlkData + u8CurIdx);
                            u8CurIdx++;
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                            break;

                        case E_HAL_HDMITX_CEAEXTTAG_YCBCR420VIDEODB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "YCbCr420 Video Data Block::\n");
                            for (i = 0; i < (u8BlkLen - 1); i++)
                            {
                                psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420VidDataBlk[i] =
                                    *(pu8BlkData + u8CurIdx);
                                u8CurIdx++;
                                HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "0x%02X, ",
                                           psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420VidDataBlk[i]);
                            }
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                            break;

                        case E_HAL_HDMITX_CEAEXTTAG_YCBCR420CAPMAPDB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "YCbCr420 Capability Map Data Block::\n");
                            for (i = 0; i < (u8BlkLen - 1); i++)
                            {
                                psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420CapMapDataBlk[i] =
                                    *(pu8BlkData + u8CurIdx);
                                u8CurIdx++;
                                HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "0x%02X, ",
                                           psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420CapMapDataBlk[i]);
                            }
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                            break;

                        case E_HAL_HDMITX_CEAEXTTAG_COLORIMETRYDB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "Colorimetry Data Block::\n");
                            if (u8BlkLen != 0x03) // valid length
                            {
                                HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "Invalid Length of Colorimetry Data Block\r\n");
                                return FALSE;
                            }
                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ExtColorimetry = *(pu8BlkData + u8CurIdx);
                            u8CurIdx += 2; // u8CurIdx++;
                            // TBD: MD parsing
                            break;

                        case E_HAL_HDMITX_CEAEXTTAG_HDRSTATICMETADB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "HDR Static Metadata Data Block::\n");
                            for (i = 0; i < (u8BlkLen - 1); i++)
                            {
                                psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDRStaticDataBlk[i] = *(pu8BlkData + u8CurIdx);
                                u8CurIdx++;
                                HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "0x%02X, ",
                                           psHDMITXResPri->stHDMITxInfo.RxEdidInfo.HDRStaticDataBlk[i]);
                                if ((i + 1) % 16 == 0)
                                {
                                    HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                                }
                            }
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");

                            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.bSupportHDR = TRUE;
                            break;

                        case E_HAL_HDMITX_CEAEXTTAG_VSVIDEO_DB:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "Vendor Specific Video Data Block::\n");
                            for (i = 0; i < (u8BlkLen - 1); i++)
                            {
                                psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VSVideoBlk[i] = *(pu8BlkData + u8CurIdx);
                                u8CurIdx++;
                                HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "0x%02X, ",
                                           psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VSVideoBlk[i]);
                                if ((i + 1) % 16 == 0)
                                {
                                    HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                                }
                            }
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT, "\r\n");
                            break;

                        default:
                            HDMITX_DBG(HDMITX_DBG_LEVEL_EDIDEXT,
                                       "drv_HDMITx_ParseCEAExtEdid()::Extend Tag Code Unsupport!!\n");
                            u8CurIdx += (u8BlkLen - 1);
                            break;
                    }
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen[u8BlkTagCode] = u8BlkLen;
                }
                break;

                case E_HAL_HDMITX_CEATAG_SPEAKERALLOC:
                case E_HAL_HDMITX_CEATAG_VESA_DTCBLK:
                case E_HAL_HDMITX_CEATAG_RESERVED_1:
                default:
                    u8CurIdx += (u8BlkLen + 1);
                    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.CEADataBlkLen[u8BlkTagCode] = u8BlkLen;
                    break;
            }
        }
    }

    return TRUE;
}

static u8 _HalHdmitxIfEdidChecking(DrvHdmitxCtxConfig_t *pInstance)
{
    u8                          i                              = 0;
    u8                          u8EDIDRetryCnt                 = 3;
    u8                          u8ExtBlkNum                    = 0;
    u8                          u8ValidBlkNum                  = 0;
    u8                          ablkData[HDMITX_EDID_BLK_SIZE] = {0x00};
    const u8                    aEdidHeaderTag[8]              = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }
    if (!pInstance->bCheckEDID)
    {
        return TRUE;
    }
    // init buf
    pInstance->u8SupTimingNum = 0;
    _HalHdmitxIfEdidCheckInit(psHDMITXResPri);

    psHDMITXResPri->stHDMITxInfo.output_mode &= (~(u8)HDMITX_BIT1); // default is DVI mode;
    psHDMITXResPri->stHDMITxInfo.output_color = E_HAL_HDMITX_COLOR_RGB444;

    while (HalHdmitxUtilityEdidReadBlock(0x00, ablkData) == FALSE)
    {
        u8EDIDRetryCnt--;

        if (u8EDIDRetryCnt == 0)
        {
            memset(ablkData, 0x00, HDMITX_EDID_BLK_SIZE);
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "[HDMITx] %s %d, Check EDID fail!!\n", __FUNCTION__, __LINE__);
            break;
        }

        DrvHdmitxOsMsDelay(100);
    }
    if (HDMITX_EDID_DEBUG_CHECK())
    {
        u8 *pu8Str = NULL;
        u8 *pu8Buf = NULL;
        HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s %d, EDID ReTryCnt:%d\n", __FUNCTION__, __LINE__, u8EDIDRetryCnt);
        pu8Str = DrvHdmitxOsMemAlloc(512 * 8);

        if (pu8Str)
        {
            pu8Buf = pu8Str;
            HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s %d::block 0 ==>\n", __FUNCTION__, __LINE__);
            for (i = 0; i < HDMITX_EDID_BLK_SIZE; i++)
            {
                pu8Buf += HDMITX_SNPRINTF(pu8Buf, PAGE_SIZE, "0x%02X, ", ablkData[i]);
                if (((i + 1) % 16) == 0)
                {
                    pu8Buf += HDMITX_SNPRINTF(pu8Buf, PAGE_SIZE, "\n");
                }
            }
            HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s\n", pu8Str);
            DrvHdmitxOsMemRelease(pu8Str);
        }
    }

    // validiate EDID tag
    for (i = 0; i < sizeof(aEdidHeaderTag); i++)
    {
        if (ablkData[i] != aEdidHeaderTag[i])
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s::invalid EDID header!!\n", __FUNCTION__);
            if (!psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode) // AP force output mode
            {
                return FALSE;
            }
        }
    }

    // store block 0
    memcpy(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk0, ablkData, HDMITX_EDID_BLK_SIZE * sizeof(u8));

    // Get Manufacturer ID
    //****************************************
    //   EISA 3-character ID
    //   08h_b'[6..2]: first char
    //   08h_b'[1..0] & 09h_b'[7..5]: second char
    //   09h_b'[4..0]: third char
    //****************************************
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ManufacturerID[0] = (ablkData[8] & 0x7C) >> 2;
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ManufacturerID[1] =
        ((ablkData[8] & 0x03) << 3) | ((ablkData[9] & 0xE0) >> 5);
    psHDMITXResPri->stHDMITxInfo.RxEdidInfo.ManufacturerID[2] = ablkData[9] & 0x1F;

    // check if there are more Extended block
    u8ExtBlkNum = ablkData[0x7E];

    if (HDMITX_EDID_DEBUG_CHECK())
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s::Extension Block Number = %d\r\n", __FUNCTION__,u8ExtBlkNum);
        if (u8ExtBlkNum > 4)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s::Number of Extension Block > 4!!\n", __FUNCTION__);
        }
    }

    u8ValidBlkNum = (u8ExtBlkNum >= 4) ? 4 : u8ExtBlkNum;
    for (i = 1; i <= u8ValidBlkNum; i++)
    {
        u8EDIDRetryCnt = 3;
        while (HalHdmitxUtilityEdidReadBlock(i, ablkData) == FALSE)
        {
            u8EDIDRetryCnt--;

            if ((u8EDIDRetryCnt == 0) || (u8EDIDRetryCnt > 3))
            {
                // reset buf
                memset(ablkData, 0x00, HDMITX_EDID_BLK_SIZE);
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "[HDMITx]%s, Check EDID fail--2!!\n", __FUNCTION__);
                break;
            }
            DrvHdmitxOsMsDelay(100);
        }

        if (u8EDIDRetryCnt != 0)
        {
            // dump block content
            if (HDMITX_EDID_DEBUG_CHECK())
            {
                u8  j = 0;
                u8 *pu8Str;
                u8 *pu8Buf;

                HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s::Block #%d\r\n", __FUNCTION__, i);
                pu8Str = DrvHdmitxOsMemAlloc(512 * 8);
                if (pu8Str)
                {
                    pu8Buf = pu8Str;
                    for (j = 0; j < HDMITX_EDID_BLK_SIZE; j++)
                    {
                        pu8Buf += HDMITX_SNPRINTF(pu8Buf, PAGE_SIZE, "0x%02X, ", ablkData[i]);
                        if ((j + 1) % 16 == 0)
                        {
                            pu8Buf += HDMITX_SNPRINTF(pu8Buf, PAGE_SIZE, "\n");
                        }
                    }
                    HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s\n", pu8Str);
                    DrvHdmitxOsMemRelease(pu8Str);
                }
            }

            if (i == 1)
            {
                memcpy(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1, &ablkData, 128); // store block #1
            }

            if (!_HalHdmitxIfParseCEAExtEdid(pInstance, ablkData)) // invalid CEA Extension block
            {
                continue;
            }
        }
        else
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s::Access EDID fail @ Block 0x%02X!!\n", __FUNCTION__, i);
            if (psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode)
            {
                continue;
            }
        }
    }

    // TBD: if API layer force output mode
    if (psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode) // AP force output mode
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_EDID, "%s %d::ForceOututMode: %s!!\n", __FUNCTION__, __LINE__,
                   (psHDMITXResPri->stHDMITxInfo.force_output_mode & HDMITX_BIT1) ? "HDMI" : "DVI");
        psHDMITXResPri->stHDMITxInfo.output_mode = psHDMITXResPri->stHDMITxInfo.force_output_mode;
        if ((psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI)
            || (psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI_HDCP)) // DVI mode
        {
            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI = FALSE;
            psHDMITXResPri->stHDMITxInfo.output_color            = E_HAL_HDMITX_COLOR_RGB444;
        }
    }

    // AP force output color format
    if ((psHDMITXResPri->stHDMITxInfo.output_mode == E_HAL_HDMITX_OUTPUT_MODE_HDMI
         || psHDMITXResPri->stHDMITxInfo.output_mode == E_HAL_HDMITX_OUTPUT_MODE_HDMI_HDCP)
        && (psHDMITXResPri->stHDMITxInfo.hdmitx_force_output_color))
    {
        psHDMITXResPri->stHDMITxInfo.output_color = psHDMITXResPri->stHDMITxInfo.force_output_color;
    }

    psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready = TRUE;
    pInstance->bCheckEDID                          = FALSE;
    return TRUE;
}
static u8 _HalHdmitxIfCheckEdidRdy(DrvHdmitxCtxConfig_t *pInstance)
{
    u8  bEdidChk = 1;
    u32 u32Time  = DrvHdmitxOsGetSystemTime();
    while (1)
    {
        DrvHdmitxOsMsSleep(10);
        bEdidChk = _HalHdmitxIfEdidChecking(pInstance);

        if (bEdidChk || (DrvHdmitxOsGetSystemTime() - u32Time) > 200)
        {
            break;
        }
    }

    return bEdidChk;
}
static void _HalHdmitxIfSetVideoOutputMode(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxTimingResType_e enVidTiming)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stHDMITxInfo.output_video_timing = enVidTiming;
    psHDMITXResPri->stHDMITxInfo.output_aspect_ratio = HAL_HDMITX_SET_ASPECTRATIOFROMTIMING(enVidTiming);
    psHDMITXResPri->stHDMITxInfo.colorimetry         = HAL_HDMITX_SET_COLORMETRYFROMTIMING(enVidTiming);
    _HalHdmitxIfSetAVIInfoPktPara(pInstance);

    if (psHDMITXResPri->stHDMITxInfo.output_video_timing >= E_HAL_HDMITX_RES_MAX)
    {
        HDMITX_ERR("[%s][%d] Unsupport HDMI mode 0x%08x\n", __FUNCTION__, __LINE__,
                   psHDMITXResPri->stHDMITxInfo.output_video_timing);
        return;
    }
    HalHdmitxSetVideoOutputMode(psHDMITXResPri->u32DeviceID, enVidTiming, &psHDMITXResPri->stHDMITxInfo.analog_setting);
    HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_AVI, E_HDMITX_CYCLIC_PACKET);
}
static void _HalHdmitxIfSetHDMITxMode(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxOutpuModeType_e mode)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stHDMITxInfo.output_mode = mode;

    // HDMI / DVI

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode) // AP force output mode
    {
        psHDMITXResPri->stHDMITxInfo.output_mode = psHDMITXResPri->stHDMITxInfo.force_output_mode;
        if ((psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI)
            || (psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI_HDCP)) // DVI mode
        {
            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI = FALSE;
            psHDMITXResPri->stHDMITxInfo.output_color            = E_HAL_HDMITX_COLOR_RGB444;
        }
    }

    if (psHDMITXResPri->stHDMITxInfo.output_mode & HDMITX_BIT1) // HDMI
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag = TRUE;
        HalHdmitxSetHDMImode(psHDMITXResPri->u32DeviceID, TRUE, psHDMITXResPri->stHDMITxInfo.output_colordepth_val);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_GC, E_HDMITX_CYCLIC_PACKET);
        if (psHDMITXResPri->stInfoFrameCfg[E_HAL_HDMITX_INFOFRAM_TYPE_VS].bForceDisable == TRUE)
        {
            HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_VS, E_HDMITX_STOP_PACKET);
        }
        HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_SPD, E_HDMITX_CYCLIC_PACKET);
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s : HDMI mode = %d\r\n", __FUNCTION__,
                   psHDMITXResPri->stHDMITxInfo.output_mode);
    }
    else // DVI
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag = FALSE;
        HalHdmitxSetHDMImode(psHDMITXResPri->u32DeviceID, FALSE, psHDMITXResPri->stHDMITxInfo.output_colordepth_val);
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s : DVI mode = %d\r\n", __FUNCTION__,
                   psHDMITXResPri->stHDMITxInfo.output_mode);
    }
}
static u8 _HalHdmitxIfColorandRangeTransform(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxColorType_e incolor,
                                             HalHdmitxColorType_e outcolor, HalHdmitxYccQuantRange_e inange,
                                             HalHdmitxYccQuantRange_e outrange)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    u8                          bRet = FALSE;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }
    psHDMITXResPri->stHDMITxInfo.input_color     = incolor;
    psHDMITXResPri->stHDMITxInfo.output_color    = outcolor;
    psHDMITXResPri->stHDMITxInfo.input_range     = inange;
    psHDMITXResPri->stHDMITxInfo.output_range    = outrange;
    psHDMITXResPri->stHDMITxInfo.enYCCQuantRange = outrange;
    _HalHdmitxIfSetAVIInfoPktPara(pInstance);

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag == FALSE)
    {
        if (!HDMITX_CSC_SUPPORT_R2Y(psHDMITXResPri->u32DeviceID))
        {
            incolor  = E_HAL_HDMITX_COLOR_RGB444;
            outcolor = E_HAL_HDMITX_COLOR_RGB444;
        }
        else
        {
            incolor = E_HAL_HDMITX_COLOR_RGB444;
        }
    }

    if ((incolor == E_HAL_HDMITX_COLOR_RGB444) && (outcolor != E_HAL_HDMITX_COLOR_RGB444))
    {
        // Old chip limited by R2Y
        if (!HDMITX_CSC_SUPPORT_R2Y(psHDMITXResPri->u32DeviceID))
        {
            HDMITX_ERR("Can not support for R2Y\r\n");
            return FALSE;
        }
    }

    if (!psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi
        && (!psHDMITXResPri->stHDMITxInfo.hdmitx_force_output_color))
    {
        if (outcolor != E_HAL_HDMITX_COLOR_RGB444)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s DVI mode needs RGB only\r\n", __FUNCTION__);
            return FALSE;
        }
    }

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag == FALSE)
    {
        bRet = HalHdmitxColorandRangeTransform(psHDMITXResPri->u32DeviceID, incolor, outcolor,
                                               psHDMITXResPri->stHDMITxInfo.input_range,
                                               psHDMITXResPri->stHDMITxInfo.output_range);
    }
    else
    {
        bRet = HalHdmitxColorandRangeTransform(psHDMITXResPri->u32DeviceID, psHDMITXResPri->stHDMITxInfo.input_color,
                                               psHDMITXResPri->stHDMITxInfo.output_color,
                                               psHDMITXResPri->stHDMITxInfo.input_range,
                                               psHDMITXResPri->stHDMITxInfo.output_range);
    }

    if (inange != outrange)
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = TRUE;
    }
    else if (outcolor != incolor)
    {
        if (HDMITX_CSC_SUPPORT_R2Y(psHDMITXResPri->u32DeviceID))
        {
            if (outcolor == E_HAL_HDMITX_COLOR_RGB444) // Input Y -> Output R
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = TRUE;
            }
            else if (incolor != E_HAL_HDMITX_COLOR_RGB444) // Input Y -> Output Y
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = FALSE;
            }
            else // Input R -> Output Y
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = TRUE;
            }
        }
        else
        {
            if (outcolor == E_HAL_HDMITX_COLOR_RGB444) // Input Y -> Output R
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = TRUE;
            }
            else if (incolor != E_HAL_HDMITX_COLOR_RGB444) // Input Y -> Output Y
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = FALSE;
            }
            else // Input R -> Output Y
            {
                psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = FALSE;
            }
        }
    }
    else
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = FALSE;
    }

    HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_AVI, E_HDMITX_CYCLIC_PACKET);
    return bRet;
}
static void _HalHdmitxIfSetColorFormat(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxColorType_e enInColor,
                                       HalHdmitxColorType_e enOutColor)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s :B InColor = %x, OutColor = %x\r\n", __FUNCTION__, enInColor, enOutColor);
    _HalHdmitxIfColorandRangeTransform(pInstance, enInColor, enOutColor, psHDMITXResPri->stHDMITxInfo.input_range,
                                       psHDMITXResPri->stHDMITxInfo.output_range);
}
static void _HalHdmitxIfSetAudioFrequency(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxAudioFreqType_e enAudFreq)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stHDMITxInfo.output_audio_frequncy = enAudFreq;
    _HalHdmitxIfSetAudInfoPktPara(pInstance);

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag)
    {
        HalHdmitxSetAudioFrequency(psHDMITXResPri->u32DeviceID, psHDMITXResPri->stHDMITxInfo.output_audio_frequncy,
                                   psHDMITXResPri->stHDMITxInfo.output_audio_channel,
                                   psHDMITXResPri->stHDMITxInfo.output_audio_type,
                                   psHDMITXResPri->stHDMITxInfo.output_video_timing);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ACR, E_HDMITX_CYCLIC_PACKET);
        HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO, E_HDMITX_CYCLIC_PACKET);
    }
}
static void _HalHdmitxIfSetVideoOnOff(DrvHdmitxCtxConfig_t *pInstance, u8 state)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag = state;

    _HalHdmitxIfColorandRangeTransform(
        pInstance, psHDMITXResPri->stHDMITxInfo.input_color, psHDMITXResPri->stHDMITxInfo.output_color,
        psHDMITXResPri->stHDMITxInfo.input_range, psHDMITXResPri->stHDMITxInfo.output_range);
    HalHdmitxSetVideoOnOff(psHDMITXResPri->u32DeviceID, psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag,
                           psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag,
                           (psHDMITXResPri->stHDMITxInfo.colorimetry == E_HDMITX_COLORIMETRY_ITUR709) ? 1 : 0);
}

static void _HalHdmitxIfSetAudioOnOff(DrvHdmitxCtxConfig_t *pInstance, u8 state)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag = state;
    _HalHdmitxIfSetAudInfoPktPara(pInstance);

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag) // turn on
    {
        HalHdmitxSetAudioOnOff(psHDMITXResPri->u32DeviceID, TRUE);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ACR, E_HDMITX_CYCLIC_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_AS, E_HDMITX_CYCLIC_PACKET);
        HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO, E_HDMITX_CYCLIC_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ACP, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ISRC1, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ISRC2, E_HDMITX_STOP_PACKET);
    }

    else // turn off
    {
        HalHdmitxSetAudioOnOff(psHDMITXResPri->u32DeviceID, FALSE);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ACR, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_AS, E_HDMITX_STOP_PACKET);
        HalHdmitxSendInfoFrame(psHDMITXResPri, E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ACP, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ISRC1, E_HDMITX_STOP_PACKET);
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_ISRC2, E_HDMITX_STOP_PACKET);
    }
}
static void _HalHdmitxIfSetAVMUTE(DrvHdmitxCtxConfig_t *pInstance, u8 bflag)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    if (!psHDMITXResPri->stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_GC].bForceDisable)
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag = bflag;
        _HalHdmitxIfSetGCPParameter(pInstance); // update packet content; wilson@kano
        HalHdmitxSendPacket(psHDMITXResPri, E_HAL_HDMITX_PACKET_TYPE_GC, E_HDMITX_CYCLIC_PACKET);
        if (bflag)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s Set\n", __FUNCTION__);
        }
        else
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s Clear\n", __FUNCTION__);
        }
    }
}

void HalHdmiTxHandler(DrvHdmitxCtxConfig_t *pInstance, u32 u32State, u32 u32DevId) // wilson@kano
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    u32 *                       p32HpdOnTime;
    u32 *                       p32TransmitTime;
    u32 *                       p32ValidateEdidTime;
    u32 *                       pbHpdRestart;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_TASK, "%s:: GetHwCtx Fail\n",__FUNCTION__);
        return;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_TASK, "%s:: pre= %d, State = %d\n",__FUNCTION__,
                psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status, u32State);
    p32HpdOnTime        = &psHDMITXResPri->stHDMITxInfo.u32HpdOnTime;
    p32TransmitTime     = &psHDMITXResPri->stHDMITxInfo.u32TransmitTime;
    p32ValidateEdidTime = &psHDMITXResPri->stHDMITxInfo.u32ValidateEdidTime;
    pbHpdRestart        = &psHDMITXResPri->stHDMITxInfo.bHpdRestart;

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_bypass_flag)
    {
        // RXTXBypass
        HDMITX_DBG(HDMITX_DBG_LEVEL_TASK, "%s:: hdmitx_bypass_flag \n",__FUNCTION__);
        return;
    }

    if (u32State & (E_HAL_HDMITX_EVENT_RXTIMER | E_HAL_HDMITX_EVENT_HPD_ISR))
    {
        HalHdmitxRxStatus_e enCurRxStatus;

        if (u32State & E_HAL_HDMITX_EVENT_HPD_ISR)
        {
            enCurRxStatus                                    = HalHdmitxGetRXStatus(psHDMITXResPri->bSetHPD, u32DevId);
            psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status = enCurRxStatus;
        }
        else
        {
            enCurRxStatus = psHDMITXResPri->stHDMITxInfo.hdmitx_curRX_status;
        }

        if (psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status != enCurRxStatus)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "E_HDMITX_FSM_CHECK_HPD:: pre= %d, cur = %d\n",
                       psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status, enCurRxStatus);
            if ((enCurRxStatus == E_HDMITX_DVICLOCK_H_HPD_H) || (enCurRxStatus == E_HDMITX_DVICLOCK_L_HPD_H))
            {
                *pbHpdRestart = 1;
                *p32HpdOnTime = DrvHdmitxOsGetSystemTime();
                // update EDID when (1) HPD L -> H or (2) DVI clock L -> H with HPD always high
                pInstance->bCheckEDID = TRUE;
                HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_VALIDATE_EDID);
                psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready = FALSE;

                if (enCurRxStatus == E_HDMITX_DVICLOCK_H_HPD_H)
                {
                    psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_flag = TRUE;
                }

                // some monitor will reset HPD when TMDS is disabled too long.
                // enable TMDS immediate when HPD=1 to resolve it.
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, TMDS=%x\n", __FUNCTION__, __LINE__,
                           HalHdmitxGetTMDS(u32DevId));
                _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, TRUE);
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, TMDS=%x\n", __FUNCTION__, __LINE__,
                           HalHdmitxGetTMDS(u32DevId));
            }
            else
            {
                pInstance->bCheckEDID = FALSE;
                HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_PENDING);
                psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready = FALSE;
                _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, FALSE);
            }

            psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status = enCurRxStatus;

            DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RUN);
        }
        else // To avoid two events are coming together - HDCP CTS 1A-02
        {
            if (u32State & E_HAL_HDMITX_EVENT_RUN)
            {
                DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RUN);
            }
        }
    }
    else if (u32State & E_HAL_HDMITX_EVENT_RUN)
    {
        if ((psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status == E_HDMITX_DVICLOCK_H_HPD_H)
            || (psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status == E_HDMITX_DVICLOCK_L_HPD_H)) // rx attached
        {
            if (psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state != psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate)
            {
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "HDMI handler preState:curState = %d : %d\r\n",
                           psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate,
                           psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state);
            }
            psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate =
                psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state; // update state

            switch (psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state)
            {
                case E_HAL_HDMITX_FSM_PENDING: // idle state
                {
                    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate, E_HAL_HDMITX_FSM_PENDING);
                    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_PENDING);
                }
                break;

                case E_HAL_HDMITX_FSM_VALIDATE_EDID:
                {
                    psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate = E_HAL_HDMITX_FSM_VALIDATE_EDID;
                    psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready   = FALSE;

                    if (_HalHdmitxIfEdidChecking(pInstance))
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "\nMDrv_HDMITX_Process()::EDID OK\n");
                        psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready = TRUE;
                        _HalHdmitxIfSetVideoOutputMode(pInstance, psHDMITXResPri->stHDMITxInfo.output_video_timing);
                        HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_TRANSMIT);
                    }
                    else // invalid EDID
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "\nMDrv_HDMITX_Process()::EDID FAIL!!\n");
                        HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_PENDING);
                    }
                    *p32ValidateEdidTime = DrvHdmitxOsGetSystemTime();
                    DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RUN);
                }
                break;

                case E_HAL_HDMITX_FSM_TRANSMIT:
                {
                    psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate = E_HAL_HDMITX_FSM_TRANSMIT;

                    _HalHdmitxIfSetHDMITxMode(pInstance, psHDMITXResPri->stHDMITxInfo.output_mode);
                    _HalHdmitxIfSetColorFormat(pInstance, psHDMITXResPri->stHDMITxInfo.input_color,
                                               psHDMITXResPri->stHDMITxInfo.output_color);
                    _HalHdmitxIfSetAudioFrequency(pInstance, psHDMITXResPri->stHDMITxInfo.output_audio_frequncy);
                    _HalHdmitxIfSetVideoOnOff(pInstance, TRUE); // psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag =
                                                                // TRUE;
                    _HalHdmitxIfSetAudioOnOff(pInstance, TRUE);

                    if (psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag == TRUE)
                    {
                        _HalHdmitxIfSetAVMUTE(pInstance, FALSE);
                    }

                    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_DONE);
                    *p32TransmitTime = DrvHdmitxOsGetSystemTime();
                    DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RUN);
                }
                break;

                case E_HAL_HDMITX_FSM_DONE:
                {
                    if (*pbHpdRestart)
                    {
                        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF,
                                   "%s %d, HpdTIme=%d, EdidTime:%d, TransmitTime=%d, DoneTime=%d\n", __FUNCTION__,
                                   __LINE__, *p32HpdOnTime, *p32ValidateEdidTime - *p32HpdOnTime,
                                   *p32TransmitTime - *p32HpdOnTime, DrvHdmitxOsGetSystemTime() - *p32HpdOnTime);
                        *pbHpdRestart = 0;
                    }
                }
                break;

                default:
                    break;
            }
        }
    }
    else if (u32State & E_HAL_HDMITX_EVENT_IRQ)
    {
        HalHdmitxIrqGetStatus(u32DevId);
    }
}

#ifndef CONFIG_HDMITX_NONUSEGPIO
static int _HalHdmiTxTask(void *pCtx) // wilson@kano
{
    u32                         u32Events = 0;
    DrvHdmitxCtxConfig_t *      pInstance = pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "[%s] hdmi task %d Exec Runing\r\n", __FUNCTION__, psHDMITXResPri->u32DeviceID);
    while (psHDMITXResPri && psHDMITXResPri->bEnableHDMITxTask)
    {
        DrvHdmitxOsWaitEvent(pInstance->s32HDMITxEventId,
                             E_HAL_HDMITX_EVENT_RUN | E_HAL_HDMITX_EVENT_IRQ | E_HAL_HDMITX_EVENT_RXTIMER
                                 | E_HAL_HDMITX_EVENT_HPD_ISR | E_HAL_HDMITX_EVENT_STOPTASK | E_HAL_HDMITX_EVENT_CECRX,
                             &u32Events, E_DRV_HDMITX_OS_EVENT_MD_OR_CLEAR, HDMITX_OS_WAIT_FOREVER);
        if (DrvHdmitxOsTaskShouldStop() == CAM_OS_OK)
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF,"[%s] DrvHdmitxOsTaskShouldStop\n", __FUNCTION__);
            break;
        }
        if ((u32Events & E_HAL_HDMITX_EVENT_STOPTASK))
        {
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF,"[%s] E_HAL_HDMITX_EVENT_STOPTASK\n", __FUNCTION__);
        }
        else if (u32Events)
        {
            HAL_HDMITX_CTX_LOCK_SEM(&pInstance->stSem);
            HalHdmiTxHandler(pInstance, u32Events, psHDMITXResPri->u32DeviceID);
            HAL_HDMITX_CTX_UNLOCK_SEM(&pInstance->stSem);
        }
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "[%s] hdmi task %d Exec Ending\r\n", __FUNCTION__, psHDMITXResPri->u32DeviceID);
    return 0;
}
#endif
#if HDMITX_ISR_ENABLE
static irqreturn_t HalHdmitxIfIsr(u32 u32IntNum, void *dev_id)
{
    u32                         irq_status;
    DrvHdmitxCtxConfig_t *      pInstance = dev_id;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return IRQ_HANDLED;
    }
    u32IntNum = u32IntNum; // prevent compile warning

    // Clear Interrupt Bit
    irq_status = HalHdmitxIrqGetStatus(pInstance);
    if (irq_status & E_HDMITX_IRQ_12) // TMDS hot-plug
    {
        HalHdmitxIrqClear(E_HDMITX_IRQ_12, psHDMITXResPri->u32DeviceID);
    }
    else if (irq_status & E_HDMITX_IRQ_10) // Rx disconnection
    {
        HalHdmitxIrqClear(E_HDMITX_IRQ_10, psHDMITXResPri->u32DeviceID);
    }

    DrvHdmitxOsSetEvent(_s32HDMITxEventId, E_HAL_HDMITX_EVENT_IRQ);
    return IRQ_HANDLED;
}
#endif
u32 HalHdmitxifHPDIsr(u32 eIntNum, void *pstDevParam)
{
    // printf("%s %d, HPD=%d\n", __FUNCTION__, __LINE__, DrvHdmitxOsGetGpioValue( DrvHdmitxOsGetHpdGpinPin() ));
    DrvHdmitxCtxConfig_t *pInstance = pstDevParam;

    DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_HPD_ISR);

    return 1;
}
#ifndef CONFIG_HDMITX_NONUSEGPIO
static void _HalHdmitxIfCheckRxTimerCallback(u32 stTimer, void *pData)
{
    DrvHdmitxCtxConfig_t *      pInstance = pData;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_TASK, "%s:: GetHwCtx Fail\n",__FUNCTION__);
        return;
    }
    // Check HDMI receiver status
    psHDMITXResPri->stHDMITxInfo.hdmitx_curRX_status =
        HalHdmitxGetRXStatus(psHDMITXResPri->bSetHPD, psHDMITXResPri->u32DeviceID);
    HDMITX_DBG(HDMITX_DBG_LEVEL_TASK, "%s:: pre= %d, cur = %d ,bSetHPD = %d g_bHpdIrqEn=%hhd\n",__FUNCTION__,
                psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status, psHDMITXResPri->stHDMITxInfo.hdmitx_curRX_status, psHDMITXResPri->bSetHPD,g_bHpdIrqEn);
    if (g_bHpdIrqEn)
    {
        if (psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status == E_HDMITX_DVICLOCK_L_HPD_L
            || psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status == E_HDMITX_DVICLOCK_H_HPD_L)
        {
            DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RXTIMER);
        }
    }
    else
    {
        DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_RXTIMER);
    }
}
#endif

static u8 _HalHdmitxIfDeviceInit(DrvHdmitxCtxConfig_t *pInstance, u32 u32DevId)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    u8                          u8TaskName[16];

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }
    psHDMITXResPri                           = pInstance->pstHwCtx;
    pInstance->_hdmitx_status.bIsInitialized = TRUE;
    pInstance->_hdmitx_status.bIsRunning     = TRUE;
    pInstance->bCheckEDID                    = g_u16I2cUsed;
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d DevId:%d\n", __FUNCTION__, __LINE__, u32DevId);

    if (!DrvHdmitxOsGetMmioBase(&(psHDMITXResPri->stHDMITxInfo.pvPMRIUBaseAddress),
                                &(psHDMITXResPri->stHDMITxInfo.u32PMBankSize), E_HDMITX_OS_MMIO_PM))
    {
        HDMITX_ERR("MApi_XC_Init Get PM BASE failure\n");
        return FALSE;
    }

    if (DrvHdmitxOsGetMmioBase(&(psHDMITXResPri->stHDMITxInfo.pvCoproRIUBase),
                               &(psHDMITXResPri->stHDMITxInfo.u32PMBankSize), E_HDMITX_OS_MMIO_NONEPM))
    {
        HalHdmitxUtilitySetIOMapBase(psHDMITXResPri->stHDMITxInfo.pvPMRIUBaseAddress,
                                     psHDMITXResPri->stHDMITxInfo.pvPMRIUBaseAddress,
                                     psHDMITXResPri->stHDMITxInfo.pvCoproRIUBase);
    }
    else
    {
        HDMITX_ERR("_COPRO_Init: IOMap failure\n");
        return FALSE;
    }

    HalHdmitxPowerOnOff(TRUE, u32DevId);
    HalHdmitxDeviceOnOff(0, u32DevId);
    if (psHDMITXResPri->bResPriInit == TRUE)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s:%d bInit already initialized.\n", __FUNCTION__, __LINE__);
        return TRUE;
    }

    if (psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated) // if (_s32HDMITxTaskId >= 0)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s:%d bHDMITxTaskIdCreated already initialized.\n", __FUNCTION__,
                   __LINE__);
        return TRUE;
    }

#if HDMITX_ISR_ENABLE
    // Todo:paul need to add getid function.
    DrvHdmitxOsDisableInterrupt(E_INT_IRQ_HDMI_LEVEL);
    DrvHdmitxOsDetachInterrupt(E_INT_IRQ_HDMI_LEVEL);

    HalHdmitxIrqDisable(HAL_HDMITX_IRQ_EN_FLAG, u32DevId);
    HalHdmitxIrqClear(HAL_HDMITX_IRQ_EN_FLAG, u32DevId);
#endif // #if HDMITX_ISR_ENABLE

    HalHdmitxInitSeq(u32DevId);
    // Set DDC speed to 50KHz as default
    HalHdmitxUtilityAdjustDDCFreq(100); // real speed is 80%
    HalHdmitxInitPadTop(u32DevId);
    HDMITX_SNPRINTF(u8TaskName, 16,"HDMITx_Task_%u", u32DevId);
#ifndef CONFIG_HDMITX_NONUSEGPIO
    if (psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated == FALSE)
    {
        pInstance->s32HDMITxEventId                        = DrvHdmitxOsCreateEventGroup((u8 *)"HDMITx_Event");
        psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated = TRUE;
    }

    if (pInstance->s32HDMITxEventId < 0)
    {
        HDMITX_ERR("%s %d, CreateEvent Fail\n", __FUNCTION__, __LINE__);
        psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated = FALSE;
        return FALSE;
    }
    if (psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated == FALSE)
    {
        psHDMITXResPri->bEnableHDMITxTask = 1;
        if (DrvHdmitxOsCreateTask(&pInstance->stHDMITxTask, (TASK_ENTRY_CB)_HalHdmiTxTask, pInstance, u8TaskName, TRUE))
        {
            pInstance->s32HDMITxTaskId                        = pInstance->stHDMITxTask.s32Id;
            psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated = TRUE;
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "create hdmi task Done\r\n");
        }
    }

    if (pInstance->s32HDMITxTaskId >= 0)
    {
        psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated = TRUE;
    }
    else
    {
        HDMITX_ERR("create hdmi task failed\r\n");
        DrvHdmitxOsDeleteEventGroup(pInstance->s32HDMITxEventId);
        psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated  = FALSE;
        psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated = FALSE;
        return FALSE;
    }
#else
    return FALSE;
#endif

#if HDMITX_ISR_ENABLE
    sprintf(u8TaskName, "_HDMITx_Isr_%u", u32DevId);
    if (CamOsIrqRequest(E_INT_IRQ_HDMI_LEVEL, (CamOsIrqHandler)HalHdmitxIfIsr, u8TaskName, (void *)pInstance))
    {
        HDMITX_ERR("%s(%d):: Request IRQ Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }
    DrvHdmitxOsEnableInterrupt(E_INT_IRQ_HDMI_LEVEL); // Enable TSP interrupt
#endif                                                // #if HDMITX_ISR_ENABLE

    if (HalHdmitxUtilityGetSwI2cEn() == FALSE)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, HW I2c\n", __FUNCTION__, __LINE__);
        if (DrvHdmitxOsSetI2cAdapter(DrvHdmitxOsGetI2cId()) == 0) // ToDo bus number ??
        {
            HDMITX_ERR("%s %d, I2C Adapter fail\n", __FUNCTION__, __LINE__);
            return FALSE;
        }
    }
    else
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, SW I2c\n", __FUNCTION__, __LINE__);
        DrvHdmitxOsGpioRequestOutput(HalHdmitxUtilityGetSwI2cSdaPin());
        DrvHdmitxOsGpioRequestOutput(HalHdmitxUtilityGetSwI2cSclPin());
        DrvHdmitxOsSetGpioValue(HalHdmitxUtilityGetSwI2cSdaPin(), 1); // keep sda to high
        DrvHdmitxOsSetGpioValue(HalHdmitxUtilityGetSwI2cSclPin(), 1); // keep scl to high
    }

    return TRUE;
}
static u8 _HalHdmitxIfOpenDevice(HalHdmitxResourcePrivate_t *psHDMITXResPri, u32 u32DevId)
{
    u8 bRet = TRUE;

    if (psHDMITXResPri && psHDMITXResPri->bResPriUsed == 0)
    {
        psHDMITXResPri->u32DeviceID = u32DevId;
        psHDMITXResPri->bResPriUsed       = 1;
        psHDMITXResPri->bSetHPD     = g_stForceAttrCfg.bHpdDetect;
        memset(&psHDMITXResPri->stHDMITxInfo, 0, sizeof(HalHdmitxParameterList_t));
    }
    else
    {
        HDMITX_ERR("%s %d, HDMITx %u Alrad Open\n", __FUNCTION__, __LINE__, u32DevId);
    }
    return bRet;
}
static u8 _HalHdmitxIfInit(DrvHdmitxCtxConfig_t *pstHdmitxCtx, u32 u32DevId)
{
    u8                          bReturn        = 0;
    HalHdmitxResourcePrivate_t *psHDMITXResPri = NULL;

    if (!_HalHdmitxIfGetHwCtx(pstHdmitxCtx, &psHDMITXResPri))
    {
        return bReturn;
    }
    if (psHDMITXResPri->bResPriUsed == 0)
    {
        if (_HalHdmitxIfOpenDevice(psHDMITXResPri, u32DevId) != TRUE)
        {
            HDMITX_ERR("%s Open HDMITX failed\n", __FUNCTION__);
            return FALSE;
        }
    }
    if (psHDMITXResPri->bResPriInit == 0)
    {
        _HalHdmitxIfSwInit(pstHdmitxCtx, u32DevId);
        bReturn               = _HalHdmitxIfDeviceInit(pstHdmitxCtx, u32DevId);
        psHDMITXResPri->bResPriInit = bReturn;
    }
    return bReturn;
}
static u8 _HalHdmitxIfExit(DrvHdmitxCtxConfig_t *pInstance, u32 u32DevId)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, deInit Dev:%d\n", __FUNCTION__, __LINE__, u32DevId);
    pInstance->bCheckEDID = g_u16I2cUsed;
    HalHdmitxDeviceOnOff(TRUE, u32DevId);
    HalHdmitxPowerOnOff(FALSE, u32DevId);
    psHDMITXResPri->bResPriInit = FALSE;
    psHDMITXResPri->bResPriUsed = FALSE;
    if (psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated)
    {
        DrvHdmitxOsStopTimer(&pInstance->stCheckRxTimerCfg);
        psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated = 0;
        pInstance->stCheckRxTimerCfg.s32Id                  = -1;
    }
    if (psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated)
    {
        psHDMITXResPri->bEnableHDMITxTask = 0;
        DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_STOPTASK);
        // because of destory task will wait task done, but it maybe into semphore.
        HAL_HDMITX_CTX_UNLOCK_SEM(&pInstance->stSem);
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, DestroyTask Dev:%d\n", __FUNCTION__, __LINE__, u32DevId);
        if (DrvHdmitxOsDestroyTask(&pInstance->stHDMITxTask))
        {
            pInstance->s32HDMITxTaskId                        = -1;
            psHDMITXResPri->stHDMITxInfo.bHDMITxTaskIdCreated = 0;
        }
        HAL_HDMITX_CTX_LOCK_SEM(&pInstance->stSem);
    }
    if (psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated)
    {
        if (DrvHdmitxOsDeleteEventGroup(pInstance->s32HDMITxEventId))
        {
            pInstance->s32HDMITxEventId                        = -1;
            psHDMITXResPri->stHDMITxInfo.bHDMITxEventIdCreated = FALSE;
        }
    }
#if HDMITX_ISR_ENABLE
    DrvHdmitxOsDisableInterrupt(E_INT_IRQ_HDMI_LEVEL); // Enable TSP interrupt
    CamOsIrqFree(E_INT_IRQ_HDMI_LEVEL, (void *)pInstance);
#endif // #if HDMITX_ISR_ENABLE
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, deInit Done:%d\n", __FUNCTION__, __LINE__, u32DevId);
    return TRUE;
}
static void _HalHdmitxIfTurnOnOff(DrvHdmitxCtxConfig_t *pInstance, u8 state)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stHDMITxInfo.hdmitx_enable_flag = state;
    psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_flag   = state;

    if (state) // turn on
    {
#if HDMITX_ISR_ENABLE
        HalHdmitxIrqEnable(HAL_HDMITX_IRQ_EN_FLAG, psHDMITXResPri->u32DeviceID);
#endif
    }
    else // turn off
    {
#if HDMITX_ISR_ENABLE
        HalHdmitxIrqDisable(HAL_HDMITX_IRQ_EN_FLAG, psHDMITXResPri->u32DeviceID);
        HalHdmitxIrqClear(HAL_HDMITX_IRQ_EN_FLAG, psHDMITXResPri->u32DeviceID);
#endif
        _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, state);
        HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_prestate, E_HAL_HDMITX_FSM_PENDING);
        HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_PENDING);
        psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status = E_HDMITX_DVICLOCK_L_HPD_L;

        if (psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated)
        {
            DrvHdmitxOsStopTimer(&pInstance->stCheckRxTimerCfg);
        }
    }
}
static void _HalHdmitxIfForceHDMIOutputMode(DrvHdmitxCtxConfig_t *pInstance, u8 bflag,
                                            HalHdmitxOutpuModeType_e output_mode)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stHDMITxInfo.hdmitx_force_mode = bflag;
    if (bflag)
    {
        psHDMITXResPri->stHDMITxInfo.force_output_mode = output_mode;
        psHDMITXResPri->stHDMITxInfo.output_mode       = psHDMITXResPri->stHDMITxInfo.force_output_mode;
        if ((psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI)
            || (psHDMITXResPri->stHDMITxInfo.force_output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI_HDCP)) // DVI mode
        {
            // gHDMITxInfo.hdmitx_audio_supportAI = FALSE;
            psHDMITXResPri->stHDMITxInfo.RxEdidInfo.AudSupportAI = FALSE;
            psHDMITXResPri->stHDMITxInfo.output_color            = E_HAL_HDMITX_COLOR_RGB444;
            _HalHdmitxIfSetAVIInfoPktPara(pInstance);
        }
    }
}
static u8 _HalHdmitxIfForceHDMIOutputColorFormat(DrvHdmitxCtxConfig_t *pInstance, u8 bflag,
                                                 HalHdmitxColorType_e output_color)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    if (bflag)
    {
        if ((output_color != E_HAL_HDMITX_COLOR_RGB444)
            && (psHDMITXResPri->stHDMITxInfo.output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI
                || psHDMITXResPri->stHDMITxInfo.output_mode == E_HAL_HDMITX_OUTPUT_MODE_DVI_HDCP))
        {
            HDMITX_ERR("Set force output color format failed!\n");
            return FALSE;
        }

        psHDMITXResPri->stHDMITxInfo.hdmitx_force_output_color = 1;
        psHDMITXResPri->stHDMITxInfo.force_output_color        = output_color;
        psHDMITXResPri->stHDMITxInfo.output_color              = psHDMITXResPri->stHDMITxInfo.force_output_color;

        if ((psHDMITXResPri->stHDMITxInfo.input_color != psHDMITXResPri->stHDMITxInfo.output_color)
            && (psHDMITXResPri->stHDMITxInfo.output_color == E_HAL_HDMITX_COLOR_RGB444))
        {
            psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = TRUE;
        }
        else // bypass
        {
            psHDMITXResPri->stHDMITxInfo.hdmitx_csc_flag = FALSE;
        }

        _HalHdmitxIfSetAVIInfoPktPara(pInstance);
    }
    else
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_force_output_color = 0;
    }
    return TRUE;
}
static void _HalHdmitxIfSetDeepColorMode(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxColorDepthType_e enDeepColorMode)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    psHDMITXResPri->stHDMITxInfo.output_colordepth_val = enDeepColorMode;
    _HalHdmitxIfSetGCPParameter(pInstance);
}
static void _HalHdmitxIfExhibit(DrvHdmitxCtxConfig_t *pInstance)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
#ifndef CONFIG_HDMITX_NONUSEGPIO
    if (((psHDMITXResPri->stHDMITxInfo.output_color != E_HAL_HDMITX_COLOR_YUV420)
         && (psHDMITXResPri->stHDMITxInfo.output_color != E_HAL_HDMITX_COLOR_YUV422))
        && ((psHDMITXResPri->stHDMITxInfo.output_video_timing == E_HAL_HDMITX_RES_3840X2160P_50HZ)
            || (psHDMITXResPri->stHDMITxInfo.output_video_timing == E_HAL_HDMITX_RES_3840X2160P_60HZ)))
    {
        psHDMITXResPri->stHDMITxInfo.output_colordepth_val = E_HAL_HDMITX_CD_24_BITS;
    }

    _HalHdmitxIfSetDeepColorMode(pInstance, psHDMITXResPri->stHDMITxInfo.output_colordepth_val);
    _HalHdmitxIfSetColorFormat(pInstance, psHDMITXResPri->stHDMITxInfo.input_color,
                               psHDMITXResPri->stHDMITxInfo.output_color);
    _HalHdmitxIfSetHDMITxMode(pInstance, psHDMITXResPri->stHDMITxInfo.output_mode);
    _HalHdmitxIfSetVideoOutputMode(pInstance, psHDMITXResPri->stHDMITxInfo.output_video_timing);
    _HalHdmitxIfSetAudioFrequency(pInstance, psHDMITXResPri->stHDMITxInfo.output_audio_frequncy);

    // The 1st HDCP authentication
    if (!HDMITX_FSM_IS_DONE(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state))
    {
        if ((!psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready)
            || (psHDMITXResPri->stHDMITxInfo.hdmitx_preRX_status != E_HDMITX_DVICLOCK_H_HPD_H))
        {
            HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_VALIDATE_EDID);
        }

        psHDMITXResPri->stHDMITxInfo.output_video_prevtiming = psHDMITXResPri->stHDMITxInfo.output_video_timing;

        // Create timer for timing monitor
        if (!psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated)
        {
            if (g_bHpdIrqEn)
            {
                DrvHdmitxOsGpioIrq(DrvHdmitxOsGetHpdGpinPin(), (INTERRUPT_CB)HalHdmitxifHPDIsr, 1, pInstance);
            }
            pInstance->stCheckRxTimerCfg.s32Id       = -1;
            pInstance->stCheckRxTimerCfg.pCtx        = pInstance;
            pInstance->stCheckRxTimerCfg.u32TimerOut = HDMITX_CHECK_RX_TIMER;
            pInstance->stCheckRxTimerCfg.pfnFunc     = _HalHdmitxIfCheckRxTimerCallback;
            if (DrvHdmitxOsCreateTimer(&pInstance->stCheckRxTimerCfg))
            {
                psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated = TRUE;
            }

            if (!psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated)
            {
                HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s: Create Check Rx timer fail!\n", __FUNCTION__);
                psHDMITXResPri->stHDMITxInfo.bCheckRxTimerIdCreated = FALSE;
            }

            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s: Create Check Rx timer success!\n", __FUNCTION__);
        }
        else
        {
            DrvHdmitxOsStopTimer(&pInstance->stCheckRxTimerCfg);
            DrvHdmitxOsStartTimer(&pInstance->stCheckRxTimerCfg);

            if (g_bHpdIrqEn)
            {
                DrvHdmitxOsGpioIrq(DrvHdmitxOsGetHpdGpinPin(), (INTERRUPT_CB)HalHdmitxifHPDIsr, 0, pInstance);
                DrvHdmitxOsGpioIrq(DrvHdmitxOsGetHpdGpinPin(), (INTERRUPT_CB)HalHdmitxifHPDIsr, 1, pInstance);

                if (psHDMITXResPri->stHDMITxInfo.hdmitx_enable_flag)
                {
                    // To trig _Hdmitx_Task when don turnoff and the turn on, but no hpd interrupt
                    DrvHdmitxOsSetEvent(pInstance->s32HDMITxEventId, E_HAL_HDMITX_EVENT_HPD_ISR);
                }
            }
        }
        // return;
    }

    if (HDMITX_FSM_IS_DONE(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state))
    {
        _HalHdmitxIfSetVideoOnOff(pInstance, psHDMITXResPri->stHDMITxInfo.hdmitx_video_flag);
        _HalHdmitxIfSetAudioOnOff(pInstance, psHDMITXResPri->stHDMITxInfo.hdmitx_audio_flag);
        if (psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag)
        {
            _HalHdmitxIfSetAVMUTE(pInstance, FALSE);
            psHDMITXResPri->stHDMITxInfo.hdmitx_avmute_flag = FALSE;
        }
    }
#else
    HDMITX_FSM_SET(psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state, E_HAL_HDMITX_FSM_DONE);
#endif
}
static u8 _HalHdmitxIfGetEDIDData(DrvHdmitxCtxConfig_t *pInstance, u8 *pu8Buffer, u8 BlockIdx)
{
    u8                          bRet = FALSE;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready)
    {
        if (BlockIdx)
        {
            memcpy(pu8Buffer, psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1, 128 * sizeof(u8));
        }
        else
        {
            memcpy(pu8Buffer, psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk0, 128 * sizeof(u8));
        }
        bRet = TRUE;
    }
    else
    {
        memset(pu8Buffer, 0x00, 128 * sizeof(u8));
        bRet = FALSE;
    }

    return bRet;
}
static u8 _HalHdmitxIfGetColorFormatFromEDID(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxTimingResType_e timing,
                                             u32 *pColorFmt)
{
    u8                          bRet = FALSE;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    u8                          i         = 0;
    HalHdmitxAviVic_e           enVICCode = (0x01 << E_HDMITX_VIC_NOT_AVAILABLE);

    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return 0;
    }

    if (psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready)
    {
        enVICCode  = HalHdmitxMapVideoTimingToVIC(timing);
        *pColorFmt = (0x1 << E_HAL_HDMITX_COLOR_RGB444);
        if ((psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1[0] == 0x02)
            && (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1[1] <= 0x04))
        {
            // YCbCr 420 only
            if (enVICCode != E_HDMITX_VIC_NOT_AVAILABLE)
            {
                for (i = 0; i < 32; i++)
                {
                    if (enVICCode == psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420VidDataBlk[i])
                    {
                        *pColorFmt = (0x1 << E_HAL_HDMITX_COLOR_YUV420);
                        return TRUE;
                    }
                }
            }
            *pColorFmt |= (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1[0x03] & HDMITX_EDID_CHECK_422)
                              ? (0x1 << E_HAL_HDMITX_COLOR_YUV422)
                              : 0;
            *pColorFmt |= (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.EdidBlk1[0x03] & HDMITX_EDID_CHECK_444)
                              ? (0x1 << E_HAL_HDMITX_COLOR_YUV444)
                              : 0;

            for (i = 0; i < sizeof(psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoDataBlk); i++)
            {
                if (enVICCode != E_HDMITX_VIC_NOT_AVAILABLE
                    && enVICCode == psHDMITXResPri->stHDMITxInfo.RxEdidInfo.VideoDataBlk[i])
                {
                    if (psHDMITXResPri->stHDMITxInfo.RxEdidInfo.YCbCr420CapMapDataBlk[i >> 3] & (0x01 << (i % 8)))
                    {
                        *pColorFmt |= (0x1 << E_HAL_HDMITX_COLOR_YUV420);
                    }
                }
            }
        }
        bRet = TRUE;
    }
    else
    {
        *pColorFmt = (0x1 << E_HAL_HDMITX_COLOR_RGB444);
        bRet       = FALSE;
    }
    return bRet;
}
static void _HalHdmitxIfInfoFrameUserDefine(DrvHdmitxCtxConfig_t *pInstance, HalHdmitxInfoFrameType_e packet_type,
                                            u8 def_flag, HalHdmitxPacketProcess_e def_process, u8 def_fcnt)
{
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }
    psHDMITXResPri->stInfoFrameCfg[packet_type].Define_FCnt    = def_fcnt;
    psHDMITXResPri->stInfoFrameCfg[packet_type].Define_Process = def_process;
    psHDMITXResPri->stInfoFrameCfg[packet_type].User_Define    = def_flag;

    if ((packet_type == E_HAL_HDMITX_INFOFRAM_TYPE_VS))
    {
        if (def_process == E_HDMITX_CYCLIC_PACKET)
        {
            psHDMITXResPri->stInfoFrameCfg[packet_type].bForceDisable = FALSE;
        }
        else
        {
            psHDMITXResPri->stInfoFrameCfg[packet_type].bForceDisable = TRUE;
        }
    }

    if (pInstance->_hdmitx_status.bIsInitialized) // fix code dump
    {
        HalHdmitxSendInfoFrame(psHDMITXResPri, packet_type, def_process);
    }
}

//------------------------------------------------------------------------------
// Init
//------------------------------------------------------------------------------

static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoInitConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;

    pstHalCtx->bInit       = 1;
    pstHalCtx->bVideoOnOff = 0;
    pstHalCtx->bAudioOnOff = 0;
    pstHalCtx->bSignal     = 0;
    pstHalCtx->bAvMute     = 1;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Init:%d, Video:%d, Audio:%d, AvMute:%d, Singal:%d\n", __FUNCTION__,
               __LINE__, pstHalCtx->bInit, pstHalCtx->bVideoOnOff, pstHalCtx->bAudioOnOff, pstHalCtx->bAvMute,
               pstHalCtx->bSignal);

    return enRet;
}

static void _HalHdmitxIfSetInitConfig(void *pCtx, void *pCfg)
{
    DrvHdmitxCtxConfig_t *      pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *  pstHalCtx    = pstHdmitxCtx->pstMhalCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pstHdmitxCtx, &psHDMITXResPri))
    {
        return;
    }
    if (_HalHdmitxIfInit(pstHdmitxCtx, pstHdmitxCtx->s32HdmitxId))
    {
        _HalHdmitxIfSetVideoOnOff(pstHdmitxCtx, pstHalCtx->bVideoOnOff);
        _HalHdmitxIfSetAudioOnOff(pstHdmitxCtx, pstHalCtx->bAudioOnOff);
        _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, pstHalCtx->bSignal);
        _HalHdmitxIfSetAVMUTE(pstHdmitxCtx, pstHalCtx->bAvMute);
    }
}

//------------------------------------------------------------------------------
// DeInit
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoDeInitConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e   enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;

    pstHdmitxCtx->pstMhalCtx->bInit = 0;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Init:%d\n", __FUNCTION__, __LINE__, pstHdmitxCtx->pstMhalCtx->bInit);

    return enRet;
}

static void _HalHdmitxIfSetDeInitConfig(void *pCtx, void *pCfg)
{
    DrvHdmitxCtxConfig_t *      pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pstHdmitxCtx, &psHDMITXResPri))
    {
        return;
    }
    _HalHdmitxIfExit(pstHdmitxCtx, psHDMITXResPri->u32DeviceID);
}

//------------------------------------------------------------------------------
// AttrBegin
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoAttrBeginConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Attr Begin, %x\n", __FUNCTION__, __LINE__, pstHalCtx->enStatusFlag);

    pstHalCtx->enStatusFlag |= E_HAL_HDMITX_STATUS_FLAG_ATTR_BEGIN;
    pstHalCtx->enStatusFlag &= ~E_HAL_HDMITX_STATUS_FLAG_ATTR_END;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Attr Begin, %x\n", __FUNCTION__, __LINE__, pstHalCtx->enStatusFlag);
    return enRet;
}

static void _HalHdmitxIfSetAttrBeingConfig(void *pCtx, void *pCfg)
{
    _HalHdmitxIfTurnOnOff(pCtx, 0);
}

//------------------------------------------------------------------------------
// Attr
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoAttrConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;
    HalHdmitxAttrConfig_t *   pstAttrCfg   = (HalHdmitxAttrConfig_t *)pCfg;

    memcpy(&pstHalCtx->stAttrCfg, pstAttrCfg, sizeof(HalHdmitxAttrConfig_t));

    HDMITX_DBG(
        HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Video(%d, %s %s %s %s %s), Audio(%d, %s %s %s %s)\n", __FUNCTION__, __LINE__,
        pstHalCtx->stAttrCfg.bVideoEn, PARSING_HAL_OUTPUT_MODE(pstHalCtx->stAttrCfg.enOutputMode),
        PARSING_HAL_COLOR_DEPTH(pstHalCtx->stAttrCfg.enColorDepth),
        PARSING_HAL_COLOR_FMT(pstHalCtx->stAttrCfg.enInColor), PARSING_HAL_COLOR_FMT(pstHalCtx->stAttrCfg.enOutColor),
        PARSING_HAL_TIMING(pstHalCtx->stAttrCfg.enTiming), pstHalCtx->stAttrCfg.bAudioEn,
        PARSING_HAL_AUDIO_FREQ(pstHalCtx->stAttrCfg.enAudioFreq), PARSING_HAL_AUDIO_CH(pstHalCtx->stAttrCfg.enAudioCh),
        PARSING_HAL_AUDIO_CODING(pstHalCtx->stAttrCfg.enAudioCode),
        PARSING_HAL_AUDIO_FMT(pstHalCtx->stAttrCfg.enAudioFmt));
    return enRet;
}

static void _HalHdmitxIfSetAttrConfig(void *pCtx, void *pCfg)
{
    HalHdmitxAttrConfig_t *     pstAttrCfg = (HalHdmitxAttrConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pInstance  = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    if (pstAttrCfg->bAudioEn)
    {
        psHDMITXResPri->stHDMITxInfo.output_audio_channel  = pstAttrCfg->enAudioCh;
        psHDMITXResPri->stHDMITxInfo.output_audio_type     = pstAttrCfg->enAudioCode;
        psHDMITXResPri->stHDMITxInfo.output_audio_frequncy = pstAttrCfg->enAudioFreq;
        _HalHdmitxIfSetAudioFrequency(pInstance, pstAttrCfg->enAudioFreq);
    }

    if (pstAttrCfg->bVideoEn)
    {
        _HalHdmitxIfSetHDMITxMode(pInstance, pstAttrCfg->enOutputMode);
        _HalHdmitxIfSetColorFormat(pInstance, pstAttrCfg->enInColor, pstAttrCfg->enOutColor);
        _HalHdmitxIfForceHDMIOutputColorFormat(pInstance, (pstAttrCfg->enOutColor == E_HAL_HDMITX_COLOR_AUTO) ? 0 : 1,
                                               pstAttrCfg->enOutColor);
        _HalHdmitxIfSetDeepColorMode(pInstance, pstAttrCfg->enColorDepth);
        _HalHdmitxIfSetVideoOutputMode(pCtx, pstAttrCfg->enTiming);
    }
}

//------------------------------------------------------------------------------
// AttrEnd
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoAttrEndConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Attr End, %x\n", __FUNCTION__, __LINE__, pstHalCtx->enStatusFlag);

    if (pstHalCtx->enStatusFlag & E_HAL_HDMITX_STATUS_FLAG_ATTR_BEGIN)
    {
        pstHalCtx->enStatusFlag |= E_HAL_HDMITX_STATUS_FLAG_ATTR_END;
        pstHalCtx->enStatusFlag &= ~E_HAL_HDMITX_STATUS_FLAG_ATTR_BEGIN;
    }
    else
    {
        enRet = E_HAL_HDMITX_QUERY_RET_ERR;
        pstHalCtx->enStatusFlag |= E_HAL_HDMITX_STATUS_FLAG_ATTR_END;
        HDMITX_ERR("%s %d, No Begin\n", __FUNCTION__, __LINE__);
    }

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Attr End, %x\n", __FUNCTION__, __LINE__, pstHalCtx->enStatusFlag);

    return enRet;
}

static void _HalHdmitxIfSetAttrEndConfig(void *pCtx, void *pCfg)
{
    volatile HalHdmitxFsmType_e enStatus;
    u32                         u32Time1, u32Time2;
    DrvHdmitxCtxConfig_t *      pInstance = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    _HalHdmitxIfTurnOnOff(pInstance, 1);
    _HalHdmitxIfExhibit(pInstance);
    u32Time1 = DrvHdmitxOsGetSystemTime();

    while (1)
    {
        DrvHdmitxOsMsSleep(10);

        enStatus = psHDMITXResPri->stHDMITxInfo.hdmitx_fsm_state;
        u32Time2 = DrvHdmitxOsGetSystemTime();

        if (HDMITX_FSM_IS_DONE(enStatus) || (u32Time2 - u32Time1) > 500)
        {
            break;
        }
    }

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Status:%d, Time:%d\n", __FUNCTION__, __LINE__, enStatus,
               (u32Time2 - u32Time1));
}

//------------------------------------------------------------------------------
// Signal
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoSignalConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;
    HalHdmitxSignalConfig_t * pstSignalCfg = (HalHdmitxSignalConfig_t *)pCfg;

    pstHalCtx->bSignal = pstSignalCfg->bEn;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Signal=%d\n", __FUNCTION__, __LINE__, pstHalCtx->bSignal);

    return enRet;
}

static void _HalHdmitxIfSetSignalConfig(void *pCtx, void *pCfg)
{
    HalHdmitxSignalConfig_t *   pstSignalCfg = (HalHdmitxSignalConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pstHdmitxCtx, &psHDMITXResPri))
    {
        return;
    }
    if (pstSignalCfg->bEn)
    {
        psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_locked = 0;
        _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, 1);
    }
    else
    {
        _HalHdmitxIfSetTMDSOnOff(psHDMITXResPri, 0);
        psHDMITXResPri->stHDMITxInfo.hdmitx_tmds_locked = 1;
    }
}

//------------------------------------------------------------------------------
// Mute
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoMuteConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e       enRet        = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *    pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    DrvHdmitxCtxHalContain_t *pstHalCtx    = pstHdmitxCtx->pstMhalCtx;
    HalHdmitxMuteConfig_t *   pstMuteCfg   = (HalHdmitxMuteConfig_t *)pCfg;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Vidoe:%d, Audio:%d, AvMute:%d\n", __FUNCTION__, __LINE__,
               pstHalCtx->bVideoOnOff, pstHalCtx->bAudioOnOff, pstHalCtx->bAvMute);

    if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_VIDEO)
    {
        pstHalCtx->bVideoOnOff = pstMuteCfg->bMute ? 0 : 1;
    }

    if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AUDIO)
    {
        pstHalCtx->bAudioOnOff = pstMuteCfg->bMute ? 0 : 1;
    }

    if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AVMUTE)
    {
        pstHalCtx->bAvMute = pstMuteCfg->bMute;
    }

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, enType:%x, Vidoe:%d, Audio:%d, AvMute:%d\n", __FUNCTION__, __LINE__,
               pstMuteCfg->enType, pstHalCtx->bVideoOnOff, pstHalCtx->bAudioOnOff, pstHalCtx->bAvMute);

    return enRet;
}

static void _HalHdmitxIfSetMuteConfig(void *pCtx, void *pCfg)
{
    HalHdmitxMuteConfig_t *     pstMuteCfg   = (HalHdmitxMuteConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pstHdmitxCtx, &psHDMITXResPri))
    {
        return;
    }
    if (pstMuteCfg->bMute)
    {
        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AVMUTE)
        {
            _HalHdmitxIfSetAVMUTE(pstHdmitxCtx, 1);
            psHDMITXResPri->stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_GC].bForceDisable = 1;
        }

        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_VIDEO)
        {
            _HalHdmitxIfSetVideoOnOff(pstHdmitxCtx, 0);
        }

        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AUDIO)
        {
            _HalHdmitxIfSetAudioOnOff(pstHdmitxCtx, 0);
        }
    }
    else
    {
        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AUDIO)
        {
            _HalHdmitxIfSetAudioOnOff(pstHdmitxCtx, 1);
        }

        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_VIDEO)
        {
            _HalHdmitxIfSetVideoOnOff(pstHdmitxCtx, 1);
        }

        if (pstMuteCfg->enType & E_HAL_HDMITX_MUTE_AVMUTE)
        {
            psHDMITXResPri->stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_GC].bForceDisable = 0;
            _HalHdmitxIfSetAVMUTE(pstHdmitxCtx, 0);
        }
    }
}

//------------------------------------------------------------------------------
// Analog Driving Current
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoAnalogDrvCurConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e           enRet             = E_HAL_HDMITX_QUERY_RET_OK;
    DrvHdmitxCtxConfig_t *        pstHdmitxCtxCfg   = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxAnaloDrvCurConfig_t *pstAnalgDrvCurCfg = (HalHdmitxAnaloDrvCurConfig_t *)pCfg;

    memcpy(&pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg, pstAnalgDrvCurCfg, sizeof(HalHdmitxAnaloDrvCurConfig_t));
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, (%x %x %x %x), (%x %x %x %x) (%x %x %x)\n", __FUNCTION__, __LINE__,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch0,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch1,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch2,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch3,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch0,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch1,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch2,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch3,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdRtermCh,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdLdoPreDrvCh,
               pstHdmitxCtxCfg->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdLdoMuxCh);

    return enRet;
}

static void _HalHdmitxIfSetAnalogDrvCurConfig(void *pCtx, void *pCfg)
{
    DrvHdmitxCtxConfig_t *      pInstance = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    HalHdmitxSetAnalogDrvCur(psHDMITXResPri->u32DeviceID, (HalHdmitxAnaloDrvCurConfig_t *)pCfg);
}

//------------------------------------------------------------------------------
// Sink Info
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoSinkInfoConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e         enRet            = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxSinkInfoConfig_t * pstHalSinkInfCfg = (HalHdmitxSinkInfoConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pInstance        = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return enRet;
    }

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, enType: %s\n", __FUNCTION__, __LINE__,
               PARSING_HAL_SINK_TYPE(pstHalSinkInfCfg->enType));

    switch (pstHalSinkInfCfg->enType)
    {
        case E_HAL_HDMITX_SINK_INFO_EDID_DATA:
        {
            HalHdmitxSinkEdidDataConfig_t *pstEdidData = &pstHalSinkInfCfg->stInfoUnit.stEdidData;

            if (_HalHdmitxIfGetEDIDData(pInstance, pstEdidData->au8EdidData, pstEdidData->u8BlockId) == 0)
            {
                if (_HalHdmitxIfCheckEdidRdy(pCtx))
                {
                    if (_HalHdmitxIfGetEDIDData(pInstance, pstEdidData->au8EdidData, pstEdidData->u8BlockId) == 0)
                    {
                        enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                        HDMITX_ERR("%s %d, Get Edid Data Fail\n", __FUNCTION__, __LINE__);
                    }
                }
                else
                {
                    enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                    HDMITX_ERR("%s %d, Check Edid Data Fail\n", __FUNCTION__, __LINE__);
                }
            }
            break;
        }

        case E_HAL_HDMITX_SINK_INFO_COLOR_FORMAT:
        {
            HalHdmitxSinkColorFormatConfig_t *pstSinkColorCfg = &pstHalSinkInfCfg->stInfoUnit.stColoFmt;
            HalHdmitxTimingResType_e          timing          = pstSinkColorCfg->enTiming;
            u32                               u32ColorFmt     = 0;

            if (_HalHdmitxIfGetColorFormatFromEDID(pInstance, timing, &u32ColorFmt) == 0)
            {
                if (_HalHdmitxIfCheckEdidRdy(pCtx))
                {
                    if (_HalHdmitxIfGetColorFormatFromEDID(pInstance, timing, &u32ColorFmt) == 0)
                    {
                        enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                        HDMITX_ERR("%s %d, Get Color FmtFail\n", __FUNCTION__, __LINE__);
                    }
                }
                else
                {
                    enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                    HDMITX_ERR("%s %d, Check Edid Data Fail\n", __FUNCTION__, __LINE__);
                }
            }

            if (enRet == E_HAL_HDMITX_QUERY_RET_OK)
            {
                pstSinkColorCfg->u32Color = u32ColorFmt;
            }
            break;
        }

        case E_HAL_HDMITX_SINK_INFO_HDMI_SUPPORT:
        {
            HalHdmitxSinkSupportHdmiConfig_t *pstHdmiSupportedCfg = &pstHalSinkInfCfg->stInfoUnit.stSupportedHdmi;
            u8                                bHdmiSupport        = 0;
            bHdmiSupport                                          = psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi;
            if (psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready)
            {
                bHdmiSupport = psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi;
            }
            else
            {
                if (_HalHdmitxIfCheckEdidRdy(pCtx))
                {
                    if (psHDMITXResPri->stHDMITxInfo.hdmitx_edid_ready)
                    {
                        bHdmiSupport = psHDMITXResPri->stHDMITxInfo.RxEdidInfo.SupportHdmi;
                    }
                    else
                    {
                        enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                        HDMITX_ERR("%s %d, Get HDMI Supported Fail\n", __FUNCTION__, __LINE__);
                    }
                }
                else
                {
                    enRet = E_HAL_HDMITX_QUERY_RET_ERR;
                    HDMITX_ERR("%s %d, Check Edid Data Fail\n", __FUNCTION__, __LINE__);
                }
            }

            if (enRet == E_HAL_HDMITX_QUERY_RET_OK)
            {
                pstHdmiSupportedCfg->bSupported = bHdmiSupport;
            }
            break;
        }

        case E_HAL_HDMITX_SINK_INFO_HPD_STATUS:
            pstHalSinkInfCfg->stInfoUnit.stHpdStatus.bHpd =
                (HalHdmitxGetRXStatus(psHDMITXResPri->bSetHPD, psHDMITXResPri->u32DeviceID)
                 == E_HDMITX_DVICLOCK_H_HPD_H)
                    ? 1
                    : 0;
            break;

        default:
            enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
            HDMITX_ERR("%s %d, Unkonw Type %d\n", __FUNCTION__, __LINE__, pstHalSinkInfCfg->enType);
            break;
    }

    return enRet;
}

static void _HalHdmitxIfSetSinkInfoConfig(void *pCtx, void *pCfg) {}

//------------------------------------------------------------------------------
// Info Frame
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoInfoFrameConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e         enRet           = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxInfoFrameConfig_t *pstInfoFrameCfg = (HalHdmitxInfoFrameConfig_t *)pCfg;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, enType:%s, En:%d\n", __FUNCTION__, __LINE__,
               PARSING_HAL_INFOFRAME_TYPE(pstInfoFrameCfg->enType), pstInfoFrameCfg->bEn);

    if (pstInfoFrameCfg->enType == E_HAL_HDMITX_INFOFRAM_TYPE_MAX)
    {
        enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
        HDMITX_ERR("%s %d, enType is Unknown %d", __FUNCTION__, __LINE__, pstInfoFrameCfg->enType);
    }
    else
    {
        u8 u8DataLen;

        u8DataLen = pstInfoFrameCfg->enType == E_HAL_HDMITX_INFOFRAM_TYPE_AVI   ? HAL_HDMITX_IF_AVI_INFOFRAME_DATA_SIZE
                    : pstInfoFrameCfg->enType == E_HAL_HDMITX_INFOFRAM_TYPE_SPD ? HAL_HDMITX_IF_SPD_INFOFRAME_DATA_SIZE
                    : pstInfoFrameCfg->enType == E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO
                        ? HAL_HDMITX_IF_AUD_INFOFRAME_DATA_SIZE
                        : 0;

        if (pstInfoFrameCfg->u8DataLen > u8DataLen)
        {
            enRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
            HDMITX_ERR("%s %d, %s InfoFrame Data Len is not mathc %d != %d", __FUNCTION__, __LINE__,
                       PARSING_HAL_INFOFRAME_TYPE(pstInfoFrameCfg->enType), pstInfoFrameCfg->u8DataLen, u8DataLen);
        }
    }
    return enRet;
}

static void _HalHdmitxIfSetInfoFrameConfig(void *pCtx, void *pCfg)
{
    HalHdmitxInfoFrameConfig_t *pstInfoFrameCfg = (HalHdmitxInfoFrameConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pInstance       = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    if (pstInfoFrameCfg->bEn)
    {
        if (HalHdmitxInfoFrameContentDefine(psHDMITXResPri->u32DeviceID, pstInfoFrameCfg->enType,
                                            pstInfoFrameCfg->au8Data, pstInfoFrameCfg->u8DataLen)
            == 0)
        {
            HDMITX_ERR("%s %d, Set %s Info Fail\n", __FUNCTION__, __LINE__,
                       PARSING_HAL_INFOFRAME_TYPE(pstInfoFrameCfg->enType));
        }
        else
        {
            _HalHdmitxIfInfoFrameUserDefine(pInstance, pstInfoFrameCfg->enType, 1, E_HDMITX_CYCLIC_PACKET, 1);
        }
    }
    else
    {
        _HalHdmitxIfInfoFrameUserDefine(pInstance, pstInfoFrameCfg->enType, 0, E_HDMITX_CYCLIC_PACKET, 1);
    }
}

//------------------------------------------------------------------------------
// Debug Level
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoDebugLevelConfig(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e enRet = E_HAL_HDMITX_QUERY_RET_OK;

    return enRet;
}

static void _HalHdmitxIfSetDebugLevelConfig(void *pCtx, void *pCfg)
{
    u32 *pu32DebugLevel = (u32 *)pCfg;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, DbgLevel:%x\n", __FUNCTION__, __LINE__, *pu32DebugLevel);
    HDMITX_DBG_VAL(pu32DebugLevel);
}

//------------------------------------------------------------------------------
// Hpd
//------------------------------------------------------------------------------
static HalHdmitxQueryRet_e _HalHdmitxIfGetHpdConfig(void *pCtx, void *pCfg)
{
    DrvHdmitxCtxConfig_t *pstHdmitxCtx = (DrvHdmitxCtxConfig_t *)pCtx;
    HalHdmitxHpdConfig_t *pstHpdCfg    = (HalHdmitxHpdConfig_t *)pCfg;
    HalHdmitxQueryRet_e   enRet        = E_HAL_HDMITX_QUERY_RET_OK;

    memcpy(&pstHdmitxCtx->pstMhalCtx->stHpdCfg, pstHpdCfg, sizeof(HalHdmitxHpdConfig_t));

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Hpd GpioNum=%d\n", __FUNCTION__, __LINE__,
               pstHdmitxCtx->pstMhalCtx->stHpdCfg.u8GpioNum);
    return enRet;
}

static void _HalHdmitxIfSetHpdConfig(void *pCtx, void *pCfg)
{
    HalHdmitxHpdConfig_t *pstHpdCfg = (HalHdmitxHpdConfig_t *)pCfg;

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "_gHPDGpioPin = 0x%hhx\r\n", pstHpdCfg->u8GpioNum);
    DrvHdmitxOsSetHpdGpinPin(pstHpdCfg->u8GpioNum);
    DrvHdmitxOsGpioRequestInput(pstHpdCfg->u8GpioNum);
}
static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoForceAttrConfigSet(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e         enRet           = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxForceAttrConfig_t *pstForceAttrCfg = (HalHdmitxForceAttrConfig_t *)pCfg;

    if (pstForceAttrCfg->eFlag & E_HAL_HDMITX_FORCE_FLAG_OUPUT_MODE)
    {
        g_stForceAttrCfg.eFlag |= E_HAL_HDMITX_FORCE_FLAG_OUPUT_MODE;
        g_stForceAttrCfg.bOutputMode  = pstForceAttrCfg->bOutputMode;
        g_stForceAttrCfg.enOutputMode = pstForceAttrCfg->enOutputMode;
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, OutpuModeEn:%d, Mode:%s\n", __FUNCTION__, __LINE__,
                   g_stForceAttrCfg.bOutputMode, PARSING_HAL_OUTPUT_MODE(g_stForceAttrCfg.enOutputMode));
    }

    if (pstForceAttrCfg->eFlag & E_HAL_HDMITX_FORCE_FLAG_HPD_DETECT)
    {
        g_stForceAttrCfg.eFlag |= E_HAL_HDMITX_FORCE_FLAG_HPD_DETECT;
        g_stForceAttrCfg.bHpdDetect = pstForceAttrCfg->bHpdDetect;
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, HpdDetect:%d\n", __FUNCTION__, __LINE__,
                   g_stForceAttrCfg.bHpdDetect);
    }

    return enRet;
}

static void _HalHdmitxIfSetForceAttrConfigSet(void *pCtx, void *pCfg)
{
    HalHdmitxForceAttrConfig_t *pstForceAttrCfg = (HalHdmitxForceAttrConfig_t *)pCfg;
    DrvHdmitxCtxConfig_t *      pInstance       = pCtx;
    HalHdmitxResourcePrivate_t *psHDMITXResPri;
    if (!_HalHdmitxIfGetHwCtx(pInstance, &psHDMITXResPri))
    {
        return;
    }

    if (pstForceAttrCfg->eFlag & E_HAL_HDMITX_FORCE_FLAG_OUPUT_MODE)
    {
        _HalHdmitxIfForceHDMIOutputMode(pCtx, g_stForceAttrCfg.bOutputMode, g_stForceAttrCfg.enOutputMode);
    }

    if (pstForceAttrCfg->eFlag & E_HAL_HDMITX_FORCE_FLAG_HPD_DETECT)
    {
        psHDMITXResPri->bSetHPD = pstForceAttrCfg->bHpdDetect;
    }
}

static HalHdmitxQueryRet_e _HalHdmitxIfGetInfoForceAttrConfigGet(void *pCtx, void *pCfg)
{
    HalHdmitxQueryRet_e         enRet           = E_HAL_HDMITX_QUERY_RET_OK;
    HalHdmitxForceAttrConfig_t *pstForceAttrCfg = (HalHdmitxForceAttrConfig_t *)pCfg;

    memcpy(pstForceAttrCfg, &g_stForceAttrCfg, sizeof(HalHdmitxForceAttrConfig_t));

    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL_IF, "%s %d, Flag:%x, OutpuModeEn:%d, Mode:%s, HpdDetect:%d\n", __FUNCTION__,
               __LINE__, g_stForceAttrCfg.eFlag, g_stForceAttrCfg.bOutputMode,
               PARSING_HAL_OUTPUT_MODE(g_stForceAttrCfg.enOutputMode), g_stForceAttrCfg.bHpdDetect);

    return enRet;
}

static void _HalHdmitxIfSetForceAttrConfigGet(void *pCtx, void *pCfg) {}

//------------------------------------------------------------------------------
// Call Back
//------------------------------------------------------------------------------
static u8 _HalHdmitxIfGetCallBack(DrvHdmitxCtxConfig_t *pstHdmitxCfg, HalHdmitxQueryConfig_t *pstQueryCfg)
{
    memset(&pstQueryCfg->stOutCfg, 0, sizeof(HalHdmitxQueryOutConfig_t));

    if (pstQueryCfg->stInCfg.u32CfgSize != g_aHdmitxCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize)
    {
        pstQueryCfg->stOutCfg.enQueryRet = E_HAL_HDMITX_QUERY_RET_CFGERR;
        HDMITX_ERR("%s %d, Size %d != %d\n", __FUNCTION__, __LINE__, pstQueryCfg->stInCfg.u32CfgSize,
                   g_aHdmitxCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize);
    }
    else
    {
        pstQueryCfg->stOutCfg.pSetFunc = g_aHdmitxCbTbl[pstQueryCfg->stInCfg.enQueryType].pSetFunc;

        if (pstQueryCfg->stOutCfg.pSetFunc == NULL)
        {
            pstQueryCfg->stOutCfg.enQueryRet = E_HAL_HDMITX_QUERY_RET_NOTSUPPORT;
        }
        else
        {
            pstQueryCfg->stOutCfg.enQueryRet = g_aHdmitxCbTbl[pstQueryCfg->stInCfg.enQueryType].pGetInfoFunc(
                pstHdmitxCfg, pstQueryCfg->stInCfg.pInCfg);
        }
    }

    return 1;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 HalHdmitxIfInit(void)
{
    if (g_bHalIfInit)
    {
        return 1;
    }

    memset(g_aHdmitxCbTbl, 0, sizeof(HalHdmitxQueryCallBackFunc_t) * E_HAL_HDMITX_QUERY_MAX);
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INIT].pSetFunc     = _HalHdmitxIfSetInitConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INIT].pGetInfoFunc = _HalHdmitxIfGetInfoInitConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INIT].u16CfgSize   = 0;

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEINIT].pSetFunc     = _HalHdmitxIfSetDeInitConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEINIT].pGetInfoFunc = _HalHdmitxIfGetInfoDeInitConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEINIT].u16CfgSize   = 0;

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_BEGIN].pSetFunc     = _HalHdmitxIfSetAttrBeingConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_BEGIN].pGetInfoFunc = _HalHdmitxIfGetInfoAttrBeginConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_BEGIN].u16CfgSize   = 0;

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR].pSetFunc     = _HalHdmitxIfSetAttrConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR].pGetInfoFunc = _HalHdmitxIfGetInfoAttrConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR].u16CfgSize   = sizeof(HalHdmitxAttrConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_END].pSetFunc     = _HalHdmitxIfSetAttrEndConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_END].pGetInfoFunc = _HalHdmitxIfGetInfoAttrEndConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ATTR_END].u16CfgSize   = 0;

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SIGNAL].pSetFunc     = _HalHdmitxIfSetSignalConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SIGNAL].pGetInfoFunc = _HalHdmitxIfGetInfoSignalConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SIGNAL].u16CfgSize   = sizeof(HalHdmitxSignalConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_MUTE].pSetFunc     = _HalHdmitxIfSetMuteConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_MUTE].pGetInfoFunc = _HalHdmitxIfGetInfoMuteConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_MUTE].u16CfgSize   = sizeof(HalHdmitxMuteConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ANALOG_DRV_CUR].pSetFunc     = _HalHdmitxIfSetAnalogDrvCurConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ANALOG_DRV_CUR].pGetInfoFunc = _HalHdmitxIfGetInfoAnalogDrvCurConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_ANALOG_DRV_CUR].u16CfgSize   = sizeof(HalHdmitxAnaloDrvCurConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SINK_INFO].pSetFunc     = _HalHdmitxIfSetSinkInfoConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SINK_INFO].pGetInfoFunc = _HalHdmitxIfGetInfoSinkInfoConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_SINK_INFO].u16CfgSize   = sizeof(HalHdmitxSinkInfoConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INFO_FRAME].pSetFunc     = _HalHdmitxIfSetInfoFrameConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INFO_FRAME].pGetInfoFunc = _HalHdmitxIfGetInfoInfoFrameConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_INFO_FRAME].u16CfgSize   = sizeof(HalHdmitxInfoFrameConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEBUG_LEVEL].pSetFunc     = _HalHdmitxIfSetDebugLevelConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEBUG_LEVEL].pGetInfoFunc = _HalHdmitxIfGetInfoDebugLevelConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_DEBUG_LEVEL].u16CfgSize   = sizeof(u32);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_HPD].pSetFunc     = _HalHdmitxIfSetHpdConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_HPD].pGetInfoFunc = _HalHdmitxIfGetHpdConfig;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_HPD].u16CfgSize   = sizeof(HalHdmitxHpdConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_SET].pSetFunc     = _HalHdmitxIfSetClkSet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_SET].pGetInfoFunc = _HalHdmitxIfGetInfoClkSet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_SET].u16CfgSize   = sizeof(HalHdmitxClkConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_GET].pSetFunc           = _HalHdmitxIfSetClkGet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_GET].pGetInfoFunc       = _HalHdmitxIfGetInfoClkGet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_CLK_GET].u16CfgSize         = sizeof(HalHdmitxClkConfig_t);
    
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_SET].pSetFunc     = _HalHdmitxIfSetForceAttrConfigSet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_SET].pGetInfoFunc = _HalHdmitxIfGetInfoForceAttrConfigSet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_SET].u16CfgSize   = sizeof(HalHdmitxForceAttrConfig_t);

    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_GET].pSetFunc     = _HalHdmitxIfSetForceAttrConfigGet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_GET].pGetInfoFunc = _HalHdmitxIfGetInfoForceAttrConfigGet;
    g_aHdmitxCbTbl[E_HAL_HDMITX_QUERY_FORCE_CFG_GET].u16CfgSize   = sizeof(HalHdmitxForceAttrConfig_t);
    g_bHalIfInit                                                  = 1;
    if (DrvHdmitxOsInit() == FALSE)
    {
        return FALSE;
    }
    if (g_s32HDMITXMutex == -1)
    {
        if (DrvHdmitxOsCreateMutex(&g_tHDMITXMutex))
        {
            g_s32HDMITXMutex = g_tHDMITXMutex.s32Id;
        }

        if (g_s32HDMITXMutex == -1)
        {
            HDMITX_ERR("[HDMITX][%06d] create mutex fail\n", __LINE__);
            return FALSE;
        }
    }
    return 1;
}

u8 HalHdmitxIfDeInit(void)
{
    if (g_bHalIfInit == 0)
    {
        HDMITX_ERR("%s %d, HalIf not init\n", __FUNCTION__, __LINE__);
        return 0;
    }
    g_bHalIfInit = 0;
    if (g_s32HDMITXMutex)
    {
        if (DrvHdmitxOsDestroyMutex(&g_tHDMITXMutex))
        {
            g_s32HDMITXMutex = -1;
        }
    }
    memset(g_aHdmitxCbTbl, 0, sizeof(HalHdmitxQueryCallBackFunc_t) * E_HAL_HDMITX_QUERY_MAX);

    return 1;
}

u8 HalHdmitxIfQuery(void *pCtx, void *pCfg)
{
    u8 bRet = 1;

    if (pCtx == NULL || pCfg == NULL)
    {
        HDMITX_ERR("%s %d, Input Param is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = _HalHdmitxIfGetCallBack((DrvHdmitxCtxConfig_t *)pCtx, (HalHdmitxQueryConfig_t *)pCfg);
    }

    return bRet;
}
