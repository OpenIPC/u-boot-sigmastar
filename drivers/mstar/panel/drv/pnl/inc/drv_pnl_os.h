/*
* drv_pnl_os.h- Sigmastar
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

#ifndef _DRV_PNL_OS_H_
#define _DRV_PNL_OS_H_

#include "drv_pnl_os_header.h"

#define    DRV_PNL_OS_OUTPUT_10BIT_MODE        0x00
#define    DRV_PNL_OS_OUTPUT_6BIT_MODE         0x01
#define    DRV_PNL_OS_OUTPUT_8BIT_MODE         0x02
#define    DRV_PNL_OS_OUTPUT_565BIT_MODE       0x03

#define    DRV_PNL_OS_LINK_TTL                 0x00
#define    DRV_PNL_OS_LINK_LVDS                0x01
#define    DRV_PNL_OS_LINK_RSDS                0x02
#define    DRV_PNL_OS_LINK_MINILVDS            0x03
#define    DRV_PNL_OS_LINK_ANALOG_MINILVDS     0x04
#define    DRV_PNL_OS_LINK_DIGITAL_MINILVDS    0x05
#define    DRV_PNL_OS_LINK_MFC                 0x06
#define    DRV_PNL_OS_LINK_DAC_I               0x07
#define    DRV_PNL_OS_LINK_DAC_P               0x08
#define    DRV_PNL_OS_LINK_PDPLVDS             0x09
#define    DRV_PNL_OS_LINK_EXT                 0x0A
#define    DRV_PNL_OS_LINK_MIPI_DSI            0x0B
#define    DRV_PNL_OS_LINK_BT656               0x0C
#define    DRV_PNL_OS_LINK_BT601               0x0D
#define    DRV_PNL_OS_LINK_TTL_SPI_IF          0x0E

#ifdef _DRV_PNL_OS_C_
#define INTERFACE
#else
#define INTERFACE extern
#endif


INTERFACE void * DrvPnlOsMemAlloc(u32 u32Size);
INTERFACE void DrvPnlOsMemRelease(void *pPtr);
INTERFACE void DrvPnlOsMsSleep(u32 u32Msec);
INTERFACE void DrvPnlOsUsSleep(u32 u32Usec);
INTERFACE bool DrvPnlOsPadMuxActive(void);
INTERFACE int  DrvPnlOsPadMuxGetMode(void);
INTERFACE int  DrvPnlOsGetMode(u16 u16LinkType, u16 u16OutputFormatBitMode);
INTERFACE bool DrvPnlOsSetDeviceNode(void *pPlatFormDev);
INTERFACE bool DrvPnlOsSetClkOn(void *pClkRate, u32 u32ClkRateSize);
INTERFACE bool DrvPnlOsSetClkOff(void *pClkRate, u32 u32ClkRateSize);

#undef INTERFACE

#endif
