/*
* reg_hdmitx_top.h- Sigmastar
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
#ifndef __REG_HDMITX_TOP__
#define __REG_HDMITX_TOP__

#define REG_DISP_GP_CTRL_BASE (0x113700)
#define REG_HDMI_LTP_BASE     (0x145800)
#define REG_HDMI_TX_BASE      (0x145900)
#define REG_HDMI_TX_ATOP_BASE (0x145A00)
#define REG_DISP_GP_CTRL_00_L (REG_DISP_GP_CTRL_BASE + 0x0)
#define REG_DISP_GP_CTRL_01_L (REG_DISP_GP_CTRL_BASE + 0x2)
#define REG_DISP_GP_CTRL_02_L (REG_DISP_GP_CTRL_BASE + 0x4)
#define REG_DISP_GP_CTRL_03_L (REG_DISP_GP_CTRL_BASE + 0x6)
#define REG_DISP_GP_CTRL_04_L (REG_DISP_GP_CTRL_BASE + 0x8)
#define REG_DISP_GP_CTRL_05_L (REG_DISP_GP_CTRL_BASE + 0xa)
#define REG_DISP_GP_CTRL_06_L (REG_DISP_GP_CTRL_BASE + 0xc)
#define REG_DISP_GP_CTRL_07_L (REG_DISP_GP_CTRL_BASE + 0xe)
#define REG_DISP_GP_CTRL_08_L (REG_DISP_GP_CTRL_BASE + 0x10)
#define REG_DISP_GP_CTRL_09_L (REG_DISP_GP_CTRL_BASE + 0x12)
#define REG_DISP_GP_CTRL_0A_L (REG_DISP_GP_CTRL_BASE + 0x14)
#define REG_DISP_GP_CTRL_0B_L (REG_DISP_GP_CTRL_BASE + 0x16)
#define REG_DISP_GP_CTRL_0C_L (REG_DISP_GP_CTRL_BASE + 0x18)
#define REG_DISP_GP_CTRL_0D_L (REG_DISP_GP_CTRL_BASE + 0x1a)
#define REG_DISP_GP_CTRL_0E_L (REG_DISP_GP_CTRL_BASE + 0x1c)
#define REG_DISP_GP_CTRL_0F_L (REG_DISP_GP_CTRL_BASE + 0x1e)
// bigger than 16 bit case allh0010
#define offset_of_disp_gp_ctrl_reg_sram_sd_en (0x20)
#define mask_of_disp_gp_ctrl_reg_sram_sd_en   (0xffffffff)
#define shift_of_disp_gp_ctrl_reg_sram_sd_en  (0)
#define DISP_GP_CTRL_REG_SRAM_SD_EN           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_sram_sd_en)
// h0010, bit: 15
/* 1. Each bits means  each IP's SRAM group power ON/OFF signal.
2. 1'b0: Power-ON
[0] : bach sram power ctrl.: 0:power-on, 1:power-off
[1] : aesdma sram power ctrl.: 0:power-on, 1:power-off
[2] : cmdq sram power ctrl.: 0:power-on, 1:power-off
[3] : emac sram power ctrl.: 0:power-on, 1:power-off
[4] : sd sram power ctrl.: 0:power-on, 1:power-off
[5] : gop0 sram power ctrl.: 0:power-on, 1:power-off
[6] : imi sram power ctrl.: 0:power-on, 1:power-off
[7] : isp sram power ctrl.: 0:power-on, 1:power-off
[8] : jpe sram power ctrl.: 0:power-on, 1:power-off
[9] : dip0 sram power ctrl.: 0:power-on, 1:power-off
[10] : sdio sram power ctrl.: 0:power-on, 1:power-off
[11] : usb sram power ctrl.: 0:power-on, 1:power-off
[12] : hvsp0 sram power ctrl.: 0:power-on, 1:power-off
[13] : hvsp1 sram power ctrl.: 0:power-on, 1:power-off
[14] : hvsp2 sram power ctrl.: 0:power-on, 1:power-off
[15] : reserved
[16] : reserved
[17] : reserved
[18] : reserved
[19] : reserved
[20] : sc0_frm sram power ctrl.: 0:power-on, 1:power-off
[21] : sc1_frm sram power ctrl.: 0:power-on, 1:power-off
[22] : debug sram power ctrl.: 0:power-on, 1:power-off
[23] : sc0_frm_r sram power ctrl.: 0:power-on, 1:power-off
[24] : reserved
[25] : sc2_frm sram power ctrl.: 0:power-on, 1:power-off
[26] : rsv1 sram power ctrl.: 0:power-on, 1:power-off
[27] : rsv2 sram power ctrl.: 0:power-on, 1:power-off
[28] : isp_dnr sram power ctrl.: 0:power-on, 1:power-off
[29] : isp_rot sram power ctrl.: 0:power-on, 1:power-off
[30] : reserved
[31] : usb_p1 sram power ctrl.: 0:power-on, 1:power-off
[32] : ive sram power ctrl.: 0:power-on, 1:power-off
[33] : cmdq1 sram power ctrl.: 0:power-on, 1:power-off
[34] : cmdq2 sram power ctrl.: 0:power-on, 1:power-off
[35] : gop1 sram power ctrl.: 0:power-on, 1:power-off
[36] : gop2 sram power ctrl.: 0:power-on, 1:power-off
[37] : reserved
[38] : reserved
[39] : reservedf
[40] : reserved
[41] : reserved
[42] : osdb1 sram power ctrl.: 0:power-on, 1:power-off
[43]: osdb2 sram power ctrl.: 0:power-on, 1:power-off
[44]: osdb3 sram power ctrl.: 0:power-on, 1:power-off
[45]: ldc sram power ctrl.: 0:power-on, 1:power-off
[46]: csi_tx sram power ctrl.: 0:power-on, 1:power-off
[47]: reserved
[48]: isp_bay sram power ctrl.: 0:power-on, 1:power-off
[49]: isp_pxl sram power ctrl.: 0:power-on, 1:power-off
[50]: isp_hdr0 sram power ctrl.: 0:power-on, 1:power-off
[51]: isp_hdr1 sram power ctrl.: 0:power-on, 1:power-off
[53]: isp_rot1 sram power ctrl.: 0:power-on, 1:power-off
[54]: isp_mot0 sram power ctrl.: 0:power-on, 1:power-off
[55] isp_mot1 sram power ctrl.: 0:power-on, 1:power-off
[56]: isp_wdma sram power ctrl.: 0:power-on, 1:power-off
[57]: isp_rdma sram power ctrl.: 0:power-on, 1:power-off
[58]: isp_420dma sram power ctrl.: 0:power-on, 1:power-off
[59]: isp_rgbir sram power ctrl.: 0:power-on, 1:power-off
[60]: isp_3dnr sram power ctrl.: 0:power-on, 1:power-off
[61]: isp_afifo1 sram power ctrl.: 0:power-on, 1:power-off
[62]: isp_afifo2 sram power ctrl.: 0:power-on, 1:power-off*/
#define offset_of_disp_gp_ctrl_reg_sram_sd_en_0 (0x20)
#define mask_of_disp_gp_ctrl_reg_sram_sd_en_0   (0xffff)
#define shift_of_disp_gp_ctrl_reg_sram_sd_en_0  (0)
#define DISP_GP_CTRL_REG_SRAM_SD_EN_0           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_sram_sd_en_0)
#define REG_DISP_GP_CTRL_10_L                   (DISP_GP_CTRL_REG_SRAM_SD_EN_0)
// h0011, bit: 15
/* 1. Each bits means  each IP's SRAM group power ON/OFF signal.
2. 1'b0: Power-ON
[0] : bach sram power ctrl.: 0:power-on, 1:power-off
[1] : aesdma sram power ctrl.: 0:power-on, 1:power-off
[2] : cmdq sram power ctrl.: 0:power-on, 1:power-off
[3] : emac sram power ctrl.: 0:power-on, 1:power-off
[4] : sd sram power ctrl.: 0:power-on, 1:power-off
[5] : gop0 sram power ctrl.: 0:power-on, 1:power-off
[6] : imi sram power ctrl.: 0:power-on, 1:power-off
[7] : isp sram power ctrl.: 0:power-on, 1:power-off
[8] : jpe sram power ctrl.: 0:power-on, 1:power-off
[9] : dip0 sram power ctrl.: 0:power-on, 1:power-off
[10] : sdio sram power ctrl.: 0:power-on, 1:power-off
[11] : usb sram power ctrl.: 0:power-on, 1:power-off
[12] : hvsp0 sram power ctrl.: 0:power-on, 1:power-off
[13] : hvsp1 sram power ctrl.: 0:power-on, 1:power-off
[14] : hvsp2 sram power ctrl.: 0:power-on, 1:power-off
[15] : reserved
[16] : reserved
[17] : reserved
[18] : reserved
[19] : reserved
[20] : sc0_frm sram power ctrl.: 0:power-on, 1:power-off
[21] : sc1_frm sram power ctrl.: 0:power-on, 1:power-off
[22] : debug sram power ctrl.: 0:power-on, 1:power-off
[23] : sc0_frm_r sram power ctrl.: 0:power-on, 1:power-off
[24] : reserved
[25] : sc2_frm sram power ctrl.: 0:power-on, 1:power-off
[26] : rsv1 sram power ctrl.: 0:power-on, 1:power-off
[27] : rsv2 sram power ctrl.: 0:power-on, 1:power-off
[28] : isp_dnr sram power ctrl.: 0:power-on, 1:power-off
[29] : isp_rot sram power ctrl.: 0:power-on, 1:power-off
[30] : reserved
[31] : usb_p1 sram power ctrl.: 0:power-on, 1:power-off
[32] : ive sram power ctrl.: 0:power-on, 1:power-off
[33] : cmdq1 sram power ctrl.: 0:power-on, 1:power-off
[34] : cmdq2 sram power ctrl.: 0:power-on, 1:power-off
[35] : gop1 sram power ctrl.: 0:power-on, 1:power-off
[36] : gop2 sram power ctrl.: 0:power-on, 1:power-off
[37] : reserved
[38] : reserved
[39] : reservedf
[40] : reserved
[41] : reserved
[42] : osdb1 sram power ctrl.: 0:power-on, 1:power-off
[43]: osdb2 sram power ctrl.: 0:power-on, 1:power-off
[44]: osdb3 sram power ctrl.: 0:power-on, 1:power-off
[45]: ldc sram power ctrl.: 0:power-on, 1:power-off
[46]: csi_tx sram power ctrl.: 0:power-on, 1:power-off
[47]: reserved
[48]: isp_bay sram power ctrl.: 0:power-on, 1:power-off
[49]: isp_pxl sram power ctrl.: 0:power-on, 1:power-off
[50]: isp_hdr0 sram power ctrl.: 0:power-on, 1:power-off
[51]: isp_hdr1 sram power ctrl.: 0:power-on, 1:power-off
[53]: isp_rot1 sram power ctrl.: 0:power-on, 1:power-off
[54]: isp_mot0 sram power ctrl.: 0:power-on, 1:power-off
[55] isp_mot1 sram power ctrl.: 0:power-on, 1:power-off
[56]: isp_wdma sram power ctrl.: 0:power-on, 1:power-off
[57]: isp_rdma sram power ctrl.: 0:power-on, 1:power-off
[58]: isp_420dma sram power ctrl.: 0:power-on, 1:power-off
[59]: isp_rgbir sram power ctrl.: 0:power-on, 1:power-off
[60]: isp_3dnr sram power ctrl.: 0:power-on, 1:power-off
[61]: isp_afifo1 sram power ctrl.: 0:power-on, 1:power-off
[62]: isp_afifo2 sram power ctrl.: 0:power-on, 1:power-off*/
#define offset_of_disp_gp_ctrl_reg_sram_sd_en_1 (0x22)
#define mask_of_disp_gp_ctrl_reg_sram_sd_en_1   (0xffff)
#define shift_of_disp_gp_ctrl_reg_sram_sd_en_1  (0)
#define DISP_GP_CTRL_REG_SRAM_SD_EN_1           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_sram_sd_en_1)
#define REG_DISP_GP_CTRL_11_L                   (DISP_GP_CTRL_REG_SRAM_SD_EN_1)
#define REG_DISP_GP_CTRL_12_L                   (REG_DISP_GP_CTRL_BASE + 0x24)
#define REG_DISP_GP_CTRL_13_L                   (REG_DISP_GP_CTRL_BASE + 0x26)
#define REG_DISP_GP_CTRL_14_L                   (REG_DISP_GP_CTRL_BASE + 0x28)
#define REG_DISP_GP_CTRL_15_L                   (REG_DISP_GP_CTRL_BASE + 0x2a)
#define REG_DISP_GP_CTRL_16_L                   (REG_DISP_GP_CTRL_BASE + 0x2c)
#define REG_DISP_GP_CTRL_17_L                   (REG_DISP_GP_CTRL_BASE + 0x2e)
#define REG_DISP_GP_CTRL_18_L                   (REG_DISP_GP_CTRL_BASE + 0x30)
#define REG_DISP_GP_CTRL_19_L                   (REG_DISP_GP_CTRL_BASE + 0x32)
#define REG_DISP_GP_CTRL_1A_L                   (REG_DISP_GP_CTRL_BASE + 0x34)
#define REG_DISP_GP_CTRL_1B_L                   (REG_DISP_GP_CTRL_BASE + 0x36)
#define REG_DISP_GP_CTRL_1C_L                   (REG_DISP_GP_CTRL_BASE + 0x38)
#define REG_DISP_GP_CTRL_1D_L                   (REG_DISP_GP_CTRL_BASE + 0x3a)
#define REG_DISP_GP_CTRL_1E_L                   (REG_DISP_GP_CTRL_BASE + 0x3c)
#define REG_DISP_GP_CTRL_1F_L                   (REG_DISP_GP_CTRL_BASE + 0x3e)
// h0020, bit: 2
/* clk_imi clock setting  (See CLKGEN reg_ckg_boot)
[0]: gate (0: pass, 1: gated)
[1]: reserved
[2]: select BOOT clock source (See reg_ckg_boot) (glitch free)
0: select BOOT clock 12MHz (xtali)
1: select MIU clock*/
#define offset_of_disp_gp_ctrl_reg_ckg_imi (0x40)
#define mask_of_disp_gp_ctrl_reg_ckg_imi   (0x7)
#define shift_of_disp_gp_ctrl_reg_ckg_imi  (0)
#define DISP_GP_CTRL_REG_CKG_IMI           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_imi)
#define REG_DISP_GP_CTRL_20_L              (DISP_GP_CTRL_REG_CKG_IMI)
// h0020, bit: 8
/* */
#define offset_of_disp_gp_ctrl_reg_clk_mcu_brg_free_run (0x40)
#define mask_of_disp_gp_ctrl_reg_clk_mcu_brg_free_run   (0x100)
#define shift_of_disp_gp_ctrl_reg_clk_mcu_brg_free_run  (8)
#define DISP_GP_CTRL_REG_CLK_MCU_BRG_FREE_RUN           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_clk_mcu_brg_free_run)
// h0021, bit: 3
/* clk_gop0_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop0_psram (0x42)
#define mask_of_disp_gp_ctrl_reg_ckg_gop0_psram   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_gop0_psram  (0)
#define DISP_GP_CTRL_REG_CKG_GOP0_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop0_psram)
#define REG_DISP_GP_CTRL_21_L                     (DISP_GP_CTRL_REG_CKG_GOP0_PSRAM)
// h0021, bit: 7
/* clk_gop1_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop1_psram (0x42)
#define mask_of_disp_gp_ctrl_reg_ckg_gop1_psram   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_gop1_psram  (4)
#define DISP_GP_CTRL_REG_CKG_GOP1_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop1_psram)
// h0021, bit: 11
/* clk_gop2_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop2_psram (0x42)
#define mask_of_disp_gp_ctrl_reg_ckg_gop2_psram   (0xf00)
#define shift_of_disp_gp_ctrl_reg_ckg_gop2_psram  (8)
#define DISP_GP_CTRL_REG_CKG_GOP2_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop2_psram)
// h0022, bit: 4
/* clk_odclk0 setting
[4:2]: select, 000: hdmi clock, 001: dac_gate_div2 clock, 010: disp0_pixel_clk, 011: disp1_pixel_clk, 100: tvenc_27M,
101:disp2_pixel_clk [1]: invert clock [0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_odclk0 (0x44)
#define mask_of_disp_gp_ctrl_reg_ckg_odclk0   (0x1f)
#define shift_of_disp_gp_ctrl_reg_ckg_odclk0  (0)
#define DISP_GP_CTRL_REG_CKG_ODCLK0           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_odclk0)
#define REG_DISP_GP_CTRL_22_L                 (DISP_GP_CTRL_REG_CKG_ODCLK0)
// h0022, bit: 12
/* clk_odclk1 setting
[4:2]: select, 000: hdmi clock, 001: dac_gate_div2 clock, 010: disp0_pixel_clk, 011: disp1_pixel_clk, 100: tvenc_27M,
101:disp2_pixel_clk [1]: invert clock [0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_odclk1 (0x44)
#define mask_of_disp_gp_ctrl_reg_ckg_odclk1   (0x1f00)
#define shift_of_disp_gp_ctrl_reg_ckg_odclk1  (8)
#define DISP_GP_CTRL_REG_CKG_ODCLK1           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_odclk1)
// h0023, bit: 3
/* clk_mipi_pixel_g setting
[3:2]: select, 00: clk_odlck0 clock, 01: clk_odclk1 clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g (0x46)
#define mask_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g  (0)
#define DISP_GP_CTRL_REG_CKG_MIPI_PIXEL_G           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_mipi_pixel_g)
#define REG_DISP_GP_CTRL_23_L                       (DISP_GP_CTRL_REG_CKG_MIPI_PIXEL_G)
// h0023, bit: 7
/* clk_mipi_pixel setting
[3:2]: select, 00: disp0_pixel_clk, 01: disp1_pixel_clk
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_mipi_pixel (0x46)
#define mask_of_disp_gp_ctrl_reg_ckg_mipi_pixel   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_mipi_pixel  (4)
#define DISP_GP_CTRL_REG_CKG_MIPI_PIXEL           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_mipi_pixel)
// h0024, bit: 3
/* clk_lcd_src setting
[3:2]: select, 00: clk_odlck0 clock, 01: clk_odclk1 clock, 10:clk_odclk2 clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_lcd_src (0x48)
#define mask_of_disp_gp_ctrl_reg_ckg_lcd_src   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_lcd_src  (0)
#define DISP_GP_CTRL_REG_CKG_LCD_SRC           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_lcd_src)
#define REG_DISP_GP_CTRL_24_L                  (DISP_GP_CTRL_REG_CKG_LCD_SRC)
// h0024, bit: 12
/* clk_lcd setting
[4:2]: select,
000: disp0_pixel_clk,
001: disp1_pixel_clk,
010: dac_gate_div2 clock,
011: disp0_pixel_clk_div2
100: disp1_pixel_clk_div2
101: disp2_pixel_clk
110: disp2_pixel_clk_div2
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_lcd (0x48)
#define mask_of_disp_gp_ctrl_reg_ckg_lcd   (0x1f00)
#define shift_of_disp_gp_ctrl_reg_ckg_lcd  (8)
#define DISP_GP_CTRL_REG_CKG_LCD           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_lcd)
// h0025, bit: 3
/* clk_fpll_odclk setting
[3:2]: select,
00: clk_odlck0 clock,
01: clk_odclk1 clock,
10: clk_odclk2 clock,
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_fpll_odclk (0x4a)
#define mask_of_disp_gp_ctrl_reg_ckg_fpll_odclk   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_fpll_odclk  (0)
#define DISP_GP_CTRL_REG_CKG_FPLL_ODCLK           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_fpll_odclk)
#define REG_DISP_GP_CTRL_25_L                     (DISP_GP_CTRL_REG_CKG_FPLL_ODCLK)
// h0025, bit: 7
/* clk_fpll_odclk1 setting
[3:2]: select,
00: clk_odlck0 clock,
01: clk_odclk1 clock
10: clk_odclk2 clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_fpll_odclk1 (0x4a)
#define mask_of_disp_gp_ctrl_reg_ckg_fpll_odclk1   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_fpll_odclk1  (4)
#define DISP_GP_CTRL_REG_CKG_FPLL_ODCLK1           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_fpll_odclk1)
// h0025, bit: 11
/* clk_odclk_cvbs setting
[3:2]: select, 00: clk_odlck0 clock, 01: clk_odclk1 clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_odclk_cvbs (0x4a)
#define mask_of_disp_gp_ctrl_reg_ckg_odclk_cvbs   (0xf00)
#define shift_of_disp_gp_ctrl_reg_ckg_odclk_cvbs  (8)
#define DISP_GP_CTRL_REG_CKG_ODCLK_CVBS           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_odclk_cvbs)
// h0026, bit: 3
/* clk_gop0_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop3_psram (0x4c)
#define mask_of_disp_gp_ctrl_reg_ckg_gop3_psram   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_gop3_psram  (0)
#define DISP_GP_CTRL_REG_CKG_GOP3_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop3_psram)
#define REG_DISP_GP_CTRL_26_L                     (DISP_GP_CTRL_REG_CKG_GOP3_PSRAM)
// h0026, bit: 7
/* clk_gop1_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop4_psram (0x4c)
#define mask_of_disp_gp_ctrl_reg_ckg_gop4_psram   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_gop4_psram  (4)
#define DISP_GP_CTRL_REG_CKG_GOP4_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop4_psram)
// h0026, bit: 11
/* clk_gop2_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop5_psram (0x4c)
#define mask_of_disp_gp_ctrl_reg_ckg_gop5_psram   (0xf00)
#define shift_of_disp_gp_ctrl_reg_ckg_gop5_psram  (8)
#define DISP_GP_CTRL_REG_CKG_GOP5_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop5_psram)
// h0027, bit: 3
/* clk_gop0_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop6_psram (0x4e)
#define mask_of_disp_gp_ctrl_reg_ckg_gop6_psram   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_gop6_psram  (0)
#define DISP_GP_CTRL_REG_CKG_GOP6_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop6_psram)
#define REG_DISP_GP_CTRL_27_L                     (DISP_GP_CTRL_REG_CKG_GOP6_PSRAM)
// h0027, bit: 7
/* clk_gop1_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop7_psram (0x4e)
#define mask_of_disp_gp_ctrl_reg_ckg_gop7_psram   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_gop7_psram  (4)
#define DISP_GP_CTRL_REG_CKG_GOP7_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop7_psram)
// h0027, bit: 11
/* clk_gop2_psram setting
[3:2]: select, 00: gop clock, 01: miu clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_gop8_psram (0x4e)
#define mask_of_disp_gp_ctrl_reg_ckg_gop8_psram   (0xf00)
#define shift_of_disp_gp_ctrl_reg_ckg_gop8_psram  (8)
#define DISP_GP_CTRL_REG_CKG_GOP8_PSRAM           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_gop8_psram)
// h0028, bit: 1
/* */
#define offset_of_disp_gp_ctrl_reg_ckg_bach (0x50)
#define mask_of_disp_gp_ctrl_reg_ckg_bach   (0x3)
#define shift_of_disp_gp_ctrl_reg_ckg_bach  (0)
#define DISP_GP_CTRL_REG_CKG_BACH           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_bach)
#define REG_DISP_GP_CTRL_28_L               (DISP_GP_CTRL_REG_CKG_BACH)
// h0029, bit: 4
/* clk_odclk1 setting
[4:2]: select,
000: hdmi clock,
001: dac_gate_div2 clock,
010: disp0_pixel_clk,
011: disp1_pixel_clk,
100: tvenc_27M,
101:disp2_pixel_clk
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_odclk2 (0x52)
#define mask_of_disp_gp_ctrl_reg_ckg_odclk2   (0x1f)
#define shift_of_disp_gp_ctrl_reg_ckg_odclk2  (0)
#define DISP_GP_CTRL_REG_CKG_ODCLK2           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_odclk2)
#define REG_DISP_GP_CTRL_29_L                 (DISP_GP_CTRL_REG_CKG_ODCLK2)
#define REG_DISP_GP_CTRL_2A_L                 (REG_DISP_GP_CTRL_BASE + 0x54)
#define REG_DISP_GP_CTRL_2B_L                 (REG_DISP_GP_CTRL_BASE + 0x56)
#define REG_DISP_GP_CTRL_2C_L                 (REG_DISP_GP_CTRL_BASE + 0x58)
#define REG_DISP_GP_CTRL_2D_L                 (REG_DISP_GP_CTRL_BASE + 0x5a)
#define REG_DISP_GP_CTRL_2E_L                 (REG_DISP_GP_CTRL_BASE + 0x5c)
#define REG_DISP_GP_CTRL_2F_L                 (REG_DISP_GP_CTRL_BASE + 0x5e)
// h0030, bit: 15
/* spare register, default low*/
#define offset_of_disp_gp_ctrl_reg_disp_spare_lo (0x60)
#define mask_of_disp_gp_ctrl_reg_disp_spare_lo   (0xffff)
#define shift_of_disp_gp_ctrl_reg_disp_spare_lo  (0)
#define DISP_GP_CTRL_REG_DISP_SPARE_LO           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_disp_spare_lo)
#define REG_DISP_GP_CTRL_30_L                    (DISP_GP_CTRL_REG_DISP_SPARE_LO)
// h0031, bit: 15
/* spare register, default high*/
#define offset_of_disp_gp_ctrl_reg_disp_spare_hi (0x62)
#define mask_of_disp_gp_ctrl_reg_disp_spare_hi   (0xffff)
#define shift_of_disp_gp_ctrl_reg_disp_spare_hi  (0)
#define DISP_GP_CTRL_REG_DISP_SPARE_HI           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_disp_spare_hi)
#define REG_DISP_GP_CTRL_31_L                    (DISP_GP_CTRL_REG_DISP_SPARE_HI)
// h0032, bit: 7
/* select group IP testbus*/
#define offset_of_disp_gp_ctrl_reg_disp_test_in_sel (0x64)
#define mask_of_disp_gp_ctrl_reg_disp_test_in_sel   (0xff)
#define shift_of_disp_gp_ctrl_reg_disp_test_in_sel  (0)
#define DISP_GP_CTRL_REG_DISP_TEST_IN_SEL           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_disp_test_in_sel)
#define REG_DISP_GP_CTRL_32_L                       (DISP_GP_CTRL_REG_DISP_TEST_IN_SEL)
// h0033, bit: 15
/* imi_arb_disp ip0~ipf rq_type (0>1)*/
#define offset_of_disp_gp_ctrl_reg_rq_type_sel (0x66)
#define mask_of_disp_gp_ctrl_reg_rq_type_sel   (0xffff)
#define shift_of_disp_gp_ctrl_reg_rq_type_sel  (0)
#define DISP_GP_CTRL_REG_RQ_TYPE_SEL           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_rq_type_sel)
#define REG_DISP_GP_CTRL_33_L                  (DISP_GP_CTRL_REG_RQ_TYPE_SEL)
// h0034, bit: 15
/* imi_arb_disp ip0~ipf rq_mask */
#define offset_of_disp_gp_ctrl_reg_rq_mask (0x68)
#define mask_of_disp_gp_ctrl_reg_rq_mask   (0xffff)
#define shift_of_disp_gp_ctrl_reg_rq_mask  (0)
#define DISP_GP_CTRL_REG_RQ_MASK           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_rq_mask)
#define REG_DISP_GP_CTRL_34_L              (DISP_GP_CTRL_REG_RQ_MASK)
// h0035, bit: 3
/* clk_hdmi clock setting
[3:2]: 00: hdmi clock source
[1]: invert clock
[0]: disable clock; 1: gated
*/
#define offset_of_disp_gp_ctrl_reg_ckg_hdmi (0x6a)
#define mask_of_disp_gp_ctrl_reg_ckg_hdmi   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_hdmi  (0)
#define DISP_GP_CTRL_REG_CKG_HDMI           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_hdmi)
#define REG_DISP_GP_CTRL_35_L               (DISP_GP_CTRL_REG_CKG_HDMI)
// h0035, bit: 7
/* clk_cvbs clock setting
[3:2]: 00: cvbs clock source
[1]: invert clock
[0]: disable clock; 1: gated
*/
#define offset_of_disp_gp_ctrl_reg_ckg_tve (0x6a)
#define mask_of_disp_gp_ctrl_reg_ckg_tve   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_tve  (4)
#define DISP_GP_CTRL_REG_CKG_TVE           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_tve)
// h0035, bit: 11
/* clk_hdmi_div2 clock setting
[3:2]: 00: hdmi clock source
[1]: invert clock
[0]: disable clock; 1: gated
*/
#define offset_of_disp_gp_ctrl_reg_ckg_hdmi_div2 (0x6a)
#define mask_of_disp_gp_ctrl_reg_ckg_hdmi_div2   (0xf00)
#define shift_of_disp_gp_ctrl_reg_ckg_hdmi_div2  (8)
#define DISP_GP_CTRL_REG_CKG_HDMI_DIV2           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_hdmi_div2)
// h0036, bit: 3
/* clk dac clock setting
[3:2]: 00: dac clock source
[1]: invert clock
[0]: disable clock; 1: gated
*/
#define offset_of_disp_gp_ctrl_reg_ckg_dac (0x6c)
#define mask_of_disp_gp_ctrl_reg_ckg_dac   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_dac  (0)
#define DISP_GP_CTRL_REG_CKG_DAC           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_dac)
#define REG_DISP_GP_CTRL_36_L              (DISP_GP_CTRL_REG_CKG_DAC)
// h0036, bit: 7
/* clk_dac_waff setting
[3:2]: select, 00: hdmi clock, 01: dac_gate_div2 clock
[1]: invert clock
[0]: disable clock; 1: gated*/
#define offset_of_disp_gp_ctrl_reg_ckg_dac_waff (0x6c)
#define mask_of_disp_gp_ctrl_reg_ckg_dac_waff   (0xf0)
#define shift_of_disp_gp_ctrl_reg_ckg_dac_waff  (4)
#define DISP_GP_CTRL_REG_CKG_DAC_WAFF           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_dac_waff)
// h0037, bit: 3
/* */
#define offset_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb (0x6e)
#define mask_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb   (0xf)
#define shift_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb  (0)
#define DISP_GP_CTRL_REG_CKG_MIPI_TX_DSI_APB           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_ckg_mipi_tx_dsi_apb)
#define REG_DISP_GP_CTRL_37_L                          (DISP_GP_CTRL_REG_CKG_MIPI_TX_DSI_APB)
// h0038, bit: 7
/* imi_arb_disp limit_max (unit 4)*/
#define offset_of_disp_gp_ctrl_reg_rq_limit_max (0x70)
#define mask_of_disp_gp_ctrl_reg_rq_limit_max   (0xff)
#define shift_of_disp_gp_ctrl_reg_rq_limit_max  (0)
#define DISP_GP_CTRL_REG_RQ_LIMIT_MAX           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_rq_limit_max)
#define REG_DISP_GP_CTRL_38_L                   (DISP_GP_CTRL_REG_RQ_LIMIT_MAX)
// h0038, bit: 8
/* imi_arb_disp limit_en*/
#define offset_of_disp_gp_ctrl_reg_limit_en (0x70)
#define mask_of_disp_gp_ctrl_reg_limit_en   (0x100)
#define shift_of_disp_gp_ctrl_reg_limit_en  (8)
#define DISP_GP_CTRL_REG_LIMIT_EN           (REG_DISP_GP_CTRL_BASE + offset_of_disp_gp_ctrl_reg_limit_en)
#define REG_DISP_GP_CTRL_39_L               (REG_DISP_GP_CTRL_BASE + 0x72)
#define REG_DISP_GP_CTRL_3A_L               (REG_DISP_GP_CTRL_BASE + 0x74)
#define REG_DISP_GP_CTRL_3B_L               (REG_DISP_GP_CTRL_BASE + 0x76)
#define REG_DISP_GP_CTRL_3C_L               (REG_DISP_GP_CTRL_BASE + 0x78)
#define REG_DISP_GP_CTRL_3D_L               (REG_DISP_GP_CTRL_BASE + 0x7a)
#define REG_DISP_GP_CTRL_3E_L               (REG_DISP_GP_CTRL_BASE + 0x7c)
#define REG_DISP_GP_CTRL_3F_L               (REG_DISP_GP_CTRL_BASE + 0x7e)
#define REG_DISP_GP_CTRL_40_L               (REG_DISP_GP_CTRL_BASE + 0x80)
#define REG_DISP_GP_CTRL_41_L               (REG_DISP_GP_CTRL_BASE + 0x82)
#define REG_DISP_GP_CTRL_42_L               (REG_DISP_GP_CTRL_BASE + 0x84)
#define REG_DISP_GP_CTRL_43_L               (REG_DISP_GP_CTRL_BASE + 0x86)
#define REG_DISP_GP_CTRL_44_L               (REG_DISP_GP_CTRL_BASE + 0x88)
#define REG_DISP_GP_CTRL_45_L               (REG_DISP_GP_CTRL_BASE + 0x8a)
#define REG_DISP_GP_CTRL_46_L               (REG_DISP_GP_CTRL_BASE + 0x8c)
#define REG_DISP_GP_CTRL_47_L               (REG_DISP_GP_CTRL_BASE + 0x8e)
#define REG_DISP_GP_CTRL_48_L               (REG_DISP_GP_CTRL_BASE + 0x90)
#define REG_DISP_GP_CTRL_49_L               (REG_DISP_GP_CTRL_BASE + 0x92)
#define REG_DISP_GP_CTRL_4A_L               (REG_DISP_GP_CTRL_BASE + 0x94)
#define REG_DISP_GP_CTRL_4B_L               (REG_DISP_GP_CTRL_BASE + 0x96)
#define REG_DISP_GP_CTRL_4C_L               (REG_DISP_GP_CTRL_BASE + 0x98)
#define REG_DISP_GP_CTRL_4D_L               (REG_DISP_GP_CTRL_BASE + 0x9a)
#define REG_DISP_GP_CTRL_4E_L               (REG_DISP_GP_CTRL_BASE + 0x9c)
#define REG_DISP_GP_CTRL_4F_L               (REG_DISP_GP_CTRL_BASE + 0x9e)
#define REG_DISP_GP_CTRL_50_L               (REG_DISP_GP_CTRL_BASE + 0xa0)
#define REG_DISP_GP_CTRL_51_L               (REG_DISP_GP_CTRL_BASE + 0xa2)
#define REG_DISP_GP_CTRL_52_L               (REG_DISP_GP_CTRL_BASE + 0xa4)
#define REG_DISP_GP_CTRL_53_L               (REG_DISP_GP_CTRL_BASE + 0xa6)
#define REG_DISP_GP_CTRL_54_L               (REG_DISP_GP_CTRL_BASE + 0xa8)
#define REG_DISP_GP_CTRL_55_L               (REG_DISP_GP_CTRL_BASE + 0xaa)
#define REG_DISP_GP_CTRL_56_L               (REG_DISP_GP_CTRL_BASE + 0xac)
#define REG_DISP_GP_CTRL_57_L               (REG_DISP_GP_CTRL_BASE + 0xae)
#define REG_DISP_GP_CTRL_58_L               (REG_DISP_GP_CTRL_BASE + 0xb0)
#define REG_DISP_GP_CTRL_59_L               (REG_DISP_GP_CTRL_BASE + 0xb2)
#define REG_DISP_GP_CTRL_5A_L               (REG_DISP_GP_CTRL_BASE + 0xb4)
#define REG_DISP_GP_CTRL_5B_L               (REG_DISP_GP_CTRL_BASE + 0xb6)
#define REG_DISP_GP_CTRL_5C_L               (REG_DISP_GP_CTRL_BASE + 0xb8)
#define REG_DISP_GP_CTRL_5D_L               (REG_DISP_GP_CTRL_BASE + 0xba)
#define REG_DISP_GP_CTRL_5E_L               (REG_DISP_GP_CTRL_BASE + 0xbc)
#define REG_DISP_GP_CTRL_5F_L               (REG_DISP_GP_CTRL_BASE + 0xbe)
#define REG_DISP_GP_CTRL_60_L               (REG_DISP_GP_CTRL_BASE + 0xc0)
#define REG_DISP_GP_CTRL_61_L               (REG_DISP_GP_CTRL_BASE + 0xc2)
#define REG_DISP_GP_CTRL_62_L               (REG_DISP_GP_CTRL_BASE + 0xc4)
#define REG_DISP_GP_CTRL_63_L               (REG_DISP_GP_CTRL_BASE + 0xc6)
#define REG_DISP_GP_CTRL_64_L               (REG_DISP_GP_CTRL_BASE + 0xc8)
#define REG_DISP_GP_CTRL_65_L               (REG_DISP_GP_CTRL_BASE + 0xca)
#define REG_DISP_GP_CTRL_66_L               (REG_DISP_GP_CTRL_BASE + 0xcc)
#define REG_DISP_GP_CTRL_67_L               (REG_DISP_GP_CTRL_BASE + 0xce)
#define REG_DISP_GP_CTRL_68_L               (REG_DISP_GP_CTRL_BASE + 0xd0)
#define REG_DISP_GP_CTRL_69_L               (REG_DISP_GP_CTRL_BASE + 0xd2)
#define REG_DISP_GP_CTRL_6A_L               (REG_DISP_GP_CTRL_BASE + 0xd4)
#define REG_DISP_GP_CTRL_6B_L               (REG_DISP_GP_CTRL_BASE + 0xd6)
#define REG_DISP_GP_CTRL_6C_L               (REG_DISP_GP_CTRL_BASE + 0xd8)
#define REG_DISP_GP_CTRL_6D_L               (REG_DISP_GP_CTRL_BASE + 0xda)
#define REG_DISP_GP_CTRL_6E_L               (REG_DISP_GP_CTRL_BASE + 0xdc)
#define REG_DISP_GP_CTRL_6F_L               (REG_DISP_GP_CTRL_BASE + 0xde)
#define REG_DISP_GP_CTRL_70_L               (REG_DISP_GP_CTRL_BASE + 0xe0)
#define REG_DISP_GP_CTRL_71_L               (REG_DISP_GP_CTRL_BASE + 0xe2)
#define REG_DISP_GP_CTRL_72_L               (REG_DISP_GP_CTRL_BASE + 0xe4)
#define REG_DISP_GP_CTRL_73_L               (REG_DISP_GP_CTRL_BASE + 0xe6)
#define REG_DISP_GP_CTRL_74_L               (REG_DISP_GP_CTRL_BASE + 0xe8)
#define REG_DISP_GP_CTRL_75_L               (REG_DISP_GP_CTRL_BASE + 0xea)
#define REG_DISP_GP_CTRL_76_L               (REG_DISP_GP_CTRL_BASE + 0xec)
#define REG_DISP_GP_CTRL_77_L               (REG_DISP_GP_CTRL_BASE + 0xee)
#define REG_DISP_GP_CTRL_78_L               (REG_DISP_GP_CTRL_BASE + 0xf0)
#define REG_DISP_GP_CTRL_79_L               (REG_DISP_GP_CTRL_BASE + 0xf2)
#define REG_DISP_GP_CTRL_7A_L               (REG_DISP_GP_CTRL_BASE + 0xf4)
#define REG_DISP_GP_CTRL_7B_L               (REG_DISP_GP_CTRL_BASE + 0xf6)
#define REG_DISP_GP_CTRL_7C_L               (REG_DISP_GP_CTRL_BASE + 0xf8)
#define REG_DISP_GP_CTRL_7D_L               (REG_DISP_GP_CTRL_BASE + 0xfa)
#define REG_DISP_GP_CTRL_7E_L               (REG_DISP_GP_CTRL_BASE + 0xfc)
#define REG_DISP_GP_CTRL_7F_L               (REG_DISP_GP_CTRL_BASE + 0xfe)
// h0000, bit: 0
/* trigger the hdmi 2.1 training pattern to run.*/
#define offset_of_hdmi_ltp_reg_ltp_mode_trig (0x0)
#define mask_of_hdmi_ltp_reg_ltp_mode_trig   (0x1)
#define shift_of_hdmi_ltp_reg_ltp_mode_trig  (0)
#define HDMI_LTP_REG_LTP_MODE_TRIG           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp_mode_trig)
#define REG_HDMI_LTP_00_L                    (HDMI_LTP_REG_LTP_MODE_TRIG)
// h0001, bit: 3
/* select the training pattern number. See on table 6-64 . Morover, 9=> open ltp5~8 at the same time.*/
#define offset_of_hdmi_ltp_reg_ltp_mode_sel (0x2)
#define mask_of_hdmi_ltp_reg_ltp_mode_sel   (0xf)
#define shift_of_hdmi_ltp_reg_ltp_mode_sel  (0)
#define HDMI_LTP_REG_LTP_MODE_SEL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp_mode_sel)
#define REG_HDMI_LTP_01_L                   (HDMI_LTP_REG_LTP_MODE_SEL)
// h0001, bit: 15
/* enable the training pattern function.
0: hdmi_tx 1.4 function
1: hdmi 2.1 training pattern*/
#define offset_of_hdmi_ltp_reg_hdmi_21_ltp_en (0x2)
#define mask_of_hdmi_ltp_reg_hdmi_21_ltp_en   (0x8000)
#define shift_of_hdmi_ltp_reg_hdmi_21_ltp_en  (15)
#define HDMI_LTP_REG_HDMI_21_LTP_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_21_ltp_en)
// h0002, bit: 1
/* [0]: disable clock
[1]: invert clock
note: reg_ckg_hdmi14 is in hdmi_tx_atop.xls*/
#define offset_of_hdmi_ltp_reg_ckg_hdmi_14_fifo_out (0x4)
#define mask_of_hdmi_ltp_reg_ckg_hdmi_14_fifo_out   (0x3)
#define shift_of_hdmi_ltp_reg_ckg_hdmi_14_fifo_out  (0)
#define HDMI_LTP_REG_CKG_HDMI_14_FIFO_OUT           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ckg_hdmi_14_fifo_out)
#define REG_HDMI_LTP_02_L                           (HDMI_LTP_REG_CKG_HDMI_14_FIFO_OUT)
// h0002, bit: 9
/* [0]: disable clock
[1]: invert clock*/
#define offset_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_in (0x4)
#define mask_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_in   (0x300)
#define shift_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_in  (8)
#define HDMI_LTP_REG_CKG_HDMI_21_FIFO_IN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_in)
// h0002, bit: 13
/* [0]: disable clock
[1]: invert clock*/
#define offset_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_out (0x4)
#define mask_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_out   (0x3000)
#define shift_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_out  (12)
#define HDMI_LTP_REG_CKG_HDMI_21_FIFO_OUT           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ckg_hdmi_21_fifo_out)
// h0003, bit: 1
/* select the hdmi fifo out packet sequence
default(0): {fifo_out[9:0] , fifo_out[19:10] , 16'b0}
1: { 16'b0 , fifo_out[19:10] , fifo_out[9:0] ,}
2: { 16'b0 , fifo_out[9:0] , fifo_out[19:10] }
3: {fifo_out[9:0] , fifo_out[19:10] , 16'b0}
<ps> reg_hdmi_tx control the sequence of  hdmi_ch0[9:0],hdmi_ch1[9:0],hdmi_ch2[9:0],hdmi_ch3[9:0]*/
#define offset_of_hdmi_ltp_reg_hdmi_14_out_sel (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_14_out_sel   (0x3)
#define shift_of_hdmi_ltp_reg_hdmi_14_out_sel  (0)
#define HDMI_LTP_REG_HDMI_14_OUT_SEL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_14_out_sel)
#define REG_HDMI_LTP_03_L                      (HDMI_LTP_REG_HDMI_14_OUT_SEL)
// h0003, bit: 3
/* clear the data and ptr of hdmi1.4 fifo*/
#define offset_of_hdmi_ltp_reg_hdmi_14_fifo_clear_en (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_14_fifo_clear_en   (0x8)
#define shift_of_hdmi_ltp_reg_hdmi_14_fifo_clear_en  (3)
#define HDMI_LTP_REG_HDMI_14_FIFO_CLEAR_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_14_fifo_clear_en)
// h0003, bit: 4
/* enable the  fifo skip data of hdmi1.4*/
#define offset_of_hdmi_ltp_reg_hdmi_14_skip_data (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_14_skip_data   (0x10)
#define shift_of_hdmi_ltp_reg_hdmi_14_skip_data  (4)
#define HDMI_LTP_REG_HDMI_14_SKIP_DATA           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_14_skip_data)
// h0003, bit: 5
/* enable the  fifo off data of hdmi1.4*/
#define offset_of_hdmi_ltp_reg_hdmi_14_off_data (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_14_off_data   (0x20)
#define shift_of_hdmi_ltp_reg_hdmi_14_off_data  (5)
#define HDMI_LTP_REG_HDMI_14_OFF_DATA           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_14_off_data)
// h0003, bit: 8
/* select the hdmi fifo out packet sequence
0: {fifo_out[35:0]}
1: {fifo_out[17:0] , fifo_out[35:18]}*/
#define offset_of_hdmi_ltp_reg_hdmi_21_out_sel (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_21_out_sel   (0x100)
#define shift_of_hdmi_ltp_reg_hdmi_21_out_sel  (8)
#define HDMI_LTP_REG_HDMI_21_OUT_SEL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_21_out_sel)
// h0003, bit: 11
/* clear the data and ptr of hdmi2.1 fifo*/
#define offset_of_hdmi_ltp_reg_hdmi_21_fifo_clear_en (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_21_fifo_clear_en   (0x800)
#define shift_of_hdmi_ltp_reg_hdmi_21_fifo_clear_en  (11)
#define HDMI_LTP_REG_HDMI_21_FIFO_CLEAR_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_21_fifo_clear_en)
// h0003, bit: 12
/* enable the  fifo skip data of hdmi2.1*/
#define offset_of_hdmi_ltp_reg_hdmi_21_skip_data (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_21_skip_data   (0x1000)
#define shift_of_hdmi_ltp_reg_hdmi_21_skip_data  (12)
#define HDMI_LTP_REG_HDMI_21_SKIP_DATA           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_21_skip_data)
// h0003, bit: 13
/* enable the  fifo off data of hdmi2.1*/
#define offset_of_hdmi_ltp_reg_hdmi_21_off_data (0x6)
#define mask_of_hdmi_ltp_reg_hdmi_21_off_data   (0x2000)
#define shift_of_hdmi_ltp_reg_hdmi_21_off_data  (13)
#define HDMI_LTP_REG_HDMI_21_OFF_DATA           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_21_off_data)
// h0004, bit: 1
/* select the trigger fifo read timing*/
#define offset_of_hdmi_ltp_reg_fifo_rd_off_sel (0x8)
#define mask_of_hdmi_ltp_reg_fifo_rd_off_sel   (0x3)
#define shift_of_hdmi_ltp_reg_fifo_rd_off_sel  (0)
#define HDMI_LTP_REG_FIFO_RD_OFF_SEL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_fifo_rd_off_sel)
#define REG_HDMI_LTP_04_L                      (HDMI_LTP_REG_FIFO_RD_OFF_SEL)
// h0004, bit: 2
/* make fifo input can delay 1t. To adjust location data location in the fifo[35:0]*/
#define offset_of_hdmi_ltp_reg_fifo_in_delay1t (0x8)
#define mask_of_hdmi_ltp_reg_fifo_in_delay1t   (0x4)
#define shift_of_hdmi_ltp_reg_fifo_in_delay1t  (2)
#define HDMI_LTP_REG_FIFO_IN_DELAY1T           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_fifo_in_delay1t)
#define REG_HDMI_LTP_05_L                      (REG_HDMI_LTP_BASE + 0xa)
#define REG_HDMI_LTP_06_L                      (REG_HDMI_LTP_BASE + 0xc)
#define REG_HDMI_LTP_07_L                      (REG_HDMI_LTP_BASE + 0xe)
#define REG_HDMI_LTP_08_L                      (REG_HDMI_LTP_BASE + 0x10)
#define REG_HDMI_LTP_09_L                      (REG_HDMI_LTP_BASE + 0x12)
#define REG_HDMI_LTP_0A_L                      (REG_HDMI_LTP_BASE + 0x14)
#define REG_HDMI_LTP_0B_L                      (REG_HDMI_LTP_BASE + 0x16)
// bigger than 16 bit case allh000c
#define offset_of_hdmi_ltp_reg_ltp123_data (0x18)
#define mask_of_hdmi_ltp_reg_ltp123_data   (0xffffffff)
#define shift_of_hdmi_ltp_reg_ltp123_data  (0)
#define HDMI_LTP_REG_LTP123_DATA           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp123_data)
// h000c, bit: 15
/* data of ltp1~ltp3*/
#define offset_of_hdmi_ltp_reg_ltp123_data_0 (0x18)
#define mask_of_hdmi_ltp_reg_ltp123_data_0   (0xffff)
#define shift_of_hdmi_ltp_reg_ltp123_data_0  (0)
#define HDMI_LTP_REG_LTP123_DATA_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp123_data_0)
#define REG_HDMI_LTP_0C_L                    (HDMI_LTP_REG_LTP123_DATA_0)
// h000d, bit: 15
/* data of ltp1~ltp3*/
#define offset_of_hdmi_ltp_reg_ltp123_data_1 (0x1a)
#define mask_of_hdmi_ltp_reg_ltp123_data_1   (0xffff)
#define shift_of_hdmi_ltp_reg_ltp123_data_1  (0)
#define HDMI_LTP_REG_LTP123_DATA_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp123_data_1)
#define REG_HDMI_LTP_0D_L                    (HDMI_LTP_REG_LTP123_DATA_1)
// h000e, bit: 3
/* data of ltp1~ltp3*/
#define offset_of_hdmi_ltp_reg_ltp123_data_2 (0x1c)
#define mask_of_hdmi_ltp_reg_ltp123_data_2   (0xf)
#define shift_of_hdmi_ltp_reg_ltp123_data_2  (0)
#define HDMI_LTP_REG_LTP123_DATA_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp123_data_2)
#define REG_HDMI_LTP_0E_L                    (HDMI_LTP_REG_LTP123_DATA_2)
#define REG_HDMI_LTP_0F_L                    (REG_HDMI_LTP_BASE + 0x1e)
// h0010, bit: 15
/* set the ltp4 data*/
#define offset_of_hdmi_ltp_reg_ltp4_data_in (0x20)
#define mask_of_hdmi_ltp_reg_ltp4_data_in   (0xffff)
#define shift_of_hdmi_ltp_reg_ltp4_data_in  (0)
#define HDMI_LTP_REG_LTP4_DATA_IN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_data_in)
#define REG_HDMI_LTP_10_L                   (HDMI_LTP_REG_LTP4_DATA_IN)
#define REG_HDMI_LTP_11_L                   (REG_HDMI_LTP_BASE + 0x22)
// h0012, bit: 0
/* reverse the sequential of ltp4_data_out*/
#define offset_of_hdmi_ltp_reg_ltp4_reverse_en (0x24)
#define mask_of_hdmi_ltp_reg_ltp4_reverse_en   (0x1)
#define shift_of_hdmi_ltp_reg_ltp4_reverse_en  (0)
#define HDMI_LTP_REG_LTP4_REVERSE_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_reverse_en)
#define REG_HDMI_LTP_12_L                      (HDMI_LTP_REG_LTP4_REVERSE_EN)
// h0012, bit: 1
/* clear data of ltp4*/
#define offset_of_hdmi_ltp_reg_ltp4_clear (0x24)
#define mask_of_hdmi_ltp_reg_ltp4_clear   (0x2)
#define shift_of_hdmi_ltp_reg_ltp4_clear  (1)
#define HDMI_LTP_REG_LTP4_CLEAR           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_clear)
// bigger than 16 bit case allh0013
#define offset_of_hdmi_ltp_reg_ltp4_data_out (0x26)
#define mask_of_hdmi_ltp_reg_ltp4_data_out   (0x3ffff)
#define shift_of_hdmi_ltp_reg_ltp4_data_out  (0)
#define HDMI_LTP_REG_LTP4_DATA_OUT           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_data_out)
// h0013, bit: 15
/* data output of ltp4*/
#define offset_of_hdmi_ltp_reg_ltp4_data_out_0 (0x26)
#define mask_of_hdmi_ltp_reg_ltp4_data_out_0   (0xffff)
#define shift_of_hdmi_ltp_reg_ltp4_data_out_0  (0)
#define HDMI_LTP_REG_LTP4_DATA_OUT_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_data_out_0)
#define REG_HDMI_LTP_13_L                      (HDMI_LTP_REG_LTP4_DATA_OUT_0)
// h0014, bit: 1
/* data output of ltp4*/
#define offset_of_hdmi_ltp_reg_ltp4_data_out_1 (0x28)
#define mask_of_hdmi_ltp_reg_ltp4_data_out_1   (0x3)
#define shift_of_hdmi_ltp_reg_ltp4_data_out_1  (0)
#define HDMI_LTP_REG_LTP4_DATA_OUT_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp4_data_out_1)
#define REG_HDMI_LTP_14_L                      (HDMI_LTP_REG_LTP4_DATA_OUT_1)
#define REG_HDMI_LTP_15_L                      (REG_HDMI_LTP_BASE + 0x2a)
#define REG_HDMI_LTP_16_L                      (REG_HDMI_LTP_BASE + 0x2c)
#define REG_HDMI_LTP_17_L                      (REG_HDMI_LTP_BASE + 0x2e)
#define REG_HDMI_LTP_18_L                      (REG_HDMI_LTP_BASE + 0x30)
#define REG_HDMI_LTP_19_L                      (REG_HDMI_LTP_BASE + 0x32)
#define REG_HDMI_LTP_1A_L                      (REG_HDMI_LTP_BASE + 0x34)
#define REG_HDMI_LTP_1B_L                      (REG_HDMI_LTP_BASE + 0x36)
#define REG_HDMI_LTP_1C_L                      (REG_HDMI_LTP_BASE + 0x38)
#define REG_HDMI_LTP_1D_L                      (REG_HDMI_LTP_BASE + 0x3a)
#define REG_HDMI_LTP_1E_L                      (REG_HDMI_LTP_BASE + 0x3c)
#define REG_HDMI_LTP_1F_L                      (REG_HDMI_LTP_BASE + 0x3e)
// h0020, bit: 0
/* clear data of ltp5~ltp8*/
#define offset_of_hdmi_ltp_reg_ltp58_clear (0x40)
#define mask_of_hdmi_ltp_reg_ltp58_clear   (0x1)
#define shift_of_hdmi_ltp_reg_ltp58_clear  (0)
#define HDMI_LTP_REG_LTP58_CLEAR           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_ltp58_clear)
#define REG_HDMI_LTP_20_L                  (HDMI_LTP_REG_LTP58_CLEAR)
// h0020, bit: 1
/* make lfsr not reset when 4095*/
#define offset_of_hdmi_ltp_reg_lfsr_continuous (0x40)
#define mask_of_hdmi_ltp_reg_lfsr_continuous   (0x2)
#define shift_of_hdmi_ltp_reg_lfsr_continuous  (1)
#define HDMI_LTP_REG_LFSR_CONTINUOUS           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_continuous)
// h0020, bit: 8
/* 0: data_out = lfsr[0:15] ^ data_in[15:0]
1: data_out =  sequence reverse of     lfsr[0:15] ^ data_in[15:0]*/
#define offset_of_hdmi_ltp_reg_lfsr_reverse_en (0x40)
#define mask_of_hdmi_ltp_reg_lfsr_reverse_en   (0x100)
#define shift_of_hdmi_ltp_reg_lfsr_reverse_en  (8)
#define HDMI_LTP_REG_LFSR_REVERSE_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_reverse_en)
// h0021, bit: 0
/* set disparity into 00 (-3)*/
#define offset_of_hdmi_ltp_reg_rfd_set_00 (0x42)
#define mask_of_hdmi_ltp_reg_rfd_set_00   (0x1)
#define shift_of_hdmi_ltp_reg_rfd_set_00  (0)
#define HDMI_LTP_REG_RFD_SET_00           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_rfd_set_00)
#define REG_HDMI_LTP_21_L                 (HDMI_LTP_REG_RFD_SET_00)
// h0021, bit: 1
/* set disparity into 01 (-1)*/
#define offset_of_hdmi_ltp_reg_rfd_set_01 (0x42)
#define mask_of_hdmi_ltp_reg_rfd_set_01   (0x2)
#define shift_of_hdmi_ltp_reg_rfd_set_01  (1)
#define HDMI_LTP_REG_RFD_SET_01           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_rfd_set_01)
// h0021, bit: 2
/* set disparity into 10 (1)*/
#define offset_of_hdmi_ltp_reg_rfd_set_10 (0x42)
#define mask_of_hdmi_ltp_reg_rfd_set_10   (0x4)
#define shift_of_hdmi_ltp_reg_rfd_set_10  (2)
#define HDMI_LTP_REG_RFD_SET_10           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_rfd_set_10)
// h0021, bit: 3
/* set disparity into 11 (3)*/
#define offset_of_hdmi_ltp_reg_rfd_set_11 (0x42)
#define mask_of_hdmi_ltp_reg_rfd_set_11   (0x8)
#define shift_of_hdmi_ltp_reg_rfd_set_11  (3)
#define HDMI_LTP_REG_RFD_SET_11           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_rfd_set_11)
// h0022, bit: 0
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_in_16b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_in_16b_en   (0x1)
#define shift_of_hdmi_ltp_reg_reverse_in_16b_en  (0)
#define HDMI_LTP_REG_REVERSE_IN_16B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_in_16b_en)
#define REG_HDMI_LTP_22_L                        (HDMI_LTP_REG_REVERSE_IN_16B_EN)
// h0022, bit: 1
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_in_9b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_in_9b_en   (0x2)
#define shift_of_hdmi_ltp_reg_reverse_in_9b_en  (1)
#define HDMI_LTP_REG_REVERSE_IN_9B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_in_9b_en)
// h0022, bit: 2
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_in_7b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_in_7b_en   (0x4)
#define shift_of_hdmi_ltp_reg_reverse_in_7b_en  (2)
#define HDMI_LTP_REG_REVERSE_IN_7B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_in_7b_en)
// h0022, bit: 4
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_out_18b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_out_18b_en   (0x10)
#define shift_of_hdmi_ltp_reg_reverse_out_18b_en  (4)
#define HDMI_LTP_REG_REVERSE_OUT_18B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_out_18b_en)
// h0022, bit: 5
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_out_10b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_out_10b_en   (0x20)
#define shift_of_hdmi_ltp_reg_reverse_out_10b_en  (5)
#define HDMI_LTP_REG_REVERSE_OUT_10B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_out_10b_en)
// h0022, bit: 6
/* reverse hdmi 16b18b encoder data*/
#define offset_of_hdmi_ltp_reg_reverse_out_8b_en (0x44)
#define mask_of_hdmi_ltp_reg_reverse_out_8b_en   (0x40)
#define shift_of_hdmi_ltp_reg_reverse_out_8b_en  (6)
#define HDMI_LTP_REG_REVERSE_OUT_8B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_reverse_out_8b_en)
#define REG_HDMI_LTP_23_L                        (REG_HDMI_LTP_BASE + 0x46)
// h0024, bit: 15
/* input data of lfsr  ch0*/
#define offset_of_hdmi_ltp_reg_lfsr_data_in_ch0 (0x48)
#define mask_of_hdmi_ltp_reg_lfsr_data_in_ch0   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_in_ch0  (0)
#define HDMI_LTP_REG_LFSR_DATA_IN_CH0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_in_ch0)
#define REG_HDMI_LTP_24_L                       (HDMI_LTP_REG_LFSR_DATA_IN_CH0)
// h0025, bit: 15
/* input seed of lfsr  ch0*/
#define offset_of_hdmi_ltp_reg_lfsr_seed_in_ch0 (0x4a)
#define mask_of_hdmi_ltp_reg_lfsr_seed_in_ch0   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_seed_in_ch0  (0)
#define HDMI_LTP_REG_LFSR_SEED_IN_CH0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_seed_in_ch0)
#define REG_HDMI_LTP_25_L                       (HDMI_LTP_REG_LFSR_SEED_IN_CH0)
// h0026, bit: 15
/* input data of lfsr  ch1*/
#define offset_of_hdmi_ltp_reg_lfsr_data_in_ch1 (0x4c)
#define mask_of_hdmi_ltp_reg_lfsr_data_in_ch1   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_in_ch1  (0)
#define HDMI_LTP_REG_LFSR_DATA_IN_CH1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_in_ch1)
#define REG_HDMI_LTP_26_L                       (HDMI_LTP_REG_LFSR_DATA_IN_CH1)
// h0027, bit: 15
/* input seed of lfsr  ch1*/
#define offset_of_hdmi_ltp_reg_lfsr_seed_in_ch1 (0x4e)
#define mask_of_hdmi_ltp_reg_lfsr_seed_in_ch1   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_seed_in_ch1  (0)
#define HDMI_LTP_REG_LFSR_SEED_IN_CH1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_seed_in_ch1)
#define REG_HDMI_LTP_27_L                       (HDMI_LTP_REG_LFSR_SEED_IN_CH1)
// h0028, bit: 15
/* input data of lfsr  ch2*/
#define offset_of_hdmi_ltp_reg_lfsr_data_in_ch2 (0x50)
#define mask_of_hdmi_ltp_reg_lfsr_data_in_ch2   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_in_ch2  (0)
#define HDMI_LTP_REG_LFSR_DATA_IN_CH2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_in_ch2)
#define REG_HDMI_LTP_28_L                       (HDMI_LTP_REG_LFSR_DATA_IN_CH2)
// h0029, bit: 15
/* input seed of lfsr  ch2*/
#define offset_of_hdmi_ltp_reg_lfsr_seed_in_ch2 (0x52)
#define mask_of_hdmi_ltp_reg_lfsr_seed_in_ch2   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_seed_in_ch2  (0)
#define HDMI_LTP_REG_LFSR_SEED_IN_CH2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_seed_in_ch2)
#define REG_HDMI_LTP_29_L                       (HDMI_LTP_REG_LFSR_SEED_IN_CH2)
// h002a, bit: 15
/* input data of lfsr  ch3*/
#define offset_of_hdmi_ltp_reg_lfsr_data_in_ch3 (0x54)
#define mask_of_hdmi_ltp_reg_lfsr_data_in_ch3   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_in_ch3  (0)
#define HDMI_LTP_REG_LFSR_DATA_IN_CH3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_in_ch3)
#define REG_HDMI_LTP_2A_L                       (HDMI_LTP_REG_LFSR_DATA_IN_CH3)
// h002b, bit: 15
/* input seed of lfsr  ch3*/
#define offset_of_hdmi_ltp_reg_lfsr_seed_in_ch3 (0x56)
#define mask_of_hdmi_ltp_reg_lfsr_seed_in_ch3   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_seed_in_ch3  (0)
#define HDMI_LTP_REG_LFSR_SEED_IN_CH3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_seed_in_ch3)
#define REG_HDMI_LTP_2B_L                       (HDMI_LTP_REG_LFSR_SEED_IN_CH3)
// h002c, bit: 15
/* input of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_lfsr_data_ch0 (0x58)
#define mask_of_hdmi_ltp_reg_lfsr_data_ch0   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_ch0  (0)
#define HDMI_LTP_REG_LFSR_DATA_CH0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_ch0)
#define REG_HDMI_LTP_2C_L                    (HDMI_LTP_REG_LFSR_DATA_CH0)
// h002d, bit: 15
/* input of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_lfsr_data_ch1 (0x5a)
#define mask_of_hdmi_ltp_reg_lfsr_data_ch1   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_ch1  (0)
#define HDMI_LTP_REG_LFSR_DATA_CH1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_ch1)
#define REG_HDMI_LTP_2D_L                    (HDMI_LTP_REG_LFSR_DATA_CH1)
// h002e, bit: 15
/* input of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_lfsr_data_ch2 (0x5c)
#define mask_of_hdmi_ltp_reg_lfsr_data_ch2   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_ch2  (0)
#define HDMI_LTP_REG_LFSR_DATA_CH2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_ch2)
#define REG_HDMI_LTP_2E_L                    (HDMI_LTP_REG_LFSR_DATA_CH2)
// h002f, bit: 15
/* input of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_lfsr_data_ch3 (0x5e)
#define mask_of_hdmi_ltp_reg_lfsr_data_ch3   (0xffff)
#define shift_of_hdmi_ltp_reg_lfsr_data_ch3  (0)
#define HDMI_LTP_REG_LFSR_DATA_CH3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_lfsr_data_ch3)
#define REG_HDMI_LTP_2F_L                    (HDMI_LTP_REG_LFSR_DATA_CH3)
// bigger than 16 bit case allh0030
#define offset_of_hdmi_ltp_reg_18b_data_out_ch0 (0x60)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch0   (0x3ffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch0  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch0)
// h0030, bit: 15
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch0_0 (0x60)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch0_0   (0xffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch0_0  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH0_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch0_0)
#define REG_HDMI_LTP_30_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH0_0)
// h0031, bit: 1
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch0_1 (0x62)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch0_1   (0x3)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch0_1  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH0_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch0_1)
#define REG_HDMI_LTP_31_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH0_1)
// bigger than 16 bit case allh0032
#define offset_of_hdmi_ltp_reg_18b_data_out_ch1 (0x64)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch1   (0x3ffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch1  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch1)
// h0032, bit: 15
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch1_0 (0x64)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch1_0   (0xffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch1_0  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH1_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch1_0)
#define REG_HDMI_LTP_32_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH1_0)
// h0033, bit: 1
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch1_1 (0x66)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch1_1   (0x3)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch1_1  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH1_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch1_1)
#define REG_HDMI_LTP_33_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH1_1)
// bigger than 16 bit case allh0034
#define offset_of_hdmi_ltp_reg_18b_data_out_ch2 (0x68)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch2   (0x3ffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch2  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch2)
// h0034, bit: 15
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch2_0 (0x68)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch2_0   (0xffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch2_0  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH2_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch2_0)
#define REG_HDMI_LTP_34_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH2_0)
// h0035, bit: 1
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch2_1 (0x6a)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch2_1   (0x3)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch2_1  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH2_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch2_1)
#define REG_HDMI_LTP_35_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH2_1)
// bigger than 16 bit case allh0036
#define offset_of_hdmi_ltp_reg_18b_data_out_ch3 (0x6c)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch3   (0x3ffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch3  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch3)
// h0036, bit: 15
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch3_0 (0x6c)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch3_0   (0xffff)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch3_0  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH3_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch3_0)
#define REG_HDMI_LTP_36_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH3_0)
// h0037, bit: 1
/* output of 16bto18b encoder*/
#define offset_of_hdmi_ltp_reg_18b_data_out_ch3_1 (0x6e)
#define mask_of_hdmi_ltp_reg_18b_data_out_ch3_1   (0x3)
#define shift_of_hdmi_ltp_reg_18b_data_out_ch3_1  (0)
#define HDMI_LTP_REG_18B_DATA_OUT_CH3_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_18b_data_out_ch3_1)
#define REG_HDMI_LTP_37_L                         (HDMI_LTP_REG_18B_DATA_OUT_CH3_1)
// bigger than 16 bit case allh0038
#define offset_of_hdmi_ltp_reg_hdmi_data_initial (0x70)
#define mask_of_hdmi_ltp_reg_hdmi_data_initial   (0x3ffff)
#define shift_of_hdmi_ltp_reg_hdmi_data_initial  (0)
#define HDMI_LTP_REG_HDMI_DATA_INITIAL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_data_initial)
// h0038, bit: 15
/* output initial value to fifo.*/
#define offset_of_hdmi_ltp_reg_hdmi_data_initial_0 (0x70)
#define mask_of_hdmi_ltp_reg_hdmi_data_initial_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_data_initial_0  (0)
#define HDMI_LTP_REG_HDMI_DATA_INITIAL_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_data_initial_0)
#define REG_HDMI_LTP_38_L                          (HDMI_LTP_REG_HDMI_DATA_INITIAL_0)
// h0039, bit: 1
/* output initial value to fifo.*/
#define offset_of_hdmi_ltp_reg_hdmi_data_initial_1 (0x72)
#define mask_of_hdmi_ltp_reg_hdmi_data_initial_1   (0x3)
#define shift_of_hdmi_ltp_reg_hdmi_data_initial_1  (0)
#define HDMI_LTP_REG_HDMI_DATA_INITIAL_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_data_initial_1)
#define REG_HDMI_LTP_39_L                          (HDMI_LTP_REG_HDMI_DATA_INITIAL_1)
// h0039, bit: 15
/* set for initial value valid to fifo*/
#define offset_of_hdmi_ltp_reg_hdmi_data_initial_en (0x72)
#define mask_of_hdmi_ltp_reg_hdmi_data_initial_en   (0x8000)
#define shift_of_hdmi_ltp_reg_hdmi_data_initial_en  (15)
#define HDMI_LTP_REG_HDMI_DATA_INITIAL_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_data_initial_en)
#define REG_HDMI_LTP_3A_L                           (REG_HDMI_LTP_BASE + 0x74)
#define REG_HDMI_LTP_3B_L                           (REG_HDMI_LTP_BASE + 0x76)
#define REG_HDMI_LTP_3C_L                           (REG_HDMI_LTP_BASE + 0x78)
#define REG_HDMI_LTP_3D_L                           (REG_HDMI_LTP_BASE + 0x7a)
// h003e, bit: 15
/* reg_hdmi_ltp_reserved_0*/
#define offset_of_hdmi_ltp_reg_hdmi_ltp_reserved_0 (0x7c)
#define mask_of_hdmi_ltp_reg_hdmi_ltp_reserved_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_ltp_reserved_0  (0)
#define HDMI_LTP_REG_HDMI_LTP_RESERVED_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_ltp_reserved_0)
#define REG_HDMI_LTP_3E_L                          (HDMI_LTP_REG_HDMI_LTP_RESERVED_0)
// h003f, bit: 15
/* reg_hdmi_ltp_reserved_1*/
#define offset_of_hdmi_ltp_reg_hdmi_ltp_reserved_1 (0x7e)
#define mask_of_hdmi_ltp_reg_hdmi_ltp_reserved_1   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_ltp_reserved_1  (0)
#define HDMI_LTP_REG_HDMI_LTP_RESERVED_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_ltp_reserved_1)
#define REG_HDMI_LTP_3F_L                          (HDMI_LTP_REG_HDMI_LTP_RESERVED_1)
#define REG_HDMI_LTP_40_L                          (REG_HDMI_LTP_BASE + 0x80)
#define REG_HDMI_LTP_41_L                          (REG_HDMI_LTP_BASE + 0x82)
#define REG_HDMI_LTP_42_L                          (REG_HDMI_LTP_BASE + 0x84)
#define REG_HDMI_LTP_43_L                          (REG_HDMI_LTP_BASE + 0x86)
#define REG_HDMI_LTP_44_L                          (REG_HDMI_LTP_BASE + 0x88)
#define REG_HDMI_LTP_45_L                          (REG_HDMI_LTP_BASE + 0x8a)
#define REG_HDMI_LTP_46_L                          (REG_HDMI_LTP_BASE + 0x8c)
#define REG_HDMI_LTP_47_L                          (REG_HDMI_LTP_BASE + 0x8e)
#define REG_HDMI_LTP_48_L                          (REG_HDMI_LTP_BASE + 0x90)
#define REG_HDMI_LTP_49_L                          (REG_HDMI_LTP_BASE + 0x92)
#define REG_HDMI_LTP_4A_L                          (REG_HDMI_LTP_BASE + 0x94)
#define REG_HDMI_LTP_4B_L                          (REG_HDMI_LTP_BASE + 0x96)
#define REG_HDMI_LTP_4C_L                          (REG_HDMI_LTP_BASE + 0x98)
#define REG_HDMI_LTP_4D_L                          (REG_HDMI_LTP_BASE + 0x9a)
#define REG_HDMI_LTP_4E_L                          (REG_HDMI_LTP_BASE + 0x9c)
#define REG_HDMI_LTP_4F_L                          (REG_HDMI_LTP_BASE + 0x9e)
#define REG_HDMI_LTP_50_L                          (REG_HDMI_LTP_BASE + 0xa0)
#define REG_HDMI_LTP_51_L                          (REG_HDMI_LTP_BASE + 0xa2)
#define REG_HDMI_LTP_52_L                          (REG_HDMI_LTP_BASE + 0xa4)
#define REG_HDMI_LTP_53_L                          (REG_HDMI_LTP_BASE + 0xa6)
#define REG_HDMI_LTP_54_L                          (REG_HDMI_LTP_BASE + 0xa8)
#define REG_HDMI_LTP_55_L                          (REG_HDMI_LTP_BASE + 0xaa)
#define REG_HDMI_LTP_56_L                          (REG_HDMI_LTP_BASE + 0xac)
#define REG_HDMI_LTP_57_L                          (REG_HDMI_LTP_BASE + 0xae)
#define REG_HDMI_LTP_58_L                          (REG_HDMI_LTP_BASE + 0xb0)
#define REG_HDMI_LTP_59_L                          (REG_HDMI_LTP_BASE + 0xb2)
#define REG_HDMI_LTP_5A_L                          (REG_HDMI_LTP_BASE + 0xb4)
#define REG_HDMI_LTP_5B_L                          (REG_HDMI_LTP_BASE + 0xb6)
#define REG_HDMI_LTP_5C_L                          (REG_HDMI_LTP_BASE + 0xb8)
#define REG_HDMI_LTP_5D_L                          (REG_HDMI_LTP_BASE + 0xba)
#define REG_HDMI_LTP_5E_L                          (REG_HDMI_LTP_BASE + 0xbc)
#define REG_HDMI_LTP_5F_L                          (REG_HDMI_LTP_BASE + 0xbe)
#define REG_HDMI_LTP_60_L                          (REG_HDMI_LTP_BASE + 0xc0)
#define REG_HDMI_LTP_61_L                          (REG_HDMI_LTP_BASE + 0xc2)
#define REG_HDMI_LTP_62_L                          (REG_HDMI_LTP_BASE + 0xc4)
// h0063, bit: 0
/* enable 36bits data from register*/
#define offset_of_hdmi_ltp_reg_din_36b_en (0xc6)
#define mask_of_hdmi_ltp_reg_din_36b_en   (0x1)
#define shift_of_hdmi_ltp_reg_din_36b_en  (0)
#define HDMI_LTP_REG_DIN_36B_EN           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_en)
#define REG_HDMI_LTP_63_L                 (HDMI_LTP_REG_DIN_36B_EN)
// bigger than 16 bit case allh0064
#define offset_of_hdmi_ltp_reg_din_36b (0xc8)
#define mask_of_hdmi_ltp_reg_din_36b   (0xffffffff)
#define shift_of_hdmi_ltp_reg_din_36b  (0)
#define HDMI_LTP_REG_DIN_36B           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b)
// h0064, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_0_0 (0xc8)
#define mask_of_hdmi_ltp_reg_din_36b_0_0   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_0_0  (0)
#define HDMI_LTP_REG_DIN_36B_0_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_0_0)
#define REG_HDMI_LTP_64_L                  (HDMI_LTP_REG_DIN_36B_0_0)
// h0065, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_0_1 (0xca)
#define mask_of_hdmi_ltp_reg_din_36b_0_1   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_0_1  (0)
#define HDMI_LTP_REG_DIN_36B_0_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_0_1)
#define REG_HDMI_LTP_65_L                  (HDMI_LTP_REG_DIN_36B_0_1)
// h0066, bit: 3
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_0_2 (0xcc)
#define mask_of_hdmi_ltp_reg_din_36b_0_2   (0xf)
#define shift_of_hdmi_ltp_reg_din_36b_0_2  (0)
#define HDMI_LTP_REG_DIN_36B_0_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_0_2)
#define REG_HDMI_LTP_66_L                  (HDMI_LTP_REG_DIN_36B_0_2)
// bigger than 16 bit case allh0067
#define offset_of_hdmi_ltp_reg_din_36b_1 (0xce)
#define mask_of_hdmi_ltp_reg_din_36b_1   (0xffffffff)
#define shift_of_hdmi_ltp_reg_din_36b_1  (0)
#define HDMI_LTP_REG_DIN_36B_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_1)
// h0067, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_1_0 (0xce)
#define mask_of_hdmi_ltp_reg_din_36b_1_0   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_1_0  (0)
#define HDMI_LTP_REG_DIN_36B_1_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_1_0)
#define REG_HDMI_LTP_67_L                  (HDMI_LTP_REG_DIN_36B_1_0)
// h0068, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_1_1 (0xd0)
#define mask_of_hdmi_ltp_reg_din_36b_1_1   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_1_1  (0)
#define HDMI_LTP_REG_DIN_36B_1_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_1_1)
#define REG_HDMI_LTP_68_L                  (HDMI_LTP_REG_DIN_36B_1_1)
// h0069, bit: 3
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_1_2 (0xd2)
#define mask_of_hdmi_ltp_reg_din_36b_1_2   (0xf)
#define shift_of_hdmi_ltp_reg_din_36b_1_2  (0)
#define HDMI_LTP_REG_DIN_36B_1_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_1_2)
#define REG_HDMI_LTP_69_L                  (HDMI_LTP_REG_DIN_36B_1_2)
// bigger than 16 bit case allh006a
#define offset_of_hdmi_ltp_reg_din_36b_2 (0xd4)
#define mask_of_hdmi_ltp_reg_din_36b_2   (0xffffffff)
#define shift_of_hdmi_ltp_reg_din_36b_2  (0)
#define HDMI_LTP_REG_DIN_36B_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_2)
// h006a, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_2_0 (0xd4)
#define mask_of_hdmi_ltp_reg_din_36b_2_0   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_2_0  (0)
#define HDMI_LTP_REG_DIN_36B_2_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_2_0)
#define REG_HDMI_LTP_6A_L                  (HDMI_LTP_REG_DIN_36B_2_0)
// h006b, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_2_1 (0xd6)
#define mask_of_hdmi_ltp_reg_din_36b_2_1   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_2_1  (0)
#define HDMI_LTP_REG_DIN_36B_2_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_2_1)
#define REG_HDMI_LTP_6B_L                  (HDMI_LTP_REG_DIN_36B_2_1)
// h006c, bit: 3
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_2_2 (0xd8)
#define mask_of_hdmi_ltp_reg_din_36b_2_2   (0xf)
#define shift_of_hdmi_ltp_reg_din_36b_2_2  (0)
#define HDMI_LTP_REG_DIN_36B_2_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_2_2)
#define REG_HDMI_LTP_6C_L                  (HDMI_LTP_REG_DIN_36B_2_2)
// bigger than 16 bit case allh006d
#define offset_of_hdmi_ltp_reg_din_36b_3 (0xda)
#define mask_of_hdmi_ltp_reg_din_36b_3   (0xffffffff)
#define shift_of_hdmi_ltp_reg_din_36b_3  (0)
#define HDMI_LTP_REG_DIN_36B_3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_3)
// h006d, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_3_0 (0xda)
#define mask_of_hdmi_ltp_reg_din_36b_3_0   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_3_0  (0)
#define HDMI_LTP_REG_DIN_36B_3_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_3_0)
#define REG_HDMI_LTP_6D_L                  (HDMI_LTP_REG_DIN_36B_3_0)
// h006e, bit: 15
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_3_1 (0xdc)
#define mask_of_hdmi_ltp_reg_din_36b_3_1   (0xffff)
#define shift_of_hdmi_ltp_reg_din_36b_3_1  (0)
#define HDMI_LTP_REG_DIN_36B_3_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_3_1)
#define REG_HDMI_LTP_6E_L                  (HDMI_LTP_REG_DIN_36B_3_1)
// h006f, bit: 3
/* set register for hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_din_36b_3_2 (0xde)
#define mask_of_hdmi_ltp_reg_din_36b_3_2   (0xf)
#define shift_of_hdmi_ltp_reg_din_36b_3_2  (0)
#define HDMI_LTP_REG_DIN_36B_3_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_din_36b_3_2)
#define REG_HDMI_LTP_6F_L                  (HDMI_LTP_REG_DIN_36B_3_2)
// h0070, bit: 15
/* set hdmi_ltp debug bus*/
#define offset_of_hdmi_ltp_reg_debug_bus_sel (0xe0)
#define mask_of_hdmi_ltp_reg_debug_bus_sel   (0xffff)
#define shift_of_hdmi_ltp_reg_debug_bus_sel  (0)
#define HDMI_LTP_REG_DEBUG_BUS_SEL           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_debug_bus_sel)
#define REG_HDMI_LTP_70_L                    (HDMI_LTP_REG_DEBUG_BUS_SEL)
// bigger than 16 bit case allh0071
#define offset_of_hdmi_ltp_reg_debug_bus (0xe2)
#define mask_of_hdmi_ltp_reg_debug_bus   (0xffffff)
#define shift_of_hdmi_ltp_reg_debug_bus  (0)
#define HDMI_LTP_REG_DEBUG_BUS           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_debug_bus)
// h0071, bit: 15
/* read hdmi_ltp debug bus*/
#define offset_of_hdmi_ltp_reg_debug_bus_0 (0xe2)
#define mask_of_hdmi_ltp_reg_debug_bus_0   (0xffff)
#define shift_of_hdmi_ltp_reg_debug_bus_0  (0)
#define HDMI_LTP_REG_DEBUG_BUS_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_debug_bus_0)
#define REG_HDMI_LTP_71_L                  (HDMI_LTP_REG_DEBUG_BUS_0)
// h0072, bit: 7
/* read hdmi_ltp debug bus*/
#define offset_of_hdmi_ltp_reg_debug_bus_1 (0xe4)
#define mask_of_hdmi_ltp_reg_debug_bus_1   (0xff)
#define shift_of_hdmi_ltp_reg_debug_bus_1  (0)
#define HDMI_LTP_REG_DEBUG_BUS_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_debug_bus_1)
#define REG_HDMI_LTP_72_L                  (HDMI_LTP_REG_DEBUG_BUS_1)
#define REG_HDMI_LTP_73_L                  (REG_HDMI_LTP_BASE + 0xe6)
// bigger than 16 bit case allh0074
#define offset_of_hdmi_ltp_reg_hdmi_out_36b (0xe8)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b   (0xffffffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b)
// h0074, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_0_0 (0xe8)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_0_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_0_0  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_0_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_0_0)
#define REG_HDMI_LTP_74_L                       (HDMI_LTP_REG_HDMI_OUT_36B_0_0)
// h0075, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_0_1 (0xea)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_0_1   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_0_1  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_0_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_0_1)
#define REG_HDMI_LTP_75_L                       (HDMI_LTP_REG_HDMI_OUT_36B_0_1)
// h0076, bit: 3
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_0_2 (0xec)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_0_2   (0xf)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_0_2  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_0_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_0_2)
#define REG_HDMI_LTP_76_L                       (HDMI_LTP_REG_HDMI_OUT_36B_0_2)
// bigger than 16 bit case allh0077
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_1 (0xee)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_1   (0xffffffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_1  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_1)
// h0077, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_1_0 (0xee)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_1_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_1_0  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_1_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_1_0)
#define REG_HDMI_LTP_77_L                       (HDMI_LTP_REG_HDMI_OUT_36B_1_0)
// h0078, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_1_1 (0xf0)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_1_1   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_1_1  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_1_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_1_1)
#define REG_HDMI_LTP_78_L                       (HDMI_LTP_REG_HDMI_OUT_36B_1_1)
// h0079, bit: 3
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_1_2 (0xf2)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_1_2   (0xf)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_1_2  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_1_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_1_2)
#define REG_HDMI_LTP_79_L                       (HDMI_LTP_REG_HDMI_OUT_36B_1_2)
// bigger than 16 bit case allh007a
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_2 (0xf4)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_2   (0xffffffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_2  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_2)
// h007a, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_2_0 (0xf4)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_2_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_2_0  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_2_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_2_0)
#define REG_HDMI_LTP_7A_L                       (HDMI_LTP_REG_HDMI_OUT_36B_2_0)
// h007b, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_2_1 (0xf6)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_2_1   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_2_1  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_2_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_2_1)
#define REG_HDMI_LTP_7B_L                       (HDMI_LTP_REG_HDMI_OUT_36B_2_1)
// h007c, bit: 3
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_2_2 (0xf8)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_2_2   (0xf)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_2_2  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_2_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_2_2)
#define REG_HDMI_LTP_7C_L                       (HDMI_LTP_REG_HDMI_OUT_36B_2_2)
// bigger than 16 bit case allh007d
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_3 (0xfa)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_3   (0xffffffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_3  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_3           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_3)
// h007d, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_3_0 (0xfa)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_3_0   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_3_0  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_3_0           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_3_0)
#define REG_HDMI_LTP_7D_L                       (HDMI_LTP_REG_HDMI_OUT_36B_3_0)
// h007e, bit: 15
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_3_1 (0xfc)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_3_1   (0xffff)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_3_1  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_3_1           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_3_1)
#define REG_HDMI_LTP_7E_L                       (HDMI_LTP_REG_HDMI_OUT_36B_3_1)
// h007f, bit: 3
/* read hdmi_36b output data to analog*/
#define offset_of_hdmi_ltp_reg_hdmi_out_36b_3_2 (0xfe)
#define mask_of_hdmi_ltp_reg_hdmi_out_36b_3_2   (0xf)
#define shift_of_hdmi_ltp_reg_hdmi_out_36b_3_2  (0)
#define HDMI_LTP_REG_HDMI_OUT_36B_3_2           (REG_HDMI_LTP_BASE + offset_of_hdmi_ltp_reg_hdmi_out_36b_3_2)
#define REG_HDMI_LTP_7F_L                       (HDMI_LTP_REG_HDMI_OUT_36B_3_2)
// h0000, bit: 0
/* power down, active low*/
#define offset_of_hdmi_tx_reg_pd_n (0x0)
#define mask_of_hdmi_tx_reg_pd_n   (0x1)
#define shift_of_hdmi_tx_reg_pd_n  (0)
#define HDMI_TX_REG_PD_N           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pd_n)
#define REG_HDMI_TX_00_L           (HDMI_TX_REG_PD_N)
// h0000, bit: 1
/* write 1 to reset.*/
#define offset_of_hdmi_tx_reg_hdmi_tx_sw_rst (0x0)
#define mask_of_hdmi_tx_reg_hdmi_tx_sw_rst   (0x2)
#define shift_of_hdmi_tx_reg_hdmi_tx_sw_rst  (1)
#define HDMI_TX_REG_HDMI_TX_SW_RST           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_sw_rst)
// h0000, bit: 10
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_00 (0x0)
#define mask_of_hdmi_tx_reg_reserved_00   (0x7fc)
#define shift_of_hdmi_tx_reg_reserved_00  (2)
#define HDMI_TX_REG_RESERVED_00           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_00)
// h0000, bit: 11
/* chk FSM during vertical blanking*/
#define offset_of_hdmi_tx_reg_vb_field_st_err (0x0)
#define mask_of_hdmi_tx_reg_vb_field_st_err   (0x800)
#define shift_of_hdmi_tx_reg_vb_field_st_err  (11)
#define HDMI_TX_REG_VB_FIELD_ST_ERR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vb_field_st_err)
// h0000, bit: 12
/* chk FSM during active line*/
#define offset_of_hdmi_tx_reg_act_line_st_err (0x0)
#define mask_of_hdmi_tx_reg_act_line_st_err   (0x1000)
#define shift_of_hdmi_tx_reg_act_line_st_err  (12)
#define HDMI_TX_REG_ACT_LINE_ST_ERR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_act_line_st_err)
// h0000, bit: 13
/* cal de compare with timing de*/
#define offset_of_hdmi_tx_reg_de_mismatch (0x0)
#define mask_of_hdmi_tx_reg_de_mismatch   (0x2000)
#define shift_of_hdmi_tx_reg_de_mismatch  (13)
#define HDMI_TX_REG_DE_MISMATCH           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_de_mismatch)
// h0000, bit: 14
/* ctrl pkt transfer miss*/
#define offset_of_hdmi_tx_reg_pkt_transfer_miss (0x0)
#define mask_of_hdmi_tx_reg_pkt_transfer_miss   (0x4000)
#define shift_of_hdmi_tx_reg_pkt_transfer_miss  (14)
#define HDMI_TX_REG_PKT_TRANSFER_MISS           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt_transfer_miss)
// h0000, bit: 15
/* audio fifo error*/
#define offset_of_hdmi_tx_reg_audio_tx_error (0x0)
#define mask_of_hdmi_tx_reg_audio_tx_error   (0x8000)
#define shift_of_hdmi_tx_reg_audio_tx_error  (15)
#define HDMI_TX_REG_AUDIO_TX_ERROR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_tx_error)
// h0001, bit: 0
/* 1: hdmi mode, 0: dvi mode*/
#define offset_of_hdmi_tx_reg_hdmi_dvi (0x2)
#define mask_of_hdmi_tx_reg_hdmi_dvi   (0x1)
#define shift_of_hdmi_tx_reg_hdmi_dvi  (0)
#define HDMI_TX_REG_HDMI_DVI           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_dvi)
#define REG_HDMI_TX_01_L               (HDMI_TX_REG_HDMI_DVI)
// h0001, bit: 1
/* 1: 8 channel, 0: 2 channel*/
#define offset_of_hdmi_tx_reg_audio_layout (0x2)
#define mask_of_hdmi_tx_reg_audio_layout   (0x2)
#define shift_of_hdmi_tx_reg_audio_layout  (1)
#define HDMI_TX_REG_AUDIO_LAYOUT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_layout)
// h0001, bit: 2
/* 1: transmit full-fill unit only, 0: transmit if sample received*/
#define offset_of_hdmi_tx_reg_audio_fulfill (0x2)
#define mask_of_hdmi_tx_reg_audio_fulfill   (0x4)
#define shift_of_hdmi_tx_reg_audio_fulfill  (2)
#define HDMI_TX_REG_AUDIO_FULFILL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_fulfill)
// h0001, bit: 3
/* 1: enable dvi ctrl encoding*/
#define offset_of_hdmi_tx_reg_dvi_char_mode (0x2)
#define mask_of_hdmi_tx_reg_dvi_char_mode   (0x8)
#define shift_of_hdmi_tx_reg_dvi_char_mode  (3)
#define HDMI_TX_REG_DVI_CHAR_MODE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_dvi_char_mode)
// h0001, bit: 6
/* d0: 24b, [31:8] valid, linear pcm, asp packet, header: auto gen
'd1: 28b, [31:4] valid, linear pcm+pcuv, asp packet, header: auto gen
     pcuv gen by previous module
'd7: 32b, [31:0] valid, test, l+pcm/compress+pcuv+ecc(4b), pkt header: tst_*
     audio_layout must set 1 even if 2ch defined in header
other: reserved*/
#define offset_of_hdmi_tx_reg_audio_format (0x2)
#define mask_of_hdmi_tx_reg_audio_format   (0x70)
#define shift_of_hdmi_tx_reg_audio_format  (4)
#define HDMI_TX_REG_AUDIO_FORMAT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_format)
// h0001, bit: 7
/* 0: new st trans condition, 1: old st trans condition*/
#define offset_of_hdmi_tx_reg_v_none_st_sel (0x2)
#define mask_of_hdmi_tx_reg_v_none_st_sel   (0x80)
#define shift_of_hdmi_tx_reg_v_none_st_sel  (7)
#define HDMI_TX_REG_V_NONE_ST_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_none_st_sel)
// h0001, bit: 11
/* 8 ch sample present, decide if all 8ch can be present
0000, 0001, 0011, 0111, 1111*/
#define offset_of_hdmi_tx_reg_pcm_8ch_alloc (0x2)
#define mask_of_hdmi_tx_reg_pcm_8ch_alloc   (0xf00)
#define shift_of_hdmi_tx_reg_pcm_8ch_alloc  (8)
#define HDMI_TX_REG_PCM_8CH_ALLOC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pcm_8ch_alloc)
// h0001, bit: 12
/* audio mute enable, set 1 to enable rem, zero, hold, invalid*/
#define offset_of_hdmi_tx_reg_audio_mute (0x2)
#define mask_of_hdmi_tx_reg_audio_mute   (0x1000)
#define shift_of_hdmi_tx_reg_audio_mute  (12)
#define HDMI_TX_REG_AUDIO_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_mute)
// h0001, bit: 13
/* mute all enable, 1: replace with mult value*/
#define offset_of_hdmi_tx_reg_video_mute (0x2)
#define mask_of_hdmi_tx_reg_video_mute   (0x2000)
#define shift_of_hdmi_tx_reg_video_mute  (13)
#define HDMI_TX_REG_VIDEO_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_video_mute)
// h0001, bit: 14
/* 1: set set_avmute flag = 1*/
#define offset_of_hdmi_tx_reg_av_mute (0x2)
#define mask_of_hdmi_tx_reg_av_mute   (0x4000)
#define shift_of_hdmi_tx_reg_av_mute  (14)
#define HDMI_TX_REG_AV_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_av_mute)
// h0001, bit: 15
/* 1: enable bit reverse in afe channel*/
#define offset_of_hdmi_tx_reg_afe_bit_reverse (0x2)
#define mask_of_hdmi_tx_reg_afe_bit_reverse   (0x8000)
#define shift_of_hdmi_tx_reg_afe_bit_reverse  (15)
#define HDMI_TX_REG_AFE_BIT_REVERSE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_afe_bit_reverse)
// h0002, bit: 1
/* 00: sel rgb_in[7:0], 01: sel rgb_in[15:8], other: sel rgb_in[23:16]*/
#define offset_of_hdmi_tx_reg_vif_ch0_sel (0x4)
#define mask_of_hdmi_tx_reg_vif_ch0_sel   (0x3)
#define shift_of_hdmi_tx_reg_vif_ch0_sel  (0)
#define HDMI_TX_REG_VIF_CH0_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vif_ch0_sel)
#define REG_HDMI_TX_02_L                  (HDMI_TX_REG_VIF_CH0_SEL)
// h0002, bit: 3
/* 00: sel rgb_in[7:0], 01: sel rgb_in[15:8], other: sel rgb_in[23:16]*/
#define offset_of_hdmi_tx_reg_vif_ch1_sel (0x4)
#define mask_of_hdmi_tx_reg_vif_ch1_sel   (0xc)
#define shift_of_hdmi_tx_reg_vif_ch1_sel  (2)
#define HDMI_TX_REG_VIF_CH1_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vif_ch1_sel)
// h0002, bit: 5
/* 00: sel rgb_in[7:0], 01: sel rgb_in[15:8], other: sel rgb_in[23:16]*/
#define offset_of_hdmi_tx_reg_vif_ch2_sel (0x4)
#define mask_of_hdmi_tx_reg_vif_ch2_sel   (0x30)
#define shift_of_hdmi_tx_reg_vif_ch2_sel  (4)
#define HDMI_TX_REG_VIF_CH2_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vif_ch2_sel)
// h0002, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_02 (0x4)
#define mask_of_hdmi_tx_reg_reserved_02   (0xc0)
#define shift_of_hdmi_tx_reg_reserved_02  (6)
#define HDMI_TX_REG_RESERVED_02           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_02)
// h0002, bit: 9
/* tmds channel sel, default 00. 00: ch0, 01: ch1, 10: ch2, 11: ch_ck.*/
#define offset_of_hdmi_tx_reg_afe_ch0_sel (0x4)
#define mask_of_hdmi_tx_reg_afe_ch0_sel   (0x300)
#define shift_of_hdmi_tx_reg_afe_ch0_sel  (8)
#define HDMI_TX_REG_AFE_CH0_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_afe_ch0_sel)
// h0002, bit: 11
/* tmds channel sel, default 01. 00: ch0, 01: ch1, 10: ch2, 11: ch_ck*/
#define offset_of_hdmi_tx_reg_afe_ch1_sel (0x4)
#define mask_of_hdmi_tx_reg_afe_ch1_sel   (0xc00)
#define shift_of_hdmi_tx_reg_afe_ch1_sel  (10)
#define HDMI_TX_REG_AFE_CH1_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_afe_ch1_sel)
// h0002, bit: 13
/* tmds channel sel, default 10. 00: ch0, 01: ch1, 10: ch2, 11: ch_ck*/
#define offset_of_hdmi_tx_reg_afe_ch2_sel (0x4)
#define mask_of_hdmi_tx_reg_afe_ch2_sel   (0x3000)
#define shift_of_hdmi_tx_reg_afe_ch2_sel  (12)
#define HDMI_TX_REG_AFE_CH2_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_afe_ch2_sel)
// h0002, bit: 15
/* tmds channel sel, default 11. 00: ch0, 01: ch1, 10: ch2, 11: ch_ck*/
#define offset_of_hdmi_tx_reg_afe_ch_ck_sel (0x4)
#define mask_of_hdmi_tx_reg_afe_ch_ck_sel   (0xc000)
#define shift_of_hdmi_tx_reg_afe_ch_ck_sel  (14)
#define HDMI_TX_REG_AFE_CH_CK_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_afe_ch_ck_sel)
// h0003, bit: 0
/* enable ch0 mute value = 16 when full range = 0
only active to confirm the level of video mute.*/
#define offset_of_hdmi_tx_reg_ycc422 (0x6)
#define mask_of_hdmi_tx_reg_ycc422   (0x1)
#define shift_of_hdmi_tx_reg_ycc422  (0)
#define HDMI_TX_REG_YCC422           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ycc422)
#define REG_HDMI_TX_03_L             (HDMI_TX_REG_YCC422)
// h0003, bit: 1
/* select yuv black out.*/
#define offset_of_hdmi_tx_reg_ycc444 (0x6)
#define mask_of_hdmi_tx_reg_ycc444   (0x2)
#define shift_of_hdmi_tx_reg_ycc444  (1)
#define HDMI_TX_REG_YCC444           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ycc444)
// h0003, bit: 2
/* 0: select mute value = 0, 1: select mute value = 16
Input video quantization level is full range, use to confirm the level of video mute.*/
#define offset_of_hdmi_tx_reg_full_range (0x6)
#define mask_of_hdmi_tx_reg_full_range   (0x4)
#define shift_of_hdmi_tx_reg_full_range  (2)
#define HDMI_TX_REG_FULL_RANGE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_full_range)
// h0003, bit: 3
/* no use*/
#define offset_of_hdmi_tx_reg_rgb (0x6)
#define mask_of_hdmi_tx_reg_rgb   (0x8)
#define shift_of_hdmi_tx_reg_rgb  (3)
#define HDMI_TX_REG_RGB           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_rgb)
// h0003, bit: 4
/* 1: hold audio data*/
#define offset_of_hdmi_tx_reg_audio_hold (0x6)
#define mask_of_hdmi_tx_reg_audio_hold   (0x10)
#define shift_of_hdmi_tx_reg_audio_hold  (4)
#define HDMI_TX_REG_AUDIO_HOLD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_hold)
// h0003, bit: 5
/* 1: set audio data to zero*/
#define offset_of_hdmi_tx_reg_audio_zero (0x6)
#define mask_of_hdmi_tx_reg_audio_zero   (0x20)
#define shift_of_hdmi_tx_reg_audio_zero  (5)
#define HDMI_TX_REG_AUDIO_ZERO           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_zero)
// h0003, bit: 6
/* 1: will not sample audio data, (remove input samples)*/
#define offset_of_hdmi_tx_reg_audio_rem (0x6)
#define mask_of_hdmi_tx_reg_audio_rem   (0x40)
#define shift_of_hdmi_tx_reg_audio_rem  (6)
#define HDMI_TX_REG_AUDIO_REM           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_rem)
// h0003, bit: 7
/* 1: mark invalid for audio samples*/
#define offset_of_hdmi_tx_reg_audio_invalid (0x6)
#define mask_of_hdmi_tx_reg_audio_invalid   (0x80)
#define shift_of_hdmi_tx_reg_audio_invalid  (7)
#define HDMI_TX_REG_AUDIO_INVALID           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_invalid)
// h0003, bit: 8
/* 1: select cts_n old mode, 0: select cts_n 1k mode*/
#define offset_of_hdmi_tx_reg_audio_n_sel (0x6)
#define mask_of_hdmi_tx_reg_audio_n_sel   (0x100)
#define shift_of_hdmi_tx_reg_audio_n_sel  (8)
#define HDMI_TX_REG_AUDIO_N_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_n_sel)
// h0003, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_03 (0x6)
#define mask_of_hdmi_tx_reg_reserved_03   (0xfe00)
#define shift_of_hdmi_tx_reg_reserved_03  (9)
#define HDMI_TX_REG_RESERVED_03           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_03)
// h0004, bit: 9
/* to hdmi_atop ck out*/
#define offset_of_hdmi_tx_reg_ch_ck_out (0x8)
#define mask_of_hdmi_tx_reg_ch_ck_out   (0x3ff)
#define shift_of_hdmi_tx_reg_ch_ck_out  (0)
#define HDMI_TX_REG_CH_CK_OUT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ch_ck_out)
#define REG_HDMI_TX_04_L                (HDMI_TX_REG_CH_CK_OUT)
// h0004, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_04 (0x8)
#define mask_of_hdmi_tx_reg_reserved_04   (0xfc00)
#define shift_of_hdmi_tx_reg_reserved_04  (10)
#define HDMI_TX_REG_RESERVED_04           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_04)
// h0005, bit: 0
/* graphic test enable, 1: sel rgb_test, 0: sel rgb_in*/
#define offset_of_hdmi_tx_reg_gtest_en (0xa)
#define mask_of_hdmi_tx_reg_gtest_en   (0x1)
#define shift_of_hdmi_tx_reg_gtest_en  (0)
#define HDMI_TX_REG_GTEST_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_gtest_en)
#define REG_HDMI_TX_05_L               (HDMI_TX_REG_GTEST_EN)
// h0005, bit: 2
/* 00: pure mode, 01: guard mode, 10: strip mode*/
#define offset_of_hdmi_tx_reg_gtest_mod (0xa)
#define mask_of_hdmi_tx_reg_gtest_mod   (0x6)
#define shift_of_hdmi_tx_reg_gtest_mod  (1)
#define HDMI_TX_REG_GTEST_MOD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_gtest_mod)
// h0005, bit: 3
/* mute ch0 enable*/
#define offset_of_hdmi_tx_reg_ch0_mute (0xa)
#define mask_of_hdmi_tx_reg_ch0_mute   (0x8)
#define shift_of_hdmi_tx_reg_ch0_mute  (3)
#define HDMI_TX_REG_CH0_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ch0_mute)
// h0005, bit: 4
/* mute ch1 enable*/
#define offset_of_hdmi_tx_reg_ch1_mute (0xa)
#define mask_of_hdmi_tx_reg_ch1_mute   (0x10)
#define shift_of_hdmi_tx_reg_ch1_mute  (4)
#define HDMI_TX_REG_CH1_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ch1_mute)
// h0005, bit: 5
/* mute ch2 enable*/
#define offset_of_hdmi_tx_reg_ch2_mute (0xa)
#define mask_of_hdmi_tx_reg_ch2_mute   (0x20)
#define shift_of_hdmi_tx_reg_ch2_mute  (5)
#define HDMI_TX_REG_CH2_MUTE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ch2_mute)
// h0005, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_05 (0xa)
#define mask_of_hdmi_tx_reg_reserved_05   (0xc0)
#define shift_of_hdmi_tx_reg_reserved_05  (6)
#define HDMI_TX_REG_RESERVED_05           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_05)
// h0005, bit: 15
/* strip_wid mode pattern*/
#define offset_of_hdmi_tx_reg_strip_width (0xa)
#define mask_of_hdmi_tx_reg_strip_width   (0xff00)
#define shift_of_hdmi_tx_reg_strip_width  (8)
#define HDMI_TX_REG_STRIP_WIDTH           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_strip_width)
// h0006, bit: 3
/* write 1 to enable
[0]: ycc422 10b to 8b
[1]: ycc444 to rgb444
[2]: rgb444 to ycc444
[3]: ycc444 to ycc422*/
#define offset_of_hdmi_tx_reg_csc_function_sel (0xc)
#define mask_of_hdmi_tx_reg_csc_function_sel   (0xf)
#define shift_of_hdmi_tx_reg_csc_function_sel  (0)
#define HDMI_TX_REG_CSC_FUNCTION_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_csc_function_sel)
#define REG_HDMI_TX_06_L                       (HDMI_TX_REG_CSC_FUNCTION_SEL)
// h0006, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_06 (0xc)
#define mask_of_hdmi_tx_reg_reserved_06   (0xfff0)
#define shift_of_hdmi_tx_reg_reserved_06  (4)
#define HDMI_TX_REG_RESERVED_06           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_06)
// h0007, bit: 0
/* cbcr swap*/
#define offset_of_hdmi_tx_reg_ycc_444to422_cbcr_swap (0xe)
#define mask_of_hdmi_tx_reg_ycc_444to422_cbcr_swap   (0x1)
#define shift_of_hdmi_tx_reg_ycc_444to422_cbcr_swap  (0)
#define HDMI_TX_REG_YCC_444TO422_CBCR_SWAP           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ycc_444to422_cbcr_swap)
#define REG_HDMI_TX_07_L                             (HDMI_TX_REG_YCC_444TO422_CBCR_SWAP)
// h0007, bit: 1
/* 1: average, 0: drop*/
#define offset_of_hdmi_tx_reg_ycc_444to422_c_filter (0xe)
#define mask_of_hdmi_tx_reg_ycc_444to422_c_filter   (0x2)
#define shift_of_hdmi_tx_reg_ycc_444to422_c_filter  (1)
#define HDMI_TX_REG_YCC_444TO422_C_FILTER           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ycc_444to422_c_filter)
// h0007, bit: 3
/* 00: normal
01: hdmi video input = csc r2y output
10: hdmi video input = csc y2r output
11: hdmi video input = sc input (skip csc module)*/
#define offset_of_hdmi_tx_reg_skip_csc (0xe)
#define mask_of_hdmi_tx_reg_skip_csc   (0xc)
#define shift_of_hdmi_tx_reg_skip_csc  (2)
#define HDMI_TX_REG_SKIP_CSC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_skip_csc)
// h0007, bit: 4
/* r2y dithering enable*/
#define offset_of_hdmi_tx_reg_r2y_dith_en (0xe)
#define mask_of_hdmi_tx_reg_r2y_dith_en   (0x10)
#define shift_of_hdmi_tx_reg_r2y_dith_en  (4)
#define HDMI_TX_REG_R2Y_DITH_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_r2y_dith_en)
// h0007, bit: 5
/* y2r dithering enable*/
#define offset_of_hdmi_tx_reg_y2r_dith_en (0xe)
#define mask_of_hdmi_tx_reg_y2r_dith_en   (0x20)
#define shift_of_hdmi_tx_reg_y2r_dith_en  (5)
#define HDMI_TX_REG_Y2R_DITH_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_y2r_dith_en)
// h0007, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_07 (0xe)
#define mask_of_hdmi_tx_reg_reserved_07   (0xffc0)
#define shift_of_hdmi_tx_reg_reserved_07  (6)
#define HDMI_TX_REG_RESERVED_07           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_07)
// h0008, bit: 0
/* 1: enable auxiliary data period in VB*/
#define offset_of_hdmi_tx_reg_v_cdi_en (0x10)
#define mask_of_hdmi_tx_reg_v_cdi_en   (0x1)
#define shift_of_hdmi_tx_reg_v_cdi_en  (0)
#define HDMI_TX_REG_V_CDI_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_cdi_en)
#define REG_HDMI_TX_08_L               (HDMI_TX_REG_V_CDI_EN)
// h0008, bit: 1
/* 1: enable audio data island in VB*/
#define offset_of_hdmi_tx_reg_v_adi_en (0x10)
#define mask_of_hdmi_tx_reg_v_adi_en   (0x2)
#define shift_of_hdmi_tx_reg_v_adi_en  (1)
#define HDMI_TX_REG_V_ADI_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_adi_en)
// h0008, bit: 2
/* 1: enable auxiliary data period in HB*/
#define offset_of_hdmi_tx_reg_h_cdi_en (0x10)
#define mask_of_hdmi_tx_reg_h_cdi_en   (0x4)
#define shift_of_hdmi_tx_reg_h_cdi_en  (2)
#define HDMI_TX_REG_H_CDI_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_cdi_en)
// h0008, bit: 3
/* 1: enable audio data island in HB*/
#define offset_of_hdmi_tx_reg_h_adi_en (0x10)
#define mask_of_hdmi_tx_reg_h_adi_en   (0x8)
#define shift_of_hdmi_tx_reg_h_adi_en  (3)
#define HDMI_TX_REG_H_ADI_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_adi_en)
// h0008, bit: 4
/* 1: enable opr and default ecc value, 0: internal ecc generation*/
#define offset_of_hdmi_tx_reg_eecc_en (0x10)
#define mask_of_hdmi_tx_reg_eecc_en   (0x10)
#define shift_of_hdmi_tx_reg_eecc_en  (4)
#define HDMI_TX_REG_EECC_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_eecc_en)
// h0008, bit: 5
/* no use
1: mix audio & control packet into data island A
0: no control packet mixed in data island A*/
#define offset_of_hdmi_tx_reg_pkt_mix (0x10)
#define mask_of_hdmi_tx_reg_pkt_mix   (0x20)
#define shift_of_hdmi_tx_reg_pkt_mix  (5)
#define HDMI_TX_REG_PKT_MIX           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt_mix)
// h0008, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_08 (0x10)
#define mask_of_hdmi_tx_reg_reserved_08   (0xffc0)
#define shift_of_hdmi_tx_reg_reserved_08  (6)
#define HDMI_TX_REG_RESERVED_08           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_08)
// h0009, bit: 7
/* 1st control period len in VB, unit Tpix
cdi len = vi_start[7:0]*4 + 4*/
#define offset_of_hdmi_tx_reg_v_di_start (0x12)
#define mask_of_hdmi_tx_reg_v_di_start   (0xff)
#define shift_of_hdmi_tx_reg_v_di_start  (0)
#define HDMI_TX_REG_V_DI_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_di_start)
#define REG_HDMI_TX_09_L                 (HDMI_TX_REG_V_DI_START)
// h0009, bit: 11
/* unit auxiliary pkt cnt per data island, start from 0(valid)*/
#define offset_of_hdmi_tx_reg_v_cdi_cnt (0x12)
#define mask_of_hdmi_tx_reg_v_cdi_cnt   (0xf00)
#define shift_of_hdmi_tx_reg_v_cdi_cnt  (8)
#define HDMI_TX_REG_V_CDI_CNT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_cdi_cnt)
// h0009, bit: 15
/* unit audio pkt cnt per data island, start from 0(valid)*/
#define offset_of_hdmi_tx_reg_v_adi_cnt (0x12)
#define mask_of_hdmi_tx_reg_v_adi_cnt   (0xf000)
#define shift_of_hdmi_tx_reg_v_adi_cnt  (12)
#define HDMI_TX_REG_V_ADI_CNT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_adi_cnt)
// h000a, bit: 7
/* 1st control period len in HB, unit Tpix
cdi len = hi_start[7:0]*4 + 4*/
#define offset_of_hdmi_tx_reg_h_di_start (0x14)
#define mask_of_hdmi_tx_reg_h_di_start   (0xff)
#define shift_of_hdmi_tx_reg_h_di_start  (0)
#define HDMI_TX_REG_H_DI_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_di_start)
#define REG_HDMI_TX_0A_L                 (HDMI_TX_REG_H_DI_START)
// h000a, bit: 11
/* unit auxiliary pkt cnt per data island, start from 0(valid)*/
#define offset_of_hdmi_tx_reg_h_cdi_cnt (0x14)
#define mask_of_hdmi_tx_reg_h_cdi_cnt   (0xf00)
#define shift_of_hdmi_tx_reg_h_cdi_cnt  (8)
#define HDMI_TX_REG_H_CDI_CNT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_cdi_cnt)
// h000a, bit: 15
/* unit audio pkt cnt per island, start from 0(valid)*/
#define offset_of_hdmi_tx_reg_h_adi_cnt (0x14)
#define mask_of_hdmi_tx_reg_h_adi_cnt   (0xf000)
#define shift_of_hdmi_tx_reg_h_adi_cnt  (12)
#define HDMI_TX_REG_H_ADI_CNT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_adi_cnt)
// h000b, bit: 7
/* VB max data island cnt (start from 0)
i.e. auxiliary data island count*/
#define offset_of_hdmi_tx_reg_v_cdi_max_cnt (0x16)
#define mask_of_hdmi_tx_reg_v_cdi_max_cnt   (0xff)
#define shift_of_hdmi_tx_reg_v_cdi_max_cnt  (0)
#define HDMI_TX_REG_V_CDI_MAX_CNT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_cdi_max_cnt)
#define REG_HDMI_TX_0B_L                    (HDMI_TX_REG_V_CDI_MAX_CNT)
// h000b, bit: 11
/* control period len between data island
= vi_spac[3:0]*4 + 4*/
#define offset_of_hdmi_tx_reg_v_cdi_spac (0x16)
#define mask_of_hdmi_tx_reg_v_cdi_spac   (0xf00)
#define shift_of_hdmi_tx_reg_v_cdi_spac  (8)
#define HDMI_TX_REG_V_CDI_SPAC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_cdi_spac)
// h000b, bit: 15
/* control period len between data island
= hi_spac[3:0]*4 + 4*/
#define offset_of_hdmi_tx_reg_h_cdi_spac (0x16)
#define mask_of_hdmi_tx_reg_h_cdi_spac   (0xf000)
#define shift_of_hdmi_tx_reg_h_cdi_spac  (12)
#define HDMI_TX_REG_H_CDI_SPAC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_cdi_spac)
// h000c, bit: 0
/* error flag clr toggle*/
#define offset_of_hdmi_tx_reg_error_flag_clr (0x18)
#define mask_of_hdmi_tx_reg_error_flag_clr   (0x1)
#define shift_of_hdmi_tx_reg_error_flag_clr  (0)
#define HDMI_TX_REG_ERROR_FLAG_CLR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_error_flag_clr)
#define REG_HDMI_TX_0C_L                     (HDMI_TX_REG_ERROR_FLAG_CLR)
// h000c, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_0c (0x18)
#define mask_of_hdmi_tx_reg_reserved_0c   (0xfffe)
#define shift_of_hdmi_tx_reg_reserved_0c  (1)
#define HDMI_TX_REG_RESERVED_0C           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_0c)
// h000d, bit: 3
/* ctl value in non-preamble period*/
#define offset_of_hdmi_tx_reg_ctl (0x1a)
#define mask_of_hdmi_tx_reg_ctl   (0xf)
#define shift_of_hdmi_tx_reg_ctl  (0)
#define HDMI_TX_REG_CTL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ctl)
#define REG_HDMI_TX_0D_L          (HDMI_TX_REG_CTL)
// h000d, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_0d (0x1a)
#define mask_of_hdmi_tx_reg_reserved_0d   (0xfff0)
#define shift_of_hdmi_tx_reg_reserved_0d  (4)
#define HDMI_TX_REG_RESERVED_0D           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_0d)
#define REG_HDMI_TX_0E_L                  (REG_HDMI_TX_BASE + 0x1c)
// h000f, bit: 0
/* 1: inverse, timing info usage, only use when sc is NEG*/
#define offset_of_hdmi_tx_reg_vsync_in_pol (0x1e)
#define mask_of_hdmi_tx_reg_vsync_in_pol   (0x1)
#define shift_of_hdmi_tx_reg_vsync_in_pol  (0)
#define HDMI_TX_REG_VSYNC_IN_POL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vsync_in_pol)
#define REG_HDMI_TX_0F_L                   (HDMI_TX_REG_VSYNC_IN_POL)
// h000f, bit: 1
/* 1: inverse, timing info usage, only use when sc is NEG*/
#define offset_of_hdmi_tx_reg_hsync_in_pol (0x1e)
#define mask_of_hdmi_tx_reg_hsync_in_pol   (0x2)
#define shift_of_hdmi_tx_reg_hsync_in_pol  (1)
#define HDMI_TX_REG_HSYNC_IN_POL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hsync_in_pol)
// h000f, bit: 2
/* 0: normal out, 1: inverse out*/
#define offset_of_hdmi_tx_reg_vsync_out_pol (0x1e)
#define mask_of_hdmi_tx_reg_vsync_out_pol   (0x4)
#define shift_of_hdmi_tx_reg_vsync_out_pol  (2)
#define HDMI_TX_REG_VSYNC_OUT_POL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vsync_out_pol)
// h000f, bit: 3
/* 0: normal out, 1: inverse out*/
#define offset_of_hdmi_tx_reg_hsync_out_pol (0x1e)
#define mask_of_hdmi_tx_reg_hsync_out_pol   (0x8)
#define shift_of_hdmi_tx_reg_hsync_out_pol  (3)
#define HDMI_TX_REG_HSYNC_OUT_POL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hsync_out_pol)
// h000f, bit: 4
/* interlace enable*/
#define offset_of_hdmi_tx_reg_interlace_en (0x1e)
#define mask_of_hdmi_tx_reg_interlace_en   (0x10)
#define shift_of_hdmi_tx_reg_interlace_en  (4)
#define HDMI_TX_REG_INTERLACE_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_interlace_en)
// h000f, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_0f (0x1e)
#define mask_of_hdmi_tx_reg_reserved_0f   (0xffe0)
#define shift_of_hdmi_tx_reg_reserved_0f  (5)
#define HDMI_TX_REG_RESERVED_0F           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_0f)
// h0010, bit: 12
/* H_TOTAL, total pixel minus 1*/
#define offset_of_hdmi_tx_reg_h_total (0x20)
#define mask_of_hdmi_tx_reg_h_total   (0x1fff)
#define shift_of_hdmi_tx_reg_h_total  (0)
#define HDMI_TX_REG_H_TOTAL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_h_total)
#define REG_HDMI_TX_10_L              (HDMI_TX_REG_H_TOTAL)
// h0011, bit: 12
/* V_TOTAL, total pixel minus 1*/
#define offset_of_hdmi_tx_reg_v_total (0x22)
#define mask_of_hdmi_tx_reg_v_total   (0x1fff)
#define shift_of_hdmi_tx_reg_v_total  (0)
#define HDMI_TX_REG_V_TOTAL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_v_total)
#define REG_HDMI_TX_11_L              (HDMI_TX_REG_V_TOTAL)
// h0012, bit: 12
/* HSYNC start (include 0)*/
#define offset_of_hdmi_tx_reg_hs_start (0x24)
#define mask_of_hdmi_tx_reg_hs_start   (0x1fff)
#define shift_of_hdmi_tx_reg_hs_start  (0)
#define HDMI_TX_REG_HS_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hs_start)
#define REG_HDMI_TX_12_L               (HDMI_TX_REG_HS_START)
// h0013, bit: 12
/* HSYNC end (include 0)*/
#define offset_of_hdmi_tx_reg_hs_end (0x26)
#define mask_of_hdmi_tx_reg_hs_end   (0x1fff)
#define shift_of_hdmi_tx_reg_hs_end  (0)
#define HDMI_TX_REG_HS_END           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hs_end)
#define REG_HDMI_TX_13_L             (HDMI_TX_REG_HS_END)
// h0014, bit: 12
/* VSYNC start (include 0)*/
#define offset_of_hdmi_tx_reg_vs_start (0x28)
#define mask_of_hdmi_tx_reg_vs_start   (0x1fff)
#define shift_of_hdmi_tx_reg_vs_start  (0)
#define HDMI_TX_REG_VS_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vs_start)
#define REG_HDMI_TX_14_L               (HDMI_TX_REG_VS_START)
// h0015, bit: 12
/* VSYNC end (include 0)*/
#define offset_of_hdmi_tx_reg_vs_end (0x2a)
#define mask_of_hdmi_tx_reg_vs_end   (0x1fff)
#define shift_of_hdmi_tx_reg_vs_end  (0)
#define HDMI_TX_REG_VS_END           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vs_end)
#define REG_HDMI_TX_15_L             (HDMI_TX_REG_VS_END)
// h0016, bit: 12
/* H frame de start (include 0)*/
#define offset_of_hdmi_tx_reg_hfde_start (0x2c)
#define mask_of_hdmi_tx_reg_hfde_start   (0x1fff)
#define shift_of_hdmi_tx_reg_hfde_start  (0)
#define HDMI_TX_REG_HFDE_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hfde_start)
#define REG_HDMI_TX_16_L                 (HDMI_TX_REG_HFDE_START)
// h0017, bit: 12
/* H frame de end (include 0)*/
#define offset_of_hdmi_tx_reg_hfde_end (0x2e)
#define mask_of_hdmi_tx_reg_hfde_end   (0x1fff)
#define shift_of_hdmi_tx_reg_hfde_end  (0)
#define HDMI_TX_REG_HFDE_END           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hfde_end)
#define REG_HDMI_TX_17_L               (HDMI_TX_REG_HFDE_END)
// h0018, bit: 12
/* V frame de start (include 0)*/
#define offset_of_hdmi_tx_reg_vfde_start (0x30)
#define mask_of_hdmi_tx_reg_vfde_start   (0x1fff)
#define shift_of_hdmi_tx_reg_vfde_start  (0)
#define HDMI_TX_REG_VFDE_START           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vfde_start)
#define REG_HDMI_TX_18_L                 (HDMI_TX_REG_VFDE_START)
// h0019, bit: 12
/* V frame de end (include 0)*/
#define offset_of_hdmi_tx_reg_vfde_end (0x32)
#define mask_of_hdmi_tx_reg_vfde_end   (0x1fff)
#define shift_of_hdmi_tx_reg_vfde_end  (0)
#define HDMI_TX_REG_VFDE_END           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_vfde_end)
#define REG_HDMI_TX_19_L               (HDMI_TX_REG_VFDE_END)
// h001a, bit: 0
/* enable hdmi TMDS 1to40 clock gating.*/
#define offset_of_hdmi_tx_reg_hdmi_clk1to40 (0x34)
#define mask_of_hdmi_tx_reg_hdmi_clk1to40   (0x1)
#define shift_of_hdmi_tx_reg_hdmi_clk1to40  (0)
#define HDMI_TX_REG_HDMI_CLK1TO40           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_clk1to40)
#define REG_HDMI_TX_1A_L                    (HDMI_TX_REG_HDMI_CLK1TO40)
// h001a, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1a (0x34)
#define mask_of_hdmi_tx_reg_reserved_1a   (0xfffe)
#define shift_of_hdmi_tx_reg_reserved_1a  (1)
#define HDMI_TX_REG_RESERVED_1A           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1a)
// h001b, bit: 0
/* 1: pkt0 enable, high priority*/
#define offset_of_hdmi_tx_reg_pkt0_en (0x36)
#define mask_of_hdmi_tx_reg_pkt0_en   (0x1)
#define shift_of_hdmi_tx_reg_pkt0_en  (0)
#define HDMI_TX_REG_PKT0_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_en)
#define REG_HDMI_TX_1B_L              (HDMI_TX_REG_PKT0_EN)
// h001b, bit: 1
/* 1: pkt1 enable*/
#define offset_of_hdmi_tx_reg_pkt1_en (0x36)
#define mask_of_hdmi_tx_reg_pkt1_en   (0x2)
#define shift_of_hdmi_tx_reg_pkt1_en  (1)
#define HDMI_TX_REG_PKT1_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_en)
// h001b, bit: 2
/* 1: audio top function enable*/
#define offset_of_hdmi_tx_reg_audio_en (0x36)
#define mask_of_hdmi_tx_reg_audio_en   (0x4)
#define shift_of_hdmi_tx_reg_audio_en  (2)
#define HDMI_TX_REG_AUDIO_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_audio_en)
// h001b, bit: 3
/* 1: acr enable*/
#define offset_of_hdmi_tx_reg_acr_en (0x36)
#define mask_of_hdmi_tx_reg_acr_en   (0x8)
#define shift_of_hdmi_tx_reg_acr_en  (3)
#define HDMI_TX_REG_ACR_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_en)
// h001b, bit: 4
/* 1: ado enable*/
#define offset_of_hdmi_tx_reg_ado_en (0x36)
#define mask_of_hdmi_tx_reg_ado_en   (0x10)
#define shift_of_hdmi_tx_reg_ado_en  (4)
#define HDMI_TX_REG_ADO_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_en)
// h001b, bit: 5
/* 1: avi enable*/
#define offset_of_hdmi_tx_reg_avi_en (0x36)
#define mask_of_hdmi_tx_reg_avi_en   (0x20)
#define shift_of_hdmi_tx_reg_avi_en  (5)
#define HDMI_TX_REG_AVI_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_en)
// h001b, bit: 6
/* 1: gc enable*/
#define offset_of_hdmi_tx_reg_gc_en (0x36)
#define mask_of_hdmi_tx_reg_gc_en   (0x40)
#define shift_of_hdmi_tx_reg_gc_en  (6)
#define HDMI_TX_REG_GC_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_gc_en)
// h001b, bit: 7
/* 1: nul enable*/
#define offset_of_hdmi_tx_reg_nul_en (0x36)
#define mask_of_hdmi_tx_reg_nul_en   (0x80)
#define shift_of_hdmi_tx_reg_nul_en  (7)
#define HDMI_TX_REG_NUL_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_nul_en)
// h001b, bit: 8
/* 1: pkt2 enable*/
#define offset_of_hdmi_tx_reg_pkt2_en (0x36)
#define mask_of_hdmi_tx_reg_pkt2_en   (0x100)
#define shift_of_hdmi_tx_reg_pkt2_en  (8)
#define HDMI_TX_REG_PKT2_EN           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_en)
// h001b, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1b (0x36)
#define mask_of_hdmi_tx_reg_reserved_1b   (0xfe00)
#define shift_of_hdmi_tx_reg_reserved_1b  (9)
#define HDMI_TX_REG_RESERVED_1B           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1b)
// h001c, bit: 0
/* sw use pkt0 rdy (toggle)*/
#define offset_of_hdmi_tx_reg_pkt0_rdy (0x38)
#define mask_of_hdmi_tx_reg_pkt0_rdy   (0x1)
#define shift_of_hdmi_tx_reg_pkt0_rdy  (0)
#define HDMI_TX_REG_PKT0_RDY           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_rdy)
#define REG_HDMI_TX_1C_L               (HDMI_TX_REG_PKT0_RDY)
// h001c, bit: 1
/* sw use pkt1 rdy (toggle)*/
#define offset_of_hdmi_tx_reg_pkt1_rdy (0x38)
#define mask_of_hdmi_tx_reg_pkt1_rdy   (0x2)
#define shift_of_hdmi_tx_reg_pkt1_rdy  (1)
#define HDMI_TX_REG_PKT1_RDY           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_rdy)
// h001c, bit: 2
/* sw use pkt2 rdy (toggle)*/
#define offset_of_hdmi_tx_reg_pkt2_rdy (0x38)
#define mask_of_hdmi_tx_reg_pkt2_rdy   (0x4)
#define shift_of_hdmi_tx_reg_pkt2_rdy  (2)
#define HDMI_TX_REG_PKT2_RDY           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_rdy)
// h001c, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1c (0x38)
#define mask_of_hdmi_tx_reg_reserved_1c   (0xfff8)
#define shift_of_hdmi_tx_reg_reserved_1c  (3)
#define HDMI_TX_REG_RESERVED_1C           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1c)
// h001d, bit: 0
/* 1: sel pkt0_txon(hw), 0: sel pkt0_rdy(sw)*/
#define offset_of_hdmi_tx_reg_pkt0_rdy_sel (0x3a)
#define mask_of_hdmi_tx_reg_pkt0_rdy_sel   (0x1)
#define shift_of_hdmi_tx_reg_pkt0_rdy_sel  (0)
#define HDMI_TX_REG_PKT0_RDY_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_rdy_sel)
#define REG_HDMI_TX_1D_L                   (HDMI_TX_REG_PKT0_RDY_SEL)
// h001d, bit: 1
/* 1: sel pkt1_txon(hw), 0: sel pkt1_rdy(sw)*/
#define offset_of_hdmi_tx_reg_pkt1_rdy_sel (0x3a)
#define mask_of_hdmi_tx_reg_pkt1_rdy_sel   (0x2)
#define shift_of_hdmi_tx_reg_pkt1_rdy_sel  (1)
#define HDMI_TX_REG_PKT1_RDY_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_rdy_sel)
// h001d, bit: 2
/* 1: sel pkt2_txon(hw), 0: sel pkt2_rdy(sw)*/
#define offset_of_hdmi_tx_reg_pkt2_rdy_sel (0x3a)
#define mask_of_hdmi_tx_reg_pkt2_rdy_sel   (0x4)
#define shift_of_hdmi_tx_reg_pkt2_rdy_sel  (2)
#define HDMI_TX_REG_PKT2_RDY_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_rdy_sel)
// h001d, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1d (0x3a)
#define mask_of_hdmi_tx_reg_reserved_1d   (0xfff8)
#define shift_of_hdmi_tx_reg_reserved_1d  (3)
#define HDMI_TX_REG_RESERVED_1D           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1d)
// h001e, bit: 1
/* txon_set
00: act_line_nshot
01: vsync_pshot
other: vsync_nshot*/
#define offset_of_hdmi_tx_reg_pfp_ts (0x3c)
#define mask_of_hdmi_tx_reg_pfp_ts   (0x3)
#define shift_of_hdmi_tx_reg_pfp_ts  (0)
#define HDMI_TX_REG_PFP_TS           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pfp_ts)
#define REG_HDMI_TX_1E_L             (HDMI_TX_REG_PFP_TS)
// h001e, bit: 3
/* txon_clr
00: vsync_pshot
01: vsync_nshot
other: act_line_pshot*/
#define offset_of_hdmi_tx_reg_pfp_te (0x3c)
#define mask_of_hdmi_tx_reg_pfp_te   (0xc)
#define shift_of_hdmi_tx_reg_pfp_te  (2)
#define HDMI_TX_REG_PFP_TE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pfp_te)
// h001e, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1e (0x3c)
#define mask_of_hdmi_tx_reg_reserved_1e   (0xfff0)
#define shift_of_hdmi_tx_reg_reserved_1e  (4)
#define HDMI_TX_REG_RESERVED_1E           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1e)
// h001f, bit: 0
/* 1: control by reg_av_mute level, level sensitive
0: edge sensitive*/
#define offset_of_hdmi_tx_reg_av_mute_mod (0x3e)
#define mask_of_hdmi_tx_reg_av_mute_mod   (0x1)
#define shift_of_hdmi_tx_reg_av_mute_mod  (0)
#define HDMI_TX_REG_AV_MUTE_MOD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_av_mute_mod)
#define REG_HDMI_TX_1F_L                  (HDMI_TX_REG_AV_MUTE_MOD)
// h001f, bit: 1
/* av_mute edge trigger delay for gc
0: trigger one time
1: trigger clr_avmute/set_avmute four times*/
#define offset_of_hdmi_tx_reg_av_mute_mtr (0x3e)
#define mask_of_hdmi_tx_reg_av_mute_mtr   (0x2)
#define shift_of_hdmi_tx_reg_av_mute_mtr  (1)
#define HDMI_TX_REG_AV_MUTE_MTR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_av_mute_mtr)
// h001f, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_1f (0x3e)
#define mask_of_hdmi_tx_reg_reserved_1f   (0xfffc)
#define shift_of_hdmi_tx_reg_reserved_1f  (2)
#define HDMI_TX_REG_RESERVED_1F           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_1f)
// h0020, bit: 3
/* asp sample flat setting*/
#define offset_of_hdmi_tx_reg_asp_samp_flat (0x40)
#define mask_of_hdmi_tx_reg_asp_samp_flat   (0xf)
#define shift_of_hdmi_tx_reg_asp_samp_flat  (0)
#define HDMI_TX_REG_ASP_SAMP_FLAT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_samp_flat)
#define REG_HDMI_TX_20_L                    (HDMI_TX_REG_ASP_SAMP_FLAT)
// h0020, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_20 (0x40)
#define mask_of_hdmi_tx_reg_reserved_20   (0xfff0)
#define shift_of_hdmi_tx_reg_reserved_20  (4)
#define HDMI_TX_REG_RESERVED_20           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_20)
// bigger than 16 bit case allh0021
#define offset_of_hdmi_tx_reg_asp_ch_status (0x42)
#define mask_of_hdmi_tx_reg_asp_ch_status   (0xffffffff)
#define shift_of_hdmi_tx_reg_asp_ch_status  (0)
#define HDMI_TX_REG_ASP_CH_STATUS           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status)
// h0021, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_0 (0x42)
#define mask_of_hdmi_tx_reg_asp_ch_status_0   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_0  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_0)
#define REG_HDMI_TX_21_L                      (HDMI_TX_REG_ASP_CH_STATUS_0)
// h0022, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_1 (0x44)
#define mask_of_hdmi_tx_reg_asp_ch_status_1   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_1  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_1)
#define REG_HDMI_TX_22_L                      (HDMI_TX_REG_ASP_CH_STATUS_1)
// h0023, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_2 (0x46)
#define mask_of_hdmi_tx_reg_asp_ch_status_2   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_2  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_2)
#define REG_HDMI_TX_23_L                      (HDMI_TX_REG_ASP_CH_STATUS_2)
// h0024, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_3 (0x48)
#define mask_of_hdmi_tx_reg_asp_ch_status_3   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_3  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_3)
#define REG_HDMI_TX_24_L                      (HDMI_TX_REG_ASP_CH_STATUS_3)
// h0025, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_4 (0x4a)
#define mask_of_hdmi_tx_reg_asp_ch_status_4   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_4  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_4           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_4)
#define REG_HDMI_TX_25_L                      (HDMI_TX_REG_ASP_CH_STATUS_4)
// h0026, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_5 (0x4c)
#define mask_of_hdmi_tx_reg_asp_ch_status_5   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_5  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_5           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_5)
#define REG_HDMI_TX_26_L                      (HDMI_TX_REG_ASP_CH_STATUS_5)
// h0027, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_6 (0x4e)
#define mask_of_hdmi_tx_reg_asp_ch_status_6   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_6  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_6           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_6)
#define REG_HDMI_TX_27_L                      (HDMI_TX_REG_ASP_CH_STATUS_6)
// h0028, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_7 (0x50)
#define mask_of_hdmi_tx_reg_asp_ch_status_7   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_7  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_7           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_7)
#define REG_HDMI_TX_28_L                      (HDMI_TX_REG_ASP_CH_STATUS_7)
// h0029, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_8 (0x52)
#define mask_of_hdmi_tx_reg_asp_ch_status_8   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_8  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_8           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_8)
#define REG_HDMI_TX_29_L                      (HDMI_TX_REG_ASP_CH_STATUS_8)
// h002a, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_9 (0x54)
#define mask_of_hdmi_tx_reg_asp_ch_status_9   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_9  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_9           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_9)
#define REG_HDMI_TX_2A_L                      (HDMI_TX_REG_ASP_CH_STATUS_9)
// h002b, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_10 (0x56)
#define mask_of_hdmi_tx_reg_asp_ch_status_10   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_10  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_10           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_10)
#define REG_HDMI_TX_2B_L                       (HDMI_TX_REG_ASP_CH_STATUS_10)
// h002c, bit: 15
/* channel status(pcuv), for asp packet (l_pcm/compress), 192 frames usage*/
#define offset_of_hdmi_tx_reg_asp_ch_status_11 (0x58)
#define mask_of_hdmi_tx_reg_asp_ch_status_11   (0xffff)
#define shift_of_hdmi_tx_reg_asp_ch_status_11  (0)
#define HDMI_TX_REG_ASP_CH_STATUS_11           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_asp_ch_status_11)
#define REG_HDMI_TX_2C_L                       (HDMI_TX_REG_ASP_CH_STATUS_11)
// bigger than 16 bit case allh002d
#define offset_of_hdmi_tx_reg_test_asp_hd (0x5a)
#define mask_of_hdmi_tx_reg_test_asp_hd   (0xffffff)
#define shift_of_hdmi_tx_reg_test_asp_hd  (0)
#define HDMI_TX_REG_TEST_ASP_HD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_test_asp_hd)
// h002d, bit: 15
/* test, asp head*/
#define offset_of_hdmi_tx_reg_test_asp_hd_0 (0x5a)
#define mask_of_hdmi_tx_reg_test_asp_hd_0   (0xffff)
#define shift_of_hdmi_tx_reg_test_asp_hd_0  (0)
#define HDMI_TX_REG_TEST_ASP_HD_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_test_asp_hd_0)
#define REG_HDMI_TX_2D_L                    (HDMI_TX_REG_TEST_ASP_HD_0)
// h002e, bit: 7
/* test, asp head*/
#define offset_of_hdmi_tx_reg_test_asp_hd_1 (0x5c)
#define mask_of_hdmi_tx_reg_test_asp_hd_1   (0xff)
#define shift_of_hdmi_tx_reg_test_asp_hd_1  (0)
#define HDMI_TX_REG_TEST_ASP_HD_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_test_asp_hd_1)
#define REG_HDMI_TX_2E_L                    (HDMI_TX_REG_TEST_ASP_HD_1)
// h002e, bit: 15
/* test, asp head ecc*/
#define offset_of_hdmi_tx_reg_test_asp_hd_ecc (0x5c)
#define mask_of_hdmi_tx_reg_test_asp_hd_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_test_asp_hd_ecc  (8)
#define HDMI_TX_REG_TEST_ASP_HD_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_test_asp_hd_ecc)
// h002f, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_2f (0x5e)
#define mask_of_hdmi_tx_reg_reserved_2f   (0xffff)
#define shift_of_hdmi_tx_reg_reserved_2f  (0)
#define HDMI_TX_REG_RESERVED_2F           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_2f)
#define REG_HDMI_TX_2F_L                  (HDMI_TX_REG_RESERVED_2F)
// h0030, bit: 0
/* 1: sel extr_cts & extr_n(from counter), 0: sel acr_cts & acr_n(from reg)*/
#define offset_of_hdmi_tx_reg_acr_mod (0x60)
#define mask_of_hdmi_tx_reg_acr_mod   (0x1)
#define shift_of_hdmi_tx_reg_acr_mod  (0)
#define HDMI_TX_REG_ACR_MOD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_mod)
#define REG_HDMI_TX_30_L              (HDMI_TX_REG_ACR_MOD)
// h0030, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_30 (0x60)
#define mask_of_hdmi_tx_reg_reserved_30   (0xfffe)
#define shift_of_hdmi_tx_reg_reserved_30  (1)
#define HDMI_TX_REG_RESERVED_30           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_30)
// h0031, bit: 15
/* audio clock regeneration cts[15:0]*/
#define offset_of_hdmi_tx_reg_acr_cts_l (0x62)
#define mask_of_hdmi_tx_reg_acr_cts_l   (0xffff)
#define shift_of_hdmi_tx_reg_acr_cts_l  (0)
#define HDMI_TX_REG_ACR_CTS_L           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_cts_l)
#define REG_HDMI_TX_31_L                (HDMI_TX_REG_ACR_CTS_L)
// h0032, bit: 15
/* audio clock regeneration n[15:0]*/
#define offset_of_hdmi_tx_reg_acr_n_l (0x64)
#define mask_of_hdmi_tx_reg_acr_n_l   (0xffff)
#define shift_of_hdmi_tx_reg_acr_n_l  (0)
#define HDMI_TX_REG_ACR_N_L           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_n_l)
#define REG_HDMI_TX_32_L              (HDMI_TX_REG_ACR_N_L)
// h0033, bit: 3
/* audio clock regeneration cts[19:16]*/
#define offset_of_hdmi_tx_reg_acr_cts_h (0x66)
#define mask_of_hdmi_tx_reg_acr_cts_h   (0xf)
#define shift_of_hdmi_tx_reg_acr_cts_h  (0)
#define HDMI_TX_REG_ACR_CTS_H           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_cts_h)
#define REG_HDMI_TX_33_L                (HDMI_TX_REG_ACR_CTS_H)
// h0033, bit: 7
/* audio clock regeneration n[19:16]*/
#define offset_of_hdmi_tx_reg_acr_n_h (0x66)
#define mask_of_hdmi_tx_reg_acr_n_h   (0xf0)
#define shift_of_hdmi_tx_reg_acr_n_h  (4)
#define HDMI_TX_REG_ACR_N_H           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_acr_n_h)
// h0033, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_reg_reserved_33 (0x66)
#define mask_of_hdmi_tx_reg_reserved_33   (0xff00)
#define shift_of_hdmi_tx_reg_reserved_33  (8)
#define HDMI_TX_REG_RESERVED_33           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_reserved_33)
// bigger than 16 bit case allh0034
#define offset_of_hdmi_tx_reg_avi_s0 (0x68)
#define mask_of_hdmi_tx_reg_avi_s0   (0xffffffff)
#define shift_of_hdmi_tx_reg_avi_s0  (0)
#define HDMI_TX_REG_AVI_S0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s0)
// h0034, bit: 15
/* avi subpacket0*/
#define offset_of_hdmi_tx_reg_avi_s0_0 (0x68)
#define mask_of_hdmi_tx_reg_avi_s0_0   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s0_0  (0)
#define HDMI_TX_REG_AVI_S0_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s0_0)
#define REG_HDMI_TX_34_L               (HDMI_TX_REG_AVI_S0_0)
// h0035, bit: 15
/* avi subpacket0*/
#define offset_of_hdmi_tx_reg_avi_s0_1 (0x6a)
#define mask_of_hdmi_tx_reg_avi_s0_1   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s0_1  (0)
#define HDMI_TX_REG_AVI_S0_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s0_1)
#define REG_HDMI_TX_35_L               (HDMI_TX_REG_AVI_S0_1)
// h0036, bit: 15
/* avi subpacket0*/
#define offset_of_hdmi_tx_reg_avi_s0_2 (0x6c)
#define mask_of_hdmi_tx_reg_avi_s0_2   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s0_2  (0)
#define HDMI_TX_REG_AVI_S0_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s0_2)
#define REG_HDMI_TX_36_L               (HDMI_TX_REG_AVI_S0_2)
// h0037, bit: 7
/* avi subpacket0*/
#define offset_of_hdmi_tx_reg_avi_s0_3 (0x6e)
#define mask_of_hdmi_tx_reg_avi_s0_3   (0xff)
#define shift_of_hdmi_tx_reg_avi_s0_3  (0)
#define HDMI_TX_REG_AVI_S0_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s0_3)
#define REG_HDMI_TX_37_L               (HDMI_TX_REG_AVI_S0_3)
// bigger than 16 bit case allh0038
#define offset_of_hdmi_tx_reg_avi_s1 (0x70)
#define mask_of_hdmi_tx_reg_avi_s1   (0xffffffff)
#define shift_of_hdmi_tx_reg_avi_s1  (0)
#define HDMI_TX_REG_AVI_S1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s1)
// h0038, bit: 15
/* avi subpacket1*/
#define offset_of_hdmi_tx_reg_avi_s1_0 (0x70)
#define mask_of_hdmi_tx_reg_avi_s1_0   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s1_0  (0)
#define HDMI_TX_REG_AVI_S1_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s1_0)
#define REG_HDMI_TX_38_L               (HDMI_TX_REG_AVI_S1_0)
// h0039, bit: 15
/* avi subpacket1*/
#define offset_of_hdmi_tx_reg_avi_s1_1 (0x72)
#define mask_of_hdmi_tx_reg_avi_s1_1   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s1_1  (0)
#define HDMI_TX_REG_AVI_S1_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s1_1)
#define REG_HDMI_TX_39_L               (HDMI_TX_REG_AVI_S1_1)
// h003a, bit: 15
/* avi subpacket1*/
#define offset_of_hdmi_tx_reg_avi_s1_2 (0x74)
#define mask_of_hdmi_tx_reg_avi_s1_2   (0xffff)
#define shift_of_hdmi_tx_reg_avi_s1_2  (0)
#define HDMI_TX_REG_AVI_S1_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s1_2)
#define REG_HDMI_TX_3A_L               (HDMI_TX_REG_AVI_S1_2)
// h003b, bit: 7
/* avi subpacket1*/
#define offset_of_hdmi_tx_reg_avi_s1_3 (0x76)
#define mask_of_hdmi_tx_reg_avi_s1_3   (0xff)
#define shift_of_hdmi_tx_reg_avi_s1_3  (0)
#define HDMI_TX_REG_AVI_S1_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_avi_s1_3)
#define REG_HDMI_TX_3B_L               (HDMI_TX_REG_AVI_S1_3)
// bigger than 16 bit case allh003c
#define offset_of_hdmi_tx_reg_ado_s0 (0x78)
#define mask_of_hdmi_tx_reg_ado_s0   (0xffffffff)
#define shift_of_hdmi_tx_reg_ado_s0  (0)
#define HDMI_TX_REG_ADO_S0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_s0)
// h003c, bit: 15
/* audio info subpack0*/
#define offset_of_hdmi_tx_reg_ado_s0_0 (0x78)
#define mask_of_hdmi_tx_reg_ado_s0_0   (0xffff)
#define shift_of_hdmi_tx_reg_ado_s0_0  (0)
#define HDMI_TX_REG_ADO_S0_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_s0_0)
#define REG_HDMI_TX_3C_L               (HDMI_TX_REG_ADO_S0_0)
// h003d, bit: 15
/* audio info subpack0*/
#define offset_of_hdmi_tx_reg_ado_s0_1 (0x7a)
#define mask_of_hdmi_tx_reg_ado_s0_1   (0xffff)
#define shift_of_hdmi_tx_reg_ado_s0_1  (0)
#define HDMI_TX_REG_ADO_S0_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_s0_1)
#define REG_HDMI_TX_3D_L               (HDMI_TX_REG_ADO_S0_1)
// h003e, bit: 15
/* audio info subpack0*/
#define offset_of_hdmi_tx_reg_ado_s0_2 (0x7c)
#define mask_of_hdmi_tx_reg_ado_s0_2   (0xffff)
#define shift_of_hdmi_tx_reg_ado_s0_2  (0)
#define HDMI_TX_REG_ADO_S0_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_s0_2)
#define REG_HDMI_TX_3E_L               (HDMI_TX_REG_ADO_S0_2)
// h003f, bit: 7
/* audio info subpack0*/
#define offset_of_hdmi_tx_reg_ado_s0_3 (0x7e)
#define mask_of_hdmi_tx_reg_ado_s0_3   (0xff)
#define shift_of_hdmi_tx_reg_ado_s0_3  (0)
#define HDMI_TX_REG_ADO_S0_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_ado_s0_3)
#define REG_HDMI_TX_3F_L               (HDMI_TX_REG_ADO_S0_3)
// bigger than 16 bit case allh0040
#define offset_of_hdmi_tx_reg_pkt0_hd (0x80)
#define mask_of_hdmi_tx_reg_pkt0_hd   (0xffffff)
#define shift_of_hdmi_tx_reg_pkt0_hd  (0)
#define HDMI_TX_REG_PKT0_HD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_hd)
// h0040, bit: 15
/* pkt0 head*/
#define offset_of_hdmi_tx_reg_pkt0_hd_0 (0x80)
#define mask_of_hdmi_tx_reg_pkt0_hd_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_hd_0  (0)
#define HDMI_TX_REG_PKT0_HD_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_hd_0)
#define REG_HDMI_TX_40_L                (HDMI_TX_REG_PKT0_HD_0)
// h0041, bit: 7
/* pkt0 head*/
#define offset_of_hdmi_tx_reg_pkt0_hd_1 (0x82)
#define mask_of_hdmi_tx_reg_pkt0_hd_1   (0xff)
#define shift_of_hdmi_tx_reg_pkt0_hd_1  (0)
#define HDMI_TX_REG_PKT0_HD_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_hd_1)
#define REG_HDMI_TX_41_L                (HDMI_TX_REG_PKT0_HD_1)
// h0041, bit: 15
/* pkt0 ecc*/
#define offset_of_hdmi_tx_reg_pkt0_hd_ecc (0x82)
#define mask_of_hdmi_tx_reg_pkt0_hd_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt0_hd_ecc  (8)
#define HDMI_TX_REG_PKT0_HD_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_hd_ecc)
// bigger than 16 bit case allh0042
#define offset_of_hdmi_tx_reg_pkt0_s0 (0x84)
#define mask_of_hdmi_tx_reg_pkt0_s0   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt0_s0  (0)
#define HDMI_TX_REG_PKT0_S0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0)
// h0042, bit: 15
/* pkt0 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt0_s0_0 (0x84)
#define mask_of_hdmi_tx_reg_pkt0_s0_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s0_0  (0)
#define HDMI_TX_REG_PKT0_S0_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0_0)
#define REG_HDMI_TX_42_L                (HDMI_TX_REG_PKT0_S0_0)
// h0043, bit: 15
/* pkt0 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt0_s0_1 (0x86)
#define mask_of_hdmi_tx_reg_pkt0_s0_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s0_1  (0)
#define HDMI_TX_REG_PKT0_S0_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0_1)
#define REG_HDMI_TX_43_L                (HDMI_TX_REG_PKT0_S0_1)
// h0044, bit: 15
/* pkt0 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt0_s0_2 (0x88)
#define mask_of_hdmi_tx_reg_pkt0_s0_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s0_2  (0)
#define HDMI_TX_REG_PKT0_S0_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0_2)
#define REG_HDMI_TX_44_L                (HDMI_TX_REG_PKT0_S0_2)
// h0045, bit: 7
/* pkt0 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt0_s0_3 (0x8a)
#define mask_of_hdmi_tx_reg_pkt0_s0_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt0_s0_3  (0)
#define HDMI_TX_REG_PKT0_S0_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0_3)
#define REG_HDMI_TX_45_L                (HDMI_TX_REG_PKT0_S0_3)
// h0045, bit: 15
/* pkt0 subpacket0 ecc*/
#define offset_of_hdmi_tx_reg_pkt0_s0_ecc (0x8a)
#define mask_of_hdmi_tx_reg_pkt0_s0_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt0_s0_ecc  (8)
#define HDMI_TX_REG_PKT0_S0_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s0_ecc)
// bigger than 16 bit case allh0046
#define offset_of_hdmi_tx_reg_pkt0_s1 (0x8c)
#define mask_of_hdmi_tx_reg_pkt0_s1   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt0_s1  (0)
#define HDMI_TX_REG_PKT0_S1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1)
// h0046, bit: 15
/* pkt0 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt0_s1_0 (0x8c)
#define mask_of_hdmi_tx_reg_pkt0_s1_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s1_0  (0)
#define HDMI_TX_REG_PKT0_S1_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1_0)
#define REG_HDMI_TX_46_L                (HDMI_TX_REG_PKT0_S1_0)
// h0047, bit: 15
/* pkt0 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt0_s1_1 (0x8e)
#define mask_of_hdmi_tx_reg_pkt0_s1_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s1_1  (0)
#define HDMI_TX_REG_PKT0_S1_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1_1)
#define REG_HDMI_TX_47_L                (HDMI_TX_REG_PKT0_S1_1)
// h0048, bit: 15
/* pkt0 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt0_s1_2 (0x90)
#define mask_of_hdmi_tx_reg_pkt0_s1_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s1_2  (0)
#define HDMI_TX_REG_PKT0_S1_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1_2)
#define REG_HDMI_TX_48_L                (HDMI_TX_REG_PKT0_S1_2)
// h0049, bit: 7
/* pkt0 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt0_s1_3 (0x92)
#define mask_of_hdmi_tx_reg_pkt0_s1_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt0_s1_3  (0)
#define HDMI_TX_REG_PKT0_S1_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1_3)
#define REG_HDMI_TX_49_L                (HDMI_TX_REG_PKT0_S1_3)
// h0049, bit: 15
/* pkt0 subpacket1 ecc*/
#define offset_of_hdmi_tx_reg_pkt0_s1_ecc (0x92)
#define mask_of_hdmi_tx_reg_pkt0_s1_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt0_s1_ecc  (8)
#define HDMI_TX_REG_PKT0_S1_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s1_ecc)
// bigger than 16 bit case allh004a
#define offset_of_hdmi_tx_reg_pkt0_s2 (0x94)
#define mask_of_hdmi_tx_reg_pkt0_s2   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt0_s2  (0)
#define HDMI_TX_REG_PKT0_S2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2)
// h004a, bit: 15
/* pkt0 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt0_s2_0 (0x94)
#define mask_of_hdmi_tx_reg_pkt0_s2_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s2_0  (0)
#define HDMI_TX_REG_PKT0_S2_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2_0)
#define REG_HDMI_TX_4A_L                (HDMI_TX_REG_PKT0_S2_0)
// h004b, bit: 15
/* pkt0 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt0_s2_1 (0x96)
#define mask_of_hdmi_tx_reg_pkt0_s2_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s2_1  (0)
#define HDMI_TX_REG_PKT0_S2_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2_1)
#define REG_HDMI_TX_4B_L                (HDMI_TX_REG_PKT0_S2_1)
// h004c, bit: 15
/* pkt0 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt0_s2_2 (0x98)
#define mask_of_hdmi_tx_reg_pkt0_s2_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s2_2  (0)
#define HDMI_TX_REG_PKT0_S2_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2_2)
#define REG_HDMI_TX_4C_L                (HDMI_TX_REG_PKT0_S2_2)
// h004d, bit: 7
/* pkt0 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt0_s2_3 (0x9a)
#define mask_of_hdmi_tx_reg_pkt0_s2_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt0_s2_3  (0)
#define HDMI_TX_REG_PKT0_S2_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2_3)
#define REG_HDMI_TX_4D_L                (HDMI_TX_REG_PKT0_S2_3)
// h004d, bit: 15
/* pkt0 subpacket2 ecc*/
#define offset_of_hdmi_tx_reg_pkt0_s2_ecc (0x9a)
#define mask_of_hdmi_tx_reg_pkt0_s2_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt0_s2_ecc  (8)
#define HDMI_TX_REG_PKT0_S2_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s2_ecc)
// bigger than 16 bit case allh004e
#define offset_of_hdmi_tx_reg_pkt0_s3 (0x9c)
#define mask_of_hdmi_tx_reg_pkt0_s3   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt0_s3  (0)
#define HDMI_TX_REG_PKT0_S3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3)
// h004e, bit: 15
/* pkt0 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt0_s3_0 (0x9c)
#define mask_of_hdmi_tx_reg_pkt0_s3_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s3_0  (0)
#define HDMI_TX_REG_PKT0_S3_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3_0)
#define REG_HDMI_TX_4E_L                (HDMI_TX_REG_PKT0_S3_0)
// h004f, bit: 15
/* pkt0 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt0_s3_1 (0x9e)
#define mask_of_hdmi_tx_reg_pkt0_s3_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s3_1  (0)
#define HDMI_TX_REG_PKT0_S3_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3_1)
#define REG_HDMI_TX_4F_L                (HDMI_TX_REG_PKT0_S3_1)
// h0050, bit: 15
/* pkt0 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt0_s3_2 (0xa0)
#define mask_of_hdmi_tx_reg_pkt0_s3_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt0_s3_2  (0)
#define HDMI_TX_REG_PKT0_S3_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3_2)
#define REG_HDMI_TX_50_L                (HDMI_TX_REG_PKT0_S3_2)
// h0051, bit: 7
/* pkt0 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt0_s3_3 (0xa2)
#define mask_of_hdmi_tx_reg_pkt0_s3_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt0_s3_3  (0)
#define HDMI_TX_REG_PKT0_S3_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3_3)
#define REG_HDMI_TX_51_L                (HDMI_TX_REG_PKT0_S3_3)
// h0051, bit: 15
/* pkt0 subpacket3 ecc*/
#define offset_of_hdmi_tx_reg_pkt0_s3_ecc (0xa2)
#define mask_of_hdmi_tx_reg_pkt0_s3_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt0_s3_ecc  (8)
#define HDMI_TX_REG_PKT0_S3_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt0_s3_ecc)
// bigger than 16 bit case allh0052
#define offset_of_hdmi_tx_reg_pkt1_hd (0xa4)
#define mask_of_hdmi_tx_reg_pkt1_hd   (0xffffff)
#define shift_of_hdmi_tx_reg_pkt1_hd  (0)
#define HDMI_TX_REG_PKT1_HD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_hd)
// h0052, bit: 15
/* pkt1 head*/
#define offset_of_hdmi_tx_reg_pkt1_hd_0 (0xa4)
#define mask_of_hdmi_tx_reg_pkt1_hd_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_hd_0  (0)
#define HDMI_TX_REG_PKT1_HD_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_hd_0)
#define REG_HDMI_TX_52_L                (HDMI_TX_REG_PKT1_HD_0)
// h0053, bit: 7
/* pkt1 head*/
#define offset_of_hdmi_tx_reg_pkt1_hd_1 (0xa6)
#define mask_of_hdmi_tx_reg_pkt1_hd_1   (0xff)
#define shift_of_hdmi_tx_reg_pkt1_hd_1  (0)
#define HDMI_TX_REG_PKT1_HD_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_hd_1)
#define REG_HDMI_TX_53_L                (HDMI_TX_REG_PKT1_HD_1)
// h0053, bit: 15
/* pkt1 head ecc*/
#define offset_of_hdmi_tx_reg_pkt1_hd_ecc (0xa6)
#define mask_of_hdmi_tx_reg_pkt1_hd_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt1_hd_ecc  (8)
#define HDMI_TX_REG_PKT1_HD_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_hd_ecc)
// bigger than 16 bit case allh0054
#define offset_of_hdmi_tx_reg_pkt1_s0 (0xa8)
#define mask_of_hdmi_tx_reg_pkt1_s0   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt1_s0  (0)
#define HDMI_TX_REG_PKT1_S0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0)
// h0054, bit: 15
/* pkt1 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt1_s0_0 (0xa8)
#define mask_of_hdmi_tx_reg_pkt1_s0_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s0_0  (0)
#define HDMI_TX_REG_PKT1_S0_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0_0)
#define REG_HDMI_TX_54_L                (HDMI_TX_REG_PKT1_S0_0)
// h0055, bit: 15
/* pkt1 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt1_s0_1 (0xaa)
#define mask_of_hdmi_tx_reg_pkt1_s0_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s0_1  (0)
#define HDMI_TX_REG_PKT1_S0_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0_1)
#define REG_HDMI_TX_55_L                (HDMI_TX_REG_PKT1_S0_1)
// h0056, bit: 15
/* pkt1 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt1_s0_2 (0xac)
#define mask_of_hdmi_tx_reg_pkt1_s0_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s0_2  (0)
#define HDMI_TX_REG_PKT1_S0_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0_2)
#define REG_HDMI_TX_56_L                (HDMI_TX_REG_PKT1_S0_2)
// h0057, bit: 7
/* pkt1 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt1_s0_3 (0xae)
#define mask_of_hdmi_tx_reg_pkt1_s0_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt1_s0_3  (0)
#define HDMI_TX_REG_PKT1_S0_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0_3)
#define REG_HDMI_TX_57_L                (HDMI_TX_REG_PKT1_S0_3)
// h0057, bit: 15
/* pkt1 subpacket0 ecc*/
#define offset_of_hdmi_tx_reg_pkt1_s0_ecc (0xae)
#define mask_of_hdmi_tx_reg_pkt1_s0_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt1_s0_ecc  (8)
#define HDMI_TX_REG_PKT1_S0_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s0_ecc)
// bigger than 16 bit case allh0058
#define offset_of_hdmi_tx_reg_pkt1_s1 (0xb0)
#define mask_of_hdmi_tx_reg_pkt1_s1   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt1_s1  (0)
#define HDMI_TX_REG_PKT1_S1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1)
// h0058, bit: 15
/* pkt1 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt1_s1_0 (0xb0)
#define mask_of_hdmi_tx_reg_pkt1_s1_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s1_0  (0)
#define HDMI_TX_REG_PKT1_S1_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1_0)
#define REG_HDMI_TX_58_L                (HDMI_TX_REG_PKT1_S1_0)
// h0059, bit: 15
/* pkt1 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt1_s1_1 (0xb2)
#define mask_of_hdmi_tx_reg_pkt1_s1_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s1_1  (0)
#define HDMI_TX_REG_PKT1_S1_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1_1)
#define REG_HDMI_TX_59_L                (HDMI_TX_REG_PKT1_S1_1)
// h005a, bit: 15
/* pkt1 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt1_s1_2 (0xb4)
#define mask_of_hdmi_tx_reg_pkt1_s1_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s1_2  (0)
#define HDMI_TX_REG_PKT1_S1_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1_2)
#define REG_HDMI_TX_5A_L                (HDMI_TX_REG_PKT1_S1_2)
// h005b, bit: 7
/* pkt1 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt1_s1_3 (0xb6)
#define mask_of_hdmi_tx_reg_pkt1_s1_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt1_s1_3  (0)
#define HDMI_TX_REG_PKT1_S1_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1_3)
#define REG_HDMI_TX_5B_L                (HDMI_TX_REG_PKT1_S1_3)
// h005b, bit: 15
/* pkt1 subpacket1 ecc*/
#define offset_of_hdmi_tx_reg_pkt1_s1_ecc (0xb6)
#define mask_of_hdmi_tx_reg_pkt1_s1_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt1_s1_ecc  (8)
#define HDMI_TX_REG_PKT1_S1_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s1_ecc)
// bigger than 16 bit case allh005c
#define offset_of_hdmi_tx_reg_pkt1_s2 (0xb8)
#define mask_of_hdmi_tx_reg_pkt1_s2   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt1_s2  (0)
#define HDMI_TX_REG_PKT1_S2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2)
// h005c, bit: 15
/* pkt1 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt1_s2_0 (0xb8)
#define mask_of_hdmi_tx_reg_pkt1_s2_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s2_0  (0)
#define HDMI_TX_REG_PKT1_S2_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2_0)
#define REG_HDMI_TX_5C_L                (HDMI_TX_REG_PKT1_S2_0)
// h005d, bit: 15
/* pkt1 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt1_s2_1 (0xba)
#define mask_of_hdmi_tx_reg_pkt1_s2_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s2_1  (0)
#define HDMI_TX_REG_PKT1_S2_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2_1)
#define REG_HDMI_TX_5D_L                (HDMI_TX_REG_PKT1_S2_1)
// h005e, bit: 15
/* pkt1 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt1_s2_2 (0xbc)
#define mask_of_hdmi_tx_reg_pkt1_s2_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s2_2  (0)
#define HDMI_TX_REG_PKT1_S2_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2_2)
#define REG_HDMI_TX_5E_L                (HDMI_TX_REG_PKT1_S2_2)
// h005f, bit: 7
/* pkt1 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt1_s2_3 (0xbe)
#define mask_of_hdmi_tx_reg_pkt1_s2_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt1_s2_3  (0)
#define HDMI_TX_REG_PKT1_S2_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2_3)
#define REG_HDMI_TX_5F_L                (HDMI_TX_REG_PKT1_S2_3)
// h005f, bit: 15
/* pkt1 subpacket2 ecc*/
#define offset_of_hdmi_tx_reg_pkt1_s2_ecc (0xbe)
#define mask_of_hdmi_tx_reg_pkt1_s2_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt1_s2_ecc  (8)
#define HDMI_TX_REG_PKT1_S2_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s2_ecc)
// bigger than 16 bit case allh0060
#define offset_of_hdmi_tx_reg_pkt1_s3 (0xc0)
#define mask_of_hdmi_tx_reg_pkt1_s3   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt1_s3  (0)
#define HDMI_TX_REG_PKT1_S3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3)
// h0060, bit: 15
/* pkt1 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt1_s3_0 (0xc0)
#define mask_of_hdmi_tx_reg_pkt1_s3_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s3_0  (0)
#define HDMI_TX_REG_PKT1_S3_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3_0)
#define REG_HDMI_TX_60_L                (HDMI_TX_REG_PKT1_S3_0)
// h0061, bit: 15
/* pkt1 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt1_s3_1 (0xc2)
#define mask_of_hdmi_tx_reg_pkt1_s3_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s3_1  (0)
#define HDMI_TX_REG_PKT1_S3_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3_1)
#define REG_HDMI_TX_61_L                (HDMI_TX_REG_PKT1_S3_1)
// h0062, bit: 15
/* pkt1 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt1_s3_2 (0xc4)
#define mask_of_hdmi_tx_reg_pkt1_s3_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt1_s3_2  (0)
#define HDMI_TX_REG_PKT1_S3_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3_2)
#define REG_HDMI_TX_62_L                (HDMI_TX_REG_PKT1_S3_2)
// h0063, bit: 7
/* pkt1 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt1_s3_3 (0xc6)
#define mask_of_hdmi_tx_reg_pkt1_s3_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt1_s3_3  (0)
#define HDMI_TX_REG_PKT1_S3_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3_3)
#define REG_HDMI_TX_63_L                (HDMI_TX_REG_PKT1_S3_3)
// h0063, bit: 15
/* pkt1 subpacket3 ecc*/
#define offset_of_hdmi_tx_reg_pkt1_s3_ecc (0xc6)
#define mask_of_hdmi_tx_reg_pkt1_s3_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt1_s3_ecc  (8)
#define HDMI_TX_REG_PKT1_S3_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt1_s3_ecc)
// bigger than 16 bit case allh0064
#define offset_of_hdmi_tx_reg_pkt2_hd (0xc8)
#define mask_of_hdmi_tx_reg_pkt2_hd   (0xffffff)
#define shift_of_hdmi_tx_reg_pkt2_hd  (0)
#define HDMI_TX_REG_PKT2_HD           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_hd)
// h0064, bit: 15
/* pkt2 head*/
#define offset_of_hdmi_tx_reg_pkt2_hd_0 (0xc8)
#define mask_of_hdmi_tx_reg_pkt2_hd_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_hd_0  (0)
#define HDMI_TX_REG_PKT2_HD_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_hd_0)
#define REG_HDMI_TX_64_L                (HDMI_TX_REG_PKT2_HD_0)
// h0065, bit: 7
/* pkt2 head*/
#define offset_of_hdmi_tx_reg_pkt2_hd_1 (0xca)
#define mask_of_hdmi_tx_reg_pkt2_hd_1   (0xff)
#define shift_of_hdmi_tx_reg_pkt2_hd_1  (0)
#define HDMI_TX_REG_PKT2_HD_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_hd_1)
#define REG_HDMI_TX_65_L                (HDMI_TX_REG_PKT2_HD_1)
// h0065, bit: 15
/* pkt2 head ecc*/
#define offset_of_hdmi_tx_reg_pkt2_hd_ecc (0xca)
#define mask_of_hdmi_tx_reg_pkt2_hd_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt2_hd_ecc  (8)
#define HDMI_TX_REG_PKT2_HD_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_hd_ecc)
// bigger than 16 bit case allh0066
#define offset_of_hdmi_tx_reg_pkt2_s0 (0xcc)
#define mask_of_hdmi_tx_reg_pkt2_s0   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt2_s0  (0)
#define HDMI_TX_REG_PKT2_S0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0)
// h0066, bit: 15
/* pkt2 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt2_s0_0 (0xcc)
#define mask_of_hdmi_tx_reg_pkt2_s0_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s0_0  (0)
#define HDMI_TX_REG_PKT2_S0_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0_0)
#define REG_HDMI_TX_66_L                (HDMI_TX_REG_PKT2_S0_0)
// h0067, bit: 15
/* pkt2 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt2_s0_1 (0xce)
#define mask_of_hdmi_tx_reg_pkt2_s0_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s0_1  (0)
#define HDMI_TX_REG_PKT2_S0_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0_1)
#define REG_HDMI_TX_67_L                (HDMI_TX_REG_PKT2_S0_1)
// h0068, bit: 15
/* pkt2 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt2_s0_2 (0xd0)
#define mask_of_hdmi_tx_reg_pkt2_s0_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s0_2  (0)
#define HDMI_TX_REG_PKT2_S0_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0_2)
#define REG_HDMI_TX_68_L                (HDMI_TX_REG_PKT2_S0_2)
// h0069, bit: 7
/* pkt2 subpacket0*/
#define offset_of_hdmi_tx_reg_pkt2_s0_3 (0xd2)
#define mask_of_hdmi_tx_reg_pkt2_s0_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt2_s0_3  (0)
#define HDMI_TX_REG_PKT2_S0_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0_3)
#define REG_HDMI_TX_69_L                (HDMI_TX_REG_PKT2_S0_3)
// h0069, bit: 15
/* pkt2 subpacket0 ecc*/
#define offset_of_hdmi_tx_reg_pkt2_s0_ecc (0xd2)
#define mask_of_hdmi_tx_reg_pkt2_s0_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt2_s0_ecc  (8)
#define HDMI_TX_REG_PKT2_S0_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s0_ecc)
// bigger than 16 bit case allh006a
#define offset_of_hdmi_tx_reg_pkt2_s1 (0xd4)
#define mask_of_hdmi_tx_reg_pkt2_s1   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt2_s1  (0)
#define HDMI_TX_REG_PKT2_S1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1)
// h006a, bit: 15
/* pkt2 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt2_s1_0 (0xd4)
#define mask_of_hdmi_tx_reg_pkt2_s1_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s1_0  (0)
#define HDMI_TX_REG_PKT2_S1_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1_0)
#define REG_HDMI_TX_6A_L                (HDMI_TX_REG_PKT2_S1_0)
// h006b, bit: 15
/* pkt2 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt2_s1_1 (0xd6)
#define mask_of_hdmi_tx_reg_pkt2_s1_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s1_1  (0)
#define HDMI_TX_REG_PKT2_S1_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1_1)
#define REG_HDMI_TX_6B_L                (HDMI_TX_REG_PKT2_S1_1)
// h006c, bit: 15
/* pkt2 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt2_s1_2 (0xd8)
#define mask_of_hdmi_tx_reg_pkt2_s1_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s1_2  (0)
#define HDMI_TX_REG_PKT2_S1_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1_2)
#define REG_HDMI_TX_6C_L                (HDMI_TX_REG_PKT2_S1_2)
// h006d, bit: 7
/* pkt2 subpacket1*/
#define offset_of_hdmi_tx_reg_pkt2_s1_3 (0xda)
#define mask_of_hdmi_tx_reg_pkt2_s1_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt2_s1_3  (0)
#define HDMI_TX_REG_PKT2_S1_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1_3)
#define REG_HDMI_TX_6D_L                (HDMI_TX_REG_PKT2_S1_3)
// h006d, bit: 15
/* pkt2 subpacket1 ecc*/
#define offset_of_hdmi_tx_reg_pkt2_s1_ecc (0xda)
#define mask_of_hdmi_tx_reg_pkt2_s1_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt2_s1_ecc  (8)
#define HDMI_TX_REG_PKT2_S1_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s1_ecc)
// bigger than 16 bit case allh006e
#define offset_of_hdmi_tx_reg_pkt2_s2 (0xdc)
#define mask_of_hdmi_tx_reg_pkt2_s2   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt2_s2  (0)
#define HDMI_TX_REG_PKT2_S2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2)
// h006e, bit: 15
/* pkt2 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt2_s2_0 (0xdc)
#define mask_of_hdmi_tx_reg_pkt2_s2_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s2_0  (0)
#define HDMI_TX_REG_PKT2_S2_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2_0)
#define REG_HDMI_TX_6E_L                (HDMI_TX_REG_PKT2_S2_0)
// h006f, bit: 15
/* pkt2 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt2_s2_1 (0xde)
#define mask_of_hdmi_tx_reg_pkt2_s2_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s2_1  (0)
#define HDMI_TX_REG_PKT2_S2_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2_1)
#define REG_HDMI_TX_6F_L                (HDMI_TX_REG_PKT2_S2_1)
// h0070, bit: 15
/* pkt2 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt2_s2_2 (0xe0)
#define mask_of_hdmi_tx_reg_pkt2_s2_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s2_2  (0)
#define HDMI_TX_REG_PKT2_S2_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2_2)
#define REG_HDMI_TX_70_L                (HDMI_TX_REG_PKT2_S2_2)
// h0071, bit: 7
/* pkt2 subpacket2*/
#define offset_of_hdmi_tx_reg_pkt2_s2_3 (0xe2)
#define mask_of_hdmi_tx_reg_pkt2_s2_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt2_s2_3  (0)
#define HDMI_TX_REG_PKT2_S2_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2_3)
#define REG_HDMI_TX_71_L                (HDMI_TX_REG_PKT2_S2_3)
// h0071, bit: 15
/* pkt2 subpacket2 ecc*/
#define offset_of_hdmi_tx_reg_pkt2_s2_ecc (0xe2)
#define mask_of_hdmi_tx_reg_pkt2_s2_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt2_s2_ecc  (8)
#define HDMI_TX_REG_PKT2_S2_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s2_ecc)
// bigger than 16 bit case allh0072
#define offset_of_hdmi_tx_reg_pkt2_s3 (0xe4)
#define mask_of_hdmi_tx_reg_pkt2_s3   (0xffffffff)
#define shift_of_hdmi_tx_reg_pkt2_s3  (0)
#define HDMI_TX_REG_PKT2_S3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3)
// h0072, bit: 15
/* pkt2 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt2_s3_0 (0xe4)
#define mask_of_hdmi_tx_reg_pkt2_s3_0   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s3_0  (0)
#define HDMI_TX_REG_PKT2_S3_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3_0)
#define REG_HDMI_TX_72_L                (HDMI_TX_REG_PKT2_S3_0)
// h0073, bit: 15
/* pkt2 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt2_s3_1 (0xe6)
#define mask_of_hdmi_tx_reg_pkt2_s3_1   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s3_1  (0)
#define HDMI_TX_REG_PKT2_S3_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3_1)
#define REG_HDMI_TX_73_L                (HDMI_TX_REG_PKT2_S3_1)
// h0074, bit: 15
/* pkt2 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt2_s3_2 (0xe8)
#define mask_of_hdmi_tx_reg_pkt2_s3_2   (0xffff)
#define shift_of_hdmi_tx_reg_pkt2_s3_2  (0)
#define HDMI_TX_REG_PKT2_S3_2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3_2)
#define REG_HDMI_TX_74_L                (HDMI_TX_REG_PKT2_S3_2)
// h0075, bit: 7
/* pkt2 subpacket3*/
#define offset_of_hdmi_tx_reg_pkt2_s3_3 (0xea)
#define mask_of_hdmi_tx_reg_pkt2_s3_3   (0xff)
#define shift_of_hdmi_tx_reg_pkt2_s3_3  (0)
#define HDMI_TX_REG_PKT2_S3_3           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3_3)
#define REG_HDMI_TX_75_L                (HDMI_TX_REG_PKT2_S3_3)
// h0075, bit: 15
/* pkt2 subpacket3 ecc*/
#define offset_of_hdmi_tx_reg_pkt2_s3_ecc (0xea)
#define mask_of_hdmi_tx_reg_pkt2_s3_ecc   (0xff00)
#define shift_of_hdmi_tx_reg_pkt2_s3_ecc  (8)
#define HDMI_TX_REG_PKT2_S3_ECC           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_pkt2_s3_ecc)
// h0076, bit: 15
/* interrupt mask*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_mask (0xec)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_mask   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_mask  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_MASK           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_mask)
#define REG_HDMI_TX_76_L                       (HDMI_TX_REG_HDMI_TX_IRQ_MASK)
// h0077, bit: 15
/* interrupt sw force*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_force (0xee)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_force   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_force  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_FORCE           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_force)
#define REG_HDMI_TX_77_L                        (HDMI_TX_REG_HDMI_TX_IRQ_FORCE)
// h0078, bit: 15
/* write 1 to clear interrupt*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_clr (0xf0)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_clr   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_clr  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_CLR           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_clr)
#define REG_HDMI_TX_78_L                      (HDMI_TX_REG_HDMI_TX_IRQ_CLR)
// h0079, bit: 0
/* raw status select, 0: IP source, 1: F.F.*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_select (0xf2)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_select   (0x1)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_select  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_SELECT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_select)
#define REG_HDMI_TX_79_L                         (HDMI_TX_REG_HDMI_TX_IRQ_SELECT)
// h007a, bit: 15
/* irq final status*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_final_status (0xf4)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_final_status   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_final_status  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_FINAL_STATUS           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_final_status)
#define REG_HDMI_TX_7A_L                               (HDMI_TX_REG_HDMI_TX_IRQ_FINAL_STATUS)
// h007b, bit: 15
/* irq raw status*/
#define offset_of_hdmi_tx_reg_hdmi_tx_irq_raw_status (0xf6)
#define mask_of_hdmi_tx_reg_hdmi_tx_irq_raw_status   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_irq_raw_status  (0)
#define HDMI_TX_REG_HDMI_TX_IRQ_RAW_STATUS           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_irq_raw_status)
#define REG_HDMI_TX_7B_L                             (HDMI_TX_REG_HDMI_TX_IRQ_RAW_STATUS)
// h007c, bit: 7
/* debug out sel*/
#define offset_of_hdmi_tx_reg_hdmi_tx_debug_out_sel (0xf8)
#define mask_of_hdmi_tx_reg_hdmi_tx_debug_out_sel   (0xff)
#define shift_of_hdmi_tx_reg_hdmi_tx_debug_out_sel  (0)
#define HDMI_TX_REG_HDMI_TX_DEBUG_OUT_SEL           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_debug_out_sel)
#define REG_HDMI_TX_7C_L                            (HDMI_TX_REG_HDMI_TX_DEBUG_OUT_SEL)
// h007c, bit: 15
/* fixed data while mute*/
#define offset_of_hdmi_tx_reg_fix_data_ch0 (0xf8)
#define mask_of_hdmi_tx_reg_fix_data_ch0   (0xff00)
#define shift_of_hdmi_tx_reg_fix_data_ch0  (8)
#define HDMI_TX_REG_FIX_DATA_CH0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_fix_data_ch0)
// bigger than 16 bit case allh007d
#define offset_of_hdmi_tx_reg_hdmi_tx_debug_out (0xfa)
#define mask_of_hdmi_tx_reg_hdmi_tx_debug_out   (0xffffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_debug_out  (0)
#define HDMI_TX_REG_HDMI_TX_DEBUG_OUT           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_debug_out)
// h007d, bit: 15
/* read debug out*/
#define offset_of_hdmi_tx_reg_hdmi_tx_debug_out_0 (0xfa)
#define mask_of_hdmi_tx_reg_hdmi_tx_debug_out_0   (0xffff)
#define shift_of_hdmi_tx_reg_hdmi_tx_debug_out_0  (0)
#define HDMI_TX_REG_HDMI_TX_DEBUG_OUT_0           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_debug_out_0)
#define REG_HDMI_TX_7D_L                          (HDMI_TX_REG_HDMI_TX_DEBUG_OUT_0)
// h007e, bit: 7
/* read debug out*/
#define offset_of_hdmi_tx_reg_hdmi_tx_debug_out_1 (0xfc)
#define mask_of_hdmi_tx_reg_hdmi_tx_debug_out_1   (0xff)
#define shift_of_hdmi_tx_reg_hdmi_tx_debug_out_1  (0)
#define HDMI_TX_REG_HDMI_TX_DEBUG_OUT_1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_hdmi_tx_debug_out_1)
#define REG_HDMI_TX_7E_L                          (HDMI_TX_REG_HDMI_TX_DEBUG_OUT_1)
// h007f, bit: 7
/* fixed data while mute*/
#define offset_of_hdmi_tx_reg_fix_data_ch1 (0xfe)
#define mask_of_hdmi_tx_reg_fix_data_ch1   (0xff)
#define shift_of_hdmi_tx_reg_fix_data_ch1  (0)
#define HDMI_TX_REG_FIX_DATA_CH1           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_fix_data_ch1)
#define REG_HDMI_TX_7F_L                   (HDMI_TX_REG_FIX_DATA_CH1)
// h007f, bit: 15
/* fixed data while mute*/
#define offset_of_hdmi_tx_reg_fix_data_ch2 (0xfe)
#define mask_of_hdmi_tx_reg_fix_data_ch2   (0xff00)
#define shift_of_hdmi_tx_reg_fix_data_ch2  (8)
#define HDMI_TX_REG_FIX_DATA_CH2           (REG_HDMI_TX_BASE + offset_of_hdmi_tx_reg_fix_data_ch2)
// h0000, bit: 3
/* clk_hdmi_tx_atop_p clock setting
[0]: disable clock
[1]: invert clock
[3:2]: no select mode*/
#define offset_of_hdmi_tx_atop_reg_ckg_hdmi_tx_atop (0x0)
#define mask_of_hdmi_tx_atop_reg_ckg_hdmi_tx_atop   (0xf)
#define shift_of_hdmi_tx_atop_reg_ckg_hdmi_tx_atop  (0)
#define HDMI_TX_ATOP_REG_CKG_HDMI_TX_ATOP           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_ckg_hdmi_tx_atop)
#define REG_HDMI_TX_ATOP_00_L                       (HDMI_TX_ATOP_REG_CKG_HDMI_TX_ATOP)
// h0001, bit: 2
/* write 1 to reset
[0]: fpll scal synth sw rst
[1]: hdmi_tx_atop dig sw rst
[2]: hdmi_tx_atop riu sw rst*/
#define offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_sw_rst (0x2)
#define mask_of_hdmi_tx_atop_reg_hdmi_tx_atop_sw_rst   (0x7)
#define shift_of_hdmi_tx_atop_reg_hdmi_tx_atop_sw_rst  (0)
#define HDMI_TX_ATOP_REG_HDMI_TX_ATOP_SW_RST           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_sw_rst)
#define REG_HDMI_TX_ATOP_01_L                          (HDMI_TX_ATOP_REG_HDMI_TX_ATOP_SW_RST)
// h0002, bit: 7
/* test of synthesizer*/
#define offset_of_hdmi_tx_atop_reg_hdmi_syn_test (0x4)
#define mask_of_hdmi_tx_atop_reg_hdmi_syn_test   (0xff)
#define shift_of_hdmi_tx_atop_reg_hdmi_syn_test  (0)
#define HDMI_TX_ATOP_REG_HDMI_SYN_TEST           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_syn_test)
#define REG_HDMI_TX_ATOP_02_L                    (HDMI_TX_ATOP_REG_HDMI_SYN_TEST)
// bigger than 16 bit case allh0003
#define offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd (0x6)
#define mask_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd   (0xffffff)
#define shift_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd  (0)
#define HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd)
// h0003, bit: 15
/* read set value of FPLL (hdmi synthesizer)*/
#define offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_0 (0x6)
#define mask_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_0  (0)
#define HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_0)
#define REG_HDMI_TX_ATOP_03_L                         (HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD_0)
// h0004, bit: 7
/* read set value of FPLL (hdmi synthesizer)*/
#define offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_1 (0x8)
#define mask_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_1   (0xff)
#define shift_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_1  (0)
#define HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_fpll_set_rd_1)
#define REG_HDMI_TX_ATOP_04_L                         (HDMI_TX_ATOP_REG_HDMI_FPLL_SET_RD_1)
// h0005, bit: 10
/* step of ssc*/
#define offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_step (0xa)
#define mask_of_hdmi_tx_atop_reg_hdmi_syn_ssc_step   (0x7ff)
#define shift_of_hdmi_tx_atop_reg_hdmi_syn_ssc_step  (0)
#define HDMI_TX_ATOP_REG_HDMI_SYN_SSC_STEP           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_step)
#define REG_HDMI_TX_ATOP_05_L                        (HDMI_TX_ATOP_REG_HDMI_SYN_SSC_STEP)
// h0006, bit: 14
/* span of ssc*/
#define offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_span (0xc)
#define mask_of_hdmi_tx_atop_reg_hdmi_syn_ssc_span   (0x7fff)
#define shift_of_hdmi_tx_atop_reg_hdmi_syn_ssc_span  (0)
#define HDMI_TX_ATOP_REG_HDMI_SYN_SSC_SPAN           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_span)
#define REG_HDMI_TX_ATOP_06_L                        (HDMI_TX_ATOP_REG_HDMI_SYN_SSC_SPAN)
// h0007, bit: 0
/* SYNC of ssc*/
#define offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_sync (0xe)
#define mask_of_hdmi_tx_atop_reg_hdmi_syn_ssc_sync   (0x1)
#define shift_of_hdmi_tx_atop_reg_hdmi_syn_ssc_sync  (0)
#define HDMI_TX_ATOP_REG_HDMI_SYN_SSC_SYNC           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_sync)
#define REG_HDMI_TX_ATOP_07_L                        (HDMI_TX_ATOP_REG_HDMI_SYN_SSC_SYNC)
// h0007, bit: 1
/* select mode of ssc*/
#define offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_mode (0xe)
#define mask_of_hdmi_tx_atop_reg_hdmi_syn_ssc_mode   (0x2)
#define shift_of_hdmi_tx_atop_reg_hdmi_syn_ssc_mode  (1)
#define HDMI_TX_ATOP_REG_HDMI_SYN_SSC_MODE           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_syn_ssc_mode)
// h0008, bit: 3
/* select hdmitx_atop testbus out*/
#define offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_testbus_sel (0x10)
#define mask_of_hdmi_tx_atop_reg_hdmi_tx_atop_testbus_sel   (0xf)
#define shift_of_hdmi_tx_atop_reg_hdmi_tx_atop_testbus_sel  (0)
#define HDMI_TX_ATOP_REG_HDMI_TX_ATOP_TESTBUS_SEL \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_testbus_sel)
#define REG_HDMI_TX_ATOP_08_L (HDMI_TX_ATOP_REG_HDMI_TX_ATOP_TESTBUS_SEL)
// h0009, bit: 15
/* dummy*/
#define offset_of_hdmi_tx_atop_reg_dummy (0x12)
#define mask_of_hdmi_tx_atop_reg_dummy   (0xffff)
#define shift_of_hdmi_tx_atop_reg_dummy  (0)
#define HDMI_TX_ATOP_REG_DUMMY           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_dummy)
#define REG_HDMI_TX_ATOP_09_L            (HDMI_TX_ATOP_REG_DUMMY)
// h000a, bit: 0
/* enable div ana bist out*/
#define offset_of_hdmi_tx_atop_reg_bist_div_en (0x14)
#define mask_of_hdmi_tx_atop_reg_bist_div_en   (0x1)
#define shift_of_hdmi_tx_atop_reg_bist_div_en  (0)
#define HDMI_TX_ATOP_REG_BIST_DIV_EN           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_bist_div_en)
#define REG_HDMI_TX_ATOP_0A_L                  (HDMI_TX_ATOP_REG_BIST_DIV_EN)
// h000b, bit: 1
/* 0: div 2
1: div 4
2: div 8
3: div 16*/
#define offset_of_hdmi_tx_atop_reg_bist_div (0x16)
#define mask_of_hdmi_tx_atop_reg_bist_div   (0x3)
#define shift_of_hdmi_tx_atop_reg_bist_div  (0)
#define HDMI_TX_ATOP_REG_BIST_DIV           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_bist_div)
#define REG_HDMI_TX_ATOP_0B_L               (HDMI_TX_ATOP_REG_BIST_DIV)
#define REG_HDMI_TX_ATOP_0C_L               (REG_HDMI_TX_ATOP_BASE + 0x18)
#define REG_HDMI_TX_ATOP_0D_L               (REG_HDMI_TX_ATOP_BASE + 0x1a)
#define REG_HDMI_TX_ATOP_0E_L               (REG_HDMI_TX_ATOP_BASE + 0x1c)
#define REG_HDMI_TX_ATOP_0F_L               (REG_HDMI_TX_ATOP_BASE + 0x1e)
// h0010, bit: 3
/* analog test inputs enable of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_anatest_en_ch (0x20)
#define mask_of_hdmi_tx_atop_reg_anatest_en_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_anatest_en_ch  (0)
#define HDMI_TX_ATOP_REG_ANATEST_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_anatest_en_ch)
#define REG_HDMI_TX_ATOP_10_L                    (HDMI_TX_ATOP_REG_ANATEST_EN_CH)
// h0010, bit: 7
/* bist function enable signals of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_bist_en_ch (0x20)
#define mask_of_hdmi_tx_atop_reg_bist_en_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_bist_en_ch  (4)
#define HDMI_TX_ATOP_REG_BIST_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_bist_en_ch)
#define REG_HDMI_TX_ATOP_11_L                 (REG_HDMI_TX_ATOP_BASE + 0x22)
#define REG_HDMI_TX_ATOP_12_L                 (REG_HDMI_TX_ATOP_BASE + 0x24)
#define REG_HDMI_TX_ATOP_13_L                 (REG_HDMI_TX_ATOP_BASE + 0x26)
#define REG_HDMI_TX_ATOP_14_L                 (REG_HDMI_TX_ATOP_BASE + 0x28)
#define REG_HDMI_TX_ATOP_15_L                 (REG_HDMI_TX_ATOP_BASE + 0x2a)
// h0016, bit: 3
/* data output enable signal of Tap0 post-driver of HDMI chammels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap0_ch (0x2c)
#define mask_of_hdmi_tx_atop_reg_din_en_pstdrv_tap0_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_din_en_pstdrv_tap0_ch  (0)
#define HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP0_CH \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap0_ch)
#define REG_HDMI_TX_ATOP_16_L (HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP0_CH)
// h0016, bit: 7
/* data output enable signal of Tap1 post-driver of HDMI chammels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap1_ch (0x2c)
#define mask_of_hdmi_tx_atop_reg_din_en_pstdrv_tap1_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_din_en_pstdrv_tap1_ch  (4)
#define HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP1_CH \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap1_ch)
// h0016, bit: 11
/* data output enable signal of Tap2 post-driver of HDMI chammels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap2_ch (0x2c)
#define mask_of_hdmi_tx_atop_reg_din_en_pstdrv_tap2_ch   (0xf00)
#define shift_of_hdmi_tx_atop_reg_din_en_pstdrv_tap2_ch  (8)
#define HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP2_CH \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_en_pstdrv_tap2_ch)
// h0017, bit: 3
/* disconnect function enable signals of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_discdet_en_ch (0x2e)
#define mask_of_hdmi_tx_atop_reg_discdet_en_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_discdet_en_ch  (0)
#define HDMI_TX_ATOP_REG_DISCDET_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_discdet_en_ch)
#define REG_HDMI_TX_ATOP_17_L                    (HDMI_TX_ATOP_REG_DISCDET_EN_CH)
// h0018, bit: 1
/* analog 4 inputs selection bit for ch0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch0 (0x30)
#define mask_of_hdmi_tx_atop_reg_gcr_ain_sel_ch0   (0x3)
#define shift_of_hdmi_tx_atop_reg_gcr_ain_sel_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_AIN_SEL_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch0)
#define REG_HDMI_TX_ATOP_18_L                      (HDMI_TX_ATOP_REG_GCR_AIN_SEL_CH0)
// h0018, bit: 3
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_18_0 (0x30)
#define mask_of_hdmi_tx_atop_reg_reserved_18_0   (0xc)
#define shift_of_hdmi_tx_atop_reg_reserved_18_0  (2)
#define HDMI_TX_ATOP_REG_RESERVED_18_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_18_0)
// h0018, bit: 5
/* analog 4 inputs selection bit for ch1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch1 (0x30)
#define mask_of_hdmi_tx_atop_reg_gcr_ain_sel_ch1   (0x30)
#define shift_of_hdmi_tx_atop_reg_gcr_ain_sel_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_AIN_SEL_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch1)
// h0018, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_18_1 (0x30)
#define mask_of_hdmi_tx_atop_reg_reserved_18_1   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_18_1  (6)
#define HDMI_TX_ATOP_REG_RESERVED_18_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_18_1)
// h0018, bit: 9
/* analog 4 inputs selection bit for ch2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch2 (0x30)
#define mask_of_hdmi_tx_atop_reg_gcr_ain_sel_ch2   (0x300)
#define shift_of_hdmi_tx_atop_reg_gcr_ain_sel_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_AIN_SEL_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch2)
// h0018, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_18_2 (0x30)
#define mask_of_hdmi_tx_atop_reg_reserved_18_2   (0xc00)
#define shift_of_hdmi_tx_atop_reg_reserved_18_2  (10)
#define HDMI_TX_ATOP_REG_RESERVED_18_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_18_2)
// h0018, bit: 13
/* analog 4 inputs selection bit for ch3 (ck)*/
#define offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch3 (0x30)
#define mask_of_hdmi_tx_atop_reg_gcr_ain_sel_ch3   (0x3000)
#define shift_of_hdmi_tx_atop_reg_gcr_ain_sel_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_AIN_SEL_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ain_sel_ch3)
// h0018, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_18_3 (0x30)
#define mask_of_hdmi_tx_atop_reg_reserved_18_3   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_18_3  (14)
#define HDMI_TX_ATOP_REG_RESERVED_18_3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_18_3)
// h0019, bit: 4
/* 10mA current trimming bits of CH0, all four channels can be share same efuse.*/
#define offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch0 (0x32)
#define mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch0   (0x1f)
#define shift_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch0)
#define REG_HDMI_TX_ATOP_19_L                           (HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH0)
// h0019, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_19_0 (0x32)
#define mask_of_hdmi_tx_atop_reg_reserved_19_0   (0xe0)
#define shift_of_hdmi_tx_atop_reg_reserved_19_0  (5)
#define HDMI_TX_ATOP_REG_RESERVED_19_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_19_0)
// h0019, bit: 12
/* 10mA current trimming bits of CH1, all four channels can be share same efuse.*/
#define offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch1 (0x32)
#define mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch1   (0x1f00)
#define shift_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch1  (8)
#define HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch1)
// h0019, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_19_1 (0x32)
#define mask_of_hdmi_tx_atop_reg_reserved_19_1   (0xe000)
#define shift_of_hdmi_tx_atop_reg_reserved_19_1  (13)
#define HDMI_TX_ATOP_REG_RESERVED_19_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_19_1)
// h001a, bit: 4
/* 10mA current trimming bits of CH2, all four channels can be share same efuse.*/
#define offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch2 (0x34)
#define mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch2   (0x1f)
#define shift_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch2  (0)
#define HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch2)
#define REG_HDMI_TX_ATOP_1A_L                           (HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH2)
// h001a, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_1a_0 (0x34)
#define mask_of_hdmi_tx_atop_reg_reserved_1a_0   (0xe0)
#define shift_of_hdmi_tx_atop_reg_reserved_1a_0  (5)
#define HDMI_TX_ATOP_REG_RESERVED_1A_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_1a_0)
// h001a, bit: 12
/* 10mA current trimming bits of CH3, all four channels can be share same efuse.*/
#define offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch3 (0x34)
#define mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch3   (0x1f00)
#define shift_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch3  (8)
#define HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch3)
// h001a, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_1a_1 (0x34)
#define mask_of_hdmi_tx_atop_reg_reserved_1a_1   (0xe000)
#define shift_of_hdmi_tx_atop_reg_reserved_1a_1  (13)
#define HDMI_TX_ATOP_REG_RESERVED_1A_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_1a_1)
// h001b, bit: 0
/* HDMI PLL Test Enable control*/
#define offset_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_test (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_test   (0x1)
#define shift_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_test  (0)
#define HDMI_TX_ATOP_REG_GCR_EN_HDMITXPLL_TEST \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_test)
#define REG_HDMI_TX_ATOP_1B_L (HDMI_TX_ATOP_REG_GCR_EN_HDMITXPLL_TEST)
// h001b, bit: 1
/* HDMI PLL 24MHz crystal input path enable*/
#define offset_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_xtal (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_xtal   (0x2)
#define shift_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_xtal  (1)
#define HDMI_TX_ATOP_REG_GCR_EN_HDMITXPLL_XTAL \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_en_hdmitxpll_xtal)
// h001b, bit: 3
/* HDMITX PLL charge pump bias current selection
2'h0: icp = 4uA (default)
2'h1: icp = 3uA
2'h2: icp = 2uA
2'h3: icp = 1uA*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_cp_sw (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_cp_sw   (0xc)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_cp_sw  (2)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_CP_SW           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_cp_sw)
// h001b, bit: 6
/* HDMITX-LL output clock divider for dac
3'h0: VCO/5
3'h1: VCO/10
3'h2: VCO/20 (default)
3'h3: VCO/40
3'h4: VCO/10
3'h5: VCO/20
3'h6: VCO/40
3'h7: VCO/80*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_dac_div (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_dac_div   (0x70)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_dac_div  (4)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_DAC_DIV \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_dac_div)
// h001b, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_1b_0 (0x36)
#define mask_of_hdmi_tx_atop_reg_reserved_1b_0   (0x80)
#define shift_of_hdmi_tx_atop_reg_reserved_1b_0  (7)
#define HDMI_TX_ATOP_REG_RESERVED_1B_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_1b_0)
// h001b, bit: 10
/* HDMITX PLL charge pump current selection
3'h0: 1*icp/4 (default)
3'h1: 2*icp/4
3'h2: 3*icp/4
3'h3: 4*icp/4
3'h4: 5*icp/4
3'h5: 6*icp/4
3'h6: 7*icp/4
3'h7: 8*icp/4*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_ictrl (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_ictrl   (0x700)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_ictrl  (8)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_ICTRL           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_ictrl)
// h001b, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_1b_1 (0x36)
#define mask_of_hdmi_tx_atop_reg_reserved_1b_1   (0x800)
#define shift_of_hdmi_tx_atop_reg_reserved_1b_1  (11)
#define HDMI_TX_ATOP_REG_RESERVED_1B_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_1b_1)
// h001b, bit: 13
/* HDMITX PLL input clock divider
3'h0: CLKIN/1 (default)
3'h1: CLKIN/2
3'h2: CLKIN/4
3'h3: CLKIN/8*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_input_div (0x36)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_input_div   (0x3000)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_input_div  (12)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_INPUT_DIV \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_input_div)
// h001c, bit: 2
/* HDMITXPLL Loop Divider (1 of 2)
3'h0: VCO/2
3'h1: VCO/4 (default)
3'h2: VCO/8
3'h3: VCO/16
3'h4: VCO/1
3'h5: VCO/2
3'h6: VCO/4
3'h7: VCO/8*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_first (0x38)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_first   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_first  (0)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_LOOP_DIV_FIRST \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_first)
#define REG_HDMI_TX_ATOP_1C_L (HDMI_TX_ATOP_REG_GCR_HDMITXPLL_LOOP_DIV_FIRST)
// h001c, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_1c (0x38)
#define mask_of_hdmi_tx_atop_reg_reserved_1c   (0xf8)
#define shift_of_hdmi_tx_atop_reg_reserved_1c  (3)
#define HDMI_TX_ATOP_REG_RESERVED_1C           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_1c)
// h001c, bit: 15
/* HDMITXPLL Loop Divider (2 of 2)
8'h0: VCO/1
8'h1: VCO/1
8'h2: VCO/2
8'h3: VCO/3
8'h4: VCO/4
8'h5: VCO/5 (default)
8'h6: VCO/6
...*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_second (0x38)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_second   (0xff00)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_second  (8)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_LOOP_DIV_SECOND \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_loop_div_second)
// h001d, bit: 2
/* HDMITXPLL Post Divider:
3'h0: VCO/1 (default)
3'h1: VCO/2
3'h2: VCO/4
3'h3: VCO/8
3'h4: VCO/1
3'h5: VCO/2
3'h6: VCO/4
3'h7: VCO/8*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_output_div (0x3a)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_output_div   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_output_div  (0)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_OUTPUT_DIV \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_output_div)
#define REG_HDMI_TX_ATOP_1D_L (HDMI_TX_ATOP_REG_GCR_HDMITXPLL_OUTPUT_DIV)
// h001d, bit: 3
/* 1'h0: disable PLL reset
1'h1: enable PLL reset*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_porst (0x3a)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_porst   (0x8)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_porst  (3)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_PORST           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_porst)
// h001d, bit: 6
/* HDMITX-LL output clock divider for TMDS:
3'h0: VCO/5
3'h1: VCO/10 (default)
3'h2: VCO/20
3'h3: VCO/40
3'h4: VCO/10
3'h5: VCO/20
3'h6: VCO/40
3'h7: VCO/80*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_tmds_div (0x3a)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitxpll_tmds_div   (0x70)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitxpll_tmds_div  (4)
#define HDMI_TX_ATOP_REG_GCR_HDMITXPLL_TMDS_DIV \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitxpll_tmds_div)
#define REG_HDMI_TX_ATOP_1E_L (REG_HDMI_TX_ATOP_BASE + 0x3c)
#define REG_HDMI_TX_ATOP_1F_L (REG_HDMI_TX_ATOP_BASE + 0x3e)
// h0020, bit: 5
/* TAP0 Post Driver Current Control of HDMI Channel 0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch0 (0x40)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch0   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch0)
#define REG_HDMI_TX_ATOP_20_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH0)
// h0020, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_20_0 (0x40)
#define mask_of_hdmi_tx_atop_reg_reserved_20_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_20_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_20_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_20_0)
// h0020, bit: 13
/* TAP0 Post Driver Current Control of HDMI Channel 1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch1 (0x40)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch1   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch1  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch1)
// h0020, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_20_1 (0x40)
#define mask_of_hdmi_tx_atop_reg_reserved_20_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_20_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_20_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_20_1)
// h0021, bit: 5
/* TAP0 Post Driver Current Control of HDMI Channel 2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch2 (0x42)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch2   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch2  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch2)
#define REG_HDMI_TX_ATOP_21_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH2)
// h0021, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_21_0 (0x42)
#define mask_of_hdmi_tx_atop_reg_reserved_21_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_21_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_21_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_21_0)
// h0021, bit: 13
/* TAP0 Post Driver Current Control of HDMI Channel 3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch3 (0x42)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch3   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch3  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap0_ch3)
// h0021, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_21_1 (0x42)
#define mask_of_hdmi_tx_atop_reg_reserved_21_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_21_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_21_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_21_1)
// h0022, bit: 5
/* TAP1 Post Driver Current Control of HDMI Channel 0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0 (0x44)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0)
#define REG_HDMI_TX_ATOP_22_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH0)
// h0022, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_22_0 (0x44)
#define mask_of_hdmi_tx_atop_reg_reserved_22_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_22_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_22_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_22_0)
// h0022, bit: 13
/* TAP1 Post Driver Current Control of HDMI Channel 1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1 (0x44)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1)
// h0022, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_22_1 (0x44)
#define mask_of_hdmi_tx_atop_reg_reserved_22_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_22_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_22_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_22_1)
// h0023, bit: 5
/* TAP1 Post Driver Current Control of HDMI Channel 2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2 (0x46)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2)
#define REG_HDMI_TX_ATOP_23_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH2)
// h0023, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_23_0 (0x46)
#define mask_of_hdmi_tx_atop_reg_reserved_23_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_23_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_23_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_23_0)
// h0023, bit: 13
/* TAP1 Post Driver Current Control of HDMI Channel 3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3 (0x46)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3)
// h0023, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_23_1 (0x46)
#define mask_of_hdmi_tx_atop_reg_reserved_23_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_23_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_23_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_23_1)
// h0024, bit: 5
/* TAP2 Post Driver Current Control of HDMI Channel 0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0 (0x48)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0)
#define REG_HDMI_TX_ATOP_24_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH0)
// h0024, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_24_0 (0x48)
#define mask_of_hdmi_tx_atop_reg_reserved_24_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_24_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_24_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_24_0)
// h0024, bit: 13
/* TAP2 Post Driver Current Control of HDMI Channel 1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1 (0x48)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1)
// h0024, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_24_1 (0x48)
#define mask_of_hdmi_tx_atop_reg_reserved_24_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_24_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_24_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_24_1)
// h0025, bit: 5
/* TAP2 Post Driver Current Control of HDMI Channel 2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2 (0x4a)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2  (0)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2)
#define REG_HDMI_TX_ATOP_25_L (HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH2)
// h0025, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_25_0 (0x4a)
#define mask_of_hdmi_tx_atop_reg_reserved_25_0   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_25_0  (6)
#define HDMI_TX_ATOP_REG_RESERVED_25_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_25_0)
// h0025, bit: 13
/* TAP2 Post Driver Current Control of HDMI Channel 3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3 (0x4a)
#define mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3  (8)
#define HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3)
// h0025, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_25_1 (0x4a)
#define mask_of_hdmi_tx_atop_reg_reserved_25_1   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_25_1  (14)
#define HDMI_TX_ATOP_REG_RESERVED_25_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_25_1)
// h0026, bit: 2
/* Clock Tree LDO output level selection*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_clktree_outlvl_sel (0x4c)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_clktree_outlvl_sel   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_clktree_outlvl_sel  (0)
#define HDMI_TX_ATOP_REG_GCR_LDO_CLKTREE_OUTLVL_SEL \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_clktree_outlvl_sel)
#define REG_HDMI_TX_ATOP_26_L (HDMI_TX_ATOP_REG_GCR_LDO_CLKTREE_OUTLVL_SEL)
// h0027, bit: 2
/* Serializer LDO output level selection of HDMI channel 0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch0 (0x4e)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch0   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_LDO_MUX_OUTLVL_SEL_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch0)
#define REG_HDMI_TX_ATOP_27_L (HDMI_TX_ATOP_REG_GCR_LDO_MUX_OUTLVL_SEL_CH0)
// h0027, bit: 3
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_27_0 (0x4e)
#define mask_of_hdmi_tx_atop_reg_reserved_27_0   (0x8)
#define shift_of_hdmi_tx_atop_reg_reserved_27_0  (3)
#define HDMI_TX_ATOP_REG_RESERVED_27_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_27_0)
// h0027, bit: 6
/* Serializer LDO output level selection of HDMI channel 1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch1 (0x4e)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch1   (0x70)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_LDO_MUX_OUTLVL_SEL_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch1)
// h0027, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_27_1 (0x4e)
#define mask_of_hdmi_tx_atop_reg_reserved_27_1   (0x80)
#define shift_of_hdmi_tx_atop_reg_reserved_27_1  (7)
#define HDMI_TX_ATOP_REG_RESERVED_27_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_27_1)
// h0027, bit: 10
/* Serializer LDO output level selection of HDMI channel 2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch2 (0x4e)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch2   (0x700)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_LDO_MUX_OUTLVL_SEL_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch2)
// h0027, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_27_2 (0x4e)
#define mask_of_hdmi_tx_atop_reg_reserved_27_2   (0x800)
#define shift_of_hdmi_tx_atop_reg_reserved_27_2  (11)
#define HDMI_TX_ATOP_REG_RESERVED_27_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_27_2)
// h0027, bit: 14
/* Serializer LDO output level selection of HDMI channel 3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch3 (0x4e)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch3   (0x7000)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_LDO_MUX_OUTLVL_SEL_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_outlvl_sel_ch3)
// h0027, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_27_3 (0x4e)
#define mask_of_hdmi_tx_atop_reg_reserved_27_3   (0x8000)
#define shift_of_hdmi_tx_atop_reg_reserved_27_3  (15)
#define HDMI_TX_ATOP_REG_RESERVED_27_3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_27_3)
// h0028, bit: 2
/* Pre-driver LDO output level selection of HDMI channel 0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch0 (0x50)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch0   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_OUTLVL_SEL_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch0)
#define REG_HDMI_TX_ATOP_28_L (HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_OUTLVL_SEL_CH0)
// h0028, bit: 3
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_28_0 (0x50)
#define mask_of_hdmi_tx_atop_reg_reserved_28_0   (0x8)
#define shift_of_hdmi_tx_atop_reg_reserved_28_0  (3)
#define HDMI_TX_ATOP_REG_RESERVED_28_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_28_0)
// h0028, bit: 6
/* Pre-driver LDO output level selection of HDMI channel 1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch1 (0x50)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch1   (0x70)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_OUTLVL_SEL_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch1)
// h0028, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_28_1 (0x50)
#define mask_of_hdmi_tx_atop_reg_reserved_28_1   (0x80)
#define shift_of_hdmi_tx_atop_reg_reserved_28_1  (7)
#define HDMI_TX_ATOP_REG_RESERVED_28_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_28_1)
// h0028, bit: 10
/* Pre-driver LDO output level selection of HDMI channel 2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch2 (0x50)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch2   (0x700)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_OUTLVL_SEL_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch2)
// h0028, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_28_2 (0x50)
#define mask_of_hdmi_tx_atop_reg_reserved_28_2   (0x800)
#define shift_of_hdmi_tx_atop_reg_reserved_28_2  (11)
#define HDMI_TX_ATOP_REG_RESERVED_28_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_28_2)
// h0028, bit: 14
/* Pre-driver LDO output level selection of HDMI channel 3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch3 (0x50)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch3   (0x7000)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_OUTLVL_SEL_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_outlvl_sel_ch3)
// h0028, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_28_3 (0x50)
#define mask_of_hdmi_tx_atop_reg_reserved_28_3   (0x8000)
#define shift_of_hdmi_tx_atop_reg_reserved_28_3  (15)
#define HDMI_TX_ATOP_REG_RESERVED_28_3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_28_3)
#define REG_HDMI_TX_ATOP_29_L                    (REG_HDMI_TX_ATOP_BASE + 0x52)
// h002a, bit: 3
/* HDMI CH0 Negative terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch0 (0x54)
#define mask_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch0   (0xf)
#define shift_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_PADN_SLEWRATE_CTRL_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch0)
#define REG_HDMI_TX_ATOP_2A_L (HDMI_TX_ATOP_REG_GCR_PADN_SLEWRATE_CTRL_CH0)
// h002a, bit: 7
/* HDMI CH1 Negative terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch1 (0x54)
#define mask_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch1   (0xf0)
#define shift_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_PADN_SLEWRATE_CTRL_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch1)
// h002a, bit: 11
/* HDMI CH2 Negative terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch2 (0x54)
#define mask_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch2   (0xf00)
#define shift_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_PADN_SLEWRATE_CTRL_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch2)
// h002a, bit: 15
/* HDMI CH3 Negative terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch3 (0x54)
#define mask_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch3   (0xf000)
#define shift_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_PADN_SLEWRATE_CTRL_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padn_slewrate_ctrl_ch3)
// h002b, bit: 3
/* HDMI CH0 Positive terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch0 (0x56)
#define mask_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch0   (0xf)
#define shift_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_PADP_SLEWRATE_CTRL_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch0)
#define REG_HDMI_TX_ATOP_2B_L (HDMI_TX_ATOP_REG_GCR_PADP_SLEWRATE_CTRL_CH0)
// h002b, bit: 7
/* HDMI CH1 Positive terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch1 (0x56)
#define mask_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch1   (0xf0)
#define shift_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_PADP_SLEWRATE_CTRL_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch1)
// h002b, bit: 11
/* HDMI CH2 Positive terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch2 (0x56)
#define mask_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch2   (0xf00)
#define shift_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_PADP_SLEWRATE_CTRL_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch2)
// h002b, bit: 15
/* HDMI CH3 Positive terminal Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch3 (0x56)
#define mask_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch3   (0xf000)
#define shift_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_PADP_SLEWRATE_CTRL_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_padp_slewrate_ctrl_ch3)
// h002c, bit: 3
/* Clamp function enable of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_vclamp_drv_en_ch (0x58)
#define mask_of_hdmi_tx_atop_reg_vclamp_drv_en_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_vclamp_drv_en_ch  (0)
#define HDMI_TX_ATOP_REG_VCLAMP_DRV_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_vclamp_drv_en_ch)
#define REG_HDMI_TX_ATOP_2C_L                       (HDMI_TX_ATOP_REG_VCLAMP_DRV_EN_CH)
// h002d, bit: 3
/* test clock enable signals*/
#define offset_of_hdmi_tx_atop_reg_testclk_en_ch (0x5a)
#define mask_of_hdmi_tx_atop_reg_testclk_en_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_testclk_en_ch  (0)
#define HDMI_TX_ATOP_REG_TESTCLK_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_testclk_en_ch)
#define REG_HDMI_TX_ATOP_2D_L                    (HDMI_TX_ATOP_REG_TESTCLK_EN_CH)
// h002d, bit: 7
/* SCAN IN function enable signals of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_scanin_en_ch (0x5a)
#define mask_of_hdmi_tx_atop_reg_scanin_en_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_scanin_en_ch  (4)
#define HDMI_TX_ATOP_REG_SCANIN_EN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_scanin_en_ch)
#define REG_HDMI_TX_ATOP_2E_L                   (REG_HDMI_TX_ATOP_BASE + 0x5c)
#define REG_HDMI_TX_ATOP_2F_L                   (REG_HDMI_TX_ATOP_BASE + 0x5e)
// h0030, bit: 3
/* Tap0 post driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap0_ch (0x60)
#define mask_of_hdmi_tx_atop_reg_pd_pstdrv_tap0_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_pd_pstdrv_tap0_ch  (0)
#define HDMI_TX_ATOP_REG_PD_PSTDRV_TAP0_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap0_ch)
#define REG_HDMI_TX_ATOP_30_L                        (HDMI_TX_ATOP_REG_PD_PSTDRV_TAP0_CH)
// h0030, bit: 7
/* Tap1 post driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap1_ch (0x60)
#define mask_of_hdmi_tx_atop_reg_pd_pstdrv_tap1_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_pd_pstdrv_tap1_ch  (4)
#define HDMI_TX_ATOP_REG_PD_PSTDRV_TAP1_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap1_ch)
// h0030, bit: 11
/* Tap2 post driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap2_ch (0x60)
#define mask_of_hdmi_tx_atop_reg_pd_pstdrv_tap2_ch   (0xf00)
#define shift_of_hdmi_tx_atop_reg_pd_pstdrv_tap2_ch  (8)
#define HDMI_TX_ATOP_REG_PD_PSTDRV_TAP2_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_pstdrv_tap2_ch)
// h0031, bit: 3
/* Tap0 pre driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_predrv_tap0_ch (0x62)
#define mask_of_hdmi_tx_atop_reg_pd_predrv_tap0_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_pd_predrv_tap0_ch  (0)
#define HDMI_TX_ATOP_REG_PD_PREDRV_TAP0_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_predrv_tap0_ch)
#define REG_HDMI_TX_ATOP_31_L                        (HDMI_TX_ATOP_REG_PD_PREDRV_TAP0_CH)
// h0031, bit: 7
/* Tap1 pre driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_predrv_tap1_ch (0x62)
#define mask_of_hdmi_tx_atop_reg_pd_predrv_tap1_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_pd_predrv_tap1_ch  (4)
#define HDMI_TX_ATOP_REG_PD_PREDRV_TAP1_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_predrv_tap1_ch)
// h0031, bit: 11
/* Tap2 pre driver Power-down control signal of HDMI Channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_predrv_tap2_ch (0x62)
#define mask_of_hdmi_tx_atop_reg_pd_predrv_tap2_ch   (0xf00)
#define shift_of_hdmi_tx_atop_reg_pd_predrv_tap2_ch  (8)
#define HDMI_TX_ATOP_REG_PD_PREDRV_TAP2_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_predrv_tap2_ch)
// h0032, bit: 3
/* 50 ohm Rterm Disable signal*/
#define offset_of_hdmi_tx_atop_reg_pd_rterm_ch (0x64)
#define mask_of_hdmi_tx_atop_reg_pd_rterm_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_pd_rterm_ch  (0)
#define HDMI_TX_ATOP_REG_PD_RTERM_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_rterm_ch)
#define REG_HDMI_TX_ATOP_32_L                  (HDMI_TX_ATOP_REG_PD_RTERM_CH)
// h0032, bit: 7
/* Pre-driver LDO disable control signal of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch (0x64)
#define mask_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch   (0xf0)
#define shift_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch  (4)
#define HDMI_TX_ATOP_REG_PD_LDO_PREDRV_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch)
// h0032, bit: 11
/* Serializer LDO disable control signal of HDMI channels (CH[3:0] -> CH3:CH0)*/
#define offset_of_hdmi_tx_atop_reg_pd_ldo_mux_ch (0x64)
#define mask_of_hdmi_tx_atop_reg_pd_ldo_mux_ch   (0xf00)
#define shift_of_hdmi_tx_atop_reg_pd_ldo_mux_ch  (8)
#define HDMI_TX_ATOP_REG_PD_LDO_MUX_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_ldo_mux_ch)
// h0033, bit: 0
/* Clock Tree LDO disalbe control signal of HDMI*/
#define offset_of_hdmi_tx_atop_reg_pd_ldo_clktree (0x66)
#define mask_of_hdmi_tx_atop_reg_pd_ldo_clktree   (0x1)
#define shift_of_hdmi_tx_atop_reg_pd_ldo_clktree  (0)
#define HDMI_TX_ATOP_REG_PD_LDO_CLKTREE           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_ldo_clktree)
#define REG_HDMI_TX_ATOP_33_L                     (HDMI_TX_ATOP_REG_PD_LDO_CLKTREE)
// h0034, bit: 0
/* HDMITX PLL power-down down control signal*/
#define offset_of_hdmi_tx_atop_reg_pd_hdmitxpll (0x68)
#define mask_of_hdmi_tx_atop_reg_pd_hdmitxpll   (0x1)
#define shift_of_hdmi_tx_atop_reg_pd_hdmitxpll  (0)
#define HDMI_TX_ATOP_REG_PD_HDMITXPLL           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_hdmitxpll)
#define REG_HDMI_TX_ATOP_34_L                   (HDMI_TX_ATOP_REG_PD_HDMITXPLL)
// h0035, bit: 3
/* Local Biasgen disable control signal of HDMI Channels*/
#define offset_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch (0x6a)
#define mask_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch  (0)
#define HDMI_TX_ATOP_REG_PD_DRV_BIASGEN_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch)
#define REG_HDMI_TX_ATOP_35_L                        (HDMI_TX_ATOP_REG_PD_DRV_BIASGEN_CH)
// h0035, bit: 4
/* Top Biasgen disable control signal*/
#define offset_of_hdmi_tx_atop_reg_pd_biasgen (0x6a)
#define mask_of_hdmi_tx_atop_reg_pd_biasgen   (0x10)
#define shift_of_hdmi_tx_atop_reg_pd_biasgen  (4)
#define HDMI_TX_ATOP_REG_PD_BIASGEN           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_pd_biasgen)
#define REG_HDMI_TX_ATOP_36_L                 (REG_HDMI_TX_ATOP_BASE + 0x6c)
#define REG_HDMI_TX_ATOP_37_L                 (REG_HDMI_TX_ATOP_BASE + 0x6e)
// h0038, bit: 4
/* 50 ohm Rterm trimming bits.
For > 2.25Gsps, GCR_RTERM50_TRIM can be connected to GCR_DRV_IOUT_SEL_CH**/
#define offset_of_hdmi_tx_atop_reg_gcr_rterm50_trim (0x70)
#define mask_of_hdmi_tx_atop_reg_gcr_rterm50_trim   (0x1f)
#define shift_of_hdmi_tx_atop_reg_gcr_rterm50_trim  (0)
#define HDMI_TX_ATOP_REG_GCR_RTERM50_TRIM           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_rterm50_trim)
#define REG_HDMI_TX_ATOP_38_L                       (HDMI_TX_ATOP_REG_GCR_RTERM50_TRIM)
// h0039, bit: 1
/* HDMI CH0 Pre-driver Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch0 (0x72)
#define mask_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch0   (0x3)
#define shift_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch0)
#define REG_HDMI_TX_ATOP_39_L (HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH0)
// h0039, bit: 3
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_39_0 (0x72)
#define mask_of_hdmi_tx_atop_reg_reserved_39_0   (0xc)
#define shift_of_hdmi_tx_atop_reg_reserved_39_0  (2)
#define HDMI_TX_ATOP_REG_RESERVED_39_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_39_0)
// h0039, bit: 5
/* HDMI CH1 Pre-driver Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch1 (0x72)
#define mask_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch1   (0x30)
#define shift_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch1)
// h0039, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_39_1 (0x72)
#define mask_of_hdmi_tx_atop_reg_reserved_39_1   (0xc0)
#define shift_of_hdmi_tx_atop_reg_reserved_39_1  (6)
#define HDMI_TX_ATOP_REG_RESERVED_39_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_39_1)
// h0039, bit: 9
/* HDMI CH2 Pre-driver Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch2 (0x72)
#define mask_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch2   (0x300)
#define shift_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch2)
// h0039, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_39_2 (0x72)
#define mask_of_hdmi_tx_atop_reg_reserved_39_2   (0xc00)
#define shift_of_hdmi_tx_atop_reg_reserved_39_2  (10)
#define HDMI_TX_ATOP_REG_RESERVED_39_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_39_2)
// h0039, bit: 13
/* HDMI CH3 Pre-driver Slew rate control*/
#define offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch3 (0x72)
#define mask_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch3   (0x3000)
#define shift_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_predrv_slewrate_ctrl_ch3)
// h0039, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_39_3 (0x72)
#define mask_of_hdmi_tx_atop_reg_reserved_39_3   (0xc000)
#define shift_of_hdmi_tx_atop_reg_reserved_39_3  (14)
#define HDMI_TX_ATOP_REG_RESERVED_39_3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_39_3)
// h003a, bit: 0
/* hdmitx pll loop lock-detection signal output*/
#define offset_of_hdmi_tx_atop_reg_ro_hdmitxpll_high (0x74)
#define mask_of_hdmi_tx_atop_reg_ro_hdmitxpll_high   (0x1)
#define shift_of_hdmi_tx_atop_reg_ro_hdmitxpll_high  (0)
#define HDMI_TX_ATOP_REG_RO_HDMITXPLL_HIGH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_ro_hdmitxpll_high)
#define REG_HDMI_TX_ATOP_3A_L                        (HDMI_TX_ATOP_REG_RO_HDMITXPLL_HIGH)
// h003a, bit: 1
/* hdmitx pll ring voltage-detection comparator output*/
#define offset_of_hdmi_tx_atop_reg_ro_hdmitxpll_lock (0x74)
#define mask_of_hdmi_tx_atop_reg_ro_hdmitxpll_lock   (0x2)
#define shift_of_hdmi_tx_atop_reg_ro_hdmitxpll_lock  (1)
#define HDMI_TX_ATOP_REG_RO_HDMITXPLL_LOCK           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_ro_hdmitxpll_lock)
// h003b, bit: 2
/* hdmitx ch0 serializer TAP2/TAP1/TAP0/ enable signal*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch0 (0x76)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch0   (0x7)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch0)
#define REG_HDMI_TX_ATOP_3B_L                         (HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH0)
// h003b, bit: 3
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_3b_0 (0x76)
#define mask_of_hdmi_tx_atop_reg_reserved_3b_0   (0x8)
#define shift_of_hdmi_tx_atop_reg_reserved_3b_0  (3)
#define HDMI_TX_ATOP_REG_RESERVED_3B_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_3b_0)
// h003b, bit: 6
/* hdmitx ch1 serializer TAP2/TAP1/TAP0/ enable signal*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch1 (0x76)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch1   (0x70)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch1  (4)
#define HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch1)
// h003b, bit: 7
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_3b_1 (0x76)
#define mask_of_hdmi_tx_atop_reg_reserved_3b_1   (0x80)
#define shift_of_hdmi_tx_atop_reg_reserved_3b_1  (7)
#define HDMI_TX_ATOP_REG_RESERVED_3B_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_3b_1)
// h003b, bit: 10
/* hdmitx ch2 serializer TAP2/TAP1/TAP0/ enable signal*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch2 (0x76)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch2   (0x700)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch2  (8)
#define HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch2)
// h003b, bit: 11
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_3b_2 (0x76)
#define mask_of_hdmi_tx_atop_reg_reserved_3b_2   (0x800)
#define shift_of_hdmi_tx_atop_reg_reserved_3b_2  (11)
#define HDMI_TX_ATOP_REG_RESERVED_3B_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_3b_2)
// h003b, bit: 14
/* hdmitx ch3 serializer TAP2/TAP1/TAP0/ enable signal*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch3 (0x76)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch3   (0x7000)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch3  (12)
#define HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tap_en_ch3)
// h003b, bit: 15
/* reserved*/
#define offset_of_hdmi_tx_atop_reg_reserved_3b_3 (0x76)
#define mask_of_hdmi_tx_atop_reg_reserved_3b_3   (0x8000)
#define shift_of_hdmi_tx_atop_reg_reserved_3b_3  (15)
#define HDMI_TX_ATOP_REG_RESERVED_3B_3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_reserved_3b_3)
// h003c, bit: 0
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_p_ch0 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_p_ch0   (0x1)
#define shift_of_hdmi_tx_atop_reg_disc_flag_p_ch0  (0)
#define HDMI_TX_ATOP_REG_DISC_FLAG_P_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_p_ch0)
#define REG_HDMI_TX_ATOP_3C_L                      (HDMI_TX_ATOP_REG_DISC_FLAG_P_CH0)
// h003c, bit: 1
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_n_ch0 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_n_ch0   (0x2)
#define shift_of_hdmi_tx_atop_reg_disc_flag_n_ch0  (1)
#define HDMI_TX_ATOP_REG_DISC_FLAG_N_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_n_ch0)
// h003c, bit: 2
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_p_ch1 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_p_ch1   (0x4)
#define shift_of_hdmi_tx_atop_reg_disc_flag_p_ch1  (2)
#define HDMI_TX_ATOP_REG_DISC_FLAG_P_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_p_ch1)
// h003c, bit: 3
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_n_ch1 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_n_ch1   (0x8)
#define shift_of_hdmi_tx_atop_reg_disc_flag_n_ch1  (3)
#define HDMI_TX_ATOP_REG_DISC_FLAG_N_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_n_ch1)
// h003c, bit: 4
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_p_ch2 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_p_ch2   (0x10)
#define shift_of_hdmi_tx_atop_reg_disc_flag_p_ch2  (4)
#define HDMI_TX_ATOP_REG_DISC_FLAG_P_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_p_ch2)
// h003c, bit: 5
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_n_ch2 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_n_ch2   (0x20)
#define shift_of_hdmi_tx_atop_reg_disc_flag_n_ch2  (5)
#define HDMI_TX_ATOP_REG_DISC_FLAG_N_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_n_ch2)
// h003c, bit: 6
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_p_ch3 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_p_ch3   (0x40)
#define shift_of_hdmi_tx_atop_reg_disc_flag_p_ch3  (6)
#define HDMI_TX_ATOP_REG_DISC_FLAG_P_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_p_ch3)
// h003c, bit: 7
/* disconnect flag*/
#define offset_of_hdmi_tx_atop_reg_disc_flag_n_ch3 (0x78)
#define mask_of_hdmi_tx_atop_reg_disc_flag_n_ch3   (0x80)
#define shift_of_hdmi_tx_atop_reg_disc_flag_n_ch3  (7)
#define HDMI_TX_ATOP_REG_DISC_FLAG_N_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_disc_flag_n_ch3)
// bigger than 16 bit case allh003d
#define offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll (0x7a)
#define mask_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll   (0xffffffff)
#define shift_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll  (0)
#define HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll)
// h003d, bit: 15
/* HDMITXPLL Test control bits  note: 32 -> 48bits in I7*/
#define offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_0 (0x7a)
#define mask_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_0  (0)
#define HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_0)
#define REG_HDMI_TX_ATOP_3D_L                           (HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_0)
// h003e, bit: 15
/* HDMITXPLL Test control bits  note: 32 -> 48bits in I7*/
#define offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_1 (0x7c)
#define mask_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_1   (0xffff)
#define shift_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_1  (0)
#define HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_1)
#define REG_HDMI_TX_ATOP_3E_L                           (HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_1)
// h003f, bit: 15
/* HDMITXPLL Test control bits  note: 32 -> 48bits in I7*/
#define offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_2 (0x7e)
#define mask_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_2   (0xffff)
#define shift_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_2  (0)
#define HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_test_hdmitxpll_2)
#define REG_HDMI_TX_ATOP_3F_L                           (HDMI_TX_ATOP_REG_GCR_TEST_HDMITXPLL_2)
#define REG_HDMI_TX_ATOP_40_L                           (REG_HDMI_TX_ATOP_BASE + 0x80)
#define REG_HDMI_TX_ATOP_41_L                           (REG_HDMI_TX_ATOP_BASE + 0x82)
#define REG_HDMI_TX_ATOP_42_L                           (REG_HDMI_TX_ATOP_BASE + 0x84)
#define REG_HDMI_TX_ATOP_43_L                           (REG_HDMI_TX_ATOP_BASE + 0x86)
#define REG_HDMI_TX_ATOP_44_L                           (REG_HDMI_TX_ATOP_BASE + 0x88)
#define REG_HDMI_TX_ATOP_45_L                           (REG_HDMI_TX_ATOP_BASE + 0x8a)
#define REG_HDMI_TX_ATOP_46_L                           (REG_HDMI_TX_ATOP_BASE + 0x8c)
// bigger than 16 bit case allh0047
#define offset_of_hdmi_tx_atop_reg_din_ch0 (0x8e)
#define mask_of_hdmi_tx_atop_reg_din_ch0   (0xfffff)
#define shift_of_hdmi_tx_atop_reg_din_ch0  (0)
#define HDMI_TX_ATOP_REG_DIN_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch0)
// h0047, bit: 15
/* data inputs of HDMI ch0*/
#define offset_of_hdmi_tx_atop_reg_din_ch0_0 (0x8e)
#define mask_of_hdmi_tx_atop_reg_din_ch0_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_din_ch0_0  (0)
#define HDMI_TX_ATOP_REG_DIN_CH0_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch0_0)
#define REG_HDMI_TX_ATOP_47_L                (HDMI_TX_ATOP_REG_DIN_CH0_0)
// h0048, bit: 3
/* data inputs of HDMI ch0*/
#define offset_of_hdmi_tx_atop_reg_din_ch0_1 (0x90)
#define mask_of_hdmi_tx_atop_reg_din_ch0_1   (0xf)
#define shift_of_hdmi_tx_atop_reg_din_ch0_1  (0)
#define HDMI_TX_ATOP_REG_DIN_CH0_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch0_1)
#define REG_HDMI_TX_ATOP_48_L                (HDMI_TX_ATOP_REG_DIN_CH0_1)
// bigger than 16 bit case allh0049
#define offset_of_hdmi_tx_atop_reg_din_ch1 (0x92)
#define mask_of_hdmi_tx_atop_reg_din_ch1   (0xfffff)
#define shift_of_hdmi_tx_atop_reg_din_ch1  (0)
#define HDMI_TX_ATOP_REG_DIN_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch1)
// h0049, bit: 15
/* data inputs of HDMI ch1*/
#define offset_of_hdmi_tx_atop_reg_din_ch1_0 (0x92)
#define mask_of_hdmi_tx_atop_reg_din_ch1_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_din_ch1_0  (0)
#define HDMI_TX_ATOP_REG_DIN_CH1_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch1_0)
#define REG_HDMI_TX_ATOP_49_L                (HDMI_TX_ATOP_REG_DIN_CH1_0)
// h004a, bit: 3
/* data inputs of HDMI ch1*/
#define offset_of_hdmi_tx_atop_reg_din_ch1_1 (0x94)
#define mask_of_hdmi_tx_atop_reg_din_ch1_1   (0xf)
#define shift_of_hdmi_tx_atop_reg_din_ch1_1  (0)
#define HDMI_TX_ATOP_REG_DIN_CH1_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch1_1)
#define REG_HDMI_TX_ATOP_4A_L                (HDMI_TX_ATOP_REG_DIN_CH1_1)
// bigger than 16 bit case allh004b
#define offset_of_hdmi_tx_atop_reg_din_ch2 (0x96)
#define mask_of_hdmi_tx_atop_reg_din_ch2   (0xfffff)
#define shift_of_hdmi_tx_atop_reg_din_ch2  (0)
#define HDMI_TX_ATOP_REG_DIN_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch2)
// h004b, bit: 15
/* data inputs of HDMI ch2*/
#define offset_of_hdmi_tx_atop_reg_din_ch2_0 (0x96)
#define mask_of_hdmi_tx_atop_reg_din_ch2_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_din_ch2_0  (0)
#define HDMI_TX_ATOP_REG_DIN_CH2_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch2_0)
#define REG_HDMI_TX_ATOP_4B_L                (HDMI_TX_ATOP_REG_DIN_CH2_0)
// h004c, bit: 3
/* data inputs of HDMI ch2*/
#define offset_of_hdmi_tx_atop_reg_din_ch2_1 (0x98)
#define mask_of_hdmi_tx_atop_reg_din_ch2_1   (0xf)
#define shift_of_hdmi_tx_atop_reg_din_ch2_1  (0)
#define HDMI_TX_ATOP_REG_DIN_CH2_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch2_1)
#define REG_HDMI_TX_ATOP_4C_L                (HDMI_TX_ATOP_REG_DIN_CH2_1)
// bigger than 16 bit case allh004d
#define offset_of_hdmi_tx_atop_reg_din_ch3 (0x9a)
#define mask_of_hdmi_tx_atop_reg_din_ch3   (0xfffff)
#define shift_of_hdmi_tx_atop_reg_din_ch3  (0)
#define HDMI_TX_ATOP_REG_DIN_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch3)
// h004d, bit: 15
/* data inputs of HDMI ch3 (ck)*/
#define offset_of_hdmi_tx_atop_reg_din_ch3_0 (0x9a)
#define mask_of_hdmi_tx_atop_reg_din_ch3_0   (0xffff)
#define shift_of_hdmi_tx_atop_reg_din_ch3_0  (0)
#define HDMI_TX_ATOP_REG_DIN_CH3_0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch3_0)
#define REG_HDMI_TX_ATOP_4D_L                (HDMI_TX_ATOP_REG_DIN_CH3_0)
// h004e, bit: 3
/* data inputs of HDMI ch3 (ck)*/
#define offset_of_hdmi_tx_atop_reg_din_ch3_1 (0x9c)
#define mask_of_hdmi_tx_atop_reg_din_ch3_1   (0xf)
#define shift_of_hdmi_tx_atop_reg_din_ch3_1  (0)
#define HDMI_TX_ATOP_REG_DIN_CH3_1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_din_ch3_1)
#define REG_HDMI_TX_ATOP_4E_L                (HDMI_TX_ATOP_REG_DIN_CH3_1)
// h004f, bit: 0
/* 0: select hdmi tx atop din from hdmi tx dig
1: select hdmi tx atop din from register*/
#define offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_din_sel (0x9e)
#define mask_of_hdmi_tx_atop_reg_hdmi_tx_atop_din_sel   (0x1)
#define shift_of_hdmi_tx_atop_reg_hdmi_tx_atop_din_sel  (0)
#define HDMI_TX_ATOP_REG_HDMI_TX_ATOP_DIN_SEL           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_hdmi_tx_atop_din_sel)
#define REG_HDMI_TX_ATOP_4F_L                           (HDMI_TX_ATOP_REG_HDMI_TX_ATOP_DIN_SEL)
// h0050, bit: 0
/* hdmitx 2.1 gcr_ser_36bit_mode_ch0*/
#define offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch0 (0xa0)
#define mask_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch0   (0x1)
#define shift_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH0 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch0)
#define REG_HDMI_TX_ATOP_50_L (HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH0)
// h0050, bit: 1
/* hdmitx 2.1 gcr_ser_36bit_mode_ch1*/
#define offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch1 (0xa0)
#define mask_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch1   (0x2)
#define shift_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch1  (1)
#define HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH1 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch1)
// h0050, bit: 2
/* hdmitx 2.1 gcr_ser_36bit_mode_ch2*/
#define offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch2 (0xa0)
#define mask_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch2   (0x4)
#define shift_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch2  (2)
#define HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH2 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch2)
// h0050, bit: 3
/* hdmitx 2.1 gcr_ser_36bit_mode_ch3*/
#define offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch3 (0xa0)
#define mask_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch3   (0x8)
#define shift_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch3  (3)
#define HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH3 \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ser_36bit_mode_ch3)
// h0050, bit: 8
/* hdmitx 2.1 gcr_div_en*/
#define offset_of_hdmi_tx_atop_reg_gcr_div_en (0xa0)
#define mask_of_hdmi_tx_atop_reg_gcr_div_en   (0x100)
#define shift_of_hdmi_tx_atop_reg_gcr_div_en  (8)
#define HDMI_TX_ATOP_REG_GCR_DIV_EN           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_div_en)
// h0051, bit: 15
/* hdmitx 2.1 gcr_hdmitx_test*/
#define offset_of_hdmi_tx_atop_reg_gcr_hdmitx_test (0xa2)
#define mask_of_hdmi_tx_atop_reg_gcr_hdmitx_test   (0xffff)
#define shift_of_hdmi_tx_atop_reg_gcr_hdmitx_test  (0)
#define HDMI_TX_ATOP_REG_GCR_HDMITX_TEST           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_hdmitx_test)
#define REG_HDMI_TX_ATOP_51_L                      (HDMI_TX_ATOP_REG_GCR_HDMITX_TEST)
// h0052, bit: 4
/* hdmitx 2.1 gcr_ch0_op_adj*/
#define offset_of_hdmi_tx_atop_reg_gcr_ch0_op_adj (0xa4)
#define mask_of_hdmi_tx_atop_reg_gcr_ch0_op_adj   (0x1f)
#define shift_of_hdmi_tx_atop_reg_gcr_ch0_op_adj  (0)
#define HDMI_TX_ATOP_REG_GCR_CH0_OP_ADJ           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ch0_op_adj)
#define REG_HDMI_TX_ATOP_52_L                     (HDMI_TX_ATOP_REG_GCR_CH0_OP_ADJ)
// h0052, bit: 12
/* hdmitx 2.1 gcr_ch1_op_adj*/
#define offset_of_hdmi_tx_atop_reg_gcr_ch1_op_adj (0xa4)
#define mask_of_hdmi_tx_atop_reg_gcr_ch1_op_adj   (0x1f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ch1_op_adj  (8)
#define HDMI_TX_ATOP_REG_GCR_CH1_OP_ADJ           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ch1_op_adj)
// h0053, bit: 4
/* hdmitx 2.1 gcr_ch2_op_adj*/
#define offset_of_hdmi_tx_atop_reg_gcr_ch2_op_adj (0xa6)
#define mask_of_hdmi_tx_atop_reg_gcr_ch2_op_adj   (0x1f)
#define shift_of_hdmi_tx_atop_reg_gcr_ch2_op_adj  (0)
#define HDMI_TX_ATOP_REG_GCR_CH2_OP_ADJ           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ch2_op_adj)
#define REG_HDMI_TX_ATOP_53_L                     (HDMI_TX_ATOP_REG_GCR_CH2_OP_ADJ)
// h0053, bit: 12
/* hdmitx 2.1 gcr_ch3_op_adj*/
#define offset_of_hdmi_tx_atop_reg_gcr_ch3_op_adj (0xa6)
#define mask_of_hdmi_tx_atop_reg_gcr_ch3_op_adj   (0x1f00)
#define shift_of_hdmi_tx_atop_reg_gcr_ch3_op_adj  (8)
#define HDMI_TX_ATOP_REG_GCR_CH3_OP_ADJ           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ch3_op_adj)
// h0054, bit: 5
/* hdmitx 2.1 reg_gcr_mux_tiel_en_ch0*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch0 (0xa8)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch0   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch0  (0)
#define HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch0)
#define REG_HDMI_TX_ATOP_54_L                          (HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH0)
// h0054, bit: 7
/* hdmitx 2.1 reg_gcr_pw_comp_en_ch0*/
#define offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch0 (0xa8)
#define mask_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch0   (0x80)
#define shift_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch0  (7)
#define HDMI_TX_ATOP_REG_GCR_PW_COMP_EN_CH0           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch0)
// h0054, bit: 13
/* hdmitx 2.1 reg_gcr_mux_tiel_en_ch1*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch1 (0xa8)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch1   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch1  (8)
#define HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch1)
// h0054, bit: 15
/* hdmitx 2.1 reg_gcr_pw_comp_en_ch1*/
#define offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch1 (0xa8)
#define mask_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch1   (0x8000)
#define shift_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch1  (15)
#define HDMI_TX_ATOP_REG_GCR_PW_COMP_EN_CH1           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch1)
// h0055, bit: 5
/* hdmitx 2.1 reg_gcr_mux_tiel_en_ch2*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch2 (0xaa)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch2   (0x3f)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch2  (0)
#define HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch2)
#define REG_HDMI_TX_ATOP_55_L                          (HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH2)
// h0055, bit: 7
/* hdmitx 2.1 reg_gcr_pw_comp_en_ch2*/
#define offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch2 (0xaa)
#define mask_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch2   (0x80)
#define shift_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch2  (7)
#define HDMI_TX_ATOP_REG_GCR_PW_COMP_EN_CH2           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch2)
// h0055, bit: 13
/* hdmitx 2.1 reg_gcr_mux_tiel_en_ch3*/
#define offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch3 (0xaa)
#define mask_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch3   (0x3f00)
#define shift_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch3  (8)
#define HDMI_TX_ATOP_REG_GCR_MUX_TIEL_EN_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_mux_tiel_en_ch3)
// h0055, bit: 15
/* hdmitx 2.1 reg_gcr_pw_comp_en_ch3*/
#define offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch3 (0xaa)
#define mask_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch3   (0x8000)
#define shift_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch3  (15)
#define HDMI_TX_ATOP_REG_GCR_PW_COMP_EN_CH3           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_pw_comp_en_ch3)
// h0056, bit: 1
/* hdmitx 2.1 reg_gcr_dclk_phase*/
#define offset_of_hdmi_tx_atop_reg_gcr_dclk_phase (0xac)
#define mask_of_hdmi_tx_atop_reg_gcr_dclk_phase   (0x3)
#define shift_of_hdmi_tx_atop_reg_gcr_dclk_phase  (0)
#define HDMI_TX_ATOP_REG_GCR_DCLK_PHASE           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_dclk_phase)
#define REG_HDMI_TX_ATOP_56_L                     (HDMI_TX_ATOP_REG_GCR_DCLK_PHASE)
// h0056, bit: 9
/* reg_gcr_ain_sel*/
#define offset_of_hdmi_tx_atop_reg_gcr_ain_sel (0xac)
#define mask_of_hdmi_tx_atop_reg_gcr_ain_sel   (0x300)
#define shift_of_hdmi_tx_atop_reg_gcr_ain_sel  (8)
#define HDMI_TX_ATOP_REG_GCR_AIN_SEL           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ain_sel)
// h0057, bit: 0
/* reg_gcr_anatest_en*/
#define offset_of_hdmi_tx_atop_reg_gcr_anatest_en (0xae)
#define mask_of_hdmi_tx_atop_reg_gcr_anatest_en   (0x1)
#define shift_of_hdmi_tx_atop_reg_gcr_anatest_en  (0)
#define HDMI_TX_ATOP_REG_GCR_ANATEST_EN           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_anatest_en)
#define REG_HDMI_TX_ATOP_57_L                     (HDMI_TX_ATOP_REG_GCR_ANATEST_EN)
// h0057, bit: 8
/* reg_gcr_ldo_clktree_fb_res_opt*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_clktree_fb_res_opt (0xae)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_clktree_fb_res_opt   (0x100)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_clktree_fb_res_opt  (8)
#define HDMI_TX_ATOP_REG_GCR_LDO_CLKTREE_FB_RES_OPT \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_clktree_fb_res_opt)
// h0058, bit: 3
/* reg_gcr_ldo_mux_fb_res_opt_ch*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_fb_res_opt_ch (0xb0)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_mux_fb_res_opt_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_mux_fb_res_opt_ch  (0)
#define HDMI_TX_ATOP_REG_GCR_LDO_MUX_FB_RES_OPT_CH \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_mux_fb_res_opt_ch)
#define REG_HDMI_TX_ATOP_58_L (HDMI_TX_ATOP_REG_GCR_LDO_MUX_FB_RES_OPT_CH)
// h0058, bit: 11
/* reg_gcr_ldo_predrv_fb_res_opt_ch*/
#define offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_fb_res_opt_ch (0xb0)
#define mask_of_hdmi_tx_atop_reg_gcr_ldo_predrv_fb_res_opt_ch   (0xf00)
#define shift_of_hdmi_tx_atop_reg_gcr_ldo_predrv_fb_res_opt_ch  (8)
#define HDMI_TX_ATOP_REG_GCR_LDO_PREDRV_FB_RES_OPT_CH \
    (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_ldo_predrv_fb_res_opt_ch)
// h0059, bit: 3
/* reg_gcr_test_clk_sel_ch*/
#define offset_of_hdmi_tx_atop_reg_gcr_test_clk_sel_ch (0xb2)
#define mask_of_hdmi_tx_atop_reg_gcr_test_clk_sel_ch   (0xf)
#define shift_of_hdmi_tx_atop_reg_gcr_test_clk_sel_ch  (0)
#define HDMI_TX_ATOP_REG_GCR_TEST_CLK_SEL_CH           (REG_HDMI_TX_ATOP_BASE + offset_of_hdmi_tx_atop_reg_gcr_test_clk_sel_ch)
#define REG_HDMI_TX_ATOP_59_L                          (HDMI_TX_ATOP_REG_GCR_TEST_CLK_SEL_CH)
#define REG_HDMI_TX_ATOP_5A_L                          (REG_HDMI_TX_ATOP_BASE + 0xb4)
#define REG_HDMI_TX_ATOP_5B_L                          (REG_HDMI_TX_ATOP_BASE + 0xb6)
#define REG_HDMI_TX_ATOP_5C_L                          (REG_HDMI_TX_ATOP_BASE + 0xb8)
#define REG_HDMI_TX_ATOP_5D_L                          (REG_HDMI_TX_ATOP_BASE + 0xba)
#define REG_HDMI_TX_ATOP_5E_L                          (REG_HDMI_TX_ATOP_BASE + 0xbc)
#define REG_HDMI_TX_ATOP_5F_L                          (REG_HDMI_TX_ATOP_BASE + 0xbe)
#define REG_HDMI_TX_ATOP_60_L                          (REG_HDMI_TX_ATOP_BASE + 0xc0)
#define REG_HDMI_TX_ATOP_61_L                          (REG_HDMI_TX_ATOP_BASE + 0xc2)
#define REG_HDMI_TX_ATOP_62_L                          (REG_HDMI_TX_ATOP_BASE + 0xc4)
#define REG_HDMI_TX_ATOP_63_L                          (REG_HDMI_TX_ATOP_BASE + 0xc6)
#define REG_HDMI_TX_ATOP_64_L                          (REG_HDMI_TX_ATOP_BASE + 0xc8)
#define REG_HDMI_TX_ATOP_65_L                          (REG_HDMI_TX_ATOP_BASE + 0xca)
#define REG_HDMI_TX_ATOP_66_L                          (REG_HDMI_TX_ATOP_BASE + 0xcc)
#define REG_HDMI_TX_ATOP_67_L                          (REG_HDMI_TX_ATOP_BASE + 0xce)
#define REG_HDMI_TX_ATOP_68_L                          (REG_HDMI_TX_ATOP_BASE + 0xd0)
#define REG_HDMI_TX_ATOP_69_L                          (REG_HDMI_TX_ATOP_BASE + 0xd2)
#define REG_HDMI_TX_ATOP_6A_L                          (REG_HDMI_TX_ATOP_BASE + 0xd4)
#define REG_HDMI_TX_ATOP_6B_L                          (REG_HDMI_TX_ATOP_BASE + 0xd6)
#define REG_HDMI_TX_ATOP_6C_L                          (REG_HDMI_TX_ATOP_BASE + 0xd8)
#define REG_HDMI_TX_ATOP_6D_L                          (REG_HDMI_TX_ATOP_BASE + 0xda)
#define REG_HDMI_TX_ATOP_6E_L                          (REG_HDMI_TX_ATOP_BASE + 0xdc)
#define REG_HDMI_TX_ATOP_6F_L                          (REG_HDMI_TX_ATOP_BASE + 0xde)
#define REG_HDMI_TX_ATOP_70_L                          (REG_HDMI_TX_ATOP_BASE + 0xe0)
#define REG_HDMI_TX_ATOP_71_L                          (REG_HDMI_TX_ATOP_BASE + 0xe2)
#define REG_HDMI_TX_ATOP_72_L                          (REG_HDMI_TX_ATOP_BASE + 0xe4)
#define REG_HDMI_TX_ATOP_73_L                          (REG_HDMI_TX_ATOP_BASE + 0xe6)
#define REG_HDMI_TX_ATOP_74_L                          (REG_HDMI_TX_ATOP_BASE + 0xe8)
#define REG_HDMI_TX_ATOP_75_L                          (REG_HDMI_TX_ATOP_BASE + 0xea)
#define REG_HDMI_TX_ATOP_76_L                          (REG_HDMI_TX_ATOP_BASE + 0xec)
#define REG_HDMI_TX_ATOP_77_L                          (REG_HDMI_TX_ATOP_BASE + 0xee)
#define REG_HDMI_TX_ATOP_78_L                          (REG_HDMI_TX_ATOP_BASE + 0xf0)
#define REG_HDMI_TX_ATOP_79_L                          (REG_HDMI_TX_ATOP_BASE + 0xf2)
#define REG_HDMI_TX_ATOP_7A_L                          (REG_HDMI_TX_ATOP_BASE + 0xf4)
#define REG_HDMI_TX_ATOP_7B_L                          (REG_HDMI_TX_ATOP_BASE + 0xf6)
#define REG_HDMI_TX_ATOP_7C_L                          (REG_HDMI_TX_ATOP_BASE + 0xf8)
#define REG_HDMI_TX_ATOP_7D_L                          (REG_HDMI_TX_ATOP_BASE + 0xfa)
#define REG_HDMI_TX_ATOP_7E_L                          (REG_HDMI_TX_ATOP_BASE + 0xfc)
#define REG_HDMI_TX_ATOP_7F_L                          (REG_HDMI_TX_ATOP_BASE + 0xfe)
#endif
