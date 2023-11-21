/*
* drv_disp_vip.c- Sigmastar
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


#define DRV_VIP_C

//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------
#include "drv_scl_os.h"
#include "drv_scl_dbg.h"

#include "hal_scl_util.h"
#include "hal_scl_reg.h"
#include "drv_scl_vip.h"
#include "hal_scl_vip.h"
#include "drv_scl_cmdq.h"
#include "drv_scl_hvsp_st.h"
#include "drv_scl_hvsp.h"
#include "drv_scl_irq_st.h"
#include "drv_scl_irq.h"
#include "drv_scl_pq_define.h"
#include "drv_scl_pq_declare.h"
#include "drv_scl_pq.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define DRV_VIP_DBG 0
#define DRV_VIP_LDC_DMAP_align(x,align) ((x+align) & ~(align-1))
#define DMAPBLOCKUNIT       32
#define DMAPBLOCKUNITBYTE   4
#define DMAPBLOCKALIGN      4
#define DEFAULTLDCMD 0x1
#define SRAMNORMAL 0

#define DLCVariableSection 8
#define DLCCurveFitEnable 7
#define DLCCurveFitRGBEnable 13
#define DLCDitherEnable 5
#define DLCHistYRGBEnable 10
#define DLCStaticEnable 1
#define LDCFBRWDiff 4
#define LDCSWModeEnable 10
#define LDCAppointFBidx 2
#define FHD_Width   1920
#define FHD_Height  1080
#define DRV_VIP_MUTEX_LOCK()            DrvSclOsObtainMutex(_VIP_Mutex,SCLOS_WAIT_FOREVER)
#define DRV_VIP_MUTEX_UNLOCK()          DrvSclOsReleaseMutex(_VIP_Mutex)


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
s32 _VIP_Mutex = -1;
void *gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_NUM];
bool gbSRAMCkeckPass = 1;
//-------------------------------------------------------------------------------------------------
//  Loacl Functions
//-------------------------------------------------------------------------------------------------
u16 _DrvSclVipGetDlcEnableSetting(ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg)
{
    u16 u16valueForReg1Eh04;
    u16valueForReg1Eh04 = ((u16)stDLCCfg.bVariable_Section<<DLCVariableSection)|
        ((u16)stDLCCfg.bcurve_fit_en<<DLCCurveFitEnable)|
        ((u16)stDLCCfg.bcurve_fit_rgb_en<<DLCCurveFitRGBEnable)|
        ((u16)stDLCCfg.bDLCdither_en<<DLCDitherEnable)|
        ((u16)stDLCCfg.bhis_y_rgb_mode_en<<DLCHistYRGBEnable)|
        ((u16)stDLCCfg.bstatic<<DLCStaticEnable);
    return u16valueForReg1Eh04;
}
void _DrvSclVipSetDlcHistRangeEachSection(ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg)
{
    u16 u16sec;
    u16 u16tvalue;
    u16 u16Mask;
    u32 u32Reg;
    if(stDLCCfg.stFCfg.bEn)
    {
        for(u16sec=0;u16sec<VIP_DLC_HISTOGRAM_SECTION_NUM;u16sec++)
        {
            u32Reg = REG_VIP_DLC_0C_L+(((u16sec)/2)*2);
            if((u16sec%2) == 0)
            {
                u16Mask     = 0x00FF;
                u16tvalue   = ((u16)stDLCCfg.u8Histogram_Range[u16sec]);

            }
            else
            {
                u16Mask     = 0xFF00;
                u16tvalue   = ((u16)stDLCCfg.u8Histogram_Range[u16sec])<<8;
            }

            DrvSclCmdqWriteCmd(u32Reg,(u16tvalue),u16Mask,0);
        }
    }
    else
    {
        for(u16sec=0;u16sec<VIP_DLC_HISTOGRAM_SECTION_NUM;u16sec++)
        {
            HalSclVipDlcHistSetRange(stDLCCfg.u8Histogram_Range[u16sec],u16sec+1);
        }
    }
}
void _DrvSclVipSetDlcHistogramCmdqConfig
    (ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg,u16 u16valueForReg1Eh04)
{
    DrvSclCmdqWriteCmd
        (REG_VIP_DLC_04_L,u16valueForReg1Eh04,0x25a2,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_DLC_03_L,stDLCCfg.u8HistSft,0x0007,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_15_L,stDLCCfg.bstat_MIU,0x0001,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_1C_L,stDLCCfg.u8trig_ref_mode<<2,0x0004,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_18_L,(u16)(stDLCCfg.u32StatBase[0]>>4),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_19_L,(u16)(stDLCCfg.u32StatBase[0]>>20),0x07FF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_1A_L,(u16)(stDLCCfg.u32StatBase[1]>>4),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_1B_L,(u16)(stDLCCfg.u32StatBase[1]>>20),0x07FF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_DLC_08_L,stDLCCfg.bRange<<7,0x0080,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_01_L,stDLCCfg.u16Vst,0x03FF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_02_L,stDLCCfg.u16Vnd,0x03FF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_03_L,stDLCCfg.u16Hst,0x01FF,0);
    DrvSclCmdqWriteCmd
        (REG_VIP_MWE_04_L,stDLCCfg.u16Vnd,0x01FF,0);

    _DrvSclVipSetDlcHistRangeEachSection(stDLCCfg);
    DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,1);
}
void _DrvSclVipSetDlcHistogramRiuConfig(ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg,u16 u16valueForReg1Eh04)
{
    HalSclVipDlcHistVarOnOff(u16valueForReg1Eh04);
    HalSclVipSetDlcstatMIU(stDLCCfg.bstat_MIU,stDLCCfg.u32StatBase[0],stDLCCfg.u32StatBase[1]);
    HalSclVipSetDlcShift(stDLCCfg.u8HistSft);
    HAlSclVipSetDlcMode(stDLCCfg.u8trig_ref_mode);
	HAlSclVipSetDlcSWTrig(stDLCCfg.u8SWTrigger);
    HalSclVipSetDlcActWin(stDLCCfg.bRange,stDLCCfg.u16Vst,stDLCCfg.u16Hst,stDLCCfg.u16Vnd,stDLCCfg.u16Hnd);
    _DrvSclVipSetDlcHistRangeEachSection(stDLCCfg);
}
DrvSclVipDlcHistogramReport_t _DrvSclVipGetDlcHistogramConfig(void)
{
    DrvSclVipDlcHistogramReport_t stdlc;

    DrvSclOsMemset((void *)&stdlc, 0x00, sizeof(DrvSclVipDlcHistogramReport_t));
    stdlc.u32PixelCount  = HalSclVipDlcGetPC();
    stdlc.u32PixelWeight = HalSclVipDlcGetPW();
    stdlc.u8Baseidx      = HalSclVipDlcGetBaseIdx();
    stdlc.u8MaxPixel     = HalSclVipDlcGetMaxP();
    stdlc.u8MinPixel     = HalSclVipDlcGetMinP();
    return stdlc;
}
bool _DrvSclVipSetLdcFrameBufferConfig(DrvSclVipLdcMdConfig_t stLDCCfg)
{
    HalSclVipSetLdcBypass(stLDCCfg.enbypass);
    HalSclVipSetLdcHwRwDiff(((u16)stLDCCfg.u8FBrwdiff));
    HalSclVipSetLdcSwIdx(stLDCCfg.u8FBidx);
    HalSclVipSetLdcSwMode(stLDCCfg.bEnSWMode);
    if(!stLDCCfg.enbypass)
    {
        //HW issue: 64pixel mode bypass can't use;
        DrvSclVipSetLdcBankMode(stLDCCfg.enLDCType);
    }
    else
    {
        DrvSclVipSetLdcBankMode(E_DRV_SCLVIP_LDCLCBANKMODE_128);
    }
    return TRUE;
}
void DrvSclVipSetLdcBankMode(DrvSclVipLdcLcBankModeType_e enType)
{
    HalSclVipSetLdcBankMode(enType);
}
bool _DrvSclVipSetLdcFrameBufferConfigByCmdq(DrvSclVipLdcMdConfig_t stLDCCfg)
{
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_10_L, stLDCCfg.u8FBidx,BIT1|BIT0,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_11_L, stLDCCfg.u8FBrwdiff,BIT1|BIT0,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_12_L, stLDCCfg.bEnSWMode,BIT0,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_0F_L,((u16)stLDCCfg.enbypass),0x0001,0);
    if(!stLDCCfg.enbypass)
    {
        //HW issue: 64pixel mode bypass can't use;
        DrvSclCmdqWriteCmd
            (REG_SCL_LDC_09_L,((u16)stLDCCfg.enLDCType == E_DRV_SCLHVSP_LDCLCBANKMODE_64)? BIT0 :0,BIT0,
            0);
    }
    else
    {
        DrvSclCmdqWriteCmd(REG_SCL_LDC_09_L,0,BIT0,0);
    }
    DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,1);
    return TRUE;
}
void _DrvSclVipSetNlmSrambyAutodownLoad(DrvSclVipNlmSramConfig_t stCfg)
{
    HalSclVipSetAutoDownloadAddr(stCfg.u32baseadr,stCfg.u16iniaddr,VIP_NLM_AUTODOWNLOAD_CLIENT);
    HalSclVipSetAutoDownloadReq(stCfg.u16depth,stCfg.u16reqlen,VIP_NLM_AUTODOWNLOAD_CLIENT);
    HalSclVipSetAutoDownload(stCfg.bCLientEn,stCfg.btrigContinue,VIP_NLM_AUTODOWNLOAD_CLIENT);
#if DRV_VIP_DBG
    u16 u16entry;
    for(u16entry=0;u16entry<VIP_NLM_ENTRY_NUM;u16entry++)
    {
        HalSclVipGetNlmSram(u16entry);
    }
#endif
}
void _DrvSclVipSetNlmSrambyCpu(DrvSclVipNlmSramConfig_t stCfg)
{
    u16 u16entry;
    u32 u32value,u32addr;
    u32 *pu32Addr = NULL;
    for(u16entry = 0;u16entry<VIP_NLM_ENTRY_NUM;u16entry++)
    {
        u32addr  = stCfg.u32viradr + u16entry * VIP_NLM_AUTODOWNLOAD_BASE_UNIT ;// 1entry cost 16 byte(128 bit)
        pu32Addr = (u32 *)(u32addr);
        u32value = *pu32Addr;
        HalSclVipSetNlmSrambyCPU(u16entry,u32value);
#if DRV_VIP_DBG
        HalSclVipGetNlmSram(u16entry);
#endif
    }
}
u32 _DrvSclVipGetLdcDmapPitch(DrvSclVipLdcDmapConfig_t stLDCCfg)
{
    u32 u32DMAP_pitch;
    u32DMAP_pitch = ((stLDCCfg.u16DMAPWidth/DMAPBLOCKUNIT)+1);
    u32DMAP_pitch = DRV_VIP_LDC_DMAP_align(u32DMAP_pitch,DMAPBLOCKALIGN);
    u32DMAP_pitch = u32DMAP_pitch * DMAPBLOCKUNITBYTE;
    return u32DMAP_pitch;
}
void _DrvSclVipSetLdcDmapByCmdq(DrvSclVipLdcDmapConfig_t stLDCCfg,u32 u32DMAP_pitch)
{
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_0A_L,((u16)stLDCCfg.u32DMAPaddr),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_0B_L,((u16)(stLDCCfg.u32DMAPaddr>>16)),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_0C_L,((u16)u32DMAP_pitch),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_0E_L,((u16)stLDCCfg.bEnPowerSave)<<6|
        ((u16)stLDCCfg.u8DMAPoffset),0x007F,0);
    DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,1);
}
void _DrvSclVipSetLdcDmap(DrvSclVipLdcDmapConfig_t stLDCCfg,u32 u32DMAP_pitch)
{
    HalSclVipSetLdcDmapBase(stLDCCfg.u32DMAPaddr);
    HalSclVipSetLdcDmapPitch(u32DMAP_pitch);
    HalSclVipSetLdcDmapOffset(stLDCCfg.u8DMAPoffset);
    HalSclVipSetLdcDmapPS(stLDCCfg.bEnPowerSave);
}
void _DrvSclVipSetLdcSramCfgByCmdq(DrvSclVipLdcSramConfig_t stLDCCfg)
{
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_22_L,((u16)stLDCCfg.u16SRAMhoramount),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_2A_L,((u16)stLDCCfg.u16SRAMveramount),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_24_L,((u16)(stLDCCfg.u32loadhoraddr>>4)),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_25_L,((u16)(stLDCCfg.u32loadhoraddr>>20)),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_2C_L,((u16)(stLDCCfg.u32loadveraddr>>4)),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_2D_L,((u16)(stLDCCfg.u32loadveraddr>>20)),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_23_L,((u16)stLDCCfg.u16SRAMhorstr),0xFFFF,0);
    DrvSclCmdqWriteCmd
        (REG_SCL_LDC_2B_L,((u16)stLDCCfg.u16SRAMverstr),0xFFFF,0);
    DrvSclCmdqFire(E_DRV_SCLCMDQ_TYPE_IP0,1);
}
//-------------------------------------------------------------------------------------------------
//  Functions
//-------------------------------------------------------------------------------------------------
void DrvSclVipExit(void)
{
    int i;
    if(_VIP_Mutex != -1)
    {
        DrvSclOsDeleteMutex(_VIP_Mutex);
        _VIP_Mutex = -1;
    }
    for(i=0;i<E_DRV_SCLVIP_AIP_SRAM_NUM;i++)
    {
        DrvSclOsVirMemFree(gpvSRAMBuffer[i]);
    }
}
u16 _DrvSclVipGetSramBufferSize(DrvSclVipSramType_e enAIPType)
{
    u16 u16StructSize;
    switch(enAIPType)
    {
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y:
            u16StructSize = PQ_IP_YUV_Gamma_tblY_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_U:
            u16StructSize = PQ_IP_YUV_Gamma_tblU_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_V:
            u16StructSize = PQ_IP_YUV_Gamma_tblV_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_R:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_R_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_G:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_G_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_B:
            u16StructSize = PQ_IP_ColorEng_GM10to12_Tbl_B_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_R:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_R_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_G:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_G_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_B:
            u16StructSize = PQ_IP_ColorEng_GM12to10_CrcTbl_B_SRAM_SIZE_Main;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_WDR:
            u16StructSize = (81*2 *8);
            break;
        default:
            u16StructSize = 0;
            break;
    }
    return u16StructSize;
}

bool DrvSclVipInit(DrvSclVipInitConfig_t *pCfg)
{
    char word[] = {"_VIP_Mutex"};
    int idx;
    if(_VIP_Mutex != -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s already done\n", __FUNCTION__);
        return TRUE;
    }

    _VIP_Mutex = DrvSclOsCreateMutex(E_DRV_SCLOS_FIFO, word, SCLOS_PROCESS_SHARED);

    if (_VIP_Mutex == -1)
    {
        SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s create mutex fail\n", __FUNCTION__);
        return FALSE;
    }

    HalSclVipSetRiuBase(pCfg->u32RiuBase);
    DrvSclVipSramDump();
    for(idx=0;idx<E_DRV_SCLVIP_AIP_SRAM_NUM;idx++)
    {
        gpvSRAMBuffer[idx] = DrvSclOsVirMemalloc(_DrvSclVipGetSramBufferSize(idx));
    }


    return TRUE;
}
void _DrvSclVipLdcInit(void)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s: ", __FUNCTION__);
    HalSclVipSetLdc422To444Md(DEFAULTLDCMD);
    HalSclVipSetLdc444To422Md(0);
    HalSclVipLdcEco();
}
void DrvSclVipOpen(void)
{
    HalSclVipInitY2R();
}
bool DrvSclVipGetIsBlankingRegion(void)
{
    //u32 u32Events;
    if(DrvSclIrqGetIsVipBlankingRegion())
    {
        return 1;
    }
    else
    {
        //DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
        //return DrvSclIrqGetIsBlankingRegion();
        return 0;
    }
}
bool DrvSclVipGetEachDmaEn(void)
{
    return DrvSclIrqGetEachDMAEn();
}
void DrvSclVipHWInit(void)
{
    _DrvSclVipLdcInit();
    HalSclVipMcnrInit();
    HalSclVipHistInit();
    HalSclVipAipDB(0);
}
HalSclVipSramDumpType_e _DrvSclVipGetSramType(DrvSclVipSramType_e enAIPType)
{
    HalSclVipSramDumpType_e enType;
    switch(enAIPType)
    {
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_U:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GAMMA_V:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_R:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_G:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM10to12_B:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_R:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_G:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_GM12to10_B:
            enType = E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B;
            break;
        case E_DRV_SCLVIP_AIP_SRAM_WDR:
            enType = E_HAL_SCLVIP_SRAM_DUMP_WDR;
            break;
        default:
            enType = E_HAL_SCLVIP_SRAM_DUMP_NUM;
            break;
    }
    return enType;
}
void * DrvSclVipSetAipSramConfig(void * pvPQSetParameter, DrvSclVipSramType_e enAIPType)
{
    HalSclVipSramDumpType_e enType;
    bool bRet;
    //u32 u32Events;
    void * pvPQSetPara = NULL;
    enType =  _DrvSclVipGetSramType(enAIPType);
    if(pvPQSetParameter == NULL)
    {
        HalSclVipSramDump(enType,SRAMNORMAL);
        gbSRAMCkeckPass = 1;
    }
    else
    {
        //if(!DrvSclIrqGetIsBlankingRegion())
        //{
        //    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
        //}
        //[Daniel] 20170826
        bRet = HalSclVipSramDump(enType,(u32)pvPQSetParameter);

        pvPQSetPara = pvPQSetParameter;
        DRV_VIP_MUTEX_LOCK();
        DrvSclOsMemcpy(gpvSRAMBuffer[enAIPType],pvPQSetParameter,_DrvSclVipGetSramBufferSize(enAIPType));
        //gpvSRAMBuffer[enAIPType] = pvPQSetParameter;
        gbSRAMCkeckPass = bRet;
        DRV_VIP_MUTEX_UNLOCK();
    }
    return pvPQSetPara;
}
bool DrvSclVipGetSramCheckPass(void)
{
    return gbSRAMCkeckPass;
}
void DrvSclVipSramDump(void)
{
    HalSclVipSramDump(E_HAL_SCLVIP_SRAM_DUMP_IHC,SRAMNORMAL);
    HalSclVipSramDump(E_HAL_SCLVIP_SRAM_DUMP_ICC,SRAMNORMAL);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_HAL_SCLVIP_SRAM_DUMP_WDR],E_HAL_SCLVIP_SRAM_DUMP_WDR);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y],E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_U],E_DRV_SCLVIP_AIP_SRAM_GAMMA_U);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_V],E_DRV_SCLVIP_AIP_SRAM_GAMMA_V);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_R],E_DRV_SCLVIP_AIP_SRAM_GM10to12_R);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_G],E_DRV_SCLVIP_AIP_SRAM_GM10to12_G);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_B],E_DRV_SCLVIP_AIP_SRAM_GM10to12_B);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_R],E_DRV_SCLVIP_AIP_SRAM_GM12to10_R);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_G],E_DRV_SCLVIP_AIP_SRAM_GM12to10_G);
    DrvSclVipSetAipSramConfig(gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_B],E_DRV_SCLVIP_AIP_SRAM_GM12to10_B);
    gbSRAMCkeckPass = 1;

}
bool DrvSclVipGetCmdqHwDone(void)
{
    return DrvSclHvspGetCmdqDoneStatus(E_DRV_SCLHVSP_POLL_ID_VIP);
}

void * DrvSclVipGetWaitQueueHead(void)
{
    return DrvSclHvspGetWaitQueueHead();
}

void DrvSclVipSetMcnrIpmRead(bool bEn)
{
    HalSclVipSeMcnrIPMRead(bEn);
}
void DrvSclVipSetIpmConpress(bool bEn)
{
    HalSclVipSetIPMConpress(bEn);
}
void DrvSclVipSetCiirRead(bool bEn)
{
    HalSclVipSeCiirRead(bEn);
}
void DrvSclVipSetCiirWrite(bool bEn)
{
    HalSclVipSeCiirWrite(bEn);
}
void DrvSclVipSetIpmCiirRW(bool bIpmEn, bool bCiirEn)
{
    HalSclVipSetIpmCiirRW(bIpmEn, bCiirEn);
}
void DrvSclVipSetMCNR(bool bEn)
{
    HalSclVipSetMCNR(bEn);
}

bool DrvSclVipSetNlmSramConfig(DrvSclVipNlmSramConfig_t stCfg)
{
    DrvSclOsWaitForCpuWriteToDMem();
    if(stCfg.bCLientEn)
    {
        _DrvSclVipSetNlmSrambyAutodownLoad(stCfg);
    }
    else
    {
        _DrvSclVipSetNlmSrambyCpu(stCfg);
    }
    return TRUE;
}
bool DrvSclVipSetDlcHistogramConfig(ST_VIP_DLC_HISTOGRAM_CONFIG stDLCCfg)
{
    u16 u16valueForReg1Eh04;
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s: ", __FUNCTION__);
    u16valueForReg1Eh04 = _DrvSclVipGetDlcEnableSetting(stDLCCfg);
    if(stDLCCfg.stFCfg.bEn)
    {
        _DrvSclVipSetDlcHistogramCmdqConfig(stDLCCfg,u16valueForReg1Eh04);
    }
    else
    {
        _DrvSclVipSetDlcHistogramRiuConfig(stDLCCfg,u16valueForReg1Eh04);
        SCL_DBG(SCL_DBG_LV_DRVVIP(),
        "[DRVVIP]u16valueForReg1Eh04:%hx ,stDLCCfg.u32StatBase[0]:%x,stDLCCfg.u32StatBase[1]%x,stDLCCfg.bstat_MIU:%hhd\n"
            ,u16valueForReg1Eh04,stDLCCfg.u32StatBase[0],stDLCCfg.u32StatBase[1],stDLCCfg.bstat_MIU);
    }

    return TRUE;
}

u32 DrvSclVipGetDlcHistogramReport(u16 u16range)
{
    u32 u32value;
    u32value = HalSclVipDlcHistGetRange(u16range);
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s Histogram_%hd:%x \n", __FUNCTION__,u16range,u32value);
    return u32value;
}
DrvSclVipDlcHistogramReport_t DrvSclVipGetDlcHistogramConfig(void)
{
    DrvSclVipDlcHistogramReport_t stdlc;
    DrvSclOsMemset((void *)&stdlc, 0x00, sizeof(DrvSclVipDlcHistogramReport_t));
    stdlc = _DrvSclVipGetDlcHistogramConfig();
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s PixelCount:%x,PixelWeight:%x,Baseidx:%hhx \n"
        ,__FUNCTION__, stdlc.u32PixelCount,stdlc.u32PixelWeight,stdlc.u8Baseidx);
    return stdlc;
}
bool DrvSclVipSetLdcMdConfig(DrvSclVipLdcMdConfig_t stLDCCfg)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s \n", __FUNCTION__);
    DrvSclOsWaitForCpuWriteToDMem();
    if(stLDCCfg.stFCfg.bEn)
    {
        _DrvSclVipSetLdcFrameBufferConfigByCmdq(stLDCCfg);
    }
    else
    {
        _DrvSclVipSetLdcFrameBufferConfig(stLDCCfg);
    }
    return TRUE;
}
bool DrvSclVipSetLdcOnConfig(bool bEn)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s: bEn:%hhd\n", __FUNCTION__,bEn);
    DrvSclOsWaitForCpuWriteToDMem();
    if(!bEn)
    {
        //HW issue: 64pixel mode bypass can't use;
        DrvSclVipSetLdcBankMode(E_DRV_SCLVIP_LDCLCBANKMODE_128);
    }
    HalSclVipSetLdcBypass(!bEn);
    DrvSclHvspSetLdcONOFF(bEn);
    return TRUE;
}
bool DrvSclVipSetLdcDmapConfig(DrvSclVipLdcDmapConfig_t stLDCCfg)
{
    u32 u32DMAP_pitch =_DrvSclVipGetLdcDmapPitch(stLDCCfg);
    SCL_DBG(SCL_DBG_LV_DRVVIP(),
        "[DRVVIP]%s DMAP_pitch:%x,DMAPaddr:%x \n",__FUNCTION__,u32DMAP_pitch,stLDCCfg.u32DMAPaddr);
    if(stLDCCfg.stFCfg.bEn)
    {
        _DrvSclVipSetLdcDmapByCmdq(stLDCCfg,u32DMAP_pitch);
    }
    else
    {
        _DrvSclVipSetLdcDmap(stLDCCfg,u32DMAP_pitch);
    }
    return TRUE;
}
bool DrvSclVipCheckIpmResolution(void)
{
    bool bRet = 0;
    DrvSclHvspIpmConfig_t stInformCfg;
    DrvSclHvspGetFrameBufferAttribute(E_DRV_SCLHVSP_ID_1, &stInformCfg);
    if(stInformCfg.u16Fetch<=1920 && stInformCfg.u16Vsize<=1080)
    {
        bRet = 1;
    }
    return bRet;
}
bool DrvSclVipSetLdcSramConfig(DrvSclVipLdcSramConfig_t stLDCCfg)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s loadhoraddr:%x,loadveraddr:%x \n"
        ,__FUNCTION__,stLDCCfg.u32loadhoraddr,stLDCCfg.u32loadveraddr);
    if(stLDCCfg.stFCfg.bEn)
    {
        _DrvSclVipSetLdcSramCfgByCmdq(stLDCCfg);
    }
    else
    {
        HalSclVipSetLdcSramAmount(stLDCCfg.u16SRAMhoramount,stLDCCfg.u16SRAMveramount);
        HalSclVipSetLdcSramBase(stLDCCfg.u32loadhoraddr,stLDCCfg.u32loadveraddr);
        HalSclVipSetLdcSramStr(stLDCCfg.u16SRAMhorstr,stLDCCfg.u16SRAMverstr);
    }
    return TRUE;
}
bool DrvSclVipVtrackSetPayloadData(u16 u16Timecode, u8 u8OperatorID)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s \n", __FUNCTION__);
    HalSclVipVtrackSetPayloadData(u16Timecode, u8OperatorID);
    return 1;
}
bool DrvSclVipVtrackSetKey(bool bUserDefinded, u8 *pu8Setting)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s \n", __FUNCTION__);
    HalSclVipVtrackSetKey(bUserDefinded, pu8Setting);
    return 1;
}
bool DrvSclVipVtrackSetUserDefindedSetting(bool bUserDefinded, u8 *pu8Setting)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s \n", __FUNCTION__);
    HalSclVipVtrackSetUserDefindedSetting(bUserDefinded, pu8Setting);
    return 1;
}
bool DrvSclVipVtrackEnable( u8 u8FrameRate, DrvSclVipVtrackEnableType_e bEnable)
{
    SCL_DBG(SCL_DBG_LV_DRVVIP(), "[DRVVIP]%s \n", __FUNCTION__);
    HalSclVipVtrackEnable(u8FrameRate, bEnable);
    return 1;
}
bool DrvSclVipGetBypassStatus(E_DRV_SCLVIP_CONFIG_TYPE enIPType)
{
    bool bRet;
    switch(enIPType)
    {
        case E_DRV_SCLVIP_CONFIG:
            bRet = HalSclVipGetVipBypass();
            break;

        case E_DRV_SCLVIP_MCNR_CONFIG:
            bRet = HalSclVipGetMcnrBypass();
            break;

        case E_DRV_SCLVIP_LDC_CONFIG:
            bRet = HalSclVipGetLdcBypass();
            break;

        case E_DRV_SCLVIP_NLM_CONFIG:
            bRet = HalSclVipGetNlmBypass();
            break;
        default:
            bRet = 0;
            break;
    }
    return bRet;
}
void DrvSclVipCheckVipSram(u32 u32Type)
{
    u32 u32Events;
    if(!DrvSclIrqGetIsBlankingRegion())
    {
        DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
    }
    DRV_VIP_MUTEX_LOCK();
    switch(u32Type)
    {
        case E_DRV_SCLIRQ_EVENT_GAMMA_Y:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y],REG_VIP_SCNR_41_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_Y,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_Y],REG_VIP_SCNR_41_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GAMMA_U:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_U],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_U],REG_VIP_SCNR_41_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_U],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_U,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_U],REG_VIP_SCNR_41_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GAMMA_V:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_V],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUV(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_V],REG_VIP_SCNR_41_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_V],REG_VIP_SCNR_41_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaYUVCallback(E_HAL_SCLVIP_SRAM_DUMP_GAMMA_V,
                (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GAMMA_V],REG_VIP_SCNR_41_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM10to12_R:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_R],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_R],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_R],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_R,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_R],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM10to12_G:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_G],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_G],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_G],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_G,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_G],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM10to12_B:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_B],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_B],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_B],REG_SCL_HVSP1_7A_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM10to12_B,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM10to12_B],REG_SCL_HVSP1_7A_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM12to10_R:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_R],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_R],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_R],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                        HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_R,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_R],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM12to10_G:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_G],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_G],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_G],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_G,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_G],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }

        case E_DRV_SCLIRQ_EVENT_GM12to10_B:
            if((VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQCHECK)||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLCheck)
                ||(VIPSETRULE() == E_DRV_SCLOS_VIPSETRUle_CMDQALLONLYSRAMCheck))
            {
                if(HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_B],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGB(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B,
                            (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_B],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }
            else
            {
                if(HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B,
                    (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_B],REG_SCL_HVSP1_7D_L))
                {
                    break;
                }
                else
                {

                    DrvSclOsWaitEvent(DrvSclIrqGetIrqSYNCEventID(), E_DRV_SCLIRQ_EVENT_FRMENDSYNC, &u32Events, E_DRV_SCLOS_EVENT_MD_OR, 200); // get status: FRM END
                    HalSclVipGetSramDumpGammaRGBCallback(E_HAL_SCLVIP_SRAM_DUMP_GM12to10_B,
                        (u32)gpvSRAMBuffer[E_DRV_SCLVIP_AIP_SRAM_GM12to10_B],REG_SCL_HVSP1_7D_L);
                    break;
                }
            }

        default:
            break;
    }
    DRV_VIP_MUTEX_UNLOCK();

}

#undef DRV_VIP_C
