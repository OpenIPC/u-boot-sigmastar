/*
* drv_scl_pq_declare.h- Sigmastar
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
#ifndef _DRVPQ_DECLARE_H
#define _DRVPQ_DECLARE_H

///////////////////////////////////////////////////////////////////////////////
// function macro
//
//### MDrv_PQ_DumpTable
#define MDrv_PQ_DumpTable_(PQTABLE_NAME, pIP_Info) MDrv_PQ_DumpTable_##PQTABLE_NAME(pIP_Info)

#define DECLARE_MDrv_PQ_DumpTable_(PQTABLE_NAME) extern void MDrv_PQ_DumpTable_##PQTABLE_NAME(DrvPqIpInfo_t *pIP_Info);

#define INSTALL_MDrv_PQ_DumpTable_(PQTABLE_NAME)                   \
    void MDrv_PQ_DumpTable_##PQTABLE_NAME(DrvPqIpInfo_t *pIP_Info) \
    {                                                              \
        _MDrv_PQ_DumpTable(pIP_Info);                              \
    }

//### MDrv_PQ_GetIPNum
#define MDrv_PQ_GetIPNum_(PQTABLE_NAME) MDrv_PQ_GetIPNum_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_GetIPNum_(PQTABLE_NAME) extern u16 MDrv_PQ_GetIPNum_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_GetIPNum_(PQTABLE_NAME) \
    u16 MDrv_PQ_GetIPNum_##PQTABLE_NAME(void)   \
    {                                           \
        return _MDrv_PQ_GetIPNum();             \
    }

//### MDrv_PQ_GetTableNum
#define MDrv_PQ_GetTableNum_(PQTABLE_NAME, u8PQIPIdx) MDrv_PQ_GetTableNum_##PQTABLE_NAME(u8PQIPIdx)

#define DECLARE_MDrv_PQ_GetTableNum_(PQTABLE_NAME) extern u16 MDrv_PQ_GetTableNum_##PQTABLE_NAME(u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_GetTableNum_(PQTABLE_NAME)       \
    u16 MDrv_PQ_GetTableNum_##PQTABLE_NAME(u8 u8PQIPIdx) \
    {                                                    \
        return _MDrv_PQ_GetTableNum(u8PQIPIdx);          \
    }

//### MDrv_PQ_GetCurrentTableIndex
#define MDrv_PQ_GetCurrentTableIndex_(PQTABLE_NAME, u8PQIPIdx) MDrv_PQ_GetCurrentTableIndex_##PQTABLE_NAME(u8PQIPIdx)

#define DECLARE_MDrv_PQ_GetCurrentTableIndex_(PQTABLE_NAME) \
    extern u16 MDrv_PQ_GetCurrentTableIndex_##PQTABLE_NAME(u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_GetCurrentTableIndex_(PQTABLE_NAME)       \
    u16 MDrv_PQ_GetCurrentTableIndex_##PQTABLE_NAME(u8 u8PQIPIdx) \
    {                                                             \
        return _MDrv_PQ_GetCurrentTableIndex(u8PQIPIdx);          \
    }

//### DrvPQGetTableIndex
#define MDrv_PQ_GetTableIndex_(PQTABLE_NAME, u8TableIdx, u16PQSrcType, u8PQIPIdx) \
    MDrv_PQ_GetTableIndex_##PQTABLE_NAME(u8TableIdx, u16PQSrcType, u8PQIPIdx)

#define DECLARE_MDrv_PQ_GetTableIndex_(PQTABLE_NAME) \
    extern u16 MDrv_PQ_GetTableIndex_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_GetTableIndex_(PQTABLE_NAME)                                        \
    u16 MDrv_PQ_GetTableIndex_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx) \
    {                                                                                       \
        return _MDrv_PQ_GetTableIndex(u8TableIdx, u16PQSrcType, u8PQIPIdx);                 \
    }

//### MDrv_PQ_GetTable
#define MDrv_PQ_GetTable_(PQTABLE_NAME, u8TableIdx, u8TabIdx, u8PQIPIdx) \
    MDrv_PQ_GetTable_##PQTABLE_NAME(u8TableIdx, u8TabIdx, u8PQIPIdx)

#define DECLARE_MDrv_PQ_GetTable_(PQTABLE_NAME) \
    extern DrvPqIpInfo_t MDrv_PQ_GetTable_##PQTABLE_NAME(u8 u8TableIdx, u8 u8TabIdx, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_GetTable_(PQTABLE_NAME)                                             \
    DrvPqIpInfo_t MDrv_PQ_GetTable_##PQTABLE_NAME(u8 u8TableIdx, u8 u8TabIdx, u8 u8PQIPIdx) \
    {                                                                                       \
        return _MDrv_PQ_GetTable(u8TableIdx, u8TabIdx, u8PQIPIdx);                          \
    }

//### DrvPQLoadTableData
#define MDrv_PQ_LoadTableData_(PQTABLE_NAME, u8TabIdx, u8PQIPIdx, pTable, u16TableSize) \
    MDrv_PQ_LoadTableData_##PQTABLE_NAME(u8TabIdx, u8PQIPIdx, pTable, u16TableSize)

#define DECLARE_MDrv_PQ_LoadTableData_(PQTABLE_NAME) \
    extern void MDrv_PQ_LoadTableData_##PQTABLE_NAME(u8 u8TabIdx, u8 u8PQIPIdx, u8 *pTable, u16 u16TableSize);

#define INSTALL_MDrv_PQ_LoadTableData_(PQTABLE_NAME)                                                   \
    void MDrv_PQ_LoadTableData_##PQTABLE_NAME(u8 u8TabIdx, u8 u8PQIPIdx, u8 *pTable, u16 u16TableSize) \
    {                                                                                                  \
        _MDrv_PQ_LoadTableData(u8TabIdx, u8PQIPIdx, pTable, u16TableSize);                             \
    }

//### DrvPQLoadTable
#define MDrv_PQ_LoadTable_(PQTABLE_NAME, u8TableIdx, u8TabIdx, u8PQIPIdx) \
    MDrv_PQ_LoadTable_##PQTABLE_NAME(u8TableIdx, u8TabIdx, u8PQIPIdx)

#define DECLARE_MDrv_PQ_LoadTable_(PQTABLE_NAME) \
    extern void MDrv_PQ_LoadTable_##PQTABLE_NAME(u8 u8TableIdx, u8 u8TabIdx, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_LoadTable_(PQTABLE_NAME)                                    \
    void MDrv_PQ_LoadTable_##PQTABLE_NAME(u8 u8TableIdx, u8 u8TabIdx, u8 u8PQIPIdx) \
    {                                                                               \
        _MDrv_PQ_LoadTable(u8TableIdx, u8TabIdx, u8PQIPIdx);                        \
    }

//### MDrv_PQ_LoadCommTable
#define MDrv_PQ_LoadCommTable_(PQTABLE_NAME) MDrv_PQ_LoadCommTable_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_LoadCommTable_(PQTABLE_NAME) extern void MDrv_PQ_LoadCommTable_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_LoadCommTable_(PQTABLE_NAME) \
    void MDrv_PQ_LoadCommTable_##PQTABLE_NAME(void)  \
    {                                                \
        _MDrv_PQ_LoadCommTable();                    \
    }

//### MDrv_PQ_AddTable
#define MDrv_PQ_AddTable_(PQTABLE_NAME, u8TableIdx, pPQTableInfo) \
    MDrv_PQ_AddTable_##PQTABLE_NAME(u8TableIdx, pPQTableInfo)

#define DECLARE_MDrv_PQ_AddTable_(PQTABLE_NAME) \
    extern void MDrv_PQ_AddTable_##PQTABLE_NAME(u8 u8TableIdx, DrvPQTableTotalInfo_t *pPQTableInfo);

#define INSTALL_MDrv_PQ_AddTable_(PQTABLE_NAME)                                              \
    void MDrv_PQ_AddTable_##PQTABLE_NAME(u8 u8TableIdx, DrvPQTableTotalInfo_t *pPQTableInfo) \
    {                                                                                        \
        _MDrv_PQ_AddTable(u8TableIdx, pPQTableInfo);                                         \
    }

//### MDrv_PQ_LoadTableBySrcType

#define MDrv_PQ_LoadTableBySrcType_(PQTABLE_NAME, u8TableIdx, u16PQSrcType, u8PQIPIdx) \
    MDrv_PQ_LoadTableBySrcType_##PQTABLE_NAME(u8TableIdx, u16PQSrcType, u8PQIPIdx)

#define DECLARE_MDrv_PQ_LoadTableBySrcType_(PQTABLE_NAME) \
    extern void MDrv_PQ_LoadTableBySrcType_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_LoadTableBySrcType_(PQTABLE_NAME)                                         \
    void MDrv_PQ_LoadTableBySrcType_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx) \
    {                                                                                             \
        _MDrv_PQ_LoadTableBySrcType(u8TableIdx, u16PQSrcType, u8PQIPIdx);                         \
    }

//### MDrv_PQ_Set_CmdqCfg
#define MDrv_PQ_Set_CmdqCfg_(PQTABLE_NAME, CmdqCfg) MDrv_PQ_Set_CmdqCfg_##PQTABLE_NAME(CmdqCfg)

#define DECLARE_MDrv_PQ_Set_CmdqCfg_(PQTABLE_NAME) \
    extern void MDrv_PQ_Set_CmdqCfg_##PQTABLE_NAME(DrvPQCmdqConfig_t CmdqCfg);

#define INSTALL_MDrv_PQ_Set_CmdqCfg_(PQTABLE_NAME)                     \
    void MDrv_PQ_Set_CmdqCfg_##PQTABLE_NAME(DrvPQCmdqConfig_t CmdqCfg) \
    {                                                                  \
        _MDrv_PQ_Set_CmdqCfg(CmdqCfg);                                 \
    }
//### MDrv_PQ_Set_CmdqCfg
#define MDrv_PQ_GetIPRegCount_(PQTABLE_NAME, u8TableIdx, u16PQSrcType, u8PQIPIdx) \
    MDrv_PQ_GetIPRegCount_##PQTABLE_NAME(u8TableIdx, u16PQSrcType, u8PQIPIdx)

#define DECLARE_MDrv_PQ_GetIPRegCount_(PQTABLE_NAME) \
    extern u16 MDrv_PQ_GetIPRegCount_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_GetIPRegCount_(PQTABLE_NAME)                                        \
    u16 MDrv_PQ_GetIPRegCount_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx) \
    {                                                                                       \
        return _MDrv_PQ_GetIPRegCount(u8TableIdx, u16PQSrcType, u8PQIPIdx);                 \
    }

//### MDRv_PQ_Check_Reg
#define MDrv_PQ_Check_Type_(PQTABLE_NAME, EnCheck) MDrv_PQ_Check_Type_##PQTABLE_NAME(EnCheck)

#define DECLARE_MDrv_PQ_Check_Type_(PQTABLE_NAME) \
    extern void MDrv_PQ_Check_Type_##PQTABLE_NAME(E_PQ_CHECK_TYPE EnCheck);

#define INSTALL_MDrv_PQ_Check_Type_(PQTABLE_NAME)                   \
    void MDrv_PQ_Check_Type_##PQTABLE_NAME(E_PQ_CHECK_TYPE EnCheck) \
    {                                                               \
        _MDrv_PQ_Check_Type(EnCheck);                               \
    }

//### MDrv_PQ_LoadTableByData
#define MDrv_PQ_LoadTableByData_(PQTABLE_NAME, u8TableIdx, u16PQSrcType, u8PQIPIdx, pData) \
    MDrv_PQ_LoadTableByData_##PQTABLE_NAME(u8TableIdx, u16PQSrcType, u8PQIPIdx, pData)

#define DECLARE_MDrv_PQ_LoadTableByData_(PQTABLE_NAME)                                                \
    extern void MDrv_PQ_LoadTableByData_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx, \
                                                       DrvPQDataInfo_t *pData);

#define INSTALL_MDrv_PQ_LoadTableByData_(PQTABLE_NAME)                                                                 \
    void MDrv_PQ_LoadTableByData_##PQTABLE_NAME(u8 u8TableIdx, u16 u16PQSrcType, u8 u8PQIPIdx, DrvPQDataInfo_t *pData) \
    {                                                                                                                  \
        _MDrv_PQ_LoadTableByData(u8TableIdx, u16PQSrcType, u8PQIPIdx, pData);                                          \
    }

//### MDrv_PQ_CheckCommTable
#define MDrv_PQ_CheckCommTable_(PQTABLE_NAME) MDrv_PQ_CheckCommTable_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_CheckCommTable_(PQTABLE_NAME) extern void MDrv_PQ_CheckCommTable_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_CheckCommTable_(PQTABLE_NAME) \
    void MDrv_PQ_CheckCommTable_##PQTABLE_NAME(void)  \
    {                                                 \
        _MDrv_PQ_CheckCommTable();                    \
    }

//### MDrv_PQ_CheckTableBySrcType
#define MDrv_PQ_CheckTableBySrcType_(PQTABLE_NAME, u16PQSrcType, u8PQIPIdx) \
    MDrv_PQ_CheckTableBySrcType_##PQTABLE_NAME(u16PQSrcType, u8PQIPIdx)

#define DECLARE_MDrv_PQ_CheckTableBySrcType_(PQTABLE_NAME) \
    extern void MDrv_PQ_CheckTableBySrcType_##PQTABLE_NAME(u16 u16PQSrcType, u8 u8PQIPIdx);

#define INSTALL_MDrv_PQ_CheckTableBySrcType_(PQTABLE_NAME)                          \
    void MDrv_PQ_CheckTableBySrcType_##PQTABLE_NAME(u16 u16PQSrcType, u8 u8PQIPIdx) \
    {                                                                               \
        _MDrv_PQ_CheckTableBySrcType(u16PQSrcType, u8PQIPIdx);                      \
    }

//### MDrv_PQ_GetXRuleIPIndex
#define MDrv_PQ_GetXRuleIPIndex_(PQTABLE_NAME, u8XRuleType, u8XRuleIP) \
    MDrv_PQ_GetXRuleIPIndex_##PQTABLE_NAME(u8XRuleType, u8XRuleIP)

#define DECLARE_MDrv_PQ_GetXRuleIPIndex_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_GetXRuleIPIndex_##PQTABLE_NAME(u8 u8XRuleType, u8 u8XRuleIP);

#define INSTALL_MDrv_PQ_GetXRuleIPIndex_(PQTABLE_NAME)                      \
    u8 MDrv_PQ_GetXRuleIPIndex_##PQTABLE_NAME(u8 u8XRuleType, u8 u8XRuleIP) \
    {                                                                       \
        return _MDrv_PQ_GetXRuleIPIndex(u8XRuleType, u8XRuleIP);            \
    }

//### MDrv_PQ_GetXRuleTableIndex
#define MDrv_PQ_GetXRuleTableIndex_(PQTABLE_NAME, u8XRuleType, u8XRuleIdx, u8XRuleIP) \
    MDrv_PQ_GetXRuleTableIndex_##PQTABLE_NAME(u8XRuleType, u8XRuleIdx, u8XRuleIP)

#define DECLARE_MDrv_PQ_GetXRuleTableIndex_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_GetXRuleTableIndex_##PQTABLE_NAME(u8 u8XRuleType, u8 u8XRuleIdx, u8 u8XRuleIP);

#define INSTALL_MDrv_PQ_GetXRuleTableIndex_(PQTABLE_NAME)                                     \
    u8 MDrv_PQ_GetXRuleTableIndex_##PQTABLE_NAME(u8 u8XRuleType, u8 u8XRuleIdx, u8 u8XRuleIP) \
    {                                                                                         \
        return _MDrv_PQ_GetXRuleTableIndex(u8XRuleType, u8XRuleIdx, u8XRuleIP);               \
    }

//### MDrv_PQ_GetXRuleIPNum
#define MDrv_PQ_GetXRuleIPNum_(PQTABLE_NAME, u8XRuleType) MDrv_PQ_GetXRuleIPNum_##PQTABLE_NAME(u8XRuleType)

#define DECLARE_MDrv_PQ_GetXRuleIPNum_(PQTABLE_NAME) extern u8 MDrv_PQ_GetXRuleIPNum_##PQTABLE_NAME(u8 u8XRuleType);

#define INSTALL_MDrv_PQ_GetXRuleIPNum_(PQTABLE_NAME)        \
    u8 MDrv_PQ_GetXRuleIPNum_##PQTABLE_NAME(u8 u8XRuleType) \
    {                                                       \
        return _MDrv_PQ_GetXRuleIPNum(u8XRuleType);         \
    }

//### MDrv_PQ_GetGRule_LevelIndex
#define MDrv_PQ_GetGRule_LevelIndex_(PQTABLE_NAME, u8GRuleType, u8GRuleLevelIndex) \
    MDrv_PQ_GetGRule_LevelIndex_##PQTABLE_NAME(u8GRuleType, u8GRuleLevelIndex)

#define DECLARE_MDrv_PQ_GetGRule_LevelIndex_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_GetGRule_LevelIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8GRuleLevelIndex);

#define INSTALL_MDrv_PQ_GetGRule_LevelIndex_(PQTABLE_NAME)                              \
    u8 MDrv_PQ_GetGRule_LevelIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8GRuleLevelIndex) \
    {                                                                                   \
        return _MDrv_PQ_GetGRule_LevelIndex(u8GRuleType, u8GRuleLevelIndex);            \
    }

//### MDrv_PQ_GetGRule_IPIndex
#define MDrv_PQ_GetGRule_IPIndex_(PQTABLE_NAME, u8GRuleType, u8GRuleIPIndex) \
    MDrv_PQ_GetGRule_IPIndex_##PQTABLE_NAME(u8GRuleType, u8GRuleIPIndex)

#define DECLARE_MDrv_PQ_GetGRule_IPIndex_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_GetGRule_IPIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8GRuleIPIndex);

#define INSTALL_MDrv_PQ_GetGRule_IPIndex_(PQTABLE_NAME)                           \
    u8 MDrv_PQ_GetGRule_IPIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8GRuleIPIndex) \
    {                                                                             \
        return _MDrv_PQ_GetGRule_IPIndex(u8GRuleType, u8GRuleIPIndex);            \
    }

//### MDrv_PQ_GetGRule_TableIndex
#define MDrv_PQ_GetGRule_TableIndex_(PQTABLE_NAME, u8GRuleType, u8PQSrcType, u8PQ_NRIdx, u8GRuleIPIndex) \
    MDrv_PQ_GetGRule_TableIndex_##PQTABLE_NAME(u8GRuleType, u8PQSrcType, u8PQ_NRIdx, u8GRuleIPIndex)

#define DECLARE_MDrv_PQ_GetGRule_TableIndex_(PQTABLE_NAME)                                              \
    extern u8 MDrv_PQ_GetGRule_TableIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8PQSrcType, u8 u8PQ_NRIdx, \
                                                         u8 u8GRuleIPIndex);

#define INSTALL_MDrv_PQ_GetGRule_TableIndex_(PQTABLE_NAME)                                                          \
    u8 MDrv_PQ_GetGRule_TableIndex_##PQTABLE_NAME(u8 u8GRuleType, u8 u8PQSrcType, u8 u8PQ_NRIdx, u8 u8GRuleIPIndex) \
    {                                                                                                               \
        return _MDrv_PQ_GetGRule_TableIndex(u8GRuleType, u8PQSrcType, u8PQ_NRIdx, u8GRuleIPIndex);                  \
    }

//### MDrv_PQ_Set_MLoadEn
#define MDrv_PQ_Set_MLoadEn_(PQTABLE_NAME, bEn) MDrv_PQ_Set_MLoadEn_##PQTABLE_NAME(bEn)

#define DECLARE_MDrv_PQ_Set_MLoadEn_(PQTABLE_NAME) extern void MDrv_PQ_Set_MLoadEn_##PQTABLE_NAME(u8 bEn);

#define INSTALL_MDrv_PQ_Set_MLoadEn_(PQTABLE_NAME)  \
    void MDrv_PQ_Set_MLoadEn_##PQTABLE_NAME(u8 bEn) \
    {                                               \
        _MDrv_PQ_Set_MLoadEn(bEn);                  \
    }

//### MDrv_PQ_PreInitLoadTableInfo
#define MDrv_PQ_PreInitLoadTableInfo(PQTABLE_NAME) MDrv_PQ_PreInitLoadTableInfo_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_PreInitLoadTableInfo_(PQTABLE_NAME) \
    extern void MDrv_PQ_PreInitLoadTableInfo_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_PreInitLoadTableInfo_(PQTABLE_NAME) \
    void MDrv_PQ_PreInitLoadTableInfo_##PQTABLE_NAME(void)  \
    {                                                       \
        _MDrv_PQ_PreInitLoadTableInfo();                    \
    }

//### _MDrv_PQ_Set_LoadTableInfo_IP_Tab
#define MDrv_PQ_Set_LoadTableInfo_IP_Tab(PQTABLE_NAME, u16IPIdx, u8TabIdx) \
    MDrv_PQ_Set_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16IPIdx, u8TabIdx)

#define DECLARE_MDrv_PQ_Set_LoadTableInfo_IP_Tab_(PQTABLE_NAME) \
    extern void MDrv_PQ_Set_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16 u16IPIdx, u8 u8TabIdx);

#define INSTALL_MDrv_PQ_Set_LoadTableInfo_IP_Tab_(PQTABLE_NAME)                     \
    void MDrv_PQ_Set_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16 u16IPIdx, u8 u8TabIdx) \
    {                                                                               \
        _MDrv_PQ_Set_LoadTableInfo_IP_Tab(u16IPIdx, u8TabIdx);                      \
    }

//### _MDrv_PQ_Get_LoadTableInfo_IP_Tab
#define MDrv_PQ_Get_LoadTableInfo_IP_Tab(PQTABLE_NAME, u16IPIdx) \
    MDrv_PQ_Get_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16IPIdx)

#define DECLARE_MDrv_PQ_Get_LoadTableInfo_IP_Tab_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_Get_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16 u16IPIdx);

#define INSTALL_MDrv_PQ_Get_LoadTableInfo_IP_Tab_(PQTABLE_NAME)      \
    u8 MDrv_PQ_Get_LoadTableInfo_IP_Tab_##PQTABLE_NAME(u16 u16IPIdx) \
    {                                                                \
        return _MDrv_PQ_Get_LoadTableInfo_IP_Tab(u16IPIdx);          \
    }

//### _MDrv_PQ_Set_LoadTableInfo_SrcType
#define MDrv_PQ_Set_LoadTableInfo_SrcType(PQTABLE_NAME, u16SrcType) \
    MDrv_PQ_Set_LoadTableInfo_SrcType_##PQTABLE_NAME(u16SrcType)

#define DECLARE_MDrv_PQ_Set_LoadTableInfo_SrcType_(PQTABLE_NAME) \
    extern void MDrv_PQ_Set_LoadTableInfo_SrcType_##PQTABLE_NAME(u16 u16SrcType);

#define INSTALL_MDrv_PQ_Set_LoadTableInfo_SrcType_(PQTABLE_NAME)          \
    void MDrv_PQ_Set_LoadTableInfo_SrcType_##PQTABLE_NAME(u16 u16SrcType) \
    {                                                                     \
        _MDrv_PQ_Set_LoadTableInfo_SrcType(u16SrcType);                   \
    }

//### _MDrv_PQ_Get_LoadTableInfo_SrcType
#define MDrv_PQ_Get_LoadTableInfo_SrcType(PQTABLE_NAME) MDrv_PQ_Get_LoadTableInfo_SrcType_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_Get_LoadTableInfo_SrcType_(PQTABLE_NAME) \
    extern u16 MDrv_PQ_Get_LoadTableInfo_SrcType_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_Get_LoadTableInfo_SrcType_(PQTABLE_NAME) \
    u16 MDrv_PQ_Get_LoadTableInfo_SrcType_##PQTABLE_NAME(void)   \
    {                                                            \
        return _MDrv_PQ_Get_LoadTableInfo_SrcType();             \
    }

//### _MDrv_PQ_ClearTableIndex
#define MDrv_PQ_ClearTableIndex(PQTABLE_NAME) MDrv_PQ_ClearTableIndex_##PQTABLE_NAME()

#define DECLARE_MDrv_PQ_ClearTableIndex_(PQTABLE_NAME) extern void MDrv_PQ_ClearTableIndex_##PQTABLE_NAME(void);

#define INSTALL_MDrv_PQ_ClearTableIndex_(PQTABLE_NAME) \
    void MDrv_PQ_ClearTableIndex_##PQTABLE_NAME(void)  \
    {                                                  \
        _MDrv_PQ_ClearTableIndex();                    \
    }

//### MDrv_PQ_LoadPictureSetting
#define MDrv_PQ_LoadPictureSetting_(PQTABLE_NAME, u8TabIdx, u8PQIPIdx, pTable, u16TableSize) \
    MDrv_PQ_LoadPictureSetting_##PQTABLE_NAME(u8TabIdx, u8PQIPIdx, pTable, u16TableSize)

#define DECLARE_MDrv_PQ_LoadPictureSetting_(PQTABLE_NAME) \
    extern u8 MDrv_PQ_LoadPictureSetting_##PQTABLE_NAME(u8 u8TabIdx, u8 u8PQIPIdx, void *pTable, u16 u16TableSize);

#define INSTALL_MDrv_PQ_LoadPictureSetting_(PQTABLE_NAME)                                                   \
    u8 MDrv_PQ_LoadPictureSetting_##PQTABLE_NAME(u8 u8TabIdx, u8 u8PQIPIdx, void *pTable, u16 u16TableSize) \
    {                                                                                                       \
        return _MDrv_PQ_LoadPictureSetting(u8TabIdx, u8PQIPIdx, pTable, u16TableSize);                      \
    }

#define DECLARE_PQ_FUNCTIONS(table_name)                                                                               \
    DECLARE_MDrv_PQ_LoadCommTable_(table_name) DECLARE_MDrv_PQ_DumpTable_(table_name) DECLARE_MDrv_PQ_GetIPNum_(       \
        table_name) DECLARE_MDrv_PQ_GetTableNum_(table_name) DECLARE_MDrv_PQ_GetTableIndex_(table_name)                \
        DECLARE_MDrv_PQ_GetCurrentTableIndex_(table_name) DECLARE_MDrv_PQ_GetTable_(                                   \
            table_name) DECLARE_MDrv_PQ_LoadTableData_(table_name) DECLARE_MDrv_PQ_LoadTable_(table_name)              \
            DECLARE_MDrv_PQ_LoadCommTable_(table_name) DECLARE_MDrv_PQ_LoadTableBySrcType_(                            \
                table_name) DECLARE_MDrv_PQ_Set_CmdqCfg_(table_name) DECLARE_MDrv_PQ_GetIPRegCount_(table_name)        \
                DECLARE_MDrv_PQ_Check_Type_(table_name) DECLARE_MDrv_PQ_LoadTableByData_(table_name)                   \
                    DECLARE_MDrv_PQ_CheckCommTable_(table_name) DECLARE_MDrv_PQ_CheckTableBySrcType_(table_name)       \
                        DECLARE_MDrv_PQ_AddTable_(table_name) DECLARE_MDrv_PQ_GetXRuleIPIndex_(table_name)             \
                            DECLARE_MDrv_PQ_GetXRuleTableIndex_(table_name) DECLARE_MDrv_PQ_GetXRuleIPNum_(table_name) \
                                DECLARE_MDrv_PQ_GetGRule_LevelIndex_(table_name) DECLARE_MDrv_PQ_GetGRule_IPIndex_(    \
                                    table_name) DECLARE_MDrv_PQ_GetGRule_TableIndex_(table_name)                       \
                                    DECLARE_MDrv_PQ_Set_MLoadEn_(table_name)                                           \
                                        DECLARE_MDrv_PQ_PreInitLoadTableInfo_(table_name)                              \
                                            DECLARE_MDrv_PQ_Set_LoadTableInfo_IP_Tab_(table_name)                      \
                                                DECLARE_MDrv_PQ_Get_LoadTableInfo_IP_Tab_(table_name)                  \
                                                    DECLARE_MDrv_PQ_Set_LoadTableInfo_SrcType_(table_name)             \
                                                        DECLARE_MDrv_PQ_Get_LoadTableInfo_SrcType_(table_name)         \
                                                            DECLARE_MDrv_PQ_ClearTableIndex_(table_name)               \
                                                                DECLARE_MDrv_PQ_LoadPictureSetting_(table_name)

#define INSTALL_PQ_FUNCTIONS(table_name)                                                                      \
    INSTALL_MDrv_PQ_DumpTable_(table_name) INSTALL_MDrv_PQ_GetIPNum_(table_name)                              \
        INSTALL_MDrv_PQ_GetTableNum_(table_name) INSTALL_MDrv_PQ_GetIPRegCount_(table_name)                   \
            INSTALL_MDrv_PQ_GetTableIndex_(table_name) INSTALL_MDrv_PQ_GetCurrentTableIndex_(table_name)      \
                INSTALL_MDrv_PQ_GetTable_(table_name) INSTALL_MDrv_PQ_Set_CmdqCfg_(table_name)                \
                    INSTALL_MDrv_PQ_Check_Type_(table_name) INSTALL_MDrv_PQ_LoadTableByData_(table_name)      \
                        INSTALL_MDrv_PQ_AddTable_(table_name) INSTALL_MDrv_PQ_LoadTableBySrcType_(table_name) \
                            INSTALL_MDrv_PQ_ClearTableIndex_(table_name) INSTALL_MDrv_PQ_LoadTable_(table_name)

DECLARE_PQ_FUNCTIONS(MAIN) // table config parameter
DECLARE_PQ_FUNCTIONS(Dspy) // table config parameter

// SC
//#include "Infinity3e_Main.h"              // table config parameter
//#include "Infinity3e_Main_HSPRule.h"      // table config parameter
//#include "Infinity3e_Main_VSPRule.h"      // table config parameter
//#include "Infinity3e_Main_GRule.h"        // table config parameter
//[Daniel]

#endif
