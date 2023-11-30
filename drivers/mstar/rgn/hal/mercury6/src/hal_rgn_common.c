/*
* hal_rgn_common.c- Sigmastar
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

#define __HAL_RGN_COMMON_C__
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "hal_rgn_common.h"
#include "hal_rgn_util.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#define RGN_REG_CMDQ_MD (gbUseCmdq)
#define RGN_CMDQ_ONESHOTLOCK() //(CamOsTsemDown(&gstRgnTsem))
#define RGN_CMDQ_ONESHOTUNLOCK() //(CamOsTsemUp(&gstRgnTsem))
#define RGN_CMDQ_ONESHOTCNT (32)
#define RGN_CMDQ_ST_SIZE() (sizeof(MHAL_CMDQ_MultiCmdBufMask_t))
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
MHAL_CMDQ_CmdqInterface_t _tRgnCmdqLocal[E_HAL_RGN_CMDQ_ID_NUM];
MHAL_CMDQ_MultiCmdBufMask_t *pgstCmdqBuf[E_HAL_RGN_CMDQ_ID_NUM] = {NULL,NULL};
u32 gu32CmdCnt[E_HAL_RGN_CMDQ_ID_NUM];
//CamOsTsem_t gstRgnTsem;
bool gbUseCmdq = 0;
bool gbrgndbglv = 0;
void (*gRgnpfForSet[E_HAL_QUERY_RGN_MAX])(HalRgnOsdFlag_e, void *); //*pCfg

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void HalRgnUpdateReg(void)
{
    HalRgnCmdqIdType_e enType;
    RGN_CMDQ_ONESHOTLOCK();
    for(enType=E_HAL_RGN_CMDQ_VPE_ID_0;enType<E_HAL_RGN_CMDQ_ID_NUM;enType++)
    {
        if(pgstCmdqBuf[enType] && gu32CmdCnt[enType]>0 && _tRgnCmdqLocal[enType].MHAL_CMDQ_WriteRegCmdqMaskMulti)
        {
            _tRgnCmdqLocal[enType].MHAL_CMDQ_WriteRegCmdqMaskMulti(&_tRgnCmdqLocal[enType],pgstCmdqBuf[enType],gu32CmdCnt[enType]);
            gu32CmdCnt[enType] = 0;
        }
    }
    RGN_CMDQ_ONESHOTUNLOCK();
}
bool HalRgnCheckBindRation(HalRgnOsdFlag_e *enFlag)
{
    bool bRet = 1;
    u32 u32Flag = *enFlag;
    u32 u32DefBind = (E_HAL_RGN_OSD_FLAG_PORT0|E_HAL_RGN_OSD_FLAG_PORT1|E_HAL_RGN_OSD_FLAG_PORT2|
                      E_HAL_RGN_OSD_FLAG_PORT3|E_HAL_RGN_OSD_FLAG_PORT4|
                      E_HAL_RGN_OSD_FLAG_DISP0_CUR|E_HAL_RGN_OSD_FLAG_DISP0_UI|
                      E_HAL_RGN_OSD_FLAG_DISP1_CUR|E_HAL_RGN_OSD_FLAG_DISP1_UI);

    if((u32Flag!=(u32DefBind|E_HAL_RGN_OSD_FLAG_PORT5))&&
        (u32Flag!=(u32DefBind|E_HAL_RGN_OSD_FLAG_DIP))&&
        (u32Flag!=(u32DefBind|E_HAL_RGN_OSD_FLAG_LDC)))
    {
        bRet = 0;
        HALRGNERR("[GOP]%s %d: Bind Fail \n", __FUNCTION__, __LINE__);
    }
    else
    {
        bRet = 1;
    }
    return bRet;
}
void HalRgnWrite2Byte(u32 u32Reg, u16 u16Val,HalRgnCmdqIdType_e eCmdqId)
{
    if(pgstCmdqBuf[eCmdqId] && eCmdqId < E_HAL_RGN_CMDQ_ID_NUM && RGN_REG_CMDQ_MD && _tRgnCmdqLocal[eCmdqId].MHAL_CMDQ_WriteRegCmdq)
    {
        //_tRgnCmdqLocal[eCmdqId].MHAL_CMDQ_WriteRegCmdq(&_tRgnCmdqLocal[eCmdqId],u32Reg,u16Val);
        RGN_CMDQ_ONESHOTLOCK();
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u16Mask = 0xFFFF;
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u16RegValue = u16Val;
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u32RegAddr = u32Reg;
        gu32CmdCnt[eCmdqId]++;
        if(gu32CmdCnt[eCmdqId]==RGN_CMDQ_ONESHOTCNT)
        {
            HALRGNERR("[GOP]Cnt overflow\n");
        }
        RGN_CMDQ_ONESHOTUNLOCK();
    }
    else
    {
        W2BYTE(u32Reg, u16Val);
        if((u32Reg&0xFF) != 0xFE)
        HALHLRGNDBG("[GOP]%s %d: reg:%x val:%hx \n", __FUNCTION__, __LINE__,(u32Reg&0xFFFF00) + ((u32Reg&0xFF)/2),u16Val);
    }
}
u16 HalRgnRead2Byte(u32 u32Reg)
{
    return R2BYTE(u32Reg);
}

void HalRgnWrite2ByteMsk(u32 u32Reg, u16 u16Val, u16 u16Mask,HalRgnCmdqIdType_e eCmdqId)
{
    HALREGRGNDGB("[GOP]reg:0x%06x val:0x%04x mask:0x%04x\n", (u32Reg&0xFFFF00) + ((u32Reg&0xFF)/2),u16Val,u16Mask);
    if(pgstCmdqBuf[eCmdqId] && eCmdqId < E_HAL_RGN_CMDQ_ID_NUM && RGN_REG_CMDQ_MD && _tRgnCmdqLocal[eCmdqId].MHAL_CMDQ_WriteRegCmdqMask)
    {
        //_tRgnCmdqLocal[eCmdqId].MHAL_CMDQ_WriteRegCmdqMask(&_tRgnCmdqLocal[eCmdqId],u32Reg,u16Val,u16Mask);
        RGN_CMDQ_ONESHOTLOCK();
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u16RegValue = u16Val;
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u16Mask = u16Mask;
        pgstCmdqBuf[eCmdqId][gu32CmdCnt[eCmdqId]].u32RegAddr = u32Reg;
        gu32CmdCnt[eCmdqId]++;
        if(gu32CmdCnt[eCmdqId]==RGN_CMDQ_ONESHOTCNT)
        {
            HALRGNERR("[GOP]Cnt overflow\n");
        }
        RGN_CMDQ_ONESHOTUNLOCK();
    }
    else
    {
        //u32ValandMask = (u32)(u16Val | (u32)((~u16Mask)<<16));
        W2BYTEMSK(u32Reg, u16Val,u16Mask);
        //if((u32Reg&0xFF) != 0xFE)
        //HALHLRGNDBG("[GOP]%s %d: reg:%x val:%hx mask:%hx\n", __FUNCTION__, __LINE__,(u32Reg&0xFFFF00) + ((u32Reg&0xFF)/2),u16Val,u16Mask);
    }
}
void HalRgnInitCmdq(void)
{
    memset(&_tRgnCmdqLocal[0],0,sizeof(MHAL_CMDQ_CmdqInterface_t)*E_HAL_RGN_CMDQ_ID_NUM);
    HALRGNDBG("[GOP]%s %d: out \n", __FUNCTION__, __LINE__);
}
void HalRgnUseCmdq(bool bEn)
{
    gbUseCmdq = bEn;
}

void HalRgnInit(void)
{
    HalRgnCmdqIdType_e enType;
    // if multi cmdq for vpe,maybe need to mutex lock.
    //CamOsTsemInit(&gstRgnTsem,1);
    RGN_CMDQ_ONESHOTLOCK();
    for(enType=E_HAL_RGN_CMDQ_VPE_ID_0;enType<E_HAL_RGN_CMDQ_ID_NUM;enType++)
    {
        if(!pgstCmdqBuf[enType])
        {
            pgstCmdqBuf[enType] = CamOsMemAlloc(RGN_CMDQ_ST_SIZE()*RGN_CMDQ_ONESHOTCNT);
            gu32CmdCnt[enType] = 0;
        }
    }
    HalRgnUseCmdq(1);
    RGN_CMDQ_ONESHOTUNLOCK();
}
void HalRgnDeinit(void)
{
    HalRgnCmdqIdType_e enType;
    // if multi cmdq for vpe,maybe need to mutex lock.
    RGN_CMDQ_ONESHOTLOCK();
    for(enType=E_HAL_RGN_CMDQ_VPE_ID_0;enType<E_HAL_RGN_CMDQ_ID_NUM;enType++)
    {
        if(pgstCmdqBuf[enType])
        {
            CamOsMemRelease(pgstCmdqBuf[enType]);
            pgstCmdqBuf[enType] = NULL;
            gu32CmdCnt[enType] = 0;
        }
    }
    HalRgnUseCmdq(0);
    RGN_CMDQ_ONESHOTUNLOCK();
    //CamOsTsemDeinit(&gstRgnTsem);
}

void HalRgnSetCmdq(MHAL_CMDQ_CmdqInterface_t *pstCmdq,HalRgnCmdqIdType_e eHalCmdqId)
{
    if(eHalCmdqId >= E_HAL_RGN_CMDQ_ID_NUM)
    {
        HALRGNERR("[GOP]%s %d: wrong CMDQId\n", __FUNCTION__, __LINE__);
        return ;
    }
    if(pstCmdq)
    {
        memcpy(&_tRgnCmdqLocal[eHalCmdqId],pstCmdq,sizeof(MHAL_CMDQ_CmdqInterface_t));
    }
    else
    {
        if(_tRgnCmdqLocal[eHalCmdqId].MHAL_CMDQ_WriteRegCmdqMask != NULL)
        {
            memset(&_tRgnCmdqLocal[eHalCmdqId],0,sizeof(MHAL_CMDQ_CmdqInterface_t));
        }
    }
}
bool HalRgnTransCmdqId(const char* p8Str, HalRgnCmdqIdType_e *pHalCmdqId)
{
    bool ret = 1;
    if(!strcmp(p8Str, RGN_DEVICE_SC))
    {
        *pHalCmdqId = E_HAL_RGN_CMDQ_VPE_ID_0;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DIP))
    {
        *pHalCmdqId = E_HAL_RGN_CMDQ_DIVP_ID_1;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_LDC))
    {
        *pHalCmdqId = E_HAL_RGN_CMDQ_LDC_ID_2;
    }
    else if(!strcmp(p8Str, RGN_DEVICE_DISP))
    {
        *pHalCmdqId = E_HAL_RGN_CMDQ_DISP_ID_3;
    }
    else
    {
        ret = 0;
    }
    return ret;
}
bool HalRgnQuery(void *pQF)
{
    bool bRet = 1;
    HalRgnQueryFuncConfig_t *pQFunc;

    pQFunc = pQF;
    if(pQFunc->stInQ.enQF<E_HAL_QUERY_RGN_MAX)
    {
        pQFunc->stOutQ.pfForSet = gRgnpfForSet[pQFunc->stInQ.enQF];
        pQFunc->stOutQ.pfForPatch = NULL;
    }
    if(pQFunc->stOutQ.pfForSet)
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_RGN_OK;
    }
    else
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_RGN_NOTSUPPORT;
    }
    
    return bRet;
}

