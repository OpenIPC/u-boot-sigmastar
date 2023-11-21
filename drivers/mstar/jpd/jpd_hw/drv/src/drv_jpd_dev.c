/*
 * drv_jpd_dev.c- Sigmastar
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

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include "drv_jpd_dev.h"
#include "drv_jpd_vdi.h"
#include "hal_jpd_ops.h"
#include "hal_jpd_def.h"

volatile MS_U32 g_jpdIsrStatus[MAX_JPD_DEVICE_NUM]; // only for progressive mode

void ApiJpdAsmCpuSync(void)
{
    asm __volatile__("DSB sy");
}

void ApiJPDSetISRStatus(int devid, MS_U32 status)
{
    JPD_LOGD("jpd%d set isr status %d\n", devid, status);
    g_jpdIsrStatus[devid] |= status;
}

void ApiJPDResetISRStatus(int devid)
{
    JPD_LOGD("jpd%d reset isr status\n", devid);
    g_jpdIsrStatus[devid] = 0;
}

int ApiJPDGetISRStatus(int devid)
{
    return g_jpdIsrStatus[devid];
}

MS_S32 ApiJPDRst(JPD_Dev_t *pJpdDev)
{
    MapiJpdReset(pJpdDev->u32DevId);
    MapiJpdWriteIMIEnable(pJpdDev->u32DevId, FALSE);    // reg_jpd_w2imi_enable   = 0
    MapiJpdSetWRingBufEnable(pJpdDev->u32DevId, FALSE); // reg_jpd_wbuf_ring_en    = 0
    return TRUE;
}

#ifdef NJPD_CMDQ_SUPPORT
void ApiJpdSetCfgCmdq(MS_U8 u8DevId, void *pCmdqHandle, MS_U64 *pu64CmdqBuf)
{
    MapiJpdSetCfgCmdq(u8DevId, pCmdqHandle, pu64CmdqBuf);
}

void ApiJpdGetCfgCmdq(MS_U8 u8DevId, void **ppCmdqHandle, MS_U64 **ppu64CmdqBuf, MS_U32 *pu32CmdLen)
{
    MapiJpdGetCfgCmdq(u8DevId, ppCmdqHandle, ppu64CmdqBuf, pu32CmdLen);
}
#endif
