/*
* hal_disp_irq.c- Sigmastar
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

#define _HAL_DISP_IRQ_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_irq.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_IRQ_VSYNC_BIT (0x0001)     // BIT0
#define HAL_DISP_IRQ_VSYNC_MSK (0x0001)

#define HAL_DISP_IRQ_VDE_BIT   (0x0000)
#define HAL_DISP_IRQ_VDE_MSK   (0x0000)     //Not Support


#define HAL_DISP_IRQ_MASK_REG(id)  ((id == 0) ? REG_DISP_MOPG_BK00_0C_L : REG_DISP_MOPG_BK10_0C_L)
#define HAL_DISP_IRQ_FLAG_REG(id)  ((id == 0) ? REG_DISP_MOPG_BK00_02_L : REG_DISP_MOPG_BK10_02_L)
#define HAL_DISP_IRQ_CLEAR_REG(id) ((id == 0) ? REG_DISP_MOPG_BK00_0C_L : REG_DISP_MOPG_BK10_0C_L)


#define HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT      (0x0008) // BIT3
#define HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK      (0x0008)

#define HAL_DISP_IRQ_DMA_ACTIVE_OFF_BIT     (0x0004) // BIT2
#define HAL_DISP_IRQ_DMA_ACTIVE_OFF_MSK     (0x0004)


#define HAL_DISP_IRQ_DMA_MASK_REG(id)       ((id == 0) ? REG_DISP_TOP_0_03_L : REG_DISP_TOP_1_03_L)
#define HAL_DISP_IRQ_DMA_FLAG_REG(id)       ((id == 0) ? REG_DISP_TOP_0_02_L : REG_DISP_TOP_1_02_L)
#define HAL_DISP_IRQ_DMA_CLEAR_REG(id)      ((id == 0) ? REG_DISP_TOP_0_02_L : REG_DISP_TOP_1_02_L)
//-------------------------------------------------------------------------------------------------
// Internal Isr for TimeZone
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_IRQ_INTERNAL_MASK_REG(id)  ((id == 0) ? REG_DISP_TOP_0_03_L : REG_DISP_TOP_1_03_L)
#define HAL_DISP_IRQ_INTERNAL_FLAG_REG(id)  ((id == 0) ? REG_DISP_TOP_0_02_L : REG_DISP_TOP_1_02_L)
#define HAL_DISP_IRQ_INTERNAL_CLEAR_REG(id) ((id == 0) ? REG_DISP_TOP_0_02_L : REG_DISP_TOP_1_02_L)


#define HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_BIT    (0x0080) // BIT7
#define HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_MSK    (0x0080)

#define HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_BIT    (0x0040) // BIT6
#define HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_MSK    (0x0040)

#define HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_BIT      (0x0020) // BIT5
#define HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_MSK      (0x0020)

#define HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_BIT      (0x0010) // BIT4
#define HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_MSK      (0x0010)

//-------------------------------------------------------------------------------------------------
// VGA HPD Isr
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_IRQ_VGA_HPD_MASK_REG   (REG_DISP_TOP_0_3B_L)
#define HAL_DISP_IRQ_VGA_HPD_FLAG_REG   (REG_DISP_TOP_0_3E_L)
#define HAL_DISP_IRQ_VGA_HPD_CLEAR_REG  (REG_DISP_TOP_0_3D_L)


#define HAL_DISP_IRQ_VGA_HPD_ON_BIT     (0x0004) // BIT2
#define HAL_DISP_IRQ_VGA_HPD_ON_MSK     (0x0004)

#define HAL_DISP_IRQ_VGA_HPD_OFF_BIT    (0x0008) // BIT3
#define HAL_DISP_IRQ_VGA_HPD_OFF_MSK    (0x0008)


#define TIMEZONE_MUTEX_LOCK(x, y)     //spin_lock_irqsave(x, y)
#define TIMEZONE_MUTEX_UNLOCK(x, y)   //spin_unlock_irqrestore(x, y)

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

HalDispIrqTimeZoneStatus_t gstTimeZoneStatus[HAL_DISP_DEVICE_MAX] =
{
    {0, {0, 0, 0, 0}},
    {0, {0, 0, 0, 0}},
};

u32 gu32VgaHpdIsrStatus = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
__inline void _HalDispIrqEnableIsr(HalDispIrqType_e enType, bool *pbEn, u32 u32Id)
{
    u16 u16Val, u16Msk;
    bool bEnable = *pbEn;

    u16Val = 0;
    u16Msk = 0;

    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, Type:%x, En:%d\n", __FUNCTION__, __LINE__, enType , bEnable);
    if(enType & E_HAL_DISP_IRQ_TYPE_VSYNC)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_VSYNC_BIT;
        u16Msk |= HAL_DISP_IRQ_VSYNC_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VDE)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_VDE_BIT;
        u16Msk |= HAL_DISP_IRQ_VDE_MSK;
    }

    W2BYTEMSK(HAL_DISP_IRQ_MASK_REG(u32Id), u16Val, u16Msk);
}


__inline void _HalDispIrqGetFlag(HalDispIrqType_e enType, u32 *pu32Flag, u32 u32Id)
{
    u16 u16Val, u16IrqMsk;

    *pu32Flag = 0;

    u16Val = R2BYTE(HAL_DISP_IRQ_FLAG_REG(u32Id));
    u16IrqMsk = R2BYTE(HAL_DISP_IRQ_MASK_REG(u32Id));

    if(enType & E_HAL_DISP_IRQ_TYPE_VSYNC)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VSYNC_MSK) ? E_HAL_DISP_IRQ_TYPE_VSYNC : 0;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VDE)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VDE_MSK) ? E_HAL_DISP_IRQ_TYPE_VDE : 0;
    }


    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, enType:%x, Flag:%lx (%04x %04x)\n", __FUNCTION__, __LINE__, enType, *pu32Flag, u16IrqMsk, u16Val);
}


__inline void _HalDispIrqClear(HalDispIrqType_e enType, u32 u32Id)
{
    u16 u16Val, u16Msk;

    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;
    if(enType & E_HAL_DISP_IRQ_TYPE_VSYNC)
    {
        u16Val |= HAL_DISP_IRQ_VSYNC_BIT;
        u16Msk |= HAL_DISP_IRQ_VSYNC_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VDE)
    {
        u16Val |= HAL_DISP_IRQ_VDE_BIT;
        u16Msk |= HAL_DISP_IRQ_VDE_MSK;
    }

    // Clear, 1->0
    W2BYTEMSK(HAL_DISP_IRQ_CLEAR_REG(u32Id), u16Val, u16Msk);
    W2BYTEMSK(HAL_DISP_IRQ_CLEAR_REG(u32Id), 0, u16Msk);
}

__inline void _HalDispIrqGetId(u8 *pu8Id, u32 u32Id)
{
    *pu8Id = u32Id ? HAL_DISP_IRQ_ID_DEVICE_1 : HAL_DISP_IRQ_ID_DEVICE_0;
}

__inline void  _HalDispIrqGetDmaIsrIdx(u8 *pu8Id, u32 u32Id)
{
    *pu8Id = u32Id ? HAL_DISP_IRQ_ID_TIMEZONE_1 : HAL_DISP_IRQ_ID_TIMEZONE_0;
}

__inline void _HalDispIrqEnableDmaIsr(HalDispIrqType_e enType, bool *pbEn, u32 u32Id)
{
    u16 u16Val, u16Msk;
    bool bEnable = *pbEn;

    u16Val = 0;
    u16Msk = 0;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, Type:%x, En:%d\n", __FUNCTION__, __LINE__, enType , bEnable);
    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_DMA_ACTIVE_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_ACTIVE_OFF_MSK;
    }

    W2BYTEMSK(HAL_DISP_IRQ_DMA_MASK_REG(u32Id), u16Val, u16Msk);
}

__inline void _HalDispIrqGetDmaFlag(HalDispIrqType_e enType, u32 *pu32Flag, u32 u32Id)
{
    u16 u16Val, u16IrqMsk;

    *pu32Flag = 0;

    u16Val = R2BYTE(HAL_DISP_IRQ_DMA_FLAG_REG(u32Id));
    u16IrqMsk = R2BYTE(HAL_DISP_IRQ_DMA_MASK_REG(u32Id));

    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, Id:%ld, enType:%x, Msk:%04x, Val:%04x \n",
        __FUNCTION__, __LINE__,
        u32Id, enType,
        u16IrqMsk, u16Val);

    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON)
    {
        if(u16Val & HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON;
        }
   }

    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF)
    {
        if(u16Val & HAL_DISP_IRQ_DMA_ACTIVE_OFF_MSK)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF;
        }
    }
}


__inline void _HalDispIrqClearDma(HalDispIrqType_e enType, u32 u32Id)
{
    u16 u16Val, u16Msk;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;
    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON)
    {
        u16Val |= HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF)
    {
        u16Val |= HAL_DISP_IRQ_DMA_ACTIVE_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_ACTIVE_OFF_MSK;
    }

    // Clear, 1 Clear
    W2BYTE(HAL_DISP_IRQ_DMA_CLEAR_REG(u32Id), u16Val);
}

__inline void _HalDispIrqSupportedTimeZoneIsr(bool *pbSupported)
{
    *pbSupported = DISP_TIMEZONE_ISR_SUPPORT;
}

__inline void  _HalDispIrqGetTimeZoneIsrIdx(u8 *pu8Id, u32 u32Id)
{
    *pu8Id = u32Id ?
            HAL_DISP_IRQ_ID_TIMEZONE_1 :
            HAL_DISP_IRQ_ID_TIMEZONE_0;
}

__inline void _HalDispIrqEnableTimeZoneIsr(HalDispIrqType_e enType, bool *pbEn, u32 u32Id)
{
    u16 u16Val, u16Msk;
    bool bEnable = *pbEn;
    u16Val = 0;
    u16Msk = 0;

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_MSK;
    }

    W2BYTEMSK(HAL_DISP_IRQ_INTERNAL_MASK_REG(u32Id), u16Val, u16Msk);
}

__inline void _HalDispIrqGetTimeZoneFlag(HalDispIrqType_e enType, u32 *pu32Flag, HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    u16 u16Val, u16MskVal;
    u64 u64CurTime;

    u64CurTime = DrvDispOsGetSystemTimeStamp();
    u16Val = R2BYTE(HAL_DISP_IRQ_INTERNAL_FLAG_REG(pstCmdqCtx->s32CmdqId));
    u16MskVal = R2BYTE(HAL_DISP_IRQ_INTERNAL_MASK_REG(pstCmdqCtx->s32CmdqId));

    *pu32Flag = 0;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Id:%ld, enType:%x, Msk:%04x, Val:%04x\n",
        __FUNCTION__, __LINE__,
        pstCmdqCtx->s32CmdqId, enType, u16MskVal,
        u16Val);

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE)
    {
        if(u16Val & HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_BIT)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE;
            gstTimeZoneStatus[pstCmdqCtx->s32CmdqId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE] = u64CurTime;
        }
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE)
    {
        if(u16Val & HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_BIT)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE;
            gstTimeZoneStatus[pstCmdqCtx->s32CmdqId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_NEGATIVE] = u64CurTime;
        }
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE)
    {
        if(u16Val & HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_BIT)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE;
            gstTimeZoneStatus[pstCmdqCtx->s32CmdqId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE] = u64CurTime;
        }
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        if(u16Val & HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_BIT)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE;
            gstTimeZoneStatus[pstCmdqCtx->s32CmdqId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE] = u64CurTime;
            if(pstCmdqCtx->bEnSyncCnt)
            {
                HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16SyncCnt, 1);
            }
        }
    }

    gstTimeZoneStatus[pstCmdqCtx->s32CmdqId].u32IrqFlags = *pu32Flag;

}

__inline void _HalDispIrqClearTimeZone(HalDispIrqType_e enType, u32 u32Id)
{
    u16 u16Val, u16Msk;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE)
    {
        u16Val |= HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VSYNC_POSITIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE)
    {
        u16Val |= HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VSYNC_NEGATIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE)
    {
        u16Val |= HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VDE_POSITIVE_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        u16Val |= HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_INTERNAL_VDE_NEGATIVE_MSK;
    }

    W2BYTE(HAL_DISP_IRQ_INTERNAL_CLEAR_REG(u32Id), u16Val);
}

void _HalDispIrqGetTimeZoneStatus(HalDispIrqType_e enType, HalDispIrqTimeZoneStatus_t *pstStatus, u32 u32Id)
{
    memcpy(pstStatus, &gstTimeZoneStatus[u32Id], sizeof(HalDispIrqTimeZoneStatus_t));
}

__inline void _HalDispIrqSupportedVgaHpdIsr(bool *pbSupported)
{
    *pbSupported = DISP_VGA_HPD_ISR_SUPPORT;
}

__inline void  _HalDispIrqGetVgaHpdIsrIdx(u8 *pu8Id)
{
    *pu8Id = HAL_DISP_IRQ_ID_VGAHPD;
}


__inline void _HalDispIrqEnableVgaHpdIsr(HalDispIrqType_e enType, bool *pbEn)
{
    u16 u16Val, u16Msk;
    bool bEnable = *pbEn;
    u16Val = 0;
    u16Msk = 0;

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_VGA_HPD_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_ON_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        u16Val |= bEnable ?  0 : HAL_DISP_IRQ_VGA_HPD_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_OFF_MSK;
    }



    W2BYTEMSK(HAL_DISP_IRQ_VGA_HPD_MASK_REG, u16Val, u16Msk);
}

__inline void _HalDispIrqGetVgaHpdFlag(HalDispIrqType_e enType, u32 *pu32Flag)
{
    u16 u16Val;
    //u16 u16IrqMsk;
    *pu32Flag = 0;

    u16Val = R2BYTE(HAL_DISP_IRQ_VGA_HPD_FLAG_REG);
    //u16IrqMsk = R2BYTE(HAL_DISP_IRQ_INTERNAL_MASK_REG);

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_HPD_ON_MSK) ? E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON : 0;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_HPD_OFF_BIT) ? E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF : 0;
    }

    gu32VgaHpdIsrStatus = *pu32Flag;
    //DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x, Flag:%x (%04x %04x)\n", __FUNCTION__, __LINE__, enType, *pu32Flag, u16IrqMsk, u16Val);
}


__inline void _HalDispIrqClearVgaHpd(HalDispIrqType_e enType)
{
    u16 u16Val, u16Msk;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        u16Val |= HAL_DISP_IRQ_VGA_HPD_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_ON_MSK;
    }

    if(enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        u16Val |= HAL_DISP_IRQ_VGA_HPD_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_OFF_MSK;
    }

    W2BYTEMSK(HAL_DISP_IRQ_VGA_HPD_CLEAR_REG, u16Val, u16Msk);

    //DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x \n", __FUNCTION__, __LINE__, enType);
}

__inline void _HalDispIrqGetVgaHpdIsrStatus(u32 *pu32Status)
{
    *pu32Status = gu32VgaHpdIsrStatus;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool HalDispIrqIoCtl(HalDispIrqIoctlConfig_t *pCfg)
{
    bool bRet = 1;
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;

    pstDispCtx = (DrvDispCtxConfig_t *)pCfg->pDispCtx;

    if(pstDispCtx == NULL)
    {
        DISP_ERR("%s %d, Null DispCtx\n", __FUNCTION__, __LINE__);
        return 0;
    }

    if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];

        DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, CtxId:%ld, DevId:%ld, Ioctl:%s, IrqType:%s\n",
            __FUNCTION__, __LINE__,
            pstDispCtx->u32Idx,
            pstDevContain->u32DevId,
            PARSING_HAL_IRQ_IOCTL(pCfg->enIoctlType), PARSING_HAL_IRQ_TYPE(pCfg->enIrqType));
    }
    else
    {
        pstDmaContain = pstDispCtx->pstCtxContain->pstDmaContain[pstDispCtx->u32Idx];

        if(pstDmaContain->pSrcDevContain == NULL)
        {
            DISP_ERR("%s %d, Dma No Bind Src Device\n", __FUNCTION__, __LINE__);
            return 0;
        }
        else
        {
            pstDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
            DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, CtxId:%ld, DmaId:%ld, Ioctl:%s, IrqType:%s\n",
                __FUNCTION__, __LINE__,
                pstDispCtx->u32Idx,
                pstDmaContain->u32DmaId,
                PARSING_HAL_IRQ_IOCTL(pCfg->enIoctlType), PARSING_HAL_IRQ_TYPE(pCfg->enIrqType));
        }
    }

    switch(pCfg->enIoctlType)
    {
        case E_HAL_DISP_IRQ_IOCTL_ENABLE:
            _HalDispIrqEnableIsr(pCfg->enIrqType, (bool *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_GET_FLAG:
            _HalDispIrqGetFlag(pCfg->enIrqType,(u32 * )pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_CLEAR:
            _HalDispIrqClear(pCfg->enIrqType, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_GET_ID:
            _HalDispIrqGetId((u8 * )pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE:
            _HalDispIrqEnableDmaIsr(pCfg->enIrqType, (bool *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG:
            _HalDispIrqGetDmaFlag(pCfg->enIrqType,(u32 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR:
            _HalDispIrqClearDma(pCfg->enIrqType, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID:
            _HalDispIrqGetDmaIsrIdx((u8 * )pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED:
            _HalDispIrqSupportedTimeZoneIsr((bool *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID:
            _HalDispIrqGetTimeZoneIsrIdx((u8 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE:
            _HalDispIrqEnableTimeZoneIsr(pCfg->enIrqType, (bool *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG:
            pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pstDispCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
            _HalDispIrqGetTimeZoneFlag(pCfg->enIrqType, (u32 *)pCfg->pParam, pstCmdqCtx);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS:
            _HalDispIrqGetTimeZoneStatus(pCfg->enIrqType, (HalDispIrqTimeZoneStatus_t *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR:
            _HalDispIrqClearTimeZone(pCfg->enIrqType, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED:
            _HalDispIrqSupportedVgaHpdIsr((bool *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID:
            _HalDispIrqGetVgaHpdIsrIdx((u8 *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE:
            _HalDispIrqEnableVgaHpdIsr(pCfg->enIrqType, (bool *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_FLAG:
            _HalDispIrqGetVgaHpdFlag(pCfg->enIrqType, (u32 *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_CLEAR:
            _HalDispIrqClearVgaHpd(pCfg->enIrqType);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_STATUS:
            _HalDispIrqGetVgaHpdIsrStatus((u32 *)pCfg->pParam);
            break;

        default:
            bRet = FALSE;
            DISP_ERR("%s %d, UnKnown Irq Iocl:%d\n", __FUNCTION__, __LINE__, pCfg->enIoctlType);
            break;
    }
    return bRet;
}

void HalDispIrqSetDacEn(u8 u8Val)
{
    W2BYTEMSK(REG_DISP_DAC_15_L, u8Val ? 0x0007: 0x0000, 0x0007);
}

