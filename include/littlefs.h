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
 * lwfs.h
 */

#ifndef __LITTLEFS_H__
#define __LITTLEFS_H__
#include "lfs.h"
#include "fs_partition.h"


#ifndef O_CREAT
#define O_CREAT     00000100
#endif
#ifndef O_PATH
#define O_PATH      010000000
#endif
#ifndef O_RDWR
#define O_RDWR      00000002
#endif
#ifndef O_WRONLY
#define O_WRONLY    00000001
#endif
#ifndef O_RDONLY
#define O_RDONLY    00000000
#endif
#ifndef O_ACCMODE
#define O_ACCMODE   00000003
#endif


#ifndef SEEK_SET
#define SEEK_SET    0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif
#ifndef SEEK_END
#define SEEK_END    2
#endif


S32 littlefs_mount(char *partition, char *mnt_path);
void littlefs_unmount(void);
void* littlefs_open(char *filename, U32  flags, U32 mode);
S32 littlefs_close(void* fd);
S32 littlefs_read(void* fd, void *buf, U32 count);
S32 littlefs_write(void* fd, void *buf, U32 count);
S32 littlefs_lseek(void* fd, S32 offset, S32 whence);

#endif //__LWFS_H__
