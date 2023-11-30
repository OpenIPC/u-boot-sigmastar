/*
* drv_audio_pri.h- Sigmastar
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

#ifndef __DRV_AUDIO_PRI_H__
#define __DRV_AUDIO_PRI_H__

#define DMA_EMPTY 0x0
#define DMA_UNDERRUN 0x1
#define DMA_OVERRUN 0x2
#define DMA_FULL 0x3
#define DMA_NORMAL 0x4
#define DMA_INIT 0x5

#define SND_PCM_STATE_INIT     (1<<0)
#define SND_PCM_STATE_SETUP    (1<<1)
#define SND_PCM_STATE_OPEN     (1<<2)
#define SND_PCM_STATE_PREPARED (1<<3)
#define SND_PCM_STATE_RUNNING  (1<<4)
#define SND_PCM_STATE_XRUN     (1<<5)
#define SND_PCM_STATE_PAUSED   (1<<6)



typedef long PcmFrames_t;

typedef enum
{
    PCM_STREAM_PLAYBACK,
    PCM_STREAM_CAPTURE,
} PcmStream_e;

typedef struct AecInfo
{
    u64 nStartPts;
    s64 nReadPtr;
    s64 nWritePtr;

} AecInfo_t;

typedef u32 dma_addr_t;

typedef struct
{
    u8  szName[20];    /* stream identifier */
    u16 nChnId;    /* Channel ID */
    u8 *pDmaArea;	/* DMA area */
    dma_addr_t tDmaAddr; /* physical bus address (not accessible from main CPU) */
    u32 nBufBytes; /* Size of the DMA transfer */
    PcmFrames_t tBufSize;    /* Size of the DMA transfer */
    PcmFrames_t tPeriodSize;
    PcmFrames_t tApplPtr;
    //CamOsSpinlock_t tLock;
    AecInfo_t tAecInfo;

} PcmDmaData_t;

typedef struct PcmRuntimeData
{
    //CamOsSpinlock_t       tPcmLock;
    //CamOsTsem_t           tSleep;
    volatile u16          nState;
    u32                   nRemainCount;
    BOOL                  bEnableIrq;
    PcmStream_e           eStream;
    void                  *pPrivateData;
    u16                   nFrameBits;
    u16                   nHwFrameBits;
    u16                   nStatus;
    s32 (*copy)(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames);
} PcmRuntimeData_t;



#endif //__DRV_AUDIO_PRI_H__