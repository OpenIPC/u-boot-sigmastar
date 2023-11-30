/*
* drv_audio.c- Sigmastar
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

// Hal
#include "hal_audio_common.h"
#include "hal_audio_sys.h"
#include "hal_audio_config.h"
#include "hal_audio_reg.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"

// Drv
#include "drv_audio.h"
#include "drv_audio_api.h"
#include "drv_audio_pri.h"

#include <malloc.h>

#define SEEK_SET        0       /* Seek from beginning of file.  */
#define SEEK_CUR        1       /* Seek from current position.  */

#ifndef max
#define max(x, y) ({				\
	typeof(x) _max1 = (x);			\
	typeof(y) _max2 = (y);			\
	(void) (&_max1 == &_max2);		\
	_max1 > _max2 ? _max1 : _max2; })
#endif


PcmRuntimeData_t _gaRuntimeData[AUD_DMA_NUM];
DmaParam_t _gaDmaParam[AUD_DMA_NUM];
static BOOL _gbI2sMaster[AUD_I2S_IF_NUM];
int nMchExt[AUD_DMA_NUM];

extern void Chip_Flush_Memory(void);

/**
 * bytes_to_frames - Unit conversion of the size from bytes to frames
 * @runtime: PCM runtime instance
 * @size: size in bytes
 */
static inline PcmFrames_t _BytesToFrames(PcmRuntimeData_t *pRtd, u32 size)
{
    return size / (pRtd->nFrameBits >> 3);
}

static inline PcmFrames_t _BytesToHwFrames(PcmRuntimeData_t *pRtd, u32 size)
{
    return size / (pRtd->nHwFrameBits >> 3);
}

/**
 * frames_to_bytes - Unit conversion of the size from frames to bytes
 * @runtime: PCM runtime instance
 * @size: size in frames
 */
static inline u32 _FramesToBytes(PcmRuntimeData_t *pRtd, PcmFrames_t size)
{
    return size * (pRtd->nFrameBits >> 3);
}

static inline u32 _HwFramesToBytes(PcmRuntimeData_t *pRtd, PcmFrames_t size)
{
    return size * (pRtd->nHwFrameBits >> 3);
}

/**
 * _AudPcmPlaybackAvail - Get the available (writable) space for playback
 */
static inline PcmFrames_t _AudPcmPlaybackAvail(PcmDmaData_t *pDmaData)
{
    //unsigned long nFlags;
    PcmFrames_t tAvail;
    u32 nCurDmaLevelCount = 0;

    //CamOsSpinLockIrqSave(&pDmaData->tLock);
    nCurDmaLevelCount = HalAudDmaGetLevelCnt(pDmaData->nChnId);

    //printf(" AO --- Curr LC = %u\r\n", nCurDmaLevelCount);

    tAvail = pDmaData->tBufSize - _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],nCurDmaLevelCount+_gaRuntimeData[pDmaData->nChnId].nRemainCount);

#if NO_EXHAUST_DMA_BUF
    if ( tAvail >= _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],DMA_BUF_REMAINDER ) )
    {
        tAvail = tAvail - _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],DMA_BUF_REMAINDER );
    }
    else
    {
        tAvail = 0;
    }
#endif

    //CamOsSpinUnlockIrqRestore(&pDmaData->tLock);

    return tAvail;
}

/**
 * _AudPcmCaptureAvail - Get the available (readable) space for capture
 */
static inline PcmFrames_t _AudPcmCaptureAvail(PcmDmaData_t *pDmaData)
{
    //unsigned long nFlags;
    PcmFrames_t tAvail;
    u32 nCurDmaLevelCount = 0;

    //CamOsSpinLockIrqSave(&pDmaData->tLock);
    nCurDmaLevelCount = HalAudDmaGetLevelCnt(pDmaData->nChnId);

    //printf(" AI - Curr LC = %u\r\n", nCurDmaLevelCount);

    tAvail = _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],nCurDmaLevelCount-_gaRuntimeData[pDmaData->nChnId].nRemainCount);

#if NO_EXHAUST_DMA_BUF
    if ( tAvail >= _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],DMA_BUF_REMAINDER ) )
    {
        tAvail = tAvail - _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],DMA_BUF_REMAINDER );
    }
    else
    {
        tAvail = 0;
    }
#endif

    //CamOsSpinUnlockIrqRestore(&pDmaData->tLock);

    //printf("buf %ld, count %u,remain %u\n",pDmaData->tBufSize,nCurDmaLevelCount,_gaRuntimeData[pDmaData->nChnId].nRemainCount);

    if(_gaRuntimeData[pDmaData->nChnId].nRemainCount>0)
        printf("remain %u\n",_gaRuntimeData[pDmaData->nChnId].nRemainCount);
    return tAvail;
}

/**
 * _AudPcmCaptureRawAvail - Get the actual record data length for capture
 */
static inline PcmFrames_t _AudPcmCaptureRawAvail(PcmDmaData_t *pDmaData)
{
    //unsigned long nFlags;
    PcmFrames_t tAvail;
    u32 nCurDmaLevelCount = 0;
    //CamOsSpinLockIrqSave(&pDmaData->tLock);

    nCurDmaLevelCount = HalAudDmaGetRawLevelCnt(pDmaData->nChnId);
    tAvail = _BytesToHwFrames(&_gaRuntimeData[pDmaData->nChnId],nCurDmaLevelCount-_gaRuntimeData[pDmaData->nChnId].nRemainCount);

    //CamOsSpinUnlockIrqRestore(&pDmaData->tLock);

    return tAvail;
}

static BOOL _AudDmaInitChannel(AudDmaChn_e eDmaChannel,
                               u8 *pDmaBuf,
                               u32 nPhysDmaAddr,
                               u32 nBufferSize,
                               u32 nChannels,
                               AudBitWidth_e eBitWidth,
                               u32 nPeriodSize)
{
    BOOL bRet = TRUE;
    //printf("%s chn = %d\n",__FUNCTION__,eDmaChannel);

    // Set up the physical DMA buffer address
    bRet &= HalAudDmaSetPhyAddr(eDmaChannel, nPhysDmaAddr, nBufferSize);

    if ( eDmaChannel >= AUD_DMA_NUM )
    {
        return FALSE;
    }

    if ( eDmaChannel < AUD_DMA_READER1 ) // Writer
    {
        bRet &= HalAudDmaWrSetThreshold(eDmaChannel, nPeriodSize);
        _gaRuntimeData[eDmaChannel].bEnableIrq = TRUE;
    }
    else
    {
        bRet &= HalAudDmaRdSetThreshold(eDmaChannel, nBufferSize - nPeriodSize);
        _gaRuntimeData[eDmaChannel].bEnableIrq = TRUE;
    }

    return bRet;
}

static BOOL _AudDmaSetRate(AudDmaChn_e eDma, AudRate_e eRate)
{
    BOOL bRet = TRUE;
    //printf("%s chn = %d, rate %d\n",__FUNCTION__,eDma,eRate);

    if ( eDma >= AUD_DMA_NUM )
    {
        printf("%s chn = %d >= AUD_DMA_NUM\n",__FUNCTION__,eDma );
        return FALSE;
    }

    bRet &= HalAudDmaSetRate(eDma,eRate);

    return bRet;
}

static BOOL _AudDmaStartChannel(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    //PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    //CamOsTimespec_t stTime;

    // Writer for device (CPU read from device).
    // Flush L3 before dma start.
    if (eDmaChannel < AUD_DMA_READER1)
    {
        Chip_Flush_Memory();
    }

    //printf("%s chn = %d, level %u\n",__FUNCTION__,eDmaChannel,HalAudDmaGetLevelCnt(eDmaChannel));
    bRet &= HalAudDmaClearInt(eDmaChannel);
    bRet &= HalAudDmaEnable(eDmaChannel, TRUE);

    //CamOsGetMonotonicTime(&stTime);
    //ptDmaData->tAecInfo.nStartPts = (stTime.nSec*1000000ULL) + (stTime.nNanoSec/1000);

    return bRet;
}


static BOOL _AudDmaStopChannel(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);

    //printf("%s chn = %d\n",__FUNCTION__,eDmaChannel);
    bRet &= HalAudDmaEnable(eDmaChannel, FALSE);
    bRet &= HalAudDmaReset(eDmaChannel); // Here & DrvAudPrepareDma() both need DmaReset, here is for clear level count when empty occur.

    ptDmaData->tAecInfo.nStartPts = 0;

    return bRet;
}

static int _WaitForAvail(PcmRuntimeData_t *ptRuntimeData, PcmFrames_t *ptAvail)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(ptRuntimeData->pPrivateData);
    BOOL bIsPlayback = (ptRuntimeData->eStream == PCM_STREAM_PLAYBACK);
    int err = 0;
    long nWaitTime;
    //CamOsRet_e eRet;
    PcmFrames_t tAvail;


    if (_gaDmaParam[ptDmaData->nChnId].nSampleRate)
    {
        nWaitTime = ptDmaData->tPeriodSize*2*1000/ _gaDmaParam[ptDmaData->nChnId].nSampleRate;
    }
    else
    {
        nWaitTime = 5000;
    }

    nWaitTime = max((long)5000, nWaitTime);//min 5s wait
    //nWaitTime = msecs_to_jiffies(nWaitTime);

    for (;;)
    {
        /*
         * We need to check if space became available already
         * (and thus the wakeup happened already) first to close
         * the race of space already having become available.
         * This check must happen after been added to the waitqueue
         * and having current state be INTERRUPTIBLE.
         */
        if (bIsPlayback)
            tAvail = _AudPcmPlaybackAvail(ptDmaData);
        else
            tAvail = _AudPcmCaptureAvail(ptDmaData);

#if NO_EXHAUST_DMA_BUF
        if ( tAvail >= ( ptDmaData->tPeriodSize - _BytesToHwFrames(&_gaRuntimeData[ptDmaData->nChnId],DMA_BUF_REMAINDER ) ) ) //any available data
                break;
#else
        if (tAvail >=ptDmaData->tPeriodSize) //any available data
            break;
#endif
        //CamOsSpinUnlockIrqRestore(&ptRuntimeData->tPcmLock);

        //eRet = CamOsTsemTimedDown(&ptRuntimeData->tSleep, nWaitTime);

        //CamOsSpinLockIrqSave(&ptRuntimeData->tPcmLock);

        switch (ptRuntimeData->nStatus)
        {
            case SND_PCM_STATE_XRUN:
                err = -EPIPE;
                goto _endloop;
            case SND_PCM_STATE_OPEN:
            case SND_PCM_STATE_SETUP:
                err = -EBADFD;
                goto _endloop;
            case SND_PCM_STATE_PAUSED:
                continue;
        }
#if 0
        if (eRet != CAM_OS_OK)
        {
            switch (eRet)
            {
                case CAM_OS_TIMEOUT:
                    //printf( "TIMEOUT\n");
                    break;
                case CAM_OS_PARAM_ERR:
                    //printf( "PARAM_ERR\n");
                    break;
                default: // CAM_OS_FAIL
                    //printf( "Condition not init\n");
                    break;
            }

            err = -EIO;
            break;
        }
#endif
    }
_endloop:
    *ptAvail = tAvail;
    return err;
}

//writer HW frames should be the same as user's
s32 _AudDmaWrCopyI16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u8 *pHwbufPtr = ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff);
    u8 *pBufPtr = pBuf + _FramesToBytes(ptRuntimeData,nPos);

    memcpy(pBufPtr, pHwbufPtr, _FramesToBytes(ptRuntimeData,tFrames));
    return 0;
}

s32 _AudDmaWrCopyI32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u8 *pHwbufPtr = ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff);
    u8 *pBufPtr = pBuf + _FramesToBytes(ptRuntimeData,nPos);

    memcpy(pBufPtr, pHwbufPtr, _FramesToBytes(ptRuntimeData,tFrames));

    return 0;
}

//output noninterleave data, writer HW frames should be the same as user's
s32 _AudDmaWrCopyN16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = nMchExt[ptDmaData->nChnId];
    u32 i;
    s16 *pnInSample, *pnOutSample;
    u32 nInterval = nBufSize/nChannels;

    pnInSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));
    while(tFrames>0)
    {
        for(i=0; i<nChannels; i++)
        {
            pnOutSample = (s16*)(pBuf + nInterval*i)+nPos;
            *pnOutSample = *pnInSample;
            pnInSample++;
        }
        tFrames --;
        nPos ++;
    }
    return 0;
}

s32 _AudDmaWrCopyN32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

s32 _AudDmaRdCopyI16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = _gaDmaParam[ptDmaData->nChnId].nChannels;
    s16 *pnInSample;
    s16 *pnOutSample;
    u32 i;
    s32 nSampleValue;

    pnOutSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));
    pnInSample = (s16*)(pBuf + _FramesToBytes(ptRuntimeData,nPos));

    while(tFrames>0)
    {
        for(i=0; i<nChannels; i++)
        {
            nSampleValue = (s32)(*pnInSample);
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
            pnInSample++;
        }
        tFrames--;
    }

    return 0;
}

s32 _AudDmaRdCopyI32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

//input noninterleave data
s32 _AudDmaRdCopyN16(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    u16 nChannels = _gaDmaParam[ptDmaData->nChnId].nChannels;
    s16 *pnInSample;
    s16 *pnOutSample;
    u32 nInterval = nBufSize/nChannels;
    u32 i;
    s32 nSampleValue;

    pnOutSample = (s16*)(ptDmaData->pDmaArea + _HwFramesToBytes(ptRuntimeData,nHwoff));

    while(tFrames>0)
    {
        for(i=0; i<nChannels; i++)
        {
            pnInSample = (s16*)(pBuf + nInterval*i)+nPos;
            nSampleValue = (s32)(*pnInSample);
            *pnOutSample = (s16)nSampleValue;
            pnOutSample++;
        }
        tFrames --;
        nPos ++;
    }

    return 0;
}

s32 _AudDmaRdCopyN32(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    return 0;
}

//just copy(not handle wrap-around), function call should check enough size,
s32 _AudDmaKernelCopy(struct PcmRuntimeData *ptRuntimeData, u32 nHwoff, u8* pBuf, u32 nBufSize, u32 nPos, PcmFrames_t tFrames)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t*)ptRuntimeData->pPrivateData;
    //unsigned long nFlags;
    u32 nLevelCount = 0;
    u32 nTrigCount;
    u32 nOldState = ptRuntimeData->nState;
    //u16 nRetry = 0;
    if (PCM_STREAM_PLAYBACK == ptRuntimeData->eStream)
    {
        //printf("%s pos=%u, frames %lu\n",__FUNCTION__,nPos,tFrames);
        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);

        //CamOsSpinLockIrqSave(&ptDmaData->tLock);
        if(nOldState != DMA_EMPTY && ptRuntimeData->nState == DMA_EMPTY)
        {
            //CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
            //printf("%s state changing!!!\n",__FUNCTION__);
            return 0;
        }

        Chip_Flush_Memory();

        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        nTrigCount = HalAudDmaTrigLevelCnt(ptDmaData->nChnId, (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount));
        if (ptRuntimeData->nState == DMA_EMPTY || ptRuntimeData->nState == DMA_INIT)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }
#if 0
        /* Be careful!! check level count updated*/
        while(nRetry<20)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)>nLevelCount)
            {
                printk("Re = %u\r\n",nRetry);
                break;
            }
            nRetry++;
        }

        if(nRetry==20)
        {
            printk("nTrigCount = %u !\r\n",nTrigCount);
            //printf("update level count too slow!!!!!!!!\n");
        }
#endif
        if (((nLevelCount + nTrigCount)  >= (ptDmaData->nBufBytes - _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize))) && ptRuntimeData->nStatus == SND_PCM_STATE_RUNNING)
        {
            //CamOsUsDelay(1); //wait level count ready
            ptRuntimeData->nState = DMA_NORMAL;
            //HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE);
        }

        if ((nLevelCount + nTrigCount)  > ptDmaData->nBufBytes )
            printf("l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));

        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;

        //CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
    }
    else if (PCM_STREAM_CAPTURE == ptRuntimeData->eStream)
    {
        //printf("%s pos=%u, frames %lu\n",__FUNCTION__,nPos,tFrames);
        ptRuntimeData->copy(ptRuntimeData, nHwoff, pBuf, nBufSize, nPos, tFrames);

        //CamOsSpinLockIrqSave(&ptDmaData->tLock);
        if(nOldState != DMA_FULL && ptRuntimeData->nState == DMA_FULL)
        {
            //CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
            //printf("%s state changing!!!\n",__FUNCTION__);
            return 0;
        }

        nLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
        nTrigCount = HalAudDmaTrigLevelCnt(ptDmaData->nChnId, (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount));
        if (ptRuntimeData->nState == DMA_FULL || ptRuntimeData->nState == DMA_INIT)
        {
            HalAudDmaClearInt(ptDmaData->nChnId);
            ptRuntimeData->nState = DMA_NORMAL;
        }

#if 0
        while(nRetry<20)
        {
            if(HalAudDmaGetLevelCnt(ptDmaData->nChnId)<nLevelCount)
                break;
            nRetry++;
        }

         if(nRetry==20)
            printf("update level count too slow!!!!!!!!\n");
#endif

        if (((nLevelCount - nTrigCount)  < _HwFramesToBytes(ptRuntimeData,ptDmaData->tPeriodSize)) && _gaRuntimeData[ptDmaData->nChnId].nStatus == SND_PCM_STATE_RUNNING)
        {
            //CamOsUsDelay(1); //wait level count ready
            ptRuntimeData->nState = DMA_NORMAL;
            //HalAudDmaIntEnable(ptDmaData->nChnId, TRUE, FALSE);
        }

        if (nLevelCount < nTrigCount )
            printf("l:%u, t:%u, size %u!!!\n",nLevelCount,nTrigCount,_HwFramesToBytes(ptRuntimeData,tFrames));

        ptRuntimeData->nRemainCount = (_HwFramesToBytes(ptRuntimeData,tFrames) + ptRuntimeData->nRemainCount) - nTrigCount;
        if(ptRuntimeData->nRemainCount)
        {
            //printf("%s remain count = %ul\n",__FUNCTION__,ptRuntimeData->nRemainCount);
        }

        //CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);
    }
    //printf("%s chn=%d, frames size %d bytes, Hw pos %d , level count %u\n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff),ptRuntimeData->nDmaLevelCount);
    //printf("%s chn=%d, frames size %d bytes, Hw pos %d \n",__FUNCTION__,ptDmaData->nChnId,_HwFramesToBytes(ptRuntimeData,tFrames),_HwFramesToBytes(ptRuntimeData,nHwoff));
    return 0;
}

static BOOL _DrvAudAoAmpEnable(BOOL bEnable, S8 s8Ch);

static BOOL _DrvAudAoAmpEnable(BOOL bEnable, S8 s8Ch)
{
    if ( bEnable == TRUE )
    {
        //enable gpio for line-out, should after atop enable
        DrvAudApiAoAmpEnable(bEnable, s8Ch);
    }
    else
    {
        //disable gpio for line-out, should before atop disable
        DrvAudApiAoAmpEnable(bEnable, s8Ch);
    }

    return TRUE;
}

void DrvAudInit(void)
{
    u16 n;

    //printf("%s\n",__FUNCTION__);

    // Get DTS info
    //DrvAudApiDtsInit();

    // IRQ init
    //DrvAudApiIrqInit();

    //
    //DrvAudApiIrqSetDmaStopCb(_AudDmaStopChannel);

    // Callback
    HalAudSetAoAmpCb( _DrvAudAoAmpEnable );

    //
    DrvAudApiAoAmpCtrlInit();

    // Hal
    //HalBachSysSetBankBaseAddr(DrvAudApiIoAddress(0x1F000000));
    HalBachSysSetBankBaseAddr(0x1F000000);
    HalAudMainInit();

    // Software init
    for(n=0; n<AUD_I2S_IF_NUM; n++)
    {
        _gbI2sMaster[n]=TRUE; // ???
    }

    for(n=0; n<AUD_DMA_NUM; n++)
    {
        memset(&_gaRuntimeData[n], 0, sizeof(PcmRuntimeData_t));
        _gaRuntimeData[n].nStatus= SND_PCM_STATE_INIT;
        //CamOsSpinInit(&_gaRuntimeData[n].tPcmLock);
        //CamOsTsemInit(&_gaRuntimeData[n].tSleep, 0);
        if(n<AUD_DMA_READER1)
        {
            _gaRuntimeData[n].eStream = PCM_STREAM_CAPTURE;
        }
        else
        {
            _gaRuntimeData[n].eStream = PCM_STREAM_PLAYBACK;
        }

        HalAudDmaIntEnable(n, FALSE, FALSE);
    }
}

void DrvAudDeInit(void)
{
    HalAudMainDeInit();
}

//To-Do : MISC I2s slave mode with MCLK
BOOL DrvAudConfigI2s(AudI2sIf_e eI2sIf, AudI2sCfg_t *ptI2sCfg)
{
    AudI2s_e eI2s;
    BOOL bRet = TRUE;
    int nI2sIf = 0;

    //printf("%s I2sIf =%d\n",__FUNCTION__,eI2sIf);

    // ------------------------
    // Get Pre info.
    // ------------------------
    nI2sIf = (int)eI2sIf;

    if ( nI2sIf >= card.num_i2sIfs )
    {
        printf("In function %s, I2S If Id = %d, Error !\n",__FUNCTION__, nI2sIf);
        return FALSE;
    }

    eI2s = (AudI2s_e)card.i2sIfs[nI2sIf].info->i2s;

    //
    bRet &= HalAudI2sSetWireMode(eI2s, ptI2sCfg->eWireMode);
    bRet &= HalAudI2sSetFmt(eI2s, ptI2sCfg->eFormat);
    bRet &= HalAudI2sSetTdmMode(eI2s, ptI2sCfg->eMode);
    bRet &= HalAudI2sSetMsMode(eI2s,ptI2sCfg->eMsMode);
    bRet &= HalAudI2sSetWidth(eI2s, ptI2sCfg->eWidth);
    bRet &= HalAudI2sSetChannel(eI2s, ptI2sCfg->nChannelNum);
    bRet &= HalAudI2sSetMck(eI2s, ptI2sCfg->eMck);
    bRet &= HalAudI2sSetTdmDetails(eI2s);

    // Private Callback
    if ( card.i2sIfs[nI2sIf].op != NULL )
    {
        if ( ( card.i2sIfs[nI2sIf].op->CfgI2sIfPrivateCb != NULL ) && ( ptI2sCfg->pPriData != NULL ) )
        {
            bRet &= card.i2sIfs[nI2sIf].op->CfgI2sIfPrivateCb( ptI2sCfg->pPriData );
        }
    }

    // Set I2S Rate, must be after HalAudI2sSetWidth & HalAudI2sSetChannel, if you need to set bck, you need width and channel info.
    if ( ptI2sCfg->eMsMode == AUD_I2S_MSMODE_MASTER )
    {
        bRet &= HalAudI2sSetRate( eI2s, ptI2sCfg->eRate );

        _gbI2sMaster[eI2sIf] = TRUE;
    }
    else
    {
        _gbI2sMaster[eI2sIf] = FALSE;
    }

    return bRet;
}

BOOL DrvAudEnableI2s(AudI2sIf_e eI2sIf, BOOL bEnable)
{
    AudI2s_e eI2s;
    BOOL bRet = TRUE;
    AudPadMux_e ePadMux;
    int nI2sIf = 0;

    //printf("%s I2s[%d] = %d\n",__FUNCTION__,eI2sIf,bEnable);

    // ------------------------
    // Get Pre info.
    // ------------------------
    nI2sIf = (int)eI2sIf;

    if ( nI2sIf >= card.num_i2sIfs )
    {
        printf("In function %s, I2S If Id = %d, Error !\n",__FUNCTION__, nI2sIf);
        return FALSE;
    }

    eI2s = (AudI2s_e)card.i2sIfs[nI2sIf].info->i2s;
    ePadMux = (AudPadMux_e)card.i2sIfs[nI2sIf].info->pad_mux;

    // PadMux On/Off
    bRet &= HalAudSetPadMux(ePadMux, bEnable);

    // MCK
    bRet &= HalAudI2sEnableMck(eI2s, bEnable);

    // Bck Enable/Disable
    if ( _gbI2sMaster[eI2sIf] == TRUE )
    {
        bRet &= HalAudI2sEnable(eI2s, bEnable);
    }

    return bRet;
}

//mux have to be configured before dma rate
BOOL DrvAudSetMux(AudMux_e eMux, u8 nChoice)
{
    //printf("%s Mux[%d] = %d\n",__FUNCTION__,eMux,nChoice);

    if(HalAudSetMux(eMux, nChoice))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL DrvAudMonoEnable(AudDmaChn_e eDmaChannel, BOOL bEnable)
{
    if(HalAudDmaMonoEnable(eDmaChannel, bEnable))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL DrvAudEnableAtop(AudAtopPath_e eAtop, BOOL bEnable)
{
    //printf("%s Atop[%d] = %d\n",__FUNCTION__,eAtop,bEnable);
    if(bEnable)
    {
        return HalAudAtopOpen(eAtop);
    }
    else
    {
        return HalAudAtopClose(eAtop);
    }
}

BOOL DrvAudConfigDmaParam(AudDmaChn_e eDmaChannel, DmaParam_t *ptParam )
{
    AudBitWidth_e eBitWidth;
    BOOL bRet =TRUE;
    const struct audio_info_mch *info_mch = (const struct audio_info_mch *)ptParam->private;
    u32 nPeriodSize = 0;
    int nNumMchCh = 0;
    int i = 0;
    const int *mch_sel = NULL;

    //printf("%s chn = %d,status %d\n",__FUNCTION__,eDmaChannel,_gaRuntimeData[eDmaChannel].nStatus);
    if(_gaRuntimeData[eDmaChannel].nStatus!= SND_PCM_STATE_INIT
            && _gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_SETUP)
    {
        printf("%s error status\n",__FUNCTION__);
        return FALSE;
    }

    switch(ptParam->nBitWidth)
    {
        case 16:
            eBitWidth=AUD_BITWIDTH_16;
            break;
        case 32:
            eBitWidth=AUD_BITWIDTH_32;
            break;
        default:
            printf("%s bitwidth not support\n",__FUNCTION__);
            return FALSE;
    }

    if(eDmaChannel>=AUD_DMA_READER1)
    {
        nPeriodSize = (ptParam->nPeriodSize);
    }
    else // Writer "overrun threshold" should add dma local buffer size !
    {
        nPeriodSize = ( (ptParam->nPeriodSize)?(ptParam->nPeriodSize+DMA_LOCALBUF_SIZE):(ptParam->nPeriodSize) );
    }

    //
    nMchExt[eDmaChannel] = ptParam->nChannels;

    bRet = _AudDmaInitChannel(eDmaChannel,ptParam->pDmaBuf,ptParam->nPhysDmaAddr,ptParam->nBufferSize,ptParam->nChannels,eBitWidth,nPeriodSize);

    // mch
    if ( info_mch != NULL )
    {
        if ( ( info_mch->mch_bind != NULL ) && ( info_mch->num_mch_bind > 0 ) )
        {
            for ( i = 0; i < info_mch->num_mch_bind; i++ )
            {
                if ( ptParam->nChannels == info_mch->mch_bind[i].para_dma_ch )
                {
                    nNumMchCh = info_mch->mch_bind[i].num_mch_ch;
                    mch_sel = info_mch->mch_bind[i].mch_sel;

                    if(ptParam->nChannels == 1)
                    {
                        nMchExt[eDmaChannel] = nNumMchCh/2;
                    }
                    else
                    {
                        nMchExt[eDmaChannel] = nNumMchCh;
                    }

                    break;
                }
            }

            if ( i >= info_mch->num_mch_bind )
            {
                printf("in %s, DMA channel = %d, no find matched mch_bind ! ptParam->nChannels = %d\n",__FUNCTION__,eDmaChannel, ptParam->nChannels );
                return FALSE;
            }
        }
        else
        {
            printf("in %s, DMA channel = %d, no define mch_bind !\n",__FUNCTION__, eDmaChannel);
            return FALSE;
        }

        bRet &= HalAudDmaWrConfigMch( (AudMchSet_e)info_mch->mch_set,
                                        nNumMchCh,
                                        (AudMchClkRef_e)info_mch->mch_clk_ref,
                                        mch_sel );
    }

    if(bRet)
    {
        memcpy(&_gaDmaParam[eDmaChannel],ptParam,sizeof(DmaParam_t));

        _gaRuntimeData[eDmaChannel].nFrameBits = nMchExt[eDmaChannel] * ptParam->nBitWidth;
        _gaRuntimeData[eDmaChannel].nHwFrameBits = _gaRuntimeData[eDmaChannel].nFrameBits;
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_SETUP;
    }

    return bRet;
}

BOOL DrvAudOpenDma(AudDmaChn_e eDmaChannel)
{
    PcmDmaData_t *ptPcmData;
    u8 szName[20];
    AudRate_e eSampleRate;
    BOOL bRet =TRUE;
    //printf("in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_SETUP)
    {
        printf("%s status wrong\n",__FUNCTION__);
        return FALSE;
    }

    //ptPcmData = CamOsMemCalloc(1, sizeof(PcmDmaData_t));
    ptPcmData = calloc(1, sizeof(PcmDmaData_t));
    if (ptPcmData== NULL)
    {
        printf("%s insufficient resource\n",__FUNCTION__);
        return FALSE;
    }

    snprintf((char*)szName,sizeof(szName),"aio_dma%d",(s16)eDmaChannel);
    memcpy(ptPcmData->szName, szName, strlen((char*)szName));
    ptPcmData->nChnId = eDmaChannel;
    ptPcmData->pDmaArea = _gaDmaParam[eDmaChannel].pDmaBuf;
    ptPcmData->tDmaAddr = _gaDmaParam[eDmaChannel].nPhysDmaAddr;
    ptPcmData->nBufBytes = _gaDmaParam[eDmaChannel].nBufferSize;
    ptPcmData->tBufSize = _BytesToHwFrames(&_gaRuntimeData[eDmaChannel], _gaDmaParam[eDmaChannel].nBufferSize);
    ptPcmData->tPeriodSize = _BytesToHwFrames(&_gaRuntimeData[eDmaChannel] ,_gaDmaParam[eDmaChannel].nPeriodSize);
    ptPcmData->tApplPtr = 0;

    memset(ptPcmData->pDmaArea,0 ,ptPcmData->nBufBytes);
    //CamOsSpinInit(&ptPcmData->tLock);
    _gaRuntimeData[eDmaChannel].pPrivateData = ptPcmData;

    //if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    //{
    //    if (DrvAudApiIrqRequest( (const char *)ptPcmData->szName, (void *)&_gaRuntimeData[eDmaChannel] ) == FALSE)
    //    {
    //        return FALSE;
    //    }
    //}

    switch(_gaDmaParam[eDmaChannel].nSampleRate)
    {
        case 8000:
            eSampleRate=AUD_RATE_8K;
            break;
        case 11000:
            eSampleRate=AUD_RATE_11K;
            break;
        case 12000:
            eSampleRate=AUD_RATE_12K;
            break;
        case 16000:
            eSampleRate=AUD_RATE_16K;
            break;
        case 22000:
            eSampleRate=AUD_RATE_22K;
            break;
        case 24000:
            eSampleRate=AUD_RATE_24K;
            break;
        case 32000:
            eSampleRate=AUD_RATE_32K;
            break;
        case 44000:
            eSampleRate=AUD_RATE_44K;
            break;
        case 48000:
            eSampleRate=AUD_RATE_48K;
            break;
        default:
            printf("%s eSampleRate unknown !\n",__FUNCTION__);
            return FALSE;
    }

    bRet &= _AudDmaSetRate(eDmaChannel,eSampleRate);

    if(!bRet)
        return bRet;

    switch(_gaDmaParam[eDmaChannel].nBitWidth)
    {
        case 16:
            if(_gaDmaParam[eDmaChannel].nInterleaved)
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyI16;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyI16;
            }
            else
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyN16;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyN16;
            }
            break;
        case 32:
        default:
            if(_gaDmaParam[eDmaChannel].nInterleaved)
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyI32;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyI32;
            }
            else
            {
                if(_gaRuntimeData[eDmaChannel].eStream == PCM_STREAM_PLAYBACK)
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaRdCopyN32;
                else
                    _gaRuntimeData[eDmaChannel].copy = _AudDmaWrCopyN32;
            }
            break;

    }

    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_OPEN;

    return TRUE;
}

BOOL DrvAudPrepareDma(AudDmaChn_e eDmaChannel)
{
    PcmDmaData_t *ptPcmData = (PcmDmaData_t*)_gaRuntimeData[eDmaChannel].pPrivateData;
    BOOL bRet = TRUE;

    //printf("in %s\n",__FUNCTION__);

    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_OPEN
            && _gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_XRUN )
    {
        printf("%s status error!\n",__FUNCTION__);
        return FALSE;
    }

    bRet &= HalAudDmaReset(eDmaChannel);

    //_gaRuntimeData[eDmaChannel].nDmaLevelCount = 0;
    _gaRuntimeData[eDmaChannel].nRemainCount = 0;
    _gaRuntimeData[eDmaChannel].nState = DMA_INIT;

    //ptPcmData->tHwPtr = ptPcmData->tApplPtr = 0;
    ptPcmData->tApplPtr = 0;
    memset(ptPcmData->pDmaArea,0 ,ptPcmData->nBufBytes);

    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_PREPARED;

    return bRet;
}



BOOL DrvAudStartDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    //printf("in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_PREPARED)
    {
        printf("%s error status %d\n",__FUNCTION__,_gaRuntimeData[eDmaChannel].nStatus);
        return FALSE;
    }
    bRet = _AudDmaStartChannel(eDmaChannel);
    if(bRet)
    {
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_RUNNING;
    }

    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    {
        //HalAudDmaIntEnable(eDmaChannel,TRUE,FALSE);
        HalAudDmaIntEnable(eDmaChannel,FALSE,TRUE);
    }


    return bRet;
}

BOOL DrvAudStopDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    //printf("in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    {
        bRet = HalAudDmaIntEnable(eDmaChannel,FALSE,FALSE);
    }
    bRet &= _AudDmaStopChannel(eDmaChannel);


    if(bRet)
    {
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_OPEN;
    }
    return bRet;
}

BOOL DrvAudPauseDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    //printf("in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_RUNNING)
    {
        return FALSE;
    }
    else
    {
        if((bRet = HalAudDmaPause(eDmaChannel)))
        {
            _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_PAUSED;
            return TRUE;
        }
        return bRet;
    }

}

BOOL DrvAudResumeDma(AudDmaChn_e eDmaChannel)
{
    BOOL bRet = TRUE;
    //printf("in %s\n",__FUNCTION__);
    if(_gaRuntimeData[eDmaChannel].nStatus != SND_PCM_STATE_PAUSED)
    {
        return FALSE;
    }
    else
    {
        if((bRet = HalAudDmaResume(eDmaChannel)))
        {
            _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_RUNNING;
            return TRUE;
        }
        return bRet;
    }
}

BOOL DrvAudCloseDma(AudDmaChn_e eDmaChannel)
{
    //printf("in %s\n",__FUNCTION__);
    //if(_gaRuntimeData[eDmaChannel].bEnableIrq)
    //    DrvAudApiIrqFree((void *)&_gaRuntimeData[eDmaChannel]);

    if(_gaRuntimeData[eDmaChannel].pPrivateData)
    {
        //CamOsMemRelease(_gaRuntimeData[eDmaChannel].pPrivateData);
        free(_gaRuntimeData[eDmaChannel].pPrivateData);
        _gaRuntimeData[eDmaChannel].pPrivateData = NULL;
    }
    _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_INIT;
    return TRUE;
}

BOOL DrvAudIsXrun(AudDmaChn_e eDmaChannel)
{
#if 0
    return (_gaRuntimeData[eDmaChannel].nStatus==SND_PCM_STATE_XRUN);
#else
    BOOL bTrigger=0, bBoundary=0, bLocalData=0;
    BOOL bTriggerInt=0, bBoundaryInt=0;//,bLocalDataInt;

    HalAudDmaGetFlags(eDmaChannel, &bTrigger, &bBoundary, &bLocalData);
    HalAudDmaGetInt(eDmaChannel, &bTriggerInt, &bBoundaryInt);

    if ((bBoundary==TRUE)&&(bBoundaryInt==TRUE))
    {
        _gaRuntimeData[eDmaChannel].nStatus = SND_PCM_STATE_XRUN;
        _gaRuntimeData[eDmaChannel].nState = DMA_EMPTY;
        HalAudDmaIntEnable(eDmaChannel, FALSE, FALSE);

        return 1;
    }

    return 0;
#endif
}


s32 DrvAudWritePcm(AudDmaChn_e eDmaChannel, void *pWrBuffer, u32 nSize, BOOL bBlock)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    s32 nErr=0;
    PcmFrames_t tAvail;
    PcmFrames_t tFreeSize;
    PcmFrames_t tWorkSize;
    PcmFrames_t tOffset=0;
    PcmFrames_t tApplptr;
    PcmFrames_t tBufSize;
    //printf("%s chn=%d\n",__FUNCTION__,eDmaChannel);
    if (nSize == 0)
        return 0;
    else
        tBufSize = _BytesToFrames(&_gaRuntimeData[eDmaChannel],nSize);

    //CamOsSpinLockIrqSave(&_gaRuntimeData[eDmaChannel].tPcmLock);
    switch (_gaRuntimeData[eDmaChannel].nStatus)
    {
        case SND_PCM_STATE_PREPARED:
        case SND_PCM_STATE_RUNNING:
        case SND_PCM_STATE_PAUSED:
            break;
        case SND_PCM_STATE_XRUN:
            nErr = -EPIPE;
            goto _end_unlock;
        default:
            nErr = -EBADFD;
            goto _end_unlock;
    }

    tAvail = _AudPcmPlaybackAvail(ptDmaData);
    //printf("%s tAvail %lu,tBufSize %lu\n",__FUNCTION__,tAvail,tBufSize);
    while(tBufSize>0)
    {
        if(tAvail==0)
        {
            //printf("Function - %s, DMA Id = %d, tAvail Frame = %d\n", __func__, eDmaChannel, tAvail);

            if (!bBlock)
            {
                nErr = -EAGAIN;
                goto _end_unlock;
            }
            //printf("_WaitForAvail\n");
            nErr = _WaitForAvail(&_gaRuntimeData[eDmaChannel], &tAvail);
            if (nErr < 0)
                goto _end_unlock;
            //printf("_WaitForAvail avail %lu\n",tAvail);
        }

        tWorkSize = (tBufSize > tAvail? tAvail : tBufSize);
        tFreeSize = ptDmaData->tBufSize - (ptDmaData->tApplPtr%ptDmaData->tBufSize);

        if(tWorkSize > tFreeSize)
        {
            tWorkSize = tFreeSize;
        }

        //printf("tOffset %ld, tWorkSize %ld, tAvail %ld\n",tOffset,tWorkSize,tAvail);

        if(tWorkSize == 0)
        {
            nErr = -EINVAL;
            goto _end_unlock;
        }

        tApplptr = ptDmaData->tApplPtr%ptDmaData->tBufSize;

        //CamOsSpinUnlockIrqRestore(&_gaRuntimeData[eDmaChannel].tPcmLock);
        //if(tOffset)
        //    printf("tOffset %ld!!!!!\n",tOffset);
        nErr = _AudDmaKernelCopy(&_gaRuntimeData[eDmaChannel], tApplptr, pWrBuffer, nSize, tOffset, tWorkSize);
        //CamOsSpinLockIrqSave(&_gaRuntimeData[eDmaChannel].tPcmLock);

        if(nErr<0)
            goto _end_unlock;

        switch (_gaRuntimeData[eDmaChannel].nStatus)
        {
            case SND_PCM_STATE_XRUN:
                nErr = -EPIPE;
                goto _end_unlock;

            default:
                break;
        }
        tApplptr += tWorkSize;
        if (tApplptr >= ptDmaData->tBufSize)
        {
            tApplptr -= ptDmaData->tBufSize;
        }
        ptDmaData->tApplPtr = tApplptr;

        tOffset += tWorkSize;
        tBufSize -= tWorkSize;
        tAvail -= tWorkSize;
    }

_end_unlock:

    //CamOsSpinUnlockIrqRestore(&_gaRuntimeData[eDmaChannel].tPcmLock);
    return nErr < 0 ? nErr:_FramesToBytes(&_gaRuntimeData[eDmaChannel],tOffset);//nSize;
}

s32 DrvAudReadPcm(AudDmaChn_e eDmaChannel, void *pRdBuffer, u32 nSize, BOOL bBlock, u64 *puAiPts)
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);
    s32 nErr=0;
    PcmFrames_t tAvail;
    PcmFrames_t tFreeSize;
    PcmFrames_t tWorkSize;
    PcmFrames_t tOffset=0;
    PcmFrames_t tApplptr;
    PcmFrames_t tBufSize;
    // For AEC
    //CamOsTimespec_t stTime;
    //u64 uTmpPts, uOs = 0;
    //PcmFrames_t tActualDataLen = 0;
    //u32 nSr = 0;

    //printf("%s chn=%d\n",__FUNCTION__,eDmaChannel);
    if (nSize == 0)
        return 0;
    else
        tBufSize = _BytesToFrames(&_gaRuntimeData[eDmaChannel],nSize);

    //CamOsSpinLockIrqSave(&_gaRuntimeData[eDmaChannel].tPcmLock);
    switch (_gaRuntimeData[eDmaChannel].nStatus)
    {
        case SND_PCM_STATE_PREPARED:
        case SND_PCM_STATE_RUNNING:
        case SND_PCM_STATE_PAUSED:
            break;
        case SND_PCM_STATE_XRUN:
            nErr = -EPIPE;
            goto _end_unlock;
        default:
            nErr = -EBADFD;
            goto _end_unlock;
    }

    tAvail = _AudPcmCaptureAvail(ptDmaData);
#if 0
    // For AEC
    // Cal Ai PTS
    tActualDataLen = _AudPcmCaptureRawAvail(ptDmaData);
    CamOsGetMonotonicTime(&stTime);
    uTmpPts = (stTime.nSec*1000000ULL) + (stTime.nNanoSec/1000);

    nSr = _gaDmaParam[eDmaChannel].nSampleRate;

    if ( nSr != 0 )
    {
        uOs = (u64)( tActualDataLen * 1000000 / nSr );
        if ( uTmpPts < uOs )
        {
            *puAiPts = 0;
        }
        else
        {
            *puAiPts = uTmpPts - uOs;
        }
    }
    else
    {
        *puAiPts = 0;
    }
#endif
    //
    //printf("%s tAvail %lu,tBufSize %lu\n",__FUNCTION__,tAvail,tBufSize);
    while(tBufSize>0)
    {
        if(tAvail==0)
        {
            if (!bBlock)
            {
                nErr = -EAGAIN;
                goto _end_unlock;
            }

            //printf("_WaitForAvail\n");
            nErr = _WaitForAvail(&_gaRuntimeData[eDmaChannel], &tAvail);
            if (nErr < 0)
                goto _end_unlock;
            //printf("_WaitForAvail avail %lu\n",tAvail);
        }

        tWorkSize = (tBufSize > tAvail? tAvail : tBufSize);
        tFreeSize = ptDmaData->tBufSize - (ptDmaData->tApplPtr % ptDmaData->tBufSize);

        if(tWorkSize > tFreeSize)
        {
            tWorkSize = tFreeSize;
        }

        if(tWorkSize == 0)
        {
            nErr = -EINVAL;
            goto _end_unlock;
        }

        tApplptr = ptDmaData->tApplPtr%ptDmaData->tBufSize;

        //CamOsSpinUnlockIrqRestore(&_gaRuntimeData[eDmaChannel].tPcmLock);
        nErr = _AudDmaKernelCopy(&_gaRuntimeData[eDmaChannel], tApplptr, pRdBuffer, nSize, tOffset, tWorkSize);
        //CamOsSpinLockIrqSave(&_gaRuntimeData[eDmaChannel].tPcmLock);

        if(nErr<0)
            goto _end_unlock;

        switch (_gaRuntimeData[eDmaChannel].nStatus)
        {
            case SND_PCM_STATE_XRUN:
                nErr = -EPIPE;
                goto _end_unlock;

            default:
                break;
        }
        tApplptr += tWorkSize;
        if (tApplptr >= ptDmaData->tBufSize)
        {
            tApplptr -= ptDmaData->tBufSize;
        }
        ptDmaData->tApplPtr = tApplptr;
        tOffset += tWorkSize;
        tBufSize -= tWorkSize;
        tAvail -= tWorkSize;
    }

_end_unlock:

    //CamOsSpinUnlockIrqRestore(&_gaRuntimeData[eDmaChannel].tPcmLock);
    return nErr < 0 ? nErr:_FramesToBytes(&_gaRuntimeData[eDmaChannel],tOffset);//nSize;
}

BOOL DrvAudSetAdcGain(AudAdcSel_e eAdc, U16 nSel, S8 s8Ch)
{
    return HalAudAtopSetAdcGain(eAdc, nSel, s8Ch);
}

BOOL DrvAudSetMicAmpGain(AudAdcSel_e eAdc, U16 nSel, S8 s8Ch)
{
    return HalAudAtopMicAmpGain(eAdc, nSel, s8Ch);
}

BOOL DrvAudAdcSetMux(AudAdcSel_e eAdcSel)
{
    return HalAudAtopSetAdcMux(eAdcSel);
}

BOOL DrvAudDpgaSetGain(AudDpga_e eDpga, S8 s8Gain, S8 s8Ch)
{
    return HalAudDpgaSetGain(eDpga, s8Gain, s8Ch);
}

BOOL DrvAudDpgaSetGainFading(AudDpga_e eDpga, U8 nFading, S8 s8Ch)
{
    return HalAudDpgaSetGainFading(eDpga, nFading, s8Ch);
}

BOOL DrvAudConfigDigMicParam(DigMicParam_t *ptDigMicParam)
{
    BOOL bRet = TRUE;
    AudRate_e eSampleRate;

    switch(ptDigMicParam->nSampleRate)
    {
        case 8000:
            eSampleRate=AUD_RATE_8K;
            break;
        case 16000:
            eSampleRate=AUD_RATE_16K;
            break;
        case 32000:
            eSampleRate=AUD_RATE_32K;
            break;
        case 48000:
            eSampleRate=AUD_RATE_48K;
            break;
        default:
            printf("%s eSampleRate unknown !\n",__FUNCTION__);
            return FALSE;
    }

    bRet &= HalAudDigMicSetRate(eSampleRate);

    bRet &= HalAudDigMicSetChannel(ptDigMicParam->nChannels);

    return bRet;
}

BOOL DrvAudDigMicEnable(BOOL bEn)
{
    BOOL bRet = TRUE;
    AudPadMux_e ePadMux;

    //printf("in %s\n",__FUNCTION__);

    // ------------------------
    // Get Pre info.
    // ------------------------
    ePadMux = (AudPadMux_e)card.dmics[0].info->pad_mux;

    // PadMux On/Off
    bRet &= HalAudSetPadMux(ePadMux, bEn);

    bRet &= HalAudDigMicEnable(bEn);

    return bRet;
}

BOOL DrvAudSetDigMicGain(S8 s8Gain, S8 s8Ch)
{
    BOOL bRet = TRUE;
    //printf("in %s\n",__FUNCTION__);

    bRet &= HalAudDigMicSetGain(s8Gain, s8Ch);

    return bRet;
}

BOOL DrvAudGetBufCurrDataLen( AudDmaChn_e eDmaChannel, u32 *len )
{
    //unsigned long nFlags;
    u32 nCurDmaLevelCount = 0;
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);

    //CamOsSpinLockIrqSave(&ptDmaData->tLock);
    nCurDmaLevelCount = HalAudDmaGetLevelCnt(ptDmaData->nChnId);
    //CamOsSpinUnlockIrqRestore(&ptDmaData->tLock);

    *len = nCurDmaLevelCount;

    return TRUE;
}

BOOL DrvAudConfigAecParam(AecParam_t *ptAecParam)
{
    BOOL bRet = TRUE;
    AudRate_e eSampleRate;

    switch(ptAecParam->nSampleRate)
    {
        case 8000:
            eSampleRate=AUD_RATE_8K;
            break;
        case 16000:
            eSampleRate=AUD_RATE_16K;
            break;
        case 32000:
            eSampleRate=AUD_RATE_32K;
            break;
        case 48000:
            eSampleRate=AUD_RATE_48K;
            break;
        default:
            printf("%s eSampleRate unknown !\n",__FUNCTION__);
            return FALSE;
    }

    bRet = HalAudSrcSetRate(eSampleRate);

    return bRet;
}

BOOL DrvAudGetStartPts( AudDmaChn_e eDmaChannel, u64 *pnPts )
{
    PcmDmaData_t *ptDmaData = (PcmDmaData_t *)(_gaRuntimeData[eDmaChannel].pPrivateData);

    if ( _gaRuntimeData[eDmaChannel].pPrivateData == NULL )
    {
        return FALSE;
    }

    *pnPts = ptDmaData->tAecInfo.nStartPts;

    return TRUE;
}

BOOL DrvAudDmaSineGen(AudDmaChn_e eDmaChannel, BOOL bEnable, U8 freq, U8 gain)
{
    if(HalAudDmaSineGen(eDmaChannel, bEnable, freq, gain))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL DrvAudEnableHdmi(BOOL bEnable)
{
    BOOL bRet = TRUE;

    //printf("in %s\n",__FUNCTION__);

    bRet &= HalAud2Hdmi(bEnable);

    return bRet;
}

BOOL DrvAudEnableAmp(BOOL bEnable, S8 s8Ch)
{
    BOOL bRet = TRUE;

    //printf("in %s\n",__FUNCTION__);

    bRet &= DrvAudApiAoAmpEnable(bEnable, s8Ch);

    return bRet;
}
