/*
* drv_hdmitx_ctx.h- Sigmastar
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

#ifndef _DRV_HDMITX_CTX_H_
#define _DRV_HDMITX_CTX_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

typedef struct DrvHdmitxCtxHalContain_s
{
    u8                           bInit;
    u8                           bAvMute;
    u8                           bVideoOnOff;
    u8                           bAudioOnOff;
    u8                           bSignal;
    HalHdmitxStatusFlag_e        enStatusFlag;
    HalHdmitxHpdConfig_t         stHpdCfg;
    HalHdmitxAnaloDrvCurConfig_t stAnalogDrvCurCfg;
    HalHdmitxAttrConfig_t        stAttrCfg;
    HalHdmitxInfoFrameConfig_t   stInfoFrameCfg;
} DrvHdmitxCtxHalContain_t;

typedef struct DrvHdmitxCtxConfig_s
{
    u8                        bUsed;
    s32                       s32CtxId;
    s32                       s32HdmitxId;
    DrvHdmitxOsSemConfig_t    stSem;
    DrvHdmitxCtxHalContain_t *pstMhalCtx;
    HalHdmitxStatus_t         _hdmitx_status;
    s32                       s32HDMITxEventId;
    DrvHdmitxOsTaskConfig_t   stHDMITxTask;
    DrvHdmitxOsTimerConfig_t  stCheckRxTimerCfg;
    s32                       s32HDMITxTaskId;
    u8                        bCheckEDID;
    u8                        u8SupTimingNum;
    void *                    pstHwCtx; // HalHdmitxResourcePrivate_t
} DrvHdmitxCtxConfig_t;

typedef struct DrvHdmitxCtxAllocConfig_s
{
    s32 s32HdmitxId;
} DrvHdmitxCtxAllocConfig_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _DRV_HDMITX_CTX_C_
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE u8 DrvHdmitxCtxInit(void);
INTERFACE u8 DrvHdmitxCtxDeInit(void);
INTERFACE u8 DrvHdmitxCtxAllocate(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx);
INTERFACE u8 DrvHdmitxCtxFree(DrvHdmitxCtxConfig_t *pCtx);
INTERFACE u8 DrvHdmitxCtxIsAllFree(void);
INTERFACE u8 DrvHdmitxCtxGet(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx);

#undef INTERFACE

#endif
