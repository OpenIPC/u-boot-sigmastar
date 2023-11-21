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
#include <mdrvSpinandBbtBbm.h>
#include <mdrvParts.h>
#include <drvSPINAND.h>

#define    MAX_CIS_BLOCK_NUM        10
#define FLASH_PNI_PAGE_SIZE                 g_st_nand_info.u16_PageSize
#define FLASH_PNI_BLOCK_SIZE                g_st_nand_info.u32_BlockSize
#define FLASH_PNI_PAGES_PER_BLOCK           g_st_nand_info.u16_BlkPageCnt

static FLASH_NAND_INFO_t g_st_nand_info;

#if defined(CONFIG_FLASH_WRITE_BACK)
#include <mdrvFlash.h>

/********************************************************************************
*Function: _MDRV_PNI_adjust_onebin(PARTS_INFO_t *pst_PartsInfo,u8 u8_Trunk)
*Description: Adjust the partititon for onebin
*Input:
        pst_PartsInfo: The partition info need to be adjust
        u8_Trunk: The trunk number of partition
*Output:
*Retrun:
    TRUE : The table of onebin has been changed
    0: The table of onebin hasn't been changed
********************************************************************************/
static u8 _MDRV_PNI_adjust_onebin(u8 *pni_buf, u8 u8_Trunk)
{
    u8 u8_change = 0;
    u32 u32_i = 0,u32Count = 0;
    u32 u32_iplEnd = 0,u32_iplCustEnd = 0,u32_ubootStart = 0,u32_size = 0;
    u32 u32_check_start=0,u32_check_size=0,u32_bad_offset=0,u32_end = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstPartsInfo;
    PARTS_INFO_t stIplInfo,stIplCustInfo,stUbootInfo;

    pst_partsTbl = (PARTS_TBL_t*)pni_buf;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstPartsInfo = (PARTS_INFO_t*)(pni_buf + sizeof(PARTS_TBL_t));

    memset((void *)&stIplInfo, 0, sizeof(PARTS_INFO_t));
    memset((void *)&stIplCustInfo, 0, sizeof(PARTS_INFO_t));
    memset((void *)&stUbootInfo, 0, sizeof(PARTS_INFO_t));

    for(u32_i = 0; u32_i < u32Count; u32_i++)
    {
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"IPL") && (0 == pstPartsInfo->u8_Group))
        {
            memcpy(&stIplInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_iplEnd = pstPartsInfo->u32_Offset + pstPartsInfo->u32_Size;
        }
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"IPL_CUST") && (0 == pstPartsInfo->u8_Group))
        {
            memcpy(&stIplCustInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_iplCustEnd = pstPartsInfo->u32_Offset + pstPartsInfo->u32_Size;
        }
        if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"UBOOT") && (0 == pstPartsInfo->u8_Group))
        {
            memcpy(&stUbootInfo, pstPartsInfo, sizeof(PARTS_INFO_t));
            u32_size += pstPartsInfo->u32_Size;
            u32_ubootStart = pstPartsInfo->u32_Offset;
        }
        pstPartsInfo++;
    }
    if((u32_iplEnd != stIplCustInfo.u32_Offset) || (u32_iplCustEnd != stUbootInfo.u32_Offset)
        || 0 == u32_iplEnd || 0 == u32_iplCustEnd || 0 == u32_ubootStart)
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
            stIplInfo.u32_Offset += FLASH_PNI_BLOCK_SIZE;
        else
            stIplInfo.u32_Size += FLASH_PNI_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_PNI_BLOCK_SIZE;
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
            stIplCustInfo.u32_Offset += FLASH_PNI_BLOCK_SIZE;
        else
            stIplCustInfo.u32_Size += FLASH_PNI_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_PNI_BLOCK_SIZE;
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
            stUbootInfo.u32_Offset += FLASH_PNI_BLOCK_SIZE;

        u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
        u32_check_start = u32_bad_offset + FLASH_PNI_BLOCK_SIZE;
    }
    if(stUbootInfo.u32_Offset >= u32_end)//beyond the onebine range
    {
        stUbootInfo.u32_Offset = u32_end;
        stUbootInfo.u32_Size = 0;
    }

    if((u32_ubootStart != stUbootInfo.u32_Offset) && (u32_ubootStart != 0))
    {
        pstPartsInfo = (PARTS_INFO_t*)(pni_buf + sizeof(PARTS_TBL_t));
        for(u32_i = 0; u32_i < u32Count; u32_i++)
        {
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"IPL"))
            {
                memcpy(pstPartsInfo, &stIplInfo, sizeof(PARTS_INFO_t));
            }
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"IPL_CUST"))
            {
                memcpy(pstPartsInfo, &stIplCustInfo,sizeof(PARTS_INFO_t));
            }
            if ((pstPartsInfo->u8_Trunk == u8_Trunk) && !FLASH_IMPL_STRCMP(pstPartsInfo->au8_partName, (u8*)"UBOOT"))
            {
                memcpy(pstPartsInfo, &stUbootInfo,sizeof(PARTS_INFO_t));
            }
            pstPartsInfo++;
        }
        u8_change = 1;
        FLASH_IMPL_PRINTF_HEX("Change the table of onebin", u8_Trunk, "\r\n");
    }
    return u8_change;
}

static u8 _MDRV_PNI_adjust_group(u8 *pni_buf)
{
    u8 u8_change = 0;
    u8 u8_group = 0;
    u32 u32_i = 0,u32Count = 0;
    u32 u32_bad_offset = 0;
    u32 u32_badBlockSize = 0;
    u32 u32_check_start=0,u32_check_size=0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstPartsInfo;
    PARTS_INFO_t *pstTmpInfo = NULL;

    pst_partsTbl = (PARTS_TBL_t*)pni_buf;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstPartsInfo = (PARTS_INFO_t*)(pni_buf + sizeof(PARTS_TBL_t));

    for(u32_i = 0; u32_i < u32Count; u32_i++)
    {
        if ((0 != u8_group) && (u8_group != pstPartsInfo->u8_Group) && u32_badBlockSize)
        {
            do
            {
                if (pstTmpInfo->u32_Size >= u32_badBlockSize)
                {
                    pstTmpInfo->u32_Size -= u32_badBlockSize;
                    u32_badBlockSize = 0;
                }
                else
                {
                    u32_badBlockSize -= pstTmpInfo->u32_Size;
                    pstTmpInfo->u32_Size = 0;
                }

                pstTmpInfo--;
            } while(u32_badBlockSize && (u8_group == pstTmpInfo->u8_Group));

            u8_group = 0;
            u32_badBlockSize = 0;
            pstTmpInfo = NULL;
            u8_change = 1;
        }

        if (0 != pstPartsInfo->u8_Group)
        {
            if (u8_group != pstPartsInfo->u8_Group)
            {
                u8_group = pstPartsInfo->u8_Group;
            }
            else
            {
                pstPartsInfo->u32_Offset += u32_badBlockSize;
                FLASH_IMPL_PRINTF_HEX((const char *)pstPartsInfo->au8_partName, pstPartsInfo->u8_Trunk, NULL);
                FLASH_IMPL_PRINTF_HEX("  u32_Offset 0x", pstPartsInfo->u32_Offset, NULL);
                FLASH_IMPL_PRINTF_HEX("  u32_badBlockSize 0x", u32_badBlockSize, "\r\n");
            }

            pstTmpInfo = pstPartsInfo;
            u32_check_start = pstPartsInfo->u32_Offset;
            u32_check_size = pstPartsInfo->u32_Size;

            while (NO_ERR_BLOCK != (u32_bad_offset = MDRV_FLASH_get_bad(u32_check_start, u32_check_size)))
            {
                if(u32_bad_offset == pstPartsInfo->u32_Offset)//The first block of IPL_CUST is bad block
                    pstPartsInfo->u32_Offset += FLASH_PNI_BLOCK_SIZE;
                else
                    pstPartsInfo->u32_Size += FLASH_PNI_BLOCK_SIZE;

                u32_badBlockSize += FLASH_PNI_BLOCK_SIZE;

                u32_check_size = u32_check_size - (u32_bad_offset - u32_check_start);
                u32_check_start = u32_bad_offset + FLASH_PNI_BLOCK_SIZE;
            }
        }

        pstPartsInfo++;
    }

    if (pstTmpInfo && u32_badBlockSize)
    {
        pstTmpInfo->u32_Size -= u32_badBlockSize;
        u8_change = 1;
    }

    return u8_change;
}


/********************************************************************************
*Function: _MDRV_PNI_adjust_parts_tbl(u32 u32_address)
*Description: Adjust the PNI(partititon table) for onebin
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
********************************************************************************/
static void _MDRV_PNI_adjust_parts_tbl(u8 *pni_buf)
{
    u8 u8_ret = 0;
    PARTS_TBL_t *pst_partsTbl = (PARTS_TBL_t*)pni_buf;

    u8_ret |= _MDRV_PNI_adjust_onebin(pni_buf,0);
    u8_ret |= _MDRV_PNI_adjust_onebin(pni_buf,1);
    u8_ret |= _MDRV_PNI_adjust_group(pni_buf);

    if(u8_ret)
    {
        //when the pni is changed,the checksum should be recaculated
        pst_partsTbl->u32_Checksum = FLASH_IMPL_checksum((u8 *)(&(pst_partsTbl->u32_Size)), pst_partsTbl->u32_Size + sizeof(pst_partsTbl->u32_Size));
    }
}

static u8 _MDRV_PNI_scan_cis(u32 u32_pni_offset_bak, u8 *pni_buf)
{
    u32 u32_remain_size;
    u32 u32_read_size;
    u32 u32_offset = 0;
    if(g_st_nand_info.u8_BLPINB > 0 && g_st_nand_info.u8_BL0PBA < MAX_CIS_BLOCK_NUM)
    {
        u32_remain_size = FLASH_PNI_BLOCK_SIZE - 3 * (FLASH_PNI_PAGE_SIZE);  //block 0 contain CIS | PNI | IPL | sni list | PNI
    }
    else
    {
        u32_remain_size = 0;
    }
    while(u32_remain_size)
    {
        u32_read_size = u32_remain_size > FLASH_PNI_PAGE_SIZE ? FLASH_PNI_PAGE_SIZE : u32_remain_size;
        if (u32_read_size != MDRV_FLASH_read_skip_bad(u32_pni_offset_bak + u32_offset, FLASH_PNI_PAGE_SIZE, pni_buf + FLASH_PNI_PAGE_SIZE, u32_read_size))
        {
            return 1;
        }
        u32_offset += u32_read_size;
        u32_remain_size -= u32_read_size;
    }//check IPL region no ecc error
    return 0;
}


static u8 _MDRV_PNI_write_back_cis(u32 u32_pni_offset, u32 u32_pni_offset_bak, u8 *pni_buf)
{
    u8 u8_ret = 0;
    u32 u32_offset;
    u32 u32_remain_size;
    u32 u32_write_size;
    u32 u32_pni_offset_flag;

    if (MDRV_FLASH_block_isbad(u32_pni_offset))
        return u8_ret;

    if (FLASH_MAX_SNI_SIZE == MDRV_FLASH_info_recovery(u32_pni_offset) &&
        MAX_PARTS_TBL_SIZE == MDRV_FLASH_write(u32_pni_offset + FLASH_PNI_PAGE_SIZE, pni_buf, MAX_PARTS_TBL_SIZE))
    {
        FLASH_IMPL_PRINTF_HEX("[PARTS] Write table to 0x", u32_pni_offset + FLASH_PNI_PAGE_SIZE, "\r\n");
    }//Write back SNI and PNI

    //Init write back page offset and size.
    u32_offset = FLASH_PNI_PAGE_SIZE * 2;
    if(g_st_nand_info.u8_BLPINB > 0 && g_st_nand_info.u8_BL0PBA < MAX_CIS_BLOCK_NUM)
    {
        u32_remain_size = FLASH_PNI_BLOCK_SIZE - 3 * (FLASH_PNI_PAGE_SIZE);  //block 0 contain CIS | PNI | IPL | sni list | PNI
    }
    else
    {
        u32_remain_size = 0;
    }

    while(u32_remain_size)
    {
        u32_write_size = u32_remain_size > FLASH_PNI_PAGE_SIZE ? FLASH_PNI_PAGE_SIZE : u32_remain_size;

        if (u32_write_size != MDRV_FLASH_read_skip_bad(u32_pni_offset_bak + u32_offset, FLASH_PNI_PAGE_SIZE, pni_buf + FLASH_PNI_PAGE_SIZE, u32_write_size) ||
            u32_write_size != MDRV_FLASH_write(u32_pni_offset + u32_offset, pni_buf + FLASH_PNI_PAGE_SIZE, u32_write_size))
        {
            break;
        }

        u32_offset += u32_write_size;
        u32_remain_size -= u32_write_size;
    }//Write back IPL from the last odd block to update block in pni_buf + 4K.

    if(u32_remain_size != 0)
    {
        FLASH_IMPL_PRINTF_HEX("[PARTS]Error: Write IPL to 0x", u32_pni_offset + FLASH_PNI_PAGE_SIZE * 2, "\r\n");
        u8_ret =1;
    }

    u32_pni_offset_flag = ((u32_pni_offset + FLASH_PNI_BLOCK_SIZE) & (~(FLASH_PNI_BLOCK_SIZE - 1))) - FLASH_PNI_PAGE_SIZE;

    if (MAX_PARTS_TBL_SIZE != MDRV_FLASH_write(u32_pni_offset_flag, pni_buf, MAX_PARTS_TBL_SIZE))
    {
        FLASH_IMPL_PRINTF_HEX("[PARTS]Error: Write table to 0x", u32_pni_offset, "\r\n");
        u8_ret = 1;
    }//Write back a pni in last page to make sure that the last page is written correctly for prevent power off while writing

    return u8_ret;
}


/********************************************************************************
*Function: _MDRV_PNI_write_back_parts_tbl(u32 u32_address)
*Description: Read PNI from odd block,and write it back to even block
*Input:
        u32_address: Address to store partition table
*Output:
*Retrun:
        TRUE : Write back PNI successful
        0: Write back PNI failed
********************************************************************************/
static void _MDRV_PNI_write_back_parts_tbl(u8 *pni_buf)
{
    u32 u32_pni_offset;
    u32 u32_pni_offset_bak;
    PARTS_TBL_t st_partsTbl;

    FLASH_IMPL_PRINTF("[PARTS] Not found table, write back now!\r\n");

    u32_pni_offset_bak = MAX_CIS_BLOCK_NUM * FLASH_PNI_BLOCK_SIZE;

    do
    {
        u32_pni_offset_bak -= FLASH_PNI_BLOCK_SIZE;
        if ((u32_pni_offset_bak & FLASH_PNI_BLOCK_SIZE))
        {
            if (sizeof(PARTS_TBL_t) == MDRV_FLASH_read_skip_bad(u32_pni_offset_bak + FLASH_PNI_PAGE_SIZE, FLASH_PNI_BLOCK_SIZE, (u8 *)&st_partsTbl, sizeof(PARTS_TBL_t)))
            {
                if (MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic) && (0 == _MDRV_PNI_scan_cis(u32_pni_offset_bak, pni_buf)))
                {
                    break;    //Record last odd block which contains PNI.
                }
            }
        }
    } while (0 != u32_pni_offset_bak);

    u32_pni_offset = u32_pni_offset_bak;

//Update CIS from the last odd block - 1 to block 0
    do
    {
        if (!MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic))
        {
            break;
        }

        if (FLASH_PNI_PAGE_SIZE != MDRV_FLASH_read_skip_bad(u32_pni_offset_bak + FLASH_PNI_PAGE_SIZE, FLASH_PNI_BLOCK_SIZE, pni_buf, FLASH_PNI_PAGE_SIZE))
        {
            break;
        }

        _MDRV_PNI_adjust_parts_tbl(pni_buf); //Ajust the partiton table for boot image

        if (!MDrv_PARTS_is_tbl_valid(pni_buf))
        {
            break;
        }

        do
        {
            u32_pni_offset -= FLASH_PNI_BLOCK_SIZE;
            if (0 == (u32_pni_offset & FLASH_PNI_BLOCK_SIZE))
            {
                _MDRV_PNI_write_back_cis(u32_pni_offset, u32_pni_offset_bak, pni_buf);
            }
        } while(0 != u32_pni_offset);
    } while (0);
}

u8 MDRV_PNI_write_back(u8 *pni_buf)
{
    u32 u32_pni_offset;
    u32 u32_pni_offset_bak;
    u8 u8_no_parts_tbl_exist;
    PARTS_TBL_t st_partsTbl;

    u8_no_parts_tbl_exist = 1;

    u32_pni_offset_bak = MAX_CIS_BLOCK_NUM * FLASH_PNI_BLOCK_SIZE;

    do
    {
        u32_pni_offset_bak -= FLASH_PNI_BLOCK_SIZE;
        if ((u32_pni_offset_bak & FLASH_PNI_BLOCK_SIZE))
        {
            if (sizeof(PARTS_TBL_t) == MDRV_FLASH_read_skip_bad(u32_pni_offset_bak + FLASH_PNI_PAGE_SIZE, FLASH_PNI_BLOCK_SIZE, (u8 *)&st_partsTbl, sizeof(PARTS_TBL_t)))
            {
                if (MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic) && (0 == _MDRV_PNI_scan_cis(u32_pni_offset_bak, pni_buf)))
                {
                    break;    //Record last odd block which contains PNI.
                }
            }
        }
    } while (0 != u32_pni_offset_bak);

    u32_pni_offset = u32_pni_offset_bak;

//Update CIS from the last odd block - 1 to block 0
    do
    {
        u32_pni_offset -= FLASH_PNI_BLOCK_SIZE;
        if (0 == (u32_pni_offset & FLASH_PNI_BLOCK_SIZE))
        {
            if (sizeof(PARTS_TBL_t) == MDRV_FLASH_read_skip_bad(u32_pni_offset + FLASH_PNI_PAGE_SIZE, FLASH_PNI_PAGE_SIZE, (u8 *)&st_partsTbl, sizeof(PARTS_TBL_t))
                && (MDRV_PARTS_is_tbl_header(st_partsTbl.au8_magic)))
            {

                if (MDRV_FLASH_get_bad(u32_pni_offset, FLASH_PNI_BLOCK_SIZE))
                {
                    MDRV_FLASH_mark_bad(u32_pni_offset, RUN_TIME_BAD_BLOCK);
                    MDRV_BBT_fill_blk_info(u32_pni_offset, RUN_TIME_BAD_BLOCK);
                    continue;
                }
                _MDRV_PNI_write_back_cis(u32_pni_offset, u32_pni_offset_bak, pni_buf);
                u8_no_parts_tbl_exist = 0;
            }
        }
    } while(0 != u32_pni_offset);

    MDRV_BBT_save_bbt(0);

    if(u8_no_parts_tbl_exist)
    {
        FLASH_IMPL_PRINTF("No parts tbl\r\n");
        return 1;
    }

    return 0;
}

#else
#define _MDRV_PNI_write_back_parts_tbl(x)
u8 MDRV_PNI_write_back(u8 *pni_buf)
{
    FLASH_IMPL_UNUSED_VAR(pni_buf);
    return 0;
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
        0: Init failed
********************************************************************************/
static void _MDRV_PNI_load_parts_tbl(u8 *pni_buf)
{
    u32 u32_pni_page;
    u32 u32_pni_end;

    u32_pni_page = 0x0;
    u32_pni_end = FLASH_PNI_PAGES_PER_BLOCK * 10;

    for (; u32_pni_page != u32_pni_end; u32_pni_page += FLASH_PNI_PAGES_PER_BLOCK)
    {
        if (0 == (u32_pni_page & FLASH_PNI_PAGES_PER_BLOCK))
        {
            if ((!mdrv_spinand_block_isbad(u32_pni_page)) &&
                (ERR_SPINAND_SUCCESS == mdrv_spinand_page_read(u32_pni_page + FLASH_PNI_PAGES_PER_BLOCK - 1, 0, pni_buf, MAX_PARTS_TBL_SIZE)))
            {
                if (MDrv_PARTS_is_tbl_valid(pni_buf)) // Make sure the last page is written correctly for prevent power off while writing
                {
                    FLASH_IMPL_PRINTF_HEX("[PARTS] Found table in 0x", u32_pni_page + 1, "\r\n");
                    return;
                }
                break;
            }
        }
    }

    _MDRV_PNI_write_back_parts_tbl(pni_buf);
}

/********************************************************************************
*Function: MDRV_PNI_init_tlb(u32 u32_address)
*Description:  Init the PNI(partititon table) and BBT(bad block table)
*Input:
       u32_address: Address to store pni and bbt for share information
*Output:
*Retrun:
********************************************************************************/
u8 MDRV_PNI_init_tlb(u8 *pni_buf)
{
    mdrv_spinand_info(&g_st_nand_info);

    if (0 == g_st_nand_info.u32_Capacity)
    {
        FLASH_IMPL_PRINTF("No flash in board!\r\n");
        return 1;
    }

    _MDRV_PNI_load_parts_tbl(pni_buf);

    if (!MDrv_PARTS_is_tbl_valid(pni_buf))
    {
        FLASH_IMPL_PRINTF("[PARTS] Load table failure!\r\n");
        return 1;
    }

    return 0;
}
