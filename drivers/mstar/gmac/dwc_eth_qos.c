/*
* dwc_eth_qos.c- Sigmastar
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

/* Core registers */

#define EQOS_MAC_REGS_BASE 0x000
struct eqos_mac_regs {
    uint32_t configuration;             /* 0x000 */
    uint32_t unused_004[(0x070 - 0x004) / 4];   /* 0x004 */
    uint32_t q0_tx_flow_ctrl;           /* 0x070 */
    uint32_t unused_070[(0x090 - 0x074) / 4];   /* 0x074 */
    uint32_t rx_flow_ctrl;              /* 0x090 */
    uint32_t unused_094;                /* 0x094 */
    uint32_t txq_prty_map0;             /* 0x098 */
    uint32_t unused_09c;                /* 0x09c */
    uint32_t rxq_ctrl0;             /* 0x0a0 */
    uint32_t unused_0a4;                /* 0x0a4 */
    uint32_t rxq_ctrl2;             /* 0x0a8 */
    uint32_t unused_0ac[(0x0dc - 0x0ac) / 4];   /* 0x0ac */
    uint32_t us_tic_counter;            /* 0x0dc */
    uint32_t unused_0e0[(0x11c - 0x0e0) / 4];   /* 0x0e0 */
    uint32_t hw_feature0;               /* 0x11c */
    uint32_t hw_feature1;               /* 0x120 */
    uint32_t hw_feature2;               /* 0x124 */
    uint32_t unused_128[(0x200 - 0x128) / 4];   /* 0x128 */
    uint32_t mdio_address;              /* 0x200 */
    uint32_t mdio_data;             /* 0x204 */
    uint32_t unused_208[(0x300 - 0x208) / 4];   /* 0x208 */
    uint32_t address0_high;             /* 0x300 */
    uint32_t address0_low;              /* 0x304 */
    uint32_t address1_high;             /* 0x308 */
    uint32_t address1_low;              /* 0x30c */
    uint32_t address2_high;             /* 0x310 */
    uint32_t address2_low;              /* 0x314 */
    uint32_t address3_high;             /* 0x318 */
    uint32_t address3_low;              /* 0x31c */
    uint32_t address4_high;             /* 0x310 */
    uint32_t address4_low;              /* 0x314 */
    uint32_t address5_high;             /* 0x310 */
    uint32_t address5_low;              /* 0x314 */
    uint32_t address6_high;             /* 0x310 */
    uint32_t address6_low;              /* 0x314 */
    uint32_t address7_high;             /* 0x310 */
    uint32_t address7_low;              /* 0x314 */
};

#define EQOS_MAC_CONFIGURATION_GPSLCE           BIT(23)
#define EQOS_MAC_CONFIGURATION_CST          BIT(21)
#define EQOS_MAC_CONFIGURATION_ACS          BIT(20)
#define EQOS_MAC_CONFIGURATION_WD           BIT(19)
#define EQOS_MAC_CONFIGURATION_JD           BIT(17)
#define EQOS_MAC_CONFIGURATION_JE           BIT(16)
#define EQOS_MAC_CONFIGURATION_PS           BIT(15)
#define EQOS_MAC_CONFIGURATION_FES          BIT(14)
#define EQOS_MAC_CONFIGURATION_DM           BIT(13)
#define EQOS_MAC_CONFIGURATION_TE           BIT(1)
#define EQOS_MAC_CONFIGURATION_RE           BIT(0)

#define EQOS_MAC_Q0_TX_FLOW_CTRL_PT_SHIFT       16
#define EQOS_MAC_Q0_TX_FLOW_CTRL_PT_MASK        0xffff
#define EQOS_MAC_Q0_TX_FLOW_CTRL_TFE            BIT(1)

#define EQOS_MAC_RX_FLOW_CTRL_RFE           BIT(0)

#define EQOS_MAC_TXQ_PRTY_MAP0_PSTQ0_SHIFT      0
#define EQOS_MAC_TXQ_PRTY_MAP0_PSTQ0_MASK       0xff

#define EQOS_MAC_RXQ_CTRL0_RXQ0EN_SHIFT         0
#define EQOS_MAC_RXQ_CTRL0_RXQ0EN_MASK          3
#define EQOS_MAC_RXQ_CTRL0_RXQ0EN_NOT_ENABLED       0
#define EQOS_MAC_RXQ_CTRL0_RXQ0EN_ENABLED_DCB       2

#define EQOS_MAC_RXQ_CTRL2_PSRQ0_SHIFT          0
#define EQOS_MAC_RXQ_CTRL2_PSRQ0_MASK           0xff

#define EQOS_MAC_HW_FEATURE1_TXFIFOSIZE_SHIFT       6
#define EQOS_MAC_HW_FEATURE1_TXFIFOSIZE_MASK        0x1f
#define EQOS_MAC_HW_FEATURE1_RXFIFOSIZE_SHIFT       0
#define EQOS_MAC_HW_FEATURE1_RXFIFOSIZE_MASK        0x1f

#define EQOS_MAC_MDIO_ADDRESS_PA_SHIFT          21
#define EQOS_MAC_MDIO_ADDRESS_RDA_SHIFT         16
#define EQOS_MAC_MDIO_ADDRESS_CR_SHIFT          8
#define EQOS_MAC_MDIO_ADDRESS_CR_20_35          7//2
#define EQOS_MAC_MDIO_ADDRESS_SKAP          BIT(4)
#define EQOS_MAC_MDIO_ADDRESS_GOC_SHIFT         2
#define EQOS_MAC_MDIO_ADDRESS_GOC_READ          3
#define EQOS_MAC_MDIO_ADDRESS_GOC_WRITE         1
#define EQOS_MAC_MDIO_ADDRESS_C45E          BIT(1)
#define EQOS_MAC_MDIO_ADDRESS_GB            BIT(0)

#define EQOS_MAC_MDIO_DATA_GD_MASK          0xffff

#define EQOS_MTL_REGS_BASE 0xd00
struct eqos_mtl_regs {
    uint32_t txq0_operation_mode;           /* 0xd00 */
    uint32_t unused_d04;                /* 0xd04 */
    uint32_t txq0_debug;                /* 0xd08 */
    uint32_t unused_d0c[(0xd18 - 0xd0c) / 4];   /* 0xd0c */
    uint32_t txq0_quantum_weight;           /* 0xd18 */
    uint32_t unused_d1c[(0xd30 - 0xd1c) / 4];   /* 0xd1c */
    uint32_t rxq0_operation_mode;           /* 0xd30 */
    uint32_t unused_d34;                /* 0xd34 */
    uint32_t rxq0_debug;                /* 0xd38 */
};

#define EQOS_MTL_TXQ0_OPERATION_MODE_TQS_SHIFT      16
#define EQOS_MTL_TXQ0_OPERATION_MODE_TQS_MASK       0x1ff
#define EQOS_MTL_TXQ0_OPERATION_MODE_TXQEN_SHIFT    2
#define EQOS_MTL_TXQ0_OPERATION_MODE_TXQEN_MASK     3
#define EQOS_MTL_TXQ0_OPERATION_MODE_TXQEN_ENABLED  2
#define EQOS_MTL_TXQ0_OPERATION_MODE_TSF        BIT(1)
#define EQOS_MTL_TXQ0_OPERATION_MODE_FTQ        BIT(0)

#define EQOS_MTL_TXQ0_DEBUG_TXQSTS          BIT(4)
#define EQOS_MTL_TXQ0_DEBUG_TRCSTS_SHIFT        1
#define EQOS_MTL_TXQ0_DEBUG_TRCSTS_MASK         3

#define EQOS_MTL_RXQ0_OPERATION_MODE_RQS_SHIFT      20
#define EQOS_MTL_RXQ0_OPERATION_MODE_RQS_MASK       0x3ff
#define EQOS_MTL_RXQ0_OPERATION_MODE_RFD_SHIFT      14
#define EQOS_MTL_RXQ0_OPERATION_MODE_RFD_MASK       0x3f
#define EQOS_MTL_RXQ0_OPERATION_MODE_RFA_SHIFT      8
#define EQOS_MTL_RXQ0_OPERATION_MODE_RFA_MASK       0x3f
#define EQOS_MTL_RXQ0_OPERATION_MODE_EHFC       BIT(7)
#define EQOS_MTL_RXQ0_OPERATION_MODE_RSF        BIT(5)

#define EQOS_MTL_RXQ0_DEBUG_PRXQ_SHIFT          16
#define EQOS_MTL_RXQ0_DEBUG_PRXQ_MASK           0x7fff
#define EQOS_MTL_RXQ0_DEBUG_RXQSTS_SHIFT        4
#define EQOS_MTL_RXQ0_DEBUG_RXQSTS_MASK         3

#define EQOS_DMA_REGS_BASE 0x1000
struct eqos_dma_regs {
    uint32_t mode;                  /* 0x1000 */
    uint32_t sysbus_mode;               /* 0x1004 */
    uint32_t unused_1008[(0x1100 - 0x1008) / 4];    /* 0x1008 */
    uint32_t ch0_control;               /* 0x1100 */
    uint32_t ch0_tx_control;            /* 0x1104 */
    uint32_t ch0_rx_control;            /* 0x1108 */
    uint32_t unused_110c;               /* 0x110c */
    uint32_t ch0_txdesc_list_haddress;      /* 0x1110 */
    uint32_t ch0_txdesc_list_address;       /* 0x1114 */
    uint32_t ch0_rxdesc_list_haddress;      /* 0x1118 */
    uint32_t ch0_rxdesc_list_address;       /* 0x111c */
    uint32_t ch0_txdesc_tail_pointer;       /* 0x1120 */
    uint32_t unused_1124;               /* 0x1124 */
    uint32_t ch0_rxdesc_tail_pointer;       /* 0x1128 */
    uint32_t ch0_txdesc_ring_length;        /* 0x112c */
    uint32_t ch0_rxdesc_ring_length;        /* 0x1130 */
};

#define EQOS_DMA_MODE_SWR               BIT(0)

#define EQOS_DMA_SYSBUS_MODE_RD_OSR_LMT_SHIFT       16
#define EQOS_DMA_SYSBUS_MODE_RD_OSR_LMT_MASK        0xf
#define EQOS_DMA_SYSBUS_MODE_EAME           BIT(11)
#define EQOS_DMA_SYSBUS_MODE_BLEN16         BIT(3)
#define EQOS_DMA_SYSBUS_MODE_BLEN8          BIT(2)
#define EQOS_DMA_SYSBUS_MODE_BLEN4          BIT(1)

#define EQOS_DMA_CH0_CONTROL_DSL_SHIFT			18
#define EQOS_DMA_CH0_CONTROL_PBLX8			BIT(16)

#define EQOS_DMA_CH0_TX_CONTROL_TXPBL_SHIFT     16
#define EQOS_DMA_CH0_TX_CONTROL_TXPBL_MASK      0x3f
#define EQOS_DMA_CH0_TX_CONTROL_OSP         BIT(4)
#define EQOS_DMA_CH0_TX_CONTROL_ST          BIT(0)

#define EQOS_DMA_CH0_RX_CONTROL_RXPBL_SHIFT     16
#define EQOS_DMA_CH0_RX_CONTROL_RXPBL_MASK      0x3f
#define EQOS_DMA_CH0_RX_CONTROL_RBSZ_SHIFT      1
#define EQOS_DMA_CH0_RX_CONTROL_RBSZ_MASK       0x3fff
#define EQOS_DMA_CH0_RX_CONTROL_SR          BIT(0)

/* These registers are Tegra186-specific */
#define EQOS_TEGRA186_REGS_BASE 0x8800
struct eqos_tegra186_regs {
    uint32_t sdmemcomppadctrl;          /* 0x8800 */
    uint32_t auto_cal_config;           /* 0x8804 */
    uint32_t unused_8808;               /* 0x8808 */
    uint32_t auto_cal_status;           /* 0x880c */
};

#define EQOS_SDMEMCOMPPADCTRL_PAD_E_INPUT_OR_E_PWRD BIT(31)

#define EQOS_AUTO_CAL_CONFIG_START          BIT(31)
#define EQOS_AUTO_CAL_CONFIG_ENABLE         BIT(29)

#define EQOS_AUTO_CAL_STATUS_ACTIVE         BIT(31)

/* Descriptors */
#define EQOS_DESCRIPTOR_SIZE    (64)
/* We assume ARCH_DMA_MINALIGN >= 16; 16 is the EQOS HW minimum */
#define EQOS_DESCRIPTOR_ALIGN   ARCH_DMA_MINALIGN
#define EQOS_DESCRIPTORS_TX 16
#define EQOS_DESCRIPTORS_RX 64
#define EQOS_DESCRIPTORS_NUM    (EQOS_DESCRIPTORS_TX + EQOS_DESCRIPTORS_RX)
#define EQOS_TX_DESCRIPTORS_SIZE   ALIGN(EQOS_DESCRIPTORS_TX * \
                      EQOS_DESCRIPTOR_SIZE, ARCH_DMA_MINALIGN)
#define EQOS_RX_DESCRIPTORS_SIZE   ALIGN(EQOS_DESCRIPTORS_RX * \
                      EQOS_DESCRIPTOR_SIZE, ARCH_DMA_MINALIGN)
#define EQOS_BUFFER_ALIGN   ARCH_DMA_MINALIGN
#define EQOS_MAX_PACKET_SIZE    ALIGN(1568, ARCH_DMA_MINALIGN)
#define EQOS_RX_BUFFER_SIZE (EQOS_DESCRIPTORS_RX * EQOS_MAX_PACKET_SIZE)

/*
 * Warn if the cache-line size is larger than the descriptor size. In such
 * cases the driver will likely fail because the CPU needs to flush the cache
 * when requeuing RX buffers, therefore descriptors written by the hardware
 * may be discarded. Architectures with full IO coherence, such as x86, do not
 * experience this issue, and hence are excluded from this condition.
 *
 * This can be fixed by defining CONFIG_SYS_NONCACHED_MEMORY which will cause
 * the driver to allocate descriptors from a pool of non-cached memory.
 */
#if EQOS_DESCRIPTOR_SIZE < ARCH_DMA_MINALIGN
#if !defined(CONFIG_SYS_NONCACHED_MEMORY) && \
    !defined(CONFIG_SYS_DCACHE_OFF) && !defined(CONFIG_X86)
#warning Cache line size is larger than descriptor size
#endif
#endif

struct eqos_desc {
    u32 des0;
    u32 des1;
    u32 des2;
    u32 des3;
};

#define EQOS_DESC3_OWN      BIT(31)
#define EQOS_DESC3_FD       BIT(29)
#define EQOS_DESC3_LD       BIT(28)
#define EQOS_DESC3_BUF1V    BIT(24)

//struct eqos_config {
//    bool reg_access_always_ok;
//};
#define SSTAR_GMAC_DBG 0

/*******************************************************************************
								Global Define
*******************************************************************************/
#define PA2BUS(a)  ((a) - (BUS_MIU0_BASE_CACHE))
#define BUS2PA(a)  ((a) + (BUS_MIU0_BASE_CACHE))

struct mii_dev Gmii;

extern void Chip_Flush_Memory(void);
unsigned char phy_id=0;
int gmac_init=0;
int G_gmac_calb = 0;


/*
 * TX and RX descriptors are 16 bytes. This causes problems with the cache
 * maintenance on CPUs where the cache-line size exceeds the size of these
 * descriptors. What will happen is that when the driver receives a packet
 * it will be immediately requeued for the hardware to reuse. The CPU will
 * therefore need to flush the cache-line containing the descriptor, which
 * will cause all other descriptors in the same cache-line to be flushed
 * along with it. If one of those descriptors had been written to by the
 * device those changes (and the associated packet) will be lost.
 *
 * To work around this, we make use of non-cached memory if available. If
 * descriptors are mapped uncached there's no need to manually flush them
 * or invalidate them.
 *
 * Note that this only applies to descriptors. The packet data buffers do
 * not have the same constraints since they are 1536 bytes large, so they
 * are unlikely to share cache-lines.
 */
static void *eqos_alloc_descs(struct eqos_priv *eqos, unsigned int num)
{
	eqos->desc_size = ALIGN(sizeof(struct eqos_desc),
				(unsigned int)ARCH_DMA_MINALIGN);

	return memalign(eqos->desc_size, num * eqos->desc_size);
}

static void eqos_free_descs(void *descs)
{
    free(descs);
}

static void eqos_inval_desc(void *desc)
{
    unsigned long start = (unsigned long)desc & ~(ARCH_DMA_MINALIGN - 1);
    unsigned long end = ALIGN(start + EQOS_DESCRIPTOR_SIZE,
                  ARCH_DMA_MINALIGN);
    invalidate_dcache_range(start, end);
}

static void eqos_flush_desc(void *desc)
{
#if 1
	unsigned long desc_start = (uint32_t)desc;
	unsigned long desc_end = desc_start + roundup(sizeof(*desc), ARCH_DMA_MINALIGN);

	flush_dcache_range(desc_start, desc_end);
#else
    flush_cache((unsigned long)desc, EQOS_DESCRIPTOR_SIZE);
#endif
	Chip_Flush_Memory();
}

static void eqos_inval_buffer(void *buf, size_t size)
{
    unsigned long start = (unsigned long)buf & ~(ARCH_DMA_MINALIGN - 1);
    unsigned long end = ALIGN(start + size, ARCH_DMA_MINALIGN);

    invalidate_dcache_range(start, end);
}

static void eqos_flush_buffer(void *buf, size_t size)
{
    flush_cache((unsigned long)buf, size);
}

static int eqos_mdio_wait_idle(struct eqos_priv *eqos)
{
    return wait_for_bit(__func__, &eqos->mac_regs->mdio_address,
                EQOS_MAC_MDIO_ADDRESS_GB, false, 1000000, true);
}

int eqos_mdio_read(struct mii_dev *bus, int mdio_addr, int mdio_devad,
              int mdio_reg)
{
    struct eqos_priv *eqos = bus->priv;
    u32 val;
    int ret;

    debug("%s(dev=%p, addr=%x, reg=%d):\n", __func__, eqos->dev, mdio_addr,
          mdio_reg);

    ret = eqos_mdio_wait_idle(eqos);
    if (ret) {
        error("MDIO not idle at entry");
        return ret;
    }

    val = readl(&eqos->mac_regs->mdio_address);
    val &= EQOS_MAC_MDIO_ADDRESS_SKAP |
        EQOS_MAC_MDIO_ADDRESS_C45E;
    val |= (mdio_addr << EQOS_MAC_MDIO_ADDRESS_PA_SHIFT) |
        (mdio_reg << EQOS_MAC_MDIO_ADDRESS_RDA_SHIFT) |
        (EQOS_MAC_MDIO_ADDRESS_CR_20_35 <<
         EQOS_MAC_MDIO_ADDRESS_CR_SHIFT) |
        (EQOS_MAC_MDIO_ADDRESS_GOC_READ <<
         EQOS_MAC_MDIO_ADDRESS_GOC_SHIFT) |
        EQOS_MAC_MDIO_ADDRESS_GB;
    writel(val, &eqos->mac_regs->mdio_address);

    udelay(10);

    ret = eqos_mdio_wait_idle(eqos);
    if (ret) {
        error("MDIO read didn't complete");
        return ret;
    }

    val = readl(&eqos->mac_regs->mdio_data);
    val &= EQOS_MAC_MDIO_DATA_GD_MASK;

    debug("%s: val=%x\n", __func__, val);

    return val;
}

int eqos_mdio_write(struct mii_dev *bus, int mdio_addr, int mdio_devad,
               int mdio_reg, u16 mdio_val)
{
    struct eqos_priv *eqos = bus->priv;
    u32 val;
    int ret;

    debug("%s(dev=%p, addr=%x, reg=%d, val=%x):\n", __func__, eqos->dev,
          mdio_addr, mdio_reg, mdio_val);

    ret = eqos_mdio_wait_idle(eqos);
    if (ret) {
        error("MDIO not idle at entry");
        return ret;
    }

    writel(mdio_val, &eqos->mac_regs->mdio_data);

    val = readl(&eqos->mac_regs->mdio_address);
    val &= EQOS_MAC_MDIO_ADDRESS_SKAP |
        EQOS_MAC_MDIO_ADDRESS_C45E;
    val |= (mdio_addr << EQOS_MAC_MDIO_ADDRESS_PA_SHIFT) |
        (mdio_reg << EQOS_MAC_MDIO_ADDRESS_RDA_SHIFT) |
        (EQOS_MAC_MDIO_ADDRESS_CR_20_35 <<
         EQOS_MAC_MDIO_ADDRESS_CR_SHIFT) |
        (EQOS_MAC_MDIO_ADDRESS_GOC_WRITE <<
         EQOS_MAC_MDIO_ADDRESS_GOC_SHIFT) |
        EQOS_MAC_MDIO_ADDRESS_GB;
    writel(val, &eqos->mac_regs->mdio_address);

    udelay(10);

    ret = eqos_mdio_wait_idle(eqos);
    if (ret) {
        error("MDIO read didn't complete");
        return ret;
    }

    return 0;
}

static int eqos_set_full_duplex(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    setbits_le32(&eqos->mac_regs->configuration, EQOS_MAC_CONFIGURATION_DM);

    return 0;
}

static int eqos_set_half_duplex(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    clrbits_le32(&eqos->mac_regs->configuration, EQOS_MAC_CONFIGURATION_DM);

    /* WAR: Flush TX queue when switching to half-duplex */
    setbits_le32(&eqos->mtl_regs->txq0_operation_mode,
             EQOS_MTL_TXQ0_OPERATION_MODE_FTQ);

    return 0;
}

static int eqos_set_gmii_speed(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    clrbits_le32(&eqos->mac_regs->configuration,
             EQOS_MAC_CONFIGURATION_PS | EQOS_MAC_CONFIGURATION_FES);

    return 0;
}

static int eqos_set_mii_speed_100(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    setbits_le32(&eqos->mac_regs->configuration,
             EQOS_MAC_CONFIGURATION_PS | EQOS_MAC_CONFIGURATION_FES);

    return 0;
}

static int eqos_set_mii_speed_10(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    clrsetbits_le32(&eqos->mac_regs->configuration,
            EQOS_MAC_CONFIGURATION_FES, EQOS_MAC_CONFIGURATION_PS);

    return 0;
}

static int eqos_adjust_link(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    int ret;

    debug("%s(dev=%p):\n", __func__, dev);

    if (eqos->phy->duplex)
        ret = eqos_set_full_duplex(dev);
    else
        ret = eqos_set_half_duplex(dev);
    if (ret < 0) {
        error("eqos_set_*_duplex() failed: %d", ret);
        return ret;
    }

    switch (eqos->phy->speed) {
    case SPEED_1000:
        ret = eqos_set_gmii_speed(dev);
        printf("1000M\n");
        break;
    case SPEED_100:
    	printf("100M\n");
        ret = eqos_set_mii_speed_100(dev);
        break;
    case SPEED_10:
        ret = eqos_set_mii_speed_10(dev);
        printf("10M\n");
        break;
    default:
        error("invalid speed %d", eqos->phy->speed);
        return -EINVAL;
    }
    if (ret < 0) {
        error("eqos_set_*mii_speed*() failed: %d", ret);
        return ret;
    }

    return 0;
}

static int eqos_write_hwaddr(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    uint32_t val;

    /*
     * This function may be called before start() or after stop(). At that
     * time, on at least some configurations of the EQoS HW, all clocks to
     * the EQoS HW block will be stopped, and a reset signal applied. If
     * any register access is attempted in this state, bus timeouts or CPU
     * hangs may occur. This check prevents that.
     *
     * A simple solution to this problem would be to not implement
     * write_hwaddr(), since start() always writes the MAC address into HW
     * anyway. However, it is desirable to implement write_hwaddr() to
     * support the case of SW that runs subsequent to U-Boot which expects
     * the MAC address to already be programmed into the EQoS registers,
     * which must happen irrespective of whether the U-Boot user (or
     * scripts) actually made use of the EQoS device, and hence
     * irrespective of whether start() was ever called.
     *
     * Note that this requirement by subsequent SW is not valid for
     * Tegra186, and is likely not valid for any non-PCI instantiation of
     * the EQoS HW block. This function is implemented solely as
     * future-proofing with the expectation the driver will eventually be
     * ported to some system where the expectation above is true.
     */
    if (!eqos->reg_access_ok)
        return 0;

    writel(0x00000000, &eqos->mac_regs->address0_high);
	writel(0x00000000, &eqos->mac_regs->address0_low);

    /* Update the MAC address */
    val = (dev->enetaddr[5] << 8) |
        (dev->enetaddr[4]);
    writel(val, &eqos->mac_regs->address0_high);
    val = (dev->enetaddr[3] << 24) |
        (dev->enetaddr[2] << 16) |
        (dev->enetaddr[1] << 8) |
        (dev->enetaddr[0]);
    writel(val, &eqos->mac_regs->address0_low);

    writel(0x00000000, &eqos->mac_regs->address1_high);
	writel(0x00000000, &eqos->mac_regs->address1_low);
    writel(0x00000000, &eqos->mac_regs->address2_high);
	writel(0x00000000, &eqos->mac_regs->address2_low);
    writel(0x00000000, &eqos->mac_regs->address3_high);
	writel(0x00000000, &eqos->mac_regs->address3_low);
    writel(0x00000000, &eqos->mac_regs->address4_high);
	writel(0x00000000, &eqos->mac_regs->address4_low);
    writel(0x00000000, &eqos->mac_regs->address5_high);
	writel(0x00000000, &eqos->mac_regs->address5_low);
    writel(0x00000000, &eqos->mac_regs->address6_high);
	writel(0x00000000, &eqos->mac_regs->address6_low);
    writel(0x00000000, &eqos->mac_regs->address7_high);
	writel(0x00000000, &eqos->mac_regs->address7_low);

    return 0;
}

#if(SSTAR_GMAC_DBG)
void MDrv_GMAC_DumpPhy(struct mii_dev *bus)
{
	u32 idx;
	int bmcr;

    printf("======= PHY Reg =======\n");

    for (idx = 0; idx < 0x20; idx++)
    {
		bmcr = eqos_mdio_read(bus, 1, 0, idx);
        printf("  Reg-%-3X=%-4x   ",idx, bmcr);
        if (idx%5 == 4)
            printf("\n");
    }
	printf("\n");

}
#endif

extern int ip101a_g_config_init(struct mii_dev *mii);
extern int albany_config_init(struct mii_dev *bus);
extern int motorcomm_config_init(struct eth_device *dev);

int MDrv_GMAC_Phy_Detect(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    u32 phy_id0 = 0x00000000;
    u32 phy_id1 = 0x00000000;

    phy_id0 = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_2);
    phy_id1 = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_3);

    if(((0xffff != phy_id0)&&(0x0000 != phy_id0)) || ((0xffff != phy_id1)&&(0x0000 != phy_id1)))
    {
    	printf("phy_id:%u ,0x%x,0x%x\n",phy_id,phy_id0,phy_id1);
        return 0;
    }
	return -1;
}


void MDrv_GMAC_PhyAddrScan(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    u32 word_ETH_MAN  = 0x00000000;

    word_ETH_MAN = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);

    if((0xffff != word_ETH_MAN)&&(0x0000 != word_ETH_MAN))
    {
        printf("phy id [%d]=%04x\n",phy_id, word_ETH_MAN);
    	printf("phy_id:%u ,0x%04x,0x%04x\n",phy_id,\
    			eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_2),\
    			eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_3));
        return;
    }

    for (phy_id = 0; phy_id < 32; phy_id++)
    {
        word_ETH_MAN = eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_STATUS);
        printf("phy [%d]=%04x\n",phy_id, word_ETH_MAN);
        if((0xffff != word_ETH_MAN)&&(0x0000 != word_ETH_MAN))
        {
        	printf("phy_id:%u ,0x%04x,0x%04x\n",phy_id,\
        			eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_2),\
        			eqos_mdio_read(eqos->mii, phy_id, 0, PHY_REG_ID_3));
            return;
        }
    }

    printf("Can't get correct PHY Addr and reset to 0\n");
    phy_id = 0;
}

void MDrv_GMAC_AutoNegotiationPHY(struct eth_device *dev)
{
	// IMPORTANT: Run NegotiationPHY() before writing REG_ETH_CFG.
	MHal_GMAC_NegotiationPHY(dev);
}

static int eqos_start(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    int ret=0, i;
//    ulong rate;
    u32 val, tx_fifo_sz, rx_fifo_sz, tqs, rqs, pbl;
    ulong last_rx_desc;
	u32 value;
	int limit;
    unsigned long desc_pad;
	
    debug("%s(dev=%p):\n", __func__, dev);

    eqos->tx_desc_idx = 0;
    eqos->rx_desc_idx = 0;

    eqos->reg_access_ok = true;

#if 0
    ret = wait_for_bit(__func__, &eqos->dma_regs->mode,
               EQOS_DMA_MODE_SWR, false, 10, false);
    if (ret) {
        error("EQOS_DMA_MODE_SWR stuck");
        goto err_stop_resets;
    }
#else

    value = readl(&eqos->dma_regs->mode);//0x1f34b000);
    /* DMA SW reset */
    value |= EQOS_DMA_MODE_SWR;
    writel(value,&eqos->dma_regs->mode);//0x1f34b000);
    limit = 10;
    while (limit--) {
	    if (!(readl(&eqos->dma_regs->mode) & EQOS_DMA_MODE_SWR))
		    break;
	    mdelay(10);
    }

    if(limit == 0)
    	goto err_stop_resets;
#endif

#if 0 //NOPHYLIB
    eqos->phy = phy_connect(eqos->mii, PHY_REG, dev, 0);
    if (!eqos->phy) {
        error("phy_connect() failed");
        goto err_stop_resets;
    }

	ip101a_g_config_init(eqos->mii);

    ret = phy_config(eqos->phy);
    if (ret < 0) {
        error("phy_config() failed: %d", ret);
        goto err_shutdown_phy;
    }
    ret = phy_startup(eqos->phy);
    if (ret < 0) {
        error("phy_startup() failed: %d", ret);
        goto err_shutdown_phy;
    }
#else

#if	PHASE_CALB
	if(!G_gmac_calb)
	{
		// read phy 0 only if failed return
		ret = MDrv_GMAC_Phy_Detect(dev);
		if (ret < 0) {
			debug("No phy detect ! \r\n");
			return ret;
		}
	}
	else
#endif
	{
		MDrv_GMAC_PhyAddrScan(dev);
	}

	if (PHY_INTERFACE_MODE_MII == eqos->phy->interface) {
		albany_config_init(eqos->mii);
	}
	else {
	    ip101a_g_config_init(eqos->mii);
	}

#if	PHASE_CALB
	if(G_gmac_calb)
#endif
	{
		MDrv_GMAC_AutoNegotiationPHY(dev);
	}
#endif

    if (!eqos->phy->link) {
        error("No link");
        goto err_shutdown_phy;
    }

    ret = eqos_adjust_link(dev);
    if (ret < 0) {
        error("eqos_adjust_link() failed: %d", ret);
        goto err_shutdown_phy;
    }

#ifdef CONFIG_MOTORCOMM_PHY
	motorcomm_config_init(dev);
#endif

#if DYN_PHASE_CALB
    sstar_gmac_prepare_dyncalibrat(dev);
#endif

    sstar_gmac_set_rate(dev);

#if	PHASE_CALB
	if(G_gmac_calb)
#endif
	{
		sstar_gmac_set_tx_clk_pad_sel(dev);
	}

    /* Configure MTL */
    //printf("[matt] &eqos->mtl_regs->txq0_operation_mode d00: 0x%08x \r\n",(u32)&eqos->mtl_regs->txq0_operation_mode);
    /* Enable Store and Forward mode for TX */
    /* Program Tx operating mode */
    setbits_le32(&eqos->mtl_regs->txq0_operation_mode,
             EQOS_MTL_TXQ0_OPERATION_MODE_TSF |
             (EQOS_MTL_TXQ0_OPERATION_MODE_TXQEN_ENABLED <<
              EQOS_MTL_TXQ0_OPERATION_MODE_TXQEN_SHIFT));

    /* Transmit Queue weight */
    writel(0x10, &eqos->mtl_regs->txq0_quantum_weight);

    /* Enable Store and Forward mode for RX, since no jumbo frame */
    setbits_le32(&eqos->mtl_regs->rxq0_operation_mode,
             EQOS_MTL_RXQ0_OPERATION_MODE_RSF);

    /* Transmit/Receive queue fifo size; use all RAM for 1 queue */
    val = readl(&eqos->mac_regs->hw_feature1);
    tx_fifo_sz = (val >> EQOS_MAC_HW_FEATURE1_TXFIFOSIZE_SHIFT) &
        EQOS_MAC_HW_FEATURE1_TXFIFOSIZE_MASK;
    rx_fifo_sz = (val >> EQOS_MAC_HW_FEATURE1_RXFIFOSIZE_SHIFT) &
        EQOS_MAC_HW_FEATURE1_RXFIFOSIZE_MASK;

    /*
     * r/tx_fifo_sz is encoded as log2(n / 128). Undo that by shifting.
     * r/tqs is encoded as (n / 256) - 1.
     */
    tqs = (128 << tx_fifo_sz) / 256 - 1;
    rqs = (128 << rx_fifo_sz) / 256 - 1;

    clrsetbits_le32(&eqos->mtl_regs->txq0_operation_mode,
            EQOS_MTL_TXQ0_OPERATION_MODE_TQS_MASK <<
            EQOS_MTL_TXQ0_OPERATION_MODE_TQS_SHIFT,
            tqs << EQOS_MTL_TXQ0_OPERATION_MODE_TQS_SHIFT);
    clrsetbits_le32(&eqos->mtl_regs->rxq0_operation_mode,
            EQOS_MTL_RXQ0_OPERATION_MODE_RQS_MASK <<
            EQOS_MTL_RXQ0_OPERATION_MODE_RQS_SHIFT,
            rqs << EQOS_MTL_RXQ0_OPERATION_MODE_RQS_SHIFT);

    /* Flow control used only if each channel gets 4KB or more FIFO */
    if (rqs >= ((4096 / 256) - 1)) {
        u32 rfd, rfa;

        setbits_le32(&eqos->mtl_regs->rxq0_operation_mode,
                 EQOS_MTL_RXQ0_OPERATION_MODE_EHFC);

        /*
         * Set Threshold for Activating Flow Contol space for min 2
         * frames ie, (1500 * 1) = 1500 bytes.
         *
         * Set Threshold for Deactivating Flow Contol for space of
         * min 1 frame (frame size 1500bytes) in receive fifo
         */
        if (rqs == ((4096 / 256) - 1)) {
            /*
             * This violates the above formula because of FIFO size
             * limit therefore overflow may occur inspite of this.
             */
            rfd = 0x3;  /* Full-3K */
            rfa = 0x1;  /* Full-1.5K */
        } else if (rqs == ((8192 / 256) - 1)) {
            rfd = 0x6;  /* Full-4K */
            rfa = 0xa;  /* Full-6K */
        } else if (rqs == ((16384 / 256) - 1)) {
            rfd = 0x6;  /* Full-4K */
            rfa = 0x12; /* Full-10K */
        } else {
            rfd = 0x6;  /* Full-4K */
            rfa = 0x1E; /* Full-16K */
        }

        clrsetbits_le32(&eqos->mtl_regs->rxq0_operation_mode,
                (EQOS_MTL_RXQ0_OPERATION_MODE_RFD_MASK <<
                 EQOS_MTL_RXQ0_OPERATION_MODE_RFD_SHIFT) |
                (EQOS_MTL_RXQ0_OPERATION_MODE_RFA_MASK <<
                 EQOS_MTL_RXQ0_OPERATION_MODE_RFA_SHIFT),
                (rfd <<
                 EQOS_MTL_RXQ0_OPERATION_MODE_RFD_SHIFT) |
                (rfa <<
                 EQOS_MTL_RXQ0_OPERATION_MODE_RFA_SHIFT));
    }

    /* Configure MAC */

    clrsetbits_le32(&eqos->mac_regs->rxq_ctrl0,
            EQOS_MAC_RXQ_CTRL0_RXQ0EN_MASK <<
            EQOS_MAC_RXQ_CTRL0_RXQ0EN_SHIFT,
            EQOS_MAC_RXQ_CTRL0_RXQ0EN_ENABLED_DCB <<
            EQOS_MAC_RXQ_CTRL0_RXQ0EN_SHIFT);

    /* Set TX flow control parameters */
    /* Set Pause Time */
    setbits_le32(&eqos->mac_regs->q0_tx_flow_ctrl,
             0xffff << EQOS_MAC_Q0_TX_FLOW_CTRL_PT_SHIFT);
    /* Assign priority for TX flow control */
    clrbits_le32(&eqos->mac_regs->txq_prty_map0,
             EQOS_MAC_TXQ_PRTY_MAP0_PSTQ0_MASK <<
             EQOS_MAC_TXQ_PRTY_MAP0_PSTQ0_SHIFT);
    /* Assign priority for RX flow control */
    clrbits_le32(&eqos->mac_regs->rxq_ctrl2,
             EQOS_MAC_RXQ_CTRL2_PSRQ0_MASK <<
             EQOS_MAC_RXQ_CTRL2_PSRQ0_SHIFT);
    /* Enable flow control */
#ifdef CONFIG_SSTAR_GMAC_DISABLE_TX_FCTL
    clrbits_le32(&eqos->mac_regs->q0_tx_flow_ctrl,
             EQOS_MAC_Q0_TX_FLOW_CTRL_TFE);
#else
    setbits_le32(&eqos->mac_regs->q0_tx_flow_ctrl,
             EQOS_MAC_Q0_TX_FLOW_CTRL_TFE);
#endif
    setbits_le32(&eqos->mac_regs->rx_flow_ctrl,
             EQOS_MAC_RX_FLOW_CTRL_RFE);

    clrsetbits_le32(&eqos->mac_regs->configuration,
            EQOS_MAC_CONFIGURATION_GPSLCE |
            EQOS_MAC_CONFIGURATION_WD |
            EQOS_MAC_CONFIGURATION_JD |
            EQOS_MAC_CONFIGURATION_JE,
            EQOS_MAC_CONFIGURATION_CST |
            EQOS_MAC_CONFIGURATION_ACS);

    eqos_write_hwaddr(dev);

    /* Configure DMA */

    /* Enable OSP mode */
    setbits_le32(&eqos->dma_regs->ch0_tx_control,
             EQOS_DMA_CH0_TX_CONTROL_OSP);

    /* RX buffer size. Must be a multiple of bus width */
    clrsetbits_le32(&eqos->dma_regs->ch0_rx_control,
            EQOS_DMA_CH0_RX_CONTROL_RBSZ_MASK <<
            EQOS_DMA_CH0_RX_CONTROL_RBSZ_SHIFT,
            EQOS_MAX_PACKET_SIZE <<
            EQOS_DMA_CH0_RX_CONTROL_RBSZ_SHIFT);

	desc_pad = (eqos->desc_size - sizeof(struct eqos_desc)) / 16;

	setbits_le32(&eqos->dma_regs->ch0_control,
		     EQOS_DMA_CH0_CONTROL_PBLX8 |
		     (desc_pad << EQOS_DMA_CH0_CONTROL_DSL_SHIFT));

	/*
	 * Burst length must be < 1/2 FIFO size.
	 * FIFO size in tqs is encoded as (n / 256) - 1.
	 * Each burst is n * 8 (PBLX8) * 16 (AXI width) == 128 bytes.
	 * Half of n * 256 is n * 128, so pbl == tqs, modulo the -1.
	 */
	pbl = tqs + 1;
	if (pbl > 32)
		pbl = 32;
	clrsetbits_le32(&eqos->dma_regs->ch0_tx_control,
			EQOS_DMA_CH0_TX_CONTROL_TXPBL_MASK <<
			EQOS_DMA_CH0_TX_CONTROL_TXPBL_SHIFT,
			pbl << EQOS_DMA_CH0_TX_CONTROL_TXPBL_SHIFT);

    clrsetbits_le32(&eqos->dma_regs->ch0_rx_control,
            EQOS_DMA_CH0_RX_CONTROL_RXPBL_MASK <<
            EQOS_DMA_CH0_RX_CONTROL_RXPBL_SHIFT,
            8 << EQOS_DMA_CH0_RX_CONTROL_RXPBL_SHIFT);

    /* DMA performance configuration */
    val = (2 << EQOS_DMA_SYSBUS_MODE_RD_OSR_LMT_SHIFT) |
        EQOS_DMA_SYSBUS_MODE_EAME | EQOS_DMA_SYSBUS_MODE_BLEN16 |
        EQOS_DMA_SYSBUS_MODE_BLEN8 | EQOS_DMA_SYSBUS_MODE_BLEN4;
    writel(val, &eqos->dma_regs->sysbus_mode);

    /* Set up descriptors */

	memset(eqos->descs, 0, eqos->desc_size * EQOS_DESCRIPTORS_NUM);

	for (i = 0; i < EQOS_DESCRIPTORS_TX; i++) {
		struct eqos_desc *tx_desc = (struct eqos_desc *)((int)eqos->tx_descs + (eqos->desc_size * i));
		eqos_flush_desc(tx_desc);
	}

	for (i = 0; i < EQOS_DESCRIPTORS_RX; i++) {
        struct eqos_desc *rx_desc = (struct eqos_desc *)((int)eqos->rx_descs + (eqos->desc_size * i));
        rx_desc->des0 = PA2BUS((u32)(ulong)(eqos->rx_dma_buf +
                         (i * EQOS_MAX_PACKET_SIZE)));
        rx_desc->des3 |= EQOS_DESC3_OWN | EQOS_DESC3_BUF1V;
		mb();
		eqos_flush_desc(rx_desc);
	}
	//Chip_Flush_Memory();


    writel(0, &eqos->dma_regs->ch0_txdesc_list_haddress);
    writel(PA2BUS((ulong)eqos->tx_descs), &eqos->dma_regs->ch0_txdesc_list_address);
    writel(EQOS_DESCRIPTORS_TX - 1,
           &eqos->dma_regs->ch0_txdesc_ring_length);

    writel(0, &eqos->dma_regs->ch0_rxdesc_list_haddress);
    writel(PA2BUS((ulong)eqos->rx_descs), &eqos->dma_regs->ch0_rxdesc_list_address);

    writel(EQOS_DESCRIPTORS_RX - 1,
           &eqos->dma_regs->ch0_rxdesc_ring_length);

    /* Enable everything */

    setbits_le32(&eqos->mac_regs->configuration,
             EQOS_MAC_CONFIGURATION_TE | EQOS_MAC_CONFIGURATION_RE);

    setbits_le32(&eqos->dma_regs->ch0_tx_control,
             EQOS_DMA_CH0_TX_CONTROL_ST);
    setbits_le32(&eqos->dma_regs->ch0_rx_control,
             EQOS_DMA_CH0_RX_CONTROL_SR);

    /* TX tail pointer not written until we need to TX a packet */
    /*
     * Point RX tail pointer at last descriptor. Ideally, we'd point at the
     * first descriptor, implying all descriptors were available. However,
     * that's not distinguishable from none of the descriptors being
     * available.
     */
    last_rx_desc = (ulong)&(eqos->rx_descs[(EQOS_DESCRIPTORS_RX - 1)]);
    writel(PA2BUS(last_rx_desc), &eqos->dma_regs->ch0_rxdesc_tail_pointer);

    eqos->started = true;
#if DYN_PHASE_CALB
    if(NEED_CALB)
        sstar_gmac_do_dyncalibrat(dev);
#endif
    debug("%s: OK\n", __func__);
    return 0;

err_shutdown_phy:
//    phy_shutdown(eqos->phy);
    eqos->phy = NULL;
err_stop_resets:
    error("FAILED: %d", ret);
    return ret;
}

void eqos_stop(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    int i;

    debug("%s(dev=%p):\n", __func__, dev);

    if (!eqos->started)
        return;
    eqos->started = false;
    eqos->reg_access_ok = false;

    /* Disable TX DMA */
    clrbits_le32(&eqos->dma_regs->ch0_tx_control,
             EQOS_DMA_CH0_TX_CONTROL_ST);

    /* Wait for TX all packets to drain out of MTL */
    for (i = 0; i < 1000000; i++) {
        u32 val = readl(&eqos->mtl_regs->txq0_debug);

        u32 trcsts = (val >> EQOS_MTL_TXQ0_DEBUG_TRCSTS_SHIFT) &
            EQOS_MTL_TXQ0_DEBUG_TRCSTS_MASK;
        u32 txqsts = val & EQOS_MTL_TXQ0_DEBUG_TXQSTS;
        if ((trcsts != 1) && (!txqsts))
            break;
    }

    /* Turn off MAC TX and RX */
    clrbits_le32(&eqos->mac_regs->configuration,
             EQOS_MAC_CONFIGURATION_TE | EQOS_MAC_CONFIGURATION_RE);

    /* Wait for all RX packets to drain out of MTL */
    for (i = 0; i < 1000000; i++) {
        u32 val = readl(&eqos->mtl_regs->rxq0_debug);
        u32 prxq = (val >> EQOS_MTL_RXQ0_DEBUG_PRXQ_SHIFT) &
            EQOS_MTL_RXQ0_DEBUG_PRXQ_MASK;
        u32 rxqsts = (val >> EQOS_MTL_RXQ0_DEBUG_RXQSTS_SHIFT) &
            EQOS_MTL_RXQ0_DEBUG_RXQSTS_MASK;
        if ((!prxq) && (!rxqsts))
            break;
    }

    /* Turn off RX DMA */
    clrbits_le32(&eqos->dma_regs->ch0_rx_control,
             EQOS_DMA_CH0_RX_CONTROL_SR);
/*
    if (eqos->phy) {
        phy_shutdown(eqos->phy);
        eqos->phy = NULL;
    }
*/
    debug("%s: OK\n", __func__);
}

#if SSTAR_GMAC_DBG
void MDrv_GMAC_DumpMem(u32 addr, u32 len)
{
    u8 *ptr = (u8 *)addr;
    u32 i;

    printf("\n ===== Dump %lx =====\n", (long unsigned int)ptr);
    for (i=0; i<len; i++)
    {
        if ((u32)i%0x10 ==0)
            printf("%lx: ", (long unsigned int)ptr);
        if (*ptr < 0x10)
            printf("0%x ", *ptr);
        else
            printf("%x ", *ptr);
        if ((u32)i%0x10 == 0x0f)
            printf("\n");
	ptr++;
    }
    printf("\r\n");
}
#endif

int eqos_send(struct eth_device *dev, void *packet, int length)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    struct eqos_desc *tx_desc;
    int i;

    debug("%s(dev=%p, packet=%p, length=%d):\n", __func__, dev, packet,
          length);

    memcpy(eqos->tx_dma_buf, packet, length);
    eqos_flush_buffer(eqos->tx_dma_buf, length);

    tx_desc = (struct eqos_desc *)((int)eqos->tx_descs + (eqos->desc_size * eqos->tx_desc_idx));
    eqos->tx_desc_idx++;
    eqos->tx_desc_idx %= EQOS_DESCRIPTORS_TX;

#if SSTAR_GMAC_DBG
		printf("[%s][%d] \r\n",__func__,__LINE__);
		MDrv_GMAC_DumpMem((u32)eqos->tx_dma_buf ,length);
#endif

    tx_desc->des0 = PA2BUS((ulong)eqos->tx_dma_buf);
    tx_desc->des1 = 0;
    tx_desc->des2 = length;
    /*
     * Make sure that if HW sees the _OWN write below, it will see all the
     * writes to the rest of the descriptor too.
     */
    mb();
    tx_desc->des3 = EQOS_DESC3_OWN | EQOS_DESC3_FD | EQOS_DESC3_LD | length;
    eqos_flush_desc(tx_desc);

	//printf("tx desc: 0x%x. dma buf: 0x%x \r\n",(void*)tx_desc, eqos->tx_dma_buf);
    writel(PA2BUS((ulong)(tx_desc + 1)), &eqos->dma_regs->ch0_txdesc_tail_pointer);

    for (i = 0; i < 1000000; i++) {
        eqos_inval_desc(tx_desc);
        if (!(readl(&tx_desc->des3) & EQOS_DESC3_OWN)){
            return 0;
        }
        udelay(1);
    }

    debug("%s: TX timeout\n", __func__);

    return -ETIMEDOUT;
}

int eqos_recv(struct eth_device *dev, int flags, uchar **packetp)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    struct eqos_desc *rx_desc;
    u32 length;

    debug("%s(dev=%p, flags=%x):\n", __func__, dev, flags);

    rx_desc = (struct eqos_desc *)((int)eqos->rx_descs + (eqos->desc_size * eqos->rx_desc_idx));
    eqos_inval_desc(rx_desc);

//	printf("RX Start IdxNumber:%d,desc:0x%08x, des3:0x%08x\r\n",eqos->rx_desc_idx, (void*)rx_desc, rx_desc->des3 );
	
    if (rx_desc->des3 & EQOS_DESC3_OWN) {
        //debug("%s: RX packet not available\n", __func__);
        return -EAGAIN;
    }

//    *packetp = (uchar*)(eqos->rx_dma_buf + (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE));

    length = rx_desc->des3 & 0x7fff;

//    debug("%s: *packetp=%p, length=%d\n", __func__, *packetp, length);
    eqos_inval_buffer((eqos->rx_dma_buf + (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE)), length);

#if SSTAR_GMAC_DBG
    MDrv_GMAC_DumpMem((u32)(eqos->rx_dma_buf + (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE)) ,length);
#endif

	NetReceive((eqos->rx_dma_buf + (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE)),length);

	/*
	 * Make the current descriptor valid again and go to
	 * the next one
	 */
#if(0)
	//desc_p->txrx_status |= DESC_RXSTS_OWNBYDMA;
	rx_desc->des3 |= EQOS_DESC3_OWN;
#else
    mb();
    rx_desc->des0 = (u32)PA2BUS((eqos->rx_dma_buf + (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE)));
    rx_desc->des1 = 0;
    rx_desc->des2 = 0;
    rx_desc->des3 |= EQOS_DESC3_OWN | EQOS_DESC3_BUF1V;
#endif
	/* Flush only status field - others weren't changed */
	eqos_flush_desc(rx_desc);

	writel((ulong)rx_desc, &eqos->dma_regs->ch0_rxdesc_tail_pointer);

	if(++(eqos->rx_desc_idx) >= EQOS_DESCRIPTORS_RX)
		eqos->rx_desc_idx = 0;

    return length;
}

int eqos_free_pkt(struct eth_device *dev, uchar *packet, int length)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    uchar *packet_expected;
    struct eqos_desc *rx_desc;

    debug("%s(packet=%p, length=%d)\n", __func__, packet, length);

    packet_expected = eqos->rx_dma_buf +
        (eqos->rx_desc_idx * EQOS_MAX_PACKET_SIZE);
    if (packet != packet_expected) {
        debug("%s: Unexpected packet (expected %p)\n", __func__,
              packet_expected);
        return -EINVAL;
    }

    rx_desc = &(eqos->rx_descs[eqos->rx_desc_idx]);
    rx_desc->des0 = (u32)(ulong)packet;
    rx_desc->des1 = 0;
    rx_desc->des2 = 0;
    /*
     * Make sure that if HW sees the _OWN write below, it will see all the
     * writes to the rest of the descriptor too.
     */
    mb();
    rx_desc->des3 |= EQOS_DESC3_OWN | EQOS_DESC3_BUF1V;
    eqos_flush_desc(rx_desc);

    writel((ulong)rx_desc, &eqos->dma_regs->ch0_rxdesc_tail_pointer);

    eqos->rx_desc_idx++;
    eqos->rx_desc_idx %= EQOS_DESCRIPTORS_RX;

    return 0;
}

static int eqos_probe_resources_core(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    int ret;

    debug("%s(dev=%p):\n", __func__, dev);
    //dcache_enable();
	eqos->descs = eqos_alloc_descs(eqos, EQOS_DESCRIPTORS_NUM);

	
    if (!eqos->descs) {
        debug("%s: eqos_alloc_descs() failed\n", __func__);
        ret = -ENOMEM;
        goto err;
    }
    eqos->tx_descs = (struct eqos_desc *)eqos->descs;
    eqos->rx_descs = (struct eqos_desc *)((int)eqos->descs + (EQOS_DESCRIPTORS_TX * eqos->desc_size));

    eqos->tx_dma_buf = memalign(EQOS_BUFFER_ALIGN, EQOS_MAX_PACKET_SIZE);
//	eqos->tx_dma_buf = &GTxBuf;

    if (!eqos->tx_dma_buf) {
        debug("%s: memalign(tx_dma_buf) failed\n", __func__);
        ret = -ENOMEM;
        goto err_free_descs;
    }
    debug("%s: rx_dma_buf=%p\n", __func__, eqos->rx_dma_buf);

    eqos->rx_dma_buf = memalign(EQOS_BUFFER_ALIGN, EQOS_RX_BUFFER_SIZE);
//	eqos->rx_dma_buf = &GRxBuf;
    if (!eqos->rx_dma_buf) {
        debug("%s: memalign(rx_dma_buf) failed\n", __func__);
        ret = -ENOMEM;
        goto err_free_tx_dma_buf;
    }
    debug("%s: tx_dma_buf=%p\n", __func__, eqos->tx_dma_buf);

    eqos->rx_pkt = malloc(EQOS_MAX_PACKET_SIZE);
    if (!eqos->rx_pkt) {
        debug("%s: malloc(rx_pkt) failed\n", __func__);
        ret = -ENOMEM;
        goto err_free_rx_dma_buf;
    }
    debug("%s: rx_pkt=%p\n", __func__, eqos->rx_pkt);

    debug("%s: OK\n", __func__);
    return 0;

err_free_rx_dma_buf:
    free(eqos->rx_dma_buf);
err_free_tx_dma_buf:
    free(eqos->tx_dma_buf);
err_free_descs:
    eqos_free_descs(eqos->descs);
err:

    debug("%s: returns %d\n", __func__, ret);
    return ret;
}

#if(0)
static int eqos_remove_resources_core(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    free(eqos->rx_pkt);
    free(eqos->rx_dma_buf);
    free(eqos->tx_dma_buf);
    eqos_free_descs(eqos->descs);

    debug("%s: OK\n", __func__);
    return 0;
}
#endif

int eqos_probe(struct eth_device *dev, bd_t * bis)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;
    int ret;

    debug("%s(dev=%p):\n", __func__, dev);

	sstar_gmac_probe(dev, eqos->phy->interface ,bis);

    eqos->dev = dev;
//    eqos->config = (void *)dev_get_driver_data(dev);

    eqos->regs = dev->iobase;


	//eqos->dma_regs = (void*)(eqos->regs + EQOS_DMA_REGS_BASE);
	//printf("[%s] dma reg: %x, %p, %x \r\n",__func__,(void**)eqos->dma_regs,(void**)eqos->dma_regs,(eqos->regs + EQOS_DMA_REGS_BASE));

    ret = eqos_probe_resources_core(dev);
    if (ret < 0) {
        error("eqos_probe_resources_core() failed: %d", ret);
        return ret;
    }

#if 0 //NOPHYLIB
    eqos->mii = mdio_alloc();
	printf("[%s][%d] mii: %x, %x \r\n",__func__,__LINE__,&eqos->mii,eqos->mii);
    if (!eqos->mii) {
        error("mdio_alloc() failed");
        goto err_remove_resources_tegra;
    }
    eqos->mii->read = eqos_mdio_read;
    eqos->mii->write = eqos_mdio_write;
    eqos->mii->priv = eqos;
    strcpy(eqos->mii->name, dev->name);

    ret = mdio_register(eqos->mii);
    if (ret < 0) {
        error("mdio_register() failed: %d", ret);
        goto err_free_mdio;
    }
#else

	eqos->mii = &Gmii;
	eqos->mii->priv = eqos;
	strcpy(eqos->mii->name, dev->name);

#endif

    debug("%s: OK\n", __func__);

	ret = eqos_start(dev);
	if(ret <0)
		return ret;

	gmac_init=1;

    return 0;
#if(0)
err_free_mdio:
    free(eqos->mii);
    eqos_remove_resources_core(dev);

    debug("%s: returns %d\n", __func__, ret);
    return ret;
#endif
}
/*
static int eqos_remove(struct eth_device *dev)
{
    struct eqos_priv *eqos = (struct eqos_priv*) dev->priv;

    debug("%s(dev=%p):\n", __func__, dev);

    mdio_unregister(eqos->mii);
    mdio_free(eqos->mii);
    eqos_remove_resources_tegra186(dev);
    eqos_probe_resources_core(dev);

    debug("%s: OK\n", __func__);
    return 0;
}
*/
/*
static const struct eth_ops eqos_ops = {
    .start = eqos_start,
    .stop = eqos_stop,
    .send = eqos_send,
    .recv = eqos_recv,
    .free_pkt = eqos_free_pkt,
    .write_hwaddr = eqos_write_hwaddr,
};

static const struct eqos_config eqos_tegra186_config = {
    .reg_access_always_ok = false,
};

static const struct udevice_id eqos_ids[] = {
    {
        .compatible = "nvidia,tegra186-eqos",
        .data = (ulong)&eqos_tegra186_config
    },
    { }
};

U_BOOT_DRIVER(eth_eqos) = {
    .name = "eth_eqos",
    .id = UCLASS_ETH,
    .of_match = eqos_ids,
    .probe = eqos_probe,
    .remove = eqos_remove,
    .ops = &eqos_ops,
    .priv_auto_alloc_size = sizeof(struct eqos_priv),
    .platdata_auto_alloc_size = sizeof(struct eth_pdata),
};
*/

