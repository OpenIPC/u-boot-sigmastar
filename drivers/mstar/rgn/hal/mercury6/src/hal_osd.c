/*
* hal_osd.c- Sigmastar
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

#define __HAL_OSD_C__
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "cam_os_wrapper.h"
#include "hal_osd.h"
#include "hal_osd_reg.h"
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
void (*gOsdpfForSet[E_HAL_QUERY_OSD_MAX])(HalOsdIdType_e, void *); //*pCfg

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
bool _HalOsdIdTransCmdqID(HalOsdIdType_e eOsdId,HalRgnCmdqIdType_e *pCmdqId)
{
    switch(eOsdId)
    {
        case E_HAL_OSD_PORT0:
        case E_HAL_OSD_PORT1:
        case E_HAL_OSD_PORT2:
        case E_HAL_OSD_PORT3:
        case E_HAL_OSD_PORT4:
        case E_HAL_OSD_PORT5:
            *pCmdqId = E_HAL_RGN_CMDQ_VPE_ID_0;
            return TRUE;
        case E_HAL_OSD_DIP:
            *pCmdqId = E_HAL_RGN_CMDQ_DIVP_ID_1;
            return TRUE;
        case E_HAL_OSD_LDC:
            *pCmdqId =  E_HAL_RGN_CMDQ_LDC_ID_2;
            return TRUE;
        case E_HAL_OSD_DISP0_CUR:
        case E_HAL_OSD_DISP0_UI:
        case E_HAL_OSD_DISP1_CUR:
        case E_HAL_OSD_DISP1_UI:
            *pCmdqId =  E_HAL_RGN_CMDQ_DISP_ID_3;
            return TRUE;
        default:
            HALRGNERR("[OSD]%s %d: wrong OSDId\n", __FUNCTION__, __LINE__);
            pCmdqId = NULL;
            break;
    }
    return FALSE;
}
u32 _HalOsdGetBaseAddr(HalOsdIdType_e eOsdId)
{
    u32 u32Base = 0;

    switch(eOsdId)
    {
        case E_HAL_OSD_PORT0:
            u32Base = REG_SCL_TO_GOP_BASE0;
            break;
        case E_HAL_OSD_PORT1:
            u32Base = REG_SCL_TO_GOP_BASE1;
            break;
        case E_HAL_OSD_PORT2:
            u32Base = REG_SCL_TO_GOP_BASE2;
            break;
        case E_HAL_OSD_PORT3:
            u32Base = REG_SCL_TO_GOP_BASE3;
            break;
        case E_HAL_OSD_PORT4:
            u32Base = REG_SCL_TO_GOP_BASE4;
            break;
        case E_HAL_OSD_PORT5:
        case E_HAL_OSD_DIP:
            u32Base = REG_SCL_TO_GOP_BASE5;
            break;
        case E_HAL_OSD_LDC:
            u32Base = REG_LDC_TO_GOP_BASE;
            break;
        case E_HAL_OSD_DISP0_CUR:
        case E_HAL_OSD_DISP0_UI:
        case E_HAL_OSD_DISP1_CUR:
        case E_HAL_OSD_DISP1_UI:
            u32Base = 0; // not support OSDB
            break;
        default:
            HALRGNERR("[%s]Wrong eOsdId=%d",__FUNCTION__,eOsdId);
            CamOsPanic("");
            break;
    }
    return u32Base;
}
u32 _HalGopOsdbRegBaseShift(HalOsdIdType_e eOsdId)
{
    u32 u32RegBaseShift = REG_SCL_EN_OSD;

    switch(eOsdId)
    {
        case E_HAL_OSD_PORT0:
        case E_HAL_OSD_PORT1:
        case E_HAL_OSD_PORT2:
        case E_HAL_OSD_PORT3:
        case E_HAL_OSD_PORT4:
        case E_HAL_OSD_PORT5:
        case E_HAL_OSD_DIP:
            u32RegBaseShift = REG_SCL_EN_OSD;
            break;
        case E_HAL_OSD_LDC:
            u32RegBaseShift = REG_LDC_EN_OSD;
            break;
        case E_HAL_OSD_DISP0_CUR:
        case E_HAL_OSD_DISP0_UI:
        case E_HAL_OSD_DISP1_CUR:
        case E_HAL_OSD_DISP1_UI:
            u32RegBaseShift = 0; // not support OSDB
            break;
        default:
            HALRGNERR("[%s]Wrong eOsdId=%d",__FUNCTION__,eOsdId);
            CamOsPanic("");
            break;

    }
    return u32RegBaseShift;
}
void _HalOsdGetBindReg(u32 u32GopOsdFlag,u16 *u16GopMap)
{
    // GOP Map
    if(u32GopOsdFlag&E_HAL_RGN_OSD_FLAG_LDC)
    {
        *u16GopMap = REG_OSD_TO_GOP_MAP_LDC;
    }
    else
    {
        *u16GopMap = REG_OSD_TO_GOP_MAP_SC;
    }
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool HalOsdTransId(const char* p8Str, HalOsdIdType_e *pHalId)
{
    bool ret = 1;
    if(!strcmp(p8Str, RGN_DEVICE_SC_PORT0))
    {
        *pHalId = E_HAL_OSD_PORT0;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT1))
    {
        *pHalId = E_HAL_OSD_PORT1;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT2))
    {
        *pHalId = E_HAL_OSD_PORT2;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT3))
    {
        *pHalId = E_HAL_OSD_PORT3;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT4))
    {
        *pHalId = E_HAL_OSD_PORT4;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_SC_PORT5))
    {
        *pHalId = E_HAL_OSD_PORT5;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_LDC))
    {
        *pHalId = E_HAL_OSD_LDC;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DIP))
    {
        *pHalId = E_HAL_OSD_DIP;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DISP0_CUR))
    {
        *pHalId = E_HAL_OSD_DISP0_CUR;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DISP0_UI))
    {
        *pHalId = E_HAL_OSD_DISP0_UI;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DISP1_CUR))
    {
        *pHalId = E_HAL_OSD_DISP1_CUR;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DISP1_UI))
    {
        *pHalId = E_HAL_OSD_DISP1_UI;
    }
    else
    {
        ret = 0;
    }
    return ret;
}
bool HalOsdColInvParamCheck(HalOsdColorInvParamConfig_t *ptColInvCfg)
{
    MS_BOOL bRet = TRUE;
    return bRet;
}
void HalOsdSetOsdBypassForScEnable(HalOsdIdType_e eOsdId, bool bEn)
{
}
bool HalOsdSetBindGop(u32 u32GopOsdFlag)
{
    u32 OSD_Reg_Base = 0;
    u32 OSDB_Reg_Shift_Base = 0;
    bool bRet = TRUE;
    u16 u16GopMap = 0;
    HALRGNDBG("[OSD]%s %d: Set BindGop  = [%x]\n", __FUNCTION__, __LINE__, u32GopOsdFlag);
    OSD_Reg_Base = REG_OSD_SET_BIND_GOP_BASE;
    OSDB_Reg_Shift_Base = REG_SCL_BIND_OSD;
    _HalOsdGetBindReg(u32GopOsdFlag,&u16GopMap);
    HalRgnWrite2ByteMsk(OSD_Reg_Base + OSDB_Reg_Shift_Base, u16GopMap, REG_OSD_TO_GOP_MAP_MASK, E_HAL_RGN_CMDQ_ID_NUM);
    return bRet;
}
void HalOsdSetOsdEnable(HalOsdIdType_e eOsdId, bool bEn)
{
    u32 OSD_Reg_Base = 0;
    u32 OSDB_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;

    if(eOsdId==E_HAL_OSD_DISP0_CUR ||
       eOsdId==E_HAL_OSD_DISP0_UI  ||
       eOsdId==E_HAL_OSD_DISP1_CUR ||
       eOsdId==E_HAL_OSD_DISP1_UI)
    {
        HALRGNDBG("[OSD]%s %d: Skip enable_OSD ID:%d = [%d]\n", __FUNCTION__, __LINE__, eOsdId, bEn);
    }
    else
    {
        HALRGNDBG("[OSD]%s %d: Set enable_OSD  ID:%d = [%d]\n", __FUNCTION__, __LINE__, eOsdId, bEn);
        OSD_Reg_Base = _HalOsdGetBaseAddr(eOsdId);
        if(!OSD_Reg_Base)
        {
            return;
        }
        OSDB_Reg_Shift_Base = _HalGopOsdbRegBaseShift(eOsdId);
        _HalOsdIdTransCmdqID(eOsdId,&eCmdqId);
        HALRGNDBG("[OSD]%s %d: OSD_Reg_Base=0x%x, OSDB_Reg_Shift_Base=0x%x\n", __FUNCTION__, __LINE__, OSD_Reg_Base, OSDB_Reg_Shift_Base);
        // enable/disable gop from scaler
        HalRgnWrite2ByteMsk(OSD_Reg_Base + OSDB_Reg_Shift_Base, bEn ? OSD_ENABLE : 0, OSD_ENABLE, eCmdqId);
        // if id is DIP, set DE mode ( osdb en: close DE mode (enable handshake mode); osdb disable: enable DE mode)
        if(OSD_Reg_Base==REG_LDC_TO_GOP_BASE)
        {
            HalRgnWrite2ByteMsk(OSD_Reg_Base + OSDB_Reg_Shift_Base, bEn ? 0 : DIP_DE_MD_EN, DIP_DE_MD_MASK, eCmdqId);
            // osdb en: reg_dip_osd_vs_inv=0; osdb disable: reg_dip_osd_vs_inv=1)
            HalRgnWrite2ByteMsk(OSD_Reg_Base + OSDB_Reg_Shift_Base, bEn ? 0 : DIP_VS_INV_EN, DIP_VS_INV_MASK, eCmdqId);
        }
        else // if id is scaler, set reg_osd_pipe_guard_cycle
        {
            //HalRgnWrite2ByteMsk(OSD_Reg_Base + OSDB_Reg_Shift_Base + REG_OSD_01, bEn ? REG_SCL_PIPE_GUARD_CYCLE : 0, REG_SCL_PIPE_GUARD_CYCLE_MASK, eCmdqId);
        }
    }
}
void HalOsdInit(void)
{
    memset(gOsdpfForSet, 0, sizeof(gOsdpfForSet));
}

bool HalOsdQuery(void *pQF)
{
    bool bRet = 1;
    HalOsdQueryFuncConfig_t *pQFunc;

    pQFunc = pQF;
    if(pQFunc->stInQ.enQF<E_HAL_QUERY_OSD_MAX)
    {
        pQFunc->stOutQ.pfForSet = gOsdpfForSet[pQFunc->stInQ.enQF];
        pQFunc->stOutQ.pfForPatch = NULL;
    }
    if(pQFunc->stOutQ.pfForSet)
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_OSD_OK;
    }
    else
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_OSD_NOTSUPPORT;
    }

    return bRet;
}

