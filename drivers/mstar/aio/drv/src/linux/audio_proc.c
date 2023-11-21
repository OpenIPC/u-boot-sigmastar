/*
* audio_proc.c- Sigmastar
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

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>

//#include "cam_os_wrapper.h"
#include "audio_proc.h"

//
#include "hal_audio_common.h"
#include "hal_audio_config.h"
#include "hal_audio_sys.h"
#include "hal_audio_types.h"
#include "hal_audio_api.h"

AudProcInfoAi_t gAudProInfoAiList[AUD_AI_DEV_NUM] = {{0}};
AudProcInfoAo_t gAudProInfoAoList[AUD_AO_DEV_NUM] = {{0}};

static u32 gAudioProcInited = 0;
struct proc_dir_entry *gpRootAudioDir;

//
static int AudioAiProcShow(struct seq_file *m, void *v)
{
    int i = 0, dma = 0;
    unsigned int nSampleRate = 0;

    seq_printf(m, "\n[AUDIO AI]\n");

    for (i = 0; i < AUD_AI_DEV_NUM; i++)
    {
        dma = card.ai_devs[i].info->dma;
        HalAudDmaGetRate(dma, NULL, &nSampleRate);

        seq_printf(m, "--------\n");
        seq_printf(m, "Dev ID       = %d\n", i);
        seq_printf(m, "Enable       = %d\n", gAudProInfoAiList[i].enable);
        seq_printf(m, "SampleRate   = %d\n", nSampleRate);
        seq_printf(m, "Channel      = %d\n", gAudProInfoAiList[i].channels);
        seq_printf(m, "BitWidth     = %d\n", gAudProInfoAiList[i].bit_width);
    }

	return 0;
}

static ssize_t AudioAiProcWrite (struct file *file,
		const char __user *buffer, size_t count, loff_t *pos)
{
	char buf[] = "0x00000000\n";
	size_t len = min(sizeof(buf) - 1, count);
	unsigned long val;

	if (copy_from_user(buf, buffer, len))
		return count;
	buf[len] = 0;
	if (sscanf(buf, "%li", &val) != 1)
    {
		printk(": %s is not in hex or decimal form.\n", buf);
    }
	else
    {
        if (val >= AUD_AI_DEV_NUM)
        {
            printk(": %lu is invalid AI ID.\n", val);
        }
        else
        {
            // gAudioAiProcDevId = val;
        }
    }

	return strnlen(buf, len);
}

static int AudioAiProcOpen(struct inode *inode, struct file *file)
{
	return single_open(file, AudioAiProcShow, NULL);
}

static const struct file_operations AudioAiProcOps = {
	.owner		= THIS_MODULE,
	.open		= AudioAiProcOpen,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= AudioAiProcWrite,
};

//
static int AudioAoProcShow(struct seq_file *m, void *v)
{
    int i = 0, dma = 0;
    unsigned int nSampleRate = 0;

    seq_printf(m, "\n[AUDIO AO]\n");

    for (i = 0; i < AUD_AO_DEV_NUM; i++)
    {
        dma = card.ao_devs[i].info->dma;
        HalAudDmaGetRate(dma, NULL, &nSampleRate);

        seq_printf(m, "--------\n");
        seq_printf(m, "Dev ID       = %d\n", i);
        seq_printf(m, "Enable       = %d\n", gAudProInfoAoList[i].enable);
        seq_printf(m, "SampleRate   = %d\n", nSampleRate);
        seq_printf(m, "Channel      = %d\n", gAudProInfoAoList[i].channels);
        seq_printf(m, "BitWidth     = %d\n", gAudProInfoAoList[i].bit_width);
    }

	return 0;
}

static ssize_t AudioAoProcWrite (struct file *file,
		const char __user *buffer, size_t count, loff_t *pos)
{
	char buf[] = "0x00000000\n";
	size_t len = min(sizeof(buf) - 1, count);
	unsigned long val;

	if (copy_from_user(buf, buffer, len))
		return count;
	buf[len] = 0;
	if (sscanf(buf, "%li", &val) != 1)
    {
		printk(": %s is not in hex or decimal form.\n", buf);
    }
	else
    {
        if (val >= AUD_AO_DEV_NUM)
        {
            printk(": %lu is invalid AO ID.\n", val);
        }
        else
        {
            // gAudioAoProcDevId = val;
        }
    }

	return strnlen(buf, len);
}

static int AudioAoProcOpen(struct inode *inode, struct file *file)
{
	return single_open(file, AudioAoProcShow, NULL);
}

static const struct file_operations AudioAoProcOps = {
	.owner		= THIS_MODULE,
	.open		= AudioAoProcOpen,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
	.write		= AudioAoProcWrite,
};

//
void AudioProcInit(void)
{
    struct proc_dir_entry *pde;

    if (gAudioProcInited)
    {
        return;
    }

    gAudioProcInited = 1;

    printk("%s %d\n",__FUNCTION__,__LINE__); //

    gpRootAudioDir = proc_mkdir("audio", NULL);
	if (!gpRootAudioDir)
    {
        printk("[AUDIO PROC] proc_mkdir fail\n");
		return;
    }

    //
	pde = proc_create("ai", S_IRUGO, gpRootAudioDir, &AudioAiProcOps);
	if (!pde)
    {
        printk("[AUDIO PROC] proc_create fail - ai\n");
		goto out_ai;
    }
	pde = proc_create("ao", S_IRUGO,gpRootAudioDir, &AudioAoProcOps);
	if (!pde)
    {
        printk("[AUDIO PROC] proc_create fail - ao\n");
		goto out_ao;
    }

	return ;

out_ao:
	remove_proc_entry("ai",gpRootAudioDir);
out_ai:

    return ;
}

void AudioProcDeInit(void)
{
    if (!gAudioProcInited)
    {
        return;
    }

    gAudioProcInited = 0;

    printk("%s %d\n",__FUNCTION__,__LINE__); //

    remove_proc_entry("ao", gpRootAudioDir);
    remove_proc_entry("ai", gpRootAudioDir);
	remove_proc_entry("audio", NULL);
}

