/*
* ms_wdt.c- Sigmastar
*
* Copyright (C) 2018 Sigmastar Technology Corp.
*
* Author: sylvia.nain <sylvia.nain@sigmastar.com.tw>
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


/*
 * The Watchdog Timer Mode Register can be only written to once. If the
 * timeout need to be set from U-Boot, be sure that the bootstrap doesn't
 * write to this register. Inform Linux to it too
 */

#include <common.h>
#include <watchdog.h>
#include "environment.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_version.h"


/* Hardware timeout in seconds */
#define WDT_HW_TIMEOUT 60

void hw_watchdog_disable(void)
{
    U32 u32Timeout = (U32)CONFIG_WDT_CLOCK * WDT_HW_TIMEOUT;
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_H), ((u32Timeout>>16) & 0x0));
    OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_L), (u32Timeout & 0x0));
}


void hw_watchdog_reset(void)
{
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_WDT_CLR), 0x1); //clear
}

void hw_watchdog_init(void)
{
    U32 u32Timeout = (U32)CONFIG_WDT_CLOCK * WDT_HW_TIMEOUT;
	printf("[WDT] Enalbe WATCHDOG 60s\n");
	OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_H), ((u32Timeout>>16) & 0xFFFF));
    OUTREG16(GET_REG_ADDR(REG_ADDR_BASE_WDT, WDT_MAX_PRD_L), (u32Timeout & 0xFFFF));
	hw_watchdog_reset();
}


