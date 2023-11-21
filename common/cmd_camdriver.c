/*
* cmd_camdriver.c- Sigmastar
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
#include "jinclude.h"
#include "jpeglib.h"
#include "jpegimg.c"
#endif


#if defined(CONFIG_SSTAR_RGN)
#include "mhal_rgn_datatype.h"
#include "mhal_rgn.h"
#endif

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define CAMDRV_DISP_TIMING_NUM         16
#define CAMDRV_DISP_USER_TIMIE_ID      (CAMDRV_DISP_TIMING_NUM - 1)

#if defined(CONFIG_ARCH_MERCURY6)
#define CAMDRV_DISP_DEVICE_MAX         2
#define CAMDRV_DISP_VIDLAYER_MAX       2
#define CAMDRV_DISP_INPUTPORT_MAX      17
#else
#define CAMDRV_DISP_DEVICE_MAX         1
#define CAMDRV_DISP_VIDLAYER_MAX       1
#define CAMDRV_DISP_INPUTPORT_MAX      1
#endif

#define CAMDRV_ALIGN(x, y)           (((x)+(y-1)) & ~(y-1))

#define IMAGE_WIDHT        256
#define IMAGE_HEIGHT       256
#define IMAGE_STRIDE       (CAMDRV_ALIGN(IMAGE_WIDHT+16, 16) * 2)


#define PRINT_NONE    "\33[m"
#define PRINT_RED     "\33[1;31m"
#define PRINT_YELLOW  "\33[1;33m"
#define PRINT_GREEN   "\33[1;32m"

#define CAMDRV_DBG_LV_ERR      0x01
#define CAMDRV_DBG_LV_INF      0x02
#define CAMDRV_DBG_LV_JPEG      0x04

#define CAMDRV_DBG(dbglv, _fmt, _args...)    \
    do                                          \
    if(dbglv & gu32BootCamDbgLevel)              \
    {                                           \
            printf(PRINT_YELLOW _fmt PRINT_NONE, ## _args);             \
    }while(0)

#define CAMDRV_DISP_INTF_PNL ( MHAL_DISP_INTF_TTL | MHAL_DISP_INTF_MIPIDSI | MHAL_DISP_INTF_BT656 | MHAL_DISP_INTF_BT601 | MHAL_DISP_INTF_BT1120 | MHAL_DISP_INTF_SRGB | MHAL_DISP_INTF_MCU)

#define CAMDRV_VIRTUAL_ADDRESS_OFFSET 0x20000000


#define IS_JPEG_COLOR_YUV(color)    ( (color>=E_CAMDRV_JPEG_OUTPUT_COLOR_YUV444) && (color < E_CAMDRV_JPEG_OUTPUT_COLOR_YUV_MAX) )


#define PARSING_JPEG_OUT_COLOR(x)    (  x == E_CAMDRV_JPEG_OUTPUT_COLOR_YUV444      ? "YUV444" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_YUV420_NV12 ? "YUV420_NV12" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_RGB888      ? "RGB888" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_RGB565      ? "RGB565" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB1555    ? "ARGB1555" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB4444    ? "ARGB4444" : \
                                        x == E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB8888    ? "ARGB8888" : \
                                                                                      "UNKNOWN" )

//-------------------------------------------------------------------------------------------------
//  structure & Enu
//-------------------------------------------------------------------------------------------------
typedef enum
{
   //------ COLOR YUV ----
    E_CAMDRV_JPEG_OUTPUT_COLOR_YUV444        = 0x00,
    E_CAMDRV_JPEG_OUTPUT_COLOR_YUV420_NV12   = 0x01,
    E_CAMDRV_JPEG_OUTPUT_COLOR_YUV422_YVYU   = 0x02,
    E_CAMDRV_JPEG_OUTPUT_COLOR_YUV_MAX       = 0x7F,

    //------ COLOR RGB ----
    E_CAMDRV_JPEG_OUTPUT_COLOR_RGB888        = 0x80,
    E_CAMDRV_JPEG_OUTPUT_COLOR_RGB565        = 0x81,
    E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB1555      = 0x82,
    E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB4444      = 0x83,
    E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB8888      = 0x84,
    E_CAMDRV_JPEG_OUTPUT_COLOR_RGB_MAX       = 0xFF,

    E_CAMDRV_JPEG_OUTPUT_COLOR_MAX      = 0x100,
}CamDrvJpegOutputColorFormat_e;

typedef enum
{
    E_CAMDRV_MODULE_NONE    = 0x0000,
    E_CAMDRV_MODULE_DISP    = 0x0001,
    E_CAMDRV_MODULE_PNL     = 0x0002,
    E_CAMDRV_MODULE_HDMITX  = 0x0004,
    E_CAMDRV_MODULE_MOP     = 0x0008,
    E_CAMDRV_MODULE_GOP     = 0x0010,
    E_CAMDRV_MODULE_JPEG    = 0x0020,
}CamDrvModuleType_e;

typedef struct
{
    u8  aPanelName[20];
    u16 PanelType;
    u16 u16Tgn_HSyncWidth;
    u16 u16Tgn_HSyncBackPorch;
    u16 u16Tgn_VSyncWidth;
    u16 u16Tgn_VSyncBackPorch;
    u16 u16Tgn_HStart;
    u16 u16Tgn_VStart;
    u16 u16Tgn_Width;
    u16 u16Tgn_Height;
    u16 u16Tgn_HTotal;
    u16 u16Tgn_VTotal;
    u32 u32Tgn_DCLK;
    u16 u16Tgn_InvDCLK;
    u16 u16Tgn_InvDE;
    u16 u16Tgn_InvHSync;
    u16 u16Tgn_InvVSync;
    u16 u16Tgn_OutputFormatBitMode;
    u16 u16Tgn_PadMux;
}BootCamPnlUnifiedParam_t;

typedef struct
{
    u32 u32DispDevIntf;
    u32 u32PathId;
    BootCamPnlUnifiedParam_t *pPnlUnfiedParam;
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_DeviceTiming_e eDispTiming;
#endif
#if defined(CONFIG_SSTAR_HDMITX)
    MhaHdmitxTimingResType_e eHdmiTiming;
    MhalHdmitxColorType_e eInColor;
    MhalHdmitxColorType_e eOutColor;
#endif
    CamDrvJpegOutputColorFormat_e eJpegColor;

    u16 u16InputWidth;
    u16 u16InputHeight;
    u16 u16InputStide;
    u16 u16OutputWidth;
    u16 u16OutputHeight;
    u64 u64PhyAddr;
}CamDrvInputParam_t;

typedef struct
{
    void *pDevCtx;
    void *pVidLayerCtx[CAMDRV_DISP_VIDLAYER_MAX];
    void *pInputPortCtx[CAMDRV_DISP_INPUTPORT_MAX];
}CamDrvDispContext_t;

typedef struct
{
#if defined(CONFIG_SSTAR_PNL)
    MhalPnlUnifiedParamConfig_t stUnfiedParam;
#endif
    void *pCtx;
}CamDrvPnlContext_t;

typedef struct
{
#if defined(CONFIG_SSTAR_HDMITX)
    MhalHdmitxColorType_e enInColor;
    MhalHdmitxColorType_e enOutColor;
    MhalHdmitxOutpuModeType_e enOuputMode;
    MhalHdmitxColorDepthType_e enColorDepth;
    MhaHdmitxTimingResType_e enOutputTiming;
    MhalHdmitxAudioFreqType_e enAudioFrq;
    MhalHdmitxAudioChannelType_e enAudioCh;
#endif
    bool bHpdDetect;
    void *pHdmitxCtx;
}CamDrvHdmiTxContext_t;

typedef struct
{
#if defined(CONFIG_SSTAR_DISP)
    MHAL_DISP_DeviceTiming_e enTiminId;
#endif
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
}CamDrvDispTimingConfig_t;

typedef struct
{
    u8 *pJpgBuffer;
    CamDrvJpegOutputColorFormat_e enOutputcolor;
    u8 *pData;
    u16 u16Width;
    u16 u16Height;
    u16 u16PixelSize;
} CamDrvJpegInfo_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 gu32BootCamDbgLevel = 0xFF;

CamDrvDispTimingConfig_t gstDispTimingTable[CAMDRV_DISP_TIMING_NUM] =
{
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
        64, 68, 4, 39, 132, 44, 720, 5760, 864, 625, 27},

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

    {   E_MHAL_DISP_OUTPUT_NTSC,
        137, 1, 44, 1, 138, 45, 720, 480, 858, 525, 27},

    {   E_MHAL_DISP_OUTPUT_PAL,
        64, 68, 3, 59, 132, 44, 720, 576, 864, 625, 27},

    {   E_MHAL_DISP_OUTPUT_USER,
        48, 46,  4,  23, 98, 27, 1024, 600, 1344, 635, 51},
};


BootCamPnlUnifiedParam_t gstPnlUnfiedParam_Ttl1024x600 =
{
    "TTL1024x600",
    0,    //  PanelType;
    48,   // u16Tgn_HSyncWidth;
    46,   // u16Tgn_HSyncBackPorch;
    4,    // u16Tgn_VSyncWidth;
    23,   // u16Tgn_VSyncBackPorch;
    94,   // u16Tgn_HStart;
    27,   // u16Tgn_VStart;
    1024, // u16Tgn_Width;
    600,  // u16Tgn_Height;
    1344, // u16Tgn_HTotal;
    635,  // u16Tgn_VTotal;
    51,   // u16Tgn_DCLK;
    0,    //u16Tgn_InvDCLK;
    0,    //u16Tgn_InvDE;
    0,    //u16Tgn_InvHSync;
    0,    //u16Tgn_InvVSync;
    0,    //u16Tgn_OutputFormatBitMode;
    1,    //u16PadMux
};

CamDrvDispContext_t gstCamDrvDispCtx[CAMDRV_DISP_DEVICE_MAX];
CamDrvHdmiTxContext_t gstCamDrvHdmitxCtx;
CamDrvPnlContext_t gstCamDrvPnlCtx[CAMDRV_DISP_DEVICE_MAX];
void *gpImageAddr = NULL;
u32  gu32ImageSize = 0;

//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
#if defined(CONFIG_SSTAR_HDMITX)

MhalHdmitxColorType_e _CamDrvTransHdmiColor(char *pColor)
{
    MhalHdmitxColorType_e eColor;

    eColor = !strcmp(pColor, "rgb")  ? E_MHAL_HDMITX_COLOR_RGB444 :
             !strcmp(pColor, "yuv")  ? E_MHAL_HDMITX_COLOR_YUV444 :
                                       E_MHAL_HDMITX_COLOR_AUTO;
    return eColor;
}

MhaHdmitxTimingResType_e _CamDrvTransHdmiTiming(char *pTiming)
{
    MhaHdmitxTimingResType_e eTiming;

    eTiming = !strcmp(pTiming, "480p60")  ? E_MHAL_HDMITX_RES_720X480P_60HZ :
              !strcmp(pTiming, "576p50")  ? E_MHAL_HDMITX_RES_720X576P_50HZ :
              !strcmp(pTiming, "720p50")  ? E_MHAL_HDMITX_RES_1280X720P_50HZ:
              !strcmp(pTiming, "720p60")  ? E_MHAL_HDMITX_RES_1280X720P_60HZ:
              !strcmp(pTiming, "1080p50") ? E_MHAL_HDMITX_RES_1920X1080P_50HZ:
              !strcmp(pTiming, "1080p60") ? E_MHAL_HDMITX_RES_1920X1080P_60HZ:
                                            E_MHAL_HDMITX_RES_MAX;
    return eTiming;
}

void _CamDrvHdmitxCreateInstance(void)
{
    if(gstCamDrvHdmitxCtx.pHdmitxCtx == FALSE)
    {
        MhalHdmitxCreateInstance(&gstCamDrvHdmitxCtx.pHdmitxCtx, 0);
    }
}


void _CamDrvHdmitxShow(CamDrvInputParam_t *pstInputParam)
{
    CamDrvHdmiTxContext_t *pstHdmitxCtx = &gstCamDrvHdmitxCtx;
    MhalHdmitxSignalConfig_t stSignalCfg;
    MhalHdmitxMuteConfig_t stMuteCfg;
    MhalHdmitxAttrConfig_t stAttrCfg;

    pstHdmitxCtx->bHpdDetect = 1;
    pstHdmitxCtx->enInColor = pstInputParam->eInColor;
    pstHdmitxCtx->enOutColor = pstInputParam->eOutColor;
    pstHdmitxCtx->enOutputTiming = pstInputParam->eHdmiTiming;
    pstHdmitxCtx->enColorDepth = E_MHAL_HDMITX_CD_24_BITS;
    pstHdmitxCtx->enOuputMode = E_MHAL_HDMITX_OUTPUT_MODE_HDMI;
    pstHdmitxCtx->enAudioFrq =  E_MHAL_HDMITX_AUDIO_FREQ_48K;
    pstHdmitxCtx->enAudioCh  =  E_MHAL_HDMITX_AUDIO_CH_2;

    _CamDrvHdmitxCreateInstance();

    stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
    stMuteCfg.bMute = 1;
    MhalHdmitxSetMute(pstHdmitxCtx->pHdmitxCtx, &stMuteCfg);

    stSignalCfg.bEn = 0;
    MhalHdmitxSetSignal(pstHdmitxCtx->pHdmitxCtx, &stSignalCfg);

    stAttrCfg.bVideoEn = 1;
    stAttrCfg.enOutColor   = pstHdmitxCtx->enOuputMode;
    stAttrCfg.enInColor    = pstHdmitxCtx->enInColor;
    stAttrCfg.enOutColor   = pstHdmitxCtx->enOutColor;
    stAttrCfg.enOutputMode = pstHdmitxCtx->enOuputMode;
    stAttrCfg.enColorDepth = pstHdmitxCtx->enColorDepth;
    stAttrCfg.enTiming     = pstHdmitxCtx->enOutputTiming;

    stAttrCfg.bAudioEn = 1;
    stAttrCfg.enAudioFreq = pstHdmitxCtx->enAudioFrq;
    stAttrCfg.enAudioCh   = pstHdmitxCtx->enAudioCh;
    stAttrCfg.enAudioFmt  = E_MHAL_HDMITX_AUDIO_FORMAT_PCM;
    stAttrCfg.enAudioCode = E_MHAL_HDMITX_AUDIO_CODING_PCM;

    MhalHdmitxSetAttr(pstHdmitxCtx->pHdmitxCtx, &stAttrCfg);

    stSignalCfg.bEn = 1;
    MhalHdmitxSetSignal(pstHdmitxCtx->pHdmitxCtx, &stSignalCfg);

    stMuteCfg.enType = E_MHAL_HDMITX_MUTE_AUDIO | E_MHAL_HDMITX_MUTE_VIDEO | E_MHAL_HDMITX_MUTE_AVMUTE;
    stMuteCfg.bMute = 0;
    MhalHdmitxSetMute(pstHdmitxCtx->pHdmitxCtx, &stMuteCfg);
}

#endif


#if defined(CONFIG_SSTAR_DISP)

MHAL_DISP_DeviceTiming_e _CamDrvTransDispTimingId(char *pTiming)
{
    MHAL_DISP_DeviceTiming_e eTiming;

    eTiming = !strcmp(pTiming, "480p60")  ? E_MHAL_DISP_OUTPUT_480P60 :
              !strcmp(pTiming, "576p50")  ? E_MHAL_DISP_OUTPUT_576P50 :
              !strcmp(pTiming, "720p50")  ? E_MHAL_DISP_OUTPUT_720P50 :
              !strcmp(pTiming, "720p60")  ? E_MHAL_DISP_OUTPUT_720P60 :
              !strcmp(pTiming, "1080p50") ? E_MHAL_DISP_OUTPUT_1080P50 :
              !strcmp(pTiming, "1080p60") ? E_MHAL_DISP_OUTPUT_1080P60 :
              !strcmp(pTiming, "ntsc")    ? E_MHAL_DISP_OUTPUT_NTSC :
              !strcmp(pTiming, "pal")     ? E_MHAL_DISP_OUTPUT_PAL :
                                            E_MHAL_DISP_OUTPUT_USER;
    return eTiming;
}

void _CamDrvDispCreateInstance(u32 u32Id)
{
    CamDrvDispContext_t *pstDispCtx = &gstCamDrvDispCtx[u32Id];

    if(pstDispCtx->pDevCtx == NULL)
    {
        MHAL_DISP_AllocPhyMem_t stDispAlloc;

        stDispAlloc.alloc = NULL;
        stDispAlloc.free = NULL;

        MHAL_DISP_DeviceCreateInstance(&stDispAlloc, u32Id, &pstDispCtx->pDevCtx);

        MHAL_DISP_VideoLayerCreateInstance(&stDispAlloc, u32Id == 0 ? 0 : 2, &pstDispCtx->pVidLayerCtx[0]);
        MHAL_DISP_VideoLayerBind(pstDispCtx->pVidLayerCtx[0], pstDispCtx->pDevCtx);

        MHAL_DISP_InputPortCreateInstance(
            &stDispAlloc, pstDispCtx->pVidLayerCtx[0], 0, &pstDispCtx->pInputPortCtx[0]);
     }
}

void _CamDrvDispPnlInit(void)
{
    MHAL_DISP_PanelConfig_t stPnlCfg[CAMDRV_DISP_TIMING_NUM];
    u16 i;

    if( sizeof(gstDispTimingTable)/sizeof(CamDrvDispTimingConfig_t) > CAMDRV_DISP_TIMING_NUM)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d:: Timing Talbe is bigger than %d\n",
            __FUNCTION__, __LINE__, CAMDRV_DISP_TIMING_NUM);
        return;
    }
    memset(stPnlCfg, 0, sizeof(MHAL_DISP_PanelConfig_t)*CAMDRV_DISP_TIMING_NUM);
    for(i=0; i<CAMDRV_DISP_TIMING_NUM; i++)
    {
        stPnlCfg[i].bValid = 1;
        stPnlCfg[i].eTiming = gstDispTimingTable[i].enTiminId;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncWidth     = gstDispTimingTable[i].u16HsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelHSyncBackPorch = gstDispTimingTable[i].u16HsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_ucPanelVSyncWidth     = gstDispTimingTable[i].u16VsyncWidht;
        stPnlCfg[i].stPanelAttr.m_ucPanelVBackPorch     = gstDispTimingTable[i].u16VsyncBacPorch;
        stPnlCfg[i].stPanelAttr.m_wPanelHStart          = gstDispTimingTable[i].u16Hstart;
        stPnlCfg[i].stPanelAttr.m_wPanelVStart          = gstDispTimingTable[i].u16Vstart;
        stPnlCfg[i].stPanelAttr.m_wPanelWidth           = gstDispTimingTable[i].u16Hactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHeight          = gstDispTimingTable[i].u16Vactive;
        stPnlCfg[i].stPanelAttr.m_wPanelHTotal          = gstDispTimingTable[i].u16Htotal;
        stPnlCfg[i].stPanelAttr.m_wPanelVTotal          = gstDispTimingTable[i].u16Vtotal;
        stPnlCfg[i].stPanelAttr.m_dwPanelDCLK           = gstDispTimingTable[i].u16DclkMhz;
    }

    MHAL_DISP_InitPanelConfig(stPnlCfg, CAMDRV_DISP_TIMING_NUM);
}

void _CamDrvDispRegFlipAndWaitDone(u32 u32Id)
{
    MHAL_DISP_RegFlipConfig_t stRegFlipCfg;
    MHAL_DISP_RegWaitDoneConfig_t stRegWaitDoneCfg;
    CamDrvDispContext_t *pstDispCtx = NULL;

    pstDispCtx = &gstCamDrvDispCtx[u32Id];

    stRegFlipCfg.bEnable = 1;
    stRegFlipCfg.pCmdqInf = NULL;
    MHAL_DISP_SetRegFlipConfig(pstDispCtx->pDevCtx, &stRegFlipCfg);

    stRegWaitDoneCfg.pCmdqInf = NULL;
    stRegWaitDoneCfg.u32WaitType = 0;
    MHAL_DISP_SetRegWaitDoneConfig(pstDispCtx->pDevCtx, &stRegWaitDoneCfg);
}


void _CamDrvDispShow(CamDrvInputParam_t *pstInputParam)
{
    CamDrvDispContext_t *pstDispCtx = NULL;
    MHAL_DISP_DeviceTimingInfo_t stDeviceTimingInfo;
    MHAL_DISP_SyncInfo_t stSyncInfo;

    _CamDrvDispCreateInstance(pstInputParam->u32PathId);
    pstDispCtx = &gstCamDrvDispCtx[pstInputParam->u32PathId];

    _CamDrvDispPnlInit();

    memset(&stDeviceTimingInfo, 0, sizeof(MHAL_DISP_DeviceTimingInfo_t));
    stDeviceTimingInfo.eTimeType = pstInputParam->eDispTiming;

    if(pstInputParam->u32DispDevIntf & CAMDRV_DISP_INTF_PNL)
    {
        BootCamPnlUnifiedParam_t *pPnlUnfidParam = pstInputParam->pPnlUnfiedParam;
        if(pPnlUnfidParam)
        {
            stSyncInfo.u16Vact = pPnlUnfidParam->u16Tgn_Height;
            stSyncInfo.u16Vpw  = pPnlUnfidParam->u16Tgn_VSyncWidth;
            stSyncInfo.u16Vbb  = pPnlUnfidParam->u16Tgn_VSyncBackPorch;
            stSyncInfo.u16Vfb  =
                pPnlUnfidParam->u16Tgn_VTotal - pPnlUnfidParam->u16Tgn_Height - pPnlUnfidParam->u16Tgn_VSyncWidth - pPnlUnfidParam->u16Tgn_VSyncBackPorch;
            stSyncInfo.u16Hact = pPnlUnfidParam->u16Tgn_Width;
            stSyncInfo.u16Hpw  = pPnlUnfidParam->u16Tgn_HSyncWidth;
            stSyncInfo.u16Hbb  = pPnlUnfidParam->u16Tgn_HSyncBackPorch;
            stSyncInfo.u16Hfb  =
                pPnlUnfidParam->u16Tgn_HTotal - pPnlUnfidParam->u16Tgn_Width - pPnlUnfidParam->u16Tgn_HSyncWidth - pPnlUnfidParam->u16Tgn_HSyncBackPorch;
            stSyncInfo.u32FrameRate = 60;
        }
        stDeviceTimingInfo.pstSyncInfo = &stSyncInfo;
    }

    MHAL_DISP_DeviceSetBackGroundColor(pstDispCtx->pDevCtx, 0x800080);
    MHAL_DISP_DeviceEnable(pstDispCtx->pDevCtx, 0);
    MHAL_DISP_DeviceAddOutInterface(pstDispCtx->pDevCtx, pstInputParam->u32DispDevIntf);
    MHAL_DISP_DeviceSetOutputTiming(pstDispCtx->pDevCtx, 1, &stDeviceTimingInfo);
    MHAL_DISP_DeviceEnable(pstDispCtx->pDevCtx, 1);

    _CamDrvDispRegFlipAndWaitDone(pstInputParam->u32PathId);

}

void _CamDrvDispMop(CamDrvInputParam_t *pstInputParam)
{
    MHAL_DISP_InputPortAttr_t stMopAttr;
    MHAL_DISP_VideoFrameData_t stVideoFrameBuffer;
    MHAL_DISP_VideoLayerAttr_t stVideoLayerAttr;

    CamDrvDispContext_t *pstDispCtx = NULL;

    stVideoFrameBuffer.au32Stride[2] = 0;

    _CamDrvDispCreateInstance(pstInputParam->u32PathId);

    pstDispCtx = &gstCamDrvDispCtx[pstInputParam->u32PathId];

    stVideoLayerAttr.stVidLayerDispWin.u16X = 0;
    stVideoLayerAttr.stVidLayerDispWin.u16Y = 0;
    stVideoLayerAttr.stVidLayerDispWin.u16Width = pstInputParam->u16OutputWidth;
    stVideoLayerAttr.stVidLayerDispWin.u16Height = pstInputParam->u16OutputHeight;
    MHAL_DISP_VideoLayerSetAttr(pstDispCtx->pVidLayerCtx[0], &stVideoLayerAttr);


    stMopAttr.stDispWin.u16X = 0;
    stMopAttr.stDispWin.u16Y = 0;
    stMopAttr.stDispWin.u16Width = pstInputParam->u16OutputWidth;
    stMopAttr.stDispWin.u16Height = pstInputParam->u16OutputHeight;
    stMopAttr.u16SrcWidth = pstInputParam->u16InputWidth;;
    stMopAttr.u16SrcHeight = pstInputParam->u16InputHeight;

    stVideoFrameBuffer.ePixelFormat = E_MHAL_PIXEL_FRAME_YUV_MST_420;
    stVideoFrameBuffer.aPhyAddr[0] = pstInputParam->u64PhyAddr;
    stVideoFrameBuffer.aPhyAddr[1] = pstInputParam->u64PhyAddr + pstInputParam->u16InputStide * pstInputParam->u16InputHeight;
    stVideoFrameBuffer.aPhyAddr[2] = 0;
    stVideoFrameBuffer.au32Stride[0] = pstInputParam->u16InputStide;
    stVideoFrameBuffer.au32Stride[1] = 0;

    MHAL_DISP_InputPortSetAttr(pstDispCtx->pInputPortCtx[0], &stMopAttr);
    MHAL_DISP_InputPortFlip(pstDispCtx->pInputPortCtx[0], &stVideoFrameBuffer);
    MHAL_DISP_InputPortEnable(pstDispCtx->pInputPortCtx[0], TRUE);

    _CamDrvDispRegFlipAndWaitDone(pstInputParam->u32PathId);

}
#endif

#if defined(CONFIG_SSTAR_PNL)

void _CamDrvPnlInitUnifiedParam(u32 u32Id, BootCamPnlUnifiedParam_t *pPnlParam)
{
    MhalPnlUnifiedParamConfig_t *pstPanelCfg = NULL;

    if(pPnlParam)
    {
        pstPanelCfg = &gstCamDrvPnlCtx[u32Id].stUnfiedParam;
        memset(pstPanelCfg, 0, sizeof(MhalPnlUnifiedParamConfig_t));

        pstPanelCfg->pPanelName = (const char *)pPnlParam->aPanelName;
        pstPanelCfg->eLinkType = pPnlParam->PanelType;

        pstPanelCfg->u8TgnTimingFlag = 1;
        pstPanelCfg->stTgnTimingInfo.u16HSyncWidth     =  pPnlParam->u16Tgn_HSyncWidth;
        pstPanelCfg->stTgnTimingInfo.u16HSyncBackPorch =  pPnlParam->u16Tgn_HSyncBackPorch;
        pstPanelCfg->stTgnTimingInfo.u16VSyncWidth     =  pPnlParam->u16Tgn_VSyncWidth;
        pstPanelCfg->stTgnTimingInfo.u16VSyncBackPorch =  pPnlParam->u16Tgn_VSyncBackPorch;
        pstPanelCfg->stTgnTimingInfo.u16HStart         =  pPnlParam->u16Tgn_HStart;
        pstPanelCfg->stTgnTimingInfo.u16VStart         =  pPnlParam->u16Tgn_VStart;
        pstPanelCfg->stTgnTimingInfo.u16HActive        =  pPnlParam->u16Tgn_Width;
        pstPanelCfg->stTgnTimingInfo.u16VActive        =  pPnlParam->u16Tgn_Height;
        pstPanelCfg->stTgnTimingInfo.u16HTotal         =  pPnlParam->u16Tgn_HTotal;
        pstPanelCfg->stTgnTimingInfo.u16VTotal         =  pPnlParam->u16Tgn_VTotal;
        pstPanelCfg->stTgnTimingInfo.u32Dclk           =  pPnlParam->u32Tgn_DCLK;

        pstPanelCfg->u8TgnPolarityFlag = 1;
        pstPanelCfg->stTgnPolarityInfo.u8InvDCLK  = pPnlParam->u16Tgn_InvDCLK;
        pstPanelCfg->stTgnPolarityInfo.u8InvDE    = pPnlParam->u16Tgn_InvDE;
        pstPanelCfg->stTgnPolarityInfo.u8InvHSync = pPnlParam->u16Tgn_InvHSync;
        pstPanelCfg->stTgnPolarityInfo.u8InvVSync = pPnlParam->u16Tgn_InvVSync;

        pstPanelCfg->u8TgnOutputBitMdFlag = 1;
        pstPanelCfg->eOutputFormatBitMode = pPnlParam->u16Tgn_OutputFormatBitMode;

        pstPanelCfg->u8TgnPadMuxFlag = 1;
        pstPanelCfg->u16PadMux = pPnlParam->u16Tgn_PadMux;
    }
}

void _CamDrvPnlCreateInstance(u16 u16Id, MhalPnlLinkType_e eLinkType)
{
    if(gstCamDrvPnlCtx[u16Id].pCtx == NULL)
    {
        MhalPnlCreateInstanceEx(&gstCamDrvPnlCtx[u16Id].pCtx, eLinkType, u16Id);
    }
}

void _CamDrvPnlShow(CamDrvInputParam_t *pstInputParam)
{
    _CamDrvPnlInitUnifiedParam(pstInputParam->u32PathId, pstInputParam->pPnlUnfiedParam);

    _CamDrvPnlCreateInstance(pstInputParam->u32PathId, pstInputParam->pPnlUnfiedParam->PanelType);

    MhalPnlSetUnifiedParamConfig(
        gstCamDrvPnlCtx[pstInputParam->u32PathId].pCtx,
        &gstCamDrvPnlCtx[pstInputParam->u32PathId].stUnfiedParam);
}


#endif


#if defined(CONFIG_SSTAR_RGN)
void _CamDrvRgnGop(CamDrvInputParam_t *pstInputParam)
{
    static bool bInit = 0;
    MHAL_RGN_GopType_e eGopId = E_MHAL_GOP_DISP0_UI;
    MHAL_RGN_GopGwinId_e eGwinId = E_MHAL_GOP_GWIN_ID_0;
    MHAL_RGN_GopWindowConfig_t stSrcWinCfg;
    MHAL_RGN_GopWindowConfig_t stDstWinCfg;

    stSrcWinCfg.u32X = 0;
    stSrcWinCfg.u32Y = 0;
    stSrcWinCfg.u32Width = pstInputParam->u16InputWidth;
    stSrcWinCfg.u32Height =  pstInputParam->u16InputHeight;
    stDstWinCfg.u32X = 0;
    stDstWinCfg.u32Y = 0;
    stDstWinCfg.u32Width = stSrcWinCfg.u32Width;
    stDstWinCfg.u32Height = stSrcWinCfg.u32Height;

    CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d\n", __FUNCTION__, __LINE__);
    if(!bInit)
    {
        MHAL_RGN_GopInit();
        bInit = 1;
    }

    MHAL_RGN_GopSetBaseWindow(eGopId, &stSrcWinCfg, &stDstWinCfg);
    MHAL_RGN_GopGwinSetPixelFormat(eGopId, eGwinId, E_MHAL_RGN_PIXEL_FORMAT_ARGB1555);
    MHAL_RGN_GopGwinSetWindow(eGopId, eGwinId,
        pstInputParam->u16InputWidth,
        pstInputParam->u16InputHeight,
        pstInputParam->u16InputWidth * 2,
        0, 0);

    MHAL_RGN_GopGwinSetBuffer(eGopId, eGwinId, pstInputParam->u64PhyAddr);
    MHAL_RGN_GopSetAlphaZeroOpaque(eGopId, FALSE, FALSE, E_MHAL_RGN_PIXEL_FORMAT_ARGB1555);
    MHAL_RGN_GopSetAlphaType(eGopId, eGwinId, E_MHAL_GOP_GWIN_ALPHA_CONSTANT, 0x00);
    MHAL_RGN_GopGwinEnable(eGopId,eGwinId);

}
#endif

#if defined(CONFIG_SSTAR_JPD)

bool _CamDrvJpegDecompress(CamDrvJpegInfo_t *pJpegInfo)
{
	// Variables for the source jpg
	u32 jpg_size;
	u8 *jpg_buffer;

	// Variables for the decompressor itself
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// Variables for the output buffer, and how long each row is
	u32 u32BmpSize;
	u8 *pu8BmpBuffer;
	u16 u16RowStride;
	int rc;


	// Load the jpeg data from a file into a memory buffer for
	// the purpose of this demonstration.
	// Normally, if it's a file, you'd use jpeg_stdio_src, but just
	// imagine that this was instead being downloaded from the Internet
	// or otherwise not coming from disk
    jpg_size = sizeof(JpegImage);
	jpg_buffer = JpegImage;


	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Create Decompress struct\n", __FUNCTION__, __LINE__);
	// Allocate a new decompress struct, with the default error handler.
	// The default error handler will exit() on pretty much any issue,
	// so it's likely you'll want to replace it or supplement it with
	// your own.
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);


	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Set memory buffer as source\n", __FUNCTION__, __LINE__);
	// Configure this decompressor to read its data from a memory
	// buffer starting at u8 *jpg_buffer, which is jpg_size
	// long, and which must contain a complete jpg already.
	//
	// If you need something fancier than this, you must write your
	// own data source manager, which shouldn't be too hard if you know
	// what it is you need it to do. See jpeg-8d/jdatasrc.c for the
	// implementation of the standard jpeg_mem_src and jpeg_stdio_src
	// managers as examples to work from.
	jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);


	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Read the JPEG header\n", __FUNCTION__, __LINE__);
	// Have the decompressor scan the jpeg header. This won't populate
	// the cinfo struct output fields, but will indicate if the
	// jpeg is valid.
	rc = jpeg_read_header(&cinfo, TRUE);

	if (rc != 1)
	{
		CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: File does not seem to be a normal JPEG\n", __FUNCTION__, __LINE__);
		return 0;
	}

	cinfo.out_color_space = IS_JPEG_COLOR_YUV(pJpegInfo->enOutputcolor) ? JCS_YCbCr : JCS_RGB;

	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Initiate JPEG decompression, color:%d\n", __FUNCTION__, __LINE__, cinfo.out_color_space);
	// By calling jpeg_start_decompress, you populate cinfo
	// and can then allocate your output bitmap buffers for
	// each scanline.
	jpeg_start_decompress(&cinfo);

	pJpegInfo->u16Width = cinfo.output_width;
	pJpegInfo->u16Height = cinfo.output_height;
	pJpegInfo->u16PixelSize = cinfo.output_components;

	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Image is %d x %d with %d components\n",
	    __FUNCTION__, __LINE__, pJpegInfo->u16Width, pJpegInfo->u16Height, pJpegInfo->u16PixelSize);

	u32BmpSize = pJpegInfo->u16Width * pJpegInfo->u16Height * pJpegInfo->u16PixelSize;
	pu8BmpBuffer = (u8*) malloc(u32BmpSize);

    if(pu8BmpBuffer == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d:: Alloc Buffer Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    pJpegInfo->pData = pu8BmpBuffer;

	// The u16RowStride is the total number of bytes it takes to store an
	// entire scanline (row).
	u16RowStride = pJpegInfo->u16Width * pJpegInfo->u16PixelSize;


	//printf( "Proc: Start reading scanlines\n");
	//
	// Now that you have the decompressor entirely configured, it's time
	// to read out all of the scanlines of the jpeg.
	//
	// By default, scanlines will come out in RGBRGBRGB...  order,
	// but this can be changed by setting cinfo.out_color_space
	//
	// jpeg_read_scanlines takes an array of buffers, one for each scanline.
	// Even if you give it a complete set of buffers for the whole image,
	// it will only ever decompress a few lines at a time. For best
	// performance, you should pass it an array with cinfo.rec_outbuf_height
	// scanline buffers. rec_outbuf_height is typically 1, 2, or 4, and
	// at the default high quality decompression setting is always 1.
	while (cinfo.output_scanline < cinfo.output_height)
	{
		u8 *buffer_array[1];
		buffer_array[0] = pu8BmpBuffer + \
						   (cinfo.output_scanline) * u16RowStride;

		jpeg_read_scanlines(&cinfo, buffer_array, 1);

	}
	CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Done reading scanlines\n", __FUNCTION__, __LINE__);


	// Once done reading *all* scanlines, release all internal buffers,
	// etc by calling jpeg_finish_decompress. This lets you go back and
	// reuse the same cinfo object with the same settings, if you
	// want to decompress several jpegs in a row.
	//
	// If you didn't read all the scanlines, but want to stop early,
	// you instead need to call jpeg_abort_decompress(&cinfo)
	jpeg_finish_decompress(&cinfo);

    CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: Finsh Decomporess\n", __FUNCTION__, __LINE__);
	// At this point, optionally go back and either load a new jpg into
	// the jpg_buffer, or define a new jpeg_mem_src, and then start
	// another decompress operation.

	// Once you're really really done, destroy the object to free everything
	jpeg_destroy_decompress(&cinfo);

	return 1;
}

bool _CamDrvJpegTransToYuv444(CamDrvJpegInfo_t *pJpegInfo)
{
    bool bRet = 1;



    return bRet;
}

bool _CamDrvJpegTransToRgb888(CamDrvJpegInfo_t *pJpegInfo)
{
    return 1;
}

bool _CamDrvJpegTransToRgb565(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Buf;
    u16 i, j, u16PixVal;
    u32 u32SrcIdx, u32DstIdx, u32BufSize;;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 2;
    pu8Buf = malloc(u32BufSize);

    if(pu8Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    u32SrcIdx = 0;
    u32DstIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            u16PixVal = (pJpegInfo->pData[u32SrcIdx] >> 3)  |
                        (pJpegInfo->pData[u32SrcIdx + 1] >> 2) << 5 |
                        (pJpegInfo->pData[u32SrcIdx + 2] >> 3) << 11 ;

            pu8Buf[u32DstIdx] = (u16PixVal & 0x00FF) ;
            pu8Buf[u32DstIdx+1] = (u16PixVal & 0xFF00) >> 8;
            u32SrcIdx += 3;
            u32DstIdx += 2;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Buf;
    pJpegInfo->u16PixelSize = 2;
    return 1;
}

bool _CamDrvJpegTransToArgb1555(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Buf;
    u16 i, j, u16PixVal;
    u32 u32SrcIdx, u32DstIdx, u32BufSize;;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 2;
    pu8Buf = malloc(u32BufSize);

    if(pu8Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR,"%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }
    CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d, W:%d H:%d, Size:%d\n",
        __FUNCTION__, __LINE__,
        pJpegInfo->u16Width,  pJpegInfo->u16Height, u32BufSize);
    u32SrcIdx = 0;
    u32DstIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            u16PixVal = (pJpegInfo->pData[u32SrcIdx] >> 3)  |
                        (pJpegInfo->pData[u32SrcIdx + 1] >> 3) << 5 |
                        (pJpegInfo->pData[u32SrcIdx + 2] >> 3) << 10 ;

            if(j< pJpegInfo->u16Width/2)
            {
                u16PixVal |= 0x8000;
            }

            pu8Buf[u32DstIdx] = (u16PixVal & 0x00FF) ;
            pu8Buf[u32DstIdx+1] = (u16PixVal & 0xFF00) >> 8;
            u32SrcIdx += 3;
            u32DstIdx += 2;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Buf;
    pJpegInfo->u16PixelSize = 2;
    return 1;
}

bool _CamDrvJpegTransToArgb4444(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Buf;
    u16 i, j, u16PixVal;
    u32 u32SrcIdx, u32DstIdx, u32BufSize;;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 2;
    pu8Buf = malloc(u32BufSize);

    if(pu8Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    u32SrcIdx = 0;
    u32DstIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            u16PixVal = (pJpegInfo->pData[u32SrcIdx] >> 4)  |
                        (pJpegInfo->pData[u32SrcIdx + 1] >> 4) << 4 |
                        (pJpegInfo->pData[u32SrcIdx + 2] >> 4) << 8 ;

            if(j< pJpegInfo->u16Width/2)
            {
                u16PixVal |= 0xF000;
            }
            else
            {
                u16PixVal |= 0x8000;
            }

            pu8Buf[u32DstIdx] = (u16PixVal & 0x00FF) ;
            pu8Buf[u32DstIdx+1] = (u16PixVal & 0xFF00) >> 8;
            u32SrcIdx += 3;
            u32DstIdx += 2;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Buf;
    pJpegInfo->u16PixelSize = 2;
    return 1;
}


bool _CamDrvJpegTransToArgb8888(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Buf;
    u16 i, j;
    u32 u32SrcIdx, u32DstIdx, u32BufSize;;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 4;
    pu8Buf = malloc(u32BufSize);

    if(pu8Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    u32SrcIdx = 0;
    u32DstIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            pu8Buf[u32DstIdx] = pJpegInfo->pData[u32SrcIdx];
            pu8Buf[u32DstIdx+1] = pJpegInfo->pData[u32SrcIdx+1];
            pu8Buf[u32DstIdx+2] = pJpegInfo->pData[u32SrcIdx+2];
            pu8Buf[u32DstIdx+3] = j<pJpegInfo->u16Width/2 ? 0xFF : 0x80;

            u32SrcIdx += 3;
            u32DstIdx += 4;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Buf;
    pJpegInfo->u16PixelSize = 4;
    return 1;
}


bool _CamDrvJpegTransToYuv422_YVYU(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Buf;
    u16 i, j;
    u32 u32SrcIdx, u32DstIdx, u32BufSize;;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 2;
    pu8Buf = malloc(u32BufSize);

    if(pu8Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    u32SrcIdx = 0;
    u32DstIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            pu8Buf[u32DstIdx] = pJpegInfo->pData[u32SrcIdx];
            pu8Buf[u32DstIdx + 1] = (j & 0x01) ?
                                    pJpegInfo->pData[u32SrcIdx + 1] :
                                    pJpegInfo->pData[u32SrcIdx + 2];
            u32SrcIdx += 3;
            u32DstIdx += 2;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Buf;
    pJpegInfo->u16PixelSize = 2;
    return 1;
}


bool _CamDrvJpegTransToYuv420_NV12(CamDrvJpegInfo_t *pJpegInfo)
{
    u8 *pu8Yuv420Buf;
    u16 i, j;
    u8 *pu8DesY = NULL, *pu8DesUV = NULL;;
    u8 *pu8SrcYUV = NULL;
    u32 u32YDesIdx, u32UVDesIdx;
    u32 u32YSrcIdx;
    u32 u32BufSize;

    u32BufSize = pJpegInfo->u16Width * pJpegInfo->u16Height * 3 / 2;
    pu8Yuv420Buf = malloc(u32BufSize);

    if(pu8Yuv420Buf == NULL)
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d: Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        return 0;
    }

    pu8SrcYUV = pJpegInfo->pData;

    pu8DesY = pu8Yuv420Buf;
    pu8DesUV = pu8DesY + pJpegInfo->u16Width * pJpegInfo->u16Height;

    u32UVDesIdx = 0;
    u32YDesIdx = 0;
    u32YSrcIdx = 0;

    for(i=0; i<pJpegInfo->u16Height; i++)
    {
        for(j=0; j<pJpegInfo->u16Width; j++)
        {
            //printf("(%d %d):: SrcY:%d, DstY:%d\n", i, j, u32YSrcIdx,u32YDesIdx);
            pu8DesY[u32YDesIdx++] = pu8SrcYUV[u32YSrcIdx];

            if((i & 0x01) && (j & 0x01))
            {
                //printf("(%d %d)::SrcUV:%d, %d DstUV:%d\n", i, j, u32YSrcIdx+1,u32YSrcIdx+2, u32UVDesIdx);
                pu8DesUV[u32UVDesIdx++] = pu8SrcYUV[u32YSrcIdx+1];
                pu8DesUV[u32UVDesIdx++] = pu8SrcYUV[u32YSrcIdx+2];
            }

            u32YSrcIdx += 3;
        }
    }

    free(pJpegInfo->pData);
    pJpegInfo->pData = pu8Yuv420Buf;
    pJpegInfo->u16PixelSize = 1;
    return 1;
}


bool _CamDrvJpegTransToOutputFmt(CamDrvJpegInfo_t *pJpegInfo)
{
    bool bRet = TRUE;

    switch(pJpegInfo->enOutputcolor)
    {
        case E_CAMDRV_JPEG_OUTPUT_COLOR_RGB888:
            bRet = _CamDrvJpegTransToRgb888(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_RGB565:
            bRet = _CamDrvJpegTransToRgb565(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB1555:
            bRet = _CamDrvJpegTransToArgb1555(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB4444:
            bRet = _CamDrvJpegTransToArgb4444(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB8888:
            bRet = _CamDrvJpegTransToArgb8888(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_YUV444:
            bRet = _CamDrvJpegTransToYuv444(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_YUV420_NV12:
            bRet =_CamDrvJpegTransToYuv420_NV12(pJpegInfo);
            break;

        case E_CAMDRV_JPEG_OUTPUT_COLOR_YUV422_YVYU:
            bRet = _CamDrvJpegTransToYuv422_YVYU(pJpegInfo);
            break;

        default:
            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, Not Support Format: %s\n",
                __FUNCTION__, __LINE__, PARSING_JPEG_OUT_COLOR(pJpegInfo->enOutputcolor));
            bRet = FALSE;
            break;
    }

    CAMDRV_DBG(CAMDRV_DBG_LV_JPEG, "%s %d: %s : %s\n",
        __FUNCTION__, __LINE__,
        PARSING_JPEG_OUT_COLOR(pJpegInfo->enOutputcolor),
        bRet ? "Success" : "Fail");

    return bRet;
}

void _CamDrvLibJpgTrans(CamDrvInputParam_t *pstInputParam)
{
    CamDrvJpegInfo_t stJpegInfo;
    u32 u32Size;

    memset(&stJpegInfo, 0, sizeof(CamDrvJpegInfo_t));

    stJpegInfo.enOutputcolor = pstInputParam->eJpegColor;

    _CamDrvJpegDecompress(&stJpegInfo);

    _CamDrvJpegTransToOutputFmt(&stJpegInfo);

    pstInputParam->u16InputHeight = stJpegInfo.u16Height;
    pstInputParam->u16InputWidth  = stJpegInfo.u16Width;
    pstInputParam->u16InputStide  = stJpegInfo.u16Width * stJpegInfo.u16PixelSize;

    if(pstInputParam->eJpegColor == E_CAMDRV_JPEG_OUTPUT_COLOR_YUV420_NV12)
    {
        u32Size = (pstInputParam->u16InputHeight * pstInputParam->u16InputWidth * 3 / 2);
    }
    else
    {
        u32Size = (pstInputParam->u16InputHeight * pstInputParam->u16InputStide);
    }

    if(gpImageAddr == NULL || gu32ImageSize < u32Size)
    {
        if(gpImageAddr)
        {
            free(gpImageAddr);
        }
        gpImageAddr = malloc(u32Size);

        gu32ImageSize = (u32Size);
    }

    memcpy(gpImageAddr, stJpegInfo.pData, u32Size);
    free(stJpegInfo.pData);

    pstInputParam->u64PhyAddr = (uintptr_t)gpImageAddr - CAMDRV_VIRTUAL_ADDRESS_OFFSET;
}



#endif

CamDrvJpegOutputColorFormat_e _CamDrvParsingJpegOutputColor(char *pColor)
{
    CamDrvJpegOutputColorFormat_e eColor;

    eColor =
        !strcmp(pColor, "yuv444")       ? E_CAMDRV_JPEG_OUTPUT_COLOR_YUV444 :
        !strcmp(pColor, "rgb888")       ? E_CAMDRV_JPEG_OUTPUT_COLOR_RGB888 :
        !strcmp(pColor, "rgb565")       ? E_CAMDRV_JPEG_OUTPUT_COLOR_RGB565 :
        !strcmp(pColor, "argb1555")     ? E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB1555 :
        !strcmp(pColor, "argb4444")     ? E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB4444 :
        !strcmp(pColor, "argb8888")     ? E_CAMDRV_JPEG_OUTPUT_COLOR_ARGB8888 :
        !strcmp(pColor, "yuv420_nv12")  ? E_CAMDRV_JPEG_OUTPUT_COLOR_YUV420_NV12 :
        !strcmp(pColor, "yuv422_yvyu")  ? E_CAMDRV_JPEG_OUTPUT_COLOR_YUV422_YVYU :
                                          E_CAMDRV_JPEG_OUTPUT_COLOR_MAX;
    return eColor;
}

bool _CamDrvParsingHdmiParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_HDMI_PARAM_NUM   6
#define CAMDRV_HDMI_PARAM_START 3

    bool bRet = 0;
    if(argc == CAMDRV_HDMI_PARAM_NUM || argc == (CAMDRV_HDMI_PARAM_NUM+1))
    {
        pstInputParam->u32DispDevIntf =
            !strcmp(argv[CAMDRV_HDMI_PARAM_START-1], "vgahdmi") ?
            (MHAL_DISP_INTF_HDMI) |  MHAL_DISP_INTF_VGA :
            MHAL_DISP_INTF_HDMI;

#if defined(CONFIG_SSTAR_DISP)
        pstInputParam->eDispTiming = _CamDrvTransDispTimingId(argv[CAMDRV_HDMI_PARAM_START]);
#endif
#if defined(CONFIG_SSTAR_HDMITX)
        pstInputParam->eHdmiTiming = _CamDrvTransHdmiTiming(argv[CAMDRV_HDMI_PARAM_START]);
        pstInputParam->eInColor = _CamDrvTransHdmiColor(argv[CAMDRV_HDMI_PARAM_START+1]);
        pstInputParam->eOutColor = _CamDrvTransHdmiColor(argv[CAMDRV_HDMI_PARAM_START+2]);
#endif
        pstInputParam->u32PathId = (argc == (CAMDRV_HDMI_PARAM_NUM+1)) ? simple_strtol(argv[CAMDRV_HDMI_PARAM_START+3], NULL, 10) : 0;
        bRet = 1;
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show hdmi  [Timing] [IN COLOR] [OUT COLOR] [PATH ID]\n");
    }
    return bRet;
}

bool _CamDrvParsingVgaCvbsParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_VGA_CVBS_PARAM_NUM   4
#define CAMDRV_VGA_CVBS_PARAM_START 3

    bool bRet = 0;

    if(argc == CAMDRV_VGA_CVBS_PARAM_NUM || argc == (CAMDRV_VGA_CVBS_PARAM_NUM+1))
    {
        pstInputParam->u32DispDevIntf =
            !strcmp(argv[CAMDRV_VGA_CVBS_PARAM_START-1], "vga") ? MHAL_DISP_INTF_VGA : MHAL_DISP_INTF_CVBS;

        pstInputParam->eDispTiming = _CamDrvTransDispTimingId(argv[CAMDRV_VGA_CVBS_PARAM_START]);

        pstInputParam->u32PathId = (argc == (CAMDRV_VGA_CVBS_PARAM_NUM+1)) ? simple_strtol(argv[CAMDRV_VGA_CVBS_PARAM_START+1], NULL, 10) : 0;
        bRet = 1;
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show vga     [TIMING ID] [PATH ID]\n");
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show cvbs    [TIMING ID] [PATH ID]\n");
    }
    return bRet;
}

bool _CamDrvGetPnlParma(char *pPnlName, CamDrvInputParam_t *pstInputParam)
{
    bool bRet = 1;
    if( !strcmp(pPnlName, "ttl1024x600") )
    {
        pstInputParam->pPnlUnfiedParam = &gstPnlUnfiedParam_Ttl1024x600;
    }
    else
    {
        pstInputParam->pPnlUnfiedParam = NULL;
        bRet = 0;
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, UnSupport Pnl:%s\n",
            __FUNCTION__, __LINE__, pPnlName);
    }
    return bRet;
}

bool _CamDrvParsingPnlParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_PNL_PARAM_NUM    4
#define CAMDRV_PNL_PARAM_START  3

    bool bRet = 0;
    if(argc == CAMDRV_PNL_PARAM_NUM || argc == (CAMDRV_PNL_PARAM_NUM+1))
    {
        pstInputParam->u32DispDevIntf =
            !strcmp(argv[CAMDRV_PNL_PARAM_START-1], "ttl") ? MHAL_DISP_INTF_TTL : MHAL_DISP_INTF_MIPIDSI;

        pstInputParam->eDispTiming = E_MHAL_DISP_OUTPUT_USER;
        pstInputParam->u32PathId = (argc == CAMDRV_PNL_PARAM_NUM+1) ? simple_strtol(argv[CAMDRV_PNL_PARAM_START+1], NULL, 10) : 0;
        bRet = _CamDrvGetPnlParma(argv[CAMDRV_PNL_PARAM_START], pstInputParam);
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show ttl     [PNL NAME] [PATH ID]\n");
    }
    return bRet;
}

bool _CamDrvParsingMopParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_MOP_PARAM_NUM    6
#define CAMDRV_MOP_PARAM_START  3
    bool bRet = 0;


    if(argc == CAMDRV_MOP_PARAM_NUM || argc == (CAMDRV_MOP_PARAM_NUM+1))
    {
        pstInputParam->u64PhyAddr = (uintptr_t)gpImageAddr - CAMDRV_VIRTUAL_ADDRESS_OFFSET;
        pstInputParam->u16OutputWidth  = simple_strtol(argv[CAMDRV_MOP_PARAM_START], NULL, 10);
        pstInputParam->u16OutputHeight = simple_strtol(argv[CAMDRV_MOP_PARAM_START+1], NULL, 10);
        pstInputParam->eJpegColor = _CamDrvParsingJpegOutputColor(argv[CAMDRV_MOP_PARAM_START+2]);
        pstInputParam->u32PathId = (argc == CAMDRV_MOP_PARAM_NUM+1) ? simple_strtol(argv[CAMDRV_MOP_PARAM_START+3], NULL, 10) : 0;
        bRet = pstInputParam->eJpegColor == E_CAMDRV_JPEG_OUTPUT_COLOR_MAX ? 0 :1;
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show mop [DISP WIDHT][DISP HEIGHT] [COLOR FMT] [PATH ID]\n");
    }
    return bRet;
}

bool _CamDrvParsingGopParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_GOP_PARAM_NUM    6
#define CAMDRV_GOP_PARAM_START  3
    bool bRet = 0;


    if(argc == CAMDRV_GOP_PARAM_NUM || argc == (CAMDRV_GOP_PARAM_NUM+1))
    {
        pstInputParam->u16OutputWidth  = simple_strtol(argv[CAMDRV_GOP_PARAM_START], NULL, 10);
        pstInputParam->u16OutputHeight = simple_strtol(argv[CAMDRV_GOP_PARAM_START+1], NULL, 10);
        pstInputParam->eJpegColor = _CamDrvParsingJpegOutputColor(argv[CAMDRV_GOP_PARAM_START+2]);
        pstInputParam->u32PathId = (argc == CAMDRV_GOP_PARAM_NUM+1) ? simple_strtol(argv[CAMDRV_GOP_PARAM_START+3], NULL, 10) : 0;
        bRet = pstInputParam->eJpegColor == E_CAMDRV_JPEG_OUTPUT_COLOR_MAX ? 0 :1;
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show gop [DISP WIDHT] [DISP HEIGHT] [COLOR FMT] [PATH ID]\n");
    }
    return bRet;
}


bool _CamDrvParsingJpegParam(int argc, char *const argv[], CamDrvInputParam_t *pstInputParam)
{
#define CAMDRV_JPEG_PARAM_NUM    4
#define CAMDRV_JPEG_PARAM_START  3
    bool bRet = 0;


    if(argc == CAMDRV_JPEG_PARAM_NUM)
    {
        pstInputParam->eJpegColor =_CamDrvParsingJpegOutputColor(argv[CAMDRV_JPEG_PARAM_START]);
        bRet = pstInputParam->eJpegColor == E_CAMDRV_JPEG_OUTPUT_COLOR_MAX ? 0 :1;
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "camdriver show jpeg [OUTPUT COLOR]\n");
    }
    return bRet;
}


bool _CamDrvSetDbgmg(int argc, char *const argv[])
{
    bool bRet = 0;
    u32 u32DbgLevel;

    if(argc == 4)
    {

        u32DbgLevel =  simple_strtol(argv[3], NULL, 16);

        if(!strcmp(argv[2], "disp"))
        {
        #if defined(CONFIG_SSTAR_DISP)
            MHAL_DISP_DbgLevel(&u32DbgLevel);
            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "disp %x\n", u32DbgLevel);
            bRet = 1;
        #endif
        }
        else if(!strcmp(argv[2], "pnl"))
        {
        #if defined(CONFIG_SSTAR_PNL)
            MhalPnlSetDebugLevel(&u32DbgLevel);

            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "pnl %x\n", u32DbgLevel);
            bRet = 1;
        #endif
        }
        else if(!strcmp(argv[2], "hdmitx"))
        {
        #if defined(CONFIG_SSTAR_HDMITX)
            _CamDrvHdmitxCreateInstance();
            MhalHdmitxSetDebugLevel(gstCamDrvHdmitxCtx.pHdmitxCtx, u32DbgLevel);
            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "hdmitx %x\n", u32DbgLevel);
            bRet = 1;
        #endif
        }
    }
    else
    {
        CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "dbgmg [MODULE] [LEVEL]\n");
    }
    return bRet;
}


void _CamDrvCtxInit(void)
{
    static bool bInit = 0;

    if(!bInit)
    {
        memset(&gstCamDrvDispCtx, 0, sizeof(CamDrvDispContext_t) * CAMDRV_DISP_DEVICE_MAX);
        memset(&gstCamDrvHdmitxCtx, 0, sizeof(CamDrvHdmiTxContext_t));
        memset(&gstCamDrvPnlCtx, 0, sizeof(CamDrvPnlContext_t)*CAMDRV_DISP_DEVICE_MAX);
        bInit = 1;
    }
}

void _CamDrvParsingShowParam(int argc, char * const argv[], CamDrvInputParam_t *pstInputParam)
{
    CamDrvModuleType_e eModule = E_CAMDRV_MODULE_NONE;

    if(argc >= 2)
    {
        if(!strcmp(argv[2], "hdmi"))
        {
            if(_CamDrvParsingHdmiParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_HDMITX | E_CAMDRV_MODULE_DISP;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else if(!strcmp(argv[2], "vga") || !strcmp(argv[2], "cvbs"))
        {
            if(_CamDrvParsingVgaCvbsParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_DISP;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }

        }
        else if(!strcmp(argv[2], "ttl") || !strcmp(argv[2], "mipidsi"))
        {
            if(_CamDrvParsingPnlParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_PNL | E_CAMDRV_MODULE_DISP;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else if(!strcmp(argv[2], "mop"))
        {
            if(_CamDrvParsingMopParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_MOP | E_CAMDRV_MODULE_JPEG;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else if(!strcmp(argv[2], "gop"))
        {
            if(_CamDrvParsingGopParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_GOP | E_CAMDRV_MODULE_JPEG;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else if(!strcmp(argv[2], "jpeg"))
        {
            if(_CamDrvParsingJpegParam(argc, argv, pstInputParam))
            {
                eModule = E_CAMDRV_MODULE_JPEG;
            }
            else
            {
                CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "%s %d, ParsingCmd Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "Un-Support CMD: %s\n", argv[1]);
        }
    }

    if(eModule != E_CAMDRV_MODULE_NONE)
    {
        _CamDrvCtxInit();

        if(eModule & E_CAMDRV_MODULE_DISP)
        {
        #if defined(CONFIG_SSTAR_DISP)
            _CamDrvDispShow(pstInputParam);
        #endif
        }

        if(eModule & E_CAMDRV_MODULE_PNL)
        {
        #if defined(CONFIG_SSTAR_PNL)
            _CamDrvPnlShow(pstInputParam);
        #endif
        }

        if(eModule & E_CAMDRV_MODULE_HDMITX)
        {
        #if defined(CONFIG_SSTAR_HDMITX)
            _CamDrvHdmitxShow(pstInputParam);
        #endif
        }

        if(eModule & E_CAMDRV_MODULE_JPEG)
        {
        #if defined(CONFIG_SSTAR_JPD)
            _CamDrvLibJpgTrans(pstInputParam);
        #endif
        }

        if(eModule & E_CAMDRV_MODULE_MOP)
        {
        #if defined(CONFIG_SSTAR_DISP)
            _CamDrvDispMop(pstInputParam);
        #endif
        }

        if(eModule & E_CAMDRV_MODULE_GOP)
        {
        #if defined(CONFIG_SSTAR_RGN)
            _CamDrvRgnGop(pstInputParam);
        #endif
        }
    }
}

int do_camdriver (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    CamDrvInputParam_t stInputParam;

    memset(&stInputParam, 0, sizeof(CamDrvInputParam_t));
    if(argc >= 2)
    {
        if(!strcmp(argv[1], "show"))
        {
            _CamDrvParsingShowParam(argc, argv, &stInputParam);
        }
        else if(!strcmp(argv[1], "dbgmg"))
        {
            _CamDrvSetDbgmg(argc, argv);
        }
        else
        {
            CAMDRV_DBG(CAMDRV_DBG_LV_ERR, "UnSuporot Cmd: %s\n", argv[1]);
        }
    }

    return 0;
}

U_BOOT_CMD(
    camdriver, CONFIG_SYS_MAXARGS, 1,    do_camdriver,
    "camdrvier test",
    NULL
);

