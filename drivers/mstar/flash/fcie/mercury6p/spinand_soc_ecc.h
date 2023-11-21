/*
 * spinand_soc_ecc.h- Sigmastar
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

#ifndef _SPINAND_SOC_ECC_H_
#define _SPINAND_SOC_ECC_H_

#include <drvSPINAND.h>

typedef struct
{
    u16 u16_pageSize;
    u16 u16_spareSize;
    u16 u16_sector_size;
    u8  u8_ecc_bits;
    u8 u8_ecc_steps;
    u16 u16_ecc_bytes;
} stFlashConfig;

typedef struct
{
    u8 *data_cache_codec;
    u8 *oob_cache_codec;
    u8 *data_cache;
    u8 *oob_cache;
    u16 u16_col_address;
    u16 u16_data_offset;
    u16 u16_oob_offset;
    u32 u32_total;
    u8  u8_ecc_bytes;
    u8  u8_ecc_status;
    u8  u8_ecc_bitflip_count;
    DRV_SPINAND_INFO_t *pst_spinand_drv_info;
} SPINAND_FCIE_CFG_t;

u8 drv_spinand_soc_ecc_is_enable(void);
void drv_spinand_soc_ecc_setup(const stFlashConfig *pst_flash_config);
u8 drv_spinand_soc_ecc_read(SPINAND_FCIE_CFG_t *pst_fcie_cfg);
u8 drv_spinand_soc_ecc_write(SPINAND_FCIE_CFG_t *pst_fcie_cfg);

#endif /* _SPINAND_SOC_ECC_H_ */
