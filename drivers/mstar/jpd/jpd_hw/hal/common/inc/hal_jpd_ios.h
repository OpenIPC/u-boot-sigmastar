/*
 * hal_jpd_ios.h- Sigmastar
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

#ifndef _HAL_JPD_IOS_H_
#define _HAL_JPD_IOS_H_

#include "mhal_common.h"

extern unsigned long g_njpdregisterBase[];
//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------
// NJPD debug level enum
typedef enum
{
    E_NJPD_DEBUG_HAL_NONE    = 0x0,
    E_NJPD_DEBUG_HAL_ERR     = 0x1,
    E_NJPD_DEBUG_HAL_WARNING = 0x2,
    E_NJPD_DEBUG_HAL_MSG     = 0x3,
    E_NJPD_DEBUG_HAL_INFO    = 0x4,
    E_NJPD_DEBUG_HAL_DEBUG   = 0x5
} EN_NJPD_HAL_DBG_LEVEL;

typedef enum
{
    E_NJPD_OUTPUT_ORIGINAL = 0x0,
    E_NJPD_OUTPUT_YC_SWAP  = 0x01,
    E_NJPD_OUTPUT_UV_SWAP  = 0x02,
    E_NJPD_OUTPUT_UV_7BIT  = 0x03,
    E_NJPD_OUTPUT_UV_MSB   = 0x04
} EN_NJPD_OUTPUT_FORMAT;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

void   MhalJpdSetMIU(MS_U8 eNJPDNum, MS_U8 u8Idx);
void   MhalJpdSetGlobalSetting00(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 HalJpdGetGlobalSetting00(MS_U8 eNJPDNum);
void   MhalJpdSetGlobalSetting01(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 HalJpdGetGlobalSetting01(MS_U8 eNJPDNum);
void   MhalJpdSetGlobalSetting02(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetGlobalSetting02(MS_U8 eNJPDNum);

void MhalJpdSetMRCBuf0StartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf0StartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf0EndLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf0EndHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf1StartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf1StartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf1EndLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMRCBuf1EndHigh(MS_U8 eNJPDNum, MS_U16 u16Value);

void   MhalJpdSetMRCStartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetMRCStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetMWCBufStartLow(MS_U8 eNJPDNum);
MS_U16 MhalJpdGetMWCBufStartHigh(MS_U8 eNJPDNum);
MS_U16 MhalJpdGetMWCBufWritePtrLow(MS_U8 eNJPDNum);
MS_U16 MhalJpdGetMWCBufWritePtrHigh(MS_U8 eNJPDNum);

void MhalJpdSetMWCBufStartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetMWCBufStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetPicHorizontal(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalSetPicVertical(MS_U8 eNJPDNum, MS_U16 u16Value);

void   MhalJpdClearEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdForceEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdMaskEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetEventFlag(MS_U8 eNJPDNum);

void MhalJpdSetROIHorizontal(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhaljpdSetROIVertical(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetROIWidth(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetROIHeight(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetClockGate(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetSramClockGate(MS_U8 eNJPDNum, MS_U32 u32Value);

void MhalJpdInitRegBase(int u32DevId, MS_U64 JPD_Base);
void MhalJpdInitClk(int u32DevId, MS_U64 JPD_ClkBase);
void MhalJpdSetRSTIntv(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetDbgLevel(MS_U8 u8DbgLevel);
void MhalJpdRst(MS_U8 eNJPDNum);

MS_U32 MhalJpdGetCurMRCAddr(MS_U8 eNJPDNum);
MS_U16 MhalJpdGetCurRow(MS_U8 eNJPDNum);
MS_U16 MhalJpdGetCurCol(MS_U8 eNJPDNum);
void   MhalJpdSetAutoProtect(MS_U8 eNJPDNum, MS_BOOL enable);

void   MhalJpdSetMRBurstThd(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetMARB06(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetMARB06(MS_U8 eNJPDNum);
void   MhalJpdSetMARB07(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetMARB07(MS_U8 eNJPDNum);

void MhalJpdSetWriteProtectEnableUnbound0Low(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetWriteProtectEnableUnbound0High(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetWriteProtectEnableBound0Low(MS_U8 eNJPDNum, MS_U16 u16Value);
void MhalJpdSetWriteProtectEnableBound0HIgh(MS_U8 eNJPDNum, MS_U16 u16Value);

void   MhalJpdSetSpare00(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare00(MS_U8 eNJPDNum);
void   MhalJpdSetSpare01(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare01(MS_U8 eNJPDNum);
void   MhalJpdSetSpare02(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare02(MS_U8 eNJPDNum);
void   MhalJpdSetSpare03(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare03(MS_U8 eNJPDNum);
void   MhalJpdSetSpare04(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare04(MS_U8 eNJPDNum);
void   MhalJpdSetSpare05(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare05(MS_U8 eNJPDNum);
void   MhalJpdSetSpare06(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare06(MS_U8 eNJPDNum);
void   MhalJpdSetSpare07(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetSpare07(MS_U8 eNJPDNum);
void   MhalJpdSetWriteOneClearReg(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdGetWriteOneClearReg2(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetWriteOneClearReg(MS_U8 eNJPDNum);
void   MhalJpdSetHTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetHTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetQTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetQTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetGTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetGTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetHTableSize(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetHTableSize(MS_U8 eNJPDNum);
void   MhalJpdSetRIUInterface(MS_U8 eNJPDNum, MS_U16 u16Value);
MS_U16 MhalJpdGetRIUInterface(MS_U8 eNJPDNum);
MS_U16 MhalJpdTBCReadDataLow(MS_U8 eNJPDNum);
MS_U16 MhalJpdTBCReadDataHigh(MS_U8 eNJPDNum);
void   MhalJpdSetIBufReadLength(MS_U8 eNJPDNum, MS_U8 u8Min, MS_U8 u8Max);
void   MhalJpdSetCRCReadMode(MS_U8 eNJPDNum);
void   MhalJpdSetCRCWriteMode(MS_U8 eNJPDNum);
void   MhalJpdSetOutputFormat(MS_U8 eNJPDNum, MS_BOOL bRst, EN_NJPD_OUTPUT_FORMAT eOutputFormat);
void   MhalJpdDebug(MS_U8 eNJPDNum);
void   MhalJpdSetMRCBuf64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetMWCBuf64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdSetTable64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value);
void   MhalJpdPZ1SetFlag(MS_U16 u16Value);
#ifdef NJPD_CMDQ_SUPPORT
void MhalJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf);
void MhalJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen);
#endif
void MhalJpdWrite(MS_U8 u8DevID, MS_U32 u32Reg, MS_U16 u16Val);
#endif //_HAL_JPD_IOS_H_
