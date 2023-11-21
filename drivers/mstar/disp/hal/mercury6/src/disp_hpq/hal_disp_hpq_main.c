/*
* hal_disp_hpq_main.c- Sigmastar
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

#define MHAL_PQ_C


//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_disp_os_header.h"
#include "drv_disp_hpq_os.h"

#include "disp_debug.h"
#include "hal_disp_util.h"
#include "hal_disp_reg.h"

#include "hal_scl_pq_hw_reg.h"
#include "drv_scl_pq_define.h"

//[Daniel]

#include "drv_scl_pq_data_types.h"
#include "hal_scl_pq.h"
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------
#ifndef UNUSED //to avoid compile warnings...
#define UNUSED(var) (void)((var) = (var))
#endif

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
u32 PQ_RIU_BASE;

// Put this function here because hwreg_utility2 only for hal.
void Hal_PQ_init_riu_base(u32 u32riu_base)
{
    PQ_RIU_BASE = u32riu_base;
}

#if (PQ_ENABLE_UNUSED_FUNC)

u8 Hal_PQ_get_sync_flag(PQ_WIN ePQWin)
{
    return 0;
}


u8 Hal_PQ_get_input_vsync_value(PQ_WIN ePQWin)
{
    return (Hal_PQ_get_sync_flag(ePQWin) & 0x04) ? 1:0;
}

u8 Hal_PQ_get_output_vsync_value(PQ_WIN ePQWin)
{
    return (Hal_PQ_get_sync_flag(ePQWin) & 0x01) ? 1 : 0;
}

u8 Hal_PQ_get_input_vsync_polarity(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_memfmt_doublebuffer(PQ_WIN ePQWin, bool bEn)
{

}
#endif

void Hal_PQ_set_sourceidx(PQ_WIN ePQWin, u16 u16Idx)
{

}

#if (PQ_ENABLE_UNUSED_FUNC)

void Hal_PQ_set_mem_fmt(PQ_WIN ePQWin, u16 u16val, u16 u16Mask)
{

}

void Hal_PQ_set_mem_fmt_en(PQ_WIN ePQWin, u16 u16val, u16 u16Mask)
{

}

void Hal_PQ_set_420upSample(PQ_WIN ePQWin, u16 u16value)
{

}

void Hal_PQ_set_force_y_motion(PQ_WIN ePQWin, u16 u16value)
{

}

u8 Hal_PQ_get_force_y_motion(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_force_c_motion(PQ_WIN ePQWin, u16 u16value)
{

}

u8 Hal_PQ_get_force_c_motion(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_dipf_temporal(PQ_WIN ePQWin, u16 u16val)
{
}

u16 Hal_PQ_get_dipf_temporal(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_dipf_spatial(PQ_WIN ePQWin, u16 u16val)
{
}

u8 Hal_PQ_get_dipf_spatial(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_vsp_sram_filter(PQ_WIN ePQWin, u8 u8vale)
{

}

u8 Hal_PQ_get_vsp_sram_filter(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_dnr(PQ_WIN ePQWin, u8 u8val)
{

}

u8 Hal_PQ_get_dnr(PQ_WIN ePQWin)
{
    return 0;
}


void Hal_PQ_set_presnr(PQ_WIN ePQWin, u8 u8val)
{
}

u8 Hal_PQ_get_presnr(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_film(PQ_WIN ePQWin, u16 u16val)
{

}

u8 Hal_PQ_get_film(PQ_WIN ePQWin)
{
    return 0;
}
#endif

void Hal_PQ_set_yc_sram(u8 enID, u8 u8sramtype, void *pData)
{

#if 0
    u32 u32reg_41, u32reg_42, u32reg_43;
    u32 u32Addr;
    u8 u8Ramcode[10];
    u16 u16IdxBase = 0;
    u16 i, j, x;
    bool bC_SRAM;

#ifdef C3_SIM
    return;
#endif

    if(enID == FILTER_SRAM_SC2)
    {
        u32reg_41 = REG_SCL_HVSP1_41_L;
        u32reg_42 = REG_SCL_HVSP1_42_L;
        u32reg_43 = REG_SCL_HVSP1_43_L;
    }
    else if(enID == FILTER_SRAM_SC3)
    {
        u32reg_41 = REG_SCL_HVSP2_41_L;
        u32reg_42 = REG_SCL_HVSP2_43_L;
        u32reg_43 = REG_SCL_HVSP2_43_L;
    }
    else
    {
        u32reg_41 = REG_SCL_HVSP0_41_L;
        u32reg_42 = REG_SCL_HVSP0_42_L;
        u32reg_43 = REG_SCL_HVSP0_43_L;
    }

    bC_SRAM = u8sramtype >= SC_FILTER_C_SRAM1 ? TRUE : FALSE;

    if(u8sramtype == SC_FILTER_Y_SRAM1 || u8sramtype == SC_FILTER_C_SRAM1)
    {
        u16IdxBase = 0x00;
    }
    else if(u8sramtype == SC_FILTER_Y_SRAM2 || u8sramtype == SC_FILTER_C_SRAM2)
    {
        u16IdxBase = 0x40;
    }
    else if(u8sramtype == SC_FILTER_Y_SRAM3 || u8sramtype == SC_FILTER_C_SRAM3)
    {
        u16IdxBase = 0x80;
    }
    else
    {
        u16IdxBase = 0xC0;
    }

    u32Addr = (u32)pData;

    W2BYTEMSK(u32reg_41, bC_SRAM ? BIT1 : BIT0, BIT1|BIT0); // reg_cram_rw_en

    for(i=0; i<64; i++)
    {
        while(MApi_XC_R2BYTE(u32reg_41) & BIT8);
        j=i*5;

        MApi_XC_W2BYTEMSK(u32reg_42,(i|u16IdxBase), 0x00FF);
        for ( x=0;x<5;x++ )
        {
            u8Ramcode[x] = *((u8 *)(u32Addr + (j+x)));
        }

        MApi_XC_W2BYTEMSK(u32reg_43+0x00, (((u16)u8Ramcode[1])<<8|(u16)u8Ramcode[0]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32reg_43+0x02, (((u16)u8Ramcode[3])<<8|(u16)u8Ramcode[2]), 0xFFFF);
        MApi_XC_W2BYTEMSK(u32reg_43+0x04, ((u16)u8Ramcode[4]), 0x00FF);


        MApi_XC_W2BYTEMSK(u32reg_41, BIT8, BIT8);
    }
#endif
}

void Hal_PQ_set_sram_color_index_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData)
{
    //ToDo

}

void Hal_PQ_set_sram_color_gain_snr_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData)
{

}


void Hal_PQ_set_sram_color_gain_dnr_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData)
{

}

#define ICC_H_SIZE	32
#define ICC_V_SIZE	32
void Hal_PQ_set_sram_icc_crd_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData)
{

#if 0
    u16 i;
    u16 u16Ramcode_L,u16Ramcode_H;
    u16 u16Ramcode;
    u32 u32Addr;
    u32 u32BK18_78, u32BK18_79, u32BK18_7A;

    if(u8sramtype != SC_FILTER_SRAM_ICC_CRD)
    {
        return;
    }

    if(ePQWin == PQ_MAIN_WINDOW)
    {
        u32BK18_78 = REG_VIP_ACE2_78_L;
        u32BK18_79 = REG_VIP_ACE2_79_L;
        u32BK18_7A = REG_VIP_ACE2_7A_L;
    }
    else
    {
        return;
    }

    u32Addr = (u32)pData;

    MApi_XC_W2BYTEMSK(u32BK18_78, BIT(0), BIT(0)); // io_en disable

    for(i=0; i<(ICC_H_SIZE*ICC_V_SIZE); i++)
    {
        u16Ramcode_L = *((u8 *)(u32Addr + (2*i)));
        u16Ramcode_H = *((u8 *)(u32Addr + (2*i+1)));
        u16Ramcode = ((u16Ramcode_H & 0x01)<<8)+u16Ramcode_L;

        //while (MApi_XC_R2BYTE(u32BK18_7A) & BIT(15));
        MApi_XC_W2BYTEMSK(u32BK18_79, i, 0x03FF); // address
        MApi_XC_W2BYTEMSK(u32BK18_7A, (u16Ramcode & 0x1FF), 0x01FF); //data[8:0]
        MApi_XC_W2BYTEMSK(u32BK18_7A, BIT(15), BIT(15)); // io_w enable

    }

    MApi_XC_W2BYTEMSK(u32BK18_78, 0, BIT(0)); // io_en enable
#endif
}


#define IHC_H_SIZE	33
#define IHC_V_SIZE	33
#define BUF_BYTE_SIZE (IHC_H_SIZE*IHC_V_SIZE)
#define MAX_SRAM_SIZE 0x124

#define SRAM1_IHC_COUNT 289//81
#define SRAM2_IHC_COUNT 272//72
#define SRAM3_IHC_COUNT 272//72
#define SRAM4_IHC_COUNT 256//64

#define SRAM1_IHC_OFFSET 0
#define SRAM2_IHC_OFFSET SRAM1_IHC_COUNT
#define SRAM3_IHC_OFFSET (SRAM2_IHC_OFFSET + SRAM2_IHC_COUNT)
#define SRAM4_IHC_OFFSET (SRAM3_IHC_OFFSET + SRAM3_IHC_COUNT)

#define SRAM_IHC_TOTAL_COUNT SRAM1_IHC_COUNT + SRAM2_IHC_COUNT + SRAM3_IHC_COUNT + SRAM4_IHC_COUNT


void _Hal_PQ_set_sram_ihc_crd_table(PQ_WIN ePQWin, u16 *pBuf, u8 u8SRAM_Idx, u16 u16Cnt)
{

#if 0
    u16 i;
    u32 u32BK18_7C, u32BK18_7D, u32BK18_7E;

    if(u8SRAM_Idx > 3)
    {
        u8SRAM_Idx = 0;
    }

    if(ePQWin == PQ_MAIN_WINDOW)
    {
        u32BK18_7C = REG_VIP_ACE2_7C_L;
        u32BK18_7D = REG_VIP_ACE2_7D_L;
        u32BK18_7E = REG_VIP_ACE2_7E_L;
    }
    else
    {
        return;
    }


    MApi_XC_W2BYTEMSK(u32BK18_7C, BIT(0), BIT(0)); // io_en disable
    MApi_XC_W2BYTEMSK(u32BK18_7C, u8SRAM_Idx<<1, BIT(2)|BIT(1)); // sram select

    for(i=0; i<u16Cnt; i++)
    {
        MApi_XC_W2BYTEMSK(u32BK18_7D, i, 0x01FF); // address
        MApi_XC_W2BYTEMSK(u32BK18_7E, pBuf[i], 0x01FF); //data

        MApi_XC_W2BYTEMSK(u32BK18_7E, BIT(15), BIT(15)); // io_w enable
    }

    MApi_XC_W2BYTEMSK(u32BK18_7C, 0, BIT(0)); // io_en enable
#endif
}

u16 PQ_SRAM1_IHC[MAX_SRAM_SIZE];
u16 PQ_SRAM2_IHC[MAX_SRAM_SIZE];
u16 PQ_SRAM3_IHC[MAX_SRAM_SIZE];
u16 PQ_SRAM4_IHC[MAX_SRAM_SIZE];

void Hal_PQ_set_sram_ihc_crd_table(PQ_WIN ePQWin, u8 u8sramtype, void *pData)
{
    // New method for download IHC
    u16* u32Addr = (u16*)pData;
    u16 u16Index = 0 ;
    u16 cont1=0, cont2=0, cont3=0, cont4=0;

    if(u8sramtype != SC_FILTER_SRAM_IHC_CRD)
    {
        return;
    }

    for (;u16Index < SRAM_IHC_TOTAL_COUNT ; u16Index++)
    {
        if (u16Index < SRAM2_IHC_OFFSET)
        {
            PQ_SRAM1_IHC[cont1++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM3_IHC_OFFSET)
        {
            PQ_SRAM2_IHC[cont2++] = u32Addr[u16Index];
        }
        else if (u16Index < SRAM4_IHC_OFFSET)
        {
            PQ_SRAM3_IHC[cont3++] = u32Addr[u16Index];
        }
        else
        {
            PQ_SRAM4_IHC[cont4++] = u32Addr[u16Index];
        }
    }

    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &PQ_SRAM1_IHC[0], 0, SRAM1_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &PQ_SRAM2_IHC[0], 1, SRAM2_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &PQ_SRAM3_IHC[0], 2, SRAM3_IHC_COUNT);
    _Hal_PQ_set_sram_ihc_crd_table(ePQWin, &PQ_SRAM4_IHC[0], 3, SRAM4_IHC_COUNT);

}


#if (PQ_ENABLE_UNUSED_FUNC)
u16 Hal_PQ_get_420_cup_idx(MS_420_CUP_TYPE etype)
{
    return 0xFFFF;
}

u16 Hal_PQ_get_hnonlinear_idx(MS_HNONLINEAR_TYPE etype)
{
    return 0xFFFF;
}


u16 Hal_PQ_get_madi_idx(PQ_WIN ePQWin, MS_MADI_TYPE etype)
{
    return 0xFFFF;
}
#endif


u16 Hal_PQ_get_sram_size(u16 u16sramtype)
{
#if 0
    u16 u16ret = 0;

    switch(u16sramtype)
    {
        case SC_FILTER_Y_SRAM1:
            u16ret = PQ_IP_SRAM1_SIZE_Main;
            break;

        case SC_FILTER_Y_SRAM2:
            u16ret = PQ_IP_SRAM2_SIZE_Main;
            break;

        case SC_FILTER_Y_SRAM3:
            u16ret = PQ_IP_SRAM3_SIZE_Main;
            break;

        case SC_FILTER_Y_SRAM4:
            u16ret = PQ_IP_SRAM4_SIZE_Main;
            break;

        case SC_FILTER_C_SRAM1:
            u16ret = PQ_IP_C_SRAM1_SIZE_Main;
            break;

        case SC_FILTER_C_SRAM2:
            u16ret = PQ_IP_C_SRAM2_SIZE_Main;
            break;

        case SC_FILTER_C_SRAM3:
            u16ret = PQ_IP_C_SRAM3_SIZE_Main;
            break;

        case SC_FILTER_C_SRAM4:
            u16ret = PQ_IP_C_SRAM4_SIZE_Main;
            break;

        case SC_FILTER_SRAM_ICC_CRD:
            u16ret = PQ_IP_VIP_ICC_CRD_SRAM_SIZE_Main;
            break;

        case SC_FILTER_SRAM_IHC_CRD:
            u16ret = PQ_IP_VIP_IHC_CRD_SRAM_SIZE_Main;
            break;

        default:
            u16ret = 0;
            break;
    }

    return u16ret;
#endif
    u16 u16ret = 0;
    return u16ret;

}

#if (PQ_ENABLE_UNUSED_FUNC)

u16 Hal_PQ_get_ip_idx(MS_PQ_IP_TYPE eType)
{
    return 0xFFFF;
}


u16 Hal_PQ_get_hsd_sampling_idx(MS_HSD_SAMPLING_TYPE eType)
{
    return 0xFFFF;
}

void Hal_PQ_OPWriteOff_Enable(bool bEna)
{

}

u16 Hal_PQ_get_adc_sampling_idx(MS_ADC_SAMPLING_TYPE eType)
{
    return 0xFFFF;

}

u16 Hal_PQ_get_rw_method(PQ_WIN ePQWin)
{
    return 0;
}

void Hal_PQ_set_sram_xvycc_gamma_table(u8 u8sramtype, void *pData)
{
}

void Hal_PQ_set_rw_method(PQ_WIN ePQWin, u16 u16method)
{
}

bool HAL_PQ_user_mode_control(void)
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
// set equation to VIP CSC
// Both equation selection rules are
// 0: SDTV(601) R  G  B  : 16-235
// 1: SDTV(601) R  G  B  : 0-255
/// @param bMainWin       \b IN: Enable
//-------------------------------------------------------------------------------------------------
void HAL_PQ_set_SelectCSC(u16 u16selection, PQ_WIN ePQWin)
{

}

u8 Hal_PQ_get_madi_fbl_mode(bool bMemFmt422,bool bInterlace)
{
    return 0;
}

u16 Hal_PQ_get_csc_ip_idx(MS_CSC_IP_TYPE enCSC)
{
    return 0;
}

u16 Hal_PQ_get_422To444_idx(MS_422To444_TYPE etype)
{

    return 0;
}

u8 Hal_PQ_get_422To444_mode(bool bMemFmt422)
{
    return 0;
}
#endif
