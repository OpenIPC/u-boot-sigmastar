/*
* hal_disp.h- Sigmastar
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

#ifndef _HAL_DISP_H_
#define _HAL_DISP_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define GET_DISP_ID(pstDispCtx) \
    ((pstDispCtx) ? (pstDispCtx)->pstCtxContain->pstDevContain[pstDispCtx->u32ContainIdx]->u32DevId : HAL_DISP_DEVICE_MAX)
#define GET_DISP_ID_BYDEVCONTAIN(pstDevContain) \
            ((pstDevContain) ? pstDevContain->u32DevId : HAL_DISP_DEVICE_MAX)
#define GET_DISP_RED(ID, BANKNAME, REGNAME) ((ID == 0) ? BANKNAME##0_##REGNAME : (ID == 1) ? BANKNAME##1_##REGNAME : 0)
//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalDispQueryRet_e (*pGetInfoFunc)(void *, void *);
    void (*pSetFunc)(void *, void *);
    u16 u16CfgSize;
} HalDispQueryCallBackFunc_t;
typedef struct
{
    u8    bClkGpCtrl[HAL_DISP_DEVICE_MAX];
    void *pvCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM]; // HalDispUtilityCmdqContext_t
    void *pvClkGpCtrl[HAL_DISP_DEVICE_MAX];
} HalDispHwContain_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef __HAL_DISP_C__
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE u8 HalDispGetCmdqByCtx(void *pCtx, void **pCmdqCtx);
INTERFACE u8 HalDispGetUtilityIdByDevId(u32 u32DevId, s32 *ps32UtilityId);
INTERFACE u8 HalDispGetUtilityIdByDmaId(u32 u32DmaId, s32 *ps32UtilityId);
INTERFACE u32 HalDispGetDeviceId(void *pstCtx, u8 u8bDevContain);
INTERFACE u32 HalDispGetInterface(void *pstCtx, u8 u8bDevContain);
INTERFACE u16 HalDispGetDeviceTimeGenStart(void *pstCtx, u32 u32DevId);
INTERFACE u32 HalDispWaitDeviceVdeDone(void *pstCtx, u32 u32DevId);
#undef INTERFACE

#endif
