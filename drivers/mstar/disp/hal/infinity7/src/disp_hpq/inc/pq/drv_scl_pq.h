/*
* drv_scl_pq.h- Sigmastar
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

#ifndef _DRVPQ_H_
#define _DRVPQ_H_

#ifdef __cplusplus
extern "C"
{
#endif

    //-------------------------------------------------------------------------------------------------
    //  Macro and Define
    //-------------------------------------------------------------------------------------------------

#define MSIF_PQ_TAG        \
    {                      \
        'M', 'S', 'I', 'F' \
    } // MSIF
#define MSIF_PQ_CLASS \
    {                 \
        '0', '0'      \
    } // DRV/API (DDI)
#define MSIF_PQ_CUS  0x0000
#define MSIF_PQ_MOD  0x0000
#define MSIF_PQ_CHIP 0x0000
#define MSIF_PQ_CPU  '0'
#define MSIF_PQ_OS   '0'

//  library information

/// the PQ lib code
#define MSIF_PQ_LIB_CODE   \
    {                      \
        'P', 'Q', '_', '_' \
    }
/// the PQ lib version
#define MSIF_PQ_LIBVER \
    {                  \
        '0', '0'       \
    }
/// the PQ build number
#define MSIF_PQ_BUILDNUM \
    {                    \
        '0', '7'         \
    }
/// the PQ CL
#define MSIF_PQ_CHANGELIST                     \
    {                                          \
        '0', '0', '3', '5', '3', '5', '7', '6' \
    }

/// the PQ driver version
#define PQ_DRV_VERSION                               /* Character String for DRV/API version */ \
    MSIF_PQ_TAG,                                     /* 'MSIF'                               */ \
        MSIF_PQ_CLASS,                               /* '00'                                 */ \
        MSIF_PQ_CUS,                                 /* 0x0000                               */ \
        MSIF_PQ_MOD,                                 /* 0x0000                               */ \
        MSIF_PQ_CHIP, MSIF_PQ_CPU, MSIF_PQ_LIB_CODE, /* IP__                                */  \
        MSIF_PQ_LIBVER,                              /* 0.0 ~ Z.Z                           */  \
        MSIF_PQ_BUILDNUM,                            /* 00 ~ 99                             */  \
        MSIF_PQ_CHANGELIST,                          /* CL#                                 */  \
        MSIF_PQ_OS
    //-------------------------------------------------------------------------------------------------
    //  Type and Structure
    //-------------------------------------------------------------------------------------------------

#define ENABLE_PQ_LOAD_TABLE_INFO 0 ///< Pr init PQ load table info

    /**
     *  HDMI COLOR FORMAT
     */
    typedef enum
    {
        /// RGB
        E_PQ_HDMI_COLOR_RGB,
        /// YUV422
        E_PQ_HDMI_COLOR_YUV_422,
        /// YUV444
        E_PQ_HDMI_COLOR_YUV_444,
    } PQ_HDMI_COLOR_TYPE;

    /**
     *  Different PQ Types for PQ information merged to code, need to re-compile if changed PQ setting.
     */
    typedef enum
    {
        /// Select main window standard PQ setting for each input source/resolution
        E_PQ_BIN_STD_MAIN     = 0,
        E_PQ_BIN_STD_SUB      = 1,
        E_PQ_BIN_STD_SC1_MAIN = 2,
        E_PQ_BIN_STD_SC2_MAIN = 3,
        E_PQ_BIN_STD_SC2_SUB  = 4,
        E_PQ_BIN_EXT_MAIN     = 5,
        E_PQ_BIN_EXT_SUB      = 6,
        E_PQ_BIN_EXT_SC1_MAIN = 7,
        E_PQ_BIN_EXT_SC2_MAIN = 8,
        E_PQ_BIN_EXT_SC2_SUB  = 9,
        /// The max number of PQ Bin
        E_MAX_PQ_BIN_NUM,
    } PQ_BIN_TYPE;

    /**
     *  Different PQ Types for PQ information merged to binary, need to modify binary if changed PQ setting.
     */
    typedef enum
    {
        /// Select main window standard PQ setting for each input source/resolution
        E_PQ_TEXT_BIN_STD_MAIN,
        /// Select sub window standard PQ setting for each input source/resolution
        E_PQ_TEXT_BIN_STD_SUB,
        /// The max number of PQ Text Bin
        E_PQ_TEXT_BIN_MAX,
    } PQ_TEXT_BIN_TYPE;

    typedef enum
    {
        /// PQ VD sampling is on
        E_PQ_VD_SAMPLING_ON,
        /// PQ VD sampling is off
        E_PQ_VD_SAMPLING_OFF,
        /// the max number of PQ VD sampling
        E_PQ_VD_SAMPLING_NUM,
    } PQ_VD_SAMPLING_TYPE;

    /**
     *  PQ library idetifcation information.
     */
    typedef struct DevPQBinInfo_s
    {
        /// ID
        u8 u8PQID;
        /// Virtual address
        void *pPQBin_AddrVirt;
        /// Physical address
        ss_phys_addr_t PQBin_PhyAddr;
        u32            PQ_Bin_BufSize;
    } DevPQBinInfo_t;

    /**
     *  PQ initialize information.
     */
    typedef struct DrvPQInitInfo_s
    {
        /// DDR2
        u8 bDDR2;
        /// MIU0 mem size
        u32 u32miu0em_size;
        /// MIU1 mem size
        u32 u32miu1em_size;
        /// DDR Frequency
        u32 u32DDRFreq;
        /// Bus width
        u8 u8BusWidth;
        /// Panel width
        u16 u16PnlWidth;
        /// Panel height
        u16 u16PnlHeight;
        /// Panel Vtotal
        u16 u16Pnl_vtotal;
        /// OSD Hsize
        u16 u16OSD_hsize;
        /// Bin count
        u8 u8PQBinCnt;
        /// Text Bin count
        u8 u8PQTextBinCnt;
        /// PQ Bin informaton array
        DevPQBinInfo_t stPQBinInfo[E_MAX_PQ_BIN_NUM];
        /// PQ Text bin information array
        DevPQBinInfo_t stPQTextBinInfo[E_PQ_TEXT_BIN_MAX];
    } DrvPQInitInfo_t;
    typedef struct DrvPQSrcTypeInfo_s
    {
        u8  u8Device;
        u8  u8Interface;
        u16 u16Width;
        u16 u16Height;
    } DrvPQSrcTypeInfo_t;

    typedef struct DrvPQSrcTypeConfig_s
    {
        u16                 u16SrcTypeNum;
        DrvPQSrcTypeInfo_t *pSrcTypeInfoTbl;
    } DrvPQSrcTypeConfig_t;

#define PQ_DBG_PQTAB   (0x0001) ///< Debug PQ Table
#define PQ_DBG_SRULE   (0x0002) ///< Debug S RULE
#define PQ_DBG_CSCRULE (0x0004) ///< Debug CSC RULE
#define PQ_DBG_GRULE   (0x0008) ///< Debug G RULE
#define PQ_DBG_BW      (0x0010) ///< Debug BANDWIDTH
#define PQ_DBG_MADI    (0x0020) ///< Debug MADI
#define PQ_DBG_INFO    (0x0040) ///< Debug INFO
#define PQ_DBG_IOCTL   (0x0080) ///< Debug MADI
#define PQ_DBG_P2P     (0x0100) ///< Debug point to point
#define PQ_DBG_COLOR   (0x0200) ///< Debug PQ Color

    //-------------------------------------------------------------------------------------------------
    //  Function and Variable
    //-------------------------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------------------
    /// Initial function
    /// @param  pstPQInitInfo                 \b IN: point to PQ initial information
    //-------------------------------------------------------------------------------------------------
    u8 DrvPQInit(DrvPQInitInfo_t *pstPQInitInfo, void *pCtx);

    //-------------------------------------------------------------------------------------------------
    /// Exit function
    /// @retval Success or fail
    //-------------------------------------------------------------------------------------------------
    u8 DrvPQExit(DrvPQInitInfo_t *pstPQInitInfo, void *pCtx);

    //-------------------------------------------------------------------------------------------------
    /// Get Match SourceType
    /// @retval Success or fail
    //-------------------------------------------------------------------------------------------------
    u8 DrvPQGetMatchSoruceType(E_PQ_WIN_TYPE_e eWindow, DrvPQSrcTypeInfo_t *pSrcTypeInfo, u16 *pu16SrcType);

    //-------------------------------------------------------------------------------------------------
    /// Init SourceType Table
    /// @retval Success or fail
    //-------------------------------------------------------------------------------------------------
    u8 DrvPQInitSourceTypeTable(u32 u32Dev);
    u8 DrvPQFreeSourceTypeTable(u32 u32Dev);

    //-------------------------------------------------------------------------------------------------
    /// Set source type
    /// @param  eWindow                    \b IN: TBD
    /// @param  u16SrcType          \b IN: input source type
    //-------------------------------------------------------------------------------------------------
    void DrvPQSetSrcType(u32 u32Dev, u16 u16SrcType);

    //-------------------------------------------------------------------------------------------------
    /// Get source type
    /// @param  eWindow                    \b IN: TBD
    /// @return Main window or Sub window
    //-------------------------------------------------------------------------------------------------
    u16 DrvPQGetSrcType(u32 u32Dev);

    //-------------------------------------------------------------------------------------------------
    /// Set Pnl Id
    /// @param  eWindow           \b IN: TBD
    /// @param  u16PnlId          \b IN: Pnl Id
    //-------------------------------------------------------------------------------------------------

    void DrvPQSetPnlId(E_PQ_WIN_TYPE_e eWindow, u16 u16PnlId);

    //-------------------------------------------------------------------------------------------------
    /// Get Pnl Num
    /// @param  eWindow           \b IN: TBD
    //-------------------------------------------------------------------------------------------------
    u16 DrvPQGetPnlNum(E_PQ_WIN_TYPE_e eWindow);
    //-------------------------------------------------------------------------------------------------
    /// Load settings
    /// @param  eWindow                       \b IN: TBD
    //-------------------------------------------------------------------------------------------------
    void DrvPQLoadSettings(u32 u32Dev);

    //-------------------------------------------------------------------------------------------------
    /// Load settings by Data
    /// @param  eWindow                       \b IN: TBD
    /// @param  pData                         \b IN: TBD
    /// @param  u16DataSize                   \b IN: TBD
    //-------------------------------------------------------------------------------------------------

    void DrvPQLoadTableData(E_PQ_WIN_TYPE_e eWindow, u16 u16TabIdx, u16 u16PQIPIdx, u8 *pTable, u16 u16TableSize);

    u16 DrvPQGetTableIndex(u32 u32Dev, u16 u16IPIdx);

    u16 DrvPQGetIPRegCount(u16 u16PQIPIdx);
    u16 DrvPQGetIPRegCountSub(u16 u16PQIPIdx);

    //-------------------------------------------------------------------------------------------------
    /// Load table
    /// @param  eWindow                         \b IN: TBD
    /// @param  u16TabIdx                       \b IN: TBD
    /// @param  u16PQIPIdx                      \b IN: TBD
    //-------------------------------------------------------------------------------------------------
    void DrvPQLoadTable(E_PQ_WIN_TYPE_e eWindow, u16 u16TabIdx, u16 u16PQIPIdx);

#ifdef __cplusplus
}
#endif

#endif /* _DRVPQ_H_ */
