/*
* hal_hdmitx_if.h- Sigmastar
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

#ifndef _HAL_HDMITX_IF_H_
#define _HAL_HDMITX_IF_H_

#ifdef _HAL_HDMITX_IF_C_
#define INTERFACE

#else
#define INTERFACE extern
#endif

INTERFACE u8 HalHdmitxIfInit(void);
INTERFACE u8 HalHdmitxIfDeInit(void);
INTERFACE u8 HalHdmitxIfQuery(void *pCtx, void *pCfg);
INTERFACE void HalHdmitxUtilityW2BYTEMSK(u32 addr, u16 reg_data, u16 reg_mask);
#undef INTERFACE

#endif
