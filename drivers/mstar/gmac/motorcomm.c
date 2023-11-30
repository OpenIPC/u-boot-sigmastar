/*
* motorcomm.c- Sigmastar
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
#include <net.h>
#include <netdev.h>
#include <phy.h>
#include "mhal_gmac.h"



#define REG_DEBUG_ADDR_OFFSET           0x1e
#define REG_DEBUG_DATA                  0x1f

#define PHY_ID_YT8531_0					0x4f51
#define PHY_ID_YT8531_1					0xe91b
#define MOTORCOMM_PHY_ID_MASK           0x00000fff
#define YT8531_RGMII_CONFIG1			0xa003
#define YT8531_TX_DELAY_MSK				0xf
#define YT8531_TX_DELAY_1ns				0x5


extern int eqos_mdio_write(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg, u16 mdio_val);
extern int eqos_mdio_read(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg);
extern unsigned char phy_id;


static int ytphy_read_ext(struct mii_dev *bus, u32 regnum)
{
	int ret;
	int val;

	ret = eqos_mdio_write(bus, phy_id, 0, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	val = eqos_mdio_read(bus, phy_id, 0, REG_DEBUG_DATA);

	return val;
}

static int ytphy_write_ext(struct mii_dev *bus, u32 regnum, u16 val)
{
	int ret;

	ret = eqos_mdio_write(bus, phy_id, 0, REG_DEBUG_ADDR_OFFSET, regnum);
	if (ret < 0)
		return ret;

	ret = eqos_mdio_write(bus, phy_id, 0, REG_DEBUG_DATA, val);

	return ret;
}


int motorcomm_config_init(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
	int val, ret;
    u32 phy_id0 = 0x00000000;
    u32 phy_id1 = 0x00000000;

    phy_id0 = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_2);
    phy_id1 = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_3);

    if((PHY_ID_YT8531_0 != phy_id0) || (PHY_ID_YT8531_1 != phy_id1))
    {
        return 0;
    }

	if(eqos->phy->speed == SPEED_1000)
	{
		val = ytphy_read_ext(eqos->mii, YT8531_RGMII_CONFIG1);
		if (val < 0)
			return val;

		//set tx clk delay from 2ns to 1ns
		val = ((val & ~(YT8531_TX_DELAY_MSK)) | YT8531_TX_DELAY_1ns);

		ret = ytphy_write_ext(eqos->mii, YT8531_RGMII_CONFIG1, val);
		if (ret < 0)
			return ret;
	}

	return 0;

}
