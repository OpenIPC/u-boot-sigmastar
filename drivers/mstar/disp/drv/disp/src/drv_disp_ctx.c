/*
* drv_disp_ctx.c- Sigmastar
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

#define _DRV_DISP_CTX_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_if.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "disp_debug.h"
#include "drv_disp_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispCtxContain_t g_stDispCtxContainTbl[HAL_DISP_CTX_MAX_INST];
u8                  g_bDispCtxInit = 0;

DrvDispCtxConfig_t *g_pastCurDispDevCtx[HAL_DISP_DEVICE_MAX];
DrvDispCtxConfig_t *g_pastCurVidLayerCtx[HAL_DISP_VIDLAYER_MAX];
DrvDispCtxConfig_t *g_pastCurInputPortCtx[HAL_DISP_INPUTPORT_MAX];
DrvDispCtxConfig_t *g_pastCurDmaCtx[HAL_DISP_DMA_MAX];
//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static u32 _DrvDispCtxGetContainIdx(DrvDispCtxType_e enCtxType, u32 u32DevId)
{
    u32 u32ContainIdx = 0;
    u16 i, j = 0;
    
    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        if (enCtxType == E_DISP_CTX_TYPE_DEVICE && u32DevId < HAL_DISP_DEVICE_MAX)
        {
            for (j = 0; j < HAL_DISP_DEVICE_MAX; j++)
            {
                if (g_stDispCtxContainTbl[i].bDevContainUsed[j] == 1)
                {
                    if(g_stDispCtxContainTbl[i].pstDevContain[j]->u32DevId == u32DevId)
                    {
                        u32ContainIdx = j;
                        break;
                    }
                }
            }
            
        }
        else if (enCtxType == E_DISP_CTX_TYPE_VIDLAYER && u32DevId < HAL_DISP_VIDLAYER_MAX)
        {
            for (j = 0; j < HAL_DISP_VIDLAYER_MAX; j++)
            {
                if (g_stDispCtxContainTbl[i].bVidLayerContainUsed[j] == 1)
                {
                    if(g_stDispCtxContainTbl[i].pstVidLayerContain[j]->u32VidLayerId == u32DevId)
                    {
                        u32ContainIdx = j;
                        break;
                    }
                }
            }
        }
        else if (enCtxType == E_DISP_CTX_TYPE_INPUTPORT && u32DevId < HAL_DISP_INPUTPORT_MAX)
        {
            for (j = 0; j < HAL_DISP_DEVICE_MAX; j++)
            {
                if (g_stDispCtxContainTbl[i].bInputPortContainUsed[j] == 1)
                {
                    if(g_stDispCtxContainTbl[i].pstInputPortContain[j]->u32PortId == u32DevId)
                    {
                        u32ContainIdx = j;
                        break;
                    }
                }
            }
        }
        else if (enCtxType == E_DISP_CTX_TYPE_DMA && u32DevId < HAL_DISP_DMA_MAX)
        {
            for (j = 0; j < HAL_DISP_DMA_MAX; j++)
            {
                if (g_stDispCtxContainTbl[i].bDmaContainUsed[j] == 1)
                {
                    if(g_stDispCtxContainTbl[i].pstDmaContain[j]->u32DmaId == u32DevId)
                    {
                        u32ContainIdx = j;
                        break;
                    }
                }
            }
        }
    }
    return u32ContainIdx;
}
static u8 _DrvDispCtxBindInputPortWidthVidLayer(DrvDispCtxConfig_t *pstVidLayerCtx, u16 u16InputPortCtxId,
                                                u16 u16InputPortId)
{
    u8                             bRet                = 1;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain  = NULL;
    DrvDispCtxInputPortContain_t * pstInputPortContain = NULL;

    if (pstVidLayerCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER && u16InputPortCtxId != HAL_DISP_INPUTPORT_MAX
        && u16InputPortId < HAL_DISP_INPUTPORT_NUM)
    {
        pstVidLayerContain = pstVidLayerCtx->pstCtxContain->pstVidLayerContain[pstVidLayerCtx->u32ContainIdx];

        pstInputPortContain = pstVidLayerCtx->pstCtxContain->pstInputPortContain[u16InputPortCtxId];

        pstVidLayerContain->pstInputPortContain[u16InputPortId] = pstInputPortContain;
        pstInputPortContain->pstVidLayerContain                 = pstVidLayerContain;

        DISP_DBG(DISP_DBG_LEVEL_CTX, "%s %d, VideContainId=%d, VideId=%d,(%px), InputPortContainId:%d, InputPortId:%d,(%px)\n",
                 __FUNCTION__, __LINE__, pstVidLayerCtx->u32ContainIdx, pstVidLayerContain->u32VidLayerId, pstVidLayerContain,
                 u16InputPortCtxId, u16InputPortId, pstInputPortContain);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type is Err %s, \n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstVidLayerCtx->enCtxType));
    }
    return bRet;
}

static u8 _DrvDispCtxUnBindInputPortWidthVidLayer(DrvDispCtxConfig_t *pstInputPortCtx)
{
    u8                             bRet                = 1;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain  = NULL;
    DrvDispCtxInputPortContain_t * pstInputPortContain = NULL;

    if (pstInputPortCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        pstInputPortContain = pstInputPortCtx->pstCtxContain->pstInputPortContain[pstInputPortCtx->u32ContainIdx];
        pstVidLayerContain  = pstInputPortContain->pstVidLayerContain;

        pstVidLayerContain->pstInputPortContain[pstInputPortContain->u32PortId]                          = NULL;
        pstInputPortCtx->pstCtxContain->pstInputPortContain[pstInputPortCtx->u32ContainIdx]->pstVidLayerContain = NULL;

        DISP_DBG(DISP_DBG_LEVEL_CTX, "%s %d, VideLayerId=%d, InputPortContainId:%d, InputPortId:%d\n", __FUNCTION__,
                 __LINE__, pstVidLayerContain->u32VidLayerId, pstInputPortCtx->u32ContainIdx, pstInputPortContain->u32PortId);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type is Err %s, \n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstInputPortCtx->enCtxType));
    }
    return bRet;
}

static u8 _DrvDispCtxInitDeviceContain(DrvDispCtxContain_t *pDispContain)
{
    u16 i;
    u8  bRet = 1;

    for (i = 0; i < HAL_DISP_DEVICE_MAX; i++)
    {
        pDispContain->bDevContainUsed[i]      = 0;
        pDispContain->pstDevContain[i] = DrvDispOsMemAlloc(sizeof(DrvDispCtxDeviceContain_t));
        if (pDispContain->pstDevContain[i] == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Alloc Device Contain Fail\n", __FUNCTION__, __LINE__);
            break;
        }
        memset(pDispContain->pstDevContain[i], 0, sizeof(DrvDispCtxDeviceContain_t));
    }
    return bRet;
}

static void _DrvDispCtxDeInitDeviceContain(DrvDispCtxContain_t *pDispContain)
{
    u16 i;

    for (i = 0; i < HAL_DISP_DEVICE_MAX; i++)
    {
        if (pDispContain->pstDevContain[i])
        {
            DrvDispOsMemRelease(pDispContain->pstDevContain[i]);
            pDispContain->pstDevContain[i] = NULL;
            pDispContain->bDevContainUsed[i]      = 0;
        }
    }
}

static u8 _DrvDispCtxInitVidLayerContain(DrvDispCtxContain_t *pDispContain)
{
    u16 i;
    u8  bRet = 1;

    for (i = 0; i < HAL_DISP_VIDLAYER_MAX; i++)
    {
        pDispContain->bVidLayerContainUsed[i]      = 0;
        pDispContain->pstVidLayerContain[i] = DrvDispOsMemAlloc(sizeof(DrvDispCtxVideoLayerContain_t));
        if (pDispContain->pstVidLayerContain[i] == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Alloc VidLayer Contain Fail\n", __FUNCTION__, __LINE__);
            break;
        }
        memset(pDispContain->pstVidLayerContain[i], 0, sizeof(DrvDispCtxVideoLayerContain_t));
    }
    return bRet;
}
static void _DrvDispCtxDeInitVidLayerContain(DrvDispCtxContain_t *pDispContain)
{
    u16 i;

    for (i = 0; i < HAL_DISP_VIDLAYER_MAX; i++)
    {
        if (pDispContain->pstVidLayerContain[i])
        {
            DrvDispOsMemRelease(pDispContain->pstVidLayerContain[i]);
            pDispContain->pstVidLayerContain[i] = NULL;
            pDispContain->bVidLayerContainUsed[i]      = 0;
        }
    }
}

static u8 _DrvDispCtxInitInputPortContain(DrvDispCtxContain_t *pstDispContain)
{
    u16 i;
    u8  bRet = 1;

    for (i = 0; i < HAL_DISP_INPUTPORT_MAX; i++)
    {
        pstDispContain->bInputPortContainUsed[i]      = 0;
        pstDispContain->pstInputPortContain[i] = DrvDispOsMemAlloc(sizeof(DrvDispCtxInputPortContain_t));
        if (pstDispContain->pstInputPortContain[i] == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Alloc InputPort Contain Fail\n", __FUNCTION__, __LINE__);
            break;
        }
        memset(pstDispContain->pstInputPortContain[i], 0, sizeof(DrvDispCtxInputPortContain_t));
    }
    return bRet;
}

static void _DrvDispCtxDeInitInputPortContain(DrvDispCtxContain_t *pstDispCtxContain)
{
    u16 i;

    for (i = 0; i < HAL_DISP_INPUTPORT_MAX; i++)
    {
        if (pstDispCtxContain->pstInputPortContain[i])
        {
            DrvDispOsMemRelease(pstDispCtxContain->pstInputPortContain[i]);
            pstDispCtxContain->pstInputPortContain[i] = NULL;
            pstDispCtxContain->bInputPortContainUsed[i]      = 0;
        }
    }
}

static u8 _DrvDispCtxInitDmaContain(DrvDispCtxContain_t *pDispContain)
{
    u16 i;
    u8  bRet = 1;

    for (i = 0; i < HAL_DISP_DMA_MAX; i++)
    {
        pDispContain->bDmaContainUsed[i]      = 0;
        pDispContain->pstDmaContain[i] = DrvDispOsMemAlloc(sizeof(DrvDispCtxDmaContain_t));
        if (pDispContain->pstDmaContain[i] == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Alloc Dma Contain Fail\n", __FUNCTION__, __LINE__);
            break;
        }
        memset(pDispContain->pstDmaContain[i], 0, sizeof(DrvDispCtxDmaContain_t));
    }
    return bRet;
}

static void _DrvDispCtxDeInitDmaContain(DrvDispCtxContain_t *pDispCtxContain)
{
    u16 i;

    for (i = 0; i < HAL_DISP_DMA_MAX; i++)
    {
        if (pDispCtxContain->pstDmaContain[i])
        {
            DrvDispOsMemRelease(pDispCtxContain->pstDmaContain[i]);
            pDispCtxContain->pstDmaContain[i]  = NULL;
            pDispCtxContain->bInputPortContainUsed[i] = 0;
        }
    }
}

static u8 _DrvDispCtxInitHalHwContain(DrvDispCtxContain_t *pDispContain)
{
    u8 bRet = 1;
    if (sizeof(HalDispHwContain_t) != 0)
    {
        pDispContain->pstHalHwContain = DrvDispOsMemAlloc(sizeof(HalDispHwContain_t));
        if (pDispContain->pstHalHwContain == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, Alloc Hw Contain Fail\n", __FUNCTION__, __LINE__);
        }
        memset(pDispContain->pstHalHwContain, 0, sizeof(HalDispHwContain_t));
    }
    return bRet;
}

static void _DrvDispCtxDeInitHalHwContain(DrvDispCtxContain_t *pDispCtxContain)
{
    if (pDispCtxContain->pstHalHwContain)
    {
        DrvDispOsMemRelease(pDispCtxContain->pstHalHwContain);
        pDispCtxContain->pstHalHwContain = NULL;
    }
}

static u8 _DrvDispCtxAllocDevContain(DrvDispCtxAllocConfig_t *pstAllocCfg, DrvDispCtxConfig_t **pCtx)
{
    u16                 i, j, u16EmptyIdx, u16DevContainId;
    u8                  bRet       = 1;
    DrvDispCtxConfig_t *pstDispCtx = NULL;

    u16EmptyIdx = HAL_DISP_CTX_MAX_INST;
    u16DevContainId = HAL_DISP_DEVICE_MAX;

    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_DEVICE_MAX; j++)
        {
            if (g_stDispCtxContainTbl[i].bDevContainUsed[j] == 0)
            {
                u16EmptyIdx = i;
                u16DevContainId = j;
                break;
            }
        }
    }

    if (u16EmptyIdx != HAL_DISP_CTX_MAX_INST && u16DevContainId != HAL_DISP_DEVICE_MAX)
    {
        pstDispCtx = DrvDispOsMemAlloc(sizeof(DrvDispCtxConfig_t));

        if (pstDispCtx)
        {
            pstDispCtx->enCtxType     = E_DISP_CTX_TYPE_DEVICE;
            pstDispCtx->u32CtxIdx     = u16EmptyIdx;
            pstDispCtx->u32ContainIdx = u16DevContainId;
            pstDispCtx->pstCtxContain = &g_stDispCtxContainTbl[u16EmptyIdx];

            memset(pstDispCtx->pstCtxContain->pstDevContain[u16DevContainId], 0, sizeof(DrvDispCtxDeviceContain_t));

            pstDispCtx->pstCtxContain->bDevContainUsed[u16DevContainId]                = 1;
            pstDispCtx->pstCtxContain->pstDevContain[u16DevContainId]->u32DevId = pstAllocCfg->u32DevId;
            pstDispCtx->pstCtxContain->stMemAllcCfg.alloc                   = pstAllocCfg->stMemAllcCfg.alloc;
            pstDispCtx->pstCtxContain->stMemAllcCfg.free                    = pstAllocCfg->stMemAllcCfg.free;
            *pCtx                                                           = pstDispCtx;
        }
        else
        {
            bRet  = 0;
            *pCtx = NULL;
            DISP_ERR("%s %d, Alloc Ctx Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        bRet  = 0;
        *pCtx = NULL;
        DISP_ERR("%s %d, Alloc Dev Contain Fail\n", __FUNCTION__, __LINE__);
    }

    return bRet;
}

static u8 _DrvDispCtxFreeDevContain(DrvDispCtxConfig_t *pCtx)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8                         bRet          = 1;

    if (pCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain = pCtx->pstCtxContain->pstDevContain[pCtx->u32ContainIdx];
        memset(pstDevContain, 0, sizeof(DrvDispCtxDeviceContain_t));
        pCtx->pstCtxContain->bDevContainUsed[pCtx->u32ContainIdx] = 0;
        DrvDispOsMemRelease(pCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type Err, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}

static u8 _DrvDispCtxAllocVidLayerContain(DrvDispCtxAllocConfig_t *pstAllocCfg, DrvDispCtxConfig_t **pCtx)
{
    u16                 i, j, u16EmptyIdx, u16VidLayerContainId;
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    u8                  bRet       = 1;

    u16EmptyIdx      = HAL_DISP_CTX_MAX_INST;
    u16VidLayerContainId = HAL_DISP_VIDLAYER_MAX;

    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_VIDLAYER_MAX; j++)
        {
            if (g_stDispCtxContainTbl[i].bVidLayerContainUsed[j] == 0)
            {
                u16EmptyIdx      = i;
                u16VidLayerContainId = j;
                break;
            }
        }
    }

    if (u16EmptyIdx != HAL_DISP_CTX_MAX_INST && u16VidLayerContainId != HAL_DISP_VIDLAYER_MAX)
    {
        pstDispCtx = DrvDispOsMemAlloc(sizeof(DrvDispCtxConfig_t));

        if (pstDispCtx)
        {
            pstDispCtx->enCtxType     = E_DISP_CTX_TYPE_VIDLAYER;
            pstDispCtx->u32CtxIdx     = u16EmptyIdx;
            pstDispCtx->u32ContainIdx = u16VidLayerContainId;
            pstDispCtx->pstCtxContain = &g_stDispCtxContainTbl[u16EmptyIdx];

            memset(pstDispCtx->pstCtxContain->pstVidLayerContain[u16VidLayerContainId], 0,
                   sizeof(DrvDispCtxVideoLayerContain_t));
            pstDispCtx->pstCtxContain->bVidLayerContainUsed[u16VidLayerContainId]                     = 1;
            pstDispCtx->pstCtxContain->pstVidLayerContain[u16VidLayerContainId]->u32VidLayerId = pstAllocCfg->u32DevId;
            *pCtx                                                                          = pstDispCtx;
        }
        else
        {
            *pCtx = NULL;
            bRet  = 0;
            DISP_ERR("%s %d, Alloc Ctx Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        *pCtx = NULL;
        bRet  = 0;
        DISP_ERR("%s %d, Alloc VidLayer Contain Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

static u8 _DrvDispCtxFreeVidLayerContain(DrvDispCtxConfig_t *pCtx)
{
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain = NULL;
    u8                             bRet               = 1;

    if (pCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        pstVidLayerContain = pCtx->pstCtxContain->pstVidLayerContain[pCtx->u32ContainIdx];

        memset(pstVidLayerContain, 0, sizeof(DrvDispCtxVideoLayerContain_t));
        pCtx->pstCtxContain->bVidLayerContainUsed[pCtx->u32ContainIdx] = 0;
        DrvDispOsMemRelease(pCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type Err, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}

static u8 _DrvDispCtxAllocInputPortContain(DrvDispCtxAllocConfig_t *pstAllocCfg, DrvDispCtxConfig_t **pCtx)
{
    u16                 i, j, u16EmptyIdx, u16InputPortContainId;
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    u8                  bRet       = 1;

    u16EmptyIdx       = HAL_DISP_CTX_MAX_INST;
    u16InputPortContainId = HAL_DISP_INPUTPORT_MAX;

    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_INPUTPORT_MAX; j++)
        {
            if (g_stDispCtxContainTbl[i].bInputPortContainUsed[j] == 0)
            {
                u16EmptyIdx       = i;
                u16InputPortContainId = j;
                break;
            }
        }
    }

    if (u16EmptyIdx != HAL_DISP_CTX_MAX_INST && u16InputPortContainId != HAL_DISP_INPUTPORT_MAX)
    {
        pstDispCtx = DrvDispOsMemAlloc(sizeof(DrvDispCtxConfig_t));

        if (pstDispCtx)
        {
            pstDispCtx->enCtxType     = E_DISP_CTX_TYPE_INPUTPORT;
            pstDispCtx->u32CtxIdx     = u16EmptyIdx;
            pstDispCtx->u32ContainIdx = u16InputPortContainId;
            pstDispCtx->pstCtxContain = &g_stDispCtxContainTbl[u16EmptyIdx];

            memset(pstDispCtx->pstCtxContain->pstInputPortContain[u16InputPortContainId], 0,
                   sizeof(DrvDispCtxInputPortContain_t));

            if (_DrvDispCtxBindInputPortWidthVidLayer(pstAllocCfg->pstBindCtx, u16InputPortContainId, pstAllocCfg->u32DevId))
            {
                pstDispCtx->pstCtxContain->bInputPortContainUsed[u16InputPortContainId]                 = 1;
                pstDispCtx->pstCtxContain->pstInputPortContain[u16InputPortContainId]->u32PortId = pstAllocCfg->u32DevId;
                *pCtx                                                                        = pstDispCtx;
            }
            else
            {
                DrvDispOsMemRelease(pstDispCtx);
                *pCtx = NULL;
                bRet  = 0;
                DISP_ERR("%s %d, Bind Fail\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            *pCtx = NULL;
            bRet  = 0;
            DISP_ERR("%s %d, Alloc Ctx Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        *pCtx = NULL;
        bRet  = 0;
        DISP_ERR("%s %d, Alloc InputPort Contain Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

static u8 _DrvDispCtxFreeInputPortContain(DrvDispCtxConfig_t *pCtx)
{
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    u8                            bRet                = 1;

    if (pCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        if (_DrvDispCtxUnBindInputPortWidthVidLayer(pCtx))
        {
            pstInputPortContain = pCtx->pstCtxContain->pstInputPortContain[pCtx->u32ContainIdx];

            memset(pstInputPortContain, 0, sizeof(DrvDispCtxInputPortContain_t));
            pCtx->pstCtxContain->bInputPortContainUsed[pCtx->u32ContainIdx] = 0;
            DrvDispOsMemRelease(pCtx);
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, UnBind Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type Err, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}

static u8 _DrvDispCtxAllocDmaContain(DrvDispCtxAllocConfig_t *pstAllocCfg, DrvDispCtxConfig_t **pCtx)
{
    u16                 i, j, u16EmptyIdx, u16DmaContainId;
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    u8                  bRet       = 1;

    u16EmptyIdx = HAL_DISP_CTX_MAX_INST;
    u16DmaContainId = HAL_DISP_DMA_MAX;

    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_DMA_MAX; j++)
        {
            if (g_stDispCtxContainTbl[i].bDmaContainUsed[j] == 0)
            {
                u16EmptyIdx = i;
                u16DmaContainId = j;
                break;
            }
        }
    }

    if (u16EmptyIdx != HAL_DISP_CTX_MAX_INST && u16DmaContainId != HAL_DISP_DMA_MAX)
    {
        pstDispCtx = DrvDispOsMemAlloc(sizeof(DrvDispCtxConfig_t));

        if (pstDispCtx)
        {
            pstDispCtx->enCtxType     = E_DISP_CTX_TYPE_DMA;
            pstDispCtx->u32CtxIdx     = u16EmptyIdx;
            pstDispCtx->u32ContainIdx = u16DmaContainId;
            pstDispCtx->pstCtxContain = &g_stDispCtxContainTbl[u16EmptyIdx];

            memset(pstDispCtx->pstCtxContain->pstDmaContain[u16DmaContainId], 0, sizeof(DrvDispCtxDmaContain_t));
            pstDispCtx->pstCtxContain->bDmaContainUsed[u16DmaContainId]                = 1;
            pstDispCtx->pstCtxContain->pstDmaContain[u16DmaContainId]->u32DmaId = pstAllocCfg->u32DevId;
            *pCtx                                                           = pstDispCtx;
        }
        else
        {
            *pCtx = NULL;
            bRet  = 0;
            DISP_ERR("%s %d, Alloc Dma Ctx Fail\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        *pCtx = NULL;
        bRet  = 0;
        DISP_ERR("%s %d, Alloc Dma Contain Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

static u8 _DrvDispCtxFreeDmaContain(DrvDispCtxConfig_t *pCtx)
{
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    u8                      bRet          = 1;

    if (pCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        pstDmaContain = pCtx->pstCtxContain->pstDmaContain[pCtx->u32ContainIdx];
        memset(pstDmaContain, 0, sizeof(DrvDispCtxDmaContain_t));
        pCtx->pstCtxContain->bDmaContainUsed[pCtx->u32ContainIdx] = 0;
        DrvDispOsMemRelease(pCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx Type Err, %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 DrvDispCtxInit(void)
{
    u16 i;
    u8  bRet = 1;

    if (g_bDispCtxInit == 1)
    {
        return 1;
    }
    DrvDispIrqInit();
    memset(&g_stDispCtxContainTbl, 0, sizeof(DrvDispCtxContain_t) * HAL_DISP_CTX_MAX_INST);
    HalDispIfInit();
    for (i = 0; i < HAL_DISP_DEVICE_MAX; i++)
    {
        g_pastCurDispDevCtx[i] = NULL;
    }

    for (i = 0; i < HAL_DISP_VIDLAYER_MAX; i++)
    {
        g_pastCurVidLayerCtx[i] = NULL;
    }

    for (i = 0; i < HAL_DISP_INPUTPORT_MAX; i++)
    {
        g_pastCurInputPortCtx[i] = NULL;
    }

    for (i = 0; i < HAL_DISP_DMA_MAX; i++)
    {
        g_pastCurDmaCtx[i] = NULL;
    }
    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        // Device
        if (_DrvDispCtxInitDeviceContain(&g_stDispCtxContainTbl[i]) == FALSE)
        {
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            bRet = 0;
            break;
        }

        // VideoLayer
        if (_DrvDispCtxInitVidLayerContain(&g_stDispCtxContainTbl[i]) == FALSE)
        {
            _DrvDispCtxDeInitVidLayerContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            bRet = 0;
            break;
        }

        // InputPort
        if (_DrvDispCtxInitInputPortContain(&g_stDispCtxContainTbl[i]) == FALSE)
        {
            _DrvDispCtxDeInitInputPortContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitVidLayerContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            bRet = 0;
            break;
        }

        // Dma
        if (_DrvDispCtxInitDmaContain(&g_stDispCtxContainTbl[i]) == FALSE)
        {
            _DrvDispCtxDeInitDmaContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitInputPortContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitVidLayerContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            bRet = 0;
            break;
        }

        // HalHw
        if (_DrvDispCtxInitHalHwContain(&g_stDispCtxContainTbl[i]) == FALSE)
        {
            _DrvDispCtxDeInitHalHwContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDmaContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitInputPortContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitVidLayerContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            bRet = 0;
            break;
        }
        // MemAlloc
        g_stDispCtxContainTbl[i].stMemAllcCfg.alloc = NULL;
        g_stDispCtxContainTbl[i].stMemAllcCfg.free  = NULL;
    }

    g_bDispCtxInit = 1;
    return bRet;
}

u8 DrvDispCtxDeInit(void)
{
    u16 i;
    u8  bRet = 1;

    if (g_bDispCtxInit == 0)
    {
        bRet = 0;
        DISP_ERR("%s %d, Ctx not Init\n", __FUNCTION__, __LINE__);
    }
    else
    {
        for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
        {
            _DrvDispCtxDeInitHalHwContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDmaContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitInputPortContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitVidLayerContain(&g_stDispCtxContainTbl[i]);
            _DrvDispCtxDeInitDeviceContain(&g_stDispCtxContainTbl[i]);
            g_stDispCtxContainTbl[i].stMemAllcCfg.alloc = NULL;
            g_stDispCtxContainTbl[i].stMemAllcCfg.free  = NULL;
        }

        for (i = 0; i < HAL_DISP_DEVICE_MAX; i++)
        {
            g_pastCurDispDevCtx[i] = NULL;
        }

        for (i = 0; i < HAL_DISP_VIDLAYER_MAX; i++)
        {
            g_pastCurVidLayerCtx[i] = NULL;
        }

        for (i = 0; i < HAL_DISP_INPUTPORT_MAX; i++)
        {
            g_pastCurInputPortCtx[i] = NULL;
        }

        for (i = 0; i < HAL_DISP_DMA_MAX; i++)
        {
            g_pastCurDmaCtx[i] = NULL;
        }

        HalDispIfDeInit();
        g_bDispCtxInit = 0;
        DISP_DBG(DISP_DBG_LEVEL_CTX, "%s %d\n", __FUNCTION__, __LINE__);
    }

    return bRet;
}

u8 DrvDispCtxAllocate(DrvDispCtxAllocConfig_t *pstAllocCfg, DrvDispCtxConfig_t **pCtx)
{
    u8 bRet = 1;

    if (pstAllocCfg->enType == E_DISP_CTX_TYPE_DEVICE)
    {
        bRet = _DrvDispCtxAllocDevContain(pstAllocCfg, pCtx);
    }
    else if (pstAllocCfg->enType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        bRet = _DrvDispCtxAllocVidLayerContain(pstAllocCfg, pCtx);
    }
    else if (pstAllocCfg->enType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        bRet = _DrvDispCtxAllocInputPortContain(pstAllocCfg, pCtx);
    }
    else if (pstAllocCfg->enType == E_DISP_CTX_TYPE_DMA)
    {
        bRet = _DrvDispCtxAllocDmaContain(pstAllocCfg, pCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Alloc Type is Err %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstAllocCfg->enType));
    }
    DISP_DBG(DISP_DBG_LEVEL_CTX, "%s %d, CtxType:%s bCreate:%hhd\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstAllocCfg->enType), bRet);
    return bRet;
}

u8 DrvDispCtxFree(DrvDispCtxConfig_t *pCtx)
{
    u8 bRet = 1;
    
    DISP_DBG(DISP_DBG_LEVEL_CTX, "%s %d, CtxType=%s, ContainIdx:%d\n", __FUNCTION__,
             __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType), pCtx->u32ContainIdx);
    if (pCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        bRet = _DrvDispCtxFreeDevContain(pCtx);
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        bRet = _DrvDispCtxFreeVidLayerContain(pCtx);
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        bRet = _DrvDispCtxFreeInputPortContain(pCtx);
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        bRet = _DrvDispCtxFreeDmaContain(pCtx);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Alloc Type is Err %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}
u8 DrvDispCtxIsLastDeviceCtx(DrvDispCtxConfig_t *pCtx)
{
    u16 i, j;
    u8  bContainUse = 0;

    
    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_DEVICE_MAX; j++)
        {
            bContainUse += g_stDispCtxContainTbl[i].bDevContainUsed[j];
        }
    }
    return (bContainUse==1) ? 1 : 0;
}
u8 DrvDispCtxIsAllFree(void)
{
    u16 i, j;
    u8  bContainUse = 0;

    for (i = 0; i < HAL_DISP_CTX_MAX_INST; i++)
    {
        for (j = 0; j < HAL_DISP_DEVICE_MAX; j++)
        {
            bContainUse |= g_stDispCtxContainTbl[i].bDevContainUsed[j];
        }

        for (j = 0; j < HAL_DISP_VIDLAYER_MAX; j++)
        {
            bContainUse |= g_stDispCtxContainTbl[i].bVidLayerContainUsed[j];
        }

        for (j = 0; j < HAL_DISP_INPUTPORT_MAX; j++)
        {
            bContainUse |= g_stDispCtxContainTbl[i].bInputPortContainUsed[j];
        }

        for (j = 0; j < HAL_DISP_DMA_MAX; j++)
        {
            bContainUse |= g_stDispCtxContainTbl[i].bDmaContainUsed[j];
        }
    }

    return bContainUse ? 0 : 1;
}

u8 DrvDispCtxSetCurCtx(DrvDispCtxConfig_t *pCtx, u32 u32Idx)
{
    u8 bRet = TRUE;
    ;
    if (pCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE && u32Idx < HAL_DISP_DEVICE_MAX)
    {
        g_pastCurDispDevCtx[u32Idx] = pCtx;
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER && u32Idx < HAL_DISP_VIDLAYER_MAX)
    {
        g_pastCurVidLayerCtx[u32Idx] = pCtx;
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT && u32Idx < HAL_DISP_INPUTPORT_MAX)
    {
        g_pastCurInputPortCtx[u32Idx] = pCtx;
    }
    else if (pCtx->enCtxType == E_DISP_CTX_TYPE_DMA && u32Idx < HAL_DISP_DMA_MAX)
    {
        g_pastCurDmaCtx[u32Idx] = pCtx;
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, SetCurCtx Type is Err %s\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pCtx->enCtxType));
    }
    return bRet;
}
u8 DrvDispCtxGetCurCtx(DrvDispCtxType_e enCtxType, u32 u32DeviceId, DrvDispCtxConfig_t **pCtx)
{
    u8 bRet = TRUE;
    u32 u32ContainIdx = _DrvDispCtxGetContainIdx(enCtxType, u32DeviceId);

    if (enCtxType == E_DISP_CTX_TYPE_DEVICE && u32DeviceId < HAL_DISP_DEVICE_MAX)
    {
        *pCtx = g_pastCurDispDevCtx[u32ContainIdx];
    }
    else if (enCtxType == E_DISP_CTX_TYPE_VIDLAYER && u32DeviceId < HAL_DISP_VIDLAYER_MAX)
    {
        *pCtx = g_pastCurVidLayerCtx[u32ContainIdx];
    }
    else if (enCtxType == E_DISP_CTX_TYPE_INPUTPORT && u32DeviceId < HAL_DISP_INPUTPORT_MAX)
    {
        *pCtx = g_pastCurInputPortCtx[u32ContainIdx];
    }
    else if (enCtxType == E_DISP_CTX_TYPE_DMA && u32DeviceId < HAL_DISP_DMA_MAX)
    {
        *pCtx = g_pastCurDmaCtx[u32ContainIdx];
    }
    else
    {
        bRet  = FALSE;
        *pCtx = NULL;
        DISP_ERR("%s %d, GetCurCtx Fail, Type=%s, ContainIdx=%d DeviceId=%d \n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(enCtxType),
                 u32ContainIdx, u32DeviceId);
    }
    return bRet;
}
