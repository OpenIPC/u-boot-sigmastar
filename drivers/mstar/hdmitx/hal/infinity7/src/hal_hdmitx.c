/*
* hal_hdmitx.c- Sigmastar
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

#define HAL_HDMITX_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
// Common Definition
#include "drv_hdmitx_os.h"
#include "mhal_common.h"

// Internal Definition
#include "hal_hdmitx_chip.h"
#include "hal_hdmitx_enum.h"
#include "hal_hdmitx_st.h"
#include "regHDMITx.h"
#include "halHDMIUtilTx.h"
#include "hdmitx_debug.h"
// External Definition
//-------------------------------------------------------------------------------------------------
//  Driver Compiler Options
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Defines
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Structures
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Variables
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Variables
//-------------------------------------------------------------------------------------------------
HalHdmitxAudioFreqType_t g_stTxAudioFreqTbl[E_HAL_HDMITX_AUDIO_FREQ_MAX] = {
    {0x02, {0x01800, 0x06978}}, // No signal, set to 48 KHz
    {0x03, {0x01000, 0x06978}}, // 32
    {0x00, {0x01880, 0x07530}}, // 44
    {0x02, {0x01800, 0x06978}}, // 48
    {0x08, {0x03100, 0x07530}}, // 88
    {0x0a, {0x03000, 0x06978}}, // 96
    {0x0c, {0x06200, 0x07530}}, // 176
    {0x0e, {0x06000, 0x06978}}, // 192
};

//-------------------------------------------------------------------------------------------------
//  Debug Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
HalHdmitxVideoPolarity_e HalHdmitxMapVideoTimingToHPolarity(HalHdmitxTimingResType_e timing)
{
    HalHdmitxVideoPolarity_e enVideoPolarity;
    enVideoPolarity = (timing == E_HAL_HDMITX_RES_640X480P_60HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       (timing == E_HAL_HDMITX_RES_720X480P_60HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       (timing == E_HAL_HDMITX_RES_720X576P_50HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       E_HDMITX_VIDEO_POLARITY_HIGH;
    return enVideoPolarity;
}
HalHdmitxVideoPolarity_e HalHdmitxMapVideoTimingToVPolarity(HalHdmitxTimingResType_e timing)
{
    HalHdmitxVideoPolarity_e enVideoPolarity;
    enVideoPolarity = (timing == E_HAL_HDMITX_RES_640X480P_60HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       (timing == E_HAL_HDMITX_RES_720X480P_60HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       (timing == E_HAL_HDMITX_RES_720X576P_50HZ) ? E_HDMITX_VIDEO_POLARITY_LOW :
                       E_HDMITX_VIDEO_POLARITY_HIGH;
    return enVideoPolarity;
}
HalHdmitxVideoMode_e HalHdmitxMapVideoTimingToMode(HalHdmitxTimingResType_e timing)
{
    HalHdmitxVideoMode_e enVideoMode;
    enVideoMode = E_HDMITX_VIDEO_PROGRESSIVE_MODE;
    return enVideoMode;
}
HalHdmitxAviVic_e HalHdmitxMapVideoTimingToVIC(HalHdmitxTimingResType_e timing)
{
    switch (timing)
    {
        case E_HAL_HDMITX_RES_1280X720P_24HZ:
            return E_HDMITX_VIC_1280X720P_24_16_9;
        case E_HAL_HDMITX_RES_1280X720P_25HZ:
            return E_HDMITX_VIC_1280X720P_25_16_9;
        case E_HAL_HDMITX_RES_1280X720P_29D97HZ:
        case E_HAL_HDMITX_RES_1280X720P_30HZ:
            return E_HDMITX_VIC_1280X720P_30_16_9;
        case E_HAL_HDMITX_RES_1280X720P_50HZ:
            return E_HDMITX_VIC_1280X720P_50_16_9;
        case E_HAL_HDMITX_RES_1280X720P_60HZ:
        case E_HAL_HDMITX_RES_1280X720P_59D94HZ:
            return E_HDMITX_VIC_1280X720P_60_16_9;

        case E_HAL_HDMITX_RES_1920X1080P_23D98HZ:
        case E_HAL_HDMITX_RES_1920X1080P_24HZ:
            return E_HDMITX_VIC_1920X1080P_24_16_9;
        case E_HAL_HDMITX_RES_1920X1080P_25HZ:
            return E_HDMITX_VIC_1920X1080P_25_16_9;
        case E_HAL_HDMITX_RES_1920X1080P_30HZ:
        case E_HAL_HDMITX_RES_1920X1080P_29D97HZ:
            return E_HDMITX_VIC_1920X1080P_30_16_9;
        case E_HAL_HDMITX_RES_1920X1080P_50HZ:
            return E_HDMITX_VIC_1920X1080P_50_16_9;
        case E_HAL_HDMITX_RES_1920X1080P_60HZ:
        case E_HAL_HDMITX_RES_1920X1080P_59D94HZ:
            return E_HDMITX_VIC_1920X1080P_60_16_9;

        case E_HAL_HDMITX_RES_3840X2160P_24HZ:
            return E_HDMITX_VIC_3840X2160P_24_16_9;
        case E_HAL_HDMITX_RES_3840X2160P_25HZ:
            return E_HDMITX_VIC_3840X2160P_25_16_9;
        case E_HAL_HDMITX_RES_3840X2160P_29D97HZ:
        case E_HAL_HDMITX_RES_3840X2160P_30HZ:
            return E_HDMITX_VIC_3840X2160P_30_16_9;
        case E_HAL_HDMITX_RES_3840X2160P_50HZ:
            return E_HDMITX_VIC_3840X2160P_50_16_9;
        case E_HAL_HDMITX_RES_3840X2160P_60HZ:
            return E_HDMITX_VIC_3840X2160P_60_16_9;

        case E_HAL_HDMITX_RES_640X480P_60HZ:
            return E_HDMITX_VIC_640X480P_60_4_3;
        case E_HAL_HDMITX_RES_720X480P_60HZ:
            return E_HDMITX_VIC_720X480P_60_4_3;
        case E_HAL_HDMITX_RES_720X576P_50HZ:
            return E_HDMITX_VIC_720X576P_50_4_3;


        default:
            return E_HDMITX_VIC_NOT_AVAILABLE;
    }
}
static u8 _HalHdmitxInfoFrameCheckSum(u32 u32Dev, HalHdmitxInfoFrameType_e packet_type)
{
    u32 u32SumVal = 0;
    u8  i         = 0;

    switch (packet_type)
    {
        case E_HAL_HDMITX_INFOFRAM_TYPE_VS:
            u32SumVal += (HDMITX_VS_INFO_PKT_TYPE + HDMITX_VS_INFO_PKT_VER + HDMITX_VS_INFO_PKT_LEN);

            for (i = 0; i < HDMITX_VS_INFO_PKT_LEN; i++)
            {
                u32SumVal += HalHdmitxUtilityRBYTE(REG_VS_00_L + 1 + i);
            }
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AVI:
            u32SumVal += (HDMITX_AVI_INFO_PKT_TYPE + HDMITX_AVI_INFO_PKT_VER + HDMITX_AVI_INFO_PKT_LEN);
            for (i = 0; i < HDMITX_PKT_AVI_LEN; i++)
            {
                u32SumVal += HalHdmitxUtilityRBYTE(REG_AVI_00_L + 1 + i);
            }
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_SPD:
            u32SumVal += (HDMITX_SPD_INFO_PKT_TYPE + HDMITX_SPD_INFO_PKT_VER + HDMITX_SPD_INFO_PKT_LEN);
            for (i = 0; i < HDMITX_SPD_INFO_PKT_LEN; i++)
            {
                u32SumVal += HalHdmitxUtilityRBYTE(REG_SPD_00_L + 1 + i);
            }
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO:
            u32SumVal += (HDMITX_AUD_INFO_PKT_TYPE + HDMITX_AUD_INFO_PKT_VER + HDMITX_AUD_INFO_PKT_LEN);
            for (i = 0; i < HDMITX_PKT_ADO_LEN; i++)
            {
                u32SumVal += HalHdmitxUtilityRBYTE(REG_ADO_00_L + 1 + i);
            }
            break;

        default:
            break;
    }

    u32SumVal = 0x100 - (u32SumVal & 0xFF);

    return (u8)(u32SumVal);
}
void HalHdmitxDeviceOnOff(u8 bEnable, u32 u32Dev)
{
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PD_N, bEnable ? mask_of_hdmi_tx_reg_pd_n : 0, mask_of_hdmi_tx_reg_pd_n);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_HDMI_TX_SW_RST, bEnable ? 0 : mask_of_hdmi_tx_reg_hdmi_tx_sw_rst, mask_of_hdmi_tx_reg_hdmi_tx_sw_rst);
}

void HalHdmitxPowerOnOff(u8 bEnable, u32 u32Dev)
{
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PD_N, bEnable ? mask_of_hdmi_tx_reg_pd_n : 0, mask_of_hdmi_tx_reg_pd_n);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_LDO_CLKTREE, bEnable ? 0 : mask_of_hdmi_tx_atop_reg_pd_ldo_clktree,
                              mask_of_hdmi_tx_atop_reg_pd_ldo_clktree);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_HDMITXPLL, bEnable ? 0 : mask_of_hdmi_tx_atop_reg_pd_hdmitxpll,
                              mask_of_hdmi_tx_atop_reg_pd_hdmitxpll);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_DRV_BIASGEN_CH,
                              bEnable ? 0 : mask_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch,
                              mask_of_hdmi_tx_atop_reg_pd_drv_biasgen_ch);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_BIASGEN, bEnable ? 0 : mask_of_hdmi_tx_atop_reg_pd_biasgen,
                              mask_of_hdmi_tx_atop_reg_pd_biasgen);
}
void HalHdmitxIrqDisable(u32 u32Int, u32 u32Dev) {}

void HalHdmitxIrqEnable(u32 u32Int, u32 u32Dev) {}

void HalHdmitxIrqClear(u32 u32Int, u32 u32Dev) {}

u32 HalHdmitxIrqGetStatus(u32 u32Dev)
{
    return 0;
}
void HalHdmitxInitSeq(u32 u32Dev)
{
    // if the following reigster is 0, it means that no trim value in efuse.
    if ((HalHdmitxUtilityR2BYTE(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH0)) == 0x0000)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH0, 0x001e,
                                  mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch0);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH1, 0x1e00,
                                  mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch1);
    }
    if (HalHdmitxUtilityR2BYTE(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH2) == 0x0000)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH2, 0x001e,
                                  mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch2);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_DRV_IOUT_SEL_CH3, 0x1e00,
                                  mask_of_hdmi_tx_atop_reg_gcr_drv_iout_sel_ch3);
    }

    if ((HalHdmitxUtilityR2BYTE(HDMI_TX_ATOP_REG_GCR_RTERM50_TRIM) & mask_of_hdmi_tx_atop_reg_gcr_rterm50_trim)
        == 0x0000)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_RTERM50_TRIM, 0x0000, mask_of_hdmi_tx_atop_reg_gcr_rterm50_trim);
    }
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AV_MUTE_MOD, mask_of_hdmi_tx_reg_av_mute_mod,
                              mask_of_hdmi_tx_reg_av_mute_mod);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AV_MUTE_MTR, mask_of_hdmi_tx_reg_av_mute_mtr,
                              mask_of_hdmi_tx_reg_av_mute_mtr);
}
void HalHdmitxInitPadTop(u32 u32Dev) {}
#define HDMITX_HPD_CLOCK_MSK                                                         \
    (mask_of_hdmi_tx_atop_reg_reserved_18_0 | mask_of_hdmi_tx_atop_reg_reserved_18_1 \
     | mask_of_hdmi_tx_atop_reg_reserved_18_2 | mask_of_hdmi_tx_atop_reg_reserved_18_3)
HalHdmitxRxStatus_e HalHdmitxGetRXStatus(u8 u8bHpd, u32 u32Dev)
{
    u8                  u8dviclockS;
    u8                  u8hpdS = FALSE;
    HalHdmitxRxStatus_e state;

    // maybe useless,
    u8dviclockS = HalHdmitxUtilityR2BYTE(REG_HDMI_TX_ATOP_18_L) & (HDMITX_HPD_CLOCK_MSK) ? FALSE : TRUE;
#if !defined(CONFIG_MBOOT)
    u8hpdS = DrvHdmitxOsGetGpioValue(DrvHdmitxOsGetHpdGpinPin());
#else
    u8hpdS = 1;
#endif

    if (u8bHpd)
    {
        if ((u8dviclockS == FALSE) && (u8hpdS == FALSE))
        {
            state = E_HDMITX_DVICLOCK_L_HPD_L;
        }
        else if ((u8dviclockS == FALSE) && (u8hpdS == TRUE))
        {
            state = E_HDMITX_DVICLOCK_L_HPD_H;
        }
        else if ((u8dviclockS == TRUE) && (u8hpdS == FALSE))
        {
            state = E_HDMITX_DVICLOCK_H_HPD_L;
        }
        else
        {
            state = E_HDMITX_DVICLOCK_H_HPD_H;
        }
    }
    else
    {
        state = E_HDMITX_DVICLOCK_H_HPD_H;
    }
    return state;
}

u16 HalHdmitxGetTMDS(u32 u32Dev)
{
    return (HalHdmitxUtilityR2BYTE(HDMI_TX_REG_HDMI_CLK1TO40) &mask_of_hdmi_tx_reg_hdmi_clk1to40) ? 0 : 1;
}
void HalHdmitxSetTMDSOnOff(u32 u32Dev, u8 bRB_Swap, u8 bTMDS)
{
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_HDMI_CLK1TO40, bTMDS ? 0x00 : mask_of_hdmi_tx_reg_hdmi_clk1to40,
                              mask_of_hdmi_tx_reg_hdmi_clk1to40);
}
void HalHdmitxSetAnalogDrvCur(u32 u32Dev, HalHdmitxAnaloDrvCurConfig_t *pCfg)
{
    if (pCfg->u8DrvCurTap1Ch0 || pCfg->u8DrvCurTap1Ch1 || pCfg->u8DrvCurTap1Ch2 || pCfg->u8DrvCurTap1Ch3
        || pCfg->u8DrvCurTap2Ch0 || pCfg->u8DrvCurTap2Ch1 || pCfg->u8DrvCurTap2Ch2 || pCfg->u8DrvCurTap2Ch3
        || pCfg->u8DrvCurPdRtermCh || pCfg->u8DrvCurPdLdoPreDrvCh || pCfg->u8DrvCurPdLdoMuxCh)
    {
        HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "%s %d, DrvCur %hhx Tap1(%hhx %hhx %hhx %hhx) Tap2(%hhx %hhx %hhx %hhx), Pd(%hhx %hhx %hhx)",
                   __FUNCTION__, __LINE__, pCfg->u8DrvCurPdRtermCh, pCfg->u8DrvCurTap1Ch0, pCfg->u8DrvCurTap1Ch1,
                   pCfg->u8DrvCurTap1Ch2, pCfg->u8DrvCurTap1Ch3, pCfg->u8DrvCurTap2Ch0, pCfg->u8DrvCurTap1Ch1,
                   pCfg->u8DrvCurTap2Ch2, pCfg->u8DrvCurTap2Ch3, pCfg->u8DrvCurPdRtermCh, pCfg->u8DrvCurPdLdoPreDrvCh,
                   pCfg->u8DrvCurPdLdoMuxCh);

        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH0,
                                  (u16)pCfg->u8DrvCurTap1Ch0 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0,
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch0);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH1,
                                  ((u16)pCfg->u8DrvCurTap1Ch1 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1),
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch1);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH2,
                                  (u16)pCfg->u8DrvCurTap1Ch2 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2,
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch2);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP1_CH3,
                                  ((u16)pCfg->u8DrvCurTap1Ch3 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3),
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap1_ch3);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH0,
                                  (u16)pCfg->u8DrvCurTap2Ch0 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0,
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch0);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH1,
                                  ((u16)pCfg->u8DrvCurTap2Ch1 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1),
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch1);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH2,
                                  (u16)pCfg->u8DrvCurTap2Ch2 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2,
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch2);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_GCR_ICTRL_SEL_TAP2_CH3,
                                  ((u16)pCfg->u8DrvCurTap2Ch3 << shift_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3),
                                  mask_of_hdmi_tx_atop_reg_gcr_ictrl_sel_tap2_ch3);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_RTERM_CH, ((u16)pCfg->u8DrvCurPdRtermCh),
                                  mask_of_hdmi_tx_atop_reg_pd_rterm_ch);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_LDO_PREDRV_CH,
                                  ((u16)pCfg->u8DrvCurPdLdoPreDrvCh << shift_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch),
                                  mask_of_hdmi_tx_atop_reg_pd_ldo_predrv_ch);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_ATOP_REG_PD_LDO_MUX_CH,
                                  ((u16)pCfg->u8DrvCurPdLdoMuxCh << shift_of_hdmi_tx_atop_reg_pd_ldo_mux_ch),
                                  mask_of_hdmi_tx_atop_reg_pd_ldo_mux_ch);
    }
}

void HalHdmitxSetVideoOutputMode(u32 u32Dev, HalHdmitxTimingResType_e Timing, HalHdmitxAnaloDrvCurConfig_t *pInfo)
{
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_VSYNC_OUT_POL,
                              HalHdmitxMapVideoTimingToVPolarity(Timing) == E_HDMITX_VIDEO_POLARITY_LOW
                                  ? mask_of_hdmi_tx_reg_vsync_out_pol
                                  : 0,
                              mask_of_hdmi_tx_reg_vsync_out_pol);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_HSYNC_OUT_POL,
                              HalHdmitxMapVideoTimingToHPolarity(Timing) == E_HDMITX_VIDEO_POLARITY_LOW
                                  ? mask_of_hdmi_tx_reg_hsync_out_pol
                                  : 0,
                              mask_of_hdmi_tx_reg_hsync_out_pol);
    HalHdmitxSetAnalogDrvCur(u32Dev, pInfo);
}
static void _HalHdmitxSendVSInfoFrame(u32 u32Dev, HalHdmitxPacketParameter_t *pstPkCfg,
                                      HalHdmitxVSInfoPktPara_t *pstInfoPara)
{
    u8 u8ChkSum;

    if (pstPkCfg->Define_Process == E_HDMITX_STOP_PACKET)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT1_EN, 0, mask_of_hdmi_tx_reg_pkt1_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT1_RDY_SEL, 0, mask_of_hdmi_tx_reg_pkt1_rdy_sel);
    }
    else
    {
        if (pstPkCfg->User_Define != TRUE)
        {
            // fill IEEE HDMI tag
            HalHdmitxUtilityW2BYTEMSK(VS_HD_INFOFRAME_TYPE_CODE,
                                      ((HDMITX_VS_INFO_PKT_TYPE << shift_of_vs_hd_infoframe_type_code)),
                                      mask_of_vs_hd_infoframe_type_code);
            HalHdmitxUtilityW2BYTEMSK(VS_HD_INFOFRAME_VERSION_NUMBER,
                                      (HDMITX_PKT_VS_HD_VERSION_VAL << shift_of_vs_hd_infoframe_version_number),
                                      mask_of_vs_hd_infoframe_version_number);
            HalHdmitxUtilityW2BYTEMSK(VS_HD_LENGTH_OF_VS, HDMITX_PKT_VS_LEN << shift_of_vs_hd_length_of_VS,
                                      mask_of_vs_hd_length_of_VS);

            HalHdmitxUtilityW2BYTEMSK(VS_IEEE_REG_ID_0, 0x03 << shift_of_vs_IEEE_reg_id_0, mask_of_vs_IEEE_reg_id_0);
            HalHdmitxUtilityW2BYTEMSK(VS_IEEE_REG_ID_1, 0x0c, mask_of_vs_IEEE_reg_id_1);
            HalHdmitxUtilityW2BYTEMSK(VS_HDMI_VIDEO_FORMAT, pstInfoPara->enVSFmt << shift_of_vs_HDMI_video_format,
                                      mask_of_vs_HDMI_video_format);
            HalHdmitxUtilityW2BYTEMSK(VS_3D_EXT_DATA, (pstInfoPara->en3DStruct) << shift_of_vs_3d_ext_data,
                                      mask_of_vs_3d_ext_data);
            HalHdmitxUtilityW2BYTEMSK(VS_HDMI_VIC, (pstInfoPara->en4k2kVIC) << shift_of_vs_HDMI_VIC,
                                      mask_of_vs_HDMI_VIC);
        }
        u8ChkSum = _HalHdmitxInfoFrameCheckSum(u32Dev, E_HAL_HDMITX_INFOFRAM_TYPE_VS);
        HalHdmitxUtilityW2BYTEMSK(VS_CHECKSUM, u8ChkSum, mask_of_vs_checksum);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT1_EN, mask_of_hdmi_tx_reg_pkt1_en, mask_of_hdmi_tx_reg_pkt1_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT1_RDY_SEL, mask_of_hdmi_tx_reg_pkt1_rdy_sel,
                                  mask_of_hdmi_tx_reg_pkt1_rdy_sel);
    }
}
static void _HalHdmitxSendAVIInfoFrame(u32 u32Dev, HalHdmitxPacketParameter_t *pstPkCfg,
                                       HalHdmitxAVIInfoPktPara_t *pstInfoPara)
{
    u8 u8ChkSum;
    if (pstPkCfg->Define_Process == E_HDMITX_STOP_PACKET)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AVI_EN, 0, mask_of_hdmi_tx_reg_avi_en);
    }
    else
    {
        if (pstPkCfg->User_Define != TRUE)
        {
            u16 u16PktVal = 0;

            HalHdmitxUtilityW2BYTEMSK(AVI_SCAN_INFO, (pstInfoPara->enScanInfo) << shift_of_avi_scan_info,
                                      mask_of_avi_scan_info);
            HalHdmitxUtilityW2BYTEMSK(AVI_BAR_INFO, 0, mask_of_avi_bar_info);
            HalHdmitxUtilityW2BYTEMSK(AVI_ACTIVE_INFO, (pstInfoPara->A0Value == 0x01) ? mask_of_avi_active_info : 0,
                                      mask_of_avi_active_info);
            HalHdmitxUtilityW2BYTEMSK(AVI_COLOR_INDICATOR, pstInfoPara->enColorFmt << shift_of_avi_color_indicator,
                                      mask_of_avi_color_indicator);

            // C1, C0, M1, M0
            HalHdmitxUtilityW2BYTEMSK(AVI_PICTURE_ASPECT_RATIO,
                                      pstInfoPara->enAspectRatio << shift_of_avi_picture_aspect_ratio,
                                      mask_of_avi_picture_aspect_ratio);

            // R3, R2, R1, R0: active porting aspect ration
            if (pstInfoPara->enableAFDoverWrite == TRUE)
            {
                u16PktVal = (pstInfoPara->enAFDRatio); // R
            }
            else
            {
                u16PktVal = E_HDMITX_VIDEO_AFD_SAMEASPICTUREAR;
            }
            HalHdmitxUtilityW2BYTEMSK(AVI_ACTIVE_FORMAT_ASPECT_RATIO,
                                      u16PktVal << shift_of_avi_active_format_aspect_ratio,
                                      mask_of_avi_active_format_aspect_ratio);
            HalHdmitxUtilityW2BYTEMSK(AVI_COLORIMETRY, pstInfoPara->enColorimetry << shift_of_avi_colorimetry,
                                      mask_of_avi_colorimetry);

            // EC0, EC1, EC2
            u16PktVal = (u8)pstInfoPara->enExtColorimetry;
            u16PktVal = (u16PktVal > 6) ? 6 : u16PktVal; // BT2020 RGB & BT2020 YCbCr share same value 6; 7 is reserved;
            HalHdmitxUtilityW2BYTEMSK(AVI_EXT_COLORIMETRY, u16PktVal << shift_of_avi_ext_colorimetry,
                                      mask_of_avi_ext_colorimetry);
            u16PktVal = 0;
            if (pstInfoPara->enColorFmt == E_HAL_HDMITX_COLOR_RGB444)
            {
                // Q1, Q0
                if (pstInfoPara->enRGBQuantRange == E_HDMITX_RGB_QUANT_LIMIT)
                {
                    u16PktVal = 1;
                }
                else if (pstInfoPara->enRGBQuantRange == E_HDMITX_RGB_QUANT_FULL)
                {
                    u16PktVal = 2;
                }
            }
            HalHdmitxUtilityW2BYTEMSK(AVI_QUANTIZATION_RANGE, u16PktVal << shift_of_avi_quantization_range,
                                      mask_of_avi_quantization_range);
            u16PktVal = 0;
            if (pstInfoPara->enColorFmt != E_HAL_HDMITX_COLOR_RGB444)
            {
                // YQ1, YQ0
                u16PktVal = pstInfoPara->enYCCQuantRange;
            }
            HalHdmitxUtilityW2BYTEMSK(AVI_YCC, u16PktVal << shift_of_avi_YCC, mask_of_avi_YCC);

            // VIC code: VIC code shoud +1 if aspect ration is 16:9
            u16PktVal = (u16)HalHdmitxMapVideoTimingToVIC(pstInfoPara->enVidTiming);
            if (((pstInfoPara->enVidTiming >= E_HAL_HDMITX_RES_720X480P_60HZ)
                 && (pstInfoPara->enVidTiming <= E_HAL_HDMITX_RES_720X576P_50HZ))
                && (pstInfoPara->enAspectRatio == E_HDMITX_VIDEO_AR_16_9))
            {
                u16PktVal += 1;
            }
            else if (pstInfoPara->enAspectRatio == E_HDMITX_VIDEO_AR_21_9)
            {
                u8 u8Ar21to9MappingTbl[14][2] = {{60, 65}, {61, 66}, {62, 67}, {19, 68}, {4, 69},  {41, 70}, {47, 71},
                                                 {32, 72}, {33, 73}, {34, 74}, {31, 75}, {16, 76}, {64, 77}, {63, 78}};

                if ((u16PktVal >= E_HDMITX_VIC_3840X2160P_24_16_9) && (u16PktVal <= E_HDMITX_VIC_3840X2160P_60_16_9)) // 3840*2160p@24 ~ 3840*2160@60
                {
                    u16PktVal += 10;
                }
                else if ((u16PktVal > 78) && (u16PktVal <= 92))
                {
                    // do nothing;
                }
                else
                {
                    u8 i         = 0;
                    u8 bValidVIC = FALSE;

                    for (i = 0; i < 14; i++)
                    {
                        if (u8Ar21to9MappingTbl[i][0] == u16PktVal)
                        {
                            u16PktVal = u8Ar21to9MappingTbl[i][1];
                            bValidVIC = TRUE;
                            break;
                        }
                    }

                    if (!bValidVIC)
                    {
                        HDMITX_ERR("%s :: Invalid VIC Code for 21:9 Aspect Ratio!!!\r\n", __FUNCTION__);
                    }
                }
            }
            HalHdmitxUtilityW2BYTEMSK(AVI_VIC, u16PktVal << shift_of_avi_VIC, mask_of_avi_VIC);

            // check repetition
            if ((HalHdmitxMapVideoTimingToMode(pstInfoPara->enVidTiming) == E_HDMITX_VIDEO_INTERLACE_MODE))
            {
                u16PktVal = 1;
            }
            else
            {
                u16PktVal = 0;
            }
            HalHdmitxUtilityW2BYTEMSK(AVI_PIXEL_REPETITION, u16PktVal << shift_of_avi_pixel_repetition,
                                      mask_of_avi_pixel_repetition);
        }
        u8ChkSum = _HalHdmitxInfoFrameCheckSum(u32Dev, E_HAL_HDMITX_INFOFRAM_TYPE_AVI);
        HalHdmitxUtilityW2BYTEMSK(AVI_CHECKSUM, u8ChkSum, mask_of_avi_checksum);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AVI_EN, mask_of_hdmi_tx_reg_avi_en, mask_of_hdmi_tx_reg_avi_en);
    }
}
static void _HalHdmitxSendSPDInfoFrame(u32 u32Dev, HalHdmitxPacketParameter_t *pstPkCfg)
{
    u8 u8ChkSum;
    u32 u32off = 0;
    u8 u8HdmitxSPDData[HDMITX_PKT_SPD_LEN] = {
        // 0~7 Vendor Name, 8~24 Producet name
        "SIGMA   HDMITX TX DEMO   ",
    };

    if (pstPkCfg->Define_Process == E_HDMITX_STOP_PACKET)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, 0, mask_of_hdmi_tx_reg_pkt0_en);
    }
    else
    {
        if (pstPkCfg->User_Define != TRUE)
        {
            u8 i = 0;
            HalHdmitxUtilityW2BYTEMSK(SPD_HD_INFOFRAME_TYPE_CODE, HDMITX_SPD_INFO_PKT_TYPE,
                                      mask_of_spd_hd_infoframe_type_code);
            HalHdmitxUtilityW2BYTEMSK(SPD_HD_INFOFRAME_VERSION_NUMBER,
                                      0x0001 << shift_of_spd_hd_infoframe_version_number,
                                      mask_of_spd_hd_infoframe_version_number);
            HalHdmitxUtilityW2BYTEMSK(SPD_HD_LENGTH_OF_SPD, HDMITX_SPD_INFO_PKT_LEN, mask_of_spd_hd_length_of_SPD);

            for (i = 0; i < HDMITX_SPD_INFO_PKT_LEN; i++)
            {
                if (i == HDMITX_SPD_INFO_PKT_LEN - 1)
                {
                    HalHdmitxUtilityW2BYTEMSK(SPD_SOURCE_INFO, HDMITX_PACKET_SPD_SDI << shift_of_spd_source_info,
                                              mask_of_spd_source_info);
                }
                else
                {
                    if(i == 0 || i==6 || i ==13 || i == 20)
                    {
                        u32off++;
                    }
                    HalHdmitxUtilityWBYTEMSK(REG_SPD_00_L   + i + u32off, u8HdmitxSPDData[i], 0xFF);
                    
                }
            }
        }
        u8ChkSum = _HalHdmitxInfoFrameCheckSum(u32Dev, E_HAL_HDMITX_INFOFRAM_TYPE_SPD);
        HalHdmitxUtilityW2BYTEMSK(SPD_CHECKSUM, u8ChkSum, mask_of_spd_checksum);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, mask_of_hdmi_tx_reg_pkt0_en, mask_of_hdmi_tx_reg_pkt0_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_RDY_SEL, mask_of_hdmi_tx_reg_pkt0_rdy_sel,
                                  mask_of_hdmi_tx_reg_pkt0_rdy_sel);
    }
}

static void _HalHdmitxSendAUDInfoFrame(u32 u32Dev, HalHdmitxPacketParameter_t *pstPkCfg,
                                       HalHdmitxAUDInfoPktPara_t *pstInfoPara)
{
    u8 u8ChkSum;

    if (pstPkCfg->Define_Process == E_HDMITX_STOP_PACKET)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, 0, mask_of_hdmi_tx_reg_audio_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, 0, mask_of_hdmi_tx_reg_ado_en);
    }
    else
    {
        // Modified for HDMI CTS test -
        //     - Audio Coding Type (CT3~CT0) is 0x0 then continue else then FAIL
        //     - Sampling Frequency (SF2~ SF0) is zero then continue else then FAIL.
        //     - Sample Size (SS1~ SS0) is zero then continue else then FAIL.
        // tmp_value = (gHDMITxInfo.output_audio_frequncy << 10) | 0x11;  // audio sampling frequency, PCM and 2
        // channel.

        if (pstInfoPara->enAudChCnt == E_HAL_HDMITX_AUDIO_CH_2) // 2-channel
        {
            HalHdmitxUtilityW2BYTEMSK(ADO_CHANNEL_COUNT, (HDMITX_AUDIO_CH_2_CNT - 1) << shift_of_ado_channel_count,
                                      mask_of_ado_channel_count);
            HalHdmitxUtilityW2BYTEMSK(ADO_CHANNEL_ALLOCATION, 0x0, mask_of_ado_channel_allocation);
        }
        else // 8- channel
        {
            HalHdmitxUtilityW2BYTEMSK(ADO_CHANNEL_COUNT, (HDMITX_AUDIO_CH_8_CNT - 1) << shift_of_ado_channel_count,
                                      mask_of_ado_channel_count);
            HalHdmitxUtilityW2BYTEMSK(ADO_CHANNEL_ALLOCATION, 0x1F, mask_of_ado_channel_allocation);
        }
        HalHdmitxUtilityW2BYTEMSK(ADO_PK_2, 0, mask_of_ado_pk_2);
        HalHdmitxUtilityW2BYTEMSK(ADO_PK_3, 0, mask_of_ado_pk_3);
        // clear LFEP value
        HalHdmitxUtilityW2BYTEMSK(ADO_LFEP, 0 << shift_of_ado_LFEP, mask_of_ado_LFEP);

        u8ChkSum = _HalHdmitxInfoFrameCheckSum(u32Dev, E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO);
        HalHdmitxUtilityW2BYTEMSK(ADO_CHECKSUM, u8ChkSum, mask_of_ado_checksum);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, mask_of_hdmi_tx_reg_audio_en, mask_of_hdmi_tx_reg_audio_en);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PKT0_EN, mask_of_hdmi_tx_reg_ado_en, mask_of_hdmi_tx_reg_ado_en);
    }

    if (pstPkCfg->User_Define == FALSE)
    {
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_BPCM,
                                  ((pstInfoPara->enAudType == E_HAL_HDMITX_AUDIO_CODING_PCM) ? 0 : mask_of_asp_cs_bPCM),
                                  mask_of_asp_cs_bPCM);
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_PK_2, 0x00, mask_of_asp_cs_pk_2);
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_CHANNEL_COUNT, (pstInfoPara->enAudChCnt << shift_of_asp_cs_channel_count),
                                  mask_of_asp_cs_channel_count);
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_SAMPLE_FREQ, (g_stTxAudioFreqTbl[pstInfoPara->enAudFreq].CH_Status3),
                                  mask_of_asp_cs_sample_freq);
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_PK_5, 0x00, mask_of_asp_cs_pk_5);
        HalHdmitxUtilityW2BYTEMSK(ASP_CS_PK_6, 0x00, mask_of_asp_cs_pk_6);
    }
    // Audio sampling frequency
    // 1           1        0        0        32 kHz
    // 0           0        0        0        44.1 kHz
    // 0           0        0        1        88.2 kHz
    // 0           0        1        1        176.4 kHz
    // 0           1        0        0        48 kHz
    // 0           1        0        1        96 kHz
    // 0           1        1        1        192 kHz
    // 1           0        0        1        768 kHz
}

void HalHdmitxSendInfoFrame(HalHdmitxResourcePrivate_t *psHDMITXResPri, HalHdmitxInfoFrameType_e enPktType,
                            HalHdmitxPacketProcess_e packet_process)
{
    psHDMITXResPri->stInfoFrameCfg[enPktType].Define_Process = packet_process;

    switch (enPktType)
    {
        case E_HAL_HDMITX_INFOFRAM_TYPE_VS:
            _HalHdmitxSendVSInfoFrame(psHDMITXResPri->u32DeviceID, &(psHDMITXResPri->stInfoFrameCfg[enPktType]),
                                      &(psHDMITXResPri->stVSInfoPara));
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AVI:
            _HalHdmitxSendAVIInfoFrame(psHDMITXResPri->u32DeviceID, &(psHDMITXResPri->stInfoFrameCfg[enPktType]),
                                       &(psHDMITXResPri->stAVIInfoPara));
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_SPD:
            _HalHdmitxSendSPDInfoFrame(psHDMITXResPri->u32DeviceID, &(psHDMITXResPri->stInfoFrameCfg[enPktType]));
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO:
            _HalHdmitxSendAUDInfoFrame(psHDMITXResPri->u32DeviceID, &(psHDMITXResPri->stInfoFrameCfg[enPktType]),
                                       &(psHDMITXResPri->stAUDInfoPara));
            break;
        default:
            HDMITX_ERR("%s:: Invalid Packet Type!!\r\n", __FUNCTION__);
            break;
    }
}
void HalHdmitxSendPacket(HalHdmitxResourcePrivate_t *psHDMITXResPri, HalHdmitxPacketType_e enPktType,
                         HalHdmitxPacketProcess_e packet_process)
{
    psHDMITXResPri->stPacketCfg[enPktType].Define_Process = packet_process;

    switch (enPktType)
    {
        case E_HAL_HDMITX_PACKET_TYPE_NULL:
            HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_NUL_EN,
                                      (packet_process == E_HDMITX_STOP_PACKET) ? 0 : mask_of_hdmi_tx_reg_nul_en,
                                      mask_of_hdmi_tx_reg_nul_en);
            break;

        case E_HAL_HDMITX_PACKET_TYPE_ACR:
            HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ACR_EN,
                                      (packet_process == E_HDMITX_STOP_PACKET) ? 0 : mask_of_hdmi_tx_reg_acr_en,
                                      mask_of_hdmi_tx_reg_acr_en);
            break;

        case E_HAL_HDMITX_PACKET_TYPE_AS:
            HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AUDIO_EN,
                                      (packet_process == E_HDMITX_STOP_PACKET) ? 0 : mask_of_hdmi_tx_reg_audio_en,
                                      mask_of_hdmi_tx_reg_audio_en);
            break;

        case E_HAL_HDMITX_PACKET_TYPE_GC:
            HalHdmitxUtilityW2BYTEMSK(
                HDMI_TX_REG_AV_MUTE,
                (psHDMITXResPri->stGCPara.enAVMute == E_HDMITX_GCP_SET_AVMUTE) ? mask_of_hdmi_tx_reg_av_mute : 0,
                mask_of_hdmi_tx_reg_av_mute);
            HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_GC_EN,
                                      (packet_process == E_HDMITX_STOP_PACKET) ? 0 : mask_of_hdmi_tx_reg_gc_en,
                                      mask_of_hdmi_tx_reg_gc_en);
            break;
        case E_HAL_HDMITX_PACKET_TYPE_ACP:
            // TBD
            break;

        case E_HAL_HDMITX_PACKET_TYPE_ISRC1:
            // TBD
            break;

        case E_HAL_HDMITX_PACKET_TYPE_ISRC2:
            // TBD
            break;

        case E_HAL_HDMITX_PACKET_TYPE_DSD:
            // TBD
            break;

        case E_HAL_HDMITX_PACKET_TYPE_HBR:
            // TBD
            break;

        case E_HAL_HDMITX_PACKET_TYPE_GM:
            // TBD
            break;
        default:
            HDMITX_ERR("%s:: Invalid Packet Type!! %u \r\n", __FUNCTION__, enPktType);
            break;
    }
}
u8 HalHdmitxInfoFrameContentDefine(u32 u32Dev, HalHdmitxInfoFrameType_e packet_type, u8 *data, u8 length)
{
    u8 bRet = TRUE;
    u8 i, *ptr;

    ptr = data;
    switch (packet_type)
    {
        case E_HAL_HDMITX_INFOFRAM_TYPE_VS:
            for (i = 0; i < length; i++)
            {
                if (i >= HDMITX_PKT_VS_LEN)
                {
                    // Packet over size, last VS packet PB register is REG_PKT_VS_14_34[7:0]
                    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "VS packet over size, length = %d \n", length);
                    break;
                }
                HalHdmitxUtilityWBYTEMSK(REG_VS_00_L + 1 + i, *(ptr + i), 0xFF);
            }
            break;
        case E_HAL_HDMITX_INFOFRAM_TYPE_SPD:
            for (i = 0; i < length; i++)
            {
                if (i >= HDMITX_PKT_SPD_LEN)
                {
                    // Packet over size, last SPD packet PB register is REG_PKT_SPD_13_21[7:0]
                    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "VS packet over size, length = %d \n", length);
                    break;
                }
                HalHdmitxUtilityWBYTEMSK(REG_SPD_00_L + 1 + i, *(ptr + i), 0xFF);
            }
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AVI:
            for (i = 0; i < length; i++)
            {
                if (length > HDMITX_PKT_AVI_LEN)
                {
                    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "AVI packet over size, length = %d \n", length);
                    break;
                }
                HalHdmitxUtilityWBYTEMSK(REG_AVI_00_L + 1 + i, *(ptr + i), 0xFF);
            }
            break;

        case E_HAL_HDMITX_INFOFRAM_TYPE_AUDIO:
        {
            // total length should be 192 bits ; 24byte
            length = (length < ((192 >> 3))) ? length : ((192 >> 3));
            for (i = 0; i < length; i++)
            {
                HalHdmitxUtilityWBYTEMSK(REG_ASP_CS_00_L + 1 + i, *(ptr + i), 0xFF);
            }
        }
        break;

        default:
            bRet = FALSE;
            HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "Not implemented, packet type = %u\n", packet_type);
            break;
    }

    return bRet;
}
void HalHdmitxSetHDMImode(u32 u32Dev, u8 bflag, HalHdmitxColorDepthType_e cd_val)
{
    HDMITX_DBG(HDMITX_DBG_LEVEL_HAL, "[%s][%d]HDMI mode = %d, Color Depth = %d \n", __FUNCTION__, __LINE__, bflag,
               cd_val);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_HDMI_DVI, bflag ? mask_of_hdmi_tx_reg_hdmi_dvi : 0,
                              mask_of_hdmi_tx_reg_hdmi_dvi);
}
u8 HalHdmitxColorandRangeTransform(u32 u32Dev, HalHdmitxColorType_e incolor, HalHdmitxColorType_e outcolor,
                                   HalHdmitxYccQuantRange_e inrange, HalHdmitxYccQuantRange_e outrange)
{
    u8  bRet = TRUE;
    u16 u16Val;

    u16Val = HDMITX_GET_CSC_SEL_VAL(incolor, outcolor);
    if (u16Val != E_HDMITX_CSC_SEL_MAX)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CSC_FUNCTION_SEL, u16Val, mask_of_hdmi_tx_reg_csc_function_sel);
    }
    else
    {
        bRet = FALSE;
        HDMITX_ERR("%s %d, InColor=%d, OutColor=%d, Not Supported\n", __FUNCTION__, __LINE__, incolor, outcolor);
    }

    return bRet;
}
void HalHdmitxSetAudioFrequency(u32 u32Dev, HalHdmitxAudioFreqType_e afidx, HalHdmitxAudioChannelType_e achidx,
                                HalHdmitxAudioCodingType_e actidx, HalHdmitxTimingResType_e vidtiming)
{
    u32 u32CTS, u32N;

    // HDMI audio channel setting
    if (achidx == E_HAL_HDMITX_AUDIO_CH_2) // 2 channels
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AUDIO_LAYOUT, 0, mask_of_hdmi_tx_reg_audio_layout);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PCM_8CH_ALLOC, 0, mask_of_hdmi_tx_reg_pcm_8ch_alloc);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ASP_SAMP_FLAT, 0, mask_of_hdmi_tx_reg_asp_samp_flat);
    }
    else // 8 channels
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AUDIO_LAYOUT, mask_of_hdmi_tx_reg_audio_layout,
                                  mask_of_hdmi_tx_reg_audio_layout);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_PCM_8CH_ALLOC, mask_of_hdmi_tx_reg_pcm_8ch_alloc,
                                  mask_of_hdmi_tx_reg_pcm_8ch_alloc);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ASP_SAMP_FLAT, 0, mask_of_hdmi_tx_reg_asp_samp_flat);
    }

    // Audio channel status
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ASP_CH_STATUS_0, ((actidx == E_HAL_HDMITX_AUDIO_CODING_PCM) ? 0 : HDMITX_BIT1),
                              mask_of_hdmi_tx_reg_asp_ch_status_0);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ASP_CH_STATUS_1, (achidx << 4) | g_stTxAudioFreqTbl[afidx].CH_Status3 << 8,
                              mask_of_hdmi_tx_reg_asp_ch_status_1);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ASP_CH_STATUS_2, 0, mask_of_hdmi_tx_reg_asp_ch_status_2);

    // ACR CTS code
    u32CTS = g_stTxAudioFreqTbl[afidx].stCtsN.u32CTS;
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ACR_CTS_L, u32CTS & mask_of_hdmi_tx_reg_acr_cts_l,
                              mask_of_hdmi_tx_reg_acr_cts_l);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ACR_CTS_H, u32CTS >> (16 - shift_of_hdmi_tx_reg_acr_cts_h),
                              mask_of_hdmi_tx_reg_acr_cts_h);

    // ACR N code
    u32N = g_stTxAudioFreqTbl[afidx].stCtsN.u32N;
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ACR_N_L, u32N & mask_of_hdmi_tx_reg_acr_cts_l, mask_of_hdmi_tx_reg_acr_n_l);
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_ACR_N_H, u32N >> (16 - shift_of_hdmi_tx_reg_acr_n_h),
                              mask_of_hdmi_tx_reg_acr_n_h);
}
void HalHdmitxSetVideoOnOff(u32 u32Dev, u8 bVideo, u8 bCSC, u8 b709format)
{
    if (bVideo == TRUE)
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH0_MUTE, 0x00, mask_of_hdmi_tx_reg_ch0_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH1_MUTE, 0x00, mask_of_hdmi_tx_reg_ch1_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH2_MUTE, 0x00, mask_of_hdmi_tx_reg_ch2_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_VIDEO_MUTE, 0x00, mask_of_hdmi_tx_reg_video_mute);
    }
    else
    {
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH0_MUTE, mask_of_hdmi_tx_reg_ch0_mute, mask_of_hdmi_tx_reg_ch0_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH1_MUTE, mask_of_hdmi_tx_reg_ch1_mute, mask_of_hdmi_tx_reg_ch1_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_CH2_MUTE, mask_of_hdmi_tx_reg_ch2_mute, mask_of_hdmi_tx_reg_ch2_mute);
        HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_VIDEO_MUTE, mask_of_hdmi_tx_reg_video_mute,
                                  mask_of_hdmi_tx_reg_video_mute);
    }
}
void HalHdmitxSetAudioOnOff(u32 u32Dev, u8 bflag)
{
    HalHdmitxUtilityW2BYTEMSK(HDMI_TX_REG_AUDIO_MUTE, bflag ? 0 : mask_of_hdmi_tx_reg_audio_mute,
                              mask_of_hdmi_tx_reg_audio_mute);
}
u8 HalHdmitxSetClk(u8 *pbEn, u32 *pu32ClkRate, u32 u32Size)
{
    u16 u16RegVal;
    u8  bRet = TRUE;

    if (pbEn && pu32ClkRate && u32Size == HAL_HDMITX_CLK_NUM)
    {
        u16RegVal = pu32ClkRate[0] <= CLK_MHZ(432) ? 0x00 << 10 : 0x00;

        u16RegVal |= pbEn[0] ? 0x0000 : 0x0100;
        HalHdmitxUtilityW2BYTEMSK(HDMITX_CLKGEN_48_L, u16RegVal, 0x0F00);
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}

u8 HalHdmitxGetClk(u8 *pbEn, u32 *pu32ClkRate, u32 u32Size)
{
    u16 u16RegVal;
    u8  bRet = TRUE;

    if (pbEn && pu32ClkRate && u32Size == HAL_HDMITX_CLK_NUM)
    {
        u16RegVal = HalHdmitxUtilityR2BYTE(HDMITX_CLKGEN_48_L);

        pbEn[0]        = (u16RegVal & 0x0100) ? 0 : 1;
        u16RegVal      = (u16RegVal >> 10) & 0x03;
        pu32ClkRate[0] = (u16RegVal == 0x00) ? CLK_MHZ(432) : 0;

        bRet = TRUE;
    }
    else
    {
        bRet = FALSE;
    }
    return bRet;
}
