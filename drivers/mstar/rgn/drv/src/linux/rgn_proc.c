/*
* rgn_proc.c- Sigmastar
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

#define RGN_SYSFS_C
#include "drv_gop.h"
#include "drv_osd.h"
#include "drv_cover.h"

#include <linux/device.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include "rgn_sysfs_st.h"
#include "ms_msys.h"
#include "cam_os_wrapper.h"
struct proc_dir_entry *gpRgnProcDir;
#define DRV_RGN_DEVICE_MAJOR    0x87
#define DRV_RGN_DEVICE_MINOR    0x66
#define DrvRgnScnprintf(buf, size, _fmt, _args...)        seq_printf(buf, _fmt, ## _args)
#define RGN_GOP_ID_SHIFTBIT 4
#define PARSING_RGN_PORT(x)       (x==E_DRV_OSD_PORT0    ? "Port0" : \
                                   x==E_DRV_OSD_PORT1    ? "Port1" : \
                                   x==E_DRV_OSD_PORT2    ? "Port2" : \
                                   x==E_DRV_OSD_PORT3    ? "Port3" : \
                                   x==E_DRV_OSD_PORT4    ? "Port4" : \
                                   x==E_DRV_OSD_PORT5    ? "Port5" : \
                                   x==E_DRV_OSD_DIP      ? "DIP" : \
                                   x==E_DRV_OSD_LDC      ? "LDC" : \
                                   x==E_DRV_OSD_DISP0_CUR? "CUR0" : \
                                   x==E_DRV_OSD_DISP0_UI ? "UI0" : \
                                   x==E_DRV_OSD_DISP1_CUR? "CUR1" : \
                                   x==E_DRV_OSD_DISP1_UI ? "UI1" : \
                                                          "UNKNOWN")
#define PARSING_RGN_Bind(x)       (x==E_DRV_GOP_UNBIND ? "UNBIND" : \
                                                        (char[2]){'0'+x,'\0'})
#define PARSING_RGN_GOPFMT(x)       (x==E_DRV_GOP_GWIN_SRC_FMT_ARGB4444   ? "ARGB4444" : \
                                     x==E_DRV_GOP_GWIN_SRC_FMT_ARGB1555   ? "ARGB1555" : \
                                     x==E_DRV_GOP_GWIN_SRC_FMT_I8_PALETTE ? "I8" : \
                                     x==E_DRV_GOP_GWIN_SRC_I4_PALETTE     ? "I4" : \
                                     x==E_DRV_GOP_GWIN_SRC_I2_PALETTE     ? "I2" : \
                                     x==E_DRV_GOP_GWIN_SRC_FMT_ARGB8888   ? "ARGB8888" : \
                                     x==E_DRV_GOP_GWIN_SRC_FMT_RGB565     ? "RGB565" : \
                                     x==E_DRV_GOP_GWIN_SRC_FMT_UV8Y8      ? "UV8Y8" : \
                                                                           "UNKNOWN")
#define COVER_SUPPORT(x) ((RGN_CHIP_COVER_SUPPORT &(1<<(x))))

extern bool _DrvOsdTransId(DrvOsdId_e eOsdId, HalOsdIdType_e *pHalId);
//-------------------------------------------------------------------------------------------------
// internal function
//-------------------------------------------------------------------------------------------------
DrvGopIdType_e RgnGopTransId(DrvOsdId_e eOsdId)
{
    DrvGopIdType_e DrvId;
    DrvId = geGopIdFromOsd[eOsdId];
    return DrvId;
}
DrvOsdId_e RgnOsdTransId(DrvGopIdType_e eGopId)
{
    DrvOsdId_e DrvId;
    DrvGopIdType_e DrvGopId;
    for(DrvId = E_DRV_OSD_PORT0;DrvId<E_DRV_OSD_ID_MAX;DrvId++)
    {
        DrvGopId = geGopIdFromOsd[DrvId];
        if(eGopId == DrvGopId)
        {
            break;
        }
    }
    return DrvId;
}
int RgnDebugDbglvShow(struct seq_file *buf, void *v)
{
    struct seq_file *str = buf;
    void *end = (void *)buf + PAGE_SIZE;
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    DrvRgnScnprintf(str, end - str, "dbglv=%hhx\n",gbrgndbglv);
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    end = end;
    return (str - buf);
}
int RgnDebugCoverShow(struct seq_file *buf, void *v)
{
    struct seq_file *str = buf;
    void *end = (void *)buf + PAGE_SIZE;
    u32 i,j;
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    for(i=0;i<E_HAL_COVER_ID_NUM;i++)
    {
        DrvRgnScnprintf(str, end - str, "----------------------COVER_%s----------------------\n",PARSING_RGN_PORT(i));
        DrvRgnScnprintf(str, end - str, "%-8s%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n",
                                        "BwinId","X","Y","W","H","R","G","B");

        for(j=0;j<E_HAL_COVER_BWIN_ID_NUM;j++)
        {
            if(_tCoverLocSettings[i].tCoverWinsSet[j].bBwinEn)
            {
                DrvRgnScnprintf(str, end - str, "%-8d%-6d%-6d%-6d%-6d0x%-4x0x%-4x0x%-4x\n",
                    j,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinSet.u16X,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinSet.u16Y,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinSet.u16Width,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinSet.u16Height,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinColorSet.u8R,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinColorSet.u8G,
                    _tCoverLocSettings[i].tCoverWinsSet[j].tWinColorSet.u8B);
            }
        }
    }
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    end = end;
    return (str - buf);
}
int RgnDebugOsdShow(struct seq_file *buf, void *v)
{
    struct seq_file *str = buf;
    void *end = (void *)buf + PAGE_SIZE;
    u32 u32DrvOsdId,u32HalOsdId;
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    DrvRgnScnprintf(str, end - str, "%-7s%-4s%-7s%-8s\n",
                                    "OSDId","En","bUsed","BindId");
    for(u32DrvOsdId=E_DRV_OSD_PORT0;u32DrvOsdId<E_DRV_OSD_ID_MAX;u32DrvOsdId++)
    {
        if(_DrvOsdTransId(u32DrvOsdId,&u32HalOsdId)) {
            DrvRgnScnprintf(str, end - str, "%-7s%-4d%-7d%-8s\n",
                                            PARSING_RGN_PORT(u32DrvOsdId),
                                            _tOsdpLocSettings[u32HalOsdId].bOsdEn,
                                            (geGopOsdFlag&(0x1<<u32DrvOsdId))? 1 : 0,
                                            PARSING_RGN_Bind(RgnGopTransId(u32DrvOsdId)));
        }
    }
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    end = end;
    return (str - buf);
}
int RgnDebugGopShow(struct seq_file *buf, void *v)
{
    struct seq_file *str = buf;
    void *end = (void *)buf + PAGE_SIZE;
    u32 i,j;
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    for(i=0;i<HAL_RGN_GOP_NUM;i++)
    {
        DrvRgnScnprintf(str, end - str, "----------------------GOP_%d----------------------\n",i);
        DrvRgnScnprintf(str, end - str, "%-8s%-8s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n",
            "BindID","CKeyEn","R","G","B","SrcX","SrcY","SrcW","SrcH","DstX","DstY","DstW","DstH");
        DrvRgnScnprintf(str, end - str, "%-8s%-8d0x%-4x0x%-4x0x%-4x%-6d%-6d%-6d%-6d%-6d%-6d%-6d%-6d\n",
            PARSING_RGN_PORT(RgnOsdTransId(i)),
            _tGopLocSettings[i].tGopColorKeySet.bEn,
            _tGopLocSettings[i].tGopColorKeySet.u8R,
            _tGopLocSettings[i].tGopColorKeySet.u8G,
            _tGopLocSettings[i].tGopColorKeySet.u8B,
            _tGopLocSettings[i].tGopSrcStrWinSet.u16X,
            _tGopLocSettings[i].tGopSrcStrWinSet.u16Y,
            _tGopLocSettings[i].tGopSrcStrWinSet.u16Width,
            _tGopLocSettings[i].tGopSrcStrWinSet.u16Height,
            _tGopLocSettings[i].tGopDstStrWinSet.u16X,
            _tGopLocSettings[i].tGopDstStrWinSet.u16Y,
            _tGopLocSettings[i].tGopDstStrWinSet.u16Width,
            _tGopLocSettings[i].tGopDstStrWinSet.u16Height);
        DrvRgnScnprintf(str, end - str, "\n%-8s%-9s%-6s%-12s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s%-6s\n",
            "GwinId","Fmt","MemH","MemAddr","XOft","X","Y","W","H","AType","CVal","AVal0","AVal1");
        for(j=0;j<HAL_RGN_GOP_GWIN_NUM;j++)
        {
            if(_tGopLocSettings[i].tGwinLocSet[j].bGwinEn)
            {
                DrvRgnScnprintf(str, end - str, "%-8d%-9s%-6d0x%-10x%-6d%-6d%-6d%-6d%-6d%-6d0x%-4x0x%-4x0x%-4x\n",
                    j,
                    PARSING_RGN_GOPFMT(_tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.eSrcFmt),
                    _tGopLocSettings[i].tGwinLocSet[j].u32MemPitch,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.u32BaseAddr,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.u16Base_XOffset,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.tDisplayWin.u16X,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.tDisplayWin.u16Y,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.tDisplayWin.u16Width,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinGenSet.tDisplayWin.u16Height,
                    _tGopLocSettings[i].tGwinLocSet[j].tGwinAblSet.eAlphaType,
                    *_tGopLocSettings[i].tGwinLocSet[j].p8ConstantAlphaVal,
                    *_tGopLocSettings[i].tGwinLocSet[j].p8Argb1555Alpha0Val,
                    *_tGopLocSettings[i].tGwinLocSet[j].p8Argb1555Alpha1Val);
            }
        }
    }
    DrvRgnScnprintf(str, end - str, "=====================RGN Info=====================\n");
    end = end;
    return (str - buf);
}

static int rgn_gop_show(struct inode *inode, struct file *file)
{
    return single_open(file, RgnDebugGopShow, NULL);
}
static int rgn_osd_show(struct inode *inode, struct file *file)
{
    return single_open(file, RgnDebugOsdShow, NULL);
}
static int rgn_cover_show(struct inode *inode, struct file *file)
{
    return single_open(file, RgnDebugCoverShow, NULL);
}
static int rgn_dbglv_show(struct inode *inode, struct file *file)
{
    return single_open(file, RgnDebugDbglvShow, NULL);
}

static ssize_t rgn_dbglv_store (struct file *file,
        const char __user *buffer, size_t count, loff_t *pos)
{
    char buf[16];
    size_t len = min(sizeof(buf) - 1, count);

    if (copy_from_user(buf, buffer, len))
        return count;
    gbrgndbglv = CamOsStrtol(buf, NULL,16);

    return strnlen(buf, len);
}
bool bRgnInit =0;
static const struct file_operations RgnDbglvlProcOps =
{
    .owner		= THIS_MODULE,
    .open		= rgn_dbglv_show,
    .read		= seq_read,
    .llseek		= seq_lseek,
    .release	= single_release,
    .write		= rgn_dbglv_store,
};
static const struct file_operations RgnGopProcOps =
{
    .owner      = THIS_MODULE,
    .open       = rgn_gop_show,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};
static const struct file_operations RgnOsdProcOps =
{
    .owner      = THIS_MODULE,
    .open       = rgn_osd_show,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};
static const struct file_operations RgnCoverProcOps =
{
    .owner      = THIS_MODULE,
    .open       = rgn_cover_show,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};
void RgnSysfsInit(void)
{
    struct proc_dir_entry *pde;

    if(bRgnInit)
        return;
        
    bRgnInit = 1;
    gbrgndbglv = 0;
    gpRgnProcDir = proc_mkdir("mrgn", NULL);
    if (!gpRgnProcDir)
    {
        CamOsPrintf("[RGN] Can not create proc\n");
        return;
    }

    pde = proc_create("dbglv", S_IRUGO, gpRgnProcDir, &RgnDbglvlProcOps);
    if (!pde)
        goto out_dbglvl;
    pde = proc_create("gop", S_IRUGO, gpRgnProcDir, &RgnGopProcOps);
    if (!pde)
        goto out_gop;
    pde = proc_create("osd", S_IRUGO, gpRgnProcDir, &RgnOsdProcOps);
    if (!pde)
        goto out_osd;
    pde = proc_create("cover", S_IRUGO, gpRgnProcDir, &RgnCoverProcOps);
    if (!pde)
        goto out_cover;
    return ;
out_cover:
    remove_proc_entry("osd", gpRgnProcDir);
out_osd:
    remove_proc_entry("gop", gpRgnProcDir);
out_gop:
    remove_proc_entry("dbglvl", gpRgnProcDir);
out_dbglvl:
    return ;
}
void RgnSysfsDeInit(void)
{
    if(!bRgnInit)
        return;
    //ToDo
    bRgnInit = 0;
    gbrgndbglv = 0;
    remove_proc_entry("cover", gpRgnProcDir);
    remove_proc_entry("osd", gpRgnProcDir);
    remove_proc_entry("gop", gpRgnProcDir);
    remove_proc_entry("dbglv", gpRgnProcDir);
    remove_proc_entry("mrgn", NULL);
}
#undef RGN_SYSFS_C
