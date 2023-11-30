/*
* drv_disp_ctx.h- Sigmastar
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

#ifndef _DRV_DISP_CTX_H_
#define _DRV_DISP_CTX_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_CTX_INVAILD_IDX 0xFFFF
//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_DISP_CTX_TYPE_DEVICE,
    E_DISP_CTX_TYPE_VIDLAYER,
    E_DISP_CTX_TYPE_INPUTPORT,
    E_DISP_CTX_TYPE_DMA,
    E_DISP_CTX_TYPE_MAX,
} DrvDispCtxType_e;

//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef s32 (*pDispCtxMemAlloc)(u8 *pu8Name, u32 size, ss_phys_addr_t *ppaPhyAddr);
typedef s32 (*pDispCtxMemFree)(ss_phys_addr_t paPhyAddr);

typedef struct
{
    pDispCtxMemAlloc alloc;
    // Success return 0
    pDispCtxMemFree free;
} DrvDispCtxMemAllocConfig_t;

typedef struct
{
    u8                       bUsed;
    u32                      u32PortId;
    u8                       bEnable;
    u8                       bDisplay;
    u8                       bBeginEnd;
    HalDispVideoFrameData_t  stFrameData;
    HalDispInputPortAttr_t   stAttr;
    HalDispVidWinRect_t      stCrop;
    HalDispInputPortRotate_t stRot;
    HalDispRingBuffAttr_t    stRingBuffAttr;
    void *                   pstVidLayerContain;
    u16                      u16FlipFrontPorchCnt;
} DrvDispCtxInputPortContain_t;

typedef struct
{
    u32                             u32VidLayerId;
    void *                          pstDevCtx;
    u8                              bEnable;
    HalDispVideoLayerAttr_t         stAttr;
    HalDispVideoLayerCompressAttr_t stCompress;
    u32                             u32Priority;
    DrvDispCtxInputPortContain_t *  pstInputPortContain[HAL_DISP_INPUTPORT_NUM];
} DrvDispCtxVideoLayerContain_t;

typedef struct
{
    u32                          u32DmaId;
    HalDispDmaInputConfig_t      stInputCfg;
    HalDispDmaOutputConfig_t     stOutputCfg;
    HalDispDmaBufferAttrConfig_t stBufferAttrCfg;
    void *                       pSrcDevContain;
    void *                       pDestDevContain;
    u8                           u8RingModeEn;
    u16                          u16RingMdChange;
} DrvDispCtxDmaContain_t;

typedef struct
{
    u32                            u32DevId;
    u8                             bEnable;
    u8                             bMergeCase;
    u32                            u32BgColor;
    u32                            u32Interface;
    u16                            u16McuCnt;
    u16                            u16McuFenceId;
    u32                            u32DeviceSynth[E_HAL_DISP_DEV_SYNTH_MAX];
    HalDispVideoLayerType_e        eBindVideoLayer;
    HalDispDeviceTiming_e          eTimeType;
    HalDispDeviceTimingConfig_t    stDevTimingCfg;
    HalDispDeviceParam_t           stDeviceParam;
    HalDispGammaParam_t            stGammaParam;
    HalDispColorTemp_t             stColorTemp;
    HalDispTimeZoneConfig_t        stTimeZoeCfg;
    DrvDispCtxVideoLayerContain_t *pstVidLayeCtx[HAL_DISP_VIDLAYER_MAX];
    void *                         pstDevAttachSrc;
} DrvDispCtxDeviceContain_t;

typedef struct
{
    u8                             bDevContainUsed[HAL_DISP_DEVICE_MAX];
    u8                             bVidLayerContainUsed[HAL_DISP_VIDLAYER_MAX];
    u8                             bInputPortContainUsed[HAL_DISP_INPUTPORT_MAX];
    u8                             bDmaContainUsed[HAL_DISP_DMA_MAX];
    DrvDispCtxDeviceContain_t *    pstDevContain[HAL_DISP_DEVICE_MAX];
    DrvDispCtxVideoLayerContain_t *pstVidLayerContain[HAL_DISP_VIDLAYER_MAX];
    DrvDispCtxInputPortContain_t * pstInputPortContain[HAL_DISP_INPUTPORT_MAX];
    DrvDispCtxDmaContain_t *       pstDmaContain[HAL_DISP_DMA_MAX];
    DrvDispCtxMemAllocConfig_t     stMemAllcCfg;
    HalDispHwContain_t *           pstHalHwContain;
} DrvDispCtxContain_t;

typedef struct
{
    DrvDispCtxType_e     enCtxType;
    u32                  u32ContainIdx;
    u32                  u32CtxIdx;
    DrvDispCtxContain_t *pstCtxContain;
} DrvDispCtxConfig_t;

typedef struct
{
    DrvDispCtxType_e           enType;
    u32                        u32DevId;
    DrvDispCtxConfig_t *       pstBindCtx;
    DrvDispCtxMemAllocConfig_t stMemAllcCfg;
} DrvDispCtxAllocConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifndef _DRV_DISP_CTX_C_
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE u8   DrvDispCtxInit(void);
INTERFACE u8   DrvDispCtxDeInit(void);
INTERFACE void DrvDispIrqInit(void);
INTERFACE u8   DrvDispCtxAllocate(DrvDispCtxAllocConfig_t *pAllocCfg, DrvDispCtxConfig_t **pCtx);
INTERFACE u8   DrvDispCtxFree(DrvDispCtxConfig_t *pCtx);
INTERFACE u8   DrvDispCtxIsAllFree(void);
INTERFACE u8   DrvDispCtxIsLastDeviceCtx(DrvDispCtxConfig_t *pCtx);
INTERFACE u8   DrvDispCtxSetCurCtx(DrvDispCtxConfig_t *pCtx, u32 u32Idx);
INTERFACE u8   DrvDispCtxGetCurCtx(DrvDispCtxType_e enCtxType, u32 u32Idx, DrvDispCtxConfig_t **pCtx);

#undef INTERFACE

#endif
