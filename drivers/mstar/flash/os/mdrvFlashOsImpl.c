/*
 * mdrvFlashOsImpl.c- Sigmastar
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
#include <common.h>
#include <mdrvFlashOsImpl.h>

#if defined(CONFIG_FLASH_BDMA)
#include <hal_bdma.h>

u8 FLASH_IMPL_BdmaTransfer(u8 ePath, u8 *pSrcAddr, u8 *nDstAddr, u32 nLength)
{
    HalBdmaParam_t tBdmaParam;
    memset(&tBdmaParam, 0, sizeof(HalBdmaParam_t));
    tBdmaParam.bIntMode          = 0; //0:use polling mode
    if(ePath == BDMA_SPI2MIU)
    {
        tBdmaParam.ePathSel          = HAL_BDMA_SPI_TO_MIU0;
        tBdmaParam.eSrcDataWidth     = HAL_BDMA_DATA_BYTE_8;
        tBdmaParam.eDstDataWidth     = HAL_BDMA_DATA_BYTE_16;
        tBdmaParam.pSrcAddr          = (void*)(unsigned long)pSrcAddr;
        tBdmaParam.pDstAddr          = (void*)((unsigned long)nDstAddr - MIU0_BASE);
    }
    else if(ePath == BDMA_MIU2SPI)
    {
        tBdmaParam.ePathSel          = HAL_BDMA_MIU0_TO_SPI;
        tBdmaParam.eSrcDataWidth     = HAL_BDMA_DATA_BYTE_16;
        tBdmaParam.eDstDataWidth     = HAL_BDMA_DATA_BYTE_8;
        tBdmaParam.pSrcAddr          = (void*)((unsigned long)pSrcAddr - MIU0_BASE);
        tBdmaParam.pDstAddr          = 0;
    }
    tBdmaParam.eDstAddrMode      = HAL_BDMA_ADDR_INC; //address increase
    tBdmaParam.u32TxCount        = nLength;
    tBdmaParam.u32Pattern        = 0;
    tBdmaParam.pfTxCbFunc        = NULL;
    if (HAL_BDMA_PROC_DONE != HalBdma_DoTransfer(HAL_BDMA_CH0, &tBdmaParam))
    {
        FLASH_IMPL_PRINTF("[SPINOR] bdma fail\r\n");
        return 1;
    }
    return DRV_BDMA_PROC_DONE;
}
#endif

void FLASH_IMPL_PRINTF_HEX(const char *pu8_data, u32 u32_value, const char *pu8_data2)
{
    if (pu8_data && pu8_data2)
        FLASH_IMPL_PRINTF("%s%x%s", pu8_data, u32_value, pu8_data2);
    else if (pu8_data)
        FLASH_IMPL_PRINTF("%s%x", pu8_data, u32_value);
    else if (pu8_data2)
        FLASH_IMPL_PRINTF("%x%s", u32_value, pu8_data2);
    else
        FLASH_IMPL_PRINTF("%x", u32_value);
}

void FLASH_IMPL_SHOW_ID(u8 *pu8_data, u8 u8_cnt)
{
    FLASH_IMPL_PRINTF("[FLASH] Device id is 0x%02x 0x%02x 0x%02x\n", pu8_data[0], pu8_data[1], pu8_data[2]);
}

u8 FLASH_IMPL_count_bits(u32 u32_x)
{
    u8 u8_i = 0;

    while (u32_x)
    {
        u8_i++;
        u32_x >>= 1;
    }

    return (u8_i - 1);
}

u32 FLASH_IMPL_checksum(u8 *pu8_data, u32 u32_size)
{
    u32 u32ChkSum = 0;

    while (0 < u32_size)
    {
        u32ChkSum += *pu8_data;
        pu8_data++;
        u32_size--;
    }

    return u32ChkSum;
}
