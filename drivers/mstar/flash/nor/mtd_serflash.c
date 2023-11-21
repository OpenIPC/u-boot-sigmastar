/*
 * mtd_serflash.c- Sigmastar
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
#include <flash.h>
#include <malloc.h>
#include <asm/errno.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/concat.h>
#include <spi_flash.h>
#include <drvSPINOR.h>
#include <linux/err.h>
#include "asm/arch/mach/platform.h"
#include <mdrvParts.h>

static struct spi_flash *serflash = NULL;
static u32 g_u32_sni_address = 0;
static u32 g_u32_pni_address = 0;
static u32 g_u32_flash_rw_buff = 0;
unsigned int g_size=0;

#ifdef CONFIG_MS_MTD_ISP_FLASH
unsigned long flash_init (void)
{
    unsigned int bus = CONFIG_SF_DEFAULT_BUS;
    unsigned int cs = CONFIG_SF_DEFAULT_CS;
    unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
    unsigned int mode = CONFIG_SF_DEFAULT_MODE;

    spi_flash_probe(bus, cs, speed, mode);
    if (g_size>0)
        return (g_size);

    return 0;
}
#endif // CONFIG_MS_MTD_ISP_FLASH

#ifdef CONFIG_CMD_SF
#include <spi_flash.h>
int _spi_flash_read(struct spi_flash *flash, u32 offset,
		size_t len, void *buf)
{
    u8 u8_status = 0;
    u32 u32_read_size;

    /* sanity checks */
    if (!len)
        return -EINVAL;

    if (offset + len > flash->size)
        return -EINVAL;

    if ((u32)buf % ARCH_DMA_MINALIGN)
    {
        u32_read_size = ARCH_DMA_MINALIGN - ((u32)buf % ARCH_DMA_MINALIGN);

        if (ERR_SPINOR_SUCCESS != (u8_status = mdrv_spinor_read(offset, (u8*)g_u32_flash_rw_buff, u32_read_size)))
        {
            printk(KERN_WARNING "err 0x%x\r\n", u8_status);
            return (-EIO);
        }

        memcpy((void *)buf, (const void *)g_u32_flash_rw_buff, u32_read_size);

        buf += u32_read_size;
        offset += u32_read_size;
        len -= u32_read_size;
    }

    u32_read_size = (len % ARCH_DMA_MINALIGN) ? (len - (len % ARCH_DMA_MINALIGN)) : len;

    if (ERR_SPINOR_SUCCESS != (u8_status = mdrv_spinor_read(offset, (u8*)buf, u32_read_size)))
    {
        printk(KERN_WARNING "err 0x%x\r\n", u8_status);
        return (-EIO);
    }

    buf += u32_read_size;
    offset += u32_read_size;
    len -= u32_read_size;

    if (len)
    {
        u32_read_size = len;

        if (ERR_SPINOR_SUCCESS != (u8_status = mdrv_spinor_read(offset, (u8*)g_u32_flash_rw_buff, u32_read_size)))
        {
            printk(KERN_WARNING "err 0x%x\r\n", u8_status);
            return (-EIO);
        }

        memcpy((void *)buf, (const void *)g_u32_flash_rw_buff, u32_read_size);
    }

    return 0;
}

int _spi_flash_write(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
    printk(KERN_WARNING "%s to 0x%x, len 0x%x from 0x%x \r\n", __func__,(u32)offset, len, (unsigned int)buf);
    u32 starttime = get_timer(0);
    u16 u16_write_size = 0;
    u8 *pu8_data = (u8 *)buf;
    u8 *pu8_write_data;
    u32 total;
    u32 percent = 0;
    u32 pre_percent = 0;
    u32 u32_page_size = flash->page_size;
    u32 u32_page_size_mask = u32_page_size - 1;

    /* sanity checks */
    if (!len)
        return (-EINVAL);
    if (offset + len > flash->size)
        return (-EINVAL);

    total = len;
    while (0 != len)
    {
        u16_write_size = u32_page_size - (u32_page_size_mask & offset);
        if (u16_write_size > len)
        {
            u16_write_size = len;
        }

        if (((u32)pu8_data % ARCH_DMA_MINALIGN) || u16_write_size % ARCH_DMA_MINALIGN)
        {
            memcpy((void *)g_u32_flash_rw_buff, (const void *)pu8_data, u16_write_size);
            pu8_write_data = (u8 *)g_u32_flash_rw_buff;
        }
        else
        {
            pu8_write_data = pu8_data;
        }

        if (ERR_SPINOR_SUCCESS != mdrv_spinor_program(offset, pu8_write_data, u16_write_size))
        {
            printk("[FLASH_ERR] Program page fail\r\n");
            return (-EIO);
        }

        offset += u16_write_size;
        pu8_data += u16_write_size;
        len -= u16_write_size;

        percent = (total - len) * 100 / total;
        if(((percent - pre_percent) >= 5) || (percent == 100))
        {
            printk(KERN_WARNING "\rWriting at 0x%x -- %3d%% complete.", offset, percent);
            pre_percent = percent;
        }
    }

    printk(KERN_WARNING "(cost %ld ms)\n", get_timer(0) - starttime);
    return 0;
}

int _spi_flash_erase(struct spi_flash *flash, u32 offset,
		size_t len)
{
    printk(KERN_WARNING"%s: addr 0x%x, len 0x%x \r\n", __func__, (u32)offset, len);

    u32 u32_bytes_left = 0;
    u32 starttime = get_timer(0);
    u32 percent = 0;
    u32 pre_percent = 0;
    u32 u32_erase_size;
    u32 u32_sector_size;
    u32 u32_block_size;
    FLASH_NOR_INFO_t st_flash_nor_info;

    mdrv_spinor_info(&st_flash_nor_info);

    u32_sector_size = st_flash_nor_info.u32_sectorSize;
    u32_block_size = st_flash_nor_info.u32_blockSize;
    /* range and alignment check */
    if (!len)
        return (-EINVAL);
    if (offset + len > flash->size || len % u32_sector_size || offset % u32_sector_size)
        return (-EINVAL);

    u32_bytes_left = len;

    while (0 != u32_bytes_left)
    {
        if ((0 != (~(u32_block_size - 1) & u32_bytes_left)) && (0 == ((u32_block_size - 1) & offset)))
        {
            if (ERR_SPINOR_SUCCESS != mdrv_spinor_erase(offset, u32_block_size))
            {
                printk(KERN_WARNING "block erase failed!\n");
                return (-EIO);
            }
            u32_erase_size = u32_block_size;
        }
        else
        {
            if (ERR_SPINOR_SUCCESS != mdrv_spinor_erase(offset, u32_sector_size))
            {
                printk(KERN_WARNING "sector erase failed!\n");
                return (-EIO);
            }
            u32_erase_size = u32_sector_size;
        }

        offset += u32_erase_size;
        u32_bytes_left -= u32_erase_size;

        percent = (len - u32_bytes_left) * 100 / len;
        if(((percent - pre_percent) >= 5) || (percent == 100))
        {
            printk(KERN_WARNING "\rErasing at 0x%x -- %3d%% complete.", offset, percent);
            pre_percent = percent;
        }

    }

    printk(KERN_WARNING "(cost %ld ms)\n", get_timer(0) - starttime);
    return 0;
}

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode)
{
    int i = 0;

    struct spi_flash *flash = NULL;
    struct mtd_info *mtd = NULL;
    FLASH_NOR_INFO_t st_flash_nor_info;

    if (serflash)
        return serflash;

    if (0 == g_u32_sni_address)
    {
        g_u32_sni_address = (u32)malloc(FLASH_SNI_TABLE_SIZE);
        g_u32_pni_address = (u32)memalign(ARCH_DMA_MINALIGN, MAX_PARTS_TBL_SIZE);

        if (!g_u32_sni_address) {
            debug("SF: Failed to allocate memory\n");
            goto err;
        }

        if (!g_u32_pni_address) {
            debug("SF: Failed to memalign\n");
            goto err;
        }

        memset((void *)g_u32_sni_address, 0, FLASH_SNI_TABLE_SIZE);
        // jedec_probe() will read id, so initialize hardware first
        if(ERR_SPINOR_SUCCESS != mdrv_spinor_hardware_init((u8 *)g_u32_sni_address))
        {
            goto err;
        }
        printk("flash init successful\r\n");

        memset((void *)g_u32_pni_address, 0, MAX_PARTS_TBL_SIZE);
        MDRV_PARTS_hardware_init_tlb((u8 *)g_u32_pni_address);
    }

    mdrv_spinor_info(&st_flash_nor_info);

    if (!st_flash_nor_info.u32_capacity)
    {
        goto err;
    }

    if (0 == g_u32_flash_rw_buff)
    {
        g_u32_flash_rw_buff = (u32)memalign(ARCH_DMA_MINALIGN, st_flash_nor_info.u32_pageSize);

        if (!g_u32_flash_rw_buff) {
            debug("SF: Failed to memalign\n");
            goto err;
        }
    }

    flash = malloc(sizeof(struct spi_flash));
    if (!flash) {
        debug("SF: Failed to allocate memory\n");
        goto err;
    }

    memset((void *)flash, 0, sizeof(struct spi_flash));
    flash->spi = NULL;
    flash->name = "nor0";
    flash->write = _spi_flash_write;
    flash->erase = _spi_flash_erase;
    flash->read = _spi_flash_read;
    flash->size = st_flash_nor_info.u32_capacity;
    flash->page_size = st_flash_nor_info.u32_pageSize;
    flash->sector_size = st_flash_nor_info.u32_sectorSize;
    flash->erase_size = st_flash_nor_info.u32_sectorSize;

    if(g_u32_sni_address)
    {
        flash->mtd.priv = flash;
        flash->mtd.name ="nor0";
        flash->mtd.type = MTD_NORFLASH;
        flash->mtd.writesize = 1;
        flash->mtd.flags = MTD_CAP_NORFLASH;
        flash->mtd.size = flash->size;
        g_size = flash->mtd.size;
        flash->mtd.erasesize = flash->erase_size ;
        flash->mtd.numeraseregions = 0;
        printk(
               "mtd .name = %s, .size = 0x%.8x (%uMiB) "
               ".erasesize = 0x%.8x .numeraseregions = %d\n",
               flash->mtd.name,
               (unsigned int)flash->mtd.size, (unsigned int)(flash->mtd.size >>20),
               (unsigned int)flash->mtd.erasesize,
               flash->mtd.numeraseregions);

        if (flash->mtd.numeraseregions)
            for (i = 0; i < flash->mtd.numeraseregions; i++)
                printk(
                       "mtd.eraseregions[%d] = { .offset = 0x%.8x, "
                       ".erasesize = 0x%.8x (%uKiB), "
                       ".numblocks = %d }\n",
                       i, (u32)flash->mtd.eraseregions[i].offset,
                       (unsigned int)flash->mtd.eraseregions[i].erasesize,
                       (flash->mtd.eraseregions[i].erasesize >>10 ),
                       (unsigned int)flash->mtd.eraseregions[i].numblocks);

        if (!add_mtd_device(&flash->mtd))
        {
            printk("SF: add mtd device successfully\n");
        }
    }
    if (!flash) {
        printk("SF: Unsupported manufacturer\n");
        goto err;
    }

    printk("SF: Detected %s with total size ", flash->name);
    print_size(flash->size, "\n");

    serflash = flash;

    return flash;

err:
    return NULL;
}

struct spi_flash* spi_flash_probe_driver(u32 u32_address, unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode)
{
    if (!g_u32_sni_address)
        return NULL;

    if (ERR_SPINOR_SUCCESS != mdrv_spinor_find_sni_form_dram((u8 *)g_u32_sni_address, (u8 *)u32_address))
        return NULL;

    printf("spi_flash_probe_driver\r\n");

    if (ERR_SPINOR_SUCCESS !=  mdrv_spinor_hardware_init((u8 *)g_u32_sni_address))
        return NULL;

    if (serflash)
    {
        del_mtd_device(&serflash->mtd);
        free((void *)serflash);
        serflash = NULL;
    }

    return spi_flash_probe(bus, cs, max_hz, spi_mode);
}

void spi_flash_free(struct spi_flash *flash)
{
}

#endif
