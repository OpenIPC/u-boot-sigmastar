/*
* rgn_capdef.h- Sigmastar
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


#ifndef __RGNCAPDEF_H__
#define __RGNCAPDEF_H__

extern unsigned long geGopOsdFlag;
//=============================================================================
// Defines
//=============================================================================
#define FINAL_PALETTE_NUM 255
#define RGN_CHIP_GOP_GWIN_FMT_SUPPORT(fmt) (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB1555) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I2) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I4) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I8) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB4444) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB8888) ? 0 :0

#define RGN_CHIP_GOP_GWIN_OVERLAP_SUPPORT 1
#define RGN_CHIP_GOP_GWIN_XPOS_OVERLAP_SUPPORT 1
#define RGN_CHIP_GOP_PITCH_ALIGNMENT 16
#define RGN_CHIP_GOP_HW_GWIN_CNT 4
#define RGN_CHIP_DISP_HW_GWIN_CNT 0
#define RGN_CHIP_GOP_GWIN_WIDTH_ALIGNMENT(fmt) (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB1555) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I2) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I4) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_I8) ? 1 :\
                                      (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB4444) ? 1 :MHAL_FAILURE

#define RGN_CHIP_GOP_GWIN_XPOS_ALIGNMENT(fmt) (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB1555) ? 1 :\
                                          (fmt== E_MHAL_RGN_PIXEL_FORMAT_I2) ? 1 :\
                                          (fmt== E_MHAL_RGN_PIXEL_FORMAT_I4) ? 1 :\
                                          (fmt== E_MHAL_RGN_PIXEL_FORMAT_I8) ? 1 :\
                                          (fmt== E_MHAL_RGN_PIXEL_FORMAT_ARGB4444) ? 1 :MHAL_FAILURE
// Color inverse limitation
#define RGN_CHIP_OSD_DEFAULTFLAGBIND (E_MHAL_OSD_FLAG_PORT0|E_MHAL_OSD_FLAG_PORT1)
#define RGN_CHIP_GOP_ID_SHIFTBIT 4
#define RGN_CHIP_GOPID_DEFAULT (MS_U32)(E_DRV_GOP_ID_0<<E_MHAL_GOP_VPE_PORT0*RGN_CHIP_GOP_ID_SHIFTBIT|\
                                E_DRV_GOP_ID_1<<E_MHAL_GOP_VPE_PORT1*RGN_CHIP_GOP_ID_SHIFTBIT|\
                                E_DRV_GOP_UNBIND<<E_MHAL_GOP_VPE_PORT2*RGN_CHIP_GOP_ID_SHIFTBIT|\
                                E_DRV_GOP_UNBIND<<E_MHAL_GOP_VPE_PORT3*RGN_CHIP_GOP_ID_SHIFTBIT|\
                                E_DRV_GOP_ID_2<<E_MHAL_GOP_DIVP_PORT0*RGN_CHIP_GOP_ID_SHIFTBIT)
#define RGN_CHIP_GET_GOPID_FROM_BINDFLAG(osd,flag) (((osd== E_MHAL_GOP_VPE_PORT0)&&(flag&E_MHAL_OSD_FLAG_PORT0)) ? E_DRV_GOP_ID_0 :\
        ((osd== E_MHAL_GOP_VPE_PORT1)&&!(flag&E_MHAL_OSD_FLAG_PORT0)&&(flag&E_MHAL_OSD_FLAG_PORT1)) ? E_DRV_GOP_ID_0 :\
        ((osd== E_MHAL_GOP_VPE_PORT1)&&(flag&E_MHAL_OSD_FLAG_PORT1)) ? E_DRV_GOP_ID_1 :\
    ((osd== E_MHAL_GOP_VPE_PORT2)&&(!(flag&E_MHAL_OSD_FLAG_PORT0)||!(flag&E_MHAL_OSD_FLAG_PORT1))\
    &&(flag&E_MHAL_OSD_FLAG_PORT2)) ? E_DRV_GOP_ID_1 :\
    ((osd== E_MHAL_GOP_VPE_PORT2)&& (flag&E_MHAL_OSD_FLAG_PORT2)) ? E_DRV_GOP_ID_2 :\
  ((osd== E_MHAL_GOP_DIVP_PORT0)&&(flag&E_MHAL_OSD_FLAG_DIP) )? E_DRV_GOP_ID_2 :\
                                              E_DRV_GOP_UNBIND)

#endif
