/*
* hal_disp_picture_if.h- Sigmastar
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

#ifndef _HAL_DISP_PICTURE_IF_H_
#define _HAL_DISP_PICTURE_IF_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_PICTURE_IF_SHARPNESS_NUM   2

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_PICTURE_CONTRAST = 0,
    E_HAL_DISP_PICTURE_R_BRIGHTNESS,
    E_HAL_DISP_PICTURE_G_BRIGHTNESS,
    E_HAL_DISP_PICTURE_B_BRIGHTNESS,
    E_HAL_DISP_PICTURE_SATURATION,
    E_HAL_DISP_PICTURE_SHARPNESS,
    E_HAL_DISP_PICTURE_SHARPNESS1,
    E_HAL_DISP_PICTURE_HUE,
    E_HAL_DISP_PICTURE_NUM,
}HalDispPictureIfType_e;


typedef struct
{
    u16 u16OSD_0;
    u16 u16OSD_25;
    u16 u16OSD_50;
    u16 u16OSD_75;
    u16 u16OSD_100;
}HalDispPictureIfNonLinearCurveType_t;

typedef struct
{
    HalDispPictureIfNonLinearCurveType_t stDispDevice0[E_HAL_DISP_PICTURE_NUM];
    HalDispPictureIfNonLinearCurveType_t stDispDevice1[E_HAL_DISP_PICTURE_NUM];
}HalDispPictureIfNonLinearCurveConfig_t;

typedef struct
{
    u16 u16Hue;
    u16 u16Saturation;
    u16 u16Contrast;
    u16 u16BrightnessR;
    u16 u16BrightnessG;
    u16 u16BrightnessB;
    u16 u16Sharpness[HAL_DISP_PICTURE_IF_SHARPNESS_NUM];
}HalDispPictureConfig_t;

typedef struct
{
    bool bUpdate;
    u16 u16GainR;
    u16 u16GainG;
    u16 u16GainB;
    u16 u16BrightnessR;
    u16 u16BrightnessG;
    u16 u16BrightnessB;
    u16 u16Hue;
    u16 u16Saturation;
    u16 u16Contrast;
    u16 u16Sharpness;
    u16 u16Sharpness1;
    s16 as16Coef[9];
}HalDispPicturePqConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_PICTURE_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void HalDispPictureIfSetConfig(HalDispCscmatrix_e eCscMatrix, HalDispPictureConfig_t *pstPictureCfg, void *pCtx);
INTERFACE bool HalDispPictureIfTransNonLinear(void *pCtx, HalDispCsc_t *pstCsc, u32 *pu32Sharpness, HalDispPictureConfig_t *pstPictureCfg);
INTERFACE void HalDispPictureIfSetPqConfig(void *pCtx);


#undef INTERFACE
#endif
