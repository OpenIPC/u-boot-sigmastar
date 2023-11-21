/*
* hal_disp_if.c- Sigmastar
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

#define _HAL_DISP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_disp_os.h"
#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp_op2.h"
#include "hal_disp_clk.h"
#include "hal_disp_color.h"
#include "hal_disp_mop.h"
#include "hal_disp_mop_if.h"
#include "hal_disp_reg.h"
#include "hal_disp_util.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"
#include "drv_disp_os.h"
#include "hal_disp_mace.h"
#include "hal_disp_hpq.h"
#include "hal_disp_irq.h"
#include "hal_disp_if.h"
#include "hal_disp_dma_if.h"
#include "hal_disp_picture_if.h"
#include "hal_disp_vga_timing_tbl.h"
#include "hal_disp_cvbs_timing_tbl.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalDispQueryRet_e (*pGetInfoFunc) (void *, void *);
    void (*pSetFunc) (void * , void *);
    u16  u16CfgSize;
}HalDispQueryCallBackFunc_t;


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
bool gbDispHwIfInit = 0;
HalDispQueryCallBackFunc_t gpDispCbTbl[E_HAL_DISP_QUERY_MAX];
HalDispInternalConfig_t gstInterCfg[HAL_DISP_DEVICE_MAX] =
{
    {0, 0, HAL_DISP_COLOR_CSC_ID},
    {0, 0, HAL_DISP_COLOR_CSC_ID},
};

// color & picture
s16 tVideoColorCorrectionMatrix[]=
{
   0x0400, 0x0000, 0x0000, 0x0000, 0x0400, 0x0000, 0x0000, 0x0000,
   0x0400,-0x034B, 0x0196,-0x068B, 0x03C9,-0x0439, 0x0032,-0x0004,
  -0x07EE, 0x04E7, 0x07CB,-0x04C3, 0x0404, 0x023B,-0x023E, 0x01D5,
  -0x0831, 0x0100,-0x0102, 0x0101,-0x0101, 0x0000, 0x0000, 0x0000,
};

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
void __HalDispIfSetTgenRest(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *pCmdqCtx = NULL;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, Interface:%s(%lx)\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        PARSING_HAL_INTERFACE(pstDevContain->u32Interface), pstDevContain->u32Interface);


    if(pstDevContain->u32Interface & HAL_DISP_INTF_TTL)
    {
        HalDispSetLcdAffReset(1 ,(void *)pstDispCtxCfg);

        HalDispUtilityWriteDelayCmd(10, (void *)pCmdqCtx);

        HalDispSetLcdAffReset(0 ,(void *)pstDispCtxCfg);
    }

    if(pstDevContain->u32Interface & HAL_DISP_INTF_VGA)
    {
        if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            HalDispClearDisp0SwReset((void *)pstDispCtxCfg);
        }
        HalDispSetDacFpllEn(0,(void *)pstDispCtxCfg);

        HalDispSetSwReste(1, (void *)pstDispCtxCfg);
        HalDispSetSwReste(0, (void *)pstDispCtxCfg);

        if(pstDevContain->u32Interface == (HAL_DISP_INTF_VGA | HAL_DISP_INTF_HDMI))
        {
            HalDispSetDacFpllEn(1,(void *)pstDispCtxCfg);
        }
    }

    if(pstDevContain->u32Interface & HAL_DISP_INTF_CVBS)
    {
        //HalDispSetDacAffReset(1 ,(void *)pstDispCtxCfg);
        //HalDispSetDacAffReset(0 ,(void *)pstDispCtxCfg);
    }
}


void __HalDispIfSetRegFlipPreAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    u16 u16CmdqInProcess;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    u16CmdqInProcess = HalDispUtilityR2BYTEMaskDirect(
                        REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(pstCmdqCtx->s32CmdqId),
                        REG_CMDQ_IN_PROCESS_MSK);

    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        // Add Wait Event if cmdq is idle
        if(u16CmdqInProcess == 0  &&
           !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx]->u32Interface & HAL_DISP_INTF_MCU) &&
           !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx]->u32Interface & HAL_DISP_INTF_MCU_NOFLM))
        {
            HalDispUtilityAddWaitCmd((void *)pstCmdqCtx);

            HalDispUtilityW2BYTEMSK(
                    HAL_DISP_UTILITY_DISP_TO_CMDQ_CLEAR(pstCmdqCtx->s32CmdqId),
                    0xFFFF,
                    0xFFFF,
                    (void *)pstCmdqCtx);
        }

        HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16RegFlipCnt, 1);
    }
}

void __HalDispIfSetRegFlipPostAct(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    HalDispHwContain_t *pstHalHwContain = NULL;
    u8 i;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;

    HalDispMopIfSetDdbfWr((void *)pstDispCtxCfg);

    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSK(
            REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(pstCmdqCtx->s32CmdqId),
            REG_CMDQ_IN_PROCESS_ON,
            REG_CMDQ_IN_PROCESS_MSK,
            (void *)pstCmdqCtx);

        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }

    if(pstHalHwContain->bTimingUpdate[pstDevContain->u32DevId] &&
       (pstDevContain->u32Interface == (HAL_DISP_INTF_VGA | HAL_DISP_INTF_HDMI)))
    {
        HalDispSetDacAffReset(1, (void *)pstDispCtxCfg);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);


        if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
        {
            for(i=0; i<4; i++)
            {
                HalDispUtilityAddWaitCmd((void *)pstCmdqCtx);

                HalDispUtilityW2BYTEMSK(
                        HAL_DISP_UTILITY_DISP_TO_CMDQ_CLEAR(pstCmdqCtx->s32CmdqId),
                        0xFFFF,
                        0xFFFF,
                        (void *)pstCmdqCtx);
                HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
            }
        }
        else
        {
            DrvDispOsMsDelay(100);
        }
        HalDispSetDacAffReset(0, (void *)pstDispCtxCfg);
        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);

        pstHalHwContain->bTimingUpdate[pstDevContain->u32DevId] = 0;
    }
}

void __HalDispIfSetWaitDonePreAct(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    u16 u16CmdqInProcess;

    u16CmdqInProcess = HalDispUtilityR2BYTEMaskDirect(
                        REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(pstCmdqCtx->s32CmdqId),
                        REG_CMDQ_IN_PROCESS_MSK);

    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        if( u16CmdqInProcess == 0 && pstCmdqCtx->bEnSyncCnt == 0)
        {
            pstCmdqCtx->bEnSyncCnt = 1;
        }

        HAL_DISP_UTILITY_CNT_ADD(pstCmdqCtx->u16WaitDoneCnt, 1);
    }
}

void __HalDispIfSetWaitDonePostAct(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
    {
        HalDispUtilityW2BYTEMSK(
           HAL_DISP_UTILITY_DISP_TO_CMDQ_CLEAR(pstCmdqCtx->s32CmdqId),
           0xFFFF,
           0xFFFF,
           (void *)pstCmdqCtx);

        HalDispUtilityW2BYTEMSK(
           REG_HAL_DISP_UTILITY_CMDQ_WAIT_CNT(pstCmdqCtx->s32CmdqId),
           (pstCmdqCtx->u16WaitDoneCnt << REG_CMDQ_WAIT_CNT_SHIFT),
           REG_CMDQ_WAIT_CNT_MSK,
           (void *)pstCmdqCtx);

        HalDispUtilityW2BYTEMSK(
          REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(pstCmdqCtx->s32CmdqId),
          REG_CMDQ_IN_PROCESS_OFF,
          REG_CMDQ_IN_PROCESS_MSK,
          (void *)pstCmdqCtx);

        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
    }
}

void __HalDispIfSetWaitDonePostActForMcuFrameDone(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    static u8 u8FrameDonecount=0;

    u8FrameDonecount++;
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8FrameDonecount<<12,
                                      0xF000);

    // Clear LCD frame done flag
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_0D_L,
                                      0x0001,
                                      0x0001);
    // Hold DISP top reset
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      HAL_DISP_UTILITY_DISP_TOP_RST(pstCmdqCtx->s32CmdqId),
                                      0x0100,
                                      0x0100);
#if 0
    // Disable DSI gate enable
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      HAL_DISP_UTILITY_DISP_DSI_GATE_EN(pstCmdqCtx->s32CmdqId),
                                      0x0000,
                                      0x0010);
    // afifo reset
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_30_L,
                                      0x000E,
                                      0x000E);
    // afifo sw reset
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_00_L,
                                      0x0001,
                                      0x0001);
    // afifo sw reset done
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_00_L,
                                      0x0000,
                                      0x0001);
    // afifo reset done
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_30_L,
                                      0x0000,
                                      0x000E);
    // DSI gate enable
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      HAL_DISP_UTILITY_DISP_DSI_GATE_EN(pstCmdqCtx->s32CmdqId),
                                      0x0010,
                                      0x0010);
#endif

    // Poll GPIO
    // Disable LCD frame done IRQ mask and enable FLM IRQ mask
#if (CMDQ_POLL_EQ_MODE == 0)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_08_L,
                                      0xfdff,
                                      0xffff);
#endif
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_HAL_DISP_UTILIYT_CMDQ_IN_PROCESS(pstCmdqCtx->s32CmdqId),
                                      REG_CMDQ_IN_PROCESS_OFF,
                                      REG_CMDQ_IN_PROCESS_MSK);
}

void __HalDispIfSetWaitDonePostActForMcuFlm(HalDispUtilityCmdqContext_t *pstCmdqCtx)
{
    static u8 u8flmcount=0;

    // LCD status clear
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_0D_L,
                                      0x0200,
                                      0x0200);

    // DISP top reset release
    u8flmcount++;
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      HAL_DISP_UTILITY_DISP_TOP_RST(pstCmdqCtx->s32CmdqId),
                                      0x0000,
                                      0x1100);
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_00_L,
                                      0x0001,
                                      0x0001);
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_MISC_00_L,
                                      0x0000,
                                      0x0001);
#if 0
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      0x103ea8,
                                      0x0,
                                      0x2);
#endif
//        HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);
//        HalDispUtilityPollWait((void *)pstCmdqCtx->pvCmdqInf, 0x103ea8,0x0, 0x2, 0, 1);

#if (CMDQ_POLL_EQ_MODE == 0)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_08_L,
                                      0xfffe,
                                      0xffff);
#endif

    // Dummy register for delay used (TBD)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);

    // Dummy register for delay used (TBD)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);

    // Dummy register for delay used (TBD)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);

    // Dummy register for delay used (TBD)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);

    // Dummy register for delay used (TBD)
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_DISP_TOP_0_30_L,
                                      u8flmcount<<8,
                                      0x0F00);

    // FW refresh
    HalDispUtilityW2BYTEMSKDirectCmdq((void *)pstCmdqCtx,
                                      REG_LCD_00_L,
                                      0x0042,
                                      0x0042);
}

bool __HalDispIfUpdateMaceConfig(HalDispPqConfig_t *pstPqCfg, void *pCtx)
{
    bool bRet = 1;
    HalDispMaceHwContext_t *pstHwMaceCfg = NULL;
    HalDispMaceHwContext_t *pstInMaceCfg = NULL;

    if(pstPqCfg->u32DataSize == sizeof(HalDispMaceHwContext_t))
    {
        if(pstPqCfg->u32PqFlags & 0x800001FF)
        {
            HalDispMaceGetHwCtx(&pstHwMaceCfg, pCtx);
            pstInMaceCfg = (HalDispMaceHwContext_t *)pstPqCfg->pData;
            if(pstHwMaceCfg && pstInMaceCfg)
            {
                memcpy(pstHwMaceCfg, pstInMaceCfg, sizeof(HalDispMaceHwContext_t));
            }
            else
            {
                bRet = 0;
                DISP_ERR("%s %d, NULL Point, pstHwMaceCfg:%p, pstInMaceCfg:%p \n",
                    __FUNCTION__, __LINE__,
                    pstHwMaceCfg, pstInMaceCfg);
            }
        }
        else
        {
            bRet = 0;
            DISP_ERR("%s %d, PQ Type is not Correctd: %lx\n",
                __FUNCTION__, __LINE__, pstPqCfg->u32PqFlags);
        }
    }
    else
    {
        bRet = 0;
        DISP_ERR("%s %d, Pq Size is not Correct, InSize:%ld != %d\n",
            __FUNCTION__, __LINE__,
            pstPqCfg->u32DataSize,
            sizeof(HalDispMaceHwContext_t));
    }
    return bRet;
}

void __HalDispIfSetTgenConfig(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceTimingConfig_t *pstDeviceTimingCfg = NULL;
    u16 u16HsyncSt, u16VsyncSt;
    u16 u16HdeSt, u16VdeSt;

    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstDeviceTimingCfg = &pstDevContain->stDevTimingCfg;

    u16HdeSt = pstDeviceTimingCfg->u16Hstart;
    u16VdeSt = pstDeviceTimingCfg->u16Vstart;

    u16HsyncSt = (u16HdeSt) ?
        pstDeviceTimingCfg->u16Hstart - pstDeviceTimingCfg->u16HsyncWidth - pstDeviceTimingCfg->u16HsyncBackPorch :
        pstDeviceTimingCfg->u16Htotal - pstDeviceTimingCfg->u16HsyncWidth - pstDeviceTimingCfg->u16HsyncBackPorch;

    u16VsyncSt = (u16VdeSt) ?
        pstDeviceTimingCfg->u16Vstart - pstDeviceTimingCfg->u16VsyncWidth - pstDeviceTimingCfg->u16VsyncBackPorch :
        pstDeviceTimingCfg->u16Vtotal - pstDeviceTimingCfg->u16VsyncWidth - pstDeviceTimingCfg->u16VsyncBackPorch;

    DISP_DBG(DISP_DBG_LEVEL_HAL,"%s %d, Hsync(%d %d), Vsync(%d %d) SyncStart(%d %d), DeStart(%d, %d) Size(%d %d), Total(%d %d)\n",
        __FUNCTION__, __LINE__,
        pstDeviceTimingCfg->u16HsyncWidth, pstDeviceTimingCfg->u16HsyncBackPorch,
        pstDeviceTimingCfg->u16VsyncWidth, pstDeviceTimingCfg->u16VsyncBackPorch,
        u16HsyncSt, u16VsyncSt,
        u16HdeSt, u16VdeSt,
        pstDeviceTimingCfg->u16Hactive, pstDeviceTimingCfg->u16Vactive,
        pstDeviceTimingCfg->u16Htotal, pstDeviceTimingCfg->u16Vtotal);

    if(pstDevContain->u32Interface & HAL_DISP_INTF_MIPIDSI)
    {

        HalDispSetTgenVsyncSt(u16VsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVsyncEnd(u16VsyncSt + pstDeviceTimingCfg->u16VsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeSt(u16VdeSt + 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive, (void *)pstDispCtxCfg);

        HalDispSetTgenVdeSt(u16VdeSt + 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive, (void *)pstDispCtxCfg);
        HalDispSetTgenVtt(pstDeviceTimingCfg->u16Vtotal, (void *)pstDispCtxCfg);

        HalDispSetTgenHsyncSt(u16HsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHsyncEnd(u16HsyncSt + pstDeviceTimingCfg->u16HsyncWidth - 1, (void *)pstDispCtxCfg);

        u16HdeSt = u16HdeSt > 58 ? u16HdeSt - 58 : u16HdeSt;

        HalDispSetTgenHfdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHfdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHtt(pstDeviceTimingCfg->u16Htotal - 1, (void *)pstDispCtxCfg);
    }
    else
    {
        // Vertical
        HalDispSetTgenVsyncSt(u16VsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVsyncEnd(u16VsyncSt + pstDeviceTimingCfg->u16VsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeSt(u16VdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVfdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVdeSt(u16VdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenVdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenVtt(pstDeviceTimingCfg->u16Vtotal - 1, (void *)pstDispCtxCfg);

        // horizontal
        HalDispSetTgenHsyncSt(u16HsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHsyncEnd(u16HsyncSt + pstDeviceTimingCfg->u16HsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHfdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHfdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenHdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenHtt(pstDeviceTimingCfg->u16Htotal - 1, (void *)pstDispCtxCfg);
    }

    if(pstDevContain->u32Interface & HAL_DISP_INTF_VGA)
    {
        HalDispSetTgenDacHsyncSt(u16HsyncSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHsyncEnd(u16HsyncSt + pstDeviceTimingCfg->u16HsyncWidth - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHdeSt(u16HdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacHdeEnd(u16HdeSt + pstDeviceTimingCfg->u16Hactive - 1, (void *)pstDispCtxCfg);
        HalDispSetTgenDacVdeSt(u16VdeSt, (void *)pstDispCtxCfg);
        HalDispSetTgenDacVdeEnd(u16VdeSt + pstDeviceTimingCfg->u16Vactive - 1, (void *)pstDispCtxCfg);
    }
}

void __HalDispIfSetInitWithoutCmdq(DrvDispCtxConfig_t *pstDispCtx)
{
    u32 u32OrigAccessMode = HalDispUtilityGetRegAccessMode();

    HalDispUtilitySetRegAccessMode(E_DISP_UTILITY_REG_ACCESS_CPU);
    HalDispSetCmdqIntMask(0xFFFB, (void *)pstDispCtx); //
    HalDispSetMaceSrcSel(0x0001, (void *)pstDispCtx);  // mace src from mop
    HalDispUtilitySetRegAccessMode(u32OrigAccessMode);
}

HalDispTimeZoneType_e __HalDispIfGetTimeZoneType(DrvDispCtxConfig_t *pstDispCtxCfg)
{
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;
    HalDispIrqTimeZoneStatus_t stTimeZoneStatus;
    HalDispTimeZoneType_e enTimeZoneType;
    HalDispDeviceTimingConfig_t *pstTimeCfg;
    u64 u64TimeDiff, u64PeriodTime;
    HalDispIrqIoctlConfig_t stIrqIoctlCfg;

    stIrqIoctlCfg.enIoctlType = E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS;
    stIrqIoctlCfg.enIrqType = E_HAL_DISP_IRQ_TYPE_TIMEZONE;
    stIrqIoctlCfg.pDispCtx = (void *)pstDispCtxCfg;
    stIrqIoctlCfg.pParam = (void *)&stTimeZoneStatus;
    HalDispIrqIoCtl(&stIrqIoctlCfg);

    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstTimeCfg = &pstDeviceContain->stDevTimingCfg;

    if(stTimeZoneStatus.u32IrqFlags == E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE)
    {
        u64TimeDiff = DrvDispOsGetSystemTimeStamp() - stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE];
        u64PeriodTime = pstTimeCfg->u32VBackPorchPeriod;
        enTimeZoneType = E_HAL_DISP_TIMEZONE_BACKPORCH;
    }
    else if(stTimeZoneStatus.u32IrqFlags == E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE ||
            stTimeZoneStatus.u32IrqFlags == (E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE | E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE) )
    {
        u64TimeDiff = DrvDispOsGetSystemTimeStamp() - stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE];
        u64PeriodTime = pstTimeCfg->u32VActivePeriod;
        if( u64TimeDiff > u64PeriodTime)
        {
            enTimeZoneType = E_HAL_DISP_TIMEZONE_FRONTPORCH;
        }
        else
        {
            enTimeZoneType = E_HAL_DISP_TIMEZONE_ACTIVE;
        }
    }
    else if(stTimeZoneStatus.u32IrqFlags == E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE)
    {
        u64TimeDiff = DrvDispOsGetSystemTimeStamp() - stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE];
        u64PeriodTime = pstTimeCfg->u32VFrontPorchPeriod;
        enTimeZoneType = E_HAL_DISP_TIMEZONE_FRONTPORCH;
    }
    else
    {
        u64TimeDiff = 0;
        u64PeriodTime = 0;
        enTimeZoneType = E_HAL_DISP_TIMEZONE_NONE;
    }

    DISP_DBG(DISP_DBG_LEVEL_IRQ_TIMEZONE,"%s %d, IrqFlag=%lx Zone:%-12s T(%5lld, %5lld), (%10lld %10lld %10lld)\n",
        __FUNCTION__, __LINE__,
        stTimeZoneStatus.u32IrqFlags,
        PARSING_HAL_TIMEZONE(enTimeZoneType),
        u64PeriodTime, u64TimeDiff,
        stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE],
        stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE],
        stTimeZoneStatus.u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE]);

    return enTimeZoneType;
}

u16 __HalDispIfGetTblId(HalDispDeviceTiming_e eTimeType)
{
    u16 u16TblId;

    switch(eTimeType)
    {
        case E_HAL_DISP_OUTPUT_1080P24:
            u16TblId = HAL_DISP_VGA_ID_1080_24P;
            break;
        case E_HAL_DISP_OUTPUT_1080P25:
            u16TblId = HAL_DISP_VGA_ID_1080_25P;
            break;
        case E_HAL_DISP_OUTPUT_1080P30:
            u16TblId = HAL_DISP_VGA_ID_1080_30P;
            break;
        case E_HAL_DISP_OUTPUT_720P50:
            u16TblId = HAL_DISP_VGA_ID_720_50P;
            break;
        case E_HAL_DISP_OUTPUT_720P60:
            u16TblId = HAL_DISP_VGA_ID_720_60P;
            break;
        case E_HAL_DISP_OUTPUT_1080P50:
            u16TblId = HAL_DISP_VGA_ID_1080_50P;
            break;
        case E_HAL_DISP_OUTPUT_1080P60:
            u16TblId = HAL_DISP_VGA_ID_1080_60P;
            break;
        case E_HAL_DISP_OUTPUT_576P50:
            u16TblId = HAL_DISP_VGA_ID_576_50P;
            break;
        case E_HAL_DISP_OUTPUT_480P60:
            u16TblId = HAL_DISP_VGA_ID_480_60P;
            break;
        case E_HAL_DISP_OUTPUT_1024x768_60:
            u16TblId = HAL_DISP_VGA_ID_1024x768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280x1024_60:
            u16TblId = HAL_DISP_VGA_ID_1280x1024_60P;
            break;
        case E_HAL_DISP_OUTPUT_1366x768_60:
            u16TblId = HAL_DISP_VGA_ID_1366x768_60P;
            break;
        case E_HAL_DISP_OUTPUT_1440x900_60:
            u16TblId = HAL_DISP_VGA_ID_1440x900_60P;
            break;
        case E_HAL_DISP_OUTPUT_1280x800_60:
            u16TblId = HAL_DISP_VGA_ID_1280x800_60P;
            break;
        case E_HAL_DISP_OUTPUT_1680x1050_60:
            u16TblId = HAL_DISP_VGA_ID_1680x1050_60P;
            break;
        case E_HAL_DISP_OUTPUT_1600x1200_60:
            u16TblId = HAL_DISP_VGA_ID_1600x1200_60P;
            break;
        case E_HAL_DISP_OUTPUT_2560x1440_30:
            u16TblId = HAL_DISP_VGA_ID_2560x1440_30P;
            break;
        case E_HAL_DISP_OUTPUT_3840x2160_30:
            u16TblId = HAL_DISP_VGA_ID_3840x2160_30P;
            break;
        case E_HAL_DISP_OUTPUT_PAL:
            u16TblId = HAL_DISP_CVBS_ID_PAL;
            break;
        case E_HAL_DISP_OUTPUT_NTSC:
            u16TblId = HAL_DISP_CVBS_ID_NTSC;
            break;
        case E_HAL_DISP_OUTPUT_960H_PAL:
        case E_HAL_DISP_OUTPUT_960H_NTSC:
        case E_HAL_DISP_OUTPUT_1080I50:
        case E_HAL_DISP_OUTPUT_1080I60:
        case E_HAL_DISP_OUTPUT_640x480_60:
        case E_HAL_DISP_OUTPUT_800x600_60:
        case E_HAL_DISP_OUTPUT_1920x1200_60:
        case E_HAL_DISP_OUTPUT_2560x1600_60:
        case E_HAL_DISP_OUTPUT_3840x2160_60:
        case E_HAL_DISP_OUTPUT_USER:
        case E_HAL_DISP_OUTPUT_MAX:
        default:
            u16TblId = HAL_DISP_VGA_ID_NUM;
            break;
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, TblId:%d\n", __FUNCTION__, __LINE__, u16TblId);
    return u16TblId;
}

void __HalDispIfDumpIdacTbl(void *pCtx, u16 u16TblId)
{
    HAL_DISP_VGA_INFO *pstTblInfo = NULL;

    if(u16TblId == HAL_DISP_VGA_ID_NUM)
    {
        DISP_ERR("%s %d, TblId (%d) is not correct\n", __FUNCTION__, __LINE__, u16TblId);
    }
    pstTblInfo = &stHAL_DISP_VGA_TIMING_TBL[HAL_DISP_VGA_COMMON_TAB_IDAC_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, 1, 0);

    pstTblInfo = &stHAL_DISP_VGA_TIMING_TBL[HAL_DISP_VGA_TAB_IDAC_ATOP];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, HAL_DISP_VGA_ID_NUM, u16TblId);
}

void __HalDispIfDumpSynthSetTbl(void *pCtx, u16 u16TblId)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HAL_DISP_VGA_INFO *pstTblInfo = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];


    if(u16TblId == HAL_DISP_VGA_ID_NUM)
    {
        DISP_ERR("%s %d, TblId (%d) is not correct\n", __FUNCTION__, __LINE__, u16TblId);
    }

    if(pstDevContain->u32Interface == (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA))
    {
        HalDispSetHdmiSynthSet(0, pCtx);
    }

    pstTblInfo =  &stHAL_DISP_VGA_TIMING_TBL[HAL_DISP_VGA_TAB_SYNTH_SET_HDMITX];
    HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, HAL_DISP_VGA_ID_NUM, u16TblId);

    if(pstDevContain->u32Interface & HAL_DISP_INTF_VGA)
    {
        pstTblInfo =  &stHAL_DISP_VGA_TIMING_TBL[HAL_DISP_VGA_TAB_SYNTH_SET_VGA];
        HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, HAL_DISP_VGA_ID_NUM, u16TblId);
    }
}

void __HalDispIfDumpCvbsTbl(void *pCtx, u16 u16TblId)
{
    HAL_DISP_CVBS_INFO *pstTblInfo = NULL;
    u8 i;

    if(u16TblId == HAL_DISP_CVBS_ID_NUM)
    {
        DISP_ERR("%s %d, TblId (%d) is not correct\n", __FUNCTION__, __LINE__, u16TblId);
    }

    for(i=0; i<HAL_DISP_CVBS_TAB_NUM; i++)
    {
        pstTblInfo = &stHAL_DISP_CVBS_TIMING_TBL[i];

        if(pstTblInfo->enIPType == HAL_DISP_CVBS_IP_COMMON)
        {
            HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, 1, 0);
        }
        else
        {
            HalDispDumpRegTab(pCtx, pstTblInfo->pData, pstTblInfo->u16RegNum, HAL_DISP_CVBS_ID_NUM, u16TblId);
        }
    }
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *pvCmdqCtx = NULL;
    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];

    if(HalDispUtilityInit(pstDevContain->u32DevId) == FALSE)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, CtxId:%ld, DevId:%ld, Utility Init Fail\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld\n",
            __FUNCTION__, __LINE__, pstDispCtxCfg->u32Idx, pstDevContain->u32DevId);

        if(HalDispUtilityGetCmdqContext(&pvCmdqCtx, pstDevContain->u32DevId) == FALSE)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            if(pvCmdqCtx)
            {
                pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId] = pvCmdqCtx;

                pstDevContain->stVgaParam.stCsc.u32Contrast = 50;
                pstDevContain->stVgaParam.stCsc.u32Hue = 50;
                pstDevContain->stVgaParam.stCsc.u32Saturation = 50;
                pstDevContain->stVgaParam.stCsc.u32Luma = 50;

                pstDevContain->stHdmiParam.stCsc.u32Contrast = 50;
                pstDevContain->stHdmiParam.stCsc.u32Hue = 50;
                pstDevContain->stHdmiParam.stCsc.u32Saturation = 50;
                pstDevContain->stHdmiParam.stCsc.u32Luma = 50;

                pstDevContain->stLcdParam.stCsc.u32Contrast = 50;
                pstDevContain->stLcdParam.stCsc.u32Hue = 50;
                pstDevContain->stLcdParam.stCsc.u32Saturation = 50;
                pstDevContain->stLcdParam.stCsc.u32Luma = 50;

                pstDevContain->u32BgColor = 0;
                HalDispClkInitGpCtrlCfg(pCtx);
            }
            else
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, Get CmdqContext Fail\n", __FUNCTION__, __LINE__);
            }
        }
    }

    return enRet;
}

void _HalDispIfSetDeviceInit(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    HalDispPictureConfig_t stPictureCfg;

    pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];
    pstCmdqCtx = pstDispCtx->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];


    // Init Setting
    __HalDispIfSetInitWithoutCmdq(pstDispCtx);

    // SwRst=0
    HalDispSetSwReste(0, (void *)pstDispCtx);
    HalDispUtilityW2BYTEMSKDirectCmdqWrite((void *)pstCmdqCtx);

    HalDispSetFrameColor(0x80, 0x00, 0x80, pCtx);
    HalDispSetDispWinColor(0x80, 0x00, 0x80, pCtx);

    HalDispSetDacRgbHpdInit();

    HalDispColorInitVar(pCtx);
    HalDispColorSetColorCorrectMatrix(0, tVideoColorCorrectionMatrix, pCtx);
    HalDispColorSetColorCorrectMatrix(1, tVideoColorCorrectionMatrix, pCtx);
    HalDispColorSeletYuvToRgbMatrix(0, E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV, NULL, pCtx);
    HalDispColorSeletYuvToRgbMatrix(1, E_DISP_COLOR_YUV_2_RGB_MATRIX_HDTV, NULL, pCtx);

    HalDispPictureIfTransNonLinear(pCtx, &pstDevContain->stHdmiParam.stCsc, NULL, &stPictureCfg);

    HalDispColorAdjustHCS(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        stPictureCfg.u16Hue,
        stPictureCfg.u16Saturation,
        stPictureCfg.u16Contrast,
        pCtx);

    HalDispColorAdjustBrightness(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        stPictureCfg.u16BrightnessR,
        stPictureCfg.u16BrightnessG,
        stPictureCfg.u16BrightnessB,
        pCtx);
}


HalDispQueryRet_e _HalDispIfGetInfoDeviceDeInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];

    HalDispClkDeInitGpCtrlCfg(pCtx);

    if(HalDispUtilityDeInit(pstDevContain->u32DevId) == FALSE)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, CtxId:%ld, DevId:%ld, Utility DeInit Fail\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId);
    }
    else
    {
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId);

        pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId] = NULL;
    }

    return enRet;
}

void _HalDispIfSetDeviceDeInit(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEn;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pbEn = (bool *)pCfg;

    pstDevContain->bEnable = *pbEn;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, Enable:%x\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        pstDevContain->bEnable);

    return enRet;
}

void _HalDispIfSetDeviceEnable(void *pCtx, void *pCfg)
{
    bool bEn;
    bEn = *(bool *)pCfg;

    HalDispSetDispWinColorForce(bEn ? 0 : 1, pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceAttach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfgDst = NULL;
    DrvDispCtxConfig_t *pstDispCtxCfgSrc = NULL;

    pstDispCtxCfgSrc = (DrvDispCtxConfig_t *)pCtx;
    pstDispCtxCfgDst = (DrvDispCtxConfig_t *)pCfg;

    if(pstDispCtxCfgSrc && pstDispCtxCfgDst)
    {
        pstDispCtxCfgDst->pstCtxContain->pstDevContain[pstDispCtxCfgDst->u32Idx]->pstDevAttachSrc =
                (void *)pstDispCtxCfgSrc->pstCtxContain->pstDevContain[pstDispCtxCfgSrc->u32Idx];

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, SRC(CtxId:%ld, DevId:%ld), DST(CtxId:%ld, DevId:%ld)\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfgSrc->u32Idx, pstDispCtxCfgSrc->pstCtxContain->pstDevContain[pstDispCtxCfgSrc->u32Idx]->u32DevId,
            pstDispCtxCfgDst->u32Idx, pstDispCtxCfgDst->pstCtxContain->pstDevContain[pstDispCtxCfgDst->u32Idx]->u32DevId);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, SrcCtx:%p, DstCtx:%p\n",
            __FUNCTION__, __LINE__, pstDispCtxCfgSrc, pstDispCtxCfgDst);
    }

    return enRet;
}

void _HalDispIfSetDeviceAttach(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceDetach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetDeviceDetach(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceBackGroundColor(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    u32 *pu32BgColor;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pu32BgColor = (u32 *)pCfg;

    pstDevContain->u32BgColor = *pu32BgColor;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, BgColor:%lx\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        pstDevContain->u32BgColor);

    return enRet;
}

void _HalDispIfSetDeviceBackGroundColor(void *pCtx, void *pCfg)
{
    u32 *pu32BgColor;
    u8 u8R, u8G, u8B;

    pu32BgColor = (u32 *)pCfg;

    u8R = (*pu32BgColor & 0x000000FF);
    u8G = (*pu32BgColor & 0x0000FF00) >> (8);
    u8B = (*pu32BgColor & 0x00FF0000) >> (16);
    HalDispSetFrameColor(u8R, u8G, u8B, pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceInterface(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    u32 *pu32Interface;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pu32Interface = (u32 *)pCfg;

    if( *pu32Interface & (HAL_DISP_INTF_YPBPR | HAL_DISP_INTF_LCD))
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, CtxId:%ld, DevId:%ld, In Intf:%s(%lx) Not Supported\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
            PARSING_HAL_INTERFACE(*pu32Interface), *pu32Interface);
    }
    else
    {
        u8 i, u8Cnt;

        u8Cnt = 0;
        for(i=0; i<32; i++)
        {
            u8Cnt = (*pu32Interface & (1<<i)) ? u8Cnt+1 : u8Cnt;
        }

        if(u8Cnt > 1 && (*pu32Interface != (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA)))
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CtxId:%ld, DevId:%ld, In Intf:%s(%lx) Not Supported\n",
                __FUNCTION__, __LINE__,
                pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
                PARSING_HAL_INTERFACE(*pu32Interface), *pu32Interface);
        }
        else
        {
            pstDevContain->u32Interface = *pu32Interface;
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, In Intf:%s(%lx), Contain Intf:%s(%lx)\n",
                __FUNCTION__, __LINE__,
                pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
                PARSING_HAL_INTERFACE(*pu32Interface), *pu32Interface,
                PARSING_HAL_INTERFACE(pstDevContain->u32Interface), pstDevContain->u32Interface);
        }
    }

    return enRet;
}

void _HalDispIfSetDeviceInterface(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispClkGpCtrlConfig_t *pstClkGpCtrlCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstClkGpCtrlCfg = (HalDispClkGpCtrlConfig_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvClkGpCtrl[pstDevContain->u32DevId];

    HalDispSetLcdFpllRefSel(0x03, pCtx);

    if(pstDevContain->u32Interface == HAL_DISP_INTF_TTL    ||
       pstDevContain->u32Interface == HAL_DISP_INTF_BT656  ||
       pstDevContain->u32Interface == HAL_DISP_INTF_BT601  ||
       pstDevContain->u32Interface == HAL_DISP_INTF_BT1120 ||
       pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
    {
        if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            if (pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
            {
                HalDispSetLcdFpllDly(0xB4, pCtx);
            }
            else
            {
                HalDispSetLcdFpllDly(0xB8, pCtx);
            }
            HalDispSetLcdSrcMux(1, pCtx);
        }
        else
        {
            if (pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
            {
                HalDispSetLcdFpllDly(0x33, pCtx);
            }
            else
            {
                HalDispSetLcdFpllDly(0x60, pCtx);
            }
            HalDispSetLcdSrcMux(0, pCtx);
        }

        if(pstDevContain->u32Interface == HAL_DISP_INTF_BT656 ||
           pstDevContain->u32Interface == HAL_DISP_INTF_BT601)
        {
            HalDispSetHdmi2OdClkRate(1, pCtx);
        }
        else if(pstDevContain->u32Interface == HAL_DISP_INTF_SRGB)
        {
            HalDispSetHdmi2OdClkRate(2, pCtx);
        }

        pstClkGpCtrlCfg->bEn = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_LCD;
    }
    else if((pstDevContain->u32Interface & (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA)))
    {
        if(pstDevContain->u32Interface & HAL_DISP_INTF_HDMI)
        {
            if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
            {
                HalDispSetHdmiSrcMux(1, pCtx);
            }
            else
            {
                HalDispSetHdmiSrcMux(0, pCtx);
            }
        }

        if(pstDevContain->u32Interface & HAL_DISP_INTF_VGA)
        {
            if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
            {
                HalDispSetDacSrcMux(1, pCtx);
                HalDispSetDacFpllDly(0xB8, pCtx);
            }
            else
            {
                HalDispSetDacSrcMux(0, pCtx);
                HalDispSetDacFpllDly(0x50, pCtx);
            }

            HalDispSetDacFpllRefSel(0x03, pCtx);
            HalDispSetDacExtFrmRstEn(0x01, pCtx);
            HalDispSetDacSynthSetSel(
                (pstDevContain->u32Interface == HAL_DISP_INTF_VGA) ? 0x01 : 0x00,
                pCtx);

            HalDispSetVgaHVsyncPad(pCtx);
            HalDispSetDacFpllGain(0x99, pCtx);
            HalDispSetDacFpllThd(0x08, pCtx);
            //HalDispSetDacFpllAbsLimit(0x000F0000, pCtx);
        }

        pstClkGpCtrlCfg->bEn = 1;
        pstClkGpCtrlCfg->eType = (pstDevContain->u32Interface == HAL_DISP_INTF_VGA)  ? E_HAL_DISP_CLK_GP_CTRL_DAC :
                                 (pstDevContain->u32Interface == HAL_DISP_INTF_HDMI) ? E_HAL_DISP_CLK_GP_CTRL_HDMI:
                                                                                       E_HAL_DISP_CLK_GP_CTRL_HDMI_DAC;
    }
    else if(pstDevContain->u32Interface == HAL_DISP_INTF_CVBS)
    {
        if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            HalDispSetCvbsSrcMux(1, pCtx);
            //HalDispSetDacFpllDly(0xB8, pCtx);
        }
        else
        {
            HalDispSetCvbsSrcMux(0, pCtx);
        }

        pstClkGpCtrlCfg->bEn = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_CVBS;
    }
    else if(pstDevContain->u32Interface == HAL_DISP_INTF_MIPIDSI)
    {
        if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            HalDispSetLcdFpllDly(0xB8, pCtx);
            HalDispSetDsiSrcMux(1, pCtx);
        }
        else
        {
            HalDispSetLcdFpllDly(0x60, pCtx);
            HalDispSetDsiSrcMux(0, pCtx);
        }

        HalDispSetDsiClkGateEn(1, pCtx);
        HalDispSetTgenExtHsEn(1, pCtx);
        HalDispSetDsiAff(0x6140, pCtx);

        pstClkGpCtrlCfg->bEn = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_MIPIDSI;
    }
    else if(pstDevContain->u32Interface == HAL_DISP_INTF_MCU ||
            pstDevContain->u32Interface == HAL_DISP_INTF_MCU_NOFLM)
    {
        HalDispMcuLcdCmdqInit(pstDevContain->u32Interface);
        HalDispKeepMcuReset(1,(void *)pstDispCtxCfg);
        if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_1)
        {
            HalDispSetCmdqMux(1,pCtx);
			HalDispSetLcdSrcMux(1, pCtx);
        }
        else
        {
            HalDispSetCmdqMux(1,pCtx);
			HalDispSetLcdSrcMux(0, pCtx);
        }
        pstClkGpCtrlCfg->bEn = 1;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_LCD;
    }
    else
    {
        pstClkGpCtrlCfg->bEn = 0;
        pstClkGpCtrlCfg->eType = E_HAL_DISP_CLK_GP_CTRL_NONE;
    }
    HalDispClkSetGpCtrlCfg(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceOutputTiming(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispDeviceTimingInfo_t *pstDeviceTimingCfg;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstDeviceTimingCfg = (HalDispDeviceTimingInfo_t *)pCfg;

    pstDevContain->eTimeType = pstDeviceTimingCfg->eTimeType;
    memcpy(&pstDevContain->stDevTimingCfg, &pstDeviceTimingCfg->stDeviceTimingCfg, sizeof(HalDispDeviceTimingConfig_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, H(%d %d %d %d %d) V(%d %d %d %d %d) Fps:%d, Ssc(%x %x), Peroid(%ld %ld %ld %ld)\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        pstDevContain->stDevTimingCfg.u16HsyncWidth, pstDevContain->stDevTimingCfg.u16HsyncBackPorch,
        pstDevContain->stDevTimingCfg.u16Hstart, pstDevContain->stDevTimingCfg.u16Hactive,
        pstDevContain->stDevTimingCfg.u16Htotal,
        pstDevContain->stDevTimingCfg.u16VsyncWidth, pstDevContain->stDevTimingCfg.u16VsyncBackPorch,
        pstDevContain->stDevTimingCfg.u16Vstart, pstDevContain->stDevTimingCfg.u16Vactive,
        pstDevContain->stDevTimingCfg.u16Vtotal,
        pstDevContain->stDevTimingCfg.u16Fps,
        pstDevContain->stDevTimingCfg.u16SscStep, pstDevContain->stDevTimingCfg.u16SscSpan,
        pstDevContain->stDevTimingCfg.u32VSyncPeriod, pstDevContain->stDevTimingCfg.u32VBackPorchPeriod,
        pstDevContain->stDevTimingCfg.u32VActivePeriod, pstDevContain->stDevTimingCfg.u32VFrontPorchPeriod);

    return enRet;
}

void _HalDispIfSetDeviceOutputTiming(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispDeviceTimingInfo_t *pstDeviceTimingCfg;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispHwContain_t *pstHalHwContain = NULL;
    u16 u16TblId;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstHalHwContain = pstDispCtxCfg->pstCtxContain->pstHalHwContain;
    pstDeviceTimingCfg = (HalDispDeviceTimingInfo_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%ld %ld, Interface:%lx, TimingId:%s\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx,
        pstDevContain->u32DevId,
        pstDevContain->u32Interface, PARSING_HAL_TMING_ID(pstDeviceTimingCfg->eTimeType));

    pstHalHwContain->bTimingUpdate[pstDevContain->u32DevId] = 1;

    __HalDispIfSetTgenConfig(pstDispCtxCfg);

    if(pstDevContain->u32Interface & (HAL_DISP_INTF_HDMI | HAL_DISP_INTF_VGA))
    {
        u16TblId = __HalDispIfGetTblId(pstDeviceTimingCfg->eTimeType);
        if(pstDevContain->u32Interface & HAL_DISP_INTF_VGA)
        {
            __HalDispIfDumpIdacTbl(pCtx, u16TblId);
        }

        __HalDispIfDumpSynthSetTbl(pCtx, u16TblId);
    }

    __HalDispIfSetTgenRest(pstDispCtxCfg);

    if(pstDevContain->u32Interface & HAL_DISP_INTF_CVBS)
    {
        u16TblId = __HalDispIfGetTblId(pstDeviceTimingCfg->eTimeType);

        //HalDispSetCvbsSwRst(1, pCtx);
        if(pstDeviceTimingCfg->eTimeType == E_HAL_DISP_OUTPUT_PAL)
        {
           // Vertical
           HalDispSetTgenVsyncSt(0, (void *)pstDispCtxCfg);
           HalDispSetTgenVsyncEnd(0x30, (void *)pstDispCtxCfg);
           HalDispSetTgenVfdeSt(0x31, (void *)pstDispCtxCfg);
           HalDispSetTgenVfdeEnd(0x270, (void *)pstDispCtxCfg);
           HalDispSetTgenVdeSt(0x31, (void *)pstDispCtxCfg);
           HalDispSetTgenVdeEnd(0x270, (void *)pstDispCtxCfg);
           HalDispSetTgenVtt(0x270, (void *)pstDispCtxCfg);
           // horizontal
           HalDispSetTgenHsyncSt(0, (void *)pstDispCtxCfg);
           HalDispSetTgenHsyncEnd(0x8f, (void *)pstDispCtxCfg);
           HalDispSetTgenHfdeSt(0x80, (void *)pstDispCtxCfg);
           HalDispSetTgenHfdeEnd(0x355, (void *)pstDispCtxCfg);
           HalDispSetTgenHdeSt(0x83, (void *)pstDispCtxCfg);
           HalDispSetTgenHdeEnd(0x352, (void *)pstDispCtxCfg);
           HalDispSetTgenHtt(0x35f, (void *)pstDispCtxCfg);
        }
        else if(pstDeviceTimingCfg->eTimeType == E_HAL_DISP_OUTPUT_NTSC)
        {
            // Vertical
            HalDispSetTgenVsyncSt(0, (void *)pstDispCtxCfg);
            HalDispSetTgenVsyncEnd(0x2c, (void *)pstDispCtxCfg);
            HalDispSetTgenVfdeSt(0x2D, (void *)pstDispCtxCfg);
            HalDispSetTgenVfdeEnd(0x20C, (void *)pstDispCtxCfg);
            HalDispSetTgenVdeSt(0x2D, (void *)pstDispCtxCfg);
            HalDispSetTgenVdeEnd(0x20C, (void *)pstDispCtxCfg);
            HalDispSetTgenVtt(0x20C, (void *)pstDispCtxCfg);
            // horizontal
            HalDispSetTgenHsyncSt(0, (void *)pstDispCtxCfg);
            HalDispSetTgenHsyncEnd(0x89, (void *)pstDispCtxCfg);
            HalDispSetTgenHfdeSt(0x80, (void *)pstDispCtxCfg);
            HalDispSetTgenHfdeEnd(0x351, (void *)pstDispCtxCfg);
            HalDispSetTgenHdeSt(0x84, (void *)pstDispCtxCfg);
            HalDispSetTgenHdeEnd(0x353, (void *)pstDispCtxCfg);
            HalDispSetTgenHtt(0x359, (void *)pstDispCtxCfg);
        }
        HalDispSetCvbsSwRst(0, pCtx);
        __HalDispIfDumpCvbsTbl(pCtx, u16TblId);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceVgaParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispVgaParam_t *pstVgaParm = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstVgaParm = (HalDispVgaParam_t *)pCfg;

    if(pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS ||
       pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ||
       pstVgaParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stVgaParam, pstVgaParm, sizeof(HalDispVgaParam_t));

        DISP_DBG(DISP_DBG_LEVEL_HAL,
            "%s %d, CtxId:%ld, DevId:%ld, Matrix:%s, Luma:%ld, Contrast:%ld, Hue:%ld, Sat:%ld, Gain:%ld Sharp:%ld\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
            PARSING_HAL_CSC_MATRIX(pstVgaParm->stCsc.eCscMatrix),
            pstVgaParm->stCsc.u32Luma, pstVgaParm->stCsc.u32Contrast,
            pstVgaParm->stCsc.u32Hue, pstVgaParm->stCsc.u32Saturation,
            pstVgaParm->u32Gain, pstVgaParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__, PARSING_HAL_CSC_MATRIX(pstVgaParm->stCsc.eCscMatrix));
    }

    return enRet;
}


void _HalDispIfSetDeviceVgaParam(void *pCtx, void *pCfg)
{
    HalDispVgaParam_t *pstVgaParm = NULL;
    HalDispPictureConfig_t stPictureCfg;

    pstVgaParm = (HalDispVgaParam_t *)pCfg;
    if(HalDispPictureIfTransNonLinear(pCtx, &pstVgaParm->stCsc, &pstVgaParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstVgaParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
        //ToDo Output Drvive Current
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceHdmiParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispHdmiParam_t *pstHdmiParm = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstHdmiParm = (HalDispHdmiParam_t *)pCfg;

    if(pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS ||
       pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ||
       pstHdmiParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stHdmiParam, pstHdmiParm, sizeof(HalDispHdmiParam_t));

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, Matrix:%s, Luma:%ld, Contrsat:%ld, Hue:%ld, Sat:%ld, Sharp:%ld\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
            PARSING_HAL_CSC_MATRIX(pstHdmiParm->stCsc.eCscMatrix),
            pstHdmiParm->stCsc.u32Luma, pstHdmiParm->stCsc.u32Contrast,
            pstHdmiParm->stCsc.u32Hue, pstHdmiParm->stCsc.u32Saturation,
            pstHdmiParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__, PARSING_HAL_CSC_MATRIX(pstHdmiParm->stCsc.eCscMatrix));
    }

    return enRet;
}


void _HalDispIfSetDeviceHdmiParam(void *pCtx, void *pCfg)
{
    HalDispPictureConfig_t stPictureCfg;
    HalDispHdmiParam_t *pstHdmiParm = NULL;

    pstHdmiParm = (HalDispHdmiParam_t *)pCfg;
    if(HalDispPictureIfTransNonLinear(pCtx, &pstHdmiParm->stCsc, &pstHdmiParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstHdmiParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceLcdParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispLcdParam_t *pstLcdParm = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstLcdParm = (HalDispLcdParam_t *)pCfg;

    if(pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BYPASS ||
       pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT601_TO_RGB_PC ||
       pstLcdParm->stCsc.eCscMatrix == E_HAL_DISP_CSC_MATRIX_BT709_TO_RGB_PC)
    {
        memcpy(&pstDevContain->stLcdParam, pstLcdParm, sizeof(HalDispLcdParam_t));

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, Matrix:%s, Luma:%ld, Contrast:%ld, Hue:%ld, Sat:%ld, Sharp:%ld\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
            PARSING_HAL_CSC_MATRIX(pstLcdParm->stCsc.eCscMatrix),
            pstLcdParm->stCsc.u32Luma, pstLcdParm->stCsc.u32Contrast,
            pstLcdParm->stCsc.u32Hue, pstLcdParm->stCsc.u32Saturation,
            pstLcdParm->u32Sharpness);
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support %s\n", __FUNCTION__, __LINE__, PARSING_HAL_CSC_MATRIX(pstLcdParm->stCsc.eCscMatrix));
    }

    return enRet;
}


void _HalDispIfSetDeviceLcdParam(void *pCtx, void *pCfg)
{
    HalDispLcdParam_t *pstLcdParm = NULL;
    HalDispPictureConfig_t stPictureCfg;

    pstLcdParm = (HalDispLcdParam_t *)pCfg;
    if(HalDispPictureIfTransNonLinear(pCtx, &pstLcdParm->stCsc, &pstLcdParm->u32Sharpness, &stPictureCfg))
    {
        HalDispPictureIfSetConfig(pstLcdParm->stCsc.eCscMatrix, &stPictureCfg, pCtx);
    }
    else
    {
        DISP_ERR("%s %d, Trans NonLiear Fail\n", __FUNCTION__, __LINE__);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceGammaParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispGammaParam_t *pstGammaParam = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstGammaParam = (HalDispGammaParam_t *)pCfg;

    if(pstGammaParam->bEn && pstGammaParam->u16EntryNum != 33)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, CtxId:%ld, DevId:%ld ColorEntyr is %d, not 32\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
            pstGammaParam->u16EntryNum);
    }
    else
    {
        if(pstDevContain->stGammaParam.pu8ColorR == NULL &&
           pstDevContain->stGammaParam.pu8ColorB == NULL &&
           pstDevContain->stGammaParam.pu8ColorG == NULL)
        {
            pstDevContain->stGammaParam.pu8ColorR = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorG = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorB = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
        }
        else if(pstDevContain->stGammaParam.u16EntryNum < pstGammaParam->u16EntryNum)
        {
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorR);
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorG);
            DrvDispOsMemRelease(pstDevContain->stGammaParam.pu8ColorB);
            pstDevContain->stGammaParam.pu8ColorR = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorG = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.pu8ColorB = DrvDispOsMemAlloc(pstGammaParam->u16EntryNum);
        }


        if(pstDevContain->stGammaParam.pu8ColorR == NULL ||
           pstDevContain->stGammaParam.pu8ColorG == NULL ||
           pstDevContain->stGammaParam.pu8ColorB == NULL)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, Alloc Memory Fail\n", __FUNCTION__, __LINE__);
        }
        else
        {
            memcpy(pstDevContain->stGammaParam.pu8ColorR, pstGammaParam->pu8ColorR, pstGammaParam->u16EntryNum);
            memcpy(pstDevContain->stGammaParam.pu8ColorG, pstGammaParam->pu8ColorG, pstGammaParam->u16EntryNum);
            memcpy(pstDevContain->stGammaParam.pu8ColorB, pstGammaParam->pu8ColorB, pstGammaParam->u16EntryNum);
            pstDevContain->stGammaParam.u16EntryNum = pstGammaParam->u16EntryNum;
            pstDevContain->stGammaParam.bEn = pstGammaParam->bEn;
        }
    }
    return enRet;
}



void _HalDispIfSetDeviceGammaParam(void *pCtx, void *pCfg)
{
    HalDispGammaParam_t *pstGammaParam = NULL;
    HalDispMaceGammaConfig_t stHalPqGammaCfg;

    pstGammaParam = (HalDispGammaParam_t *)pCfg;

    stHalPqGammaCfg.bUpdate = 1;
    stHalPqGammaCfg.u8En = pstGammaParam->bEn;
    memcpy(stHalPqGammaCfg.u8R, pstGammaParam->pu8ColorR, 33);
    memcpy(stHalPqGammaCfg.u8G, pstGammaParam->pu8ColorG, 33);
    memcpy(stHalPqGammaCfg.u8B, pstGammaParam->pu8ColorB, 33);

    HalDispMaceSetGammaConfig(&stHalPqGammaCfg, pCtx);
}


HalDispQueryRet_e _HalDispIfGetInfoDeviceCvbsParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetDeviceCvbsParam(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceColorTemp(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispColorTemp_t *pstColorTemp = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstColorTemp = (HalDispColorTemp_t *)pCfg;

    memcpy(&pstDevContain->stColorTemp, pstColorTemp, sizeof(HalDispColorTemp_t));

    DISP_DBG(DISP_DBG_LEVEL_COLOR, "%s %d, CtxId:%ld, DevId:%ld, Offset(%x %x %x) Color(%x %x %x]\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        pstDevContain->stColorTemp.u16RedOffset, pstDevContain->stColorTemp.u16GreenOffset, pstDevContain->stColorTemp.u16BlueOffset,
        pstDevContain->stColorTemp.u16RedColor, pstDevContain->stColorTemp.u16GreenColor, pstDevContain->stColorTemp.u16BlueColor);

    return enRet;
}

void _HalDispIfSetDeviceColorTemp(void *pCtx, void *pCfg)
{
    HalDispColorTemp_t *pstColorTemp = NULL;
    DrvDispCtxConfig_t *pstDispCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;

    pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtx->pstCtxContain->pstDevContain[pstDispCtx->u32Idx];

    pstColorTemp = (HalDispColorTemp_t *)pCfg;
    HalDispColorAdjustVideoRGB(gstInterCfg[pstDevContain->u32DevId].u8ColorId,
        pstColorTemp->u16RedColor,
        pstColorTemp->u16GreenColor,
        pstColorTemp->u16BlueColor,
        pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDevicTimeZone(void *pCtx, void *pCfg)
{
#if DISP_TIMEZONE_ISR_SUPPORT
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispTimeZone_t *pstTimeZone = NULL;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstTimeZone = (HalDispTimeZone_t *)pCfg;

    pstTimeZone->enType = __HalDispIfGetTimeZoneType(pstDispCtxCfg);
    return enRet;
#else
    return E_HAL_DISP_QUERY_RET_NOTSUPPORT;
#endif
}

void _HalDispIfSetDeviceTimeZone(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDevicTimeZoneConfig(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetDeviceTimeZoneConfig(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDevicDisplayInfo(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispDeviceTimingConfig_t *pstDevTimingCfg = NULL;
    HalDispDisplayInfo_t *pstDisplayInfo = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstDevTimingCfg = &pstDevContain->stDevTimingCfg;
    pstDisplayInfo = (HalDispDisplayInfo_t *)pCfg;

    pstDisplayInfo->u16Htotal = pstDevTimingCfg->u16Htotal;
    pstDisplayInfo->u16Vtotal = pstDevTimingCfg->u16Vtotal;
    pstDisplayInfo->u16HdeStart = pstDevTimingCfg->u16Hstart;
    pstDisplayInfo->u16VdeStart = pstDevTimingCfg->u16Vstart;
    pstDisplayInfo->u16Width = pstDevTimingCfg->u16Hactive;
    pstDisplayInfo->u16Height = pstDevTimingCfg->u16Vactive;
    pstDisplayInfo->bInterlace = FALSE;
    pstDisplayInfo->bYuvOutput = FALSE;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DevId:%ld, Total(%d %d) DeSt(%d %d), Size(%d %d), Interlace:%d, Yuv:%d\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx, pstDevContain->u32DevId,
        pstDisplayInfo->u16Htotal, pstDisplayInfo->u16Vtotal,
        pstDisplayInfo->u16HdeStart, pstDisplayInfo->u16VdeStart,
        pstDisplayInfo->u16Width, pstDisplayInfo->u16Height,
        pstDisplayInfo->bInterlace, pstDisplayInfo->bYuvOutput);

    return enRet;
}

void _HalDispIfSetDeviceDisplayInfo(void *pCtx, void *pCfg)
{

}

//-------------------------------------------------------------------------------
// VidLayer
//-------------------------------------------------------------------------------
HalDispQueryRet_e _HalDispIfGetInfoVidLayerInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetVidLayerInit(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerEnable(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispVideoLayerCtx = NULL;
    DrvDispCtxConfig_t *pstDispDevCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain = NULL;

    pstDispVideoLayerCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDispDevCtx = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32Idx];
    pstVidLayerContain = pstDispVideoLayerCtx->pstCtxContain->pstVidLayerContain[pstDispVideoLayerCtx->u32Idx];

    if(pstDispDevCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE &&
       pstDispVideoLayerCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        if(pstVidLayerContain->pstDevCtx)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, VidLayerCtxId:%ld, VidLayerId:%ld,  Arelady Bind The Device\n",
                __FUNCTION__, __LINE__,
                pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId);
        }
        else
        {
            if(pstDevContain->u32DevId == HAL_DISP_DEVICE_ID_0 &&
              (pstVidLayerContain->u32VidLayerId == E_HAL_DISP_LAYER_MOPG_VIDEO1_ID ||
               pstVidLayerContain->u32VidLayerId == E_HAL_DISP_LAYER_MOPS_VIDEO1_ID))
            {
                enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                DISP_ERR("%s %d, VidLayerCtxId:%ld, VidLayerId:%ld,  HW Not Supoorted\n",
                    __FUNCTION__, __LINE__,
                    pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId);
            }
            else
            {
                DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevCtxId:%ld, DevId:%ld, VidLayerCtxId:%ld, VidLayerId:%ld\n",
                    __FUNCTION__, __LINE__,
                    pstDispDevCtx->u32Idx, pstDevContain->u32DevId,
                    pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId);

                pstVidLayerContain->pstDevCtx = (void *)pstDevContain;
                pstDevContain->pstVidLayeCtx[pstVidLayerContain->u32VidLayerId] = (void *)pstVidLayerContain;
            }
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevCtxId:%ld, DevId:%ld(%s), VidLayerCtxId:%ld, VidLayerId:%ld(%s)\n",
            __FUNCTION__, __LINE__,
            pstDispDevCtx->u32Idx, pstDevContain->u32DevId,
            PARSING_CTX_TYPE(pstDispDevCtx->enCtxType),
            pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId,
            PARSING_CTX_TYPE(pstDispVideoLayerCtx->enCtxType));
    }

    return enRet;
}

void _HalDispIfSetVidLayerBind(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispDevCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    u8 i, u8VideLayerBindNum;

    pstDispDevCtx = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32Idx];

    u8VideLayerBindNum = 0;
    for(i=0; i<HAL_DISP_VIDLAYER_MAX; i++)
    {
        if(pstDevContain->pstVidLayeCtx[i])
        {
            u8VideLayerBindNum++;
        }
    }

    if(u8VideLayerBindNum > 2)
    { // MOP Merge case
        HalDispSetMopWinMerge(1, pCtx);
        HalDispMopIfSetInfoStretchWinMop0Auto(pCtx, FALSE);//while merge, MOP0 can only use its own clock
        HalDispClkSetMergeOdClk(pCtx);
    }
    else
    { //non Merge case
        HalDispSetMopWinMerge(0, pCtx);
        HalDispMopIfSetInfoStretchWinMop0Auto(pCtx, TRUE);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerUnBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispVideoLayerCtx = NULL;
    DrvDispCtxConfig_t *pstDispDevCtx = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain = NULL;

    pstDispVideoLayerCtx = (DrvDispCtxConfig_t *)pCtx;
    pstDispDevCtx = (DrvDispCtxConfig_t *)pCfg;
    pstDevContain = pstDispDevCtx->pstCtxContain->pstDevContain[pstDispDevCtx->u32Idx];
    pstVidLayerContain = pstDispVideoLayerCtx->pstCtxContain->pstVidLayerContain[pstDispVideoLayerCtx->u32Idx];

    if(pstDispDevCtx->enCtxType == E_DISP_CTX_TYPE_DEVICE &&
       pstDispVideoLayerCtx->enCtxType == E_DISP_CTX_TYPE_VIDLAYER)
    {
        if(pstVidLayerContain->pstDevCtx == pstDevContain)
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevCtxId:%ld, DevId:%ld, VidLayerCtxId:%ld, VidLayrId:%ld\n",
                __FUNCTION__, __LINE__,
            pstDispDevCtx->u32Idx,  pstDevContain->u32DevId,
            pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId);

            pstVidLayerContain->pstDevCtx = NULL;
            pstDevContain->pstVidLayeCtx[pstVidLayerContain->u32VidLayerId] = NULL;
        }
        else
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, DevCtxId:%ld, DevId:%ld,(%p) != VidLayerCtxId:%ld, VidLayerId:%ld(%p)\n",
                __FUNCTION__, __LINE__,
                pstDispDevCtx->u32Idx, pstDevContain->u32DevId, pstDevContain,
                pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId, pstVidLayerContain->pstDevCtx);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevCtxId:%ld, DevId:%ld(%s), VidLayerCtxId:%ld, VidLayerId:%ld(%s)\n",
            __FUNCTION__, __LINE__,
            pstDispDevCtx->u32Idx, pstDevContain->u32DevId,
            PARSING_CTX_TYPE(pstDispDevCtx->enCtxType),
            pstDispVideoLayerCtx->u32Idx, pstVidLayerContain->u32VidLayerId,
            PARSING_CTX_TYPE(pstDispVideoLayerCtx->enCtxType));
    }

    return enRet;
}

void _HalDispIfSetVidLayerUnBind(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoStretchWinSize(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetVidLayerAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInfoStretchWinSize(pCtx, pCfg);
    HalDispMopIfSetInfoHextSize(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoImiStartAddr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoImiStartAddr(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetImiStartAddr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetImiStartAddr(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerCompress(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetVidLayerComporess(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerPriority(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetVidLayerPriority(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerBufferFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetVidLayerBufferFire(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerCheckFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetVidLayerCheckFire(void *pCtx, void *pCfg)
{

}

//-------------------------------------------------------------------------------
// InputPort
//-------------------------------------------------------------------------------
HalDispQueryRet_e _HalDispIfGetInfoInputPortInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;

    enRet = HalDispMopIfGetInfoInputPortInit(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetInputPortInit(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortInit(pCtx, pCfg);
}


HalDispQueryRet_e _HalDispIfGetInfoInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortEnable(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortEnable(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortAttr(pCtx, pCfg);
    return enRet;
}


void _HalDispIfSetInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortAttr(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortShow(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetInputPortShow(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortHide(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetInputPortHide(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortBegin(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetInputPortBegin(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortEnd(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
    return enRet;
}

void _HalDispIfSetInputPortEnd(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortFlip(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortFlip(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetInputPortFlip(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortFlip(pCtx, pCfg);
}


HalDispQueryRet_e _HalDispIfGetInfoInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortRotate(pCtx, pCfg);
    return enRet;
}

void _HalDispIfSetInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortRotate(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortCrop(pCtx, pCfg);
    return enRet;
}
void _HalDispIfSetInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortCrop(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    enRet = HalDispMopIfGetInfoInputPortRingBuffAttr(pCtx, pCfg);
    return enRet;
}
void _HalDispIfSetInputPortRingBuffAttr(void *pCtx, void *pCfg)
{
    HalDispMopIfSetInputPortRingBuffAttr(pCtx, pCfg);
}

HalDispQueryRet_e _HalDispIfGetInfoClkGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkConfig_t *pstClkCfg = NULL;
    u8 u8ClkMuxAttr[HAL_DISP_CLK_NUM] = HAL_DISP_CLK_MUX_ATTR;
    u8 u8ClkName[HAL_DISP_CLK_NUM][20] = HAL_DISP_CLK_NAME;
    u32 i;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;

    pstClkCfg->u32Num = HAL_DISP_CLK_NUM;
    HalDispClkGetMop0(&pstClkCfg->bEn[0], &pstClkCfg->u32Rate[0]);
    if(u8ClkMuxAttr[0] == 0)
    {
        pstClkCfg->u32Rate[0] = HalDispClkMapMopToRate(pstClkCfg->u32Rate[0]);
    }

    HalDispClkGetMop1(&pstClkCfg->bEn[1], &pstClkCfg->u32Rate[1]);
    if(u8ClkMuxAttr[1] == 0)
    {
        pstClkCfg->u32Rate[1] = HalDispClkMapMopToRate(pstClkCfg->u32Rate[1]);
    }

    HalDispClkGetDisp432(&pstClkCfg->bEn[2], &pstClkCfg->u32Rate[2]);
    if(u8ClkMuxAttr[2] == 0)
    {
        pstClkCfg->u32Rate[2] = HalDispClkMapDisp432ToRate(pstClkCfg->u32Rate[2]);
    }

    HalDispClkGetDisp216(&pstClkCfg->bEn[3], &pstClkCfg->u32Rate[3]);
    if(u8ClkMuxAttr[3] == 0)
    {
        pstClkCfg->u32Rate[3] = HalDispClkMapDisp216ToRate(pstClkCfg->u32Rate[3]);
    }

    for(i=0; i< HAL_DISP_CLK_NUM; i++)
    {
        DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, CLK_%s, En:%d, Rate:%ld\n",
            __FUNCTION__, __LINE__,
            u8ClkName[i], pstClkCfg->bEn[i], pstClkCfg->u32Rate[i]);
    }

    return enRet;
}

void _HalDispIfSetClkGet(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoClkSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkConfig_t *pstClkCfg = NULL;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;

    if(pstClkCfg->u32Num != HAL_DISP_CLK_NUM)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Clk Num (%ld) is not match %d\n",
            __FUNCTION__, __LINE__,
            pstClkCfg->u32Num, HAL_DISP_CLK_NUM);
    }
    else
    {
        u32 i;
        u8 au8ClkName[HAL_DISP_CLK_NUM][20] = HAL_DISP_CLK_NAME;

        for(i=0; i<HAL_DISP_CLK_NUM; i++)
        {
            DISP_DBG(DISP_DBG_LEVEL_CLK, "%s %d, CLK_%s: En:%d, Rate:%ld\n",
                __FUNCTION__, __LINE__,
                au8ClkName[i], pstClkCfg->bEn[i],pstClkCfg->u32Rate[i]);
        }
    }

    return enRet;
}

void _HalDispIfSetClkSet(void *pCtx, void *pCfg)
{
    HalDispClkConfig_t *pstClkCfg = NULL;
    u8 u8ClkMuxAttr[HAL_DISP_CLK_NUM] = HAL_DISP_CLK_MUX_ATTR;
    u32 u32ClkIdx;

    pstClkCfg = (HalDispClkConfig_t *)pCfg;

    u32ClkIdx = u8ClkMuxAttr[0] ? pstClkCfg->u32Rate[0] : HalDispClkMapMopToIdx(pstClkCfg->u32Rate[0]);
    HalDispClkSetMop0(pstClkCfg->bEn[0], u32ClkIdx);

    u32ClkIdx = u8ClkMuxAttr[1] ? pstClkCfg->u32Rate[1] : HalDispClkMapMopToIdx(pstClkCfg->u32Rate[1]);
    HalDispClkSetMop1(pstClkCfg->bEn[1], u32ClkIdx);

    u32ClkIdx = u8ClkMuxAttr[2] ? pstClkCfg->u32Rate[2] : HalDispClkMapDisp432ToIdx(pstClkCfg->u32Rate[2]);
    HalDispClkSetDisp432(pstClkCfg->bEn[2], u32ClkIdx);

    u32ClkIdx = u8ClkMuxAttr[3] ? pstClkCfg->u32Rate[3] : HalDispClkMapDisp216ToIdx(pstClkCfg->u32Rate[3]);
    HalDispClkSetDisp216(pstClkCfg->bEn[3], u32ClkIdx);
}

HalDispQueryRet_e _HalDispIfGetInfoPqSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispPqConfig_t *pstPqCfg = NULL;

    pstPqCfg = (HalDispPqConfig_t *)pCfg;

    if(pstPqCfg == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Null Data\n", __FUNCTION__, __LINE__);
    }
    else
    {
        if(pstPqCfg->pData == NULL || pstPqCfg->u32DataSize == 0)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, pData=%p, Size:%ld\n",
                __FUNCTION__, __LINE__, pstPqCfg->pData, pstPqCfg->u32DataSize);
        }
    }

    return enRet;
}

void _HalDispIfSetPqSet(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg0 = NULL;
    DrvDispCtxConfig_t *pstDispCtxCfg1 = NULL;

    DrvDispCtxGetCurCtx(E_DISP_CTX_TYPE_DEVICE, 0, &pstDispCtxCfg0);
    DrvDispCtxGetCurCtx(E_DISP_CTX_TYPE_DEVICE, 1, &pstDispCtxCfg1);

    HalDispHpqLoadBin(pCtx, pCfg); // Load PQ Bin

    if(pstDispCtxCfg0)
    {
        HalDispPictureIfSetPqConfig(pstDispCtxCfg0); // Update Picture Setting
    }
    else
    {
        DISP_ERR("%s %d, Device 0 not Create\n", __FUNCTION__, __LINE__);
    }

    if(pstDispCtxCfg1)
    {
        HalDispPictureIfSetPqConfig(pstDispCtxCfg1); // Update Picture Setting
    }
    else
    {
        DISP_ERR("%s %d, Device 1 not Create\n", __FUNCTION__, __LINE__);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoPqGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetPqGet(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDrvTurningSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetDrvTurningSet(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDrvTurningGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;

}

void _HalDispIfSetDrvTurningGet(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoDbgmgGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    HalDispDbgmgConfig_t *pstDbgmgCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = (HalDispUtilityCmdqContext_t *)pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];
    pstDbgmgCfg = (HalDispDbgmgConfig_t *)pCfg;

    DrvDispOsSprintfStrcat(pstDbgmgCfg->pData, "Cmdq_%ld:: SynCnt:%d, WaitCnt:%d, FlipCnt:%d\n",
                pstCmdqCtx->s32CmdqId,
                pstCmdqCtx->u16SyncCnt, pstCmdqCtx->u16WaitDoneCnt, pstCmdqCtx->u16RegFlipCnt);

    return enRet;
}

void _HalDispIfSetDbgmgGet(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoRegAccess(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *pstCmdqCtx = NULL;
    HalDispRegAccessConfig_t *pstRegAccessCfg = (HalDispRegAccessConfig_t *)pCfg;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    DISP_DBG(DISP_DBG_LEVEL_UTILITY_CMDQ, "%s %d, AccessMode:%s, CmdqHandler:%p\n",
        __FUNCTION__, __LINE__, PARSING_HAL_REG_ACCESS_TYPE(pstRegAccessCfg->enType), pstCmdqCtx);

    return enRet;
}

void _HalDispIfSetRegAccess(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispRegAccessConfig_t *pstRegAccessCfg = (HalDispRegAccessConfig_t *)pCfg;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];

    if(pstRegAccessCfg->pCmdqInf)
    {
        HalDispUtilitySetCmdqInf(pstRegAccessCfg->pCmdqInf, pstDevContain->u32DevId);
    }

    if(pstRegAccessCfg->enType == E_HAL_DISP_REG_ACCESS_CMDQ)
    {
        HalDispUtilitySetRegAccessMode(E_DISP_UTILITY_REG_ACCESS_CMDQ);
    }
    else
    {
        HalDispUtilitySetRegAccessMode(E_DISP_UTILITY_REG_ACCESS_CPU);
    }
    HalDispMopIfSetForceWr(pCtx, 0);
}


HalDispQueryRet_e _HalDispIfGetInfoRegFlip(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t  *pstCmdqCtx = NULL;
    HalDispRegFlipConfig_t *pstRegFlipCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstRegFlipCfg = (HalDispRegFlipConfig_t *)pCfg;
    pstCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    if(pstCmdqCtx)
    {
        if((HalDispUtilityGetRegAccessMode() == E_HAL_DISP_REG_ACCESS_CPU) ||
           pstRegFlipCfg->pCmdqInf == NULL)
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%ld, RIU MODE\n",
                __FUNCTION__, __LINE__,
                PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType),
                pstCmdqCtx->s32CmdqId);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, CtxType:%s, CmdqId:%ld, CMDQ MODE\n",
                __FUNCTION__, __LINE__,
                PARSING_CTX_TYPE(pstDispCtxCfg->enCtxType),
                pstCmdqCtx->s32CmdqId);
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, CmdqId:%ld, CmdqCtx:%p, CmdqHander:%p \n",
            __FUNCTION__, __LINE__,
            pstCmdqCtx->s32CmdqId, pstCmdqCtx, pstRegFlipCfg->pCmdqInf);
    }

    return enRet;
}

void _HalDispIfSetRegFlip(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    void *pstCmdqCtx = NULL;
    HalDispRegFlipConfig_t *pstRegFlipCfg = (HalDispRegFlipConfig_t *)pCfg;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    if(pstRegFlipCfg->pCmdqInf)
    {
        HalDispUtilitySetCmdqInf(pstRegFlipCfg->pCmdqInf, pstDevContain->u32DevId);
    }

    if(pstRegFlipCfg->bEnable)
    {
        if(pstCmdqCtx)
        {
            __HalDispIfSetRegFlipPreAct(pstDispCtxCfg);

            HalDispUtilityW2BYTEMSKDirectCmdqWrite(pstCmdqCtx);

            __HalDispIfSetRegFlipPostAct(pstDispCtxCfg);
        }
    }
    else
    {
        HalDispUtilityResetCmdqCnt(pstCmdqCtx);
    }
}

HalDispQueryRet_e _HalDispIfGetInfoRegWaitDone(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    HalDispRegWaitDoneConfig_t *pstRegWaitDoneCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstRegWaitDoneCfg = (HalDispRegWaitDoneConfig_t *)pCfg;
    pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

    if(pstCmdqCtx && pstRegWaitDoneCfg->pCmdqInf)
    {
        DISP_DBG(DISP_DBG_LEVEL_UTILITY_FLIP, "%s %d, WaitDoneCnt:%d\n",
            __FUNCTION__, __LINE__,
            ((HalDispUtilityCmdqContext_t *)pstCmdqCtx)->u16WaitDoneCnt);
        enRet = E_HAL_DISP_QUERY_RET_OK;
    }
    else
    {
        if(HalDispUtilityGetRegAccessMode() == E_DISP_UTILITY_REG_ACCESS_CMDQ)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CmdqCtx:%p, CmdqHander:%p \n",
                __FUNCTION__, __LINE__,
                pstCmdqCtx, pstRegWaitDoneCfg->pCmdqInf);
        }
    }

    return enRet;
}


void _HalDispIfSetRegWaitDone(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDeviceContain_t *pstDevContain = NULL;
    HalDispUtilityCmdqContext_t *pstCmdqCtx = NULL;
    HalDispRegWaitDoneConfig_t *pstRegWaitDoneCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstRegWaitDoneCfg = (HalDispRegWaitDoneConfig_t *)pCfg;

    if(pstDispCtxCfg->enCtxType == E_DISP_CTX_TYPE_DEVICE)
    {
        pstDevContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
        pstCmdqCtx = pstDispCtxCfg->pstCtxContain->pstHalHwContain->pvCmdqCtx[pstDevContain->u32DevId];

        if(pstRegWaitDoneCfg->pCmdqInf)
        {
            HalDispUtilitySetCmdqInf(pstRegWaitDoneCfg->pCmdqInf, pstDevContain->u32DevId);
        }
        if(!(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx]->u32Interface & HAL_DISP_INTF_MCU) &&
           !(pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx]->u32Interface & HAL_DISP_INTF_MCU_NOFLM))
        {
            __HalDispIfSetWaitDonePreAct((HalDispUtilityCmdqContext_t *)pstCmdqCtx);
            HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait even
            __HalDispIfSetWaitDonePostAct((HalDispUtilityCmdqContext_t *)pstCmdqCtx); // Clear disp2cmdq intterupt
        }
        else
        {
            __HalDispIfSetWaitDonePreAct((HalDispUtilityCmdqContext_t *)pstCmdqCtx);

            // Wait FLM rising edge
            if (pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx]->u32Interface & HAL_DISP_INTF_MCU)
            {
                #if CMDQ_POLL_EQ_MODE  //polling mode
                HalDispUtilityPollWait((void *)pstRegWaitDoneCfg->pCmdqInf, REG_LCD_0B_L,0x200, 0x200, 0, 1);
                #else
                //wait mode
                HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait even
                #endif
            }
            __HalDispIfSetWaitDonePostActForMcuFlm((HalDispUtilityCmdqContext_t *)pstCmdqCtx);

            #if CMDQ_POLL_EQ_MODE  //polling mode
            HalDispUtilityPollWait((void *)pstRegWaitDoneCfg->pCmdqInf, REG_LCD_0B_L,0x1, 0x1, 0, 1);// add wait event  wait frame done status
            #else
            //wait mode
            HalDispUtilityAddWaitCmd(pstCmdqCtx); // add wait event
            #endif
            // Clear frame done status
            __HalDispIfSetWaitDonePostActForMcuFrameDone((HalDispUtilityCmdqContext_t *)pstCmdqCtx);
        }
    }
}

HalDispQueryRet_e _HalDispIfGetInfoDmaInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetDmaInit(void *pCtx, void *pCfg)
{
    HalDispDmaIfInit(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDmaDeInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetDmaDeInit(void *pCtx, void *pCfg)
{
    HalDispDmaIfDeInit(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDmaBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxConfig_t *pstSrcCtxCfg = NULL;
    DrvDispCtxConfig_t *pstDestCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBindConfig_t *pstDmaBindCfg =NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDmaBindCfg = (HalDispDmaBindConfig_t *)pCfg;

    if(pstDmaBindCfg->pDestDevCtx && pstDmaContain->pDestDevContain)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Dest DevContain Already Bind\n", __FUNCTION__, __LINE__);
    }
    else if(pstDmaBindCfg->pSrcDevCtx && pstDmaContain->pSrcDevContain)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Src DevContain Already Bind\n", __FUNCTION__, __LINE__);
    }
    else
    {
        pstDestCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pDestDevCtx;
        pstSrcCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pSrcDevCtx;

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DmaId:%ld\n",
            __FUNCTION__, __LINE__,
            pstDispCtxCfg->u32Idx, pstDmaContain->u32DmaId);

        if(pstDestCtxCfg && pstSrcCtxCfg)
        {
            if(pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32Idx]->u32DevId !=
               pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32Idx]->u32DevId)
            {
                pstDmaContain->pDestDevContain = pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32Idx];
                pstDmaContain->pSrcDevContain = pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32Idx];

                DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld DmaId:%ld, SrcDevId:%ld, DestDevId:%ld\n",
                    __FUNCTION__, __LINE__,
                    pstDispCtxCfg->u32Idx, pstDmaContain->u32DmaId,
                   ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId,
                   ((DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain)->u32DevId);
            }
            else
            {
                    enRet = E_HAL_DISP_QUERY_RET_CFGERR;
                    DISP_ERR("%s %d, SrcDevId(%ld) == DestDevId(%ld)\n",
                        __FUNCTION__, __LINE__,
                        pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32Idx]->u32DevId,
                        pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32Idx]->u32DevId);
            }
        }
    }

    return enRet;
}

void _HalDispIfSetDmaBind(void *pCtx, void *pCfg)
{
    HalDispDmaIfBind(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDmaUnBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxConfig_t *pstSrcCtxCfg = NULL;
    DrvDispCtxConfig_t *pstDestCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBindConfig_t *pstDmaBindCfg =NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDmaBindCfg = (HalDispDmaBindConfig_t *)pCfg;

    pstSrcCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pSrcDevCtx;
    pstDestCtxCfg = (DrvDispCtxConfig_t *)pstDmaBindCfg->pDestDevCtx;

    if(pstSrcCtxCfg && pstDestCtxCfg)
    {
        if(pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32Idx] != pstDmaContain->pSrcDevContain)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CtxId:%ld DmaId:%ld, SrcContain Not Match %p != %p\n",
                __FUNCTION__, __LINE__,
                pstDispCtxCfg->u32Idx, pstDmaContain->u32DmaId,
                pstSrcCtxCfg->pstCtxContain->pstDevContain[pstSrcCtxCfg->u32Idx],
                pstDmaContain->pSrcDevContain);
        }
        else if(pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32Idx] != pstDmaContain->pDestDevContain)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, CtxId:%ld DmaId:%ld, DestContain Not Match %p != %p\n",
                __FUNCTION__, __LINE__,
                pstDispCtxCfg->u32Idx, pstDmaContain->u32DmaId,
                pstDestCtxCfg->pstCtxContain->pstDevContain[pstDestCtxCfg->u32Idx],
                pstDmaContain->pDestDevContain);
        }
        else
        {
            DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, CtxId:%ld, DmaId:%ld, SrcDevId:%ld, DestDevId:%ld\n",
                __FUNCTION__, __LINE__,
                pstDispCtxCfg->u32Idx, pstDmaContain->u32DmaId,
                ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId,
                ((DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain)->u32DevId);

        }
    }
    else
    {
        DISP_ERR("%s %d, NULL Ctx, SrcCtx:%p, DestCtx:%p \n",
            __FUNCTION__, __LINE__, pstSrcCtxCfg, pstDestCtxCfg);
    }

    return enRet;
}

void _HalDispIfSetDmaUnBind(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;

    HalDispDmaIfUnBind(pCtx);

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];

    pstDmaContain->pDestDevContain = NULL;
    pstDmaContain->pSrcDevContain = NULL;
}


HalDispQueryRet_e _HalDispIfGetInfoDmaAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaAttrConfig_t *pstDmaAttrCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDmaAttrCfg = (HalDispDmaAttrConfig_t *)pCfg;

    if(pstDmaContain->pDestDevContain == NULL || pstDmaContain->pSrcDevContain == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContain NULL, SrcDevContain:%p, DestDevContain:%p \n",
            __FUNCTION__, __LINE__,
            pstDmaContain->pSrcDevContain, pstDmaContain->pDestDevContain);
    }
    else
    {
        memcpy(&pstDmaContain->stInputCfg, &pstDmaAttrCfg->stInputCfg, sizeof(HalDispDmaInputConfig_t));
        memcpy(&pstDmaContain->stOutputCfg, &pstDmaAttrCfg->stOutputCfg, sizeof(HalDispDmaOutputConfig_t));

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, SrcDevId:%ld, DestDevId:%ld, In(%s %s %s, %d %d) Out(%s %s %s %s %d %d)\n",
            __FUNCTION__, __LINE__,
            ((DrvDispCtxDeviceContain_t *)pstDmaContain->pSrcDevContain)->u32DevId,
            ((DrvDispCtxDeviceContain_t *)pstDmaContain->pDestDevContain)->u32DevId,
            PARSING_HAL_DMA_INPUT_TYPE(pstDmaContain->stInputCfg.eType),
            PARSING_HAL_DMA_PIXEL_FMT(pstDmaContain->stInputCfg.ePixelFormat),
            PARSING_HAL_COMPRESS_MD(pstDmaContain->stInputCfg.eCompressMode),
            pstDmaContain->stInputCfg.u16Width, pstDmaContain->stInputCfg.u16Height,
            PARSING_HAL_DMA_ACCESS_TYPE(pstDmaContain->stOutputCfg.eAccessType),
            PARSING_HAL_DMA_OUTPUT_MODE(pstDmaContain->stOutputCfg.eMode),
            PARSING_HAL_DMA_PIXEL_FMT(pstDmaContain->stOutputCfg.ePixelFormat),
            PARSING_HAL_COMPRESS_MD(pstDmaContain->stOutputCfg.eCompressMode),
            pstDmaContain->stOutputCfg.u16Width, pstDmaContain->stOutputCfg.u16Height);
    }
    return enRet;
}

void _HalDispIfSetDmaAttr(void *pCtx, void *pCfg)
{
    HalDispDmaIfSetAttr(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoDmaBufferAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxDmaContain_t *pstDmaContain = NULL;
    HalDispDmaBufferAttrConfig_t *pstDmaBufferAttrCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDmaContain = pstDispCtxCfg->pstCtxContain->pstDmaContain[pstDispCtxCfg->u32Idx];
    pstDmaBufferAttrCfg = (HalDispDmaBufferAttrConfig_t *)pCfg;

    if(pstDmaContain->pDestDevContain == NULL || pstDmaContain->pSrcDevContain == NULL)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, DevContain NULL, SrcDevContain:%p, DestDevContain:%p \n",
            __FUNCTION__, __LINE__,
            pstDmaContain->pSrcDevContain, pstDmaContain->pDestDevContain);
    }
    else
    {
        memcpy(&pstDmaContain->stBufferAttrCfg, pstDmaBufferAttrCfg, sizeof(HalDispDmaBufferAttrConfig_t));

        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d, PhyAddr(%llx %llx %llx) Stride(%ld %ld %ld), RingBuffHeight:%d, RingStartLine:%d, FrameIdx:%d\n",
            __FUNCTION__, __LINE__,
            pstDmaContain->stBufferAttrCfg.bEn,
            pstDmaContain->stBufferAttrCfg.u64PhyAddr[0], pstDmaContain->stBufferAttrCfg.u64PhyAddr[1], pstDmaContain->stBufferAttrCfg.u64PhyAddr[2],
            pstDmaContain->stBufferAttrCfg.u32Stride[0], pstDmaContain->stBufferAttrCfg.u32Stride[1], pstDmaContain->stBufferAttrCfg.u32Stride[2],
            pstDmaContain->stBufferAttrCfg.u16RingBuffHeight,
            pstDmaContain->stBufferAttrCfg.u16RingStartLine,
            pstDmaContain->stBufferAttrCfg.u16FrameIdx);
    }

    return enRet;
}

void _HalDispIfSetDmaBufferAttr(void *pCtx, void *pCfg)
{
    HalDispDmaIfSetBufferAttr(pCtx);
}

HalDispQueryRet_e _HalDispIfGetInfoHwInfoConfig(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispHwInfoConfig_t *pstHwInfoCfg = (HalDispHwInfoConfig_t *)pCfg;

    if(pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_DEVICE)
    {
        pstHwInfoCfg->u32Count = HAL_DISP_DEVICE_MAX;
    }
    else if(pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_VIDEOLAYER)
    {
        pstHwInfoCfg->u32Count = HAL_DISP_VIDLAYER_MAX;
    }
    else if(pstHwInfoCfg->eType == E_HAL_DISP_HW_INFO_INPUTPORT)
    {
        if(pstHwInfoCfg->u32Id >= HAL_DISP_VIDLAYER_MAX)
        {
            enRet = E_HAL_DISP_QUERY_RET_CFGERR;
            DISP_ERR("%s %d, VidLayerId:%ld, is out of range\n",
                __FUNCTION__, __LINE__, pstHwInfoCfg->u32Id);
        }
        else
        {
            if(pstHwInfoCfg->u32Id == HAL_DISP_VIDLAYER_ID_0 ||
               pstHwInfoCfg->u32Id == HAL_DISP_VIDLAYER_ID_2)
            {
                pstHwInfoCfg->u32Count = HAL_DISP_MOPG_GWIN_NUM;
            }
            else
            {
                pstHwInfoCfg->u32Count = HAL_DISP_MOPS_GWIN_NUM;
            }
        }
    }
    else
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Unknown Type:%d\n", __FUNCTION__, __LINE__, pstHwInfoCfg->eType);
    }
    return enRet;
}

void _HalDispIfSetHwInfoConfig(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoClkInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispClkInitConfig_t *pstClkInitCfg = (HalDispClkInitConfig_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, ClkInit:%d\n", __FUNCTION__, __LINE__, pstClkInitCfg->bEn);
    return enRet;
}

void _HalDispIfSetClkInit(void *pCtx, void *pCfg)
{
    HalDispClkInitConfig_t *pstClkInitCfg = (HalDispClkInitConfig_t *)pCfg;

    HalDispClkInit(pstClkInitCfg->bEn);
}

HalDispQueryRet_e _HalDispIfGetInfoCmdqInf(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispCmdqInfConfig_t *pstCmdqInfCfg = (HalDispCmdqInfConfig_t *)pCfg;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%ld, CmdqInf:%p\n",
        __FUNCTION__, __LINE__, pstCmdqInfCfg->u32DevId, pstCmdqInfCfg->pCmdqInf);
    return enRet;
}

void _HalDispIfSetCmdqInf(void *pCtx, void *pCfg)
{
    HalDispCmdqInfConfig_t *pstCmdqInfCfg = (HalDispCmdqInfConfig_t *)pCfg;

    if(pstCmdqInfCfg->pCmdqInf)
    {
        HalDispUtilitySetCmdqInf(pstCmdqInfCfg->pCmdqInf, pstCmdqInfCfg->u32DevId);
    }
}


HalDispQueryRet_e _HalDispIfGetInfoInterCfgSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispInternalConfig_t *pstInterCfg = NULL;
    DrvDispCtxConfig_t *pstDispCtx = NULL;

    pstDispCtx = (DrvDispCtxConfig_t *)pCtx;
    pstInterCfg = (HalDispInternalConfig_t *)pCfg;

    if(pstInterCfg->eType & E_HAL_DISP_INTER_CFG_BOOTLOGO)
    {
        gstInterCfg[pstDispCtx->u32Idx].bBootlogoEn = pstInterCfg->bBootlogoEn;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%ld BootLogo En:%d\n",
            __FUNCTION__, __LINE__,
            pstDispCtx->u32Idx,
            gstInterCfg[pstDispCtx->u32Idx].bBootlogoEn);
    }

    if(pstInterCfg->eType & E_HAL_DISP_INTER_CFG_COLORID)
    {
        gstInterCfg[pstDispCtx->u32Idx].u8ColorId = pstInterCfg->u8ColorId < 2 ? pstInterCfg->u8ColorId : 0;
        DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%ld, ColorID:%d\n",
            __FUNCTION__, __LINE__,
            pstDispCtx->u32Idx,
            gstInterCfg[pstDispCtx->u32Idx].u8ColorId);
    }

    if(pstInterCfg->eType & E_HAL_DISP_INTER_CFG_GOPBLENDID)
    {
        enRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Not Support GOP Blend ID\n", __FUNCTION__, __LINE__);
    }

    if(enRet == E_HAL_DISP_QUERY_RET_OK)
    {
        gstInterCfg[pstDispCtx->u32Idx].eType |= pstInterCfg->eType;
    }
    return enRet;
}

void _HalDispIfSetInterCfgSet(void *pCtx, void *pCfg)
{
}


HalDispQueryRet_e _HalDispIfGetInfoInterCfgGet(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    HalDispInternalConfig_t *pstInterCfg = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInterCfg = (HalDispInternalConfig_t *)pCfg;

    memcpy(pstInterCfg, &gstInterCfg[pstDispCtxCfg->u32Idx], sizeof(HalDispInternalConfig_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, DevId:%ld, Type:%x, BootLogo:%d, ColorID:%d, GopBlendId:%d\n",
        __FUNCTION__, __LINE__,
        pstDispCtxCfg->u32Idx,
        pstInterCfg->eType, pstInterCfg->bBootlogoEn,
        pstInterCfg->u8ColorId, pstInterCfg->u8GopBlendId);
    return enRet;
}

void _HalDispIfSetInterCfgGet(void *pCtx, void *pCfg)
{
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

bool _HalDispIfGetCallBack(DrvDispCtxConfig_t *pstDispCfg, HalDispQueryConfig_t *pstQueryCfg)
{
    memset(&pstQueryCfg->stOutCfg, 0, sizeof(HalDispQueryOutConfig_t));

    if(pstQueryCfg->stInCfg.u32CfgSize != gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize)
    {
        pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Query:%s, Size %ld != %d\n",
            __FUNCTION__, __LINE__,
            PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType),
            pstQueryCfg->stInCfg.u32CfgSize,
            gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize);
    }
    else
    {
        pstQueryCfg->stOutCfg.pSetFunc = gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pSetFunc;

        if(pstQueryCfg->stOutCfg.pSetFunc == NULL)
        {
            pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
            DISP_ERR("%s %d, Query:%s, SetFunc Empty\n",
                __FUNCTION__, __LINE__,
                PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType));
        }
        else
        {
            pstQueryCfg->stOutCfg.enQueryRet =
                gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pGetInfoFunc(pstDispCfg, pstQueryCfg->stInCfg.pInCfg);

        }
    }

    return 1;
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool HalDispIfInit(void)
{
    void *pNull;

    if(gbDispHwIfInit)
    {
        return 1;
    }

    pNull = NULL;

    memset(gpDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t)*E_HAL_DISP_QUERY_MAX);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pGetInfoFunc = _HalDispIfGetInfoDeviceInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pSetFunc     = _HalDispIfSetDeviceInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].pGetInfoFunc = _HalDispIfGetInfoDeviceDeInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].pSetFunc     = _HalDispIfSetDeviceDeInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DEINIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pGetInfoFunc = _HalDispIfGetInfoDeviceEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pSetFunc     = _HalDispIfSetDeviceEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pGetInfoFunc = _HalDispIfGetInfoDeviceAttach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pSetFunc     = _HalDispIfSetDeviceAttach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pGetInfoFunc = _HalDispIfGetInfoDeviceDetach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pSetFunc     = _HalDispIfSetDeviceDetach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pGetInfoFunc = _HalDispIfGetInfoDeviceBackGroundColor;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pSetFunc     = _HalDispIfSetDeviceBackGroundColor;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pGetInfoFunc = _HalDispIfGetInfoDeviceInterface;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pSetFunc     = _HalDispIfSetDeviceInterface;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pGetInfoFunc = _HalDispIfGetInfoDeviceOutputTiming;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pSetFunc     = _HalDispIfSetDeviceOutputTiming;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].u16CfgSize   = sizeof(HalDispDeviceTimingInfo_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceVgaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pSetFunc     = _HalDispIfSetDeviceVgaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].u16CfgSize   = sizeof(HalDispVgaParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceCvbsParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pSetFunc     = _HalDispIfSetDeviceCvbsParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].u16CfgSize   = sizeof(HalDispCvbsParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].pGetInfoFunc = _HalDispIfGetInfoDeviceColorTemp;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].pSetFunc     = _HalDispIfSetDeviceColorTemp;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_COLORTEMP].u16CfgSize   = sizeof(HalDispColorTemp_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pGetInfoFunc =_HalDispIfGetInfoDeviceHdmiParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pSetFunc     = _HalDispIfSetDeviceHdmiParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].u16CfgSize   = sizeof(HalDispHdmiParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].pGetInfoFunc =_HalDispIfGetInfoDeviceLcdParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].pSetFunc     = _HalDispIfSetDeviceLcdParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_LCD_PARAM].u16CfgSize   = sizeof(HalDispLcdParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].pGetInfoFunc =_HalDispIfGetInfoDeviceGammaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].pSetFunc     = _HalDispIfSetDeviceGammaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_GAMMA_PARAM].u16CfgSize   = sizeof(HalDispGammaParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pGetInfoFunc =_HalDispIfGetInfoDevicTimeZone;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pSetFunc     = _HalDispIfSetDeviceTimeZone;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].u16CfgSize   = sizeof(HalDispTimeZone_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].pGetInfoFunc =_HalDispIfGetInfoDevicTimeZoneConfig;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].pSetFunc     = _HalDispIfSetDeviceTimeZoneConfig;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE_CFG].u16CfgSize   = sizeof(HalDispTimeZoneConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].pGetInfoFunc =_HalDispIfGetInfoDevicDisplayInfo;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].pSetFunc     = _HalDispIfSetDeviceDisplayInfo;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DISPLAY_INFO].u16CfgSize   = sizeof(HalDispDisplayInfo_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pGetInfoFunc = _HalDispIfGetInfoVidLayerInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pSetFunc     = _HalDispIfSetVidLayerInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pGetInfoFunc = _HalDispIfGetInfoVidLayerEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pSetFunc     = _HalDispIfSetVidLayerEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pSetFunc     = _HalDispIfSetVidLayerBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pSetFunc     = _HalDispIfSetVidLayerUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pGetInfoFunc = _HalDispIfGetInfoVidLayerAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pSetFunc     = _HalDispIfSetVidLayerAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].u16CfgSize   = sizeof(HalDispVideoLayerAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pGetInfoFunc = _HalDispIfGetInfoVidLayerCompress;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pSetFunc     = _HalDispIfSetVidLayerComporess;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].u16CfgSize   = sizeof(HalDispVideoLayerCompressAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pGetInfoFunc = _HalDispIfGetInfoVidLayerPriority;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pSetFunc     = _HalDispIfSetVidLayerPriority;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerBufferFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pSetFunc     = _HalDispIfSetVidLayerBufferFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerCheckFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pSetFunc     = _HalDispIfSetVidLayerCheckFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pGetInfoFunc = _HalDispIfGetInfoInputPortInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pSetFunc     = _HalDispIfSetInputPortInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pGetInfoFunc = _HalDispIfGetInfoInputPortEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pSetFunc     = _HalDispIfSetInputPortEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pGetInfoFunc = _HalDispIfGetInfoInputPortAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pSetFunc     = _HalDispIfSetInputPortAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].u16CfgSize   = sizeof(HalDispInputPortAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pGetInfoFunc = _HalDispIfGetInfoInputPortShow;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pSetFunc     = _HalDispIfSetInputPortShow;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pGetInfoFunc = _HalDispIfGetInfoInputPortHide;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pSetFunc     = _HalDispIfSetInputPortHide;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pGetInfoFunc = _HalDispIfGetInfoInputPortBegin;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pSetFunc     = _HalDispIfSetInputPortBegin;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pGetInfoFunc = _HalDispIfGetInfoInputPortEnd;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pSetFunc     = _HalDispIfSetInputPortEnd;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pGetInfoFunc = _HalDispIfGetInfoInputPortFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pSetFunc     = _HalDispIfSetInputPortFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].u16CfgSize   = sizeof(HalDispVideoFrameData_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pGetInfoFunc = _HalDispIfGetInfoInputPortRotate;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pSetFunc     = _HalDispIfSetInputPortRotate;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].u16CfgSize   = sizeof(HalDispInputPortRotate_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pGetInfoFunc = _HalDispIfGetInfoInputPortCrop;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pSetFunc     = _HalDispIfSetInputPortCrop;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].u16CfgSize   = sizeof(HalDispVidWinRect_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].pGetInfoFunc = _HalDispIfGetInfoInputPortRingBuffAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].pSetFunc     = _HalDispIfSetInputPortRingBuffAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_RING_BUFF_ATTR].u16CfgSize   = sizeof(HalDispRingBuffAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].pGetInfoFunc = _HalDispIfGetInfoImiStartAddr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].pSetFunc     = _HalDispIfSetImiStartAddr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_IMIADDR].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pGetInfoFunc = _HalDispIfGetInfoClkSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pSetFunc     = _HalDispIfSetClkSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pGetInfoFunc = _HalDispIfGetInfoClkGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pSetFunc     = _HalDispIfSetClkGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].pGetInfoFunc = _HalDispIfGetInfoPqSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].pSetFunc     = _HalDispIfSetPqSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_SET].u16CfgSize   = sizeof(HalDispPqConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].pGetInfoFunc = _HalDispIfGetInfoPqGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].pSetFunc     = _HalDispIfSetPqGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_PQ_GET].u16CfgSize   = sizeof(HalDispPqConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].pGetInfoFunc = _HalDispIfGetInfoDrvTurningSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].pSetFunc     = _HalDispIfSetDrvTurningSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_SET].u16CfgSize   = sizeof(HalDispDrvTurningConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].pGetInfoFunc = _HalDispIfGetInfoDrvTurningGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].pSetFunc     = _HalDispIfSetDrvTurningGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DRVTURNING_GET].u16CfgSize   = sizeof(HalDispDrvTurningConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].pGetInfoFunc = _HalDispIfGetInfoDbgmgGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].pSetFunc     = _HalDispIfSetDbgmgGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_DBGMG_GET].u16CfgSize   = sizeof(HalDispDbgmgConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].pGetInfoFunc = _HalDispIfGetInfoRegAccess;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].pSetFunc     = _HalDispIfSetRegAccess;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_ACCESS].u16CfgSize   = sizeof(HalDispRegAccessConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].pGetInfoFunc = _HalDispIfGetInfoRegFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].pSetFunc     = _HalDispIfSetRegFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_FLIP].u16CfgSize   = sizeof(HalDispRegFlipConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].pGetInfoFunc = _HalDispIfGetInfoRegWaitDone;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].pSetFunc     = _HalDispIfSetRegWaitDone;
    gpDispCbTbl[E_HAL_DISP_QUERY_REG_WAITDONE].u16CfgSize   = sizeof(HalDispRegWaitDoneConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].pGetInfoFunc = _HalDispIfGetInfoDmaInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].pSetFunc     = _HalDispIfSetDmaInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].pGetInfoFunc = _HalDispIfGetInfoDmaDeInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].pSetFunc     = _HalDispIfSetDmaDeInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_DEINIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].pGetInfoFunc = _HalDispIfGetInfoDmaBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].pSetFunc     = _HalDispIfSetDmaBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BIND].u16CfgSize   = sizeof(HalDispDmaBindConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].pGetInfoFunc = _HalDispIfGetInfoDmaUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].pSetFunc     = _HalDispIfSetDmaUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_UNBIND].u16CfgSize   = sizeof(HalDispDmaBindConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].pGetInfoFunc = _HalDispIfGetInfoDmaAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].pSetFunc     = _HalDispIfSetDmaAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_ATTR].u16CfgSize   = sizeof(HalDispDmaAttrConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].pGetInfoFunc = _HalDispIfGetInfoDmaBufferAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].pSetFunc     = _HalDispIfSetDmaBufferAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_DMA_BUFFERATTR].u16CfgSize   = sizeof(HalDispDmaBufferAttrConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].pGetInfoFunc = _HalDispIfGetInfoHwInfoConfig;
    gpDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].pSetFunc     = _HalDispIfSetHwInfoConfig;
    gpDispCbTbl[E_HAL_DISP_QUERY_HW_INFO].u16CfgSize   = sizeof(HalDispHwInfoConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].pGetInfoFunc = _HalDispIfGetInfoClkInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].pSetFunc     = _HalDispIfSetClkInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_INIT].u16CfgSize   = sizeof(HalDispClkInitConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].pGetInfoFunc = _HalDispIfGetInfoCmdqInf;
    gpDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].pSetFunc     = _HalDispIfSetCmdqInf;
    gpDispCbTbl[E_HAL_DISP_QUERY_CMDQINF].u16CfgSize   = sizeof(HalDispCmdqInfConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].pGetInfoFunc = _HalDispIfGetInfoInterCfgSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].pSetFunc     = _HalDispIfSetInterCfgSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_SET].u16CfgSize   = sizeof(HalDispInternalConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].pGetInfoFunc = _HalDispIfGetInfoInterCfgGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].pSetFunc     = _HalDispIfSetInterCfgGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_INTERCFG_GET].u16CfgSize   = sizeof(HalDispInternalConfig_t);

    gbDispHwIfInit = 1;

    return 1;
}

bool HalDispIfDeInit(void)
{
    if(gbDispHwIfInit == 0)
    {
        DISP_ERR("%s %d, HalIf not init\n", __FUNCTION__, __LINE__);
        return 0;
    }
    gbDispHwIfInit = 0;
    memset(gpDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t)* E_HAL_DISP_QUERY_MAX);

    HalDispMopIfInputPortDeinit();

    return 1;
}

bool HalDispIfQuery(void *pCtx,  void *pCfg)
{
    bool bRet = 1;

    if(pCtx == NULL)
    {
        DISP_ERR("%s %d, Input Ctx is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else if(pCfg == NULL)
    {
        DISP_ERR("%s %d, Input Cfg is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = _HalDispIfGetCallBack((DrvDispCtxConfig_t *)pCtx, (HalDispQueryConfig_t *)pCfg);
    }

    return bRet;
}

