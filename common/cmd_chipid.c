/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * Boot support
 */
#include <common.h>
#include <command.h>
#include <match_chipid.h>


void do_chipid()
{
    printf("0X%02X\n",GET_CHIPID);
}


/***************************************************/
U_BOOT_CMD(
    chipid,  1,   0,  do_chipid,
    "get chipID",
    ""
);


