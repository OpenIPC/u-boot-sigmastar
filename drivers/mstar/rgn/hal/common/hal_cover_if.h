/*
* hal_cover_if.h- Sigmastar
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


#ifndef __HAL_COVER_IF_H__
#define __HAL_COVER_IF_H__
#include "hal_rgn_common.h"


//=============================================================================
// Defines
//=============================================================================

//=============================================================================
// struct
//=============================================================================
typedef enum
{
     E_HAL_QUERY_COV_OK = 0,
     E_HAL_QUERY_COV_CFGERR,
     E_HAL_QUERY_COV_SIZEERR,
     E_HAL_QUERY_COV_NOTSUPPORT,
     E_HAL_QUERY_COV_NOTBIND,
     E_HAL_QUERY_COV_NONEED,
}HalCovQueryRet_e;
 typedef enum
 {
     E_HAL_QUERY_COV_NULL,  // for future.
     E_HAL_QUERY_COV_MAX
 } HalCovQueryFunction_e;
typedef struct
{
    HalCovQueryFunction_e enQF;
    HalCoverIdType_e eCovId;
    void *pCfg;
    u32 u32CfgSize;
} HalCovQueryInputConfig_t;
typedef struct
{
    HalCovQueryRet_e enQueryRet;
    void (*pfForSet)(HalCoverIdType_e ,void *); // pfForSet(*pCtx,*pCfg)
    void *pPatch; // if need patch
    void (*pfForPatch)(HalCoverIdType_e ,void *); // pfForPatch(*pCtx,*pPatch)
} HalCovQueryOutputConfig_t;
typedef struct
{
    HalCovQueryInputConfig_t stInQ;
    HalCovQueryOutputConfig_t stOutQ;
} HalCovQueryFuncConfig_t;
typedef struct
{
    u16 u16X;
    u16 u16Y;
    u16 u16Width;
    u16 u16Height;
} HalCoverWindowType_t;

typedef struct
{
    u8 u8R;
    u8 u8G;
    u8 u8B;
} HalCoverColorType_t;

//=============================================================================

//=============================================================================
#ifndef __HAL_COVER_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE void HalCoverInit(void);
INTERFACE bool HalCoverQuery(void *pQF);
INTERFACE void HalCoverSetWindowSize(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverWindowType_t *ptWinCfg);
INTERFACE void HalCoverSetColor(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, HalCoverColorType_t *ptType);
INTERFACE void HalCoverSetEnableWin(HalCoverIdType_e eCoverId, HalCoverWinIdType_e eWinId, bool bEn);
INTERFACE bool HalCoverTransHalId(const char* p8Str, HalCoverIdType_e *pCoverId);

#undef INTERFACE
#endif /* __HAL_COVER_H__ */
