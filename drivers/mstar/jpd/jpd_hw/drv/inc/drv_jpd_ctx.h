/*
 * drv_jpd_ctx.h- Sigmastar
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

#ifndef __DRV_JPD_CTX_H__
#define __DRV_JPD_CTX_H__

#include "drv_jpd_st.h"

#define MsOS_DelayTaskUs(x) udelay(x)
#define MsOS_DelayTask(x)   mdelay(x)

MS_S32     MdrvJpdCtxInitialize(JPD_Ctx_t *pCtx);
MS_S32     MdrvJpdCtxRegister(JPD_Dev_t *pDev, JPD_Ctx_t *pCtx);
MS_S32     MdrvJpdCtxUnregister(JPD_Dev_t *pDev, JPD_Ctx_t *pCtx);
JPD_Ctx_t *MdrvJpdCtxAlloc(JPD_Dev_t *pDev);

MS_S32            _ApiJpdWriteRegTrig(JPD_Ctx_t *pCtx);
void              _ApiTerminate(JPD_Ctx_t *pCtx, EN_JPEG_ERR_CODE status);
MS_S32            ApiJpdDefaultConfig(JPD_Ctx_t *pJpdCtx);
MS_S32            ApiDecodeHeaderFirstStage(JPD_Ctx_t *pCtx);
MS_S32            ApiDecodeHdrLastStage(JPD_Ctx_t *pCtx);
MS_S32            ApiJpdExit(JPD_Ctx_t *pCtx);
MS_S32            ApiJpdDealHeaderInfo(JPD_Ctx_t *pCtx);
EN_JPEG_RESULT    ApiJpdStartDecode(JPD_Ctx_t *pCtx);
MHAL_JPD_Status_e ApiJpdGetEventStatus(JPD_Ctx_t *pCtx);
#endif //__DRV_JPD_CTX_H__
