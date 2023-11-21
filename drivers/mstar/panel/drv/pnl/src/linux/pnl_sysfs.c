/*
* pnl_sysfs.c- Sigmastar
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

#define _PNL_SYSFS_C_
//-------------------------------------------------------------------------------------------------
//  Include Files
//-------------------------------------------------------------------------------------------------

#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/delay.h>
#include <linux/device.h>

#include "drv_pnl_os.h"
#include "cam_sysfs.h"
#include "hal_pnl_common.h"
#include "pnl_debug.h"
#include "hal_pnl_chip.h"
#include "hal_pnl_st.h"
#include "hal_pnl.h"
#include "hal_pnl_reg.h"
#include "mhal_pnl_datatype.h"
#include "mhal_pnl.h"
#include "drv_pnl_os.h"
#include "drv_pnl_if.h"
#include "drv_pnl_ctx.h"
//-------------------------------------------------------------------------------------------------
//  Defines & Macro
//-------------------------------------------------------------------------------------------------
#define PNLSYSFS_SPRINTF_STRCAT(str, _fmt, _args...) \
    do {                                   \
        char tmpstr[1024];                 \
        sprintf(tmpstr, _fmt, ## _args);   \
        strcat(str, tmpstr);               \
    }while(0)

#define PNLSYSFS_SPRINTF(str, _fmt, _args...)  sprintf(str, _fmt, ## _args)

#define PNLSYSFS_DBG(_fmt, _args...)                 \
    do{                                         \
        CamOsPrintf(PRINT_GREEN _fmt PRINT_NONE, ## _args);       \
    }while(0)



//-------------------------------------------------------------------------------------------------
//  structure
//-------------------------------------------------------------------------------------------------
typedef struct
{
    int argc;
    char *argv[200];
}PnlSysFsStrConfig_t;


//-------------------------------------------------------------------------------------------------
//  Variable
//-------------------------------------------------------------------------------------------------
u32 gu32DbgLevel = 0;

//-------------------------------------------------------------------------------------------------
//  Local Functions
//-------------------------------------------------------------------------------------------------
int _PnlSysFsSplit(char **arr, char *str,  char* del)
{
    char *cur = str;
    char *token = NULL;
    int cnt = 0;

    token = strsep(&cur, del);
    while (token)
    {
        arr[cnt] = token;
        token = strsep(&cur, del);
        cnt++;
    }
    return cnt;
}

void _PnlSysFsParsingString(char *str, PnlSysFsStrConfig_t *pstStrCfg)
{
    char del[] = " ";
    int len;

    pstStrCfg->argc = _PnlSysFsSplit(pstStrCfg->argv, (char *)str, del);
    len = strlen(pstStrCfg->argv[pstStrCfg->argc-1]);
    pstStrCfg->argv[pstStrCfg->argc-1][len-1] = '\0';

}


void _PnlDbgmgStore(PnlSysFsStrConfig_t *pstStringCfg)
{
    int ret;
    bool bParamSet = 0;

    if(pstStringCfg->argc == 1)
    {
        ret = kstrtol(pstStringCfg->argv[0], 16, (long *)&gu32DbgLevel);
        bParamSet = 1;
    }

    if(bParamSet)
    {
        MhalPnlSetDebugLevel((void *)&gu32DbgLevel);
        printk("dbg level=%lx\n", gu32DbgLevel);
    }
    else
    {
        PNLSYSFS_DBG("[LEVEL]\n");
    }
}


int _PnlDbgmgShow(char *DstBuf)
{
    int RetSprintf = -1;
    char *SrcBuf;

    SrcBuf = (char *)DrvPnlOsMemAlloc(1024*3);

    if(SrcBuf)
    {
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "------------------- PNL DBGMG ------------------- \n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "DbgLvl: 0x%08lx\n", gu32DbgLevel);
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "  NONE:   0x00000000\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "   DRV:   0x00000001\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "   HAL:   0x00000002\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "MODULE:   0x00000004\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "  CTX :   0x00000008\n");

        RetSprintf = PNLSYSFS_SPRINTF(DstBuf, "%s", SrcBuf);
        DrvPnlOsMemRelease(SrcBuf);
    }
    return RetSprintf;
}

//-----------------------------------------------------------------------------
void _PnlClkStore(PnlSysFsStrConfig_t *pstStringCfg)
{
    int ret, idx;
    char *pClkName = NULL;
    long bEn = 0;
    long u32ClkRate = 0;
    bool bParamSet = 0;
    DrvPnlCtxConfig_t stPnlCtxCfg;
    bool abEn[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_OFF_SETTING;
    u32  au32ClkRate[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_OFF_SETTING;
    u8   au8ClkName[HAL_PNL_CLK_NUM][20] = HAL_PNL_CLK_NAME;
    u8   au8ClkMuxAttr[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_MUX_ATTR;
    u8   i;

    if(strcmp(pstStringCfg->argv[0], "clktree") == 0)
    {
        pClkName = pstStringCfg->argv[0];
        ret = kstrtol(pstStringCfg->argv[1], 10, &bEn);
        for(i=0; i<HAL_PNL_CLK_NUM; i++)
        {
            ret = kstrtol(pstStringCfg->argv[2+i], 10, &u32ClkRate);
            au32ClkRate[i] = u32ClkRate;
        }
        bParamSet = 1;
    }
    else if(pstStringCfg->argc == 3)
    {
        pClkName = pstStringCfg->argv[0];
        ret = kstrtol(pstStringCfg->argv[1], 10, &bEn);
        ret = kstrtol(pstStringCfg->argv[2], 10, &u32ClkRate);
        bParamSet = 1;
    }
    else
    {
        char *SrcBuf = NULL;
        SrcBuf = (char *)DrvPnlOsMemAlloc(1024*3);
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "----------------- CLK TREE  -----------------\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "clktree [En] [VALUE0 ~ %d]\n", HAL_PNL_CLK_NUM-1);
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "----------------- DRV Update -----------------\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "[CLK Type] [En] [VALUE]\n");
        PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "----------------- CLK Type ----------------- \n");
        for(i=0; i< HAL_PNL_CLK_NUM; i++)
        {
            PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "%s, %s\n", au8ClkName[i], au8ClkMuxAttr[i] ? "CLKIdx": "ClkRate");
        }
        PNLSYSFS_DBG("%s\n", SrcBuf);
        DrvPnlOsMemRelease(SrcBuf);
        return;
    }

    if(bParamSet)
    {
        if(strcmp(pClkName, "clktree") == 0)
        {
            if(bEn)
            {
                if(DrvPnlOsSetClkOn(au32ClkRate, HAL_PNL_CLK_NUM) == 0)
                {
                    PNL_ERR("%s %d Set Clk Tree On Fail\n", __FUNCTION__, __LINE__);
                }
            }
            else
            {
                if(DrvPnlOsSetClkOff(au32ClkRate, HAL_PNL_CLK_NUM) == 0)
                {
                    PNL_ERR("%s %d Set Clk Tree Off Fail\n", __FUNCTION__, __LINE__);
                }
            }
        }
        else
        {
            if(DrvPnlIfGetClkConfig((void *)&stPnlCtxCfg, abEn, au32ClkRate, HAL_PNL_CLK_NUM) == 0)
            {
                PNL_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
            }

            for(idx=0; idx < HAL_PNL_CLK_NUM; idx++)
            {
                if(strcmp(pClkName, au8ClkName[idx]) == 0)
                {
                    abEn[idx] = bEn ? 1 : 0;
                    au32ClkRate[idx] = u32ClkRate;
                    if(DrvPnlIfSetClkConfig((void *)&stPnlCtxCfg, abEn, au32ClkRate, HAL_PNL_CLK_NUM) == 0)
                    {
                        PNL_ERR("%s %d, Set Clk Fail\n", __FUNCTION__, __LINE__);
                    }
                    break;
                }
            }

            if(idx == HAL_PNL_CLK_NUM)
            {
                PNL_ERR("%s %d, Unknown clk type: %s\n", __FUNCTION__, __LINE__, pClkName);
            }
        }
    }
}


int _PnlClkShow(char *DstBuf)
{
    bool abEn[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_OFF_SETTING;
    u32  au32ClkRate[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_OFF_SETTING;
    u8   au8ClkName[HAL_PNL_CLK_NUM][20] = HAL_PNL_CLK_NAME;
    u8   au8ClkMuxAttr[HAL_PNL_CLK_NUM] = HAL_PNL_CLK_MUX_ATTR;
    int RetSprintf = -1;
    char *SrcBuf;
    DrvPnlCtxConfig_t stPnlCtxCfg;
    u8   i;

    SrcBuf = (char *)DrvPnlOsMemAlloc(1024*3);

    if(SrcBuf)
    {
        if(DrvPnlIfGetClkConfig((void *)&stPnlCtxCfg, abEn, au32ClkRate, HAL_PNL_CLK_NUM) == 0)
        {
            PNL_ERR("%s %d, Get Clk Fail\n", __FUNCTION__, __LINE__);
        }

        for(i=0; i<HAL_PNL_CLK_NUM; i++)
        {
            PNLSYSFS_SPRINTF_STRCAT(SrcBuf, "%-20s: En:%d %s:%ld\n",
                au8ClkName[i],
                abEn[i],
                au8ClkMuxAttr[i] ? "ClkIdx" : "ClkRate",
                au32ClkRate[i]);
        }

        RetSprintf = PNLSYSFS_SPRINTF(DstBuf, "%s", SrcBuf);
        DrvPnlOsMemRelease(SrcBuf);
    }
    return RetSprintf;
}



//-----------------------------------------------------------------------------

ssize_t check_pnldbgmg_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if(NULL!=buf)
    {
        PnlSysFsStrConfig_t stStrCfg;

        _PnlSysFsParsingString((char *)buf, &stStrCfg);
        _PnlDbgmgStore(&stStrCfg);
        return n;
    }
    return 0;
}

ssize_t check_pnldbgmg_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return _PnlDbgmgShow(buf);
}

static DEVICE_ATTR(dbgmg,0644, check_pnldbgmg_show, check_pnldbgmg_store);


ssize_t check_pnlclk_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t n)
{
    if(NULL!=buf)
    {
        PnlSysFsStrConfig_t stStrCfg;

        _PnlSysFsParsingString((char *)buf, &stStrCfg);
        _PnlClkStore(&stStrCfg);
        return n;
    }
    return 0;
}

ssize_t check_pnlclk_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    return _PnlClkShow(buf);
}

static DEVICE_ATTR(clk,0644, check_pnlclk_show, check_pnlclk_store);




//-------------------------------------------------------------------------------------------------
//  Public Functions
//-------------------------------------------------------------------------------------------------

void DrvPnlSysfsInit(struct device *device)
{
    CamDeviceCreateFile(device, &dev_attr_dbgmg);
    CamDeviceCreateFile(device, &dev_attr_clk);
}
