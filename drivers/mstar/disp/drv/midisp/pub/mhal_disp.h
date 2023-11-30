/*
* mhal_disp.h- Sigmastar
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

/**
 * \defgroup HAL_DISP_group  HAL_DISP driver
 * @{
 */
#ifndef __MHAL_DISP_H__
#define __MHAL_DISP_H__

#include "mhal_disp_datatype.h"
//=============================================================================
// API
//=============================================================================

#define DISP_ENABLE_CUS_ALLOCATOR 0

#ifndef __MHAL_DISP_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

// Driver Physical memory: MI

//----------------------------------- Device -----------------------------------------------

/// @brief Get device IRQ number
/// @param[in] pDevCtx: Context of created device
/// @param[out] pu32LcdIrq: IRQ number
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetDevIrq(void *pDevCtx, u32 *pu32DevIrq);

/// @brief Enalbe / Disabe device IRQ
/// @param[in] pDevCtx: Context of created device
/// @param[in] u32DevIrq: IRQ number
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_EnableDevIrq(void *pDevCtx, u32 u32DevIrq, u8 bEnable);

/// @brief Clear device Irq
/// @param[in] pDevCtx: Context of created device
/// @param[in] pData: Irq flag to clear
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_ClearDevInterrupt(void *pDevCtx, void *pData);

/// @brief Get device Irq status
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstIrqFlag: Current Irq status
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetDevIrqFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);

/// @brief Get device IRQ number with specific deivce ID
/// @param[in] u32DevId: Device ID
/// @param[out] pu32DevIrq: the IRQ number of specific device ID
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetDevIrqById(u32 u32DevId, u32 *pu32DevIrq);

/// @brief Get LCD IRQ number
/// @param[in] pDevCtx: Context of created device
/// @param[out] pu32LcdIrq: IRQ number
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetLcdIrq(void *pDevCtx, u32 *pu32LcdIrq);

/// @brief Enalbe / Disabe LCD IRQ
/// @param[in] pDevCtx: Context of created device
/// @param[in] u32DevIrq: Irq number
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_EnableLcdIrq(void *pDevCtx, u32 u32DevIrq, u8 bEnable);

/// @brief Clear LCD Irq
/// @param[in] pDevCtx: Context of created device
/// @param[in] pData: Irq flag to clear
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_ClearLcdInterrupt(void *pDevCtx, void *pData);

/// @brief Get LCD Irq status
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstIrqFlag: Current Irq status
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetLcdIrqFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);

/// @brief Initl timing configuration
/// @param[in] pstPanelConfig: Data of timing configuration
/// @param[in] u8Size: Size of data
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InitPanelConfig(MHAL_DISP_PanelConfig_t *pstPanelConfig, u8 u8Size);

/// @brief Initl memory map configuration
/// @param[in] eMmType: memory map type
/// @param[in] pstMmapInfo: Configuraiton of memory map information
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InitMmapConfig(MHAL_DISP_MmapType_e eMemType, MHAL_DISP_MmapInfo_t *pstMmapInfo);

/// @brief Set register accessing mode
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstRegAccessCfg: the configuration of register accessing mode
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_SetRegAccessConfig(void *pDevCtx, MHAL_DISP_RegAccessConfig_t *pstRegAccessCfg);

/// @brief Apply register setting
/// @param[in] pDevCtx: Context of created device / DMA
/// @param[in] pstRegFlipCfg: the configuration of register flip
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_SetRegFlipConfig(void *pDevCtx, MHAL_DISP_RegFlipConfig_t *pstRegFlipCfg);

/// @brief Set register wait done
/// @param[in] pDevCtx: Context of created device / DMA
/// @param[in] pstRegWaitDoneCfg: the configuration of register wait done
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_SetRegWaitDoneConfig(void *pDevCtx, MHAL_DISP_RegWaitDoneConfig_t *pstRegWaitDoneCfg);

/// @brief Set CMDQ interface handler to device/DMA with specific device/DMA ID
/// @param[in] pDevCtx: Context of created device/DMA
/// @param[in] pstCmdqInfCfg: the configuration of CMDQ interface handler
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_SetCmdqInterface(u32 u32DevId, MHAL_DISP_CmdqInfConfig_t *pstCmdqInfCfg);

/// @brief Create instance of Disp Device
/// @param[in] pstAlloc: Configuration of physical memory allocate handler
/// @param[in] u32DeviceId: Device ID
/// @param[in] pDevCtx: Context of created instance
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, const u32 u32DeviceId,
                                            void **pDevCtx);

/// @brief Destroy instance of of Disp Device
/// @param[in] pDevCtx: Context of created device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceDestroyInstance(void *pDevCtx);

/// @brief Enable / Disable Device
/// @param[in] pDevCtx: Context of created device
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceEnable(void *pDevCtx, const u8 bEnable);

/// @brief Attach 2 Device
/// @param[in] pSrcDevCtx: Context of created source device
/// @param[in] pDstDevCtx: Context of created destination device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceAttach(const void *pSrcDevCtx, const void *pDstDevCtx);

/// @brief DeAttach 2 Device
/// @param[in] pSrcDevCtx: Context of created source device
/// @param[in] pDstDevCtx: Context of created destination device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceDetach(const void *pSrcDevCtx, const void *pDstDevCtx);

/// @brief Set back ground color of Device
/// @param[in] pDevCtx: Context of created device
/// @param[in] u32BgColor: R/Cr[7:0], G/Y[15:8], B/Cb[23:16]
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetBackGroundColor(void *pDevCtx, const u32 u32BgColor);

/// @brief Set output interface
/// @param[in] pDevCtx: Context of created device
/// @param[in] u32Interface: inteface ID
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceAddOutInterface(void *pDevCtx, const u32 u32Interface);

/// @brief Set device output timing
/// @param[in] pDevCtx: Context of created device
/// @param[in] u32Interface: inteface ID
/// @param[in] pstTimingInfo: The configuration of timing information
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetOutputTiming(void *pDevCtx, const u32 u32Interface,
                                             const MHAL_DISP_DeviceTimingInfo_t *pstTimingInfo);

/// @brief Set VGA parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstVgaInfo: The configuration of VGA parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetVgaParam(void *pDevCtx, const MHAL_DISP_VgaParam_t *pstVgaInfo);

/// @brief Get VGA parameter
/// @param[in] pDevCtx: Context of created device
/// @param[out] pstVgaInfo: The configuration of VGA parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetVgaParam(void *pDevCtx, const MHAL_DISP_VgaParam_t *pstVgaInfo);

/// @brief Set HDMI parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstHdmiInfo: The configuration of HDMI parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetHdmiParam(void *pDevCtx, const MHAL_DISP_HdmiParam_t *pstHdmiInfo);

/// @brief Get HDMI parameter
/// @param[in] pDevCtx: Context of created device
/// @param[out] pstHdmiInfo: The configuration of HDMI parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetHdmiParam(void *pDevCtx, const MHAL_DISP_HdmiParam_t *pstHdmiInfo);

/// @brief Set CVBS parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstCopstCvbsInfolorTempInfo: The configuration of CVBS parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetCvbsParam(void *pDevCtx, const MHAL_DISP_CvbsParam_t *pstCvbsInfo);
/// @brief Get CVBS parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstCopstCvbsInfolorTempInfo: The configuration of CVBS parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetCvbsParam(void *pDevCtx, const MHAL_DISP_CvbsParam_t *pstCvbsInfo);

/// @brief Set device color tempeture
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstColorTempInfo: The configuration of color tempeture information
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetColorTempeture(void *pDevCtx, const MHAL_DISP_ColorTempeture_t *pstColorTempInfo);

/// @brief Set LCD parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstLcdInfo: The configuration of LCD parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetLcdParam(void *pDevCtx, const MHAL_DISP_LcdParam_t *pstLcdInfo);

/// @brief Get LCD parameter
/// @param[in] pDevCtx: Context of created device
/// @param[out] pstLcdInfo: The configuration of LCD parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetLcdParam(void *pDevCtx, const MHAL_DISP_LcdParam_t *pstLcdInfo);

/// @brief Set Gamma  parameter
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstGammaInfo: The configuration of gamma parameter
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetGammaParam(void *pDevCtx, const MHAL_DISP_GammaParam_t *pstGammaInfo);

/// @brief Get current time zone area
/// @param[in] pDevCtx: Context of created device
/// @param[out] pstTimeZone: the current time zone area
/// @note: get current time is sync / backproch / acitve / front porch
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetTimeZone(void *pDevCtx, MHAL_DISP_TimeZone_t *pstTimeZone);

/// @brief Set configuration of timezone
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstTimeZoneCfg: The invalid area of time zone
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetTimeZoneConfig(void *pDevCtx, MHAL_DISP_TimeZoneConfig_t *pstTimeZoneCfg);

/// @brief Get current display timing inforamtion
/// @param[in] pDevCtx: Context of created device
/// @param[out] pstDisplayInfo: The display timing inforamtion
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetDisplayInfo(void *pDevCtx, MHAL_DISP_DisplayInfo_t *pstDisplayInfo);

/// @brief Get current active deivce context by specific device ID
/// @param[in] u32DeviceId: Device ID
/// @param[out] pDevCtx: Current active deivce context
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetInstance(u32 u32DeviceId, void **pDevCtx);

/// @brief Set PQ configuration
/// @param[in] pDevCtx: Context of created device
/// @param[in] pstPqCfg: The configuration of PQ
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetPqConfig(void *pDevCtx, MHAL_DISP_PqConfig_t *pstPqCfg);

/// @brief Set device configuration with specific device ID
/// @param[in] u32DevId: Device ID
/// @param[in] pstDevCfg: The setting of device configuration
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceSetConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg);

/// @brief Get device configuration with specific device ID
/// @param[in] u32DevId: Device ID
/// @param[out] pstDevCfg: The setting of device configuration
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg);

/// @brief Get device capability with specific device ID
/// @param[in] u32DevId: Device ID
/// @param[out] pstDevCapCfg: The setting of device capability
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetCapabilityConfig(u32 u32DevId, MHAL_DISP_DeviceCapabilityConfig_t *pstDevCapCfg);

/// @brief Get interface capability with specific interface ID
/// @param[in] u32Interface: interface ID
/// @param[out] pstInterfaceCapCfg: The setting of interface capability
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetIntefaceCapabilityConfig(u32                                    u32Interface,
                                                         MHAL_DISP_InterfaceCapabilityConfig_t *pstInterfaceCapCfg);

/// @brief Create instance of Disp DMA
/// @param[in] pstAlloc: Configuration of physical memory allocate handler
/// @param[in] u32DmaId: dma ID
/// @param[out] pDmaCtx: Context of created instance
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DmaId, void **pDmaCtx);

/// @brief Destroy instance of of Disp dma
/// @param[in] pDmaCtx: Context of created DMA
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaDestoryInstance(void *pDmaCtx);

/// @brief Bind source / destination device context to DMA
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pstDmaBindCfg: THe configuraiton of DMA binding
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg);

/// @brief Unbind source / destination device context to DMA
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pstDmaBindCfg: THe configuraiton of DMA binding
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaUnBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg);

/// @brief Set attribute of DMA
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pstDmaAttrCfg: THe configuraiton of DMA attribute
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaSetAttr(void *pDmaCtx, MHAL_DISP_DmaAttrConfig_t *pstDmaAttrCfg);

/// @brief Set buffer attribute of DMA
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pstDmaBufferAttrCfg: THe configuraiton of DMA buffer attribute
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaSetBufferAttr(void *pDmaCtx, MHAL_DISP_DmaBufferAttrConfig_t *pstDmaBufferAttrCfg);

/// @brief Enalbe / Disabe DMA IRQ
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] u32DevIrq: Irq number
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_EnableDmaIrq(void *pDmaCtx, u32 u32DevIrq, u8 bEnable);

/// @brief Clear DMA Irq
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pData: Irq flag to clear
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_ClearDmaInterrupt(void *pDmaCtx, void *pData);

/// @brief Get DMA Irq status
/// @param[in] pDmaCtx: Context of created DMA
/// @param[in] pstIrqFlag: Current Irq status
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetDmaIrqFlag(void *pDmaCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag);

/// @brief Get DMA IRQ number
/// @param[in] pDmaCtx: Context of created DMA
/// @param[out] pu32DevIrq: IRQ number
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_GetDmaIrq(void *pDmaCtx, u32 *pu32DevIrq);

/// @brief Get DMA capability with specific DMA ID
/// @param[in] u32DmaId: DMA ID
/// @param[out] u32DmaId: The setting of DMA capability
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaGetCapabilityConfig(u32 u32DmaId, MHAL_DISP_DmaCapabiliytConfig_t *pstDmaCapCfg);

/// @brief Get DMA HW count
/// @param[out] pu32Count: count of DMA HW
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DmaGetHwCount(u32 *pu32Count);

/// @brief Get count of HW device
/// @param[out] pu32Count: count of HW device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DeviceGetHwCount(u32 *pu32Count);

//----------------------------------- Video Layer --------------------------------------------
/// @brief Create instance of Disp Videolayer
/// @param[in] pstAlloc: Configuration of physical memory allocate handler
/// @param[in] u32LayerId: Videolayer ID
/// @param[out] pVidLayerCtx: Context of created instance
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, const u32 u32LayerId,
                                                void **pVidLayerCtx);

/// @brief Destroy instance of of Disp Videolayer
/// @param[in] pVidLayerCtx: Context of created device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerDestoryInstance(void *pVidLayerCtx);

/// @brief Set videolayer on/off
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerEnable(void *pVidLayerCtx, const u8 bEnable);

/// @brief Bind videolyaer with specific device
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] pDevCtx: Context of created device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerBind(void *pVidLayerCtx, void *pDevCtx);

/// @brief Unbind videolyaer frin specific device
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] pDevCtx: Context of created device
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerUnBind(void *pVidLayerCtx, void *pDevCtx);

/// @brief Set attribute of videolayer
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] pstAttr: Configuration of videolayer attribute
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerSetAttr(void *pVidLayerCtx, const MHAL_DISP_VideoLayerAttr_t *pstAttr);

/// @brief Set compress setting of videolayer
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] pstCompressAttr: Attribute of compress
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerSetCompress(void *pVidLayerCtx, const MHAL_DISP_CompressAttr_t *pstCompressAttr);

/// @brief Set priority of videolayer
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @param[in] u32Priority: Level of priority
/// @note Not Support
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerSetPriority(void *pVidLayerCtx, const u32 u32Priority);

/// @brief Trigger the setting of videolayer
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @note Not Support
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerBufferFire(void *pVidLayerCtx);

/// @brief Check the action of MHAL_DISP_VideoLayerBufferFire is done
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @note Not Support
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerCheckBufferFired(void *pVidLayerCtx);

/// @brief Get videolayer capability with specific videolayer ID
/// @param[in] eVidLayerType: videolayer ID
/// @param[out] pstVidLayerCapCfg: The setting of videolayer capability
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_VideoLayerGetCapabilityConfig(u32 u32LayerId,
                                                     MHAL_DISP_VideoLayerCapabilityConfig_t *pstVidLayerCapCfg);

//----------------------------------- Input Port ---------------------------------------------
/// @brief Create instance of Disp Inputport
/// @param[in] pstAlloc: Configuration of physical memory allocate handler
/// @param[in] pVidLayerCtx: Context of videolayer which the parent of specific inputport
/// @param[in] u32PortId: InputPort ID
/// @param[out] pVidLapCtxyerCtx: Context of created instance
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, void *pVidLayerCtx,
                                               const u32 u32PortId, void **pCtx);

/// @brief Destroy instance of of Disp inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortDestroyInstance(const void *pCtx);

/// @brief Set flip inputport on/off
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] bEnable: 1: enable, 0: disable
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortEnable(void *pInputPortCtx, const u8 bEnable);

/// @brief Set attribute of inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] pstAttr: The setting of inputport attribute
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortSetAttr(void *pInputPortCtx, const MHAL_DISP_InputPortAttr_t *pstAttr);

/// @brief Set inputport be showed
/// @param[in] pInputPortCtx: Context of created inputport
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortShow(void *pInputPortCtx);

/// @brief Set inputport be hidden
/// @param[in] pInputPortCtx: Context of created inputport
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortHide(void *pInputPortCtx);

/// @brief Set inputport at starting stage of attribue updating
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortAttrBegin(void *pVidLayerCtx);

/// @brief Set inputport at endign stage of attribue updating
/// @param[in] pVidLayerCtx: Context of created videolayer
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortAttrEnd(void *pVidLayerCtx);

/// @brief Set flip configuration of inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] pstVideoFrameBuffer: The configuration of video frame data
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortFlip(void *pInputPortCtx, MHAL_DISP_VideoFrameData_t *pstVideoFrameBuffer);

/// @brief Set rotation configuration of inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] pstRotateCfg: The setting of inputport rotation
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortRotate(void *pInputPortCtx, MHAL_DISP_RotateConfig_t *pstRotateCfg);

/// @brief Set crop configuration of inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] pstWinRect: The crop window setting
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortSetCropAttr(void *pInputPortCtx, MHAL_DISP_VidWinRect_t *pstWinRect);

/// @brief Set ring buffer configuration of inputport
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] pstRingBufAttr: The attribute of ring buffer
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortSetRingBuffAttr(void *pInputPortCtx, MHAL_DISP_RingBufferAttr_t *pstRingBufAttr);

/// @brief Set imi address of rotation buffer
/// @param[in] pInputPortCtx: Context of created inputport
/// @param[in] paImiAddr: The phyasical address of imi
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_InputPortSetImiAddr(void *pInputPortCtx, const ss_phys_addr_t paImiAddr);

/// @brief Set debug level
/// @param[in] p: The vakye of debug level
/// @return retval = 1 success,  retval = 0 failure
INTERFACE u8 MHAL_DISP_DbgLevel(void *p);

#undef INTERFACE
#endif    //
/** @} */ // end of HAL_DISP_group
