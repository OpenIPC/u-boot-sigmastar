/*
* mhal_audio.c- Sigmastar
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

#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_types.h"

#include "mhal_common.h"
#include "mhal_audio.h"
#include "mhal_audio_datatype.h"

#include "drv_audio.h"

//static CamOsMutex_t _gtAudInitMutex = {{0}};
static MHAL_AUDIO_PcmCfg_t _astOutPcmCfg[AUD_AO_DEV_NUM];
static MHAL_AUDIO_PcmCfg_t _astInPcmCfg[AUD_AI_DEV_NUM];
static MS_S16 _gnInitialed = 0;

static const MS_U16 _gaMicInCombineGainTable[][2] = {
//  {ADC_gain, Pre_gain},
    {0, 0}, // 0: -6dB
    {1, 0}, // 1: -3dB
    {2, 0}, // 2: 0dB
    {3, 0}, // 3: 3dB
    {4, 0}, // 4: 6dB
    {5, 0}, // 5: 9dB
    {6, 0}, // 6: 12dB
    {7, 0}, // 7: 15dB
    {8, 0}, // 8: 18dB
    {9, 0}, // 9: 21dB
    {8, 1}, // 10: 18dB + 6dB = 24dB
    {9, 1}, // 11: 21dB + 6dB = 27dB
    {2, 4}, // 12: 0dB + 30dB = 30dB
    {3, 4}, // 13: 3dB + 30dB = 33dB
    {2, 5}, // 14: 0dB + 36dB = 36dB
    {3, 5}, // 15: 3dB + 36dB = 39dB
    {4, 5}, // 16: 6dB + 36dB = 42dB
    {5, 5}, // 17: 9dB + 36dB = 45dB
    {6, 5}, // 18: 12dB + 36dB = 48dB
    {7, 5}, // 19: 15dB + 36dB = 51dB
    {8, 5}, // 20: 18dB + 36dB = 54dB
    {9, 5}, // 21: 21dB + 36dB = 57dB
};

/**
* \brief Init audio HW
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_Init(void *pdata)
{
    //printf("in %s\n",__FUNCTION__);

    //CamOsMutexLock(&_gtAudInitMutex);

    if (_gnInitialed == 0)
    {
        //printf("MHAL_AUDIO_Init \n",__FUNCTION__);

        //
        DrvAudInit();

        //
        _gnInitialed = 1;
    }

    //CamOsMutexUnlock(&_gtAudInitMutex);

    return MHAL_SUCCESS;
}

/**
* \brief De-Init audio HW
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_DeInit(void)
{
    //printf("in %s\n",__FUNCTION__);

    //CamOsMutexLock(&_gtAudInitMutex);

    //
    DrvAudDeInit();

    //
    _gnInitialed = 0;

    //printf("MHAL_AUDIO_DeInit \n",__FUNCTION__);

    //CamOsMutexUnlock(&_gtAudInitMutex);

    return MHAL_SUCCESS;
}

/**
* \brief Config audio I2S output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig)
{
    AudI2sCfg_t tI2sCfg;
    MS_BOOL bRet = TRUE;
    int dev = 0;
    int i = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    void* i2sIf_cfg_private_data;

    // Mux
    const int (*mux_list_for_config_i2s)[2];
    int num_mux_config_i2s = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    use_i2s = card.ao_devs[dev].info->use_i2s;
    i2s_if = card.ao_devs[dev].info->i2s_if;
    i2sIf_cfg_private_data = card.ao_devs[dev].info->i2sIf_cfg_private_data;
    mux_list_for_config_i2s = card.ao_devs[dev].info->mux_list_for_config_i2s;
    num_mux_config_i2s = card.ao_devs[dev].info->num_mux_config_i2s;

    //
    if ( use_i2s == TRUE )
    {
        switch(pstI2sConfig->eWidth)
        {
            case E_MHAL_AUDIO_BITWIDTH_16:
                tI2sCfg.eWidth = AUD_BITWIDTH_16;
                break;
            case E_MHAL_AUDIO_BITWIDTH_32:
                tI2sCfg.eWidth = AUD_BITWIDTH_32;
                break;
            default:
                printf("in %s, bitwidth %d  error\n",__FUNCTION__,pstI2sConfig->eWidth);
                return MHAL_FAILURE;
        }

        switch(pstI2sConfig->eFmt)
        {
            case E_MHAL_AUDIO_I2S_FMT_I2S:
                tI2sCfg.eFormat= AUD_I2S_FMT_I2S;
                break;
            case E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:
                tI2sCfg.eFormat= AUD_I2S_FMT_LEFT_JUSTIFY;
                break;
            default:
                printf("in %s, I2S format %d  error\n",__FUNCTION__,pstI2sConfig->eFmt);
                return MHAL_FAILURE;
        }

        switch(pstI2sConfig->e4WireMode)
        {
            case E_MHAL_AUDIO_4WIRE_ON:
                tI2sCfg.eWireMode= AUD_I2S_WIRE_4;
                break;
            case E_MHAL_AUDIO_4WIRE_OFF:
                tI2sCfg.eWireMode= AUD_I2S_WIRE_6;
                break;
            default:
                printf("in %s, I2S wire mode %d  error\n",__FUNCTION__,pstI2sConfig->e4WireMode);
                return MHAL_FAILURE;;
        }

        switch(pstI2sConfig->eRate)
        {
            case E_MHAL_AUDIO_RATE_8K:
                tI2sCfg.eRate= AUD_RATE_8K;
                break;
            case E_MHAL_AUDIO_RATE_16K:
                tI2sCfg.eRate= AUD_RATE_16K;
                break;
            case E_MHAL_AUDIO_RATE_32K:
                tI2sCfg.eRate= AUD_RATE_32K;
                break;
            case E_MHAL_AUDIO_RATE_48K:
                tI2sCfg.eRate= AUD_RATE_48K;
                break;
            default:
                printf("in %s, I2S Sample Rate %d  error\n",__FUNCTION__,pstI2sConfig->eRate);
                return MHAL_FAILURE;
        }

        //
        switch(pstI2sConfig->eMode)
        {
            case E_MHAL_AUDIO_MODE_I2S_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                break;
            case E_MHAL_AUDIO_MODE_I2S_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
                break;
            case E_MHAL_AUDIO_MODE_TDM_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                if(pstI2sConfig->u16Channels!=4 && pstI2sConfig->u16Channels!=8)
                {
                    printf("in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;
                }
                break;
            case E_MHAL_AUDIO_MODE_TDM_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
                if(pstI2sConfig->u16Channels!=4 && pstI2sConfig->u16Channels!=8)
                {
                    printf("in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;
                }
                break;
            default:
                return MHAL_FAILURE;
        }

        //
        if ( ( pstI2sConfig->u16Channels % 2 ) != 0 )
        {
            printf("in %s, pstI2sConfig->u16Channels = %d  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
            return MHAL_FAILURE;
        }
        else
        {
            tI2sCfg.nChannelNum = pstI2sConfig->u16Channels;
        }

        //
        switch(pstI2sConfig->eMck)
        {
            case E_MHAL_AUDIO_MCK_NULL:
                tI2sCfg.eMck= AUD_I2S_MCK_NULL;
                break;
            case E_MHAL_AUDIO_MCK_12_288M:
                tI2sCfg.eMck= AUD_I2S_MCK_12_288M;
                break;
            case E_MHAL_AUDIO_MCK_16_384M:
                tI2sCfg.eMck= AUD_I2S_MCK_16_384M;
                break;
            case E_MHAL_AUDIO_MCK_18_432M:
                tI2sCfg.eMck= AUD_I2S_MCK_18_432M;
                break;
            case E_MHAL_AUDIO_MCK_24_576M:
                tI2sCfg.eMck= AUD_I2S_MCK_24_576M;
                break;
            case E_MHAL_AUDIO_MCK_24M:
                tI2sCfg.eMck= AUD_I2S_MCK_24M;
                break;
            case E_MHAL_AUDIO_MCK_48M:
                tI2sCfg.eMck= AUD_I2S_MCK_48M;
                break;
            default:
                printf("in %s, I2S Mck %d  error\n",__FUNCTION__,pstI2sConfig->eMck);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        //
        for(i=0; i<num_mux_config_i2s; i++)
        {
            bRet &= DrvAudSetMux(mux_list_for_config_i2s[i][0], mux_list_for_config_i2s[i][1]);
        }

        //
        tI2sCfg.pPriData = i2sIf_cfg_private_data;

        //
        bRet &= DrvAudConfigI2s((AudI2sIf_e)i2s_if, &tI2sCfg);

        if(!bRet)
        {
            return MHAL_FAILURE;
        }
    }
    else
    {
        return MHAL_FAILURE;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Config audio I2S input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigI2sIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_I2sCfg_t *pstI2sConfig)
{
    AudI2sCfg_t tI2sCfg;
    MS_BOOL bRet = TRUE;
    int dev = 0;
    int i = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    void* i2sIf_cfg_private_data;

    // Mux
    const int (*mux_list_for_config_i2s)[2];
    int num_mux_config_i2s = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    use_i2s = card.ai_devs[dev].info->use_i2s;
    i2s_if = card.ai_devs[dev].info->i2s_if;
    i2sIf_cfg_private_data = card.ai_devs[dev].info->i2sIf_cfg_private_data;
    mux_list_for_config_i2s = card.ai_devs[dev].info->mux_list_for_config_i2s;
    num_mux_config_i2s = card.ai_devs[dev].info->num_mux_config_i2s;

    //
    if( use_i2s == TRUE )
    {
        switch(pstI2sConfig->eWidth)
        {
            case E_MHAL_AUDIO_BITWIDTH_16:
                tI2sCfg.eWidth = AUD_BITWIDTH_16;
                break;
            case E_MHAL_AUDIO_BITWIDTH_32:
                tI2sCfg.eWidth = AUD_BITWIDTH_32;
                break;
            default:
                printf("in %s, bitwidth %d  error\n",__FUNCTION__,pstI2sConfig->eWidth);
                return MHAL_FAILURE;
        }

        switch(pstI2sConfig->eFmt)
        {
            case E_MHAL_AUDIO_I2S_FMT_I2S:
                tI2sCfg.eFormat= AUD_I2S_FMT_I2S;
                break;
            case E_MHAL_AUDIO_I2S_FMT_LEFT_JUSTIFY:
                tI2sCfg.eFormat= AUD_I2S_FMT_LEFT_JUSTIFY;
                break;
            default:
                printf("in %s, I2S format %d  error\n",__FUNCTION__,pstI2sConfig->eFmt);
                return MHAL_FAILURE;
        }

        switch(pstI2sConfig->e4WireMode)
        {
            case E_MHAL_AUDIO_4WIRE_ON:
                tI2sCfg.eWireMode= AUD_I2S_WIRE_4;
                break;
            case E_MHAL_AUDIO_4WIRE_OFF:
                tI2sCfg.eWireMode= AUD_I2S_WIRE_6;
                break;
            default:
                printf("in %s, I2S wire mode %d  error\n",__FUNCTION__,pstI2sConfig->e4WireMode);
                return MHAL_FAILURE;
        }

        switch(pstI2sConfig->eMck)
        {
            case E_MHAL_AUDIO_MCK_NULL:
                tI2sCfg.eMck= AUD_I2S_MCK_NULL;
                break;
            case E_MHAL_AUDIO_MCK_12_288M:
                tI2sCfg.eMck= AUD_I2S_MCK_12_288M;
                break;
            case E_MHAL_AUDIO_MCK_16_384M:
                tI2sCfg.eMck= AUD_I2S_MCK_16_384M;
                break;
            case E_MHAL_AUDIO_MCK_18_432M:
                tI2sCfg.eMck= AUD_I2S_MCK_18_432M;
                break;
            case E_MHAL_AUDIO_MCK_24_576M:
                tI2sCfg.eMck= AUD_I2S_MCK_24_576M;
                break;
            case E_MHAL_AUDIO_MCK_24M:
                tI2sCfg.eMck= AUD_I2S_MCK_24M;
                break;
            case E_MHAL_AUDIO_MCK_48M:
                tI2sCfg.eMck= AUD_I2S_MCK_48M;
                break;
            default:
                printf("in %s, I2S Mck %d  error\n",__FUNCTION__,pstI2sConfig->eMck);
                return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
        }

        switch(pstI2sConfig->eRate)
        {
            case E_MHAL_AUDIO_RATE_8K:
                tI2sCfg.eRate= AUD_RATE_8K;
                break;
            case E_MHAL_AUDIO_RATE_16K:
                tI2sCfg.eRate= AUD_RATE_16K;
                break;
            case E_MHAL_AUDIO_RATE_32K:
                tI2sCfg.eRate= AUD_RATE_32K;
                break;
            case E_MHAL_AUDIO_RATE_48K:
                tI2sCfg.eRate= AUD_RATE_48K;
                break;
            default:
                printf("in %s, I2S Sample Rate %d  error\n",__FUNCTION__,pstI2sConfig->eRate);
                return MHAL_FAILURE;
        }

        //
        switch(pstI2sConfig->eMode)
        {
            case E_MHAL_AUDIO_MODE_I2S_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                break;
            case E_MHAL_AUDIO_MODE_I2S_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_I2S;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
                break;
            case E_MHAL_AUDIO_MODE_TDM_MASTER:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_MASTER;
                if(pstI2sConfig->u16Channels!=4 && pstI2sConfig->u16Channels!=8)
                {
                    printf("in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;
                }
                break;
            case E_MHAL_AUDIO_MODE_TDM_SLAVE:
                tI2sCfg.eMode = AUD_I2S_MODE_TDM;
                tI2sCfg.eMsMode = AUD_I2S_MSMODE_SLAVE;
                if(pstI2sConfig->u16Channels!=4 && pstI2sConfig->u16Channels!=8)
                {
                    printf("in %s, channel %u  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
                    return MHAL_FAILURE;
                }
                break;
        }

        //
        if ( ( pstI2sConfig->u16Channels % 2 ) != 0 )
        {
            printf("in %s, pstI2sConfig->u16Channels = %d  error\n",__FUNCTION__,pstI2sConfig->u16Channels);
            return MHAL_FAILURE;
        }
        else
        {
            tI2sCfg.nChannelNum = pstI2sConfig->u16Channels;
        }

        //
        for(i=0; i<num_mux_config_i2s; i++)
        {
            bRet &= DrvAudSetMux(mux_list_for_config_i2s[i][0], mux_list_for_config_i2s[i][1]);
        }

        //
        tI2sCfg.pPriData = i2sIf_cfg_private_data;

        //
        bRet &= DrvAudConfigI2s((AudI2sIf_e)i2s_if, &tI2sCfg);

        if(!bRet)
        {
            return MHAL_FAILURE;
        }
    }
    else
    {
        return MHAL_FAILURE;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Config audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig)
{
    MS_BOOL bRet = TRUE;
    DmaParam_t tParam;
    int i = 0;
    int dev = 0, dma = 0;
    u16 nBitWidth;
    // Mux
    const int (*mux_list_for_config_pcm)[2];
    int num_mux_config_pcm = 0;
    // Mch
    BOOL use_mch;
    const struct audio_info_mch *info_mch;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;
    num_mux_config_pcm = card.ao_devs[dev].info->num_mux_config_pcm;
    mux_list_for_config_pcm = card.ao_devs[dev].info->mux_list_for_config_pcm;
    use_mch = card.ao_devs[dev].info->use_mch;
    info_mch = card.ao_devs[dev].info->info_mch;

    //
    switch(pstDmaConfig->eWidth)
    {
        case E_MHAL_AUDIO_BITWIDTH_16:
            nBitWidth = 16;
            break;
        case E_MHAL_AUDIO_BITWIDTH_32:
            nBitWidth = 32;
            break;
        default:
            return E_MHAL_ERR_ILLEGAL_PARAM;
    }

    tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
    tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
    tParam.nBufferSize = pstDmaConfig->u32BufferSize;
    tParam.nChannels = pstDmaConfig->u16Channels;
    tParam.nBitWidth = nBitWidth;
    tParam.nSampleRate = pstDmaConfig->eRate;
    tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
    tParam.nStartThres = pstDmaConfig->u32StartThres;
    tParam.nInterleaved = pstDmaConfig->bInterleaved;

    if ( use_mch == TRUE )
    {
        tParam.private = (void*)info_mch;
    }
    else
    {
        tParam.private = NULL;
    }

    if ( pstDmaConfig->u16Channels == 1 )
    {
        bRet &= DrvAudMonoEnable( dma, TRUE );
    }
    else
    {
        bRet &= DrvAudMonoEnable( dma, FALSE );
    }

    for ( i = 0; i < num_mux_config_pcm; i++ )
    {
        bRet &= DrvAudSetMux( mux_list_for_config_pcm[i][0], mux_list_for_config_pcm[i][1] );
    }

    bRet &= DrvAudConfigDmaParam( dma, &tParam );

    //
    if ( bRet )
    {
        memcpy( &_astOutPcmCfg[AoutDevId],pstDmaConfig,sizeof(MHAL_AUDIO_PcmCfg_t) );
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;
    }
}

/**
* \brief Config audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ConfigPcmIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_PcmCfg_t *pstDmaConfig)
{
    MS_BOOL bRet = TRUE;
    DmaParam_t tParam;
    DigMicParam_t tDigMicParam;
    AecParam_t tAecParam;
    int i = 0;
    int dev = 0, dma = 0;
    u16 nBitWidth;
    // Mux
    const int (*mux_list_for_config_pcm)[2];
    int num_mux_config_pcm = 0;
    // Mch
    BOOL use_mch;
    const struct audio_info_mch *info_mch;
    // Dmic
    BOOL use_dmic;
    // Atop
    BOOL use_atop;
    int adc_sel;
    // AEC
    BOOL use_aec;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;
    num_mux_config_pcm = card.ai_devs[dev].info->num_mux_config_pcm;
    mux_list_for_config_pcm = card.ai_devs[dev].info->mux_list_for_config_pcm;
    use_mch = card.ai_devs[dev].info->use_mch;
    info_mch = card.ai_devs[dev].info->info_mch;
    use_dmic = card.ai_devs[dev].info->use_dmic;
    use_atop = card.ai_devs[dev].info->use_atop;
    adc_sel = card.ai_devs[dev].info->adc_sel;
    use_aec = card.ai_devs[dev].info->use_aec;

    //
    switch(pstDmaConfig->eWidth)
    {
        case E_MHAL_AUDIO_BITWIDTH_16:
            nBitWidth = 16;
            break;
        case E_MHAL_AUDIO_BITWIDTH_32:
            nBitWidth = 32;
            break;
        default:
            return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
    }

    //
    tParam.pDmaBuf = pstDmaConfig->pu8DmaArea;
    tParam.nPhysDmaAddr = pstDmaConfig->phyDmaAddr;//!!!MIU_OFFSET
    tParam.nBufferSize = pstDmaConfig->u32BufferSize;
    tParam.nChannels = pstDmaConfig->u16Channels;
    tParam.nPeriodSize = pstDmaConfig->u32PeriodSize;
    tParam.nBitWidth = nBitWidth;
    tParam.nSampleRate = pstDmaConfig->eRate;
    tParam.nStartThres = 1;
    tParam.nInterleaved = pstDmaConfig->bInterleaved;

    //
    tDigMicParam.nChannels = pstDmaConfig->u16Channels;
    tDigMicParam.nBitWidth = nBitWidth;
    tDigMicParam.nSampleRate = pstDmaConfig->eRate;

    //
    tAecParam.nSampleRate = pstDmaConfig->eRate;

    //
    if(pstDmaConfig->u16Channels!=1 && pstDmaConfig->u16Channels!=2 &&
        pstDmaConfig->u16Channels!=4 && pstDmaConfig->u16Channels!=8)
    {
        printf("In function %s, Device Id = %d, Channel not support !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
    }

    //
    if(tParam.nBitWidth != 16)
    {
        printf("In function %s, Device Id = %d, BitWidth not support !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;//E_MHAL_ERR_ILLEGAL_PARAM;
    }

    //
    if ( use_mch == TRUE )
    {
        tParam.private = (void*)info_mch;
    }
    else
    {
        tParam.private = NULL;
    }

    //
    if ( use_atop == TRUE )
    {
        if ( adc_sel != AUD_ADC_SEL_NULL )
        {
            bRet &= DrvAudAdcSetMux((AudAdcSel_e)adc_sel);
        }
    }

    //
    if(pstDmaConfig->u16Channels == 1)
    {
        bRet &= DrvAudMonoEnable(dma, TRUE);
    }
    else
    {
        bRet &= DrvAudMonoEnable(dma, FALSE);
    }

    //
    for ( i = 0; i < num_mux_config_pcm; i++ )
    {
        bRet &= DrvAudSetMux( mux_list_for_config_pcm[i][0], mux_list_for_config_pcm[i][1] );
    }

    //
    if ( use_dmic == TRUE )
    {
        bRet &= DrvAudConfigDigMicParam(&tDigMicParam);
    }

    //
    if ( use_aec == TRUE )
    {
        bRet &= DrvAudConfigAecParam(&tAecParam);
    }

    //
    bRet &= DrvAudConfigDmaParam(dma, &tParam);

    //
    if(bRet)
    {
        memcpy(&_astInPcmCfg[AinDevId],pstDmaConfig,sizeof(MHAL_AUDIO_PcmCfg_t));
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Open audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    // Atop
    int atop_path = 0;
    BOOL use_atop;
    BOOL use_hdmi;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;
    atop_path = card.ao_devs[dev].info->atop_path;
    i2s_if = card.ao_devs[dev].info->i2s_if;
    use_atop = card.ao_devs[dev].info->use_atop;
    use_i2s = card.ao_devs[dev].info->use_i2s;
    use_hdmi = card.ao_devs[dev].info->use_hdmi;

    //
    bRet &= DrvAudOpenDma( (AudDmaChn_e)dma );

    if ( use_atop == TRUE )
    {
        bRet &= DrvAudEnableAtop( atop_path, TRUE );
    }

    if ( use_i2s == TRUE )
    {
        bRet &= DrvAudEnableI2s( i2s_if ,TRUE );
    }

    if ( use_hdmi == TRUE )
    {
        bRet &= DrvAudEnableHdmi( TRUE );
    }

    bRet &= DrvAudPrepareDma( (AudDmaChn_e)dma );

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;
    }
}

/**
* \brief Open audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_OpenPcmIn(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    // Atop
    int atop_path = 0;
    BOOL use_atop;
    // Dmic
    BOOL use_dmic;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;
    use_i2s = card.ai_devs[dev].info->use_i2s;
    i2s_if = card.ai_devs[dev].info->i2s_if;
    use_dmic = card.ai_devs[dev].info->use_dmic;
    use_atop = card.ai_devs[dev].info->use_atop;
    atop_path = card.ai_devs[dev].info->atop_path;

    // If AMIC & DMIC use the same DMA, we must do HalAudDigMicEnable() before HalAudDmaSetRate() for gain compensation.
    if ( use_dmic == TRUE )
    {
        bRet &= DrvAudDigMicEnable(TRUE);
    }

    //
    bRet &= DrvAudOpenDma( (AudDmaChn_e)dma );

    if ( use_atop == TRUE )
    {
        bRet &= DrvAudEnableAtop( atop_path, TRUE );
    }

    if ( use_i2s == TRUE )
    {
        bRet &= DrvAudEnableI2s( i2s_if ,TRUE );
    }

    bRet &= DrvAudPrepareDma( (AudDmaChn_e)dma );

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}


/**
* \brief Close audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    // Atop
    int atop_path = 0;
    BOOL use_atop;
    BOOL use_hdmi;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;
    atop_path = card.ao_devs[dev].info->atop_path;
    i2s_if = card.ao_devs[dev].info->i2s_if;
    use_atop = card.ao_devs[dev].info->use_atop;
    use_i2s = card.ao_devs[dev].info->use_i2s;
    use_hdmi = card.ao_devs[dev].info->use_hdmi;

    //
    bRet &= DrvAudCloseDma( (AudDmaChn_e)dma );

    if ( use_atop == TRUE )
    {
        bRet &= DrvAudEnableAtop( atop_path, FALSE );
    }

    if ( use_i2s == TRUE )
    {
        bRet &= DrvAudEnableI2s( i2s_if ,FALSE );
    }

    if ( use_hdmi == TRUE )
    {
        bRet &= DrvAudEnableHdmi( FALSE );
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;
    }
}

/**
* \brief Close audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ClosePcmIn(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    // I2S
    int i2s_if = 0;
    BOOL use_i2s;
    // Atop
    int atop_path = 0;
    BOOL use_atop;
    // Dmic
    BOOL use_dmic;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;
    use_i2s = card.ai_devs[dev].info->use_i2s;
    i2s_if = card.ai_devs[dev].info->i2s_if;
    use_dmic = card.ai_devs[dev].info->use_dmic;
    use_atop = card.ai_devs[dev].info->use_atop;
    atop_path = card.ai_devs[dev].info->atop_path;

    //
    if ( use_dmic == TRUE )
    {
        bRet &= DrvAudDigMicEnable(FALSE);
    }

    //
    bRet &= DrvAudCloseDma( (AudDmaChn_e)dma );

    if ( use_atop == TRUE )
    {
        bRet &= DrvAudEnableAtop( atop_path, FALSE );
    }

    if ( use_i2s == TRUE )
    {
        bRet &= DrvAudEnableI2s( i2s_if ,FALSE );
    }

    if(bRet)
    {
        return MHAL_SUCCESS;
    }
    else
    {
        return MHAL_FAILURE;//E_MHAL_ERR_NOT_SUPPORT;
    }
}

/**
* \brief Start audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    //
    bRet &= DrvAudStartDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Start audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StartPcmIn(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    //
    bRet &= DrvAudStartDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Stop audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    //
    bRet &= DrvAudStopDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Stop audio PCM input device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_StopPcmIn(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    //
    bRet &= DrvAudStopDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Pause audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_PausePcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    //
    bRet &= DrvAudPauseDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}


/**
* \brief Resume audio PCM output device
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_ResumePcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    //
    bRet &= DrvAudResumeDma( dma );

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}


/**
* \brief Read audio raw data from PCM input device(period unit)
* \return value>0 => read data bytes, value<0 => error number, -2 => empty
*/
//MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_AiDev_e AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock, MS_U64 *pu64AiPts )
MS_S32 MHAL_AUDIO_ReadDataIn(MHAL_AUDIO_AiDev_e AinDevId, VOID *pRdBuffer, MS_U32 u32Size, MS_BOOL bBlock )
{
    MS_S32 s32Err;
    int dev = 0, dma = 0;
    // Temp
    MS_U64 *pu64AiPts;
    MS_U64 u64AiPts = 0;
    pu64AiPts = &u64AiPts;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    //
    dma = card.ai_devs[dev].info->dma;

    s32Err = DrvAudReadPcm(dma, pRdBuffer, u32Size, bBlock, pu64AiPts);

    if(s32Err>=0)
    {
        return s32Err;
    }
    else
    {
        if ( s32Err == -EAGAIN )
        {
            return -2;
        }

        return MHAL_FAILURE;
    }
}

/**
* \brief Write audio raw data to PCM output device(period unit)
* \return value>0 => write data bytes, value<0 => error number, -2 => full
*/
MS_S32 MHAL_AUDIO_WriteDataOut(MHAL_AUDIO_AoDev_e AoutDevId, VOID *pWrBuffer, MS_U32 u32Size, MS_BOOL bBlock)
{
    MS_S32 s32Err;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    //
    dma = card.ao_devs[dev].info->dma;

    s32Err = DrvAudWritePcm(dma, pWrBuffer, u32Size, bBlock);

    if (s32Err >= 0)
    {
        return s32Err;
    }
    else
    {
        if ( s32Err == -EAGAIN )
        {
            return -2;
        }

        return MHAL_FAILURE;
    }
}

/**
* \brief Get PCM device status(empty or not, if empty, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmOutXrun(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    //
    bRet &= (DrvAudIsXrun(dma)?TRUE:FALSE);
    //if(bRet)
    //    DrvAudPrepareDma(dma);

    return bRet;
}

/**
* \brief Get PCM device status(full or not, if full, reset status automatically)
* \return 1=> xun , 0=>normal
*/
MS_BOOL MHAL_AUDIO_IsPcmInXrun(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    //
    bRet &= (DrvAudIsXrun(dma)?TRUE:FALSE);
    if(bRet)
        DrvAudPrepareDma(dma);

    return bRet;
}

/**
* \brief you can restart PcmOut (call MHAL_AUDIO_StartPcmOut), after this function is called.
* \return 0 => success, <0 => error number
*/
MS_BOOL MHAL_AUDIO_PrepareToRestartPcmOut(MHAL_AUDIO_AoDev_e AoutDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    bRet &= DrvAudStopDma(dma);
    bRet &= DrvAudPrepareDma(dma);

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief you can restart PcmIn (call MHAL_AUDIO_StartPcmIn), after this function is called.
* \return 0 => success, <0 => error number
*/
MS_BOOL MHAL_AUDIO_PrepareToRestartPcmIn(MHAL_AUDIO_AiDev_e AinDevId)
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    bRet &= DrvAudStopDma(dma);
    bRet &= DrvAudPrepareDma(dma);

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Set output path gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainOut(MHAL_AUDIO_AoDev_e AoutDevId, MS_S16 s16Gain, MS_S8 s8Ch, MHAL_AUDIO_GainFading_e eFading)
{
    int i = 0;
    int dev = 0;
    int num_dpga = 0;
    const int *dpga_list;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    num_dpga = card.ao_devs[dev].info->num_dpga;
    dpga_list = card.ao_devs[dev].info->dpga_list;

    //
    if ( num_dpga <= 0 || dpga_list == NULL )
    {
        printf("In function %s, Device Id = %d, DPGA not support !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    // Set gain fading before setting gain value.
    for ( i = 0; i < num_dpga; i++ )
    {
        if ( !DrvAudDpgaSetGainFading( dpga_list[i], (U8)eFading, s8Ch ) )
        {
            return MHAL_FAILURE;
        }
    }

    for ( i = 0; i < num_dpga; i++ )
    {
        if ( !DrvAudDpgaSetGain( dpga_list[i], (S8)s16Gain, s8Ch ) )
        {
            return MHAL_FAILURE;
        }
    }

    return MHAL_SUCCESS;
}

/**
* \brief Set input path gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetGainIn(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch)
{
    int i = 0;
    int dev = 0;
    int num_dpga = 0;
    const int *dpga_list;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    num_dpga = card.ai_devs[dev].info->num_dpga;
    dpga_list = card.ai_devs[dev].info->dpga_list;

    //
    if ( num_dpga <= 0 || dpga_list == NULL)
    {
        printf("In function %s, Device Id = %d, DPGA not support !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    for ( i = 0; i < num_dpga; i++ )
    {
        if ( !DrvAudDpgaSetGain( dpga_list[i], (S8)s16Gain, s8Ch ) )
        {
            return MHAL_FAILURE;
        }
    }

    return MHAL_SUCCESS;
}

/**
* \brief Set ADC gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetAdcGain(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch)
{
    int dev = 0;
    // Atop
    BOOL use_atop;
    U16 adc_gain = 0;
    U16 pre_gain = 0;
    int adc_sel;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    use_atop = card.ai_devs[dev].info->use_atop;
    adc_sel = card.ai_devs[dev].info->adc_sel;

    if(s16Gain > 21 || s16Gain < 0)
    {
        printf("%s unsupported Gain (0~21)\n",__FUNCTION__);
        return MHAL_FAILURE;
    }

    adc_gain = _gaMicInCombineGainTable[s16Gain][0];
    pre_gain = _gaMicInCombineGainTable[s16Gain][1];

    if(use_atop)
    {
        switch( adc_sel )
        {
            case AUD_ADC_SEL_MICIN:
                if ( !DrvAudSetAdcGain(AUD_ADC_SEL_MICIN, adc_gain, s8Ch) ) //0~9 : -6dB~21dB
                {
                    return MHAL_FAILURE;
                }
                if ( !DrvAudSetMicAmpGain(AUD_ADC_SEL_MICIN, pre_gain, s8Ch) ) //0:0dB, 1:6dB, 2:13dB, 3:23dB, 4:30dB, 5:36dB
                {
                    return MHAL_FAILURE;
                }
                break;

            case AUD_ADC_SEL_MICIN2:
                if ( !DrvAudSetAdcGain(AUD_ADC_SEL_MICIN2, adc_gain, s8Ch) ) //0~9 : -6dB~21dB
                {
                    return MHAL_FAILURE;
                }
                if ( !DrvAudSetMicAmpGain(AUD_ADC_SEL_MICIN2, pre_gain, s8Ch) ) //0:0dB, 1:6dB, 2:13dB, 3:23dB, 4:30dB, 5:36dB
                {
                    return MHAL_FAILURE;
                }
                break;

            case AUD_ADC_SEL_MICIN_4CH:
                if ( !DrvAudSetAdcGain(AUD_ADC_SEL_MICIN_4CH, adc_gain, s8Ch) ) //0~9 : -6dB~21dB
                {
                    return MHAL_FAILURE;
                }
                if ( !DrvAudSetMicAmpGain(AUD_ADC_SEL_MICIN_4CH, pre_gain, s8Ch) ) //0:0dB, 1:6dB, 2:13dB, 3:23dB, 4:30dB, 5:36dB
                {
                    return MHAL_FAILURE;
                }
                break;

            case AUD_ADC_SEL_LINEIN:
                if ( !DrvAudSetAdcGain(AUD_ADC_SEL_LINEIN, adc_gain, s8Ch) ) //0~7 : -6dB~15dB
                {
                    return MHAL_FAILURE;
                }
                break;

            default:
                return MHAL_FAILURE;
                break;
        }
    }
    else
    {
        return MHAL_FAILURE;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Set DMIC gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetDmicGain(MHAL_AUDIO_AiDev_e AinDevId, MS_S16 s16Gain, MS_S8 s8Ch)
{
    int dev = 0;
    BOOL use_dmic;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    use_dmic = card.ai_devs[dev].info->use_dmic;

    //
    if ( use_dmic == TRUE )
    {
        if ( !DrvAudSetDigMicGain((S8)s16Gain, s8Ch) )
        {
            return MHAL_FAILURE;
        }
    }
    else
    {
        return MHAL_FAILURE;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Set src path gain
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_SetSrcGain(MHAL_AUDIO_AoDev_e AoutDevId, MS_S16 s16Gain, MS_S8 s8Ch)
{
    int dev;
    BOOL use_src_dpga;
    int src_dpga;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    use_src_dpga = card.ao_devs[dev].info->use_src_dpga;
    src_dpga = card.ao_devs[dev].info->src_dpga;

    //
    if ( use_src_dpga == TRUE )
    {
        if ( !DrvAudDpgaSetGain( src_dpga, (S8)s16Gain, s8Ch ) )
        {
            return MHAL_FAILURE;
        }
    }
    else
    {
        printf("In function %s, Device Id = %d, DPGA not support !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    return MHAL_SUCCESS;
}

/**
* \brief Get Ao dma buf current data length ( bytes ).
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmOutCurrDataLen( MHAL_AUDIO_AoDev_e AoutDevId, MS_U32 *len )
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    bRet &= DrvAudGetBufCurrDataLen(dma, len);

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Get Ai dma buf current data length ( bytes ).
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmInCurrDataLen( MHAL_AUDIO_AiDev_e AinDevId, MS_U32 *len )
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    bRet &= DrvAudGetBufCurrDataLen(dma, len);

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \brief Get Ao Start Pts.
* \return 0 => success, <0 => error number
*/
MS_S32 MHAL_AUDIO_GetPcmOutStartPts(MHAL_AUDIO_AoDev_e AoutDevId, MS_U64 *pu64AoStartPts )
{
    MS_BOOL bRet = TRUE;
    MS_U64 u64TempPts;
    int dev = 0, dma = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    bRet &= DrvAudGetStartPts(dma, &u64TempPts);

    *pu64AoStartPts = u64TempPts;

    return (bRet?MHAL_SUCCESS:MHAL_FAILURE);
}

/**
* \enable AO SINEGEN, SampleRate: 48K.
*/
MS_S32 MHAL_AUDIO_DmaSineGenOut(MHAL_AUDIO_AoDev_e AoutDevId, MHAL_AUDIO_SineGenCfg_t SineGen )
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    BOOL enable = FALSE;
    U8 freq = 0, gain = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AoutDevId;

    if ( dev >= card.num_ao_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ao_devs[dev].info->dma;

    enable = SineGen.bEnable;
    freq = SineGen.u8Freq;
    gain = SineGen.u8Gain;

    bRet &= DrvAudDmaSineGen(dma, enable, freq, gain);

    if (bRet)
    {
        return MHAL_SUCCESS;
    }

    return MHAL_FAILURE;
}

/**
* \enable AI SINEGEN, SampleRate: 48K.
*/
MS_S32 MHAL_AUDIO_DmaSineGenIn(MHAL_AUDIO_AiDev_e AinDevId, MHAL_AUDIO_SineGenCfg_t SineGen )
{
    MS_BOOL bRet = TRUE;
    int dev = 0, dma = 0;
    BOOL enable = FALSE;
    U8 freq = 0, gain = 0;

    // ------------------------
    // Get Pre info.
    // ------------------------
    dev = (int)AinDevId;

    if ( dev >= card.num_ai_devs )
    {
        printf("In function %s, Device Id = %d, Error !\n",__FUNCTION__, dev);
        return MHAL_FAILURE;
    }

    dma = card.ai_devs[dev].info->dma;

    enable = SineGen.bEnable;
    freq = SineGen.u8Freq;
    gain = SineGen.u8Gain;

    bRet &= DrvAudDmaSineGen(dma, enable, freq, gain);

    if (bRet)
    {
        return MHAL_SUCCESS;
    }

    return MHAL_FAILURE;
}

MS_BOOL MHAL_AUDIO_AmpEnable(MS_BOOL bEnable, MS_S8 s8Ch)
{
    MS_BOOL bRet = TRUE;

    //printf("in %s --- Enable:%d\n",__FUNCTION__, bEnable);

    bRet &= DrvAudEnableAmp(bEnable, s8Ch);

    if (bRet)
    {
        return MHAL_SUCCESS;
    }

    return MHAL_FAILURE;
}
