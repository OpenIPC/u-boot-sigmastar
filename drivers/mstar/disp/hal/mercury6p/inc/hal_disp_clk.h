/*
* hal_disp_clk.h- Sigmastar
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

#ifndef _HAL_DISP_CLK_H_
#define _HAL_DISP_CLK_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
// reg_ckg_odclk0 / reg_ckg_odclk1
#define HAL_DISP_ODCLK_SEL_HDMI          0x00
#define HAL_DISP_ODCLK_SEL_DAC_DIV2      0x01
#define HAL_DISP_ODCLK_SEL_DISP0_PIX_CLK 0x02
#define HAL_DISP_ODCLK_SEL_DISP1_PIX_CLK 0x03
#define HAL_DISP_ODCLK_SEL_DISP2_PIX_CLK 0x05
#define HAL_DISP_ODCLK_SEL_TVENC_27M     0x04

// reg_ckg_mipi_pixel_g
#define HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK0 0x00
#define HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK1 0x01
#define HAL_DISP_CLK_MIPI_PIXEL_G_SEL_ODCLK2 0x02

// reg_ckg_mipi_pixel
#define HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK0 0x00
#define HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK1 0x01
#define HAL_DISP_CLK_MIPI_PIXEL_SEL_ODCLK2 0x02

// reg_clk_lcd_src
#define HAL_DISP_CLK_LCD_SRC_SEL_ODCLK0 0x00
#define HAL_DISP_CLK_LCD_SRC_SEL_ODCLK1 0x01
#define HAL_DISP_CLK_LCD_SRC_SEL_ODCLK2 0x02

// reg_clk_lcd
#define HAL_DISP_CLK_LCD_SEL_ODCLK0   0x00
#define HAL_DISP_CLK_LCD_SEL_ODCLK1   0x01
#define HAL_DISP_CLK_LCD_SEL_ODCLK2   0x05
#define HAL_DISP_CLK_LCD_SEL_DAC_DIV2 0x02

// reg_ckg_fpll_odclk / reg_ckg_fpll_odclk1
#define HAL_DISP_CLK_FPLL_SEL_ODCLK0 0x00
#define HAL_DISP_CLK_FPLL_SEL_ODCLK1 0x01
#define HAL_DISP_CLK_FPLL_SEL_ODCLK2 0x02

// reg_ckg_hdmi
#define HAL_DISP_CLK_HDMI_SEL_HDMI 0x00

// reg_ckg_tve
#define HAL_DISP_CLK_TVE_SEL_CVBS 0x00

// reg_ckg_dac
#define HAL_DISP_CLK_DAC_SEL_DAC 0x00

// reg_ckg_dac_waff
#define HAL_DISP_CLK_DAC_WAFF_SEL_HDMI     0x00
#define HAL_DISP_CLK_DAC_WAFF_SEL_DAC_DIV2 0x01

// reg_ckg_mop0 / reg_ckg_mop1
#define HAL_DISP_CLK_MOP_SEL_384M 0x00
#define HAL_DISP_CLK_MOP_SEL_432M 0x01
#define HAL_DISP_CLK_MOP_SEL_480M 0x02
#define HAL_DISP_CLK_MOP_SEL_320M 0x03
#define HAL_DISP_CLK_MOP_SEL_288M 0x04
#define HAL_DISP_CLK_MOP_SEL_240M 0x05
#define HAL_DISP_CLK_MOP_SEL_216M 0x06
#define HAL_DISP_CLK_MOP_SEL_192M 0x07

// reg_ckg_disp_432
#define HAL_DISP_CLK_DISP_432_SEL_432 0x00

// reg_ckg_disp_216
#define HAL_DISP_CLK_DISP_216_SEL_216 0x00

// reg_ckg_disp_pixel_0 / reg_ckg_disp_pixel_1
#define HAL_DISP_CLK_DISP_PIXEL_SEL_288M     0x00
#define HAL_DISP_CLK_DISP_PIXEL_SEL_320M     0x01
#define HAL_DISP_CLK_DISP_PIXEL_SEL_192M     0x02
#define HAL_DISP_CLK_DISP_PIXEL_SEL_240M     0x03
#define HAL_DISP_CLK_DISP_PIXEL_SEL_216M     0x04
#define HAL_DISP_CLK_DISP_PIXEL_SEL_172_8M   0x05
#define HAL_DISP_CLK_DISP_PIXEL_SEL_123_4M   0x06
#define HAL_DISP_CLK_DISP_PIXEL_SEL_144M     0x07
#define HAL_DISP_CLK_DISP_PIXEL_SEL_108M     0x08
#define HAL_DISP_CLK_DISP_PIXEL_SEL_LPLL     0x09
#define HAL_DISP_CLK_DISP_PIXEL_SEL_DISP_PLL 0x0A

// reg_ckg_mipi_tx_dsi
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_LPLL 0x00
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_160M 0x01
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_144M 0x02
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_108M 0x03
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_216M 0x04
#define HAL_DISP_CLK_MIPI_TX_DSI_SEL_240M 0x05

// reg_ckg_odcl_cvbs
#define HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK0 0x00
#define HAL_DISP_CLK_ODCL_CVBS_SEL_ODCLK1 0x01

// reg_clk_mipi_tx_dsi_apb
#define HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_0 0x00
#define HAL_DISP_CLK_MIPI_TX_DSI_APB_SEL_1 0x01

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_CLK_GP_CTRL_NONE    = 0x0000,
    E_HAL_DISP_CLK_GP_CTRL_HDMI    = 0x0001,
    E_HAL_DISP_CLK_GP_CTRL_LCD     = 0x0002,
    E_HAL_DISP_CLK_GP_CTRL_DAC     = 0x0004,
    E_HAL_DISP_CLK_GP_CTRL_MIPIDSI = 0x0008,
    E_HAL_DISP_CLK_GP_CTRL_CVBS    = 0x0010,

    E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC = 0x0005,
} HalDispClkGpCtrlType_e;

typedef struct
{
    u8                     bEn;
    HalDispClkGpCtrlType_e eType;
} HalDispClkGpCtrlConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_CLK_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE void HalDispClkSetOdclk(u32 u32Dev, u8 bEn, u32 u32ClkRate);
INTERFACE void HalDispClkSetHdmiPowerOnOff(u8 bEn);
INTERFACE void HalDispClkGetOdclk(u32 u32Dev, u8 *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetFpllOdclk(u8 bEn, u32 u32Dev, u32 u32FpllId);
INTERFACE void HalDispClkSetDma(u8 bEn, u32 u32Dev);
INTERFACE void HalDispClkGetDma(u8 *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetBasicClkgen(HalDispClkType_e enClkType, u8 bEn, u32 u32ClkRate);
INTERFACE u32  HalDispClkMapBasicClkgenToIdx(HalDispClkType_e enClkType, u32 u32ClkRate);
INTERFACE void HalDispClkGetBasicClkgen(HalDispClkType_e enClkType, u8 *pbEn, u32 *pu32ClkRate);
INTERFACE u32 HalDispClkMapBasicClkgenToRate(HalDispClkType_e enClkType, u32 u32ClkRateIdx);

INTERFACE u32  HalDispClkMapDispPixelToIdx(u32 u32ClkRate);
INTERFACE u32  HalDispClkMapDispPixelToRate(u32 u32ClkRateIdx);
INTERFACE void HalDispClkSetDispPixel0(u8 bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetDispPixel0(u8 *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetDispPixel1(u8 bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetDispPixel1(u8 *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetMipiTxDsi(u8 bEn, u32 u32ClkRate);
INTERFACE void HalDispClkGetMipiTxDsi(u8 *pbEn, u32 *pu32ClkRate);
INTERFACE void HalDispClkSetDispPixel(u32 u32Dev, u8 bEn, u32 u32ClkRate);

INTERFACE void HalDispClkInitGpCtrlCfg(void *pCtx);
INTERFACE void HalDispClkDeInitGpCtrlCfg(void *pCtx);
INTERFACE void HalDispClkSetGpCtrlCfg(void *pCtx);
INTERFACE void HalDispClkSetMergeOdClk(void *pCtx);
INTERFACE void HalDispClkInit(u8 bEn);
#undef INTERFACE
#endif
