/*
* drv_disp_hpq_bin.c- Sigmastar
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

#define _DRV_PQ_BIN_C

#include "drv_disp_os_header.h"

#include "drv_disp_hpq_os.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"
#include "drv_scl_pq_data_types.h"
#include "hal_scl_pq.h"

#include "drv_disp_pq_bin.h"

//#include "drv_scl_pq_quality_mode.c"

#ifdef PQ_REG_FUN
#undef PQ_REG_FUN
#define PQ_REG_FUNC
#endif
//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#ifndef UNUSED // to avoid compile warnings...
#define UNUSED(var) (void)((var) = (var))
#endif
#define PQ_BIN_DBG(x)             // x
#define PQ_BIN_DBG_HEADER(x)      // x
#define PQ_BIN_DUMP_DBG(x)        // x
#define PQ_BIN_DUMP_FILTER_DBG(x) //(x)
#define PQ_BIN_DBG_SRAMERROR(x)   // x

extern void *g_pHpqCtx;

#define DISP_PQ_REG_FUNC(u32Reg, u8Value, u8Mask) WBYTEMSK(u32Reg, u8Value, u8Mask)
//-------------------------------------------------------------------------------------------------
//  Local Variable
//-------------------------------------------------------------------------------------------------
static u16 g_u16PQTabIdx[E_PQ_MAX_WINDOW][PQ_BIN_MAX_PNL][PQ_BIN_MAX_IP]; // store all TabIdx of all IPs
static u8  g_u8PQBinDisplayType[E_PQ_MAX_WINDOW];
static u8  g_u8PQBinPnlIdx[E_PQ_MAX_WINDOW];
static u16 g_u16IccCrdTable[E_PQ_MAX_WINDOW];
static u16 g_u16IhcCrdTable[E_PQ_MAX_WINDOW];
u8 g_u8PkSramEn[HAL_DISP_DEVICE_MAX];
//-------------------------------------------------------------------------------------------------
//  Function
//-------------------------------------------------------------------------------------------------
void DrvDispPQBinClearSRAMTableIndex(void)
{
    u8 i;

    for (i = 0; i < E_PQ_MAX_WINDOW; i++)
    {
        g_u16IccCrdTable[i] = 0xFFFF;
        g_u16IhcCrdTable[i] = 0xFFFF;
    }
}

E_PQ_WIN_TYPE_e DrvDispPQBinTransToPQWin(u8 u8BinID)
{
    E_PQ_WIN_TYPE_e ePQWin;
    if (u8BinID == E_PQ_BIN_STD_MAIN || u8BinID == E_PQ_BIN_EXT_MAIN)
    {
        ePQWin = E_PQ_MAIN_WINDOW;
    }
    else
    {
        ePQWin = E_PQ_MAIN_WINDOW;
        DISP_ERR("%s %d: Error ID %d\n", __FUNCTION__, __LINE__, u8BinID);
    }

    return ePQWin;
}

u8 DrvDispPQBinGetByteData(void *pAddrVirt, u32 *u32Offset)
{
    u8 u8ret   = *((u8 *)pAddrVirt);
    *u32Offset = *u32Offset + 1;

    return u8ret;
}

u16 DrvDispPQBinGet2ByteData(void *pAddrVirt, u32 *u32Offset)
{
    u16 u16ret = (((u16) * ((u8 *)pAddrVirt + 0)) << 0) | (((u16) * ((u8 *)pAddrVirt + 1)) << 8);

    *u32Offset = *u32Offset + 2;

    return u16ret;
}

u32 DrvDispPQBinGet4ByteData(void *pAddrVirt, u32 *u32Offset)
{
    u32 u32ret = (((u32) * ((u8 *)pAddrVirt + 0)) << 0) | (((u32) * ((u8 *)pAddrVirt + 1)) << 8)
                 | (((u32) * ((u8 *)pAddrVirt + 2)) << 16) | (((u32) * ((u8 *)pAddrVirt + 3)) << 24);

    *u32Offset = *u32Offset + 4;

    return u32ret;
}

static void _DrvDispPQBinWriteByteMsk(u32 u32RegAddr, u8 u8Value, u8 u8Mask, u32 u32Dev)
{
    u32 u32BankOffset = HAL_DISP_GET_BANKOFFSET(u32Dev, u32RegAddr);
    u8  u8Support     = HAL_DISP_GET_APPLY(u32Dev, u32RegAddr);

    if (u8Support)
    {
        if(HAL_DISP_IS_PKSRAM_REG(u32Dev, u32RegAddr, u8Mask))
        {
            g_u8PkSramEn[u32Dev] = ((u8Value & u8Mask) & mask_of_vps_reg_vps_sram_act) ? 1 : 0;
            DISP_DBG(DISP_DBG_LEVEL_HPQREG, "[%s]: [addr=%x, msk=%hhx, val=%hhx] BankOffset = %x Dev:%d PkSramEn:%hhd\n",
                     __FUNCTION__, u32RegAddr, u8Mask, u8Value, u32BankOffset, u32Dev, g_u8PkSramEn[u32Dev]);
        }
        u32RegAddr += u32BankOffset;
        DISP_PQ_REG_FUNC(u32RegAddr, u8Value, u8Mask);
    }
    DISP_DBG(DISP_DBG_LEVEL_HPQREG, "[%s]: [addr=%x, msk=%hhx, val=%hhx] BankOffset = %x Dev:%d Support:%hhd\n",
             __FUNCTION__, u32RegAddr, u8Mask, u8Value, u32BankOffset, u32Dev, u8Support);
}

void DrvDispPQBinDumpGeneralRegTable(const DrvPQBinIPTableInfo_t *const pTabInfo)
{
    u8    u8Bank;
    u8    u8Addr;
    u8    u8Mask;
    u8    u8Value;
    u16   i;
    u32   u32Offset;
    u32   u32RegAddr;
    void *pvAddr;

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "%s:%d tab: general\n", __FUNCTION__, __LINE__);
    if (pTabInfo->u16GroupIdx >= pTabInfo->u16GroupNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "%s:%d IP_Info error: General Reg Table\n", __FUNCTION__, __LINE__);
        return;
    }

    pvAddr = pTabInfo->pvAddOffAddr;
    for (i = 0; i < pTabInfo->u16RegNum; i++)
    {
        u32Offset = (PQ_BIN_BANK_SIZE + PQ_BIN_ADDR_SIZE + PQ_BIN_MASK_SIZE + (u32)pTabInfo->u16GroupNum) * (u32)i;
        u8Bank    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Addr    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Mask    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);

        u32Offset += pTabInfo->u16GroupIdx;
        u8Value = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        DISP_DBG(DISP_DBG_LEVEL_HPQHL,
                 "4  [read addr=%x], GroupIdx=%x,RegNum=%x,reg=%04x, msk=%02x, val=%02x, u8CurBank=%x\n",
                 (u8Addr + u32Offset - (u32)pTabInfo->u16GroupIdx), pTabInfo->u16GroupIdx, pTabInfo->u16RegNum,
                 BK_SCALER_BASE | (((u16)u8Bank) << 8) | (u16)u8Addr, u8Mask, u8Value, u8Bank);

        u32RegAddr = BK_SCALER_BASE | (((u16)u8Bank) << 8) | (u16)u8Addr;
        _DrvDispPQBinWriteByteMsk(u32RegAddr, u8Value, u8Mask, pTabInfo->u32DevId);
    }
}

void DrvDispPQBinDumpScalerRegTable(const DrvPQBinIPTableInfo_t *const pTabInfo)
{
    u32   u32RegAddr;
    u8    u8Mask;
    u8    u8Addr;
    u8    u8Value;
    u8    u8CurBank = 0xff;
    u32   u32Offset;
    void *pvAddr;
    u16   i;

    // consider dump reg amount is very large, so we shouldn't use MApi_XC_W2BYTE(),
    //   because mutex cost much time.

    if (pTabInfo->u16GroupIdx >= pTabInfo->u16GroupNum)
    {
        DISP_ERR("[PQBin]IP_Info error: Scaler Reg Table\n");
        return;
    }

    pvAddr = pTabInfo->pvAddOffAddr;
    for (i = 0; i < pTabInfo->u16RegNum; i++)
    {
        u32Offset = (PQ_BIN_BANK_SIZE + PQ_BIN_ADDR_SIZE + PQ_BIN_MASK_SIZE + (u32)pTabInfo->u16GroupNum) * (u32)i;
        u8CurBank = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Addr    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Mask    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);

        u32Offset += (u32)pTabInfo->u16GroupIdx;
        u8Value = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        DISP_DBG(DISP_DBG_LEVEL_HPQHL,
                 "[%s]: read addr=%px, GroupIdx=%x,RegNum=%x,reg=%04x, msk=%02x, val=%02x, u8CurBank=%x\n",
                 __FUNCTION__, (pvAddr + u32Offset - (u32)pTabInfo->u16GroupIdx), pTabInfo->u16GroupIdx,
                 pTabInfo->u16RegNum, BK_SCALER_BASE | (u32)u8Addr, u8Mask, u8Value, u8CurBank);

        u32RegAddr = BK_SCALER_BASE | (((u32)u8CurBank) << 8) | (u32)u8Addr;
        _DrvDispPQBinWriteByteMsk(u32RegAddr, u8Value, u8Mask, pTabInfo->u32DevId);
    }
}

void DrvDispPQBinDumpFilterTable(DrvPQBinIPTableInfo_t *pTabInfo)
{
    void *pvAddr;
    if (pTabInfo->u16GroupIdx >= pTabInfo->u16GroupNum)
    {
        DISP_ERR("[PQBin]IP_Info error: SRAM Table\n");
        return;
    }

    pvAddr = pTabInfo->pvAddOffAddr + pTabInfo->u16GroupIdx * pTabInfo->u16RegNum;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "PQ Filter Table: Win:%d, Type:%d \n", pTabInfo->ePQWin, pTabInfo->u16TableType);
    switch (pTabInfo->u16TableType)
    {
        case E_PQ_TABTYPE_VIP_ICC_CRD_SRAM:
            if (g_u16IhcCrdTable[pTabInfo->ePQWin] != pTabInfo->u16GroupIdx)
            {
                DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]old IHC_CRD_SRAM: %u, new IHC_CRD_SRAM: %u\n",
                         g_u16IhcCrdTable[pTabInfo->ePQWin], pTabInfo->u16GroupIdx);
                g_u16IhcCrdTable[pTabInfo->ePQWin] = pTabInfo->u16GroupIdx;
                HalPQSetSramIccCrdTable(pTabInfo->u32DevId, SC_FILTER_SRAM_ICC_CRD, pvAddr);
            }
            break;
        case E_PQ_TABTYPE_VIP_IHC_CRD_SRAM:
            if (g_u16IccCrdTable[pTabInfo->ePQWin] != pTabInfo->u16GroupIdx)
            {
                DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]old ICC_CRD_SRAM: %u, new ICC_CRD_SRAM: %u\n",
                         g_u16IccCrdTable[pTabInfo->ePQWin], pTabInfo->u16GroupIdx);
                g_u16IccCrdTable[pTabInfo->ePQWin] = pTabInfo->u16GroupIdx;
                HalPQSetSramIhcCrdTable(pTabInfo->u32DevId, SC_FILTER_SRAM_IHC_CRD, pvAddr);
            }
            break;

        default:
            DISP_ERR("[PQBin] Unknown sram type %u\n", pTabInfo->u16TableType);
            break;
    }
}

void DrvDispPQBinDumpTable(DrvPQBinIPTableInfo_t *pTabInfo)
{
    // to save loading SRAM table time, SRAM are only downloaded
    // when current SRAM table is different to previous SRAM table

    if ((pTabInfo->u16RegNum == 0) || (pTabInfo->pvAddOffAddr == PQ_BIN_ADDR_NULL))
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[%s]NULL Table\n", __FUNCTION__);
        return;
    }
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[%s] TableType:%d \n", __FUNCTION__, pTabInfo->u16TableType);
    switch (pTabInfo->u16TableType)
    {
        case E_PQ_TABTYPE_SCALER:
            DrvDispPQBinDumpScalerRegTable(pTabInfo);
            break;
        case E_PQ_TABTYPE_GENERAL:
            DrvDispPQBinDumpGeneralRegTable(pTabInfo);
            break;
        case E_PQ_TABTYPE_VIP_IHC_CRD_SRAM:
        case E_PQ_TABTYPE_VIP_ICC_CRD_SRAM:
            DrvDispPQBinDumpFilterTable(pTabInfo);
            break;
        default:
            DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[%s]DumpTable:unknown type: %u\n", __FUNCTION__, pTabInfo->u16TableType);
            break;
    }
}

// return total IP count
u16 DrvDispPQBinGetIPNum(DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[%s]:IPNum=%u\n", __FUNCTION__, pPQBinHeader->u16IP_Comm_Num);
    return pPQBinHeader->u16IP_Comm_Num;
}

u16 DrvDispPQBinGetTableIndex(u16 u16PQSrcType, u16 u16PQIPIdx, u16 u16PQPnlIdx, DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    void *pvAddr;
    u32   u32Offset;
    void *pvSourceLUTAddr;
    u32   u32SourceLUTOffset;
    u16   u16SourceLUTSourceNum;
    u16   u16SourceLUTIPNum;
    u32   u32SourceLUTDataPitch;
    u16   u16TableIdx;

    if (u16PQPnlIdx >= pPQBinHeader->u16SourceLUT_PnlNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]invalid panel type\n");
        return PQ_BIN_IP_NULL;
    }

    pvAddr                = (pPQBinHeader->pvBinStartAddress + pPQBinHeader->u32SourceLUT_Offset);
    u32Offset             = pPQBinHeader->u32SourceLUT_Pitch * u16PQPnlIdx + PQ_BIN_SOURCELUT_SOURCE_NUM_OFFSET;
    u16SourceLUTSourceNum = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);
    if (u16PQSrcType >= u16SourceLUTSourceNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]invalid input type\n");
        return PQ_BIN_IP_NULL;
    }

    u32Offset         = pPQBinHeader->u32SourceLUT_Pitch * u16PQPnlIdx + PQ_BIN_SOURCELUT_IP_NUM_OFFSET;
    u16SourceLUTIPNum = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);
    if (u16PQIPIdx >= u16SourceLUTIPNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]invalid ip type\n");
        return PQ_BIN_IP_NULL;
    }

    u32Offset       = pPQBinHeader->u32SourceLUT_Pitch * u16PQPnlIdx + PQ_BIN_SOURCELUT_OFFSET_OFFSET;
    pvSourceLUTAddr = pPQBinHeader->pvBinStartAddress + PQ_BIN_HEADER_LEN
                      + DrvDispPQBinGet4ByteData(((pvAddr + u32Offset)), &u32Offset);

    u32SourceLUTDataPitch = (u32)u16SourceLUTIPNum * 2;
    u32SourceLUTOffset    = u32SourceLUTDataPitch * (u32)u16PQSrcType + (u32)u16PQIPIdx * 2;

    u16TableIdx = DrvDispPQBinGet2ByteData((pvSourceLUTAddr + u32SourceLUTOffset), &u32SourceLUTOffset);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]%s %d: SrcType:%u, IpId:%d, PnlId:%d, TableId=%u\n", __FUNCTION__, __LINE__,
             u16PQSrcType, u16PQIPIdx, u16PQPnlIdx, u16TableIdx);
    return u16TableIdx;
}

void DrvDispPQBinGetTable(u16 u16PnlIdx, u16 u16TabIdx, u16 u16PQIPIdx, DrvPQBinIPTableInfo_t *pTableInfo,
                          DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    u32   u32Offset;
    void *pvAddr;

    E_PQ_WIN_TYPE_e ePQWin = DrvDispPQBinTransToPQWin(pPQBinHeader->u8BinID);
    if ((u16PnlIdx >= PQ_BIN_MAX_PNL) || (u16PQIPIdx >= PQ_BIN_MAX_IP))
    {
        DISP_ERR("[PQBin]:Out Of Range!! PnlIdx=%u, IPIdx=%u\n", u16PnlIdx, u16PQIPIdx);
        return;
    }
    g_u16PQTabIdx[ePQWin][u16PnlIdx][u16PQIPIdx] = u16TabIdx;

    if (u16TabIdx != PQ_BIN_IP_NULL)
    {
        pvAddr = (pPQBinHeader->u32IP_Comm_Offset + pPQBinHeader->pvBinStartAddress);

#if 1
        if (u16TabIdx == PQ_BIN_IP_COMM)
        {
            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET;
            pTableInfo->u16TableType = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);

            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_COMM_REG_NUM_OFFSET;
            pTableInfo->u16RegNum = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);

            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_COMMOFFSET_OFFSET;
            pTableInfo->u32TabOffset = DrvDispPQBinGet4ByteData(((pvAddr + u32Offset)), &u32Offset);
            pTableInfo->pvAddOffAddr =
                (pTableInfo->u32TabOffset + (pPQBinHeader->pvBinStartAddress + PQ_BIN_HEADER_LEN));

            pTableInfo->u16GroupNum = 1;
            pTableInfo->u16GroupIdx = 0;
        }
        else
#endif
        {
            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET;
            pTableInfo->u16TableType = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);

            u32Offset             = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_IP_REG_NUM_OFFSET;
            pTableInfo->u16RegNum = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);

            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_IP_GROUP_NUM_OFFSET;
            pTableInfo->u16GroupNum = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);

            u32Offset = pPQBinHeader->u32IP_Comm_Pitch * (u32)u16PQIPIdx + PQ_BIN_IP_COMM_IPOFFSET_OFFSET;
            pTableInfo->u32TabOffset = DrvDispPQBinGet4ByteData(((pvAddr + u32Offset)), &u32Offset);
            pTableInfo->pvAddOffAddr =
                (pTableInfo->u32TabOffset + (pPQBinHeader->pvBinStartAddress + PQ_BIN_HEADER_LEN));

            pTableInfo->u16GroupIdx = u16TabIdx;
            pTableInfo->u32DevId    = pPQBinHeader->u32DevID;
        }
    }
    else
    {
        pTableInfo->u16GroupIdx  = 0;
        pTableInfo->pvAddOffAddr = PQ_BIN_ADDR_NULL;
        pTableInfo->u16RegNum    = 0;
        pTableInfo->u16TableType = 0;
        pTableInfo->u16GroupNum  = 0;
    }

    pTableInfo->ePQWin = DrvDispPQBinTransToPQWin(pPQBinHeader->u8BinID);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL,
             "[PQBin]: %s: Offset =%x addr=%px, RegNum=%u, GroupIdx=%u, GroupNum=%u, Type=%u eWin=%d\n", __FUNCTION__,
             pTableInfo->u32TabOffset, pTableInfo->pvAddOffAddr, pTableInfo->u16RegNum, pTableInfo->u16GroupIdx,
             pTableInfo->u16GroupNum, pTableInfo->u16TableType, pTableInfo->ePQWin);
}

void DrvDispPQBinLoadTableData(u16 u16PnlIdx, u16 u16TabIdx, u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader,
                               u8 *pTable, u16 u16TableSize)
{
    DrvPQBinIPTableInfo_t stTableInfo;
    u8                    u8Mask    = 0;
    u8                    u8Addr    = 0;
    u8                    u8Value   = 0;
    u8                    u8CurBank = 0xff;
    u32                   u32Offset = 0;
    void *                pvAddr    = 0;
    u16                   i         = 0;

    UNUSED(u8CurBank);
    UNUSED(u8Addr);
    UNUSED(u8Mask);

    memset(&stTableInfo, 0, sizeof(DrvPQBinIPTableInfo_t));

    DrvDispPQBinGetTable(u16PnlIdx, u16TabIdx, u16PQIPIdx, &stTableInfo, pPQBinHeader);

    if (stTableInfo.u16GroupIdx >= stTableInfo.u16GroupNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]IP_Info error: LoadTable Data\n");
        return;
    }

    if (u16TableSize < stTableInfo.u16RegNum)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[PQBin]IP_Info error: LoadTable Data is too samall, %d %d\n", u16TableSize,
                 stTableInfo.u16RegNum);
        return;
    }
    pvAddr = (stTableInfo.pvAddOffAddr);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "%s: RegNum:%d\n", __FUNCTION__, stTableInfo.u16RegNum);
    for (i = 0; i < stTableInfo.u16RegNum; i++)
    {
        u32Offset = (PQ_BIN_BANK_SIZE + PQ_BIN_ADDR_SIZE + PQ_BIN_MASK_SIZE + (u32)stTableInfo.u16GroupNum) * (u32)i;
        u8CurBank = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Addr    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);
        u8Mask    = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);

        u32Offset += (u32)stTableInfo.u16GroupIdx;
        u8Value = DrvDispPQBinGetByteData(((pvAddr + u32Offset)), &u32Offset);

        pTable[i] = u8Value;
    }
}

void DrvDispPQBinLoadTable(u16 u16PnlIdx, u16 u16TabIdx, u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    DrvPQBinIPTableInfo_t stTableInfo;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "---------DrvDispPQBinLoadTable: u16TabIdx=%u, u16PQIPIdx=%u\n", u16TabIdx,
             u16PQIPIdx);
    DrvDispPQBinGetTable(u16PnlIdx, u16TabIdx, u16PQIPIdx, &stTableInfo, pPQBinHeader);

    DrvDispPQBinDumpTable(&stTableInfo);
}

void DrvDispPQBinLoadCommTable(u16 u16PnlIdx, DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    u16                   i;
    u32                   u32DevId;
    DrvPQBinIPTableInfo_t stTableInfo;

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "[%s]%d\n", __FUNCTION__, __LINE__);
    for (i = 0; i < pPQBinHeader->u16IP_Comm_Num; i++)
    {
        DISP_DBG(DISP_DBG_LEVEL_HPQHL, "--------%s:PQIPIdx(column)=%u, Total=%u\n", __FUNCTION__, i,
                 pPQBinHeader->u16IP_Comm_Num);
        DrvDispPQBinGetTable(u16PnlIdx, PQ_BIN_IP_COMM, i, &stTableInfo, pPQBinHeader);

        if (((stTableInfo.u16TableType >= E_PQ_TABTYPE_SRAM1) && (stTableInfo.u16TableType <= E_PQ_TABTYPE_C_SRAM1))
            || (stTableInfo.u16TableType == E_PQ_TABTYPE_VIP_IHC_CRD_SRAM)
            || (stTableInfo.u16TableType == E_PQ_TABTYPE_VIP_ICC_CRD_SRAM))
        {
            continue;
        }
        for (u32DevId = HAL_DISP_DEVICE_ID_0; u32DevId < HAL_DISP_DEVICE_MAX; u32DevId++)
        {
            stTableInfo.u32DevId = u32DevId;
            DrvDispPQBinDumpTable(&stTableInfo);
        }
    }
}

void DrvDispPQBinParsing(DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    void *pvPQBinAddr;
    u32   u32Offset;
    u8    i;

    { // for sclprintf
        pPQBinHeader->u8Header[PQ_BIN_HEADER_LEN]           = '\0';
        pPQBinHeader->u8Version[PQ_BIN_VERSION_LEN]         = '\0';
        pPQBinHeader->u8Dummy[PQ_BIN_DUMMY]                 = '\0';
        pPQBinHeader->u8GRuleHader[PQ_BIN_HEADER_LEN]       = '\0';
        pPQBinHeader->u8XRuleHader[PQ_BIN_HEADER_LEN]       = '\0';
        pPQBinHeader->u8SkipRuleHader[PQ_BIN_HEADER_LEN]    = '\0';
        pPQBinHeader->u8IP_Comm_Header[PQ_BIN_HEADER_LEN]   = '\0';
        pPQBinHeader->u8SourceLUT_Header[PQ_BIN_HEADER_LEN] = '\0';
    }

    u32Offset   = 0;
    pvPQBinAddr = pPQBinHeader->pvBinStartAddress;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "\n %s :BinStartaDDR ID%u=%px \n", __FUNCTION__, pPQBinHeader->u8BinID, pvPQBinAddr);
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8Header[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8Version[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }
    pPQBinHeader->u32StartOffset = DrvDispPQBinGet4ByteData((pvPQBinAddr + u32Offset), &u32Offset);
    pPQBinHeader->u32EndOffset   = DrvDispPQBinGet4ByteData((pvPQBinAddr + u32Offset), &u32Offset);

    for (i = 0; i < PQ_BIN_DUMMY; i++)
    {
        pPQBinHeader->u8Dummy[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    pPQBinHeader->u8PQID = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "Header :%s \n", pPQBinHeader->u8Header);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "Version:%s \n", pPQBinHeader->u8Version);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "startADDR=%x, EndAddr=%x \n", pPQBinHeader->u32StartOffset,
             pPQBinHeader->u32EndOffset);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "Dummy :%s \n", pPQBinHeader->u8Dummy);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "PQID :%d \n", pPQBinHeader->u8PQID);
    //-------------------------------------------------------------------------------------------
    // GRule
    //
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8GRuleHader[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    pPQBinHeader->u16GRule_RuleNum = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);
    pPQBinHeader->u16GRule_PnlNum  = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "GRuleHeader:%s\n", pPQBinHeader->u8GRuleHader);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "GRule: RuleNum=%u, PnlNum=%u \n", pPQBinHeader->u16GRule_RuleNum,
             pPQBinHeader->u16GRule_PnlNum);

    pPQBinHeader->u32GRule_Offset = u32Offset;
    pPQBinHeader->u32GRule_Pitch  = PQ_BIN_GRULE_INFO_SIZE;
    u32Offset += pPQBinHeader->u32GRule_Pitch * (u32)pPQBinHeader->u16GRule_RuleNum;

    pPQBinHeader->u32GRule_Lvl_Offset = u32Offset;
    pPQBinHeader->u32GRule_Lvl_Pitch  = PQ_BIN_GRULE_LEVEL_INFO_SIZE;
    u32Offset +=
        pPQBinHeader->u32GRule_Lvl_Pitch * (u32)pPQBinHeader->u16GRule_RuleNum * (u32)pPQBinHeader->u16GRule_PnlNum;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "GRule Rule : offset=%x, pitch=%u \n", pPQBinHeader->u32GRule_Offset,
             pPQBinHeader->u32GRule_Pitch);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "GRule Level: offset=%x, pitch=%u \n", pPQBinHeader->u32GRule_Lvl_Offset,
             pPQBinHeader->u32GRule_Lvl_Pitch);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "XRule Start:%x\n", u32Offset);

    //-------------------------------------------------------------------------------------------
    // XRule
    //
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8XRuleHader[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }
    pPQBinHeader->u16XRuleNum = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "XRuleHeader:%s\n", pPQBinHeader->u8XRuleHader);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "XRule: RuleNum=%u\n", pPQBinHeader->u16XRuleNum);
    pPQBinHeader->u32XRule_Offset = u32Offset;
    pPQBinHeader->u32XRUle_Pitch  = PQ_BIN_XRULE_INFO_SIZE;
    u32Offset += pPQBinHeader->u32XRUle_Pitch * (u32)pPQBinHeader->u16XRuleNum;

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "XRule: offset=%x, pitch=%u\n", pPQBinHeader->u32XRule_Offset,
             pPQBinHeader->u32XRUle_Pitch);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SkipRule Start:%x\n", u32Offset);
    //-------------------------------------------------------------------------------------------
    // SkipRule
    //
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8SkipRuleHader[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    pPQBinHeader->u16SkipRule_IPNum  = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);
    pPQBinHeader->u32SkipRule_Offset = DrvDispPQBinGet4ByteData((pvPQBinAddr + u32Offset), &u32Offset);

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SkipRuleHeader:%s IpNum=%u, Offset=%x\n", pPQBinHeader->u8SkipRuleHader,
             pPQBinHeader->u16SkipRule_IPNum, pPQBinHeader->u32SkipRule_Offset);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "IP_Comm Start:%x\n", u32Offset);
    //-------------------------------------------------------------------------------------------
    // IP & Common
    //
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8IP_Comm_Header[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    pPQBinHeader->u16IP_Comm_Num = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);

    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "IP_CommHeader:%s\n", pPQBinHeader->u8IP_Comm_Header);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "IP_Comm, Num=%u\n", pPQBinHeader->u16IP_Comm_Num);
    pPQBinHeader->u32IP_Comm_Offset = u32Offset;
    pPQBinHeader->u32IP_Comm_Pitch  = PQ_BIN_IP_COMM_INFO_SIZE;
    u32Offset += pPQBinHeader->u32IP_Comm_Pitch * (u32)pPQBinHeader->u16IP_Comm_Num;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "IP_Comm, Offset=%x, pitch=%u\n", pPQBinHeader->u32IP_Comm_Offset,
             pPQBinHeader->u32IP_Comm_Pitch);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SourceLUT Start:%x\n", u32Offset);
    //-------------------------------------------------------------------------------------------
    // SourceLUT
    //
    for (i = 0; i < PQ_BIN_HEADER_LEN; i++)
    {
        pPQBinHeader->u8SourceLUT_Header[i] = DrvDispPQBinGetByteData((pvPQBinAddr + u32Offset), &u32Offset);
    }

    pPQBinHeader->u16SourceLUT_PnlNum = DrvDispPQBinGet2ByteData((pvPQBinAddr + u32Offset), &u32Offset);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SourceLUTHeader:%s\n", pPQBinHeader->u8SourceLUT_Header);
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SourceLUT, PnlNum=%u\n", pPQBinHeader->u16SourceLUT_PnlNum);

    pPQBinHeader->u32SourceLUT_Offset = u32Offset;
    pPQBinHeader->u32SourceLUT_Pitch  = PQ_BIN_SOURCELUT_INFO_SIZE;
    u32Offset += pPQBinHeader->u32SourceLUT_Pitch * (u32)pPQBinHeader->u16SourceLUT_PnlNum;
    DISP_DBG(DISP_DBG_LEVEL_HPQHL, "SourceLUT: offset=%x, pitch=%u\n", pPQBinHeader->u32SourceLUT_Offset,
             pPQBinHeader->u32SourceLUT_Pitch);
}

u16 DrvDispPQBinGetSourceLutPnlNum(DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    return pPQBinHeader->u16SourceLUT_PnlNum;
}

void DrvDispPQBinGetSourceLut(DrvPQBinHeaderInfo_t *pPQBinHeader, u16 u16SourceLutPnlIdx,
                              DrvPQBinSourceLUTInfo_t *pSrcLutInfo)
{
    u32 u32tmpOffset;

    u32tmpOffset = pPQBinHeader->u32SourceLUT_Offset + (u16SourceLutPnlIdx * PQ_BIN_SOURCELUT_INFO_SIZE);

    pSrcLutInfo->u16SourceNum =
        DrvDispPQBinGet2ByteData((pPQBinHeader->pvBinStartAddress + u32tmpOffset), &u32tmpOffset);
    pSrcLutInfo->u16IPNum  = DrvDispPQBinGet2ByteData((pPQBinHeader->pvBinStartAddress + u32tmpOffset), &u32tmpOffset);
    pSrcLutInfo->u32Offset = DrvDispPQBinGet4ByteData((pPQBinHeader->pvBinStartAddress + u32tmpOffset), &u32tmpOffset);
    DISP_DBG(DISP_DBG_LEVEL_HPQ,"%s:SourceLUT %u, SourceNum=%u, IPNum=%u, Offset=%x\n", __FUNCTION__, u16SourceLutPnlIdx,
             pSrcLutInfo->u16SourceNum, pSrcLutInfo->u16IPNum, pSrcLutInfo->u32Offset);
}

u16 DrvDispPQBinGetSkipRule(u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    void *pvAddr;
    u32   u32Offset;
    u16   u16SkipRet;

    pvAddr = (pPQBinHeader->u32SkipRule_Offset + pPQBinHeader->pvBinStartAddress + PQ_BIN_SKIPRULE_HEADER_LEN);

    u32Offset  = u16PQIPIdx * 2;
    u16SkipRet = DrvDispPQBinGet2ByteData(((pvAddr + u32Offset)), &u32Offset);
    return u16SkipRet;
}

void DrvDispPQBinLoadTableBySrcType(u16 u16PQSrcType, u16 u16PQIPIdx, u16 u16PQPnlIdx,
                                    DrvPQBinHeaderInfo_t *pPQBinHeader)
{
    u16                   u16QMIPtypeSize;
    u16                   u16i;
    u16                   u16TabIdx;
    DrvPQBinIPTableInfo_t stTableInfo;
    // XC_ApiStatusEx stXCStatusEx; Ryan

    if (u16PQIPIdx == PQ_BIN_IP_ALL)
    {
        u16QMIPtypeSize = DrvDispPQBinGetIPNum(pPQBinHeader);
        u16PQIPIdx      = 0;
    }
    else
    {
        u16QMIPtypeSize = 1;
    }
    DISP_DBG(DISP_DBG_LEVEL_HPQ, "[%s]u16QMIPtypeSize=%d \n", __FUNCTION__, u16QMIPtypeSize);
    for (u16i = 0; u16i < u16QMIPtypeSize; u16i++, u16PQIPIdx++)
    {
        if (DrvDispPQBinGetSkipRule(u16PQIPIdx, pPQBinHeader))
        {
            DISP_DBG(DISP_DBG_LEVEL_HPQHL, "skip ip idx:%u\n", u16PQIPIdx);
            continue;
        }

        u16TabIdx = DrvDispPQBinGetTableIndex(u16PQSrcType, u16PQIPIdx, u16PQPnlIdx, pPQBinHeader);
        DISP_DBG(DISP_DBG_LEVEL_HPQHL,
                 "----------%s:u16TabIdx=%u, u16PQIPIdx=%u SrcType=%u, IPIdx=%u, PnlIdx:%u, TabIdx=%u\n", __FUNCTION__,
                 u16TabIdx, u16PQIPIdx, u16PQSrcType, u16PQIPIdx, u16PQPnlIdx, u16TabIdx);

        DrvDispPQBinGetTable(u16PQPnlIdx, u16TabIdx, u16PQIPIdx, &stTableInfo, pPQBinHeader);

        DrvDispPQBinDumpTable(&stTableInfo);
    }
}

void DrvDispPQBinSetDisplayType(PQ_BIN_DISPLAY_TYPE_e enDisplayType, E_PQ_WIN_TYPE_e ePQWin)
{
    g_u8PQBinDisplayType[ePQWin] = enDisplayType;
}

PQ_BIN_DISPLAY_TYPE_e DrvDispPQBinGetDisplayType(E_PQ_WIN_TYPE_e ePQWin)
{
    return (PQ_BIN_DISPLAY_TYPE_e)g_u8PQBinDisplayType[ePQWin];
}

void DrvDispPQBinSetPanelID(u8 u8PnlIDx, E_PQ_WIN_TYPE_e ePQWin)
{
    g_u8PQBinPnlIdx[ePQWin] = u8PnlIDx;
}

u8 DrvDispPQBinGetPanelIdx(E_PQ_WIN_TYPE_e ePQWin)
{
    return g_u8PQBinPnlIdx[ePQWin] * 1 + g_u8PQBinDisplayType[ePQWin];
}
