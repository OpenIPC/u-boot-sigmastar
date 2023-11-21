/*
* hal_disp_hpq_main.c- Sigmastar
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

#define MHAL_PQ_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_disp_os_header.h"
#include "drv_disp_hpq_os.h"

#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "hal_scl_pq_hw_reg.h"
#include "drv_scl_pq_define.h"

//[Daniel]

#include "drv_scl_pq_data_types.h"
#include "hal_scl_pq.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifndef UNUSED // to avoid compile warnings...
#define UNUSED(var) (void)((var) = (var))
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
#define ICC_H_SIZE 32
#define ICC_V_SIZE 32
void HalPQSetSramIccCrdTable(u32 u32DevId, u8 u8sramtype, void *pData)
{
    u16   i;
    u16   u16RamcodeL, u16RamcodeH;
    u16   u16Ramcode;
    void *pvAddr;
    u32   u32BankOffset = HAL_DISP_HPQ_BANKOFFSET(u32DevId);

    if (u8sramtype != SC_FILTER_SRAM_ICC_CRD)
    {
        DISP_ERR("%s %d, Type(%d) is Not Correct\n", u8sramtype);
        return;
    }

    if (!HAL_DISP_DEV_HPQ_SUPPORT(u32DevId))
    {
        DISP_DBG(DISP_DBG_LEVEL_MACE, "%s %d, u32DevId(%d) is Not Correct\n", __FUNCTION__, __LINE__, u32DevId);
        return;
    }

    pvAddr = pData;

    W2BYTEMSK(ACE2_REG_ICC_SRAM_IO_EN + u32BankOffset, mask_of_ace2_reg_icc_sram_io_en,
              mask_of_ace2_reg_icc_sram_io_en);

    for (i = 0; i < (ICC_H_SIZE * ICC_V_SIZE); i++)
    {
        u16RamcodeL = *((u8 *)(pvAddr + (2 * i)));
        u16RamcodeH = *((u8 *)(pvAddr + (2 * i + 1)));
        u16Ramcode  = ((u16RamcodeH) << 8) + u16RamcodeL;

        // while (MApi_XC_R2BYTE(u32BK18_7A) & BIT(15));
        W2BYTEMSK(ACE2_REG_ICC_IOADDR + u32BankOffset, i, mask_of_ace2_reg_icc_ioaddr);
        W2BYTEMSK(ACE2_REG_ICC_IOWDATA + u32BankOffset, u16Ramcode, mask_of_ace2_reg_icc_iowdata);
        W2BYTEMSK(ACE2_REG_ICC_IOWE + u32BankOffset, mask_of_ace2_reg_icc_iowe, mask_of_ace2_reg_icc_iowe);
    }

    W2BYTEMSK(ACE2_REG_ICC_SRAM_IO_EN + u32BankOffset, 0, mask_of_ace2_reg_icc_sram_io_en); // io_en enable
}

#define IHC_H_SIZE    33
#define IHC_V_SIZE    33
#define BUF_BYTE_SIZE (IHC_H_SIZE * IHC_V_SIZE)
#define MAX_SRAM_SIZE 0x124

#define SRAM1_IHC_COUNT 289 // 81
#define SRAM2_IHC_COUNT 272 // 72
#define SRAM3_IHC_COUNT 272 // 72
#define SRAM4_IHC_COUNT 256 // 64

#define SRAM1_IHC_OFFSET 0
#define SRAM2_IHC_OFFSET SRAM1_IHC_COUNT
#define SRAM3_IHC_OFFSET (SRAM2_IHC_OFFSET + SRAM2_IHC_COUNT)
#define SRAM4_IHC_OFFSET (SRAM3_IHC_OFFSET + SRAM3_IHC_COUNT)

#define SRAM_IHC_TOTAL_COUNT SRAM1_IHC_COUNT + SRAM2_IHC_COUNT + SRAM3_IHC_COUNT + SRAM4_IHC_COUNT

static void _HalPQSetSramIhcCrdTable(u32 u32DevId, u16 *pBuf, u8 u8SRAM_Idx, u16 u16Cnt)
{
    u16 i;
    u32 u32BankOffset = HAL_DISP_HPQ_BANKOFFSET(u32DevId);

    if (u8SRAM_Idx > 3)
    {
        u8SRAM_Idx = 0;
    }

    W2BYTEMSK(ACE2_REG_IHC_SRAM_IO_EN + u32BankOffset, mask_of_ace2_reg_ihc_sram_io_en,
              mask_of_ace2_reg_ihc_sram_io_en);
    W2BYTEMSK(ACE2_REG_IHC_SRAM_IO_SELECT + u32BankOffset, u8SRAM_Idx << shift_of_ace2_reg_ihc_sram_io_select,
              mask_of_ace2_reg_ihc_sram_io_select);
    for (i = 0; i < u16Cnt; i++)
    {
        W2BYTEMSK(ACE2_REG_IHC_IOADDR + u32BankOffset, i, mask_of_ace2_reg_ihc_ioaddr);
        W2BYTEMSK(ACE2_REG_IHC_IOWDATA + u32BankOffset, pBuf[i], mask_of_ace2_reg_ihc_iowdata);
        W2BYTEMSK(ACE2_REG_IHC_IOWE + u32BankOffset, mask_of_ace2_reg_ihc_iowe, mask_of_ace2_reg_ihc_iowe);
    }
    W2BYTEMSK(ACE2_REG_IHC_SRAM_IO_EN + u32BankOffset, 0, mask_of_ace2_reg_ihc_sram_io_en); // io_en enable
}

u16 g_u16PQSram1Ihc[MAX_SRAM_SIZE];
u16 g_u16PQSram2Ihc[MAX_SRAM_SIZE];
u16 g_u16PQSram3Ihc[MAX_SRAM_SIZE];
u16 g_u16PQSram4Ihc[MAX_SRAM_SIZE];

void HalPQSetSramIhcCrdTable(u32 u32DevId, u8 u8sramtype, void *pData)
{
    // New method for download IHC
    u16 *u32Addr  = (u16 *)pData;
    u16  u16Index = 0;
    u16  cont1 = 0, cont2 = 0, cont3 = 0, cont4 = 0;

    if (u8sramtype != SC_FILTER_SRAM_IHC_CRD)
    {
        DISP_ERR("%s %d, Type(%d) is not correct\n", __FUNCTION__, __LINE__, u8sramtype);
        return;
    }
    if (!HAL_DISP_DEV_HPQ_SUPPORT(u32DevId))
    {
        DISP_DBG(DISP_DBG_LEVEL_MACE, "%s %d, u32DevId:%d is not correct\n", __FUNCTION__, __LINE__, u32DevId);
        return;
    }

    for (; u16Index < SRAM_IHC_TOTAL_COUNT; u16Index++)
    {
        if (u16Index < SRAM2_IHC_OFFSET)
        {
            g_u16PQSram1Ihc[cont1++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM3_IHC_OFFSET)
        {
            g_u16PQSram2Ihc[cont2++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM4_IHC_OFFSET)
        {
            g_u16PQSram3Ihc[cont3++] = u32Addr[u16Index];
        }
        else
        {
            g_u16PQSram4Ihc[cont4++] = u32Addr[u16Index];
        }
    }

    _HalPQSetSramIhcCrdTable(u32DevId, &g_u16PQSram1Ihc[0], 0, SRAM1_IHC_COUNT);
    _HalPQSetSramIhcCrdTable(u32DevId, &g_u16PQSram2Ihc[0], 1, SRAM2_IHC_COUNT);
    _HalPQSetSramIhcCrdTable(u32DevId, &g_u16PQSram3Ihc[0], 2, SRAM3_IHC_COUNT);
    _HalPQSetSramIhcCrdTable(u32DevId, &g_u16PQSram4Ihc[0], 3, SRAM4_IHC_COUNT);
}
