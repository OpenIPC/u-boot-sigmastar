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
#define UART1_CLK                      0x31
#define FUART_CLK                      0x34
#define PM_UART_CLK                    0x25

#define MS_BASE_REG_UART1_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110900)
#define MS_BASE_REG_FUART_PA           GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x110200)
#define MS_BASE_REG_PM_UART_PA         GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003500)
#define MS_BASE_REG_PADMUX             GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x103C00)
#define MS_BASE_REG_PM_PADMUX          GET_BASE_ADDR_BY_BANK(MS_BASE_REG_RIU_PA, 0x003F00)


#define UART1_CLK_REG                      GET_REG_ADDR(MS_BASE_REG_CLK_PA, UART1_CLK)
#define FUART_CLK_REG                      GET_REG_ADDR(MS_BASE_REG_CLK_PA, FUART_CLK)
#define PM_UART_CLK_REG                    GET_REG_ADDR(MS_BASE_REG_PM_SLEEP_PA, PM_UART_CLK)

#define UART1_PADMUX_REG                   GET_REG_ADDR(MS_BASE_REG_PADMUX, 0x6D)
#define FUART_PADMUX_REG                   GET_REG_ADDR(MS_BASE_REG_PADMUX, 0x6E)
#define PM_UART_PADMUX_REG                 GET_REG_ADDR(MS_BASE_REG_PM_PADMUX, 0x53)

#define CONFIG_UART1_PAD_MODE                      (1)
#define CONFIG_FUART_PAD_MODE                      (1)
#define CONFIG_PM_UART_PAD_MODE                    (1)

#define UART1_PAD_MASK                             (0xF)
#define FUART_PAD_MASK                             (0xF)
#define PM_UART_PAD_MASK                           (0x3)

#if 0
#define REG_UART2_CLK                0x1F201650 /*0x100B, h14*/
#define REG_FUART_CLK                0x1F20165c /*0x100B, h17*/
#define REG_FUART_MODE         0x1F204C14 /*0x1026, h05*/
#define REG_UART_MODE          0x1F204C04 /*0x1026, h01*/
#define REG_FUART_SEL          0x1F203D50 /*0x101E, h54*/
#define REG_UART_SEL           0x1F203D4C /*0x101E, h53*/

#define UART_PIU_UART1   1
#define UART_VD_MHEG5    2
#define UART_PIU_UART2   3
#define UART_PIU_UART0   4
#define UART_PIU_FUART   7

//infinity2
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

#define CONFIG_UART1_PAD PAD_HSYNC_OUT
/* #define CONFIG_UART1_PAD PAD_SNR3_D4 */
/* #define CONFIG_UART1_PAD PAD_SPI0_CK */
/* #define CONFIG_UART1_PAD PAD_UART1_TX */
#define CONFIG_UART2_PAD PAD_FUART_RTS
/* #define CONFIG_UART2_PAD PAD_UART2_TX */
/* #define CONFIG_UART2_PAD PAD_TTL_HSYNC  */
/* #define CONFIG_UART2_PAD PAD_SPI0_DO */
#define CONFIG_UART3_PAD PAD_FUART_TX
/* #define CONFIG_UART3_PAD PAD_NAND_DA4 */
/* #define CONFIG_UART3_PAD PAD_SNR3_DO  */
#endif
extern U32 uart_multi_base;

U32 ms_uart_padmux(U8 u8_Port)
{
    u64 uartClk = 0;
    switch(u8_Port)
    {
        case 1:
            uart_multi_base = MS_BASE_REG_UART1_PA;
            OUTREGMSK16(UART1_CLK_REG, 0x0000, 0x0F00);
            OUTREGMSK16(UART1_PADMUX_REG, (CONFIG_UART1_PAD_MODE << 4), (UART1_PAD_MASK << 4));
            uartClk = 17280000000;
            break;
        case 2:
            uart_multi_base = MS_BASE_REG_FUART_PA;
            OUTREGMSK16(FUART_CLK_REG, 0x0000, 0x000F);
            OUTREGMSK16(FUART_PADMUX_REG, (CONFIG_FUART_PAD_MODE << 4), (FUART_PAD_MASK << 4));
            uartClk = 17280000000;
            break;
        case 3:
            uart_multi_base = MS_BASE_REG_PM_UART_PA;
            OUTREGMSK16(PM_UART_CLK_REG, 0x0008, 0x000F);
            OUTREGMSK16(PM_UART_PADMUX_REG, (CONFIG_PM_UART_PAD_MODE << 0), (PM_UART_PAD_MASK << 0));
            uartClk = 108000000;
            break;
        default:
            uart_multi_base = MS_BASE_REG_UART1_PA;
            OUTREGMSK16(UART1_CLK_REG, 0x0000, 0x0F00);
            OUTREGMSK16(UART1_PADMUX_REG, (CONFIG_UART1_PAD_MODE << 4), (UART1_PAD_MASK << 4));
            uartClk = 17280000000;
    }
    return uartClk;
}
