/*
 * mdrvPni.c- Sigmastar
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
#include <mdrvFlashOsImpl.h>
#include <mdrvParts.h>
#include <drvSPINOR.h>

#define PARTS_CIS_LOAD_OFFSET   0x08
#define PARTS_SEARCH_END        (0x10000)
#define DEFAULT_PNI_OFFSET      0x21000
#define FLASH_SECTOR_SIZE       g_st_nor_info.u32_sectorSize

static u32 g_u32_pni_offset;
static FLASH_NOR_INFO_t g_st_nor_info;

#if defined(CONFIG_FLASH_WRITE_BACK)
/********************************************************************************
*Function: MDRV_PARTS_mark_active(u8 *pu8_PartName, u8 u8_Trunk)
*Description:  According to partName and trunk,mark the partition as active
*Input:
        pu8_PartName: The partition name
        u8_Trunk    : The trunk number
*Output:
*Retrun:
        TRUE : Mark active successful
        FALSE: Mark active failed
********************************************************************************/
u8 MDRV_PNI_write_back(u8 *pni_buf)
{
    u32 u32_parts_size = 0;
    PARTS_TBL_t *pst_partsTbl;

    pst_partsTbl = (PARTS_TBL_t*)pni_buf;
    u32_parts_size = sizeof(PARTS_TBL_t) + pst_partsTbl->u32_Size;

    if (u32_parts_size > FLASH_SECTOR_SIZE)
    {
        return 0;
    }

    //erase pni,write back pni
    if ((ERR_SPINOR_SUCCESS != mdrv_spinor_erase(g_u32_pni_offset, FLASH_SECTOR_SIZE)) ||
       (ERR_SPINOR_SUCCESS != mdrv_spinor_program(g_u32_pni_offset, (u8 *)u32_parts_size, u32_parts_size)))
    {
        FLASH_IMPL_PRINTF("[PARTS_ERR] There is no partition table in block!\r\n");
        return 0;
    }

    return 1;
}
#endif

/********************************************************************************
*Function: _MDRV_PNI_load_parts_tbl(u32 u32_address)
*Description: Init the PNI(partititon table)
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
        TRUE : Init successful
        FALSE: Init failed
********************************************************************************/
static u8 _MDRV_PNI_load_parts_tbl(u8 *pni_buf)
{
    u32 u32_pni_address;
    u32_pni_address = 0;

    if (ERR_SPINOR_SUCCESS != mdrv_spinor_read(PARTS_CIS_LOAD_OFFSET, (u8 *)&u32_pni_address, 2))
    {
        FLASH_IMPL_PANIC("[FLASH_ERR] SNI offset of satrting fail!\r\n");
    }

    u32_pni_address = (u32_pni_address + 0xfff) & ~0xfff;

    for (; PARTS_SEARCH_END > u32_pni_address; u32_pni_address += 0x1000)
    {
        if (ERR_SPINOR_SUCCESS != mdrv_spinor_read(u32_pni_address, pni_buf, sizeof(PARTS_TBL_t)))
        {
            FLASH_IMPL_PRINTF("[PARTS] Load PNI header fail.\r\n");
            return 0;
        }

        if (MDRV_PARTS_is_tbl_header(pni_buf))
        {
            break;
        }
    }

    if (!MDRV_PARTS_is_tbl_header(pni_buf))
    {
        u32_pni_address = DEFAULT_PNI_OFFSET;
    }

    if (ERR_SPINOR_SUCCESS != mdrv_spinor_read(u32_pni_address, pni_buf, MAX_PARTS_TBL_SIZE))
    {
        FLASH_IMPL_PRINTF("[PARTS] Load parts tbl fail!\r\n");
        return 0;
    }

    if (!MDrv_PARTS_is_tbl_valid(pni_buf))
    {
        FLASH_IMPL_PRINTF("[PARTS] No partition table!\r\n");
        return 0;
    }

    g_u32_pni_offset = u32_pni_address;
    FLASH_IMPL_PRINTF_HEX("[PARTS] Load parts tbl from 0x", u32_pni_address, "\r\n");

    return 1;
}

/********************************************************************************
*Function: MDRV_Parts_init_tlb(u32 u32Addr)
*Description: Init the PNI(partititon table)
*Input:
        u32Addr: Address to store partition table
*Output:
*Retrun:
********************************************************************************/
u8 MDRV_PNI_init_tlb(u8 *pni_buf)
{
    mdrv_spinor_info(&g_st_nor_info);

    g_u32_pni_offset = 0;

    if (0 == g_st_nor_info.u32_capacity)
    {
        FLASH_IMPL_PRINTF("[PARTS_ERR] No flash in board!\r\n");
        return 1;
    }

    if(!_MDRV_PNI_load_parts_tbl(pni_buf))
    {
        FLASH_IMPL_PRINTF("[PARTS_ERR] Load parts tbl failed!\r\n");
        return 1;
    }

    FLASH_IMPL_PRINTF_HEX("[PARTS] Got PNI @ 0x", (u32)pni_buf, "\r\n");

    return 0;
}
