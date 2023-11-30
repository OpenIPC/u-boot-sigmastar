/*
* halHDMIUtilTx.h- Sigmastar
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

#ifndef _HAL_HDMIUTILTX_UTI_H_
#define _HAL_HDMIUTILTX_UTI_H_

#ifdef __cplusplus
extern "C"
{
#endif

//-------------------------------------------------------------------------------------------------
//  Hardware Capability
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Macro and Define
//-------------------------------------------------------------------------------------------------
#define DDC_I2C_HW 1
#define DDC_I2C_SW 2

#define DDC_I2C_TYPE DDC_I2C_SW

#define HDMITX_BIT0  0x0001U
#define HDMITX_BIT1  0x0002U
#define HDMITX_BIT2  0x0004U
#define HDMITX_BIT3  0x0008U
#define HDMITX_BIT4  0x0010U
#define HDMITX_BIT5  0x0020U
#define HDMITX_BIT6  0x0040U
#define HDMITX_BIT7  0x0080U
#define HDMITX_BIT8  0x0100U
#define HDMITX_BIT9  0x0200U
#define HDMITX_BIT10 0x0400U
#define HDMITX_BIT11 0x0800U
#define HDMITX_BIT12 0x1000U
#define HDMITX_BIT13 0x2000U
#define HDMITX_BIT14 0x4000U
#define HDMITX_BIT15 0x8000U

//-------------------------------------------------------------------------------------------------
//  Type and Structure
//-------------------------------------------------------------------------------------------------

typedef struct HalHdmitxRegType_s
{
    u32 bank;
    u16 address;
    u16 mask;
    u16 value;
} HalHdmitxRegType_t;

/////////////Wilson:: for Kano, HDMITX 2.0 architecture @20150702
typedef enum
{
    E_SCDC_SINK_VER_IDX          = 0x01,
    E_SCDC_SRC_VER_IDX           = 0x02,
    E_SCDC_UPDATE_0_IDX          = 0x10,
    E_SCDC_UPDATE_1_IDX          = 0x11,
    E_SCDC_TMDS_CONFIG_IDX       = 0x20,
    E_SCDC_SCRAMBLE_STAT_IDX     = 0x21,
    E_SCDC_CONFIG_0_IDX          = 0x30,
    E_SCDC_STATFLAG_0_IDX        = 0x40,
    E_SCDC_STATFLAG_1_IDX        = 0x41,
    E_SCDC_ERR_DET_START_IDX     = 0x50,
    E_SCDC_ERR_DET_CHKSUM_IDX    = 0x56,
    E_SCDC_TEST_CONFIG_0_IDX     = 0xC0,
    E_SCDC_MANUFACTURER_OUI_IDX  = 0xD0,
    E_SCDC_DEVICE_ID_IDX         = 0xD3,
    E_SCDC_MANUFACTURER_SPEC_IDX = 0xDE,
    E_SCDC_MAX,
} HalHdmitxScdcFieldOffsetList_e;

//-------------------------------------------------------------------------------------------------
//  Function and Variable
//-------------------------------------------------------------------------------------------------

#ifdef MHAL_HDMIUTILTX_C
#define INTERFACE
#else
#define INTERFACE extern
#endif

    INTERFACE void HalHdmitxUtilitySetIOMapBase(void* u32Base, void* u32PMBase, void* u32CoproBase);

    INTERFACE u8 HalHdmitxUtilityEdidReadBlock(u8 num, u8* buf);

    INTERFACE u16  HalHdmitxUtilityR2BYTE(u32 addr);
    INTERFACE u8   HalHdmitxUtilityRBYTE(u32 addr);
    INTERFACE void HalHdmitxUtilityWBYTEMSK(u32 addr, u8 reg_data, u8 reg_mask);
    INTERFACE void HalHdmitxUtilityW2BYTEMSK(u32 addr, u16 reg_data, u16 reg_mask);
    INTERFACE void HalHdmitxUtilityUtilDebugEnable(u8 benable);

    INTERFACE u32  HalHdmitxUtilityGetDDCDelayCount(void);
    INTERFACE void HalHdmitxUtilitySetDDCDelayCount(u32 u32Delay);
    INTERFACE u8   HalHdmitxUtilityAdjustDDCFreq(u32 u32Speed_K);

    // Get SwI2c En
    INTERFACE u8 HalHdmitxUtilityGetSwI2cEn(void);
    INTERFACE u8 HalHdmitxUtilityGetSwI2cSdaPin(void);
    INTERFACE u8 HalHdmitxUtilityGetSwI2cSclPin(void);

#ifdef __cplusplus
}
#endif
#undef INTERFACE

#endif // _HAL_HDMIUTILTX_UTI_H_
