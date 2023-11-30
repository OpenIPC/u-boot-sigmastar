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
#define RIU_GET_ADDR(addr)          ((void*)(gu32DispUtilityRiuBaseVir + ((addr)<<1)))

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
DrvDispOsMutexConfig_t gstDispUtilityMutexCfg;
u32 u32DispUtilityCmdqInitFlag = 0;
HalDispUtilityCmdqContext_t gstDispUtilityCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM];
HalDispUtilityRegAccessMode_e geDispRegAccessMode = DISP_REG_ACCESS_MODE;
void *gpvRegBuffer = NULL;

static u32 gu32DispUtilityRiuBaseVir  = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void _HalDispUtilitySetInitFlagByIdx(u32 u32CtxId, bool bEnable)
{
    if(bEnable)
    {
        u32DispUtilityCmdqInitFlag |= (1<< u32CtxId);
    }
    else
    {
        u32DispUtilityCmdqInitFlag &= ~(1 << u32CtxId);
    }
}

bool _HalDispUtilityGetInitFlagByIdx(u32 u32CtxId)
{
    return (u32DispUtilityCmdqInitFlag & (1 << u32CtxId)) ? 1 : 0;
}

u32 _HalDispUtilityGetInitFlag(void)
{
    return u32DispUtilityCmdqInitFlag;
}

u32 _HalDispUtilityTransTypeToBank(u32 u32Type)
{
    u32 u32BankAddr = 0;

    switch(u32Type)
    {
        case E_DISP_UTILITY_REG_DISP_DAC_BASE:
            u32BankAddr = REG_DISP_DAC_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_0_BASE:
            u32BankAddr = REG_DISP_TOP_0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_OP2_0_BASE:
            u32BankAddr = REG_DISP_TOP_OP2_0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_MACE_BASE:
            u32BankAddr = REG_DISP_TOP_MACE_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_GAMMA_0_BASE:
            u32BankAddr = REG_DISP_TOP_GAMMA_0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_1_BASE:
            u32BankAddr = REG_DISP_TOP_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_OP2_1_BASE:
            u32BankAddr = REG_DISP_TOP_OP2_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_TOP_GAMMA_1_BASE:
            u32BankAddr = REG_DISP_TOP_GAMMA_1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK00_BASE:
            u32BankAddr = REG_DISP_MOPG_BK00_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK01_BASE:
            u32BankAddr = REG_DISP_MOPG_BK01_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK02_BASE:
            u32BankAddr = REG_DISP_MOPG_BK02_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPS_BK0_BASE:
            u32BankAddr = REG_DISP_MOPS_BK0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOP_ROT_BK0_BASE:
            u32BankAddr = REG_DISP_MOP_ROT_BK0_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK10_BASE:
            u32BankAddr = REG_DISP_MOPG_BK10_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK11_BASE:
            u32BankAddr = REG_DISP_MOPG_BK11_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPG_BK12_BASE:
            u32BankAddr = REG_DISP_MOPG_BK12_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOPS_BK1_BASE:
            u32BankAddr = REG_DISP_MOPS_BK1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_MOP_ROT_BK1_BASE:
            u32BankAddr = REG_DISP_MOP_ROT_BK1_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_DMA_BASE:
            u32BankAddr = REG_DISP_DMA_BASE;
            break;
        case E_DISP_UTILITY_REG_CLKGEN_BASE:
            u32BankAddr = REG_CLKGEN_BASE;
            break;
        case E_DISP_UTILITY_REG_LCD_MISC_BASE:
            u32BankAddr = REG_LCD_MISC_BASE;
            break;
        case E_DISP_UTILITY_REG_TVENC_BASE:
            u32BankAddr = REG_TVENCODER_BASE;
            break;
        case E_DISP_UTILITY_REG_DISP_LPLL_BASE:
            u32BankAddr = REG_DISP_LPLL_BASE;
            break;
        default:
            u32BankAddr = 0;
            break;
    }

    return u32BankAddr;
}

u32 _HalDispUtilityTransBankToType(u32 u32Bank, u32 u32CtxId)
{
    HalDispUtilityRegConfigType_e enType = E_DISP_UTILITY_REG_CONFIG_NUM;

    switch(u32Bank)
    {
        case REG_DISP_DAC_BASE:
            enType = E_DISP_UTILITY_REG_DISP_DAC_BASE;
            break;
        case REG_DISP_TOP_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_0_BASE;
            break;
        case REG_DISP_TOP_OP2_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_OP2_0_BASE;
            break;
        case REG_DISP_TOP_MACE_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_MACE_BASE;
            break;
        case REG_DISP_TOP_GAMMA_0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_GAMMA_0_BASE;
            break;
        case REG_DISP_TOP_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_1_BASE;
            break;
        case REG_DISP_TOP_OP2_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_OP2_1_BASE;
            break;
        case REG_DISP_TOP_GAMMA_1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_TOP_GAMMA_1_BASE;
            break;
        case REG_DISP_MOPG_BK00_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK00_BASE;
            break;
        case REG_DISP_MOPG_BK01_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK01_BASE;
            break;
        case REG_DISP_MOPG_BK02_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK02_BASE;
            break;
        case REG_DISP_MOPS_BK0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPS_BK0_BASE;
            break;
        case REG_DISP_MOP_ROT_BK0_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOP_ROT_BK0_BASE;
            break;
        case REG_DISP_MOPG_BK10_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK10_BASE;
            break;
        case REG_DISP_MOPG_BK11_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK11_BASE;
            break;
        case REG_DISP_MOPG_BK12_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPG_BK12_BASE;
            break;
        case REG_DISP_MOPS_BK1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOPS_BK1_BASE;
            break;
        case REG_DISP_MOP_ROT_BK1_BASE:
            enType = E_DISP_UTILITY_REG_DISP_MOP_ROT_BK1_BASE;
            break;
        case REG_DISP_DMA_BASE:
            enType = E_DISP_UTILITY_REG_DISP_DMA_BASE;
            break;
        case REG_CLKGEN_BASE:
            enType = E_DISP_UTILITY_REG_CLKGEN_BASE;
            break;
        case REG_LCD_MISC_BASE:
            enType = E_DISP_UTILITY_REG_LCD_MISC_BASE;
            break;
        case REG_TVENCODER_BASE:
            enType = E_DISP_UTILITY_REG_TVENC_BASE;
            break;
        case REG_DISP_LPLL_BASE:
            enType = E_DISP_UTILITY_REG_DISP_LPLL_BASE;
            break;
        default:
            enType = E_DISP_UTILITY_REG_CONFIG_NUM;
            DISP_ERR("%s %d, Unknown Bank: %lx\n", __FUNCTION__, __LINE__, u32Bank);
            break;
    }

    return (u32)enType;
}

void _HalDispUtilityReadBankCpyToBuffer(HalDispUtilityRegConfigType_e enType)
{
    u32 u32Bank = 0;
    void *pvBankAddr = NULL;

    if (gpvRegBuffer)
    {
        u32Bank     = _HalDispUtilityTransTypeToBank(enType);
        pvBankAddr  = RIU_GET_ADDR(u32Bank);

        DrvDispOsObtainMutex(&gstDispUtilityMutexCfg);
        memcpy((void *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType)), (void *)pvBankAddr, HAL_DISP_UTILITY_REG_BANK_SIZE);
        DrvDispOsReleaseMutex(&gstDispUtilityMutexCfg);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Type [%d] Bank [%lx] Addr [%lx]\n",
                __FUNCTION__, __LINE__,
                enType, u32Bank, (u32)pvBankAddr);

        if( DISP_DBG_CHECK(DISP_DBG_LEVEL_UTILITY_CMDQ) )
        {
            u8 *pu8Src = DrvDispOsMemAlloc(512 * 8);
            u8 *pu8RegBuf = (u8 *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType));
            u16 idx;

            if(pu8Src)
            {
                DrvDispOsSprintfStrcat(pu8Src, "Bank:%lx\n", u32Bank);
                for(idx=0; idx<0x80; idx++)
                {
                    if( (idx%8) == 0)
                    {
                        DrvDispOsSprintfStrcat(pu8Src, "%02X: ", idx);
                    }

                    DrvDispOsSprintfStrcat(pu8Src, "0x%02X%02X ", pu8RegBuf[idx*4+1], pu8RegBuf[idx*4]);

                    if( ((idx+1)%8) == 0 )
                    {
                        DrvDispOsSprintfStrcat(pu8Src, "\n");
                    }
                }
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s\n", pu8Src);
                DrvDispOsMemRelease(pu8Src);
            }
        }
    }
}

void _HalDispUtilityReadBufferCpyToBank(HalDispUtilityRegConfigType_e enType)
{
    u32 u32Bank = 0;
    void *pvBankAddr = NULL;

    if (gpvRegBuffer)
    {
        u32Bank     = _HalDispUtilityTransTypeToBank(enType);
        pvBankAddr  = RIU_GET_ADDR(u32Bank);

        DrvDispOsObtainMutex(&gstDispUtilityMutexCfg);
        memcpy((void *)pvBankAddr, (void *)(gpvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*enType)), HAL_DISP_UTILITY_REG_BANK_SIZE);
        DrvDispOsReleaseMutex(&gstDispUtilityMutexCfg);

        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, Type [%d] Bank [%lx] Addr [%lx]\n",
                __FUNCTION__, __LINE__,
                enType, u32Bank, (u32)pvBankAddr);
    }
}

u32 _HalDispUtilityGetMaxType(void)
{
    return (u32)E_DISP_UTILITY_REG_CONFIG_NUM;
}

void* _HalDispUtilityGetRegBuffer(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return pstCmdqCtx->pvRegBuffer;
}

void* _HalDispUtilityGetDirectCmdqBuffer(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return pstCmdqCtx->pvDirectCmdqBuffer;
}

void* _HalDispUtilityGetDirectCmdqCnt(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return (void *)&pstCmdqCtx->u32DirectCmdqCnt;
}

void* _HalDispUtilityGetCmdqCmdBuffer(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return pstCmdqCtx->pvCmdqCmdBuffer;
}

void* _HalDispUtilityGetCmdqCmdCnt(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    return (void *)&pstCmdqCtx->u32CmdqCmdCnt;
}

void _HalDispUtilityFillCmdqStruct(HalDispUtilityInquire_t* pstInq, u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    u32 u32Bank = 0;
    HalDispUtilityCmdqContext_t *pvCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    u32Bank = (u32Reg & 0xFFFF00);

    pstInq->enType          = _HalDispUtilityTransBankToType(u32Bank, (u32)pvCmdqCtx->s32CmdqId);
    pstInq->u32TypeMax      = _HalDispUtilityGetMaxType();
    pstInq->stCmd.u32Addr   = u32Reg;
    pstInq->stCmd.u16Mask   = u16Mask;
    pstInq->stCmd.u16Data   = u16Val;
    pstInq->bSet            = (pstInq->enType < pstInq->u32TypeMax) ? 1 : 0;
    pstInq->pCtx            = pvCtxIn;
}

u16 _HalDispUtilityReadRegBuffer(u32 u32Reg, void *pvRegBuffer, u32 u32CtxId)
{
    u32 u32Bank = 0;
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;
    u32 u32Type = 0;
    u32 u32TypeMax = 0;

    if (pvRegBuffer)
    {
        u32Bank = (u32Reg & 0xFFFF00);
        u16Addr = (u16)(u32Reg & 0xFF);

        u32Type     = _HalDispUtilityTransBankToType(u32Bank, u32CtxId);
        u32TypeMax  = _HalDispUtilityGetMaxType();

        if (u32Type < u32TypeMax)
        {
            pu16BankAddr = (u16 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*u32Type) + (u16Addr<<1));
            return *pu16BankAddr;
        }
        else
        {
            DISP_ERR("%s %d, Type [%ld] Wrong\n", __FUNCTION__, __LINE__, u32Type);
            return 0;
        }
    }
    else
    {
        return 0;
    }
}


u8 _HalDispUtilityWrite1ByteRegBuffer(HalDispUtilityInquire_t* pstInq)
{
    u16 u16Addr = 0;
    u8 *pu8BankAddr = NULL;
    void *pvRegBuffer = NULL;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    u16Addr   = (u16)(pstInq->stCmd.u32Addr & 0xFF);

    if (pstInq->bSet && pvRegBuffer)
    {
        pu8BankAddr = (u8 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE*pstInq->enType) + (u16Addr));
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, W1Byte : Reg [%06lx] Val [%02hx] Mask [%02hx] OriRegVal [%02hx]\n",
            __FUNCTION__, __LINE__,
            pstInq->stCmd.u32Addr, (u8)pstInq->stCmd.u16Data, (u8)pstInq->stCmd.u16Mask, (u8)*pu8BankAddr);

        if (pstInq->stCmd.u16Mask == 0xFF)
        {
            *pu8BankAddr = (u8)pstInq->stCmd.u16Data;
        }
        else
        {
            *pu8BankAddr = (u8)(((u8)(pstInq->stCmd.u16Data & pstInq->stCmd.u16Mask)) | (*pu8BankAddr & ((u8)(~pstInq->stCmd.u16Mask))));
        }

        return 1;
    }
    else
    {
        return 0;
    }
}

u16 _HalDispUtilityWrite2ByteRegBuffer(HalDispUtilityInquire_t* pstInq)
{
    u16 u16Addr = 0;
    u16 *pu16BankAddr = NULL;
    void *pvRegBuffer = NULL;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(((HalDispUtilityCmdqContext_t*)(pstInq->pCtx)));
    u16Addr     = (u16)(pstInq->stCmd.u32Addr & 0xFF);

    if (pstInq->bSet && pvRegBuffer)
    {
        pu16BankAddr = (u16 *)(pvRegBuffer + (HAL_DISP_UTILITY_REG_BANK_SIZE * pstInq->enType) + (u16Addr));
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, W2Byte : Reg [%lx] Val [%04hx] Mask [%04hx] OriRegVal [%04hx]\n",
            __FUNCTION__, __LINE__,
            pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask, *pu16BankAddr);

        if (pstInq->stCmd.u16Mask == 0xFFFF)
        {
            *pu16BankAddr = pstInq->stCmd.u16Data;
        }
        else
        {
            *pu16BankAddr = (u16)((pstInq->stCmd.u16Data & pstInq->stCmd.u16Mask) | (*pu16BankAddr & (~pstInq->stCmd.u16Mask)));
        }

        return 1;
    }
    else
    {
        DISP_ERR("%s %d, W2Byte Fail : Type [%d] MaxType [%lu] Reg [%lx] Val [%hx] Mask [%hx]\n",
            __FUNCTION__, __LINE__,
                pstInq->enType, pstInq->u32TypeMax, pstInq->stCmd.u32Addr, pstInq->stCmd.u16Data, pstInq->stCmd.u16Mask);
        return 0;
    }
}

bool  _HalDispUtilityAddWriteRegCmdqMaskMulti(MHAL_CMDQ_CmdqInterface_t *pstCmdqInf, void *pvCmdqBuffer, u32 u32CmdqCmdCnt)
{
    bool bRet = 1;
    s32 s32CmdqRet = 0;
    u32 u32CmdqState = 0;
    u32 i = 0;
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;

    if(pstCmdqInf && pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq && pvCmdqBuffer && u32CmdqCmdCnt)
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Cnt:%ld, Buff:%p\n",
            __FUNCTION__, __LINE__, u32CmdqCmdCnt, pvCmdqBuffer);

        for (i = 0; i < u32CmdqCmdCnt; i++)
        {
            pstCmdReg = (HalDispUtilityCmdReg_t *)(pvCmdqBuffer + (i * sizeof(HalDispUtilityCmdReg_t)));
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx%02lx] Val [0x%04x] Mask [0x%04x]\n",
                __FUNCTION__, __LINE__,
                (pstCmdReg->u32Addr & 0xFFFF00) >> 8,
                (pstCmdReg->u32Addr & 0xFF) >> 1,
                pstCmdReg->u16Data,
                pstCmdReg->u16Mask);
        }

        while (0 == (s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CheckBufAvailable(pstCmdqInf, u32CmdqCmdCnt + 10)))
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_ReadStatusCmdq(pstCmdqInf, (MS_U32*)&u32CmdqState);
            if (s32CmdqRet)
            {
                //DISP_ERR("%s %d, MHAL_CMDQ_ReadStatusCmdq Error\n", __FUNCTION__, __LINE__);
            }

            if ((u32CmdqState & MHAL_CMDQ_ERROR_STATUS) != 0)
            {
                DISP_ERR("%s %d, Cmdq Status Error\n", __FUNCTION__, __LINE__);
                pstCmdqInf->MHAL_CMDQ_CmdqResetEngine(pstCmdqInf);
            }

        }

        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti(pstCmdqInf,
                                                                (MHAL_CMDQ_MultiCmdBufMask_t *)pvCmdqBuffer,
                                                                u32CmdqCmdCnt);
        if (s32CmdqRet < u32CmdqCmdCnt)
        {
            DISP_ERR("%s %d, DirectCmdqTbl Cmdq Write Multi Fail, Ret [%ld] Cnt [%ld]\n",
                __FUNCTION__, __LINE__, s32CmdqRet, u32CmdqCmdCnt);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, pstCmdqInf:%p, CmdqCnt:%ld, Buffer:%p\n",
            __FUNCTION__, __LINE__,
            pstCmdqInf, u32CmdqCmdCnt, pvCmdqBuffer);
    }
    return bRet;
}

void _HalDispUtilityDirectCmdqWriteWithCmdqCmd(void *pvCtxIn)
{
    u32 i = 0;
    HalDispUtilityCmdqCmd_t *pstCmdqCmdPre = NULL;
    HalDispUtilityCmdqCmd_t *pstCmdqCmdCur = NULL;
    HalDispUtilityCmdReg_t *pvCmdqBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    u32 *pu32DirectCmdqCnt = NULL;
    void *pvDirctCmdqBuffer = NULL;
    u32 *pu32CmdqCmdCnt = NULL;
    void *pvCmdqCmdBuffer = NULL;
    u32 u32CmdqBufferCnt = 0;

    pstCmdqInf = pstCmdqCtx->pvCmdqInf;

    pvDirctCmdqBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

    pvCmdqCmdBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32CmdqCmdCnt = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CmdqCmdCnt:%ld, DirectCmdqCnt:%ld\n",
        __FUNCTION__, __LINE__, *pu32CmdqCmdCnt, *pu32DirectCmdqCnt);

    for( i = 0; i < *pu32CmdqCmdCnt; i++)
    {
        pstCmdqCmdPre =
            i > 0 ?
          (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + ((i-1)* sizeof(HalDispUtilityCmdqCmd_t))) :
          NULL;

        pstCmdqCmdCur =
            (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + (i* sizeof(HalDispUtilityCmdqCmd_t)));

        if(pstCmdqCmdPre)
        {
            u32CmdqBufferCnt = pstCmdqCmdCur->u32CmdqBufIdx - pstCmdqCmdPre->u32CmdqBufIdx;
            pvCmdqBuffer = (HalDispUtilityCmdReg_t *)(pvDirctCmdqBuffer + (pstCmdqCmdPre->u32CmdqBufIdx * sizeof(HalDispUtilityCmdReg_t)));
        }
        else
        {
            u32CmdqBufferCnt = pstCmdqCmdCur->u32CmdqBufIdx;
            pvCmdqBuffer = (HalDispUtilityCmdReg_t *)pvDirctCmdqBuffer;
        }

        _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, (void *)pvCmdqBuffer, u32CmdqBufferCnt);

        if(pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_DELAY_CMD)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, DelayCmd Time [%lx]\n",
                __FUNCTION__, __LINE__, pstCmdqCmdCur->u32Time);
            HalDispUtilityAddDelayCmd(pvCtxIn, pstCmdqCmdCur->u32Time);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, PollCmd [0x%04lx%02lx] Val [0x%04x] Mask [0x%04x] Time [%lx]\n",
                __FUNCTION__, __LINE__,
                (pstCmdqCmdCur->u32Addr & 0xFFFF00) >> 8,
                (pstCmdqCmdCur->u32Addr & 0xFF) >> 1,
                pstCmdqCmdCur->u16Data,
                pstCmdqCmdCur->u16Mask,
                pstCmdqCmdCur->u32Time);

            HalDispUtilityPollWait((void *)pstCmdqInf,
                pstCmdqCmdCur->u32Addr,
                pstCmdqCmdCur->u16Data,
                pstCmdqCmdCur->u16Mask,
                pstCmdqCmdCur->u32Time,
                pstCmdqCmdCur->bPollEq);

            HalDispUtilityAddWaitCmd(pvCtxIn);
        }
    }

    u32CmdqBufferCnt = *pu32DirectCmdqCnt - pstCmdqCmdCur->u32CmdqBufIdx;
    pvCmdqBuffer = (void *)(pvDirctCmdqBuffer + (pstCmdqCmdCur->u32CmdqBufIdx * sizeof(HalDispUtilityCmdReg_t)));

    _HalDispUtilityAddWriteRegCmdqMaskMulti(pstCmdqInf, pvCmdqBuffer, u32CmdqBufferCnt);

}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------

bool HalDispUtilityInit(u32 u32CtxId)
{
    bool bRet = 1;
    u16 i;

    if(u32CtxId < HAL_DISP_UTILITY_CMDQ_NUM)
    {
        if( _HalDispUtilityGetInitFlagByIdx(u32CtxId) )
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, CmdqCtx (%ld) already Init\n", __FUNCTION__, __LINE__, u32CtxId);
            return 1;
        }
        else
        {
            if( _HalDispUtilityGetInitFlag() == 0)
            {
                gu32DispUtilityRiuBaseVir = DISP_OS_VIR_RIUBASE;
                if(DrvDispOsCreateMutex(&gstDispUtilityMutexCfg) == 0)
                {
                    DISP_ERR("%s %d, Create Mutex Fail\n", __FUNCTION__, __LINE__);
                    return 0;
                }

                gpvRegBuffer = DrvDispOsMemAlloc(HAL_DISP_UTILITY_REG_BANK_NUM * HAL_DISP_UTILITY_REG_BANK_SIZE);
                if(gpvRegBuffer == NULL)
                {
                    DISP_ERR("%s %d, Memalloc RegBuffer Fail\n", __FUNCTION__, __LINE__);
                    return 0;
                }
            }

            gstDispUtilityCmdqCtx[u32CtxId].s32CmdqId = u32CtxId;
            gstDispUtilityCmdqCtx[u32CtxId].pvRegBuffer = gpvRegBuffer;

            gstDispUtilityCmdqCtx[u32CtxId].u16RegFlipCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].u16SyncCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].u16WaitDoneCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].bEnSyncCnt = 0;

            memset(gstDispUtilityCmdqCtx[u32CtxId].pvRegBuffer, 0, HAL_DISP_UTILITY_REG_BANK_NUM * HAL_DISP_UTILITY_REG_BANK_SIZE);

            gstDispUtilityCmdqCtx[u32CtxId].u32DirectCmdqCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer = DrvDispOsMemAlloc(sizeof(HalDispUtilityCmdReg_t) * HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);

            if(gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, Cmdq:%ld, DirectCmdqBUffer Memallc Failn", __FUNCTION__, __LINE__, u32CtxId);
            }


            gstDispUtilityCmdqCtx[u32CtxId].u32CmdqCmdCnt = 0;
            gstDispUtilityCmdqCtx[u32CtxId].pvCmdqCmdBuffer = DrvDispOsMemAlloc(sizeof(HalDispUtilityCmdqCmd_t) * HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            if(gstDispUtilityCmdqCtx[u32CtxId].pvCmdqCmdBuffer == NULL)
            {
                bRet = 0;
                DISP_ERR("%s %d, Cmdq:%ld, PollCmdq Buffer Memallc Failn", __FUNCTION__, __LINE__, u32CtxId);
            }

            if( _HalDispUtilityGetInitFlag() == 0)
            {
                for(i=0; i<E_DISP_UTILITY_REG_CONFIG_NUM; i++)
                {
                    _HalDispUtilityReadBankCpyToBuffer(i);
                }
            }

            _HalDispUtilitySetInitFlagByIdx(u32CtxId, 1);
        }
    }
    else
    {
        DISP_ERR("%s %d, CtxId(%ld) is bigger than CMDQ NUM (%d)\n",
            __FUNCTION__, __LINE__, u32CtxId, HAL_DISP_UTILITY_CMDQ_NUM);
        bRet = 0;
    }

    return bRet;
}


bool HalDispUtilityDeInit(u32 u32CtxId)
{
    if( _HalDispUtilityGetInitFlagByIdx(u32CtxId) )
    {
        gstDispUtilityCmdqCtx[u32CtxId].s32CmdqId = -1;
        gstDispUtilityCmdqCtx[u32CtxId].u16RegFlipCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].u16SyncCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].u16WaitDoneCnt = 0;
        gstDispUtilityCmdqCtx[u32CtxId].bEnSyncCnt = 0;

        gstDispUtilityCmdqCtx[u32CtxId].u32CmdqCmdCnt = 0;
        DrvDispOsMemRelease(gstDispUtilityCmdqCtx[u32CtxId].pvCmdqCmdBuffer);

        gstDispUtilityCmdqCtx[u32CtxId].u32DirectCmdqCnt = 0;
        DrvDispOsMemRelease(gstDispUtilityCmdqCtx[u32CtxId].pvDirectCmdqBuffer);

        _HalDispUtilitySetInitFlagByIdx(u32CtxId, 0);
    }

    if( _HalDispUtilityGetInitFlag() == 0 )
    {
        DrvDispOsMemRelease(gpvRegBuffer);
        DrvDispOsDestroyMutex(&gstDispUtilityMutexCfg);
    }

    return 1;
}

bool HalDispUtilityGetCmdqContext(void **pvCtx, u32 u32CtxId)
{
    bool bRet = 1;
    if(u32DispUtilityCmdqInitFlag)
    {
        *pvCtx = (void *)&gstDispUtilityCmdqCtx[u32CtxId];
    }
    else
    {
        *pvCtx = NULL;
        bRet = 0;
        DISP_ERR("%s %d, No Disp Utility Init\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

void HalDispUtilitySetCmdqInf(void *pCmdqInf, u32 u32CtxId)
{
    gstDispUtilityCmdqCtx[u32CtxId].pvCmdqInf = pCmdqInf;
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

u16 HalDispUtilityR2BYTERegBuf(u32 u32Reg, void *pvCtxIn)
{
    u16 u16RetVal = 0;
    void *pvRegBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvRegBuffer = _HalDispUtilityGetRegBuffer(pstCmdqCtx);
    if (pvRegBuffer)
    {
        u16RetVal = _HalDispUtilityReadRegBuffer(u32Reg, pvRegBuffer, (u32)pstCmdqCtx->s32CmdqId);
    }
    return u16RetVal;
}

void HalDispUtilityWBYTEMSKRegBuf(u32 u32Reg, u8 u8Val, u8 u8Mask, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;
    u16 u16Val = 0, u16Mask = 0;

    u16Val  = u8Val;
    u16Mask = u8Mask;
    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pvCtxIn);
    _HalDispUtilityWrite1ByteRegBuffer(&stInq);
}

void HalDispUtilityWBYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pvCtxIn);

    _HalDispUtilityWrite1ByteRegBuffer(&stInq);

    if(stInq.stCmd.u32Addr & 0x01)
    {
        stInq.stCmd.u32Addr = stInq.stCmd.u32Addr & 0xFFFFFFFE;
        stInq.stCmd.u16Data = stInq.stCmd.u16Data << 8;
        stInq.stCmd.u16Mask = stInq.stCmd.u16Mask << 8;
    }

    HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn,
        stInq.stCmd.u32Addr, stInq.stCmd.u16Data, stInq.stCmd.u16Mask);

}

void HalDispUtilityWBYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityWBYTEMSKRegBuf(u32Reg, (u8)u16Val, (u8)u16Mask, pvCtxIn);
    WBYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityWBYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityWBYTEMSKInquireBuf(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
    else
    {
        HalDispUtilityWBYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKRegBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pvCtxIn);
    _HalDispUtilityWrite2ByteRegBuffer(&stInq);
}

void HalDispUtilityW2BYTEInqireBuf(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, 0xFFFF, pvCtxIn);

    _HalDispUtilityWrite2ByteRegBuffer(&stInq);

    HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn,
        stInq.stCmd.u32Addr, stInq.stCmd.u16Data, stInq.stCmd.u16Mask);
}

void HalDispUtilityW2BYTEDirect(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    HalDispUtilityW2BYTEMSKRegBuf(u32Reg, u16Val, 0xFFFF, pvCtxIn);
    W2BYTE(u32Reg, u16Val);
}

void HaDisplUtilityW2BYTE(u32 u32Reg, u16 u16Val, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEInqireBuf(u32Reg, u16Val, pvCtxIn);
    }
    else
    {
        HalDispUtilityW2BYTEDirect(u32Reg, u16Val, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKInquireBuf(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityInquire_t stInq;

    _HalDispUtilityFillCmdqStruct(&stInq, u32Reg, u16Val, u16Mask, pvCtxIn);

    _HalDispUtilityWrite2ByteRegBuffer(&stInq);

    HalDispUtilityW2BYTEMSKDirectCmdq(pvCtxIn,
        stInq.stCmd.u32Addr, stInq.stCmd.u16Data, stInq.stCmd.u16Mask);
}

void HalDispUtilityW2BYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    HalDispUtilityW2BYTEMSKRegBuf(u32Reg, u16Val, u16Mask, pvCtxIn);
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn)
{
    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSKInquireBuf(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
    else
    {
        HalDispUtilityW2BYTEMSKDirect(u32Reg, u16Val, u16Mask, pvCtxIn);
    }
}

void HalDispUtilityW2BYTEMSKDirectCmdq(void *pvCtxIn, u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    HalDispUtilityCmdReg_t *pstCmdReg = NULL;
    u32 *pu32TblCnt = NULL;
    void *pvTblBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
    pu32TblCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32TblCnt >= HAL_DISP_UTILITY_DIRECT_CMDQ_CNT)
        {
            DISP_ERR("%s %d, Cnt [%ld] over max [%d] \n",
                __FUNCTION__, __LINE__,
                *pu32TblCnt, HAL_DISP_UTILITY_DIRECT_CMDQ_CNT);

            return;
        }

        pstCmdReg = (HalDispUtilityCmdReg_t *)(pvTblBuffer + ((*pu32TblCnt) * sizeof(HalDispUtilityCmdReg_t)));
        pstCmdReg->u32Addr = u32Reg;
        pstCmdReg->u16Mask = u16Mask;
        pstCmdReg->u16Data = u16Val;
        (*pu32TblCnt)++;
    }
    else
    {
        W2BYTEMSK(u32Reg, u16Val, u16Mask);
    }
}

void HalDispUtilityW2BYTEMSKDirectCmdqWrite(void *pvCtxIn)
{
    u32 i = 0;
    u32 j = 0;
    HalDispUtilityCmdReg_t *pstCmdReg = 0;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    HalDispUtilityCmdqCmd_t *pstCmdqCmdCur = NULL;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    u32 *pu32DirectCmdqCnt = NULL;
    void *pvDirctCmdqBuffer = NULL;
    u32 *pu32CmdqCmdCnt = NULL;
    void *pvCmdqCmdBuffer = NULL;

    if (pstCmdqCtx)
    {
        pstCmdqInf = pstCmdqCtx->pvCmdqInf;
    }

    pvDirctCmdqBuffer = _HalDispUtilityGetDirectCmdqBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);

    pvCmdqCmdBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32CmdqCmdCnt = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (pvDirctCmdqBuffer && *pu32DirectCmdqCnt > 0)
    {
        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_WriteRegCmdqMaskMulti)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Use Cmdq_%ld \n",
                __FUNCTION__, __LINE__, pstCmdqCtx->s32CmdqId);

            if(pvCmdqCmdBuffer && *pu32CmdqCmdCnt)
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
                if(*pu32CmdqCmdCnt)
                {
                    pstCmdqCmdCur = (HalDispUtilityCmdqCmd_t *)(pvCmdqCmdBuffer + (j* sizeof(HalDispUtilityCmdqCmd_t)));

                    if(pstCmdqCmdCur->u32CmdqBufIdx == i)
                    {
                        if(pstCmdqCmdCur->u32Addr == HAL_DISP_UTILITY_CMDQ_DELAY_CMD)
                        {
                            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, DelayCmd [%ld]\n",
                                __FUNCTION__, __LINE__, pstCmdqCmdCur->u32Time);
                            HalDispUtilityAddDelayCmd(pvCtxIn, pstCmdqCmdCur->u32Time);
                        }
                        else
                        {
                            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Poll [0x%04lx%02lx] Val [0x%04x] Mask [0x%04x]\n",
                                __FUNCTION__, __LINE__,
                                (pstCmdqCmdCur->u32Addr & 0xFFFF00) >> 8,
                                (pstCmdqCmdCur->u32Addr & 0xFF) >> 1,
                                pstCmdqCmdCur->u16Data,
                                pstCmdqCmdCur->u16Mask);

                            HalDispUtilityPollWait(pstCmdqInf,
                                pstCmdqCmdCur->u32Addr,
                                pstCmdqCmdCur->u16Data,
                                pstCmdqCmdCur->u16Mask,
                                pstCmdqCmdCur->u32Time,
                                pstCmdqCmdCur->bPollEq);
                        }
                        j++;
                    }
                }

                pstCmdReg = (HalDispUtilityCmdReg_t *)(pvDirctCmdqBuffer + (i* sizeof(HalDispUtilityCmdReg_t)));
                DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Addr [0x%04lx%02lx] Val [0x%04x] Mask [0x%04x]\n",
                    __FUNCTION__, __LINE__,
                    (pstCmdReg->u32Addr & 0xFFFF00) >> 8,
                    (pstCmdReg->u32Addr & 0xFF) >> 1,
                    pstCmdReg->u16Data,
                    pstCmdReg->u16Mask);

                W2BYTEMSK(pstCmdReg->u32Addr, pstCmdReg->u16Data, pstCmdReg->u16Mask);
            }
        }

        (*pu32DirectCmdqCnt) = 0;
        (*pu32CmdqCmdCnt) = 0;
    }
}

void HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32 u32Reg, u16 u16Val, u16 u16Mask)
{
    W2BYTEMSK(u32Reg, u16Val, u16Mask);
}

void HalDispUtilityWritePollCmd(u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, bool bPollEq, void *pvCtxIn)
{
    HalDispUtilityCmdqCmd_t *pstCmdqCmd = NULL;
    u32 *pu32DirectCmdqCnt = NULL;
    u32 *pu32CmdqCmdCnt = NULL;
    void *pvTblBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);
    pu32CmdqCmdCnt = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32CmdqCmdCnt >= HAL_DISP_UTILITY_CMDQ_CMD_CNT)
        {
            DISP_ERR("%s %d, CmdqPollCmd Cnt [%ld] over max [%d] \n",
                __FUNCTION__, __LINE__,
                *pu32CmdqCmdCnt, HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            return;
        }

        pstCmdqCmd = (HalDispUtilityCmdqCmd_t *)(pvTblBuffer + ((*pu32CmdqCmdCnt) * sizeof(HalDispUtilityCmdqCmd_t)));
        pstCmdqCmd->u32CmdqBufIdx =  *pu32DirectCmdqCnt;
        pstCmdqCmd->u32Addr = u32Reg;
        pstCmdqCmd->u16Mask = u16Mask;
        pstCmdqCmd->u16Data = u16Val;
        pstCmdqCmd->u32Time = u32PollTime;
        pstCmdqCmd->bPollEq = bPollEq;
        (*pu32CmdqCmdCnt)++;
    }
    else
    {
        u32 u32CurTime = DrvDispOsGetSystemTime();
        while(1)
        {
            if(bPollEq)
            {
                if((R2BYTE(u32Reg) & u16Mask) == (u16Val & u16Mask))
                {
                    break;
                }
            }
            else
            {
                if((R2BYTE(u32Reg) & u16Mask) != (u16Val & u16Mask))
                {
                    break;
                }
            }

            if( DrvDispOsGetSystemTime()- u32CurTime > u32PollTime/1000000)
            {
                DISP_ERR("%s %d, Polling TimeOut, reg:%06lx, val:%04x, msk:%4x\n", __FUNCTION__, __LINE__, u32Reg, u16Val, u16Mask);
                break;
            }
        }
    }
}

void HalDispUtilityWriteDelayCmd(u16 u16PollingTime, void *pvCtxIn)
{
    HalDispUtilityCmdqCmd_t *pstCmdqCmd = NULL;
    u32 *pu32DirectCmdqCnt = NULL;
    u32 *pu32CmdqCmdCnt = NULL;
    void *pvTblBuffer = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pvTblBuffer = _HalDispUtilityGetCmdqCmdBuffer(pstCmdqCtx);
    pu32DirectCmdqCnt  = _HalDispUtilityGetDirectCmdqCnt(pstCmdqCtx);
    pu32CmdqCmdCnt = _HalDispUtilityGetCmdqCmdCnt(pstCmdqCtx);

    if (HalDispUtilityGetRegAccessMode() ==  E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (*pu32CmdqCmdCnt >= HAL_DISP_UTILITY_CMDQ_CMD_CNT)
        {
            DISP_ERR("%s %d, CmdqPollCmd Cnt [%ld] over max [%d] \n",
                __FUNCTION__, __LINE__,
                *pu32CmdqCmdCnt, HAL_DISP_UTILITY_CMDQ_CMD_CNT);

            return;
        }

        pstCmdqCmd = (HalDispUtilityCmdqCmd_t *)(pvTblBuffer + ((*pu32CmdqCmdCnt) * sizeof(HalDispUtilityCmdqCmd_t)));
        pstCmdqCmd->u32CmdqBufIdx =  *pu32DirectCmdqCnt;
        pstCmdqCmd->u32Addr = HAL_DISP_UTILITY_CMDQ_DELAY_CMD;
        pstCmdqCmd->u16Mask = 0xFFFF;
        pstCmdqCmd->u16Data = 0xFFFF;
        pstCmdqCmd->u32Time = u16PollingTime;
        pstCmdqCmd->bPollEq = 0;
        (*pu32CmdqCmdCnt)++;
    }
    else
    {
        DrvDispOsMsDelay(u16PollingTime);
    }
}

bool HalDispUtilityPollWait(void *pvCmdqInf, u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, bool bPollEq)

{
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = (MHAL_CMDQ_CmdqInterface_t *)pvCmdqInf;
    s32 s32CmdqRet = 0;
    u32 u32WaitCnt = 0;
    bool bRet = 1;

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, Poll [0x%04lx%02lx] Val [0x%04x] Mask [0x%04x] Time:%ld, PollEq:%d\n",
        __FUNCTION__, __LINE__,
        (u32Reg & 0xFFFF00) >> 8,
        (u32Reg & 0xFF) >> 1,
        u16Val,u16Mask, u32PollTime, bPollEq);

    if (pstCmdqInf == NULL || pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits == NULL)
    {
        while (!(HalDispUtilityR2BYTEDirect(u32Reg) & (u16Mask & u16Val)))
        {
            if (++u32WaitCnt > 200)
            {
                return 1;
            }
            DrvDispOsUsSleep(((u32PollTime/10000) ? (u32PollTime/10000) : 1)); // Wait time ns/1000
        }
    }
    else
    {
        if(u32PollTime)
        {
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdqPollRegBits_ByTime(pstCmdqInf, u32Reg, u16Val, u16Mask, bPollEq, u32PollTime);
            if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
            {
                bRet = 0;
                DISP_ERR("%s %d, CmdqPollRegBits_ByTime [%lu] Fail\n", __FUNCTION__, __LINE__, u32PollTime);
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

bool HalDispUtilityAddWaitCmd(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    MHAL_CMDQ_EventId_e     enCmdqEvent = 0;
    s32 s32CmdqRet = 0;
    bool bRet = 1;

    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if (pstCmdqCtx)
        {
            pstCmdqInf  = pstCmdqCtx->pvCmdqInf;
        }

        if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd)
        {
            enCmdqEvent =  HAL_DISP_UTILITY_CMDQ0_WAIT_DONE_EVENT(pstCmdqCtx->s32CmdqId);
            s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdqInf, enCmdqEvent);
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
            DISP_ERR("%s %d, NULL CmdqInf\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

bool HalDispUtilityAddDelayCmd(void *pvCtxIn, u16 u16PollingTime)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqInf = NULL;
    s32 s32CmdqRet = 0;
    bool bRet = 1;

    if (pstCmdqCtx)
    {
        pstCmdqInf  = pstCmdqCtx->pvCmdqInf;
    }

    if (pstCmdqInf && pstCmdqInf->MHAL_CMDQ_CmdDelay)
    {
        s32CmdqRet = pstCmdqInf->MHAL_CMDQ_CmdDelay(pstCmdqInf, 1000*u16PollingTime);
        if (s32CmdqRet != 0 /*MHAL_SUCCESS*/)
        {
            bRet = 0;
            DISP_ERR("%s %d, CmdDelay Fail\r\n", __FUNCTION__, __LINE__ );
        }
    }
    else
    {
        bRet = 0;
    }

    return bRet;
}

void HalDispUtilitySetRegAccessMode(u32 u32Mode)
{
    geDispRegAccessMode = u32Mode;
}

u32 HalDispUtilityGetRegAccessMode(void)
{
    return (u32)geDispRegAccessMode;
}

void HalDispUtilityResetCmdqCnt(void *pvCtxIn)
{
    HalDispUtilityCmdqContext_t *pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pvCtxIn;

    pstCmdqCtx->u16RegFlipCnt = 0;
    pstCmdqCtx->u16SyncCnt = 0;
    pstCmdqCtx->u16WaitDoneCnt = 0;
    pstCmdqCtx->bEnSyncCnt = 0;
}

