/*
* hal_disp_mop.c- Sigmastar
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

#define _HAL_DISP_MOP_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_mop.h"
#include "hal_disp.h"

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_MOP_MIU_BUS 4

#define HAL_DISP_MOP_DBLBF_WRITE_MD 0
#define HAL_DISP_MOP_FORCE_WRITE_MD 1

#define HAL_DISP_MOP_WRITE_MD HAL_DISP_MOP_DBLBF_WRITE_MD

#define ROT_WDMA_BLOCK_STRIDE_ALGIN(x) ((x & 0x1FF) ? (((x >> 9) + 1) << 9) : (x)) // 512 align
#define ROT_RDMA_BLOCK_STRIDE_ALGIN(x) ((x & 0x07F) ? (((x >> 7) + 1) << 7) : (x)) // 128 align

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

// SW shadow
// bk 0x1406
u16 g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_SHADOW_NUM];
u32 g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_SHADOW_NUM];

// bk 0x1407
u16 g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_SHADOW_NUM];
u32 g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_SHADOW_NUM];

// bk 0x1408
u16 g_u16MopsSwShadowVal[E_HAL_DISP_LAYER_MOPS_SHADOW_NUM];
u32 g_u16MopsSwShadowReg[E_HAL_DISP_LAYER_MOPS_SHADOW_NUM];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

// sw shadow
// close shadow, add if needed
/*
void HalMopsShadowInit(void)
{
    //bk 0x1406
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_01] = R2BYTE(REG_DISP_MOPG_BK01_01_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_01] = REG_DISP_MOPG_BK01_01_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_11] = R2BYTE(REG_DISP_MOPG_BK01_11_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_11] = REG_DISP_MOPG_BK01_11_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_21] = R2BYTE(REG_DISP_MOPG_BK01_21_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_21] = REG_DISP_MOPG_BK01_21_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_31] = R2BYTE(REG_DISP_MOPG_BK01_31_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_31] = REG_DISP_MOPG_BK01_31_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_41] = R2BYTE(REG_DISP_MOPG_BK01_41_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_41] = REG_DISP_MOPG_BK01_41_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_51] = R2BYTE(REG_DISP_MOPG_BK01_51_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_51] = REG_DISP_MOPG_BK01_51_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_61] = R2BYTE(REG_DISP_MOPG_BK01_61_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_61] = REG_DISP_MOPG_BK01_61_L;
    g_u16Mopg01SwShadowVal[E_HAL_DISP_LAYER_MOPG01_71] = R2BYTE(REG_DISP_MOPG_BK01_71_L);
    g_u16Mopg01SwShadowReg[E_HAL_DISP_LAYER_MOPG01_71] = REG_DISP_MOPG_BK01_71_L;

    //bk 0x1407
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_01] = R2BYTE(REG_DISP_MOPG_BK02_01_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_01] = REG_DISP_MOPG_BK02_01_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_11] = R2BYTE(REG_DISP_MOPG_BK02_11_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_11] = REG_DISP_MOPG_BK02_11_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_21] = R2BYTE(REG_DISP_MOPG_BK02_21_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_21] = REG_DISP_MOPG_BK02_21_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_31] = R2BYTE(REG_DISP_MOPG_BK02_31_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_31] = REG_DISP_MOPG_BK02_31_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_41] = R2BYTE(REG_DISP_MOPG_BK02_41_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_41] = REG_DISP_MOPG_BK02_41_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_51] = R2BYTE(REG_DISP_MOPG_BK02_51_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_51] = REG_DISP_MOPG_BK02_51_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_61] = R2BYTE(REG_DISP_MOPG_BK02_61_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_61] = REG_DISP_MOPG_BK02_61_L;
    g_u16Mopg02SwShadowVal[E_HAL_DISP_LAYER_MOPG02_71] = R2BYTE(REG_DISP_MOPG_BK02_71_L);
    g_u16Mopg02SwShadowReg[E_HAL_DISP_LAYER_MOPG02_71] = REG_DISP_MOPG_BK02_71_L;

    //bk 0x1408
    g_u16MopsSwShadowVal[E_HAL_DISP_LAYER_MOPS_51] = R2BYTE(REG_DISP_MOPS_BK0_51_L);
    g_u16MopsSwShadowReg[E_HAL_DISP_LAYER_MOPS_51] = REG_DISP_MOPS_BK0_51_L;

}
u16 _HalMopGetSwShadow(u32 eMopBk, u32 u32Reg)
{
    u16 u16val;
    if(eMopBk == REG_DISP_MOPS_BK0_BASE)
    {
        //mops sw shadow register
        u16val = g_u16MopsSwShadowVal[u32Reg];
    }
    else if(eMopBk == REG_DISP_MOPG_BK01_BASE)
    {
        //mopg01 sw shadow register
        u16val = g_u16Mopg01SwShadowVal[u32Reg];
    }
    else if(eMopBk == REG_DISP_MOPG_BK02_BASE)
    {
        //mopg02 sw shadow register
        u16val = g_u16Mopg02SwShadowVal[u32Reg];
    }
    else //left for other mop bank if needed
    {
        u16val=0;
    }
    return u16val;
}
void _HalMopSetSwShadow(u32 eMopBk, u32 u32Reg, u16 u16Val)
{
    if(eMopBk == REG_DISP_MOPS_BK0_BASE)
    {
        //Gwin
        g_u16MopsSwShadowVal[u32Reg] = u16Val;
    }
    else if(eMopBk == REG_DISP_MOPG_BK01_BASE)
    {
        //mopg01 sw shadow register
        g_u16Mopg01SwShadowVal[u32Reg] = u16Val;
    }
    else if(eMopBk == REG_DISP_MOPG_BK02_BASE)
    {
        //mopg02 sw shadow register
        g_u16Mopg02SwShadowVal[u32Reg] = u16Val;
    }
    else //left for other mop bank if needed
    {

    }
}
u32 _HalMopGetShadowReg(u32 eMopBk, u32 u32Reg)
{
    u32 u32val;
    if(eMopBk == REG_DISP_MOPS_BK0_BASE)
    {
        //mops sw shadow register
        u32val = g_u16MopsSwShadowReg[u32Reg];
    }
    else if(eMopBk == REG_DISP_MOPG_BK01_BASE)
    {
        //mopg01 sw shadow register
        u32val = g_u16Mopg01SwShadowReg[u32Reg];
    }
    else if(eMopBk == REG_DISP_MOPG_BK02_BASE)
    {
        //mopg02 sw shadow register
        u32val = g_u16Mopg02SwShadowReg[u32Reg];
    }
    else //left for other mop bank if needed
    {
        u32val=0;
    }
    return u32val;
}
void _HalMopSwShadowHandler(u32 eMopBk, u32 u32Reg, u16 u16Val, u16 u16Msk)
{
    u16 u16ShadowVal;
    u32 u32RegVal;
    u16ShadowVal = _HalMopGetSwShadow(eMopBk,u32Reg);
    u16ShadowVal = ((u16Val&u16Msk)|(u16ShadowVal&(~u16Msk)));
    _HalMopSetSwShadow(eMopBk,u32Reg,u16ShadowVal);
    u32RegVal=_HalMopGetShadowReg(eMopBk, u32Reg);
    W2BYTEMSK(u32RegVal, u16ShadowVal, 0xFFFF);
}
*/
static u32 _HalDispMopgGwinRegBkMap(HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId)
{
    u32 u32RegBk;

    if (eMopId == E_HAL_DISP_MOPID_00)
    {
        switch (eMopgId)
        {
            case E_HAL_DISP_MOPG_ID_00:
            case E_HAL_DISP_MOPG_ID_01:
            case E_HAL_DISP_MOPG_ID_02:
            case E_HAL_DISP_MOPG_ID_03:
            case E_HAL_DISP_MOPG_ID_04:
            case E_HAL_DISP_MOPG_ID_05:
            case E_HAL_DISP_MOPG_ID_06:
            case E_HAL_DISP_MOPG_ID_07:
                u32RegBk = REG_DISP_MOPG_BK01_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_08:
            case E_HAL_DISP_MOPG_ID_09:
            case E_HAL_DISP_MOPG_ID_10:
            case E_HAL_DISP_MOPG_ID_11:
            case E_HAL_DISP_MOPG_ID_12:
            case E_HAL_DISP_MOPG_ID_13:
            case E_HAL_DISP_MOPG_ID_14:
            case E_HAL_DISP_MOPG_ID_15:
                u32RegBk = REG_DISP_MOPG_BK02_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_16:
            case E_HAL_DISP_MOPG_ID_17:
            case E_HAL_DISP_MOPG_ID_18:
            case E_HAL_DISP_MOPG_ID_19:
            case E_HAL_DISP_MOPG_ID_20:
            case E_HAL_DISP_MOPG_ID_21:
            case E_HAL_DISP_MOPG_ID_22:
            case E_HAL_DISP_MOPG_ID_23:
                u32RegBk = REG_DISP_MOPG_BK03_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_24:
            case E_HAL_DISP_MOPG_ID_25:
            case E_HAL_DISP_MOPG_ID_26:
            case E_HAL_DISP_MOPG_ID_27:
            case E_HAL_DISP_MOPG_ID_28:
            case E_HAL_DISP_MOPG_ID_29:
            case E_HAL_DISP_MOPG_ID_30:
            case E_HAL_DISP_MOPG_ID_31:
                u32RegBk = REG_DISP_MOPG_BK04_BASE;
                break;
            default:
                DISP_ERR("[HALMOP]%s %d, Mopg ID %d not support\n", __FUNCTION__, __LINE__, eMopgId);
                u32RegBk = REG_DISP_MOPG_BK01_BASE;
                break;
        }
    }
    else if (eMopId == E_HAL_DISP_MOPID_01)
    {
        switch (eMopgId)
        {
            case E_HAL_DISP_MOPG_ID_00:
            case E_HAL_DISP_MOPG_ID_01:
            case E_HAL_DISP_MOPG_ID_02:
            case E_HAL_DISP_MOPG_ID_03:
            case E_HAL_DISP_MOPG_ID_04:
            case E_HAL_DISP_MOPG_ID_05:
            case E_HAL_DISP_MOPG_ID_06:
            case E_HAL_DISP_MOPG_ID_07:
                u32RegBk = REG_DISP_MOPG_BK11_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_08:
            case E_HAL_DISP_MOPG_ID_09:
            case E_HAL_DISP_MOPG_ID_10:
            case E_HAL_DISP_MOPG_ID_11:
            case E_HAL_DISP_MOPG_ID_12:
            case E_HAL_DISP_MOPG_ID_13:
            case E_HAL_DISP_MOPG_ID_14:
            case E_HAL_DISP_MOPG_ID_15:
                u32RegBk = REG_DISP_MOPG_BK12_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_16:
            case E_HAL_DISP_MOPG_ID_17:
            case E_HAL_DISP_MOPG_ID_18:
            case E_HAL_DISP_MOPG_ID_19:
            case E_HAL_DISP_MOPG_ID_20:
            case E_HAL_DISP_MOPG_ID_21:
            case E_HAL_DISP_MOPG_ID_22:
            case E_HAL_DISP_MOPG_ID_23:
                u32RegBk = REG_DISP_MOPG_BK13_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_24:
            case E_HAL_DISP_MOPG_ID_25:
            case E_HAL_DISP_MOPG_ID_26:
            case E_HAL_DISP_MOPG_ID_27:
            case E_HAL_DISP_MOPG_ID_28:
            case E_HAL_DISP_MOPG_ID_29:
            case E_HAL_DISP_MOPG_ID_30:
            case E_HAL_DISP_MOPG_ID_31:
                u32RegBk = REG_DISP_MOPG_BK14_BASE;
                break;
            default:
                DISP_ERR("[HALMOP]%s %d, Mopg ID %d not support\n", __FUNCTION__, __LINE__, eMopgId);
                u32RegBk = REG_DISP_MOPG_BK01_BASE;
                break;
        }
    }
    else if (eMopId == E_HAL_DISP_MOPID_02)
    {
        switch (eMopgId)
        {
            case E_HAL_DISP_MOPG_ID_00:
            case E_HAL_DISP_MOPG_ID_01:
            case E_HAL_DISP_MOPG_ID_02:
            case E_HAL_DISP_MOPG_ID_03:
            case E_HAL_DISP_MOPG_ID_04:
            case E_HAL_DISP_MOPG_ID_05:
            case E_HAL_DISP_MOPG_ID_06:
            case E_HAL_DISP_MOPG_ID_07:
                u32RegBk = REG_DISP_MOPG_BK21_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_08:
            case E_HAL_DISP_MOPG_ID_09:
            case E_HAL_DISP_MOPG_ID_10:
            case E_HAL_DISP_MOPG_ID_11:
            case E_HAL_DISP_MOPG_ID_12:
            case E_HAL_DISP_MOPG_ID_13:
            case E_HAL_DISP_MOPG_ID_14:
            case E_HAL_DISP_MOPG_ID_15:
                u32RegBk = REG_DISP_MOPG_BK22_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_16:
            case E_HAL_DISP_MOPG_ID_17:
            case E_HAL_DISP_MOPG_ID_18:
            case E_HAL_DISP_MOPG_ID_19:
            case E_HAL_DISP_MOPG_ID_20:
            case E_HAL_DISP_MOPG_ID_21:
            case E_HAL_DISP_MOPG_ID_22:
            case E_HAL_DISP_MOPG_ID_23:
                u32RegBk = REG_DISP_MOPG_BK23_BASE;
                break;
            case E_HAL_DISP_MOPG_ID_24:
            case E_HAL_DISP_MOPG_ID_25:
            case E_HAL_DISP_MOPG_ID_26:
            case E_HAL_DISP_MOPG_ID_27:
            case E_HAL_DISP_MOPG_ID_28:
            case E_HAL_DISP_MOPG_ID_29:
            case E_HAL_DISP_MOPG_ID_30:
            case E_HAL_DISP_MOPG_ID_31:
                u32RegBk = REG_DISP_MOPG_BK24_BASE;
                break;
            default:
                DISP_ERR("[HALMOP]%s %d, Mopg ID %d not support\n", __FUNCTION__, __LINE__, eMopgId);
                u32RegBk = REG_DISP_MOPG_BK01_BASE;
                break;
        }
    }
    else
    {
        DISP_ERR("[HALMOP]%s %d, Mop ID %d not support\n", __FUNCTION__, __LINE__, eMopId);
        u32RegBk = REG_DISP_MOPG_BK01_BASE;
    }

    return u32RegBk;
}

static u32 _HalDispMopgGwinRegShiftMap(HalDispMopgGwinId_e eMopgId)
{
    u32 u32GeinRegShift;

    switch (eMopgId)
    {
        case E_HAL_DISP_MOPG_ID_00:
        case E_HAL_DISP_MOPG_ID_08:
        case E_HAL_DISP_MOPG_ID_16:
        case E_HAL_DISP_MOPG_ID_24:
            u32GeinRegShift = 0x00;
            break;
        case E_HAL_DISP_MOPG_ID_01:
        case E_HAL_DISP_MOPG_ID_09:
        case E_HAL_DISP_MOPG_ID_17:
        case E_HAL_DISP_MOPG_ID_25:
            u32GeinRegShift = 0x20;
            break;
        case E_HAL_DISP_MOPG_ID_02:
        case E_HAL_DISP_MOPG_ID_10:
        case E_HAL_DISP_MOPG_ID_18:
        case E_HAL_DISP_MOPG_ID_26:
            u32GeinRegShift = 0x40;
            break;
        case E_HAL_DISP_MOPG_ID_03:
        case E_HAL_DISP_MOPG_ID_11:
        case E_HAL_DISP_MOPG_ID_19:
        case E_HAL_DISP_MOPG_ID_27:
            u32GeinRegShift = 0x60;
            break;
        case E_HAL_DISP_MOPG_ID_04:
        case E_HAL_DISP_MOPG_ID_12:
        case E_HAL_DISP_MOPG_ID_20:
        case E_HAL_DISP_MOPG_ID_28:
            u32GeinRegShift = 0x80;
            break;
        case E_HAL_DISP_MOPG_ID_05:
        case E_HAL_DISP_MOPG_ID_13:
        case E_HAL_DISP_MOPG_ID_21:
        case E_HAL_DISP_MOPG_ID_29:
            u32GeinRegShift = 0xA0;
            break;
        case E_HAL_DISP_MOPG_ID_06:
        case E_HAL_DISP_MOPG_ID_14:
        case E_HAL_DISP_MOPG_ID_22:
        case E_HAL_DISP_MOPG_ID_30:
            u32GeinRegShift = 0xC0;
            break;
        case E_HAL_DISP_MOPG_ID_07:
        case E_HAL_DISP_MOPG_ID_15:
        case E_HAL_DISP_MOPG_ID_23:
        case E_HAL_DISP_MOPG_ID_31:
            u32GeinRegShift = 0xE0;
            break;
        default:
            DISP_ERR("[HALMOP]%s %d, Mopg ID %d not support\n", __FUNCTION__, __LINE__, eMopgId);
            u32GeinRegShift = 0x00;
            break;
    }
    return u32GeinRegShift;
}

static void _HalDispMopRotDbBfWrWithoutCmdq(void *pCtx, HalDispMopId_e eMopId, HalDispMopRotId_e eMopRotId)
{
    u16   u16RegVal;
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;

    u16RegVal = (eMopRotId == E_HAL_DISP_MOPROT_ROT0_ID)   ? DISP_BIT2
                : (eMopRotId == E_HAL_DISP_MOPROT_ROT1_ID) ? DISP_BIT3
                                                           : DISP_BIT2;

    if (pCmdqCtx)
    {
        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_DBLBF_WRITE_MD)
        {
            HalDispUtilityW2BYTEMSKDirect(u32MopRegBase, u16RegVal, u16RegVal, pCmdqCtx);
            HalDispUtilityW2BYTEMSKDirect(u32MopRegBase, 0, u16RegVal, pCmdqCtx);
        }
    }
}

// close sw shadow, open if needed
/*u32 _HalDispMopgGwinRegSwShadowMap(HalDispMopgGwinId_e eMopgId)
{
    u32 u32SwShadowMap = 0;

    switch(eMopgId)
    {
        case E_HAL_DISP_MOPG_ID_00:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_01;
            break;
        case E_HAL_DISP_MOPG_ID_01:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_11;
            break;
        case E_HAL_DISP_MOPG_ID_02:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_21;
            break;
        case E_HAL_DISP_MOPG_ID_03:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_31;
            break;
        case E_HAL_DISP_MOPG_ID_04:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_41;
            break;
        case E_HAL_DISP_MOPG_ID_05:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_51;
            break;
        case E_HAL_DISP_MOPG_ID_06:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_61;
            break;
        case E_HAL_DISP_MOPG_ID_07:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG01_71;
            break;
        case E_HAL_DISP_MOPG_ID_08:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_01;
            break;
        case E_HAL_DISP_MOPG_ID_09:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_11;
            break;
        case E_HAL_DISP_MOPG_ID_10:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_21;
            break;
        case E_HAL_DISP_MOPG_ID_11:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_31;
            break;
        case E_HAL_DISP_MOPG_ID_12:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_41;
            break;
        case E_HAL_DISP_MOPG_ID_13:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_51;
            break;
        case E_HAL_DISP_MOPG_ID_14:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_61;
            break;
        case E_HAL_DISP_MOPG_ID_15:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG02_71;
            break;
        case E_HAL_DISP_MOPG_ID_16:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_01;
            break;
        case E_HAL_DISP_MOPG_ID_17:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_11;
            break;
        case E_HAL_DISP_MOPG_ID_18:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_21;
            break;
        case E_HAL_DISP_MOPG_ID_19:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_31;
            break;
        case E_HAL_DISP_MOPG_ID_20:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_41;
            break;
        case E_HAL_DISP_MOPG_ID_21:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_51;
            break;
        case E_HAL_DISP_MOPG_ID_22:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_61;
            break;
        case E_HAL_DISP_MOPG_ID_23:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG03_71;
            break;
        case E_HAL_DISP_MOPG_ID_24:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_01;
            break;
        case E_HAL_DISP_MOPG_ID_25:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_11;
            break;
        case E_HAL_DISP_MOPG_ID_26:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_21;
            break;
        case E_HAL_DISP_MOPG_ID_27:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_31;
            break;
        case E_HAL_DISP_MOPG_ID_28:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_41;
            break;
        case E_HAL_DISP_MOPG_ID_29:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_51;
            break;
        case E_HAL_DISP_MOPG_ID_30:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_61;
            break;
        case E_HAL_DISP_MOPG_ID_31:
            u32SwShadowMap = E_HAL_DISP_LAYER_MOPG04_71;
            break;
        default:
            DISP_ERR("[HALMOP]%s %d, Mopg ID %d not support\n", __FUNCTION__, __LINE__,eMopgId);
            u32SwShadowMap = 0x00;
            break;
    }
    return u32SwShadowMap;
}*/

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
/// MOPG
void HalDispMopgSwReset(HalDispMopId_e eMopId, void *pCmdq)
{
    u32 u32MopgGwinRegBK   = 0;
    u32 u32MopgGwinRegBase = 0;
    u32MopgGwinRegBK       = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                               : REG_DISP_MOPG_BK00_BASE;

    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_00_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_00_L
                                                                         : REG_DISP_MOPG_BK00_00_L;
    HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, 0x0001, 0x0001, pCmdq);
    HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, 0x0000, 0x0001, pCmdq);
}

void HalDispMopgSetAutoStretchWinSizeEn(void *pCtx, HalDispMopId_e eMopId,
                                        u8 bEnAuto) // 0: use reg_gw_hsize/vsize; 1: use display size
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_01_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_01_L
                                                                         : REG_DISP_MOPG_BK00_01_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(
            u32MopgGwinRegBase, bEnAuto ? (DISP_BIT11 | DISP_BIT10) : 0, (DISP_BIT11 | DISP_BIT10),
            pCmdqCtx); // bit11:auto blanking. 0:use reg h15; 1:use display blanking;bit10:auto stretch window
    }
}

void HalDispMopgSetStretchWinSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height)
{
    void *pCmdqCtx              = NULL;
    u32   u32MopgGwinRegBK      = 0;
    u32   u32MopgGwinRegBasegwh = 0;
    u32   u32MopgGwinRegBasegwv = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK      = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                            : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                              : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBasegwh = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_07_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_07_L
                                                                            : REG_DISP_MOPG_BK00_07_L;

    u32MopgGwinRegBasegwv = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_08_L
                                                                            : REG_DISP_MOPG_BK00_08_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBasegwh, u16Width, 0x1FFF, pCmdqCtx);  // reg_gw_hsize
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBasegwv, u16Height, 0x1FFF, pCmdqCtx); // reg_gw_vsize
    }
}

void HalDispMopgSetHextSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Hext)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_15_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_15_L
                                                                         : REG_DISP_MOPG_BK00_15_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u16Hext, 0x1FFF, pCmdqCtx); // reg_gw_hsize
    }
}

void HalDispMopgSetPipeDelay(void *pCtx, HalDispMopId_e eMopId, u8 u8PipeDelay)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_03_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_03_L
                                                                         : REG_DISP_MOPG_BK00_03_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u8PipeDelay, 0x001F, pCmdqCtx); // reg_pipe_delay
    }
}

void HalDispMopgSetDmaYFifoThrd(void *pCtx, HalDispMopId_e eMopId, u8 u8YThread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_04_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_04_L
                                                                         : REG_DISP_MOPG_BK00_04_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u8YThread, 0x00FF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u8YThread, 0x00FF, pCmdqCtx);
    }
}

void HalDispMopgSetDmaCFifoThrd(void *pCtx, HalDispMopId_e eMopId, u8 u8CThread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_05_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_05_L
                                                                         : REG_DISP_MOPG_BK00_05_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u8CThread, 0x00FF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u8CThread, 0x00FF, pCmdqCtx);
    }
}

void HalDispMopgSetFf0DmaFifoThrd(void *pCtx, HalDispMopId_e eMopId, u16 u16Thread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                           : REG_DISP_MOPG_BK00_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK10_BASE)   ? REG_DISP_MOPG_BK10_52_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPG_BK20_BASE) ? REG_DISP_MOPG_BK20_52_L
                                                                         : REG_DISP_MOPG_BK00_52_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u16Thread, 0xFFFF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u16Thread, 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopgCompressMdEn(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u8 bEn)
{
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0F_L
                                                                          : REG_DISP_MOPG_BK01_0F_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, bEn ? DISP_BIT3 : 0, DISP_BIT3,
                                pCmdqCtx); // compress enable
    }
}

void HalDispMopgCompressCropBlk(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u8 u8Val)
{
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0F_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0F_L
                                                                          : REG_DISP_MOPG_BK01_0F_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, u8Val, 0x3, pCmdqCtx); // compress blk
    }
}

void HalDispMopgInit(void *pCtx)
{
    u32            u32MopgGwinRegBK = 0;
    HalDispMopId_e eMopId;
    void *         pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    for (eMopId = E_HAL_DISP_MOPID_00; eMopId < E_HAL_DISP_MOP_NUM; eMopId++)
    {
        u32MopgGwinRegBK = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_BASE
                           : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_BASE
                                                             : REG_DISP_MOPG_BK00_BASE;

        HalDispMopgSwReset(eMopId, pCmdqCtx);
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x01) * 2, DISP_BIT8, DISP_BIT8,
                                      pCmdqCtx); // clk_miu. 0:use free run; 1:use gated clock
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x01) * 2, DISP_BIT9, DISP_BIT9,
                                      pCmdqCtx); // clk_gop. 0:use free run; 1:use gated clock
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x01) * 2, DISP_BIT10, DISP_BIT10,
                                      pCmdqCtx); // auto stretch win. 0:use reg h8; 1:use display blanking
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x01) * 2, DISP_BIT11, DISP_BIT11,
                                      pCmdqCtx); // auto blanking. 0:use reg h15; 1:use display blanking
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x03) * 2, 0x0A, 0x001F,
                                      pCmdqCtx); // set pipe delay (default=0xA)

        HalDispMopgSetDmaYFifoThrd(pCtx, eMopId, 0x30, 1); // h04 [7:0]ymd_thd
        HalDispMopgSetDmaCFifoThrd(pCtx, eMopId, 0x30, 1); // h05 [7:0]cdma_thd; default=0xf8, use 0xd0
        HalDispMopgSetFf0DmaFifoThrd(pCtx, eMopId, 0x4040, 1);

        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x06) * 2, 0xF0, 0x00FF,
                                      pCmdqCtx); // h06 Luma DMA priority [7]yhp_en [6:0] yhp_thd
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x0B) * 2, 0xF0, 0x00FF,
                                      pCmdqCtx); // h0b Chroma DMA priority [7]yhp_en [6:0] yhp_thd
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x15) * 2, 0x80, 0x1FFF, pCmdqCtx); // h15 [12:0]gw_hext

        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x40) * 2, 0x0689, 0x07FF, pCmdqCtx); // h40 set 4tap

        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x70) * 2, 0x0223, 0x0773,
                                      pCmdqCtx); // h70 [0]axi_en(default=1) [1]comp_en(engine_en default=1, gwin has
                                                 // other en) [6:4]axi_y_maxlen=0x2(8) [10:8]axi_c_maxlen=0x2(8)

        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_FORCE_WRITE_MD)
        {
            HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x7F) * 2, DISP_BIT9, DISP_BIT9,
                                          pCmdqCtx); // set force write or not
        }
        else
        {
            HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x7F) * 2, DISP_BIT8, DISP_BIT8, pCmdqCtx);
            HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBK + (0x7F) * 2, DISP_BIT8, 0, pCmdqCtx);
        }
    }
}

void HalDispMopgGwinEn(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u8 bEn)
{
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_00_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_00_L
                                                                          : REG_DISP_MOPG_BK01_00_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, bEn ? DISP_BIT0 : 0, DISP_BIT0,
                                pCmdqCtx); // reg_pipe_delay
    }
}

void HalDispMopgSetYAddr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, ss_phys_addr_t paPhyAddr)
{
    u32   u32RegVal;
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    u32   u32MopgGwinRegBase2 = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_02_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_02_L
                                                                          : REG_DISP_MOPG_BK01_02_L;

    u32MopgGwinRegBase2 = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_03_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_03_L
                                                                          : REG_DISP_MOPG_BK01_03_L;
    u32RegVal           = (paPhyAddr >> HAL_DISP_MOP_MIU_BUS);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase2 + u32MopgGwinRegShift, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF,
                                pCmdqCtx);
    }
}

void HalDispMopgSetAddr16Offset(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u8 u8Offset)
{
    u32 u32RegVal;
    u32 u32MopgGwinRegBK    = 0;
    u32 u32MopgGwinRegShift = 0;
    u32 u32MopgGwinRegBase  = 0;
    // u32 u32RegForSwShd = 0;
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_01_L
                                                                          : REG_DISP_MOPG_BK01_01_L;
    u32RegVal           = (u8Offset << 12);

    // u32RegForSwShd = _HalDispMopgGwinRegSwShadowMap(eMopgId);

    //_HalMopSwShadowHandler(u32MopgGwinRegBK, u32RegForSwShd, (u32RegVal & 0x1E00), 0x1E00);

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, (u32RegVal & 0xF000), 0xF000, pCmdqCtx);
    }
}

void HalDispMopgSetCAddr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, ss_phys_addr_t paPhyAddr)
{
    u32   u32RegVal;
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    u32   u32MopgGwinRegBase2 = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_04_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_04_L
                                                                          : REG_DISP_MOPG_BK01_04_L;

    u32MopgGwinRegBase2 = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_05_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_05_L
                                                                          : REG_DISP_MOPG_BK01_05_L;
    u32RegVal           = (paPhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase2 + u32MopgGwinRegShift, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF,
                                pCmdqCtx);
    }
}

void HalDispMopgSetGwinParam(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16Hst, u16 u16Vst,
                             u16 u16Width, u16 u16Height)
{
    u16   u16Hend;
    u16   u16Vend;
    u32   u32MopgGwinRegBK       = 0;
    u32   u32MopgGwinRegShift    = 0;
    u32   u32MopgGwinRegHstBase  = 0;
    u32   u32MopgGwinRegHendBase = 0;
    u32   u32MopgGwinRegVstBase  = 0;
    u32   u32MopgGwinRegVendBase = 0;
    void *pCmdqCtx               = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK      = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift   = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegHstBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_06_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_06_L
                                                                            : REG_DISP_MOPG_BK01_06_L;

    u32MopgGwinRegHendBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_07_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_07_L
                                                                             : REG_DISP_MOPG_BK01_07_L;

    u32MopgGwinRegVstBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_08_L
                            : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_08_L
                                                                            : REG_DISP_MOPG_BK01_08_L;

    u32MopgGwinRegVendBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_09_L
                             : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_09_L
                                                                             : REG_DISP_MOPG_BK01_09_L;
    u16Hend                = u16Hst + u16Width - 1;
    u16Vend                = u16Vst + u16Height - 1;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegHstBase + u32MopgGwinRegShift, u16Hst, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegHendBase + u32MopgGwinRegShift, u16Hend, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegVstBase + u32MopgGwinRegShift, u16Vst, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegVendBase + u32MopgGwinRegShift, u16Vend, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopgSetPitch(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16Pitch)
{
    u16   u16RegVal;
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0A_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0A_L
                                                                          : REG_DISP_MOPG_BK01_0A_L;
    u16RegVal           = u16Pitch >> HAL_DISP_MOP_MIU_BUS;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, u16RegVal, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopgSetSourceParam(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16SrcWidth,
                               u16 u16SrcHeight)
{
    u32   u32MopgGwinRegBK            = 0;
    u32   u32MopgGwinRegShift         = 0;
    u32   u32MopgGwinRegSrcWidthBase  = 0;
    u32   u32MopgGwinRegSrcHeightBase = 0;
    void *pCmdqCtx                    = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK           = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift        = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegSrcWidthBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0B_L
                                 : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0B_L
                                                                                 : REG_DISP_MOPG_BK01_0B_L;

    u32MopgGwinRegSrcHeightBase = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0C_L
                                  : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0C_L
                                                                                  : REG_DISP_MOPG_BK01_0C_L;


    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegSrcWidthBase + u32MopgGwinRegShift, (u16SrcWidth - 1), 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegSrcHeightBase + u32MopgGwinRegShift, (u16SrcHeight - 1), 0x1FFF,
                                pCmdqCtx);

        // hsize mask = source_width/16
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegSrcWidthBase + u32MopgGwinRegShift, u16SrcWidth, 0x3FF0, pCmdqCtx);
    }
}

void HalDispMopgSetHScaleFac(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16HRatio)
{
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0D_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0D_L
                                                                          : REG_DISP_MOPG_BK01_0D_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, u16HRatio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopgSetVScaleFac(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16VRatio)
{
    u32   u32MopgGwinRegBK    = 0;
    u32   u32MopgGwinRegShift = 0;
    u32   u32MopgGwinRegBase  = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_0E_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_0E_L
                                                                          : REG_DISP_MOPG_BK01_0E_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, u16VRatio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopgSetLineBufStr(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId, u16 u16GwinHstr,
                              u8 u8WinPri)
{
    u32 u32MopgGwinRegBK    = 0;
    u32 u32MopgGwinRegShift = 0;
    u32 u32MopgGwinRegBase  = 0;
    u16 u16RegVal           = 0;
    // u32 u32RegForSwShd = 0;
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopgGwinRegBK    = _HalDispMopgGwinRegBkMap(eMopId, eMopgId);
    u32MopgGwinRegShift = _HalDispMopgGwinRegShiftMap(eMopgId);
    u32MopgGwinRegBase  = (u32MopgGwinRegBK == REG_DISP_MOPG_BK02_BASE)   ? REG_DISP_MOPG_BK02_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK03_BASE) ? REG_DISP_MOPG_BK03_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK04_BASE) ? REG_DISP_MOPG_BK04_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK11_BASE) ? REG_DISP_MOPG_BK11_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK12_BASE) ? REG_DISP_MOPG_BK12_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK13_BASE) ? REG_DISP_MOPG_BK13_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK14_BASE) ? REG_DISP_MOPG_BK14_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK21_BASE) ? REG_DISP_MOPG_BK21_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK22_BASE) ? REG_DISP_MOPG_BK22_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK23_BASE) ? REG_DISP_MOPG_BK23_01_L
                          : (u32MopgGwinRegBK == REG_DISP_MOPG_BK24_BASE) ? REG_DISP_MOPG_BK24_01_L
                                                                          : REG_DISP_MOPG_BK01_01_L;

    // u32RegForSwShd = _HalDispMopgGwinRegSwShadowMap(eMopgId);
    if (u8WinPri == 0)
    {
        u16RegVal = 0;
    }
    else
    {
        u16RegVal =
            ((u16GwinHstr >> 3) + 2 * u8WinPri) & 0x1FF; // lb_str = gwin_hstr/8 + 2*gwin_priority; gwin_priority is the
                                                         // order of the position of display(from left on) 0~15
    }

    //_HalMopSwShadowHandler(u32MopgGwinRegBK, u32RegForSwShd, u16RegVal, 0x00FF);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase + u32MopgGwinRegShift, u16RegVal, 0x01FF, pCmdqCtx);
    }
}

/// MOPS
void HalDispMopsSwReset(HalDispMopId_e eMopId, void *pCmdq)
{
    u32 u32MopsGwinRegBK   = 0;
    u32 u32MopsGwinRegBase = 0;
    u32MopsGwinRegBK       = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_BASE
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_BASE
                                                               : REG_DISP_MOPS_BK0_BASE;
    u32MopsGwinRegBase     = (u32MopsGwinRegBK == REG_DISP_MOPS_BK1_BASE)   ? REG_DISP_MOPS_BK1_00_L
                             : (u32MopsGwinRegBK == REG_DISP_MOPS_BK2_BASE) ? REG_DISP_MOPS_BK2_00_L
                                                                            : REG_DISP_MOPS_BK0_00_L;
    HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBase, 0x0001, 0x0001, pCmdq);
    HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBase, 0x0000, 0x0001, pCmdq);
}

void HalDispMopsSetAutoStretchWinSizeEn(void *pCtx, HalDispMopId_e eMopId,
                                        u8 bEnAuto) // 0: use reg_gw_hsize/vsize; 1: use display size
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_01_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK1_02_L
                                                           : REG_DISP_MOPS_BK0_01_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(
            u32MopsGwinRegBase, bEnAuto ? (DISP_BIT11 | DISP_BIT10) : 0, (DISP_BIT11 | DISP_BIT10),
            pCmdqCtx); // bit11:auto blanking. 0:use reg h15; 1:use display blanking;bit10:auto stretch window
    }
}

void HalDispMopsSetStretchWinSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Width, u16 u16Height)
{
    void *pCmdqCtx                 = NULL;
    u32   u32MopsGwinWidthRegBase  = 0;
    u32   u32MopsGwinHeightRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinWidthRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_07_L
                              : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_07_L
                                                                : REG_DISP_MOPS_BK0_07_L;

    u32MopsGwinHeightRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_08_L
                               : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_08_L
                                                                 : REG_DISP_MOPS_BK0_08_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinWidthRegBase, u16Width, 0x1FFF, pCmdqCtx);   // reg_gw_hsize
        HalDispUtilityW2BYTEMSK(u32MopsGwinHeightRegBase, u16Height, 0x1FFF, pCmdqCtx); // reg_gw_vsize
    }
}

void HalDispMopsSetHextSize(void *pCtx, HalDispMopId_e eMopId, u16 u16Hext)
{
    void *pCmdqCtx                = NULL;
    u32   u32MopsGwinWidthRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinWidthRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_15_L
                              : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_15_L
                                                                : REG_DISP_MOPS_BK0_15_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinWidthRegBase, u16Hext, 0x1FFF, pCmdqCtx); // reg_gw_hsize
    }
}

void HalDispMopsSetPipeDelay(void *pCtx, HalDispMopId_e eMopId, u8 u8PipeDelay)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_03_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK1_03_L
                                                           : REG_DISP_MOPS_BK0_03_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u8PipeDelay, 0x001F, pCmdqCtx); // reg_pipe_delay
    }
}

void HalDispMopsSetDmaYFifoThrd(void *pCtx, HalDispMopId_e eMopId, u8 u8YThread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_BASE
                                                           : REG_DISP_MOPS_BK0_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPS_BK1_BASE)   ? REG_DISP_MOPS_BK1_04_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPS_BK2_BASE) ? REG_DISP_MOPS_BK2_04_L
                                                                        : REG_DISP_MOPS_BK0_04_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u8YThread, 0x00FF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u8YThread, 0x00FF, pCmdqCtx);
    }
}

void HalDispMopsSetDmaCFifoThrd(void *pCtx, HalDispMopId_e eMopId, u8 u8CThread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_BASE
                                                           : REG_DISP_MOPS_BK0_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPS_BK1_BASE)   ? REG_DISP_MOPS_BK1_05_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPS_BK2_BASE) ? REG_DISP_MOPS_BK2_05_L
                                                                        : REG_DISP_MOPS_BK0_05_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u8CThread, 0x00FF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u8CThread, 0x00FF, pCmdqCtx);
    }
}

void HalDispMopsSetFf0DmaFifoThrd(void *pCtx, HalDispMopId_e eMopId, u16 u16Thread, u8 bDirect)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopgGwinRegBK   = 0;
    u32   u32MopgGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopgGwinRegBK   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_BASE
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_BASE
                                                           : REG_DISP_MOPS_BK0_BASE;
    u32MopgGwinRegBase = (u32MopgGwinRegBK == REG_DISP_MOPS_BK1_BASE)   ? REG_DISP_MOPS_BK1_53_L
                         : (u32MopgGwinRegBK == REG_DISP_MOPS_BK2_BASE) ? REG_DISP_MOPS_BK2_53_L
                                                                        : REG_DISP_MOPS_BK0_53_L;

    if (bDirect)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MopgGwinRegBase, u16Thread, 0xFFFF, pCmdqCtx);
    }
    else if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegBase, u16Thread, 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopsCompressMdEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    void *pCmdqCtx             = NULL;
    u32   u32MopsGwinRegBase   = 0;
    u32   u32MopsGwinRegBaseDE = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_70_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_70_L
                                                           : REG_DISP_MOPS_BK0_70_L;

    u32MopsGwinRegBaseDE = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_71_L
                           : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_71_L
                                                             : REG_DISP_MOPS_BK0_71_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, bEn ? DISP_BIT1 : 0, DISP_BIT1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBaseDE, bEn ? 0 : DISP_BIT0, DISP_BIT0, pCmdqCtx);
    }
}

void HalDispMopsCompressCropBlk(void *pCtx, HalDispMopId_e eMopId, u8 u8Val)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2F_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2F_L
                                                           : REG_DISP_MOPS_BK0_2F_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u8Val, 0x3, pCmdqCtx);
    }
}

void HalDispMopsInit(void *pCtx)
{
    u32            u32MopsGwinRegBK = 0;
    HalDispMopId_e eMopId;
    void *         pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    for (eMopId = E_HAL_DISP_MOPID_00; eMopId < E_HAL_DISP_MOP_NUM; eMopId++)
    {
        u32MopsGwinRegBK = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_BASE
                           : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_BASE
                                                             : REG_DISP_MOPS_BK0_BASE;

        HalDispMopsSwReset(eMopId, pCmdqCtx);
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x01) * 2, DISP_BIT8, DISP_BIT8,
                                      pCmdqCtx); // clk_miu. 0:use free run; 1:use gated clock
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x01) * 2, DISP_BIT9, DISP_BIT9,
                                      pCmdqCtx); // clk_gop. 0:use free run; 1:use gated clock
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x01) * 2, DISP_BIT10, DISP_BIT10,
                                      pCmdqCtx); // auto stretch win. 0:use reg h7,8; 1:use display blanking
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x01) * 2, DISP_BIT11, DISP_BIT11,
                                      pCmdqCtx); // auto blanking. 0:use reg h15; 1:use display blanking
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x03) * 2, 0x0A, 0x001F,
                                      pCmdqCtx); // set pipe delay (default=0xA)

        HalDispMopsSetDmaYFifoThrd(pCtx, eMopId, 0x30, 1); // h04 [7:0]ymd_thd
        HalDispMopsSetDmaCFifoThrd(pCtx, eMopId, 0x30, 1); // h05 [7:0]cdma_thd; default=0xf8, use 0xd0
        HalDispMopsSetFf0DmaFifoThrd(pCtx, eMopId, 0x4040, 1);

        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x06) * 2, 0xF0, 0x00FF,
                                      pCmdqCtx); // h06 Luma DMA priority [7]yhp_en [6:0] yhp_thd
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x0B) * 2, 0xF0, 0x00FF,
                                      pCmdqCtx); // h0b Chroma DMA priority [7]yhp_en [6:0] yhp_thd
        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x15) * 2, 0x80, 0x1FFF, pCmdqCtx); // h15 [12:0]gw_hext

        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x40) * 2, 0x0689, 0x07FF, pCmdqCtx); // h40 set 4tap

        HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x70) * 2, 0x0221, 0x0771,
                                      pCmdqCtx); // h70 [0]axi_en(default=1) [1]comp_en(engine_en default=0, gwins has
                                                 // no other en) [6:4]axi_y_maxlen=0x2(8) [10:8]axi_c_maxlen=0x2(8)
        // HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x71)*2, 0x0001, 0x0001, pCmdqCtx);//h71 [0]DE bypass en
        HalDispMopsCompressMdEn(pCtx, eMopId, 0); // default close compress mode

        // set double buffer write
        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_FORCE_WRITE_MD)
        {
            HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x7F) * 2, DISP_BIT9, DISP_BIT9,
                                          pCmdqCtx); // set force write or not
        }
        else
        {
            HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x7F) * 2, DISP_BIT8, DISP_BIT8, pCmdqCtx);
            HalDispUtilityW2BYTEMSKDirect(u32MopsGwinRegBK + (0x7F) * 2, DISP_BIT8, 0, pCmdqCtx);
        }
    }
}

void HalDispMopsGwinEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_20_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_20_L
                                                           : REG_DISP_MOPS_BK0_20_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, bEn ? DISP_BIT0 : 0, DISP_BIT0, pCmdqCtx);
    }
}

void HalDispMopsSetYAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t paPhyAddr)
{
    u32   u32RegVal;
    void *pCmdqCtx            = NULL;
    u32   u32MopsGwinRegBase  = 0;
    u32   u32MopsGwinRegBase2 = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase  = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_22_L
                          : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_22_L
                                                            : REG_DISP_MOPS_BK0_22_L;
    u32MopsGwinRegBase2 = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_23_L
                          : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_23_L
                                                            : REG_DISP_MOPS_BK0_23_L;
    u32RegVal           = (u32)(paPhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopsSetAddr16Offset(void *pCtx, HalDispMopId_e eMopId, u8 u8Offset)
{
    u32   u32RegVal;
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_21_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_21_L
                                                           : REG_DISP_MOPS_BK0_21_L;

    u32RegVal = (u8Offset << 9);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, (u32RegVal & 0x1E00), 0x1E00, pCmdqCtx);
    }
}

void HalDispMopsSetCAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t paPhyAddr)
{
    u32   u32RegVal;
    void *pCmdqCtx            = NULL;
    u32   u32MopsGwinRegBase  = 0;
    u32   u32MopsGwinRegBase2 = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase  = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_24_L
                          : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_24_L
                                                            : REG_DISP_MOPS_BK0_24_L;
    u32MopsGwinRegBase2 = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_25_L
                          : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_25_L
                                                            : REG_DISP_MOPS_BK0_25_L;
    u32RegVal           = (u32)(paPhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopsSetGwinParam(void *pCtx, HalDispMopId_e eMopId, u16 u16Hst, u16 u16Vst, u16 u16Width, u16 u16Height)
{
    u16   u16Hend;
    u16   u16Vend;
    void *pCmdqCtx               = NULL;
    u32   u32MopsGwinRegHstBase  = 0;
    u32   u32MopsGwinRegHendBase = 0;
    u32   u32MopsGwinRegVstBase  = 0;
    u32   u32MopsGwinRegVendBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegHstBase  = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_26_L
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_26_L
                                                               : REG_DISP_MOPS_BK0_26_L;
    u32MopsGwinRegHendBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_27_L
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_27_L
                                                               : REG_DISP_MOPS_BK0_27_L;
    u32MopsGwinRegVstBase  = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_28_L
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_28_L
                                                               : REG_DISP_MOPS_BK0_28_L;
    u32MopsGwinRegVendBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_29_L
                             : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_29_L
                                                               : REG_DISP_MOPS_BK0_29_L;
    u16Hend                = u16Hst + u16Width - 1;
    u16Vend                = u16Vst + u16Height - 1;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegHstBase, u16Hst, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegHendBase, u16Hend, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegVstBase, u16Vst, 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegVendBase, u16Vend, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopsSetPitch(void *pCtx, HalDispMopId_e eMopId, u16 u16Pitch)
{
    u16   u16RegVal;
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2A_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2A_L
                                                           : REG_DISP_MOPS_BK0_2A_L;
    u16RegVal          = u16Pitch >> HAL_DISP_MOP_MIU_BUS;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u16RegVal, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopsSetSourceParam(void *pCtx, HalDispMopId_e eMopId, u16 u16SrcWidth, u16 u16SrcHeight)
{
    void *pCmdqCtx                  = NULL;
    u32   u32MopsGwinRegWidthBase   = 0;
    u32   u32MopsGwinRegHeightBase  = 0;
    u32   u32MopgGwinRegHsizMskBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegWidthBase   = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2B_L
                                : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2B_L
                                                                  : REG_DISP_MOPS_BK0_2B_L;
    u32MopsGwinRegHeightBase  = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2C_L
                                : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2C_L
                                                                  : REG_DISP_MOPS_BK0_2C_L;
    u32MopgGwinRegHsizMskBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2F_L
                                : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2F_L
                                                                  : REG_DISP_MOPS_BK0_2F_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegWidthBase, (u16SrcWidth - 1), 0x1FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegHeightBase, (u16SrcHeight - 1), 0x1FFF, pCmdqCtx);

        // hsize mask = source_width/16
        HalDispUtilityW2BYTEMSK(u32MopgGwinRegHsizMskBase, u16SrcWidth, 0x3FF0, pCmdqCtx); // h2f[13:4]
    }
}

void HalDispMopsSetHScaleFac(void *pCtx, HalDispMopId_e eMopId, u16 u16HRatio)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2D_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2D_L
                                                           : REG_DISP_MOPS_BK0_2D_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u16HRatio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopsSetVScaleFac(void *pCtx, HalDispMopId_e eMopId, u16 u16VRatio)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_2E_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_2E_L
                                                           : REG_DISP_MOPS_BK0_2E_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u16VRatio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopsSetRingEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_54_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_54_L
                                                           : REG_DISP_MOPS_BK0_54_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, bEn ? DISP_BIT15 : 0, DISP_BIT15, pCmdqCtx);
    }
}

void HalDispMopsSetRingSrc(void *pCtx, HalDispMopId_e eMopId, u8 bImi)
{
    void *pCmdqCtx              = NULL;
    u32   u32MopsGwinRegBase    = 0;
    u32   u32MopsGwinaxiRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_54_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_54_L
                                                           : REG_DISP_MOPS_BK0_54_L;

    u32MopsGwinaxiRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_70_L
                            : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_70_L
                                                              : REG_DISP_MOPS_BK0_70_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, bImi ? DISP_BIT14 : 0, DISP_BIT14, pCmdqCtx);
        // imi mode, close axi interface
        HalDispUtilityW2BYTEMSK(u32MopsGwinaxiRegBase, bImi ? 0 : DISP_BIT0, DISP_BIT0, pCmdqCtx);
    }
}

void HalDispMopsSetRingBuffHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16RingBufHeight)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_54_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_54_L
                                                           : REG_DISP_MOPS_BK0_54_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u16RingBufHeight, 0x1FFF, pCmdqCtx);
    }
}

void HalDispMopsSetBuffStartLine(void *pCtx, HalDispMopId_e eMopId, u16 u16RingBufStartLine)
{
    void *pCmdqCtx           = NULL;
    u32   u32MopsGwinRegBase = 0;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    u32MopsGwinRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPS_BK1_55_L
                         : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPS_BK2_55_L
                                                           : REG_DISP_MOPS_BK0_55_L;

    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopsGwinRegBase, u16RingBufStartLine, 0x1FFF, pCmdqCtx);
    }
}

// Set ClkRate
void HalDispMopSetClk(u8 bEn, u32 u32ClkRate)
{
    u16 u16RegVal;

    u16RegVal = (u32ClkRate <= CLK_MHZ(288))   ? (0x02 << 2)
                : (u32ClkRate <= CLK_MHZ(320)) ? (0x00 << 2)
                : (u32ClkRate <= CLK_MHZ(384)) ? (0x01 << 2)
                                               : (0x03 << 2);

    u16RegVal |= bEn ? 0x0000 : 0x0001;

    W2BYTEMSK(REG_CLKGEN_54_L, u16RegVal, 0x000F);
}

// Get ClkRate
void HalDispMopGetClk(u8 *pbEn, u32 *pu32ClkRate)
{
    u16 u16RegVal;

    u16RegVal = R2BYTE(REG_CLKGEN_54_L);

    *pbEn = u16RegVal & 0x0001 ? 0 : 1;

    *pu32ClkRate = ((u16RegVal & 0x0C) >> 2) == 0x00   ? CLK_MHZ(320)
                   : ((u16RegVal & 0x0C) >> 2) == 0x01 ? CLK_MHZ(384)
                   : ((u16RegVal & 0x0C) >> 2) == 0x02 ? CLK_MHZ(288)
                                                       : 0;
}

/// MOP common
void HalDispMopDbBfWr(void *pCtx, HalDispMopId_e eMopId)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_7F_L
                    : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_7F_L
                                                      : REG_DISP_MOPG_BK00_7F_L;
    if (pCmdqCtx)
    {
        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_DBLBF_WRITE_MD)
        {
            HalDispUtilityW2BYTEMSK(u32MopRegBase, DISP_BIT8, DISP_BIT8, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(u32MopRegBase, 0, DISP_BIT8, pCmdqCtx);
        }
    }
}

// MOP rotate
void HalDispMopRotDbBfWr(void *pCtx, HalDispMopId_e eMopId, HalDispMopRotId_e eMopRotId)
{
    u16   u16RegVal;
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;

    u16RegVal = (eMopRotId == E_HAL_DISP_MOPROT_ROT0_ID)   ? DISP_BIT2
                : (eMopRotId == E_HAL_DISP_MOPROT_ROT1_ID) ? DISP_BIT3
                                                           : DISP_BIT2;

    if (pCmdqCtx)
    {
        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_DBLBF_WRITE_MD)
        {
            HalDispUtilityW2BYTEMSK(u32MopRegBase, u16RegVal, u16RegVal, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(u32MopRegBase, 0, u16RegVal, pCmdqCtx);
        }
    }
}

void HalDispMopRot0DbfEn(HalDispMopId_e eMopId, u8 bEn, void *pCmdq)
{
    u32 u32MopRegBase = 0;
    u32MopRegBase     = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;
    HalDispUtilityW2BYTEMSKDirect(u32MopRegBase, bEn ? DISP_BIT0 : 0, DISP_BIT0, pCmdq);
}

void HalDispMopRot1DbfEn(HalDispMopId_e eMopId, u8 bEn, void *pCmdq)
{
    u32 u32MopRegBase = 0;
    u32MopRegBase     = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;
    HalDispUtilityW2BYTEMSKDirect(u32MopRegBase, bEn ? DISP_BIT1 : 0, DISP_BIT1, pCmdq);
}

void HalDispMopRot0En(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase       = 0;
    u32   u32MopRegBase2      = 0;
    u32   u32MopROTTOPRegBase = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase       = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOPS_BK1_0A_L : REG_DISP_MOPS_BK0_0A_L;
    u32MopRegBase2      = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;
    u32MopROTTOPRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? DISP_TOP1_REG_MIU_MOPROT_MUX : DISP_TOP0_REG_MIU_MOPROT_MUX;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, bEn ? DISP_BIT0 : 0, DISP_BIT0, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, bEn ? DISP_BIT14 : 0, DISP_BIT14, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopROTTOPRegBase, bEn ? DISP_BIT14 : 0, DISP_BIT14, pCmdqCtx);
    }
}

void HalDispMopRot1En(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase       = 0;
    u32   u32MopRegBase2      = 0;
    u32   u32MopROTTOPRegBase = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase       = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOPS_BK1_51_L : REG_DISP_MOPS_BK0_51_L;
    u32MopRegBase2      = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_01_L : REG_DISP_MOP_ROT_BK0_01_L;
    u32MopROTTOPRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? DISP_TOP1_REG_MIU_MOPROT_MUX : DISP_TOP0_REG_MIU_MOPROT_MUX;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, bEn ? DISP_BIT0 : 0, DISP_BIT0, pCmdqCtx);
        //_HalMopSwShadowHandler(REG_DISP_MOPS_BK0_BASE, E_HAL_DISP_LAYER_MOPS_51, bEn ? DISP_BIT0 : 0, DISP_BIT0);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, bEn ? DISP_BIT15 : 0, DISP_BIT15, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopROTTOPRegBase, bEn ? DISP_BIT15 : 0, DISP_BIT15, pCmdqCtx);
    }
}

void HalDispMopRot0ReadBlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch)
{
    u32   u32MopRegBlkNumBase = 0;
    u32   u32BlkNum           = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_19_L : REG_DISP_MOP_ROT_BK0_19_L;
    // block num need 128 align
    u32BlkNum = ROT_RDMA_BLOCK_STRIDE_ALGIN(u16pitch) >> 7;
    if (pCmdqCtx)
    {
        // block num
        HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum, 0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot0WriteBlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch)
{
    u32   u32MopRegBlkNumBase = 0;
    u32   u32BlkNum           = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_29_L : REG_DISP_MOP_ROT_BK0_29_L;
    // block num need 512 align
    u32BlkNum = ROT_WDMA_BLOCK_STRIDE_ALGIN(u16pitch) >> 4;
    if (pCmdqCtx)
    {
        // block num
        HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum, 0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot1ReadBlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch)
{
    u32   u32MopRegBlkNumBase = 0;
    u32   u32BlkNum           = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_39_L : REG_DISP_MOP_ROT_BK0_39_L;
    // block num need 128 align
    u32BlkNum = ROT_RDMA_BLOCK_STRIDE_ALGIN(u16pitch) >> 7;
    if (pCmdqCtx)
    {
        // block num
        HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum, 0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot1WriteBlkNum(void *pCtx, HalDispMopId_e eMopId, u16 u16pitch)
{
    u32   u32MopRegBlkNumBase = 0;
    u32   u32BlkNum           = 0;
    void *pCmdqCtx            = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_49_L : REG_DISP_MOP_ROT_BK0_49_L;
    // block num need 512 align
    u32BlkNum = ROT_WDMA_BLOCK_STRIDE_ALGIN(u16pitch) >> 4;
    if (pCmdqCtx)
    {
        // block num
        HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum, 0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot0SourceWidth(void *pCtx, HalDispMopId_e eMopId, u16 u16inputWidth)
{
    u32 u32MopRegBase  = 0;
    u32 u32MopRegBase2 = 0;
    // u32 u32MopRegBlkNumBase = 0;
    // u32 u32BlkNum = 0;
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_10_L : REG_DISP_MOP_ROT_BK0_10_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_20_L : REG_DISP_MOP_ROT_BK0_20_L;
    /*u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_29_L:
                                                            REG_DISP_MOP_ROT_BK0_29_L;
    u32BlkNum = u16inputWidth/128;//block num need 128 align
    if(u16inputWidth%128)
    {
        u32BlkNum++;
    }*/
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16inputWidth, 0x07FF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16inputWidth >> 1), 0x07FF, pCmdqCtx);
        // block num
        // HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum,  0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot0SourceHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16inputHeight)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_11_L : REG_DISP_MOP_ROT_BK0_11_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_21_L : REG_DISP_MOP_ROT_BK0_21_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16inputHeight, 0x07FF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16inputHeight >> 1), 0x07FF, pCmdqCtx);
    }
}

void HalDispMopRot1SourceWidth(void *pCtx, HalDispMopId_e eMopId, u16 u16inputWidth)
{
    u32 u32MopRegBase  = 0;
    u32 u32MopRegBase2 = 0;
    // u32 u32MopRegBlkNumBase = 0;
    // u32 u32BlkNum = 0;
    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_30_L : REG_DISP_MOP_ROT_BK0_30_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_40_L : REG_DISP_MOP_ROT_BK0_40_L;
    /*u32MopRegBlkNumBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_49_L:
                                                            REG_DISP_MOP_ROT_BK0_49_L;
    u32BlkNum = u16inputWidth/128;//block num need 128 align
    if(u16inputWidth%128)
    {
        u32BlkNum++;
    }*/
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16inputWidth, 0x07FF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16inputWidth >> 1), 0x07FF, pCmdqCtx);
        // block num
        // HalDispUtilityW2BYTEMSK(u32MopRegBlkNumBase, u32BlkNum,  0x03FF, pCmdqCtx);
    }
}

void HalDispMopRot1SourceHeight(void *pCtx, HalDispMopId_e eMopId, u16 u16inputHeight)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_31_L : REG_DISP_MOP_ROT_BK0_31_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_41_L : REG_DISP_MOP_ROT_BK0_41_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16inputHeight, 0x07FF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16inputHeight >> 1), 0x07FF, pCmdqCtx);
    }
}

void HalDispMopRot0SetReadYAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t paPhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_14_L : REG_DISP_MOP_ROT_BK0_14_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_15_L : REG_DISP_MOP_ROT_BK0_15_L;

    u32RegVal = (u32)(paPhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot0SetReadCAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_24_L : REG_DISP_MOP_ROT_BK0_24_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_25_L : REG_DISP_MOP_ROT_BK0_25_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetReadYAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_34_L : REG_DISP_MOP_ROT_BK0_34_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_35_L : REG_DISP_MOP_ROT_BK0_35_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetReadCAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_44_L : REG_DISP_MOP_ROT_BK0_44_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_45_L : REG_DISP_MOP_ROT_BK0_45_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot0SetWriteYAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_17_L : REG_DISP_MOP_ROT_BK0_17_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_18_L : REG_DISP_MOP_ROT_BK0_18_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot0SetWriteCAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_27_L : REG_DISP_MOP_ROT_BK0_27_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_28_L : REG_DISP_MOP_ROT_BK0_28_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetWriteYAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_37_L : REG_DISP_MOP_ROT_BK0_37_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_38_L : REG_DISP_MOP_ROT_BK0_38_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetWriteCAddr(void *pCtx, HalDispMopId_e eMopId, ss_phys_addr_t u64PhyAddr)
{
    u32   u32RegVal;
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_47_L : REG_DISP_MOP_ROT_BK0_47_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_48_L : REG_DISP_MOP_ROT_BK0_48_L;

    u32RegVal = (u32)(u64PhyAddr >> HAL_DISP_MOP_MIU_BUS);
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u32RegVal & 0xFFFF), 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u32RegVal >> 16) & 0xFFFF), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot0SetRotateMode(void *pCtx, HalDispMopId_e eMopId, HalDispRotateMode_e eRotAng)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOPS_BK1_51_L : REG_DISP_MOPS_BK0_51_L;
    if ((eRotAng == E_HAL_DISP_ROTATE_90) || (eRotAng == E_HAL_DISP_ROTATE_270) || (eRotAng == E_HAL_DISP_ROTATE_NONE))
    {
        if (pCmdqCtx)
        {
            HalDispUtilityW2BYTEMSK(u32MopRegBase, (eRotAng == E_HAL_DISP_ROTATE_270) ? DISP_BIT1 : 0, DISP_BIT1,
                                    pCmdqCtx);
        }
        //_HalMopSwShadowHandler(REG_DISP_MOPS_BK0_BASE, E_HAL_DISP_LAYER_MOPS_51, (eRotAng == E_HAL_DISP_ROTATE_270)?
        //DISP_BIT1 : 0, DISP_BIT1);
    }
    else
    {
        DISP_ERR("[HALMOP]%s %d, Rotate ID %d not support\n", __FUNCTION__, __LINE__, eRotAng);
    }
}

void HalDispMopRot1SetRotateMode(void *pCtx, HalDispMopId_e eMopId, HalDispRotateMode_e eRotAng)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOPS_BK1_51_L : REG_DISP_MOPS_BK0_51_L;
    if ((eRotAng == E_HAL_DISP_ROTATE_90) || (eRotAng == E_HAL_DISP_ROTATE_270) || (eRotAng == E_HAL_DISP_ROTATE_NONE))
    {
        if (pCmdqCtx)
        {
            HalDispUtilityW2BYTEMSK(u32MopRegBase, (eRotAng == E_HAL_DISP_ROTATE_270) ? DISP_BIT2 : 0, DISP_BIT2,
                                    pCmdqCtx);
        }
        //_HalMopSwShadowHandler(REG_DISP_MOPS_BK0_BASE, E_HAL_DISP_LAYER_MOPS_51, (eRotAng == E_HAL_DISP_ROTATE_270)?
        //DISP_BIT2 : 0, DISP_BIT2);
    }
    else
    {
        DISP_ERR("[HALMOP]%s %d, Rotate ID %d not support\n", __FUNCTION__, __LINE__, eRotAng);
    }
}

void HalDispMopRot0SetPixDummy(void *pCtx, HalDispMopId_e eMopId, u16 u16DummyPix)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_16_L : REG_DISP_MOP_ROT_BK0_16_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_26_L : REG_DISP_MOP_ROT_BK0_26_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u16DummyPix << 12), 0xF000, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u16DummyPix / 2) << 12), 0x7000, pCmdqCtx);
    }
}

void HalDispMopRot1SetPixDummy(void *pCtx, HalDispMopId_e eMopId, u16 u16DummyPix)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_36_L : REG_DISP_MOP_ROT_BK0_36_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_46_L : REG_DISP_MOP_ROT_BK0_46_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u16DummyPix << 12), 0xF000, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, ((u16DummyPix / 2) << 12), 0x7000, pCmdqCtx);
    }
}

void HalDispMopRot0SetRotateCropXEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2A_L : REG_DISP_MOP_ROT_BK0_2A_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (bEn << 15), 0x8000, pCmdqCtx);
    }
}
void HalDispMopRot0SetRotateCropYEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2A_L : REG_DISP_MOP_ROT_BK0_2A_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (bEn << 14), 0x4000, pCmdqCtx);
    }
}

void HalDispMopRot1SetRotateCropXEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4A_L : REG_DISP_MOP_ROT_BK0_4A_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (bEn << 15), 0x8000, pCmdqCtx);
    }
}
void HalDispMopRot1SetRotateCropYEn(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
    u32   u32MopRegBase = 0;
    void *pCmdqCtx      = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4A_L : REG_DISP_MOP_ROT_BK0_4A_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (bEn << 14), 0x4000, pCmdqCtx);
    }
}

void HalDispMopRot0SetRotateCropXClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2A_L : REG_DISP_MOP_ROT_BK0_2A_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2B_L : REG_DISP_MOP_ROT_BK0_2B_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u16ClipNum >> 4), 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16ClipNumCmp >> 4), 0x0FFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetRotateCropXClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4A_L : REG_DISP_MOP_ROT_BK0_4A_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4B_L : REG_DISP_MOP_ROT_BK0_4B_L;
    if (pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(u32MopRegBase, (u16ClipNum >> 4), 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16ClipNumCmp >> 4), 0x0FFF, pCmdqCtx);
    }
}

void HalDispMopRot0SetRotateCropYClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    u32   u32MopRegBase3 = 0;
    u32   u32MopRegBase4 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2C_L : REG_DISP_MOP_ROT_BK0_2C_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2D_L : REG_DISP_MOP_ROT_BK0_2D_L;
    u32MopRegBase3 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2E_L : REG_DISP_MOP_ROT_BK0_2E_L;
    u32MopRegBase4 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_2F_L : REG_DISP_MOP_ROT_BK0_2F_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16ClipNum, 0xFFFF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16ClipNum / 2), 0xFFFF, pCmdqCtx);
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase3, u16ClipNumCmp, 0xFFFF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase4, (u16ClipNumCmp / 2), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRot1SetRotateCropYClipNum(void *pCtx, HalDispMopId_e eMopId, u16 u16ClipNum, u16 u16ClipNumCmp)
{
    u32   u32MopRegBase  = 0;
    u32   u32MopRegBase2 = 0;
    u32   u32MopRegBase3 = 0;
    u32   u32MopRegBase4 = 0;
    void *pCmdqCtx       = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase  = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4C_L : REG_DISP_MOP_ROT_BK0_4C_L;
    u32MopRegBase2 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4D_L : REG_DISP_MOP_ROT_BK0_4D_L;
    u32MopRegBase3 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4E_L : REG_DISP_MOP_ROT_BK0_4E_L;
    u32MopRegBase4 = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_4F_L : REG_DISP_MOP_ROT_BK0_4F_L;
    if (pCmdqCtx)
    {
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase, u16ClipNum, 0xFFFF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase2, (u16ClipNum / 2), 0xFFFF, pCmdqCtx);
        // Y
        HalDispUtilityW2BYTEMSK(u32MopRegBase3, u16ClipNumCmp, 0xFFFF, pCmdqCtx);
        // C
        HalDispUtilityW2BYTEMSK(u32MopRegBase4, (u16ClipNumCmp / 2), 0xFFFF, pCmdqCtx);
    }
}

void HalDispMopRotInit(void *pCtx)
{
    HalDispMopId_e eMopId           = 0;
    u32            u32MopRotRegBase = 0;
    void *         pCmdqCtx         = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    for (eMopId = E_HAL_DISP_MOPID_00; eMopId <= E_HAL_DISP_MOPID_01; eMopId++)
    {
        u32MopRotRegBase = (eMopId == E_HAL_DISP_MOPID_01) ? REG_DISP_MOP_ROT_BK1_BASE : REG_DISP_MOP_ROT_BK0_BASE;
        // rot0 Y
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x12) * 2, 0x0820, 0x3F3F,
                                      pCmdqCtx); //[13:8]reg_rot0_rdma_length_y=0x8; [5:0]reg_rot0_rdma_thd_y=0x20
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x13) * 2, 0x101F, 0xBFFF,
                                      pCmdqCtx); //[15]reg_rot0_rdma_hp_en_y=0x1; [13:8]reg_rot0_rdma_hp_thd_y=0x20;
                                                 //[7:0]reg_rot0_rdma_stop_y=0x1F
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x16) * 2, 0x0008, 0x0F0F,
                                      pCmdqCtx); //[11:8]reg_rot0_burst_wr_thd_y=0x8; [7:0]reg_rot0_pre_rdy_thd_y=0x1

        // rot0 C
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x22) * 2, 0x0820, 0x3F3F,
                                      pCmdqCtx); //[13:8]reg_rot0_rdma_length_c=0x8; [5:0]reg_rot0_rdma_thd_c=0x20
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x23) * 2, 0x101F, 0xBFFF,
                                      pCmdqCtx); //[15]reg_rot0_rdma_hp_en_c=0x1; [13:8]reg_rot0_rdma_hp_thd_c=0x20;
                                                 //[7:0]reg_rot0_rdma_stop_c=0x1F
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x26) * 2, 0x0008, 0x0F0F,
                                      pCmdqCtx); //[11:8]reg_rot0_burst_wr_thd_c=0x8; [7:0]reg_rot0_pre_rdy_thd_c=0x1

        // rot1 Y
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x32) * 2, 0x0820, 0x3F3F,
                                      pCmdqCtx); //[13:8]reg_rot1_rdma_length_y=0x8; [5:0]reg_rot1_rdma_thd_y=0x20
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x33) * 2, 0x101F, 0xBFFF,
                                      pCmdqCtx); //[15]reg_rot1_rdma_hp_en_y=0x1; [13:8]reg_rot1_rdma_hp_thd_y=0x20;
                                                 //[7:0]reg_rot1_rdma_stop_y=0x1F
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x36) * 2, 0x0008, 0x0F0F,
                                      pCmdqCtx); //[11:8]reg_rot1_burst_wr_thd_y=0x8; [7:0]reg_rot1_pre_rdy_thd_y=0x1

        // rot1 C
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x42) * 2, 0x0820, 0x3F3F,
                                      pCmdqCtx); //[13:8]reg_rot1_rdma_length_c=0x8; [5:0]reg_rot1_rdma_thd_c=0x20
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x43) * 2, 0x101F, 0xBFFF,
                                      pCmdqCtx); //[15]reg_rot1_rdma_hp_en_c=0x1; [13:8]reg_rot1_rdma_hp_thd_c=0x20;
                                                 //[7:0]reg_rot1_rdma_stop_c=0x1F
        HalDispUtilityW2BYTEMSKDirect(u32MopRotRegBase + (0x46) * 2, 0x0008, 0x0F0F,
                                      pCmdqCtx); //[11:8]reg_rot1_burst_wr_thd_c=0x8; [7:0]reg_rot1_pre_rdy_thd_c=0x1

        // open double buffer write
        if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_DBLBF_WRITE_MD)
        {
            HalDispMopRot0DbfEn(eMopId, TRUE, pCmdqCtx);
            _HalDispMopRotDbBfWrWithoutCmdq(pCtx, eMopId, E_HAL_DISP_MOPROT_ROT0_ID);
            HalDispMopRot1DbfEn(eMopId, TRUE, pCmdqCtx);
            _HalDispMopRotDbBfWrWithoutCmdq(pCtx, eMopId, E_HAL_DISP_MOPROT_ROT1_ID);
        }
        else
        {
            HalDispMopRot0DbfEn(eMopId, FALSE, pCmdqCtx);
            HalDispMopRot1DbfEn(eMopId, FALSE, pCmdqCtx);
        }
    }
}

void HalDispMopSetForceWrite(void *pCtx, HalDispMopId_e eMopId, u8 bEn)
{
#if (HAL_DISP_MOP_WRITE_MD == HAL_DISP_MOP_DBLBF_WRITE_MD)
    u32 u32MopRegBase = 0;

    void *pCmdqCtx = NULL;
    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    u32MopRegBase = (eMopId == E_HAL_DISP_MOPID_01)   ? REG_DISP_MOPG_BK10_7F_L
                    : (eMopId == E_HAL_DISP_MOPID_02) ? REG_DISP_MOPG_BK20_7F_L
                                                      : REG_DISP_MOPG_BK00_7F_L;

    HalDispUtilityW2BYTEMSKDirect(u32MopRegBase, bEn ? DISP_BIT9 : 0, DISP_BIT9, pCmdqCtx);
#endif
}
