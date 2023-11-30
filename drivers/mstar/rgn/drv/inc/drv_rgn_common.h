/*
* drv_rgn_common.h- Sigmastar
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

#ifndef __DRV_RGN_COMMON_H__
#define __DRV_RGN_COMMON_H__
#include "hal_rgn_common.h"

//=============================================================================
// Defines
//=============================================================================
#ifndef bool
#define  bool   unsigned char
#endif

//=============================================================================
// enum
//=============================================================================
typedef enum
{
    E_DRV_RGN_VPE_CMDQ_ID_0,
    E_DRV_RGN_DIVP_CMDQ_ID_1,
    E_DRV_RGN_LDC_CMDQ_ID_2,
    E_DRV_RGN_DISP_CMDQ_ID_3,
    E_DRV_RGN_CMDQ_ID_NUM,
} DrvRgnCmdqIdType_e;
typedef enum
{
    E_DRV_RGN_INIT_COVER = 0x1,
    E_DRV_RGN_INIT_GOP = 0x2,
} DrvRgnInitType_e;

//=============================================================================
// struct
//=============================================================================
//=============================================================================

//=============================================================================
#ifndef __DRV_RGN_COMMON_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif
INTERFACE bool DrvRgnInit(void);
INTERFACE bool DrvRgnDeinit(void);
INTERFACE bool DrvRgnUpdateReg(void);
INTERFACE bool DrvRgnGetOsdBindGop(unsigned int *eGopNum);
INTERFACE bool DrvRgnSetOsdBindGop(unsigned int eGopNum);
INTERFACE bool DrvRgnSetCmdq(MHAL_CMDQ_CmdqInterface_t *pstCmdq,DrvRgnCmdqIdType_e eCmdqId);
INTERFACE void DrvRgnUseCmdq(bool bEn);

#undef INTERFACE
#endif //
/** @} */ // end of hvsp_group
