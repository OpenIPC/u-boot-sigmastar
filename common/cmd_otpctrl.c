/*
* cmd_otpctrl.c- Sigmastar
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

    if (argc < 3)
    {
        return CMD_RET_USAGE;
    }

    if ((!strncmp(argv[1], "-w", strlen("-w"))))
    {
        if (argc < 5)
        {
            return CMD_RET_USAGE;
        }

        cmd = (u32)simple_strtoul(argv[2], NULL, 16);
        if(cmd >= 0x100)
        {
            printf("[ERR] otp cmd 0x%lx out of range 0~0xFF\n", cmd);
            return CMD_RET_USAGE;
        }

        off = (u32)simple_strtoul(argv[3], NULL, 16);
        val = (u32)simple_strtoul(argv[4], NULL, 16);
        otp_ctrl_W((cmd<<CMD_OFFSET|off),val);
        otp_show_results(0x1);
    }
    else if ((!strncmp(argv[1], "-r", strlen("-r"))))
    {
        cmd = (u32)simple_strtoul(argv[2], NULL, 16);
        if(cmd >= 0x100)
        {
            printf("[ERR] otp cmd 0x%lx out of range 0~0xFF\n", cmd);
            return CMD_RET_USAGE;
        }

        off = 0;
        otp_ctrl_R((cmd<<CMD_OFFSET|off));
        otp_show_results(0x2);
    }

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
    otpctrl, CONFIG_SYS_MAXARGS, 1,	do_otp,
    "Used for otp program/read.",
    "Usage: otpctrl [-r] [otp command] for reading data from otp\n\r"
    "               otpctrl [-w] [otp command] [address offset] [data] for programing data to otp\n\r"
    "otp command:   0x0 > RSA Public N Key\n\r"
    "               0x1 > RSA Public E Key\n\r"
);


#if (defined(CONFIG_ARCH_INFINITY7) || defined(CONFIG_ARCH_MERCURY6P))

#define READ_WORD(_reg)             (*(volatile unsigned short*)(_reg))
#define WRITE_WORD(_reg, _val)      {(*((volatile unsigned short*)(_reg))) = (unsigned short)(_val); }

#define REG_ADDR_BASE_OTP5 (0x1f000000UL+(0x1025UL<<9))
#define REG_ADDR_BASE_OTP0 (0x1f000000UL+(0x1018UL<<9))
#define GET_REG16_ADDR(x, y)        (x+(y)*4)

#define OTP5_READ(addr)              READ_WORD(GET_REG16_ADDR(REG_ADDR_BASE_OTP5, addr))
#define OTP5_WRITE(addr, val)        WRITE_WORD(GET_REG16_ADDR(REG_ADDR_BASE_OTP5, addr),(val))

#define OTP0_READ(addr)              READ_WORD(GET_REG16_ADDR(REG_ADDR_BASE_OTP0, addr))
#define OTP0_WRITE(addr, val)        WRITE_WORD(GET_REG16_ADDR(REG_ADDR_BASE_OTP0, addr),(val))


void OTP5ReadformPoint(u32 u32point)
{
    u32 local;

    if(u32point >= 8)
    {
        printf("[U-Boot] unknow status\n");
        return;
    }
    else if(u32point == 0 )
    {
        u32point = 8;
    }

#if defined(CONFIG_ARCH_INFINITY7)
    local = (u32point-1)*2;
    printf("[U-Boot] p%x %x\n\r",u32point-1,OTP5_READ(0x40+local)|(OTP5_READ(0x41+local)<<16));

#elif defined(CONFIG_ARCH_MERCURY6P)
    local = (u32point-1)*4;
    printf("[U-Boot]< Low32Bit> p%x : %x\n\r",u32point-1,OTP5_READ(0x40+local)|(OTP5_READ(0x41+local)<<16));
    printf("[U-Boot]<High32bit> p%x : %x\n\r",u32point-1,OTP5_READ(0x42+local)|(OTP5_READ(0x43+local)<<16));
#endif

}

int do_otp_read_trig(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u32 addr = 0;
    u32 value=0;

    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }

    addr = (u32)simple_strtoul(argv[1], NULL, 16);

    OTP0_WRITE(0x0,0x9503);         //setting trig cmd
    OTP0_WRITE(0x1,0x8001);         //cmd trig
    udelay(11);
    //check trig enable
    while( (OTP5_READ(0x32) & 0x1) != 0x1 )
    {
        ;
    }

    /*
     *we can set the otp read address trig and wait 1us,then can read value in OTP.
     *in the trig read mode,this process always could be read other address in OTP until trig read mode exit
    */
#if defined(CONFIG_ARCH_INFINITY7)
    OTP5_WRITE(0x31,addr);          //set customer addr
#elif defined(CONFIG_ARCH_MERCURY6P)
    /*
     *Double pointer, one operation can read otp data of two addresses,
     *address1 [36] is in the lower 32 bits, address2 [37] is in the upper 32 bits
    */
    OTP5_WRITE(0x36,addr);          //set customer addr1
    OTP5_WRITE(0x37,addr+1);        //set customer addr2
#endif

    OTP5_WRITE(0x30,1);             //trig read
    udelay(1);
    value = OTP5_READ(0x34)&0x7;    //get piont
    OTP5ReadformPoint(value);

    OTP5_WRITE(0x30,0x100);         //exit trig read mode

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
    otpread, CONFIG_SYS_MAXARGS, 1,   do_otp_read_trig,
    "Only read customer area otp.",
    "Usage: otpread [Customer OTP Address]\n\r"
);
#endif

