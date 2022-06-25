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
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DATA_LANE_12_5MHZ  (12500000)
#define DATA_LANE_25MHZ    (25000000)
#define DATA_LANE_50MHZ    (50000000)
#define DATA_LANE_100MHZ   (100000000)
#define DATA_LANE_187_5MHZ (187500000)
#define DATA_LANE_200MHZ   (200000000)
#define DATA_LANE_400MHZ   (400000000)
#define DATA_LANE_800MHZ   (800000000)
#define DATA_LANE_1500MHZ  (1500000000)

#define IS_DATA_LANE_LESS_100M(bps)            ( bps <= DATA_LANE_100MHZ )
#define IS_DATA_LANE_BPS_100M_TO_200M(bps)     ( (bps > DATA_LANE_100MHZ) && (bps <= DATA_LANE_200MHZ ) )
#define IS_DATA_LANE_BPS_200M_TO_400M(bps)     ( (bps > DATA_LANE_200MHZ) && (bps <= DATA_LANE_400MHZ ) )
#define IS_DATA_LANE_BPS_400M_TO_800M(bps)     ( (bps > DATA_LANE_400MHZ) && (bps <= DATA_LANE_800MHZ ) )
#define IS_DATA_LANE_BPS_800M_TO_15000M(bps)   ( (bps > DATA_LANE_800MHZ) && (bps <= DATA_LANE_1500MHZ ) )

#define IS_DATA_LANE_LESS_12_5M(bps)           (  bps <= DATA_LANE_12_5MHZ )
#define IS_DATA_LANE_BPS_12_5M_TO_25M(bps)     ( (bps > DATA_LANE_12_5MHZ) && (bps <= DATA_LANE_25MHZ ) )
#define IS_DATA_LANE_BPS_25M_TO_50M(bps)       ( (bps > DATA_LANE_25MHZ) && (bps <= DATA_LANE_50MHZ ) )
#define IS_DATA_LANE_BPS_50M_TO_100M(bps)      ( (bps > DATA_LANE_50MHZ) && (bps <= DATA_LANE_100MHZ ) )
#define IS_DATA_LANE_BPS_100M_TO_187_5M(bps)   ( (bps > DATA_LANE_100MHZ) && (bps <= DATA_LANE_187_5MHZ ) )


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 gu32JTAGmode = 0;


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
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
        if( IS_DATA_LANE_LESS_12_5M(u32Dclk) )
        {
            *pu16Idx = E_HAL_PNL_SUPPORTED_LPLL_MAX;
            bRet = 0;
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
    return u16LoopGain[u16Idx];
}

u16 HalPnlGetLpllDiv(u16 u16Idx, bool bDsi)
{
    return u16LoopDiv[u16Idx];
}

void HalPnlSetTTLPadMux(HalPnlOutputFormatBitMode_e enFmt)
{
    W2BYTEMSK(REG_CHIPTOP_0F_L,0x00C0,0x00C0);
}

void HalPnlSetTtlPadCotnrol(u32 u32Ie, u32 u32Pe, u32 u32Ps, u32 u32Drv)
{
    //data driving
    W2BYTE(REG_CHIPTOP_48_L, (u32Drv & 0xFFFF));
    W2BYTE(REG_CHIPTOP_49_L, ((u32Drv>16) & 0xFFFF));
    //input enable=
    W2BYTE(REG_CHIPTOP_4A_L, (u32Ie & 0xFFFF));
    W2BYTE(REG_CHIPTOP_4B_L, ((u32Ie>>16) & 0xFFFF));

    W2BYTE(REG_CHIPTOP_4C_L, (u32Pe & 0xFFFF));
    W2BYTE(REG_CHIPTOP_4D_L, ((u32Pe>>16) & 0xFFFF));

    W2BYTE(REG_CHIPTOP_4E_L, (u32Ps & 0xFFFF));
    W2BYTE(REG_CHIPTOP_4F_L, ((u32Ps>>16) & 0xFFFF));


}

void HalPnlSetMipiDisPadMux(HalPnlMipiDsiLaneMode_e enLaneNum)
{

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

void HalPnlSetVSyncSt(u16 u16Val)
{
    //W2BYTEMSK(REG_DISP_TOP_OP2_15_L, u16Val, 0x1FFF);
    W2BYTEMSK(REG_SCL2_01_L, u16Val, 0x07FF);
}

void HalPnlSetVSyncEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_02_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_03_L, u16Val, 0x07FF);
}

void HalPnlSetVfdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_04_L, u16Val, 0x07FF);
}

void HalPnlSetVdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_05_L, u16Val, 0x07FF);
}

void HalPnlSetVdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_06_L, u16Val, 0x07FF);
}

void HalPnlSetVtt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_07_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_09_L, u16Val, 0x07FF);
}

void HalPnlSetHSyncEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0A_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0B_L, u16Val, 0x07FF);
}

void HalPnlSetHfdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0C_L, u16Val, 0x07FF);
}

void HalPnlSetHdeSt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0D_L, u16Val, 0x07FF);
}

void HalPnlSetHdeEnd(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0E_L, u16Val, 0x07FF);
}

void HalPnlSetHtt(u16 u16Val)
{
    W2BYTEMSK(REG_SCL2_0F_L, u16Val, 0x07FF);
}

void HalPnlSetClk(bool bEn, bool bDsi)
{
   
    #if 0
    u16Val= enClkType << 2;
    W2BYTEMSK(REG_CLKGEN_63_L, bEn ? u16Val : 0x0001, 0x003F);
    if(bDsi)
    {
        W2BYTEMSK(REG_CLKGEN_6F_L,  0x0000, 0x001F);  // reg_ckg_mipi_tx_dsi [4:2], 3'b000: lpll clk
        W2BYTEMSK(REG_SC_CTRL_37_L, 0x0004, 0x000F); // reg_ckg_mipi_tx_dsi_apb [3:2] 2'b01: clk_mipi_tx_dsi_p
        W2BYTEMSK(REG_SC_CTRL_35_L, 0x0004, 0x000F); // reg_ckg_hdmi ->[3:2] 2'b01 : sc_pixel clock
        W2BYTEMSK(REG_SC_CTRL_36_L, 0x0004, 0x000F); // reg_ckg_dac -> [3:2] 2'b01 : src from reg_ckg_hdmi
    }
    else
    {
        W2BYTEMSK(REG_SC_CTRL_35_L, 0x0004, 0x000F); // reg_ckg_hdmi ->[3:2] 2'b01 : sc_pixel clock
    }
    #endif
     W2BYTEMSK(REG_CLKGEN_66_L, 0x000c, 0x000F); // reg_ckg_dac -> [3:2] 2'b01 : src from reg_ckg_hdmi

}

void HalPnlSetLpllSkew(u16 u16Val)
{
    W2BYTEMSK(REG_LPLL_44_L, (u16Val << 8), 0xFF00);
}

void HalPnlSetFrameColorEn(bool bEn)
{
    W2BYTEMSK(REG_SCL2_10_L, bEn ? 0x100 : 0x0000, 0x100);
}

void HalPnlSetFrameColor(u8 u16R, u16 u16G, u16 u16B)
{
    u16 u16Val;

    u16Val = (u16B & 0xfF) | (u16)(u16G & 0xfF) << 8 | (u16)(u16R & 0xfF) << 16;
    W4BYTE(REG_SCL2_11_L, u16Val);
}

void HalPnlSetClkInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT0 : 0, BIT0);
}

void HalPnlSetVsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT1 : 0, BIT1);
}

void HalPnlSetHsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT2 : 0, BIT2);
}

void HalPnlSetDeInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? BIT3 : 0, BIT3);
}

void HalPnlSetVsynRefMd(bool bEn)
{
    W2BYTEMSK(REG_SCL0_2D_L, bEn ? BIT4 : 0, BIT4);
}

void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk)
{
    W2BYTEMSK(u32Reg, u16Val, u16Msk);
}

void HalPnlSetTgenExtHsEn(u8 u8Val)
{
}

void HalPnlSetSwReste(u8 u8Val)
{
}

void HalPnlSetFifoRest(u8 u8Val)
{
}

void HalPnlSetDacHsyncSt(u16 u16Val)
{
}

void HalPnlSetDispToDsiMd(u8 u8Val)
{
}

void HalPnlSetDacHsyncEnd(u16 u16Val)
{
}

void HalPnlSetDacHdeSt(u16 u16Val)
{
}

void HalPnlSetDacHdeEnd(u16 u16Val)
{
}

void HalPnlSetDacVdeSt(u16 u16Val)
{
}

void HalPnlSetDacVdeEnd(u16 u16Val)
{
}

void HalPnlSetRgbMode(HalPnlOutputFormatBitMode_e enFmt)
{

}

void HalPnlSetRgbSwap(HalPnlRgbSwapType_e enChR, HalPnlRgbSwapType_e enChG, HalPnlRgbSwapType_e enChB)
{

}

void HalPnlSetRgbMlSwap(u8 u8Val)
{
}

//------------------------------------------------------------------------------
// Mipi DSI

void HalPnlInitMipiDsiDphy(void)
{

}


void HalPnlSetMipiDsiPadOutSel(HalPnlMipiDsiLaneMode_e enLaneMode)
{
    // setting by IPL
}

void HalPnlResetMipiDsi(void)
{

}

void HalPnlEnableMipiDsiClk(void)
{

}

void HalPnlDisableMipiDsiClk(void)
{
}

void HalPnlSetMipiDsiLaneNum(HalPnlMipiDsiLaneMode_e enLaneMode)
{

}

void HalPnlSetMipiDsiCtrlMode(HalPnlMipiDsiCtrlMode_e enCtrlMode)
{

}

bool HalPnlGetMipiDsiClkHsMode(void)
{

    return 1;
}

void HalPnlSetMpiDsiClkHsMode(bool bEn)
{

}

bool HalPnlGetMipiDsiShortPacket(u8 u8ReadBackCount, u8 u8RegAddr)
{


   return 1;
}

bool HalPnlSetMipiDsiShortPacket(u8 u8Count, u8 u8Cmd, u8 *pu8ParamList)
{


    return 1;
}


bool HalPnlSetMipiDsiLongPacket(u8 u8Count, u8 u8Cmd, u8 *pu8ParamList)
{

    return 1;
}


bool HalPnlSetMipiDsiPhyTimConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg)
{
    return 1;
}

bool HalPnlSetMipiDsiVideoTimingConfig(HalPnlMipiDsiConfig_t *pstMipiDsiCfg, HalPnlHwMipiDsiConfig_t *pstHwCfg)
{

    return 1;
}

void HalPnlGetMipiDsiReg(u32 u32Addr, u32 *pu32Val)
{
    *pu32Val =  R4BYTE(u32Addr);
}


void HalPnlSetMipiDsiChSel(HalPnlMipiDsiConfig_t *pstMipiDisCfg)
{
   }

void HalPnlSetMipiDsiPatGen(void)
{

}
