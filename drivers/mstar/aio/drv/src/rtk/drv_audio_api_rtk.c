/*
* drv_audio_api_rtk.c- Sigmastar
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

//
#include "cam_os_wrapper.h"
#include "sys_MsWrapper_cus_os_int_ctrl.h"
#include "hal_int_ctrl_pub.h"
#include "mdrv_gpio_io.h"

// Hal
#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"
#include "hal_audio_reg.h"

// Drv
#include "drv_audio_dbg.h"
#include "drv_audio_api.h"
#include "drv_audio_pri.h"



// ------------------------
// DTS
// ------------------------
#include "rtk_audio_dts_st.h"

#include "rtk_bach_board_cfg.c"




// ------------------------
// IRQ
// ------------------------
// Follow Linux's enum
enum irqreturn {
	IRQ_NONE		= (0 << 0),
	IRQ_HANDLED		= (1 << 0),
	IRQ_WAKE_THREAD = (1 << 1),
};

typedef enum irqreturn irqreturn_t;

//
typedef struct
{
    BOOL IsActive;
    void *dev_id;

} RtkIrqInfo_t;

typedef struct
{
    int rtkIrqReqCnt;
    RtkIrqInfo_t rtkIrqInfo[AUD_DMA_NUM];
    CamOsSpinlock_t rtkIrqLock;

} RtkIrqHandle_t;



// ------------------------
// Default Value
// ------------------------
#define AUD_DEF_IRQ_ID      MS_INT_NUM_IRQ_BACH
#define AUD_DEF_AMP_GPIO    { PAD_UNKNOWN, 0, PAD_UNKNOWN, 0 }



// ------------------------
// Variable
// ------------------------
static unsigned int INFINITY_IRQ_ID = AUD_DEF_IRQ_ID;
static U32 _gaAmpGpio[4] = AUD_DEF_AMP_GPIO;
static DmaStopChannelCbFn _gpDmaStopChannelCbFn = NULL;

// irq
static RtkIrqHandle_t _gtRtkIrqHandle;



// ------------------------
// Static declaration
// ------------------------
static void _AudDmaIrqRtk(u32 irq, void *pri);
static irqreturn_t _AudDmaIrqRtkCore(int irq, void *dev_id);



static void _AudDmaIrqRtk(u32 irq, void *pri)
{
    int i = 0;

    CamOsSpinLockIrqSave(&_gtRtkIrqHandle.rtkIrqLock);

    for ( i = 0; i < AUD_DMA_NUM; i++ )
    {
        if ((_gtRtkIrqHandle.rtkIrqInfo[i].IsActive == TRUE) && (_gtRtkIrqHandle.rtkIrqInfo[i].dev_id != NULL ))
        {
            _AudDmaIrqRtkCore( 0, (void *)_gtRtkIrqHandle.rtkIrqInfo[i].dev_id);
        }
    }

    CamOsSpinUnlockIrqRestore(&_gtRtkIrqHandle.rtkIrqLock);
    return;
}

static irqreturn_t _AudDmaIrqRtkCore(int irq, void *dev_id)
{
    PcmRuntimeData_t *ptRuntimeData = (PcmRuntimeData_t*)dev_id;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)ptRuntimeData->pPrivateData;
    //unsigned long nFlags;
    BOOL bTrigger=0, bBoundary=0, bLocalData=0;
    BOOL bTriggerInt=0, bBoundaryInt=0;//,bLocalDataInt;

    CamOsSpinLockIrqSave(&ptDmaData->tLock);

    HalAudDmaGetFlags(ptDmaData->nChnId, &bTrigger, &bBoundary, &bLocalData);
    HalAudDmaGetInt(ptDmaData->nChnId, &bTriggerInt, &bBoundaryInt);

    if ( ( ( bTrigger == FALSE ) || ( bTriggerInt == FALSE ) )
        && ( ( bBoundary == FALSE ) || ( bBoundaryInt == FALSE ) )
        //&& ( ( bLocalData == FALSE ) || ( bLocalDataInt == FALSE ) )
        )
    {
        CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
        return IRQ_NONE;
    }

    if ((PCM_STREAM_PLAYBACK == ptRuntimeData->eStream) && ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING)
    {
        if (ptRuntimeData->nState != DMA_EMPTY)
        {
            if (bBoundary==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;
                ptRuntimeData->nState = DMA_EMPTY;

                AUD_PRINTF(IRQ_LEVEL, "EMPTY: DMA Id = %d\n",  ptDmaData->nChnId);
            }
            else if ((ptRuntimeData->nState != DMA_UNDERRUN) && bTrigger==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, TRUE);

                ptRuntimeData->nState  = DMA_UNDERRUN;
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);
                AUD_PRINTF(IRQ_LEVEL, "UNDER: DMA Id = %d, LC = %u\n", ptDmaData->nChnId, HalAudDmaGetLevelCnt(ptDmaData->nChnId));
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL, "AO UNKNOWN interrupt %d %d %d\n",bTrigger,bBoundary,bLocalData);
            }
        }
    }
    else if ((PCM_STREAM_CAPTURE == ptRuntimeData->eStream) && (ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING))
    {
        if (ptRuntimeData->nState != DMA_FULL)
        {
            if (bBoundary==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;

                AUD_PRINTF(IRQ_LEVEL, "FULL: DMA Id = %d\n", ptDmaData->nChnId);

                ptRuntimeData->nState = DMA_FULL;
            }
            else if ((ptRuntimeData->nState != DMA_OVERRUN) && bTrigger==TRUE)
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, TRUE);

                ptRuntimeData->nState = DMA_OVERRUN;
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);

                AUD_PRINTF(IRQ_LEVEL, "OVER: DMA Id = %d, LC = %u\n", ptDmaData->nChnId, HalAudDmaGetLevelCnt(ptDmaData->nChnId));
            }
            else
                AUD_PRINTF(ERROR_LEVEL, "AI UNKNOWN interrupt %d %d %d\n",bTrigger,bBoundary,bLocalData);
        }
    }

    AUD_PRINTF(IRQ_LEVEL, "Function - %s, DMA Id = %d, cur state = %d\n", __func__, ptDmaData->nChnId, ptRuntimeData->nState);
    AUD_PRINTF(IRQ_LEVEL, "bTrigger = %d   bTriggerInt = %d\r\n",bTrigger,bTriggerInt);
    AUD_PRINTF(IRQ_LEVEL, "bBoundary = %d  bBoundaryInt = %d\r\n",bBoundary,bBoundaryInt);
    //AUD_PRINTF(IRQ_LEVEL, "bLocalData = %d bLocalDataInt = %d\r\n",bLocalData,bLocalDataInt);

    CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

    return IRQ_HANDLED;
}

void DrvAudApiDtsInit(void)
{
    int ret, i = 0;
    S32 nPadMuxMode, nKeepI2sClk = 0;
    S32 nKeepAdcPowerOn=0;
    S32 nKeepDacPowerOn=0;
    S32 nI2sPcmMode=0;
    const char *dts_compatible_str = NULL, *dts_amp_gpio_str = NULL, *dts_keep_i2s_clk_str = NULL;
    const char *dts_keep_adc_power_on = NULL, *dts_keep_dac_power_on = NULL, *dts_i2s_pcm_mode_str = NULL;
    // ------------------------
    // Get Pre info.
    // ------------------------
    if ( card.dts->audio_dts_compatible_str != NULL )
    {
        dts_compatible_str = card.dts->audio_dts_compatible_str;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_compatible_str = NULL !\n",__FUNCTION__);
        return;
    }

    if ( card.dts->audio_dts_amp_gpio_str != NULL )
    {
        dts_amp_gpio_str = card.dts->audio_dts_amp_gpio_str;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_amp_gpio_str = NULL !\n",__FUNCTION__);
        return;
    }

    if ( card.dts->audio_dts_keep_i2s_clk_str != NULL )
    {
        dts_keep_i2s_clk_str = card.dts->audio_dts_keep_i2s_clk_str;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_keep_i2s_clk_str = NULL !\n",__FUNCTION__);
        return;
    }
    if ( card.dts->audio_dts_keep_adc_power_on != NULL )
    {
        dts_keep_adc_power_on = card.dts->audio_dts_keep_adc_power_on;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_keep_adc_power_on = NULL !\n",__FUNCTION__);
        return;
    }
    if ( card.dts->audio_dts_keep_dac_power_on != NULL )
    {
        dts_keep_dac_power_on = card.dts->audio_dts_keep_dac_power_on;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_keep_dac_power_on = NULL !\n",__FUNCTION__);
        return;
    }
    if ( card.dts->audio_dts_i2s_pcm_str != NULL )
    {
        dts_i2s_pcm_mode_str = card.dts->audio_dts_i2s_pcm_str;
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, audio_dts_i2s_pcm_str = NULL !\n",__FUNCTION__);
        return;
    }
    //
    if(_AudRtkDtsFindCompatibleNode(dts_compatible_str) == TRUE)
    {
        // IRQ
        INFINITY_IRQ_ID = _AudRtkDtsGetIrqId();

        //  enable for line-out
        if ( _AudRtkDtsGetGpio(dts_amp_gpio_str, _gaAmpGpio) != TRUE )
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request amp-gpio !\n",__FUNCTION__);
            AUD_PRINTF(ERROR_LEVEL,"%s, amp-gpio chn0 will use default:%d %d !\n",__FUNCTION__, _gaAmpGpio[0], _gaAmpGpio[1]);
            AUD_PRINTF(ERROR_LEVEL,"%s, amp-gpio chn1 will use default:%d %d !\n",__FUNCTION__, _gaAmpGpio[2], _gaAmpGpio[3]);
        }
        else
        {
            if (_gaAmpGpio[0] == PAD_UNKNOWN)
            {
                // Do nothing
                AUD_PRINTF(DEBUG_LEVEL,"AmpGpio chn0 = PAD_UNKNOWN, don't do CamGpioRequest !\n");
            }
            else
            {
                ret = camdriver_gpio_request(NULL, _gaAmpGpio[0]);
                if(ret)
                {
                    AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request amp-gpio chn0 !\n",__FUNCTION__);
                }
            }

            if (_gaAmpGpio[2] == PAD_UNKNOWN)
            {
                // Do nothing
                AUD_PRINTF(DEBUG_LEVEL,"AmpGpio chn1 = PAD_UNKNOWN, don't do CamGpioRequest !\n");
            }
            else
            {
                ret = camdriver_gpio_request(NULL, _gaAmpGpio[2]);
                if(ret)
                {
                    AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request amp-gpio chn1 !\n",__FUNCTION__);
                }
            }
        }

        for ( i = 0; i < card.dts->num_padmux; i++ )
        {
            if (_AudRtkDtsGetPadMux(card.dts->audio_dts_padmux_list[i].str, &nPadMuxMode) == TRUE)
            {
                card.dts->audio_dts_padmux_list[i].fp(nPadMuxMode);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s !\n",__FUNCTION__, card.dts->audio_dts_padmux_list[i].str);
                AUD_PRINTF(ERROR_LEVEL,"%s, %s will use default !\n",__FUNCTION__, card.dts->audio_dts_padmux_list[i].str);
            }
        }

        if (_AudRtkDtsGetKeepI2sClk(dts_keep_i2s_clk_str, &nKeepI2sClk) == TRUE)
        {
            HalAudI2sSetKeepClk(nKeepI2sClk);
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, dts_keep_i2s_clk_str);
        }
        
        if (_AudRtkDtsGetKeepAdcPowerOn(dts_keep_adc_power_on, &nKeepAdcPowerOn) == TRUE)
        {
            HalAudKeepAdcPowerOn((U8)nKeepAdcPowerOn);
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, dts_keep_adc_power_on);
        }

        if (_AudRtkDtsGetKeepDacPowerOn(dts_keep_dac_power_on, &nKeepDacPowerOn) == TRUE)
        {
            HalAudKeepDacPowerOn((U8)nKeepDacPowerOn);
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, dts_keep_dac_power_on);
        }

        if (_AudRtkDtsGetI2sPcmMode(dts_i2s_pcm_mode_str, &nI2sPcmMode) == TRUE)
        {
            HalAudI2sSetPcmMode((U8)nI2sPcmMode);
        }
        else
        {
            //AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, dts_i2s_pcm_mode_str);
        }
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, Failed to find device node !\n",__FUNCTION__);
        AUD_PRINTF(ERROR_LEVEL,"IRQ ID will use default:%d !\n", INFINITY_IRQ_ID);
        AUD_PRINTF(ERROR_LEVEL,"amp-gpio chn0 will use default:%d %d !\n", _gaAmpGpio[0],_gaAmpGpio[1]);
        AUD_PRINTF(ERROR_LEVEL,"amp-gpio chn1 will use default:%d %d !\n", _gaAmpGpio[2],_gaAmpGpio[3]);
        AUD_PRINTF(ERROR_LEVEL,"All padmux will use default !\n");
    }

    AUD_PRINTF(DEBUG_LEVEL,"INFINITY_IRQ_ID = %d !\n", INFINITY_IRQ_ID);
    AUD_PRINTF(DEBUG_LEVEL," _gaAmpGpio[0] = %d, _gaAmpGpio[1] = %d !\n",_gaAmpGpio[0], _gaAmpGpio[1]);
    AUD_PRINTF(DEBUG_LEVEL," _gaAmpGpio[2] = %d, _gaAmpGpio[3] = %d !\n",_gaAmpGpio[2], _gaAmpGpio[3]);
}

void DrvAudApiIrqInit(void)
{
    int i = 0;
    MsIntInitParam_u uInitParam = {{0}};

    // Lock for IRQ handle
    CamOsSpinInit(&_gtRtkIrqHandle.rtkIrqLock);

    _gtRtkIrqHandle.rtkIrqReqCnt = 0;

    for ( i = 0; i < AUD_DMA_NUM; i++ )
    {
        _gtRtkIrqHandle.rtkIrqInfo[i].IsActive = FALSE;
        _gtRtkIrqHandle.rtkIrqInfo[i].dev_id = NULL;
    }

    // Register IRQ
    uInitParam.intc.eMap         = INTC_MAP_IRQ;
    uInitParam.intc.ePriority    = INTC_PRIORITY_7;
    uInitParam.intc.pfnIsr       = _AudDmaIrqRtk;

    MsInitInterrupt(&uInitParam, INFINITY_IRQ_ID);
}

void DrvAudApiIrqSetDmaStopCb(DmaStopChannelCbFn pfnCb )
{
    _gpDmaStopChannelCbFn = pfnCb;
}

BOOL DrvAudApiIrqRequest(const char *name, void *dev_id)
{
    PcmRuntimeData_t *ptRuntimeData = (PcmRuntimeData_t*)dev_id;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)ptRuntimeData->pPrivateData;

    if (ptDmaData->nChnId >= AUD_DMA_NUM)
        return FALSE;

    CamOsSpinLockIrqSave(&_gtRtkIrqHandle.rtkIrqLock);

    _gtRtkIrqHandle.rtkIrqInfo[ptDmaData->nChnId].IsActive = TRUE;
    _gtRtkIrqHandle.rtkIrqInfo[ptDmaData->nChnId].dev_id = dev_id;

    if ( _gtRtkIrqHandle.rtkIrqReqCnt == 0 )
    {
        MsUnmaskInterrupt(INFINITY_IRQ_ID);
    }

    _gtRtkIrqHandle.rtkIrqReqCnt++;

    CamOsSpinUnlockIrqRestore(&_gtRtkIrqHandle.rtkIrqLock);
    return TRUE;
}

BOOL DrvAudApiIrqFree(void *dev_id)
{
    PcmRuntimeData_t *ptRuntimeData = (PcmRuntimeData_t*)dev_id;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)ptRuntimeData->pPrivateData;

    if (ptDmaData->nChnId >= AUD_DMA_NUM)
        return FALSE;

    CamOsSpinLockIrqSave(&_gtRtkIrqHandle.rtkIrqLock);

    _gtRtkIrqHandle.rtkIrqInfo[ptDmaData->nChnId].IsActive = FALSE;
    _gtRtkIrqHandle.rtkIrqInfo[ptDmaData->nChnId].dev_id = NULL;

    if ( _gtRtkIrqHandle.rtkIrqReqCnt > 0 )
    {
        _gtRtkIrqHandle.rtkIrqReqCnt--;

        if ( _gtRtkIrqHandle.rtkIrqReqCnt == 0 )
        {
            MsMaskInterrupt(INFINITY_IRQ_ID);
        }
    }
    else
    {
        CamOsSpinUnlockIrqRestore(&_gtRtkIrqHandle.rtkIrqLock);
        return FALSE;
    }

    CamOsSpinUnlockIrqRestore(&_gtRtkIrqHandle.rtkIrqLock);
    return TRUE;
}

BOOL DrvAudApiAoAmpEnable(BOOL bEnable, S8 s8Ch)
{
    if ((_gaAmpGpio[0] == PAD_UNKNOWN) && (_gaAmpGpio[2] == PAD_UNKNOWN))
    {
        // Do nothing
        AUD_PRINTF(DEBUG_LEVEL,"AmpGpio chn0 & chn1 = PAD_UNKNOWN in %s\n",__FUNCTION__);
        return TRUE;
    }

    if ( bEnable == TRUE )
    {
        //enable gpio for line-out, should after atop enable
        if(s8Ch == 0)
        {
            if(_gaAmpGpio[0] != PAD_UNKNOWN)
                camdriver_gpio_direction_output(NULL, _gaAmpGpio[0], _gaAmpGpio[1]);
        }
        else if (s8Ch == 1)
        {
            if(_gaAmpGpio[2] != PAD_UNKNOWN)
                camdriver_gpio_direction_output(NULL, _gaAmpGpio[2], _gaAmpGpio[3]);
        }
        else
        {
             AUD_PRINTF(ERROR_LEVEL,"AmpGpio channel error, in %s\n",__FUNCTION__);
        }

#if 0 // QFN128
        // PAD_PM_GPIO0
        HalBachWriteReg2Byte(0xF00, 0x0003, 0x0002); // Output High
#elif 0 // QFN88 (Board must be modify manually)
        // PAD_SAR_GPIO1(79)
        HalBachWriteReg2Byte(0x1422, 0x3F3F , 0x0000); // Switch to GPIO mode + Output enable
        HalBachWriteReg2Byte(0x1423, 0x0002 , 0x0000); // Output
        HalBachWriteReg2Byte(0x1424, 0x0002 , 0x0002); // High
#else // Cocoa
        // PAD_PM_LED1(77)
        HalBachWriteReg2Byte(0xF96, 0x0003 , 0x0002); // Output High
#endif
    }
    else
    {
        //disable gpio for line-out, should before atop disable
        if(s8Ch == 0)
        {
            if(_gaAmpGpio[0] != PAD_UNKNOWN)
                camdriver_gpio_direction_output(NULL, _gaAmpGpio[0], !(_gaAmpGpio[1]));
        }
        else if (s8Ch == 1)
        {
            if(_gaAmpGpio[2] != PAD_UNKNOWN)
                camdriver_gpio_direction_output(NULL, _gaAmpGpio[2], !(_gaAmpGpio[3]));
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"AmpGpio channel error, in %s\n",__FUNCTION__);
        }

#if 0 // QFN128
        // PAD_PM_GPIO0
        HalBachWriteReg2Byte(0xF00, 0x0003, 0x0000); // Output Low
#elif 0 // QFN88 (Board must be modify manually)
        // PAD_SAR_GPIO1(79)
        HalBachWriteReg2Byte(0x1422, 0x3F3F , 0x0000); // Switch to GPIO mode + Output enable
        HalBachWriteReg2Byte(0x1423, 0x0002 , 0x0000); // Output
        HalBachWriteReg2Byte(0x1424, 0x0002 , 0x0000); // Low
#else // Cocoa
        // PAD_PM_LED1(77)
        HalBachWriteReg2Byte(0xF96, 0x0003 , 0x0000); // Output Low
#endif
    }

    return TRUE;
}

BOOL DrvAudApiAoAmpCtrlInit(void)
{
    return TRUE;
}

u32 DrvAudApiIoAddress(u32 addr)
{
    return addr;
}
