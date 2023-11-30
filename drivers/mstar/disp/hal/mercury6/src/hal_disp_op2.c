/*
* hal_disp_op2.c- Sigmastar
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

#define _HAL_DISP_OP2_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "hal_disp_vga_timing_tbl.h"
#include "hal_disp_cvbs_timing_tbl.h"
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
void _HalDispOp2GetCmdqAndReg(void *pCtx, void **pCmdqCtx, u32 *pu32Reg,  u32 u32Reg0, u32 u32Reg1)
{
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain = NULL;

    if(HalDispGetCmdqByCtx(pCtx, pCmdqCtx) == 0)
    {
        *pCmdqCtx = NULL;
        *pu32Reg = 0;
    }
    else
    {
        if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
        {
            pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];
        }
        else if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
        {
            pstDevContain =
                (DrvDispCtxDeviceContain_t *)pstDispCtx->pstCtxContain->pstVidLayerContain[pstDispCtx->u32Idx]->pstDevCtx;
        }
        else if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
        {
            pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstDispCtx->pstCtxContain->pstInputPortContain[pstDispCtx->u32Idx]->pstVidLayerContain;
            pstDevContain = (DrvDispCtxDeviceContain_t *)pstVidLayerContain->pstDevCtx;
        }

        if(pstDevContain)
        {
            *pu32Reg = pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0 ? u32Reg0 :
                       pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1 ? u32Reg1 :
                                                                         0;
        }
    }
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HalDispSetPatGenMd(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_07_L, REG_DISP_TOP_1_07_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val << 1,  0x000E, pCmdqCtx);
    }
}


void HalDispSetSwReste(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_00_L, REG_DISP_TOP_1_00_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8Val) << 8, 0xFF00, pCmdqCtx);
    }
}

void HalDispClearDisp0SwReset(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_00_L, REG_DISP_TOP_0_00_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, 0x0000, 0xFF00, pCmdqCtx);
    }
}

void HalDispSetDacMux(u8 u8Val, void *pCtx)
{

}

void HalDispSetTgenHtt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_11_L, REG_DISP_TOP_OP2_1_11_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVtt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_12_L, REG_DISP_TOP_OP2_1_12_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_13_L, REG_DISP_TOP_OP2_1_13_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_14_L, REG_DISP_TOP_OP2_1_14_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_15_L, REG_DISP_TOP_OP2_1_15_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_16_L, REG_DISP_TOP_OP2_1_16_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHfdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_17_L, REG_DISP_TOP_OP2_1_17_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHfdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_18_L, REG_DISP_TOP_OP2_1_18_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVfdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_19_L, REG_DISP_TOP_OP2_1_19_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVfdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1A_L, REG_DISP_TOP_OP2_1_1A_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1D_L, REG_DISP_TOP_OP2_1_1D_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenHdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1E_L, REG_DISP_TOP_OP2_1_1E_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1F_L, REG_DISP_TOP_OP2_1_1F_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenVdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_20_L, REG_DISP_TOP_OP2_1_20_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacHsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_23_L, REG_DISP_TOP_OP2_0_23_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacHsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_24_L, REG_DISP_TOP_OP2_0_24_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacHdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_27_L, REG_DISP_TOP_OP2_0_27_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacHdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_28_L, REG_DISP_TOP_OP2_0_28_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacVdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_29_L, REG_DISP_TOP_OP2_0_29_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenDacVdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_2A_L, REG_DISP_TOP_OP2_0_2A_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispSetTgenExtHsEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_10_L, REG_DISP_TOP_OP2_0_10_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0004 : 0x0000, 0x0004, pCmdqCtx);
    }
}

u16 HalDispGetTgenHtt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_11_L, REG_DISP_TOP_OP2_1_11_L);

    return  HalDispUtilityR2BYTEDirect(u32Reg);
}

u16 HalDispGetTgenVtt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_12_L, REG_DISP_TOP_OP2_1_12_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHsyncSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_13_L, REG_DISP_TOP_OP2_1_13_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHsyncEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_14_L, REG_DISP_TOP_OP2_1_14_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHfdeSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_17_L, REG_DISP_TOP_OP2_1_17_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHfdeEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_18_L, REG_DISP_TOP_OP2_1_18_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHdeSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1D_L, REG_DISP_TOP_OP2_1_1D_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u16 HalDispGetTgenHdeEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1E_L, REG_DISP_TOP_OP2_1_1E_L);
    if(pCmdqCtx && u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    else
    {
        return 0;
    }
}

u32 HalDispGetSynthSet(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Val = 0;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_11_L, REG_DISP_TOP_1_11_L);
    if(pCmdqCtx && u32Reg)
    {
        u32Val = (HalDispUtilityR2BYTEDirect(u32Reg) & 0x00FF) << 16;
    }

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_10_L, REG_DISP_TOP_1_10_L);
    if(pCmdqCtx && u32Reg)
    {
        u32Val |= HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return u32Val;
}

void HalDispSetHdmiSynthSet(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16Val = 0;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_11_L, REG_DISP_TOP_1_11_L);

    if(pCmdqCtx)
    {
        u16Val = u32Val & 0x0000FFFF;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_10_L, u16Val, 0xFFFF, pCmdqCtx);
        u16Val = (u32Val & 0xFFFF0000) >> 16;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_11_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispSetDacTrimming(u16 u16R, u16 u16G, u16 u16B, void *pCtx)
{
    void *pCmdqCtx = NULL;

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d DacTrim(0x%x, 0x%x, 0x%x)\n", __FUNCTION__, __LINE__, u16B, u16G, u16B);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DAC_1A_L, (u16B & 0x007F), 0x007F, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DAC_1B_L, (u16G & 0x007F), 0x007F, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DAC_1C_L, (u16R & 0x007F), 0x007F, pCmdqCtx);
    }
}

void HalDispGetDacTriming(u16 *pu16R, u16 *pu16G, u16 *pu16B, void *pCtx)
{
    *pu16B = HalDispUtilityR2BYTEDirect(REG_DISP_DAC_1A_L) & 0x007F;
    *pu16G = HalDispUtilityR2BYTEDirect(REG_DISP_DAC_1B_L) & 0x007F;
    *pu16R = HalDispUtilityR2BYTEDirect(REG_DISP_DAC_1C_L) & 0x007F;
}

void HalDispSetDacAffReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_30_L, REG_DISP_TOP_0_30_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispSetVgaHpdInit(void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_34_L, 0x0001, 0x0001, pCmdqCtx); // dac_hpd_en
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_37_L, 0x0110, 0x0FFF, pCmdqCtx); // dac_hpd_len
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_38_L, 0x00FF, 0x00FF, pCmdqCtx); // dac_hpd_delay
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_39_L, 0x0050, 0x00FF, pCmdqCtx); // dac_hpd_data_code
        HalDispUtilityW2BYTEMSK(REG_DISP_DAC_1E_L, 0x0000, 0x0003, pCmdqCtx); // threshold, U01=0x02, U02=0x00
        HalDispUtilityW2BYTEMSK(REG_DISP_DAC_14_L, 0x0007, 0x000F, pCmdqCtx); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det [2]:reg_en_hd_dac_r_det
    }
}


void HalDispSetHdmitxSsc(u16 u16Step, u16 u16Span, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_HDMITX_ATOP_05_L, u16Step, 0x07FF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_HDMITX_ATOP_06_L, u16Span, 0x7FFF, pCmdqCtx);
    }
}

void HalDispSetFrameColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    u16 u16Val;

    u16Val = (u8B & 0xFF) | (u16)(u8G & 0xFF) << 8;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1B_L, REG_DISP_TOP_OP2_1_1B_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }

    u16Val = u8R & 0xFF;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1C_L, REG_DISP_TOP_OP2_1_1C_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispSetFrameColorForce(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_1C_L, REG_DISP_TOP_OP2_1_1C_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
    }
}

void HalDispSetDispWinColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    u16 u16Val;

    u16Val = (u8B & 0xFF) | (u16)(u8G & 0xFF) << 8;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_21_L, REG_DISP_TOP_OP2_1_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }

    u16Val = u8R & 0xFF;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_22_L, REG_DISP_TOP_OP2_1_22_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispSetDispWinColorForce(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_22_L, REG_DISP_TOP_OP2_1_22_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
    }
}

void HalDispSetLcdAffReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_LCD_MISC_30_L, REG_DISP_TOP_0_30_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x0002 : 0x0000, 0x0002, pCmdqCtx);
    }
}

void HalDispSetLcdFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_1_10_L, REG_DISP_TOP_OP2_1_10_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void HalDispSetLcdFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_1_10_L, REG_DISP_TOP_OP2_1_10_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, (u16Val & 0x0003) << 6, 0x00C0, pCmdqCtx);
    }
}

void HalDispSetDacFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_10_L, REG_DISP_TOP_OP2_0_10_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, (u16Val & 0x00FF) << 8, 0xFF00, pCmdqCtx);
    }
}

void HalDispSetDacFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_10_L, REG_DISP_TOP_OP2_0_10_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, (u16Val & 0x0003) << 6, 0x00C0, pCmdqCtx);
    }
}

void HalDispSetDacSynthSetSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_DAC_05_L, REG_DISP_DAC_05_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0010 : 0x0000, 0x0010, pCmdqCtx);
    }
}

void HalDispSetDacFpllEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_15_L, REG_DISP_TOP_0_15_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x0001: 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispSetDacFpllGain(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_1B_L, REG_DISP_TOP_0_1B_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispSetDacFpllThd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_1B_L, REG_DISP_TOP_0_1B_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val << 8, 0xFF00, pCmdqCtx);
    }
}


void HalDispSetDacFpllAbsLimit(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    u16 u16Val;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_1C_L, REG_DISP_TOP_0_1C_L);
    if(pCmdqCtx && u32Reg)
    {
        u16Val = (u32Val & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
        u16Val = (u32Val >> 16);
        HalDispUtilityW2BYTEMSK(u32Reg+2, u16Val, 0xFFFF, pCmdqCtx);
    }
}

void HalDispSetDacRgbHpdInit(void)
{
#if DISP_VGA_HPD_ISR_SUPPORT
    W2BYTEMSK(REG_DISP_TOP_0_34_L, 0x0001, 0x0001); // dac_hpd_en
    W2BYTEMSK(REG_DISP_TOP_0_37_L, 0x0110, 0x0FFF); // dac_hpd_len
    W2BYTEMSK(REG_DISP_TOP_0_38_L, 0x00FF, 0x00FF); // dac_hpd_delay
    W2BYTEMSK(REG_DISP_TOP_0_39_L, 0x0120, 0x03FF); // dac_hpd_data_code
    W2BYTEMSK(REG_DISP_DAC_1E_L, 0x0000, 0x0003); // threshold, U01=0x02, U02=0x00
    W2BYTEMSK(REG_DISP_DAC_14_L, 0x0007, 0x000F); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det [2]:reg_en_hd_dac_r_det
#endif
}


void HalDispSetDacExtFrmRstEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_OP2_0_10_L, REG_DISP_TOP_OP2_0_10_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0010 : 0x0000, 0x0010, pCmdqCtx);
    }
}

void HalDispSetHdmi2OdClkRate(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_20_L, REG_DISP_TOP_1_20_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0x000F, pCmdqCtx);
    }
}

void HalDispSetDacSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_21_L, REG_DISP_TOP_0_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispSetCvbsSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_21_L, REG_DISP_TOP_0_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0010 : 0x0000, 0x0010, pCmdqCtx);
    }
}
void HalDispSetHdmiSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_21_L, REG_DISP_TOP_0_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0002 : 0x0000, 0x0002, pCmdqCtx);
    }
}

void HalDispSetLcdSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_21_L, REG_DISP_TOP_0_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0004 : 0x0000, 0x0004, pCmdqCtx);
    }
}

void HalDispSetDsiSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_21_L, REG_DISP_TOP_0_21_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0008 : 0x0000, 0x0008, pCmdqCtx);
    }
}

void HalDispSetCmdqIntMask(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_41_L, REG_DISP_TOP_1_41_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }
}

void HalDispSetMaceSrcSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_07_L, REG_DISP_TOP_1_07_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispSetMopWinMerge(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_1_20_L, REG_DISP_TOP_1_20_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x1000 : 0x0000, 0x1000, pCmdqCtx);
    }
}

void HalDispSetCvbsSwRst(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_TVENC_40_L, REG_TVENC_40_L);
    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

/*------------------------------------------------------
 For i80/m68 interface
------------------------------------------------------*/
void HalDispSetClkDsiGateEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = REG_DISP_TOP_0_25_L;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_25_L, REG_DISP_TOP_0_25_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x10 : 0x00, 0x10, pCmdqCtx);
    }
}

void HalDispLcdAffGateEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = REG_DISP_TOP_0_25_L;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_LCD_MISC_32_L, REG_LCD_MISC_32_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x01 : 0x00, 0x01, pCmdqCtx);
    }
}
void HalDispMcuLcdRefresh(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_LCD_00_L, REG_LCD_00_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x42 : 0x00, 0x42, pCmdqCtx);
    }
}

void HalDispMcuLcdReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_LCD_00_L, REG_LCD_00_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x1 : 0x00, 0x1, pCmdqCtx);
    }
}

void HalDispKeepMcuReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    //direct riu write
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_00_H, REG_DISP_TOP_1_00_H);
    WBYTEMSK(u32Reg, (0x11), 0x11);
    WBYTEMSK(REG_LCD_MISC_00_L, u8Val, 0x1);
    WBYTEMSK(REG_LCD_MISC_00_L, 0, 0x1);
}
void HalDispSetCmdqMux(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_00_L, REG_DISP_TOP_1_00_L);

    if(pCmdqCtx && u32Reg)
    {
        #if (CMDQ_POLL_EQ_MODE ==0)
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? 0x1 : 0x00, 0x1, pCmdqCtx);
        #endif
    }
}

void HalDispMcuLcdCmdqInit(u32 u32Interface)
{
    u32 u32Reg = REG_LCD_08_L;
    #if (CMDQ_POLL_EQ_MODE)
    W2BYTEMSK(u32Reg, 0x00, 0x1);
    W2BYTEMSK(u32Reg, (u32Interface == HAL_DISP_INTF_MCU) ? 0 : 0x200, 0x200);
    #else
    W2BYTEMSK(u32Reg, 0x00, 0x200);
    #endif
}

void HalDispDumpRegTab(void *pCtx, u8 *pData, u16 u16RegNum, u16 u16DataSize, u8 u8DataOffset)
{
    u16 i, j;
    u32 u32Addr;
    u16 u16Mask, u16Value;
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;

    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, 0, 0);

    for(i=0; i< u16RegNum; i++)
    {
        j = i *  (REG_TBL_ADDR_SIZE+REG_TBL_MASK_SIZE+u16DataSize);

        u32Addr = ((u32)pData[j]) << 16 | ((u32)pData[j+1]) << 8 | (u32)(pData[j+2]);
        u16Mask  = pData[j+3];
        u16Value = pData[j+4+u8DataOffset];

        HalDispUtilityWBYTEMSK(u32Addr, u16Value, u16Mask, pCmdqCtx);
    }
}

void HalDispSetVgaHVsyncPad(void *pCtx)
{
    if(DrvDispOsPadMuxActive() == 0)
    {
        HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_PADTOP_77_L, 0x0300, 0x0300);
    }
}

void HalDispSetDsiClkGateEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_25_L, REG_DISP_TOP_0_25_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? 0x0010 : 0x0000, 0x0010, pCmdqCtx);
    }
}


void HalDispSetDsiAff(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32Reg = 0;
    _HalDispOp2GetCmdqAndReg(pCtx, &pCmdqCtx, &u32Reg, REG_DISP_TOP_0_24_L, REG_DISP_TOP_0_24_L);

    if(pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, 0xFFFF, pCmdqCtx);
    }
}

