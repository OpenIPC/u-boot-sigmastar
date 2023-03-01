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

#ifndef _HAL_DISP_IRQ_H_
#define _HAL_DISP_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_IRQ_IOCTL_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_GET_ID,

    E_HAL_DISP_IRQ_IOCTL_DMA_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_DMA_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_DMA_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_DMA_GET_ID,

    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_SUPPORTED,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_ID,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_TIMEZONE_GET_STATUS,

    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_SUPPORTED,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_ID,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_VGA_HPD_GET_STATUS,
    E_HAL_DISP_IRQ_IOCTL_NUM,
}HalDispIrqIoCtlType_e;

typedef enum
{
    E_HAL_DISP_IRQ_TYPE_NONE                    = 0x00000000,
    E_HAL_DISP_IRQ_TYPE_VSYNC                   = 0x00000001,
    E_HAL_DISP_IRQ_TYPE_VDE                     = 0x00000002,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE   = 0x00000100,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_POSITIVE   = 0x00000200,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_NEGATIVE = 0x00000400,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VSYNC_POSITIVE = 0x00000800,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON              = 0x00001000,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF             = 0x00002000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON           = 0x00004000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF          = 0x00008000,
}HalDispIrqType_e;

typedef enum
{
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE = 0x00,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_NEGATIVE = 0x01,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE   = 0x02,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE   = 0x03,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM            = 0x04,
}HalDispIrqTimeStampType_e;

typedef struct
{
    u32 u32IrqFlags;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM];
}HalDispIrqTimeZoneStatus_t;

typedef struct
{
    HalDispIrqIoCtlType_e enIoctlType;
    HalDispIrqType_e enIrqType;
    void *pDispCtx;
    void *pParam;
}HalDispIrqIoctlConfig_t;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifdef _HAL_DISP_IRQ_C_
#define INTERFACE

#else
#define INTERFACE extern
#endif
INTERFACE bool HalDispIrqIoCtl(HalDispIrqIoctlConfig_t *pCfg);
INTERFACE void HalDispIrqSetDacEn(u8 u8Val);

#undef INTERFACE

#endif
