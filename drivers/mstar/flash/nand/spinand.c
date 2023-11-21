/*
 * spinand.c- Sigmastar
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
#include <malloc.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <asm/io.h>
#include <asm/errno.h>
#include <drvSPINAND.h>
#include <nand.h>
#include <mdrvParts.h>
#include <mdrvSpinandBbtBbm.h>

typedef struct
{
    u32 u32_command_ctrl;
    u32 u32_r_ptr;
    u32 u32_w_ptr;
    u32 u32_buf_size;
} COMMAND_OPS;

static u32 g_u32_sni_address = 0;
static u32 g_u32_pni_address = 0;
static u32 g_u32_bbt_address = 0;
static u32 g_u32_flash_rw_buff = 0;
static u32 g_u32_socecc_cache = 0;
static COMMAND_OPS g_command_ops = {0, 0, 0};

#if 1
#define spi_nand_msg(fmt, ...) printk(KERN_NOTICE "%s: " fmt "\n", __func__, ##__VA_ARGS__)
#define spi_nand_debug(fmt, ...)
#else
#define spi_nand_msg(fmt, ...)
#define spi_nand_debug(fmt, ...) printk(KERN_NOTICE "%s: " fmt "\n", __func__, ##__VA_ARGS__)
#endif

static struct nand_ecclayout nand_oob_256 = {
    .eccbytes = 48,
    .eccpos = {
        80, 81, 82, 83, 84, 85, 86, 87,
        88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103,
        104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119,
        120, 121, 122, 123, 124, 125, 126, 127},
    .oobfree = {
        {.offset = 2,
        .length = 78},
        {.offset = 128,
        .length = 128}}
};

static uint8_t spi_nand_read_byte(struct mtd_info *mtd)
{
    u8 u8_byte;
    u8 *pu8_data;

    pu8_data = (u8 *)g_u32_flash_rw_buff;

    u8_byte = pu8_data[g_command_ops.u32_r_ptr];

    g_command_ops.u32_r_ptr += 1;

    if (g_command_ops.u32_r_ptr == g_command_ops.u32_w_ptr)
        g_command_ops.u32_r_ptr = 0;

    return u8_byte;
}

static u16 spi_nand_read_word(struct mtd_info *mtd)
{
    u16 u16_word;
    u8 *pu8_data;

    pu8_data = (u8 *)g_u32_flash_rw_buff;

    u16_word = pu8_data[g_command_ops.u32_r_ptr];

    g_command_ops.u32_r_ptr += 1;

    if (g_command_ops.u32_r_ptr == g_command_ops.u32_w_ptr)
        g_command_ops.u32_r_ptr = 0;

    u16_word |= (pu8_data[g_command_ops.u32_r_ptr] << 8);

    g_command_ops.u32_r_ptr += 1;

    if (g_command_ops.u32_r_ptr == g_command_ops.u32_w_ptr)
        g_command_ops.u32_r_ptr = 0;

    return u16_word;
}

static void spi_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    u32 u32_read_byte;

    while (len != 0)
    {
        u32_read_byte = (g_command_ops.u32_w_ptr - g_command_ops.u32_r_ptr);
        u32_read_byte = (u32_read_byte > len) ? len : u32_read_byte;

        memcpy((void *)buf, (const void *)(g_u32_flash_rw_buff + g_command_ops.u32_r_ptr), u32_read_byte);

        len -= u32_read_byte;
        buf += u32_read_byte;
        g_command_ops.u32_r_ptr += u32_read_byte;

        if (g_command_ops.u32_r_ptr == g_command_ops.u32_w_ptr)
            g_command_ops.u32_r_ptr = 0;
    }
}

static void spi_nand_select_chip(struct mtd_info *mtd, int chip)
{
    //printf("spi_nand_select_chip  Not support\r\n");
}

static void spi_nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    spi_nand_debug("spi_nand_cmd_ctrl Not support\r\n");
}

static int spi_nand_dev_ready(struct mtd_info *mtd)
{
    spi_nand_debug("spi_nand_dev_ready Not support\r\n");

    return 1;
}

static void spi_nand_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
    struct nand_chip *chip;
    chip = (struct nand_chip*)mtd->priv;

    g_command_ops.u32_r_ptr = 0;

    switch (command)
    {
        case NAND_CMD_STATUS:
            spi_nand_debug("NAND_CMD_STATUS");
            g_command_ops.u32_command_ctrl = NAND_CMD_STATUS;
            g_command_ops.u32_w_ptr = 1;
            mdrv_spinand_read_status((u8 *)g_u32_flash_rw_buff);
            break;

        case NAND_CMD_PAGEPROG:
            spi_nand_debug("NAND_CMD_PAGEPROG");
            chip->pagebuf = -1;
            break;

        case NAND_CMD_READOOB:
            spi_nand_debug("NAND_CMD_READOOB page_addr: 0x%x", page_addr);
            g_command_ops.u32_command_ctrl = NAND_CMD_READOOB;
            g_command_ops.u32_w_ptr = 1;
            mdrv_spinand_page_read(page_addr, (u16)(column + mtd->writesize), (u8 *)g_u32_flash_rw_buff, 1);
            break;

        case NAND_CMD_READID:
            spi_nand_debug("NAND_CMD_READID\r\n");
            g_command_ops.u32_command_ctrl = NAND_CMD_READID;
            g_command_ops.u32_w_ptr = 6;
            mdrv_spinand_read_id((u8 *)g_u32_flash_rw_buff, 6);
            break;

        case NAND_CMD_ERASE2:
            spi_nand_debug("NAND_CMD_ERASE2\r\n");
            break;

        case NAND_CMD_ERASE1:
            spi_nand_debug("NAND_CMD_ERASE1, page_addr: 0x%x", page_addr);
            mdrv_spinand_block_erase(page_addr);
            break;
        case NAND_CMD_READ0:
            break;
        case NAND_CMD_SEQIN:
            spi_nand_debug("NAND_CMD_SEQIN");
            chip->pagebuf = page_addr;
            break;
        case NAND_CMD_RESET:
            spi_nand_debug("NAND_CMD_RESET");
            mdrv_spinand_reset();
            break;
        case NAND_CMD_OTP_ENABLE:
            spi_nand_debug("NAND_CMD_OTP_ENABLE");
            mdrv_spinand_set_otp_mode(column);
            break;
        case NAND_CMD_OTP_LOCK:
            spi_nand_debug("NAND_CMD_OTP_LOCK");
            mdrv_spinand_set_otp_mode(column);
            break;
        case NAND_CMD_DO_ECC:
            spi_nand_debug("NAND_CMD_DO_ECC");
            mdrv_spinand_set_ecc_mode(column);
            break;
        default:
            spi_nand_msg("unsupported command %02Xh\n", command);
            break;
    }
    return;
}

static int spi_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
    u8 u8_status;

    spi_nand_debug("spi_nand_waitfunc\r\n");

    u8_status = mdrv_spinand_read_status(NULL);

    return (ERR_SPINAND_E_FAIL > u8_status)? NAND_STATUS_READY : NAND_STATUS_FAIL;
}

static void spi_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
    spi_nand_debug(" spi_nand_ecc_hwctl Not support");
}

static int spi_nand_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
    spi_nand_debug("spi_nand_ecc_calculate Not support");
    return 0;
}

static int spi_nand_ecc_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
    spi_nand_debug(" spi_nand_ecc_correct Not support");
    return 0;
}

static int spi_nand_ecc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
    uint8_t *buf, int oob_required, int page)
{
    u8 *pu8_data;
    u32 u32_read_size;

    pu8_data = (u8 *)(oob_required ? (u8 *)g_u32_flash_rw_buff : buf);
    u32_read_size = oob_required ? (mtd->writesize + mtd->oobsize) : mtd->writesize;

    if (ERR_SPINAND_TIMEOUT <= mdrv_spinand_page_read(page, 0, pu8_data, u32_read_size))
    {
        return -EIO;
    }

    if (oob_required)
    {
        memcpy((void *)buf, (const void *)pu8_data, mtd->writesize);
        memcpy((void *)(chip->oob_poi), (const void *)(pu8_data + mtd->writesize), mtd->oobsize);
    }

    return 0;
}

static int spi_nand_ecc_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required)
{
    u8 *pu8_data;
    u32 u32_write_size;

    spi_nand_debug("spi_nand_ecc_write_page_raw\r\n");

    pu8_data = (u8 *)(oob_required ? (u8 *)g_u32_flash_rw_buff : buf);
    u32_write_size = oob_required ? (mtd->writesize + mtd->oobsize) : mtd->writesize;

    if (oob_required)
    {
        memcpy((void *)g_u32_flash_rw_buff, (const void *)buf, mtd->writesize);
        memcpy((void *)(g_u32_flash_rw_buff + mtd->writesize), (const void *)(chip->oob_poi), mtd->oobsize);
    }

    if (ERR_SPINAND_SUCCESS != mdrv_spinand_page_program(chip->pagebuf, 0, pu8_data, u32_write_size))
    {
        return -EIO;
    }

    return 0;
}

static int spi_nand_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
    uint8_t *buf, int oob_required, int page)
{
    u8 u8_status;
    u8 *pu8_data;
    u32 u32_read_size;

    pu8_data = (u8 *)(oob_required ? (u8 *)g_u32_flash_rw_buff : buf);
    u32_read_size = oob_required ? (mtd->writesize + mtd->oobsize) : mtd->writesize;

    u8_status = mdrv_spinand_page_read(page, 0, pu8_data, u32_read_size);

    if (oob_required)
    {
        memcpy((void *)buf, (const void *)pu8_data, mtd->writesize);
        memcpy((void *)(chip->oob_poi), (const void *)(pu8_data + mtd->writesize), mtd->oobsize);
    }

    if (ERR_SPINAND_ECC_NOT_CORRECTED == u8_status)
        mtd->ecc_stats.failed++;
    else if (ERR_SPINAND_ECC_CORRECTED == u8_status)
        mtd->ecc_stats.corrected++;
    else if (ERR_SPINAND_TIMEOUT <= u8_status)
        return -EIO;

    return 0;
}

static int spi_nand_ecc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip,
    uint32_t offs, uint32_t len, uint8_t *buf, int page)
{
    u8 u8_status;
    u8 *pu8_data;
    u32 u32_read_size;

    spi_nand_debug("page = 0x%x, offs = 0x%x, len = 0x%x", page ,offs, len);

    pu8_data = buf + offs;
    u32_read_size = len;

    if (len % ARCH_DMA_MINALIGN)
    {
        pu8_data = (u8 *)g_u32_flash_rw_buff;
        u32_read_size = len + ARCH_DMA_MINALIGN;
        u32_read_size &= ~(ARCH_DMA_MINALIGN - 1);
    }

    u8_status = mdrv_spinand_page_read(page, (u16)offs, pu8_data, u32_read_size);

    if (len % ARCH_DMA_MINALIGN)
    {
        memcpy((void *)(buf + offs), (const void *)(pu8_data), len);
    }

    if (ERR_SPINAND_ECC_NOT_CORRECTED == u8_status)
        mtd->ecc_stats.failed++;
    else if (ERR_SPINAND_ECC_CORRECTED == u8_status)
        mtd->ecc_stats.corrected++;
    else if (ERR_SPINAND_TIMEOUT <= u8_status)
        return -EIO;

    return 0;
}

static int spi_nand_ecc_write_subpage(struct mtd_info *mtd, struct nand_chip *chip,
                               uint32_t offset, uint32_t data_len,
                               const uint8_t *data_buf, int oob_required)
{
    u8 *pu8_data;
    u32 u32_write_size;

    spi_nand_debug("spi_nand_ecc_write_subpage\r\n");

    pu8_data = (u8 *)(oob_required ? (u8 *)g_u32_flash_rw_buff : data_buf);
    u32_write_size = oob_required ? (data_len + mtd->oobsize) : data_len;

    if (u32_write_size % ARCH_DMA_MINALIGN)
    {
        pu8_data = (u8 *)g_u32_flash_rw_buff;
        u32_write_size = u32_write_size + ARCH_DMA_MINALIGN;
        u32_write_size &= ~(ARCH_DMA_MINALIGN - 1);
    }

    if (oob_required || u32_write_size % ARCH_DMA_MINALIGN)
    {
        memcpy((void *)g_u32_flash_rw_buff, (const void *)data_buf, data_len);
        if (oob_required)
            memcpy((void *)(g_u32_flash_rw_buff + data_len), (const void *)(chip->oob_poi), mtd->oobsize);
    }

    if (ERR_SPINAND_SUCCESS != mdrv_spinand_page_program(chip->pagebuf, (u16)offset, pu8_data, u32_write_size))
    {
        return -EIO;
    }

    return 0;
}

static int spi_nand_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required)
{
    u8 *pu8_data;
    u32 u32_write_size;

    spi_nand_debug("spi_nand_ecc_write_page\r\n");

    pu8_data = (u8 *)(oob_required ? (u8 *)g_u32_flash_rw_buff : buf);
    u32_write_size = oob_required ? (mtd->writesize + mtd->oobsize) : mtd->writesize;

    if (oob_required)
    {
        memcpy((void *)g_u32_flash_rw_buff, (const void *)buf, mtd->writesize);
        memcpy((void *)(g_u32_flash_rw_buff + mtd->writesize), (const void *)(chip->oob_poi), mtd->oobsize);
    }

    if (ERR_SPINAND_SUCCESS != mdrv_spinand_page_program(chip->pagebuf, 0, pu8_data, u32_write_size))
    {
        return -EIO;
    }

    return 0;
}

static int spi_nand_read_oob_std(struct mtd_info *mtd, struct nand_chip *chip,int page)
{
    if (ERR_SPINAND_TIMEOUT <= mdrv_spinand_page_read(page, (u16)(mtd->writesize), chip->oob_poi, mtd->oobsize))
    {
        return -EIO;
    }

    return 0;
}

static int spi_nand_write_oob_std(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
    if (ERR_SPINAND_SUCCESS != mdrv_spinand_page_program(page, (u16)(mtd->writesize), chip->oob_poi, mtd->oobsize))
    {
        return -EIO;
    }

        return 0;
}

int board_nand_init(struct nand_chip *nand)
{
    u8 u8_dev_id;
    u32 u32_flash_size;
    struct nand_flash_dev *type;
    FLASH_NAND_INFO_t st_flash_nand_info;

    do
    {
        if (0 == g_u32_sni_address)
        {
            if (0 == (g_u32_sni_address = (u32)memalign(ARCH_DMA_MINALIGN, FLASH_SNI_TABLE_SIZE)))
            {
                break;
            }
        }

        if (0 == g_u32_pni_address)
        {
            if (0 == (g_u32_pni_address = (u32)memalign(ARCH_DMA_MINALIGN, MAX_PARTS_TBL_SIZE)))
            {
                break;
            }
        }

        if (0 == g_u32_bbt_address)
        {
            if (0 == (g_u32_bbt_address = (u32)memalign(ARCH_DMA_MINALIGN, MAX_BBT_TBL_SIZE)))
            {
                break;
            }
        }

        if (mdrv_spinand_is_socecc() && (0 == g_u32_socecc_cache))
        {
            if (0 == (g_u32_socecc_cache = (u32)memalign(ARCH_DMA_MINALIGN, FLASH_SOC_ECC_CACHE_SIZE)))
            {
                break;
            }

            mdrv_spinand_socecc_init(NULL, (u8 *)g_u32_socecc_cache);
        }

        if (ERR_SPINAND_SUCCESS != mdrv_spinand_hardware_init((u8 *)g_u32_sni_address))
            break;

        MDRV_PARTS_hardware_init_tlb((u8 *)g_u32_pni_address);

        MDRV_BBT_init((u8 *)g_u32_bbt_address);

        mdrv_spinand_info(&st_flash_nand_info);

        if (0 == g_u32_flash_rw_buff)
        {
            if (0 == (g_u32_flash_rw_buff = (u32)memalign(ARCH_DMA_MINALIGN, st_flash_nand_info.u16_PageSize + st_flash_nand_info.u16_OobSize)))
            {
                break;
            }
        }

        u32_flash_size = st_flash_nand_info.u32_Capacity;

        u8_dev_id = 0xEE;

        for (type = nand_flash_ids; type->name != NULL; type++)
        {
            if (u8_dev_id == type->dev_id)
            {
                printf("[FLASH] dev_id = 0x%x\r\n", type->dev_id);
                type->mfr_id = st_flash_nand_info.au8_ID[0];
                type->dev_id = st_flash_nand_info.au8_ID[1];
                type->id_len = st_flash_nand_info.u8_IDByteCnt;
                strncpy((char*)type->id, (const char*)st_flash_nand_info.au8_ID, st_flash_nand_info.u8_IDByteCnt);
                type->chipsize = u32_flash_size >> 20;
                type->pagesize = st_flash_nand_info.u16_PageSize;
                type->oobsize = st_flash_nand_info.u16_OobSize;
                type->erasesize = st_flash_nand_info.u32_BlockSize;
                type->ecc.strength_ds = st_flash_nand_info.u16_SectorSize;
                type->ecc.step_ds = st_flash_nand_info.u16_PageSize / st_flash_nand_info.u16_SectorSize;
                printf("[FLASH] mfr_id = 0x%x, dev_id= 0x%x id_len = 0x%x\r\n", type->id[0], type->id[1], type->id_len);
                break;
            }
        }

        nand->options = NAND_BROKEN_XD | NAND_SUBPAGE_READ | NAND_SKIP_BBTSCAN | NAND_NO_SUBPAGE_WRITE;
        nand->read_byte = spi_nand_read_byte;
        nand->read_word = spi_nand_read_word;
        nand->read_buf = spi_nand_read_buf;

        nand->select_chip = spi_nand_select_chip;
        nand->cmd_ctrl = spi_nand_cmd_ctrl;
        nand->dev_ready = spi_nand_dev_ready;
        nand->cmdfunc = spi_nand_cmdfunc;
        nand->waitfunc = spi_nand_waitfunc;

        nand->bits_per_cell = 1;
        nand->chip_delay = 0;
        nand->bbt_options = NAND_BBT_USE_FLASH;
        nand->ecc.mode = NAND_ECC_HW;
        nand->ecc.size = st_flash_nand_info.u16_SectorSize;
        nand->ecc.hwctl = spi_nand_ecc_hwctl;
        nand->ecc.calculate = spi_nand_ecc_calculate;
        nand->ecc.correct = spi_nand_ecc_correct;
        nand->ecc.read_page_raw = spi_nand_ecc_read_page_raw;
        nand->ecc.write_page_raw = spi_nand_ecc_write_page_raw;
        nand->ecc.read_page = spi_nand_ecc_read_page;
        nand->ecc.read_subpage = spi_nand_ecc_read_subpage;
        nand->ecc.write_page = spi_nand_ecc_write_page;
        nand->ecc.write_subpage = spi_nand_ecc_write_subpage;
        nand->ecc.read_oob = spi_nand_read_oob_std;
        nand->ecc.write_oob = spi_nand_write_oob_std;
        if (st_flash_nand_info.u16_OobSize == 256)
            nand->ecc.layout = &nand_oob_256;

        return 0;
    } while (0);

    if (0 != g_u32_sni_address)
    {
        free((void*)g_u32_sni_address);
        g_u32_sni_address = 0;
    }

    if (0 != g_u32_pni_address)
    {
        free((void*)g_u32_pni_address);
        g_u32_pni_address = 0;
    }

    if (0 != g_u32_bbt_address)
    {
        free((void*)g_u32_bbt_address);
        g_u32_bbt_address = 0;
    }

    if (0 != g_u32_flash_rw_buff)
    {
        free((void*)g_u32_flash_rw_buff);
        g_u32_flash_rw_buff = 0;
    }

    if (0 != g_u32_socecc_cache)
    {
        free((void*)g_u32_socecc_cache);
        g_u32_socecc_cache = 0;
    }

    return -1;
}

int spi_nand_probe(int column)
{
    if ((0 == g_u32_sni_address) && 0 == (g_u32_sni_address = (u32)calloc(1, FLASH_SNI_TABLE_SIZE)))
    {
        return -1;
    }

    if (!mdrv_spinand_find_sni_form_dram((u8 *)g_u32_sni_address, (u8 *)column))
    {
        return -1;
    }

    get_mtd_device_nm("nand0");
    nand_init();

    return 0;
}
