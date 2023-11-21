/*
* hal_disp_mace.h- Sigmastar
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

#ifndef _HAL_DISP_MACE_H_
#define _HAL_DISP_MACE_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_MACE_FLAG_NONE      = 0x0000,
    E_HAL_DISP_MACE_FLAG_BW        = 0x0001,
    E_HAL_DISP_MACE_FLAG_LPF       = 0x0002,
    E_HAL_DISP_MACE_FLAG_HCORING   = 0x0004,
    E_HAL_DISP_MACE_FLAG_PEAKING   = 0x0008,
    E_HAL_DISP_MACE_FLAG_FCC       = 0x0010,
    E_HAL_DISP_MACE_FLAG_DLC_DCR   = 0x0020,
    E_HAL_DISP_MACE_FLAG_GAMMA     = 0x0040,
    E_HAL_DISP_MACE_FLAG_3X3MATRIX = 0x0080,
    E_HAL_DISP_MACE_FLAG_PICTURE   = 0x0100,
} HalDispMaceFlag_e;

typedef enum
{
    E_HAL_DISP_MACE_PICTURE_SRC_HDMI = 0x0001,
    E_HAL_DISP_MACE_PICTURE_SRC_VGA  = 0x0002,
    E_HAL_DISP_MACE_PICTURE_SRC_LCD  = 0x0004,
} HalDispMacePictureSourceType_e;
//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u8 bUpdate;
    u8 u8Bw2sbri;
    u8 u8BleEn;
    u8 u8WleEn;
    u8 u8BlackStart;
    u8 u8BlackSlop;
    u8 u8WhiteStart;
    u8 u8WhiteSlop;
} HalDispMaceBwExtensionConfig_t;

typedef struct
{
    u8 bUpdate;
    u8 u8LpfY;
    u8 u8LpfC;
} HalDispMaceLpfConfig_t;

typedef struct
{
    u8  bUpdate;
    u16 u16DcrEn;
    u16 u16DcrOffset;

    u16 u16Hactive;
    u16 u16Vactive;
    u16 u16Hblank;

    u16 u16PwmDuty;
    u16 u16PwmPeriod;

    u16 u16DlcEn;
    u16 u16DlcOffset;
    u16 u16DlcGain;
    u16 u16YGain;
    u16 u16YGainOffset;
} HalDispMaceDlcDcrConfig_t;

typedef struct
{
    u8 bUpdate;
    u8 u8YBand1HCoringEn;
    u8 u8YBand2HCoringEn;
    u8 u8HCoringYDither_En;
    u8 u8YTableStep;

    u8 u8CBand1HCoringEn;
    u8 u8CBand2HCoringEn;
    u8 u8HCoringCDither_En;
    u8 u8CTableStep;

    u8 u8PcMode;
    u8 u8HighPassEn;
    u8 u8CoringTable0;
    u8 u8CoringTable1;
    u8 u8CoringTable2;
    u8 u8CoringTable3;
} HalDispMaceHCoringConfig_t;

typedef struct
{
    u8 bUpdate;

    u8 u8LtiEn;
    u8 u8LtiCoef;
    u8 u8LtiStep;
    u8 u8LtiMediaFilterOn;

    u8 u8CtiEn;
    u8 u8CtiCoef;
    u8 u8CtiStep;
    u8 u8CtiMediaFilterOn;

    u8 u8DiffAdapEn;
    u8 u8PosLimitTh;
    u8 u8NegLimitTh;

    u8 u8PeakingEn;

    u8 u8Band1Coef;
    u8 u8Band1Step;
    u8 u8Band1DiffAdapEn;
    u8 u8Band1PosLimitTh;
    u8 u8Band1NegLimitTh;

    u8 u8Band2Coef;
    u8 u8Band2Step;
    u8 u8Band2DiffAdapEn;
    u8 u8Band2PosLimitTh;
    u8 u8Band2NegLimitTh;

    u8 u8CoringTh1;
    u8 u8CoringTh2;

    u8 u8PeakingTerm1Sel;
    u8 u8PeakingTerm2Sel;
    u8 u8PeakingTerm3Sel;
    u8 u8PeakingTerm4Sel;
} HalDispMacePeakingConfig_t;

typedef struct
{
    u8 bUpdate;
    u8 u8Cb_T1;
    u8 u8Cr_T1;
    u8 u8Cb_T2;
    u8 u8Cr_T2;
    u8 u8Cb_T3;
    u8 u8Cr_T3;
    u8 u8Cb_T4;
    u8 u8Cr_T4;
    u8 u8Cb_T5;
    u8 u8Cr_T5;
    u8 u8Cb_T6;
    u8 u8Cr_T6;
    u8 u8Cb_T7;
    u8 u8Cr_T7;
    u8 u8Cb_T8;
    u8 u8Cr_T8;
    u8 u8CbCr_D1D_D1U;
    u8 u8CbCr_D2D_D2U;
    u8 u8CbCr_D3D_D3U;
    u8 u8CbCr_D4D_D4U;
    u8 u8CbCr_D5D_D5U;
    u8 u8CbCr_D6D_D6U;
    u8 u8CbCr_D7D_D7U;
    u8 u8CbCr_D8D_D8U;
    u8 u8CbCr_D9;
    u8 u8K_T2_K_T1;
    u8 u8K_T4_K_T3;
    u8 u8K_T6_K_T5;
    u8 u8K_T8_K_T7;
    u8 u8En;
} HalDispMaceFccConfig_t;

typedef struct
{
    u8 bUpdate;
    u8 u8En;
    u8 u8R[33];
    u8 u8G[33];
    u8 u8B[33];
} HalDispMaceGammaConfig_t;

typedef struct
{
    u8  bUpdate;
    u8  u8Id;
    u8  bEn;
    u8  u8RangeR;
    u8  u8RangeG;
    u8  u8RangeB;
    u16 u16Coef11;
    u16 u16Coef12;
    u16 u16Coef13;
    u16 u16Coef21;
    u16 u16Coef22;
    u16 u16Coef23;
    u16 u16Coef31;
    u16 u16Coef32;
    u16 u16Coef33;
} HalDispMace3x3MatrixConfig_t;

typedef struct
{
    u8                             bUpdate;
    HalDispMacePictureSourceType_e enSrcType;
    u16                            u16Hue;
    u16                            u16Saturation;
    u16                            u16RGain;
    u16                            u16GGain;
    u16                            u16BGain;
    u16                            u16Contrast;
    u16                            u16BrightnessR;
    u16                            u16BrightnessG;
    u16                            u16BrightnessB;
} HalDispMacePictureConfig_t;

typedef struct
{
    HalDispMaceFlag_e              enFlag;
    HalDispMaceBwExtensionConfig_t stBwExtCfg;
    HalDispMaceLpfConfig_t         stLpfCfg;
    HalDispMaceDlcDcrConfig_t      stDlcDcrCfg;
    HalDispMaceHCoringConfig_t     stHCoringCfg;
    HalDispMacePeakingConfig_t     stPeakingCfg;
    HalDispMaceFccConfig_t         stFccCfg;
    HalDispMaceGammaConfig_t       stGammaCfg;
    HalDispMace3x3MatrixConfig_t   st3x3MatrixCfg;
} HalDispMaceHwContext_t;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_MACE_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void HalDispMaceSetGammaConfig(HalDispMaceGammaConfig_t *pstGammaCfg, void *pCtx);
INTERFACE void HalDispMaceSetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE void HalDispGammaSetEn(void *pCtx, u8 u8bEn, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode);

#undef INTERFACE
#endif
