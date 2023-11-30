/*
* drv_disp_pq_bin.h- Sigmastar
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

#ifndef _DRV_PQ_BIN_H_
#define _DRV_PQ_BIN_H_

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------

#define PQ_BIN_MAX_IP  256
#define PQ_BIN_MAX_PNL 30

#define PQ_BIN_IP_ALL    0xFFFF
#define PQ_BIN_IP_NULL   0xFFFF
#define PQ_BIN_IP_COMM   0xFFFE
#define PQ_BIN_ADDR_NULL 0

#define PQ_BIN_BANK_SIZE 1
#define PQ_BIN_ADDR_SIZE 1
#define PQ_BIN_MASK_SIZE 1

#define PQ_BIN_HEADER_LEN  16
#define PQ_BIN_VERSION_LEN 16
#define PQ_BIN_DUMMY       7

#define PQ_BIN_HEADER_LEN_1  17
#define PQ_BIN_VERSION_LEN_1 17
#define PQ_BIN_DUMMY_1       9

#define PQ_TEXT_BIN_HEADER_LEN  16
#define PQ_TEXT_BIN_VERSION_LEN 16
#define PQ_TEXT_BIN_DUMMY       8

#define PQ_BIN_ENABLE_CHECK 0

// Size of each table
#define PQ_BIN_GRULE_INFO_SIZE       14
#define PQ_BIN_GRULE_LEVEL_INFO_SIZE 6
#define PQ_BIN_XRULE_INFO_SIZE       12
#define PQ_BIN_SKIPRULE_INFO_SIZE    6
#define PQ_BIN_IP_COMM_INFO_SIZE     16
#define PQ_BIN_SOURCELUT_INFO_SIZE   8

// Item Offset of each table
// GRule
#define PQ_BIN_GRULE_HEADER_LEN    (16)
#define PQ_BIN_GRULE_RULE_NUM_LEN  (2)
#define PQ_BIN_GRULE_PANEL_NUM_LEN (2)

#define PQ_BIN_GRULE_IP_NUM_OFFSET     (0)
#define PQ_BIN_GRULE_SOURCE_NUM_OFFSET (PQ_BIN_GRULE_IP_NUM_OFFSET + 2)
#define PQ_BIN_GRULE_GROUP_NUM_OFFSET  (PQ_BIN_GRULE_SOURCE_NUM_OFFSET + 2)
#define PQ_BIN_GRULE_IPOFFSET_OFFSET   (PQ_BIN_GRULE_GROUP_NUM_OFFSET + 2)
#define PQ_BIN_GRULE_RULEOFFSET_OFFSET (PQ_BIN_GRULE_IPOFFSET_OFFSET + 4)

#define PQ_BIN_GRULE_LEVEL_LVL_NUM_OFFSET (0)
#define PQ_BIN_GRULE_LEVEL_OFFSET_OFFSET  (PQ_BIN_GRULE_LEVEL_LVL_NUM_OFFSET + 2)

// XRule
#define PQ_BIN_XRULE_HEADER_LEN   (16)
#define PQ_BIN_XRULE_RULE_NUM_LEN (2)

#define PQ_BIN_XRULE_IP_NUM_OFFSET      (0)
#define PQ_BIN_XRULE_GROUP_NUM_OFFSET   (PQ_BIN_XRULE_IP_NUM_OFFSET + 2)
#define PQ_BIN_XRULE_IPOFFSET_OFFSET    (PQ_BIN_XRULE_GROUP_NUM_OFFSET + 2)
#define PQ_BIN_XRULE_GROUPOFFSET_OFFSET (PQ_BIN_XRULE_IPOFFSET_OFFSET + 4)

// SkipRule
#define PQ_BIN_SKIPRULE_HEADER_LEN    (16)
#define PQ_BIN_SKIPRULE_IP_NUM_OFFSET (0)
#define PQ_BIN_SKIPRULE_OFFSET_OFFSET (PQ_BIN_SKIPRULE_IP_NUM_OFFSET + 2)

// IP & Common
#define PQ_BIN_IP_COMM_HEADER_LEN          (16)
#define PQ_BIN_IP_COMM_IP_NUM_LEN          (2)
#define PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET   (0)
#define PQ_BIN_IP_COMM_COMM_REG_NUM_OFFSET (PQ_BIN_IP_COMM_TABLE_TYPE_OFFSET + 2)
#define PQ_BIN_IP_COMM_IP_REG_NUM_OFFSET   (PQ_BIN_IP_COMM_COMM_REG_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_IP_GROUP_NUM_OFFSET (PQ_BIN_IP_COMM_IP_REG_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_COMMOFFSET_OFFSET   (PQ_BIN_IP_COMM_IP_GROUP_NUM_OFFSET + 2)
#define PQ_BIN_IP_COMM_IPOFFSET_OFFSET     (PQ_BIN_IP_COMM_COMMOFFSET_OFFSET + 4)

// SourceLUT
#define PQ_BIN_SOURCELUT_HEADER_LEN    (16)
#define PQ_BIN_SOURCELUT_PANEL_NUM_LEN (2)

#define PQ_BIN_SOURCELUT_SOURCE_NUM_OFFSET (0)
#define PQ_BIN_SOURCELUT_IP_NUM_OFFSET     (PQ_BIN_SOURCELUT_SOURCE_NUM_OFFSET + 2)
#define PQ_BIN_SOURCELUT_OFFSET_OFFSET     (PQ_BIN_SOURCELUT_IP_NUM_OFFSET + 2)

// Source Info
#define PQ_BIN_IP_INFO_DEVICE_LEN    (8)
#define PQ_BIN_IP_INFO_INTERFACE_LEN (8)
#define PQ_BIN_IP_INFO_WIDHT         (16)
#define PQ_BIN_IP_INFO_HEIGHT        (16)
#define PQ_BIN_IP_INFO_INFO_SIZE \
    (PQ_BIN_IP_INFO_DEVICE_LEN + PQ_BIN_IP_INFO_INTERFACE_LEN + PQ_BIN_IP_INFO_WIDHT + PQ_BIN_IP_INFO_HEIGHT)

#define PQ_BIN_IP_INFO_DEVICE_IDX    (0)
#define PQ_BIN_IP_INFO_INTERFACE_IDX (PQ_BIN_IP_INFO_DEVICE_LEN / 8)
#define PQ_BIN_IP_INFO_WIDTH_IDX     (PQ_BIN_IP_INFO_INTERFACE_IDX + PQ_BIN_IP_INFO_DEVICE_LEN / 8)
#define PQ_BIN_IP_INFO_HEIGHT_IDX    (PQ_BIN_IP_INFO_WIDTH_IDX + PQ_BIN_IP_INFO_WIDHT / 8)

#define PQ_BIN_SOURCELUT_DATA_LEN 2 // 2 bytes
#define PQ_BIN_IP_COMM_DATA_LEN   1 // 1 byte

#define PQ_BIN_INFO_IP_IDX    (0)
#define PQ_BIN_PICTURE_IP_DIX (1)

#define PQ_BIN_INFO_IP_INTF_NONE    (0x00)
#define PQ_BIN_INFO_IP_INTF_HDMI    (0x01)
#define PQ_BIN_INFO_IP_INTF_VGA     (0x02)
#define PQ_BIN_INFO_IP_INTF_CVBS    (0x03)
#define PQ_BIN_INFO_IP_INTF_BT1120  (0x04)
#define PQ_BIN_INFO_IP_INTF_BT656   (0x05)
#define PQ_BIN_INFO_IP_INTF_BT601   (0x06)
#define PQ_BIN_INFO_IP_INTF_TTL     (0x07)
#define PQ_BIN_INFO_IP_INTF_MIPIDSI (0x08)
#define PQ_BIN_INFO_IP_INTF_MCU     (0x09)
#define PQ_BIN_INFO_IP_INTF_SRGB    (0x0A)

// Text Bin
// Size of each table
#define PQ_TEXT_BIN_SRCTYPE_INFO_SIZE   6
#define PQ_TEXT_BIN_IPNAME_INFO_SIZE    6
#define PQ_TEXT_BIN_GROUPNAME_INFO_SIZE 6

// SrcType
#define PQ_TEXT_BIN_SRCTYPE_HEADER_LEN     (16)
#define PQ_TEXT_BIN_SRCTYPE_SRCNUM_LEN     (2)
#define PQ_TEXT_BIN_SRCTYPE_SRC_NUM_OFFSET (0)
#define PQ_TEXT_BIN_SRCTYPE_OFFSET_OFFSET  (PQ_TEXT_BIN_SRCTYPE_SRC_NUM_OFFSET + 2)

// IPName
#define PQ_TEXT_BIN_IPNAME_HEADER_LEN    (16)
#define PQ_TEXT_BIN_IPNAME_IPNUM_LEN     (2)
#define PQ_TEXT_BIN_IPNAME_IPNUM_OFFSET  (0)
#define PQ_TEXT_BIN_IPNAME_OFFSET_OFFSET (PQ_TEXT_BIN_IPNAME_IPNUM_OFFSET + 2)

// GroupName
#define PQ_TEXT_BIN_GROUPNAME_HEADER_LEN              (16)
#define PQ_TEXT_BIN_GROUPNAME_IPNUM_LEN               (2)
#define PQ_TEXT_BIN_GROUPNAME_IPGROUP_GROUPNUM_OFFSET (0)
#define PQ_TEXT_BIN_GROUPNAME_IPGROUP_OFFSET_OFFSET   (PQ_TEXT_BIN_GROUPNAME_IPGROUP_GROUPNUM_OFFSET + 2)

#define PQ_TEXT_BIN_GROUPNAME_GROUPOFFSET_LEN    (4)
#define PQ_TEXT_BIN_GROUPNAME_GROUPLENGTH_LEN    (2)
#define PQ_TEXT_BIN_GROUPNAME_GROUPOFFSET_OFFSET (0)

//-------------------------------------------------------------------------------------------------
//  enum
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_PQ_BIN_DISPLAY_ONE = 0,
    E_PQ_BIN_DISPLAY_PIP,
    E_PQ_BIN_DISPLAY_POP,
    E_PQ_BIN_DISPLAY_NUM,
} PQ_BIN_DISPLAY_TYPE_e;

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct DrvPQBinSourceLUTInfo_s
{
    u16 u16SourceNum;
    u16 u16IPNum;
    u32 u32Offset;
} DrvPQBinSourceLUTInfo_t;

typedef struct DrvPQBinHeaderInfo_s
{
    u8             u8BinID;
    ss_phys_addr_t paBinStartAddress;
    void *         pvBinStartAddress;
    u8             u8Header[PQ_BIN_HEADER_LEN_1];
    u8             u8Version[PQ_BIN_VERSION_LEN_1];
    u32            u32StartOffset;
    u32            u32EndOffset;
    u8             u8Dummy[PQ_BIN_DUMMY_1];
    u8             u8PQID;
    u32            u32DevID;
    // GRule
    u8  u8GRuleHader[PQ_BIN_HEADER_LEN_1];
    u16 u16GRule_RuleNum;
    u16 u16GRule_PnlNum;
    u32 u32GRule_Offset;
    u32 u32GRule_Pitch;
    u32 u32GRule_Lvl_Offset;
    u32 u32GRule_Lvl_Pitch;

    // XRule
    u8  u8XRuleHader[PQ_BIN_HEADER_LEN_1];
    u16 u16XRuleNum;
    u32 u32XRule_Offset;
    u32 u32XRUle_Pitch;

    // SkipRule
    u8  u8SkipRuleHader[PQ_BIN_HEADER_LEN_1];
    u16 u16SkipRule_IPNum;
    u32 u32SkipRule_Offset;

    // IP & Common
    u8  u8IP_Comm_Header[PQ_BIN_HEADER_LEN_1];
    u16 u16IP_Comm_Num;
    u32 u32IP_Comm_Offset;
    u32 u32IP_Comm_Pitch;

    // SourceLUT
    u8  u8SourceLUT_Header[PQ_BIN_HEADER_LEN_1];
    u16 u16SourceLUT_PnlNum;
    u32 u32SourceLUT_Offset;
    u32 u32SourceLUT_Pitch;
} DrvPQBinHeaderInfo_t;

typedef struct DrvPQTextBinHeaderInfo_s
{
    u8             u8BinID;
    void *         pvBinStartAddress;
    ss_phys_addr_t paBinStartAddress;
    u8             u8Header[PQ_TEXT_BIN_HEADER_LEN];
    u8             u8Version[PQ_TEXT_BIN_HEADER_LEN];
    u32            u32StartOffset;
    u32            u32EndOffset;
    u8             u8Dummy[PQ_TEXT_BIN_HEADER_LEN];

    // SourceType
    u8  u8SrcTypeHeader[PQ_TEXT_BIN_HEADER_LEN];
    u16 u16SrcNum;
    u32 u32SrcType_Offset;
    u32 u32SrcType_Pitch;

    // IPName
    u8  u8IPNameHeader[PQ_TEXT_BIN_HEADER_LEN];
    u16 u16IPNum;
    u32 u32IPName_Offset;
    u32 u32IPName_Pitch;

    // GroupName
    u8  u8GroupNameHeader[PQ_TEXT_BIN_HEADER_LEN];
    u16 u16GroupIPNum;
    u32 u32IP_Group_Offset;
    u32 u32IP_Group_pitch;
} DrvPQTextBinHeaderInfo_t;

typedef struct
{
    u32             u32TabOffset;
    void *          pvAddOffAddr;
    u16             u16RegNum;
    u16             u16TableType; // E_PQ_TABTYPE_e
    u16             u16GroupNum;  // each sheet group list count
    u16             u16GroupIdx;  // set group type ,ex. off/on/normal ,depend on sheet SRC.
    E_PQ_WIN_TYPE_e ePQWin;
    u32             u32DevId;
} DrvPQBinIPTableInfo_t;

u16  DrvDispPQBinGetSourceLutPnlNum(DrvPQBinHeaderInfo_t *pPQBinHeader);
void DrvDispPQBinGetSourceLut(DrvPQBinHeaderInfo_t *pPQBinHeader, u16 u16SourceLutPnlIdx,
                              DrvPQBinSourceLUTInfo_t *pSrcLutInfo);

void DrvDispPQBinParsing(DrvPQBinHeaderInfo_t *pPQBinHeader);
void DrvDispPQBinLoadCommTable(u16 u16PnlIdx, DrvPQBinHeaderInfo_t *pPQBinHeader);
u16  DrvDispPQBinGetIPNum(DrvPQBinHeaderInfo_t *pPQBinHeader);
void DrvDispPQBinLoadTableData(u16 u16PnlIdx, u16 u16TabIdx, u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader,
                               u8 *pTable, u16 u16TableSize);
void DrvDispPQBinLoadTable(u16 u16PnlIdx, u16 u16TabIdx, u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader);
u16  DrvDispPQBinGetTableIndex(u16 u16PQSrcType, u16 u16PQIPIdx, u16 u16PQPnlIdx, DrvPQBinHeaderInfo_t *pPQBinHeader);
u16  DrvDispPQBinGetSkipRule(u16 u16PQIPIdx, DrvPQBinHeaderInfo_t *pPQBinHeader);
void DrvDispPQBinLoadTableBySrcType(u16 u16PQSrcType, u16 u16PQIPIdx, u16 u16PQPnlIdx,
                                    DrvPQBinHeaderInfo_t *pPQBinHeader);

void                  DrvDispPQBinSetDisplayType(PQ_BIN_DISPLAY_TYPE_e enDisplayType, E_PQ_WIN_TYPE_e ePQWin);
PQ_BIN_DISPLAY_TYPE_e DrvDispPQBinGetDisplayType(E_PQ_WIN_TYPE_e ePQWin);

void DrvDispPQBinSetPanelID(u8 u8PnlID, E_PQ_WIN_TYPE_e ePQWin);
u8   DrvDispPQBinGetPanelIdx(E_PQ_WIN_TYPE_e ePQWin);

E_PQ_WIN_TYPE_e DrvDispPQBinTransToPQWin(u8 u8BinID);
void            DrvDispPQBinClearSRAMTableIndex(void);

#endif
