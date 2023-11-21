/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/


/*
 * lwfs.h
 */

#ifndef __FIRMWAREFS_H__
#define __FIRMWAREFS_H__

#include "fwfs.h"
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

#ifndef O_APPEND
#define O_APPEND    00002000
#endif

#ifndef O_TRUNC
#define O_TRUNC     00001000
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


S32 firmwarefs_mount(char *partition, char *mnt_path);
void firmwarefs_unmount(void);
void* firmwarefs_open(char *filename, U32  flags, U32 mode);
S32 firmwarefs_close(void* fd);
S32 firmwarefs_read(void* fd, void *buf, U32 count);
S32 firmwarefs_write(void* fd, void *buf, U32 count);
S32 firmwarefs_lseek(void* fd, S32 offset, S32 whence);

#endif //__LWFS_H__
