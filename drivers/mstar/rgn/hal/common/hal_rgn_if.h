/*
* hal_rgn_if.h- Sigmastar
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


#ifndef __HAL_RGN_COMMON_IF_H__
#define __HAL_RGN_COMMON_IF_H__
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "drv_rgn_os_header.h"
#include "rgn_debug.h"

extern unsigned long geGopOsdFlag;
//=============================================================================
#define RGN_DEVICE_SC        "VPE"
#define RGN_DEVICE_SC_PORT0  "PORT0"
#define RGN_DEVICE_SC_PORT1  "PORT1"
#define RGN_DEVICE_SC_PORT2  "PORT2"
#define RGN_DEVICE_SC_PORT3  "PORT3"
#define RGN_DEVICE_SC_PORT4  "PORT4"
#define RGN_DEVICE_SC_PORT5  "PORT5"
#define RGN_DEVICE_DIP       "DIVP"
#define RGN_DEVICE_LDC       "LDC"
#define RGN_DEVICE_DISP      "DISP"
#define RGN_DEVICE_DISP0_CUR "DISP0_CUR"
#define RGN_DEVICE_DISP0_UI  "DISP0_UI"
#define RGN_DEVICE_DISP1_CUR "DISP1_CUR"
#define RGN_DEVICE_DISP1_UI  "DISP1_UI"
//=============================================================================
typedef enum
{
     E_HAL_QUERY_RGN_OK = 0,
     E_HAL_QUERY_RGN_CFGERR,
     E_HAL_QUERY_RGN_SIZEERR,
     E_HAL_QUERY_RGN_NOTSUPPORT,
     E_HAL_QUERY_RGN_NOTBIND,
     E_HAL_QUERY_RGN_NONEED,
}HalRgnQueryRet_e;
 typedef enum
 {
     E_HAL_QUERY_RGN_NULL,  // for future.
     E_HAL_QUERY_RGN_MAX
 } HalRgnQueryFunction_e;
typedef struct
{
    HalRgnQueryFunction_e enQF;
    HalRgnOsdFlag_e eRgnId;
    void *pCfg;
    u32 u32CfgSize;
} HalRgnQueryInputConfig_t;
typedef struct
{
    HalRgnQueryRet_e enQueryRet;
    void (*pfForSet)(HalRgnOsdFlag_e ,void *); // pfForSet(*pCtx,*pCfg)
    void *pPatch; // if need patch
    void (*pfForPatch)(HalRgnOsdFlag_e ,void *); // pfForPatch(*pCtx,*pPatch)
} HalRgnQueryOutputConfig_t;
typedef struct
{
    HalRgnQueryInputConfig_t stInQ;
    HalRgnQueryOutputConfig_t stOutQ;
} HalRgnQueryFuncConfig_t;
//=============================================================================

#ifndef __HAL_RGN_COMMON_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE void HalRgnInitCmdq(void);
INTERFACE void HalRgnUseCmdq(bool bEn);
INTERFACE void HalRgnSetCmdq(MHAL_CMDQ_CmdqInterface_t *pstCmdq,HalRgnCmdqIdType_e eHalCmdqId);
INTERFACE void HalRgnWrite2Byte(u32 u32Reg, u16 u16Val,HalRgnCmdqIdType_e eCmdqId);
INTERFACE void HalRgnWrite2ByteMsk(u32 u32Reg, u16 u16Val, u16 u16Mask,HalRgnCmdqIdType_e eCmdqId);
INTERFACE u16  HalRgnRead2Byte(u32 u32Reg);
INTERFACE bool HalRgnCheckBindRation(HalRgnOsdFlag_e *enFlag);
INTERFACE void HalRgnUpdateReg(void);
INTERFACE void RgnSysfsInit(void);
INTERFACE void RgnSysfsDeInit(void);
INTERFACE void HalRgnInit(void);
INTERFACE void HalRgnDeinit(void);
INTERFACE bool HalRgnTransCmdqId(const char* p8Str, HalRgnCmdqIdType_e *pHalCmdqId);
INTERFACE bool HalRgnQuery(void *pQF);

#undef INTERFACE
#endif /* __HAL_GOP_H__ */
