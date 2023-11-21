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


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef struct
{
    bool bTimingUpdate[HAL_DISP_DEVICE_MAX];
    bool bClkGpCtrl[HAL_DISP_DEVICE_MAX];
    void *pvCmdqCtx[HAL_DISP_UTILITY_CMDQ_NUM];
    void *pvClkGpCtrl[HAL_DISP_DEVICE_MAX];
}HalDispHwContain_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef __HAL_DISP_C__
#define INTERFACE
#else
#define INTERFACE extern
#endif

INTERFACE bool HalDispGetCmdqByCtx(void *pCtx, void **pCmdqCtx);

#undef INTERFACE

#endif
