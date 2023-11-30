/*
 * drv_jpd_dev.h- Sigmastar
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

#ifndef __DRV_JPD_DEV_H__
#define __DRV_JPD_DEV_H__

#include <mhal_common.h>
#include "drv_jpd_st.h"

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//===================================================================================
void   ApiJpdAsmCpuSync(void);
void   ApiJPDSetISRStatus(int devid, MS_U32 status);
void   ApiJPDResetISRStatus(int devid);
int    ApiJPDGetISRStatus(int devid);
MS_S32 ApiJPDRst(JPD_Dev_t *pJpdDev);
#ifdef NJPD_CMDQ_SUPPORT
void ApiJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf);
void ApiJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen);
#endif

#endif //__DRV_JPD_DEV_H__