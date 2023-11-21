/*
* hal_disp_dma.h- Sigmastar
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

#ifndef _HAL_DISP_DMA_H_
#define _HAL_DISP_DMA_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define HAL_DISP_DMA_SCALING_RATIO(in, out) (((((u32)out) * 4096) / ((u32)in)) + 1)

// REG_DIP_DMA_02_L
#define HAL_DISP_DMA_FRM_W_422_TO_420 (0x0008)
#define HAL_DISP_DMA_FRM_W_420_PLANER (0x0010)
#define HAL_DISP_DMA_FRM_W_422_SEP    (0x0020)
#define HAL_DISP_DMA_FRM_W_422_PLANER (0x0040)

#define HAL_DISP_DMA_FRM_W_422_PACKET_ON  (0x0080)
#define HAL_DISP_DMA_FRM_W_422_PACKET_OFF (0x0000)

#define HAL_DISP_DMA_FRM_W_32B     (0x0100)
#define HAL_DISP_DMA_FRM_W_32B_EXT (0x0200)

#define HAL_DISP_DMA_FRM_W_ARGB_MSK     (0x4000)
#define HAL_DISP_DMA_FRM_W_ARGB_MD_ARGB (0x4000)
#define HAL_DISP_DMA_FRM_W_ARGB_MD_YUV  (0x0000)

#define HAL_DISP_DMA_FRM_W_444_PACKET_MSK        (0x8000)
#define HAL_DISP_DMA_FRM_W_444_PACKET_MD_444     (0x8000)
#define HAL_DISP_DMA_FRM_W_444_PACKET_MD_422_420 (0x0000)

#define HAL_DISP_DMA_FRM_W_444_A_VAL_SHIFT   (0)
#define HAL_DISP_DMA_FRM_W_444_Y_G_LOC_SHIFT (8)
#define HAL_DISP_DMA_FRM_W_444_U_B_LOC_SHIFT (10)
#define HAL_DISP_DMA_FRM_W_444_V_R_LOC_SHIFT (12)

#define HAL_DISP_DMA_VEN_MODE_SEL_NO_HANDSHAKE (0x0000)
#define HAL_DISP_DMA_VEN_MODE_SEL_HANDSHAKE    (0x0001)
#define HAL_DISP_DMA_VEN_MODE_SEL_RING         (0x0002)

#define HAL_DISP_DMA_CVBS_DEST_IMI (0x0004)
#define HAL_DISP_DMA_CVBS_DEST_EMI (0x0000)

#define HAL_DISP_DMA_CVBS_MOP_MUX_PATH_0 (0x0000)
#define HAL_DISP_DMA_CVBS_MOP_MUX_PATH_1 (0x0002)

#define HAL_DISP_DMA_BUFFER_SHIFT_BIT (4)
#define HAL_DISP_DMA_IRQ_WRITEDONE    0x0004

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_DMA_RST_WOPATH_H,
    E_HAL_DISP_DMA_RST_WOPATH_L,
    E_HAL_DISP_DMA_RST_TYPE_NUM,
} HalDispDmaRstType_e;

//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------

#ifdef _HAL_DISP_DMA_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
INTERFACE void HalDispDmaEnsureIdle(void *pCtx, HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE u8   HalDispDmaCheckIdle(void *pCtx, HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE void HalDispDmaSetFrmWEn(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetFrmWTriggerModeEn(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetFrmWTriggerModeCfg(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetVsSel(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetRegMonitor(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetFrmWDbEn(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetFrmWTrigger(u8 bEn, void *pCtx);
INTERFACE void HalDispDmaSetFrmWCFilter(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWPixelFormat(u16 u16Val0, u16 u16Val1, void *pCtx);
INTERFACE void HalDispDmaSetFrmWVflip(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWHmirror(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWSize(u16 u16Width, u16 u16Height, void *pCtx);
INTERFACE void HalDispDmaSetFrmW444Packet(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWBaseAddr(u32 *pu32BaseAddr, u16 u16FrameIdx, void *pCtx);
INTERFACE void HalDispDmaSetFrmWReqTh(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWReqLen(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWReqThC(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWReqLenC(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWMaxLenghtR0(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWMaxLenghtR1(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWMaxLenghtAll(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWLineOffset(u8 bEn, u16 *pu16LineOffset, void *pCtx);
INTERFACE void HalDispDmaSetFrmWImiEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWIdxMd(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWIdxW(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFrmWImiStarAddr(u32 *pu32StartAddr, void *pCtx);
INTERFACE void HalDispDmaSetFrmWImiOffset(u32 *pu32Offset, void *pCtx);
INTERFACE void HalDispDmaSetVenModeSel(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetVenRingStarAddr(u32 *pu32StartAddr, void *pCtx);
INTERFACE void HalDispDmaSetVenRingEndAddr(u32 *pu32EndAddr, void *pCtx);
INTERFACE void HalDispDmaSetDdaHratio(u8 bEn, u16 u16Ratio, void *pCtx);
INTERFACE u16  HalDispDmaGetFrmVCnt(u32 u32DmaId, u8 u8bImmediate);
INTERFACE void HalDispDmaSetDdaVratio(u8 bEn, u16 u16Ratio, void *pCtx);
INTERFACE void HalDispDmaSetDdaInSize(u16 u16Hsize, u16 u16Vsize, void *pCtx);
INTERFACE void HalDispDmaSetDdaOutSize(u16 u16Hsize, u16 u16Vsize, void *pCtx);
INTERFACE void HalDispDmaSetCvbsRingEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCvbsMopMux(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCvbsDest(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCvbsOffsetEn(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCvbsImiStartAddr(u32 *pu32StartAddr, void *pCtx);
INTERFACE void HalDispDmaSetCvbsImiOffset(u32 *pu32Offset, void *pCtx);
INTERFACE void HalDispDmaSetCvbsSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx);
INTERFACE void HalDispDmaSetCvbsBufSize(u16 *pu16Hsize, u16 *pu16Vsize, void *pCtx);
INTERFACE void HalDispDmaSetVideoPathFpllDly(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetVideoPathFpllRefSel(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaHwInit(void *pCtx);
INTERFACE void HalDispDmaHwDeinit(void *pCtx);
INTERFACE void HalDispDmaSwRst(void *pCtx, HalDispDmaRstType_e enRst, HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE void HalDispDmaSetTgenExtFrmResetMode(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetFpll1SrcMux(u8 bEn, u16 u16Mux, void *pCtx);
INTERFACE void HalDispDmaSetDispToMiuMux(u16 u16Val, void *pCtx);
INTERFACE u32 HalDispDmaGetDispToMiuMuxDev(void *pCtx);
INTERFACE void HalDispDmaSetR2Y(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetHdmiSynthSet(u32 u32Val, void *pCtx);
INTERFACE u8   HalDispDmaCheckFrmWEn(void *pCtx);
INTERFACE void HalDispDmaSetCscY2RMatrix(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCscY2RMode(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetCscY2RPortEn(u16 u16Val, void *pCtx);

INTERFACE void HalDispDmaSetTvencSynthSet(u32 u32Val, void *pCtx);
INTERFACE void HalDispDmaSetTvencSynthSel(u16 u16Val, void *pCtx);
INTERFACE void HalDispDmaSetDacAffRst(void *pCtx);
INTERFACE void HalDispDmaOnOffCmdqIrqWriteDone(void *pCtx, u8 u8bEn);

INTERFACE void HalDispDmaClearIrqWriteDone(void *pCtx);
INTERFACE void HalDispDmaPollWriteDone(void *pCtx);
INTERFACE void HalDispDmaPollVWriteCnt(void *pCtx, u16 u16LineCnt);

#undef INTERFACE
#endif
