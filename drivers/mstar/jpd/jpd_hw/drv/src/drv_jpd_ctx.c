/*
 * drv_jpd_ctx.c- Sigmastar
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

#include "drv_jpd_ctx.h"
#include "drv_jpd_parser.h"
#include "drv_jpd_dev.h"
#include "drv_jpd_vdi.h"
#include "hal_jpd_def.h"
#include "hal_jpd_ops.h"
#include <cam_os_wrapper.h>

#define JPD_CMDQ_BUF_SIZE       (sizeof(u64) * 1024)
#define WRITE_RLE_TIMEOUT_COUNT 5000000UL

MS_S32 MdrvJpdCtxInitialize(JPD_Ctx_t *pCtx)
{
    //    CamOsMutexInit(&pCtx->m_Mutex);
    //    CamOsTsemInit(&pCtx->m_Sem, 1);

    return 0;
}

void MdrvJpdJCtxRelease(void *pCtxIn)
{
    JPD_Ctx_t *pCtx = (JPD_Ctx_t *)pCtxIn;

    if (pCtx)
    {
        if (pCtx->pu64CmdqBuf)
        {
            free((void *)pCtx->pu64CmdqBuf);
            pCtx->pu64CmdqBuf = NULL;
        }

        //        CamOsMutexDestroy(&pCtx->m_Mutex);
        //        CamOsTsemDeinit(&pCtx->m_Sem);
        free(pCtx);
    }
}

MS_S32 MdrvJpdCtxRegister(JPD_Dev_t *pDev, JPD_Ctx_t *pCtx)
{
    int i = 0;
    //    CamOsMutexLock(&pDev->m_Mutex);
    pCtx->s32Index = -1;
    while (i < MAX_JPD_CHN_PER_DEV)
    {
        if (pDev->user[i++].pCtx)
        {
            continue;
        }
        pDev->user[--i].pCtx = pCtx;
        pCtx->pDev           = pDev;
        JPD_LOGD("<notice> pCtx->pDev is register i=%d\n", i);
        pCtx->s32Index = i;
        break;
    }
    //    CamOsMutexUnlock(&pDev->m_Mutex);

    return pCtx->s32Index;
}

MS_S32 MdrvJpdCtxUnregister(JPD_Dev_t *pDev, JPD_Ctx_t *pCtx)
{
    if ((!pDev) || (!pCtx))
    {
        return E_MHAL_ERR_ILLEGAL_PARAM;
    }

    //    CamOsMutexLock(&pDev->m_Mutex);
    pDev->user[pCtx->s32Index].pCtx = NULL;
    pCtx->pDev                      = NULL;
    pCtx->s32Index                  = -1;
    //    CamOsMutexUnlock(&pDev->m_Mutex);

    return E_JPD_RET_CODE_SUCCESS;
}

JPD_Ctx_t *MdrvJpdCtxAlloc(JPD_Dev_t *pDev)
{
    JPD_Ctx_t *pCtx = NULL;

    if (pDev == NULL)
    {
        JPD_LOGE("pDev is null\n");
        return NULL;
    }

    do
    {
        pCtx = malloc(sizeof(JPD_Ctx_t));
        if (pCtx == NULL)
        {
            JPD_LOGE("alloc JPD_Ctx_t fail\n");
            return NULL;
        }

        memset(pCtx, 0, sizeof(JPD_Ctx_t));
        pCtx->i_state    = 0;
        pCtx->CtxRelease = MdrvJpdJCtxRelease;

#ifdef NJPD_CMDQ_SUPPORT
        pCtx->pu64CmdqBuf = malloc(1, JPD_CMDQ_BUF_SIZE);
        if (pCtx->pu64CmdqBuf)
        {
            memset(pCtx->pu64CmdqBuf, 0x0, JPD_CMDQ_BUF_SIZE);
        }
        else
        {
            JPD_LOGE("alloc cmdq buffer fail\n");
            break;
        }
#endif

        return pCtx;
    } while (0);

#ifdef NJPD_CMDQ_SUPPORT
    if (pCtx->pu64CmdqBuf)
    {
        free((void *)pCtx->pu64CmdqBuf);
        pCtx->pu64CmdqBuf = NULL;
    }
#endif

    if (pCtx)
    {
        free((void *)pCtx);
        pCtx = NULL;
    }

    return NULL;
}

void _ApiTerminate(JPD_Ctx_t *pCtx, EN_JPEG_ERR_CODE status)
{
    pCtx->Error_code = status;

    JpegFreeAllBlocks(pCtx);

    JPD_LOGD("ERROR= %d\n", pCtx->Error_code);
}
//------------------------------------------------------------------------------
// Reset thumbnail parameters
JPEG_STATIC void _ApiInitThumbnail(JPD_Ctx_t *pCtx)
{
    memset(&pCtx->stThumbnailInfo, 0, sizeof(pCtx->stThumbnailInfo));
    memset(&pCtx->stExifInfo.stEXIF_DateTime, 0, sizeof(pCtx->stExifInfo.stEXIF_DateTime));

    pCtx->stExifInfo.eEXIF_Orientation = E_JPEG_EXIF_ORIENT_NOT_FOUND;

    JPD_LOGD("init thumbnail\n");

#if SUPPORT_EXIF_EXTRA_INFO
    memset(&pCtx->stExifInfo, 0, sizeof(pCtx->stExifInfo));
#endif
}

//------------------------------------------------------------------------------
// Set scale down parameters
JPEG_STATIC void _ApiJpdSetScaleDownInfo(JPD_Ctx_t *pCtx)
{
    JPEG_Deal_Info_t *pstDealInfo = &pCtx->stDealInfo;
    switch (pCtx->eScaleDownMode)
    {
        case E_MHAL_JPD_SCALE_DOWN_EIGHTH:
        {
            JPD_LOGD("down scale 1/8!!\n");
            pstDealInfo->u8DownScaleRatio  = E_NJPD_DOWNSCALE_EIGHTH;
            pstDealInfo->u8ScaleDownFactor = 8;
            break;
        }
        case E_MHAL_JPD_SCALE_DOWN_FOURTH:
        {
            JPD_LOGD("down scale 1/4!!\n");
            pstDealInfo->u8DownScaleRatio  = E_NJPD_DOWNSCALE_FOURTH;
            pstDealInfo->u8ScaleDownFactor = 4;
            break;
        }
        case E_MHAL_JPD_SCALE_DOWN_HALF:
        {
            JPD_LOGD("down scale 1/2!!\n");
            pstDealInfo->u8DownScaleRatio  = E_NJPD_DOWNSCALE_HALF;
            pstDealInfo->u8ScaleDownFactor = 2;
            break;
        }
        case E_MHAL_JPD_SCALE_DOWN_ORG:
        {
            JPD_LOGD("down scale 1/1!!\n");
            pstDealInfo->u8DownScaleRatio  = E_NJPD_DOWNSCALE_ORG;
            pstDealInfo->u8ScaleDownFactor = 1;
            break;
        }
        default:
        {
            JPD_LOGE("Unsupported scale mode: %d!!!\n", pCtx->eScaleDownMode);
            break;
        }
    }

    if (E_NJPD_DOWNSCALE_ORG != pstDealInfo->u8DownScaleRatio)
    {
        pstDealInfo->u16AlignedImgPitch = pstDealInfo->u16AlignedImgPitch / pstDealInfo->u8ScaleDownFactor;
        pstDealInfo->u16AlignedImgPitch_H =
            ((pstDealInfo->u16AlignedImgPitch_H + ((MS_U32)pstDealInfo->u8ScaleDownFactor - 1))
             & (~((MS_U32)pstDealInfo->u8ScaleDownFactor - 1)))
            / pstDealInfo->u8ScaleDownFactor;
        pstDealInfo->u16AlignedImgWidth = pCtx->stHeadInfo.u16Image_x_size / pstDealInfo->u8ScaleDownFactor;
        pstDealInfo->u16AlignedImgHeight =
            ((pstDealInfo->u16AlignedImgHeight + ((MS_U32)pstDealInfo->u8ScaleDownFactor - 1))
             & (~((MS_U32)pstDealInfo->u8ScaleDownFactor - 1)))
            / pstDealInfo->u8ScaleDownFactor;
    }

    switch (pstDealInfo->u8DownScaleRatio)
    {
        case E_NJPD_DOWNSCALE_HALF:
            pstDealInfo->ROI_width = (pstDealInfo->u16AlignedImgPitch / 16) * 16;
            break;

        case E_NJPD_DOWNSCALE_FOURTH:
            pstDealInfo->ROI_width = (pstDealInfo->u16AlignedImgPitch / 32) * 32;
            break;

        case E_NJPD_DOWNSCALE_EIGHTH:
            pstDealInfo->ROI_width = (pstDealInfo->u16AlignedImgPitch / 64) * 64;
            break;

        default:
            pstDealInfo->ROI_width = pstDealInfo->u16AlignedImgPitch; // this
            break;
    }
    JPD_LOGD("Original ROI_width = %d \n", pstDealInfo->ROI_width);

    if (pstDealInfo->ROI_width != pstDealInfo->u16AlignedImgPitch)
    {
        pstDealInfo->u16AlignedImgPitch = pstDealInfo->u16AlignedImgWidth =
            pstDealInfo->ROI_width / pstDealInfo->u8DownScaleRatio;

        if (pCtx->stHeadInfo.bProgressive_flag)
        {
            pstDealInfo->Progressive_ROI_flag = true;
        }
    }
}

static MS_S32 _ApiJpdValueInit(JPD_Ctx_t *pCtx)
{
    // must know decode type. mi don't know this type. reservered.
    if (pCtx->u8DecodeType == E_MHAL_JPD_DECODE_MJPEG)
    {
        pCtx->bIsMjpeg = TRUE;
    }

    pCtx->eOutputFmt  = E_MHAL_JPD_OUTPUT_FMT_YUV422;
    pCtx->eOutputMode = E_MHAL_JPD_OUTPUT_FRAME;
    pCtx->u32LineNum  = 32;
    pCtx->u8FrameRdy  = FALSE;

    memset(&pCtx->stHeadInfo, 0, sizeof(pCtx->stHeadInfo));
    memset(&pCtx->stDealInfo, 0, sizeof(pCtx->stDealInfo));

    pCtx->Error_code             = E_JPEG_NO_ERROR;
    pCtx->stDealInfo.u8LumaCi    = 1;
    pCtx->stDealInfo.u8ChromaCi  = 2;
    pCtx->stDealInfo.u8Chroma2Ci = 3;
    pCtx->stDealInfo.bFirstRLE   = TRUE;

    pCtx->bIsMjpeg  = FALSE;
    pCtx->bMHEG5    = FALSE;
    pCtx->bEOF_flag = FALSE;

    pCtx->u8Tem_flag = 0;
    pCtx->Error_code = 0;

    if (pCtx->u8DecodeType == E_MHAL_JPD_DECODE_THHUMBNAIL)
    {
        _ApiInitThumbnail(pCtx);
    }

#if SUPPORT_PROGRESSIVE_MODE
    memset(&pCtx->stProgressiveInfo, 0, sizeof(pCtx->stProgressiveInfo));
#endif
    JPD_LOGD("Thumbnail AccessMode %d\n", pCtx->stThumbnailInfo.bThumbnailAccessMode);

    // Ready the input buffer.
    if (pCtx->stThumbnailInfo.bThumbnailAccessMode)
    {
        // It means that thumbnail is found and re-call JPEG_init()
        // to re-fill thumbnail data to internal buffer.
        if (!JpegInitParseAddr(pCtx))
        {
            JPD_LOGE("JPEG_init : JPEG_fill_read_buffer failed!!!\n");
            return FALSE;
        }
    }
    else
    {
        pCtx->u32Total_bytes_read = pCtx->u32In_buf_left_bytes;
    }
    return TRUE;
}

JPEG_STATIC void _ApiJpdGetAlignedResolution(JPD_Ctx_t *pCtx, MS_U16 *width, MS_U16 *height)
{
    MS_U8 mcuWidth, mcuHeight;
    MS_U8 yVsf = pCtx->stHeadInfo.u8Comp_v_samp[0];
    MS_U8 yHsf = pCtx->stHeadInfo.u8Comp_h_samp[0];

    *width  = pCtx->stHeadInfo.u16OriImg_x_size;
    *height = pCtx->stHeadInfo.u16OriImg_y_size;

    mcuWidth = pCtx->stHeadInfo.u16OriImg_x_size % (yHsf * 8);
    if (mcuWidth)
    {
        *width += (yHsf * 8 - mcuWidth);
    }

    mcuHeight = pCtx->stHeadInfo.u16OriImg_y_size % (yVsf * 8);
    if (mcuHeight)
    {
        *height += (yVsf * 8 - mcuHeight);
    }

    JPD_LOGD("samp(h v): <%d %d>\n", pCtx->stHeadInfo.u8Comp_h_samp[0], pCtx->stHeadInfo.u8Comp_v_samp[0]);
    JPD_LOGD("Ori Image size: %d*%d\n", pCtx->stHeadInfo.u16OriImg_x_size, pCtx->stHeadInfo.u16OriImg_y_size);
    JPD_LOGD("Align     size: %d*%d\n", *width, *height);
}
//------------------------------------------------------------------------------
//*************************************************
// write group information
//*************************************************
JPEG_STATIC void _ApiJpdWriteGroupTable(JPD_Ctx_t *pCtx) // type : luma=>0  chroma=>1
{
    MS_U16      ci, dcTblNumLuma = 0, dcTblNumChroma = 0;
    MS_U16      acTblNumLuma = 0, acTblNumChroma = 0;
    MS_U8       lumaCi = 0, chromaCi = 0;
    MS_U16      dcTblNumChroma2 = 0;
    MS_U16      acTblNumChroma2 = 0;
    MS_U8       chroma2Ci       = 0;
    NJPD_GRPINF structGrpinf;

    JPEG_memset((void *)&structGrpinf, 0, sizeof(structGrpinf));

    if (pCtx->stDealInfo.HeadCheck.DHT)
    {
        for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
        {
            if (pCtx->stDealInfo.u8LumaCi == pCtx->stHeadInfo.u8Comp_ident[ci])
            {
                lumaCi = ci;
                break;
            }
        }

        for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
        {
            if (pCtx->stDealInfo.u8ChromaCi == pCtx->stHeadInfo.u8Comp_ident[ci])
            {
                chromaCi = ci;
                break;
            }
        }

        if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
        {
            for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
            {
                if (pCtx->stDealInfo.u8Chroma2Ci == pCtx->stHeadInfo.u8Comp_ident[ci])
                {
                    chroma2Ci = ci;
                    break;
                }
            }
        }

        dcTblNumLuma   = pCtx->stHeadInfo.u8Comp_dc_tab[lumaCi];
        dcTblNumChroma = pCtx->stHeadInfo.u8Comp_dc_tab[chromaCi];

        acTblNumLuma   = pCtx->stHeadInfo.u8Comp_ac_tab[lumaCi];
        acTblNumChroma = pCtx->stHeadInfo.u8Comp_ac_tab[chromaCi];

        if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
        {
            dcTblNumChroma2 = pCtx->stHeadInfo.u8Comp_dc_tab[chroma2Ci];
            acTblNumChroma2 = pCtx->stHeadInfo.u8Comp_ac_tab[chroma2Ci];
        }
    }

    structGrpinf.DHT              = pCtx->stDealInfo.HeadCheck.DHT;
    structGrpinf.bUVHuffman       = pCtx->stDealInfo.bIs3HuffTbl;
    structGrpinf.u8DcLumaValid    = pCtx->stHeadInfo.stHuff_info[dcTblNumLuma].u8Valid;
    structGrpinf.u8DcLumaSymbol   = pCtx->stHeadInfo.stHuff_info[dcTblNumLuma].u8Symbol;
    structGrpinf.u16DcLumaCode    = pCtx->stHeadInfo.stHuff_info[dcTblNumLuma].u16Code;
    structGrpinf.u8DcChromaValid  = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma].u8Valid;
    structGrpinf.u8DcChromaSymbol = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma].u8Symbol;
    structGrpinf.u16DcChromaCode  = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma].u16Code;
    structGrpinf.u8AcLumaValid    = pCtx->stHeadInfo.stHuff_info[acTblNumLuma].u8Valid;
    structGrpinf.u8AcLumaSymbol   = pCtx->stHeadInfo.stHuff_info[acTblNumLuma].u8Symbol;
    structGrpinf.u16AcLumaCode    = pCtx->stHeadInfo.stHuff_info[acTblNumLuma].u16Code;
    structGrpinf.u8AcChromaValid  = pCtx->stHeadInfo.stHuff_info[acTblNumChroma].u8Valid;
    structGrpinf.u8AcChromaSymbol = pCtx->stHeadInfo.stHuff_info[acTblNumChroma].u8Symbol;
    structGrpinf.u16AcChromaCode  = pCtx->stHeadInfo.stHuff_info[acTblNumChroma].u16Code;
    structGrpinf.u8DcLumaCnt      = pCtx->stHeadInfo.stHuff_info[dcTblNumLuma].u8SymbolCnt;
    structGrpinf.u8DcChromaCnt    = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma].u8SymbolCnt;
    structGrpinf.u8AcLumaCnt      = pCtx->stHeadInfo.stHuff_info[acTblNumLuma].u8SymbolCnt;
    structGrpinf.u8AcChromaCnt    = pCtx->stHeadInfo.stHuff_info[acTblNumChroma].u8SymbolCnt;
    if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
    {
        structGrpinf.u8DcChroma2Valid  = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma2].u8Valid;
        structGrpinf.u8DcChroma2Symbol = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma2].u8Symbol;
        structGrpinf.u16DcChroma2Code  = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma2].u16Code;
        structGrpinf.u8AcChroma2Valid  = pCtx->stHeadInfo.stHuff_info[acTblNumChroma2].u8Valid;
        structGrpinf.u8AcChroma2Symbol = pCtx->stHeadInfo.stHuff_info[acTblNumChroma2].u8Symbol;
        structGrpinf.u16AcChroma2Code  = pCtx->stHeadInfo.stHuff_info[acTblNumChroma2].u16Code;
        structGrpinf.u8DcChroma2Cnt    = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma2].u8SymbolCnt;
        structGrpinf.u8AcChroma2Cnt    = pCtx->stHeadInfo.stHuff_info[acTblNumChroma2].u8SymbolCnt;
    }
    // write  ac dc haffman
    MapiJpdWriteGrpinf(pCtx->pDev->u32DevId, structGrpinf);
}

//------------------------------------------------------------------------------
//*************************************************
// write symbol table
//*************************************************
//------------------------------------------------------------------------------
JPEG_STATIC void _ApiJpdWriteSymidx(JPD_Ctx_t *pCtx)
{
    MS_U16      acTblNumLuma = 0, acTblNumChroma = 0;
    MS_U16      dcTblNumLuma = 0, dcTblNumChroma = 0;
    MS_U8       ci, lumaCi = 0, chromaCi = 0;
    MS_U16      acTblNumChroma2 = 0;
    MS_U16      dcTblNumChroma2 = 0;
    MS_U8       chroma2Ci       = 0;
    NJPD_SYMIDX structSymidx;

    JPEG_memset((void *)(&structSymidx), 0, sizeof(structSymidx));

    if (pCtx->stDealInfo.HeadCheck.DHT)
    {
        for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
        {
            if (pCtx->stDealInfo.u8LumaCi == pCtx->stHeadInfo.u8Comp_ident[ci])
            {
                lumaCi = ci;
                break;
            }
        }

        for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
        {
            if (pCtx->stDealInfo.u8ChromaCi == pCtx->stHeadInfo.u8Comp_ident[ci])
            {
                chromaCi = ci;
                break;
            }
        }

        if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
        {
            for (ci = 0; ci < pCtx->stHeadInfo.u8Comps_in_frame; ci++)
            {
                if (pCtx->stDealInfo.u8Chroma2Ci == pCtx->stHeadInfo.u8Comp_ident[ci])
                {
                    chroma2Ci = ci;
                    break;
                }
            }
        }

        acTblNumLuma   = pCtx->stHeadInfo.u8Comp_ac_tab[lumaCi];
        acTblNumChroma = pCtx->stHeadInfo.u8Comp_ac_tab[chromaCi];

        dcTblNumLuma   = pCtx->stHeadInfo.u8Comp_dc_tab[lumaCi];
        dcTblNumChroma = pCtx->stHeadInfo.u8Comp_dc_tab[chromaCi];
        if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
        {
            acTblNumChroma2 = pCtx->stHeadInfo.u8Comp_ac_tab[chroma2Ci];
            dcTblNumChroma2 = pCtx->stHeadInfo.u8Comp_dc_tab[chroma2Ci];
        }
    }

    structSymidx.DHT               = pCtx->stDealInfo.HeadCheck.DHT;
    structSymidx.bUVHuffman        = pCtx->stDealInfo.bIs3HuffTbl;
    structSymidx.u8DcLumaHuffVal   = pCtx->stHeadInfo.stHuff_info[dcTblNumLuma].u8HuffVal;
    structSymidx.u8DcChromaHuffVal = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma].u8HuffVal;
    structSymidx.u8AcLumaHuffVal   = pCtx->stHeadInfo.stHuff_info[acTblNumLuma].u8HuffVal;
    structSymidx.u8AcChromaHuffVal = pCtx->stHeadInfo.stHuff_info[acTblNumChroma].u8HuffVal;
    if (TRUE == pCtx->stDealInfo.bIs3HuffTbl)
    {
        structSymidx.u8DcChroma2HuffVal = pCtx->stHeadInfo.stHuff_info[dcTblNumChroma2].u8HuffVal;
        structSymidx.u8AcChroma2HuffVal = pCtx->stHeadInfo.stHuff_info[acTblNumChroma2].u8HuffVal;
    }
    ApiJpdWriteSymidx(pCtx->pDev->u32DevId, structSymidx);
}
//------------------------------------------------------------------------------
//***************************************************
// write quantization table
//***************************************************
JPEG_STATIC void _ApiJpdWriteIQuantizationTable(JPD_Ctx_t *pCtx)
{
    // MS_U8 i, j;
    MS_U8       i;
    MS_U8       comNum = 0;
    MS_U8       comp[JPEG_MAXCOMPONENTS];
    NJPD_IQ_TBL structIqtbl;

    JPEG_memset((void *)&structIqtbl, 0, sizeof(structIqtbl));

    if (pCtx->stDealInfo.HeadCheck.DQT)
    {
        // Calculate how many valid quantization tables
        JPEG_memset((void *)comp, 0, JPEG_MAXCOMPONENTS);
        for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_frame; i++)
        {
            comp[pCtx->stHeadInfo.u8Comp_quant[i]] = 1;
        }

        for (i = 0; i < JPEG_MAXCOMPONENTS; i++)
        {
            if (comp[i] == 1)
            {
                comNum++;
            }
        }
    }

    structIqtbl.DQT         = pCtx->stDealInfo.HeadCheck.DQT;
    structIqtbl.u8CompNum   = comNum;
    structIqtbl.u8CompQuant = pCtx->stHeadInfo.u8Comp_quant;
    structIqtbl.QuantTables = (NJPD_QUAN_TBL *)pCtx->stHeadInfo.stQuantTables;

    MapiJpdWriteIQTbl(pCtx->pDev->u32DevId, structIqtbl);
}

MS_S32 _ApiJpdWriteRegTrig(JPD_Ctx_t *pCtx)
{
    MS_U16       picWidth, picHeight;
    MS_U8        yVsf = pCtx->stHeadInfo.u8Comp_v_samp[0];
    MS_U8        yHsf = pCtx->stHeadInfo.u8Comp_h_samp[0];
    MS_BOOL      bUvEn;
    MS_U16       regValue;
    MS_U8        i;
    MS_U8        comNum = 0;
    MS_U8        comp[JPEG_MAXCOMPONENTS];
    NJPD_BUF_CFG structBuf;

    JPD_LOGD("\n");
    ApiJPDResetISRStatus(pCtx->pDev->u32DevId);

    MapiJpdReset(pCtx->pDev->u32DevId);

    // Calculate how many valid quantization tables for components
    JPEG_memset((void *)comp, 0, JPEG_MAXCOMPONENTS);
    for (i = 0; i < pCtx->stHeadInfo.u8Comps_in_frame; i++)
    {
        comp[pCtx->stHeadInfo.u8Comp_quant[i]] = 1;
    }

    for (i = 0; i < JPEG_MAXCOMPONENTS; i++)
    {
        if (comp[i] == 1)
        {
            comNum++;
        }
    }

    if (pCtx->stHeadInfo.u8Comps_in_frame > 1)
    {
        bUvEn = TRUE;
    }
    else
    {
        bUvEn = FALSE;
    }

    _ApiJpdGetAlignedResolution(pCtx, &picWidth, &picHeight);

    structBuf.bProgressive          = pCtx->stHeadInfo.bProgressive_flag;
    structBuf.bThumbnailAccessMode  = pCtx->stThumbnailInfo.bThumbnailAccessMode;
    structBuf.u32ThumbnailBufAddr   = pCtx->stInternalBuf.phyInternalBufAddr;
    structBuf.u32ThumbnailBufSize   = JPEG_DEFAULT_EXIF_SIZE;
    structBuf.u32ThumbnailBufOffset = pCtx->stThumbnailInfo.u32ThumbnailBufferOffset + JpegGetECS(pCtx);

    structBuf.u32MRCBufAddr    = (pCtx->stInBufInfo.phyBufAddr1);
    structBuf.u32MRCBufSize    = pCtx->stInBufInfo.u32InputBufSize1;
    structBuf.u32MRCBufOffset  = JpegGetECS(pCtx);
    structBuf.u32MWCBufAddr    = (pCtx->stOutputBuf.phyOutputBufAddr[0]);
    structBuf.u32VirMRCBufAddr = (((unsigned long)pCtx->stInBufInfo.pInputBufVA1));

    structBuf.u32TableAddr    = (pCtx->stInternalBuf.phyInternalBufAddr);
    structBuf.u32VirTableAddr = (((unsigned long)pCtx->stInternalBuf.pInternalBufVA));

    if (pCtx->eOutputMode == E_MHAL_JPD_OUTPUT_FRAME)
    {
        MapiJpdWriteIMIEnable(pCtx->pDev->u32DevId, FALSE);
        MapiJpdSetWRingBufEnable(pCtx->pDev->u32DevId, FALSE);
        MapiJpdWbufPackEnable(pCtx->pDev->u32DevId, TRUE); // m6p haps is ok, need check again on ASIC

        JPD_LOGD("MRC BufSize = %x\n", structBuf.u32MRCBufSize);

        // HW limitation:if we don't enable write-protect mode, set this value to zero.
#if (ENABLE_TEST_09_NJPEGWriteProtectTest == FALSE)
        structBuf.u16MWCBufLineNum = 0;
#else
        structBuf.u16MWCBufLineNum = 32;
#endif
    }
    else if (pCtx->eOutputMode == E_MHAL_JPD_OUTPUT_IMI)
    {
        structBuf.u16MWCBufLineNum = pCtx->u32LineNum;
#if !defined(JPD_UT_TEST)
        structBuf.u32MWCBufAddr = pCtx->stOutputBuf.phyOutputBufAddr[0];
#else
        structBuf.u32MWCBufAddr    = 0;
#endif
        structBuf.bIMIMode = TRUE;

        MapiJpdWbufPackEnable(pCtx->pDev->u32DevId, FALSE);
        MapiJpdWriteIMIEnable(pCtx->pDev->u32DevId, TRUE);    // reg_jpd_w2imi_enable   = 1
        MapiJpdSetWRingBufEnable(pCtx->pDev->u32DevId, TRUE); // reg_jpd_wbuf_ring_en   = 1
        JPD_LOGD("[notice]IMI RING. line %d\n", pCtx->u32LineNum);
    }
    else if (pCtx->eOutputMode == E_MHAL_JPD_OUTPUT_EMI)
    {
        structBuf.u16MWCBufLineNum = pCtx->u32LineNum;
        MapiJpdWriteIMIEnable(pCtx->pDev->u32DevId, FALSE);   // reg_jpd_w2imi_enable   = 0
        MapiJpdSetWRingBufEnable(pCtx->pDev->u32DevId, TRUE); // reg_jpd_wbuf_ring_en   = 1
        MapiJpdWbufPackEnable(pCtx->pDev->u32DevId, TRUE);    // m6p haps is ok, need check again on ASIC

        JPD_LOGD("[notice]EMI RING. line %d\n", pCtx->u32LineNum);
    }
    else
    {
        _ApiTerminate(pCtx, E_JPEG_STOP_DECODE);

        JPD_LOGE("not support eOutputMode <%d>\n", pCtx->eOutputMode);
        return FALSE;
    }

    JPD_LOGD("ThumbnailBufAddr = 0x%llx, ThumbnailBufSize = 0x%x, ThumbnailBufOffset = 0x%x\n",
             structBuf.u32ThumbnailBufAddr, structBuf.u32ThumbnailBufSize, structBuf.u32ThumbnailBufOffset);
    JPD_LOGD("MRCBufAddr = 0x%llx, MRCBufSize = 0x%x, MRCBufOffset = 0x%x\n", structBuf.u32MRCBufAddr,
             structBuf.u32MRCBufSize, structBuf.u32MRCBufOffset);
    JPD_LOGD("MWCBufAddr = 0x%llx, MWCBufLineNum = %d\n", structBuf.u32MWCBufAddr, structBuf.u16MWCBufLineNum);

    pCtx->stDealInfo.u32DataOffset += JpegGetECS(pCtx);

    if (structBuf.u32MRCBufOffset < structBuf.u32MRCBufSize)
    {
        JPD_LOGD("enough MRC size(0x%x, 0x%x)\n", pCtx->stDealInfo.u32DataOffset, structBuf.u32MRCBufSize);
    }
    else
    {
        _ApiTerminate(pCtx, E_JPEG_NOTENOUGHMEM);

        JPD_LOGE("not enough MRC size(0x%x, 0x%x)\n", pCtx->stDealInfo.u32DataOffset, structBuf.u32MRCBufSize);
        return FALSE;
    }

    pCtx->stDealInfo.bDifferentHL = MapiJpdInitBuffer(pCtx->pDev->u32DevId, structBuf, pCtx->bIsMjpeg); // mjpeg change

    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        // This setting is only needed in NPJD
        // ROI is implemented by HW in NJPD
        MapiJpdSetPicDimension(pCtx->pDev->u32DevId, pCtx->stDealInfo.ROI_width, picHeight);
    }
    else
    {
        MapiJpdSetPicDimension(pCtx->pDev->u32DevId, picWidth, picHeight);
    }

    // Check if it needs to do ROI
    if (E_NJPD_DOWNSCALE_ORG != pCtx->stDealInfo.u8DownScaleRatio)
    {
        if (pCtx->stDealInfo.ROI_width != picWidth && !pCtx->stHeadInfo.bProgressive_flag)
        {
            JPD_LOGD("ROI!! ROI Size: %d*%d\n", pCtx->stDealInfo.ROI_width, picHeight);
            MapiJpdSetROI(pCtx->pDev->u32DevId, 0, 0, ((pCtx->stDealInfo.ROI_width) >> 3), ((picHeight) >> 3));
            // regValue = regValue | NJPD_ROI_EN;
        }
    }

    // In NJPD, software VLD mode, we don't need to write huff & symbol tables
    if (pCtx->stHeadInfo.bProgressive_flag == FALSE)
    {
        MapiJpdSetDifferentHTable(
            pCtx->pDev->u32DevId,
            pCtx->stDealInfo.bIs3HuffTbl);              // This function must be set before _ApiJpdWriteGroupTable()
        MapiJpdGTableRst(pCtx->pDev->u32DevId, ENABLE); // write reg to group table rest
        _ApiJpdWriteGroupTable(pCtx);
        _ApiJpdWriteSymidx(pCtx);
    }

    _ApiJpdWriteIQuantizationTable(pCtx);

    flush_dcache_range((unsigned long)(pCtx->stInternalBuf.pInternalBufVA),
                       (unsigned long)(pCtx->stInternalBuf.pInternalBufVA + pCtx->stInternalBuf.u32InternalBufSize));

    MapiJpdTableLoadingStart(pCtx->pDev->u32DevId); // set reg to start
    yVsf -= 1;
    yHsf -= 1;
    regValue = MapiJpdGetGlobalSetting00(pCtx->pDev->u32DevId);

    if (pCtx->stHeadInfo.u16Restart_interval)
    {
        JPD_LOGD("RST found! Enable NJPD_RST_EN!");
        MapiJpdSetRstIntv(pCtx->pDev->u32DevId, pCtx->stHeadInfo.u16Restart_interval);
        MapiJpdSetScalingDownFactor(pCtx->pDev->u32DevId,
                                    (EN_NJPD_SCALING_DOWN_FACTOR)pCtx->stDealInfo.u8DownScaleRatio);
        regValue |= (NJPD_RST_EN | ((MS_U32)(bUvEn)) << 3 | ((yVsf) << 2) | yHsf);
    }
    else
    {
        MapiJpdSetScalingDownFactor(pCtx->pDev->u32DevId,
                                    (EN_NJPD_SCALING_DOWN_FACTOR)pCtx->stDealInfo.u8DownScaleRatio);
        regValue |= (((MS_U32)bUvEn) << 3 | ((yVsf) << 2) | yHsf);
    }

    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        MapiJpdSetSoftwareVLD(pCtx->pDev->u32DevId, ENABLE);
    }

    MapiJpdEnablePowerSaving(pCtx->pDev->u32DevId);

    if (pCtx->bWriteProtectEnable == 1)
    {
        MapiJpdSetAutoProtect(pCtx->pDev->u32DevId, TRUE);
        JPD_LOGD("[NOTICE] write protect\n");
        MapiJpdSetWPENStartAddr(pCtx->pDev->u32DevId, structBuf.u32MWCBufAddr);
        MapiJpdSetWPENEndAddr(pCtx->pDev->u32DevId, structBuf.u32MWCBufAddr + pCtx->stOutputBuf.OutputBufSize);
    }
    else
    {
        MapiJpdSetAutoProtect(pCtx->pDev->u32DevId, FALSE);
    }

    MapiJpdSetGlobalSetting00(pCtx->pDev->u32DevId, regValue | NJPD_SWRST);

    if (!pCtx->stHeadInfo.bProgressive_flag)
    {
        JPD_LOGD("MapiJpdSetMRCValid\n");
        MapiJpdSetMRCValid(pCtx->pDev->u32DevId, NJPD_MRC0_VALID);
        MsOS_DelayTaskUs(100);
        MapiJpdSetMRCValid(pCtx->pDev->u32DevId, NJPD_MRC1_VALID);
    }

    //    CamOsGetMonotonicTime(&pCtx->pDev->isr_set_time);
    if (pCtx->JPD_Debug == 2)
    {
        MapiJpdPZ1SetFlag(0x3333);
    }

    if (pCtx->JPD_Debug == 1)
    {
        while (1)
        {
            ;
        }
    }
    JPD_LOGD("After Setting SCONFIG, NJPD START!!\n");
    invalidate_dcache_range((unsigned long)(pCtx->stOutputBuf.pOutputBufVA[0]),
                            (unsigned long)(pCtx->stOutputBuf.pOutputBufVA[0] + pCtx->stOutputBuf.OutputBufSize));

    MapiJpdDecodeEnable(pCtx->pDev->u32DevId);

    return TRUE;
}

#if SUPPORT_PROGRESSIVE_MODE
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Loads and dequantizes the next row of (already decoded) coefficients.
// Progressive images only.
JPEG_STATIC JPEG_BLOCK_TYPE *_ApiCoeffBufferGetp(JPD_Ctx_t *pCtx, PJPEG_COEFF_BUF cb, MS_U16 block_x, MS_U16 block_y)
{
    if (block_x >= cb->u16Block_num_x)
    {
        _ApiTerminate(pCtx, E_JPEG_ASSERTION_ERROR);
        return NULL;
    }

    if (block_y >= cb->u16Block_num_y)
    {
        _ApiTerminate(pCtx, E_JPEG_ASSERTION_ERROR);
        return NULL;
    }

    return (JPEG_BLOCK_TYPE *)((unsigned long)(cb->pu8Data + block_x * cb->u16Block_size
                                               + block_y * (cb->u16Block_size * cb->u16Block_num_x)));
}

//------------------------------------------------------------------------------
// Write RLE result
static MS_BOOL _ApiJpegWriteRLE(JPD_Ctx_t *pCtx, JPEG_SVLD *pVld, MS_BOOL bDecodeNow)
{
    MS_U8 *mrcBuffer = (MS_U8 *)((unsigned long)(pCtx->stInBufInfo.pInputBufVA1));
    MS_U16 status;
    MS_U32 startTime;

    JPEG_memcpy((void *)(mrcBuffer + pCtx->stProgressiveInfo.u32RLEOffset), (void *)pVld, 4);
    pCtx->stProgressiveInfo.u32RLEOffset += 4;
    ApiJPDResetISRStatus(pCtx->pDev->u32DevId);

    // Check if buffer full
    if ((pCtx->stInBufInfo.u32InputBufSize1 == pCtx->stProgressiveInfo.u32RLEOffset) || (TRUE == bDecodeNow))
    {
        JPD_LOGD("Do RLE, LENG 0x%x, bDecodeNow = %d\n", pCtx->stProgressiveInfo.u32RLEOffset, bDecodeNow);
        JPD_LOGD("CPU Sync and Flush Memory~~~~\n");
        ApiJpdAsmCpuSync();
        flush_dcache_range((unsigned long)pCtx->stInBufInfo.pInputBufVA1,
                           (unsigned long)(pCtx->stInBufInfo.pInputBufVA1 + pCtx->stInBufInfo.u32InputBufSize1));

        if (pCtx->stDealInfo.bFirstRLE == TRUE)
        {
            if (!_ApiJpdWriteRegTrig(pCtx))
            {
                return FALSE;
            }
            pCtx->stDealInfo.bFirstRLE = FALSE;
        }

        // clear MRC low/high portion read complete event. if suppouse isr,don't need to clear
        MapiJpdSetEventFlag(pCtx->pDev->u32DevId, E_NJPD_EVENT_MRC0_EMPTY | E_NJPD_EVENT_MRC1_EMPTY);

        if (pCtx->stInBufInfo.u32InputBufSize1 / 2 >= pCtx->stProgressiveInfo.u32RLEOffset)
        {
            if (TRUE == bDecodeNow)
            {
                JPD_LOGD("last one bit enable~~ 1\n");
                MapiJpdReadLastBuffer(pCtx->pDev->u32DevId);
            }
            else
            {
                JPD_LOGW("something strange~~~\n");
            }
            MapiJpdSetMRCValid(pCtx->pDev->u32DevId, NJPD_MRC0_VALID);
            MsOS_DelayTaskUs(100);
            JPD_LOGD("MRC0 Delay~~~~ 1\n");
        }
        else
        {
            MapiJpdSetMRCValid(pCtx->pDev->u32DevId, NJPD_MRC0_VALID);
            MsOS_DelayTaskUs(100);
            JPD_LOGD("MRC0 Delay~~~~ 2\n");

            if (TRUE == bDecodeNow)
            {
                JPD_LOGD("last one bit enable~~ 2\n");
                MapiJpdReadLastBuffer(pCtx->pDev->u32DevId);
            }
            MapiJpdSetMRCValid(pCtx->pDev->u32DevId, NJPD_MRC1_VALID);
        }

        startTime = 0;

        if (bDecodeNow)
        {
            return TRUE; // wait done in main loop
        }

        while (1)
        {
            // status = MDrv_NJPD_ReadNJPDStatus();
            status = ApiJPDGetISRStatus(pCtx->pDev->u32DevId);
            if (status & E_NJPD_EVENT_DEC_DONE)
            {
                JPD_LOGD("P deocde done\n");
                break;
            }

            if ((status & E_NJPD_EVENT_MINICODE_ERR) || (status & E_NJPD_EVENT_INV_SCAN_ERR)
                || (status & E_NJPD_EVENT_RES_MARKER_ERR) || (status & E_NJPD_EVENT_RMID_ERR)
                //                || (status & E_NJPD_EVENT_WRITE_PROTECT)
            )
            {
                // temp patch for protect NJPD from writing to illegal memory
                JPD_LOGW("CurRow = %d, CurCol = %d ", MapiJpdGetCurRow(pCtx->pDev->u32DevId),
                         MapiJpdGetCurCol(pCtx->pDev->u32DevId));
                JPD_LOGW("CurMRCAddr = 0x%x\n", MapiJpdGetCurMRCAddr(pCtx->pDev->u32DevId));
                MapiJpdReset(pCtx->pDev->u32DevId);
                // while(1);
                _ApiTerminate(pCtx, E_JPEG_JPD_DECODE_ERROR);
                return FALSE;
            }

            if ((status & E_NJPD_EVENT_MRC0_EMPTY) && (status & E_NJPD_EVENT_MRC1_EMPTY))
            {
                JPD_LOGD("Partial SVLD decode done\n");
                break;
            }

            if (startTime++ > WRITE_RLE_TIMEOUT_COUNT)
            {
                JPD_LOGW("Partial SVLD decode time out\n");
                // temp patch for protect NJPD from writing to illegal memory
                JPD_LOGW("CurRow = %d, CurCol = %d ", MapiJpdGetCurRow(pCtx->pDev->u32DevId),
                         MapiJpdGetCurCol(pCtx->pDev->u32DevId));
                JPD_LOGW("CurMRCAddr = 0x%x\n", MapiJpdGetCurMRCAddr(pCtx->pDev->u32DevId));
                MapiJpdReset(pCtx->pDev->u32DevId);

                _ApiTerminate(pCtx, E_JPEG_JPD_DECODE_ERROR);
                return FALSE;
            }
        }
        ApiJPDResetISRStatus(pCtx->pDev->u32DevId);

        pCtx->stProgressiveInfo.u32RLEOffset = 0;
    }
    return TRUE;
}

JPEG_STATIC MS_BOOL _ApiJpdDoRle(JPD_Ctx_t *pCtx, JPEG_BLOCK_TYPE *p, MS_BOOL eop, MS_U8 comp_id, MS_BOOL BlockInRange)
{
    JPEG_SVLD       myvld;
    MS_U8           counter;
    MS_S16          value;
    MS_U16          run;
    MS_U8           curBlk;
    JPEG_BLOCK_TYPE predictor;

    if (comp_id == 0)
    {
        curBlk = 1; // Y
    }
    else if (comp_id == 1)
    {
        curBlk = 3; // U
    }
    else
    {
        curBlk = 2; // V
    }

    predictor = pCtx->stDealInfo.s16dc_predicotr[curBlk - 1];

    run         = 0;
    myvld.byte0 = myvld.byte1 = myvld.byte2 = myvld.byte3 = 0;

    // sent DC info
    if (BlockInRange) // Current block is within display range.
    {
        myvld.run = 8;
    }
    else
    {
        myvld.run = 0;
    }

    value      = (p[0] - predictor);
    myvld.data = value;

    if (!_ApiJpegWriteRLE(pCtx, &myvld, FALSE))
    {
        return FALSE;
    }

    if (BlockInRange == FALSE) // Current block is not within display range.
    {
        return TRUE;
    }

    myvld.byte0 = myvld.byte1 = myvld.byte2 = myvld.byte3 = 0;

    for (counter = 1; counter < 64; counter++)
    {
        if (p[counter] == 0)
        {
            run++;
        }
        else
        {
            while (run > 15)
            {
                myvld.data = 0;
                myvld.run  = 15;

                if (!_ApiJpegWriteRLE(pCtx, &myvld, FALSE))
                {
                    return FALSE;
                }
                myvld.byte0 = myvld.byte1 = myvld.byte2 = myvld.byte3 = 0;
                run -= 16;
            }

            myvld.data = p[counter];
            myvld.run  = run;

            // Check if the last byte is non-zero. If it's non-zero & EOP, add the EOP flag
            if (counter == 63 && eop && p[63] != 0)
            {
                if (!_ApiJpegWriteRLE(pCtx, &myvld, TRUE))
                {
                    return FALSE;
                }
                pCtx->stDealInfo.s16dc_predicotr[curBlk - 1] = p[0]; // update predictor
                return TRUE;
            }

            if (!_ApiJpegWriteRLE(pCtx, &myvld, FALSE))
            {
                return FALSE;
            }

            myvld.byte0 = myvld.byte1 = myvld.byte2 = myvld.byte3 = 0;
            run                                                   = 0;
        }
    }

    counter = 63;

    if (p[counter] == 0)
    {
        myvld.data = p[counter];
        myvld.EOB  = 1;
        myvld.run  = 0;
        if (eop)
        {
            if (!_ApiJpegWriteRLE(pCtx, &myvld, TRUE))
            {
                return FALSE;
            }
        }
        else
        {
            if (!_ApiJpegWriteRLE(pCtx, &myvld, FALSE))
            {
                return FALSE;
            }
        }
    }

    pCtx->stDealInfo.s16dc_predicotr[curBlk - 1] = p[0]; // update predictor
    return TRUE;
}

static MS_BOOL _ApiJpegLoadNextRow(JPD_Ctx_t *pCtx)
{
    JPEG_BLOCK_TYPE p[64];
    MS_BOOL         eofFlag = FALSE; // CL82399

    MS_U16 mcuRow, mcuBlock;
    MS_U8  componentNum, componentId;
    MS_U16 blockXMcu[JPEG_MAXCOMPONENTS];

    JPEG_memset((void *)blockXMcu, 0, JPEG_MAXCOMPONENTS * sizeof(MS_U16));

    for (mcuRow = 0; mcuRow < pCtx->stDealInfo.u16Mcus_per_row; mcuRow++)
    {
        MS_U16 blockXMcuOfs = 0, blockYMcuOfs = 0;

        for (mcuBlock = 0; mcuBlock < pCtx->stDealInfo.u8Blocks_per_mcu; mcuBlock++)
        {
            JPEG_BLOCK_TYPE *pAC;
            JPEG_BLOCK_TYPE *pDC;
            MS_U8            u8i;

            componentId = pCtx->stDealInfo.u8Mcu_org[mcuBlock];

            pAC = _ApiCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.AC_Coeffs[componentId],
                                      blockXMcu[componentId] + blockXMcuOfs,
                                      pCtx->stProgressiveInfo.u32Block_y_mcu[componentId] + blockYMcuOfs);
            if (pAC == NULL)
            {
                JPD_LOGE("\n");
                return FALSE;
            }

            pDC = _ApiCoeffBufferGetp(pCtx, pCtx->stProgressiveInfo.DC_Coeffs[componentId],
                                      blockXMcu[componentId] + blockXMcuOfs,
                                      pCtx->stProgressiveInfo.u32Block_y_mcu[componentId] + blockYMcuOfs);
            if (pDC == NULL)
            {
                JPD_LOGE("\n");
                return FALSE;
            }
            p[0] = pDC[0];
#if 0
            JPEG_memcpy((void *)&p[1], (void *)&pAC[1], 63 * sizeof( JPEG_BLOCK_TYPE ) );
#else
            for (u8i = 1; u8i < 64; u8i++)
            {
                p[u8i] = pAC[u8i];
            }
#endif

            if (pCtx->stDealInfo.Progressive_ROI_flag == FALSE)
            {
                if ((mcuBlock == (pCtx->stDealInfo.u8Blocks_per_mcu - 1))
                    && (mcuRow == (pCtx->stDealInfo.u16Mcus_per_row - 1))
                    && (pCtx->u16Total_lines_left == pCtx->stDealInfo.gu8Max_mcu_y_size))
                {
                    JPD_LOGD("EOF!!!!!ROI enable!!!\n");
                    if (!_ApiJpdDoRle(pCtx, p, TRUE, componentId, TRUE)) // means it is end of picture
                    {
                        return FALSE;
                    }
                }
                else
                {
                    if (!_ApiJpdDoRle(pCtx, p, FALSE, componentId, TRUE))
                    {
                        return FALSE;
                    }
                }
            }
            else
            {
                if (pCtx->u16Total_lines_left == pCtx->stDealInfo.gu8Max_mcu_y_size) // Last Line
                {
                    if ((mcuBlock == (pCtx->stDealInfo.u8Blocks_per_mcu - 1))
                        && ((mcuRow + 2) * pCtx->stDealInfo.gu8Max_mcu_x_size
                            > pCtx->stDealInfo.ROI_width)) // Last line last block within rang
                    {
                        if (eofFlag == FALSE)
                        {
                            eofFlag = TRUE;
                            // JPD_LOGD("EOF!!!!!No ROI!!!\n");
                            if (!_ApiJpdDoRle(pCtx, p, TRUE, componentId, TRUE)) // means it is end of picture
                            {
                                return FALSE;
                            }
                        }
                    }
                    else
                    {
                        if (!_ApiJpdDoRle(pCtx, p, FALSE, componentId, TRUE))
                        {
                            return FALSE;
                        }
                    }
                }
                else
                {
                    if ((mcuRow + 1) * pCtx->stDealInfo.gu8Max_mcu_x_size
                        > pCtx->stDealInfo.ROI_width) // ever line out rang block
                    {
                        // _ApiJpdDoRle(p, FALSE, componentId, FALSE);
                    }
                    else
                    {
                        if (!_ApiJpdDoRle(pCtx, p, FALSE, componentId, TRUE))
                        {
                            return FALSE;
                        }
                    }
                }
            }

            if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
            {
                blockXMcu[componentId]++;
            }
            else
            {
                if (++blockXMcuOfs == pCtx->stHeadInfo.u8Comp_h_samp[componentId])
                {
                    blockXMcuOfs = 0;

                    if (++blockYMcuOfs == pCtx->stHeadInfo.u8Comp_v_samp[componentId])
                    {
                        blockYMcuOfs = 0;

                        blockXMcu[componentId] += pCtx->stHeadInfo.u8Comp_h_samp[componentId];
                    }
                }
            }
        }
    }

    if (pCtx->stHeadInfo.u8Comps_in_scan == 1)
    {
        pCtx->stProgressiveInfo.u32Block_y_mcu[pCtx->stHeadInfo.u8Comp_list[0]]++;
    }
    else
    {
        for (componentNum = 0; componentNum < pCtx->stHeadInfo.u8Comps_in_scan; componentNum++)
        {
            componentId = pCtx->stHeadInfo.u8Comp_list[componentNum];

            pCtx->stProgressiveInfo.u32Block_y_mcu[componentId] += pCtx->stHeadInfo.u8Comp_v_samp[componentId];
        }
    }
    return TRUE;
}

//----------------------------------------------------------------------------
/******************************************************************************/
/// Start Progressive JPEG decode for NJPD
/******************************************************************************/
EN_JPEG_RESULT _ApiProgressiveDecode(JPD_Ctx_t *pCtx)
{
    if (pCtx->stHeadInfo.bProgressive_flag == FALSE)
    {
        return (E_JPEG_FAILED);
    }

    if (pCtx->u16Total_lines_left == 0)
    {
        return (E_JPEG_OKAY);
    }

    if (pCtx->u16Total_lines_left > 0)
    {
        JPD_LOGD("pCtx->u16Total_lines_left = %d\n", pCtx->u16Total_lines_left);
        if (!_ApiJpegLoadNextRow(pCtx))
        {
            return E_JPEG_FAILED;
        }

        pCtx->u16Total_lines_left -= pCtx->stDealInfo.gu8Max_mcu_y_size;
    }

    return (E_JPEG_DONE);
}
#endif

// must fist call the function
MS_S32 ApiJpdDefaultConfig(JPD_Ctx_t *pCtx)
{
    //    CamOsMutexLock(&pCtx->m_Mutex);

    _ApiJpdValueInit(pCtx);

    JpegInitParseAddr(pCtx);

    //    CamOsMutexUnlock(&pCtx->m_Mutex);

    return 0;
}

MS_S32 ApiJpdExit(JPD_Ctx_t *pCtx)
{
    //    CamOsMutexLock(&pCtx->m_Mutex);
    pCtx->u8FrameRdy = FALSE;
    // MapiJpdPowerOff(); // power off only call in MHAL_JPD_DestroyDevice
    JpegFreeAllBlocks(pCtx);
    //    CamOsMutexUnlock(&pCtx->m_Mutex);
    return TRUE;
}

MS_S32 ApiDecodeHeaderFirstStage(JPD_Ctx_t *pCtx)
{
    //    CamOsMutexLock(&pCtx->m_Mutex);

    if (FALSE == JpegLocateSOFMarker(pCtx))
    {
        JPD_LOGE("Fail !!!\n");
        //        CamOsMutexUnlock(&pCtx->m_Mutex);
        return FALSE;
    }

    JPD_LOGD("bProgressive_flag =%d\n", pCtx->stHeadInfo.bProgressive_flag); // 0

    if (pCtx->bMHEG5)
    {
        if (pCtx->Error_code == E_JPEG_UNSUPPORTED_MARKER)
        {
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return E_JPEG_FAILED;
        }
    }

    // init mempool for progressive decoding.
    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        if (!DrvJpdVdiMemInitPool((pCtx->stInternalBuf.pInternalBufVA + JPEG_DEFAULT_EXIF_SIZE),
                                  (pCtx->stInternalBuf.u32InternalBufSize - JPEG_DEFAULT_EXIF_SIZE)))
        {
            JPD_LOGE("DrvJpdVdiMemInitPool FAIL!!\n");
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return E_JPEG_FAILED;
        }
        JPD_LOGD("DrvJpdVdiMemInitPool OK!!\n");
    }

    if (pCtx->u8DecodeType == E_MHAL_JPD_DECODE_THHUMBNAIL)
    {
        if (pCtx->stThumbnailInfo.bThumbnailFound)
        {
            pCtx->stThumbnailInfo.bThumbnailAccessMode = TRUE;
            JpegFreeAllBlocks(pCtx);

            if (!_ApiJpdValueInit(pCtx))
            {
                //                CamOsMutexUnlock(&pCtx->m_Mutex);
                return FALSE;
            }
            // save the original image size, because msAPI_JPEG_init will reset all variables to 0
            if (!JpegLocateSOFMarker(pCtx))
            {
                //                CamOsMutexUnlock(&pCtx->m_Mutex);
                return FALSE;
            }
            if (pCtx->stHeadInfo.bProgressive_flag)
            {
                JPD_LOGE("Progressive image in thumbnail... do not handle it\n");
                //                CamOsMutexUnlock(&pCtx->m_Mutex);
                return FALSE;
            }
        }
        else
        {
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return FALSE;
        }
    }

    //    CamOsMutexUnlock(&pCtx->m_Mutex);
    return TRUE;
}

MS_S32 ApiDecodeHdrLastStage(JPD_Ctx_t *pCtx)
{
    //    CamOsMutexLock(&pCtx->m_Mutex);

    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        JpegGetAlignedImageInfo(pCtx);
        if (JpegCalcBlock(pCtx) == FALSE)
        {
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return FALSE;
        }

        JPD_LOGD("JpegInitProgressive.\n");
        if (!JpegInitProgressive(pCtx))
        {
            JPD_LOGE("Fail\n");
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return E_JPEG_FAILED;
        }
    }
    else
    {
        if (FALSE == JpegLocateSOSMarker(pCtx))
        {
            JPD_LOGE("Fail !!!\n");
            //            CamOsMutexUnlock(&pCtx->m_Mutex);
            return FALSE;
        }
        ApiJpdDealHeaderInfo(pCtx);
    }
    //    CamOsMutexUnlock(&pCtx->m_Mutex);
    return TRUE;
}

EN_JPEG_RESULT ApiJpdStartDecode(JPD_Ctx_t *pCtx)
{
    EN_JPEG_RESULT retVal = E_JPEG_FAILED;

    //    CamOsMutexLock(&pCtx->m_Mutex);

    _ApiJpdSetScaleDownInfo(pCtx);

    if (pCtx->stHeadInfo.bProgressive_flag)
    {
        retVal = _ApiProgressiveDecode(pCtx);
    }
    else
    {
        JPD_LOGD("In_buf_offset1 = %p\n", pCtx->pu8In_buf_offset1);
        JPD_LOGD("In_buf1_bytes  = %x\n", pCtx->u32In_buf1_bytes);
        JPD_LOGD("In_buf_left_bytes = %x\n", pCtx->u32In_buf_left_bytes);
        JPD_LOGD("Total_bytes_read  = %x\n", pCtx->u32Total_bytes_read);
        JPD_LOGD("Total_lines_left  = %x\n", pCtx->u16Total_lines_left);
        _ApiJpdWriteRegTrig(pCtx);

        if (pCtx->Error_code == E_JPEG_NO_ERROR)
        {
            retVal           = E_JPEG_OKAY;
            pCtx->u8FrameRdy = TRUE;
        }
        else
        {
            retVal = E_JPEG_FAILED;
        }
    }
    //    CamOsMutexUnlock(&pCtx->m_Mutex);

    return retVal;
}

MHAL_JPD_Status_e ApiJpdGetEventStatus(JPD_Ctx_t *pCtx)
{
    MS_U16            u16Event = 0;
    MHAL_JPD_Status_e eStatus;

    u16Event = MapiJpdGetEventFlag(pCtx->pDev->u32DevId);
    if (pCtx->u8FrameRdy)
    {
        if (u16Event & NJPD_EVENT_DECODE_DONE)
        {
            eStatus = E_MHAL_JPD_STATUS_DONE;
        }
        else if ((NJPD_EVENT_ERROR | NJPD_EVENT_DECODE_DONE) & u16Event)
        {
            eStatus = E_MHAL_JPD_STATUS_FAIL;
        }
        else if (NJPD_EVENT_ERROR & u16Event)
        {
            eStatus = E_MHAL_JPD_STATUS_ERROR;
        }
        else
        {
            eStatus = E_MHAL_JPD_STATUS_DECODING;
        }
    }
    else
    {
        return E_MHAL_JPD_STATUS_DECODING;
    }

    return eStatus;
}

MS_S32 ApiJpdDealHeaderInfo(JPD_Ctx_t *pCtx)
{
    JPD_LOGD("\n");

    if (JpegDealHeaderInfo(pCtx) == FALSE)
    {
        return FALSE;
    }

    return TRUE;
}
