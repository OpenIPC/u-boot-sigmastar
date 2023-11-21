/*
* hal_disp_clk.c- Sigmastar
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

#define _HAL_DISP_CLK_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "hal_disp_clk.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
HalDispClkGpCtrlConfig_t stDispClkGpCtrlCfg[HAL_DISP_DEVICE_MAX] ={{0, 0},{0, 0},};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
void _HalDispClkSetMipiPixelG(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0 : 1;
    u16Val |= ( u32ClkRate & 0x0003) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_23_L, u16Val, 0x000F);
}

void _HalDispClkGetMipiPixelG(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_23_L) & 0x000F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x0003;
}


void _HalDispClkSetMipiPixel(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= ( u32ClkRate & 0x0003) << 6;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_23_L, u16Val, 0x00F0);
}

void _HalDispClkGetMipiPixel(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_23_L) & 0x00F0;

    *pbEn = (u16Val & 0x0010) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 6) & 0x0003;
}

void _HalDispClkSetLcdSrc(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0003) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_24_L, u16Val, 0x000F);
}

void _HalDispClkGetLcdSrc(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_24_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x0003;
}

void _HalDispClkSetLcd(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= (u32ClkRate & 0x0003) << 10;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_24_L, u16Val, 0x0F00);
}

void _HalDispClkGetLcd(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_24_L);
    *pbEn = u16Val & 0x0100 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 10) & 0x0003;
}

void _HalDispClkSetHdmi(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0003) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_35_L, u16Val, 0x000F);
}

void _HalDispClkGetHdmi(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_35_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x0003;
}

void _HalDispClkSetTve(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= (u32ClkRate & 0x0003) << 6;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_35_L, u16Val, 0x00F0);
}

void _HalDispClkGetTve(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_35_L);
    *pbEn = u16Val & 0x0010 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 6) & 0x0003;
}

void _HalDispClkSetDac(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0003) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_36_L, u16Val, 0x000F);
}

void _HalDispClkGetDac(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_36_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x003;
}

void _HalDispClkSetDacWaff(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= (u32ClkRate & 0x0003) << 6;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_36_L, u16Val, 0x00F0);
}

void _HalDispClkGetDacWaff(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_36_L);
    *pbEn = u16Val & 0x0010 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 6) & 0x0003;
}

void _HalDispClkSetMipiTxDsiApb(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= (u32ClkRate & 0x0003) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_37_L, u16Val, 0x000F);
}

void _HalDispClkGetMipiTxDsiApb(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_37_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x0003;
}

//------------------------------------------------------------------------------
// Global Function
//------------------------------------------------------------------------------
void HalDispClkSetOdclk0(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x0007) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_22_L, u16Val, 0x001F);
}

void HalDispClkGetOdclk0(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_22_L) & 0x001F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x0007;
}


void HalDispClkSetOdclk1(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= ( u32ClkRate & 0x0007) << 10;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_22_L, u16Val, 0x1F00);
}


void HalDispClkGetOdclk1(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_22_L) & 0x1F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0007;
}

void HalDispClkSetFpllOdclk0(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0003) << 2;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_25_L, u16Val, 0x000F);
}

void HalDispClkGetFpllOdclk0(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_25_L);
    *pbEn = u16Val & 0x0001 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 2) & 0x0003;
}


void HalDispClkSetFpllOdclk1(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0010;
    u16Val |= (u32ClkRate & 0x0003) << 6;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_25_L, u16Val, 0x00F0);
}

void HalDispClkGetFpllOdclk1(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_25_L);
    *pbEn = u16Val & 0x0010 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 6) & 0x0003;
}


void HalDispClkSetDma(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= (u32ClkRate & 0x0003) << 10;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_DISP_GP_CTRL_25_L, u16Val, 0x0F00);
}

void HalDispClkGetDma(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val= HalDispUtilityR2BYTEDirect(REG_DISP_GP_CTRL_25_L);
    *pbEn = u16Val & 0x0100 ? 0 : 1;
    *pu32ClkRate =  (u16Val >> 10) & 0x0003;
}

u32 HalDispClkMapMopToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(192) ? 0x07 :
                u32ClkRate <= CLK_MHZ(216) ? 0x06 :
                u32ClkRate <= CLK_MHZ(240) ? 0x05 :
                u32ClkRate <= CLK_MHZ(288) ? 0x04 :
                u32ClkRate <= CLK_MHZ(320) ? 0x03 :
                u32ClkRate <= CLK_MHZ(384) ? 0x00 :
                u32ClkRate <= CLK_MHZ(432) ? 0x01 :
                u32ClkRate <= CLK_MHZ(480) ? 0x02 :
                                             0x02;
    return u32ClkIdx;
}

u32 HalDispClkMapMopToRate(u32 u32ClkIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkIdx == 0x00 ? CLK_MHZ(384) :
                 u32ClkIdx == 0x01 ? CLK_MHZ(480) :
                 u32ClkIdx == 0x02 ? CLK_MHZ(432) :
                 u32ClkIdx == 0x03 ? CLK_MHZ(320) :
                 u32ClkIdx == 0x04 ? CLK_MHZ(288) :
                 u32ClkIdx == 0x05 ? CLK_MHZ(240) :
                 u32ClkIdx == 0x06 ? CLK_MHZ(216) :
                 u32ClkIdx == 0x07 ? CLK_MHZ(192) :
                                     CLK_MHZ(384);
    return u32ClkRate;
}

void HalDispClkSetMop0(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x0007) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_47_L, u16Val, 0x001F);
}

void HalDispClkGetMop0(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_47_L) & 0x001F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x0007;
}

void HalDispClkSetMop1(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= ( u32ClkRate & 0x0007) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_47_L, u16Val, 0x1F00);
}

void HalDispClkGetMop1(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_47_L) & 0x1F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0007;
}

u32 HalDispClkMapDisp432ToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(432) ? 0x00 : 0x00;
    return u32ClkIdx;
}

u32 HalDispClkMapDisp432ToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(432) : 0;
    return u32ClkRate;
}

void HalDispClkSetDisp432(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= ( u32ClkRate & 0x0003) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_48_L, u16Val, 0x0F00);
}

void HalDispClkGetDisp432(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_48_L) & 0x0F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0003;
}

u32 HalDispClkMapDisp216ToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate <= CLK_MHZ(216) ? 0x00 : 0x00;
    return u32ClkIdx;
}

u32 HalDispClkMapDisp216ToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(216) : 0;
    return u32ClkRate;
}

void HalDispClkSetDisp216(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x1000;
    u16Val |= ( u32ClkRate & 0x0003) << 14;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_48_L, u16Val, 0xF000);
}

void HalDispClkGetDisp216(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_48_L) & 0xF000;

    *pbEn = (u16Val & 0x1000) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 14) & 0x0003;
}

u32 HalDispClkMapDispPixelToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate == 10           ? 0x0A :
                u32ClkRate <= CLK_MHZ(108) ? 0x08 :
                u32ClkRate <= CLK_MHZ(123) ? 0x06 :
                u32ClkRate <= CLK_MHZ(144) ? 0x07 :
                u32ClkRate <= CLK_MHZ(172) ? 0x05 :
                u32ClkRate <= CLK_MHZ(192) ? 0x02 :
                u32ClkRate <= CLK_MHZ(216) ? 0x04 :
                u32ClkRate <= CLK_MHZ(240) ? 0x03 :
                u32ClkRate <= CLK_MHZ(288) ? 0x00 :
                u32ClkRate <= CLK_MHZ(320) ? 0x01 :
                                             0x09;
    return u32ClkIdx;
}

u32 HalDispClkMapDispPixelToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0x0A? 10           :
                 u32ClkRateIdx == 0x08? CLK_MHZ(108) :
                 u32ClkRateIdx == 0x06? CLK_MHZ(123) :
                 u32ClkRateIdx == 0x07? CLK_MHZ(144) :
                 u32ClkRateIdx == 0x05? CLK_MHZ(172) :
                 u32ClkRateIdx == 0x02? CLK_MHZ(192) :
                 u32ClkRateIdx == 0x04? CLK_MHZ(216) :
                 u32ClkRateIdx == 0x03? CLK_MHZ(240) :
                 u32ClkRateIdx == 0x00? CLK_MHZ(288) :
                 u32ClkRateIdx == 0x01? CLK_MHZ(320) :
                                        0x09;
    return u32ClkRate;
}


void HalDispClkSetDispPixel0(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= ( u32ClkRate & 0x000F) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_49_L, u16Val, 0x003F);
}

void HalDispClkGetDispPixel0(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_49_L) & 0x003F;

    *pbEn = (u16Val & 0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 2) & 0x000F;
}

void HalDispClkSetDispPixel1(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= ( u32ClkRate & 0x000F) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_49_L, u16Val, 0x3F00);
}

void HalDispClkGetDispPixel1(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_49_L) & 0x3F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x000F;
}

void HalDispClkSetMipiTxDsi(bool bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= ( u32ClkRate & 0x0007) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_5E_L, u16Val, 0x1F00);
}

void HalDispClkGetMipiTxDsi(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_5E_L) & 0x1F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0007;
}

void HalDispClkInitGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispHwContain_t *pstHalHwContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;

    if(pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] == 0)
    {
        memset(&stDispClkGpCtrlCfg[pstDevContain->u32DevId], 0, sizeof(HalDispClkGpCtrlConfig_t));
        pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId] = (void *)&stDispClkGpCtrlCfg[pstDevContain->u32DevId];
        pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] = 1;
    }

}

void HalDispClkDeInitGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispHwContain_t *pstHalHwContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;

    if(pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] == 1)
    {
        pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId] = NULL;
        pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] = 0;
    }
}


void HalDispClkSetMergeOdClk(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg1 = NULL;
    u32 u32Rate;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstClkGpCtrlCfg1 = (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[HAL_DISP_DEVICE_ID_1];

    u32Rate = pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI    ? HAL_DISP_ODCLK_SEL_HDMI          :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_LCD     ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_DAC     ? HAL_DISP_ODCLK_SEL_DAC_DIV2      :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS    ? HAL_DISP_ODCLK_SEL_TVENC_27M     :
                                                                          HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK;
    HalDispClkSetOdclk0(pstClkGpCtrlCfg1->bEn, u32Rate);

}

void HalDispClkSetGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg0 = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg1 = NULL;
    HalDispClkGpCtrlConfig_t stFameClkGpCtrlCfg = {0, 0};
    u32 u32Rate;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstClkGpCtrlCfg0 = (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[HAL_DISP_DEVICE_ID_0];
    pstClkGpCtrlCfg1 = (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[HAL_DISP_DEVICE_ID_1];

    if(pstClkGpCtrlCfg0 == NULL)
    {
        pstClkGpCtrlCfg0 = &stFameClkGpCtrlCfg;
    }

    if(pstClkGpCtrlCfg1 == NULL)
    {
        pstClkGpCtrlCfg1 = &stFameClkGpCtrlCfg;
    }


    DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, Dev0(%d, %s), Dev1(%d, %s)\n",
        __FUNCTION__, __LINE__,
        pstClkGpCtrlCfg0->bEn, PARSING_HAL_CLK_GP_CTRL_TYPE(pstClkGpCtrlCfg0->eType),
        pstClkGpCtrlCfg1->bEn, PARSING_HAL_CLK_GP_CTRL_TYPE(pstClkGpCtrlCfg1->eType));

    //Odclk_0
    u32Rate = pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI     ? HAL_DISP_ODCLK_SEL_HDMI          :
              pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_LCD      ? HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK :
              pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_DAC      ? HAL_DISP_ODCLK_SEL_DAC_DIV2      :
              pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI  ? HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK :
              pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS     ? HAL_DISP_ODCLK_SEL_TVENC_27M     :
              pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC ? HAL_DISP_ODCLK_SEL_HDMI          :
                                                                           HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK ;
    HalDispClkSetOdclk0(pstClkGpCtrlCfg0->bEn, u32Rate);

    //Odclk_1
    u32Rate = pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI     ? HAL_DISP_ODCLK_SEL_HDMI          :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_LCD      ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_DAC      ? HAL_DISP_ODCLK_SEL_DAC_DIV2      :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI  ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS     ? HAL_DISP_ODCLK_SEL_TVENC_27M     :
              pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC ? HAL_DISP_ODCLK_SEL_HDMI          :
                                                                           HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK ;
    HalDispClkSetOdclk1(pstClkGpCtrlCfg1->bEn, u32Rate);

    //MipiDsi
    if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
    {
        _HalDispClkSetMipiPixelG(1, HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK1);
        _HalDispClkSetMipiPixel(1, HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK1);
        HalDispClkSetMipiTxDsi(1, HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL);
        _HalDispClkSetMipiTxDsiApb(1, HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0);
    }
    else if(pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
    {
        _HalDispClkSetMipiPixelG(1, HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK0);
        _HalDispClkSetMipiPixel(1, HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK0);
        HalDispClkSetMipiTxDsi(1, HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL);
        _HalDispClkSetMipiTxDsiApb(1, HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0);

    }
    else
    {
        _HalDispClkSetMipiPixelG(0, HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK0);
        _HalDispClkSetMipiPixel(0, HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK0);
        HalDispClkSetMipiTxDsi(0, HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL);
        _HalDispClkSetMipiTxDsiApb(0, HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0);
    }

    // Lcd & fpll_odclk1
    if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        _HalDispClkSetLcdSrc(1, HAL_DISP_CLK_LCD_SRC_SEL_ODCLK1);
        _HalDispClkSetLcd(1, HAL_DISP_CLK_LCD_SEL_ODCLK1);
        HalDispClkSetFpllOdclk1(1, HAL_DISP_CLK_FPLL_SEL_ODCLK1);
    }
    else if(pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        _HalDispClkSetLcdSrc(1, HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0);
        _HalDispClkSetLcd(1, HAL_DISP_CLK_LCD_SEL_ODCLK0);
        HalDispClkSetFpllOdclk1(1, HAL_DISP_CLK_FPLL_SEL_ODCLK0);
    }
    else
    {
        _HalDispClkSetLcdSrc(0, HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0);
        _HalDispClkSetLcd(0, HAL_DISP_CLK_LCD_SEL_ODCLK0);
        HalDispClkSetFpllOdclk1(0, HAL_DISP_CLK_FPLL_SEL_ODCLK0);
    }

    // Hdmi
    if((pstClkGpCtrlCfg0->bEn == 1 &&
       ((pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC) || (pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI))) ||
       (pstClkGpCtrlCfg1->bEn == 1 &&
       ((pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC) ||  (pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_HDMI)) ))
    {
        _HalDispClkSetHdmi(1, HAL_DISP_CLK_HDMI_SEL_HDMI);
    }
    else
    {
        _HalDispClkSetHdmi(0, HAL_DISP_CLK_HDMI_SEL_HDMI);
    }

    // Tve
    if((pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS) ||
       (pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS))
    {
        _HalDispClkSetTve(1, HAL_DISP_CLK_TVE_SEL_CVBS);
    }
    else
    {
        _HalDispClkSetTve(0, HAL_DISP_CLK_TVE_SEL_CVBS);
    }


    // DAC & fpll_odclk_0
    if(pstClkGpCtrlCfg1->bEn == 1 && (pstClkGpCtrlCfg1->eType & E_HAL_DISP_CLK_GP_CTRL_DAC))
    {
        _HalDispClkSetDac(1, HAL_DISP_CLK_DAC_SEL_DAC);
        _HalDispClkSetDacWaff(1, HAL_DISP_CLK_DAC_WAFF_SEL_DAC_DIV2);
        HalDispClkSetFpllOdclk0(1, HAL_DISP_CLK_FPLL_SEL_ODCLK1);

    }
    else if(pstClkGpCtrlCfg0->bEn == 1 && (pstClkGpCtrlCfg0->eType & E_HAL_DISP_CLK_GP_CTRL_DAC))
    {
        _HalDispClkSetDac(1, HAL_DISP_CLK_DAC_SEL_DAC);
        _HalDispClkSetDacWaff(1, HAL_DISP_CLK_DAC_WAFF_SEL_DAC_DIV2);
        HalDispClkSetFpllOdclk0(1, HAL_DISP_CLK_FPLL_SEL_ODCLK0);
    }
    else
    {
        _HalDispClkSetDac(0, HAL_DISP_CLK_DAC_SEL_DAC);
        _HalDispClkSetDacWaff(0, HAL_DISP_CLK_DAC_WAFF_SEL_HDMI);
        HalDispClkSetFpllOdclk0(0, HAL_DISP_CLK_FPLL_SEL_ODCLK0);
    }

    // disp_pixel_1
    if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        HalDispClkSetDispPixel1(1, HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL);
    }
    else if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS)
    {
        HalDispClkSetDispPixel1(1, HAL_DISP_CLK_DISP_PIXEL_SEL_DISP_PLL);
    }
    else if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
    {
        HalDispClkSetDispPixel1(1, HAL_DISP_CLK_DISP_PIXEL_SEL_320M);
    }
    else
    {
        HalDispClkSetDispPixel1(0, HAL_DISP_CLK_DISP_PIXEL_SEL_288M);
    }


    // disp_pixel_0
    if(pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        HalDispClkSetDispPixel0(1, HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL);
    }
    else if(pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS)
    {
        HalDispClkSetDispPixel0(1, HAL_DISP_CLK_DISP_PIXEL_SEL_DISP_PLL);
    }
    else if(pstClkGpCtrlCfg0->bEn == 1 && pstClkGpCtrlCfg0->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
    {
        HalDispClkSetDispPixel0(1, HAL_DISP_CLK_DISP_PIXEL_SEL_320M);
    }
    else if(pstClkGpCtrlCfg1->bEn == 1 && pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS)
    {
        HalDispClkSetDispPixel1(1, HAL_DISP_CLK_DISP_PIXEL_SEL_320M);
    }
    else
    {
        HalDispClkSetDispPixel0(0, HAL_DISP_CLK_DISP_PIXEL_SEL_288M);
    }
}

void HalDispClkInit(bool bEn)
{
    HalDispClkSetOdclk0(bEn, HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK);
    HalDispClkSetOdclk1(bEn, HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK);
}

