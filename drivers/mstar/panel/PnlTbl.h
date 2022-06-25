
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006-2011 MStar Semiconductor, Inc.
// All rights reserved.
//
// Unless otherwise stipulated in writing, any and all information contained
// herein regardless in any format shall remain the sole proprietary of
// MStar Semiconductor Inc. and be kept in strict confidence
// ("MStar Confidential Information") by the recipient.
// Any unauthorized act including without limitation unauthorized disclosure,
// copying, use, reproduction, sale, distribution, modification, disassembling,
// reverse engineering and compiling of the contents of MStar Confidential
// Information is unlawful and strictly prohibited. MStar hereby reserves the
// rights to any and all damages, losses, costs and expenses resulting therefrom.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __PNL_TBL_H__
#define __PNL_TBL_H__

//-------------------------------------------------------------------------------------------------
// Define & Macro
//-------------------------------------------------------------------------------------------------
#ifdef __PNL_TBL_C__
#define INTERFACE
#else
#define INTERFACE extern
#endif

typedef struct  {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
}LCM_setting_table;

#if 0
INTERFACE MhalPnlParamConfig_t  tPanel_InnovLux1280x800[];

INTERFACE MhalPnlParamConfig_t  tPanel_720x1280_60[];
INTERFACE MhalPnlMipiDsiConfig_t tPanel_RM68200_720x1280_4Lane_Sync_Pulse_RGB888;
INTERFACE MhalPnlMipiDsiConfig_t tPanel_HX8394_720x1280_2Lane_Sync_Pulse_RGB888;
INTERFACE MhalPnlParamConfig_t  stPanel_LX50FWB4001[];
INTERFACE unsigned short  _480x854setting[];
INTERFACE LCM_setting_table RM68172_V2_CmdTable[];
#endif
extern MhalPnlParamConfig_t  stPanel_LX50FWB4001[];
INTERFACE unsigned int table_size;

INTERFACE void Lcm_init(unsigned char *stLCMTable, u32 u32CmdCount);

#endif
