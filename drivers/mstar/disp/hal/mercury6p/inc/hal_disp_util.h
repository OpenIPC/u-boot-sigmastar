/*
* hal_disp_util.h- Sigmastar
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

#ifndef __HAL_DISP_UTIL_H__
#define __HAL_DISP_UTIL_H__

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define RIU_OFFSET           0x1F000000
#define RIU_READ_BYTE(addr)  (READ_BYTE(RIU_OFFSET + (addr)))
#define RIU_READ_2BYTE(addr) (READ_WORD(RIU_OFFSET + (addr)))
#define RIU_READ_4BYTE(addr) (READ_LONG(RIU_OFFSET + (addr)))

#define RIU_WRITE_BYTE(addr, val)  WRITE_BYTE((RIU_OFFSET + (addr)), val)
#define RIU_WRITE_2BYTE(addr, val) WRITE_WORD(RIU_OFFSET + (addr), val)
#define RIU_WRITE_4BYTE(addr, val) WRITE_LONG(RIU_OFFSET + (addr), val)

#define RBYTE(u32Reg) RIU_READ_BYTE((u32Reg) << 1)

#define R2BYTE(u32Reg) RIU_READ_2BYTE((u32Reg) << 1)

#define R2BYTEMSK(u32Reg, u16mask) ((RIU_READ_2BYTE((u32Reg) << 1) & u16mask))

#define R4BYTE(u32Reg) ({ (RIU_READ_4BYTE((u32Reg) << 1)); })

#define R4BYTEMSK(u32Reg, u32Mask) (RIU_READ_4BYTE((u32Reg) << 1) & u32Mask)

#define WBYTE(u32Reg, u8Val) RIU_WRITE_BYTE(((u32Reg) << 1), u8Val)

#define WBYTEMSK(u32Reg, u8Val, u8Mask)              \
    RIU_WRITE_BYTE((((u32Reg) << 1) - ((u32Reg)&1)), \
                   (RIU_READ_BYTE((((u32Reg) << 1) - ((u32Reg)&1))) & ~(u8Mask)) | ((u8Val) & (u8Mask)))

#define W2BYTE(u32Reg, u16Val) RIU_WRITE_2BYTE((u32Reg) << 1, u16Val)

#define W2BYTEMSK(u32Reg, u16Val, u16Mask) \
    RIU_WRITE_2BYTE((u32Reg) << 1, (RIU_READ_2BYTE((u32Reg) << 1) & ~(u16Mask)) | ((u16Val) & (u16Mask)))

#if 0
#define W4BYTE(u32Reg, u32Val)                                                     \
    (                                                                              \
        {                                                                          \
            RIU_WRITE_2BYTE((u32Reg) << 1, ((u32Val)&0x0000FFFF));                 \
            RIU_WRITE_2BYTE(((u32Reg) + 2) << 1, (((u32Val) >> 16) & 0x0000FFFF)); \
        })
#else
#define W4BYTE(u32Reg, u32Val) ({ RIU_WRITE_4BYTE((u32Reg) << 1, u32Val); })
#endif

#define W4BYTEMSK(u32Reg, u32Val, u32Mask) \
    RIU_WRITE_4BYTE((u32Reg) << 1, (RIU_READ_4BYTE((u32Reg) << 1) & ~(u32Mask)) | ((u32Val) & (u32Mask)))

#define W3BYTE(u32Reg, u32Val)                                   \
    (                                                            \
        {                                                        \
            RIU_WRITE_2BYTE((u32Reg) << 1, u32Val);              \
            RIU_WRITE_BYTE((u32Reg + 2) << 1, ((u32Val) >> 16)); \
        })

#define DISP_BIT0  0x00000001
#define DISP_BIT1  0x00000002
#define DISP_BIT2  0x00000004
#define DISP_BIT3  0x00000008
#define DISP_BIT4  0x00000010
#define DISP_BIT5  0x00000020
#define DISP_BIT6  0x00000040
#define DISP_BIT7  0x00000080
#define DISP_BIT8  0x00000100
#define DISP_BIT9  0x00000200
#define DISP_BIT10 0x00000400
#define DISP_BIT11 0x00000800
#define DISP_BIT12 0x00001000
#define DISP_BIT13 0x00002000
#define DISP_BIT14 0x00004000
#define DISP_BIT15 0x00008000
#define DISP_BIT16 0x00010000
#define DISP_BIT17 0x00020000
#define DISP_BIT18 0x00040000
#define DISP_BIT19 0x00080000
#define DISP_BIT20 0x00100000
#define DISP_BIT21 0x00200000
#define DISP_BIT22 0x00400000
#define DISP_BIT23 0x00800000
#define DISP_BIT24 0x01000000
#define DISP_BIT25 0x02000000
#define DISP_BIT26 0x04000000
#define DISP_BIT27 0x08000000
#define DISP_BIT28 0x10000000
#define DISP_BIT29 0x20000000
#define DISP_BIT30 0x40000000
#define DISP_BIT31 0x80000000

#define HAL_DISP_UTILITY_CMDQ_ID_DEVICE0 (0)
#define HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 (1)
#define HAL_DISP_UTILITY_CMDQ_ID_DMA     (2)
#define HAL_DISP_UTILITY_CMDQ_NUM        (3)

#define HAL_DISP_UTILITY_REG_BANK_NUM (E_DISP_UTILITY_REG_CONFIG_NUM) // MAX number of reigster bank
#define HAL_DISP_UTILIYT_REG_NUM      (0x80)
#define HAL_DISP_UTILITY_REG_BANK_SIZE \
    (HAL_DISP_UTILIYT_REG_NUM * 2) // Each bank has 128 register and each 16bits for value
#define HAL_DISP_UTILITY_DIRECT_CMDQ_CNT   (3840)
#define HAL_DISP_UTILITY_CMDQ_CMD_CNT      (32)
#define HAL_DISP_UTILITY_CMDQ_DELAY_CMD    (0xFFFF02)
#define HAL_DISP_UTILITY_CMDQ_WAITDONE_CMD (0xFFFF04)

#define HAL_DISP_UTILITY_FLAG_CMDQ_IDX_EXIST 0x8000
// Todo:Paul wait CMDQ
#define HAL_DISP_IRQ_TOP_MASK_REG(id)                               \
    ((id == HAL_DISP_DEVICE_ID_0)   ? DISP_TOP0_REG_INTR_MASK_GRP_A \
     : (id == HAL_DISP_DEVICE_ID_1) ? DISP_TOP1_REG_INTR_MASK_GRP_A \
                                    : 0)
#define HAL_DISP_IRQ_TOP_FLAG_REG(id)                                 \
    ((id == HAL_DISP_DEVICE_ID_0)   ? DISP_TOP0_REG_INTR_STATUS_GRP_A \
     : (id == HAL_DISP_DEVICE_ID_1) ? DISP_TOP1_REG_INTR_STATUS_GRP_A \
                                    : 0)
#define HAL_DISP_IRQ_TOP_RAWFLAG_REG(id)                                  \
    ((id == HAL_DISP_DEVICE_ID_0)   ? DISP_TOP0_REG_INTR_RAW_STATUS_GRP_A \
     : (id == HAL_DISP_DEVICE_ID_1) ? DISP_TOP1_REG_INTR_RAW_STATUS_GRP_A \
                                    : 0)
#define HAL_DISP_IRQ_TOP_CLEAR_REG(id)                                \
    ((id == HAL_DISP_DEVICE_ID_0)   ? DISP_TOP0_REG_INTR_STATUS_GRP_A \
     : (id == HAL_DISP_DEVICE_ID_1) ? DISP_TOP1_REG_INTR_STATUS_GRP_A \
                                    : 0)

#define HAL_DISP_IRQ_TOP_CLEAR_REG_ALL(id) \
    ((id == HAL_DISP_DEVICE_ID_0) ? DISP_TOP0_REG_SW_RST : (id == HAL_DISP_DEVICE_ID_1) ? DISP_TOP1_REG_SW_RST : 0)
#define HAL_DISP_UTILITY_CMDQDEV_WAIT_DONE_EVENT(id)                        \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? E_MHAL_CMDQEVE_DISP_0_INT \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? E_MHAL_CMDQEVE_DISP_1_INT \
                                                : HAL_DISP_UTILITY_CMDQDMA_WAIT_DONE_EVENT(id))
#define HAL_DISP_UTILITY_CMDQDMA_WAIT_DONE_EVENT(id) \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DMA ? E_MHAL_CMDQEVE_DISP_2_INT : 0)

#define HAL_DISP_UTILITY_DISP_TO_CMDQDEV_RAW(id)                                           \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_CMDQ_INTR_RAW_STATUS_GRP_A \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP1_REG_CMDQ_INTR_RAW_STATUS_GRP_A \
                                                : 0)
#define HAL_DISP_UTILITY_DISP_TO_CMDQDEV_CLEAR(id)                                     \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_CMDQ_INTR_STATUS_GRP_A \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP1_REG_CMDQ_INTR_STATUS_GRP_A \
                                                : 0)
#define HAL_DISP_UTILITY_DISP_TO_CMDQDMA_CLEAR(id) \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DMA ? DISP_CVBS_REG_CMDQ_INTR_STATUS_GRP_A : 0)

#define REG_HAL_DISP_UTILIYT_CMDQDEV_IN_PROCESS(id)                               \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_SOFTWARE_RESERVE0 \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP0_REG_SOFTWARE_RESERVE1 \
                                                : 0)
#define REG_HAL_DISP_UTILIYT_CMDQDEV_TIMEGEN_START(id)                            \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_SOFTWARE_RESERVE0 \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP0_REG_SOFTWARE_RESERVE1 \
                                                : 0)
#define REG_HAL_DISP_UTILIYT_CMDQDMA_IN_PROCESS(id) \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DMA ? DISP_WDMA_REG_SC_DMA_DUMMY0 : 0)
#define REG_CMDQ_PROCESS_FENCE_MSK     (0x0FFF)
#define REG_CMDQ_IN_PROCESS_MSK        (0x8000)
#define REG_CMDQ_IN_PROCESS_ON         (0x8000)
#define REG_CMDQ_IN_PROCESS_OFF        (0x0000)
#define REG_CMDQ_DEV_TIMEGEN_START_MSK (0x4000)

#define REG_HAL_DISP_UTILITY_CMDQ_WAIT_CNT(id)                                    \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_SOFTWARE_RESERVE2 \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP0_REG_SOFTWARE_RESERVE3 \
                                                : 0)

#define REG_CMDQ_WAIT_CNT_MSK   (0xFF00)
#define REG_CMDQ_WAIT_CNT_SHIFT (8)

#define REG_HAL_DISP_UTILITY_CMDQ_FLIP_CNT(id)                                    \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_SOFTWARE_RESERVE4 \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP0_REG_SOFTWARE_RESERVE5 \
                                                : 0)

#define REG_CMDQ_FLIP_CNT_MSK    (0xFF00)
#define REG_CMDQ_FLIPT_CNT_SHIFT (8)

#define HAL_DISP_UTILITY_DISP_TOP_RST(id)                              \
    ((id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE0   ? DISP_TOP0_REG_SW_RST \
     : (id) == HAL_DISP_UTILITY_CMDQ_ID_DEVICE1 ? DISP_TOP1_REG_SW_RST \
                                                : 0)
#define HAL_DISP_UTILITY_DISP_DSI_GATE_EN(id) (DISP_TOP0_REG_DSI_CLK_GATE_EN)
#define REG_WDMA_WRITE_DONE(id)               (DISP_CVBS_REG_INTR_STATUS_GRP_A)

#define HAL_DISP_UTILITY_CMDQ_CNT_MAX  (0xFFFF)
#define HAL_DISP_UTILITY_CNT_ADD(x, y) (x = (x + y) >= HAL_DISP_UTILITY_CMDQ_CNT_MAX ? 1 : (x + y))

#define DISP_DACAFF_STATUS_FULL(status)      (status & mask_of_disp_top0_reg_dac_aff_full_flag)
#define DISP_DACAFF_STATUS_EMPTY(status)     (status & mask_of_disp_top0_reg_dac_aff_empty_flag)
#define DISP_FPLL_LOCK_STATUS_LOST(status)   ((status & mask_of_disp_top0_reg_fpll_locked) == 0)
#define DISP_FPLL_LOCK_STATUS_LOCKED(status) (status & mask_of_disp_top0_reg_fpll_locked)
#define mask_of_reg_cmdq_mux                 (0x0001)
#define shift_of_reg_cmdq_mux                (0)
#define REG_CMDQ_MUX_0                       (DISP_TOP0_REG_SC_TOP_HW_RESERVE)
#define REG_CMDQ_MUX_1                       (DISP_TOP1_REG_SC_TOP_HW_RESERVE)
#define mask_of_reg_flm_count                (0x0f00)
#define shift_of_reg_flm_count               (8)
#define REG_FLM_COUNT                        (DISP_TOP0_REG_SOFTWARE_RESERVE6)
#define mask_of_reg_frame_done_count         (0xf000)
#define shift_of_reg_frame_done_count        (12)
#define REG_FRAME_DONE_COUNT                 ((DISP_TOP0_REG_SOFTWARE_RESERVE6))

//-------------------------------------------------------------------------------------------------
//  Enum
//-------------------------------------------------------------------------------------------------

typedef enum
{
    E_DISP_UTILITY_REG_ACCESS_CPU,
    E_DISP_UTILITY_REG_ACCESS_CMDQ,
    E_DISP_UTILITY_REG_ACCESS_CMDQ_DIRECT,
    E_DISP_UTILITY_REG_ACCESS_NUM,
} HalDispUtilityRegAccessMode_e;
//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    u32 u32Addr; // 16bit Bank addr + 8bit/16bit-regaddr
    u16 u16Data; // 16bit data
    u16 u16Mask; // inverse normal case
} HalDispUtilityCmdReg_t;

typedef struct
{
    u32 u32CmdqBufIdx;
    u32 u32Addr;
    u16 u16Data;
    u16 u16Mask;
    u32 u32Time;
    u8  bPollEq;
} HalDispUtilityCmdqCmd_t;

typedef struct
{
    s32 s32UtilityId;

    void *pvDirectCmdqBuffer;
    u32   u32DirectCmdqCnt;

    void *pvCmdqCmdBuffer;
    u32   u32CmdqCmdCnt;

    void *pvCmdqInf;
    u16   u16RegFlipCnt;
    u16   u16WaitDoneCnt;

    DrvDispOsMutexConfig_t stMutxCfg;
} HalDispUtilityCmdqContext_t;

//-------------------------------------------------------------------------------------------------
// Prototype
//-------------------------------------------------------------------------------------------------

#ifndef __HAL_DISP_UTILITY_C__
#define INTERFACE extern
#else
#define INTERFACE
#endif

INTERFACE u8   HalDispUtilityInit(u32 u32CtxId);
INTERFACE u8   HalDispUtilityDeInit(u32 u32CtxId);
INTERFACE u8   HalDispUtilityGetCmdqContext(void **pvCtx, u32 u32CtxId);
INTERFACE void HalDispUtilitySetCmdqInf(void *pCmdqInf, u32 u32CtxId);

INTERFACE u16  HalDispUtilityR2BYTEDirect(u32 u32Reg);
INTERFACE u16  HalDispUtilityR2BYTEMaskDirect(u32 u32Reg, u16 u16Mask);
INTERFACE u32  HalDispUtilityR4BYTEDirect(u32 u32Reg);
INTERFACE void HalDispUtilityWBYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityWBYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEDirect(u32 u32Reg, u16 u16Val, void *pvCtxIn);
INTERFACE void HaDisplUtilityW2BYTE(u32 u32Reg, u16 u16Val, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKDirect(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKByType(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn,
                                             HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE void HalDispUtilityW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKDirectCmdq(void *pvCtxIn, u32 u32Reg, u16 u16Val, u16 u16Mask);
INTERFACE void HalDispUtilityW2BYTEMSKDirectCmdqWrite(void *pvCtxIn);
INTERFACE void HalDispUtilityKickoffCmdq(void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKDirectAndNoshadow(u32 u32Reg, u16 u16Val, u16 u16Mask);
INTERFACE void HalDispUtilityAddW2BYTEMSK(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE void HalDispUtilityW2BYTEMSKCmdq(u32 u32Reg, u16 u16Val, u16 u16Mask, void *pvCtxIn);
INTERFACE u8   HalDispUtilityAddWaitCmd(void *pvCtxIn);
INTERFACE u8   HalDispUtilityAddDelayCmd(void *pvCtxIn, u32 u32PollingTime);
INTERFACE void HalDispUtilitySetRegAccessMode(u32 u32Id, u32 u32Mode);
INTERFACE u32  HalDispUtilityGetRegAccessMode(u32 u32Id);
INTERFACE u8 HalDispUtilityPollWait(void *pvCmdqInf, u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq);
INTERFACE void HalDispUtilityWritePollCmd(u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq,
                                          void *pvCtxIn);
INTERFACE u8   HalDispUtilityWritePollCmdByType(u32 u32Reg, u16 u16Val, u16 u16Mask, u32 u32PollTime, u8 bPollEq,
                                                void *pvCtxIn, HalDispUtilityRegAccessMode_e enRiuMode);
INTERFACE void HalDispUtilityWriteDelayCmd(u32 u32PollingTime, void *pvCtxIn);
INTERFACE void HalDispUtilityWriteWaitDoneCmd(void *pvCtxIn);

#undef INTERFACE
#endif /* __HAL_DISP_UTIL_H__ */
