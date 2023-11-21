/*
 * drv_jpd_vdi.h- Sigmastar
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

#ifndef _DRV_JPD_VDI_H_
#define _DRV_JPD_VDI_H_

#include <mhal_common.h>

typedef struct
{
    MS_S32 (*AllocMma)(void *pmhalHandle, MS_U8 *pu8MMAHeapName, MS_U8 *pu8BufName, MS_U32 u32Size, MS_PHY *phyAddr);
    MS_S32 (*FreeMma)(void *pmhalHandle, MS_PHY phyAddr);
    void *(*Map)(MS_PHY phyAddr, MS_U32 u32Size, MS_U8 bCache);
    void (*Unmap)(void *pVirtAddr);
    MS_S32 (*Flush)(void *pVirtAddr, MS_U32 u32Size);
    // int32 (*GetMMaHeapName)(MS_U8 *pu8MmaHeapName, uint32 u32BufferSize, uint32 u32HeapType);
    // int32 (*SetMiuProtectMMa)(MS_U8 *pu8MmaHeapName,uint32 *pu32ProtectId ,uint32 u32ClientIdNum, MS_U8 bEnable);
} JPDMMACB;

typedef struct
{
    unsigned char *pu8MMAHeapName;
    JPDMMACB       JpdMMACB;
} JPD_MMA_info_t;

typedef struct JpdAddrBuffer_s
{
    unsigned int       size;
    unsigned long long phys_addr;
    unsigned long long base;
    void *             not_aligned_virt_addr;
    void *             virt_addr;

} Jpd_AddrBuffer_t;

void            MhalJpdVdiResetMmaInfo(unsigned int u32DevId);
JPD_MMA_info_t *MhalJpdVdiGetMmaInfo(unsigned int u32DevId);

int  MhalJpdVdiMmaAlloc(unsigned int u32DeviceId, Jpd_AddrBuffer_t *pDevBuf, MS_U8 *pMmaHeapName, char *name,
                        MS_U8 u8NeedVmap);
void MhalJpdVdiMmaFree(unsigned int u32DeviceId, Jpd_AddrBuffer_t *pDevBuf);

MS_BOOL DrvJpdVdiMemInitPool(void *pool, MS_U32 size);
void *  DrvJpdVdiMemAlloc(MS_U32 size);
void    DrvJpdVdiMemFree(void *memp);
#endif //_DRV_JPD_VDI_H_
