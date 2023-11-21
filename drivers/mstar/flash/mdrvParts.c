/*
 * mdrvParts.c- Sigmastar
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
#include <mdrvPni.h>
#if defined(CONFIG_FLASH_IS_NAND)
#include <mdrvSpinandBbtBbm.h>
#endif

static const u8 au8_magicData[] = {'S', 'S', 'T', 'A', 'R', 'S', 'E', 'M', 'I', 'C', 'I', 'S', '0', '0', '0', '1'};
static u8 *g_pni_buf;

/********************************************************************************
*Function: MDRV_PARTS_hardware_init_tlb(u32 u32_address)
*Description:  Init the PNI(partititon table) and BBT(bad block table)
*Input:
       u32_address: Address to store pni and bbt for share information
*Output:
*Retrun:
********************************************************************************/
void MDRV_PARTS_hardware_init_tlb(u8 *pni_buf)
{
    g_pni_buf = NULL;

    if (!MDRV_PNI_init_tlb(pni_buf))
    {
        g_pni_buf = pni_buf;
    }
}

/********************************************************************************
*Function: MDRV_PARTS_init_tlb(u32 u32_address)
*Description:  Init the PNI(partititon table) and BBT(bad block table)
*Input:
       u32_address: Address to store pni and bbt for share information
*Output:
*Retrun:
********************************************************************************/
void MDRV_PARTS_init_tlb(u8 *pni_buf)
{
    g_pni_buf = NULL;

    if (MDrv_PARTS_is_tbl_valid(pni_buf))
    {
        g_pni_buf = pni_buf;
    }
}

/********************************************************************************
*Function: MDRV_PARTS_is_tbl_header(u8 *pni_buf)
*Description: Check the partition table header
*Input:
        pni_buf: The data to be checked
*Output:
*Retrun:
        TRUE:  It is partition table header
        FALSE: It is not partition table header
********************************************************************************/
u8 MDRV_PARTS_is_tbl_header(u8 *pni_buf)
{
    u8 u8_i;

    for(u8_i = 0; u8_i < sizeof(au8_magicData); u8_i++)
    {
        if (au8_magicData[u8_i] != pni_buf[u8_i])
        {
            return 0;
        }
    }

    return 1;
}

/********************************************************************************
*Function: MDrv_PARTS_is_tbl_valid(u32 u32_address)
*Description: Check the magic and checksum of PNI
*Input:
        u32_address: The address where store the PNI to be checked
*Output:
*Retrun:
        TRUE : Check successful
        FALSE: Check failed
********************************************************************************/
u8 MDrv_PARTS_is_tbl_valid(u8 *pni_buf)
{
    PARTS_TBL_t *pstPartsTbl;

    pstPartsTbl = (PARTS_TBL_t*)pni_buf;

    //check the partition's table
    if (FLASH_IMPL_MEMCMP(pstPartsTbl->au8_magic, au8_magicData, sizeof(au8_magicData)))
    {
        if ( pstPartsTbl->u32_Checksum == FLASH_IMPL_checksum((u8 *)&(pstPartsTbl->u32_Size), pstPartsTbl->u32_Size + sizeof(pstPartsTbl->u32_Size)))
        {
            return 1;
        }

        FLASH_IMPL_PRINTF_HEX("[PARTS_ERR] checksum err, pstPartsTbl->u32_Checksum = 0x", pstPartsTbl->u32_Checksum, "\r\n");
        FLASH_IMPL_PRINTF_HEX("[PARTS_ERR] checksum err, Calc Checksum = 0x", FLASH_IMPL_checksum((u8 *)&(pstPartsTbl->u32_Size), pstPartsTbl->u32_Size + sizeof(pstPartsTbl->u32_Size)), "\r\n");
    }

    return 0;
}

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
u8 MDRV_PARTS_mark_active(u8 *pu8_PartName, u8 u8_Trunk)
{
    u32 i = 0;
    u32 u32Count = 0;
    PARTS_TBL_t *pstPartsTbl;
    PARTS_INFO_t *pstPartsInfo;

    if (!g_pni_buf || !pu8_PartName)
    {
        return 1;
    }

    pstPartsTbl = (PARTS_TBL_t *)g_pni_buf;
    pstPartsInfo = (PARTS_INFO_t *)(g_pni_buf + sizeof(PARTS_TBL_t));
    u32Count = pstPartsTbl->u32_Size / sizeof(PARTS_INFO_t);

    for(i = 0; i < u32Count; i++)
    {
        if (!FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, pu8_PartName))
        {
            pstPartsInfo->u8_Active = 0;

            if (u8_Trunk == pstPartsInfo->u8_Trunk)
            {
                pstPartsInfo->u8_Active = 1;
            }

        }
        pstPartsInfo++;
    }
    //when the pni is changed,the checksum should be recaculated
    pstPartsTbl->u32_Checksum = FLASH_IMPL_checksum((u8 *)&(pstPartsTbl->u32_Size), pstPartsTbl->u32_Size + sizeof(pstPartsTbl->u32_Size));

    return MDRV_PNI_write_back(g_pni_buf);
}

/********************************************************************************
*Function: MDRV_PARTS_get_part(u8 *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_partInfo)
*Description:  According to partName and trunk,find the partition
*Input:
        pu8_PartName: The partition name
        u8_Trunk    : The trunk number
        pst_partInfo: The address to save partition info
*Output:
*Retrun:
        TRUE : Get partition successful
        FALSE: Get partition failed
********************************************************************************/
u8 MDRV_PARTS_get_part(u8 *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_partInfo)
{
    u8 u8_i;
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;

    if (!g_pni_buf || !pu8_PartName || !pst_partInfo)
    {
        return 0;
    }

    pst_partsTbl = (PARTS_TBL_t*)g_pni_buf;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_pni_buf + sizeof(PARTS_TBL_t));

    for (u8_i = 0; u8_i < u32Count; u8_i++)
    {
        if (!FLASH_IMPL_STRCMP(pstTemp->au8_partName, pu8_PartName))
        {
            if (u8_Trunk == pstTemp->u8_Trunk)
            {
                memcpy(pst_partInfo, pstTemp, sizeof(PARTS_INFO_t));
                break;
            }
        }
        pstTemp++;
    }

    if (u8_i == u32Count)
    {
        return 0;
    }

    return 1;
}

/********************************************************************************
*Function: MDRV_PARTS_get_active_part(u8 *pu8_PartName, PARTS_INFO_t *pst_partInfo)
*Description:  According to partName,find the active partition
*Input:
        pu8_PartName: The partition name
        pst_partInfo: The address to save partition info
*Output:
*Retrun:
        TRUE : Get active partition successful
        FALSE: Get active partition failed
********************************************************************************/
u8 MDRV_PARTS_get_active_part(u8 *pu8_PartName, PARTS_INFO_t *pst_partInfo)
{
    u32 i = 0;
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp = NULL;

    if (!g_pni_buf || !pu8_PartName || !pst_partInfo)
    {
        return 0;
    }

    pst_partsTbl = (PARTS_TBL_t*)g_pni_buf;
    pstTemp = (PARTS_INFO_t*)(g_pni_buf + sizeof(PARTS_TBL_t));
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    for (i = 0; i < u32Count; i++)
    {
        if ((pstTemp->u8_Active == 1) && !FLASH_IMPL_STRCMP(pstTemp->au8_partName, pu8_PartName))
        {
            memcpy(pst_partInfo, pstTemp, sizeof(PARTS_INFO_t));
            break;
        }
        pstTemp++;
    }

    if (i == u32Count)
    {
        return 0;
    }

    return 1;

}

/********************************************************************************
*Function: MDRV_PARTS_get_part(u8 *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_PartInfo)
*Description:  According to partName and trunk,find the partition
*Input:
        pu8_PartName: The partition name
        u8_Trunk    : The trunk number
        pst_PartInfo: The address to save partition info
*Output:
*Retrun:
        1 : Get partition successful
        FALSE: Get partition failed
********************************************************************************/
u8 MDRV_PARTS_get_part_nm(u8 u8_partId, PARTS_INFO_t *pst_partInfo)
{
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;

    if (!g_pni_buf || !pst_partInfo)
    {
        return 0;
    }

    pst_partsTbl = (PARTS_TBL_t*)g_pni_buf;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_pni_buf + sizeof(PARTS_TBL_t));

    if (u8_partId < u32Count)
    {
        memcpy(pst_partInfo, &pstTemp[u8_partId], sizeof(PARTS_INFO_t));
    }
    else
    {
        return 0;
    }

    return 1;
}

#if defined(CONFIG_FLASH_NO_SUPPORT_MTD)
#include <mdrvFlash.h>

/********************************************************************************
*Function: MDRV_PARTS_load_part(PARTS_INFO_t *pst_partInfo, u32 u32_address, u32 u32_size)
*Description: Load partition
*Input:
        pst_partInfo: Find the partition to be loaded
        u32_address : The address to load partition
        u32_size    : The size of partition
*Output:
*Retrun:
        Success : The image's size
        Failed  : The image actually loaded size
********************************************************************************/
u32 MDRV_PARTS_load_part(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
{
    u8 u8_last_Trunk;
    u32 u32RdSz = 0;

    if (!pst_partInfo)
    {
        return 0;
    }

    if ((u32_offset > pst_partInfo->u32_Size) || (u32_size > (pst_partInfo->u32_Size - u32_offset)))
    {
        return 0;
    }

    u8_last_Trunk = -1;

load_image:

#if defined(CONFIG_FLASH_IS_NAND)
    //Power failure recovery from bbt(page1),if recover failed,switch active partition
    MDRV_BBM_failure_recover(pst_partInfo, pu8_data);
#endif

    FLASH_IMPL_PRINTF_LOAD_INFO((void *)pst_partInfo, u32_offset, (u32)pu8_data, u32_size);

    FLASH_IMPL_GET_TIME();

#if defined(CONFIG_FLASH_IS_NAND)
    u32RdSz = MDRV_FLASH_read_skip_bad(pst_partInfo->u32_Offset + u32_offset, pst_partInfo->u32_Size - u32_offset, pu8_data, u32_size);

    if((u32RdSz == u32_size) && (0 == u32_offset))
    {
        //When loading partition image,if found there have ecc correctable blocks,do bbm flow
        //Needn't do do bbm when the offset is larger than 0,because we don't know the image be loaded is complete or not
        MDRV_BBM_load_part_bbm(pst_partInfo, pu8_data, u32_size);
    }
#else
    u32RdSz = MDRV_FLASH_read(pst_partInfo->u32_Offset + u32_offset, pu8_data, u32_size);
#endif

    FLASH_IMPL_PRINTF_TIME_DIFF();

    if (u32RdSz != u32_size)
    {
        if (pst_partInfo->u8_BackupTrunk != pst_partInfo->u8_Trunk)
        {
            //If the partition isn't broken beacause of bbm,mark other trunk as active or not decided by customer
            if (-1 == u8_last_Trunk || u8_last_Trunk != pst_partInfo->u8_BackupTrunk)
            {
                FLASH_IMPL_PRINTF("[PARTS] The partiton is bad,get the backup partition\r\n");
                MDRV_PARTS_get_part(pst_partInfo->au8_partName, pst_partInfo->u8_BackupTrunk, pst_partInfo);

                if (-1 == u8_last_Trunk)
                {
                    u8_last_Trunk = pst_partInfo->u8_Trunk;
                }
                goto load_image;
            }
        }
    }

    return u32_size;
}

/********************************************************************************
*Function: MDRV_PARTS_block_isbad(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u32 u32_address, u32 u32_size)
*Description: block isbad partition
*Input:
        pst_partInfo: The partition to be programmed
        u32_offset : The partition offset
*Output:
*Retrun:
        0 : good block  1 : bad block
********************************************************************************/

u32 MDRV_PARTS_block_isbad(PARTS_INFO_t *pst_partInfo, u32 u32_offset)
{
#if defined(CONFIG_FLASH_IS_NAND)
    if((!pst_partInfo) || (!g_pni_buf))
        return 0;

    if (u32_offset > pst_partInfo->u32_Size)
    {
        return 0;
    }

    return MDRV_FLASH_block_isbad(pst_partInfo->u32_Offset + u32_offset);
#else
    pst_partInfo = pst_partInfo;
    u32_offset = u32_offset;

    return 0;
#endif
}

/********************************************************************************
*Function: MDRV_PARTS_read(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u32 u32_address, u32 u32_size)
*Description: read partition
*Input:
        pst_partInfo: The partition to be programmed
        u32_address : The partition data to be written
        u32_size    : The size of partition
*Output:
*Retrun:
        u32WrSize  : The size of the data being written
********************************************************************************/
u32 MDRV_PARTS_read(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
{
    if(!pst_partInfo)
        return 0;

    if ((u32_offset > pst_partInfo->u32_Size) || (u32_size > (pst_partInfo->u32_Size - u32_offset)))
    {
        return 0;
    }

    if (u32_size != MDRV_FLASH_read(pst_partInfo->u32_Offset + u32_offset, pu8_data, u32_size))
    {
        return 0;
    }

    return u32_size;
}

/********************************************************************************
*Function: MDRV_PARTS_write(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u32 u32_address, u32 u32_size)
*Description: Program partition
*Input:
        pst_partInfo: The partition to be programmed
        u32_address : The partition data to be written
        u32_size    : The size of partition
*Output:
*Retrun:
        u32WrSize  : The size of the data being written
********************************************************************************/
u32 MDRV_PARTS_write(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
{
    if(!pst_partInfo)
        return 0;

    if ((u32_offset > pst_partInfo->u32_Size) || (u32_size > (pst_partInfo->u32_Size - u32_offset)))
    {
        return 0;
    }

    if (u32_size != MDRV_FLASH_write(pst_partInfo->u32_Offset + u32_offset, pu8_data, u32_size))
    {
        return 0;
    }

    return u32_size;
}

/********************************************************************************
*Function: MDRV_PARTS_erase(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u32 u32_size)
*Description: erase partition
*Input:
        pst_partInfo: The partition to be programmed
        u32_address : The partition data to be written
        u32_size    : The size of partition
*Output:
*Retrun:
        u32WrSize  : The size of the data being written
********************************************************************************/
u32 MDRV_PARTS_erase(PARTS_INFO_t *pst_partInfo, u32 u32_offset, u32 u32_size)
{
    if(!pst_partInfo)
        return 0;

    if ((u32_offset > pst_partInfo->u32_Size) || (u32_size > (pst_partInfo->u32_Size - u32_offset)))
    {
        return 0;
    }

    if (u32_size != MDRV_FLASH_erase(pst_partInfo->u32_Offset + u32_offset, u32_size))
    {
        return 0;
    }

    return u32_size;
}
#endif
