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
#define GetRegVal(reg, type) \
    ((HalDispUtilityR2BYTEDirect(reg) & mask_of_disp_gp_ctrl_reg_ckg_##type) >> shift_of_disp_gp_ctrl_reg_ckg_##type)

;
#define GetClkRate(eDevType, eDev)                                                                                     \
    (eDevType == E_HAL_DISP_CLK_GP_CTRL_HDMI                                        ? HAL_DISP_ODCLK_SEL_HDMI          \
     : eDevType == E_HAL_DISP_CLK_GP_CTRL_DAC                                       ? HAL_DISP_ODCLK_SEL_DAC_DIV2      \
     : (eDevType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI && eDev == HAL_DISP_DEVICE_ID_0) ? HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK \
     : (eDevType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI && eDev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK \
     : (eDevType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI && eDev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_ODCLK_SEL_DISP2_PIX_CLK \
     : eDevType == E_HAL_DISP_CLK_GP_CTRL_CVBS                                      ? HAL_DISP_ODCLK_SEL_TVENC_27M     \
     : eDevType == E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC                                  ? HAL_DISP_ODCLK_SEL_HDMI          \
    : ((eDevType & E_HAL_DISP_CLK_GP_CTRL_LCD) && eDev == HAL_DISP_DEVICE_ID_0)     ? HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK \
    : ((eDevType & E_HAL_DISP_CLK_GP_CTRL_LCD) && eDev == HAL_DISP_DEVICE_ID_1)     ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK \
    : ((eDevType & E_HAL_DISP_CLK_GP_CTRL_LCD) && eDev == HAL_DISP_DEVICE_ID_2)     ? HAL_DISP_ODCLK_SEL_DISP2_PIX_CLK \
                                                                                    : HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK)
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 u32Reg;
    u16 u16Mak;
    u16 u16shift;
} HalDispClkSetReg_t;

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
HalDispClkGpCtrlConfig_t g_stDispClkGpCtrlCfg[HAL_DISP_DEVICE_MAX] = {{0, 0}, {0, 0}, {0, 0}};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HalDispClkFillRegStruct(HalDispClkSetReg_t *pCfg, u32 u32Reg, u16 u16Mak, u16 u16shift)
{
    pCfg->u32Reg = u32Reg;
    pCfg->u16shift = u16shift;
    pCfg->u16Mak = u16Mak;
}
static void _HalDispClkGetVal(u16 u16Val, u8 *pbEn, u32 *pu32ClkRate)
{
    *pbEn        = u16Val & (0x0001) ? 0 : 1;
    *pu32ClkRate = (u16Val >> (2));
}
static u16 _HalDispClkSetVal(u8 bEn, u32 u32ClkRate, u16 u16shift)
{
    u16 u16Val;

    u16Val = bEn ? 0 : (1 << u16shift);
    u16Val |= (u32ClkRate) << (u16shift + 2);
    return u16Val;
}
static void _HalDispClkSetMipiPixelG(u8 bEn, u32 u32Dev)
{
    u16 u16Val;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK1
                     : (u32Dev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK2
                                                        : HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK0;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_MIPI_PIXEL_G, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g);
}

static void _HalDispClkSetMipiPixel(u8 bEn, u32 u32Dev)
{
    u16 u16Val;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK1
                     : (u32Dev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK2
                                                        : HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK0;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_mipi_pixel);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_MIPI_PIXEL, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_mipi_pixel);
}

static void _HalDispClkSetLcdSrc(u8 bEn, u32 u32Dev)
{
    u16 u16Val;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_LCD_SRC_SEL_ODCLK1
                     : (u32Dev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_CLK_LCD_SRC_SEL_ODCLK2
                                                        : HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_lcd_src);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_LCD_SRC, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_lcd_src);
}

static void _HalDispClkSetLcd(u8 bEn, u32 u32Dev)
{
    u16 u16Val;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_LCD_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_LCD_SEL_ODCLK1
                     : (u32Dev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_CLK_LCD_SEL_ODCLK2
                                                        : HAL_DISP_CLK_LCD_SEL_ODCLK0;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_lcd);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_LCD, u16Val, mask_of_disp_gp_ctrl_reg_ckg_lcd);
}
void HalDispClkSetHdmiPowerOnOff(u8 bEn)
{
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(HDMI_TX_ATOP_REG_PD_LDO_CLKTREE, bEn ? 0 : mask_of_hdmi_tx_atop_reg_pd_ldo_clktree,
                              mask_of_hdmi_tx_atop_reg_pd_ldo_clktree);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(HDMI_TX_ATOP_REG_PD_HDMITXPLL, bEn ? 0 : mask_of_hdmi_tx_atop_reg_pd_hdmitxpll,
                              mask_of_hdmi_tx_atop_reg_pd_hdmitxpll);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(HDMI_TX_ATOP_REG_PD_DRV_BIASGEN_CH,
                              bEn ? 0 : mask_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch,
                              mask_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(HDMI_TX_ATOP_REG_PD_BIASGEN, bEn ? 0 : mask_of_hdmi_tx_atop_reg_pd_biasgen,
                              mask_of_hdmi_tx_atop_reg_pd_biasgen);
}

static void _HalDispClkSetHdmi(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_hdmi);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_HDMI, u16Val, mask_of_disp_gp_ctrl_reg_ckg_hdmi);
    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_hdmi_div2);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_HDMI_DIV2, u16Val, mask_of_disp_gp_ctrl_reg_ckg_hdmi_div2);
    if(bEn == 0)
    {
        HalDispClkSetHdmiPowerOnOff(bEn);
    }
}
static void _HalDispClkSetTve(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_tve);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_TVE, u16Val, mask_of_disp_gp_ctrl_reg_ckg_tve);
}

static void _HalDispClkSetDac(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_dac);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_DAC, u16Val, mask_of_disp_gp_ctrl_reg_ckg_dac);
}

static void _HalDispClkSetDacWaff(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_dac_waff);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_DAC_WAFF, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_dac_waff);
}

static void _HalDispClkSetMipiTxDsiApb(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_MIPI_TX_DSI_APB, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb);
}
static u8 _HalDispClkSetDispPixelAll(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    u8  u8Ret = 1;
    u8  idx;
    u8  u8LcdIdx;
    u8  u8LcdDev[HAL_DISP_DEVICE_MAX] = {HAL_DISP_DEVICE_MAX, HAL_DISP_DEVICE_MAX};
    u8  u8Lcdcnt                      = 0;
    u32 u32Rate                       = 0;
    u8  u8bPllUsed[HAL_DISP_PLL_CNT]  = {0, 0};

    for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
    {
        if (u8bEn[idx] == 1 && eDevType[idx] & E_HAL_DISP_CLK_GP_CTRL_LCD)
        {
            u8LcdDev[u8Lcdcnt] = idx;
            u8Lcdcnt++;
        }
        if (u8bEn[idx] == 1 && eDevType[idx] == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
        {
            HalDispClkSetDispPixel(idx, 1, HAL_DISP_CLK_DISP_PIXEL_SEL_320M);
            u8bPllUsed[HAL_DISP_PLL_LPLL] = 1;
        }
        else
        {
            HalDispClkSetDispPixel(idx, 1, HAL_DISP_CLK_DISP_PIXEL_SEL_288M);
        }
        if (u8bEn[idx] == 1 && ((eDevType[idx] & E_HAL_DISP_CLK_GP_CTRL_BT1120_SDR)||
                                 (eDevType[idx] & E_HAL_DISP_CLK_GP_CTRL_BT1120_DDR)))
        {
            HalDispClkSetPixel2Bt1120(1, idx);
        }
    }
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_CVBS)
    {
        u8bPllUsed[HAL_DISP_PLL_DISPPLL] = 1;
    }
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        for (idx = 0; idx < HAL_DISP_PLL_CNT; idx++)
        {
            if (u8bPllUsed[idx] == 0)
            {

                if(eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_BT1120_SDR)
                {
                    u32Rate = (idx == HAL_DISP_PLL_LPLL)  ? HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL_DIV4
                          : (idx == HAL_DISP_PLL_DISPPLL) ? HAL_DISP_CLK_DISP_PIXEL_SEL_DISPLL_DIV4
                                                          : 0;
                    HalDispClkSetBt1120(1, idx);
                }
                else if(eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_BT1120_DDR)
                {
                    u32Rate = (idx == HAL_DISP_PLL_LPLL)  ? HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL_DIV2
                          : (idx == HAL_DISP_PLL_DISPPLL) ? HAL_DISP_CLK_DISP_PIXEL_SEL_DISPLL_DIV2
                                                          : 0;
                    HalDispClkSetBt1120(1, idx);
                }
                else
                {
                    u32Rate = (idx == HAL_DISP_PLL_LPLL)  ? HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL
                          : (idx == HAL_DISP_PLL_DISPPLL) ? HAL_DISP_CLK_DISP_PIXEL_SEL_DISP_PLL
                                                          : 0;
                    HalDispClkSetBt1120(0, 0);
                }
                for (u8LcdIdx = 0; u8LcdIdx < u8Lcdcnt; u8LcdIdx++)
                {
                    HalDispClkSetDispPixel(u8LcdDev[u8LcdIdx], 1, u32Rate);
                }
                u8bPllUsed[idx] = 1;
                break;
            }
        }
        if (u32Rate == 0)
        {
            u8Ret = 0;
            DISP_ERR("%s %d, PLL already used on other interface(MIPI & CVBS), Lcd can't use.\n", __FUNCTION__,
                     __LINE__);
        }
    }
    return u8Ret;
}
static void _HalDispClkSetDispMipiDsi(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    u8 idx;
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
    {
        for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
        {
            if (u8bEn[idx] == 1 && eDevType[idx] == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI)
            {
                _HalDispClkSetMipiPixelG(1, idx);
                _HalDispClkSetMipiPixel(1, idx);
            }
        }
        HalDispClkSetMipiTxDsi(1, HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL);
        _HalDispClkSetMipiTxDsiApb(1, HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0);
    }
    else
    {
        _HalDispClkSetMipiPixelG(0, HAL_DISP_DEVICE_ID_0);
        _HalDispClkSetMipiPixel(0, HAL_DISP_DEVICE_ID_0);
        HalDispClkSetMipiTxDsi(0, HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL);
        _HalDispClkSetMipiTxDsiApb(0, HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0);
    }
}
static void _HalDispClkSetDispLcd(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    u8 idx;
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_LCD)
    {
        for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
        {
            if (u8bEn[idx] == 1 && eDevType[idx] & E_HAL_DISP_CLK_GP_CTRL_LCD)
            {
                _HalDispClkSetLcdSrc(1, idx);
                _HalDispClkSetLcd(1, idx);
                HalDispClkSetFpllOdclk(1, idx, HAL_DISP_FPLL_1_CVBS_DAC);
            }
        }
    }
    else
    {
        _HalDispClkSetLcdSrc(0, HAL_DISP_DEVICE_ID_0);
        _HalDispClkSetLcd(0, HAL_DISP_DEVICE_ID_0);
        HalDispClkSetFpllOdclk(0, HAL_DISP_DEVICE_ID_0, HAL_DISP_FPLL_1_CVBS_DAC);
    }
}
static void _HalDispClkSetDispHdmi(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_HDMI)
    {
        _HalDispClkSetHdmi(1, HAL_DISP_CLK_HDMI_SEL_HDMI);
    }
    else
    {
        _HalDispClkSetHdmi(0, HAL_DISP_CLK_HDMI_SEL_HDMI);
    }
}
static void _HalDispClkSetDispCvbs(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_CVBS)
    {
        _HalDispClkSetTve(1, HAL_DISP_CLK_TVE_SEL_CVBS);
    }
    else
    {
        _HalDispClkSetTve(0, HAL_DISP_CLK_TVE_SEL_CVBS);
    }
}
static void _HalDispClkSetDispDac(HalDispClkGpCtrlType_e eDevTypeEn, HalDispClkGpCtrlType_e *eDevType, u8 *u8bEn)
{
    u8 idx;
    if (eDevTypeEn & E_HAL_DISP_CLK_GP_CTRL_DAC)
    {
        _HalDispClkSetDacWaff(1, HAL_DISP_CLK_DAC_WAFF_SEL_DAC_DIV2);
        for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
        {
            if (u8bEn[idx] == 1 && eDevType[idx] & (E_HAL_DISP_CLK_GP_CTRL_DAC))
            {
                HalDispClkSetFpllOdclk(1, idx, HAL_DISP_FPLL_0_VGA_HDMI);
                if(eDevType[idx] & (E_HAL_DISP_CLK_GP_CTRL_HDMI))
                {
                    _HalDispClkSetDacWaff(1, HAL_DISP_CLK_DAC_WAFF_SEL_HDMI);
                }
            }
        }
        _HalDispClkSetDac(1, HAL_DISP_CLK_DAC_SEL_DAC);
    }
    else
    {
        _HalDispClkSetDac(0, HAL_DISP_CLK_DAC_SEL_DAC);
        _HalDispClkSetDacWaff(0, HAL_DISP_CLK_DAC_WAFF_SEL_HDMI);
        HalDispClkSetFpllOdclk(0, HAL_DISP_DEVICE_ID_0, HAL_DISP_FPLL_0_VGA_HDMI);
    }
}

//------------------------------------------------------------------------------
// Global Function
//------------------------------------------------------------------------------
void HalDispClkGetOdclk(u32 u32Dev, u8 *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val = 0;

    if (u32Dev == HAL_DISP_DEVICE_ID_0)
    {
        u16Val = GetRegVal(DISP_GP_CTRL_REG_CKG_ODCLK0, odclk0);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_1)
    {
        u16Val = GetRegVal(DISP_GP_CTRL_REG_CKG_ODCLK1, odclk1);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_2)
    {
        u16Val = GetRegVal(DISP_GP_CTRL_REG_CKG_ODCLK2, odclk2);
    }
    _HalDispClkGetVal(u16Val, pbEn, pu32ClkRate);
}
void HalDispClkSetOdclk(u32 u32Dev, u8 bEn, u32 u32ClkRate)
{
    u16 u16Val  = 0;
    HalDispClkSetReg_t stCfg;

    if (u32Dev == HAL_DISP_DEVICE_ID_0)
    {
        _HalDispClkFillRegStruct(&stCfg, DISP_GP_CTRL_REG_CKG_ODCLK0,
            mask_of_disp_gp_ctrl_reg_ckg_odclk0, shift_of_disp_gp_ctrl_reg_ckg_odclk0);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_1)
    {
        _HalDispClkFillRegStruct(&stCfg, DISP_GP_CTRL_REG_CKG_ODCLK1,
            mask_of_disp_gp_ctrl_reg_ckg_odclk1, shift_of_disp_gp_ctrl_reg_ckg_odclk1);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_2)
    {
        _HalDispClkFillRegStruct(&stCfg, DISP_GP_CTRL_REG_CKG_ODCLK2,
            mask_of_disp_gp_ctrl_reg_ckg_odclk2, shift_of_disp_gp_ctrl_reg_ckg_odclk2);
    }
    else
    {
        DISP_ERR("%s %d, Dev Not Support:%d\n", __FUNCTION__, __LINE__, u32Dev);
        return;
    }
    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, stCfg.u16shift);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(stCfg.u32Reg, u16Val, stCfg.u16Mak);
}
void HalDispClkSetFpllOdclk(u8 bEn, u32 u32Dev, u32 u32FpllId)
{
    HalDispClkSetReg_t stCfg;
    u16 u16Val  = 0;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_FPLL_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_FPLL_SEL_ODCLK1
                     : (u32Dev == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_CLK_FPLL_SEL_ODCLK2
                                                        : HAL_DISP_CLK_FPLL_SEL_ODCLK0;

    if(u32FpllId == HAL_DISP_FPLL_0_VGA_HDMI)
    {
        _HalDispClkFillRegStruct(&stCfg, DISP_GP_CTRL_REG_CKG_FPLL_ODCLK,
            mask_of_disp_gp_ctrl_reg_ckg_fpll_odclk, shift_of_disp_gp_ctrl_reg_ckg_fpll_odclk);
    }
    else if(u32FpllId == HAL_DISP_FPLL_1_CVBS_DAC)
    {
        _HalDispClkFillRegStruct(&stCfg, DISP_GP_CTRL_REG_CKG_FPLL_ODCLK1,
            mask_of_disp_gp_ctrl_reg_ckg_fpll_odclk1, shift_of_disp_gp_ctrl_reg_ckg_fpll_odclk1);
    }
    else
    {
        DISP_ERR("%s %d, FpllId Not Support:%d\n", __FUNCTION__, __LINE__, u32FpllId);
        return;
    }
    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, stCfg.u16shift);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(stCfg.u32Reg, u16Val, stCfg.u16Mak);
}

void HalDispClkSetDma(u8 bEn, u32 u32Dev)
{
    u16 u16Val;
    u32 u32ClkRate = (u32Dev == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0
                     : (u32Dev == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK1
                                                        : HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0;

    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, shift_of_disp_gp_ctrl_reg_ckg_odclk_cvbs);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(DISP_GP_CTRL_REG_CKG_ODCLK_CVBS, u16Val,
                                             mask_of_disp_gp_ctrl_reg_ckg_odclk_cvbs);
}

u32 HalDispClkMapBasicClkgenToIdx(HalDispClkType_e enClkType, u32 u32ClkRate)
{
    u32 u32ClkIdx;

    switch (enClkType)
    {
        case E_HAL_DISP_CLK_MOP0:
        case E_HAL_DISP_CLK_MOP1:
        case E_HAL_DISP_CLK_MOP2:
            u32ClkIdx = u32ClkRate <= CLK_MHZ(192)   ? 0x07
                        : u32ClkRate <= CLK_MHZ(216) ? 0x06
                        : u32ClkRate <= CLK_MHZ(240) ? 0x05
                        : u32ClkRate <= CLK_MHZ(288) ? 0x04
                        : u32ClkRate <= CLK_MHZ(320) ? 0x03
                        : u32ClkRate <= CLK_MHZ(384) ? 0x00
                        : u32ClkRate <= CLK_MHZ(432) ? 0x01
                        : u32ClkRate <= CLK_MHZ(480) ? 0x02
                                                     : 0x08;
            break;
        case E_HAL_DISP_CLK_DISP_432:
            u32ClkIdx = 0x00;
            break;
        case E_HAL_DISP_CLK_DISP_216:
            u32ClkIdx = 0x00;
            break;
        default:
            DISP_ERR("%s %d, Type Not Support:%d\n", __FUNCTION__, __LINE__, enClkType);
            return 0;
    }
    return u32ClkIdx;
}
u32 HalDispClkMapBasicClkgenToRate(HalDispClkType_e enClkType, u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    switch (enClkType)
    {
        case E_HAL_DISP_CLK_MOP0:
        case E_HAL_DISP_CLK_MOP1:
        case E_HAL_DISP_CLK_MOP2:
            u32ClkRate = u32ClkRateIdx == 0x00   ? CLK_MHZ(384)
                         : u32ClkRateIdx == 0x01 ? CLK_MHZ(480)
                         : u32ClkRateIdx == 0x02 ? CLK_MHZ(432)
                         : u32ClkRateIdx == 0x03 ? CLK_MHZ(320)
                         : u32ClkRateIdx == 0x04 ? CLK_MHZ(288)
                         : u32ClkRateIdx == 0x05 ? CLK_MHZ(240)
                         : u32ClkRateIdx == 0x06 ? CLK_MHZ(216)
                         : u32ClkRateIdx == 0x07 ? CLK_MHZ(192)
                                             : CLK_MHZ(384);
            break;
        case E_HAL_DISP_CLK_DISP_432:
            u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(432) : 0;
            break;
        case E_HAL_DISP_CLK_DISP_216:
            u32ClkRate = u32ClkRateIdx == 0 ? CLK_MHZ(216) : 0;
            break;
        default:
            DISP_ERR("%s %d, Type Not Support:%d\n", __FUNCTION__, __LINE__, enClkType);
            return 0;
    }
    return u32ClkRate;
}
void HalDispClkGetBasicClkgen(HalDispClkType_e enClkType, u8 *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val = 0;
    HalDispClkSetReg_t stCfg;

    switch (enClkType)
    {
        case E_HAL_DISP_CLK_MOP0:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_47_L, 0x001F, 0);
            break;
        case E_HAL_DISP_CLK_MOP1:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_47_L, 0x1F00, 8);
            break;
        case E_HAL_DISP_CLK_MOP2:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN2_26_L, 0x001F, 0);
            break;
        case E_HAL_DISP_CLK_DISP_432:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_48_L, 0x0F00, 8);
            break;
        case E_HAL_DISP_CLK_DISP_216:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_48_L, 0xF000, 12);
            break;
        default:
            DISP_ERR("%s %d, Type Not Support:%d\n", __FUNCTION__, __LINE__, enClkType);
            return;
    }
    u16Val = HalDispUtilityR2BYTEDirect(stCfg.u32Reg) & stCfg.u16Mak;
    u16Val = u16Val >> stCfg.u16shift;
    _HalDispClkGetVal(u16Val, pbEn, pu32ClkRate);
}

void HalDispClkSetBasicClkgen(HalDispClkType_e enClkType, u8 bEn, u32 u32ClkRate)
{
    HalDispClkSetReg_t stCfg;
    u16 u16Val = 0;

    switch (enClkType)
    {
        case E_HAL_DISP_CLK_MOP0:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_47_L, 0x001F, 0);
            break;
        case E_HAL_DISP_CLK_MOP1:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_47_L, 0x1F00, 8);
            break;
        case E_HAL_DISP_CLK_MOP2:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN2_26_L, 0x001F, 0);
            break;
        case E_HAL_DISP_CLK_DISP_432:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_48_L, 0x0F00, 8);
            break;
        case E_HAL_DISP_CLK_DISP_216:
            _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_48_L, 0xF000, 12);
            break;
        default:
            DISP_ERR("%s %d, Type Not Support:%d\n", __FUNCTION__, __LINE__, enClkType);
            return;
    }
    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, stCfg.u16shift);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(stCfg.u32Reg, u16Val, stCfg.u16Mak);
}


void HalDispClkSetDispPixel(u32 u32Dev, u8 bEn, u32 u32ClkRate)
{
    HalDispClkSetReg_t stCfg;
    u16 u16Val = 0;

    if (u32Dev == HAL_DISP_DEVICE_ID_0)
    {
        _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_49_L, 0x003F, 0);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_1)
    {
        _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN_49_L, 0x3F00, 8);
    }
    else if (u32Dev == HAL_DISP_DEVICE_ID_2)
    {
        _HalDispClkFillRegStruct(&stCfg, REG_CLKGEN2_29_L, 0x003F, 0);
    }
    else
    {
        DISP_ERR("%s %d, Dev Not Support:%d\n", __FUNCTION__, __LINE__, u32Dev);
        return;
    }
    u16Val = _HalDispClkSetVal(bEn, u32ClkRate, stCfg.u16shift);
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(stCfg.u32Reg, u16Val, stCfg.u16Mak);
}

void HalDispClkSetBt1120(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0007) << 2;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN2_1A_L, u16Val, 0x001F);
}

void HalDispClkSetPixel2Bt1120(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0001;
    u16Val |= (u32ClkRate & 0x0003) << 2;//depend on device

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN2_1B_L, u16Val, 0x001F);
}

void HalDispClkSetMipiTxDsi(u8 bEn, u32 u32ClkRate)
{
    u16 u16Val;

    u16Val = bEn ? 0x0000 : 0x0100;
    u16Val |= (u32ClkRate & 0x0007) << 10;

    HalDispUtilityW2BYTEMSKDirectAndNoshadow(REG_CLKGEN_5E_L, u16Val, 0x1F00);
}

void HalDispClkInitGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg   = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    HalDispHwContain_t *       pstHalHwContain = NULL;

    pstDispCtxCfg   = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain   = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;

    if (pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] == 0)
    {
        memset(&g_stDispClkGpCtrlCfg[pstDevContain->u32DevId], 0, sizeof(HalDispClkGpCtrlConfig_t));
        pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId] =
            (void *)&g_stDispClkGpCtrlCfg[pstDevContain->u32DevId];
        pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] = 1;
    }
}

void HalDispClkDeInitGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg   = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    HalDispHwContain_t *       pstHalHwContain = NULL;

    pstDispCtxCfg   = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain   = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;

    if (pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId] == 1)
    {
        pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId] = NULL;
        pstHalHwContain->bClkGpCtrl[pstDevContain->u32DevId]                                = 0;
    }
}

void HalDispClkSetMergeOdClk(void *pCtx)
{
    DrvDispCtxConfig_t *      pstDispCtxCfg    = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg1 = NULL;
    u32                       u32Rate;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstClkGpCtrlCfg1 =
        (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[HAL_DISP_DEVICE_ID_1];

    u32Rate = pstClkGpCtrlCfg1->eType & E_HAL_DISP_CLK_GP_CTRL_HDMI      ? HAL_DISP_ODCLK_SEL_HDMI
              : pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_DAC     ? HAL_DISP_ODCLK_SEL_DAC_DIV2
              : pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_MIPIDSI ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK
              : pstClkGpCtrlCfg1->eType == E_HAL_DISP_CLK_GP_CTRL_CVBS    ? HAL_DISP_ODCLK_SEL_TVENC_27M
              : pstClkGpCtrlCfg1->eType & E_HAL_DISP_CLK_GP_CTRL_LCD     ? HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK
                                                                          : HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK;
    HalDispClkSetOdclk(HAL_DISP_DEVICE_ID_0, pstClkGpCtrlCfg1->bEn, u32Rate);
}
void HalDispClkSetGpCtrlCfg(void *pCtx)
{
    DrvDispCtxConfig_t *      pstDispCtxCfg = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg[HAL_DISP_DEVICE_MAX];
    HalDispClkGpCtrlType_e    eDevType[HAL_DISP_DEVICE_MAX];
    HalDispClkGpCtrlType_e    eDevTypeEn = 0;
    u8                        u8bEn[HAL_DISP_DEVICE_MAX];
    u8                        idx;
    HalDispClkGpCtrlConfig_t  stFameClkGpCtrlCfg = {0, 0};
    u32                       u32Rate;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
    {
        pstClkGpCtrlCfg[idx] =
            (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[idx];
        if (pstClkGpCtrlCfg[idx] == NULL)
        {
            pstClkGpCtrlCfg[idx] = &stFameClkGpCtrlCfg;
        }
        eDevType[idx] = pstClkGpCtrlCfg[idx]->eType;
        u8bEn[idx]    = pstClkGpCtrlCfg[idx]->bEn;
        if (pstClkGpCtrlCfg[idx]->bEn)
        {
            eDevTypeEn |= pstClkGpCtrlCfg[idx]->eType;
        }
        // Odclk_x
        u32Rate = GetClkRate(eDevType[idx], idx);
        HalDispClkSetOdclk(idx, u8bEn[idx], u32Rate);
    }

    DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, Dev0(%d, %s), Dev1(%d, %s), Dev2(%d, %s)\n", __FUNCTION__, __LINE__,
             u8bEn[HAL_DISP_DEVICE_ID_0], PARSING_HAL_CLK_GP_CTRL_TYPE(eDevType[HAL_DISP_DEVICE_ID_0]),
             u8bEn[HAL_DISP_DEVICE_ID_1], PARSING_HAL_CLK_GP_CTRL_TYPE(eDevType[HAL_DISP_DEVICE_ID_1]),
             u8bEn[HAL_DISP_DEVICE_ID_2], PARSING_HAL_CLK_GP_CTRL_TYPE(eDevType[HAL_DISP_DEVICE_ID_2]));

    _HalDispClkSetDispMipiDsi(eDevTypeEn, eDevType, u8bEn);
    _HalDispClkSetDispLcd(eDevTypeEn, eDevType, u8bEn);
    _HalDispClkSetDispHdmi(eDevTypeEn, eDevType, u8bEn);
    _HalDispClkSetDispCvbs(eDevTypeEn, eDevType, u8bEn);
    _HalDispClkSetDispDac(eDevTypeEn, eDevType, u8bEn);
    _HalDispClkSetDispPixelAll(eDevTypeEn, eDevType, u8bEn);
}

void HalDispClkInit(u8 bEn)
{
    u8  bClkEn;
    u32 u32ClkRate;
    u8  idx;

    for (idx = 0; idx < HAL_DISP_DEVICE_MAX; idx++)
    {
        HalDispClkGetOdclk(idx, &bClkEn, &u32ClkRate);
        HalDispClkSetOdclk(idx, bEn, u32ClkRate);
    }
}
/*
u32 HalDispClkMapDispPixelToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate == 10             ? 0x0A
                : u32ClkRate <= CLK_MHZ(108) ? 0x08
                : u32ClkRate <= CLK_MHZ(123) ? 0x06
                : u32ClkRate <= CLK_MHZ(144) ? 0x07
                : u32ClkRate <= CLK_MHZ(172) ? 0x05
                : u32ClkRate <= CLK_MHZ(192) ? 0x02
                : u32ClkRate <= CLK_MHZ(216) ? 0x04
                : u32ClkRate <= CLK_MHZ(240) ? 0x03
                : u32ClkRate <= CLK_MHZ(288) ? 0x00
                : u32ClkRate <= CLK_MHZ(320) ? 0x01
                                             : 0x09;
    return u32ClkIdx;
}

u32 HalDispClkMapDispPixelToRate(u32 u32ClkRateIdx)
{
    u32 u32ClkRate;

    u32ClkRate = u32ClkRateIdx == 0x0A   ? 10
                 : u32ClkRateIdx == 0x08 ? CLK_MHZ(108)
                 : u32ClkRateIdx == 0x06 ? CLK_MHZ(123)
                 : u32ClkRateIdx == 0x07 ? CLK_MHZ(144)
                 : u32ClkRateIdx == 0x05 ? CLK_MHZ(172)
                 : u32ClkRateIdx == 0x02 ? CLK_MHZ(192)
                 : u32ClkRateIdx == 0x04 ? CLK_MHZ(216)
                 : u32ClkRateIdx == 0x03 ? CLK_MHZ(240)
                 : u32ClkRateIdx == 0x00 ? CLK_MHZ(288)
                 : u32ClkRateIdx == 0x01 ? CLK_MHZ(320)
                                         : 0x09;
    return u32ClkRate;
}
void HalDispClkGetMipiTxDsi(u8 *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = HalDispUtilityR2BYTEDirect(REG_CLKGEN_5E_L) & 0x1F00;

    *pbEn        = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0007;
}
void HalDispClkGetDma(u8 *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;

    u16Val = GetRegVal(DISP_GP_CTRL_REG_CKG_ODCLK_CVBS, odclk_cvbs);
    _HalDispClkGetVal(u16Val, pbEn, pu32ClkRate);
}


*/
