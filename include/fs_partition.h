#ifndef __FS_PARTITION_H__
#define __FS_PARTITION_H__

#ifndef U32
#define U32  unsigned long
#endif
#ifndef U16
#define U16  unsigned short
#endif
#ifndef U8
#define U8   unsigned char
#endif
#ifndef S32
#define S32  signed long
#endif
#ifndef S16
#define S16  signed short
#endif
#ifndef S8
#define S8   signed char
#endif

typedef struct
{
    U8  u8_UseExternBlockSize;
    U16 u16_StartBlk;
    U16 u16_BlkCnt;
    U32 u32_PageSize;
    U32 u32_BlkSize;
} FS_PARTITION_Info_t;
#endif
