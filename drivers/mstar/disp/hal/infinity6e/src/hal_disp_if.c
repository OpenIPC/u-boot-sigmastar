/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

 Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
Sigmastar Technology Corp. and be kept in strict confidence
(Sigmastar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of Sigmastar Confidential
Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#define _HAL_DISP_IF_C_

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include <linux/string.h>

#include "drv_disp_os.h"

#include "hal_disp_common.h"
#include "disp_debug.h"
#include "hal_disp_chip.h"
#include "hal_disp_st.h"
#include "hal_disp.h"
#include "drv_disp_ctx.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    HalDispQueryRet_e (*pGetInfoFunc) (void *, void *);
    void (*pSetFunc) (void * , void *);
    u16  u16CfgSize;
}HalDispQueryCallBackFunc_t;


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
bool gbDispHwIfInit = 0;
HalDispQueryCallBackFunc_t gpDispCbTbl[E_HAL_DISP_QUERY_MAX];


//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
HalDispQueryRet_e _HalDispIfGetInfoDeviceInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetDeviceInit(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoDeviceEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceEnable(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceAttach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceAttach(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceDetach(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceDetach(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceBackGroundColor(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceBackGroundColor(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceInterface(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceInterface(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceOutpuTiming(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispDeviceTimingInfo_t *pstDeviceTimingCfg;
    DrvDispCtxDeviceContain_t *pstDeviceContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstDeviceContain = pstDispCtxCfg->pstCtxContain->pstDevContain[pstDispCtxCfg->u32Idx];
    pstDeviceTimingCfg = (HalDispDeviceTimingInfo_t *)pCfg;

    memcpy(&pstDeviceContain->stDevTimingCfg, &pstDeviceTimingCfg->stDeviceTimingCfg, sizeof(HalDispDeviceTimingConfig_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, H(%d %d %d %d %d) V(%d %d %d %d %d) Fps:%d\n",
        __FUNCTION__, __LINE__,
        pstDeviceContain->stDevTimingCfg.u16HsyncWidth, pstDeviceContain->stDevTimingCfg.u16HsyncBackPorch,
        pstDeviceContain->stDevTimingCfg.u16Hstart, pstDeviceContain->stDevTimingCfg.u16Hactive,
        pstDeviceContain->stDevTimingCfg.u16Htotal,
        pstDeviceContain->stDevTimingCfg.u16VsyncWidth, pstDeviceContain->stDevTimingCfg.u16VsyncBackPorch,
        pstDeviceContain->stDevTimingCfg.u16Vstart, pstDeviceContain->stDevTimingCfg.u16Vactive,
        pstDeviceContain->stDevTimingCfg.u16Vtotal,
        pstDeviceContain->stDevTimingCfg.u16Fps);

    return enRet;
}

void _HalDispIfSetDeviceOutputTiming(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceVgaParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceVgaParam(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceHdmiParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceHdmiParam(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoDeviceCvbsParam(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceCvbsParam(void *pCtx, void *pCfg)
{

}


HalDispQueryRet_e _HalDispIfGetInfoDevicTimeZone(void *pCtx, void *pCfg)
{
     HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetDeviceTimeZone(void *pCtx, void *pCfg)
{

}

//-------------------------------------------------------------------------------
// VidLayer
//-------------------------------------------------------------------------------
HalDispQueryRet_e _HalDispIfGetInfoVidLayerInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetVidLayerInit(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerEnable(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerBind(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerUnBind(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerUnBind(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerAttr(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerCompress(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerComporess(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerPriority(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerPriority(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerBufferFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerBufferFire(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoVidLayerCheckFire(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetVidLayerCheckFire(void *pCtx, void *pCfg)
{

}

//-------------------------------------------------------------------------------
// InputPort
//-------------------------------------------------------------------------------
HalDispQueryRet_e _HalDispIfGetInfoInputPortInit(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetInputPortInit(void *pCtx, void *pCfg)
{
    HalDipSetBaseNum(1);
    HalDispSetY2RMatrix();
}


HalDispQueryRet_e _HalDispIfGetInfoInputPortEnable(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    bool *pbEnable = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pbEnable = (bool *)pCfg;

    pstInputPortContain->bEnable = *pbEnable;

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, En:%d\n", __FUNCTION__, __LINE__, *pbEnable);

    return enRet;
}

void _HalDispIfSetInputPortEnable(void *pCtx, void *pCfg)
{
    bool *pbEnable = NULL;

    pbEnable = (bool *)pCfg;

    HalDispSetOutPutOnOff(*pbEnable);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;

    memcpy(&pstInputPortContain->stAttr, pstHalInputPortCfg, sizeof(HalDispInputPortAttr_t));

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Disp(%d %d %d %d) Src(%d %d)\n",
        __FUNCTION__, __LINE__,
        pstHalInputPortCfg->stDispWin.u16X, pstHalInputPortCfg->stDispWin.u16Y,
        pstHalInputPortCfg->stDispWin.u16Width, pstHalInputPortCfg->stDispWin.u16Height,
        pstHalInputPortCfg->u16SrcWidth, pstHalInputPortCfg->u16SrcHeight);

    return enRet;
}

void _HalDispIfSetInputPortAttr(void *pCtx, void *pCfg)
{
    HalDispInputPortAttr_t *pstHalInputPortCfg = NULL;

    pstHalInputPortCfg = (HalDispInputPortAttr_t *)pCfg;

    HalDispSetOutputWidth(pstHalInputPortCfg->stDispWin.u16Width);
    HalDispSetOutputHeight(pstHalInputPortCfg->stDispWin.u16Height);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortShow(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetInputPortShow(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortHide(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetInputPortHide(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortBegin(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetInputPortBegin(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortEnd(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetInputPortEnd(void *pCtx, void *pCfg)
{

}

HalDispQueryRet_e _HalDispIfGetInfoInputPortFlip(void *pCtx, void *pCfg)
{
    u16 i;
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    HalDispVideoFrameData_t *pstFramedata = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;

    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];
    pstFramedata = (HalDispVideoFrameData_t *)pCfg;

    memcpy(&pstInputPortContain->stFrameData, pstFramedata, sizeof(HalDispVideoFrameData_t));

    if(pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV422_YUYV ||
       pstFramedata->ePixelFormat == E_HAL_DISP_PIXEL_FRAME_YUV_SEMIPLANAR_422)
    {
        for(i=0; i<3; i++)
        {
            pstInputPortContain->stFrameData.au32Stride[i] /= 2;

           if(i==1)
           {
               pstInputPortContain->stFrameData.au64PhyAddr[1] =
                pstInputPortContain->stFrameData.au64PhyAddr[0] + 0x10;
           }
        }
    }

    DISP_DBG(DISP_DBG_LEVEL_HAL, "%s %d, Fmt:%s, PhyAddr(%08llx, %08llx, %08llx), Stride(%ld %ld %ld)\n",
        __FUNCTION__, __LINE__,
        PARSING_HAL_PIXEL_FMT(pstInputPortContain->stFrameData.ePixelFormat),
        pstInputPortContain->stFrameData.au64PhyAddr[0], pstInputPortContain->stFrameData.au64PhyAddr[1], pstInputPortContain->stFrameData.au64PhyAddr[2],
        pstInputPortContain->stFrameData.au32Stride[0], pstInputPortContain->stFrameData.au32Stride[1], pstInputPortContain->stFrameData.au32Stride[2]);

    return enRet;
}

void _HalDispIfSetInputPortFlip(void *pCtx, void *pCfg)
{
    DrvDispCtxConfig_t *pstDispCtxCfg = NULL;
    DrvDispCtxInputPortContain_t *pstInputPortContain = NULL;
    pstDispCtxCfg = (DrvDispCtxConfig_t *)pCtx;
    pstInputPortContain = pstDispCtxCfg->pstCtxContain->pstInputPortContain[pstDispCtxCfg->u32Idx];

    HalDispSetColorFormat(pstInputPortContain->stFrameData.ePixelFormat);
    HalDispSetBaseAddr( pstInputPortContain->stFrameData.au64PhyAddr[0], pstInputPortContain->stFrameData.au64PhyAddr[1], 0);
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortRotate(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}

void _HalDispIfSetInputPortRotate(void *pCtx, void *pCfg)
{
    
}

HalDispQueryRet_e _HalDispIfGetInfoInputPortCrop(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_NONEED;
    return enRet;
}
void _HalDispIfSetInputPortCrop(void *pCtx, void *pCfg)
{
    
}

//-------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------

bool _HalDispIfGetCallBack(DrvDispCtxConfig_t *pstDispCfg, HalDispQueryConfig_t *pstQueryCfg)
{
    memset(&pstQueryCfg->stOutCfg, 0, sizeof(HalDispQueryOutConfig_t));

    if(pstQueryCfg->stInCfg.u32CfgSize != gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize)
    {
        pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_CFGERR;
        DISP_ERR("%s %d, Query:%s, Size %ld != %d\n",
            __FUNCTION__, __LINE__,
            PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType),
            pstQueryCfg->stInCfg.u32CfgSize,
            gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].u16CfgSize);
    }
    else
    {
        pstQueryCfg->stOutCfg.pSetFunc = gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pSetFunc;

        if(pstQueryCfg->stOutCfg.pSetFunc == NULL)
        {
            pstQueryCfg->stOutCfg.enQueryRet = E_HAL_DISP_QUERY_RET_NOTSUPPORT;
            DISP_ERR("%s %d, Query:%s, SetFunc Empty\n",
                __FUNCTION__, __LINE__,
                PARSING_HAL_QUERY_TYPE(pstQueryCfg->stInCfg.enQueryType));
        }
        else
        {
            pstQueryCfg->stOutCfg.enQueryRet =
                gpDispCbTbl[pstQueryCfg->stInCfg.enQueryType].pGetInfoFunc(pstDispCfg, pstQueryCfg->stInCfg.pInCfg);

        }
    }

    return 1;
}

HalDispQueryRet_e _HalDispIfGetInfoClkGet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
    return enRet;
}

void _HalDispIfSetClkGet(void *pCtx, void *pCfg)
{
}

HalDispQueryRet_e _HalDispIfGetInfoClkSet(void *pCtx, void *pCfg)
{
    HalDispQueryRet_e enRet = E_HAL_DISP_QUERY_RET_OK;
   

    return enRet;
}

void _HalDispIfSetClkSet(void *pCtx, void *pCfg)
{
    
}

//-------------------------------------------------------------------------------------------------
//  Global Functions
//-------------------------------------------------------------------------------------------------
bool HalDispIfInit(void)
{
    void *pNull;
    if(gbDispHwIfInit)
    {
        return 1;
    }

    pNull = NULL;

    memset(gpDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t)*E_HAL_DISP_QUERY_MAX);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pGetInfoFunc = _HalDispIfGetInfoDeviceInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].pSetFunc     = _HalDispIfSetDeviceInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pGetInfoFunc = _HalDispIfGetInfoDeviceEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].pSetFunc     = _HalDispIfSetDeviceEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pGetInfoFunc = _HalDispIfGetInfoDeviceAttach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].pSetFunc     = _HalDispIfSetDeviceAttach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_ATTACH].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pGetInfoFunc = _HalDispIfGetInfoDeviceDetach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].pSetFunc     = _HalDispIfSetDeviceDetach;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_DETACH].u16CfgSize   = sizeof(pNull);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pGetInfoFunc = _HalDispIfGetInfoDeviceBackGroundColor;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].pSetFunc     = _HalDispIfSetDeviceBackGroundColor;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_BACKGROUND_COLOR].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pGetInfoFunc = _HalDispIfGetInfoDeviceInterface;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].pSetFunc     = _HalDispIfSetDeviceInterface;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_INTERFACE].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pGetInfoFunc = _HalDispIfGetInfoDeviceOutpuTiming;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].pSetFunc     = _HalDispIfSetDeviceOutputTiming;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_OUTPUTTIMING].u16CfgSize   = sizeof(HalDispDeviceTimingInfo_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceVgaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].pSetFunc     = _HalDispIfSetDeviceVgaParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_VGA_PARAM].u16CfgSize   = sizeof(HalDispVgaParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pGetInfoFunc = _HalDispIfGetInfoDeviceCvbsParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].pSetFunc     = _HalDispIfSetDeviceCvbsParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_CVBS_PARAM].u16CfgSize   = sizeof(HalDispCvbsParam_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pGetInfoFunc =_HalDispIfGetInfoDeviceHdmiParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].pSetFunc     = _HalDispIfSetDeviceHdmiParam;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_HDMI_PARAM].u16CfgSize   = sizeof(HalDispHdmiParam_t);



    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pGetInfoFunc =_HalDispIfGetInfoDevicTimeZone;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].pSetFunc     = _HalDispIfSetDeviceTimeZone;
    gpDispCbTbl[E_HAL_DISP_QUERY_DEVICE_TIME_ZONE].u16CfgSize   = sizeof(HalDispTimeZone_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pGetInfoFunc = _HalDispIfGetInfoVidLayerInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].pSetFunc     = _HalDispIfSetVidLayerInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pGetInfoFunc = _HalDispIfGetInfoVidLayerEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].pSetFunc     = _HalDispIfSetVidLayerEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].pSetFunc     = _HalDispIfSetVidLayerBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BIND].u16CfgSize   = 4;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pGetInfoFunc = _HalDispIfGetInfoVidLayerUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].pSetFunc     = _HalDispIfSetVidLayerUnBind;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_UNBIND].u16CfgSize   = 4;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pGetInfoFunc = _HalDispIfGetInfoVidLayerAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].pSetFunc     = _HalDispIfSetVidLayerAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_ATTR].u16CfgSize   = sizeof(HalDispVideoLayerAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pGetInfoFunc = _HalDispIfGetInfoVidLayerCompress;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].pSetFunc     = _HalDispIfSetVidLayerComporess;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_COMPRESS].u16CfgSize   = sizeof(HalDispVideoLayerCompressAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pGetInfoFunc = _HalDispIfGetInfoVidLayerPriority;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].pSetFunc     = _HalDispIfSetVidLayerPriority;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_PRIORITY].u16CfgSize   = sizeof(u32);

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerBufferFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].pSetFunc     = _HalDispIfSetVidLayerBufferFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_BUFFER_FIRE].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pGetInfoFunc = _HalDispIfGetInfoVidLayerCheckFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].pSetFunc     = _HalDispIfSetVidLayerCheckFire;
    gpDispCbTbl[E_HAL_DISP_QUERY_VIDEOLAYER_CHECK_FIRE].u16CfgSize   = 0;


    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pGetInfoFunc = _HalDispIfGetInfoInputPortInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].pSetFunc     = _HalDispIfSetInputPortInit;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_INIT].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pGetInfoFunc = _HalDispIfGetInfoInputPortEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].pSetFunc     = _HalDispIfSetInputPortEnable;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ENABLE].u16CfgSize   = sizeof(bool);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pGetInfoFunc = _HalDispIfGetInfoInputPortAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].pSetFunc     = _HalDispIfSetInputPortAttr;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ATTR].u16CfgSize   = sizeof(HalDispInputPortAttr_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pGetInfoFunc = _HalDispIfGetInfoInputPortShow;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].pSetFunc     = _HalDispIfSetInputPortShow;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_SHOW].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pGetInfoFunc = _HalDispIfGetInfoInputPortHide;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].pSetFunc     = _HalDispIfSetInputPortHide;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_HIDE].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pGetInfoFunc = _HalDispIfGetInfoInputPortBegin;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].pSetFunc     = _HalDispIfSetInputPortBegin;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_BEGIN].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pGetInfoFunc = _HalDispIfGetInfoInputPortEnd;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].pSetFunc     = _HalDispIfSetInputPortEnd;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_END].u16CfgSize   = 0;

    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pGetInfoFunc = _HalDispIfGetInfoInputPortFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].pSetFunc     = _HalDispIfSetInputPortFlip;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_FLIP].u16CfgSize   = sizeof(HalDispVideoFrameData_t);

	gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pGetInfoFunc = _HalDispIfGetInfoInputPortRotate;
	gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].pSetFunc 	= _HalDispIfSetInputPortRotate;
	gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_ROTATE].u16CfgSize	= 0;

	gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pGetInfoFunc = _HalDispIfGetInfoInputPortCrop;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].pSetFunc     = _HalDispIfSetInputPortCrop;
    gpDispCbTbl[E_HAL_DISP_QUERY_INPUTPORT_CROP].u16CfgSize   = 8;

    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pGetInfoFunc = _HalDispIfGetInfoClkSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].pSetFunc     = _HalDispIfSetClkSet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_SET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pGetInfoFunc = _HalDispIfGetInfoClkGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].pSetFunc     = _HalDispIfSetClkGet;
    gpDispCbTbl[E_HAL_DISP_QUERY_CLK_GET].u16CfgSize   = sizeof(HalDispClkConfig_t);

    gbDispHwIfInit = 1;
    return 1;
}

bool HalDispIfDeInit(void)
{
    if(gbDispHwIfInit == 0)
    {
        DISP_ERR("%s %d, HalIf not init\n", __FUNCTION__, __LINE__);
        return 0;
    }
    gbDispHwIfInit = 0;
    memset(gpDispCbTbl, 0, sizeof(HalDispQueryCallBackFunc_t)* E_HAL_DISP_QUERY_MAX);
    return 1;
}

bool HalDispIfQuery(void *pCtx,  void *pCfg)
{
    bool bRet = 1;

    if(pCtx == NULL)
    {
        DISP_ERR("%s %d, Input Ctx is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else if(pCfg == NULL)
    {
        DISP_ERR("%s %d, Input Cfg is Empty\n", __FUNCTION__, __LINE__);
        bRet = 0;
    }
    else
    {
        bRet = _HalDispIfGetCallBack((DrvDispCtxConfig_t *)pCtx, (HalDispQueryConfig_t *)pCfg);
    }

    return bRet;
}



