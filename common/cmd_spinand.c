/*
 * cmd_spinand.c- Sigmastar
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
#include <command.h>
#include <malloc.h>
#include <mdrvParts.h>
#include <nand.h>
#include <mdrvSNI.h>
#include <ubi_uboot.h>
#include <drvSPINAND.h>

typedef struct {
    char vol_name[16];
    u32 vol_size;
} SPINAND_UBI_VOL;

typedef struct {
    char *ubi_name[16];
    u8 ubivol_count;
    SPINAND_UBI_VOL spinand_ubivol[8];
} SPINAND_UBI_DEV;

static u8 *g_bin_buf = NULL;
static SPINAND_INFO_t *g_spinand_info = NULL;
static PARTS_TBL_t *g_part_tbl = NULL;
static PARTS_INFO_t g_boot_part;
static u8 g_ubi_dev_index;
static SPINAND_UBI_DEV g_spinand_ubi_dev[8];

extern void ubi_dev_remove(void);

static void _flash_show_id(u8* au8_id)
{
    printk("[FLASH] ID : 0x%02x 0x%02x 0x%02x\n", au8_id[0], au8_id[1], au8_id[2]);
}

static u8 _flash_is_id_match(u8* au8_id, u8 u8_id_len)
{
    u8 u8_i;
    u8 au8_device_id[8];
    if (ERR_SPINAND_SUCCESS != mdrv_spinand_read_id(au8_device_id, u8_id_len))
    {
        printf("[FLASH] read device id fail!\r\n");
        return 0;
    }
    for(u8_i = 0; u8_id_len > u8_i; u8_i++)
    {
        if (au8_device_id[u8_i] != au8_id[u8_i])
        {
            printk("[FLASH] No matched ID\n");
            _flash_show_id(au8_device_id);
            return 0;
        }
    }
    return 1;
}

static u8 spi_nand_is_match(u8 *sni_buf)
{
    SPINAND_SNI_t *pst_sni = NULL;
    SPINAND_INFO_t *pst_info = NULL;

    pst_sni = (SPINAND_SNI_t *)sni_buf;
    pst_info = &pst_sni->spinand_info;

    return _flash_is_id_match(pst_info->au8_ID, pst_info->u8_IDByteCnt);
}

int _spinand_loadbin_ubi(PARTS_INFO_t *pst_PartInfo, u8 *pu8_data)
{
    u8 i;
    struct ubi_volume *vol = NULL;
    struct ubi_device *spinand_ubi = NULL;
    char ubi_cmd[64];

    memset((void *)ubi_cmd, 0, 64);
    sprintf(ubi_cmd, "ubi part %s", pst_PartInfo->au8_partName);
    printf("[%s]\n", ubi_cmd);
    run_command(ubi_cmd, 0);

    spinand_ubi = ubi_devices[0];

    for (i = 0; i < spinand_ubi->vtbl_slots; i++)
    {
        vol = spinand_ubi->volumes[i];

        if (vol)
        {
            memset((void *)ubi_cmd, 0, 64);
            sprintf(ubi_cmd, "ubi read 0x%lx %s 0x%llx", (unsigned long)pu8_data, vol->name, vol->used_bytes);
            printf("[%s]\n", ubi_cmd);
            run_command(ubi_cmd, 0);

            memcpy((void *)(g_spinand_ubi_dev[g_ubi_dev_index].spinand_ubivol[i].vol_name), (const void *)(vol->name), vol->name_len);
            //g_spinand_ubi_dev[g_ubi_dev_index].spinand_ubivol[i].vol_size = vol->used_ebs * (nand->erasesize - (2 * nand->writesize));
            g_spinand_ubi_dev[g_ubi_dev_index].spinand_ubivol[i].vol_size = vol->used_bytes;
            g_spinand_ubi_dev[g_ubi_dev_index].ubivol_count = i+1;;
            pu8_data += vol->used_bytes;
        }
    }

    memcpy((void *)(g_spinand_ubi_dev[g_ubi_dev_index].ubi_name), (const void *)(pst_PartInfo->au8_partName), 16);
    g_ubi_dev_index++;

    return 0;
}

u32 _spinand_load_part(PARTS_INFO_t *pst_PartInfo, u32 u32_offset, u8 *pu8_data, u32 u32_size)
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

static int _spinand_loadbin(PARTS_INFO_t *pst_PartInfo, u8 *pu8_data)
{
    u32 u32_offset;
    u32 u32_size;
    size_t retlen;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];
    

    for (u32_offset=pst_PartInfo->u32_Offset, u32_size=pst_PartInfo->u32_Size; u32_offset < u32_size; u32_offset+=nand->erasesize)
    {
        if (nand_block_isbad(nand, (loff_t)u32_offset))
            retlen -= nand->erasesize;
    }

    retlen = nand->erasesize << 1;

    if (retlen < pst_PartInfo->u32_Size && nand->writesize == _spinand_load_part(pst_PartInfo, 0, pu8_data, nand->writesize))
    {
        if (!strncmp((const char *)pu8_data, "UBI", 3))
        {
            return _spinand_loadbin_ubi(pst_PartInfo, pu8_data);
        }
        else if (nand->writesize == _spinand_load_part(pst_PartInfo, nand->erasesize, pu8_data, nand->writesize) &&
            !strncmp((const char *)pu8_data, "UBI", 3))
        {
            return _spinand_loadbin_ubi(pst_PartInfo, pu8_data);
        }
    }

    retlen = pst_PartInfo->u32_Size;

    if (retlen != _spinand_load_part(pst_PartInfo, 0, pu8_data, retlen))
    {
        return 1;
    }

    return 0;
}

static int _spinand_loadbin_cis(PARTS_INFO_t *pst_PartInfo, u8 *pu8_data)
{
    u32 u32_offset;
    u32 u32_end;
    int ret = 0;
    nand_info_t *nand;
    SPINAND_SNI_t *sni_info;

    nand = &nand_info[nand_curr_device];
    pst_PartInfo->u8_Trunk = 0;
    pst_PartInfo->u8_BackupTrunk = 0;
    u32_end = pst_PartInfo->u32_Size + pst_PartInfo->u32_Offset;

    for (u32_offset=pst_PartInfo->u32_Offset; u32_offset < u32_end; u32_offset+=nand->erasesize)
    {
        if (!(u32_offset & nand->erasesize) && !nand_block_isbad(nand, (loff_t)u32_offset) &&
            nand->erasesize == _spinand_load_part(pst_PartInfo, u32_offset, pu8_data, nand->erasesize))
        {
            ret = 1;
            break;
        }
    }

    if (!ret)
        return 1;

    for (u32_offset=nand->erasesize; u32_offset<pst_PartInfo->u32_Size; u32_offset+=nand->erasesize)
        memcpy((void *)(pu8_data + u32_offset), (const void *)pu8_data, nand->erasesize);

    sni_info = (SPINAND_SNI_t *)pu8_data;
    g_spinand_info = &sni_info->spinand_info;
    g_part_tbl = (PARTS_TBL_t *)(pu8_data + nand->writesize);

    return 0;
}

u8 _spinand_get_part(const char *pu8_PartName, u8 u8_Trunk, PARTS_INFO_t *pst_PartInfo)
{
    u8 u8_i;
    u32 u32Count = 0;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;

    if ((!pu8_PartName) || (!pst_PartInfo))
        return 0;

    pst_partsTbl = (PARTS_TBL_t*)g_part_tbl;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_part_tbl + sizeof(PARTS_TBL_t));

    for (u8_i = 0; u8_i < u32Count; u8_i++)
    {
        if (!strcmp((const char *)(pstTemp->au8_partName), pu8_PartName))
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

static u32 _spinand_checksum(u32 u32_address, u32 u32_size)
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

static void _spinand_adjust_parts_tbl(void)
{
    u8 u8_i;
    u32 u32Count = 0;
    u32 u32_offset;
    u32 u32_size;
    PARTS_TBL_t *pst_partsTbl;
    PARTS_INFO_t *pstTemp;
    PARTS_INFO_t stOld;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];

    pst_partsTbl = (PARTS_TBL_t*)g_part_tbl;
    u32Count = pst_partsTbl->u32_Size / sizeof(PARTS_INFO_t);
    pstTemp = (PARTS_INFO_t*)(g_part_tbl + sizeof(PARTS_TBL_t));

    for (u8_i = 0; u8_i < u32Count; u8_i++)
    {
        if (!strcmp((const char *)(pstTemp->au8_partName), "IPL"))
        {
            if (0 == pstTemp->u8_Trunk)
                pstTemp->u32_Offset = g_spinand_info->u8_BL0PBA * nand->erasesize;
            else
                pstTemp->u32_Offset = g_spinand_info->u8_BL1PBA * nand->erasesize;

            MDRV_PARTS_get_part(pstTemp->au8_partName, pstTemp->u8_Trunk, &stOld);

            for (u32_offset=stOld.u32_Offset, u32_size=stOld.u32_Size; u32_offset < u32_size; u32_offset+=nand->erasesize)
            {
                if (nand_block_isbad(nand, (loff_t)u32_offset))
                {
                    stOld.u32_Size -= nand->erasesize;
                }
            }

            pstTemp->u32_Size = stOld.u32_Size;

            if (pstTemp->u32_Offset != stOld.u32_Offset)
            {
                memcpy((void *)(g_bin_buf+pstTemp->u32_Offset), (const void *)(g_bin_buf+stOld.u32_Offset), pstTemp->u32_Size);
            }
        }
        else if(!strcmp((const char *)(pstTemp->au8_partName), "IPL_CUST") || !strcmp((const char *)(pstTemp->au8_partName), "UBOOT"))
        {
            if (!strcmp((const char *)(pstTemp->au8_partName), "IPL_CUST"))
                _spinand_get_part("IPL", pstTemp->u8_Trunk, &stOld);
            else
                _spinand_get_part("IPL_CUST", pstTemp->u8_Trunk, &stOld);

            pstTemp->u32_Offset = stOld.u32_Offset + stOld.u32_Size;

            MDRV_PARTS_get_part(pstTemp->au8_partName, pstTemp->u8_Trunk, &stOld);

            for (u32_offset=stOld.u32_Offset, u32_size=stOld.u32_Size; u32_offset < u32_size; u32_offset+=nand->erasesize)
            {
                if (nand_block_isbad(nand, (loff_t)u32_offset))
                {
                    stOld.u32_Size -= nand->erasesize;
                }
            }

            pstTemp->u32_Size = stOld.u32_Size;
            if (pstTemp->u32_Offset != stOld.u32_Offset)
            {
                memcpy((void *)(g_bin_buf+pstTemp->u32_Offset), (const void *)(g_bin_buf+stOld.u32_Offset), pstTemp->u32_Size);
            }
        }

        pstTemp++;
    }

    pst_partsTbl->u32_Checksum = _spinand_checksum((u32)&(pst_partsTbl->u32_Size), pst_partsTbl->u32_Size + sizeof(pst_partsTbl->u32_Size));

    MDRV_PARTS_init_tlb((u8 *)g_part_tbl);
}

static int spinand_loadbin(void)
{
    u8 u8_partId = 0;
    int ret;
    PARTS_INFO_t st_PartInfo;

    do
    {
        ret = MDRV_PARTS_get_part_nm(u8_partId, &st_PartInfo);

        if (ret)
        {
            printf("[SPINAND] Load %s form 0x%x to 0x%lx\n", st_PartInfo.au8_partName, st_PartInfo.u32_Offset, (unsigned long)(g_bin_buf + st_PartInfo.u32_Offset));
            if (!strcmp((const char *)(st_PartInfo.au8_partName), "CIS"))
            {
                ret = _spinand_loadbin_cis(&st_PartInfo, g_bin_buf + st_PartInfo.u32_Offset);
            }
            else
            {
                ret = _spinand_loadbin(&st_PartInfo, g_bin_buf + st_PartInfo.u32_Offset);
            }

            if (ret)
                return 1;
        }

        u8_partId++;
    } while(ret);

    _spinand_adjust_parts_tbl();

    return 0;
}

int _spinand_savebin_ubi(PARTS_INFO_t *pst_PartInfo, u8 *pu8_data, u8 ubi_dev)
{
    u8 i;
    char ubi_cmd[64];

    memset((void *)ubi_cmd, 0, 64);
    sprintf(ubi_cmd, "ubi part %s", pst_PartInfo->au8_partName);
    printf("[%s]\n", ubi_cmd);
    run_command(ubi_cmd, 0);

    for (i=0; i<g_spinand_ubi_dev[ubi_dev].ubivol_count; i++)
    {
        if (g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_size)
        {
            memset((void *)ubi_cmd, 0, 64);
            sprintf(ubi_cmd, "ubi create %s 0x%x", g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_name,
                g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_size);
            printf("[%s]\n", ubi_cmd);
            run_command(ubi_cmd, 0);
            memset((void *)ubi_cmd, 0, 64);
            sprintf(ubi_cmd, "ubi write 0x%lx %s 0x%x", (unsigned long)pu8_data,
                g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_name,
                g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_size);
            printf("[%s]\n", ubi_cmd);
            run_command(ubi_cmd, 0);

            pu8_data += g_spinand_ubi_dev[ubi_dev].spinand_ubivol[i].vol_size;
        }
    }

    return 0;
}

static int _spinand_savebin(PARTS_INFO_t *pst_PartInfo, u8 *pu8_data)
{
    u32 u32_offset;
    u32 u32_size;
    size_t retlen;
    nand_info_t *nand;

    nand = &nand_info[nand_curr_device];

    if (!strcmp((const char *)(pst_PartInfo->au8_partName), "IPL"))
    {
        g_boot_part.u32_Offset = pst_PartInfo->u32_Offset;
        g_boot_part.u32_Size = pst_PartInfo->u32_Size;
        return 0;
    }
    else if (!strcmp((const char *)(pst_PartInfo->au8_partName), "IPL_CUST"))
    {
        g_boot_part.u32_Size += pst_PartInfo->u32_Size;
        return 0;
    }
    else if (!strcmp((const char *)(pst_PartInfo->au8_partName), "UBOOT"))
    {
        pu8_data -= g_boot_part.u32_Size;
        g_boot_part.u32_Size += pst_PartInfo->u32_Size;
        pst_PartInfo = &g_boot_part;
    }

    retlen = pst_PartInfo->u32_Size;

    for (u32_offset=pst_PartInfo->u32_Offset, u32_size=pst_PartInfo->u32_Size; u32_offset < u32_size; u32_offset+=nand->erasesize)
    {
        if (nand_block_isbad(nand, (loff_t)u32_offset))
            retlen -= nand->erasesize;
    }

    if(0 != nand_write_skip_bad(nand, (loff_t)(pst_PartInfo->u32_Offset), &retlen, NULL, pst_PartInfo->u32_Size, pu8_data, 0))
    {
        return 1;
    }

    return 0;
}

static int spinand_savebin(void)
{
    u8 u8_partId = 0;
    u8 i;
    int ret;
    PARTS_INFO_t st_PartInfo;
    SPINAND_SNI_t *sni_info;

    if (!g_bin_buf)
    {
        printk("[SPINAND] No bin\n");
        return 0;
    }

    if (!spi_nand_is_match(g_bin_buf))
    {
        sni_info = (SPINAND_SNI_t *)g_bin_buf;
        printk("[SPINAND] Please change to %s 0x%02x 0x%02x 0x%02x\n", sni_info->au8_partnumber, g_spinand_info->au8_ID[0],
            g_spinand_info->au8_ID[1], g_spinand_info->au8_ID[2]);
        return 0;
    }

    ubi_dev_remove();

    if (!IS_ERR(get_mtd_device_nm("nand0")))
    {
        nand_init();
    }

    run_command("nand erase.chip", 0);

    do
    {
        ret = MDRV_PARTS_get_part_nm(u8_partId++, &st_PartInfo);

        printf("[SPINAND] Save %s form 0x%lx to 0x%x\n", st_PartInfo.au8_partName, (unsigned long)(g_bin_buf + st_PartInfo.u32_Offset), st_PartInfo.u32_Offset);
        if (!ret)
        {
            for (i=0; i<g_ubi_dev_index; i++)
            {
                if (!strcmp((const char *)(st_PartInfo.au8_partName), (const char *)(g_spinand_ubi_dev[i].ubi_name)))
                {
                    return _spinand_savebin_ubi(&st_PartInfo, g_bin_buf + st_PartInfo.u32_Offset, i);
                }
            }

            ret = _spinand_savebin(&st_PartInfo, g_bin_buf + st_PartInfo.u32_Offset);

            if (ret)
                return 1;
        }
    } while(!ret);

    return 0;
}

static int do_spinand(cmd_tbl_t *cmdtp, int flag, int argc,
		       char * const argv[])
{
    const char *cmd;
    int ret = -1;
    char *endptr;;

    /* need at least two arguments */
    if (argc < 2)
        goto usage;

    cmd = argv[1];

    if (strcmp(cmd, "probe") == 0)
    {
        ret = 0;
        goto done;
    }

    if (strcmp(cmd, "remove") == 0)
    {
        ret = 0;
    }

    if (strcmp(cmd, "savebin") == 0)
    {
        ret = spinand_savebin();
        goto done;
    }

    if (argc >= 3 && strcmp(cmd, "loadbin") == 0)
    {
        g_bin_buf = (u8 *)simple_strtoul(argv[2], &endptr, 16);
        g_ubi_dev_index = 0;
        memset((void *)g_spinand_ubi_dev, 0, sizeof(g_spinand_ubi_dev));
        ret = spinand_loadbin();

        goto done;
    }

done:
    return ret;

usage:
    return CMD_RET_USAGE;
}

U_BOOT_CMD(
    spinand,  CONFIG_SYS_MAXARGS,    1,    do_spinand,
    "spinand sub-system",
    "spinand probe\n"
    "spinand remove\n"
    "spinand loadbin address\n"
    "spinand savebin\n"
);
