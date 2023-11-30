/*
* drv_disp_irq.c- Sigmastar
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

#define _DRV_DISP_IRQ_C_

#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_if.h"
#include "hal_disp_util.h"
#include "hal_disp_irq.h"
#include "hal_disp.h"
#include "disp_debug.h"
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
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
typedef struct
{
    u32 u32IrqNum;
    u8  bInit;
} DrvDispIrqConfig_t;
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispIrqConfig_t     g_stDispIrqCfg[HAL_DISP_IRQ_ID_MAX] = HAL_DISP_IRQ_CFG;
HalDispIrqStatusHist_t g_stDispIrqHist;
//-------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void DrvDispIrqInit(void)
{
    memset(&g_stDispIrqHist, 0, sizeof(HalDispIrqStatusHist_t));
    HalDispIrqInit();
}
void DrvDispIrqSetIsrNum(u32 u32DevId, u32 u32IsrNum)
{
    if (u32DevId >= HAL_DISP_IRQ_ID_MAX)
    {
        DISP_ERR("%s %d, DevId too big: %d\n", __FUNCTION__, __LINE__, u32DevId);
        return;
    }
    DrvDispIrqInit();
    g_stDispIrqCfg[u32DevId].bInit     = 1;
    g_stDispIrqCfg[u32DevId].u32IrqNum = u32IsrNum;
}

u8 DrvDispIrqGetIsrNumByDevId(u32 u32DevId, u32 *pu32IsrNum)
{
    u8                        bRet = 1;
    HalDispIrqIoctlConfig_t   stIrqIoctlCfg;
    DrvDispCtxConfig_t        stDispCtxCfg;
    DrvDispCtxContain_t       stDispCtxContain;
    DrvDispCtxDeviceContain_t stDevContain;
    u8                        u8Idx;

    // Create a Fake Ctx
    stDispCtxCfg.enCtxType                   = E_DISP_CTX_TYPE_DEVICE;
    stDispCtxCfg.u32ContainIdx                      = u32DevId;
    stDispCtxCfg.pstCtxContain               = &stDispCtxContain;
    stDispCtxContain.pstDevContain[u32DevId] = &stDevContain;
    stDevContain.u32DevId                    = u32DevId;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_GET_ID;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
    stIrqIoctlCfg.pDispCtx    = &stDispCtxCfg;
    stIrqIoctlCfg.pParam      = (void *)&u8Idx;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    if (g_stDispIrqCfg[u8Idx].bInit)
    {
        *pu32IsrNum = g_stDispIrqCfg[u8Idx].u32IrqNum;
        DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, DevId:%d, Idx:%d, IrqNum=%d\n", __FUNCTION__, __LINE__, u32DevId, u8Idx,
                 *pu32IsrNum);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Irq Idx %d, not init\n", __FUNCTION__, __LINE__, u8Idx);
    }
    return bRet;
}

u8 DrvDispIrqGetIsrNum(void *pDevCtx, u32 *pu32IsrNum)
{
    u8                      bRet       = 1;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    u8                      u8Idx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);

        if (g_stDispIrqCfg[u8Idx].bInit)
        {
            *pu32IsrNum = g_stDispIrqCfg[u8Idx].u32IrqNum;
            DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, ContainId=%d, DevId:%d, Idx:%d, IrqNum=%d\n", __FUNCTION__, __LINE__,
                     pstDispCtx->u32ContainIdx, HalDispGetDeviceId(pstDispCtx, 0), u8Idx,
                     *pu32IsrNum);
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Irq Idx %d, not init\n", __FUNCTION__, __LINE__, u8Idx);
        }
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;

        if (HalDispIrqIoCtl(&stIrqIoctlCfg) == 0)
        {
            DISP_ERR("%s %d, Irq Ioctl(%d) Fail\n", __FUNCTION__, __LINE__, stIrqIoctlCfg.enIoctlType);
            *pu32IsrNum = 0;
        }
        else
        {
            if (g_stDispIrqCfg[u8Idx].bInit)
            {
                *pu32IsrNum = g_stDispIrqCfg[u8Idx].u32IrqNum;
                DISP_DBG(DISP_DBG_LEVEL_IRQ_DMA, "%s %d, ContainId=%d, DmaId:%d, Idx:%d, IrqNum=%d\n", __FUNCTION__,
                         __LINE__, pstDispCtx->u32ContainIdx,
                         pstDispCtx->pstCtxContain->pstDmaContain[pstDispCtx->u32ContainIdx]->u32DmaId, u8Idx, *pu32IsrNum);
            }
            else
            {
                bRet = 0;
                DISP_ERR("%s %d, Irq Idx %d, not init\n", __FUNCTION__, __LINE__, u8Idx);
            }
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 DrvDispIrqEnable(void *pDevCtx, u32 u32DevIrq, u8 bEnable)
{
    u8                         bRet = 1;
    HalDispIrqIoctlConfig_t    stIrqIoctlCfg;
    DrvDispCtxConfig_t *       pstDispCtx    = (DrvDispCtxConfig_t *)pDevCtx;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8                         u8Idx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain             = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;
        if (HalDispIrqIoCtl(&stIrqIoctlCfg))
        {
            if (g_stDispIrqCfg[u8Idx].u32IrqNum == u32DevIrq)
            {
                stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_ENABLE;
                stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_MOP_VSYNC;
                stIrqIoctlCfg.pDispCtx    = pDevCtx;
                stIrqIoctlCfg.pParam      = (void *)&bEnable;
                HalDispIrqIoCtl(&stIrqIoctlCfg);
            }
            else
            {
                bRet = 0;
                DISP_ERR("%s %d, IrqNum not match %d != %d\n", __FUNCTION__, __LINE__, u32DevIrq,
                         g_stDispIrqCfg[pstDevContain->u32DevId].u32IrqNum);
            }
        }
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;
        if (HalDispIrqIoCtl(&stIrqIoctlCfg))
        {
            if (g_stDispIrqCfg[u8Idx].u32IrqNum == u32DevIrq)
            {
                stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE;
                stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_DMA;
                stIrqIoctlCfg.pDispCtx    = pDevCtx;
                stIrqIoctlCfg.pParam      = (void *)&bEnable;
                HalDispIrqIoCtl(&stIrqIoctlCfg);
            }
            else
            {
                bRet = 0;
                DISP_ERR("%s %d, IrqNum not match %d != %d\n", __FUNCTION__, __LINE__, u32DevIrq,
                         g_stDispIrqCfg[pstDevContain->u32DevId].u32IrqNum);
            }
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 DrvDispIrqGetFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag)
{
    u8                      bRet = 1;
    u32                     u32IrqFlag;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_GET_FLAG;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_MOP_VSYNC;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u32IrqFlag;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
        pstIrqFlag->u32IrqFlag = u32IrqFlag;
        pstIrqFlag->u32IrqMask = E_HAL_DISP_IRQ_TYPE_MOP_VSYNC;
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_DMA;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u32IrqFlag;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
        pstIrqFlag->u32IrqFlag = u32IrqFlag;
        pstIrqFlag->u32IrqMask = E_HAL_DISP_IRQ_TYPE_DMA;
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 DrvDispIrqClear(void *pDevCtx, void *pData)
{
    u8                      bRet = 1;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    MHAL_DISP_IRQFlag_t *   pstIrqFlag = (MHAL_DISP_IRQFlag_t *)pData;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_CLEAR;
        stIrqIoctlCfg.enIrqType   = pstIrqFlag->u32IrqFlag & pstIrqFlag->u32IrqMask;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = NULL;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR;
        stIrqIoctlCfg.enIrqType   = pstIrqFlag->u32IrqFlag & pstIrqFlag->u32IrqMask;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = NULL;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }
    return bRet;
}

u8 DrvDispIrqGetLcdIsrNum(void *pDevCtx, u32 *pu32IsrNum)
{
    u8                      bRet       = 1;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    u8                      u8Idx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_LCD_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);

        if (g_stDispIrqCfg[u8Idx].bInit)
        {
            *pu32IsrNum = g_stDispIrqCfg[u8Idx].u32IrqNum;
            DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, ContainId=%d, DevId:%d, Idx:%d, IrqNum=%d\n", __FUNCTION__, __LINE__,
                     pstDispCtx->u32ContainIdx, HalDispGetDeviceId(pstDispCtx, 0), u8Idx,
                     *pu32IsrNum);
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, Irq Idx %d, not init\n", __FUNCTION__, __LINE__, u8Idx);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }

    return bRet;
}

u8 DrvDispIrqEnableLcd(void *pDevCtx, u32 u32DevIrq, u8 bEnable)
{
    u8                         bRet = 1;
    HalDispIrqIoctlConfig_t    stIrqIoctlCfg;
    DrvDispCtxConfig_t *       pstDispCtx    = (DrvDispCtxConfig_t *)pDevCtx;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8                         u8Idx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain             = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx];
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_LCD_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8Idx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);

        if (g_stDispIrqCfg[u8Idx].u32IrqNum == u32DevIrq)
        {
            stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_LCD_ENABLE;
            stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_LCD_FRM_END | E_HAL_DISP_IRQ_TYPE_LCD_FLM;
            stIrqIoctlCfg.pDispCtx    = pDevCtx;
            stIrqIoctlCfg.pParam      = (void *)&bEnable;
            HalDispIrqIoCtl(&stIrqIoctlCfg);
            DISP_DBG(DISP_DBG_LEVEL_IRQ, "%s %d, ContainId=%d, DevId:%d, En_Idx:%d\n", __FUNCTION__, __LINE__,
                     pstDispCtx->u32ContainIdx, HalDispGetDeviceId(pstDispCtx, 0), u8Idx);
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, IrqNum not match %d != %d\n", __FUNCTION__, __LINE__, u32DevIrq,
                     g_stDispIrqCfg[pstDevContain->u32DevId].u32IrqNum);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }

    return bRet;
}

u8 DrvDispIrqGetLcdFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag)
{
    u8                      bRet = 1;
    u32                     u32IrqFlag;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_LCD_GET_FLAG;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_LCD_FRM_END;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = (void *)&u32IrqFlag;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
        pstIrqFlag->u32IrqFlag = u32IrqFlag;
        pstIrqFlag->u32IrqMask = E_HAL_DISP_IRQ_TYPE_LCD_FRM_END;
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }

    return bRet;
}

u8 DrvDispIrqClearLcd(void *pDevCtx, void *pData)
{
    u8                      bRet = 1;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;
    MHAL_DISP_IRQFlag_t *   pstIrqFlag = (MHAL_DISP_IRQFlag_t *)pData;
    DrvDispCtxConfig_t *    pstDispCtx = (DrvDispCtxConfig_t *)pDevCtx;

    if (pstDispCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_LCD_CLEAR;
        stIrqIoctlCfg.enIrqType   = pstIrqFlag->u32IrqFlag & pstIrqFlag->u32IrqMask;
        stIrqIoctlCfg.pDispCtx    = pDevCtx;
        stIrqIoctlCfg.pParam      = NULL;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, CtxType=%s, Wrong\n", __FUNCTION__, __LINE__, PARSING_CTX_TYPE(pstDispCtx->enCtxType));
    }

    return bRet;
}

//------------------------------------------------------------------------------
// Internal Isr
//------------------------------------------------------------------------------

#if DISP_TIMEZONE_ISR_SUPPORT
static irqreturn_t _DrvDispIrqTimeZoneCb(int eIntNum, void *pstDevParam)
{
    u32                        u32Flag[HAL_DISP_DEVICE_MAX];
    HalDispIrqIoctlConfig_t    stIrqIoctlCfg[HAL_DISP_DEVICE_MAX];
    DrvDispCtxConfig_t *       pstDispCtxCfg = (DrvDispCtxConfig_t *)pstDevParam;
    DrvDispCtxDeviceContain_t *pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32ContainIdx];
    u32                        u32DevId      = pstDevContain->u32DevId;

    stIrqIoctlCfg[u32DevId].enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG;
    stIrqIoctlCfg[u32DevId].enIrqType   = E_HAL_DISP_IRQ_TYPE_TIMEZONE;
    stIrqIoctlCfg[u32DevId].pDispCtx    = pstDevParam;
    stIrqIoctlCfg[u32DevId].pParam      = (void *)&u32Flag[u32DevId];
    HalDispIrqIoCtl(&stIrqIoctlCfg[u32DevId]);

    stIrqIoctlCfg[u32DevId].enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR;
    stIrqIoctlCfg[u32DevId].enIrqType   = u32Flag[u32DevId];
    stIrqIoctlCfg[u32DevId].pDispCtx    = pstDevParam;
    stIrqIoctlCfg[u32DevId].pParam      = NULL;
    HalDispIrqIoCtl(&stIrqIoctlCfg[u32DevId]);

    return IRQ_HANDLED;
}
#endif

static u8 _DrvDispCreateTimeZoneIsr(DrvDispCtxConfig_t *pDispCtx)
{
#if DISP_TIMEZONE_ISR_SUPPORT
    s32                        s32IrqRet;
    u32                        u32IrqNum;
    u8                         bSupported;
    HalDispIrqIoctlConfig_t    stIrqIoctlCfg;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
    stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
    stIrqIoctlCfg.pParam      = (void *)&bSupported;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    if (bSupported)
    {
        u8 u8DeviceIdx;
        u8 bEnable;

        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8DeviceIdx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
        if (u8DeviceIdx == HAL_DISP_IRQ_ID_MAX)
        {
            DISP_ERR("Get Irq Num Fail\n");
            return 0;
        }
        u32IrqNum = g_stDispIrqCfg[u8DeviceIdx].u32IrqNum;

        pstDevContain = pDispCtx->pstCtxContain->pstDevContain[pDispCtx->u32ContainIdx];

        s32IrqRet = request_irq(u32IrqNum, _DrvDispIrqTimeZoneCb, (IRQF_ONESHOT | IRQF_SHARED),
                                (pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0)   ? "mdisp_interisr0"
                                : (pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1) ? "mdisp_interisr1"
                                                                                    : "mdisp_interisr2",
                                (void *)pDispCtx);
        if (0 == s32IrqRet)
        {
            DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, IrqNum=%d\n", __FUNCTION__, __LINE__, u32IrqNum);
        }
        else
        {
            DISP_ERR("Attach Irq Fail\n");
            return 0;
        }

        disable_irq(u32IrqNum);
        enable_irq(u32IrqNum);

        bEnable                   = 1;
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_TIMEZONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&bEnable;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Not Support\n", __FUNCTION__, __LINE__);
    }
#endif

    return 1;
}

#if DISP_VGA_HPD_ISR_SUPPORT
static irqreturn_t _DrvDispIrqVgaHpdIsrCb(int eIntNum, void *pstDevParam)
{
    u32                     u32Flag;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_FLAG;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF;
    stIrqIoctlCfg.pDispCtx    = pstDevParam;
    stIrqIoctlCfg.pParam      = (void *)&u32Flag;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_CLEAR;
    stIrqIoctlCfg.enIrqType   = u32Flag;
    stIrqIoctlCfg.pDispCtx    = pstDevParam;
    stIrqIoctlCfg.pParam      = NULL;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    HalDispIrqSetDacMute((u32Flag & E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF) ? 1 : 0);

    DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d VgaHpd:%s\n", __FUNCTION__, __LINE__,
             (u32Flag & E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON) ? "ON" : "OFF");

    return IRQ_HANDLED;
}
#endif

static u8 _DrvDispIrqCreateVgaHpdIsr(DrvDispCtxConfig_t *pDispCtx)
{
#if DISP_VGA_HPD_ISR_SUPPORT
    s32                     s32IrqRet;
    u32                     u32IrqNum;
    u8                      bSupported;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
    stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
    stIrqIoctlCfg.pParam      = (void *)&bSupported;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    if (bSupported)
    {
        u8 u8DeviceIdx;
        u8 bEnable;

        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8DeviceIdx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);

        u32IrqNum = g_stDispIrqCfg[u8DeviceIdx].u32IrqNum;

        s32IrqRet = request_irq(u32IrqNum, _DrvDispIrqVgaHpdIsrCb, (IRQF_ONESHOT | IRQF_SHARED), "mdisp_vgahpdisr",
                                (void *)pDispCtx);
        if (s32IrqRet)
        {
            DISP_ERR("Attach Irq Fail\n");
            return 0;
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, IrqNum=%d\n", __FUNCTION__, __LINE__, u32IrqNum);
        }

        disable_irq(u32IrqNum);
        enable_irq(u32IrqNum);

        bEnable                   = 1;
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&bEnable;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, Not Support\n", __FUNCTION__, __LINE__);
    }
#endif
    return 1;
}

static u8 _DrvDispIrqDestroyTimeZoneIsr(DrvDispCtxConfig_t *pDispCtx)
{
#if DISP_TIMEZONE_ISR_SUPPORT
    u32                     u32IrqNum;
    u8                      bSupported;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
    stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
    stIrqIoctlCfg.pParam      = (void *)&bSupported;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    if (bSupported)
    {
        u8 u8DeviceIdx;
        u8 bEnable;

        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8DeviceIdx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
        if (u8DeviceIdx == HAL_DISP_IRQ_ID_MAX)
        {
            DISP_ERR("Get Irq Num Fail\n");
            return 0;
        }
        u32IrqNum = g_stDispIrqCfg[u8DeviceIdx].u32IrqNum;
        disable_irq(u32IrqNum);
        free_irq(u32IrqNum, (void *)pDispCtx);

        bEnable                   = 0;
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_TIMEZONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&bEnable;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE, "%s %d, Not Support\n", __FUNCTION__, __LINE__);
    }
#endif
    return 1;
}

static u8 _DrvDispIrqDestroyVgaHpdIsr(DrvDispCtxConfig_t *pDispCtx)
{
#if DISP_VGA_HPD_ISR_SUPPORT
    u32                     u32IrqNum;
    u8                      bSupported;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED;
    stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
    stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
    stIrqIoctlCfg.pParam      = (void *)&bSupported;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    if (bSupported)
    {
        u8 u8DeviceIdx;
        u8 bEnable;

        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_NONE;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&u8DeviceIdx;
        HalDispIrqIoCtl(&stIrqIoctlCfg);

        u32IrqNum = g_stDispIrqCfg[u8DeviceIdx].u32IrqNum;
        disable_irq(u32IrqNum);
        free_irq(u32IrqNum, (void *)pDispCtx);

        bEnable                   = 0;
        stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE;
        stIrqIoctlCfg.enIrqType   = E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON_OFF;
        stIrqIoctlCfg.pDispCtx    = (void *)pDispCtx;
        stIrqIoctlCfg.pParam      = (void *)&bEnable;
        HalDispIrqIoCtl(&stIrqIoctlCfg);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_IRQ_VGA_HPD, "%s %d, Not Support\n", __FUNCTION__, __LINE__);
    }
#endif
    return 1;
}

u8 DrvDispIrqCreateInternalIsr(void *pDispCtx)
{
    u8 bRet = 1;

    bRet &= _DrvDispCreateTimeZoneIsr((DrvDispCtxConfig_t *)pDispCtx);
    bRet &= _DrvDispIrqCreateVgaHpdIsr((DrvDispCtxConfig_t *)pDispCtx);
    return bRet;
}

u8 DrvDispIrqDestroyInternalIsr(void *pDispCtx)
{
    u8 bRet = 1;

    bRet &= _DrvDispIrqDestroyTimeZoneIsr((DrvDispCtxConfig_t *)pDispCtx);
    bRet &= _DrvDispIrqDestroyVgaHpdIsr((DrvDispCtxConfig_t *)pDispCtx);
    return bRet;
}

u16 DrvDispIrqGetIrqCount(void)
{
    return HAL_DISP_IRQ_ID_MAX;
}
