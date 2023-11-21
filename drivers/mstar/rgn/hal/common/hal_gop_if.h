/*
* hal_gop_if.h- Sigmastar
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


#ifndef __HAL_GOP_IF_H__
#define __HAL_GOP_IF_H__
#include "hal_rgn_common.h"
//=============================================================================
// enum
//=============================================================================
typedef struct
{
    u16 u16Htotal;
    u16 u16Vtotal;
    u16 u16HdeStart;
    u16 u16VdeStart;
    u16 u16Width;
    u16 u16Height;
    bool bInterlaceMode;
    bool bYuvOutput;
} HalGopVideoTimingInfoConfig_t;

typedef enum
{
    E_HAL_GOP_DISPLAY_MODE_INTERLACE = 0x00,
    E_HAL_GOP_DISPLAY_MODE_PROGRESS  = 0x01,
} HalGopDisplayModeType_e;

typedef enum
{
    E_HAL_GOP_OUT_FMT_RGB = 0x00,
    E_HAL_GOP_OUT_FMT_YUV = 0x01,
} HalGopOutFormatType_e;

typedef enum
{
    E_HAL_GOP_ALLBANK_DOUBLE_WR = GOP_BANK_ALLBANK_DOUBLE_WR,
    E_HAL_GOP_FORCE_WR = GOP_BANK_FORCE_WR,
    E_HAL_GOP_ONEBANK_DOUBLE_WR = GOP_BANK_ONEBANK_DOUBLE_WR,
} HalGopWrRegType_e;

typedef enum
{
    E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA0 = 0,
    E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA1 = 1,
} HalGopGwinArgb1555Def_e;
typedef enum
{
    E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA0 = 0,
    E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA1 = 1,
    E_HAL_GOP_GWIN_ALPHA_DEFINE_CONST = 2,
} HalGopGwinAlphaDef_e;
typedef enum
{
     E_HAL_QUERY_GOP_OK = 0,
     E_HAL_QUERY_GOP_CFGERR,
     E_HAL_QUERY_GOP_SIZEERR,
     E_HAL_QUERY_GOP_NOTSUPPORT,
     E_HAL_QUERY_GOP_NOTBIND,
     E_HAL_QUERY_GOP_NONEED,
}HalGopQueryRet_e;
 typedef enum
 {
        E_HAL_QUERY_SETVIDEOTIMINGINFO,
        E_HAL_QUERY_GOP_MAX
 } HalGopQueryFunction_e;
 //=============================================================================
 // struct
 //=============================================================================
typedef struct
{
    HalGopQueryFunction_e enQF;
    HalGopIdType_e eGopId;
    void *pCfg;
    u32 u32CfgSize;
} HalGopQueryInputConfig_t;
typedef struct
{
    HalGopQueryRet_e enQueryRet;
    void (*pfForSet)(HalGopIdType_e ,void *); // pfForSet(*pCtx,*pCfg)
    void *pPatch; // if need patch
    void (*pfForPatch)(HalGopIdType_e ,void *); // pfForPatch(*pCtx,*pPatch)
} HalGopQueryOutputConfig_t;
typedef struct
{
    HalGopQueryInputConfig_t stInQ;
    HalGopQueryOutputConfig_t stOutQ;
} HalGopQueryFuncConfig_t;

typedef struct
{
    u16 u16X;
    u16 u16Y;
    u16 u16Width;
    u16 u16Height;
} HalGopWindowType_t;
typedef struct
{
    u8 u8ConstantAlpahValue;
    u8 u8Argb1555Alpha0Val; // Be use when register alpha 0 not equal register constant alpha.
    u8 u8Argb1555Alpha1Val;
}HalGopGwinAlphaVal_t;
typedef struct
{
    HalGopDisplayModeType_e eDisplayMode;
    HalGopOutFormatType_e eOutFormat;
    HalGopWindowType_t tStretchWindow;
} HalGopParamConfig_t;

typedef struct
{
    HalGopGwinSrcFormat_e eSrcFmt;
    HalGopWindowType_t tDispWindow;
    u32 u32BaseAddr;
    u16 u16Base_XOffset;
    u32 u32Base_YOffset;
} HalGopGwinParamConfig_t;
typedef struct
{
    u8 u8Idx;
    u8 u8A;
    u8 u8R;
    u8 u8G;
    u8 u8B;
} HalGopPaletteConfig_t;
typedef struct
{
    bool bEn;
    u8 u8R;
    u8 u8G;
    u8 u8B;
} HalGopColorKeyConfig_t;
typedef struct
{
    HalGopIdType_e eGopId;
    HalGopGwinIdType_e eGwinId;
    u32 u32Reg;
    u32 u16Val;
    u32 u16Msk;
    u32 u32RegType;
    HalRgnCmdqIdType_e eCmdqId;
} HalGopShadowConfig_t;

//=============================================================================

//=============================================================================
#ifndef __HAL_GOP_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool HalGopQuery(void *pQF);
INTERFACE void HalGopInit(void);
INTERFACE void HalGopGetClkNum(HalGopIdType_e eGopId, u32 *u32clknum);
INTERFACE void HalGopSetAlphaPointVal(HalGopGwinAlphaDef_e enType,HalGopGwinAlphaVal_t *tAlphaVal,u8 **p8Alpha);
INTERFACE bool HalGopSetClkEnable(HalGopIdType_e eGopId);
INTERFACE bool HalGopSetClkDisable(HalGopIdType_e eGopId,bool bEn);
INTERFACE void HalGopMiuSelect(HalGopIdType_e eGopId, u32 *u32BaseAddr);
INTERFACE void HalGopUpdateBaseXoffset(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, u16 u16Xoffset);
INTERFACE void HalGopUpdateBase(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, u32 u32BaseAddr);
INTERFACE void HalGopSetGwinMemPitchDirect(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, u32 u32MemPitch);
INTERFACE void HalGopUpdateParam(HalGopIdType_e eGopId, HalGopParamConfig_t *ptParamCfg);
INTERFACE bool HalGopCheckIdSupport(HalGopIdType_e eGopId);
INTERFACE void HalGopUpdateGwinParam(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId,HalGopGwinParamConfig_t *ptParamCfg);
INTERFACE void HalGopSetStretchWindowSize(HalGopIdType_e eGopId, HalGopWindowType_t *ptSrcWinCfg, HalGopWindowType_t *ptDstWinCfg);
INTERFACE void HalGopSetGwinSize(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopWindowType_t *ptGwinCfg, HalGopGwinSrcFormat_e eSrcFmt);
INTERFACE void HalGopSetGwinSrcFmt(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopGwinSrcFormat_e eSrcFmt);
INTERFACE void HalGopSetAlphaBlending(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, bool bConstantAlpha, u8 Alpha);
INTERFACE void HalGopSetColorKey(HalGopIdType_e eGopId, HalGopColorKeyConfig_t *ptCfg, bool bVYU);
INTERFACE void HalGopSetPaletteRiu(HalGopIdType_e eGopId, HalGopPaletteConfig_t *ptCfg);
INTERFACE void HalGopSetEnableGwin(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, bool bEn);
INTERFACE void HalGopBindOsd(HalGopIdType_e eGopId);
INTERFACE void HalGopEnsureIdle(HalGopIdType_e eGopId);
INTERFACE void HalGopSetArgb1555Alpha(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopGwinArgb1555Def_e eAlphaType, u8 eAlphaVal);
INTERFACE void HalGopSetAlphaInvert(HalGopIdType_e eGopId, bool bInv);
INTERFACE bool HalGopCheckAlphaZeroOpaque(HalGopIdType_e eGopId,bool bEn,bool bConAlpha,HalGopGwinSrcFormat_e eFmt);

#undef INTERFACE
#endif /* __HAL_GOP_H__ */
