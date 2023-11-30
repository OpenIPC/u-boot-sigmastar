/*
* hal_disp_dma_if.h- Sigmastar
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

#ifndef _HAL_DISP_DMA_IF_H_
#define _HAL_DISP_DMA_IF_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_DMA_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE u8   HalDispDmaIfInit(void *pCtx);
INTERFACE u8   HalDispDmaIfDeInit(void *pCtx);
INTERFACE void HalDispIrqSetDmaTriggerCount(u8 u8Reset, u32 u32Id);
INTERFACE void HalDispDmaIfWaitDone(void *pstDispCtxCfg);
INTERFACE void HalDispDmaIfRegFlip(void *pCtx);

#undef INTERFACE
#endif
