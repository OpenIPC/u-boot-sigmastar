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
#include "hal_disp_chip.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_irq.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "hal_disp_op2.h"
#include "drv_disp_ctx.h"
#include "hal_disp_dma.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

#define HAL_DISP_IRQ_MOP_MASK_REG(id)                         \
    ((id == HAL_DISP_DEVICE_ID_0)   ? REG_DISP_MOPG_BK00_0C_L \
     : (id == HAL_DISP_DEVICE_ID_1) ? REG_DISP_MOPG_BK10_0C_L \
                                    : REG_DISP_MOPG_BK20_0C_L)

#define HAL_DISP_IRQ_MOP_FLAG_REG(id)                         \
    ((id == HAL_DISP_DEVICE_ID_0)   ? REG_DISP_MOPG_BK00_02_L \
     : (id == HAL_DISP_DEVICE_ID_1) ? REG_DISP_MOPG_BK10_02_L \
                                    : REG_DISP_MOPG_BK20_02_L)

#define HAL_DISP_IRQ_MOP_CLEAR_REG(id)                        \
    ((id == HAL_DISP_DEVICE_ID_0)   ? REG_DISP_MOPG_BK00_0C_L \
     : (id == HAL_DISP_DEVICE_ID_1) ? REG_DISP_MOPG_BK10_0C_L \
                                    : REG_DISP_MOPG_BK20_0C_L)

#define HAL_DISP_IRQ_DMA_MASK_REG(id)    (DISP_CVBS_REG_INTR_MASK_GRP_A)
#define HAL_DISP_IRQ_DMA_FLAG_REG(id)    (DISP_CVBS_REG_INTR_STATUS_GRP_A)
#define HAL_DISP_IRQ_DMA_RAWFLAG_REG(id) (DISP_CVBS_REG_INTR_RAW_STATUS_GRP_A)
#define HAL_DISP_IRQ_DMA_CLEAR_REG(id)   (DISP_CVBS_REG_INTR_STATUS_GRP_A)
//-------------------------------------------------------------------------------------------------
// Internal Isr for TimeZone
//-------------------------------------------------------------------------------------------------

#define HAL_DISP_FPLL_LOCKED_CHECK_TIME         10
#define HAL_DISP_FPLL_LOCKED_STABLE_CHECK_FRAME 10
#define HAL_DISP_FPLL_LOCKED_UNSTABLE_ADJUST_FRAME 30
#define HAL_DISP_FPLL_LOCKED_UNSTABLE_RST_FRAME 120
#define HAL_DISP_FPLL_CONTINUE_ERR_PHASE_CNT      5
#define HAL_DISP_AFF_CONTINUE_ERR_MUTE_CNT      5
#define HAL_DISP_AFF_CONTINUE_ERR_RST_CNT       60

//-------------------------------------------------------------------------------------------------
// VGA HPD Isr
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_IRQ_VGA_HPD_MASK_REG  (DISP_TOP0_REG_DAC_IRQ_MASK)
#define HAL_DISP_IRQ_VGA_HPD_FLAG_REG  (DISP_TOP0_REG_DAC_IRQ_FINAL_STATUS)
#define HAL_DISP_IRQ_VGA_HPD_CLEAR_REG (DISP_TOP0_REG_DAC_IRQ_CLR)

#define HAL_DISP_IS_IRQ_EN(msk, irq) ((msk & irq) == 0)

#define HAL_DISP_TIMEZONE_MUTEX_LOCK(x, y)   // spin_lock_irqsave(x, y)
#define HAL_DISP_TIMEZONE_MUTEX_UNLOCK(x, y) // spin_unlock_irqrestore(x, y)

#define HalDispGetFpllUnlocked(dev) ((dev < HAL_DISP_FPLL_CNT) && !(g_stDispIrqHist.stWorkingStatus.stFpllStatus[dev].u8FpllLocked))
#define HalDispGetFpllUsed(dev)     ((dev < HAL_DISP_FPLL_CNT) && (g_stDispIrqHist.stWorkingStatus.stFpllStatus[dev].u8FpllEn))
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

typedef struct HalDispIrqDeviceStatus_s
{
    u16 u16LockStatus[HAL_DISP_FPLL_CNT];
    u16 u16PhaseErr[HAL_DISP_FPLL_CNT];
    u16 u16FreqErr[HAL_DISP_FPLL_CNT];
    u16 u16DacAffStatus;
} HalDispIrqDeviceStatus_t;
u8                             g_u8DvContinue       = 0;
u32                            g_u32VgaHpdIsrStatus = 0;
extern HalDispInternalConfig_t g_stInterCfg[HAL_DISP_DEVICE_MAX];
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static __inline void _HalDispIrqEnableIsr(HalDispIrqType_e enType, u8 *pbEn, u32 u32DevId)
{
    u16 u16Val, u16Msk;
    u8  bEnable = *pbEn;

    u16Val = 0;
    u16Msk = 0;

    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, Type:%x, En:%d\n", __FUNCTION__, __LINE__, enType, bEnable);
    if (enType & E_HAL_DISP_IRQ_TYPE_MOP_VSYNC)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_MOP_VSYNC_BIT;
        u16Msk |= HAL_DISP_IRQ_MOP_VSYNC_MSK;
    }

    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_MOP_MASK_REG(u32DevId), u16Val, u16Msk, NULL);
}

static __inline void _HalDispIrqGetFlag(HalDispIrqType_e enType, u32 *pu32Flag, u32 u32DevId)
{
    u16 u16Val, u16IrqMsk;

    *pu32Flag = 0;

    u16Val    = R2BYTE(HAL_DISP_IRQ_MOP_FLAG_REG(u32DevId));
    u16IrqMsk = R2BYTE(HAL_DISP_IRQ_MOP_MASK_REG(u32DevId));

    if (enType & E_HAL_DISP_IRQ_TYPE_MOP_VSYNC)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_MOP_VSYNC_MSK) ? E_HAL_DISP_IRQ_TYPE_MOP_VSYNC : 0;
    }

    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, enType:%x, Flag:%x (%04x %04x)\n", __FUNCTION__, __LINE__, enType, *pu32Flag,
             u16IrqMsk, u16Val);
    DISP_DBG_VAL(u16IrqMsk);
    DISP_DBG_VAL(u16Val);
}
void HalDispIrqClearFpllUnLock(u32 u32FpllId)
{
    u32 u32Reg = 0;

    u32Reg = GET_DISP_RED(u32FpllId, DISP_TOP, REG_FPLL_UNLOCKED);
    if (u32Reg && u32FpllId < HAL_DISP_FPLL_CNT)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, mask_of_disp_top0_reg_fpll_unlocked, mask_of_disp_top0_reg_fpll_unlocked,
                                      NULL);
    }
}
void HalDispIrqClearDacAffSt(u32 u32DevId)
{
    HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_AFF_FULL_CLR, mask_of_disp_top0_reg_dac_aff_full_clr,
                                  mask_of_disp_top0_reg_dac_aff_full_clr, NULL);
    HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_AFF_EMPTY_CLR, mask_of_disp_top0_reg_dac_aff_empty_clr,
                                  mask_of_disp_top0_reg_dac_aff_empty_clr, NULL);
    HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_AFF_FULL_CLR, 0,
                                  mask_of_disp_top0_reg_dac_aff_full_clr, NULL);
    HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_AFF_EMPTY_CLR, 0,
                                  mask_of_disp_top0_reg_dac_aff_empty_clr, NULL);
}
void HalDispIrqFpllSwRst(u32 u32FpllId, u16 u16Val)
{
    u32 u32Reg = 0;

    u32Reg = GET_DISP_RED(u32FpllId, DISP_TOP, REG_FRAME_PLL_RST);
    if (u32Reg && u32FpllId < HAL_DISP_FPLL_CNT)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val ? mask_of_disp_top0_reg_frame_pll_rst : 0x0000,
                                      mask_of_disp_top0_reg_frame_pll_rst, NULL);
    }
}
u16 _HalDispIrqGetFpllThd(u32 u32FpllId)
{
    u32 u32Reg = 0;
    u16 u16Val = 0;

    u32Reg = GET_DISP_RED(u32FpllId, DISP_TOP, REG_FPLL_PHASE_LOCK_THD);
    if (u32Reg && u32FpllId < HAL_DISP_FPLL_CNT)
    {
        u16Val = (HalDispUtilityR2BYTEDirect(u32Reg)&mask_of_disp_top0_reg_fpll_phase_lock_thd) >> shift_of_disp_top0_reg_fpll_phase_lock_thd;
    }
    return u16Val;
}
void _HalDispIrqSetFpllThd(u32 u32FpllId, u16 u16Val)
{
    u32 u32Reg = 0;

    u32Reg = GET_DISP_RED(u32FpllId, DISP_TOP, REG_FPLL_PHASE_LOCK_THD);
    if (u32Reg && u32FpllId < HAL_DISP_FPLL_CNT)
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val << shift_of_disp_top0_reg_fpll_phase_lock_thd,
                                      mask_of_disp_top0_reg_fpll_phase_lock_thd, NULL);
    }
}
void HaldispSetDacSwRst(HalDispInternalConfigType_e enType, u16 u16Val)
{
    if ((enType & E_HAL_DISP_INTER_CFG_RST_DACSYN))
    {
        HalDispUtilityW2BYTEMSKDirect(DAC_ATOP_REG_DAC_ATOP_SW_RST, (u16Val) ? 0x0003 : 0x0000,
                                      mask_of_dac_atop_reg_dac_atop_sw_rst, NULL);
        if(u16Val != HAL_DISP_DAC_RESET_FORCE)
        {
            HalDispUtilityW2BYTEMSKDirect(DAC_ATOP_REG_DAC_ATOP_SW_RST, 0x0000,
                                          mask_of_dac_atop_reg_dac_atop_sw_rst, NULL);
        }
    }
    if ((enType & E_HAL_DISP_INTER_CFG_RST_DAC))
    {
        HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_SW_RST, (u16Val) ? mask_of_disp_top0_reg_dac_sw_rst : 0x0000,
                                      mask_of_disp_top0_reg_dac_sw_rst, NULL);
    }
    if ((enType & E_HAL_DISP_INTER_CFG_RST_DACAFF))
    {
        HalDispUtilityW2BYTEMSKDirect(DISP_TOP0_REG_DAC_AFF_SW_RST,
                                      (u16Val) ? mask_of_disp_top0_reg_dac_aff_sw_rst : 0x0000,
                                      mask_of_disp_top0_reg_dac_aff_sw_rst, NULL);
    }
}

static __inline void _HalDispIrqClear(HalDispIrqType_e enType, u32 u32DevId)
{
    u16 u16Val, u16Msk;
    u64 u64Time = DrvDispOsGetSystemTimeStamp();

    DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;
    if (enType & E_HAL_DISP_IRQ_TYPE_MOP_VSYNC)
    {
        u16Val |= HAL_DISP_IRQ_MOP_VSYNC_BIT;
        u16Msk |= HAL_DISP_IRQ_MOP_VSYNC_MSK;
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDevHist[u32DevId].u32VsCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDevHist[u32DevId].u64TimeStamp = u64Time);
    }

    // Clear, 1->0
    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_MOP_CLEAR_REG(u32DevId), u16Val, u16Msk, NULL);
    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_MOP_CLEAR_REG(u32DevId), 0, u16Msk, NULL);
    DISP_DBG_VAL(u64Time);
}

static __inline void _HalDispIrqGetId(u8 *pu8Id, u32 u32DevId)
{
    *pu8Id = (u32DevId == HAL_DISP_DEVICE_ID_0)   ? HAL_DISP_IRQ_ID_DEVICE_0
             : (u32DevId == HAL_DISP_DEVICE_ID_1) ? HAL_DISP_IRQ_ID_DEVICE_1
                                                  : HAL_DISP_IRQ_ID_DEVICE_2;
}

static __inline void _HalDispIrqGetDmaIsrIdx(u8 *pu8Id, u32 u32DmaId)
{
    *pu8Id = HAL_DISP_IRQ_ID_DMA;
}

static __inline void _HalDispIrqEnableDmaIsr(HalDispIrqType_e enType, u8 *pbEn, u32 u32DmaId)
{
    u16 u16Val, u16Msk;
    u8  bEnable = *pbEn;

    u16Val = 0;
    u16Msk = 0;

    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, Type:%x, En:%d\n", __FUNCTION__, __LINE__, enType, bEnable);
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_REALDONE)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_DMA_REALDONE_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_REALDONE_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_DMA_FIFOFULL_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_FIFOFULL_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_DMA_REGFAIL_BIT;
        u16Msk |= HAL_DISP_IRQ_DMA_REGFAIL_MSK;
    }
    if (bEnable)
    {
        HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_DMA_CLEAR_REG(u32DmaId), u16Msk, u16Msk, NULL);
    }
    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_DMA_MASK_REG(u32DmaId), u16Val, u16Msk, NULL);
}
void HalDispIrqEnableVsync(u8 u8bEn, u32 u32DmaId, void *pCtx)
{
    HalDispUtilityCmdqContext_t *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, (void *)&pCmdqCtx);
    if (u8bEn)
    {
        HalDispUtilityW2BYTEMSK(HAL_DISP_IRQ_DMA_CLEAR_REG(u32DmaId), HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT,
                                HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK, pCmdqCtx);
    }
    HalDispUtilityW2BYTEMSK(HAL_DISP_IRQ_DMA_MASK_REG(u32DmaId), u8bEn ? 0 : HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT,
                            HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK, pCmdqCtx);
}

static __inline void _HalDispIrqGetDmaFlag(HalDispIrqType_e enType, u32 *pu32Flag, u32 u32DmaId)
{
    u16       u16Val, u16IrqRaw, u16Msk;
    u16       u16ClrVal = 0;
    u8        u8bEn;
    static u8 u8Clrfifofull = 0;
    u64       u64Time       = DrvDispOsGetSystemTimeStamp();
    u64       u64ActTime = 1;

    *pu32Flag = 0;

    u16Val    = R2BYTE(HAL_DISP_IRQ_DMA_FLAG_REG(u32DmaId));
    u16Msk    = R2BYTE(HAL_DISP_IRQ_DMA_MASK_REG(u32DmaId));
    u16IrqRaw = R2BYTE(HAL_DISP_IRQ_DMA_RAWFLAG_REG(u32DmaId));

    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, Id:%d, enType:%x, Msk:%04x, Raw:%04x, Val:%04x \n", __FUNCTION__, __LINE__,
             u32DmaId, enType, u16Msk, u16IrqRaw, u16Val);

    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON)
    {
        if (u16Val & HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK)
        {
            u16ClrVal |= (HAL_DISP_IRQ_DMA_ACTIVE_ON_BIT);
            if (HAL_DISP_IS_IRQ_EN(u16Msk, HAL_DISP_IRQ_DMA_ACTIVE_ON_MSK))
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVE_ON] = u64Time);
            }
            g_stDispIrqHist.stWorkingStatus.stDmaStatus[u32DmaId].u8bDmaIdle = 0;
        }
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_REALDONE)
    {
        if (u16Val & HAL_DISP_IRQ_DMA_REALDONE_MSK)
        {
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_DMA_REALDONE;
            u16ClrVal |= (HAL_DISP_IRQ_DMA_REALDONE_BIT | HAL_DISP_IRQ_DMA_DONE_BIT);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u32FrDoneCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_REALDONE] = u64Time);
            DISP_STATISTIC_VAL(u64ActTime = g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVE_ON]);
            if (u64ActTime)
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVETIME] =
                    u64Time - g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVE_ON]);
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVE_ON] = 0);
                u8bEn                                                                          = 0;
                _HalDispIrqEnableDmaIsr(E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON, &u8bEn, 0);
            }
            g_u8DvContinue = 0;
            g_stDispIrqHist.stWorkingStatus.stDmaStatus[u32DmaId].u8bDmaIdle = 1;
        }
        if (u16Val & HAL_DISP_IRQ_DMA_DONE_MSK)
        {
            if (!HAL_DISP_IS_IRQ_EN(u16Msk, HAL_DISP_IRQ_DMA_REGFAIL_MSK))
            {
                u8bEn = 1;
                _HalDispIrqEnableDmaIsr(E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL, &u8bEn, 0);
            }
            if (!HAL_DISP_IS_IRQ_EN(u16Msk, HAL_DISP_IRQ_DMA_FIFOFULL_MSK))
            {
                if (u8Clrfifofull)
                {
                    u8Clrfifofull = 0;
                    *pu32Flag |= E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL;
                }
                u8bEn = 1;
                _HalDispIrqEnableDmaIsr(E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL, &u8bEn, 0);
            }
            u16ClrVal |= HAL_DISP_IRQ_DMA_DONE_BIT;
        }
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL)
    {
        if (u16Val & HAL_DISP_IRQ_DMA_FIFOFULL_MSK)
        {
            u8bEn = 0;
            _HalDispIrqEnableDmaIsr(E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL, &u8bEn, 0);
            if (u16IrqRaw & HAL_DISP_IRQ_DMA_FIFOFULL_MSK)
            {
                u8Clrfifofull = 1;
            }
            u16ClrVal |= HAL_DISP_IRQ_DMA_FIFOFULL_BIT;
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u32affCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_FIFOFULL] = u64Time);
        }
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL)
    {
        if (u16Val & HAL_DISP_IRQ_DMA_REGFAIL_MSK)
        {
            if (HAL_DISP_IS_IRQ_EN(u16Msk, HAL_DISP_IRQ_DMA_REGFAIL_MSK))
            {
                u8bEn = 0;
                _HalDispIrqEnableDmaIsr(E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL, &u8bEn, 0);
            }
            u16ClrVal |= HAL_DISP_IRQ_DMA_REGFAIL_BIT;
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u32SetFailCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_REGSETFAIL] = u64Time);
        }
    }
    // Clear, 1 Clear
    HalDispUtilityW2BYTEMSKDirectAndNoshadow(HAL_DISP_IRQ_DMA_CLEAR_REG(u32DmaId), u16ClrVal,
                                             mask_of_disp_cvbs_reg_intr_status_grp_a);
    DISP_DBG_VAL(u64Time);
}

static __inline void _HalDispIrqClearDma(HalDispIrqType_e enType, u32 u32DmaId)
{
    DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);
}

static __inline void _HalDispIrqSupportedTimeZoneIsr(u8 *pbSupported)
{
    *pbSupported = DISP_TIMEZONE_ISR_SUPPORT;
}

static __inline void _HalDispIrqGetTimeZoneIsrIdx(u8 *pu8Id, u32 u32DevId)
{
    *pu8Id = (u32DevId == HAL_DISP_DEVICE_ID_1)   ? HAL_DISP_IRQ_ID_TIMEZONE_1
             : (u32DevId == HAL_DISP_DEVICE_ID_2) ? HAL_DISP_IRQ_ID_TIMEZONE_2
                                                  : HAL_DISP_IRQ_ID_TIMEZONE_0;
}

static __inline void _HalDispIrqEnableTimeZoneIsr(HalDispIrqType_e enType, u8 *pbEn, u32 u32DevId)
{
    u16 u16Val, u16Msk;
    u32 u32ClrReg;
    u32 u32MskReg;
    u8  bEnable = *pbEn;
    u16Val      = 0;
    u16Msk      = 0;

    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVEN_BIT;
        u16Msk |= HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVEN_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VS_POSITIVE)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_TIMEZONE_VS_ACTIVE_BIT;
        u16Msk |= HAL_DISP_IRQ_TIMEZONE_VS_ACTIVE_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_UNLOCK)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_BIT;
        u16Msk |= HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_LOCKED)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_BIT;
        u16Msk |= HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_MSK;
    }
    u32ClrReg = HAL_DISP_IRQ_TOP_CLEAR_REG(u32DevId);
    u32MskReg = HAL_DISP_IRQ_TOP_MASK_REG(u32DevId);
    if (bEnable && u32ClrReg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32ClrReg, u16Msk, u16Msk, NULL);
    }
    if (u32MskReg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32MskReg, u16Val, u16Msk, NULL);
    }
}
static void _HalDispIrqPrintDeviceStatus(u32 u32DevId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u32 u32FpllId;

    for (u32FpllId = 0; u32FpllId < HAL_DISP_FPLL_CNT; u32FpllId++)
    {
        if (HalDispGetFpllUsedByDev(u32FpllId, u32DevId))
        {
            DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "     FpllId:%u LockSt:%hx, PhaseErr:%hd, FreqErr:%hd\n", u32FpllId,
                     pstDevStatus->u16LockStatus[u32FpllId], pstDevStatus->u16PhaseErr[u32FpllId],
                     pstDevStatus->u16FreqErr[u32FpllId]);
        }
    }
    if (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn && g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev == u32DevId)
    {
        DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "     Dac Aff st:%hx\n", pstDevStatus->u16DacAffStatus);
    }
}
static void _HalDispIrqGetDeviceStatus(HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u32 u32FpllId;
    u32 u32Reg = 0;

    for (u32FpllId = 0; u32FpllId < HAL_DISP_FPLL_CNT; u32FpllId++)
    {
        u32Reg                                 = GET_DISP_RED(u32FpllId, DISP_TOP, REG_RAW_FREQ_ERR_RD);
        pstDevStatus->u16FreqErr[u32FpllId]    = R2BYTE(u32Reg);
        u32Reg                                 = GET_DISP_RED(u32FpllId, DISP_TOP, REG_RAW_PHASE_ERR_RD);
        pstDevStatus->u16PhaseErr[u32FpllId]   = R2BYTE(u32Reg);
        u32Reg                                 = GET_DISP_RED(u32FpllId, DISP_TOP, REG_FPLL_UNLOCKED);
        pstDevStatus->u16LockStatus[u32FpllId] = (R2BYTE(u32Reg) & (mask_of_disp_top0_reg_fpll_locked));
    }
    pstDevStatus->u16DacAffStatus =
        R2BYTE(DISP_TOP0_REG_DAC_AFF_FULL_FLAG)
        & (mask_of_disp_top0_reg_dac_aff_full_flag | mask_of_disp_top0_reg_dac_aff_empty_flag);
}
static u8 _HalDispIrqIsLockedTmp(u32 u32FpllId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u8 ret = 0;
    u8 cnt = 0;

    if (u32FpllId < HAL_DISP_FPLL_CNT)
    {
        while (DISP_FPLL_LOCK_STATUS_LOCKED(pstDevStatus->u16LockStatus[u32FpllId]))
        {
            if (cnt > HAL_DISP_FPLL_LOCKED_CHECK_TIME)
            {
                ret = 1;
                break;
            }
            cnt++;
            _HalDispIrqGetDeviceStatus(pstDevStatus);
        }
    }
    return ret;
}
static void _HalDispIrqHandleFpllIrqOnOff(u32 u32FpllId, u8 u8bLock, u8 u8bOnOff)
{
    u8               u8bEn;
    HalDispIrqType_e enType;

    u8bEn = u8bOnOff;
    if (u8bLock)
    {
        enType = E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_LOCKED;
    }
    else
    {
        enType = E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_UNLOCK;
    }
    _HalDispIrqEnableTimeZoneIsr(enType, &u8bEn, u32FpllId);
}
static u32 _HalDispIrqHandleFpllSetStm(u32 u32FpllId, HalDispIrqTimeZoneFpllStatus_e enStm)
{
    u32 u32Ret     = 1;
    u64 u64CurTime = DrvDispOsGetSystemTimeStamp();

    switch (enStm)
    {
        case E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_DISABLE:
            break;
        case E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING:
            if ((DISP_IRQ_FPLL_STM_STATBLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE;
            }
            break;
        case E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED:
            if ((DISP_IRQ_FPLL_STM_FREE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
                || (DISP_IRQ_FPLL_STM_DISABLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING;
            }
            else if (DISP_IRQ_FPLL_STM_STATBLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE;
            }

            break;
        case E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE:
            if ((DISP_IRQ_FPLL_STM_LOCKING(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED;
            }
            else if ((DISP_IRQ_FPLL_STM_FREE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
                     || (DISP_IRQ_FPLL_STM_DISABLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING;
            }
            break;
        case E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE:
            if ((DISP_IRQ_FPLL_STM_LOCKING(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
                || (DISP_IRQ_FPLL_STM_LOCKED(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
            {
                enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING;
            }
            break;
        default:
            u32Ret = 0;
            break;
    }

    if (u32Ret)
    {
        if (DISP_IRQ_FPLL_STM_STATBLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm) && DISP_IRQ_FPLL_STM_FREE(enStm))
        {
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32FpllLostLockCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u64FpllLostLockTime = u64CurTime);
            DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_FPLL,
                     "%s %d, Id:%d,Lost Stable Locked to Free LostCnt=%u DacRstCnt=%u FpllRstCnt=%u @%llu\n",
                     __FUNCTION__, __LINE__, u32FpllId, g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32FpllLostLockCnt,
                     g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt, g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt,
                     u64CurTime);
        }
        else if ((DISP_IRQ_FPLL_STM_LOCKED(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
                 && DISP_IRQ_FPLL_STM_STATBLE(enStm))
        {
            DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_FPLL,
                     "%s %d, Id:%d,FPLL Stable Locked UnstableCnt=%u DacRstCnt=%u FpllRstCnt=%u @%llu\n", __FUNCTION__,
                     __LINE__, u32FpllId, g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt,
                     g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt, g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt,
                     u64CurTime);
            g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt = 0;
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u64FpllStableTime = u64CurTime);
        }
        if ((DISP_IRQ_FPLL_STM_LOCKED(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm))
            && !(DISP_IRQ_FPLL_STM_LOCKED(enStm)))
        {
            g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32LockedFrmCnt = 0;
        }
        g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm = enStm;
    }
    DISP_DBG_VAL(u64CurTime);
    return u32Ret;
}
static u8 _HalDispIrqHandleFpllCheckLocked(u32 u32FpllId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    if (_HalDispIrqIsLockedTmp(u32FpllId, pstDevStatus))
    {
        g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLocked = 1;
    }
    else
    {
        g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLocked = 0;
        HalDispSetFpllLockedStableFlag(u32FpllId, 0, NULL);
    }
    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllEn = 1;
    return g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLocked;
}
static void _HalDispIrqHandleFpllStatusInVde(u32 u32FpllId, u32 u32DevId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u64 u64CurTime = DrvDispOsGetSystemTimeStamp();
    s16 s16PhaseErr = pstDevStatus->u16PhaseErr[u32FpllId];
    u16 u16Thrd = 0;

    if (HalDispGetFpllUsed(u32FpllId))
    {
        if (HalDispGetFpllUnlocked(u32FpllId) || DISP_FPLL_LOCK_STATUS_LOST(pstDevStatus->u16LockStatus[u32FpllId]))
        {
            if (_HalDispIrqHandleFpllCheckLocked(u32FpllId, pstDevStatus) == 0)
            {
                g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt++;
                if((pstDevStatus->u16PhaseErr[u32FpllId]) == 0x7FFF || (pstDevStatus->u16PhaseErr[u32FpllId]) == 0x8000)
                {
                    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32PhaseErrCnt++;
                    if(0==(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32PhaseErrCnt % HAL_DISP_FPLL_CONTINUE_ERR_PHASE_CNT))
                    {
                        HalDispSetSwReset(1, NULL, u32DevId);
                        HalDispSetSwReset(0, NULL, u32DevId);
                        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32PhaseErrRstCnt++);
                        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u64PhaseErrRstTime = u64CurTime);
                        DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_FPLL,
                                 "%s %d, Id: %d,DevId: %d, PhaseErrCnt: %u  ,PhaseErrRstCnt: %u ,FpllRstCnt: %u FpllUstableCnt:%hu @%llu\n",
                                 __FUNCTION__, __LINE__, u32FpllId, u32DevId,g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32PhaseErrCnt,
                                 g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32PhaseErrRstCnt,
                                 g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt, 
                                 g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt, u64CurTime);
                    }
                }
                else
                {
                    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32PhaseErrCnt = 0;
                }
                u16Thrd = _HalDispIrqGetFpllThd(u32FpllId);
                if (0 == (g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt % HAL_DISP_FPLL_LOCKED_UNSTABLE_ADJUST_FRAME))
                {
                    if(s16PhaseErr < 100 && s16PhaseErr > -100 && u16Thrd< 0xFF)
                    {
                        u16Thrd++;
                        _HalDispIrqSetFpllThd(u32FpllId, u16Thrd);
                    }
                    DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_FPLL,
                             "%s %d, Id: %d, UnstableFrmCnt: %u  ,LostLockCnt: %u ,FpllRstCnt: %u FpllThrd:%hu @%llu\n",
                             __FUNCTION__, __LINE__, u32FpllId, g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt,
                             g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32FpllLostLockCnt,
                             g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt, u16Thrd, u64CurTime);
                }
                if (0 == (g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt % HAL_DISP_FPLL_LOCKED_UNSTABLE_RST_FRAME))
                {
                    if(!(s16PhaseErr < 100 && s16PhaseErr > -100 && u16Thrd< 0xFF))
                    {
                        HalDispIrqFpllSwRst(u32FpllId, 1);
                        HalDispIrqFpllSwRst(u32FpllId, 0);
                        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt++);
                        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u64UnLockRstTime = u64CurTime);
                    }
                    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLockedIrqEn = 1;
                    DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_FPLL,
                             "%s %d, Id: %d, UnstableFrmCnt: %u  ,LostLockCnt: %u ,FpllRstCnt: %u FpllThrd:%hu @%llu\n",
                             __FUNCTION__, __LINE__, u32FpllId, g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32UnstableFrmCnt,
                             g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32FpllLostLockCnt,
                             g_stDispIrqHist.stWorkingHist.stFpllHist[u32FpllId].u32UnLockRstCnt, u16Thrd, u64CurTime);
                }
                _HalDispIrqHandleFpllSetStm(u32FpllId, E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING);
            }
            else
            {
                _HalDispIrqHandleFpllSetStm(u32FpllId, E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED);
            }
        }
        else if (!(DISP_IRQ_FPLL_STM_STATBLE(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].enFpllStm)))
        {
            _HalDispIrqHandleFpllSetStm(u32FpllId, E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED);
            g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32LockedFrmCnt++;
            if (g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u32LockedFrmCnt > HAL_DISP_FPLL_LOCKED_STABLE_CHECK_FRAME)
            {
                _HalDispIrqHandleFpllSetStm(u32FpllId, E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE);
                g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllUnLockIrqEn = 1;
                g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLockedIrqEn = 1;
                HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DACAFF, HAL_DISP_DAC_RESET_OFF);
                HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DACAFF, HAL_DISP_DAC_RESET_ON);
                HalDispSetFpllLockedStableFlag(u32FpllId, 1, NULL);
            }
        }
    }
    DISP_DBG_VAL(u64CurTime);
}
static void _HalDispIrqHandleDacAffInVde(u32 u32DevId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u16        u16DacMux;
    u16        u16DacEnable;
    u8         u8bStatusErr          = 0;
    static u32 u32FifoContinueErrCnt = 0;
    u64        u64CurTime            = DrvDispOsGetSystemTimeStamp();

    u16DacMux =
        ((R2BYTE(DISP_TOP0_REG_DAC_SRC_MUX) & mask_of_disp_top0_reg_dac_src_mux) >> shift_of_disp_top0_reg_dac_src_mux);
    u16DacEnable =
        ((R2BYTE(DAC_ATOP_REG_EN_IDAC_R) & mask_of_dac_atop_reg_en_idac_r) >> shift_of_dac_atop_reg_en_idac_r);
    if (u32DevId == u16DacMux)
    {
        if (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn && g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev == u32DevId)
        {
            if (DISP_DACAFF_STATUS_FULL(pstDevStatus->u16DacAffStatus))
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32affCnt++);
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOFULL] = u64CurTime);
                u8bStatusErr                                                        = 1;
            }
            if (DISP_DACAFF_STATUS_EMPTY(pstDevStatus->u16DacAffStatus))
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32afeCnt++);
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOEMPTY] = u64CurTime);
                u8bStatusErr                                                         = 1;
            }
            if (u8bStatusErr)
            {
                u32FifoContinueErrCnt++;
                if (u32FifoContinueErrCnt == HAL_DISP_AFF_CONTINUE_ERR_MUTE_CNT)
                {
                    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute = 1;
                    DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_TIMEZONE,
                             "%s %d,CONTINUE_ERR_MUTE Id: %d, affcnt: %u  ,afecnt: %u ,DacRstCnt: %u @%llu\n",
                             __FUNCTION__, __LINE__, u32DevId, g_stDispIrqHist.stWorkingHist.stVgaHist.u32affCnt,
                             g_stDispIrqHist.stWorkingHist.stVgaHist.u32afeCnt, g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt, u64CurTime);
                }
                else if (u32FifoContinueErrCnt == HAL_DISP_AFF_CONTINUE_ERR_RST_CNT)
                {
                    DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_TIMEZONE,
                             "%s %d,CONTINUE_ERR_RST Id: %d, affcnt: %u  ,afecnt: %u ,DacRstCnt: %u @%llu\n",
                             __FUNCTION__, __LINE__, u32DevId, g_stDispIrqHist.stWorkingHist.stVgaHist.u32affCnt,
                             g_stDispIrqHist.stWorkingHist.stVgaHist.u32afeCnt, g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt, u64CurTime);
                    u32FifoContinueErrCnt = 0;
                    DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt++);
                    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst = HAL_DISP_DAC_RESET_ON;
                }
            }
            else
            {
                u32FifoContinueErrCnt = 0;
                if (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute)
                {
                    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute = 0;
                }
                if (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst == HAL_DISP_DAC_RESET_ON)
                {
                    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst = HAL_DISP_DAC_RESET_OFF;
                }
            }
            HalDispIrqSetDacMute(g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute);
            if(g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst != HAL_DISP_DAC_RESET_FORCE)
            {
                HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DACALL, g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst);
            }
            HalDispIrqClearDacAffSt(u32DevId);
            g_stInterCfg[u32DevId].bRstDac    = g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst;
            g_stInterCfg[u32DevId].bRstDacAff = g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst;
            g_stInterCfg[u32DevId].bRstDacSyn = g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst;
        }
        else
        {
            if (u16DacEnable)
            {
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn     = 1;
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute     = 0;
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev = u32DevId;
            }
            else
            {
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn     = 0;
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute     = 0;
                g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev = HAL_DISP_DEVICE_MAX;
            }
        }
    }
    DISP_DBG_VAL(u64CurTime);
}
static void _HalDispIrqHandleFpllInVde(u32 u32DevId, HalDispIrqDeviceStatus_t *pstDevStatus)
{
    u16 u16FpllId, u16MskVal = 0;
    u32 u32MskReg;

    u32MskReg = HAL_DISP_IRQ_TOP_MASK_REG(u32DevId);
    if (u32MskReg)
    {
        u16MskVal = R2BYTE(u32MskReg);
    }
    for (u16FpllId = HAL_DISP_FPLL_0_VGA_HDMI; u16FpllId < HAL_DISP_FPLL_CNT; u16FpllId++)
    {
        if (HalDispGetFpllUsedByDev(u16FpllId, u32DevId))
        {
            _HalDispIrqHandleFpllStatusInVde(u16FpllId, u32DevId, pstDevStatus);
            if (!HAL_DISP_IS_IRQ_EN(u16MskVal, HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_BIT)
                && g_stDispIrqHist.stWorkingStatus.stFpllStatus[u16FpllId].u8FpllLockedIrqEn)
            {
                _HalDispIrqHandleFpllIrqOnOff(u16FpllId, 1, 1);
            }
            if (!HAL_DISP_IS_IRQ_EN(u16MskVal, HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_BIT)
                && g_stDispIrqHist.stWorkingStatus.stFpllStatus[u16FpllId].u8FpllUnLockIrqEn)
            {
                _HalDispIrqHandleFpllIrqOnOff(u16FpllId, 0, 1);
            }
            HalDispIrqClearFpllUnLock(u16FpllId);
        }
    }
}
#define FPSSEC ((1000000) * 10)
u32 HalDispIrqSetFps(u64 u64Time, u64 u64OriTime, u32 u32DoneCount)
{
    u32 u32Temp = 0;
    u32 u32Fps  = 0;

    if (u32DoneCount)
    {
        u32Temp = (((u32)u64Time - (u32)u64OriTime) / u32DoneCount);

        if (u32Temp)
        {
            u32Fps = FPSSEC / u32Temp; // FPSSEC (x10) for get integer and float fps.
        }
    }
    return u32Fps;
}
void HalDispIrqSetDevIntClr(u32 u32DevId, u16 val)
{
    u32 u32ClrReg;

    u32ClrReg = HAL_DISP_IRQ_TOP_CLEAR_REG(u32DevId);
    if (u32ClrReg)
    {
        HalDispUtilityW2BYTEMSKDirect(u32ClrReg, val, mask_of_disp_top0_reg_intr_status_grp_a, NULL);
    }
}
static void _HalDispIrqDmaDoubleVsyncHandler(u64 u64Time, u32 u32DmaId)
{
    DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u32DmaRstCnt++);
    DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_RST] = u64Time);
    HalDispDmaSwRst(NULL, E_HAL_DISP_DMA_RST_WOPATH_H, E_DISP_UTILITY_REG_ACCESS_CPU);
    HalDispDmaSwRst(NULL, E_HAL_DISP_DMA_RST_WOPATH_L, E_DISP_UTILITY_REG_ACCESS_CPU);
    HalDispDmaSetFrmWTrigger(1, NULL);
    DISP_DBG_STAT(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, Id:%d, Double Vsync without frame done VCnt:%hu,FDT@:%llu ,Rst @%llu\n", __FUNCTION__,
             __LINE__, u32DmaId, HalDispDmaGetFrmVCnt(u32DmaId, 1),
             g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_REALDONE], u64Time);
}
static void _HalDispIrqDmaMiscHandler(u64 u64Time, u32 u32DevId, u32 u32DmaId)
{
    u16 u16CmdqInProcess;
    u16 u16FenceId;
    static u16 u16LastFenceId = 0;
    
    if(g_stDispIrqHist.stWorkingStatus.stDmaStatus[u32DmaId].u8DmaBindDevSrc == u32DevId)
    {
        u16CmdqInProcess =
            HalDispUtilityR2BYTEMaskDirect(REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(HAL_DISP_UTILITY_CMDQ_ID_DMA),
                                           REG_CMDQ_IN_PROCESS_MSK | REG_CMDQ_DEV_TIMEGEN_START_MSK | REG_CMDQ_PROCESS_FENCE_MSK);
        u16FenceId = (u16CmdqInProcess & REG_CMDQ_PROCESS_FENCE_MSK);
        if(((u16CmdqInProcess & REG_CMDQ_IN_PROCESS_MSK) == REG_CMDQ_IN_PROCESS_ON) && u16LastFenceId == u16FenceId)
        {
            g_u8DvContinue++;
            if (g_u8DvContinue > HAL_DISP_IRQ_DMAMISC_DOUBLEVS_CNT)
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u32DoubleVsyncCnt++);
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DmaId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_DOUBLEVSYNC] = u64Time);
                _HalDispIrqDmaDoubleVsyncHandler(u64Time, u32DmaId);
                g_u8DvContinue = 0;
            }
        }
        else
        {
            g_u8DvContinue = 0;
        }
        u16LastFenceId = u16FenceId;
    }
}
static __inline void _HalDispIrqGetTimeZoneFlag(HalDispIrqType_e enType, u32 *pu32Flag, u32 u32DevId)
{
    u16                            u16Val, u16MskVal, u16ClrVal;
    HalDispIrqDeviceStatus_t       stDeviceStatus;
    HalDispIrqTimeZoneFpllStatus_e enStm;
    u64                            u64CurTime = DrvDispOsGetSystemTimeStamp();
    u32                            u32ClrReg, u32FlgReg, u32MskReg;
    u32                            u32VDeDoneCnt = 1;

    u32ClrReg = HAL_DISP_IRQ_TOP_CLEAR_REG(u32DevId);
    u32FlgReg = HAL_DISP_IRQ_TOP_FLAG_REG(u32DevId);
    u32MskReg = HAL_DISP_IRQ_TOP_MASK_REG(u32DevId);
    _HalDispIrqGetDeviceStatus(&stDeviceStatus);
    if (u32FlgReg)
    {
        u16Val = R2BYTE(u32FlgReg);
    }
    if (u32MskReg)
    {
        u16MskVal = R2BYTE(u32MskReg);
    }
    u16ClrVal = u16Val & (~u16MskVal);
    *pu32Flag = 0;
    DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Id:%d, enType:%x, Msk:%04x, Val:%04x @%llu\n", __FUNCTION__, __LINE__,
             u32DevId, enType, u16MskVal, u16Val, u64CurTime);
    _HalDispIrqPrintDeviceStatus(u32DevId, &stDeviceStatus);
    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VS_POSITIVE)
    {
        if (u16Val & HAL_DISP_IRQ_TIMEZONE_VS_ACTIVE_BIT)
        {
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32VSCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VS_POSTIVE] = u64CurTime);
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VS_POSITIVE;
        }
    }
    // only for enable,devid = fpllid in HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_BIT
    if (u32DevId < HAL_DISP_FPLL_CNT && u16ClrVal & HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK_BIT)
    {
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32FpllUnlockCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK] = u64CurTime);
        *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_UNLOCK;
        if (_HalDispIrqHandleFpllCheckLocked(u32DevId, &stDeviceStatus) == 0)
        {
            g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32DevId].u8FpllUnLockIrqEn = 0;
            _HalDispIrqHandleFpllIrqOnOff(u32DevId, 0, 0);
            enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE;
        }
        else
        {
            enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED;
        }
        _HalDispIrqHandleFpllSetStm(u32DevId, enStm);
        DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Id:%d, FPLL_UNLOCK, Stm:%u, %s @%llu\n", __FUNCTION__, __LINE__,
                 u32DevId, enStm, PARSING_FPLL_STM(enStm), u64CurTime);
    }
    // only for enable,devid = fpllid in HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_BIT
    if (u32DevId < HAL_DISP_FPLL_CNT && u16ClrVal & HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED_BIT)
    {
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32FplllockCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED] = u64CurTime);
        *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_LOCKED;
        if (_HalDispIrqHandleFpllCheckLocked(u32DevId, &stDeviceStatus) == 0)
        {
            g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32DevId].u8FpllLockedIrqEn = 0;
            _HalDispIrqHandleFpllIrqOnOff(u32DevId, 1, 0);
            enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING;
        }
        else
        {
            enStm = E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED;
        }
        _HalDispIrqHandleFpllSetStm(u32DevId, enStm);
        DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Id:%d, FPLL_LOCKED, Stm:%u, %s @%llu\n", __FUNCTION__, __LINE__,
                 u32DevId, enStm, PARSING_FPLL_STM(enStm), u64CurTime);
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        if (u16Val & HAL_DISP_IRQ_TIMEZONE_VDE_ACTIVEN_BIT)
        {
            DISP_STATISTIC_VAL(u32VDeDoneCnt = g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32VDeDoneCnt);
            if (u32VDeDoneCnt % 1000 == 0)
            {
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32fps = HalDispIrqSetFps(
                    g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VDE_NEGATIVE],
                    g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp_Vde0, 1000));
                DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp_Vde0 = u64CurTime);
            }
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u32VDeDoneCnt++);
            DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VDE_NEGATIVE] = u64CurTime);
            *pu32Flag |= E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE;
            _HalDispIrqHandleDacAffInVde(u32DevId, &stDeviceStatus);
            _HalDispIrqHandleFpllInVde(u32DevId, &stDeviceStatus);
            HalDispSetTimeGenStartFlag(1, NULL, u32DevId);
            _HalDispIrqDmaMiscHandler(u64CurTime, u32DevId, 0);
        }
    }
    g_stDispIrqHist.stWorkingStatus.stTimeZoneStatus[u32DevId].u32IrqFlags = *pu32Flag;
    if (u32ClrReg)
    {
        HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32ClrReg, u16ClrVal, mask_of_disp_top0_reg_intr_status_grp_a);
    }
}

static __inline void _HalDispIrqClearTimeZone(HalDispIrqType_e enType, u32 u32DevId) {}

static void _HalDispIrqGetTimeZoneStatus(HalDispIrqType_e enType, HalDispIrqTimeZoneStatus_t *pstStatus, u32 u32DevId)
{
    u64 u64Time = 0;
    
    pstStatus->u32IrqFlags = g_stDispIrqHist.stWorkingStatus.stTimeZoneStatus[u32DevId].u32IrqFlags;
    DISP_STATISTIC_VAL(u64Time = g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VDE_NEGATIVE]);
    pstStatus->u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE] = u64Time;
    DISP_STATISTIC_VAL(u64Time = g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VS_POSTIVE]);
    pstStatus->u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE] =
        pstStatus->u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_NEGATIVE] =
            pstStatus->u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE] =
                u64Time;
}

static __inline void _HalDispIrqSupportedVgaHpdIsr(u8 *pbSupported)
{
    *pbSupported = DISP_VGA_HPD_ISR_SUPPORT;
}

static __inline void _HalDispIrqGetVgaHpdIsrIdx(u8 *pu8Id)
{
    *pu8Id = HAL_DISP_IRQ_ID_VGAHPD;
}

static __inline void _HalDispIrqEnableVgaHpdIsr(HalDispIrqType_e enType, u8 *pbEn)
{
    u16 u16Val, u16Msk;
    u8  bEnable = *pbEn;
    u16Val      = 0;
    u16Msk      = 0;

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_VGA_HPD_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_ON_MSK;
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_VGA_HPD_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_OFF_MSK;
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_VGA_AFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_AFF_MSK;
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY)
    {
        u16Val |= bEnable ? 0 : HAL_DISP_IRQ_VGA_AFE_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_AFE_MSK;
    }

    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8Vgaused = bEnable;
    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_VGA_HPD_MASK_REG, u16Val, u16Msk, NULL);
}

static __inline void _HalDispIrqGetVgaHpdFlag(HalDispIrqType_e enType, u32 *pu32Flag)
{
    u16 u16Val;
    u8  u8bEn;
    // u16 u16IrqMsk;
    *pu32Flag = 0;

    u16Val = R2BYTE(HAL_DISP_IRQ_VGA_HPD_FLAG_REG);
    // u16IrqMsk = R2BYTE(HAL_DISP_IRQ_TOP_MASK_REG);

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_HPD_ON_MSK) ? E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON : 0;
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_HPD_OFF_BIT) ? E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF : 0;
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_AFF_MSK) ? E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL : 0;
        if ((u16Val & HAL_DISP_IRQ_VGA_AFF_MSK))
        {
            u8bEn = 0;
            _HalDispIrqEnableVgaHpdIsr(E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL, &u8bEn);
        }
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY)
    {
        *pu32Flag |= (u16Val & HAL_DISP_IRQ_VGA_AFE_MSK) ? E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY : 0;
        if ((u16Val & HAL_DISP_IRQ_VGA_AFE_MSK))
        {
            u8bEn = 0;
            _HalDispIrqEnableVgaHpdIsr(E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY, &u8bEn);
        }
    }

    g_u32VgaHpdIsrStatus = *pu32Flag;
    // DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x, Flag:%x (%04x %04x)\n", __FUNCTION__, __LINE__, enType,
    // *pu32Flag, u16IrqMsk, u16Val);
}

static __inline void _HalDispIrqClearVgaHpd(HalDispIrqType_e enType)
{
    u16 u16Val, u16Msk;
    u64 u64Time = DrvDispOsGetSystemTimeStamp();

    DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x\n", __FUNCTION__, __LINE__, enType);

    u16Val = 0;
    u16Msk = 0;

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON)
    {
        u16Val |= HAL_DISP_IRQ_VGA_HPD_ON_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_ON_MSK;
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32HPDonCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_HPD_ON] = u64Time);
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF)
    {
        u16Val |= HAL_DISP_IRQ_VGA_HPD_OFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_HPD_OFF_MSK;
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32HPDoffCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_HPD_OFF] = u64Time);
    }

    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL)
    {
        u16Val |= HAL_DISP_IRQ_VGA_AFF_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_AFF_MSK;
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32affCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOFULL] = u64Time);
    }
    if (enType & E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY)
    {
        u16Val |= HAL_DISP_IRQ_VGA_AFE_BIT;
        u16Msk |= HAL_DISP_IRQ_VGA_AFE_MSK;
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u32afeCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOEMPTY] = u64Time);
    }
    HalDispUtilityW2BYTEMSKDirect(HAL_DISP_IRQ_VGA_HPD_CLEAR_REG, u16Val, u16Msk, NULL);
    DISP_DBG_VAL(u64Time);
    // DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, enType:%x \n", __FUNCTION__, __LINE__, enType);
}

static __inline void _HalDispIrqGetVgaHpdIsrStatus(u32 *pu32Status)
{
    *pu32Status = g_u32VgaHpdIsrStatus;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 HalDispIrqIoCtl(HalDispIrqIoctlConfig_t *pCfg)
{
    u8                         bRet          = 1;
    DrvDispCtxConfig_t *       pstDispCtx    = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxDmaContain_t *   pstDmaContain = NULL;

    pstDispCtx = (DrvDispCtxConfig_t *)pCfg->pDispCtx;

    if (pstDispCtx == NULL)
    {
        DISP_ERR("%s %d, Null DispCtx\n", __FUNCTION__, __LINE__);
        return 0;
    }

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];

        DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, ContainId:%d, DevId:%d, Ioctl:%s, IrqType:%s\n", __FUNCTION__, __LINE__,
                 pstDispCtx->u32ContainIdx, pstDevContain->u32DevId, PARSING_HAL_IRQ_IOCTL(pCfg->enIoctlType),
                 PARSING_HAL_IRQ_TYPE(pCfg->enIrqType));
    }
    else
    {
        pstDmaContain = pstDispCtx->pstCtxContain->pstDmaContain[pstDispCtx->u32ContainIdx];

        if (pstDmaContain->pSrcDevContain != NULL)
        {
            pstDevContain = (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain;
            DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, ContainId:%d, DmaId:%d, Ioctl:%s, IrqType:%s\n", __FUNCTION__,
                     __LINE__, pstDispCtx->u32ContainIdx, pstDmaContain->u32DmaId, PARSING_HAL_IRQ_IOCTL(pCfg->enIoctlType),
                     PARSING_HAL_IRQ_TYPE(pCfg->enIrqType));
        }
        else
        {
            DISP_ERR("%s %d, Dma No Bind Src Device\n", __FUNCTION__, __LINE__);
            return 0;
        }
    }

    switch (pCfg->enIoctlType)
    {
        case E_HAL_DISP_IRQ_IOCTL_ENABLE:
            _HalDispIrqEnableIsr(pCfg->enIrqType, (u8 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_GET_FLAG:
            _HalDispIrqGetFlag(pCfg->enIrqType, (u32 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_CLEAR:
            _HalDispIrqClear(pCfg->enIrqType, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_GET_ID:
            _HalDispIrqGetId((u8 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE:
            _HalDispIrqEnableDmaIsr(pCfg->enIrqType, (u8 *)pCfg->pParam, pstDmaContain->u32DmaId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG:
            _HalDispIrqGetDmaFlag(pCfg->enIrqType, (u32 *)pCfg->pParam, pstDmaContain->u32DmaId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR:
            _HalDispIrqClearDma(pCfg->enIrqType, pstDmaContain->u32DmaId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID:
            _HalDispIrqGetDmaIsrIdx((u8 *)pCfg->pParam, pstDmaContain->u32DmaId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED:
            _HalDispIrqSupportedTimeZoneIsr((u8 *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID:
            _HalDispIrqGetTimeZoneIsrIdx((u8 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE:
            _HalDispIrqEnableTimeZoneIsr(pCfg->enIrqType, (u8 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG:
            _HalDispIrqGetTimeZoneFlag(pCfg->enIrqType, (u32 *)pCfg->pParam, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS:
            _HalDispIrqGetTimeZoneStatus(pCfg->enIrqType, (HalDispIrqTimeZoneStatus_t *)pCfg->pParam,
                                         pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR:
            _HalDispIrqClearTimeZone(pCfg->enIrqType, pstDevContain->u32DevId);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED:
            _HalDispIrqSupportedVgaHpdIsr((u8 *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID:
            _HalDispIrqGetVgaHpdIsrIdx((u8 *)pCfg->pParam);
            break;

        case E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE:
            _HalDispIrqEnableVgaHpdIsr(pCfg->enIrqType, (u8 *)pCfg->pParam);
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
void HalDispIrqSetDmaTriggerCount(u8 u8Reset, u32 u32DevId)
{
    if (u8Reset)
    {
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DevId].u32FrTrigCnt                          = 0);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_TRIG] = 0);
    }
    else
    {
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DevId].u32FrTrigCnt++);
        DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDmaHist[u32DevId].u64TimeStamp[E_HAL_DISP_IRQ_DMA_TRIG] = DrvDispOsGetSystemTimeStamp());
    }
}
void HalDispIrqSetDacMute(u8 u8Mute)
{
    HalDispUtilityW2BYTEMSKDirect(DAC_ATOP_REG_EN_IDAC_B, u8Mute ? 0x0000 : mask_of_dac_atop_reg_en_idac_b,
                                  mask_of_dac_atop_reg_en_idac_b, NULL);
    HalDispUtilityW2BYTEMSKDirect(DAC_ATOP_REG_EN_IDAC_G, u8Mute ? 0x0000 : mask_of_dac_atop_reg_en_idac_g,
                                  mask_of_dac_atop_reg_en_idac_g, NULL);
    HalDispUtilityW2BYTEMSKDirect(DAC_ATOP_REG_EN_IDAC_R, u8Mute ? 0x0000 : mask_of_dac_atop_reg_en_idac_r,
                                  mask_of_dac_atop_reg_en_idac_r, NULL);
}
void HalDispIrqSetFpllEn(u32 u32FpllId, u8 u8bEn)
{
    HalDispIrqTimeZoneFpllStatus_e enFpllStm;

    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllUnLockIrqEn = u8bEn;
    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllLockedIrqEn = u8bEn;
    g_stDispIrqHist.stWorkingStatus.stFpllStatus[u32FpllId].u8FpllEn          = u8bEn;
    enFpllStm = (u8bEn == 0) ? E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_DISABLE : E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE;
    _HalDispIrqHandleFpllSetStm(u32FpllId, enFpllStm);
}
void HalDispIrqSetDacEn(u32 u32DevId, u8 u8bEn)
{
    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn     = u8bEn;
    g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev = u32DevId;
}
void HalDispIrqInit(void)
{
    g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u8bDmaIdle = 1;
    g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u8DmaBindDevSrc= 0xFF;
    g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u8DmaBindDevDst= 0xFF;
}
