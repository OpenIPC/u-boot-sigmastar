/*
* hal_pnl_lpll_tbl.h- Sigmastar
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

#ifndef _LPLL_TBL_H_
#define _LPLL_TBL_H_

#define HAL_PNL_LPLL_REG_NUM    6

typedef enum
{
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ, //0
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ,    //1
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ,     //2
    E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ,   //3
    E_HAL_PNL_SUPPORTED_LPLL_MAX,                      //4
} HalPnlLpllType_e;

typedef struct
{
    u32  address;
    u16 value;
}HalPnlLpllTbl_t;

HalPnlLpllTbl_t LPLLSettingTBL[E_HAL_PNL_SUPPORTED_LPLL_MAX][HAL_PNL_LPLL_REG_NUM]=
{
    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
      //Address,Value
      {0x103380, 0x2201},
      {0x103382, 0x0420},
      {0x103384, 0x0041},
      {0x103386, 0x0000},
      {0x103394, 0x0001},
      {0x103396, 0x0000},
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
          //Address,Value
      {0x103380, 0x2201},
      {0x103382, 0x0420},
      {0x103384, 0x0042},
      {0x103386, 0x0001},
      {0x103394, 0x0001},
      {0x103396, 0x0000},
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
              //Address,Value
      {0x103380, 0x2201},
      {0x103382, 0x0420},
      {0x103384, 0x0043},
      {0x103386, 0x0002},
      {0x103394, 0x0001},
      {0x103396, 0x0000},
    },

    { //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
                  //Address,Value
      {0x103380, 0x2201},
      {0x103382, 0x0420},
      {0x103384, 0x0083},
      {0x103386, 0x0003},
      {0x103394, 0x0001},
      {0x103396, 0x0000},
    },
};

u16 u16LoopGainDsi[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    16,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    4,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    2,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
};


u16 u16LoopDivDsi[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    1,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    1,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    1,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    1,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
};


u16 u16LoopGain[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    16,          //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    4,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    2,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
};

u16 u16LoopDiv[E_HAL_PNL_SUPPORTED_LPLL_MAX]=
{
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ    NO.0
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ    NO.1
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ    NO.2
    8,           //E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ    NO.3
};

#endif //_LPLL_TBL_H_
