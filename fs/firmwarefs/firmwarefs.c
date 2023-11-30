/*
* firmwarefs.c- Sigmastar
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
/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/


/*
 * firmwarefs.c
 */
#include <inttypes.h>
#include <stdlib.h>
#include <linux/mtd/mtd.h>
#include <fwfs.h>
#include <firmwarefs.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#include <common.h>

#if defined(CONFIG_MS_SPINAND)
#include <nand.h>
#include <mdrvSpinandBbtBbm.h>
#else
static struct spi_flash *spinor_flash;
#endif

#if defined(CONFIG_FS_FIRMWAREFS)

#define BBT_MAP_INVALID (0xffffffff)

typedef struct
{
    fwfs_t fwfs;
    struct fwfs_config cfg;
} FirmwarefsMntHandle;

typedef struct
{
    fwfs_t *fwfs_file;
    fwfs_file_t fwfs_fd;
} FirmwarefsFileHandle;


extern int printf(const char *fmt, ...);
extern int get_mtdpart_range(char *partition, u64 *offset, u64 *size);

static FirmwarefsMntHandle _gFwfsMntHandle;

#if defined(CONFIG_MS_SPINAND)
static void block_device_bbt_map_build(struct fwfs_config *c)
{
    int flag;
    fwfs_block_t bad_num = 0;
    fwfs_block_t *maps;
    fwfs_block_t j = 0;

    maps = (fwfs_block_t *)calloc(sizeof(fwfs_block_t), c->block_count);
    if (maps == NULL)
        return;

    for (fwfs_block_t i = 0; i < c->block_count; i++)
    {
        flag = c->bbt(c, i);
        if (flag != FACTORY_BAD_BLOCK)
        {
            maps[j++] = i;
        } else
        {
            printf("%s: block 0x%"PRIx32" is a factory bad block (0x%x)\n",
                        __FUNCTION__, i, flag);
            bad_num++;
        }
    }

    for (; j < c->block_count; j++)
    {
        maps[j] = BBT_MAP_INVALID;
    }

#if 0
    printf("BBT map table begin:\n");
    for (j = 0; j < c->block_count; j++)
    {
        if (j != 0 && (j % 8) == 0)
            printf("\n");
        printf("0x%08"PRIx32",0x%08"PRIx32";",
                    j, maps[j]);
    }
    printf("\nBBT map table end:\n");
#endif

    if (bad_num == 0)
    {
        free(maps);
        return;
    }
    c->context = (void *)maps;
}

static fwfs_block_t block_device_bbt_map(const struct fwfs_config *c, fwfs_block_t block) {
    fwfs_block_t nblock = block;
    fwfs_block_t *maps = (fwfs_block_t *)c->context;

    if (maps == NULL)
        return block;

    nblock = maps[nblock];
    return nblock;
}
#endif

//block device operations
static int block_device_read(const struct fwfs_config *c, fwfs_block_t block, fwfs_off_t off, void *buffer, fwfs_size_t size)
{
    U32 u32Addr;
#if defined(CONFIG_MS_SPINAND)
    nand_info_t *nand;
    fwfs_size_t retlen;

    block = block_device_bbt_map(c, block);
    if (block == BBT_MAP_INVALID)
        return -EIO;
    u32Addr = c->block_size * (block + c->block_offset) + off;

    nand = &nand_info[0];
    return mtd_read(nand, u32Addr, size, &retlen, buffer);
#else
    u32Addr = c->block_size * (block + c->block_offset) + off;
    return spi_flash_read(spinor_flash, u32Addr, size, buffer);
#endif
}

static int block_device_prog(const struct fwfs_config *c, fwfs_block_t block,
                             fwfs_off_t off, const void *buffer, fwfs_size_t size)
{
    u32 u32Addr;
#if defined(CONFIG_MS_SPINAND)
    nand_info_t *nand;
    fwfs_size_t retlen;

    block = block_device_bbt_map(c, block);
    if (block == BBT_MAP_INVALID)
        return -EIO;
    u32Addr = c->block_size * (block + c->block_offset) + off;

    nand = &nand_info[0];
    return mtd_write(nand, u32Addr, size, &retlen, buffer);
#else
    u32Addr = c->block_size * (block + c->block_offset) + off;
    return spi_flash_write(spinor_flash, u32Addr, size, buffer);
#endif
}

static int block_device_erase(const struct fwfs_config *c, fwfs_block_t block)
{
    u32 u32Addr;

#if defined(CONFIG_MS_SPINAND)
    struct erase_info instr;
    nand_info_t *nand;

    block = block_device_bbt_map(c, block);
    if (block == BBT_MAP_INVALID)
        return -EIO;
    u32Addr = c->block_size * (block + c->block_offset);

    nand = &nand_info[0];
    instr.mtd = nand;
    instr.addr = u32Addr;
    instr.len = c->block_size;
    instr.callback = 0;
    return mtd_erase(nand, &instr);
#else
    u32Addr = c->block_size * (block + c->block_offset);
    return spi_flash_erase(spinor_flash, u32Addr, c->block_size);
#endif
}


static int block_device_sync(const struct fwfs_config *c)
{
    //CamOsPrintf("### block_device_sync\n");
    return 0;
}

static int block_device_bbt(const struct fwfs_config *c, fwfs_block_t block) {
#if defined(CONFIG_MS_SPINAND)
    U32 u32Addr;

    u32Addr = c->block_size * (block + c->block_offset);
    return MDRV_BBT_get_blk_info(u32Addr);
#else
    return 0;
#endif
}

#if defined(CONFIG_FIRMWAREFS_ON_MMC)
FirmwarefsDevAndPartContext *firmwarefs_find_partition(char *partition) {
    int part;
    char dev_part_str[64];
    char *dev_env;
    int i;
    bool found = false;
    FirmwarefsDevAndPartContext *context;

    context = (FirmwarefsDevAndPartContext *)calloc(sizeof(FirmwarefsDevAndPartContext), 1);
    if (context == NULL)
        return NULL;

#if defined(CONFIG_FIRMWAREFS_ON_EMMC_FLASH)
    for (i = 1; i <= 20; i++) {
        snprintf(dev_part_str, sizeof(dev_part_str), "0:%d", i);
        part = get_device_and_partition("mmc", dev_part_str, &context->dev_desc,
                                        &context->info, 1);
        if (part < 0)
            break;

        if ((strcmp((char *)context->info.name, partition) == 0) ||
            (strcmp(partition, "MISC") == 0 &&
             strcmp((char *)context->info.name, "misca") == 0)) {
            found = true;
            break;
        }
    }
#else
    dev_env = getenv(PARTITION_MISC_INDEX);
    i = simple_strtoull(dev_env, NULL, 10);
    snprintf(dev_part_str, sizeof(dev_part_str), "0:%d", i);
    part = get_device_and_partition("mmc", dev_part_str, &context->dev_desc, &context->info, 1);
    if(part < 0)
        found = false;
    else
        found = true;
#endif
    if (!found) {
        free(context);
        context = NULL;
    }
    return context;
}
#endif

S32  firmwarefs_mount(char *partition, char *mnt_path)
{
#if defined(CONFIG_MS_SPINAND)
    U32 FIRMWAREFS_CACHE_SIZE                     = 2048;
    U32 FIRMWAREFS_FILE_CACHE_SIZE                = 8192;
    U32 FIRMWAREFS_READ_SIZE                      = 2048;
    U32 FIRMWAREFS_PROG_SIZE                      = 2048;
    U32 FIRMWAREFS_BLK_SIZE                       = 131072;
    U32 FIRMWAREFS_SUBBLK_SIZE                    = 32768;
    U32 FIRMWAREFS_CACHE_POOL_SIZE                = 8;
    struct mtd_info *mtd;
#else
    U32 FIRMWAREFS_CACHE_SIZE                     = 2048;
    U32 FIRMWAREFS_FILE_CACHE_SIZE                = 2048;
    U32 FIRMWAREFS_READ_SIZE                      = 2048;
    U32 FIRMWAREFS_PROG_SIZE                      = 2048;
    U32 FIRMWAREFS_BLK_SIZE                       = 65536;
    U32 FIRMWAREFS_SUBBLK_SIZE                    = 32768;
    U32 FIRMWAREFS_CACHE_POOL_SIZE                = 4;
#endif
    u64 part_offset;
    u64 part_size;
    int err = 0;

    memset(&_gFwfsMntHandle.cfg.fwfs_partition, 0x0,
           sizeof(_gFwfsMntHandle.cfg.fwfs_partition));
#if defined(CONFIG_MS_SPINAND)
    mtd = get_mtd_device(NULL, 0);
    if (mtd)
    {
        FIRMWAREFS_BLK_SIZE   = mtd->erasesize;
        FIRMWAREFS_CACHE_SIZE = mtd->writesize;
        FIRMWAREFS_FILE_CACHE_SIZE = mtd->writesize * 4;
        FIRMWAREFS_READ_SIZE = mtd->writesize;
        FIRMWAREFS_PROG_SIZE = mtd->writesize;

        put_mtd_device(mtd);
        mtd = NULL;
    }
#else
    spinor_flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS,
                                   CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
    if (!spinor_flash)
    {
        printf("spi_flash_probe return NULL\n");
        return -1;
    }
#endif

    _gFwfsMntHandle.cfg.fwfs_partition.u8_UseExternBlockSize = 0;
    if (!get_mtdpart_range(partition, &part_offset, &part_size))
    {
        printf("get_mtdpart_range return error\n");
        return -1;
    }

    //block device operations
    _gFwfsMntHandle.cfg.read = block_device_read;
    _gFwfsMntHandle.cfg.prog = block_device_prog;
    _gFwfsMntHandle.cfg.erase = block_device_erase;
    _gFwfsMntHandle.cfg.sync = block_device_sync;
    _gFwfsMntHandle.cfg.bbt = block_device_bbt;

    //block device configuration
     //spinad need set flash page size, value is 2048, spinor not limit
    _gFwfsMntHandle.cfg.page_size = FIRMWAREFS_PROG_SIZE;
    // block size is a multiple of cache size
    _gFwfsMntHandle.cfg.block_size = FIRMWAREFS_BLK_SIZE;
    _gFwfsMntHandle.cfg.block_count = ((u32)part_size) / _gFwfsMntHandle.cfg.block_size;
    _gFwfsMntHandle.cfg.block_offset = ((u32)part_offset) / _gFwfsMntHandle.cfg.block_size;
    _gFwfsMntHandle.cfg.subblock_size = FIRMWAREFS_SUBBLK_SIZE;

    _gFwfsMntHandle.cfg.read_size = FIRMWAREFS_READ_SIZE;
    _gFwfsMntHandle.cfg.cache_pool_size = FIRMWAREFS_CACHE_POOL_SIZE;
    //cache size is a multiple of read sizes
    _gFwfsMntHandle.cfg.cache_size = FIRMWAREFS_CACHE_SIZE;
    _gFwfsMntHandle.cfg.prog_size = FIRMWAREFS_PROG_SIZE;
    //Must be a multiple of the read sizes
    _gFwfsMntHandle.cfg.file_cache_size = FIRMWAREFS_FILE_CACHE_SIZE;
    _gFwfsMntHandle.cfg.block_cycles = 500;
    _gFwfsMntHandle.cfg.lookahead_size = 8;

#if defined(CONFIG_MS_SPINAND)
    block_device_bbt_map_build(&_gFwfsMntHandle.cfg);
#endif

    // mount the filesystem
    err = fwfs_mount(&_gFwfsMntHandle.fwfs, &_gFwfsMntHandle.cfg);

    return err;
}

void firmwarefs_unmount(void)
{
    fwfs_unmount(&_gFwfsMntHandle.fwfs);
#if defined(CONFIG_MS_SPINAND)
    if (_gFwfsMntHandle.cfg.context != NULL)
    {
        free(_gFwfsMntHandle.cfg.context);
        _gFwfsMntHandle.cfg.context = NULL;
    }
#else
    if (spinor_flash != NULL)
    {
        spi_flash_free(spinor_flash);
        spinor_flash = NULL;
    }
#endif
}

void* firmwarefs_open(char *filename, U32 flags, U32 mode)
{
    int err;
    int nflags = 0x0;
    void *ret = NULL;
    FirmwarefsFileHandle *fd = NULL;

    fd = (FirmwarefsFileHandle *)calloc(sizeof(FirmwarefsFileHandle), 1);
    if (!fd)
    {
        printf("%s: alloc fd fail\n", __FUNCTION__);
        goto firmwarefs_open_end;
    }

    if (flags & O_RDONLY)
        nflags |= FWFS_O_RDONLY;

    if (flags & O_WRONLY)
        nflags |= FWFS_O_WRONLY;

    if (flags & O_RDWR)
        nflags |= FWFS_O_RDWR;

    if (flags & O_CREAT)
        nflags |= FWFS_O_CREAT;

    if (flags & O_APPEND)
        nflags |= FWFS_O_APPEND;

    if (flags & O_TRUNC)
        nflags |= FWFS_O_TRUNC;

    if (nflags == 0x0)
        nflags = FWFS_O_RDONLY;

    err = fwfs_file_open(&_gFwfsMntHandle.fwfs, &fd->fwfs_fd, filename, nflags);
    if (err)
    {
        printf("%s: open %s fail(%d)\n", __FUNCTION__, filename, err);
        free((void*)fd);
        goto firmwarefs_open_end;
    }

    fd->fwfs_file = &_gFwfsMntHandle.fwfs;
    ret = fd;

firmwarefs_open_end:
    return ret;
}

S32 firmwarefs_close(void* fd)
{
    int ret = 0;
    FirmwarefsFileHandle *file = (FirmwarefsFileHandle *)fd;
    if (fd)
    {
        ret = fwfs_file_close(file->fwfs_file, &file->fwfs_fd);
        if(ret)
        {
            printf("%s: close fail(%d)\n", __FUNCTION__, ret);
        }
        free(fd);
        fd = NULL;
    }
    return ret;
}

S32 firmwarefs_read(void* fd, void *buf, U32 count)
{
    FirmwarefsFileHandle *file = (FirmwarefsFileHandle *)fd;

    return fwfs_file_read(file->fwfs_file, &file->fwfs_fd, buf, count);
}

S32 firmwarefs_write(void* fd, void *buf, U32 count)
{
    FirmwarefsFileHandle *file = (FirmwarefsFileHandle *)fd;

    return fwfs_file_write(file->fwfs_file, &file->fwfs_fd, buf, count);
}

S32 firmwarefs_lseek(void* fd, S32 offset, S32 whence)
{
    FirmwarefsFileHandle *file = (FirmwarefsFileHandle *)fd;
    u32 ret;

    switch (whence)
    {
        case SEEK_SET:
            ret = fwfs_file_seek(file->fwfs_file, &file->fwfs_fd,
                                 offset, FWFS_SEEK_SET);
            break;
        case SEEK_CUR:
            ret = fwfs_file_seek(file->fwfs_file, &file->fwfs_fd,
                                 offset, FWFS_SEEK_CUR);
        case SEEK_END:
            ret = fwfs_file_size(file->fwfs_file, &file->fwfs_fd);
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}
#endif
