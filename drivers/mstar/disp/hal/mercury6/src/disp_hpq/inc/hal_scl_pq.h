/*
* hal_scl_pq.h- Sigmastar
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

#ifndef MHAL_PQ_H
#define MHAL_PQ_H

#define MEMFMT24BIT_YC10BIT_MOT     0xBB
#define MEMFMT24BIT_YC8BIT_MOT      0xAA
#define MEMFMT24BIT_YC10BIT         0x99
#define MEMFMT24BIT_YC8BIT          0x88
#define MEMFMT24BIT_CLEAR           0x00

#define SC_FILTER_Y_SRAM1               0x00
#define SC_FILTER_Y_SRAM2               0x01
#define SC_FILTER_Y_SRAM3               0x02
#define SC_FILTER_Y_SRAM4               0x03
#define SC_FILTER_C_SRAM1               0x04
#define SC_FILTER_C_SRAM2               0x05
#define SC_FILTER_C_SRAM3               0x06
#define SC_FILTER_C_SRAM4               0x07
#define SC_FILTER_SRAM_COLOR_INDEX      0x08
#define SC_FILTER_SRAM_COLOR_GAIN_SNR   0x09
#define SC_FILTER_SRAM_COLOR_GAIN_DNR   0x0A
#define SC_FILTER_SRAM_ICC_CRD          0x0B
#define SC_FILTER_SRAM_IHC_CRD          0x0C
#define SC_FILTER_SRAM_XVYCC_DE_GAMMA   0x0D
#define SC_FILTER_SRAM_XVYCC_GAMMA      0x0E

#define FILTER_SRAM_SC1                 0x00
#define FILTER_SRAM_SC2                 0x01
#define FILTER_SRAM_SC3                 0x02



#define SUPPORT_SCMI_V2     TRUE
#define SUPPORT_DYNAMIC_MEM_FMT TRUE

void Hal_PQ_init_riu_base(u32 u32riu_base);
u8   Hal_PQ_get_sync_flag(PQ_WIN ePQWin);
u8   Hal_PQ_get_input_vsync_value(PQ_WIN ePQWin);
u8   Hal_PQ_get_output_vsync_value(PQ_WIN ePQWin);
u8   Hal_PQ_get_input_vsync_polarity(PQ_WIN ePQWin);
void Hal_PQ_set_memfmt_doublebuffer(PQ_WIN ePQWin, bool bEn);
void Hal_PQ_set_sourceidx(PQ_WIN ePQWin, u16 u16Idx);
void Hal_PQ_set_mem_fmt(PQ_WIN ePQWin, u16 u16val, u16 u16Mask);
void Hal_PQ_set_mem_fmt_en(PQ_WIN ePQWin, u16 u16val, u16 u16Mask);
void Hal_PQ_set_420upSample(PQ_WIN ePQWin, u16 u16value);
void Hal_PQ_set_force_y_motion(PQ_WIN ePQWin, u16 u16value);
u8   Hal_PQ_get_force_y_motion(PQ_WIN ePQWin);
void Hal_PQ_set_force_c_motion(PQ_WIN ePQWin, u16 u16value);
u8   Hal_PQ_get_force_c_motion(PQ_WIN ePQWin);
void Hal_PQ_set_dipf_temporal(PQ_WIN ePQWin, u16 u16val);
u16  Hal_PQ_get_dipf_temporal(PQ_WIN ePQWin);
void Hal_PQ_set_dipf_spatial(PQ_WIN ePQWin, u16 u16val);
u8   Hal_PQ_get_dipf_spatial(PQ_WIN ePQWin);
void Hal_PQ_set_vsp_sram_filter(PQ_WIN ePQWin, u8 u8vale);
u8   Hal_PQ_get_vsp_sram_filter(PQ_WIN ePQWin);
void Hal_PQ_set_dnr(PQ_WIN ePQWin, u8 u8val);
u8   Hal_PQ_get_dnr(PQ_WIN ePQWin);
void Hal_PQ_set_presnr(PQ_WIN ePQWin, u8 u8val);
u8   Hal_PQ_get_presnr(PQ_WIN ePQWin);
void Hal_PQ_set_film(PQ_WIN ePQWin, u16 u16val);
u8   Hal_PQ_get_film(PQ_WIN ePQWin);
void Hal_PQ_set_yc_sram(u8 enID, u8 u8sramtype, void *pData);
void Hal_PQ_set_sram_color_index_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData);
void Hal_PQ_set_sram_color_gain_snr_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData);
void Hal_PQ_set_sram_color_gain_dnr_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData);
u16  Hal_PQ_get_420_cup_idx(MS_420_CUP_TYPE etype);
u16  Hal_PQ_get_hnonlinear_idx(MS_HNONLINEAR_TYPE etype);
u16  Hal_PQ_get_422To444_idx(MS_422To444_TYPE etype);
u8   Hal_PQ_get_422To444_mode(bool bMemFmt422);
u8   Hal_PQ_get_madi_fbl_mode(bool bMemFmt422,bool bInterlace);
u16  Hal_PQ_get_csc_ip_idx(MS_CSC_IP_TYPE enCSC);
u8   Hal_PQ_get_madi_fbl_mode(bool bMemFmt422,bool bInterlace);
u16  Hal_PQ_get_csc_ip_idx(MS_CSC_IP_TYPE enCSC);
u16  Hal_PQ_get_madi_idx(PQ_WIN ePQWin, MS_MADI_TYPE etype);
u16  Hal_PQ_get_sram_size(u16 u16sramtype);
u16  Hal_PQ_get_ip_idx(MS_PQ_IP_TYPE eType);
u16  Hal_PQ_get_hsd_sampling_idx(MS_HSD_SAMPLING_TYPE eType);
void Hal_PQ_OPWriteOff_Enable(bool bEna);
u16  Hal_PQ_get_adc_sampling_idx(MS_ADC_SAMPLING_TYPE eType);
void Hal_PQ_set_sram_ihc_crd_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData);
void Hal_PQ_set_sram_icc_crd_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData);
u16  Hal_PQ_get_rw_method(PQ_WIN ePQWin);
void Hal_PQ_set_rw_method(PQ_WIN ePQWin, u16 u16method);
bool HAL_PQ_user_mode_control(void);
void HAL_PQ_set_SelectCSC(u16 u16selection, PQ_WIN ePQWin);

#endif
