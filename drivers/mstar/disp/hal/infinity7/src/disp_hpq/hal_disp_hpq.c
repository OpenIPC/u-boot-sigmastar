/*
* hal_disp_hpq.c- Sigmastar
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

#define _HAL_DISP_HPQ_C_

#include "drv_disp_os.h"

#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"

#include "hal_disp_st.h"
#include "hal_disp_hpq.h"

// hpq
#include "drv_disp_hpq_os.h"
#include "hal_scl_pq_color_reg.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"
#include "drv_scl_pq_data_types.h"
#include "drv_disp_pq_bin.h"

#include "hal_disp_hpq_main.c"
#include "hal_disp_hpq_if.c"

#include "drv_disp_hpq_bin.c"

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
HalDispPqLoadSettingType_e g_ePqLoadSettingType = E_HAL_DISP_PQ_LOAD_SETTING_AUTO;

static u8 _HalDispHpqTransInterface(u32 u32Interface)
{
    u8 u8PqInterface = PQ_BIN_INFO_IP_INTF_NONE;

    u8PqInterface = (u32Interface & HAL_DISP_INTF_HDMI)       ? PQ_BIN_INFO_IP_INTF_HDMI
                    : (u32Interface == HAL_DISP_INTF_VGA)     ? PQ_BIN_INFO_IP_INTF_VGA
                    : (u32Interface == HAL_DISP_INTF_CVBS)    ? PQ_BIN_INFO_IP_INTF_CVBS
                    : (u32Interface == HAL_DISP_INTF_BT1120)  ? PQ_BIN_INFO_IP_INTF_BT1120
                    : (u32Interface == HAL_DISP_INTF_BT656)   ? PQ_BIN_INFO_IP_INTF_BT656
                    : (u32Interface == HAL_DISP_INTF_BT601)   ? PQ_BIN_INFO_IP_INTF_BT601
                    : (u32Interface == HAL_DISP_INTF_TTL)     ? PQ_BIN_INFO_IP_INTF_TTL
                    : (u32Interface == HAL_DISP_INTF_MIPIDSI) ? PQ_BIN_INFO_IP_INTF_MIPIDSI
                    : (u32Interface == HAL_DISP_INTF_MCU)     ? PQ_BIN_INFO_IP_INTF_MCU
                    : (u32Interface == HAL_DISP_INTF_SRGB)    ? PQ_BIN_INFO_IP_INTF_SRGB
                                                              : PQ_BIN_INFO_IP_INTF_NONE;

    return u8PqInterface;
}
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

// ----------------------------------
void HalDispHpqLoadBin(void *pCtx, void *pCfg)
{
    HalDispPqConfig_t *pPqCfg = (HalDispPqConfig_t *)pCfg;
    DrvPQInitInfo_t    stPqInfo;

    memset(&stPqInfo, 0, sizeof(stPqInfo));

    stPqInfo.u8PQTextBinCnt                 = 0;
    stPqInfo.u8PQBinCnt                     = 1;
    stPqInfo.stPQBinInfo[0].PQ_Bin_BufSize  = pPqCfg->u32DataSize;
    stPqInfo.stPQBinInfo[0].pPQBin_AddrVirt = pPqCfg->pData;
    stPqInfo.stPQBinInfo[0].PQBin_PhyAddr   = 0;
    stPqInfo.stPQBinInfo[0].u8PQID          = 0;

    DrvPQInit(&stPqInfo, pCtx);
    DrvPQInitSourceTypeTable(0);
}
void HalDispHpqFreeBin(void *pCtx, void *pCfg)
{
    HalDispPqConfig_t *pPqCfg = (HalDispPqConfig_t *)pCfg;
    DrvPQInitInfo_t    stPqInfo;

    memset(&stPqInfo, 0, sizeof(stPqInfo));

    stPqInfo.u8PQTextBinCnt                 = 0;
    stPqInfo.u8PQBinCnt                     = 1;
    stPqInfo.stPQBinInfo[0].PQ_Bin_BufSize  = pPqCfg->u32DataSize;
    stPqInfo.stPQBinInfo[0].pPQBin_AddrVirt = pPqCfg->pData;
    stPqInfo.stPQBinInfo[0].PQBin_PhyAddr   = 0;
    stPqInfo.stPQBinInfo[0].u8PQID          = 0;

    DrvPQFreeSourceTypeTable(0);
    DrvPQExit(&stPqInfo, pCtx);
}

u8 HalDispHpqGetSourceIdx(void *pCtx, u16 *pu16SrcIdx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    DrvPQSrcTypeInfo_t         stPqSrcTypeInfo;
    u8                         bRet;

    if (g_ePqLoadSettingType == E_HAL_DISP_PQ_LOAD_SETTING_AUTO)
    {
        pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
        pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];

        stPqSrcTypeInfo.u8Device    = pstDeviceContain->u32DevId;
        stPqSrcTypeInfo.u8Interface = _HalDispHpqTransInterface(pstDeviceContain->u32Interface);
        stPqSrcTypeInfo.u16Width    = pstDeviceContain->stDevTimingCfg.u16Hactive;
        stPqSrcTypeInfo.u16Height   = pstDeviceContain->stDevTimingCfg.u16Vactive;
        bRet                        = DrvPQGetMatchSoruceType(0, &stPqSrcTypeInfo, pu16SrcIdx);
    }
    else
    {
        bRet = 1;
    }
    return bRet;
}

void HalDispHqpLoadSetting(void *pCtx)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;

    pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    DrvPQLoadSettings(pstDeviceContain->u32DevId);
}

void HalDispHpqSetSrcId(void *pCtx, u16 u16DevId, u16 u16SrcId)
{
    DrvPQSetSrcType(u16DevId, u16SrcId);
}

void HalDispHpqSetPnlId(void *pCtx, u16 u16PnlId)
{
    u16 u16PnlNum = DrvPQGetPnlNum(0);
    if (u16PnlId < u16PnlNum)
    {
        DrvPQSetPnlId(0, u16PnlId);
    }
}

void HalDispHpqLoadData(void *pCtx, u8 *pu8Data, u32 u32DataSize)
{
    u16                        u16TblIdx;
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    u32                        u32DevId         = 0;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    if (pstDispCtxCfg && pstDispCtxCfg->pstCtxContain)
    {
        pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    }
    if (pstDeviceContain)
    {
        u32DevId = pstDeviceContain->u32DevId;
    }
    u16TblIdx = DrvPQGetTableIndex(u32DevId, PQ_BIN_PICTURE_IP_DIX);
    DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, TblIdx:%d\n", __FUNCTION__, __LINE__, u16TblIdx);

    DrvPQLoadTableData(0, u16TblIdx, PQ_BIN_PICTURE_IP_DIX, pu8Data, u32DataSize);
}

void HalDispHpqSetLoadSettingType(HalDispPqLoadSettingType_e eType)
{
    g_ePqLoadSettingType = eType;
    DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, Setting eType(%u)\n", __FUNCTION__, __LINE__, eType);
}

HalDispPqLoadSettingType_e HalDispHpqGetLoadSettingType(void)
{
    return g_ePqLoadSettingType;
}
void HalDispHpqSetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode)
{
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (HAL_DISP_DEV_HPQ_SUPPORT(u32Dev))
    {
        HalDispUtilityW2BYTEMSKByType(
            LCE_REG_VIP_FUN_BYPASS_EN + HAL_DISP_GET_BANKOFFSET(u32Dev, LCE_REG_VIP_FUN_BYPASS_EN),
            u8bBypass ? mask_of_lce_reg_vip_fun_bypass_en : 0, mask_of_lce_reg_vip_fun_bypass_en, pCmdqCtx, enRiuMode);
    }
}
void HalDispHpqSetPeakingCoef(void *pCtx, u16 u16Coef1, u16 u16Coef2)
{
    void *              pCmdqCtx   = NULL;
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    u32                 u32Dev     = 1;

    if (HalDispGetCmdqByCtx(pCtx, &pCmdqCtx))
    {
        u32Dev = HalDispGetDeviceId(pstDispCtx, 0);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND1_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef1 << shift_of_vps_reg_main_band1_coef, mask_of_vps_reg_main_band1_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND2_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef2 << shift_of_vps_reg_main_band2_coef, mask_of_vps_reg_main_band2_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND3_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef1 << shift_of_vps_reg_main_band3_coef, mask_of_vps_reg_main_band3_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND4_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef2 << shift_of_vps_reg_main_band4_coef, mask_of_vps_reg_main_band4_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND5_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef1 << shift_of_vps_reg_main_band5_coef, mask_of_vps_reg_main_band5_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND6_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef2 << shift_of_vps_reg_main_band6_coef, mask_of_vps_reg_main_band6_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND7_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef1 << shift_of_vps_reg_main_band7_coef, mask_of_vps_reg_main_band7_coef,
                                pCmdqCtx);
        HalDispUtilityW2BYTEMSK(VPS_REG_MAIN_BAND8_COEF + HAL_DISP_HPQ_BANKOFFSET(u32Dev),
                                u16Coef2 << shift_of_vps_reg_main_band8_coef, mask_of_vps_reg_main_band8_coef,
                                pCmdqCtx);
    }
    else
    {
        DISP_ERR("%s %d, GetCmdqCtx Fail\n", __FUNCTION__, __LINE__);
    }
    DISP_DBG_VAL(u32Dev);
}
