/*
 * fat.c
 *
 * R/O (V)FAT 12/16/32 filesystem implementation by Marcus Sundberg
 *
 * 2002-07-28 - rjones@nexus-tech.net - ported to ppcboot v1.1.6
 * 2003-03-10 - kharris@nexus-tech.net - ported to uboot
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */

#include <common.h>
#include <config.h>
#include <exports.h>
#include <fat.h>
#include <fs.h>
#include <asm/byteorder.h>
#include <part.h>
#include <malloc.h>
#include <linux/compiler.h>
#include <linux/ctype.h>
#include "errno.h"

#define BYTE_PER_SEC    512
#define RESERVED_CNT    32
#define SECTOR_SIZE     512

#define DOS_BOOT_MAGIC_OFFSET   0x1fe
#define DOS_FS_TYPE_OFFSET      0x36
#define DOS_FS32_TYPE_OFFSET    0x52

#ifdef CONFIG_SUPPORT_VFAT
static const int vfat_enabled = 1;
#else
static const int vfat_enabled = 0;
#endif

/*
 * Convert a string to lowercase.
 */
static void downcase(char *str)
{
    while (*str != '\0') {
        *str = tolower(*str);
        str++;
    }
}

static block_dev_desc_t *cur_dev;
static disk_partition_t cur_part_info;
static unsigned long part_offset = 0;
static unsigned int cur_part = 1;

static int disk_read(__u32 block, __u32 nr_blocks, void *buf)
{
    if (!cur_dev || !cur_dev->block_read)
        return -1;

    return cur_dev->block_read(cur_dev->dev,
            cur_part_info.start + block, nr_blocks, buf);
}

/*
 * Copy string, padding with spaces.
 */
static void setstr(u_int8_t *dest, const char *src, size_t len)
{
    while (len--)
        *dest++ = *src ? *src++ : ' ';
}

static int write_pbr(block_dev_desc_t *dev_desc, disk_partition_t *info)
{
    ALLOC_CACHE_ALIGN_BUFFER(__u8, img, sizeof(__u8)*512);
    struct bs *bs;
    struct bsbpb *bsbpb;
    struct bsxbpb *bsxbpb;
    struct bsx *bsx;
    int img_offset = 0;
//  int reserved_cnt= 0;
    int i;
    int fat_size = 0;

    memset(img, 0x0, sizeof(__u8)*512);


    /* Erase Reserved Sector(PBR) */
    for (i = 0;i < RESERVED_CNT; i++) {
        if (dev_desc->block_write(dev_desc->dev, info->start + i, 1, (ulong *)img) != 1) {
            printf ("Can't erase reserved sector~~~!!!\n");
            return -1;
        }
    }

    /* Set bs */
    bs = (struct bs *)img;
    img_offset += sizeof(struct bs);

    mk1(bs->jmp[0], 0xeb);
    mk1(bs->jmp[1], 0x58);
    mk1(bs->jmp[2], 0x90); /* Jump Boot Code */
    setstr(bs->oem, "SSTAR", sizeof(bs->oem)); /* OEM Name */

    uint spc;
    /* Set bsbpb */
    bsbpb = (struct bsbpb *)(img + img_offset);
    img_offset += sizeof(struct bsbpb);

    mk2(bsbpb->bps, 512); /* Byte Per Sector */
    /* Sector Per Cluster */
    if (info->size < 0x10000) { /* partition size >= 32Mb */
        printf("Can't format less than 32Mb partition!!\n");
        return -1;
    }
    if (info->size <= 0x20000) { /* under 64M -> 512 bytes */
        printf("Under 64M\n");
        mk1(bsbpb->spc, 1);
        spc = 1;
    }
    else if (info->size <= 0x40000) { /* under 128M -> 1K */
        printf("Under 128M\n");
        mk1(bsbpb->spc, 2);
        spc = 2;
    }
    else if (info->size <= 0x80000) { /* under 256M -> 2K */
        printf("Under 256M\n");
        mk1(bsbpb->spc, 4);
        spc = 4;
    }
    else if (info->size <= 0xFA0000) { /* under 8G -> 4K */
        printf("Under 8G\n");
        mk1(bsbpb->spc, 8);
        spc = 8;
    }
    else if (info->size <= 0x1F40000) { /* under 16G -> 8K */
        printf("Under 16G\n");
        mk1(bsbpb->spc, 16);
        spc = 16;
    }
    else {
        printf("16G~\n");
        mk1(bsbpb->spc, 32);
        spc = 32;
    }

    mk2(bsbpb->res, RESERVED_CNT); /* Reserved Sector Count */
    mk1(bsbpb->nft, 2); /* Number of FATs */
    mk2(bsbpb->rde, 0); /* Root Directory Entry Count : It's no use in FAT32 */
    mk2(bsbpb->sec, 0); /* Total Sector : It's no use in FAT32 */
    mk1(bsbpb->mid, 0xF8); /* Media */
    mk2(bsbpb->spf, 0); /* FAT Size 16 : It's no use in FAT32 */
    mk2(bsbpb->spt, 0); /* Sector Per Track */
    mk2(bsbpb->hds, 0); /* Number Of Heads */
    mk4(bsbpb->hid, 0); /* Hidden Sector */
    mk4(bsbpb->bsec, info->size); /* Total Sector For FAT32 */

    /* Set bsxbpb */
    bsxbpb = (struct bsxbpb *)(img + img_offset);
    img_offset += sizeof(struct bsxbpb);

    mk4(bsxbpb->bspf, (info->size / (spc * 128))); /* FAT Size 32 */
    fat_size = info->size / (spc * 128);
    printf("Fat size : 0x%lx\n", info->size / (spc * 128));
    mk2(bsxbpb->xflg, 0); /* Ext Flags */
    mk2(bsxbpb->vers, 0); /* File System Version */
    mk4(bsxbpb->rdcl, 2); /* Root Directory Cluster */
    mk2(bsxbpb->infs, 1); /* File System Information */
    mk2(bsxbpb->bkbs, 0); /* Boot Record Backup Sector */

    /* Set bsx */
    bsx = (struct bsx *)(img + img_offset);
    mk1(bsx->drv, 0); /* Drive Number */
    mk1(bsx->sig, 0x29); /* Boot Signature */
    mk4(bsx->volid, 0x3333); /* Volume ID : 0x3333 means nothing */
    setstr(bsx->label, "NO NAME ", sizeof(bsx->label)); /* Volume Label */
    setstr(bsx->type, "FAT32", sizeof(bsx->type)); /* File System Type */

    /* Set Magic Number */
    mk2(img + BYTE_PER_SEC - 2, 0xaa55); /* Signature */

    /*
    printf("Print Boot Recode\n");
    for(i = 0;i<512;i++) {
        if(img[i] == 0)
            printf("00 ");
        else
            printf("%2x ", img[i]);
        if (!((i+1) % 16))
            printf("\n");
    }
    */

    if (dev_desc->block_write(dev_desc->dev, info->start, 1, (ulong *)img) != 1) {
        printf ("Can't write PBR~~~!!!\n");
        return -1;
    }

    return fat_size;
}

static int write_reserved(block_dev_desc_t *dev_desc, disk_partition_t *info)
{
    /* Set Reserved Region */
    ALLOC_CACHE_ALIGN_BUFFER(__u8, img, sizeof(__u8)*512);
//  int i;
    memset(img, 0x0, sizeof(__u8)*512);

    mk4(img, 0x41615252); /* Lead Signature */
    mk4(img + BYTE_PER_SEC - 28, 0x61417272); /* Struct Signature */
    mk4(img + BYTE_PER_SEC - 24, 0xffffffff); /* Free Cluster Count */
    mk4(img + BYTE_PER_SEC - 20, 0x3); /* Next Free Cluster */
    mk2(img + BYTE_PER_SEC - 2, 0xaa55); /* Trail Signature */

/*
    printf("Print Reserved Region\n");
    for(i = 0;i<512;i++) {
        if(img[i] == 0)
            printf("00 ");
        else
            printf("%2x ", img[i]);
        if (!((i+1) % 16))
            printf("\n");
    }
*/

    /* Write Reserved region */
    if (dev_desc->block_write(dev_desc->dev, info->start+1, 1, (ulong *)img) != 1) {
        printf ("Can't write reserved region~~~!!!\n");
        return -1;
    }

    return 1;
}
static int write_fat(block_dev_desc_t *dev_desc, disk_partition_t *info, int fat_size)
{
    int i;
    /* Create buffer for FAT */
    ALLOC_CACHE_ALIGN_BUFFER(__u8, img, sizeof(__u8)*512);
    /* Create buffer for erase */
    ALLOC_CACHE_ALIGN_BUFFER(__u8, dummy, sizeof(__u8) * 8192);

    memset(img, 0x0, sizeof(__u8) * 512);
    memset(dummy, 0x0, sizeof(__u8) * 8192);

    /* Erase FAT Region */
    int erase_block_cnt = (fat_size * 2);
//  int temp = 0;
    printf("Erase FAT region");
    for (i = 0;i < erase_block_cnt + 10; i+=16) {
        if (dev_desc->block_write(dev_desc->dev, info->start +
            RESERVED_CNT + i, 16, (ulong *)dummy) != 16) {
            printf ("Can't erase FAT region~~!!!\n");
        }
        if((i % 160) == 0)
            printf(".");

    }
    printf("\n");

    mk4(img, 0x0ffffff8);
    mk4(img+4, 0x0fffffff);
    mk4(img+8, 0x0fffffff); /* Root Directory */

    /*
    printf("Print FAT Region\n");
    for(i = 0;i<512;i++) {
        if(img[i] == 0)
            printf("00 ");
        else
            printf("%2x ", img[i]);
        if (!((i+1) % 16))
            printf("\n");
    }
    */
    /* Write FAT Region */
    if (dev_desc->block_write(dev_desc->dev, info->start + RESERVED_CNT, 1, (ulong *)img) != 1) {
        printf ("Can't write FAT~~~!!!\n");
        return -1;
    }

    return 1;
}

int fat_set_blk_dev(block_dev_desc_t *dev_desc, disk_partition_t *info)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);

    cur_dev = dev_desc;
    cur_part_info = *info;

    /* Make sure it has a valid FAT header */
    if (disk_read(0, 1, buffer) != 1) {
        cur_dev = NULL;
        return -1;
    }

    /* Check if it's actually a DOS volume */
    if (memcmp(buffer + DOS_BOOT_MAGIC_OFFSET, "\x55\xAA", 2)) {
        cur_dev = NULL;
        return -1;
    }

    /* Check for FAT12/FAT16/FAT32 filesystem */
    if (!memcmp(buffer + DOS_FS_TYPE_OFFSET, "FAT", 3))
        return 0;
    if (!memcmp(buffer + DOS_FS32_TYPE_OFFSET, "FAT32", 5))
        return 0;

    cur_dev = NULL;
    return -1;
}

int fat_register_device(block_dev_desc_t *dev_desc, int part_no)
{
    disk_partition_t info;

    /* First close any currently found FAT filesystem */
    cur_dev = NULL;

    /* Read the partition table, if present */
    if (get_partition_info(dev_desc, part_no, &info)) {
        if (part_no != 0) {
            printf("** Partition %d not valid on device %d **\n",
                    part_no, dev_desc->dev);
            return -1;
        }

        info.start = 0;
        info.size = dev_desc->lba;
        info.blksz = dev_desc->blksz;
        info.name[0] = 0;
        info.type[0] = 0;
        info.bootable = 0;
#ifdef CONFIG_PARTITION_UUIDS
        info.uuid[0] = 0;
#endif
    }

    return fat_set_blk_dev(dev_desc, &info);
}

/*
 * Get the first occurence of a directory delimiter ('/' or '\') in a string.
 * Return index into string if found, -1 otherwise.
 */
static int dirdelim(char *str)
{
    char *start = str;

    while (*str != '\0') {
        if (ISDIRDELIM(*str))
            return str - start;
        str++;
    }
    return -1;
}

/*
 * Extract zero terminated short name from a directory entry.
 */
static void get_name(dir_entry *dirent, char *s_name)
{
    char *ptr;

    memcpy(s_name, dirent->name, 8);
    s_name[8] = '\0';
    ptr = s_name;
    while (*ptr && *ptr != ' ')
        ptr++;
    if (dirent->ext[0] && dirent->ext[0] != ' ') {
        *ptr = '.';
        ptr++;
        memcpy(ptr, dirent->ext, 3);
        ptr[3] = '\0';
        while (*ptr && *ptr != ' ')
            ptr++;
    }
    *ptr = '\0';
    if (*s_name == DELETED_FLAG)
        *s_name = '\0';
    else if (*s_name == aRING)
        *s_name = DELETED_FLAG;
    downcase(s_name);
}

/*
 * Get the entry at index 'entry' in a FAT (12/16/32) table.
 * On failure 0x00 is returned.
 */
static __u32 get_fatent(fsdata *mydata, __u32 entry)
{
    __u32 bufnum;
    __u32 off16, offset;
    __u32 ret = 0x00;
    __u16 val1, val2;

    switch (mydata->fatsize) {
    case 32:
        bufnum = entry / FAT32BUFSIZE;
        offset = entry - bufnum * FAT32BUFSIZE;
        break;
    case 16:
        bufnum = entry / FAT16BUFSIZE;
        offset = entry - bufnum * FAT16BUFSIZE;
        break;
    case 12:
        bufnum = entry / FAT12BUFSIZE;
        offset = entry - bufnum * FAT12BUFSIZE;
        break;

    default:
        /* Unsupported FAT size */
        return ret;
    }

    debug("FAT%d: entry: 0x%04x = %d, offset: 0x%04x = %d\n",
           mydata->fatsize, entry, entry, offset, offset);

    /* Read a new block of FAT entries into the cache. */
    if (bufnum != mydata->fatbufnum) {
        __u32 getsize = FATBUFBLOCKS;
        __u8 *bufptr = mydata->fatbuf;
        __u32 fatlength = mydata->fatlength;
        __u32 startblock = bufnum * FATBUFBLOCKS;

        if (startblock + getsize > fatlength)
            getsize = fatlength - startblock;

        startblock += mydata->fat_sect; /* Offset from start of disk */

        if (disk_read(startblock, getsize, bufptr) < 0) {
            debug("Error reading FAT blocks\n");
            return ret;
        }
        mydata->fatbufnum = bufnum;
    }

    /* Get the actual entry from the table */
    switch (mydata->fatsize) {
    case 32:
        ret = FAT2CPU32(((__u32 *) mydata->fatbuf)[offset]);
        break;
    case 16:
        ret = FAT2CPU16(((__u16 *) mydata->fatbuf)[offset]);
        break;
    case 12:
        off16 = (offset * 3) / 4;

        switch (offset & 0x3) {
        case 0:
            ret = FAT2CPU16(((__u16 *) mydata->fatbuf)[off16]);
            ret &= 0xfff;
            break;
        case 1:
            val1 = FAT2CPU16(((__u16 *)mydata->fatbuf)[off16]);
            val1 &= 0xf000;
            val2 = FAT2CPU16(((__u16 *)mydata->fatbuf)[off16 + 1]);
            val2 &= 0x00ff;
            ret = (val2 << 4) | (val1 >> 12);
            break;
        case 2:
            val1 = FAT2CPU16(((__u16 *)mydata->fatbuf)[off16]);
            val1 &= 0xff00;
            val2 = FAT2CPU16(((__u16 *)mydata->fatbuf)[off16 + 1]);
            val2 &= 0x000f;
            ret = (val2 << 8) | (val1 >> 8);
            break;
        case 3:
            ret = FAT2CPU16(((__u16 *)mydata->fatbuf)[off16]);
            ret = (ret & 0xfff0) >> 4;
            break;
        default:
            break;
        }
        break;
    }
    debug("FAT%d: ret: %08x, offset: %04x\n",
           mydata->fatsize, ret, offset);

    return ret;
}

/*
 * Read at most 'size' bytes from the specified cluster into 'buffer'.
 * Return 0 on success, -1 otherwise.
 */
static int get_cluster(fsdata *mydata, __u32 clustnum, __u8 *buffer, unsigned long size)
{
    __u32 idx = 0;
    __u32 startsect;
    int ret;

    if (clustnum > 0) {
        startsect = mydata->data_begin +
                clustnum * mydata->clust_size;
    } else {
        startsect = mydata->rootdir_sect;
    }

    debug("gc - clustnum: %d, startsect: %d\n", clustnum, startsect);

    if ((unsigned long)buffer & (ARCH_DMA_MINALIGN - 1)) {
        ALLOC_CACHE_ALIGN_BUFFER(__u8, tmpbuf, mydata->sect_size);

        //printf("FAT: Misaligned buffer address (%p)\n", buffer);

        while (size >= mydata->sect_size) {
            ret = disk_read(startsect++, 1, tmpbuf);
            if (ret != 1) {
                debug("Error reading data (got %d)\n", ret);
                return -1;
            }

            memcpy(buffer, tmpbuf, mydata->sect_size);
            buffer += mydata->sect_size;
            size -= mydata->sect_size;
        }
    } else {
        idx = size / mydata->sect_size;
        ret = disk_read(startsect, idx, buffer);
        if (ret != idx) {
            debug("Error reading data (got %d)\n", ret);
            return -1;
        }
        startsect += idx;
        idx *= mydata->sect_size;
        buffer += idx;
        size -= idx;
    }
    if (size) {
        ALLOC_CACHE_ALIGN_BUFFER(__u8, tmpbuf, mydata->sect_size);

        ret = disk_read(startsect, 1, tmpbuf);
        if (ret != 1) {
            debug("Error reading data (got %d)\n", ret);
            return -1;
        }

        memcpy(buffer, tmpbuf, size);
    }

    return 0;
}

/*
 * Read at most 'maxsize' bytes from 'pos' in the file associated with 'dentptr'
 * into 'buffer'.
 * Update the number of bytes read in *gotsize or return -1 on fatal errors.
 */
__u8 get_contents_vfatname_block[MAX_CLUSTSIZE]
    __aligned(ARCH_DMA_MINALIGN);

static int get_contents(fsdata *mydata, dir_entry *dentptr, loff_t pos,
            __u8 *buffer, loff_t maxsize, loff_t *gotsize)
{
    loff_t filesize = FAT2CPU32(dentptr->size);
    unsigned int bytesperclust = mydata->clust_size * mydata->sect_size;
    __u32 curclust = START(dentptr);
    __u32 endclust, newclust;
    loff_t actsize;

    *gotsize = 0;
    debug("Filesize: %llu bytes\n", filesize);

    if (pos >= filesize) {
        debug("Read position past EOF: %llu\n", pos);
        return 0;
    }

    if (maxsize > 0 && filesize > pos + maxsize)
        filesize = pos + maxsize;

    debug("%llu bytes\n", filesize);

    actsize = bytesperclust;

    /* go to cluster at pos */
    while (actsize <= pos) {
        curclust = get_fatent(mydata, curclust);
        if (CHECK_CLUST(curclust, mydata->fatsize)) {
            debug("curclust: 0x%x\n", curclust);
            debug("Invalid FAT entry\n");
            return 0;
        }
        actsize += bytesperclust;
    }

    /* actsize > pos */
    actsize -= bytesperclust;
    filesize -= actsize;
    pos -= actsize;

    /* align to beginning of next cluster if any */
    if (pos) {
        actsize = min(filesize, (loff_t)bytesperclust);
        if (get_cluster(mydata, curclust, get_contents_vfatname_block,
                (int)actsize) != 0) {
            printf("Error reading cluster\n");
            return -1;
        }
        filesize -= actsize;
        actsize -= pos;
        memcpy(buffer, get_contents_vfatname_block + pos, actsize);
        *gotsize += actsize;
        if (!filesize)
            return 0;
        buffer += actsize;

        curclust = get_fatent(mydata, curclust);
        if (CHECK_CLUST(curclust, mydata->fatsize)) {
            debug("curclust: 0x%x\n", curclust);
            debug("Invalid FAT entry\n");
            return 0;
        }
    }

    actsize = bytesperclust;
    endclust = curclust;

    do {
        /* search for consecutive clusters */
        while (actsize < filesize) {
            newclust = get_fatent(mydata, endclust);
            if ((newclust - 1) != endclust)
                goto getit;
            if (CHECK_CLUST(newclust, mydata->fatsize)) {
                debug("curclust: 0x%x\n", newclust);
                debug("Invalid FAT entry\n");
                return 0;
            }
            endclust = newclust;
            actsize += bytesperclust;
        }

        /* get remaining bytes */
        actsize = filesize;
        if (get_cluster(mydata, curclust, buffer, (int)actsize) != 0) {
            printf("Error reading cluster\n");
            return -1;
        }
        *gotsize += actsize;
        return 0;
getit:
        if (get_cluster(mydata, curclust, buffer, (int)actsize) != 0) {
            printf("Error reading cluster\n");
            return -1;
        }
        *gotsize += (int)actsize;
        filesize -= actsize;
        buffer += actsize;

        curclust = get_fatent(mydata, endclust);
        if (CHECK_CLUST(curclust, mydata->fatsize)) {
            debug("curclust: 0x%x\n", curclust);
            printf("Invalid FAT entry\n");
            return 0;
        }
        actsize = bytesperclust;
        endclust = curclust;
    } while (1);
}

/*
 * Extract the file name information from 'slotptr' into 'l_name',
 * starting at l_name[*idx].
 * Return 1 if terminator (zero byte) is found, 0 otherwise.
 */
static int slot2str(dir_slot *slotptr, char *l_name, int *idx)
{
    int j;

    for (j = 0; j <= 8; j += 2) {
        l_name[*idx] = slotptr->name0_4[j];
        if (l_name[*idx] == 0x00)
            return 1;
        (*idx)++;
    }
    for (j = 0; j <= 10; j += 2) {
        l_name[*idx] = slotptr->name5_10[j];
        if (l_name[*idx] == 0x00)
            return 1;
        (*idx)++;
    }
    for (j = 0; j <= 2; j += 2) {
        l_name[*idx] = slotptr->name11_12[j];
        if (l_name[*idx] == 0x00)
            return 1;
        (*idx)++;
    }

    return 0;
}

/*
 * Extract the full long filename starting at 'retdent' (which is really
 * a slot) into 'l_name'. If successful also copy the real directory entry
 * into 'retdent'
 * Return 0 on success, -1 otherwise.
 */
static int
get_vfatname(fsdata *mydata, int curclust, __u8 *cluster,
         dir_entry *retdent, char *l_name)
{
    dir_entry *realdent;
    dir_slot *slotptr = (dir_slot *)retdent;
    __u8 *buflimit = cluster + mydata->sect_size * ((curclust == 0) ?
                            PREFETCH_BLOCKS :
                            mydata->clust_size);
    __u8 counter = (slotptr->id & ~LAST_LONG_ENTRY_MASK) & 0xff;
    int idx = 0;

    if (counter > VFAT_MAXSEQ) {
        debug("Error: VFAT name is too long\n");
        return -1;
    }

    while ((__u8 *)slotptr < buflimit) {
        if (counter == 0)
            break;
        if (((slotptr->id & ~LAST_LONG_ENTRY_MASK) & 0xff) != counter)
            return -1;
        slotptr++;
        counter--;
    }

    if ((__u8 *)slotptr >= buflimit) {
        dir_slot *slotptr2;

        if (curclust == 0)
            return -1;
        curclust = get_fatent(mydata, curclust);
        if (CHECK_CLUST(curclust, mydata->fatsize)) {
            debug("curclust: 0x%x\n", curclust);
            printf("Invalid FAT entry\n");
            return -1;
        }

        if (get_cluster(mydata, curclust, get_contents_vfatname_block,
                mydata->clust_size * mydata->sect_size) != 0) {
            debug("Error: reading directory block\n");
            return -1;
        }

        slotptr2 = (dir_slot *)get_contents_vfatname_block;
        while (counter > 0) {
            if (((slotptr2->id & ~LAST_LONG_ENTRY_MASK)
                & 0xff) != counter)
                return -1;
            slotptr2++;
            counter--;
        }

        /* Save the real directory entry */
        realdent = (dir_entry *)slotptr2;
        while ((__u8 *)slotptr2 > get_contents_vfatname_block) {
            slotptr2--;
            slot2str(slotptr2, l_name, &idx);
        }
    } else {
        /* Save the real directory entry */
        realdent = (dir_entry *)slotptr;
    }

    do {
        slotptr--;
        if (slot2str(slotptr, l_name, &idx))
            break;
    } while (!(slotptr->id & LAST_LONG_ENTRY_MASK));

    l_name[idx] = '\0';
    if (*l_name == DELETED_FLAG)
        *l_name = '\0';
    else if (*l_name == aRING)
        *l_name = DELETED_FLAG;
    downcase(l_name);

    /* Return the real directory entry */
    memcpy(retdent, realdent, sizeof(dir_entry));

    return 0;
}

/* Calculate short name checksum */
static __u8 mkcksum(const char name[8], const char ext[3])
{
    int i;

    __u8 ret = 0;

    for (i = 0; i < 8; i++)
        ret = (((ret & 1) << 7) | ((ret & 0xfe) >> 1)) + name[i];
    for (i = 0; i < 3; i++)
        ret = (((ret & 1) << 7) | ((ret & 0xfe) >> 1)) + ext[i];

    return ret;
}

/*
 * Get the directory entry associated with 'filename' from the directory
 * starting at 'startsect'
 */
__u8 get_dentfromdir_block[MAX_CLUSTSIZE]
    __aligned(ARCH_DMA_MINALIGN);

static dir_entry *get_dentfromdir(fsdata *mydata, int startsect,
                  char *filename, dir_entry *retdent,
                  int dols)
{
    __u16 prevcksum = 0xffff;
    __u32 curclust = START(retdent);
    int files = 0, dirs = 0;

    debug("get_dentfromdir: %s\n", filename);

    while (1) {
        dir_entry *dentptr;

        int i;

        if (get_cluster(mydata, curclust, get_dentfromdir_block,
                mydata->clust_size * mydata->sect_size) != 0) {
            debug("Error: reading directory block\n");
            return NULL;
        }

        dentptr = (dir_entry *)get_dentfromdir_block;

        for (i = 0; i < DIRENTSPERCLUST; i++) {
            char s_name[14], l_name[VFAT_MAXLEN_BYTES];

            l_name[0] = '\0';
            if (dentptr->name[0] == DELETED_FLAG) {
                dentptr++;
                continue;
            }
            if ((dentptr->attr & ATTR_VOLUME)) {
                if (vfat_enabled &&
                    (dentptr->attr & ATTR_VFAT) == ATTR_VFAT &&
                    (dentptr->name[0] & LAST_LONG_ENTRY_MASK)) {
                    prevcksum = ((dir_slot *)dentptr)->alias_checksum;
                    get_vfatname(mydata, curclust,
                             get_dentfromdir_block,
                             dentptr, l_name);
                    if (dols) {
                        int isdir;
                        char dirc;
                        int doit = 0;

                        isdir = (dentptr->attr & ATTR_DIR);

                        if (isdir) {
                            dirs++;
                            dirc = '/';
                            doit = 1;
                        } else {
                            dirc = ' ';
                            if (l_name[0] != 0) {
                                files++;
                                doit = 1;
                            }
                        }
                        if (doit) {
                            if (dirc == ' ') {
                                printf(" %8u   %s%c\n",
                                       FAT2CPU32(dentptr->size),
                                    l_name,
                                    dirc);
                            } else {
                                printf("            %s%c\n",
                                    l_name,
                                    dirc);
                            }
                        }
                        dentptr++;
                        continue;
                    }
                    debug("vfatname: |%s|\n", l_name);
                } else {
                    /* Volume label or VFAT entry */
                    dentptr++;
                    continue;
                }
            }
            if (dentptr->name[0] == 0) {
                if (dols) {
                    printf("\n%d file(s), %d dir(s)\n\n",
                        files, dirs);
                }
                debug("Dentname == NULL - %d\n", i);
                return NULL;
            }
            if (vfat_enabled) {
                __u8 csum = mkcksum(dentptr->name, dentptr->ext);
                if (dols && csum == prevcksum) {
                    prevcksum = 0xffff;
                    dentptr++;
                    continue;
                }
            }

            get_name(dentptr, s_name);
            if (dols) {
                int isdir = (dentptr->attr & ATTR_DIR);
                char dirc;
                int doit = 0;

                if (isdir) {
                    dirs++;
                    dirc = '/';
                    doit = 1;
                } else {
                    dirc = ' ';
                    if (s_name[0] != 0) {
                        files++;
                        doit = 1;
                    }
                }

                if (doit) {
                    if (dirc == ' ') {
                        printf(" %8u   %s%c\n",
                               FAT2CPU32(dentptr->size),
                            s_name, dirc);
                    } else {
                        printf("            %s%c\n",
                            s_name, dirc);
                    }
                }

                dentptr++;
                continue;
            }

            if (strcmp(filename, s_name)
                && strcmp(filename, l_name)) {
                debug("Mismatch: |%s|%s|\n", s_name, l_name);
                dentptr++;
                continue;
            }

            memcpy(retdent, dentptr, sizeof(dir_entry));

            debug("DentName: %s", s_name);
            debug(", start: 0x%x", START(dentptr));
            debug(", size:  0x%x %s\n",
                  FAT2CPU32(dentptr->size),
                  (dentptr->attr & ATTR_DIR) ? "(DIR)" : "");

            return retdent;
        }

        curclust = get_fatent(mydata, curclust);
        if (CHECK_CLUST(curclust, mydata->fatsize)) {
            debug("curclust: 0x%x\n", curclust);
            printf("Invalid FAT entry\n");
            return NULL;
        }
    }

    return NULL;
}

/*
 * Read boot sector and volume info from a FAT filesystem
 */
static int
read_bootsectandvi(boot_sector *bs, volume_info *volinfo, int *fatsize)
{
    __u8 *block;
    volume_info *vistart;
    int ret = 0;

    if (cur_dev == NULL) {
        debug("Error: no device selected\n");
        return -1;
    }

    block = memalign(ARCH_DMA_MINALIGN, cur_dev->blksz);
    if (block == NULL) {
        debug("Error: allocating block\n");
        return -1;
    }

    if (disk_read(0, 1, block) < 0) {
        debug("Error: reading block\n");
        goto fail;
    }

    memcpy(bs, block, sizeof(boot_sector));
    bs->reserved = FAT2CPU16(bs->reserved);
    bs->fat_length = FAT2CPU16(bs->fat_length);
    bs->secs_track = FAT2CPU16(bs->secs_track);
    bs->heads = FAT2CPU16(bs->heads);
    bs->total_sect = FAT2CPU32(bs->total_sect);

    /* FAT32 entries */
    if (bs->fat_length == 0) {
        /* Assume FAT32 */
        bs->fat32_length = FAT2CPU32(bs->fat32_length);
        bs->flags = FAT2CPU16(bs->flags);
        bs->root_cluster = FAT2CPU32(bs->root_cluster);
        bs->info_sector = FAT2CPU16(bs->info_sector);
        bs->backup_boot = FAT2CPU16(bs->backup_boot);
        vistart = (volume_info *)(block + sizeof(boot_sector));
        *fatsize = 32;
    } else {
        vistart = (volume_info *)&(bs->fat32_length);
        *fatsize = 0;
    }
    memcpy(volinfo, vistart, sizeof(volume_info));

    if (*fatsize == 32) {
        if (strncmp(FAT32_SIGN, vistart->fs_type, SIGNLEN) == 0)
            goto exit;
    } else {
        if (strncmp(FAT12_SIGN, vistart->fs_type, SIGNLEN) == 0) {
            *fatsize = 12;
            goto exit;
        }
        if (strncmp(FAT16_SIGN, vistart->fs_type, SIGNLEN) == 0) {
            *fatsize = 16;
            goto exit;
        }
    }

    debug("Error: broken fs_type sign\n");
fail:
    ret = -1;
exit:
    free(block);
    return ret;
}


static int get_fs_info(fsdata *mydata)
{
	boot_sector bs;
	volume_info volinfo;
	int ret;

	ret = read_bootsectandvi(&bs, &volinfo, &mydata->fatsize);
	if (ret) {
		debug("Error: reading boot sector\n");
		return ret;
	}

	if (mydata->fatsize == 32) {
		mydata->fatlength = bs.fat32_length;
		mydata->total_sect = bs.total_sect;
	} else {
		mydata->fatlength = bs.fat_length;
		mydata->total_sect = (bs.sectors[1] << 8) + bs.sectors[0];
		if (!mydata->total_sect)
			mydata->total_sect = bs.total_sect;
	}
	if (!mydata->total_sect) /* unlikely */
		mydata->total_sect = (u32)cur_part_info.size;

	mydata->fats = bs.fats;
	mydata->fat_sect = bs.reserved;

	mydata->rootdir_sect = mydata->fat_sect + mydata->fatlength * bs.fats;

	mydata->sect_size = (bs.sector_size[1] << 8) + bs.sector_size[0];
	mydata->clust_size = bs.cluster_size;
	if (mydata->sect_size != cur_part_info.blksz) {
		printf("Error: FAT sector size mismatch (fs=%hu, dev=%lu)\n",
				mydata->sect_size, cur_part_info.blksz);
		return -1;
	}

	if (mydata->fatsize == 32) {
		mydata->data_begin = mydata->rootdir_sect -
					(mydata->clust_size * 2);
		mydata->root_cluster = bs.root_cluster;
	} else {
		mydata->rootdir_size = ((bs.dir_entries[1]  * (int)256 +
					 bs.dir_entries[0]) *
					 sizeof(dir_entry)) /
					 mydata->sect_size;
		mydata->data_begin = mydata->rootdir_sect +
					mydata->rootdir_size -
					(mydata->clust_size * 2);
		mydata->root_cluster =
			sect_to_clust(mydata, mydata->rootdir_sect);
	}

	mydata->fatbufnum = -1;
	mydata->fat_dirty = 0;
	mydata->fatbuf = malloc_cache_aligned(FATBUFSIZE);
	if (mydata->fatbuf == NULL) {
		debug("Error: allocating memory\n");
		return -1;
	}

	debug("FAT%d, fat_sect: %d, fatlength: %d\n",
	       mydata->fatsize, mydata->fat_sect, mydata->fatlength);
	debug("Rootdir begins at cluster: %d, sector: %d, offset: %x\n"
	       "Data begins at: %d\n",
	       mydata->root_cluster,
	       mydata->rootdir_sect,
	       mydata->rootdir_sect * mydata->sect_size, mydata->data_begin);
	debug("Sector size: %d, cluster size: %d\n", mydata->sect_size,
	      mydata->clust_size);

	return 0;
}

typedef struct {
	fsdata    *fsdata;        /* filesystem parameters */
	unsigned   start_clust;   /* first cluster */
	unsigned   clust;         /* current cluster */
	unsigned   next_clust;    /* next cluster if remaining == 0 */
	int        last_cluster;  /* set once we've read last cluster */
	int        is_root;       /* is iterator at root directory */
	int        remaining;     /* remaining dent's in current cluster */

	/* current iterator position values: */
	dir_entry *dent;          /* current directory entry */
    int dent_rem;             /* remaining entries after long name start */
    unsigned int dent_clust;  /* cluster of long name start */
    dir_entry *dent_start;    /* first directory entry for long name */
	char       l_name[VFAT_MAXLEN_BYTES];    /* long (vfat) name */
	char       s_name[14];    /* short 8.3 name */
	char      *name;          /* l_name if there is one, else s_name */

	/* storage for current cluster in memory: */
	u8         block[MAX_CLUSTSIZE] __aligned(ARCH_DMA_MINALIGN);
} fat_itr;

static int fat_itr_isdir(fat_itr *itr);

/**
 * fat_itr_root() - initialize an iterator to start at the root
 * directory
 *
 * @itr: iterator to initialize
 * @fsdata: filesystem data for the partition
 * @return 0 on success, else -errno
 */
static int fat_itr_root(fat_itr *itr, fsdata *fsdata)
{
	if (get_fs_info(fsdata))
		return -ENXIO;

	itr->fsdata = fsdata;
	itr->start_clust = 0;
	itr->clust = fsdata->root_cluster;
	itr->next_clust = fsdata->root_cluster;
	itr->dent = NULL;
	itr->remaining = 0;
	itr->last_cluster = 0;
	itr->is_root = 1;

	return 0;
}

/**
 * fat_itr_child() - initialize an iterator to descend into a sub-
 * directory
 *
 * Initializes 'itr' to iterate the contents of the directory at
 * the current cursor position of 'parent'.  It is an error to
 * call this if the current cursor of 'parent' is pointing at a
 * regular file.
 *
 * Note that 'itr' and 'parent' can be the same pointer if you do
 * not need to preserve 'parent' after this call, which is useful
 * for traversing directory structure to resolve a file/directory.
 *
 * @itr: iterator to initialize
 * @parent: the iterator pointing at a directory entry in the
 *    parent directory of the directory to iterate
 */
static void fat_itr_child(fat_itr *itr, fat_itr *parent)
{
	fsdata *mydata = parent->fsdata;  /* for silly macros */
	unsigned clustnum = START(parent->dent);

	assert(fat_itr_isdir(parent));

	itr->fsdata = parent->fsdata;
	itr->start_clust = clustnum;
	if (clustnum > 0) {
		itr->clust = clustnum;
		itr->next_clust = clustnum;
		itr->is_root = 0;
	} else {
		itr->clust = parent->fsdata->root_cluster;
		itr->next_clust = parent->fsdata->root_cluster;
        itr->start_clust = parent->fsdata->root_cluster;
		itr->is_root = 1;
	}
	itr->dent = NULL;
	itr->remaining = 0;
	itr->last_cluster = 0;
}

/**
 * fat_next_cluster() - load next FAT cluster
 *
 * The function is used when iterating through directories. It loads the
 * next cluster with directory entries
 *
 * @itr:	directory iterator
 * @nbytes:	number of bytes read, 0 on error
 * Return:	first directory entry, NULL on error
 */
void *fat_next_cluster(fat_itr *itr, unsigned int *nbytes)
{
	int ret;
	u32 sect;
	u32 read_size;

	/* have we reached the end? */
	if (itr->last_cluster)
		return NULL;

	if (itr->is_root && itr->fsdata->fatsize != 32) {
		/*
		 * The root directory is located before the data area and
		 * cannot be indexed using the regular unsigned cluster
		 * numbers (it may start at a "negative" cluster or not at a
		 * cluster boundary at all), so consider itr->next_clust to be
		 * a offset in cluster-sized units from the start of rootdir.
		 */
		unsigned sect_offset = itr->next_clust * itr->fsdata->clust_size;
		unsigned remaining_sects = itr->fsdata->rootdir_size - sect_offset;
		sect = itr->fsdata->rootdir_sect + sect_offset;
		/* do not read past the end of rootdir */
		read_size = min_t(u32, itr->fsdata->clust_size,
				  remaining_sects);
	} else {
		sect = clust_to_sect(itr->fsdata, itr->next_clust);
		read_size = itr->fsdata->clust_size;
	}

	debug("FAT read(sect=%d), clust_size=%d, read_size=%u\n",
		  sect, itr->fsdata->clust_size, read_size);

	/*
	 * NOTE: do_fat_read_at() had complicated logic to deal w/
	 * vfat names that span multiple clusters in the fat16 case,
	 * which get_dentfromdir() probably also needed (and was
	 * missing).  And not entirely sure what fat32 didn't have
	 * the same issue..  We solve that by only caring about one
	 * dent at a time and iteratively constructing the vfat long
	 * name.
	 */
	ret = disk_read(sect, read_size, itr->block);
	if (ret < 0) {
		debug("Error: reading block\n");
		return NULL;
	}

	*nbytes = read_size * itr->fsdata->sect_size;
	itr->clust = itr->next_clust;
	if (itr->is_root && itr->fsdata->fatsize != 32) {
		itr->next_clust++;
		if (itr->next_clust * itr->fsdata->clust_size >=
		    itr->fsdata->rootdir_size) {
			debug("nextclust: 0x%x\n", itr->next_clust);
			itr->last_cluster = 1;
		}
	} else {
		itr->next_clust = get_fatent(itr->fsdata, itr->next_clust);
		if (CHECK_CLUST(itr->next_clust, itr->fsdata->fatsize)) {
			debug("nextclust: 0x%x\n", itr->next_clust);
			itr->last_cluster = 1;
		}
	}

	return itr->block;
}


static dir_entry *next_dent(fat_itr *itr)
{
	if (itr->remaining == 0) {
        unsigned nbytes;
		struct dir_entry *dent = fat_next_cluster(itr, &nbytes);

		/* have we reached the last cluster? */
		if (!dent) {
			/* a sign for no more entries left */
			itr->dent = NULL;
			return NULL;
		}

		itr->remaining = nbytes / sizeof(dir_entry) - 1;
		itr->dent = dent;
	} else {
		itr->remaining--;
		itr->dent++;
	}

	/* have we reached the last valid entry? */
	if (itr->dent->name[0] == 0)
		return NULL;

	return itr->dent;
}

static dir_entry *extract_vfat_name(fat_itr *itr)
{
	struct dir_entry *dent = itr->dent;
	int seqn = itr->dent->name[0] & ~LAST_LONG_ENTRY_MASK;
	u8 chksum, alias_checksum = ((dir_slot *)dent)->alias_checksum;
	int n = 0;

	while (seqn--) {
		char buf[13];
		int idx = 0;

		slot2str((dir_slot *)dent, buf, &idx);

		/* shift accumulated long-name up and copy new part in: */
		memmove(itr->l_name + idx, itr->l_name, n);
		memcpy(itr->l_name, buf, idx);
		n += idx;

		dent = next_dent(itr);
		if (!dent)
			return NULL;
	}

	itr->l_name[n] = '\0';

	chksum = mkcksum(dent->name, dent->ext);

	/* checksum mismatch could mean deleted file, etc.. skip it: */
	if (chksum != alias_checksum) {
		debug("** chksum=%x, alias_checksum=%x, l_name=%s, s_name=%8s.%3s\n",
		      chksum, alias_checksum, itr->l_name, dent->name, dent->ext);
		return NULL;
	}

	return dent;
}

/**
 * fat_itr_next() - step to the next entry in a directory
 *
 * Must be called once on a new iterator before the cursor is valid.
 *
 * @itr: the iterator to iterate
 * @return boolean, 1 if success or 0 if no more entries in the
 *    current directory
 */
static int fat_itr_next(fat_itr *itr)
{
	dir_entry *dent;

	itr->name = NULL;

	while (1) {
		dent = next_dent(itr);
		if (!dent)
			return 0;

        itr->dent_rem = itr->remaining;
        itr->dent_start = itr->dent;
        itr->dent_clust = itr->clust;

		if (dent->name[0] == DELETED_FLAG ||
		    dent->name[0] == aRING)
			continue;

		if (dent->attr & ATTR_VOLUME) {
			if ((dent->attr & ATTR_VFAT) == ATTR_VFAT &&
			    (dent->name[0] & LAST_LONG_ENTRY_MASK)) {
				dent = extract_vfat_name(itr);
				if (!dent)
					continue;
				itr->name = itr->l_name;
				break;
			} else {
				/* Volume label or VFAT entry, skip */
				continue;
			}
		}

		break;
	}

	get_name(dent, itr->s_name);
	if (!itr->name)
		itr->name = itr->s_name;

	return 1;
}

/**
 * fat_itr_isdir() - is current cursor position pointing to a directory
 *
 * @itr: the iterator
 * @return true if cursor is at a directory
 */
static int fat_itr_isdir(fat_itr *itr)
{
	return !!(itr->dent->attr & ATTR_DIR);
}

/*
 * Helpers:
 */

#define TYPE_FILE 0x1
#define TYPE_DIR  0x2
#define TYPE_ANY  (TYPE_FILE | TYPE_DIR)

/**
 * fat_itr_resolve() - traverse directory structure to resolve the
 * requested path.
 *
 * Traverse directory structure to the requested path.  If the specified
 * path is to a directory, this will descend into the directory and
 * leave it iterator at the start of the directory.  If the path is to a
 * file, it will leave the iterator in the parent directory with current
 * cursor at file's entry in the directory.
 *
 * @itr: iterator initialized to root
 * @path: the requested path
 * @type: bitmask of allowable file types
 * @return 0 on success or -errno
 */
static int fat_itr_resolve(fat_itr *itr, const char *path, unsigned type)
{
	const char *next;

	/* chomp any extra leading slashes: */
	while (path[0] && ISDIRDELIM(path[0]))
		path++;

	/* are we at the end? */
	if (strlen(path) == 0) {
		if (!(type & TYPE_DIR))
			return -ENOENT;
		return 0;
	}

	/* find length of next path entry: */
	next = path;
	while (next[0] && !ISDIRDELIM(next[0]))
		next++;

	if (itr->is_root) {
		/* root dir doesn't have "." nor ".." */
		if ((((next - path) == 1) && !strncmp(path, ".", 1)) ||
		    (((next - path) == 2) && !strncmp(path, "..", 2))) {
			/* point back to itself */
			itr->clust = itr->fsdata->root_cluster;
			itr->next_clust = itr->fsdata->root_cluster;
			itr->dent = NULL;
			itr->remaining = 0;
			itr->last_cluster = 0;

			if (next[0] == 0) {
				if (type & TYPE_DIR)
					return 0;
				else
					return -ENOENT;
			}

			return fat_itr_resolve(itr, next, type);
		}
	}

	while (fat_itr_next(itr)) {
		int match = 0;
		unsigned n = max(strlen(itr->name), (size_t)(next - path));

		/* check both long and short name: */
		if (!strncasecmp(path, itr->name, n))
			match = 1;
		else if (itr->name != itr->s_name &&
			 !strncasecmp(path, itr->s_name, n))
			match = 1;

		if (!match)
			continue;

		if (fat_itr_isdir(itr)) {
			/* recurse into directory: */
			fat_itr_child(itr, itr);
			return fat_itr_resolve(itr, next, type);
		} else if (next[0]) {
			/*
			 * If next is not empty then we have a case
			 * like: /path/to/realfile/nonsense
			 */
			debug("bad trailing path: %s\n", next);
			return -ENOENT;
		} else if (!(type & TYPE_FILE)) {
			return -ENOTDIR;
		} else {
			return 0;
		}
	}

	return -ENOENT;
}

__u8 do_fat_read_at_block[MAX_CLUSTSIZE]
    __aligned(ARCH_DMA_MINALIGN);

int do_fat_read_at(const char *filename, loff_t pos, void *buffer,
           loff_t maxsize, int dols, int dogetsize, loff_t *size)
{
    char fnamecopy[2048];
    boot_sector bs;
    volume_info volinfo;
    fsdata datablock;
    fsdata *mydata = &datablock;
    dir_entry *dentptr = NULL;
    __u16 prevcksum = 0xffff;
    char *subname = "";
    __u32 cursect;
    int idx, isdir = 0;
    int files = 0, dirs = 0;
    int ret = -1;
    int firsttime;
    __u32 root_cluster = 0;
    __u32 read_blk;
    int rootdir_size = 0;
    int buffer_blk_cnt;
    int do_read;
    __u8 *dir_ptr;

    if (read_bootsectandvi(&bs, &volinfo, &mydata->fatsize)) {
        debug("Error: reading boot sector\n");
        return -1;
    }

    if (mydata->fatsize == 32) {
        root_cluster = bs.root_cluster;
        mydata->fatlength = bs.fat32_length;
    } else {
        mydata->fatlength = bs.fat_length;
    }

    mydata->fat_sect = bs.reserved;

    cursect = mydata->rootdir_sect
        = mydata->fat_sect + mydata->fatlength * bs.fats;

    mydata->sect_size = (bs.sector_size[1] << 8) + bs.sector_size[0];
    mydata->clust_size = bs.cluster_size;
    if (mydata->sect_size != cur_part_info.blksz) {
        printf("Error: FAT sector size mismatch (fs=%hu, dev=%lu)\n",
                mydata->sect_size, cur_part_info.blksz);
        return -1;
    }

    if (mydata->fatsize == 32) {
        mydata->data_begin = mydata->rootdir_sect -
                    (mydata->clust_size * 2);
    } else {
        rootdir_size = ((bs.dir_entries[1]  * (int)256 +
                 bs.dir_entries[0]) *
                 sizeof(dir_entry)) /
                 mydata->sect_size;
        mydata->data_begin = mydata->rootdir_sect +
                    rootdir_size -
                    (mydata->clust_size * 2);
    }

    mydata->fatbufnum = -1;
    mydata->fatbuf = memalign(ARCH_DMA_MINALIGN, FATBUFSIZE);
    if (mydata->fatbuf == NULL) {
        debug("Error: allocating memory\n");
        return -1;
    }

    if (vfat_enabled)
        debug("VFAT Support enabled\n");

    debug("FAT%d, fat_sect: %d, fatlength: %d\n",
           mydata->fatsize, mydata->fat_sect, mydata->fatlength);
    debug("Rootdir begins at cluster: %d, sector: %d, offset: %x\n"
           "Data begins at: %d\n",
           root_cluster,
           mydata->rootdir_sect,
           mydata->rootdir_sect * mydata->sect_size, mydata->data_begin);
    debug("Sector size: %d, cluster size: %d\n", mydata->sect_size,
          mydata->clust_size);

    /* "cwd" is always the root... */
    while (ISDIRDELIM(*filename))
        filename++;

    /* Make a copy of the filename and convert it to lowercase */
    strcpy(fnamecopy, filename);
    downcase(fnamecopy);

    if (*fnamecopy == '\0') {
        if (!dols)
            goto exit;

        dols = LS_ROOT;
    } else if ((idx = dirdelim(fnamecopy)) >= 0) {
        isdir = 1;
        fnamecopy[idx] = '\0';
        subname = fnamecopy + idx + 1;

        /* Handle multiple delimiters */
        while (ISDIRDELIM(*subname))
            subname++;
    } else if (dols) {
        isdir = 1;
    }

    buffer_blk_cnt = 0;
    firsttime = 1;
    while (1) {
        int i;

        if (mydata->fatsize == 32 || firsttime) {
            dir_ptr = do_fat_read_at_block;
            firsttime = 0;
        } else {
            /**
             * FAT16 sector buffer modification:
             * Each loop, the second buffered block is moved to
             * the buffer begin, and two next sectors are read
             * next to the previously moved one. So the sector
             * buffer keeps always 3 sectors for fat16.
             * And the current sector is the buffer second sector
             * beside the "firsttime" read, when it is the first one.
             *
             * PREFETCH_BLOCKS is 2 for FAT16 == loop[0:1]
             * n = computed root dir sector
             * loop |  cursect-1  | cursect    | cursect+1  |
             *   0  |  sector n+0 | sector n+1 | none       |
             *   1  |  none       | sector n+0 | sector n+1 |
             *   0  |  sector n+1 | sector n+2 | sector n+3 |
             *   1  |  sector n+3 | ...
            */
            dir_ptr = (do_fat_read_at_block + mydata->sect_size);
            memcpy(do_fat_read_at_block, dir_ptr, mydata->sect_size);
        }

        do_read = 1;

        if (mydata->fatsize == 32 && buffer_blk_cnt)
            do_read = 0;

        if (do_read) {
            read_blk = (mydata->fatsize == 32) ?
                    mydata->clust_size : PREFETCH_BLOCKS;

            debug("FAT read(sect=%d, cnt:%d), clust_size=%d, DIRENTSPERBLOCK=%zd\n",
                cursect, read_blk, mydata->clust_size, DIRENTSPERBLOCK);

            if (disk_read(cursect, read_blk, dir_ptr) < 0) {
                debug("Error: reading rootdir block\n");
                goto exit;
            }

            dentptr = (dir_entry *)dir_ptr;
        }

        for (i = 0; i < DIRENTSPERBLOCK; i++) {
            char s_name[14], l_name[VFAT_MAXLEN_BYTES];
            __u8 csum;

            l_name[0] = '\0';
            if (dentptr->name[0] == DELETED_FLAG) {
                dentptr++;
                continue;
            }

            if (vfat_enabled)
                csum = mkcksum(dentptr->name, dentptr->ext);

            if (dentptr->attr & ATTR_VOLUME) {
                if (vfat_enabled &&
                    (dentptr->attr & ATTR_VFAT) == ATTR_VFAT &&
                    (dentptr->name[0] & LAST_LONG_ENTRY_MASK)) {
                    prevcksum =
                        ((dir_slot *)dentptr)->alias_checksum;

                    get_vfatname(mydata,
                             root_cluster,
                             dir_ptr,
                             dentptr, l_name);

                    if (dols == LS_ROOT) {
                        char dirc;
                        int doit = 0;
                        int isdir =
                            (dentptr->attr & ATTR_DIR);

                        if (isdir) {
                            dirs++;
                            dirc = '/';
                            doit = 1;
                        } else {
                            dirc = ' ';
                            if (l_name[0] != 0) {
                                files++;
                                doit = 1;
                            }
                        }
                        if (doit) {
                            if (dirc == ' ') {
                                printf(" %8u   %s%c\n",
                                       FAT2CPU32(dentptr->size),
                                    l_name,
                                    dirc);
                            } else {
                                printf("            %s%c\n",
                                    l_name,
                                    dirc);
                            }
                        }
                        dentptr++;
                        continue;
                    }
                    debug("Rootvfatname: |%s|\n",
                           l_name);
                } else {
                    /* Volume label or VFAT entry */
                    dentptr++;
                    continue;
                }
            } else if (dentptr->name[0] == 0) {
                debug("RootDentname == NULL - %d\n", i);
                if (dols == LS_ROOT) {
                    printf("\n%d file(s), %d dir(s)\n\n",
                        files, dirs);
                    ret = 0;
                }
                goto exit;
            }
            else if (vfat_enabled &&
                 dols == LS_ROOT && csum == prevcksum) {
                prevcksum = 0xffff;
                dentptr++;
                continue;
            }

            get_name(dentptr, s_name);

            if (dols == LS_ROOT) {
                int isdir = (dentptr->attr & ATTR_DIR);
                char dirc;
                int doit = 0;

                if (isdir) {
                    dirc = '/';
                    if (s_name[0] != 0) {
                        dirs++;
                        doit = 1;
                    }
                } else {
                    dirc = ' ';
                    if (s_name[0] != 0) {
                        files++;
                        doit = 1;
                    }
                }
                if (doit) {
                    if (dirc == ' ') {
                        printf(" %8u   %s%c\n",
                               FAT2CPU32(dentptr->size),
                            s_name, dirc);
                    } else {
                        printf("            %s%c\n",
                            s_name, dirc);
                    }
                }
                dentptr++;
                continue;
            }

            if (strcmp(fnamecopy, s_name)
                && strcmp(fnamecopy, l_name)) {
                debug("RootMismatch: |%s|%s|\n", s_name,
                       l_name);
                dentptr++;
                continue;
            }

            if (isdir && !(dentptr->attr & ATTR_DIR))
                goto exit;

            debug("RootName: %s", s_name);
            debug(", start: 0x%x", START(dentptr));
            debug(", size:  0x%x %s\n",
                   FAT2CPU32(dentptr->size),
                   isdir ? "(DIR)" : "");

            goto rootdir_done;  /* We got a match */
        }
        debug("END LOOP: buffer_blk_cnt=%d   clust_size=%d\n", buffer_blk_cnt,
               mydata->clust_size);

        /*
         * On FAT32 we must fetch the FAT entries for the next
         * root directory clusters when a cluster has been
         * completely processed.
         */
        ++buffer_blk_cnt;
        int rootdir_end = 0;
        if (mydata->fatsize == 32) {
            if (buffer_blk_cnt == mydata->clust_size) {
                int nxtsect = 0;
                int nxt_clust = 0;

                nxt_clust = get_fatent(mydata, root_cluster);
                rootdir_end = CHECK_CLUST(nxt_clust, 32);

                nxtsect = mydata->data_begin +
                    (nxt_clust * mydata->clust_size);

                root_cluster = nxt_clust;

                cursect = nxtsect;
                buffer_blk_cnt = 0;
            }
        } else {
            if (buffer_blk_cnt == PREFETCH_BLOCKS)
                buffer_blk_cnt = 0;

            rootdir_end = (++cursect - mydata->rootdir_sect >=
                       rootdir_size);
        }

        /* If end of rootdir reached */
        if (rootdir_end) {
            if (dols == LS_ROOT) {
                printf("\n%d file(s), %d dir(s)\n\n",
                       files, dirs);
                *size = 0;
            }
            goto exit;
        }
    }
rootdir_done:

    firsttime = 1;

    while (isdir) {
        int startsect = mydata->data_begin
            + START(dentptr) * mydata->clust_size;
        dir_entry dent;
        char *nextname = NULL;

        dent = *dentptr;
        dentptr = &dent;

        idx = dirdelim(subname);

        if (idx >= 0) {
            subname[idx] = '\0';
            nextname = subname + idx + 1;
            /* Handle multiple delimiters */
            while (ISDIRDELIM(*nextname))
                nextname++;
            if (dols && *nextname == '\0')
                firsttime = 0;
        } else {
            if (dols && firsttime) {
                firsttime = 0;
            } else {
                isdir = 0;
            }
        }

        if (get_dentfromdir(mydata, startsect, subname, dentptr,
                     isdir ? 0 : dols) == NULL) {
            if (dols && !isdir)
                *size = 0;
            goto exit;
        }

        if (isdir && !(dentptr->attr & ATTR_DIR))
            goto exit;

        if (idx >= 0)
            subname = nextname;
    }

    if (dogetsize) {
        *size = FAT2CPU32(dentptr->size);
        ret = 0;
    } else {
        ret = get_contents(mydata, dentptr, pos, buffer, maxsize, size);
    }
    debug("Size: %u, got: %llu\n", FAT2CPU32(dentptr->size), *size);

exit:
    free(mydata->fatbuf);
    return ret;
}

int do_fat_read(const char *filename, void *buffer, loff_t maxsize, int dols,
        loff_t *actread)
{
    return do_fat_read_at(filename, 0, buffer, maxsize, dols, 0, actread);
}

int file_fat_detectfs(void)
{
    boot_sector bs;
    volume_info volinfo;
    int fatsize;
    char vol_label[12];

    if (cur_dev == NULL) {
        printf("No current device\n");
        return 1;
    }

#if defined(CONFIG_CMD_IDE) || \
    defined(CONFIG_CMD_SATA) || \
    defined(CONFIG_CMD_SCSI) || \
    defined(CONFIG_CMD_USB) || \
    defined(CONFIG_MMC)
    printf("Interface:  ");
    switch (cur_dev->if_type) {
    case IF_TYPE_IDE:
        printf("IDE");
        break;
    case IF_TYPE_SATA:
        printf("SATA");
        break;
    case IF_TYPE_SCSI:
        printf("SCSI");
        break;
    case IF_TYPE_ATAPI:
        printf("ATAPI");
        break;
    case IF_TYPE_USB:
        printf("USB");
        break;
    case IF_TYPE_DOC:
        printf("DOC");
        break;
    case IF_TYPE_MMC:
        printf("MMC");
        break;
    default:
        printf("Unknown");
    }

    printf("\n  Device %d: ", cur_dev->dev);
    dev_print(cur_dev);
#endif

    if (read_bootsectandvi(&bs, &volinfo, &fatsize)) {
        printf("\nNo valid FAT fs found\n");
        return 1;
    }

    memcpy(vol_label, volinfo.volume_label, 11);
    vol_label[11] = '\0';
    volinfo.fs_type[5] = '\0';

    printf("Filesystem: %s \"%s\"\n", volinfo.fs_type, vol_label);

    return 0;
}

int file_fat_ls(const char *dir)
{
    loff_t size;

    return do_fat_read(dir, NULL, 0, LS_YES, &size);
}

int fat_exists(const char *filename)
{
    int ret;
    loff_t size;

    ret = do_fat_read_at(filename, 0, NULL, 0, LS_NO, 1, &size);
    return ret == 0;
}

int fat_size(const char *filename, loff_t *size)
{
    return do_fat_read_at(filename, 0, NULL, 0, LS_NO, 1, size);
}

int file_fat_read_at(const char *filename, loff_t pos, void *buffer,
             loff_t maxsize, loff_t *actread)
{
    printf("reading %s\n", filename);
    return do_fat_read_at(filename, pos, buffer, maxsize, LS_NO, 0,
                  actread);
}

int file_fat_read(const char *filename, void *buffer, int maxsize)
{
    loff_t actread;
    int ret;

    ret =  file_fat_read_at(filename, 0, buffer, maxsize, &actread);
    if (ret)
        return ret;
    else
        return actread;
}

int fat_read_file(const char *filename, void *buf, loff_t offset, loff_t len,
          loff_t *actread)
{
    int ret;

    ret = file_fat_read_at(filename, offset, buf, len, actread);
    if (ret)
        printf("** Unable to read file %s **\n", filename);

    return ret;
}

void fat_close(void)
{
}

/*
 * Format device
 */
int fat_format_device(block_dev_desc_t *dev_desc, int part_no)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, sizeof(unsigned char)*SECTOR_SIZE);
    disk_partition_t info;

    if (!dev_desc->block_read)
        return -1;
    cur_dev = dev_desc;
    /* check if we have a MBR (on floppies we have only a PBR) */
    if (dev_desc->block_read (dev_desc->dev, 0, 1, (ulong *) buffer) != 1) {
        printf ("** Can't read from device %d **\n", dev_desc->dev);
        return -1;
    }
    if (buffer[DOS_BOOT_MAGIC_OFFSET] != 0x55 ||
        buffer[DOS_BOOT_MAGIC_OFFSET + 1] != 0xaa) {
        printf("** MBR is broken **\n");
        /* no signature found */
        return -1;
    }
#if (defined(CONFIG_CMD_IDE) || defined(CONFIG_CMD_SCSI) || defined(CONFIG_CMD_USB) || defined(CONFIG_MMC) || defined(CONFIG_SYSTEMACE) )
    /* First we assume, there is a MBR */
    if (!get_partition_info (dev_desc, part_no, &info)) {
        part_offset = info.start;
        cur_part = part_no;
    } else if (!strncmp((char *)&buffer[DOS_FS_TYPE_OFFSET], "FAT", 3)) {
        /* ok, we assume we are on a PBR only */
        cur_part = 1;
        part_offset = 0;
    } else {
        printf ("** Partition %d not valid on device %d **\n", part_no, dev_desc->dev);
        return -1;
    }
#endif

    printf("Partition%d: Start Address(0x%lx), Size(0x%lx)\n", part_no, info.start, info.size);

    int fat_size;
    fat_size = write_pbr(dev_desc, &info);
    if(fat_size < 0)
        return -1;
    if(write_reserved(dev_desc, &info) < 0)
        return -1;
    if(write_fat(dev_desc, &info, fat_size) < 0)
        return -1;
    printf("Partition%d format complete.\n", part_no);

    return 0;
}


