/*
 * hal_jpd_ops.c- Sigmastar
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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#if !defined(__I_SW__)
#include <linux/string.h>
#endif
#include "mhal_common.h"
#include "hal_jpd_ops.h"
#include "hal_jpd_ios.h"
#include "hal_jpd_def.h"
#include <cam_os_wrapper.h>

#define ENABLE_FF00FFD_TO_FFFFFFD FALSE

#if ENABLE_FF00FFD_TO_FFFFFFD
#include "../wdt/drvWDT.h"
#endif

#define HVD_FW_MEM_OFFSET 0x100000UL // 1M
#define NJPD_OVER_4G_ADDR (0x300000000ULL)

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------
static NJPD_STATUS g_stNjpdStatus = {
    0,     // Current MRC address
    0,     // Current decoded Vidx
    0,     // Current decoded Row
    0,     // Current decoded Col
    FALSE, // busy or not
    FALSE  // Isr is enabled or not
};

NJPD_DECODE_CTRL_S g_stJpdDecodeCtrl[MAX_JPD_DEVICE_NUM];

// This is for zigzag inverse for njpd
static const MS_U8 g_u8JpdZigzagOrder[64] = {0,  2,  3,  9,  10, 20, 21, 35, 1,  4,  8,  11, 19, 22, 34, 36,
                                             5,  7,  12, 18, 23, 33, 37, 48, 6,  13, 17, 24, 32, 38, 47, 49,
                                             14, 16, 25, 31, 39, 46, 50, 57, 15, 26, 30, 40, 45, 51, 56, 58,
                                             27, 29, 41, 44, 52, 55, 59, 62, 28, 42, 43, 53, 54, 60, 61, 63};

static const MS_U16 g_u16IQTable[128] = {
    0x0010, 0x000c, 0x000e, 0x000e, 0x0012, 0x0018, 0x0031, 0x0048, 0x000b, 0x000c, 0x000d, 0x0011, 0x0016,
    0x0023, 0x0040, 0x005c, 0x000a, 0x000e, 0x0010, 0x0016, 0x0025, 0x0037, 0x004e, 0x005f, 0x0010, 0x0013,
    0x0018, 0x001d, 0x0038, 0x0040, 0x0057, 0x0062, 0x0018, 0x001a, 0x0028, 0x0033, 0x0044, 0x0051, 0x0067,
    0x0070, 0x0028, 0x003a, 0x0039, 0x0057, 0x006d, 0x0068, 0x0079, 0x0064, 0x0033, 0x003c, 0x0045, 0x0050,
    0x0067, 0x0071, 0x0078, 0x0067, 0x003d, 0x0037, 0x0038, 0x003e, 0x004d, 0x005c, 0x0065, 0x0063,

    0x0011, 0x0012, 0x0018, 0x002f, 0x0063, 0x0063, 0x0063, 0x0063, 0x0012, 0x0015, 0x001a, 0x0042, 0x0063,
    0x0063, 0x0063, 0x0063, 0x0018, 0x001a, 0x0038, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x002f, 0x0042,
    0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063,
    0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063,
    0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063, 0x0063

};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

void MapiJpdPrintMemory(MS_VIRT u32Addr, MS_U32 u32Size)
{
    MS_U32 u32i;
    JPD_LOGD("===========================================================\n");
    JPD_LOGD("print memory addr=0x%tx, size=0x%tx\n", (ptrdiff_t)u32Addr, (ptrdiff_t)u32Size);
    JPD_LOGD("===========================================================\n");
#if 1
    for (u32i = 0; u32i < u32Size / 8 + ((u32Size % 8) ? 1 : 0); u32i++)
    {
        JPD_LOGD("%02x %02x %02x %02x %02x %02x %02x %02x \n", *((MS_U8 *)(u32Addr + u32i * 8)),
                 *((MS_U8 *)(u32Addr + u32i * 8 + 1)), *((MS_U8 *)(u32Addr + u32i * 8 + 2)),
                 *((MS_U8 *)(u32Addr + u32i * 8 + 3)), *((MS_U8 *)(u32Addr + u32i * 8 + 4)),
                 *((MS_U8 *)(u32Addr + u32i * 8 + 5)), *((MS_U8 *)(u32Addr + u32i * 8 + 6)),
                 *((MS_U8 *)(u32Addr + u32i * 8 + 7)));
    }
#else
    for (u32i = 0; u32i < u32Size / 16 + ((u32Size % 16) ? 1 : 0); u32i++)
    {
        JPD_LOGD("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x \n",
                 *((MS_U8 *)(u32Addr + u32i * 16)), *((MS_U8 *)(u32Addr + u32i * 16 + 1)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 2)), *((MS_U8 *)(u32Addr + u32i * 16 + 3)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 4)), *((MS_U8 *)(u32Addr + u32i * 16 + 5)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 6)), *((MS_U8 *)(u32Addr + u32i * 16 + 7)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 8)), *((MS_U8 *)(u32Addr + u32i * 16 + 9)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 10)), *((MS_U8 *)(u32Addr + u32i * 16 + 11)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 12)), *((MS_U8 *)(u32Addr + u32i * 16 + 13)),
                 *((MS_U8 *)(u32Addr + u32i * 16 + 14)), *((MS_U8 *)(u32Addr + u32i * 16 + 15)));
    }
#endif
    JPD_LOGD("===========================================================\n");
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

/********************************************************************/
/// Reset NJPD -- Reset must be called before trigger NJPD
///@param NULL
///@return none
/********************************************************************/
void MapiJpdReset(MS_U8 DevId)
{
    MhalJpdRst(DevId);
}

/*
void MDrv_JPD_SetVerificationMode(int mode)
{
    HAL_NJPD_SetVerificationMode((NJPD_VERIFICATION_MODE)mode);
}*/
NJPD_VERIFICATION_MODE MapiJpdGetVerificationMode(void)
{
    return 0;
}

void MapiJpdSetReadBuffer0(MS_U8 DevId, MS_PHY u32BufAddr, MS_U32 u32BufSize)
{
    MS_U8               u8MiuSel   = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    MhalJpdSetMIU(DevId, u8MiuSel);

    pstDecCtrl->u32MRC0_Start = u32BufAddr;

    // set start address of read buffer
    MhalJpdSetMRCBuf0StartLow(DevId, u32BufAddr & 0xffffUL);
    MhalJpdSetMRCBuf0StartHigh(DevId, u32BufAddr >> 16);

    pstDecCtrl->u32MRC0_End = ((u32BufAddr + u32BufSize) - 1);
    // set end address of read buffer
    MhalJpdSetMRCBuf0EndLow(DevId, ((u32BufAddr + u32BufSize) - 1) & 0xffffUL);
    MhalJpdSetMRCBuf0EndHigh(DevId, ((u32BufAddr + u32BufSize) - 1) >> 16);
}

void MapiJpdSetReadBuffer1(MS_U8 DevId, MS_PHY u32BufAddr, MS_U32 u32BufSize)
{
    MS_U8               u8MiuSel   = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    MhalJpdSetMIU(DevId, u8MiuSel);

    pstDecCtrl->u32MRC1_Start = u32BufAddr;

    // set start address of read buffer
    MhalJpdSetMRCBuf1StartLow(DevId, u32BufAddr & 0xffffUL);
    MhalJpdSetMRCBuf1StartHigh(DevId, u32BufAddr >> 16);

    pstDecCtrl->u32MRC1_End = ((u32BufAddr + u32BufSize) - 1);
    // set end address of read buffer
    MhalJpdSetMRCBuf1EndLow(DevId, ((u32BufAddr + u32BufSize) - 1) & 0xffffUL);
    MhalJpdSetMRCBuf1EndHigh(DevId, ((u32BufAddr + u32BufSize) - 1) >> 16);
}

void MapiJpdSetMRCStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
    g_stJpdDecodeCtrl[DevId].u32READ_Start = u32ByteOffset;

#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetMRCStart_8G_0((u32ByteOffset & 0xffffUL));
        HAL_NJPD_EXT_SetMRCStart_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMRCStart_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        MhalJpdSetMRCStartLow(DevId, (u32ByteOffset & 0xffffUL));
        MhalJpdSetMRCStartHigh(DevId, (u32ByteOffset >> 16) & 0xffffUL);
    }
}

MS_U32 MapiJpdGetMWCStartAddr(MS_U8 DevId)
{
    MS_U32 u32Addr;
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        // note: ignore HAL_NJPD_EXT_GetMWCBuf_Start_8G_2() part since not used
        u32Addr = (((MS_U32)HAL_NJPD_EXT_GetMWCBuf_Start_8G_1()) << 16 | HAL_NJPD_EXT_GetMWCBuf_Start_8G_0());
    }
    else
#endif
    {
        u32Addr = (((MS_U32)MhalJpdGetMWCBufStartHigh(DevId)) << 16 | MhalJpdGetMWCBufStartLow(DevId));
    }
    return u32Addr;
}

MS_U32 MapiJpdGetWritePtrAddr(MS_U8 DevId)
{
    MS_U32 u32Addr;
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        // note: ignore HAL_NJPD_EXT_GetMWCBuf_WritePtr_8G_2() part since not used
        u32Addr = (((MS_U32)HAL_NJPD_EXT_GetMWCBuf_WritePtr_8G_1()) << 16 | HAL_NJPD_EXT_GetMWCBuf_WritePtr_8G_0());
    }
    else
#endif
    {
        u32Addr = (((MS_U32)MhalJpdGetMWCBufWritePtrHigh(DevId)) << 16 | MhalJpdGetMWCBufWritePtrLow(DevId));
    }
    return u32Addr;
}

#if SUPPORT_HIGH_LOW_REVERSE
MS_BOOL MapiJpdSetReadBuffer_Patch(MS_U8 DevId, MS_PHY u32BufAddr, MS_U32 u32BufSize, MS_PHY u32BufStartAddr)
{
    MS_BOOL             bHighLowReverse = FALSE;
    MS_U8               u8i;
    MS_U8               u8MiuSel   = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    MhalJpdSetMIU(DevId, u8MiuSel);

    u32READ_Start = u32BufStartAddr;

    MapiJpdSetMRCStartAddr(DevId, u32READ_Start);
    if (u32BufStartAddr >= u32BufAddr + u32BufSize / 2)
    {
        bHighLowReverse           = TRUE;
        pstDecCtrl->u32MRC1_Start = u32BufAddr;
        pstDecCtrl->u32MRC1_End   = ((u32BufAddr + u32BufSize / 2) - 1);
        pstDecCtrl->u32MRC0_Start = u32BufAddr + u32BufSize / 2;
        pstDecCtrl->u32MRC0_End   = ((u32BufAddr + u32BufSize) - 1);
        for (u8i = 0; u8i < 16; u8i++)
        {
            if ((((pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32READ_Start / 16) + 1) % 16 == 1)
                || (((pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32MRC1_Start / 16) + 1) % 16 == 1)
                || (((pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32MRC0_Start / 16) + 1) % 16 == 1))
            {
                pstDecCtrl->u32MRC1_End += 16;
                pstDecCtrl->u32MRC0_Start += 16;
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        pstDecCtrl->u32MRC0_Start = u32BufAddr;
        pstDecCtrl->u32MRC0_End   = ((u32BufAddr + u32BufSize / 2) - 1);
        pstDecCtrl->u32MRC1_Start = u32BufAddr + u32BufSize / 2;
        pstDecCtrl->u32MRC1_End   = ((u32BufAddr + u32BufSize) - 1);
        for (u8i = 0; u8i < 16; u8i++)
        {
            if ((((pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32READ_Start / 16) + 1) % 16 == 1)
                || (((pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32MRC0_Start / 16) + 1) % 16 == 1)
                || (((pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32MRC1_Start / 16) + 1) % 16 == 1))
            {
                pstDecCtrl->u32MRC0_End += 16;
                pstDecCtrl->u32MRC1_Start += 16;
            }
            else
            {
                break;
            }
        }
    }
    MhalJpdSetMRCBuf0StartLow(pstDecCtrl->u32MRC0_Start & 0xffffUL);
    MhalJpdSetMRCBuf0StartHigh(pstDecCtrl->u32MRC0_Start >> 16);
    MhalJpdSetMRCBuf0EndLow(pstDecCtrl->u32MRC0_End & 0xffffUL);
    MhalJpdSetMRCBuf0EndHigh(pstDecCtrl->u32MRC0_End >> 16);
    MhalJpdSetMRCBuf1StartLow(pstDecCtrl->u32MRC1_Start & 0xffffUL);
    MhalJpdSetMRCBuf1StartHigh(pstDecCtrl->u32MRC1_Start >> 16);
    MhalJpdSetMRCBuf1EndLow(pstDecCtrl->u32MRC1_End & 0xffffUL);
    MhalJpdSetMRCBuf1EndHigh(pstDecCtrl->u32MRC1_End >> 16);

    JPD_LOGD("[offset: MRC0: MRC1]=[%ld, %ld, %ld], u8i=%d\n",
             (pstDecCtrl->u32READ_Start / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16,
             (pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16,
             (pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32MRC1_Start / 16 + 1) % 16, u8i);
    return bHighLowReverse;
}
#else
MS_BOOL MapiJpdSetReadBufferNew(MS_U8 DevId, MS_PHY u32BufAddr, MS_U32 u32BufSize, MS_PHY u32BufStartAddr)
{
    MS_BOOL bDifferentHL = FALSE;
    MS_U8 u8MiuSel = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    MhalJpdSetMIU(DevId, u8MiuSel);

    pstDecCtrl->u32READ_Start = u32BufStartAddr;

    pstDecCtrl->u32MRC0_Start = u32BufAddr;
    pstDecCtrl->u32MRC0_End = ((u32BufAddr + u32BufSize / 2) - 1);
    pstDecCtrl->u32MRC1_Start = u32BufAddr + u32BufSize / 2;
    pstDecCtrl->u32MRC1_End = ((u32BufAddr + u32BufSize) - 1);
    if (pstDecCtrl->u32READ_Start > pstDecCtrl->u32MRC0_End)
    {
        pstDecCtrl->u32MRC0_End = pstDecCtrl->u32READ_Start | 0xF;
        pstDecCtrl->u32MRC1_Start = pstDecCtrl->u32MRC0_End + 1;
        bDifferentHL = TRUE;
    }

    MapiJpdSetMRCStartAddr(DevId, pstDecCtrl->u32READ_Start);

#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetMRCBuf0_Start_8G_0(pstDecCtrl->u32MRC0_Start & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf0_Start_8G_1((pstDecCtrl->u32MRC0_Start >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf0_Start_8G_2(((MS_U64)pstDecCtrl->u32MRC0_Start) >> 32);
        HAL_NJPD_EXT_SetMRCBuf0_End_8G_0(pstDecCtrl->u32MRC0_End & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf0_End_8G_1((pstDecCtrl->u32MRC0_End >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf0_End_8G_2(((MS_U64)pstDecCtrl->u32MRC0_End) >> 32);
        HAL_NJPD_EXT_SetMRCBuf1_Start_8G_0(pstDecCtrl->u32MRC1_Start & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf1_Start_8G_1((pstDecCtrl->u32MRC1_Start >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf1_Start_8G_2(((MS_U64)pstDecCtrl->u32MRC1_Start) >> 32);
        HAL_NJPD_EXT_SetMRCBuf1_End_8G_0(pstDecCtrl->u32MRC1_End & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf1_End_8G_1((pstDecCtrl->u32MRC1_End >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMRCBuf1_End_8G_2(((MS_U64)pstDecCtrl->u32MRC1_End) >> 32);
    }
    else
#endif
    {
        MS_U16 u16RAddrOffset;
        u16RAddrOffset = (u32BufAddr >> 32) & 0xf;

        MhalJpdSetMRCBuf0StartLow(DevId, pstDecCtrl->u32MRC0_Start & 0xffffUL); // 12 13
        MhalJpdSetMRCBuf0StartHigh(DevId, (pstDecCtrl->u32MRC0_Start >> 16) & 0xffffUL);
        MhalJpdSetMRCBuf0EndLow(DevId, pstDecCtrl->u32MRC0_End & 0xffffUL); // 14 15
        MhalJpdSetMRCBuf0EndHigh(DevId, (pstDecCtrl->u32MRC0_End >> 16) & 0xffffUL);
        MhalJpdSetMRCBuf1StartLow(DevId, pstDecCtrl->u32MRC1_Start & 0xffffUL); // 16 17
        MhalJpdSetMRCBuf1StartHigh(DevId, (pstDecCtrl->u32MRC1_Start >> 16) & 0xffffUL);
        MhalJpdSetMRCBuf1EndLow(DevId, pstDecCtrl->u32MRC1_End & 0xffffUL); // 18 19
        MhalJpdSetMRCBuf1EndHigh(DevId, (pstDecCtrl->u32MRC1_End >> 16) & 0xffffUL);
        u16RAddrOffset = u16RAddrOffset << 12;
        MhalJpdSetMRCBuf64BHigh(DevId, u16RAddrOffset);
    }

    JPD_LOGD("MRC0 start:%tx, MRC0 end:%tx, Offset:%tx\n", (ptrdiff_t)pstDecCtrl->u32MRC0_Start,
             (ptrdiff_t)pstDecCtrl->u32MRC0_End, (ptrdiff_t)pstDecCtrl->u32READ_Start);
    JPD_LOGD("[offset: MRC0: MRC1]=[%td, %td, %td]\n",
             (ptrdiff_t)((pstDecCtrl->u32READ_Start / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16),
             (ptrdiff_t)((pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16),
             (ptrdiff_t)((pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32MRC1_Start / 16 + 1) % 16));

    return bDifferentHL;
}
#endif
/******************************************************************************/
/// Set output frame buffer address for NJPD writing NJPEG uncompressed data
///@param u32BufAddr \b IN Start address for NJPD reading in MRC buffer
///@return none
/******************************************************************************/
void MapiJpdSetOutputFrameBuffer(MS_U8 DevId, MS_PHY u32BufAddr, MS_U16 u16LineNum)
{
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetMWCBuf_Start_8G_0(u32BufAddr & 0xffffUL);
        HAL_NJPD_EXT_SetMWCBuf_Start_8G_1((u32BufAddr >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetMWCBuf_Start_8G_2(((MS_U64)u32BufAddr) >> 32);
    }
    else
#endif
    {
        unsigned long u16WAddrOffset;
        u16WAddrOffset = (u32BufAddr >> 32) & 0xf;
        MhalJpdSetMWCBufStartLow(DevId, u32BufAddr & 0xffffUL);
        MhalJpdSetMWCBufStartHigh(DevId, u32BufAddr >> 16 & 0xffffUL);
        u16WAddrOffset = u16WAddrOffset << 12;
        MhalJpdSetMWCBuf64BHigh(DevId, u16WAddrOffset);
    }
    // UNUSED(u16LineNum);
    if (u16LineNum != 0) //&& MapiJpdGetWRingBufEnable()
    {
        MapiJpdSetWRingBufLineNum(DevId, u16LineNum);
    }
}

/******************************************************************************/
/// Setting the address of NJPD MRC, MWC buffer
///@param NJPD_BUF_CFG \b IN
///  structure {
///      MS_U32 u32ThumbnailBufAddr; \b IN Thumbnail buffer address
///      MS_U32 u32ThumbnailBufSize; \b IN Thumbnail buffer size
///      MS_U32 u32ThumbnailBufOffset; \b IN Access byte address offset in
///                                          Thumbnail buffer relative to
///                                          Thumbnail start address
///      MS_U32 u32MRCBufAddr; \b IN MRC buffer address
///      MS_U32 u32MRCBufSize; \b IN MRC buffer size
///      MS_U32 u32MRCBufOffset; \b IN Access byte address offset in MRC buffer
///                                    relative to MRC start address
///      MS_U32 u32MWCBufAddr; \b IN MWC buffer address
///      MS_BOOL bProgressive;
///      MS_BOOL bThumbnailAccessMode;
///            };
///@return none
/******************************************************************************/
#if SUPPORT_HIGH_LOW_REVERSE
MS_BOOL MapiJpdInitBuffer(MS_U8 DevId, NJPD_BUF_CFG in, MS_PHY u32MWCBufAddr, MS_BOOL bMJPEG)
{
    MS_BOOL             bHighLowReverse = FALSE;
    NJPD_DECODE_CTRL_S *pstDecCtrl      = &g_stJpdDecodeCtrl[DevId];

    if (in.bThumbnailAccessMode)
    {
        // Set MRC start access byte address
        MapiJpdSetMRCStartAddr(in.u32ThumbnailBufAddr + in.u32ThumbnailBufOffset);
        // Set MRC buffer for NJPD
        if (in.u32ThumbnailBufOffset < in.u32ThumbnailBufSize / 2)
        {
            MapiJpdSetReadBuffer0(in.u32ThumbnailBufAddr, in.u32ThumbnailBufSize / 2);
            MapiJpdSetReadBuffer1(in.u32ThumbnailBufAddr + in.u32ThumbnailBufSize / 2, in.u32ThumbnailBufSize / 2);
            bHighLowReverse = FALSE;
        }
        else
        {
            JPD_LOGD("Reverse the MRC High/Low buffer\n");
            MapiJpdSetReadBuffer1(in.u32ThumbnailBufAddr, in.u32ThumbnailBufSize / 2);
            MapiJpdSetReadBuffer0(in.u32ThumbnailBufAddr + in.u32ThumbnailBufSize / 2, in.u32ThumbnailBufSize / 2);
            bHighLowReverse = TRUE;
        }
    }
    else
    {
        if (in.bProgressive)
        {
            // Set MRC buffer for NJPD
            MapiJpdSetReadBuffer0(in.u32MRCBufAddr, in.u32MRCBufSize / 2);
            MapiJpdSetReadBuffer1(in.u32MRCBufAddr + in.u32MRCBufSize / 2, in.u32MRCBufSize / 2);
            // Set MRC start access byte address
            MapiJpdSetMRCStartAddr(in.u32MRCBufAddr);
        }
        else
        {
            JPD_LOGD("%lx, %lx, %lx", in.u32MRCBufAddr, in.u32MRCBufAddr + in.u32MRCBufSize,
                     in.u32MRCBufAddr + in.u32MRCBufOffset);

            // Set MRC start access byte address
            JPD_LOGD("in.u32MRCBufOffset=0x%lx\n", in.u32MRCBufOffset);
            MapiJpdSetMRCStartAddr(in.u32MRCBufAddr + in.u32MRCBufOffset);
            // Set MRC buffer for NJPD
            if (in.u32MRCBufOffset < in.u32MRCBufSize / 2)
            {
                MapiJpdSetReadBuffer0(in.u32MRCBufAddr, in.u32MRCBufSize / 2);
                MapiJpdSetReadBuffer1(in.u32MRCBufAddr + in.u32MRCBufSize / 2, in.u32MRCBufSize / 2);
                bHighLowReverse = FALSE;
            }
            else
            {
                JPD_LOGD("Reverse the MRC High/Low buffer\n");
                MapiJpdSetReadBuffer1(in.u32MRCBufAddr, in.u32MRCBufSize / 2);
                MapiJpdSetReadBuffer0(in.u32MRCBufAddr + in.u32MRCBufSize / 2, in.u32MRCBufSize / 2);
                bHighLowReverse = TRUE;
            }
        }
    }
    MapiJpdSetOutputFrameBuffer(in.u32MWCBufAddr, in.u16MWCBufLineNum);

    // Set the end of MWC buffer for NJPD to save the table
    if (bMJPEG == FALSE)
    {
        pstDecCtrl->u32TablePhyAddr = in.u32MWCBufAddr;
    }
    else
    {
        // u32TablePhyAddr[DevId] = in.u32ThumbnailBufAddr;
        pstDecCtrl->u32TablePhyAddr = in.u32MWCBufAddr;
    }
    JPD_LOGD("u32TablePhyAddr=0x%x, VA=0x%x\n", pstDecCtrl->u32TablePhyAddr, pstDecCtrl->u32TableVirAddr);

    return bHighLowReverse;
}
#else
MS_BOOL MapiJpdInitBuffer(MS_U8 DevId, NJPD_BUF_CFG in, MS_BOOL bMJPEG)
{
    MS_BOOL bDifferentHL = FALSE;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    memset(pstDecCtrl, 0x00, sizeof(NJPD_DECODE_CTRL_S));
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if ((MS_U64)in.u32MRCBufAddr & NJPD_OVER_4G_ADDR)
    {
        HAL_NJPD_EXT_Enable8G(TRUE);
    }
    else
    {
        HAL_NJPD_EXT_Enable8G(FALSE);
    }
#endif
    //  JPD_LOGD("\033[1;32m""%s\n""\033[0m", __FUNCTION__);

    if (in.bThumbnailAccessMode)
    {
        bDifferentHL = MapiJpdSetReadBufferNew(DevId, in.u32ThumbnailBufAddr, in.u32ThumbnailBufSize,
                                               in.u32ThumbnailBufAddr + in.u32ThumbnailBufOffset);
    }
    else
    {
        if (in.bProgressive)
        {
            // bDifferentHL = MapiJpdSetReadBufferNew(in.u32MRCBufAddr, in.u32MRCBufSize, in.u32MRCBufAddr +
            // in.u32MRCBufOffset);
            bDifferentHL = MapiJpdSetReadBufferNew(DevId, in.u32MRCBufAddr, in.u32MRCBufSize, in.u32MRCBufAddr);
        }
        else
        {
            bDifferentHL = MapiJpdSetReadBufferNew(DevId, in.u32MRCBufAddr, in.u32MRCBufSize,
                                                   in.u32MRCBufAddr + in.u32MRCBufOffset);
        }
    }
    MapiJpdSetOutputFrameBuffer(DevId, in.u32MWCBufAddr, in.u16MWCBufLineNum);

    // Set the end of MWC buffer for NJPD to save the table
    if (bMJPEG == FALSE)
    {
        pstDecCtrl->u32TablePhyAddr = in.u32TableAddr;
        pstDecCtrl->u32TableVirAddr = in.u32VirTableAddr;
    }
    else
    {
        pstDecCtrl->u32TablePhyAddr = in.u32TableAddr;
        pstDecCtrl->u32TableVirAddr = in.u32VirTableAddr;
    }

    JPD_LOGD("u32TablePhyAddr=0x%llx, VA=0x%llx\n", pstDecCtrl->u32TablePhyAddr, pstDecCtrl->u32TableVirAddr);

    return bDifferentHL;
}

#endif

/******************************************************************************/
/// Set width and height of picture
///@param u16Width \b IN picture width
///@param u16Height \b IN picture height
///@return none
/******************************************************************************/
void MapiJpdSetPicDimension(MS_U8 DevId, MS_U16 u16PicWidth, MS_U16 u16PicHeight)
{
    MhalJpdSetPicHorizontal(DevId, (u16PicWidth >> 3));
    MhalSetPicVertical(DevId, (u16PicHeight >> 3));
}

/******************************************************************************/
/// Get NJPD Event Flag
///@return EventFlag
/******************************************************************************/
MS_U16 MapiJpdGetEventFlag(MS_U8 DevId)
{
    return MhalJpdGetEventFlag(DevId);
}

/******************************************************************************/
/// Set NJPD Event Flag
///@param u16Value \n IN EventFlag value
///@return none
/******************************************************************************/
void MapiJpdSetEventFlag(MS_U8 DevId, MS_U16 u16Value)
{
    // clear by write
    MhalJpdClearEventFlag(DevId, u16Value);
}

/******************************************************************************/
/// Set ROI region
///@param start_x \n IN start X position
///@param start_y \n IN start Y position
///@param width \n IN width
///@param height \n IN height
///@return none
/******************************************************************************/
void MapiJpdSetROI(MS_U8 DevId, MS_U16 start_x, MS_U16 start_y, MS_U16 width, MS_U16 height)
{
    //    MS_U16 reg_val;

    //    reg_val = HAL_NJPD_Get_S_Config();

    MhalJpdSetROIHorizontal(DevId, start_x);
    MhaljpdSetROIVertical(DevId, start_y);
    MhalJpdSetROIWidth(DevId, width);
    MhalJpdSetROIHeight(DevId, height);
}

void MapiJpdPZ1SetFlag(MS_U16 u16Value)
{
    MhalJpdPZ1SetFlag(u16Value);
}

#ifdef NJPD_CMDQ_SUPPORT
void MapiJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf)
{
    MhalJpdSetCfgCmdq(u8DevId, pCmdqHandle, pu64CmdqBuf);
}

void MapiJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen)
{
    MhalJpdGetCfgCmdq(u8DevId, ppCmdqHandle, ppu64CmdqBuf, pu32CmdLen);
}
#endif

void MapiJpdInitRegBase(int u32DevId, MS_U64 u16Value)
{
    MhalJpdInitRegBase(u32DevId, u16Value);
}

void MapiJpdInitClk(int u32DevId, MS_U64 u16Value)
{
    MhalJpdInitClk(u32DevId, u16Value);
}

/********************************************************************/
/// Set RSTIntv
///@param u16Value \n IN register RSTIntv
///@return none
/********************************************************************/
void MapiJpdSetRstIntv(MS_U8 DevId, MS_U16 u16Value)
{
    MhalJpdSetRSTIntv(DevId, u16Value);
}

/********************************************************************/
/// Get current vertical line index written to memory
///@return current vertical line index written to memory
/********************************************************************/
MS_U16 MapiJpdGetCurVidx(void)
{
    JPD_LOGE("Error!!! Do not support %s() in NJPD!!!!!!!!!!!!!!!!\n", __FUNCTION__);
    return 0;
}

/********************************************************************/
/// Write Grpinf of Table IndirectAccess
///@param in \b IN structure for Grpinf of Table IndirectAccess
///@return none
/********************************************************************/
void MapiJpdWriteGrpinf(MS_U8 DevId, NJPD_GRPINF in)
{
    MS_U16              i;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];
    invalidate_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE),
                            (unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 96 * 4));
    if (in.DHT)
    {
        pstDecCtrl->u8DcLumaCnt    = in.u8DcLumaCnt;
        pstDecCtrl->u8DcChromaCnt  = in.u8DcChromaCnt;
        pstDecCtrl->u8DcChroma2Cnt = in.u8DcChroma2Cnt;
        pstDecCtrl->u8AcLumaCnt    = in.u8AcLumaCnt;
        pstDecCtrl->u8AcChromaCnt  = in.u8AcChromaCnt;
        pstDecCtrl->u8AcChroma2Cnt = in.u8AcChroma2Cnt;
//#if ENABLE_NJPD_DEBUG_MSG
#if 0

        for(i= 1; i<=16; i++)
        {
            if(i== 1 || i== 8|| i== 16)
                JPD_LOGD("dc luma i=%d, [%02x %04x %02x]\n", i, in.u8DcLumaValid[i], in.u16DcLumaCode[i], in.u8DcLumaSymbol[i]);
        }
        for(i= 1; i<=16; i++)
        {
            if(i== 1 || i== 8|| i== 16)
                JPD_LOGD("ac luma i=%d, [%02x %04x %02x]\n", i, in.u8AcLumaValid[i], in.u16AcLumaCode[i], in.u8AcLumaSymbol[i]);
        }
        for(i= 1; i<=16; i++)
        {
            if(i== 1 || i== 8|| i== 16)
            JPD_LOGD("dc chroma i=%d, [%02x %04x %02x]\n", i, in.u8DcChromaValid[i], in.u16DcChromaCode[i], in.u8DcChromaSymbol[i]);
        }
        for(i= 1; i<=16; i++)
        {
            if(i== 1 || i== 8|| i== 16)
                JPD_LOGD("ac chroma i=%d, [%02x %04x %02x]\n",i , in.u8AcChromaValid[i], in.u16AcChromaCode[i], in.u8AcChromaSymbol[i]);
        }
        if(bIs3HuffTbl[DevId])
        {
            for(i= 1; i<=16; i++)
            {
               // JPD_LOGD("dc chroma2 i=%d, [%02x %04x %02x]\n", i, in.u8DcChroma2Valid[i], in.u16DcChroma2Code[i], in.u8DcChroma2Symbol[i]);
            }
            for(i= 1; i<=16; i++)
            {
              //  JPD_LOGD("ac chroma2 i=%d, [%02x %04x %02x]\n",i, in.u8AcChroma2Valid[i], in.u16AcChroma2Code[i], in.u8AcChroma2Symbol[i]);
            }
        }
#else
        if (MapiJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
        {
            for (i = 1; i <= 16; i++)
            {
                JPD_LOGD("dc luma i=%d, [%02x %04x %02x]\n", i, in.u8DcLumaValid[i], in.u16DcLumaCode[i],
                         in.u8DcLumaSymbol[i]);
            }
            for (i = 1; i <= 16; i++)
            {
                JPD_LOGD("ac luma i=%d, [%02x %04x %02x]\n", i, in.u8AcLumaValid[i], in.u16AcLumaCode[i],
                         in.u8AcLumaSymbol[i]);
            }
            for (i = 1; i <= 16; i++)
            {
                JPD_LOGD("dc chroma i=%d, [%02x %04x %02x]\n", i, in.u8DcChromaValid[i], in.u16DcChromaCode[i],
                         in.u8DcChromaSymbol[i]);
            }
            for (i = 1; i <= 16; i++)
            {
                JPD_LOGD("ac chroma i=%d, [%02x %04x %02x]\n", i, in.u8AcChromaValid[i], in.u16AcChromaCode[i],
                         in.u8AcChromaSymbol[i]);
            }
            if (pstDecCtrl->bIs3HuffTbl)
            {
                for (i = 1; i <= 16; i++)
                {
                    JPD_LOGD("dc chroma2 i=%d, [%02x %04x %02x]\n", i, in.u8DcChroma2Valid[i], in.u16DcChroma2Code[i],
                             in.u8DcChroma2Symbol[i]);
                }
                for (i = 1; i <= 16; i++)
                {
                    JPD_LOGD("ac chroma2 i=%d, [%02x %04x %02x]\n", i, in.u8AcChroma2Valid[i], in.u16AcChroma2Code[i],
                             in.u8AcChroma2Symbol[i]);
                }
            }
        }
#endif
        // DC
        for (i = 0; i < 16; i++)
        {
            if (in.u8DcLumaValid[i + 1])
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 0)) =
                    in.u8DcLumaSymbol[i + 1];
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 1)) =
                    in.u16DcLumaCode[i + 1] & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 2)) =
                    (in.u16DcLumaCode[i + 1] >> 8) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 3)) =
                    in.u8DcLumaValid[i + 1];
            }
            else
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 0)) =
                    in.u8DcLumaSymbol[i + 1];
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + i * 4 + 3)) =
                    in.u8DcLumaValid[i + 1];
            }
        }

        // AC
        for (i = 0; i < 16; i++)
        {
            if (in.u8AcLumaValid[i + 1])
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 0)) =
                    (in.u8AcLumaSymbol[i + 1] + in.u8DcLumaCnt);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 1)) =
                    (in.u16AcLumaCode[i + 1] & 0xff);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 2)) =
                    ((in.u16AcLumaCode[i + 1] >> 8) & 0xff);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 3)) =
                    in.u8AcLumaValid[i + 1];
            }
            else
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 0)) =
                    (in.u8AcLumaSymbol[i + 1] + in.u8DcLumaCnt);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 16 * 4 + i * 4 + 3)) =
                    in.u8AcLumaValid[i + 1];
            }
        }

        for (i = 0; i < 16; i++)
        {
            if (in.u8DcChromaValid[i + 1])
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 0)) =
                    in.u8DcChromaSymbol[i + 1];
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 1)) =
                    in.u16DcChromaCode[i + 1] & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 2)) =
                    (in.u16DcChromaCode[i + 1] >> 8) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 3)) =
                    in.u8DcChromaValid[i + 1];
            }
            else
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 0)) =
                    in.u8DcChromaSymbol[i + 1];
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 32 * 4 + i * 4 + 3)) =
                    in.u8DcChromaValid[i + 1];
            }
        }

        for (i = 0; i < 16; i++)
        {
            if (in.u8AcChromaValid[i + 1])
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 0)) =
                    (in.u8AcChromaSymbol[i + 1] + in.u8DcChromaCnt);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 1)) =
                    (in.u16AcChromaCode[i + 1] & 0xff);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 2)) =
                    ((in.u16AcChromaCode[i + 1] >> 8) & 0xff);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 3)) =
                    in.u8AcChromaValid[i + 1];
            }
            else
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 0)) =
                    (in.u8AcChromaSymbol[i + 1] + in.u8DcChromaCnt);
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 48 * 4 + i * 4 + 3)) =
                    in.u8AcChromaValid[i + 1];
            }
        }

        if (pstDecCtrl->bIs3HuffTbl)
        {
            for (i = 0; i < 16; i++)
            {
                if (in.u8DcChroma2Valid[i + 1])
                {
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 0)) = in.u8DcChroma2Symbol[i + 1];
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 1)) = in.u16DcChroma2Code[i + 1] & 0xff;
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 2)) = (in.u16DcChroma2Code[i + 1] >> 8) & 0xff;
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 3)) = in.u8DcChroma2Valid[i + 1];
                }
                else
                {
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 0)) = in.u8DcChroma2Symbol[i + 1];
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 64 * 4 + i * 4
                                               + 3)) = in.u8DcChroma2Valid[i + 1];
                }
            }

            for (i = 0; i < 16; i++)
            {
                if (in.u8AcChroma2Valid[i + 1])
                {
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 0)) = (in.u8AcChroma2Symbol[i + 1] + in.u8DcChroma2Cnt);
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 1)) = in.u16AcChroma2Code[i + 1] & 0xff;
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 2)) = (in.u16AcChroma2Code[i + 1] >> 8) & 0xff;
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 3)) = in.u8AcChroma2Valid[i + 1];
                }
                else
                {
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 0)) = (in.u8AcChroma2Symbol[i + 1] + in.u8DcChroma2Cnt);
                    *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 80 * 4 + i * 4
                                               + 3)) = in.u8AcChroma2Valid[i + 1];
                }
            }
        }
    }

    flush_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE),
                       (unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE + 96 * 4));

    MapiJpdGTableReloadEnable(DevId, ENABLE);
#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Group table!!!!!!!!!!!!!!!!\n");
    if (pstDecCtrl->bIs3HuffTbl)
    {
        MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE), 96 * 4);
    }
    else
    {
        MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE), 64 * 4);
    }
#else
    if (MapiJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        JPD_LOGD("print the Group table!!!!!!!!!!!!!!!!\n");
        if (pstDecCtrl->bIs3HuffTbl)
        {
            MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE), 96 * 4);
        }
        else
        {
            MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SCWGIF_BASE), 64 * 4);
        }
    }
#endif
    MapiJpdSetGTableStartAddr(DevId, pstDecCtrl->u32TablePhyAddr + NJPD_MEM_SCWGIF_BASE);
}
/********************************************************************/
/// Write Symidx of Table Indirect Access
///@param in \b IN structure for Symidx of Table Indirect Access
///@return none
/********************************************************************/
void ApiJpdWriteSymidx(MS_U8 DevId, NJPD_SYMIDX in)
{
    MS_U16              i             = 0;
    MS_U16              u16LumaCnt    = 0;
    MS_U16              u16ChromaCnt  = 0;
    MS_U16              u16Chroma2Cnt = 0;
    MS_U16              u16MaxCnt     = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl    = &g_stJpdDecodeCtrl[DevId];

    invalidate_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE),
                            (unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16MaxCnt * 4));
    if (in.DHT)
    {
        //        for ( i = 0; i < 16; i++ )
        for (i = 0; i < 12; i++)
        {
            if (i < pstDecCtrl->u8DcLumaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16LumaCnt * 4 + 0)) =
                    in.u8DcLumaHuffVal[i];
                u16LumaCnt++;
            }
            if (i < pstDecCtrl->u8DcChromaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16ChromaCnt * 4 + 1)) =
                    in.u8DcChromaHuffVal[i];
                u16ChromaCnt++;
            }
            if (pstDecCtrl->bIs3HuffTbl && i < pstDecCtrl->u8DcChroma2Cnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16Chroma2Cnt * 4
                                           + 2)) = in.u8DcChroma2HuffVal[i];
                u16Chroma2Cnt++;
            }
            else if (i < pstDecCtrl->u8DcChromaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16Chroma2Cnt * 4
                                           + 2)) = in.u8DcChromaHuffVal[i];
                u16Chroma2Cnt++;
            }
        }

        //        for ( i = 0; i < 240; i++ )
        for (i = 0; i < 162; i++)
        {
            if (i < pstDecCtrl->u8AcLumaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16LumaCnt * 4 + 0)) =
                    in.u8AcLumaHuffVal[i];
                u16LumaCnt++;
            }
            if (i < pstDecCtrl->u8AcChromaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16ChromaCnt * 4 + 1)) =
                    in.u8AcChromaHuffVal[i];
                u16ChromaCnt++;
            }
            if (pstDecCtrl->bIs3HuffTbl && i < pstDecCtrl->u8AcChroma2Cnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16Chroma2Cnt * 4
                                           + 2)) = in.u8AcChroma2HuffVal[i];
                u16Chroma2Cnt++;
            }
            else if (i < pstDecCtrl->u8AcChromaCnt)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16Chroma2Cnt * 4
                                           + 2)) = in.u8AcChromaHuffVal[i];
                u16Chroma2Cnt++;
            }
        }
    }

    u16MaxCnt = (u16LumaCnt > u16ChromaCnt) ? u16LumaCnt : u16ChromaCnt;
    u16MaxCnt = (u16MaxCnt > u16Chroma2Cnt) ? u16MaxCnt : u16Chroma2Cnt;
    u16MaxCnt = ((u16MaxCnt % 4) == 2 || (u16MaxCnt % 4) == 3) ? u16MaxCnt : ((u16MaxCnt / 4) * 4 + 2);
    flush_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE),
                       (unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE + u16MaxCnt * 4));

    MapiJpdHTableReloadEnable(DevId, ENABLE);

#if 0
    if(u16MaxCnt > u16LumaCnt)
    {
        for(i=u16LumaCnt; i<u16MaxCnt; i++)
            JPEG_memset((void*)((u32TablePhyAddr+NJPD_MEM_SYMIDX_BASE+i*4+0)), 0, 1);
    }
    if(u16MaxCnt > u16ChromaCnt)
    {
        for(i=u16ChromaCnt; i<u16MaxCnt; i++)
            JPEG_memset((void*)((u32TablePhyAddr+NJPD_MEM_SYMIDX_BASE+i*4+1)), 0, 1);
    }
    if(u16MaxCnt > u16Chroma2Cnt)
    {
        for(i=u16Chroma2Cnt; i<u16MaxCnt; i++)
        {
            JPEG_memset((void*)((u32TablePhyAddr+NJPD_MEM_SYMIDX_BASE+i*4+2)), 0, 1);
            JPEG_memset((void*)((u32TablePhyAddr+NJPD_MEM_SYMIDX_BASE+i*4+3)), 0, 1);
        }
    }
#endif

#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Huffman table!!!!!!!!!!!!!!!!\n");
    MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE), u16MaxCnt * 4);
#else
    if (MapiJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        JPD_LOGD("print the Huffman table!!!!!!!!!!!!!!!!\n");
        MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_SYMIDX_BASE), u16MaxCnt * 4);
    }
#endif

    MapiJpdSetHTableStartAddr(DevId, pstDecCtrl->u32TablePhyAddr + NJPD_MEM_SYMIDX_BASE);
    MhalJpdSetHTableSize(DevId, u16MaxCnt);
}
/********************************************************************/
/// Write IQtbl of Table Indirect Access
///@param in \b IN structure for IQtbl of Table Indirect Access
///@return none
/********************************************************************/
void MapiJpdWriteIQTbl(MS_U8 DevId, NJPD_IQ_TBL in)
{
    MS_U8               i          = 0;
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    invalidate_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE),
                            (unsigned long)(pstDecCtrl->u32TableVirAddr + 64 * 4));
    if (in.DQT)
    {
        for (i = 0; i < 64; i++)
        {
            if (in.u8CompNum == 1)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 0)) =
                    in.QuantTables[in.u8CompQuant[0]].s16Value[g_u8JpdZigzagOrder[i]] & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 1)) =
                    in.QuantTables[in.u8CompQuant[0]].s16Value[g_u8JpdZigzagOrder[i]] & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 2)) =
                    in.QuantTables[in.u8CompQuant[0]].s16Value[g_u8JpdZigzagOrder[i]] & 0xff;
            }
            else if (in.u8CompNum == 2)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 0)) =
                    (in.QuantTables[in.u8CompQuant[0]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 1)) =
                    (in.QuantTables[in.u8CompQuant[1]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 2)) =
                    (in.QuantTables[in.u8CompQuant[1]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
            }
            else if (in.u8CompNum == 3)
            {
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 0)) =
                    (in.QuantTables[in.u8CompQuant[0]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 1)) =
                    (in.QuantTables[in.u8CompQuant[1]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
                *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 2)) =
                    (in.QuantTables[in.u8CompQuant[2]].s16Value[g_u8JpdZigzagOrder[i]]) & 0xff;
            }
        }
    }
    else
    {
        MS_U8 u8Value;
        for (i = 0; i < 64; i++)
        {
            u8Value = g_u16IQTable[i] & 0xff;
            *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + i * 4 + 0)) = u8Value;
        }
        for (i = 64; i < 128; i++)
        {
            u8Value = g_u16IQTable[i] & 0xff;
            *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + (i - 64) * 4 + 1)) = u8Value;
            *(MS_U8 *)(unsigned long)((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE + (i - 64) * 4 + 2)) = u8Value;
        }
    }

    if (in.u8CompNum == 3)
    {
        MapiJpdSetDifferentQTable(DevId, ENABLE);
    }

    flush_dcache_range((unsigned long)(pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE),
                       (unsigned long)(pstDecCtrl->u32TableVirAddr + 64 * 4));

    MapiJpdQTableReloadEnable(DevId, ENABLE);
#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Quantization table!!!!!!!!!!!!!!!!\n");
    MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE), 64 * 4);
#else
    if (MapiJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        JPD_LOGD("print the Quantization table!!!!!!!!!!!!!!!!\n");
        MapiJpdPrintMemory((pstDecCtrl->u32TableVirAddr + NJPD_MEM_QTBL_BASE), 64 * 4);
    }
#endif
    MapiJpdSetQTableStartAddr(DevId, pstDecCtrl->u32TablePhyAddr + NJPD_MEM_QTBL_BASE);
}

void MapiJpdTableRead(MS_U8 DevId)
{
    MS_U16 u16Value = 0;
    MS_U8  u8Table;
    MS_U16 u16Addr;
    MS_U8  u8TableCnt;
    MS_U16 u16RIUValue = MhalJpdGetRIUInterface(DevId);

    MhalJpdSetGlobalSetting02(DevId, MhalJpdGetGlobalSetting02(DevId) & ~NJPD_TBC_MODE);

    JPD_LOGD("\n%s(), read h table start================\n", __FUNCTION__);
    u8Table    = 0; // 2'b00: h table, 2'b01: g table, 2'b10: q table
    u8TableCnt = MhalJpdGetHTableSize(DevId);
    for (u16Addr = 0; u16Addr < u8TableCnt; u16Addr++)
    {
        u16Value = NJPD_JPD_TBC_TABLE_READ | ((u8Table & 0x3) << 1 & NJPD_JPD_TBC_SEL) | u16Addr << 8;
        MhalJpdSetRIUInterface(DevId, u16Value);
        MhalJpdGetWriteOneClearReg2(DevId, NJPD_TBC_EN);
        while (1)
        {
            if (MhalJpdGetRIUInterface(DevId) & NJPD_JPD_TBC_TABLE_READ)
            {
                JPD_LOGD("%02x %02x %02x %02x ", MhalJpdTBCReadDataLow(DevId) & 0xff,
                         (MhalJpdTBCReadDataLow(DevId) & 0xff00) >> 8, MhalJpdTBCReadDataHigh(DevId) & 0xff,
                         (MhalJpdTBCReadDataHigh(DevId) & 0xff00) >> 8);
                if (u16Addr % 2 == 1)
                {
                    JPD_LOGD("\n");
                }
                break;
            }
        }
    }
    JPD_LOGD("%s(), read h table end  ================\n", __FUNCTION__);

    JPD_LOGD("\n%s(), read g table start================\n", __FUNCTION__);
    u8Table = 1; // 2'b00: h table, 2'b01: g table, 2'b10: q table
    if (g_stJpdDecodeCtrl[DevId].bIs3HuffTbl)
    {
        u8TableCnt = 96;
    }
    else
    {
        u8TableCnt = 64;
    }

    for (u16Addr = 0; u16Addr < u8TableCnt; u16Addr++)
    {
        u16Value = NJPD_JPD_TBC_TABLE_READ | ((u8Table & 0x3) << 1 & NJPD_JPD_TBC_SEL) | u16Addr << 8;
        MhalJpdSetRIUInterface(DevId, u16Value);
        MhalJpdGetWriteOneClearReg2(DevId, NJPD_TBC_EN);
        while (1)
        {
            if (MhalJpdGetRIUInterface(DevId) & NJPD_JPD_TBC_TABLE_READ)
            {
                JPD_LOGD("%02x %02x %02x %02x ", MhalJpdTBCReadDataLow(DevId) & 0xff,
                         (MhalJpdTBCReadDataLow(DevId) & 0xff00) >> 8, MhalJpdTBCReadDataHigh(DevId) & 0xff,
                         (MhalJpdTBCReadDataHigh(DevId) & 0xff00) >> 8);
                if (u16Addr % 2 == 1)
                {
                    JPD_LOGD("\n");
                }
                break;
            }
        }
    }
    JPD_LOGD("%s(), read g table end  ================\n", __FUNCTION__);

    JPD_LOGD("\n%s(), read q table start================\n", __FUNCTION__);
    u8Table    = 2; // 2'b00: h table, 2'b01: g table, 2'b10: q table
    u8TableCnt = 64;
    for (u16Addr = 0; u16Addr < u8TableCnt; u16Addr++)
    {
        u16Value = NJPD_JPD_TBC_TABLE_READ | ((u8Table & 0x3) << 1 & NJPD_JPD_TBC_SEL) | u16Addr << 8;
        MhalJpdSetRIUInterface(DevId, u16Value);
        MhalJpdGetWriteOneClearReg2(DevId, NJPD_TBC_EN);
        while (1)
        {
            if (MhalJpdGetRIUInterface(DevId) & NJPD_JPD_TBC_TABLE_READ)
            {
                JPD_LOGD("%02x %02x %02x %02x ", MhalJpdTBCReadDataLow(DevId) & 0xff,
                         (MhalJpdTBCReadDataLow(DevId) & 0xff00) >> 8, MhalJpdTBCReadDataHigh(DevId) & 0xff,
                         (MhalJpdTBCReadDataHigh(DevId) & 0xff00) >> 8);
                if (u16Addr % 2 == 1)
                {
                    JPD_LOGD("\n");
                }
                break;
            }
        }
    }
    JPD_LOGD("%s(), read q table end  ================\n", __FUNCTION__);
    MhalJpdSetGlobalSetting02(DevId, MhalJpdGetGlobalSetting02(DevId) | NJPD_TBC_MODE);
    MhalJpdSetRIUInterface(DevId, u16RIUValue);
}

/*
void MDrv_NJPD_SetVerificationMode(NJPD_VERIFICATION_MODE VerificationMode)
{
    HAL_NJPD_SetVerificationMode(VerificationMode);
}
*/

void MapiJpdDebug(MS_U8 DevId)
{
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

#if (ENABLE_TEST_NJPD_01_table_read_write_test == TRUE)
    MapiJpdTableRead(DevId);
#else
    if (MapiJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        MapiJpdTableRead(DevId);
    }
#endif
    MhalJpdDebug(DevId);

    JPD_LOGD("[offset: MRC0: MRC1]=[%td, %td, %td]\n",
             (ptrdiff_t)((pstDecCtrl->u32READ_Start / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16),
             (ptrdiff_t)((pstDecCtrl->u32MRC0_End / 16 - pstDecCtrl->u32MRC0_Start / 16 + 1) % 16),
             (ptrdiff_t)((pstDecCtrl->u32MRC1_End / 16 - pstDecCtrl->u32MRC1_Start / 16 + 1) % 16));

    // MapiJpdPrintMemory((u32TableVirAddr), 2048);
}

/********************************************************************/
/// Get NJPD driver status
///@param none
///@return the pointer of NjpdStatus
/********************************************************************/
NJPD_STATUS *MapiJpdGetStatus(MS_U8 DevId)
{
    g_stNjpdStatus.u32CurMRCAddr = MhalJpdGetCurMRCAddr(DevId);
    g_stNjpdStatus.u16CurVidx    = 0;
    g_stNjpdStatus.u16CurRow     = MhalJpdGetCurRow(DevId);
    g_stNjpdStatus.u16CurCol     = MhalJpdGetCurCol(DevId);
    return &g_stNjpdStatus;
}

/********************************************************************/
/// Get current vertical line index written to memory
///@return current vertical line index written to memory
/********************************************************************/
MS_U32 MapiJpdGetCurMRCAddr(MS_U8 DevId)
{
    return MhalJpdGetCurMRCAddr(DevId);
}

/********************************************************************/
/// Get current vertical line index written to memory
///@return current vertical line index written to memory
/********************************************************************/
MS_U16 MapiJpdGetCurRow(MS_U8 DevId)
{
    return MhalJpdGetCurRow(DevId);
}

/********************************************************************/
/// Get current vertical line index written to memory
///@return current vertical line index written to memory
/********************************************************************/
MS_U16 MapiJpdGetCurCol(MS_U8 DevId)
{
    return MhalJpdGetCurCol(DevId);
}
/********************************************************************/
/// Set write protect flag
///@return none
/********************************************************************/
void MapiJpdSetAutoProtect(MS_U8 DevId, MS_BOOL enable)
{
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_8G_WRITE_BOUND_ENABLE(enable);
    }
    else
#endif
    {
        MhalJpdSetAutoProtect(DevId, enable);
    }
}

/******************************************************************************/
/// Set WPEN end address for NJPD
///@param u32ByteOffset \b IN End address for NJPD writing to MWC buffer
///@return none
/******************************************************************************/
void MapiJpdSetWPENEndAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
    JPD_LOGD("MapiJpdSetWPENEndAddr=0x%tx\n", (ptrdiff_t)u32ByteOffset);
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetWPENUBound_8G_0(u32ByteOffset & 0xffffUL);
        HAL_NJPD_EXT_SetWPENUBound_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetWPENUBound_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        MhalJpdSetWriteProtectEnableUnbound0Low(DevId, ((u32ByteOffset & 0xffffffffULL) >> 3) & 0xffffUL);
        MhalJpdSetWriteProtectEnableUnbound0High(DevId, ((u32ByteOffset & 0xffffffffULL) >> 3) >> 16);
    }
}

void MapiJpdSetWPENStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
    JPD_LOGD("MapiJpdSetWPENStartAddr=0x%tx\n", (ptrdiff_t)u32ByteOffset);
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetWPENLBound_8G_0(u32ByteOffset & 0xffffUL);
        HAL_NJPD_EXT_SetWPENLBound_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetWPENLBound_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        MhalJpdSetWriteProtectEnableBound0Low(DevId, ((u32ByteOffset & 0xffffffffULL) >> 3) & 0xffffUL);
        MhalJpdSetWriteProtectEnableBound0HIgh(DevId, ((u32ByteOffset & 0xffffffffULL) >> 3) >> 16);
    }
}

/********************************************************************/
/// Set Spare Register
///@param u16Value \n IN register SCONFIG
///@return none
/********************************************************************/
void MapiJpdSetSpare(MS_U8 DevId, MS_U16 u16Value)
{
    MhalJpdSetSpare00(DevId, u16Value);
}

/********************************************************************/
/// Get Spare Register
///@return Spare Register
/********************************************************************/
MS_U16 MapiJpdGetSpare(MS_U8 DevId)
{
    return MhalJpdGetSpare00(DevId);
}

void MapiJpdSetMRCValid(MS_U8 DevId, MS_U16 u16Value)
{
    // JPD_LOGD("%s() with u16Value=0x%x\n", __FUNCTION__, u16Value);
#if ENABLE_FF00FFD_TO_FFFFFFD
    MS_U32              u32Time    = MDrv_TIMER_GetUs(E_TIMER_1);
    NJPD_DECODE_CTRL_S *pstDecCtrl = &g_stJpdDecodeCtrl[DevId];

    if (u16Value & NJPD_MRC0_VALID)
    {
        MS_U32 u32addr;
        for (u32addr = (pstDecCtrl->u32MRC0_Start); u32addr < (pstDecCtrl->u32MRC0_End); u32addr++)
        {
            if (((*(MS_U8 *)u32addr) == 0xff) && ((*(MS_U8 *)(u32addr + 1)) == 0x00)
                && ((*(MS_U8 *)(u32addr + 2)) == 0xff) && (((*(MS_U8 *)(u32addr + 3)) >> 4) == 0xd))
            {
                (*(MS_U8 *)(u32addr + 1)) = 0xff;
            }
        }
    }
    if (u16Value & NJPD_MRC1_VALID)
    {
        MS_U32 u32addr;
        for (u32addr = (pstDecCtrl->u32MRC1_Start); u32addr < (pstDecCtrl->u32MRC1_End); u32addr++)
        {
            if (((*(MS_U8 *)u32addr) == 0xff) && ((*(MS_U8 *)(u32addr + 1)) == 0x00)
                && ((*(MS_U8 *)(u32addr + 2)) == 0xff) && (((*(MS_U8 *)(u32addr + 3)) >> 4) == 0xd))
            {
                (*(MS_U8 *)(u32addr + 1)) = 0xff;
            }
        }
    }

    if (u16Value & NJPD_MRC0_VALID && u16Value & NJPD_MRC1_VALID)
        JPD_LOGD("[B]waste %ldus, size=%ld\n", MDrv_TIMER_GetUs(E_TIMER_1) - u32Time,
                 u32MRC1_End[DevId] - u32MRC0_Start[DevId]);
    else if (u16Value & NJPD_MRC0_VALID)
        JPD_LOGD("[0]waste %ldus, size=%ld\n", MDrv_TIMER_GetUs(E_TIMER_1) - u32Time,
                 u32MRC0_End[DevId] - u32MRC0_Start[DevId]);
    else if (u16Value & NJPD_MRC1_VALID)
        JPD_LOGD("[1]waste %ldus, size=%ld\n", MDrv_TIMER_GetUs(E_TIMER_1) - u32Time,
                 u32MRC1_End[DevId] - u32MRC1_Start[DevId]);
#endif

    MhalJpdSetWriteOneClearReg(DevId, u16Value);
}

void MapiJpdDecodeEnable(MS_U8 DevId)
{
#if 0
    MapiJpdTableRead();
#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Group table!!!!!!!!!!!!!!!!\n");
    if(bIs3HuffTbl[DevId])
    {
        MapiJpdPrintMemory((u32TablePhyAdd[DevId]r+NJPD_MEM_SCWGIF_BASE), 96*4);
    }
    else
    {
        MapiJpdPrintMemory((u32TablePhyAddr[DevId]+NJPD_MEM_SCWGIF_BASE), 64*4);
    }
#endif

#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Huffman table!!!!!!!!!!!!!!!!\n");
    MapiJpdPrintMemory((u32TablePhyAddr[DevId]+NJPD_MEM_SYMIDX_BASE), 0x2b8);
#endif

#if ENABLE_NJPD_DEBUG_MSG
    JPD_LOGD("print the Quantization table!!!!!!!!!!!!!!!!\n");
    MapiJpdPrintMemory((u32TablePhyAddr[DevId]+NJPD_MEM_QTBL_BASE), 64*4);
#endif
#endif

    MhalJpdSetWriteOneClearReg(DevId, NJPD_DECODE_ENABLE);
    // JPD_LOGD("%s().....\n", __FUNCTION__);
}

void MapiJpdTableLoadingStart(MS_U8 DevId)
{
    JPD_LOGD("%s().....\n", __FUNCTION__);
    MhalJpdSetWriteOneClearReg(DevId, NJPD_TABLE_LOADING_START);
}

void MapiJpdReadLastBuffer(MS_U8 DevId)
{
    JPD_LOGD("%s().....\n", __FUNCTION__);
    MhalJpdSetWriteOneClearReg(DevId, NJPD_MRC_LAST);
}

void MapiJpdSetScalingDownFactor(MS_U8 DevId, EN_NJPD_SCALING_DOWN_FACTOR eScalingFactor)
{
    MS_U16 u16Value;
    JPD_LOGD("[notice]MapiJpdSetScalingDownFactor\n");
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_DOWN_SCALE) | ((MS_U16)eScalingFactor << 0));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdGTableRst(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    if (bEnable)
    {
        u16Value = (HalJpdGetGlobalSetting01(DevId) & ~NJPD_GTABLE_RST);
        MhalJpdSetGlobalSetting01(DevId, u16Value);
        u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_GTABLE_RST) | ((MS_U16)bEnable << 10));
        MhalJpdSetGlobalSetting01(DevId, u16Value);
    }
}

void MapiJpdHTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_HTABLE_RELOAD_EN) | ((MS_U16)bEnable << 11));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdGTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_GTABLE_RELOAD_EN) | ((MS_U16)bEnable << 12));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdQTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_QTABLE_RELOAD_EN) | ((MS_U16)bEnable << 13));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdWriteIMIEnable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_W2_IMI_EN) | ((MS_U16)bEnable << 15));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdWbufPackEnable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_WBUF_PACK) | ((MS_U16)bEnable << 14));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
}

void MapiJpdSetSoftwareVLD(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((MhalJpdGetGlobalSetting02(DevId) & ~(NJPD_REMOVE_0xFF00 | NJPD_REMOVE_0xFFFF | NJPD_FFD9_EN))
                | NJPD_BITSTREAM_LE);
    MhalJpdSetGlobalSetting02(DevId, u16Value);
    u16Value = ((HalJpdGetGlobalSetting01(DevId) & ~NJPD_SVLD) | ((MS_U16)bEnable << 6));
    MhalJpdSetGlobalSetting01(DevId, u16Value);
    if (bEnable)
    {
        // 20120619, clear last bit when SVJD or NJPD_REMOVE_0xFFFF ==0
        MhalJpdSetSpare00(DevId, MhalJpdGetSpare00(DevId) & 0xfffeUL);
    }
}

void MapiJpdSetDifferentQTable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;
    u16Value = ((HalJpdGetGlobalSetting00(DevId) & ~NJPD_SUVQ) | ((MS_U16)bEnable << 7));
    MhalJpdSetGlobalSetting00(DevId, u16Value);
}

void MapiJpdSetDifferentHTable(MS_U8 DevId, MS_BOOL bEnable)
{
    MS_U16 u16Value;

    JPD_LOGD("%s\n", __FUNCTION__);
    g_stJpdDecodeCtrl[DevId].bIs3HuffTbl = bEnable;
    u16Value                             = ((HalJpdGetGlobalSetting00(DevId) & ~NJPD_SUVH) | ((MS_U16)bEnable << 8));
    MhalJpdSetGlobalSetting00(DevId, u16Value);
}

void MapiJpdSetGlobalSetting00(MS_U8 DevId, MS_U16 u16Value)
{
    MhalJpdSetGlobalSetting00(DevId, u16Value);
}

MS_U16 MapiJpdGetGlobalSetting00(MS_U8 DevId)
{
    return HalJpdGetGlobalSetting00(DevId);
}

void MapiJpdSetHTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetHTableStart_8G_0(u32ByteOffset & 0xffffUL);
        HAL_NJPD_EXT_SetHTableStart_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetHTableStart_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        MhalJpdSetHTableStartLow(DevId, (u32ByteOffset & 0xffffUL));
        MhalJpdSetHTableStartHigh(DevId, (u32ByteOffset >> 16) & 0xffffUL);
    }
}

void MapiJpdSetQTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetQTableStart_8G_0(u32ByteOffset & 0xffffUL);
        HAL_NJPD_EXT_SetQTableStart_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetQTableStart_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        MhalJpdSetQTableStartLow(DevId, (u32ByteOffset & 0xffffUL));
        MhalJpdSetQTableStartHigh(DevId, (u32ByteOffset >> 16) & 0xffffUL);
    }
}

void MapiJpdSetGTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset)
{
#ifdef NJPD_SUPPORT_8G_NJPD_EXT
    if (HAL_NJPD_EXT_IsEnable8G())
    {
        HAL_NJPD_EXT_SetGTableStart_8G_0(u32ByteOffset & 0xffffUL);
        HAL_NJPD_EXT_SetGTableStart_8G_1((u32ByteOffset >> 16) & 0xffffUL);
        HAL_NJPD_EXT_SetGTableStart_8G_2(((MS_U64)u32ByteOffset) >> 32);
    }
    else
#endif
    {
        unsigned long u16TAddrOffset;
        u16TAddrOffset = (u32ByteOffset >> 32) & 0xf;
        MhalJpdSetGTableStartLow(DevId, (u32ByteOffset & 0xffffUL));
        MhalJpdSetGTableStartHigh(DevId, (u32ByteOffset >> 16) & 0xffffUL);
        u16TAddrOffset = u16TAddrOffset << 12;
        MhalJpdSetTable64BHigh(DevId, u16TAddrOffset);
    }
}

// Enable clk gating/sram clk gating for power saving;
// Set register clk gate (0d) 16bit & sram clk gate (2e-2f) 20bit with 1
void MapiJpdEnablePowerSaving(MS_U8 DevId)
{
    // Set register clk gate (0d) 8bit temporarily for IMI case fail, need to fix me.
    MhalJpdSetClockGate(DevId, 0xffUL);
    // Close sram clk gate temporarily for IMI case fail, need to fix me.
    //    MhalJpdSetSramClockGate(DevId, 0x000fffffUL);
}

void MapiJpdSetWRingBufLineNum(MS_U8 DevId, MS_U16 LineNum)
{
    MS_U16 u16Value;
    JPD_LOGD("[notice] MDrv_NJPD_SetWbuf_LineNum =%d \n", LineNum);
    u16Value = ((MhalJpdGetSpare02(DevId) & ~NJPD_RING_NUM) | (LineNum << 0));
    MhalJpdSetSpare02(DevId, u16Value);
}

void MapiJpdSetWRingBufEnable(MS_U8 DevId, MS_U16 IsEnable)
{
    MS_U16 u16Value;
    JPD_LOGD("[notice] MapiJpdSetWRingBufEnable =%d\n", IsEnable);
    u16Value = ((MhalJpdGetSpare02(DevId) & ~NJPD_RING_ENABLE) | (IsEnable << 15));
    MhalJpdSetSpare02(DevId, u16Value);
}

MS_U16 MapiJpdGetWRingBufEnable(MS_U8 DevId)
{
    MS_U16 u16Value;
    u16Value = (MhalJpdGetSpare02(DevId) & NJPD_RING_ENABLE) >> 15;
    JPD_LOGD("[notice] MapiJpdGetWRingBufEnable %d\n", u16Value);
    return u16Value;
}
