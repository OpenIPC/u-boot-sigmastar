/*
* drv_scl_pq_define.h- Sigmastar
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

#ifndef _DRVPQ_DEFINE_H_
#define _DRVPQ_DEFINE_H_

#define MAIN_WINDOW 0
#define SUB_WINDOW  1

#define PQ_MAP_REG(reg) (((reg) >> 8) & 0xFF), ((reg)&0xFF)
#define REG_ADDR_SIZE   2
#define REG_MASK_SIZE   1

#define PQ_IP_NULL 0xFF

#define PQTBL_EX     0x10
#define PQTBL_NORMAL 0x11

#define E_XRULE_HSD 0
#define E_XRULE_VSD 1
#define E_XRULE_HSP 2
#define E_XRULE_VSP 3
#define E_XRULE_CSC 4

#define E_XRULE_NUM 5

// Here must be the same order as MST_GRule_Index_Main
typedef enum
{
    E_GRULE_NR,
    E_GRULE_NUM,
} EN_GRULE_INDEX_e;

#define _END_OF_TBL_ 0xFFFF

typedef enum
{
    E_PQ_TABTYPE_GENERAL = 50,
    E_PQ_TABTYPE_COMB    = 51,
    E_PQ_TABTYPE_SCALER  = 52,
    E_PQ_TABTYPE_SRAM1   = 53,
    E_PQ_TABTYPE_SRAM2   = 54,
    E_PQ_TABTYPE_SRAM3   = 55,
    E_PQ_TABTYPE_SRAM4   = 56,
    E_PQ_TABTYPE_C_SRAM1 = 57,
    E_PQ_TABTYPE_C_SRAM2,
    E_PQ_TABTYPE_C_SRAM3,
    E_PQ_TABTYPE_C_SRAM4,
    E_PQ_TABTYPE_SRAM_COLOR_INDEX,
    E_PQ_TABTYPE_SRAM_COLOR_GAIN_SNR,
    E_PQ_TABTYPE_SRAM_COLOR_GAIN_DNR,
    E_PQ_TABTYPE_VIP_IHC_CRD_SRAM,
    E_PQ_TABTYPE_VIP_ICC_CRD_SRAM,
    E_PQ_TABTYPE_XVYCC_DE_GAMMA_SRAM,
    E_PQ_TABTYPE_XVYCC_GAMMA_SRAM,
    E_PQ_TABTYPE_SWDRIVER,
    E_PQ_TABTYPE_PICTURE_1,
    E_PQ_TABTYPE_PICTURE_2,
    E_PQ_TABTYPE_MAX,
} E_PQ_TABTYPE_e;

typedef enum
{
    E_PQ_CHECK_OFF  = 0, // reset before test
    E_PQ_CHECK_REG  = 1, // test register mode
    E_PQ_CHECK_SIZE = 2, // test size mode
    E_PQ_CHECK_NUM,
} E_PQ_CHECK_TYPE;

typedef struct DrvPqIptabInfo_s
{
    u8 *pIPCommTable;
    u8 *pIPTable;
    u8  u8TabNums;
    u8  u8TabType;
} DrvPqIptabInfo_t;

typedef struct DrvPqIpInfo_s
{
    u8 *pIPTable;
    u8  u8TabNums;
    u8  u8TabType;
    u8  u8TabIdx;
} DrvPqIpInfo_t;

enum
{
    E_PQ_FUNC_DUMP_REG,
    E_PQ_FUNC_CHK_REG,
};

#define PQ_MUX_DEBUG 0

#define SCALER_REGISTER_SPREAD 1

#define MApi_XC_ReadByte(u32Reg)
#define MApi_XC_WriteByteMask(u32Reg, u8Val, u8Msk)
#define MApi_XC_WriteByte(u32Reg, u8Val)

#define MApi_XC_R2BYTE(u32Reg)
#define MApi_XC_R2BYTEMSK(u32Reg, u16mask)

#define MApi_XC_W2BYTE(u32Reg, u16Val)
#define MApi_XC_W2BYTEMSK(u32Reg, u16Val, u16mask)
#if (SCALER_REGISTER_SPREAD)
#define BK_SCALER_BASE 0x110000
#define COMB_REG_BASE  0x110000
// no need to store bank for spread reg
#define SC_BK_STORE_NOMUTEX

#define SC_BK_RESTORE_NOMUTEX

// switch bank
#define SC_BK_SWITCH(_x_)

#define SC_BK_CURRENT (u8CurBank)

#endif

// store bank
#define COMB_BK_STORE \
    u8 u8Bank;        \
    u8Bank = MApi_XC_ReadByte(COMB_REG_BASE)

// restore bank
#define COMB_BK_RESTORE MApi_XC_WriteByte(COMB_REG_BASE, u8Bank)
// switch bank
#define COMB_BK_SWITCH(_x_) MApi_XC_WriteByte(COMB_REG_BASE, _x_)

#define COMB_BK_CURRENT MApi_XC_ReadByte(COMB_REG_BASE)

#define PQ_IP_COMM 0xfe
#define PQ_IP_ALL  0xff

typedef enum
{
    /// Main window
    E_PQ_MAIN_WINDOW = 0,

    E_PQ_DSPY_WINDOW,

    /// The max support window of PQ
    E_PQ_MAX_WINDOW
} E_PQ_WIN_TYPE_e;

typedef struct DrvPQTableTotalInfo_s
{
    E_PQ_WIN_TYPE_e   eWin;
    u8                u8PQ_InputType_Num;
    u8                u8PQ_IP_Num;
    u8 *              pQuality_Map_Aray;
    DrvPqIptabInfo_t *pIPTAB_Info;
    u8 *              pSkipRuleIP;

    u8  u8PQ_XRule_IP_Num[E_XRULE_NUM];
    u8 *pXRule_IP_Index[E_XRULE_NUM];
    u8 *pXRule_Array[E_XRULE_NUM];

    u8  u8PQ_GRule_Num[E_GRULE_NUM];
    u8  u8PQ_GRule_IPNum[E_GRULE_NUM];
    u8  u8PQ_GRule_LevelNum[E_GRULE_NUM];
    u8 *pGRule_IP_Index[E_GRULE_NUM];
    u8 *pGRule_Array[E_GRULE_NUM];
    u8 *pGRule_Level[E_GRULE_NUM];
} DrvPQTableTotalInfo_t;

/**
 *  Video data information to supply when in PQ mode.
 */
typedef struct DrvPQModeInfo_s
{
    /// is FBL or not
    u8 bFBL;
    /// is interlace mode or not
    u8 bInterlace;
    /// input Horizontal size
    u16 u16input_hsize;
    /// input Vertical size
    u16 u16input_vsize;
    /// input Vertical total
    u16 u16input_vtotal;
    /// input Vertical frequency
    u16 u16input_vfreq;
    /// output Vertical frequency
    u16 u16ouput_vfreq;
    /// Display Horizontal size
    u16 u16display_hsize;
    /// Display Vertical size
    u16 u16display_vsize;
} DrvPQModeInfo_t;

typedef struct DrvPQDataInfo_s
{
    u8 *pBuf;
    u16 u16BufSize;
} DrvPQDataInfo_t;

typedef struct DrvPQCmdqConfig_s
{
    u8 bEnFmCnt;
    u8 u8FmCnt;
    u8 bfire;
} DrvPQCmdqConfig_t;

#endif /* _DRVPQ_DEFINE_H_ */
