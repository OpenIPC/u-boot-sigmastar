/*
 * hal_jpd_ios.c- Sigmastar
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

#include <cam_os_wrapper.h>
#include "hal_jpd_ios.h"
#include "hal_jpd_ops.h"
#include "hal_jpd_reg.h"
#include "hal_jpd_def.h"

//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------
#define HAL_NJPD_ENABLE  1
#define HAL_NJPD_DISABLE 0

#define HAL_NJPD_MIU_PROTECT HAL_NJPD_ENABLE

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

#define __HAL_NJPD_DelayMs(x)        \
    do                               \
    {                                \
        MS_U32 ticks = 0;            \
        while ((ticks++) > (x * 10)) \
            ;                        \
    } while (0)

//#define NJPD_RIU_MAP 0xFD000000  //obtain in init
//#define NJPD_RIU_MAP 0  //obtain in init

typedef struct
{
    void *  pCmdQHandle;
    MS_U32  u32CmdLen;
    MS_U64 *pu64CmdqBuf;
} ST_JpdCmdqCfg;

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

unsigned long g_njpdregisterBase[MAX_JPD_DEVICE_NUM];
unsigned long g_jpdClkRegisterBase[MAX_JPD_DEVICE_NUM];
#ifdef NJPD_CMDQ_SUPPORT
ST_JpdCmdqCfg g_stJpdCmdqCfg[MAX_JPD_DEVICE_NUM];
#endif

void MhalJpdWrite(MS_U8 u8DevID, MS_U32 u32Reg, MS_U16 u16Val)
{
#ifdef NJPD_CMDQ_SUPPORT
    if (g_stJpdCmdqCfg[u8DevID].pCmdQHandle && g_stJpdCmdqCfg[u8DevID].pu64CmdqBuf)
    {
        // FIXME:just write reg for read
        if (BK_NJPD1_WRITE_ONE_CLEAR != u32Reg || NJPD_DECODE_ENABLE != u16Val)
        {
            NJPD_WRITE_WORD(u8DevID, u32Reg, u16Val);
        }

        NJPD_WRITE_CMD(u8DevID, u32Reg, u16Val, g_stJpdCmdqCfg[u8DevID].pu64CmdqBuf,
                       &g_stJpdCmdqCfg[u8DevID].u32CmdLen);
    }
    else
    {
        NJPD_WRITE_WORD(u8DevID, u32Reg, u16Val);
    }
#else
    NJPD_WRITE_WORD(u8DevID, u32Reg, u16Val);
#endif
}

#ifdef NJPD_CMDQ_SUPPORT
void MhalJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf)
{
    g_stJpdCmdqCfg[u8DevId].pCmdQHandle = pCmdqHandle;
    g_stJpdCmdqCfg[u8DevId].pu64CmdqBuf = pu64CmdqBuf;
    g_stJpdCmdqCfg[u8DevId].u32CmdLen   = 0;
    JPD_LOGD("JPD%d HAL_NJPD_Cmdq_point %x. \n", u8DevId, pCmdqHandle);
}

void MhalJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen)
{
    if (ppCmdqHandle)
    {
        *ppCmdqHandle = g_stJpdCmdqCfg[u8DevId].pCmdQHandle;
    }

    if (ppu64CmdqBuf)
    {
        *ppu64CmdqBuf = g_stJpdCmdqCfg[u8DevId].pu64CmdqBuf;
    }

    if (pu32CmdLen)
    {
        *pu32CmdLen = g_stJpdCmdqCfg[u8DevId].u32CmdLen;
    }
}
#endif
void MhalJpdSetMiuProtectMask(MS_U8 eNJPDNum, MS_BOOL bEnable)
{
#if (HAL_NJPD_MIU_PROTECT == HAL_NJPD_ENABLE)
    // MhalJpdWriteBit_miu(eNJPDNum,NJPD_MIU0_RQ2_MASK, bEnable, NJPD0_MIU0_CLIENT_NJPD);
    // MhalJpdWriteBit(eNJPDNum,NJPD_MIU1_RQ2_MASK, bEnable, NJPD0_MIU1_CLIENT_NJPD);

    // JPD_LOGD("MIU0(0x%x) : 0x%04x\n", NJPD_MIU0_RQ2_MASK,
    // NJPD_READ_WORD(NJPD_MIU0_RQ2_MASK));
    // JPD_LOGD("MIU1(0x%x) : 0x%04x\n", NJPD_MIU1_RQ2_MASK,
    // MhalJpdRead2Byte(eNJPDNum,NJPD_MIU1_RQ2_MASK));
#endif
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetMIU(MS_U8 eNJPDNum, MS_U8 u8Idx)
{
    MS_U16 u16Reg;
    JPD_LOGD("NJPD0, ");
    // MhalJpdWriteBit(eNJPDNum,NJPD0_MIU1_MIU_SEL2, 1, NJPD0_MIU1_CLIENT_NJPD);

    u16Reg = (MS_U16)u8Idx;
    u16Reg = u16Reg << NJPD_MIU_SEL_SHIFT;
    u16Reg = u16Reg | (MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IBUF_READ_LENGTH) & ~(NJPD_MIU_SEL | NJPD_MIU_SEL_1));
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IBUF_READ_LENGTH, u16Reg);

    JPD_LOGD("NJPD0(0x%x) : 0x%04x\n", BK_NJPD1_IBUF_READ_LENGTH, u16Reg);
}

void MhalJpdSetGlobalSetting00(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING00, u16Value);
    JPD_LOGD("HAL_NJPD1_Get_GlobalSetting00 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING00));
}

MS_U16 HalJpdGetGlobalSetting00(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING00);
}

void MhalJpdSetGlobalSetting01(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING01, u16Value);
    JPD_LOGD("HAL_NJPD1_Get_GlobalSetting01 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING01));
}

MS_U16 HalJpdGetGlobalSetting01(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING01);
}

void MhalJpdSetGlobalSetting02(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING02, u16Value);
    JPD_LOGD("HAL_NJPD1_Get_GlobalSetting02 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING02));
}

MS_U16 MhalJpdGetGlobalSetting02(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING02);
}

void MhalJpdSetMRCBuf0StartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf0_StartLow : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_L));
}

void MhalJpdSetMRCBuf0StartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf0_StartHigh : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_START_ADDR_H));
}

void MhalJpdSetMRCBuf0EndLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf0_EndLow : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_L));
}

void MhalJpdSetMRCBuf0EndHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf0_EndHigh : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER0_END_ADDR_H));
}

void MhalJpdSetMRCBuf1StartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf1_StartLow : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_L));
}

void MhalJpdSetMRCBuf1StartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf1_StartHigh : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_START_ADDR_H));
}

void MhalJpdSetMRCBuf1EndLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf1_EndLow : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_L));
}

void MhalJpdSetMRCBuf1EndHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCBuf1_EndHigh : 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_BUFFER1_END_ADDR_H));
}

void MhalJpdSetMRCStartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCStart_Low : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_START_ADDR_L));
}

void MhalJpdSetMRCStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_READ_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMRCStart_High : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_START_ADDR_H));
}

void MhalJpdSetMWCBufStartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMWCBuf_StartLow : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_L));
}

void MhalJpdSetMWCBufStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    {
        MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_H, u16Value);
        JPD_LOGD("HAL_NJPD1_GetMWCBuf_StartHigh : 0x%04x\n",
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_H));
    }
}

void MhalJpdSetMRCBuf64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_READBUFADDR_OFFSET_64B, u16Value);
}

void MhalJpdSetMWCBuf64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_WRITEBUFADDR_OFFSET_64B, u16Value);
}

void MhalJpdSetTable64BHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_TABLE_ADDR_OFFSET_64B, u16Value);
}

MS_U16 MhalJpdGetMWCBufStartLow(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_L);
}

MS_U16 MhalJpdGetMWCBufStartHigh(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_START_ADDR_H);
}

MS_U16 MhalJpdGetMWCBufWritePtrLow(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_POINTER_ADDR_L);
}

MS_U16 MhalJpdGetMWCBufWritePtrHigh(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_WRITE_POINTER_ADDR_H);
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetPicHorizontal(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IMG_HSIZE, u16Value);
    JPD_LOGD("HAL_NJPD1_GetPic_H : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IMG_HSIZE));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalSetPicVertical(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IMG_VSIZE, u16Value);
    JPD_LOGD("HAL_NJPD1_GetPic_V : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IMG_VSIZE));
}

void MhalJpdClearEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    // Write 1 clear
    // JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IRQ_CLEAR, u16Value);
}
void MhalJpdForceEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IRQ_FORCE, u16Value);
}
void MhalJpdMaskEventFlag(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IRQ_MASK, u16Value);
}
MS_U16 MhalJpdGetEventFlag(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IRQ_FINAL_S);
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetROIHorizontal(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_ROI_H_START, u16Value);
    JPD_LOGD("HAL_NJPD1_GetROI_H : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_ROI_H_START));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhaljpdSetROIVertical(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_ROI_V_START, u16Value);
    JPD_LOGD("HAL_NJPD1_GetROI_V : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_ROI_V_START));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetROIWidth(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_ROI_H_SIZE, u16Value);
    JPD_LOGD("HAL_NJPD1_GetROIWidth : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_ROI_H_SIZE));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetROIHeight(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_ROI_V_SIZE, u16Value);
    JPD_LOGD("HAL_NJPD1_GetROIHeight : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_ROI_V_SIZE));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetClockGate(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_GATED_CLOCK_CTRL, u16Value);
    JPD_LOGD("HAL_NJPD1_GetClockGate : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GATED_CLOCK_CTRL));
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetSramClockGate(MS_U8 eNJPDNum, MS_U32 u32Value)
{
    MS_U16 u16RegisterData = 0;
    JPD_LOGD("0x%08x\n", u32Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SRAM_GATED_CLK_CTRL_00, (MS_U16)(u32Value & 0xffffUL));
    JPD_LOGD("HAL_NJPD1_GetSramClockGateLow : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SRAM_GATED_CLK_CTRL_00));

    u16RegisterData = 0xfff0UL & MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SRAM_GATED_CLK_CTRL_01);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SRAM_GATED_CLK_CTRL_01, u16RegisterData | (MS_U16)(u32Value >> 16));
    JPD_LOGD("HAL_NJPD1_GetSramClockGateHigh : 0x%04lx\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SRAM_GATED_CLK_CTRL_01) & 0x000fUL);
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdInitRegBase(int u32DevId, MS_U64 JPD_Base)
{
    g_njpdregisterBase[u32DevId] = JPD_Base;

    JPD_LOGD("g_njpdregisterBase[%d] = 0x%lx\n", u32DevId, g_njpdregisterBase[u32DevId]);
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdInitClk(int u32DevId, MS_U64 JPD_ClkBase)
{
    g_jpdClkRegisterBase[u32DevId] = JPD_ClkBase;

    JPD_LOGD("g_jpdClkRegisterBase[%d] = 0x%lx\n", u32DevId, g_jpdClkRegisterBase[u32DevId]);
    WRITE_WORD((g_jpdClkRegisterBase[u32DevId] + (0x4a << 2)), 0x0)
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
void MhalJpdSetRSTIntv(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    {
        MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_RESTART_INTERVAL, u16Value);
        JPD_LOGD("HAL_NJPD1_GetRSTIntv : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_RESTART_INTERVAL));
    }
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return none
/******************************************************************************/
void MhalJpdSetDbgLevel(MS_U8 u8DbgLevel)
{
    //_u8NJPDHalDbgLevel = u8DbgLevel;
}

NJPD_VERIFICATION_MODE MhalJpdGetVerificationMode(void)
{
    return 0;
}

/******************************************************************************/
/// Reset NJPD
///@param value \b IN
///@param value \b OUT
///@return none
/******************************************************************************/
void MhalJpdRst(MS_U8 eNJPDNum)
{
    MhalJpdSetMiuProtectMask(eNJPDNum, TRUE);

    // reset event flag
    MhalJpdClearEventFlag(eNJPDNum, NJPD_EVENT_ALL);
    // reset : low active

#if ENABLE_TEST_NJPD_17_Obuf_Output_Format
    MhalJpdSetOutputFormat(TRUE, E_NJPD_OUTPUT_YC_SWAP);
//    MhalJpdSetOutputFormat(TRUE, E_NJPD_OUTPUT_UV_SWAP);
//    MhalJpdSetOutputFormat(TRUE, E_NJPD_OUTPUT_UV_7BIT);
//    MhalJpdSetOutputFormat(TRUE, E_NJPD_OUTPUT_UV_MSB);
#else
    if (MhalJpdGetVerificationMode() == E_NJPD17_OBUF_OUTPUT_FORMAT_YC_SWAP)
    {
        MhalJpdSetOutputFormat(eNJPDNum, TRUE, E_NJPD_OUTPUT_YC_SWAP);
    }
    else if (MhalJpdGetVerificationMode() == E_NJPD17_OBUF_OUTPUT_FORMAT_UV_SWAP)
    {
        MhalJpdSetOutputFormat(eNJPDNum, TRUE, E_NJPD_OUTPUT_UV_SWAP);
    }
    else if (MhalJpdGetVerificationMode() == E_NJPD17_OBUF_OUTPUT_FORMAT_UV_7BIT)
    {
        MhalJpdSetOutputFormat(eNJPDNum, TRUE, E_NJPD_OUTPUT_UV_7BIT);
    }
    else if (MhalJpdGetVerificationMode() == E_NJPD17_OBUF_OUTPUT_FORMAT_UV_MSB)
    {
        MhalJpdSetOutputFormat(eNJPDNum, TRUE, E_NJPD_OUTPUT_UV_MSB);
    }
    else
    {
        MhalJpdSetOutputFormat(eNJPDNum, TRUE, E_NJPD_OUTPUT_ORIGINAL);
    }
#endif

#if ENABLE_NJPD_DEBUG_MSG
    MhalJpdSetGlobalSetting02(eNJPDNum, 0xFC | NJPD_TBC_MODE | NJPD_LITTLE_ENDIAN | NJPD_REMOVE_0xFF00
                                            | NJPD_REMOVE_0xFFFF | NJPD_HUFF_DATA_LOSS_ERROR);
#else
    if (MhalJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        MhalJpdSetGlobalSetting02(eNJPDNum, 0xFC | NJPD_TBC_MODE | NJPD_LITTLE_ENDIAN | NJPD_REMOVE_0xFF00
                                                | NJPD_REMOVE_0xFFFF | NJPD_HUFF_DATA_LOSS_ERROR);
    }
    else
    {
        MhalJpdSetGlobalSetting02(eNJPDNum, NJPD_TBC_MODE | NJPD_LITTLE_ENDIAN | NJPD_REMOVE_0xFF00 | NJPD_REMOVE_0xFFFF
                                                | NJPD_HUFF_DATA_LOSS_ERROR);
    }
#endif
    MhalJpdSetGlobalSetting00(eNJPDNum, HalJpdGetGlobalSetting00(eNJPDNum) | NJPD_SWRST);
#if 0
MhalJpdMaskEventFlag(0xffff);

#else

    MhalJpdMaskEventFlag(eNJPDNum, ~NJPD_EVENT_ALL); // set 0 to turn on the irq
#endif

    JPD_LOGD("Get BK_NJPD_GLOBAL_SETTING00 : 0x%04x\n", HalJpdGetGlobalSetting00(eNJPDNum));
    JPD_LOGD("Get BK_NJPD_GLOBAL_SETTING01 : 0x%04x\n", HalJpdGetGlobalSetting01(eNJPDNum));
    JPD_LOGD("Get BK_NJPD_GLOBAL_SETTING02 : 0x%04x\n", MhalJpdGetGlobalSetting02(eNJPDNum));

    // Edison@20120620 by Arong
    // 1. Add the fucntion of auto last buffer when FFD9 is detected (reg_spare00[0])
    // 2. Fix last buffer error (reg_spare00[1])
    //// 3. write burst_error (reg_spare00[3])                // Edison/Einstein doesnt have this bit
    //// 4. Fix read-burst error (reg_spare00[4])             // Edison/Einstein doesnt have this bit
    // 5. Coding error (reg_spare00[5])
    // 6. read-burst error (reg_spare00[6])

    // Kano@20150326 by Ethan Chang
    // 7. let IP to MIU's last signel would trigger down when ready_ack(reg_spare00[7]=1    // When Kano BWP function
    // enable would affect the result

    // Nike&Einstein@20130412 by Tony Tseng
    // 9. ECO new error concealment(reg_spare00[9])      // Nike have this bit

    MhalJpdSetSpare00(eNJPDNum, 0x2FB);

    MhalJpdSetMiuProtectMask(eNJPDNum, FALSE);
#if ENABLE_TEST_NJPD_18_Ibuf_Burst_Length_Test
    MhalJpdSetIBufReadLength(eNJPDNum, 0x15, 0x1a);
#else
    if (MhalJpdGetVerificationMode() == E_NJPD18_IBUF_BURST_LENGTH)
    {
        MhalJpdSetIBufReadLength(eNJPDNum, 0x15, 0x1a);
    }
    else
    {
        MhalJpdSetIBufReadLength(eNJPDNum, 0x10, 0x1f);
    }
#endif

#if ENABLE_NJPD_DEBUG_MSG
    // MhalJpdSetCRCReadMode();

    MhalJpdSetCRCWriteMode(eNJPDNum);
#else
    if (MhalJpdGetVerificationMode() == E_NJPD01_TABLE_READ_WRITE)
    {
        MhalJpdSetCRCWriteMode(eNJPDNum);
    }
    else
    {
        MhalJpdSetCRCReadMode(eNJPDNum);
    }
#endif
    JPD_LOGD("end!!\n");
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
MS_U32 MhalJpdGetCurMRCAddr(MS_U8 eNJPDNum)
{
    MS_U32 curMRCAddr;

    curMRCAddr = ((MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_POINTER_ADDR_H) << 16)
                  | MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_POINTER_ADDR_L));

    return curMRCAddr;
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
MS_U16 MhalJpdGetCurRow(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_ROW_IDEX);
}

/******************************************************************************/
///
///@param value \b IN
///@param value \b OUT
///@return status
/******************************************************************************/
MS_U16 MhalJpdGetCurCol(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_COLUMN_IDEX);
}

void MhalJpdSetAutoProtect(MS_U8 eNJPDNum, MS_BOOL enable)
{
    MS_U16 u16RegValue;
    u16RegValue = MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H);
    u16RegValue &= 0x1FFF;
    u16RegValue |= enable << 13;
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H, u16RegValue);
    JPD_LOGD("0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H));
}

void MhalJpdSetMARB06(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04X\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_06, u16Value);
    JPD_LOGD("HAL_NJPD1_Get_MARB06 : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_06));
}

MS_U16 MhalJpdGetMARB06(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_06);
}

void MhalJpdSetMARB07(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04X\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_07, u16Value);
    JPD_LOGD("HAL_NJPD1_Get_MARB07 : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_07));
}

MS_U16 MhalJpdGetMARB07(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_07);
}

void MhalJpdSetWriteProtectEnableUnbound0Low(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04X\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_UBOUND_0_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetWPENUBound_0_L : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_UBOUND_0_L));
}
void MhalJpdSetWriteProtectEnableUnbound0High(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04X\n", u16Value);

    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_UBOUND_0_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetWPENUBound_0_H : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_UBOUND_0_H));
}
void MhalJpdSetWriteProtectEnableBound0Low(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04X\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetWPENLBound_0_L : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_L));
}
void MhalJpdSetWriteProtectEnableBound0HIgh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    MS_U16 u16RegValue;
    u16RegValue = MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H);
    u16RegValue &= 0xE000;
    u16Value &= 0x1FFF;
    u16RegValue |= u16Value;
    JPD_LOGD("0x%04X\n", u16RegValue);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H, u16RegValue);
    JPD_LOGD("HAL_NJPD1_GetWPENLBound_0_H : 0x%04X\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_LBOUND_0_H));
}

void MhalJpdSetSpare00(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    //  [0]: hw auto detect ffd9 => cannot be used in svld mode, unless the "last buffer" cannot work, do not enable
    //  this [1]: input buffer bug => always turn on [2]: marb bug => always turn on (not used in 2011/12/28 ECO
    //  version) [3]: bypass marb => 2011/12/28 ECO version

    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE00, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare00 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE00));
}
MS_U16 MhalJpdGetSpare00(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE00);
}

void MhalJpdSetSpare01(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE01, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare01 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE01));
}
MS_U16 MhalJpdGetSpare01(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE01);
}

void MhalJpdSetSpare02(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE02, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare02 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE02));
}
MS_U16 MhalJpdGetSpare02(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE02);
}

void MhalJpdSetSpare03(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE03, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare03 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE03));
}
MS_U16 MhalJpdGetSpare03(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE03);
}

void MhalJpdSetSpare04(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE04, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare04 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE04));
}
MS_U16 MhalJpdGetSpare04(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE04);
}

void MhalJpdSetSpare05(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE05, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare05 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE05));
}
MS_U16 MhalJpdGetSpare05(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE05);
}

void MhalJpdSetSpare06(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE06, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare06 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE06));
}
MS_U16 MhalJpdGetSpare06(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE06);
}

void MhalJpdSetSpare07(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_SPARE07, u16Value);
    JPD_LOGD("HAL_NJPD1_GetSpare07 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE07));
}
MS_U16 MhalJpdGetSpare07(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_SPARE07);
}

void MhalJpdSetWriteOneClearReg(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_WRITE_ONE_CLEAR, u16Value);
}

void MhalJpdGetWriteOneClearReg2(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    // for convience, not to print any message in this function
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_WRITE_ONE_CLEAR, u16Value);
}

MS_U16 MhalJpdGetWriteOneClearReg(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_WRITE_ONE_CLEAR);
}

void MhalJpdSetHTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetHTableStart_Low : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_START_ADDR_L));
}

void MhalJpdSetHTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetHTableStart_High : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_START_ADDR_H));
}

void MhalJpdSetQTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_QTABLE_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetQTableStart_Low : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_QTABLE_START_ADDR_L));
}

void MhalJpdSetQTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_QTABLE_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetQTableStart_High : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_QTABLE_START_ADDR_H));
}

void MhalJpdSetGTableStartLow(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_GTABLE_START_ADDR_L, u16Value);
    JPD_LOGD("HAL_NJPD1_GetGTableStart_Low : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_GTABLE_START_ADDR_L));
}

void MhalJpdSetGTableStartHigh(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_GTABLE_START_ADDR_H, u16Value);
    JPD_LOGD("HAL_NJPD1_GetGTableStart_High : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_GTABLE_START_ADDR_H));
}

void MhalJpdSetHTableSize(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    u16Value &= 0x00FF;
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_SIZE, u16Value);
    JPD_LOGD("HAL_NJPD1_GetHTableSize : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_SIZE));
}

MS_U16 MhalJpdGetHTableSize(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_HTABLE_SIZE);
}
void MhalJpdSetRIUInterface(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_TBC, u16Value);
}

MS_U16 MhalJpdGetRIUInterface(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_TBC);
}

MS_U16 MhalJpdTBCReadDataLow(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_TBC_RDATA_L);
}

MS_U16 MhalJpdTBCReadDataHigh(MS_U8 eNJPDNum)
{
    return MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_TBC_RDATA_H);
}

void MhalJpdSetIBufReadLength(MS_U8 eNJPDNum, MS_U8 u8Min, MS_U8 u8Max)
{
    MS_U16 u16Value;
    u16Value = (u8Min << 5) + u8Max;
    JPD_LOGD("0x%04x\n", u16Value);
    u16Value = (u16Value | (MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IBUF_READ_LENGTH) & 0xfc00));
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_IBUF_READ_LENGTH, u16Value);
    JPD_LOGD("HAL_NJPD1_GetIBufReadLength : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_IBUF_READ_LENGTH));
}

void MhalJpdSetMRBurstThd(MS_U8 eNJPDNum, MS_U16 u16Value)
{
    JPD_LOGD("0x%04x\n", u16Value);
    u16Value = (u16Value | (MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_04) & 0xffe0));
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_04, u16Value);
    JPD_LOGD("HAL_NJPD1_GetMARB_SETTING_04 : 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_SETTING_04));
}

void MhalJpdSetCRCReadMode(MS_U8 eNJPDNum)
{
    JPD_LOGD("0x%04x\n", 0x200);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_CRC_MODE, 0x200);
}

void MhalJpdSetCRCWriteMode(MS_U8 eNJPDNum)
{
    JPD_LOGD("0x%04x\n", 0x300);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_CRC_MODE, 0x300);
}

void MhalJpdResetMarb(MS_U8 eNJPDNum)
{
    JPD_LOGD("\n");
    JPD_LOGD("set BK_NJPD1_MARB_RESET(0x%x) to 0x%02x\n", BK_NJPD1_MARB_RESET,
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) & 0xFE);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_RESET, MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) & 0xFE);
    JPD_LOGD("set BK_NJPD1_MARB_RESET(0x%x) to 0x%02x\n", BK_NJPD1_MARB_RESET,
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) | 0x01);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_RESET, MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) | 0x01);
    JPD_LOGD("set BK_NJPD1_MARB_RESET(0x%x) to 0x%02x\n", BK_NJPD1_MARB_RESET,
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) & 0xFD);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_RESET, MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) & 0xFD);
    JPD_LOGD("set BK_NJPD1_MARB_RESET(0x%x) to 0x%02x\n", BK_NJPD1_MARB_RESET,
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) | 0x02);
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_MARB_RESET, MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_RESET) | 0x02);
    //        __HAL_NJPD_WriteByte(eNJPDNum,(BK_NJPD1_MARB_RESET, __HAL_NJPD_ReadByte(eNJPDNum,BK_NJPD1_MARB_RESET) |
    //        0x12); JPD_LOGD("!!set BK_NJPD1_MARB_RESET to 0x%02x\n",
    //        __HAL_NJPD_ReadByte(eNJPDNum,BK_NJPD1_MARB_RESET));
}

void MhalJpdSetOutputFormat(MS_U8 eNJPDNum, MS_BOOL bRst, EN_NJPD_OUTPUT_FORMAT eOutputFormat)
{
    switch (eOutputFormat)
    {
        case E_NJPD_OUTPUT_ORIGINAL:
            if (bRst)
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, 0);
                MhalJpdResetMarb(eNJPDNum);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & NJPD_GTABLE_RST);
            }
            else
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, HalJpdGetGlobalSetting00(eNJPDNum) & 0xf3ff);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & 0xfcff);
            }
            break;
        case E_NJPD_OUTPUT_YC_SWAP:
            if (bRst)
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, NJPD_YC_SWAP);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & NJPD_GTABLE_RST);
            }
            else
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, (HalJpdGetGlobalSetting00(eNJPDNum) & 0xf3ff) | NJPD_YC_SWAP);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & 0xfcff);
            }
            break;
        case E_NJPD_OUTPUT_UV_SWAP:
            if (bRst)
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, NJPD_UV_SWAP);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & NJPD_GTABLE_RST);
            }
            else
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, (HalJpdGetGlobalSetting00(eNJPDNum) & 0xf3ff) | NJPD_UV_SWAP);
                MhalJpdSetGlobalSetting01(eNJPDNum, HalJpdGetGlobalSetting01(eNJPDNum) & 0xfcff);
            }
            break;
        case E_NJPD_OUTPUT_UV_7BIT:
            if (bRst)
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, 0);
                //                MhalJpdSetGlobalSetting01(NJPD_UV_7BIT);
                MhalJpdSetGlobalSetting01(eNJPDNum,
                                          (HalJpdGetGlobalSetting01(eNJPDNum) & NJPD_GTABLE_RST) | NJPD_UV_7BIT);
            }
            else
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, HalJpdGetGlobalSetting00(eNJPDNum) & 0xf3ff);
                MhalJpdSetGlobalSetting01(eNJPDNum, (HalJpdGetGlobalSetting01(eNJPDNum) & 0xfcff) | NJPD_UV_7BIT);
            }

            break;
        case E_NJPD_OUTPUT_UV_MSB:
            if (bRst)
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, 0);
                //                MhalJpdSetGlobalSetting01(NJPD_UV_7BIT | NJPD_UV_MSB);
                MhalJpdSetGlobalSetting01(eNJPDNum, (HalJpdGetGlobalSetting01(eNJPDNum) & NJPD_GTABLE_RST)
                                                        | (NJPD_UV_7BIT | NJPD_UV_MSB));
            }
            else
            {
                MhalJpdSetGlobalSetting00(eNJPDNum, HalJpdGetGlobalSetting00(eNJPDNum) & 0xf3ff);
                MhalJpdSetGlobalSetting01(eNJPDNum,
                                          (HalJpdGetGlobalSetting01(eNJPDNum) & 0xfcff) | NJPD_UV_7BIT | NJPD_UV_MSB);
            }
            break;
    }
}

void MhalJpdDebug(MS_U8 eNJPDNum)
{
    MS_U8  u8i;
    MS_U16 u16i;
    JPD_LOGD("read BK_NJPD1_MARB_CRC_RESULT_0: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_CRC_RESULT_0));
    JPD_LOGD("read BK_NJPD1_MARB_CRC_RESULT_1: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_CRC_RESULT_1));
    JPD_LOGD("read BK_NJPD1_MARB_CRC_RESULT_2: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_CRC_RESULT_2));
    JPD_LOGD("read BK_NJPD1_MARB_CRC_RESULT_3: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MARB_CRC_RESULT_3));
    JPD_LOGD("read BK_NJPD1_GLOBAL_SETTING00: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING00));
    JPD_LOGD("read BK_NJPD1_GLOBAL_SETTING01: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING01));
    JPD_LOGD("read BK_NJPD1_GLOBAL_SETTING02: 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GLOBAL_SETTING02));
    JPD_LOGD("read BK_NJPD1_MIU_READ_STATUS = 0x%04x\n", MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_STATUS));
    JPD_LOGD("read BK_NJPD1_MIU_READ_POINTER_ADDR_L: 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_POINTER_ADDR_L));
    JPD_LOGD("read BK_NJPD1_MIU_READ_POINTER_ADDR_H: 0x%04x\n",
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_MIU_READ_POINTER_ADDR_H));

    for (u16i = 0; u16i <= 0x7b; u16i++)
    {
        MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_SELECT, u16i);
        JPD_LOGD("BK_NJPD1_DEBUG_BUS[H:L] = [0x%02x][0x%04x:0x%04x]\n", u16i,
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_H), MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_L));
    }
    MhalJpdWrite2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_SELECT, 0xFF);
    JPD_LOGD("BK_NJPD1_DEBUG_BUS[H:L] = [0x%02x][0x%04x:0x%04x]\n", 0xFF,
             MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_H), MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_DEBUG_BUS_L));

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("NJPD  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u16i = 0; u16i < 0x80; u16i += 8)
    {
        JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n", u16i,
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 1)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 2)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 3)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 4)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 5)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 6)),
                 MhalJpdRead2Byte(eNJPDNum, BK_NJPD1_GENERAL(u16i + 7)));
    }
    JPD_LOGD("=======================================================\n");

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("MIU0  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u8i = 0; u8i < 0x80; u8i += 8)
    {
        /*JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n", u8i, MhalJpdRead2Byte(eNJPDNum,
           BK_MIU0_GENERAL(u8i)), MhalJpdRead2Byte(eNJPDNum, BK_MIU0_GENERAL(u8i + 1)), MhalJpdRead2Byte(eNJPDNum,
           BK_MIU0_GENERAL(u8i + 2)), MhalJpdRead2Byte(eNJPDNum, BK_MIU0_GENERAL(u8i + 3)), MhalJpdRead2Byte(eNJPDNum,
           BK_MIU0_GENERAL(u8i + 4)), MhalJpdRead2Byte(eNJPDNum, BK_MIU0_GENERAL(u8i + 5)), MhalJpdRead2Byte(eNJPDNum,
           BK_MIU0_GENERAL(u8i + 6)), MhalJpdRead2Byte(eNJPDNum, BK_MIU0_GENERAL(u8i + 7)));*/
    }
    JPD_LOGD("=======================================================\n");

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("MIU1  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u8i = 0; u8i < 0x80; u8i += 8)
    {
        /*
            JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n",u8i,
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+1)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+2)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+3)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+4)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+5)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+6)),
            MhalJpdRead2Byte(eNJPDNum,BK_MIU1_GENERAL(u8i+7))
            );*/
        ;
    }
    JPD_LOGD("=======================================================\n");

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("1608  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u8i = 0; u8i < 0x80; u8i += 8)
    {
        /*
            JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n",u8i,
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+1)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+2)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+3)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+4)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+5)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+6)),
            MhalJpdRead2Byte(eNJPDNum,BK_1608_GENERAL(u8i+7))
            );
            */
        ;
    }
    JPD_LOGD("=======================================================\n");

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("160F  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u8i = 0; u8i < 0x80; u8i += 8)
    {
        /*
            JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n",u8i,
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+1)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+2)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+3)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+4)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+5)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+6)),
            MhalJpdRead2Byte(eNJPDNum,BK_160F_GENERAL(u8i+7))
            );
            */
        ;
    }
    JPD_LOGD("=======================================================\n");

    JPD_LOGD("=======================================================\n");
    JPD_LOGD("1615  | 00/08 01/09 02/0A 03/0B 04/0C 05/0D 06/0E 07/0F\n");
    JPD_LOGD("=======================================================\n");
    for (u8i = 0; u8i < 0x80; u8i += 8)
    {
        /*
            JPD_LOGD("%02x    | %04x  %04x  %04x  %04x  %04x  %04x  %04x  %04x \n",u8i,
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+1)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+2)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+3)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+4)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+5)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+6)),
            MhalJpdRead2Byte(eNJPDNum,BK_1615_GENERAL(u8i+7))
            );
            */
        ;
    }
    JPD_LOGD("=======================================================\n");
}

void MhalJpdPZ1SetFlag(MS_U16 u16Value)
{
    // MhalJpdWrite2Byte_Oth(BK_NJPD1_SETLFAG, u16Value);
}
