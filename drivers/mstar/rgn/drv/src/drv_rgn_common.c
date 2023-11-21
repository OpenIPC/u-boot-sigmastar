/*
* drv_rgn_common.c- Sigmastar
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
#define __DRV_RGN_COMMON_C__


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_rgn_common.h"
#include "rgn_sysfs.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

bool _DrvRgnTransCmdqId(DrvRgnCmdqIdType_e eCmdqId,HalRgnCmdqIdType_e *pHalCmdqId)
{
    bool bRet = TRUE;
    bool str[8];

    if(eCmdqId == E_DRV_RGN_VPE_CMDQ_ID_0)
    {
        strcpy((char *)str,RGN_DEVICE_SC);
    }
    else if(eCmdqId == E_DRV_RGN_DIVP_CMDQ_ID_1)
    {
        strcpy((char *)str,RGN_DEVICE_DIP);
    }
    else if(eCmdqId == E_DRV_RGN_LDC_CMDQ_ID_2)
    {
        strcpy((char *)str,RGN_DEVICE_LDC);
    }
    else if(eCmdqId == E_DRV_RGN_DISP_CMDQ_ID_3)
    {
        strcpy((char *)str,RGN_DEVICE_DISP);
    }
    else
    {
        return FALSE;
    }
    bRet = HalRgnTransCmdqId((const char*)str,pHalCmdqId);
    return bRet;
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool DrvRgnUpdateReg(void)
{
    HalRgnUpdateReg();
    return 1;
}
bool DrvRgnInit(void)
{
    HalRgnInit();
    HalRgnInitCmdq();
    return 1;
}
bool DrvRgnDeinit(void)
{
    HalRgnDeinit();
    HalRgnInitCmdq();
    return 1;
}
bool DrvRgnSetOsdBindGop(unsigned int eGopNum)
{
    HalRgnOsdFlag_e eOsdNum = eGopNum;
    if(HalRgnCheckBindRation(&eOsdNum))
    {
        geGopOsdFlag = eGopNum;
        return 1;
    }
    else
    {
        return 0;
    }
}
bool DrvRgnGetOsdBindGop(unsigned int *eGopNum)
{
    *eGopNum = geGopOsdFlag;
    return 1;
}
void DrvRgnUseCmdq(bool bEn)
{
    HalRgnUseCmdq(bEn);
}
bool DrvRgnSetCmdq(MHAL_CMDQ_CmdqInterface_t *pstCmdq,DrvRgnCmdqIdType_e eCmdqId)
{
    bool bRet = FALSE;
    HalRgnCmdqIdType_e eHalCmdqId;
    if(_DrvRgnTransCmdqId(eCmdqId, &eHalCmdqId) == TRUE)
    {
        HalRgnSetCmdq(pstCmdq,eHalCmdqId);
        bRet = TRUE;
    }
    return bRet;
}
