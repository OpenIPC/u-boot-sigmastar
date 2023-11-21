/*
* mhal_disp.c- Sigmastar
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

#define __MHAL_DISP_C__

#include "drv_disp_os.h"
#include "mhal_common.h"
#include "mhal_cmdq.h"
#include "mhal_disp_datatype.h"
#include "mhal_disp.h"

#include "disp_debug.h"
#include "drv_disp_if.h"
#include "drv_disp_irq.h"
#include "hal_disp_chip.h"

u8 MHAL_DISP_InitPanelConfig(MHAL_DISP_PanelConfig_t *pstPanelConfig, u8 u8Size)
{
    u8 bRet = TRUE;
    if (pstPanelConfig == NULL || u8Size == 0)
    {
        DISP_ERR("%s %d, pstPanelConfig or u8Size is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInitPanelConfig(pstPanelConfig, u8Size) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Init PanelConfig Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InitMmapConfig(MHAL_DISP_MmapType_e eMmType, MHAL_DISP_MmapInfo_t *pstMmapInfo)
{
    u8 bRet = TRUE;
    return bRet;
}

u8 MHAL_DISP_DeviceCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, const u32 u32DeviceId, void **pDevCtx)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DeviceId);
    
    if (pstAlloc == NULL)
    {
        DISP_ERR("%s %d, pstAlloc is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DeviceId);
        }
        else if (DrvDispIfDeviceCreateInstance((MHAL_DISP_AllocPhyMem_t *)pstAlloc, u32DevNewId, pDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceDestroyInstance(void *pDevCtx)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceDestroyInstance(pDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, DestroyInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceEnable(void *pDevCtx, const u8 bEnable)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceEnable(pDevCtx, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, DeviceEnable Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetBackGroundColor(void *pDevCtx, const u32 u32BgColor)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetBackGroundColor(pDevCtx, u32BgColor) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, SetBackGroundColor Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceAddOutInterface(void *pDevCtx, const u32 u32Interface)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceAddOutInterface(pDevCtx, u32Interface) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, AddOutInterface Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetOutputTiming(void *pDevCtx, const u32 u32Interface,
                                   const MHAL_DISP_DeviceTimingInfo_t *pstTimingInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstTimingInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetOutputTiming(pDevCtx, u32Interface, (MHAL_DISP_DeviceTimingInfo_t *)pstTimingInfo)
            == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, SetOutputTiming Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetColorTempeture(void *pDevCtx, const MHAL_DISP_ColorTempeture_t *pstColorTempInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstColorTempInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstColorTempInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetColortemp(pDevCtx, (MHAL_DISP_ColorTempeture_t *)pstColorTempInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set ColorTemp Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}
u8 MHAL_DISP_DeviceGetCvbsParam(void *pDevCtx, const MHAL_DISP_CvbsParam_t *pstCvbsInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstCvbsInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstCvbsInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetCvbsParam(pDevCtx, (MHAL_DISP_CvbsParam_t *)pstCvbsInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set CvbsParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}
u8 MHAL_DISP_DeviceSetCvbsParam(void *pDevCtx, const MHAL_DISP_CvbsParam_t *pstCvbsInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstCvbsInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstCvbsInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetCvbsParam(pDevCtx, (MHAL_DISP_CvbsParam_t *)pstCvbsInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set CvbsParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetHdmiParam(void *pDevCtx, const MHAL_DISP_HdmiParam_t *pstHdmiInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstHdmiInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstHdmiInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetHdmiParam(pDevCtx, (MHAL_DISP_HdmiParam_t *)pstHdmiInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set HdmiParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetHdmiParam(void *pDevCtx, const MHAL_DISP_HdmiParam_t *pstHdmiInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstHdmiInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstHdmiInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetHdmiParam(pDevCtx, (MHAL_DISP_HdmiParam_t *)pstHdmiInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Get HdmiParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetLcdParam(void *pDevCtx, const MHAL_DISP_LcdParam_t *pstLcdInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstLcdInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstLcdInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetLcdParam(pDevCtx, (MHAL_DISP_LcdParam_t *)pstLcdInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set LcdParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetLcdParam(void *pDevCtx, const MHAL_DISP_LcdParam_t *pstLcdInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstLcdInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstLcdInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetLcdParam(pDevCtx, (MHAL_DISP_LcdParam_t *)pstLcdInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Get LcdParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetGammaParam(void *pDevCtx, const MHAL_DISP_GammaParam_t *pstGammaInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstGammaInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstGammaInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetGammaParam(pDevCtx, (MHAL_DISP_GammaParam_t *)pstGammaInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set GammaParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetVgaParam(void *pDevCtx, const MHAL_DISP_VgaParam_t *pstVgaInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstVgaInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstVgaInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetVgaParam(pDevCtx, (MHAL_DISP_VgaParam_t *)pstVgaInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set VgaParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetVgaParam(void *pDevCtx, const MHAL_DISP_VgaParam_t *pstVgaInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL || pstVgaInfo == NULL)
    {
        DISP_ERR("%s %d, pDevCtx  or pstVgaInfo is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetVgaParam(pDevCtx, (MHAL_DISP_VgaParam_t *)pstVgaInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Get VgaParam Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceAttach(const void *pSrcDevCtx, const void *pDstDevCtx)
{
    u8 bRet = TRUE;
    if (pSrcDevCtx == NULL || pDstDevCtx == NULL)
    {
        DISP_ERR("%s %d, pSrcDevCtx  or pDstDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceAttach((void *)pSrcDevCtx, (void *)pDstDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Device Attach Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceDetach(const void *pSrcDevCtx, const void *pDstDevCtx)
{
    u8 bRet = TRUE;
    if (pSrcDevCtx == NULL || pDstDevCtx == NULL)
    {
        DISP_ERR("%s %d, pSrcDevCtx  or pDstDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceDetach((void *)pSrcDevCtx, (void *)pDstDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Get Device Detach Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetTimeZoneConfig(void *pDevCtx, MHAL_DISP_TimeZoneConfig_t *pstTimeZoneCfg)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceSetTimeZoneConfig(pDevCtx, pstTimeZoneCfg) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set Device TimeZone Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetTimeZone(void *pDevCtx, MHAL_DISP_TimeZone_t *pstTimeZone)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetTimeZone(pDevCtx, pstTimeZone) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Device TimeZone Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetDisplayInfo(void *pDevCtx, MHAL_DISP_DisplayInfo_t *pstDisplayInfo)
{
    u8 bRet = TRUE;
    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfDeviceGetDisplayInfo(pDevCtx, pstDisplayInfo) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Device DisplayInfo Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetInstance(u32 u32DeviceId, void **pDevCtx)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DeviceId);
    
    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DeviceId);
    }
    else if (DrvDispIfDeviceGetInstance(u32DevNewId, pDevCtx) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device DisplayInfo Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetPqConfig(void *pDevCtx, MHAL_DISP_PqConfig_t *pstPqCfg)
{
    u8 bRet = TRUE;

    if (DrvDispIfDeviceSetPqConfig(pDevCtx, pstPqCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device DisplayInfo Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetHwCount(u32 *pu32Count)
{
    u8 bRet = TRUE;

    if (DrvDispIfDeviceGetHwCount(pu32Count) == FALSE)
    {
        DISP_ERR("%s %d, Get Hw Count Fail\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_DeviceSetConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DevId);

    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DevId);
    }
    else if (DrvDispIfSetDeviceConfig(u32DevNewId, pstDevCfg) == FALSE)
    {
        DISP_ERR("%s %d, Set Config\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetConfig(u32 u32DevId, MHAL_DISP_DeviceConfig_t *pstDevCfg)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DevId);

    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DevId);
    }
    else if (DrvDispIfGetDeviceConfig(u32DevNewId, pstDevCfg) == FALSE)
    {
        DISP_ERR("%s %d, Get Config\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetCapabilityConfig(u32 u32DevId, MHAL_DISP_DeviceCapabilityConfig_t *pstDevCapCfg)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DevId);

    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DevId);
    }
    else if (DrvDispIfDeviceGetCapabilityConfig(u32DevNewId, pstDevCapCfg) == FALSE)
    {
        DISP_ERR("%s %d, Get Dev CapabilityConfig\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_DeviceGetIntefaceCapabilityConfig(u32                                    u32Interface,
                                               MHAL_DISP_InterfaceCapabilityConfig_t *pstInterfaceCapCfg)
{
    u8 bRet = TRUE;

    if (DrvDispIfDeviceGetInterfaceCapabilityConfig(u32Interface, pstInterfaceCapCfg) == FALSE)
    {
        DISP_ERR("%s %d, Get Interface CapabilityConfig\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, const u32 u32LayerId,
                                      void **pVidLayerCtx)
{
    u8 bRet = TRUE;
    u32 u32NewLayerId = HAL_DISP_MAPPING_VIDLAYERID_FROM_MI(u32LayerId);
    
    if (pstAlloc == NULL)
    {
        DISP_ERR("%s %d, pstAlloc is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if(u32NewLayerId >= HAL_DISP_VIDLAYER_MAX)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Layer %d Fail\n", __FUNCTION__, __LINE__,u32LayerId);
        }
        else if (DrvDispIfVideoLayerCreateInstance((MHAL_DISP_AllocPhyMem_t *)pstAlloc, 
        u32NewLayerId, pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerDestoryInstance(void *pVidLayerCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerDestoryInstance(pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, DestoryInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerEnable(void *pVidLayerCtx, const u8 bEnable)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerEnable(pVidLayerCtx, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLayerEnable Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerBind(void *pVidLayerCtx, void *pDevCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL || pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx or PDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerBind(pVidLayerCtx, pDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer Bind Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerUnBind(void *pVidLayerCtx, void *pDevCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL || pDevCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx or PDevCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerUnBind(pVidLayerCtx, pDevCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer UnBind Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerSetAttr(void *pVidLayerCtx, const MHAL_DISP_VideoLayerAttr_t *pstAttr)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL || pstAttr == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx or pstAttr is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerSetAttr(pVidLayerCtx, (MHAL_DISP_VideoLayerAttr_t *)pstAttr) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer SetAttr Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerBufferFire(void *pVidLayerCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerBufferFire(pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer BufferFire Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerCheckBufferFired(void *pVidLayerCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerCheckBufferFired(pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer Check BufferFire Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerSetCompress(void *pVidLayerCtx, const MHAL_DISP_CompressAttr_t *pstCompressAttr)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL || pstCompressAttr == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx or pstCompressAttr is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerSetCompress(pVidLayerCtx, (MHAL_DISP_CompressAttr_t *)pstCompressAttr) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer SetComporess Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerSetPriority(void *pVidLayerCtx, const u32 u32Priority)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfVideoLayerSetPriority(pVidLayerCtx, u32Priority) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, VideoLyaer SetPriority Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_VideoLayerGetCapabilityConfig(u32 u32LayerId, MHAL_DISP_VideoLayerCapabilityConfig_t *pstVidLayerCapCfg)
{
    u8 bRet = TRUE;
    u32 u32NewLayerId = HAL_DISP_MAPPING_VIDLAYERID_FROM_MI(u32LayerId);
    
    if(u32NewLayerId >= HAL_DISP_VIDLAYER_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Layer %d Fail\n", __FUNCTION__, __LINE__,u32LayerId);
    }
    else if (DrvDispIfVideoLayerGetCapabilityConfig(u32NewLayerId,pstVidLayerCapCfg) == FALSE)
    {
        DISP_ERR("%s %d, Get Dev CapabilityConfig\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_InputPortCreateInstance(const MHAL_DISP_AllocPhyMem_t *pstAlloc, void *pVidLayerCtx, const u32 u32PortId,
                                     void **pCtx)
{
    u8 bRet = TRUE;
    if (pstAlloc == NULL || pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pstAlloc or pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortCreateInstance((MHAL_DISP_AllocPhyMem_t *)pstAlloc, pVidLayerCtx, u32PortId, pCtx)
            == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortDestroyInstance(const void *pInputPortCtx)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortDestroyInstance((void *)pInputPortCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, DestoryInstance Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortFlip(void *pInputPortCtx, MHAL_DISP_VideoFrameData_t *pstVideoFrameBuffer)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL || pstVideoFrameBuffer == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx or pstVideoFrameBuffer is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortFlip(pInputPortCtx, pstVideoFrameBuffer) == FALSE)
        {
            bRet = FALSE;
            DISP_DBG(DISP_DBG_LEVEL_DRV, "%s %d, InputPort Flip Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortEnable(void *pInputPortCtx, const u8 bEnable)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortEnable(pInputPortCtx, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Enable Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortSetAttr(void *pInputPortCtx, const MHAL_DISP_InputPortAttr_t *pstAttr)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL || pstAttr == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx or pstAttr is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortSetAttr(pInputPortCtx, (MHAL_DISP_InputPortAttr_t *)pstAttr) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort SetAttr Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortShow(void *pInputPortCtx)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortShow(pInputPortCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Show Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortHide(void *pInputPortCtx)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortHide(pInputPortCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Hide Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortAttrBegin(void *pVidLayerCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortAttrBegin(pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort AttrBegin Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortAttrEnd(void *pVidLayerCtx)
{
    u8 bRet = TRUE;

    if (pVidLayerCtx == NULL)
    {
        DISP_ERR("%s %d, pVidLayerCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortAttrEnd(pVidLayerCtx) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort AttrEnd Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortRotate(void *pInputPortCtx, MHAL_DISP_RotateConfig_t *pstRotateCfg)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortRotate(pInputPortCtx, pstRotateCfg) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Rotate Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortSetCropAttr(void *pInputPortCtx, MHAL_DISP_VidWinRect_t *pstWinRect)

{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortSetCropAttr(pInputPortCtx, pstWinRect) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Rotate Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortSetRingBuffAttr(void *pInputPortCtx, MHAL_DISP_RingBufferAttr_t *pstRingBufAttr)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortSetRingBuffAttr(pInputPortCtx, pstRingBufAttr) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort RingBuffAttr Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_InputPortSetImiAddr(void *pInputPortCtx, const u64 paImiAddr)
{
    u8 bRet = TRUE;

    if (pInputPortCtx == NULL)
    {
        DISP_ERR("%s %d, pInputPortCtx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfInputPortSetImiAddr(pInputPortCtx, paImiAddr) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, InputPort Enable Fail\n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DmaCreateInstance(MHAL_DISP_AllocPhyMem_t *pstAlloc, u32 u32DmaId, void **pDmaCtx)
{
    u8 bRet = TRUE;

    if (DrvDispIfDmaCreateInstance(pstAlloc, u32DmaId, pDmaCtx) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, CreateInstance Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaDestoryInstance(void *pDmaCtx)
{
    u8 bRet = TRUE;
    if (DrvDispIfDmaDestoryInstance(pDmaCtx) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DestoryInstance Fail\n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfDmaBind(pDmaCtx, pstDmaBindCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device Bind Dma Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaUnBind(void *pDmaCtx, MHAL_DISP_DmaBindConfig_t *pstDmaBindCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfDmaUnBind(pDmaCtx, pstDmaBindCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device UnBind Dma Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaSetAttr(void *pDmaCtx, MHAL_DISP_DmaAttrConfig_t *pstDmaAttrCfg)
{
    u8 bRet = TRUE;

    if (DrvDispIfDmaSetAttr(pDmaCtx, pstDmaAttrCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device SetDmaAttr Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaSetBufferAttr(void *pDmaCtx, MHAL_DISP_DmaBufferAttrConfig_t *pstDmaBufferAttrCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfDmaSetBufferAttr(pDmaCtx, pstDmaBufferAttrCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device SetDmaBufferAttr Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_DmaGetHwCount(u32 *pu32Count)
{
    u8 bRet = TRUE;

    if (DrvDispIfDmaGetHwCount(pu32Count) == FALSE)
    {
        DISP_ERR("%s %d, Get Hw Count Fail\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_DmaGetCapabilityConfig(u32 u32DmaId, MHAL_DISP_DmaCapabiliytConfig_t *pstDmaCapCfg)
{
    u8 bRet = TRUE;

    if (DrvDispIfDmaGetCapabilityConfig(u32DmaId, pstDmaCapCfg) == FALSE)
    {
        DISP_ERR("%s %d, Get Dma CapabilityConfig\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    return bRet;
}

u8 MHAL_DISP_EnableDmaIrq(void *pDmaCtx, u32 u32DevIrq, u8 bEnable)
{
    u8 bRet = TRUE;

    if (pDmaCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqEnable(pDmaCtx, u32DevIrq, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set EnableIrq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_ClearDmaInterrupt(void *pDmaCtx, void *pData)
{
    u8 bRet = TRUE;

    if (pDmaCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqClear(pDmaCtx, pData) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_GetDmaIrqFlag(void *pDmaCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag)
{
    u8 bRet = TRUE;

    if (pDmaCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetFlag(pDmaCtx, pstIrqFlag) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_GetDmaIrq(void *pDmaCtx, u32 *pu32DevIrq)
{
    u8 bRet = TRUE;

    if (pDmaCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetIsrNum(pDmaCtx, pu32DevIrq) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_DbgLevel(void *p)
{
    u8 bRet = TRUE;

    if (p == NULL)
    {
        DISP_ERR("%s %d, p is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIfSetDbgLevel(p) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set DebugLevel Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_EnableLcdIrq(void *pDevCtx, u32 u32DevIrq, u8 bEnable)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqEnableLcd(pDevCtx, u32DevIrq, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set EnableIrq Fail \n", __FUNCTION__, __LINE__);
        }
    }

    return bRet;
}

u8 MHAL_DISP_ClearLcdInterrupt(void *pDevCtx, void *pData)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqClearLcd(pDevCtx, pData) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }

    return bRet;
}

u8 MHAL_DISP_GetLcdIrqFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetLcdFlag(pDevCtx, pstIrqFlag) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }

    return bRet;
}

u8 MHAL_DISP_GetLcdIrq(void *pDevCtx, u32 *pu32LcdIrq)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetLcdIsrNum(pDevCtx, pu32LcdIrq) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }

    return bRet;
}

u8 MHAL_DISP_EnableDevIrq(void *pDevCtx, u32 u32DevIrq, u8 bEnable)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqEnable(pDevCtx, u32DevIrq, bEnable) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Set EnableIrq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_ClearDevInterrupt(void *pDevCtx, void *pData)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqClear(pDevCtx, pData) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_GetDevIrqFlag(void *pDevCtx, MHAL_DISP_IRQFlag_t *pstIrqFlag)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetFlag(pDevCtx, pstIrqFlag) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_GetDevIrq(void *pDevCtx, u32 *pu32DevIrq)
{
    u8 bRet = TRUE;

    if (pDevCtx == NULL)
    {
        DISP_ERR("%s %d, Ctx is Null\n", __FUNCTION__, __LINE__);
        bRet = FALSE;
    }
    else
    {
        if (DrvDispIrqGetIsrNum(pDevCtx, pu32DevIrq) == FALSE)
        {
            bRet = FALSE;
            DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
        }
    }
    return bRet;
}

u8 MHAL_DISP_SetRegAccessConfig(void *pDevCtx, MHAL_DISP_RegAccessConfig_t *pstRegAccessCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfSetRegAccessConfig(pDevCtx, pstRegAccessCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device SetRegAccess Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_SetRegFlipConfig(void *pDevCtx, MHAL_DISP_RegFlipConfig_t *pstRegFlipCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfSetRegFlipConfig(pDevCtx, pstRegFlipCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device SetRegFlip Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_SetRegWaitDoneConfig(void *pDevCtx, MHAL_DISP_RegWaitDoneConfig_t *pstRegWaitDoneCfg)
{
    u8 bRet = TRUE;
    if (DrvDispIfSetRegWaitDoneConfig(pDevCtx, pstRegWaitDoneCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Device SetRegWaitDone Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_SetCmdqInterface(u32 u32DevId, MHAL_DISP_CmdqInfConfig_t *pstCmdqInfCfg)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DevId);
    
    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DevId);
    }
    else if (DrvDispIfSetCmdqInterfaceConfig(u32DevNewId, pstCmdqInfCfg) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, SetCmdqInf Fail \n", __FUNCTION__, __LINE__);
    }
    return bRet;
}

u8 MHAL_DISP_GetDevIrqById(u32 u32DevId, u32 *pu32DevIrq)
{
    u8 bRet = TRUE;
    u32 u32DevNewId = HAL_DISP_MAPPING_DEVICEID_FROM_MI(u32DevId);

    if(u32DevNewId >= HAL_DISP_DEVICE_MAX)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, DeviceId %d Fail\n", __FUNCTION__, __LINE__,u32DevId);
    }
    else if (DrvDispIrqGetIsrNumByDevId(u32DevNewId, pu32DevIrq) == FALSE)
    {
        bRet = FALSE;
        DISP_ERR("%s %d, Clear Irq Fail \n", __FUNCTION__, __LINE__);
    }

    return bRet;
}
