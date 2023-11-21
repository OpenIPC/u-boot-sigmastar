/*
* drv_hdmitx_debug.c- Sigmastar
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

#define _HDMITX_DEBUG_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_hdmitx_os.h"
#include "mhal_hdmitx_datatype.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "drv_hdmitx_if.h"
#include "hal_hdmitx_st.h"
#include "drv_hdmitx_ctx.h"
#include "hal_hdmitx_if.h"
#include "regHDMITx.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u8  g_bPtGenEn            = 0;
u8  g_u8PtGenWidth        = 0;
u32 g_u32HdmitxDbgmgLevel = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void DrvHdmitxDebugDbgmgStore(DrvHdmitxOsStrConfig_t *pstStringCfg)
{
    if (pstStringCfg->argc < 1)
    {
        return;
    }

    DrvHdmitxOsStrToL(pstStringCfg->argv[0], 16, &g_u32HdmitxDbgmgLevel);
    DrvHdmitxIfSetDebugLevel(NULL, g_u32HdmitxDbgmgLevel);
    HDMITX_MSG("%s %d, DebugLevel=%x\n", __FUNCTION__, __LINE__, g_u32HdmitxDbgmgLevel);
}

u32 DrvHdmitxDebugDbgmgShow(u8 *DstBuf)
{
    u32 u32RetSprintf = -1;

    u32RetSprintf = HDMITX_SNPRINTF(DstBuf, PAGE_SIZE, "dbglevle: %08x \n", (u32)g_u32HdmitxDbgmgLevel);
    return u32RetSprintf;
}


void DrvHdmitxDebugPtGenStore(DrvHdmitxOsStrConfig_t *pstStringCfg)
{
    u32 u32PtGenEn;
    u32 u32PtGenWidth;

    if (pstStringCfg->argc == 2)
    {
        DrvHdmitxOsStrToL(pstStringCfg->argv[0], 10, &u32PtGenEn);
        DrvHdmitxOsStrToL(pstStringCfg->argv[1], 10, &u32PtGenWidth);

        g_bPtGenEn     = u32PtGenEn ? 1 : 0;
        g_u8PtGenWidth = (u8)u32PtGenWidth;

        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_GTEST_EN, u32PtGenEn ? mask_of_hdmi_tx_reg_gtest_en : 0,
                                  mask_of_hdmi_tx_reg_gtest_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_GTEST_MOD, u32PtGenEn ? (0x1 << shift_of_hdmi_tx_reg_gtest_mod) : 0,
                                  mask_of_hdmi_tx_reg_gtest_mod);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_STRIP_WIDTH, u32PtGenWidth << shift_of_hdmi_tx_reg_strip_width,
                                  mask_of_hdmi_tx_reg_strip_width);
    }
    else
    {
        HDMITX_MSG("[En] [Pt Wdith]\n");
    }
}

u32 DrvHdmitxDebugPtGenShow(u8 *DstBuf)
{
    u32 u32RetSprintf = -1;

    u32RetSprintf = HDMITX_SNPRINTF(DstBuf, PAGE_SIZE, "ptgen=%d %d \n", g_bPtGenEn, g_u8PtGenWidth);
    return u32RetSprintf;
}

void DrvHdmitxDebugClkStore(DrvHdmitxOsStrConfig_t *pstStringCfg)
{
    u32                  idx;
    u32                  u32Enable                          = 0;
    u32                  u32ClkRate                         = 0;
    u8 *                 p8ClkName                          = NULL;
    u8                   abClkEn[HAL_HDMITX_CLK_NUM]        = HAL_HDMITX_CLK_ON_SETTING;
    u32                  au32ClkRate[HAL_HDMITX_CLK_NUM]    = HAL_HDMITX_CLK_RATE_SETTING;
    u8                   au8ClkName[HAL_HDMITX_CLK_NUM][20] = HAL_HDMITX_CLK_NAME;
    DrvHdmitxCtxConfig_t stHdmitxCtxCfg;
    u32                  i;
    u8 *                 p8SrcBuf = NULL;
    u8 *                 u32RetSprintf = NULL;

    if (pstStringCfg->argc == HAL_HDMITX_CLK_NUM + 2)
    {
        p8ClkName = pstStringCfg->argv[0];
        DrvHdmitxOsStrToL(pstStringCfg->argv[1], 10, &u32Enable);
        for (i = 0; i < HAL_HDMITX_CLK_NUM; i++)
        {
            DrvHdmitxOsStrToL(pstStringCfg->argv[i + 2], 10, &u32ClkRate);
            au32ClkRate[i] = u32ClkRate;
        }
    }
    else if (pstStringCfg->argc == 3)
    {
        p8ClkName = pstStringCfg->argv[0];
        DrvHdmitxOsStrToL(pstStringCfg->argv[1], 10, &u32Enable);
        DrvHdmitxOsStrToL(pstStringCfg->argv[2], 10, &u32ClkRate);
    }
    else
    {
        p8SrcBuf = (u8 *)DrvHdmitxOsMemAlloc(1024 * 3);
        u32RetSprintf = p8SrcBuf;
        u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "----------------- CLK TREE  -----------------\n");
        u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "clktree [En] [Rate0 ~ Rate%d]\n", HAL_HDMITX_CLK_NUM - 1);
        u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "----------------- DRV Update -----------------\n");
        u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "[CLK Type] [En] [Rate]\n");
        u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "----------------- CLK Type ----------------- \n");
        for (i = 0; i < HAL_HDMITX_CLK_NUM; i++)
        {
            u32RetSprintf += HDMITX_SNPRINTF(u32RetSprintf, PAGE_SIZE, "%s, \n", au8ClkName[i]);
        }
        HDMITX_MSG("%s\n", p8SrcBuf);
        DrvHdmitxOsMemRelease(p8SrcBuf);
        return;
    }

    if (DrvHdmitxOsStrCmp(p8ClkName, "clktree") == 0)
    {
        if (u32Enable)
        {
            u8 au8ClkEn[HAL_HDMITX_CLK_NUM] = HAL_HDMITX_CLK_OFF_SETTING;
            if (DrvHdmitxOsSetClkOn(au8ClkEn, au32ClkRate, HAL_HDMITX_CLK_NUM) == 0)
            {
                HDMITX_ERR("%s %d, Set Clk On Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            u8 au8ClkEn[HAL_HDMITX_CLK_NUM] = HAL_HDMITX_CLK_ON_SETTING;
            if (DrvHdmitxOsSetClkOff(au8ClkEn, au32ClkRate, HAL_HDMITX_CLK_NUM) == 0)
            {
                HDMITX_ERR("%s %d, Set Clk Off Fail\n", __FUNCTION__, __LINE__);
            }
        }
    }
    else
    {
        stHdmitxCtxCfg.pstMhalCtx = (DrvHdmitxCtxHalContain_t *)DrvHdmitxOsMemAlloc(sizeof(DrvHdmitxCtxHalContain_t));

        if (stHdmitxCtxCfg.pstMhalCtx)
        {
            if (DrvHdmitxIfGetClk((void *)&stHdmitxCtxCfg, abClkEn, au32ClkRate, HAL_HDMITX_CLK_NUM)
                != E_MHAL_HDMITX_RET_SUCCESS)
            {
                HDMITX_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
            }
            else
            {
                for (idx = 0; idx < HAL_HDMITX_CLK_NUM; idx++)
                {
                    if (DrvHdmitxOsStrCmp(p8ClkName, (const char *)au8ClkName[idx]) == 0)
                    {
                        abClkEn[idx] = u32Enable ? 1 : 0, au32ClkRate[idx] = u32ClkRate;
                        if (DrvHdmitxIfSetClk((void *)&stHdmitxCtxCfg, abClkEn, au32ClkRate, HAL_HDMITX_CLK_NUM)
                            != E_MHAL_HDMITX_RET_SUCCESS)
                        {
                            HDMITX_ERR("%s %d, Set Clk Fail\n", __FUNCTION__, __LINE__);
                        }
                        break;
                    }
                }

                if (idx == HAL_HDMITX_CLK_NUM)
                {
                    HDMITX_ERR("%s %d, p8ClkName (%s) is not correct\n", __FUNCTION__, __LINE__, p8ClkName);
                }
            }
            DrvHdmitxOsMemRelease(stHdmitxCtxCfg.pstMhalCtx);
        }
        else
        {
            HDMITX_ERR("%s %d, MemeAlloc Fail\n", __FUNCTION__, __LINE__);
        }
    }
}

u32 DrvHdmitxDebugClkShow(u8 *DstBuf)
{
    u8                   abEn[HAL_HDMITX_CLK_NUM];
    u32                  au32ClkRate[HAL_HDMITX_CLK_NUM];
    u8 *                 p8SrcBuf = DstBuf;
    DrvHdmitxCtxConfig_t stHdmitxCtxCfg;
    u8                   aCLKName[][20] = {"CLK_HDMI", "CLK_DISP_432", "CLK_DISP_216"};
    u32                  i;

    stHdmitxCtxCfg.pstMhalCtx = (DrvHdmitxCtxHalContain_t *)DrvHdmitxOsMemAlloc(sizeof(DrvHdmitxCtxHalContain_t));

    if (stHdmitxCtxCfg.pstMhalCtx)
    {
        if (DrvHdmitxIfGetClk((void *)&stHdmitxCtxCfg, abEn, au32ClkRate, HAL_HDMITX_CLK_NUM)
            != E_MHAL_HDMITX_RET_SUCCESS)
        {
            HDMITX_ERR("%s %d, Get CLK Fail\n", __FUNCTION__, __LINE__);
        }
        DrvHdmitxOsMemRelease(stHdmitxCtxCfg.pstMhalCtx);
        for (i = 0; i < HAL_HDMITX_CLK_NUM; i++)
        {
            p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "%-15s: En:%d, ClkRate:%d \n", aCLKName[i], abEn[i],
                                       au32ClkRate[i]);
        }
    }
    else
    {
        HDMITX_ERR("%s %d, MemAlloc Fail\n", __FUNCTION__, __LINE__);
    }
    return (p8SrcBuf -DstBuf);
}

void DrvHdmitxDebugForceCfgStore(DrvHdmitxOsStrConfig_t *pstStringCfg)
{
    u32                        u32Value;
    HalHdmitxQueryConfig_t     stQueryCfg;
    HalHdmitxForceAttrConfig_t stForceAttrCfg;
    DrvHdmitxCtxConfig_t       stHdmitxCtxCfg;
    DrvHdmitxCtxConfig_t       *pCtx;
    u8                         bParamSet = 1;
    DrvHdmitxCtxAllocConfig_t stHdmitxCtxAllcCfg;

    if (pstStringCfg->argc >= 2)
    {
        if (!DrvHdmitxOsStrCmp(pstStringCfg->argv[0], "outputmode"))
        {
            DrvHdmitxOsStrToL(pstStringCfg->argv[1], 10, &u32Value);
            stForceAttrCfg.eFlag        = E_HAL_HDMITX_FORCE_FLAG_OUPUT_MODE;
            stForceAttrCfg.bOutputMode  = u32Value ? 1 : 0;
            stForceAttrCfg.enOutputMode = DrvHdmitxOsStrCmp(pstStringCfg->argv[2], "hdmi") == 0 ? E_MHAL_HDMITX_OUTPUT_MODE_HDMI
                                                                                     : E_MHAL_HDMITX_OUTPUT_MODE_DVI;
        }
        else if (!DrvHdmitxOsStrCmp(pstStringCfg->argv[0], "hpddetect"))
        {
            DrvHdmitxOsStrToL(pstStringCfg->argv[1], 10, &u32Value);
            stForceAttrCfg.eFlag      = E_HAL_HDMITX_FORCE_FLAG_HPD_DETECT;
            stForceAttrCfg.bHpdDetect = u32Value ? 1 : 0;
        }
        else
        {
            bParamSet = 0;
        }

        if (bParamSet)
        {
            stQueryCfg.stInCfg.enQueryType = E_HAL_HDMITX_QUERY_FORCE_CFG_SET;
            stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalHdmitxForceAttrConfig_t);
            stQueryCfg.stInCfg.pInCfg      = &stForceAttrCfg;

            HalHdmitxIfInit();
            stHdmitxCtxAllcCfg.s32HdmitxId = 0;
            if (!DrvHdmitxCtxGet(&stHdmitxCtxAllcCfg, &pCtx))
            {
                memset(&stHdmitxCtxCfg, 0, sizeof(DrvHdmitxCtxConfig_t));
                pCtx = &stHdmitxCtxCfg;
                HDMITX_ERR("%s %d: Get Ctx Fail\n", __FUNCTION__, __LINE__);
            }
            if (HalHdmitxIfQuery((void *)pCtx, &stQueryCfg))
            {
                if (stQueryCfg.stOutCfg.pSetFunc)
                {
                    stQueryCfg.stOutCfg.pSetFunc((void *)pCtx, stQueryCfg.stInCfg.pInCfg);
                }
            }
        }
    }
    else
    {
        HDMITX_MSG("outputmode [1/0] [hdmi/dvi]\n");
        HDMITX_MSG("hpddetect  [1/0]\n");
    }
}

u32 DrvHdmitxDebugForceCfgShow(u8 *DstBuf)
{
    u8 *                       p8SrcBuf = DstBuf;
    HalHdmitxQueryConfig_t     stQueryCfg;
    HalHdmitxForceAttrConfig_t stForceAttrCfg;
    DrvHdmitxCtxConfig_t       stHdmitxCtxCfg;

    stQueryCfg.stInCfg.enQueryType = E_HAL_HDMITX_QUERY_FORCE_CFG_GET;
    stQueryCfg.stInCfg.u32CfgSize  = sizeof(HalHdmitxForceAttrConfig_t);
    stQueryCfg.stInCfg.pInCfg      = &stForceAttrCfg;

    memset(&stForceAttrCfg, 0, sizeof(HalHdmitxForceAttrConfig_t));

    HalHdmitxIfInit();

    if (HalHdmitxIfQuery((void *)&stHdmitxCtxCfg, &stQueryCfg))
    {
        if (stQueryCfg.stOutCfg.pSetFunc)
        {
            stQueryCfg.stOutCfg.pSetFunc((void *)&stHdmitxCtxCfg, stQueryCfg.stInCfg.pInCfg);
        }
    }
    p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "Flag:%x, ForceOutputMode(%d, %s), HpdDetect:%x \n"
        , stForceAttrCfg.eFlag, stForceAttrCfg.bOutputMode, PARSING_HAL_OUTPUT_MODE(stForceAttrCfg.enOutputMode),
                                       stForceAttrCfg.bHpdDetect);
    return (p8SrcBuf- DstBuf);
}
u32 DrvHdmitxDebugCfgShow(u8 *DstBuf)
{
    u8 *                       p8SrcBuf = DstBuf;
    u8                         i;
    DrvHdmitxCtxConfig_t *     pDevCtx;
    HalHdmitxResourcePrivate_t *pResPriData;
    
    for(i = 0 ;i <HAL_HDMITX_DEVICE_MAX; i++)
    {
        DrvHdmitxIfGetInstance((void **)&pDevCtx, 0);
        if (pDevCtx && pDevCtx->bUsed)
        {
            pResPriData = pDevCtx->pstHwCtx;
            p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------HDMITX MHAL CFG -------\n");
            p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------DEV%hhd Init:%hhd Running:%hhd-------\n",
                i, pDevCtx->_hdmitx_status.bIsInitialized, pDevCtx->_hdmitx_status.bIsRunning);
            if (pDevCtx->pstMhalCtx)
            {
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "AVMute Von Aon Signal Status\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "%hhd %6hhd %3hhd %3hhd %6d\n",
                    pDevCtx->pstMhalCtx->bAvMute, pDevCtx->pstMhalCtx->bVideoOnOff, pDevCtx->pstMhalCtx->bAudioOnOff,
                    pDevCtx->pstMhalCtx->bSignal, pDevCtx->pstMhalCtx->enStatusFlag);
                
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "Hpd Gpio Num:%hhd \n", pDevCtx->pstMhalCtx->stHpdCfg.u8GpioNum);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "AnlogDrv Pd[%hhd %hhd %hhd] Tap1[%hhd %hhd %hhd %hhd] Tap2[%hhd %hhd %hhd %hhd]\n",
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdRtermCh, pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdLdoPreDrvCh, 
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurPdLdoMuxCh, pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch0,
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch1, pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch2,
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap1Ch3, pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch0,
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch1, pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch2,
                    pDevCtx->pstMhalCtx->stAnalogDrvCurCfg.u8DrvCurTap2Ch3);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "Vedio Cfg: [En, InC, OutC, CD, OM, Timing] = [%hhd, %s, %s, %s, %s, %s] \n",
                    pDevCtx->pstMhalCtx->stAttrCfg.bVideoEn,
                    PARSING_HAL_COLOR_FMT(pDevCtx->pstMhalCtx->stAttrCfg.enInColor),
                    PARSING_HAL_COLOR_FMT(pDevCtx->pstMhalCtx->stAttrCfg.enOutColor),
                    PARSING_HAL_COLOR_DEPTH(pDevCtx->pstMhalCtx->stAttrCfg.enColorDepth),
                    PARSING_HAL_OUTPUT_MODE(pDevCtx->pstMhalCtx->stAttrCfg.enOutputMode),
                    PARSING_HAL_TIMING(pDevCtx->pstMhalCtx->stAttrCfg.enTiming));
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "Audio Cfg: [En, Ch, Freq, Fmt, Code] = [%hhd, %s, %s, %s, %s] \n",
                    pDevCtx->pstMhalCtx->stAttrCfg.bAudioEn,
                    PARSING_HAL_AUDIO_CH(pDevCtx->pstMhalCtx->stAttrCfg.enAudioCh),
                    PARSING_HAL_AUDIO_FREQ(pDevCtx->pstMhalCtx->stAttrCfg.enAudioFreq),
                    PARSING_HAL_AUDIO_FMT(pDevCtx->pstMhalCtx->stAttrCfg.enAudioFmt),
                    PARSING_HAL_AUDIO_CODING(pDevCtx->pstMhalCtx->stAttrCfg.enAudioCode));
            }
            if(pResPriData && pResPriData->bResPriUsed)
            {
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "\n-------HDMITX HW Real Setting Task:%hhd HPD:%hhd-------\n",
                    pResPriData->bEnableHDMITxTask, pResPriData->bSetHPD);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "en tmds video audio force edidok avmute fsm Rx outmode\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "%hhd %2hhd %4hhd %5hhd %5hhd %5hhd %6hhd %6d %3d %2d\n",
                    pResPriData->stHDMITxInfo.hdmitx_enable_flag, pResPriData->stHDMITxInfo.hdmitx_tmds_flag,
                    pResPriData->stHDMITxInfo.hdmitx_video_flag, pResPriData->stHDMITxInfo.hdmitx_audio_flag,
                    pResPriData->stHDMITxInfo.hdmitx_force_mode, pResPriData->stHDMITxInfo.hdmitx_edid_ready,
                    pResPriData->stHDMITxInfo.hdmitx_avmute_flag, pResPriData->stHDMITxInfo.hdmitx_fsm_state,
                    pResPriData->stHDMITxInfo.hdmitx_preRX_status, pResPriData->stHDMITxInfo.output_mode);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "cm cd outtiming inc outc inrange outrange aspectratio\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "%d %2d %2d %9d %3d %4d %7d %8d\n",
                    pResPriData->stHDMITxInfo.colorimetry, pResPriData->stHDMITxInfo.output_colordepth_val,
                    pResPriData->stHDMITxInfo.output_video_timing, pResPriData->stHDMITxInfo.input_color,
                    pResPriData->stHDMITxInfo.output_color, pResPriData->stHDMITxInfo.input_range,
                    pResPriData->stHDMITxInfo.output_range, pResPriData->stHDMITxInfo.output_aspect_ratio);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "HpdOnTime TransmitTime ValidateEdidTime HpdRestart\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "%d %9d %12d %16d\n",
                    pResPriData->stHDMITxInfo.u32HpdOnTime, pResPriData->stHDMITxInfo.u32TransmitTime,
                    pResPriData->stHDMITxInfo.u32ValidateEdidTime, pResPriData->stHDMITxInfo.bHpdRestart);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------HDMITX AVI INFO :: Status-%d-------\n",
                    pResPriData->stInfoFrameCfg[E_HAL_HDMITX_INFOFRAM_TYPE_AVI].Define_Process);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "UserDef A0 fmt Cm ECm YQR RQR Vid AFD Scan Aspect\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "%hhd %7hhd %2d %3d %2d %3d %3d %3d %3d %3d %4d\n",
                    pResPriData->stInfoFrameCfg[E_HAL_HDMITX_INFOFRAM_TYPE_AVI].User_Define,
                    pResPriData->stAVIInfoPara.A0Value, pResPriData->stAVIInfoPara.enColorFmt,
                    pResPriData->stAVIInfoPara.enColorimetry, pResPriData->stAVIInfoPara.enExtColorimetry,
                    pResPriData->stAVIInfoPara.enYCCQuantRange, pResPriData->stAVIInfoPara.enRGBQuantRange,
                    pResPriData->stAVIInfoPara.enVidTiming, pResPriData->stAVIInfoPara.enAFDRatio,
                    pResPriData->stAVIInfoPara.enScanInfo, pResPriData->stAVIInfoPara.enAspectRatio);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------HDMITX GC PKT :: Status-%d-------\n",
                    pResPriData->stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_GC].Define_Process);
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,
                    "AVMute:%d ColorDepInfo:%d User_Define:%hhd\n",
                    pResPriData->stGCPara.enAVMute, pResPriData->stGCPara.enColorDepInfo,
                    pResPriData->stPacketCfg[E_HAL_HDMITX_PACKET_TYPE_GC].User_Define);
            }
        }
    }
    return (p8SrcBuf- DstBuf);
}
u32 DrvHdmitxDebugEdidShow(u8 *DstBuf)
{
    u8 *                       p8SrcBuf = DstBuf;
    u8                         i,edididx;
    DrvHdmitxCtxConfig_t *     pDevCtx;
    HalHdmitxResourcePrivate_t *pResPriData;

    for(i = 0 ;i <HAL_HDMITX_DEVICE_MAX; i++)
    {
        DrvHdmitxIfGetInstance((void **)&pDevCtx, 0);
        if (pDevCtx && pDevCtx->bUsed)
        {
            pResPriData = pDevCtx->pstHwCtx;
            if(pResPriData)
            {
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "------- HDMITX EDID CFG -------\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------  EDID0 -------\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15");
                for(edididx = 0;edididx<HDMITX_EDID_BLK_SIZE;edididx++)
                {
                    if(0 == edididx%16)
                    {
                        p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "\n0x%2x::",edididx);
                    }
                    p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,"%2hhd ",
                        pResPriData->stHDMITxInfo.RxEdidInfo.EdidBlk0[edididx]);
                }
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "-------  EDID1 -------\n");
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15");
                for(edididx = 0;edididx<HDMITX_EDID_BLK_SIZE;edididx++)
                {
                    if(0 == edididx%16)
                    {
                        p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "\n0x%02x::",edididx);
                    }
                    p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE,"%2hhd ",
                        pResPriData->stHDMITxInfo.RxEdidInfo.EdidBlk1[edididx]);
                }
                p8SrcBuf += HDMITX_SNPRINTF(p8SrcBuf, PAGE_SIZE, "\n");
            }
        }
    }
    return (p8SrcBuf- DstBuf);
}
