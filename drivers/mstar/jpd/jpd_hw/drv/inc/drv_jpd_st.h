/*
 * drv_jpd_st.h- Sigmastar
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

#ifndef __DRV_JPD_ST_H__
#define __DRV_JPD_ST_H__

#if !defined(__I_SW__)
#include <asm/types.h>
//#include <linux/cdev.h>
//#include <linux/device.h>
//#include <linux/interrupt.h>
//#include <linux/kernel.h>
//#include <linux/platform_device.h>
//#include <cam_clkgen.h>
//#include <cam_sysfs.h>
#endif

#include "hal_jpd_def.h"
#include "mhal_jpd.h"

#define SUPPORT_PROGRESSIVE_MODE    1
#define SUPPORT_EXIF_EXTRA_INFO     1
#define SUPPORT_GET_FULL_IMAGE_INFO 0

// Max. allocated blocks
#define JPEG_MAXBLOCKS         50UL
#define JPEG_MANUFACTURER_SIZE 32UL
#define JPEG_MODEL_SIZE        128UL

#define JPEG_DEFAULT_EXIF_SIZE (64 * 1024UL + 128)
#define MRC_DECODE_TIMEOUT     3000UL

#define JPEG_MAXBLOCKS 50UL

#define JPD_CLOCKS_NR 2
#define JPD_CLOCK_ON  1
#define JPD_CLOCK_OFF 0
#define DLL_PACKED    __attribute__((__packed__))

#define JPD_DEV_HANDLE_MAGIC_NUM (0x55aaaa55)
#define JPD_CHECK_DEV_MAGIC_NUM(handle)                                            \
    {                                                                              \
        if ((handle) == NULL || (handle)->s32MagicNum != JPD_DEV_HANDLE_MAGIC_NUM) \
        {                                                                          \
            JPD_LOGE("CHECK DEV MAGIC NUM FAIL!\n");                               \
            return E_MHAL_ERR_NOT_SUPPORT;                                         \
        }                                                                          \
    }

#define JPD_CHECK_DEV_ID(u32DevId, u32MaxId)                           \
    do                                                                 \
    {                                                                  \
        if ((u32DevId) >= (u32MaxId))                                  \
        {                                                              \
            JPD_LOGE("CHECK DEV ID fail u32DevId=0x%x\n", (u32DevId)); \
            return E_MHAL_ERR_INVALID_DEVID;                           \
        }                                                              \
    } while (0)

#define MAX_JPD_CHN_PER_DEV (64)
#define MAX_JPD_CHANNEL     (MAX_JPD_CHN_PER_DEV * MAX_JPD_DEVICE_NUM)

#if defined(CHIP_I7)
#define JPD_CLK_DEFAULT (E_JPD_CLKGEN_432_MHZ)
#elif defined(CHIP_M6P)
#define JPD_CLK_DEFAULT (E_JPD_CLKGEN_384_MHZ)
#else
// do nothing
#endif
#define JPD_CLK_NUM_DEFAULT (4)

typedef enum
{
    E_JPD_CLKGEN_432_MHZ    = 432000000,
    E_JPD_CLKGEN_384_MHZ    = 384000000,
    E_JPD_CLKGEN_320_MHZ    = 320000000,
    E_JPD_CLKGEN_288_MHZ    = 288000000,
    E_JPD_CLKGEN_172_P8_MHZ = 172800000,
    E_JPD_CLKGEN_240_MHZ    = 240000000,

    // I7 JPD new add
    E_JPD_CLKGEN_600_MHZ = 600000000,
    E_JPD_CLKGEN_533_MHZ = 533000000,
    E_JPD_CLKGEN_480_MHZ = 480000000,
    E_JPD_CLKGEN_345_MHZ = 345000000,
    E_JPD_CLKGEN_INVALID = -1
} EN_JPD_CLK;

typedef enum
{
    E_JPEG_FAILED = 0,
    E_JPEG_OKAY   = 1,
    E_JPEG_DONE   = 2,
    E_JPEG_RETRY  = 3
} EN_JPEG_RESULT;

//-----------------------------------------------------------------------------
/// @brief \b Enum \b Name: JPEG_ErrCode
/// @brief \b Enum \b Description: JPEG error code
//-----------------------------------------------------------------------------
typedef enum
{
    E_JPEG_NO_ERROR                    = 0,
    E_JPEG_BAD_DHT_COUNTS              = -200,
    E_JPEG_BAD_DHT_INDEX               = -201,
    E_JPEG_BAD_DHT_MARKER              = -202,
    E_JPEG_BAD_DQT_MARKER              = -203,
    E_JPEG_BAD_DQT_TABLE               = -204,
    E_JPEG_BAD_PRECISION               = -205,
    E_JPEG_BAD_HEIGHT                  = -206,
    E_JPEG_BAD_WIDTH                   = -207,
    E_JPEG_TOO_MANY_COMPONENTS         = -208,
    E_JPEG_BAD_SOF_LENGTH              = -209,
    E_JPEG_BAD_VARIABLE_MARKER         = -210,
    E_JPEG_BAD_DRI_LENGTH              = -211,
    E_JPEG_BAD_SOS_LENGTH              = -212,
    E_JPEG_BAD_SOS_COMP_ID             = -213,
    E_JPEG_W_EXTRA_BYTES_BEFORE_MARKER = -214,
    E_JPEG_NO_ARITHMETIC_SUPPORT       = -215,
    E_JPEG_UNEXPECTED_MARKER           = -216,
    E_JPEG_NOT_JPEG                    = -217,
    E_JPEG_UNSUPPORTED_MARKER          = -218,
    E_JPEG_BAD_DQT_LENGTH              = -219,
    E_JPEG_TOO_MANY_BLOCKS             = -221,
    E_JPEG_UNDEFINED_QUANT_TABLE       = -222,
    E_JPEG_UNDEFINED_HUFF_TABLE        = -223,
    E_JPEG_NOT_SINGLE_SCAN             = -224,
    E_JPEG_UNSUPPORTED_COLORSPACE      = -225,
    E_JPEG_UNSUPPORTED_SAMP_FACTORS    = -226,
    E_JPEG_DECODE_ERROR                = -227,
    E_JPEG_BAD_RESTART_MARKER          = -228,
    E_JPEG_ASSERTION_ERROR             = -229,
    E_JPEG_BAD_SOS_SPECTRAL            = -230,
    E_JPEG_BAD_SOS_SUCCESSIVE          = -231,
    E_JPEG_STREAM_READ                 = -232,
    E_JPEG_NOTENOUGHMEM                = -233,
    E_JPEG_STOP_DECODE                 = -234 // kevinhuang, add
    ,
    E_JPEG_BAD_APP1_MARKER         = -235,
    E_JPEG_NO_THUMBNAIL            = -236,
    E_JPEG_UNSUPPORTED_HUFF_DECODE = -237,
    E_JPEG_READBUFFER_TOOSMALL     = -238,
    E_JPEG_NOT_ENOUGH_HEADER_INFO  = -240,
    E_JPEG_RE_INIT                 = -241,
    E_JPEG_NOT_INIT                = -242,
    E_JPEG_BAD_APP0_MARKER         = -243,
    E_JPEG_JPD_DECODE_ERROR        = 0x5566
} EN_JPEG_ERR_CODE;

//-----------------------------------------------------------------------------
/// @brief \b Enum \b Name: JPEG_EXIF_IFD0_Orientation
/// @brief \b Enum \b Description: The orientation of JPEG EXIF
//-----------------------------------------------------------------------------
typedef enum
{
    E_JPEG_EXIF_ORIENT_NOT_FOUND           = 0,
    E_JPEG_EXIF_ORIENT_ROTATE_0            = 1,
    E_JPEG_EXIF_ORIENT_MIRROR_LR           = 2,
    E_JPEG_EXIF_ORIENT_ROTATE_180          = 3,
    E_JPEG_EXIF_ORIENT_MIRROR_TB           = 4,
    E_JPEG_EXIF_ORIENT_ROTATE_90_MIRROR_LR = 5,
    E_JPEG_EXIF_ORIENT_ROTATE_90           = 6,
    E_JPEG_EXIF_ORIENT_ROTATE_90_MIRROR_TB = 7,
    E_JPEG_EXIF_ORIENT_ROTATE_270          = 8,
    E_JPEG_EXIF_ORIENT_RESERVED            = 9
} EN_JPEG_EXIF_ORIENTATION;

typedef enum
{
    E_JPD_STEP_DEC_BEGIN = 0x01,
    E_JPD_STEP_DEC_ERR   = 0x02,
    E_JPD_STEP_WAIT_DONE = 0x03,
    E_JPD_STEP_INIT      = 0x04
} EN_JPD_DEC_STEP;

typedef enum
{
    E_JPD_RET_CODE_SUCCESS        = 0, /**< pass*/
    E_JPD_RET_CODE_DEV_NOTINIT    = 1, /**< Error code of Dev_NoInit*/
    E_JPD_RET_CODE_CTX_NOTINIT    = 2, /**< Error code of Ctx_NoInit*/
    E_JPD_RET_CODE_ILLEGAL_PARAM  = 3, /**< Error code of Illegal*/
    E_JPD_RET_CODE_REGISTER_FAIL  = 4, /**< Error code of Register*/
    E_JPD_RET_CODE_ALLOC_CTX_FAIL = 5, /**< Error code of Alloc Ctx Memory*/
    E_JPD_RET_CODE_INIT_ADDR_FAIL = 6, /**< Error code of init Addr failed*/
} EN_JPD_RET_CODE;

//-----------------------------------------------------------------------------
/// @brief \b Enum \b Name: JPEG_BuffLoadType
/// @brief \b Enum \b Description: JPEG buffer loading mode
//-----------------------------------------------------------------------------
typedef enum
{
    E_JPEG_BUFFER_NONE = 0,
    E_JPEG_BUFFER_HIGH = 1,
    E_JPEG_BUFFER_LOW  = 2
} EN_JPEG_BUFF_LOAD_TYPE;

typedef union
{
    struct
    {
        MS_U8 DQT : 1; //<has Quant Table?
        MS_U8 DHT : 1; //<has Huffman Table?
    };
    MS_U8 result; ///< wildcard for header check
} JPEG_HDR_CHK;

/******* pCtx *****/

typedef struct
{
    MS_U16 u16Xdensity; // inch or cm
    MS_U16 u16Ydensity; // inch or cm
    MS_U8  u8Unit;      // APP0 Mark units : 0-> no units, X and Y specify the pixel aspect ratio
} APP0_UNIT;

typedef struct
{
    union
    {
        struct
        {
            MS_U32 numerator;
            MS_U32 denominator;
        };

        struct
        {
            MS_S32 s_numerator;
            MS_S32 s_denominator;
        };
    };
} JPEG_RATIONAL;

typedef struct
{
    union
    {
        struct
        {
            MS_U32 data : 12;   ///< The value of VLI(2's compl & sign-ext)
            MS_U32 run : 4;     ///< run value
            MS_U32 EOB : 1;     ///< end of block
            MS_U32 altzz : 1;   ///< alternative zig-zag
            MS_U32 zzorder : 1; ///< zig-zag scan order
            MS_U32 trash : 13;  ///< reserved
        };

        struct
        {
            MS_U8 byte0; ///< byte0 of SVLD
            MS_U8 byte1; ///< byte1 of SVLD
            MS_U8 byte2; ///< byte2 of SVLD
            MS_U8 byte3; ///< byte3 of SVLD
        };
    };
} JPEG_SVLD;

//-----------------------------------------------------------------------------
/// @brief \b Struct \b Name: JPEG_COEFF_BUF
/// @brief \b Struct \b Description: The info of coefficient for JPEG decode
//-----------------------------------------------------------------------------
typedef struct
{
    MS_U8 *pu8Data;        ///< data of coefficient of DC, AC
    MS_U16 u16Block_num_x; ///< the number of block for width
    MS_U16 u16Block_num_y; ///< the number of block for height
    MS_U16 u16Block_size;  ///< block size
    MS_U8  u8Block_len_x;  ///< The width of block
    MS_U8  u8Block_len_y;  ///< The height of block
} JPEG_COEFF_BUF, *PJPEG_COEFF_BUF;

typedef struct JpegThumbnailInfo_s
{
    /******* Thumbnail related *******/
    MS_BOOL bThumbnailFound;          // header
    MS_BOOL bThumbnailAccessMode;     // dep bThumbnailFound
    MS_U32  u32ThumbnailOffset;       // dep header parse
    MS_U16  u16ThumbnailSize;         // header
    MS_BOOL bTiffBigEndian;           // header
    MS_U32  u32ThumbnailBufferOffset; // keep track of thumbnail buffer access address
    MS_U32  u16ThumbnailBufferSize;   // keep track of thumbnail buffer size
} JPEG_ThumbnailInfo_t;

typedef struct JpegDealInfo_s
{
    // Scale Down Factor
    MS_U8        u8ScaleDownFactor; // calc
    MS_U8        u8DownScaleRatio;  // relation to u8ScaleDownFactor
    MS_U8        u8Scan_type;       // Grey, Yh1v1, Yh1v2, Yh2v1, Yh2v2,
    MS_BOOL      bFirstRLE;         // run-length encoding
    JPEG_HDR_CHK HeadCheck;         // header has DQT/DHT ?
    // The original size before alignment
    MS_U16 u16NonAlignImg_x_size;
    MS_U16 u16NonAlignImg_y_size;

    // The width/height/pitch of image for displaying.
    MS_U16  u16AlignedImgWidth;
    MS_U16  u16AlignedImgPitch;
    MS_U16  u16AlignedImgHeight;
    MS_U16  u16AlignedImgPitch_H;
    MS_U8   u8LumaCi;
    MS_U8   u8ChromaCi;
    MS_U8   u8Chroma2Ci;
    MS_U16  u16Comp_h_blocks[JPEG_MAXCOMPONENTS]; // marco block
    MS_U16  u16Comp_v_blocks[JPEG_MAXCOMPONENTS];
    MS_U8   u8Blocks_per_mcu;
    MS_U32  u32Max_blocks_per_row;
    MS_U16  u16Mcus_per_row;
    MS_U16  u16Mcus_per_col;
    MS_U8   u8Mcu_org[JPEG_MAXBLOCKSPERMCU];
    MS_U8   gu8Max_mcu_x_size; /* MCU's max. X size in pixels */
    MS_U8   gu8Max_mcu_y_size; /* MCU's max. Y size in pixels */
    MS_U16  gu16Max_mcus_per_row;
    MS_U16  u16Max_blocks_per_mcu;
    MS_U16  u16Max_mcus_per_col;
    MS_U16  u16Restarts_left;
    MS_U16  u16Next_restart_num;
    MS_BOOL Progressive_ROI_flag; // CL82399
    MS_U16  ROI_width;            // CL82399
    MS_U32  u32MRCheckCount;
    /*===========================================================================*/
    MS_U32 u32RealMjpegBase;
    MS_U32 u32TrickyMjpegBase;
    /*===========================================================================*/
    // record data offset
    MS_U32                 u32DataOffset; // header
    MS_U32                 u32SOFOffset;  // for SEC request, they need to know the offset of SOF marker
    MS_BOOL                bIs3HuffTbl;
    MS_BOOL                bDifferentHL;
    EN_JPEG_BUFF_LOAD_TYPE u8PreLHFlag;
    JPEG_BLOCK_TYPE        s16dc_predicotr[3]; // header predictor
} JPEG_Deal_Info_t;

typedef struct JpegHeaderInfo_s
{
    // The width/height may be the thumbnail or original image size, it based on decoding mode
    MS_U16 u16Image_x_size;
    MS_U16 u16Image_y_size;

    // The original size of this JPEG file after alignment
    MS_U16 u16OriImg_x_size;
    MS_U16 u16OriImg_y_size;

    // header
    JPEG_HUFF_INFO stHuff_info[JPEG_MAXHUFFTABLES];
    JPEG_QUANT_TBL stQuantTables[JPEG_MAXQUANTTABLES]; /* pointer to quantization tables */

    MS_U8 u8Comps_in_frame;                  /* # of components in frame */
    MS_U8 u8Comp_h_samp[JPEG_MAXCOMPONENTS]; /* component's horizontal sampling factor */
    MS_U8 u8Comp_v_samp[JPEG_MAXCOMPONENTS]; /* component's vertical sampling factor */
    MS_U8 u8Comp_quant[JPEG_MAXCOMPONENTS];  /* component's quantization table selector */
    MS_U8 u8Comp_ident[JPEG_MAXCOMPONENTS];  /* component's ID */

    // The Luma and Chroma (YU) component ID, default is 1 & 2
    MS_U8 u8Comps_in_scan;                   /* # of components in scan */
    MS_U8 u8Comp_list[JPEG_MAXCOMPSINSCAN];  /* components in this scan */
    MS_U8 u8Comp_dc_tab[JPEG_MAXCOMPONENTS]; /* component's DC Huffman coding table selector */
    MS_U8 u8Comp_ac_tab[JPEG_MAXCOMPONENTS]; /* component's AC Huffman coding table selector */

    MS_U8   u8Spectral_start;  /* spectral selection start */
    MS_U8   u8Spectral_end;    /* spectral selection end   */
    MS_U8   u8Successive_low;  /* successive approximation low */
    MS_U8   u8Successive_high; /* successive approximation high */
    MS_BOOL bProgressive_flag; /*header parse msg */
    MS_S16  s16Bits_left;
    MS_U32  u32Bit_buf;
    MS_U16  u16Restart_interval;
    void *  pBlocks[JPEG_MAXBLOCKS]; /* list of all dynamically allocated blocks */
} JPEG_Header_Info_t;

typedef struct JpegExifDateTime_s
{
    MS_U32  u32Year;      ///< The year info of JPEG EXIF DataTime
    MS_U32  u32Month;     ///< The month info of JPEG EXIF DataTime
    MS_U32  u32Day;       ///< The day info of JPEG EXIF DataTime
    MS_U32  u32Hour;      ///< The hour info of JPEG EXIF DataTime
    MS_U32  u32Minute;    ///< The minute info of JPEG EXIF DataTime
    MS_U32  u32Second;    ///< The second info of JPEG EXIF DataTime
    MS_BOOL bHasDataTime; ///< JPEG EXIF DataTime info exist or not
} JPEG_EXIF_DATE_TIME;

//-----------------------------------------------------------------------------
/// @brief \b Enum \b Name: JPEG_EXIF_IFD0_Orientation
/// @brief \b Enum \b Description: The orientation of JPEG EXIF
//-----------------------------------------------------------------------------
typedef struct JpegExifInfo_s
{
    // The info of JPEG EXIF
    JPEG_EXIF_DATE_TIME      stEXIF_DateTime;
    EN_JPEG_EXIF_ORIENTATION eEXIF_Orientation;

#if SUPPORT_EXIF_EXTRA_INFO
    MS_U8         u8EXIF_Manufacturer[JPEG_MANUFACTURER_SIZE];
    MS_U8         u8EXIF_Model[JPEG_MODEL_SIZE];
    MS_U16        u16EXIF_Exposureprogram;
    MS_U16        u16EXIF_Flash;
    MS_U32        u32EXIF_ISOSpeedRatings;
    JPEG_RATIONAL stEXIF_ExposureTime;
    JPEG_RATIONAL stEXIF_FNumber;
    JPEG_RATIONAL stEXIF_ShutterSpeedValue;
    JPEG_RATIONAL stEXIF_ApertureValue;
    JPEG_RATIONAL stEXIF_ExposureBiasValue;
    JPEG_RATIONAL stEXIF_FocalLength;
    MS_U32        u32EXIF_ImageWidth;
    MS_U32        u32EXIF_ImageHeight;
#endif
} JPEG_EXIF_Info_t;

typedef struct JpegResInfo_s
{
    // max resolution setting
    MS_U16 jpeg_max_width; // config
    MS_U16 jpeg_max_height;

    // max progressive resolution setting
    MS_U16 jpeg_pro_max_width;
    MS_U16 jpeg_pro_max_height;

    // reserved
    MS_U16 jpeg_max_width_hd;
    MS_U16 jpeg_max_height_hd;

#if SUPPORT_GET_FULL_IMAGE_INFO
    // The full size of this JPEG file before alignment
    MS_U16  u16FullImage_x_size;
    MS_U16  u16FullImage_y_size;
    MS_BOOL bFullImageProgressive_flag;
#endif
} JPEG_Res_Info_t;

typedef struct JpegProgressiveInfo_s
{
    MS_U32          u32RLEOffset; //< offset to record the current RLE access address, progressive use only
    MS_U32          u32Block_y_mcu[JPEG_MAXCOMPONENTS];
    JPEG_HUFF_TBL   Huff_tbls[JPEG_MAXHUFFTABLES];
    PJPEG_COEFF_BUF DC_Coeffs[JPEG_MAXCOMPONENTS];
    PJPEG_COEFF_BUF AC_Coeffs[JPEG_MAXCOMPONENTS];
    MS_U32          u32Last_dc_val[JPEG_MAXCOMPONENTS];
    MS_U32          u32EOB_run;
} JPEG_Progressive_Info_t;

typedef struct JpegInputBuf_s
{
    void * pInputBufVA1;
    MS_PHY phyBufAddr1;      ///< jpeg buffer address1, for support ring buffer management
    MS_U32 u32InputBufSize1; ///< jpeg buffer size1,for support ring buffer management, must not zero
    void * pInputBufVA2;
    MS_PHY phyBufAddr2;      ///< jpeg buffer address2,for support ring buffer management, must not zero
    MS_U32 u32InputBufSize2; ///< jpeg buffer size2, for support ring buffer management, could be zero
} JPEG_InputBuf_t;

typedef struct JpegInternalBuffer_s
{
    MS_U32 u32InternalBufSize; ///< internal buffer size
    MS_PHY phyInternalBufAddr; ///< internal buffer address
    void * pInternalBufVA;
} JPEG_InternalBuffer_t;

typedef struct JpegOutputBuf_s
{
    void *pOutputBufVA[3];
    // MHAL_JPD_OUTBUF_MODE_e eOutBuf_Mode;
    MS_PHY phyOutputBufAddr[3]; ///< YUV buffer address
    MS_U32 u32Stride[3];

    MS_U32 outputRingBufHeight; ///< valid in case of E_MHAL_JPD_FRAME_HWRING_MODE
    MS_U32 outputRingTagId;     ///< valid in case of E_MHAL_JPD_FRAME_HWRING_MODE
    MS_U32 OutputBufSize;
} JPEG_OutputBuf_t;

typedef struct JpdDev_s JPD_Dev_t;

typedef struct JpdCtx_s
{
    MS_S32                 s32Index; // index of streams
    MS_BOOL                bIsMjpeg;
    MS_BOOL                bMHEG5;
    MS_BOOL                bEOF_flag;
    MS_U8                  u8Tem_flag;
    MS_U8                  u8FrameRdy;
    EN_JPEG_ERR_CODE       Error_code; // reason of error
    MS_U8                  JPD_DbgLevel;
    MS_U8                  JPD_Debug;
    NJPD_VERIFICATION_MODE JPD_MODE; // for imi emi mode
    MS_U32                 total_frame;
    MS_U32                 u32LineNum; // for EMI IMI line num

    MS_BOOL                  bWriteProtectEnable;
    MHAL_JPD_ScaleDownMode_e eScaleDownMode;

    MS_U8 *pu8In_buf_offset1;    // save read header offset, ponit to pInputBufVA1, current only process VA1
    MS_U8 *pu8In_buf_offset2;    // save read header offset, ponit to pInputBufVA2
    MS_U32 u32In_buf1_bytes;     // Number of remaining bytes
    MS_U32 u32In_buf2_bytes;     // Number of remaining bytes
    MS_U32 u32In_buf_left_bytes; // Number of remaining bytes, equals u32In_buf1_bytes + u32In_buf2_bytes
    MS_U32 u32Total_bytes_read;
    MS_U16 u16Total_lines_left; // total # lines left in image

    MS_U64 *pu64CmdqBuf;

    MHAL_JPD_DecodeMode_e u8DecodeType; // config main thumbnail mjpeg
                                        //    CamOsTsem_t           m_Sem;
                                        //    CamOsMutex_t          m_Mutex;

    JPD_Dev_t *pDev; // store struct JPD_Dev_t
    void *     p_handle;

    APP0_UNIT             APP0_UNIT_Data;
    MHAL_JPD_OutputFmt_e  eOutputFmt;
    MHAL_JPD_OutputMode_e eOutputMode;
    JPEG_InputBuf_t       stInBufInfo;
    JPEG_InternalBuffer_t stInternalBuf;
    JPEG_OutputBuf_t      stOutputBuf;
    JPEG_Res_Info_t       stResInfo;
    JPEG_ThumbnailInfo_t  stThumbnailInfo; // ThumbnailInfo
    JPEG_Header_Info_t    stHeadInfo;      // direct get msg form parse header
    JPEG_Deal_Info_t      stDealInfo;      // msg form header_info by calc
    JPEG_EXIF_Info_t      stExifInfo;      // msg form Exif
#if SUPPORT_PROGRESSIVE_MODE
    JPEG_Progressive_Info_t stProgressiveInfo;
#endif

    int i_state;
    int i_index;

    void (*CtxRelease)(void *);
    //    JpdCtxRC_t tRCStates[5];

} JPD_Ctx_t;

typedef struct JpdDevUserT_s
{
    JPD_Ctx_t *pCtx;
} JpdDevUserT_s;

/******* Dev *****/
typedef struct JpdDev_s
{
    int s32MagicNum;
    //    CamOsTsem_t  m_Sem;
    //    CamOsMutex_t m_Mutex;

    struct clk *pClock[JPD_CLOCKS_NR];

#if defined(CONFIG_CAM_CLK)
    void **pvJpdclk;
    u32    JpdParentCnt;
#endif
#if defined(__I_SW__)
    void *pClkHandle;
#endif
    unsigned int  nClkSelect; // clk select
    int           nRefCount;  // Reference count, how many instances created
    unsigned int  irq;        // IRQ number
    unsigned int  uClkEnabled;
    JpdDevUserT_s user[MAX_JPD_CHN_PER_DEV];
    MS_BOOL       bInit;
    u32           u_done_isrCnt;
    int           u32DevId;   // pDevice users
    unsigned long BaseDevReg; // pDevice users
    unsigned int  EOI;
    void (*funcIsrCB)(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_Status_e eStatus);

    void *p_asicip;
    u32   u_isrcnt;
    //    CamOsTimespec_t utilization_calc_start;
    //    CamOsTimespec_t utilization_calc_end;
    //    CamOsTimespec_t decode_start_time;
    //    CamOsTimespec_t isr_set_time;
    //    CamOsTimespec_t isr_trg_time;
    //    CamOsTimespec_t isr_end_time;
    //    CamOsAtomic_t   tPendingFrmCnt;
    u64 Trg_isr_ns;
    u64 End_isr_ns;
} JPD_Dev_t;

#endif //__DRV_JPD_ST_H__