/*
* rgn_file_access.c- Sigmastar
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

//==============================================================================
//
//                              INCLUDE FILES
//
//==============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/unistd.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/buffer_head.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include "cam_os_wrapper.h"

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

struct file *OpenFile(char *path,int flag,int mode)
{
    struct file *filp = NULL;
    mm_segment_t oldfs;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flag, mode);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        return NULL;
    }
    return filp;
}

int ReadFile(struct file *fp, char *buf,int readlen)
{
    mm_segment_t oldfs;
    int ret;
    // loff_t pos = fp->f_pos;
    oldfs = get_fs();
    set_fs(get_ds());
    ret = vfs_read(fp, buf, readlen, &fp->f_pos);
    // fp->f_pos = pos;
    set_fs(oldfs);
    return ret;
}

int WriteFile(struct file *fp,char *buf,int writelen)
{
    mm_segment_t oldfs;
    int ret;
    // loff_t pos = fp->f_pos;
    oldfs = get_fs();
    set_fs(get_ds());
    ret = vfs_write(fp, buf, writelen, &fp->f_pos);
    // fp->f_pos = pos;
    set_fs(oldfs);
    return ret;
}

int CloseFile(struct file *fp)
{
    filp_close(fp, NULL);
    return 0;
}
