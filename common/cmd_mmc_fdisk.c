/*
* cmd_mmc_fdisk.c- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: jz.xiang <jz.xiang@sigmastar.com.cn>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/


#include <common.h>
#include <command.h>
#include <mmc.h>
#include "part.h"

/////////////////////////////////////////////////////////////////
static int read_partition(block_dev_desc_t *dev_desc, disk_partition_t info, unsigned char *buffer, int offset, int size)
{
    if(offset < 0 || size < 0 || (offset + size) >  info.size )
    {
        printf("Illegal parameter : offset=%X, size=%X, while partition size=%lX \n", offset, size, info.size);
        return -1;
    }
    return dev_desc->block_read(dev_desc->dev, info.start + offset, size, buffer);
}

/////////////////////////////////////////////////////////////////
static int write_partition(block_dev_desc_t *dev_desc, disk_partition_t info, unsigned char *buffer, int offset, int size)
{
    if(offset < 0 || size < 0 || (offset + size) >  info.size )
    {
        printf("Illegal parameter : offset=%X, size=%X, while partition size=%lX \n", offset, size, info.size);
        return -1;
    }
    return dev_desc->block_write(dev_desc->dev, info.start + offset, size, buffer);
}

/////////////////////////////////////////////////////////////////
static int erase_partition(block_dev_desc_t *dev_desc, disk_partition_t info, int offset, int size)
{
    if(offset < 0 || size < 0 || (offset + size) >  info.size )
    {
        printf("Illegal parameter : offset=%X, size=%X, while partition size=%lX \n", offset, size, info.size);
        return -1;
    }
    return dev_desc->block_erase(dev_desc->dev, info.start + offset, size);
}

/////////////////////////////////////////////////////////////////
int print_mmc_part_info(int argc, char * const argv[])
{
    int dev;
    block_dev_desc_t *dev_desc;

    if(argc != 3)
        return CMD_RET_USAGE;

    dev = get_device("mmc", argv[2], &dev_desc);
    if(dev < 0)
        return CMD_RET_FAILURE;

    print_part_dos (dev_desc);

    return CMD_RET_SUCCESS;
}

/////////////////////////////////////////////////////////////////
int create_mmc_fdisk(int argc, char * const argv[])
{
    int part_size, ret;
    block_dev_desc_t *dev_desc;

    if(argc != 4)
        return CMD_RET_USAGE;

    get_device("mmc", argv[2], &dev_desc);

    part_size = simple_strtoul(argv[3], NULL, 16);

    ret = create_partition_dos(dev_desc, part_size);
    if(ret)
        return CMD_RET_FAILURE;
    else
        return CMD_RET_SUCCESS;
}


/////////////////////////////////////////////////////////////////
int delete_mmc_fdisk(int argc, char * const argv[])
{
    int erase_parts, ret;
    block_dev_desc_t *dev_desc;
    int upgrade_flag=0;

    if(argc == 4)
         erase_parts = simple_strtoul(argv[3], NULL, 16);
     else if(argc == 3)
         erase_parts = 0;
     else
         return CMD_RET_USAGE;

    get_device("mmc", argv[2], &dev_desc);
    if(!strcmp(argv[1], "-d.up"))
        upgrade_flag=1;

    ret = delete_partition_dos(dev_desc, erase_parts, upgrade_flag);
    if(ret == -1)
        printf("delete partition fail\n");

    return CMD_RET_SUCCESS;
}

/////////////////////////////////////////////////////////////////
int read_mmc_fdisk(int argc, char * const argv[])
{
    int part, ret;
    block_dev_desc_t *dev_desc;
    disk_partition_t info;
    unsigned long addr;
    unsigned int offset;
    unsigned int size;

    if(argc != 6)
        return CMD_RET_USAGE;

    addr = simple_strtoul(argv[3], NULL, 16);
    offset = simple_strtoul(argv[4], NULL, 16);
    size = simple_strtoul(argv[5], NULL, 16);

    part = get_device_and_partition("mmc", argv[2], &dev_desc, &info, 1);
    if (part < 0)
        return 1;

    ret = read_partition(dev_desc, info, (unsigned char *)addr, offset, size);
    if(ret >= 0)
    {
        printf("fdisk read pass, start:%lX, offset:%X, size:%X, address:%lX\n", info.start, offset, size, addr);
        return CMD_RET_SUCCESS;
    }

    return CMD_RET_FAILURE;
}

/////////////////////////////////////////////////////////////////
int write_mmc_fdisk(int argc, char * const argv[])
{
    int part, ret;
    block_dev_desc_t *dev_desc;
    disk_partition_t info;
    unsigned long addr;
    unsigned int offset;
    unsigned int size;

    if(argc != 6)
        return CMD_RET_USAGE;

    addr = simple_strtoul(argv[3], NULL, 16);
    offset = simple_strtoul(argv[4], NULL, 16);
    size = simple_strtoul(argv[5], NULL, 16);

    part = get_device_and_partition("mmc", argv[2], &dev_desc, &info, 1);
    if (part < 0)
        return 1;

    ret = write_partition(dev_desc, info, (unsigned char *)addr, offset, size);
    if(ret >= 0)
    {
        printf("fdisk write pass, start:%lX, offset:%X, size:%X, address:%lX\n", info.start, offset, size, addr);
        return CMD_RET_SUCCESS;
    }

    return CMD_RET_FAILURE;
}

/////////////////////////////////////////////////////////////////
int erase_mmc_fdisk(int argc, char * const argv[])
{
    int part, ret;
    block_dev_desc_t *dev_desc;
    disk_partition_t info;
    unsigned int offset;
    unsigned int size;

    part = get_device_and_partition("mmc", argv[2], &dev_desc, &info, 1);
    if (part < 0)
        return 1;

    if(argc == 5)
    {
        offset = simple_strtoul(argv[3], NULL, 16);
        size = simple_strtoul(argv[4], NULL, 16);
    }
    else if(argc == 3)
    {
        offset = 0;
        size  = info.size;
    }
    else
        return CMD_RET_USAGE;

    ret = erase_partition(dev_desc, info, offset, size);
    if(ret >= 0)
    {
        printf("fdisk erase pass, start:%lX, offset:%X, size:%X\n", info.start, offset, size);
        return CMD_RET_SUCCESS;
    }

    return CMD_RET_FAILURE;
}

static int do_fdisk(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    if ( strcmp(argv[1], "-c") == 0 )
        return create_mmc_fdisk(argc, argv);

    else if ( (strcmp(argv[1], "-d") == 0) || (strcmp(argv[1], "-d.up") == 0) )
        return delete_mmc_fdisk(argc, argv);

    else if ( strcmp(argv[1], "-p") == 0 )
        return print_mmc_part_info(argc, argv);

    else if ( strcmp(argv[1], "-r") == 0 )
        return read_mmc_fdisk(argc, argv);

    else if ( strcmp(argv[1], "-w") == 0 )
        return write_mmc_fdisk(argc, argv);

    else if ( strcmp(argv[1], "-e") == 0 )
        return erase_mmc_fdisk(argc, argv);

    else
        return CMD_RET_USAGE;
}


U_BOOT_CMD(
    fdisk, 6, 0, do_fdisk,
    "fdisk\t- fdisk for sd/mmc.\n",
    "fdisk -p <device_num>\t- print partition information\n"
    "fdisk -c <device_num> <part size(block)> \t- create partition.\n"
    "fdisk -d <device_num> [<part_numbers>]\t- delete the last <part_numbers> of partition, input <0> or empty delete all partition.\n"
    "fdisk -r <device_num:part_num> <address> <offset(block)> <size(block)> \t- read data from partition.\n"
    "fdisk -w <device_num:part_num> <address> <offset(block)> <size(block)> \t- write data to partition.\n"
    "fdisk -e <device_num:part_num> [<offset(block)> <size(block)>] \t- erase the partition.\n"
);

