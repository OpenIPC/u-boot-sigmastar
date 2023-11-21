/*
* hal_disp_mop_if.c- Sigmastar
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

#define _HAL_DISP_MOP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "hal_disp_common.h"

#include "hal_disp_mop.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PARSING_MOP_ROT_ANGLE(x) (\
                            x == E_HAL_DISP_ROTATE_NONE    ? "Rotate 0"   :   \
                            x == E_HAL_DISP_ROTATE_90      ? "Rotate 90"  :   \
                            x == E_HAL_DISP_ROTATE_180     ? "Rotate 180" :   \
                            x == E_HAL_DISP_ROTATE_270     ? "Rotate 270" :   \
                            "UNKNOWN")

#define Count_str_align(crop_x) ((crop_x>>4)<<4)

#define Count_end_align(crop_x) (((crop_x)&0xF) ? ((((crop_x)>>4)+1) <<4): (crop_x))


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalDispMopgGwinId_e eMopgId;
    u16 u16GwinHst;
    u8 u8RealOrder;
    bool bIsAttred;
    u16 u16GwinVst;
}HalDispMopgLineBufOrder_t;

//mopg support only one window do rotate at the same time, so record it
typedef struct
{
    bool bRotEn;
    HalDispMopgGwinId_e eMopgId;
    HalDispRotateMode_e stRotAng;
}HalDispMopgRotateRecord_t;


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8 gbMopgInitNum[E_HAL_DISP_MOP_NUM] = {0};//for InputPort
HalDispMopgLineBufOrder_t gpMopgLbOdTbl[E_HAL_DISP_MOP_NUM][E_HAL_DISP_MOPG_ID_NUM];//for InputPort counting mopg line buffer start
//rotate record for MOPG
HalDispMopgRotateRecord_t _gstMopgRotRec[E_HAL_DISP_MOP_NUM];
//rotate use imi address
void* u32MopRotImiTotalAddr;
u32 u32RotImiIniaddr = 0;
u32 u32Rot0ImiYaddr[E_HAL_DISP_MOP_NUM];
u32 u32Rot0ImiCaddr[E_HAL_DISP_MOP_NUM];
u32 u32Rot1ImiYaddr[E_HAL_DISP_MOP_NUM];
u32 u32Rot1ImiCaddr[E_HAL_DISP_MOP_NUM];
bool bImiInit=0;
bool bImiSetAddrInit=0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

HalDispMopId_e _HalDispMopIfMopIdMap(HalDispVideoLayerId_e eVdLyId)
{
    HalDispMopId_e eHalMopId = 0;
    switch(eVdLyId)
    {
        case E_HAL_DISP_LAYER_MOPG_ID:
        case E_HAL_DISP_LAYER_MOPS_ID:
            eHalMopId = E_HAL_DISP_MOPID_00;
            break;
        case E_HAL_DISP_LAYER_MOPG_VIDEO1_ID:
        case E_HAL_DISP_LAYER_MOPS_VIDEO1_ID:
            eHalMopId = E_HAL_DISP_MOPID_01;
            break;
        default:
            DISP_ERR("[HALMOP]%s %d, Video Layer ID %d not support\n", __FUNCTION__, __LINE__,eVdLyId);
            eHalMopId = E_HAL_DISP_MOPID_00;
            break;
    }

    return eHalMopId;
}

//ECO done, temp remove
/*HalDispQueryRet_e _HalDispMopIfGetMopG0IsFstOdr(HalDispMopId_e eMopId, u16 u16G0Hstr, u16 u16G0Vstr)//for sw patch, check Gwin 0 is first window or not
{
    int i=0;
    for(i=0;i<gbMopgInitNum[eMopId];i++)//check whether G0 is the first window or not
    {
        if((gpMopgLbOdTbl[eMopId][i].eMopgId!=E_HAL_DISP_MOPG_ID_00)&&(gpMopgLbOdTbl[eMopId][i].bIsAttred))
        {
            if(gpMopgLbOdTbl[eMopId][i].u16GwinVst < u16G0Vstr)//if any window Vstr is upper than G0, then no problem
            {
                return E_HAL_DISP_QUERY_RET_OK;
            }
            else if((gpMopgLbOdTbl[eMopId][i].u16GwinVst == u16G0Vstr)&&(gpMopgLbOdTbl[eMopId][i].u16GwinHst < u16G0Hstr))//if Vstr same, Hstr is lefter, then no problem
            {
                return E_HAL_DISP_QUERY_RET_OK;
            }
        }
    }
    //if there's no window at G0's left-upper, means need to patch
    return E_HAL_DISP_QUERY_RET_CFGERR; //return 1 means to patch
}*/

void _HalDispMopIfSetAllMopgLineBufVal(void *pCtx, HalDispMopId_e eMopId)
{
    bool bFirstAttred=0;
    int i=0;
    u16 u16TmpHstr=0;
    u8 u8TmpOrder=0;

    for(i=0;i<gbMopgInitNum[eMopId];i++)//set line_buffer_start
    {
        gpMopgLbOdTbl[eMopId][i].u8RealOrder=0;//reset order first
        if(gpMopgLbOdTbl[eMopId][i].bIsAttred)//find real order for those actually setting attr
        {
            if(bFirstAttred==0)
            {
                gpMopgLbOdTbl[eMopId][i].u8RealOrder=0;
                u16TmpHstr = gpMopgLbOdTbl[eMopId][i].u16GwinHst;
                u8TmpOrder = gpMopgLbOdTbl[eMopId][i].u8RealOrder;
                HalDispMopgSetLineBufStr(pCtx, eMopId, gpMopgLbOdTbl[eMopId][i].eMopgId,
                                         gpMopgLbOdTbl[eMopId][i].u16GwinHst,
                                         gpMopgLbOdTbl[eMopId][i].u8RealOrder);
                bFirstAttred=1;
            }
            else
            {
                if(gpMopgLbOdTbl[eMopId][i].u16GwinHst == u16TmpHstr)
                {
                    gpMopgLbOdTbl[eMopId][i].u8RealOrder = u8TmpOrder;
                }
                else
                {
                    gpMopgLbOdTbl[eMopId][i].u8RealOrder = u8TmpOrder+1;
                    u8TmpOrder = gpMopgLbOdTbl[eMopId][i].u8RealOrder;
                    u16TmpHstr = gpMopgLbOdTbl[eMopId][i].u16GwinHst;
                }

                if((gpMopgLbOdTbl[eMopId][i].eMopgId==E_HAL_DISP_MOPG_ID_00)&&(gpMopgLbOdTbl[eMopId][i].u8RealOrder!=0))//for sw patch, if gwin0 is the first, set lbstr = 0
                {
                    //ECO done, temp remove
/*                    if(_HalDispMopIfGetMopG0IsFstOdr(eMopId, gpMopgLbOdTbl[eMopId][i].u16GwinHst,gpMopgLbOdTbl[eMopId][i].u16GwinVst))
                    {
                        HalDispMopgSetLineBufStr(pCtx, eMopId, gpMopgLbOdTbl[eMopId][i].eMopgId,
                                                 gpMopgLbOdTbl[eMopId][i].u16GwinHst,
                                                 0);
                    }
                    else*/
                    {
                        HalDispMopgSetLineBufStr(pCtx, eMopId, gpMopgLbOdTbl[eMopId][i].eMopgId,
                                                 gpMopgLbOdTbl[eMopId][i].u16GwinHst,
                                                 gpMopgLbOdTbl[eMopId][i].u8RealOrder);
                    }
                }
                else
                {
                    HalDispMopgSetLineBufStr(pCtx, eMopId, gpMopgLbOdTbl[eMopId][i].eMopgId,
                                             gpMopgLbOdTbl[eMopId][i].u16GwinHst,
                                             gpMopgLbOdTbl[eMopId][i].u8RealOrder);
                }
            }
        }
        DISP_DBG(DISP_DBG_LEVEL_HAL, "[%d] ID=%d, Win_st=%d, order=%d, attred=%d\n",i, gpMopgLbOdTbl[eMopId][i].eMopgId,gpMopgLbOdTbl[eMopId][i].u16GwinHst,gpMopgLbOdTbl[eMopId][i].u8RealOrder,gpMopgLbOdTbl[eMopId][i].bIsAttred);

    }
}

void _HalDispMopIfSetMopgWinIsUsedForLbstart(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId,bool bAttred)
{
    int i=0;
    for(i=0;i<gbMopgInitNum[eMopId];i++)//set line_buffer_start
    {
        if(gpMopgLbOdTbl[eMopId][i].eMopgId == eMopgId)
        {
            gpMopgLbOdTbl[eMopId][i].bIsAttred = bAttred;
        }
    }
    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, MOPG num=%d in use, set gop=%d, en=%d\n", __FUNCTION__, __LINE__, gbMopgInitNum[eMopId],eMopgId,bAttred);
    _HalDispMopIfSetAllMopgLineBufVal(pCtx, eMopId);
}

void _HalDispMopIfSetMopgLineBufOrder(void *pCtx, HalDispMopId_e eMopId, HalDispMopgGwinId_e eMopgId,u16 Hstr, bool bEn,u16 Vstr)
{
    u8 i=0;
    bool bFoundOrder=0;
    bool bFoundId=0;
    HalDispMopgGwinId_e eTmpId=0;
    HalDispMopgGwinId_e eTmpId2=0;
    u16 u16TmpHstr=0;
    u16 u16TmpHstr2=0;
    u16 u16TmpVstr=0;
    u16 u16TmpVstr2=0;
    //u8 u8TmpOrder=0;
    bool bTmpAttred=0;
    bool bTmpAttred2=0;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, set MOPG_id=%d Hst=%d\n", __FUNCTION__, __LINE__, eMopgId,Hstr);

    for(i=0;i<gbMopgInitNum[eMopId];i++)
    {
        if(i==(gbMopgInitNum[eMopId]-1))//last one case
        {
            if(!bFoundOrder)
            {
                bFoundId=1;
                bFoundOrder = 1;
                gpMopgLbOdTbl[eMopId][i].eMopgId = eMopgId;
                gpMopgLbOdTbl[eMopId][i].u16GwinHst = Hstr;
                gpMopgLbOdTbl[eMopId][i].u16GwinVst = Vstr;
                //gpMopgLbOdTbl[i].bIsAttred = 1;
                gpMopgLbOdTbl[eMopId][i].bIsAttred = bEn;
                break;
            }
        }
        if(!bFoundId)
        {
            if(!bFoundOrder)//id not found, order not found
            {
                if(gpMopgLbOdTbl[eMopId][i].eMopgId ==eMopgId)
                {
                    bFoundId=1;
                    gpMopgLbOdTbl[eMopId][i].eMopgId = gpMopgLbOdTbl[eMopId][i+1].eMopgId;
                    gpMopgLbOdTbl[eMopId][i].u16GwinHst = gpMopgLbOdTbl[eMopId][i+1].u16GwinHst;
                    gpMopgLbOdTbl[eMopId][i].u16GwinVst = gpMopgLbOdTbl[eMopId][i+1].u16GwinVst;
                    gpMopgLbOdTbl[eMopId][i].bIsAttred = gpMopgLbOdTbl[eMopId][i+1].bIsAttred;
                    if(gpMopgLbOdTbl[eMopId][i].u16GwinHst < Hstr)
                    {
                        bFoundOrder=0;
                    }
                    else
                    {
                        bFoundOrder = 1;
                        gpMopgLbOdTbl[eMopId][i].eMopgId = eMopgId;
                        gpMopgLbOdTbl[eMopId][i].u16GwinHst = Hstr;
                        gpMopgLbOdTbl[eMopId][i].u16GwinVst = Vstr;
                        //gpMopgLbOdTbl[i].bIsAttred = 1;
                        gpMopgLbOdTbl[eMopId][i].bIsAttred = bEn;
                    }
                }
                else
                {
                    if(gpMopgLbOdTbl[eMopId][i].u16GwinHst < Hstr)
                    {
                        bFoundOrder=0;
                    }
                    else
                    {
                        bFoundOrder = 1;
                        eTmpId = gpMopgLbOdTbl[eMopId][i].eMopgId;
                        u16TmpHstr = gpMopgLbOdTbl[eMopId][i].u16GwinHst;
                        u16TmpVstr = gpMopgLbOdTbl[eMopId][i].u16GwinVst;
                        bTmpAttred = gpMopgLbOdTbl[eMopId][i].bIsAttred;
                        gpMopgLbOdTbl[eMopId][i].eMopgId = eMopgId;
                        gpMopgLbOdTbl[eMopId][i].u16GwinHst = Hstr;
                        gpMopgLbOdTbl[eMopId][i].u16GwinVst = Vstr;
                        //gpMopgLbOdTbl[i].bIsAttred = 1;
                        gpMopgLbOdTbl[eMopId][i].bIsAttred = bEn;
                    }
                }
            }
            else//id not found, order found
            {
                if(gpMopgLbOdTbl[eMopId][i].eMopgId ==eMopgId)
                {
                    bFoundId=1;
                    gpMopgLbOdTbl[eMopId][i].eMopgId = eTmpId;
                    gpMopgLbOdTbl[eMopId][i].u16GwinHst = u16TmpHstr;
                    gpMopgLbOdTbl[eMopId][i].u16GwinVst = u16TmpVstr;
                    gpMopgLbOdTbl[eMopId][i].bIsAttred = bTmpAttred;
                }
                else
                {
                    bFoundId=0;
                    eTmpId2 = gpMopgLbOdTbl[eMopId][i].eMopgId;
                    u16TmpHstr2 = gpMopgLbOdTbl[eMopId][i].u16GwinHst;
                    u16TmpVstr2 = gpMopgLbOdTbl[eMopId][i].u16GwinVst;
                    bTmpAttred2 = gpMopgLbOdTbl[eMopId][i].bIsAttred;
                    gpMopgLbOdTbl[eMopId][i].eMopgId = eTmpId;
                    gpMopgLbOdTbl[eMopId][i].u16GwinHst = u16TmpHstr;
                    gpMopgLbOdTbl[eMopId][i].u16GwinVst = u16TmpVstr;
                    gpMopgLbOdTbl[eMopId][i].bIsAttred = bTmpAttred;
                    eTmpId = eTmpId2;
                    u16TmpHstr = u16TmpHstr2;
                    u16TmpVstr = u16TmpVstr2;
                    bTmpAttred = bTmpAttred2;
                }
            }
        }
        else
        {
            if(!bFoundOrder)//id found, order not found
            {
                gpMopgLbOdTbl[eMopId][i].eMopgId = gpMopgLbOdTbl[eMopId][i+1].eMopgId;
                gpMopgLbOdTbl[eMopId][i].u16GwinHst = gpMopgLbOdTbl[eMopId][i+1].u16GwinHst;
                gpMopgLbOdTbl[eMopId][i].u16GwinVst = gpMopgLbOdTbl[eMopId][i+1].u16GwinVst;
                gpMopgLbOdTbl[eMopId][i].bIsAttred = gpMopgLbOdTbl[eMopId][i+1].bIsAttred;
                if(gpMopgLbOdTbl[eMopId][i].u16GwinHst < Hstr)
                {
                    bFoundOrder=0;
                }
                else
                {
                    bFoundOrder = 1;
                    gpMopgLbOdTbl[eMopId][i].eMopgId = eMopgId;
                    gpMopgLbOdTbl[eMopId][i].u16GwinHst = Hstr;
                    gpMopgLbOdTbl[eMopId][i].u16GwinVst = Vstr;
                    //gpMopgLbOdTbl[i].bIsAttred = 1;
                    gpMopgLbOdTbl[eMopId][i].bIsAttred = bEn;
                }
            }
            else//id found, order found
            {
                break;
            }
        }
    }

    u16TmpHstr=0;
    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, MOPG num=%d in use\n", __FUNCTION__, __LINE__, gbMopgInitNum[eMopId]);

    _HalDispMopIfSetAllMopgLineBufVal(pCtx, eMopId);


}

void _HalDispMopIfSetMopRotImiAddrManage(HalDispMopId_e eMopId, bool bMopRotId, u32 u32Ysize, u32 u32Csize)//Id=0: Rot0; Id=1: Rot1
{
    u32 u32TmpAddr=0;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, rot_id=%d, Ysize=0x%lx, Csize=0x%lx\n",
            __FUNCTION__, __LINE__,
            bMopRotId,u32Ysize,u32Csize);
    //imi property: MOP0_Rot0_Y -> MOP0_Rot0_C -> MOP0_Rot1_Y -> MOP0_Rot1_C -> MOP1_Rot0_Y -> MOP1_Rot0_C -> MOP1_Rot1_Y -> MOP1_Rot1_C
    if(bMopRotId==0)//Rot0
    {
        u32TmpAddr = u32Rot1ImiYaddr[eMopId];
        u32Rot0ImiCaddr[eMopId] = u32Rot0ImiYaddr[eMopId] + u32Ysize;
        u32Rot1ImiYaddr[eMopId] = u32Rot0ImiCaddr[eMopId] + u32Csize;
        u32Rot1ImiCaddr[eMopId] = u32Rot1ImiCaddr[eMopId] + u32Rot1ImiYaddr[eMopId] - u32TmpAddr;
        if(eMopId == E_HAL_DISP_MOPID_00)
        {
            u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] = u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] + u32Rot1ImiYaddr[eMopId] - u32TmpAddr;
            u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01] = u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01] + u32Rot1ImiYaddr[eMopId] - u32TmpAddr;
            u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01] = u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01] + u32Rot1ImiYaddr[eMopId] - u32TmpAddr;
            u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01] = u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01] + u32Rot1ImiYaddr[eMopId] - u32TmpAddr;
        }
    }
    else//Rot1
    {
        u32Rot1ImiCaddr[eMopId] = u32Rot1ImiYaddr[eMopId] + u32Ysize;
        if(eMopId == E_HAL_DISP_MOPID_00)
        {
            u32TmpAddr = u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01];
            u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] = u32Rot1ImiCaddr[eMopId] + u32Csize;
            u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01] = u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01] + u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] - u32TmpAddr;
            u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01] = u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01] + u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] - u32TmpAddr;
            u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01] = u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01] + u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01] - u32TmpAddr;
        }
    }
}

void _HalDispMopIfSetMopRotImiIniAddrManage(u32 u32ImiStartAddr)
{
    HalDispMopId_e eMopId = 0;
    u32 u32TmpIniAddr = 0;

    u32TmpIniAddr = u32RotImiIniaddr;
    u32RotImiIniaddr = u32ImiStartAddr;

    if(!bImiInit)
    {
        for(eMopId = E_HAL_DISP_MOPID_00; eMopId < E_HAL_DISP_MOP_NUM; eMopId++)
        {
            //new way
            u32Rot0ImiYaddr[eMopId]=u32RotImiIniaddr;
            u32Rot0ImiCaddr[eMopId]=u32RotImiIniaddr;
            u32Rot1ImiYaddr[eMopId]=u32RotImiIniaddr;
            u32Rot1ImiCaddr[eMopId]=u32RotImiIniaddr;
        }
        bImiInit=1;
    }

    if(!bImiSetAddrInit)
    {
        bImiSetAddrInit=1;
    }

    for(eMopId = E_HAL_DISP_MOPID_00; eMopId < E_HAL_DISP_MOP_NUM; eMopId++)
    {
        //new way
        u32Rot0ImiYaddr[eMopId]=u32Rot0ImiYaddr[eMopId] + u32RotImiIniaddr - u32TmpIniAddr;
        u32Rot0ImiCaddr[eMopId]=u32Rot0ImiCaddr[eMopId] + u32RotImiIniaddr - u32TmpIniAddr;
        u32Rot1ImiYaddr[eMopId]=u32Rot1ImiYaddr[eMopId] + u32RotImiIniaddr - u32TmpIniAddr;
        u32Rot1ImiCaddr[eMopId]=u32Rot1ImiCaddr[eMopId] + u32RotImiIniaddr - u32TmpIniAddr;
    }

}


//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
HalDispQueryRet_e HalDispMopIfGetInfoInputPortInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstInputPortContain->u16FlipFrontPorchCnt = 0;

    if(pstInputPortContain->u32PortId > E_HAL_DISP_MOPG_ID_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

    return enRet;
}

void HalDispMopIfSetInputPortInit(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    static bool sbMopgHwInit = 0;
    static bool sbMopsHwInit = 0;
    static bool sbMopRotHwInit = 0;
//    static bool sbMopsSwShadowInit = 0;
    HalDispMopgGwinId_e stTemp = 0;
    u8 i = 0;
    u8 j = 0;
    u8 k = 0;
    bool bPortInit = 0;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    //init MOP rotate HW
    if(!sbMopRotHwInit)
    {
        HalDispMopRotInit(pCtx);
        for(k=0; k<E_HAL_DISP_MOP_NUM; k++)
        {
            _gstMopgRotRec[k].bRotEn = 0;
            _gstMopgRotRec[k].eMopgId = E_HAL_DISP_MOPG_ID_NUM;
            _gstMopgRotRec[k].stRotAng = E_HAL_DISP_ROTATE_NONE;
        }
        sbMopRotHwInit=1;
/*        if(!sbMopsSwShadowInit)
        {
            HalMopsShadowInit();
            sbMopsSwShadowInit=1;
        }*/
    }

    if(pstVidLayerContain)
    {
        if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
           (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//init mopg
        {
            if(sbMopgHwInit==0)
            {
                HalDispMopgInit(pCtx);
                sbMopgHwInit=1;
            }
            //reset gwin register when create instance
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, init Port_id:%ld\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
            HalDispMopgGwinEn(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, FALSE);
            HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, 1, 1);
            HalDispMopgSetGwinParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, 0, 0, 1, 1);
            for(i=0;i<gbMopgInitNum[eMopId];i++)//find this id is init or not
            {
                if(gpMopgLbOdTbl[eMopId][i].eMopgId == pstInputPortContain->u32PortId)
                {
                    bPortInit=1;
                    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Port_id:%ld has been initialized, reinit\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
                    break;
                }
            }
            if(bPortInit)//reinit for lb count list
            {
                stTemp = gpMopgLbOdTbl[eMopId][i].eMopgId;
                for(j=i;j>0;j--)//shift all member to next
                {
                    gpMopgLbOdTbl[eMopId][j].eMopgId = gpMopgLbOdTbl[eMopId][j-1].eMopgId;
                    gpMopgLbOdTbl[eMopId][j].u16GwinHst = gpMopgLbOdTbl[eMopId][j-1].u16GwinHst;
                    gpMopgLbOdTbl[eMopId][j].u8RealOrder = gpMopgLbOdTbl[eMopId][j-1].u8RealOrder;
                    gpMopgLbOdTbl[eMopId][j].u16GwinVst = gpMopgLbOdTbl[eMopId][j-1].u16GwinVst;
                    gpMopgLbOdTbl[eMopId][j].bIsAttred = gpMopgLbOdTbl[eMopId][j-1].bIsAttred;
                }
                gpMopgLbOdTbl[eMopId][0].eMopgId = stTemp;//add new member at [0]
                gpMopgLbOdTbl[eMopId][0].u16GwinHst = 0;
                gpMopgLbOdTbl[eMopId][0].u8RealOrder = 0;
                gpMopgLbOdTbl[eMopId][0].u16GwinVst = 0;
                gpMopgLbOdTbl[eMopId][0].bIsAttred = 0;
            }
            if(!bPortInit && (gbMopgInitNum[eMopId]<E_HAL_DISP_MOPG_ID_NUM))//add new member into lb count list
            {
                for(i=gbMopgInitNum[eMopId];i>0;i--)//shift all member to next
                {
                    gpMopgLbOdTbl[eMopId][i].eMopgId = gpMopgLbOdTbl[eMopId][i-1].eMopgId;
                    gpMopgLbOdTbl[eMopId][i].u16GwinHst = gpMopgLbOdTbl[eMopId][i-1].u16GwinHst;
                    gpMopgLbOdTbl[eMopId][i].u8RealOrder = gpMopgLbOdTbl[eMopId][i-1].u8RealOrder;
                    gpMopgLbOdTbl[eMopId][i].u16GwinVst = gpMopgLbOdTbl[eMopId][i-1].u16GwinVst;
                    gpMopgLbOdTbl[eMopId][i].bIsAttred = gpMopgLbOdTbl[eMopId][i-1].bIsAttred;
                }
                gpMopgLbOdTbl[eMopId][0].eMopgId = (HalDispMopgGwinId_e)pstInputPortContain->u32PortId;//add new member at [0]
                gpMopgLbOdTbl[eMopId][0].u16GwinHst = 0;
                gpMopgLbOdTbl[eMopId][0].u8RealOrder = 0;
                gpMopgLbOdTbl[eMopId][0].u16GwinVst = 0;
                gpMopgLbOdTbl[eMopId][0].bIsAttred = 0;
                gbMopgInitNum[eMopId]++;
            }
        }
        else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
                (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//init mops
        {
            if(sbMopsHwInit==0)
            {
                HalDispMopsInit(pCtx);
                sbMopsHwInit=1;
/*                if(!sbMopsSwShadowInit)
                {
                    HalMopsShadowInit();
                    sbMopsSwShadowInit=1;
                }*/
                //mops will work when mopg is init...
                if(!sbMopgHwInit)
                {
                    HalDispMopgInit(pCtx);
                    sbMopgHwInit=1;
                }
            }
            //reset gwin register when create instance
            HalDispMopsGwinEn(pCtx, eMopId, FALSE);
            HalDispMopsSetSourceParam(pCtx, eMopId, 0, 0);
            HalDispMopsSetGwinParam(pCtx, eMopId, 0, 0, 1, 1);
        }
        else
        {
            DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld VidoeLayer Not Bind\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEnable = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pbEnable = (bool *)pCfg;

    if(pstInputPortContain->bEnable == 0 && *pbEnable == 1)
    {
        pstInputPortContain->u16FlipFrontPorchCnt = 0;
    }

    pstInputPortContain->bEnable = *pbEnable;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, video_id=%ld, Port_id:%ld settings\n", __FUNCTION__, __LINE__,pstVidLayerContain->u32VidLayerId, pstInputPortContain->u32PortId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d\n", __FUNCTION__, __LINE__, *pbEnable);

    if(pstInputPortContain->u32PortId > E_HAL_DISP_MOPG_ID_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

    return enRet;
}

void HalDispMopIfSetInputPortEnable(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEnable = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pbEnable = (bool *)pCfg;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
       (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        //go to lb start algorithm
        _HalDispMopIfSetMopgWinIsUsedForLbstart(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, *pbEnable);

        HalDispMopgGwinEn(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, *pbEnable);

    }
    else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        HalDispMopsGwinEn(pCtx, eMopId, *pbEnable);
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;

    memcpy(&pstInputPortContain->stAttr, pstHalInputPortCfg, sizeof(HalDispInputPortAttr_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, video_id=%ld, Port_id:%ld settings\n", __FUNCTION__, __LINE__,pstVidLayerContain->u32VidLayerId, pstInputPortContain->u32PortId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Disp(%d %d %d %d) Src(%d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->stDispWin.u16X, pstHalInputPortCfg->stDispWin.u16Y,
        pstHalInputPortCfg->stDispWin.u16Width, pstHalInputPortCfg->stDispWin.u16Height,
        pstHalInputPortCfg->u16SrcWidth, pstHalInputPortCfg->u16SrcHeight);

    if(pstInputPortContain->u32PortId > E_HAL_DISP_MOPG_ID_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

    return enRet;
}

void HalDispMopIfSetInputPortAttr(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u16 u16Hratio = 0x1000;
    u16 u16Vratio = 0x1000;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
       (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, pstHalInputPortCfg->u16SrcWidth,
                                                                              pstHalInputPortCfg->u16SrcHeight);
        HalDispMopgSetGwinParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, pstHalInputPortCfg->stDispWin.u16X,
                                                                            pstHalInputPortCfg->stDispWin.u16Y,
                                                                            pstHalInputPortCfg->stDispWin.u16Width,
                                                                            pstHalInputPortCfg->stDispWin.u16Height);
        _HalDispMopIfSetMopgLineBufOrder(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId,pstHalInputPortCfg->stDispWin.u16X
                                                                                         ,pstInputPortContain->bEnable
                                                                                         ,pstHalInputPortCfg->stDispWin.u16Y);
        if(pstHalInputPortCfg->stDispWin.u16Width > pstHalInputPortCfg->u16SrcWidth)
        {
            u16Hratio = ((pstHalInputPortCfg->u16SrcWidth*4096)/pstHalInputPortCfg->stDispWin.u16Width);
            HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
        }
        else
        {
            u16Hratio = 0x1000;
            HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
        }
        if(pstHalInputPortCfg->stDispWin.u16Height > pstHalInputPortCfg->u16SrcHeight)
        {
            u16Vratio = ((pstHalInputPortCfg->u16SrcHeight*4096)/pstHalInputPortCfg->stDispWin.u16Height);
            HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
        }
        else
        {
            u16Vratio = 0x1000;
            HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
        }
    }
    else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        HalDispMopsSetSourceParam(pCtx, eMopId, pstHalInputPortCfg->u16SrcWidth,
                                  pstHalInputPortCfg->u16SrcHeight);
        HalDispMopsSetGwinParam(pCtx, eMopId, pstHalInputPortCfg->stDispWin.u16X,
                                pstHalInputPortCfg->stDispWin.u16Y,
                                pstHalInputPortCfg->stDispWin.u16Width,
                                pstHalInputPortCfg->stDispWin.u16Height);
        if(pstHalInputPortCfg->stDispWin.u16Width > pstHalInputPortCfg->u16SrcWidth)
        {
            u16Hratio = ((pstHalInputPortCfg->u16SrcWidth*4096)/pstHalInputPortCfg->stDispWin.u16Width);
            HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
        }
        else
        {
            u16Hratio = 0x1000;
            HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
        }
        if(pstHalInputPortCfg->stDispWin.u16Height > pstHalInputPortCfg->u16SrcHeight)
        {
            u16Vratio = ((pstHalInputPortCfg->u16SrcHeight*4096)/pstHalInputPortCfg->stDispWin.u16Height);
            HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
        }
        else
        {
            u16Vratio = 0x1000;
            HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortFlip(void *pCtx, void *pCfg)
{
    u16 i;
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVideoFrameData_t *pstFramedata = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    //DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
//    u32 u32CurTime, u32TimeOutBound;
//    bool bTimeOut = FALSE;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    //pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[0]; // only device_0 is valid
    pstFramedata = (HalDispVideoFrameData_t *)pCfg;

/*    if(_gstMopgRotRec.bRotEn)
    {
        u32CurTime = DrvDispOsGetSystemTime();
        u32TimeOutBound = 1000 / pstDeviceContain->stDevTimingCfg.u16Fps;
        bTimeOut = 0;
        while(1)
        {
            bTimeOut = DrvDispOsGetSystemTime() - u32CurTime > u32TimeOutBound ? 1 : 0;
            if( bTimeOut || __HalDispIfGetTimeZoneType(pstDeviceContain) == E_HAL_DISP_TIMEZONE_FRONTPORCH)
            {
                break;
            }
        }

        if(bTimeOut)
        {
            enRet = E_HAL_DISP_QUERY_RET_IMPLICIT_ERR;
            pstInputPortContain->u16FlipFrontPorchCnt++;
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Rotate Flip tiemout\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        if(__HalDispIfGetTimeZoneType(pstDeviceContain) == E_HAL_DISP_TIMEZONE_FRONTPORCH)
        {
            enRet = E_HAL_DISP_QUERY_RET_IMPLICIT_ERR;
            pstInputPortContain->u16FlipFrontPorchCnt++;
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Flip at FrontPorch\n", __FUNCTION__, __LINE__);
        }
    }*/

    if(enRet == E_HAL_DISP_QUERY_RET_OK)
    {
        memcpy(&pstInputPortContain->stFrameData, pstFramedata, sizeof(HalDispVideoFrameData_t));

        for(i=0; i<3; i++)
         {
             if(pstInputPortContain->stFrameData.au64PhyAddr[i]&0xF)//physical address should be 16 align
             {
                 enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                 DISP_ERR("%s %d, PHY_ADDR should be 16 align, addr[%d](%08llx)\n", __FUNCTION__, __LINE__,i, pstInputPortContain->stFrameData.au64PhyAddr[i]);
             }

             if(pstInputPortContain->stFrameData.au32Stride[i]&0xF)//stride should be 16 align
             {
                 enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                 DISP_ERR("%s %d, Stride should be 16 align, addr[%d](%ld)\n", __FUNCTION__, __LINE__,i, pstInputPortContain->stFrameData.au32Stride[i]);
             }
         }

        if(pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV ||
           pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422)
        {
            for(i=0; i<3; i++)
            {
                pstInputPortContain->stFrameData.au32Stride[i] /= 2;

               if(i==1)
               {
                   pstInputPortContain->stFrameData.au64PhyAddr[1] =
                    pstInputPortContain->stFrameData.au64PhyAddr[0] + 0x10;
               }
            }
        }

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Fmt:%s, PhyAddr(%08llx, %08llx, %08llx), Stride(%ld %ld %ld)\n",
            __FUNCTION__, __LINE__,
            PARSING_HAL_PIXEL_FMT(pstInputPortContain->stFrameData.ePixelFormat),
            pstInputPortContain->stFrameData.au64PhyAddr[0], pstInputPortContain->stFrameData.au64PhyAddr[1], pstInputPortContain->stFrameData.au64PhyAddr[2],
            pstInputPortContain->stFrameData.au32Stride[0], pstInputPortContain->stFrameData.au32Stride[1], pstInputPortContain->stFrameData.au32Stride[2]);

        if(pstInputPortContain->u32PortId > E_HAL_DISP_MOPG_ID_NUM)
        {
            enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
            DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
        }
    }

    return enRet;
}

void HalDispMopIfSetInputPortFlip(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u64 u64Yaddr=0;
    u64 u64Caddr=0;
    u8 u8AddrOffset=0;
    u16 u16tempRotInW = 0;
    u16 u16RotDummyPix = 0;
    u16 u16tempRotInH = 0;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    //Y Address, after counting crop, Yaddr = Yaddr + offset_y*stride + offset_x
    u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0] +
               pstInputPortContain->stCrop.u16Y * pstInputPortContain->stFrameData.au32Stride[0]+
               pstInputPortContain->stCrop.u16X;
    //C Address, after counting crop, Caddr = Caddr + offset_y/2*stride + offset_x
    u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1] +
               (pstInputPortContain->stCrop.u16Y/2) * pstInputPortContain->stFrameData.au32Stride[0]+
               pstInputPortContain->stCrop.u16X;

    u8AddrOffset = (u8)(pstInputPortContain->stCrop.u16X & 0xF);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
       (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)
        {
            //rotate + crop case, address not need to shift
            u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0];
            u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1];
            //set rotate pitch
            HalDispMopRot0BlkNum(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);

            //refined
            u16tempRotInW = pstInputPortContain->stCrop.u16X + pstInputPortContain->stAttr.u16SrcWidth;
            u16RotDummyPix = (u16tempRotInW & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInW = ((u16tempRotInW>>4) + 1)<<4;
            }
            //rot_r source_width=source_width align 16
            HalDispMopRot0SourceWidth(pCtx, eMopId, u16tempRotInW);

            //calculate width in imi (mop pitch)(~source height)
            //u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;//old way
            //new way
            u16tempRotInH = Count_end_align(pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight) -
                            Count_str_align(pstInputPortContain->stCrop.u16Y);
            //pitch is the same as source height, 256 align
            /*if((u16tempRotInH & 0xF))
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }*/
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            _HalDispMopIfSetMopRotImiAddrManage(eMopId, 0, (u16tempRotInH*16), (u16tempRotInH*8));

            //imi init address check
            if(!bImiSetAddrInit)
            {
                DISP_ERR("%s %d, Imi address is not set!\n", __FUNCTION__, __LINE__);
            }

            //set rot_r read address
            HalDispMopRot0SetReadYAddr(pCtx, eMopId, u64Yaddr);
            HalDispMopRot0SetReadCAddr(pCtx, eMopId, u64Caddr);
            //set rot_r write imi address
            HalDispMopRot0SetWriteYAddr(pCtx, eMopId, u32Rot0ImiYaddr[eMopId]);
            HalDispMopRot0SetWriteCAddr(pCtx, eMopId, u32Rot0ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopgSetYAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiYaddr[eMopId]);
            HalDispMopgSetCAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiCaddr[eMopId]);
            //pitch is set in rotate/crop config
            //crop offset 16 change to dummy pixel in imi
            HalDispMopRot0SetPixDummy(pCtx, eMopId, u8AddrOffset);

            //reset Rot1 imi address because the change in management function, add mop1 rot0/rot1
            //set rot_r write imi address
            HalDispMopRot1SetWriteYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopRot1SetWriteCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopsSetYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopsSetCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            if(eMopId == E_HAL_DISP_MOPID_00)
            {
                //set rot_r write imi address
                HalDispMopRot0SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot0SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopgSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopgSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set rot_r write imi address
                HalDispMopRot1SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot1SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopsSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopsSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
            }

        }
        else if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270)
        {
            //rotate + crop case, address not need to shift
            u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0];
            u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1];
            //set rotate pitch
            HalDispMopRot0BlkNum(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);

            //refined
            u16tempRotInW = pstInputPortContain->stCrop.u16X + pstInputPortContain->stAttr.u16SrcWidth;
            u16RotDummyPix = (u16tempRotInW & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInW = ((u16tempRotInW>>4) + 1)<<4;
            }
            //rot_r source_width=source_width align 16
            HalDispMopRot0SourceWidth(pCtx, eMopId, u16tempRotInW);

            //calculate width in imi (mop pitch)(~source height)
            //u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;//old way
            //new way
            u16tempRotInH = Count_end_align(pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight) -
                            Count_str_align(pstInputPortContain->stCrop.u16Y);
            //pitch is the same as source height, 256 align
            /*if((u16tempRotInH & 0xF))
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }*/
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            _HalDispMopIfSetMopRotImiAddrManage(eMopId, 0, (u16tempRotInH*16), (u16tempRotInH*8));

            //imi init address check
            if(!bImiSetAddrInit)
            {
                DISP_ERR("%s %d, Imi address is not set!\n", __FUNCTION__, __LINE__);
            }

            //set rot_r read address
            HalDispMopRot0SetReadYAddr(pCtx, eMopId, u64Yaddr);
            HalDispMopRot0SetReadCAddr(pCtx, eMopId, u64Caddr);
            //set rot_r write imi address
            HalDispMopRot0SetWriteYAddr(pCtx, eMopId, (u32)u32Rot0ImiYaddr[eMopId]);
            HalDispMopRot0SetWriteCAddr(pCtx, eMopId, (u32)u32Rot0ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopgSetYAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, (u32)u32Rot0ImiYaddr[eMopId]);
            HalDispMopgSetCAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, (u32)u32Rot0ImiCaddr[eMopId]);
            //pitch is set in rotate/crop config
            //crop offset 16 change to cooperated with dummy pixel in imi
            //refined
            u8AddrOffset = u16tempRotInW-u8AddrOffset-pstInputPortContain->stAttr.u16SrcWidth - Count_str_align(pstInputPortContain->stCrop.u16X);
            HalDispMopRot0SetPixDummy(pCtx, eMopId, u8AddrOffset);

            //reset Rot1 imi address because the change in management function, add mop1 rot0/rot1
            //set rot_r write imi address
            HalDispMopRot1SetWriteYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopRot1SetWriteCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopsSetYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopsSetCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            if(eMopId == E_HAL_DISP_MOPID_00)
            {
                //set rot_r write imi address
                HalDispMopRot0SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot0SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopgSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopgSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set rot_r write imi address
                HalDispMopRot1SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot1SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopsSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopsSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
            }
        }
        else
        {
            HalDispMopgSetYAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u64Yaddr);
            HalDispMopgSetCAddr(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u64Caddr);
            HalDispMopgSetPitch(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, pstInputPortContain->stFrameData.au32Stride[0]);
            HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u8AddrOffset);
            if(u8AddrOffset)
            {
                HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, (pstInputPortContain->stAttr.u16SrcWidth+u8AddrOffset),
                                                                                  pstInputPortContain->stAttr.u16SrcHeight);
            }
        }
    }
    else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)
        {
            //rotate + crop case, address not need to shift
            u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0];
            u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1];
            //set rotate pitch
            HalDispMopRot1BlkNum(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);

            u16tempRotInW = pstInputPortContain->stCrop.u16X + pstInputPortContain->stAttr.u16SrcWidth;
            u16RotDummyPix = (u16tempRotInW & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInW = ((u16tempRotInW>>4) + 1)<<4;
            }
            //rot_r source_width=source_width align 16
            HalDispMopRot1SourceWidth(pCtx, eMopId, u16tempRotInW);

            //calculate width in imi (mop pitch)(~source height)
            //u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            //new way
            u16tempRotInH = Count_end_align(pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight) -
                            Count_str_align(pstInputPortContain->stCrop.u16Y);
            //pitch is the same as source height, but 256 align
            /*if((u16tempRotInH & 0xF))
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }*/
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            _HalDispMopIfSetMopRotImiAddrManage(eMopId, 1, (u16tempRotInH*16), (u16tempRotInH*8));

            //imi init address check
            if(!bImiSetAddrInit)
            {
                DISP_ERR("%s %d, Imi address is not set!\n", __FUNCTION__, __LINE__);
            }
            //set rot_r read address
            HalDispMopRot1SetReadYAddr(pCtx, eMopId, u64Yaddr);
            HalDispMopRot1SetReadCAddr(pCtx, eMopId, u64Caddr);
            //set rot_r write imi address
            HalDispMopRot1SetWriteYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopRot1SetWriteCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopsSetYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopsSetCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //pitch is set in rotate/crop config
            //crop offset 16 change to dummy pixel in imi
            HalDispMopRot1SetPixDummy(pCtx, eMopId, u8AddrOffset);

            //reset Rot1 imi address because the change in management function, add mop1 rot0/rot1
            if(eMopId == E_HAL_DISP_MOPID_00)
            {
                //set rot_r write imi address
                HalDispMopRot0SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot0SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopgSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopgSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set rot_r write imi address
                HalDispMopRot1SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot1SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopsSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopsSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
            }
        }
        else if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270)
        {
            //rotate + crop case, address not need to shift
            u64Yaddr = pstInputPortContain->stFrameData.au64PhyAddr[0];
            u64Caddr = pstInputPortContain->stFrameData.au64PhyAddr[1];
            //set rotate pitch
            HalDispMopRot1BlkNum(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);

            u16tempRotInW = pstInputPortContain->stCrop.u16X + pstInputPortContain->stAttr.u16SrcWidth;
            u16RotDummyPix = (u16tempRotInW & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInW = ((u16tempRotInW>>4) + 1)<<4;
            }
            //rot_r source_width=source_width align 16
            HalDispMopRot1SourceWidth(pCtx, eMopId, u16tempRotInW);
            //calculate width in imi (mop pitch)(~source height)
            //u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            //new way
            u16tempRotInH = Count_end_align(pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight) -
                            Count_str_align(pstInputPortContain->stCrop.u16Y);
            //pitch is the same as source height, but 256 align
            /*if((u16tempRotInH & 0xF))
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }*/
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            _HalDispMopIfSetMopRotImiAddrManage(eMopId, 1, (u16tempRotInH*16), (u16tempRotInH*8));
            //imi init address check
            if(!bImiSetAddrInit)
            {
                DISP_ERR("%s %d, Imi address is not set!\n", __FUNCTION__, __LINE__);
            }
            //set rot_r read address
            HalDispMopRot1SetReadYAddr(pCtx, eMopId, u64Yaddr);
            HalDispMopRot1SetReadCAddr(pCtx, eMopId, u64Caddr);
            //set rot_r write imi address
            HalDispMopRot1SetWriteYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopRot1SetWriteCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //set MOPG read imi address
            HalDispMopsSetYAddr(pCtx, eMopId, (u32)u32Rot1ImiYaddr[eMopId]);
            HalDispMopsSetCAddr(pCtx, eMopId, (u32)u32Rot1ImiCaddr[eMopId]);
            //pitch is set in rotate/crop config
            //crop offset 16 change to cooperated with dummy pixel in imi
            //refined
            u8AddrOffset = u16tempRotInW-u8AddrOffset-pstInputPortContain->stAttr.u16SrcWidth - Count_str_align(pstInputPortContain->stCrop.u16X);
            HalDispMopRot1SetPixDummy(pCtx, eMopId, u8AddrOffset);

            //reset Rot1 imi address because the change in management function, add mop1 rot0/rot1
            if(eMopId == E_HAL_DISP_MOPID_00)
            {
                //set rot_r write imi address
                HalDispMopRot0SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot0SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopgSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopgSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u32Rot0ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set rot_r write imi address
                HalDispMopRot1SetWriteYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopRot1SetWriteCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
                //set MOPG read imi address
                HalDispMopsSetYAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiYaddr[E_HAL_DISP_MOPID_01]);
                HalDispMopsSetCAddr(pCtx, E_HAL_DISP_MOPID_01, (u32)u32Rot1ImiCaddr[E_HAL_DISP_MOPID_01]);
            }
        }
        else
        {
            HalDispMopsSetYAddr(pCtx, eMopId, u64Yaddr);
            HalDispMopsSetCAddr(pCtx, eMopId, u64Caddr);
            HalDispMopsSetPitch(pCtx, eMopId, pstInputPortContain->stFrameData.au32Stride[0]);
            HalDispMopsSetAddr16Offset(pCtx, eMopId, u8AddrOffset);
            if(u8AddrOffset)
            {
                HalDispMopsSetSourceParam(pCtx, eMopId, (pstInputPortContain->stAttr.u16SrcWidth+u8AddrOffset),pstInputPortContain->stAttr.u16SrcHeight);
            }
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortRotate_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
//    s32 imi_ret;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortRotate_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, video_id=%ld, Port_id:%ld settings\n", __FUNCTION__, __LINE__,pstVidLayerContain->u32VidLayerId, pstInputPortContain->u32PortId);

    if(!bImiInit)
    {
/*        imi_ret = DrvDispOsImiHeapAlloc(0xC000, &u32MopRotImiTotalAddr);
        if(imi_ret)
        {
            DISP_ERR("%s %d, imi_heap init Rot 0x4000 fail, err=%ld\n", __FUNCTION__, __LINE__, imi_ret);
        }*/
        for(eMopId = E_HAL_DISP_MOPID_00; eMopId < E_HAL_DISP_MOP_NUM; eMopId++)
        {
//            u32Rot0ImiYaddr[eMopId]=(u32)u32MopRotImiTotalAddr;
//            u32Rot0ImiCaddr[eMopId]=(u32)u32MopRotImiTotalAddr;
//            u32Rot1ImiYaddr[eMopId]=(u32)u32MopRotImiTotalAddr;
//            u32Rot1ImiCaddr[eMopId]=(u32)u32MopRotImiTotalAddr;
            //new way
            u32Rot0ImiYaddr[eMopId]=u32RotImiIniaddr;
            u32Rot0ImiCaddr[eMopId]=u32RotImiIniaddr;
            u32Rot1ImiYaddr[eMopId]=u32RotImiIniaddr;
            u32Rot1ImiCaddr[eMopId]=u32RotImiIniaddr;
        }
        bImiInit=1;
    }

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
       (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        if((_gstMopgRotRec[eMopId].bRotEn)&&(pstInputPortContain->u32PortId!=_gstMopgRotRec[eMopId].eMopgId))
        {
            DISP_ERR("%s error, MOPG rotate only support one window\n", __FUNCTION__);
            DISP_ERR("%s %d, MOPG rotate is used by mopg_id=%d\n", __FUNCTION__, __LINE__, _gstMopgRotRec[eMopId].eMopgId);
            return E_HAL_DISP_QUERY_RET_CFGERR;
        }
        else
        {
            if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_NONE)
            {
                _gstMopgRotRec[eMopId].bRotEn = 0;
            }
            else if((pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_90)||
                    (pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_270))
            {
                _gstMopgRotRec[eMopId].bRotEn = 1;
            }
            else
            {
                _gstMopgRotRec[eMopId].bRotEn = 0;
                DISP_ERR("%s %d, MOPG %s is not supported\n", __FUNCTION__, __LINE__, PARSING_MOP_ROT_ANGLE(pstHalInputPortCfg->enRotate));
                return E_HAL_DISP_QUERY_RET_NOTSUPPORT;
            }
            _gstMopgRotRec[eMopId].eMopgId = (HalDispMopgGwinId_e)pstInputPortContain->u32PortId;
            _gstMopgRotRec[eMopId].stRotAng = pstHalInputPortCfg->enRotate;
        }
    }
    else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        if((pstHalInputPortCfg->enRotate!=E_HAL_DISP_ROTATE_NONE)&&
           (pstHalInputPortCfg->enRotate!=E_HAL_DISP_ROTATE_90)&&
           (pstHalInputPortCfg->enRotate!=E_HAL_DISP_ROTATE_270))
        {
            DISP_ERR("%s %d, MOPS %s is not supported\n", __FUNCTION__, __LINE__, PARSING_MOP_ROT_ANGLE(pstHalInputPortCfg->enRotate));
            return E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

    memcpy(&pstInputPortContain->stRot, pstHalInputPortCfg, sizeof(HalDispInputPortRotate_t));

    return enRet;
}

void HalDispMopIfSetInputPortRotate(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortRotate_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u16 u16Hratio = 0x1000;
    u16 u16Vratio = 0x1000;
    u16 u16tempRotInH = 0;
    u16 u16RotDummyPix = 0;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispInputPortRotate_t *)pCfg;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, debug, MOPID=%d, rot_ang=%s, source_height=%d\n", __FUNCTION__, __LINE__,eMopId, PARSING_MOP_ROT_ANGLE(pstHalInputPortCfg->enRotate),pstInputPortContain->stAttr.u16SrcHeight);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
   (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_NONE)
        {
            HalDispMopRot0En(pCtx, eMopId, FALSE);
            HalDispMopRot0SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
        }
        else if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_90)
        {
            HalDispMopRot0En(pCtx, eMopId, TRUE);
            HalDispMopRot0SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
            //Rot_r input_width=pitch, height=source_height align 16
            u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot0SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            HalDispMopgSetPitch(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16tempRotInH);

            HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16RotDummyPix);

            //rotate, exchange source W&H
            HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId,
                                       pstInputPortContain->stAttr.u16SrcHeight,
                                       pstInputPortContain->stAttr.u16SrcWidth);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstInputPortContain->stAttr.u16SrcHeight)
            {
                u16Hratio = ((pstInputPortContain->stAttr.u16SrcHeight*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstInputPortContain->stAttr.u16SrcWidth)
            {
                u16Vratio = ((pstInputPortContain->stAttr.u16SrcWidth*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
        }
        else if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_270)
        {
            HalDispMopRot0En(pCtx, eMopId, TRUE);
            HalDispMopRot0SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
            u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot0SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            HalDispMopgSetPitch(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16tempRotInH);

            //rotate, exchange source W&H
            HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId,
                                       pstInputPortContain->stAttr.u16SrcHeight,
                                       pstInputPortContain->stAttr.u16SrcWidth);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstInputPortContain->stAttr.u16SrcHeight)
            {
                u16Hratio = ((pstInputPortContain->stAttr.u16SrcHeight*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstInputPortContain->stAttr.u16SrcWidth)
            {
                u16Vratio = ((pstInputPortContain->stAttr.u16SrcWidth*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }

            //reset to 0, 270 no need
            HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, 0);
        }
        else
        {
            HalDispMopRot0En(pCtx, eMopId, FALSE);
            HalDispMopRot0SetRotateMode(pCtx, eMopId, E_HAL_DISP_ROTATE_NONE);
            DISP_ERR("%s %d, MOPG %s is not supported\n", __FUNCTION__, __LINE__, PARSING_MOP_ROT_ANGLE(pstHalInputPortCfg->enRotate));
        }

    }
    else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_NONE)
        {
            HalDispMopRot1En(pCtx, eMopId, FALSE);
            HalDispMopRot1SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
        }
        else if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_90)
        {
            HalDispMopRot1En(pCtx, eMopId, TRUE);
            HalDispMopRot1SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
            //Rot_r input_width=pitch, height=source_height align 16
            u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot1SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            HalDispMopsSetPitch(pCtx, eMopId, u16tempRotInH);

            HalDispMopsSetAddr16Offset(pCtx, eMopId, u16RotDummyPix);

            //rotate, exchange source W&H
            HalDispMopsSetSourceParam(pCtx, eMopId, pstInputPortContain->stAttr.u16SrcHeight,
                                      pstInputPortContain->stAttr.u16SrcWidth);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstInputPortContain->stAttr.u16SrcHeight)
            {
                u16Hratio = ((pstInputPortContain->stAttr.u16SrcHeight*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstInputPortContain->stAttr.u16SrcWidth)
            {
                u16Vratio = ((pstInputPortContain->stAttr.u16SrcWidth*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
        }
        else if(pstHalInputPortCfg->enRotate==E_HAL_DISP_ROTATE_270)
        {
            HalDispMopRot1En(pCtx, eMopId, TRUE);
            HalDispMopRot1SetRotateMode(pCtx, eMopId, pstHalInputPortCfg->enRotate);
            //Rot_r input_width=pitch, height=source_height align 16
            u16tempRotInH = pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot1SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            if((u16tempRotInH & 0xFF))
            {
                u16tempRotInH = ((u16tempRotInH>>8) + 1)<<8;
            }
            HalDispMopsSetPitch(pCtx, eMopId, u16tempRotInH);

            //rotate, exchange source W&H
            HalDispMopsSetSourceParam(pCtx, eMopId, pstInputPortContain->stAttr.u16SrcHeight,
                                      pstInputPortContain->stAttr.u16SrcWidth);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstInputPortContain->stAttr.u16SrcHeight)
            {
                u16Hratio = ((pstInputPortContain->stAttr.u16SrcHeight*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstInputPortContain->stAttr.u16SrcWidth)
            {
                u16Vratio = ((pstInputPortContain->stAttr.u16SrcWidth*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }

            //reset to 0, 270 no need
            HalDispMopsSetAddr16Offset(pCtx, eMopId, 0);
        }
        else
        {
            HalDispMopRot1En(pCtx, eMopId, FALSE);
            HalDispMopRot1SetRotateMode(pCtx, eMopId, E_HAL_DISP_ROTATE_NONE);
            DISP_ERR("%s %d, MOPG %s is not supported\n", __FUNCTION__, __LINE__, PARSING_MOP_ROT_ANGLE(pstHalInputPortCfg->enRotate));
        }
    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVidWinRect_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstHalInputPortCfg = (HalDispVidWinRect_t *)pCfg;

    if((pstHalInputPortCfg->u16X&0x1)||
       (pstHalInputPortCfg->u16Y&0x1)||
       (pstHalInputPortCfg->u16Width&0x1)||
       (pstHalInputPortCfg->u16Height&0x1))//crop parameters should be all 2 align
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, crop parameters should be 2 align\n", __FUNCTION__, __LINE__);
        DISP_ERR("%s %d, Crop(%d %d %d %d) not support\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->u16X, pstHalInputPortCfg->u16Y,
        pstHalInputPortCfg->u16Width, pstHalInputPortCfg->u16Height);
    }

    memcpy(&pstInputPortContain->stCrop, pstHalInputPortCfg, sizeof(HalDispVidWinRect_t));

    pstInputPortContain->stAttr.u16SrcWidth = pstHalInputPortCfg->u16Width;
    pstInputPortContain->stAttr.u16SrcHeight = pstHalInputPortCfg->u16Height;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Crop(%d %d %d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->u16X, pstHalInputPortCfg->u16Y,
        pstHalInputPortCfg->u16Width, pstHalInputPortCfg->u16Height);

    if(pstInputPortContain->u32PortId > E_HAL_DISP_MOPG_ID_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }

    return enRet;
}



void HalDispMopIfSetInputPortCrop(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVidWinRect_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    u16 u16Hratio = 0x1000;
    u16 u16Vratio = 0x1000;
    u16 u16tempRotInH = 0;
    u16 u16RotDummyPix = 0;
    HalDispMopId_e eMopId = 0;
    //count crop+rotate case, coordinate for before rotate_w
    u16 u16tmpHstForCR = 0;
    u16 u16tmpHendForCR = 0;
    u16 u16tmpVstForCR = 0;
    u16 u16tmpVendForCR = 0;
    u16 u16tmpWidthForCR = 0;
    u16 u16tmpHeightForCR = 0;
    u16 u16tempPitch = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalInputPortCfg = (HalDispVidWinRect_t *)pCfg;
    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPG_VIDEO1_ID))&&
       (pstInputPortContain->u32PortId < E_HAL_DISP_MOPG_ID_NUM))//mopg
    {
        if((pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)||
           (pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270))//rotate case
        {
            //count mop source W/H for crop+ rotate case
            u16tmpHstForCR = Count_str_align(pstInputPortContain->stCrop.u16X);
            u16tmpVstForCR = Count_str_align(pstInputPortContain->stCrop.u16Y);
            u16tmpHendForCR = Count_end_align(pstInputPortContain->stCrop.u16X+pstInputPortContain->stAttr.u16SrcWidth);
            u16tmpVendForCR = Count_end_align(pstInputPortContain->stCrop.u16Y+pstInputPortContain->stAttr.u16SrcHeight);
            u16tmpWidthForCR = u16tmpHendForCR - u16tmpHstForCR;
            u16tmpHeightForCR = u16tmpVendForCR - u16tmpVstForCR;
            //rotate, exchange source W&H
            HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId,
                                      u16tmpHeightForCR, u16tmpWidthForCR);

            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstHalInputPortCfg->u16Height)
            {
                u16Hratio = ((pstHalInputPortCfg->u16Height*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstHalInputPortCfg->u16Width)
            {
                u16Vratio = ((pstHalInputPortCfg->u16Width*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }

            //Rot_r input_width=pitch, height=source_height align 16
            u16tempRotInH = pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot0SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            //refined
            u16tempPitch = u16tempRotInH - Count_str_align(pstInputPortContain->stCrop.u16Y);
            if((u16tempPitch & 0xFF))
            {
                u16tempPitch = ((u16tempPitch>>8) + 1)<<8;
            }
            HalDispMopgSetPitch(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16tempPitch);

            //crop + rotate new settings
            HalDispMopRot0SetRotateCropXEn(pCtx, eMopId, 1);
            HalDispMopRot0SetRotateCropYEn(pCtx, eMopId, 1);
            HalDispMopRot0SetRotateCropXClipNum(pCtx, eMopId, u16tmpHstForCR, u16tmpWidthForCR);
            HalDispMopRot0SetRotateCropYClipNum(pCtx, eMopId, u16tmpVstForCR, u16tmpHeightForCR);

            if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)
            {
                //refined
                u16RotDummyPix = u16tempRotInH - (pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight);
                HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16RotDummyPix);
            }
            else if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270)
            {
                //refined
                u16RotDummyPix = pstInputPortContain->stCrop.u16Y - Count_str_align(pstInputPortContain->stCrop.u16Y);
                HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16RotDummyPix);
            }
            else
            {
                //reset to 0
                HalDispMopgSetAddr16Offset(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, 0);
            }
        }
        else//not rotate case
        {
            HalDispMopgSetSourceParam(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, pstHalInputPortCfg->u16Width,
                                                                                  pstHalInputPortCfg->u16Height);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstHalInputPortCfg->u16Width)
            {
                u16Hratio = ((pstHalInputPortCfg->u16Width*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopgSetHScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstHalInputPortCfg->u16Height)
            {
                u16Vratio = ((pstHalInputPortCfg->u16Height*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopgSetVScaleFac(pCtx, eMopId, (HalDispMopgGwinId_e)pstInputPortContain->u32PortId, u16Vratio);
            }
        }
    }
       else if(((pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_ID)||(pstVidLayerContain->u32VidLayerId==E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))&&
            (pstInputPortContain->u32PortId == E_HAL_DISP_MOPS_ID_00))//mops
    {
        if((pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)||
           (pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270))//rotate case
        {
            //count mop source W/H for crop+ rotate case
            u16tmpHstForCR = Count_str_align(pstInputPortContain->stCrop.u16X);
            u16tmpVstForCR = Count_str_align(pstInputPortContain->stCrop.u16Y);
            u16tmpHendForCR = Count_end_align(pstInputPortContain->stCrop.u16X+pstInputPortContain->stAttr.u16SrcWidth);
            u16tmpVendForCR = Count_end_align(pstInputPortContain->stCrop.u16Y+pstInputPortContain->stAttr.u16SrcHeight);
            u16tmpWidthForCR = u16tmpHendForCR - u16tmpHstForCR;
            u16tmpHeightForCR = u16tmpVendForCR - u16tmpVstForCR;
            //rotate, exchange source W&H
            HalDispMopsSetSourceParam(pCtx, eMopId, u16tmpHeightForCR,
                                      u16tmpWidthForCR);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstHalInputPortCfg->u16Height)
            {
                u16Hratio = ((pstHalInputPortCfg->u16Height*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstHalInputPortCfg->u16Width)
            {
                u16Vratio = ((pstHalInputPortCfg->u16Width*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
            //Rot_r input_width=pitch, height=source_height align 16
            u16tempRotInH = pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight;
            u16RotDummyPix = (u16tempRotInH & 0xF);
            if(u16RotDummyPix)
            {
                u16tempRotInH = ((u16tempRotInH>>4) + 1)<<4;
            }
            HalDispMopRot1SourceHeight(pCtx, eMopId, u16tempRotInH);
            //pitch is the same as source width, but 256 align
            //refined
            u16tempPitch = u16tempRotInH - Count_str_align(pstInputPortContain->stCrop.u16Y);
            if((u16tempPitch & 0xFF))
            {
                u16tempPitch = ((u16tempPitch>>8) + 1)<<8;
            }
            HalDispMopsSetPitch(pCtx, eMopId, u16tempPitch);

            //crop + rotate new settings
            HalDispMopRot1SetRotateCropXEn(pCtx, eMopId, 1);
            HalDispMopRot1SetRotateCropYEn(pCtx, eMopId, 1);
            HalDispMopRot1SetRotateCropXClipNum(pCtx, eMopId, u16tmpHstForCR, u16tmpWidthForCR);
            HalDispMopRot1SetRotateCropYClipNum(pCtx, eMopId, u16tmpVstForCR, u16tmpHeightForCR);

            if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_90)
            {
                //refined
                u16RotDummyPix = u16tempRotInH - (pstInputPortContain->stCrop.u16Y + pstInputPortContain->stAttr.u16SrcHeight);
                HalDispMopsSetAddr16Offset(pCtx, eMopId, u16RotDummyPix);
            }
            else if(pstInputPortContain->stRot.enRotate==E_HAL_DISP_ROTATE_270)
            {
                //refined
                u16RotDummyPix = pstInputPortContain->stCrop.u16Y - Count_str_align(pstInputPortContain->stCrop.u16Y);
                HalDispMopsSetAddr16Offset(pCtx, eMopId, u16RotDummyPix);
            }
            else
            {
                //reset to 0
                HalDispMopsSetAddr16Offset(pCtx, eMopId, 0);
            }
        }
        else//not rotate case
        {
            HalDispMopsSetSourceParam(pCtx, eMopId, pstHalInputPortCfg->u16Width,
                                      pstHalInputPortCfg->u16Height);
            if(pstInputPortContain->stAttr.stDispWin.u16Width > pstHalInputPortCfg->u16Width)
            {
                u16Hratio = ((pstHalInputPortCfg->u16Width*4096)/pstInputPortContain->stAttr.stDispWin.u16Width);
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            else
            {
                u16Hratio = 0x1000;
                HalDispMopsSetHScaleFac(pCtx, eMopId, u16Hratio);
            }
            if(pstInputPortContain->stAttr.stDispWin.u16Height > pstHalInputPortCfg->u16Height)
            {
                u16Vratio = ((pstHalInputPortCfg->u16Height*4096)/pstInputPortContain->stAttr.stDispWin.u16Height);
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
            else
            {
                u16Vratio = 0x1000;
                HalDispMopsSetVScaleFac(pCtx, eMopId, u16Vratio);
            }
        }

    }
    else
    {
        DISP_ERR("%s %d, Port_id:%ld not support\n", __FUNCTION__, __LINE__, pstInputPortContain->u32PortId);
    }
}

HalDispQueryRet_e HalDispMopIfGetInfoInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    HalDispRingBuffAttr_t *pstHalRingBuffAttr = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstHalRingBuffAttr = (HalDispRingBuffAttr_t *)pCfg;

    if(pstHalRingBuffAttr->bEn && pstHalRingBuffAttr->u16BuffHeight == 0)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Ring is On, BuffHeight is 0 \n", __FUNCTION__, __LINE__);
    }
    else
    {
        memcpy(&pstInputPortContain->stRingBuffAttr, pstHalRingBuffAttr, sizeof(HalDispRingBuffAttr_t));
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d, BuffHeight:%d, StartLine:%d, Access:%s\n",
            __FUNCTION__, __LINE__,
            pstInputPortContain->stRingBuffAttr.bEn,
            pstInputPortContain->stRingBuffAttr.u16BuffHeight,
            pstInputPortContain->stRingBuffAttr.u16BuffStartLine,
            PARSING_HAL_DMA_ACCESS_TYPE(pstInputPortContain->stRingBuffAttr.eAccessType));
    }


    return enRet;
}

void HalDispMopIfSetInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispRingBuffAttr_t *pstHalRingBuffAttr = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    HalDispMopId_e eMopId = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    pstHalRingBuffAttr = (HalDispRingBuffAttr_t *)pCfg;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    HalDispMopsSetRingEn(pCtx, eMopId, pstHalRingBuffAttr->bEn);
    HalDispMopsSetRingSrc(pCtx, eMopId, pstHalRingBuffAttr->eAccessType == E_HAL_DISP_DMA_ACCESS_TYPE_IMI ? 1 : 0);
    HalDispMopsSetRingBuffHeight(pCtx, eMopId, pstHalRingBuffAttr->u16BuffHeight);
    HalDispMopsSetBuffStartLine(pCtx, eMopId, pstHalRingBuffAttr->u16BuffStartLine);
}

HalDispQueryRet_e HalDispMopIfGetInfoStretchWinSize(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispVideoLayerAttr_t *stVdInf = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    stVdInf = (HalDispVideoLayerAttr_t *)pCfg;

    memcpy(&pstVidLayerContain->stAttr, stVdInf, sizeof(HalDispVideoLayerAttr_t));

    return enRet;
}

void HalDispMopIfSetInfoStretchWinSize(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    HalDispMopId_e eMopId = 0;
    HalDispVideoLayerAttr_t *stVdInf = NULL;
    u16 u16Width = 0;
    u16 u16Height = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    stVdInf = (HalDispVideoLayerAttr_t *)pCfg;

    u16Width = stVdInf->stVidLayerDispWin.u16Width;
    u16Height = stVdInf->stVidLayerDispWin.u16Height;

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    HalDispMopgSetStretchWinSize(pCtx, eMopId, u16Width, u16Height);
    HalDispMopsSetStretchWinSize(pCtx, eMopId, u16Width, u16Height);

}

HalDispQueryRet_e HalDispMopIfGetInfoImiStartAddr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void HalDispMopIfSetImiStartAddr(void *pCtx, void *pCfg)
{
    u32 *pu32ImiAddr = NULL;

    pu32ImiAddr = (u32 *)pCfg;
    _HalDispMopIfSetMopRotImiIniAddrManage(*pu32ImiAddr);
}

void HalDispMopIfSetInfoHextSize(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain;
    DrvDispCtxDeviceContain_t *pstDevContain;
    HalDispMopId_e eMopId = 0;
    u16 u16Hext = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstVidLayerContain = pstDispCtxCfg->pstCtxContain->pstVidLayerContain[pstDispCtxCfg->u32Idx];
    pstDevContain = (DrvDispCtxDeviceContain_t *)pstVidLayerContain->pstDevCtx;

    u16Hext = pstDevContain->stDevTimingCfg.u16Htotal - pstDevContain->stDevTimingCfg.u16Hactive - 9;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, debug, Htotal=%d, Hactive=%d, Hext=%d\n", __FUNCTION__, __LINE__, pstDevContain->stDevTimingCfg.u16Htotal
                                                                  , pstDevContain->stDevTimingCfg.u16Hactive, u16Hext);

    eMopId = _HalDispMopIfMopIdMap((HalDispVideoLayerId_e)pstVidLayerContain->u32VidLayerId);

    HalDispMopgSetHextSize(pCtx, eMopId, u16Hext);
    HalDispMopsSetHextSize(pCtx, eMopId, u16Hext);

}

void HalDispMopIfInputPortDeinit(void)
{
    int i=0;
    //rotate deinit
    for(i=0;i<E_HAL_DISP_MOP_NUM;i++)
    {
        _gstMopgRotRec[i].bRotEn = 0;
        _gstMopgRotRec[i].eMopgId = E_HAL_DISP_MOPG_ID_NUM;
        _gstMopgRotRec[i].stRotAng = E_HAL_DISP_ROTATE_NONE;
    }
    //free imi
    if(bImiInit)
    {
        //DrvDispOsImiHeapFree(u32MopRotImiTotalAddr);
        bImiInit=0;
    }
    if(bImiSetAddrInit)
    {
        bImiSetAddrInit=0;
    }

}

void HalDispMopIfSetInfoStretchWinMop0Auto(void *pCtx, bool bEnAuto)
{
    HalDispMopgSetAutoStretchWinSizeEn(pCtx, E_HAL_DISP_MOPID_00, bEnAuto);
    HalDispMopsSetAutoStretchWinSizeEn(pCtx, E_HAL_DISP_MOPID_00, bEnAuto);
}

void HalDispMopIfSetDdbfWr(void *pCtx)
{
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_00,E_HAL_DISP_MOPROT_ROT0_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_00,E_HAL_DISP_MOPROT_ROT1_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_01,E_HAL_DISP_MOPROT_ROT0_ID);
    HalDispMopRotDbBfWr(pCtx, E_HAL_DISP_MOPID_01,E_HAL_DISP_MOPROT_ROT1_ID);

    HalDispMopDbBfWr(pCtx, E_HAL_DISP_MOPID_00);
    HalDispMopDbBfWr(pCtx, E_HAL_DISP_MOPID_01);
}


void HalDispMopIfSetForceWr(void *pCtx, bool bEn)
{
    HalDispMopSetForceWrite(pCtx, E_HAL_DISP_MOPID_00, bEn);
    HalDispMopSetForceWrite(pCtx, E_HAL_DISP_MOPID_01, bEn);
}

