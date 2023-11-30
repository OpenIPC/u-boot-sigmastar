/*
* hal_audio_config.c- Sigmastar
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
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_reg.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"
#include "hal_audio_pri_api.h"

////////////////////////////////////////////////////////////////////////////
//
// Just config it !
//
////////////////////////////////////////////////////////////////////////////

// Macro for audio config
#define AUD_ARRAY_SIZE(arr)     (sizeof(arr) / sizeof((arr)[0]))



// ------------------------
// DTS
// ------------------------

// audio_dts_padmux
static const struct audio_dts_padmux audio_dts_padmux_array[] =
{
    {
        .str = "i2s-padmux",
        .fp = HalAudPriSetPadMuxI2s,
    },

    {
        .str = "digmic-padmux",
        .fp = HalAudPriSetPadMuxDmic,
    },
};

// template_fp
static template_fp_type template_fp_array[] =
{
    HalAudPriTemplateFp,
    NULL,
};

static const struct audio_dts audio_dts_cfg[] =
{
    {
    .id = 0,
    .audio_dts_compatible_str = "sstar,audio",
    .audio_dts_amp_gpio_str = "amp-gpio",
    .audio_dts_keep_i2s_clk_str = "keep-i2s-clk",
    .audio_dts_i2s_pcm_str = "i2s-pcm",
    .audio_dts_padmux_list = audio_dts_padmux_array,
    .num_padmux = AUD_ARRAY_SIZE(audio_dts_padmux_array),
    .audio_dts_i2s_Rx_Tdm_WsPgm_str = NULL,
    .audio_dts_i2s_Rx_Tdm_WsWidth_str = NULL,
    .audio_dts_i2s_Rx_Tdm_WsInv_str = NULL,
    .audio_dts_i2s_Rx_Tdm_ChSwap_str = NULL,
    .audio_dts_i2s_Tx_Tdm_WsPgm_str = NULL,
    .audio_dts_i2s_Tx_Tdm_WsWidth_str = NULL,
    .audio_dts_i2s_Tx_Tdm_WsInv_str = NULL,
    .audio_dts_i2s_Tx_Tdm_ChSwap_str = NULL,
    .audio_dts_i2s_Tx_Tdm_ActiveSlot_str = NULL,
    .template_fp_list = template_fp_array, // template_fp
    }
};

// ------------------------
// DMIC Op
// ------------------------
static const struct audio_dmic_op dmic_ops[] =
{

};

// ------------------------
// DMIC Info
// ------------------------
// info
static const struct audio_dmic_info dmic_infos[] =
{
    {
        .pad_mux    = AUD_PAD_MUX_DMIC,
    },
};

// ------------------------
// DMIC
// ------------------------
static const struct audio_dmic audio_dmics[] =
{
    {
        .id = 0,
        .info = dmic_infos,
        .op = dmic_ops,
    },
};

// ------------------------
// I2S Op
// ------------------------

// ------------------------
// I2S Info
// ------------------------
// info

// ------------------------
// I2S
// ------------------------
static const struct audio_i2s audio_i2ss[] =
{
    {
        .id = AUD_I2S_CODEC_RX,
        .info = NULL,
        .op = NULL,
    },

    {
        .id = AUD_I2S_CODEC_TX,
        .info = NULL,
        .op = NULL,
    },
};

// ------------------------
// I2S_IF Op
// ------------------------
static const struct audio_i2sIf_op i2sIf_ops_codec_rx[] =
{
    {
        .CfgI2sIfPrivateCb  = HalAudCfgI2sIfPrivateCb,
    },
};

// ------------------------
// I2S_IF Info
// ------------------------
// info
static const struct audio_i2sIf_info i2sIf_infos_codec_rx[] =
{
    {
        .i2s        = AUD_I2S_CODEC_RX,
        .pad_mux    = AUD_PAD_MUX_I2S_RX,
    },
};

static const struct audio_i2sIf_info i2sIf_infos_codec_tx[] =
{
    {
        .i2s        = AUD_I2S_CODEC_TX,
        .pad_mux    = AUD_PAD_MUX_I2S_TX,
    },
};

// ------------------------
// I2S_IF
// ------------------------
static const struct audio_i2sIf audio_i2sIfs[] =
{
    {
        .id = AUD_I2S_IF_CODEC_RX,
        .info = i2sIf_infos_codec_rx,
        .op = i2sIf_ops_codec_rx,
    },

    {
        .id = AUD_I2S_IF_CODEC_TX,
        .info = i2sIf_infos_codec_tx,
        .op = NULL,
    },
};

// ------------------------
// AO Op
// ------------------------
static const struct audio_ao_dev_op dev_ops_ao_line_out[] =
{

};

static const struct audio_ao_dev_op dev_ops_ao_i2s_tx[] =
{
    //.config_i2s = Template_Function;
};

static const struct audio_ao_dev_op dev_ops_ao_hdmi[] =
{

};

// ------------------------
// AI Op
// ------------------------
static const struct audio_ai_dev_op dev_ops_ai_adc[] =
{


};

static const struct audio_ai_dev_op dev_ops_ai_dmic[] =
{


};

static const struct audio_ai_dev_op dev_ops_ai_i2s_rx[] =
{


};

static const struct audio_ai_dev_op dev_ops_ai_amic_rx[] =
{


};

static const struct audio_ai_dev_op dev_ops_ai_dmic_rx[] =
{


};

// ------------------------
// AO Info
// ------------------------

// mux when config pcm
static const int mux_list_for_config_pcm_ao_line_out[][2] =
{
    { AUD_MUX_MMC1, 1 },
};

// mux when config i2s
static const int mux_list_for_config_i2s_ao_i2s_tx[][2] =
{
    { AUD_MUX_I2S_TX, 0 }, // Bypass SRC.
};

// mux when config hdmi
static const int mux_list_for_config_i2s_ao_hdmi[][2] =
{
    { AUD_MUX_MMC1, 1 },
};

// dpga
static const int dpga_list_ao_line_out[] =
{
    AUD_DPGA_MMC1,
};

static const int dpga_list_ao_i2s_tx[] =
{
    AUD_DPGA_MMC1,
};

static const int dpga_list_ao_hdmi[] =
{
    AUD_DPGA_MMC1,
};

// info
static const struct audio_ao_dev_info dev_infos_ao_line_out[] =
{
    {
    .dma        = AUD_DMA_READER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = AUD_DAC_LINEOUT,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = TRUE,
    .use_hdmi   = FALSE,
    .use_src_dpga = TRUE,
    .src_dpga = AUD_DPGA_DEC1,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ao_line_out,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ao_line_out),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = dpga_list_ao_line_out,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ao_line_out),

    /* Mch */
    .use_mch = FALSE,
    .info_mch = NULL,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,
    },
};

static const struct audio_ao_dev_info dev_infos_ao_i2s_tx[] =
{
    {
    .dma        = AUD_DMA_READER1,
    .i2s_if     = AUD_I2S_IF_CODEC_TX,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = 0,
    .use_i2s    = TRUE,
    .use_tdm    = FALSE,
    .use_atop   = FALSE,
    .use_hdmi   = FALSE,
    .use_src_dpga = TRUE,
    .src_dpga = AUD_DPGA_DEC1,
    .mux_list_for_config_pcm = NULL,
    .num_mux_config_pcm = 0,
    .mux_list_for_config_i2s = mux_list_for_config_i2s_ao_i2s_tx,
    .num_mux_config_i2s = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ao_i2s_tx),
    .dpga_list  = dpga_list_ao_i2s_tx,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ao_i2s_tx),

    /* Mch */
    .use_mch = FALSE,
    .info_mch = NULL,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,
    },
};

static const struct audio_ao_dev_info dev_infos_ao_hdmi[] =
{
    {
    .dma        = AUD_DMA_READER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = 0,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = FALSE,
    .use_hdmi   = TRUE,
    .use_src_dpga = FALSE,
    .src_dpga = AUD_DPGA_NULL,
    .mux_list_for_config_pcm = mux_list_for_config_i2s_ao_hdmi,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ao_hdmi),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = dpga_list_ao_hdmi,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ao_hdmi),

    /* Mch */
    .use_mch = FALSE,
    .info_mch = NULL,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,
    },
};

static const struct audio_ao_dev_info dev_infos_ao_lineout_hdmi[] =
{
    {
    .dma        = AUD_DMA_READER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = AUD_DAC_LINEOUT,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = TRUE,
    .use_hdmi   = TRUE,
    .use_src_dpga = TRUE,
    .src_dpga = AUD_DPGA_DEC1,
    .mux_list_for_config_pcm = mux_list_for_config_i2s_ao_hdmi,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ao_hdmi),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = dpga_list_ao_hdmi,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ao_hdmi),

    /* Mch */
    .use_mch = FALSE,
    .info_mch = NULL,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,
    },
};

// ------------------------
// AI Info
// ------------------------

// mux when config pcm
static const int mux_list_for_config_pcm_ai_adc[][2] =
{
    { AUD_MUX_AMIC_DMIC, 0 },
    { AUD_MUX_DMAWR1, 1 },
    { AUD_MUX_DMAWR1_MCH, 1 },
};

static const int mux_list_for_config_pcm_ai_dmic[][2] =
{
    { AUD_MUX_AMIC_DMIC, 1 },
    { AUD_MUX_DMAWR1, 1 },
    { AUD_MUX_DMAWR1_MCH, 1 },
};

// mux when config i2s
static const int mux_list_for_config_i2s_ai_i2s_rx[][2] =
{
    { AUD_MUX_DMAWR1_MCH, 1 },
};

// dpga
static const int dpga_list_ai_adc[] =
{
    AUD_DPGA_ADC,
    AUD_DPGA_AEC1,
};

static const int dpga_list_ai_dmic[] =
{

};

// mch_sel
static const int mch_sel_ai_adc[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_ADC,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_NULL,
};

static const int mch_sel_ai_dmic_0[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_DMIC,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_NULL,
};

static const int mch_sel_ai_dmic_1[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_NULL,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_DMIC,
};

static const int mch_sel_ai_i2s_rx[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_I2S_RX,
    AUD_MCH_SEL_NULL,
    AUD_MCH_SEL_NULL,
};

static const int mch_sel_ai_amic_rx[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_ADC,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_I2S_RX,
};

static const int mch_sel_ai_dmic_rx[AUD_MCH_CH_BIND_NUM] =
{
    AUD_MCH_SEL_DMIC,
    AUD_MCH_SEL_SRC,
    AUD_MCH_SEL_I2S_RX,
};

// mch_ch_sel_bind
static const struct audio_mch_bind mch_bind_ai_adc[] =
{
    // when ch = 1 -> mch_ch_num = 4, ch01:MIC ch23:SRC ch4567: X
    {
        .para_dma_ch = 1,
        .num_mch_ch = 4,
        .mch_sel = mch_sel_ai_adc,
    },

    // when ch = 2 -> mch_ch_num = 4, ch01:MIC ch23:SRC ch4567: X
    {
        .para_dma_ch = 2,
        .num_mch_ch = 4,
        .mch_sel = mch_sel_ai_adc,
    },
};

static const struct audio_mch_bind mch_bind_ai_dmic[] =
{
    // when ch = 1 -> mch_ch_num = 4, ch01:DMIC ch23:SRC ch4567: X
    {
        .para_dma_ch = 1,
        .num_mch_ch = 4,
        .mch_sel = mch_sel_ai_dmic_0,
    },

    // when ch = 2 -> mch_ch_num = 4, ch01:DMIC ch23:SRC ch4567: X
    {
        .para_dma_ch = 2,
        .num_mch_ch = 4,
        .mch_sel = mch_sel_ai_dmic_0,
    },

    // when ch = 4 -> mch_ch_num = 8, ch01:X ch23:SRC ch4567: DMIC
    {
        .para_dma_ch = 4,
        .num_mch_ch = 8,
        .mch_sel = mch_sel_ai_dmic_1,
    },
};

static const struct audio_mch_bind mch_bind_ai_i2s_rx[] =
{
    // when ch = 2 -> mch_ch_num = 2, ch01:i2s_rx ch23:X ch4567: X
    {
        .para_dma_ch = 2,
        .num_mch_ch = 2,
        .mch_sel = mch_sel_ai_i2s_rx,
    },
};

static const struct audio_mch_bind mch_bind_ai_amic_rx[] =
{
    // when ch = 2 -> mch_ch_num = 2, ch01:i2s_rx ch23:X ch4567: X
    {
        .para_dma_ch = 2,
        .num_mch_ch = 8,
        .mch_sel = mch_sel_ai_amic_rx,
    },
};

static const struct audio_mch_bind mch_bind_ai_dmic_rx[] =
{
    // when ch = 2 -> mch_ch_num = 2, ch01:i2s_rx ch23:X ch4567: X
    {
        .para_dma_ch = 2,
        .num_mch_ch = 8,
        .mch_sel = mch_sel_ai_dmic_rx,
    },
};

// info_mch
static const struct audio_info_mch info_mch_ai_adc[] =
{
    {
    .mch_set = AUD_MCH_SET_W1,
    .mch_bind = mch_bind_ai_adc,
    .num_mch_bind = AUD_ARRAY_SIZE(mch_bind_ai_adc),
    .mch_clk_ref = AUD_MCH_CLK_REF_ADC,
    },
};

static const struct audio_info_mch info_mch_ai_dmic[] =
{
    {
    .mch_set = AUD_MCH_SET_W1,
    .mch_bind = mch_bind_ai_dmic,
    .num_mch_bind = AUD_ARRAY_SIZE(mch_bind_ai_dmic),
    .mch_clk_ref = AUD_MCH_CLK_REF_DMIC,
    },
};

static const struct audio_info_mch info_mch_ai_i2s_rx[] =
{
    {
    .mch_set = AUD_MCH_SET_W1,
    .mch_bind = mch_bind_ai_i2s_rx,
    .num_mch_bind = AUD_ARRAY_SIZE(mch_bind_ai_i2s_rx),
    .mch_clk_ref = AUD_MCH_CLK_REF_ADC,
    },
};

static const struct audio_info_mch info_mch_ai_amic_rx[] =
{
    {
    .mch_set = AUD_MCH_SET_W1,
    .mch_bind = mch_bind_ai_amic_rx,
    .num_mch_bind = AUD_ARRAY_SIZE(mch_bind_ai_amic_rx),
    .mch_clk_ref = AUD_MCH_CLK_REF_ADC,
    },
};

static const struct audio_info_mch info_mch_ai_dmic_rx[] =
{
    {
    .mch_set = AUD_MCH_SET_W1,
    .mch_bind = mch_bind_ai_dmic_rx,
    .num_mch_bind = AUD_ARRAY_SIZE(mch_bind_ai_dmic_rx),
    .mch_clk_ref = AUD_MCH_CLK_REF_DMIC,
    },
};

// info
static const struct audio_ai_dev_info dev_infos_ai_mic[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = AUD_ADC_MICIN,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = TRUE,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ai_adc,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ai_adc),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = dpga_list_ai_adc,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ai_adc),

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_adc,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_MICIN,
    /* Dmic */
    .use_dmic   = FALSE,
    /* AEC */
    .use_aec    = TRUE,
    },
};

static const struct audio_ai_dev_info dev_infos_ai_dmic[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = 0,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = FALSE,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ai_dmic,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ai_dmic),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = NULL,
    .num_dpga   = 0,

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_dmic,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_NULL,
    /* Dmic */
    .use_dmic   = TRUE,
    /* AEC */
    .use_aec    = TRUE,
    },
};

static const struct audio_ai_dev_info dev_infos_ai_i2s_rx[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_CODEC_RX,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = 0,
    .use_i2s    = TRUE,
    .use_tdm    = FALSE,
    .use_atop   = FALSE,
    .mux_list_for_config_pcm = NULL,
    .num_mux_config_pcm = 0,
    .mux_list_for_config_i2s = mux_list_for_config_i2s_ai_i2s_rx,
    .num_mux_config_i2s = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ai_i2s_rx),
    .dpga_list  = NULL,
    .num_dpga   = 0,

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_i2s_rx,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_NULL,
    /* Dmic */
    .use_dmic   = FALSE,
    /* AEC */
    .use_aec    = FALSE,
    },
};

static const struct audio_ai_dev_info dev_infos_ai_line_in[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_NULL,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = AUD_ADC_LINEIN,
    .use_i2s    = FALSE,
    .use_tdm    = FALSE,
    .use_atop   = TRUE,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ai_adc,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ai_adc),
    .mux_list_for_config_i2s = NULL,
    .num_mux_config_i2s = 0,
    .dpga_list  = dpga_list_ai_adc,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ai_adc),

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_adc,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_LINEIN,
    /* Dmic */
    .use_dmic   = FALSE,
    /* AEC */
    .use_aec    = TRUE,
    },
};

static const struct audio_ai_dev_info dev_infos_ai_amic_rx[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_CODEC_RX,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = AUD_ADC_MICIN,
    .use_i2s    = TRUE,
    .use_tdm    = FALSE,
    .use_atop   = TRUE,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ai_adc,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ai_adc),
    .mux_list_for_config_i2s = mux_list_for_config_i2s_ai_i2s_rx,
    .num_mux_config_i2s = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ai_i2s_rx),
    .dpga_list  = dpga_list_ai_adc,
    .num_dpga   = AUD_ARRAY_SIZE(dpga_list_ai_adc),

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_amic_rx,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_MICIN,
    /* Dmic */
    .use_dmic   = FALSE,
    /* AEC */
    .use_aec    = TRUE,
    },
};

static const struct audio_ai_dev_info dev_infos_ai_dmic_rx[] =
{
    {
    .dma        = AUD_DMA_WRITER1,
    .i2s_if     = AUD_I2S_IF_CODEC_RX,
    .i2s        = 0,
    .tdm        = 0,
    .atop_path  = 0,
    .use_i2s    = TRUE,
    .use_tdm    = FALSE,
    .use_atop   = FALSE,
    .mux_list_for_config_pcm = mux_list_for_config_pcm_ai_dmic,
    .num_mux_config_pcm = AUD_ARRAY_SIZE(mux_list_for_config_pcm_ai_dmic),
    .mux_list_for_config_i2s = mux_list_for_config_i2s_ai_i2s_rx,
    .num_mux_config_i2s = AUD_ARRAY_SIZE(mux_list_for_config_i2s_ai_i2s_rx),
    .dpga_list  = NULL,
    .num_dpga   = 0,

    /* Mch */
    .use_mch = TRUE,
    .info_mch = info_mch_ai_dmic_rx,

    /* i2sIf cfg private data */
    .i2sIf_cfg_private_data = NULL,

    // Only For AI below
    /* ADC */
    .adc_sel = AUD_ADC_SEL_NULL,
    /* Dmic */
    .use_dmic   = TRUE,
    /* AEC */
    .use_aec    = TRUE,
    },
};


// ------------------------
// AO Device
// ------------------------
static const struct audio_ao_dev audio_ao_devs[] =
{
    {
        .id = AUD_AO_DEV_LINE_OUT,
        .info = dev_infos_ao_line_out,
        .op = dev_ops_ao_line_out,
    },

    {
        .id = AUD_AO_DEV_I2S_TX,
        .info = dev_infos_ao_i2s_tx,
        .op = dev_ops_ao_i2s_tx,
    },

    {
        .id = AUD_AO_DEV_HDMI,
        .info = dev_infos_ao_hdmi,
        .op = dev_ops_ao_hdmi,
    },

    {
        .id = AUD_AO_DEV_LINEOUT_HDMI,
        .info = dev_infos_ao_lineout_hdmi,
        .op = dev_ops_ao_hdmi,
    },
};

// ------------------------
// AI Device
// ------------------------
static const struct audio_ai_dev audio_ai_devs[] =
{
    {
        .id = AUD_AI_DEV_MIC,
        .info = dev_infos_ai_mic,
        .op = dev_ops_ai_adc,
    },

    {
        .id = AUD_AI_DEV_DMIC,
        .info = dev_infos_ai_dmic,
        .op = dev_ops_ai_dmic,
    },

    {
        .id = AUD_AI_DEV_I2S_RX,
        .info = dev_infos_ai_i2s_rx,
        .op = dev_ops_ai_i2s_rx,
    },

    {
        .id = AUD_AI_DEV_LINE_IN,
        .info = dev_infos_ai_line_in,
        .op = dev_ops_ai_adc,
    },

    {
        .id = AUD_AI_DEV_AMIC_RX,
        .info = dev_infos_ai_amic_rx,
        .op = dev_ops_ai_amic_rx,
    },

    {
        .id = AUD_AI_DEV_DMIC_RX,
        .info = dev_infos_ai_dmic_rx,
        .op = dev_ops_ai_dmic_rx,
    },
};

// ------------------------
// Card
// ------------------------
struct audio_card card =
{
    .name           = "infinity2m",
    .ao_devs        = audio_ao_devs,
    .num_ao_devs    = AUD_ARRAY_SIZE(audio_ao_devs),
    .ai_devs        = audio_ai_devs,
    .num_ai_devs    = AUD_ARRAY_SIZE(audio_ai_devs),
    .i2sIfs         = audio_i2sIfs,
    .num_i2sIfs     = AUD_ARRAY_SIZE(audio_i2sIfs),
    .i2ss           = audio_i2ss,
    .num_i2ss       = AUD_ARRAY_SIZE(audio_i2ss),
    .dmics          = audio_dmics,
    .dts            = audio_dts_cfg,
};

#if 0 // Mask it for avoid warning
// ------------------------
// DMA Op
// ------------------------
static const struct audio_dma_op dma_op_writer_1[] =
{

};

static const struct audio_dma_op dma_op_reader_1[] =
{

};

// ------------------------
// DMA Info
// ------------------------
static const struct audio_dma_info dma_info_writer_1[] =
{

};

static const struct audio_dma_info dma_info_reader_1[] =
{

};

// ------------------------
// DMA
// ------------------------
static const struct audio_dma audio_dmas[] =
{
    {
        .id = AUD_DMA_WRITER1,
        .info = dma_info_writer_1,
        .op = dma_op_writer_1,
    },

    {
        .id = AUD_DMA_READER1,
        .info = dma_info_reader_1,
        .op = dma_op_reader_1,
    },
};
#endif
