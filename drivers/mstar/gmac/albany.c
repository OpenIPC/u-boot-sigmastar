/*
* albany.c- Sigmastar
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


#define SSTAR_BASE_REG_RIU_PA		(0x1F000000)

#define REG_BANK_ALBANY0         (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151800)
#define REG_BANK_ALBANY1         (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151900)
#define REG_BANK_ALBANY2         (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x151A00)
#define REG_BANK_CLKGEN0         (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x103800)
#define REG_BANK_SCGPCTRL        (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x113300)
#define REG_BANK_XTAL            (ulong) GET_BASE_ADDR_BY_BANK(SSTAR_BASE_REG_RIU_PA, 0x111b00)


#define GET_REG_ADDR8(x, y)      ((x) + ((y) << 1) - ((y)&1))

#define MHal_EMAC_WritReg8(bank, offset, val)   OUTREG8(GET_REG_ADDR8((bank), (offset)), (val))
#define MHal_EMAC_ReadReg8(bank, offset)        INREG8(GET_REG_ADDR8((bank), (offset)))

#define CONFIG_ETHERNET_ALBANY

static int b_albany_config_init = 0;

int albany_config_init(struct mii_dev *bus)
{
    u8 uRegVal;

    if (b_albany_config_init)
        return 0;

    //emac_clk gen
#ifdef CONFIG_ETHERNET_ALBANY

    // lucas
    uRegVal = MHal_EMAC_ReadReg8(REG_BANK_XTAL, 0x12);
    uRegVal = (uRegVal & (~0x20));
    MHal_EMAC_WritReg8(REG_BANK_XTAL, 0x12, uRegVal);



    /*  
        wriu    0x103884    0x00        //Set CLK_EMAC_AHB to 123MHz (Enabled)
        wriu    0x113344    0x00        //Set CLK_EMAC_RX to CLK_EMAC_RX_in (25MHz) (Enabled)
        wriu    0x113346    0x00        //Set CLK_EMAC_TX to CLK_EMAC_TX_IN (25MHz) (Enabled)
    */
    MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0x84, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x44, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x46, 0x00);


    /* eth_link_sar*/
    //gain shift
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xb4, 0x02);

    //det max
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x4f, 0x02);

    //det min
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x51, 0x01);

    //snr len (emc noise)
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x77, 0x18);

    //lpbk_enable set to 0
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x72, 0xa0);

    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xfc, 0x00);   // Power-on LDO
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xfd, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xa1, 0x80);   // Power-on SADC
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xcc, 0x40);   // Power-on ADCPL
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xbb, 0x04);   // Power-on REF
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0x3a, 0x00);   // Power-on TX
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xf1, 0x00);   // Power-on TX

    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0x8a, 0x01);    // CLKO_ADC_SEL
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x3b, 0x01);   // reg_adc_clk_select
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xc4, 0x44);  // TEST
    uRegVal = MHal_EMAC_ReadReg8(REG_BANK_ALBANY2, 0x80);
    uRegVal = (uRegVal & 0x0F) | 0x30;
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0x80, uRegVal); // sadc timer

    //100 gat
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xc5, 0x00);

    //200 gat
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0x30, 0x43);

    //en_100t_phase
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0x39, 0x41);   // en_100t_phase;  [6] save2x_tx

    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xf2, 0xf5);  // LP mode, DAC OFF
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xf3, 0x0d); // DAC off

    // Prevent packet drop by inverted waveform
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x79, 0xd0);   // prevent packet drop by inverted waveform
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x77, 0x5a);

    //disable eee
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x2d, 0x7c);   // disable eee

    //10T waveform
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xe8, 0x06);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x2b, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xe8, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0x2b, 0x00);

//    printf("[EMAC_todo] tune for analog\n");
    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xe8, 0x06);   // shadow_ctrl
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xaa, 0x1c);   // tin17_s2
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xac, 0x1c);   // tin18_s2
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xad, 0x1c);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xae, 0x1c);   // tin19_s2
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xaf, 0x1c);

    MHal_EMAC_WritReg8(REG_BANK_ALBANY2, 0xe8, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xaa, 0x1c);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY0, 0xab, 0x28);

    //speed up timing recovery
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xf5, 0x02);

    // Signal_det k
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x0f, 0xc9);

    // snr_h
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x89, 0x50);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x8b, 0x80);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x8e, 0x0e);
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0x90, 0x04);

    //set CLKsource to hv
    MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xC7, 0x80);
    // MHal_EMAC_WritReg8(REG_BANK_ALBANY1, 0xC7, 0x00);

#else
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc0, 0x00);
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc1, 0x04);
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc2, 0x04);
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc3, 0x00);
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc4, 0x00);
    //MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0xc5, 0x00);

    MHal_EMAC_WritReg8(REG_BANK_CLKGEN0, 0x84, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x44, 0x04);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x45, 0x00);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x46, 0x04);
    MHal_EMAC_WritReg8(REG_BANK_SCGPCTRL, 0x47, 0x00);

#endif

/*
    //chiptop [15] allpad_in
    uRegVal = MHal_EMAC_ReadReg8(REG_BANK_CHIPTOP, 0xa1);
    uRegVal &= 0x7f;
    MHal_EMAC_WritReg8(REG_BANK_CHIPTOP, 0xa1, uRegVal);
*/

    //et_mode = 1
#ifdef CONFIG_ETHERNET_ALBANY
    //uRegVal = MHal_EMAC_ReadReg8(REG_BANK_CHIPTOP, 0xdf);
    //uRegVal &= 0xfe;
    //MHal_EMAC_WritReg8(REG_BANK_CHIPTOP, 0xdf, uRegVal);
#else
    //0x101e_0f[2]
    uRegVal = MHal_EMAC_ReadReg8(REG_BANK_CHIPTOP, 0x1E);
    uRegVal |= 0x4;
    MHal_EMAC_WritReg8(REG_BANK_CHIPTOP, 0x1E, uRegVal);
#endif

    // enable LED
    OUTREGMSK16(GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x63), 0x0011, 0x0033);

    b_albany_config_init = 1;
    return 0;
}

