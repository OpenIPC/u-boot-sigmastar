/*
* mdrv_gmac.c- Sigmastar
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
#include <clk.h>
#include <dm.h>
#include <errno.h>
//#include <memalign.h>
#include <asm/system.h>
#include <miiphy.h>
#include <net.h>
#include <netdev.h>
#include <phy.h>
#include <reset.h>
#include <wait_bit.h>
#include <asm/io.h>
#include <malloc.h>
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "mhal_gmac.h"

#define EQOS_MAC_REGS_BASE 0x000
#define EQOS_MTL_REGS_BASE 0xd00
#define EQOS_DMA_REGS_BASE 0x1000

char GMACADDR[6];
struct phy_device Gphy;
struct eqos_priv hw_device;

extern void Chip_Flush_Memory(void);
extern int eqos_recv(struct eth_device *dev, int flags, uchar **packetp);
extern int eqos_probe(struct eth_device *dev, bd_t * bis);
extern void eqos_stop(struct eth_device *dev);
extern int eqos_send(struct eth_device *dev, void *packet, int length);

extern unsigned char phy_id;
extern u16 G_txc_phase;
extern int G_gmac_calb;

extern int eqos_mdio_write(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg, u16 mdio_val);
extern int eqos_mdio_read(struct mii_dev *bus, int mdio_addr, int mdio_devad,int mdio_reg);
extern int eqos_send(struct eth_device *dev, void *packet, int length);
extern int eqos_recv(struct eth_device *dev, int flags, uchar **packetp);

#if PHASE_CALB
int loopback_test(struct eth_device *dev, u16 phase)
{
	char gmac_calb_pass = 0;
	u32 ret=0;
	unsigned char packet_fill[]={
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xc0, 0xa8, 0x01, 0x02,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x01, 0x0f};

	if (dev->iobase == GMAC1_Base)
	{
		//OUTREG16(0x1f2a22c4, (0x140 | phase));

		//rx use negedge, without overwrite bit 2,3
		OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x140, ~GMAC_CALB_MSK);
		//only update bit 11.12
		OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), phase , GMAC_TX_PHASE_MSK);
		debug("TX Phase : 0x%08x \r\n",INREG16(0x1f2a22c4));

	}
	else
	{
		//OUTREG16(0x1f2a20c4, (0x140 | phase));

		//rx use negedge, without overwrite bit 2,3
		OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x140, ~GMAC_CALB_MSK);
		//only update bit 11.12
		OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), phase , GMAC_TX_PHASE_MSK);
		debug("TX Phase : 0x%08x \r\n",INREG16(0x1f2a20c4));
	}

	eqos_send(dev, &packet_fill, sizeof(packet_fill));

	for(int i=0;i<6;i++){
		ret = eqos_recv(dev, 0, 0);
		if(ret != -EAGAIN){
			gmac_calb_pass = 1;
			break;
		}
		mdelay(1);
	}

	if(gmac_calb_pass == 1){
		printf("Phase %x loopback pass ! \r\n",phase);
		return 1;
	}
	else{
		debug("Phase %x loopback fail ! \r\n",phase);
		return 0;
	}

}
#endif

int GMAC_initialize(bd_t *bis)
{
	char *s,*e;
	int i;
	struct eth_device *dev;
	struct eqos_priv *priv=NULL;
#if PHASE_CALB
	u32 val = 0, ori_phy = 0;
	struct eqos_priv *eqos = NULL;
	u32 gmac_base[2] = {GMAC0_Base, GMAC1_Base};
	u16 ret = 0;
	u32 ori_gmac_base = 0;
#endif

	debug("[%s][%d] \r\n",__func__,__LINE__);

	dev = (struct eth_device *)malloc(sizeof *dev);
	if (!dev) {
		printf("Can not allocate memory of rtl8139\n");
		return -1;
	}

	memset(dev, 0, sizeof(*dev));
	Chip_Flush_Memory();

	Gphy.interface = PHY_INTERFACE_MODE_RGMII;
	if ((s = getenv ("gmac_bustype")))
	{
		if(strcmp(s,"rgmii") == 0)
		{
			Gphy.interface = PHY_INTERFACE_MODE_RGMII;
		}
		else if(strcmp(s,"rmii") == 0)
		{
			Gphy.interface = PHY_INTERFACE_MODE_RMII;
		}
		else if(strcmp(s,"mii") == 0)
		{
			Gphy.interface = PHY_INTERFACE_MODE_MII;
		}
	}

	s = getenv ("ethact");

	if(strcmp(s,"sstar_gmac1") == 0)
	{
		sprintf(dev->name, "sstar_gmac1");
		dev->iobase = GMAC1_Base;

		s = getenv ("eth1addr");
	}
	else
	{
		sprintf(dev->name, "sstar_gmac0");
		dev->iobase = GMAC0_Base;
		s = getenv ("ethaddr");
	}

	dev->init = eqos_probe;
	dev->halt = eqos_stop;
	dev->send = eqos_send;
	dev->recv = (void *)eqos_recv;

    if (s)
    {
        for (i = 0; i < sizeof(GMACADDR); ++i)
        {
        	GMACADDR[i] = s ? simple_strtoul (s, &e, 16) : 0;
            if (s)
            {
                s = (*e) ? e + 1 : e;
            }
        }
    }

    memcpy(dev->enetaddr, GMACADDR, sizeof(GMACADDR));
    MHal_GMAC_Write_MACAddr(dev, GMACADDR);

	priv=&hw_device;
    priv->mac_regs = (void *)(dev->iobase + EQOS_MAC_REGS_BASE);
    priv->mtl_regs = (void *)(dev->iobase + EQOS_MTL_REGS_BASE);
    priv->dma_regs = (void *)(dev->iobase + EQOS_DMA_REGS_BASE);
	dev->priv = (void *)priv;
	priv->phy=&Gphy;


//	printf("[%s] mac reg: %x \r\n",__func__, (dev->iobase + EQOS_MAC_REGS_BASE));
//	printf("[%s] dma reg: %x, %p, %p \r\n",__func__,(u32)&priv->dma_regs,(void**)priv->dma_regs,(void *)(dev->iobase + EQOS_DMA_REGS_BASE));

	debug ("sstar GMAC @0x%x\n", dev->iobase);

	eth_register(dev);

#if PHASE_CALB
	ori_gmac_base = dev->iobase;

	for(int gmac_idx = 0; gmac_idx < 2; gmac_idx++){
		dev->iobase = gmac_base[gmac_idx];
		priv->mac_regs = (void *)(dev->iobase + EQOS_MAC_REGS_BASE);
		priv->mtl_regs = (void *)(dev->iobase + EQOS_MTL_REGS_BASE);
		priv->dma_regs = (void *)(dev->iobase + EQOS_DMA_REGS_BASE);
		dev->priv = (void *)priv;

		eqos = (struct eqos_priv*) dev->priv;
		eqos->phy->link = 1;
		eqos->phy->duplex = 1;
		eqos->phy->speed = SPEED_100;

		ret = eqos_probe(dev, bis);
		if(ret != 0){
			printf("[gmac%d] calb break\r\n",gmac_idx);
			continue;
		}
		else
			debug("[gmac%d] calb ongoing\r\n",gmac_idx);

		val = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_BASIC);
		ori_phy = val;
		debug("original phy: 0x%08x \r\n",ori_phy);

		//force speed at 100M and enable loopback mode
		eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, 0x6100);
		val = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_BASIC);
		debug("100M loopback phy0: 0x%08x \r\n",val);

		while(1){
			if (loopback_test(dev, txc_0_phase)){
				G_txc_phase = txc_0_phase;
				break;
			}
			if(loopback_test(dev, txc_180_phase)){
				G_txc_phase = txc_180_phase;
				break;
			}
			if(loopback_test(dev, txc_90_phase)){
				G_txc_phase = txc_90_phase;
				break;
			}
			if(loopback_test(dev, txc_270_phase)){
				G_txc_phase = txc_270_phase;
				break;
			}
			printf("[gmac%d] calb failed\r\n",gmac_idx);
			break;
		}

		eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, ori_phy);
		val = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_BASIC);
		debug("recovery phy: 0x%08x \r\n",val);

		if (dev->iobase == GMAC1_Base)
			OUTREGMSK16(0x1f2a22c4, G_txc_phase>>9, GMAC_CALB_MSK);
		else
			OUTREGMSK16(0x1f2a20c4, G_txc_phase>>9, GMAC_CALB_MSK);

	}

	dev->iobase = ori_gmac_base;
	priv->mac_regs = (void *)(dev->iobase + EQOS_MAC_REGS_BASE);
	priv->mtl_regs = (void *)(dev->iobase + EQOS_MTL_REGS_BASE);
	priv->dma_regs = (void *)(dev->iobase + EQOS_DMA_REGS_BASE);
	dev->priv = (void *)priv;

	G_gmac_calb = 1;

#endif

	return 0;
}



int board_eth_init(bd_t * bis)
{
//	printf("[%s][%d] \r\n",__func__,__LINE__);
    if (getenv_yesno("autoestart") != 0 || getenv_yesno("autogstart") != 0) {

		//printf("[%s][%d] \r\n",__func__,__LINE__);
        return GMAC_initialize(bis);
	}
    return 0;
}
