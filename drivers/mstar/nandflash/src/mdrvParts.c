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
#include <common.h>
#include <drvSPINAND.h>
#include "mdrvParts.h"
#include <drvFSP_QSPI.h>
#include <halFSP_QSPI.h>
#include <mdrvSNI.h>
#include <nand.h>

typedef struct {
    u32 u32_eraseSize;
    u32 u32_writeSize;
    u32 u32_capacity;
    u32 u32_oobSize;
} FLASH_PARTS_t;


static const u8 au8_magicData[] = {'S', 'S', 'T', 'A', 'R', 'S', 'E', 'M', 'I', 'C', 'I', 'S', '0', '0', '0', '1'};
static u8 *g_partsTbl;
static FLASH_PARTS_t g_parts_info = {0, 0, 0, 0};

#define FLASH_PAGE_SIZE                 g_parts_info.u32_writeSize
#define FLASH_BLOCK_SIZE                g_parts_info.u32_eraseSize
#define FLASH_SIZE                      g_parts_info.u32_capacity

/********************************************************************************
*Function: MDRV_PARTS_is_tbl_header(u8 *pu8_data)
*Description: Check the partition table header
*Input:
        pu8_data: The data to be checked
*Output:
*Retrun:
        1:  It is partition table header
        FALSE: It is not partition table header
********************************************************************************/
u8 MDRV_PARTS_is_tbl_header(u8 *pu8_data)
{
    u8 u8_i;

    for(u8_i = 0; u8_i < sizeof(au8_magicData); u8_i++)
    {
        if (au8_magicData[u8_i] != pu8_data[u8_i])
        {
            return 0;
        }
    }

    return 1;
}

/********************************************************************************
*Function: _MDRV_PARTS_is_parts_match(PARTS_INFO_t *pst_partInfo, u8 *pu8_partName)
*Description: According to the partition name to check the partition is matched or not
*Input:
        pst_partInfo: The partition info
        pu8_partName: The name of the partition to be check
*Output:
*Retrun:
        1:  Match the partition
        FALSE: Don't match the partition
********************************************************************************/
static u8 _MDRV_PARTS_is_parts_match(PARTS_INFO_t *pst_partInfo, u8 *pu8_partName)
{
    u8 u8_i;

    if ('\0' == pst_partInfo->au8_partName[0])
    {
        return 0;
    }

    for (u8_i = 0; sizeof(pst_partInfo->au8_partName) > u8_i;)
    {
        if('\0' != pst_partInfo->au8_partName[u8_i] && '\0' != *(pu8_partName + u8_i))
        {
            if (pst_partInfo->au8_partName[u8_i] != *(pu8_partName + u8_i))
            {
                return 0;
            }

            u8_i++;
            continue;
        }

        if('\0' != pst_partInfo->au8_partName[u8_i] || '\0' != *(pu8_partName + u8_i))
            return 0;

        break;
    }

    return 1;
}

/********************************************************************************
*Function: _MDRV_PARTS_checksum(u32 u32_address, u32 u32_size)
*Description: Calculate the PNI's checksum
*Input:
        u32_address: The partition info which need to be calculated
        u32_size   : The size of the partition
*Output:
*Retrun:
        u32ChkSum : The checksum of partition's info
********************************************************************************/
static u32 _MDRV_PARTS_checksum(u32 u32_address, u32 u32_size)
{
    u32 u32ChkSum = 0;

    while (0 != u32_size)
    {
        u32ChkSum += *((u8*)u32_address);
        u32_address++;
        u32_size--;
    }

    return u32ChkSum;
}

/********************************************************************************
*Function: _MDRV_PARTS_creat_default_patrs_tbl(u32 u32_address)
*Description: Creat default partiton table
*Input:
        u32_address: Address to store default PNI
*Output:
*Retrun:
********************************************************************************/
#if 0
static void _MDRV_PARTS_creat_default_patrs_tbl(u32 u32_address)
{
    u8 u8_i = 0;
    PARTS_TBL_t stDefPartsTbl;

    PARTS_INFO_t stDefPartsInfo[DEFAULT_PARTITION_NUM] = {
        {"IPL",0,1,1,0,10,3,0},            //blockIndex=10,blockNum=3
        {"IPL_CUST",0,1,1,0,16,3,0},       //blockIndex=16,blockNum=3
        {"UBOOT",0,1,1,0,22,6,0},          //blockIndex=22,blockNum=6
    };

    memcpy(stDefPartsTbl.au8_magic, au8_magicData, sizeof(au8_magicData));

    while (u8_i < DEFAULT_PARTITION_NUM)
    {
        stDefPartsInfo[u8_i].u32_Offset *= FLASH_BLOCK_SIZE;    //change blockIndex into u32_Offset(byte)
        stDefPartsInfo[u8_i].u32_Size *= FLASH_BLOCK_SIZE;      //change blockNum into u32_Size(byte)
        u8_i++;
    }

    stDefPartsTbl.u32_Size = sizeof(PARTS_INFO_t) * DEFAULT_PARTITION_NUM;

    memcpy((void*)(u32_address + sizeof(PARTS_TBL_t)), stDefPartsInfo, stDefPartsTbl.u32_Size);
    stDefPartsTbl.u32_Checksum = _MDRV_PARTS_checksum((u32)&(stDefPartsTbl.u32_Size), stDefPartsTbl.u32_Size + sizeof(stDefPartsTbl.u32_Size));
    memcpy((void*)u32_address, &stDefPartsTbl, sizeof(PARTS_TBL_t));
}
#endif
/********************************************************************************
*Function: _MDrv_PARTS_is_tbl_valid(u32 u32_address)
*Description: Check the magic and checksum of PNI
*Input:
        u32_address: The address where store the PNI to be checked
*Output:
*Retrun:
        1 : Check successful
        FALSE: Check failed
********************************************************************************/
static u8 _MDrv_PARTS_is_tbl_valid(u8 *pni_buf)
{
    PARTS_TBL_t *pstPartsTbl;

    pstPartsTbl = (PARTS_TBL_t*)pni_buf;

    //check the partition's table
    if (1 == MDRV_PARTS_is_tbl_header(pstPartsTbl->au8_magic))
    {
        if ( pstPartsTbl->u32_Checksum == _MDRV_PARTS_checksum((u32)&(pstPartsTbl->u32_Size), pstPartsTbl->u32_Size + sizeof(pstPartsTbl->u32_Size)))
        {
            return 1;
        }
    }

    return 0;
}

/********************************************************************************
*Function: _MDRV_PARTS_is_programmable(u32 u32_address, u32 u32_offset, u32 u32_size)
*Description: Check programming is successful or not
*Input:
        u32_address: The address where store the data to be written
        u32_offset : The flash address to write
        u32_size   : The data size
*Output:
*Retrun:
        1 : Progaram successful
        FALSE: Progaram failed
********************************************************************************/
#if 0
u8 MDRV_PARTS_is_programmable(u32 u32_offset, u32 u32_address, u32 u32_size)
{
    if (FLASH_BLOCK_SIZE >= u32_size && NO_ERR_BLOCK == MDRV_FLASH_get_bad(u32_offset, FLASH_BLOCK_SIZE))
    {
        if (FLASH_BLOCK_SIZE == MDRV_FLASH_erase(u32_offset, FLASH_BLOCK_SIZE) &&
        u32_size == MDRV_FLASH_write_pages(u32_offset, u32_address, u32_size))
        {
            return 1;
        }
    }

    return FALSE;
}
#endif
/********************************************************************************
*Function: _MDRV_PARTS_adjust_onebin(PARTS_INFO_t *pst_PartsInfo,u8 u8_Trunk)
*Description: Adjust the partititon for onebin
*Input:
        pst_PartsInfo: The partition info need to be adjust
        u8_Trunk: The trunk number of partition
*Output:
*Retrun:
    1 : The table of onebin has been changed
    FALSE: The table of onebin hasn't been changed
********************************************************************************/
#if 0
static u8 _MDRV_PARTS_adjust_onebin(u32 u32_address,u8 u8_Trunk)
{
    u8 u8_change = 0;
    u32 u32_i = 0,u32Count = 0;
    u32 u32_iplEnd = 0,u32_iplCustEnd = 0,u32_ubootStart = 0,u32_size = 0;
    u32 u32_check_start=0,u32_check_size=0,u32_bad_offset=0,u32_end = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstPartsInfo;
    PARTS_INFO_t stIplInfo,stIplCustInfo,stUbootInfo;

    pst_partsTbl = (PARTS_TBL_t*)u32_address;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstPartsInfo = (PARTS_INFO_t*)(u32_address + sizeof(PARTS_TBL_t));

    memset((void *)&stIplInfo, 0, sizeof(PARTS_INFO_t));
    memset((void *)&stIplCustInfo, 0, sizeof(PARTS_INFO_t));
    memset((void *)&stUbootInfo, 0, sizeof(PARTS_INFO_t));

    for(u32_i = 0; u32_i < u32Count; u32_i++)
    {
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"IPL"))
        {
            memcpy(&stIplInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_iplEnd = pstPartsInfo->u32_Offset + pstPartsInfo->u32_Size;
        }
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"IPL_CUST"))
        {
            memcpy(&stIplCustInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_iplCustEnd = pstPartsInfo->u32_Offset + pstPartsInfo->u32_Size;
        }
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"UBOOT"))
        {
            memcpy(&stUbootInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_ubootStart = pstPartsInfo->u32_Offset;
        }
        pstPartsInfo++;
    }
    if((u32_iplEnd != stIplCustInfo.u32_Offset) || (u32_iplCustEnd != stUbootInfo.u32_Offset)
        || 0 != u32_iplEnd || 0 != u32_iplCustEnd || 0 != u32_ubootStart)
    {
        //It isn't onebin,needn't to adjust partition table
        return 0;
    }
    u32_end = stIplInfo.u32_Offset + u32_size;
    //find ipl_cust start address
    u32_check_start = stIplInfo.u32_Offset;
    u32_check_size = stIplInfo.u32_Size;    //good block size
    while (NO_ERR_BLOCK != (u32_bad_offset = MDRV_FLASH_get_bad(u32_check_start, u32_check_size)))
    {
        if(u32_bad_offset == stIplInfo.u32_Offset)//The first block of IPL_CUST is bad block
            stIplInfo.u32_Offset += FLASH_BLOCK_SIZE;
        else
            stIplInfo.u32_Size += FLASH_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_BLOCK_SIZE;
    }

    if(stIplInfo.u32_Offset >= u32_end)
    {
        stIplInfo.u32_Offset = u32_end;
        stIplInfo.u32_Size = 0;
    }

    stIplCustInfo.u32_Offset = stIplInfo.u32_Offset + stIplInfo.u32_Size;
    if(stIplCustInfo.u32_Offset >= u32_end)//beyond the onebine range
    {
        stIplCustInfo.u32_Offset = u32_end;
        stIplCustInfo.u32_Size = 0;
    }

    //find uboot start address
    u32_check_start = stIplCustInfo.u32_Offset;
    u32_check_size = stIplCustInfo.u32_Size;    //good block size
    while (NO_ERR_BLOCK != (u32_bad_offset = MDRV_FLASH_get_bad(u32_check_start, u32_check_size)))
    {
        if(u32_bad_offset == stIplCustInfo.u32_Offset)//The first block of IPL_CUST is bad block
            stIplCustInfo.u32_Offset += FLASH_BLOCK_SIZE;
        else
            stIplCustInfo.u32_Size += FLASH_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_BLOCK_SIZE;
    }

    if(stIplCustInfo.u32_Offset >= u32_end)
    {
        stIplCustInfo.u32_Offset = u32_end;
        stIplCustInfo.u32_Size = 0;
    }

    stUbootInfo.u32_Offset = stIplCustInfo.u32_Offset + stIplCustInfo.u32_Size;
    if(stUbootInfo.u32_Offset >= u32_end)//beyond the onebine range
    {
        stUbootInfo.u32_Offset = u32_end;
        stUbootInfo.u32_Size = 0;
    }
    else
    {
        stUbootInfo.u32_Size = u32_end - stUbootInfo.u32_Offset;
    }
    //check bad block in uboot partition
    u32_check_start = stUbootInfo.u32_Offset;
    u32_check_size = stUbootInfo.u32_Size;    //good block size
    while (NO_ERR_BLOCK != (u32_bad_offset = MDRV_FLASH_get_bad(u32_check_start, u32_check_size)))
    {
        if(u32_bad_offset == stUbootInfo.u32_Offset)//The first block of IPL_CUST is bad block
            stUbootInfo.u32_Offset += FLASH_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_BLOCK_SIZE;
    }
    if(stUbootInfo.u32_Offset >= u32_end)//beyond the onebine range
    {
        stUbootInfo.u32_Offset = u32_end;
        stUbootInfo.u32_Size = 0;
    }

    if((u32_ubootStart != stUbootInfo.u32_Offset) && (u32_ubootStart != 0))
    {
        pstPartsInfo = (PARTS_INFO_t*)(u32_address + sizeof(PARTS_TBL_t));
        for(u32_i = 0; u32_i < u32Count; u32_i++)
        {
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"IPL"))
            {
                memcpy(pstPartsInfo, &stIplInfo, sizeof(PARTS_INFO_t));
            }
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"IPL_CUST"))
            {
                memcpy(pstPartsInfo, &stIplCustInfo,sizeof(PARTS_INFO_t));
            }
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && _MDRV_PARTS_is_parts_match(pstPartsInfo, (u8*)"UBOOT"))
            {
                memcpy(pstPartsInfo, &stUbootInfo,sizeof(PARTS_INFO_t));
            }
            pstPartsInfo++;
        }
        u8_change = 1;
        printf("Change the table of onebin");
        FLASH_ERR_VAR8(u8_Trunk);
        printf("\r\n");
    }
    return u8_change;
}
#endif
/********************************************************************************
*Function: _MDRV_PARTS_adjust_parts_tbl(u32 u32_address)
*Description: Adjust the PNI(partititon table) for onebin
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
********************************************************************************/
#if 0
static void _MDRV_PARTS_adjust_parts_tbl(u32 u32_address)
{
    u8 u8_ret0=0,u8_ret1=0;
    PARTS_TBL_t *pst_partsTbl = (PARTS_TBL_t*)u32_address;

    //u8_ret0 = _MDRV_PARTS_adjust_onebin(u32_address,0);
    //u8_ret1 = _MDRV_PARTS_adjust_onebin(u32_address,1);

    if(u8_ret0 || u8_ret1)
    {
        //when the pni is changed,the checksum should be recaculated
        pst_partsTbl->u32_Checksum = _MDRV_PARTS_checksum((u32)&(pst_partsTbl->u32_Size), pst_partsTbl->u32_Size + sizeof(pst_partsTbl->u32_Size));
    }
}
#endif
/********************************************************************************
*Function: _MDRV_PARTS_write_back_parts_tbl(u32 u32_address)
*Description: Read PNI from odd block,and write it back to even block
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
        1 : Write back PNI successful
        FALSE: Write back PNI failed
********************************************************************************/
#if 0
static void _MDRV_PARTS_write_back_parts_tbl(u32 u32_address)
{
    u32 u32_pni_offset;
    u32 u32_parts_tbl_size;

    PARTS_TBL_t st_partsTbl;

    u32_pni_offset = 10 * FLASH_BLOCK_SIZE; 
    u32_parts_tbl_size = 0;

    printf("[PARTS] Not found table, write back now!\r\n");

    do
    {
        u32_pni_offset -= FLASH_BLOCK_SIZE;
        if ((u32_pni_offset & FLASH_BLOCK_SIZE))
        {
            if (sizeof(PARTS_TBL_t) == MDRV_FLASH_read(u32_pni_offset + FLASH_PAGE_SIZE, FLASH_PAGE_SIZE, (u32)&st_partsTbl, sizeof(PARTS_TBL_t)))
            {
                if (MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic))
                {
                    u32_parts_tbl_size = sizeof(PARTS_TBL_t) + st_partsTbl.u32_Size;//found parts table
                    break;    //Record last odd block
                }
            }
        }
    } while (0 != u32_pni_offset);

    do
    {
        if (0 == u32_parts_tbl_size)
        {
            break;
        }

        if (u32_parts_tbl_size != MDRV_FLASH_read(u32_pni_offset + FLASH_PAGE_SIZE, FLASH_PAGE_SIZE, u32_address, u32_parts_tbl_size))
        {
            break;
        }

        _MDRV_PARTS_adjust_parts_tbl(u32_address); //Ajust the partiton table for boot image
        if (!_MDrv_PARTS_is_tbl_valid(u32_address))
        {
            break;
        }

        do
        {
            u32_pni_offset -= FLASH_BLOCK_SIZE;
            if (0 == (u32_pni_offset & FLASH_BLOCK_SIZE))
            {
                if (u32_parts_tbl_size == MDRV_FLASH_write_pages(u32_pni_offset + FLASH_PAGE_SIZE, u32_address, u32_parts_tbl_size))
                {
                    printf("[PARTS] Write table to 0x");
                    FLASH_ERR_VAR32(u32_pni_offset + FLASH_PAGE_SIZE);
                    printf("\r\n");
                }
            }
        } while(0 != u32_pni_offset);
    } while (0);

    if (0 == u32_parts_tbl_size || !_MDrv_PARTS_is_tbl_valid(u32_address))
    {
        _MDRV_PARTS_creat_default_patrs_tbl(u32_address);
    }
}
#else
#define _MDRV_PARTS_write_back_parts_tbl(x) (-1)
#endif
/********************************************************************************
*Function: _MDRV_PARTS_load_parts_tbl(u32 u32_address)
*Description: Init the PNI(partititon table)
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
        1 : Init successful
        FALSE: Init failed
********************************************************************************/
static u8 _MDRV_PARTS_load_parts_tbl(u8 *pni_buf)
{
    u32 u32_pni_offset;
    u32 u32_pni_end;
    size_t retlen;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];
    if (!nand)
    {
        return -1;
    }

    u32_pni_offset = 0x0;
    u32_pni_end = FLASH_BLOCK_SIZE * 10;

    for (; u32_pni_offset != u32_pni_end; u32_pni_offset += FLASH_BLOCK_SIZE)
    {
        if (0 == (u32_pni_offset & FLASH_BLOCK_SIZE))
        {
            retlen = FLASH_PAGE_SIZE;

            if (FLASH_PAGE_SIZE == nand_read_skip_bad(nand, (loff_t)(u32_pni_offset + FLASH_PAGE_SIZE), &retlen, NULL, FLASH_BLOCK_SIZE, pni_buf))
            {
                if (_MDrv_PARTS_is_tbl_valid(pni_buf))
                {
                    printf("[PARTS] Found table in 0x%x\n",u32_pni_offset + FLASH_PAGE_SIZE);
                    return 0;
                }

                break;
            }
            
        }
    }

    if (_MDrv_PARTS_is_tbl_valid(pni_buf))
    {
        return 0;
    }

    return _MDRV_PARTS_write_back_parts_tbl(pni_buf);
}

/********************************************************************************
*Function: MDRV_PARTS_init_tlb(u32 u32_address)
*Description:  Init the PNI(partititon table) and BBT(bad block table)
*Input:
       u32_address: Address to store pni and bbt for share information
*Output:
*Retrun:
********************************************************************************/
u8 MDRV_PARTS_init_tlb(u8 *pni_buf)
{
    struct mtd_info *mtd = NULL;

    if (IS_ERR(mtd = get_mtd_device_nm("nand0")))
    {
        return -1;
    }

    g_partsTbl = pni_buf;
    g_parts_info.u32_capacity = mtd->size;
    g_parts_info.u32_eraseSize = mtd->erasesize;
    g_parts_info.u32_writeSize = mtd->writesize;

    if (!_MDrv_PARTS_is_tbl_valid(pni_buf))
    {
        _MDRV_PARTS_load_parts_tbl(pni_buf);

        if (!_MDrv_PARTS_is_tbl_valid(pni_buf))
        {
            printf("[PARTS] Load table failure!\r\n");
            return -1;
        }
    }

    put_mtd_device(mtd);

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
********************************************************************************/
#if 0
void MDRV_PARTS_mark_active(u8 *pu8_PartName, u8 u8_Trunk)
{
    u8 u8_i = 0;
    u32 u32Count = 0;
    u32 u32Offset = 0;
    u32 u32_parts_size = 0;
    BOOL b_no_parts_tbl_exist;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_TBL_t st_partsTbl;
    PARTS_INFO_t *pstPartsInfo;

    pst_partsTbl = (PARTS_TBL_t*)g_partsTbl;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstPartsInfo = (PARTS_INFO_t*)(g_partsTbl + sizeof(PARTS_TBL_t));

    for(u8_i = 0; u8_i < u32Count; u8_i++)
    {
        if (_MDRV_PARTS_is_parts_match(pstPartsInfo, pu8_PartName))
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
    pst_partsTbl->u32_Checksum = _MDRV_PARTS_checksum((u32)&(pst_partsTbl->u32_Size), pst_partsTbl->u32_Size + sizeof(pst_partsTbl->u32_Size));
    b_no_parts_tbl_exist = 1;

    for (u8_i = MAX_CIS_BLOCK_NUM - 2; u8_i >= 0; u8_i -= 2)
    {
        u32Offset = u8_i * FLASH_BLOCK_SIZE;

        if (sizeof(PARTS_TBL_t) == MDRV_FLASH_read(u32Offset + FLASH_PAGE_SIZE, FLASH_PAGE_SIZE, (u32)&st_partsTbl, sizeof(PARTS_TBL_t)))
        {
            if (1 == MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic))
            {
                u32_parts_size = sizeof(PARTS_TBL_t) + pst_partsTbl->u32_Size;

                if (FLASH_BLOCK_SIZE != MDRV_FLASH_erase(u32Offset, FLASH_BLOCK_SIZE) ||
                    0 == MDRV_FLASH_info_recovery(u32Offset) ||
                    u32_parts_size != MDRV_FLASH_write_pages(u32Offset + FLASH_PAGE_SIZE, (u32)pst_partsTbl, u32_parts_size))
                {
                    MDRV_FLASH_mark_bad(u32Offset, RUN_TIME_BAD_BLOCK);
                    MDRV_BBT_fill_blk_info(u32Offset, g_u32_bbt_address, RUN_TIME_BAD_BLOCK);
                    continue;
                }
                b_no_parts_tbl_exist = FALSE;
            }
        }

        if (0 == u8_i)
        {
            break;
        }
    }
    MDRV_BBT_save_bbt(g_u32_bbt_address, FALSE);

    if(1 == b_no_parts_tbl_exist)
    {
        printf("No parts tbl\r\n");
        halt();
    }
}
#endif
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
u8 MDRV_PARTS_get_part_nm(u8 u8_partId, PARTS_INFO_t *pst_PartInfo)
{
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;

    if (!pst_PartInfo)
        return 1;

    pst_partsTbl = (PARTS_TBL_t*)g_partsTbl;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_partsTbl + sizeof(PARTS_TBL_t));

    if (u8_partId < u32Count)
    {
        memcpy(pst_PartInfo, &pstTemp[u8_partId], sizeof(PARTS_INFO_t));
    }
    else
    {
        return 1;
    }

    return 0;
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
u8 MDRV_PARTS_get_part(u8 *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_PartInfo)
{
    u8 u8_i;
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;

    if ((!pu8_PartName) || (!pst_PartInfo))
        return 0;

    pst_partsTbl = (PARTS_TBL_t*)g_partsTbl;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_partsTbl + sizeof(PARTS_TBL_t));

    for (u8_i = 0; u8_i < u32Count; u8_i++)
    {
        if (_MDRV_PARTS_is_parts_match(pstTemp, pu8_PartName))
        {
            if (u8_Trunk == pstTemp->u8_Trunk)
            {
                memcpy(pst_PartInfo, pstTemp, sizeof(PARTS_INFO_t));
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
*Function: MDRV_PARTS_get_active_part(u8 *pu8_PartName, PARTS_INFO_t *pst_PartInfo)
*Description:  According to partName,find the active partition
*Input:
        pu8_PartName: The partition name
        pst_PartInfo: The address to save partition info
*Output:
*Retrun:
        1 : Get active partition successful
        FALSE: Get active partition failed
********************************************************************************/
u8 MDRV_PARTS_get_active_part(u8 *pu8_PartName, PARTS_INFO_t *pst_PartInfo)
{
    u32 i = 0;
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp = NULL;

    if ((!pu8_PartName) || (!pst_PartInfo))
        return 0;
    pst_partsTbl = (PARTS_TBL_t*)g_partsTbl;
    pstTemp = (PARTS_INFO_t*)(g_partsTbl + sizeof(PARTS_TBL_t));
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    for (i = 0; i < u32Count; i++)
    {
        if ((pstTemp->u8_Active == 1) && _MDRV_PARTS_is_parts_match(pstTemp, pu8_PartName))
        {
            memcpy(pst_PartInfo, pstTemp, sizeof(PARTS_INFO_t));
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
*Function: MDRV_PARTS_load_part(PARTS_INFO_t *pst_PartInfo, u32 u32_address, u32 u32_size)
*Description: Load partition
*Input:
        pst_PartInfo: Find the partition to be loaded
        u32_address : The address to load partition
        u32_size    : The size of partition
*Output:
*Retrun:
        Success : The image's size
        Failed  : The image actually loaded size
********************************************************************************/
u32 MDRV_PARTS_load_part(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
{
    u8 u8_last_Trunk;
    u32 u32_load_offset;
    u32 u32_part_size;
    size_t retlen;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];
    if (!nand)
    {
        return -1;
    }

    if (!pst_PartInfo)
    {
        return 0;
    }

    u8_last_Trunk = -1;

load_image:

    u32_load_offset = u32_offset + pst_PartInfo->u32_Offset;
    retlen = u32_size;

    u32_part_size = pst_PartInfo->u32_Size - (u32_load_offset - pst_PartInfo->u32_Offset);

    if(!nand_read_skip_bad(nand, (loff_t)u32_load_offset, &retlen, NULL, u32_part_size, pu8_data))
    {
        return u32_size;
    }

    if (pst_PartInfo->u8_BackupTrunk != pst_PartInfo->u8_Trunk)
    {
        //If the partition isn't broken beacause of bbm,mark other trunk as active or not decided by customer
        if (-1 == u8_last_Trunk || u8_last_Trunk != pst_PartInfo->u8_BackupTrunk)
        {
            printf("Bad partiton,get the backup\r\n");
            MDRV_PARTS_get_part(pst_PartInfo->au8_partName, pst_PartInfo->u8_BackupTrunk, pst_PartInfo);

            if (-1 == u8_last_Trunk)
            {
                u8_last_Trunk = pst_PartInfo->u8_Trunk;
            }
            goto load_image;
        }
    }

    return 0;
}

/********************************************************************************
*Function: MDRV_PARTS_program_part(PARTS_INFO_t *pst_PartInfo, u32 u32_address, u32 u32_size)
*Description: Program partition
*Input:
        pst_PartInfo: The partition to be programmed
        u32_address : The partition data to be written
        u32_size    : The size of partition
*Output:
*Retrun:
        u32WrSize  : The size of the data being written
********************************************************************************/
u32 MDRV_PARTS_program_part(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
{
    u32 u32_load_offset;
    u32 u32_part_size;
    size_t retlen;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];
    if (!nand)
    {
        return -1;
    }

    if (!pst_PartInfo)
    {
        return 0;
    }

    u32_load_offset = u32_offset + pst_PartInfo->u32_Offset;
    retlen = u32_size;

    u32_part_size = pst_PartInfo->u32_Size - (u32_load_offset - pst_PartInfo->u32_Offset);

    if(!nand_write_skip_bad(nand, (loff_t)u32_load_offset, &retlen, NULL, u32_part_size, pu8_data, 0))
    {
        return u32_size;
    }

    return 0;
}
