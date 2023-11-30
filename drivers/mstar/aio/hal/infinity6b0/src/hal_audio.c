/*
* hal_audio.c- Sigmastar
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
#include "hal_audio_reg.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"



#define REF_FREQ 384     //384MHz
#define I2S_PADMUX_OFFSET 10
#define DIGMIC_PADMUX_OFFSET 8

// Private I2S Config
typedef struct
{

} AudPriI2sCfg_t;

//
static void _uDelay(U32 u32MicroSeconds)
{
    CamOsUsDelay(u32MicroSeconds);
}

//
static void _mSleep(U32 u32MilliSeconds)
{
    CamOsMsSleep(u32MilliSeconds);
}

static S32 _gaPadMuxMode[AUD_PAD_MUX_NUM] =
{
    AUD_DEF_DIG_PMUX,
    AUD_DEF_I2S_PMUX,
    AUD_DEF_I2S_PMUX,
};

static BOOL _gbAdcActive = FALSE;
static BOOL _gbDacActive = FALSE;
static BOOL _gbAtopStatus[AUD_ATOP_NUM];
static AudI2sCfg_t _gaI2sCfg[AUD_I2S_NUM];
//static AudPriI2sCfg_t _gaPriI2sCfg[AUD_I2S_NUM];
static AudAtopPath_e _gnAdcSelect;

static BOOL _gbI2sRxActive = FALSE, _gbI2sTxActive = FALSE;
static BOOL _gbI2sKeepClk = FALSE;
static BOOL _gbI2sPcmMode = FALSE;

static U8 _gnI2sRxTdmWsPgm = FALSE;
static U8 _gnI2sRxTdmWsWidth = 0;
static U8 _gnI2sRxTdmWsInv = 0;
static U8 _gnI2sRxTdmChSwap = 0; //[0]: 0<->2, 1<->3, 4<->6, 5<->7 [1]: 0<->4, 1<->5, 2<->6, 3<->7

static U8 _gnI2sTxTdmWsPgm = FALSE;
static U8 _gnI2sTxTdmWsWidth = 0;
static U8 _gnI2sTxTdmWsInv = 0;
static U8 _gnI2sTxTdmChSwap = 0; //[0]: 0<->2, 1<->3, 4<->6, 5<->7 [1]: 0<->4, 1<->5, 2<->6, 3<->7
static U8 _gnI2sTxTdmActiveSlot = 0;

static AoAmpCb _HalAudAoAmpCb;

static BOOL _gbIsDmicUsed = FALSE;

static void _HalAudSysInit(void);
static void _HalAudAtopInit(void);
static void _HalAudDmicRegInit(void);
static BOOL _HalAudDmaFirstInit(void);
static BOOL _HalAudDmaWrMchInit(void);
static void _HalAudDpgaInit(void);
static void _HalAudPadMuxInit(void);
static BOOL _HalAudIntEnable(void);
static void _HalAudAtopAdc(BOOL bEnable);
static void _HalAudAtopEnableRef(BOOL bEnable);
static BOOL _HalAudAtopSwitch(AudAtopPath_e ePath, BOOL bEnable);
static BOOL _HalAudDmaReInit(AudDmaChn_e eDmaChannel);
static BOOL _HalAudDmaRdIntEnable(AudDmaChn_e eDmaChannel, BOOL bUnderrun, BOOL bEmpty);
static BOOL _HalAudDmaWrIntEnable(AudDmaChn_e eDmaChannel, BOOL bOverrun, BOOL bFull);
static BOOL _HalAudDmaGetRdInt(AudDmaChn_e eDmaChannel, BOOL *bUnderrun, BOOL *bEmpty);
static BOOL _HalAudDmaGetWrInt(AudDmaChn_e eDmaChannel, BOOL *bOverrun, BOOL *bFull);
static BOOL _HalAudDmaRdGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbUnderrun, BOOL *pbEmtpy, BOOL *pbLocalEmpty);
static BOOL _HalAudDmaWrGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbOverrun, BOOL *pbFull, BOOL *pbLocalFull);
static U32 _HalAudDmaGetRawMiuUnitLevelCnt(AudDmaChn_e eDmaChannel);
static BOOL _HalAudDmaGetMchSelConfigValue(AudMchChBind_e eMchChBind, AudMchSel_e eMchSel, U16 *nChSel);
static void _HalAudDpgaCalGain(S8 s8Gain, U8 *pU8GainIdx);

static void _HalAudSysInit(void)
{
    _HalAudAtopInit();

    HalBachWriteRegByte(0x00150200, 0xff, 0x00);
    HalBachWriteRegByte(0x00150201, 0xff, 0x40);
    HalBachWriteRegByte(0x00150200, 0xff, 0xff);
    //HalBachWriteRegByte(0x00150201, 0xff, 0x8d);
    HalBachWriteRegByte(0x00150201, 0xff, 0x89);
    HalBachWriteRegByte(0x00150202, 0xff, 0x88);
    //HalBachWriteRegByte(0x00150203, 0xff, 0xfA); // Reset to 32K sample rate
    HalBachWriteRegByte(0x00150203, 0xff, 0xff);
    HalBachWriteRegByte(0x00150204, 0xff, 0x03);
    HalBachWriteRegByte(0x00150205, 0xff, 0x00);
    HalBachWriteRegByte(0x00150206, 0xff, 0xB4);
    HalBachWriteRegByte(0x00150207, 0xff, 0x19);
    HalBachWriteRegByte(0x00150208, 0xff, 0x00);
    HalBachWriteRegByte(0x00150209, 0xff, 0xf0);
    HalBachWriteRegByte(0x0015020a, 0xff, 0x00);
    HalBachWriteRegByte(0x0015020b, 0xff, 0x80);
    HalBachWriteRegByte(0x0015020c, 0xff, 0x9a);
    HalBachWriteRegByte(0x0015020d, 0xff, 0xc0);
    HalBachWriteRegByte(0x0015020e, 0xff, 0x5a);
    HalBachWriteRegByte(0x0015020f, 0xff, 0x55);
    //HalBachWriteRegByte(0x00150212, 0xff, 0x05);
    HalBachWriteRegByte(0x00150212, 0xff, 0x09);
    HalBachWriteRegByte(0x00150213, 0xff, 0x02);
    HalBachWriteRegByte(0x00150214, 0xff, 0x00);
    HalBachWriteRegByte(0x00150215, 0xff, 0x00);
    HalBachWriteRegByte(0x00150216, 0xff, 0x7d);
    HalBachWriteRegByte(0x00150217, 0xff, 0x00);
    HalBachWriteRegByte(0x0015023a, 0xff, 0x1d);
    HalBachWriteRegByte(0x0015023b, 0xff, 0x02);
    HalBachWriteRegByte(0x0015023a, 0xff, 0x00);
    HalBachWriteRegByte(0x0015023b, 0xff, 0x00);
    HalBachWriteRegByte(0x0015031c, 0xff, 0x03);
    HalBachWriteRegByte(0x0015031d, 0xff, 0x00);
    HalBachWriteRegByte(0x0015032c, 0xff, 0x03);
    HalBachWriteRegByte(0x0015031d, 0xff, 0x00);
    HalBachWriteRegByte(0x00150226, 0xff, 0x00);
    HalBachWriteRegByte(0x00150227, 0xff, 0xd4);

    //correct IC default value
    HalBachWriteRegByte(0x00150248, 0xff, 0x07);
    HalBachWriteRegByte(0x00150249, 0xff, 0x00);
    HalBachWriteRegByte(0x00150250, 0xff, 0x07);

    // For AEC, DMA_RD_DATA will be masked to zero when DMA read disable nor level_cnt_live is zero.
    HalBachWriteRegByte(0x00150280, 0x80, 0x80);
    // HalBachWriteRegByte(0x001502A0, 0x80, 0x80); // I6 have no DMA2, so mask it.

    // Sin Gen
    //HalBachWriteRegByte(0x001502EA, 0xff, 0x02);
    //HalBachWriteRegByte(0x001502EB, 0xff, 0xE0); // AO 0xE0, AI 0xF0
}

static void _HalAudAtopInit(void)
{
    S32 i;
    HalBachWriteRegByte(0x00103400, 0xff, 0x14);
    //HalBachWriteRegByte(0x00103401, 0xff, 0x02);
    HalBachWriteRegByte(0x00103401, 0xff, 0x0a); //enable MSP, speed up charge VREF
    HalBachWriteRegByte(0x00103402, 0xff, 0x30);
    HalBachWriteRegByte(0x00103403, 0xff, 0x00);
    HalBachWriteRegByte(0x00103404, 0xff, 0x80);
    HalBachWriteRegByte(0x00103405, 0xff, 0x00);
    HalBachWriteRegByte(0x00103406, 0xff, 0x00);
    HalBachWriteRegByte(0x00103407, 0xff, 0x00);
    //HalBachWriteRegByte(0x00103406, 0xff, 0xf7);
    //HalBachWriteRegByte(0x00103407, 0xff, 0x1f);
    HalBachWriteRegByte(0x00103408, 0xff, 0x00);
    HalBachWriteRegByte(0x00103409, 0xff, 0x00);
    HalBachWriteRegByte(0x0010340a, 0xff, 0x77);
    HalBachWriteRegByte(0x0010340b, 0xff, 0x00);
    HalBachWriteRegByte(0x0010340c, 0xff, 0x00);
    HalBachWriteRegByte(0x0010340d, 0xff, 0x00);
    HalBachWriteRegByte(0x0010340e, 0xff, 0x00);
    HalBachWriteRegByte(0x0010340f, 0xff, 0x00);
    HalBachWriteRegByte(0x00103410, 0xff, 0x00);
    HalBachWriteRegByte(0x00103411, 0xff, 0x30);
    HalBachWriteRegByte(0x00103424, 0xff, 0x00);
    HalBachWriteRegByte(0x00103425, 0xff, 0x00);

    //
    for(i = 0; i < AUD_ATOP_NUM; i++)
    {
        _HalAudAtopSwitch((AudAtopPath_e)i, FALSE);
    }

    //
    HalAudAtopSetAdcMux( AUD_ADC_SEL_LINEIN );
}

static void _HalAudDmicRegInit(void)
{
    HalBachWriteRegByte(0x0015033a, 0xff, 0x02);
    HalBachWriteRegByte(0x0015033b, 0xff, 0x40); // Reg 0x3a,0x3b: [13] 0:4M, 1:2M
    HalBachWriteRegByte(0x0015033c, 0xff, 0x04);
    HalBachWriteRegByte(0x0015033d, 0xff, 0x01); // DMIC mux default off.
}

static BOOL _HalAudDmaFirstInit(void)
{
    //reset DMA1 interal register
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_4, 0xFFFF, 0);		                //reset DMA 1 read size
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_12, 0xFFFF, 0);		                //reset DMA 1 write size

    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_SW_RST_DMA, REG_SW_RST_DMA);	//DMA 1 software reset
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_SW_RST_DMA, 0);

    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK),
                    (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK));

    return TRUE;
}

static BOOL _HalAudDmaWrMchInit(void)
{
    //AUD_MCH_SET_W1
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_MCH_DEBUG_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_BIT_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_MCH_32B_EN, BACH_MCH_32B_EN);

    return TRUE;
}

static void _HalAudDpgaInit(void)
{
    U8 i;

    for ( i = 0; i < AUD_DPGA_NUM; i++ )
    {
        HalAudDpgaCtrl((AudDpga_e)i, TRUE, TRUE, TRUE );
    }
}

static void _HalAudPadMuxInit(void)
{
    _gbI2sRxActive = FALSE;
    _gbI2sTxActive = FALSE;

    HalAudSetPadMux(AUD_PAD_MUX_DMIC, FALSE);
    HalAudSetPadMux(AUD_PAD_MUX_I2S_TX, FALSE);
    HalAudSetPadMux(AUD_PAD_MUX_I2S_RX, FALSE);
}

static BOOL _HalAudIntEnable(void)
{
    HalBachWriteReg(BACH_REG_BANK2, BACH_INT_EN, REG_DMA_INT_EN, REG_DMA_INT_EN);

    return TRUE;
}

static void _HalAudAtopAdc(BOOL bEnable)
{
    U16 nMask;
    nMask = (REG_PD_ADC0 | REG_PD_INMUX_MSK | REG_PD_LDO_ADC );
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, (bEnable? 0:((REG_PD_ADC0 | (1<<REG_PD_INMUX_POS) | REG_PD_LDO_ADC ))));
}

static void _HalAudAtopEnableRef(BOOL bEnable)
{
    U16 nMask;
    nMask = (REG_PD_VI | REG_PD_VREF);
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, (bEnable? 0:nMask));
}

static BOOL _HalAudAtopSwitch(AudAtopPath_e ePath, BOOL bEnable)
{
    U16 nMask;

    switch(ePath)
    {
        case AUD_ADC_LINEIN:
            if (bEnable)
            {
                _HalAudAtopAdc(TRUE);
                _mSleep(50); //delay for analog initialize completed
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_LINEIN] = TRUE;
            }
            else
            {
                _HalAudAtopAdc(FALSE);
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_LINEIN] = FALSE;
            }
            break;

        case AUD_ADC_MICIN:
            if (bEnable)
            {
                _HalAudAtopAdc(TRUE);
                HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R, 0);
                _mSleep(50); //delay for analog initialize completed
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_MICIN]=TRUE;
            }
            else
            {
                _HalAudAtopAdc(FALSE);
                HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R);
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_MICIN]=FALSE;
            }
            break;

        case AUD_DAC_LINEOUT:
            nMask = (REG_PD_BIAS_DAC | REG_PD_L0_DAC | REG_PD_LDO_DAC | REG_PD_R0_DAC | REG_PD_REF_DAC);
            if (bEnable)
            {
                HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, 0);
                _gbDacActive = bEnable;
                _gbAtopStatus[AUD_DAC_LINEOUT] = TRUE;

                //enable gpio for line-out, should after atop enable
                _mSleep(10);
                _HalAudAoAmpCb(bEnable, 0); //chn0
                _HalAudAoAmpCb(bEnable, 1); //chn1
            }
            else
            {
                //disable gpio for line-out, should before atop disable
                _HalAudAoAmpCb(bEnable, 0); //chn0
                _HalAudAoAmpCb(bEnable, 1); //chn1

                HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, nMask);
                _gbDacActive = bEnable;
                _gbAtopStatus[AUD_DAC_LINEOUT] = FALSE;
            }
            break;

        default:
            ERRMSG("Function - %s # %d - ePath = %d, error !] \n", __func__, __LINE__, (int)ePath);
            return FALSE;
    }

    return TRUE;
}

static BOOL _HalAudDmaReInit(AudDmaChn_e eDmaChannel)
{
    switch ( eDmaChannel )
    {
        case AUD_DMA_READER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_TRIG, 0); // prevent from triggering levelcount at toggling init step
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_INIT, REG_RD_INIT);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_INIT, 0);
            break;

        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_TRIG, 0);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_INIT, REG_WR_INIT);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_INIT, 0);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            return FALSE;
            break;
    }

    return TRUE;
}

static BOOL _HalAudDmaRdIntEnable(AudDmaChn_e eDmaChannel, BOOL bUnderrun, BOOL bEmpty)
{
    U8 nAddr;
    BachRegBank_e eBank;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_0;
            break;
        default:
            return FALSE;
    }

    nConfigValue = HalBachReadReg(eBank,nAddr);

    /* Underrun interrupt */
    if(bUnderrun)
    {
        nConfigValue |= REG_RD_UNDERRUN_INT_EN;
    }
    else
    {
        nConfigValue &= ~REG_RD_UNDERRUN_INT_EN;
    }

    /* Empty interrupt */
    if(bEmpty)
    {
        nConfigValue |= REG_RD_EMPTY_INT_EN;
    }
    else
    {
        nConfigValue &= ~REG_RD_EMPTY_INT_EN;
    }

    HalBachWriteReg(eBank, nAddr, (REG_RD_UNDERRUN_INT_EN | REG_RD_EMPTY_INT_EN), nConfigValue);
    return TRUE;
}

static BOOL _HalAudDmaWrIntEnable(AudDmaChn_e eDmaChannel, BOOL bOverrun, BOOL bFull)
{
    U8 nAddr;
    BachRegBank_e eBank;
    U16 nConfigValue;
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_0;
            break;
        default:
            return FALSE;
    }

    nConfigValue = HalBachReadReg(eBank,nAddr);

    /* Overrun interrupt */
    if(bOverrun)
    {
        nConfigValue |= REG_WR_OVERRUN_INT_EN;
    }
    else
    {
        nConfigValue &= ~REG_WR_OVERRUN_INT_EN;
    }

    /* Full interrupt */
    if(bFull)
    {
        nConfigValue |= REG_WR_FULL_INT_EN;
    }
    else
    {
        nConfigValue &= ~REG_WR_FULL_INT_EN;
    }

    HalBachWriteReg(eBank, nAddr, (REG_WR_OVERRUN_INT_EN | REG_WR_FULL_INT_EN), nConfigValue);
    return TRUE;
}

static BOOL _HalAudDmaGetRdInt(AudDmaChn_e eDmaChannel, BOOL *bUnderrun, BOOL *bEmpty)
{
    BachRegBank_e eBank;
    U8 nAddr;
    U16 nConfigValue = 0;

    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_0;
            break;
        default:
            return FALSE;
    }

    nConfigValue = HalBachReadReg(eBank,nAddr);

    if ( ( nConfigValue & REG_RD_EMPTY_INT_EN ) == 0 )
    {
        *bEmpty = FALSE;
    }
    else
    {
        *bEmpty = TRUE;
    }

    if ( ( nConfigValue & REG_RD_UNDERRUN_INT_EN ) == 0 )
    {
        *bUnderrun = FALSE;
    }
    else
    {
        *bUnderrun = TRUE;
    }

    return TRUE;
}

static BOOL _HalAudDmaGetWrInt(AudDmaChn_e eDmaChannel, BOOL *bOverrun, BOOL *bFull)
{
    U8 nAddr;
    BachRegBank_e eBank;
    U16 nConfigValue = 0;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_0;
            break;
        default:
            return FALSE;
    }

    nConfigValue = HalBachReadReg(eBank,nAddr);

    if ( ( nConfigValue & REG_WR_FULL_INT_EN ) == 0 )
    {
        *bFull = FALSE;
    }
    else
    {
        *bFull = TRUE;
    }

    if ( ( nConfigValue & REG_WR_OVERRUN_INT_EN ) == 0 )
    {
        *bOverrun = FALSE;
    }
    else
    {
        *bOverrun = TRUE;
    }

    return TRUE;
}

static BOOL _HalAudDmaRdGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbUnderrun, BOOL *pbEmtpy, BOOL *pbLocalEmpty)
{
    U8 nAddr;
    U16 nConfigValue;
    BachRegBank_e eBank;
    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_8;
            break;

        default:
            return FALSE;
    }
    nConfigValue = HalBachReadReg(eBank, nAddr);
    *pbUnderrun = (nConfigValue & REG_RD_UNDERRUN_FLAG) ? TRUE : FALSE;
    *pbEmtpy = (nConfigValue & REG_RD_EMPTY_FLAG) ? TRUE : FALSE;
    *pbLocalEmpty = (nConfigValue & REG_RD_LOCALBUF_EMPTY) ? TRUE : FALSE;

    return TRUE;

}

static BOOL _HalAudDmaWrGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbOverrun, BOOL *pbFull, BOOL *pbLocalFull)
{
    U8 nAddr;
    U16 nConfigValue;
    BachRegBank_e eBank;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA1_CTRL_8;
            break;

        default:
            return FALSE;
    }
    nConfigValue = HalBachReadReg(eBank, nAddr);
    *pbOverrun = (nConfigValue & REG_WR_OVERRUN_FLAG) ? TRUE : FALSE;
    *pbFull = (nConfigValue & REG_WR_FULL_FLAG) ? TRUE : FALSE;
    *pbLocalFull = (nConfigValue & REG_WR_LOCALBUF_FULL) ? TRUE : FALSE;
    return TRUE;
}

static U32 _HalAudDmaGetRawMiuUnitLevelCnt(AudDmaChn_e eDmaChannel)
{
    U16 nConfigValue = 0;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_LEVEL_CNT_MASK, REG_WR_LEVEL_CNT_MASK);
            nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_15);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_LEVEL_CNT_MASK, 0);
            break;

        case AUD_DMA_READER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_LEVEL_CNT_MASK, REG_RD_LEVEL_CNT_MASK);
            nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_7);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_LEVEL_CNT_MASK, 0);
            break;

        default:
            ERRMSG("HalAudioDmaGetLevelCnt - ERROR bank default case! [%s ## %d] \n", __func__, __LINE__);
            return 0;
    }

    return (U32)nConfigValue;
}

static BOOL _HalAudDmaGetMchSelConfigValue(AudMchChBind_e eMchChBind, AudMchSel_e eMchSel, U16 *nChSel)
{
    switch(eMchSel)
    {
        case AUD_MCH_SEL_ADC:
            if (( eMchChBind == AUD_MCH_CH_BIND_01 ) || ( eMchChBind == AUD_MCH_CH_BIND_23 ))
            {
                *nChSel = 1;
            }
            else
            {
                ERRMSG("Function - %s # %d - eMchChBind = %d, eMchSel = %d, error !] \n", __func__, __LINE__, eMchChBind, eMchSel);
                return FALSE;
            }

            break;

        case AUD_MCH_SEL_I2S_RX:
            if (( eMchChBind == AUD_MCH_CH_BIND_01 ) || ( eMchChBind == AUD_MCH_CH_BIND_23 ))
            {
                *nChSel = 2;
            }
            else if (eMchChBind == AUD_MCH_CH_BIND_4567)
            {
                *nChSel = 1;
            }
            else
            {
                ERRMSG("Function - %s # %d - eMchChBind = %d, eMchSel = %d, error !] \n", __func__, __LINE__, eMchChBind, eMchSel);
                return FALSE;
            }
            break;

        case AUD_MCH_SEL_SRC:
            if (( eMchChBind == AUD_MCH_CH_BIND_01 ) || ( eMchChBind == AUD_MCH_CH_BIND_23 ))
            {
                *nChSel = 3;
            }
            else
            {
                ERRMSG("Function - %s # %d - eMchChBind = %d, eMchSel = %d, error !] \n", __func__, __LINE__, eMchChBind, eMchSel);
                return FALSE;
            }
            break;

        case AUD_MCH_SEL_NULL:
            if (( eMchChBind == AUD_MCH_CH_BIND_01 ) || ( eMchChBind == AUD_MCH_CH_BIND_23 ))
            {
                *nChSel = 0; // Default = 0, if NULL
            }
            else if (eMchChBind == AUD_MCH_CH_BIND_4567)
            {
                *nChSel = 0; // Default = 0, if NULL
            }
            else
            {
                ERRMSG("Function - %s # %d - eMchChBind = %d, eMchSel = %d, error !] \n", __func__, __LINE__, eMchChBind, eMchSel);
                return FALSE;
            }
            break;

        default:
            ERRMSG("Function - %s # %d - eMchSel = %d, error !] \n", __func__, __LINE__, eMchSel);
            return FALSE;
    }

    return TRUE;
}

static void _HalAudDpgaCalGain(S8 s8Gain, U8 *pU8GainIdx)
{
    if(s8Gain > BACH_DPGA_GAIN_MAX_DB)
        s8Gain = BACH_DPGA_GAIN_MAX_DB;
    else if(s8Gain < BACH_DPGA_GAIN_MIN_DB)
        s8Gain = BACH_DPGA_GAIN_MIN_DB;

    if(s8Gain == BACH_DPGA_GAIN_MIN_DB)
        *pU8GainIdx = BACH_DPGA_GAIN_MIN_IDX;
    else
        *pU8GainIdx = (U8)(-2 * s8Gain); //index = -2 * (gain) ,because step = -0.5dB
}

static void _HalAudI2sPcmMode(BOOL bEnable)
{
    /* only for non-TDM platform */
#if 1
    if(_gbI2sPcmMode)
    {
        HalBachWriteReg(BACH_REG_BANK1, BACH_MUX2_SEL, REG_CODEC_PCM_MODE, REG_CODEC_PCM_MODE);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK1, BACH_MUX2_SEL, REG_CODEC_PCM_MODE, 0);
    }
#endif
}

void HalAudMainInit(void)
{
    _HalAudSysInit();
    _HalAudDmicRegInit();
    _HalAudDmaFirstInit();
    _HalAudDmaWrMchInit();
    _HalAudDpgaInit();
    _HalAudPadMuxInit();
    _HalAudIntEnable();
}

void HalAudMainDeInit(void)
{
    _gbAdcActive =0;
    _gbDacActive =0;
}

void HalAudSetAoAmpCb(AoAmpCb cb)
{
    _HalAudAoAmpCb = cb;
}

BOOL HalAudSetMux(AudMux_e eMux, U8 nChoice)
{
    switch(eMux)
    {
        case AUD_MUX_MMC1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_MUX0_SEL, REG_MMC1_SRC_SEL, (nChoice?REG_MMC1_SRC_SEL:0));
            break;

        case AUD_MUX_DMAWR1:
            HalBachWriteReg(BACH_REG_BANK2, BACH_MUX1_SEL, MUX_ASRC_ADC_SEL, (nChoice?MUX_ASRC_ADC_SEL:0));
            break;

        case AUD_MUX_DMAWR1_MCH:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_NEW_MODE, (nChoice?BACH_DMA_WR_NEW_MODE:0));
            break;

        case AUD_MUX_I2S_TX:
            if(nChoice > 2 )
            {
                ERRMSG("Function - %s # %d - nChoice = %d, error !] \n", __func__, __LINE__, (int)nChoice);
                return FALSE;
            }
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, MUX_CODEC_TX_SEL_MSK, nChoice<<MUX_CODEC_TX_SEL_POS);
            break;

        case AUD_MUX_AMIC_DMIC:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL1, REG_CIC_SEL, (nChoice?REG_CIC_SEL:0));
            break;

        case AUD_MUX_DAAD_LOOP:
            HalBachWriteReg(BACH_REG_BANK1, BACH_PAD0_CFG, CODEC_DAAD_LOOP, (nChoice?CODEC_DAAD_LOOP:0));
            break;

        default:
            ERRMSG("Function - %s # %d - eMux = %d, error !] \n", __func__, __LINE__, (int)eMux);
            return FALSE;
    }
    return TRUE;
}

BOOL HalAudSetPadMux(AudPadMux_e PadMux, BOOL bEnable)
{
    U16 nConfigValue;
    S32 nMode;

    switch(PadMux)
    {
        case AUD_PAD_MUX_I2S_RX:
            nConfigValue = HalBachReadReg2Byte(0x101E1E);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_RX];
            if(bEnable)
            {
                _gbI2sRxActive = TRUE;

                nConfigValue &= ~(0x03<<I2S_PADMUX_OFFSET);
                nConfigValue |= (nMode<<I2S_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
            }
            else
            {
                _gbI2sRxActive = FALSE;

                if ( _gbI2sKeepClk == FALSE )
                {
                    if ( ( _gbI2sRxActive == FALSE ) && ( _gbI2sTxActive == FALSE ) )
                    {
                        nConfigValue &= ~(0x03<<I2S_PADMUX_OFFSET);
                        HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
                    }
                }
            }
            break;

        case AUD_PAD_MUX_I2S_TX:
            nConfigValue = HalBachReadReg2Byte(0x101E1E);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_TX];
            if(bEnable)
            {
                _gbI2sTxActive = TRUE;

                nConfigValue &= ~(0x03<<I2S_PADMUX_OFFSET);
                nConfigValue |= (nMode<<I2S_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
            }
            else
            {
                _gbI2sTxActive = FALSE;

                if ( _gbI2sKeepClk == FALSE )
                {
                    if ( ( _gbI2sRxActive == FALSE ) && ( _gbI2sTxActive == FALSE ) )
                    {
                        nConfigValue &= ~(0x03<<I2S_PADMUX_OFFSET);
                        HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
                    }
                }
            }
            break;

        case AUD_PAD_MUX_DMIC:
            nConfigValue = HalBachReadReg2Byte(0x101E1E);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_DMIC];
            if(bEnable)
            {
                nConfigValue &= ~(0x03<<DIGMIC_PADMUX_OFFSET);
                nConfigValue |= (nMode<<DIGMIC_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
            }
            else
            {
                nConfigValue &= ~(0x03<<DIGMIC_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x101E1E, 0xFFFF, nConfigValue);
            }
            break;

        default:
            return FALSE;
    }

    // i2s pcm mode
    _HalAudI2sPcmMode(_gbI2sPcmMode);

    return TRUE;
}

BOOL HalAudSrcSetRate(AudRate_e eRate)
{
    switch(eRate)
    {
        case AUD_RATE_8K:
            HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_1_SEL_MSK, 0<<REG_CIC_1_SEL_POS);
            break;

        case AUD_RATE_16K:
            HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_1_SEL_MSK, 1<<REG_CIC_1_SEL_POS);
            break;

        case AUD_RATE_32K:
            HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_1_SEL_MSK, 2<<REG_CIC_1_SEL_POS);
            break;

        case AUD_RATE_48K:
            HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_1_SEL_MSK, 3<<REG_CIC_1_SEL_POS);
            break;

        default:
            ERRMSG("HalAudSrcSetRate - ERROR SRC Sample Rate errors [%s ## %d] \n", __func__, __LINE__);
            return FALSE;
    }

    return TRUE;

}

BOOL HalAudAtopOpen(AudAtopPath_e ePath)
{
    if (ePath < 0 || ePath > AUD_ATOP_NUM)
    {
        ERRMSG("Function - %s # %d - ePath = %d, error !] \n", __func__, __LINE__, (int)ePath);
        return FALSE;
    }
    else
    {
        if ( !(_gbAdcActive||_gbDacActive) )
        {
            _HalAudAtopEnableRef(TRUE);
        }

        switch(ePath)
        {
            case AUD_ADC_LINEIN:
                if (_gbAtopStatus[AUD_ADC_MICIN] == TRUE)
                {
                    ERRMSG("Function - %s # %d - Can't switch to ePath %d when AUD_ADC_MICIN is active, error !] \n", __func__, __LINE__, (int)ePath);
                    return FALSE;
                }

                break;

            case AUD_ADC_MICIN:
                if (_gbAtopStatus[AUD_ADC_LINEIN] == TRUE)
                {
                    ERRMSG("Function - %s # %d - Can't switch to ePath %d when AUD_ADC_LINEIN is active, error !] \n", __func__, __LINE__, (int)ePath);
                    return FALSE;
                }
                break;

            case AUD_DAC_LINEOUT:
                // Do nothing
                break;

            default:
                ERRMSG("Function - %s # %d - ePath = %d, error !] \n", __func__, __LINE__, (int)ePath);
                return FALSE;
        }

        if ( _gbAtopStatus[ePath] == FALSE )
        {
            _HalAudAtopSwitch(ePath, TRUE);
        }
        else
        {
            ERRMSG("Function - %s # %d - ePath %d has been opened before, error !] \n", __func__, __LINE__, (int)ePath);
        }

        return TRUE;
    }
}

BOOL HalAudAtopClose(AudAtopPath_e ePath)
{
    if(ePath < 0 || ePath > AUD_ATOP_NUM)
    {
        ERRMSG("Function - %s # %d - ePath = %d, error !] \n", __func__, __LINE__, (int)ePath);
        return FALSE;
    }
    else
    {
        switch(ePath)
        {
            case AUD_ADC_LINEIN:
                if(_gbAtopStatus[AUD_ADC_LINEIN])
                    _HalAudAtopSwitch(AUD_ADC_LINEIN, FALSE);
                break;
            case AUD_ADC_MICIN:
                if(_gbAtopStatus[AUD_ADC_MICIN])
                    _HalAudAtopSwitch(AUD_ADC_MICIN, FALSE);
                break;
            case AUD_DAC_LINEOUT:
                if(_gbAtopStatus[AUD_DAC_LINEOUT])
                    _HalAudAtopSwitch(AUD_DAC_LINEOUT, FALSE);
                break;
            default:
                ERRMSG("Function - %s # %d - ePath = %d, error !] \n", __func__, __LINE__, (int)ePath);
                return FALSE;
        }

        if( !(_gbAdcActive||_gbDacActive) )
        {
            _HalAudAtopEnableRef(FALSE);
        }

        return TRUE;
    }
}

BOOL HalAudAtopMicAmpGain(AudAdcSel_e eAdcSel, U16 nSel, S8 ch)
{
    U16 nGain, nRegMsk, nPos, nPos_ext;

    if (ch == 0)
    {
        nRegMsk = REG_SEL_MICGAIN_STG_L_MSK;
        nPos = REG_SEL_MICGAIN_STG1_L_POS;
        nPos_ext = REG_SEL_MICGAIN_STG2_L_POS;
    }
    else if (ch == 1)
    {
        nRegMsk = REG_SEL_MICGAIN_STG_R_MSK;
        nPos = REG_SEL_MICGAIN_STG1_R_POS;
        nPos_ext = REG_SEL_MICGAIN_STG2_R_POS;
    }
    else
    {
        ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
        return FALSE;
    }

    if(nSel>5)
    {
        ERRMSG("Function - %s # %d - Gain nSel = %d not Support, error !] \n", __func__, __LINE__, (int)nSel);
        return FALSE;
    }

    if(nSel<2)
    {
        nGain = nSel;
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL08, nRegMsk, (nGain << nPos) | (1 << nPos_ext));
    }
    else
    {
        nGain = nSel - 2;
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL08, nRegMsk, (nGain << nPos));
    }

    return TRUE;
}

BOOL HalAudAtopSetAdcGain(AudAdcSel_e eAdcSel, U16 nSel, S8 ch)
{
	U16 nGain, nConfigValue, nRegMsk, nPos;

    if (ch == 0)
    {
        nConfigValue = 0x0001;
        nRegMsk = REG_SEL_GAIN_INMUX0_MSK;
        nPos = REG_SEL_GAIN_INMUX0_POS;
    }
    else if (ch == 1)
    {
        nConfigValue = 0x0002;
        nRegMsk = REG_SEL_GAIN_INMUX1_MSK;
        nPos = REG_SEL_GAIN_INMUX1_POS;
    }
    else
    {
        ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
        return FALSE;
    }

    if(nSel>9)
    {
        ERRMSG("AtopAdcGain - ERROR!! Level too large [%s ## %d] \n", __func__, __LINE__);
        return FALSE;
    }

    if(nSel > 7)
    {
        nSel = nSel - 2;
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL08, (REG_SEL_MICGAIN_INMUX_MSK & nConfigValue), nConfigValue<<REG_SEL_MICGAIN_INMUX_POS);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL08, (REG_SEL_MICGAIN_INMUX_MSK & nConfigValue), 0<<REG_SEL_MICGAIN_INMUX_POS);
    }

    if(nSel==2)
        nGain = 0x0;
    else if(nSel<2)
        nGain = 0x1 + nSel;
    else if(nSel)
        nGain = nSel;

    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL06, nRegMsk, (nGain << nPos));

    return TRUE;

}

BOOL HalAudAtopSetAdcMux(AudAdcSel_e eAdcSel)
{
    switch(eAdcSel)
    {
        case AUD_ADC_SEL_LINEIN:
            HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL05, REG_SEL_CH_INMUX0_MSK | REG_SEL_CH_INMUX1_MSK, 0x0<<REG_SEL_CH_INMUX0_POS | 0x0<<REG_SEL_CH_INMUX1_POS);
            _gnAdcSelect = eAdcSel;
            break;
        case AUD_ADC_SEL_MICIN:
            HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL05, REG_SEL_CH_INMUX0_MSK | REG_SEL_CH_INMUX1_MSK, 0x7<<REG_SEL_CH_INMUX0_POS | 0x7<<REG_SEL_CH_INMUX1_POS);
            _gnAdcSelect = eAdcSel;
            break;
        default:
            ERRMSG("Function - %s # %d - eAdcPath = %d, error !] \n", __func__, __LINE__, (int)eAdcSel);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudAtopSwap(BOOL bEnable)
{
    if(bEnable)
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ANALOG_LR_SWAP, REG_ANALOG_LR_SWAP);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ANALOG_LR_SWAP, ~REG_ANALOG_LR_SWAP);
    }

    return TRUE;
}

BOOL HalAudI2sSaveRxTdmWsPgm(BOOL bEnable)
{
    _gnI2sRxTdmWsPgm = (U8)bEnable;

    return TRUE;
}

BOOL HalAudI2sSaveRxTdmWsWidth(U8 nWsWidth)
{
    _gnI2sRxTdmWsWidth = nWsWidth;

    return TRUE;
}

BOOL HalAudI2sSaveRxTdmWsInv(BOOL bEnable)
{
    _gnI2sRxTdmWsInv = (U8)bEnable;

    return TRUE;
}

BOOL HalAudI2sSaveRxTdmChSwap(U8 nSwap_0_2, U8 nSwap_0_4)
{
    _gnI2sRxTdmChSwap = 0;

    if (nSwap_0_2 != 0)
    {
        _gnI2sRxTdmChSwap |= 0x01;
    }

    if (nSwap_0_4 != 0)
    {
        _gnI2sRxTdmChSwap |= 0x02;
    }

    return TRUE;
}

BOOL HalAudI2sSaveTxTdmWsPgm(BOOL bEnable)
{
    _gnI2sTxTdmWsPgm = (U8)bEnable;

    return TRUE;
}

BOOL HalAudI2sSaveTxTdmWsWidth(U8 nWsWidth)
{
    _gnI2sTxTdmWsWidth = nWsWidth;

    return TRUE;
}

BOOL HalAudI2sSaveTxTdmWsInv(BOOL bEnable)
{
    _gnI2sTxTdmWsInv = (U8)bEnable;

    return TRUE;
}

BOOL HalAudKeepAdcPowerOn(U8 u8AdcPowerOn)
{
    return TRUE;
}

BOOL HalAudKeepDacPowerOn(U8 u8DacPowerOn)
{
    return TRUE;
}

BOOL HalAudI2sSaveTxTdmChSwap(U8 nSwap_0_2, U8 nSwap_0_4)
{
    _gnI2sTxTdmChSwap = 0;

    if (nSwap_0_2 != 0)
    {
        _gnI2sTxTdmChSwap |= 0x01;
    }

    if (nSwap_0_4 != 0)
    {
        _gnI2sTxTdmChSwap |= 0x02;
    }

    return TRUE;
}

BOOL HalAudI2sSaveTxTdmActiveSlot(U8 nActiveSlot)
{
    _gnI2sTxTdmActiveSlot = nActiveSlot;

    return TRUE;
}

BOOL HalAudI2sSetTdmDetails(AudI2s_e eI2s)
{
    //

    return TRUE;
}

BOOL HalAudI2sSetKeepClk(BOOL bEnable)
{
    _gbI2sKeepClk = bEnable;

    return TRUE;
}

BOOL HalAudI2sSetPcmMode(BOOL bEnable)
{
    _gbI2sPcmMode = bEnable;

    return TRUE;
}

BOOL HalAudI2sSetRate(AudI2s_e eI2s, AudRate_e eRate)
{
    switch(eRate)
    {
        case AUD_RATE_8K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 0<<REG_CODEC_SEL_POS);
            break;

        case AUD_RATE_16K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 1<<REG_CODEC_SEL_POS);
            break;

        case AUD_RATE_32K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 2<<REG_CODEC_SEL_POS);
            break;

        case AUD_RATE_48K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 3<<REG_CODEC_SEL_POS);
            break;

        default:
            ERRMSG("HalAudI2sSetRate - ERROR I2S-Tx Sample Rate errors [%s ## %d] \n", __func__, __LINE__);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudI2sSetTdmMode(AudI2s_e eI2s, AudI2sMode_e eMode)
{
    U16 nTdmMode;

    switch(eMode)
    {
        case AUD_I2S_MODE_I2S:
            nTdmMode=0;
            break;
        case AUD_I2S_MODE_TDM:
            nTdmMode=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;
        case AUD_I2S_CODEC_TX:
            break;
        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }

    if ( nTdmMode == 1 )
    {
        ERRMSG("Function - %s # %d - I2S can't be TDM mode !] \n", __func__, __LINE__);
        return FALSE;
    }

    _gaI2sCfg[eI2s].eMode = eMode;
    return TRUE;
}

BOOL HalAudI2sSetMsMode(AudI2s_e eI2s, AudI2sMsMode_e eMsMode)
{
    U16 nMsMode;

    switch(eMsMode)
    {
        case AUD_I2S_MSMODE_MASTER:
            nMsMode=1;
            break;
        case AUD_I2S_MSMODE_SLAVE:
            nMsMode=0;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;
        case AUD_I2S_CODEC_TX:
            break;
        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }

    if ( nMsMode == 0 )
    {
        ERRMSG("Function - %s # %d - I2S can't be slave !] \n", __func__, __LINE__);
        return FALSE;
    }

    _gaI2sCfg[eI2s].eMsMode = eMsMode;

    return TRUE;
}

BOOL HalAudI2sSetFmt(AudI2s_e eI2s, AudI2sFmt_e eFmt)
{
    U16 nSel;
    switch(eFmt)
    {
        case AUD_I2S_FMT_I2S:
            nSel=0;
            break;
        case AUD_I2S_FMT_LEFT_JUSTIFY:
            nSel=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            HalBachWriteReg(BACH_REG_BANK1, BACH_CODEC_I2S_RX_CTRL, REG_I2S_FMT, (nSel?REG_I2S_FMT:0));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK1, BACH_CODEC_I2S_TX_CTRL, REG_I2S_FMT, (nSel?REG_I2S_FMT:0));
            break;

        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }
    _gaI2sCfg[eI2s].eFormat = eFmt;

    return TRUE;
}

BOOL HalAudI2sSetWidth(AudI2s_e eI2s, AudBitWidth_e eWidth)
{
    U16 nSel;
    switch(eWidth)
    {
        case AUD_BITWIDTH_16:
            nSel=0;
            break;
        case AUD_BITWIDTH_32:
            nSel=1;
            break;
        default:
            return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;

        case AUD_I2S_CODEC_TX:
            break;

        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }

    if ( nSel == 1 )
    {
        ERRMSG("Function - %s # %d - I2S can't be 32Bits !] \n", __func__, __LINE__);
        return FALSE;
    }

    _gaI2sCfg[eI2s].eWidth = eWidth;

    return TRUE;
}

BOOL HalAudI2sSetChannel(AudI2s_e eI2s, U16 nChannel)
{
    if ( nChannel != 2 )
    {
        ERRMSG("Function - %s # %d - I2S is always 2 channel, nChannel = %d, error !] \n", __func__, __LINE__, nChannel);
        return FALSE;
    }

    _gaI2sCfg[eI2s].nChannelNum = nChannel;
    return TRUE;
}

BOOL HalAudI2sSetWireMode(AudI2s_e eI2s, AudWireMode_e eWireMode)
{
    if ( eWireMode == AUD_I2S_WIRE_6 )
    {
        ERRMSG("Function - %s # %d - I2S is always 4 wire, eWireMode = %d, error !] \n", __func__, __LINE__, eWireMode);
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;
        case AUD_I2S_CODEC_TX:
            break;
        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }

    _gaI2sCfg[eI2s].eWireMode = eWireMode;
    return TRUE;
}

BOOL HalAudI2sSetTdmSlotConfig(AudI2s_e eI2s, U16 nSlotMsk, AudTdmChnMap_e eMap)
{
    // ERRMSG("Function - %s # %d - I6 don't have TDM, error !] \n", __func__, __LINE__);

    return TRUE;
}

BOOL HalAudI2sEnableMck(AudI2s_e eI2s, BOOL bEnable)
{
#if 0
    if ( bEnable != FALSE )
    {
        ERRMSG("Function - %s # %d - I6 don't have MCK, error !] \n", __func__, __LINE__);
        return FALSE;
    }

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;
        case AUD_I2S_CODEC_TX:
            break;
        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }
#endif
    return TRUE;
}

BOOL HalAudI2sSetMck(AudI2s_e eI2s, AudI2sMck_e MckMux)
{
    // ERRMSG("Function - %s # %d - I6 don't have MCK error !] \n", __func__, __LINE__);

    return TRUE;
}

//for I2S master
BOOL HalAudI2sEnable(AudI2s_e eI2s, BOOL bEnable)
{
    // I6 no enable setting.

    return TRUE;
}

BOOL HalAudDmaSetRate(AudDmaChn_e eDmaChannel, AudRate_e eRate)
{
    switch(eDmaChannel)
    {
        //ADC rate should be set according to the DMA writer rate
        case AUD_DMA_WRITER1:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_WRITER_SEL_MSK, 0<<REG_WRITER_SEL_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_3_SEL_MSK, 0<<REG_CIC_3_SEL_POS);
                    break;

                case AUD_RATE_16K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_WRITER_SEL_MSK, 1<<REG_WRITER_SEL_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_3_SEL_MSK, 1<<REG_CIC_3_SEL_POS);
                    break;

                case AUD_RATE_32K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_WRITER_SEL_MSK, 2<<REG_WRITER_SEL_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_3_SEL_MSK, 2<<REG_CIC_3_SEL_POS);
                    break;

                case AUD_RATE_48K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_WRITER_SEL_MSK, 3<<REG_WRITER_SEL_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_CIC_3_SEL_MSK, 3<<REG_CIC_3_SEL_POS);
                    break;

                default:
                    ERRMSG("Function - %s # %d - AUD_DMA_WRITER1 eRate = %d, error !] \n", __func__, __LINE__, (int)eRate);
                    return FALSE;
            }

#if (AI_GAIN_TUNE == TRUE)
            // DMIC:
            // +0.5dB
            //
            // AMIC & SRC:
            // 48K: +0.5dB
            // Others: +2.0dB

            if (_gbIsDmicUsed == TRUE)
            {
                HalAudDpgaSetGainOffset(AUD_DPGA_ADC, -1 ,0);
                HalAudDpgaSetGainOffset(AUD_DPGA_AEC1, -1 ,1);
            }
            else
            {
                if (eRate == AUD_RATE_48K)
                {
                    //AMIC
                    HalAudDpgaSetGainOffset(AUD_DPGA_ADC, -1 ,0);
                    HalAudDpgaSetGainOffset(AUD_DPGA_AEC1, -1 ,1);
                    //SRC
                    HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,0);
                    HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,1);
                }
                else
                {
                    //AMIC
                    HalAudDpgaSetGainOffset(AUD_DPGA_ADC, -4 ,0);
                    HalAudDpgaSetGainOffset(AUD_DPGA_AEC1, -4 ,1);
                    //SRC
                    HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, -3 ,0);
                    HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, -3 ,1);
                }
            }
#endif

            break;

        case AUD_DMA_READER1:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 0<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_11K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 1<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_12K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 2<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_16K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 3<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_22K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 4<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_24K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 5<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_32K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 6<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_44K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 7<<REG_SRC1_SEL_POS);
                    break;

                case AUD_RATE_48K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC1_SEL_MSK, 8<<REG_SRC1_SEL_POS);
                    break;

                default:
                    ERRMSG("Function - %s # %d - AUD_DMA_READER1 eRate = %d, error !] \n", __func__, __LINE__, (int)eRate);
                    return FALSE;
            }
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudDmaGetRate(AudDmaChn_e eDmaChannel, AudRate_e *eRate, U32 *nRate)
{
    ERRMSG("Function - %s # %d - have NOT support yet, warning !] \n", __func__, __LINE__);

    return TRUE;
}

BOOL HalAudDmaReset(AudDmaChn_e eDmaChannel)
{
    _HalAudDmaReInit(eDmaChannel);

    return TRUE;
}

BOOL HalAudDmaEnable(AudDmaChn_e eDmaChannel, BOOL bEnable)
{
    switch ( eDmaChannel )
    {
        case AUD_DMA_READER1:
            if(bEnable)
            {
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_ENABLE, REG_ENABLE); //reader prefetch enable, it should be enabled before reader enable
                _uDelay(10);
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_ENABLE, REG_RD_ENABLE);
            }
            else
            {
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_ENABLE, 0);
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_ENABLE, 0); //reader prefetch enable, it has to be disabled before dma init
            }
            break;

        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_ENABLE, (bEnable ? REG_WR_ENABLE : 0));
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudDmaPause(AudDmaChn_e eDmaChannel)
{
    HalAudDmaEnable(eDmaChannel, FALSE);

    return TRUE;
}

BOOL HalAudDmaResume(AudDmaChn_e eDmaChannel)
{
    HalAudDmaEnable(eDmaChannel, TRUE);

    return TRUE;
}

BOOL HalAudDmaIntEnable(AudDmaChn_e eDmaChannel, BOOL bDatatrigger, BOOL bDataboundary)
{
    BOOL ret = TRUE;

    if (eDmaChannel < AUD_DMA_READER1)
        ret = _HalAudDmaWrIntEnable(eDmaChannel, bDatatrigger, bDataboundary);
    else
        ret = _HalAudDmaRdIntEnable(eDmaChannel, bDatatrigger, bDataboundary);

    return ret;
}

BOOL HalAudDmaGetInt(AudDmaChn_e eDmaChannel, BOOL *bDatatrigger, BOOL *bDataboundary)
{
    BOOL ret = TRUE;

    if (eDmaChannel < AUD_DMA_READER1)
        ret = _HalAudDmaGetWrInt(eDmaChannel, bDatatrigger, bDataboundary);
    else
        ret = _HalAudDmaGetRdInt(eDmaChannel, bDatatrigger, bDataboundary);

    return ret;
}

BOOL HalAudDmaClearInt(AudDmaChn_e eDmaChannel)
{
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            //DMA writer1 full flag clear / DMA writer1 local buffer full flag clear
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_WR_FULL_FLAG_CLR, REG_WR_FULL_FLAG_CLR);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_WR_FULL_FLAG_CLR, 0);
            break;

        case AUD_DMA_READER1:
            //DMA reader1 empty flag clear / DMA reader1 local buffer empty flag clear
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_RD_EMPTY_FLAG_CLR, REG_RD_EMPTY_FLAG_CLR);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_0, REG_RD_EMPTY_FLAG_CLR, 0 );
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

U32 HalAudDmaGetLevelCnt(AudDmaChn_e eDmaChannel)
{
    U32 nRawMiuUnitLevelCnt;

    //
    nRawMiuUnitLevelCnt = _HalAudDmaGetRawMiuUnitLevelCnt(eDmaChannel);

    //
    if(eDmaChannel < AUD_DMA_READER1) //dma writer
    {
        nRawMiuUnitLevelCnt = ((nRawMiuUnitLevelCnt>DMA_LOCALBUF_LINE)? (nRawMiuUnitLevelCnt-DMA_LOCALBUF_LINE):0); //level count contains the local buffer data size
    }

    return ( nRawMiuUnitLevelCnt * MIU_WORD_BYTE_SIZE );
}

U32 HalAudDmaGetRawLevelCnt(AudDmaChn_e eDmaChannel)
{
    U32 nRawMiuUnitLevelCnt;

    //
    nRawMiuUnitLevelCnt = _HalAudDmaGetRawMiuUnitLevelCnt(eDmaChannel);

    return ( nRawMiuUnitLevelCnt * MIU_WORD_BYTE_SIZE );
}

U32 HalAudDmaTrigLevelCnt(AudDmaChn_e eDmaChannel, U32 nDataSize)
{
    U16 nConfigValue = 0;
    nConfigValue = (U16)((nDataSize / MIU_WORD_BYTE_SIZE) & REG_WR_SIZE_MSK);
    nDataSize = nConfigValue * MIU_WORD_BYTE_SIZE;

    if (nConfigValue > 0)
    {
        switch(eDmaChannel)
        {
            case AUD_DMA_WRITER1:
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_12, 0xFFFF, nConfigValue);
                nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9);
                if(nConfigValue & REG_WR_TRIG)
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_TRIG, 0);
                }
                else
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_TRIG, REG_WR_TRIG);
                }
                break;

            case AUD_DMA_READER1:
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_4, 0xFFFF, nConfigValue);
                nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1);
                if(nConfigValue & REG_RD_TRIG)
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_TRIG, 0);
                }
                else
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_TRIG, REG_RD_TRIG);
                }
                break;

            default:
                ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
                return 0;
        }

        return nDataSize;
    }

    return 0;
}

BOOL HalAudDmaSetPhyAddr(AudDmaChn_e eDmaChannel, U32 nBufAddrOffset, U32 nBufSize)
{
    U16 nMiuAddrLo, nMiuAddrHi, nMiuSize;

    if((nBufSize/MIU_WORD_BYTE_SIZE) > ((1<<16)-1))
    {
        ERRMSG("Function - %s # %d - eDmaChannel = %d, BufSize overflow ! error !] \n", __func__, __LINE__, nBufSize);
        return FALSE;
    }

    if((nBufSize%MIU_WORD_BYTE_SIZE) != 0)
    {
        ERRMSG("Function - %s # %d - Buf size must be multiple of MIU_WORD_BYTE_SIZE, nBufSize = %u, error !] \n", __func__, __LINE__, nBufSize);
        return FALSE;
    }

    if((nBufAddrOffset%MIU_WORD_BYTE_SIZE) != 0)
    {
        ERRMSG("Function - %s # %d - Buf addr must be multiple of MIU_WORD_BYTE_SIZE, nBufAddrOffset = %u, error !] \n", __func__, __LINE__, nBufAddrOffset);
        return FALSE;
    }

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_WR_BASE_ADDR_LO_MSK);
            nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK);
            nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK);

            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_BASE_ADDR_LO_MSK, nMiuAddrLo);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_10, REG_WR_BASE_ADDR_HI_MSK, nMiuAddrHi);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_11, 0xFFFF, nMiuSize);
            break;

        case AUD_DMA_READER1:
            nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_RD_BASE_ADDR_LO_MSK);
            nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_RD_BASE_ADDR_HI_OFFSET) & REG_RD_BASE_ADDR_HI_MSK);
            nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_RD_BUFF_SIZE_MSK);

            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_1, REG_RD_BASE_ADDR_LO_MSK, nMiuAddrLo);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_2, REG_RD_BASE_ADDR_HI_MSK, nMiuAddrHi);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_3, 0xFFFF, nMiuSize);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

BOOL HalAudDmaWrSetThreshold(AudDmaChn_e eDmaChannel, U32 nOverrunTh)
{
    U16 nMiuOverrunTh;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_OVERRUN_TH_MSK);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_13, 0xFFFF, nMiuOverrunTh);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

BOOL HalAudDmaRdSetThreshold(AudDmaChn_e eDmaChannel, U32 nUnderrunTh)
{
    U16 nMiuUnderrunTh;

    switch(eDmaChannel)
    {
        case AUD_DMA_READER1:
            nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_UNDERRUN_TH_MSK);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_6, 0xFFFF, nMiuUnderrunTh);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

BOOL HalAudDmaGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbDatatrigger, BOOL *pbDataboundary, BOOL *pbLocalData)
{
    BOOL ret = TRUE;

    if (eDmaChannel < AUD_DMA_READER1)
        ret = _HalAudDmaWrGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);
    else
        ret = _HalAudDmaRdGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);

    return ret;
}

BOOL HalAudDmaWrConfigMch(AudMchSet_e eMchSet, U16 nChNum, AudMchClkRef_e eMchClkRef, const int *mch_sel)
{
    U16 nChNumSetting = 0, nClkRefSetting = 0, nChSel01 = 0, nChSel23 = 0, nChSel4567 = 0;
    BOOL bRet = TRUE;

    //
    if (nChNum==1)
    {
        nChNumSetting = 0;
    }
    else if (nChNum==2)
    {
        nChNumSetting = 1;
    }
    else if (nChNum==4)
    {
        nChNumSetting = 2;
    }
    else if (nChNum==8)
    {
        nChNumSetting = 3;
    }
    else
    {
        ERRMSG(" Set Mch Channel Num - ERROR! [%s ## %d] nCh = %d\n", __func__, __LINE__, nChNum);
        return FALSE;
    }

    //
    switch(eMchClkRef)
    {
        case AUD_MCH_CLK_REF_ADC:
            nClkRefSetting = 1;
            break;
        default: // AUD_MCH_SEL_NULL
            ERRMSG(" Set Mch Clock Ref - ERROR! [%s ## %d] eMchClkRef = %d\n", __func__, __LINE__, eMchClkRef);
            return FALSE;
    }

    //
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchChBind_e)AUD_MCH_CH_BIND_01, (AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_01], &nChSel01);
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchChBind_e)AUD_MCH_CH_BIND_23, (AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_23], &nChSel23);
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchChBind_e)AUD_MCH_CH_BIND_4567, (AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_4567], &nChSel4567);

    if ( bRet != TRUE )
    {
        return FALSE;
    }

    switch(eMchSet)
    {
        case AUD_MCH_SET_W1:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH47_SEL_MSK, (nChSel4567<<BACH_DMA_WR_CH47_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH23_SEL_MSK, (nChSel23<<BACH_DMA_WR_CH23_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH01_SEL_MSK, (nChSel01<<BACH_DMA_WR_CH01_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_VALID_SEL_MSK, (nClkRefSetting<<BACH_DMA_WR_VALID_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH_MODE_MSK, (nChNumSetting<<BACH_DMA_WR_CH_MODE_POS));
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudDmaMonoEnable(AudDmaChn_e eDmaChannel, BOOL bEnable)
{
    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA1_WR_MONO, (bEnable?REG_DMA1_WR_MONO:0));
            break;

        case AUD_DMA_READER1:
            if(bEnable)
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA1_RD_MONO | REG_DMA1_RD_MONO_COPY, REG_DMA1_RD_MONO | REG_DMA1_RD_MONO_COPY);
            else
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA1_RD_MONO | REG_DMA1_RD_MONO_COPY, 0);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

void HalAudDpgaCtrl(AudDpga_e eDpga, BOOL bEnable, BOOL bMute, BOOL bFade)
{
    U8 nAddr = 0;
    U16 nConfigValue;

    switch(eDpga)
    {
        case AUD_DPGA_MMC1:
            nAddr = BACH_MMC1_DPGA_CFG1;
            break;

        case AUD_DPGA_ADC:
            nAddr = BACH_ADC_DPGA_CFG1;
            break;

        case AUD_DPGA_AEC1:
            nAddr = BACH_AEC1_DPGA_CFG1;
            break;

        case AUD_DPGA_DEC1:
            nAddr = BACH_MMCDEC1_DPGA_CFG1;
            break;

        default:
            ERRMSG("Function - %s # %d - eDpga = %d, error !] \n", __func__, __LINE__, eDpga);
            return;
    }

    nConfigValue = 0;
    if(bEnable)
        nConfigValue |= DPGA_EN;
    if(bMute)
        nConfigValue |= MUTE_2_ZERO;
    if(bFade)
        nConfigValue |= FADING_EN;

    HalBachWriteReg(BACH_REG_BANK1, nAddr, (DPGA_EN | MUTE_2_ZERO | FADING_EN), nConfigValue);
    //Step Size for fading function, 4: 16 sample, 5: 32 sample, 6: 64 sample
    HalBachWriteReg(BACH_REG_BANK1, nAddr, STEP_MSK, 6<<STEP_POS);
}

BOOL HalAudDpgaSetGain(AudDpga_e eDpga, S8 gain, S8 ch)
{
    U8 nAddr;
    U8 nGainIdx;
    U16 nRegMsk, nValue;

    // Audio input, Each channel use one DPGA.
    if (eDpga == AUD_DPGA_ADC)
    {
        if (ch != 0)
        {
            return TRUE;
        }
    }

    if (eDpga == AUD_DPGA_AEC1)
    {
        if (ch != 1)
        {
            return TRUE;
        }
    }

    _HalAudDpgaCalGain(gain, &nGainIdx);

    switch(eDpga)
    {
        case AUD_DPGA_MMC1:
            nAddr = BACH_MMC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0xFF << REG_GAIN_R_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_L_POS);
            }
            else if (ch == 1)
            {
                nValue = nValue & (0xFF << REG_GAIN_L_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_R_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_ADC:
            nAddr = BACH_ADC_DPGA_CFG2;
            nRegMsk = (REG_GAIN_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0xFF << REG_GAIN_R_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_L_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_AEC1:
            nAddr = BACH_AEC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 1)
            {
                nValue = nValue & (0xFF << REG_GAIN_R_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_L_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_DEC1:
            nAddr = BACH_MMCDEC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0xFF << REG_GAIN_R_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_L_POS);
            }
            else if (ch == 1)
            {
                nValue = nValue & (0xFF << REG_GAIN_L_POS);
                nValue = nValue | (nGainIdx << REG_GAIN_R_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        default:
            ERRMSG("Function - %s # %d - eDpga = %d, error !] \n", __func__, __LINE__, eDpga);
            return FALSE;
    }

    HalBachWriteReg(BACH_REG_BANK1, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDpgaSetGainOffset(AudDpga_e eDpga, S8 gainRegValue, S8 ch)
{
    U8 nAddr;
    U8 nGainIdx;
    U16 nRegMsk, nValue;
    S8 nGain;

    // Audio input, Each channel use one DPGA.
    if (eDpga == AUD_DPGA_ADC)
    {
        if (ch != 0)
        {
            return TRUE;
        }
    }

    if (eDpga == AUD_DPGA_AEC1)
    {
        if (ch != 1)
        {
            return TRUE;
        }
    }

    //
    // 5bits: 15 ~ -16 (0xF ~ 0x10)
    // -7.5dB ~ 8dB

    nGain = gainRegValue;
    if (nGain > 15)
    {
        nGain = 15;
    }
    else if (nGain < -16)
    {
        nGain = -16;
    }

    nGainIdx = (U8)nGain;

    switch(eDpga)
    {
        case AUD_DPGA_MMC1:
            nAddr = BACH_MMC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0x1F << REG_OFFSET_R_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_L_POS);
            }
            else if (ch == 1)
            {
                nValue = nValue & (0x1F << REG_OFFSET_L_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_R_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_ADC:
            nAddr = BACH_ADC_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0x1F << REG_OFFSET_R_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_L_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_AEC1:
            nAddr = BACH_AEC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 1)
            {
                nValue = nValue & (0x1F << REG_OFFSET_R_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_L_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        case AUD_DPGA_DEC1:
            nAddr = BACH_MMCDEC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if (ch == 0)
            {
                nValue = nValue & (0x1F << REG_OFFSET_R_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_L_POS);
            }
            else if (ch == 1)
            {
                nValue = nValue & (0x1F << REG_OFFSET_L_POS);
                nValue = nValue | (nGainIdx << REG_OFFSET_R_POS);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        default:
            ERRMSG("Function - %s # %d - eDpga = %d, error !] \n", __func__, __LINE__, eDpga);
            return FALSE;
    }

    HalBachWriteReg(BACH_REG_BANK1, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDpgaSetGainFading(AudDpga_e eDpga, U8 nFading, S8 ch)
{
    U8 nAddr;
    U16 nRegMsk, nValue;
    U8 nFadingEn = 1, nFadingValue = 0;

    // 0:OFF    1~7:reg value 0~6
    if ( nFading > 7 )
    {
        ERRMSG("Function - %s # %d - nFading = %d, error !] \n", __func__, __LINE__, eDpga);
    }

    if ( nFading == 0 ) // OFF
    {
        nFadingEn = 0;
        nFadingValue = 0;
    }
    else
    {
        nFadingEn = 1;
        nFadingValue = nFading - 1;
    }

    switch(eDpga)
    {
        case AUD_DPGA_MMC1:
            nAddr = BACH_MMC1_DPGA_CFG1;
            nRegMsk = (STEP_MSK | FADING_EN);
            nValue = HalBachReadReg(BACH_REG_BANK1, nAddr);

            if ((ch == 0) || (ch == 1))
            {
                nValue = nValue & (~nRegMsk);
                nValue = nValue | (nFadingValue<<STEP_POS) | (nFadingEn<<1);
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
            }

            break;

        default:
            ERRMSG("Function - %s # %d - eDpga = %d, error !] \n", __func__, __LINE__, eDpga);
            return FALSE;
    }

    HalBachWriteReg(BACH_REG_BANK1, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDigMicEnable(BOOL bEn)
{
    HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_EN, (bEn ? REG_DIGMIC_EN :0) );

    _gbIsDmicUsed = bEn;

    return TRUE;
}

BOOL HalAudDigMicSetChannel(U16 nCh)
{
    //if (nCh != 2)
    //{
        //ERRMSG("Function - %s # %d - nCh = %d, error !] \n", __func__, __LINE__, nCh);
        //return FALSE;
    //}

    return TRUE;
}

BOOL HalAudDigMicSetRate(AudRate_e eRate)
{
    //nConfigValue = InfinityReadReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0);

    //if(nConfigValue & REG_DIGMIC_CLK_MODE)// 0:4M, 1:2M
    if ( 0 ) // 2M
    {
        switch(eRate)
        {
        case AUD_RATE_8K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_SEL_MSK, 1<<REG_DIGMIC_SEL_POS);
            break;
        case AUD_RATE_16K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_SEL_MSK, 2<<REG_DIGMIC_SEL_POS);
            break;
        default:
            ERRMSG("Function - %s # %d - eRate = %d, error !] \n", __func__, __LINE__, eRate);
            return FALSE;
        }
    }
    else // 4M
    {
        switch(eRate)
        {
        case AUD_RATE_8K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_SEL_MSK, 0<<REG_DIGMIC_SEL_POS);
            break;
        case AUD_RATE_16K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_SEL_MSK, 1<<REG_DIGMIC_SEL_POS);
            break;
        case AUD_RATE_32K:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DIG_MIC_CTRL0, REG_DIGMIC_SEL_MSK, 2<<REG_DIGMIC_SEL_POS);
            break;
        default:
            ERRMSG("Function - %s # %d - eRate = %d, error !] \n", __func__, __LINE__, eRate);
            return FALSE;
        }
    }

    return TRUE;
}

BOOL HalAudDigMicSetGain(S8 gain, S8 ch) // s8Gain: -64 ~ 30, dB: -63.5 ~ 30, reg range: 127,126,124,122,...,-56,-58,-60
{
    if (gain > BACH_DPGA_GAIN_MAX_DB)
    {
        ERRMSG("Function - %s # %d - gain = %d, max = %d, error !] \n", __func__, __LINE__, gain, BACH_DPGA_GAIN_MAX_DB);

        gain = BACH_DPGA_GAIN_MAX_DB;
    }
    else if (gain < BACH_DPGA_GAIN_MIN_DB)
    {
        ERRMSG("Function - %s # %d - gain = %d, min = %d,error !] \n", __func__, __LINE__, gain, BACH_DPGA_GAIN_MIN_DB);

        gain = BACH_DPGA_GAIN_MIN_DB;
    }

    if (ch == 0)
    {
        HalAudDpgaSetGain(AUD_DPGA_ADC, gain, ch);
    }
    else if (ch == 1)
    {
        HalAudDpgaSetGain(AUD_DPGA_AEC1, gain, ch);
    }
    else
    {
        ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
    }

    return TRUE;
}

BOOL HalAudCfgI2sIfPrivateCb( void *pPriData )
{
    //

    return TRUE;
}

BOOL HalAudDmaSineGen(AudDmaChn_e eDmaChannel, BOOL bEnable, U8 freq, U8 gain)
{
    switch ( eDmaChannel )
    {
        case AUD_DMA_READER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_EN, (bEnable ? REG_SINE_GEN_EN : 0));
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_L|REG_SINE_GEN_R, REG_SINE_GEN_L|REG_SINE_GEN_R);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_RD_WR, 0);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_GAIN_MSK, gain<<REG_SINE_GEN_GAIN_POS);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_FREQ_MSK, freq<<REG_SINE_GEN_FREQ_POS);
            break;

        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_EN, (bEnable ? REG_SINE_GEN_EN : 0));
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_L|REG_SINE_GEN_R, REG_SINE_GEN_L|REG_SINE_GEN_R);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_RD_WR, 1);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_GAIN_MSK, gain<<REG_SINE_GEN_GAIN_POS);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_FREQ_MSK, freq<<REG_SINE_GEN_FREQ_POS);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAud2Hdmi(BOOL bEnable)
{
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Private ( Only for Hal layer )
//
////////////////////////////////////////////////////////////////////////////
BOOL HalAudPriSetPadMuxI2s(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_I2S_TX] = nPadMuxMode;
    _gaPadMuxMode[AUD_PAD_MUX_I2S_RX] = nPadMuxMode;
    return TRUE;
}

BOOL HalAudPriSetPadMuxDmic(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_DMIC] = nPadMuxMode;
    return TRUE;
}

// template_fp
BOOL HalAudPriTemplateFp(S32 temp)
{
    DBGMSG("%s --- %d \n",__func__,temp);
    return TRUE;
}
