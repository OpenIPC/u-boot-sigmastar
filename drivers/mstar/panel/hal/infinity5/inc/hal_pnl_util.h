/*
* hal_pnl_util.h- Sigmastar
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

#ifndef __HAL_PNL_UTIL_H__
#define __HAL_PNL_UTIL_H__

extern u32 UTILITY_RIU_BASE_Vir;     // This should be inited before XC library starting.



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


#define _BIT0                           0x00000001
#define _BIT1                           0x00000002
#define _BIT2                           0x00000004
#define _BIT3                           0x00000008
#define _BIT4                           0x00000010
#define _BIT5                           0x00000020
#define _BIT6                           0x00000040
#define _BIT7                           0x00000080
#define _BIT8                           0x00000100
#define _BIT9                           0x00000200
#define _BIT10                          0x00000400
#define _BIT11                          0x00000800
#define _BIT12                          0x00001000
#define _BIT13                          0x00002000
#define _BIT14                          0x00004000
#define _BIT15                          0x00008000
#define _BIT16                          0x00010000
#define _BIT17                          0x00020000
#define _BIT18                          0x00040000
#define _BIT19                          0x00080000
#define _BIT20                          0x00100000
#define _BIT21                          0x00200000
#define _BIT22                          0x00400000
#define _BIT23                          0x00800000
#define _BIT24                          0x01000000
#define _BIT25                          0x02000000
#define _BIT26                          0x04000000
#define _BIT27                          0x08000000
#define _BIT28                          0x10000000
#define _BIT29                          0x20000000
#define _BIT30                          0x40000000
#define _BIT31                          0x80000000



extern s8 UartSendTrace(const char *strFormat, ...);
#endif /* __HAL_PNL_UTIL_H__ */
