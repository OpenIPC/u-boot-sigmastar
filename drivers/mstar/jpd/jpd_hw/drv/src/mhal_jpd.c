/*
 * mhal_jpd.c- Sigmastar
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

#include "mhal_common.h"
#include "mhal_jpd.h"

#include "drv_jpd_dev.h"
#include "drv_jpd_st.h"
#include "drv_jpd_ctx.h"
#include "drv_jpd_vdi.h"
#ifdef NJPD_CMDQ_SUPPORT
#include "mhal_cmdq.h"
#endif

#include "hal_jpd_def.h"
#include "hal_jpd_ops.h"

#if !defined(JPD_SUPPORT)

MS_S32 MHAL_JPD_CreateDevice(MS_U32 u32DevId, MHAL_JPD_DEV_HANDLE *ppstDevHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_DestroyDevice(MHAL_JPD_DEV_HANDLE pstDevHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_SetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam)
{
    return 0;
}

MS_S32 MHAL_JPD_GetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam)
{
    return 0;
}

MS_S32 MHAL_JPD_CreateInstance(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_INST_HANDLE *ppstInstHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_DestroyInstance(MHAL_JPD_INST_HANDLE pstInstHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_ResetDev(MHAL_JPD_DEV_HANDLE pstDevHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_SetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd, MHAL_JPD_Param_t *pstParam)
{
    return 0;
}

MS_S32 MHAL_JPD_GetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd, MHAL_JPD_Param_t *pstParam)
{
    return 0;
}

MS_S32 MHAL_JPD_ExtractJpgInfo(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_InputBuf_t *pstInputBuf,
                               MHAL_JPD_JpgInfo_t *pstJpgInfo)
{
    return 0;
}

MS_S32 MHAL_JPD_StartDecodeOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_JpgFrame_t *pstJpgFrame)
{
    return 0;
}

MS_S32 MHAL_JPD_BarrierOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle)
{
    return 0;
}

MS_S32 MHAL_JPD_CheckDecodeStatus(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_Status_e *peDecodeStatus)
{
    return 0;
}

MS_S32 MHAL_JPD_Decodetimes(MHAL_JPD_INST_HANDLE pstInstHandle)
{
    return 0;
}
#else

#define INTERBUFSIZE     (0x200000) /**<JPD Interbuf defalut size */
#define JPD_RESET_TEST   (0)        /**<JPD FPGA Verfy case */
#define JPD_CRC32_ENABLE (0)

#if JPD_RESET_TEST
// for jpd FPGA Case Test
static MS_U32 g_u32RST_CMD = 0;
#define JPD_MIF_RESET_TEST(hInst, Cnt)                                                                   \
    do                                                                                                   \
    {                                                                                                    \
        if (g_u32RST_CMD == Cnt)                                                                         \
        {                                                                                                \
            g_u32RST_CMD++;                                                                              \
            MHAL_JPD_ResetDev(hInst, NULL);                                                              \
            ApiJpdExit(hInst);                                                                           \
            JPD_LOGW("\n[notice]*******************REST AND EXIT %d******************\n", g_u32RST_CMD); \
            return -1;                                                                                   \
        }                                                                                                \
        JPD_LOGW("\n[notice]*******************REST func******************\n");                          \
    } while (0)

#endif

#if defined(JPD_UT_TEST) // ut run
typedef struct _performance_stati
{
    u64             total_time[2];
    u32             total_frame[2];
    u32             av_time[2];
    CamOsTimespec_t start[2];
} performance_stati;

static performance_stati JpdDoneStart[MAX_JPD_DEVICE_NUM];

static u64 _MdrvJpdOSTimespecDiffNs(CamOsTimespec_t *start, CamOsTimespec_t *stop)
{
    return (u64)(stop->nSec - start->nSec) * 1000000000 + stop->nNanoSec - start->nNanoSec;
}
#endif

Jpd_AddrBuffer_t g_stJpdInterbuf[MAX_JPD_DEVICE_NUM];

static unsigned int _MdrvJpdDevHandle2DevId(JPD_Dev_t *pstDev)
{
    unsigned int u32Ret = 0;

    if (pstDev == NULL)
    {
        u32Ret = 0;
    }
    else if ((JPD_DEV_HANDLE_MAGIC_NUM != (*(int *)pstDev)) && (*(int *)pstDev) >= 0
             && (*(int *)pstDev) <= MAX_JPD_DEVICE_NUM)
    {
        u32Ret = (*(int *)pstDev);
    }
    else
    {
        u32Ret = pstDev->u32DevId;
    }

    return u32Ret;
}

static MS_S32 _MhalJpdDefaultConfig(JPD_Ctx_t *pCtx)
{
    MS_S32 s32Ret = -1;

    pCtx->stInternalBuf.phyInternalBufAddr = g_stJpdInterbuf[pCtx->pDev->u32DevId].phys_addr;
    pCtx->stInternalBuf.pInternalBufVA     = g_stJpdInterbuf[pCtx->pDev->u32DevId].virt_addr;
    pCtx->stInternalBuf.u32InternalBufSize = g_stJpdInterbuf[pCtx->pDev->u32DevId].size;

    JPD_LOGD("DevId <%d>.\n", pCtx->pDev->u32DevId);

    s32Ret = ApiJpdDefaultConfig(pCtx);
    return s32Ret;
}

static MS_S32 _MhalJpdCheckDecodeParaValid(JPD_Ctx_t *pstInstHandle, MHAL_JPD_JpgFrame_t *pstJpgFrame)
{
    MS_S32 s32Ret = 0;

    unsigned int u32ExpectOutputStride  = 0;
    unsigned int u32ExpectOutputBufSize = 0;

    JPEG_Header_Info_t *  pstHeaderInfo   = &pstInstHandle->stHeadInfo;
    MHAL_JPD_OutputBuf_t *pstOutputBuffer = &pstJpgFrame->stOutputBuf;

#if !defined(JPD_UT_TEST)
    if (pstOutputBuffer->eScaleDownMode != E_MHAL_JPD_SCALE_DOWN_ORG)
    {
        JPD_LOGE("ERROR! Scaling Down Need Verify More, Maybe there are Case not Handled\n");
        s32Ret = -1;
    }
    // HW limitation: ProtectEnable can not mask write request, just make sure that not write data to dram.
    // So given not enough write buffer, will still trigger JPD_W miu hit when ProtectEnable.
    if (pstOutputBuffer->bProtectEnable == 1)
    {
        JPD_LOGE("ERROR! Write Protect Need Verify More, Maybe there are Case not Handled\n");
        s32Ret = -1;
    }
#else
    if (pstOutputBuffer->eScaleDownMode != E_MHAL_JPD_SCALE_DOWN_ORG)
    {
        u32ExpectOutputStride =
            JPD_ALIGN_UP(pstHeaderInfo->u16Image_x_size / (int)pstOutputBuffer->eScaleDownMode, SCALE_DOWN_ALIGNMENT_W)
            * 2;
        if (pstOutputBuffer->eOutputMode == E_MHAL_JPD_OUTPUT_FRAME)
        {
            u32ExpectOutputBufSize =
                u32ExpectOutputStride
                * JPD_ALIGN_UP(pstHeaderInfo->u16Image_y_size / (int)pstOutputBuffer->eScaleDownMode,
                               SCALE_DOWN_ALIGNMENT_H);
        }
        else
        {
            u32ExpectOutputBufSize = u32ExpectOutputStride * pstOutputBuffer->u32LineNum;
        }

        if (pstOutputBuffer->u32BufSize < u32ExpectOutputBufSize)
        {
            JPD_LOGE("Output BufSize is not aligned after scaling! Expected size=0x%x, but get 0x%x\n",
                     u32ExpectOutputBufSize, pstOutputBuffer->u32BufSize);
            s32Ret = -1;
        }
        if (pstOutputBuffer->u32BufStride[0] != u32ExpectOutputStride)
        {
            JPD_LOGE("Output buffer stride is wrong! Expected stride=0x%x, but get 0x%x\n", u32ExpectOutputStride,
                     pstOutputBuffer->u32BufStride[0]);
            s32Ret = -1;
        }
    }
    else
#endif
    {
        // Output format is only yuyv422, so stride is double Image_x_size.
        // If support other output format, stride caculation need adjust.
        u32ExpectOutputStride = pstHeaderInfo->u16Image_x_size * 2;
        if (pstOutputBuffer->eOutputMode == E_MHAL_JPD_OUTPUT_FRAME)
        {
            u32ExpectOutputBufSize = u32ExpectOutputStride * pstHeaderInfo->u16Image_y_size;
        }
        else
        {
            u32ExpectOutputBufSize = u32ExpectOutputStride * pstOutputBuffer->u32LineNum;
        }

        if (pstOutputBuffer->u32BufSize < u32ExpectOutputBufSize)
        {
            JPD_LOGE("Output BufSize is not aligned by input MCU size! Expected size=0x%x, but get 0x%x\n",
                     u32ExpectOutputBufSize, pstOutputBuffer->u32BufSize);
            s32Ret = -1;
        }
        // Stride must be strictly equal to Image_x_size * 2.
        // If not, the data will be disordered and can not disp right image.
        if (pstOutputBuffer->u32BufStride[0] != u32ExpectOutputStride)
        {
            JPD_LOGE("Output buffer stride is wrong! Expected stride=0x%x, but get 0x%x\n", u32ExpectOutputStride,
                     pstOutputBuffer->u32BufStride[0]);
            s32Ret = -1;
        }
    }

    return s32Ret;
}

//------------------------------------------------------------------------------
/// @brief JPD hw init,include clk and common attr
/// @param[in] pstInitParam: Pointer to pstInitParam.
/// @return MHAL_JPD_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_JPD_CreateDevice(MS_U32 u32DevId, MHAL_JPD_DEV_HANDLE *ppstDevHandle)
{
    MS_S32     s32Error     = 0;
    JPD_Dev_t *pJpdDev      = NULL;
    void *     pJpdBankBase = NULL;
    void *     pInterBuffer = NULL;

    JPD_LOGD("DevId <%d>\n", u32DevId);

    JPD_CHECK_VALID_POINTER(ppstDevHandle);
    JPD_CHECK_DEV_ID(u32DevId, MAX_JPD_DEVICE_NUM);

    do
    {
        if (!(pJpdDev = malloc(sizeof(JPD_Dev_t))))
        {
            JPD_LOGE("DevId <%d> alloc device handle faild!\n", u32DevId);
            s32Error = E_MHAL_ERR_NOMEM;
            break;
        }
        memset(pJpdDev, 0x0, sizeof(JPD_Dev_t));

        pJpdDev->s32MagicNum = JPD_DEV_HANDLE_MAGIC_NUM;
        pJpdDev->u32DevId    = u32DevId;

        /*
                if (0 != MdrvJpdOSGetResourceBank(u32DevId, &pJpdBankBase, &u32BankSize))
                {
                    JPD_LOGE("DevId <%d> get bank base failed!\n", u32DevId);
                    s32Error = E_MHAL_ERR_ILLEGAL_PARAM;
                    break;
                }
        */
        pJpdBankBase        = (void *)(0x1f000000 + (0x1622 << 9));
        pJpdDev->BaseDevReg = (unsigned long)pJpdBankBase;
        MapiJpdInitRegBase(u32DevId, (MS_U64)pJpdBankBase);
        MapiJpdInitClk(u32DevId, (MS_U64)(0x1f000000 + (0x1038 << 9)));

        //        MdrvJpdOSInitRegisterBase(u32DevId, pJpdBankBase);

        /*
                if (0 != MdrvJpdOSDeviceClockInit(pJpdDev, u32DevId))
                {
                    JPD_LOGE("DevId <%d> clk init failed!\n", u32DevId);
                    s32Error = E_MHAL_ERR_ILLEGAL_PARAM;
                    break;
                }
                if (0 != MdrvJpdOSDeviceClockOn(pJpdDev, JPD_CLOCK_ON))
                {
                    JPD_LOGE("DevId <%d> clk on failed!\n", u32DevId);
                    s32Error = E_MHAL_ERR_ILLEGAL_PARAM;
                    break;
                }
        */

        // InterBuffer
        pInterBuffer = malloc(INTERBUFSIZE + 64);
        if (!pInterBuffer)
        {
            JPD_LOGE("%s %d::DevId <%d> alloc inter buffer failed!\n", __FUNCTION__, __LINE__, u32DevId);
            s32Error = E_MHAL_ERR_NOBUF;
            break;
        }
        g_stJpdInterbuf[u32DevId].not_aligned_virt_addr = pInterBuffer;
        memset(pInterBuffer, 0, INTERBUFSIZE + 64);
        g_stJpdInterbuf[u32DevId].virt_addr = (void *)JPD_ALIGN_UP((unsigned long)pInterBuffer, 64);
        g_stJpdInterbuf[u32DevId].phys_addr = (unsigned long long)(g_stJpdInterbuf[u32DevId].virt_addr - 0x20000000);
        g_stJpdInterbuf[u32DevId].size      = INTERBUFSIZE;

        JPD_LOGD("DevId <%d>, Interbuf(%llx, %p, %x)\n", u32DevId, g_stJpdInterbuf[u32DevId].phys_addr,
                 g_stJpdInterbuf[u32DevId].virt_addr, g_stJpdInterbuf[u32DevId].size);

        ApiJPDRst(pJpdDev);

        /*
                if (0 != MdrvJPDOSRequestIrq(pJpdDev))
                {
                    JPD_LOGE("DevId <%d> request irq failed!\n", u32DevId);
                    s32Error = E_MHAL_ERR_ILLEGAL_PARAM;
                    break;
                }

                MdrvJpdProcInit(pJpdDev);
        */

        //        CamOsMutexInit(&pJpdDev->m_Mutex);
        //        CamOsTsemInit(&pJpdDev->m_Sem, 1);

#if defined(JPD_UT_TEST)
        memset(&JpdDoneStart[0], 0, sizeof(performance_stati) * MAX_JPD_DEVICE_NUM);
        CamOsGetMonotonicTime(&JpdDoneStart[u32DevId].start[0]);
        CamOsGetMonotonicTime(&JpdDoneStart[u32DevId].start[1]);
#endif

        *ppstDevHandle = (MHAL_JPD_DEV_HANDLE)pJpdDev;

        return MHAL_SUCCESS;
    } while (0);

    if (pJpdDev)
    {
        JPD_LOGE("Abnormal case !!!\n");
        free(pJpdDev);
    }

    if (0 != g_stJpdInterbuf[u32DevId].phys_addr)
    {
        free((void *)g_stJpdInterbuf[u32DevId].phys_addr);
    }

    return s32Error;
}

MS_S32 MHAL_JPD_DestroyDevice(MHAL_JPD_DEV_HANDLE pstDevHandle)
{
    JPD_Dev_t *pJpdDev = (JPD_Dev_t *)pstDevHandle;

    JPD_CHECK_VALID_POINTER(pstDevHandle);
    JPD_CHECK_DEV_ID(pJpdDev->u32DevId, MAX_JPD_DEVICE_NUM);

    JPD_LOGD("DevId <%d>\n", pJpdDev->u32DevId);

    //    MdrvJpdProcDeInit(pJpdDev);
    //    MdrvJpdOSDeviceClockDeinit(pJpdDev);
    /*
        if (pJpdDev->irq)
        {
            CamOsIrqFree(pJpdDev->irq, pJpdDev);
        }
    */
    free((void *)g_stJpdInterbuf[pJpdDev->u32DevId].not_aligned_virt_addr);
    //    MhalJpdVdiResetMmaInfo(pJpdDev->u32DevId);

    memset(pJpdDev, 0x0, sizeof(JPD_Dev_t));

    free(pJpdDev);
    pJpdDev      = NULL;
    pstDevHandle = NULL;

    return MHAL_SUCCESS;
}

MS_S32 MHAL_JPD_SetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam)
{
    MS_S32          s32Error      = 0;
    unsigned int    u32DevId      = 0;
    JPD_MMA_info_t *pstJpdMmaInfo = NULL;
    JPD_Dev_t *     pstDev        = (JPD_Dev_t *)pstDevHandle;

    JPD_CHECK_VALID_POINTER(pstParam);
    u32DevId = _MdrvJpdDevHandle2DevId(pstDev);
    JPD_CHECK_DEV_ID(u32DevId, MAX_JPD_DEVICE_NUM);

    JPD_LOGD("DevId <%d>, eType <%d>\n", u32DevId, eType);

    switch (eType)
    {
        case E_MHAL_JPD_DEV_CONFIG_ISR_CB:
        {
            MHAL_JPD_IsrCallback_t *pstJpdIsrCB = (MHAL_JPD_IsrCallback_t *)pstParam;
            JPD_CHECK_VALID_POINTER(pstDev);
            JPD_CHECK_DEV_MAGIC_NUM(pstDev);
            pstDev->funcIsrCB = pstJpdIsrCB->funcIsrCB;
            break;
        }
        case E_MHAL_JPD_DEV_CONFIG_MMA_CB:
        {
            MHAL_JPD_MMA_CB_t *pstJpdMMACB = (MHAL_JPD_MMA_CB_t *)pstParam;
            if (!pstJpdMMACB->Map || !pstJpdMMACB->Unmap
                || (!pstJpdMMACB->AllocMma || !pstJpdMMACB->FreeMma || !pstJpdMMACB->Flush))
            {
                JPD_LOGE("JPD MMA callback function is NULL!\n");
                return E_MHAL_ERR_NULL_PTR;
            }

            pstJpdMmaInfo                    = MhalJpdVdiGetMmaInfo(u32DevId);
            pstJpdMmaInfo->JpdMMACB.AllocMma = pstJpdMMACB->AllocMma;
            pstJpdMmaInfo->JpdMMACB.FreeMma  = pstJpdMMACB->FreeMma;
            pstJpdMmaInfo->JpdMMACB.Map      = pstJpdMMACB->Map;
            pstJpdMmaInfo->JpdMMACB.Unmap    = pstJpdMMACB->Unmap;
            pstJpdMmaInfo->JpdMMACB.Flush    = pstJpdMMACB->Flush;
            break;
        }
        default:
        {
            JPD_LOGE("Command 0x%x is not support!\n", eType);
            return E_MHAL_ERR_ILLEGAL_PARAM;
        }
    }

    return s32Error;
}

MS_S32 MHAL_JPD_GetDevConfig(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_DevConfig_e eType, MHAL_JPD_Param_t *pstParam)
{
    MHAL_ErrCode_e       eError    = 0;
    unsigned int         u32DevId  = 0;
    JPD_Dev_t *          pstDev    = (JPD_Dev_t *)pstDevHandle;
    MHAL_JPD_ParamInt_t *pstJpdInt = NULL;

    JPD_CHECK_VALID_POINTER(pstParam);
    u32DevId = _MdrvJpdDevHandle2DevId(pstDev);
    JPD_CHECK_DEV_ID(u32DevId, MAX_JPD_DEVICE_NUM);

    JPD_LOGD("DevId <%d>, eType <%d>\n", u32DevId, eType);

    switch (eType)
    {
        case E_MHAL_JPD_DEV_CONFIG_CMDQ_BUF_SIZE:
        {
            pstJpdInt         = (MHAL_JPD_ParamInt_t *)pstParam;

#ifdef NJPD_CMDQ_SUPPORT
            pstJpdInt->u32Val = 0x1800; // per command is 8bytes, this CmdqService max store (0x1800 / 8) = 768 commands
#else
            pstJpdInt->u32Val                   = 0;
#endif
            break;
        }
        case E_MHAL_JPD_DEV_CONFIG_EXTRA_CAPABILITY:
        {
            MHAL_JPD_ExtraCapability_t *pstJpdExtraCapability = NULL;

            if (pstParam == NULL)
            {
                return E_MHAL_ERR_NULL_PTR;
            }

            pstJpdExtraCapability = (MHAL_JPD_ExtraCapability_t *)pstParam;
            memset(pstJpdExtraCapability, 0, sizeof(MHAL_JPD_ExtraCapability_t));

#ifdef NJPD_CMDQ_SUPPORT
            pstJpdExtraCapability->bCmdqSupport = true;
#else
            pstJpdExtraCapability->bCmdqSupport = false;
#endif

            pstJpdExtraCapability->u32MaxDeviceNum = MAX_JPD_DEVICE_NUM;

#if defined(CHIP_I7)
            if (u32DevId == 0 || u32DevId == 1)
            {
                // HW LIMITATION: only jpd 0/1 support imi, and not support jpd 0/1 use imi same time, because SCL rdma
                // only has one set of imi interface.
                pstJpdExtraCapability->bImiSupport = true;
            }
            else
            {
                pstJpdExtraCapability->bImiSupport = false;
            }
#else
            pstJpdExtraCapability->bImiSupport  = true;
#endif

            break;
        }
        default:
            JPD_LOGE("command 0x%x is not support!\n", eType);
            eError = E_MHAL_ERR_ILLEGAL_PARAM;
            break;
    }

    return eError;
}

// create jpd channel
MS_S32 MHAL_JPD_CreateInstance(MHAL_JPD_DEV_HANDLE pstDevHandle, MHAL_JPD_INST_HANDLE *ppstInstHandle)
{
    MHAL_ErrCode_e eError = MHAL_SUCCESS;
    JPD_Ctx_t *    pCtx   = NULL;
    JPD_Dev_t *    pDev   = (JPD_Dev_t *)pstDevHandle;

    JPD_CHECK_VALID_POINTER(pstDevHandle);
    JPD_CHECK_VALID_POINTER(ppstInstHandle);

    JPD_LOGD("DevId <%d>\n", pDev->u32DevId);

    do
    {
        pCtx = MdrvJpdCtxAlloc(pDev);
        if (!pCtx)
        {
            JPD_LOGE("MdrvJpdCtxAlloc fail !!!\n");
            eError = E_MHAL_ERR_NULL_PTR;
            break;
        }
        if (0 > MdrvJpdCtxRegister(pDev, pCtx))
        {
            JPD_LOGE("Fail !!!\n");
            eError = E_MHAL_ERR_ILLEGAL_PARAM;
            break;
        }
        MdrvJpdCtxInitialize(pCtx);

        pDev->nRefCount++; // Total channel count
        *ppstInstHandle = pCtx;
        JPD_CHECK_VALID_POINTER(pCtx);

        JPD_LOGD("success\n");
        return eError;
    } while (0);

    if (pCtx)
    {
        pCtx->CtxRelease((void *)pCtx);
        pCtx = NULL;
    }

    return eError;
}

MS_S32 MHAL_JPD_DestroyInstance(MHAL_JPD_INST_HANDLE pstInstHandle)
{
    JPD_Ctx_t *pCtx = (JPD_Ctx_t *)pstInstHandle;
    JPD_Dev_t *pDev = (JPD_Dev_t *)pCtx->pDev;

    JPD_CHECK_VALID_POINTER(pCtx);
    JPD_CHECK_VALID_POINTER(pDev);

    JPD_LOGD("DevId <%d>, ChnId <%d>\n", pDev->u32DevId, pCtx->s32Index);

    MdrvJpdCtxUnregister(pCtx->pDev, pCtx);

    if ((NULL != pCtx) && (NULL != pDev))
    {
        pCtx->CtxRelease((void *)pCtx);
    }

    JPD_LOGD("success\n");
    pDev->nRefCount--;

    return MHAL_SUCCESS;
}

MS_S32 MHAL_JPD_ResetDev(MHAL_JPD_DEV_HANDLE pstDevHandle)
{
    JPD_Dev_t *pJpdDev = (JPD_Dev_t *)pstDevHandle;

    JPD_CHECK_VALID_POINTER(pstDevHandle);
    JPD_CHECK_DEV_ID(pJpdDev->u32DevId, MAX_JPD_DEVICE_NUM);

    JPD_LOGD("DevId <%d>\n", pJpdDev->u32DevId);

    ApiJPDRst(pJpdDev);

    return MHAL_SUCCESS;
}

//------------------------------------------------------------------------------
/// @brief Dynamic Parameters setting of JPD
/// @param[in]  pstParam: Pointer to pstParam.
/// @return MHAL_JPD_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_JPD_SetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd, MHAL_JPD_Param_t *pstParam)
{
    JPD_Ctx_t *pCtx = (JPD_Ctx_t *)pstInstHandle;

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(pstParam);

    JPD_LOGD("ChnId <%d>, eParamCmd <%d>\n", pCtx->s32Index, eParamCmd);

    switch (eParamCmd)
    {
        case E_JPD_PARAM_MAX_DEC_RES:
        {
            MHAL_JPD_MaxDecRes_t *pstMaxDecRes = (MHAL_JPD_MaxDecRes_t *)pstParam;

            pCtx->stResInfo.jpeg_max_width      = pstMaxDecRes->u16MaxWidth;
            pCtx->stResInfo.jpeg_max_height     = pstMaxDecRes->u16MaxHeight;
            pCtx->stResInfo.jpeg_pro_max_width  = pstMaxDecRes->u16ProMaxWidth;
            pCtx->stResInfo.jpeg_pro_max_height = pstMaxDecRes->u16ProMaxHeight;
            break;
        }
        case E_JPD_PARAM_FPGA_STOP_DECODE: // debug
            pCtx->JPD_Debug = *(MS_S32 *)(pstParam);
            break;
        default:
            JPD_LOGE("Unkwon cmd = 0x%x\n", eParamCmd);
            return E_MHAL_ERR_ILLEGAL_PARAM;
    }

    return MHAL_SUCCESS;
}

MS_S32 MHAL_JPD_GetParam(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_PARAM_CMD_e eParamCmd, MHAL_JPD_Param_t *pstParam)
{
    JPD_Ctx_t *pCtx = (JPD_Ctx_t *)pstInstHandle;

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(pstParam);

    JPD_LOGD("ChnId <%d>, eParamCmd <%d>\n", pCtx->s32Index, eParamCmd);

    switch (eParamCmd)
    {
        case E_JPD_PARAM_MAX_DEC_RES:
        {
            MHAL_JPD_MaxDecRes_t *pstMaxDecRes = (MHAL_JPD_MaxDecRes_t *)pstParam;

            pstMaxDecRes->u16MaxWidth     = pCtx->stResInfo.jpeg_max_width;
            pstMaxDecRes->u16MaxHeight    = pCtx->stResInfo.jpeg_max_height;
            pstMaxDecRes->u16ProMaxWidth  = pCtx->stResInfo.jpeg_pro_max_width;
            pstMaxDecRes->u16ProMaxHeight = pCtx->stResInfo.jpeg_pro_max_height;
            break;
        }
        default:
            JPD_LOGE("Unkwon cmd = 0x%x\n", eParamCmd);
            return E_MHAL_ERR_ILLEGAL_PARAM;
    }
    return MHAL_SUCCESS;
}

//------------------------------------------------------------------------------
/// @brief Parse header information from pstInputBuf
/// @param[in]  pstInputBuf: Pointer to pstInputBuf.
/// @param[out]  pstJpgInfo: Pointer to pstJpgInfo.
/// @return MHAL_JPD_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_JPD_ExtractJpgInfo(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_InputBuf_t *pstInputBuf,
                               MHAL_JPD_JpgInfo_t *pstJpgInfo)
{
    MS_S32          s32Ret  = -1;
    JPD_Ctx_t *     pCtx    = (JPD_Ctx_t *)pstInstHandle;
#if defined(JPD_UT_TEST)
    JPD_Dev_t *     pJpdDev = (JPD_Dev_t *)pCtx->pDev;
    CamOsTimespec_t timer;
    CamOsGetMonotonicTime(&pJpdDev->decode_start_time);
#endif

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(pstInputBuf);
    JPD_CHECK_VALID_POINTER(pstJpgInfo);

    JPD_LOGD("DevId <%d>, ChnId <%d>\n", pCtx->pDev->u32DevId, pCtx->s32Index);

    pCtx->stInBufInfo.pInputBufVA1     = pstInputBuf->pVirtBufAddr1;
    pCtx->stInBufInfo.pInputBufVA2     = pstInputBuf->pVirtBufAddr2;
    pCtx->stInBufInfo.phyBufAddr1      = JPD_MIU_ADDR(pstInputBuf->u64PhyBufAddr1);
    pCtx->stInBufInfo.phyBufAddr2      = JPD_MIU_ADDR(pstInputBuf->u64PhyBufAddr2);
    pCtx->stInBufInfo.u32InputBufSize1 = pstInputBuf->u32BufSize1;
    pCtx->stInBufInfo.u32InputBufSize2 = pstInputBuf->u32BufSize2;
#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 0);
#endif

    s32Ret = _MhalJpdDefaultConfig(pCtx);
    if (0 != s32Ret)
    {
        JPD_LOGE("MHAL_JPD_DefaultConfig Fail !!!\n");
        return E_MHAL_ERR_ILLEGAL_PARAM;
    }
#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 1);
#endif
    s32Ret = ApiDecodeHeaderFirstStage(pCtx); // decode header
    if (0 == s32Ret)
    {
        JPD_LOGE("MApi_JPD_DecodeHdr: Fail !!!\n");
        return E_JPEG_DECODE_ERROR;
    }
#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 2);
#endif
    if (ApiDecodeHdrLastStage(pCtx)
        == E_JPEG_FAILED) // decode header to at sof; get image w h and samp ident frame quant
    {
        ApiJPDRst(pCtx->pDev);
        ApiJpdExit(pCtx);
        return FALSE;
    }

    pstJpgInfo->u16AlignedWidth   = pCtx->stHeadInfo.u16Image_x_size;
    pstJpgInfo->u16AlignedHeight  = pCtx->stHeadInfo.u16Image_y_size;
    pstJpgInfo->u16OriginWidth    = pCtx->stHeadInfo.u16OriImg_x_size;
    pstJpgInfo->u16OriginHeight   = pCtx->stHeadInfo.u16OriImg_y_size;
    pstJpgInfo->u8SampleFactorHor = pCtx->stHeadInfo.u8Comp_h_samp[0];
    pstJpgInfo->u8SampleFactorVer = pCtx->stHeadInfo.u8Comp_v_samp[0];

#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 3);
#endif

#if defined(JPD_UT_TEST)
    CamOsGetMonotonicTime(&timer);
    pCtx->tRCStates[0].total_extra_time +=
        _MdrvJpdOSTimespecDiffNs(&pJpdDev->decode_start_time, &timer); // extre -> this
    pCtx->tRCStates[1].total_extra_time += _MdrvJpdOSTimespecDiffNs(&pJpdDev->decode_start_time, &timer);
#endif
    return MHAL_SUCCESS;
}

//------------------------------------------------------------------------------
/// @brief Push one jpeg frame to decoder and start decoding
/// @param[in]  pstJpgFrame: Pointer to pstJpgFrame.
/// @return MHAL_JPD_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_JPD_StartDecodeOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_JpgFrame_t *pstJpgFrame)
{
    JPD_Ctx_t *     pCtx    = (JPD_Ctx_t *)pstInstHandle;
    EN_JPD_DEC_STEP u8State = E_JPD_STEP_DEC_BEGIN;
#if defined(JPD_UT_TEST)
    CamOsTimespec_t timer;
    CamOsTimespec_t timer_start;
    CamOsGetMonotonicTime(&timer_start);
#endif

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(pstJpgFrame);

#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 4);
#endif
    JPD_LOGD("DevId <%d>, ChnId <%d>\n", pCtx->pDev->u32DevId, pCtx->s32Index);

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(pstJpgFrame);

    if (0 != _MhalJpdCheckDecodeParaValid(pCtx, pstJpgFrame))
    {
        JPD_LOGE("Decode Parameter is invalid!\n");
        return E_MHAL_ERR_ILLEGAL_PARAM;
    }

#ifndef NJPD_CMDQ_SUPPORT
    if (pstJpgFrame->stOutputBuf.pCmdQ)
    {
        JPD_LOGW("MHAL_JPD_StartDecodeOneFrame not support cmdq\n");
    }
#endif

    pCtx->eOutputMode         = pstJpgFrame->stOutputBuf.eOutputMode;
    pCtx->eOutputFmt          = pstJpgFrame->stOutputBuf.eOutputFmt;
    pCtx->u32LineNum          = pstJpgFrame->stOutputBuf.u32LineNum;
    pCtx->bWriteProtectEnable = pstJpgFrame->stOutputBuf.bProtectEnable;
    pCtx->eScaleDownMode      = pstJpgFrame->stOutputBuf.eScaleDownMode;

    pCtx->stInBufInfo.pInputBufVA1     = pstJpgFrame->stInputBuf.pVirtBufAddr1;
    pCtx->stInBufInfo.pInputBufVA2     = pstJpgFrame->stInputBuf.pVirtBufAddr2;
    pCtx->stInBufInfo.phyBufAddr1      = JPD_MIU_ADDR(pstJpgFrame->stInputBuf.u64PhyBufAddr1);
    pCtx->stInBufInfo.phyBufAddr2      = JPD_MIU_ADDR(pstJpgFrame->stInputBuf.u64PhyBufAddr2);
    pCtx->stInBufInfo.u32InputBufSize1 = pstJpgFrame->stInputBuf.u32BufSize1;
    pCtx->stInBufInfo.u32InputBufSize2 = pstJpgFrame->stInputBuf.u32BufSize2;

    pCtx->stOutputBuf.u32Stride[0]        = pstJpgFrame->stOutputBuf.u32BufStride[0];
    pCtx->stOutputBuf.u32Stride[1]        = pstJpgFrame->stOutputBuf.u32BufStride[1];
    pCtx->stOutputBuf.u32Stride[2]        = pstJpgFrame->stOutputBuf.u32BufStride[2];
    pCtx->stOutputBuf.pOutputBufVA[0]     = pstJpgFrame->stOutputBuf.pVirtBufAddr[0];
    pCtx->stOutputBuf.pOutputBufVA[1]     = pstJpgFrame->stOutputBuf.pVirtBufAddr[1];
    pCtx->stOutputBuf.pOutputBufVA[2]     = pstJpgFrame->stOutputBuf.pVirtBufAddr[2];
    pCtx->stOutputBuf.phyOutputBufAddr[0] = JPD_MIU_ADDR(pstJpgFrame->stOutputBuf.u64PhyBufAddr[0]);
    pCtx->stOutputBuf.phyOutputBufAddr[1] = JPD_MIU_ADDR(pstJpgFrame->stOutputBuf.u64PhyBufAddr[1]);
    pCtx->stOutputBuf.phyOutputBufAddr[2] = JPD_MIU_ADDR(pstJpgFrame->stOutputBuf.u64PhyBufAddr[2]);
    pCtx->stOutputBuf.OutputBufSize       = pstJpgFrame->stOutputBuf.u32BufSize;
    pCtx->stOutputBuf.outputRingBufHeight = pstJpgFrame->stOutputBuf.u32RingBufHeight;
    pCtx->stOutputBuf.outputRingTagId     = pstJpgFrame->stOutputBuf.u32RingTagId;

    JPD_LOGD("[notice]InBuf(PA):%llx,(VA)= %p\n", pCtx->stInBufInfo.phyBufAddr1, pCtx->stInBufInfo.pInputBufVA1);
    JPD_LOGD("Output(PA):%llx,(VA)= %p\n", pCtx->stOutputBuf.phyOutputBufAddr[0], pCtx->stOutputBuf.pOutputBufVA[0]);
    JPD_LOGD("Inter(PA):%llx,(VA)= %p\n", pCtx->stInternalBuf.phyInternalBufAddr, pCtx->stInternalBuf.pInternalBufVA);

    pCtx->stInBufInfo.u32InputBufSize1 = ((pCtx->stInBufInfo.u32InputBufSize1 + 0xf) & ~0x0f) * 2;
    // if(pCtx->stInBufInfo.u32InputBufSize1 < JPEG_DEFAULT_EXIF_SIZE)
    {
        // JPD_MSG(E_MHAL_JPD_DBG_DBG, "JPD InputBufSise to short.\n");
        // return E_JPEG_FAILED;
    }
#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 5);
#endif

#if JPD_RESET_TEST
    JPD_MIF_RESET_TEST(pCtx, 6);
#endif

#ifdef NJPD_CMDQ_SUPPORT
    if (pstJpgFrame->stOutputBuf.pCmdQ)
    {
        ApiJpdSetCfgCmdq(pCtx->pDev->u32DevId, pstJpgFrame->stOutputBuf.pCmdQ, pCtx->pu64CmdqBuf);
    }
#endif

    while (u8State == E_JPD_STEP_DEC_BEGIN)
    {
        switch (ApiJpdStartDecode(pCtx))
        {
            case E_JPEG_DONE:
                // This state is only for progressive JPEG file
                JPD_LOGD("Decode_state: DONE\n");
                u8State = E_JPD_STEP_DEC_BEGIN;
                break;
            case E_JPEG_OKAY:
                JPD_LOGD("Decode_state: OKAY\n");
#if JPD_RESET_TEST
                JPD_MIF_RESET_TEST(pCtx, 7);
#endif
                u8State = E_JPD_STEP_WAIT_DONE;
                break;
            case E_JPEG_FAILED:
            default:
                JPD_LOGE("Decode_state: Decode failed\n");
                u8State = E_JPD_STEP_DEC_ERR;
                break;
        }
    }

#ifdef NJPD_CMDQ_SUPPORT
    if (pstJpgFrame->stOutputBuf.pCmdQ)
    {
        MS_U8                      u8Cnt      = 0;
        MS_U32                     u32CmdqRet = 0, u32RegCmdLen = 0;
        MS_U64 *                   pu64CmdqBuf   = NULL;
        MHAL_CMDQ_CmdqInterface_t *pstCmdqHandle = NULL;

        ApiJpdGetCfgCmdq(pCtx->pDev->u32DevId, (void **)&pstCmdqHandle, &pu64CmdqBuf, &u32RegCmdLen);

        if (!pstCmdqHandle || !pu64CmdqBuf || !u32RegCmdLen)
        {
            JPD_LOGE("invalid cmdq cfg, pstCmdqHandle:%p pu64CmdqBuf:%p u32RegCmdLen:%d\n", pstCmdqHandle, pu64CmdqBuf,
                     u32RegCmdLen);

            return E_MHAL_ERR_ILLEGAL_PARAM;
        }

        while (0 == pstCmdqHandle->MHAL_CMDQ_CheckBufAvailable(pstCmdqHandle, u32RegCmdLen))
        {
            u8Cnt++;
            if (u8Cnt == 100)
            {
                JPD_LOGE("CmdQ Check Buf Available failed.\n");
                return E_MHAL_ERR_NOBUF;
            }
        }

        u32CmdqRet = pstCmdqHandle->MHAL_CMDQ_WriteRegCmdqMulti(pstCmdqHandle, (MHAL_CMDQ_MultiCmdBuf_t *)pu64CmdqBuf,
                                                                u32RegCmdLen);
        if (u32CmdqRet != u32RegCmdLen)
        {
            JPD_LOGE("CMDQ_WriteRegCmdqMulti not write all cmd [%d < %d]\n", u32CmdqRet, u32RegCmdLen);
        }
    }
#endif

#if JPD_CRC32_ENABLE
    MdrvJpdOSGetCRC(pCtx->stInBufInfo.pInputBufVA1,
                    pCtx->stHeadInfo.u16Image_x_size * pCtx->stHeadInfo.u16Image_y_size * 2);
#endif

#if defined(JPD_UT_TEST)
    CamOsGetMonotonicTime(&timer);
    pCtx->tRCStates[0].total_oneframe_time += _MdrvJpdOSTimespecDiffNs(&timer_start, &timer);
#endif

    return MHAL_SUCCESS;
}

MS_S32 MHAL_JPD_BarrierOneFrame(MHAL_JPD_INST_HANDLE pstInstHandle)
{
    MS_S32                     s32Error      = 0;
#ifdef NJPD_CMDQ_SUPPORT
    MS_U8                      u8Cnt         = 0;
    JPD_Ctx_t *                pstCtx        = (JPD_Ctx_t *)pstInstHandle;
    MHAL_CMDQ_CmdqInterface_t *pstCmdqHandle = NULL;

    JPD_CHECK_VALID_POINTER(pstCtx);

    JPD_LOGD("DevId <%d>, ChnId <%d>\n", pstCtx->pDev->u32DevId, pstCtx->s32Index);

    ApiJpdGetCfgCmdq(pstCtx->pDev->u32DevId, (void **)&pstCmdqHandle, NULL, NULL);
    if (pstCmdqHandle)
    {
        while (0 == pstCmdqHandle->MHAL_CMDQ_CheckBufAvailable(pstCmdqHandle, 1))
        {
            u8Cnt++;
            if (u8Cnt == 100)
            {
                JPD_LOGE("CmdQ Check Buf Available failed.\n");
                return E_MHAL_ERR_NOBUF;
            }
        }

        if (pstCtx->pDev->u32DevId == 0)
        {
            s32Error = pstCmdqHandle->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdqHandle, E_MHAL_CMDQEVE_JPD0_TRIG);
        }
        else
        {
            s32Error = pstCmdqHandle->MHAL_CMDQ_CmdqAddWaitEventCmd(pstCmdqHandle, E_MHAL_CMDQEVE_JPD1_TRIG);
        }

        ApiJpdSetCfgCmdq(pstCtx->pDev->u32DevId, NULL, NULL);
    }
    else
    {
        JPD_LOGE("CmdQ handle null, not support BarrierOneFrame\n");
        return E_MHAL_ERR_NOT_SUPPORT;
    }
#else
    UNUSED(pstInstHandle);

    JPD_LOGE("MHAL_JPD_BarrierOneFrame not support\n");
    return E_MHAL_ERR_NOT_SUPPORT;
#endif

    return s32Error;
}

//------------------------------------------------------------------------------
/// @brief Get JPD status
/// @param[in]  s32JpdChn: JPD channel ID
/// @param[out]  pstDecodeStatus: Pointer to pstEventFlag.
/// @return MHAL_JPD_ErrCode_e
//------------------------------------------------------------------------------
MS_S32 MHAL_JPD_CheckDecodeStatus(MHAL_JPD_INST_HANDLE pstInstHandle, MHAL_JPD_Status_e *peDecodeStatus)
{
    JPD_Ctx_t *pCtx = (JPD_Ctx_t *)pstInstHandle;

    JPD_CHECK_VALID_POINTER(pstInstHandle);
    JPD_CHECK_VALID_POINTER(peDecodeStatus);

    JPD_LOGD("DevId <%d>, ChnId <%d>\n", pCtx->pDev->u32DevId, pCtx->s32Index);

    *peDecodeStatus = ApiJpdGetEventStatus(pCtx);
    JPD_LOGD("Decode status %x\n", *peDecodeStatus);

    return (*peDecodeStatus & E_MHAL_JPD_STATUS_DONE) ? 0 : 1;
}

#endif
