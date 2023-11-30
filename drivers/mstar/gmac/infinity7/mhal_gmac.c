/*
* mhal_gmac.c- Sigmastar
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
#include <phy.h>
#include "asm/arch/mach/platform.h"
#include <asm/io.h>
#include "mhal_gmac.h"

#define GET_REG8_ADDR(x, y)                         ((x) + ((y) << 1) - ((y) & 1))

#define GAMC_DRIVING_UPDATEALL 0xff
#define GMAC_MAX_GEAR          16
#define GMAC_DRIVING_REG_MASK  0x780
#define GMAC_DRIVING_REG_SHIFT 7


#define GMAC1_BANK_ADDR_OFFSET   0x0A
#define rebase_based             0x1F000000
#define GET_REG_ADDR8(x, y)      ((x) + ((y) << 1) - ((y)&1))
#define GET_BASE_ADDR_BY_BANK(x, y)	((x) + ((y) << 1))
#define SSTAR_BASE_REG_RIU_PA		(0x1F000000)
#define SSTAR_BASE_REG_CHIPTOP_PA	GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x101E00)
#define BASE_REG_CLKGEN_2_PA     (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x103F00)
#define BASE_REG_INTR_CTRL1_1_PA (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x101700)
#define BASE_REG_GMACPLL_PA      (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x103A00)
#define BASE_REG_PAD_GPIO2_PA    (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x110400)
#define BASE_REG_GMAC0_PA        (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151000)
#define BASE_REG_GMAC1_PA        (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151100)
#define BASE_REG_NET_GP_CTRL_PA  (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151200)
#define BASE_REG_X32_GMAC0_PA    (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x1A5000)
#define BASE_REG_X32_GMAC1_PA    (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x1A5A00)

#define GMAC0    0
#define GMAC1    1
#define GMAC_NUM 2

#define GMAC_DBG 0

enum
{
    GMAC_MDC,
    GMAC_MDIO,
    GMAC_RGMII_TX_CLK,
    GMAC_RGMII_TX_D0,
    GMAC_RGMII_TX_D1,
    GMAC_RGMII_TX_D2,
    GMAC_RGMII_TX_D3,
    GMAC_RGMII_TX_CTL,
    GMAC_MAX_IO
};

char *Gmac_IOName[GMAC_MAX_IO] = {
    "MDC", "MDIO", "RGMII_TX_CLK", "RGMII_TX_D0", "RGMII_TX_D1", "RGMII_TX_D2", "RGMII_TX_D3", "RGMII_TX_CTL",
};

u8 GMAC_Driving_Setting[GMAC_NUM][GMAC_MAX_IO] = {{
                                                      7, // RGMII0_MDC
                                                      7, // RGMII0_MDIO
                                                      7, // RGMII0_TX_CLK
                                                      7, // RGMII0_TX_D0
                                                      7, // RGMII0_TX_D1
                                                      7, // RGMII0_TX_D2
                                                      7, // RGMII0_TX_D3
                                                      7  // RGMII0_TX_CTL
                                                  },
                                                  {
                                                      7, // RGMII1_MDC
                                                      7, // RGMII1_MDIO
                                                      7, // RGMII1_TX_CLK
                                                      7, // RGMII1_TX_D0
                                                      7, // RGMII1_TX_D1
                                                      7, // RGMII1_TX_D2
                                                      7, // RGMII1_TX_D3
                                                      7  // RGMII1_TX_CTL
                                                  }};

u8 GMAC_Driving_Offset[GMAC_NUM][GMAC_MAX_IO] = {{0x68, 0x69, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F},
                                                 {0x78, 0x79, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F}};

/*
 * Usage of proc for GMAC
 * 1.cat /proc/gmac_0/driving
 * 2.echo io_idx gear > /proc/gmac_0/driving
 *
 * ex :
 * input:
 * echo 0 8 > /proc/gmac_0/driving
 * output on console:
 * MDC driving = 8
 *
 * Driving Gear Mapping :
 * 3.3V mode (typical)
 * Gear (DS3, DS2, DS1, DS0) =
 * 0 -> (0, 0, 0, 0) -> 4.4mA
 * 1 -> (0, 0, 0, 1) -> 6.5mA
 * 2 -> (0, 0, 1, 0) -> 8.7mA
 * 3 -> (0, 0, 1, 1) -> 10.9mA
 * 4 -> (0, 1, 0, 0) -> 13.0mA
 * 5 -> (0, 1, 0, 1) -> 15.2mA
 * 6 -> (0, 1, 1, 0) -> 17.4mA
 * 7 -> (0, 1, 1, 1) -> 19.5mA
 * 8 -> (1, 0, 0, 0) -> 21.7mA
 * 9 -> (1, 0, 0, 1) -> 23.9mA
 * 10 -> (1, 0, 1, 0) -> 26.0mA
 * 11 -> (1, 0, 1, 1) -> 28.2mA
 * 12 -> (1, 1, 0, 0) -> 30.3mA
 * 13 -> (1, 1, 0, 1) -> 32.5mA
 * 14 -> (1, 1, 1, 0) -> 34.6mA
 * 15 -> (1, 1, 1, 1) -> 36.8mA
 * */
static char gmac_reset0;
static char gmac_reset1;
static char gmac_an=0;

static void mhal_gmac_gpio_reset(u8 gmacId)
{
	char *s;
	u32 bank, offset;

	s = getenv ("sstar_gmac_rst");

    if(gmacId == GMAC0)
    {
#ifdef GMAC_FPGA
    	OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x0000, 0x000F);
    	mdelay(1);
    	OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x0002, 0x000F);
#else
    	if(s && strcmp(s, "IO1") == 0)
    	{
        	//GPIO 231
        	bank = BASE_REG_PAD_GPIO2_PA;
        	offset = 0x67;
        	OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x42), 0x2000, 0x2000);
    	}
    	else
    	{
        	//GPIO 230
        	bank = BASE_REG_PAD_GPIO2_PA;
        	offset = 0x66;
        	OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x42), 0x1000, 0x1000);
    	}

		if(gmac_reset0 == 0)
        {
	        // Output Low
	        OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
	        mdelay(20);
	        // Output High
	        OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
            gmac_reset0 = 1;
            gmac_reset1 = 0;
        }


#endif
    }
    else if(gmacId == GMAC1)
    {
#ifdef GMAC_FPGA
        OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x0000, 0x000F);
        mdelay(1);
        OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x0002, 0x000F);
#else
    	if(s && strcmp(s, "IO1") == 0)
    	{
        	//GPIO 247
        	bank = BASE_REG_PAD_GPIO2_PA;
        	offset = 0x77;
        	OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x43), 0x2000, 0x2000);
    	}
    	else
    	{
        	//GPIO 246
        	bank = BASE_REG_PAD_GPIO2_PA;
        	offset = 0x76;
        	OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x43), 0x1000, 0x1000);
    	}

        if(gmac_reset1 == 0)
        {
            // Output Low
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
            mdelay(20);
            // Output High
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
            gmac_reset1=1;
            gmac_reset0=0;
        }
#endif
    }
}
static void mhal_gmac_update_driving(u8 gmacId, u8 io_idx)
{
    if (io_idx == GAMC_DRIVING_UPDATEALL)
    { // Update All IO
        int i = 0;

        for (i = 0; i < GMAC_MAX_IO; i++)
        {
            OUTREGMSK16(GET_REG_ADDR(BASE_REG_PAD_GPIO2_PA, GMAC_Driving_Offset[gmacId][i]),
                        (u16)(GMAC_Driving_Setting[gmacId][i] << GMAC_DRIVING_REG_SHIFT), GMAC_DRIVING_REG_MASK);
        }
    }
    else
    { // Update one IO by io_idx
        OUTREGMSK16(GET_REG_ADDR(BASE_REG_PAD_GPIO2_PA, GMAC_Driving_Offset[gmacId][io_idx]),
                    (u16)(GMAC_Driving_Setting[gmacId][io_idx] << GMAC_DRIVING_REG_SHIFT), GMAC_DRIVING_REG_MASK);
    }
}

int sstar_gmac_probe(struct eth_device *dev, int interface, bd_t * bis)
{
    // turn on gmacpll; wait around 200us for PLL to be stable
    // swch 4
    // wriu   0x00103a02 0x00
    // wriu   0x00103a03 0x00
    OUTREG8(GET_REG_ADDR8(BASE_REG_GMACPLL_PA, 0x02), 0);
    OUTREG8(GET_REG_ADDR8(BASE_REG_GMACPLL_PA, 0x03), 0);

    // OUTREG16(0x1f207402, 0x0000);
    //  turn on d2s & vco_en
    // wriu   0x00103a2c 0xbe
    OUTREG8(GET_REG_ADDR8(BASE_REG_GMACPLL_PA, 0x2c), 0xbe);

    // new add for U02
    // wait 1
    // wriu 0x00103a15 0x40
    mdelay(1);
    OUTREG8(GET_REG_ADDR8(BASE_REG_GMACPLL_PA, 0x15), 0x40);

    if (dev->iobase == GMAC1_Base)
    {
        mhal_gmac_update_driving(GMAC1, GAMC_DRIVING_UPDATEALL);
        // padtop 103c -> RGMII 1.8V
        // OUTREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04), (INREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04)) | 0x02));
#if (GMAC_DBG)
        printk("[%s][%d] SS GMAC1 Setting : Interface=%u\r\n", __func__, __LINE__, interface);
#endif
        if (interface == PHY_INTERFACE_MODE_MII)
        {
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x01), 0x0000);
        }
        else if (interface == PHY_INTERFACE_MODE_RMII)
        {
            // synopsys x32 bank (10M : xA003,100M : xE003, 1000M : ?)
            // OUTREG32(GET_REG_ADDR(BASE_REG_X32_GMAC1_PA,0x00),0xA000);
            // MAC via RMII
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x01), 0x8004);
            // padtop 103c
            OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x41), BIT1, BIT1);
            // chiptop 101e
            OUTREGMSK16(GET_REG_ADDR(SSTAR_BASE_REG_CHIPTOP_PA, 0x50), 0x0000, 0xFF00);
        }
        else if (interface == PHY_INTERFACE_MODE_RGMII)
        {
            // synopsys x32 bank (10M : xA003,100M : xE003, 1000M : ?)
            // OUTREG32(GET_REG_ADDR(BASE_REG_X32_GMAC1_PA,0x00),0xA000);
            // MAC via RGMII
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x01), 0x0001);
            // padtop 103c
            OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x41), BIT1, BIT1);
            // chiptop 101e
            OUTREGMSK16(GET_REG_ADDR(SSTAR_BASE_REG_CHIPTOP_PA, 0x50), 0x0000, 0xFF00);
        }
        mhal_gmac_gpio_reset(GMAC1);
#if GMAC_DBG
        printk("[%s][%d] INREG16(0x1f2A2204)=0x%x\r\n", __func__, __LINE__,
               INREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x01)));
#endif
        // 1512 10 bit1 block reg for interrupt entry gmac1
        OUTREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10),
                 INREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10)) & (~BIT(1)));
#if (GMAC_DBG)
        printk("[%s][%d] INREG16(0x1f2a2440)=%x\r\n", __func__, __LINE__,
               INREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10)));
#endif
    }
    else
    {
        mhal_gmac_update_driving(GMAC0, GAMC_DRIVING_UPDATEALL);
        // padtop 103c -> RGMII 1.8V
        // OUTREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04), (INREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04)) | 0x80));
#if (GMAC_DBG)
        printk("[%s][%d] SS GMAC0 Setting : Interface=%u\r\n", __func__, __LINE__, interface);
#endif
        if (interface == PHY_INTERFACE_MODE_MII)
        {
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x01), 0x0000);
        }
        else if (interface == PHY_INTERFACE_MODE_RMII)
        {
            // synopsys x32 bank (10M : xA003,100M : xE003, 1000M : ?)
            // OUTREG32(GET_REG_ADDR(BASE_REG_X32_GMAC0_PA,0x00),0xA000);
            // MAC via RMII
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x01), 0x8004);
            // padtop 103c
            OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x41), BIT0, BIT0);
            // chiptop 101e
            OUTREGMSK16(GET_REG_ADDR(SSTAR_BASE_REG_CHIPTOP_PA, 0x50), 0x0000, 0xFF00);
        }
        else if (interface == PHY_INTERFACE_MODE_RGMII)
        {
            // synopsys x32 bank (10M : xA003,100M : xE003, 1000M : ?)
            // OUTREG32(GET_REG_ADDR(BASE_REG_X32_GMAC0_PA,0x00),0xA000);
            // MAC via RGMII
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x01), 0x0001);
            // padtop 103c
            OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x41), BIT0, BIT0);
            // chiptop 101e All padin
            OUTREGMSK16(GET_REG_ADDR(SSTAR_BASE_REG_CHIPTOP_PA, 0x50), 0x0000, 0xFF00);
        }

        mhal_gmac_gpio_reset(GMAC0);
#if (GMAC_DBG)
        printf("[%s][%d] INREG16(0x1f2A2004)=0x%x\r\n", __func__, __LINE__,
               INREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x01)));
#endif
        // 1512 10 bit0 block reg for interrupt entry gmac0
        OUTREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10),
                 INREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10)) & (~BIT(0)));
#if (GMAC_DBG)
        printf("[%s][%d] INREG16(0x1f2a2440)=%x\r\n", __func__, __LINE__,
               INREG16(GET_REG_ADDR(BASE_REG_NET_GP_CTRL_PA, 0x10)));
#endif
    }
#if (GMAC_DBG)
    // 1017 h6f bit11 clear to 0 for interrupt entry 2 , we got two entry of interrupt
    printf("[%s][%d] INREG16(0x1f202fbc)=%x\r\n", __func__, __LINE__,
           INREG16(GET_REG_ADDR(BASE_REG_INTR_CTRL1_1_PA, 0x6F)));
#endif
    OUTREG16(GET_REG_ADDR(BASE_REG_INTR_CTRL1_1_PA, 0x6F),
             INREG16(GET_REG_ADDR(BASE_REG_INTR_CTRL1_1_PA, 0x6F)) & (~BIT(11)));
#if (GMAC_DBG)
    printf("[%s][%d] INREG16(0x1f202fbc)=%x\r\n", __func__, __LINE__,
           INREG16(GET_REG_ADDR(BASE_REG_INTR_CTRL1_1_PA, 0x6F)));
#endif
    return 0;
}

int sstar_gmac_set_rate(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    int offset = 0x35;

    if (dev->iobase == GMAC1_Base)
    {
        offset = 0x36;
    }
    else
    {
        offset = 0x35;
    }

    switch (eqos->phy->speed)
    {
        case SPEED_1000:
            // Clk 125M for 1000Mbps
            OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, offset), 0x0);
            printf("SPEED_1000\n");
            break;
        case SPEED_100:
            // Clk 25M for 100Mbps
            OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, offset), 0x4);
            printf("SPEED_100\n");
            break;
        case SPEED_10:
            OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, offset), 0x8);
            printf("SPEED_10\n");
            break;
        default:
            printf("invalid speed %d", eqos->phy->speed);
            return -1;
    }
    return 0;
}

void ssta_gmac_update_trxc(void)
{
    // no need
}

int sstar_gmac_set_tx_clk_pad_sel(struct eth_device *dev)
{
    // no need
    return 0;
}



#define GMAC1_BANK_ADDR_OFFSET 0x0A
#define rebase_based 0x1F000000


#if 1
u32 dwmac_to_sigma_rebase(void __iomem *addr)
{
	u32 bank=0, axi_offset=0, offset=0;
	u32 reg=(unsigned long)addr;
	u32 bank_offset;
	unsigned long rebase_addr;

	if(reg>=GMAC1_Base)
	  offset = (reg-GMAC1_Base);
	else
	  offset = (reg-GMAC0_Base);

	axi_offset = offset>>8;

	switch (axi_offset) {
	  case 0x00:
	  case 0x01:
		   axi_offset = 0x00;
		   bank = 0x1A50;
		   break;
	  case 0x02:
	  case 0x03:
		   axi_offset = 0x02;
		   bank = 0x1A51;
		   break;
	  case 0x07:
	  case 0x08:
		   axi_offset = 0x07;
		   bank = 0x1A52;
		   break;
	  case 0x09:
	  case 0x0a:
		   axi_offset = 0x09;
		   bank = 0x1A53;
		   break;
	  case 0x0b:
	  case 0x0c:
		   axi_offset = 0x0b;
		   bank = 0x1A54;
		   break;
	  case 0x0d:
	  case 0x0e:
		   axi_offset = 0x0d;
		   bank = 0x1A55;
		   break;
	  case 0x0f:
	  case 0x10:
		   axi_offset = 0x0f;
		   bank = 0x1A56;
		   break;
	  case 0x11:
	  case 0x12:
		   axi_offset = 0x11;
		   bank = 0x1A57;
		   break;

	};

	bank_offset = offset - (axi_offset<<8);
	if(reg>=GMAC1_Base)
	  bank+=GMAC1_BANK_ADDR_OFFSET;

	rebase_addr = rebase_based + (bank << 9) + bank_offset;

#if 0
	if(bank==0x1A56){
		printf("[RBS] input write addr: 0x%x\r\n",reg);
		printf("[RBS] rebse write addr= 0x%lx \r\n", rebase_addr);
		printf("[RBS] input read addr: 0x%x\r\n",reg);
		printf("[RBS] rebse read addr= 0x%lx \r\n", rebase_addr);
	}
#endif

	return rebase_addr;
}
#endif

#if(0)
void MDrv_GMAC_DumpReg(u32 bank)
{
	int i;
	u32* addr;
	u32 content_x32;

	printf("BANK:0x%04X\n", bank);
    for (i=0; i <= 0x7f; i+=1)
    {
        if(i%0x8==0x0) printf("%02X: ", i);
        addr = (unsigned long)(0x1f000000 + bank*0x200 + i*4);
        content_x32 = *(unsigned int *)addr;
        printf("0x%08X ", content_x32);
        if(i%0x8==0x7) printf("\n");
    }
	printf("\r\n");
}
#endif

extern int eqos_mdio_write(struct mii_dev *bus, int mdio_addr, int mdio_devad, int mdio_reg, u16 mdio_val);
extern int eqos_mdio_read(struct mii_dev *bus, int mdio_addr, int mdio_devad, int mdio_reg);
extern unsigned char phy_id;

//-------------------------------------------------------------------------------------------------
// MAC cable connection detection
//-------------------------------------------------------------------------------------------------
u32 MHal_GMAC_CableConnection(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    u32 value, partner, counter, Gigacap;

    //check status
    counter = 0;
#if (FORCE_100 == 0) && (FORCE_1000 == 0)
    value = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);

    while(!(value & PHY_AN_DONE))
    {
        //wait 4 secs
        counter++;
        if(counter > 20)
        {
            printf("AN fail \r\n");
            return 0;
        }
        mdelay(200);
        value = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);
    }
#endif

    counter = 0;
    value = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);
    while(!(value & PHY_LINK_UP))
    {
        //wait 4 secs
        counter++;
        if(counter > 20)
        {
            printf("Link up fail");
            return -1;
        }

        mdelay(200);
        value = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);
    }

	eqos->phy->link = 1;

	Gigacap = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_GBESR);
	partner = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_GBSR);

	Gigacap = (Gigacap >> 12) & 0x03;
	partner = (partner >> 10) & 0x03;
	partner &= (Gigacap);

	if(partner & 0x01){
        eqos->phy->duplex = 0;
        eqos->phy->speed = SPEED_1000;
	}
	else if(partner & 0x02){
        eqos->phy->duplex = 1;
        eqos->phy->speed = SPEED_1000;
	}
	else{
		partner = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_LINK_PARTNER);

		value = (value >> 11) & 0x0000000F;
		partner = (partner >> 5) & 0x0000000F;
		value &= (partner);

		//100MB Full
		if (value & 0x08)
		{
			eqos->phy->duplex = 1;
			eqos->phy->speed = SPEED_100;
		}
		//100MB Half
		else if (value & 0x04)
		{
			eqos->phy->speed = SPEED_100;
		}
		//10MB Full
		else if (value & 0x02)
		{
			eqos->phy->duplex = 1;
		}
	}
	//printf("Link Status Speed:%d Full-duplex:%d\n", eqos->phy->speed, eqos->phy->duplex);

    return 0;
}

//-------------------------------------------------------------------------------------------------
// EMAC Negotiation PHY
//-------------------------------------------------------------------------------------------------
void MHal_GMAC_NegotiationPHY(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
#if PATCH
	int val;
#endif

    // Set default as 10Mb full-duplex if negotiation fails.
	eqos->phy->duplex = 1;
	eqos->phy->speed = SPEED_10;


#if (FORCE_100)
	printf("Force 100 \r\n");
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_ANAR, 0xd01);
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_GBCR, 0x0);

	eqos->phy->link = 1;
	eqos->phy->duplex = 1;
	eqos->phy->speed = SPEED_100;
	return;
#elif (FORCE_1000)
	printf("Force 1000 \r\n");
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_ANAR, 0xc01);
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_GBCR, 0x200);

	eqos->phy->link = 1;
	eqos->phy->duplex = 1;
	eqos->phy->speed = SPEED_1000;
	return;
#else
    printf("Auto-Negotiation...\n");
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_ANAR, 0xde1);
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_GBCR, 0x200);

#endif
	if(gmac_an == 0)
	{
		eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, 0x1240);
		gmac_an = 1;
	}
	else
	{
		eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, 0x1040);
	}

#if PATCH
    val = eqos_mdio_read(eqos->mii, phy_id, 0, 4);
    if(val<0)
	    return;
    eqos_mdio_write(eqos->mii, phy_id, 0, 4, 0x61);
#endif

    MHal_GMAC_CableConnection(dev);
}

void MHal_GMAC_Write_MACAddr(struct eth_device *dev, char enetaddr[])
{
	uint32_t val;

    /* Update the MAC address */
    val = (enetaddr[5] << 8) |
        (enetaddr[4]);
    writel(val, (void *) dev->iobase + 0x300);//high
    val = (enetaddr[3] << 24) |
        (enetaddr[2] << 16) |
        (enetaddr[1] << 8) |
        (enetaddr[0]);
    writel(val, (void *) dev->iobase + 0x304);//low

	return;
}

