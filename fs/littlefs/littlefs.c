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
 * littlefs.c
 */
#include <stdlib.h>
#include <lfs.h>
#include <littlefs.h>


#if defined(CONFIG_FS_LITTLEFS)

typedef struct
{
    lfs_t lfs;
    struct lfs_config cfg;
} LittleFsMntHandle;

typedef struct
{
    lfs_t *lfs_file;
    lfs_file_t lfs_fd;
} LittleFsFileHandle;


extern int printf(const char *fmt, ...);

#if defined(CONFIG_MS_SPINAND)
extern  void MDrv_SPINAND_GetPartitionInfo(char *partition, void *pPartInfo);
extern  U32 MDrv_SPINAND_ReadData(U32 u32_PageIdx, U8 *u8Data, U8 *pu8_SpareBuf, U32 u32_DataSize, void *lfsPartitionInfo);
#define LITTLEFS_GET_PART_INFO                  MDrv_SPINAND_GetPartitionInfo
#else
extern int nor_flash_get_partitioninfo(char *partition, void *pPartitionInfo);
extern int _spi_flash_read_data(u32 offset, size_t len, void *buf);
#define    LITTLEFS_GET_PART_INFO                  nor_flash_get_partitioninfo
#endif

#define LITTLEFS_CACHE_SIZE                     2048
#define LITTLEFS_READ_SIZE                      2048
#define LITTLEFS_BLK_SIZE                       32768
#define LITTLEFS_READ_BUFFER_CNT                4


static LittleFsMntHandle _gLfsMntHandle;


//block device operations
static int block_device_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    U32 u32Addr;
    u32Addr = c->block_size * (block + c->block_offset) + off;

#if defined(CONFIG_MS_SPINAND)
    S32 ret;
    U32 u32PageIdx;
    U32 u32PageCnt, i;
    u32PageIdx = u32Addr / c->page_size;
    u32PageCnt =  size / c->page_size;

    for (i = 0; i < u32PageCnt; i++)
    {
        ret = MDrv_SPINAND_ReadData(u32PageIdx+i, (u8*)buffer+c->page_size*i, NULL, c->page_size, (void*)&c->lfs_partition);
        if(ret)
        {
            break;
        }
    }
    return ret;
#else
    return _spi_flash_read_data(u32Addr, size, buffer);
#endif

}

S32  littlefs_mount(char *partition, char *mnt_path)
{
    int err = 0;
    memset(&_gLfsMntHandle.cfg.lfs_partition, 0x0, sizeof(_gLfsMntHandle.cfg.lfs_partition));

    _gLfsMntHandle.cfg.lfs_partition.u8_UseExternBlockSize = 1;
    _gLfsMntHandle.cfg.lfs_partition.u32_BlkSize = LITTLEFS_BLK_SIZE;
    LITTLEFS_GET_PART_INFO(partition, (void*)&_gLfsMntHandle.cfg.lfs_partition);

    //block device operations
    _gLfsMntHandle.cfg.read = block_device_read;

    //block device configuration
     //spinad need set flash page size, value is 2048, spinor not limit
    _gLfsMntHandle.cfg.page_size = _gLfsMntHandle.cfg.lfs_partition.u32_PageSize;
    // block size is a multiple of cache size
    _gLfsMntHandle.cfg.block_size = _gLfsMntHandle.cfg.lfs_partition.u32_BlkSize;
    _gLfsMntHandle.cfg.block_count = _gLfsMntHandle.cfg.lfs_partition.u16_BlkCnt;
    _gLfsMntHandle.cfg.block_offset = _gLfsMntHandle.cfg.lfs_partition.u16_StartBlk;

    _gLfsMntHandle.cfg.read_size = LITTLEFS_READ_SIZE;
    _gLfsMntHandle.cfg.read_buffer_cnt = LITTLEFS_READ_BUFFER_CNT;
    //cache size is a multiple of read sizes
    _gLfsMntHandle.cfg.cache_size = LITTLEFS_CACHE_SIZE;

    // mount the filesystem
    err = lfs_mount(&_gLfsMntHandle.lfs, &_gLfsMntHandle.cfg);

    return err;
}

void littlefs_unmount()
{
    lfs_unmount(&_gLfsMntHandle.lfs);
}

void* littlefs_open(char *filename, U32 flags, U32 mode)
{
    int err;
    int flag;
    void *ret = NULL;
    LittleFsFileHandle *fd = NULL;

    fd = (LittleFsFileHandle *)calloc(sizeof(LittleFsFileHandle), 1);
    if (!fd)
    {
        printf("%s: alloc fd fail\n", __FUNCTION__);
        goto littlefs_open_end;
    }

    switch(flags)
    {
        case O_RDONLY:
            flag = LFS_O_RDONLY;
            break;
        case O_WRONLY:
            flag = LFS_O_WRONLY;
            break;
        case O_RDWR:
            flag = LFS_O_RDWR;
            break;
        default:
            flag = LFS_O_RDONLY;
            break;
    }

    err = lfs_file_open(&_gLfsMntHandle.lfs, &fd->lfs_fd, filename, flag);
    if (err)
    {
        printf("%s: open %s fail(%d)\n", __FUNCTION__, filename, err);
        free((void*)fd);
        goto littlefs_open_end;
    }

    fd->lfs_file = &_gLfsMntHandle.lfs;
    ret = fd;

littlefs_open_end:
    return ret;
}

S32 littlefs_close(void* fd)
{
    int ret = 0;
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;
    if (fd)
    {
        ret = lfs_file_close(file->lfs_file, &file->lfs_fd);
        if(ret)
        {
            printf("%s: close fail(%d)\n", __FUNCTION__, ret);
        }
        free(fd);
        fd = NULL;
    }
    return ret;
}

S32 littlefs_read(void* fd, void *buf, U32 count)
{
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;

    return lfs_file_read(file->lfs_file, &file->lfs_fd, buf, count);
}

S32 littlefs_write(void* fd, void *buf, U32 count)
{
    //TODO
    return 0;
}

S32 littlefs_lseek(void* fd, S32 offset, S32 whence)
{
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;
    u32 ret;

    switch (whence)
    {
        case SEEK_SET:
            ret = 0;   // Do nothing
            break;
        case SEEK_CUR:
            ret = 0;   // Do nothing
            break;
        case SEEK_END:
            ret = lfs_file_size(file->lfs_file, &file->lfs_fd);
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}
#endif
