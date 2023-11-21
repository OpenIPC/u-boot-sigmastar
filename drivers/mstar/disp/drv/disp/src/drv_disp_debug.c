/*
* drv_disp_debug.c- Sigmastar
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

#define _DISP_DEBUG_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "drv_disp_if.h"
#include "hal_disp_if.h"
#include "hal_disp_irq.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "hal_disp_dma.h"
#include "drv_disp_ctx.h"
#include "hal_disp.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 g_u32DispDbgmgLevel = 0;
extern HalDispUtilityRegAccessMode_e g_eDispRegAccessMode[HAL_DISP_UTILITY_CMDQ_NUM];

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void DrvDispIfTransDeviceConfigToMHal(MHAL_DISP_DeviceConfig_t *pMhalCfg, HalDispInternalConfig_t *pstHalCfg);
static u8 _DrvDispParserFunc(u8 *ps8FuncType, MHAL_DISP_DeviceConfig_t *pstCfg, u8 val)
{
    HalDispInternalConfig_t stInterCfg;
    u8                      u8Ret = 0;

    memset(&stInterCfg, 0, sizeof(HalDispInternalConfig_t));
    u8Ret = HalDispIfFuncParser(ps8FuncType, &stInterCfg, val);
    DrvDispIfTransDeviceConfigToMHal(pstCfg, &stInterCfg);
    return u8Ret;
}
void DrvDispDebugSetDbgmgFlag(DrvDispOsStrConfig_t *pstStringCfg)
{
    u32 u32Level;

    if (pstStringCfg->argc < 1)
    {
        return;
    }

    if (pstStringCfg->argc == 1)
    {
        DrvDispOsStrToL(pstStringCfg->argv[0], 16, &u32Level);
    }
    else
    {
        DISP_MSG("dbgg [level] \n");
        DISP_MSG("DRV:       0x00000001 \n");
        DISP_MSG("HAL:       0x00000002 \n");
        DISP_MSG("IO:        0x00000004 \n");
        DISP_MSG("CTX:       0x00000008 \n");
        DISP_MSG("COLOR:     0x00000010 \n");
        DISP_MSG("IRQ:       0x00000020 \n");
        DISP_MSG("IRQ:       0x00000040 \n");
        DISP_MSG("Inter IRQ: 0x00000080 \n");
        DISP_MSG("CLK:       0x00000100 \n");
        DISP_MSG("CMDQ:       0x00000200 \n");
        DISP_MSG("FLIP:       0x00000400 \n");
        DISP_MSG("DMA:       0x00000800 \n");
        DISP_MSG("MCU:       0x00001000 \n");
        DISP_MSG("IRQ_FPLL:  0x00002000 \n");
        DISP_MSG("MOP_ORDER:0x00004000 \n");
        DISP_MSG("TIMEZONE: 0x00008000 \n");
        DISP_MSG("HPQ:       0x00010000 \n");
        DISP_MSG("HPQHL:     0x00020000 \n");
        DISP_MSG("MACE:      0x00040000 \n");
        DISP_MSG("HPQREG:    0x00080000 \n");
        DISP_MSG("HWRD:      0x00100000 \n");
        DISP_MSG("HWRDDL:    0x00200000 \n");
        DISP_MSG("WARN:       0x80000000 \n");
        return;
    }

    g_u32DispDbgmgLevel = u32Level;
    MHAL_DISP_DbgLevel((void *)&u32Level);
    DISP_MSG("DbgLevel:%08x\n", u32Level);
}

u32 DrvDispDebugGetDbgmgFlag(u8 *DstBuf)
{
    u8  u8idx;
    u8  *str = DstBuf;
    void *pDevCtx;
    HalDispQueryConfig_t stQueryCfg;
    HalDispDbgmgConfig_t stDbgmgCfg;

    str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "dbglevle: %08x \n", (u32)g_u32DispDbgmgLevel);

    for (u8idx = 0; u8idx < HAL_DISP_DEVICE_MAX; u8idx++)
    {
        MHAL_DISP_DeviceGetInstance(HAL_DISP_MAPPING_DEVICEID_FROM_MI(u8idx), &pDevCtx);
        if (pDevCtx)
        {
            memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));
            memset(&stDbgmgCfg, 0, sizeof(HalDispDbgmgConfig_t));

            stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DBGMG_GET;
            stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDbgmgConfig_t);
            stQueryCfg.stInCfg.pInCfg      = (void *)&stDbgmgCfg;

            stDbgmgCfg.pData = str;

            if (HalDispIfQuery(pDevCtx, &stQueryCfg))
            {
                if (stQueryCfg.stOutCfg.pSetFunc)
                {
                    stQueryCfg.stOutCfg.pSetFunc(pDevCtx, stQueryCfg.stInCfg.pInCfg);
                }
            }
            str = stDbgmgCfg.pData;
        }
    }

    return (str - DstBuf);
}
#if DISP_STATISTIC_EN
void DrvDispDebugCfgStore(DrvDispOsStrConfig_t *pstStringCfg) {}

u32 DrvDispDebugCfgShow(u8 *p8DstBuf)
{
    u8 *                       p8SrcBuf = p8DstBuf;
    u8                         i;
    DrvDispCtxConfig_t *       pDevCtx;
    DrvDispCtxDeviceContain_t *pDevCon;
    DrvDispCtxDmaContain_t *   pDmaCon;

    MHAL_DISP_DeviceGetInstance(HAL_DISP_MAPPING_DEVICEID_FROM_MI(0), (void **)&pDevCtx);
    if (pDevCtx && pDevCtx->pstCtxContain)
    {
        for (i = 0; i < HAL_DISP_DEVICE_MAX; i++)
        {
            if (pDevCtx->pstCtxContain->bDevContainUsed[i])
            {
                pDevCon = pDevCtx->pstCtxContain->pstDevContain[i];
                p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP CFG Con %hhd DEV%d BindVid:%x-------\n",
                    i, pDevCon->u32DevId, pDevCon->eBindVideoLayer);
                if (pDevCon)
                {
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Access Mode:%s\n",
                                                  PARSING_HAL_REG_ACCESS_TYPE(g_eDispRegAccessMode[i]));
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "En      TIMING      BGcolor       Interface\n");
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "%hhd        %s       %x            %s\n", pDevCon->bEnable,
                                                  PARSING_HAL_TMING_ID(pDevCon->eTimeType), pDevCon->u32BgColor,
                                                  PARSING_HAL_INTERFACE(pDevCon->u32Interface));
                    p8DstBuf +=
                        DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "H[syncW, BackP, Hstart, Act, Htt]: [%hu ,%hu ,%hu ,%hu ,%hu]   \n",
                                          pDevCon->stDevTimingCfg.u16HsyncWidth,
                                          pDevCon->stDevTimingCfg.u16HsyncBackPorch, pDevCon->stDevTimingCfg.u16Hstart,
                                          pDevCon->stDevTimingCfg.u16Hactive, pDevCon->stDevTimingCfg.u16Htotal);
                    p8DstBuf +=
                        DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "V[syncW, BackP, Vstart, Act, Vtt]: [%hu ,%hu ,%hu ,%hu ,%hu]   \n",
                                          pDevCon->stDevTimingCfg.u16VsyncWidth,
                                          pDevCon->stDevTimingCfg.u16VsyncBackPorch, pDevCon->stDevTimingCfg.u16Vstart,
                                          pDevCon->stDevTimingCfg.u16Vactive, pDevCon->stDevTimingCfg.u16Vtotal);
                    p8DstBuf += DISPDEBUG_SPRINTF(
                        p8DstBuf, PAGE_SIZE, "P[Fps, Vsp, Vbpp, Vap, Vfpp]: [%hu ,%u ,%u ,%u ,%u]   \n",
                        pDevCon->stDevTimingCfg.u16Fps, pDevCon->stDevTimingCfg.u32VSyncPeriod,
                        pDevCon->stDevTimingCfg.u32VBackPorchPeriod, pDevCon->stDevTimingCfg.u32VActivePeriod,
                        pDevCon->stDevTimingCfg.u32VFrontPorchPeriod);
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "SSC: [%hu ,%hu]   \n", pDevCon->stDevTimingCfg.u16SscStep,
                                                  pDevCon->stDevTimingCfg.u16SscSpan);
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "GAMMA: %hhu   \n", pDevCon->stGammaParam.bEn);
                    p8DstBuf += DISPDEBUG_SPRINTF(
                        p8DstBuf, PAGE_SIZE,
                        "CSC:[Type, Luma, Con, Hue, Sat, shp, Gain]: [%s ,%u ,%u ,%u ,%u ,%u ,%u]   \n",
                        PARSING_HAL_CSC_MATRIX(pDevCon->stDeviceParam.stCsc.eCscMatrix),
                        pDevCon->stDeviceParam.stCsc.u32Luma, pDevCon->stDeviceParam.stCsc.u32Contrast,
                        pDevCon->stDeviceParam.stCsc.u32Hue, pDevCon->stDeviceParam.stCsc.u32Saturation,
                        pDevCon->stDeviceParam.u32Sharpness, pDevCon->stDeviceParam.u32Gain);
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "CSC:%hhu   \n", pDevCon->stDeviceParam.bEnable);
                    if (DISP_OUTDEV_IS_HDMI(pDevCon->u32Interface))
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "HDMI:[Synth Dclk]: [0x%x ,%u]   \n",
                            pDevCon->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_HDMI] ,pDevCon->stDevTimingCfg.u32Dclk);
                    }
                    if (DISP_OUTDEV_IS_VGA(pDevCon->u32Interface))
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA:[Synth Dclk]: [0x%x ,%u]   \n",
                            pDevCon->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_VGA] ,pDevCon->stDevTimingCfg.u32Dclk);
                    }
                    if (DISP_OUTDEV_IS_CVBS(pDevCon->u32Interface))
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "CVBS:[Synth Dclk]: [0x%x ,%u]   \n",
                            pDevCon->u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_CVBS] ,pDevCon->stDevTimingCfg.u32Dclk);
                    }
                }
            }
        }
        for (i = 0; i < HAL_DISP_DMA_MAX; i++)
        {
            if (pDevCtx->pstCtxContain->bDmaContainUsed[i])
            {
                p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP CFG DMA Con:%hhd Dev:%d-------\n",
                    i, pDevCtx->pstCtxContain->pstDmaContain[i]->u32DmaId);
                pDmaCon = pDevCtx->pstCtxContain->pstDmaContain[i];
                if (pDmaCon)
                {
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "En        Fmt      Mode       InType\n");
                    p8DstBuf +=
                        DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "%hhd        %s    %s       %s\n", pDmaCon->stBufferAttrCfg.bEn,
                                          PARSING_HAL_DMA_PIXEL_FMT(pDmaCon->stOutputCfg.ePixelFormat),
                                          PARSING_HAL_DMA_OUTPUT_MODE(pDmaCon->stOutputCfg.eMode),
                                          PARSING_HAL_DMA_INPUT_TYPE(pDmaCon->stInputCfg.eType));
                    pDevCon = pDmaCon->pSrcDevContain;
                    if (pDevCon)
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "In[W ,H ,ID ,IF]: [%hu, %hu, %u, %s]   \n",
                                                      pDmaCon->stInputCfg.u16Width, pDmaCon->stInputCfg.u16Height,
                                                      pDevCon->u32DevId, PARSING_HAL_INTERFACE(pDevCon->u32Interface));
                    }
                    pDevCon = pDmaCon->pDestDevContain;
                    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Out[W ,H]: [%hu ,%hu]   \n", pDmaCon->stOutputCfg.u16Width,
                                                  pDmaCon->stOutputCfg.u16Height);
                    if (pDevCon)
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Out[ID ,IF]: [%u ,%s]   \n",
                                                      pDevCon->u32DevId,
                                                      PARSING_HAL_INTERFACE(pDevCon->u32Interface));
                    }
                    else
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Output data to dram only   \n");
                    }
                    p8DstBuf +=
                        DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Stride: [%u ,%u]   \n", pDmaCon->stBufferAttrCfg.u32Stride[0],
                                          pDmaCon->stBufferAttrCfg.u32Stride[1]);
                    p8DstBuf += DISPDEBUG_SPRINTF(
                        p8DstBuf, PAGE_SIZE, "Addr: [0x%llx ,0x%llx ,0x%llx]   \n", pDmaCon->stBufferAttrCfg.paPhyAddr[0],
                        pDmaCon->stBufferAttrCfg.paPhyAddr[1], pDmaCon->stBufferAttrCfg.paPhyAddr[2]);
                    if (pDmaCon->stOutputCfg.eMode == E_HAL_DISP_DMA_OUTPUT_MODE_RING)
                    {
                        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "RING[BH ,StL ,Fidx]: [%hu ,%hu ,%hu]   \n",
                                                      pDmaCon->stBufferAttrCfg.u16RingBuffHeight,
                                                      pDmaCon->stBufferAttrCfg.u16RingStartLine,
                                                      pDmaCon->stBufferAttrCfg.u16FrameIdx);
                    }
                }
            }
        }
        for (i = 0; i < HAL_DISP_FPLL_CNT; i++)
        {
            if (g_stDispIrqHist.stWorkingStatus.stFpllStatus[i].u8FpllEn)
            {
                p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP CFG FPLL%hhd-------\n", i);
                p8DstBuf += DISPDEBUG_SPRINTF(
                    p8DstBuf, PAGE_SIZE, "-------FPLL [Locked LockIrq UnLockIrq]: [%hhd %hhd %hhd]-------\n",
                    g_stDispIrqHist.stWorkingStatus.stFpllStatus[i].u8FpllLocked, g_stDispIrqHist.stWorkingStatus.stFpllStatus[i].u8FpllLockedIrqEn,
                    g_stDispIrqHist.stWorkingStatus.stFpllStatus[i].u8FpllUnLockIrqEn);
            }
        }
    }
    return (u32)(p8DstBuf - p8SrcBuf);
}
#endif
void DrvDispDebugSetFunc(DrvDispOsStrConfig_t *pstStringCfg)
{
    u32                      val;
    u32                      u32DevId;
    MHAL_DISP_DeviceConfig_t stCfg;

    if (pstStringCfg->argc == 3)
    {
        DrvDispOsStrToL(pstStringCfg->argv[2], 10, &u32DevId);
        DrvDispOsStrToL(pstStringCfg->argv[1], 10, &val);
        if (!_DrvDispParserFunc(pstStringCfg->argv[0], &stCfg, val))
        {
            DrvDispIfSetDeviceConfig(u32DevId, &stCfg);
        }
    }
    else
    {
        DISP_MSG("[func] [value] [Id]  \n");
        return;
    }
}

u32 DrvDispDebugFuncShow(u8 *p8DstBuf)
{
    u8 *                     p8SrcBuf = p8DstBuf;
    u8                       u8DevIdx;
    MHAL_DISP_DeviceConfig_t stDevCfg;

    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP FUNC SHOW-------\n");
    p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "echo [func] [value] [Id] > func \n");
    for (u8DevIdx = 0; u8DevIdx < HAL_DISP_DEVICE_MAX; u8DevIdx++)
    {
        DrvDispIfGetDeviceConfig(u8DevIdx, &stDevCfg);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP FUNC DEV%hhd-------\n", u8DevIdx);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "disppat::%hhu\n", stDevCfg.bDispPat);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Test Pattern Md:%hhu\n", stDevCfg.u8PatMd);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "bootlogo::%hhu\n", stDevCfg.bBootlogoEn);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "colorid::%hhu\n", stDevCfg.u8ColorId);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "cscen::%hhu\n", stDevCfg.bCscEn);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "gopblendid::%hhu\n", stDevCfg.u8GopBlendId);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstdac::%hhu\n", stDevCfg.bRstDac);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstdacaff::%hhu\n", stDevCfg.bRstDacAff);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstdacsyn::%hhu\n", stDevCfg.bRstDacSyn);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstdisp::%hhu\n", stDevCfg.bRstDisp);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstfpll::%hhu\n", stDevCfg.bRstFpll);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rsthdmitx::%hhu\n", stDevCfg.bRstHdmitx);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstlcd::%hhu\n", stDevCfg.bRstLcd);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstmop::%hhu\n", stDevCfg.bRstMop);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "mopythrd::%hhu\n", stDevCfg.u8MopYThrd);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "mopcthrd::%hhu\n", stDevCfg.u8MopCThrd);

    }
    for (u8DevIdx = 0; u8DevIdx < HAL_DISP_DMA_MAX; u8DevIdx++)
    {
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DMA%hhd FUNC MISC-------\n",u8DevIdx);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "rstdma::%hhu\n", g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u8bRstWdma);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "crc16md::%hu\n", g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u16Crc16Md);
    }
    return (u32)(p8DstBuf - p8SrcBuf);
}
#if DISP_STATISTIC_EN
void      DrvDispDebugIntsStore(DrvDispOsStrConfig_t *pstStringCfg) {}

u32 DrvDispDebugIntsShow(u8 *p8DstBuf)
{
    u8 *p8SrcBuf = p8DstBuf;
    u8  u8DevIdx;

    for (u8DevIdx = 0; u8DevIdx < HAL_DISP_DEVICE_MAX; u8DevIdx++)
    {
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP INTS DEV%hhd-------\n", u8DevIdx);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "MOP VS CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stDevHist[u8DevIdx].u32VsCnt,
                              g_stDispIrqHist.stWorkingHist.stDevHist[u8DevIdx].u64TimeStamp);
        p8DstBuf += DISPDEBUG_SPRINTF(
            p8DstBuf, PAGE_SIZE, "Dev Vde CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u32VDeDoneCnt,
            g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_VDE_NEGATIVE]);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Dev Change Timing Cnt:%u  @:%llu\n", g_stDispIrqHist.stWorkingHist.stDevHist[u8DevIdx].u32ChangeTimingCnt,
                              g_stDispIrqHist.stWorkingHist.stDevHist[u8DevIdx].u64ChangeTimingTimeStamp);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "Dev  Fps        [%u.%u]\n",
                                      g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u32fps / 10,
                                      ((u32)g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u32fps) % 10);
    }
    for (u8DevIdx = 0; u8DevIdx < HAL_DISP_DMA_MAX; u8DevIdx++)
    {
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP INTS DMA%hhd idle:%hhd-------\n", u8DevIdx, g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u8bDmaIdle);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA Trig CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32FrTrigCnt,
                              g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_TRIG]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA ACTIVE  TIME: [%llu]\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_ACTIVETIME]);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA Done CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32FrDoneCnt,
                              g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_REALDONE]);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA AFF CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32affCnt,
                              g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_FIFOFULL]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA SetErr CNT:%u     @:%llu\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32SetFailCnt,
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_REGSETFAIL]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA DVs  CNT:%u     @:%llu\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32DoubleVsyncCnt,
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_DOUBLEVSYNC]);
        p8DstBuf += DISPDEBUG_SPRINTF(
            p8DstBuf, PAGE_SIZE, "DMA Rst CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32DmaRstCnt,
            g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_RST]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA Cmd Empty CNT:%u     @:%llu\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32CmdFIFOEmptyCnt,
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_CMDEMPTY]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA OverWrite CNT:%u     @:%llu\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32OverWriteCnt,
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_OVERWRITE]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA Trig Fail CNT:%u     @:%llu\n",
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u32TrigFailCnt,
                                      g_stDispIrqHist.stWorkingHist.stDmaHist[u8DevIdx].u64TimeStamp[E_HAL_DISP_IRQ_DMA_TRIGFAIL]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "DMA VCnt : [%hu]\n", HalDispDmaGetFrmVCnt(u8DevIdx, 1));
        p8DstBuf += DISPDEBUG_SPRINTF(
            p8DstBuf, PAGE_SIZE, "DMA CRC16 [Y U V] = [%x %x %x]\n", g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u32CRC16[0],
            g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u32CRC16[1], g_stDispIrqHist.stWorkingStatus.stDmaStatus[u8DevIdx].u32CRC16[2]);
    }
    if (g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8Vgaused || g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacSwEn)
    {
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP INTS VGA-------\n");
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA HPD ON CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stVgaHist.u32HPDonCnt,
                                      g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_HPD_ON]);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA HPD OFF CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stVgaHist.u32HPDoffCnt,
                              g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_HPD_OFF]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA AFF CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stVgaHist.u32affCnt,
                                      g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOFULL]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA AFE CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stVgaHist.u32afeCnt,
                                      g_stDispIrqHist.stWorkingHist.stVgaHist.u64TimeStamp[E_HAL_DISP_IRQ_VGA_FIFOEMPTY]);
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "VGA RSTCNT:%u DacMute:%hhd DacBindDev:%u\n",
                                      g_stDispIrqHist.stWorkingHist.stVgaHist.u32DacRstCnt, g_stDispIrqHist.stWorkingStatus.stVgaStatus.u8DacMute,
                                      g_stDispIrqHist.stWorkingStatus.stVgaStatus.u32DacBindDev);
    }
    if (g_stDispIrqHist.stWorkingStatus.stCvbsStatus.u8Cvsbused)
    {
        p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP INTS CVBS-------\n");
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "CVBS HPD ON CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stCvbsHist.u32HPDonCnt,
                              g_stDispIrqHist.stWorkingHist.stCvbsHist.u64TimeStamp[E_HAL_DISP_IRQ_CVBS_HPD_ON]);
        p8DstBuf +=
            DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "CVBS HPD OFF CNT:%u     @:%llu\n", g_stDispIrqHist.stWorkingHist.stCvbsHist.u32HPDoffCnt,
                              g_stDispIrqHist.stWorkingHist.stCvbsHist.u64TimeStamp[E_HAL_DISP_IRQ_CVBS_HPD_OFF]);
    }
    for (u8DevIdx = 0; u8DevIdx < HAL_DISP_FPLL_CNT; u8DevIdx++)
    {
        if (g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].u8FpllEn)
        {
            p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "-------DISP INTS FPLL%hhd-------\n", u8DevIdx);
            p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "FPLL Irq Cnt: [LockIrq UnLockIrq]: [%u %u]\n",
                                          g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u32FplllockCnt,
                                          g_stDispIrqHist.stWorkingHist.stTimeZoneHist[u8DevIdx].u32FpllUnlockCnt);
            p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "FPLL Lost CNT:%u     Rst CNT:%u  Phase Rst:%u\n",
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u32FpllLostLockCnt,
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u32UnLockRstCnt,
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u32PhaseErrRstCnt);
            p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "FPLL Lost Time:%llu     Rst Time:%llu  Phase Rst Time:%llu Stable Time:%llu\n",
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u64FpllLostLockTime,
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u64UnLockRstTime,
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u64PhaseErrRstTime,
                                          g_stDispIrqHist.stWorkingHist.stFpllHist[u8DevIdx].u64FpllStableTime);
            p8DstBuf +=
                DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "FPLL Stm:%u     %s\n", g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].enFpllStm,
                                  PARSING_FPLL_STM(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].enFpllStm));
            if (DISP_IRQ_FPLL_STM_LOCKING(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].enFpllStm)
                || DISP_IRQ_FPLL_STM_LOCKED(g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].enFpllStm))
            {
                p8DstBuf += DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "FPLL UnstableFrmCnt:%u     LockedFrmCnt:%u\n",
                                              g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].u32UnstableFrmCnt,
                                              g_stDispIrqHist.stWorkingStatus.stFpllStatus[u8DevIdx].u32LockedFrmCnt);
            }
        }
    }
    return (u32)(p8DstBuf - p8SrcBuf);
}
#endif
void DrvDispDebugSetClk(DrvDispOsStrConfig_t *pstStringCfg)
{
    u32                u32idx;
    u32                u32Enable  = 0;
    u32                u32ClkRate = 0;
    DrvDispCtxConfig_t stDispCtx;
    u8 *               p8ClkName   = NULL;
    u8 *               pbClkEn     = NULL;
    u32 *              pu32ClkRate = NULL;
    u32                u32i;
    u8                 bParamSet = 1;

    if (pstStringCfg->argc < 1)
    {
        return;
    }

    if (E_HAL_DISP_CLK_NUM)
    {
        pbClkEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);
        pu32ClkRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);

        if (pstStringCfg->argc == E_HAL_DISP_CLK_NUM + 2)
        {
            p8ClkName = pstStringCfg->argv[0];
            DrvDispOsStrToL(pstStringCfg->argv[1], 10, &u32Enable);
            for (u32i = 0; u32i < E_HAL_DISP_CLK_NUM; u32i++)
            {
                DrvDispOsStrToL(pstStringCfg->argv[u32i + 2], 10, &u32ClkRate);
                pu32ClkRate[u32i] = u32ClkRate;
            }
        }
        else if (pstStringCfg->argc == 3)
        {
            p8ClkName = pstStringCfg->argv[0];
            DrvDispOsStrToL(pstStringCfg->argv[1], 10, &u32Enable);
            DrvDispOsStrToL(pstStringCfg->argv[2], 10, &u32ClkRate);
        }
        else
        {
            u8 *p8SrcBuf = NULL;
            u8 *str = NULL;
            p8SrcBuf     = (u8 *)DrvDispOsMemAlloc(1024 * 3);
            str = p8SrcBuf;
            str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "----------------- CLK TREE  -----------------\n");
            str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "clktree [En] [Value0 ~ %d]\n", E_HAL_DISP_CLK_NUM - 1);
            str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "----------------- DRV Update -----------------\n");
            str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "[CLK Type] [En] [Value]\n");
            str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "----------------- CLK Type ----------------- \n");
            for (u32i = 0; u32i < E_HAL_DISP_CLK_NUM; u32i++)
            {
                str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "%s, %s\n", HAL_DISP_CLK_GET_NAME(u32i),
                                       HAL_DISP_CLK_GET_MUX_ATTR(u32i) ? "CLKIdx" : "u32ClkRate");
            }
            DISP_MSG("%s\n", p8SrcBuf);
            DrvDispOsMemRelease(p8SrcBuf);
            bParamSet = 0;
        }

        if (bParamSet)
        {
            if (DrvDispOsStrCmp(p8ClkName, "clktree") == 0)
            {
                if (u32Enable)
                {
                    for (u32i = 0; u32i < E_HAL_DISP_CLK_NUM; u32i++)
                    {
                        pbClkEn[u32i] = HAL_DISP_CLK_GET_OFF_SETTING(u32i);
                    }

                    if (DrvDispOsSetClkOn(pbClkEn, pu32ClkRate, E_HAL_DISP_CLK_NUM) == 0)
                    {
                        DISP_ERR("%s %d, Set Clk On Fail\n", __FUNCTION__, __LINE__);
                    }
                }
                else
                {
                    for (u32i = 0; u32i < E_HAL_DISP_CLK_NUM; u32i++)
                    {
                        pbClkEn[u32i] = HAL_DISP_CLK_GET_ON_SETTING(u32i);
                    }
                    if (DrvDispOsSetClkOff(pbClkEn, pu32ClkRate, E_HAL_DISP_CLK_NUM) == 0)
                    {
                        DISP_ERR("%s %d, Set Clk Off Fail\n", __FUNCTION__, __LINE__);
                    }
                }
            }
            else
            {
                if (DrvDispIfGetClk((void *)&stDispCtx, pbClkEn, pu32ClkRate, E_HAL_DISP_CLK_NUM) == 0)
                {
                    DISP_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
                }
                else
                {
                    for (u32idx = 0; u32idx < E_HAL_DISP_CLK_NUM; u32idx++)
                    {
                        if (DrvDispOsStrCmp(p8ClkName, HAL_DISP_CLK_GET_NAME(u32idx)) == 0)
                        {
                            pbClkEn[u32idx] = u32Enable, pu32ClkRate[u32idx] = u32ClkRate;
                            if (DrvDispIfSetClk((void *)&stDispCtx, pbClkEn, pu32ClkRate, E_HAL_DISP_CLK_NUM) == 0)
                            {
                                DISP_ERR("%s %d, Set Clk Fail\n", __FUNCTION__, __LINE__);
                            }
                            break;
                        }
                    }

                    if (u32idx == E_HAL_DISP_CLK_NUM)
                    {
                        DISP_ERR("%s %d, p8ClkName (%s) is not correct\n", __FUNCTION__, __LINE__, p8ClkName);
                    }
                }
            }
        }

        if (pbClkEn)
        {
            DrvDispOsMemRelease(pbClkEn);
        }
        if (pu32ClkRate)
        {
            DrvDispOsMemRelease(pu32ClkRate);
        }
    }
}

u32 DrvDispDebugGetClk(u8 *p8DstBuf)
{
    u8 *               pbClkEn       = NULL;
    u32 *              pu32ClkRate   = NULL;
    DrvDispCtxConfig_t stDispCtx;
    u32                i;
    u8  *str = p8DstBuf;

    if (E_HAL_DISP_CLK_NUM)
    {
        pbClkEn     = DrvDispOsMemAlloc(sizeof(u8) * E_HAL_DISP_CLK_NUM);
        pu32ClkRate = DrvDispOsMemAlloc(sizeof(u32) * E_HAL_DISP_CLK_NUM);

        if (pbClkEn && pu32ClkRate)
        {
            if (DrvDispIfGetClk((void *)&stDispCtx, pbClkEn, pu32ClkRate, E_HAL_DISP_CLK_NUM) == 0)
            {
                DISP_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
            }

            for (i = 0; i < E_HAL_DISP_CLK_NUM; i++)
            {
                str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "CLK_%-15s: En:%d, ClkRate:%d \n", HAL_DISP_CLK_GET_NAME(i),
                                       pbClkEn[i], pu32ClkRate[i]);
            }

        }

        if (pbClkEn)
        {
            DrvDispOsMemRelease(pbClkEn);
        }
        if (pu32ClkRate)
        {
            DrvDispOsMemRelease(pu32ClkRate);
        }
    }
    return (str - p8DstBuf);
}

void DrvDispDebugSetPq(DrvDispOsStrConfig_t *pstStringCfg)
{
    u8 *                  pFileName;
    DrvDispOsTextConfig_t stTxtCfg;
    HalDispQueryConfig_t  stQueryCfg;
    HalDispPqConfig_t     stPqCfg;
    DrvDispCtxConfig_t    stDispCtx;
    u32                   u32PqType;

    if (pstStringCfg->argc < 1)
    {
        return;
    }

    if (pstStringCfg->argc == 2)
    {
        pFileName = pstStringCfg->argv[0];

        DrvDispOsStrToL(pstStringCfg->argv[1], 16, &u32PqType);
    }
    else
    {
        u32PqType = 0x112233;
    }

    if (u32PqType == 0x112233)
    {
        u8 *p8SrcBuf;

        p8SrcBuf = (u8 *)DrvDispOsMemAlloc(1024 * 10);
        if (p8SrcBuf)
        {
            memset(&stPqCfg, 0, sizeof(HalDispPqConfig_t));
            memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

            stPqCfg.pData                  = (void *)p8SrcBuf;
            stPqCfg.u32PqFlags             = 0x112233;
            stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_PQ_GET;
            stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispPqConfig_t);
            stQueryCfg.stInCfg.pInCfg      = (void *)&stPqCfg;

            HalDispIfInit();
            if (HalDispIfQuery((void *)&stDispCtx, &stQueryCfg))
            {
                if (stQueryCfg.stOutCfg.pSetFunc)
                {
                    stQueryCfg.stOutCfg.pSetFunc((void *)&stDispCtx, stQueryCfg.stInCfg.pInCfg);
                }
            }

            DISP_MSG("%s", p8SrcBuf);
            DrvDispOsMemRelease(p8SrcBuf);
        }
    }
    else
    {
        DrvDispOsParsingTxt(pFileName, &stTxtCfg);

        memset(&stPqCfg, 0, sizeof(HalDispPqConfig_t));
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stPqCfg.pData      = (void *)&stTxtCfg;
        stPqCfg.u32PqFlags = u32PqType;

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_PQ_SET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispPqConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stPqCfg;

        HalDispIfInit();

        if (HalDispIfQuery((void *)&stDispCtx, &stQueryCfg))
        {
            if (stQueryCfg.stOutCfg.pSetFunc)
            {
                stQueryCfg.stOutCfg.pSetFunc((void *)&stDispCtx, stQueryCfg.stInCfg.pInCfg);
            }
        }
        DrvDispOsMemRelease(stTxtCfg.pstItem);
    }
}

u32 DrvDispDebugGetPqBinName(u8 *p8DstBuf)
{
    HalDispQueryConfig_t stQueryCfg;
    HalDispPqConfig_t    stPqCfg;
    DrvDispCtxConfig_t   stDispCtx;
    u32                  u32RetSprintf = -1;
    u8 *                 p8SrcBuf;

    p8SrcBuf = (u8 *)DrvDispOsMemAlloc(1024 * 10);

    if (p8SrcBuf)
    {
        memset(&stPqCfg, 0, sizeof(HalDispPqConfig_t));
        memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

        stPqCfg.pData = (void *)p8SrcBuf;

        stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_PQ_GET;
        stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispPqConfig_t);
        stQueryCfg.stInCfg.pInCfg      = (void *)&stPqCfg;

        HalDispIfInit();
        if (HalDispIfQuery((void *)&stDispCtx, &stQueryCfg))
        {
            if (stQueryCfg.stOutCfg.pSetFunc)
            {
                stQueryCfg.stOutCfg.pSetFunc((void *)&stDispCtx, stQueryCfg.stInCfg.pInCfg);
            }
        }

        u32RetSprintf = DISPDEBUG_SPRINTF(p8DstBuf, PAGE_SIZE, "%s", p8SrcBuf);
        DrvDispOsMemRelease(p8SrcBuf);
    }
    return u32RetSprintf;
}

void DrvDispDebugSetTurnDrv(DrvDispOsStrConfig_t *pstStringCfg)
{
    u32                       val;
    HalDispQueryConfig_t      stQueryCfg;
    HalDispDrvTurningConfig_t stDacTrimCfg;
    DrvDispCtxConfig_t        stDispCtx;

    memset(&stDacTrimCfg, 0, sizeof(HalDispDrvTurningConfig_t));
    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    if (pstStringCfg->argc == 2 && DrvDispOsStrCmp(pstStringCfg->argv[0], "vga_drv") == 0)
    {
        DrvDispOsStrToL(pstStringCfg->argv[1], 10, &val);
        stDacTrimCfg.enType     = E_HAL_DISP_DRV_TURNING_RGB;
        stDacTrimCfg.u16Trim[0] = val;
        stDacTrimCfg.u16Trim[1] = val;
        stDacTrimCfg.u16Trim[2] = val;
    }
    else
    {
        DISP_MSG("vga_drv [value] \n");
        return;
    }

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DRVTURNING_SET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDrvTurningConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stDacTrimCfg;

    HalDispIfInit();

    if (HalDispIfQuery((void *)&stDispCtx, &stQueryCfg))
    {
        if (stQueryCfg.stOutCfg.pSetFunc)
        {
            stQueryCfg.stOutCfg.pSetFunc((void *)&stDispCtx, stQueryCfg.stInCfg.pInCfg);
        }
    }
}

u32 DrvDispDebugGetTurnDrvInfo(u8 *p8DstBuf)
{
    HalDispQueryConfig_t      stQueryCfg;
    HalDispDrvTurningConfig_t stDacTrimCfg;
    DrvDispCtxConfig_t        stDispCtx;
    u8  *str = p8DstBuf;

    memset(&stDacTrimCfg, 0, sizeof(HalDispDrvTurningConfig_t));
    memset(&stQueryCfg, 0, sizeof(HalDispQueryConfig_t));

    stQueryCfg.stInCfg.enQueryType = E_HAL_DISP_QUERY_DRVTURNING_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalDispDrvTurningConfig_t);
    stQueryCfg.stInCfg.pInCfg      = (void *)&stDacTrimCfg;

    HalDispIfInit();
    if (HalDispIfQuery((void *)&stDispCtx, &stQueryCfg))
    {
        if (stQueryCfg.stOutCfg.pSetFunc)
        {
            stQueryCfg.stOutCfg.pSetFunc((void *)&stDispCtx, stQueryCfg.stInCfg.pInCfg);
        }
    }
    str += DISPDEBUG_SPRINTF(str, PAGE_SIZE, "Vga Drv: 0x%02x 0x%02x 0x%02x \n", stDacTrimCfg.u16Trim[0],
                           stDacTrimCfg.u16Trim[1], stDacTrimCfg.u16Trim[2]);
    return (str - p8DstBuf);
}
void DrvDispDebugInit(void)
{
}
