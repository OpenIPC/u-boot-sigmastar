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
#include "hal_disp_op2.h"
#include "drv_disp_ctx.h"
#include "hal_disp_irq.h"
#include "hal_disp_vga_timing_tbl.h"
#include "hal_disp_cvbs_timing_tbl.h"
#include "hal_hdmitx_top_tbl.h"

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
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HalDispSetPatGenMd(u8 u8Val, void *pCtx, u32 u32DevId)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_PATGEN_MODE);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val << shift_of_disp_top0_reg_patgen_mode, mask_of_disp_top0_reg_patgen_mode,
                                pCmdqCtx);
    }
    else if (u32Reg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u8Val << shift_of_disp_top0_reg_patgen_mode,
                                      mask_of_disp_top0_reg_patgen_mode, pCmdqCtx);
    }
}
void HalDispSetMaceSrcSel(u16 u16Val, void *pCtx, u32 u32DevId)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_MACE_SRC_SEL);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? mask_of_disp_top0_reg_mace_src_sel : 0x0000,
                                mask_of_disp_top0_reg_mace_src_sel, pCmdqCtx);
    }
    else if (u32Reg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val ? mask_of_disp_top0_reg_mace_src_sel : 0x0000,
                                      mask_of_disp_top0_reg_mace_src_sel, pCmdqCtx);
    }
}

void HalDispSetSwReset(u8 u8Val, void *pCtx, u32 u32DevId)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_SW_RST);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, ((u16)u8Val) << shift_of_disp_top0_reg_sw_rst,
                                (0x1 << shift_of_disp_top0_reg_sw_rst), pCmdqCtx);
    }
    else if (u32Reg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, ((u16)u8Val) << shift_of_disp_top0_reg_sw_rst,
                                      (0x1 << shift_of_disp_top0_reg_sw_rst), pCmdqCtx);
    }
}
void HalDispSetVgaDtopDumpByDiv(u32 u32Mulit, u32 u32Div, void *pCtx)
{
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_EMMCPLL_PDIV, HAL_DISP_GET_VGAPLLPOSTDIVIDX(u32Div),
                            mask_of_dac_atop_reg_emmcpll_pdiv, pCmdqCtx);
}
void HalDispSetHdmitxAtopDumpByDiv(u32 u32Mulit, u32 u32Div, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u8 u8DrivingUp = 0;
    u8 u8Val = 0;
    while(u32Div/2)
    {
        u32Div /= 2;
        u8Val++;
    }
    if(u8Val == 1 || u8Val == 0)
    {
        u8DrivingUp = 1;
    }
    u8Val |= 0x10;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_HDMITXPLL_OUTPUT_DIV, u8Val,
                            mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_output_div, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_HDMITXPLL_TMDS_DIV, u8Val,
                            mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_tmds_div, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_16_L, (u8DrivingUp ? 0x7f0 : 0xf0),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_22_L, (u8DrivingUp ? 0x2b2b : 0x1414),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_23_L, (u8DrivingUp ? 0x142b : 0x1414),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_24_L, (u8DrivingUp ? 0x1313 : 0),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_25_L, (u8DrivingUp ? 0x13 : 0),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_30_L, (u8DrivingUp ? 0x80f : 0xf0f),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_31_L, (u8DrivingUp ? 0x80f : 0xf0f),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_32_L, (u8DrivingUp ? 0x8 : 0xf),
                            0xffff, pCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HDMI_TX_ATOP_3B_L, (u8DrivingUp ? 0x2333 : 0x2222),
                            0xffff, pCmdqCtx);
}
void HalDispSetHdmitxRst(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(HDMI_TX_REG_HDMI_TX_SW_RST, u8Val ? mask_of_hdmi_tx_reg_hdmi_tx_sw_rst : 0x0000,
                                mask_of_hdmi_tx_reg_hdmi_tx_sw_rst, pCmdqCtx);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(HDMI_TX_REG_HDMI_TX_SW_RST, u8Val ? mask_of_hdmi_tx_reg_hdmi_tx_sw_rst : 0x0000,
                                      mask_of_hdmi_tx_reg_hdmi_tx_sw_rst, pCmdqCtx);
    }
}


void HalDispSetTgenHtt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_H_TOTAL);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_h_total, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_H_TOTAL, u16Val, mask_of_hdmi_tx_reg_h_total, pCmdqCtx);
        }
    }
}

void HalDispSetTgenVtt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_V_TOTAL);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_v_total, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_V_TOTAL, u16Val, mask_of_hdmi_tx_reg_v_total, pCmdqCtx);
        }
    }
}

void HalDispSetTgenHsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HS_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_hs_start, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_HS_START, u16Val, mask_of_hdmi_tx_reg_hs_start, pCmdqCtx);
        }
    }
}

void HalDispSetTgenHsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HS_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_hs_end, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_HS_END, u16Val, mask_of_hdmi_tx_reg_hs_end, pCmdqCtx);
        }
    }
}

void HalDispSetTgenVsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_VS_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_vs_start, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_VS_START, u16Val, mask_of_hdmi_tx_reg_vs_start, pCmdqCtx);
        }
    }
}

void HalDispSetTgenVsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_VS_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_vs_end, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_VS_END, u16Val, mask_of_hdmi_tx_reg_vs_end, pCmdqCtx);
        }
    }
}

void HalDispSetTgenHfdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HFDE_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_hfde_start, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_HFDE_START, u16Val, mask_of_hdmi_tx_reg_hfde_start, pCmdqCtx);
        }
    }
}

void HalDispSetTgenHfdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HFDE_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_hfde_end, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_HFDE_END, u16Val, mask_of_hdmi_tx_reg_hfde_end, pCmdqCtx);
        }
    }
}

void HalDispSetTgenVfdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_VFDE_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_vfde_start, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_VFDE_START, u16Val, mask_of_hdmi_tx_reg_vfde_start, pCmdqCtx);
        }
    }
}

void HalDispSetTgenVfdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_VFDE_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_vfde_end, pCmdqCtx);
        if(DISP_OUTDEV_IS_HDMI(HalDispGetInterface(pCtx, 0)))
        {
            HalDispUtilityW2BYTEMSK(HDMI_TX_REG_VFDE_END, u16Val, mask_of_hdmi_tx_reg_vfde_end, pCmdqCtx);
        }
    }
}

void HalDispSetTgenHdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_HDE_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_w0_hde_start, pCmdqCtx);
    }
}

void HalDispSetTgenHdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_HDE_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_w0_hde_end, pCmdqCtx);
    }
}

void HalDispSetTgenVdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_VDE_START);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_w0_vde_start, pCmdqCtx);
    }
}

void HalDispSetTgenVdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_VDE_END);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_tgen_w0_vde_end, pCmdqCtx);
    }
}

void HalDispSetTgenDacHsyncSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_HS_START, u16Val,
                                mask_of_disp_top_op20_reg_tgen_dac_hs_start, pCmdqCtx);
    }
}

void HalDispSetTgenDacHsyncEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_HS_END, u16Val, mask_of_disp_top_op20_reg_tgen_dac_hs_end,
                                pCmdqCtx);
    }
}

void HalDispSetTgenDacHdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_HFDE_START, u16Val,
                                mask_of_disp_top_op20_reg_tgen_dac_hfde_start, pCmdqCtx);
    }
}

void HalDispSetTgenDacHdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_HFDE_END, u16Val,
                                mask_of_disp_top_op20_reg_tgen_dac_hfde_end, pCmdqCtx);
    }
}

void HalDispSetTgenDacVdeSt(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_VFDE_START, u16Val,
                                mask_of_disp_top_op20_reg_tgen_dac_vfde_start, pCmdqCtx);
    }
}

void HalDispSetTgenDacVdeEnd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_DAC_VFDE_END, u16Val,
                                mask_of_disp_top_op20_reg_tgen_dac_vfde_end, pCmdqCtx);
    }
}
void HalDispSetRgbOrder(void *pCtx, u8 u8R, u8 u8G, u8 u8B)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_OUTPUT_SWAP_SEL_B);
    if (u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8B << shift_of_disp_top_op20_reg_output_swap_sel_b,
                                mask_of_disp_top_op20_reg_output_swap_sel_b, pCmdqCtx);
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_OUTPUT_SWAP_SEL_G);
        HalDispUtilityW2BYTEMSK(u32Reg, u8G << shift_of_disp_top_op20_reg_output_swap_sel_g,
                                mask_of_disp_top_op20_reg_output_swap_sel_g, pCmdqCtx);
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_OUTPUT_SWAP_SEL_R);
        HalDispUtilityW2BYTEMSK(u32Reg, u8R << shift_of_disp_top_op20_reg_output_swap_sel_r,
                                mask_of_disp_top_op20_reg_output_swap_sel_r, pCmdqCtx);
    }
}
void HalDispSetRgbMode(void *pCtx, HaldispDeviceRgbType_e enMode, u8 bMlSwap)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (u32Reg)
    {
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_RGB_MODE);
        HalDispUtilityW2BYTEMSK(u32Reg, enMode << shift_of_disp_top_op20_reg_rgb_mode,
                                mask_of_disp_top_op20_reg_rgb_mode, pCmdqCtx);
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_RGB_ML_SWAP);
        HalDispUtilityW2BYTEMSK(u32Reg, bMlSwap << shift_of_disp_top_op20_reg_rgb_ml_swap,
                                mask_of_disp_top_op20_reg_rgb_ml_swap, pCmdqCtx);
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_RGB_DITH_MODE);
        HalDispUtilityW2BYTEMSK(u32Reg, (enMode >=E_HAL_DISP_DEVICE_RGB565) ? mask_of_disp_top_op20_reg_rgb_dith_mode : 0,
                                mask_of_disp_top_op20_reg_rgb_dith_mode, pCmdqCtx);
        u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_RGB_DITH_EN);
        HalDispUtilityW2BYTEMSK(u32Reg, (enMode !=E_HAL_DISP_DEVICE_RGB888) ? mask_of_disp_top_op20_reg_rgb_dith_en : 0,
                                mask_of_disp_top_op20_reg_rgb_dith_en, pCmdqCtx);                               
    }
}
void HalDispSetDacMux(void *pCtx, u8 u8Val)
{
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_MUX, u8Val << shift_of_disp_top0_reg_dac_mux,
                                mask_of_disp_top0_reg_dac_mux, pCmdqCtx);
}
void HalDispSetTgenExtHsEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_EXT_HS_EN);

    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? mask_of_disp_top_op20_reg_tgen_ext_hs_en : 0x0000,
                                mask_of_disp_top_op20_reg_tgen_ext_hs_en, pCmdqCtx);
    }
}

u16 HalDispGetTgenHtt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_H_TOTAL);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenVtt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_V_TOTAL);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHsyncSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HS_START);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHsyncEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HS_END);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHfdeSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HFDE_START);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHfdeEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_HFDE_END);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHdeSt(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_HDE_START);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

u16 HalDispGetTgenHdeEnd(void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_TGEN_W0_HDE_END);
    if (u32Reg)
    {
        return HalDispUtilityR2BYTEDirect(u32Reg);
    }
    return 0;
}

void HalDispSetDacTrimming(u16 u16R, u16 u16G, u16 u16B, void *pCtx)
{
    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d DacTrim(0x%x, 0x%x, 0x%x)\n", __FUNCTION__, __LINE__, u16B, u16G, u16B);

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DAC_ATOP_REG_GCR_IDAC_TRIM_B, (u16B),
                                             mask_of_dac_atop_reg_gcr_idac_trim_b);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DAC_ATOP_REG_GCR_IDAC_TRIM_G, (u16G),
                                             mask_of_dac_atop_reg_gcr_idac_trim_g);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DAC_ATOP_REG_GCR_IDAC_TRIM_R, (u16R),
                                             mask_of_dac_atop_reg_gcr_idac_trim_r);
}

void HalDispGetDacTriming(u16 *pu16R, u16 *pu16G, u16 *pu16B, void *pCtx)
{
    *pu16B = HalDispUtilityR2BYTEDirect(DAC_ATOP_REG_GCR_IDAC_TRIM_B) & mask_of_dac_atop_reg_gcr_idac_trim_b;
    *pu16G = HalDispUtilityR2BYTEDirect(DAC_ATOP_REG_GCR_IDAC_TRIM_G) & mask_of_dac_atop_reg_gcr_idac_trim_g;
    *pu16R = HalDispUtilityR2BYTEDirect(DAC_ATOP_REG_GCR_IDAC_TRIM_R) & mask_of_dac_atop_reg_gcr_idac_trim_r;
}

void HalDispSetDacAffReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Msk   = (mask_of_disp_top0_reg_dac_aff_full_clr | mask_of_disp_top0_reg_dac_aff_empty_clr);
    u16   u16Rst   = (mask_of_disp_top0_reg_dac_aff_sw_rst);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_AFF_SW_RST, u8Val ? (u16Msk | u16Rst) : 0, (u16Msk | u16Rst),
                                pCmdqCtx);
    }
}

void HalDispSetDacReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Msk   = (mask_of_disp_top0_reg_dac_sw_rst);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_SW_RST, u8Val ? u16Msk : 0x0000, u16Msk, pCmdqCtx);
    }
}

void HalDispSetVgaHpdInit(void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_HOT_PLUG_EN, mask_of_disp_top0_reg_dac_hot_plug_en,
                                mask_of_disp_top0_reg_dac_hot_plug_en, pCmdqCtx); // dac_hpd_en
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_HP_H_LEN, 0x0110, mask_of_disp_top0_reg_dac_hp_h_len,
                                pCmdqCtx); // dac_hpd_len
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_HP_LATCH_DELAY, 0x00FF, mask_of_disp_top0_reg_dac_hp_latch_delay,
                                pCmdqCtx); // dac_hpd_delay
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_HP_DATA_CODE, 0x0050, mask_of_disp_top0_reg_dac_hp_data_code,
                                pCmdqCtx); // dac_hpd_data_code
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_GCR_LEV_PLUGDET_RGB, 0x0000, mask_of_dac_atop_reg_gcr_lev_plugdet_rgb,
                                pCmdqCtx); // threshold, U01=0x02, U02=0x00
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_B_DET, mask_of_dac_atop_reg_en_hd_dac_b_det,
                                mask_of_dac_atop_reg_en_hd_dac_b_det, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_G_DET, mask_of_dac_atop_reg_en_hd_dac_g_det,
                                mask_of_dac_atop_reg_en_hd_dac_g_det, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_R_DET, mask_of_dac_atop_reg_en_hd_dac_r_det,
                                mask_of_dac_atop_reg_en_hd_dac_r_det, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_C_DET, 0x0, mask_of_dac_atop_reg_en_hd_dac_c_det, pCmdqCtx);
    }
}

void HalDispSetFrameColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u16   u16Val;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u16Val = u8B;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_FCOLOR_B);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_fcolor_b, pCmdqCtx);
    }
    u16Val = (u16)(u8G) << shift_of_disp_top_op20_reg_fcolor_g;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_FCOLOR_G);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_fcolor_g, pCmdqCtx);
    }
    u16Val = u8R;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_FCOLOR_R);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_fcolor_r, pCmdqCtx);
    }
}

void HalDispSetFrameColorForce(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_FCOLOR_FORCE);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? mask_of_disp_top_op20_reg_fcolor_force : 0x0000,
                                mask_of_disp_top_op20_reg_fcolor_force, pCmdqCtx);
    }
}

void HalDispSetDispWinColor(u8 u8R, u8 u8G, u8 u8B, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u16   u16Val;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u16Val = u8B;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_W0_FCOLOR_B);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_w0_fcolor_b, pCmdqCtx);
    }
    u16Val = (u16)(u8G) << shift_of_disp_top_op20_reg_w0_fcolor_g;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_W0_FCOLOR_G);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_w0_fcolor_g, pCmdqCtx);
    }
    u16Val = u8R;
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_W0_FCOLOR_R);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top_op20_reg_w0_fcolor_r, pCmdqCtx);
    }
}

void HalDispSetDispWinColorForce(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP_OP2, REG_W0_FCOLOR_FORCE);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? mask_of_disp_top_op20_reg_w0_fcolor_force : 0x0000,
                                mask_of_disp_top_op20_reg_w0_fcolor_force, pCmdqCtx);
    }
}

void HalDispSetLcdAffReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_LCD_MISC_30_L, u8Val ? 0x0002 : 0x0000, 0x0002, pCmdqCtx);
    }
}

void HalDispSetLcdFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP21_REG_TGEN_FPLL_DLY, (u16Val) << shift_of_disp_top_op20_reg_tgen_fpll_dly,
                                mask_of_disp_top_op20_reg_tgen_fpll_dly, pCmdqCtx);
    }
}

void HalDispSetLcdFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP21_REG_TGEN_FPLL_REF_SEL,
                                (u16Val) << shift_of_disp_top_op20_reg_tgen_fpll_ref_sel,
                                mask_of_disp_top_op20_reg_tgen_fpll_ref_sel, pCmdqCtx);
    }
}

void HalDispSetDacFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_FPLL_DLY_UNIT, mask_of_disp_top_op20_reg_tgen_fpll_dly_unit,
                                mask_of_disp_top_op20_reg_tgen_fpll_dly_unit, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_FPLL_DLY, (u16Val) << shift_of_disp_top_op20_reg_tgen_fpll_dly,
                                mask_of_disp_top_op20_reg_tgen_fpll_dly, pCmdqCtx);
    }
}

void HalDispSetDacFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_FPLL_REF_SEL,
                                (u16Val) << shift_of_disp_top_op20_reg_tgen_fpll_ref_sel,
                                mask_of_disp_top_op20_reg_tgen_fpll_ref_sel, pCmdqCtx);
    }
}
void HalDispSetVgaSynth(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_SYN_SET_SELF_0, u32Val,
                                mask_of_disp_top0_reg_syn_set_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_SYN_SET_SELF_1, u32Val>>16,
                                mask_of_disp_top0_reg_syn_set_1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_DAC_SYN_SET_0, u32Val,
                                mask_of_dac_atop_reg_dac_syn_set_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_DAC_SYN_SET_1, u32Val>>16,
                                mask_of_dac_atop_reg_dac_syn_set_1, pCmdqCtx);
    }
}
void HalDispSetHdmitxSynth(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_SYN_SET_0, u32Val,
                                mask_of_disp_top0_reg_syn_set_0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_SYN_SET_1, u32Val>>16,
                                mask_of_disp_top0_reg_syn_set_1, pCmdqCtx);
    }
}
void HalDispSetDacSynthSetSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_DAC_SYN_SET_SEL, u16Val ? mask_of_dac_atop_reg_dac_syn_set_sel : 0x0000,
                                mask_of_dac_atop_reg_dac_syn_set_sel, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_SYN_SET_MUX, u16Val ? mask_of_disp_top0_reg_syn_set_mux : 0,
                                mask_of_disp_top0_reg_syn_set_mux, pCmdqCtx);
    }
}

void HaldispSetDacFpllSynthSwRst(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DAC_ATOP_REG_DAC_ATOP_SW_RST, u16Val ? 0x0003 : 0x0000,
                                mask_of_dac_atop_reg_dac_atop_sw_rst, pCmdqCtx);
    }
}

void HalDispSetFpllEn(u16 u16FpllId, u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FRAME_PLL_EN);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? mask_of_disp_top0_reg_frame_pll_en : 0x0000,
                                mask_of_disp_top0_reg_frame_pll_en, pCmdqCtx);
    }
}

void HalDispSetFpllRst(u16 u16FpllId, u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FRAME_PLL_RST);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? mask_of_disp_top0_reg_frame_pll_rst : 0x0000,
                                mask_of_disp_top0_reg_frame_pll_rst, pCmdqCtx);
    }
}

void HalDispClearFpllUnLock(u16 u16FpllId, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FPLL_UNLOCKED);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, mask_of_disp_top0_reg_fpll_unlocked, mask_of_disp_top0_reg_fpll_unlocked,
                                pCmdqCtx);
    }
}
void HalDispPollFpllLock(u16 u16FpllId, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Msk;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u16Msk = HalDispGetFpllLockedStableFlag(u16FpllId);

    if (pCmdqCtx && u16Msk)
    {
        HalDispUtilityWritePollCmd(HAL_DISP_REG_FPLL_LOCK_DUMMY, u16Msk, u16Msk, 2000000000, 1, pCmdqCtx); // 2sec
    }
}

void HalDispSetFpllGain(u16 u16FpllId, u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FPLL_PHASE_GAIN);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val,
                                mask_of_disp_top0_reg_fpll_phase_gain | mask_of_disp_top0_reg_fpll_freq_gain, pCmdqCtx);
    }
}

void HalDispSetFpllThd(u16 u16FpllId, u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FPLL_PHASE_LOCK_THD);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val << shift_of_disp_top0_reg_fpll_phase_lock_thd,
                                mask_of_disp_top0_reg_fpll_phase_lock_thd, pCmdqCtx);
    }
}

void HalDispSetDacPllPowerDown(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    HalDispUtilityW2BYTEMSKByType(DAC_ATOP_REG_EMMCPLL_PD, u16Val, mask_of_dac_atop_reg_emmcpll_pd, pCmdqCtx, E_DISP_UTILITY_REG_ACCESS_CMDQ);
    if (pCmdqCtx)
    {
        HalDispUtilityWriteDelayCmd(20, pCmdqCtx);
    }
}
void HalDispSetFpllStepGain(u16 u16FpllId, u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FPLL_STEP_GAIN);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top0_reg_fpll_step_gain, pCmdqCtx);
    }
}

void HalDispSetFpllAbsLimit(u16 u16FpllId, u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Val;
    u32   u32Reg = 0;

    u32Reg = GET_DISP_RED(u16FpllId, DISP_TOP, REG_FPLL_ABS_LIMIT);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        u16Val = (u32Val & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top0_reg_fpll_abs_limit_0, pCmdqCtx);
        u16Val = (u32Val >> 16);
        HalDispUtilityW2BYTEMSK(u32Reg + 2, u16Val, mask_of_disp_top0_reg_fpll_abs_limit_1, pCmdqCtx);
    }
}

void HalDispSetDacRgbHpdInit(void)
{
#if DISP_VGA_HPD_ISR_SUPPORT
    W2BYTEMSK(DISP_TOP0_REG_DAC_HOT_PLUG_EN, mask_of_disp_top0_reg_dac_hot_plug_en,
              mask_of_disp_top0_reg_dac_hot_plug_en);                                              // dac_hpd_en
    W2BYTEMSK(DISP_TOP0_REG_DAC_HP_H_LEN, 0x0110, mask_of_disp_top0_reg_dac_hp_h_len);             // dac_hpd_len
    W2BYTEMSK(DISP_TOP0_REG_DAC_HP_LATCH_DELAY, 0x00FF, mask_of_disp_top0_reg_dac_hp_latch_delay); // dac_hpd_delay
    W2BYTEMSK(DISP_TOP0_REG_DAC_HP_DATA_CODE, 0x0120, mask_of_disp_top0_reg_dac_hp_data_code);     // dac_hpd_data_code
    W2BYTEMSK(DAC_ATOP_REG_GCR_LEV_PLUGDET_RGB, 0x0000,
              mask_of_dac_atop_reg_gcr_lev_plugdet_rgb); // threshold, U01=0x02, U02=0x00
    W2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_B_DET, mask_of_dac_atop_reg_en_hd_dac_b_det,
              mask_of_dac_atop_reg_en_hd_dac_b_det); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det
                                                     // [2]:reg_en_hd_dac_r_det
    W2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_G_DET, mask_of_dac_atop_reg_en_hd_dac_g_det,
              mask_of_dac_atop_reg_en_hd_dac_g_det); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det
                                                     // [2]:reg_en_hd_dac_r_det
    W2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_R_DET, mask_of_dac_atop_reg_en_hd_dac_r_det,
              mask_of_dac_atop_reg_en_hd_dac_r_det); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det
                                                     // [2]:reg_en_hd_dac_r_det
    W2BYTEMSK(DAC_ATOP_REG_EN_HD_DAC_C_DET, 0x0,
              mask_of_dac_atop_reg_en_hd_dac_c_det); // [0]:reg_en_hd_dac_b_det, [1]:reg_en_hd_dac_g_det
                                                     // [2]:reg_en_hd_dac_r_det
#endif
}

void HalDispSetDacExtFrmRstEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP_OP20_REG_TGEN_EXT_FRM_RESET_EN_DAC,
                                u16Val ? mask_of_disp_top_op20_reg_tgen_ext_frm_reset_en_dac : 0x0000,
                                mask_of_disp_top_op20_reg_tgen_ext_frm_reset_en_dac, pCmdqCtx);
    }
}

void HalDispSetHdmi2OdClkRate(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_HDMI_TO_ODCLK_RATE);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top0_reg_hdmi_to_odclk_rate, pCmdqCtx);
    }
}

void HalDispSetDacSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8Vgaused = 0x1<<u16Val;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DAC_SRC_MUX, u16Val << shift_of_disp_top0_reg_dac_src_mux,
                                mask_of_disp_top0_reg_dac_src_mux, pCmdqCtx);
    }
}

void HalDispSetCvbsSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    g_stDispIrqHist.stWorkingStatus.stCvbsStatus.u8Cvsbused = 0x1<<u16Val;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_CVBS_SRC_MUX, u16Val << shift_of_disp_top0_reg_cvbs_src_mux,
                                mask_of_disp_top0_reg_cvbs_src_mux, pCmdqCtx);
    }
}
void HalDispSetHdmiSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_HDMI_SRC_MUX, u16Val << shift_of_disp_top0_reg_hdmi_src_mux,
                                mask_of_disp_top0_reg_hdmi_src_mux, pCmdqCtx);
    }
}

void HalDispSetLcdSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    g_stDispIrqHist.stWorkingStatus.stLcdStatus.u8Lcdused = (0x1 <<u16Val);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_LCD_SRC_MUX, u16Val << shift_of_disp_top0_reg_lcd_src_mux,
                                mask_of_disp_top0_reg_lcd_src_mux, pCmdqCtx);
    }
}

void HalDispSetDsiSrcMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DSI_SRC_MUX, u16Val << shift_of_disp_top0_reg_dsi_src_mux,
                                mask_of_disp_top0_reg_dsi_src_mux, pCmdqCtx);
    }
}

void HalDispSetCmdqIntMask(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_CMDQ_INTR_MASK_GRP_A);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top0_reg_cmdq_intr_mask_grp_a, pCmdqCtx);
    }
}
void HalDispSetCmdqIntClear(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_CMDQ_INTR_STATUS_GRP_A);
    if (pCmdqCtx && u32Reg)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, mask_of_disp_top0_reg_cmdq_intr_status_grp_a, pCmdqCtx);
    }
}

void HalDispSetMopWinMerge(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP1_REG_MOP_WIN_MERGE, u16Val ? mask_of_disp_top1_reg_mop_win_merge : 0x0000,
                                mask_of_disp_top1_reg_mop_win_merge, pCmdqCtx);
    }
}

void HalDispSetCvbsSwRst(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_CVBS_SW_RST, u8Val ? mask_of_disp_top0_reg_cvbs_sw_rst : 0x0000,
                                mask_of_disp_top0_reg_cvbs_sw_rst, pCmdqCtx);
        HalDispUtilityWriteDelayCmd(20, pCmdqCtx);
    }
}

void HalDispSetCvbsOnOff(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        if(u8Val)
        {
            HalDispUtilityW2BYTEMSK(TVENC_REG_TVE_DAC_YOUT_EN, 0x0000,
                                    mask_of_tvenc_reg_tve_dac_yout_en, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(TVENC_REG_TVE_DAC_COUT_EN, 0x0000,
                                    mask_of_tvenc_reg_tve_dac_cout_en, pCmdqCtx);
        }
        HalDispUtilityW2BYTEMSK(TVENC_REG_TVIF_EN, u8Val ? mask_of_tvenc_reg_tvif_en : 0x0000,
                                mask_of_tvenc_reg_tvif_en, pCmdqCtx);
        if(u8Val)
        {
            HalDispUtilityWriteDelayCmd(1000000, (void *)pCmdqCtx);
            HalDispUtilityW2BYTEMSK(TVENC_REG_TVE_DAC_YOUT_EN, mask_of_tvenc_reg_tve_dac_yout_en,
                                    mask_of_tvenc_reg_tve_dac_yout_en, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(TVENC_REG_TVE_DAC_COUT_EN, mask_of_tvenc_reg_tve_dac_cout_en,
                                    mask_of_tvenc_reg_tve_dac_cout_en, pCmdqCtx);
        }
    }
}

/*------------------------------------------------------
 For i80/m68 interface
------------------------------------------------------*/
void HalDispSetClkDsiGateEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DSI_CLK_GATE_EN, u8Val ? mask_of_disp_top0_reg_dsi_clk_gate_en : 0x00,
                                mask_of_disp_top0_reg_dsi_clk_gate_en, pCmdqCtx);
    }
}

void HalDispLcdAffGateEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_LCD_MISC_32_L, u8Val ? 0x01 : 0x00, 0x01, pCmdqCtx);
    }
}
void HalDispMcuLcdRefresh(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_LCD_00_L, u8Val ? 0x42 : 0x00, 0x42, pCmdqCtx);
    }
}

void HalDispMcuLcdReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_LCD_00_L, u8Val ? 0x1 : 0x00, 0x1, pCmdqCtx);
    }
}

void HalDispKeepMcuReset(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    // direct riu write
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_SW_RST);
    W2BYTEMSK(u32Reg, (0x11) << shift_of_disp_top0_reg_sw_rst, mask_of_disp_top0_reg_sw_rst);
    WBYTEMSK(REG_LCD_MISC_00_L, u8Val, 0x1);
    WBYTEMSK(REG_LCD_MISC_00_L, 0, 0x1);
}
void HalDispSetCmdqMux(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg   = 0;
    u32   u32DevId = HalDispGetDeviceId(pCtx, 0);

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = GET_DISP_RED(u32DevId, DISP_TOP, REG_SC_TOP_HW_RESERVE);

    if (pCmdqCtx && u32Reg)
    {
#if (CMDQ_POLL_EQ_MODE == 0)
        HalDispUtilityW2BYTEMSK(u32Reg, u8Val ? mask_of_reg_cmdq_mux : 0x00, mask_of_reg_cmdq_mux, pCmdqCtx);
#endif
    }
}

void HalDispMcuLcdCmdqInit(void)
{
    u32 u32Reg = REG_LCD_08_L;
#if (CMDQ_POLL_EQ_MODE)
    W2BYTEMSK(u32Reg, 0x00, 0x1);
    W2BYTEMSK(u32Reg, 0x00, 0x200);
#else
    W2BYTEMSK(u32Reg, 0x00, 0x200);
#endif
}

void HalDispDumpRegTab(void *pCtx, u16 *pData, u16 u16RegNum, u16 u16DataSize, u8 u8DataOffset)
{
    u16   i, j;
    u32   u32Addr;
    u16   u16Mask, u16Value;
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    for (i = 0; i < u16RegNum; i++)
    {
        j = i * (REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + u16DataSize);

        u32Addr  = ((u32)pData[j]) << 16 | (u32)(pData[j + 1]);
        u16Mask  = pData[j + 2];
        u16Value = pData[j + 3 + u8DataOffset];

        HalDispUtilityW2BYTEMSK(u32Addr, u16Value, u16Mask, pCmdqCtx);
    }
}
void HalDispSetVgaHVsyncPad(void *pCtx, bool bEn)
{
    if(DrvDispOsPadMuxActive() == 0)
    {
        HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_PADTOP_77_L, bEn ? 0x0300 : 0x0000, 0x0300);
    }
}

void HalDispSetDsiClkGateEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DSI_CLK_GATE_EN, u16Val ? mask_of_disp_top0_reg_dsi_clk_gate_en : 0x0000,
                                mask_of_disp_top0_reg_dsi_clk_gate_en, pCmdqCtx);
    }
}

void HalDispSetDsiAff(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DSI_AFF_FULL_THR, u16Val,
                                mask_of_disp_top0_reg_dsi_aff_full_thr | mask_of_disp_top0_reg_dsi_aff_md_sel,
                                pCmdqCtx);
    }
}
void HalDispSetDsiRst(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Msk   = mask_of_disp_top0_reg_dsi_dst_sw_rstz | mask_of_disp_top0_reg_dsi_src_sw_rstz;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(DISP_TOP0_REG_DSI_DST_SW_RSTZ, u16Val ? u16Msk : 0, u16Msk, pCmdqCtx);
    }
}
u16 HalDispGetFpllLockedStableFlag(u16 u16FpllId)
{
    u16 u16Ret = 0;

    if (u16FpllId == HAL_DISP_FPLL_0_VGA_HDMI)
    {
        u16Ret = HAL_DISP_REG_FPLL0_LOCKED_FLAG;
    }
    else if (u16FpllId == HAL_DISP_FPLL_1_CVBS_DAC)
    {
        u16Ret = HAL_DISP_REG_FPLL1_LOCKED_FLAG;
    }
    return u16Ret;
}
void HalDispSetFpllLockedStableFlag(u16 u16FpllId, u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16   u16Msk;

    u16Msk = HalDispGetFpllLockedStableFlag(u16FpllId);
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(HAL_DISP_REG_FPLL_LOCK_DUMMY, u16Val ? u16Msk : 0, u16Msk, pCmdqCtx);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(HAL_DISP_REG_FPLL_LOCK_DUMMY, u16Val ? u16Msk : 0, u16Msk, NULL);
    }
}

static void _HalDispSetFpll0UsedFlag(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(HAL_DISP_REG_FPLL_LOCK_DUMMY, u16Val, 0xF, pCmdqCtx);
    }
}
static void _HalDispSetFpll1UsedFlag(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(HAL_DISP_REG_FPLL_LOCK_DUMMY, u16Val << 8, 0xF00, pCmdqCtx);
    }
}
void HalDispSetFpllUsedFlag(u16 u16FpllId, u16 u16Dev, void *pCtx)
{
    u16 u16Val = DISP_FPLL_DEVICE_FLAG(u16Dev);

    if (u16FpllId == HAL_DISP_FPLL_0_VGA_HDMI)
    {
        _HalDispSetFpll0UsedFlag(u16Val, pCtx);
    }
    else if (u16FpllId == HAL_DISP_FPLL_1_CVBS_DAC)
    {
        _HalDispSetFpll1UsedFlag(u16Val, pCtx);
    }
}
static u16 _HalDispGetFpll0UsedByDev(u16 u16Dev)
{
    return (R2BYTE(HAL_DISP_REG_FPLL_LOCK_DUMMY) & DISP_FPLL_DEVICE_FLAG(u16Dev)) ? 1 : 0;
}
static u16 _HalDispGetFpll1UsedByDev(u16 u16Dev)
{
    return (R2BYTE(HAL_DISP_REG_FPLL_LOCK_DUMMY) & (DISP_FPLL_DEVICE_FLAG(u16Dev) << 8)) ? 1 : 0;
}
u16 HalDispGetFpllUsedByDev(u16 u16FpllId, u16 u16Dev)
{
    u16 u16Ret = 0;

    if (u16FpllId == HAL_DISP_FPLL_0_VGA_HDMI)
    {
        u16Ret = _HalDispGetFpll0UsedByDev(u16Dev);
    }
    else if (u16FpllId == HAL_DISP_FPLL_1_CVBS_DAC)
    {
        u16Ret = _HalDispGetFpll1UsedByDev(u16Dev);
    }
    return u16Ret;
}
void HalDispSetTimeGenStartFlag(u16 u16Val, void *pCtx, u32 u32DevId)
{
    void *pCmdqCtx = NULL;
    u32   u32Reg;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32Reg = REG_HAL_DISP_UTILIYT_CMDQDEV_TIMEGEN_START(u32DevId);
    if (u32Reg == 0)
    {
        return;
    }
    g_stDispIrqHist.stWorkingStatus.stDevStatus[u32DevId].u8bStartTimegen = u16Val;
    g_stDispIrqHist.stWorkingStatus.stTimeZoneStatus[u32DevId].u8TimeZoneused = u16Val;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val ? REG_CMDQ_DEV_TIMEGEN_START_MSK : 0, REG_CMDQ_DEV_TIMEGEN_START_MSK,
                                pCmdqCtx);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val ? REG_CMDQ_DEV_TIMEGEN_START_MSK : 0,
                                      REG_CMDQ_DEV_TIMEGEN_START_MSK, NULL);
    }
}
void HalDispSetInterCfgRstMop(u16 u16Val, u32 u32DevId)
{
    u32 u32Reg = u32DevId == HAL_DISP_DEVICE_ID_1 ? REG_DISP_MOPG_BK10_00_L : REG_DISP_MOPG_BK00_00_L;
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32Reg, u16Val ? 0x0001 : 0x0000, 0x0001);
}

void HalDispSetInterCfgRstLcd(u16 u16Val)
{
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_LCD_00_L, u16Val ? 0x0001 : 0x0000, 0x0001);
}
void HalDispOp2SetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode)
{
    void *pCmdqCtx = NULL;

    if (u8bBypass)
    {
        HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_OP20_REG_DLC_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_OP20_REG_DLC_EN),
                                      0, mask_of_disp_top_op20_reg_dlc_en, pCmdqCtx, enRiuMode);
        HalDispUtilityW2BYTEMSKByType(DISP_TOP_OP20_REG_DCR_EN
                                          + HAL_DISP_GET_BANKOFFSET(u32Dev, DISP_TOP_OP20_REG_DCR_EN),
                                      0, mask_of_disp_top_op20_reg_dcr_en, pCmdqCtx, enRiuMode);
    }
}
