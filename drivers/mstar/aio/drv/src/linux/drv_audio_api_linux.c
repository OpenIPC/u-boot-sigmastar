/*
* drv_audio_api_linux.c- Sigmastar
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
//#include "cam_os_wrapper.h"

//
//#include "cam_sysfs.h"
//#include "cam_clkgen.h"
//#include "mdrv_gpio_io.h"

//
//#include <linux/interrupt.h>
//#include <linux/of_irq.h>
//#include <linux/gpio.h>
//#include <linux/clk.h>

//
#include "../gpio/infinity6e/gpio.h"
#include "../gpio/infinity6e/padmux.h"
#include "../gpio/drvGPIO.h"
//#include "padmux.h"
//#include "mdrv_gpio.h"
//#include "mdrv_padmux.h"

//
//#include "ms_platform.h"

// Hal
#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"
#include "hal_audio_os_api.h"

// Drv
//#include "drv_audio_dbg.h"
#include "drv_audio_api.h"
#include "drv_audio_pri.h"

//#ifdef CONFIG_CAM_CLK
//#include "drv_camclk_Api.h"
//#endif


// ------------------------
// Default Value
// ------------------------
//#define AUD_DEF_IRQ_ID      0 // INT_IRQ_BACH
#define AUD_DEF_AMP_GPIO    { PAD_PM_GPIO4, 1, PAD_UNKNOWN, 1 }



// ------------------------
// Variable
// ------------------------
//static unsigned int INFINITY_IRQ_ID = AUD_DEF_IRQ_ID;
static U32 _gaAmpGpio[4] = AUD_DEF_AMP_GPIO;
//static DmaStopChannelCbFn _gpDmaStopChannelCbFn = NULL;



// ------------------------
// Static declaration
// ------------------------
#if 0
static irqreturn_t  _AudDmaIrq(int irq, void *dev_id);



static irqreturn_t  _AudDmaIrq(int irq, void *dev_id)
{
    PcmRuntimeData_t *ptRuntimeData = (PcmRuntimeData_t*)dev_id;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)ptRuntimeData->pPrivateData;
    //unsigned long nFlags;
    BOOL bTrigger=0, bBoundary=0, bLocalData=0;
    BOOL bTriggerInt=0, bBoundaryInt=0;//,bLocalDataInt;

    //AUD_PRINTF(IRQ_LEVEL, "Function - %s, DMA Id = %d, pre state = %d\n", __func__, ptDmaData->nChnId, ptRuntimeData->nState);
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
            if ((bBoundary==TRUE)&&(bBoundaryInt==TRUE))
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;
                ptRuntimeData->nState = DMA_EMPTY;

                AUD_PRINTF(IRQ_LEVEL, "EMPTY: DMA Id = %d\n",  ptDmaData->nChnId);
            }
            else if ((ptRuntimeData->nState != DMA_UNDERRUN) && (bTrigger==TRUE) && (bTriggerInt==TRUE))
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
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO Runtime State %d\n",__LINE__,ptRuntimeData->nState);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO UNKNOWN flags %d, %d, %d\n",__LINE__,bTrigger,bBoundary,bLocalData);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO UNKNOWN interrupt %d, %d\n",__LINE__,bTriggerInt,bBoundaryInt);
                AUD_PRINTF(ERROR_LEVEL, "[%d] DISABLE interrupt\n",__LINE__);
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);
            }
        }
        else
        {
            if ((bBoundary==TRUE)&&(bBoundaryInt==TRUE))
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;
                ptRuntimeData->nState = DMA_EMPTY;

                AUD_PRINTF(IRQ_LEVEL, "EMPTY: DMA Id = %d\n",  ptDmaData->nChnId);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO Runtime State %d\n",__LINE__,ptRuntimeData->nState);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO UNKNOWN flags %d, %d, %d\n",__LINE__,bTrigger,bBoundary,bLocalData);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AO UNKNOWN interrupt %d, %d\n",__LINE__,bTriggerInt,bBoundaryInt);
                AUD_PRINTF(ERROR_LEVEL, "[%d] DISABLE interrupt\n",__LINE__);
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);
            }
        }
    }
    else if ((PCM_STREAM_CAPTURE == ptRuntimeData->eStream) && (ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING))  // CAPTURE device
    {
        if (ptRuntimeData->nState != DMA_FULL)
        {
            if ((bBoundary==TRUE)&&(bBoundaryInt==TRUE))
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;

                AUD_PRINTF(IRQ_LEVEL, "FULL: DMA Id = %d\n", ptDmaData->nChnId);

                ptRuntimeData->nState = DMA_FULL;
            }
            else if ((ptRuntimeData->nState != DMA_OVERRUN) && (bTrigger==TRUE) && (bTriggerInt==TRUE))
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, TRUE);

                ptRuntimeData->nState = DMA_OVERRUN;
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);

                AUD_PRINTF(IRQ_LEVEL, "OVER: DMA Id = %d, LC = %u\n", ptDmaData->nChnId, HalAudDmaGetLevelCnt(ptDmaData->nChnId));
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI Runtime State %d\n",__LINE__,ptRuntimeData->nState);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI UNKNOWN flags %d, %d, %d\n",__LINE__,bTrigger,bBoundary,bLocalData);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI UNKNOWN interrupt %d, %d\n",__LINE__,bTriggerInt,bBoundaryInt);
                AUD_PRINTF(ERROR_LEVEL, "[%d] DISABLE interrupt\n",__LINE__);
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);
            }
        }
        else
        {
            if ((bBoundary==TRUE)&&(bBoundaryInt==TRUE))
            {
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                _gpDmaStopChannelCbFn(ptDmaData->nChnId);
                ptRuntimeData->nStatus = SND_PCM_STATE_XRUN;

                AUD_PRINTF(IRQ_LEVEL, "FULL: DMA Id = %d\n", ptDmaData->nChnId);

                ptRuntimeData->nState = DMA_FULL;
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI Runtime State %d\n",__LINE__,ptRuntimeData->nState);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI UNKNOWN flags %d, %d, %d\n",__LINE__,bTrigger,bBoundary,bLocalData);
                AUD_PRINTF(ERROR_LEVEL, "[%d] AI UNKNOWN interrupt %d, %d\n",__LINE__,bTriggerInt,bBoundaryInt);
                AUD_PRINTF(ERROR_LEVEL, "[%d] DISABLE interrupt\n",__LINE__);
                HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
                CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
                CamOsTsemUp(&ptRuntimeData->tSleep);
                CamOsSpinLockIrqSave(&ptDmaData->tLock);
            }
        }
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL, "[%s] Status != RUNNING\n", __func__);
        AUD_PRINTF(ERROR_LEVEL, "DMA Id = %d, Current Status = %d\n", ptDmaData->nChnId, ptRuntimeData->nStatus);
        AUD_PRINTF(ERROR_LEVEL, "Runtime State %d\n",ptRuntimeData->nState);
        AUD_PRINTF(ERROR_LEVEL, "bTrigger = %d   bTriggerInt = %d\r\n",bTrigger,bTriggerInt);
        AUD_PRINTF(ERROR_LEVEL, "bBoundary = %d  bBoundaryInt = %d\r\n",bBoundary,bBoundaryInt);

        HalAudDmaIntEnable(ptDmaData->nChnId, FALSE, FALSE);
    }

    AUD_PRINTF(IRQ_LEVEL, "Function - %s, DMA Id = %d, cur state = %d\n", __func__, ptDmaData->nChnId, ptRuntimeData->nState);
    AUD_PRINTF(IRQ_LEVEL, "bTrigger = %d   bTriggerInt = %d\r\n",bTrigger,bTriggerInt);
    AUD_PRINTF(IRQ_LEVEL, "bBoundary = %d  bBoundaryInt = %d\r\n",bBoundary,bBoundaryInt);
    //AUD_PRINTF(IRQ_LEVEL, "bLocalData = %d bLocalDataInt = %d\r\n",bLocalData,bLocalDataInt);

    CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

    return IRQ_HANDLED;
}
#endif

#if 0
void DrvAudApiDtsInit(void)
{
    int ret, i = 0;
    struct device_node *pDevNode;
    S32 nPadMuxMode, nKeepI2sClk = 0, nI2sPcmMode = 0;
    const char *dts_compatible_str = NULL, *dts_amp_gpio_str = NULL, *dts_keep_i2s_clk_str = NULL;
    U32 nI2sRxTdmWsPgm = FALSE;
    U32 nI2sRxTdmWsWidth = 0;
    U32 nI2sRxTdmWsInv = 0;
    U32 aI2sRxTdmChSwap[2] = {0};
    U32 nI2sTxTdmWsPgm = FALSE;
    U32 nI2sTxTdmWsWidth = 0;
    U32 nI2sTxTdmWsInv = 0;
    U32 aI2sTxTdmChSwap[2] = {0};
    U32 nI2sTxTdmActiveSlot = 0;
    U32 nKeepAdcPowerOn = 0;
    U32 nKeepDacPowerOn = 0;
#ifdef CONFIG_CAM_CLK
    u32 AioClk;
    void *pvAioclk = NULL;
#else
    struct clk *clk;
#endif

#if 0
    card.dts->template_fp_list[0](123);
#endif

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

    //
    pDevNode = of_find_compatible_node(NULL, NULL, dts_compatible_str);
    if(pDevNode)
    {
        // IRQ
        INFINITY_IRQ_ID = CamOfIrqToResource(of_node_get(pDevNode), 0, NULL);

        //  enable for line-out
        ret = CamOfPropertyReadU32Array(pDevNode, dts_amp_gpio_str, _gaAmpGpio, 4 );
        if (ret == 0)
        {
            // Use padmux DTS, so replace pad setting (_gaAmpGpio[0]) from audio DTS by padmux DTS.
            // _gaAmpGpio[1] is still from Audio DTS.
            if (mdrv_padmux_active())
            {
                HalAudOsApiChangeAmpGpio(&_gaAmpGpio[0], 0); //chn0
                HalAudOsApiChangeAmpGpio(&_gaAmpGpio[2], 1); //chn1
            }

            if (_gaAmpGpio[0] == PAD_UNKNOWN)
            {
                // Do nothing
                AUD_PRINTF(DEBUG_LEVEL,"AmpGpio chn0 = PAD_UNKNOWN, don't do CamGpioRequest !\n");
            }
            else
            {
                ret = CamGpioRequest(_gaAmpGpio[0], dts_amp_gpio_str);
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
                ret = CamGpioRequest(_gaAmpGpio[2], dts_amp_gpio_str);
                if(ret)
                {
                    AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request amp-gpio chn1 !\n",__FUNCTION__);
                }
            }
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request amp-gpio !\n",__FUNCTION__);
            AUD_PRINTF(ERROR_LEVEL,"%s, amp-gpio chn0 will use default:%d %d !\n",__FUNCTION__, _gaAmpGpio[0], _gaAmpGpio[1]);
            AUD_PRINTF(ERROR_LEVEL,"%s, amp-gpio chn1 will use default:%d %d !\n",__FUNCTION__, _gaAmpGpio[2], _gaAmpGpio[3]);
        }

        if (mdrv_padmux_active())
        {
            HalAudOsApiPreSetAllPadmux();
        }
        else
        {
            for ( i = 0; i < card.dts->num_padmux; i++ )
            {
                ret = CamofPropertyReadU32(pDevNode, card.dts->audio_dts_padmux_list[i].str, &nPadMuxMode);

                if (ret == 0)
                {
                    card.dts->audio_dts_padmux_list[i].fp(nPadMuxMode);
                }
                else
                {
                    AUD_PRINTF(ERROR_LEVEL,"%s, Failed to gpio_request %s !\n",__FUNCTION__, card.dts->audio_dts_padmux_list[i].str);
                    AUD_PRINTF(ERROR_LEVEL,"%s, %s will use default !\n",__FUNCTION__, card.dts->audio_dts_padmux_list[i].str);
                }
            }
        }

        // keep_i2s_clk
        ret = CamofPropertyReadU32(pDevNode, dts_keep_i2s_clk_str, &nKeepI2sClk);

        if (ret == 0)
        {
            HalAudI2sSetKeepClk(nKeepI2sClk);
        }
        else
        {
            AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, dts_keep_i2s_clk_str);
        }

        // i2s_pcm_mode
        ret = CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_pcm_str, &nI2sPcmMode);

        if (ret == 0)
        {
            HalAudI2sSetPcmMode(nI2sPcmMode);
        }
        else
        {
            //AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_pcm_str);
        }

        // I2S RX TDM
        if ( card.dts->audio_dts_i2s_Rx_Tdm_WsPgm_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Rx_Tdm_WsPgm_str, &nI2sRxTdmWsPgm))
            {
                HalAudI2sSaveRxTdmWsPgm((BOOL)nI2sRxTdmWsPgm);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Rx_Tdm_WsPgm_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Rx_Tdm_WsWidth_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Rx_Tdm_WsWidth_str, &nI2sRxTdmWsWidth))
            {
                HalAudI2sSaveRxTdmWsWidth((U8)nI2sRxTdmWsWidth);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Rx_Tdm_WsWidth_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Rx_Tdm_WsInv_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Rx_Tdm_WsInv_str, &nI2sRxTdmWsInv))
            {
                HalAudI2sSaveRxTdmWsInv((BOOL)nI2sRxTdmWsInv);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Rx_Tdm_WsInv_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Rx_Tdm_ChSwap_str != NULL )
        {
            if (0 == CamOfPropertyReadU32Array(pDevNode, card.dts->audio_dts_i2s_Rx_Tdm_ChSwap_str, aI2sRxTdmChSwap, 2 ))
            {
                HalAudI2sSaveRxTdmChSwap((U8)aI2sRxTdmChSwap[0], (U8)aI2sRxTdmChSwap[1]);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Rx_Tdm_ChSwap_str);
            }
        }

        // I2S TX TDM
        if ( card.dts->audio_dts_i2s_Tx_Tdm_WsPgm_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Tx_Tdm_WsPgm_str, &nI2sTxTdmWsPgm))
            {
                HalAudI2sSaveTxTdmWsPgm((BOOL)nI2sTxTdmWsPgm);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Tx_Tdm_WsPgm_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Tx_Tdm_WsWidth_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Tx_Tdm_WsWidth_str, &nI2sTxTdmWsWidth))
            {
                HalAudI2sSaveTxTdmWsWidth((U8)nI2sTxTdmWsWidth);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Tx_Tdm_WsWidth_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Tx_Tdm_WsInv_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Tx_Tdm_WsInv_str, &nI2sTxTdmWsInv))
            {
                HalAudI2sSaveTxTdmWsInv((U8)nI2sTxTdmWsInv);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Tx_Tdm_WsInv_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Tx_Tdm_ChSwap_str != NULL )
        {
            if (0 == CamOfPropertyReadU32Array(pDevNode, card.dts->audio_dts_i2s_Tx_Tdm_ChSwap_str, aI2sTxTdmChSwap, 2 ))
            {
                HalAudI2sSaveTxTdmChSwap((U8)aI2sTxTdmChSwap[0], (U8)aI2sTxTdmChSwap[1]);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Tx_Tdm_ChSwap_str);
            }
        }

        if ( card.dts->audio_dts_i2s_Tx_Tdm_ActiveSlot_str != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_i2s_Tx_Tdm_ActiveSlot_str, &nI2sTxTdmActiveSlot ))
            {
                HalAudI2sSaveTxTdmActiveSlot((U8)nI2sTxTdmActiveSlot);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_i2s_Tx_Tdm_ActiveSlot_str);
            }
        }
        if ( card.dts->audio_dts_keep_adc_power_on != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_keep_adc_power_on, &nKeepAdcPowerOn ))
            {
                HalAudKeepAdcPowerOn((U8)nKeepAdcPowerOn);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_keep_adc_power_on);
            }
        }
        if ( card.dts->audio_dts_keep_dac_power_on != NULL )
        {
            if (0 == CamofPropertyReadU32(pDevNode, card.dts->audio_dts_keep_dac_power_on, &nKeepDacPowerOn ))
            {
                HalAudKeepDacPowerOn((U8)nKeepDacPowerOn);
            }
            else
            {
                AUD_PRINTF(ERROR_LEVEL,"%s, Failed to get %s attr !\n",__FUNCTION__, card.dts->audio_dts_keep_dac_power_on);
            }
        }
        //
        of_node_put(pDevNode);
    }
    else
    {
        AUD_PRINTF(ERROR_LEVEL,"%s, Failed to find device node !\n",__FUNCTION__);
        AUD_PRINTF(ERROR_LEVEL,"IRQ ID will use default:%d !\n", INFINITY_IRQ_ID);
        AUD_PRINTF(ERROR_LEVEL,"amp-gpio chn0 will use default:%d %d !\n", _gaAmpGpio[0],_gaAmpGpio[1]);
        AUD_PRINTF(ERROR_LEVEL,"amp-gpio chn1 will use default:%d %d !\n", _gaAmpGpio[2],_gaAmpGpio[3]);
        AUD_PRINTF(ERROR_LEVEL,"All padmux will use default !\n");
        return;
    }

    AUD_PRINTF(DEBUG_LEVEL,"INFINITY_IRQ_ID = %d !\n", INFINITY_IRQ_ID);
    AUD_PRINTF(DEBUG_LEVEL," _gaAmpGpio[0] = %d, _gaAmpGpio[1] = %d !\n",_gaAmpGpio[0], _gaAmpGpio[1]);
    AUD_PRINTF(DEBUG_LEVEL," _gaAmpGpio[2] = %d, _gaAmpGpio[3] = %d !\n",_gaAmpGpio[2], _gaAmpGpio[3]);

    // Enable PLL
#ifdef CONFIG_CAM_CLK
    AioClk = 0;
    CamOfPropertyReadU32Index(pDevNode,"camclk", 0,&(AioClk));
    if (!AioClk)
    {
        printk(KERN_DEBUG "[%s] Fail to get clk!\n", __func__);
    }
    else
    {
        CamClkRegister("Aio",AioClk,&(pvAioclk));
        CamClkSetOnOff(pvAioclk,1);
        CamClkUnregister(pvAioclk);
    }
#else
    clk = of_clk_get(pDevNode, 0);
    if (clk == NULL)
    {
        AUD_PRINTF(ERROR_LEVEL,"Get audio clock fail!\n");
        return;
    }

    CamClkPrepareEnable(clk);
    clk_put(clk);
#endif
}
#endif

#if 0
void DrvAudApiIrqInit(void)
{
}
#endif

#if 0
void DrvAudApiIrqSetDmaStopCb(DmaStopChannelCbFn pfnCb )
{
    _gpDmaStopChannelCbFn = pfnCb;
}
#endif

#if 0
BOOL DrvAudApiIrqRequest(const char *name, void *dev_id)
{
    s32 err = 0;

    AUD_PRINTF(TRACE_LEVEL, "IRQ_ID = 0x%x\n", INFINITY_IRQ_ID);

    err = request_irq(INFINITY_IRQ_ID, //INT_MS_AUDIO_1,
                      _AudDmaIrq,
                      IRQF_SHARED,
                      name,
                      (void *)dev_id);
    if (err)
    {
        AUD_PRINTF(ERROR_LEVEL,"%s request irq err=%d\n",__FUNCTION__,err);
        return FALSE;
    }

    return TRUE;
}
#endif

#if 0
BOOL DrvAudApiIrqFree(void *dev_id)
{
    free_irq(INFINITY_IRQ_ID, (void *)dev_id);

    return TRUE;
}
#endif

BOOL DrvAudApiAoAmpEnable(BOOL bEnable, S8 s8Ch)
{
    if ((_gaAmpGpio[0] == PAD_UNKNOWN) && (_gaAmpGpio[2] == PAD_UNKNOWN))
    {
        // Do nothing
        printf("AmpGpio chn0 & chn1 = PAD_UNKNOWN in %s\n",__FUNCTION__);
        return TRUE;
    }

    if ( bEnable == TRUE )
    {
        //enable gpio for line-out, should after atop enable
        if(s8Ch == 0)
        {
            if(_gaAmpGpio[0] != PAD_UNKNOWN)
                MDrv_GPIO_Pull_High(_gaAmpGpio[0]);
        }
        else if (s8Ch == 1)
        {
            if(_gaAmpGpio[2] != PAD_UNKNOWN)
                MDrv_GPIO_Pull_High(_gaAmpGpio[2]);
        }
        else
        {
             printf("AmpGpio channel error, in %s\n",__FUNCTION__);
        }
    }
    else
    {
        //disable gpio for line-out, should before atop disable
        if(s8Ch == 0)
        {
            if(_gaAmpGpio[0] != PAD_UNKNOWN)
                MDrv_GPIO_Pull_Low(_gaAmpGpio[0]);
        }
        else if (s8Ch == 1)
        {
            if(_gaAmpGpio[2] != PAD_UNKNOWN)
                MDrv_GPIO_Pull_Low(_gaAmpGpio[2]);
        }
        else
        {
            printf("AmpGpio channel error, in %s\n",__FUNCTION__);
        }
    }

    return TRUE;
}

BOOL DrvAudApiAoAmpCtrlInit(void)
{
    if ((_gaAmpGpio[0] == PAD_UNKNOWN) && (_gaAmpGpio[2] == PAD_UNKNOWN))
    {
        // Do nothing
        printf("AmpGpio chn0 & chn1 = PAD_UNKNOWN in %s\n",__FUNCTION__);
        return TRUE;
    }

    if(_gaAmpGpio[0] != PAD_UNKNOWN)
    {
        MDrv_GPIO_PadVal_Set(_gaAmpGpio[0], PINMUX_FOR_GPIO_MODE);
        MDrv_GPIO_Pad_Oen(_gaAmpGpio[0]);
    }
    if(_gaAmpGpio[2] != PAD_UNKNOWN)
    {
        MDrv_GPIO_PadVal_Set(_gaAmpGpio[2], PINMUX_FOR_GPIO_MODE);
        MDrv_GPIO_Pad_Oen(_gaAmpGpio[2]);
    }

    return TRUE;
}

#if 0
u32 DrvAudApiIoAddress(u32 addr)
{
    return IO_ADDRESS(addr);
}
#endif
