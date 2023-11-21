/*
* cmd_gmac.c- Sigmastar
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

//-------------------------------------------------------------------------------------------------#include "e1000.h"

/*
 * EMAC Read/Write Utilities
 */
#include <common.h>
#include <command.h>
#include <malloc.h>
#include <net.h>

/*-----------------------------------------------------------------------
 * Definitions
 */
#define GMAC_INPUT_LEN                  2
#define GMAC_ADDR_INPUT_LEN                  7

#define MAX_GMAC_BYTES 0x1000	/* Maximum number of bytes we can handle */

extern int GMAC_initialize(bd_t *bis);
extern u8 MY_MAC[6];
extern int gmac_init;
extern struct mii_dev Gmii;
extern unsigned char phy_id;

/*
 * emac init
 *
 * Syntax:
 *   estart
 */
#define MACADDR_ENV    "ethaddr"
#define MACADDR_FORMAT "XX:XX:XX:XX:XX:XX"

int do_gmac (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    bd_t * bis=NULL;
    #if 0
    char * macaddr;

    if (   NULL != (macaddr = getenv(MACADDR_ENV))
        && strlen(macaddr) == strlen(MACADDR_FORMAT)
        && ':' == macaddr[2]
        && ':' == macaddr[5]
        && ':' == macaddr[8]
        && ':' == macaddr[11]
        && ':' == macaddr[14]
        )
    {
        macaddr[2]  = '\0';
        macaddr[5]  = '\0';
        macaddr[8]  = '\0';
        macaddr[11] = '\0';
        macaddr[14] = '\0';

        MY_MAC[0]   = (u8)simple_strtoul(&(macaddr[0]),  NULL, 16);
        MY_MAC[1]   = (u8)simple_strtoul(&(macaddr[3]),  NULL, 16);
        MY_MAC[2]   = (u8)simple_strtoul(&(macaddr[6]),  NULL, 16);
        MY_MAC[3]   = (u8)simple_strtoul(&(macaddr[9]),  NULL, 16);
        MY_MAC[4]   = (u8)simple_strtoul(&(macaddr[12]), NULL, 16);
        MY_MAC[5]   = (u8)simple_strtoul(&(macaddr[15]), NULL, 16);

        /* set back to ':' or the environment variable would be destoried */ // REVIEW: this coding style is dangerous
        macaddr[2]  = ':';
        macaddr[5]  = ':';
        macaddr[8]  = ':';
        macaddr[11] = ':';
        macaddr[14] = ':';
    }
    #endif
    
    GMAC_initialize(bis);
	printf("(Re)start GMAC...\n");

	return 0;
}

int do_readPhy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char phyId;
    unsigned char phyAddress;
    unsigned int phyValue;

    if (!gmac_init)
    {
        printf("Please start gmac first!\n");
        return 0;
    }
    if (argc < 2) {
        return CMD_RET_USAGE;
    }

    phyAddress = simple_strtoul(argv[1], NULL, 16);
	phyValue = eqos_mdio_read(&Gmii, phy_id, 0, phyAddress);

    printf("phy read address[%x] value is %x\n", phyAddress, phyValue);
    return 0;
}

int do_writePhy(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned char phyId;
    unsigned char phyAddress;
    unsigned int phyValue;

	printf("gmac_init= %d , argc=%d \r\n",gmac_init,argc);
    if (!gmac_init)
    {
        printf("Please start gmac first!\n");
        return 0;
    }
    if (argc < 3) {
        return CMD_RET_USAGE;
    }

    phyAddress = simple_strtoul(argv[1], NULL, 16);
    phyValue   = simple_strtoul(argv[2], NULL, 16);

    printf("phy write address[%x] value is %x\n", phyAddress, phyValue);
	eqos_mdio_write(&Gmii, phy_id, 0, phyAddress, phyValue);
    return 0;
}

int do_gmac_update (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	ssta_gmac_update_trxc();
	return 0;
}

/***************************************************/

U_BOOT_CMD(
	gstart, GMAC_INPUT_LEN,	1,	do_gmac,
	"GMAC start",
	"reset  - reset GMAC controller\n"
	"start  - start GMAC controller\n"
);

U_BOOT_CMD(
    phy_r, GMAC_ADDR_INPUT_LEN, 1,	do_readPhy,
    "phy read",
    "phyAddress"
);

U_BOOT_CMD(
    phy_w, GMAC_ADDR_INPUT_LEN, 1,	do_writePhy,
    "phy write",
    "phyAddress phyValue"
);

U_BOOT_CMD(
    gmacup, 2, 1,	do_gmac_update,
    "update gmac settings from env",
    "update gmac settings from env"
);

#if(0)
U_BOOT_CMD(
	macaddr, GMAC_ADDR_INPUT_LEN,	1,	do_setMac,
	"setup EMAC MAC addr",
    "XX XX XX XX XX XX"
);
#endif


