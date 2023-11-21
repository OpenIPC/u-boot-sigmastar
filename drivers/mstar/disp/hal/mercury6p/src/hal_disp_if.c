/*
* hal_disp_if.c- Sigmastar
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

#define _HAL_DISP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_util.h"
#include "hal_disp_st.h"
#include "hal_disp_op2.h"
#include "hal_disp_clk.h"
#include "hal_disp_color.h"
#include "hal_disp_mop.h"
#include "hal_disp_mop_if.h"
#include "hal_disp_reg.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "drv_disp_os.h"
#include "hal_disp_mace.h"
#include "hal_disp_hpq.h"
#include "hal_disp_irq.h"
#include "hal_disp_if.h"
#include "hal_disp_dma_if.h"
#include "hal_disp_picture_if.h"
#include "hal_disp_vga_timing_tbl.h"
#include "hal_disp_cvbs_timing_tbl.h"
#include "hal_hdmitx_top_tbl.h"
#include "hal_disp_dma.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8                         g_bDispHwIfInit = 0;
HalDispQueryCallBackFunc_t g_pDispCbTbl[E_HAL_DISP_QUERY_MAX];
HalDispInternalConfig_t    g_stInterCfg[HAL_DISP_DEVICE_MAX] = {
    {0, 0, HAL_DISP_COLOR_CSC_ID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, HAL_DISP_COLOR_CSC_ID, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// color & picture
s16 g_stVideoColorCorrectionMatrix[] = {
    0x0400,  0x0000, 0x0000,  0x0000, 0x0400,  0x0000,  0x0000,  0x0000, 0x0400,  -0x034B, 0x0196,
    -0x068B, 0x03C9, -0x0439, 0x0032, -0x0004, -0x07EE, 0x04E7,  0x07CB, -0x04C3, 0x0404,  0x023B,
    -0x023E, 0x01D5, -0x0831, 0x0100, -0x0102, 0x0101,  -0x0101, 0x0000, 0x0000,  0x0000,
};

u16                           g_a16VgaDacTrimDefault[3] = {0, 0, 0};
HalDispDrvTurningConfig_t     g_stVgaDacTrimCfg;
extern u8 g_u8PkSramEn[HAL_DISP_DEVICE_MAX];
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static u8 _HalDispIfGetFpllThrd(u32 u32Dclk)
{
    u8 u8FpllThrd;
    if(u32Dclk < 50000000)
    {
            u8FpllThrd = 0x08;
    }
    else if(u32Dclk < 75000000)
    {
            u8FpllThrd = 0x05;
    }
    else if(u32Dclk < 100000000)
    {
            u8FpllThrd = 0x03;
    }
    else if(u32Dclk < 125000000)
    {
            u8FpllThrd = 0x02;
    }
    else
    {
            u8FpllThrd = 0x01;
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Dclk:%d FpllThrd:%hhx\n", __FUNCTION__, __LINE__, u32Dclk, u8FpllThrd);
    return u8FpllThrd;
}
static u16 _HalDispIfGetTblId(HalDispDeviceTiming_e eTimeType)
{
    u16 u16TblId = E_HAL_DISP_VGA_ID_NUM;

    switch (eTimeType)
    {
        case E_HAL_DISP_OUTPUT_PAL:
            u16TblId = E_HAL_DISP_CVBS_ID_PAL;
            break;
        case E_HAL_DISP_OUTPUT_NTSC:
            u16TblId = E_HAL_DISP_CVBS_ID_NTSC;
            break;
        case E_HAL_DISP_OUTPUT_720P24:
            u16TblId = E_HAL_DISP_VGA_ID_720_24P;
            break;
        case E_HAL_DISP_OUTPUT_720P25:
            u16TblId = E_HAL_DISP_VGA_ID_720_25P;
            break;
        case E_HAL_DISP_OUTPUT_720P29D97:
             u16TblId = E_HAL_DISP_VGA_ID_720_29D97P;
            break;
        case E_HAL_DISP_OUTPUT_720P30:
            u16TblId = E_HAL_DISP_VGA_ID_720_30P;
            break;
        case E_HAL_DISP_OUTPUT_720P50:
            u16TblId = E_HAL_DISP_VGA_ID_720_50P;
            break;
        case E_HAL_DISP_OUTPUT_720P59D94:
            u16TblId = E_HAL_DISP_VGA_ID_720_59D94P;
            break;
        case E_HAL_DISP_OUTPUT_720P60:
            u16TblId = E_HAL_DISP_VGA_ID_720_60P;
            break;
        case E_HAL_DISP_OUTPUT_1080P23D98:
            u16TblId = E_HAL_DISP_VGA_ID_1080_23D98P;
            break;
        case E_HAL_DISP_OUTPUT_1080P24:
            u16TblId = E_HAL_DISP_VGA_ID_1080_24P;
            break;
        case E_HAL_DISP_OUTPUT_1080P25:
            u16TblId = E_HAL_DISP_VGA_ID_1080_25P;
            break;
        case E_HAL_DISP_OUTPUT_1080P29D97:
            u16TblId = E_HAL_DISP_VGA_ID_1080_29D97P;
            break;
        case E_HAL_DISP_OUTPUT_1080P30:
            u16TblId = E_HAL_DISP_VGA_ID_1080_30P;
            break;
        case E_HAL_DISP_OUTPUT_1080P50:
            u16TblId = E_HAL_DISP_VGA_ID_1080_50P;
            break;
        case E_HAL_DISP_OUTPUT_1080P59D94:
            u16TblId = E_HAL_DISP_VGA_ID_1080_59D94P;
            break;
        case E_HAL_DISP_OUTPUT_1080P60:
            u16TblId = E_HAL_DISP_VGA_ID_1080_60P;
            break;
        case E_HAL_DISP_OUTPUT_2560X1440_30:
            u16TblId = E_HAL_DISP_VGA_ID_2560X1440_30P;
            break;
        case E_HAL_DISP_OUTPUT_2560X1440_50:
            u16TblId = E_HAL_DISP_VGA_ID_2560X1440_50P;
            break;
        case E_HAL_DISP_OUTPUT_2560X1440_60:
            u16TblId = E_HAL_DISP_VGA_ID_2560X1440_60P;
            break;
        case E_HAL_DISP_OUTPUT_3840X2160_24:
            u16TblId = E_HAL_DISP_VGA_ID_3840X2160_24P;
            break;
        case E_HAL_DISP_OUTPUT_3840X2160_25:
            u16TblId = E_HAL_DISP_VGA_ID_3840X2160_25P;
            break;
        case E_HAL_DISP_OUTPUT_3840X2160_29D97:
            u16TblId = E_HAL_DISP_VGA_ID_3840X2160_29D97P;
            break;
        case E_HAL_DISP_OUTPUT_3840X2160_30:
            u16TblId = E_HAL_DISP_VGA_ID_3840X2160_30P;
            break;
        case E_HAL_DISP_OUTPUT_3840X2160_60:
            u16TblId = E_HAL_DISP_VGA_ID_3840X2160_60P;
            break;
        case E_HAL_DISP_OUTPUT_640X480_60:
            u16TblId = E_HAL_DISP_VGA_ID_640X480_60P;
            break;
        case E_HAL_DISP_OUTPUT_480P60:
            u16TblId = E_HAL_DISP_VGA_ID_480_60P;
            break;
        case E_HAL_DISP_OUTPUT_576P50:
            u16TblId = E_HAL_DISP_VGA_ID_576_50P;
            break;
        case E_HAL_DISP_OUTPUT_800X600_60:
            u16TblId = E_HAL_DISP_VGA_ID_800X600_60P;
            break;
        case E_HAL_DISP_OUTPUT_848X480_60:
            u16TblId = E_HAL_DISP_VGA_ID_848X480_60P;
            break;
        case E_HAL_DISP_OUTPUT_1024X768_60:
            u16TblId = E_HAL_DISP_VGA_ID_1024X768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280X768_60:
            u16TblId = E_HAL_DISP_VGA_ID_1280X768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280X800_60:
            u16TblId = E_HAL_DISP_VGA_ID_1280X800_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280X960_60:
            u16TblId = E_HAL_DISP_VGA_ID_1280X960_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280X1024_60:
            u16TblId = E_HAL_DISP_VGA_ID_1280X1024_60P;
            break;
        case E_HAL_DISP_OUTPUT_1360X768_60:
            u16TblId = E_HAL_DISP_VGA_ID_1360X768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1366X768_60:
            u16TblId = E_HAL_DISP_VGA_ID_1366X768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1400X1050_60:
            u16TblId = E_HAL_DISP_VGA_ID_1400X1050_60P;
            break;
        case E_HAL_DISP_OUTPUT_1440X900_60:
            u16TblId = E_HAL_DISP_VGA_ID_1440X900_60P;
            break;
        case E_HAL_DISP_OUTPUT_1600X900_60:
            u16TblId = E_HAL_DISP_VGA_ID_1600X900_60P;
            break;
        case E_HAL_DISP_OUTPUT_1600X1200_60:
            u16TblId = E_HAL_DISP_VGA_ID_1600X1200_60P;
            break;
        case E_HAL_DISP_OUTPUT_1680X1050_60:
            u16TblId = E_HAL_DISP_VGA_ID_1680X1050_60P;
            break;
        case E_HAL_DISP_OUTPUT_1920X1200_60:
            u16TblId = E_HAL_DISP_VGA_ID_1920X1200_60P;
            break;
        case E_HAL_DISP_OUTPUT_1920X1440_60:
            u16TblId = E_HAL_DISP_VGA_ID_1920X1440_60P;
            break;
        case E_HAL_DISP_OUTPUT_2560X1600_60:
            u16TblId = E_HAL_DISP_VGA_ID_2560X1600_60P;
            break;
        case E_HAL_DISP_OUTPUT_1920X2160_30:
        case E_HAL_DISP_OUTPUT_960H_PAL:
        case E_HAL_DISP_OUTPUT_960H_NTSC:
        case E_HAL_DISP_OUTPUT_1080I50:
        case E_HAL_DISP_OUTPUT_1080I60:
        case E_HAL_DISP_OUTPUT_USER:
        case E_HAL_DISP_OUTPUT_MAX:
        default:
            u16TblId = E_HAL_DISP_VGA_ID_NUM;
            break;
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, TblId:%d\n", __FUNCTION__, __LINE__, u16TblId);
    return u16TblId;
}


static void _HalDispIfSetTgenResetSt(DrvDispCtxConfig_t *pstDispCtxCfg, HalDispDeviceTimingInfo_t *pstDeviceTimingCfg)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8                         u8bSetDacFpll = 0;
    u16                       u16TimeGenStart = 0;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    u16TimeGenStart = HalDispGetDeviceTimeGenStart((void *)pstDispCtxCfg, pstDevContain->u32DevId);
    if(u16TimeGenStart)
    {
        HalDispSetSwReset(1, (void *)pstDispCtxCfg, pstDevContain->u32DevId);
    }
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface) || DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
    {
        HalDispSetFpllEn(HAL_DISP_FPLL_0_VGA_HDMI, 0, (void *)pstDispCtxCfg);
    }
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        HalDispSetDacAffReset(1, (void *)pstDispCtxCfg);
        HalDispSetDacReset(1, (void *)pstDispCtxCfg);
        if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
        {
            u8bSetDacFpll = 1;
            HalDispSetDacPllPowerDown(1, (void *)pstDispCtxCfg);
            HalDispSetVgaHVsyncPad((void *)pstDispCtxCfg, 0);
            g_stInterCfg[pstDevContain->u32DevId].bRstDisp = 1;
        }
        HalDispIrqSetDacEn(pstDevContain->u32DevId, 1);
    }
    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        HalDispSetCvbsOnOff(0, pstDispCtxCfg);
    }
    if (u8bSetDacFpll)
    {
        HalDispSetFpllUsedFlag(HAL_DISP_FPLL_0_VGA_HDMI, pstDevContain->u32DevId, (void *)pstDispCtxCfg);
        HalDispIrqSetFpllEn(HAL_DISP_FPLL_0_VGA_HDMI, 1);
    }
    else
    {
        if (HalDispGetFpllUsedByDev(HAL_DISP_FPLL_0_VGA_HDMI, pstDevContain->u32DevId))
        {
            HalDispSetFpllUsedFlag(HAL_DISP_FPLL_0_VGA_HDMI, HAL_DISP_DEVICE_MAX, (void *)pstDispCtxCfg);
            HalDispIrqSetFpllEn(HAL_DISP_FPLL_0_VGA_HDMI, 0);
        }
    }
}
static void _HalDispIfSetTgenResetEnd(DrvDispCtxConfig_t *pstDispCtxCfg, HalDispDeviceTimingInfo_t *pstDeviceTimingCfg)
{
    DrvDispCtxDeviceContain_t *  pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pCmdqCtx      = NULL;
    u16                       u16TimeGenStart = 0;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    u16TimeGenStart = HalDispGetDeviceTimeGenStart((void *)pstDispCtxCfg, pstDevContain->u32DevId);
    HalDispGetCmdqByCtx((void *)pstDispCtxCfg, (void *)&pCmdqCtx);

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d, Interface:%s(%x)\n", __FUNCTION__, __LINE__,
             pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, PARSING_HAL_INTERFACE(pstDevContain->u32Interface),
             pstDevContain->u32Interface);

    if (pstDevContain->u32Interface & HAL_DISP_INTF_TTL)
    {
        HalDispSetLcdAffReset(1, (void *)pstDispCtxCfg);

        HalDispUtilityWriteDelayCmd(10, (void *)pCmdqCtx);

        HalDispSetLcdAffReset(0, (void *)pstDispCtxCfg);
    }
    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        HalDispSetCvbsOnOff(1, pstDispCtxCfg);
    }
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        HalDispSetFpllGain(HAL_DISP_FPLL_0_VGA_HDMI, 0xFF, pstDispCtxCfg);
        HalDispSetFpllThd(HAL_DISP_FPLL_0_VGA_HDMI, _HalDispIfGetFpllThrd(pstDeviceTimingCfg->stDeviceTimingCfg.u32Dclk),
                          pstDispCtxCfg);
        HalDispSetDacPllPowerDown(0, (void *)pstDispCtxCfg);
        if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
        {
            HalDispSetFpllRst(HAL_DISP_FPLL_0_VGA_HDMI, 1, (void *)pstDispCtxCfg);
            HalDispSetFpllRst(HAL_DISP_FPLL_0_VGA_HDMI, 0, (void *)pstDispCtxCfg);
            HalDispSetFpllEn(HAL_DISP_FPLL_0_VGA_HDMI, 1, (void *)pstDispCtxCfg);
            HalDispSetSwReset(0, (void *)pstDispCtxCfg, pstDevContain->u32DevId);
            HalDispPollFpllLock(HAL_DISP_FPLL_0_VGA_HDMI, (void *)pstDispCtxCfg);
            g_stInterCfg[pstDevContain->u32DevId].bRstDisp = 0;
            //wait for isr to clear
            HalDispSetDacAffReset(1, (void *)pstDispCtxCfg);
        }
        else
        {
            HalDispUtilityWriteDelayCmd(200, pCmdqCtx);
        }
        HalDispSetDacReset(0, pstDispCtxCfg);
        HalDispSetVgaHVsyncPad((void *)pstDispCtxCfg, 1);
        HalDispClearFpllUnLock(HAL_DISP_FPLL_0_VGA_HDMI, (void *)pstDispCtxCfg);
    }
    DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDevHist[pstDevContain->u32DevId].u32ChangeTimingCnt++);
    DISP_STATISTIC_VAL(g_stDispIrqHist.stWorkingHist.stDevHist[pstDevContain->u32DevId].u64ChangeTimingTimeStamp = 
        DrvDispOsGetSystemTimeStamp());
    if(u16TimeGenStart)
    {
        HalDispSetSwReset(0, (void *)pstDispCtxCfg, pstDevContain->u32DevId);
    }
}

static void _HalDispIfSetRegFlipPreAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;
    u16                          u16CmdqInProcess;
    u16                          u16TimeGenStart = 0;
    u32                          u32Dev = HalDispGetDeviceId(pstDispCtxCfg, 0);

    HalDispGetCmdqByCtx((void *)pstDispCtxCfg, (void *)&pstCmdqCtx);

    u16CmdqInProcess = HalDispUtilityR2BYTEMaskDirect(REG_HAL_DISP_UTILIYT_CMDQDEV_IN_PROCESS(pstCmdqCtx->s32UtilityId),
                                                      REG_CMDQ_IN_PROCESS_MSK);
    u16TimeGenStart = HalDispGetDeviceTimeGenStart(pstDispCtxCfg, u32Dev);
    // Add Wait Event if cmdq is idle
    HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16RegFlipCnt, 1);
    HalDispUtilityW2BYTEMSK(REG_HAL_DISP_UTILIYT_CMDQDEV_IN_PROCESS(pstCmdqCtx->s32UtilityId),
                            REG_CMDQ_IN_PROCESS_ON | (pstCmdqCtx->u16RegFlipCnt & REG_CMDQ_PROCESS_FENCE_MSK),
                            REG_CMDQ_IN_PROCESS_MSK | REG_CMDQ_PROCESS_FENCE_MSK, (void *)pstCmdqCtx);

    if (u16CmdqInProcess == 0 && u16TimeGenStart
        && !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->u32Interface & HAL_DISP_INTF_MCU)
        && !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->u32Interface
             & HAL_DISP_INTF_MCU_NOFLM))
    {
        // Polling De Active in first cmd.
        HalDispUtilityPollWait(pstCmdqCtx->pvCmdqInf, HAL_DISP_UTILITY_DISP_TO_CMDQDEV_RAW(pstCmdqCtx->s32UtilityId),
                               HAL_DISP_CMDQIRQ_DEV_OP2VDE_ON_BIT, HAL_DISP_CMDQIRQ_DEV_OP2VDE_ON_MSK, 2000000000, 1);
    }
}

static void _HalDispIfSetRegFlipPostAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;

    HalDispGetCmdqByCtx((void *)pstDispCtxCfg, (void *)&pstCmdqCtx);
    HalDispMopIfSetDdbfWr((void *)pstDispCtxCfg);
    HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
}

static void _HalDispIfSetWaitDonePreAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pstDispCtxCfg, (void *)&pstCmdqCtx);
    HalDispSetCmdqIntClear(HAL_DISP_CMDQIRQ_DEV_VSYNC_ON_MSK, pstDispCtxCfg);
    HalDispSetCmdqIntMask(~HAL_DISP_CMDQIRQ_DEV_VSYNC_ON_MSK, pstDispCtxCfg);
    HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
}

static void _HalDispIfSetWaitDonePostAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pstDispCtxCfg, (void *)&pstCmdqCtx);
    HalDispSetCmdqIntClear(HAL_DISP_CMDQIRQ_DEV_VSYNC_ON_MSK, pstDispCtxCfg);
    HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16WaitDoneCnt, 1);
    HalDispUtilityW2BYTEMSK(REG_HAL_DISP_UTILITY_CMDQ_WAIT_CNT(pstCmdqCtx->s32UtilityId),
                            (pstCmdqCtx->u16WaitDoneCnt << REG_CMDQ_WAIT_CNT_SHIFT), REG_CMDQ_WAIT_CNT_MSK,
                            (void *)pstCmdqCtx);
    HalDispUtilityW2BYTEMSK(REG_HAL_DISP_UTILIYT_CMDQDEV_IN_PROCESS(pstCmdqCtx->s32UtilityId), REG_CMDQ_IN_PROCESS_OFF,
                            REG_CMDQ_IN_PROCESS_MSK, (void *)pstCmdqCtx);
    HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
}
static void _HalDispIfSetDefaultForClearPanelSetting(void *pCtx)
{
    HalDispSetHdmi2OdClkRate(0, pCtx);
    HalDispSetTgenExtHsEn(0, pCtx);
    HalDispSetRgbOrder(pCtx, 3, 2, 1);
    HalDispSetRgbMode(pCtx, E_HAL_DISP_DEVICE_RGB888, 0);
    HalDispSetDacMux(pCtx, 0);
}

static void _HalDispIfSetWaitDonePostActForMcuFrameDone(u32 u32Interface, HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
#if 1
    static u8 u8FrameDonecount = 0;
#endif
    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ
        && pstCmdqCtx->pvCmdqInf)
    {
#if 1
        u8FrameDonecount++;
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_FRAME_DONE_COUNT,
                                          u8FrameDonecount << shift_of_reg_frame_done_count,
                                          mask_of_reg_frame_done_count);
#endif

        // clr lcd frame done flag
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_0D_L, 0x0001, 0x0001);
        // hold disp top rst
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, HAL_DISP_UTILITY_DISP_TOP_RST(pstCmdqCtx->s32UtilityId),
                                          0x0100, 0x0100);
        //
        if (u32Interface & HAL_DISP_INTF_MCU_NOFLM)
        {
            HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_09_L, 0x0200, 0x0200);
        }

#if 0
        //disable dsi gate en
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           HAL_DISP_UTILITY_DISP_DSI_GATE_EN(pstCmdqCtx->s32UtilityId),
                                           0x0000,
                                           0x0010);
        //afifo rst
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           REG_LCD_MISC_30_L,
                                           0x000E,
                                           0x000E);
        //afifo sw rst
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           REG_LCD_MISC_00_L,
                                           0x0001,
                                           0x0001);
        //afifo sw rst  done
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           REG_LCD_MISC_00_L,
                                           0x0000,
                                           0x0001);
        //afifo  rst  done
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           REG_LCD_MISC_30_L,
                                           0x0000,
                                           0x000E);
        //dsi gate en
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           HAL_DISP_UTILITY_DISP_DSI_GATE_EN(pstCmdqCtx->s32UtilityId),
                                           0x0010,
                                           0x0010);
#endif
// poll gpio
// disable lcd frame done irq mask and enable flm irq mask
#if (CMDQ_POLL_EQ_MODE == 0)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_08_L, 0xfdff, 0xffff);
#endif
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                          REG_HAL_DISP_UTILIYT_CMDQDEV_IN_PROCESS(pstCmdqCtx->s32UtilityId),
                                          REG_CMDQ_IN_PROCESS_OFF, REG_CMDQ_IN_PROCESS_MSK);

        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }
}

static void _HalDispIfSetWaitDonePostActForMcuFlm(u32 u32Interface, HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
#if 1
    static u8 u8flmcount = 0;
#endif
    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ
        && pstCmdqCtx->pvCmdqInf)
    {
        // lcd status clear
        if (u32Interface & HAL_DISP_INTF_MCU_NOFLM)
        {
            HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_09_L, 0x0000, 0x0200);
        }
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_0D_L, 0x0200, 0x0200);
// Disp top rst release
#if 1
        u8flmcount++;
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_FLM_COUNT, u8flmcount << shift_of_reg_flm_count,
                                          mask_of_reg_flm_count);
#endif
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, HAL_DISP_UTILITY_DISP_TOP_RST(pstCmdqCtx->s32UtilityId),
                                          0x0000, 0x0100);

        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_MISC_00_L, 0x0001, 0x0001);
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_MISC_00_L, 0x0000, 0x0001);
#if 0
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                           0x103ea8,
                                           0x0,
                                           0x2);
#endif
        // HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
        // HalDispUtilityPollWait((void *)pstCmdqCtx->pvCmdqInf, 0x103ea8,0x0, 0x2, 0, 1);

#if (CMDQ_POLL_EQ_MODE == 0)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_08_L, 0xfffe, 0xffff);
#endif

#if 1
        // dummy register for delay used (TBD)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_DISP_TOP1_30_L, u8flmcount << 8, 0x0F00);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
        // dummy register for delay used (TBD)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_DISP_TOP1_30_L, u8flmcount << 8, 0x0F00);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
        // dummy register for delay used (TBD)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_DISP_TOP1_30_L, u8flmcount << 8, 0x0F00);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
        // dummy register for delay used (TBD)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_DISP_TOP1_30_L, u8flmcount << 8, 0x0F00);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
        // dummy register for delay used (TBD)
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_DISP_TOP1_30_L, u8flmcount << 8, 0x0F00);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
#endif
        // FW refresh
        HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx, REG_LCD_00_L, 0x0042, 0x0042);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }
}

static void _HalDispIfSetCvbsOutputTimeGen(DrvDispCtxConfig_t *pstDispCtxCfg, DrvDispCtxDeviceContain_t *pstDevContain)
{
    u8 u8bAdjust = 0;
    u8bAdjust = g_u8PkSramEn[pstDevContain->u32DevId] ? HAL_DISP_PKSRAM_DELAYCNT : 0;
    if (pstDevContain->eTimeType != E_HAL_DISP_OUTPUT_PAL && pstDevContain->eTimeType != E_HAL_DISP_OUTPUT_NTSC)
    {
        return;
    }
    // Vertical
    HalDispSetTgenVsyncSt(HAL_DISP_CVBS_VS_ST(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenVsyncEnd(HAL_DISP_CVBS_VS_END(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenVfdeSt(HAL_DISP_CVBS_VFDE_ST(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenVfdeEnd(HAL_DISP_CVBS_VFDE_END(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenVdeSt(HAL_DISP_CVBS_VDE_ST(pstDevContain->eTimeType) - u8bAdjust, (void *)pstDispCtxCfg);
    HalDispSetTgenVdeEnd(HAL_DISP_CVBS_VDE_END(pstDevContain->eTimeType) - u8bAdjust, (void *)pstDispCtxCfg);
    HalDispSetTgenVtt(HAL_DISP_CVBS_VTT(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    // horizontal
    HalDispSetTgenHsyncSt(HAL_DISP_CVBS_HS_ST(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenHsyncEnd(HAL_DISP_CVBS_HS_END(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenHfdeSt(HAL_DISP_CVBS_HFDE_ST(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenHfdeEnd(HAL_DISP_CVBS_HFDE_END(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
    HalDispSetTgenHdeSt(HAL_DISP_CVBS_HDE_ST(pstDevContain->eTimeType) - u8bAdjust, (void *)pstDispCtxCfg);
    HalDispSetTgenHdeEnd(HAL_DISP_CVBS_HDE_END(pstDevContain->eTimeType) - u8bAdjust, (void *)pstDispCtxCfg);
    HalDispSetTgenHtt(HAL_DISP_CVBS_HTT(pstDevContain->eTimeType), (void *)pstDispCtxCfg);
}

static void _HalDispIfSetOutputInterfaceHdmiMultiDiv(DrvDispCtxConfig_t *pstDispCtxCfg, u32 u32Mulit, u32 u32Div)
{
    if(u32Div && u32Div <= 32)
    {
        HalDispSetHdmitxAtopDumpByDiv(u32Mulit, u32Div, (void *)pstDispCtxCfg);
    }
    else
    {
        DISP_ERR("%s Div=%d Not Support\n",__FUNCTION__,u32Div);
    }
}
static void _HalDispIfSetOutputInterfaceVgaMultiDiv(DrvDispCtxConfig_t *pstDispCtxCfg, u32 u32Mulit, u32 u32Div)
{
    if(u32Div && u32Mulit && u32Mulit< 64)
    {
        HalDispSetVgaDtopDumpByDiv(u32Mulit, u32Div, (void *)pstDispCtxCfg);
    }
    else
    {
        DISP_ERR("%s Div=%d Mulit=%d Not Support\n",__FUNCTION__,u32Div,u32Mulit);
    }
}
static void _HalDispIfGetSynthDiv(u32 u32Dclk, u32 u32Interface, u32 *p32Div)
{
    u32 u32HwDclk = 0;
    u32 u32idx;
    u32 u32Multi;
    u32 u32VCO;
    u32 u32VCODiff = 0xFFFFFFFF;
    u32 u32PostPllIdx = 0;
    u32 u32DclkToHw = HAL_DISP_DCLKTOHWDCLKMULTIPLE(u32Interface);
    u32 u32VCOMin = HAL_DISP_GET_VCO_MIN(u32Interface);
    u32 u32VCOMax = HAL_DISP_GET_VCO_MAX(u32Interface);
    u32 u32VCORec = HAL_DISP_GET_VCO_RECOMMEND(u32Interface);
    u32 u32PostPllCnt = HAL_DISP_GET_PLLPOSTDIVSELCNT(u32Interface);
    u32HwDclk = (u32Dclk/HAL_DISP_100K) * (u32DclkToHw);
    if((u32HwDclk * (HAL_DISP_GET_PLLPOSTDIVVAL(u32Interface, (u32PostPllCnt-1)))) <= (u32VCOMin*10))
    {
        u32PostPllIdx = (u32PostPllCnt-1);
    }
    else if((u32HwDclk) >= (u32VCOMax*10))
    {
        u32PostPllIdx = 0;
    }
    else
    {
        for(u32idx = 0; u32idx<u32PostPllCnt; u32idx++)
        {
            u32Multi = (HAL_DISP_GET_PLLPOSTDIVVAL(u32Interface, u32idx));
            u32VCO = u32HwDclk * u32Multi;
            if(u32VCO > (u32VCOMin*10) && u32VCO<(u32VCOMax*10))
            {
                if(u32VCO > u32VCORec*10)
                {
                    if((u32VCO - (u32VCORec*10)) < u32VCODiff)
                    {
                        u32VCODiff = u32VCO - (u32VCORec*10);
                        u32PostPllIdx = u32idx;
                    }
                }
                else
                {
                    if(((u32VCORec*10) - u32VCO) < u32VCODiff)
                    {
                        u32VCODiff = (u32VCORec*10) - u32VCO;
                        u32PostPllIdx = u32idx;
                    }
                }
            }
        }
    }
    *p32Div = HAL_DISP_GET_PLLPOSTDIVVAL(u32Interface, u32PostPllIdx);
    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, %s Div(%d) VCO(%d) PostPllIdx(%d), u32Dclk(%u)\n",__FUNCTION__,__LINE__,PARSING_HAL_INTERFACE(u32Interface),
             *p32Div,(u32HwDclk*HAL_DISP_GET_PLLPOSTDIVVAL(u32Interface, u32PostPllIdx)/10),u32PostPllIdx,u32Dclk);
}
static void _HalDispIfGetSynthMulti(u32 u32Dclk, u32 u32Interface, u32 *p32Multi, u32 u32Div)
{
    u32 u32LoopDiv = 0;
    u32 u32idx = 0;
    u32 u32Synth = 0;
    u64 nDividend;
    u64 nDivisor;
    u64 u64Remainder;
    u32idx = ((HAL_DISP_DCLKTOHWDCLKMULTIPLE(u32Interface)));
    if(u32Dclk % 81 == 0)
    {
        u32idx *= 3;
    }
    else
    {
        nDividend = HAL_DISP_BASIC_CLK_DIV;
        nDivisor = (u64)((u64)u32Dclk*u32Div);
        u32Synth = DrvDispOsMathDivU64(nDividend, nDivisor, &u64Remainder);
        if(u32Synth < 0x80000)
        {
            u32idx *= 2;
        }
    }
    u32LoopDiv = HAL_DISP_GET_PLLLOOPDIVVAL(u32Interface, u32idx);
    *p32Multi = u32LoopDiv/(HAL_DISP_DCLKTOHWDCLKMULTIPLE(u32Interface));
    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, %s Div(%d) LoopDiv(%d) Multi(%d), u32Dclk(%u)\n",__FUNCTION__,__LINE__,PARSING_HAL_INTERFACE(u32Interface),
             u32Div,u32LoopDiv,*p32Multi,u32Dclk);
}
static void _HalDispIfGetOutputInterfaceSynthMultiDiv(u32 u32Dclk, u32 u32Interface, u32 *p32Mulit, u32 *p32Div)
{
    _HalDispIfGetSynthDiv(u32Dclk, u32Interface, p32Div);
    if(*p32Div)
    {
        _HalDispIfGetSynthMulti(u32Dclk, u32Interface, p32Mulit ,*p32Div);
    }
}
static u32 _HalDispIfGetOutputInterfaceSynth(HalDispDeviceTimingConfig_t *pstDeviceTimingCfg, u32 u32Interface)
{
    u32 u32Dclk = 0;
    u32 u32Div = 0;
    u32 u32Mulit = 0;
    u32 u32Synth = 0;
    u64 nDividend;
    u64 nDivisor;
    u64 u64Remainder;
    u32Dclk = pstDeviceTimingCfg->u32Dclk;
    _HalDispIfGetOutputInterfaceSynthMultiDiv(u32Dclk, u32Interface, &u32Mulit, &u32Div);
    if(u32Mulit && u32Div)
    {
        nDividend = (u64)(HAL_DISP_BASIC_CLK_DIV *u32Mulit);
        nDivisor = (u64)((u64)u32Dclk*u32Div);
        u32Synth = DrvDispOsMathDivU64(nDividend, nDivisor, &u64Remainder);
        if(u64Remainder * 2 >= nDivisor)
        {
            u32Synth++;
        }
    }
    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, Multi(%d), Div(%d) %s(%d), Synth(0x%x) Dclk(%u)\n",__FUNCTION__,__LINE__,
             u32Mulit,u32Div,PARSING_HAL_INTERFACE(u32Interface),u32Interface,u32Synth,u32Dclk);
    return u32Synth;
}
static void _HalDispIfGetOutputInterfaceClk(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *  pstDevContain      = NULL;
    HalDispDeviceTimingConfig_t *pstDeviceTimingCfg = NULL;
    pstDevContain      = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDeviceTimingCfg = &(pstDevContain->stDevTimingCfg);
    if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
    {
        pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_HDMI] =
            _HalDispIfGetOutputInterfaceSynth(pstDeviceTimingCfg, HAL_DISP_INTF_HDMI);
    }
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_VGA] =
            _HalDispIfGetOutputInterfaceSynth(pstDeviceTimingCfg, HAL_DISP_INTF_VGA);
    }
    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_CVBS] =
            _HalDispIfGetOutputInterfaceSynth(pstDeviceTimingCfg, HAL_DISP_INTF_CVBS);
    }
}
static void _HalDispIfSetOutputInterfaceClk(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *  pstDevContain      = NULL;
    HalDispDeviceTimingConfig_t *pstDeviceTimingCfg = NULL;
    u32 u32Dclk = 0;
    u32 u32Div = 0;
    u32 u32Mulit = 0;
    u32 u32Synth = 0;
    pstDevContain      = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDeviceTimingCfg = &pstDevContain->stDevTimingCfg;
    u32Dclk = pstDeviceTimingCfg->u32Dclk;
#ifdef HAL_HDMITX_TOP_TBL_BYSELF 
    if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
    {
        _HalDispIfGetOutputInterfaceSynthMultiDiv(u32Dclk, HAL_DISP_INTF_HDMI, &u32Mulit, &u32Div);
        u32Synth = pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_HDMI];
        _HalDispIfSetOutputInterfaceHdmiMultiDiv(pstDispCtxCfg, u32Mulit, u32Div);
        HalDispSetHdmitxSynth(u32Synth, (void *)pstDispCtxCfg);
    }
#endif
#ifdef HAL_DISP_VGA_TIMING_TBL_BYSELF 
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        _HalDispIfGetOutputInterfaceSynthMultiDiv(u32Dclk, HAL_DISP_INTF_VGA, &u32Mulit, &u32Div);
        u32Synth = pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_VGA];
        _HalDispIfSetOutputInterfaceVgaMultiDiv(pstDispCtxCfg, u32Mulit, u32Div);
        HalDispSetVgaSynth(u32Synth, (void *)pstDispCtxCfg);
    }
#endif
    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        _HalDispIfGetOutputInterfaceSynthMultiDiv(u32Dclk, HAL_DISP_INTF_CVBS, &u32Mulit, &u32Div);
        u32Synth = pstDevContain->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_CVBS];
        HalDispDmaSetTvencSynthSet(u32Synth, (void *)pstDispCtxCfg);
    }
    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, Multi(%d), Div(%d) %s(%d), Synth(0x%x)\n",__FUNCTION__,__LINE__,
             u32Mulit,u32Div,PARSING_HAL_INTERFACE(pstDevContain->u32Interface),pstDevContain->u32Interface,u32Synth);
}
static void _HalDispIfSetTgenConfig(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *  pstDevContain      = NULL;
    HalDispDeviceTimingConfig_t *pstDeviceTimingCfg = NULL;
    u16                          u16HsyncSt, u16VsyncSt;
    u16                          u16HdeSt, u16VdeSt;
    u8 u8bAdjust = 0;

    pstDevContain      = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDeviceTimingCfg = &pstDevContain->stDevTimingCfg;
    u8bAdjust = g_u8PkSramEn[pstDevContain->u32DevId] ? HAL_DISP_PKSRAM_DELAYCNT : 0;
    u16HdeSt = pstDeviceTimingCfg->u16Hstart;
    u16VdeSt = pstDeviceTimingCfg->u16Vstart;

    u16HsyncSt =
        (u16HdeSt)
            ? pstDeviceTimingCfg->u16Hstart - pstDeviceTimingCfg->u16HsyncWidth - pstDeviceTimingCfg->u16HsyncBackPorch
            : pstDeviceTimingCfg->u16Htotal - pstDeviceTimingCfg->u16HsyncWidth - pstDeviceTimingCfg->u16HsyncBackPorch;

    u16VsyncSt =
        (u16VdeSt)
            ? pstDeviceTimingCfg->u16Vstart - pstDeviceTimingCfg->u16VsyncWidth - pstDeviceTimingCfg->u16VsyncBackPorch
            : pstDeviceTimingCfg->u16Vtotal - pstDeviceTimingCfg->u16VsyncWidth - pstDeviceTimingCfg->u16VsyncBackPorch;
    if (pstDevContain->u32Interface & HAL_DISP_INTF_MIPIDSI)
    {
        u16HdeSt = u16HdeSt > 58 ? u16HdeSt - 58 : u16HdeSt;
        u16VdeSt++;
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, Hsync(%d %d), Vsync(%d %d) SyncStart(%d %d), DeStart(%d, %d) Size(%d %d), Total(%d %d)\n",
             __FUNCTION__, __LINE__, pstDeviceTimingCfg->u16HsyncWidth, pstDeviceTimingCfg->u16HsyncBackPorch,
             pstDeviceTimingCfg->u16VsyncWidth, pstDeviceTimingCfg->u16VsyncBackPorch, u16HsyncSt, u16VsyncSt, u16HdeSt,
             u16VdeSt, pstDeviceTimingCfg->u16Hactive, pstDeviceTimingCfg->u16Vactive, pstDeviceTimingCfg->u16Htotal,
             pstDeviceTimingCfg->u16Vtotal);

    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        _HalDispIfSetCvbsOutputTimeGen(pstDispCtxCfg, pstDevContain);
    }
    else
    {
        // Vertical
        HalDispSetTgenVsyncSt(u16VsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVsyncEnd(u16VsyncSt + pstDeviceTimingCfg->u16VsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeSt(u16VdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVdeSt(u16VdeSt - u8bAdjust, (void *)pstDispCtxCfg);
        HalDispSetTgenVdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1 - u8bAdjust, (void *)pstDispCtxCfg);
        HalDispSetTgenVtt(pstDeviceTimingCfg->u16Vtotal - 1, (void *)pstDispCtxCfg);

        // horizontal
        HalDispSetTgenHsyncSt(u16HsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHsyncEnd(u16HsyncSt + pstDeviceTimingCfg->u16HsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHfdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeSt(u16HdeSt - u8bAdjust, (void *)pstDispCtxCfg);
        HalDispSetTgenHfdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1 - u8bAdjust, (void *)pstDispCtxCfg);
        HalDispSetTgenHtt(pstDeviceTimingCfg->u16Htotal - 1, (void *)pstDispCtxCfg);
    }

    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        HalDispSetTgenDacHsyncSt(u16HsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHsyncEnd(u16HsyncSt + pstDeviceTimingCfg->u16HsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenDacVdeSt(u16VdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacVdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1, (void *)pstDispCtxCfg);
    }
}

static void _HalDispIfSetInitWithoutCmdq(DrvDispCtxConfig_t *pstDispCtx)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u32                        u32OrigAccessMode;
    s32                        s32UtilityId;

    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    if (HalDispGetUtilityIdByDevId(pstDevContain->u32DevId, &s32UtilityId)
        && ((s32UtilityId >= 0) && (s32UtilityId < HAL_DISP_UTILITY_CMDQ_NUM)))
    {
        u32OrigAccessMode = HalDispUtilityGetRegAccessMode((u32)s32UtilityId);
        HalDispUtilitySetRegAccessMode(s32UtilityId, E_DISP_UTILITY_REG_ACCESS_CPU);
        HalDispSetCmdqIntMask(~HAL_DISP_CMDQIRQ_DEV_OP2VDE_OFF_MSK, (void *)pstDispCtx); //
        HalDispSetSwReset(1, NULL, pstDevContain->u32DevId);
        HalDispSetMaceSrcSel(0x0001, (void *)pstDispCtx, pstDevContain->u32DevId);       // mace src from mop
        HalDispIrqSetDevIntClr(pstDevContain->u32DevId, 0xFFFF);
        HalDispSetTimeGenStartFlag(0, NULL, pstDevContain->u32DevId);
        HalDispUtilitySetRegAccessMode((u32)s32UtilityId, u32OrigAccessMode);
        g_stInterCfg[pstDevContain->u32DevId].bDispPat = 0;
        g_stInterCfg[pstDevContain->u32DevId].bRstDisp = 1;
        g_stDispIrqHist.stWorkingStatus.stDevStatus[pstDevContain->u32DevId].u8Deviceused = 0;
    }
    else
    {
        DISP_ERR("%s %d, GetCmdqIdByDevId Fail\n", __FUNCTION__, __LINE__);
    }
}
#if DISP_TIMEZONE_ISR_SUPPORT
static HalDispTimeZoneType_e _HalDispIfGetTimeZoneType(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *  pstDeviceContain = NULL;
    HalDispIrqTimeZoneStatus_t   stTimeZoneStatus;
    HalDispTimeZoneType_e        enTimeZoneType;
    HalDispDeviceTimingConfig_t *pstTimeCfg;
    u64                          u64TimeDiff, u64PeriodTime;
    HalDispIrqIoctlConfig_t      stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_TIMEZONE;
    stIrqIoctlCfg.pDispCtx    = (void *)pstDispCtxCfg;
    stIrqIoctlCfg.pParam      = (void *)&stTimeZoneStatus;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstTimeCfg       = &pstDeviceContain->stDevTimingCfg;

    if (stTimeZoneStatus.u32IrqFlags & E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        u64TimeDiff = DrvDispOsGetSystemTimeStamp()
                      - stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE];
        u64PeriodTime  = pstTimeCfg->u32VFrontPorchPeriod;
        enTimeZoneType = E_HAL_DISP_TIMEZONE_FRONTPORCH;
    }
    else
    {
        u64TimeDiff    = 0;
        u64PeriodTime  = 0;
        enTimeZoneType = E_HAL_DISP_TIMEZONE_NONE;
    }

    DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, IrqFlag=%x Zone:%-12s T(%5lld, %5lld), (%10lld %10lld %10lld)\n",
             __FUNCTION__, __LINE__, stTimeZoneStatus.u32IrqFlags, PARSING_HAL_TIMEZONE(enTimeZoneType), u64PeriodTime,
             u64TimeDiff, stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE],
             stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE],
             stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE]);
    DISP_DBG_VAL(u64PeriodTime);
    DISP_DBG_VAL(u64TimeDiff);
    return enTimeZoneType;
}
#endif
static void _HalDispIfDumpHdmitxTbl(void *pCtx, u16 u16TblId)
{
    HAL_HDMITX_INFO *  pstTblInfo    = NULL;
    void *pCmdqCtx = NULL;
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF
    HAL_DISP_VGA_INFO *pstVgaTblInfo = NULL;
#endif

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    if (u16TblId == E_HAL_HDMITX_ID_NUM)
    {
        DISP_DBG(DISP_DBG_LEVEL_HAL,"%s %d, TblId (%d) is not correct, maybe is user\n", __FUNCTION__, __LINE__, u16TblId);
    }
    DISP_DBG(DISP_DBG_LEVEL_IO,"%s %d E_HAL_HDMITX_TAB_NUM=%d\n", __FUNCTION__, __LINE__,E_HAL_HDMITX_TAB_NUM);
    pstTblInfo = &g_sthalhdmitxtoptbl[E_HAL_HDMITX_COMMON_TAB_INIT_HDMITX_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, 1, 0);
    if(u16TblId<E_HAL_HDMITX_ID_NUM)
    {
#ifndef HAL_HDMITX_TOP_TBL_BYSELF 
    pstTblInfo = &g_sthalhdmitxtoptbl[E_HAL_HDMITX_TAB_INIT_HDMITX_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, E_HAL_HDMITX_ID_NUM, u16TblId);
        pstVgaTblInfo = &g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_TAB_SYNTH_SET_HDMITX];
        HalDispDumpRegTab(pCtx, pstVgaTblInfo->pData, pstVgaTblInfo->u16RegNum, E_HAL_DISP_VGA_ID_NUM, u16TblId);
#endif
    }
    HalDispUtilityWriteDelayCmd(2000, pCmdqCtx);
}

static void _HalDispIfDumpIdacTbl(void *pCtx, u16 u16TblId)
{
    HAL_DISP_VGA_INFO *pstTblInfo = NULL;
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);
    if (u16TblId == E_HAL_DISP_VGA_ID_NUM)
    {
        DISP_DBG(DISP_DBG_LEVEL_HAL,"%s %d, TblId (%d) is not correct, maybe is user\n", __FUNCTION__, __LINE__, u16TblId);
    }
    DISP_DBG(DISP_DBG_LEVEL_IO,"%s %d E_HAL_DISP_VGA_TAB_NUM=%d\n", __FUNCTION__, __LINE__,E_HAL_DISP_VGA_TAB_NUM);
    if(u16TblId<E_HAL_DISP_VGA_ID_NUM)
    {
#ifndef HAL_DISP_VGA_TIMING_TBL_BYSELF
    pstTblInfo = &g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_TAB_SYNTH_SET_VGA];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, E_HAL_DISP_VGA_ID_NUM, u16TblId);
    HalDispUtilityWriteDelayCmd(2000, pCmdqCtx);
    pstTblInfo = &g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_TAB_IDAC_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, E_HAL_DISP_VGA_ID_NUM, u16TblId);
    HalDispUtilityWriteDelayCmd(2000, pCmdqCtx);
#endif
    }
    pstTblInfo = &g_sthaldispvgatimingtbl[E_HAL_DISP_VGA_COMMON_TAB_IDAC_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, 1, 0);
    HalDispUtilityWriteDelayCmd(2000, pCmdqCtx);
}

static void _HalDispIfDumpCvbsTbl(void *pCtx, u16 u16TblId)
{
    HAL_DISP_CVBS_INFO *pstTblInfo = NULL;
    u8                  i;
    void *pCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, &pCmdqCtx);

    DISP_DBG(DISP_DBG_LEVEL_IO,"%s %d E_HAL_DISP_CVBS_TAB_NUM=%d\n", __FUNCTION__, __LINE__,E_HAL_DISP_CVBS_TAB_NUM);
    for (i = 0; i < E_HAL_DISP_CVBS_TAB_NUM; i++)
    {
        pstTblInfo = &g_sthaldispcvbstimingtbl[i];
        if (pstTblInfo->enIPType == E_HAL_DISP_CVBS_IP_COMMON)
        {
            HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, 1, 0);
        }
#ifndef HAL_DISP_CVBS_TIMING_TBL_BYSELF 
        else
        {
            if (u16TblId < E_HAL_DISP_CVBS_ID_NUM)
            {
            HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, E_HAL_DISP_CVBS_ID_NUM, u16TblId);
            }
        }
#endif 
        HalDispUtilityWriteDelayCmd(2000, pCmdqCtx);
    }
}
static void _HalDispIfDumpOutputInterfaceTbl(void *pCtx, u16 u16TblId)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];

    _HalDispIfSetOutputInterfaceClk(pCtx);
    if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
    { // if dst has hdmi,hdmi timing is main timing.
        _HalDispIfDumpHdmitxTbl(pCtx, u16TblId);
    }
    if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        _HalDispIfDumpIdacTbl(pCtx, u16TblId);
    }
    if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        _HalDispIfDumpCvbsTbl(pCtx, u16TblId);
    }
}

static void _HalDispIfGetDefautDacTrim(u16 *pu16Trim, void *pCtx)
{
    if (pu16Trim[0] == 0 && pu16Trim[1] == 0 && pu16Trim[2] == 0)
    {
        HalDispGetDacTriming(&pu16Trim[0], &pu16Trim[1], &pu16Trim[2], pCtx);
    }
}

static void _HalDispIfSetDacTrim(s16 *ps16Trim, s16 s16Val, void *pCtx)
{
    u16 i;

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Trim(0x%02x 0x%02x 0x%02x), Val:%d\n", __FUNCTION__, __LINE__, ps16Trim[0],
             ps16Trim[1], ps16Trim[2], s16Val);

    for (i = 0; i < 3; i++)
    {
        if (ps16Trim[i] & 0x40)
        {
            ps16Trim[i] = ps16Trim[i] & 0x3F;
            ps16Trim[i]++;
            ps16Trim[i] = -ps16Trim[i];
        }

        ps16Trim[i] = ((ps16Trim[i] + s16Val) > 63)    ? 63
                      : ((ps16Trim[i] + s16Val) < -64) ? -64
                                                       : (ps16Trim[i] + s16Val);
        if (ps16Trim[i] < 0)
        {
            ps16Trim[i] = -ps16Trim[i];
            ps16Trim[i]--;
            ps16Trim[i] |= 0x40;
        }
    }

    HalDispSetDacTrimming(ps16Trim[0], ps16Trim[1], ps16Trim[2], pCtx);

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Trim(0x%02x 0x%02x 0x%02x)\n", __FUNCTION__, __LINE__, ps16Trim[0],
             ps16Trim[1], ps16Trim[2]);
}
static void _HalDispIfGetPictureDataFromPq(void *pCtx, HalDispPicturePqConfig_t *pstPqPictureCfg)
{
    u8 au8Data[32];
    u8 i;

    HalDispHpqLoadData(pCtx, au8Data, 32);

    pstPqPictureCfg->bUpdate  = 1;
    pstPqPictureCfg->u16GainR = au8Data[0];
    pstPqPictureCfg->u16GainG = au8Data[1];
    pstPqPictureCfg->u16GainB = au8Data[2];

    pstPqPictureCfg->u16BrightnessR = au8Data[3];
    pstPqPictureCfg->u16BrightnessG = au8Data[4];
    pstPqPictureCfg->u16BrightnessB = au8Data[5];

    pstPqPictureCfg->u16Hue        = au8Data[6];
    pstPqPictureCfg->u16Saturation = au8Data[7];
    pstPqPictureCfg->u16Contrast   = au8Data[8];
    pstPqPictureCfg->u16Sharpness  = au8Data[9];
    pstPqPictureCfg->u16Sharpness1 = au8Data[10];

    for (i = 0; i < 9; i++)
    {
        pstPqPictureCfg->as16Coef[i] = (((s16)au8Data[i * 2 + 12]) << 8) | (s16)au8Data[i * 2 + 11];
        if (pstPqPictureCfg->as16Coef[i] & 0x1000)
        {
            pstPqPictureCfg->as16Coef[i] = -(pstPqPictureCfg->as16Coef[i] & 0xFFF);
        }
    }
}
static void _HalDispIfSetCscToDefualt(u32 u32ColorId, void *pCtx)
{
    HalDispColorSeletYuvToRgbMatrix(u32ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_BYPASS, NULL, pCtx);
    HalDispColorAdjustVideoRGB(u32ColorId, 0x80, 0x80, 0x80, pCtx);
    HalDispColorAdjustHCS(u32ColorId, 50, 0x80, 0x80, pCtx);
    HalDispColorAdjustBrightness(u32ColorId, 0x80, 0x80, 0x80, pCtx);
}
static void _HalDispIfSetCscToInit(u32 u32ColorId, HalDispCsc_t *pstCsc, void *pCtx)
{
    HalDispPictureConfig_t stPictureCfg;

    HalDispColorSeletYuvToRgbMatrix(u32ColorId, E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV, NULL, pCtx);
    HalDispPictureIfTransNonLinear(pCtx, pstCsc, NULL, &stPictureCfg);

    HalDispColorAdjustHCS(u32ColorId, stPictureCfg.u16Hue, stPictureCfg.u16Saturation, stPictureCfg.u16Contrast, pCtx);

    HalDispColorAdjustBrightness(u32ColorId, stPictureCfg.u16BrightnessR, stPictureCfg.u16BrightnessG,
                                 stPictureCfg.u16BrightnessB, pCtx);
}
static void _HalDispIfSetInterfaceCsc(void *pCtx, u32 u32DevId, HalDispCscmatrix_e enMatrix)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg = pCtx;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    DISP_DBG(DISP_DBG_LEVEL_COLOR,"%s,Dev:%d pCtx:%px Matrix:%d ",
        __FUNCTION__, u32DevId, pCtx, enMatrix);
    if(!(pstDispCtxCfg) || !(pstDispCtxCfg->pstCtxContain))
    {
        DrvDispCtxGetCurCtx(E_DISP_CTX_TYPE_DEVICE, u32DevId, &pstDispCtxCfg);
    }
    if(pstDispCtxCfg && pstDispCtxCfg->pstCtxContain)
    {
        if(pstDispCtxCfg->pstCtxContain->bDevContainUsed[pstDispCtxCfg->u32ContainIdx])
        {
            pstDevContain        = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
        }
        if(pstDevContain)
        {
            DISP_DBG(DISP_DBG_LEVEL_COLOR,"%s,Dev:%d Interface:%d Matrix:%d ",
                __FUNCTION__, u32DevId, pstDevContain->u32Interface, enMatrix);
            pstDevContain->stDeviceParam.stCsc.eCscMatrix = enMatrix;
        }
    }
}
static void _HalDispIfPqSetBypass(void *pCtx, u8 u8bBypass, u32 u32Dev, HalDispUtilityRegAccessMode_e enRiuMode)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = pCtx;

    DISP_DBG(DISP_DBG_LEVEL_COLOR,"%s,Dev:%d pCtx:%px bBypass=%d",
        __FUNCTION__, u32Dev, pCtx, u8bBypass);
    HalDispHpqSetBypass(pCtx, u8bBypass, u32Dev, enRiuMode);
    if (u8bBypass)
    {
        HalDispMaceSetBypass(pCtx, u8bBypass, u32Dev, enRiuMode);
        HalDispOp2SetBypass(pCtx, u8bBypass, u32Dev, enRiuMode);
        HalDispGammaSetEn(pCtx, 0, u32Dev, enRiuMode);
            DrvDispCtxGetCurCtx(E_DISP_CTX_TYPE_DEVICE, u32Dev, &pstDispCtxCfg);
        DISP_DBG(DISP_DBG_LEVEL_COLOR,"%s,Dev:%d pCtx:%px GetDeviceId=%d",
            __FUNCTION__, u32Dev, pstDispCtxCfg, HalDispGetDeviceId(pstDispCtxCfg, 0));
            if (pstDispCtxCfg && pstDispCtxCfg->pstCtxContain
                && u32Dev == HalDispGetDeviceId(pstDispCtxCfg, 0))
            {
                _HalDispIfSetCscToInit(
                    g_stInterCfg[u32Dev].u8ColorId,
                &pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->stDeviceParam.stCsc, pstDispCtxCfg);
            _HalDispIfSetInterfaceCsc((void *)pstDispCtxCfg, u32Dev, E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC);
            g_stInterCfg[u32Dev].u8CsCMd = E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV;
        }
    }
}

static void _HalDispifClkGet(HalDispClkType_e enType, HalDispClkConfig_t *pstClkCfg)
{
    u8 * p8En                             = NULL;
    u32 *p32Rate                          = NULL;
    u8   u8ClkMuxAttr[E_HAL_DISP_CLK_NUM] = HAL_DISP_CLK_MUX_ATTR;

    if (pstClkCfg)
    {
        p8En    = &pstClkCfg->bEn[enType];
        p32Rate = &pstClkCfg->u32Rate[enType];
    }
    else
    {
        DISP_ERR("%s %d, pstClkCfg is NULL ,enType:%d\n", __FUNCTION__, __LINE__, enType);
        return;
    }
    HalDispClkGetBasicClkgen(enType, p8En, p32Rate);
    if (u8ClkMuxAttr[enType] == 0)
    {
        *p32Rate = HalDispClkMapBasicClkgenToRate(enType, *p32Rate);
    }
}
static void _HalDispifClkSet(HalDispClkType_e enType, HalDispClkConfig_t *pstClkCfg)
{
    u8  u8ClkMuxAttr[E_HAL_DISP_CLK_NUM] = HAL_DISP_CLK_MUX_ATTR;
    u32 u32ClkIdx = 0;
    u8  u8En = 0;

    if (pstClkCfg)
    {
        u8En = pstClkCfg->bEn[enType];
        u32ClkIdx = u8ClkMuxAttr[enType] ? pstClkCfg->u32Rate[enType] :
            HalDispClkMapBasicClkgenToIdx(enType, pstClkCfg->u32Rate[enType]);
    }
    HalDispClkSetBasicClkgen(enType, u8En, u32ClkIdx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *                     pvCmdqCtx     = NULL;
    s32                        u32UtilityId;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];

    if (HalDispGetUtilityIdByDevId(pstDevContain->u32DevId, &u32UtilityId) == 0)
    {
        DISP_ERR("%s %d, UnSupport Cmdq Id\n", __FUNCTION__, __LINE__);
    }
    else
    {
        if (HalDispUtilityInit((u32)u32UtilityId) == 0)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, ContainId:%d, DevId:%d, Utility Init Fail\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                     pstDevContain->u32DevId);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                     pstDevContain->u32DevId);

            if (HalDispUtilityGetCmdqContext(&pvCmdqCtx, u32UtilityId) == 0)
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
            }
            else
            {
                if (pvCmdqCtx)
                {
                    pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[u32UtilityId] = pvCmdqCtx;

                    pstDevContain->stDeviceParam.u32Gain             = 50;


                    pstDevContain->stDeviceParam.stCsc.u32Contrast   = 50;
                    pstDevContain->stDeviceParam.stCsc.u32Hue        = 50;
                    pstDevContain->stDeviceParam.stCsc.u32Saturation = 50;
                    pstDevContain->stDeviceParam.stCsc.u32Luma       = 50;
                    pstDevContain->stDeviceParam.stCsc.eCscMatrix    = E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC;
                    pstDevContain->stDeviceParam.u32Sharpness        = 50;
                    
                    pstDevContain->u32BgColor = 0;
                    HalDispClkInitGpCtrlCfg(pCtx);
                }
                else
                {
                    enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                    DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
                }
            }
        }
    }

    return enRet;
}

static void _HalDispIfSetDeviceInit(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *         pstDispCtx    = NULL;
    DrvDispCtxDeviceContain_t *  pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;

    pstDispCtx    = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx);

    if (pstCmdqCtx)
    {
        // Init Setting
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Dev:%d, ConId:%d\n", __FUNCTION__, __LINE__, pstDevContain->u32DevId, pstDispCtx->u32ContainIdx);
        _HalDispIfSetInitWithoutCmdq(pstDispCtx);
        HalDispSetFrameColor(0x80, 0x00, 0x80, pCtx);
        HalDispSetDispWinColor(0x80, 0x00, 0x80, pCtx);

        HalDispSetDacRgbHpdInit();

        HalDispColorInitVar(pCtx);
        HalDispColorSetColorCorrectMatrix(E_DISP_COLOR_CSC_ID_0, g_stVideoColorCorrectionMatrix, pCtx);
        HalDispColorSetColorCorrectMatrix(E_DISP_COLOR_CSC_ID_1, g_stVideoColorCorrectionMatrix, pCtx);
        _HalDispIfSetCscToDefualt(E_DISP_COLOR_CSC_ID_0, pCtx);
        _HalDispIfSetCscToDefualt(E_DISP_COLOR_CSC_ID_1, pCtx);
        g_stInterCfg[pstDevContain->u32DevId].bCsCEn  = 1;
        g_stInterCfg[pstDevContain->u32DevId].u8CsCMd = (u8)E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV;
        if (HalDispPictureIfIsPqUpdate(pstDevContain->u32DevId))
        {
            HalDispPictureIfApplyPqConfig(pCtx);
        }
        else
        {
            _HalDispIfSetCscToInit(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, &pstDevContain->stDeviceParam.stCsc,
                                   pCtx);
        }

        _HalDispIfGetDefautDacTrim(g_a16VgaDacTrimDefault, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Null Cmdq Ctx\n", __FUNCTION__, __LINE__);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceDeInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    s32                        s32UtilityId;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];

    HalDispClkDeInitGpCtrlCfg(pCtx);

    if (HalDispGetUtilityIdByDevId(pstDevContain->u32DevId, &s32UtilityId))
    {
        if (HalDispUtilityDeInit(s32UtilityId) == 0)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, ContainId:%d, DevId:%d, Utility DeInit Fail\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                     pstDevContain->u32DevId);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                     pstDevContain->u32DevId);

            pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[s32UtilityId] = NULL;
        }
    }
    else
    {
        DISP_ERR("%s %d, Get Cmdq Id Fail\n", __FUNCTION__, __LINE__);
    }
    return enRet;
}

static void _HalDispIfSetDeviceDeInit(void *pCtx, void *pCfg)
{
    u32 u32DevId = 0;
    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d\n", __FUNCTION__, __LINE__);
    HalDispSetDacPllPowerDown(1, NULL);
    HalDispIrqSetDacEn(HAL_DISP_DEVICE_MAX, 0);
    HalDispClkSetHdmiPowerOnOff(0);
    HalDispIrqSetFpllEn(HAL_DISP_FPLL_0_VGA_HDMI, 0);
    HalDispIrqSetFpllEn(HAL_DISP_FPLL_1_CVBS_DAC, 0);
    for(u32DevId = 0;u32DevId<HAL_DISP_DEVICE_MAX;u32DevId++)
    {
        if(g_stDispIrqHist.stWorkingStatus.stDevStatus[u32DevId].u8Deviceused)
        {
            HalDispSetSwReset(1, NULL, u32DevId);
            g_stDispIrqHist.stWorkingStatus.stDevStatus[u32DevId].u8Deviceused = 0;
        }
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    u8 *                       pbEn;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pbEn          = (u8 *)pCfg;

    pstDevContain->bEnable = *pbEn;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d, Enable:%x\n", __FUNCTION__, __LINE__,
             pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, pstDevContain->bEnable);

    return enRet;
}

static void _HalDispIfSetDeviceEnable(void *pCtx, void *pCfg)
{
    u8                         bEn;
    DrvDispCtxConfig_t *       pstDispCtx    = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtx    = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
    bEn           = *(u8 *)pCfg;

    g_stInterCfg[pstDevContain->u32DevId].bRstDisp = bEn ? 0 : 1;
    g_stDispIrqHist.stWorkingStatus.stDevStatus[pstDevContain->u32DevId].u8Deviceused = bEn;
    if (bEn == 0)
    {
        HalDispSetTimeGenStartFlag(0, pCtx, pstDevContain->u32DevId);
        if(pstDevContain->bMergeCase && pstDevContain->u32DevId== HAL_DISP_DEVICE_ID_1)
        {
            HalDispSetSwReset(1, pCtx, HAL_DISP_DEVICE_ID_0);
            pstDevContain->bMergeCase = 0;
        }
    }
    HalDispSetSwReset(bEn ? 0 : 1, pCtx, pstDevContain->u32DevId);
    HalDispSetDispWinColorForce(bEn ? 0 : 1, pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceAttach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e   enRet            = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfgDst = NULL;
    DrvDispCtxConfig_t *pstDispCtxCfgSrc = NULL;

    pstDispCtxCfgSrc = (DrvDispCtxConfig_t *)pCtx;
    pstDispCtxCfgDst = (DrvDispCtxConfig_t *)pCfg;

    if (pstDispCtxCfgSrc && pstDispCtxCfgDst)
    {
        pstDispCtxCfgDst->pstCtxContain->pstDevContain[pstDispCtxCfgDst->u32ContainIdx]->pstDevAttachSrc =
            (void *)pstDispCtxCfgSrc->pstCtxContain->pstDevContain[pstDispCtxCfgSrc->u32ContainIdx];

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, SRC(ContainId:%d, DevId:%d), DST(ContainId:%d, DevId:%d)\n", __FUNCTION__,
                 __LINE__, pstDispCtxCfgSrc->u32ContainIdx,
                 HalDispGetDeviceId(pstDispCtxCfgSrc, 0),
                 pstDispCtxCfgDst->u32ContainIdx,
                 HalDispGetDeviceId(pstDispCtxCfgDst, 0));
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, SrcCtx:%px, DstCtx:%px\n", __FUNCTION__, __LINE__, pstDispCtxCfgSrc, pstDispCtxCfgDst);
    }

    return enRet;
}

static void _HalDispIfSetDeviceAttach(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceDetach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetDeviceDetach(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceBackGroundColor(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    u32 *                      pu32BgColor;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pu32BgColor   = (u32 *)pCfg;

    pstDevContain->u32BgColor = *pu32BgColor;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d, BgColor:%x\n", __FUNCTION__, __LINE__,
             pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, pstDevContain->u32BgColor);

    return enRet;
}

static void _HalDispIfSetDeviceBackGroundColor(void *pCtx, void *pCfg)
{
    u32 *pu32BgColor;
    u8   u8R, u8G, u8B;

    pu32BgColor = (u32 *)pCfg;

    u8R = (*pu32BgColor & 0x000000FF);
    u8G = (*pu32BgColor & 0x0000FF00) >> (8);
    u8B = (*pu32BgColor & 0x00FF0000) >> (16);
    HalDispSetFrameColor(u8R, u8G, u8B, pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceInterface(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    u32 *                      pu32Interface;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pu32Interface = (u32 *)pCfg;

    if (*pu32Interface & (HAL_DISP_INTF_YPBPR | HAL_DISP_INTF_LCD))
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, ContainId:%d, DevId:%d, In Intf:%s(%x) Not Supported\n", __FUNCTION__, __LINE__,
                 pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, PARSING_HAL_INTERFACE(*pu32Interface), *pu32Interface);
    }
    else
    {
        u8 i, u8Cnt;

        u8Cnt = 0;
        for (i = 0; i < 32; i++)
        {
            u8Cnt = (*pu32Interface & (1 << i)) ? u8Cnt + 1 : u8Cnt;
        }

        if (u8Cnt > 1 && !(DISP_OUTDEV_IS_VGA(*pu32Interface) && DISP_OUTDEV_IS_HDMI(*pu32Interface)))
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, ContainId:%d, DevId:%d, In Intf:%s(%x) Not Supported\n", __FUNCTION__, __LINE__,
                     pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, PARSING_HAL_INTERFACE(*pu32Interface),
                     *pu32Interface);
        }
        else
        {
            pstDevContain->u32Interface = *pu32Interface;
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d, In Intf:%s(%x), Contain Intf:%s(%x)\n",
                     __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
                     PARSING_HAL_INTERFACE(*pu32Interface), *pu32Interface,
                     PARSING_HAL_INTERFACE(pstDevContain->u32Interface), pstDevContain->u32Interface);
        }
    }

    return enRet;
}

static void _HalDispIfSetDeviceInterface(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg   = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    HalDispClkGpCtrlConfig_t * pstClkGpCtrlCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstClkGpCtrlCfg =
        (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId];
    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Dev:%d, ConId:%d If:%s\n", __FUNCTION__, __LINE__, 
        pstDevContain->u32DevId, pstDispCtxCfg->u32ContainIdx,PARSING_HAL_INTERFACE(pstDevContain->u32Interface));
    HalDispSetLcdFpllRefSel(0x03, pCtx);

    if (pstDevContain->u32Interface == HAL_DISP_INTF_TTL || pstDevContain->u32Interface == HAL_DISP_INTF_BT656
        || pstDevContain->u32Interface == HAL_DISP_INTF_BT601 || pstDevContain->u32Interface == HAL_DISP_INTF_BT1120
        || pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
    {
        if (pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0)
        {
            if (pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
            {
                HalDispSetLcdFpllDly(0x33, pCtx);
            }
            else
            {
                HalDispSetLcdFpllDly(0x60, pCtx);
            }
        }
        else
        {
            if (pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
            {
                HalDispSetLcdFpllDly(0xB4, pCtx);
            }
            else
            {
                HalDispSetLcdFpllDly(0xCC, pCtx);
            }
        }
        HalDispSetLcdSrcMux(pstDevContain->u32DevId, pCtx);

        if (pstDevContain->u32Interface == HAL_DISP_INTF_BT656 || pstDevContain->u32Interface == HAL_DISP_INTF_BT601)
        {
            HalDispSetHdmi2OdClkRate(1, pCtx);
        }
        else if (pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
        {
            HalDispSetHdmi2OdClkRate(2, pCtx);
        }
        HalDispSetTgenExtHsEn(0, pCtx);
        pstClkGpCtrlCfg->bEn   = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_LCD;
    }
    else if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface) || DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
    {
        pstClkGpCtrlCfg->eType = 0;
        if (DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface))
        {
            HalDispSetHdmiSrcMux(pstDevContain->u32DevId, pCtx);
            pstClkGpCtrlCfg->eType |= E_HAL_DISP_CLK_GP_CTRL_HDMI;
        }

        if (DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
        {
            HalDispSetDacSynthSetSel(DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface) ? 0 : 1, pCtx);
            if (pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0)
            {
                HalDispSetDacFpllDly(0x54, pCtx);
            }
            else
            {
                HalDispSetDacFpllDly(0x95, pCtx);
            }
            HalDispSetDacSrcMux(pstDevContain->u32DevId, pCtx);
            HalDispSetDacFpllRefSel(0x03, pCtx);
            HalDispSetDacExtFrmRstEn(0x01, pCtx);
            pstClkGpCtrlCfg->eType |= E_HAL_DISP_CLK_GP_CTRL_DAC;
        }
        _HalDispIfSetDefaultForClearPanelSetting(pCtx);
        pstClkGpCtrlCfg->bEn = 1;
    }
    else if (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface))
    {
        HalDispSetCvbsSrcMux(pstDevContain->u32DevId, pCtx);
        _HalDispIfSetDefaultForClearPanelSetting(pCtx);
        pstClkGpCtrlCfg->bEn   = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_CVBS;
    }
    else if (pstDevContain->u32Interface == HAL_DISP_INTF_MIPIDSI)
    {
        HalDispSetDsiSrcMux(pstDevContain->u32DevId, pCtx);

        HalDispSetDsiClkGateEn(1, pCtx);
        HalDispSetTgenExtHsEn(1, pCtx);
        HalDispSetDsiAff(0x0140, pCtx);
        HalDispSetDsiRst(1, pCtx);
        pstClkGpCtrlCfg->bEn   = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_MIPIDSI;
    }
    else if (pstDevContain->u32Interface == HAL_DISP_INTF_MCU || pstDevContain->u32Interface == HAL_DISP_INTF_MCU_NOFLM)
    {
        HalDispMcuLcdCmdqInit();
        HalDispKeepMcuReset(1, (void *)pstDispCtxCfg);
        HalDispSetCmdqMux(1, pCtx);
        HalDispSetLcdSrcMux(pstDevContain->u32DevId, pCtx);
        HalDispSetTgenExtHsEn(0, pCtx);
        pstClkGpCtrlCfg->bEn   = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_LCD;
    }
    else
    {
        g_stDispIrqHist.stWorkingStatus.stLcdStatus.u8Lcdused = 0;
        g_stDispIrqHist.stWorkingStatus.stCvbsStatus.u8Cvsbused = 0;
        g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8Vgaused = 0;
        _HalDispIfSetDefaultForClearPanelSetting(pCtx);
        pstClkGpCtrlCfg->bEn   = 0;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_NONE;
    }
    HalDispClkSetGpCtrlCfg(pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceOutputTiming(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    HalDispDeviceTimingInfo_t *pstDeviceTimingCfg;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg      = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain      = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDeviceTimingCfg = (HalDispDeviceTimingInfo_t *)pCfg;

    pstDevContain->eTimeType = pstDeviceTimingCfg->eTimeType;
    memcpy(&pstDevContain->stDevTimingCfg, &pstDeviceTimingCfg->stDeviceTimingCfg, sizeof(HalDispDeviceTimingConfig_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL,
             "%s %d, ContainId:%d, DevId:%d, H(%d %d %d %d %d) V(%d %d %d %d %d) Fps:%d, Ssc(%x %x), Peroid(%d %d %d %d)\n",
             __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
             pstDevContain->stDevTimingCfg.u16HsyncWidth, pstDevContain->stDevTimingCfg.u16HsyncBackPorch,
             pstDevContain->stDevTimingCfg.u16Hstart, pstDevContain->stDevTimingCfg.u16Hactive,
             pstDevContain->stDevTimingCfg.u16Htotal, pstDevContain->stDevTimingCfg.u16VsyncWidth,
             pstDevContain->stDevTimingCfg.u16VsyncBackPorch, pstDevContain->stDevTimingCfg.u16Vstart,
             pstDevContain->stDevTimingCfg.u16Vactive, pstDevContain->stDevTimingCfg.u16Vtotal,
             pstDevContain->stDevTimingCfg.u16Fps, pstDevContain->stDevTimingCfg.u16SscStep,
             pstDevContain->stDevTimingCfg.u16SscSpan, pstDevContain->stDevTimingCfg.u32VSyncPeriod,
             pstDevContain->stDevTimingCfg.u32VBackPorchPeriod, pstDevContain->stDevTimingCfg.u32VActivePeriod,
             pstDevContain->stDevTimingCfg.u32VFrontPorchPeriod);

    return enRet;
}
static u8 _HalDispIfSetPqByPqBinInSetOutputTiming(void *pCtx, u32 u32DevId)
{
    HalDispPicturePqConfig_t stPictureCfg;
    u16                      u16SrcType;
    u8                       bRet = 0;

    if (HalDispHpqGetSourceIdx(pCtx, &u16SrcType))
    {
        if (HalDispHpqGetLoadSettingType() == E_HAL_DISP_PQ_LOAD_SETTING_AUTO)
        {
            DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, SrcType:%d Dev:%d\n", __FUNCTION__, __LINE__, u16SrcType, u32DevId);
            HalDispHpqSetSrcId(pCtx, u32DevId, u16SrcType);
            bRet = 1;
            _HalDispIfGetPictureDataFromPq(pCtx, &stPictureCfg);
            HalDispPictureIfSetPqConfig(u32DevId, &stPictureCfg);
            HalDispPictureIfApplyPqConfig(pCtx);
            HalDispHqpLoadSetting(pCtx);
            if(u32DevId<HAL_DISP_DEVICE_MAX)
            {
                _HalDispIfSetInterfaceCsc(pCtx, u32DevId, E_HAL_DISP_CSC_MATRIX_USER);
                g_stInterCfg[u32DevId].u8CsCMd = E_DISP_COLOR_YUV_2_RGB_MATRIX_USER;
            }
        }
    }
    return bRet;
}
static void _HalDispIfSetDeviceOutputTiming(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    HalDispDeviceTimingInfo_t *pstDeviceTimingCfg;
    DrvDispCtxDeviceContain_t *pstDevContain   = NULL;
    u16                        u16TblId = 0;

    pstDispCtxCfg      = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain      = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDeviceTimingCfg = (HalDispDeviceTimingInfo_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ConId:%d DevId:%d, Interface:%s, TimingId:%s\n", __FUNCTION__, __LINE__,
             pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, PARSING_HAL_INTERFACE(pstDevContain->u32Interface),
             PARSING_HAL_TMING_ID(pstDeviceTimingCfg->eTimeType));

    _HalDispIfSetTgenResetSt(pstDispCtxCfg, pstDeviceTimingCfg);

    u16TblId = (DISP_OUTDEV_IS_CVBS(pstDevContain->u32Interface) || DISP_OUTDEV_IS_HDMI(pstDevContain->u32Interface)
                || DISP_OUTDEV_IS_VGA(pstDevContain->u32Interface))
                   ? _HalDispIfGetTblId(pstDeviceTimingCfg->eTimeType)
                   : 0;
    _HalDispIfGetOutputInterfaceClk(pCtx);
    _HalDispIfDumpOutputInterfaceTbl(pCtx, u16TblId);
    _HalDispIfSetPqByPqBinInSetOutputTiming(pCtx, pstDevContain->u32DevId);
    _HalDispIfSetTgenConfig(pstDispCtxCfg);
    _HalDispIfSetTgenResetEnd(pstDispCtxCfg, pstDeviceTimingCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceVgaParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceParam_t *        pstVgaParm    = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstVgaParm    = (HalDispDeviceParam_t *)pCfg;

    if (pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS
        || pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC
        || pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stDeviceParam.stCsc , &pstVgaParm->stCsc, sizeof(HalDispCsc_t));
        pstDevContain->stDeviceParam.u32Sharpness = pstVgaParm->u32Sharpness;
        pstDevContain->stDeviceParam.u32Gain = pstVgaParm->u32Gain;

        DISP_DBG(DISP_DBG_LEVEL_HAL,
                 "%s %d, ContainId:%d, DevId:%d, Matrix:%s, Luma:%d, Contrast:%d, Hue:%d, Sat:%d, Gain:%d Sharp:%d\n",
                 __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
                 PARSING_HAL_CSC_MATRIX(pstVgaParm->stCsc.eCscMatrix), pstVgaParm->stCsc.u32Luma,
                 pstVgaParm->stCsc.u32Contrast, pstVgaParm->stCsc.u32Hue, pstVgaParm->stCsc.u32Saturation,
                 pstVgaParm->u32Gain, pstVgaParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_CSC_MATRIX(pstVgaParm->stCsc.eCscMatrix));
    }

    return enRet;
}

static void _HalDispIfSetDeviceVgaParam(void *pCtx, void *pCfg)
{
    HalDispDeviceParam_t *    pstVgaParm = NULL;
    HalDispPictureConfig_t stPictureCfg;
    s16                    as16TrimVal[3];

    pstVgaParm = (HalDispDeviceParam_t *)pCfg;
    if (HalDispPictureIfTransNonLinear(pCtx, &pstVgaParm->stCsc, &pstVgaParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstVgaParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);

        //          49 -> -1
        // u32Gain: 50 -> 0
        //          51 -> 1
        as16TrimVal[0] = (s16)g_stVgaDacTrimCfg.u16Trim[0];
        as16TrimVal[1] = (s16)g_stVgaDacTrimCfg.u16Trim[1];
        as16TrimVal[2] = (s16)g_stVgaDacTrimCfg.u16Trim[2];
        _HalDispIfSetDacTrim(as16TrimVal, pstVgaParm->u32Gain - 50, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceHdmiParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceParam_t *       pstHdmiParm   = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstHdmiParm   = (HalDispDeviceParam_t *)pCfg;

    if (pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS
        || pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC
        || pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stDeviceParam.stCsc , &pstHdmiParm->stCsc, sizeof(HalDispCsc_t));
        pstDevContain->stDeviceParam.u32Sharpness = pstHdmiParm->u32Sharpness;

        DISP_DBG(DISP_DBG_LEVEL_HAL,
                 "%s %d, ContainId:%d, DevId:%d, Matrix:%s, Luma:%d, Contrsat:%d, Hue:%d, Sat:%d, Sharp:%d\n", __FUNCTION__,
                 __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
                 PARSING_HAL_CSC_MATRIX(pstHdmiParm->stCsc.eCscMatrix), pstHdmiParm->stCsc.u32Luma,
                 pstHdmiParm->stCsc.u32Contrast, pstHdmiParm->stCsc.u32Hue, pstHdmiParm->stCsc.u32Saturation,
                 pstHdmiParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_CSC_MATRIX(pstHdmiParm->stCsc.eCscMatrix));
    }

    return enRet;
}

static void _HalDispIfSetDeviceHdmiParam(void *pCtx, void *pCfg)
{
    HalDispPictureConfig_t stPictureCfg;
    HalDispDeviceParam_t *   pstHdmiParm = NULL;

    pstHdmiParm = (HalDispDeviceParam_t *)pCfg;
    if (HalDispPictureIfTransNonLinear(pCtx, &pstHdmiParm->stCsc, &pstHdmiParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstHdmiParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceLcdParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceParam_t *        pstLcdParm    = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstLcdParm    = (HalDispDeviceParam_t *)pCfg;

    if (pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS
        || pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC
        || pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stDeviceParam.stCsc , &pstLcdParm->stCsc, sizeof(HalDispCsc_t));
        pstDevContain->stDeviceParam.u32Sharpness = pstLcdParm->u32Sharpness;

        DISP_DBG(DISP_DBG_LEVEL_HAL,
                 "%s %d, ContainId:%d, DevId:%d, Matrix:%s, Luma:%d, Contrast:%d, Hue:%d, Sat:%d, Sharp:%d\n", __FUNCTION__,
                 __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
                 PARSING_HAL_CSC_MATRIX(pstLcdParm->stCsc.eCscMatrix), pstLcdParm->stCsc.u32Luma,
                 pstLcdParm->stCsc.u32Contrast, pstLcdParm->stCsc.u32Hue, pstLcdParm->stCsc.u32Saturation,
                 pstLcdParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_CSC_MATRIX(pstLcdParm->stCsc.eCscMatrix));
    }

    return enRet;
}

static void _HalDispIfSetDeviceLcdParam(void *pCtx, void *pCfg)
{
    HalDispDeviceParam_t *    pstLcdParm = NULL;
    HalDispPictureConfig_t stPictureCfg;

    pstLcdParm = (HalDispDeviceParam_t *)pCfg;
    if (HalDispPictureIfTransNonLinear(pCtx, &pstLcdParm->stCsc, &pstLcdParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstLcdParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceGammaParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispGammaParam_t *      pstGammaParam = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstGammaParam = (HalDispGammaParam_t *)pCfg;

    if (pstGammaParam->bEn && pstGammaParam->u16EntryNum != 33)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, ContainId:%d, DevId:%d ColorEntyr is %d, not 32\n", __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx,
                 pstDevContain->u32DevId, pstGammaParam->u16EntryNum);
    }
    else
    {
        if (pstDevContain->stGammaParam.pu8ColorR == NULL && pstDevContain->stGammaParam.pu8ColorB == NULL
            && pstDevContain->stGammaParam.pu8ColorG == NULL)
        {
            pstDevContain->stGammaParam.pu8ColorR = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorG = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorB = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
        }
        else if (pstDevContain->stGammaParam.u16EntryNum < pstGammaParam->u16EntryNum)
        {
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorR);
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorG);
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorB);
            pstDevContain->stGammaParam.pu8ColorR = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorG = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorB = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
        }

        if (pstDevContain->stGammaParam.pu8ColorR == NULL || pstDevContain->stGammaParam.pu8ColorG == NULL
            || pstDevContain->stGammaParam.pu8ColorB == NULL)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            memcpy(pstDevContain->stGammaParam.pu8ColorR, pstGammaParam->pu8ColorR, pstGammaParam->u16EntryNum);
            memcpy(pstDevContain->stGammaParam.pu8ColorG, pstGammaParam->pu8ColorG, pstGammaParam->u16EntryNum);
            memcpy(pstDevContain->stGammaParam.pu8ColorB, pstGammaParam->pu8ColorB, pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.u16EntryNum = pstGammaParam->u16EntryNum;
            pstDevContain->stGammaParam.bEn         = pstGammaParam->bEn;
        }
    }
    return enRet;
}

static void _HalDispIfSetDeviceGammaParam(void *pCtx, void *pCfg)
{
    HalDispGammaParam_t *    pstGammaParam = NULL;
    HalDispMaceGammaConfig_t stHalPqGammaCfg;

    pstGammaParam = (HalDispGammaParam_t *)pCfg;

    stHalPqGammaCfg.bUpdate = 1;
    stHalPqGammaCfg.u8En    = pstGammaParam->bEn;
    memcpy(stHalPqGammaCfg.u8R, pstGammaParam->pu8ColorR, 33);
    memcpy(stHalPqGammaCfg.u8G, pstGammaParam->pu8ColorG, 33);
    memcpy(stHalPqGammaCfg.u8B, pstGammaParam->pu8ColorB, 33);

    HalDispMaceSetGammaConfig(&stHalPqGammaCfg, pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceCvbsParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceParam_t *pstCvbsParm = NULL;
    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstCvbsParm = (HalDispDeviceParam_t *)pCfg;
    if(pstCvbsParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS ||
       pstCvbsParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ||
       pstCvbsParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stDeviceParam.stCsc , &pstCvbsParm->stCsc, sizeof(HalDispCsc_t));
        pstDevContain->stDeviceParam.bEnable = pstCvbsParm->bEnable;
        pstDevContain->stDeviceParam.u32Sharpness = pstCvbsParm->u32Sharpness;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ContainIdx:%d, DevId:%d, Matrix:%s, Luma:%d, Contrsat:%d, Hue:%d, Sat:%d, Sharp:%d, En:%d\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId,
            PARSING_HAL_CSC_MATRIX(pstCvbsParm->stCsc.eCscMatrix),
            pstCvbsParm->stCsc.u32Luma, pstCvbsParm->stCsc.u32Contrast,
            pstCvbsParm->stCsc.u32Hue, pstCvbsParm->stCsc.u32Saturation,
            pstCvbsParm->u32Sharpness, pstCvbsParm->bEnable);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__, PARSING_HAL_CSC_MATRIX(pstCvbsParm->stCsc.eCscMatrix));
    }
    return enRet;
}

static void _HalDispIfSetDeviceCvbsParam(void *pCtx, void *pCfg) 
{
    HalDispPictureConfig_t stPictureCfg;
    HalDispDeviceParam_t *pstCvbsParm = NULL;
    pstCvbsParm = (HalDispDeviceParam_t *)pCfg;
    if(HalDispPictureIfTransNonLinear(pCtx, &pstCvbsParm->stCsc, &pstCvbsParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstCvbsParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceColorTemp(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispColorTemp_t *       pstColorTemp  = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstColorTemp  = (HalDispColorTemp_t *)pCfg;

    memcpy(&pstDevContain->stColorTemp, pstColorTemp, sizeof(HalDispColorTemp_t));

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, ContainId:%d, DevId:%d, Offset(%x %x %x) Color(%x %x %x]\n", __FUNCTION__,
             __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, pstDevContain->stColorTemp.u16RedOffset,
             pstDevContain->stColorTemp.u16GreenOffset, pstDevContain->stColorTemp.u16BlueOffset,
             pstDevContain->stColorTemp.u16RedColor, pstDevContain->stColorTemp.u16GreenColor,
             pstDevContain->stColorTemp.u16BlueColor);

    return enRet;
}

static void _HalDispIfSetDeviceColorTemp(void *pCtx, void *pCfg)
{
    HalDispColorTemp_t *       pstColorTemp  = NULL;
    DrvDispCtxConfig_t *       pstDispCtx    = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtx    = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];

    pstColorTemp = (HalDispColorTemp_t *)pCfg;
    HalDispColorAdjustVideoRGB(g_stInterCfg[pstDevContain->u32DevId].u8ColorId, pstColorTemp->u16RedColor,
                               pstColorTemp->u16GreenColor, pstColorTemp->u16BlueColor, pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDevicTimeZone(void *pCtx, void *pCfg)
{
#if DISP_TIMEZONE_ISR_SUPPORT
    HalDispQueryRet_e   enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispTimeZone_t * pstTimeZone   = NULL;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstTimeZone   = (HalDispTimeZone_t *)pCfg;

    pstTimeZone->enType = _HalDispIfGetTimeZoneType(pstDispCtxCfg);
    return enRet;
#else
    return E_HAL_DISP_QUERY_RET_NOTSUPPORT;
#endif
}

static void _HalDispIfSetDeviceTimeZone(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDevicTimeZoneConfig(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetDeviceTimeZoneConfig(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDevicDisplayInfo(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e            enRet           = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *         pstDispCtxCfg   = NULL;
    DrvDispCtxDeviceContain_t *  pstDevContain   = NULL;
    HalDispDeviceTimingConfig_t *pstDevTimingCfg = NULL;
    HalDispDisplayInfo_t *       pstDisplayInfo  = NULL;

    pstDispCtxCfg   = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain   = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstDevTimingCfg = &pstDevContain->stDevTimingCfg;
    pstDisplayInfo  = (HalDispDisplayInfo_t *)pCfg;

    pstDisplayInfo->u16Htotal   = pstDevTimingCfg->u16Htotal;
    pstDisplayInfo->u16Vtotal   = pstDevTimingCfg->u16Vtotal;
    pstDisplayInfo->u16HdeStart = pstDevTimingCfg->u16Hstart;
    pstDisplayInfo->u16VdeStart = pstDevTimingCfg->u16Vstart;
    pstDisplayInfo->u16Width    = pstDevTimingCfg->u16Hactive;
    pstDisplayInfo->u16Height   = pstDevTimingCfg->u16Vactive;
    pstDisplayInfo->bInterlace  = 0;
    pstDisplayInfo->bYuvOutput  = 0;

    DISP_DBG(
        DISP_DBG_LEVEL_HAL, "%s %d, ContainId:%d, DevId:%d, Total(%d %d) DeSt(%d %d), Size(%d %d), Interlace:%d, Yuv:%d\n",
        __FUNCTION__, __LINE__, pstDispCtxCfg->u32ContainIdx, pstDevContain->u32DevId, pstDisplayInfo->u16Htotal,
        pstDisplayInfo->u16Vtotal, pstDisplayInfo->u16HdeStart, pstDisplayInfo->u16VdeStart, pstDisplayInfo->u16Width,
        pstDisplayInfo->u16Height, pstDisplayInfo->bInterlace, pstDisplayInfo->bYuvOutput);

    return enRet;
}

static void _HalDispIfSetDeviceDisplayInfo(void *pCtx, void *pCfg) {}

//-------------------------------------------------------------------------------
// VidLayer
//-------------------------------------------------------------------------------
static HalDispQueryRet_e _HalDispIfGetInfoVidLayerInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

static void _HalDispIfSetVidLayerInit(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

static void _HalDispIfSetVidLayerEnable(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e              enRet                = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *           pstDispVideoLayerCtx = NULL;
    DrvDispCtxConfig_t *           pstDispDevCtx        = NULL;
    DrvDispCtxDeviceContain_t *    pstDevContain        = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain   = NULL;

    pstDispVideoLayerCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDispDevCtx        = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain        = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32ContainIdx];
    pstVidLayerContain   = pstDispVideoLayerCtx->pstCtxContain->pstVidLayerContain[pstDispVideoLayerCtx->u32ContainIdx];

    if (pstDispDevCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE
        && pstDispVideoLayerCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        if (pstVidLayerContain->pstDevCtx)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, VidLayerContainId:%d, VidLayerId:%d,  Arelady Bind The Device\n", __FUNCTION__, __LINE__,
                     pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId);
        }
        else
        {
            if (pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0
                && (pstVidLayerContain->u32VidLayerId == E_HAL_DISP_LAYER_MOPG_VIDEO1_ID
                    || pstVidLayerContain->u32VidLayerId == E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, VidLayerContainId:%d, VidLayerId:%d,  HW Not Supoorted\n", __FUNCTION__, __LINE__,
                         pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevContainId:%d, DevId:%d, VidLayerContainId:%d, VidLayerId:%d\n",
                         __FUNCTION__, __LINE__, pstDispDevCtx->u32ContainIdx, pstDevContain->u32DevId,
                         pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId);

                pstVidLayerContain->pstDevCtx                                   = (void *)pstDevContain;
                pstDevContain->pstVidLayeCtx[pstVidLayerContain->u32VidLayerId] = (void *)pstVidLayerContain;
                pstDevContain->eBindVideoLayer |= (0x1 << pstVidLayerContain->u32VidLayerId);
            }
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContainId:%d, DevId:%d(%s), VidLayerContainId:%d, VidLayerId:%d(%s)\n", __FUNCTION__, __LINE__,
                 pstDispDevCtx->u32ContainIdx, pstDevContain->u32DevId, PARSING_CTX_TYPE(pstDispDevCtx->enCtxType),
                 pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId,
                 PARSING_CTX_TYPE(pstDispVideoLayerCtx->enCtxType));
    }

    return enRet;
}

static void _HalDispIfSetVidLayerBind(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *       pstDispDevCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8                         i, u8VideLayerBindMergeEn;

    pstDispDevCtx = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32ContainIdx];

    u8VideLayerBindMergeEn = 0;
    for (i = 0; i < HAL_DISP_VIDLAYER_MAX; i++)
    {
        if (pstDevContain->pstVidLayeCtx[i])
        {
            if ((pstDevContain->u32DevId == 1) && ((i == 0) || (i == 1)))
            {
                u8VideLayerBindMergeEn = 1;
            }
        }
    }

    // if(u8VideLayerBindNum > 2)
    pstDevContain->bMergeCase = u8VideLayerBindMergeEn;
    if (u8VideLayerBindMergeEn)
    { // MOP Merge case
        HalDispSetMopWinMerge(1, pCtx);
        HalDispMopIfSetInfoStretchWinMop0Auto(pCtx, 0); // while merge, MOP0 can only use its own clock
        HalDispClkSetMergeOdClk(pCtx);
        HalDispSetSwReset(0, pCtx, HAL_DISP_DEVICE_ID_0);
        g_stInterCfg[pstDevContain->u32DevId].bRstDisp = 0;
    }
    else
    { // non Merge case
        HalDispSetMopWinMerge(0, pCtx);
        HalDispMopIfSetInfoStretchWinMop0Auto(pCtx, 1);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerUnBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e              enRet                = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *           pstDispVideoLayerCtx = NULL;
    DrvDispCtxConfig_t *           pstDispDevCtx        = NULL;
    DrvDispCtxDeviceContain_t *    pstDevContain        = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain   = NULL;

    pstDispVideoLayerCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDispDevCtx        = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain        = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32ContainIdx];
    pstVidLayerContain   = pstDispVideoLayerCtx->pstCtxContain->pstVidLayerContain[pstDispVideoLayerCtx->u32ContainIdx];

    if (pstDispDevCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE
        && pstDispVideoLayerCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        if (pstVidLayerContain->pstDevCtx == pstDevContain)
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevContainId:%d, DevId:%d, VidLayerContainId:%d, VidLayrId:%d\n", __FUNCTION__,
                     __LINE__, pstDispDevCtx->u32ContainIdx, pstDevContain->u32DevId, pstDispVideoLayerCtx->u32ContainIdx,
                     pstVidLayerContain->u32VidLayerId);

            pstVidLayerContain->pstDevCtx                                   = NULL;
            pstDevContain->pstVidLayeCtx[pstVidLayerContain->u32VidLayerId] = NULL;
            pstDevContain->eBindVideoLayer &= ~(0x1 << pstVidLayerContain->u32VidLayerId);
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, DevContainId:%d, DevId:%d,(%px) != VidLayerContainId:%d, VidLayerId:%d(%px)\n", __FUNCTION__,
                     __LINE__, pstDispDevCtx->u32ContainIdx, pstDevContain->u32DevId, pstDevContain,
                     pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId, pstVidLayerContain->pstDevCtx);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContainId:%d, DevId:%d(%s), VidLayerContainId:%d, VidLayerId:%d(%s)\n", __FUNCTION__, __LINE__,
                 pstDispDevCtx->u32ContainIdx, pstDevContain->u32DevId, PARSING_CTX_TYPE(pstDispDevCtx->enCtxType),
                 pstDispVideoLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId,
                 PARSING_CTX_TYPE(pstDispVideoLayerCtx->enCtxType));
    }

    return enRet;
}

static void _HalDispIfSetVidLayerUnBind(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoStretchWinSize(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetVidLayerAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInfoStretchWinSize(pCtx, pCfg);
    HalDispMopIfSetInfoHextSize(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoImiStartAddr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoImiStartAddr(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetImiStartAddr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetImiStartAddr(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerCompress(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetVidLayerComporess(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerPriority(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetVidLayerPriority(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerBufferFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetVidLayerBufferFire(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerCheckFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetVidLayerCheckFire(void *pCtx, void *pCfg) {}

//-------------------------------------------------------------------------------
// InputPort
//-------------------------------------------------------------------------------
static HalDispQueryRet_e _HalDispIfGetInfoInputPortInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;

    enRet = HalDispMopIfGetInfoInputPortInit(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetInputPortInit(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortInit(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortEnable(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortEnable(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortAttr(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortAttr(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortShow(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetInputPortShow(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortHide(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetInputPortHide(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortBegin(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetInputPortBegin(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortEnd(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

static void _HalDispIfSetInputPortEnd(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortFlip(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortFlip(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetInputPortFlip(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortFlip(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortRotate(pCtx, pCfg);
    return enRet;
}

static void _HalDispIfSetInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortRotate(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortCrop(pCtx, pCfg);
    return enRet;
}
static void _HalDispIfSetInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortCrop(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet                   = HalDispMopIfGetInfoInputPortRingBuffAttr(pCtx, pCfg);
    return enRet;
}
static void _HalDispIfSetInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortRingBuffAttr(pCtx, pCfg);
}

static HalDispQueryRet_e _HalDispIfGetInfoClkGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e   enRet                             = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkConfig_t *pstClkCfg                         = NULL;
    u8                  u8ClkName[E_HAL_DISP_CLK_NUM][20] = HAL_DISP_CLK_NAME;
    u32                 i;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;

    pstClkCfg->u32Num = E_HAL_DISP_CLK_NUM;

    for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
    {
        _HalDispifClkGet(i, pstClkCfg);
        DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, CLK_%-10s: En:%d, Rate:%d\n", __FUNCTION__, __LINE__, u8ClkName[i],
                 pstClkCfg->bEn[i], pstClkCfg->u32Rate[i]);
    }
    DISP_DBG_VAL(u8ClkName[0][0]);
    return enRet;
}

static void _HalDispIfSetClkGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoClkSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e   enRet     = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkConfig_t *pstClkCfg = NULL;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;

    if (pstClkCfg->u32Num != E_HAL_DISP_CLK_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Clk Num (%d) is not match %d\n", __FUNCTION__, __LINE__, pstClkCfg->u32Num,
                 E_HAL_DISP_CLK_NUM);
    }
    else
    {
        u32 i;
        u8  au8ClkName[E_HAL_DISP_CLK_NUM][20] = HAL_DISP_CLK_NAME;

        for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
        {
            DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, CLK_%s: En:%d, Rate:%d\n", __FUNCTION__, __LINE__, au8ClkName[i],
                     pstClkCfg->bEn[i], pstClkCfg->u32Rate[i]);
        }
        DISP_DBG_VAL(au8ClkName[0][0]);
    }

    return enRet;
}

static void _HalDispIfSetClkSet(void *pCtx, void *pCfg)
{
    HalDispClkConfig_t *pstClkCfg = NULL;
    u32                 i;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;
    for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
    {
        _HalDispifClkSet(i, pstClkCfg);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoPqSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e  enRet    = E_HAL_DISP_QUERY_RET_OK;
    HalDispPqConfig_t *pstPqCfg = NULL;

    pstPqCfg = (HalDispPqConfig_t *)pCfg;

    if (pstPqCfg == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Null Data\n", __FUNCTION__, __LINE__);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Flag: %s\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_PQ_FLAG_TYPE(pstPqCfg->u32PqFlags));
        if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_LOAD_BIN)
        {
            if (pstPqCfg->pData == NULL || pstPqCfg->u32DataSize == 0)
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, pData=%p, Size:%d\n", __FUNCTION__, __LINE__, pstPqCfg->pData, pstPqCfg->u32DataSize);
            }
        }
        else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_FREE_BIN)
        {
            if (pstPqCfg->pData == NULL)
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, pData=%p\n", __FUNCTION__, __LINE__, pstPqCfg->pData);
            }
        }
        else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_SET_SRC_ID
                 || pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_BYPASS
                 || pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_PROCESS
                 || pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE)
        {
            if (pstPqCfg->u32DataSize != sizeof(u32) || pstPqCfg->pData == NULL)
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, Data Size(%d) is not correct", __FUNCTION__, __LINE__, pstPqCfg->u32DataSize);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, Data:%x\n", __FUNCTION__, __LINE__, *((u32 *)pstPqCfg->pData));
            }
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, UnSupport Flag:%x", __FUNCTION__, __LINE__, pstPqCfg->u32PqFlags);
        }
    }

    return enRet;
}
static void _HalDispIfSetPqSet(void *pCtx, void *pCfg)
{
    HalDispPqConfig_t *        pstPqCfg = NULL;
    HalDispPicturePqConfig_t   stPictureCfg;
    u32                        u32Data = 0;
    u16                        u16DevId, u16SrcId;
    DrvDispCtxConfig_t *       pstDispCtxCfg = pCtx;
    HalDispPqLoadSettingType_e ePqLoadSettingType;

    pstPqCfg = (HalDispPqConfig_t *)pCfg;
    DISP_DBG(DISP_DBG_LEVEL_HPQ, "%s %d, PqFlags:%s(%u)\n", __FUNCTION__, __LINE__,
             PARSING_HAL_PQ_FLAG_TYPE(pstPqCfg->u32PqFlags), pstPqCfg->u32PqFlags);

    if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_LOAD_BIN)
    {
        for (u16DevId = 0; u16DevId < HAL_DISP_DEVICE_MAX; u16DevId++)
        {
            _HalDispIfPqSetBypass(pCtx, 0, u16DevId, E_DISP_UTILITY_REG_ACCESS_CPU);
        }
        HalDispHpqLoadBin(pCtx, pCfg); // Load PQ Bin
    }
    else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_FREE_BIN)
    {
        HalDispHpqFreeBin(pCtx, pCfg); // Free PQ Bin
        for (u16DevId = 0; u16DevId < HAL_DISP_DEVICE_MAX; u16DevId++)
        {
            _HalDispIfPqSetBypass(pCtx, 1, u16DevId, E_DISP_UTILITY_REG_ACCESS_CPU);
        }
    }
    else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_BYPASS)
    {
        if (pstDispCtxCfg && pstDispCtxCfg->pstCtxContain)
        {
            u16DevId = HalDispGetDeviceId(pstDispCtxCfg, 0);
        }
        else
        {
            u32Data  = *((u32 *)pstPqCfg->pData);
            u16DevId = u32Data & 0x0000FFFF;
        }
        _HalDispIfPqSetBypass(pCtx, 1, u16DevId, E_DISP_UTILITY_REG_ACCESS_CPU);
    }
    else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_SET_SRC_ID)
    {
        if (pstPqCfg->u32DataSize == sizeof(u32) && pstPqCfg->pData)
        {
            u32Data  = *((u32 *)pstPqCfg->pData);
            u16SrcId = u32Data & 0x0000FFFF;
            u16DevId = u32Data >> 16;

            HalDispHpqSetSrcId(pCtx, u16DevId, u16SrcId);
            HalDispHpqSetPnlId(pCtx, u16DevId);

            _HalDispIfGetPictureDataFromPq(pCtx, &stPictureCfg);
            HalDispPictureIfSetPqConfig(u16DevId, &stPictureCfg);
        }
    }
    else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_PROCESS)
    {
        u32Data = *((u32 *)pstPqCfg->pData);

        if (u32Data)
        {
            HalDispHqpLoadSetting(pCtx);
            HalDispPictureIfApplyPqConfig(pCtx);
            u16DevId = HalDispGetDeviceId(pCtx, 0);
            if(u16DevId<HAL_DISP_DEVICE_MAX)
            {
            _HalDispIfSetInterfaceCsc(pCtx, u16DevId, E_HAL_DISP_CSC_MATRIX_USER);
            g_stInterCfg[u16DevId].u8CsCMd = E_DISP_COLOR_YUV_2_RGB_MATRIX_USER;
            }
        }
    }
    else if (pstPqCfg->u32PqFlags == E_HAL_DISP_PQ_FLAG_SET_LOAD_SETTING_TYPE)
    {
        u32Data            = *((u32 *)pstPqCfg->pData);
        ePqLoadSettingType = u32Data == 0 ? E_HAL_DISP_PQ_LOAD_SETTING_AUTO : E_HAL_DISP_PQ_LOAD_SETTING_MANUAL;
        HalDispHpqSetLoadSettingType(ePqLoadSettingType);
    }
    else
    {
        DISP_ERR("%s %d, UnSupport Flag:%x\n", __FUNCTION__, __LINE__, pstPqCfg->u32PqFlags);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoPqGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

static void _HalDispIfSetPqGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDrvTurningSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispDrvTurningConfig_t *pstDacTrimCfg = (HalDispDrvTurningConfig_t *)pCfg;

    if (pstDacTrimCfg->enType != E_HAL_DISP_DRV_TURNING_RGB)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
    }
    else if (pstDacTrimCfg->u16Trim[0] != pstDacTrimCfg->u16Trim[1]
             || pstDacTrimCfg->u16Trim[0] != pstDacTrimCfg->u16Trim[2]
             || pstDacTrimCfg->u16Trim[1] != pstDacTrimCfg->u16Trim[2])
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_OK;
    }

    return enRet;
}

static void _HalDispIfSetDrvTurningSet(void *pCtx, void *pCfg)
{
    HalDispDrvTurningConfig_t *pstDacTrimCfg = (HalDispDrvTurningConfig_t *)pCfg;
    s16                        as16TimValue[3];
    u8                         i;

    _HalDispIfGetDefautDacTrim(g_a16VgaDacTrimDefault, pCtx);

    for (i = 0; i < 3; i++)
    {
        as16TimValue[i] = (s16)g_a16VgaDacTrimDefault[i];
    }

    _HalDispIfSetDacTrim(as16TimValue, (s16)pstDacTrimCfg->u16Trim[0], pCtx);

    HalDispGetDacTriming(&g_stVgaDacTrimCfg.u16Trim[0], &g_stVgaDacTrimCfg.u16Trim[1], &g_stVgaDacTrimCfg.u16Trim[2],
                         pCtx);
}

static HalDispQueryRet_e _HalDispIfGetInfoDrvTurningGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispDrvTurningConfig_t *pstDacTrimCfg = (HalDispDrvTurningConfig_t *)pCfg;

    if (pstDacTrimCfg->enType != E_HAL_DISP_DRV_TURNING_RGB)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
    }
    else
    {
        HalDispGetDacTriming(&pstDacTrimCfg->u16Trim[0], &pstDacTrimCfg->u16Trim[1], &pstDacTrimCfg->u16Trim[2], pCtx);
    }

    return enRet;
}

static void _HalDispIfSetDrvTurningGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDbgmgGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e            enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;
    HalDispDbgmgConfig_t *       pstDbgmgCfg   = NULL;

    pstDbgmgCfg   = (HalDispDbgmgConfig_t *)pCfg;

    HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx);
    pstDbgmgCfg->pData += DISPDEBUG_SPRINTF(pstDbgmgCfg->pData, PAGE_SIZE, "Cmdq_%d::  WaitCnt:%d, FlipCnt:%d\n",pstCmdqCtx->s32UtilityId, 
        pstCmdqCtx->u16WaitDoneCnt, pstCmdqCtx->u16RegFlipCnt);

    return enRet;
}

static void _HalDispIfSetDbgmgGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoRegAccess(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e         enRet           = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *      pstDispCtxCfg   = NULL;
    void *                    pstCmdqCtx      = NULL;

    pstDispCtxCfg   = (DrvDispCtxConfig_t *)pCtx;

    if (HalDispGetCmdqByCtx(pCtx, &pstCmdqCtx))
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, AccessMode:%s, CmdqHandler:%px\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_REG_ACCESS_TYPE(((HalDispUtilityCmdqContext_t *)pstCmdqCtx)->s32UtilityId), pstCmdqCtx);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Unknow CTX Type:%d\n", __FUNCTION__, __LINE__, pstDispCtxCfg->enCtxType);
    }
    return enRet;
}

static void _HalDispIfSetRegAccess(void *pCtx, void *pCfg)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;

    HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx);

    if (pstCmdqCtx)
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Id:%s Change AccessMode Not support:, CmdqHandler:%px\n",
                 __FUNCTION__, __LINE__,
                 PARSING_HAL_REG_ACCESS_TYPE(((HalDispUtilityCmdqContext_t *)pstCmdqCtx)->s32UtilityId), pstCmdqCtx);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoRegFlip(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e            enRet         = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *         pstDispCtxCfg = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx    = NULL;
    HalDispRegFlipConfig_t *     pstRegFlipCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstRegFlipCfg = (HalDispRegFlipConfig_t *)pCfg;

    if (HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx))
    {
        if (pstCmdqCtx)
        {
            HalDispUtilitySetCmdqInf(pstRegFlipCfg->pCmdqInf, (u32)pstCmdqCtx->s32UtilityId);

            if ((HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CPU)
                || pstRegFlipCfg->pCmdqInf == NULL)
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%d, RIU MODE\n", __FUNCTION__,
                         __LINE__, PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%d, CMDQ MODE\n", __FUNCTION__,
                         __LINE__, PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId);
            }
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CtxType:%s, CmdqId:%d, CmdqCtx:%px, CmdqHander:%px \n", __FUNCTION__, __LINE__,
                     PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId, pstCmdqCtx,
                     pstRegFlipCfg->pCmdqInf);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Get Cmdq Ctx Fail, CtxType:%s\n", __FUNCTION__, __LINE__,
                 PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType));
    }

    return enRet;
}

static void _HalDispIfSetRegFlip(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *    pstDispCtxCfg = NULL;
    void *                  pstCmdqCtx    = NULL;
    HalDispRegFlipConfig_t *pstRegFlipCfg = (HalDispRegFlipConfig_t *)pCfg;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;

    if (pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        HalDispGetCmdqByCtx(pCtx, &pstCmdqCtx);
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CmdqCtx:%px, bEnFlip:%hhd, Dev MODE\n", __FUNCTION__, __LINE__,
                 pstCmdqCtx, pstRegFlipCfg->bEnable);
        if (pstRegFlipCfg->bEnable)
        {
            if (pstCmdqCtx)
            {
                _HalDispIfSetRegFlipPreAct(pstDispCtxCfg);

                HalDispUtilityW2BYTEMSKDirectCmdqWrite(pstCmdqCtx);
                _HalDispIfSetRegFlipPostAct(pstDispCtxCfg);
            }
        }
    }
    else if (pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        HalDispDmaIfRegFlip(pstDispCtxCfg);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoRegWaitDone(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e            enRet             = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *         pstDispCtxCfg     = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = NULL;
    HalDispRegWaitDoneConfig_t * pstRegWaitDoneCfg = NULL;

    pstDispCtxCfg     = (DrvDispCtxConfig_t *)pCtx;
    pstRegWaitDoneCfg = (HalDispRegWaitDoneConfig_t *)pCfg;

    if (HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx))
    {
        if (pstCmdqCtx)
        {
            HalDispUtilitySetCmdqInf(pstRegWaitDoneCfg->pCmdqInf, pstCmdqCtx->s32UtilityId);

            if ((HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CPU)
                || pstRegWaitDoneCfg->pCmdqInf == NULL)
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%d, RIU MODE\n", __FUNCTION__,
                         __LINE__, PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%d, CMDQ MODE\n", __FUNCTION__,
                         __LINE__, PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId);
            }
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CtxType:%s, CmdqId:%d, CmdqCtx:%px, CmdqHander:%px \n", __FUNCTION__, __LINE__,
                     PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstCmdqCtx->s32UtilityId, pstCmdqCtx,
                     pstRegWaitDoneCfg->pCmdqInf);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Get Cmdq Ctx Fail, CtxType%s\n", __FUNCTION__, __LINE__,
                 PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType));
    }

    return enRet;
}

static void _HalDispIfSetRegWaitDone(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *         pstDispCtxCfg     = NULL;
    HalDispRegWaitDoneConfig_t * pstRegWaitDoneCfg = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = NULL;
    u32                          u32Interface      = 0;

    pstDispCtxCfg     = (DrvDispCtxConfig_t *)pCtx;
    pstRegWaitDoneCfg = (HalDispRegWaitDoneConfig_t *)pCfg;

    if (pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx);
        u32Interface = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->u32Interface;

        if (pstRegWaitDoneCfg->pCmdqInf)
        {
            HalDispUtilitySetCmdqInf(pstRegWaitDoneCfg->pCmdqInf, pstCmdqCtx->s32UtilityId);
        }

        if (!(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->u32Interface & HAL_DISP_INTF_MCU)
            && !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx]->u32Interface
                 & HAL_DISP_INTF_MCU_NOFLM))
        {
            _HalDispIfSetWaitDonePreAct(pstDispCtxCfg);
            HalDispUtilityAddWaitCmd(pstCmdqCtx);        // add wait even
            _HalDispIfSetWaitDonePostAct(pstDispCtxCfg); // Clear disp2cmdq intterupt
        }
        else
        {
            //_HalDispIfSetWaitDonePreAct((HalDispUtilityCmdqContext_t *)pstCmdqCtx);
            // wait flm rising edge
#if CMDQ_POLL_EQ_MODE // polling mode
            HalDispUtilityPollWait(pstRegWaitDoneCfg->pCmdqInf, REG_LCD_0B_L, 0x200, 0x200, 0, 1);
#else
            // wait mode
            HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait even
#endif
            _HalDispIfSetWaitDonePostActForMcuFlm(u32Interface, pstCmdqCtx);
#if CMDQ_POLL_EQ_MODE // polling mode
            HalDispUtilityPollWait(pstRegWaitDoneCfg->pCmdqInf, REG_LCD_0B_L, 0x1, 0x1, 0,
                                   1); // add wait event  wait frame done status
#else
            // wait mode
            HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait event
#endif
            // Clear frame done status
            _HalDispIfSetWaitDonePostActForMcuFrameDone(u32Interface, pstCmdqCtx);
        }
    }
    else if (pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        HalDispDmaIfWaitDone(pstDispCtxCfg);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoHwInfoConfig(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e      enRet        = E_HAL_DISP_QUERY_RET_OK;
    HalDispHwInfoConfig_t *pstHwInfoCfg = (HalDispHwInfoConfig_t *)pCfg;

    if (pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_DEVICE)
    {
        pstHwInfoCfg->u32Count = HAL_DISP_DEVICE_MAX;
    }
    else if (pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_VIDEOLAYER)
    {
        pstHwInfoCfg->u32Count = HAL_DISP_VIDLAYER_MAX;
    }
    else if (pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_INPUTPORT)
    {
        if (pstHwInfoCfg->u32Id >= HAL_DISP_VIDLAYER_MAX)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, VidLayerId:%d, is out of range\n", __FUNCTION__, __LINE__, pstHwInfoCfg->u32Id);
        }
        else
        {
            if (pstHwInfoCfg->u32Id == HAL_DISP_VIDLAYER_ID_0 || pstHwInfoCfg->u32Id == HAL_DISP_VIDLAYER_ID_2
                || pstHwInfoCfg->u32Id == HAL_DISP_VIDLAYER_ID_4)
            {
                pstHwInfoCfg->u32Count = HAL_DISP_MOPG_GWIN_NUM;
            }
            else
            {
                pstHwInfoCfg->u32Count = HAL_DISP_MOPS_GWIN_NUM;
            }
        }
    }
    else if (pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_DMA)
    {
        pstHwInfoCfg->u32Count = HAL_DISP_DMA_MAX;
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Unknown Type:%d\n", __FUNCTION__, __LINE__, pstHwInfoCfg->eType);
    }
    return enRet;
}

static void _HalDispIfSetHwInfoConfig(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoClkInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkInitConfig_t *pstClkInitCfg = (HalDispClkInitConfig_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ClkInit:%d\n", __FUNCTION__, __LINE__, pstClkInitCfg->bEn);
    DISP_DBG_VAL(pstClkInitCfg);
    return enRet;
}

static void _HalDispIfSetClkInit(void *pCtx, void *pCfg)
{
    HalDispClkInitConfig_t *pstClkInitCfg = (HalDispClkInitConfig_t *)pCfg;

    HalDispClkInit(pstClkInitCfg->bEn);
}

static HalDispQueryRet_e _HalDispIfGetInfoCmdqInf(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e       enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispCmdqInfConfig_t *pstCmdqInfCfg = (HalDispCmdqInfConfig_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, CmdqInf:%px\n", __FUNCTION__, __LINE__, pstCmdqInfCfg->u32DevId,
             pstCmdqInfCfg->pCmdqInf);
    DISP_DBG_VAL(pstCmdqInfCfg);
    return enRet;
}

static void _HalDispIfSetCmdqInf(void *pCtx, void *pCfg)
{
    HalDispCmdqInfConfig_t *pstCmdqInfCfg = (HalDispCmdqInfConfig_t *)pCfg;

    if (pstCmdqInfCfg->pCmdqInf)
    {
        HalDispUtilitySetCmdqInf(pstCmdqInfCfg->pCmdqInf, pstCmdqInfCfg->u32DevId);
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoInterCfgSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e        enRet       = E_HAL_DISP_QUERY_RET_OK;
    HalDispInternalConfig_t *pstInterCfg = NULL;
    DrvDispCtxConfig_t *     pstDispCtx  = NULL;
    u32                      u32Dev;
    
    pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    pstInterCfg = (HalDispInternalConfig_t *)pCfg;
    if(pstDispCtx->pstCtxContain && pstDispCtx->pstCtxContain->bDevContainUsed[pstDispCtx->u32ContainIdx])
    {
        u32Dev = HalDispGetDeviceId(pstDispCtx, 0);
    }
    else
    {
        u32Dev = pstDispCtx->u32ContainIdx;
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_BOOTLOGO)
    {
        g_stInterCfg[u32Dev].bBootlogoEn = pstInterCfg->bBootlogoEn;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d BootLogo En:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bBootlogoEn);
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_CSC_EN)
    {
        g_stInterCfg[u32Dev].bCsCEn = pstInterCfg->bCsCEn;
        pstInterCfg->u8CsCMd                    = (pstInterCfg->bCsCEn) ? g_stInterCfg[u32Dev].u8CsCMd : 0;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, ColorCscEn:%hhd\n", __FUNCTION__, __LINE__, u32Dev,
                 pstInterCfg->bCsCEn);
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_COLORID)
    {
        g_stInterCfg[u32Dev].u8ColorId = pstInterCfg->u8ColorId < 2 ? pstInterCfg->u8ColorId : 0;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, ColorID:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].u8ColorId);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_GOPBLENDID)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support GOP Blend ID\n", __FUNCTION__, __LINE__);
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_DISP_PAT)
    {
        g_stInterCfg[u32Dev].bDispPat = pstInterCfg->bDispPat;
        g_stInterCfg[u32Dev].u8PatMd  = pstInterCfg->u8PatMd;
        pstInterCfg->bCsCEn  = (pstInterCfg->bDispPat) ? 0 : g_stInterCfg[u32Dev].bCsCEn;
        pstInterCfg->u8CsCMd = (pstInterCfg->bDispPat) ? 0 : g_stInterCfg[u32Dev].u8CsCMd;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bDispPat:%hhu PatMd:%hhu \n", __FUNCTION__, __LINE__,
                 u32Dev, g_stInterCfg[u32Dev].bDispPat, pstInterCfg->u8PatMd);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_MOP)
    {
        g_stInterCfg[u32Dev].bRstMop = pstInterCfg->bRstMop;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstMop:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstMop);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DISP)
    {
        g_stInterCfg[u32Dev].bRstDisp = pstInterCfg->bRstDisp;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstDisp:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstDisp);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DAC)
    {
        g_stInterCfg[u32Dev].bRstDac = pstInterCfg->bRstDac;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstDac:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstDac);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_HDMITX)
    {
        g_stInterCfg[u32Dev].bRstHdmitx = pstInterCfg->bRstHdmitx;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstHdmitx:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstHdmitx);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_LCD)
    {
        g_stInterCfg[u32Dev].bRstLcd = pstInterCfg->bRstLcd;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstLcd:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstLcd);
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DACAFF)
    {
        g_stInterCfg[u32Dev].bRstDacAff = pstInterCfg->bRstDacAff;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstDacAff:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstDacAff);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DACSYN)
    {
        g_stInterCfg[u32Dev].bRstDacSyn = pstInterCfg->bRstDacSyn;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstDacSyn:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstDacSyn);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_FPLL)
    {
        g_stInterCfg[u32Dev].bRstFpll = pstInterCfg->bRstFpll;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, bRstFpll:%d\n", __FUNCTION__, __LINE__, u32Dev,
                 g_stInterCfg[u32Dev].bRstFpll);
    }
    if (enRet == E_HAL_DISP_QUERY_RET_OK)
    {
        g_stInterCfg[u32Dev].eType |= pstInterCfg->eType;
    }
    return enRet;
}

static void _HalDispIfSetInterCfgSet(void *pCtx, void *pCfg)
{
    HalDispInternalConfig_t *pstInterCfg = NULL;
    DrvDispCtxConfig_t *     pstDispCtx  = NULL;
    u32                      u32Dev, u16FpllId;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = NULL;

    pstDispCtx  = (DrvDispCtxConfig_t *)pCtx;
    pstInterCfg = (HalDispInternalConfig_t *)pCfg;

    if(pstDispCtx->pstCtxContain && pstDispCtx->pstCtxContain->bDevContainUsed[pstDispCtx->u32ContainIdx])
    {
        u32Dev = HalDispGetDeviceId(pstDispCtx, 0);
        HalDispGetCmdqByCtx(pCtx, (void *)&pstCmdqCtx);
    }
    else
    {
        u32Dev = pstDispCtx->u32ContainIdx;
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Dev:%d, eType:%x\n", __FUNCTION__, __LINE__, u32Dev, pstInterCfg->eType);
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_DISP_PAT)
    {
        HalDispWaitDeviceVdeDone(pstDispCtx, u32Dev);
        HalDispSetPatGenMd(pstInterCfg->u8PatMd, pstDispCtx, u32Dev);
        HalDispSetMaceSrcSel(pstInterCfg->bDispPat ? 0 : 1, pstDispCtx, u32Dev);
        HalDispColorSetColorMatrixEn(g_stInterCfg[u32Dev].u8ColorId, pstInterCfg->bCsCEn, u32Dev, pstDispCtx);
        HalDispColorSetColorMatrixMd(g_stInterCfg[u32Dev].u8ColorId, pstInterCfg->u8CsCMd, u32Dev, pstDispCtx);
        if(pstCmdqCtx)
        {
            HalDispUtilityW2BYTEMSKDirectCmdqWrite(pstCmdqCtx);
            HalDispUtilityKickoffCmdq(pstCmdqCtx);
        }
        _HalDispIfSetInterfaceCsc(pCtx, u32Dev,HAL_DISP_COLOR_Y2RM_TO_CSCM(pstInterCfg->u8CsCMd));
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_CSC_EN)
    {
        HalDispColorSetColorMatrixEn(g_stInterCfg[u32Dev].u8ColorId, pstInterCfg->bCsCEn, u32Dev, pstDispCtx);
        HalDispColorSetColorMatrixMd(g_stInterCfg[u32Dev].u8ColorId, pstInterCfg->u8CsCMd, u32Dev, pstDispCtx);
        _HalDispIfSetInterfaceCsc(pCtx, u32Dev, HAL_DISP_COLOR_Y2RM_TO_CSCM(pstInterCfg->u8CsCMd));
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_MOP)
    {
        HalDispSetInterCfgRstMop(pstInterCfg->bRstMop, u32Dev);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DISP)
    {
        HalDispSetSwReset(pstInterCfg->bRstDisp, NULL, u32Dev);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DAC)
    {
        (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst = (pstInterCfg->bRstDac) ? HAL_DISP_DAC_RESET_FORCE : HAL_DISP_DAC_RESET_OFF);
        HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DAC, pstInterCfg->bRstDac);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_HDMITX)
    {
        HalDispSetHdmitxRst(pstInterCfg->bRstHdmitx, NULL);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_LCD)
    {
        HalDispSetInterCfgRstLcd(pstInterCfg->bRstLcd);
    }
    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DACAFF)
    {
        (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst = (pstInterCfg->bRstDacAff) ? HAL_DISP_DAC_RESET_FORCE : HAL_DISP_DAC_RESET_OFF);
        HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DACAFF, pstInterCfg->bRstDacAff);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_DACSYN)
    {
        (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacRst = (pstInterCfg->bRstDacSyn) ? HAL_DISP_DAC_RESET_FORCE : HAL_DISP_DAC_RESET_OFF);
        HaldispSetDacSwRst(E_HAL_DISP_INTER_CFG_RST_DACSYN, pstInterCfg->bRstDacSyn);
    }

    if (pstInterCfg->eType & E_HAL_DISP_INTER_CFG_RST_FPLL)
    {
        for (u16FpllId = HAL_DISP_FPLL_0_VGA_HDMI; u16FpllId < HAL_DISP_FPLL_CNT; u16FpllId++)
        {
            if (HalDispGetFpllUsedByDev(u16FpllId, u32Dev))
            {
                HalDispIrqFpllSwRst(u16FpllId, pstInterCfg->bRstFpll);
            }
        }
    }
}

static HalDispQueryRet_e _HalDispIfGetInfoInterCfgGet(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *     pstDispCtxCfg = NULL;
    HalDispQueryRet_e        enRet         = E_HAL_DISP_QUERY_RET_OK;
    HalDispInternalConfig_t *pstInterCfg   = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInterCfg   = (HalDispInternalConfig_t *)pCfg;

    memcpy(pstInterCfg, &g_stInterCfg[pstDispCtxCfg->u32ContainIdx], sizeof(HalDispInternalConfig_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, Type:%x, BootLogo:%d, ColorID:%d, GopBlendId:%d\n", __FUNCTION__,
             __LINE__, pstDispCtxCfg->u32ContainIdx, pstInterCfg->eType, pstInterCfg->bBootlogoEn, pstInterCfg->u8ColorId,
             pstInterCfg->u8GopBlendId);
    return enRet;
}

static void _HalDispIfSetInterCfgGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceVgaParamGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet            = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    HalDispDeviceParam_t *        pstVgaParm       = NULL;

    pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstVgaParm       = (HalDispDeviceParam_t *)pCfg;

    memcpy(pstVgaParm, &pstDeviceContain->stDeviceParam, sizeof(HalDispDeviceParam_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Matrix:%s, Luma:%d, Contrast:%d, Hue:%d, Sat:%d, Gain:%d Sharp:%d\n",
             __FUNCTION__, __LINE__, PARSING_HAL_CSC_MATRIX(pstVgaParm->stCsc.eCscMatrix), pstVgaParm->stCsc.u32Luma,
             pstVgaParm->stCsc.u32Contrast, pstVgaParm->stCsc.u32Hue, pstVgaParm->stCsc.u32Saturation,
             pstVgaParm->u32Gain, pstVgaParm->u32Sharpness);

    return enRet;
}

static void _HalDispIfSetDeviceVgaParamGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceHdmiParamGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet            = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    HalDispDeviceParam_t *       pstHdmiParm      = NULL;

    pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstHdmiParm      = (HalDispDeviceParam_t *)pCfg;

    memcpy(pstHdmiParm, &pstDeviceContain->stDeviceParam, sizeof(HalDispDeviceParam_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Matrix:%s, Luma:%d, Contrsat:%d, Hue:%d, Sat:%d, Sharp:%d\n", __FUNCTION__,
             __LINE__, PARSING_HAL_CSC_MATRIX(pstHdmiParm->stCsc.eCscMatrix), pstHdmiParm->stCsc.u32Luma,
             pstHdmiParm->stCsc.u32Contrast, pstHdmiParm->stCsc.u32Hue, pstHdmiParm->stCsc.u32Saturation,
             pstHdmiParm->u32Sharpness);

    return enRet;
}

static void _HalDispIfSetDeviceHdmiParamGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceLcdParamGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e          enRet            = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *       pstDispCtxCfg    = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    HalDispDeviceParam_t *        pstLcdParm       = NULL;

    pstDispCtxCfg    = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstLcdParm       = (HalDispDeviceParam_t *)pCfg;

    memcpy(pstLcdParm, &pstDeviceContain->stDeviceParam, sizeof(HalDispDeviceParam_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Matrix:%s, Luma:%d, Contrast:%d, Hue:%d, Sat:%d, Sharp:%d\n", __FUNCTION__,
             __LINE__, PARSING_HAL_CSC_MATRIX(pstLcdParm->stCsc.eCscMatrix), pstLcdParm->stCsc.u32Luma,
             pstLcdParm->stCsc.u32Contrast, pstLcdParm->stCsc.u32Hue, pstLcdParm->stCsc.u32Saturation,
             pstLcdParm->u32Sharpness);

    return enRet;
}

static void _HalDispIfSetDeviceLcdParamGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceCapabilityGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e                enRet        = E_HAL_DISP_QUERY_RET_OK;
    HalDispDeviceCapabilityConfig_t *pstDevCapCfg = NULL;

    pstDevCapCfg = (HalDispDeviceCapabilityConfig_t *)pCfg;

    if (pstDevCapCfg->u32DevId < HAL_DISP_DEVICE_MAX)
    {
        if (pstDevCapCfg->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            pstDevCapCfg->ePqType       = E_HAL_DISP_DEV_PQ_HPQ;
            pstDevCapCfg->bWdmaSupport  = HAL_DISP_DEVICE_1_SUPPORT_DMA;
            pstDevCapCfg->u32VidLayerCnt = HAL_DISP_DEVICE_1_VID_CNT;
            pstDevCapCfg->u32VidLayerStartNumber = HAL_DISP_MAPPING_VIDLAYERID_FROM_MI(HAL_DISP_VIDLAYER_ID_2);
        }
        else if (pstDevCapCfg->u32DevId == HAL_DISP_DEVICE_ID_0)
        {
            pstDevCapCfg->ePqType       = E_HAL_DISP_DEV_PQ_MACE;
            pstDevCapCfg->bWdmaSupport  = HAL_DISP_DEVICE_0_SUPPORT_DMA;
            pstDevCapCfg->u32VidLayerCnt = HAL_DISP_DEVICE_0_VID_CNT;
            pstDevCapCfg->u32VidLayerStartNumber = HAL_DISP_MAPPING_VIDLAYERID_FROM_MI(HAL_DISP_VIDLAYER_ID_0);
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, DevId is not support(%d)\n", __FUNCTION__, __LINE__, pstDevCapCfg->u32DevId);
        }

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%d, Pq:%s, WdmaSupport:%d, VideLayerCnt:%x\n", __FUNCTION__, __LINE__,
                 pstDevCapCfg->u32DevId, PARSING_HAL_DEV_PQ_TYPE(pstDevCapCfg->ePqType), pstDevCapCfg->bWdmaSupport,
                 pstDevCapCfg->u32VidLayerCnt);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevId is not support(%d)\n", __FUNCTION__, __LINE__, pstDevCapCfg->u32DevId);
    }

    return enRet;
}
HalDispQueryRet_e _HalDispIfGetInfoDeviceCvbsParamGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    HalDispDeviceParam_t *pstCvbsParm = NULL;
    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    pstCvbsParm = (HalDispDeviceParam_t *)pCfg;
    memcpy(pstCvbsParm, &pstDeviceContain->stDeviceParam, sizeof(HalDispDeviceParam_t));
    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Matrix:%s, Luma:%d, Contrsat:%d, Hue:%d, Sat:%d, Sharp:%d, bEn:%d\n",
        __FUNCTION__, __LINE__,
        PARSING_HAL_CSC_MATRIX(pstCvbsParm->stCsc.eCscMatrix),
        pstCvbsParm->stCsc.u32Luma, pstCvbsParm->stCsc.u32Contrast,
        pstCvbsParm->stCsc.u32Hue, pstCvbsParm->stCsc.u32Saturation,
        pstCvbsParm->u32Sharpness, pstCvbsParm->bEnable);
    return enRet;
}
void _HalDispIfSetDeviceCvbsParamGet(void *pCtx, void *pCfg) {}

static void _HalDispIfSetDeviceCapabilityGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoVidLayerCapabilityGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e                    enRet             = E_HAL_DISP_QUERY_RET_OK;
    HalDispVideoLayerCapabilityConfig_t *pstVidLayerCapCfg = NULL;

    pstVidLayerCapCfg = (HalDispVideoLayerCapabilityConfig_t *)pCfg;

    if (pstVidLayerCapCfg->u32VidLayerId == HAL_DISP_VIDLAYER_ID_0)
    {
        pstVidLayerCapCfg->bRotateSupport             = 0;
        pstVidLayerCapCfg->bCompressFmtSupport        = 1;
        pstVidLayerCapCfg->u32InputPortHwCnt          = 32;
        pstVidLayerCapCfg->u32InputPortPitchAlignment = 16;
        pstVidLayerCapCfg->u32RotateHeightAlig        = 32;
    }
    else if (pstVidLayerCapCfg->u32VidLayerId == HAL_DISP_VIDLAYER_ID_1)
    {
        pstVidLayerCapCfg->bRotateSupport             = 0;
        pstVidLayerCapCfg->bCompressFmtSupport        = 1;
        pstVidLayerCapCfg->u32InputPortHwCnt          = 1;
        pstVidLayerCapCfg->u32InputPortPitchAlignment = 16;
        pstVidLayerCapCfg->u32RotateHeightAlig        = 32;
    }
    else if (pstVidLayerCapCfg->u32VidLayerId == HAL_DISP_VIDLAYER_ID_2)
    {
        pstVidLayerCapCfg->bRotateSupport             = 0;
        pstVidLayerCapCfg->bCompressFmtSupport        = 1;
        pstVidLayerCapCfg->u32InputPortHwCnt          = 32;
        pstVidLayerCapCfg->u32InputPortPitchAlignment = 16;
        pstVidLayerCapCfg->u32RotateHeightAlig        = 32;
    }
    else if (pstVidLayerCapCfg->u32VidLayerId == HAL_DISP_VIDLAYER_ID_3)
    {
        pstVidLayerCapCfg->bRotateSupport             = 0;
        pstVidLayerCapCfg->bCompressFmtSupport        = 1;
        pstVidLayerCapCfg->u32InputPortHwCnt          = 1;
        pstVidLayerCapCfg->u32InputPortPitchAlignment = 16;
        pstVidLayerCapCfg->u32RotateHeightAlig        = 32;
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, UnSupport VideoLayerId:%x \n", __FUNCTION__, __LINE__, pstVidLayerCapCfg->u32VidLayerId);
    }

    if (enRet == E_HAL_DISP_QUERY_RET_OK)
    {
        DISP_DBG(DISP_DBG_LEVEL_HAL,
                 "%s %d, VidLayerType:%x, Rotate:%d, InputPorHwCnt:%d, VidlayerId:%d, PitchAlight:%d, "
                 "RotateHeightAlign:%d\n",
                 __FUNCTION__, __LINE__, pstVidLayerCapCfg->u32VidLayerId, pstVidLayerCapCfg->bRotateSupport,
                 pstVidLayerCapCfg->u32InputPortHwCnt, pstVidLayerCapCfg->u32VidLayerId,
                 pstVidLayerCapCfg->u32InputPortPitchAlignment, pstVidLayerCapCfg->u32RotateHeightAlig);
    }

    return enRet;
}

static void _HalDispIfSetVidLayerCapabilityGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDeviceInterfaceCapabilityGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e                   enRet              = E_HAL_DISP_QUERY_RET_OK;
    HalDispInterfaceCapabilityConfig_t *pstInterfaceCapCfg = NULL;

    pstInterfaceCapCfg = (HalDispInterfaceCapabilityConfig_t *)pCfg;

    if (pstInterfaceCapCfg->u32Interface == HAL_DISP_INTF_CVBS)
    {
        pstInterfaceCapCfg->u32HwCount = 1;
        memset(pstInterfaceCapCfg->bSupportTiming, 0, sizeof(u8) * E_HAL_DISP_OUTPUT_MAX);
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_PAL]  = 1;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_NTSC] = 1;
    }
    else if (pstInterfaceCapCfg->u32Interface & (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA))
    {
        pstInterfaceCapCfg->u32HwCount = 1;
        memset(pstInterfaceCapCfg->bSupportTiming, 1, sizeof(u8) * E_HAL_DISP_OUTPUT_MAX);

        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_PAL]          = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_NTSC]         = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_960H_PAL]     = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_960H_NTSC]    = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_1080I50]      = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_1080I60]      = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_1920X2160_30] = 0;
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_60] = 0;

        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_USER]         = 0;

        if (pstInterfaceCapCfg->u32Interface & HAL_DISP_INTF_VGA)
        {
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_2560X1440_30] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_2560X1440_60] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_2560X1440_50] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_60] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_30] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_29D97] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_24] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_3840X2160_25] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_1920X1440_60] = 0;
            pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_2560X1600_60] = 0;
        }
    }
    else if (pstInterfaceCapCfg->u32Interface & HAL_DISP_INTF_PNL)
    {
        pstInterfaceCapCfg->u32HwCount = 1;
        memset(pstInterfaceCapCfg->bSupportTiming, 0, sizeof(u8) * E_HAL_DISP_OUTPUT_MAX);
        pstInterfaceCapCfg->bSupportTiming[E_HAL_DISP_OUTPUT_USER] = 1;
    }
    else
    {
        pstInterfaceCapCfg->u32HwCount = 0;
        memset(pstInterfaceCapCfg->bSupportTiming, 0, sizeof(u8) * E_HAL_DISP_OUTPUT_MAX);
    }
    return enRet;
}

static void _HalDispIfSetDeviceInterfaceCapabilityGet(void *pCtx, void *pCfg) {}

static HalDispQueryRet_e _HalDispIfGetInfoDmaCapabilityGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e              enRet        = E_HAL_DISP_QUERY_RET_OK;
    HalDispDmaCapabilitytConfig_t *pstDamCapCfg = NULL;
    HalDispDmaPixelFormat_e        ePixelFmt;

    pstDamCapCfg = (HalDispDmaCapabilitytConfig_t *)pCfg;

    if (pstDamCapCfg->u32DmaId < HAL_DISP_DMA_MAX)
    {
        memset(pstDamCapCfg->bSupportCompress, 0, sizeof(u8) * E_HAL_DISP_COMPRESS_MODE_MAX);
        memset(pstDamCapCfg->u8FormatWidthAlign, 0, sizeof(u8) * E_HAL_DISP_DMA_PIX_FMT_NUM);
        pstDamCapCfg->bCompressSupport                                   = 1;
        pstDamCapCfg->eDeviceType                                        = E_HAL_DISP_DEVICE_0 | E_HAL_DISP_DEVICE_1;
        pstDamCapCfg->bSupportCompress[E_HAL_DISP_COMPRESS_MODE_NONE]    = 1;
        pstDamCapCfg->bSupportCompress[E_HAL_DISP_COMPRESS_MODE_TO_6BIT] = 1;
        for (ePixelFmt = 0; ePixelFmt < E_HAL_DISP_DMA_PIX_FMT_NUM; ePixelFmt++)
        {
            pstDamCapCfg->u8FormatWidthAlign[ePixelFmt] = HAL_DISP_SUPPORT_DMA_STRIDE_ALIGN(ePixelFmt, 0);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Dma Id(%d) is Out of Range\n", __FUNCTION__, __LINE__, pstDamCapCfg->u32DmaId);
    }

    return enRet;
}

static void _HalDispIfSetDmaCapabilityGet(void *pCtx, void *pCfg) {}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

static u8 _HalDispIfGetCallBack(DrvDispCtxConfig_t *pstDispCfg, HalDispQueryConfig_t *pstQueryCfg)
{
    memset(&pstQueryCfg->stOutCfg, 0, sizeof(HalDispQueryOutConfig_t));

    if (pstQueryCfg->stInCfg.u32CfgSize != g_pDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize)
    {
        pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Query:%s, Size %d != %d\n", __FUNCTION__, __LINE__,
                 PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType), pstQueryCfg->stInCfg.u32CfgSize,
                 g_pDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize);
    }
    else
    {
        pstQueryCfg->stOutCfg.pSetFunc = g_pDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pSetFunc;

        if (pstQueryCfg->stOutCfg.pSetFunc == NULL)
        {
            pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
            DISP_ERR("%s %d, Query:%s, SetFunc Empty\n", __FUNCTION__, __LINE__,
                     PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType));
        }
        else
        {
            pstQueryCfg->stOutCfg.enQueryRet =
                g_pDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pGetInfoFunc(pstDispCfg, pstQueryCfg->stInCfg.pInCfg);
        }
    }

    return 1;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 HalDispIfFuncParser(u8 *ps8FuncType, HalDispInternalConfig_t *pstCfg, u8 val)
{
    u8 u8Ret = 0;

    if (DrvDispOsStrCmp(ps8FuncType, "bootlogo") == 0)
    {
        pstCfg->eType       = E_HAL_DISP_INTER_CFG_BOOTLOGO;
        pstCfg->bBootlogoEn = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "colorid") == 0)
    {
        pstCfg->eType     = E_HAL_DISP_INTER_CFG_COLORID;
        pstCfg->u8ColorId = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "gopblendid") == 0)
    {
        pstCfg->eType        = E_HAL_DISP_INTER_CFG_GOPBLENDID;
        pstCfg->u8GopBlendId = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "disppat") == 0)
    {
        pstCfg->eType    = E_HAL_DISP_INTER_CFG_DISP_PAT;
        pstCfg->bDispPat = val ? 1 : 0;
        pstCfg->u8PatMd  = (val == 1) ? 0x7 : val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "disppatctx") == 0)
    {
        pstCfg->eType    = E_HAL_DISP_INTER_CFG_DISP_PAT;
        pstCfg->bDispPat = val ? 1 : 0;
        pstCfg->u8PatMd  = (val == 1) ? 0x7 : val;
        pstCfg->bCtx     = 1;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstmop") == 0)
    {
        pstCfg->eType   = E_HAL_DISP_INTER_CFG_RST_MOP;
        pstCfg->bRstMop = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstdisp") == 0)
    {
        pstCfg->eType    = E_HAL_DISP_INTER_CFG_RST_DISP;
        pstCfg->bRstDisp = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstdac") == 0)
    {
        pstCfg->eType   = E_HAL_DISP_INTER_CFG_RST_DAC;
        pstCfg->bRstDac = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rsthdmitx") == 0)
    {
        pstCfg->eType      = E_HAL_DISP_INTER_CFG_RST_HDMITX;
        pstCfg->bRstHdmitx = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstlcd") == 0)
    {
        pstCfg->eType   = E_HAL_DISP_INTER_CFG_RST_LCD;
        pstCfg->bRstLcd = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstdacaff") == 0)
    {
        pstCfg->eType      = E_HAL_DISP_INTER_CFG_RST_DACAFF;
        pstCfg->bRstDacAff = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstdacsyn") == 0)
    {
        pstCfg->eType      = E_HAL_DISP_INTER_CFG_RST_DACSYN;
        pstCfg->bRstDacSyn = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstfpll") == 0)
    {
        pstCfg->eType    = E_HAL_DISP_INTER_CFG_RST_FPLL;
        pstCfg->bRstFpll = val;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "rstdma") == 0)
    {
        pstCfg->eType = E_HAL_DISP_INTER_CFG_NONE;
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u8bRstWdma  = val;
        u8Ret         = 1;
        HalDispDmaSwRst(NULL, val ? E_HAL_DISP_DMA_RST_WOPATH_H : E_HAL_DISP_DMA_RST_WOPATH_L,
                        E_DISP_UTILITY_REG_ACCESS_CPU);
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "crc16md") == 0)
    {
        pstCfg->eType = E_HAL_DISP_INTER_CFG_NONE;
        g_stDispIrqHist.stWorkingStatus.stDmaStatus[0].u16Crc16Md  = val;
        u8Ret         = 1;
    }
    else if (DrvDispOsStrCmp(ps8FuncType, "cscen") == 0)
    {
        pstCfg->eType  = E_HAL_DISP_INTER_CFG_CSC_EN;
        pstCfg->bCsCEn = val;
    }
    return u8Ret;
}
u8 HalDispIfInit(void)
{
    void *pNull;

    if (g_bDispHwIfInit)
    {
        return 1;
    }

    pNull = NULL;
    memset(g_u8PkSramEn, 0, sizeof(u8) * HAL_DISP_DEVICE_MAX);
    memset(g_pDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t) * E_HAL_DISP_QUERY_MAX);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pGetInfoFunc = _HalDispIfGetInfoDeviceInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pSetFunc     = _HalDispIfSetDeviceInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].pGetInfoFunc = _HalDispIfGetInfoDeviceDeInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].pSetFunc     = _HalDispIfSetDeviceDeInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pGetInfoFunc = _HalDispIfGetInfoDeviceEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pSetFunc     = _HalDispIfSetDeviceEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].u16CfgSize   = sizeof(u8);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pGetInfoFunc = _HalDispIfGetInfoDeviceAttach;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pSetFunc     = _HalDispIfSetDeviceAttach;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].u16CfgSize   = sizeof(pNull);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pGetInfoFunc = _HalDispIfGetInfoDeviceDetach;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pSetFunc     = _HalDispIfSetDeviceDetach;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].u16CfgSize   = sizeof(pNull);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pGetInfoFunc = _HalDispIfGetInfoDeviceBackGroundColor;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pSetFunc     = _HalDispIfSetDeviceBackGroundColor;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].u16CfgSize   = sizeof(u32);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pGetInfoFunc = _HalDispIfGetInfoDeviceInterface;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pSetFunc     = _HalDispIfSetDeviceInterface;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].u16CfgSize   = sizeof(u32);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pGetInfoFunc = _HalDispIfGetInfoDeviceOutputTiming;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pSetFunc     = _HalDispIfSetDeviceOutputTiming;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].u16CfgSize   = sizeof(HalDispDeviceTimingInfo_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceVgaParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pSetFunc     = _HalDispIfSetDeviceVgaParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceCvbsParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pSetFunc     = _HalDispIfSetDeviceCvbsParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].pGetInfoFunc = _HalDispIfGetInfoDeviceColorTemp;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].pSetFunc     = _HalDispIfSetDeviceColorTemp;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].u16CfgSize   = sizeof(HalDispColorTemp_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceHdmiParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pSetFunc     = _HalDispIfSetDeviceHdmiParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceLcdParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].pSetFunc     = _HalDispIfSetDeviceLcdParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceGammaParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].pSetFunc     = _HalDispIfSetDeviceGammaParam;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].u16CfgSize   = sizeof(HalDispGammaParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pGetInfoFunc = _HalDispIfGetInfoDevicTimeZone;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pSetFunc     = _HalDispIfSetDeviceTimeZone;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].u16CfgSize   = sizeof(HalDispTimeZone_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].pGetInfoFunc = _HalDispIfGetInfoDevicTimeZoneConfig;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].pSetFunc     = _HalDispIfSetDeviceTimeZoneConfig;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].u16CfgSize   = sizeof(HalDispTimeZoneConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].pGetInfoFunc = _HalDispIfGetInfoDevicDisplayInfo;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].pSetFunc     = _HalDispIfSetDeviceDisplayInfo;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].u16CfgSize   = sizeof(HalDispDisplayInfo_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pGetInfoFunc = _HalDispIfGetInfoVidLayerInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pSetFunc     = _HalDispIfSetVidLayerInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pGetInfoFunc = _HalDispIfGetInfoVidLayerEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pSetFunc     = _HalDispIfSetVidLayerEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].u16CfgSize   = sizeof(u8);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerBind;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pSetFunc     = _HalDispIfSetVidLayerBind;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].u16CfgSize   = sizeof(pNull);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerUnBind;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pSetFunc     = _HalDispIfSetVidLayerUnBind;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].u16CfgSize   = sizeof(pNull);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pGetInfoFunc = _HalDispIfGetInfoVidLayerAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pSetFunc     = _HalDispIfSetVidLayerAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].u16CfgSize   = sizeof(HalDispVideoLayerAttr_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pGetInfoFunc = _HalDispIfGetInfoVidLayerCompress;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pSetFunc     = _HalDispIfSetVidLayerComporess;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].u16CfgSize   = sizeof(HalDispVideoLayerCompressAttr_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pGetInfoFunc = _HalDispIfGetInfoVidLayerPriority;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pSetFunc     = _HalDispIfSetVidLayerPriority;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].u16CfgSize   = sizeof(u32);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerBufferFire;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pSetFunc     = _HalDispIfSetVidLayerBufferFire;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerCheckFire;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pSetFunc     = _HalDispIfSetVidLayerCheckFire;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pGetInfoFunc = _HalDispIfGetInfoInputPortInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pSetFunc     = _HalDispIfSetInputPortInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pGetInfoFunc = _HalDispIfGetInfoInputPortEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pSetFunc     = _HalDispIfSetInputPortEnable;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].u16CfgSize   = sizeof(u8);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pGetInfoFunc = _HalDispIfGetInfoInputPortAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pSetFunc     = _HalDispIfSetInputPortAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].u16CfgSize   = sizeof(HalDispInputPortAttr_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pGetInfoFunc = _HalDispIfGetInfoInputPortShow;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pSetFunc     = _HalDispIfSetInputPortShow;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pGetInfoFunc = _HalDispIfGetInfoInputPortHide;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pSetFunc     = _HalDispIfSetInputPortHide;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pGetInfoFunc = _HalDispIfGetInfoInputPortBegin;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pSetFunc     = _HalDispIfSetInputPortBegin;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pGetInfoFunc = _HalDispIfGetInfoInputPortEnd;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pSetFunc     = _HalDispIfSetInputPortEnd;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].u16CfgSize   = 0;

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pGetInfoFunc = _HalDispIfGetInfoInputPortFlip;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pSetFunc     = _HalDispIfSetInputPortFlip;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].u16CfgSize   = sizeof(HalDispVideoFrameData_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pGetInfoFunc = _HalDispIfGetInfoInputPortRotate;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pSetFunc     = _HalDispIfSetInputPortRotate;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].u16CfgSize   = sizeof(HalDispInputPortRotate_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pGetInfoFunc = _HalDispIfGetInfoInputPortCrop;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pSetFunc     = _HalDispIfSetInputPortCrop;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].u16CfgSize   = sizeof(HalDispVidWinRect_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].pGetInfoFunc = _HalDispIfGetInfoInputPortRingBuffAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].pSetFunc     = _HalDispIfSetInputPortRingBuffAttr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].u16CfgSize   = sizeof(HalDispRingBuffAttr_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].pGetInfoFunc = _HalDispIfGetInfoImiStartAddr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].pSetFunc     = _HalDispIfSetImiStartAddr;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].u16CfgSize   = sizeof(ss_phys_addr_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pGetInfoFunc = _HalDispIfGetInfoClkSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pSetFunc     = _HalDispIfSetClkSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pGetInfoFunc = _HalDispIfGetInfoClkGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pSetFunc     = _HalDispIfSetClkGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].pGetInfoFunc = _HalDispIfGetInfoPqSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].pSetFunc     = _HalDispIfSetPqSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].u16CfgSize   = sizeof(HalDispPqConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].pGetInfoFunc = _HalDispIfGetInfoPqGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].pSetFunc     = _HalDispIfSetPqGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].u16CfgSize   = sizeof(HalDispPqConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].pGetInfoFunc = _HalDispIfGetInfoDrvTurningSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].pSetFunc     = _HalDispIfSetDrvTurningSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].u16CfgSize   = sizeof(HalDispDrvTurningConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].pGetInfoFunc = _HalDispIfGetInfoDrvTurningGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].pSetFunc     = _HalDispIfSetDrvTurningGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].u16CfgSize   = sizeof(HalDispDrvTurningConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].pGetInfoFunc = _HalDispIfGetInfoDbgmgGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].pSetFunc     = _HalDispIfSetDbgmgGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].u16CfgSize   = sizeof(HalDispDbgmgConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].pGetInfoFunc = _HalDispIfGetInfoRegAccess;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].pSetFunc     = _HalDispIfSetRegAccess;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].u16CfgSize   = sizeof(HalDispRegAccessConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].pGetInfoFunc = _HalDispIfGetInfoRegFlip;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].pSetFunc     = _HalDispIfSetRegFlip;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].u16CfgSize   = sizeof(HalDispRegFlipConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].pGetInfoFunc = _HalDispIfGetInfoRegWaitDone;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].pSetFunc     = _HalDispIfSetRegWaitDone;
    g_pDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].u16CfgSize   = sizeof(HalDispRegWaitDoneConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].pGetInfoFunc = _HalDispIfGetInfoHwInfoConfig;
    g_pDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].pSetFunc     = _HalDispIfSetHwInfoConfig;
    g_pDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].u16CfgSize   = sizeof(HalDispHwInfoConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].pGetInfoFunc = _HalDispIfGetInfoClkInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].pSetFunc     = _HalDispIfSetClkInit;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].u16CfgSize   = sizeof(HalDispClkInitConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].pGetInfoFunc = _HalDispIfGetInfoCmdqInf;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].pSetFunc     = _HalDispIfSetCmdqInf;
    g_pDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].u16CfgSize   = sizeof(HalDispCmdqInfConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].pGetInfoFunc = _HalDispIfGetInfoInterCfgSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].pSetFunc     = _HalDispIfSetInterCfgSet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].u16CfgSize   = sizeof(HalDispInternalConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].pGetInfoFunc = _HalDispIfGetInfoInterCfgGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].pSetFunc     = _HalDispIfSetInterCfgGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].u16CfgSize   = sizeof(HalDispInternalConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET].pGetInfoFunc = _HalDispIfGetInfoDeviceVgaParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET].pSetFunc     = _HalDispIfSetDeviceVgaParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM_GET].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET].pGetInfoFunc = _HalDispIfGetInfoDeviceHdmiParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET].pSetFunc     = _HalDispIfSetDeviceHdmiParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM_GET].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET].pGetInfoFunc = _HalDispIfGetInfoDeviceLcdParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET].pSetFunc     = _HalDispIfSetDeviceLcdParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM_GET].u16CfgSize   = sizeof(HalDispDeviceParam_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM_GET].pGetInfoFunc = _HalDispIfGetInfoDeviceCvbsParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM_GET].pSetFunc     = _HalDispIfSetDeviceCvbsParamGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM_GET].u16CfgSize   = sizeof(HalDispDeviceParam_t);
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET].pGetInfoFunc = _HalDispIfGetInfoDeviceCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET].pSetFunc     = _HalDispIfSetDeviceCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CAPABILITY_GET].u16CfgSize   = sizeof(HalDispDeviceCapabilityConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET].pGetInfoFunc = _HalDispIfGetInfoVidLayerCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET].pSetFunc     = _HalDispIfSetVidLayerCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_VIDLAYER_CAPABILITY_GET].u16CfgSize   = sizeof(HalDispVideoLayerCapabilityConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET].pGetInfoFunc =
        _HalDispIfGetInfoDeviceInterfaceCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET].pSetFunc   = _HalDispIfSetDeviceInterfaceCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_INTERFACE_CAPABILITY_GET].u16CfgSize = sizeof(HalDispInterfaceCapabilityConfig_t);

    g_pDispCbTbl[E_HAL_DISP_QUERY_DMA_CAPABILITY_GET].pGetInfoFunc = _HalDispIfGetInfoDmaCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DMA_CAPABILITY_GET].pSetFunc     = _HalDispIfSetDmaCapabilityGet;
    g_pDispCbTbl[E_HAL_DISP_QUERY_DMA_CAPABILITY_GET].u16CfgSize   = sizeof(HalDispDmaCapabilitytConfig_t);

    HalDispDmaIfInit(g_pDispCbTbl);
    g_bDispHwIfInit = 1;

    return 1;
}

u8 HalDispIfDeInit(void)
{
    if (g_bDispHwIfInit == 0)
    {
        DISP_ERR("%s %d, HalIf not init\n", __FUNCTION__, __LINE__);
        return 0;
    }
    g_bDispHwIfInit = 0;
    memset(g_pDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t) * E_HAL_DISP_QUERY_MAX);

    HalDispMopIfInputPortDeinit();
    HalDispDmaIfDeInit(NULL);

    return 1;
}

u8 HalDispIfQuery(void *pCtx, void *pCfg)
{
    u8 bRet = 1;

    if (pCtx == NULL)
    {
        DISP_ERR("%s %d, Input Ctx is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else if (pCfg == NULL)
    {
        DISP_ERR("%s %d, Input Cfg is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = _HalDispIfGetCallBack((DrvDispCtxConfig_t *)pCtx, (HalDispQueryConfig_t *)pCfg);
    }

    return bRet;
}
