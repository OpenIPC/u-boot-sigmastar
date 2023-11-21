/*
 * spinand_soc_ecc.c- Sigmastar
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
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/nand_ecc.h>
#include <drvFCIE.h>
#include <drvSPINAND.h>
#include <drvFSP_QSPI.h>
#include <spinand_soc_ecc.h>
#include <mdrvFlashOsImpl.h>

static u8 drv_spinand_get_cmd_mode(u8 u8_cmd)
{
    switch (u8_cmd)
    {
        case SPI_NAND_CMD_QPL:
        case 0x6B:
        case SPI_NAND_CMD_QRPL:
        case 0xEB:
            return FSP_QUAD_MODE;
        case 0x3B:
        case 0xBB:
            return FSP_DUAL_MODE;
        default:
            return FSP_SINGLE_MODE;
    }
}

static u8 _drv_spinand_soc_ecc_correct(u16 u16_col_address, u16 oob_offset, u32 u32_total)
{
    u32 u32_boundary = 0;

    u32_boundary = u16_col_address + 512;
    //FLASH_IMPL_PRINTF("%s: u16_col = 0x%04x, oob_offset = 0x%04x, u32_total = 0x%04x\r\n", __func__, u16_col_address, oob_offset, u32_total);
    DRV_SPI2FCIE_reset();
    DRV_SPI2FCIE_boundary_enable_pre_set(u16_col_address, u32_boundary);
    DRV_SPI2FCIE_enable_upstream_size(u32_total);

    DRV_QSPI_pull_cs(0);
    DRV_FCIE_job_read_sectors();

    if (!DRV_SPI2FCIE_boundary_is_hit(FCIE_WAIT_READ_TIME)) //reach sector size
    {
        DRV_QSPI_pull_cs(1);
        return ERR_SPINAND_TIMEOUT;
    }

    DRV_QSPI_pull_cs(1);

    DRV_QSPI_pull_cs(0);
    DRV_SPI2FCIE_boundary_refresh_spi_offset(oob_offset);

    if (!DRV_FCIE_job_is_done(FCIE_WAIT_READ_TIME))
    {
        return ERR_SPINAND_TIMEOUT;
    }

    DRV_QSPI_pull_cs(1);
    DRV_FCIE_clear_job();

    return DRV_FCIE_get_ecc_status();
}

static u8 drv_spinand_soc_ecc_check_sector_status(SPINAND_FCIE_CFG_t *pst_fcie_cfg)
{
    u8 u8_status;

    DRV_QSPI_use_2bytes_address_mode(DRV_QSPI_cmd_to_mode(pst_fcie_cfg->pst_spinand_drv_info->u8_rfc), pst_fcie_cfg->pst_spinand_drv_info->u8_dummy);

    u8_status = _drv_spinand_soc_ecc_correct((pst_fcie_cfg->u16_data_offset | pst_fcie_cfg->u16_col_address), pst_fcie_cfg->u16_oob_offset, pst_fcie_cfg->u32_total);

    DRV_QSPI_use_2bytes_address_mode(DRV_QSPI_cmd_to_mode(SPI_NAND_CMD_RFC), SPI_NAND_CMD_RFC_DUMMY);

    return u8_status;
}

static u8 drv_spinand_soc_ecc_get_status(SPINAND_FCIE_CFG_t *pst_fcie_cfg)
{
    return drv_spinand_soc_ecc_check_sector_status(pst_fcie_cfg);
}

static void drv_spinand_soc_ecc_setup_read_address(void* address, void* oob_offset)
{
    DRV_FCIE_set_read_address((u64)(unsigned long)address & 0x0fffffff, (u64)(unsigned long)oob_offset & 0x0fffffff);
}

static u8 _drv_spinand_soc_ecc_outside_transmission(u8* pu8_buf, u32 u32_size, u32 u32_outside_size)
{
    u8 u8_wr_size;
    u32 u32_transfer_size;

    u8_wr_size = 0;

    if (pu8_buf)
    {
        DRV_FSP_init(FSP_USE_SINGLE_CMD);
        //DRV_FSP_clear_fifo_size();
        u8_wr_size = DRV_FSP_write_wbf(pu8_buf, u32_size);

        if (u8_wr_size != u32_size)
        {
            return ERR_SPINAND_INVALID;
        }
    }

    while (0 != u32_outside_size)
    {
        DRV_FSP_set_which_cmd_size(1, u8_wr_size);
        u32_transfer_size = DRV_FSP_enable_outside_wbf(FSP_OUTSIDE_MODE_SPI2FCIE, 0, u32_size, u32_outside_size);

        DRV_FSP_trigger();

        if (!DRV_FSP_is_done())
        {
            break;
        }

        DRV_FSP_clear_trigger();
        //DRV_FSP_clear_fifo_size();
        u8_wr_size = 0;
        u32_outside_size -= u32_transfer_size;
    }

    DRV_FSP_disable_outside_wbf();

    if (0 != u32_outside_size)
    {
        return ERR_SPINAND_TIMEOUT;
    }

    return ERR_SPINAND_SUCCESS;
}

static u8 drv_spinand_soc_ecc_random_program_load_consistency(u16 u16_offset, u32 u32_size)
{
    u8 u8_status;
    u32 u32_cmd;

    u32_cmd = 0;
    u32_cmd |= SPI_NAND_CMD_RPL;
    u32_cmd |= ((u16_offset >> 8) & 0xff) << 8;
    u32_cmd |= (u16_offset& 0xff) << 16;

    DRV_FSP_set_access_mode(drv_spinand_get_cmd_mode(SPI_NAND_CMD_RPL));
    DRV_QSPI_pull_cs(0);
    u8_status = _drv_spinand_soc_ecc_outside_transmission((u8*)&u32_cmd, 3, u32_size);
    DRV_QSPI_pull_cs(1);
    DRV_FSP_set_access_mode(0);
    return u8_status;
}

static u8 drv_spinand_soc_ecc_random_program_load(u16 u16_offset, u16 oob_offset, u8 oob_size)
{
    DRV_SPI2FCIE_reset();
    DRV_SPI2FCIE_enable_downstream();
    DRV_SPI2FCIE_block_downstream();
    DRV_FCIE_job_write_sector_start();

    if (ERR_SPINAND_SUCCESS != drv_spinand_soc_ecc_random_program_load_consistency(u16_offset, 512))
    {
        return ERR_SPINAND_TIMEOUT;
    }

    DRV_SPI2FCIE_resume_downstream();
    if (ERR_SPINAND_SUCCESS != drv_spinand_soc_ecc_random_program_load_consistency(oob_offset, oob_size))
    {
        return ERR_SPINAND_TIMEOUT;
    }

    if (!DRV_FCIE_job_is_done(FCIE_WAIT_WRITE_TIME))
    {
        return ERR_SPINAND_TIMEOUT;
    }
    DRV_SPI2FCIE_disable_stream();
    DRV_FCIE_clear_job();
    return ERR_SPINAND_SUCCESS;
}

static void drv_spinand_soc_ecc_setup_write_address(void* address, void* oob_offset)
{
    DRV_FCIE_set_write_address((u64)(unsigned long)address & 0x0fffffff, (u64)(unsigned long)oob_offset & 0x0fffffff);
}

u8 drv_spinand_soc_ecc_is_enable(void)
{
    u16 OTP_SOC_ECC;

    //OTP_SOC_ECC = FLASH_IMPL_INREG16((0x101F << 9) + (0x3E << 2)) & 0x03;
    OTP_SOC_ECC = 0x1;

    return (0x01 == OTP_SOC_ECC || 0x10 == OTP_SOC_ECC);
}

void drv_spinand_soc_ecc_setup(const stFlashConfig *pst_flash_config)
{
    DRV_FCIE_init();
    DRV_FCIE_reset();
    DRV_FCIE_stetup_ecc_ctrl(pst_flash_config->u16_pageSize, pst_flash_config->u16_spareSize, DRV_FCIE_get_ecc_mode(pst_flash_config->u16_pageSize / pst_flash_config->u8_ecc_steps, pst_flash_config->u8_ecc_bits));
    DRV_FCIE_enable_ecc(1);

    printf("[SPINAND] %s\r\n", __func__);
}

u8 drv_spinand_soc_ecc_read(SPINAND_FCIE_CFG_t *pst_fcie_cfg)
{
    drv_spinand_soc_ecc_setup_read_address(pst_fcie_cfg->data_cache_codec, pst_fcie_cfg->oob_cache_codec);

    pst_fcie_cfg->u8_ecc_status = drv_spinand_soc_ecc_get_status(pst_fcie_cfg);

    return ERR_SPINAND_SUCCESS;
}

u8 drv_spinand_soc_ecc_write(SPINAND_FCIE_CFG_t *pst_fcie_cfg)
{
    drv_spinand_soc_ecc_setup_write_address(pst_fcie_cfg->data_cache, pst_fcie_cfg->oob_cache);

    if (ERR_SPINAND_SUCCESS != drv_spinand_soc_ecc_random_program_load(pst_fcie_cfg->u16_data_offset, pst_fcie_cfg->u16_oob_offset, pst_fcie_cfg->u8_ecc_bytes))
    {
        printf("[FLASH_ERR] drv_spinand_soc_ecc_random_program_load fail!\r\n");
        return ERR_SPINAND_DEVICE_FAILURE;
    }

    return ERR_SPINAND_SUCCESS;
}

