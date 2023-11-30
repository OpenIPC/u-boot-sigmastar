/*
* mhal_cmdq.h- Sigmastar
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

/**
 * @file          mhal_cmdq.h
 * @brief         CMDQ Driver APIs
 */

#ifndef __MHAL_CMD_SERVICE_HH__
#define __MHAL_CMD_SERVICE_HH__

#include "mhal_common.h"

/**
 * @note CMDQ buffer offset, used for MemCmdq
 */
#define MHAL_CMDQ_SEEK_END                (0xFFFFFFFF)  /**< Seek to buffer end position */
#define MHAL_CMDQ_SEEK_BEG                (0)           /**< Seek to buffer start position */

/**
 * @note Old define for CMDQ timeout
 */
#define MHAL_CMDQ_POLLNEQ_TIMEOUT        (0x1<<8)       /**< Poll neq timeout */
#define MHAL_CMDQ_POLLEQ_TIMEOUT         (0x1<<9)       /**< Poll eq timeout */
#define MHAL_CMDQ_WAIT_TIMEOUT           (0x1<<10)      /**< Wait event timeout */
#define MHAL_CMDQ_WRITE_TIMEOUT          (0x1<<12)      /**< Write timeout */

/**
 * @note New define for CMDQ timeout
 */
#define MHAL_CMDQ_ALL_TIMEOUT            (0x1<<8)       /**< All timeout */

/**
 * @note CMDQ error status
 */
#define MHAL_CMDQ_ERROR_STATUS           (MHAL_CMDQ_POLLNEQ_TIMEOUT|MHAL_CMDQ_POLLEQ_TIMEOUT |MHAL_CMDQ_WAIT_TIMEOUT|MHAL_CMDQ_WRITE_TIMEOUT)


/**
 * MHAL_CMDQ_EarlyRunStatus_e
 * @note CMDQ early run status
 */
typedef enum
{
    E_CMDQ_EARLYRUN_BUSY = 0XBC5D,       /**< CMDQ busy status */
    E_CMDQ_EARLYRUN_DONE = 0x9090,       /**< CMDQ idle status */
} MHAL_CMDQ_EarlyRunStatus_e;

/**
 * MHAL_CMDQ_ByteSel_e
 * @note Direct mode CMDQ byte mode switch
 */
typedef enum
{
    E_CMDQ_BYTE_SEL_ALL   = 0,           /**< Disble one byte mode */
    E_CMDQ_BYTE_SEL_HBYTE = 1,           /**< Disble one byte mode */
    E_CMDQ_BYTE_SEL_LBYTE = 2            /**< Enable one byte mode */
} MHAL_CMDQ_ByteSel_e;



/**
 * MHAL_CMDQ_TYPE_e
 * @note CMDQ type
 */
typedef enum
{
    E_MHAL_CMDQ_TYPE_ASIC           =   0,  /**< Original CMDQ type */
    E_MHAL_CMDQ_TYPE_MEMSCRIPT      =   1,  /**< MemScript CMDQ type */
    E_MHAL_CMDQ_TYPE_MAX                    /**< CMDQ type max limitation */
} MHAL_CMDQ_TYPE_e;

/**
 * MHAL_CMDQ_EventId_e
 * @note CMDQ trigger event ID table.                    \n
 *       Don't use physical bit operation here,          \n
 *       add enum number for new event ID, should sync with HAL_CMDQ_EventId
 */
typedef enum
{
    E_MHAL_CMDQEVE_DUMMY15          =   1,  /**< DUMMY15 */
    E_MHAL_CMDQEVE_DUMMY14          =   2,  /**< DUMMY14 */
    E_MHAL_CMDQEVE_DUMMY13          =   3,  /**< DUMMY13 */
    E_MHAL_CMDQEVE_DUMMY12          =   4,  /**< DUMMY12 */
    E_MHAL_CMDQEVE_DUMMY11          =   5,  /**< DUMMY11 */
    E_MHAL_CMDQEVE_DUMMY10          =   6,  /**< DUMMY10 */
    E_MHAL_CMDQEVE_DUMMY09          =   7,  /**< DUMMY09 */
    E_MHAL_CMDQEVE_DUMMY08          =   8,  /**< DUMMY08 */
    E_MHAL_CMDQEVE_DUMMY07          =   9,  /**< DUMMY07 */
    E_MHAL_CMDQEVE_DUMMY06          =  10,  /**< DUMMY06 */
    E_MHAL_CMDQEVE_DUMMY05          =  11,  /**< DUMMY05 */
    E_MHAL_CMDQEVE_DUMMY04          =  12,  /**< DUMMY04 */
    E_MHAL_CMDQEVE_DUMMY03          =  13,  /**< DUMMY03 */
    E_MHAL_CMDQEVE_DUMMY02          =  14,  /**< DUMMY02 */
    E_MHAL_CMDQEVE_DUMMY01          =  15,  /**< DUMMY01 */
    E_MHAL_CMDQEVE_DUMMY00          =  16,  /**< DUMMY00 */
    E_MHAL_CMDQEVE_DIP_TRIG         =  17,  /**< DIP_TRIG */
    E_MHAL_CMDQEVE_MFE_TRIG         =  18,  /**< MFE_TRIG */
    E_MHAL_CMDQEVE_MHE_TRIG         =  19,  /**< MHE_TRIG */
    E_MHAL_CMDQEVE_DMAGEN_TRIGGER   =  20,  /**< DMAGEN_TRIGGER */
    E_MHAL_CMDQEVE_JPE_TRIG         =  21,  /**< JPE_TRIG */
    E_MHAL_CMDQEVE_DMA_DONE         =  22,  /**< DMA_DONE */
    E_MHAL_CMDQEVE_SC_TRIG1         =  23,  /**< SC_TRIG1 */
    E_MHAL_CMDQEVE_SC_TRIG0         =  24,  /**< SC_TRIG0 */
    E_MHAL_CMDQEVE_ISP_TRIG         =  25,  /**< ISP_TRIG */
#if 1 // I6e
    E_MHAL_CMDQEVE_LDC_TRIG         =  26,  /**< LDC_TRIG */
    E_MHAL_CMDQEVE_DIP_CMDQ_INT1    =  27,  /**< DIP_CMDQ_INT1 */
    E_MHAL_CMDQEVE_DIP_INT1         =  28,  /**< DIP_INT1 */
    E_MHAL_CMDQEVE_IVE_CMDQ         =  29,  /**< IVE_CMDQ */
#endif
#if 1 // I7
    E_MHAL_CMDQEVE_BDMA0_TRIG       =  30,  /**< BDMA0_TRIG */
    E_MHAL_CMDQEVE_BDMA1_TRIG       =  31,  /**< BDMA1_TRIG */
    E_MHAL_CMDQEVE_MIPI_TX_CSI_INT  =  32,  /**< MIPI_TX_CSI_INT */
    E_MHAL_CMDQEVE_JPE0_TRIG        =  33,  /**< JPE0_TRIG */
    E_MHAL_CMDQEVE_JPE1_TRIG        =  34,  /**< JPE1_TRIG */
    E_MHAL_CMDQEVE_JPD0_TRIG        =  35,  /**< JPD0_TRIG */
    E_MHAL_CMDQEVE_JPD1_TRIG        =  36,  /**< JPD1_TRIG */
    E_MHAL_CMDQEVE_DISP_0_INT       =  37,  /**< DISP_0_INT */
    E_MHAL_CMDQEVE_DISP_1_INT       =  38,  /**< DISP_1_INT */
    E_MHAL_CMDQEVE_DISP_2_INT       =  39,  /**< DISP_2_INT */
    E_MHAL_CMDQEVE_LDC_FEYE_TRIG0   =  40,  /**< LDC_FEYE_TRIG0 */
    E_MHAL_CMDQEVE_LDC_FEYE_TRIG1   =  41,  /**< LDC_FEYE_TRIG1 */
    E_MHAL_CMDQEVE_SC_LV_TRIG0      =  42,  /**< SC_LV_TRIG0 */
    E_MHAL_CMDQEVE_SC_LV_TRIG1      =  43,  /**< SC_LV_TRIG1 */
    E_MHAL_CMDQEVE_SC_LV_TRIG2      =  44,  /**< SC_LV_TRIG2 */
    E_MHAL_CMDQEVE_SC_LV_TRIG3      =  45,  /**< SC_LV_TRIG3 */
    E_MHAL_CMDQEVE_SC_LV_TRIG4      =  46,  /**< SC_LV_TRIG4 */
    E_MHAL_CMDQEVE_SC_LV_TRIG5      =  47,  /**< SC_LV_TRIG5 */
    E_MHAL_CMDQEVE_SC_LV_TRIG6      =  48,  /**< SC_LV_TRIG6 */
    E_MHAL_CMDQEVE_SC_LV_TRIG7      =  49,  /**< SC_LV_TRIG7 */
    E_MHAL_CMDQEVE_ROT_IRQ          =  50,  /**< ROT_IRQ */
    E_MHAL_CMDQEVE_ISP0_GP_INT0     =  51,  /**< ISP0_GP_INT0 */
    E_MHAL_CMDQEVE_ISP0_GP_INT1     =  52,  /**< ISP0_GP_INT1 */
    E_MHAL_CMDQEVE_ISP0_GP_INT2     =  53,  /**< ISP0_GP_INT2 */
    E_MHAL_CMDQEVE_ISP0_GP_INT3     =  54,  /**< ISP0_GP_INT3 */
    E_MHAL_CMDQEVE_ISP0_GP_INT4     =  55,  /**< ISP0_GP_INT4 */
    E_MHAL_CMDQEVE_ISP1_GP_INT0     =  56,  /**< ISP1_GP_INT0 */
    E_MHAL_CMDQEVE_ISP1_GP_INT1     =  57,  /**< ISP1_GP_INT1 */
    E_MHAL_CMDQEVE_ISP1_GP_INT2     =  58,  /**< ISP1_GP_INT2 */
    E_MHAL_CMDQEVE_ISP1_GP_INT3     =  59,  /**< ISP1_GP_INT3 */
    E_MHAL_CMDQEVE_ISP1_GP_INT4     =  60,  /**< ISP1_GP_INT4 */
    E_MHAL_CMDQEVE_ISP0_SSM_INT     =  61,  /**< ISP0_SSM_INT */
    E_MHAL_CMDQEVE_ISP1_SSM_INT     =  62,  /**< ISP1_SSM_INT */
    E_MHAL_CMDQEVE_DSP_TRIG0        =  63,  /**< DSP_TRIG0 */
    E_MHAL_CMDQEVE_DSP_TRIG1        =  64,  /**< DSP_TRIG1 */
    E_MHAL_CMDQEVE_DSP_TRIG2        =  65,  /**< DSP_TRIG2 */
    E_MHAL_CMDQEVE_DSP_TRIG3        =  66,  /**< DSP_TRIG3 */
    E_MHAL_CMDQEVE_DSP_TRIG4        =  67,  /**< DSP_TRIG4 */
    E_MHAL_CMDQEVE_DSP_TRIG5        =  68,  /**< DSP_TRIG5 */
    E_MHAL_CMDQEVE_DSP_TRIG6        =  69,  /**< DSP_TRIG6 */
    E_MHAL_CMDQEVE_DSP_TRIG7        =  70,  /**< DSP_TRIG7 */
    E_MHAL_CMDQEVE_MHE_INT0         =  71,  /**< MHE_INT0 */
    E_MHAL_CMDQEVE_MHE_INT1         =  72,  /**< MHE_INT1 */
    E_MHAL_CMDQEVE_IVE_TRIG         =  73,  /**< IVE_TRIG */
    E_MHAL_CMDQEVE_CMDQ_0_INT2      =  74,  /**< CMDQ_0_INT2 */
    E_MHAL_CMDQEVE_CMDQ_1_INT2      =  75,  /**< CMDQ_1_INT2 */
    E_MHAL_CMDQEVE_CMDQ_DIG_INT2    =  76,  /**< CMDQ_DIG_INT2 */
    E_MHAL_CMDQEVE_CMDQ_CMD0_INT2   =  77,  /**< CMDQ_CMD0_INT2 */
    E_MHAL_CMDQEVE_CMDQ_CMD1_INT2   =  78,  /**< CMDQ_CMD1_INT2 */
    E_MHAL_CMDQEVE_CMDQ_ISP0_INT2   =  79,  /**< CMDQ_ISP0_INT2 */
    E_MHAL_CMDQEVE_CMDQ_ISP1_INT2   =  80,  /**< CMDQ_ISP1_INT2 */
    E_MHAL_CMDQEVE_CMDQ_VEN_INT2    =  81,  /**< CMDQ_VEN_INT2 */
    E_MHAL_CMDQEVE_DISP_WDMA_0_INT  =  82,  /**< DISP_WDMA_0_INT */
#endif
#if 1 // M6  & P3
    E_MHAL_CMDQEVE_ISPVSYNC         =  83,  /**< ISPVSYNC */
    E_MHAL_CMDQEVE_DISP0_INT        =  84,  /**< DISP0_INT */
    E_MHAL_CMDQEVE_DISP1_INT        =  85,  /**< DISP1_INT */
    E_MHAL_CMDQEVE_BDMA_TRIG        =  86,  /**< BDMA_TRIG */
    E_MHAL_CMDQEVE_CODEC_INT        =  87,  /**< CODEC_INT */
    E_MHAL_CMDQEVE_WDMA0_DONE       =  88,  /**< WDMA0_DONE */
    E_MHAL_CMDQEVE_WDMA1_DONE       =  89,  /**< WDMA1_DONE */
    E_MHAL_CMDQEVE_WDMA2_DONE       =  90,  /**< WDMA2_DONE */
    E_MHAL_CMDQEVE_WDMA3_DONE       =  91,  /**< WDMA3_DONE */

    E_MHAL_CMDQEVE_ISP_VSYNC_TRIG   =  99,  /**< ISP_VSYNC_TRIG */
#endif
#if 1   //M6P
    E_MHAL_CMDQEVE_ISP_CMDQ_DONE    = 100,  /**< ISP_CMDQ_DONE */
#endif
    E_MHAL_CMDQEVE_IP_MAX                   /**< CMDQ trigger event max limitation */

} MHAL_CMDQ_EventId_e;

/**
 * MHAL_CMDQ_BridgeIP_e
 * @note CMDQ bridgeIP table
 */
typedef enum
{
#if 1 //I7
    E_MHAL_CMDQ_MIPI_TX_CSI    =   1,       /**< MIPI_TX_CSI */
    E_MHAL_CMDQ_DISP0          =   2,       /**< DISP0 */
    E_MHAL_CMDQ_DISP1          =   3,       /**< DISP1 */
    E_MHAL_CMDQ_DISP2          =   4,       /**< DISP2 */
    E_MHAL_CMDQ_I2C            =   5,       /**< I2C */
    E_MHAL_CMDQ_IVE            =   6,       /**< IVE */
    E_MHAL_CMDQ_BDMA           =   7,       /**< BDMA */
    E_MHAL_CMDQ_SCL0           =   8,       /**< SCL0 */
    E_MHAL_CMDQ_SCL1           =   9,       /**< SCL1 */
    E_MHAL_CMDQ_SCL_JPE0       =  10,       /**< SCL_JPE0 */
    E_MHAL_CMDQ_SCL_JPE1       =  11,       /**< SCL_JPE1 */
    E_MHAL_CMDQ_SCL_YUV0       =  12,       /**< SCL_YUV0 */
    E_MHAL_CMDQ_SCL_YUV1       =  13,       /**< SCL_YUV1 */
    E_MHAL_CMDQ_LDC0           =  14,       /**< LDC0 */
    E_MHAL_CMDQ_LDC1           =  15,       /**< LDC1 */
    E_MHAL_CMDQ_ISP0           =  16,       /**< ISP0 */
    E_MHAL_CMDQ_ISP0_RDMA      =  17,       /**< ISP0_RDMA */
    E_MHAL_CMDQ_VIF0           =  18,       /**< VIF0 */
    E_MHAL_CMDQ_ISP1           =  19,       /**< ISP1 */
    E_MHAL_CMDQ_ISP1_RDMA      =  20,       /**< ISP1_RDMA */
    E_MHAL_CMDQ_VIF1           =  21,       /**< VIF1 */
    E_MHAL_CMDQ_VENC0          =  22,       /**< VENC0 */
    E_MHAL_CMDQ_VENC1          =  23,       /**< VENC1 */
#endif
#if 1 //M6P
    E_MHAL_CMDQ_SCL_RDMA0      =  30,       /**< SCL_RDMA0 */
    E_MHAL_CMDQ_SCL_MAPCONV    =  31,       /**< SCL_MAPCONV */
    E_MHAL_CMDQ_DISP_WBC       =  32,       /**< DISP_WBC */
    E_MHAL_CMDQ_ISP_IQ         =  33,       /**< ISP_IQ */
#endif
    E_MHAL_CMDQ_BRIDGE_IP_MAX               /**< CMDQ bridgeIP max limitation */
} MHAL_CMDQ_BridgeIP_e;

/**
 * MHAL_CMDQ_BufDescript_t
 * @note Original CMDQ buffer description
 */
typedef struct MHAL_CMDQ_BufDescript_s
{
    MS_U32 u32CmdqBufSize;          /**< Buffer size */
    MS_U16 u32CmdqBufSizeAlign;     /**< Buffer alignment */
} MHAL_CMDQ_BufDescript_t;

/**
 * MHAL_CMDQ_BufDescript_t
 * @note MemCmdq buffer description
 */
typedef struct MHAL_CMDQ_MemScriptBufDescript_s
{
    MS_U8  u8CmdqBufName[32];       /**< Buffer name */
    MS_U16 u32CmdqBufSize;          /**< Buffer size */
    MS_U16 u32CmdqBufSizeAlign;     /**< Buffer alignment */
} MHAL_CMDQ_MemScriptBufDescript_t;

/**
 * MHAL_CMDQ_MultiCmdBuf_t
 * @note Original CMDQ write multiple cmd buffer description
 */
typedef struct MHAL_CMDQ_MultiCmdBuf_s
{
    MS_U32 u32RegAddr;              /**< Expected register address */
    MS_U32 u32RegValue;             /**< Expected value */
} MHAL_CMDQ_MultiCmdBuf_t;

/**
 * MHAL_CMDQ_MultiCmdBufMask_t
 * @note Original CMDQ write multiple cmd buffer description with mask
 */
typedef struct MHAL_CMDQ_MultiCmdBufMask_s
{
    MS_U32 u32RegAddr;              /**< Expected register address */
    MS_U16 u16RegValue;             /**< Expected value */
    MS_U16 u16Mask;                 /**< Mask value */
} MHAL_CMDQ_MultiCmdBufMask_t;

/**
 * MHAL_CMDQ_MultiCmdBufMask_t
 * @note MemCmdq write multiple cmd buffer description with mask
 */
typedef struct MHAL_MemCMDQ_MultiCmdBufMask_s
{
    MS_U16 u16Seq;                  /**< Expected cmd sequence */
    MS_U16 u16Dummy;                /**< for 32 bit alignment */
    MS_U32 u32RegAddr;              /**< Expected register address */
    MS_U16 u16RegValue;             /**< Expected value */
    MS_U16 u16Mask;                 /**< Mask value */
} MHAL_MemCMDQ_MultiCmdBufMask_t;

/**
 * MHAL_CMDQ_Mmap_Info_t
 * @note CMDQ mmap information structure
 */
typedef struct MHAL_CMDQ_Mmap_Info_s
{
    MS_U8           u8CmdqMmapGid;              /**< Mmap ID */
    MS_U8           u8CmdqMmapLayer;            /**< Memory Layer */
    MS_U8           u8CmdqMmapMiuNo;            /**< 0: MIU0 / 1: MIU1 / 2: MIU2 */
    MS_U8           u8CmdqMmapCMAHid;           /**< Memory CMAHID */
    ss_miu_addr_t   nCmdqMmapMiuAddr;           /**< Miu Memory Address */
    void*           pCmdqMmapVirAddr;           /**< Vir Memory Address */
    MS_U32          u32CmdqMmapSize;            /**< Memory Size */
    MS_U32          u32CmdqMmapAlign;           /**< Memory Align */
    MS_U32          u32CmdqMmapMemoryType;      /**< Memory Type */

} MHAL_CMDQ_Mmap_Info_t;

typedef struct MHAL_CMDQ_CmdqInterface_s     MHAL_CMDQ_CmdqInterface_t;

struct MHAL_CMDQ_CmdqInterface_s
{
//------------------------------------------------------------------------------
/// @brief        Check cmdq buffer is available
/// @param[in]    pCmdinf    : CMDQ interface
/// @param[in]    u32CmdqNum : Check cmd number
/// @return       integer
/// @retval       0  : is unavailable
/// @retval       >0 : current cmdq available number
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CheckBufAvailable)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32CmdqNum);

//------------------------------------------------------------------------------
/// @brief        Write CMDQ dummy register
/// @param[in]    pCmdinf  : CMDQ interface
/// @param[in]    u16Value : Write value
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_WriteDummyRegCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U16 u16Value);

//------------------------------------------------------------------------------
/// @brief        Read CMDQ dummy register
/// @param[in]    pCmdinf   : CMDQ interface
/// @param[out]   u16RegVal : Read value
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_ReadDummyRegCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U16* u16RegVal);

//------------------------------------------------------------------------------
/// @brief        Add write command with Mask
/// @param[in]    pCmdinf      : CMDQ interface
/// @param[in]    u32RegAddr   : Expected register addr
/// @param[in]    u16Value     : Write value
/// @param[in]    u16WriteMask : Set bit as 1 , this bit is available
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_WriteRegCmdqMask)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Value, MS_U16 u16WriteMask);

//------------------------------------------------------------------------------
/// @brief        Add multiple write command with Mask
/// @param[in]    pCmdinf            : CMDQ interface
/// @param[in]    u32MultiCmdBufMask : Multiple cmd buf
/// @param[in]    u16Size            : Expected cmd number
/// @return       integer
/// @retval       Actual size already write to CMDQ buffer
//------------------------------------------------------------------------------
    MS_U32(*MHAL_CMDQ_WriteRegCmdqMaskMulti)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MHAL_CMDQ_MultiCmdBufMask_t *u32MultiCmdBufMask, MS_U16 u16Size);

//------------------------------------------------------------------------------
/// @brief        Add memcmdq multiple write command with Mask
/// @param[in]    pCmdinf            : MemCMDQ interface
/// @param[in]    u32MultiCmdBufMask ：Multiple cmd buf with mask
/// @param[in]    u16Size            : Expected cmd number
/// @return       integer
/// @retval       Actual size already write to CMDQ buffer
//------------------------------------------------------------------------------
    MS_U32(*MHAL_MemCMDQ_WriteRegCmdqMaskMulti)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MHAL_MemCMDQ_MultiCmdBufMask_t *u32MultiCmdBufMask, MS_U16 u16Size);

//------------------------------------------------------------------------------
/// @brief        Add write command without Mask
/// @param[in]    pCmdinf      : CMDQ interface
/// @param[in]    u32RegAddr   : Expected register addr
/// @param[in]    u16Value     : Write value
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_WriteRegCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Value);

//------------------------------------------------------------------------------
/// @brief        Add multiple write command without Mask
/// @param[in]    pCmdinf            : CMDQ interface
/// @param[in]    u32MultiCmdBuf     : Multiple cmd buf without mask
/// @param[in]    u16Size            : Expected cmd number
/// @return       integer
/// @retval       Actual size already write to CMDQ buffer
//------------------------------------------------------------------------------
    MS_U32(*MHAL_CMDQ_WriteRegCmdqMulti)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MHAL_CMDQ_MultiCmdBuf_t *u32MultiCmdBuf, MS_U16 u16Size);

//------------------------------------------------------------------------------
/// @brief        Add poll command with Mask
/// @param[in]    pCmdinf      : CMDQ interface
/// @param[in]    u32RegAddr   : Expected register addr
/// @param[in]    u16Value     : Write value
/// @param[in]    u16WriteMask : Set bit as 1 , this bit is available
/// @param[in]    bPollEq      : Poll flag                   \n
///                              TRUE  : is poll eq command  \n
///                              FALSE : is poll neq command
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdqPollRegBits)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Value,  MS_U16 u16WriteMask, MS_BOOL bPollEq);

//------------------------------------------------------------------------------
/// @brief        Add wait command
/// @param[in]    pCmdinf : CMDQ interface
/// @param[in]    eEvent  : Expected event
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdqAddWaitEventCmd)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MHAL_CMDQ_EventId_e eEvent);

//------------------------------------------------------------------------------
/// @brief        Abort cmdq buffer , will go back previrous write point
/// @param[in]    pCmdinf : CMDQ interface
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdqAbortBuffer)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Reset cmdq engine
/// @param[in]    pCmdinf : CMDQ interface
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdqResetEngine)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Read cmdq current status
/// @param[in]    pCmdinf    : CMDQ interface
/// @param[out]   u32StatVal : CMDQ status
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_ReadStatusCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32* u32StatVal);

//------------------------------------------------------------------------------
/// @brief        Kick off cmdq
/// @param[in]    pCmdinf : CMDQ interface
/// @return       integer
/// @retval        < 0 : is failed
/// @retval       >= 0 : kick off cmd number
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_KickOffCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Clear all trigger bus event
/// @param[in]    pCmdinf : CMDQ interface
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_ClearTriggerEvent)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Check cmdq is empty
/// @param[in]    pCmdinf  : CMDQ interface
/// @param[out]   bIdleVal : Idle value
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_IsCmdqEmptyIdle)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_BOOL* bIdleVal);

    VOID *  pCtx; /**< Usually used as a CmdqInterfacePriv_t to store cmdq private data */

//------------------------------------------------------------------------------
/// @brief        Enable or disable function that when CMDQ is idel , notify other CMDQ
/// @param[in]    pCmdinf : CMDQ interface
/// @param[out]   bEnable : 0/1         \n
///                         0 : disable \n
///                         1 : enable
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_EnableIdleNotifyCmdq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_BOOL bEnable);

//------------------------------------------------------------------------------
/// @brief        Clear CMDQ irq2 event
/// @param[in]    CMDQ interface
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_ByIdClrGroupIrq2)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Kick off cmdq with tag
/// @param[in]    pCmdinf : CMDQ interface
/// @param[in]    u16Tag  : 0/1                 \n
///                         0 : abort buffer    \n
///                         1 : not abort buffer
/// @return       integer
/// @retval        < 0 : is failed
/// @retval       >= 0 : kick off cmd number
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_KickOffCmdqByTag)(MHAL_CMDQ_CmdqInterface_t *pCmdinf,MS_U16 u16Tag);

//------------------------------------------------------------------------------
/// @brief        command delay
/// @param[in]    pCmdinf : CMDQ interface
/// @param[in]    uTimeNs : time value(ns)
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdDelay)(MHAL_CMDQ_CmdqInterface_t *pCmdinf,MS_U32 uTimeNs);

//------------------------------------------------------------------------------
/// @brief        Get current cmdq number
/// @param[in]    pCmdinf : CMDQ interface
/// @return       Unsigned integer
/// @retval       = 0 : is failed
/// @retval       > 0 : cmdq number
//------------------------------------------------------------------------------
    MS_U32 (*MHAL_CMDQ_GetCurrentCmdqNumber)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Add poll command with Mask
/// @param[in]    pCmdinf      : CMDQ interface
/// @param[in]    u32RegAddr   : Expected register addr
/// @param[in]    u16Value     : Write value
/// @param[in]    u16WriteMask : Set bit as 1 , this bit is available
/// @param[in]    bPollEq      : Poll flag                   \n
///                              TRUE  : is poll eq command  \n
///                              FALSE : is poll neq command
/// @param[in]    uTimeNs      : poll time (ns)
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_CmdqPollRegBits_ByTime)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Value,  MS_U16 u16WriteMask, MS_BOOL bPollEq,MS_U32 uTimeNs);

//------------------------------------------------------------------------------
/// @brief        Get a dummy tag register for MI internal use
/// @param[in]    pCmdinf : CMDQ interface
/// @return       integer
/// @retval       MHAL_FAILURE : is failed
/// @retval                >=0 : u32RegAddr of dummy register
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_CmdqGetAppEventDummyRegAddr)(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Set weighting register of RoundRobin for MI
/// @param[in]    pCmdinf   : CMDQ interface
/// @param[in]    u32Weight : Expected weight
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_SetWeight)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32Weight);

//------------------------------------------------------------------------------
/// @brief        Get CMDQ bank address
/// @param[in]    pCmdinf      : CMDQ interface
/// @param[out]   pU32BankAddr : CMDQ bank, eg.0x1120
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_GetBankAddress)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 *pU32BankAddr);

//------------------------------------------------------------------------------
/// @brief        Kick off MemCmdq
/// @param[in]    pHWCmdinf : HW CMDQ interface
/// @param[in]    pMemScriptCMDQinf : MemCmdq interface that is expected to kickoff
/// @return       integer
/// @retval        < 0 : is failed
/// @retval       >= 0 : kick off cmd number
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               for(i=0; i<0x20; i++)
///                   g_pMemScriptCmdq1->MHAL_CMDQ_WriteRegWithSeq(g_pMemScriptCmdq1, (nDumAddr-(2*i)), 1, 0x2010+i);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_KickOffMemScriptCmdq(g_pSelectCmdq, g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_KickOffMemScriptCmdq)(MHAL_CMDQ_CmdqInterface_t *pHWCmdinf, MHAL_CMDQ_CmdqInterface_t *pMemscriptCmdinf);

//------------------------------------------------------------------------------
/// @brief        Get MemCmdq current write position
/// @param[in]    pMemscriptCmdinf : MemCmdq interface
/// @return       integer
/// @retval        < 0 : is failed
/// @retval       >= 0 : MemCmdq current write position
//------------------------------------------------------------------------------
    MS_U32 (*MHAL_CMDQ_GetCurPos) (MHAL_CMDQ_CmdqInterface_t *pMemscriptCmdinf);

//------------------------------------------------------------------------------
/// @brief        Abort the memcmdq buffer, will go back previrous write point
/// @param[in]    pMemscriptCmdinf : MemCmdq interface
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_MemCmdqAbortBuf) (MHAL_CMDQ_CmdqInterface_t *pMemscriptCmdinf);

//------------------------------------------------------------------------------
/// @brief        Seek to Expected write MemCmdq buffer position
/// @param[in]    pMemscriptCmdinf : MemCmdq interface
/// @param[in]    u32TargetPos     : Expected write position
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_SeekToNextWritePos) (MHAL_CMDQ_CmdqInterface_t *pMemscriptCmdinf, MS_U32 u32TargetPos); //buffer

//------------------------------------------------------------------------------
/// @brief        Get the MemCmdq buffer size already used
/// @param[in]    pMemscriptCmdinf : MemCmdq interface.
/// @return       integer
/// @retval        < 0 : is failed
/// @retval       >= 0 : MemCmdq buffer size already used
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_GetMemCmdqUsedSize) (MHAL_CMDQ_CmdqInterface_t *pMemscriptCmdinf);

//------------------------------------------------------------------------------
/// @brief        Add MemCmdq wait command
/// @param[in]    pCmdinf : MemCmdq interface.
/// @param[in]    u16Seq  : Expected sequence
/// @param[in]    eEvent  : Expected event
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_MemCmdqAddWaitEventCmd(g_pMemScriptCmdq1, E_MHAL_CMDQEVE_DUMMY15, u16Seq2);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_KickOffMemScriptCmdq(g_pSelectCmdq, g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_MemCmdqAddWaitEventCmd)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U16 u16Seq, MHAL_CMDQ_EventId_e eEvent);

//------------------------------------------------------------------------------
/// @brief        Add MemCmdq poll command with Mask
/// @param[in]    pCmdinf : MemCmdq interface
/// @param[in]    u32RegAddr   : Expected register addr
/// @param[in]    u16Value     : Write value
/// @param[in]    u16Seq       : Expected sequence
/// @param[in]    u16WriteMask : Set bit as 1 , this bit is available
/// @param[in]    bPollEq      : Poll flag                   \n
///                              TRUE  : is poll eq command  \n
///                              FALSE : is poll neq command
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_MemCmdqPollRegBits(g_pMemScriptCmdq1, ((nDumAddr)), 0xff00, u16Seq, 0xff00, 1);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_KickOffMemScriptCmdq(g_pSelectCmdq, g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
    MS_S32(*MHAL_CMDQ_MemCmdqPollRegBits)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Value, MS_U16 u16Seq, MS_U16 u16WriteMask, MS_BOOL bPollEq);

//------------------------------------------------------------------------------
/// @brief        Add MemCmdq write command with sequence
/// @param[in]    pCmdinf    : MemCmdq interface
/// @param[in]    u32RegAddr : Expected register addr
/// @param[in]    u16Seq     : Expected sequence
/// @param[in]    u16Value   : Write value
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               for(i=0; i<0x20; i++)
///                   g_pMemScriptCmdq1->MHAL_CMDQ_WriteRegWithSeq(g_pMemScriptCmdq1, (nDumAddr-(2*i)), 1, 0x2010+i);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_KickOffMemScriptCmdq(g_pSelectCmdq, g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_WriteRegWithSeq)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Seq, MS_U16 u16Value);

//------------------------------------------------------------------------------
/// @brief        Add MemCmdq write command with sequence and mask
/// @param[in]    pCmdinf    : MemCmdq interface
/// @param[in]    u32RegAddr : Expected register addr
/// @param[in]    u16Seq     : Expected sequence
/// @param[in]    u16Value   : Write value
/// @param[in]    u16WriteMask : set bit as 1 , this bit is available
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_WriteRegWithSeqMask(g_pMemScriptCmdq1, ((nDumAddr)), u16Seq, 0xaa, 0xffff);
///
///               g_pMemScriptCmdq1->MHAL_CMDQ_KickOffMemScriptCmdq(g_pSelectCmdq, g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
    MS_S32 (*MHAL_CMDQ_WriteRegWithSeqMask)(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Seq, MS_U16 u16Value, MS_U16 u16WriteMask);

    MHAL_CMDQ_TYPE_e eCmdqType; /**< Indicates the cmdq is original cmdq or memcmdq */
};

//------------------------------------------------------------------------------
/// @brief        Get bridge cmdq free ID
/// @param[in]    bridgeIp : Expect to get free ID of the engine IP
/// @return       integer
/// @retval       -1  : no free ID
/// @retval       >=0 : bridge cmdq free ID
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_GetBridgeFreeId(MHAL_CMDQ_BridgeIP_e bridgeIp);

//------------------------------------------------------------------------------
/// @brief        Get cmdq free ID
/// @return       integer
/// @retval       -1  : no free ID
/// @retval       >=0 : cmdq free ID
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_GetFreeId(void);

//------------------------------------------------------------------------------
/// @brief        Get cmdq specific ID
/// @param[in]    nCmdqId : Expected cmdq channel ID
/// @return       integer
/// @retval       -1  : this ID is not free
/// @retval       >=0 : cmdq specific ID
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_GetSpecificId(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Release cmdq ID
/// @param[in]    nCmdqId : Expected release cmdq channel ID
/// @return       NULL
//------------------------------------------------------------------------------
void MHAL_CMDQ_ReleaseId(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Get cmdq hw channel number
/// @return       unsigned integer
/// @retval       cmdq hw channel number
//------------------------------------------------------------------------------
MS_U32 MHAL_CMDQ_GetCmdqHWNumber(void);

//------------------------------------------------------------------------------
/// @brief        Init CMDQ mmap info
/// @param[in]    pCmdqMmapInfo : Cmdq mmap information structure
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_InitCmdqMmapInfo(MHAL_CMDQ_Mmap_Info_t *pCmdqMmapInfo);

//------------------------------------------------------------------------------
/// @brief        Deinit CMDQ mmap info for cmdq pool being released
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DeinitCmdqMmapInfo(void);

//------------------------------------------------------------------------------
/// @brief        Get ring mode Cmdq service
/// @param[in]    nCmdqId      : CMDQ channel ID
/// @param[in]    pCmdqBufDesp : CMDQ buffer description structure
/// @param[in]    bForceRIU    : CMDQ RIU mode
/// @return       MHAL_CMDQ_CmdqInterface_t
/// @retval       NULL                      : is failed
/// @retval       MI_ERR_INVALID_PARAMETER  : Null parameter
/// @retval       MHAL_CMDQ_CmdqInterface_t : point is success
//------------------------------------------------------------------------------
MHAL_CMDQ_CmdqInterface_t *MHAL_CMDQ_GetSysCmdqService(MS_S32 nCmdqId, MHAL_CMDQ_BufDescript_t *pCmdqBufDesp, MS_BOOL bForceRIU);

//------------------------------------------------------------------------------
/// @brief        Create ring mode MemCmdq service
/// @param[in]    pCmdqMemScriptBufDesp : MemCMDQ buffer description structure
/// @return       MHAL_CMDQ_CmdqInterface_t
/// @retval       NULL                      : is failed
/// @retval       MI_ERR_INVALID_PARAMETER  : Null parameter
/// @retval       MHAL_CMDQ_CmdqInterface_t : point is success
//------------------------------------------------------------------------------
MHAL_CMDQ_CmdqInterface_t *MHAL_CMDQ_CreateMemoryScriptCmdq(MHAL_CMDQ_MemScriptBufDescript_t *pCmdqMemScriptBufDesp);

//------------------------------------------------------------------------------
/// @brief        Duplicate ring mode MemCmdq service
/// @param[in]    pCmdqSrcInf           : Duplicate MemCmdq service source
/// @param[in]    pCmdqMemScriptBufDesp : Dup MemCMDQ buffer description structure
/// @return       MHAL_CMDQ_CmdqInterface_t
/// @retval       NULL                      : is failed
/// @retval       MHAL_CMDQ_CmdqInterface_t : point is success
//------------------------------------------------------------------------------
MHAL_CMDQ_CmdqInterface_t *MHAL_CMDQ_CreateDupMemCmdq(MHAL_CMDQ_CmdqInterface_t *pCmdqSrcInf, MHAL_CMDQ_MemScriptBufDescript_t *pCmdqMemScriptBufDesp);

//------------------------------------------------------------------------------
/// @brief        Destory ring mode MemCmdq service
/// @param[in]    pCmdqMemScriptInf : Expected release MemCMDQ handle
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DestoryMemoryScriptCmdq(MHAL_CMDQ_CmdqInterface_t *pCmdqMemScriptInf);

//------------------------------------------------------------------------------
/// @brief        Get MemCmdq Buf start address
/// @param[in]    pCmdqMemScriptInf : Expected MemCMDQ handle
/// @param[out]   pu32StartAddr     : MIU addr
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_GetMemCmdqBufAddr(MHAL_CMDQ_CmdqInterface_t *pCmdqMemScriptInf, ss_miu_addr_t* pu32StartAddr);

//------------------------------------------------------------------------------
/// @brief        Dup MemCmdq script from source MemCmdq to destination MemCmdq
/// @param[in]    pCmdqDstInf : Destination MemCMDQ handle
/// @param[in]    pCmdqSrcInf : Source MemCMDQ handle
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DupMemScriptInfo(MHAL_CMDQ_CmdqInterface_t *pCmdqDstInf, MHAL_CMDQ_CmdqInterface_t *pCmdqSrcInf);

//------------------------------------------------------------------------------
/// @brief        Begin Partial Update MemCmdq buffer
/// @param[in]    pCmdinf      : MemCMDQ handle
/// @param[in]    u32RegAddr   : Partial Update start reg addr
/// @param[in]    u16Seq       : Partial Update start reg seq
/// @param[in]    bLookupTable : Update Hashtable Flag         \n
///                              TRUE  : Update Hashtable      \n
///                              FALSE : Not update Hashtable
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               g_pSelectCmdq = MHAL_CMDQ_GetSysCmdqService(g_cmdqIdx[nSlot], &tCmdqBufDesp, 0);
///               g_pMemScriptCmdq1 = MHAL_CMDQ_CreateMemoryScriptCmdq(&cmdqMemScriptBufDesp1);
///
///               MHAL_CMDQ_BeginPartialUpdate(g_pMemScriptCmdq1, nDumAddr, seq1, 1);
///
///               for (i=0x0; i<0x30; i++)
///                   g_pSelectCmdq->MHAL_CMDQ_WriteRegCmdq(g_pMemScriptCmdq1, (nDumAddr-(2*i)), i);
///
///               MHAL_CMDQ_EndPartialUpdate(g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_BeginPartialUpdate(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_U32 u32RegAddr, MS_U16 u16Seq, MS_BOOL bLookupTable);

//------------------------------------------------------------------------------
/// @brief        Partial Update MemCmdq End
/// @param[in]    pCmdinf : MemCMDQ handle
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
/// @par Sample
/// @code
///               MHAL_CMDQ_BeginPartialUpdate(g_pMemScriptCmdq1, nDumAddr, seq1, 1);
///
///               for (i=0x0; i<0x30; i++)
///                   g_pSelectCmdq->MHAL_CMDQ_WriteRegCmdq(g_pMemScriptCmdq1, (nDumAddr-(2*i)), i);
///
///               MHAL_CMDQ_EndPartialUpdate(g_pMemScriptCmdq1);
/// @endcode
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_EndPartialUpdate(MHAL_CMDQ_CmdqInterface_t *pCmdinf);

//------------------------------------------------------------------------------
/// @brief        Get direct mode Cmdq service
/// @param[in]    nCmdqId   : CMDQ channel ID
/// @param[in]    bForceRIU : CMDQ RIU mode
/// @return       MHAL_CMDQ_CmdqInterface_t
/// @retval       NULL                      : is failed
/// @retval       MI_ERR_INVALID_PARAMETER  : Null parameter
/// @retval       MHAL_CMDQ_CmdqInterface_t : point is success
//------------------------------------------------------------------------------
MHAL_CMDQ_CmdqInterface_t *MHAL_CMDQ_GetSysCmdqServiceBufferDirectMode(MS_S32 nCmdqId, MS_BOOL bForceRIU);

//------------------------------------------------------------------------------
/// @brief        Release ring mode cmdq service
/// @param[in]    nCmdqId : CMDQ channel ID
/// @return       NULL
//------------------------------------------------------------------------------
void MHAL_CMDQ_ReleaseSysCmdqService(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Release direct mode cmdq service
/// @param[in]    nCmdqId : CMDQ channel ID
/// @return       NULL
//------------------------------------------------------------------------------
void MHAL_CMDQ_ReleaseSysCmdqServiceBufferDirectMode(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Host cmdq insert commands that is the buffer setting and the kick for slave cmdq
/// @param[in]    pCmdinf      : Host CMDQ interface
/// @param[in]    pBufVir      : Slave CMDQ buffer virtual address
/// @param[in]    nBufMiu      : Slave CMDQ buffer miu address
/// @param[in]    u32CmdNum    : Slave CMDQ command number
/// @param[in]    nSlaveCmdqId : Slave CMDQ ID
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_HostControlSlave(MHAL_CMDQ_CmdqInterface_t *pCmdinf, void* pBufVir, ss_miu_addr_t nBufMiu, MS_U32 u32CmdNum, MS_S32 nSlaveCmdqId);

//------------------------------------------------------------------------------
/// @brief        Host cmdq insert command that waits for finish of the slave cmdq
/// @param[in]    pCmdinf      : Host CMDQ interface
/// @param[in]    nSlaveCmdqId : Slave CMDQ ID
/// @return       integer
/// @retval       MHAL_SUCCESS : is ok
/// @retval       MHAL_FAILURE : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_HostAddWaitSlaveIdle(MHAL_CMDQ_CmdqInterface_t *pCmdinf, MS_S32 nSlaveCmdqId);

//------------------------------------------------------------------------------
/// @brief        Get cmdq status in early boot time
/// @param[in]    nCmdqId : CMDQ channel ID
/// @return       Cmdq status
/// @retval       E_CMDQ_EARLYRUN_BUSY : this cmdqId is busy
/// @retval       E_CMDQ_EARLYRUN_DONE : this cmdqId is done
//------------------------------------------------------------------------------
MHAL_CMDQ_EarlyRunStatus_e MHAL_CMDQ_GetEarlyRunStatus(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Run cmdq with direct mode in early boot time
/// @param[in]    nCmdqId       : CMDQ channel ID
/// @param[in]    pCmdqMmapInfo : Cmdq mmap information structure
/// @return       integer
/// @retval       -1 : direct mode cmdq early run fail
/// @retval        0 : direct mode cmdq early run success
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DirectModeEarlyRun(MS_S32 nCmdqId, MHAL_CMDQ_Mmap_Info_t *pCmdqMmapInfo);

//------------------------------------------------------------------------------
/// @brief        Stop cmdq with direct mode early run
/// @param[in]    nCmdqId : CMDQ channel ID
/// @return       integer
/// @retval       -1 : stop direct mode cmdq early run fail
/// @retval        0 : stop direct mode cmdq early run success
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DirectModeEarlyRunStop(MS_S32 nCmdqId);

//------------------------------------------------------------------------------
/// @brief        Add byte mode switch command to buffer
/// @param[in]    nCmdqId  : CMDQ channel ID
/// @param[in]    eByteSel : CMDQ byte mode
/// @param[out]   pCmdqBuf : Command buffer, size of 8 byte
/// @return       integer
/// @retval        1 : is ok
/// @retval       -1 : is failed
//------------------------------------------------------------------------------
MS_S32 MHAL_CMDQ_DirectModeByteModeCmd(MS_S32 nCmdqId, MHAL_CMDQ_ByteSel_e eByteSel, void *pCmdqBuf);

#endif
