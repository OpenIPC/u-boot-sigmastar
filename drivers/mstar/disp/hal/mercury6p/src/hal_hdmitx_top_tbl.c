/*
* hal_hdmitx_top_tbl.c- Sigmastar
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

//****************************************************
// Drive Chip           : CAMDRV_HDMITX
// Excel CodeGen Version: 1.05
// Excel SW      Version: 1.01
//****************************************************

#ifndef __HAL_HDMITX_TOP_TBL_C__
#define __HAL_HDMITX_TOP_TBL_C__

#include "hal_hdmitx_top_tbl.h"
#include "reg_hdmitx_top.h"
#include "drv_disp_os.h"
//****************************************************
// INIT_HDMITX_ATOP
//****************************************************
u16 g_stinithdmitxatopcommontbl[INIT_HDMITX_ATOP_COMMON_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE
                                                                 + REG_TBL_DATA_SIZE] = {
    {
        DRV_REG(REG_HDMI_TX_ATOP_17_L), /* HDMI_TX_ATOP_REG_DISCDET_EN_CH */
        0xFF,
        0x08,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_20_L), /* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH0, HDMI_TX_ATOP_REG_RESERVED_20_0,
                                           HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH1, HDMI_TX_ATOP_REG_RESERVED_20_1 */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_21_L), /* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH2, HDMI_TX_ATOP_REG_RESERVED_21_0,
                                           HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP0_CH3, HDMI_TX_ATOP_REG_RESERVED_21_1 */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_2C_L), /* HDMI_TX_ATOP_REG_VCLAMP_DRV_EN_CH */
        0xFF,
        0x0f,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_33_L), /* HDMI_TX_ATOP_REG_PD_LDO_CLKTREE */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_34_L), /* HDMI_TX_ATOP_REG_PD_HDMITXPLL */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_35_L), /* HDMI_TX_ATOP_REG_PD_DRV_BIASGEN_CH, HDMI_TX_ATOP_REG_PD_BIASGEN */
        0xFF,
        0x00,
    },
    {
        DRV_REG(
            REG_HDMI_TX_ATOP_39_L), /* HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH0, HDMI_TX_ATOP_REG_RESERVED_39_0,
                                       HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH1, HDMI_TX_ATOP_REG_RESERVED_39_1,
                                       HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH2, HDMI_TX_ATOP_REG_RESERVED_39_2,
                                       HDMI_TX_ATOP_REG_GCR_PREDRV_SLEWRATE_CTRL_CH3, HDMI_TX_ATOP_REG_RESERVED_39_3 */
        0xffff,
        0x3333,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_00_L), /* HDMI_TX_ATOP_REG_CKG_HDMI_TX_ATOP */
        0xFF,
        0x00,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_06_L), /* HDMI_TX_ATOP_REG_HDMI_SYN_SSC_SPAN */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_05_L), /* HDMI_TX_ATOP_REG_HDMI_SYN_SSC_STEP */
        0xffff,
        0x0,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_1C_L), /* HDMI_TX_ATOP_REG_GCR_HDMITXPLL_LOOP_DIV_FIRST, HDMI_TX_ATOP_REG_RESERVED_1C,
                                           HDMI_TX_ATOP_REG_GCR_HDMITXPLL_LOOP_DIV_SECOND */
        0xffff,
        0x501,
    },
    {
        DRV_REG(
            REG_HDMI_TX_ATOP_1B_L), /* HDMI_TX_ATOP_REG_GCR_EN_HDMITXPLL_TEST, HDMI_TX_ATOP_REG_GCR_EN_HDMITXPLL_XTAL,
                                       HDMI_TX_ATOP_REG_GCR_HDMITXPLL_CP_SW, HDMI_TX_ATOP_REG_GCR_HDMITXPLL_DAC_DIV,
                                       HDMI_TX_ATOP_REG_RESERVED_1B_0, HDMI_TX_ATOP_REG_GCR_HDMITXPLL_ICTRL,
                                       HDMI_TX_ATOP_REG_RESERVED_1B_1, HDMI_TX_ATOP_REG_GCR_HDMITXPLL_INPUT_DIV */
        0xffff,
        0x220,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_52_L), /* HDMI_TX_ATOP_REG_GCR_CH0_OP_ADJ, HDMI_TX_ATOP_REG_GCR_CH1_OP_ADJ */
        0xffff,
        0x606,
    },
    {
        DRV_REG(REG_HDMI_TX_ATOP_53_L), /* HDMI_TX_ATOP_REG_GCR_CH2_OP_ADJ, HDMI_TX_ATOP_REG_GCR_CH3_OP_ADJ */
        0xffff,
        0x606,
    },
    {
        DRV_REG(
            REG_HDMI_TX_ATOP_50_L), /* HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH0, HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH1,
                                       HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH2, HDMI_TX_ATOP_REG_GCR_SER_36BIT_MODE_CH3,
                                       HDMI_TX_ATOP_REG_GCR_DIV_EN */
        0xffff,
        0x100,
    },
    {
        DRV_REG(REG_HDMI_LTP_03_L), /* HDMI_LTP_REG_HDMI_14_OUT_SEL, HDMI_LTP_REG_HDMI_14_FIFO_CLEAR_EN,
                                       HDMI_LTP_REG_HDMI_14_SKIP_DATA, HDMI_LTP_REG_HDMI_14_OFF_DATA */
        0xFF,
        0x02,
    },
};

#ifndef HAL_HDMITX_TOP_TBL_BYSELF 
u16 g_stinithdmitxatoptbl[INIT_HDMITX_ATOP_REG_NUM][REG_TBL_ADDR_SIZE + REG_TBL_MASK_SIZE + E_HAL_HDMITX_ID_NUM] = 
{
    { DRV_REG(REG_HDMI_TX_ATOP_16_L)/* HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP2_CH, HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP0_CH, HDMI_TX_ATOP_REG_DIN_EN_PSTDRV_TAP1_CH */,
   0xffff,           0xf0,/*720_24P*/
           0xf0,/*720_25P*/
           0xf0,/*720_29D97P*/
           0xf0,/*720_30P*/
           0xf0,/*720_50P*/
           0xf0,/*720_59D94P*/
           0xf0,/*720_60P*/
           0xf0,/*1080_23D98P*/
           0xf0,/*1080_24P*/
           0xf0,/*1080_25P*/
           0xf0,/*1080_29D97P*/
           0xf0,/*1080_30P*/
           0xf0,/*1080_50P*/
           0xf0,/*1080_59D94P*/
           0xf0,/*1080_60P*/
           0xf0,/*2560x1440_30P*/
           0x7f0,/*2560x1440_50P*/
           0x7f0,/*2560x1440_60P*/
           0x7f0,/*3840x2160_24P*/
           0x7f0,/*3840x2160_25P*/
           0x7f0,/*3840x2160_29D97P*/
           0x7f0,/*3840x2160_30P*/
           0x7f0,/*3840x2160_60P*/
           0xf0,/*640x480_60P*/
           0xf0,/*480_60P*/
           0xf0,/*576_50P*/
           0xf0,/*800x600_60P*/
           0xf0,/*848x480_60P*/
           0xf0,/*1024x768_60P*/
           0xf0,/*1280x768_60P*/
           0xf0,/*1280x800_60P*/
           0xf0,/*1280x960_60P*/
           0xf0,/*1280x1024_60P*/
           0xf0,/*1360x768_60P*/
           0xf0,/*1366x768_60P*/
           0xf0,/*1400x1050_60P*/
           0xf0,/*1440x900_60P*/
           0xf0,/*1600x900_60P*/
           0xf0,/*1600x1200_60P*/
           0xf0,/*1680x1050_60P*/
           0xf0,/*1920x1200_60P*/
           0xf0,/*1920x1440_60P*/
           0x7f0,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_22_L)/* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH0, HDMI_TX_ATOP_REG_RESERVED_22_0, HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH1, HDMI_TX_ATOP_REG_RESERVED_22_1 */,
   0xffff,           0x1414,/*720_24P*/
           0x1414,/*720_25P*/
           0x1414,/*720_29D97P*/
           0x1414,/*720_30P*/
           0x1414,/*720_50P*/
           0x1414,/*720_59D94P*/
           0x1414,/*720_60P*/
           0x1414,/*1080_23D98P*/
           0x1414,/*1080_24P*/
           0x1414,/*1080_25P*/
           0x1414,/*1080_29D97P*/
           0x1414,/*1080_30P*/
           0x1414,/*1080_50P*/
           0x1414,/*1080_59D94P*/
           0x1414,/*1080_60P*/
           0x1414,/*2560x1440_30P*/
           0x2b2b,/*2560x1440_50P*/
           0x2b2b,/*2560x1440_60P*/
           0x2b2b,/*3840x2160_24P*/
           0x2b2b,/*3840x2160_25P*/
           0x2b2b,/*3840x2160_29D97P*/
           0x2b2b,/*3840x2160_30P*/
           0x2b2b,/*3840x2160_60P*/
           0x1414,/*640x480_60P*/
           0x1414,/*480_60P*/
           0x1414,/*576_50P*/
           0x1414,/*800x600_60P*/
           0x1414,/*848x480_60P*/
           0x1414,/*1024x768_60P*/
           0x1414,/*1280x768_60P*/
           0x1414,/*1280x800_60P*/
           0x1414,/*1280x960_60P*/
           0x1414,/*1280x1024_60P*/
           0x1414,/*1360x768_60P*/
           0x1414,/*1366x768_60P*/
           0x1414,/*1400x1050_60P*/
           0x1414,/*1440x900_60P*/
           0x1414,/*1600x900_60P*/
           0x1414,/*1600x1200_60P*/
           0x1414,/*1680x1050_60P*/
           0x1414,/*1920x1200_60P*/
           0x1414,/*1920x1440_60P*/
           0x2b2b,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_23_L)/* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH2, HDMI_TX_ATOP_REG_RESERVED_23_0, HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH3, HDMI_TX_ATOP_REG_RESERVED_23_1 */,
   0xffff,           0x1414,/*720_24P*/
           0x1414,/*720_25P*/
           0x1414,/*720_29D97P*/
           0x1414,/*720_30P*/
           0x1414,/*720_50P*/
           0x1414,/*720_59D94P*/
           0x1414,/*720_60P*/
           0x1414,/*1080_23D98P*/
           0x1414,/*1080_24P*/
           0x1414,/*1080_25P*/
           0x1414,/*1080_29D97P*/
           0x1414,/*1080_30P*/
           0x1414,/*1080_50P*/
           0x1414,/*1080_59D94P*/
           0x1414,/*1080_60P*/
           0x1414,/*2560x1440_30P*/
           0x142b,/*2560x1440_50P*/
           0x142b,/*2560x1440_60P*/
           0x142b,/*3840x2160_24P*/
           0x142b,/*3840x2160_25P*/
           0x142b,/*3840x2160_29D97P*/
           0x142b,/*3840x2160_30P*/
           0x142b,/*3840x2160_60P*/
           0x1414,/*640x480_60P*/
           0x1414,/*480_60P*/
           0x1414,/*576_50P*/
           0x1414,/*800x600_60P*/
           0x1414,/*848x480_60P*/
           0x1414,/*1024x768_60P*/
           0x1414,/*1280x768_60P*/
           0x1414,/*1280x800_60P*/
           0x1414,/*1280x960_60P*/
           0x1414,/*1280x1024_60P*/
           0x1414,/*1360x768_60P*/
           0x1414,/*1366x768_60P*/
           0x1414,/*1400x1050_60P*/
           0x1414,/*1440x900_60P*/
           0x1414,/*1600x900_60P*/
           0x1414,/*1600x1200_60P*/
           0x1414,/*1680x1050_60P*/
           0x1414,/*1920x1200_60P*/
           0x1414,/*1920x1440_60P*/
           0x142b,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_24_L)/* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH0, HDMI_TX_ATOP_REG_RESERVED_24_0, HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH1, HDMI_TX_ATOP_REG_RESERVED_24_1 */,
   0xffff,           0x0,/*720_24P*/
           0x0,/*720_25P*/
           0x0,/*720_29D97P*/
           0x0,/*720_30P*/
           0x0,/*720_50P*/
           0x0,/*720_59D94P*/
           0x0,/*720_60P*/
           0x0,/*1080_23D98P*/
           0x0,/*1080_24P*/
           0x0,/*1080_25P*/
           0x0,/*1080_29D97P*/
           0x0,/*1080_30P*/
           0x0,/*1080_50P*/
           0x0,/*1080_59D94P*/
           0x0,/*1080_60P*/
           0x0,/*2560x1440_30P*/
           0x1313,/*2560x1440_50P*/
           0x1313,/*2560x1440_60P*/
           0x1313,/*3840x2160_24P*/
           0x1313,/*3840x2160_25P*/
           0x1313,/*3840x2160_29D97P*/
           0x1313,/*3840x2160_30P*/
           0x1313,/*3840x2160_60P*/
           0x0,/*640x480_60P*/
           0x0,/*480_60P*/
           0x0,/*576_50P*/
           0x0,/*800x600_60P*/
           0x0,/*848x480_60P*/
           0x0,/*1024x768_60P*/
           0x0,/*1280x768_60P*/
           0x0,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x0,/*1280x1024_60P*/
           0x0,/*1360x768_60P*/
           0x0,/*1366x768_60P*/
           0x0,/*1400x1050_60P*/
           0x0,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x0,/*1600x1200_60P*/
           0x0,/*1680x1050_60P*/
           0x0,/*1920x1200_60P*/
           0x0,/*1920x1440_60P*/
           0x1313,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_25_L)/* HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH2, HDMI_TX_ATOP_REG_RESERVED_25_0, HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH3, HDMI_TX_ATOP_REG_RESERVED_25_1 */,
   0xffff,           0x0,/*720_24P*/
           0x0,/*720_25P*/
           0x0,/*720_29D97P*/
           0x0,/*720_30P*/
           0x0,/*720_50P*/
           0x0,/*720_59D94P*/
           0x0,/*720_60P*/
           0x0,/*1080_23D98P*/
           0x0,/*1080_24P*/
           0x0,/*1080_25P*/
           0x0,/*1080_29D97P*/
           0x0,/*1080_30P*/
           0x0,/*1080_50P*/
           0x0,/*1080_59D94P*/
           0x0,/*1080_60P*/
           0x0,/*2560x1440_30P*/
           0x13,/*2560x1440_50P*/
           0x13,/*2560x1440_60P*/
           0x13,/*3840x2160_24P*/
           0x13,/*3840x2160_25P*/
           0x13,/*3840x2160_29D97P*/
           0x13,/*3840x2160_30P*/
           0x13,/*3840x2160_60P*/
           0x0,/*640x480_60P*/
           0x0,/*480_60P*/
           0x0,/*576_50P*/
           0x0,/*800x600_60P*/
           0x0,/*848x480_60P*/
           0x0,/*1024x768_60P*/
           0x0,/*1280x768_60P*/
           0x0,/*1280x800_60P*/
           0x0,/*1280x960_60P*/
           0x0,/*1280x1024_60P*/
           0x0,/*1360x768_60P*/
           0x0,/*1366x768_60P*/
           0x0,/*1400x1050_60P*/
           0x0,/*1440x900_60P*/
           0x0,/*1600x900_60P*/
           0x0,/*1600x1200_60P*/
           0x0,/*1680x1050_60P*/
           0x0,/*1920x1200_60P*/
           0x0,/*1920x1440_60P*/
           0x13,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_30_L)/* HDMI_TX_ATOP_REG_PD_PSTDRV_TAP2_CH, HDMI_TX_ATOP_REG_PD_PSTDRV_TAP0_CH, HDMI_TX_ATOP_REG_PD_PSTDRV_TAP1_CH */,
   0xffff,           0xf0f,/*720_24P*/
           0xf0f,/*720_25P*/
           0xf0f,/*720_29D97P*/
           0xf0f,/*720_30P*/
           0xf0f,/*720_50P*/
           0xf0f,/*720_59D94P*/
           0xf0f,/*720_60P*/
           0xf0f,/*1080_23D98P*/
           0xf0f,/*1080_24P*/
           0xf0f,/*1080_25P*/
           0xf0f,/*1080_29D97P*/
           0xf0f,/*1080_30P*/
           0xf0f,/*1080_50P*/
           0xf0f,/*1080_59D94P*/
           0xf0f,/*1080_60P*/
           0xf0f,/*2560x1440_30P*/
           0x80f,/*2560x1440_50P*/
           0x80f,/*2560x1440_60P*/
           0x80f,/*3840x2160_24P*/
           0x80f,/*3840x2160_25P*/
           0x80f,/*3840x2160_29D97P*/
           0x80f,/*3840x2160_30P*/
           0x80f,/*3840x2160_60P*/
           0xf0f,/*640x480_60P*/
           0xf0f,/*480_60P*/
           0xf0f,/*576_50P*/
           0xf0f,/*800x600_60P*/
           0xf0f,/*848x480_60P*/
           0xf0f,/*1024x768_60P*/
           0xf0f,/*1280x768_60P*/
           0xf0f,/*1280x800_60P*/
           0xf0f,/*1280x960_60P*/
           0xf0f,/*1280x1024_60P*/
           0xf0f,/*1360x768_60P*/
           0xf0f,/*1366x768_60P*/
           0xf0f,/*1400x1050_60P*/
           0xf0f,/*1440x900_60P*/
           0xf0f,/*1600x900_60P*/
           0xf0f,/*1600x1200_60P*/
           0xf0f,/*1680x1050_60P*/
           0xf0f,/*1920x1200_60P*/
           0xf0f,/*1920x1440_60P*/
           0x80f,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_31_L)/* HDMI_TX_ATOP_REG_PD_PREDRV_TAP2_CH, HDMI_TX_ATOP_REG_PD_PREDRV_TAP0_CH, HDMI_TX_ATOP_REG_PD_PREDRV_TAP1_CH */,
   0xffff,           0xf0f,/*720_24P*/
           0xf0f,/*720_25P*/
           0xf0f,/*720_29D97P*/
           0xf0f,/*720_30P*/
           0xf0f,/*720_50P*/
           0xf0f,/*720_59D94P*/
           0xf0f,/*720_60P*/
           0xf0f,/*1080_23D98P*/
           0xf0f,/*1080_24P*/
           0xf0f,/*1080_25P*/
           0xf0f,/*1080_29D97P*/
           0xf0f,/*1080_30P*/
           0xf0f,/*1080_50P*/
           0xf0f,/*1080_59D94P*/
           0xf0f,/*1080_60P*/
           0xf0f,/*2560x1440_30P*/
           0x80f,/*2560x1440_50P*/
           0x80f,/*2560x1440_60P*/
           0x80f,/*3840x2160_24P*/
           0x80f,/*3840x2160_25P*/
           0x80f,/*3840x2160_29D97P*/
           0x80f,/*3840x2160_30P*/
           0x80f,/*3840x2160_60P*/
           0xf0f,/*640x480_60P*/
           0xf0f,/*480_60P*/
           0xf0f,/*576_50P*/
           0xf0f,/*800x600_60P*/
           0xf0f,/*848x480_60P*/
           0xf0f,/*1024x768_60P*/
           0xf0f,/*1280x768_60P*/
           0xf0f,/*1280x800_60P*/
           0xf0f,/*1280x960_60P*/
           0xf0f,/*1280x1024_60P*/
           0xf0f,/*1360x768_60P*/
           0xf0f,/*1366x768_60P*/
           0xf0f,/*1400x1050_60P*/
           0xf0f,/*1440x900_60P*/
           0xf0f,/*1600x900_60P*/
           0xf0f,/*1600x1200_60P*/
           0xf0f,/*1680x1050_60P*/
           0xf0f,/*1920x1200_60P*/
           0xf0f,/*1920x1440_60P*/
           0x80f,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_32_L)/* HDMI_TX_ATOP_REG_PD_RTERM_CH, HDMI_TX_ATOP_REG_PD_LDO_PREDRV_CH, HDMI_TX_ATOP_REG_PD_LDO_MUX_CH */,
   0xffff,           0xf,/*720_24P*/
           0xf,/*720_25P*/
           0xf,/*720_29D97P*/
           0xf,/*720_30P*/
           0xf,/*720_50P*/
           0xf,/*720_59D94P*/
           0xf,/*720_60P*/
           0xf,/*1080_23D98P*/
           0xf,/*1080_24P*/
           0xf,/*1080_25P*/
           0xf,/*1080_29D97P*/
           0xf,/*1080_30P*/
           0xf,/*1080_50P*/
           0xf,/*1080_59D94P*/
           0xf,/*1080_60P*/
           0xf,/*2560x1440_30P*/
           0x8,/*2560x1440_50P*/
           0x8,/*2560x1440_60P*/
           0x8,/*3840x2160_24P*/
           0x8,/*3840x2160_25P*/
           0x8,/*3840x2160_29D97P*/
           0x8,/*3840x2160_30P*/
           0x8,/*3840x2160_60P*/
           0xf,/*640x480_60P*/
           0xf,/*480_60P*/
           0xf,/*576_50P*/
           0xf,/*800x600_60P*/
           0xf,/*848x480_60P*/
           0xf,/*1024x768_60P*/
           0xf,/*1280x768_60P*/
           0xf,/*1280x800_60P*/
           0xf,/*1280x960_60P*/
           0xf,/*1280x1024_60P*/
           0xf,/*1360x768_60P*/
           0xf,/*1366x768_60P*/
           0xf,/*1400x1050_60P*/
           0xf,/*1440x900_60P*/
           0xf,/*1600x900_60P*/
           0xf,/*1600x1200_60P*/
           0xf,/*1680x1050_60P*/
           0xf,/*1920x1200_60P*/
           0xf,/*1920x1440_60P*/
           0x8,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_3B_L)/* HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH0, HDMI_TX_ATOP_REG_RESERVED_3B_0, HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH1, HDMI_TX_ATOP_REG_RESERVED_3B_1, HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH2, HDMI_TX_ATOP_REG_RESERVED_3B_2, HDMI_TX_ATOP_REG_GCR_MUX_TAP_EN_CH3, HDMI_TX_ATOP_REG_RESERVED_3B_3 */,
   0xffff,           0x2222,/*720_24P*/
           0x2222,/*720_25P*/
           0x2222,/*720_29D97P*/
           0x2222,/*720_30P*/
           0x2222,/*720_50P*/
           0x2222,/*720_59D94P*/
           0x2222,/*720_60P*/
           0x2222,/*1080_23D98P*/
           0x2222,/*1080_24P*/
           0x2222,/*1080_25P*/
           0x2222,/*1080_29D97P*/
           0x2222,/*1080_30P*/
           0x2222,/*1080_50P*/
           0x2222,/*1080_59D94P*/
           0x2222,/*1080_60P*/
           0x2222,/*2560x1440_30P*/
           0x2333,/*2560x1440_50P*/
           0x2333,/*2560x1440_60P*/
           0x2333,/*3840x2160_24P*/
           0x2333,/*3840x2160_25P*/
           0x2333,/*3840x2160_29D97P*/
           0x2333,/*3840x2160_30P*/
           0x2333,/*3840x2160_60P*/
           0x2222,/*640x480_60P*/
           0x2222,/*480_60P*/
           0x2222,/*576_50P*/
           0x2222,/*800x600_60P*/
           0x2222,/*848x480_60P*/
           0x2222,/*1024x768_60P*/
           0x2222,/*1280x768_60P*/
           0x2222,/*1280x800_60P*/
           0x2222,/*1280x960_60P*/
           0x2222,/*1280x1024_60P*/
           0x2222,/*1360x768_60P*/
           0x2222,/*1366x768_60P*/
           0x2222,/*1400x1050_60P*/
           0x2222,/*1440x900_60P*/
           0x2222,/*1600x900_60P*/
           0x2222,/*1600x1200_60P*/
           0x2222,/*1680x1050_60P*/
           0x2222,/*1920x1200_60P*/
           0x2222,/*1920x1440_60P*/
           0x2333,/*2560x1600_60P*/
    }, 
    { DRV_REG(REG_HDMI_TX_ATOP_1D_L)/* HDMI_TX_ATOP_REG_GCR_HDMITXPLL_OUTPUT_DIV, HDMI_TX_ATOP_REG_GCR_HDMITXPLL_PORST, HDMI_TX_ATOP_REG_GCR_HDMITXPLL_TMDS_DIV */,
   0xFF,           0x13,/*720_24P*/
           0x13,/*720_25P*/
           0x12,/*720_29D97P*/
           0x13,/*720_30P*/
           0x12,/*720_50P*/
           0x12,/*720_59D94P*/
           0x12,/*720_60P*/
           0x12,/*1080_23D98P*/
           0x12,/*1080_24P*/
           0x12,/*1080_25P*/
           0x12,/*1080_29D97P*/
           0x12,/*1080_30P*/
           0x11,/*1080_50P*/
           0x11,/*1080_59D94P*/
           0x11,/*1080_60P*/
           0x11,/*2560x1440_30P*/
           0x10,/*2560x1440_50P*/
           0x10,/*2560x1440_60P*/
           0x10,/*3840x2160_24P*/
           0x10,/*3840x2160_25P*/
           0x10,/*3840x2160_29D97P*/
           0x10,/*3840x2160_30P*/
           0x10,/*3840x2160_60P*/
           0x13,/*640x480_60P*/
           0x13,/*480_60P*/
           0x13,/*576_50P*/
           0x13,/*800x600_60P*/
           0x13,/*848x480_60P*/
           0x13,/*1024x768_60P*/
           0x12,/*1280x768_60P*/
           0x12,/*1280x800_60P*/
           0x12,/*1280x960_60P*/
           0x12,/*1280x1024_60P*/
           0x12,/*1360x768_60P*/
           0x12,/*1366x768_60P*/
           0x12,/*1400x1050_60P*/
           0x12,/*1440x900_60P*/
           0x11,/*1600x900_60P*/
           0x11,/*1600x1200_60P*/
           0x12,/*1680x1050_60P*/
           0x11,/*1920x1200_60P*/
           0x11,/*1920x1440_60P*/
           0x10,/*2560x1600_60P*/
    }, 
};
#endif 



//****************************************************
HAL_HDMITX_INFO g_sthalhdmitxtoptbl[E_HAL_HDMITX_TAB_NUM]=
{
#ifndef HAL_HDMITX_TOP_TBL_BYSELF 
    {*g_stinithdmitxatoptbl, INIT_HDMITX_ATOP_REG_NUM, E_HAL_HDMITX_IP_NORMAL},
#endif
    {*g_stinithdmitxatopcommontbl, INIT_HDMITX_ATOP_COMMON_REG_NUM, E_HAL_HDMITX_IP_COMMON},
};

#endif
