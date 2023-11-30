/*
* hal_hdmitx_chip.h- Sigmastar
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

#ifndef _HAL_HDMITX_CHIP_H_
#define _HAL_HDMITX_CHIP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_HDMITX_CTX_INST_MAX 2
#define HAL_HDMITX_DEVICE_MAX 1
#define HAL_HDMITX_CLK_NUM 1

#define CLK_MHZ(x)              (x * 1000000)
#define HAL_HDMITX_CLK_DISP_432 CLK_MHZ(432)

#define HAL_HDMITX_CLK_ON_SETTING \
    {                             \
        1,                        \
    }

#define HAL_HDMITX_CLK_OFF_SETTING \
    {                              \
        0,                         \
    }

#define HAL_HDMITX_CLK_RATE_SETTING \
    {                               \
        HAL_HDMITX_CLK_DISP_432,    \
    }

// Mux_ATTR = 1 : clk idx
// Mux_ATTR = 0 : clk rate
#define HAL_HDMITX_CLK_MUX_ATTR \
    {                           \
        0,                      \
    }

#define HAL_HDMITX_CLK_NAME \
    {                       \
        "disp432",          \
    }

#define HAL_HDMITX_IRQ_EN_FLAG      (E_HDMITX_IRQ_12 | E_HDMITX_IRQ_10)
#define HDMITX_CSC_SUPPORT_R2Y(dev) (1)
#define GENERAL_PKT_NUM             0x0BU // wilson@kano
#define INFOFRM_PKT_NUM             0x08U // for HDR packet ID = 0x87;  0x06U //wilson@kano

#define HDMITX_AUDIO_CH_2_CNT 2
#define HDMITX_AUDIO_CH_8_CNT 8

#define HDMITX_VS_INFO_PKT_VER  0x01U
#define HDMITX_VS_INFO_PKT_LEN  0x1BU
#define HDMITX_VS_INFO_PKT_TYPE 0x81U

#define HDMITX_AVI_INFO_PKT_VER  0x02U
#define HDMITX_AVI_INFO_PKT_LEN  0x0DU
#define HDMITX_AVI_INFO_PKT_TYPE 0x82U

#define HDMITX_SPD_INFO_PKT_VER  0x01U
#define HDMITX_SPD_INFO_PKT_LEN  0x19U
#define HDMITX_SPD_INFO_PKT_TYPE 0x83U

#define HDMITX_AUD_INFO_PKT_VER  0x01U
#define HDMITX_AUD_INFO_PKT_LEN  0x0AU
#define HDMITX_AUD_INFO_PKT_TYPE 0x84U

#define HDMITX_HDR_INFO_PKT_VER  0x01U
#define HDMITX_HDR_INFO_PKT_LEN  0x1BU // wilson@kano: temp solution
#define HDMITX_HDR_INFO_PKT_TYPE 0x87U
// HDMI packet cyclic frame count
#define HDMITX_PACKET_NULL_FCNT 0U  ///< 0 ~ 31
#define HDMITX_PACKET_ACR_FCNT  0U  ///< 0 ~ 15
#define HDMITX_PACKET_GC_FCNT   0U  ///< 0 ~ 1
#define HDMITX_PACKET_ACP_FCNT  15U ///< 0 ~ 31
#define HDMITX_PACKET_ISRC_FCNT 15U ///< 0 ~ 31

#define HDMITX_PACKET_VS_FCNT  0U ///< 0 ~ 31
#define HDMITX_PACKET_AVI_FCNT 0U ///< 0 ~ 31
#define HDMITX_PACKET_SPD_FCNT 1U ///< 0 ~ 31
#define HDMITX_PACKET_AUD_FCNT 0U ///< 0 ~ 31
#define HDMITX_PACKET_HDR_FCNT 0U

#define HDMITX_PACKET_SPD_SDI 1U // Digital STB

#define HDMITX_MAX_PIXEL_CLK  597000000 // Max cupported pixel clock
#define HDMITX_CHECK_RX_TIMER 100UL     // Check Rx HPD and TMDS clock status

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

#endif
