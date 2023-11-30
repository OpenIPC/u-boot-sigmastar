/*
* hal_disp_mace.c- Sigmastar
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

#define _HAL_DISP_MACE_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_mace.h"
#include "hal_disp_color.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HalDispMaceSetGamma(u8 *pu8Data,HalDispGammaType_e eRGB, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg;
    u32   i;
    u16   u16Val;
    u32   u32DevId = HalDispGetDeviceId((pCtx), 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    switch(eRGB)
    {
        case E_HAL_DISP_GAMMA_R:
            u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_GAMMA, REG_TBL_R_00);
            break;
        case E_HAL_DISP_GAMMA_G:
            u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_GAMMA, REG_TBL_G_00);
            break;
        case E_HAL_DISP_GAMMA_B:
            u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_GAMMA, REG_TBL_B_00);
            break;
        default:
            return;
    }
    if (pCmdqCtx && u32Reg)
    {
        for (i = 0; i < 32; i += 2)
        {
            u16Val = pu8Data[i] | ((u16)pu8Data[i + 1]) << 8;
            HalDispUtilityW2BYTEMSK(u32Reg + i, u16Val, 0xFFFF, pCmdqCtx);
        }
    }
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg + 0x20, pu8Data[32], mask_of_disp_top_gamma0_reg_tbl_r_32, pCmdqCtx);
    }
}

static void _HalDispMaceSetGammaEn(u16 u16Val, void *pCtx)
{
    u32 u32DevId = HalDispGetDeviceId((pCtx), 0);

    HalDispGammaSetEn(pCtx, u16Val, u32DevId, E_DISP_UTILITY_REG_ACCESS_CMDQ);
}
static void _HalDispMacePeakingSetBand1Coef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_MACE_REG_DD_MAIN_BAND1_COEF,
                                u16Val << shift_of_disp_top_mace_reg_dd_main_band1_coef,
                                mask_of_disp_top_mace_reg_dd_main_band1_coef, pCmdqCtx);
    }
}

static void _HalDispMacePeakingSetBand2Coef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_MACE_REG_DD_MAIN_BAND2_COEF,
                                (u16Val) << shift_of_disp_top_mace_reg_dd_main_band2_coef,
                                mask_of_disp_top_mace_reg_dd_main_band2_coef, pCmdqCtx);
    }
}
void HalDispGammaSetEn(void *pCtx, u8 u8bEn, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSKByType(DISP_TOP_GAMMA0_REG_GAMMA_EN
                                      + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_GAMMA0_REG_GAMMA_EN),
                                  u8bEn ? mask_of_disp_top_gamma0_reg_gamma_en : 0x0000,
                                  mask_of_disp_top_gamma0_reg_gamma_en, pCmdqCtx, enRiuMode);
}
void HalDispMaceSetGammaConfig(HalDispMaceGammaConfig_t *pstGammaCfg, void *pCtx)
{
    if (pstGammaCfg->bUpdate && pCtx)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        _HalDispMaceSetGamma(pstGammaCfg->u8R, E_HAL_DISP_GAMMA_R, pCtx);
        _HalDispMaceSetGamma(pstGammaCfg->u8G, E_HAL_DISP_GAMMA_G, pCtx);
        _HalDispMaceSetGamma(pstGammaCfg->u8B, E_HAL_DISP_GAMMA_B, pCtx);
        _HalDispMaceSetGammaEn(pstGammaCfg->u8En, pCtx);
        pstGammaCfg->bUpdate = 0;
    }
}

void HalDispMaceSetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (HAL_DISP_DEV_MACE_SUPPORT(u32Dev) && u8bBypass)
    {
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_bw2sbri, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_WLE_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_wle_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_BLE_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_ble_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_Y_LPF_MODE
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_y_lpf_mode, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_C_LPF_MODE
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_c_lpf_mode, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_Y_BAND1_H_CORING_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_y_band1_h_coring_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_Y_BAND2_H_CORING_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_y_band2_h_coring_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_H_CORING_Y_DITHER_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_h_coring_y_dither_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_C_BAND1_H_CORING_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_c_band1_h_coring_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_C_BAND2_H_CORING_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_c_band2_h_coring_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_H_CORING_C_DITHER_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_h_coring_c_dither_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_DD_MAIN_PEAK_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_dd_main_peak_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T1_T2
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t1_t2, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T3
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t3, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T4
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t4, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T5
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t5, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T6
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t6, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T7
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t7, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T8
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t8, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_MACE_REG_MAIN_EN_T9
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_MACE_REG_DD_MAIN_BW2SBRI),
                                      0, mask_of_disp_top_mace_reg_main_en_t9, pCmdqCtx, enRiuMode);
    }
}
