/*
 * (C) Copyright 2001
 * Raymond Lo, lo@routefree.com
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

/*
 * Support for harddisk partitions.
 *
 * To be compatible with LinuxPPC and Apple we use the standard Apple
 * SCSI disk partitioning scheme. For more information see:
 * http://developer.apple.com/techpubs/mac/Devices/Devices-126.html#MARKER-14-92
 */

#include <common.h>
#include <command.h>
#include <ide.h>
#include "part_dos.h"


//#define CONFIG_MACH_MESON8_ODROIDC

#ifdef HAVE_BLOCK_DEVICE

#define DOS_PART_DEFAULT_SECTOR 512

static int gu32_PartNum = 0;
/* Convert char[4] in little endian format to the host format integer
 */
static inline int le32_to_int(unsigned char *le32)
{
    return ((le32[3] << 24) + (le32[2] << 16) + (le32[1] << 8) + le32[0]);
}

static inline int is_extended(int part_type)
{
    return (part_type == 0x5 || part_type == 0xf || part_type == 0x85);
}

static inline int is_bootable(dos_partition_t *p)
{
    return p->boot_ind == 0x80;
}

static void print_one_part(dos_partition_t *p, int ext_part_sector,
               int part_num, unsigned int disksig)
{
    int lba_start = ext_part_sector + le32_to_int (p->start4);
    int lba_size  = le32_to_int (p->size4);

    gu32_PartNum++;
    printf("%3d\t%-10d\t%-10d\t%08x-%02x\t%02x%s%s\n",
        part_num, lba_start, lba_size, disksig, part_num, p->sys_ind,
        (is_extended(p->sys_ind) ? " Extd" : ""),
        (is_bootable(p) ? " Boot" : ""));
}

static int test_block_type(unsigned char *buffer)
{
    int slot;
    struct dos_partition *p;

    if((buffer[DOS_PART_MAGIC_OFFSET + 0] != 0x55) ||
        (buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa) ) {
        return (-1);
    } /* no DOS Signature at all */
    p = (struct dos_partition *)&buffer[DOS_PART_TBL_OFFSET];
    for (slot = 0; slot < 3; slot++) {
        if (p->boot_ind != 0 && p->boot_ind != 0x80) {
            if (!slot &&
                (strncmp((char *)&buffer[DOS_PBR_FSTYPE_OFFSET],
                     "FAT", 3) == 0 ||
                 strncmp((char *)&buffer[DOS_PBR32_FSTYPE_OFFSET],
                     "FAT32", 5) == 0)) {
                return DOS_PBR; /* is PBR */
            } else {
                return -1;
            }
        }
    }
    return DOS_MBR;     /* Is MBR */
}

unsigned long long calc_unit(unsigned long long length, SDInfo sdInfo)
{
    // using LBA MODE
    if (sdInfo.addr_mode == CHS_MODE)
        if( (unsigned int)length % sdInfo.unit )
            return ( ((unsigned int)length / sdInfo.unit + 1 ) * sdInfo.unit);
        else
            return ( ((unsigned int)length / sdInfo.unit ) * sdInfo.unit);
    else
        return ( length );
}


void encode_chs(int C, int H, int S, unsigned char *result)
{
    *result++ = (unsigned char) H;
    *result++ = (unsigned char) ( S + ((C & 0x00000300) >> 2) );
    *result   = (unsigned char) (C & 0x000000FF);
}




void encode_partitionInfo(PartitionInfo partInfo, unsigned char *result)
{
    *result++ = partInfo.bootable;

    encode_chs(partInfo.C_start, partInfo.H_start, partInfo.S_start, result);
    result +=3;
    *result++ = partInfo.partitionId;

    encode_chs(partInfo.C_end, partInfo.H_end, partInfo.S_end, result);
    result += 3;

    memcpy(result, (unsigned char *)&(partInfo.block_start), 4);
    result += 4;

    memcpy(result, (unsigned char *)&(partInfo.block_count), 4);
}


void decode_partitionInfo(unsigned char *in, PartitionInfo *partInfo)
{
    partInfo->bootable  = *in;
    partInfo->partitionId   = *(in + 4);

    memcpy((unsigned char *)&(partInfo->block_start), (in + 8), 4);
    memcpy((unsigned char *)&(partInfo->block_count), (in +12), 4);
}

/*  Print a partition that is relative to its Extended partition table
 */
static void print_partition_extended(block_dev_desc_t *dev_desc,
                int ext_part_sector, int relative,
                int part_num, unsigned int disksig)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);
    dos_partition_t *pt;
    int i;

    if (dev_desc->block_read(dev_desc->dev, ext_part_sector, 1, (ulong *) buffer) != 1) {
        printf ("** Can't read partition table on %d:%d **\n",
            dev_desc->dev, ext_part_sector);
        return;
    }
    i=test_block_type(buffer);
    if (i != DOS_MBR) {
        printf ("bad MBR sector signature 0x%02x%02x\n",
            buffer[DOS_PART_MAGIC_OFFSET],
            buffer[DOS_PART_MAGIC_OFFSET + 1]);
        return;
    }

    if (!ext_part_sector)
        disksig = le32_to_int(&buffer[DOS_PART_DISKSIG_OFFSET]);

    /* Print all primary/logical partitions */
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++) {
        /*
         * fdisk does not show the extended partitions that
         * are not in the MBR
         */

        if ((pt->sys_ind != 0) &&
            (ext_part_sector == 0 || !is_extended (pt->sys_ind)) ) {
            print_one_part(pt, ext_part_sector, part_num, disksig);
        }

        /* Reverse engr the fdisk part# assignment rule! */
        if ((ext_part_sector == 0) ||
            (pt->sys_ind != 0 && !is_extended (pt->sys_ind)) ) {
            part_num++;
        }
    }

    /* Follows the extended partitions */
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++) {
        if (is_extended (pt->sys_ind)) {
            int lba_start = le32_to_int (pt->start4) + relative;

            print_partition_extended(dev_desc, lba_start,
                ext_part_sector == 0  ? lba_start : relative,
                part_num, disksig);
        }
    }

    return;
}


/*  Print a partition that is relative to its Extended partition table
 */
static int get_partition_info_extended (block_dev_desc_t *dev_desc, int ext_part_sector,
                 int relative, int part_num,
                 int which_part, disk_partition_t *info,
                 unsigned int disksig)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);
    dos_partition_t *pt;
    int i;
    int dos_type;

    if (dev_desc->block_read (dev_desc->dev, ext_part_sector, 1, (ulong *) buffer) != 1) {
        printf ("** Can't read partition table on %d:%d **\n",
            dev_desc->dev, ext_part_sector);
        return -1;
    }
    if (buffer[DOS_PART_MAGIC_OFFSET] != 0x55 ||
        buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa) {
        printf ("bad MBR sector signature 0x%02x%02x\n",
            buffer[DOS_PART_MAGIC_OFFSET],
            buffer[DOS_PART_MAGIC_OFFSET + 1]);
        return -1;
    }

#ifdef CONFIG_PARTITION_UUIDS
    if (!ext_part_sector)
        disksig = le32_to_int(&buffer[DOS_PART_DISKSIG_OFFSET]);
#endif

    /* Print all primary/logical partitions */
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++) {
        /*
         * fdisk does not show the extended partitions that
         * are not in the MBR
         */
        if (((pt->boot_ind & ~0x80) == 0) &&
            (pt->sys_ind != 0) &&
            (part_num == which_part) &&
            (is_extended(pt->sys_ind) == 0)) {
            info->blksz = DOS_PART_DEFAULT_SECTOR;
            info->start = (lbaint_t)(ext_part_sector +
                    le32_to_int(pt->start4));
            info->size  = (lbaint_t)le32_to_int(pt->size4);
            info->lba_start = ext_part_sector;
            switch(dev_desc->if_type) {
                case IF_TYPE_IDE:
                case IF_TYPE_SATA:
                case IF_TYPE_ATAPI:
                    sprintf ((char *)info->name, "hd%c%d",
                        'a' + dev_desc->dev, part_num);
                    break;
                case IF_TYPE_SCSI:
                    sprintf ((char *)info->name, "sd%c%d",
                        'a' + dev_desc->dev, part_num);
                    break;
                case IF_TYPE_USB:
                    sprintf ((char *)info->name, "usbd%c%d",
                        'a' + dev_desc->dev, part_num);
                    break;
                case IF_TYPE_DOC:
                    sprintf ((char *)info->name, "docd%c%d",
                        'a' + dev_desc->dev, part_num);
                    break;
                default:
                    sprintf ((char *)info->name, "xx%c%d",
                        'a' + dev_desc->dev, part_num);
                    break;
            }
            /* sprintf(info->type, "%d, pt->sys_ind); */
            sprintf ((char *)info->type, "U-Boot");
            info->bootable = is_bootable(pt);
#ifdef CONFIG_PARTITION_UUIDS
            sprintf(info->uuid, "%08x-%02x", disksig, part_num);
#endif
            return 0;
        }

        /* Reverse engr the fdisk part# assignment rule! */
        if ((ext_part_sector == 0) ||
            (pt->sys_ind != 0 && !is_extended (pt->sys_ind)) ) {
            part_num++;
        }
    }

    if(1==which_part && (1!=part_num))
    {
        printf("For 1st partition, skipping fdisk part# assignment rule and try again...\n");
        /* Print all primary/logical partitions */
        pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
        for (i = 0; i < 4; i++, pt++)
        {
            /*
             * fdisk does not show the extended partitions that
             * are not in the MBR
             */
            if (((pt->boot_ind & ~0x80) == 0) &&
                (pt->sys_ind != 0) &&
                (is_extended(pt->sys_ind) == 0)) {
                info->blksz = 512;
                info->start = ext_part_sector + le32_to_int (pt->start4);
                info->size  = le32_to_int (pt->size4);
                info->lba_start = ext_part_sector;
                switch(dev_desc->if_type) {
                    case IF_TYPE_IDE:
                    case IF_TYPE_SATA:
                    case IF_TYPE_ATAPI:
                        sprintf ((char *)info->name, "hd%c%d",
                            'a' + dev_desc->dev, part_num);
                        break;
                    case IF_TYPE_SCSI:
                        sprintf ((char *)info->name, "sd%c%d",
                            'a' + dev_desc->dev, part_num);
                        break;
                    case IF_TYPE_USB:
                        sprintf ((char *)info->name, "usbd%c%d",
                            'a' + dev_desc->dev, part_num);
                        break;
                    case IF_TYPE_DOC:
                        sprintf ((char *)info->name, "docd%c%d",
                            'a' + dev_desc->dev, part_num);
                        break;
                    default:
                        sprintf ((char *)info->name, "xx%c%d",
                            'a' + dev_desc->dev, part_num);
                        break;
                }
                /* sprintf(info->type, "%d, pt->sys_ind); */
                sprintf ((char *)info->type, "U-Boot");
                return 0;
            }

        }

    }

    /* Follows the extended partitions */
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++) {
        if (is_extended (pt->sys_ind)) {
            int lba_start = le32_to_int (pt->start4) + relative;

            return get_partition_info_extended (dev_desc, lba_start,
                 ext_part_sector == 0 ? lba_start : relative,
                 part_num, which_part, info, disksig);
        }
    }

    /* Check for DOS PBR if no partition is found */
    dos_type = test_block_type(buffer);

    if (dos_type == DOS_PBR) {
        info->start = 0;
        info->size = dev_desc->lba;
        info->blksz = DOS_PART_DEFAULT_SECTOR;
        info->bootable = 0;
        sprintf ((char *)info->type, "U-Boot");
#ifdef CONFIG_PARTITION_UUIDS
        info->uuid[0] = 0;
#endif
        return 0;
    }

    return -1;
}

void get_SDInfo(unsigned    int block_count, SDInfo *sdInfo)
{
    int C, H, S;

    int C_max = 1023, H_max = 255, S_max = 63;
    int H_start = 1, S_start = 1;
    int diff_min = 0, diff = 0;


#if defined(CONFIG_MACH_MESON8_ODROIDC)
    sdInfo->addr_mode = LBA_MODE;
#else
    if(block_count >= _8_4GB)
            sdInfo->addr_mode = LBA_MODE;
    else
            sdInfo->addr_mode = CHS_MODE;
#endif

    //-----------------------------------------------------
    if (sdInfo->addr_mode == CHS_MODE)
    {
        diff_min = C_max;

        for (H = H_start; H <= H_max; H++)
            for (S  = S_start; S <= S_max; S++)
            {
                C = block_count / (H * S);

                if ( (C <= C_max) )
                {
                    diff = C_max - C;
                    if (diff <= diff_min)
                    {
                        diff_min = diff;
                        sdInfo->C_end = C;
                        sdInfo->H_end = H;
                        sdInfo->S_end = S;
                    }
                }
            }

    }
    //-----------------------------------------------------
    else
    {
        sdInfo->C_end = 1023;
        sdInfo->H_end = 254;
        sdInfo->S_end = 63;
    }

    //-----------------------------------------------------
    sdInfo->C_start                 = 0;
    sdInfo->H_start                 = 1;
    sdInfo->S_start                 = 1;

    sdInfo->total_block_count       = block_count;
    sdInfo->available_block         = sdInfo->C_end * sdInfo->H_end * sdInfo->S_end;
    sdInfo->unit                    = sdInfo->H_end * sdInfo->S_end;
}

void make_partitionInfo(unsigned int LBA_start, unsigned int count, SDInfo sdInfo, PartitionInfo *partInfo, unsigned int ext_part_sector)
{
    int             _10MB_unit;

    partInfo->block_start   = LBA_start - ext_part_sector;

    //-----------------------------------------------------
    if (sdInfo.addr_mode == CHS_MODE)
    {
        partInfo->C_start       = LBA_start / sdInfo.unit;
        partInfo->H_start       = sdInfo.H_start - 1;
        partInfo->S_start       = sdInfo.S_start;

        if (count == BLOCK_END)
        {
            _10MB_unit = calc_unit(CFG_PARTITION_START, sdInfo);
            partInfo->block_end     = sdInfo.C_end * sdInfo.H_end * sdInfo.S_end - _10MB_unit - 1;
            partInfo->block_count   = partInfo->block_end - LBA_start + 1;

            partInfo->C_end = partInfo->block_end / sdInfo.unit;
            partInfo->H_end = sdInfo.H_end - 1;
            partInfo->S_end = sdInfo.S_end;
        }
        else
        {
            partInfo->block_count   = count;

            partInfo->block_end     = LBA_start + count - 1;
            partInfo->C_end         = partInfo->block_end / sdInfo.unit;

            partInfo->H_end         = sdInfo.H_end - 1;
            partInfo->S_end         = sdInfo.S_end;
        }
    }
    //-----------------------------------------------------
    else
    {
        partInfo->C_start       = 0;
        partInfo->H_start       = 1;
        partInfo->S_start       = 1;

        partInfo->C_end         = 1023;
        partInfo->H_end         = 254;
        partInfo->S_end         = 63;

        if (count == BLOCK_END)
        {
            _10MB_unit = calc_unit(CFG_PARTITION_START, sdInfo);
            partInfo->block_end     = sdInfo.total_block_count - _10MB_unit - 1;
            partInfo->block_count   = partInfo->block_end - LBA_start + 1;

        }
        else
        {
            partInfo->block_count   = count;
            partInfo->block_end     = LBA_start + count - 1;
        }
    }
}


int make_mmc_partition(block_dev_desc_t *dev_desc,  int part_start_pre, int part_size_pre, int ext_part_sector, int relative, unsigned int part_num,  unsigned int part_size)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);
    dos_partition_t *pt;
    SDInfo sdInfo;
    unsigned int block_start = 0, block_offset;
    PartitionInfo   partInfo;
    int ext_part_lba;

    get_SDInfo(dev_desc->lba, &sdInfo);

    if (dev_desc->block_read (dev_desc->dev, ext_part_sector, 1, (ulong *) buffer) != 1)
    {
        printf ("** Can't read partition table on %d:%d **\n", dev_desc->dev, ext_part_sector);
        return 1;
    }

    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);

    partInfo.bootable       = 0x00;

    switch(part_num)
    {
        case 1:
            partInfo.partitionId    = 0x0c;
            block_start = calc_unit((lbaint_t)CFG_PARTITION_START, sdInfo);
            block_offset = calc_unit(part_size, sdInfo);
            break;
        case 2:
        case 3:
            partInfo.partitionId    = 0x83;
            block_start = calc_unit((lbaint_t)(part_start_pre + part_size_pre), sdInfo);
            block_offset = calc_unit(part_size, sdInfo);
            break;
        case 4:
            partInfo.partitionId    = 0x05;
            block_start = calc_unit((lbaint_t)(part_start_pre + part_size_pre), sdInfo);
            block_offset = calc_unit(dev_desc->lba - block_start - sdInfo.unit, sdInfo);
            break;
        default:
            partInfo.partitionId    = 0x05;
            block_start = calc_unit((lbaint_t)(part_start_pre + part_size_pre), sdInfo);
            block_offset = calc_unit(part_size + CFG_EXT_PARTITION_RESERVE, sdInfo);
            break;
    }

    if(dev_desc->lba < block_start + block_offset)
        block_offset = dev_desc->lba - block_start - 1;

    make_partitionInfo(block_start, block_offset, sdInfo, &partInfo, relative);

    if(part_num <= 4)
        encode_partitionInfo(partInfo, (unsigned char *)&pt[part_num-1]);
    else
        encode_partitionInfo(partInfo, (unsigned char *)&pt[1]);

    buffer[DOS_PART_MAGIC_OFFSET] = 0x55;
    buffer[DOS_PART_MAGIC_OFFSET + 1] = 0xaa;

    dev_desc->block_write(dev_desc->dev, ext_part_sector, 1, buffer);

    if(partInfo.partitionId == 0x05)
    {
        ext_part_lba = block_start;
        block_start += CFG_EXT_PARTITION_RESERVE;
        block_offset = calc_unit(block_start + part_size, sdInfo) - block_start;

        partInfo.bootable       = 0x00;
        partInfo.partitionId    = 0x83;
        make_partitionInfo(block_start, block_offset, sdInfo, &partInfo, ext_part_lba);

        memset(buffer, 0, dev_desc->blksz);
        encode_partitionInfo(partInfo, (unsigned char *)&pt[0]);
        buffer[DOS_PART_MAGIC_OFFSET] = 0x55;
        buffer[DOS_PART_MAGIC_OFFSET + 1] = 0xaa;

        dev_desc->block_write(dev_desc->dev, ext_part_lba, 1, buffer);
    }

    return 0;
}



/*  create a partition that is relative to its Extended partition table
 */
static int create_partition_info_extended (block_dev_desc_t *dev_desc, int ext_part_sector,
                 int relative, int part_num, unsigned int part_size )
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);
    dos_partition_t *pt;
    int i;
    int ret = 0;
    int part_table;
    int part_start_pre = 0, part_size_pre = 0;

    if(relative==0)
        part_table = 4;
    else
        part_table = 2;

    if (dev_desc->block_read (dev_desc->dev, ext_part_sector, 1, (ulong *) buffer) != 1)
    {
        printf ("** Can't read partition table on %d:%d **\n", dev_desc->dev, ext_part_sector);
        return 1;
    }
    if (buffer[DOS_PART_MAGIC_OFFSET] != 0x55 || buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa)
    {
        if(part_num == 1)
        {
            goto make_partition;
        }
        else
        {
            printf ("bad MBR sector signature 0x%02x%02x\n", buffer[DOS_PART_MAGIC_OFFSET], buffer[DOS_PART_MAGIC_OFFSET + 1]);
            return 1;
        }
    }

    /* Print all primary/logical partitions */
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < part_table; i++, pt++)
    {
        if (((pt->boot_ind & ~0x80) == 0) && (pt->sys_ind != 0) && (ext_part_sector == 0 || !is_extended (pt->sys_ind)) )
        {
            part_start_pre = (lbaint_t)(ext_part_sector +le32_to_int(pt->start4));
            part_size_pre = (lbaint_t)le32_to_int(pt->size4);
            part_num++;
            continue;
        }
        else if(!is_extended (pt->sys_ind))
        {
            goto make_partition;
        }
    }

    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < part_table; i++, pt++)
    {
        if (is_extended (pt->sys_ind) )
        {
            int lba_start = le32_to_int (pt->start4) + relative;

            ret = create_partition_info_extended(dev_desc, lba_start, ext_part_sector == 0  ? lba_start : relative, part_num, part_size);
        }
    }

    return ret;

make_partition:
    ret = make_mmc_partition(dev_desc, part_start_pre, part_size_pre, ext_part_sector, relative, part_num, part_size);
    if(ret)
        return ret;

    gu32_PartNum++;

    return ret;
}


static int delete_partition_info_extended (block_dev_desc_t *dev_desc, int ext_part_sector,
            int relative, int part_num, unsigned int erase_parts )
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);
    dos_partition_t *pt;
    int i, ret = 0, lba_start;

    if (dev_desc->block_read (dev_desc->dev, ext_part_sector, 1, (ulong *) buffer) != 1)
    {
        printf ("** Can't read partition table on %d:%d **\n", dev_desc->dev, ext_part_sector);
        return -1;
    }
    if (buffer[DOS_PART_MAGIC_OFFSET] != 0x55 || buffer[DOS_PART_MAGIC_OFFSET + 1] != 0xaa)
    {
        {
            printf ("bad MBR sector signature 0x%02x%02x\n", buffer[DOS_PART_MAGIC_OFFSET], buffer[DOS_PART_MAGIC_OFFSET + 1]);
            return -1;
        }
    }

    /* Print all primary/logical partitions */

    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++)
    {
        if (((pt->boot_ind & ~0x80) == 0) && (pt->sys_ind != 0) && (!is_extended (pt->sys_ind)) && (part_num == erase_parts))
        {
            memset(pt, 0 , sizeof(dos_partition_t));
            dev_desc->block_write(dev_desc->dev, ext_part_sector, 1, buffer);
            return 1;
        }

        /* Reverse engr the fdisk part# assignment rule! */
        if ((ext_part_sector == 0) || (pt->sys_ind != 0 && !is_extended (pt->sys_ind)) )
        {
            part_num++;
        }
    }
    pt = (dos_partition_t *) (buffer + DOS_PART_TBL_OFFSET);
    for (i = 0; i < 4; i++, pt++)
    {
        if (is_extended (pt->sys_ind) )
        {
            lba_start = le32_to_int (pt->start4) + relative;

            ret = delete_partition_info_extended(dev_desc, lba_start, ext_part_sector == 0  ? lba_start : relative, part_num, erase_parts);
            if(ret == 1)
            {
                memset(pt, 0, sizeof(dos_partition_t));
                dev_desc->block_write(dev_desc->dev, ext_part_sector, 1, buffer);
                return 0;
            }
        }
    }
    return ret;

}

int test_part_dos (block_dev_desc_t *dev_desc)
{
    ALLOC_CACHE_ALIGN_BUFFER(unsigned char, buffer, dev_desc->blksz);

    if (dev_desc->block_read(dev_desc->dev, 0, 1, (ulong *) buffer) != 1)
        return -1;

    if (test_block_type(buffer) != DOS_MBR)
        return -1;

    return 0;
}

void print_part_dos (block_dev_desc_t *dev_desc)
{
    gu32_PartNum = 0;
    printf("Part\tStart Sector\tNum Sectors\tUUID\t\tType\n");
    print_partition_extended(dev_desc, 0, 0, 1, 0);
}

int get_partition_info_dos (block_dev_desc_t *dev_desc, int part, disk_partition_t * info)
{
    return get_partition_info_extended(dev_desc, 0, 0, 1, part, info, 0);
}

int create_partition_dos(block_dev_desc_t *dev_desc, unsigned int part_size )
{
    return create_partition_info_extended(dev_desc, 0, 0, 1, part_size);
}

int delete_partition_dos(block_dev_desc_t *dev_desc, unsigned int erase_parts , int upgrade_flag)
{
    int ret = 0;
    if(gu32_PartNum == 0)
    {
        print_part_dos(dev_desc);
    }

    if(erase_parts > gu32_PartNum || erase_parts < 0)
    {
        printf("Partitino %u don't exist, max partition NO %u\n", erase_parts, gu32_PartNum);
        return -1;
    }

    if(erase_parts == 0)
    {
        while(gu32_PartNum)
        {
            if( upgrade_flag && (gu32_PartNum == 1) )
                break;

            ret = delete_partition_info_extended(dev_desc, 0, 0, 1, gu32_PartNum);
            if(ret == -1)
            {
                printf("Delete partition %u fail!\n", gu32_PartNum);
                return ret;
            }

            gu32_PartNum--;
        }
    }
    else
    {
        ret = delete_partition_info_extended(dev_desc, 0, 0, 1, erase_parts);
        if(ret != -1)
            gu32_PartNum--;
    }
    return ret;
}

#endif
