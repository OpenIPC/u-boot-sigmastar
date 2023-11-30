/*
* SsRecovery.c- Sigmastar
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


#ifndef __SSRECOVERY__
#define __SSRECOVERY__

#include <common.h>
#include <command.h>
#include <SsRecovery.h>
#include <environment.h>
#include "../drivers/mstar/gpio/drvGPIO.h"
#include "../arch/arm/include/asm/arch-infinity/mach/io.h"
#include "../arch/arm/include/asm/arch-infinity/mach/platform.h"

/**
 * remove blank character in string str
 *
 * @param str	string should not be const
 * @return void
 */

void RemoveBlankChar(char *str)
{
    char *str_c=str;
    int i,j=0;
    for(i=0;str[i]!='\0';i++)
    {
        if(str[i]!=' ')
            str_c[j++]=str[i];
    }
    str_c[j]='\0';
    str=str_c;
}


/**
 * Copy data from emmc to ram, then write to other parts of the emmc
 *
 * @param blkOffset	block offset of the data to be copied
 * @param size		size of the data to be copied,bytes unit
 * @part			partition name of the emmc to write data
 * @return 0 if ok, others on error
 */
int RecoveryWriteCopyData(u32 blkOffset,u32 size,char *part)
    {
    int ret;
    int i;
    int batchCount;
    int blkSize;
    int blkBatch;
    int blkRemain;
    int offset;
    char commandStr[100];

    blkSize = ALIGN(size, 512) / 512;
    blkBatch = RECOVERY_DATA_COPY_BATCH / 512;

    batchCount = blkSize / blkBatch;
    blkRemain = blkSize % blkBatch;

    /* erase the partition */
    sprintf(commandStr,"emmc erase.p %s",part);
    ret = run_command(commandStr, 0);

    /* copy the batch block data */
    for(i=0;i<batchCount;i++)
    {
        offset = blkOffset + i * blkBatch;
        sprintf(commandStr,"mmc read %x %x %x",RECOVERY_DATA_COPY_ADDR,offset,blkBatch);
        ret = run_command(commandStr, 0);
        if(ret){
            return ret;
        }

        offset = i * blkBatch;
        sprintf(commandStr,"emmc write.p.continue %x %s %x %x",RECOVERY_DATA_COPY_ADDR,part,offset,RECOVERY_DATA_COPY_BATCH);
        ret = run_command(commandStr, 0);
        if(ret)
        {
            return ret;
        }
    }

        /* copy the remain block data */
    if(0 != blkRemain)
    {

        offset = blkOffset + i * blkBatch;
        sprintf(commandStr,"mmc read %x %x %x",RECOVERY_DATA_COPY_ADDR,offset,blkRemain);
        ret = run_command(commandStr, 0);
        if(ret)
        {
            return ret;
        }

        offset = i * blkBatch;
        sprintf(commandStr,"emmc write.p.continue %x %s %x %x",RECOVERY_DATA_COPY_ADDR,part,offset,(blkRemain * 512));
        ret = run_command(commandStr, 0);
        if(ret)
        {
            return ret;
        }
    }

    return 0;

}


/**
 * Check whether need to recovery the system by check the a gpio pin
 * If needed,copy the recovery data to systems zone (in emmc)
 *
 * @return 0 if ok, others on error
 */

int RecoveryCheck(void)
{
    int flag = 0;	/* record the valid paramter */
    int loop;
    int len;
    char* env;
    MS_GPIO_NUM pinNum;
    int pinLevel;
    int level1,level2;
    char strTemp[256];
    char *pStr;
    char *token;
    char delim[] = ";";
    char *recoverPrmStr[RECOVERY_PART_NUM] = {"kernel=","rootfs=","user="};
    MS_RecoveryPrm recPrm[RECOVERY_PART_NUM];
    char temp1[100],temp2[100];

    /* parse the recovery pin parameter */
    env = getenv("reckey");
    if(NULL != env)
    {

        memset(strTemp,0,sizeof(strTemp));
        memset(temp1,0,sizeof(temp1));
        memset(temp2,0,sizeof(temp2));

        strcpy(strTemp,env);
        sscanf(strTemp,"%[^=]=%d,%[^=]=%d",temp1,&pinNum,temp2,&pinLevel);

        /* check the pin*/
        MDrv_GPIO_Pad_Set(pinNum);
        mdrv_gpio_set_input(pinNum);
        level1 = mdrv_gpio_get_level(pinNum);
        if(pinLevel !=  level1)
        {
            return 1;
        }

        mdelay(10);

        level2 = mdrv_gpio_get_level(pinNum);
        if(pinLevel != level2)
        {
            return 1;
        }

        flag += 1;	/* valid pin */
    }

    /* parse the recovery offset and size parameter */
    env = getenv("recargs");
    if(NULL != env)
    {

        memset(strTemp,0,sizeof(strTemp));
        strcpy(strTemp,env);
        RemoveBlankChar(strTemp);
        pStr = strTemp;

        loop = 0;
        for(token=strsep(&pStr, delim); token!=NULL; token=strsep(&pStr, delim))
        {

            len = strlen(recoverPrmStr[loop]);
            if(!strncmp(recoverPrmStr[loop],token,len))
            {
                memset(temp1,0,sizeof(temp1));
                memset(recPrm[loop].parta,0,sizeof(recPrm[loop].parta));
                memset(recPrm[loop].partb,0,sizeof(recPrm[loop].partb));

                sscanf(token,"%[^=]=%x,%x,%[^','],%s",temp1,&recPrm[loop].offset,&recPrm[loop].size,recPrm[loop].parta,recPrm[loop].partb);

                loop++;
                if(loop >= RECOVERY_PART_NUM)
                {
                    break;
                }
            }
        }

        /* valid recovery parameter */
        if(RECOVERY_PART_NUM == loop){
            flag += 1;
        }
    }


    if(2 == flag){

        printf("recovery system start...\n");

        /* copy data from recovery to system*/
        for(loop=0;loop<RECOVERY_PART_NUM;loop++)
        {
            RecoveryWriteCopyData(recPrm[loop].offset,recPrm[loop].size,recPrm[loop].parta);
            RecoveryWriteCopyData(recPrm[loop].offset,recPrm[loop].size,recPrm[loop].partb);
        }

        return 0;
    }

    return -1;

}


/**
 * If can't boot kernel with default env,it will try the backup one
 *
 * @return 0 if ok, others on error
 */

int RecoveryToBackup(void)
{
    char *env;

    printf("do_bootm error, try backup one...\n");

    env = getenv("bootcmdbp");
    if(NULL != env)
    {
        setenv("bootcmd",env);
    }

    env = getenv("bootargsbp");
    if(NULL != env)
    {
        setenv("bootargs",env);
    }

    saveenv();

    run_command("re", 0);

    return  0;
}

#endif
