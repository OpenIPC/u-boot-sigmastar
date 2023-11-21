/*
 * (C) Copyright 2020
 * Gerald Van Baren, Custom IDEAS, vanbaren@cideas.com
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <errno.h>
#include <sar.h>

int do_sar(cmd_tbl_t * cmdtp, int flag, int argc, char * const argv [ ])
{
    int channel = 0;
    int data = 0;
    if(argc == 2)
    {
        channel = simple_strtoul(argv[1], NULL, 16);
    }
    else
    {
        printf("Invalid input\n");
    }
    if(channel < 0 || channel > 5)
    {
        printf("channel is between 0 and 5\n");
        return -1;
    }
    ms_sar_hw_init();
    data = ms_sar_get(channel);
    printf("sar %d = %d\n",channel,data);
    return 0;
}

U_BOOT_CMD(
	sar,	2,	1,	do_sar,
	"SAR utility command",
	"[sar] channel -- read the channel data\n"
);

