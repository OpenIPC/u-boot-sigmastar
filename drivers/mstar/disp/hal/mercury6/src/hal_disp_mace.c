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
HalDispMaceHwContext_t gstHwMaceCtx;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void __HalDispMaceGetCmdqAndReg(void *pCtx, void **pCmdqCtx, u32 *pu32Reg,  u32 u32Reg0, u32 u32Reg1)
{
    if(HalDispGetCmdqByCtx(pCtx, pCmdqCtx) == 0)
    {
        *pCmdqCtx = NULL;
        *pu32Reg = 0;
    }
    else
    {
        DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
        *pu32Reg = pstDispCtx->u32Idx == 0 ? u32Reg0 :
                   pstDispCtx->u32Idx == 1 ? u32Reg1 :
                                             0;
    }
}


// ----------------- BW Extension -----------------
void _HalDispMaceSetBw2sbri(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1A_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetBleEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_16_L, u16Val ? 0x0001 : 0x0000 , 0x0001, pCmdqCtx);
    }
}

void _HalDispMaceSetWleEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_16_L, u16Val ? 0x0002 : 0x0000 , 0x0002, pCmdqCtx);
    }
}


void _HalDispMaceSetBlackStart(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_17_L, u16Val, 0x007F, pCmdqCtx);
    }
}

void _HalDispMaceSetWhiteStart(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_16_L, (u16Val & 0x007F) << 8, 0x7F00, pCmdqCtx);
    }
}

void _HalDispMaceSetBlackSlop(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0A_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetWhiteSlop(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0A_L, (u16Val & 0x00FF), 0x00FF, pCmdqCtx);
    }
}

// ----------------- Lpf -----------------
void _HalDispMaceSetLpfY(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1C_L, (u16Val & 0x03) << 2, 0x000C, pCmdqCtx);
    }
}

void _HalDispMaceSetLpfC(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1C_L, (u16Val & 0x03) << 4, 0x0030, pCmdqCtx);
    }
}

// ----------------- DLC/DCR -----------------
void _HalDispMaceSetDcrEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_4C_L, REG_DISP_TOP_OP2_1_4C_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0100 : 0x0000, 0x0100, pCmdqCtx);
    }
}

void _HalDispMaceSetDcrOffset(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_55_L, REG_DISP_TOP_OP2_1_55_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x0FFF, pCmdqCtx);
    }
}

void _HalDispMaceSetYGain(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_4D_L, REG_DISP_TOP_OP2_1_4D_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetYGainOffset(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_4F_L, REG_DISP_TOP_OP2_1_4F_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x0FFF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_56_L, REG_DISP_TOP_OP2_1_56_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0800 : 0x0000, 0x0800, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcGain(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_51_L, REG_DISP_TOP_OP2_1_51_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcOffset(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_50_L, REG_DISP_TOP_OP2_1_50_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x0FFF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcHact(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_6B_L, REG_DISP_TOP_OP2_1_6B_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x0FFF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcHblank(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_6D_L, REG_DISP_TOP_OP2_1_6D_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcVact(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_6C_L, REG_DISP_TOP_OP2_1_6C_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x0FFF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcPwmDuty(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_53_L, REG_DISP_TOP_OP2_1_53_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }
}

void _HalDispMaceSetDlcPwmPeriod(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_52_L, REG_DISP_TOP_OP2_1_52_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }
}

//----------------- HCoring -----------------
void _HalDispMaceHCoringSetYBand1En( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetYBand2En( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0002 : 0x0000, 0x0002, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetYDitherEn( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0004 : 0x0000, 0x0004, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetYTableStep( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, (u16Val & 0x07) << 4, 0x0070, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetCBand1En( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0100 : 0x0000, 0x0100, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetCBand2En( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0200 : 0x0000, 0x0200, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetCDitherEn( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0400 : 0x0000, 0x0400, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetCTableStep( u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, (u16Val & 0x07) << 12, 0x7000, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetPcMode(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0008 : 0x0000, 0x0008, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetHighPassEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1D_L, u16Val ? 0x0080 : 0x0000, 0x0080, pCmdqCtx);
    }
}

void _HalDispMaceHCoringSetTable(u16 u16Table0, u16 u16Table1, u16 u16Table2, u16 u16Table3, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1E_L, u16Table0 << 0, 0x00FF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1E_L, u16Table1 << 8, 0xFF00, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1F_L, u16Table2 << 0, 0x00FF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1F_L, u16Table3 << 8, 0xFF00, pCmdqCtx);
    }
}

// ----------------- Peaking -----------------
void _HalDispMacePeakingSetPeakEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x0400 : 0x0000, 0x0400, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetLtiEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x0800 : 0x0000, 0x0800, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetLtiMedianFilterOn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x1000 : 0x0000, 0x1000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCtiEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x2000 : 0x0000, 0x2000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCtiMedianFilterOn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x4000 : 0x0000, 0x4000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetDiffAdapEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand1Coef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_13_L, u16Val, 0x003F, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand1Step(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_13_L, (u16Val & 0x0003) << 6, 0x00C0, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand2Coef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_13_L, (u16Val & 0x003F) << 8, 0x3F00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand2Step(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_13_L, (u16Val & 0x0003) << 14, 0xC000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetLtiCoef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, u16Val, 0x003F, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetLtiStep(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, (u16Val & 0x0003) << 6, 0x00C0, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetPeakTerm1Sel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, (u16Val & 0x0003) << 8, 0x0300, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetPeakTerm2Sel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, (u16Val & 0x0003) << 10, 0x0C00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetPeakTerm3Sel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, (u16Val & 0x0003) << 12, 0x3000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetPeakTerm4Sel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_14_L, (u16Val & 0x0003) << 14, 0xC000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCoringTh1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_15_L, u16Val, 0x000F, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCoringTh2(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_15_L, (u16Val & 0x000F) << 4, 0x00F0, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCtiCoef(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_15_L, (u16Val & 0x001F) << 8, 0x1F00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetCtiStep(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_15_L, (u16Val & 0x0003) << 13, 0x6000, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand1PosLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_18_L, (u16Val & 0x00FF) << 0, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand1NegLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_18_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand2PosLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_19_L, (u16Val & 0x00FF) << 0, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand2NegLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_19_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetPosLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1B_L, (u16Val & 0x00FF) << 0, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetNegLimitTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1B_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand2DiffAdapEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1C_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void _HalDispMacePeakingSetBand1DiffAdapEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_1C_L, u16Val ? 0x0002 : 0x0000, 0x0002, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_02_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_02_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT2(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_03_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT2(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_03_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT3(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_04_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT3(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_04_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT4(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_05_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT4(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_05_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT5(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_06_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT5(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_06_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT6(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_07_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT6(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_07_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT7(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_08_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT7(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_08_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbT8(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_09_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCrT8(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_09_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD1DD1U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0B_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD2DD2U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0B_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD3DD3U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0C_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD4DD4U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0C_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD5DD5U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0D_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD6DD6U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0D_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD7DD7U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0E_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD8DD8U(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0E_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccCbCrD9(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_0F_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccKT2KT1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_10_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccKT4KT3(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_10_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccKT6KT5(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_11_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetFccKT8KT7(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_11_L, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void _HalDispMaceSetFccEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_MACE_12_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetGammaR(u8 *pu8R, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg;
    u32 i;
    u16 u16Val;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_01_L, REG_DISP_TOP_GAMMA_1_01_L);
    if(pCmdqCtx && u32Reg)
    {
        for(i=0; i<32; i+=2)
        {
            u16Val = pu8R[i] | ((u16)pu8R[i+1]) << 8;
            HalDispUtilityW2BYTEMSK(u32Reg + i, u16Val, 0xFFFF, pCmdqCtx);
        }
    }
    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_11_L, REG_DISP_TOP_GAMMA_1_11_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg,  pu8R[32], 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetGammaG(u8 *pu8G, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg;
    u32 i;
    u16 u16Val;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_12_L, REG_DISP_TOP_GAMMA_1_12_L);
    if(pCmdqCtx && u32Reg)
    {
        for(i=0; i<32; i+=2)
        {
            u16Val = pu8G[i] | ((u16)pu8G[i+1]) << 8;
            HalDispUtilityW2BYTEMSK(u32Reg + i, u16Val, 0xFFFF, pCmdqCtx);
        }
    }

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_22_L, REG_DISP_TOP_GAMMA_1_22_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, pu8G[32], 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetGammaB(u8 *pu8B, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg;
    u32 i;
    u16 u16Val;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_23_L, REG_DISP_TOP_GAMMA_1_23_L);
    if(pCmdqCtx && u32Reg)
    {
        for(i=0; i<32; i+=2)
        {
            u16Val = pu8B[i] | ((u16)pu8B[i+1]) << 8;
            HalDispUtilityW2BYTEMSK(u32Reg + i, u16Val, 0xFFFF, pCmdqCtx);
        }
    }

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_33_L, REG_DISP_TOP_GAMMA_1_33_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg,  pu8B[32], 0x00FF, pCmdqCtx);
    }
}

void _HalDispMaceSetGammaEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_GAMMA_0_00_L, REG_DISP_TOP_GAMMA_1_00_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}


void _HalDispMaceSet3x3MatrixRangeEn(u8 u8Id, u8 u8En, u8 u8RangeR, u8 u8RangeG, u8 u8RangeB, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg;
    u16 u16Val;

    if(u8Id == 0)
    {
        __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_30_L, REG_DISP_TOP_OP2_1_30_L);
    }
    else
    {

        __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_3C_L, REG_DISP_TOP_OP2_1_3C_L);
    }
    u16Val = u8En ? 0x0001 : 0x0000;
    u16Val |= u8RangeR ? 0x0002 : 0x0000;
    u16Val |= u8RangeG ? 0x0004 : 0x0000;
    u16Val |= u8RangeB ? 0x0008 : 0x0000;

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x000F, pCmdqCtx);
    }
}

void _HalDispMaceSet3x3MatrixCoef(u8 u8Id, u16 *pu16Coef, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u8 i;
    u32 u32Reg;

    if(u8Id == 0)
    {
        __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_31_L, REG_DISP_TOP_OP2_1_31_L);
    }
    else
    {
        __HalDispMaceGetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_3D_L, REG_DISP_TOP_OP2_1_3D_L);
    }

    if(pCmdqCtx && u32Reg)
    {
        for(i=0; i<9; i++)
        {
            HalDispUtilityW2BYTEMSK(u32Reg + (i*2), pu16Coef[i], 0x0FFF, pCmdqCtx);
        }
    }
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HalDispMaceGetHwCtx(HalDispMaceHwContext_t **pstMaceCtx, void *pCtx)
{
    *pstMaceCtx = &gstHwMaceCtx;
}

void HalDispMaceSetHwCtxFlag(HalDispMaceFlag_e enFlag, void *pCtx)
{
    gstHwMaceCtx.enFlag |= enFlag;
}

void HalDispMaceClearHwCtxFlag(HalDispMaceFlag_e enFlag, void *pCtx)
{
    gstHwMaceCtx.enFlag &= ~enFlag;
}

void HalDispMaceSetBwExtensionConfig(HalDispMaceBwExtensionConfig_t *pstBwCfg, void *pCtx)
{
    if(pstBwCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_BW, pCtx);
        _HalDispMaceSetBw2sbri(pstBwCfg->u8Bw2sbri, pCtx);
        _HalDispMaceSetBleEn(pstBwCfg->u8BleEn, pCtx);
        _HalDispMaceSetWleEn(pstBwCfg->u8WleEn, pCtx);
        _HalDispMaceSetBlackSlop(pstBwCfg->u8BlackSlop, pCtx);
        _HalDispMaceSetBlackStart(pstBwCfg->u8BlackStart, pCtx);
        _HalDispMaceSetWhiteSlop(pstBwCfg->u8WhiteSlop, pCtx);
        _HalDispMaceSetWhiteStart(pstBwCfg->u8WhiteStart, pCtx);
        pstBwCfg->bUpdate = 0;
    }
}

void HalDispMaceSetLpfConfig(HalDispMaceLpfConfig_t *pstLpfCfg, void *pCtx)
{
    if(pstLpfCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_LPF, pCtx);
        _HalDispMaceSetLpfY(pstLpfCfg->u8LpfY, pCtx);
        _HalDispMaceSetLpfC(pstLpfCfg->u8LpfC, pCtx);
        pstLpfCfg->bUpdate = 0;
    }
}

void HalDispMaceSetDlcDcrConfig(HalDispMaceDlcDcrConfig_t *pstDlcDcrCfg, void *pCtx)
{
    if(pstDlcDcrCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_DLC_DCR, pCtx);

        _HalDispMaceSetDcrEn(pstDlcDcrCfg->u16DcrEn, pCtx);
        _HalDispMaceSetDcrOffset(pstDlcDcrCfg->u16DcrOffset, pCtx);

        _HalDispMaceSetYGain(pstDlcDcrCfg->u16YGain, pCtx);
        _HalDispMaceSetYGainOffset(pstDlcDcrCfg->u16YGainOffset, pCtx);

        _HalDispMaceSetDlcHact(pstDlcDcrCfg->u16Hactive, pCtx);
        _HalDispMaceSetDlcVact(pstDlcDcrCfg->u16Vactive, pCtx);
        _HalDispMaceSetDlcHblank(pstDlcDcrCfg->u16Hblank, pCtx);
        _HalDispMaceSetDlcPwmDuty(pstDlcDcrCfg->u16PwmDuty, pCtx);
        _HalDispMaceSetDlcPwmPeriod(pstDlcDcrCfg->u16PwmPeriod, pCtx);

        _HalDispMaceSetDlcEn(pstDlcDcrCfg->u16DlcEn, pCtx);
        _HalDispMaceSetDlcGain(pstDlcDcrCfg->u16DlcGain, pCtx);
        _HalDispMaceSetDlcOffset(pstDlcDcrCfg->u16DlcOffset, pCtx);

        pstDlcDcrCfg->bUpdate = 0;
    }
}

void HalDispMaceSetHCoringConfig(HalDispMaceHCoringConfig_t *pstHCoringCfg, void *pCtx)
{
    if(pstHCoringCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_HCORING, pCtx);
        _HalDispMaceHCoringSetYBand1En(pstHCoringCfg->u8YBand1HCoringEn, pCtx);
        _HalDispMaceHCoringSetYBand2En(pstHCoringCfg->u8YBand2HCoringEn, pCtx);
        _HalDispMaceHCoringSetYDitherEn(pstHCoringCfg->u8HCoringYDither_En, pCtx);
        _HalDispMaceHCoringSetYTableStep(pstHCoringCfg->u8YTableStep, pCtx);

        _HalDispMaceHCoringSetCBand1En(pstHCoringCfg->u8CBand1HCoringEn, pCtx);
        _HalDispMaceHCoringSetCBand2En(pstHCoringCfg->u8CBand2HCoringEn, pCtx);
        _HalDispMaceHCoringSetCDitherEn(pstHCoringCfg->u8HCoringCDither_En, pCtx);
        _HalDispMaceHCoringSetCTableStep(pstHCoringCfg->u8CTableStep, pCtx);

        _HalDispMaceHCoringSetPcMode(pstHCoringCfg->u8PcMode, pCtx);
        _HalDispMaceHCoringSetHighPassEn(pstHCoringCfg->u8HighPassEn, pCtx);

        _HalDispMaceHCoringSetTable(
            pstHCoringCfg->u8CoringTable0, pstHCoringCfg->u8CoringTable1,
            pstHCoringCfg->u8CoringTable2, pstHCoringCfg->u8CoringTable3,
             pCtx);
        pstHCoringCfg->bUpdate = 0;
    }
}


void HalDispMaceSetPeaking(HalDispMacePeakingConfig_t *pstPeakingCfg, void *pCtx)
{
    if(pstPeakingCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Coef1=%x, Coef2:%x\n",
            __FUNCTION__, __LINE__, pstPeakingCfg->u8Band1Coef, pstPeakingCfg->u8Band2Coef);

        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_PEAKING, pCtx);
        _HalDispMacePeakingSetPeakEn(pstPeakingCfg->u8PeakingEn, pCtx);
        _HalDispMacePeakingSetLtiEn(pstPeakingCfg->u8LtiEn, pCtx);
        _HalDispMacePeakingSetLtiMedianFilterOn(pstPeakingCfg->u8LtiMediaFilterOn, pCtx);
        _HalDispMacePeakingSetCtiEn(pstPeakingCfg->u8CtiEn, pCtx);
        _HalDispMacePeakingSetCtiMedianFilterOn(pstPeakingCfg->u8CtiMediaFilterOn, pCtx);
        _HalDispMacePeakingSetDiffAdapEn(pstPeakingCfg->u8DiffAdapEn, pCtx);
        _HalDispMacePeakingSetBand1Coef(pstPeakingCfg->u8Band1Coef, pCtx);
        _HalDispMacePeakingSetBand1Step(pstPeakingCfg->u8Band1Step, pCtx);
        _HalDispMacePeakingSetBand2Coef(pstPeakingCfg->u8Band2Coef, pCtx);
        _HalDispMacePeakingSetBand2Step(pstPeakingCfg->u8Band2Step, pCtx);
        _HalDispMacePeakingSetLtiCoef(pstPeakingCfg->u8LtiCoef, pCtx);
        _HalDispMacePeakingSetLtiStep(pstPeakingCfg->u8LtiStep, pCtx);
        _HalDispMacePeakingSetPeakTerm1Sel(pstPeakingCfg->u8PeakingTerm1Sel, pCtx);
        _HalDispMacePeakingSetPeakTerm2Sel(pstPeakingCfg->u8PeakingTerm2Sel, pCtx);
        _HalDispMacePeakingSetPeakTerm3Sel(pstPeakingCfg->u8PeakingTerm3Sel, pCtx);
        _HalDispMacePeakingSetPeakTerm4Sel(pstPeakingCfg->u8PeakingTerm4Sel, pCtx);
        _HalDispMacePeakingSetCoringTh1(pstPeakingCfg->u8CoringTh1, pCtx);
        _HalDispMacePeakingSetCoringTh2(pstPeakingCfg->u8CoringTh2, pCtx);
        _HalDispMacePeakingSetCtiCoef(pstPeakingCfg->u8CtiCoef, pCtx);
        _HalDispMacePeakingSetCtiStep(pstPeakingCfg->u8CtiStep, pCtx);
        _HalDispMacePeakingSetBand1PosLimitTh(pstPeakingCfg->u8Band1PosLimitTh, pCtx);
        _HalDispMacePeakingSetBand1NegLimitTh(pstPeakingCfg->u8Band1NegLimitTh, pCtx);
        _HalDispMacePeakingSetBand2PosLimitTh(pstPeakingCfg->u8Band2PosLimitTh, pCtx);
        _HalDispMacePeakingSetBand2NegLimitTh(pstPeakingCfg->u8Band2NegLimitTh, pCtx);
        _HalDispMacePeakingSetPosLimitTh(pstPeakingCfg->u8PosLimitTh, pCtx);
        _HalDispMacePeakingSetNegLimitTh(pstPeakingCfg->u8NegLimitTh, pCtx);
        _HalDispMacePeakingSetBand2DiffAdapEn(pstPeakingCfg->u8Band2DiffAdapEn, pCtx);
        _HalDispMacePeakingSetBand1DiffAdapEn(pstPeakingCfg->u8Band1DiffAdapEn, pCtx);
        pstPeakingCfg->bUpdate = 0;
    }
}

void HalDispMaceSetFccConfig(HalDispMaceFccConfig_t *pstFccCfg, void *pCtx)
{
    if(pstFccCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_FCC, pCtx);
        _HalDispMaceSetFccCbT1(pstFccCfg->u8Cb_T1, pCtx);
        _HalDispMaceSetFccCrT1(pstFccCfg->u8Cr_T1, pCtx);
        _HalDispMaceSetFccCbT2(pstFccCfg->u8Cb_T2, pCtx);
        _HalDispMaceSetFccCrT2(pstFccCfg->u8Cr_T2, pCtx);
        _HalDispMaceSetFccCbT3(pstFccCfg->u8Cb_T3, pCtx);
        _HalDispMaceSetFccCrT3(pstFccCfg->u8Cr_T3, pCtx);
        _HalDispMaceSetFccCbT4(pstFccCfg->u8Cb_T4, pCtx);
        _HalDispMaceSetFccCrT4(pstFccCfg->u8Cr_T4, pCtx);
        _HalDispMaceSetFccCbT5(pstFccCfg->u8Cb_T5, pCtx);
        _HalDispMaceSetFccCrT5(pstFccCfg->u8Cr_T5, pCtx);
        _HalDispMaceSetFccCbT6(pstFccCfg->u8Cb_T6, pCtx);
        _HalDispMaceSetFccCrT6(pstFccCfg->u8Cr_T6, pCtx);
        _HalDispMaceSetFccCbT7(pstFccCfg->u8Cb_T7, pCtx);
        _HalDispMaceSetFccCrT7(pstFccCfg->u8Cr_T7, pCtx);
        _HalDispMaceSetFccCbT8(pstFccCfg->u8Cb_T8, pCtx);
        _HalDispMaceSetFccCrT8(pstFccCfg->u8Cr_T8, pCtx);
        _HalDispMaceSetFccCbCrD1DD1U(pstFccCfg->u8CbCr_D1D_D1U, pCtx);
        _HalDispMaceSetFccCbCrD2DD2U(pstFccCfg->u8CbCr_D2D_D2U, pCtx);
        _HalDispMaceSetFccCbCrD3DD3U(pstFccCfg->u8CbCr_D3D_D3U, pCtx);
        _HalDispMaceSetFccCbCrD4DD4U(pstFccCfg->u8CbCr_D4D_D4U, pCtx);
        _HalDispMaceSetFccCbCrD5DD5U(pstFccCfg->u8CbCr_D5D_D5U, pCtx);
        _HalDispMaceSetFccCbCrD6DD6U(pstFccCfg->u8CbCr_D6D_D6U, pCtx);
        _HalDispMaceSetFccCbCrD7DD7U(pstFccCfg->u8CbCr_D7D_D7U, pCtx);
        _HalDispMaceSetFccCbCrD8DD8U(pstFccCfg->u8CbCr_D8D_D8U, pCtx);
        _HalDispMaceSetFccCbCrD9(pstFccCfg->u8CbCr_D9, pCtx);
        _HalDispMaceSetFccKT2KT1(pstFccCfg->u8K_T2_K_T1, pCtx);
        _HalDispMaceSetFccKT4KT3(pstFccCfg->u8K_T4_K_T3, pCtx);
        _HalDispMaceSetFccKT6KT5(pstFccCfg->u8K_T6_K_T5, pCtx);
        _HalDispMaceSetFccKT8KT7(pstFccCfg->u8K_T8_K_T7, pCtx);
        _HalDispMaceSetFccEn(pstFccCfg->u8En, pCtx);
        pstFccCfg->bUpdate = 0;
    }
}

void HalDispMaceSetGammaConfig(HalDispMaceGammaConfig_t *pstGammaCfg, void *pCtx)
{
    if(pstGammaCfg->bUpdate)
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_GAMMA, pCtx);
        _HalDispMaceSetGammaR(pstGammaCfg->u8R, pCtx);
        _HalDispMaceSetGammaG(pstGammaCfg->u8G, pCtx);
        _HalDispMaceSetGammaB(pstGammaCfg->u8B, pCtx);
        _HalDispMaceSetGammaEn(pstGammaCfg->u8En, pCtx);
        pstGammaCfg->bUpdate = 0;
    }
}

void HalDispMaceSet3x3MatrixConfig(HalDispMace3x3MatrixConfig_t *pst3x3MatrixCfg, void *pCtx)
{
    if(pst3x3MatrixCfg->bUpdate)
    {
        u16 u16Coef[9];

        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d\n", __FUNCTION__, __LINE__);
        HalDispMaceSetHwCtxFlag(E_HAL_DISP_MACE_FLAG_3X3MATRIX, pCtx);
        u16Coef[0] = pst3x3MatrixCfg->u16Coef11;
        u16Coef[1] = pst3x3MatrixCfg->u16Coef12;
        u16Coef[2] = pst3x3MatrixCfg->u16Coef13;
        u16Coef[3] = pst3x3MatrixCfg->u16Coef21;
        u16Coef[4] = pst3x3MatrixCfg->u16Coef22;
        u16Coef[5] = pst3x3MatrixCfg->u16Coef23;
        u16Coef[6] = pst3x3MatrixCfg->u16Coef31;
        u16Coef[7] = pst3x3MatrixCfg->u16Coef32;
        u16Coef[8] = pst3x3MatrixCfg->u16Coef33;

         _HalDispMaceSet3x3MatrixCoef(pst3x3MatrixCfg->u8Id, u16Coef, pCtx);
         _HalDispMaceSet3x3MatrixRangeEn(pst3x3MatrixCfg->u8Id, pst3x3MatrixCfg->bEn,
            pst3x3MatrixCfg->u8RangeR, pst3x3MatrixCfg->u8RangeG, pst3x3MatrixCfg->u8RangeB, pCtx);

        pst3x3MatrixCfg->bUpdate = 0;
    }
}

