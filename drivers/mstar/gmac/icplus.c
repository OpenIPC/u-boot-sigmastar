/*
* icplus.c- Sigmastar
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
#include <common.h>

/* IP101A/G - IP1001 */
#define IP10XX_SPEC_CTRL_STATUS		16	/* Spec. Control Register */
#define IP1001_RXPHASE_SEL		(1<<0)	/* Add delay on RX_CLK */
#define IP1001_TXPHASE_SEL		(1<<1)	/* Add delay on TX_CLK */
#define IP1001_SPEC_CTRL_STATUS_2	20	/* IP1001 Spec. Control Reg 2 */
#define IP1001_APS_ON			11	/* IP1001 APS Mode  bit */
#define IP101A_G_APS_ON			2	/* IP101A/G APS Mode bit */
#define IP101A_G_IRQ_CONF_STATUS	0x11	/* Conf Info IRQ & Status Reg */
#define	IP101A_G_IRQ_PIN_USED		(1<<15) /* INTR pin used */
#define	IP101A_G_IRQ_DEFAULT		IP101A_G_IRQ_PIN_USED

#define MII_BMCR		0x00	/* Basic mode control register */
#define BMCR_RESET		0x8000	/* Reset to default state      */

#define BMCR_ANENABLE		0x1000	/* Enable auto negotiation     */
#define BMCR_ANRESTART		0x0200	/* Auto negotiation restart    */
#define BMCR_ISOLATE		0x0400	/* Isolate data paths from MII */

//extern unsigned char phy_id;
//extern void MHal_EMAC_read_phy(unsigned char phy_addr, unsigned char address,u32 *value);
//extern void MHal_EMAC_write_phy (unsigned char phy_addr, unsigned char address, u32 value);

extern int eqos_mdio_write(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg, u16 mdio_val);
extern int eqos_mdio_read(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg);

extern int genphy_update_link(struct phy_device *phydev);
extern int genphy_parse_link(struct phy_device *phydev);

extern void MDrv_GMAC_DumpPhy(struct mii_dev *bus);
#if 0
static int phy_read(unsigned char addr)
{
    u32 val = 0;
    MHal_EMAC_read_phy(phy_id, addr, &val);
    return (int)val;
}

static int phy_write(unsigned char addr, int val)
{
    u32 ret = 0;
    MHal_EMAC_write_phy(phy_id, addr, (u32)val);
    MHal_EMAC_read_phy(phy_id, addr, &ret);
    return (int)ret;
}
#endif
static int ip1xx_reset(struct mii_dev *bus)
{
	int bmcr;

	/* Software Reset PHY */
	bmcr = eqos_mdio_read(bus, 1, 0, MII_BMCR);
	if (bmcr < 0)
		return bmcr;

	//bmcr |= BMCR_RESET;
	bmcr = eqos_mdio_write(bus, 1, 0, MII_BMCR, bmcr);
	if (bmcr < 0)
		return bmcr;

	do {
		bmcr = eqos_mdio_read(bus, 1, 0, MII_BMCR);
		if (bmcr < 0)
			return bmcr;
	} while (bmcr & BMCR_RESET);

	return 0;
}

int ip101a_startup(struct phy_device *phydev)
{
	genphy_update_link(phydev);
	genphy_parse_link(phydev);

	return 0;
}

int ip101a_g_config_init(struct mii_dev *bus)
{
	int c;

	c = ip1xx_reset(bus);
	if (c < 0)
		return c;

	/* INTR pin used: speed/link/duplex will cause an interrupt */
	c = eqos_mdio_write(bus, 1, 0, IP101A_G_IRQ_CONF_STATUS, IP101A_G_IRQ_DEFAULT);
	if (c < 0)
		return c;

	/* Enable Auto Power Saving mode */
	c = eqos_mdio_read(bus, 1, 0, IP10XX_SPEC_CTRL_STATUS);
	c |= IP101A_G_APS_ON;

	return eqos_mdio_write(bus, 1, 0, IP10XX_SPEC_CTRL_STATUS, c);
}
