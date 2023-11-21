/*
* hal_cover.c- Sigmastar
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

#define __HAL_COVER_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "hal_cover.h"
#include "hal_cover_reg.h"
#define Enable_Cover_DB 0
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
void (*gCovpfForSet[E_HAL_QUERY_COV_MAX])(HalCoverIdType_e, void *); //*pCfg
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
u32 _HalCoverGetBaseAddr(HalCoverIdType_e eCoverId)
{
    u32 u32Base = REG_ISP_COVER_BASE_0;

    switch(eCoverId)
    {
        case E_HAL_ISPSCL0_COVER:
            u32Base = REG_ISP_COVER_BASE_0;
            break;
        case E_HAL_ISPSCL1_COVER:
            u32Base = REG_ISP_COVER_BASE_1;
            break;
        case E_HAL_ISPSCL2_COVER:
            u32Base = REG_ISP_COVER_BASE_2;
            break;
        case E_HAL_ISPSCL3_COVER:
            u32Base = REG_ISP_COVER_BASE_3;
            break;
        case E_HAL_ISPSCL4_COVER:
            u32Base = REG_ISP_COVER_BASE_4;
            break;
        case E_HAL_ISPSCL5_COVER:
        case E_HAL_DIP_COVER_ID:
            u32Base = REG_ISP_COVER_BASE_5;
            break;
        default:
            HALRGNERR("[%s]Wrong eCoverId=%d",__FUNCTION__,eCoverId);
            CamOsPanic("");
    }
    return u32Base;
}

u16 _HalCoverGetDoubleRW(HalCoverIdType_e eCoverId)
{
    u16 u16DoubleRW = REG_ISPSC_COVER_BWIN_TRIG;

    switch(eCoverId)
    {
        case E_HAL_ISPSCL0_COVER:
        case E_HAL_ISPSCL1_COVER:
        case E_HAL_ISPSCL2_COVER:
        case E_HAL_ISPSCL3_COVER:
        case E_HAL_ISPSCL4_COVER:
        case E_HAL_ISPSCL5_COVER:
        case E_HAL_DIP_COVER_ID:
            u16DoubleRW = REG_DIP_COVER_BWIN_TRIG;
            break;
        default:
            HALRGNERR("[%s]Wrong eCoverId=%d",__FUNCTION__,eCoverId);
            CamOsPanic("");
    }

    return u16DoubleRW;

}

bool _HalCoverIdTransCmdqID(HalCoverIdType_e eCoverId,HalCoverCmdqIdType_e *pCmdqId)
{
    bool bRet = FALSE;
    switch(eCoverId)
    {
        case E_HAL_ISPSCL0_COVER:
        case E_HAL_ISPSCL1_COVER:
        case E_HAL_ISPSCL2_COVER:
        case E_HAL_ISPSCL3_COVER:
        case E_HAL_ISPSCL4_COVER:
        case E_HAL_ISPSCL5_COVER:
            *pCmdqId = E_HAL_COVER_CMDQ_VPE_ID_0;
            bRet = TRUE;
            break;
        case E_HAL_DIP_COVER_ID:
            *pCmdqId = E_HAL_COVER_CMDQ_DIVP_ID_1;
            bRet = TRUE;
            break;
        default:
            HALRGNDBG("[COVER]%s %d: wrong CoverId\n", __FUNCTION__, __LINE__);
            bRet = FALSE;
            break;
    }
    return bRet;
}
void _HalCoverWriteDoubleBuffer(u32 COVER_Reg_Base, HalCoverIdType_e eCoverId,HalRgnCmdqIdType_e eCmdqId)
{
    #if Enable_Cover_DB
    u16 COVER_Reg_DB   = 0;
    COVER_Reg_DB = _HalCoverGetDoubleRW(eCoverId);
    // Double Buffer Write
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_DB, MSK_COVER_DB_EN, MSK_COVER_DB_EN, eCmdqId);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_DB, 0, MSK_COVER_BWIN_TRIG, eCmdqId);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_DB, 1, MSK_COVER_BWIN_TRIG, eCmdqId);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_DB, 0, MSK_COVER_BWIN_TRIG, eCmdqId);
    #endif
}
void _HalCoverSwitchRegBase(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, u32 *COVER_Reg_Base)
{
    switch(eCoverId)
    {
        case E_HAL_ISPSCL0_COVER:
        case E_HAL_ISPSCL1_COVER:
        case E_HAL_ISPSCL2_COVER:
        case E_HAL_ISPSCL3_COVER:
        case E_HAL_ISPSCL4_COVER:
        case E_HAL_ISPSCL5_COVER:
        case E_HAL_DIP_COVER_ID:
            *COVER_Reg_Base += REG_COVER_BWIN_SHIFT*eWinId;
            break;
        default:
            HALRGNERR("[%s]Wrong eCoverId=%d",__FUNCTION__,eCoverId);
            CamOsPanic("");
    }

}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HalCoverSetWindowSize(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverWindowType_t *ptWinCfg)
{
    u32 COVER_Reg_Base = 0;
    u32 COVER_Reg_HStart_Base = 0;
    u32 COVER_Reg_VStart_Base = 0;
    u32 COVER_Reg_HEnd_Base = 0;
    u32 COVER_Reg_VEnd_Base = 0;
    u16 u16DispImage_HStart, u16DispImage_HEnd;
    u16 u16DispImage_VStart, u16DispImage_VEnd;
    HalCoverCmdqIdType_e eCmdqId;
    if(!_HalCoverIdTransCmdqID(eCoverId,&eCmdqId))
    {
        return;
    }
    if(eWinId==E_HAL_COVER_BWIN_ID_NUM)
    {
        HALRGNERR("[%s]Wrong eWinId=%d",__FUNCTION__,eWinId);
        CamOsPanic("");
        return;
    }
    COVER_Reg_Base = _HalCoverGetBaseAddr(eCoverId);
    COVER_Reg_HStart_Base = REG_ISPSC_COVER_BWIN_H_STR;
    COVER_Reg_HEnd_Base   = REG_ISPSC_COVER_BWIN_H_END;
    COVER_Reg_VStart_Base = REG_ISPSC_COVER_BWIN_V_STR;
    COVER_Reg_VEnd_Base   = REG_ISPSC_COVER_BWIN_V_END;
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_HStart_Base);
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_HEnd_Base);
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_VStart_Base);
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_VEnd_Base);
    u16DispImage_HStart = ptWinCfg->u16X;
    u16DispImage_VStart = ptWinCfg->u16Y;
    u16DispImage_HEnd   = ptWinCfg->u16X + ptWinCfg->u16Width;
    u16DispImage_VEnd   = ptWinCfg->u16Y + ptWinCfg->u16Height;
    // cover bwin settings
    HalRgnWrite2Byte(COVER_Reg_Base + COVER_Reg_HStart_Base, u16DispImage_HStart, eCmdqId);
    HalRgnWrite2Byte(COVER_Reg_Base + COVER_Reg_HEnd_Base  , u16DispImage_HEnd  , eCmdqId);
    HalRgnWrite2Byte(COVER_Reg_Base + COVER_Reg_VStart_Base, u16DispImage_VStart, eCmdqId);
    HalRgnWrite2Byte(COVER_Reg_Base + COVER_Reg_VEnd_Base  , u16DispImage_VEnd  , eCmdqId);
    _HalCoverWriteDoubleBuffer(COVER_Reg_Base,eCoverId,eCmdqId);

}

void HalCoverSetColor(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverColorType_t *ptType)
{
    u32 COVER_Reg_Base = 0;
    u32 COVER_Reg_Replace_R_Base = 0;
    u32 COVER_Reg_Replace_G_Base = 0;
    u32 COVER_Reg_Replace_B_Base = 0;
    HalCoverCmdqIdType_e eCmdqId;
    if(!_HalCoverIdTransCmdqID(eCoverId,&eCmdqId))
    {
        return;
    }
    if(eWinId==E_HAL_COVER_BWIN_ID_NUM)
    {
        HALRGNERR("[%s]Wrong eWinId=%d",__FUNCTION__,eWinId);
        CamOsPanic("");
        return;
    }
    COVER_Reg_Base = _HalCoverGetBaseAddr(eCoverId);
    COVER_Reg_Replace_R_Base = REG_ISPSC_COVER_BWIN_REPLACE_R;
    COVER_Reg_Replace_G_Base = REG_ISPSC_COVER_BWIN_REPLACE_G;
    COVER_Reg_Replace_B_Base = REG_ISPSC_COVER_BWIN_REPLACE_B;
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_Replace_R_Base);
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_Replace_G_Base);
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_Replace_B_Base);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_Replace_R_Base, ptType->u8R, MSK_COVER_BWIN_REPLACE_R,eCmdqId);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_Replace_G_Base, (ptType->u8G<<8) , MSK_COVER_BWIN_REPLACE_G,eCmdqId);
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_Replace_B_Base, ptType->u8B, MSK_COVER_BWIN_REPLACE_B,eCmdqId);
    _HalCoverWriteDoubleBuffer(COVER_Reg_Base,eCoverId,eCmdqId);
}

void HalCoverSetEnableWin(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, bool bEn)
{
    u32 COVER_Reg_Base = 0;
    u32 COVER_Reg_Enable_Base = 0;
    HalCoverCmdqIdType_e eCmdqId;
    if(!_HalCoverIdTransCmdqID(eCoverId,&eCmdqId))
    {
        return;
    }
    if(eWinId==E_HAL_COVER_BWIN_ID_NUM)
    {
        HALRGNERR("[%s]Wrong eWinId=%d",__FUNCTION__,eWinId);
        CamOsPanic("");
        return;
    }
    COVER_Reg_Base = _HalCoverGetBaseAddr(eCoverId);
    COVER_Reg_Enable_Base = REG_ISPSC_COVER_BWIN_EN;
    _HalCoverSwitchRegBase(eCoverId,eWinId,&COVER_Reg_Enable_Base);
    // enable/disable gwin
    HalRgnWrite2ByteMsk(COVER_Reg_Base + COVER_Reg_Enable_Base, bEn ? 1 : 0,MSK_COVER_BWIN_EN,eCmdqId);
    _HalCoverWriteDoubleBuffer(COVER_Reg_Base,eCoverId,eCmdqId);
    HALRGNDBG("[COV]%s %d bEn=%hhd\n", __FUNCTION__, __LINE__,bEn);
}
void HalCoverInit(void)
{
    HALRGNDBG("[COV]%s %d \n", __FUNCTION__, __LINE__);
    memset(gCovpfForSet, 0, sizeof(gCovpfForSet));
}
bool HalCoverQuery(void *pQF)
{
    bool bRet = 1;
    HalCovQueryFuncConfig_t *pQFunc;

    pQFunc = pQF;
    if(pQFunc->stInQ.enQF<E_HAL_QUERY_COV_MAX)
    {
        pQFunc->stOutQ.pfForSet = gCovpfForSet[pQFunc->stInQ.enQF];
        pQFunc->stOutQ.pfForPatch = NULL;
    }
    if(pQFunc->stOutQ.pfForSet)
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_COV_OK;
    }
    else
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_COV_NOTSUPPORT;
    }
    
    return bRet;
}
bool HalCoverTransHalId(const char* p8Str, HalCoverIdType_e *pCoverId)
{
    bool ret = 1;
    if(!strcmp(p8Str, RGN_DEVICE_SC_PORT0))
    {
        *pCoverId = E_HAL_ISPSCL0_COVER;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT1))
    {
        *pCoverId = E_HAL_ISPSCL1_COVER;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT2))
    {
        *pCoverId = E_HAL_ISPSCL2_COVER;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DIP))
    {
        *pCoverId = E_HAL_DIP_COVER_ID;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT3))
    {
        *pCoverId = E_HAL_ISPSCL3_COVER;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT4))
    {
        *pCoverId = E_HAL_ISPSCL4_COVER;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT5))
    {
        *pCoverId = E_HAL_ISPSCL5_COVER;
    }
    else
    {
        ret = 0;
    }
    return ret;
}

