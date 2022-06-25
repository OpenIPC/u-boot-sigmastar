/*
* snr_cfg_infinity5.c- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: eroy.yang <eroy.yang@sigmastar.com.tw>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#include "snr_cfg_infinity6e.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MAX_SUPPORT_SNR                     (3)
#define INVALID_PAD_SEL                     (0xFF)

#define PADTOP_REG_BASE                    (0x1F207800)    // 0x103C
#define CLKGEN_REG_BASE                     (0x1F207000)    // 0x1038
#define VIFTOP_REG_BASE                     (0x1F260800)    // 0x131C

#define PADTOP_TO_SR0_BT656_REG_OFST       (0x6A)
#define PADTOP_TO_SR0_BT656_REG_MASK       (0x0003)
#define PADTOP_TO_SR0_BT656_REG_SHIFT       (0)
#define PADTOP_TO_SR0_MIPI_REG_OFST        (0x69)
#define PADTOP_TO_SR0_MIPI_REG_MASK        (0x0007)
#define PADTOP_TO_SR0_MIPI_REG_SHIFT        (0)
#define PADTOP_TO_SR0_PAR_REG_OFST         (0x6A)
#define PADTOP_TO_SR0_PAR_REG_MASK         (0x000C)
#define PADTOP_TO_SR0_PAR_REG_SHIFT         (2)

#define PADTOP_TO_SR0_CTRLPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR0_CTRLPAD_REG_MASK         (0x0010)
#define PADTOP_TO_SR0_CTRLPAD_REG_SHIFT         (4)

#define PADTOP_TO_SR0_MCLKPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR0_MCLKPAD_REG_MASK         (0x0020)
#define PADTOP_TO_SR0_MCLKPAD_REG_SHIFT         (5)

#define PADTOP_TO_SR0_RSTPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR0_RSTPAD_REG_MASK         (0x0040)
#define PADTOP_TO_SR0_RSTPAD_REG_SHIFT         (6)

#define PADTOP_TO_SR0_PDNPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR0_PDNPAD_REG_MASK         (0x0300)
#define PADTOP_TO_SR0_PDNPAD_REG_SHIFT         (8)

#define PADTOP_TO_SR1_BT656_REG_OFST       (0x6B)
#define PADTOP_TO_SR1_BT656_REG_MASK       (0x0001)
#define PADTOP_TO_SR1_BT656_REG_SHIFT       (0)
#define PADTOP_TO_SR1_MIPI_REG_OFST        (0x6B)
#define PADTOP_TO_SR1_MIPI_REG_MASK        (0x0070)
#define PADTOP_TO_SR1_MIPI_REG_SHIFT        (4)
#define PADTOP_TO_SR1_PAR_REG_OFST         (0x6B)
#define PADTOP_TO_SR1_PAR_REG_MASK         (0x0002)
#define PADTOP_TO_SR1_PAR_REG_SHIFT         (1)

#define PADTOP_TO_SR1_CTRLPAD_REG_OFST         (0x6B)
#define PADTOP_TO_SR1_CTRLPAD_REG_MASK         (0x0100)
#define PADTOP_TO_SR1_CTRLPAD_REG_SHIFT         (8)

#define PADTOP_TO_SR1_MCLKPAD_REG_OFST         (0x6B)
#define PADTOP_TO_SR1_MCLKPAD_REG_MASK         (0x0600)
#define PADTOP_TO_SR1_MCLKPAD_REG_SHIFT         (9)

#define PADTOP_TO_SR1_RSTPAD_REG_OFST         (0x6B)
#define PADTOP_TO_SR1_RSTPAD_REG_MASK         (0x1800)
#define PADTOP_TO_SR1_RSTPAD_REG_SHIFT         (11)

#define PADTOP_TO_SR2_MIPI_REG_OFST        (0x69)
#define PADTOP_TO_SR2_MIPI_REG_MASK        (0x0400)
#define PADTOP_TO_SR2_MIPI_REG_SHIFT        (10)

#define PADTOP_TO_SR2_CTRLPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR2_CTRLPAD_REG_MASK         (0x0800)
#define PADTOP_TO_SR2_CTRLPAD_REG_SHIFT         (11)

#define PADTOP_TO_SR2_MCLKPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR2_MCLKPAD_REG_MASK         (0x1000)
#define PADTOP_TO_SR2_MCLKPAD_REG_SHIFT         (12)

#define PADTOP_TO_SR2_RSTPAD_REG_OFST         (0x69)
#define PADTOP_TO_SR2_RSTPAD_REG_MASK         (0x6000)
#define PADTOP_TO_SR2_RSTPAD_REG_SHIFT         (13)

#define CLKGEN_TO_CLK_SR0_REG_OFST          (0x62)
#define CLKGEN_TO_CLK_SR0_REG_MASK          (0x3F00)
#define CLKGEN_TO_CLK_SR0_REG_SHIFT          (8)

#define CLKGEN_TO_CLK_SR1_REG_OFST          (0x65)
#define CLKGEN_TO_CLK_SR1_REG_MASK          (0x3F00)
#define CLKGEN_TO_CLK_SR1_REG_SHIFT          (8)

#define CLKGEN_TO_CLK_SR2_REG_OFST          (0x65)
#define CLKGEN_TO_CLK_SR2_REG_MASK          (0x003F)
#define CLKGEN_TO_CLK_SR2_REG_SHIFT          (0)

#define VIFTOP_TO_SR0_RST_REG_OFST          (0x3F)
#define VIFTOP_TO_SR0_RST_REG_MASK          (0x0001)
#define VIFTOP_TO_SR0_RST_REG_SHIFT          (0)

#define VIFTOP_TO_SR2_RST_REG_OFST          (0x3F)
#define VIFTOP_TO_SR2_RST_REG_MASK          (0x0004)
#define VIFTOP_TO_SR2_RST_REG_SHIFT          (2)

#define VIFTOP_TO_SR1_RST_REG_OFST          (0x3F)
#define VIFTOP_TO_SR1_RST_REG_MASK          (0x0010)
#define VIFTOP_TO_SR1_RST_REG_SHIFT          (4)

#define VIFTOP_TO_SR0_POWER_DOWN_REG_OFST   (0x3F)
#define VIFTOP_TO_SR0_POWER_DOWN_REG_MASK   (0x0002)
#define VIFTOP_TO_SR0_POWER_DOWN_REG_SHIFT   (1)

#define VIFTOP_TO_SR2_POWER_DOWN_REG_OFST   (0x3F)
#define VIFTOP_TO_SR2_POWER_DOWN_REG_MASK   (0x0008)
#define VIFTOP_TO_SR2_POWER_DOWN_REG_SHIFT   (3)

#define VIFTOP_TO_SR1_POWER_DOWN_REG_OFST   (0x3F)
#define VIFTOP_TO_SR1_POWER_DOWN_REG_MASK   (0x0020)
#define VIFTOP_TO_SR1_POWER_DOWN_REG_SHIFT   (5)

#define REG_WORD(base, idx)                 (*(((volatile unsigned short*)(base))+2*(idx)))
#define REG_WRITE(base, idx, val)           REG_WORD(base, idx) = (val)
#define REG_READ(base, idx)                 REG_WORD(base, idx)

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static int m_iSnrBusType[MAX_SUPPORT_SNR];

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

void Set_SNR_BusType(int iSnrId, int iBusType)
{
    m_iSnrBusType[iSnrId] = iBusType;
}

static void _Set_SNR0_IOPad(int iPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
        if (iPadSel == 0 || iPadSel == 1 || iPadSel == 2)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_PAR_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_PAR_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR0_PAR_REG_SHIFT) & PADTOP_TO_SR0_PAR_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_PAR_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
        if (iPadSel == 0 || iPadSel == 1 || iPadSel == 2 || iPadSel == 3 || iPadSel == 4)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_MIPI_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_MIPI_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR0_MIPI_REG_SHIFT) & PADTOP_TO_SR0_MIPI_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_MIPI_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
        if (iPadSel == 0 || iPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_MIPI_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_MIPI_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR0_MIPI_REG_SHIFT) & PADTOP_TO_SR0_MIPI_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_MIPI_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_BT656_MODE:
        if (iPadSel == 0 || iPadSel == 1 || iPadSel == 2 || iPadSel == 3)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_BT656_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_BT656_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR0_BT656_REG_SHIFT) & PADTOP_TO_SR0_BT656_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_BT656_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR0_CtrlPad(int iCtrlPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iCtrlPadSel == 0 || iCtrlPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_CTRLPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_CTRLPAD_REG_MASK);
            u16RegReadVal |= ((iCtrlPadSel << PADTOP_TO_SR0_CTRLPAD_REG_SHIFT) & PADTOP_TO_SR0_CTRLPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_CTRLPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR0_MclkPad(int iMclkPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iMclkPadSel == 0 || iMclkPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_MCLKPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_MCLKPAD_REG_MASK);
            u16RegReadVal |= ((iMclkPadSel << PADTOP_TO_SR0_MCLKPAD_REG_SHIFT) & PADTOP_TO_SR0_MCLKPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_MCLKPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR0_RstPad(int iRstPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iRstPadSel == 0 || iRstPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_RSTPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_RSTPAD_REG_MASK);
            u16RegReadVal |= ((iRstPadSel << PADTOP_TO_SR0_RSTPAD_REG_SHIFT) & PADTOP_TO_SR0_RSTPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_RSTPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR0_PdnPad(int iPdnPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iPdnPadSel == 0 || iPdnPadSel == 1 || iPdnPadSel == 2)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR0_PDNPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR0_PDNPAD_REG_MASK);
            u16RegReadVal |= ((iPdnPadSel << PADTOP_TO_SR0_PDNPAD_REG_SHIFT) & PADTOP_TO_SR0_PDNPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR0_PDNPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR1_IOPad(int iPadSel)
{
    int iSnrId = 1;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
        if (iPadSel == 0 || iPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_PAR_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_PAR_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR1_PAR_REG_SHIFT) & PADTOP_TO_SR1_PAR_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_PAR_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
        if (iPadSel == 0 || iPadSel == 1 || iPadSel == 2 || iPadSel == 3 || iPadSel == 4 || iPadSel == 5)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_MIPI_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_MIPI_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR1_MIPI_REG_SHIFT) & PADTOP_TO_SR1_MIPI_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_MIPI_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
        if (iPadSel == 0 || iPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_MIPI_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_MIPI_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR1_MIPI_REG_SHIFT) & PADTOP_TO_SR1_MIPI_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_MIPI_REG_OFST, u16RegReadVal);
        }
        break;
    case VIF_SNR_PAD_BT656_MODE:
        if (iPadSel == 0 || iPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_BT656_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_BT656_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR1_BT656_REG_SHIFT) & PADTOP_TO_SR1_BT656_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_BT656_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR1_CtrlPad(int iCtrlPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iCtrlPadSel == 0 || iCtrlPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_CTRLPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_CTRLPAD_REG_MASK);
            u16RegReadVal |= ((iCtrlPadSel << PADTOP_TO_SR1_CTRLPAD_REG_SHIFT) & PADTOP_TO_SR1_CTRLPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_CTRLPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR1_MclkPad(int iMclkPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iMclkPadSel == 0 || iMclkPadSel == 1 || iMclkPadSel == 2)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_MCLKPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_MCLKPAD_REG_MASK);
            u16RegReadVal |= ((iMclkPadSel << PADTOP_TO_SR1_MCLKPAD_REG_SHIFT) & PADTOP_TO_SR1_MCLKPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_MCLKPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR1_RstPad(int iRstPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iRstPadSel == 0 || iRstPadSel == 1 || iRstPadSel == 2)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR1_RSTPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR1_RSTPAD_REG_MASK);
            u16RegReadVal |= ((iRstPadSel << PADTOP_TO_SR1_RSTPAD_REG_SHIFT) & PADTOP_TO_SR1_RSTPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR1_RSTPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR2_IOPad(int iPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_BT656_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
        break;
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
        if (iPadSel == 0 || iPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR2_MIPI_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR2_MIPI_REG_MASK);
            u16RegReadVal |= ((iPadSel << PADTOP_TO_SR2_MIPI_REG_SHIFT) & PADTOP_TO_SR2_MIPI_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR2_MIPI_REG_OFST, u16RegReadVal);
        }
        break;

    default:
        break;
    }
}

static void _Set_SNR2_CtrlPad(int iCtrlPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iCtrlPadSel == 0 || iCtrlPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR2_CTRLPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR2_CTRLPAD_REG_MASK);
            u16RegReadVal |= ((iCtrlPadSel << PADTOP_TO_SR2_CTRLPAD_REG_SHIFT) & PADTOP_TO_SR2_CTRLPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR2_CTRLPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR2_MclkPad(int iMclkPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iMclkPadSel == 0 || iMclkPadSel == 1)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR2_MCLKPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR2_MCLKPAD_REG_MASK);
            u16RegReadVal |= ((iMclkPadSel << PADTOP_TO_SR2_MCLKPAD_REG_SHIFT) & PADTOP_TO_SR2_MCLKPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR2_MCLKPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

static void _Set_SNR2_RstPad(int iRstPadSel)
{
    int iSnrId = 0;
    u16 u16RegReadVal = 0;

    switch(m_iSnrBusType[iSnrId])
    {
    case VIF_SNR_PAD_PARALLEL_MODE:
    case VIF_SNR_PAD_MIPI_1LANE_MODE:
    case VIF_SNR_PAD_MIPI_2LANE_MODE:
    case VIF_SNR_PAD_MIPI_4LANE_MODE:
    case VIF_SNR_PAD_BT656_MODE:
        if (iRstPadSel == 0 || iRstPadSel == 1 || iRstPadSel == 2)
        {
            u16RegReadVal = REG_READ(PADTOP_REG_BASE, PADTOP_TO_SR2_RSTPAD_REG_OFST);
            u16RegReadVal &= ~(PADTOP_TO_SR2_RSTPAD_REG_MASK);
            u16RegReadVal |= ((iRstPadSel << PADTOP_TO_SR2_RSTPAD_REG_SHIFT) & PADTOP_TO_SR2_RSTPAD_REG_MASK);
            REG_WRITE(PADTOP_REG_BASE, PADTOP_TO_SR2_RSTPAD_REG_OFST, u16RegReadVal);
        }
        break;
    default:
        break;
    }
}

void Set_SNR_IOPad(int iSnrId, int iPadSel, int iCtrlPadSel, int iMclkPadSel, int iRstPadSel, int iPdnPadSel)
{
    if (iSnrId == 0) {
        _Set_SNR0_IOPad(iPadSel);
        _Set_SNR0_CtrlPad(iCtrlPadSel);
        _Set_SNR0_MclkPad(iMclkPadSel);
        _Set_SNR0_RstPad(iRstPadSel);
        _Set_SNR0_PdnPad(iPdnPadSel);
    }
    else if (iSnrId == 1) {
        _Set_SNR1_IOPad(iPadSel);
        _Set_SNR1_CtrlPad(iCtrlPadSel);
        _Set_SNR1_MclkPad(iMclkPadSel);
        _Set_SNR1_RstPad(iRstPadSel);
    }
    else if (iSnrId == 2) {
        _Set_SNR2_IOPad(iPadSel);
        _Set_SNR2_CtrlPad(iCtrlPadSel);
        _Set_SNR2_MclkPad(iMclkPadSel);
        _Set_SNR2_RstPad(iRstPadSel);
    }
}

void Set_SNR_MCLK(int iSnrId, int iEnable, int iSpeedIdx)
{
    u16 u16RegReadVal = 0;
    u16 u16Val = 0;

    switch(iSpeedIdx) {
    case SNR_MCLK_27M:
    case SNR_MCLK_72M:
    case SNR_MCLK_61P7M:
    case SNR_MCLK_54M:
    case SNR_MCLK_48M:
    case SNR_MCLK_43P2M:
    case SNR_MCLK_36M:
    case SNR_MCLK_24M:
    case SNR_MCLK_21P6M:
    case SNR_MCLK_12M:
    case SNR_MCLK_5P4M:
    case SNR_MCLK_LPLL:
    case SNR_MCLK_LPLL_DIV2:
    case SNR_MCLK_LPLL_DIV4:
    case SNR_MCLK_LPLL_DIV8:
        u16Val = (iSpeedIdx<<2) | ((iEnable>0)?0:1);
        break;
    default:
        u16Val = 1;
        break;
    }

    if (iSnrId == 0)
    {
        u16RegReadVal = REG_READ(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR0_REG_OFST);
        u16RegReadVal &= ~(CLKGEN_TO_CLK_SR0_REG_MASK);
        u16RegReadVal |= ((u16Val << CLKGEN_TO_CLK_SR0_REG_SHIFT) & CLKGEN_TO_CLK_SR0_REG_MASK);
        REG_WRITE(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR0_REG_OFST, u16RegReadVal);
    }
    else if (iSnrId == 1)
    {
        u16RegReadVal = REG_READ(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR1_REG_OFST);
        u16RegReadVal &= ~(CLKGEN_TO_CLK_SR1_REG_MASK);
        u16RegReadVal |= ((u16Val << CLKGEN_TO_CLK_SR1_REG_SHIFT) & CLKGEN_TO_CLK_SR1_REG_MASK);
        REG_WRITE(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR1_REG_OFST, u16RegReadVal);
    }
    else if (iSnrId == 2)
    {
        u16RegReadVal = REG_READ(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR2_REG_OFST);
        u16RegReadVal &= ~(CLKGEN_TO_CLK_SR2_REG_MASK);
        u16RegReadVal |= ((u16Val << CLKGEN_TO_CLK_SR2_REG_SHIFT) & CLKGEN_TO_CLK_SR2_REG_MASK);
        REG_WRITE(CLKGEN_REG_BASE, CLKGEN_TO_CLK_SR2_REG_OFST, u16RegReadVal);
    }
}

void SNR_PowerDown(int iSnrId, int iVal)
{
    u16 u16RegReadVal = 0;

    switch(iSnrId)
    {
    case 0:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR0_POWER_DOWN_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR0_POWER_DOWN_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR0_POWER_DOWN_REG_SHIFT) & VIFTOP_TO_SR0_POWER_DOWN_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR0_POWER_DOWN_REG_OFST, u16RegReadVal);
        break;
    case 1:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR1_POWER_DOWN_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR1_POWER_DOWN_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR1_POWER_DOWN_REG_SHIFT) & VIFTOP_TO_SR1_POWER_DOWN_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR1_POWER_DOWN_REG_OFST, u16RegReadVal);
        break;
    case 2:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR2_POWER_DOWN_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR2_POWER_DOWN_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR2_POWER_DOWN_REG_SHIFT) & VIFTOP_TO_SR2_POWER_DOWN_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR2_POWER_DOWN_REG_OFST, u16RegReadVal);
        break;
    default:
        break;
    }
}

void SNR_Reset(int iSnrId, int iVal)
{
    u16 u16RegReadVal = 0;

    switch(iSnrId)
    {
    case 0:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR0_RST_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR0_RST_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR0_RST_REG_SHIFT) & VIFTOP_TO_SR0_RST_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR0_RST_REG_OFST, u16RegReadVal);
        break;
    case 1:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR1_RST_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR1_RST_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR1_RST_REG_SHIFT) & VIFTOP_TO_SR1_RST_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR1_RST_REG_OFST, u16RegReadVal);
        break;
    case 2:
        u16RegReadVal = REG_READ(VIFTOP_REG_BASE, VIFTOP_TO_SR2_RST_REG_OFST);
        u16RegReadVal &= ~(VIFTOP_TO_SR2_RST_REG_MASK);
        u16RegReadVal |= ((iVal << VIFTOP_TO_SR2_RST_REG_SHIFT) & VIFTOP_TO_SR2_RST_REG_MASK);
        REG_WRITE(VIFTOP_REG_BASE, VIFTOP_TO_SR2_RST_REG_OFST, u16RegReadVal);
        break;
    default:
        break;
    }
}
