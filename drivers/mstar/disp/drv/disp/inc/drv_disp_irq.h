/*
* drv_disp_irq.h- Sigmastar
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

#ifndef _DRV_DISP_IRQ_H_
#define _DRV_DISP_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _DRV_DISP_IRQ_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
INTERFACE void DrvDispIrqSetIsrNum(u32 u32DevId, u32 u32IsrNum);
INTERFACE u8   DrvDispIrqGetIsrNum(void *pDevCtx, u32 *pu32IsrNum);
INTERFACE u8   DrvDispIrqGetIsrNumByDevId(u32 u32DevId, u32 *pu32IsrNum);
INTERFACE u8   DrvDispIrqEnable(void *pDevCtx, u32 u32DevIrq, u8 bEnable);
INTERFACE u8   DrvDispIrqGetFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);
INTERFACE u8   DrvDispIrqClear(void *pDevCtx, void *pData);
INTERFACE u8   DrvDispIrqGetLcdIsrNum(void *pDevCtx, u32 *pu32IsrNum);
INTERFACE u8   DrvDispIrqEnableLcd(void *pDevCtx, u32 u32DevIrq, u8 bEnable);
INTERFACE u8   DrvDispIrqGetLcdFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);
INTERFACE u8   DrvDispIrqClearLcd(void *pDevCtx, void *pData);

INTERFACE u8  DrvDispIrqCreateInternalIsr(void *pDispCtx);
INTERFACE u8  DrvDispIrqDestroyInternalIsr(void *pDispCtx);
INTERFACE u16 DrvDispIrqGetIrqCount(void);

#undef INTERFACE
#endif
