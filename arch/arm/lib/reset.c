/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <garyj@denx.de>
 *
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2004 Texas Insturments
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include "asm/arch/mach/platform.h"

__weak void reset_misc(void)
{
}

int do_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	puts ("resetting ...\n");

    /* set SW reset type */
    SETREG16(BASE_REG_PMPOR_PA+REG_ID_01, BIT0); //assign bit to bank por, POR will be clear to 0 only when hw reset or power on
    SETREG16(BASE_REG_WDT_PA+REG_ID_02,BIT0); //clear wdt before sw reset to recognize reset type correctly

	udelay (50000);				/* wait 50 ms */

	disable_interrupts();

	reset_misc();
	reset_cpu(0);

	/*NOTREACHED*/
	return 0;
}
