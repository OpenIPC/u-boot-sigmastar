/*
* hal_disp_picture_if.c- Sigmastar
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

#define _HAL_DISP_PICTURE_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_color.h"
#include "hal_disp_reg.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "hal_disp_picture_if.h"
#include "hal_disp_mace.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
extern HalDispInternalConfig_t g_stInterCfg[HAL_DISP_DEVICE_MAX];

HalDispPictureIfNonLinearCurveConfig_t g_stPictureNonLinearSetting[HAL_DISP_DEVICE_MAX] = {
    {
        // DispDevice0
        {
            {0, 80, 128, 160, 255}, // Contrast
            {0, 64, 128, 192, 255}, // BrightnessR
            {0, 64, 128, 192, 255}, // BrightnessG
            {0, 64, 128, 192, 255}, // BrightnessB
            {0, 96, 128, 160, 255}, // Saturation
            {0, 8, 24, 44, 63},     // Sharpness
            {0, 8, 24, 44, 63},     // Sharpness1
            {30, 40, 50, 60, 100},  // Hue
        },
    },

    {
        // DispDevice1
        {
            {0, 80, 128, 160, 255}, // Contrast
            {0, 64, 128, 192, 255}, // BrightnessR
            {0, 64, 128, 192, 255}, // BrightnessG
            {0, 64, 128, 192, 255}, // BrightnessB
            {0, 96, 128, 160, 255}, // Saturation
            {0, 8, 24, 44, 63},     // Sharpness
            {0, 8, 24, 44, 63},     // Sharpness1
            {30, 40, 50, 60, 100},  // Hue
        },
    },

    {// DispDevice2
     {
         {0, 80, 128, 160, 255}, // Contrast
         {0, 64, 128, 192, 255}, // BrightnessR
         {0, 64, 128, 192, 255}, // BrightnessG
         {0, 64, 128, 192, 255}, // BrightnessB
         {0, 96, 128, 160, 255}, // Saturation
         {0, 8, 24, 44, 63},     // Sharpness
         {0, 8, 24, 44, 63},     // Sharpness1
         {30, 40, 50, 60, 100},  // Hue
     }},
};

HalDispPicturePqConfig_t g_stPicturePqCfg[HAL_DISP_DEVICE_MAX] = {
    {0x0000,
     0x0032,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0018,
     0x0018,
     {0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0400}},

    {0x0000,
     0x0032,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0018,
     0x0018,
     {0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0400}},

    {0x0000,
     0x0032,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0080,
     0x0018,
     0x0018,
     {0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0400}},
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static u16 _HalDispPictureIfCalculateNonLInear(HalDispPictureIfNonLinearCurveType_t *pNonLinearCurve, u8 u8AdjustValue)
{
    u16 u16Value, u16Y0, u16Y1, u16X0, u16X1, u16Intercept;
    u16 u16DistanceOfY, u16DistanceOfX;

    if (pNonLinearCurve == NULL)
    {
        return 0;
    }

    if (u8AdjustValue < 25)
    {
        u16Y0 = pNonLinearCurve->u16OSD_0;
        u16Y1 = pNonLinearCurve->u16OSD_25;
        u16X0 = 0;
        u16X1 = 25;
    }
    else if (u8AdjustValue < 50)
    {
        u16Y0 = pNonLinearCurve->u16OSD_25;
        u16Y1 = pNonLinearCurve->u16OSD_50;
        u16X0 = 25;
        u16X1 = 50;
    }
    else if (u8AdjustValue < 75)
    {
        u16Y0 = pNonLinearCurve->u16OSD_50;
        u16Y1 = pNonLinearCurve->u16OSD_75;
        u16X0 = 50;
        u16X1 = 75;
    }
    else
    {
        u16Y0 = pNonLinearCurve->u16OSD_75;
        u16Y1 = pNonLinearCurve->u16OSD_100;
        u16X0 = 75;
        u16X1 = 100;
    }

    if (u16Y1 > u16Y0)
    {
        u16DistanceOfY = u16Y1 - u16Y0;
        u16DistanceOfX = u16X1 - u16X0;
        u16Intercept   = u16Y0;
        u8AdjustValue  = u8AdjustValue - u16X0;
    }
    else
    {
        u16DistanceOfY = u16Y0 - u16Y1;
        u16DistanceOfX = u16X1 - u16X0;
        u16Intercept   = u16Y1;
        u8AdjustValue  = u16X1 - u8AdjustValue;
    }

    u16Value = (u16DistanceOfY * u8AdjustValue / (u16DistanceOfX)) + u16Intercept;
    return u16Value;
}

static void _HalDispPictureIfUpdateNoneLinearCurveOsd50(HalDispPictureIfNonLinearCurveType_t *pstNonLinerCurve,
                                                        u16                                   u16Osd50)
{
    pstNonLinerCurve->u16OSD_25 = (u16Osd50 > pstNonLinerCurve->u16OSD_0)
                                      ? u16Osd50 - (u16Osd50 - pstNonLinerCurve->u16OSD_0) / 2
                                      : u16Osd50 + (pstNonLinerCurve->u16OSD_0 - u16Osd50) / 2;

    pstNonLinerCurve->u16OSD_50 = u16Osd50;

    pstNonLinerCurve->u16OSD_75 = (pstNonLinerCurve->u16OSD_100 > u16Osd50)
                                      ? (pstNonLinerCurve->u16OSD_100 - u16Osd50) / 2 + u16Osd50
                                      : u16Osd50 - (u16Osd50 - pstNonLinerCurve->u16OSD_100) / 2;

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, (%d %d %d %d %d)\n", __FUNCTION__, __LINE__, pstNonLinerCurve->u16OSD_0,
             pstNonLinerCurve->u16OSD_25, pstNonLinerCurve->u16OSD_50, pstNonLinerCurve->u16OSD_75,
             pstNonLinerCurve->u16OSD_100);
}

static HalDispPictureIfNonLinearCurveType_t *_HalDispIfGetPictureNonLinearCurve(u32                    u32DevId,
                                                                                HalDispPictureIfType_e enType)
{
    HalDispPictureIfNonLinearCurveType_t *pNonLinearCruve = NULL;

    pNonLinearCruve = (u32DevId < HAL_DISP_DEVICE_MAX && enType < E_HAL_DISP_PICTURE_NUM)
                          ? &g_stPictureNonLinearSetting[u32DevId].stDispDevice[enType]
                          : NULL;
    return pNonLinearCruve;
}

u8 HalDispPictureIfTransNonLinear(void *pCtx, HalDispCsc_t *pstCsc, u32 *pu32Sharpness,
                                  HalDispPictureConfig_t *pstPictureCfg)
{
    DrvDispCtxConfig_t *                  pstDispCtxCfg     = NULL;
    DrvDispCtxDeviceContain_t *           pstDevContain     = NULL;
    HalDispPictureIfNonLinearCurveType_t *pstNonLinearCurve = NULL;
    u8                                    bRet              = 1;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];

    if (pstCsc)
    {
        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_CONTRAST);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16Contrast =
                _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Contrast);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Contrast NonLinearCurve NULL\n", __FUNCTION__, __LINE__,
                     pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_HUE);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16Hue = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Hue);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Hue NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SATURATION);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16Saturation =
                _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Saturation);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Saturation NonLinearCurve NULL\n", __FUNCTION__, __LINE__,
                     pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve =
            _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_R_BRIGHTNESS);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessR = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Bri_R NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve =
            _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_G_BRIGHTNESS);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessG = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Bri_G NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve =
            _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_B_BRIGHTNESS);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessB = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Bri_B NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Con:%d, Bri:(%d %d %d) Hue:%d Sat:%d\n", __FUNCTION__, __LINE__,
                 pstPictureCfg->u16Contrast, pstPictureCfg->u16BrightnessR, pstPictureCfg->u16BrightnessG,
                 pstPictureCfg->u16BrightnessB, pstPictureCfg->u16Hue, pstPictureCfg->u16Saturation);
    }

    if (pu32Sharpness)
    {
        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SHARPNESS);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16Sharpness[0] = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)*pu32Sharpness);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Sharpness NonLinearCurve NULL\n", __FUNCTION__, __LINE__,
                     pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SHARPNESS1);
        if (pstNonLinearCurve)
        {
            pstPictureCfg->u16Sharpness[1] = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)*pu32Sharpness);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%d, Sharpness NonLinearCurve NULL\n", __FUNCTION__, __LINE__,
                     pstDevContain->u32DevId);
            bRet = 0;
        }

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Sharpness:%d %d\n", __FUNCTION__, __LINE__,
                 pstPictureCfg->u16Sharpness[0], pstPictureCfg->u16Sharpness[1]);
    }
    return bRet;
}

void HalDispPictureIfSetConfig(HalDispCscmatrix_e eCscMatrix, HalDispPictureConfig_t *pstPictureCfg, void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtx      = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    HalDispPicturePqConfig_t * pstPqPictureCfg = NULL;

    pstDispCtx      = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain   = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    pstPqPictureCfg = &g_stPicturePqCfg[pstDevContain->u32DevId];

    if (pstPqPictureCfg->bUpdate == 0)
    {
        g_stInterCfg[pstDevContain->u32DevId].u8CsCMd = HAL_DISP_COLOR_CSCM_TO_Y2RM(eCscMatrix);
        HalDispColorSeletYuvToRgbMatrix(g_stInterCfg[pstDevContain->u32DevId].u8ColorId,
                                        g_stInterCfg[pstDevContain->u32DevId].u8CsCMd, NULL, pCtx);
    }

    HalDispColorAdjustHCS(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstPictureCfg->u16Hue,
                          pstPictureCfg->u16Saturation, pstPictureCfg->u16Contrast, pCtx);

    HalDispColorAdjustBrightness(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstPictureCfg->u16BrightnessR,
                                 pstPictureCfg->u16BrightnessG, pstPictureCfg->u16BrightnessB, pCtx);

    if (HAL_DISP_DEV_MACE_SUPPORT(pstDevContain->u32DevId))
    {
        _HalDispMacePeakingSetBand1Coef(pstPictureCfg->u16Sharpness[0], pCtx);
        _HalDispMacePeakingSetBand2Coef(pstPictureCfg->u16Sharpness[1], pCtx);
    }
    else
    {
        HalDispHpqSetPeakingCoef(pCtx, pstPictureCfg->u16Sharpness[0], pstPictureCfg->u16Sharpness[1]);
    }
}
u8 HalDispPictureIfIsPqUpdate(u32 u32DevId)
{
    u8 bRet;

    if (u32DevId < HAL_DISP_DEVICE_MAX)
    {
        bRet = g_stPicturePqCfg[u32DevId].bUpdate;
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Dev Id(%d) out of rage\n", __FUNCTION__, __LINE__, u32DevId);
    }

    return bRet;
}

void HalDispPictureIfSetPqConfig(u32 u32DevId, HalDispPicturePqConfig_t *pstPqInCfg)
{
    HalDispPicturePqConfig_t *pstPqPictureCfg = NULL;

    pstPqPictureCfg = &g_stPicturePqCfg[u32DevId];

    memcpy(pstPqPictureCfg, pstPqInCfg, sizeof(HalDispPicturePqConfig_t));

    // Update NoneLinearCurve
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_CONTRAST], pstPqPictureCfg->u16Contrast);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_R_BRIGHTNESS],
        pstPqPictureCfg->u16BrightnessR);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_G_BRIGHTNESS],
        pstPqPictureCfg->u16BrightnessG);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_B_BRIGHTNESS],
        pstPqPictureCfg->u16BrightnessB);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_SATURATION],
        pstPqPictureCfg->u16Saturation);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_HUE], pstPqPictureCfg->u16Hue);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_SHARPNESS],
        pstPqPictureCfg->u16Sharpness);
    _HalDispPictureIfUpdateNoneLinearCurveOsd50(
        &g_stPictureNonLinearSetting[u32DevId].stDispDevice[E_HAL_DISP_PICTURE_SHARPNESS1],
        pstPqPictureCfg->u16Sharpness1);

    DISP_DBG(DISP_DBG_LEVEL_COLOR,
             "%s %d, DeviceId:%d, Gain(0x%02x 0x%02x 0x%02x) Bri(0x%02x 0x%02x 0x%02x), Hue:0x%02x, Sat:0x%02x, "
             "Contrast:0x%02x, Sharp(0x%02x 0x%02x), Coef(0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, "
             "0x%04x, 0x%04x)\n",
             __FUNCTION__, __LINE__, u32DevId, pstPqPictureCfg->u16GainR, pstPqPictureCfg->u16GainG,
             pstPqPictureCfg->u16GainB, pstPqPictureCfg->u16BrightnessR, pstPqPictureCfg->u16BrightnessG,
             pstPqPictureCfg->u16BrightnessB, pstPqPictureCfg->u16Hue, pstPqPictureCfg->u16Saturation,
             pstPqPictureCfg->u16Contrast, pstPqPictureCfg->u16Sharpness, pstPqPictureCfg->u16Sharpness1,
             pstPqPictureCfg->as16Coef[0], pstPqPictureCfg->as16Coef[1], pstPqPictureCfg->as16Coef[2],
             pstPqPictureCfg->as16Coef[3], pstPqPictureCfg->as16Coef[4], pstPqPictureCfg->as16Coef[5],
             pstPqPictureCfg->as16Coef[6], pstPqPictureCfg->as16Coef[7], pstPqPictureCfg->as16Coef[8]);
}
void HalDispPictureIfApplyPqConfig(void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtx      = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    HalDispPicturePqConfig_t * pstPqPictureCfg = NULL;

    pstDispCtx      = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain   = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    pstPqPictureCfg = &g_stPicturePqCfg[pstDevContain->u32DevId];

    // Apply PQ setting
    HalDispColorSeletYuvToRgbMatrix(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_USER,
                                    pstPqPictureCfg->as16Coef, pCtx);
    g_stInterCfg[pstDevContain->u32DevId].u8CsCMd = E_DISP_COLOR_YUV_2_RGB_MATRIX_USER;

    HalDispColorAdjustVideoRGB(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstPqPictureCfg->u16GainR,
                               pstPqPictureCfg->u16GainG, pstPqPictureCfg->u16GainB, pCtx);

    HalDispColorAdjustHCS(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstPqPictureCfg->u16Hue,
                          pstPqPictureCfg->u16Saturation, pstPqPictureCfg->u16Contrast, pCtx);

    HalDispColorAdjustBrightness(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstPqPictureCfg->u16BrightnessR,
                                 pstPqPictureCfg->u16BrightnessG, pstPqPictureCfg->u16BrightnessB, pCtx);

    if (HAL_DISP_DEV_MACE_SUPPORT(pstDevContain->u32DevId))
    {
        _HalDispMacePeakingSetBand1Coef(pstPqPictureCfg->u16Sharpness, pCtx);
        _HalDispMacePeakingSetBand2Coef(pstPqPictureCfg->u16Sharpness1, pCtx);
    }
    else
    {
        HalDispHpqSetPeakingCoef(pCtx, pstPqPictureCfg->u16Sharpness, pstPqPictureCfg->u16Sharpness1);
    }
}
