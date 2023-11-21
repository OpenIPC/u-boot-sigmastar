/*
* hal_disp.c- Sigmastar
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

#define _HAL_DISP_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "hal_disp_op2.c"
#include "hal_disp_mace.c"
#include "hal_disp_color.c"
#include "hal_disp_mop.c"
#include "hal_disp_mop_if.c"
#include "hal_disp_utility.c"
#include "hal_disp_hpq.c"
#include "hal_disp_clk.c"
#include "hal_disp_dma.c"
#include "hal_disp_dma_if.c"
#include "hal_disp_picture_if.c"
#include "hal_disp_vga_timing_tbl.c"
#include "hal_disp_cvbs_timing_tbl.c"
#include "hal_hdmitx_top_tbl.c"

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "drv_disp_ctx.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp.h"
#include "hal_disp_st.h"
#include "hal_disp_irq.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HalDispGetCmdqCtxByDevId(DrvDispCtxConfig_t *pstDispCtx, u32 u32DevId, void **pCmdqCtx)
{
    s32 s32UtilityId = -1;

    if (HalDispGetUtilityIdByDevId(u32DevId, &s32UtilityId))
    {
        *pCmdqCtx = pstDispCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[s32UtilityId];
    }
    else
    {
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, UnSupport DevId %d \n", __FUNCTION__, __LINE__, u32DevId);
    }
}

static void _HalDispGetCmdqCtxByDmaId(DrvDispCtxConfig_t *pstDispCtx, u32 u32DmaId, void **pCmdqCtx)
{
    s32 s32UtilityId = -1;

    if (HalDispGetUtilityIdByDmaId(u32DmaId, &s32UtilityId))
    {
        *pCmdqCtx = pstDispCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[s32UtilityId];
    }
    else
    {
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, UnSupport DmaId %d \n", __FUNCTION__, __LINE__, u32DmaId);
    }
}
//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------

static u8 _HalDispGetCmdqByDevCtx(DrvDispCtxConfig_t *pstDevCtx, void **pCmdqCtx)
{
    u8                         bRet          = 1;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDevContain = pstDevCtx->pstCtxContain->pstDevContain[pstDevCtx->u32ContainIdx];
    _HalDispGetCmdqCtxByDevId(pstDevCtx, pstDevContain->u32DevId, pCmdqCtx);
    if (*pCmdqCtx == NULL)
    {
        bRet = 0;
        DISP_ERR("%s %d, ContainId%d, DevId:%d No Cmdq Handler Init\n", __FUNCTION__, __LINE__, pstDevCtx->u32ContainIdx,
                 pstDevContain->u32DevId);
    }

    return bRet;
}

static u8 _HalDispGetCmdqByVideoLayerCtx(DrvDispCtxConfig_t *pstVidlayerCtx, void **pCmdqCtx)
{
    u8                             bRet                 = 1;
    DrvDispCtxDeviceContain_t *    pstDevContain        = NULL;
    DrvDispCtxVideoLayerContain_t *pstVideoLayerContain = NULL;

    pstVideoLayerContain = pstVidlayerCtx->pstCtxContain->pstVidLayerContain[pstVidlayerCtx->u32ContainIdx];
    pstDevContain        = (DrvDispCtxDeviceContain_t *)pstVideoLayerContain->pstDevCtx;

    if (pstDevContain)
    {
        _HalDispGetCmdqCtxByDevId(pstVidlayerCtx, pstDevContain->u32DevId, pCmdqCtx);

        if (*pCmdqCtx == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, ContainId:%d, VideoLayerId:%d, DevId:%d No Cmdq Handler Init\n", __FUNCTION__, __LINE__,
                     pstVidlayerCtx->u32ContainIdx, pstVideoLayerContain->u32VidLayerId, pstDevContain->u32DevId);
        }
    }
    else
    {
        *pCmdqCtx = NULL;
        bRet      = 0;
        DISP_ERR("%s %d, VideoLayer(%d) No Bind Device \n", __FUNCTION__, __LINE__, pstVidlayerCtx->u32ContainIdx);
    }

    return bRet;
}

static u8 _HalDispGetCmdqByInputPortCtx(DrvDispCtxConfig_t *pstInputPortCtx, void **pCmdqCtx)
{
    u8                             bRet                = 1;
    DrvDispCtxDeviceContain_t *    pstDevContain       = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain  = NULL;
    DrvDispCtxInputPortContain_t * pstInputPortContain = NULL;

    pstInputPortContain = pstInputPortCtx->pstCtxContain->pstInputPortContain[pstInputPortCtx->u32ContainIdx];
    pstVidLayerContain  = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    if (pstVidLayerContain)
    {
        pstDevContain = (DrvDispCtxDeviceContain_t *)pstVidLayerContain->pstDevCtx;
        if (pstDevContain)
        {
            _HalDispGetCmdqCtxByDevId(pstInputPortCtx, pstDevContain->u32DevId, pCmdqCtx);

            if (*pCmdqCtx == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, ContainId:%d, InputportId:%d, VideoayerId:%d, DevId:%d, No Cmdq Handler Init\n",
                         __FUNCTION__, __LINE__, pstInputPortCtx->u32ContainIdx, pstInputPortContain->u32PortId,
                         pstVidLayerContain->u32VidLayerId, pstDevContain->u32DevId);
            }
        }
        else
        {
            bRet      = 0;
            *pCmdqCtx = NULL;
            DISP_ERR("%s %d, VideoLayer(%d) No Bind Device \n", __FUNCTION__, __LINE__,
                     pstVidLayerContain->u32VidLayerId);
        }
    }
    else
    {
        bRet      = 0;
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, Inputport(%d), VideoLayerContain is NULL\n", __FUNCTION__, __LINE__, pstInputPortCtx->u32ContainIdx);
    }

    return bRet;
}

static u8 _HalDispGetCmdqByDmaCtx(DrvDispCtxConfig_t *pstDmaCtx, void **pCmdqCtx)
{
    u8                      bRet          = 1;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;

    pstDmaContain = pstDmaCtx->pstCtxContain->pstDmaContain[pstDmaCtx->u32ContainIdx];
    _HalDispGetCmdqCtxByDmaId(pstDmaCtx, pstDmaContain->u32DmaId, pCmdqCtx);

    if (*pCmdqCtx == NULL)
    {
        bRet = 0;
        DISP_ERR("%s %d, ContainId%d, DevId:%d No Cmdq Handler Init\n", __FUNCTION__, __LINE__, pstDmaCtx->u32ContainIdx,
                 pstDmaContain->u32DmaId);
    }

    return bRet;
}

u8 HalDispGetCmdqByCtx(void *pCtx, void **pCmdqCtx)
{
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    u8                  bRet       = 1;

    if (pCtx == NULL || pstDispCtx->pstCtxContain == NULL)
    {
        *pCmdqCtx = NULL;
        return 0;
    }
    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        bRet = _HalDispGetCmdqByDevCtx(pstDispCtx, pCmdqCtx);
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        bRet = _HalDispGetCmdqByVideoLayerCtx(pstDispCtx, pCmdqCtx);
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        bRet = _HalDispGetCmdqByInputPortCtx(pstDispCtx, pCmdqCtx);
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        bRet = _HalDispGetCmdqByDmaCtx(pstDispCtx, pCmdqCtx);
    }
    else
    {
        bRet      = 0;
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, Unknown Ctx Type %d\n", __FUNCTION__, __LINE__, pstDispCtx->enCtxType);
    }
    return bRet;
}

u8 HalDispGetUtilityIdByDevId(u32 u32DevId, s32 *ps32UtilityId)
{
    u8 bRet = 1;

    *ps32UtilityId = u32DevId == HAL_DISP_DEVICE_ID_0   ? HAL_DISP_UTILITY_CMDQ_ID_DEVICE0
                     : u32DevId == HAL_DISP_DEVICE_ID_1 ? HAL_DISP_UTILITY_CMDQ_ID_DEVICE1
                                                        : -1;
    if (*ps32UtilityId < 0)
    {
        DISP_ERR("%s %d, UnSupport DevId: %d\n", __FUNCTION__, __LINE__, u32DevId);
        bRet = 0;
    }

    return bRet;
}

u8 HalDispGetUtilityIdByDmaId(u32 u32DmaId, s32 *ps32UtilityId)
{
    u8 bRet = 1;

    *ps32UtilityId = u32DmaId == HAL_DISP_DMA_ID_0 ? HAL_DISP_UTILITY_CMDQ_ID_DMA : -1;
    if (*ps32UtilityId < 0)
    {
        DISP_ERR("%s %d, UnSupport DmaId: %d\n", __FUNCTION__, __LINE__, u32DmaId);
        bRet = 0;
    }

    return bRet;
}
u32 HalDispGetDeviceId(void *pstCtx, u8 u8bDevContain)
{
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u32 u32Dev;

    if(u8bDevContain)
    {
        pstDevContain = pstCtx;
        u32Dev = GET_DISP_ID_BYDEVCONTAIN(pstDevContain);
    }
    else
    {
        pstDispCtx = pstCtx;
        u32Dev = GET_DISP_ID(pstDispCtx);
    }
    return u32Dev;
}
u32 HalDispGetInterface(void *pstCtx, u8 u8bDevContain)
{
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u32 u32Interface = 0;

    if(u8bDevContain)
    {
        pstDevContain = pstCtx;
        u32Interface = pstDevContain->u32Interface;
    }
    else
    {
        pstDispCtx = pstCtx;
        u32Interface = (pstDispCtx) ? pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx]->u32Interface : 0;
    }
    return u32Interface;
}
u16 HalDispGetDeviceTimeGenStart(void *pstCtx, u32 u32DevId)
{
    u16                          u16TimeGenStart = 0;
    s32 s32UtilId;
    HalDispGetUtilityIdByDevId(u32DevId, &s32UtilId);
    if(g_stDispIrqHist.stWorkingStatus.stDevStatus[u32DevId].u8Deviceused
        && g_stDispIrqHist.stWorkingStatus.stDevStatus[u32DevId].u8bStartTimegen)
    {
        u16TimeGenStart  = HalDispUtilityR2BYTEMaskDirect(
         REG_HAL_DISP_UTILIYT_CMDQDEV_TIMEGEN_START(s32UtilId), REG_CMDQ_DEV_TIMEGEN_START_MSK);
    }
    return u16TimeGenStart;

}
u32 HalDispWaitDeviceVdeDone(void *pstCtx, u32 u32DevId)
{
    HalDispUtilityCmdqContext_t *pCmdqCtx = NULL;
    u16                          u16TimeGenStart = 0;
    HalDispUtilityRegAccessMode_e enRiuMode = E_DISP_UTILITY_REG_ACCESS_CPU;
    s32 s32UtilId;
    HalDispGetCmdqByCtx(pstCtx, (void *)&pCmdqCtx);
    HalDispGetUtilityIdByDevId(u32DevId, &s32UtilId);
    u16TimeGenStart = HalDispGetDeviceTimeGenStart(pstCtx, u32DevId);
    if (pCmdqCtx)
    {
        enRiuMode = E_DISP_UTILITY_REG_ACCESS_CMDQ;
    }
    if (u16TimeGenStart)
    {
        HalDispUtilityWritePollCmdByType(HAL_DISP_UTILITY_DISP_TO_CMDQDEV_RAW(s32UtilId),
                               HAL_DISP_CMDQIRQ_DEV_OP2VDE_OFF_BIT, HAL_DISP_CMDQIRQ_DEV_OP2VDE_OFF_BIT, 2000000000, 1, pCmdqCtx, enRiuMode);
    }
    return 0;
}
