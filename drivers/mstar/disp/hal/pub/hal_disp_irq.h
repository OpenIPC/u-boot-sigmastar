/*
* hal_disp_irq.h- Sigmastar
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

#ifndef _HAL_DISP_IRQ_H_
#define _HAL_DISP_IRQ_H_

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DISP_IRQ_FPLL_STM_STATBLE(enStm) (enStm == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE)
#define DISP_IRQ_FPLL_STM_FREE(enStm)    (enStm == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE)
#define DISP_IRQ_FPLL_STM_LOCKED(enStm)  (enStm == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED)
#define DISP_IRQ_FPLL_STM_LOCKING(enStm) (enStm == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING)
#define DISP_IRQ_FPLL_STM_DISABLE(enStm) (enStm == E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_DISABLE)

//-------------------------------------------------------------------------------------------------
//  structure & Enum
//-------------------------------------------------------------------------------------------------
typedef enum
{
    E_HAL_DISP_IRQ_IOCTL_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_GET_ID,

    E_HAL_DISP_IRQ_IOCTL_LCD_ENABLE,
    E_HAL_DISP_IRQ_IOCTL_LCD_GET_FLAG,
    E_HAL_DISP_IRQ_IOCTL_LCD_CLEAR,
    E_HAL_DISP_IRQ_IOCTL_LCD_GET_ID,

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
} HalDispIrqIoCtlType_e;

typedef enum
{
    E_HAL_DISP_IRQ_TYPE_NONE                  = 0x00000000,
    E_HAL_DISP_IRQ_TYPE_MOP_VSYNC             = 0x00000001,
    E_HAL_DISP_IRQ_TYPE_MOP_VDE               = 0x00000002,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VDE_NEGATIVE = 0x00000010,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_VS_POSITIVE  = 0x00000020,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_UNLOCK  = 0x00000040,
    E_HAL_DISP_IRQ_TYPE_TIMEZONE_FPLL_LOCKED  = 0x00000080,
    E_HAL_DISP_IRQ_TYPE_LCD_FRM_END           = 0x00000100,
    E_HAL_DISP_IRQ_TYPE_LCD_IDX_RDY           = 0x00000200,
    E_HAL_DISP_IRQ_TYPE_LCD_CMD_RDY           = 0x00000400,
    E_HAL_DISP_IRQ_TYPE_LCD_FLM               = 0x00000800,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_ON            = 0x00010000,
    E_HAL_DISP_IRQ_TYPE_VGA_HPD_OFF           = 0x00020000,
    E_HAL_DISP_IRQ_TYPE_VGA_FIFOFULL          = 0x00040000,
    E_HAL_DISP_IRQ_TYPE_VGA_FIFOEMPTY         = 0x00080000,
    E_HAL_DISP_IRQ_TYPE_CVBS_HPD_ON           = 0x00100000,
    E_HAL_DISP_IRQ_TYPE_CVBS_HPD_OFF          = 0x00200000,
    E_HAL_DISP_IRQ_TYPE_DMA_REGSETFAIL        = 0x01000000,
    E_HAL_DISP_IRQ_TYPE_DMA_FIFOFULL          = 0x02000000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_ON         = 0x04000000,
    E_HAL_DISP_IRQ_TYPE_DMA_ACTIVE_OFF        = 0x08000000,
    E_HAL_DISP_IRQ_TYPE_DMA_REALDONE          = 0x10000000,
    E_HAL_DISP_IRQ_TYPE_DMA_DONE              = 0x20000000,
    E_HAL_DISP_IRQ_TYPE_DMA_MISC              = 0x40000000,
    E_HAL_DISP_IRQ_TYPE_DMA_TRIG_FAIL         = 0x80000000,
} HalDispIrqType_e;
typedef enum
{
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_POSITIVE = 0x00,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VSYNC_NEGATIVE = 0x01,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_POSITIVE   = 0x02,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_VDE_NEGATIVE   = 0x03,
    E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM            = 0x04,
} HalDispIrqTimeStampType_e;

typedef struct
{
    u32 u32IrqFlags;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_TIMESTAMP_NUM];
} HalDispIrqTimeZoneStatus_t;

typedef struct
{
    HalDispIrqIoCtlType_e enIoctlType;
    HalDispIrqType_e      enIrqType;
    void *                pDispCtx;
    void *                pParam;
} HalDispIrqIoctlConfig_t;
typedef struct
{
    u32 u32VsCnt;
    u32 u32ChangeTimingCnt;
    u64 u64TimeStamp;
    u64 u64ChangeTimingTimeStamp;
} HalDispIrqDevWorkingHist_t;
typedef struct
{
    u8 u8Deviceused;
    u8 u8bStartTimegen;
} HalDispIrqDevWorkingStatus_t;
typedef enum
{
    E_HAL_DISP_IRQ_TIMEZONE_VDE_NEGATIVE,
    E_HAL_DISP_IRQ_TIMEZONE_VS_POSTIVE,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_UNLOCK,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_LOCKED,
    E_HAL_DISP_IRQ_TIMEZONE_MAX,
} HalDispIrqTimeZoneType_e;
typedef enum
{
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_DISABLE = 0,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKING,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_LOCKED,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_STATBLE,
    E_HAL_DISP_IRQ_TIMEZONE_FPLL_ST_FREE,
} HalDispIrqTimeZoneFpllStatus_e;
typedef struct
{
    u32                            u32UnLockRstCnt;
    u32                            u32PhaseErrRstCnt;
    u32                            u32FpllLostLockCnt;
    u64                            u64UnLockRstTime;
    u64                            u64PhaseErrRstTime;
    u64                            u64FpllLostLockTime;
    u64                            u64FpllStableTime;
} HalDispIrqFpllWorkingHist_t;
typedef struct
{
    HalDispIrqTimeZoneFpllStatus_e enFpllStm;
    u8                             u8FpllLocked;
    u8                             u8FpllEn;
    u8                             u8FpllLockedIrqEn;
    u8                             u8FpllUnLockIrqEn;
    u32                            u32UnstableFrmCnt;
    u32                            u32PhaseErrCnt;
    u32                            u32LockedFrmCnt;
} HalDispIrqFpllWorkingStatus_t;
typedef struct
{
    u64 u64TimeStamp[E_HAL_DISP_IRQ_TIMEZONE_MAX];
    u64 u64TimeStamp_Vde0;
    u32 u32fps;
    u32 u32VDeDoneCnt;
    u32 u32VSCnt;
    u32 u32FpllUnlockCnt;
    u32 u32FplllockCnt;
} HalDispIrqTimeZoneWorkingHist_t;
typedef struct
{
    u8  u8TimeZoneused;
    u32 u32IrqFlags;
} HalDispIrqTimeZoneWorkingStatus_t;
typedef enum
{
    E_HAL_DISP_IRQ_VGA_HPD_ON,
    E_HAL_DISP_IRQ_VGA_HPD_OFF,
    E_HAL_DISP_IRQ_VGA_FIFOFULL,
    E_HAL_DISP_IRQ_VGA_FIFOEMPTY,
    E_HAL_DISP_IRQ_VGA_MAX,
} HalDispIrqVgaType_e;
typedef struct
{
    u32 u32HPDonCnt;
    u32 u32HPDoffCnt;
    u32 u32affCnt;
    u32 u32afeCnt;
    u32 u32DacRstCnt;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_VGA_MAX];
} HalDispIrqVgaWorkingHist_t;
typedef struct
{
    u8  u8Vgaused;
    u8  u8DacSwEn;
    u8  u8DacMute;
    u8  u8DacRst;
    u32 u32DacBindDev;
} HalDispIrqVgaWorkingStatus_t;
typedef enum
{
    E_HAL_DISP_IRQ_CVBS_HPD_ON,
    E_HAL_DISP_IRQ_CVBS_HPD_OFF,
    E_HAL_DISP_IRQ_CVBS_MAX,
} HalDispIrqCvbsType_e;
typedef struct
{
    u32 u32HPDonCnt;
    u32 u32HPDoffCnt;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_CVBS_MAX];
} HalDispIrqCvbsWorkingHist_t;
typedef struct
{
    u8  u8Cvsbused;
} HalDispIrqCvbsWorkingStatus_t;
typedef struct
{
    u64 u64TimeStamp;
} HalDispIrqLcdWorkingHist_t;
typedef struct
{
    u8  u8Lcdused;
} HalDispIrqLcdWorkingStatus_t;
typedef enum
{
    E_HAL_DISP_IRQ_DMA_REGSETFAIL,
    E_HAL_DISP_IRQ_DMA_FIFOFULL,
    E_HAL_DISP_IRQ_DMA_ACTIVE_ON,
    E_HAL_DISP_IRQ_DMA_REALDONE,
    E_HAL_DISP_IRQ_DMA_TRIGFAIL,
    E_HAL_DISP_IRQ_DMA_TRIG,
    E_HAL_DISP_IRQ_DMA_DOUBLEVSYNC,
    E_HAL_DISP_IRQ_DMA_RST,
    E_HAL_DISP_IRQ_DMA_OVERWRITE,
    E_HAL_DISP_IRQ_DMA_CMDEMPTY,
    E_HAL_DISP_IRQ_DMA_ACTIVETIME,
    E_HAL_DISP_IRQ_DMA_MAX,
} HalDispIrqDmaType_e;
typedef struct
{
    u32 u32FrDoneCnt;
    u32 u32FrActCnt;
    u32 u32FrTrigCnt;
    u32 u32affCnt;
    u32 u32SetFailCnt;
    u32 u32TrigFailCnt;
    u32 u32DoubleVsyncCnt;
    u32 u32DmaRstCnt;
    u32 u32OverWriteCnt;
    u32 u32CmdFIFOEmptyCnt;
    u64 u64TimeStamp[E_HAL_DISP_IRQ_DMA_MAX];
} HalDispIrqDmaWorkingHist_t;
typedef struct
{
    u8 u8Dmaused;
    u8 u8DmaBindDevSrc;
    u8 u8DmaBindDevDst;
    u8 u8bRstWdma;
    u8 u8bDmaIdle;
    u16 u16Crc16Md;
    u32 u32CRC16[3];
} HalDispIrqDmaWorkingStatus_t;
typedef struct
{
    HalDispIrqDevWorkingHist_t      stDevHist[HAL_DISP_DEVICE_MAX];
    HalDispIrqTimeZoneWorkingHist_t stTimeZoneHist[HAL_DISP_DEVICE_MAX];
    HalDispIrqDmaWorkingHist_t      stDmaHist[HAL_DISP_DMA_MAX];
    HalDispIrqVgaWorkingHist_t      stVgaHist;
    HalDispIrqCvbsWorkingHist_t     stCvbsHist;
    HalDispIrqLcdWorkingHist_t      stLcdHist;
    HalDispIrqFpllWorkingHist_t     stFpllHist[HAL_DISP_FPLL_CNT];
} HalDispIrqWorkingHist_t;
typedef struct
{
    HalDispIrqDevWorkingStatus_t      stDevStatus[HAL_DISP_DEVICE_MAX];
    HalDispIrqTimeZoneWorkingStatus_t stTimeZoneStatus[HAL_DISP_DEVICE_MAX];
    HalDispIrqDmaWorkingStatus_t      stDmaStatus[HAL_DISP_DMA_MAX];
    HalDispIrqVgaWorkingStatus_t      stVgaStatus;
    HalDispIrqCvbsWorkingStatus_t     stCvbsStatus;
    HalDispIrqLcdWorkingStatus_t      stLcdStatus;
    HalDispIrqFpllWorkingStatus_t     stFpllStatus[HAL_DISP_FPLL_CNT];
} HalDispIrqWorkingStatus_t;

typedef struct
{
#ifndef DISP_STATISTIC_DISABLE
    HalDispIrqWorkingHist_t      stWorkingHist;
#endif
    HalDispIrqWorkingStatus_t    stWorkingStatus;
} HalDispIrqStatusHist_t;
extern HalDispIrqStatusHist_t        g_stDispIrqHist;
//-------------------------------------------------------------------------------------------------
//  Prototype
//-------------------------------------------------------------------------------------------------
#ifdef _HAL_DISP_IRQ_C_
#define INTERFACE

#else
#define INTERFACE extern
#endif
INTERFACE u8   HalDispIrqIoCtl(HalDispIrqIoctlConfig_t *pCfg);
INTERFACE void HalDispIrqSetDacMute(u8 u8Val);
INTERFACE void HalDispIrqSetFpllEn(u32 u32FpllId, u8 u8bEn);
INTERFACE void HalDispIrqSetDacEn(u32 u32DevId, u8 u8bEn);
INTERFACE void HalDispIrqSetDevIntClr(u32 u32DevId, u16 val);
INTERFACE void HalDispIrqInit(void);

#undef INTERFACE

#endif
