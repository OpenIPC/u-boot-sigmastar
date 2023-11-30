/*
 * drv_jpd_parser.h- Sigmastar
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

#ifndef __DRV_JPD_PARSER_H__
#define __DRV_JPD_PARSER_H__

#include "drv_jpd_st.h"

int           JpegCalcBlock(JPD_Ctx_t *pCtx);
int           JpegGetAlignedImageInfo(JPD_Ctx_t *pCtx);
MS_S32        JpegDealHeaderInfo(JPD_Ctx_t *pCtx);
MS_S32        JpegThumbnailInit(JPD_Ctx_t *pCtx);
MS_S32        JpegInitParseAddr(JPD_Ctx_t *pCtx);
MS_BOOL       JpegLocateSOFMarker(JPD_Ctx_t *pCtx);
MS_BOOL       JpegLocateSOSMarker(JPD_Ctx_t *pCtx);
MS_BOOL       JpegInitProgressive(JPD_Ctx_t *pCtx);
void          JpegFreeAllBlocks(JPD_Ctx_t *pCtx);
unsigned long JpegGetECS(JPD_Ctx_t *pCtx);
#endif //__DRV_JPD_PARSER_H__