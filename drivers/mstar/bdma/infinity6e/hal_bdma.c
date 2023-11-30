/*
* hal_bdma.c- Sigmastar
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
// Include files
/*=============================================================*/
#include <common.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/io.h"
#include "kernel_bdma.h"
#include "hal_bdma.h"

////////////////////////////////////////////////////////////////////////////////
// Global variable
////////////////////////////////////////////////////////////////////////////////

#define BASE_REG_BDMA0_PA   0x1F200400
#define BASE_REG_BDMA1_PA   0x1F200480
#define BASE_REG_BDMA2_PA   0x1F200500
#define BASE_REG_BDMA3_PA   0x1F200580

#define CamOsPrintf  printf

volatile KeBdma_t * const g_ptKeBdma0 = (KeBdma_t *)IO_ADDRESS(BASE_REG_BDMA0_PA);
volatile KeBdma_t * const g_ptKeBdma1 = (KeBdma_t *)IO_ADDRESS(BASE_REG_BDMA1_PA);
volatile KeBdma_t * const g_ptKeBdma2 = (KeBdma_t *)IO_ADDRESS(BASE_REG_BDMA2_PA);
volatile KeBdma_t * const g_ptKeBdma3 = (KeBdma_t *)IO_ADDRESS(BASE_REG_BDMA3_PA);

/*=============================================================*/
// Local function definition
/*=============================================================*/

static u32 _HalBdmaVA2MiuA(void* virAddr)
{
#if 1
    return (u32)virAddr;
#else
    unsigned int phyAddr;

    phyAddr = (unsigned int)MsVA2PA(virAddr);

    return HalUtilPHY2MIUAddr(phyAddr);
#endif
}

/*=============================================================*/
// Global function definition
/*=============================================================*/
HalBdmaErr_e HalBdma_DoTransfer(u8 u8DmaCh, HalBdmaParam_t *ptBdmaParam)
{
    volatile KeBdma_t* g_ptKeBdma = g_ptKeBdma0;

    switch(u8DmaCh) {
    case HAL_BDMA_CH0:
        g_ptKeBdma = g_ptKeBdma0;
        break;
    case HAL_BDMA_CH1:
        g_ptKeBdma = g_ptKeBdma1;
        break;
    case HAL_BDMA_CH2:
        g_ptKeBdma = g_ptKeBdma2;
        break;
    case HAL_BDMA_CH3:
        g_ptKeBdma = g_ptKeBdma3;
        break;
    default:
        return HAL_BDMA_PROC_DONE;
        break;
    }

    g_ptKeBdma->reg_ch0_busy = 0x1;
    g_ptKeBdma->reg_ch0_int_bdma = 0x1;
    g_ptKeBdma->reg_ch0_done = 0x1;

    switch(ptBdmaParam->ePathSel)
    {
        case HAL_BDMA_MIU0_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU0 | REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU0_TO_MIU1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU0 | REG_BDMA_CH1_MIU1;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU1_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU1 | REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU1_TO_MIU1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU1 | REG_BDMA_CH1_MIU1;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU0_TO_IMI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU0 | REG_BDMA_CH1_IMI;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU1_TO_IMI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU1 | REG_BDMA_CH1_IMI;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_IMI_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_IMI | REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_IMI_TO_MIU1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_IMI | REG_BDMA_CH1_MIU1;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_IMI_TO_IMI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_IMI | REG_BDMA_CH1_IMI;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MEM_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MEM_FILL;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_4BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MEM_TO_MIU1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MEM_FILL;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU1;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_4BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MEM_TO_IMI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MEM_FILL;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_IMI;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_4BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_SPI_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_SPI;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_8BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_SPI_TO_MIU1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_SPI;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU1;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_8BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_SPI_TO_IMI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_SPI;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_IMI;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_8BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU0_TO_SPI:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SDT_FSP;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH0_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_4BYTE;
            break;
        case HAL_BDMA_MSPI0_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MSPI0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MSPI1_TO_MIU0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MSPI1;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU0_TO_MSPI0:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MSPI0;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        case HAL_BDMA_MIU0_TO_MSPI1:
            g_ptKeBdma->reg_ch0_src_sel = REG_BDMA_SRC_MIU_IMI_CH0;
            g_ptKeBdma->reg_ch0_dst_sel = REG_BDMA_SRC_MSPI1;
            g_ptKeBdma->reg_ch0_replace_miu = REG_BDMA_CH1_MIU0;
            g_ptKeBdma->reg_ch0_src_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            g_ptKeBdma->reg_ch0_dst_dw  = REG_BDMA_DATA_DEPTH_16BYTE;
            break;
        default:
            return HAL_BDMA_PROC_DONE;
            break;
    }

    // Set Source / Destination Address
    if ((HAL_BDMA_MEM_TO_MIU0 == ptBdmaParam->ePathSel) ||
        (HAL_BDMA_MEM_TO_MIU1 == ptBdmaParam->ePathSel) ||
        (HAL_BDMA_MEM_TO_IMI == ptBdmaParam->ePathSel)) {
        g_ptKeBdma->reg_ch0_cmd0_low  = (U16)(ptBdmaParam->u32Pattern & 0xFFFF);
        g_ptKeBdma->reg_ch0_cmd0_high = (U16)(ptBdmaParam->u32Pattern >> 16);
    }
    else {
        g_ptKeBdma->reg_ch0_src_a0 = (U16)(_HalBdmaVA2MiuA(ptBdmaParam->pSrcAddr) & 0xFFFF);
        g_ptKeBdma->reg_ch0_src_a1 = (U16)(_HalBdmaVA2MiuA(ptBdmaParam->pSrcAddr) >> 16);
    }

    g_ptKeBdma->reg_ch0_dst_a0 = (U16)(_HalBdmaVA2MiuA(ptBdmaParam->pDstAddr) & 0xFFFF);
    g_ptKeBdma->reg_ch0_dst_a1 = (U16)(_HalBdmaVA2MiuA(ptBdmaParam->pDstAddr) >> 16);

    // Set Transfer Size
    g_ptKeBdma->reg_ch0_size0 = (U16)(ptBdmaParam->u32TxCount & 0xFFFF);
    g_ptKeBdma->reg_ch0_size1 = (U16)(ptBdmaParam->u32TxCount >> 16);


    /* Set LineOffset Attribute */
    if (ptBdmaParam->bEnLineOfst == TRUE) {
        g_ptKeBdma->reg_ch0_src_width_low      = (U16)(ptBdmaParam->pstLineOfst->u32SrcWidth & 0xFFFF);
        g_ptKeBdma->reg_ch0_src_width_high     = (U16)(ptBdmaParam->pstLineOfst->u32SrcWidth >> 16);
        g_ptKeBdma->reg_ch0_src_offset_low     = (U16)(ptBdmaParam->pstLineOfst->u32SrcOffset & 0xFFFF);
        g_ptKeBdma->reg_ch0_src_offset_high    = (U16)(ptBdmaParam->pstLineOfst->u32SrcOffset >> 16);
        g_ptKeBdma->reg_ch0_dst_width_low      = (U16)(ptBdmaParam->pstLineOfst->u32DstWidth & 0xFFFF);
        g_ptKeBdma->reg_ch0_dst_width_high     = (U16)(ptBdmaParam->pstLineOfst->u32DstWidth >> 16);
        g_ptKeBdma->reg_ch0_dst_offset_low     = (U16)(ptBdmaParam->pstLineOfst->u32DstOffset & 0xFFFF);
        g_ptKeBdma->reg_ch0_dst_offset_high    = (U16)(ptBdmaParam->pstLineOfst->u32DstOffset >> 16);
        g_ptKeBdma->reg_ch0_offset_en          = 1;
    }
    else {
        g_ptKeBdma->reg_ch0_offset_en = 0;
    }

    // Set Interrupt Enable
    if (ptBdmaParam->bIntMode) {
        g_ptKeBdma->reg_ch0_int_en = 1;
    }
    else {
        g_ptKeBdma->reg_ch0_int_en = 0;
    }

    // Trigger
    g_ptKeBdma->reg_ch0_trig = 0x1;

    // Polling mode
    if (!ptBdmaParam->bIntMode) {
        HalBdma_WaitTransferDone(u8DmaCh, ptBdmaParam);
    }

    return HAL_BDMA_PROC_DONE;
}

//------------------------------------------------------------------------------
//  Function    : HalBdma_WaitTransferDone
//  Description :
//------------------------------------------------------------------------------
/**
 * @brief BDMA wait transfer data done
 *
 * @param [in]  ptBdmaParam      BDMA configuration parameter
 *
 * @return HalBdmaErr_e BDMA error code
 */
HalBdmaErr_e HalBdma_WaitTransferDone(u8 u8DmaCh, HalBdmaParam_t *ptBdmaParam)
{
    volatile KeBdma_t* g_ptKeBdma;
    U32                u32TimeOut = 0x00FFFFFF;
    bool               bRet = FALSE;

    //if (!m_bBdmaInited[u8DmaCh]) {
    //    return HAL_BDMA_PROC_DONE;
    //}

    switch(u8DmaCh) {
    case HAL_BDMA_CH0:
        g_ptKeBdma = g_ptKeBdma0;
        break;
    case HAL_BDMA_CH1:
        g_ptKeBdma = g_ptKeBdma1;
        break;
    case HAL_BDMA_CH2:
        g_ptKeBdma = g_ptKeBdma2;
        break;
    case HAL_BDMA_CH3:
        g_ptKeBdma = g_ptKeBdma3;
        break;
    default:
        return HAL_BDMA_PROC_DONE;
        break;
    }

    // Polling mode
    if (!ptBdmaParam->bIntMode) {

        while(--u32TimeOut)
        {
            // Check done
            if (g_ptKeBdma->reg_ch0_done == 0x1)
            {
                bRet = TRUE;
                break;
            }
        }

        // Clear done
        g_ptKeBdma->reg_ch0_done = 0x1;

        if (bRet == FALSE) {
            CamOsPrintf("Wait BDMA Done Fail\r\n");
            return HAL_BDMA_POLLING_TIMEOUT;
        }
    }
    else {
        // Interrupt mode
    }

    return HAL_BDMA_PROC_DONE;
}
