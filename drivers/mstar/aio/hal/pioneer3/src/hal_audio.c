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
#define I2S_TDM_RX_PADMUX_OFFSET 4
#define I2S_TDM_TX_PADMUX_OFFSET 8
#define I2S_MODE_PADMUX_OFFSET 12
#define I2S_MCK_PADMUX_OFFSET 0
#define DIGMIC_PADMUX_OFFSET 0

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
    AUD_DEF_I2S_TX_PMUX,
    AUD_DEF_I2S_RX_PMUX,
    AUD_DEF_I2S_MCK_PMUX,
    AUD_DEF_I2S_RXTX_PMUX
};

static BOOL _gbAdcActive;
static BOOL _gbDacActive;
static BOOL _gbAtopStatus[AUD_ATOP_NUM];
static AudI2sCfg_t _gaI2sCfg[AUD_I2S_NUM];
//static AudPriI2sCfg_t _gaPriI2sCfg[AUD_I2S_NUM];
static AudAtopPath_e _gnAdcSelect;

static BOOL _gbI2sMckRxActive, _gbI2sMckTxActive;
static BOOL _gbI2sModeRxActive, _gbI2sModeTxActive;
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

static AudRate_e _geDmicSmpRat = AUD_RATE_NULL;
static S8 _gnDmicGain[4] = {0};

static void _HalAudSysInit(void);
static void _HalAudAtopInit(void);
static void _HalAudDmicRegInit(void);
static BOOL _HalAudDmaFirstInit(void);
static BOOL _HalAudDmaWrMchInit(void);
static void _HalAudDpgaInit(void);
static void _HalAudPadMuxInit(void);
static BOOL _HalAudIntEnable(void);
static void _HalAudAtopAdc(BOOL bEnable);
static void _HalAudAtopDac(BOOL bEnable);
static void _HalAudAtopEnableRef(BOOL bEnable);
static void _HalAudAtopEnablePreamp(BOOL bEnable);
static BOOL _HalAudAtopSwitch(AudAtopPath_e ePath, BOOL bEnable);
static BOOL _HalAudDmaReInit(AudDmaChn_e eDmaChannel);
static BOOL _HalAudDmaRdIntEnable(AudDmaChn_e eDmaChannel, BOOL bUnderrun, BOOL bEmpty);
static BOOL _HalAudDmaWrIntEnable(AudDmaChn_e eDmaChannel, BOOL bOverrun, BOOL bFull);
static BOOL _HalAudDmaGetRdInt(AudDmaChn_e eDmaChannel, BOOL *bUnderrun, BOOL *bEmpty);
static BOOL _HalAudDmaGetWrInt(AudDmaChn_e eDmaChannel, BOOL *bOverrun, BOOL *bFull);
static BOOL _HalAudDmaRdGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbUnderrun, BOOL *pbEmtpy, BOOL *pbLocalEmpty);
static BOOL _HalAudDmaWrGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbOverrun, BOOL *pbFull, BOOL *pbLocalFull);
static U32 _HalAudDmaGetRawMiuUnitLevelCnt(AudDmaChn_e eDmaChannel);
static BOOL _HalAudDmaGetMchSelConfigValue(AudMchSel_e eMchSel, U16 *nChSel);
static void _HalAudDpgaCalGain(S8 s8Gain, U8 *pU8GainIdx);
static U32 _HalAudBckCalculate(AudRate_e eRate, AudBitWidth_e eWidth, U16 Chn);
static BOOL _HalAudI2sSetBck( AudI2s_e eI2s, U32 nNfValue );
static BOOL _HalAudMckCalculate(AudI2s_e eI2s, AudI2sMck_e eMck);
static BOOL _HalAudI2sSetClkRef(AudI2s_e eI2s, AudI2sClkRef_e eI2sClkRef);
static void _HalAudDmaSetMiuSel(AudDmaChn_e eDmaChannel, U32 nBufAddrOffset);

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
    HalBachWriteRegByte(0x0015032d, 0xff, 0xc0); //disable ICG function(hw bug, will fix in i6)
    HalBachWriteRegByte(0x0015031d, 0xff, 0x00);
    HalBachWriteRegByte(0x00150226, 0xff, 0x00);
    HalBachWriteRegByte(0x00150227, 0xff, 0xd4);

    //correct IC default value
    HalBachWriteRegByte(0x00150248, 0xff, 0x07);
    HalBachWriteRegByte(0x00150249, 0xff, 0x00);
    HalBachWriteRegByte(0x00150250, 0xff, 0x07);
    //DAC2 EP
    HalBachWriteRegByte(0x001502CE, 0xff, 0x00);
    HalBachWriteRegByte(0x001502CF, 0xff, 0x01);
    HalBachWriteRegByte(0x001502D0, 0xff, 0x04);
    HalBachWriteRegByte(0x001502D1, 0xff, 0x02);
    HalBachWriteRegByte(0x001502D2, 0xff, 0x15);
    HalBachWriteRegByte(0x001502D3, 0xff, 0x80);

    //I2S TDM settings
    HalBachWriteRegByte(0x00150378, 0xff, 0x1C);
    HalBachWriteRegByte(0x00150379, 0xff, 0x11);

    //ADC2 HPF
    HalBachWriteRegByte(0x001502c8, 0xff, 0xFD);
    HalBachWriteRegByte(0x001502c9, 0xff, 0x09);
    HalBachWriteRegByte(0x001502cc, 0xff, 0x00);
    HalBachWriteRegByte(0x001502cd, 0xff, 0xf0);
}

static void _HalAudAtopInit(void)
{
    S32 i;
    HalBachWriteRegByte(0x00103400, 0xff, 0x34); //[5] enable DAC2 clk
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
    //ADC2
    HalBachWriteRegByte(0x00103412, 0xff, 0x04);
    HalBachWriteRegByte(0x00103413, 0xff, 0x02);
    HalBachWriteRegByte(0x00103414, 0xff, 0x10);
    HalBachWriteRegByte(0x00103415, 0xff, 0x00);
    HalBachWriteRegByte(0x00103416, 0xff, 0x00);
    HalBachWriteRegByte(0x00103417, 0xff, 0x00);
    HalBachWriteRegByte(0x00103418, 0xff, 0x77);
    HalBachWriteRegByte(0x00103419, 0xff, 0x00);
    HalBachWriteRegByte(0x0010341A, 0xff, 0x00);
    HalBachWriteRegByte(0x0010341B, 0xff, 0x30);
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
    //digital mic settings
    HalBachWriteRegByte(0x0015040c, 0xff, 0x00);
    HalBachWriteRegByte(0x0015040d, 0xff, 0x80);
    HalBachWriteRegByte(0x0015040c, 0xff, 0x00);
    HalBachWriteRegByte(0x0015040d, 0xff, 0x00);
    HalBachWriteRegByte(0x00150402, 0xff, 0x21);
    HalBachWriteRegByte(0x00150403, 0xff, 0x00);
    HalBachWriteRegByte(0x00150406, 0xff, 0x00);
    HalBachWriteRegByte(0x00150407, 0xff, 0x00);
    HalBachWriteRegByte(0x00150408, 0xff, 0x0a);
    HalBachWriteRegByte(0x00150409, 0xff, 0x00);
    HalBachWriteRegByte(0x0015040c, 0xff, 0x1f);
    HalBachWriteRegByte(0x0015040d, 0xff, 0x01);
    HalBachWriteRegByte(0x00150420, 0xff, 0x00);
    HalBachWriteRegByte(0x00150421, 0xff, 0x00);
    HalBachWriteRegByte(0x00150422, 0xff, 0x01);
    HalBachWriteRegByte(0x00150423, 0xff, 0x00);
    HalBachWriteRegByte(0x00150424, 0xff, 0x00);
    HalBachWriteRegByte(0x00150425, 0xff, 0x80);
    HalBachWriteRegByte(0x00150426, 0xff, 0x01);
    HalBachWriteRegByte(0x00150427, 0xff, 0x00);
    HalBachWriteRegByte(0x00150428, 0xff, 0x00);
    HalBachWriteRegByte(0x00150429, 0xff, 0x80);
    HalBachWriteRegByte(0x00150440, 0xff, 0x01);
    HalBachWriteRegByte(0x00150441, 0xff, 0x00);

    //enable HPF for DMIC
    HalBachWriteRegByte(0x0015039e, 0xff, 0x00);
    HalBachWriteRegByte(0x0015039f, 0xff, 0x00);
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

    //reset DMA2 interal register
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_4, 0xFFFF, 0);		                //reset DMA 2 read size
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_12, 0xFFFF, 0);		                //reset DMA 2 write size

    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_SW_RST_DMA, REG_SW_RST_DMA);  //DMA 2 software reset
    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_SW_RST_DMA, 0);

    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK),
                    (REG_PRIORITY_KEEP_HIGH | REG_RD_LEVEL_CNT_LIVE_MASK));

    return TRUE;
}

static BOOL _HalAudDmaWrMchInit(void)
{
    //AUD_MCH_SET_W1
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_MCH_DEBUG_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_BIT_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_MCH_32B_EN, BACH_MCH_32B_EN);
    //AUD_MCH_SET_W2
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_MCH_DEBUG_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_BIT_MODE, 0);
    HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_MCH_32B_EN, BACH_MCH_32B_EN);

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
    _gbI2sMckRxActive = FALSE;
    _gbI2sMckTxActive = FALSE;
    _gbI2sModeRxActive = FALSE;
    _gbI2sModeTxActive = FALSE;
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

    nMask = (REG_PD_ADC1 | REG_PD_INMUX2_MSK | REG_PD_LDO_ADC1 );
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL0b, nMask, (bEnable? 0:((REG_PD_ADC1 | (1<<REG_PD_INMUX2_POS) | REG_PD_LDO_ADC1 ))));

    //1: 3-CH DIFFERENTIAL MODE, 2: 4-CH SINGLE MODE
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL02, REG_EN_SW_MIC2_L_MSK, 1<<REG_EN_SW_MIC2_L_POS);
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL02, REG_EN_SW_MIC2_R_MSK, 1<<REG_EN_SW_MIC2_R_POS);
}

static void _HalAudAtopDac(BOOL bEnable)
{
    U16 nMask;
    nMask = (REG_PD_BIAS_DAC | REG_PD_L0_DAC | REG_PD_LDO_DAC | REG_PD_R0_DAC | REG_PD_REF_DAC | REG_PD_HD | REG_PD_HD_BIAS);

    if(bEnable)
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, 0);
    else
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, nMask);
}

static void _HalAudAtopEnableRef(BOOL bEnable)
{
    U16 nMask;
    nMask = (REG_PD_VI | REG_PD_VREF);
    //HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, (bEnable? 0:nMask));
    HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, nMask, 0);
}

static void _HalAudAtopEnablePreamp(BOOL bEnable)
{
    if(bEnable)
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R, 0);
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL0b, REG_PD_MIC_STG2_L | REG_PD_MIC_STG2_R, 0);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL03, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R, REG_PD_MIC_STG1_L | REG_PD_MIC_STG1_R);
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL0b, REG_PD_MIC_STG2_L | REG_PD_MIC_STG2_R, REG_PD_MIC_STG2_L | REG_PD_MIC_STG2_R);
    }
}

static BOOL _HalAudAtopSwitch(AudAtopPath_e ePath, BOOL bEnable)
{
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
                _HalAudAtopEnablePreamp(TRUE);
                _mSleep(50); //delay for analog initialize completed
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_MICIN]=TRUE;
            }
            else
            {
                _HalAudAtopAdc(FALSE);
                _HalAudAtopEnablePreamp(FALSE);
                _gbAdcActive = bEnable;
                _gbAtopStatus[AUD_ADC_MICIN]=FALSE;
            }
            break;

        case AUD_DAC_LINEOUT:
            if (bEnable)
            {
                _HalAudAtopDac(TRUE);
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

                _HalAudAtopDac(FALSE);
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

        case AUD_DMA_READER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_TRIG, 0); // prevent from triggering levelcount at toggling init step
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_INIT, REG_RD_INIT);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_INIT, 0);
            break;

        case AUD_DMA_WRITER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_TRIG, 0);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_INIT, REG_WR_INIT);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_INIT, 0);
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
        case AUD_DMA_READER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_0;
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
        case AUD_DMA_WRITER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_0;
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
        case AUD_DMA_READER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_0;
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
        case AUD_DMA_WRITER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_0;
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
        case AUD_DMA_READER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_8;
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
        case AUD_DMA_WRITER2:
            eBank=BACH_REG_BANK1;
            nAddr=BACH_DMA2_CTRL_8;
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

        case AUD_DMA_WRITER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_LEVEL_CNT_MASK, REG_WR_LEVEL_CNT_MASK);
            nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_15);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_LEVEL_CNT_MASK, 0);
            break;

        case AUD_DMA_READER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_LEVEL_CNT_MASK, REG_RD_LEVEL_CNT_MASK);
            nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_7);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_LEVEL_CNT_MASK, 0);
            break;

        default:
            ERRMSG("HalAudioDmaGetLevelCnt - ERROR bank default case! [%s ## %d] \n", __func__, __LINE__);
            return 0;
    }

    return (U32)nConfigValue;
}

/* nChSel table
0000: dmic01	0001: dmic23
0100: adc01	    0101: adc23
1000: i2s_rx01	1001: i2s_rx23
1010: i2s_rx45	1011: i2s_rx67
1100: src
*/
static BOOL _HalAudDmaGetMchSelConfigValue(AudMchSel_e eMchSel, U16 *nChSel)
{
    switch(eMchSel)
    {
        case AUD_MCH_SEL_DMIC01:
            *nChSel = 0;    //b'0000
            break;
        case AUD_MCH_SEL_DMIC23:
            *nChSel = 1;    //b'0001
            break;
        case AUD_MCH_SEL_AMIC01:
            *nChSel = 4;    //b'0100
            break;
        case AUD_MCH_SEL_AMIC23:
            *nChSel = 5;    //b'0101
            break;
        case AUD_MCH_SEL_I2S_RX01:
            *nChSel = 8;    //b'1000
            break;
        case AUD_MCH_SEL_I2S_RX23:
            *nChSel = 9;    //b'1001
            break;
        case AUD_MCH_SEL_I2S_RX45:
            *nChSel = 10;   //b'1010
            break;
        case AUD_MCH_SEL_I2S_RX67:
            *nChSel = 11;   //b'1011
            break;
        case AUD_MCH_SEL_SRC:
            *nChSel = 12;   //b'1100
            break;
        case AUD_MCH_SEL_NULL:
            *nChSel = 15;   //b'1111
            break;
        default:
            ERRMSG("Function - %s # %d - eMchSel = %d, error !] \n", __func__, __LINE__, eMchSel);
            return FALSE;
    }

    return TRUE;
}

static U32 _HalAudBckCalculate(AudRate_e eRate, AudBitWidth_e eWidth, U16 Chn)
{
    U32 Value = 0;
    U32 Rate = 0;
    U32 Width = 0;
    U32 Channel = Chn;

    switch(eRate)
    {
        case AUD_RATE_48K:
            Rate = 48;
            break;
        case AUD_RATE_32K:
            Rate = 32;
            break;
        case AUD_RATE_16K:
            Rate = 16;
            break;
        case AUD_RATE_8K:
            Rate = 8;
            break;
        default:
            return FALSE;
    }

    switch(eWidth)
    {
        case AUD_BITWIDTH_16:
            Width = 16;
            break;
        case AUD_BITWIDTH_32:
            Width = 32;
            break;
        default:
            return FALSE;
    }

    //div8, because of BACH_RX_DIV_SEL_MSK and BACH_TX_DIV_SEL_MSK set to 0x3
    Value = ((1<<22)*REF_FREQ / (Rate*Width*Channel/2) / 8)*1000;

    return Value;
}

static BOOL _HalAudI2sSetBck( AudI2s_e eI2s, U32 nNfValue )
{
    U16 BCK_VALUE_LO = 0;
    U16 BCK_VALUE_HI = 0;

    BCK_VALUE_LO = nNfValue & 0xFFFF;
    BCK_VALUE_HI = (nNfValue >> 16) & 0xFFFF;

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            //0x150378[15:14] for master RX BCK div1/div2/div4/div8 selection
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_RX_DIV_SEL_MSK, 3<<BACH_RX_DIV_SEL_POS);
            //
            HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK01, BACH_CODEC_BCK_EN_SYNTH_NF_VALUE_LO, BCK_VALUE_LO);
            HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK02, BACH_CODEC_BCK_EN_SYNTH_NF_VALUE_HI, BCK_VALUE_HI);
            break;

        case AUD_I2S_CODEC_TX:
            //0x150378[11:10] for master TX BCK div1/div2/div4/div8 selection
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_TX_DIV_SEL_MSK, 3<<BACH_TX_DIV_SEL_POS);
            //
            HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK01, BACH_CODEC_BCK_EN_SYNTH_NF_VALUE_LO, BCK_VALUE_LO);
            HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK02, BACH_CODEC_BCK_EN_SYNTH_NF_VALUE_HI, BCK_VALUE_HI);
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

static BOOL _HalAudMckCalculate(AudI2s_e eI2s, AudI2sMck_e eMck)
{
    U16 MCK_VALUE_LO = 0;
    U16 MCK_VALUE_HI = 0;
    U16 nDuty;
    U32 nNf;

    if ( eMck == AUD_I2S_MCK_NULL )
    {
        return TRUE;
    }

    switch(eMck)
    {
        case AUD_I2S_MCK_12_288M:
            nNf = ( 0x1F4000 );
            nDuty = (U16)( (U32)384000 * 50 / 12288 / 100 );
            break;

        case AUD_I2S_MCK_16_384M:
            nNf = ( 0x177000 );
            nDuty = (U16)( (U32)384000 * 50 / 16384 / 100 );
            break;

        case AUD_I2S_MCK_18_432M:
            nNf = ( 0x14D555 );
            nDuty = (U16)( (U32)384000 * 50 / 18432 / 100 );
            break;

        case AUD_I2S_MCK_24_576M:
            nNf = ( 0xFA000 );
            nDuty = (U16)( (U32)384000 * 50 / 24576 / 100 );
            break;

        default:
            return FALSE;
    }

    MCK_VALUE_HI = (U16)( (nNf) >> BACH_MCK_NF_VALUE_HI_OFFSET ) & BACH_MCK_NF_VALUE_HI_MSK;
    HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK02, BACH_MCK_NF_VALUE_HI_MSK, MCK_VALUE_HI);
    MCK_VALUE_LO = (U16)( (nNf) & BACH_MCK_NF_VALUE_LO_MSK );
    HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK01, BACH_MCK_NF_VALUE_LO_MSK, MCK_VALUE_LO);

    // Duty
    HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK02, BACH_MCK_EXPAND_MSK, ( nDuty << BACH_MCK_EXPAND_POS ));

    return TRUE;
}

static BOOL _HalAudI2sSetClkRef(AudI2s_e eI2s, AudI2sClkRef_e eI2sClkRef)
{
    U16 nClkRefSetting = 0;

    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:

            // 0 dmic,1 adc,2 i2srx,3 src
            switch(eI2sClkRef)
            {
                case AUD_I2S_CLK_REF_DMIC:
                    nClkRefSetting = 0;
                    break;
                case AUD_I2S_CLK_REF_ADC:
                    nClkRefSetting = 1;
                    break;
                case AUD_I2S_CLK_REF_I2S_TDM_RX:
                    nClkRefSetting = 2;
                    break;
                case AUD_I2S_CLK_REF_SRC:
                    nClkRefSetting = 3;
                    break;

                default:
                    ERRMSG("Function - %s # %d - eI2sClkRef = %d, error !] \n", __func__, __LINE__, (int)eI2sClkRef);
                    return FALSE;
            }

            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG02, BACH_I2S_TDM_RX_MSK, (nClkRefSetting<<BACH_I2S_TDM_RX_POS)); //0x15039A[1:0]
            break;

        case AUD_I2S_CODEC_TX:
            // No setting ?
            return FALSE;
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

static void _HalAudDmaSetMiuSel(AudDmaChn_e eDmaChannel, U32 nBufAddrOffset)
{
    //reg_dma_test_ctrl7[5:4] is defined as  DMA1_MIU_SEL[1:0]
    //reg_dma_test_ctrl7[7:6] is defined as  DMA2_MIU_SEL[1:0]
    U8 nMiuSel = 0;
    U8 nValue = 0;

    nMiuSel = nBufAddrOffset >= 0x80000000 ? 1 : 0;

    switch(eDmaChannel)
    {
        case AUD_DMA_WRITER1:
        case AUD_DMA_READER1:
            nValue = nMiuSel ? REG_DMA1_MIU_SEL0 : 0;
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA1_MIU_SEL1|REG_DMA1_MIU_SEL0, nValue);
            break;

        case AUD_DMA_WRITER2:
        case AUD_DMA_READER2:
            nValue = nMiuSel ? REG_DMA2_MIU_SEL0 : 0;
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA2_MIU_SEL1|REG_DMA2_MIU_SEL0, nValue);
            break;

        default:
            ERRMSG("_HalAudDmaSetMiuSel - ERROR [%s ## %d] \n", __func__, __LINE__);
            break;
    }

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
#if 0
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

        case AUD_MUX_MMC2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_MUX0_SEL, REG_MMC2_SRC_SEL, (nChoice?REG_MMC2_SRC_SEL:0));
            break;

        case AUD_MUX_DMARD1:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG02, BACH_DMA_RD1_SEL, (nChoice?BACH_DMA_RD1_SEL:0));
            break;

        case AUD_MUX_DMARD2:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG02, BACH_DMA_RD2_SEL, (nChoice?BACH_DMA_RD2_SEL:0));
            break;

        case AUD_MUX_DMAWR1: //combine two mux, now we only have SRC mode(0) & 32bit bypass mode(1)
            HalBachWriteReg(BACH_REG_BANK2, BACH_MUX1_SEL, MUX_ASRC_ADC_SEL, (nChoice?MUX_ASRC_ADC_SEL:0));
            break;

        case AUD_MUX_DMAWR1_MCH:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_NEW_MODE, (nChoice?BACH_DMA_WR_NEW_MODE:0));
            break;

        case AUD_MUX_DMAWR2_MCH:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_NEW_MODE, (nChoice?BACH_DMA_WR_NEW_MODE:0));
            break;

        case AUD_MUX_I2STDM_RX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_RX_EXT_SEL, (nChoice?BACH_RX_EXT_SEL:0));
            break;

        case AUD_MUX_I2S_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, MUX_CODEC_TX_SEL_MSK, nChoice<<MUX_CODEC_TX_SEL_POS);
            break;

        case AUD_MUX_I2S_CFG_00:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_DMA_RD2_PATH_MSK, nChoice<<BACH_DMA_RD2_PATH_POS);
            break;

        case AUD_MUX_SDM_DINL:
            HalBachWriteReg(BACH_REG_BANK1, BACH_MUX4_SEL, REG_SDM_DINL_SEL_MSK, nChoice<<REG_SDM_DINL_SEL_POS);
            break;

        case AUD_MUX_SDM_DINR:
            HalBachWriteReg(BACH_REG_BANK1, BACH_MUX4_SEL, REG_SDM_DINR_SEL_MSK, nChoice<<REG_SDM_DINR_SEL_POS);
            break;

        case AUD_MUX_HD_SDM_DINL:
            HalBachWriteReg(BACH_REG_BANK1, BACH_HD_SDM_CTRL_1, REG_HD_SDM_DINL_SEL_MSK, nChoice<<REG_HD_SDM_DINL_SEL_POS);
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

            //I2S TDM PadMux
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_RX];
            if(bEnable)
            {
                _gbI2sModeRxActive = TRUE;
                nConfigValue |= (nMode<<I2S_TDM_RX_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                _gbI2sModeRxActive = FALSE;
                nConfigValue &= ~(nMode<<I2S_TDM_RX_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }

            //MCK PadMux
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_MCK];
            if(bEnable)
            {
                nConfigValue |= (nMode<<I2S_MCK_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                if ( _gbI2sKeepClk == FALSE )
                {
                    if(_gbI2sModeRxActive == FALSE && _gbI2sModeTxActive == FALSE)
                    {
                        nConfigValue &= ~(nMode<<I2S_MCK_PADMUX_OFFSET);
                        HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
                    }
                }
            }

            break;

        case AUD_PAD_MUX_I2S_TX:

            //I2S TDM PadMux
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_TX];
            if(bEnable)
            {
                _gbI2sModeTxActive = TRUE;
                nConfigValue |= (nMode<<I2S_TDM_TX_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                _gbI2sModeTxActive = FALSE;
                nConfigValue &= ~(nMode<<I2S_TDM_TX_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }

            //MCK PadMux
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_MCK];
            if(bEnable)
            {
                nConfigValue |= (nMode<<I2S_MCK_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                if ( _gbI2sKeepClk == FALSE )
                {
                    if(_gbI2sModeRxActive == FALSE && _gbI2sModeTxActive == FALSE)
                    {
                        nConfigValue &= ~(nMode<<I2S_MCK_PADMUX_OFFSET);
                        HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
                    }
                }
            }
            break;

        case AUD_PAD_MUX_I2S_RXTX:
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_RXTX];
            if(bEnable)
            {
                _gbI2sModeRxActive = TRUE;
                _gbI2sModeTxActive = TRUE;
                nConfigValue |= (nMode<<I2S_MODE_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                _gbI2sModeRxActive = FALSE;
                _gbI2sModeTxActive = FALSE;
                nConfigValue &= ~(nMode<<I2S_MODE_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }

            //MCK PadMux
            nConfigValue = HalBachReadReg2Byte(0x103cc4);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_I2S_MCK];
            if(bEnable)
            {
                nConfigValue |= (nMode<<I2S_MCK_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
            }
            else
            {
                if ( _gbI2sKeepClk == FALSE )
                {
                    if(_gbI2sModeRxActive == FALSE && _gbI2sModeTxActive == FALSE)
                    {
                        nConfigValue &= ~(nMode<<I2S_MCK_PADMUX_OFFSET);
                        HalBachWriteReg2Byte(0x103cc4, 0xffff, nConfigValue);
                    }
                }
            }

            break;

        case AUD_PAD_MUX_DMIC:
            nConfigValue = HalBachReadReg2Byte(0x103cc0);
            nMode = _gaPadMuxMode[AUD_PAD_MUX_DMIC];
            if(bEnable)
            {
                nConfigValue |= (nMode<<DIGMIC_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc0, 0xffff, nConfigValue);
            }
            else
            {
                nConfigValue &= ~(nMode<<DIGMIC_PADMUX_OFFSET);
                HalBachWriteReg2Byte(0x103cc0, 0xffff, nConfigValue);
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
    if (ePath < 0 || ePath >= AUD_ATOP_NUM)
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
    if(ePath < 0 || ePath >= AUD_ATOP_NUM)
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
    U16 nRegMsk, nPos, nPos_ext, nGain;
    U8 nOffset;

    switch(eAdcSel)
    {
        case AUD_ADC_SEL_MICIN:
            if (ch == 0)
            {
                nOffset = BACH_ANALOG_CTRL08;
                nRegMsk = REG_SEL_MICGAIN_STG1_L_ALL_MSK;
                nPos = REG_SEL_MICGAIN_STG1_L_POS;
                nPos_ext = REG_SEL_MICGAIN_STG1_L_EXT_POS;
            }
            else if (ch == 1)
            {
                nOffset = BACH_ANALOG_CTRL08;
                nRegMsk = REG_SEL_MICGAIN_STG1_R_ALL_MSK;
                nPos = REG_SEL_MICGAIN_STG1_R_POS;
                nPos_ext = REG_SEL_MICGAIN_STG1_R_EXT_POS;
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
                return FALSE;
            }
            break;

        case AUD_ADC_SEL_MICIN2:
            if (ch == 0)
            {
                nOffset = BACH_ANALOG_CTRL0d;
                nRegMsk = REG_SEL_MICGAIN_STG2_L_ALL_MSK;
                nPos = REG_SEL_MICGAIN_STG2_L_POS;
                nPos_ext = REG_SEL_MICGAIN_STG2_L_EXT_POS;
            }
            else if (ch == 1)
            {
                nOffset = BACH_ANALOG_CTRL0d;
                nRegMsk = REG_SEL_MICGAIN_STG2_R_ALL_MSK;
                nPos = REG_SEL_MICGAIN_STG2_R_POS;
                nPos_ext = REG_SEL_MICGAIN_STG2_R_EXT_POS;
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
                return FALSE;
            }
            break;
        default:
            ERRMSG("Function - %s # %d - eAdcPath = %d, error !] \n", __func__, __LINE__, (int)eAdcSel);
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
        HalBachWriteReg(BACH_REG_BANK4, nOffset, nRegMsk, (nGain << nPos) | (1 << nPos_ext));
    }
    else
    {
        nGain = nSel - 2;
        HalBachWriteReg(BACH_REG_BANK4, nOffset, nRegMsk, (nGain << nPos));
    }

    return TRUE;
}

BOOL HalAudAtopSetAdcGain(AudAdcSel_e eAdcSel, U16 nSel, S8 ch)
{
	U16 nConfigValue, nRegMsk, nPos, nGain;
	U8 nOffset;

    switch(eAdcSel)
    {
        case AUD_ADC_SEL_MICIN:
            if (ch == 0)
            {
                nOffset = BACH_ANALOG_CTRL08;
                nConfigValue = 0x0001;
                nRegMsk = REG_SEL_GAIN_INMUX0_MSK;
                nPos = REG_SEL_GAIN_INMUX0_POS;
            }
            else if (ch == 1)
            {
                nOffset = BACH_ANALOG_CTRL08;
                nConfigValue = 0x0002;
                nRegMsk = REG_SEL_GAIN_INMUX1_MSK;
                nPos = REG_SEL_GAIN_INMUX1_POS;
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
                return FALSE;
            }
            break;

        case AUD_ADC_SEL_MICIN2:
            if (ch == 0)
            {
                nOffset = BACH_ANALOG_CTRL0d;
                nConfigValue = 0x0001;
                nRegMsk = REG_SEL_GAIN_INMUX2_MSK;
                nPos = REG_SEL_GAIN_INMUX2_POS;
            }
            else if (ch == 1)
            {
                nOffset = BACH_ANALOG_CTRL0d;
                nConfigValue = 0x0002;
                nRegMsk = REG_SEL_GAIN_INMUX3_MSK;
                nPos = REG_SEL_GAIN_INMUX3_POS;
            }
            else
            {
                ERRMSG("ERROR, Function:%s, ch=%d\n", __func__, ch);
                return FALSE;
            }
            break;
        default:
            ERRMSG("Function - %s # %d - eAdcPath = %d, error !] \n", __func__, __LINE__, (int)eAdcSel);
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
        HalBachWriteReg(BACH_REG_BANK4, nOffset, (REG_SEL_MICGAIN_INMUX_MSK & nConfigValue), nConfigValue<<REG_SEL_MICGAIN_INMUX_POS);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK4, nOffset, (REG_SEL_MICGAIN_INMUX_MSK & nConfigValue), 0<<REG_SEL_MICGAIN_INMUX_POS);
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
        case AUD_ADC_SEL_MICIN2:
            HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_CTRL0c, REG_SEL_CH_INMUX2_MSK | REG_SEL_CH_INMUX3_MSK, 0x7<<REG_SEL_CH_INMUX2_POS | 0x7<<REG_SEL_CH_INMUX3_POS);
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
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ADC_LR_SWAP, REG_ADC_LR_SWAP);
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ADC2_LR_SWAP, REG_ADC2_LR_SWAP);
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ADC_LR_SWAP, ~REG_ADC_LR_SWAP);
        HalBachWriteReg(BACH_REG_BANK4, BACH_ANALOG_STAT03, REG_ADC2_LR_SWAP, ~REG_ADC2_LR_SWAP);
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

BOOL HalAudKeepAdcPowerOn(U8 u8AdcPowerOn)
{
    return TRUE;
}

BOOL HalAudKeepDacPowerOn(U8 u8DacPowerOn)
{
    return TRUE;
}

BOOL HalAudI2sSaveTxTdmActiveSlot(U8 nActiveSlot)
{
    _gnI2sTxTdmActiveSlot = nActiveSlot;

    return TRUE;
}

BOOL HalAudI2sSetTdmDetails(AudI2s_e eI2s)
{
    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_WS_FMT, (_gnI2sRxTdmWsPgm?BACH_I2S_RX_WS_FMT:0));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_WS_INV, (_gnI2sRxTdmWsInv?BACH_I2S_RX_WS_INV:0));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG01, BACH_I2S_RX_WS_WDTH_MSK, (_gnI2sRxTdmWsWidth<<BACH_I2S_RX_WS_WDTH_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG02, BACH_I2S_RX_CH_SWAP_MSK, (_gnI2sRxTdmChSwap<<BACH_I2S_RX_CH_SWAP_POS));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_WS_FMT, (_gnI2sTxTdmWsPgm?BACH_I2S_TX_WS_FMT:0));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_WS_INV, (_gnI2sTxTdmWsInv?BACH_I2S_TX_WS_INV:0));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG06, BACH_I2S_TX_WS_WDTH_MSK, (_gnI2sTxTdmWsWidth<<BACH_I2S_TX_WS_WDTH_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG06, BACH_I2S_TX_CH_SWAP_MSK, (_gnI2sTxTdmChSwap<<BACH_I2S_TX_CH_SWAP_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG06, BACH_I2S_TX_ACT_SLOT_MSK, (_gnI2sTxTdmActiveSlot<<BACH_I2S_TX_ACT_SLOT_POS));
            break;

        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }

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
    _gaI2sCfg[eI2s].eRate = eRate;

    return _HalAudI2sSetBck( eI2s, _HalAudBckCalculate( _gaI2sCfg[eI2s].eRate, _gaI2sCfg[eI2s].eWidth, _gaI2sCfg[eI2s].nChannelNum ) );
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
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_TDM_MODE, (nTdmMode?BACH_I2S_RX_TDM_MODE:0));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_TDM_MODE, (nTdmMode?BACH_I2S_TX_TDM_MODE:0));
            break;

        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
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
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_MS_MODE, (nMsMode?BACH_I2S_RX_MS_MODE:0));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_MS_MODE, (nMsMode?BACH_I2S_TX_MS_MODE:0));
            break;

        default:
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
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_FMT, (nSel?BACH_I2S_RX_FMT:0));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_FMT, (nSel?BACH_I2S_TX_FMT:0));
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
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_ENC_WIDTH, (nSel?BACH_I2S_RX_ENC_WIDTH:0));
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_ENC_WIDTH, (nSel?BACH_I2S_TX_ENC_WIDTH:0));
            break;

        default:
            return FALSE;
    }
    _gaI2sCfg[eI2s].eWidth = eWidth;
    return TRUE;
}

BOOL HalAudI2sSetChannel(AudI2s_e eI2s, U16 nChannel)
{
    if(nChannel!=2 && nChannel!=4 && nChannel!=8)
        return FALSE;

    if(nChannel == 2)
    {
        _gaI2sCfg[eI2s].nChannelNum = nChannel;
        return TRUE;
    }
    else
    {
        switch(eI2s)
        {
            case AUD_I2S_CODEC_RX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG00, BACH_I2S_RX_CHLEN, (nChannel==8?BACH_I2S_RX_CHLEN:0));
                break;

            case AUD_I2S_CODEC_TX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_CHLEN, (nChannel==8?BACH_I2S_TX_CHLEN:0));
                break;

            default:
                ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
                return FALSE;
        }

        _gaI2sCfg[eI2s].nChannelNum = nChannel;
        return TRUE;
    }
}

BOOL HalAudI2sSetWireMode(AudI2s_e eI2s, AudWireMode_e eWireMode)
{
    if(_gaPadMuxMode[AUD_PAD_MUX_I2S_RXTX] == TRUE)
    {
        if(eWireMode == AUD_I2S_WIRE_6)
        {
            ERRMSG("Function - %s Only Support 4 WireMode - WireMode = %d, error !] \n", __func__, (int)eWireMode);
            return FALSE;
        }
    }

    if(eI2s == AUD_I2S_CODEC_TX)
    {
        switch(eWireMode)
        {
            case AUD_I2S_WIRE_4:
                HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_4WIRE_MODE, BACH_I2S_TX_4WIRE_MODE);
                break;

            case AUD_I2S_WIRE_6:
                HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG05, BACH_I2S_TX_4WIRE_MODE, 0);
                break;

            default:
                ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
                return FALSE;
        }

        _gaI2sCfg[eI2s].eWireMode = eWireMode;
    }
    else
    {
        _gaI2sCfg[eI2s].eWireMode = AUD_I2S_WIRE_NULL;
    }

    return TRUE;
}

BOOL HalAudI2sSetTdmSlotConfig(AudI2s_e eI2s, U16 nSlotMsk, AudTdmChnMap_e eMap)
{
    switch(eI2s)
    {
        case AUD_I2S_CODEC_RX:
            break;

        case AUD_I2S_CODEC_TX:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG06, BACH_I2S_TX_ACT_SLOT_MSK, (nSlotMsk<<BACH_I2S_TX_ACT_SLOT_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_TDM_CFG06, BACH_I2S_TX_CH_SWAP_MSK, (eMap<<BACH_I2S_TX_CH_SWAP_POS));
            break;

        default:
            ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
            return FALSE;
    }
    return TRUE;
}

BOOL HalAudI2sEnableMck(AudI2s_e eI2s, BOOL bEnable)
{
    U16 EN_TIME_GEN = BACH_ENABLE_CLK_NF_SYNTH_REF|BACH_CODEC_BCK_EN_TIME_GEN;

    if ( ( bEnable == TRUE ) && ( _gaI2sCfg[eI2s].eMck == AUD_I2S_MCK_NULL ) )
    {
        return TRUE;
    }

    if(bEnable)
    {
        switch(eI2s)
        {
            case AUD_I2S_CODEC_RX:
                _gbI2sMckRxActive = TRUE;
                break;
            case AUD_I2S_CODEC_TX:
                _gbI2sMckTxActive = TRUE;
                break;
            default:
                ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
                return FALSE;
        }

        HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
        HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG);
        HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
    }
    else
    {
        switch(eI2s)
        {
            case AUD_I2S_CODEC_RX:
                _gbI2sMckRxActive = FALSE;
                break;
            case AUD_I2S_CODEC_TX:
                _gbI2sMckTxActive = FALSE;
                break;
            default:
                ERRMSG("Function - %s # %d - I2S ID = %d, error !] \n", __func__, __LINE__, (int)eI2s);
                return FALSE;
        }

        if(_gbI2sMckTxActive == FALSE && _gbI2sMckRxActive == FALSE)
        {
            HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_MCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, 0);
        }
    }

    return TRUE;
}

BOOL HalAudI2sSetMck(AudI2s_e eI2s, AudI2sMck_e eMck)
{
    if ( eMck < AUD_I2S_MCK_NULL || eMck > AUD_I2S_MCK_48M)
    {
        return FALSE;
    }

    switch(eMck)
    {
        case AUD_I2S_MCK_48M:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_MCK_SEL_MSK, 0<<BACH_MCK_SEL_POS);
            break;
        case AUD_I2S_MCK_24M:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_MCK_SEL_MSK, 1<<BACH_MCK_SEL_POS);
            break;
        default:
            HalBachWriteReg(BACH_REG_BANK2, BACH_I2S_CFG00, BACH_MCK_SEL_MSK, 2<<BACH_MCK_SEL_POS);
            _HalAudMckCalculate(eI2s, eMck);
            break;
    }

    _gaI2sCfg[eI2s].eMck = eMck;
    return TRUE;
}

//for I2S master
BOOL HalAudI2sEnable(AudI2s_e eI2s, BOOL bEnable)
{
    U16 EN_TIME_GEN = BACH_ENABLE_CLK_NF_SYNTH_REF|BACH_CODEC_BCK_EN_TIME_GEN;

    if(bEnable)
    {
        switch(eI2s)
        {
            case AUD_I2S_CODEC_RX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG);
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
                break;

            case AUD_I2S_CODEC_TX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG);
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, EN_TIME_GEN);
                break;

            default:
                return FALSE;
        }
    }
    else
    {
        switch(eI2s)
        {
            case AUD_I2S_CODEC_RX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_RX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, 0);
                break;

            case AUD_I2S_CODEC_TX:
                HalBachWriteReg(BACH_REG_BANK2, BACH_NF_SYNTH_TX_BCK00, EN_TIME_GEN|BACH_CODEC_BCK_EN_SYNTH_TRIG, 0);
                break;

            default:
                return FALSE;
        }
    }

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

            // SRC:
            // 48K: -1.5dB
            // Others: 0dB
            if (eRate == AUD_RATE_48K)
            {
                //SRC
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 3 ,0);
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 3 ,1);
            }
            else
            {
                //SRC
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,0);
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,1);
            }
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

        case AUD_DMA_WRITER2:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CIC_3_SEL_AU2_MSK, 0<<REG_CIC_3_SEL_AU2_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CODEC_SEL_AU2_MSK, 0<<REG_CODEC_SEL_AU2_POS);
                    break;

                case AUD_RATE_16K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CIC_3_SEL_AU2_MSK, 1<<REG_CIC_3_SEL_AU2_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CODEC_SEL_AU2_MSK, 1<<REG_CODEC_SEL_AU2_POS);
                    break;

                case AUD_RATE_32K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CIC_3_SEL_AU2_MSK, 2<<REG_CIC_3_SEL_AU2_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CODEC_SEL_AU2_MSK, 2<<REG_CODEC_SEL_AU2_POS);
                    break;

                case AUD_RATE_48K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CIC_3_SEL_AU2_MSK, 3<<REG_CIC_3_SEL_AU2_POS);
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL_AU2, REG_CODEC_SEL_AU2_MSK, 3<<REG_CODEC_SEL_AU2_POS);
                    break;

                default:
                    ERRMSG("Function - %s # %d - AUD_DMA_WRITER2 eRate = %d, error !] \n", __func__, __LINE__, (int)eRate);
                    return FALSE;
            }

            // SRC:
            // 48K: -1.5dB
            // Others: 0dB
            if (eRate == AUD_RATE_48K)
            {
                //SRC
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 3 ,0);
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 3 ,1);
            }
            else
            {
                //SRC
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,0);
                HalAudDpgaSetGainOffset(AUD_DPGA_DEC1, 0 ,1);
            }
            break;

        case AUD_DMA_READER2:
            switch(eRate)
            {
                case AUD_RATE_8K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 0<<REG_SRC2_SEL_POS);
                    //HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 0<<REG_CODEC_SEL_POS);
                    break;

                case AUD_RATE_11K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 1<<REG_SRC2_SEL_POS);
                    break;

                case AUD_RATE_12K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 2<<REG_SRC2_SEL_POS);
                    break;

                case AUD_RATE_16K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 3<<REG_SRC2_SEL_POS);
                    //HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 1<<REG_CODEC_SEL_POS);
                    break;

                case AUD_RATE_22K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 4<<REG_SRC2_SEL_POS);
                    break;

                case AUD_RATE_24K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 5<<REG_SRC2_SEL_POS);
                    break;

                case AUD_RATE_32K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 6<<REG_SRC2_SEL_POS);
                    //HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 2<<REG_CODEC_SEL_POS);
                    break;

                case AUD_RATE_44K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 7<<REG_SRC2_SEL_POS);
                    break;

                case AUD_RATE_48K:
                    HalBachWriteReg(BACH_REG_BANK1, BACH_SR0_SEL, REG_SRC2_SEL_MSK, 8<<REG_SRC2_SEL_POS);
                    //HalBachWriteReg(BACH_REG_BANK2, BACH_AU_SYS_CTRL1, REG_CODEC_SEL_MSK, 3<<REG_CODEC_SEL_POS);
                    break;

                default:
                    ERRMSG("Function - %s # %d - AUD_DMA_READER2 eRate = %d, error !] \n", __func__, __LINE__, (int)eRate);
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

        case AUD_DMA_READER2:
            if(bEnable)
            {
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_ENABLE, REG_ENABLE); //reader prefetch enable, it should be enabled before reader enable
                _uDelay(10);
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_ENABLE, REG_RD_ENABLE);
            }
            else
            {
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_ENABLE, 0);
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_ENABLE, 0); //reader prefetch enable, it has to be disabled before dma init
            }
            break;

        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA1_CTRL_9, REG_WR_ENABLE, (bEnable ? REG_WR_ENABLE : 0));
            break;

        case AUD_DMA_WRITER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_ENABLE, (bEnable ? REG_WR_ENABLE : 0));
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
    BOOL ret;

    if (eDmaChannel < AUD_DMA_READER1)
        ret = _HalAudDmaWrIntEnable(eDmaChannel, bDatatrigger, bDataboundary);
    else
        ret = _HalAudDmaRdIntEnable(eDmaChannel, bDatatrigger, bDataboundary);

    return ret;
}

BOOL HalAudDmaGetInt(AudDmaChn_e eDmaChannel, BOOL *bDatatrigger, BOOL *bDataboundary)
{
    BOOL ret;

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

        case AUD_DMA_WRITER2:
            //DMA writer2 full flag clear / DMA writer2 local buffer full flag clear
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_WR_FULL_FLAG_CLR, REG_WR_FULL_FLAG_CLR);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_WR_FULL_FLAG_CLR, 0);
            break;

        case AUD_DMA_READER2:
            //DMA reader2 empty flag clear / DMA reader2 local buffer empty flag clear
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_RD_EMPTY_FLAG_CLR, REG_RD_EMPTY_FLAG_CLR);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_0, REG_RD_EMPTY_FLAG_CLR, 0 );
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

            case AUD_DMA_WRITER2:
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_12, 0xFFFF, nConfigValue);
                nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9);
                if(nConfigValue & REG_WR_TRIG)
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_TRIG, 0);
                }
                else
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_TRIG, REG_WR_TRIG);
                }
                break;

            case AUD_DMA_READER2:
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_4, 0xFFFF, nConfigValue);
                nConfigValue = HalBachReadReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1);
                if(nConfigValue & REG_RD_TRIG)
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_TRIG, 0);
                }
                else
                {
                    HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_TRIG, REG_RD_TRIG);
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

    _HalAudDmaSetMiuSel(eDmaChannel, nBufAddrOffset);

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

        case AUD_DMA_WRITER2:
            nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_WR_BASE_ADDR_LO_MSK);
            nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_WR_BASE_ADDR_HI_OFFSET) & REG_WR_BASE_ADDR_HI_MSK);
            nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_WR_BUFF_SIZE_MSK);

            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_9, REG_WR_BASE_ADDR_LO_MSK, nMiuAddrLo);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_10, REG_WR_BASE_ADDR_HI_MSK, nMiuAddrHi);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_11, 0xFFFF, nMiuSize);
            break;

        case AUD_DMA_READER2:
            nMiuAddrLo = (U16)((nBufAddrOffset / MIU_WORD_BYTE_SIZE) & REG_RD_BASE_ADDR_LO_MSK);
            nMiuAddrHi = (U16)(((nBufAddrOffset / MIU_WORD_BYTE_SIZE) >> REG_RD_BASE_ADDR_HI_OFFSET) & REG_RD_BASE_ADDR_HI_MSK);
            nMiuSize = (U16)((nBufSize / MIU_WORD_BYTE_SIZE) & REG_RD_BUFF_SIZE_MSK);

            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_1, REG_RD_BASE_ADDR_LO_MSK, nMiuAddrLo);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_2, REG_RD_BASE_ADDR_HI_MSK, nMiuAddrHi);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_3, 0xFFFF, nMiuSize);
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

        case AUD_DMA_WRITER2:
            nMiuOverrunTh = (U16)((nOverrunTh / MIU_WORD_BYTE_SIZE) & REG_WR_OVERRUN_TH_MSK);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_13, 0xFFFF, nMiuOverrunTh);
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

        case AUD_DMA_READER2:
            nMiuUnderrunTh = (U16)((nUnderrunTh / MIU_WORD_BYTE_SIZE) & REG_RD_UNDERRUN_TH_MSK);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA2_CTRL_6, 0xFFFF, nMiuUnderrunTh);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            break;
    }

    return TRUE;
}

BOOL HalAudDmaGetFlags(AudDmaChn_e eDmaChannel, BOOL *pbDatatrigger, BOOL *pbDataboundary, BOOL *pbLocalData)
{
    BOOL ret;

    if (eDmaChannel < AUD_DMA_READER1)
        ret = _HalAudDmaWrGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);
    else
        ret = _HalAudDmaRdGetFlags(eDmaChannel, pbDatatrigger, pbDataboundary, pbLocalData);

    return ret;
}

BOOL HalAudDmaWrConfigMch(AudMchSet_e eMchSet, U16 nChNum, AudMchClkRef_e eMchClkRef, const int *mch_sel)
{
    U16 nChNumSetting = 0, nClkRefSetting = 0, nChSel01 = 0, nChSel23 = 0, nChSel45 = 0, nChSel67 = 0;
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
        case AUD_MCH_CLK_REF_DMIC:
            nClkRefSetting = 0;
            break;
        case AUD_MCH_CLK_REF_ADC:
            nClkRefSetting = 1;
            break;
        case AUD_MCH_CLK_REF_I2S_TDM_RX:
            nClkRefSetting = 2;
            break;
        case AUD_MCH_CLK_REF_SRC:
            nClkRefSetting = 3;
            break;
        default: // AUD_MCH_SEL_NULL
            ERRMSG(" Set Mch Clock Ref - ERROR! [%s ## %d] eMchClkRef = %d\n", __func__, __LINE__, eMchClkRef);
            return FALSE;
    }

    //
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_01], &nChSel01);
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_23], &nChSel23);
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_45], &nChSel45);
    bRet &= _HalAudDmaGetMchSelConfigValue((AudMchSel_e)mch_sel[AUD_MCH_CH_BIND_67], &nChSel67);

    if ( bRet != TRUE )
    {
        return FALSE;
    }

    switch(eMchSet)
    {
        case AUD_MCH_SET_W1:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG03, BACH_DMA_WR1_CH67_SEL_MSK,  (nChSel67<<BACH_DMA_WR1_CH67_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG03, BACH_DMA_WR1_CH45_SEL_MSK,  (nChSel45<<BACH_DMA_WR1_CH45_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH23_SEL_MSK,   (nChSel23<<BACH_DMA_WR_CH23_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH01_SEL_MSK,   (nChSel01<<BACH_DMA_WR_CH01_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_VALID_SEL_MSK,  (nClkRefSetting<<BACH_DMA_WR_VALID_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG00, BACH_DMA_WR_CH_MODE_MSK,    (nChNumSetting<<BACH_DMA_WR_CH_MODE_POS));
            break;

        case AUD_MCH_SET_W2:
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG03, BACH_DMA_WR2_CH67_SEL_MSK,  (nChSel67<<BACH_DMA_WR2_CH67_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG03, BACH_DMA_WR2_CH45_SEL_MSK,  (nChSel45<<BACH_DMA_WR2_CH45_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_CH23_SEL_MSK,   (nChSel23<<BACH_DMA_WR_CH23_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_CH01_SEL_MSK,   (nChSel01<<BACH_DMA_WR_CH01_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_VALID_SEL_MSK,  (nClkRefSetting<<BACH_DMA_WR_VALID_SEL_POS));
            HalBachWriteReg(BACH_REG_BANK2, BACH_DMA_SRC_CFG01, BACH_DMA_WR_CH_MODE_MSK,    (nChNumSetting<<BACH_DMA_WR_CH_MODE_POS));
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

        case AUD_DMA_WRITER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA2_WR_MONO, (bEnable?REG_DMA2_WR_MONO:0));
            break;

        case AUD_DMA_READER2:
            if(bEnable)
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA2_RD_MONO | REG_DMA2_RD_MONO_COPY, REG_DMA2_RD_MONO | REG_DMA2_RD_MONO_COPY);
            else
                HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL7, REG_DMA2_RD_MONO | REG_DMA2_RD_MONO_COPY, 0);
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
    BachRegBank_e nBank;

    switch(eDpga)
    {
        case AUD_DPGA_MMC1:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC1_DPGA_CFG1;
            break;

        case AUD_DPGA_ADC:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_ADC_DPGA_CFG1;
            break;

        case AUD_DPGA_AEC1:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_AEC1_DPGA_CFG1;
            break;

        case AUD_DPGA_DEC1:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC1_DPGA_CFG1;
            break;

        case AUD_DPGA_MMC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC2_DPGA_CFG1;
            break;

        case AUD_DPGA_DEC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC2_DPGA_CFG1;
            break;

        case AUD_DPGA_TDM:
            nBank = BACH_REG_BANK2;
            nAddr = BACH_I2S_TDM_DPGA_CFG1;
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

    HalBachWriteReg(nBank, nAddr, (DPGA_EN | MUTE_2_ZERO | FADING_EN), nConfigValue);
    //Step Size for fading function, 4: 16 sample, 5: 32 sample, 6: 64 sample
    HalBachWriteReg(nBank, nAddr, STEP_MSK, 6<<STEP_POS);
}

BOOL HalAudDpgaSetGain(AudDpga_e eDpga, S8 gain, S8 ch)
{
    U8 nAddr;
    U8 nGainIdx;
    U16 nRegMsk, nValue;
    BachRegBank_e nBank;

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_ADC_DPGA_CFG2;
            nRegMsk = (REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_AEC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC1_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_MMC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC2_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_DEC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC2_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_TDM:
            nBank = BACH_REG_BANK2;
            nAddr = BACH_I2S_TDM_DPGA_CFG2;
            nRegMsk = (REG_GAIN_R_MSK | REG_GAIN_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

    HalBachWriteReg(nBank, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDpgaSetGainOffset(AudDpga_e eDpga, S8 gainRegValue, S8 ch)
{
    U8 nAddr;
    U8 nGainIdx;
    U16 nRegMsk, nValue;
    S8 nGain;
    BachRegBank_e nBank;

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_ADC_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_AEC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC1_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_MMC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC2_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_DEC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMCDEC2_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

        case AUD_DPGA_TDM:
            nBank = BACH_REG_BANK2;
            nAddr = BACH_I2S_TDM_DPGA_CFG1;
            nRegMsk = (REG_OFFSET_R_MSK | REG_OFFSET_L_MSK);
            nValue = HalBachReadReg(nBank, nAddr);

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

    HalBachWriteReg(nBank, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDpgaSetGainFading(AudDpga_e eDpga, U8 nFading, S8 ch)
{
    U8 nAddr;
    U16 nRegMsk, nValue;
    U8 nFadingEn = 1, nFadingValue = 0;
    BachRegBank_e nBank;

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
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC1_DPGA_CFG1;
            nRegMsk = (STEP_MSK | FADING_EN);
            nValue = HalBachReadReg(nBank, nAddr);

            if ((ch == 0) || (ch == 1))
            {
                nValue = nValue & (~nRegMsk);
                nValue = nValue | (nFadingValue<<STEP_POS) | (nFadingEn<<1);
            }
            else
            {
                ERRMSG("Function - %s # %d - channel=%d\n", __func__, __LINE__, ch);
            }

            break;

        case AUD_DPGA_MMC2:
            nBank = BACH_REG_BANK1;
            nAddr = BACH_MMC2_DPGA_CFG1;
            nRegMsk = (STEP_MSK | FADING_EN);
            nValue = HalBachReadReg(nBank, nAddr);

            if ((ch == 0) || (ch == 1))
            {
                nValue = nValue & (~nRegMsk);
                nValue = nValue | (nFadingValue<<STEP_POS) | (nFadingEn<<1);
            }
            else
            {
                ERRMSG("Function - %s # %d - channel=%d\n", __func__, __LINE__, ch);
            }

            break;

        case AUD_DPGA_TDM:
            nBank = BACH_REG_BANK2;
            nAddr = BACH_I2S_TDM_DPGA_CFG1;
            nRegMsk = (STEP_MSK | FADING_EN);
            nValue = HalBachReadReg(nBank, nAddr);

            if ((ch == 0) || (ch == 1))
            {
                nValue = nValue & (~nRegMsk);
                nValue = nValue | (nFadingValue<<STEP_POS) | (nFadingEn<<1);
            }
            else
            {
                ERRMSG("Function - %s # %d - channel=%d\n", __func__, __LINE__, ch);
            }

            break;

        case AUD_DPGA_ADC:
            ERRMSG("Function - %s # %d - Not Support AI Device Fading\n", __func__, __LINE__);
            break;
        case AUD_DPGA_AEC1:
            ERRMSG("Function - %s # %d - Not Support AI Device Fading\n", __func__, __LINE__);
            break;
        case AUD_DPGA_DEC1:
            ERRMSG("Function - %s # %d - Not Support AI Device Fading\n", __func__, __LINE__);
            break;
        case AUD_DPGA_DEC2:
            ERRMSG("Function - %s # %d - Not Support AI Device Fading\n", __func__, __LINE__);
            break;

        default:
            ERRMSG("Function - %s # %d - eDpga = %d, error !] \n", __func__, __LINE__, eDpga);
            return FALSE;
    }

    HalBachWriteReg(nBank, nAddr, nRegMsk, nValue);

    return TRUE;
}

BOOL HalAudDigMicEnable(BOOL bEn)
{
    //record src: DMIC
    HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL02, REG_SEL_RECORD_SRC, 0);

    return TRUE;
}

BOOL HalAudDigMicSetChannel(U16 nCh)
{
    U16 nConfigValue;

    if(nCh==1)
    {
        nConfigValue = 0;
    }
    else if(nCh==2)
    {
        nConfigValue = 1;
    }
    else if(nCh==4)
    {
        nConfigValue = 2;
    }
    else if(nCh==8)
    {
        nConfigValue = 3;
    }
    else
    {
        ERRMSG("Function - %s # %d - nCh = %d, error !] \n", __func__, __LINE__, nCh);
        return FALSE;
    }

    //DMIC channel
    HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL01, REG_CHN_MODE_DMIC_MSK, (nConfigValue<<REG_CHN_MODE_DMIC_POS));

    return TRUE;
}

BOOL HalAudDigMicSetRate(AudRate_e eRate)
{
    int i = 0;

    _geDmicSmpRat = eRate;

    // for gain compensation
    for(i=0; i<4; i++)
    {
        HalAudDigMicSetGain(_gnDmicGain[i], i);
    }

    switch(eRate)
    {
        case AUD_RATE_8K:
            HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL04, REG_FS_SEL_MSK, 7<<REG_FS_SEL_POS);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL01, REG_DEC_FILTER, REG_DEC_FILTER); //1: 3stage, 0: 5stage
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL02, REG_DEC_FILTER, REG_DEC_FILTER);
            break;

        case AUD_RATE_16K:
            HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL04, REG_FS_SEL_MSK, 12<<REG_FS_SEL_POS);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL01, REG_DEC_FILTER, REG_DEC_FILTER);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL02, REG_DEC_FILTER, REG_DEC_FILTER);
            break;

        case AUD_RATE_32K:
            HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL04, REG_FS_SEL_MSK, 15<<REG_FS_SEL_POS);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL01, REG_DEC_FILTER, 0);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL02, REG_DEC_FILTER, 0);
            break;

        case AUD_RATE_48K:
            HalBachWriteReg(BACH_REG_BANK3, BACH_VREC_CTRL04, REG_FS_SEL_MSK, 16<<REG_FS_SEL_POS);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL01, REG_DEC_FILTER, 0);
            HalBachWriteReg(BACH_REG_BANK3, BACH_CIC_CTRL02, REG_DEC_FILTER, 0);
            break;

        default:
            ERRMSG("Function - %s # %d - DigMic eRate = %d, error !] \n", __func__, __LINE__, (int)eRate);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAudDigMicSetGain(S8 s8Gain, S8 ch)
{
    U16 nGain, nAddr, nRegMsk, nPos;

    if(s8Gain>4)
    {
        ERRMSG("Function - %s # %d - s8Gain = %d, error !] \n", __func__, __LINE__, s8Gain);
        return FALSE;
    }
    else
    {
        _gnDmicGain[ch] = s8Gain;
    }

    if(_geDmicSmpRat == AUD_RATE_48K || _geDmicSmpRat == AUD_RATE_32K)
    {
        nGain = (U16)s8Gain + 1;
    }
    else
    {
        nGain = (U16)s8Gain;
    }

    if(ch == 0)
    {
        nAddr = BACH_CIC_CTRL01;
        nRegMsk = REG_CIC_GAIN_L_MSK;
        nPos = REG_CIC_GAIN_L_POS;
    }
    else if(ch == 1)
    {
        nAddr = BACH_CIC_CTRL01;
        nRegMsk = REG_CIC_GAIN_R_MSK;
        nPos = REG_CIC_GAIN_R_POS;
    }
    else if(ch == 2)
    {
        nAddr = BACH_CIC_CTRL02;
        nRegMsk = REG_CIC_GAIN_L_MSK;
        nPos = REG_CIC_GAIN_L_POS;
    }
    else if(ch == 3)
    {
        nAddr = BACH_CIC_CTRL02;
        nRegMsk = REG_CIC_GAIN_R_MSK;
        nPos = REG_CIC_GAIN_R_POS;
    }
    else
    {
        ERRMSG("Function - %s # %d - ch = %d, error !] \n", __func__, __LINE__, ch);
        return FALSE;
    }

    HalBachWriteReg(BACH_REG_BANK3, nAddr, nRegMsk, nGain << nPos);

    return TRUE;
}

BOOL HalAudCfgI2sIfPrivateCb( void *pPriData )
{
    AudI2s_e eI2s;
    AudI2sClkRef_e eI2sClkRef;
    const int (*i2s_clk_ref_list)[2];

    i2s_clk_ref_list = (const int (*)[2])pPriData;

    eI2s = (AudI2s_e)i2s_clk_ref_list[0][0];
    eI2sClkRef = (AudI2sClkRef_e)i2s_clk_ref_list[0][1];

    return _HalAudI2sSetClkRef( eI2s, eI2sClkRef );
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

        case AUD_DMA_READER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_EN, (bEnable ? REG_SINE_GEN_EN : 0));
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_L|REG_SINE_GEN_R, REG_SINE_GEN_L|REG_SINE_GEN_R);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_RD_WR, 0);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_GAIN_MSK, gain<<REG_SINE_GEN_GAIN_POS);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_FREQ_MSK, freq<<REG_SINE_GEN_FREQ_POS);
            break;

        case AUD_DMA_WRITER1:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_EN, (bEnable ? REG_SINE_GEN_EN : 0));
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_L|REG_SINE_GEN_R, REG_SINE_GEN_L|REG_SINE_GEN_R);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_RD_WR, 1);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_GAIN_MSK, gain<<REG_SINE_GEN_GAIN_POS);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL5, REG_SINE_GEN_FREQ_MSK, freq<<REG_SINE_GEN_FREQ_POS);
            break;

        case AUD_DMA_WRITER2:
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_EN, (bEnable ? REG_SINE_GEN_EN : 0));
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_L|REG_SINE_GEN_R, REG_SINE_GEN_L|REG_SINE_GEN_R);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_RD_WR, 1);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_GAIN_MSK, gain<<REG_SINE_GEN_GAIN_POS);
            HalBachWriteReg(BACH_REG_BANK1, BACH_DMA_TEST_CTRL6, REG_SINE_GEN_FREQ_MSK, freq<<REG_SINE_GEN_FREQ_POS);
            break;

        default:
            ERRMSG("Function - %s # %d - eDmaChannel = %d, error !] \n", __func__, __LINE__, (int)eDmaChannel);
            return FALSE;
    }

    return TRUE;
}

BOOL HalAud2Hdmi(BOOL bEnable)
{
    if(bEnable)
    {
        HalBachWriteReg(BACH_REG_BANK1, BACH_AU2HDMI_CTRL, REG_AU2HDMI_EN, REG_AU2HDMI_EN);
        HalBachWriteReg(BACH_REG_BANK1, BACH_AU2HDMI_CTRL, REG_AU2HDMI_MUX_SEL_MSK, 0<<REG_AU2HDMI_MUX_SEL_POS); //DMA1 RD
        HalBachWriteReg(BACH_REG_BANK1, BACH_AU2HDMI_REF_FS, REG_AU2HDMI_1K_MSK, 0<<REG_AU2HDMI_1K_POS); //1KHz
    }
    else
    {
        HalBachWriteReg(BACH_REG_BANK1, BACH_AU2HDMI_CTRL, REG_AU2HDMI_EN, 0);
    }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// Private ( Only for Hal layer )
//
////////////////////////////////////////////////////////////////////////////
BOOL HalAudPriSetPadMuxI2sTdmTx(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_I2S_TX] = nPadMuxMode;
    return TRUE;
}

BOOL HalAudPriSetPadMuxI2sTdmRx(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_I2S_RX] = nPadMuxMode;
    return TRUE;
}


BOOL HalAudPriSetPadMuxDmic(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_DMIC] = nPadMuxMode;
    return TRUE;
}

BOOL HalAudPriSetPadMuxMck(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_I2S_MCK] = nPadMuxMode;
    return TRUE;
}

BOOL HalAudPriSetPadMuxI2sMode(S32 nPadMuxMode)
{
    DBGMSG("%s --- %d \n",__func__,nPadMuxMode);
    _gaPadMuxMode[AUD_PAD_MUX_I2S_RXTX] = nPadMuxMode;
    return TRUE;
}
