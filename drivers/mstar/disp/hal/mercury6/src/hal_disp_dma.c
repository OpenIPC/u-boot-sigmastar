/*
* hal_disp_dma.c- Sigmastar
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

#define _HAL_DISP_DMA_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "hal_disp_dma.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC,
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST,
    E_HAL_DISP_DMA_CMDQ_CTX_TYPE_NUM,
}HalDispDmaCmdqCtxType_e;
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void _HalDispDmaGetCmdqCtx(void *pCtx, void **pCmdqCtx, HalDispDmaCmdqCtxType_e eDmaCmdqCtxType)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t  *pstDmaContain = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;

    if(pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DMA)
    {
        pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
        pstDevContain =
            (eDmaCmdqCtxType == E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC) ?
            (DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain :
            (DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain;
        if(pstDevContain)
        {
            *pCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
        }
        else
        {
            *pCmdqCtx = NULL;
            DISP_ERR("%s %d, DevCaontain Null \n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        *pCmdqCtx = NULL;
        DISP_ERR("%s %d, CtxTpype:%s(%d) is Not Correct\n",
            __FUNCTION__, __LINE__,
            PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType), pstDispCtxCfg->enCtxType);
    }
}

//-------------------------------------------------------------------------------
// Global Functions
//-------------------------------------------------------------------------------
void HalDispDmaSetFrmWEn(bool bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_01_L, bEn ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWDbEn(bool bEn, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_01_L, bEn ? 0x4000 : 0x0000, 0x4000, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWCFilter(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_02_L, u16Val, 0x0007, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWPixelFormat(u16 u16Val0, u16 u16Val1, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_02_L, u16Val0, 0x43F8, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_03_L, u16Val1, 0xFFFF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWVflip(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_02_L, u16Val ? 0x1000: 0x0000, 0x1000, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWHmirror(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_02_L, u16Val ? 0x2000: 0x0000, 0x2000, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWSize(u16 u16Width, u16 u16Height, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_04_L, u16Width, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_05_L, u16Height, 0xFFFF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmW444Packet(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_15_L, u16Val, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWBaseAddr(u32 *pu32BaseAddr, u16 u16FrameIdx, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u32 u32RegOffset;
    u16 u16BaseAddrY_Hi, u16BaseAddrY_Lo;
    u16 u16BaseAddrC_Hi, u16BaseAddrC_Lo;
    u16 u16BaseAddrV_Hi, u16BaseAddrV_Lo;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);

    if(pCmdqCtx)
    {
        u32RegOffset = ((u16FrameIdx % 4) << 2);

        u16BaseAddrY_Lo = (pu32BaseAddr[0] & 0x0000FFFF);
        u16BaseAddrY_Hi = (pu32BaseAddr[0] & 0xFFFF0000) >> 16;
        u16BaseAddrC_Lo = (pu32BaseAddr[1] & 0x0000FFFF);
        u16BaseAddrC_Hi = (pu32BaseAddr[1] & 0xFFFF0000) >> 16;
        u16BaseAddrV_Lo = (pu32BaseAddr[2] & 0x0000FFFF);
        u16BaseAddrV_Hi = (pu32BaseAddr[2] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_08_L, u16BaseAddrY_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_09_L, u16BaseAddrY_Hi, 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_10_L, u16BaseAddrC_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_11_L, u16BaseAddrC_Hi, 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_18_L, u16BaseAddrV_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_19_L, u16BaseAddrV_Hi, 0x0FFF, pCmdqCtx);

        if(u16FrameIdx != 0)
        {
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_08_L + u32RegOffset, u16BaseAddrY_Lo, 0xFFFF, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_09_L + u32RegOffset, u16BaseAddrY_Hi, 0x0FFF, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_10_L + u32RegOffset, u16BaseAddrC_Lo, 0xFFFF, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_11_L + u32RegOffset, u16BaseAddrC_Hi, 0x0FFF, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_18_L + u32RegOffset, u16BaseAddrV_Lo, 0xFFFF, pCmdqCtx);
            HalDispUtilityW2BYTEMSK(REG_DISP_DMA_19_L + u32RegOffset, u16BaseAddrV_Hi, 0x0FFF, pCmdqCtx);
        }
    }
}

void HalDispDmaSetFrmWReqTh(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_21_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqLen(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_21_L, (u16Val << 8), 0xFF00, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqThC(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_22_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWReqLenC(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_22_L, (u16Val << 8), 0xFF00, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtR0(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_23_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtR1(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_23_L, (u16Val << 8), 0xFF00, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWMaxLenghtAll(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_24_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWLineOffset(bool bEn, u16 *pu16LineOffset, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_30_L, pu16LineOffset[0], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_31_L, pu16LineOffset[1], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_32_L, bEn ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_37_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWIdxMd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_37_L, ((u16Val & 0x0007) << 8), 0x0700, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWIdxW(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_37_L, ((u16Val & 0x000F) << 12), 0xF000, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiStarAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16StarAddrY_Lo, u16StarAddrY_Hi;
    u16 u16StarAddrC_Lo, u16StarAddrC_Hi;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16StarAddrY_Lo = pu32StartAddr[0] & 0x0000FFFF;
        u16StarAddrY_Hi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;

        u16StarAddrC_Lo = pu32StartAddr[1] & 0x0000FFFF;
        u16StarAddrC_Hi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_38_L, u16StarAddrY_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_39_L, u16StarAddrY_Hi, 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3A_L, u16StarAddrC_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3B_L, u16StarAddrC_Hi, 0x0FFF, pCmdqCtx);
    }
}

void HalDispDmaSetFrmWImiOffset(u32 *pu32Offset, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16OffsetY_Lo, u16OffsetY_Hi;
    u16 u16OffsetC_Lo, u16OffsetC_Hi;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16OffsetY_Lo = pu32Offset[0] & 0x0000FFFF;
        u16OffsetY_Hi = (pu32Offset[0] & 0xFFFF0000) >> 16;

        u16OffsetC_Lo = pu32Offset[1] & 0x0000FFFF;
        u16OffsetC_Hi = (pu32Offset[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3C_L, u16OffsetY_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3D_L, u16OffsetY_Hi, 0x01FF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3E_L, u16OffsetC_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_3F_L, u16OffsetC_Hi, 0x01FF, pCmdqCtx);
    }
}

void HalDispDmaSetVenModeSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_42_L, u16Val, 0x0003, pCmdqCtx);
    }
}

void HalDispDmaSetVenRingStarAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16StartAddrY_Hi, u16StartAddrY_Lo;
    u16 u16StartAddrC_Hi, u16StartAddrC_Lo;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16StartAddrY_Hi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;
        u16StartAddrY_Lo = (pu32StartAddr[0] & 0x0000FFFF);
        u16StartAddrC_Hi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;
        u16StartAddrC_Lo = (pu32StartAddr[1] & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_43_L, u16StartAddrY_Lo, 0xFFFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_44_L, u16StartAddrY_Hi, 0x0FFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_47_L, u16StartAddrC_Lo, 0xFFFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_48_L, u16StartAddrC_Hi, 0x0FFF,  pCmdqCtx);
    }
}

void HalDispDmaSetVenRingEndAddr(u32 *pu32EndAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16EndAddrY_Hi, u16EndAddrY_Lo;
    u16 u16EndAddrC_Hi, u16EndAddrC_Lo;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16EndAddrY_Hi = (pu32EndAddr[0] & 0xFFFF0000) >> 16;
        u16EndAddrY_Lo = (pu32EndAddr[0] & 0x0000FFFF);
        u16EndAddrC_Hi = (pu32EndAddr[1] & 0xFFFF0000) >> 16;
        u16EndAddrC_Lo = (pu32EndAddr[1] & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_45_L, u16EndAddrY_Lo, 0xFFFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_46_L, u16EndAddrY_Hi, 0x0FFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_49_L, u16EndAddrC_Lo, 0xFFFF,  pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_4A_L, u16EndAddrC_Hi, 0x0FFF,  pCmdqCtx);
    }
}

void HalDispDmaSetDdaHratio(bool bEn, u16 u16Ratio, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_60_L, bEn ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_60_L, u16Ratio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispDmaSetDdaVratio(bool bEn, u16 u16Ratio, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_61_L, bEn ? 0x8000 : 0x0000, 0x8000, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_61_L, u16Ratio, 0x1FFF, pCmdqCtx);
    }
}

void HalDispDmaSetDdaInSize(u16 u16Hsize, u16 u16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_62_L, u16Hsize, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_63_L, u16Vsize, 0xFFFF, pCmdqCtx);
    }
}

void HalDispDmaSetDdaOutSize(u16 u16Hsize, u16 u16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_64_L, u16Hsize, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_65_L, u16Vsize, 0xFFFF, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsRingEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_6F_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsMopMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_6F_L, u16Val, 0x0002, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsDest(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_6F_L, u16Val, 0x0004, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsOffsetEn(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_6F_L, u16Val ? 0x0008 : 0x0000, 0x0008, pCmdqCtx);
    }
}


void HalDispDmaSetCvbsImiStartAddr(u32 *pu32StartAddr, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16StarAddrY_Lo, u16StarAddrY_Hi;
    u16 u16StarAddrC_Lo, u16StarAddrC_Hi;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16StarAddrY_Lo = pu32StartAddr[0] & 0x0000FFFF;
        u16StarAddrY_Hi = (pu32StartAddr[0] & 0xFFFF0000) >> 16;

        u16StarAddrC_Lo = pu32StartAddr[1] & 0x0000FFFF;
        u16StarAddrC_Hi = (pu32StartAddr[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_70_L, u16StarAddrY_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_71_L, u16StarAddrY_Hi, 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_72_L, u16StarAddrC_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_73_L, u16StarAddrC_Hi, 0x0FFF, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsImiOffset(u32 *pu32Offset, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16OffsetY_Lo, u16OffsetY_Hi;
    u16 u16OffsetC_Lo, u16OffsetC_Hi;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16OffsetY_Lo = pu32Offset[0] & 0x0000FFFF;
        u16OffsetY_Hi = (pu32Offset[0] & 0xFFFF0000) >> 16;

        u16OffsetC_Lo = pu32Offset[1] & 0x0000FFFF;
        u16OffsetC_Hi = (pu32Offset[1] & 0xFFFF0000) >> 16;

        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_74_L, u16OffsetY_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_75_L, u16OffsetY_Hi, 0x0FFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_76_L, u16OffsetC_Lo, 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_77_L, u16OffsetC_Hi, 0x0FFF, pCmdqCtx);
    }
}

void HalDispDmaSetCvbsSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_78_L, pu16Hsize[0], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7A_L, pu16Vsize[0], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7C_L, pu16Hsize[1], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7E_L, pu16Vsize[1], 0xFFFF, pCmdqCtx);
    }
}


void HalDispDmaSetCvbsBufSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_79_L, pu16Hsize[0], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7B_L, pu16Vsize[0], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7D_L, pu16Hsize[1], 0xFFFF, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(REG_DISP_DMA_7F_L, pu16Vsize[1], 0xFFFF, pCmdqCtx);
    }
}

void HalDispDmaSetVideoPathFpllDly(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_OP2_0_2C_L, u16Val, 0x1FFF, pCmdqCtx);
    }
}

void HalDispDmaSetVideoPathFpllRefSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_OP2_0_2B_L, u16Val, 0x0003, pCmdqCtx);
    }
}


void HalDispDmaSetTgenExtFrmResetMode(u16 u16Val, void *pCtx)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDestDevContain = NULL;
    void *pCmdqCtx = NULL;
    u32 u32Reg;
    u16 u16RegVal;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
        pstDestDevContain = (DrvDispCtxDeviceContain_t *)pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx]->pDestDevContain;

        u32Reg = pstDestDevContain->u32DevId == HAL_DISP_DEVICE_ID_1 ?
                 REG_DISP_TOP_OP2_1_10_L :
                 REG_DISP_TOP_OP2_0_10_L;
        u16RegVal = 0x01;

        HalDispUtilityW2BYTEMSK(u32Reg, u16RegVal, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetFpll1SrcMux(bool bEn, u16 u16Mux, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16Val;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16Val = bEn ? 0x0200 : 0x0000;
        u16Val |= u16Mux == HAL_DISP_DEVICE_ID_1 ? 0x0100 : 0x0000;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_21_L, u16Val, 0x0300, pCmdqCtx);
    }
}

void HalDispDmaSetDispToMiuMux(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_21_L, u16Val << 6, 0x00C0, pCmdqCtx);
    }
}

void HalDispDmaSetHdmiSynthSet(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16Val;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_SRC);
    if(pCmdqCtx)
    {
        u16Val = u32Val & 0x0000FFFF;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_10_L, u16Val, 0xFFFF, pCmdqCtx);
        u16Val = (u32Val & 0xFFFF0000) >> 16;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_0_11_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaWaitFpllLock(void *pCtx)
{
    void *pCmdqCtx = NULL;
    #define WAIT_FPLL_TIMEOUT   2000000000 // 2sec

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);

    if(pCmdqCtx)
    {
        HalDispUtilityWritePollCmd(REG_DISP_TOP_1_17_L, 0x8000, 0x8000, WAIT_FPLL_TIMEOUT, 1, pCmdqCtx);
        HalDispUtilityW2BYTEMSK(
                HAL_DISP_UTILITY_DISP_TO_CMDQ_CLEAR(((HalDispUtilityCmdqContext_t*) pCmdqCtx)->s32CmdqId),
                0xFFFF,
                0xFFFF,
                (void *)pCmdqCtx);
    }
}

void HalDispDmaSetFpllEn(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_15_L, u8Val ? 0x0003 : 0x0000, 0x0003, pCmdqCtx);
    }
}

void HalDispDmaSetFpllSwRst(u8 u8Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_15_L, u8Val ? 0x0080 : 0x0000, 0x0080, pCmdqCtx);
    }
}

void HalDispDmaSetTvencSynthSet(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16Val;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        u16Val = u32Val & 0x0000FFFF;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_10_L, u16Val, 0xFFFF, pCmdqCtx);
        u16Val = (u32Val & 0xFFFF0000) >> 16;
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_11_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetTvencSynthSel(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_LPLL_4C_L, u16Val ? 0x0001 : 0x0000, 0x0001, pCmdqCtx);
    }
}

void HalDispDmaSetFpllGain(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_1B_L, u16Val, 0x00FF, pCmdqCtx);
    }
}

void HalDispDmaSetFpllThd(u16 u16Val, void *pCtx)
{
    void *pCmdqCtx = NULL;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_1B_L, u16Val << 8, 0xFF00, pCmdqCtx);
    }
}


void HalDispDmaSetFpllAbsLimit(u32 u32Val, void *pCtx)
{
    void *pCmdqCtx = NULL;
    u16 u16Val;

    _HalDispDmaGetCmdqCtx(pCtx, &pCmdqCtx, E_HAL_DISP_DMA_CMDQ_CTX_TYPE_DEST);
    if(pCmdqCtx)
    {

        u16Val = (u32Val & 0x0000FFFF);
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_1C_L, u16Val, 0xFFFF, pCmdqCtx);
        u16Val = (u32Val >> 16);
        HalDispUtilityW2BYTEMSK(REG_DISP_TOP_1_1D_L, u16Val, 0xFFFF, pCmdqCtx);
    }
}

