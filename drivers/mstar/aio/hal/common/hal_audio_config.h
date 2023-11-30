/*
* hal_audio_config.h- Sigmastar
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

#ifndef __HAL_AUD_CONFIG_H__
#define __HAL_AUD_CONFIG_H__

////////////////////////////////////////////////////////////////////////////
//
// Below part is for code reuse, you can't modify it randomly !!!
//
////////////////////////////////////////////////////////////////////////////

// ------------------------
// Card
// ------------------------
struct audio_card
{
	const char *name;
    const struct audio_ao_dev *ao_devs;
    const struct audio_ai_dev *ai_devs;
    int num_ao_devs;
    int num_ai_devs;
    const struct audio_i2sIf *i2sIfs;
    int num_i2sIfs;
    const struct audio_i2s *i2ss;
    int num_i2ss;
    const struct audio_dma *dmas;
    int num_dmas;
    const struct audio_dmic *dmics;
    int num_dmic;
    const struct audio_dts *dts;
};

// ------------------------
// AO Device
// ------------------------
struct audio_ao_dev
{
	int id; /* dev ID */
    const struct audio_ao_dev_info *info;
    const struct audio_ao_dev_op *op;
};

// ------------------------
// AI Device
// ------------------------
struct audio_ai_dev
{
	int id; /* dev ID */
    const struct audio_ai_dev_info *info;
    const struct audio_ai_dev_op *op;
};

// ------------------------
// AO Info
// ------------------------
struct audio_ao_dev_info
{
    int dma;
    int i2s_if;
    int i2s;
    int tdm;
    int atop_path;
    BOOL use_i2s;
    BOOL use_tdm;
    BOOL use_atop;
    /* Mux */
    const int (*mux_list_for_config_pcm)[2];
    int num_mux_config_pcm;
    const int (*mux_list_for_config_i2s)[2];
    int num_mux_config_i2s;
    /* DPGA */
    const int *dpga_list;
    int num_dpga;
    /* Mch */
    BOOL use_mch;
    const struct audio_info_mch *info_mch;
    /* i2sIf cfg private data */
    void* i2sIf_cfg_private_data;

    // Only For AO below
    /* Audio2HDMI */
    BOOL use_hdmi;
    /* Audio2HDMI */
    BOOL use_src_dpga;
    int src_dpga;
};

// ------------------------
// AI Info
// ------------------------
struct audio_ai_dev_info
{
    int dma;
    int i2s_if;
    int i2s;
    int tdm;
    int atop_path;
    BOOL use_i2s;
    BOOL use_tdm;
    BOOL use_atop;
    /* Mux */
    const int (*mux_list_for_config_pcm)[2];
    int num_mux_config_pcm;
    const int (*mux_list_for_config_i2s)[2];
    int num_mux_config_i2s;
    /* DPGA */
    const int *dpga_list;
    int num_dpga;
    /* Mch */
    BOOL use_mch;
    const struct audio_info_mch *info_mch;
    /* i2sIf cfg private data */
    void* i2sIf_cfg_private_data;

    // Only For AI below
    /* ADC */
    int adc_sel;
    /* Dmic */
    BOOL use_dmic;
    /* AEC */
    BOOL use_aec;
};

// ------------------------
// AO Op
// ------------------------
struct audio_ao_dev_op
{

};

// ------------------------
// AI Op
// ------------------------
struct audio_ai_dev_op
{

};

// ------------------------
// I2S_IF
// ------------------------
struct audio_i2sIf
{
	int id; /* i2sIf ID */
    const struct audio_i2sIf_info *info;
    const struct audio_i2sIf_op *op;
};

// ------------------------
// I2S_IF Info
// ------------------------
struct audio_i2sIf_info
{
    int i2s;
    int pad_mux;
};

// ------------------------
// I2S_IF Op
// ------------------------
struct audio_i2sIf_op
{
    BOOL (*CfgI2sIfPrivateCb)( void *pPriData );
};

// ------------------------
// I2S
// ------------------------
struct audio_i2s
{
	int id; /* i2s ID */
    const struct audio_i2s_info *info;
    const struct audio_i2s_op *op;
};

// ------------------------
// I2S Info
// ------------------------
struct audio_i2s_info
{

};

// ------------------------
// I2S Op
// ------------------------
struct audio_i2s_op
{

};

// ------------------------
// DMA
// ------------------------
struct audio_dma
{
	int id; /* dma ID */
    const struct audio_dma_info *info;
    const struct audio_dma_op *op;
};

// ------------------------
// DMA Info
// ------------------------
struct audio_dma_info
{

};

// ------------------------
// DMA Op
// ------------------------
struct audio_dma_op
{
};

// ------------------------
// DMIC
// ------------------------
struct audio_dmic
{
	int id; /* dmic ID */
    const struct audio_dmic_info *info;
    const struct audio_dmic_op *op;
};

// ------------------------
// DMIC Info
// ------------------------
struct audio_dmic_info
{
    int pad_mux;
};

// ------------------------
// DMIC Op
// ------------------------
struct audio_dmic_op
{
};

// ------------------------
// DTS
// ------------------------
typedef BOOL (*audio_dts_padmux_fp)(int);

struct audio_dts_padmux
{
    const char *str;
    const audio_dts_padmux_fp fp;
};

typedef BOOL (*template_fp_type)(int); // template_fp

struct audio_dts
{
	int id; /* ID */
    const char *audio_dts_compatible_str;
    const char *audio_dts_amp_gpio_str;
    const char *audio_dts_keep_i2s_clk_str;
    const char *audio_dts_i2s_pcm_str;
    const struct audio_dts_padmux *audio_dts_padmux_list;
    const int num_padmux;
    const char *audio_dts_i2s_Rx_Tdm_WsPgm_str;
    const char *audio_dts_i2s_Rx_Tdm_WsWidth_str;
    const char *audio_dts_i2s_Rx_Tdm_WsInv_str;
    const char *audio_dts_i2s_Rx_Tdm_ChSwap_str;
    const char *audio_dts_i2s_Tx_Tdm_WsPgm_str;
    const char *audio_dts_i2s_Tx_Tdm_WsWidth_str;
    const char *audio_dts_i2s_Tx_Tdm_WsInv_str;
    const char *audio_dts_i2s_Tx_Tdm_ChSwap_str;
    const char *audio_dts_i2s_Tx_Tdm_ActiveSlot_str;
    const char *audio_dts_keep_adc_power_on;
    const char *audio_dts_keep_dac_power_on;
    const template_fp_type *template_fp_list;
};

// ------------------------
// OTHERS
// ------------------------
// mch ch sel
struct audio_mch_bind
{
    int para_dma_ch;
    int num_mch_ch;
    const int *mch_sel;
};

// mch
struct audio_info_mch
{
    int mch_set;
    const struct audio_mch_bind *mch_bind;
    int num_mch_bind;
    int mch_clk_ref;
};

//i2 clock ref
struct audio_info_i2s_clk_ref
{
    int clk_ref_sel;
};

////////////////////////////////////////////////////////////////////////////
// extern
////////////////////////////////////////////////////////////////////////////
extern struct audio_card card;

#endif //__HAL_AUD_CONFIG_H__
