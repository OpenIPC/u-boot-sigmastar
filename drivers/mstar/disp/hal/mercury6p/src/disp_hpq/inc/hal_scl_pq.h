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

#define MEMFMT24BIT_YC10BIT_MOT 0xBB
#define MEMFMT24BIT_YC8BIT_MOT  0xAA
#define MEMFMT24BIT_YC10BIT     0x99
#define MEMFMT24BIT_YC8BIT      0x88
#define MEMFMT24BIT_CLEAR       0x00

#define SC_FILTER_Y_SRAM1             0x00
#define SC_FILTER_Y_SRAM2             0x01
#define SC_FILTER_Y_SRAM3             0x02
#define SC_FILTER_Y_SRAM4             0x03
#define SC_FILTER_C_SRAM1             0x04
#define SC_FILTER_C_SRAM2             0x05
#define SC_FILTER_C_SRAM3             0x06
#define SC_FILTER_C_SRAM4             0x07
#define SC_FILTER_SRAM_COLOR_INDEX    0x08
#define SC_FILTER_SRAM_COLOR_GAIN_SNR 0x09
#define SC_FILTER_SRAM_COLOR_GAIN_DNR 0x0A
#define SC_FILTER_SRAM_ICC_CRD        0x0B
#define SC_FILTER_SRAM_IHC_CRD        0x0C
#define SC_FILTER_SRAM_XVYCC_DE_GAMMA 0x0D
#define SC_FILTER_SRAM_XVYCC_GAMMA    0x0E

#define FILTER_SRAM_SC1 0x00
#define FILTER_SRAM_SC2 0x01
#define FILTER_SRAM_SC3 0x02

#define SUPPORT_SCMI_V2         TRUE
#define SUPPORT_DYNAMIC_MEM_FMT TRUE

void HalPQSetSramIhcCrdTable(u32 u32DevId, u8 u8sramtype, void *pData);
void HalPQSetSramIccCrdTable(u32 u32DevId, u8 u8sramtype, void *pData);

#endif
