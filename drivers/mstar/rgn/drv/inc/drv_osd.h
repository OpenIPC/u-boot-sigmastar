/*
* drv_osd.h- Sigmastar
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


/**
* \ingroup osd_group
* @{
*/

#ifndef __DRV_OSD_H__
#define __DRV_OSD_H__
#include "drv_rgn_common.h"
#include "hal_osd.h"

//=============================================================================
// Defines
//=============================================================================
#define DRV_OSD_VERSION                        0x0300



//=============================================================================
// enum
//=============================================================================

typedef enum
{
    E_DRV_OSD_OUT_FMT_RGB,
    E_DRV_OSD_OUT_FMT_YUV,
    E_DRV_OSD_OUT_FMT_NUM,
} DrvOsdOutFmtType_e;
typedef enum
{
    E_DRV_OSD_PORT0 = 0,
    E_DRV_OSD_PORT1,
    E_DRV_OSD_PORT2,
    E_DRV_OSD_PORT3,
    E_DRV_OSD_DIP,
    E_DRV_OSD_LDC,
    E_DRV_OSD_PORT4,
    E_DRV_OSD_PORT5,
    E_DRV_OSD_DISP0_CUR,
    E_DRV_OSD_DISP0_UI,
    E_DRV_OSD_DISP1_CUR,
    E_DRV_OSD_DISP1_UI,
    E_DRV_OSD_ID_MAX
}DrvOsdId_e;

typedef enum
{
    E_DRV_OSD_VPE_CMDQ_ID_0,
    E_DRV_OSD_DIVP_CMDQ_ID_1,
    E_DRV_OSD_LDC_CMDQ_ID_1,
    E_DRV_OSD_CMDQ_ID_NUM,
} DrvOsdCmdqIdType_e;

typedef enum
{
    E_DRV_OSD_ERR_OK    =  0, ///< No Error
    E_DRV_OSD_ERR_FAULT = -1, ///< Fault
    E_DRV_OSD_ERR_INVAL = -2, ///< Invalid value
} DrvOsdErrType_e;

typedef enum
{
    E_DRV_OSD_AE0 = 0,
    E_DRV_OSD_AE1,
    E_DRV_OSD_AE2,
    E_DRV_OSD_AE3,
    E_DRV_OSD_AE4,
    E_DRV_OSD_AE5,
    E_DRV_OSD_AE6,
    E_DRV_OSD_AE7,
    E_DRV_OSD_AE_MAX
} DrvOsdColorInvWindowIdType_e;

//=============================================================================
// struct
//=============================================================================

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
} DrvOsdColorInvParamConfig_t;

typedef struct
{
    bool bOsdEn;
    bool bBypassEn;
    bool bColorInv;
    DrvOsdColorInvParamConfig_t stColorInvCfg[E_DRV_OSD_AE_MAX];
 } _DrvOsdLocalSettingsConfig_t;

//=============================================================================

//=============================================================================
#ifndef __DRV_OSD_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE bool DrvOsdSetEnable(DrvOsdId_e eGopId, bool bEn);
INTERFACE bool DrvOsdSetBindGop(u32 u32GopOsdFlag);
INTERFACE bool DrvOsdInit(void);
INTERFACE bool DrvOsdSetColorInverseEn(DrvOsdId_e eGopId, bool bEn);
INTERFACE bool DrvOsdSetColorInverseParam(DrvOsdId_e eGopId, DrvOsdColorInvWindowIdType_e eAeId, DrvOsdColorInvParamConfig_t *ptColInvCfg);
INTERFACE bool DrvOsdSetColorInverseUpdate(DrvOsdId_e eGopId);
INTERFACE bool DrvOsdWriteColorInverseData(DrvOsdId_e eGopId, u16 addr, u32 wdata);
INTERFACE bool DrvOsdReadColorInverseData(DrvOsdId_e eGopId, u16 addr, u32 *rdata);


#undef INTERFACE
#endif //
/** @} */ // end of hvsp_group
