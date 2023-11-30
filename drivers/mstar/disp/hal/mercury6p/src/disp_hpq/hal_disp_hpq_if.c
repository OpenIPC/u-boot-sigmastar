/*
* hal_disp_hpq_if.c- Sigmastar
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

#define _MDRV_PQ_C_

#include "drv_disp_os_header.h"
#include "drv_disp_hpq_os.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "hal_scl_pq_color_reg.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"
#include "drv_scl_pq_data_types.h"
#include "hal_disp_hpq.h"
#include "drv_disp_pq_bin.h"

#include "drv_disp_hpq_quality_mode.c"
#include "drv_disp_os.h"


static u16           g_u16PQSrcType;
DrvPQSrcTypeConfig_t g_stPqSrcTypeCfg = {0, NULL};

static u8                       g_bPQBinInit = 0;
DrvPQBinHeaderInfo_t            g_stPQBinHeaderInfo[E_MAX_PQ_BIN_NUM];
extern DrvPQTextBinHeaderInfo_t g_stPQTextBinHeaderInfo[E_PQ_TEXT_BIN_MAX];

//////////////////////////////////////////////////////////////////
static void _DrvPQClearSRAMTableIndex(void)
{
    if (g_bPQBinInit)
    {
        DrvDispPQBinClearSRAMTableIndex();
    }
}
u8 DrvPQGetMatchSoruceType(E_PQ_WIN_TYPE_e eWindow, DrvPQSrcTypeInfo_t *pSrcTypeInfo, u16 *pu16SrcType)
{
    u16 u16Tblidx;
    u16 u16SrcType          = 0xFFFF;
    u16 u16FirstDeviceMatch = 0xFFFF;
    u8  bRet                = 1;

    if (g_stPqSrcTypeCfg.pSrcTypeInfoTbl && g_stPqSrcTypeCfg.u16SrcTypeNum)
    {
        for (u16Tblidx = 0; u16Tblidx < g_stPqSrcTypeCfg.u16SrcTypeNum; u16Tblidx++)
        {
            if (u16FirstDeviceMatch == 0xFFFF
                && pSrcTypeInfo->u8Device == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Device)
            {
                u16FirstDeviceMatch = u16Tblidx;
            }

            if (pSrcTypeInfo->u8Device == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Device
                && pSrcTypeInfo->u8Interface == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Interface
                && pSrcTypeInfo->u16Width == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u16Width
                && pSrcTypeInfo->u16Height == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u16Height)
            {
                u16SrcType = u16Tblidx;
                break;
            }
        }

        if (u16SrcType == 0xFFFF)
        {
            if (pSrcTypeInfo->u8Device == PQ_BIN_INFO_IP_INTF_TTL
                || pSrcTypeInfo->u8Device == PQ_BIN_INFO_IP_INTF_MIPIDSI
                || pSrcTypeInfo->u8Device == PQ_BIN_INFO_IP_INTF_MCU
                || pSrcTypeInfo->u8Device == PQ_BIN_INFO_IP_INTF_SRGB)
            {
                for (u16Tblidx = 0; u16Tblidx < g_stPqSrcTypeCfg.u16SrcTypeNum; u16Tblidx++)
                {
                    if (pSrcTypeInfo->u8Device == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Device
                        && pSrcTypeInfo->u8Interface == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Interface)
                    {
                        u16SrcType = u16Tblidx;
                        break;
                    }
                }
            }
            else
            {
                for (u16Tblidx = 0; u16Tblidx < g_stPqSrcTypeCfg.u16SrcTypeNum; u16Tblidx++)
                {
                    if (pSrcTypeInfo->u8Device == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Device
                        && pSrcTypeInfo->u16Width == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u16Width
                        && pSrcTypeInfo->u16Height == g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u16Height
                        && g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16Tblidx].u8Interface == PQ_BIN_INFO_IP_INTF_NONE)
                    {
                        u16SrcType = u16Tblidx;
                        break;
                    }
                }
            }
        }

        *pu16SrcType = u16SrcType == 0xFFFF ? u16FirstDeviceMatch : u16SrcType;
        DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, Dev:%d, Intf:%d, Width:%d, Height:%d, SrcType:%d\n", __FUNCTION__,
                 __LINE__, pSrcTypeInfo->u8Device, pSrcTypeInfo->u8Interface, pSrcTypeInfo->u16Width,
                 pSrcTypeInfo->u16Height, *pu16SrcType);

        bRet = 1;
    }
    else
    {
        bRet = 0;
        DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, NO PQ BIN LOAD\n", __FUNCTION__, __LINE__);
    }

    return bRet;
}
u8 DrvPQFreeSourceTypeTable(u32 u32Dev)
{
    if (g_stPqSrcTypeCfg.pSrcTypeInfoTbl)
    {
        DrvDispOsMemRelease((void *)g_stPqSrcTypeCfg.pSrcTypeInfoTbl);
        g_stPqSrcTypeCfg.pSrcTypeInfoTbl = NULL;
        g_stPqSrcTypeCfg.u16SrcTypeNum   = 0;
    }
    return 1;
}
u8 DrvPQInitSourceTypeTable(u32 u32Dev)
{
    u16                     u16PnlNum, u16SrcIdx;
    u8                      bRet = 1;
    DrvPQBinSourceLUTInfo_t stPqBinSrcLutInfo;
    u8                      u8SourceInfoBuf[PQ_BIN_IP_INFO_INFO_SIZE / 8];
    DrvPQBinHeaderInfo_t *  pPqBinHeader = NULL;

    pPqBinHeader = &g_stPQBinHeaderInfo[0];
    u16PnlNum    = DrvDispPQBinGetSourceLutPnlNum(pPqBinHeader);

    if (u16PnlNum != 1)
    {
        DISP_ERR("%s %d, PnlNum:%d more than 1\n", __FUNCTION__, __LINE__, u16PnlNum);
        bRet = 0;
    }
    else
    {
        DrvDispPQBinGetSourceLut(pPqBinHeader, 0, &stPqBinSrcLutInfo);

        if (g_stPqSrcTypeCfg.u16SrcTypeNum < stPqBinSrcLutInfo.u16SourceNum)
        {
            if (g_stPqSrcTypeCfg.pSrcTypeInfoTbl)
            {
                DrvDispOsMemRelease((void *)g_stPqSrcTypeCfg.pSrcTypeInfoTbl);
                g_stPqSrcTypeCfg.pSrcTypeInfoTbl = NULL;
            }

            g_stPqSrcTypeCfg.pSrcTypeInfoTbl =
                DrvDispOsMemAlloc(sizeof(DrvPQSrcTypeInfo_t) * stPqBinSrcLutInfo.u16SourceNum);
            g_stPqSrcTypeCfg.u16SrcTypeNum = stPqBinSrcLutInfo.u16SourceNum;
        }

        if (g_stPqSrcTypeCfg.pSrcTypeInfoTbl == NULL)
        {
            DISP_ERR("%s %d, Allocate SrcTypeInfo Table Fail\n", __FUNCTION__, __LINE__);
            bRet = 0;
        }
        else
        {
            u16 u16TblIdx;

            for (u16SrcIdx = 0; u16SrcIdx < stPqBinSrcLutInfo.u16SourceNum; u16SrcIdx++)
            {
                DrvPQSetSrcType(u32Dev, u16SrcIdx);
                u16TblIdx = DrvPQGetTableIndex(u32Dev, PQ_BIN_INFO_IP_IDX);
                DrvPQLoadTableData(0, u16TblIdx, 0, u8SourceInfoBuf, PQ_BIN_IP_INFO_INFO_SIZE);
                g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u8Device    = u8SourceInfoBuf[PQ_BIN_IP_INFO_DEVICE_IDX];
                g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u8Interface = u8SourceInfoBuf[PQ_BIN_IP_INFO_INTERFACE_IDX];
                g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u16Width =
                    u8SourceInfoBuf[PQ_BIN_IP_INFO_WIDTH_IDX] | u8SourceInfoBuf[PQ_BIN_IP_INFO_WIDTH_IDX + 1] << 8;
                g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u16Height =
                    u8SourceInfoBuf[PQ_BIN_IP_INFO_HEIGHT_IDX] | u8SourceInfoBuf[PQ_BIN_IP_INFO_HEIGHT_IDX + 1] << 8;
                DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, SrcIdx:%d, Device:%d, Intf:%d, Width:%d, Heigh:%d\n", __FUNCTION__,
                         __LINE__, u16SrcIdx, g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u8Device,
                         g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u8Interface,
                         g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u16Width,
                         g_stPqSrcTypeCfg.pSrcTypeInfoTbl[u16SrcIdx].u16Height);
            }
        }
    }

    return bRet;
}
u8 DrvPQExit(DrvPQInitInfo_t *pstPQInitInfo, void *pCtx)
{
    if (g_bPQBinInit)
    {
        g_bPQBinInit = 0;
    }
    return 1;
}

u8 DrvPQInit(DrvPQInitInfo_t *pstPQInitInfo, void *pCtx)
{
    DrvPQTableTotalInfo_t stPQTableInfo;
    u8                    u8ID;

    memset(&stPQTableInfo, 0, sizeof(stPQTableInfo));

    _DrvPQClearSRAMTableIndex();

    if (pstPQInitInfo->u8PQBinCnt)
    {
        DISP_ERR("%s %d, Start PQ Bin Init\n", __FUNCTION__, __LINE__);
        DrvDispPQBinSetDisplayType(E_PQ_BIN_DISPLAY_ONE, MAIN_WINDOW);

        DrvDispPQBinSetPanelID(0, MAIN_WINDOW);

        // because main and sub's common table may be different,
        // so we will take main's common table as correct one.
        // so here load sub first and then use main's overwrite sub's
        // Sub first
        if (pstPQInitInfo->stPQBinInfo[E_PQ_BIN_STD_MAIN].PQ_Bin_BufSize > 0)
        {
            // E_PQ_WIN_TYPE_e ePQWin;
            u8ID                                        = pstPQInitInfo->stPQBinInfo[E_PQ_BIN_STD_MAIN].u8PQID;
            g_stPQBinHeaderInfo[u8ID].u8BinID           = pstPQInitInfo->stPQBinInfo[E_PQ_BIN_STD_MAIN].u8PQID;
            g_stPQBinHeaderInfo[u8ID].paBinStartAddress = (pstPQInitInfo->stPQBinInfo[E_PQ_BIN_STD_MAIN].PQBin_PhyAddr);
            g_stPQBinHeaderInfo[u8ID].pvBinStartAddress =
                (pstPQInitInfo->stPQBinInfo[E_PQ_BIN_STD_MAIN].pPQBin_AddrVirt);
            DrvDispPQBinParsing(&g_stPQBinHeaderInfo[u8ID]);
            // load each sheet column comm setting.
            DrvDispPQBinLoadCommTable((u16)DrvDispPQBinGetPanelIdx(0), &g_stPQBinHeaderInfo[u8ID]);
            DISP_ERR("[%s]ID=%d BinStartAddress:%px\n", __FUNCTION__, u8ID,
                     g_stPQBinHeaderInfo[u8ID].pvBinStartAddress);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d,  Error: PQ_Bin_BufSize < 0 !!\n", __FUNCTION__, __LINE__);
        }
    }
    g_bPQBinInit = 1;
    return TRUE;
}

u16 DrvPQGetIPRegCount(u16 u16PQIPIdx)
{
    return 0;
}

u16 DrvPQGetIPRegCountSub(u16 u16PQIPIdx)
{
    return 0;
}

void DrvPQLoadSettings(u32 u32Dev)
{
    if (g_bPQBinInit)
    {
        g_stPQBinHeaderInfo[E_PQ_BIN_STD_MAIN].u32DevID = u32Dev;
        DrvDispPQBinLoadTableBySrcType(g_u16PQSrcType, PQ_BIN_IP_ALL, (u16)DrvDispPQBinGetPanelIdx(E_PQ_MAIN_WINDOW),
                                       &g_stPQBinHeaderInfo[E_PQ_BIN_STD_MAIN]);
    }
}

void DrvPQLoadTable(E_PQ_WIN_TYPE_e eWindow, u16 u16TabIdx, u16 u16PQIPIdx)
{
    if (eWindow == E_PQ_MAIN_WINDOW)
    {
        if (g_bPQBinInit)
        {
            DrvDispPQBinLoadTable((u16)DrvDispPQBinGetPanelIdx(E_PQ_MAIN_WINDOW), u16TabIdx, u16PQIPIdx,
                                  &g_stPQBinHeaderInfo[E_PQ_BIN_STD_MAIN]);
        }
    }
}

void DrvPQLoadTableData(E_PQ_WIN_TYPE_e eWindow, u16 u16TabIdx, u16 u16PQIPIdx, u8 *pTable, u16 u16TableSize)
{
    if (eWindow == E_PQ_MAIN_WINDOW)
    {
        if (g_bPQBinInit)
        {
            DrvDispPQBinLoadTableData((u16)DrvDispPQBinGetPanelIdx(E_PQ_MAIN_WINDOW), u16TabIdx, u16PQIPIdx,
                                      &g_stPQBinHeaderInfo[E_PQ_BIN_STD_MAIN], pTable, u16TableSize);
        }
    }
}

void DrvPQSetSrcType(u32 u32Dev, u16 u16SrcType)
{
    DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, Setting SrcType(%u)\n", __FUNCTION__, __LINE__, u16SrcType);
    g_u16PQSrcType = u16SrcType;
}

u16 DrvPQGetSrcType(u32 u32Dev)
{
    return g_u16PQSrcType;
}
u16 DrvPQGetPnlNum(E_PQ_WIN_TYPE_e eWindow)
{
    return DrvDispPQBinGetSourceLutPnlNum(&g_stPQBinHeaderInfo[0]);
}

void DrvPQSetPnlId(E_PQ_WIN_TYPE_e eWindow, u16 u16PnlId)
{
    DrvDispPQBinSetPanelID(u16PnlId, eWindow);
}

// Get main page setting from u16IPIdx(column), according to current input source type(row)
u16 DrvPQGetTableIndex(u32 u32Dev, u16 u16IPIdx)
{
    u8 u16TabIdx = 0;

    if (g_bPQBinInit)
    {
        u16TabIdx =
            (u8)DrvDispPQBinGetTableIndex(g_u16PQSrcType, u16IPIdx, (u16)DrvDispPQBinGetPanelIdx(E_PQ_MAIN_WINDOW),
                                          &g_stPQBinHeaderInfo[E_PQ_BIN_STD_MAIN]);
    }
    else
    {
        u16TabIdx = 0;
    }
    return u16TabIdx;
}
