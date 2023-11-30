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
extern HalDispInternalConfig_t gstInterCfg[HAL_DISP_DEVICE_MAX];

HalDispPictureIfNonLinearCurveConfig_t stPictureNonLinearSetting =
{
    { // DispDevice0
        {0,  80, 128, 160, 255},  // Contrast
        {0,  64, 128, 192, 255},  // BrightnessR
        {0,  64, 128, 192, 255},  // BrightnessG
        {0,  64, 128, 192, 255},  // BrightnessB
        {0,  96, 128, 160, 255},  // Saturation
        {0,  8,   24, 44,  63},   // Sharpness
        {0,  8,   24, 44,  63},   // Sharpness1
        {30, 40, 50,  60,  100},   // Hue
    },


    { // DispDevice1
        {0,  80, 128, 160, 255},  // Contrast
        {0,  64, 128, 192, 255},  // BrightnessR
        {0,  64, 128, 192, 255},  // BrightnessG
        {0,  64, 128, 192, 255},  // BrightnessB
        {0,  96, 128, 160, 255},  // Saturation
        {0,  8,   24, 44,  63},   // Sharpness
        {0,  8,   24, 44,  63},   // Sharpness1
        {30, 40, 50,  60,  100},   // Hue
    },
};

HalDispPicturePqConfig_t gstPicturePqCfg[HAL_DISP_DEVICE_MAX] =
{
    {0x0000, 0x0032, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0018, 0x0018,
     {0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0400}},

    {0x0000, 0x0032, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0080, 0x0018, 0x0018,
     {0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000, 0x0400}},
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
u16 _HalDispPictureIfCalculateNonLInear(HalDispPictureIfNonLinearCurveType_t *pNonLinearCurve, u8 u8AdjustValue)
{
    u16 u16Value,u16Y0,u16Y1,u16X0,u16X1,u16Intercept;
    u16 u16DistanceOfY, u16DistanceOfX;

    if(pNonLinearCurve == NULL)
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
        u16Intercept  = u16Y0;
        u8AdjustValue  = u8AdjustValue - u16X0;
    }
    else
    {
        u16DistanceOfY = u16Y0 - u16Y1;
        u16DistanceOfX = u16X1 - u16X0;
        u16Intercept  = u16Y1;
        u8AdjustValue  = u16X1 - u8AdjustValue;
    }

    u16Value = ((u16)u16DistanceOfY*u8AdjustValue/(u16DistanceOfX)) + u16Intercept;
    return u16Value;
}

void _HalDispPictureIfUpdateNoneLinearCurveOsd50(HalDispPictureIfNonLinearCurveType_t *pstNonLinerCurve, u16 u16Osd50)
{
    pstNonLinerCurve->u16OSD_25 = (u16Osd50 > pstNonLinerCurve->u16OSD_0) ?
                                  (u16Osd50 - pstNonLinerCurve->u16OSD_0) / 2 :
                                  (pstNonLinerCurve->u16OSD_0 - u16Osd50) / 2 ;

    pstNonLinerCurve->u16OSD_50 = u16Osd50;

    pstNonLinerCurve->u16OSD_75 = (pstNonLinerCurve->u16OSD_100 > u16Osd50) ?
                                  (pstNonLinerCurve->u16OSD_100 - u16Osd50) / 2 + u16Osd50:
                                  u16Osd50 - (u16Osd50 - pstNonLinerCurve->u16OSD_100) / 2;

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, (%d %d %d %d %d)\n",
        __FUNCTION__, __LINE__,
        pstNonLinerCurve->u16OSD_0, pstNonLinerCurve->u16OSD_25, pstNonLinerCurve->u16OSD_50,
        pstNonLinerCurve->u16OSD_75, pstNonLinerCurve->u16OSD_100);

}



HalDispPictureIfNonLinearCurveType_t * _HalDispIfGetPictureNonLinearCurve(u32 u32DevId, HalDispPictureIfType_e enType)
{
    HalDispPictureIfNonLinearCurveType_t *pNonLinearCruve = NULL;

    pNonLinearCruve = u32DevId == 1 ? &stPictureNonLinearSetting.stDispDevice1[enType] :
                      u32DevId == 0 ? &stPictureNonLinearSetting.stDispDevice0[enType] :
                                      NULL;
    return pNonLinearCruve;
}

bool HalDispPictureIfTransNonLinear(void *pCtx, HalDispCsc_t *pstCsc, u32 *pu32Sharpness, HalDispPictureConfig_t *pstPictureCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispPictureIfNonLinearCurveType_t *pstNonLinearCurve = NULL;
    bool bRet = 1;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];

    if(pstCsc)
    {
        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_CONTRAST);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16Contrast = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Contrast);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Contrast NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_HUE);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16Hue = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Hue);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Hue NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SATURATION);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16Saturation = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Saturation);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Saturation NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_R_BRIGHTNESS);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessR = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Bri_R NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_G_BRIGHTNESS);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessG = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Bri_G NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_B_BRIGHTNESS);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16BrightnessB = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)pstCsc->u32Luma);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Bri_B NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Con:%d, Bri:(%d %d %d) Hue:%d Sat:%d\n",
            __FUNCTION__, __LINE__,
            pstPictureCfg->u16Contrast, pstPictureCfg->u16BrightnessR,
            pstPictureCfg->u16BrightnessG, pstPictureCfg->u16BrightnessB,
            pstPictureCfg->u16Hue, pstPictureCfg->u16Saturation);
    }

    if(pu32Sharpness)
    {
        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SHARPNESS);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16Sharpness[0] = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)*pu32Sharpness);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Sharpness NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }

        pstNonLinearCurve = _HalDispIfGetPictureNonLinearCurve(pstDevContain->u32DevId, E_HAL_DISP_PICTURE_SHARPNESS1);
        if(pstNonLinearCurve)
        {
            pstPictureCfg->u16Sharpness[1] = _HalDispPictureIfCalculateNonLInear(pstNonLinearCurve, (u8)*pu32Sharpness);
        }
        else
        {
            DISP_ERR("%s %d:: DevId:%ld, Sharpness NonLinearCurve NULL\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId);
            bRet = 0;
        }


        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Sharpness:%d %d\n", __FUNCTION__, __LINE__, pstPictureCfg->u16Sharpness[0], pstPictureCfg->u16Sharpness[1]);
    }
    return bRet;
}

void HalDispPictureIfSetConfig(HalDispCscmatrix_e eCscMatrix, HalDispPictureConfig_t *pstPictureCfg, void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispPicturePqConfig_t *pstPqCfg = NULL;

    pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];
    pstPqCfg = &gstPicturePqCfg[pstDevContain->u32DevId];

    if(pstPqCfg->bUpdate == 0)
    {
        if(eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS)
        {
            HalDispColorSeletYuvToRgbMatrix(gstInterCfg[pstDevContain->u32DevId].u8ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS, NULL, pCtx);
        }
        else if(eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC)
        {
            HalDispColorSeletYuvToRgbMatrix(gstInterCfg[pstDevContain->u32DevId].u8ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_SDTV, NULL, pCtx);
        }
        else if(eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
        {
            HalDispColorSeletYuvToRgbMatrix(gstInterCfg[pstDevContain->u32DevId].u8ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV, NULL, pCtx);
        }
    }

    HalDispColorAdjustHCS(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstPictureCfg->u16Hue,
        pstPictureCfg->u16Saturation,
        pstPictureCfg->u16Contrast,
        pCtx);

    HalDispColorAdjustBrightness(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstPictureCfg->u16BrightnessR,
        pstPictureCfg->u16BrightnessG,
        pstPictureCfg->u16BrightnessB,
        pCtx);

    if(pstDevContain->u32DevId == gstInterCfg[pstDevContain->u32DevId].u8ColorId)
    {
        _HalDispMacePeakingSetBand1Coef(pstPictureCfg->u16Sharpness[0], pCtx);
        _HalDispMacePeakingSetBand2Coef(pstPictureCfg->u16Sharpness[1], pCtx);
    }
    else
    {
        //ToDo Device1 Sharpness
    }
}

void HalDispPictureIfSetPqConfig(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispPicturePqConfig_t *pstPqCfg = NULL;
    u8 au8Data[32];
    u8 i;

    pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];
    pstPqCfg = &gstPicturePqCfg[pstDevContain->u32DevId];


    // Get Setting From PQ Bin
    HalDispHpqLoadData(pstDevContain->u32DevId, au8Data, 32);

    pstPqCfg->bUpdate = 1;
    pstPqCfg->u16GainR = au8Data[0];
    pstPqCfg->u16GainG = au8Data[1];
    pstPqCfg->u16GainB = au8Data[2];

    pstPqCfg->u16BrightnessR = au8Data[3];
    pstPqCfg->u16BrightnessG = au8Data[4];
    pstPqCfg->u16BrightnessB = au8Data[5];

    pstPqCfg->u16Hue        = au8Data[6];
    pstPqCfg->u16Saturation = au8Data[7];
    pstPqCfg->u16Contrast   = au8Data[8];
    pstPqCfg->u16Sharpness  = au8Data[9];
    pstPqCfg->u16Sharpness1 = au8Data[10];

    for(i=0; i<9; i++)
    {
        pstPqCfg->as16Coef[i] = (((s16)au8Data[i*2+12]) << 8) | (s16)au8Data[i*2+11];
    }

    // Update NoneLinearCurve
    if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0)
    {
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_CONTRAST], pstPqCfg->u16Contrast);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_R_BRIGHTNESS], pstPqCfg->u16BrightnessR);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_G_BRIGHTNESS], pstPqCfg->u16BrightnessG);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_B_BRIGHTNESS], pstPqCfg->u16BrightnessB);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_SATURATION], pstPqCfg->u16Saturation);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_HUE], pstPqCfg->u16Hue);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_SHARPNESS], pstPqCfg->u16Sharpness);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice0[E_HAL_DISP_PICTURE_SHARPNESS1], pstPqCfg->u16Sharpness1);
    }
    else
    {
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_CONTRAST], pstPqCfg->u16Contrast);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_R_BRIGHTNESS], pstPqCfg->u16BrightnessR);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_G_BRIGHTNESS], pstPqCfg->u16BrightnessG);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_B_BRIGHTNESS], pstPqCfg->u16BrightnessB);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_SATURATION], pstPqCfg->u16Saturation);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_HUE], pstPqCfg->u16Hue);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_SHARPNESS], pstPqCfg->u16Sharpness);
        _HalDispPictureIfUpdateNoneLinearCurveOsd50(&stPictureNonLinearSetting.stDispDevice1[E_HAL_DISP_PICTURE_SHARPNESS1], pstPqCfg->u16Sharpness1);
    }


    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d,CtxId:%ld, DeviceId:%ld, Gain(0x%02x 0x%02x 0x%02x) Bri(0x%02x 0x%02x 0x%02x), Hue:0x%02x, Sat:0x%02x, Contrast:0x%02x, Sharp(0x%02x 0x%02x), Coef(0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x, 0x%04x)\n",
        __FUNCTION__, __LINE__,
        pstDispCtx->u32Idx, pstDevContain->u32DevId,
        pstPqCfg->u16GainR, pstPqCfg->u16GainG, pstPqCfg->u16GainB,
        pstPqCfg->u16BrightnessR, pstPqCfg->u16BrightnessG, pstPqCfg->u16BrightnessB,
        pstPqCfg->u16Hue, pstPqCfg->u16Saturation, pstPqCfg->u16Contrast,
        pstPqCfg->u16Sharpness, pstPqCfg->u16Sharpness1,
        pstPqCfg->as16Coef[0], pstPqCfg->as16Coef[1], pstPqCfg->as16Coef[2],
        pstPqCfg->as16Coef[3], pstPqCfg->as16Coef[4], pstPqCfg->as16Coef[5],
        pstPqCfg->as16Coef[6], pstPqCfg->as16Coef[7], pstPqCfg->as16Coef[8]);

    // Apply PQ setting
    HalDispColorSeletYuvToRgbMatrix(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        E_DISP_COLOR_YUV_2_RGB_MATRIX_USER, pstPqCfg->as16Coef, pCtx);

    HalDispColorAdjustVideoRGB(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstPqCfg->u16GainR,
        pstPqCfg->u16GainG,
        pstPqCfg->u16GainB,
        pCtx);

    HalDispColorAdjustHCS(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstPqCfg->u16Hue,
        pstPqCfg->u16Saturation,
        pstPqCfg->u16Contrast,
        pCtx);

    HalDispColorAdjustBrightness(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstPqCfg->u16BrightnessR,
        pstPqCfg->u16BrightnessG,
        pstPqCfg->u16BrightnessB,
        pCtx);

    if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0)
    {
        _HalDispMacePeakingSetBand1Coef(pstPqCfg->u16Sharpness, pCtx);
        _HalDispMacePeakingSetBand2Coef(pstPqCfg->u16Sharpness1, pCtx);
    }
    else
    {
        //ToDo Device1 Sharpness
    }

}
