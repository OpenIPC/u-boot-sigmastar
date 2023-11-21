/*
 * uart_padmux.c- Sigmastar
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


/*-----------------------------------------------------------------------------
    Include Files
------------------------------------------------------------------------------*/
#include <common.h>
#include <command.h>
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_serial.h"
#include <linux/compiler.h>
#include <serial.h>

/* Constants of CEDRIC RIU */
#define IO_PHYS         0x1F000000 //sync with platform.h
#define IO_OFFSET       0x00000000
#define IO_SIZE         0x00400000
#define IO_VIRT         (IO_PHYS + IO_OFFSET)
#define io_p2v(pa)      ((pa) + IO_OFFSET)
#define IO_ADDRESS(x)   io_p2v(x)

/* read register by word */
#define ms_readw(a) (*(volatile unsigned short *)IO_ADDRESS(a))

/* write register by word */
#define ms_writew(v,a) (*(volatile unsigned short *)IO_ADDRESS(a) = (v))

#define INREG16(x)              ms_readw(x)
#define OUTREG16(x, y)          ms_writew((u16)(y), x)
#define SETREG16(x, y)          OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)          OUTREG16(x, INREG16(x)&~(y))
#define INREGMSK16(x, y)        (INREG16(x) & (y))
#define OUTREGMSK16(x, y, z)    OUTREG16(x, ((INREG16(x)&~(z))|((y)&(z))))

#define MS_BASE_REG_UART1_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110900)
#define MS_BASE_REG_UART2_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110A00)
#define MS_BASE_REG_UART3_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110B00)
#define MS_BASE_REG_FUART_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110200)

#if 1
#define REG_UART1_CLK                0x1F2070C4 /*0x1038, BIT[7:4]*/
#define REG_UART2_CLK                0x1F2070C4 /*0x1038, BIT[11:8]*/
#define REG_UART3_CLK                0x1F2070C4 /*0x1038, BIT[15:12]*/
#define REG_FUART_CLK                0x1F2070D0 /*0x1038, BIT[3:0]*/

#define REG_FUART_MODE         0x1F204C14 /*0x1026, h05*/
#define REG_UART_MODE          0x1F204C04 /*0x1026, h01*/

#define REG_FUART_SEL          0x1F203D50 /*0x101E, h54*/
#define REG_UART_SEL           0x1F203D4C /*0x101E, h53*/

#define UART_PIU_UART1   3
#define UART_PIU_UART2   4
#define UART_PIU_UART3   5
#define UART_PIU_FUART   1

//mercury6
/* reg_uart_sel2 <--> reg_ThirdUARTMode*/
#define PAD_HSYNC_OUT (0x4)
#define PAD_SNR3_D4 (0x3)
#define PAD_SPI0_CK (0x2)
#define PAD_UART1_TX (0x1)

/* reg_uart_sel3 <--> reg_ForthUARTMode */
#define PAD_UART2_TX (0x1)
#define PAD_TTL_HSYNC (0x2)
#define PAD_FUART_RTS (0x3)
#define PAD_SPI0_DO (0x4)

/* reg_uart_sel4 <--> reg_FastUART_RTX_Mode  */
#define PAD_FUART_TX (0x1)
#define PAD_NAND_DA4 (0x2)
#define PAD_SNR3_DO (0x3)

#define REG_UART1_PADMUX                0x1F2079B4 /*0x103C, BIT[6:4]*/
#define REG_UART2_PADMUX                0x1F2079B4 /*0x103C, BIT[10:8]*/
#define REG_UART3_PADMUX                0x1F2079B4 /*0x103C, BIT[14:12]*/
#define REG_FUART_PADMUX                0x1F2079B8 /*0x103C, BIT[11:8]*/

#define REG_PM_PAD_EXT_MODE_AA                0x1F007F54 /*0x3F, BIT[7:0]*/
#define REG_PM_PAD_EXT_MODE_AB                0x1F007F54 /*0x3F, BIT[15:8]*/
#define REG_PM_PAD_EXT_MODE_AC                0x1F007F58 /*0x3F, BIT[4:0]*/


#define CONFIG_UART1_PAD_MODE 6

#define CONFIG_UART2_PAD_MODE 0

#define CONFIG_UART3_PAD_MODE 0

#define CONFIG_FUART_PAD_MODE 3

#endif
extern U32 uart_multi_base;

U32 ms_uart_padmux(U8 u8_Port)
{
    U32 uartClk = 0;

    switch(u8_Port)
    {
        case 1:
            uart_multi_base = MS_BASE_REG_UART1_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_UART1_CLK, 0x00 << 4, 0xF << 4);    //reg_uart_sel1
              /*padmux*/
            OUTREGMSK16(REG_UART1_PADMUX, CONFIG_UART1_PAD_MODE << 4, 0x7 << 4);
            OUTREGMSK16(REG_UART_SEL, UART_PIU_UART1 << 12, 0xF << 12);
            if(CONFIG_UART1_PAD_MODE == 1)
            {
                OUTREGMSK16(REG_PM_PAD_EXT_MODE_AA, 0x3 << 4, 0x3 << 4);
            }
            else if(CONFIG_UART1_PAD_MODE == 7)
            {
                OUTREGMSK16(REG_PM_PAD_EXT_MODE_AA, 0x3 << 0, 0x3 << 0);
            }
            break;
        case 2:
            uart_multi_base = MS_BASE_REG_UART2_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_UART2_CLK, 0x00 << 8, 0xF << 8);    //reg_uart_sel1
              /*padmux*/
            OUTREGMSK16(REG_UART2_PADMUX, CONFIG_UART2_PAD_MODE << 8, 0x7 << 8);
            OUTREGMSK16(REG_FUART_SEL, UART_PIU_UART2 << 0, 0xF << 0);
            if(CONFIG_UART2_PAD_MODE == 1)
            {
                OUTREGMSK16(REG_PM_PAD_EXT_MODE_AA, 0x3 << 6, 0x3 << 6);
            }
            else if(CONFIG_UART2_PAD_MODE == 7)
            {
                OUTREGMSK16(REG_PM_PAD_EXT_MODE_AA, 0x3 << 2, 0x3 << 2);
            }
            break;
        case 3:
            uart_multi_base = MS_BASE_REG_UART3_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_UART3_CLK, 0x00 << 12, 0xF << 12);    //reg_uart_sel1
              /*padmux*/
            OUTREGMSK16(REG_UART3_PADMUX, CONFIG_UART3_PAD_MODE << 12, 0x7 << 12);
            OUTREGMSK16(REG_FUART_SEL, UART_PIU_UART3 << 4, 0xF << 4);
            break;
        case 4:
            uart_multi_base = MS_BASE_REG_FUART_PA;
            /*clk*/
            uartClk = 172800000;
            OUTREGMSK16(REG_FUART_CLK, 0x00 << 4, 0xF << 4);    //reg_uart_sel1
              /*padmux*/
            OUTREGMSK16(REG_FUART_PADMUX, CONFIG_FUART_PAD_MODE << 8, 0xF << 8);
            OUTREGMSK16(REG_UART_SEL, UART_PIU_FUART << 4, 0xF << 4);
            if(CONFIG_FUART_PAD_MODE == 1)
            {
                OUTREGMSK16(REG_PM_PAD_EXT_MODE_AA, 0xF << 4, 0xF << 4);
            }
            break;
        default:
            uart_multi_base = MS_BASE_REG_UART0_PA;
            /*clk*/
            uartClk = 172800000;
            /*padmux*/
            break;
    }
    return uartClk;
}
