/*
* cmd_otpburn.c- Sigmastar
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
* cmd_otpburn.c - Sigmastar
*
* Copyright (C) 2020 Sigmastar Technology Corp.
*
* Author: winky.she <winky.she@sigmastar.com.tw>
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
#include "ipl.h"

/*==========================================================================
    //OTP operation & info
===========================================================================*/
/* ERROR CODE*/
#define INVALID_CMD_IDX            0xFF01
#define INVALID_OTP_ADR            0xFF02
#define INVALID_OTP_PERMISSION     0xFF03

/* R/W buffer*/
#define CMD_OFFSET 24
#define OFFSET_ERR_CODE    0x0
#define OFFSET_DATA_SZ     0x2
#define OFFSET_DATA        0x4

#define DBUG_LOG_R_32(_off)         (*(volatile unsigned int*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))
#define DBUG_LOG_R_16(_off)         (*(volatile unsigned short*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))
#define DBUG_LOG_R_8(_off)          (*(volatile unsigned char*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))
#define DBUG_LOG_W_16(_off,_val)      {(*((volatile unsigned short*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))) = (unsigned short)(_val); }
#define DBUG_LOG_W_8(_off,_val)       {(*((volatile unsigned char*)(CONFIG_OTPCTRL_LOG_ADDRESS+_off))) = (unsigned char)(_val); }

/* operation*/
extern void otp_ctrl_W(unsigned long  otp_cmd,unsigned long  val);
extern void otp_ctrl_R(unsigned long  otp_cmd);

/* otp command */
enum {
    OTP_RSA_N = 0x0,
    OTP_RSA_E,
    OTP_Secure_Boot,
    OTP_RSA_Key_L_B,
    OTP_Key1,
    OTP_Key4,
    OTP_Key_L_B,
    OTP_Key_3_Chk,
    OTP_Key4_Chk,
    OTP_CID,
    OTP_CID_Chk,
    OTP_Password_Mode,
    OTP_Password,
    OTP_Password_Chk,
    OTP_RSA_Key_Digest, /*0xE*/
    OTP_Version_Ctl,
    OTP_Jtag_Disable,
    OTP_UUID,
    OTP_ALL,
    OTP_MAX
};

/*==========================================================================
    //log format for program flow
===========================================================================*/
#define HEADER_SIZE              4
#define LENGTH_SIZE              1
#define UUID_SIZE                6
#define CMD_SIZE                 1
#define DAT_SIZE                 2
#define RESERVE_SIZE             4
#define CHECKSUM_SIZE            2

#define OFFSET_HEADER            0
#define OFFSET_LENGTH            (OFFSET_HEADER  +  HEADER_SIZE)
#define OFFSET_UUID              (OFFSET_LENGTH  +  LENGTH_SIZE)
#define OFFSET_CMD               (OFFSET_UUID    +  UUID_SIZE)
#define OFFSET_DAT               (OFFSET_CMD     +  CMD_SIZE)
    #define DATA_BURNING         BIT0
    #define DATA_BURNED          BIT1
    #define DATA_SEBTEN_ERR      BIT2
    #define DATA_JTAGDIS_ERR     BIT3
    #define DATA_RSA_N_ERR       BIT4
    #define DATA_RSA_E_ERR       BIT5
    #define DATA_RSA_LOCK_ERR    BIT6
    #define DATA_RSA_BLOCK_ERR   BIT7
    #define DATA_BURN_DONE       BIT8
#define OFFSET_RESERVE           (OFFSET_DAT    +  DAT_SIZE)
#define OFFSET_CHECKSUM          (OFFSET_RESERVE +  RESERVE_SIZE)

#define FORMAT_HEADER            0x8E7F2701
#define FORMAT_LENGTH            (HEADER_SIZE+LENGTH_SIZE+UUID_SIZE+CMD_SIZE+DAT_SIZE+RESERVE_SIZE+CHECKSUM_SIZE)
#define FORMAT_CMD               1
#define FORMAT_RESERVE           0

#define DEBUG 1
#if DEBUG
#define BURN_MSG(fmt,args...)   printf("<otpburn %s[%d]> "fmt, __FUNCTION__, __LINE__, ##args)
void dump_NKey(U8* start, U16 len)
{
    U32 i;

    printf("--------dump_NKey start--------");

    for(i = 0; i < len; i++)
    {
        if(i % 0x10 == 0)
        {
            printf("\n");
            printf("%08x: ", i);
        }

        printf(" 0x%02x", *(start+i));
    }

    printf("\n--------dump_NKey end--------\n");
}
#else
#define BURN_MSG(x)
#define dump_NKey(a, b)
#endif

U32 Get_NKey_CheckSum(U32 Keyaddr)
{
    U16 i;
    U32 KeySumCalc = 0;

    for(i=0; i<256; i=i+4)
    {
        KeySumCalc += *((volatile U32*)(Keyaddr + i));
    }

    return KeySumCalc;
}

void Read_OTP_UUID(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_UUID << CMD_OFFSET);
    mdelay(10);
}

void Read_OTP_RSA_NKey(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_RSA_N << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_RSA_NKey(U32 Keyaddr)
{
    u32 off = 0;

    BURN_MSG("Program_OTP_RSA_NKey\n");

    mdelay(10);
    for(off = 0; off < 256; off=off+4)
    {
        BURN_MSG("0x%08x\n", *(U32*)(Keyaddr+off));
        otp_ctrl_W(OTP_RSA_N << CMD_OFFSET | off, *((volatile U32*)(Keyaddr+off)));
    }
    mdelay(10);
}

void Read_OTP_RSA_EKey(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_RSA_E << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_RSA_EKey(void)
{
    BURN_MSG("Program_OTP_RSA_EKey\n");

    mdelay(10);
    otp_ctrl_W(OTP_RSA_E << CMD_OFFSET, 0x01000100);
    mdelay(10);
}

void Read_OTP_RSA_Lock(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_RSA_Key_L_B << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_RSA_Lock(void)
{
    BURN_MSG("Program_OTP_RSA_Lock\n");

    mdelay(10);
    otp_ctrl_W(OTP_RSA_Key_L_B << CMD_OFFSET, 0x0C);
    mdelay(10);
}

void Read_OTP_RSA_Block(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_RSA_Key_L_B << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_RSA_Block(void)
{
    BURN_MSG("Program_OTP_RSA_Block\n");

    mdelay(10);
    otp_ctrl_W(OTP_RSA_Key_L_B << CMD_OFFSET, 0x30);
    mdelay(10);
}

void Read_OTP_SecuriryBoot_Enable(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_Secure_Boot << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_SecuriryBoot_Enable(void)
{
    BURN_MSG("Program_OTP_SecuriryBoot_Enable\n");

    mdelay(10);
    otp_ctrl_W(OTP_Secure_Boot << CMD_OFFSET, 0xFF);
    mdelay(10);
}

void Read_OTP_Jtag_Disable(void)
{
    mdelay(10);
    otp_ctrl_R(OTP_Jtag_Disable << CMD_OFFSET);
    mdelay(10);
}

void Program_OTP_Jtag_Disable(void)
{
    BURN_MSG("Program_OTP_Jtag_Disable\n");

    mdelay(10);
    otp_ctrl_W(OTP_Jtag_Disable << CMD_OFFSET, 0xF0);
    mdelay(10);
}

U8 IS_BURN_RSA_NKey(U32 KeySum)
{
    U32 KeySumCalc = 0;

    Read_OTP_RSA_NKey();

    BURN_MSG("IS_BURN_RSA_NKey\n");
    dump_NKey((U8 *)(CONFIG_OTPCTRL_LOG_ADDRESS + OFFSET_DATA), 256);

    KeySumCalc = Get_NKey_CheckSum(CONFIG_OTPCTRL_LOG_ADDRESS + OFFSET_DATA);

    BURN_MSG("KeySum: 0x%08x\n", KeySum);
    BURN_MSG("KeySumCalc: 0x%08x\n", KeySumCalc);

    if(KeySumCalc == KeySum)
    {
        return 1;
    }
    else if( KeySumCalc != 0)
    {
        return 2;
    }
    else
    {
        return 0;
    }
}

U8 IS_BURN_RSA_EKey(void)
{
    U32 val = 0;

    Read_OTP_RSA_EKey();
    val = DBUG_LOG_R_32(OFFSET_DATA);

    BURN_MSG("IS_BURN_RSA_EKey val: 0x%02x\n", val);

    if(val == 0x01000100)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

U8 IS_BURN_RSA_Lock(void)
{
    U8 val = 0;

    Read_OTP_RSA_Lock();
    val = DBUG_LOG_R_8(OFFSET_DATA);

    BURN_MSG("IS_BURN_RSA_Lock val: 0x%02x\n", val);

    if((val & 0x0C) == 0x0C)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

U8 IS_BURN_RSA_Block(void)
{
    U8 val = 0;

    Read_OTP_RSA_Block();
    val = DBUG_LOG_R_8(OFFSET_DATA);

    BURN_MSG("IS_BURN_RSA_Block val: 0x%02x\n", val);

    if((val & 0x30) == 0x30)
    {
        return 1;
    }
    else
    {
       return 0;
    }
}

U8 IS_BURN_SecuriryBoot_Enable(void)
{
    U8 val = 0;

    Read_OTP_SecuriryBoot_Enable();
    val = DBUG_LOG_R_8(OFFSET_DATA);

    BURN_MSG("IS_BURN_SecuriryBoot_Enable val: 0x%02x\n", val);

    if(val == 0xFF)
    {
        return 1;
    }
    else
    {
       return 0;
    }
}

U8 IS_BURN_Jtag_Disable(void)
{
    U8 val = 0;

    Read_OTP_Jtag_Disable();
    val = DBUG_LOG_R_8(OFFSET_DATA);

    BURN_MSG("IS_BURN_Jtag_Disable val: 0x%02x\n", val);

    if((val & 0xF0) == 0xF0)
    {
        return 1;
    }
    else
    {
       return 0;
    }
}

void OTP_LOG_Send(U16 data, U32 reserve)
{
    U16 i = 0, checksum = 0;
    volatile char log[FORMAT_LENGTH] = {0};

    //Header
    log[OFFSET_HEADER]    =  (FORMAT_HEADER>>24) & 0xFF;
    log[OFFSET_HEADER+1]  =  (FORMAT_HEADER>>16) & 0xFF;
    log[OFFSET_HEADER+2]  =  (FORMAT_HEADER>>8)  & 0xFF;
    log[OFFSET_HEADER+3]  =  (FORMAT_HEADER>>0)  & 0xFF;
    //Length
    log[OFFSET_LENGTH]    =  FORMAT_LENGTH;
    //UUID
    Read_OTP_UUID();
    for(i = 0; i < 6; i++)
    {
        log[OFFSET_UUID + (5-i)] = DBUG_LOG_R_8(OFFSET_DATA+i);
    }

    //Cmd
    log[OFFSET_CMD]       =  FORMAT_CMD;
    //DATA
    log[OFFSET_DAT]       =  (data>>8) & 0xFF;
    log[OFFSET_DAT+1]     =  data & 0xFF;
    //RESERVE
    log[OFFSET_RESERVE]   =  (reserve>>24) & 0xFF;
    log[OFFSET_RESERVE+1] =  (reserve>>16) & 0xFF;
    log[OFFSET_RESERVE+2] =  (reserve>>8) & 0xFF;
    log[OFFSET_RESERVE+3] =   reserve & 0xFF;
    //CHECKSUM
    for(i = 0; i < OFFSET_CHECKSUM; i++)
    {
        checksum += log[i];
    }
    log[OFFSET_CHECKSUM] = (checksum>>8) & 0xFF;
    log[OFFSET_CHECKSUM+1] = checksum & 0xFF;
    //uart tx send log
    for(i=0;i<FORMAT_LENGTH;i++)
    {
        serial_putc(log[i]);
    }
}

int BURN_OTP_Start(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    U16 RSA_NKey_Flag = 0, RSA_EKey_Flag = 0;
    U16 RSA_Lock_Flag = 0, RSA_Block_Flag = 0;
    U16 SecuriryBoot_Flag = 0, Jtag_Disable_Flag = 0;
    U32 Keyaddr = 0, KeySum = 0, KeySumCalc = 0;
    U32 loader_addr = 0;

    if (argc < 2)
    {
        return CMD_RET_USAGE;
    }
    else
    {
        loader_addr = (u32)simple_strtoul(argv[1], NULL, 16);
    }

    BURN_MSG("BURN_OTP_Start\n");

    Keyaddr = loader_addr + (U32)image_get_ipl_size(loader_addr) - 256 - 256;
    KeySum = *((volatile U32*)(Keyaddr - 4));
    KeySumCalc = Get_NKey_CheckSum(Keyaddr);
    KeySumCalc &= 0xFFFFFFFF;

    dump_NKey((U8 *)Keyaddr, 256);
    BURN_MSG("Keyaddr: 0x%8x\n", Keyaddr);
    BURN_MSG("KeySum: 0x%8x\n", KeySum);
    BURN_MSG("KeySumCalc: 0x%8x\n", KeySumCalc);

    if(KeySumCalc != KeySum)
    {
        printf("BURN_OTP Key Check Sum Err!\n");
        return CMD_RET_USAGE;
    }
    RSA_NKey_Flag      =  IS_BURN_RSA_NKey(KeySum);
    RSA_EKey_Flag      =  IS_BURN_RSA_EKey();
    RSA_Lock_Flag      =  IS_BURN_RSA_Lock();
    RSA_Block_Flag     =  IS_BURN_RSA_Block();
    SecuriryBoot_Flag  =  IS_BURN_SecuriryBoot_Enable();
    Jtag_Disable_Flag  =  IS_BURN_Jtag_Disable();
    BURN_MSG("RSA_NKey_Flag: %d\n", RSA_NKey_Flag);
    BURN_MSG("RSA_EKey_Flag: %d\n", RSA_EKey_Flag);
    BURN_MSG("RSA_Lock_Flag: %d\n", RSA_Lock_Flag);
    BURN_MSG("RSA_Block_Flag: %d\n", RSA_Block_Flag);
    BURN_MSG("SecuriryBoot_Flag: %d\n", SecuriryBoot_Flag);
    BURN_MSG("Jtag_Disable_Flag: %d\n", Jtag_Disable_Flag);

    if((RSA_NKey_Flag == 1) && RSA_EKey_Flag && RSA_Lock_Flag && RSA_Block_Flag && SecuriryBoot_Flag && Jtag_Disable_Flag)
    {
        BURN_MSG("DATA_BURNED\n");
        OTP_LOG_Send(DATA_BURNED, FORMAT_RESERVE);
        return CMD_RET_SUCCESS;
    }
    else if(!(RSA_NKey_Flag || RSA_EKey_Flag || RSA_Lock_Flag || RSA_Block_Flag || SecuriryBoot_Flag || Jtag_Disable_Flag))
    {
        BURN_MSG("DATA_BURNING\n");
        OTP_LOG_Send(DATA_BURNING, FORMAT_RESERVE);
        Program_OTP_RSA_NKey(Keyaddr);
        Program_OTP_RSA_EKey();
        RSA_NKey_Flag      =  IS_BURN_RSA_NKey(KeySum);
        RSA_EKey_Flag      =  IS_BURN_RSA_EKey();

        BURN_MSG("RSA_NKey_Flag: %d\n", RSA_NKey_Flag);
        BURN_MSG("RSA_EKey_Flag: %d\n", RSA_EKey_Flag);

        if(RSA_NKey_Flag && RSA_EKey_Flag)
        {
            Program_OTP_RSA_Lock();
            Program_OTP_RSA_Block();
            Program_OTP_SecuriryBoot_Enable();
            Program_OTP_Jtag_Disable();
            RSA_Lock_Flag      =  IS_BURN_RSA_Lock();
            RSA_Block_Flag     =  IS_BURN_RSA_Block();
            SecuriryBoot_Flag  =  IS_BURN_SecuriryBoot_Enable();
            Jtag_Disable_Flag  =  IS_BURN_Jtag_Disable();

            BURN_MSG("RSA_Lock_Flag: %d\n", RSA_Lock_Flag);
            BURN_MSG("RSA_Block_Flag: %d\n", RSA_Block_Flag);
            BURN_MSG("SecuriryBoot_Flag: %d\n", SecuriryBoot_Flag);
            BURN_MSG("Jtag_Disable_Flag: %d\n", Jtag_Disable_Flag);

            if(RSA_Lock_Flag && RSA_Block_Flag && SecuriryBoot_Flag && Jtag_Disable_Flag)
            {
                BURN_MSG("DATA_BURN_DONE\n");
                OTP_LOG_Send(DATA_BURN_DONE, FORMAT_RESERVE);
                return CMD_RET_SUCCESS;
            }
        }
        else
        {
            OTP_LOG_Send((RSA_NKey_Flag ? 0 : DATA_RSA_N_ERR) | (RSA_EKey_Flag ? 0 : DATA_RSA_E_ERR), FORMAT_RESERVE);
            printf("BURN_OTP EKey or NKey Program Err!\n");
            return CMD_RET_FAILURE;
        }
    }

    OTP_LOG_Send((RSA_NKey_Flag ? 0 : DATA_RSA_N_ERR) | (RSA_EKey_Flag ? 0 : DATA_RSA_E_ERR)
                | (RSA_Lock_Flag ? 0 : DATA_RSA_LOCK_ERR) | (RSA_Block_Flag ? 0 : DATA_RSA_BLOCK_ERR)
                | (SecuriryBoot_Flag ? 0 : DATA_SEBTEN_ERR) | (Jtag_Disable_Flag ? 0 : DATA_JTAGDIS_ERR), FORMAT_RESERVE);

    if(RSA_NKey_Flag && RSA_EKey_Flag)
    {
        if(!RSA_Lock_Flag)
        {
            BURN_MSG("Re-Program_OTP_RSA_Lock\n");
            Program_OTP_RSA_Lock();
        }

        if(!RSA_Block_Flag)
        {
            BURN_MSG("Re-Program_OTP_RSA_Block\n");
            Program_OTP_RSA_Block();
        }

        if(!SecuriryBoot_Flag)
        {
            BURN_MSG("Re-Program_OTP_SecuriryBoot_Enable\n");
            Program_OTP_SecuriryBoot_Enable();
        }

        if(!Jtag_Disable_Flag)
        {
            BURN_MSG("Re-Program_OTP_Jtag_Disable\n");
            Program_OTP_Jtag_Disable();
        }
    }

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	otpburn, CONFIG_SYS_MAXARGS, 0,	BURN_OTP_Start,
	"Used for auto program otp flow.",
	"Usage: otpburn [IPL_CUST loadaddr]\n\r"
	"Note: Please modify according to your needs.\n\r"
);

