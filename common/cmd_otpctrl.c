/* SigmaStar trade secret */
/*
* cmd_otpctrl.c - Sigmastar
*
* Copyright (C) 2020 Sigmastar Technology Corp.
*
* Author: nick.lin <nick.lin@sigmastar.com.tw>
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#include <common.h>
#include <command.h>
#include <environment.h>
#include <malloc.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"


#define CMD_OFFSET 24
#define OFFSET_ERR_CODE    0x0
#define OFFSET_DATA_SZ     0x2
#define OFFSET_DATA        0x4

#define DBUG_LOG_R_8(_off)             (*(volatile unsigned char*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))
#define DBUG_LOG_R_16(_off)            (*(volatile unsigned short*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))
#define DBUG_LOG_W_16(_off,_val)       {(*((volatile unsigned short*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))) = (unsigned short)(_val); }

void otp_ctrl_W(unsigned long  otp_cmd,unsigned long  val)
{
    __asm__ __volatile__("stmfd sp!,{r0-r3,lr}\n"
                         "mov r0,%0\n"
                         "mov r1,%1\n"
                         "swi 0x1\n"
                         "ldmfd sp!,{r0-r3,pc}\n"
                                       :
                                       :"r"(otp_cmd),"r"(val)
                                       :"memory");
}

void otp_ctrl_R(unsigned long  otp_cmd)
{
    __asm__ __volatile__("stmfd sp!,{r0-r3,lr}\n"
                         "mov r0,%0\n"
                         "swi 0x2\n"
                         "ldmfd sp!,{r0-r3,pc}\n"
                                       :
                                       :"r"(otp_cmd)
                                       :"memory");
}

void otp_show_results(unsigned long op)
{
    int err_code = DBUG_LOG_R_16(OFFSET_ERR_CODE);

    if (err_code) {
       printf("[ERR] otp control failure, error code:%x\n",err_code);
       return;
    }

    if (op == 0x2) {
        int i,j,k;
        unsigned long size = DBUG_LOG_R_16(OFFSET_DATA_SZ);

        if (size<=0)
            return;

        printf("Results: \n\r");

        for (i=0;i<size;i+=16)
        {
            k = (size-i) >16? 16:(size-i);
            printf("0x%02x:: ",i);
            for (j=i;j<i+k;j++)
            {
                printf("0x%02x ",DBUG_LOG_R_8(j+OFFSET_DATA));
            }

            printf("\n\r");
        }

        DBUG_LOG_W_16(OFFSET_DATA_SZ,0);
    }

}

int do_otp(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long  cmd = 0;
    unsigned long  off = 0;
    unsigned long  val = 0;

    if ((!strncmp(argv[1], "-w", strlen("-w"))))
    {
        if (argc < 5)
    	{
    		return CMD_RET_USAGE;
    	}

        cmd = (u32)simple_strtoul(argv[2], NULL, 16);
        off = (u32)simple_strtoul(argv[3], NULL, 16);
        val = (u32)simple_strtoul(argv[4], NULL, 16);
        otp_ctrl_W((cmd<<CMD_OFFSET|off),val);
        otp_show_results(0x1);
    }
    else if ((!strncmp(argv[1], "-r", strlen("-r"))))
    {
        if (argc < 3)
    	{
    		return CMD_RET_USAGE;
    	}

        cmd = (u32)simple_strtoul(argv[2], NULL, 16);
        off = 0;//(u32)simple_strtoul(argv[3], NULL, 16);
        otp_ctrl_R((cmd<<CMD_OFFSET|off));
        otp_show_results(0x2);
    }

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	otpctrl, CONFIG_SYS_MAXARGS, 1,	do_otp,
	"Used for otp program/read.",
	"Usage: otpctrl [-r] [otp command] for reading data from otp\n\r"
	"             otpctrl [-w] [otp command] [address offset] [data] for programing data to otp\n\r"
	"otp command: 0x0 > RSA Public N Key\n\r"
	"             0x1 > RSA Public E Key\n\r"
);

