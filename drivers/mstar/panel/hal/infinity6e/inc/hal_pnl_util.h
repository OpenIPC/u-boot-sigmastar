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

#ifndef __HAL_PNL_UTIL_H__
#define __HAL_PNL_UTIL_H__

extern u32 UTILITY_RIU_BASE_Vir;     // This should be inited before XC library starting.


#define PNL_IO_OFFSET 0xDE000000

#define PNL_IO_ADDRESS(x)           ( (u32)(x) + PNL_IO_OFFSET )
//#define __io_address(n)       ((void __iomem __force *)PNL_IO_ADDRESS(n))



#define RIU_READ_BYTE(addr)         ( READ_BYTE( 0x1F000000+ (addr) ) )
#define RIU_READ_2BYTE(addr)        ( READ_WORD( 0x1F000000 + (addr) ) )
#define RIU_READ_4BYTE(addr)        ( READ_LONG( 0x1F000000 + (addr) ) )

#define RIU_WRITE_BYTE(addr, val)    WRITE_BYTE( (0x1F000000 + (addr)), val)
#define RIU_WRITE_2BYTE(addr, val)   WRITE_WORD( 0x1F000000 + (addr), val)
#define RIU_WRITE_4BYTE(addr, val)   WRITE_LONG( 0x1F000000 + (addr), val)
#define BANKSIZE 512
#define RIU_GET_ADDR(addr)         ((void*)(UTILITY_RIU_BASE_Vir+ ((addr)<<1)) )

#define RBYTE( u32Reg ) RIU_READ_BYTE( (u32Reg) << 1)

#define R2BYTE( u32Reg ) RIU_READ_2BYTE( (u32Reg) << 1)

#define R2BYTEMSK( u32Reg, u16mask)\
    ( ( RIU_READ_2BYTE( (u32Reg)<< 1) & u16mask  ) )

#define R4BYTE( u32Reg )\
    ( {  (RIU_READ_4BYTE( (u32Reg)<<1)) ; } )

#define R4BYTEMSK( u32Reg, u32Mask) \
    ( RIU_READ_4BYTE( (u32Reg) << 1) & u32Mask  )

#define WBYTE(u32Reg, u8Val) RIU_WRITE_BYTE( ((u32Reg) << 1), u8Val )

#define WBYTEMSK(u32Reg, u8Val, u8Mask) \
               RIU_WRITE_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)), ( RIU_READ_BYTE( (((u32Reg)<<1) - ((u32Reg) & 1)) ) & ~(u8Mask)) | ((u8Val) & (u8Mask)) )

#define W2BYTE( u32Reg, u16Val) RIU_WRITE_2BYTE( (u32Reg) << 1 , u16Val )

#define W2BYTEMSK( u32Reg, u16Val, u16Mask)\
              RIU_WRITE_2BYTE( (u32Reg)<< 1 , (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)) )

#if 0
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1, ((u32Val) & 0x0000FFFF) ); \
                RIU_WRITE_2BYTE( ( (u32Reg) + 2) << 1 , (((u32Val) >> 16) & 0x0000FFFF)) ; } )
#else
#define W4BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_4BYTE( (u32Reg)<<1, u32Val);})
#endif

#define W4BYTEMSK( u32Reg, u32Val, u32Mask)\
                  RIU_WRITE_4BYTE( (u32Reg)<< 1 , (RIU_READ_4BYTE((u32Reg) << 1) & ~(u32Mask)) | ((u32Val) & (u32Mask)) )


#define W3BYTE( u32Reg, u32Val)\
            ( { RIU_WRITE_2BYTE( (u32Reg) << 1,  u32Val); \
                RIU_WRITE_BYTE( (u32Reg + 2) << 1 ,  ((u32Val) >> 16)); } )



#define WriteLongRegBit( u32Reg, bEnable, u32Mask)      \
    RIU_WRITE_4BYTE((u32Reg)<<1,  (bEnable) ? RIU_READ_4BYTE((u32Reg)<<1)| u32Mask :  \
    RIU_READ_4BYTE((u32Reg)<<1) & (~u32Mask));


#define MApi_XC_ReadByte(u32Reg)                    RBYTE(u32Reg)
#define MApi_XC_WriteByteMask(u32Reg, u8Val, u8Msk) WBYTEMSK( u32Reg, u8Val, u8Msk )
#define MApi_XC_WriteByte(u32Reg, u8Val)            WBYTE( u32Reg, u8Val )

#define MApi_XC_R2BYTE(u32Reg)                      R2BYTE( u32Reg )
#define MApi_XC_R2BYTEMSK(u32Reg, u16mask)          R2BYTEMSK( u32Reg, u16mask)

#define MApi_XC_W2BYTE(u32Reg, u16Val)              W2BYTE( u32Reg, u16Val)
#define MApi_XC_W2BYTEMSK(u32Reg, u16Val, u16mask)  W2BYTEMSK( u32Reg, u16Val, u16mask)


#define BIT31                          (0b10000000000000000000000000000000)
#define BIT30                          (0b01000000000000000000000000000000)
#define BIT29                          (0b00100000000000000000000000000000)
#define BIT28                          (0b00010000000000000000000000000000)
#define BIT27                          (0b00001000000000000000000000000000)
#define BIT26                          (0b00000100000000000000000000000000)
#define BIT25                          (0b00000010000000000000000000000000)
#define BIT24                          (0b00000001000000000000000000000000)
#define BIT23                          (0b00000000100000000000000000000000)
#define BIT22                          (0b00000000010000000000000000000000)
#define BIT21                          (0b00000000001000000000000000000000)
#define BIT20                          (0b00000000000100000000000000000000)
#define BIT19                          (0b00000000000010000000000000000000)
#define BIT18                          (0b00000000000001000000000000000000)
#define BIT17                          (0b00000000000000100000000000000000)
#define BIT16                          (0b00000000000000010000000000000000)
#define BIT15                          (0b00000000000000001000000000000000)
#define BIT14                          (0b00000000000000000100000000000000)
#define BIT13                          (0b00000000000000000010000000000000)
#define BIT12                          (0b00000000000000000001000000000000)
#define BIT11                          (0b00000000000000000000100000000000)
#define BIT10                          (0b00000000000000000000010000000000)
#define BIT9                           (0b00000000000000000000001000000000)
#define BIT8                           (0b00000000000000000000000100000000)
#define BIT7                           (0b00000000000000000000000010000000)
#define BIT6                           (0b00000000000000000000000001000000)
#define BIT5                           (0b00000000000000000000000000100000)
#define BIT4                           (0b00000000000000000000000000010000)
#define BIT3                           (0b00000000000000000000000000001000)
#define BIT2                           (0b00000000000000000000000000000100)
#define BIT1                           (0b00000000000000000000000000000010)
#define BIT0                           (0b00000000000000000000000000000001)




extern s8 UartSendTrace(const char *strFormat, ...);
#endif /* __HAL_PNL_UTIL_H__ */
