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

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "drv_disp_ctx.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp.h"
#include "hal_disp_st.h"
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

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------

bool _HalDispGetCmdqByDevCtx(DrvDispCtxConfig_t *pstDevCtx, void **pCmdqCtx)
{
    bool bRet = 1;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDevContain = pstDevCtx->pstCtxContain->pstDevContain[pstDevCtx->u32Idx];
    *pCmdqCtx = pstDevCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
    if(*pCmdqCtx == NULL)
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxId%ld, DevId:%ld No Cmdq Handler Init\n",
            __FUNCTION__, __LINE__, pstDevCtx->u32Idx, pstDevContain->u32DevId);
    }

    return bRet;
}

bool _HalDispGetCmdqByVideoLayerCtx(DrvDispCtxConfig_t *pstVidlayerCtx, void **pCmdqCtx)
{
    bool bRet = 1;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVideoLayerContain = NULL;

    pstVideoLayerContain = pstVidlayerCtx->pstCtxContain->pstVidLayerContain[pstVidlayerCtx->u32Idx];
    pstDevContain = (DrvDispCtxDeviceContain_t *)pstVideoLayerContain->pstDevCtx;

    if(pstDevContain)
    {
        *pCmdqCtx = pstVidlayerCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
        if(*pCmdqCtx == NULL)
        {
            bRet = 0;
            DISP_ERR("%s %d, CtxId:%ld, VideoLayerId:%ld, DevId:%ld No Cmdq Handler Init\n",
                __FUNCTION__, __LINE__,
                pstVidlayerCtx->u32Idx,
                pstVideoLayerContain->u32VidLayerId,
                pstDevContain->u32DevId);
        }
    }
    else
    {
        *pCmdqCtx = NULL;
        bRet = 0;
        DISP_ERR("%s %d, VideoLayer(%ld) No Bind Device \n",
            __FUNCTION__, __LINE__, pstVidlayerCtx->u32Idx);
    }

    return bRet;
}

bool _HalDispGetCmdqByInputPortCtx(DrvDispCtxConfig_t *pstInputPortCtx, void **pCmdqCtx)
{
    bool bRet = 1;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstInputPortContain = pstInputPortCtx->pstCtxContain->pstInputPortContain[pstInputPortCtx->u32Idx];
    pstVidLayerContain = (DrvDispCtxVideoLayerContain_t *)pstInputPortContain->pstVidLayerContain;
    if(pstVidLayerContain)
    {
        pstDevContain = (DrvDispCtxDeviceContain_t *)pstVidLayerContain->pstDevCtx;
        if(pstDevContain)
        {
            *pCmdqCtx = pstInputPortCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
            if(*pCmdqCtx == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, CtxId:%ld, InputportId:%ld, VideoayerId:%ld, DevId:%ld, No Cmdq Handler Init\n",
                    __FUNCTION__, __LINE__,
                    pstInputPortCtx->u32Idx,
                    pstInputPortContain->u32PortId,
                    pstVidLayerContain->u32VidLayerId,
                    pstDevContain->u32DevId);
            }
        }
        else
        {
            bRet = 0;
            *pCmdqCtx = NULL;
            DISP_ERR("%s %d, VideoLayer(%ld) No Bind Device \n",
                __FUNCTION__, __LINE__, pstVidLayerContain->u32VidLayerId);
        }
    }
    else
    {
        bRet = 0;
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, Inputport(%ld), VideoLayerContain is NULL\n",
            __FUNCTION__, __LINE__, pstInputPortCtx->u32Idx);
    }

    return bRet;
}

bool HalDispGetCmdqByCtx(void *pCtx, void **pCmdqCtx)
{
    DrvDispCtxConfig_t *pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    bool bRet = 1;

    if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        bRet = _HalDispGetCmdqByDevCtx(pstDispCtx, pCmdqCtx);
    }
    else if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        bRet = _HalDispGetCmdqByVideoLayerCtx(pstDispCtx, pCmdqCtx);
    }
    else if(pstDispCtx->enCtxType == E_DISP_CTX_TYPE_INPUTPORT)
    {
        bRet = _HalDispGetCmdqByInputPortCtx(pstDispCtx, pCmdqCtx);
    }
    else
    {
        bRet = 0;
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, Unknown Ctx Type %d\n", __FUNCTION__, __LINE__, pstDispCtx->enCtxType);
    }
    return bRet;
}

