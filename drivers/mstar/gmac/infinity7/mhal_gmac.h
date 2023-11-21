/*
* mhal_gmac.h- Sigmastar
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
#include <phy.h>

//--------------------------------------------------------------------------------------------------
//  Constant definition
//--------------------------------------------------------------------------------------------------
#define TRUE                                1
#define FALSE                               0
#define BIT(x)                              (1<<x)
#define BUS_MIU0_BASE_CACHE                 0x20000000//CONFIG_SYS_MIU0_CACHE      //Beginning cachable bus address of MIU0
#define BUS_MIU0_BASE_NONCACHE              0x20000000//CONFIG_SYS_MIU0_NON_CACHE      //Beginning non-cachable bus address of MIU0 

#undef writel
#undef readl
#define writel(v,reg) ({ u32 __v = v; __iowmb(); __arch_putl(__v,dwmac_to_sigma_rebase(reg)); __v; })
#define readl(reg)    ({ u32 __v = __arch_getl(dwmac_to_sigma_rebase(reg)); __iormb(); __v; })

#undef clrbits_le32
#undef setbits_le32
#undef clrsetbits_le32


#define clrbits_le32(addr, clear) clrbits(le32, dwmac_to_sigma_rebase(addr), clear)
#define setbits_le32(addr, set) setbits(le32, dwmac_to_sigma_rebase(addr), set)
#define clrsetbits_le32(addr, clear, set) clrsetbits(le32, dwmac_to_sigma_rebase(addr), clear, set)

#define PHY_REG_BASIC	                    (0)
#define PHY_REG_STATUS	                    (1)
#define PHY_REG_ID_2	                    (2)
#define PHY_REG_ID_3	                    (3)
#define PHY_REG_ANAR	                    (4)
#define PHY_REG_LINK_PARTNER	            (5)
#define PHY_REG_GBCR	                    (9)
#define PHY_REG_GBSR	            		(10)
#define PHY_REG_GBESR	            		(15)
#define PHY_AN_DONE (0x1 << 5)
#define PHY_LINK_UP (0x1 << 2)
#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})
#define PATCH 0
//#define CONFIG_SYS_NONCACHED_MEMORY

#define GMAC0_Base               0x1F34A000
#define GMAC1_Base               0x1F34B400

#ifdef CONFIG_SSTAR_GMAC_FORCE_100
#define FORCE_100                1
#else
#define FORCE_100                0
#endif

#ifdef CONFIG_SSTAR_GMAC_FORCE_1000
#define FORCE_1000               1
#else
#define FORCE_1000               0
#endif
#define PHASE_CALB               0
#define DYN_PHASE_CALB           0
#define NEED_CALB                0
//--------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------

struct eqos_priv {
    struct eth_device *dev;
//    const struct eqos_config *config;
    phys_addr_t regs;
    struct eqos_mac_regs *mac_regs;
    struct eqos_mtl_regs *mtl_regs;
    struct eqos_dma_regs *dma_regs;
    struct eqos_tegra186_regs *tegra186_regs;
//    struct gpio_desc phy_reset_gpio;
//    struct clk clk_master_bus;
//    struct clk clk_rx;
//    struct clk clk_ptp_ref;
//    struct clk clk_tx;
//    struct clk clk_slave_bus;
    struct mii_dev *mii;
    struct phy_device *phy;
    void *descs;
    struct eqos_desc *tx_descs;
    struct eqos_desc *rx_descs;
    int tx_desc_idx, rx_desc_idx;
    void *tx_dma_buf;
    void *rx_dma_buf;
    void *rx_pkt;
    bool started;
    bool reg_access_ok;
    unsigned long desc_size;
};


//--------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------


int sstar_gmac_probe(struct eth_device *dev, int interface, bd_t * bis);
u32 dwmac_to_sigma_rebase(void __iomem *addr);
void MHal_GMAC_ReadReg32( u32* addr );
u8 MHal_GMAC_ReadReg8( u32 bank, u32 reg );
void MHal_GMAC_WritReg8( u32 bank, u32 reg, u8 val );
void MHal_GMAC_NegotiationPHY(struct eth_device *dev);
int sstar_gmac_set_tx_clk_pad_sel(struct eth_device *dev);
int sstar_gmac_set_rate(struct eth_device *dev);
void MHal_GMAC_Write_MACAddr(struct eth_device *dev, char enetaddr[]);
void ssta_gmac_update_trxc(void);

