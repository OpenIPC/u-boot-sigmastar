/*
* drv_pnl_os.c- Sigmastar
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

#define _DRV_PNL_OS_C_

#include "drv_pnl_os.h"
#include "mhal_common.h"
#include "mdrv_padmux.h"
#include "pnl_debug.h"
#include "cam_clkgen.h"
#include "cam_sysfs.h"

#include "mhal_pnl_datatype.h"
#include "mhal_pnl.h"
#include "hal_pnl_chip.h"

#include "mdrv_puse.h"

//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Structure
//-------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------

struct device_node *gpPnlDeviceNode = NULL;

//-------------------------------------------------------------------------------------------------
//  Internal Functions
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------


void * DrvPnlOsMemAlloc(u32 u32Size)
{
    return CamOsMemAlloc(u32Size);
}

void DrvPnlOsMemRelease(void *pPtr)
{
    CamOsMemRelease(pPtr);
}

void DrvPnlOsMsSleep(u32 u32Msec)
{
    CamOsMsSleep(u32Msec);
}

void DrvPnlOsUsSleep(u32 u32Usec)
{
    CamOsUsSleep(u32Usec);
}

bool DrvPnlOsPadMuxActive(void)
{
    bool bRet = mdrv_padmux_active() ? 1 : 0;
    return bRet;
}

int DrvPnlOsPadMuxGetMode(void)
{
    return mdrv_padmux_getmode(MDRV_PUSE_TTL_CLK);
}

int DrvPnlOsGetMode(u16 u16LinkType, u16 u16OutputFormatBitMode)
{
    int Ret = 0;
    u32 u32Mode = 0;

    if(gpPnlDeviceNode)
    {
        if (u16LinkType == DRV_PNL_OS_LINK_TTL)
        {
            if (u16OutputFormatBitMode == DRV_PNL_OS_OUTPUT_565BIT_MODE)
            {
                Ret = CamofPropertyReadU32(gpPnlDeviceNode, "ttl-16bit-mode", &u32Mode);
            }
            else
            {
                Ret = CamofPropertyReadU32(gpPnlDeviceNode, "ttl-24bit-mode", &u32Mode);
            }
        }
    }

    return (Ret == 0) ? u32Mode : 0;
}

bool DrvPnlOsSetDeviceNode(void *pPlatFormDev)
{
    gpPnlDeviceNode = ((struct platform_device *)pPlatFormDev)->dev.of_node;

    if(gpPnlDeviceNode)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#ifdef CONFIG_CAM_CLK
    #include "drv_camclk_Api.h"
    void **pvPnlclk = NULL;
    u32 PnlParentCnt = 1;

u8 Pnl_ClkRegister(struct device_node *device_node)
{
    u32 u32clknum;
    u32 PnlClk;
    u8 str[16];

    if(of_find_property(device_node,"camclk",&PnlParentCnt))
    {
        PnlParentCnt /= sizeof(int);
        if(PnlParentCnt < 0)
        {
            CamOsPrintf( "[%s] Fail to get parent count! Error Number : %d\n", __FUNCTION__, PnlParentCnt);
            return 0;
        }
        pvPnlclk = CamOsMemAlloc((sizeof(void *) * PnlParentCnt));
        if(!pvPnlclk){
            return 0;
        }
        for(u32clknum = 0; u32clknum < PnlParentCnt; u32clknum++)
        {
            PnlClk = 0;
            msys_of_property_read_u32_index(device_node,"camclk", u32clknum,&(PnlClk));
            if (!PnlClk)
            {
                printk( "[%s] Fail to get clk!\n", __FUNCTION__);
            }
            else
            {
                CamOsSnprintf(str, 16, "Pnl_%d ",u32clknum);
                CamClkRegister(str,PnlClk,&(pvPnlclk[u32clknum]));
            }
        }
    }
    else
    {
        CamOsPrintf( "[%s] W/O Camclk \n", __FUNCTION__);
    }
    return 1;
}
u8 Pnl_ClkUnregister(void)
{

    u32 u32clknum;

    for(u32clknum=0;u32clknum<PnlParentCnt;u32clknum++)
    {
        if(pvPnlclk[u32clknum])
        {
            CamOsPrintf("[%s] %p\n", __FUNCTION__, pvPnlclk[u32clknum]);
            CamClkUnregister(pvPnlclk[u32clknum]);
            pvPnlclk[u32clknum] = NULL;
        }
    }
    CamOsMemRelease(pvPnlclk);

    return 1;
}
#endif


bool DrvPnlOsSetClkOn(void *pClkRate, u32 u32ClkRateSize)
{
    bool bRet = TRUE;
    unsigned long clk_rate;
    unsigned long *clk_rate_buf = (unsigned long *)pClkRate;
    bool abClkMuxAttr[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_MUX_ATTR;
#ifdef CONFIG_CAM_CLK
    u32 u32clknum = 0;
    CAMCLK_Set_Attribute stSetCfg;
    CAMCLK_Get_Attribute stGetCfg;

    if(gpPnlDeviceNode && pClkRate && u32ClkRateSize == HAL_PNL_CLK_NUM)
    {
        Pnl_ClkRegister(gpPnlDeviceNode);
        for(u32clknum = 0; u32clknum < PnlParentCnt; u32clknum++)
        {
            if (pvPnlclk[u32clknum])
            {
                clk_rate = clk_rate_buf[u32clknum];
                if(abClkMuxAttr[u32clknum] == 1)
                {
                    CamClkAttrGet(pvPnlclk[u32clknum],&stGetCfg);
                    CAMCLK_SETPARENT(stSetCfg,stGetCfg.u32Parent[clk_rate]);
                    CamClkAttrSet(pvPnlclk[u32clknum],&stSetCfg);
                }
                else
                {
                    CAMCLK_SETRATE_ROUNDUP(stSetCfg,clk_rate);
                    CamClkAttrSet(pvPnlclk[u32clknum],&stSetCfg);
                }
                CamClkSetOnOff(pvPnlclk[u32clknum],1);
            }
        }
    }
#else
    u32 num_parents, idx;
    struct clk **pnl_clks;
    struct clk_hw *hw_parent;

    if(gpPnlDeviceNode && pClkRate && u32ClkRateSize == HAL_PNL_CLK_NUM)
    {
        num_parents = CamOfClkGetParentCount(gpPnlDeviceNode);
        for(idx=0;idx<u32ClkRateSize; idx++)
        {
            PNL_DBG(PNL_DBG_LEVEL_CLK, "%s %d, CLK_%d = %ld\n", __FUNCTION__, __LINE__, idx, clk_rate_buf[idx]);
        }

        PNL_DBG(PNL_DBG_LEVEL_CLK, "%s %d num_parents:%d \n", __FUNCTION__, __LINE__, num_parents);
        if(num_parents ==  HAL_PNL_CLK_NUM)
        {
            pnl_clks = CamOsMemAlloc((sizeof(struct clk *) * num_parents));

            if(pnl_clks == NULL)
            {
                PNL_ERR( "%s %d Alloc pnl_clks is NULL\n" , __FUNCTION__, __LINE__);
                return 0;
            }

            for(idx=0; idx<num_parents; idx++)
            {
                pnl_clks[idx] = of_clk_get(gpPnlDeviceNode, idx);
                if (IS_ERR(pnl_clks[idx]))
                {
                    PNL_ERR( "%s %d, Fail to get [Pnl] %s\n" ,
                        __FUNCTION__, __LINE__, CamOfClkGetParentName(gpPnlDeviceNode, idx) );
                    CamOsMemRelease(pnl_clks);
                    return 0;
                }

                if(abClkMuxAttr[idx] == 1)
                {
                    clk_rate = clk_rate_buf[idx];
                    hw_parent = CamClkGetParentByIndex(__CamClkGetHw(pnl_clks[idx]), clk_rate);
                    CamClkSetParent(pnl_clks[idx], hw_parent->clk);
                }
                else
                {
                    clk_rate = CamClkRoundRate(pnl_clks[idx], clk_rate_buf[idx]);
                    CamClkSetRate(pnl_clks[idx], clk_rate + 1000000);
                }

                PNL_DBG(PNL_DBG_LEVEL_CLK, "%s %d, [Pnl] num_parents:%d-%d %20s %ld\n" ,
                            __FUNCTION__, __LINE__,
                            num_parents, idx+1,
                            CamOfClkGetParentName(gpPnlDeviceNode, idx),
                            clk_rate);

                CamClkPrepareEnable(pnl_clks[idx]);
                clk_put(pnl_clks[idx]);
            }

            CamOsMemRelease(pnl_clks);
        }
        else
        {
            bRet = FALSE;
            PNL_ERR("%s %d, num_parents %d != %d\n", __FUNCTION__, __LINE__, num_parents, HAL_PNL_CLK_NUM);
        }
    }
    else
    {
        bRet = FALSE;
        PNL_ERR("%s %d, Param Null, DeviceNode:%x, ClkRate:%x, ClkSize:%ld\n",
            __FUNCTION__, __LINE__, gpPnlDeviceNode, pClkRate, u32ClkRateSize);
    }
#endif
    return bRet;
}

bool DrvPnlOsSetClkOff(void *pClkRate, u32 u32ClkRateSize)
{
    bool bRet = TRUE;
#ifdef CONFIG_CAM_CLK
    u32 u32clknum = 0;

    for(u32clknum = 0; u32clknum < PnlParentCnt; u32clknum++)
    {
        if (pvPnlclk[u32clknum])
        {
            CamClkSetOnOff(pvPnlclk[u32clknum],0);
        }
    }
    Pnl_ClkUnregister();
#else
    u32 num_parents, idx;
    struct clk_hw *hw_parent;
    struct clk **pnl_clks;
    unsigned long clk_rate;
    unsigned long *clk_rate_buf = (unsigned long *)pClkRate;
    bool abClkMuxAttr[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_MUX_ATTR;

    if(gpPnlDeviceNode)
    {
        num_parents = CamOfClkGetParentCount(gpPnlDeviceNode);

        PNL_DBG(PNL_DBG_LEVEL_CLK, "%s %d num_parents:%d\n" , __FUNCTION__, __LINE__, num_parents);
        if(num_parents == HAL_PNL_CLK_NUM)
        {
            pnl_clks = CamOsMemAlloc((sizeof(struct clk *) * num_parents));

            if(pnl_clks == NULL)
            {
                PNL_ERR( "%s %d Alloc pnl_clks is NULL\n" , __FUNCTION__, __LINE__);
                return 0;
            }


            for(idx=0; idx<num_parents; idx++)
            {
                pnl_clks[idx] = of_clk_get(gpPnlDeviceNode, idx);
                if (IS_ERR(pnl_clks[idx]))
                {
                    PNL_ERR( "%s %d, Fail to get [Pnl] %s\n" ,
                        __FUNCTION__, __LINE__, CamOfClkGetParentName(gpPnlDeviceNode, idx) );
                    CamOsMemRelease(pnl_clks);
                    return 0;
                }
                else
                {
                    PNL_DBG(PNL_DBG_LEVEL_CLK,"%s %d, [Pnl] %d-%d, %20s Off\n",
                        __FUNCTION__, __LINE__, num_parents, idx+1,
                        CamOfClkGetParentName(gpPnlDeviceNode, idx));

                    if(abClkMuxAttr[idx] == 1)
                    {
                        clk_rate = clk_rate_buf[idx];
                        hw_parent = CamClkGetParentByIndex(__CamClkGetHw(pnl_clks[idx]), clk_rate);
                        CamClkSetParent(pnl_clks[idx], hw_parent->clk);
                    }
                    else
                    {
                        clk_rate = CamClkRoundRate(pnl_clks[idx], clk_rate_buf[idx]);
                        CamClkSetRate(pnl_clks[idx], clk_rate + 1000000);
                    }


                    CamClkDisableUnprepare(pnl_clks[idx]);
                }
                clk_put(pnl_clks[idx]);
            }
            CamOsMemRelease(pnl_clks);
        }
        else
        {
            bRet = FALSE;
        }
    }
    else
    {
        bRet = FALSE;
    }
#endif
    return bRet;
}



EXPORT_SYMBOL(MhalPnlCreateInstance);
EXPORT_SYMBOL(MhalPnlCreateInstanceEx);
EXPORT_SYMBOL(MhalPnlDestroyInstance);
EXPORT_SYMBOL(MhalPnlSetParamConfig);
EXPORT_SYMBOL(MhalPnlGetParamConfig);
EXPORT_SYMBOL(MhalPnlSetMipiDsiConfig);
EXPORT_SYMBOL(MhalPnlGetMipiDsiConfig);
EXPORT_SYMBOL(MhalPnlSetSscConfig);
EXPORT_SYMBOL(MhalPnlSetTimingConfig);
EXPORT_SYMBOL(MhalPnlGetTimingConfig);
EXPORT_SYMBOL(MhalPnlSetPowerConfig);
EXPORT_SYMBOL(MhalPnlGetPowerConfig);
EXPORT_SYMBOL(MhalPnlSetBackLightOnOffConfig);
EXPORT_SYMBOL(MhalPnlGetBackLightOnOffConfig);
EXPORT_SYMBOL(MhalPnlSetBackLightLevelConfig);
EXPORT_SYMBOL(MhalPnlGetBackLightLevelConfig);
EXPORT_SYMBOL(MhalPnlSetDrvCurrentConfig);
EXPORT_SYMBOL(MhalPnlSetTestPatternConfig);
EXPORT_SYMBOL(MhalPnlSetDebugLevel);
EXPORT_SYMBOL(MhalPnlSetUnifiedParamConfig);
EXPORT_SYMBOL(MhalPnlGetUnifiedParamConfig);
EXPORT_SYMBOL(DrvPnlOsMemRelease);
EXPORT_SYMBOL(DrvPnlOsMemAlloc);

