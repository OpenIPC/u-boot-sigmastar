/*
* hal_osd_if.h- Sigmastar
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


#ifndef __HAL_OSD_IF_H__
#define __HAL_OSD_IF_H__
#include "hal_rgn_common.h"


typedef enum
{
     E_HAL_QUERY_OSD_OK = 0,
     E_HAL_QUERY_OSD_CFGERR,
     E_HAL_QUERY_OSD_SIZEERR,
     E_HAL_QUERY_OSD_NOTSUPPORT,
     E_HAL_QUERY_OSD_NOTBIND,
     E_HAL_QUERY_OSD_NONEED,
}HalOsdQueryRet_e;
 typedef enum
 {
     E_HAL_QUERY_OSD_CI_EN,
     E_HAL_QUERY_OSD_CI_PARAM_CHECK,
     E_HAL_QUERY_OSD_CI_PARAM,
     E_HAL_QUERY_OSD_CI_UPDATE,
     E_HAL_QUERY_OSD_CI_WRITEDATA,
     E_HAL_QUERY_OSD_CI_READDATA,
     E_HAL_QUERY_OSD_MAX
 } HalOsdQueryFunction_e;
typedef struct
{
    HalOsdQueryFunction_e enQF;
    HalOsdIdType_e eOsdId;
    void *pCfg;
    u32 u32CfgSize;
} HalOsdQueryInputConfig_t;
typedef struct
{
    HalOsdQueryRet_e enQueryRet;
    void (*pfForSet)(HalOsdIdType_e ,void *); // pfForSet(*pCtx,*pCfg)
    void *pPatch; // if need patch
    void (*pfForPatch)(HalOsdIdType_e ,void *); // pfForPatch(*pCtx,*pPatch)
} HalOsdQueryOutputConfig_t;
typedef struct
{
    HalOsdQueryInputConfig_t stInQ;
    HalOsdQueryOutputConfig_t stOutQ;
} HalOsdQueryFuncConfig_t;
typedef struct
{
    u16 addr;
    u32 wdata;
    u32 *rdata;
}HalOsdColorInvRWConfig_t;
typedef struct
{
    u16 u16Th1;
    u16 u16Th2;
    u16 u16Th3;
    u16 u16X;
    u16 u16Y;
    u16 u16W;
    u16 u16H;
    u16 u16BlkNumX;
    u16 u16BlkNumY;
    HalOsdColorInvWindowIdType_e enCiWin;
} HalOsdColorInvParamConfig_t;

//=============================================================================

//=============================================================================
#ifndef __HAL_OSD_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE void HalOsdInit(void);
INTERFACE bool HalOsdQuery(void *pQF);
INTERFACE bool HalOsdTransId(const char* p8Str, HalOsdIdType_e *pHalId);
INTERFACE bool HalOsdColInvParamCheck(HalOsdColorInvParamConfig_t *ptColInvCfg);
INTERFACE void HalOsdSetOsdBypassForScEnable(HalOsdIdType_e eGopId, bool bEn);
INTERFACE void HalOsdSetOsdEnable(HalOsdIdType_e eOsdId, bool bEn);
INTERFACE bool HalOsdSetBindGop(u32 u32GopOsdFlag);
INTERFACE void HalOsdSetColorInverseEn(HalOsdIdType_e eGopId, bool bEn);
INTERFACE void HalOsdSetColorInverseParam(HalOsdIdType_e eGopId, HalOsdColorInvWindowIdType_e eAeId, HalOsdColorInvParamConfig_t *ptColInvCfg);
INTERFACE void HalOsdSetColorInverseUpdate(HalOsdIdType_e eGopId);
INTERFACE void HalOsdWriteColorInverseData(HalOsdIdType_e eGopId, u16 addr, u32 wdata);
INTERFACE void HalOsdReadColorInverseData(HalOsdIdType_e eGopId, u16 addr, u32 *rdata);
#undef INTERFACE
#endif /* __HAL_GOP_H__ */
