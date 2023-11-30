/*
 * drv_jpd_parser.c- Sigmastar
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
#include <cam_os_wrapper.h>
#include "hal_jpd_ops.h"
#include "hal_jpd_def.h"
#include "drv_jpd_dev.h"
#include "drv_jpd_vdi.h"
#include "drv_jpd_parser.h"
#include "drv_jpd_ctx.h"
#include <malloc.h>
#include <common.h>

#if defined(__KERNEL__)
#include <string.h>
#define JPEG_MALLOC(x) malloc(x)
#define JPEG_FREE(x)   free(x)
#endif

#define SOI_CHECK_LENGTH 1024

//------------------------------------------------------------------------------
/// @brief \b JPEG_DEFAULT_EXIF_SIZE : The buffer size for thumbnail
//------------------------------------------------------------------------------

static const MS_U8 g_u8DefaultDHT[] = {
    0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02,
    0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21,
    0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1,
    0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53,
    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9,
    0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03,
    0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06,
    0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23,
    0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53,
    0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
    0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8,
    0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9,
    0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa};

/***************************** EXIF parsing section *****************************/
#define JPEG_TIFF_BIG_ENDIAN        0x4D4DUL
#define JPEG_TIFF_LITTLE_ENDIAN     0x4949UL
#define MAX_NUM_OF_INTEROPERABILITY 0xffUL

// EXIF Tag
#define JPEG_EXIF_TAG_MANUFACTURER     0x010FUL
#define JPEG_EXIF_TAG_MODEL            0x0110UL
#define JPEG_EXIF_TAG_ORIENTATION      0x0112UL
#define JPEG_EXIF_TAG_DATETIME_MOD     0x0132UL
#define JPEG_EXIF_TAG_EXPOSURE_TIME    0x829AUL
#define JPEG_EXIF_TAG_F_NUMBER         0x829DUL
#define JPEG_EXIF_TAG_IFD_POINTER      0x8769UL
#define JPEG_EXIF_TAG_EXPOSURE_PROGRAM 0x8822UL
#define JPEG_EXIF_TAG_ISO_SPEED_RATING 0x8827UL
#define JPEG_EXIF_TAG_DATETIME_ORI     0x9003UL
#define JPEG_EXIF_TAG_SHUTTER_SPEED    0x9201UL
#define JPEG_EXIF_TAG_APERTURE         0x9202UL
#define JPEG_EXIF_TAG_EXPOSURE_BIAS    0x9204UL
#define JPEG_EXIF_TAG_FLASH            0x9209UL
#define JPEG_EXIF_TAG_FOCAL_LENGTH     0x920AUL
#define JPEG_EXIF_TAG_IMAGE_WIDTH      0xA002UL
#define JPEG_EXIF_TAG_IMAGE_HEIGHT     0xA003UL

#define JPEG_TIFF_SOI_OFFSET     0x0201UL
#define JPEG_TIFF_JPEG_IMG_BYTES 0x0202UL

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define EndianChangeL(_x) \
    ((((_x)&0xffUL) << 24) | (((_x)&0xff00UL) << 8) | (((_x)&0xff0000UL) >> 8) | (((_x)&0xff000000UL) >> 24))

#define EndianChangeS(_x) ((((_x)&0xffUL) << 8) | (((_x)&0xff00UL) >> 8))

#define JPEG_TAG_EXIF EndianChangeL(0x45786966UL)

JPEG_STATIC MS_BOOL _JpegDecodeExifInfo(MS_U8 *data, MS_U32 data_length, JPD_Ctx_t *pCtx);
/****************************EXIF parsing section *****************************/

/****************************progressive section *****************************/
/* entry n is (-1 << n) + 1 */
static const MS_S32 extend_offset[16] = {0,
                                         ((-1) << 1) + 1,
                                         ((-1) << 2) + 1,
                                         ((-1) << 3) + 1,
                                         ((-1) << 4) + 1,
                                         ((-1) << 5) + 1,
                                         ((-1) << 6) + 1,
                                         ((-1) << 7) + 1,
                                         ((-1) << 8) + 1,
                                         ((-1) << 9) + 1,
                                         ((-1) << 10) + 1,
                                         ((-1) << 11) + 1,
                                         ((-1) << 12) + 1,
                                         ((-1) << 13) + 1,
                                         ((-1) << 14) + 1,
                                         ((-1) << 15) + 1};

/* entry n is 2**(n-1) */
static const MS_S32 extend_test[16] = {0,    0x1,   0x2,   0x4,   0x8,   0x10,   0x20,   0x40,
                                       0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};
/*
static const MS_S32 extend_mask[] = {
    0,        (1 << 0), (1 << 1),  (1 << 2),  (1 << 3),  (1 << 4),  (1 << 5),  (1 << 6),  (1 << 7),
    (1 << 8), (1 << 9), (1 << 10), (1 << 11), (1 << 12), (1 << 13), (1 << 14), (1 << 15), (1 << 16),
};
*/

#define HUFF_EXTEND_TBL(x, s) ((x) < extend_test[s] ? (x) + extend_offset[s] : (x))

#define HUFF_EXTEND_P(x, s) HUFF_EXTEND_TBL(x, s)

typedef MS_BOOL (*Pdecode_block_func)(JPD_Ctx_t *pCtx, MS_U8, MS_U16, MS_U16);

/****************************progressive section *****************************/

JPEG_STATIC void _JpegTraceError(JPD_Ctx_t *pCtx, EN_JPEG_ERR_CODE status)
{
    pCtx->Error_code = status;

    JpegFreeAllBlocks(pCtx);

    JPD_LOGE("ERROR= %d\n", pCtx->Error_code);
}
//------------------------------------------------------------------------------
// Logical rotate left operation.
JPEG_STATIC MS_U32 _JpegRotateLeft(MS_U32 i, MS_U8 j)
{
    return ((i << j) | (i >> (32 - j)));
}

JPEG_STATIC MS_U8 _JpegGetChar(JPD_Ctx_t *pCtx)
{
    MS_U8 c;

    // Any bytes remaining in buffer?
    if (!pCtx->u32In_buf_left_bytes)
    {
        // Try to get more bytes.
        // Still nothing to get?
        if (!pCtx->u32In_buf_left_bytes)
        {
            // Padd the end of the stream with 0xFF 0xD9 (EOI marker)
            // FIXME: Is there a better padding pattern to use?
            MS_U8 t = pCtx->u8Tem_flag;
            pCtx->u8Tem_flag ^= 1;

            return t ? E_JPEG_EOI : 0xFF;
        }
    }

    if (pCtx->u32In_buf_left_bytes <= pCtx->u32In_buf2_bytes)
    {
        JPD_LOGD("_JpegGetChar pu8In_buf_offset2 !!!\n");
        c = *pCtx->pu8In_buf_offset2++;
    }
    else
    {
        c = *pCtx->pu8In_buf_offset1++;
    }

    pCtx->u32In_buf_left_bytes--;

    return (c);
}

JPEG_STATIC void _JpegSkipBytes(MS_U32 count, JPD_Ctx_t *pCtx)
{
    while (count != 0)
    {
        // Any bytes remaining in buffer?
        if (!pCtx->u32In_buf_left_bytes)
        {
            // Try to get more bytes.
            JPD_LOGD("_JpegGetChar fail !!!\n");
            // Still nothing to get?
            if (!pCtx->u32In_buf_left_bytes)
            {
                // should not happen
                break;
            }
        }

        if (count < pCtx->u32In_buf_left_bytes)
        {
            pCtx->u32In_buf_left_bytes -= count;
            if (pCtx->u32In_buf_left_bytes <= pCtx->u32In_buf2_bytes)
            {
                pCtx->pu8In_buf_offset2 += count;
            }
            else
            {
                pCtx->pu8In_buf_offset1 += count;
            }
            count = 0;
        }
        else
        {
            count -= pCtx->u32In_buf_left_bytes;
            pCtx->u32In_buf_left_bytes = 0;
        }
    }
}

// Finds the next marker.
JPEG_STATIC MS_U32 _JpegNextMarker(JPD_Ctx_t *pCtx)
{
    MS_U32 c;

    do
    {
        do
        {
            c = _JpegGetChar(pCtx);
            // JPD_LOGD("c = %X\n",c);
        } while (c != 0xFF);

        do
        {
            c = _JpegGetChar(pCtx);
            // JPD_LOGD("c = %X\n",c);
        } while (c == 0xFF);
    } while (c == 0);

    return c;
}

// Finds the start of image (SOI) marker.
JPEG_STATIC MS_S32 _JpegLocateSOIMarker(JPD_Ctx_t *pCtx)
{
    MS_U8  lastchar, thischar;
    MS_U32 bytesleft;

    lastchar = _JpegGetChar(pCtx);
    thischar = _JpegGetChar(pCtx);

    /* ok if it's a normal JPEG file without a special header */
    if ((0xFF == lastchar) && (E_JPEG_SOI == thischar))
    {
        JPD_LOGD("SOI\n");
        return TRUE;
    }

    /* ok if it's a normal JPEG file without a special header */
    if ((0xFF == lastchar) && (E_JPEG_SOI == thischar))
    {
        JPD_LOGD("SOI\n");
        return TRUE;
    }

    // Set this value to 0x1000 for 4k alignment MPO case when parse 2nd/3rd/... JPEG file
    bytesleft = 0x1000; // Fix this number from 512 -> 640 for some cases

    for (;;)
    {
        if (--bytesleft == 0)
        {
            _JpegTraceError(pCtx, E_JPEG_NO_ERROR);
            return FALSE;
        }

        lastchar = thischar;
        thischar = _JpegGetChar(pCtx);

        if ((0xFF == lastchar) && (E_JPEG_SOI == thischar))
        {
            JPD_LOGD("SOI\n");
            break;
        }
    }

    return TRUE;
}

JPEG_STATIC MS_BOOL _JpegLocateEOIMarker(JPD_Ctx_t *pCtx, MS_U32 loacl)
{
    MS_U8 c0, c1;
    c0 = *(pCtx->pu8In_buf_offset1 + loacl);
    c1 = *(pCtx->pu8In_buf_offset1 + loacl + 1);
    JPD_LOGD(" <%X %X> \n", c0, c1);

    if (c0 != 0xFF || c1 != E_JPEG_EOI)
    {
        JPD_LOGE(" <%X %X> JPD not end with FF %X\n", c0, c1, E_JPEG_EOI);
        _JpegTraceError(pCtx, E_JPEG_NOT_JPEG);

        return false;
    }

    return true;
}

// Parse APP0 to get X&Y density.
JPEG_STATIC MS_BOOL _JpegReadApp0Marker(JPD_Ctx_t *pCtx)
{
    MS_U32 left;
    MS_U8  u8Xthumbnail, u8Ythumbnail;
    // JPD_LOGD("APP0\n");

    // Get APP0 length
    left = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    left -= 2;

    if (left < 8)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_APP0_MARKER);
        return FALSE;
    }

    // Skip Block ID "JFIF"
    _JpegSkipBytes(5, pCtx);

    left -= 5;

    // Skip Version Number
    _JpegSkipBytes(2, pCtx);

    left -= 2;

    // Get units
    pCtx->APP0_UNIT_Data.u8Unit      = _JpegGetChar(pCtx);
    pCtx->APP0_UNIT_Data.u16Xdensity = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));
    pCtx->APP0_UNIT_Data.u16Ydensity = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    left -= 5;

    // Get thumbnail x,y
    u8Xthumbnail = _JpegGetChar(pCtx);
    u8Ythumbnail = _JpegGetChar(pCtx);

    left -= 2;

    // Skip Version Number
    _JpegSkipBytes(3 * u8Xthumbnail * u8Ythumbnail, pCtx);

    left -= 3 * u8Xthumbnail * u8Ythumbnail;

    JPD_LOGD("APP0 : unit : %d\n", (MS_U16)pCtx->APP0_UNIT_Data.u8Unit);
    JPD_LOGD("APP0 : Xdensity : %d\n", pCtx->APP0_UNIT_Data.u16Xdensity);
    JPD_LOGD("APP0 : Ydensity : %d\n", pCtx->APP0_UNIT_Data.u16Ydensity);

    if (left > 0)
    {
        _JpegSkipBytes(left, pCtx);
    }

    return TRUE;
}

// Used to skip unrecognized markers.
JPEG_STATIC MS_BOOL _JpegSkipVariableMarker(JPD_Ctx_t *pCtx)
{
    MS_U32 left;

    JPD_LOGD("SKIP markers\n");

    left = (MS_U32)((_JpegGetChar(pCtx) << 8) | _JpegGetChar(pCtx));

    if (left < 2)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_VARIABLE_MARKER);
        return FALSE;
    }

    left -= 2;

    _JpegSkipBytes(left, pCtx);
    return TRUE;
}

// Read exif info
JPEG_STATIC MS_BOOL _JpegReadApp1Marker(JPD_Ctx_t *pCtx)
{
    MS_U16 length;
    MS_U8 *exifBuffer = NULL;
    MS_U16 i          = 0;

    JPD_CHECK_VALID_POINTER(pCtx->stInternalBuf.pInternalBufVA);
    exifBuffer = (MS_U8 *)(unsigned long)pCtx->stInternalBuf.pInternalBufVA;
    JPD_LOGD("APP1\n");

    JPD_LOGD("Inter buf = %p\n", pCtx->stInternalBuf.pInternalBufVA);

    length = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    if (length < 2)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_APP1_MARKER);
        return FALSE;
    }

    length -= 2;
    JPD_LOGD("length = %d\n", length);
    while ((length - i) != 0)
    {
        exifBuffer[i] = _JpegGetChar(pCtx);
        i++;
    }

    if (_JpegDecodeExifInfo(exifBuffer, length, pCtx) == TRUE)
    {
        JPD_LOGD("FOUND THUMBNAIL!\n");
        pCtx->stThumbnailInfo.u32ThumbnailBufferOffset = pCtx->stThumbnailInfo.u32ThumbnailOffset;
        pCtx->stThumbnailInfo.u16ThumbnailBufferSize   = pCtx->stThumbnailInfo.u16ThumbnailSize;
    }
    else
    {
        JPD_LOGD("NO THUMBNAIL!\n");
    }

    return TRUE;
}

JPEG_STATIC MS_BOOL _JpegReadDefaultDht(JPD_Ctx_t *pCtx)
{
    MS_U16 i, index, count;
    MS_U32 left;
    MS_U8  u8HuffNum[17];
    MS_U8  u8HuffVal[256];
    MS_U8  u8Valid[17];
    MS_U32 u32DefaultDHTIndex = 0;

    JPD_LOGD("Use Default DHT!\n");

    left = (MS_U16)((g_u8DefaultDHT[u32DefaultDHTIndex] << 8) + g_u8DefaultDHT[u32DefaultDHTIndex + 1]);
    u32DefaultDHTIndex += 2;

    if (left < 2)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_DHT_MARKER);
        return FALSE;
    }

    left -= 2;

    while (left)
    {
        // set it to zero, initialize
        JPEG_memset((void *)u8HuffNum, 0, 17);
        JPEG_memset((void *)u8Valid, 0, 17);
        JPEG_memset((void *)u8HuffVal, 0, 256);

        index = g_u8DefaultDHT[u32DefaultDHTIndex];
        u32DefaultDHTIndex++;

        u8HuffNum[0] = 0;

        count = 0;

        for (i = 1; i <= 16; i++)
        {
            u8HuffNum[i] = g_u8DefaultDHT[u32DefaultDHTIndex];
            u32DefaultDHTIndex++;
            count += u8HuffNum[i];
        }

        if (count > 255)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_COUNTS);
            return FALSE;
        }

        for (i = 0; i < count; i++)
        {
            u8HuffVal[i] = g_u8DefaultDHT[u32DefaultDHTIndex];
            u32DefaultDHTIndex++;
        }

        i = 1 + 16 + count;

        if (left < (MS_U32)i)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_MARKER);
            return FALSE;
        }

        left -= i;

        index = (index & 0x0F) + ((index & 0x10) >> 4) * (JPEG_MAXHUFFTABLES >> 1); //???

        if (index >= JPEG_MAXHUFFTABLES)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_INDEX);
            return FALSE;
        }
        pCtx->stHeadInfo.stHuff_info[index].u8SymbolCnt = count;

        if (pCtx->stHeadInfo.stHuff_info[index].bValid == FALSE)
        {
            pCtx->stHeadInfo.stHuff_info[index].bValid = TRUE;
        }

        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[index].u8HuffNum, (void *)u8HuffNum, 17);
        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[index].u8HuffVal, (void *)u8HuffVal, 256);

        // Compute the inverse order of HuffNum. this step is only needed in NJPD mode (baseline)
        for (i = 1; i <= 16; i++)
        {
            if (u8HuffNum[17 - i] != 0)
            {
                count             = count - u8HuffNum[17 - i];
                u8HuffNum[17 - i] = count;
                u8Valid[17 - i]   = 1;
            }
            else
            {
                count             = count - u8HuffNum[17 - i];
                u8HuffNum[17 - i] = count;
                u8Valid[17 - i]   = 0;
            }
        }

        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[index].u8Symbol, (void *)u8HuffNum, 17);
        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[index].u8Valid, (void *)u8Valid, 17);
    }
    return TRUE;
}

// Read a Huffman code table.
JPEG_STATIC MS_BOOL _JpegReadDhtMarker(JPD_Ctx_t *pCtx)
{
    MS_U16 i, indextmp, count;
    MS_U32 left;
    MS_U8  u8HuffNum[17];
    MS_U8  u8HuffVal[256];
    MS_U8  u8Valid[17];

    JPD_LOGD("DHT\n");

    left = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    if (left < 2)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_DHT_MARKER);
        return FALSE;
    }

    left -= 2;

    while (left)
    {
        // set it to zero, initialize
        JPEG_memset((void *)u8HuffNum, 0, 17);
        JPEG_memset((void *)u8Valid, 0, 17);
        JPEG_memset((void *)u8HuffVal, 0, 256);

        indextmp = _JpegGetChar(pCtx);

        u8HuffNum[0] = 0;

        count = 0;

        for (i = 1; i <= 16; i++)
        {
            u8HuffNum[i] = _JpegGetChar(pCtx);

            count += u8HuffNum[i];
        }

        if (count > 255)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_COUNTS);
            return FALSE;
        }

        for (i = 0; i < count; i++)
        {
            u8HuffVal[i] = _JpegGetChar(pCtx);
        }

        i = 1 + 16 + count;

        if (left < (MS_U32)i)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_MARKER);
            return FALSE;
        }

        left -= i;

        indextmp = (indextmp & 0x0F) + ((indextmp & 0x10) >> 4) * (JPEG_MAXHUFFTABLES >> 1); //???

        if (indextmp >= JPEG_MAXHUFFTABLES)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DHT_INDEX);
            return FALSE;
        }

        pCtx->stHeadInfo.stHuff_info[indextmp].u8SymbolCnt = count;
        if (FALSE == pCtx->stHeadInfo.stHuff_info[indextmp].bValid)
        {
            pCtx->stHeadInfo.stHuff_info[indextmp].bValid = TRUE;
        }

        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[indextmp].u8HuffNum, (void *)u8HuffNum, 17);
        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[indextmp].u8HuffVal, (void *)u8HuffVal, 256);

        // Compute the inverse order of HuffNum. this step is only needed in NJPD mode (baseline)
        for (i = 1; i <= 16; i++)
        {
            if (u8HuffNum[17 - i] != 0)
            {
                count             = count - u8HuffNum[17 - i];
                u8HuffNum[17 - i] = count;
                u8Valid[17 - i]   = 1;
            }
            else
            {
                count             = count - u8HuffNum[17 - i];
                u8HuffNum[17 - i] = count;
                u8Valid[17 - i]   = 0;
            }
            // JPD_LOGD("inverse u8HuffVal[%d] : %x\n",i,u8HuffVal[i]);
        }

        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[indextmp].u8Symbol, (void *)u8HuffNum, 17);
        JPEG_memcpy((void *)pCtx->stHeadInfo.stHuff_info[indextmp].u8Valid, (void *)u8Valid, 17);
    }

    return TRUE;
}

//------------------------------------------------------------------------------
// Read a quantization table.
JPEG_STATIC MS_BOOL _JpegReadDqtMarker(JPD_Ctx_t *pCtx)
{
    MS_U16 n, i, prec;
    MS_U32 left;
    MS_U32 temp;

    JPD_LOGD("DQT\n");

    left = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    if (left < 2)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_DQT_MARKER);
        return FALSE;
    }

    left -= 2;

    while (left)
    {
        n    = _JpegGetChar(pCtx);
        prec = n >> 4;
        n &= 0x0F;

        if (n >= JPEG_MAXQUANTTABLES)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DQT_TABLE);
            return FALSE;
        }

        if (FALSE == pCtx->stHeadInfo.stQuantTables[n].bValid)
        {
            pCtx->stHeadInfo.stQuantTables[n].bValid = TRUE;
        }

        // read quantization entries, in zag order
        for (i = 0; i < 64; i++)
        {
            temp = _JpegGetChar(pCtx);

            if (prec)
            {
                temp = (temp << 8) + _JpegGetChar(pCtx);
            }

            pCtx->stHeadInfo.stQuantTables[n].s16Value[i] = temp;
        }

        i = 64 + 1;

        if (prec)
        {
            i += 64;
        }

        if (left < (MS_U32)i)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_DQT_LENGTH);
            return FALSE;
        }

        left -= i;
    }

    return TRUE;
}

// Read a define restart interval (DRI) marker.
JPEG_STATIC MS_BOOL _JpegRreadDriMarker(JPD_Ctx_t *pCtx)
{
    JPD_LOGD("DRI\n");

    if ((MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx)) != 4)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_DRI_LENGTH);
        return FALSE;
    }

    pCtx->stHeadInfo.u16Restart_interval = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    return TRUE;
}

JPEG_STATIC MS_S32 _JpegProcessMarkers(JPD_Ctx_t *pCtx)
{
    MS_U32 c;

    JPD_LOGD("\n");
    for (;;)
    {
        c = _JpegNextMarker(pCtx);
        switch (c)
        {
            case E_JPEG_APP1:
                // Prevent from there's thumbnail in thumbnail... & multiple APP1
                // Although it's impossible.. =_=
                // We need to get EXIF info for decoding main picture or thumbnail.
                if ((FALSE == pCtx->bIsMjpeg) && (FALSE == pCtx->stThumbnailInfo.bThumbnailFound))
                {
                    if (!_JpegReadApp1Marker(pCtx))
                    {
                        _JpegTraceError(pCtx, E_JPEG_BAD_APP1_MARKER);
                        return FALSE;
                    }
                }
                else
                {
                    if (!_JpegSkipVariableMarker(pCtx))
                    {
                        return FALSE;
                    }
                }
                break;
                // case E_JPEG_APP2: //support mpo
                // if( !JPEG_read_app2_marker() )
                //     return FALSE;
                //     break;
            case E_JPEG_SOF0:
            case E_JPEG_SOF1:
            case E_JPEG_SOF2:
            case E_JPEG_SOF3:
            case E_JPEG_SOF5:
            case E_JPEG_SOF6:
            case E_JPEG_SOF7:
                //      case E_JPEG_JPG:
            case E_JPEG_SOF9:
            case E_JPEG_SOF10:
            case E_JPEG_SOF11:
            case E_JPEG_SOF13:
            case E_JPEG_SOF14:
            case E_JPEG_SOF15:
            case E_JPEG_SOI:
            case E_JPEG_EOI:
            case E_JPEG_SOS:
            {
                return c;
            }
            case E_JPEG_DHT:
            {
                if (!_JpegReadDhtMarker(pCtx))
                {
                    return FALSE;
                }
                pCtx->stDealInfo.HeadCheck.DHT = TRUE;
                break;
            }
                // Sorry, no arithmitic support at this time. Dumb patents!
            case E_JPEG_DAC:
            {
                _JpegTraceError(pCtx, E_JPEG_NO_ARITHMETIC_SUPPORT);
                return FALSE;
                break;
            }
            case E_JPEG_DQT:
            {
                if (!_JpegReadDqtMarker(pCtx))
                {
                    return FALSE;
                }
                pCtx->stDealInfo.HeadCheck.DQT = TRUE;
                break;
            }
            case E_JPEG_DRI:
            {
                if (!_JpegRreadDriMarker(pCtx))
                {
                    return FALSE;
                }
                break;
            }
            case E_JPEG_JPG:
            case E_JPEG_RST0:
                /* no parameters */
            case E_JPEG_RST1:
            case E_JPEG_RST2:
            case E_JPEG_RST3:
            case E_JPEG_RST4:
            case E_JPEG_RST5:
            case E_JPEG_RST6:
            case E_JPEG_RST7:
            case E_JPEG_TEM:
            {
                _JpegTraceError(pCtx, E_JPEG_UNEXPECTED_MARKER);
                return FALSE;
                break;
            }
            case E_JPEG_APP0:
                if (!pCtx->bMHEG5)
                {
                    if (!_JpegSkipVariableMarker(pCtx))
                    {
                        return FALSE;
                    }
                    break;
                }
                if (!_JpegReadApp0Marker(pCtx))
                {
                    return FALSE;
                }
                break;
            case E_JPEG_APP2:
            case E_JPEG_DNL:
            case E_JPEG_DHP:
            case E_JPEG_EXP:
            case E_JPEG_APP3:
            case E_JPEG_APP4:
            case E_JPEG_APP5:
            case E_JPEG_APP6:
            case E_JPEG_APP7:
            case E_JPEG_APP8:
            case E_JPEG_APP9:
            case E_JPEG_APP10:
            case E_JPEG_APP11:
            case E_JPEG_APP12:
            case E_JPEG_APP13:
            case E_JPEG_APP14:
            case E_JPEG_APP15:
            case E_JPEG_JPG0:
            case E_JPEG_JPG1:
            case E_JPEG_JPG2:
            case E_JPEG_JPG3:
            case E_JPEG_JPG4:
            case E_JPEG_JPG5:
            case E_JPEG_JPG6:
            case E_JPEG_JPG7:
            case E_JPEG_JPG8:
            case E_JPEG_JPG9:
            case E_JPEG_JPG10:
            case E_JPEG_JPG11:
            case E_JPEG_JPG12:
            case E_JPEG_JPG13:
            case E_JPEG_COM:
                /* must be DNL, DHP, EXP, APPn, JPGn, COM, or RESn or APP0 */
                {
                    if (!_JpegSkipVariableMarker(pCtx))
                    {
                        return FALSE;
                    }
                    break;
                }
            default:
                break;
        }
    }
    return TRUE;
}
//------------------------------------------------------------------------------
// Read a start of scan (SOS) marker.
JPEG_STATIC MS_BOOL _JpegReadSOSMarker(JPD_Ctx_t *pCtx)
{
    MS_U32 left;
    MS_U16 i, ci, n, c, cc;
    MS_U8  c1;

    JPD_LOGD("SOS\n");

    left = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx)); // 00 0c //12

    n = _JpegGetChar(pCtx); // 03

    pCtx->stHeadInfo.u8Comps_in_scan = n;

    left -= 3;
    if ((left != (MS_U32)(n * 2 + 3)) || (n < 1) || (n > JPEG_MAXCOMPSINSCAN))
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_SOS_LENGTH);
        return FALSE;
    }

    for (i = 0; i < n; i++) // 3
    {
        cc = _JpegGetChar(pCtx);
        c  = _JpegGetChar(pCtx);
        left -= 2;

        for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
        {
            if (cc == pCtx->stHeadInfo.u8Comp_ident[ci])
            {
                break;
            }
        }

        if (ci >= pCtx->stHeadInfo.u8Comps_in_frame)
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_SOS_COMP_ID);
            return FALSE;
        }
        pCtx->stHeadInfo.u8Comp_list[i]    = ci;
        pCtx->stHeadInfo.u8Comp_dc_tab[ci] = (c >> 4) & 15;
        pCtx->stHeadInfo.u8Comp_ac_tab[ci] = (c & 15) + (JPEG_MAXHUFFTABLES >> 1);
    }

    // HW limitation, for baseline JPEG, U.V need to refer to the same DC and AC huffman table.
    if (!pCtx->stHeadInfo.bProgressive_flag && pCtx->stHeadInfo.u8Comps_in_frame == 3) // Y.U.V.
    {
        if ((pCtx->stHeadInfo.u8Comp_dc_tab[1] != pCtx->stHeadInfo.u8Comp_dc_tab[2])
            || (pCtx->stHeadInfo.u8Comp_ac_tab[1] != pCtx->stHeadInfo.u8Comp_ac_tab[2]))
        {
            JPD_LOGD("U, V use different Huffman table~~\n");
            pCtx->stDealInfo.bIs3HuffTbl = TRUE;
        }
    }
    pCtx->stHeadInfo.u8Spectral_start = _JpegGetChar(pCtx);
    pCtx->stHeadInfo.u8Spectral_end   = _JpegGetChar(pCtx);

    c1                                 = _JpegGetChar(pCtx);
    pCtx->stHeadInfo.u8Successive_high = (c1 & 0xf0) >> 4;
    pCtx->stHeadInfo.u8Successive_low  = (c1 & 0x0f);

    if (!pCtx->stHeadInfo.bProgressive_flag)
    {
        pCtx->stHeadInfo.u8Spectral_start = 0;
        pCtx->stHeadInfo.u8Spectral_end   = 63;
    }

    left -= 3;

    _JpegSkipBytes(left, pCtx); /* read past whatever is left */
    return TRUE;
}

//------------------------------------------------------------------------------
// Determines the component order inside each MCU.
// Also calcs how many MCU's are on each row, etc.
JPEG_STATIC void _JpegCalcMcuBlockOrder(JPD_Ctx_t *pCtx) // ok
{
    MS_U8 componentNum, componentId;
    MS_U8 maxHSamp = 0, maxVSamp = 0;
    JPD_LOGD("%s\n", __FUNCTION__); // fd fc a2 8a 28 00 a2 8a

    for (componentId = 0; componentId < pCtx->stHeadInfo.u8Comps_in_frame; componentId++)
    {
        if (pCtx->stHeadInfo.u8Comp_h_samp[componentId] > maxHSamp)
        {
            maxHSamp = pCtx->stHeadInfo.u8Comp_h_samp[componentId];
            JPD_LOGD("Comp_h_samp=%d\n", pCtx->stHeadInfo.u8Comp_h_samp[componentId]);
        }

        if (pCtx->stHeadInfo.u8Comp_v_samp[componentId] > maxVSamp)
        {
            maxVSamp = pCtx->stHeadInfo.u8Comp_v_samp[componentId];
            JPD_LOGD("Comp_v_samp =%d\n", pCtx->stHeadInfo.u8Comp_v_samp[componentId]);
        }
    }

    if ((maxHSamp == 0) || (maxVSamp == 0)) // maxHSamp=2  maxVSamp=2
    {
        _JpegTraceError(pCtx, E_JPEG_NOT_ENOUGH_HEADER_INFO);
        return;
    }

    for (componentId = 0; componentId < pCtx->stHeadInfo.u8Comps_in_frame; componentId++)
    {
        pCtx->stDealInfo.u16Comp_h_blocks[componentId] =
            ((((pCtx->stHeadInfo.u16Image_x_size * pCtx->stHeadInfo.u8Comp_h_samp[componentId]) + (maxHSamp - 1))
              / maxHSamp)
             + 7)
            / 8;
        pCtx->stDealInfo.u16Comp_v_blocks[componentId] =
            ((((pCtx->stHeadInfo.u16Image_y_size * pCtx->stHeadInfo.u8Comp_v_samp[componentId]) + (maxVSamp - 1))
              / maxVSamp)
             + 7)
            / 8;
    }

    if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
    {
        pCtx->stDealInfo.u16Mcus_per_row = pCtx->stDealInfo.u16Comp_h_blocks[pCtx->stHeadInfo.u8Comp_list[0]];
        pCtx->stDealInfo.u16Mcus_per_col = pCtx->stDealInfo.u16Comp_v_blocks[pCtx->stHeadInfo.u8Comp_list[0]];
    }
    else
    {
        pCtx->stDealInfo.u16Mcus_per_row = (((pCtx->stHeadInfo.u16Image_x_size + 7) / 8) + (maxHSamp - 1)) / maxHSamp;
        pCtx->stDealInfo.u16Mcus_per_col = (((pCtx->stHeadInfo.u16Image_y_size + 7) / 8) + (maxVSamp - 1)) / maxVSamp;
    }

    if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
    {
        pCtx->stDealInfo.u8Mcu_org[0] = pCtx->stHeadInfo.u8Comp_list[0];

        pCtx->stDealInfo.u8Blocks_per_mcu = 1;
    }
    else
    {
        pCtx->stDealInfo.u8Blocks_per_mcu = 0;

        for (componentNum = 0; componentNum < pCtx->stHeadInfo.u8Comps_in_scan; componentNum++)
        {
            MS_U8 numBlocks;

            componentId = pCtx->stHeadInfo.u8Comp_list[componentNum];

            numBlocks = pCtx->stHeadInfo.u8Comp_h_samp[componentId] * pCtx->stHeadInfo.u8Comp_v_samp[componentId];

            while (numBlocks--)
            {
                pCtx->stDealInfo.u8Mcu_org[pCtx->stDealInfo.u8Blocks_per_mcu++] = componentId;
            }
        }
    }
}

//------------------------------------------------------------------------------
// Same as previus method, except can indicate if the character is
// a "padd" character or not.
JPEG_STATIC MS_U8 _JpegGetCharP(JPD_Ctx_t *pCtx, MS_BOOL *Ppadding_flag)
{
    MS_U8 c;
    if (!pCtx->u32In_buf_left_bytes)
    {
        if (!pCtx->u32In_buf_left_bytes)
        {
            *Ppadding_flag = TRUE;
#if 1
            pCtx->u8Tem_flag ^= 1;
            //  printf_red("_u8Tem_flag %s\n",__FUNCTION__);
            // This function is called during when progressive huffman decoding
            // Should not padding 0xFFD9 at this state
            return 0;
#else
            MS_U8 t;
            t = _u8Tem_flag[pNJPEGContext->eNJPDNum];
            _u8Tem_flag[pNJPEGContext->eNJPDNum] ^= 1;
            if (t)
            {
                return (0xD9);
            }
            else
            {
                return (0xFF);
            }
#endif
        }
    }

    *Ppadding_flag = FALSE;

    c = *pCtx->pu8In_buf_offset1++;
    pCtx->u32In_buf_left_bytes--;

    return (c);
}
//------------------------------------------------------------------------------
// Inserts a previously retrieved character back into the input buffer.
JPEG_STATIC void _JpegStuffChar(JPD_Ctx_t *pCtx, MS_U8 q)
{
    *(--pCtx->pu8In_buf_offset1) = q;
    pCtx->u32In_buf_left_bytes++;
}
//------------------------------------------------------------------------------
// Retrieves one character from the input stream, but does
// not read past markers. Will continue to return 0xFF when a
// marker is encountered.
// FIXME: Bad name?
JPEG_STATIC MS_U8 _JpegGetOctet(JPD_Ctx_t *pCtx)
{
    MS_BOOL paddingFlag;
    MS_U8   c = _JpegGetCharP(pCtx, &paddingFlag);

    if (c == 0xFF)
    {
        if (paddingFlag)
        {
            return (0xFF);
        }

        c = _JpegGetCharP(pCtx, &paddingFlag);
        if (paddingFlag)
        {
            _JpegStuffChar(pCtx, 0xFF);
            return (0xFF);
        }

        if (c == 0x00)
        {
            return (0xFF);
        }

        _JpegStuffChar(pCtx, c);
        _JpegStuffChar(pCtx, 0xFF);

        return (0xFF);
    }

    return (c);
}

//------------------------------------------------------------------------------
// Retrieves a variable number of bits from the input stream.
// Markers will not be read into the input bit buffer. Instead,
// an infinite number of all 1's will be returned when a marker
// is encountered.
// FIXME: Is it better to return all 0's instead, like the older implementation?
JPEG_STATIC MS_U32 _JpegGetBits2(JPD_Ctx_t *pCtx, MS_U8 numbits)
{
    MS_U32 i, c1, c2;

    i = (pCtx->stHeadInfo.u32Bit_buf >> (16 - numbits)) & ((1 << numbits) - 1);

    pCtx->stHeadInfo.s16Bits_left -= numbits;
    if (pCtx->stHeadInfo.s16Bits_left <= 0)
    {
        pCtx->stHeadInfo.u32Bit_buf =
            _JpegRotateLeft(pCtx->stHeadInfo.u32Bit_buf, numbits += pCtx->stHeadInfo.s16Bits_left);

        c1 = _JpegGetOctet(pCtx);
        c2 = _JpegGetOctet(pCtx);

        pCtx->stHeadInfo.u32Bit_buf = (pCtx->stHeadInfo.u32Bit_buf & 0xFFFFUL) | (c1 << 24) | (c2 << 16);

        pCtx->stHeadInfo.u32Bit_buf = _JpegRotateLeft(pCtx->stHeadInfo.u32Bit_buf, -pCtx->stHeadInfo.s16Bits_left);

        pCtx->stHeadInfo.s16Bits_left += 16;
    }
    else
    {
        pCtx->stHeadInfo.u32Bit_buf = _JpegRotateLeft(pCtx->stHeadInfo.u32Bit_buf, numbits);
    }

    return i;
}
//------------------------------------------------------------------------------
// Creates the tables needed for efficient Huffman decoding.
JPEG_STATIC MS_BOOL _JpegMakeHuffTable(JPD_Ctx_t *pCtx, MS_U8 indextmp)
{
    MS_U16  p, i, l, si;
    MS_U8 * huffsize = JPEG_MALLOC(257 * sizeof(MS_U8));
    MS_U16 *huffcode = JPEG_MALLOC(257 * sizeof(MS_U16));
    MS_U16  code;
    MS_U16  subtree;
    MS_U16  codeSize;
    MS_U16  lastp;
#if SUPPORT_PROGRESSIVE_MODE
    MS_S16 nextfreeentry;
#endif
    MS_S16         currententry;
    JPEG_HUFF_TBL *hs;

    if (huffsize == NULL || huffcode == NULL)
    {
        if (huffsize)
        {
            JPEG_FREE(huffsize);
        }
        if (huffcode)
        {
            JPEG_FREE(huffcode);
        }
        _JpegTraceError(pCtx, E_JPEG_ASSERTION_ERROR);
        JPD_LOGE("malloc fail!!!\n");
        return FALSE;
    }

#if SUPPORT_PROGRESSIVE_MODE
    hs = &pCtx->stProgressiveInfo.Huff_tbls[indextmp];
#endif

    JPEG_memset(huffsize, 0, 257 * sizeof(MS_U8));
    JPEG_memset(huffcode, 0, 257 * sizeof(MS_U16));
    p = 0;
    for (l = 1; l <= 16; l++)
    {
        for (i = 1; i <= pCtx->stHeadInfo.stHuff_info[indextmp].u8HuffNum[l]; i++)
        {
            huffsize[p++] = l;

            // kevinhuang, add protection
            if (p >= 257)
            {
                _JpegTraceError(pCtx, E_JPEG_UNDEFINED_HUFF_TABLE);
                JPEG_FREE(huffsize);
                JPEG_FREE(huffcode);
                return FALSE;
            }
        }
    }

    huffsize[p] = 0;

    lastp = p;

    code = 0;
    si   = huffsize[0];
    p    = 0;

    while (huffsize[p])
    {
        while (huffsize[p] == si)
        {
            huffcode[p++] = code;
            code++;

            // kevinhuang, add protection
            if (p >= 257)
            {
                _JpegTraceError(pCtx, E_JPEG_UNDEFINED_HUFF_TABLE);
                JPEG_FREE(huffsize);
                JPEG_FREE(huffcode);
                return FALSE;
            }
        }

        code <<= 1;
        si++;
    }

    // Calculate the min code
    for (i = 1; i <= 16; i++)
    {
        pCtx->stHeadInfo.stHuff_info[indextmp].u16Code[i] =
            huffcode[pCtx->stHeadInfo.stHuff_info[indextmp].u8Symbol[i]];
    }

#if SUPPORT_PROGRESSIVE_MODE
    // In NJPD mode, SW doesn't need huff table when baseline decoding

    if (pCtx->stHeadInfo.bProgressive_flag == FALSE)
    {
        JPEG_FREE(huffsize);
        JPEG_FREE(huffcode);
        return TRUE;
    }

    JPEG_memset((void *)(hs->s16Look_up), 0, sizeof(hs->s16Look_up));
    JPEG_memset((void *)(hs->s16Tree), 0, sizeof(hs->s16Tree));
    JPEG_memset((void *)(hs->u8Code_size), 0, sizeof(hs->u8Code_size));

    nextfreeentry = -1;

    p = 0;

    while (p < lastp)
    {
        i        = pCtx->stHeadInfo.stHuff_info[indextmp].u8HuffVal[p];
        code     = huffcode[p];
        codeSize = huffsize[p];

        hs->u8Code_size[i] = codeSize;

        if (codeSize <= 8)
        {
            code <<= (8 - codeSize);

            for (l = 1 << (8 - codeSize); l > 0; l--)
            {
                hs->s16Look_up[code] = i;
                code++;
            }
        }
        else
        {
            subtree = (code >> (codeSize - 8)) & 0xFF;

            currententry = hs->s16Look_up[subtree];

            if (currententry == 0)
            {
                hs->s16Look_up[subtree] = currententry = nextfreeentry;

                nextfreeentry -= 2;
            }

            code <<= (16 - (codeSize - 8));

            for (l = codeSize; l > 9; l--)
            {
                if ((code & 0x8000) == 0)
                {
                    currententry--;
                }

                if (hs->s16Tree[-currententry - 1] == 0)
                {
                    hs->s16Tree[-currententry - 1] = nextfreeentry;

                    currententry = nextfreeentry;

                    nextfreeentry -= 2;
                }
                else
                {
                    currententry = hs->s16Tree[-currententry - 1];
                }

                code <<= 1;
            }

            if ((code & 0x8000) == 0)
            {
                currententry--;
            }

            hs->s16Tree[-currententry - 1] = i;
        }

        p++;
    }
#endif
    JPEG_FREE(huffsize);
    JPEG_FREE(huffcode);
    return TRUE;
}

//------------------------------------------------------------------------------
// Verifies the quantization tables needed for this scan are available.
MS_BOOL JpegCheckQuantTable(JPD_Ctx_t *pCtx) // ok
{
    MS_U8 i;
    JPD_LOGD("\n");

    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_scan; i++)
    {
        if (pCtx->stHeadInfo.stQuantTables[pCtx->stHeadInfo.u8Comp_quant[pCtx->stHeadInfo.u8Comp_list[i]]].bValid
            == FALSE)
        {
            _JpegTraceError(pCtx, E_JPEG_UNDEFINED_QUANT_TABLE);
            return FALSE;
        }
    }
    return TRUE;
}

/*------------------------------------------------------------------------------
 * Verifies that all the Huffman tables needed for this scan are available.
 */
MS_BOOL JpegCheckHuffTable(JPD_Ctx_t *pCtx)
{
    MS_U8 i;
    JPD_LOGD("\n");

    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_scan; i++)
    {
        if ((pCtx->stHeadInfo.u8Spectral_start == 0)
            && (pCtx->stHeadInfo.stHuff_info[pCtx->stHeadInfo.u8Comp_dc_tab[pCtx->stHeadInfo.u8Comp_list[i]]].bValid
                == FALSE))
        {
            _JpegTraceError(pCtx, E_JPEG_UNDEFINED_HUFF_TABLE);
            return FALSE;
        }

        if ((pCtx->stHeadInfo.u8Spectral_end > 0)
            && (pCtx->stHeadInfo.stHuff_info[pCtx->stHeadInfo.u8Comp_ac_tab[pCtx->stHeadInfo.u8Comp_list[i]]].bValid
                == FALSE))
        {
            _JpegTraceError(pCtx, E_JPEG_UNDEFINED_HUFF_TABLE);
            return FALSE;
        }
    }

    for (i = 0; i < JPEG_MAXHUFFTABLES; i++)
    {
        if (pCtx->stHeadInfo.stHuff_info[i].bValid)
        {
            if (!_JpegMakeHuffTable(pCtx, i))
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

// Read the start of frame (SOF) marker.
MS_BOOL JpegReadSOFMarker(JPD_Ctx_t *pCtx)
{
    MS_U8  i;
    MS_U32 left;
    MS_U16 jpegMaxWidth, jpegMaxHeight;
    MS_U8  c1;

    JPD_LOGD("\n");

    left = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));

    if (_JpegGetChar(pCtx) != 8) /* precision: sorry, only 8-bit precision is supported right now */
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_PRECISION);
        return FALSE;
    }

    pCtx->stHeadInfo.u16Image_y_size = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));
    pCtx->stHeadInfo.u16Image_x_size = (MS_U16)((_JpegGetChar(pCtx) << 8) + _JpegGetChar(pCtx));
    // save the original image size
    pCtx->stHeadInfo.u16OriImg_x_size = pCtx->stHeadInfo.u16Image_x_size;
    pCtx->stHeadInfo.u16OriImg_y_size = pCtx->stHeadInfo.u16Image_y_size;

    JPD_LOGD("Parse jpeg w=%d  h=%d\n", pCtx->stHeadInfo.u16Image_x_size, pCtx->stHeadInfo.u16Image_y_size);

    if ((pCtx->stHeadInfo.bProgressive_flag) && (E_MHAL_JPD_DECODE_MAIN == pCtx->u8DecodeType))
    {
        MS_U32 buffSize;
        // Define MAX picture size for progressive, 8 is the max factor of scale down
        jpegMaxWidth  = pCtx->stResInfo.jpeg_pro_max_width * 8; // jpeg_pro_max_width init by app
        jpegMaxHeight = pCtx->stResInfo.jpeg_pro_max_height * 8;

        // Calculate Internal Buffer size
        // check needed buffer for progressive jpeg decoding, for worst case:
        buffSize = pCtx->stHeadInfo.u16Image_x_size * pCtx->stHeadInfo.u16Image_y_size * 3 * 2
                   + pCtx->stHeadInfo.u16Image_x_size * pCtx->stHeadInfo.u16Image_y_size * 3 * 2 / 64
                   + JPEG_DEFAULT_EXIF_SIZE + 8 * 1024;
        JPD_LOGD("buff_size: %x Inter:%x\n", buffSize, pCtx->stInternalBuf.u32InternalBufSize);
        if (buffSize > pCtx->stInternalBuf.u32InternalBufSize) // internal buffer not enough
        {
            JPD_LOGE("Progressive image size too big... do not handle it\n");
            _JpegTraceError(pCtx, E_JPEG_NOTENOUGHMEM);
            return FALSE;
        }
    }
    else
    {
        // Define MAX picture size for Baseline, 8 is the max factor of scale down
        jpegMaxWidth  = pCtx->stResInfo.jpeg_max_width * 8;
        jpegMaxHeight = pCtx->stResInfo.jpeg_max_height * 8;
        if (pCtx->stResInfo.jpeg_max_width >= 8191)
        {
            jpegMaxWidth = 8191 * 8;
        }

        if (pCtx->stResInfo.jpeg_max_height >= 8191)
        {
            jpegMaxHeight = 8191 * 8;
        }
    }

    if ((pCtx->stHeadInfo.u16Image_y_size < 1) || (pCtx->stHeadInfo.u16Image_y_size > jpegMaxHeight))
    {
        JPD_LOGE("Image_y_size=%d,MaxHeight=%d\n", pCtx->stHeadInfo.u16Image_y_size, jpegMaxHeight);
        _JpegTraceError(pCtx, E_JPEG_BAD_HEIGHT);
        return FALSE;
    }

    if ((pCtx->stHeadInfo.u16Image_x_size < 1) || (pCtx->stHeadInfo.u16Image_x_size > jpegMaxWidth))
    {
        JPD_LOGE("Image_x_size=%d,MaxWidth=%d\n", pCtx->stHeadInfo.u16Image_x_size, jpegMaxWidth);
        _JpegTraceError(pCtx, E_JPEG_BAD_WIDTH);
        return FALSE;
    }

    pCtx->stHeadInfo.u8Comps_in_frame = _JpegGetChar(pCtx);

    if (pCtx->stHeadInfo.u8Comps_in_frame > JPEG_MAXCOMPONENTS)
    {
        _JpegTraceError(pCtx, E_JPEG_TOO_MANY_COMPONENTS);
        return FALSE;
    }

    if (left != (MS_U32)(pCtx->stHeadInfo.u8Comps_in_frame) * 3 + 8)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_SOF_LENGTH);
        return FALSE;
    }

    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_frame; i++)
    {
        pCtx->stHeadInfo.u8Comp_ident[i] = _JpegGetChar(pCtx);
        if (0 == pCtx->stHeadInfo.u8Comp_ident[i]) // The component ID is start from 0 (0 1 2). The normal case is start
                                                   // from 1 (1 2 3) for YUV
        {
            pCtx->stDealInfo.u8LumaCi    = 0;
            pCtx->stDealInfo.u8ChromaCi  = 1;
            pCtx->stDealInfo.u8Chroma2Ci = 2;
            JPD_LOGD("[notice]The component ID is start from 0 (0 1 2)");
        }

        c1 = _JpegGetChar(pCtx);

        pCtx->stHeadInfo.u8Comp_h_samp[i] = (c1 & 0xf0) >> 4;
        pCtx->stHeadInfo.u8Comp_v_samp[i] = (c1 & 0x0f);
        pCtx->stHeadInfo.u8Comp_quant[i]  = _JpegGetChar(pCtx);
        JPD_LOGD("u8Comp_quant%d:0x%x\n", i, pCtx->stHeadInfo.u8Comp_quant[i]);
        // patch from LG driver for the file with the following problem:
        // only has one component, but its sampling factor is 1x2
        // Per the JPEG spec A.2.2 (see the attached file, "regardless of the values of H1 and V1"),
        // please always set H=1 & V=1 to hw, when mono image.
        if (pCtx->stHeadInfo.u8Comps_in_frame == 1) // allen.chang 2011/6/16 patch
        {
            pCtx->stHeadInfo.u8Comp_h_samp[0] = 1;
            pCtx->stHeadInfo.u8Comp_v_samp[0] = 1;
        }
    }
    return TRUE;
}

int JpegCalcBlock(JPD_Ctx_t *pCtx)
{
    if (pCtx->stHeadInfo.u8Comps_in_frame == 1)
    {
        pCtx->stDealInfo.u8Scan_type = E_JPEG_GRAYSCALE;

        pCtx->stDealInfo.u16Max_blocks_per_mcu = 1;
        pCtx->stDealInfo.gu8Max_mcu_x_size     = 8;
        pCtx->stDealInfo.gu8Max_mcu_y_size     = 8;
    }
    else if (pCtx->stHeadInfo.u8Comps_in_frame == 3)
    {
        if (((pCtx->stHeadInfo.u8Comp_h_samp[1] != 1) || (pCtx->stHeadInfo.u8Comp_v_samp[1] != 1))
            || // support only U_H1V1 & V_H1V1
            ((pCtx->stHeadInfo.u8Comp_h_samp[2] != 1) || (pCtx->stHeadInfo.u8Comp_v_samp[2] != 1)))
        {
            _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_SAMP_FACTORS);
            return FALSE;
        }

        if ((pCtx->stHeadInfo.u8Comp_h_samp[0] == 1) && (pCtx->stHeadInfo.u8Comp_v_samp[0] == 1))
        {
            // set RGB based jpeg flag   0x52  0x47 0x42 = RGB
            if (pCtx->stHeadInfo.u8Comp_ident[0] == 82 || pCtx->stHeadInfo.u8Comp_ident[0] == 71
                || pCtx->stHeadInfo.u8Comp_ident[0] == 66)
            {
                _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_SAMP_FACTORS);
                return FALSE;
            }
            JPD_LOGD("%s:Warning, YUV444 can't achieve 4K 30FPS!!!\n", __FUNCTION__);

            pCtx->stDealInfo.u8Scan_type           = E_JPEG_YH1V1; // 4:4:4
            pCtx->stDealInfo.u16Max_blocks_per_mcu = 3;

            pCtx->stDealInfo.gu8Max_mcu_x_size = 8;
            pCtx->stDealInfo.gu8Max_mcu_y_size = 8;
        }
        else if ((pCtx->stHeadInfo.u8Comp_h_samp[0] == 2) && (pCtx->stHeadInfo.u8Comp_v_samp[0] == 1))
        {
            pCtx->stDealInfo.u8Scan_type           = E_JPEG_YH2V1; // 4:2:2
            pCtx->stDealInfo.u16Max_blocks_per_mcu = 4;

            pCtx->stDealInfo.gu8Max_mcu_x_size = 16;
            pCtx->stDealInfo.gu8Max_mcu_y_size = 8;
        }
        else if ((pCtx->stHeadInfo.u8Comp_h_samp[0] == 1) && (pCtx->stHeadInfo.u8Comp_v_samp[0] == 2))
        {
            pCtx->stDealInfo.u8Scan_type           = E_JPEG_YH1V2; // 440
            pCtx->stDealInfo.u16Max_blocks_per_mcu = 4;

            pCtx->stDealInfo.gu8Max_mcu_x_size = 8;
            pCtx->stDealInfo.gu8Max_mcu_y_size = 16;
        }
        else if ((pCtx->stHeadInfo.u8Comp_h_samp[0] == 2) && (pCtx->stHeadInfo.u8Comp_v_samp[0] == 2))
        {
            // this 420
            pCtx->stDealInfo.u8Scan_type           = E_JPEG_YH2V2;
            pCtx->stDealInfo.u16Max_blocks_per_mcu = 6;

            pCtx->stDealInfo.gu8Max_mcu_x_size = 16;
            pCtx->stDealInfo.gu8Max_mcu_y_size = 16;
        }
        else if ((pCtx->stHeadInfo.u8Comp_h_samp[0] == 4) && (pCtx->stHeadInfo.u8Comp_v_samp[0] == 1))
        {
            // 4:1:1
            pCtx->stDealInfo.u8Scan_type = E_JPEG_YH4V1;

            pCtx->stDealInfo.u16Max_blocks_per_mcu = 6;

            pCtx->stDealInfo.gu8Max_mcu_x_size = 32;
            pCtx->stDealInfo.gu8Max_mcu_y_size = 8;
        }
        else
        {
            _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_SAMP_FACTORS);
            return FALSE;
        }
    }
    else
    {
        _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_COLORSPACE);
        return FALSE;
    }

    pCtx->stDealInfo.gu16Max_mcus_per_row =
        (pCtx->stHeadInfo.u16Image_x_size + (pCtx->stDealInfo.gu8Max_mcu_x_size - 1))
        / pCtx->stDealInfo.gu8Max_mcu_x_size;
    pCtx->stDealInfo.u16Max_mcus_per_col = (pCtx->stHeadInfo.u16Image_y_size + (pCtx->stDealInfo.gu8Max_mcu_y_size - 1))
                                           / pCtx->stDealInfo.gu8Max_mcu_y_size;
    pCtx->stDealInfo.u32Max_blocks_per_row =
        pCtx->stDealInfo.gu16Max_mcus_per_row * pCtx->stDealInfo.u16Max_blocks_per_mcu;

    // Should never happen
    if (pCtx->stDealInfo.u32Max_blocks_per_row > JPEG_MAXBLOCKSPERROW)
    {
        _JpegTraceError(pCtx, E_JPEG_ASSERTION_ERROR);
        return FALSE;
    }

#if SUPPORT_PROGRESSIVE_MODE
    pCtx->u16Total_lines_left = pCtx->stDealInfo.u16Max_mcus_per_col * pCtx->stDealInfo.gu8Max_mcu_y_size;
#endif
    return TRUE;
}

static int _JpegCheckTable(JPD_Ctx_t *pCtx)
{
    if (pCtx->stDealInfo.HeadCheck.DHT == 0)
    {
        _JpegReadDefaultDht(pCtx);
        pCtx->stDealInfo.HeadCheck.DHT = TRUE;
    }

    if (pCtx->stDealInfo.HeadCheck.DHT)
    {
        if (!JpegCheckHuffTable(pCtx))
        {
            return FALSE;
        }
    }

    if (pCtx->stDealInfo.HeadCheck.DQT)
    {
        if (!JpegCheckQuantTable(pCtx))
        {
            return FALSE;
        }
    }

#if SUPPORT_PROGRESSIVE_MODE
    JPEG_memset((void *)pCtx->stProgressiveInfo.u32Last_dc_val, 0, pCtx->stHeadInfo.u8Comps_in_frame * sizeof(MS_U32));
    pCtx->stProgressiveInfo.u32EOB_run = 0;
#endif

    if (pCtx->stHeadInfo.u16Restart_interval)
    {
        pCtx->stDealInfo.u16Restarts_left    = pCtx->stHeadInfo.u16Restart_interval;
        pCtx->stDealInfo.u16Next_restart_num = 0;
    }

    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        // pre-fill bit buffer for later decoding
        pCtx->stHeadInfo.s16Bits_left = 16;
        _JpegGetBits2(pCtx, 16);
        _JpegGetBits2(pCtx, 16);
    }

    JPD_LOGD("JPEG_init_scan:ECS 0x%08lx\n", JpegGetECS(pCtx));

    return TRUE;
}

JPEG_STATIC void _JpegGetAlignedResolution(JPD_Ctx_t *pCtx, MS_U16 *width, MS_U16 *height)
{
    MS_U8 mcuWidth, mcuHeight;
    MS_U8 yVsf = pCtx->stHeadInfo.u8Comp_v_samp[0];
    MS_U8 yHsf = pCtx->stHeadInfo.u8Comp_h_samp[0];

    *width   = pCtx->stHeadInfo.u16OriImg_x_size;
    *height  = pCtx->stHeadInfo.u16OriImg_y_size;
    mcuWidth = pCtx->stHeadInfo.u16OriImg_x_size % (yHsf * 8);

    if (mcuWidth)
    {
        *width += (yHsf * 8 - mcuWidth);
    }

    mcuHeight = pCtx->stHeadInfo.u16OriImg_y_size % (yVsf * 8);

    if (mcuHeight)
    {
        *height += (yVsf * 8 - mcuHeight);
    }

    JPD_LOGD("Align size: %d*%d\n", *width, *height);
}

int JpegGetAlignedImageInfo(JPD_Ctx_t *pCtx)
{
    MS_U16 picWidth, picHeight;
    _JpegGetAlignedResolution(pCtx, &picWidth, &picHeight);
    pCtx->stDealInfo.u16AlignedImgPitch   = picWidth;
    pCtx->stDealInfo.u16AlignedImgPitch_H = picHeight;
    pCtx->stDealInfo.u16AlignedImgWidth   = pCtx->stHeadInfo.u16Image_x_size;
    pCtx->stDealInfo.u16AlignedImgHeight  = pCtx->stHeadInfo.u16Image_y_size;

    JPD_LOGD("\n");
    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        pCtx->stResInfo.jpeg_max_width_hd  = pCtx->stResInfo.jpeg_pro_max_width;
        pCtx->stResInfo.jpeg_max_height_hd = pCtx->stResInfo.jpeg_pro_max_height;
    }
    else
    {
        pCtx->stResInfo.jpeg_max_width_hd  = pCtx->stResInfo.jpeg_max_width;
        pCtx->stResInfo.jpeg_max_height_hd = pCtx->stResInfo.jpeg_max_height;
    }

    pCtx->stHeadInfo.u16Image_x_size = picWidth;
    pCtx->stHeadInfo.u16Image_y_size = picHeight;
    return TRUE;
}

MS_S32 JpegDealHeaderInfo(JPD_Ctx_t *pCtx)
{
    JpegGetAlignedImageInfo(pCtx);
    if (JpegCalcBlock(pCtx) == FALSE)
    {
        _JpegTraceError(pCtx, E_JPEG_NO_ARITHMETIC_SUPPORT);
        return FALSE;
    }

    _JpegCalcMcuBlockOrder(pCtx); // init frame and order
    _JpegCheckTable(pCtx);

    return TRUE;
}

MS_S32 JpegInitParseAddr(JPD_Ctx_t *pCtx)
{
    JPD_CHECK_VALID_POINTER(pCtx->stInBufInfo.pInputBufVA1);
    JPD_CHECK_VALID_POINTER(pCtx->stInBufInfo.pInputBufVA2);

    if (0 >= (pCtx->stInBufInfo.u32InputBufSize1 + pCtx->stInBufInfo.u32InputBufSize2))
    {
        JPD_LOGE("JpegInitParseAddr: input len error.\n");
        return -1;
    }

    JPD_LOGD("Jpeg buffer size %d\n", pCtx->stInBufInfo.u32InputBufSize1 + pCtx->stInBufInfo.u32InputBufSize2);

    pCtx->pu8In_buf_offset1    = pCtx->stInBufInfo.pInputBufVA1; // for parse header
    pCtx->pu8In_buf_offset2    = pCtx->stInBufInfo.pInputBufVA2;
    pCtx->u32In_buf1_bytes     = pCtx->stInBufInfo.u32InputBufSize1;
    pCtx->u32In_buf2_bytes     = pCtx->stInBufInfo.u32InputBufSize2;
    pCtx->u32In_buf_left_bytes = pCtx->stInBufInfo.u32InputBufSize1 + pCtx->stInBufInfo.u32InputBufSize2;
    JPD_LOGD("@1,  pCtx->pu8In_buf_offset1=0x%p,  pCtx->u32In_buf_left_bytes=0x%x,========\n", pCtx->pu8In_buf_offset1,
             pCtx->u32In_buf_left_bytes);

    if (pCtx->bEOF_flag && (FALSE == pCtx->stThumbnailInfo.bThumbnailAccessMode))
    {
        JPD_LOGD("JPEG_fill_read_buffer : EOF!!!\n");
        return TRUE;
    }

    if (pCtx->stThumbnailInfo.bThumbnailAccessMode)
    {
        JPD_LOGD("Fill Thumbnail, buffer left = %d, buffer offset = %d\n", pCtx->stThumbnailInfo.u16ThumbnailBufferSize,
                 pCtx->stThumbnailInfo.u32ThumbnailBufferOffset);
        if (pCtx->stThumbnailInfo.u16ThumbnailBufferSize >= pCtx->stInBufInfo.u32InputBufSize1)
        {
            JPEG_memcpy((void *)((pCtx->pu8In_buf_offset1 + pCtx->u32In_buf_left_bytes)),
                        ((void *)((unsigned long)pCtx->stInternalBuf.pInternalBufVA)
                         + pCtx->stThumbnailInfo.u32ThumbnailBufferOffset),
                        pCtx->stInBufInfo.u32InputBufSize1);
            pCtx->u32In_buf_left_bytes = pCtx->stInBufInfo.u32InputBufSize1;
            pCtx->stThumbnailInfo.u32ThumbnailBufferOffset += pCtx->stInBufInfo.u32InputBufSize1;
            pCtx->stThumbnailInfo.u16ThumbnailBufferSize -= pCtx->stInBufInfo.u32InputBufSize1;
        }
        else
        {
            JPEG_memcpy((void *)(pCtx->pu8In_buf_offset1 + pCtx->u32In_buf_left_bytes),
                        (void *)((unsigned long)((unsigned long)(pCtx->stInternalBuf.pInternalBufVA)
                                                 + pCtx->stThumbnailInfo.u32ThumbnailBufferOffset)),
                        pCtx->stThumbnailInfo.u16ThumbnailBufferSize);

            pCtx->bEOF_flag            = TRUE;
            pCtx->u32In_buf_left_bytes = pCtx->stThumbnailInfo.u16ThumbnailBufferSize;
            pCtx->stThumbnailInfo.u32ThumbnailBufferOffset += pCtx->stThumbnailInfo.u16ThumbnailBufferSize;
            pCtx->stThumbnailInfo.u16ThumbnailBufferSize = 0;
        }
    }

    return 0;
}
MS_BOOL JpegLocateSOFMarker(JPD_Ctx_t *pCtx)
{
    MS_U32 c;
    JPD_LOGD("buf_offset %p\n", pCtx->pu8In_buf_offset1);
    JPD_LOGD("%02x %02x %02x %02x %02x %02x %02x %02x \n", *pCtx->pu8In_buf_offset1, *(pCtx->pu8In_buf_offset1 + 1),
             *(pCtx->pu8In_buf_offset1 + 2), *(pCtx->pu8In_buf_offset1 + 3), *(pCtx->pu8In_buf_offset1 + 4),
             *(pCtx->pu8In_buf_offset1 + 5), *(pCtx->pu8In_buf_offset1 + 6), *(pCtx->pu8In_buf_offset1 + 7));

    if (!_JpegLocateSOIMarker(pCtx)) // find Start of image(SOI) flag
    {
        _JpegTraceError(pCtx, E_JPEG_NOT_JPEG);
        return FALSE;
    }
    if (pCtx->pDev->EOI == TRUE)
    {
        if (_JpegLocateEOIMarker(pCtx, pCtx->stInBufInfo.u32InputBufSize1 - 4) == FALSE) // find End of image(EOI)
        {
            return FALSE;
        }
    }

    c = _JpegProcessMarkers(pCtx); // find next flag until find SOF

    if (c == FALSE)
    {
        return FALSE;
    }

    switch (c)
    {
        case E_JPEG_SOF2:
        {
            JPD_LOGD("Progressive\n");
            pCtx->stHeadInfo.bProgressive_flag = TRUE;
#if !defined(JPD_UT_TEST)
            JPD_LOGE("ERROR! Progressive mode Need Verify More, Maybe there are Case not Handled\n");
            return FALSE;
#endif

            pCtx->stDealInfo.u32SOFOffset = pCtx->stDealInfo.u32DataOffset + JpegGetECS(pCtx) - 2;
            if (!JpegReadSOFMarker(pCtx))
            {
                return FALSE;
            }
            break;
        }
        case E_JPEG_SOF0:
            /* baseline DCT */
        case E_JPEG_SOF1:
            /* extended sequential DCT */
            {
                JPD_LOGD("Baseline\n");
                JPD_LOGD("u32DataOffset = %d\n", pCtx->stDealInfo.u32DataOffset);
                pCtx->stDealInfo.u32SOFOffset = pCtx->stDealInfo.u32DataOffset + JpegGetECS(pCtx) - 2;
                if (!JpegReadSOFMarker(pCtx)) // get pic w h
                {
                    return FALSE;
                }
                break;
            }
        case E_JPEG_SOF9:
            /* Arithmitic coding */
            {
                _JpegTraceError(pCtx, E_JPEG_NO_ARITHMETIC_SUPPORT);
                return FALSE;
                break;
            }

        default:
        {
            JPD_LOGE("parsed unsupported marker = 0x%04x\n", c);
            _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_MARKER);
            return FALSE;
            break;
        }
    }
    return TRUE;
}
MS_BOOL JpegLocateSOSMarker(JPD_Ctx_t *pCtx)
{
    MS_U32 c;
    JPD_LOGD("\n");

    c = _JpegProcessMarkers(pCtx);

    if (c == E_JPEG_EOI)
    {
        return FALSE;
    }

    if (c != E_JPEG_SOS) //==  satrt_of_scan
    {
        _JpegTraceError(pCtx, E_JPEG_UNEXPECTED_MARKER);
        return FALSE;
    }

    if (!_JpegReadSOSMarker(pCtx)) // FF DA  SOS  msg
    {
        return FALSE;
    }

    JPD_LOGD("==pCtx->u32In_buf_left_bytes=0x%x,pCtx->u32In_buf_left_bytes=0x%x==\n", pCtx->u32In_buf_left_bytes,
             pCtx->u32In_buf_left_bytes);
    return TRUE;
}

/* Get current access byte address in MRC buffer relative to MRC start address */
unsigned long JpegGetECS(JPD_Ctx_t *pCtx)
{
    unsigned long dataEndAddr   = (unsigned long)pCtx->pu8In_buf_offset1;
    unsigned long dataStartAddr = (unsigned long)pCtx->stInBufInfo.pInputBufVA1; // phyBufAddr1 pInputBufVA1

    // MS_VIRT data_start_addr =(MS_VIRT) pCtx->stInBufInfo.phyBufAddr1;
    // need to consider VA2
    JPD_LOGD("%s %d  data_end_addr =%lx data_start_addr=%lx\n", __FUNCTION__, __LINE__, dataEndAddr, dataStartAddr);
    return (dataEndAddr - dataStartAddr);
}

//************************Parse EXIF header***************************
//#define charingExiftest

#ifndef charingExiftest

JPEG_STATIC MS_BOOL _JpegDecodeExifInfo(MS_U8 *data, MS_U32 data_length, JPD_Ctx_t *pCtx)
{
    UNUSED(data);
    UNUSED(data_length);
    UNUSED(pCtx);

    return 0;
}

#else

JPEG_STATIC MS_U16 JpegGetU16(MS_U8 *data)
{
    MS_S8 i;
    MS_U16 val = 0;

    for (i = 1; i >= 0; i--)
        val = (val << 8) + (MS_U8) * (data + i);

    return val;
}
JPEG_STATIC MS_U32 JpegGetU32(MS_U8 *data)
{
    MS_S8 i;
    MS_U32 val = 0;

    for (i = 3; i >= 0; i--)
        val = (val << 8) + (MS_U8) * (data + i);

    return val;
}
//------------------------------------------------------------------------------
JPEG_STATIC MS_U32 JpegTiffEndianChangeL(JPD_Ctx_t *pCtx, MS_U32 u32Val)
{
    if (pCtx->stThumbnailInfo.bTiffBigEndian)
        return EndianChangeL(u32Val);
    else
        return u32Val;
}
//------------------------------------------------------------------------------
JPEG_STATIC MS_U16 JpegTiffEndianChangeS(JPD_Ctx_t *pCtx, MS_U16 u16Val)
{
    if (pCtx->stThumbnailInfo.bTiffBigEndian)
    {
        return EndianChangeS(u16Val);
    }
    else
    {
        return u16Val;
    }
}

JPEG_STATIC MS_BOOL _JpegDecodeExifInfo(MS_U8 *data, MS_U32 data_length, JPD_Ctx_t *pCtx)
{
    MS_U8 *pJpegBuff = data;
    //    MS_U32 u32tmp, u32Len;
    MS_U32 u32tmp;
    MS_U16 u16Marker, u16Len, u16NumOfEntry, i;
    MS_U8 *pTiffHdrPtr, *pNextIfd;
    MS_U32 u32DataValue;
    MS_U8 *pExifHdr = NULL, *pExifHdrOffet = NULL;
    MS_BOOL bIFD_pt_found = FALSE;

    u32tmp = JpegGetU32(pJpegBuff);

    if (JPEG_TAG_EXIF != u32tmp)
    {
        return FALSE;
    }

    // Exif starts here
    pJpegBuff += 6;
    pExifHdr = pJpegBuff;
    u16Marker = JpegGetU16(pJpegBuff);
    JPD_LOGD("pJpegBuff = %x,u16Marker = %d\n", pJpegBuff,
             u16Marker); // pJpegBuff = f0dcd006,u16Marker = 18761(0x4949)
    if (JPEG_TIFF_BIG_ENDIAN == u16Marker)
    {
        pCtx->stThumbnailInfo.bTiffBigEndian = TRUE;
    }
    else if (JPEG_TIFF_LITTLE_ENDIAN == u16Marker)
    {
        pCtx->stThumbnailInfo.bTiffBigEndian = FALSE; //
    }
    else
    {
        return FALSE;
    }

    pCtx->stThumbnailInfo.u32ThumbnailOffset += 6;
    pTiffHdrPtr = pJpegBuff;

    pJpegBuff += 2;
    u16Marker = JpegTiffEndianChangeS(pCtx, JpegGetU16(pJpegBuff));
    JPD_LOGD("u16Marker = %d\n", u16Marker); // 0x42
    if (u16Marker != 0x002A)
    {
        return FALSE;
    }

    pJpegBuff += 2;
    u16Len = (MS_U16)JpegTiffEndianChangeL(pCtx, JpegGetU32(pJpegBuff));
    pJpegBuff += (u16Len - 4); // ENDIAN(2byte) + 0x002A(2byte)

    // 0th IFD start here
    u16NumOfEntry = JpegTiffEndianChangeS(pCtx, JpegGetU16(pJpegBuff));
    pJpegBuff += 2;

    JPD_LOGD("pJpegBuff = %x,phyInternalBufAddr=%x,u16NumOfEntry*12=%x\n", (unsigned long)pJpegBuff,
             pCtx->stInternalBuf.phyInternalBufAddr, u16NumOfEntry * 12);

    JPD_LOGD("data_length =%x , phyInternalBufAddr =%x\n", data_length, pCtx->stInternalBuf.phyInternalBufAddr);
    /*
        if((((MS_VIRT) pJpegBuff) - pCtx->stInternalBuf.phyInternalBufAddr + u16NumOfEntry*12)>=data_length)
        {
            JPD_LOGE("Boundary check fail\n");
            return FALSE;
        }
    */

    // Boundary check, prevent from buffer over-run
    if ((((unsigned long)pJpegBuff) - ((MS_U32)pCtx->stInternalBuf.pInternalBufVA) + u16NumOfEntry * 12) >= data_length)
    {
        JPD_LOGE("Boundary check fail\n");
        return FALSE;
    }

    //    pJpegBuff += 12*u16NumOfEntry;

    u16NumOfEntry = MIN(u16NumOfEntry, MAX_NUM_OF_INTEROPERABILITY);
    JPD_LOGD("IFD0 : u16NumOfEntry = %d\n", u16NumOfEntry);

    // Retrieve orientation
    for (i = 0; i < u16NumOfEntry; i++)
    {
        u16Marker = JpegTiffEndianChangeS(pCtx, JpegGetU16(pJpegBuff));
        pJpegBuff += 8;

        if ((JPEG_EXIF_TAG_ORIENTATION == u16Marker)
            && (E_JPEG_EXIF_ORIENT_NOT_FOUND == pCtx->stExifInfo.eEXIF_Orientation))
        {
            // The datatype of orientation is short
            u32DataValue = (MS_U32)JpegTiffEndianChangeS(pCtx, JpegGetU16(pJpegBuff));
            if ((E_JPEG_EXIF_ORIENT_ROTATE_0 > u32DataValue) || (E_JPEG_EXIF_ORIENT_ROTATE_270 < u32DataValue))
            {
                pCtx->stExifInfo.eEXIF_Orientation = E_JPEG_EXIF_ORIENT_RESERVED;
            }
            else
            {
                pCtx->stExifInfo.eEXIF_Orientation = (JPEG_EXIF_Orientation)u32DataValue;
            }
            JPD_LOGD("JPEG EXIF Orientation = %d\n", pCtx->stExifInfo.eEXIF_Orientation);
        }
        else if (JPEG_EXIF_TAG_IFD_POINTER == u16Marker)
        {
            bIFD_pt_found = TRUE;
            // datavalue is an offset from exif marker for datetime
            u32DataValue = JpegTiffEndianChangeL(pCtx, JpegGetU32(pJpegBuff));
            pExifHdrOffet = pExifHdr + u32DataValue;
            JPD_CHECK_OVER_BUFFER(pExifHdrOffet, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                  pCtx->stInternalBuf.u32InternalBufSize);
            JPD_LOGD("Exif IFD pointer = 0x%lx\n", pExifHdrOffet);
        }
#if SUPPORT_EXIF_EXTRA_INFO
        else if (JPEG_EXIF_TAG_MANUFACTURER == u16Marker)
        {
            MS_U8 u8i = 0;
            MS_U8 *pIFDPtrOffset;
            pIFDPtrOffset = pJpegBuff;
            u32DataValue = JpegTiffEndianChangeL(pCtx, JpegGetU32(pIFDPtrOffset));
            JPD_LOGD("MANUFACTURER Offset = 0x%lx\n", u32DataValue);
            pIFDPtrOffset = pExifHdr + u32DataValue;
            if (JPD_CHECK_OVER_BUFFER((unsigned long)pIFDPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                      pCtx->stInternalBuf.u32InternalBufSize))
            {
                pJpegBuff += 4;
                continue;
            }
            // JPD_LOGD("MANUFACTURER addr = 0x%lx\n", (MS_U32)pIFDPtrOffset);
            while (*(pIFDPtrOffset + u8i) && u8i < JPEG_MANUFACTURER_SIZE)
            {
                pCtx->stExifInfo.u8EXIF_Manufacturer[u8i] = *(pIFDPtrOffset + u8i);
                u8i++;
            }
        }
        else if (JPEG_EXIF_TAG_MODEL == u16Marker)
        {
            MS_U8 u8i = 0;
            MS_U8 *pIFDPtrOffset;
            pIFDPtrOffset = pJpegBuff;
            u32DataValue = JpegTiffEndianChangeL(pCtx, JpegGetU32(pIFDPtrOffset));
            JPD_LOGD("MODEL Offset = 0x%lx\n", u32DataValue);
            pIFDPtrOffset = pExifHdr + u32DataValue;
            if (JPD_CHECK_OVER_BUFFER((unsigned long)pIFDPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                      pCtx->stInternalBuf.u32InternalBufSize))
            {
                pJpegBuff += 4;
                continue;
            }
            // JPD_LOGD("MODEL addr = 0x%lx\n", (MS_U32)pIFDPtrOffset);
            while (*(pIFDPtrOffset + u8i) && u8i < JPEG_MODEL_SIZE)
            {
                pCtx->stExifInfo.u8EXIF_Model[u8i] = *(pIFDPtrOffset + u8i);
                u8i++;
            }
        }
#endif
#if 0
        else if((JPEG_EXIF_TAG_DATETIME_MOD == u16Marker)
        && (FALSE == pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].bHasDataTime))
        {
            pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].bHasDataTime = TRUE;
            //datavalue is an offset from exif marker for datetime
            u32DataValue = JpegTiffEndianChangeL(JpegGetU32(pJpegBuff));
            pNJPEGContext->pExifOffset = pExifHdr + u32DataValue;
            mybug("pExifOffset1 = 0x%lx\n", (MS_U32)pNJPEGContext->pExifOffset);
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Year);
            pNJPEGContext->pExifOffset++;
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Month);
            pNJPEGContext->pExifOffset++;
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Day);
            pNJPEGContext->pExifOffset++;
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Hour);
            pNJPEGContext->pExifOffset++;
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Minute);
            pNJPEGContext->pExifOffset++;
            JPEG_AtoU32(pNJPEGContext->pExifOffset, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Second);
            JPD_LOGD("JPEG EXIF Orientation = %d\n", pNJPEGContext->_eEXIF_Orientation[pNJPEGContext->eNJPDNum]);
            JPD_LOGD("=======JPEG EXIF DateTime======= Exist = %d, Original = %d \n", pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].bHasDataTime, pNJPEGContext->bOriDateTimeFound);
            JPD_LOGD("Year = %ld, Month = %ld, Day = %ld\n", pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Year, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Month, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Day);
            JPD_LOGD("Hour = %ld, Minute = %ld, Second = %ld\n", pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Hour, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Minute, pNJPEGContext->_stEXIF_DateTime[pNJPEGContext->eNJPDNum].u32Second);
        }
#endif
        pJpegBuff += 4;
    }

    if (TRUE == bIFD_pt_found)
    {
        u16NumOfEntry = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifHdrOffet));
        pExifHdrOffet += 2;
        u16NumOfEntry = MIN(u16NumOfEntry, MAX_NUM_OF_INTEROPERABILITY);
        JPD_LOGD("EXIF IFD : u16NumOfEntry = %d\n", u16NumOfEntry);

        for (i = 0; i < u16NumOfEntry; i++)
        {
            MS_U8 *pExifPtrOffset;
            MS_U16 u16Type;
            MS_U32 u32Count;
            u16Marker = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifHdrOffet));
#if (SUPPORT_EXIF_EXTRA_INFO == FALSE)
            if (JPEG_EXIF_TAG_DATETIME_ORI == u16Marker)
            {
                pExifHdrOffet += 8;
                pCtx->stExifInfo.stEXIF_DateTime.bHasDataTime = TRUE;

                // datavalue is an offset from exif marker for datetime
                u32DataValue = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifHdrOffet));
                ;
                pExifHdrOffet = pExifHdr + u32DataValue;
                JPD_CHECK_OVER_BUFFER(pExifHdrOffet, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                      pCtx->stInternalBuf.u32InternalBufSize);
                JPD_LOGD("pExifOriDateTime Offset = 0x%x\n", pExifHdrOffet);
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Year);
                pExifHdrOffet++;
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Month);
                pExifHdrOffet++;
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Day);
                pExifHdrOffet++;
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Hour);
                pExifHdrOffet++;
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Minute);
                pExifHdrOffet++;
                JPEG_AtoU32(pExifHdrOffet, pCtx->stExifInfo.stEXIF_DateTime.u32Second);
                JPD_LOGD("=======JPEG EXIF DateTime=======\n");
                JPD_LOGD("Year = %d, Month = %d, Day = %d\n", pCtx->stExifInfo.stEXIF_DateTime.u32Year,
                         pCtx->stExifInfo.stEXIF_DateTime.u32Month, pCtx->stExifInfo.stEXIF_DateTime.u32Day);
                JPD_LOGD("Hour = %d, Minute = %d, Second = %d\n", pCtx->stExifInfo.stEXIF_DateTime.u32Hour,
                         pCtx->stExifInfo.stEXIF_DateTime.u32Minute, pCtx->stExifInfo.stEXIF_DateTime.u32Second);
                break;
            }
#else
            switch (u16Marker)
            {
                case JPEG_EXIF_TAG_DATETIME_ORI:
                    pExifPtrOffset                                = pExifHdrOffet + 8;
                    pCtx->stExifInfo.stEXIF_DateTime.bHasDataTime = TRUE;
                    // datavalue is an offset from exif marker for datetime
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    JPD_LOGD("pExifOriDateTime Offset = 0x%x\n", pExifPtrOffset);
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Year);
                    pExifPtrOffset++;
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Month);
                    pExifPtrOffset++;
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Day);
                    pExifPtrOffset++;
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Hour);
                    pExifPtrOffset++;
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Minute);
                    pExifPtrOffset++;
                    JPEG_AtoU32(pExifPtrOffset, pCtx->stExifInfo.stEXIF_DateTime.u32Second);
                    JPD_LOGD("=======JPEG EXIF DateTime=======\n");
                    JPD_LOGD("Year = %d, Month = %d, Day = %d\n", pCtx->stExifInfo.stEXIF_DateTime.u32Year,
                             pCtx->stExifInfo.stEXIF_DateTime.u32Month, pCtx->stExifInfo.stEXIF_DateTime.u32Day);
                    JPD_LOGD("Hour = %d, Minute = %d, Second = %d\n", pCtx->stExifInfo.stEXIF_DateTime.u32Hour,
                             pCtx->stExifInfo.stEXIF_DateTime.u32Minute, pCtx->stExifInfo.stEXIF_DateTime.u32Second);
                    break;
                case JPEG_EXIF_TAG_ISO_SPEED_RATING:
                    pExifPtrOffset = pExifHdrOffet + 2;
                    u16Type        = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    pExifPtrOffset = pExifHdrOffet + 4;
                    u32Count       = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset = pExifHdrOffet + 8;
                    if (u16Type == 3)
                    {
                        if (u32Count == 2)
                        {
                            pCtx->stExifInfo.u32EXIF_ISOSpeedRatings =
                                JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                        }
                        else if (u32Count == 1)
                        {
                            pCtx->stExifInfo.u32EXIF_ISOSpeedRatings =
                                JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                        }
                    }
                    else if (u16Type == 4)
                    {
                        if (u32Count == 1)
                        {
                            pCtx->stExifInfo.u32EXIF_ISOSpeedRatings =
                                JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                        }
                    }
                    JPD_LOGD("ISO Speed Rating=%d\n", pCtx->stExifInfo.u32EXIF_ISOSpeedRatings);
                    break;
                case JPEG_EXIF_TAG_EXPOSURE_TIME:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ExposureTime Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    // JPD_LOGD("ExposureTime addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    pCtx->stExifInfo.stEXIF_ExposureTime.numerator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_ExposureTime.denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ExposureTime numerator=%d, denominator=%d\n",
                             pCtx->stExifInfo.stEXIF_ExposureTime.numerator,
                             pCtx->stExifInfo.stEXIF_ExposureTime.denominator);
                    break;
                case JPEG_EXIF_TAG_F_NUMBER:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("FNumber Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    // JPD_LOGD("FNumber addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    pCtx->stExifInfo.stEXIF_FNumber.numerator = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_FNumber.denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("FNumber numerator=%d, denominator=%d\n", pCtx->stExifInfo.stEXIF_FNumber.numerator,
                             pCtx->stExifInfo.stEXIF_FNumber.denominator);
                    break;

                case JPEG_EXIF_TAG_SHUTTER_SPEED:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ShutterSpeedValue Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    // JPD_LOGD("ShutterSpeedValue addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    pCtx->stExifInfo.stEXIF_ShutterSpeedValue.s_numerator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_ShutterSpeedValue.s_denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ShutterSpeedValue numerator=%d, denominator=%d\n",
                             pCtx->stExifInfo.stEXIF_ShutterSpeedValue.s_numerator,
                             pCtx->stExifInfo.stEXIF_ShutterSpeedValue.s_denominator);
                    break;
                case JPEG_EXIF_TAG_APERTURE:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ApertureValue Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    // JPD_LOGD("ApertureValue addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    pCtx->stExifInfo.stEXIF_ApertureValue.numerator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_ApertureValue.denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ApertureValue numerator=%d, denominator=%d\n",
                             pCtx->stExifInfo.stEXIF_ApertureValue.numerator,
                             pCtx->stExifInfo.stEXIF_ApertureValue.denominator);
                    break;
                case JPEG_EXIF_TAG_EXPOSURE_BIAS:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ExposureBiasValue Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    // JPD_LOGD("ExposureBiasValue addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    pCtx->stExifInfo.stEXIF_ExposureBiasValue.s_numerator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_ExposureBiasValue.s_denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("ExposureBiasValue numerator=%d, denominator=%d\n",
                             pCtx->stExifInfo.stEXIF_ExposureBiasValue.s_numerator,
                             pCtx->stExifInfo.stEXIF_ExposureBiasValue.s_denominator);
                    break;
                case JPEG_EXIF_TAG_FLASH:
                    pExifPtrOffset                 = pExifHdrOffet + 8;
                    pCtx->stExifInfo.u16EXIF_Flash = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    JPD_LOGD("EXIF_Flash=0x%x\n", pCtx->stExifInfo.u16EXIF_Flash);
                    break;
                case JPEG_EXIF_TAG_FOCAL_LENGTH:
                    pExifPtrOffset = pExifHdrOffet + 8;
                    u32DataValue   = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("FocalLength Offset = 0x%lx\n", u32DataValue);
                    pExifPtrOffset = pExifHdr + u32DataValue;
                    JPD_CHECK_OVER_BUFFER(pExifPtrOffset, ((MS_U32)pCtx->stInternalBuf.pInternalBufVA),
                                          pCtx->stInternalBuf.u32InternalBufSize);
                    // JPD_LOGD("FocalLength addr = 0x%lx\n", (MS_U32)pExifPtrOffset);
                    pCtx->stExifInfo.stEXIF_FocalLength.numerator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    pExifPtrOffset += 4;
                    pCtx->stExifInfo.stEXIF_FocalLength.denominator =
                        JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    JPD_LOGD("FocalLength numerator=%d, denominator=%d\n",
                             pCtx->stExifInfo.stEXIF_FocalLength.numerator,
                             pCtx->stExifInfo.stEXIF_FocalLength.denominator);
                    break;
                case JPEG_EXIF_TAG_IMAGE_WIDTH:
                    pExifPtrOffset = pExifHdrOffet + 2;
                    u16Type        = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    pExifPtrOffset = pExifHdrOffet + 8;
                    if (u16Type == 3)
                    {
                        pCtx->stExifInfo.u32EXIF_ImageWidth =
                            (MS_U32)JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    }
                    else
                    {
                        pCtx->stExifInfo.u32EXIF_ImageWidth = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    }
                    JPD_LOGD("EXIF_ImageWidth=%d\n", pCtx->stExifInfo.u32EXIF_ImageWidth);
                    break;
                case JPEG_EXIF_TAG_IMAGE_HEIGHT:
                    pExifPtrOffset = pExifHdrOffet + 2;
                    u16Type        = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    pExifPtrOffset = pExifHdrOffet + 8;
                    if (u16Type == 3)
                    {
                        pCtx->stExifInfo.u32EXIF_ImageHeight =
                            (MS_U32)JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    }
                    else
                    {
                        pCtx->stExifInfo.u32EXIF_ImageHeight = JpegTiffEndianChangeL(pCtx, JpegGetU32(pExifPtrOffset));
                    }
                    JPD_LOGD("EXIF_ImageHeight=%d\n", pCtx->stExifInfo.u32EXIF_ImageHeight);
                    break;
                case JPEG_EXIF_TAG_EXPOSURE_PROGRAM:
                    pExifPtrOffset                           = pExifHdrOffet + 8;
                    pCtx->stExifInfo.u16EXIF_Exposureprogram = JpegTiffEndianChangeS(pCtx, JpegGetU16(pExifPtrOffset));
                    JPD_LOGD("EXIF_Exposureprogram=0x%x\n", pCtx->stExifInfo.u16EXIF_Exposureprogram);
                    break;
                default:
                    break;
            }
#endif
            pExifHdrOffet += 12;
        }
    }

    if (E_JPEG_TYPE_THUMBNAIL != pCtx->u8DecodeType)
    {
        JPD_LOGE("decode type is not thumbnail...\n");
        return FALSE;
    }

    // 1th IFD
    u32tmp = JpegTiffEndianChangeL(pCtx, JpegGetU32(pJpegBuff));
    if (u32tmp == 0)
    {
        return FALSE;
    }
    else
    {
        if (u32tmp >= data_length)
        {
            JPD_LOGE("u32tmp = %d is invalid > %d\n", u32tmp, data_length);
            return FALSE;
        }
        pNextIfd = &pTiffHdrPtr[u32tmp];
        JPD_LOGD("1st IFD pNextIfd = 0x%x, u32tmp = %d\n", pNextIfd, u32tmp);
    }

    u16NumOfEntry = JpegTiffEndianChangeS(pCtx, JpegGetU16(pNextIfd));
    pNextIfd += 2;

    // Boundary check, prevent from buffer over-run
    if ((((unsigned long)pNextIfd) - ((MS_U32)pCtx->stInternalBuf.pInternalBufVA) + u16NumOfEntry * 12) >= data_length)
    {
        return FALSE;
    }

    u16NumOfEntry = MIN(u16NumOfEntry, MAX_NUM_OF_INTEROPERABILITY);

    for (i = 0; i < u16NumOfEntry; i++)
    {
        u16Marker = JpegTiffEndianChangeS(pCtx, JpegGetU16(pNextIfd));
        //        u32Len = JpegTiffEndianChangeL(JpegGetU32(pNextIfd + 4));
        u32tmp = JpegTiffEndianChangeL(pCtx, JpegGetU32(pNextIfd + 8));

        switch (u16Marker)
        {
            case JPEG_TIFF_JPEG_IMG_BYTES:
                pCtx->stThumbnailInfo.u16ThumbnailSize = u32tmp;
                break;
            case JPEG_TIFF_SOI_OFFSET:
                JPD_LOGD("Thumbnail marker found in 0x%x!!\n", pNextIfd);
                pCtx->stThumbnailInfo.bThumbnailFound = TRUE;
                pCtx->stThumbnailInfo.u32ThumbnailOffset += u32tmp;
                break;
            default:
                break;
        }

        pNextIfd += 12;
    }

    // Boundary check, prevent from buffer over-run
    if (pCtx->stThumbnailInfo.bThumbnailFound)
    {
        if ((pCtx->stThumbnailInfo.u32ThumbnailOffset + pCtx->stThumbnailInfo.u16ThumbnailSize) > data_length)
        {
            pCtx->stThumbnailInfo.bThumbnailFound = FALSE;
        }

        // means it only contains SOI header..
        if (pCtx->stThumbnailInfo.u16ThumbnailSize <= 2)
        {
            pCtx->stThumbnailInfo.bThumbnailFound = FALSE;
        }
    }

    return pCtx->stThumbnailInfo.bThumbnailFound;
}

#endif

void JpegFreeAllBlocks(JPD_Ctx_t *pCtx)
{
    MS_U8 i;
    for (i = 0; i < JPEG_MAXBLOCKS; i++)
    {
        if (pCtx->stHeadInfo.pBlocks[i] != NULL)
        {
            DrvJpdVdiMemFree(pCtx->stHeadInfo.pBlocks[i]);
        }

        pCtx->stHeadInfo.pBlocks[i] = NULL;
    }
}

/*********************************Thumbnail*************************************/
MS_S32 JpegThumbnailInit(JPD_Ctx_t *pCtx)
{
    UNUSED(pCtx);

    return 0;
}

#if SUPPORT_PROGRESSIVE_MODE
/*****************************Progressive**********************************/
//------------------------------------------------------------------------------
JPEG_STATIC JPEG_BLOCK_TYPE *_JpegCoeffBufferGetp(JPD_Ctx_t *pCtx, PJPEG_COEFF_BUF cb, MS_U16 block_x, MS_U16 block_y)
{
    UNUSED(pCtx);

    if (block_x >= cb->u16Block_num_x)
    {
        _JpegTraceError(pCtx, E_JPEG_ASSERTION_ERROR);
        return NULL;
    }

    if (block_y >= cb->u16Block_num_y)
    {
        _JpegTraceError(pCtx, E_JPEG_ASSERTION_ERROR);
        return NULL;
    }

    return (JPEG_BLOCK_TYPE *)((unsigned long)(cb->pu8Data + block_x * cb->u16Block_size
                                               + block_y * (cb->u16Block_size * cb->u16Block_num_x)));
}

//------------------------------------------------------------------------------
// Allocate a block of memory-- store block's address in list for
// later deallocation by JpegFreeAllBlocks().
JPEG_STATIC void *_JpegAllocMemery(JPD_Ctx_t *pCtx, MS_U32 n)
{
    MS_U8 i;
    void *q;

    // Find a free slot. The number of allocated slots will
    // always be very low, so a linear search is good enough.
    for (i = 0; i < JPEG_MAXBLOCKS; i++)
    {
        if (pCtx->stHeadInfo.pBlocks[i] == NULL)
        {
            break;
        }
    }

    if (i >= JPEG_MAXBLOCKS)
    {
        _JpegTraceError(pCtx, E_JPEG_TOO_MANY_BLOCKS);
        return NULL;
    }

    // FixMe: eCos does not support aligned allocate ???
    q = DrvJpdVdiMemAlloc(n + 8);
    // q = MsOS_AllocateAlignedMemory(n+8, 8, gs32CachedPoolID);
    // q = MsOS_AllocateMemory(n+8, gs32CachedPoolID);

    if (q == NULL)
    {
        _JpegTraceError(pCtx, E_JPEG_NOTENOUGHMEM);
        return NULL;
    }

    JPEG_memset((void *)q, 0, n + 8);

    pCtx->stHeadInfo.pBlocks[i] = q;

    JPD_LOGD("JPEG %d bytes allocated\n", n);

    return q;
}

//------------------------------------------------------------------------------
// The coeff_buf series of methods originally stored the coefficients
// into a "virtual" file which was located in EMS, XMS, or a disk file. A cache
// was used to make this process more efficient. Now, we can store the entire
// thing in RAM.
JPEG_STATIC PJPEG_COEFF_BUF _JpegCoeffBufOpen(JPD_Ctx_t *pCtx, MS_U16 block_num_x, MS_U16 block_num_y,
                                              MS_U8 block_len_x, MS_U8 block_len_y)
{
    PJPEG_COEFF_BUF cb = (PJPEG_COEFF_BUF)_JpegAllocMemery(pCtx, sizeof(JPEG_COEFF_BUF));

    if (cb == NULL)
    {
        return NULL;
    }

    cb->u16Block_num_x = block_num_x;
    cb->u16Block_num_y = block_num_y;

    cb->u8Block_len_x = block_len_x;
    cb->u8Block_len_y = block_len_y;

    cb->u16Block_size = (block_len_x * block_len_y) * sizeof(JPEG_BLOCK_TYPE);

    cb->pu8Data = (MS_U8 *)_JpegAllocMemery(pCtx, cb->u16Block_size * block_num_x * block_num_y);

    if (cb->pu8Data == NULL)
    {
        return NULL;
    }

    return cb;
}
//------------------------------------------------------------------------------
// Decodes a Huffman encoded symbol.
JPEG_STATIC MS_S32 _JpegHuffDecode(JPD_Ctx_t *pCtx, JPEG_HUFF_TBL *Ph)
{
    MS_S32 symbol;

    // Check first 8-bits: do we have a complete symbol?
    symbol = Ph->s16Look_up[(pCtx->stHeadInfo.u32Bit_buf >> 8) & 0xFF];
    if (symbol < 0)
    {
        // Decode more bits, use a tree traversal to find symbol.
        _JpegGetBits2(pCtx, 8);

        do
        {
            symbol = Ph->s16Tree[~symbol + (1 - _JpegGetBits2(pCtx, 1))];
        } while (symbol < 0);
    }
    else
    {
        _JpegGetBits2(pCtx, Ph->u8Code_size[symbol]);
    }

    return symbol;
}
//------------------------------------------------------------------------------
// Restart interval processing.
JPEG_STATIC MS_BOOL _JpegProcessRestart(JPD_Ctx_t *pCtx)
{
    MS_U16 i, c = 0;

    // Let's scan a little bit to find the marker, but not _too_ far.
    // 1536 is a "fudge factor" that determines how much to scan.
    for (i = 1536; i > 0; i--)
    {
        if (_JpegGetChar(pCtx) == 0xFF)
        {
            break;
        }
    }

    if (i == 0)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_RESTART_MARKER);
        return FALSE;
    }

    for (; i > 0; i--)
    {
        c = _JpegGetChar(pCtx);
        if (c != 0xFF)
        {
            break;
        }
    }

    if (i == 0)
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_RESTART_MARKER);
        return FALSE;
    }

    // Is it the expected marker? If not, something bad happened.
    if (c != (pCtx->stDealInfo.u16Next_restart_num + E_JPEG_RST0))
    {
        _JpegTraceError(pCtx, E_JPEG_BAD_RESTART_MARKER);
        return FALSE;
    }

    // Reset each component's DC prediction values.
    JPEG_memset((void *)&pCtx->stProgressiveInfo.u32Last_dc_val, 0, pCtx->stHeadInfo.u8Comps_in_frame * sizeof(MS_U32));

    pCtx->stProgressiveInfo.u32EOB_run = 0;

    pCtx->stDealInfo.u16Restarts_left = pCtx->stHeadInfo.u16Restart_interval;

    pCtx->stDealInfo.u16Next_restart_num = (pCtx->stDealInfo.u16Next_restart_num + 1) & 7;

    // Get the bit buffer going again...
    {
        pCtx->stHeadInfo.s16Bits_left = 16;
        _JpegGetBits2(pCtx, 16);
        _JpegGetBits2(pCtx, 16);
    }
    return TRUE;
}

//------------------------------------------------------------------------------
JPEG_STATIC MS_BOOL _JpegDecodeBlockDCRefine( // JPEG_DECODER *Pd,
    JPD_Ctx_t *pCtx, MS_U8 componentId, MS_U16 block_x, MS_U16 block_y)
{
    if (_JpegGetBits2(pCtx, 1))
    {
        JPEG_BLOCK_TYPE *p =
            _JpegCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.DC_Coeffs[componentId], block_x, block_y);

        if (p == NULL)
        {
            JPD_LOGE("\n");
            return FALSE;
        }

        p[0] |= (1 << pCtx->stHeadInfo.u8Successive_low);
    }
    return TRUE;
}
//------------------------------------------------------------------------------
// The following methods decode the various types of blocks encountered
// in progressively encoded images.
JPEG_STATIC MS_BOOL _JpegDecodeBlockDCFirst( // JPEG_DECODER *Pd,
    JPD_Ctx_t *pCtx, MS_U8 componentId, MS_U16 block_x, MS_U16 block_y)
{
    MS_S32           s, r;
    JPEG_BLOCK_TYPE *p = _JpegCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.DC_Coeffs[componentId], block_x, block_y);

    if (p == NULL)
    {
        JPD_LOGE("\n");
        return FALSE;
    }

    s = _JpegHuffDecode(pCtx, &pCtx->stProgressiveInfo.Huff_tbls[pCtx->stHeadInfo.u8Comp_dc_tab[componentId]]);
    if (s != 0)
    {
        r = _JpegGetBits2(pCtx, s);
        s = HUFF_EXTEND_P(r, s);
    }

    // In NJPD mode, the DC coefficient is the difference of nearest DC
    pCtx->stProgressiveInfo.u32Last_dc_val[componentId] = (s += pCtx->stProgressiveInfo.u32Last_dc_val[componentId]);

    p[0] = s << pCtx->stHeadInfo.u8Successive_low;
    return TRUE;
}

//------------------------------------------------------------------------------
JPEG_STATIC MS_BOOL _JpegDecodeBlockACRefine( // JPEG_DECODER *Pd,
    JPD_Ctx_t *pCtx, MS_U8 componentId, MS_U16 block_x, MS_U16 block_y)
{
    MS_S32           s, k, r;
    MS_S32           p1 = 1 << pCtx->stHeadInfo.u8Successive_low;
    MS_S32           m1 = (-1) << pCtx->stHeadInfo.u8Successive_low;
    JPEG_BLOCK_TYPE *p  = _JpegCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.AC_Coeffs[componentId], block_x, block_y);

    if (p == NULL)
    {
        JPD_LOGE("\n");
        return FALSE;
    }

    k = pCtx->stHeadInfo.u8Spectral_start;

    if (pCtx->stProgressiveInfo.u32EOB_run == 0)
    {
        for (; (k <= pCtx->stHeadInfo.u8Spectral_end) && (k < 64); k++)
        {
            s = _JpegHuffDecode(pCtx, &pCtx->stProgressiveInfo.Huff_tbls[pCtx->stHeadInfo.u8Comp_ac_tab[componentId]]);

            r = s >> 4;
            s &= 15;

            if (s)
            {
                if (s != 1)
                {
                    _JpegTraceError(pCtx, E_JPEG_DECODE_ERROR);
                    return FALSE;
                }

                if (_JpegGetBits2(pCtx, 1))
                {
                    s = p1;
                }
                else
                {
                    s = m1;
                }
            }
            else
            {
                if (r != 15)
                {
                    pCtx->stProgressiveInfo.u32EOB_run = 1 << r;

                    if (r)
                    {
                        pCtx->stProgressiveInfo.u32EOB_run += _JpegGetBits2(pCtx, r);
                    }

                    break;
                }
            }

            do
            {
                // No need to do ZAG order in NJPD mode
                JPEG_BLOCK_TYPE *thisCoef;
                {
                    thisCoef = p + k;
                }

                if (*thisCoef != 0)
                {
                    if (_JpegGetBits2(pCtx, 1))
                    {
                        if ((*thisCoef & p1) == 0)
                        {
                            if (*thisCoef >= 0)
                            {
                                *thisCoef += p1;
                            }
                            else
                            {
                                *thisCoef += m1;
                            }
                        }
                    }
                }
                else
                {
                    if (--r < 0)
                    {
                        break;
                    }
                }

                k++;
            } while ((k <= pCtx->stHeadInfo.u8Spectral_end) && (k < 64));

            if ((s) && (k < 64))
            {
                p[k] = s;
            }
        }
    }

    if (pCtx->stProgressiveInfo.u32EOB_run > 0)
    {
        for (; (k <= pCtx->stHeadInfo.u8Spectral_end) && (k < 64); k++)
        {
            // No need to do ZAG order in NJPD mode
            JPEG_BLOCK_TYPE *thisCoef;

            thisCoef = p + k;

            if (*thisCoef != 0)
            {
                if (_JpegGetBits2(pCtx, 1))
                {
                    if ((*thisCoef & p1) == 0)
                    {
                        if (*thisCoef >= 0)
                        {
                            *thisCoef += p1;
                        }
                        else
                        {
                            *thisCoef += m1;
                        }
                    }
                }
            }
        }

        pCtx->stProgressiveInfo.u32EOB_run--;
    }
    return TRUE;
}
//------------------------------------------------------------------------------
JPEG_STATIC MS_BOOL _JpegDecodeBlockAcFirst( // JPEG_DECODER *Pd,
    JPD_Ctx_t *pCtx, MS_U8 componentId, MS_U16 block_x, MS_U16 block_y)
{
    JPEG_BLOCK_TYPE *p;
    MS_S32           k, s, r;

    if (pCtx->stProgressiveInfo.u32EOB_run)
    {
        pCtx->stProgressiveInfo.u32EOB_run--;
        return TRUE;
    }

    p = _JpegCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.AC_Coeffs[componentId], block_x, block_y);

    if (p == NULL)
    {
        JPD_LOGE("\n");
        return FALSE;
    }

    for (k = pCtx->stHeadInfo.u8Spectral_start; k <= pCtx->stHeadInfo.u8Spectral_end; k++)
    {
        s = _JpegHuffDecode(pCtx, &pCtx->stProgressiveInfo.Huff_tbls[pCtx->stHeadInfo.u8Comp_ac_tab[componentId]]);

        r = s >> 4;
        s &= 15;

        if (s)
        {
            k += r;
            if (k > 63)
            {
                _JpegTraceError(pCtx, E_JPEG_DECODE_ERROR);
                return FALSE;
            }

            r = _JpegGetBits2(pCtx, s);
            s = HUFF_EXTEND_P(r, s);

            {
                p[k] = s << pCtx->stHeadInfo.u8Successive_low;
            }
        }
        else
        {
            if (r == 15)
            {
                k += 15;
                if (k > 63)
                {
                    _JpegTraceError(pCtx, E_JPEG_DECODE_ERROR);
                    return FALSE;
                }
            }
            else
            {
                pCtx->stProgressiveInfo.u32EOB_run = 1 << r;

                if (r)
                {
                    pCtx->stProgressiveInfo.u32EOB_run += _JpegGetBits2(pCtx, r);
                }

                pCtx->stProgressiveInfo.u32EOB_run--;

                break;
            }
        }
    }
    return TRUE;
}
//------------------------------------------------------------------------------
// Decode a scan in a progressively encoded image.
JPEG_STATIC MS_BOOL _JpegDecodeScan(JPD_Ctx_t *pCtx, Pdecode_block_func decode_block_func)
{
    MS_U16 mcuRow, mcuCol, mcuBlock;
    MS_U32 blockXMcu[JPEG_MAXCOMPONENTS], blockYMcu[JPEG_MAXCOMPONENTS];

    JPEG_memset((void *)blockYMcu, 0, sizeof(blockYMcu));

    for (mcuCol = 0; mcuCol < pCtx->stDealInfo.u16Mcus_per_col; mcuCol++)
    {
        MS_U32 componentNum, componentId;

        JPEG_memset((void *)blockXMcu, 0, sizeof(blockXMcu));

        for (mcuRow = 0; mcuRow < pCtx->stDealInfo.u16Mcus_per_row; mcuRow++)
        {
            MS_U8 blockXMcuOfs = 0, blockYMcuOfs = 0;

            if ((pCtx->stHeadInfo.u16Restart_interval) && (pCtx->stDealInfo.u16Restarts_left == 0))
            {
                if (!_JpegProcessRestart(pCtx))
                {
                    return FALSE;
                }
            }

            for (mcuBlock = 0; mcuBlock < pCtx->stDealInfo.u8Blocks_per_mcu; mcuBlock++)
            {
                componentId = pCtx->stDealInfo.u8Mcu_org[mcuBlock];

                if (!decode_block_func(pCtx, componentId, blockXMcu[componentId] + blockXMcuOfs,
                                       blockYMcu[componentId] + blockYMcuOfs))
                {
                    JPD_LOGE("\n");
                    return FALSE;
                }

                if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
                {
                    blockXMcu[componentId]++;
                }
                else
                {
                    if (++blockXMcuOfs == pCtx->stHeadInfo.u8Comp_h_samp[componentId])
                    {
                        blockXMcuOfs = 0;

                        if (++blockYMcuOfs == pCtx->stHeadInfo.u8Comp_v_samp[componentId])
                        {
                            blockYMcuOfs = 0;

                            blockXMcu[componentId] += pCtx->stHeadInfo.u8Comp_h_samp[componentId];
                        }
                    }
                }
            }

            pCtx->stDealInfo.u16Restarts_left--;
        }

        if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
        {
            blockYMcu[pCtx->stHeadInfo.u8Comp_list[0]]++;
        }
        else
        {
            for (componentNum = 0; componentNum < pCtx->stHeadInfo.u8Comps_in_scan; componentNum++)
            {
                componentId = pCtx->stHeadInfo.u8Comp_list[componentNum];

                blockYMcu[componentId] += pCtx->stHeadInfo.u8Comp_v_samp[componentId];
            }
        }
    }
    return TRUE;
}

//------------------------------------------------------------------------------
// Decode a progressively encoded image.
MS_BOOL JpegInitProgressive(JPD_Ctx_t *pCtx)
{
    MS_U8 i;

    if (pCtx->stHeadInfo.u8Comps_in_frame == 4)
    {
        _JpegTraceError(pCtx, E_JPEG_UNSUPPORTED_COLORSPACE);
        return FALSE;
    }

    // Allocate the coefficient buffers.
    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_frame; i++)
    {
        pCtx->stProgressiveInfo.DC_Coeffs[i] = _JpegCoeffBufOpen(
            pCtx, ((pCtx->stDealInfo.gu16Max_mcus_per_row + 0x1) & ~0x1) * pCtx->stHeadInfo.u8Comp_h_samp[i],
            ((pCtx->stDealInfo.u16Max_mcus_per_col + 0x1) & ~0x1) * pCtx->stHeadInfo.u8Comp_v_samp[i], 1, 1);
        if (pCtx->stProgressiveInfo.DC_Coeffs[i] == NULL)
        {
            return FALSE;
        }

        pCtx->stProgressiveInfo.AC_Coeffs[i] = _JpegCoeffBufOpen(
            pCtx, ((pCtx->stDealInfo.gu16Max_mcus_per_row + 0x1) & ~0x1) * pCtx->stHeadInfo.u8Comp_h_samp[i],
            ((pCtx->stDealInfo.u16Max_mcus_per_col + 0x1) & ~0x1) * pCtx->stHeadInfo.u8Comp_v_samp[i], 8, 8);
        if (pCtx->stProgressiveInfo.AC_Coeffs[i] == NULL)
        {
            return FALSE;
        }
    }

    for (;;)
    {
        MS_BOOL            dcOnlyScan, refinementScan;
        Pdecode_block_func decodeBlockFunc;

        if (!JpegLocateSOSMarker(pCtx)) // start_of_scan tabel and some msg
        {
            break;
        }

        _JpegCalcMcuBlockOrder(pCtx);

        if (!_JpegCheckTable(pCtx))
        {
            break;
        }

        dcOnlyScan     = (pCtx->stHeadInfo.u8Spectral_start == 0);
        refinementScan = (pCtx->stHeadInfo.u8Successive_high != 0);

        if ((pCtx->stHeadInfo.u8Spectral_start > pCtx->stHeadInfo.u8Spectral_end)
            || (pCtx->stHeadInfo.u8Spectral_end > 63))
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_SOS_SPECTRAL);
            return FALSE;
        }

        if (dcOnlyScan)
        {
            if (pCtx->stHeadInfo.u8Spectral_end)
            {
                _JpegTraceError(pCtx, E_JPEG_BAD_SOS_SPECTRAL);
                return FALSE;
            }
        }
        else if (pCtx->stHeadInfo.u8Comps_in_scan != 1) /* AC scans can only contain one component */
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_SOS_SPECTRAL);
            return FALSE;
        }

        if ((refinementScan) && (pCtx->stHeadInfo.u8Successive_low != pCtx->stHeadInfo.u8Successive_high - 1))
        {
            _JpegTraceError(pCtx, E_JPEG_BAD_SOS_SUCCESSIVE);
            return FALSE;
        }

        if (dcOnlyScan)
        {
            if (refinementScan)
            {
                decodeBlockFunc = _JpegDecodeBlockDCRefine;
            }
            else
            {
                decodeBlockFunc = _JpegDecodeBlockDCFirst;
            }
        }
        else
        {
            if (refinementScan)
            {
                decodeBlockFunc = _JpegDecodeBlockACRefine;
            }
            else
            {
                decodeBlockFunc = _JpegDecodeBlockAcFirst;
            }
        }

        if (!_JpegDecodeScan(pCtx, decodeBlockFunc))
        {
            JPD_LOGE("_JpegDecodeScan failed! \n");
            _JpegTraceError(pCtx, E_JPEG_BAD_SOS_SPECTRAL);
            return FALSE;
        }
        pCtx->stHeadInfo.s16Bits_left = 0;
    }

    pCtx->stHeadInfo.u8Comps_in_scan = pCtx->stHeadInfo.u8Comps_in_frame;

    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_frame; i++)
    {
        pCtx->stHeadInfo.u8Comp_list[i] = i;
    }

    _JpegCalcMcuBlockOrder(pCtx);

    return TRUE;
}
#else
MS_BOOL JpegInitProgressive(JPD_Ctx_t *pCtx)
{
    return 0;
}
#endif
