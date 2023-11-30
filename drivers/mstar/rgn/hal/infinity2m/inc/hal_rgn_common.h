/*
* hal_rgn_common.h- Sigmastar
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


#ifndef __HAL_RGN_COMMON_H__
#define __HAL_RGN_COMMON_H__
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "drv_rgn_os_header.h"
#include "rgn_debug.h"

extern unsigned long geGopOsdFlag;
//=============================================================================
// enum
//=============================================================================
typedef enum
{
    E_HAL_RGN_CMDQ_VPE_ID_0 = 0,
    E_HAL_RGN_CMDQ_DIVP_ID_1,
	E_HAL_RGN_CMDQ_LDC_ID_2,
    E_HAL_RGN_CMDQ_ID_NUM,
}HalRgnCmdqIdType_e;
typedef enum
{
    E_HAL_RGN_OSD_FLAG_PORT0 = 0x1,
    E_HAL_RGN_OSD_FLAG_PORT1 = 0x2,
    E_HAL_RGN_OSD_FLAG_PORT2 = 0x4,
    E_HAL_RGN_OSD_FLAG_PORT3 = 0x8,
    E_HAL_RGN_OSD_FLAG_DIP   = 0x10,
}HalRgnOsdFlag_e;

//=============================================================================
// struct
//=============================================================================
//=============================================================================
// Defines
//=============================================================================
#define HAL_RGN_GOP_NUM 2
#define HAL_RGN_GOP_GWIN_NUM 1
#define HAL_RGN_OSD_REAL_NUM 2
#define HAL_RGN_OSD_NUM 2
#define HAL_RGN_DISP_GWIN_CNT 0
#define RIU_32_EN 0

#ifndef bool
#define  bool   unsigned char
#endif

//=============================================================================

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
INTERFACE void HalRgnInit(void);
INTERFACE void HalRgnDeinit(void);

#undef INTERFACE
#endif /* __HAL_GOP_H__ */
