/*
* hal_pnl.c- Sigmastar
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

#define _HAL_PNL_C_

#include "drv_pnl_os.h"
#include "hal_pnl_common.h"
#include "pnl_debug.h"
#include "hal_pnl_util.h"
#include "hal_pnl_reg.h"
#include "hal_pnl_dsi_reg.h"
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl_lpll_tbl.h"
#include "drv_pnl_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PARSING_MCU_TYPE(x)     (     x == E_HAL_LCD_MCU_68SYS               ? "68_SYS"              : \
                                      x == E_HAL_LCD_MCU_80SYS               ? "80_SYS"             : \
                                                                               "UNKNOWN")


#define PARSING_MCU_BUS_TYPE(x)     (   x == E_HAL_MCU_RGB565_BUS16_CFG             ? "MCU_RGB565_BUS16_CFG"  : \
                                        x == E_HAL_MCU_RGB444_BUS16_CFG             ? "MCU_RGB444_BUS16_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS16_CFG             ? "MCU_RGB666_BUS16_CFG"         : \
                                        x == E_HAL_MCU_RGB888_BUS8_CFG              ? "MCU_RGB888_BUS8_CFG"         : \
                                        x == E_HAL_MCU_RGB332_BUS8_CFG              ? "MCU_RGB332_BUS8_CFG"         : \
                                        x == E_HAL_MCU_RGB444_BUS8_CFG              ? "MCU_RGB444_BUS8_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS8_CFG              ? "MCU_RGB666_BUS8_CFG"         : \
                                        x == E_HAL_MCU_RGB565_BUS8_CFG              ? "MCU_RGB565_BUS8_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS18_CFG             ? "MCU_RGB666_BUS18_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS18_9_9_CFG         ? "MCU_RGB666_BUS18_9_9_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS18_2_16_CFG        ? "MCU_RGB666_BUS18_2_16_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS18_16_2_CFG        ? "MCU_RGB666_BUS18_16_2_CFG"         : \
                                        x == E_HAL_MCU_RGB24_BUS18_16_8_CFG         ? "MCU_RGB24_BUS18_16_8_CFG"         : \
                                        x == E_HAL_MCU_RGB24_BUS18_8_16_CFG         ? "MCU_RGB24_BUS18_8_16_CFG"         : \
                                        x == E_HAL_MCU_RGB18_BUS8_2_8_8_CFG         ? "MCU_RGB18_BUS8_2_8_8_CFG"         : \
                                        x == E_HAL_MCU_RGB666_BUS8_2_7_CFG          ? "MCU_RGB666_BUS8_2_7_CFG"         : \
                                        x == E_HAL_MCU_RGB444_B12_EXT_B16_CFG       ? "MCU_RGB444_B12_EXT_B16_CFG"         : \
                                        x == E_HAL_MCU_RGB444_B15_4_CFG             ? "MCU_RGB444_B15_4_CFG"         : \
                                        x == E_HAL_MCU_RGBB9_9_17_CFG               ? "MCU_RGBB9_9_17_CFG"         : \
                                                                                      "MCU_CFG_NOT_SUPPORT")


#define PARSING_DTYPE_BUS_TYPE(x)     ( x == 0x20                                   ? "YUV422_8BIT_UY0VY1"  : \
                                        x == 0x21                                   ? "YUV422_8BIT_VY0UY1"         : \
                                        x == 0x22                                   ? "YUV422_8BIT_UY1VY0"         : \
                                        x == 0x23                                   ? "YUV422_8BIT_VY1UY0"         : \
                                        x == 0x24                                   ? "YUV422_8BIT_Y0UY1V"         : \
                                        x == 0x25                                   ? "YUV422_8BIT_Y0VY1U"         : \
                                        x == 0x26                                   ? "YUV422_8BIT_Y1UY0V"         : \
                                        x == 0x27                                   ? "YUV422_8BIT_Y1UY0V"         : \
                                        x == 0x10                                   ? "YUV422_16BIT_[UY0][VY1]"  : \
                                        x == 0x11                                   ? "YUV422_16BIT_[VY0][UY1]"         : \
                                        x == 0x12                                   ? "YUV422_16BIT_[UY1][VY0]"         : \
                                        x == 0x13                                   ? "YUV422_16BIT_[VY1][UY0]"         : \
                                        x == 0x14                                   ? "YUV422_16BIT_[Y0U][Y1V]"         : \
                                        x == 0x15                                   ? "YUV422_16BIT_[Y0V][Y1U]"         : \
                                        x == 0x16                                   ? "YUV422_16BIT_[Y1U][Y0V]"         : \
                                        x == 0x17                                   ? "YUV422_16BIT_[Y1U][Y0V]"         : \
                                        x == 0x00                                   ? "24BIT_RGB_888"  : \
                                        x == 0x01                                   ? "24BIT_6BIT0_RGB_666"         : \
                                        x == 0x02                                   ? "24BIT_8BIT0_RGB_565"         : \
                                        x == 0x03                                   ? "24BIT_12BIT0_RGB_444"         : \
                                        x == 0x04                                   ? "24BIT_15BIT0_RGB_333"         : \
                                        x == 0x05                                   ? "24BIT_16BIT0_RGB_332"         : \
                                        x == 0x08                                   ? "24BIT_BGR_888"  : \
                                        x == 0x09                                   ? "24BIT_6BIT0_BGR_666"         : \
                                        x == 0x0A                                   ? "24BIT_8BIT0_BGR_565"         : \
                                        x == 0x0B                                   ? "24BIT_12BIT0_BGR_444"         : \
                                        x == 0x0C                                   ? "24BIT_15BIT0_BGR_333"         : \
                                        x == 0x0D                                   ? "24BIT_16BIT0_BGR_332"         : \
                                                                                      "RESERVED")



#define PARSING_SRGB_TYPE(x)     (    x == E_HAL_PNL_RGB_Delta_RGB_MODE               ? "RGB_ORDER"              : \
                                      x == E_HAL_PNL_RGB_Delta_RBG_MODE               ? "RBG_ORDER"              : \
                                      x == E_HAL_PNL_RGB_Delta_GRB_MODE               ? "GRB_ORDER"              : \
                                      x == E_HAL_PNL_RGB_Delta_GBR_MODE               ? "GBR_ORDER"              : \
                                      x == E_HAL_PNL_RGB_Delta_BRG_MODE               ? "BRG_ORDER"              : \
                                      x == E_HAL_PNL_RGB_Delta_BGR_MODE               ? "BGR_ORDER"              : \
                                                                                      "UNKNOWN")

#define PARSING_MCU_CMD_TYPE(x)     ( x == 1     ? "CMD"  : \
                                                   "IDX")

#define HAL_PNL_MERGE_MIPIDSI_T0INS(t0)      ((u32)t0.u8Confg | ((u32)t0.u8DataId << 8) | ((u32)t0.u8Data0 << 16) | ((u32)t0.u8Data1 << 24))
#define HAL_PNL_MERGE_MIPIDSI_T2INS(t2)      ((u32)t2.u8Confg | ((u32)t2.u8DataId << 8) | ((u32)t2.u16Wc << 16))
#define HAL_PNL_MERGE_MIPIDSI_CMDQ(cmd)      ((u32)cmd.byte0 | ((u32)cmd.byte1 << 8) | ((u32)cmd.byte2 << 16) | ((u32)cmd.byte3 << 24))
#define HAL_PNL_MERGE_MIPIDSI_TIMECON0(cmd)  ((u32)cmd.u8Lpx | ((u32)cmd.u8HsPrpr << 8) | ((u32)cmd.u8HsZero << 16) | ((u32)cmd.u8HsTrail << 24))
#define HAL_PNL_MERGE_MIPIDSI_TIMECON1(cmd)  ((u32)cmd.u8TaGo | ((u32)cmd.u8TaSure << 8) | ((u32)cmd.u8TaGet << 16) | ((u32)cmd.u8DaHsExit << 24))
#define HAL_PNL_MERGE_MIPIDSI_TIMECON2(cmd)  ((u32)cmd.u8ContDet | ((u32)cmd.RSV8 << 8) | ((u32)cmd.u8ClkZero << 16) | ((u32)cmd.u8ClkTrail << 24))
#define HAL_PNL_MERGE_MIPIDSI_TIMECON3(cmd)  ((u32)cmd.u8ClkHsPrpr | ((u32)cmd.u8ClkHsPost << 8) | ((u32)cmd.u8ClkHsExit << 16) | ((u32)cmd.u8rsv24 << 24))

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 gu32JTAGmode = 0;

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

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
bool _HalPnlGetRegByCtx(void *pCtx, u32  *pu32Reg, u32 u32Reg0, u32 u32Reg1)
{
    DrvPnlCtxConfig_t *pstPnlCtx = (DrvPnlCtxConfig_t *)pCtx;
    bool bRet = 1;
    if(pCtx)
    {
        if(pstPnlCtx->s16CtxId == 0)
        {
            *pu32Reg = u32Reg0;
        }
        else if(pstPnlCtx->s16CtxId == 1)
        {
            *pu32Reg = u32Reg1;
        }
        else
        {
            bRet = 0;
            PNL_ERR("%s %d, Ctx Id (%d) out of range\n", __FUNCTION__, __LINE__, pstPnlCtx->s16CtxId);
        }
    }
    else
    {
        bRet = 0;
    }
    return bRet;
}



u32 _HalPnlGetMipiUi(HalPnlMipiDsiConfig_t *pstMipiDsiCfg)
{
    u32 u32Htotal, u32Vtotal, u32BitPerPixel;
    u32 u32DclkHz, u32Mbps, u32Ui;
    u32 u32LaneNum;

    u32BitPerPixel = pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB565 ? 16 :
                     pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB666 ? 18 :
                                                                            24;

    u32LaneNum = pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_4 ? 4 :
                 pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_3 ? 3 :
                 pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_2 ? 2 :
                                                                         1;

    u32Htotal = pstMipiDsiCfg->u16Hactive + pstMipiDsiCfg->u16Hpw + pstMipiDsiCfg->u16Hfp + pstMipiDsiCfg->u16Hbp;
    u32Vtotal = pstMipiDsiCfg->u16Vactive + pstMipiDsiCfg->u16Vpw + pstMipiDsiCfg->u16Vfp + pstMipiDsiCfg->u16Vbp;

    if(pstMipiDsiCfg->u16Bllp)
    {
        u32Htotal += pstMipiDsiCfg->u16Bllp;
    }

    u32DclkHz = (u32Htotal * u32Vtotal * (u32)pstMipiDsiCfg->u16Fps)/1000;
    u32Mbps = u32DclkHz * u32BitPerPixel / u32LaneNum;
    u32Ui = 1000000000/u32Mbps;

    return u32Ui;

}


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

bool HalPnlGetLpllIdx(u32 u32Dclk, u16 *pu16Idx, bool bDsi)
{
    bool bRet = 1;

    if(bDsi)
    {
        if( IS_DATA_LANE_LESS_100M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 0;
        }
        else if( IS_DATA_LANE_BPS_100M_TO_200M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ;
        }
        else if( IS_DATA_LANE_BPS_200M_TO_400M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ;
        }
        else if( IS_DATA_LANE_BPS_400M_TO_800M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ;
        }
        else if( IS_DATA_LANE_BPS_800M_TO_15000M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ;
        }
        else
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 0;
        }
    }
    else
    {
        if( IS_DATA_LANE_LESS_9M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 0;
        }
        else if( IS_DATA_LANE_BPS_9M_TO_9_5M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 1;
        }
        else if( IS_DATA_LANE_BPS_12_5M_TO_25M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ;
        }
        else if( IS_DATA_LANE_BPS_25M_TO_50M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ;
        }
        else if( IS_DATA_LANE_BPS_50M_TO_100M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ;
        }
        else if( IS_DATA_LANE_BPS_100M_TO_187_5M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ;
        }
        else
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 0;
        }
    }

    return bRet;;
}

u16 HalPnlGetLpllGain(u16 u16Idx, bool bDsi)
{
    u16 *pu16Tbl = NULL;

    pu16Tbl = bDsi ? u16LoopGainDsi : u16LoopGain;

    return (u16Idx < E_HAL_PNL_SUPPORTED_LPLL_MAX) ? pu16Tbl[u16Idx] : 1;
}

u16 HalPnlGetLpllDiv(u16 u16Idx, bool bDsi)
{
    u16 *pu16Tbl = NULL;

    pu16Tbl = bDsi ? u16LoopDivDsi : u16LoopDiv;

    return (u16Idx < E_HAL_PNL_SUPPORTED_LPLL_MAX) ? pu16Tbl[u16Idx] : 1;
}

void HalPnlSetAllPadIn(void)
{
    W2BYTE(REG_CHIPTOP_50_L, 0);
    W2BYTE(0x116de2, 0);
}

void HalPnlSetTTLPadMux(u16 u16Val)
{
    W2BYTEMSK(REG_PADTOP_6C_L, u16Val, 0x0737);
}

void HalPnlSetBTPadMux(HalPnlLinkType_e eLinkType, u16 u16Val)
{
    if(eLinkType == E_HAL_PNL_LINK_BT656)
    {
        W2BYTEMSK(REG_PADTOP_60_L, (u16Val & 0x03)<<4, 0x0030);
    }
    else if(eLinkType == E_HAL_PNL_LINK_BT601)
    {
        W2BYTEMSK(REG_PADTOP_72_L, (u16Val & 0x03)<<4, 0x0030);
    }
    else if(eLinkType == E_HAL_PNL_LINK_BT1120)
    {
        W2BYTEMSK(REG_PADTOP_72_L, (u16Val & 0x03), 0x0003);
    }
}

void HalPnlSetRGBPadMux(HalPnlOutputFormatBitMode_e enFmt, u16 u16Val)
{
    if(enFmt == E_HAL_PNL_OUTPUT_8BIT_MODE)
    {
        W2BYTEMSK(REG_PADTOP_74_L, (u16Val & 0x03)<<8, 0x0300);
    }
    else if(E_HAL_PNL_OUTPUT_565BIT_MODE)
    {
        W2BYTEMSK(REG_PADTOP_74_L, (u16Val & 0x01)<<12, 0x1000);
    }
}

void HalPnlSetTtlPadCotnrol(u32 u32Ie, u32 u32Pe, u32 u32Ps, u32 u32Drv)
{
    W2BYTE(REG_CHIPTOP_38_L, (u32Ie & 0xFFFF));
    W2BYTE(REG_CHIPTOP_39_L, ((u32Ie>>16) & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3A_L, (u32Pe & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3B_L, ((u32Pe>>16) & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3C_L, (u32Ps & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3D_L, ((u32Ps>>16) & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3E_L, (u32Drv & 0xFFFF));
    W2BYTE(REG_CHIPTOP_3F_L, ((u32Drv>16) & 0xFFFF));
}

void HalPnlSetMipiDisPadMux(HalPnlMipiDsiLaneMode_e enLaneNum)
{
    W2BYTEMSK(REG_CHIPTOP_09_L,0x0000,0xFFFF);

    W2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0xFFFF);
    if(enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_2)
    {
        W2BYTEMSK(REG_CHIPTOP_0D_L,0x2000,0xFFFF); // MipiDsi 2Lane
    }
    else
    {
        W2BYTEMSK(REG_CHIPTOP_0D_L,0x1000,0xFFFF); // MipiDsi 4Lane
    }
}


void HalPnlSetMCUPadMux(HalPnlMcuDataBusCfg databus, u16 u16Val)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, Bus type: %s, mode: %d\n", __FUNCTION__, __LINE__,
                                PARSING_MCU_BUS_TYPE(databus),u16Val);
    switch(databus)
    {
        case E_HAL_MCU_RGB565_BUS16_CFG:
        case E_HAL_MCU_RGB444_BUS16_CFG:
        case E_HAL_MCU_RGB666_BUS16_CFG:
            W2BYTEMSK(REG_PADTOP_78_L, (u16Val & 0x03)<<2, 0x000c);
            break;
        case E_HAL_MCU_RGB888_BUS8_CFG:
        case E_HAL_MCU_RGB332_BUS8_CFG:
        case E_HAL_MCU_RGB444_BUS8_CFG:
        case E_HAL_MCU_RGB666_BUS8_CFG:
        case E_HAL_MCU_RGB565_BUS8_CFG:
        case E_HAL_MCU_RGB18_BUS8_2_8_8_CFG:
        case E_HAL_MCU_RGB666_BUS8_2_7_CFG:
            W2BYTEMSK(REG_PADTOP_78_L, (u16Val & 0x03)<<0, 0x0003);
            break;
        case E_HAL_MCU_RGB666_BUS18_CFG:
        case E_HAL_MCU_RGB666_BUS18_9_9_CFG:
        case E_HAL_MCU_RGB666_BUS18_2_16_CFG:
        case E_HAL_MCU_RGB666_BUS18_16_2_CFG:
        case E_HAL_MCU_RGB24_BUS18_16_8_CFG:
        case E_HAL_MCU_RGB24_BUS18_8_16_CFG:
            W2BYTEMSK(REG_PADTOP_78_L, (u16Val & 0x03)<<4, 0x0030);
            break;
        case E_HAL_MCU_RGB444_B12_EXT_B16_CFG:
        case E_HAL_MCU_RGB444_B15_4_CFG:
        case E_HAL_MCU_RGBB9_9_17_CFG:
        case E_HAL_MCU_CFG_NOT_SUPPORT:
            PNL_ERR("[HALPNL]%s %d, %d not supported mcu bus config. Set MCU padmux failed\n", __FUNCTION__, __LINE__, databus);
            break;
    }
}

void HalPnlDumpLpllSetting(u16 u16Idx)
{
    u16 u16RegIdx;

    if(u16Idx < E_HAL_PNL_SUPPORTED_LPLL_MAX)
    {
        for(u16RegIdx=0; u16RegIdx < HAL_PNL_LPLL_REG_NUM; u16RegIdx++)
        {

            if(LPLLSettingTBL[u16Idx][u16RegIdx].address == 0xFFFFFFF)
            {
                //DrvSclOsDelayTask(LPLLSettingTBL[u16Idx][u16RegIdx].value);
                continue;
            }

            W2BYTE(LPLLSettingTBL[u16Idx][u16RegIdx].address, LPLLSettingTBL[u16Idx][u16RegIdx].value);
        }
    }
}

void HalPnlSetLpllSet(u32 u32LpllSet)
{
    u16 u16LpllSet_Lo, u16LpllSet_Hi;

    u16LpllSet_Lo = (u16)(u32LpllSet & 0x0000FFFF);
    u16LpllSet_Hi = (u16)((u32LpllSet & 0x00FF0000) >> 16);
    W2BYTE(REG_LPLL_48_L, u16LpllSet_Lo);
    W2BYTE(REG_LPLL_49_L, u16LpllSet_Hi);
}

void HalPnlSetVSyncSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_15_L, REG_DISP_TOP_OP2_1_15_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVSyncEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_16_L, REG_DISP_TOP_OP2_1_16_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVfdeSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_19_L, REG_DISP_TOP_OP2_1_19_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVfdeEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1A_L, REG_DISP_TOP_OP2_1_1A_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVdeSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1F_L, REG_DISP_TOP_OP2_1_1F_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVdeEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_20_L, REG_DISP_TOP_OP2_1_20_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetVtt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_12_L, REG_DISP_TOP_OP2_1_12_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHSyncSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_13_L, REG_DISP_TOP_OP2_1_13_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHSyncEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_14_L, REG_DISP_TOP_OP2_1_14_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHfdeSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_17_L, REG_DISP_TOP_OP2_1_17_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHfdeEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_18_L, REG_DISP_TOP_OP2_1_18_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHdeSt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1D_L, REG_DISP_TOP_OP2_1_1D_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHdeEnd(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1E_L, REG_DISP_TOP_OP2_1_1E_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetHtt(u16 u16Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_11_L, REG_DISP_TOP_OP2_1_11_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x1FFF);
    }
}

void HalPnlSetLpllSkew(u16 u16Val)
{
    W2BYTEMSK(REG_LPLL_44_L, (u16Val << 8), 0xFF00);
}

void HalPnlSetFrameColorEn(bool bEn, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1C_L, REG_DISP_TOP_OP2_1_1C_L) )
    {
        W2BYTEMSK(u32Reg, bEn ? 0x8000 : 0x0000, 0x8000);
    }
}

void HalPnlSetFrameColor(u8 u16R, u16 u16G, u16 u16B, void *pCtx)
{
    u16 u16Val;
    u32 u32Reg = 0;

    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1B_L, REG_DISP_TOP_OP2_1_1B_L) )
    {
        u16Val = (u16B & 0xFF) | (u16)(u16G & 0xFF) << 8;
        W2BYTEMSK(u32Reg, u16Val, 0xFFFF);
    }

    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_1C_L, REG_DISP_TOP_OP2_1_1C_L) )
    {
        u16Val = u16R & 0xFF;
        W2BYTEMSK(u32Reg, u16Val, 0x00FF);
    }
}

void HalPnlSetDeInv(bool bEn)
{
}

void HalPnlSetVsynRefMd(bool bEn)
{
}

void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk)
{
    W2BYTEMSK(u32Reg, u16Val, u16Msk);
}

void HalPnlSetTgenExtHsEn(u8 u8Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_10_L, REG_DISP_TOP_OP2_1_10_L) )
    {
        W2BYTEMSK(u32Reg, u8Val ? 0x0004 : 0x0000, 0x0004);
    }
}

void HalPnlSetSwReste(u8 u8Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_0_00_L, REG_DISP_TOP_1_00_L) )
    {
        W2BYTEMSK(u32Reg, ((u16)u8Val) << 8, 0xFF00);
    }
}

void HalPnlSetDispToDsiMd(u8 u8Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_0_32_L, u8Val ? 0x0001 : 0x0000, 0x0001);
}

void HalPnlSetDacHsyncSt(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_23_L, u16Val, 0x1FFF);
}

void HalPnlSetDacHsyncEnd(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_24_L, u16Val, 0x1FFF);
}

void HalPnlSetDacHdeSt(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_27_L, u16Val, 0x1FFF);
}

void HalPnlSetDacHdeEnd(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_28_L, u16Val, 0x1FFF);
}

void HalPnlSetDacVdeSt(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_29_L, u16Val, 0x1FFF);
}

void HalPnlSetDacVdeEnd(u16 u16Val, void *pCtx)
{
    W2BYTEMSK(REG_DISP_TOP_OP2_0_2A_L, u16Val, 0x1FFF);
}

void HalPnlSetDitherEn(bool bEn, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_76_L, REG_DISP_TOP_OP2_1_76_L) )
    {
        W2BYTEMSK(u32Reg, bEn ? 0x0001 : 0x0000, 0x0001);
    }
}

void HalPnlSetRgbMode(HalPnlOutputFormatBitMode_e enFmt, void *pCtx)
{
    u32 u32Reg = 0;
    u16 u16Val = enFmt == E_HAL_PNL_OUTPUT_565BIT_MODE ? (0x02 << 6) :
                 enFmt == E_HAL_PNL_OUTPUT_6BIT_MODE ?   (0x01 << 6) :
                                                          0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_7E_L, REG_DISP_TOP_OP2_1_7E_L) )
    {
        W2BYTEMSK(u32Reg, u16Val, 0x00C0);
    }
}

void HalPnlSetRgbSwap(HalPnlRgbSwapType_e enChR, HalPnlRgbSwapType_e enChG, HalPnlRgbSwapType_e enChB, void *pCtx)
{
    u16 u16R, u16G, u16B;
    u32 u32Reg = 0;

    u16B = enChB == E_HAL_PNL_RGB_SWAP_B ? 0x01 << 0 :
           enChB == E_HAL_PNL_RGB_SWAP_G ? 0x02 << 0 :
           enChB == E_HAL_PNL_RGB_SWAP_R ? 0x03 << 0 :
                                           0x00;

    u16G = enChG == E_HAL_PNL_RGB_SWAP_B ? 0x01 << 2 :
           enChG == E_HAL_PNL_RGB_SWAP_G ? 0x02 << 2 :
           enChG == E_HAL_PNL_RGB_SWAP_R ? 0x03 << 2 :
                                           0x00;

    u16R = enChR == E_HAL_PNL_RGB_SWAP_B ? 0x01 << 4 :
           enChR == E_HAL_PNL_RGB_SWAP_G ? 0x02 << 4 :
           enChR == E_HAL_PNL_RGB_SWAP_R ? 0x03 << 4 :
                                           0x00;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_7E_L, REG_DISP_TOP_OP2_1_7E_L) )
    {
        W2BYTEMSK(u32Reg, (u16R|u16G|u16B), 0x003F);
    }
}

void HalPnlSetRgbMlSwap(u8 u8Val, void *pCtx)
{
    u32 u32Reg = 0;
    if( _HalPnlGetRegByCtx(pCtx, &u32Reg, REG_DISP_TOP_OP2_0_7E_L, REG_DISP_TOP_OP2_1_7E_L) )
    {
        W2BYTEMSK(u32Reg, u8Val ? 0x0100 : 0x0000, 0x0100);
    }
}

void HalPnlSetSscEn(u8 u8Val)
{
    W2BYTEMSK(REG_LPLL_4E_L, u8Val ? 0x8000 : 0x0000, 0x8000);
}

void HalPnlSetSscSpan(u16 u16Val)
{
    W2BYTEMSK(REG_LPLL_4F_L, u16Val , 0x3FFF);
}

void HalPnlSetSscStep(u16 u16Val)
{
    W2BYTEMSK(REG_LPLL_4E_L, u16Val , 0x0FFF);
}

u32 HalPnlGetLpllSet(void)
{
    u32 u32LpllSet;
    u16 u16LpllSet_Lo, u16LpllSet_Hi;

    u16LpllSet_Lo = R2BYTE(REG_LPLL_48_L);
    u16LpllSet_Hi = R2BYTE(REG_LPLL_49_L);
    u32LpllSet = ((u32)u16LpllSet_Hi) << 16 | u16LpllSet_Lo;
    return u32LpllSet;
}

u32 HalPnlGetRevision(void)
{
    u32 u32Revision;
    u32Revision = R2BYTE(REG_PM_TOP_67_L) & PNL_BIT8 ? CHIP_REVISION_U02 : CHIP_REVISION_U01;
    return u32Revision;
}

//------------------------------------------------------------------------------
// LCD
//------------------------------------------------------------------------------
void HalPnlSetLcdRgbVSyncWidth(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_16_L, ((u16Val & 0xFF) << 8), 0xFF00);
}

void HalPnlSetLcdRgbVBp(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_12_L, u16Val, 0x03FF);
}

void HalPnlSetLcdRgbVPorchTotal(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_13_L, u16Val, 0x03FF);
}

void HalPnlSetLcdRgbVActive(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_25_L, u16Val, 0x1FFF);
}

void HalPnlSetLcdRgbHSyncWidth(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_16_L, u16Val, 0x00FF);
}

void HalPnlSetLcdRgbHBp(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_14_L, u16Val, 0x03FF);
}

void HalPnlSetLcdRgbHPorchTotal(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_15_L, u16Val, 0x03FF);
}

void HalPnlSetLcdRgbHActive(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_24_L, u16Val, 0x1FFF);
}

void HalPnlSetLcdRgbFramePixelCnt(u32 u32Cnt)
{
    u16 u16Val;
    u16Val = (u32Cnt & 0xFFFF);
    W2BYTEMSK(REG_LCD_38_L, u16Val, 0xFFFF);
    u16Val = (u32Cnt & 0xFFFF0000) >> 16;
    W2BYTEMSK(REG_LCD_39_L, u16Val, 0xFFFF);
}

void HalPnlSetLcdRgbDeltaMode(HalPnlRgbDeltaMode_e eOddLine, HalPnlRgbDeltaMode_e eEvenLine)
{
    W2BYTEMSK(REG_LCD_1B_L, 0x0001, 0x0001);
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, sRGB Odd line:%s, Even line:%s\n", __FUNCTION__, __LINE__,
                                PARSING_SRGB_TYPE(eOddLine),PARSING_SRGB_TYPE(eEvenLine));
    W2BYTEMSK(REG_LCD_1B_L, (eOddLine & 0x7) << 2, 0x001C);
    W2BYTEMSK(REG_LCD_1B_L, (eEvenLine & 0x7) << 5, 0x00E0);
}

void HalPnlSetLcdRgbDswap(u8 u8Val)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, Data swap: %d\n", __FUNCTION__, __LINE__, u8Val);
    W2BYTEMSK(REG_LCD_19_L, u8Val ? PNL_BIT6 : 0, PNL_BIT6);
}

void HalPnlSetLcdRgbDtype(u16 u16Val)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, Dtype Bus cfg: %s\n", __FUNCTION__, __LINE__,
                                PARSING_DTYPE_BUS_TYPE(u16Val));
    W2BYTEMSK(REG_LCD_19_L, u16Val, 0x003F);
}

void HalPnlSetLcdRgbEn(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_18_L, u16Val ? 0x0001 : 0x0000, 0x0001);
}

void HalPnlSetLcdRgbVsyncPol(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_18_L, u16Val ? 0x0010 : 0x0000, 0x0010);
}

void HalPnlSetLcdRgbHsyncPol(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_18_L, u16Val ? 0x0020 : 0x0000, 0x0020);
}

void HalPnlSetLcdRgbClkPol(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_18_L, u16Val ? 0x0008 : 0x0000, 0x0008);
}

void HalPnlSetLcdMiscTgenSyncEn(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_MISC_23_L, u16Val ? 0x0010 : 0x0000, 0x0010);
}


void HalPnlSetLcdMiscDacMux(u16 u16Val)
{
    //0:hdmi/dac, 1:dsi mode, 2: cvbs mode
    W2BYTEMSK(REG_LCD_MISC_32_L, u16Val , 0x0003);
}

void HalPnlSetDsiGateEn(u16 u16Val)
{
    W2BYTEMSK(REG_DISP_TOP_0_25_L, u16Val ? 0x0010 : 0x0000, 0x0010);
}



void HalPnlSetLcdDisp1Used(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_10_L, u16Val ? 0x0040 : 0x0000, 0x0040);
}

void HalPnlSetLcdDisp1Type(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_10_L, u16Val, 0x0007);
}

void HalPnlSetLcdBtEncMode(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_60_L, u16Val ? 0x0100 : 0x0000, 0x0100);
}

void HalPnlSetLcdBtClip(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_60_L, u16Val ? 0x0200 : 0x0000, 0x0200);
}

void HalPnlSetLcdBtEncModeEn(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_60_L, u16Val ? 0x8000 : 0x0000, 0x8000);
}

void HalPnlSetLcdBt601En(u16 u16Val)
{
    W2BYTEMSK(REG_LCD_60_L, u16Val ? 0x0004 : 0x0000, 0x0004);
}




//------------------------------------------------------------------------------
// Intel 8080/Motorola 8068 interface
//------------------------------------------------------------------------------



void HalPnlMcuSetBus(HalPnlMcuDataBusCfg databus)
{
    //LCD 1 acitve
    W2BYTEMSK(REG_LCD_3A_L, 2,  0x2);
    switch(databus){
    case E_HAL_MCU_RGB565_BUS16_CFG:
    case E_HAL_MCU_RGB444_BUS16_CFG:
    case E_HAL_MCU_RGB666_BUS16_CFG:
        W2BYTEMSK(REG_LCD_50_L, 0x1, 0x3);
        break;
    case E_HAL_MCU_RGB888_BUS8_CFG:
    case E_HAL_MCU_RGB332_BUS8_CFG:
    case E_HAL_MCU_RGB444_BUS8_CFG:
    case E_HAL_MCU_RGB666_BUS8_CFG:
    case E_HAL_MCU_RGB565_BUS8_CFG:
    case E_HAL_MCU_RGB18_BUS8_2_8_8_CFG:
    case E_HAL_MCU_RGB666_BUS8_2_7_CFG:
        W2BYTEMSK(REG_LCD_50_L, 0, 0x3);
        break;
    case E_HAL_MCU_RGB666_BUS18_CFG:
    case E_HAL_MCU_RGB666_BUS18_9_9_CFG:
    case E_HAL_MCU_RGB666_BUS18_2_16_CFG:
    case E_HAL_MCU_RGB666_BUS18_16_2_CFG:
    case E_HAL_MCU_RGB24_BUS18_16_8_CFG:
    case E_HAL_MCU_RGB24_BUS18_8_16_CFG:
        W2BYTEMSK(REG_LCD_50_L, 0x2, 0x3);
        break;
    case E_HAL_MCU_RGB444_B12_EXT_B16_CFG:
    case E_HAL_MCU_RGB444_B15_4_CFG:
    case E_HAL_MCU_RGBB9_9_17_CFG:
    case E_HAL_MCU_CFG_NOT_SUPPORT:
        PNL_ERR("[HALPNL]%s %d, %d not supported mcu bus config \n", __FUNCTION__, __LINE__, databus);
        break;
    }
    W2BYTE(REG_LCD_52_L, databus);
    W2BYTEMSK(REG_LCD_52_L, (databus==E_HAL_MCU_RGB666_BUS8_CFG) ? 0x1 << 4 : 0, 0x10);

}

void HalPnlMcuCtl(void)
{
    //the usage control register of secondary displays
    //reg_disp1_type set to 0
    W2BYTEMSK(REG_LCD_10_L, 0x40, 0x40);
    //i80 panel
    W2BYTEMSK(REG_LCD_10_L, 0x0, 0x3);
    //lcd_p_en
    W2BYTEMSK(REG_LCD_51_L, 0x1, 0x1);
    W2BYTEMSK(REG_LCD_3C_L, 0x2, 0x2);
}



void __HalPnlSetMcuConfigIDXCmd(bool bIdxCmd)
{
    W2BYTEMSK(REG_LCD_4E_L, (bIdxCmd==1) ? 1 : 0, 1);
}

void _HalPnlSetMcuAutoConfigIDXCmd(u32 u32Val, u8 u8TXNum, bool bIdxCmd)
{
    u32 u32LCDTX_L_ADDR =0;
    u32 u32LCDTX_H_ADDR =0;

    switch(u8TXNum){
    case 1:
        u32LCDTX_L_ADDR = REG_LCD_40_L;
        u32LCDTX_H_ADDR = REG_LCD_41_L;
        break;
    case 2:
        u32LCDTX_L_ADDR = REG_LCD_42_L;
        u32LCDTX_H_ADDR = REG_LCD_43_L;
        break;
    case 3:
        u32LCDTX_L_ADDR = REG_LCD_44_L;
        u32LCDTX_H_ADDR = REG_LCD_45_L;
        break;
    case 4:
        u32LCDTX_L_ADDR = REG_LCD_46_L;
        u32LCDTX_H_ADDR = REG_LCD_47_L;
        break;
    case 5:
        u32LCDTX_L_ADDR = REG_LCD_48_L;
        u32LCDTX_H_ADDR = REG_LCD_49_L;
        break;
    case 6:
        u32LCDTX_L_ADDR = REG_LCD_4A_L;
        u32LCDTX_H_ADDR = REG_LCD_4B_L;
        break;
    case 7:
        u32LCDTX_L_ADDR = REG_LCD_4C_L;
        u32LCDTX_H_ADDR = REG_LCD_4D_L;
        break;
    }
    W2BYTE(u32LCDTX_L_ADDR, ((u32Val) & 0xFFFF));
    W2BYTE(u32LCDTX_H_ADDR, ((u32Val>>16) & 0xFFFF) );
    W2BYTEMSK(REG_LCD_4E_L, bIdxCmd ? (1<<(u8TXNum-1)) : (0<<(u8TXNum-1)), (1<<(u8TXNum-1)) );
}

void HalPnlSetMcuIDXCmdNum( u8 u8TXNum)
{
    W2BYTEMSK(REG_LCD_00_L, 0x40, 0x40);
    W2BYTE(REG_LCD_5E_L, (u8TXNum & 0x7) );
}

void HalPnlSetMcuUnifiedAutoCmdBuff(u32 *pu32CmdBuff, u32 u32CmdNum)
{
    u32     u32ParamIdx=0;
    u32     u32Val=0;
    bool    bIdxCmd=0;
    u8      u8AutoCmdNum=1;
    if(u32CmdNum > 14)
    {
        PNL_ERR("[HALPNL]%s %d  pre-send cmds num (%ld) is over max supported number 7.\n", __FUNCTION__, __LINE__,u32CmdNum);
        return;
    }
    for(u32ParamIdx=0; u32ParamIdx < u32CmdNum ; u32ParamIdx=u32ParamIdx+2)
    {
        u32Val  = pu32CmdBuff[u32ParamIdx];
        bIdxCmd = pu32CmdBuff[u32ParamIdx+1];
        _HalPnlSetMcuAutoConfigIDXCmd(u32Val,u8AutoCmdNum,bIdxCmd);
        u8AutoCmdNum++;
    }
    PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, total cmd num is %d\n",
                                __FUNCTION__, __LINE__,u8AutoCmdNum-1);
    HalPnlSetMcuIDXCmdNum(u8AutoCmdNum-1);
}

void _HalPnlMcuSendCmd(bool bTriggerCmdIdx)
{
    if(bTriggerCmdIdx == 1)
    {
        //trigger command
        W2BYTEMSK(REG_LCD_08_L,0x0, 0x4);
        if((R2BYTEMSK(REG_LCD_0B_L, 0x4))){
            W2BYTEMSK(REG_LCD_0D_L,0x4, 0x4);
        }
        W2BYTEMSK(REG_LCD_00_L, 0x10, 0x10);
        W2BYTEMSK(REG_LCD_08_L,0x4, 0x4);
    }
    else
    {
        //trigger idx
        W2BYTEMSK(REG_LCD_08_L,0x0, 0x2);
        if((R2BYTEMSK(REG_LCD_0B_L, 0x2))){
            W2BYTEMSK(REG_LCD_0D_L,0x2, 0x2);
        }
        W2BYTEMSK(REG_LCD_00_L, 0x4, 0x4);
        W2BYTEMSK(REG_LCD_08_L,0x2, 0x2);
    }
}

void HalPnlSetMcuUnifiedInitCmdBuff(u32 *pu32CmdBuff, u32 u32CmdNum)
{
    u32     u32ParamIdx=0;
    u32     u32Cmd=0;
    u32     u32IdxCmd=0;
    u32     u32AutoCmdNum=1;

    for(u32ParamIdx=0; u32ParamIdx < u32CmdNum ; u32ParamIdx=u32ParamIdx+2)
    {
        u32Cmd  = pu32CmdBuff[u32ParamIdx];
        u32IdxCmd = pu32CmdBuff[u32ParamIdx+1];


        if (u32Cmd == HAL_PNL_MCU_FLAG_END_OF_TABLE)
        {
            PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, MCU_FLAG_END_OF_TABLE\n",
                                __FUNCTION__, __LINE__);
            break;
        }
        else if (u32Cmd == HAL_PNL_MCU_FLAG_DELAY)
        {
            PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, MCU_FLAG_DELAY %ld ms\n",
                                __FUNCTION__, __LINE__,u32IdxCmd);
            DrvPnlOsMsSleep(u32IdxCmd);
        }
        else
        {
            PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, u32Cmd: %lx %s\n",
                                __FUNCTION__, __LINE__,u32Cmd,PARSING_MCU_CMD_TYPE(u32IdxCmd));
            //1:command, 0:idx
            if(u32IdxCmd == 1)
            {
                _HalPnlSetMcuAutoConfigIDXCmd(u32Cmd,2,(bool)u32IdxCmd);
                __HalPnlSetMcuConfigIDXCmd((bool)u32IdxCmd);
                _HalPnlMcuSendCmd(1);
            }
            else
            {
                _HalPnlSetMcuAutoConfigIDXCmd(u32Cmd,1,(bool)u32IdxCmd);
                __HalPnlSetMcuConfigIDXCmd((bool)u32IdxCmd);
                _HalPnlMcuSendCmd(0);
            }
        }
        u32AutoCmdNum++;
    }
    PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, auto cmds is %ld\n",
                                __FUNCTION__, __LINE__,u32AutoCmdNum);
}


void HalPnlSetMcuType(HalPnlMcuType enMcuSys)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d,  MCU system is %s\n", __FUNCTION__, __LINE__, PARSING_MCU_TYPE(enMcuSys));
    switch(enMcuSys){
    case E_HAL_LCD_MCU_68SYS:
        W2BYTEMSK(REG_LCD_50_L, 0x00, 0x40);
        break;
    case E_HAL_LCD_MCU_80SYS:
        W2BYTEMSK(REG_LCD_50_L, 0x40, 0x40);
        break;
    }
}

void HalPnlSetMcuPhase(bool bPHA)
{
     PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, MCU phase = %d\n", __FUNCTION__, __LINE__, bPHA);
     W2BYTEMSK(REG_LCD_50_L, bPHA?0x10:0, 0x10);
}

void HalPnlSetMcuPolar(bool bPOR)
{
     PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, MCU polarity = %d  (0:falling edge, 1: rising edge)\n", __FUNCTION__, __LINE__, bPOR);
     W2BYTEMSK(REG_LCD_50_L, bPOR?0x20:0, 0x20);
}

void HalPnlSetMcuRsPolarity(bool bRsPor)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL,"[HALPNL]%s %d, Rs Polarity=%d [1:high for idx, low for cmd] [0:high for cmd, low for idx]\n",
                                __FUNCTION__, __LINE__,bRsPor);
    W2BYTEMSK(REG_LCD_51_L, (bRsPor == 1)? 0x08:0x0, 0x08);
}

void HalPnlSetMCUIdxCmdTotalNum(u8 u8Num)
{
    W2BYTE(REG_LCD_5E_L, u8Num);
}

void HalPnlSetMCUCycleCnt(u8 u8RSLeadCs, u8 u8CSLeadRW, u8 u8RW_Cnt)
{
    PNL_DBG(PNL_DBG_LEVEL_HAL, "[HALPNL]%s %d, MCU rs lead cs cycle count is %d, MCU cs lead rw cycle count is %d, MCU rw cycle count is %d\n",
                                __FUNCTION__, __LINE__, u8RSLeadCs, u8CSLeadRW,u8RW_Cnt);
    WBYTE(REG_LCD_58_L, u8RSLeadCs);
    WBYTE(REG_LCD_5A_L, u8CSLeadRW);
    WBYTE(REG_LCD_5C_L, u8RW_Cnt);
}


//------------------------------------------------------------------------------
// Mipi DSI

void HalPnlInitMipiDsiDphy(void)
{
    W2BYTE(REG_DPHY_00_L, 0x0001);  // reg_pd_ldo = 0
    DrvPnlOsMsSleep(100);
    W2BYTE(REG_DPHY_01_L, 0x0000);  // reg_pd_dphy=0, reg_pd_ib_dphy = 0
    W2BYTE(REG_DPHY_03_L, 0x0F93);  // turn on CLKFIX_1X_4DIGTAL re_sw_dphy_cken = 1
    W2BYTE(REG_DPHY_14_L, 0x0080);  // reg_dummy0[15:0]
    W2BYTE(REG_DPHY_1C_L, 0xC000);  // switch LP data  path
    W2BYTE(REG_DPHY_1D_L, 0x0080);  // reg_hs_bist_en = 1
    W2BYTE(REG_DPHY_04_L, 0x0000);  // P0/N0
    W2BYTE(REG_DPHY_08_L, 0x0000);  // P1/N1
    W2BYTE(REG_DPHY_0C_L, 0x0000);  // P2/N2
    W2BYTE(REG_DPHY_20_L, 0x0000);  // P3/N3
    W2BYTE(REG_DPHY_23_L, 0x0000);  // P4/N4
    W2BYTE(REG_DPHY_35_L, 0x0000);  // reg_clk_dsi_phy[3:0] = 0

    W2BYTEMSK(REG_DPHY_06_L, 0x0000, 0x001E);
    W2BYTEMSK(REG_DPHY_0A_L, 0x0000, 0x001E);
    W2BYTEMSK(REG_DPHY_0E_L, 0x0000, 0x001E);
    W2BYTEMSK(REG_DPHY_22_L, 0x0000, 0x001E);
    W2BYTEMSK(REG_DPHY_25_L, 0x0000, 0x001E);
}


void HalPnlSetMipiDsiPadOutSel(HalPnlMipiDsiLaneMode_e enLaneMode)
{
    // setting by IPL
}

void HalPnlResetMipiDsi(void)
{
    W4BYTE(DSI_REG_BASE +(REG_DSI_COM_CON>>1), DSI_DSI_RESET | DSI_DPHY_RESET);
    W4BYTE(DSI_REG_BASE +(REG_DSI_COM_CON>>1), 0);
}

void HalPnlEnableMipiDsiClk(void)
{
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_START>>1), DSI_DSI_START, DSI_DSI_START);
}

void HalPnlDisableMipiDsiClk(void)
{
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START);
}

void HalPnlSetMipiDsiLaneNum(HalPnlMipiDsiLaneMode_e enLaneMode)
{
    switch(enLaneMode)
    {
        case E_HAL_PNL_MIPI_DSI_LANE_1:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x1<<2, DSI_LANE_NUM);
            break;
        case E_HAL_PNL_MIPI_DSI_LANE_2:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x3<<2, DSI_LANE_NUM);
            break;
        case E_HAL_PNL_MIPI_DSI_LANE_3:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x7<<2, DSI_LANE_NUM);
            break;
        case E_HAL_PNL_MIPI_DSI_LANE_4:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0xF<<2, DSI_LANE_NUM);
            break;
        case E_HAL_PNL_MIPI_DSI_LANE_NONE:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), 0x0<<2, DSI_LANE_NUM);
            break;
    }
}

void HalPnlSetMipiDsiCtrlMode(HalPnlMipiDsiCtrlMode_e enCtrlMode)
{
    HalPnlResetMipiDsi();

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: Ctrl=%s\n",
        __FUNCTION__, __LINE__, PARSING_HAL_MIPI_DSI_CTRL(enCtrlMode));

    switch(enCtrlMode)
    {
        case E_HAL_PNL_MIPI_DSI_CMD_MODE:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_CMD, DSI_MODE_MASK);
            break;

        case E_HAL_PNL_MIPI_DSI_SYNC_PULSE:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_VID_SYNC_PULSE, DSI_MODE_MASK);
            break;

        case E_HAL_PNL_MIPI_DSI_SYNC_EVENT:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_VID_SYNC_EVENT, DSI_MODE_MASK);
            break;

        case E_HAL_PNL_MIPI_DSI_BURST_MODE:
            W4BYTEMSK(DSI_REG_BASE +(REG_DSI_MODE_CON>>1), DSI_MODE_CON_BURST, DSI_MODE_MASK);
            break;
    }
}

bool HalPnlGetMipiDsiClkHsMode(void)
{
    bool bHs = R4BYTEMSK(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), DSI_LC_HSTX_EN) ? 1 : 0;
    return bHs;
}

void HalPnlSetMpiDsiClkHsMode(bool bEn)
{
    if(bEn && !HalPnlGetMipiDsiClkHsMode())
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), 1, DSI_LC_HSTX_EN); //
    }
    else if (!bEn && HalPnlGetMipiDsiClkHsMode())
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PHY_LCCON>>1), 0, DSI_LC_HSTX_EN); //
    }
}

bool HalPnlGetMipiDsiShortPacket(u8 u8ReadBackCount, u8 u8RegAddr)
{

    HalPnlMipiDsiT0Ins_t t0 = {0, 0, 0, 0};
    u32 u32DsiIntSta=0;
    u32 u32RXData;
    u32 u32T0Data;

    //Step1: Send Maxmum return packet with expected returned byte count
    t0.u8Confg= 0;
    t0.u8DataId=HAL_PNL_MIPI_DSI_DCS_MAXIMUM_RETURN_PACKET_SIZE ;
    t0.u8Data0 =u8ReadBackCount;

    //clear cmd done flag first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    u32T0Data = HAL_PNL_MERGE_MIPIDSI_T0INS(t0);
    W4BYTE(DSI_CMDQ_BASE + 0,  u32T0Data);
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    //wait cmd done flag
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    //Step2: Set Tx Maxmum return packet size
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_TXRX_CON>>1), u8ReadBackCount<<12, DSI_MAX_RTN_SIZE);

     //Step3: Send DCS long read cmd

    t0.u8Confg= 0x03;
    t0.u8DataId=HAL_PNL_MIPI_DSI_DCS_READ_PACKET_ID ;
    t0.u8Data0 =u8RegAddr;

    u32T0Data = HAL_PNL_MERGE_MIPIDSI_T0INS(t0);
    W4BYTE(DSI_CMDQ_BASE + 0,  u32T0Data);
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);
     //wait cmd done flag
    do{
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG));
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA03>>1));

    if( ((u32RXData & 0xFF) == 0x21 ) ||  ((u32RXData & 0xFF) == 0x22 ))
    {
        PNL_DBG(PNL_DBG_LEVEL_HAL, "DCS Short Read response Id: %lx\n",(u32RXData & 0xFF));
    }

    PNL_DBG(PNL_DBG_LEVEL_HAL, "Byte0~3 =%lx\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA47>>1));
    PNL_DBG(PNL_DBG_LEVEL_HAL, "Byte4~7 =%lx\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATA8B>>1));
    PNL_DBG(PNL_DBG_LEVEL_HAL, "Byte8~b =%lx\n",u32RXData);
    u32RXData = R4BYTE(DSI_REG_BASE + (REG_DSI_RX_DATAC>>1));
    PNL_DBG(PNL_DBG_LEVEL_HAL, "Bytec~f =%lx\n",u32RXData);

   return 1;
}

bool HalPnlSetMipiDsiShortPacket(HalPnlMipiDsiPacketType_e enPacketType, u8 u8Count, u8 u8Cmd, u8 *pu8ParamList)
{
    u16 u16CmdqIdx=0;
    //PDSI_T0_INS ptrT0;
    HalPnlMipiDsiCmdqConfig_t stCmdqCfg;
    HalPnlMipiDsiT0Ins_t stT0 = {0, 0, 0, 0};
    u32 u32DsiIntSta = 0;
    u32 u32TimeOut = 0xFF;
    u32 u32T0Data;

    u16CmdqIdx     = 0;

    stCmdqCfg.u8Bta  = 0;
    stCmdqCfg.u8Hs   = 0; //Low power mode transfer
    stCmdqCfg.u8Type = 0; //type0
    stCmdqCfg.u8Te   = 0;
    stCmdqCfg.u8Rsv  = 0;
    stCmdqCfg.u8Cl   = 0;
    stCmdqCfg.u8Rpt  = 0;

    stCmdqCfg = stCmdqCfg;

    stT0.u8Confg= 0;

    if((pu8ParamList == NULL)  && (u8Count != 0))
    {
        return 0;
    }

    if(u8Count > 2)
    {
        PNL_ERR("%s %d, Count is more than 2 (%d)", __FUNCTION__, __LINE__, u8Count);
        return 0;
    }
    else
    {
        if(u8Count==2)
        {
             stT0.u8DataId =
                enPacketType == E_HAL_PNL_MIPI_DSI_PACKET_TYPE_GENERIC ?
                                HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_1 :
                                HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_1 ;

             stT0.u8Data0  = u8Cmd;
             stT0.u8Data1  = *(pu8ParamList);
        }
        else if(u8Count==1)
        {
             stT0.u8DataId =
                 enPacketType == E_HAL_PNL_MIPI_DSI_PACKET_TYPE_GENERIC ?
                                 HAL_PNL_MIPI_DSI_GERNERIC_SHORT_PACKET_ID_0 :
                                 HAL_PNL_MIPI_DSI_DCS_SHORT_PACKET_ID_0;
             stT0.u8Data0  = u8Cmd;
             stT0.u8Data1  = 0;
        }
    }

    PNL_DBG(PNL_DBG_LEVEL_RW_PACKET, "%s %d: stT0 = %08lx\n", __FUNCTION__, __LINE__, HAL_PNL_MERGE_MIPIDSI_T0INS(stT0));

    //clear u8Cmd done flag first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    u32T0Data = HAL_PNL_MERGE_MIPIDSI_T0INS(stT0);
    W4BYTE(DSI_CMDQ_BASE +(u16CmdqIdx*2),  u32T0Data);

    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), 1, DSI_CMDQ_SIZE);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    do
    {
        DrvPnlOsUsSleep(1);
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
        u32TimeOut--;
     } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG) && u32TimeOut);
      WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    if(u32TimeOut == 0)
    {
        PNL_ERR("%s %d: CMD Done Time Out, DsiIntSta=%lx\n",  __FUNCTION__,  __LINE__, u32DsiIntSta);
    }

    #if 0
    W4BYTEMSK(DSI_CMDQ_BASE +u16Offset, 0, DSI_TYPE);

    W4BYTEMSK(DSI_CMDQ_BASE +u16Offset,  dataID<<8, DSI_DATA_ID);
    WriteLongRegBit(DSI_CMDQ_BASE +u16Offset, 0, DSI_HS);
    WriteLongRegBit(DSI_CMDQ_BASE +u16Offset, 0, DSI_BTA);

    W4BYTEMSK(DSI_CMDQ_BASE +u16Offset,  data0<<16, DSI_DATA_0);


    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), u8Count, DSI_CMDQ_SIZE);

    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);
    #endif

    return 1;
}


bool HalPnlSetMipiDsiLongPacket(HalPnlMipiDsiPacketType_e enPacketType, u8 u8Count, u8 u8Cmd, u8 *pu8ParamList)
{
    u8  i;
    //u16 u16Offset = 0;
    u8  u8CmdStIdx = 0;
    HalPnlMipiDsiT2Ins_t stT2;
    HalPnlMipiDsiCmdqConfig_t stCmdqCfg;
    HalPnlMipiDsiCmdq_t stCmdqDataSeq;
    u8 u8CMDQNum = 0;
    u32 u32DsiIntSta = 0;
    bool bFirstCmd = 0;
    u32 u32TimeOut = 0xFF;
    u32 u32Data;

    if(pu8ParamList == NULL)
    {
        PNL_ERR("%s %d: pu8ParamList NULL \n", __FUNCTION__, __LINE__);
        return 0;
    }

    stCmdqCfg.u8Bta   = 0;
    stCmdqCfg.u8Hs    = 0; //Low power mode transfer
    stCmdqCfg.u8Type  = 2; //type2
    stCmdqCfg.u8Te    = 0;
    stCmdqCfg = stCmdqCfg;

    stT2.u8Confg = 0x02;
    stT2.u8DataId =
        enPacketType == E_HAL_PNL_MIPI_DSI_PACKET_TYPE_GENERIC ?
                        HAL_PNL_MIPI_DSI_GERNERIC_LONG_WRITE_PACKET_ID :
                        HAL_PNL_MIPI_DSI_DCS_LONG_WRITE_PACKET_ID;
    stT2.u16Wc = u8Count;

    u32Data = HAL_PNL_MERGE_MIPIDSI_T2INS(stT2);
    W4BYTE(DSI_CMDQ_BASE +(0),  u32Data);

    //u16Offset = 1;
    u8CMDQNum = (u8Count/4);
    u8CmdStIdx = 1;

    for(i=0; i <u8CMDQNum ; i++)
    {
        if(!bFirstCmd)
        {
            stCmdqDataSeq.byte0 = u8Cmd;
            bFirstCmd = 1;
        }
        else
        {
            stCmdqDataSeq.byte0 = *pu8ParamList;
            pu8ParamList += 1;
        }
        stCmdqDataSeq.byte1 = *pu8ParamList;
        pu8ParamList += 1;
        stCmdqDataSeq.byte2 = *pu8ParamList;
        pu8ParamList += 1;
        stCmdqDataSeq.byte3 = *pu8ParamList;
        pu8ParamList += 1;

        W4BYTE(DSI_CMDQ_BASE +((u8CmdStIdx)*2),  AS_UINT32(&stCmdqDataSeq));
        u8CmdStIdx++;

        //u8Index++;
    }

    u8CMDQNum += (u8Count % 4) ? 1 : 0;

    stCmdqDataSeq.byte0 = 0x0;
    stCmdqDataSeq.byte1 = 0x0;
    stCmdqDataSeq.byte2 = 0x0;
    stCmdqDataSeq.byte3 = 0x0;

    if((u8Count % 4) == 3)
    {
        if(!bFirstCmd)
        {
            stCmdqDataSeq.byte0=u8Cmd;
            bFirstCmd = 1;
        }
        else
        {
            stCmdqDataSeq.byte0=*pu8ParamList;
            pu8ParamList += 1;
        }
        stCmdqDataSeq.byte1 = *pu8ParamList;
        pu8ParamList += 1;
        stCmdqDataSeq.byte2 = *pu8ParamList;
        pu8ParamList += 1;
       //u8CmdStIdx++;

        if(u8Count == 3)
        {
            u8CmdStIdx = 1;
        }
        u32Data = HAL_PNL_MERGE_MIPIDSI_CMDQ(stCmdqDataSeq);
        W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  u32Data);
    }
    else if((u8Count%4) == 2)
    {
        if(!bFirstCmd)
        {
            stCmdqDataSeq.byte0 = u8Cmd;
            bFirstCmd = 1;
        }
        else
        {
            stCmdqDataSeq.byte0 = *pu8ParamList;
            pu8ParamList += 1;
        }
        stCmdqDataSeq.byte1 = *pu8ParamList;
        pu8ParamList += 1;

        // u8CmdStIdx++;
        u32Data = HAL_PNL_MERGE_MIPIDSI_CMDQ(stCmdqDataSeq);
        W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  u32Data);
    }
    else if((u8Count%4) == 1)
    {
         if(!bFirstCmd)
         {
            stCmdqDataSeq.byte0 = u8Cmd;
            bFirstCmd = 1;
        }
        else
        {
            stCmdqDataSeq.byte0=*pu8ParamList;
            pu8ParamList+=1;
        }
        //pstCmdqDataSeq[u8Index]=stCmdqDataSeq ;
       // u8CmdStIdx++;
       u32Data = HAL_PNL_MERGE_MIPIDSI_CMDQ(stCmdqDataSeq);
       W4BYTE(DSI_CMDQ_BASE +(u8CmdStIdx*2),  u32Data);
    }

    u8CMDQNum += 1;
    #if 0
        {
    UartSendTrace("------------------------------------\n");
    UartSendTrace("Bank Addr : 0x1A2200     \n");
    UartSendTrace("------------------------------------\n");
    u16Offset =0;

    UartSendTrace("        ", 0);
    for(i=0 ; i < 8; i++) {
        UartSendTrace("%08X ",i*4);
    }
    UartSendTrace("\n");
    for(i=0 ; i < 16; i++) {
        UartSendTrace("%03X     ", u16Offset<<1);
        for(j=0; j<8 ; j++) {
            UartSendTrace("%08X ", Hal_MIPI_DSI_RegisterRead(0x1A2200+u16Offset));
            u16Offset +=2;
        }
        UartSendTrace("  \n");
    }
    }
#endif
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_CMDQ_CON>>1), u8CMDQNum, DSI_CMDQ_SIZE);

    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 0, DSI_DSI_START); //clear first
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_START>>1), 1, DSI_DSI_START);

    do
    {
        DrvPnlOsUsSleep(1);
        u32DsiIntSta =R4BYTE(DSI_REG_BASE + (REG_DSI_INTSTA>>1));
        u32TimeOut --;
    } while( !(u32DsiIntSta & DSI_CMD_DONE_INT_FLAG) && u32TimeOut);
    WriteLongRegBit(DSI_REG_BASE +(REG_DSI_INTSTA>>1), 0, DSI_CMD_DONE_INT_FLAG);

    if(u32TimeOut == 0)
    {
      PNL_ERR("%s %d: CMD(%x),Cnt:%d Done Time Out, DsiIntSta=%lx\n",
        __FUNCTION__, __LINE__, u8Cmd, u8Count, u32DsiIntSta);
    }
    else
    {
        PNL_DBG(PNL_DBG_LEVEL_RW_PACKET,"%s %d, CMD(%x), Cnt:%d, TimeOut:%ld\n",
            __FUNCTION__, __LINE__, u8Cmd, u8Count, 0xFF-u32TimeOut);
    }
    //_HalIsEngineBusy();
    return 1;
}


bool HalPnlSetMipiDsiPhyTimConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg)
{

    HalPnlMipiDsiPhyTimCon0Reg_t stTimCon0 = {0, 0, 0, 0};
    //HalPnlMipiDsiPhyTimCon0Reg_t *pstTimCon0;
    HalPnlMipiDsiPhyTimCon1Reg_t stTimCon1 = {0, 0, 0, 0};
    //HalPnlMipiDsiPhyTimCon1Reg_t *pstTimCon1;
    HalPnlMipiDsiPhyTimCon2Reg_t stTimCon2 = {0, 0, 0, 0};
    //HalPnlMipiDsiPhyTimCon2Reg_t *pstTimCon2;

    HalPnlMipiDsiPhyTimCon3Reg_t stTimCon3 = {0, 0, 0, 0};
    //HalPnlMipiDsiPhyTimCon3Reg_t *pstTimCon3;
    u32 lane_no = pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_4 ? 4 :
                  pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_3 ? 3 :
                  pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_2 ? 2 :
                                                                          1;

    //unsigned int div2_real;
    u32 cycle_time;
    u32 ui;
    u32 u32Data;
    //u32 hs_trail_m, hs_trail_n;
    //u32 div1 = 0;
    //u32 div2 = 0;
    //u32 fbk_sel = 0;
    //u32 pre_div = 0;
    //u32 fbk_div = 0;

#if 0
    //	div2_real=div2 ? div2*0x02 : 0x1;
    cycle_time = (8 * 1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2);
    ui = (1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2) + 1;

    //DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, Cycle Time = %d(ns), Unit Interval = %d(ns). div1 = %d, div2 = %d, fbk_div = %d, lane# = %d \n", cycle_time, ui, div1, div2, fbk_div, lane_no);

#define NS_TO_CYCLE(n, c)	((n) / c + (( (n) % c) ? 1 : 0))

    hs_trail_m=lane_no;
    hs_trail_n= (pstMipiDsiCfg->dsi.u8HsTrail == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : pstMipiDsiCfg->dsi.u8HsTrail;

    // +3 is recommended from designer becauase of HW latency
    stTimCon0.u8HsTrail	= ((hs_trail_m > hs_trail_n) ? hs_trail_m : hs_trail_n) + 0x0a;

    stTimCon0.u8HsPrpr 	= (pstMipiDsiCfg->dsi.u8HsPrpr == 0) ? NS_TO_CYCLE((60 + 5 * ui), cycle_time) : pstMipiDsiCfg->dsi.u8HsPrpr;
    // u8HsPrpr can't be 1.
    if (stTimCon0.u8HsPrpr == 0)
    	stTimCon0.u8HsPrpr = 1;

    stTimCon0.u8HsZero 	= (pstMipiDsiCfg->dsi.u8HsZero == 0) ? NS_TO_CYCLE((0xC8 + 0x0a * ui - stTimCon0.u8HsPrpr * cycle_time), cycle_time) : pstMipiDsiCfg->dsi.u8HsZero;

    stTimCon0.u8Lpx 		= (pstMipiDsiCfg->dsi.u8Lpx == 0) ? NS_TO_CYCLE(65, cycle_time) : pstMipiDsiCfg->dsi.u8Lpx;
    if(stTimCon0.u8Lpx == 0) stTimCon0.u8Lpx = 1;
    //	stTimCon1.TA_SACK 	= (pstMipiDsiCfg->dsi.TA_SACK == 0) ? 1 : pstMipiDsiCfg->dsi.TA_SACK;
    stTimCon1.u8TaGet 		= (pstMipiDsiCfg->dsi.u8TaGet == 0) ? (5 * stTimCon0.u8Lpx) : pstMipiDsiCfg->dsi.u8TaGet;
    stTimCon1.u8TaSure 	= (pstMipiDsiCfg->dsi.u8TaSure == 0) ? (3 * stTimCon0.u8Lpx / 2) : pstMipiDsiCfg->dsi.u8TaSure;
    stTimCon1.u8TaGo 		= (pstMipiDsiCfg->dsi.u8TaGo == 0) ? (4 * stTimCon0.u8Lpx) : pstMipiDsiCfg->dsi.u8TaGo;
    stTimCon1.u8DaHsExit  = (pstMipiDsiCfg->dsi.u8DaHsExit == 0) ? (2 * stTimCon0.u8Lpx) : pstMipiDsiCfg->dsi.u8DaHsExit;

    stTimCon2.u8ClkTrail 	= ((pstMipiDsiCfg->dsi.u8ClkTrail == 0) ? NS_TO_CYCLE(0x64, cycle_time) : pstMipiDsiCfg->dsi.u8ClkTrail) + 0x0a;
    // u8ClkTrail can't be 1.
    if (stTimCon2.u8ClkTrail < 2)
        stTimCon2.u8ClkTrail = 2;

    //  imcon2.LPX_WAIT 	= (pstMipiDsiCfg->dsi.LPX_WAIT == 0) ? 1 : pstMipiDsiCfg->dsi.LPX_WAIT;
    stTimCon2.u8ContDet 	= pstMipiDsiCfg->dsi.u8ContDet;

    stTimCon3.u8ClkHsPrpr	= (pstMipiDsiCfg->dsi.u8ClkHsPrpr == 0) ? NS_TO_CYCLE(0x40, cycle_time) : pstMipiDsiCfg->dsi.u8ClkHsPrpr;
    if(stTimCon3.u8ClkHsPrpr == 0) stTimCon3.u8ClkHsPrpr = 1;

    stTimCon2.u8ClkZero	= (pstMipiDsiCfg->dsi.u8ClkZero == 0) ? NS_TO_CYCLE(0x190 - stTimCon3.u8ClkHsPrpr * cycle_time, cycle_time) : pstMipiDsiCfg->dsi.u8ClkZero;

    stTimCon3.u8ClkHsExit= (pstMipiDsiCfg->dsi.u8ClkHsExit == 0) ? (2 * stTimCon0.u8Lpx) : pstMipiDsiCfg->dsi.u8ClkHsExit;

    stTimCon3.u8ClkHsPost= (pstMipiDsiCfg->dsi.u8ClkHsPost == 0) ? NS_TO_CYCLE((80 + 52 * ui), cycle_time) : pstMipiDsiCfg->dsi.u8ClkHsPost;

    DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, u8HsTrail = %d, u8HsZero = %d, u8HsPrpr = %d, u8Lpx = %d, u8TaGet = %d, u8TaSure = %d, u8TaGo = %d, u8ClkTrail = %d, u8ClkZero = %d, u8ClkHsPrpr = %d \n", \
        stTimCon0.u8HsTrail, stTimCon0.u8HsZero, stTimCon0.u8HsPrpr, stTimCon0.u8Lpx, stTimCon1.u8TaGet, stTimCon1.u8TaSure, stTimCon1.u8TaGo, stTimCon2.u8ClkTrail, stTimCon2.u8ClkZero, stTimCon3.u8ClkHsPrpr);

    //DSI_REG->DSI_PHY_TIMECON0=stTimCon0;
    //DSI_REG->DSI_PHY_TIMECON1=stTimCon1;
    //DSI_REG->DSI_PHY_TIMECON2=stTimCon2;
    //DSI_REG->DSI_PHY_TIMECON3=stTimCon3;
    OUTREG32(&DSI_REG->DSI_PHY_TIMECON0,AS_UINT32(&stTimCon0));
    OUTREG32(&DSI_REG->DSI_PHY_TIMECON1,AS_UINT32(&stTimCon1));
    OUTREG32(&DSI_REG->DSI_PHY_TIMECON2,AS_UINT32(&stTimCon2));
    OUTREG32(&DSI_REG->DSI_PHY_TIMECON3,AS_UINT32(&stTimCon3));
    dsi_cycle_time = cycle_time;
#else
        //	div2_real=div2 ? div2*0x02 : 0x1;

    #if 0
    cycle_time = (8 * 1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2);
    ui = (1000 * div2 * div1 * pre_div)/ (26 * (fbk_div+0x01) * fbk_sel * 2) + 1;
    #else

    #endif

    //DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] - kernel - DSI_PHY_TIMCONFIG, Cycle Time = %d(ns), Unit Interval = %d(ns). div1 = %d, div2 = %d, fbk_div = %d, lane# = %d \n", cycle_time, ui, div1, div2, fbk_div, lane_no);

#define NS_TO_CYCLE(n, c)	((n*1000) / c + (( (n) % c) ? 1 : 0))

    ui = _HalPnlGetMipiUi(pstMipiDsiCfg);
    cycle_time = ui*8;
     /*
     0x05, // HS_TRAIL       60+4UI ~ MAX
     0x03, // HS_PRPR        40+4UI ~ 85+6UI
     0x05, // HS_ZERO        105+6UI ~ MAX
     0x0A, // CLK_HS_PRPR    38ns ~ 95ns
     0x0E, // CLK_HS_EXIT    100ns ~ max
     0x03, // CLK_TRAIL      60ns ~ max
     0x0b, // CLK_ZERO       300ns -CLK_HS_PRPR
     0x0A, // CLK_HS_POST    60+52UI ~ max
     0x05, // DA_HS_EXIT     100ns ~ max
     0x00, // CONT_DET
     0x0A, // LPX            50ns ~ MAX  ===>62.5ns
     0x1a, // TA_GET         5LPX
     0x16, // TA_SURE        1LPX ~ 2LPX
     0x32, // TA_GO          4LPX
     */
     PNL_DBG(PNL_DBG_LEVEL_HAL, "Ui=%lx, Cycle=%lx\n", ui, cycle_time);

     PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HsTrail=%lx HsPrpr=%lx HsZero=%lx ClkHsPrpr=%lx ClkHsExit=%lx ClkTrail=%lx ClkZero=%lx ClkHsPost=%lx DahsExit=%lx Lpx=%lx TaGet=%lx TaSure=%lx TaGo=%lx\n",
        __FUNCTION__, __LINE__,
         ((u32)(70000UL+(4*ui))/cycle_time),  // HsTrail
         ((u32)(62500UL+(5*ui))/cycle_time),  // HsPrpr
         ((u32)(105000UL+(7*ui))/cycle_time), // HsZero
         ((u32)65000UL/cycle_time),           // ClkHsPrPr
         ((u32)200000UL/cycle_time),          // ClkHsExit
         ((u32)120000UL/cycle_time),          // ClkTrai
         ((u32)300000UL/cycle_time),          // ClkZero
         ((u32)(70000UL+(54*ui))/cycle_time), // ClkHsPost
         ((u32)200000UL/cycle_time),          // DahsExit
         ((u32)62500UL/cycle_time),           // Lpx
         ((u32)(5*62500UL)/cycle_time),       // TaGet
         ((u32)(3*62500UL/2)/cycle_time),     // TaSure
         ((u32)(4*62500UL)/cycle_time)        // TaGo
        );

#if 0  //TBD
    hs_trail_m=lane_no;
    hs_trail_n= (pstMipiDsiCfg->u8HsTrail == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : pstMipiDsiCfg->u8HsTrail;

    // +3 is recommended from designer becauase of HW latency
    stTimCon0.u8HsTrail	= ((hs_trail_m > hs_trail_n) ? hs_trail_m : hs_trail_n) + 0x0a;
#else
    stTimCon0.u8HsTrail    =(pstMipiDsiCfg->u8HsTrail == 0) ? NS_TO_CYCLE(((lane_no * 4 * ui) + 60), cycle_time) : pstMipiDsiCfg->u8HsTrail;
#endif

    stTimCon0.u8HsPrpr     = (pstMipiDsiCfg->u8HsPrpr == 0) ? NS_TO_CYCLE((60 + 5 * ui), cycle_time) : pstMipiDsiCfg->u8HsPrpr;

    // u8HsPrpr can't be 1.
    if (stTimCon0.u8HsPrpr == 0)
    {
        stTimCon0.u8HsPrpr = 1;
    }

    stTimCon0.u8HsZero 	= (pstMipiDsiCfg->u8HsZero == 0) ? NS_TO_CYCLE((0xC8 + 0x0a * ui - stTimCon0.u8HsPrpr * cycle_time), cycle_time) : pstMipiDsiCfg->u8HsZero;

    stTimCon0.u8Lpx     = (pstMipiDsiCfg->u8Lpx == 0) ? NS_TO_CYCLE(130, cycle_time) : pstMipiDsiCfg->u8Lpx;
    if(stTimCon0.u8Lpx == 0) stTimCon0.u8Lpx = 1;

    //stTimCon1.TA_SACK 	= (pstMipiDsiCfg->dsi.TA_SACK == 0) ? 1 : pstMipiDsiCfg->dsi.TA_SACK;
    stTimCon1.u8TaGet      = (pstMipiDsiCfg->u8TaGet == 0) ? (5 * stTimCon0.u8Lpx) : pstMipiDsiCfg->u8TaGet;
    stTimCon1.u8TaSure     = (pstMipiDsiCfg->u8TaSure == 0) ? (3 * stTimCon0.u8Lpx / 2) : pstMipiDsiCfg->u8TaSure;
    stTimCon1.u8TaGo       = (pstMipiDsiCfg->u8TaGo == 0) ? (4 * stTimCon0.u8Lpx) : pstMipiDsiCfg->u8TaGo;
    stTimCon1.u8DaHsExit  = (pstMipiDsiCfg->u8DaHsExit == 0) ? (2 * stTimCon0.u8Lpx) : pstMipiDsiCfg->u8DaHsExit;

    stTimCon2.u8ClkTrail 	= ((pstMipiDsiCfg->u8ClkTrail == 0) ? NS_TO_CYCLE(0x64, cycle_time) : pstMipiDsiCfg->u8ClkTrail) + 0x0a;
    // u8ClkTrail can't be 1.
    if (stTimCon2.u8ClkTrail < 2)
    {
        stTimCon2.u8ClkTrail = 2;
    }

    //  imcon2.LPX_WAIT 	= (pstMipiDsiCfg->dsi.LPX_WAIT == 0) ? 1 : pstMipiDsiCfg->dsi.LPX_WAIT;
    stTimCon2.u8ContDet 	= pstMipiDsiCfg->u8ContDet;

    stTimCon3.u8ClkHsPrpr	= (pstMipiDsiCfg->u8ClkHsPrpr == 0) ? NS_TO_CYCLE(0x40, cycle_time) : pstMipiDsiCfg->u8ClkHsPrpr;

    if(stTimCon3.u8ClkHsPrpr == 0)
    {
        stTimCon3.u8ClkHsPrpr = 1;
    }

    stTimCon2.u8ClkZero	= (pstMipiDsiCfg->u8ClkZero == 0) ? NS_TO_CYCLE(0x190 - stTimCon3.u8ClkHsPrpr * cycle_time, cycle_time) : pstMipiDsiCfg->u8ClkZero;
    stTimCon2.RSV8 =0x01;
    stTimCon3.u8ClkHsExit= (pstMipiDsiCfg->u8ClkHsExit == 0) ? (2 * stTimCon0.u8Lpx) : pstMipiDsiCfg->u8ClkHsExit;

    stTimCon3.u8ClkHsPost= (pstMipiDsiCfg->u8ClkHsPost == 0) ? NS_TO_CYCLE((80 + 52 * ui), cycle_time) : pstMipiDsiCfg->u8ClkHsPost;

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HsTrail=%x, HsZero=%x, HsPrpr=%x, Lpx=%x TaGet=%x, TaSure=%x, TaGo=%x, ClkTrail=%x, u8ClkZero=%x, ClkHsPrpr=%x \n",
        __FUNCTION__, __LINE__,
        stTimCon0.u8HsTrail, stTimCon0.u8HsZero, stTimCon0.u8HsPrpr, stTimCon0.u8Lpx,
        stTimCon1.u8TaGet, stTimCon1.u8TaSure, stTimCon1.u8TaGo,
        stTimCon2.u8ClkTrail, stTimCon2.u8ClkZero, stTimCon3.u8ClkHsPrpr);

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: TimeCon: %08lx %08lx %08lx %08lx\n",
        __FUNCTION__, __LINE__,
        HAL_PNL_MERGE_MIPIDSI_TIMECON0(stTimCon0),
        HAL_PNL_MERGE_MIPIDSI_TIMECON1(stTimCon1),
        HAL_PNL_MERGE_MIPIDSI_TIMECON2(stTimCon2),
        HAL_PNL_MERGE_MIPIDSI_TIMECON3(stTimCon3));

    u32Data = HAL_PNL_MERGE_MIPIDSI_TIMECON0(stTimCon0);
    W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON0>>1),  u32Data);

    u32Data = HAL_PNL_MERGE_MIPIDSI_TIMECON1(stTimCon1);
    W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON1>>1), u32Data);

    u32Data = HAL_PNL_MERGE_MIPIDSI_TIMECON2(stTimCon2);
    W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON2>>1), u32Data);

    u32Data = HAL_PNL_MERGE_MIPIDSI_TIMECON3(stTimCon3);
    W4BYTE(DSI_REG_BASE + (REG_DSI_PHY_TIMCON3>>1), u32Data);

#if 0
    W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON0>>1), (u32) stTimCon0);
    W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON1>>1), (u32) stTimCon1);
    W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON2>>1), (u32) stTimCon2);
    W4BYTE(DSI_REG_BASE +(REG_DSI_PHY_TIMCON3>>1), (u32) stTimCon3);
#endif
    //dsi_cycle_time = cycle_time;

#endif
    return 1;
}

bool HalPnlSetMipiDsiVideoTimingConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg, HalPnlHwMipiDsiConfig_t *pstHwCfg)
{
    //unsigned int line_byte;
    u32 horizontal_sync_active_byte = 0;
    u32 horizontal_backporch_byte = 0;
    u32 horizontal_frontporch_byte = 0;
    u32 horizontal_active_byte = 0;
    u32 horizontal_bllp_byte;
    u32 dsiTmpBufBpp;
    u32 u32LaneNum = pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_4 ? 4 :
                     pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_3 ? 3 :
                     pstMipiDsiCfg->enLaneNum == E_HAL_PNL_MIPI_DSI_LANE_2 ? 2 :
                                                                             1 ;
    if(pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB565)
    {
        dsiTmpBufBpp = 16;
    }
    else if( (pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB888 ||
              pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_LOOSELY_RGB666))
    {
        dsiTmpBufBpp = 24;
    }
    else if( (pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB666) )
    {
        dsiTmpBufBpp = 18;
    }
    else
    {
        dsiTmpBufBpp = 24;
    }

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: Lane:%ld, Bpp:%s(%ld), Ctrl:%s\n",
        __FUNCTION__, __LINE__,
        u32LaneNum,
        PARSING_HAL_MIPI_DSI_FMT(pstMipiDsiCfg->enFormat), dsiTmpBufBpp,
        PARSING_HAL_MIPI_DSI_CTRL(pstMipiDsiCfg->enCtrl));

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: VTotal=%d, VPW=%d, VBP=%d, VFP=%d\n",
        __FUNCTION__, __LINE__,
        pstMipiDsiCfg->u16Vactive + pstMipiDsiCfg->u16Vpw + pstMipiDsiCfg->u16Vbp + pstMipiDsiCfg->u16Vfp,
        pstMipiDsiCfg->u16Vpw,pstMipiDsiCfg->u16Vbp, pstMipiDsiCfg->u16Vfp);

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HTotal=%d, HPW=%d, HBP= %d, HFP=%d, BLLP=%d\n",
        __FUNCTION__, __LINE__,
        pstMipiDsiCfg->u16Hactive + pstMipiDsiCfg->u16Hpw + pstMipiDsiCfg->u16Hbp + pstMipiDsiCfg->u16Hfp + pstMipiDsiCfg->u16Bllp,
        pstMipiDsiCfg->u16Hpw, pstMipiDsiCfg->u16Hbp, pstMipiDsiCfg->u16Hfp, pstMipiDsiCfg->u16Bllp);


    W4BYTE(DSI_REG_BASE +(REG_DSI_VACT_NL>>1), pstMipiDsiCfg->u16Vactive);
    W4BYTE(DSI_REG_BASE +(REG_DSI_VSA_NL>>1),  pstMipiDsiCfg->u16Vpw);
    W4BYTE(DSI_REG_BASE +(REG_DSI_VBP_NL>>1),  pstMipiDsiCfg->u16Vbp);
    W4BYTE(DSI_REG_BASE +(REG_DSI_VFP_NL>>1),  pstMipiDsiCfg->u16Vfp);

    if (pstMipiDsiCfg->enCtrl == E_HAL_PNL_MIPI_DSI_SYNC_EVENT || pstMipiDsiCfg->enCtrl == E_HAL_PNL_MIPI_DSI_BURST_MODE )
    {
        //ASSERT((pstMipiDsiCfg->dsi.horizontal_backporch + pstMipiDsiCfg->dsi.horizontal_sync_active) * dsiTmpBufBpp> 9);
        if(((pstMipiDsiCfg->u16Hbp+ pstMipiDsiCfg->u16Hpw)* dsiTmpBufBpp)/8 > 9)
        {
            horizontal_backporch_byte = (((pstMipiDsiCfg->u16Hbp+ pstMipiDsiCfg->u16Hpw)* dsiTmpBufBpp)/8 - 10);
        }
        else
        {
            horizontal_backporch_byte = 1;
        }
    }
    else
    {
        //ASSERT(pstMipiDsiCfg->dsi.horizontal_sync_active * dsiTmpBufBpp > 9);
        if( ((pstMipiDsiCfg->u16Hpw * dsiTmpBufBpp)/8) < 9)
        {
            PNL_ERR("%s %d: horizontal_sync_active byte count less than 9\n", __FUNCTION__, __LINE__);
        }
        horizontal_sync_active_byte = (((pstMipiDsiCfg->u16Hpw * dsiTmpBufBpp)/8) > 9) ? ( (pstMipiDsiCfg->u16Hpw* dsiTmpBufBpp)/8 - 10):0;

        //ASSERT(pstMipiDsiCfg->dsi.horizontal_backporch * dsiTmpBufBpp > 9);
        if( ((pstMipiDsiCfg->u16Hbp * dsiTmpBufBpp)/8) < 9)
        {
            PNL_ERR("%s %d: horizontal_backporch byte count less than 9 \n", __FUNCTION__, __LINE__);
            PNL_ERR("%s %d: horizontal_backporch_byte = %ld \n",  __FUNCTION__, __LINE__, (pstMipiDsiCfg->u16Hbp * dsiTmpBufBpp)/8);
        }
        horizontal_backporch_byte =    (((pstMipiDsiCfg->u16Hbp * dsiTmpBufBpp)/8) > 9 )? ( (pstMipiDsiCfg->u16Hbp * dsiTmpBufBpp)/8 - 10):0;
    }

    //ASSERT(pstMipiDsiCfg->dsi.horizontal_frontporch * dsiTmpBufBpp > 11);
    horizontal_frontporch_byte  = ((pstMipiDsiCfg->u16Hfp* dsiTmpBufBpp)/8 > 12) ?((pstMipiDsiCfg->u16Hfp* dsiTmpBufBpp)/8 - 12) : 0;
    horizontal_bllp_byte = ((pstMipiDsiCfg->u16Bllp* dsiTmpBufBpp)/8 >6)?    ((pstMipiDsiCfg->u16Bllp* dsiTmpBufBpp)/8 -6):0;

    //ASSERT(pstMipiDsiCfg->dsi.horizontal_frontporch * dsiTmpBufBpp > ((300/dsi_cycle_time) * pstMipiDsiCfg->dsi.LANE_NUM));
    //horizontal_frontporch_byte -= ((300/dsi_cycle_time) * pstMipiDsiCfg->dsi.LANE_NUM);

    if( horizontal_frontporch_byte > ( (pstMipiDsiCfg->u8ClkHsExit + pstMipiDsiCfg->u8Lpx + pstMipiDsiCfg->u8HsPrpr + pstMipiDsiCfg->u8HsZero)* u32LaneNum))
    {
        horizontal_frontporch_byte -= ( (pstMipiDsiCfg->u8ClkHsExit + pstMipiDsiCfg->u8Lpx + pstMipiDsiCfg->u8HsPrpr + pstMipiDsiCfg->u8HsZero)* u32LaneNum);
    }
    else
    {
        horizontal_frontporch_byte = 1;
    }

    horizontal_active_byte = (dsiTmpBufBpp*pstMipiDsiCfg->u16Hactive)/8;

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: Byte HAC=%ld, HSA=%ld, HBP=%ld, HFP=%ld\n",
        __FUNCTION__, __LINE__, horizontal_active_byte, horizontal_sync_active_byte, horizontal_backporch_byte, horizontal_frontporch_byte);

    horizontal_active_byte = horizontal_active_byte ? horizontal_active_byte : 1;
    horizontal_sync_active_byte = horizontal_sync_active_byte ? horizontal_sync_active_byte : 1;
    horizontal_backporch_byte = horizontal_backporch_byte ? horizontal_backporch_byte : 1;
    horizontal_frontporch_byte = horizontal_frontporch_byte ? horizontal_frontporch_byte : 1;

    W4BYTE(DSI_REG_BASE +(REG_DSI_HSA_WC>>1),  ALIGN_TO(horizontal_sync_active_byte,4));
    W4BYTE(DSI_REG_BASE +(REG_DSI_HBP_WC>>1),  ALIGN_TO(horizontal_backporch_byte,4));
    W4BYTE(DSI_REG_BASE +(REG_DSI_HFP_WC>>1),  ALIGN_TO(horizontal_frontporch_byte,4));
    W4BYTE(DSI_REG_BASE +(REG_DSI_BLLP_WC>>1), ALIGN_TO(horizontal_bllp_byte,4));
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), ALIGN_TO(horizontal_active_byte,4) ,DSI_TDSI_PS_WC);

    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: PS_WC = 0x%lx \n",  __FUNCTION__, __LINE__, R4BYTE(DSI_REG_BASE +(REG_DSI_PSCON>>1)) & 0x1FFF);
    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HSA_WC= 0x%lx \n",  __FUNCTION__, __LINE__, R4BYTE(DSI_REG_BASE +(REG_DSI_HSA_WC>>1)));
    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HBP_WC= 0x%lx \n",  __FUNCTION__, __LINE__, R4BYTE(DSI_REG_BASE +(REG_DSI_HBP_WC>>1)));
    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: HFP_WC= 0x%lx \n",  __FUNCTION__, __LINE__, R4BYTE(DSI_REG_BASE +(REG_DSI_HFP_WC>>1)));

    // Save to HwConfig
    pstHwCfg->u16Vact = pstMipiDsiCfg->u16Vactive;
    pstHwCfg->u16Vbp = pstMipiDsiCfg->u16Vbp;
    pstHwCfg->u16Vfp = pstMipiDsiCfg->u16Vfp;
    pstHwCfg->u16Vsa = pstMipiDsiCfg->u16Vpw;

    pstHwCfg->u16Hact = ALIGN_TO(horizontal_active_byte, 4);
    pstHwCfg->u16Hbp  = ALIGN_TO(horizontal_backporch_byte, 4);
    pstHwCfg->u16Hfp  = ALIGN_TO(horizontal_frontporch_byte, 4);
    pstHwCfg->u16Hsa  = ALIGN_TO(horizontal_sync_active_byte, 4);


    if(pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB565)
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 0<<16 ,DSI_DSI_PS_SEL);
    }
    else if (pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB666)
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 1<<16 ,DSI_DSI_PS_SEL);
    }
    else if (pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_LOOSELY_RGB666)
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 2<<16 ,DSI_DSI_PS_SEL);
    }
    else if (pstMipiDsiCfg->enFormat == E_HAL_PNL_MIPI_DSI_RGB888)
    {
        W4BYTEMSK(DSI_REG_BASE +(REG_DSI_PSCON>>1), 3<<16 ,DSI_DSI_PS_SEL);
    }

    return 1;
}

void HalPnlGetMipiDsiReg(u32 u32Addr, u32 *pu32Val)
{
    *pu32Val =  R4BYTE(u32Addr);
}


void HalPnlSetMipiDsiChSel(HalPnlMipiDsiConfig_t *pstMipiDisCfg)
{
    u16 u16ClkLane;
    PNL_DBG(PNL_DBG_LEVEL_HAL, "%s %d: CH: %d %d %d %d %d\n",
        __FUNCTION__, __LINE__,
        pstMipiDisCfg->enCh[0], pstMipiDisCfg->enCh[1], pstMipiDisCfg->enCh[2],
        pstMipiDisCfg->enCh[3], pstMipiDisCfg->enCh[4]);

    u16ClkLane = 1 << (pstMipiDisCfg->enCh[0] + 2);
    W2BYTEMSK(REG_DPHY_03_L, u16ClkLane | 0x0080, 0x00FC);
    W2BYTEMSK(REG_DPHY_06_L, (pstMipiDisCfg->enCh[0] << 12)|(pstMipiDisCfg->enCh[0] << 2), PNL_BIT12|PNL_BIT13|PNL_BIT14|PNL_BIT4|PNL_BIT3|PNL_BIT2 );
    W2BYTEMSK(REG_DPHY_0A_L, (pstMipiDisCfg->enCh[1] << 12)|(pstMipiDisCfg->enCh[1] << 2), PNL_BIT12|PNL_BIT13|PNL_BIT14|PNL_BIT4|PNL_BIT3|PNL_BIT2);
    W2BYTEMSK(REG_DPHY_0E_L, (pstMipiDisCfg->enCh[2] << 12)|(pstMipiDisCfg->enCh[2] << 2), PNL_BIT12|PNL_BIT13|PNL_BIT14|PNL_BIT4|PNL_BIT3|PNL_BIT2);
    W2BYTEMSK(REG_DPHY_22_L, (pstMipiDisCfg->enCh[3] << 12)|(pstMipiDisCfg->enCh[3] << 2), PNL_BIT12|PNL_BIT13|PNL_BIT14|PNL_BIT4|PNL_BIT3|PNL_BIT2);
    W2BYTEMSK(REG_DPHY_25_L, (pstMipiDisCfg->enCh[4] << 12)|(pstMipiDisCfg->enCh[4] << 2), PNL_BIT12|PNL_BIT13|PNL_BIT14|PNL_BIT4|PNL_BIT3|PNL_BIT2);
}

void HalPnlSetMipiDsiChPolarity(u8 *pu8ChPol)
{
    u16 u16ChPol[5] = {1, 1, 1, 1, 1};
    u8 i;

    for(i=0; i<5; i++)
    {
        u16ChPol[i] = pu8ChPol[i] ? 1 : 0;
    }

    W2BYTEMSK(REG_DPHY_06_L, u16ChPol[0], 0x0001); // eg_ds_pol_ch0
    W2BYTEMSK(REG_DPHY_0A_L, u16ChPol[1], 0x0001); // eg_ds_pol_ch1
    W2BYTEMSK(REG_DPHY_0E_L, u16ChPol[2], 0x0001); // eg_ds_pol_ch2
    W2BYTEMSK(REG_DPHY_22_L, u16ChPol[3], 0x0001); // eg_ds_pol_ch3
    W2BYTEMSK(REG_DPHY_25_L, u16ChPol[4], 0x0001); // eg_ds_pol_ch4

}

void HalPnlSetMipiDsiPatGen(void)
{
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_BIST_CON>>1), DSI_SELF_PAT_MODE, DSI_SELF_PAT_MODE);
    W4BYTEMSK(DSI_REG_BASE +(REG_DSI_BIST_PATTE_RN>>1), 0x00FFFF, 0xFFFF);
}

u32 HalPnlClkMapMipiTxDsiToIdx(u32 u32ClkRate)
{
    u32 u32ClkIdx;

    u32ClkIdx = u32ClkRate == 0            ? 0x0000 :
                u32ClkRate <= CLK_MHZ(108) ? 0x0003 :
                u32ClkRate <= CLK_MHZ(144) ? 0x0002 :
                u32ClkRate <= CLK_MHZ(160) ? 0x0001 :
                u32ClkRate <= CLK_MHZ(216) ? 0x0004 :
                u32ClkRate <= CLK_MHZ(240) ? 0x0005 :
                                             0x0000;
    return u32ClkIdx;
}

u32 HalPnlClkMapMipiTxDsiToRate(u32 u32ClkIdx)
{
    u32 u32ClkRate;
    u32ClkRate= u32ClkIdx == 0x0000 ? 0 :
                u32ClkIdx == 0x0001 ? CLK_MHZ(160) :
                u32ClkIdx == 0x0002 ? CLK_MHZ(144) :
                u32ClkIdx == 0x0003 ? CLK_MHZ(108) :
                u32ClkIdx == 0x0004 ? CLK_MHZ(216) :
                u32ClkIdx == 0x0005 ? CLK_MHZ(240) :
                                      0;
    return u32ClkRate;
}


void HalPnlClkSetMipiTxDsi(bool bEn, u32 u32ClkRate)
{
    u16 u16RegVal;

    u16RegVal = u32ClkRate << 10;
    u16RegVal |= bEn ? 0x0000 : 0x0100;
    W2BYTEMSK(REG_CLKGEN_5E_L, u16RegVal, 0x1F00);
}

void HalPnlClkGetMipiTxDsi(bool *pbEn, u32 *pu32ClkRate)
{
    u16 u16Val;
    u16Val = R2BYTE(REG_CLKGEN_5E_L) & 0x1F00;

    *pbEn = (u16Val & 0x0100) ? 0 : 1;
    *pu32ClkRate = (u16Val >> 10) & 0x0007;
}
