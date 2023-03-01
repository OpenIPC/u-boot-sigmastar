/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#ifndef _LPLL_TBL_H_
#define _LPLL_TBL_H_

#define HAL_PNL_LPLL_REG_NUM    20

typedef enum
{
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ,          //0
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ,          //1
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ,          //2
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ,          //3
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO5MHZ,              //4
    E_HAL_PNL_SUPPORTED_LPLL_MAX,          //5
} HalPnlLpllType_e;

typedef struct
{
    u8  address;
    u16 value;
    u16 mask;
}HalPnlLpllTbl_t;

HalPnlLpllTbl_t LPLLSettingTBL[E_HAL_PNL_SUPPORTED_LPLL_MAX][HAL_PNL_LPLL_REG_NUM]=
{
    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
      //Address,Value,Mask
        {0x40,0x0000,0x8000},//reg_lpll_ext_pd
        {0x40,0x0001,0x0007},//reg_lpll_ext_ictrl
        {0x41,0x0000,0x0003},//reg_lpll_ext_input_div_fst
        {0x41,0x0020,0x0030},//reg_lpll_ext_loop_div_fst
        {0x41,0x0300,0x0F00},//reg_lpll_ext_loop_div_sec
        {0x42,0x0002,0x0003},//reg_lpll_ext_scalar_div_fst
        {0x42,0x0050,0x00F0},//reg_lpll_ext_scalar_div_sec
        {0x43,0x0007,0x0007},//reg_lpll_ext_skew_div
        {0x42,0x0000,0x0700},//reg_lpll_ext_fifo_div
        {0x43,0x0010,0x0010},//reg_lpll_ext_skew_en_fixclk
        {0x40,0x0000,0x0800},//reg_lpll_ext_dual_lp_en
        {0x40,0x0000,0x0100},//reg_lpll_ext_sdiv2p5_en
        {0x40,0x2000,0x2000},//reg_lpll_ext_en_mini
        {0x40,0x0000,0x0400},//reg_lpll_ext_en_fifo
        {0x40,0x0200,0x0200},//reg_lpll_ext_en_scalar
        {0x40,0x0000,0x1000},//reg_lpll_ext_fifo_div5_en
        {0x40,0x0000,0x0008},//reg_lpll_ext_sdiv3p5_en
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
      //Address,Value,Mask
        {0x40,0x0000,0x8000},//reg_lpll_ext_pd
        {0x40,0x0001,0x0007},//reg_lpll_ext_ictrl
        {0x41,0x0000,0x0003},//reg_lpll_ext_input_div_fst
        {0x41,0x0020,0x0030},//reg_lpll_ext_loop_div_fst
        {0x41,0x0300,0x0F00},//reg_lpll_ext_loop_div_sec
        {0x42,0x0003,0x0003},//reg_lpll_ext_scalar_div_fst
        {0x42,0x0050,0x00F0},//reg_lpll_ext_scalar_div_sec
        {0x43,0x0007,0x0007},//reg_lpll_ext_skew_div
        {0x42,0x0000,0x0700},//reg_lpll_ext_fifo_div
        {0x43,0x0010,0x0010},//reg_lpll_ext_skew_en_fixclk
        {0x40,0x0000,0x0800},//reg_lpll_ext_dual_lp_en
        {0x40,0x0000,0x0100},//reg_lpll_ext_sdiv2p5_en
        {0x40,0x2000,0x2000},//reg_lpll_ext_en_mini
        {0x40,0x0000,0x0400},//reg_lpll_ext_en_fifo
        {0x40,0x0200,0x0200},//reg_lpll_ext_en_scalar
        {0x40,0x0000,0x1000},//reg_lpll_ext_fifo_div5_en
        {0x40,0x0000,0x0008},//reg_lpll_ext_sdiv3p5_en
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
      //Address,Value,Mask
        {0x40,0x0000,0x8000},//reg_lpll_ext_pd
        {0x40,0x0001,0x0007},//reg_lpll_ext_ictrl
        {0x41,0x0000,0x0003},//reg_lpll_ext_input_div_fst
        {0x41,0x0020,0x0030},//reg_lpll_ext_loop_div_fst
        {0x41,0x0400,0x0F00},//reg_lpll_ext_loop_div_sec
        {0x42,0x0003,0x0003},//reg_lpll_ext_scalar_div_fst
        {0x42,0x0040,0x00F0},//reg_lpll_ext_scalar_div_sec
        {0x43,0x0002,0x0007},//reg_lpll_ext_skew_div
        {0x42,0x0000,0x0700},//reg_lpll_ext_fifo_div
        {0x43,0x0000,0x0010},//reg_lpll_ext_skew_en_fixclk
        {0x40,0x0000,0x0800},//reg_lpll_ext_dual_lp_en
        {0x40,0x0000,0x0100},//reg_lpll_ext_sdiv2p5_en
        {0x40,0x2000,0x2000},//reg_lpll_ext_en_mini
        {0x40,0x0000,0x0400},//reg_lpll_ext_en_fifo
        {0x40,0x0200,0x0200},//reg_lpll_ext_en_scalar
        {0x40,0x0000,0x1000},//reg_lpll_ext_fifo_div5_en
        {0x40,0x0000,0x0008},//reg_lpll_ext_sdiv3p5_en
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
      //Address,Value,Mask
        {0x40,0x0000,0x8000},//reg_lpll_ext_pd
        {0x40,0x0001,0x0007},//reg_lpll_ext_ictrl
        {0x41,0x0000,0x0003},//reg_lpll_ext_input_div_fst
        {0x41,0x0020,0x0030},//reg_lpll_ext_loop_div_fst
        {0x41,0x0300,0x0F00},//reg_lpll_ext_loop_div_sec
        {0x42,0x0003,0x0003},//reg_lpll_ext_scalar_div_fst
        {0x42,0x0080,0x00F0},//reg_lpll_ext_scalar_div_sec
        {0x43,0x0007,0x0007},//reg_lpll_ext_skew_div
        {0x42,0x0000,0x0700},//reg_lpll_ext_fifo_div
        {0x43,0x0010,0x0010},//reg_lpll_ext_skew_en_fixclk
        {0x40,0x0000,0x0800},//reg_lpll_ext_dual_lp_en
        {0x40,0x0100,0x0100},//reg_lpll_ext_sdiv2p5_en
        {0x40,0x2000,0x2000},//reg_lpll_ext_en_mini
        {0x40,0x0000,0x0400},//reg_lpll_ext_en_fifo
        {0x40,0x0200,0x0200},//reg_lpll_ext_en_scalar
        {0x40,0x0000,0x1000},//reg_lpll_ext_fifo_div5_en
        {0x40,0x0000,0x0008},//reg_lpll_ext_sdiv3p5_en
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO5MHZ    NO.4
      //Address,Value,Mask
        {0x40,0x0000,0x8000},//reg_lpll_ext_pd
        {0x40,0x0001,0x0007},//reg_lpll_ext_ictrl
        {0x41,0x0000,0x0003},//reg_lpll_ext_input_div_fst
        {0x41,0x0020,0x0030},//reg_lpll_ext_loop_div_fst
        {0x41,0x0300,0x0F00},//reg_lpll_ext_loop_div_sec
        {0x42,0x0003,0x0003},//reg_lpll_ext_scalar_div_fst
        {0x42,0x0080,0x00F0},//reg_lpll_ext_scalar_div_sec
        {0x43,0x0007,0x0007},//reg_lpll_ext_skew_div
        {0x42,0x0000,0x0700},//reg_lpll_ext_fifo_div
        {0x43,0x0010,0x0010},//reg_lpll_ext_skew_en_fixclk
        {0x40,0x0000,0x0800},//reg_lpll_ext_dual_lp_en
        {0x40,0x0100,0x0100},//reg_lpll_ext_sdiv2p5_en
        {0x40,0x2000,0x2000},//reg_lpll_ext_en_mini
        {0x40,0x0000,0x0400},//reg_lpll_ext_en_fifo
        {0x40,0x0200,0x0200},//reg_lpll_ext_en_scalar
        {0x40,0x0000,0x1000},//reg_lpll_ext_fifo_div5_en
        {0x40,0x0000,0x0008},//reg_lpll_ext_sdiv3p5_en
    },

};
u16 u16LoopGain[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    16,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    4,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    2,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
    12,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO5MHZ    NO.4
};
u16 u16LoopDiv[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_5TO5MHZ    NO.4
};

#endif //_LPLL_TBL_H_
