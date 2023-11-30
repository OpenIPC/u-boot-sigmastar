/*
 * hal_jpd_ops.h- Sigmastar
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

#ifndef _HAL_JPD_OPS_H_
#define _HAL_JPD_OPS_H_

#include <mhal_common.h>
#include "hal_jpd_def.h"
//#include "drv_jpd_dev.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ENABLE_NJPD_DEBUG_MSG FALSE

#define NJPD_MEM_SCWGIF_BASE    0x0000
#define NJPD_MEM_SYMIDX_BASE    0x0400
#define NJPD_MEM_QTBL_BASE      0x0800
#define NJPD_MEM_TBL_TOTAL_SIZE 0x1000

/*****************Config Flag*********************/

// detail for reg: BK_NJPD_GLOBAL_SETTING00 (0x00)
#define NJPD_Y_HSF1  NJPD_BIT(0)
#define NJPD_Y_HSF2  NJPD_BIT(1)
#define NJPD_Y_VSF2  NJPD_BIT(2)
#define NJPD_UV      NJPD_BIT(3)
#define NJPD_SWRST   NJPD_BIT(4)
#define NJPD_RST_EN  NJPD_BIT(5)
#define NJPD_ROI_EN  NJPD_BIT(6)
#define NJPD_SUVQ    NJPD_BIT(7)
#define NJPD_SUVH    NJPD_BIT(8)
#define NJPD_YC_SWAP NJPD_BIT(10)
#define NJPD_UV_SWAP NJPD_BIT(11)

// detail for reg: BK_NJPD_GLOBAL_SETTING01 (0x01)
#define NJPD_DOWN_SCALE       (NJPD_BIT(0) | NJPD_BIT(1))
#define NJPD_SVLD             NJPD_BIT(6)
#define NJPD_UV_7BIT          NJPD_BIT(8)
#define NJPD_UV_MSB           NJPD_BIT(9)
#define NJPD_GTABLE_RST       NJPD_BIT(10)
#define NJPD_HTABLE_RELOAD_EN NJPD_BIT(11)
#define NJPD_GTABLE_RELOAD_EN NJPD_BIT(12)
#define NJPD_QTABLE_RELOAD_EN NJPD_BIT(13)
#define NJPD_WBUF_PACK        NJPD_BIT(14)
#define NJPD_W2_IMI_EN        NJPD_BIT(15)

// detail for reg: BK_NJPD_GLOBAL_SETTING02 (0x02)
#define NJPD_TBC_MODE             NJPD_BIT(1)
#define NJPD_FFD9_EN              NJPD_BIT(2)
#define NJPD_LITTLE_ENDIAN        NJPD_BIT(8)
#define NJPD_REMOVE_0xFF00        NJPD_BIT(9)
#define NJPD_REMOVE_0xFFFF        NJPD_BIT(10)
#define NJPD_HUFF_TABLE_ERROR     NJPD_BIT(11)
#define NJPD_HUFF_DATA_LOSS_ERROR NJPD_BIT(12)
#define NJPD_BITSTREAM_LE         NJPD_BIT(13)
#define NJPD_SRAM_SD_EN           NJPD_BIT(15)

// detail for reg: BK_NJPD_WRITE_ONE_CLEAR (0x08)
#define NJPD_DECODE_ENABLE       NJPD_BIT(0)
#define NJPD_TABLE_LOADING_START NJPD_BIT(1)
#define NJPD_MIU_PARK            NJPD_BIT(2)
#define NJPD_MRC0_VALID          NJPD_BIT(3)
#define NJPD_MRC1_VALID          NJPD_BIT(4)
#define NJPD_MRC_LAST            NJPD_BIT(5)
#define NJPD_TBC_EN              NJPD_BIT(6)
#define NJPD_TBC_DONE_CLR        NJPD_BIT(7)
#define NJPD_CLEAR_CRC           NJPD_BIT(8)
#define NJPD_HANDSHAKE_SW_WOC    NJPD_BIT(9)

// detail for reg: BK_NJPD_MIU_READ_STATUS (0x0e)
#define NJPD_MIU_MRC0_STATUS NJPD_BIT(0)
#define NJPD_MIU_MRC1_STATUS NJPD_BIT(1)
#define NJPD_MIU_HTABLE_RDY  NJPD_BIT(2)
#define NJPD_MIU_GTABLE_RDY  NJPD_BIT(3)
#define NJPD_MIU_QTABLE_RDY  NJPD_BIT(4)

// detail for reg: BK_NJPD1_IBUF_READ_LENGTH (0x28)
#define NJPD_MIU_SEL_SHIFT   10
#define NJPD_MIU_SEL_1_SHIFT 11
#define NJPD_MIU_SEL         NJPD_BIT(NJPD_MIU_SEL_SHIFT)   // this reg only supported in edison/einstein/eiffel
#define NJPD_MIU_SEL_1       NJPD_BIT(NJPD_MIU_SEL_1_SHIFT) // this reg only supported in einstein3

    // detail for reg: BK_NJPD_IRQ_CLEAR (0x29)
    //[0] Decode done event flag
    //[1] Mini-code error event flag
    //[2] Inverse scan error event flag
    //[3] Restart marker error event flag
    //[4] Restart marker index disorder error event flag
    //[5] End image error event flag
    //[6] Read buffer0 empty event flag
    //[7] Read buffer1 empty event flag
    //[8] MIU write protect event flag
    //[9] Data lose error event flag
    //[10] iBuf table load done flag
    //[11] Huffman table error

#define NJPD_EVENT_DECODE_DONE    NJPD_BIT(0)
#define NJPD_EVENT_MINICODE_ERR   NJPD_BIT(1)
#define NJPD_EVENT_INV_SCAN_ERR   NJPD_BIT(2)
#define NJPD_EVENT_RES_MARKER_ERR NJPD_BIT(3)
#define NJPD_EVENT_RMID_ERR       NJPD_BIT(4)
#define NJPD_EVENT_END_IMAGE_ERR  NJPD_BIT(5)
#define NJPD_EVENT_MRC0_EMPTY     NJPD_BIT(6)
#define NJPD_EVENT_MRC1_EMPTY     NJPD_BIT(7)
#define NJPD_EVENT_WRITE_PROTECT  NJPD_BIT(8)
#define NJPD_EVENT_DATA_LOSS_ERR  NJPD_BIT(9)
#define NJPD_EVENT_IBUF_LOAD_DONE NJPD_BIT(10)
#define NJPD_EVENT_HUFF_TABLE_ERR NJPD_BIT(11)
#define NJPD_EVENT_ALL                                                                                      \
    (NJPD_EVENT_DECODE_DONE | NJPD_EVENT_MINICODE_ERR | NJPD_EVENT_INV_SCAN_ERR | NJPD_EVENT_RES_MARKER_ERR \
     | NJPD_EVENT_RMID_ERR | NJPD_EVENT_END_IMAGE_ERR | NJPD_EVENT_MRC0_EMPTY | NJPD_EVENT_MRC1_EMPTY       \
     | NJPD_EVENT_WRITE_PROTECT | NJPD_EVENT_DATA_LOSS_ERR | NJPD_EVENT_IBUF_LOAD_DONE | NJPD_EVENT_HUFF_TABLE_ERR)

#define NJPD_EVENT_ERROR                                                                                 \
    (NJPD_EVENT_MINICODE_ERR | NJPD_EVENT_INV_SCAN_ERR | NJPD_EVENT_RES_MARKER_ERR | NJPD_EVENT_RMID_ERR \
     | NJPD_EVENT_END_IMAGE_ERR | NJPD_EVENT_DATA_LOSS_ERR | NJPD_EVENT_HUFF_TABLE_ERR)

// detail for reg: BK_NJPD_TBC (0x40)
#define NJPD_JPD_TBC_RW         NJPD_BIT(0)
#define NJPD_JPD_TBC_SEL        (NJPD_BIT(1) | NJPD_BIT(2))
#define NJPD_JPD_TBC_TABLE_READ NJPD_BIT(4)
#define NJPD_JPD_TBC_ADR                                                                                  \
    (NJPD_BIT(8) | NJPD_BIT(9) | NJPD_BIT(10) | NJPD_BIT(11) | NJPD_BIT(12) | NJPD_BIT(13) | NJPD_BIT(14) \
     | NJPD_BIT(15))

// spare (0x4a)
#define NJPD_RING_NUM    (0x7f)         // bit0-bit14
#define NJPD_RING_ENABLE (NJPD_BIT(15)) // bit0-bit14

// detail for reg: BK_NJPD2_MARB_SETTING_06 (0x56)
#define NJPD_JPD_MARB_MRPRIORITY_SW (NJPD_BIT(0) | NJPD_BIT(1))

// detail for reg: BK_NJPD2_MARB_SETTING_07 (0x57)
#define NJPD_JPD_MARB_BURST_SPLIT (NJPD_BIT(12) | NJPD_BIT(13))

// detail for reg: BK_NJPD_MARB_LBOUND_0_H (0x5b)
#define NJPD_MARB_MIU_BOUND_EN_0 NJPD_BIT(13)

// detail for reg: BK_NJPD_TOP_MARB_PORT_ENABLE (0x76)
#define NJPD_TOP_MARB_P0_ENABLE          NJPD_BIT(0)
#define NJPD_TOP_MARB_P1_ENABLE          NJPD_BIT(1)
#define NJPD_TOP_MARB_P2_ENABLE          NJPD_BIT(2)
#define NJPD_TOP_MARB_P0_W_BYPASS_ENABLE NJPD_BIT(4)
#define NJPD_TOP_MARB_P1_W_BYPASS_ENABLE NJPD_BIT(5)
#define NJPD_TOP_MARB_P2_W_BYPASS_ENABLE NJPD_BIT(6)
#define NJPD_TOP_MARB_P0_R_BYPASS_ENABLE NJPD_BIT(7)
#define NJPD_TOP_MARB_P1_R_BYPASS_ENABLE NJPD_BIT(0) // second byte
#define NJPD_TOP_MARB_P2_R_BYPASS_ENABLE NJPD_BIT(1) // second byte

    //-------------------------------------------------------------------------------------------------
    //  Type and Structure
    //-------------------------------------------------------------------------------------------------

    // JPEG header marker id
    typedef enum
    {
        E_JPEG_SOF0  = 0xC0,
        E_JPEG_SOF1  = 0xC1,
        E_JPEG_SOF2  = 0xC2,
        E_JPEG_SOF3  = 0xC3,
        E_JPEG_SOF5  = 0xC5,
        E_JPEG_SOF6  = 0xC6,
        E_JPEG_SOF7  = 0xC7,
        E_JPEG_JPG   = 0xC8,
        E_JPEG_SOF9  = 0xC9,
        E_JPEG_SOF10 = 0xCA,
        E_JPEG_SOF11 = 0xCB,
        E_JPEG_SOF13 = 0xCD,
        E_JPEG_SOF14 = 0xCE,
        E_JPEG_SOF15 = 0xCF,
        E_JPEG_DHT   = 0xC4,
        E_JPEG_DAC   = 0xCC,
        E_JPEG_RST0  = 0xD0,
        E_JPEG_RST1  = 0xD1,
        E_JPEG_RST2  = 0xD2,
        E_JPEG_RST3  = 0xD3,
        E_JPEG_RST4  = 0xD4,
        E_JPEG_RST5  = 0xD5,
        E_JPEG_RST6  = 0xD6,
        E_JPEG_RST7  = 0xD7,
        E_JPEG_SOI   = 0xD8,
        E_JPEG_EOI   = 0xD9,
        E_JPEG_SOS   = 0xDA,
        E_JPEG_DQT   = 0xDB,
        E_JPEG_DNL   = 0xDC,
        E_JPEG_DRI   = 0xDD,
        E_JPEG_DHP   = 0xDE,
        E_JPEG_EXP   = 0xDF,
        E_JPEG_APP0  = 0xE0,
        E_JPEG_APP1  = 0xE1,
        E_JPEG_APP2  = 0xE2,
        E_JPEG_APP3  = 0xE3,
        E_JPEG_APP4  = 0xE4,
        E_JPEG_APP5  = 0xE5,
        E_JPEG_APP6  = 0xE6,
        E_JPEG_APP7  = 0xE7,
        E_JPEG_APP8  = 0xE8,
        E_JPEG_APP9  = 0xE9,
        E_JPEG_APP10 = 0xEA,
        E_JPEG_APP11 = 0xEB,
        E_JPEG_APP12 = 0xEC,
        E_JPEG_APP13 = 0xED,
        E_JPEG_APP14 = 0xEE,
        E_JPEG_APP15 = 0xEF,
        E_JPEG_JPG0  = 0xF0,
        E_JPEG_JPG1  = 0xF1,
        E_JPEG_JPG2  = 0xF2,
        E_JPEG_JPG3  = 0xF3,
        E_JPEG_JPG4  = 0xF4,
        E_JPEG_JPG5  = 0xF5,
        E_JPEG_JPG6  = 0xF6,
        E_JPEG_JPG7  = 0xF7,
        E_JPEG_JPG8  = 0xF8,
        E_JPEG_JPG9  = 0xF9,
        E_JPEG_JPG10 = 0xFA,
        E_JPEG_JPG11 = 0xFB,
        E_JPEG_JPG12 = 0xFC,
        E_JPEG_JPG13 = 0xFD,
        E_JPEG_COM   = 0xFE,
        E_JPEG_TEM   = 0x01,
        E_JPEG_ERROR = 0x100
    } EN_JPEG_HDR_MARKER;

    typedef enum
    {
        E_NJPD_EVENT_DEC_NONE       = 0x00,
        E_NJPD_EVENT_DEC_DONE       = 0x01,
        E_NJPD_EVENT_MINICODE_ERR   = 0x02,
        E_NJPD_EVENT_INV_SCAN_ERR   = 0x04,
        E_NJPD_EVENT_RES_MARKER_ERR = 0x08,
        E_NJPD_EVENT_RMID_ERR       = 0x10

        ,
        E_NJPD_EVENT_END_IMAGE_ERR = 0x20,
        E_NJPD_EVENT_MRC0_EMPTY    = 0x40,
        E_NJPD_EVENT_MRC1_EMPTY    = 0x80,
        E_NJPD_EVENT_WRITE_PROTECT = 0x100,
        E_NJPD_EVENT_DATA_LOSS_ERR = 0x200

        ,
        E_NJPD_EVENT_IBUF_LOAD_DONE = 0x400,
        E_NJPD_EVENT_HUFF_TABLE_ERR = 0x800
    } EN_NJPD_EVENT;

    // NJPD Downscale Ratio
    // Bellows are 1, 1/2, 1/4 and 1/8 in order
    typedef enum
    {
        E_NJPD_DOWNSCALE_ORG    = 0x00,
        E_NJPD_DOWNSCALE_HALF   = 0x01,
        E_NJPD_DOWNSCALE_FOURTH = 0x02,
        E_NJPD_DOWNSCALE_EIGHTH = 0x03
    } EN_NJPD_DOWN_SCALE;

    // NJPD debug level enum
    typedef enum
    {
        E_NJPD_DEBUG_DRV_NONE = 0x0,
        E_NJPD_DEBUG_DRV_MSG  = 0x02,
        E_NJPD_DEBUG_DRV_ERR  = 0x08
    } EN_NJPD_DRV_DBG_LEVEL;

    typedef enum
    {
        E_NJPD_FAILED        = 0,
        E_NJPD_OK            = 1,
        E_NJPD_INVALID_PARAM = 2
    } EN_NJPD_RETURN;

    typedef enum
    {
        E_SCALING_ORG = 0,
        E_SCALING_1_2 = 1,
        E_SCALING_1_4 = 2,
        E_SCALING_1_8 = 3
    } EN_NJPD_SCALING_DOWN_FACTOR;

    typedef enum
    {
        E_MJPEG_DRV_STREAM_NONE = 0,
        E_MJPEG_DRV_MAIN_STREAM,
        E_MJPEG_DRV_SUB_STREAM
    } EN_MJPEG_DRV_STREAM_TYPE;

    typedef enum
    {
        E_NJPD_NJPD1,
        E_NJPD_NJPD2,
        E_NJPD_NJPD_TOTAL, // Do not remove this one
        E_NJPD_MJPEG_ON  = 0xcc,
        E_NJPD_MJPEG_OFF = 0xdd,
    } EN_JPD_NUMBER;

    // NJPD Interrupt Register Function
    typedef void (*NJPD_ISR_FUNC_CB)(void);

    //------------------------------------------------------------------------------
    // Structure for buffer
    typedef struct
    {
        MS_PHY  u32ThumbnailBufAddr;
        MS_U32  u32ThumbnailBufSize;
        MS_U32  u32ThumbnailBufOffset;
        MS_PHY  u32MRCBufAddr;    // MRC buffer address
        MS_U32  u32MRCBufSize;    // MRC buffer size
        MS_U32  u32MRCBufOffset;  // MRC access byte address offset in MRC buffer relative to MRC start address
        MS_PHY  u32VirMRCBufAddr; // MWC buffer address
        MS_PHY  u32MWCBufAddr;    // MWC buffer address
        MS_U16  u16MWCBufLineNum; // MWC Line number
        MS_PHY  u32TableAddr;     // table address
        MS_PHY  u32VirTableAddr;  //  table address
        MS_BOOL bProgressive;
        MS_BOOL bThumbnailAccessMode;
        MS_BOOL bIMIMode;
    } NJPD_BUF_CFG;
    //------------------------------------------------------------------------------
    // Structure for Grpinf of Table IndirectAccess
    typedef struct
    {
        MS_U8 * u8DcLumaValid;
        MS_U8 * u8DcLumaSymbol;
        MS_U16 *u16DcLumaCode;
        MS_U8 * u8DcChromaValid;
        MS_U8 * u8DcChromaSymbol;
        MS_U16 *u16DcChromaCode;
        MS_U8 * u8DcChroma2Valid;
        MS_U8 * u8DcChroma2Symbol;
        MS_U16 *u16DcChroma2Code;
        MS_U8 * u8AcLumaValid;
        MS_U8 * u8AcLumaSymbol;
        MS_U16 *u16AcLumaCode;
        MS_U8 * u8AcChromaValid;
        MS_U8 * u8AcChromaSymbol;
        MS_U16 *u16AcChromaCode;
        MS_U8 * u8AcChroma2Valid;
        MS_U8 * u8AcChroma2Symbol;
        MS_U16 *u16AcChroma2Code;
        MS_BOOL DHT;
        MS_BOOL bUVHuffman;
        MS_U8   u8DcLumaCnt;
        MS_U8   u8DcChromaCnt;
        MS_U8   u8DcChroma2Cnt;
        MS_U8   u8AcLumaCnt;
        MS_U8   u8AcChromaCnt;
        MS_U8   u8AcChroma2Cnt;
    } NJPD_GRPINF;
    //------------------------------------------------------------------------------
    // Structure for Symidx of Table Indirect Access
    typedef struct
    {
        MS_U8 * u8DcLumaHuffVal;
        MS_U8 * u8DcChromaHuffVal;
        MS_U8 * u8DcChroma2HuffVal;
        MS_U8 * u8AcLumaHuffVal;
        MS_U8 * u8AcChromaHuffVal;
        MS_U8 * u8AcChroma2HuffVal;
        MS_BOOL DHT;
        MS_BOOL bUVHuffman;
    } NJPD_SYMIDX;
    //------------------------------------------------------------------------------
    // Structure for Quantization Table
    typedef struct
    {
        MS_U16  s16Value[64];
        MS_BOOL bValid;
    } NJPD_QUAN_TBL;
    //------------------------------------------------------------------------------
    // Structure for IQtbl of Table Indirect Access
    typedef struct
    {
        MS_U8 *        u8CompQuant;
        NJPD_QUAN_TBL *QuantTables;
        MS_U8          u8CompNum;
        MS_BOOL        DQT;
    } NJPD_IQ_TBL;
    //------------------------------------------------------------------------------
    // Structure for NJPD capability
    typedef struct
    {
        MS_BOOL bBaseline;
        MS_BOOL bProgressive;
        MS_BOOL bMJPEG;
    } NJPD_CAP;

    //------------------------------------------------------------------------------
    // Structure for NJPD Status
    typedef struct
    {
        MS_U32  u32CurMRCAddr;
        MS_U16  u16CurVidx;
        MS_U16  u16CurRow;
        MS_U16  u16CurCol;
        MS_BOOL bIsBusy;
        MS_BOOL bIsrEnable;
    } NJPD_STATUS;

    typedef struct
    {
        MS_PHY u32TableVirAddr;
        MS_PHY u32TablePhyAddr;

        MS_BOOL bIs3HuffTbl;
        MS_U8   u8DcLumaCnt;
        MS_U8   u8DcChromaCnt;
        MS_U8   u8DcChroma2Cnt;
        MS_U8   u8AcLumaCnt;
        MS_U8   u8AcChromaCnt;
        MS_U8   u8AcChroma2Cnt;

        MS_PHY u32MRC0_Start;
        MS_PHY u32MRC0_End;
        MS_PHY u32MRC1_Start;
        MS_PHY u32MRC1_End;
        MS_PHY u32READ_Start;

    } NJPD_DECODE_CTRL_S;

    //-------------------------------------------------------------------------------------------------
    //  Function and Variable
    //-------------------------------------------------------------------------------------------------
    NJPD_STATUS *MapiJpdGetStatus(MS_U8 DevId);

    void        MapiJpdSetRstIntv(MS_U8 DevId, MS_U16 u16Value);
    void        MapiJpdReset(MS_U8 DevId);
    void        MapiJpdSetPicDimension(MS_U8 DevId, MS_U16 u16Width, MS_U16 u16Height);
    void        MapiJpdSetEventFlag(MS_U8 DevId, MS_U16 u16Value);
    void        MapiJpdSetROI(MS_U8 DevId, MS_U16 start_x, MS_U16 start_y, MS_U16 width, MS_U16 height);
    void        MapiJpdWriteGrpinf(MS_U8 DevId, NJPD_GRPINF in);
    void        ApiJpdWriteSymidx(MS_U8 DevId, NJPD_SYMIDX in);
    void        MapiJpdWriteIQTbl(MS_U8 DevId, NJPD_IQ_TBL in);
    void        MapiJpdSetAutoProtect(MS_U8 DevId, MS_BOOL enable);
    void        MapiJpdSetWPENEndAddr(MS_U8 DevId, MS_PHY u32ByteOffset);
    void        MapiJpdSetWPENStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset);
    void        MapiJpdSetSpare(MS_U8 DevId, MS_U16 u16Value);
    void        MapiJpdSetMRCValid(MS_U8 DevId, MS_U16 u16Value);
    void        MapiJpdDecodeEnable(MS_U8 DevId);
    void        MapiJpdTableLoadingStart(MS_U8 DevId);
    void        MapiJpdReadLastBuffer(MS_U8 DevId);
    void        MapiJpdSetScalingDownFactor(MS_U8 DevId, EN_NJPD_SCALING_DOWN_FACTOR eScalingFactor);
    void        MapiJpdGTableRst(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdHTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdGTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdQTableReloadEnable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdWriteIMIEnable(MS_U8 DevId, MS_BOOL bEnable); // imi
    void        MapiJpdWbufPackEnable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdSetSoftwareVLD(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdSetDifferentQTable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdSetDifferentHTable(MS_U8 DevId, MS_BOOL bEnable);
    void        MapiJpdSetGlobalSetting00(MS_U8 DevId, MS_U16 u16Value);
    void        MapiJpdSetHTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset);
    void        MapiJpdSetQTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset);
    void        MapiJpdSetGTableStartAddr(MS_U8 DevId, MS_PHY u32ByteOffset);
    void        MapiJpdDebug(MS_U8 DevId);
    void        MapiJpdEnablePowerSaving(MS_U8 DevId);
    void        MapiJpdSetWRingBufLineNum(MS_U8 DevId, MS_U16 LineNum);
    void        MapiJpdSetWRingBufEnable(MS_U8 DevId, MS_U16 IsEnable);
    MS_U16      MapiJpdGetWRingBufEnable(MS_U8 DevId);
    MS_U16      MapiJpdGetCurRow(MS_U8 DevId);
    MS_U16      MapiJpdGetCurCol(MS_U8 DevId);
    MS_U16      MapiJpdGetSpare(MS_U8 DevId);
    MS_U16      MapiJpdGetEventFlag(MS_U8 DevId);
    MS_U16      MapiJpdGetGlobalSetting00(MS_U8 DevId);
    MS_U16      MapiJpdGetCurVidx(void);
    MS_U32      MapiJpdGetCurMRCAddr(MS_U8 DevId);
    MS_U32      MapiJpdGetWritePtrAddr(MS_U8 DevId);
    MS_U32      MapiJpdGetMWCStartAddr(MS_U8 DevId);
    MS_BOOL     MapiJpdInitBuffer(MS_U8 DevId, NJPD_BUF_CFG in, MS_BOOL bMJPEG);
    void        MapiJpdPZ1SetFlag(MS_U16 u16Value);
    void        MapiJpdInitRegBase(int u32DevId, MS_U64 u16Value);
    void        MapiJpdInitClk(int u32DevId, MS_U64 u16Value);
    extern void invalidate_dcache_range(unsigned long start, unsigned long stop);
    extern void flush_dcache_range(unsigned long start, unsigned long stop);
    extern void udelay(unsigned long usec);

#ifdef NJPD_CMDQ_SUPPORT
    void MapiJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf);
    void MapiJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen);
#endif
#ifdef __cplusplus
}
#endif

#endif // _HAL_JPD_OPS_H_
