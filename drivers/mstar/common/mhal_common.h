/*
* mhal_common.h- Sigmastar
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

#ifndef _MHAL_COMMON_H_
#define _MHAL_COMMON_H_


//-------------------------------------------------------------------------------------------------
//  System Data Type
//-------------------------------------------------------------------------------------------------

/// data type unsigned char, data length 1 byte
typedef unsigned char               MS_U8;                              // 1 byte
/// data type unsigned short, data length 2 byte
typedef unsigned short              MS_U16;                             // 2 bytes
/// data type unsigned int, data length 4 byte
typedef unsigned int               MS_U32;                             // 4 bytes
/// data type unsigned int, data length 8 byte
typedef unsigned long long          MS_U64;                             // 8 bytes
/// data type signed char, data length 1 byte
typedef signed char                 MS_S8;                              // 1 byte
/// data type signed short, data length 2 byte
typedef signed short                MS_S16;                             // 2 bytes
/// data type signed int, data length 4 byte
typedef signed int                 MS_S32;                             // 4 bytes
/// data type signed int, data length 8 byte
typedef signed long long            MS_S64;                             // 8 bytes
/// data type float, data length 4 byte
typedef float                       MS_FLOAT;                           // 4 bytes
/// data type pointer content 4 byte in arch32 and 8 byte in arch64
typedef unsigned long               MS_VIRT;                            // 4/8 bytes
/// data type hardware physical address
typedef MS_U64                      MS_PHYADDR;                         // 8 bytes
/// data type 64bit physical address
typedef MS_U64                      MS_PHY;                             // 8 bytes
/// data type size_t
typedef unsigned long long          MS_SIZE;                            // 8 bytes
/// definition for MS_BOOL
typedef unsigned char               MS_BOOL;
/// print type  MPRI_PHY
#define                             MPRI_PHY                            "%x"
/// print type  MPRI_PHY
#define                             MPRI_VIRT                           "%tx"


//-------------------------------------------------------------------------------------------------
//  Software Data Type
//-------------------------------------------------------------------------------------------------


/// definition for VOID
#ifndef VOID
typedef void                        VOID;
#endif
/// definition for FILEID
typedef MS_S32                      FILEID;



/*
#ifndef true
/// definition for true
#define true                        1
/// definition for false
#define false                       0
#endif
*/

#if !defined(TRUE) && !defined(FALSE)
/// definition for TRUE
#define TRUE                        1
/// definition for FALSE
#define FALSE                       0
#endif


#if defined(ENABLE) && (ENABLE!=1)
#warning ENALBE is not 1
#else
#define ENABLE                      1
#endif

#if defined(DISABLE) && (DISABLE!=0)
#warning DISABLE is not 0
#else
#define DISABLE                     0
#endif
//-------------------------------------------------------------------------------------------------
//  Software Data Type
//-------------------------------------------------------------------------------------------------

typedef MS_U16                      MHAL_AUDIO_DEV;


//-------------------------------------------------------------------------------------------------
// MHAL Interface Return  Value Define
//-------------------------------------------------------------------------------------------------


#define MHAL_SUCCESS    (0)
#define MHAL_FAILURE    (-1)
#define MHAL_ERR_ID  (0x80000000L + 0x20000000L)


/******************************************************************************
|----------------------------------------------------------------|
| 1 |   APP_ID   |   MOD_ID    | ERR_LEVEL |   ERR_ID            |
|----------------------------------------------------------------|
|<--><--7bits----><----8bits---><--3bits---><------13bits------->|
******************************************************************************/

#define MHAL_DEF_ERR( module, level, errid) \
    ((MS_S32)( (MHAL_ERR_ID) | ((module) << 16 ) | ((level)<<13) | (errid) ))

/*! @brief Error level*/
typedef enum
{
    E_MHAL_ERR_LEVEL_INFO,      /**< information*/
    E_MHAL_ERR_LEVEL_WARNING,   /**< warning*/
    E_MHAL_ERR_LEVEL_ERROR,     /**< error*/
    E_MHAL_ERR_LEVEL_BUTT       /**< maximum error level*/
} MHAL_ErrLevel_e;

/*! @brief Error code*/
typedef enum
{
    E_MHAL_ERR_INVALID_DEVID = 1,   /**< invlalid device ID*/
    E_MHAL_ERR_INVALID_CHNID = 2,   /**< invlalid channel ID*/
    E_MHAL_ERR_ILLEGAL_PARAM = 3,   /**< at lease one parameter is illagal. eg, an illegal enumeration value*/
    E_MHAL_ERR_EXIST         = 4,   /**< resource exists*/
    E_MHAL_ERR_UNEXIST       = 5,   /**< resource unexists*/
    E_MHAL_ERR_NULL_PTR      = 6,   /**< using a NULL point*/
    E_MHAL_ERR_NOT_CONFIG    = 7,   /**< try to enable or initialize system, device or channel, before configing attribute*/
    E_MHAL_ERR_NOT_SUPPORT   = 8,   /**< operation or type is not supported by NOW*/
    E_MHAL_ERR_NOT_PERM      = 9,   /**< operation is not permitted. eg, try to change static attribute*/
    E_MHAL_ERR_NOMEM         = 12,  /**< failure caused by malloc memory*/
    E_MHAL_ERR_NOBUF         = 13,  /**< failure caused by malloc buffer*/
    E_MHAL_ERR_BUF_EMPTY     = 14,  /**< no data in buffer*/
    E_MHAL_ERR_BUF_FULL      = 15,  /**< no buffer for new data*/
    E_MHAL_ERR_SYS_NOTREADY  = 16,  /**< System is not ready,maybe not initialed or loaded. Returning the error code when opening a device file failed.*/
    E_MHAL_ERR_BADADDR       = 17,  /**< bad address. eg. used for copy_from_user & copy_to_user*/
    E_MHAL_ERR_BUSY          = 18,  /**< resource is busy. eg. destroy a venc chn without unregister it*/
    E_MHAL_ERR_BUTT          = 63,  /**< maxium code, private error code of all modules must be greater than it*/
} MHAL_ErrCode_e;

/// data type 64bit physical address
typedef unsigned long long                       PHY;        // 8 bytes

/*! @brief Pixel format*/
typedef enum
{
    E_MHAL_PIXEL_FRAME_YUV422_YUYV = 0,             /**< yuv422 yuyv*/
    E_MHAL_PIXEL_FRAME_ARGB8888,                    /**< argb8888*/
    E_MHAL_PIXEL_FRAME_ABGR8888,                    /**< abgr8888*/
    E_MHAL_PIXEL_FRAME_BGRA8888,                    /**< bgra8888*/

    E_MHAL_PIXEL_FRAME_RGB565,                      /**< rgb565*/
    E_MHAL_PIXEL_FRAME_ARGB1555,                    /**< argb1555*/
    E_MHAL_PIXEL_FRAME_ARGB4444,                    /**< argb4444*/
    E_MHAL_PIXEL_FRAME_I2,                          /**< i2*/
    E_MHAL_PIXEL_FRAME_I4,                          /**< i4*/
    E_MHAL_PIXEL_FRAME_I8,                          /**< i8*/

    E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_422,          /**< semiplanar 422*/
    E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420,          /**< semiplanar 420*/
    E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420_NV21,     /**< semiplanar 420 nv21*/
    // mstar mdwin/mgwin
    E_MHAL_PIXEL_FRAME_YUV_MST_420,                 /**< mst 420*/
    E_MHAL_PIXEL_FRAME_YUV422_UYVY,                 /**< yuv422 uyvy*/
    E_MHAL_PIXEL_FRAME_YUV422_YVYU,                 /**< yuv422 yvyu*/
    E_MHAL_PIXEL_FRAME_YUV422_VYUY,                 /**< yuv422 vyuy*/
    E_MHAL_PIXEL_FRAME_YUV422_PLANAR,               /**< yuv422 planar*/
    E_MHAL_PIXEL_FRAME_YUV420_PLANAR,               /**< yuv420 planar*/

    //vdec mstar private video format
    E_MHAL_PIXEL_FRAME_YC420_MSTTILE1_H264,         /**< yc420 msttile1 h264*/
    E_MHAL_PIXEL_FRAME_YC420_MSTTILE2_H265,         /**< yc420 msttile2 h265*/
    E_MHAL_PIXEL_FRAME_YC420_MSTTILE3_H265,         /**< yc420 msttile3 h265*/

    E_MHAL_PIXEL_FRAME_RGB_BAYER_8BPP_RG,           /**< bayer 8bit RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_8BPP_GR,           /**< bayer 8bit GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_8BPP_BG,           /**< bayer 8bit BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_8BPP_GB,           /**< bayer 18bit GB*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_10BPP_RG,          /**< bayer 10bit RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_10BPP_GR,          /**< bayer 10bit GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_10BPP_BG,          /**< bayer 10bit BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_10BPP_GB,          /**< bayer 10bit GB*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_12BPP_RG,          /**< bayer 12bit RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_12BPP_GR,          /**< bayer 12bit GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_12BPP_BG,          /**< bayer 12bit BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_12BPP_GB,          /**< bayer 12bit GB*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_14BPP_RG,          /**< bayer 14bit RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_14BPP_GR,          /**< bayer 14bit GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_14BPP_BG,          /**< bayer 14bit BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_14BPP_GB,          /**< bayer 14bit GB*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_16BPP_RG,          /**< bayer 16bit RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_16BPP_GR,          /**< bayer 16bit GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_16BPP_BG,          /**< bayer 16bit BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_16BPP_GB,          /**< bayer 16bit GB*/
//////
    E_MHAL_PIXEL_FRAME_RGB_BAYER_RG,                /**< bayer RG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_GR,                /**< bayer GR*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_BG,                /**< bayer BG*/
    E_MHAL_PIXEL_FRAME_RGB_BAYER_GB,                /**< bayer GB*/

    E_MHAL_PIXEL_FRAME_RGBIR_R0,                    /**< rgbir R0*/
    E_MHAL_PIXEL_FRAME_RGBIR_G0,                    /**< rgbir G0*/
    E_MHAL_PIXEL_FRAME_RGBIR_B0,                    /**< rgbir B0*/
    E_MHAL_PIXEL_FRAME_RGBIR_G1,                    /**< rgbir G1*/
    E_MHAL_PIXEL_FRAME_RGBIR_G2,                    /**< rgbir G2*/
    E_MHAL_PIXEL_FRAME_RGBIR_I0,                    /**< rgbir I0*/
    E_MHAL_PIXEL_FRAME_RGBIR_G3,                    /**< rgbir G3*/
    E_MHAL_PIXEL_FRAME_RGBIR_I1,                    /**< rgbir I1*/
    E_MHAL_PIXEL_FRAME_YUV_SEMIPLANAR_420_SSTILE,   /**< semiplanar 420 sstile*/
    E_MHAL_PIXEL_FRAME_RGB101010,                   /**< rgb101010*/
    E_MHAL_PIXEL_FRAME_FORMAT_MAX,                  /**< maximum pixel format*/
} MHalPixelFormat_e;

/*! @brief Data precision*/
typedef enum
{
    E_MHAL_DATA_PRECISION_8BPP,         /**< 8bits per pixel*/
    E_MHAL_DATA_PRECISION_10BPP,        /**< 10bits per pixel*/
    E_MHAL_DATA_PRECISION_12BPP,        /**< 12bits per pixel*/
    E_MHAL_DATA_PRECISION_14BPP,        /**< 14bits per pixel*/
    E_MHAL_DATA_PRECISION_16BPP,        /**< 16bits per pixel*/
    E_MHAL_DATA_PRECISION_16BPP_Mode2,  /**< 16bits per pixel (mode2)*/
    E_MHAL_DATA_PRECISION_MAX,          /**< maximum data precision*/
} MHalDataPrecision_e;

/*! @brief Bayer ID*/
typedef enum
{
    E_MHAL_RGB_BAYER_RG = 0,    /**< RGRG...;GBGB...*/
    E_MHAL_RGB_BAYER_GR = 1,    /**< GRGR...;BGBG...*/
    E_MHAL_RGB_BAYER_GB = 2,    /**< GBGB...;RGRG...*/
    E_MHAL_RGB_BAYER_BG = 3     /**< BGBG...;GRGR...*/
} MHalRGBBayerID_e;

typedef enum
{
    E_MHAL_COMPRESS_MODE_NONE,//no compress
    E_MHAL_COMPRESS_MODE_SEG,//compress unit is 256 bytes as a segment
    E_MHAL_COMPRESS_MODE_LINE,//compress unit is the whole line
    E_MHAL_COMPRESS_MODE_FRAME,//compress unit is the whole frame
    E_MHAL_COMPRESS_MODE_TO_8BIT,
    E_MHAL_COMPRESS_MODE_TO_6BIT,
    E_MHAL_COMPRESS_MODE_AFBC,
    E_MHAL_COMPRESS_MODE_SFBC0, // DSC
    E_MHAL_COMPRESS_MODE_SFBC1, // Encoder Fbc
    E_MHAL_COMPRESS_MODE_SFBC2, // Decoder Fbc
    E_MHAL_COMPRESS_MODE_BUTT, //number
    E_MHAL_COMPRESS_MODE_MAX, //number
} MHalPixelCompressMode_e;

/*! @brief Frame scan mode*/
typedef enum
{
    E_MHAL_FRAME_SCAN_MODE_PROGRESSIVE = 0x0,   /**< progressive mode*/
    E_MHAL_FRAME_SCAN_MODE_INTERLACE   = 0x1,   /**< interlace mode*/
    E_MHAL_FRAME_SCAN_MODE_MAX,                 /**< maximum scan mode*/
} MHalFrameScanMode_e;

/*! @brief Field type*/
typedef enum
{
    E_MHAL_FIELDTYPE_NONE,      /**< field type none*/
    E_MHAL_FIELDTYPE_TOP,       /**< field type top*/
    E_MHAL_FIELDTYPE_BOTTOM,    /**< field type bottom*/
    E_MHAL_FIELDTYPE_BOTH,      /**< field type both*/
    E_MHAL_FIELDTYPE_NUM        /**< maximum field type*/
} MHalFieldType_e;

/*! @brief Window rectangle*/
typedef struct MHalWindowRect_s
{
    MS_U16 u16X;        /**< x start*/
    MS_U16 u16Y;        /**< y start*/
    MS_U16 u16Width;    /**< width*/
    MS_U16 u16Height;   /**< height*/
}MHalWindowRect_t;

/*! @brief HDR type*/
typedef enum
{
    E_MHAL_HDR_TYPE_OFF,        /**< HDR off*/
    E_MHAL_HDR_TYPE_VC,         /**< virtual channel*/
    E_MHAL_HDR_TYPE_DOL,        /**< sony DOL*/
    E_MHAL_HDR_TYPE_EMBEDDED,   /**< embedded*/
    E_MHAL_HDR_TYPE_LI,         /**< line interlaced*/
    E_MHAL_HDR_TYPE_MULTI_VC,   /**< non HDR, multi virtual channel YUV*/
    E_MHAL_HDR_TYPE_MAX         /**< maximum HDR type*/
} MHalHDRType_e;

/*! @brief HDR virtual channel source*/
typedef enum
{
    E_MHAL_HDR_SOURCE_VC0,  /**< virtual channel 0*/
    E_MHAL_HDR_SOURCE_VC1,  /**< virtual channel 1*/
    E_MHAL_HDR_SOURCE_VC2,  /**< virtual channel 2*/
    E_MHAL_HDR_SOURCE_VC3,  /**< virtual channel 3*/
    E_MHAL_HDR_SOURCE_MAX   /**< maximum virtual channel*/
} MHal_HDRSource_e;

/*! @brief Sensor pad ID*/
typedef enum
{
    E_MHAL_SNR_PAD_ID_0 = 0,        /**< sensor pad ID 0*/
    E_MHAL_SNR_PAD_ID_1 = 1,        /**< sensor pad ID 1*/
    E_MHAL_SNR_PAD_ID_2 = 2,        /**< sensor pad ID 2*/
    E_MHAL_SNR_PAD_ID_3 = 3,        /**< sensor pad ID 3*/
    E_MHAL_SNR_PAD_ID_4 = 4,        /**< sensor pad ID 4*/
    E_MHAL_SNR_PAD_ID_5 = 5,        /**< sensor pad ID 5*/
    E_MHAL_SNR_PAD_ID_6 = 6,        /**< sensor pad ID 6*/
    E_MHAL_SNR_PAD_ID_7 = 7,        /**< sensor pad ID 7*/
    E_MHAL_SNR_PAD_ID_MAX,          /**< maximum sensor pad ID*/
    E_MHAL_SNR_PAD_ID_NA = 0xFF,    /**< sensor pad ID NA*/
} MHal_SNR_PAD_ID_e;

/*! @brief MCLK selection*/
typedef enum
{
    E_MHAL_MCLK_12MHZ,  /**< 12MHz*/
    E_MHAL_MCLK_18MHZ,  /**< 18MHz*/
    E_MHAL_MCLK_27MHZ,  /**< 27MHz*/
    E_MHAL_MCLK_36MHZ,  /**< 36MHz*/
    E_MHAL_MCLK_54MHZ,  /**< 54MHz*/
    E_MHAL_MCLK_108MHZ, /**< 108MHz*/
    E_MHAL_MCLK_MAX     /**< maximum mclk*/
} MHal_VIF_MclkSource_e;

/*! @brief Interface mode*/
typedef enum
{
    E_MHAL_VIF_MODE_BT656,              /**< bt656 interface*/
    E_MHAL_VIF_MODE_DIGITAL_CAMERA,     /**< parallel interface*/
    E_MHAL_VIF_MODE_BT1120_STANDARD,    /**< bt1120 interface*/
    E_MHAL_VIF_MODE_BT1120_INTERLEAVED, /**< interleaved bt1120 interface*/
    E_MHAL_VIF_MODE_MIPI,               /**< mipi interface*/
    E_MHAL_VIF_MODE_LVDS,               /**< lvds interface*/
    E_MHAL_VIF_MODE_MAX                 /**< maximum interface*/
} MHal_VIF_IntfMode_e;

/*! @brief Clock edge*/
typedef enum
{
    E_MHAL_VIF_CLK_EDGE_SINGLE_UP,      /**< rising clock edge*/
    E_MHAL_VIF_CLK_EDGE_SINGLE_DOWN,    /**< falling clock edge*/
    E_MHAL_VIF_CLK_EDGE_DOUBLE,         /**< dual clock edge*/
    E_MHAL_VIF_CLK_EDGE_MAX             /**< maximum clock edge*/
} MHal_VIF_ClkEdge_e;

/*! @brief Pin polarity*/
typedef enum
{
    E_MHAL_VIF_PIN_POLAR_POS,   /**< positive polarity*/
    E_MHAL_VIF_PIN_POLAR_NEG    /**< negative polarity*/
} MHal_VIF_Polar_e;

/*! @brief Synchronization attribute*/
typedef struct Mhal_VIF_SignalPolarity_s
{
    MHal_VIF_Polar_e   eVsyncPolarity;  /**< vsync polarity*/
    MHal_VIF_Polar_e   eHsyncPolarity;  /**< hsync polarity*/
    MHal_VIF_Polar_e   ePclkPolarity;   /**< pclk polarity*/
    MS_U32   VsyncDelay;                /**< vsync delay*/
    MS_U32   HsyncDelay;                /**< hsync delay*/
    MS_U32   PclkDelay;                 /**< pclk delay*/
} MHal_VIF_SyncAttr_t;

#endif // _MHAL_COMMON_H_
