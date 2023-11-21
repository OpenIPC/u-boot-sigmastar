/*
* drv_hdmitx_ctx.c- Sigmastar
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

#define _DRV_HDMITX_CTX_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_common.h"
#include "drv_hdmitx_os.h"
#include "hdmitx_debug.h"
#include "hal_hdmitx_chip.h"
#include "hal_hdmitx_st.h"
#include "drv_hdmitx_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HDMITX_CTX_LOCK_SEM(x)   DrvHdmitxOsObtainSemaphore(x)
#define HDMITX_CTX_UNLOCK_SEM(x) DrvHdmitxOsReleaseSemaphore(x)

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvHdmitxCtxConfig_t g_stHdmitxCtxTbl[HAL_HDMITX_CTX_INST_MAX];
u8                   g_bHdmitxCtxInit = FALSE;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u8 DrvHdmitxCtxInit(void)
{
    u16 i;
    u8  bRet = 1;

    if (g_bHdmitxCtxInit)
    {
        return 1;
    }

    g_bHdmitxCtxInit = 1;

    for (i = 0; i < HAL_HDMITX_CTX_INST_MAX; i++)
    {
        g_stHdmitxCtxTbl[i].bUsed    = 0;
        g_stHdmitxCtxTbl[i].s32HdmitxId = -1;
        g_stHdmitxCtxTbl[i].s32CtxId = -1;

        g_stHdmitxCtxTbl[i].pstMhalCtx = DrvHdmitxOsMemAlloc(sizeof(DrvHdmitxCtxHalContain_t));
        if (g_stHdmitxCtxTbl[i].pstMhalCtx == NULL)
        {
            bRet = 0;
            HDMITX_ERR("[HdmitxCTX] %s %d, Allocate Mhal Ctx Fail\n", __FUNCTION__, __LINE__);
            break;
        }
        memset(g_stHdmitxCtxTbl[i].pstMhalCtx, 0, sizeof(DrvHdmitxCtxHalContain_t));
    }

    return bRet;
}

u8 DrvHdmitxCtxDeInit(void)
{
    u16 i;

    if (g_bHdmitxCtxInit == 0)
    {
        HDMITX_ERR("%s %d, Hdmitx Ctx not Inited\n", __FUNCTION__, __LINE__);
        return 0;
    }

    for (i = 0; i < HAL_HDMITX_CTX_INST_MAX; i++)
    {
        g_stHdmitxCtxTbl[i].bUsed    = 0;
        g_stHdmitxCtxTbl[i].s32HdmitxId = -1;
        g_stHdmitxCtxTbl[i].s32CtxId = -1;
        DrvHdmitxOsMemRelease(g_stHdmitxCtxTbl[i].pstMhalCtx);
        g_stHdmitxCtxTbl[i].pstMhalCtx = NULL;
    }
    g_bHdmitxCtxInit = 0;

    return 1;
}

u8 DrvHdmitxCtxIsAllFree(void)
{
    u16 i;
    u8  bFree = 1;

    for (i = 0; i < HAL_HDMITX_CTX_INST_MAX; i++)
    {
        if (g_stHdmitxCtxTbl[i].bUsed)
        {
            bFree = 0;
            break;
        }
    }
    return bFree;
}

u8 DrvHdmitxCtxAllocate(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx)
{
    u16 i    = 0;
    u8  bRet = 1;

    if (pAllocCfg == NULL)
    {
        HDMITX_ERR("[HdmitxCTX] %40s %d, CtxConfig is Empty\n", __FUNCTION__, __LINE__);
        *pCtx = NULL;
        bRet  = 0;
    }
    else
    {
        *pCtx = NULL;
        for (i = 0; i < HAL_HDMITX_CTX_INST_MAX; i++)
        {
            if (g_stHdmitxCtxTbl[i].bUsed == 0)
            {
                g_stHdmitxCtxTbl[i].bUsed    = 1;
                g_stHdmitxCtxTbl[i].s32HdmitxId = pAllocCfg->s32HdmitxId;
                g_stHdmitxCtxTbl[i].s32CtxId = i;
                g_stHdmitxCtxTbl[i].pstHwCtx = DrvHdmitxOsMemAlloc(sizeof(HalHdmitxResourcePrivate_t));
                if (!DrvHdmitxOsCreateSemaphore(&g_stHdmitxCtxTbl[i].stSem, 1))
                {
                    return FALSE;
                }
                if (g_stHdmitxCtxTbl[i].pstHwCtx == NULL)
                {
                    bRet = 0;
                    HDMITX_ERR("[HdmitxCTX] %s %d, Allocate Hw Ctx Fail\n", __FUNCTION__, __LINE__);
                    break;
                }
                memset(g_stHdmitxCtxTbl[i].pstHwCtx, 0, sizeof(HalHdmitxResourcePrivate_t));
                *pCtx = &g_stHdmitxCtxTbl[i];
                break;
            }
        }
    }

    if (*pCtx == NULL)
    {
        bRet = 0;
        HDMITX_ERR("%s %d, Alloc Ctx Fail\n", __FUNCTION__, __LINE__);
    }
    else
    {
        bRet = 1;
        HDMITX_DBG(HDMITX_DBG_LEVEL_CTX, "%s %d, Ctx Id:%d\n", __FUNCTION__, __LINE__, i);
    }

    return bRet;
}

u8 DrvHdmitxCtxGet(DrvHdmitxCtxAllocConfig_t *pAllocCfg, DrvHdmitxCtxConfig_t **pCtx)
{
    u16 i    = 0;
    u8  bRet = 1;

    if (pAllocCfg == NULL)
    {
        HDMITX_ERR("[HdmitxCTX] %40s %d, CtxConfig is Empty\n", __FUNCTION__, __LINE__);
        *pCtx = NULL;
        bRet  = 0;
    }
    else
    {
        *pCtx = NULL;
        for (i = 0; i < HAL_HDMITX_CTX_INST_MAX; i++)
        {
            if (g_stHdmitxCtxTbl[i].bUsed == 1 && g_stHdmitxCtxTbl[i].s32HdmitxId == pAllocCfg->s32HdmitxId)
            {
                *pCtx = &g_stHdmitxCtxTbl[i];
                break;
            }
        }
    }

    if (*pCtx == NULL)
    {
        bRet = 0;
        HDMITX_ERR("%s %d, Get Ctx is NULL\n", __FUNCTION__, __LINE__);
    }
    else
    {
        bRet = 1;
        HDMITX_DBG(HDMITX_DBG_LEVEL_CTX, "%s %d, Ctx Id:%d\n", __FUNCTION__, __LINE__, i);
    }

    return bRet;
}

u8 DrvHdmitxCtxFree(DrvHdmitxCtxConfig_t *pCtx)
{
    u8 bRet = 1;

    if (pCtx == NULL)
    {
        bRet = 0;
        HDMITX_ERR("[HdmitxCTX] %40s %d, CtxConfig is Empty\n", __FUNCTION__, __LINE__);
    }
    else
    {
        if (pCtx->bUsed)
        {
            DrvHdmitxOsDestroySemaphore(&pCtx->stSem);
            DrvHdmitxOsMemRelease(pCtx->pstHwCtx);
            pCtx->pstHwCtx = NULL;
        }
        pCtx->bUsed    = 0;
        pCtx->s32HdmitxId = -1;
        pCtx->s32CtxId = -1;
        memset(pCtx->pstMhalCtx, 0, sizeof(DrvHdmitxCtxHalContain_t));
    }
    return bRet;
}
