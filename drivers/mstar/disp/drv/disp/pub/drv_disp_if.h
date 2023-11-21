/*
* drv_disp_if.h- Sigmastar
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

#ifndef _DRV_DISP_IF_H_
#define _DRV_DISP_IF_H_

#include "mhal_disp_datatype.h"

#ifdef _DRV_DISP_IF_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif
INTERFACE u8 DrvDispIfInitPanelConfig(MHAL_DISP_PanelConfig_t *pstPanelConfig, u8 u8Size);
INTERFACE u8 DrvDispIfDeviceCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DeviceId, void **pDevCtx);
INTERFACE u8 DrvDispIfDeviceDestroyInstance(void *pDevCtx);
INTERFACE u8 DrvDispIfDeviceEnable(void *pDevCtx, u8 bEnable);
INTERFACE u8 DrvDispIfDeviceSetBackGroundColor(void *pDevCtx, u32 u32BgColor);
INTERFACE u8 DrvDispIfDeviceAddOutInterface(void *pDevCtx, u32 u32Interface);
INTERFACE u8 DrvDispIfDeviceSetOutputTiming(void *pDevCtx, u32 u32Interface,
                                            MHAL_DISP_DeviceTimingInfo_t *pstTimingInfo);
INTERFACE u8 DrvDispIfDeviceSetColortemp(void *pDevCtx, MHAL_DISP_ColorTempeture_t *pstcolorTemp);
INTERFACE u8 DrvDispIfDeviceSetCvbsParam(void *pDevCtx, MHAL_DISP_CvbsParam_t *pstCvbsInfo);
INTERFACE u8 DrvDispIfDeviceGetCvbsParam(void *pDevCtx, MHAL_DISP_CvbsParam_t *pstCvbsInfo);
INTERFACE u8 DrvDispIfDeviceSetHdmiParam(void *pDevCtx, MHAL_DISP_HdmiParam_t *pstHdmiInfo);
INTERFACE u8 DrvDispIfDeviceGetHdmiParam(void *pDevCtx, MHAL_DISP_HdmiParam_t *pstHdmiInfo);
INTERFACE u8 DrvDispIfDeviceSetVgaParam(void *pDevCtx, MHAL_DISP_VgaParam_t *pstVgaInfo);
INTERFACE u8 DrvDispIfDeviceGetVgaParam(void *pDevCtx, MHAL_DISP_VgaParam_t *pstVgaInfo);
INTERFACE u8 DrvDispIfDeviceSetGammaParam(void *pDevCtx, MHAL_DISP_GammaParam_t *pstGammaInfo);
INTERFACE u8 DrvDispIfDeviceSetLcdParam(void *pDevCtx, MHAL_DISP_LcdParam_t *pstLcdInfo);
INTERFACE u8 DrvDispIfDeviceGetLcdParam(void *pDevCtx, MHAL_DISP_LcdParam_t *pstLcdInfo);
INTERFACE u8 DrvDispIfDeviceAttach(void *pSrcDevCtx, void *pDstDevCtx);
INTERFACE u8 DrvDispIfDeviceDetach(void *pSrcDevCtx, void *pDstDevCtx);
INTERFACE u8 DrvDispIfDeviceGetTimeZone(void *pDevCtx, MHAL_DISP_TimeZone_t *pstTimeZone);
INTERFACE u8 DrvDispIfDeviceSetTimeZoneConfig(void *pDevCtx, MHAL_DISP_TimeZoneConfig_t *pstTimeZoneCfg);
INTERFACE u8 DrvDispIfDeviceGetInstance(u32 u32DeviceId, void **pDevCtx);
INTERFACE u8 DrvDispIfDeviceGetDisplayInfo(void *pDevCtx, MHAL_DISP_DisplayInfo_t *pstDisplayInfo);
INTERFACE u8 DrvDispIfDeviceSetPqConfig(void *pDevCtx, MHAL_DISP_PqConfig_t *pstPqCfg);
INTERFACE u8 DrvDispIfDeviceGetHwCount(u32 *pu32Count);
INTERFACE u8 DrvDispIfSetDeviceConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg);
INTERFACE u8 DrvDispIfGetDeviceConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg);
INTERFACE u8 DrvDispIfDeviceGetCapabilityConfig(u32 u32DevId, MHAL_DISP_DeviceCapabilityConfig_t *pstDevCapCfg);
INTERFACE u8 DrvDispIfDeviceGetInterfaceCapabilityConfig(u32                                    u32Interface,
                                                         MHAL_DISP_InterfaceCapabilityConfig_t *pstInterfaceCapCfg);
INTERFACE u8 DrvDispIfClkOn(void);
INTERFACE u8 DrvDispIfClkOff(void);
INTERFACE u8 DrvDispIfGetClk(void *pDevCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
INTERFACE u8 DrvDispIfSetClk(void *pDevCtx, u8 *pbEn, u32 *pu32ClkRate, u32 u32ClkNum);
INTERFACE u8 DrvDispIfSetRegAccessConfig(void *pDevCtx, MHAL_DISP_RegAccessConfig_t *pstRegAccessCfg);
INTERFACE u8 DrvDispIfSetRegFlipConfig(void *pDevCtx, MHAL_DISP_RegFlipConfig_t *pstRegFlipCfg);
INTERFACE u8 DrvDispIfSetRegWaitDoneConfig(void *pDevCtx, MHAL_DISP_RegWaitDoneConfig_t *pstRegWaitDoneCfg);
INTERFACE u8 DrvDispIfSetCmdqInterfaceConfig(u32 u32DevId, MHAL_DISP_CmdqInfConfig_t *pstCmdqInfCfg);
INTERFACE u8 DrvDispIfVideoLayerCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32LayerId, void **pVidLayerCtx);
INTERFACE u8 DrvDispIfVideoLayerDestoryInstance(void *pVidLayerCtx);
INTERFACE u8 DrvDispIfVideoLayerEnable(void *pVidLayerCtx, u8 bEnable);
INTERFACE u8 DrvDispIfVideoLayerBind(void *pVidLayerCtx, void *pDevCtx);
INTERFACE u8 DrvDispIfVideoLayerUnBind(void *pVidLayerCtx, void *pDevCtx);
INTERFACE u8 DrvDispIfVideoLayerSetAttr(void *pVidLayerCtx, MHAL_DISP_VideoLayerAttr_t *pstAttr);
INTERFACE u8 DrvDispIfVideoLayerBufferFire(void *pVidLayerCtx);
INTERFACE u8 DrvDispIfVideoLayerCheckBufferFired(void *pVidLayerCtx);
INTERFACE u8 DrvDispIfVideoLayerSetCompress(void *pVidLayerCtx, MHAL_DISP_CompressAttr_t *pstCompressAttr);
INTERFACE u8 DrvDispIfVideoLayerSetPriority(void *pVidLayerCtx, u32 u32Priority);
INTERFACE u8 DrvDispIfVideoLayerGetCapabilityConfig(u32 u32LayerId,
                                                    MHAL_DISP_VideoLayerCapabilityConfig_t *pstVidLayerCapCfg);
INTERFACE u8 DrvDispIfInputPortCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, void *pVidLayerCtx, u32 u32PortId,
                                              void **pCtx);
INTERFACE u8 DrvDispIfInputPortDestroyInstance(void *pInputPortCtx);
INTERFACE u8 DrvDispIfInputPortFlip(void *pInputPortCtx, MHAL_DISP_VideoFrameData_t *pstVideoFrameData);
INTERFACE u8 DrvDispIfInputPortEnable(void *pInputPortCtx, u8 bEnable);
INTERFACE u8 DrvDispIfInputPortSetAttr(void *pInputPortCtx, MHAL_DISP_InputPortAttr_t *pstAttr);
INTERFACE u8 DrvDispIfInputPortShow(void *pInputPortCtx);
INTERFACE u8 DrvDispIfInputPortHide(void *pInputPortCtx);
INTERFACE u8 DrvDispIfInputPortAttrBegin(void *pVidLayerCtx);
INTERFACE u8 DrvDispIfInputPortAttrEnd(void *pVidLayerCtx);
INTERFACE u8 DrvDispIfInputPortRotate(void *pInputPortCtx, MHAL_DISP_RotateConfig_t *pstRotateCfg);
INTERFACE u8 DrvDispIfInputPortSetCropAttr(void *pInputPortCtx, MHAL_DISP_VidWinRect_t *pstCropAttr);
INTERFACE u8 DrvDispIfInputPortSetRingBuffAttr(void *pInputPortCtx, MHAL_DISP_RingBufferAttr_t *pstRingBufAttr);
INTERFACE u8 DrvDispIfInputPortSetImiAddr(void *pInputPortCtx, ss_phys_addr_t paImiAddr);
INTERFACE u8 DrvDispIfSetDbgLevel(void *p);
INTERFACE u8 DrvDispIfDmaCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DmaId, void **pDmaCtx);
INTERFACE u8 DrvDispIfDmaDestoryInstance(void *pDmaCtx);
INTERFACE u8 DrvDispIfDmaBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg);
INTERFACE u8 DrvDispIfDmaUnBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg);
INTERFACE u8 DrvDispIfDmaSetAttr(void *pDmaCtx, MHAL_DISP_DmaAttrConfig_t *pstDmatAttrCfg);
INTERFACE u8 DrvDispIfDmaSetBufferAttr(void *pDmaCtx, MHAL_DISP_DmaBufferAttrConfig_t *pstDmaBufferAttrCfg);
INTERFACE u8 DrvDispIfDmaGetHwCount(u32 *pu32Count);
INTERFACE u8 DrvDispIfDmaGetCapabilityConfig(u32 u32DmaId, MHAL_DISP_DmaCapabiliytConfig_t *pstDmaCapCfg);

INTERFACE void DrvDispDebugInit(void);
INTERFACE u32 DrvDispDebugGetTurnDrvInfo(u8 *p8DstBuf);
INTERFACE void DrvDispDebugSetTurnDrv(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugGetPqBinName(u8 *p8DstBuf);
INTERFACE void DrvDispDebugSetPq(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugGetClk(u8 *p8DstBuf);
INTERFACE void DrvDispDebugSetClk(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugIntsShow(u8 *p8DstBuf);
INTERFACE void      DrvDispDebugIntsStore(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugFuncShow(u8 *p8DstBuf);
INTERFACE void DrvDispDebugSetFunc(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugCfgShow(u8 *p8DstBuf);
INTERFACE void DrvDispDebugCfgStore(DrvDispOsStrConfig_t *pstStringCfg);
INTERFACE u32 DrvDispDebugGetDbgmgFlag(u8 *DstBuf);
INTERFACE void DrvDispDebugSetDbgmgFlag(DrvDispOsStrConfig_t *pstStringCfg);

#undef INTERFACE
#endif
