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
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl_lpll_tbl.h"
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DCLK_12D5MHZ       12500000
#define DCLK_25MHZ         25000000
#define DCLK_50MHZ         50000000
#define DCLK_100MHZ        100000000
#define DCLK_187D5MHZ      187500000

#define IsDclkLess12D5M(Dclk)     ((Dclk) <= DCLK_12D5MHZ)
#define IsDclk12D5MTo25M(Dclk)    ((Dclk > DCLK_12D5MHZ) && (Dclk <= DCLK_25MHZ))
#define IsDclk25MTo50M(Dclk)      ((Dclk > DCLK_25MHZ) && (Dclk <= DCLK_50MHZ))
#define IsDclk50MTo100M(Dclk)     ((Dclk > DCLK_50MHZ) && (Dclk <= DCLK_100MHZ))
#define IsDclk100MTo187D5M(Dclk)  ((Dclk > DCLK_100MHZ) && (Dclk <= DCLK_187D5MHZ))


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


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

u16 HalPnlGetLpllIdx(u32 u32Dclk)
{
    u16 u16Idx = 0;

    if(IsDclkLess12D5M(u32Dclk))
    {
        u16Idx = 0xFF;
    }
    else if(IsDclk12D5MTo25M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_12D5TO25MHZ;
    }
    else if(IsDclk25MTo50M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_25TO50MHZ;
    }
    else if(IsDclk50MTo100M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_50TO100MHZ;
    }
    else if(IsDclk100MTo187D5M(u32Dclk))
    {
        u16Idx = E_HAL_PNL_SUPPORTED_LPLL_HS_LVDS_CH_100TO187D5MHZ;
    }
    else
    {
        u16Idx = 0xFF;
    }
    return u16Idx;
}

void HalPnlSetLSSC(u8 u8SscEn, u32 u32SSCStep, u32 u32SSCSpan)
{
    W2BYTEMSK(REG_SCL_LPLL_4E_L, u8SscEn?(BIT15):0, BIT15);
    W2BYTEMSK(REG_SCL_LPLL_4E_L, u32SSCStep, 0xFFF);
    W2BYTEMSK(REG_SCL_LPLL_4F_L, u32SSCSpan, 0x3FFF);
}

u16 HalPnlGetLpllGain(u16 u16Idx)
{
    return u16LoopGain[u16Idx];
}

u16 HalPnlGetLpllDiv(u16 u16Idx)
{
    return u16LoopDiv[u16Idx];
}

void HalPnlIfSetOutFormatConfig(u16 eFormatMode)
{
  //printk("%s, %d, mode \n",__FUNCTION__,eFormatMode);
  if(eFormatMode == 3)
  {
    W2BYTEMSK(REG_SCL2_10_L, 0x8000, 0x8000);
    W2BYTEMSK(REG_SCL2_24_L, 0x80, 0x80);
  }
  else{
    W2BYTEMSK(REG_SCL2_10_L, 0x0000, 0x8000);
    W2BYTEMSK(REG_SCL2_24_L, 0x0000, 0x80);
  }
}
void HalPnlSetChipTop(bool bEn)
{
    if(bEn)
    {
        W2BYTEMSK(REG_CHIPTOP_0F_L,0x00C0,0x0080);
        W2BYTEMSK(REG_CHIPTOP_50_L,0x0000,0xFFFF);
        W2BYTEMSK(REG_CHIPTOP_09_L,0x0000,0xFFFF); // I2C pad mux
    }
    else
    {
        W2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0080);
    }

    if(gu32JTAGmode)
    {
        W2BYTEMSK(REG_CHIPTOP_0F_L,0x0002,0x0002);
    }
    else
    {
        W2BYTEMSK(REG_CHIPTOP_0F_L,0x0000,0x0002);
    }
}


void HalPnlDumpLpllSetting(u16 u16Idx)
{
    u16 u16RegIdx;

    for(u16RegIdx=0; u16RegIdx < HAL_PNL_LPLL_REG_NUM; u16RegIdx++)
    {

        if(LPLLSettingTBL[u16Idx][u16RegIdx].address == 0xFF)
        {
            //DrvSclOsDelayTask(LPLLSettingTBL[u16Idx][u16RegIdx].value);
            continue;
        }

        W2BYTEMSK((REG_SCL_LPLL_BASE | ((u32)LPLLSettingTBL[u16Idx][u16RegIdx].address *2)),
                  LPLLSettingTBL[u16Idx][u16RegIdx].value,
                  LPLLSettingTBL[u16Idx][u16RegIdx].mask);
    }
}

void HalPnlSetLpllSet(u32 u32LpllSet)
{
    u16 u16LpllSet_Lo, u16LpllSet_Hi;

    u16LpllSet_Lo = (u16)(u32LpllSet & 0x0000FFFF);
    u16LpllSet_Hi = (u16)((u32LpllSet & 0x00FF0000) >> 16);
    W2BYTE(REG_SCL_LPLL_48_L, u16LpllSet_Lo);
    W2BYTE(REG_SCL_LPLL_49_L, u16LpllSet_Hi);
}

void HalPnlUnifiedSetMLSwap(bool bSwap)
{
    W2BYTEMSK(REG_SCL2_24_L, bSwap?0x40:0, 0x40);
}

void HalPnlUnifiedSetFixedClk(u32 u32FixedClkRate)
{
    if(u32FixedClkRate == 21600000)
    {
        W2BYTEMSK(0x1038CC, 0x08, 0x0F);
    }
    else if(u32FixedClkRate == 43200000)
    {
        W2BYTEMSK(0x1038CC, 0x04, 0x0F);
    }
    else if(u32FixedClkRate == 86400000)
    {
        W2BYTEMSK(0x1038CC, 0x00, 0x0F);
    }
    else
    {
        PNL_ERR("%s %d, Clock not set.fixed clock is only supported 21600000/43200000/86400000 Hz \n",
            __FUNCTION__, __LINE__);
    }
}


void HalPnlEnableBT656(u8 u8En)
{
    W2BYTEMSK(REG_SCL0_05_L, u8En ? 0 : BIT12, BIT12);  //bt656 or bt1120 sel
    W2BYTEMSK(REG_SCL0_05_L, u8En ? BIT14 : 0, BIT14);  //bt clip
    W2BYTEMSK(REG_SCL0_05_L, u8En ? BIT15 : 0, BIT15);  //bt_enable

    W2BYTEMSK(REG_SCL2_28_L, u8En ? BIT0: 0, BIT0);     //eco bit

    W2BYTEMSK(REG_SCL2_48_L, u8En ? BIT4 : 0, BIT4);  //bt_enable
}


void HalPnlSetVSyncSt(u16 u16Val)
{
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

void HalPnlSetOpenLpllCLK(u8 bLpllClk)
{
    W2BYTEMSK(REG_SCL_CLK_66_L, bLpllClk ? 0x000C : 0x0001, 0x000F);
}

void HalPnlSetFrameColorEn(bool bEn)
{
    W2BYTEMSK(REG_SCL2_10_L, bEn ? PNL_BIT8 : 0, PNL_BIT8);
}

void HalPnlSetFrameColor(u16 u16R, u16 u16G, u16 u16B)
{
    W2BYTE(REG_SCL2_11_L, (u16B | (u16G << 8)));
    W2BYTE(REG_SCL2_12_L, u16R);
}

void HalPnlSetClkInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? PNL_BIT0 : 0, PNL_BIT0);
}

void HalPnlSetVsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? PNL_BIT1 : 0, PNL_BIT1);
}

void HalPnlSetBt656ClkInv(bool bEn)
{
    //1:rising edge, 0:falling edge
    W2BYTEMSK(REG_SCL0_05_L, bEn ? BIT13: 0, BIT13);
}


void HalPnlSetHsyncInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? PNL_BIT2 : 0, PNL_BIT2);
}

void HalPnlSetDeInv(bool bEn)
{
    W2BYTEMSK(REG_SCL2_48_L, bEn ? PNL_BIT3 : 0, PNL_BIT3);
}

void HalPnlSetVsynRefMd(bool bEn)
{
    W2BYTEMSK(REG_SCL0_2D_L, bEn ? PNL_BIT4 : 0, PNL_BIT4);
}

void HalPnlW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Msk)
{
    W2BYTEMSK(u32Reg, u16Val, u16Msk);
}

void HalPnlResetOdclk(bool bEn)
{
    W2BYTEMSK(REG_SCL0_01_L, bEn ? PNL_BIT2 : 0, PNL_BIT2);
}

void HalPnlSetY2RMatrix(bool bEn)
{
    W2BYTE(REG_SCL2_10_L, 0x1000);  //disable frame color

    if(bEn == 1)
        W2BYTE(REG_SCL2_19_L, 0x1001);  // cm_en
    else
        W2BYTE(REG_SCL2_19_L, 0x0000);  // cm_en

    W2BYTE(REG_SCL2_1A_L, 0x8080);  // cm_en
    W2BYTE(REG_SCL2_1B_L, 0x0cc4);  // cm_11
    W2BYTE(REG_SCL2_1C_L, 0x0950);  // cm_12
    W2BYTE(REG_SCL2_1D_L, 0x3ffc);  // cm_13
    W2BYTE(REG_SCL2_1E_L, 0x397e);  // cm_21
    W2BYTE(REG_SCL2_1F_L, 0x0950);  // cm_22
    W2BYTE(REG_SCL2_20_L, 0x3cde);  // cm_23
    W2BYTE(REG_SCL2_21_L, 0x3ffe);  // cm_31
    W2BYTE(REG_SCL2_22_L, 0x0950);  // cm_32
    W2BYTE(REG_SCL2_23_L, 0x1024);  // cm_33
}

