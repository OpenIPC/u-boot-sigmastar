/*
* hal_disp_utility.c- Sigmastar
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

#define __HAL_DISP_UTILITY_C__

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "mhal_cmdq.h"
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_reg.h"
#include "hal_disp_util.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define RIU_GET_ADDR(addr) ((void *)(g_pvDispUtilityRiuBaseVir + ((addr) << 1)))

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispOsMutexConfig_t        g_stDispUtilityMutexCfg;
u32                           g_u32DispUtilityCmdqInitFlag = 0;
HalDispUtilityCmdqContext_t   g_stDispUtilityCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM];
HalDispUtilityRegAccessMode_e g_eDispRegAccessMode[HAL_DISP_UTILITY_CMDQ_NUM] = {
    DISP_REG_ACCESS_MODE, DISP_REG_ACCESS_MODE, DISP_REG_ACCESS_MODE};

static void *g_pvDispUtilityRiuBaseVir = NULL;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static void _HalDispUtilitySetInitFlagByIdx(u32 u32CtxId, u8 bEnable)
{
    if (bEnable)
    {
        g_u32DispUtilityCmdqInitFlag |= (1 << u32CtxId);
    }
    else
    {
        g_u32DispUtilityCmdqInitFlag &= ~(1 << u32CtxId);
    }
}

static u8 _HalDispUtilityGetInitFlagByIdx(u32 u32CtxId)
{
    return (g_u32DispUtilityCmdqInitFlag & (1 << u32CtxId)) ? 1 : 0;
}

static u32 _HalDispUtilityGetInitFlag(void)
{
    return g_u32DispUtilityCmdqInitFlag;
}

static void *_HalDispUtilityGetDirectCmdqBuffer(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return pstCmdqCtx->pvDirectCmdqBuffer;
}

static void *_HalDispUtilityGetDirectCmdqCnt(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return (void *)&pstCmdqCtx->u32DirectCmdqCnt;
}

static void *_HalDispUtilityGetCmdqCmdBuffer(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return pstCmdqCtx->pvCmdqCmdBuffer;
}

static void *_HalDispUtilityGetCmdqCmdCnt(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return (void *)&pstCmdqCtx->u32CmdqCmdCnt;
}

static u8 _HalDispUtilityAddWriteRegCmdqMaskMulti(MHAL_CMDQ_CmdqInterface_t *pstCmdqInf, void *pvCmdqBuffer,
                                                  u32 u32CmdqCmdCnt)
{
    u8                      bRet         = 1;
    s32                     s32CmdqRet   = 0;
    u32                     u32CmdqState = 0;
    u32                     i            = 0;
    HalDispUtilityCmdReg_t *pstCmdReg    = NULL;

    if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq && pvCmdqBuffer && u32CmdqCmdCnt)
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Cnt:%d, Buff:%px\n", __FUNCTION__, __LINE__, u32CmdqCmdCnt,
                 pvCmdqBuffer);

        for (i = 0; i < u32CmdqCmdCnt; i++)
        {
            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvCmdqBuffer + (i * sizeof(HalDispUtilityCmdReg_t)));
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04x%02x] Val [0x%04x] Mask [0x%04x]\n",
                     __FUNCTION__, __LINE__, (pstCmdReg->u32Addr & 0xFFFF00) >> 8, (pstCmdReg->u32Addr & 0xFF) >> 1,
                     pstCmdReg->u16Data, pstCmdReg->u16Mask);
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP_HWRD, "wriu -w 0x%04x%02x 0x%04hx \n", (pstCmdReg->u32Addr >> 8),
                     (pstCmdReg->u32Addr & 0xFF),
                     ((pstCmdReg->u16Data & pstCmdReg->u16Mask)
                      | HalDispUtilityR2BYTEMaskDirect(pstCmdReg->u32Addr, ~pstCmdReg->u16Mask)));
        }

        while (0 == (s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CheckBufAvailable(pstCmdqInf, u32CmdqCmdCnt + 10)))
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq(pstCmdqInf, &u32CmdqState);
            if (s32CmdqRet)
            {
                // DISP_ERR("%s %d, MHAL_CMDQ_ReadStatusCmdq Error\n", __FUNCTION__, __LINE__);
            }

            if ((u32CmdqState & MHAL_CMDQ_ERROR_STATUS) != 0)
            {
                DISP_ERR("%s %d, Cmdq Status Error\n", __FUNCTION__, __LINE__);
                pstCmdqInf->MHAL_CMDQ_CmdqResetEngine(pstCmdqInf);
            }
        }

        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti(
            pstCmdqInf, (MHAL_CMDQ_MultiCmdBufMask_t *)pvCmdqBuffer, u32CmdqCmdCnt);
        if (s32CmdqRet < u32CmdqCmdCnt)
        {
            DISP_ERR("%s %d, DirectCmdqTbl Cmdq Write Multi Fail, Ret [%d] Cnt [%d]\n", __FUNCTION__, __LINE__,
                     s32CmdqRet, u32CmdqCmdCnt);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, pstCmdqInf:%px, CmdqCnt:%d, Buffer:%px\n", __FUNCTION__, __LINE__, pstCmdqInf, u32CmdqCmdCnt,
                 pvCmdqBuffer);
    }
    DISP_DBG_VAL(pstCmdReg);
    return bRet;
}

static void _HalDispUtilityDirectCmdqWriteWithCmdqCmd(void *pvCtxIn)
{
    u32                          i                 = 0;
    HalDispUtilityCmdqCmd_t *    pstCmdqCmdPre     = NULL;
    HalDispUtilityCmdqCmd_t *    pstCmdqCmdCur     = NULL;
    HalDispUtilityCmdReg_t *     pvCmdqBuffer      = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf        = NULL;
    u32 *                        pu32DirectCmdqCnt = NULL;
    void *                       pvDirctCmdqBuffer = NULL;
    u32 *                        pu32CmdqCmdCnt    = NULL;
    void *                       pvCmdqCmdBuffer   = NULL;
    u32                          u32CmdqBufferCnt  = 0;

    pstCmdqInf = pstCmdqCtx->pvCmdqInf;

    pvDirctCmdqBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

    pvCmdqCmdBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32CmdqCmdCnt  = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CmdqCmdCnt:%d, DirectCmdqCnt:%d\n", __FUNCTION__, __LINE__,
             *pu32CmdqCmdCnt, *pu32DirectCmdqCnt);

    for (i = 0; i < *pu32CmdqCmdCnt; i++)
    {
        pstCmdqCmdPre =
            i > 0 ? (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + ((i - 1) * sizeof(HalDispUtilityCmdqCmd_t))) : NULL;

        pstCmdqCmdCur = (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + (i * sizeof(HalDispUtilityCmdqCmd_t)));

        if (pstCmdqCmdPre)
        {
            u32CmdqBufferCnt = pstCmdqCmdCur->u32CmdqBufIdx - pstCmdqCmdPre->u32CmdqBufIdx;
            pvCmdqBuffer =
                (HalDispUtilityCmdReg_t *)(pvDirctCmdqBuffer
                                           + (pstCmdqCmdPre->u32CmdqBufIdx * sizeof(HalDispUtilityCmdReg_t)));
        }
        else
        {
            u32CmdqBufferCnt = pstCmdqCmdCur->u32CmdqBufIdx;
            pvCmdqBuffer     = (HalDispUtilityCmdReg_t *)pvDirctCmdqBuffer;
        }

        if (u32CmdqBufferCnt)
        {
            _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, (void *)pvCmdqBuffer, u32CmdqBufferCnt);
        }

        if (pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_DELAY_CMD)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, DelayCmd Time [%d]\n", __FUNCTION__, __LINE__,
                     pstCmdqCmdCur->u32Time);
            HalDispUtilityAddDelayCmd(pvCtxIn, pstCmdqCmdCur->u32Time);
        }
        else if (pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_WAITDONE_CMD)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, WaitDone\n", __FUNCTION__, __LINE__);
            HalDispUtilityAddWaitCmd(pvCtxIn);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP,
                     "%s %d, PollCmd [0x%04x%02x] Val [0x%04x] Mask [0x%04x] Time [%x]\n", __FUNCTION__, __LINE__,
                     (pstCmdqCmdCur->u32Addr & 0xFFFF00) >> 8, (pstCmdqCmdCur->u32Addr & 0xFF) >> 1,
                     pstCmdqCmdCur->u16Data, pstCmdqCmdCur->u16Mask, pstCmdqCmdCur->u32Time);

            HalDispUtilityPollWait((void *)pstCmdqInf, pstCmdqCmdCur->u32Addr, pstCmdqCmdCur->u16Data,
                                   pstCmdqCmdCur->u16Mask, pstCmdqCmdCur->u32Time, pstCmdqCmdCur->bPollEq);
        }
    }

    u32CmdqBufferCnt = *pu32DirectCmdqCnt - pstCmdqCmdCur->u32CmdqBufIdx;
    pvCmdqBuffer =
        (HalDispUtilityCmdReg_t *)(pvDirctCmdqBuffer + (pstCmdqCmdCur->u32CmdqBufIdx * sizeof(HalDispUtilityCmdReg_t)));

    if (u32CmdqBufferCnt)
    {
        _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, pvCmdqBuffer, u32CmdqBufferCnt);
    }
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

u8 HalDispUtilityInit(u32 u32UtilityId)
{
    u8 bRet = 1;

    if (u32UtilityId >= HAL_DISP_UTILITY_CMDQ_NUM)
    {
        DISP_ERR("%s %d, UtiId(%d) is bigger than CMDQ NUM (%d)\n", __FUNCTION__, __LINE__, u32UtilityId,
                 HAL_DISP_UTILITY_CMDQ_NUM);
        bRet = 0;
    }
    else
    {
        if (!_HalDispUtilityGetInitFlagByIdx(u32UtilityId))
        {
            if (_HalDispUtilityGetInitFlag() == 0)
            {
                g_pvDispUtilityRiuBaseVir = (void *)DISP_OS_VIR_RIUBASE;
                if (DrvDispOsCreateMutex(&g_stDispUtilityMutexCfg) == 0)
                {
                    DISP_ERR("%s %d, Create Mutex Fail\n", __FUNCTION__, __LINE__);
                    return 0;
                }
            }

            g_stDispUtilityCmdqCtx[u32UtilityId].s32UtilityId = u32UtilityId;

            g_stDispUtilityCmdqCtx[u32UtilityId].u16RegFlipCnt  = 0;
            g_stDispUtilityCmdqCtx[u32UtilityId].u16WaitDoneCnt = 0;

            g_stDispUtilityCmdqCtx[u32UtilityId].u32DirectCmdqCnt = 0;
            g_stDispUtilityCmdqCtx[u32UtilityId].pvDirectCmdqBuffer =
                DrvDispOsMemAlloc(sizeof(HalDispUtilityCmdReg_t) * HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);

            if (g_stDispUtilityCmdqCtx[u32UtilityId].pvDirectCmdqBuffer == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, Cmdq:%d, DirectCmdqBUffer Memallc Failn", __FUNCTION__, __LINE__, u32UtilityId);
            }

            if (DrvDispOsCreateMutex(&g_stDispUtilityCmdqCtx[u32UtilityId].stMutxCfg) == 0)
            {
                bRet = 0;
                DISP_ERR("%s %d, CreateMutex Fail\n", __FUNCTION__, __LINE__);
            }

            g_stDispUtilityCmdqCtx[u32UtilityId].u32CmdqCmdCnt = 0;
            g_stDispUtilityCmdqCtx[u32UtilityId].pvCmdqCmdBuffer =
                DrvDispOsMemAlloc(sizeof(HalDispUtilityCmdqCmd_t) * HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            if (g_stDispUtilityCmdqCtx[u32UtilityId].pvCmdqCmdBuffer == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, Cmdq:%d, PollCmdq Buffer Memallc Failn", __FUNCTION__, __LINE__, u32UtilityId);
            }

            _HalDispUtilitySetInitFlagByIdx(u32UtilityId, 1);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqCtx (%d) already Init\n", __FUNCTION__, __LINE__,
                     u32UtilityId);
            return 1;
        }
    }
    return bRet;
}

u8 HalDispUtilityDeInit(u32 u32CtxId)
{
    if (_HalDispUtilityGetInitFlagByIdx(u32CtxId))
    {
        g_stDispUtilityCmdqCtx[u32CtxId].s32UtilityId   = -1;
        g_stDispUtilityCmdqCtx[u32CtxId].pvCmdqInf      = NULL;
        g_stDispUtilityCmdqCtx[u32CtxId].u16RegFlipCnt  = 0;
        g_stDispUtilityCmdqCtx[u32CtxId].u16WaitDoneCnt = 0;
        g_stDispUtilityCmdqCtx[u32CtxId].u32CmdqCmdCnt  = 0;
        DrvDispOsMemRelease(g_stDispUtilityCmdqCtx[u32CtxId].pvCmdqCmdBuffer);

        g_stDispUtilityCmdqCtx[u32CtxId].u32DirectCmdqCnt = 0;
        DrvDispOsMemRelease(g_stDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer);

        _HalDispUtilitySetInitFlagByIdx(u32CtxId, 0);

        DrvDispOsDestroyMutex(&g_stDispUtilityCmdqCtx[u32CtxId].stMutxCfg);
    }

    if (_HalDispUtilityGetInitFlag() == 0)
    {
        DrvDispOsDestroyMutex(&g_stDispUtilityMutexCfg);
    }

    return 1;
}

u8 HalDispUtilityGetCmdqContext(void **pvCtx, u32 u32CtxId)
{
    u8 bRet = 1;
    if (g_u32DispUtilityCmdqInitFlag)
    {
        *pvCtx = (void *)&g_stDispUtilityCmdqCtx[u32CtxId];
    }
    else
    {
        *pvCtx = NULL;
        bRet   = 0;
        DISP_ERR("%s %d, No Disp Utility Init\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

void HalDispUtilitySetCmdqInf(void *pCmdqInf, u32 u32CtxId)
{
    g_stDispUtilityCmdqCtx[u32CtxId].pvCmdqInf = pCmdqInf;
}

u16 HalDispUtilityR2BYTEDirect(u32 u32Reg)
{
    u16 u16RetVal = 0;

    u16RetVal = R2BYTE(u32Reg);
    return u16RetVal;
}

u16 HalDispUtilityR2BYTEMaskDirect(u32 u32Reg, u16 u16Mask)
{
    u16 u16RetVal = 0;

    u16RetVal = R2BYTEMSK(u32Reg, u16Mask);
    return u16RetVal;
}

u32 HalDispUtilityR4BYTEDirect(u32 u32Reg)
{
    u32 u32RetVal = 0;

    u32RetVal = R4BYTE(u32Reg);
    return u32RetVal;
}

void HalDispUtilityWBYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    WBYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityWBYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    if (pstCmdqCtx && HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (u32Reg & 0x01)
        {
            u32Reg  = u32Reg & 0xFFFFFFFE;
            u16Val  = u16Val << 8;
            u16Mask = u16Mask << 8;
        }
        HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn, u32Reg, u16Val, u16Mask);
    }
    else
    {
        HalDispUtilityWBYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEDirect(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    W2BYTE(u32Reg, u16Val);
}

void HaDisplUtilityW2BYTE(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    if (pstCmdqCtx && HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn, u32Reg, u16Val, 0xFFFF);
    }
    else
    {
        HalDispUtilityW2BYTEDirect(u32Reg, u16Val, pvCtxIn);
    }
}
void HalDispUtilityW2BYTEMSKByType(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pCmdqCtx,
                                   HalDispUtilityRegAccessMode_e enRiuMode)
{
    if (enRiuMode == E_DISP_UTILITY_REG_ACCESS_CPU)
    {
        HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32Reg, u16Val, u16Mask);
    }
    else if (enRiuMode == E_DISP_UTILITY_REG_ACCESS_CMDQ_DIRECT)
    {
        HalDispUtilityW2BYTEMSKCmdq(u32Reg, u16Val, u16Mask, pCmdqCtx);
    }
    else
    {
        HalDispUtilityW2BYTEMSK(u32Reg, u16Val, u16Mask, pCmdqCtx);
    }
}
void HalDispUtilityW2BYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
    DISP_DBG(DISP_DBG_LEVEL_UTILITY_DW_HWRD, "wriu -w 0x%04x%02x 0x%04hx \n", (u32Reg >> 8), (u32Reg & 0xFF),
             ((u16Val & u16Mask) | HalDispUtilityR2BYTEMaskDirect(u32Reg, ~u16Mask)));
}

void HalDispUtilityW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    if (pstCmdqCtx && HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn, u32Reg, u16Val, u16Mask);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}
void HalDispUtilityW2BYTEMSKCmdq(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf = NULL;

    if (pstCmdqCtx && HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        pstCmdqInf = pstCmdqCtx->pvCmdqInf;
        if (pstCmdqCtx->pvCmdqInf && pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMask)
        {
            pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMask(pstCmdqInf, u32Reg, u16Val, u16Mask);
        }
    }
}

void HalDispUtilityAddW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf = NULL;
    HalDispUtilityCmdReg_t       stCmdReg;

    if (pstCmdqCtx)
    {
        pstCmdqInf = pstCmdqCtx->pvCmdqInf;
    }

    stCmdReg.u32Addr = u32Reg;
    stCmdReg.u16Data = u16Val;
    stCmdReg.u16Mask = u16Mask;

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04x%02x] Val [0x%04x] Mask [0x%04x]\n", __FUNCTION__,
             __LINE__, (stCmdReg.u32Addr & 0xFFFF00) >> 8, (stCmdReg.u32Addr & 0xFF) >> 1, stCmdReg.u16Data,
             stCmdReg.u16Mask);

    if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti)
    {
        _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, (void *)&stCmdReg, 1);
    }
    else
    {
        W2BYTEMSK(u32Reg, u16Val, u16Mask);
    }
}

void HalDispUtilityW2BYTEMSKDirectCmdq(void *pvCtxIn, u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    HalDispUtilityCmdReg_t *     pstCmdReg   = NULL;
    u32 *                        pu32TblCnt  = NULL;
    void *                       pvTblBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx  = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        DrvDispOsObtainMutex(&pstCmdqCtx->stMutxCfg);

        pvTblBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
        pu32TblCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

        if (*pu32TblCnt >= HAL_DISP_UTILITY_DIRECT_CMDQ_CNT)
        {
            DISP_ERR("%s %d, Cnt [%d] over max [%d] \n", __FUNCTION__, __LINE__, *pu32TblCnt,
                     HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);
        }
        else
        {
            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvTblBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
            pstCmdReg->u32Addr = u32Reg;
            pstCmdReg->u16Mask = u16Mask;
            pstCmdReg->u16Data = u16Val;
            (*pu32TblCnt)++;
        }

        DrvDispOsReleaseMutex(&pstCmdqCtx->stMutxCfg);
    }
    else
    {
        W2BYTEMSK(u32Reg, u16Val, u16Mask);
    }
}
void HalDispUtilityKickoffCmdq(void *pvCtxIn)
{
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf        = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = pvCtxIn;
    pstCmdqInf = pstCmdqCtx->pvCmdqInf;
    if(pstCmdqInf && pstCmdqInf->MHAL_CMDQ_KickOffCmdq)
    {
        pstCmdqInf->MHAL_CMDQ_KickOffCmdq(pstCmdqInf);
    }
}
void HalDispUtilityW2BYTEMSKDirectCmdqWrite(void *pvCtxIn)
{
    u32                          i                 = 0;
    u32                          j                 = 0;
    HalDispUtilityCmdReg_t *     pstCmdReg         = 0;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    HalDispUtilityCmdqCmd_t *    pstCmdqCmdCur     = NULL;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf        = NULL;
    u32 *                        pu32DirectCmdqCnt = NULL;
    void *                       pvDirctCmdqBuffer = NULL;
    u32 *                        pu32CmdqCmdCnt    = NULL;
    void *                       pvCmdqCmdBuffer   = NULL;

    if (pstCmdqCtx)
    {
        pstCmdqInf = pstCmdqCtx->pvCmdqInf;
    }

    DrvDispOsObtainMutex(&pstCmdqCtx->stMutxCfg);

    pvDirctCmdqBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

    pvCmdqCmdBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32CmdqCmdCnt  = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (pvDirctCmdqBuffer && *pu32DirectCmdqCnt > 0)
    {
        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Use Cmdq_%d \n", __FUNCTION__, __LINE__,
                     pstCmdqCtx->s32UtilityId);

            if (pvCmdqCmdBuffer && *pu32CmdqCmdCnt)
            {
                _HalDispUtilityDirectCmdqWriteWithCmdqCmd(pvCtxIn);
            }
            else
            {
                _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, pvDirctCmdqBuffer, *pu32DirectCmdqCnt);
            }
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Use RIU\n", __FUNCTION__, __LINE__);

            for (i = 0, j = 0; i < *pu32DirectCmdqCnt; i++)
            {
                if (*pu32CmdqCmdCnt)
                {
                    pstCmdqCmdCur =
                        (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + (j * sizeof(HalDispUtilityCmdqCmd_t)));

                    if (pstCmdqCmdCur->u32CmdqBufIdx == i)
                    {
                        if (pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_DELAY_CMD)
                        {
                            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, DelayCmd [%d]\n", __FUNCTION__, __LINE__,
                                     pstCmdqCmdCur->u32Time);
                            HalDispUtilityAddDelayCmd(pvCtxIn, pstCmdqCmdCur->u32Time);
                        }
                        else if (pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_WAITDONE_CMD)
                        {
                            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, WaitDone\n", __FUNCTION__, __LINE__);
                            HalDispUtilityAddWaitCmd(pvCtxIn);
                        }
                        else
                        {
                            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP,
                                     "%s %d, Poll [0x%04x%02x] Val [0x%04x] Mask [0x%04x]\n", __FUNCTION__, __LINE__,
                                     (pstCmdqCmdCur->u32Addr & 0xFFFF00) >> 8, (pstCmdqCmdCur->u32Addr & 0xFF) >> 1,
                                     pstCmdqCmdCur->u16Data, pstCmdqCmdCur->u16Mask);

                            HalDispUtilityPollWait(pstCmdqInf, pstCmdqCmdCur->u32Addr, pstCmdqCmdCur->u16Data,
                                                   pstCmdqCmdCur->u16Mask, pstCmdqCmdCur->u32Time,
                                                   pstCmdqCmdCur->bPollEq);
                        }
                        j++;
                    }
                }

                pstCmdReg = (HalDispUtilityCmdReg_t *)(pvDirctCmdqBuffer + (i * sizeof(HalDispUtilityCmdReg_t)));
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04x%02x] Val [0x%04x] Mask [0x%04x]\n",
                         __FUNCTION__, __LINE__, (pstCmdReg->u32Addr & 0xFFFF00) >> 8, (pstCmdReg->u32Addr & 0xFF) >> 1,
                         pstCmdReg->u16Data, pstCmdReg->u16Mask);

                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP_HWRD, "wriu -w 0x%04x%02x 0x%04hx \n", (pstCmdReg->u32Addr >> 8),
                         (pstCmdReg->u32Addr & 0xFF),
                         ((pstCmdReg->u16Data & pstCmdReg->u16Mask)
                          | HalDispUtilityR2BYTEMaskDirect(pstCmdReg->u32Addr, ~pstCmdReg->u16Mask)));
                W2BYTEMSK(pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);
            }
        }

        (*pu32DirectCmdqCnt) = 0;
        (*pu32CmdqCmdCnt)    = 0;
    }

    DrvDispOsReleaseMutex(&pstCmdqCtx->stMutxCfg);
}

void HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
}
u8 HalDispUtilityWritePollCmdByType(u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq, void *pvCtxIn,
                                    HalDispUtilityRegAccessMode_e enRiuMode)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    u8                           u8Ret      = 0;

    if (enRiuMode == E_DISP_UTILITY_REG_ACCESS_CPU)
    {
        u8Ret = HalDispUtilityPollWait(NULL, u32Reg, u16Val, u16Mask, u32PollTime, bPollEq);
    }
    else if (enRiuMode == E_DISP_UTILITY_REG_ACCESS_CMDQ_DIRECT)
    {
        u8Ret = HalDispUtilityPollWait(pstCmdqCtx->pvCmdqInf, u32Reg, u16Val, u16Mask, u32PollTime, bPollEq);
    }
    else
    {
        HalDispUtilityWritePollCmd(u32Reg, u16Val, u16Mask, u32PollTime, bPollEq, pvCtxIn);
    }
    return u8Ret;
}
void HalDispUtilityWritePollCmd(u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq, void *pvCtxIn)
{
    HalDispUtilityCmdqCmd_t *    pstCmdqCmd        = NULL;
    u32 *                        pu32DirectCmdqCnt = NULL;
    u32 *                        pu32CmdqCmdCnt    = NULL;
    void *                       pvTblBuffer       = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer       = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);
    pu32CmdqCmdCnt    = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32CmdqCmdCnt >= HAL_DISP_UTILITY_CMDQ_CMD_CNT)
        {
            DISP_ERR("%s %d, CmdqPollCmd Cnt [%d] over max [%d] \n", __FUNCTION__, __LINE__, *pu32CmdqCmdCnt,
                     HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            return;
        }

        pstCmdqCmd = (HalDispUtilityCmdqCmd_t *)(pvTblBuffer + ((*pu32CmdqCmdCnt) * sizeof(HalDispUtilityCmdqCmd_t)));
        pstCmdqCmd->u32CmdqBufIdx = *pu32DirectCmdqCnt;
        pstCmdqCmd->u32Addr       = u32Reg;
        pstCmdqCmd->u16Mask       = u16Mask;
        pstCmdqCmd->u16Data       = u16Val;
        pstCmdqCmd->u32Time       = u32PollTime;
        pstCmdqCmd->bPollEq       = bPollEq;
        (*pu32CmdqCmdCnt)++;
    }
    else
    {
        HalDispUtilityPollWait(NULL, u32Reg, u16Val, u16Mask, u32PollTime, bPollEq);
    }
}

void HalDispUtilityWriteDelayCmd(u32 u32PollingTime, void *pvCtxIn)
{
    HalDispUtilityCmdqCmd_t *    pstCmdqCmd        = NULL;
    u32 *                        pu32DirectCmdqCnt = NULL;
    u32 *                        pu32CmdqCmdCnt    = NULL;
    void *                       pvTblBuffer       = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer       = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);
    pu32CmdqCmdCnt    = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32CmdqCmdCnt >= HAL_DISP_UTILITY_CMDQ_CMD_CNT)
        {
            DISP_ERR("%s %d, CmdqPollCmd Cnt [%d] over max [%d] \n", __FUNCTION__, __LINE__, *pu32CmdqCmdCnt,
                     HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            return;
        }

        pstCmdqCmd = (HalDispUtilityCmdqCmd_t *)(pvTblBuffer + ((*pu32CmdqCmdCnt) * sizeof(HalDispUtilityCmdqCmd_t)));
        pstCmdqCmd->u32CmdqBufIdx = *pu32DirectCmdqCnt;
        pstCmdqCmd->u32Addr       = HAL_DISP_UTILITY_CMDQ_DELAY_CMD;
        pstCmdqCmd->u16Mask       = 0xFFFF;
        pstCmdqCmd->u16Data       = 0xFFFF;
        pstCmdqCmd->u32Time       = u32PollingTime;
        pstCmdqCmd->bPollEq       = 0;
        (*pu32CmdqCmdCnt)++;
    }
}

void HalDispUtilityWriteWaitDoneCmd(void *pvCtxIn)
{
    HalDispUtilityCmdqCmd_t *    pstCmdqCmd        = NULL;
    u32 *                        pu32DirectCmdqCnt = NULL;
    u32 *                        pu32CmdqCmdCnt    = NULL;
    void *                       pvTblBuffer       = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx        = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer       = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);
    pu32CmdqCmdCnt    = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32CmdqCmdCnt >= HAL_DISP_UTILITY_CMDQ_CMD_CNT)
        {
            DISP_ERR("%s %d, CmdqPollCmd Cnt [%d] over max [%d] \n", __FUNCTION__, __LINE__, *pu32CmdqCmdCnt,
                     HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            return;
        }

        pstCmdqCmd = (HalDispUtilityCmdqCmd_t *)(pvTblBuffer + ((*pu32CmdqCmdCnt) * sizeof(HalDispUtilityCmdqCmd_t)));
        pstCmdqCmd->u32CmdqBufIdx = *pu32DirectCmdqCnt;
        pstCmdqCmd->u32Addr       = HAL_DISP_UTILITY_CMDQ_WAITDONE_CMD;
        pstCmdqCmd->u16Mask       = 0xFFFF;
        pstCmdqCmd->u16Data       = 0xFFFF;
        pstCmdqCmd->u32Time       = 0xFFFF;
        pstCmdqCmd->bPollEq       = 0;
        (*pu32CmdqCmdCnt)++;
    }
}

u8 HalDispUtilityPollWait(void *pvCmdqInf, u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq)

{
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = (MHAL_CMDQ_CmdqInterface_t *)pvCmdqInf;
    s32                        s32CmdqRet = 0;
    u32                        u32WaitCnt = 0;
    u8                         bRet       = 1;

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Poll [0x%04x%02x] Val [0x%04x] Mask [0x%04x] Time:%d, PollEq:%d\n",
             __FUNCTION__, __LINE__, (u32Reg & 0xFFFF00) >> 8, (u32Reg & 0xFF) >> 1, u16Val, u16Mask, u32PollTime,
             bPollEq);

    if (pstCmdqInf == NULL || pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits == NULL)
    {
        while ((bPollEq && (HalDispUtilityR2BYTEMaskDirect(u32Reg, u16Mask) != (u16Val)))
               || (!bPollEq && (HalDispUtilityR2BYTEMaskDirect(u32Reg, u16Mask) == (u16Val))))
        {
            if (++u32WaitCnt > 2000)
            {
                return 1;
            }
            DrvDispOsUsSleep(((u32PollTime / (1000 * 2000)) ? (u32PollTime / (1000 * 2000)) : 1)); // Wait time ns/1000
        }
    }
    else
    {
        if (u32PollTime)
        {
            s32CmdqRet =
                pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits_ByTime(pstCmdqInf, u32Reg, u16Val, u16Mask, bPollEq, u32PollTime);
            if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
            {
                bRet = 0;
                DISP_ERR("%s %d, CmdqPollRegBits_ByTime [%u] Fail\n", __FUNCTION__, __LINE__, u32PollTime);
            }
        }
        else
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits(pstCmdqInf, u32Reg, u16Val, u16Mask, bPollEq);
            if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
            {
                bRet = 0;
                DISP_ERR("%s %d, MHAL_CMDQ_CmdqPollRegBits Fail\n", __FUNCTION__, __LINE__);
            }
        }
    }
    return bRet;
}

u8 HalDispUtilityAddWaitCmd(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx  = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf  = NULL;
    MHAL_CMDQ_EventId_e          enCmdqEvent = 0;
    s32                          s32CmdqRet  = 0;
    u8                           bRet        = 1;

    if (HalDispUtilityGetRegAccessMode((u32)pstCmdqCtx->s32UtilityId) == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (pstCmdqCtx)
        {
            pstCmdqInf = pstCmdqCtx->pvCmdqInf;
        }

        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd)
        {
            enCmdqEvent = HAL_DISP_UTILITY_CMDQDEV_WAIT_DONE_EVENT(pstCmdqCtx->s32UtilityId);
            s32CmdqRet  = pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdqInf, enCmdqEvent);
            if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
            {
                bRet = 0;
                DISP_ERR("%s %d, CmdqAddWaitEventCmd Fail\n", __FUNCTION__, __LINE__);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d\n", __FUNCTION__, __LINE__);
            }
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, NULL CmdqInf\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 HalDispUtilityAddDelayCmd(void *pvCtxIn, u32 u32PollingTime)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *  pstCmdqInf = NULL;
    s32                          s32CmdqRet = 0;
    u8                           bRet       = 1;

    if (pstCmdqCtx)
    {
        pstCmdqInf = pstCmdqCtx->pvCmdqInf;
    }

    if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdDelay)
    {
        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdDelay(pstCmdqInf, 1000 * u32PollingTime);
        if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
        {
            bRet = 0;
            DISP_ERR("%s %d, CmdDelay Fail\r\n", __FUNCTION__, __LINE__);
        }
    }
    else
    {
        // if need
        if (u32PollingTime > 500)
        {
            DrvDispOsUsDelay(u32PollingTime);
        }
    }

    return bRet;
}

void HalDispUtilitySetRegAccessMode(u32 u32Id, u32 u32Mode)
{
    g_eDispRegAccessMode[u32Id] = u32Mode;
}

u32 HalDispUtilityGetRegAccessMode(u32 u32Id)
{
    return (u32)g_eDispRegAccessMode[u32Id];
}
