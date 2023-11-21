/*
* PnlTbl.h- Sigmastar
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

struct spi_attr {
    unsigned char       edge_trigger_mode;
    unsigned int        clock;
    unsigned char       gpio_mode_OnOff;
    unsigned int        cs_gpio_num;
    unsigned int        clk_gpio_num;
    unsigned int        mosi_gpio_num;
    unsigned int        miso_gpio_num;
    unsigned int        rst_gpio_num;
    unsigned int        bl_gpio_num;
    unsigned int        cmd_mode;   //use variable len:8bits&9bits&16bits
    unsigned int        cmd_num;
};

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
INTERFACE void Lcm_init_Ini(unsigned short *stLCMTable, u32 u32CmdCount,struct spi_attr stSpiCfg);

#endif
