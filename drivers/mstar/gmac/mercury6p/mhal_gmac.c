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
#include "errno.h"
#include "mhal_gmac.h"

#define GET_REG8_ADDR(x, y)                         ((x) + ((y) << 1) - ((y) & 1))

#define GAMC_DRIVING_UPDATEALL 0xff
#define GMAC_MAX_GEAR          4
#define GMAC_DRIVING_REG_MASK  0x180
#define GMAC_DRIVING_REG_SHIFT 7


#define GMAC1_BANK_ADDR_OFFSET   0x0A
#define rebase_based             0x1F000000
#define GET_REG_ADDR8(x, y)      ((x) + ((y) << 1) - ((y)&1))
#define GET_BASE_ADDR_BY_BANK(x, y)	((x) + ((y) << 1))

#define GMAC0    0
#define GMAC1    1
#define GMAC_NUM 2

#define GMAC_DBG 0
#define GMAC0_OUTPUT_MCLK 1

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
                                                      1, // RGMII0_MDC
                                                      1, // RGMII0_MDIO
                                                      1, // RGMII0_TX_CLK
                                                      1, // RGMII0_TX_D0
                                                      1, // RGMII0_TX_D1
                                                      1, // RGMII0_TX_D2
                                                      1, // RGMII0_TX_D3
                                                      1  // RGMII0_TX_CTL
                                                  },
                                                  {
                                                      1, // RGMII1_MDC
                                                      1, // RGMII1_MDIO
                                                      1, // RGMII1_TX_CLK
                                                      1, // RGMII1_TX_D0
                                                      1, // RGMII1_TX_D1
                                                      1, // RGMII1_TX_D2
                                                      1, // RGMII1_TX_D3
                                                      1  // RGMII1_TX_CTL
                                                  }};

u8 GMAC_Driving_Offset[GMAC_NUM][GMAC_MAX_IO] = {{0x73, 0x72, 0x6C, 0x6E, 0x6F, 0x70, 0x71, 0x6D},
                                                 {0x64, 0x63, 0x5D, 0x5F, 0x60, 0x61, 0x62, 0x5E}};

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
u16 G_txc_phase = txc_0_phase;
extern int G_gmac_calb;

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
    	if(s && strcmp(s, "MCLK") == 0)
    	{
        	//GPIO 138
        	bank = BASE_REG_PAD_GPIO2_PA;
        	offset = 0x0A;
    	}
    	else
    	{
        	//GPIO 101
        	bank = BASE_REG_PAD_GPIO_PA;
        	offset = 0x65;
    	}
#if	PHASE_CALB
    	if(!G_gmac_calb)
		{
			// Output Low
			OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
			mdelay(20);
			// Output High
			OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
			mdelay(50);
		}else
#endif
		if(gmac_reset0 == 0)
        {
            // Output Low
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
            mdelay(20);
            // Output High
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
            gmac_reset0 = 1;
            gmac_reset1 = 0;
            mdelay(300);
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
    	if(s && strcmp(s, "MCLK") == 0)
    	{
        	//GPIO 86
        	bank = BASE_REG_PAD_GPIO_PA;
        	offset = 0x56;
    	}
    	else
    	{
        	//GPIO 85
        	bank = BASE_REG_PAD_GPIO_PA;
        	offset = 0x55;
    	}
#if	PHASE_CALB
		if(!G_gmac_calb)
		{
			// Output Low
			OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
			mdelay(20);
			// Output High
			OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
			mdelay(50);
		}else
#endif
        if(gmac_reset1 == 0)
        {
            // Output Low
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x0008, 0x000F);
            mdelay(20);
            // Output High
            OUTREGMSK16(GET_REG_ADDR(bank, offset), 0x000A, 0x000F);
            mdelay(300);
            gmac_reset1=1;
            gmac_reset0=0;
        }
#endif
    }
}

static void mhal_gmac_mclk_setting(u8 gmacId)
{
	char *s1,*s2;

    if(gmacId == GMAC0)
    {
#if (GMAC0_OUTPUT_MCLK)
		// GPIO138 : disable GPIO mode ,enable output
		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO2_PA, 0x14), 0x0a50);
		// padtop reg_gphy0_ref_mode
		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0001, BIT1|BIT0);
		// CLKGEN2 reg_ckg_gphy0_ref
		OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, 0x1C), 0x0000);
#endif
    	//sstar_gmac_mclk : 0 , 25MHz , 50MHz
    	s1 = getenv ("sstar_gmac_mclk");

    	if(!s1 || strcmp(s1,"0") == 0)
    	{
    		//do nothing
    	}
    	else
    	{
    		//sstar_gmac_ref_mode : 1 or 2
    		s2 = getenv ("sstar_gmac_ref_mode");

    		if(s2)
    		{
    			if (strcmp(s2,"1") == 0)
    			{
    	    		// GPIO138 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO2_PA, 0x14), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0001, BIT1|BIT0);
    			}
    			else if (strcmp(s2,"2") == 0)
    			{
    	    		// GPIO101 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO_PA, 0xCA), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0002, BIT1|BIT0);
    			}
    			else
    			{
    				//default is 1
    	    		// GPIO138 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO2_PA, 0x14), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0001, BIT1|BIT0);
    			}
    		}

    		if (strcmp(s1,"25M") == 0)
    		{
	    		//CLKGEN2 reg_ckg_gphy0_ref
	    		OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, 0x1C), 0x0000);
    		}
    		else if (strcmp(s1,"50M") == 0)
    		{
	    		//CLKGEN2 reg_ckg_gphy0_ref
	    		OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, 0x1C), 0x0004);
    		}
    	}
    }
    else if(gmacId == GMAC1)
    {
    	//sstar_gmac_mclk : 0 , 25MHz , 50MHz
    	s1 = getenv ("sstar_gmac_mclk");

    	if(!s1 || strcmp(s1,"0") == 0)
    	{
    		//do nothing
    	}
    	else
    	{
    		//sstar_gmac_ref_mode : 1 or 2
    		s2 = getenv ("sstar_gmac_ref_mode");

    		if(s2)
    		{
    			if (strcmp(s2,"1") == 0)
    			{
    	    		// GPIO86 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO_PA, 0xAC), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0010, BIT4|BIT5);
    			}
    			else if (strcmp(s2,"2") == 0)
    			{
    	    		// GPIO85 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO_PA, 0xAA), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0020, BIT4|BIT5);
    			}
    			else
    			{
    				//default is 1
    	    		// GPIO86 : disable GPIO mode ,enable output
    	    		OUTREG8(GET_REG_ADDR8(BASE_REG_PAD_GPIO_PA, 0xAC), 0x0a50);
    	    		// padtop reg_gphy0_ref_mode
    	    		OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x38), 0x0010, BIT4|BIT5);
    			}
    		}

    		if (strcmp(s1,"25M") == 0)
    		{
	    		//CLKGEN2 reg_ckg_gphy0_ref
	    		OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, 0x1D), 0x0000);
    		}
    		else if (strcmp(s1,"50M") == 0)
    		{
	    		//CLKGEN2 reg_ckg_gphy0_ref
	    		OUTREG16(GET_REG_ADDR(BASE_REG_CLKGEN_2_PA, 0x1D), 0x0004);
    		}
    	}
    }
}

static void mhal_gmac_update_driving(u8 gmacId, u8 io_idx)
{
    if (io_idx == GAMC_DRIVING_UPDATEALL)
    { // Update All IO
        int i = 0;

        for (i = 0; i < GMAC_MAX_IO; i++)
        {
            OUTREGMSK16(GET_REG_ADDR(BASE_REG_PAD_GPIO_PA, GMAC_Driving_Offset[gmacId][i]),
                        (u16)(GMAC_Driving_Setting[gmacId][i] << GMAC_DRIVING_REG_SHIFT), GMAC_DRIVING_REG_MASK);
        }
    }
    else
    { // Update one IO by io_idx
        OUTREGMSK16(GET_REG_ADDR(BASE_REG_PAD_GPIO_PA, GMAC_Driving_Offset[gmacId][io_idx]),
                    (u16)(GMAC_Driving_Setting[gmacId][io_idx] << GMAC_DRIVING_REG_SHIFT), GMAC_DRIVING_REG_MASK);
    }
}

int sstar_gmac_probe(struct eth_device *dev, int interface, bd_t * bis)
{
    // disable long packet protection for gmac0
    OUTREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x28), INREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x28)) & 0x7FFF);
    // disable long packet protection for gmac1
    OUTREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x28), INREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x28)) & 0x7FFF);


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
    	mhal_gmac_mclk_setting(GMAC1);
        mhal_gmac_update_driving(GMAC1, GAMC_DRIVING_UPDATEALL);
        // padtop 103c -> RGMII 1.8V
        // OUTREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04), (INREG16(GET_REG_ADDR(BASE_REG_PADTOP_PA,0x04)) | 0x02));
#if (GMAC_DBG)
        printk("[%s][%d] SS GMAC1 Setting : Interface=%u\r\n", __func__, __LINE__, interface);
#endif
        if (interface == PHY_INTERFACE_MODE_MII)
        {
            OUTREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x01), 0x8000);
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
    	mhal_gmac_mclk_setting(GMAC0);
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
	char *s;
	int i = 0;
	u32 gmac_bank;

	for(i = 0 ; i < GMAC_NUM ; i++)
	{
		//overwrite settings by env "sstar_gmacx_txc"
		if (i == GMAC1)
		{
			s = getenv ("sstar_gmac1_txc");
			gmac_bank = BASE_REG_GMAC1_PA;
		}
		else
		{
			s = getenv ("sstar_gmac0_txc");
			gmac_bank = BASE_REG_GMAC0_PA;
		}

		if(s)
		{
			//BIT11~BIT12 : TXC Phase(B11->degree 90)(B10->degree 270)(B00->degree 0)(B01->degree 180)
			if(strcmp(s,"0") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x0000, (BIT11|BIT12));
				printf("tx degree 0\n");
			}
			else if(strcmp(s,"90") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x1800, (BIT11|BIT12));
				printf("tx degree 90\n");
			}
			else if(strcmp(s,"180") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x0800, (BIT11|BIT12));
				printf("tx degree 180\n");
			}
			else if(strcmp(s,"270") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x1000, (BIT11|BIT12));
				printf("tx degree 270\n");
			}
			else
			{
				//do nothing to keep default setting
			}
		}

		//overwrite settings by env "sstar_gmacx_rxc"
		if (i == GMAC1)
		{
			s = getenv ("sstar_gmac1_rxc");
		}
		else
		{
			s = getenv ("sstar_gmac0_rxc");
		}

		if(s)
		{
			//BIT8~BIT9 : RXC latch timing(B01->falling ,B10->raising (for 100Mbps only))
			if(strcmp(s,"falling") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x0100, (BIT8|BIT9));
				printf("rxc falling\n");
			}
			else if(strcmp(s,"raising") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x0200, (BIT8|BIT9));
				printf("rxc raising\n");
			}
			else if(strcmp(s,"none") == 0)
			{
				OUTREGMSK16(GET_REG_ADDR(gmac_bank, 0x31), 0x0000, (BIT8|BIT9));
				printf("rxc none\n");
			}
			else
			{
				//do nothing to keep default setting
			}
		}
	}
}

int sstar_gmac_set_tx_clk_pad_sel(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
	u16 val;

	////GMAC offset 0x31,default setting
	if (dev->iobase == GMAC1_Base)
	{
	    if (eqos->phy->interface == PHY_INTERFACE_MODE_RMII)
        {
            switch (eqos->phy->speed)
            {
                case SPEED_1000:
					//without overwrite bit 2,3
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x0000, ~GMAC_SEL_MSK);
                    break;
                case SPEED_100:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x2200, ~GMAC_SEL_MSK);
                    break;
                case SPEED_10:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x2200, ~GMAC_SEL_MSK);
                    break;
            }
        }
        else if (eqos->phy->interface == PHY_INTERFACE_MODE_RGMII)
        {
            switch (eqos->phy->speed)
            {
                case SPEED_1000:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x0000, ~GMAC_SEL_MSK);
                    break;
                case SPEED_100:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x140, ~GMAC_SEL_MSK);
                #if DYN_PHASE_CALB == 0
        			//reference bit 2,3 to calibrate tx clk phase
                    val = (INREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31)) & GMAC_CALB_MSK) >> 2;
        			//only update bit 11.12
        			OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), val<<11 , GMAC_TX_PHASE_MSK);
                #endif
                    break;
                case SPEED_10:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), 0x0280, ~GMAC_SEL_MSK);
                    break;
            }
         #if DYN_PHASE_CALB
			//reference bit 2,3 to calibrate tx clk phase
            val = (INREG16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31)) & GMAC_CALB_MSK) >> 2;
			//only update bit 11.12
			OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31), val<<11 , GMAC_TX_PHASE_MSK);
        #endif
        }
	}
	else
    {
        if (eqos->phy->interface == PHY_INTERFACE_MODE_RMII)
        {
            switch (eqos->phy->speed)
            {
                case SPEED_1000:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x0000, ~GMAC_SEL_MSK);
                    break;
                case SPEED_100:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x2200, ~GMAC_SEL_MSK);
                    break;
                case SPEED_10:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x2200, ~GMAC_SEL_MSK);
                    break;
            }
        }
        else if (eqos->phy->interface == PHY_INTERFACE_MODE_RGMII)
        {
            switch (eqos->phy->speed)
            {
                case SPEED_1000:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x0000, ~GMAC_SEL_MSK);
                    break;
                case SPEED_100:
					OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x140, ~GMAC_SEL_MSK);
                #if DYN_PHASE_CALB == 0
		            //reference bit 2,3 to calibrate tx clk phase
                    val = (INREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31)) & GMAC_CALB_MSK) >> 2;
		            //only update bit 11.12
		            OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), val<<11 , GMAC_TX_PHASE_MSK);
                #endif
                    break;
                case SPEED_10:
                    OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), 0x0280, ~GMAC_SEL_MSK);
                    break;
            }
        #if DYN_PHASE_CALB
			//reference bit 2,3 to calibrate tx clk phase
			val = (INREG16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31)) & GMAC_CALB_MSK) >> 2;
			//only update bit 11.12
			OUTREGMSK16(GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31), val<<11 , GMAC_TX_PHASE_MSK);
        #endif
        }
    }

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

#if DYN_PHASE_CALB

int gNeedCalibrat;
u16 gPHYReg0BackUp;

int dyn_loopback(struct eth_device *dev, u16 phase)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
	char gmac_calb_pass = 0;
	u32 ret=0;
	u32 reg;

	unsigned char packet_fill[]={
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x08, 0x06, 0x00, 0x01,
		0x08, 0x00, 0x06, 0x04, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xc0, 0xa8, 0x01, 0x02,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x01, 0x0f};

	if (dev->iobase == GMAC1_Base)
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31);
	}
	else
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31);
	}

	//only update bit 11.12
	OUTREGMSK16(reg, phase , GMAC_TX_PHASE_MSK);
	debug("TX Phase : 0x%08x \r\n",INREG16(reg));

	ret = eqos_send(dev, &packet_fill, sizeof(packet_fill));

	for(int i=0;i<6;i++){
		ret = eqos_recv(dev, 0, 0);
		if(ret != -EAGAIN){
			gmac_calb_pass = 1;
			break;
		}
		mdelay(1);
	}

	if(gmac_calb_pass == 1){
		//printf("Phase %x loopback pass ! \r\n",phase);
		return 1;
	}
	else{
		//debug("Phase %x loopback fail ! \r\n",phase);
		return 0;
	}

}

int sstar_gmac_prepare_dyncalibrat(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
	u16 val;
	u32 reg;
	u16 phy_speed;

	gNeedCalibrat = 0;

	if(eqos->phy->interface != PHY_INTERFACE_MODE_RGMII)
		return 0;

	if (dev->iobase == GMAC1_Base)
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31);
	}
	else
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31);
	}

	val = (INREG16(reg) & GMAC_RESV_SPEED_MSK) >> 14;
	//Check if we need calibration for current speed
    switch (eqos->phy->speed)
    {
        case SPEED_1000:
        	if(val == 1)
        		return 0;
        	phy_speed = 0x140;
            break;
        case SPEED_100:
        	if(val == 2)
        		return 0;
        	phy_speed = 0x2100;
            break;
        case SPEED_10:
        	if(val == 3)
        		return 0;
            break;
    }

    gNeedCalibrat = 1;
    gPHYReg0BackUp = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_BASIC);
	//Raising loopback bit
    eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC,  phy_speed | BIT14/*Loopback*/);
    mdelay(100);
	return -1;
}

void sstar_gmac_do_dyncalibrat(struct eth_device *dev)
{
	struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
	u16 val;
	u32 reg;

	if(!gNeedCalibrat)
		return;

	while(1){
		if (dyn_loopback(dev, txc_0_phase)){
			G_txc_phase = txc_0_phase;
			break;
		}
		if(dyn_loopback(dev, txc_180_phase)){
			G_txc_phase = txc_180_phase;
			break;
		}
		if(dyn_loopback(dev, txc_90_phase)){
			G_txc_phase = txc_90_phase;
			break;
		}
		if(dyn_loopback(dev, txc_270_phase)){
			G_txc_phase = txc_270_phase;
			break;
		}
		printf("\r\n\r\n");
		eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, (val & ~0x4000) | (BIT12/*AN enable*/ & gPHYReg0BackUp));
		return;
	}

	if (dev->iobase == GMAC1_Base)
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC1_PA, 0x31);
	}
	else
	{
		reg = GET_REG_ADDR(BASE_REG_GMAC0_PA, 0x31);
	}

	OUTREGMSK16(reg, G_txc_phase>>9, GMAC_CALB_MSK);

    switch (eqos->phy->speed)
    {
        case SPEED_1000:
        	OUTREGMSK16(reg, 1<<14 ,GMAC_RESV_SPEED_MSK);
            break;
        case SPEED_100:
        	OUTREGMSK16(reg, 2<<14 ,GMAC_RESV_SPEED_MSK);
            break;
        case SPEED_10:
        	OUTREGMSK16(reg, 3<<14 ,GMAC_RESV_SPEED_MSK);
            break;
    }

	//Clear loopback bit
	val = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_BASIC);
	eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, (val & ~0x4000) | (BIT12/*AN enable*/ & gPHYReg0BackUp));

	sstar_gmac_set_tx_clk_pad_sel(dev);
}
#endif

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

    if (PHY_INTERFACE_MODE_MII == eqos->phy->interface)
    {
        printf("Auto-Negotiation...\n");
        eqos_mdio_write(eqos->mii, phy_id, 0, PHY_REG_BASIC, 0x3100);
        goto jmp_cable_connection;
    }

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

jmp_cable_connection:
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
