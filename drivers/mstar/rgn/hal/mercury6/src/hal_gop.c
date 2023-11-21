/*
* hal_gop.c- Sigmastar
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

#define __HAL_GOP_C__
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
//#include "cam_os_wrapper.h"
#include "hal_gop_reg.h"
#include "hal_gop.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif


#define PARSING_ALPHA_TYPE(bConst)(\
                                   bConst == 0 ? "PIXEL_ALPHA": \
                                   bConst == 1 ? "CONST_ALPHA": \
                                   "Wrong_Alpha_Type")

#define PARSING_DISPLAY_MODE(x) (\
                                 x == E_HAL_GOP_DISPLAY_MODE_INTERLACE ? "INTERLACE" : \
                                 x == E_HAL_GOP_DISPLAY_MODE_PROGRESS  ? "PROGRESS"  : \
                                 "UNKNOWN")

#define PARSING_OUT_MODE(x) (\
                             x == E_HAL_GOP_OUT_FMT_RGB ? "RGB" : \
                             x == E_HAL_GOP_OUT_FMT_YUV ? "YUV" : \
                             "UNKNOWIN")

#define PARSING_SRC_FMT(x) (\
                            x == E_HAL_GOP_GWIN_SRC_RGB1555    ? "RGB1555"  :   \
                            x == E_HAL_GOP_GWIN_SRC_RGB565     ? "RGB565"   :   \
                            x == E_HAL_GOP_GWIN_SRC_ARGB4444   ? "ARGB4444" :   \
                            x == E_HAL_GOP_GWIN_SRC_2266       ? "2266"     :   \
                            x == E_HAL_GOP_GWIN_SRC_I8_PALETTE ? "I8_PALETTE" : \
                            x == E_HAL_GOP_GWIN_SRC_ARGB8888   ? "ARGB8888" :   \
                            x == E_HAL_GOP_GWIN_SRC_ARGB1555   ? "ARGB1555" :   \
                            x == E_HAL_GOP_GWIN_SRC_ABGR8888   ? "ABGR8888" :   \
                            x == E_HAL_GOP_GWIN_SRC_UV7Y8      ? "YV7Y8"    :   \
                            x == E_HAL_GOP_GWIN_SRC_UV8Y8      ? "UV8Y8"    :   \
                            x == E_HAL_GOP_GWIN_SRC_RGBA5551   ? "TGBA5551" :   \
                            x == E_HAL_GOP_GWIN_SRC_RGBA4444   ? "RGBA444"  :   \
                            x == E_HAL_GOP_GWIN_SRC_I4_PALETTE ? "I4_PALETTE" : \
                            x == E_HAL_GOP_GWIN_SRC_I2_PALETTE ? "I2_PALETTE" : \
                            "UNKNOWN")

#define HAL_GOP_PIX2BYTE(x, fmt)     (fmt)==E_HAL_GOP_GWIN_SRC_ARGB8888     || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_ABGR8888     ? (x) * 4 : \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_RGB1555      || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_RGB565       || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_ARGB4444     || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_ARGB1555     || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_RGBA5551     || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_RGBA4444     || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_2266         || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_UV7Y8        || \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_UV8Y8        ? (x) * 2 : \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_I8_PALETTE   ? (x) * 1 : \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_I4_PALETTE   ? (x) / 2 : \
                                     (fmt)==E_HAL_GOP_GWIN_SRC_I2_PALETTE   ? (x) / 4 : \
                                                                             (x)

#define GOP_WORD_BASE 0
#define GOP_PIXEL_BASE 1
//#define GOP_PIPE_DELAY(x) 0

#define HAL_GOP_GET_BANK(eGopId) ((eGopId==E_HAL_GOP_ID_0)?REG_GOP0_BASE:\
                                  (eGopId==E_HAL_GOP_ID_1)?REG_GOP1_BASE:\
                                  (eGopId==E_HAL_GOP_ID_2)?REG_GOP2_BASE:\
                                  (eGopId==E_HAL_GOP_ID_3)?REG_GOP3_BASE:\
                                  (eGopId==E_HAL_GOP_ID_4)?REG_GOP4_BASE:\
                                  (eGopId==E_HAL_GOP_ID_5)?REG_GOP5_BASE:\
                                  (eGopId==E_HAL_GOP_ID_6)?REG_GOP_DISP0_CUR_BASE:\
                                  (eGopId==E_HAL_GOP_ID_7)?REG_GOP_DISP0_UI_BASE:\
                                  (eGopId==E_HAL_GOP_ID_8)?REG_GOP_DISP1_CUR_BASE:\
                                  (eGopId==E_HAL_GOP_ID_9)?REG_GOP_DISP1_UI_BASE:\
                                                          0)

#define HAL_GOP_IS_LDC(eGopId)      (eGopId==E_HAL_GOP_ID_5 && (geGopOsdFlag&E_HAL_RGN_OSD_FLAG_LDC))
#define HAL_GOP_IS_DISP_CUR(eGopId) (eGopId==E_HAL_GOP_ID_6 || eGopId==E_HAL_GOP_ID_8)
#define HAL_GOP_IS_DISP_UI(eGopId)  (eGopId==E_HAL_GOP_ID_7 || eGopId==E_HAL_GOP_ID_9)
#define HAL_GOP_IS_DISP(eGopId)     (HAL_GOP_IS_DISP_CUR(eGopId)||HAL_GOP_IS_DISP_UI(eGopId))

typedef struct
{
    MS_U16 u16HStart;
    MS_U16 u16VStart;
    MS_U16 u16HEnd;
    MS_U16 u16VEnd;
} HalGopGwinSizeReg_t;

extern unsigned long gGopDmaThd;

//if set 1, use cmdq to set register, if set 0, use cpu to set register
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
HalGopWrRegType_e _eGopWrRegMd = E_HAL_GOP_ALLBANK_DOUBLE_WR;
void (*gGoppfForSet[E_HAL_QUERY_GOP_MAX])(HalGopIdType_e, void *); //*pCfg

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
static bool _HalGopIdTransCmdqID(HalGopIdType_e eGopId,HalRgnCmdqIdType_e *pCmdqId)
{
    switch(eGopId)
    {
        case E_HAL_GOP_ID_0:
        case E_HAL_GOP_ID_1:
        case E_HAL_GOP_ID_2:
        case E_HAL_GOP_ID_3:
        case E_HAL_GOP_ID_4:
            *pCmdqId = E_HAL_RGN_CMDQ_VPE_ID_0;
            return TRUE;
        case E_HAL_GOP_ID_5:
            if(geGopOsdFlag & E_HAL_RGN_OSD_FLAG_DIP)
            {
                *pCmdqId =  E_HAL_RGN_CMDQ_DIVP_ID_1;
            }
            else if(geGopOsdFlag & E_HAL_RGN_OSD_FLAG_LDC)
            {
                *pCmdqId =  E_HAL_RGN_CMDQ_LDC_ID_2;
            }
            else
            {
                *pCmdqId = E_HAL_RGN_CMDQ_VPE_ID_0;
            }
            return TRUE;
        case E_HAL_GOP_ID_6:
        case E_HAL_GOP_ID_7:
        case E_HAL_GOP_ID_8:
        case E_HAL_GOP_ID_9:
            *pCmdqId = E_HAL_RGN_CMDQ_DISP_ID_3;
            return TRUE;
        default:
            *pCmdqId =  E_HAL_RGN_CMDQ_ID_NUM;
            HALRGNERR("[GOP]%s %d: wrong GOPId\n", __FUNCTION__, __LINE__);
            break;
    }
    return FALSE;
}
static MS_U16 _HalGopGetDoubleRW(HalGopIdType_e eGopId)
{
    MS_U16 u16DoubleRW;
    if(_eGopWrRegMd==E_HAL_GOP_FORCE_WR)
    {
        u16DoubleRW = GOP_BANK_FORCE_WR;
    }
    else if(_eGopWrRegMd==E_HAL_GOP_ALLBANK_DOUBLE_WR)
    {
        u16DoubleRW = GOP_BANK_ALL_DOUBLE_WR_G00;
    }
    else
    {
        u16DoubleRW = GOP_BANK_DOUBLE_WR_G00;
    }
    return u16DoubleRW;

}
static void _HalGopWriteDoubleBuffer(MS_U32 GOP_Reg_Base, HalGopIdType_e eGopId,HalRgnCmdqIdType_e eCmdqId)
{
    MS_U16 GOP_Reg_DB = 0;
    GOP_Reg_DB = _HalGopGetDoubleRW(eGopId);
    // Double Buffer Write
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_7F, 0, GOP_BANK_WR_SEL_MSK,eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_7F, GOP_Reg_DB, (GOP_BANK_WR_SEL_MSK | GOP_BANK_SEL_MSK),eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_7F, 0, GOP_BANK_WR_SEL_MSK,eCmdqId);
}

static MS_U32 _HalGopGetBaseAddr(HalGopIdType_e eGopId)
{
    MS_U32 u32Base = 0;

    u32Base = HAL_GOP_GET_BANK(eGopId);
    if(u32Base==0) {
        HALRGNERR("[%s]Wrong eGopId=%d",__FUNCTION__,eGopId);
        CamOsPanic("");
    }
    return u32Base;
}
static MS_U32 _HalGopGetGwinBaseAddr(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId)
{
    MS_U32 u32Base = 0, u32GwinShift = 0;

    u32Base = HAL_GOP_GET_BANK(eGopId);
    if(u32Base==0) {
        HALRGNERR("[%s]Wrong eGopId=%d",__FUNCTION__,eGopId);
        CamOsPanic("");
    }

    if(eGwinId >= E_HAL_GOP_GWIN_ID_NUM) {
        HALRGNERR("[%s]Wrong eGwinId=%d",__FUNCTION__,eGwinId);
        CamOsPanic("");
    }
    u32GwinShift = ((eGwinId)>E_HAL_GOP_GWIN_ID_3?REG_GOP_GWIN_BASE_SHIFT*2:REG_GOP_GWIN_BASE_SHIFT);
    return (u32Base + u32GwinShift);
}


static MS_U32 _HalGopGwinRegBaseShift(HalGopGwinIdType_e eGwinId)
{
    MS_U32 u32RegBaseShift = REG_GWIN0_REGBASE_OFFSET;

    switch(eGwinId)
    {
        case E_HAL_GOP_GWIN_ID_0:
        case E_HAL_GOP_GWIN_ID_4:
            u32RegBaseShift = REG_GWIN0_REGBASE_OFFSET;
            break;
        case E_HAL_GOP_GWIN_ID_1:
        case E_HAL_GOP_GWIN_ID_5:
            u32RegBaseShift = REG_GWIN1_REGBASE_OFFSET;
            break;
        case E_HAL_GOP_GWIN_ID_2:
        case E_HAL_GOP_GWIN_ID_6:
            u32RegBaseShift = REG_GWIN2_REGBASE_OFFSET;
            break;
        case E_HAL_GOP_GWIN_ID_3:
        case E_HAL_GOP_GWIN_ID_7:
            u32RegBaseShift = REG_GWIN3_REGBASE_OFFSET;
            break;
        default:
            HALRGNERR("[%s]Wrong eGwinId=%d",__FUNCTION__,eGwinId);
            CamOsPanic("");
            break;
    }
    return u32RegBaseShift;
}

static MS_U16 _HalGopGetMiuAlign(HalGopIdType_e eGopId)
{
    MS_U16 u16MiuAlign;

    switch(eGopId)
    {
        case E_HAL_GOP_ID_0:
        case E_HAL_GOP_ID_1:
        case E_HAL_GOP_ID_2:
        case E_HAL_GOP_ID_3:
        case E_HAL_GOP_ID_4:
        case E_HAL_GOP_ID_5:
        case E_HAL_GOP_ID_6:
        case E_HAL_GOP_ID_7:
        case E_HAL_GOP_ID_8:
        case E_HAL_GOP_ID_9:
            u16MiuAlign = MIU_BUS_ALIGN_GOP_DEFAULT;
            break;
        default:
            HALRGNERR("[%s]Wrong eGopId=%d",__FUNCTION__,eGopId);
            CamOsPanic("");
            break;
    }
    return u16MiuAlign;

}
void _HalGopCalStrWinRatio(MS_U32 *ratio_floor, MS_U32 *init, MS_U16 src, MS_U16 dst)
{
    if(src==dst) {
        *ratio_floor = 0x1000;
        *init = 0;
    } else {
        *ratio_floor = ((src)<<12)/(dst);
        *init = ((src)<<12)-(dst)*(*ratio_floor);
    }
}
static HalGopGwinSrcFormatRealVal_e _HalGopSrcFmtTransRealVal(HalGopIdType_e eGopId, HalGopGwinSrcFormat_e eSrcFmt)
{
    HalGopGwinSrcFormatRealVal_e eVal = E_HAL_GOP_GWIN_SRC_VAL_NOTSUPPORT;
    if(HAL_GOP_IS_DISP_UI(eGopId)) {
        switch(eSrcFmt)
        {
            case E_HAL_GOP_GWIN_SRC_I8_PALETTE:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_I8_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_I4_PALETTE:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_I4_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_I2_PALETTE:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_I2_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_ARGB1555:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_ARGB1555;
                break;
            case E_HAL_GOP_GWIN_SRC_ARGB4444:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_ARGB4444;
                break;
            case E_HAL_GOP_GWIN_SRC_RGB1555:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_RGB1555;
                break;
            case E_HAL_GOP_GWIN_SRC_RGB565:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_RGB565;
                break;
            case E_HAL_GOP_GWIN_SRC_2266:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_2266;
                break;
            case E_HAL_GOP_GWIN_SRC_ARGB8888:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_ARGB8888;
                break;
            case E_HAL_GOP_GWIN_SRC_ABGR8888:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_ABGR8888;
                break;
            case E_HAL_GOP_GWIN_SRC_UV7Y8:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_UV7Y8;
                break;
            case E_HAL_GOP_GWIN_SRC_UV8Y8:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_UV8Y8;
                break;
            case E_HAL_GOP_GWIN_SRC_RGBA5551:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_RGBA5551;
                break;
            case E_HAL_GOP_GWIN_SRC_RGBA4444:
                eVal = E_HAL_GOP_OLD_GWIN_SRC_RGBA4444;
                break;
            default:
                eVal = E_HAL_GOP_GWIN_SRC_VAL_NOTSUPPORT;
                HALRGNERR("[GOP]%s %d: GOPId=0x%x %s not support\n", __FUNCTION__, __LINE__, eGopId, PARSING_SRC_FMT(eSrcFmt));
                break;
        }
    } else {
        switch(eSrcFmt)
        {
            case E_HAL_GOP_GWIN_SRC_I8_PALETTE:
                eVal = E_HAL_GOP_NEW_GWIN_SRC_I8_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_I4_PALETTE:
                eVal = E_HAL_GOP_NEW_GWIN_SRC_I4_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_I2_PALETTE:
                eVal = E_HAL_GOP_NEW_GWIN_SRC_I2_PALETTE;
                break;
            case E_HAL_GOP_GWIN_SRC_ARGB1555:
                eVal = E_HAL_GOP_NEW_GWIN_SRC_ARGB1555;
                break;
            case E_HAL_GOP_GWIN_SRC_ARGB4444:
                eVal = E_HAL_GOP_NEW_GWIN_SRC_ARGB4444;
                break;
            default:
                eVal = E_HAL_GOP_GWIN_SRC_VAL_NOTSUPPORT;
                HALRGNERR("[GOP]%s %d: GOPId=0x%x %s not support\n", __FUNCTION__, __LINE__, eGopId, PARSING_SRC_FMT(eSrcFmt));
                break;
        }
    }
    return eVal;
}
static MS_U16 _HalGopGetBindMode(HalGopIdType_e eGopId, HalGopOutFormatType_e *outFmt)
{
    if(HAL_GOP_IS_LDC(eGopId)) // LDC
    {
        (*outFmt) = E_HAL_GOP_OUT_FMT_YUV;
        return GOP_HS_MASK|(((MS_U16)(*outFmt)) << 10);
    }
    else if(HAL_GOP_IS_DISP_CUR(eGopId)) // DISP_CUR
    {
        (*outFmt) = E_HAL_GOP_OUT_FMT_RGB;
        return (GOP_HS_MASK|(((MS_U16)(*outFmt)) << 10));
    }
    else if(HAL_GOP_IS_DISP_UI(eGopId)) // DISP_UI
    {
        (*outFmt) = E_HAL_GOP_OUT_FMT_RGB;
        return (GOP_VS_INV|GWIN_DISP_MD_PROGRESS|(((MS_U16)(*outFmt)) << 10));
    }
    else // SCL
    {
        (*outFmt) = E_HAL_GOP_OUT_FMT_YUV;
        return (GOP_VS_INV|GOP_HS_MASK|(((MS_U16)(*outFmt)) << 10));
    }
}
static MS_U16 _HalGopGetStretchWidth(HalGopIdType_e eGopId, MS_U16 u16Width)
{
    if(HAL_GOP_IS_DISP_UI(eGopId)) // DISP_UI
    {
        return (u16Width>>1);
    }
    else
    {
        return u16Width;
    }
}
static void _HalGopGetGwinSize(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopWindowType_t *ptGwinCfg, HalGopGwinSrcFormat_e eSrcFmt, HalGopGwinSizeReg_t *pstGwinSz, HalGopGwinSizeReg_t *pstGwinReg)
{
    MS_U16 MIU_BUS = _HalGopGetMiuAlign(eGopId);
    if(HAL_GOP_IS_DISP_UI(eGopId)) {
        pstGwinSz->u16HStart = (HAL_GOP_PIX2BYTE(ptGwinCfg->u16X, eSrcFmt))>>MIU_BUS;
        pstGwinSz->u16VStart = ptGwinCfg->u16Y;
        pstGwinSz->u16HEnd   = (HAL_GOP_PIX2BYTE(ptGwinCfg->u16X + ptGwinCfg->u16Width, eSrcFmt))>>MIU_BUS;
        pstGwinSz->u16VEnd   = ptGwinCfg->u16Y + ptGwinCfg->u16Height;
        pstGwinReg->u16HStart = REG_GOP_04;
        pstGwinReg->u16VStart = REG_GOP_06;
        pstGwinReg->u16HEnd   = REG_GOP_05;
        pstGwinReg->u16VEnd   = REG_GOP_08;

    } else {
        pstGwinSz->u16HStart = ptGwinCfg->u16X;
        pstGwinSz->u16VStart = ptGwinCfg->u16Y;
        pstGwinSz->u16HEnd   = ptGwinCfg->u16X + ptGwinCfg->u16Width -1;
        pstGwinSz->u16VEnd   = ptGwinCfg->u16Y + ptGwinCfg->u16Height-1;
        pstGwinReg->u16HStart = REG_GOP_03;
        pstGwinReg->u16VStart = REG_GOP_05;
        pstGwinReg->u16HEnd   = REG_GOP_04;
        pstGwinReg->u16VEnd   = REG_GOP_06;
    }
}
//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
void HalGopGetClkNum(HalGopIdType_e eGopId, MS_U32 *u32clknum)
{
	HalRgnCmdqIdType_e eCmdqId;
	if(_HalGopIdTransCmdqID(eGopId,&eCmdqId))
	{
	    if(HAL_GOP_IS_LDC(eGopId))
		{
			*u32clknum = 2;
		}
		else if(HAL_GOP_IS_DISP(eGopId))
		{
			*u32clknum = 3;
		}
		else
		{
			*u32clknum = 1;
		}
	}
	else
	{
		HALRGNERR("[GOP]%s %d: wrong GOPId\n", __FUNCTION__, __LINE__);
		*u32clknum = 0;
	}
}
bool HalGopSetClkEnable(HalGopIdType_e eGopId)
{
    HalRgnCmdqIdType_e eCmdqId = E_HAL_RGN_CMDQ_ID_NUM;
    MS_U16 u16Clk;
    MS_U32 u32ClkReg;
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    if(HAL_GOP_IS_DISP(eGopId))
    {
        u32ClkReg = (REG_DISP_TO_GOP_CLK_BASE); // TODO
    }
    else if(HAL_GOP_IS_LDC(eGopId))
    {
        u32ClkReg = (REG_LDC_TO_GOP_CLK_BASE);
    }
    else
    {
        u32ClkReg = (REG_SC_TO_GOP_CLK_BASE);
    }
    eCmdqId = E_HAL_RGN_CMDQ_ID_NUM;
    u16Clk = HalRgnRead2Byte(u32ClkReg);
    if(u16Clk&REG_CLK_DISABLE)
    {
        HalRgnWrite2ByteMsk(u32ClkReg, REG_CLK_ENABLE, REG_CLK_EN_MSK,eCmdqId);
    }
    return (u16Clk&REG_CLK_DISABLE) ? 1 : 0;
}
bool HalGopSetClkDisable(HalGopIdType_e eGopId,bool bEn)
{
    MS_U32 u32ClkReg;
    MS_U16 u16Clk;
    HalRgnCmdqIdType_e eCmdqId = E_HAL_RGN_CMDQ_ID_NUM;
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    if(HAL_GOP_IS_DISP(eGopId))
    {
        u32ClkReg = (REG_DISP_TO_GOP_CLK_BASE); // TODO
    }
    else if(HAL_GOP_IS_LDC(eGopId))
    {
        u32ClkReg = (REG_LDC_TO_GOP_CLK_BASE);
    }
    else
    {
        u32ClkReg = (REG_SC_TO_GOP_CLK_BASE);
    }
    if(bEn)
    {
        u16Clk = HalRgnRead2Byte(u32ClkReg);
        if(u16Clk&REG_CLK_DISABLE)
        {
            // ToDo:for ensure DIP/SC module didn't init
            //HalRgnWrite2ByteMsk(u32ClkReg, REG_CLK_DISABLE, REG_CLK_EN_MSK,E_HAL_RGN_CMDQ_ID_NUM);
        }
    }
    return bEn;
}
void HalGopMiuSelect(HalGopIdType_e eGopId, MS_U32 *u32BaseAddr)
{
}
void HalGopUpdateBaseXoffset(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, MS_U16 u16Xoffset)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;

    if(!HAL_GOP_IS_DISP_UI(eGopId))
    {
        GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
        GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
        HALRGNDBG("[GOP]%s %d: Id:%02x, in, Xoffset=0x%x \n", __FUNCTION__, __LINE__, eGopId, u16Xoffset);
        _HalGopIdTransCmdqID(eGopId,&eCmdqId);
        //base address register is at gwin

        HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_08, (u16Xoffset<<GOP_dram_rblk0_hstr_Shift),GOP_dram_rblk0_hstr_MASK, eCmdqId);

        // Double Buffer Write
        _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
    }
    else
    {
        HALRGNERR("[GOP]%s %d: GOPId=0x%x not support\n", __FUNCTION__, __LINE__,eGopId);
    }
}
void HalGopUpdateBase(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, MS_U32 u32BaseAddr)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    MS_U16 MIU_BUS;
    MS_U32 u32TempVal;
    HalRgnCmdqIdType_e eCmdqId;
    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
    MIU_BUS = _HalGopGetMiuAlign(eGopId);
    HALHLRGNDBG("[GOP]%s %d: Id:%02x, in, addr=0x%x \n", __FUNCTION__, __LINE__, eGopId, u32BaseAddr);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    //base address register is at gwin

    u32TempVal = u32BaseAddr >> MIU_BUS; // 128-bit unit = 16 bytes
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_01, (u32TempVal & 0xFFFF),0xFFFF,eCmdqId); //Ring block start Low address 0x21:16
    u32TempVal = u32TempVal >> 0x10;
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_02, (u32TempVal & 0xFFFF),0xFFFF,eCmdqId); //Ring block start Hi address 0x22:6, page number
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
// set gwin pitch directly without calcuting with color format
void HalGopSetGwinMemPitchDirect(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, MS_U32 u32MemPitch)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    MS_U16 MIU_BUS, u16Reg;
    HalRgnCmdqIdType_e eCmdqId;
    HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);
    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
    MIU_BUS = _HalGopGetMiuAlign(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    u16Reg = HAL_GOP_IS_DISP_UI(eGopId)?REG_GOP_09:REG_GOP_07;
    // framebuffer pitch
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + u16Reg, (u32MemPitch >> MIU_BUS),0x7FF,eCmdqId); //bytes per line for gop framebuffer
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
bool HalGopCheckIdSupport(HalGopIdType_e eGopId)
{
    bool bRet = 1;
    if(eGopId > E_HAL_GOP_ID_MAX)
    {
        bRet = 0;
    }
    HALHLRGNDBG("[GOP]%s %hhd\n", __FUNCTION__, bRet);
    return bRet;
}
void HalGopSetAlphaPointVal(HalGopGwinAlphaDef_e enType,HalGopGwinAlphaVal_t *tAlphaVal,u8 **p8Alpha)
{
    switch(enType)
    {
        case E_HAL_GOP_GWIN_ALPHA_DEFINE_CONST:
        case E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA0:
            *p8Alpha = &tAlphaVal->u8ConstantAlpahValue;
        break;
        case E_HAL_GOP_GWIN_ALPHA_DEFINE_ALPHA1:
            *p8Alpha = &tAlphaVal->u8Argb1555Alpha1Val;
        break;
    }
}
void HalGopUpdateParam(HalGopIdType_e eGopId, HalGopParamConfig_t *ptParamCfg)
{
    MS_U32 GOP_Reg_Base = 0;
    //MS_U16 GOP_Pipe_Delay = 0;
    HalRgnCmdqIdType_e eCmdqId;
    MS_U16 u16Width = 0, u16Mode = 0;

    HALRGNDBG("[GOP]%s %d: Id:%02x, Disp:%s, Out:%s, (%d %d %d %d)\n",
              __FUNCTION__, __LINE__, eGopId,
              PARSING_DISPLAY_MODE(ptParamCfg->eDisplayMode),
              PARSING_OUT_MODE(ptParamCfg->eOutFormat),
              ptParamCfg->tStretchWindow.u16X, ptParamCfg->tStretchWindow.u16Y,
              ptParamCfg->tStretchWindow.u16Width, ptParamCfg->tStretchWindow.u16Height);
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);
    //GOP_Pipe_Delay = GOP_PIPE_DELAY(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);

    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00, GOP_SOFT_RESET,GOP_SOFT_RESET,eCmdqId); //reset GOP_FIELD_INV not use
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00, 0,GOP_SOFT_RESET,eCmdqId); //reset GOP_FIELD_INV not use
    //HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_01, GOP_REGDMA_INTERVAL_START | GOP_REGDMA_INTERVAL_END,GOP_REGDMA_INTERVAL_ST_MSK|GOP_REGDMA_INTERVAL_ED_MSK,eCmdqId);

    u16Mode = _HalGopGetBindMode(eGopId, &(ptParamCfg->eOutFormat));
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00,
                         u16Mode,
                         GOP_VS_INV|GOP_HS_MASK|GOP_DISPLAY_MODE|GOP_OUTPUT_FORMAT,eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_02, GOP_GWIN_VDE_INV_ENABLE|GOP_GWIN_VS_SEL_VSYNC,
                                                   GOP_GWIN_VDE_INV_MSK   |GOP_GWIN_VS_SEL_MSK,eCmdqId);
    //HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_0E, (ptParamCfg->tStretchWindow.u16Width >> 1),0x7FF,eCmdqId); //miu efficiency = Stretch Window H size (unit:2 pixel)/2
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_19, (gGopDmaThd)?gGopDmaThd:GOP_DMA_THD, GOP_DMA_THD_MSK,eCmdqId);
    if(HAL_GOP_IS_DISP_UI(eGopId)) {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_10, 0x501,0xFFF,eCmdqId); // h10 [14:8]reg_pause_detect_thd=5
    } else {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_70, 0, REG_GOPG_WAIT_RD_LAT_EN|REG_GOPG_WAIT_REGDMA_DONE_EN,eCmdqId); // mantis 1684582
    }

    u16Width = _HalGopGetStretchWidth(eGopId,ptParamCfg->tStretchWindow.u16Width);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_30, u16Width,0xFFF,eCmdqId); // Stretch Window H size (unit:1 pixel)
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_31, ptParamCfg->tStretchWindow.u16Height,0xFFF,eCmdqId); // Stretch window V size
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_32, ptParamCfg->tStretchWindow.u16X,0xFFF,eCmdqId); // Stretch Window H coordinate
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_34, ptParamCfg->tStretchWindow.u16Y,0xFFF,eCmdqId); // Stretch Window V coordinate

    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopUpdateGwinParam(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopGwinParamConfig_t *ptParamCfg)
{
    u32 GOP_Reg_Base = 0;
    u16 MIU_BUS;
    u32 u32BaseAddr;
    u32 GWIN_Reg_Shift_Base = 0;
    HalGopGwinSizeReg_t stGwinSz, stGwinReg;
    HalRgnCmdqIdType_e eCmdqId;

    HALRGNDBG("[GOP]%s %d: Id:%02x, Src:%s, (%d %d %d %d), Base:%08x Offset(%04x, %08x)\n",
              __FUNCTION__, __LINE__, eGopId,
              PARSING_SRC_FMT(ptParamCfg->eSrcFmt),
              ptParamCfg->tDispWindow.u16X, ptParamCfg->tDispWindow.u16Y,
              ptParamCfg->tDispWindow.u16Width, ptParamCfg->tDispWindow.u16Height,
              ptParamCfg->u32BaseAddr,
              ptParamCfg->u16Base_XOffset, ptParamCfg->u32Base_YOffset);

    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
    MIU_BUS        =  _HalGopGetMiuAlign(eGopId);

    _HalGopGetGwinSize(eGopId, eGwinId, &(ptParamCfg->tDispWindow), ptParamCfg->eSrcFmt, &stGwinSz, &stGwinReg);

    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_00, (((u8)ptParamCfg->eSrcFmt)<<4),0x00F0, eCmdqId);

    // framebuffer starting address
    u32BaseAddr = ptParamCfg->u32BaseAddr;
    u32BaseAddr = u32BaseAddr >> MIU_BUS; // 128-bit unit = 16 bytes
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_01, (u32BaseAddr & 0xFFFF),0xFFFF, eCmdqId);
    u32BaseAddr = u32BaseAddr >> 0x10;
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_02, (u32BaseAddr & 0xFFFF),0xFFF, eCmdqId);
    // framebuffer pitch ((h_size * byte_per_pixel)>>MIU_BUS)
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_07,
        (HAL_GOP_PIX2BYTE(ptParamCfg->tDispWindow.u16Width, ptParamCfg->eSrcFmt)) >> MIU_BUS,0x7FF, eCmdqId); //bytes per line for gop framebuffer

    if(!HAL_GOP_IS_DISP_UI(eGopId)) {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_08, ((ptParamCfg->u16Base_XOffset<<GOP_dram_rblk0_hstr_Shift)|GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK),GOP_dram_rblk0_hstr_MASK|GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK, eCmdqId);
    } else {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_0A, GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK,GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK, eCmdqId);
    }
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16HStart, stGwinSz.u16HStart,0xFFF, eCmdqId); // H start
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16HEnd  , stGwinSz.u16HEnd  ,0xFFF, eCmdqId); // H end
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16VStart, stGwinSz.u16VStart,0xFFF, eCmdqId); // V start line
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16VEnd  , stGwinSz.u16VEnd  ,0xFFF, eCmdqId); // V end line
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}

void HalGopSetStretchWindowSize(HalGopIdType_e eGopId, HalGopWindowType_t *ptSrcWinCfg, HalGopWindowType_t *ptDstWinCfg)
{
    MS_U32 GOP_Reg_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    MS_U16 u16Width = 0;
    MS_U32 u32RatioFloor=0, u32Init=0;
    HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    u16Width = _HalGopGetStretchWidth(eGopId,ptSrcWinCfg->u16Width);

    // GOP global settings
    //HalRgnWrite2ByteMsk(GOP_Reg_Base+REG_GOP_0E,(ptSrcWinCfg->u16Width >>1)+1,0x7FF,eCmdqId);//miu efficiency = Stretch Window H size (unit:2 pixel) /2 +1
    // GOP display area global settings
    HalRgnWrite2ByteMsk(GOP_Reg_Base+REG_GOP_30,u16Width,0xFFFF,eCmdqId); //Stretch Window H size (unit:1 pixel)
    HalRgnWrite2ByteMsk(GOP_Reg_Base+REG_GOP_31,ptSrcWinCfg->u16Height,0xFFFF,eCmdqId);  //Stretch window V size
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_32, ptSrcWinCfg->u16X,0xFFFF,eCmdqId); // Stretch Window H coordinate
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_34, ptSrcWinCfg->u16Y,0xFFFF,eCmdqId); // Stretch Window V coordinate

    if(HAL_GOP_IS_DISP_UI(eGopId)) { // support only scaling up

        if((ptSrcWinCfg->u16Width > ptDstWinCfg->u16Width) ||
           (ptSrcWinCfg->u16Height > ptDstWinCfg->u16Height))
        {
            HALRGNERR("[GOP]%s %d: DISP_UI Not Support H/V scaling down\n", __FUNCTION__, __LINE__);
            HALRGNERR("[GOP]%s %d: SrcW=%d, SrcH=%d, DstW=%d, DstH=%d\n", __FUNCTION__, __LINE__,
            ptSrcWinCfg->u16Width,ptSrcWinCfg->u16Height,ptDstWinCfg->u16Width,ptDstWinCfg->u16Height);
        }
        else
        {
            //H scaling
            _HalGopCalStrWinRatio(&u32RatioFloor,&u32Init,ptSrcWinCfg->u16Width,ptDstWinCfg->u16Width);
            HALRGNDBG("[GOP]ID:%d,W:src=%d,dst=%d,ratio=0x%x,init=0x%x\n",eGopId,ptSrcWinCfg->u16Width,ptDstWinCfg->u16Width,u32RatioFloor,u32Init);
            HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_35,u32RatioFloor&0x1FFF,0xFFFF,eCmdqId);  //Stretch Window H ratio (in/out * 2^12)
            HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_38,u32Init&0x1FFF,0xFFFF,eCmdqId);  // h38 [12:0]reg_hstrch_ini(Stretch H start value)

            //V scaling
            _HalGopCalStrWinRatio(&u32RatioFloor,&u32Init,ptSrcWinCfg->u16Height,ptDstWinCfg->u16Height);
            HALRGNDBG("[GOP]ID:%d,H:src=%d,dst=%d,ratio=0x%x,init=0x%x\n",eGopId,ptSrcWinCfg->u16Height,ptDstWinCfg->u16Height,u32RatioFloor,u32Init);
            HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_36,u32RatioFloor&0x1FFF,0xFFFF,eCmdqId);  //Stretch window V ratio  (in/out * 2^12)
            HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_39,u32Init&0x1FFF,0xFFFF,eCmdqId);  // h39 [12:0]reg_vstrch_ini(Stretch V start value)

            //stretch mode
            HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_3A, 0,0xFFFF,eCmdqId); // h3A [4]reg_trs_strch(0:Transparent color only duplicate if stretch
                                                                    //                      1:Stretch transparent color like normal color)
                                                                    //     [3:2]reg_vstrch_md(0:Linear 1:Duplicate 2:Nearest)
                                                                    //     [1:0]reg_hstrch_md(0:6-tap(including nearest) 1:duplicate)
        }
    }
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}

void HalGopSetGwinSize(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopWindowType_t *ptGwinCfg, HalGopGwinSrcFormat_e eSrcFmt)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    HalGopGwinSizeReg_t stGwinSz, stGwinReg;
    HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);
    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);

    _HalGopGetGwinSize(eGopId, eGwinId, ptGwinCfg, eSrcFmt, &stGwinSz, &stGwinReg);

    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16HStart, stGwinSz.u16HStart,0xFFF, eCmdqId); // H start
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16HEnd  , stGwinSz.u16HEnd,0xFFF, eCmdqId); // H end
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16VStart, stGwinSz.u16VStart,0xFFF, eCmdqId); // V start line
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + stGwinReg.u16VEnd  , stGwinSz.u16VEnd,0xFFF, eCmdqId); // V end line
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}

void HalGopSetGwinSrcFmt(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopGwinSrcFormat_e eSrcFmt)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    HalGopGwinIdType_e eGwinIdTmp = E_HAL_GOP_GWIN_ID_0;
    HalGopGwinIdType_e eGwinMax = (HAL_GOP_IS_DISP(eGopId))?E_HAL_GOP_GWIN_ID_0:E_HAL_GOP_GWIN_ID_7;
    HalGopGwinSrcFormatRealVal_e eValSrcFmt;

    HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);

    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    eValSrcFmt = _HalGopSrcFmtTransRealVal(eGopId, eSrcFmt);
    for(eGwinIdTmp=E_HAL_GOP_GWIN_ID_0;eGwinIdTmp<=eGwinMax;eGwinIdTmp++)
    {
        GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinIdTmp);
        GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinIdTmp);
        HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_00, (eValSrcFmt<<4),0x00F0, eCmdqId);
    }
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopSetAlphaBlending(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, bool bConstantAlpha, u8 Alpha)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    MS_U16 u16ATypeCon=0, u16ATypePix=0, u16ATypeMsk=0, u16AValReg=0;
    HalRgnCmdqIdType_e eCmdqId;

    // 1. GOP Set Alpha Blending
    // 2. pixel_alpha=1, constant_alpha=0
    // 3. Alpha: constant Alpha value
    HALRGNDBG("[GOP]%s %d: Id:%02x, bConstantAlpha:%d, Alpha:%x\n", __FUNCTION__, __LINE__, eGopId, bConstantAlpha, Alpha);
    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);

    if(HAL_GOP_IS_DISP_UI(eGopId)) {
        u16ATypeCon = GOP_OLD_CONST_ALPHA_EN;
        u16ATypePix = GOP_OLD_PIXEL_ALPHA_EN;
        u16ATypeMsk = GOP_OLD_ALPHA_MASK;
        u16AValReg  = REG_GOP_0A;
    } else {
        u16ATypeCon = GOP_CONST_ALPHA_EN;
        u16ATypePix = GOP_PIXEL_ALPHA_EN;
        u16ATypeMsk = GOP_ALPHA_MASK;
        u16AValReg  = REG_GOP_08;
    }
    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_00, (bConstantAlpha ? u16ATypeCon : u16ATypePix),u16ATypeMsk, eCmdqId);
    // Alpha value
    if(bConstantAlpha)
    {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + u16AValReg, Alpha,GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK, eCmdqId);
    }
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopSetColorKey(HalGopIdType_e eGopId, HalGopColorKeyConfig_t *ptCfg, bool bVYU)
{
    MS_U32 GOP_Reg_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    // 1. GOP Set RGB Color Key
    // 2. bEn= 0: disable; 1: enable
    // 3. u8R: red color value; u8G: green color value; u8B: blue color value ; value range: 0~255 (0x0~0xFF)
    HALRGNDBG("[GOP]%s %d: Id:%02x, bEn:%d, (R=%x, G=%x, B=%x)\n", __FUNCTION__, __LINE__,
    eGopId, ptCfg->bEn, ptCfg->u8R, ptCfg->u8G, ptCfg->u8B);
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    if(bVYU && !HAL_GOP_IS_DISP_UI(eGopId))
    {
        return;
    }
    if(ptCfg->bEn)
    {

        HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00, bVYU?GOP_VYU_TRANSPARENT_COLOR_ENABLE:GOP_RGB_TRANSPARENT_COLOR_ENABLE,
                                                       bVYU?GOP_VYU_TRANSPARENT_COLOR_ENABLE:GOP_RGB_TRANSPARENT_COLOR_ENABLE,eCmdqId);
        HalRgnWrite2ByteMsk(GOP_Reg_Base + (bVYU?REG_GOP_26:REG_GOP_24), (ptCfg->u8B | (ptCfg->u8G << 8)), 0xFFFF,eCmdqId);
        HalRgnWrite2ByteMsk(GOP_Reg_Base + (bVYU?REG_GOP_27:REG_GOP_25),  ptCfg->u8R, 0x00FF,eCmdqId);
    }
    else
    {
        HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00, bVYU?GOP_VYU_TRANSPARENT_COLOR_DISABLE:GOP_RGB_TRANSPARENT_COLOR_DISABLE,
                                                       bVYU?GOP_VYU_TRANSPARENT_COLOR_ENABLE:GOP_RGB_TRANSPARENT_COLOR_ENABLE,eCmdqId);
    }
    // Double Buffer Write
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopSetPaletteRiu(HalGopIdType_e eGopId, HalGopPaletteConfig_t *ptCfg)
{
    MS_U32 GOP_Reg_Base = 0;
    HalRgnCmdqIdType_e eCmdqId = E_HAL_RGN_CMDQ_ID_NUM; // force RIU
    HalRgnCmdqIdType_e eGopBindType;
    _HalGopIdTransCmdqID(eGopId,&eGopBindType); // for selecting clock source
    // 1. GOP Set Palette by RIU Mode
    // 2. GOP Palette SRAM Clock should be opened
    // 3. Set Force Write
    // 4. Set RIU Mode
    // 5. Set Palette Index, A, R, G, B Values
    // 6. Trigger Palette Write
    // 7. Close Force Write
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);

    //set force write
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_7F, GOP_BANK_FORCE_WR, GOP_BANK_FORCE_WR,eCmdqId);

    //ensure clock select correct
    if(ptCfg->u8Idx==0)
    {
        if(eGopId==E_HAL_GOP_ID_5) {
            if(HAL_GOP_IS_LDC(eGopId))
            {
                HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_10, GOP_CLK_CTRL_LDC,GOP_CLK_CTRL_MSK,eCmdqId); //clk src
            }
            else
            {
                HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_10, GOP_CLK_CTRL_SCX,GOP_CLK_CTRL_MSK,eCmdqId); //clk src
            }
        }
    }
    //set RIU mode
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_05, GOP_PALETTE_SRAM_CTRL_RIU, GOP_PALETTE_SRAM_CTRL_MASK,eCmdqId);
    //set palette value
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_03, ptCfg->u8G << 8 | ptCfg->u8B,0xFFFF,eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_04, ptCfg->u8A << 8 | ptCfg->u8R,0xFFFF,eCmdqId);
    //set palette index
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_05, ptCfg->u8Idx, GOP_PALETTE_TABLE_ADDRESS_MASK,eCmdqId);
    if(!ptCfg->u8Idx)
    {
        HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);
    }
    //write trigger
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_05, 0x0100, GOP_PALETTE_WRITE_ENABLE_MASK,eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_05, 0x0000, GOP_PALETTE_WRITE_ENABLE_MASK,eCmdqId);
    //close force write
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_7F, 0x0000, GOP_BANK_FORCE_WR,eCmdqId);
}
bool HalGopCheckAlphaZeroOpaque(HalGopIdType_e eGopId,bool bEn,bool bConAlpha,HalGopGwinSrcFormat_e eFmt)
{
    if(HAL_GOP_IS_DISP_UI(eGopId))
    {
        if((eFmt==E_HAL_GOP_GWIN_SRC_ARGB1555) && (!bConAlpha))
        {
            return bEn;
        }
        else
        {
            return !bEn;
        }
    }
    else
    {
        return bEn;
    }
}
void HalGopSetAlphaInvert(HalGopIdType_e eGopId, bool bInv)
{
    MS_U32 GOP_Reg_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    HALRGNDBG("[GOP]%s %d\n", __FUNCTION__, __LINE__);
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);
    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00, bInv?GOP_ALPHA_INV:0, GOP_ALPHA_INV, eCmdqId);
    //write gop register
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopBindOsd(HalGopIdType_e eGopId)
{
    MS_U32 GOP_Reg_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;
    MS_U16 u16Mode;
    HalGopOutFormatType_e outFmt;
    GOP_Reg_Base = _HalGopGetBaseAddr(eGopId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);

    u16Mode = _HalGopGetBindMode(eGopId, &outFmt);

    HalRgnWrite2ByteMsk(GOP_Reg_Base + REG_GOP_00,
                        u16Mode,
                        GOP_VS_INV|GOP_HS_MASK|GOP_DISPLAY_MODE|GOP_OUTPUT_FORMAT,eCmdqId);

    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopEnsureIdle(HalGopIdType_e eGopId)
{
}
void HalGopSetEnableGwin(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, bool bEn)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;

    HALRGNDBG("[GOP]%s %d: Set GopId=%d GwinId=%d enable_GOP = [%d]\n", __FUNCTION__, __LINE__, eGopId, eGwinId, bEn);
    GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
    GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
    _HalGopIdTransCmdqID(eGopId,&eCmdqId);

    HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_00, (bEn ? GOP_GWIN_ENABLE : 0),GOP_GWIN_ENABLE, eCmdqId);

    //write gop register
    _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
}
void HalGopSetArgb1555Alpha(HalGopIdType_e eGopId, HalGopGwinIdType_e eGwinId, HalGopGwinArgb1555Def_e eAlphaType, u8 u8AlphaVal)
{
    MS_U32 GOP_Reg_Base = 0;
    MS_U32 GWIN_Reg_Shift_Base = 0;
    HalRgnCmdqIdType_e eCmdqId;

    if(!HAL_GOP_IS_DISP_UI(eGopId))
    {
        HALRGNDBG("[GOP]%s %d: Set GopId=%d GwinId=%d, argb1555 alpha type=%d, val=%d\n", __FUNCTION__, __LINE__, eGopId, eGwinId, eAlphaType, u8AlphaVal);
        GOP_Reg_Base = _HalGopGetGwinBaseAddr(eGopId,eGwinId);
        GWIN_Reg_Shift_Base = _HalGopGwinRegBaseShift(eGwinId);
        _HalGopIdTransCmdqID(eGopId,&eCmdqId);
        // enable/disable gwin
        if(eAlphaType==E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA0)
        {
            HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base + REG_GOP_08, (u8AlphaVal),GOP_GWIN_ARGB1555_ALPHA0_DEF_MSK, eCmdqId);
        }
        else if(eAlphaType==E_HAL_GOP_GWIN_ARGB1555_DEFINE_ALPHA1)
        {
            HalRgnWrite2ByteMsk(GOP_Reg_Base + GWIN_Reg_Shift_Base  + REG_GOP_09, (u8AlphaVal), GOP_GWIN_ARGB1555_ALPHA1_DEF_MSK,eCmdqId);
        }
        //write gop register
        _HalGopWriteDoubleBuffer(GOP_Reg_Base,eGopId,eCmdqId);
    }
    else
    {
        //HALRGNDBGERR("[GOP]%s %d: GOPId=0x%x not support\n", __FUNCTION__, __LINE__,eGopId);
    }
}
void HalGopInit(void)
{
    memset(gGoppfForSet, 0, sizeof(gGoppfForSet));
}
bool HalGopQuery(void *pQF)
{
    bool bRet = 1;
    HalGopQueryFuncConfig_t *pQFunc;

    pQFunc = pQF;
    if(pQFunc->stInQ.enQF<E_HAL_QUERY_GOP_MAX)
    {
        pQFunc->stOutQ.pfForSet = gGoppfForSet[pQFunc->stInQ.enQF];
        pQFunc->stOutQ.pfForPatch = NULL;
    }
    if(pQFunc->stOutQ.pfForSet)
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_GOP_OK;
    }
    else
    {
        pQFunc->stOutQ.enQueryRet = E_HAL_QUERY_GOP_NOTSUPPORT;
    }

    return bRet;
}
