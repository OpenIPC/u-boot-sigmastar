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
#include <mdrvSNI.h>
#include <drvSPINAND.h>
#include <nand.h>
#include <drvFSP_QSPI.h>

static const u8 au8_magicData[] = {0x4D, 0x53, 0x54, 0x41, 0x52, 0x53, 0x45, 0x4D, 0x49, 0x55, 0x53, 0x46, 0x44, 0x43, 0x49, 0x53};
static const u8 au8_extMagicData[] = {'S', 'S', 'T', 'A', 'R', 'S', 'E', 'M', 'I', 'S', 'N', 'I', 'V', '0', '0', '0'};
static u8 u8_csFlg = 0;
static u32 g_u32_sni_address = 0;
SPINAND_SNI_t *g_pst_spinand_sni = NULL;
static SPINAND_INFO_t *g_pst_spinand_info = NULL;
static SPINAND_EXT_INFO_t *g_pst_spinand_ext_info = NULL;
SPINAND_EXT_CONFIGURATION_t *g_pst_spinand_ext_configuration = NULL;

#define FLASH_LEAGCY_SNI_SIZE           (sizeof(au8_magicData) + sizeof(SPINAND_INFO_t))

#define FLASH_SNI_HEADER_SIZE           sizeof(au8_magicData)
#define FLASH_EXT_SNI_HEADER_SIZE       sizeof(au8_extMagicData)
#define FLASH_MAX_SNI_SIZE              512
#define FLASH_EXT_SNI_RESERVED_SIZE     0x1BC
#define FLASH_PAGE_SIZE_MASK            (g_pst_spinand_info->u16_PageByteCnt - 1)
#define FLASH_BLOCK_MASK                (g_pst_spinand_info->u16_BlkPageCnt - 1)
#define FLASH_PAGE_SIZE                 g_pst_spinand_info->u16_PageByteCnt
#define FLASH_BLOCK_SIZE                (FLASH_PAGE_SIZE * g_pst_spinand_info->u16_BlkPageCnt)
#define FLASH_BLOCK_SIZE_MASK           (FLASH_BLOCK_SIZE - 1)
#define FLASH_BLOCK_COUNT               g_pst_spinand_info->u16_BlkCnt
#define FLASH_PAGES_PER_BLOCK           g_pst_spinand_info->u16_BlkPageCnt
#define FLASH_SNI_BLOCKS_SIZE           10
#define FLASH_SNI_TABLE_SIZE            0x800
#define FLASH_PAGES_PER_BLOCK_DEFAULT   64
#define FLASH_BLOCK_MASK_DEFAULT        (FLASH_PAGES_PER_BLOCK_DEFAULT - 1)
#define FLASH_ECC_NO_CORRECTED          0
#define FLASH_ECC_BITFLIP               (g_pst_spinand_ext_info->st_profile.st_eccConfig.u8_bitflipThreshold)
#define FLASH_ECC_NOT_CORRECTED         (g_pst_spinand_ext_info->st_profile.st_eccConfig.u8_eccNotCorrectStatus)
#define FLASH_ECC_RESERVED              (g_pst_spinand_ext_info->st_profile.st_eccConfig.u8_eccReserved)
#define FLASH_ECC_STATUS_MASK           (g_pst_spinand_ext_info->st_profile.st_eccConfig.u8_eccStatusMask)

#if 1
#define spi_nand_msg(fmt, ...) printk(KERN_NOTICE "%s: " fmt "\n", __func__, ##__VA_ARGS__)
#define spi_nand_debug(fmt, ...)
#else
#define spi_nand_msg(fmt, ...)
#define spi_nand_debug(fmt, ...) printk(KERN_NOTICE "%s: " fmt "\n", __func__, ##__VA_ARGS__)
#endif

static u8 (*_flash_check_page_status)(void);

u8 _flash_count_bits(u32 u32_x)
{
    u8 u8_i = 0;

    while (u32_x)
    {
        u8_i++;
        u32_x >>= 1;
    }

    return (u8_i - 1);
}

static u8 _flash_is_sni_header(const u8 *u8_header, u8 u8_size, u8 *pu8_data)
{
    return !(memcmp(u8_header, pu8_data, u8_size));
}

static void _flash_unlock_whole_block(SPINAND_EXT_INFO_t *pst_ext_sni)
{
    u32 u32_status;

    SPINAND_PROTECT_t *pst_protect_status;
    FLASH_CMD_SET_t *pst_cmd;

    if (_flash_is_sni_header(au8_extMagicData, FLASH_EXT_SNI_HEADER_SIZE, pst_ext_sni->au8_magic))
    {
        if (0 == (SPINAND_ALL_LOCK & pst_ext_sni->st_profile.u16_flags))
        {
            return;
        }

        pst_protect_status = &pst_ext_sni->st_profile.st_extConfig.st_protectStatus;
        pst_cmd = &pst_protect_status->st_blockStatus.st_blocks;
        u32_status = 0;

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_cmd->u32_address, (u8*)&u32_status, pst_cmd->u16_dataBytes))
        {
            printf("%s get features failed\r\n",__func__);
        }

        u32_status &= ~pst_cmd->u16_value;

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_set_features(pst_cmd->u32_address, (u8*)&u32_status, pst_cmd->u16_dataBytes))
        {
            printf("%s set features failed\r\n",__func__);
        }

        printf("[FLASH] Unlock all block.\r\n");
    }
}

static void _flash_show_id(u8* au8_id)
{
    printk("[FLASH] ID : 0x%02x 0x%02x 0x%02x\n", au8_id[0], au8_id[1], au8_id[2]);
}

static u8 _flash_is_id_match(u8* au8_id, u8 u8_id_len)
{
    u8 u8_i;
    u8 au8_device_id[8];
    if (ERR_SPINAND_SUCCESS != DRV_SPINAND_read_id(au8_device_id, u8_id_len))
    {
        spi_nand_debug("[FLASH] read device id fail!\r\n");
        return 0;
    }
    for(u8_i = 0; u8_id_len > u8_i; u8_i++)
    {
        if (au8_device_id[u8_i] != au8_id[u8_i])
        {
            printk("[FLASH] No matched ID\n");
            _flash_show_id(au8_device_id);
            return 0;
        }
    }
    return 1;
}

static int _MDRV_FLASH_switch_config(FLASH_CMD_SET_t *pst_config, u8 u8_enabled)
{
    u32 u32_config;

    u32_config = 0;

    if (ERR_SPINAND_TIMEOUT != DRV_SPINAND_check_status(NULL))
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_config->u32_address, (u8*)&u32_config, pst_config->u16_dataBytes))
        {
            spi_nand_msg("[FLASH_ERR]get features failed\r\n");
            return 0;
        }

        if (u8_enabled)
        {
            u32_config |= pst_config->u16_value;
        }
        else
        {
            u32_config &= ~pst_config->u16_value;
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_set_features(pst_config->u32_address, (u8*)&u32_config, pst_config->u16_dataBytes))
        {
            spi_nand_msg("[FLASH_ERR] set features failed\r\n");
            return 0;
        }
    }
    else
    {
        return 0;
    }

    if (ERR_SPINAND_TIMEOUT != DRV_SPINAND_check_status(NULL))
        return 1;

    return 0;
}

static u8 _flash_check_page_status_default(void)
{
    u8 u8_ret;
    u8 u8_status;
    SPINAND_EXT_PROFILE_t *pst_profile;

    u8_ret = DRV_SPINAND_check_status(&u8_status);

    if(ERR_SPINAND_SUCCESS != u8_ret)
        return u8_ret;

    u8_status &= ECC_STATUS_MASK;

    if (0 < u8_status)
    {
        if (ECC_NOT_CORRECTED == u8_status)
        {
            return ERR_SPINAND_ECC_NOT_CORRECTED;
        }

        if (ECC_STATUS_RESERVED == u8_status)
        {
            return ERR_SPINAND_ECC_RESERVED;
        }
        return ERR_SPINAND_ECC_CORRECTED;
    }

    if (_flash_is_sni_header(au8_extMagicData, FLASH_EXT_SNI_HEADER_SIZE, g_pst_spinand_ext_info->au8_magic))
    {
        pst_profile = &g_pst_spinand_ext_info->st_profile;

        if (ERR_SPINAND_ECC_RESERVED == u8_status)
        {
            if (pst_profile->u16_flags & SPINAND_ECC_RESERVED_NONE_CORRECTED)
            {
                return ERR_SPINAND_ECC_NOT_CORRECTED;
            }

            if (pst_profile->u16_flags & SPINAND_ECC_RESERVED_CORRECTED)
            {
                return ERR_SPINAND_ECC_CORRECTED;
            }
        }
    }

    return u8_status;
}

static u8 _flash_check_page_status_by_sni(void)
{
    u8 u8_ret;
    u8 u8_status;
    SPINAND_EXT_PROFILE_t *pst_profile = &g_pst_spinand_ext_info->st_profile;
    SPINAND_ECC_CONFIG_t *pst_eccConfig = &pst_profile->st_eccConfig;
    FLASH_CMD_SET_t *pst_eccStatus = &pst_eccConfig->st_eccStatus;

    u8_ret = DRV_SPINAND_check_status(&u8_status);

    if(ERR_SPINAND_SUCCESS != u8_ret)
        return u8_ret;

    u8_status &= FLASH_ECC_STATUS_MASK;

    if (FLASH_ECC_NO_CORRECTED == u8_status)
        return ERR_SPINAND_SUCCESS;
    else if (FLASH_ECC_NOT_CORRECTED == u8_status)
        return ERR_SPINAND_ECC_NOT_CORRECTED;
    else if ((pst_eccConfig->u8_eccType & SPINAND_RESERVED_NONE_CORRECTED) && u8_status == FLASH_ECC_RESERVED)
        return ERR_SPINAND_ECC_NOT_CORRECTED;

    if(pst_eccConfig->u8_eccType & SPINAND_THRESHOLD)
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(pst_eccStatus->u8_command, (u8)(pst_eccStatus->u32_address), &u8_status, 1))
            return ERR_SPINAND_DEVICE_FAILURE;

        if((u8_status & pst_eccStatus->u16_value) != pst_eccStatus->u16_value)
            return ERR_SPINAND_SUCCESS;
    }
    else if(pst_eccConfig->u8_eccType & SPINAND_BITFLIP)
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(pst_eccStatus->u8_command, (u8)(pst_eccStatus->u32_address), &u8_status, 1))
            return ERR_SPINAND_DEVICE_FAILURE;

        if ((u8_status & pst_eccStatus->u16_value) < FLASH_ECC_BITFLIP)
            return ERR_SPINAND_SUCCESS;
    }

    return ERR_SPINAND_ECC_CORRECTED;
}

static void _flash_setup_by_sni(SPINAND_EXT_INFO_t *pst_ext_sni)
{
    SPINAND_EXT_PROFILE_t *pst_profile;
    SPINAND_ACCESS_CONFIG_t *pst_access;
    SPINAND_ECC_CONFIG_t *pst_eccConfig;

    if(pst_ext_sni)
    {
        if (_flash_is_sni_header(au8_extMagicData, FLASH_EXT_SNI_HEADER_SIZE, pst_ext_sni->au8_magic))
        {
            pst_profile = &pst_ext_sni->st_profile;
            pst_access = &pst_profile->st_access;
            pst_eccConfig = &pst_profile->st_eccConfig;

            if (pst_eccConfig->u8_eccEn)
            {
                _flash_check_page_status = _flash_check_page_status_by_sni;
                printk("[FLASH] check page status by sni\r\n");
            }
            else
                _flash_check_page_status = _flash_check_page_status_default;

            DRV_SPINAND_setup_access(pst_access->st_read.u8_command,
                                     pst_access->st_read.u8_dummy,
                                     pst_access->st_program.u8_command,
                                     pst_access->st_random.u8_command);
            if (pst_profile->u16_flags & SPINAND_NEED_QE)
            {
                if(!_MDRV_FLASH_switch_config(&pst_access->st_qeStatus, 1))
                    spi_nand_msg("[FLASH_ERR] Enable quad mode failed\r\n");
            }

            if (pst_eccConfig->u8_eccType & SPINAND_THRESHOLD)
            {
                if(!_MDRV_FLASH_switch_config(&pst_eccConfig->st_threshold, 1))
                    spi_nand_msg("[FLASH_ERR] Set threshold failed\r\n");
            }
            DRV_SPINAND_setup_timeout(pst_profile->u32_maxWaitTime);
            _flash_unlock_whole_block(pst_ext_sni);
        }
    }
}

static void _flash_reset_status(SPINAND_EXT_INFO_t *pst_ext_sni)
{
    SPINAND_EXT_PROFILE_t *pst_profile;

     DRV_SPINAND_reset_status();
    if(pst_ext_sni)
    {
        if (_flash_is_sni_header(au8_extMagicData, FLASH_EXT_SNI_HEADER_SIZE, pst_ext_sni->au8_magic))
        {
            pst_profile = &pst_ext_sni->st_profile;

            if (pst_profile->u16_flags & SPINAND_VOLATILE)
            {
                _flash_setup_by_sni(g_pst_spinand_ext_info);
            }
        }
    }
}

static void _flash_wait_dev_ready(void)
{
    if (ERR_SPINAND_TIMEOUT == DRV_SPINAND_check_status(NULL))
    {
        printf("[FLASH_ERR] check status fail!\r\n");
    }
}

static u16 _flash_offset_wrap(u32 u32_page_address)
{
    if (1 < g_pst_spinand_info->u8PlaneCnt)
    {
        if (0 < (u32_page_address & FLASH_PAGES_PER_BLOCK))
        {
            return (1 << 12);
        }
    }

    return 0;
}

static void _flash_die_sel(u32 u32_page_address)
{
    u32 u32_which_die;
    u32 u32_limit_page_address;

    if (g_pst_spinand_ext_info->st_profile.u16_flags & SPINAND_MULTI_DIES)
    {
        u32_which_die = 0;
        u32_limit_page_address = g_pst_spinand_ext_info->st_profile.st_extConfig.st_dieConfig.u32_dieSize / FLASH_PAGE_SIZE;

        u32_which_die = g_pst_spinand_ext_info->st_profile.st_extConfig.st_dieConfig.st_dieCode.u8_command;

        if (u32_limit_page_address <= u32_page_address)
        {
            u32_which_die |= (1 << 8);
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_complete_transmission((u8*)&u32_which_die, g_pst_spinand_ext_info->st_profile.st_extConfig.st_dieConfig.st_dieCode.u16_dataBytes + 1))
        {
            spi_nand_msg("[FLASH_ERR] Change die fail!");
        }
    }
}

static void _flash_otp_enable(u8 u8_enable)
{
    u8 u8_status = 0;
    SPINAND_OTP_t *pst_otp;

    pst_otp = &g_pst_spinand_ext_info->st_profile.st_extConfig.st_otp;

    do
    {
        if (ERR_SPINAND_SUCCESS !=DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_otp->st_otpEnabled.u32_address, &u8_status, pst_otp->st_otpEnabled.u16_dataBytes))
        {
            spi_nand_msg("get status register:0x%x failed\r\n",pst_otp->st_otpEnabled.u32_address);
            break;
        }
        if(u8_enable)
        {
            u8_status |= pst_otp->st_otpEnabled.u16_value;
        }
        else
        {
            u8_status &= ~(pst_otp->st_otpEnabled.u16_value);
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_set_features(pst_otp->st_otpEnabled.u32_address, &u8_status, pst_otp->st_otpEnabled.u16_dataBytes))
        {
            spi_nand_msg("set status register:0x%x failed\r\n",pst_otp->st_otpEnabled.u32_address);
        }
    } while (0);
}

static void _flash_otp_lock(u8 u8_enable)
{
    u8 u8_status;

    SPINAND_OTP_t *pst_otp;

    pst_otp = &g_pst_spinand_ext_info->st_profile.st_extConfig.st_otp;

    do
    {
        if (ERR_SPINAND_SUCCESS !=DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_otp->st_otpLock.u32_address, &u8_status, pst_otp->st_otpLock.u16_dataBytes))
        {
            spi_nand_msg("get status register:0x%x failed",pst_otp->st_otpLock.u32_address);
            break;
        }

        if (u8_enable)
        {
            u8_status |= pst_otp->st_otpLock.u16_value;
        }
        else
        {
            spi_nand_msg("just for reading the register status=0x%x,can't unlock the otp\r\n",u8_status);
            u8_status &= (~pst_otp->st_otpLock.u16_value);
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_set_features(pst_otp->st_otpLock.u32_address, &u8_status, pst_otp->st_otpLock.u16_dataBytes))
        {
            spi_nand_msg("set status register:0x%x failed",pst_otp->st_otpLock.u32_address);
            break;
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_program_execute(0))
        {
            spi_nand_msg("DRV_SPINAND_cmd_program_execute fail");
        }
    } while (0);

}

static void _flash_do_ecc(u8 u8_enable)
{
    u8 u8Status;

    FLASH_CMD_SET_t *pst_eccEnable;
    pst_eccEnable = &g_pst_spinand_ext_info->st_profile.st_extConfig.st_eccEnabled;

    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_eccEnable->u32_address, &u8Status, pst_eccEnable->u16_dataBytes))
        {
            spi_nand_msg("get status register:0x%x failed.", pst_eccEnable->u32_address);
            break;
        }

        if(u8_enable)
        {
            u8Status |= pst_eccEnable->u16_value;
        }
        else
        {
            u8Status &= ~pst_eccEnable->u16_value;
        }

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_set_features(pst_eccEnable->u32_address, &u8Status, pst_eccEnable->u16_dataBytes))
        {
            spi_nand_msg("set status register:0x%x failed.", pst_eccEnable->u32_address);
            break;
        }

        u8Status = 0;

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_get_features(SPI_NAND_CMD_GF, pst_eccEnable->u32_address, &u8Status, pst_eccEnable->u16_dataBytes))
        {
            spi_nand_msg("get status register:0x%x failed.", pst_eccEnable->u32_address);
            break;
        }

        spi_nand_msg("after ECC engine: 0x%x.", u8Status);
    } while (0);
}

static int _flash_read_from_cache(u32 u32_offset, u32 u32_address, u32 u32_size)
{
    if (ERR_SPINAND_SUCCESS != DRV_SPINAND_read_from_cache(u32_offset, (u8*)u32_address, u32_size))
    {
        spi_nand_msg("[FLASH_ERR] Read data from page fail!");
        return 0;
    }

    return u32_size;
}

static int _flash_check_sni_available(u32 u32_address)
{
    u8 u8_ismatch = 0;
    u32 u32_page;
    u32 u32_search_page;

    SPINAND_SNI_t *pst_sni;
    SPINAND_EXT_INFO_t *pst_ext_info;
    SPINAND_EXT_CONFIGURATION_t *pst_ext_configuration;

    pst_sni = (SPINAND_SNI_t *)u32_address;
    pst_ext_info = &pst_sni->spinand_ext_info;
    pst_ext_configuration = &pst_sni->spinand_ext_configuration;
    u32_page = 10 * FLASH_PAGES_PER_BLOCK_DEFAULT;

    for (u32_search_page = 0; u32_search_page < u32_page; u32_search_page += (FLASH_PAGES_PER_BLOCK_DEFAULT << 1))
    {
        if (ERR_SPINAND_SUCCESS !=  DRV_SPINAND_page_read(u32_search_page))
        {
            printf("[FLASH] page read fail!\r\n");
            break;
        }

        _flash_wait_dev_ready();

        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_read_from_cache(0, (u8 *)u32_address, FLASH_SNI_TABLE_SIZE))
        {
            break;
        }

        if(pst_sni->u32_crc32 == crc32(0, (void *)u32_address, (u32)&pst_sni->u32_crc32 - u32_address))
        {
            u8_ismatch = 1;
            break;
        }

        u8_ismatch = 0;
    }

    if (pst_ext_info->u32_extSni)
        printf("[FLASH] Load EXT SNI\n");

    if (u8_ismatch && pst_ext_info->u32_extSni &&
        pst_ext_configuration->u32_crc32 != crc32(0, (void *)((u32)pst_ext_configuration + 4), FLASH_EXT_SNI_RESERVED_SIZE * pst_ext_info->u32_extSni - 4))
    {
        printk("[FLASH] Load EXT SNI fail,EXT SNI can not use\n");
        memset((void *)pst_ext_configuration, 0,sizeof(SPINAND_EXT_CONFIGURATION_t));
    }

    return u8_ismatch;
}

static u32 _flash_load_sni(u32 u32_dst, u32 u32_src)
{
    u32 u32_extsni = 0;
    SPINAND_SNI_t *pst_sni = NULL;
    SPINAND_INFO_t *pst_info = NULL;
    SPINAND_EXT_INFO_t *pst_ext_info = NULL;
    SPINAND_EXT_SNI_t *pst_ext_sni = NULL;

    while (_flash_is_sni_header(au8_magicData, FLASH_SNI_HEADER_SIZE, (u8*)u32_src))
    {
        pst_sni = (SPINAND_SNI_t *)u32_src;
        pst_info = &pst_sni->spinand_info;
        pst_ext_info = &pst_sni->spinand_ext_info;

        if (_flash_is_id_match(pst_info->au8_ID, pst_info->u8_IDByteCnt) &&
            pst_sni->u32_crc32 == crc32(0, (void *)u32_src, (u32)&pst_sni->u32_crc32 - u32_src))
        {
            memcpy((void *)u32_dst, (void *)u32_src, FLASH_MAX_SNI_SIZE);
            u32_dst += FLASH_MAX_SNI_SIZE;
            break;
        }

        u32_src += FLASH_MAX_SNI_SIZE;
    }

    if (!_flash_is_sni_header(au8_magicData, FLASH_SNI_HEADER_SIZE, (u8*)u32_src))
        return 0;

    if (pst_ext_info)
        u32_extsni = pst_ext_info->u32_extSni;

    while (u32_extsni)
    {
        u32_src += FLASH_MAX_SNI_SIZE;
        pst_ext_sni = (SPINAND_EXT_SNI_t *)u32_src;
        if (pst_ext_sni->u32_crc32 == crc32(0, (void *)u32_src, (u32)&pst_ext_sni->u32_crc32 - u32_src))
        {
            memcpy((void *)u32_dst, (void *)pst_ext_sni->au8_reserved, sizeof(pst_ext_sni->au8_reserved));
            u32_dst += sizeof(pst_ext_sni->au8_reserved);
        }
        else
            return 0;

        u32_extsni--;
    }

    return 1;
}

uint8_t	spi_nand_read_byte(struct mtd_info *mtd)
{
    u8 u8_byte;
    DRV_SPINAND_receive_data(&u8_byte, 1);
    return u8_byte;
}

u16 spi_nand_read_word(struct mtd_info *mtd)
{
    u16 u16_word;
    DRV_SPINAND_receive_data((u8*)&u16_word, 2);
    return u16_word;
}

void spi_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
    DRV_SPINAND_receive_data(buf, len);
}

void spi_nand_select_chip(struct mtd_info *mtd, int chip)
{
    //printf("spi_nand_select_chip  Not support\r\n");
}

void spi_nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
    spi_nand_debug("spi_nand_cmd_ctrl Not support\r\n");
}

int spi_nand_dev_ready(struct mtd_info *mtd)
{
    spi_nand_debug("spi_nand_dev_ready Not support\r\n");

    return 1;
}

void spi_nand_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
    struct nand_chip *chip;
    chip = (struct nand_chip*)mtd->priv;
    if(u8_csFlg)
    {
        DRV_QSPI_pull_cs(1);
        u8_csFlg = 0;
    }
    switch (command)
    {
        case NAND_CMD_STATUS:
            spi_nand_debug("NAND_CMD_STATUS");
            DRV_QSPI_pull_cs(0);
            u8_csFlg = 1;
            DRV_SPINAND_cmd_read_status_register();
            break;

        case NAND_CMD_PAGEPROG:
            spi_nand_debug("NAND_CMD_PAGEPROG");
            chip->pagebuf = -1;
            break;

        case NAND_CMD_READOOB:
            spi_nand_debug("NAND_CMD_READOOB page_addr: 0x%x", page_addr);
            _flash_die_sel(page_addr);
            if (ERR_SPINAND_TIMEOUT != DRV_SPINAND_page_read_with_status(page_addr))
            {
                DRV_QSPI_pull_cs(0);
                u8_csFlg = 1;
                DRV_SPINAND_cmd_normal_read_from_cache(_flash_offset_wrap(page_addr) | (mtd->writesize + column));
            }
            break;

        case NAND_CMD_READID:
            spi_nand_debug("NAND_CMD_READID\r\n");
            DRV_QSPI_pull_cs(0);
            u8_csFlg = 1;
            DRV_SPINAND_cmd_read_id();
            break;

        case NAND_CMD_ERASE2:
            spi_nand_debug("NAND_CMD_ERASE2\r\n");
            break;

        case NAND_CMD_ERASE1:
            spi_nand_debug("NAND_CMD_ERASE1, page_addr: 0x%x", page_addr);
            _flash_die_sel(page_addr);
            DRV_SPINAND_block_erase(page_addr);
            break;
        case NAND_CMD_READ0:
            break;
        case NAND_CMD_SEQIN:
            spi_nand_debug("NAND_CMD_SEQIN");
            chip->pagebuf = page_addr;
            break;
        case NAND_CMD_RESET:
            spi_nand_debug("NAND_CMD_RESET");
            _flash_reset_status(g_pst_spinand_ext_info);
            break;
        case NAND_CMD_OTP_ENABLE:
            spi_nand_debug("NAND_CMD_OTP_ENABLE");
            _flash_otp_enable(column);
            break;
        case NAND_CMD_OTP_LOCK:
            spi_nand_debug("NAND_CMD_OTP_LOCK");
            _flash_otp_lock(column);
            break;
        case NAND_CMD_DO_ECC:
            spi_nand_debug("NAND_CMD_DO_ECC");
            _flash_do_ecc(column);
            break;
        default:
            spi_nand_msg("unsupported command %02Xh\n", command);
            break;
    }
    return;
}

int spi_nand_waitfunc(struct mtd_info *mtd, struct nand_chip *this)
{
    spi_nand_debug("spi_nand_waitfunc\r\n");
    u8 u8_status;
    SPINAND_EXT_INFO_t *pst_spinand_ext_info = (SPINAND_EXT_INFO_t *)this->priv;

    DRV_SPINAND_setup_timeout(400000);

    u8_status = DRV_SPINAND_check_status(NULL);

    DRV_SPINAND_setup_timeout(pst_spinand_ext_info->st_profile.u32_maxWaitTime);

    return (ERR_SPINAND_E_FAIL > u8_status)? NAND_STATUS_READY : NAND_STATUS_FAIL;
}

void spi_nand_ecc_hwctl(struct mtd_info *mtd, int mode)
{
    spi_nand_debug(" spi_nand_ecc_hwctl Not support");
}

int spi_nand_ecc_calculate(struct mtd_info *mtd, const uint8_t *dat, uint8_t *ecc_code)
{
    spi_nand_debug("spi_nand_ecc_calculate Not support");
    return 0;
}

int spi_nand_ecc_correct(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc, uint8_t *calc_ecc)
{
    spi_nand_debug(" spi_nand_ecc_correct Not support");
    return 0;
}

int spi_nand_ecc_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
    uint8_t *buf, int oob_required, int page)
{
    do
    {
        _flash_die_sel(page);
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_page_read(page))
        {
            break;
        }
        _flash_wait_dev_ready();
        if(mtd->writesize != _flash_read_from_cache((0 | _flash_offset_wrap(page)), (u32)buf, mtd->writesize))
        {
            break;
        }

        if (oob_required)
        {
            _flash_wait_dev_ready();
            if(mtd->oobsize != _flash_read_from_cache((mtd->writesize | _flash_offset_wrap(page)), (u32)chip->oob_poi, mtd->oobsize))
            {
                break;
            }
        }

        return 0;
    }while(0);
    return -EIO;
}

int spi_nand_ecc_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required)
{
    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_write_enable())
        {
            break;
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_load(0x0 | _flash_offset_wrap(chip->pagebuf),(u8 *)buf, mtd->writesize))
        {
            break;
        }

        if (oob_required)
        {
            _flash_wait_dev_ready();
            if(ERR_SPINAND_SUCCESS != DRV_SPINAND_random_program_load(mtd->writesize | _flash_offset_wrap(chip->pagebuf), chip->oob_poi, mtd->oobsize))
            {
                break;
            }
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_execute(chip->pagebuf))
        {
            break;
        }

        if(NAND_STATUS_FAIL == spi_nand_waitfunc(mtd, chip))
        {
            break;
        }

        return 0;
    }while (0);
    return -EIO;
}


int spi_nand_ecc_read_page(struct mtd_info *mtd, struct nand_chip *chip,
    uint8_t *buf, int oob_required, int page)
{
    u8 u8_status;
    do
    {
        _flash_die_sel(page);
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_page_read(page))
        {
            break;
        }

        if(ERR_SPINAND_ECC_NOT_CORRECTED == (u8_status = _flash_check_page_status()))
        {
            mtd->ecc_stats.failed++;
            spi_nand_msg("ecc failed");
        }

        if (mtd->writesize != _flash_read_from_cache((0 | _flash_offset_wrap(page)), (u32)buf, mtd->writesize))
        {
            break;
        }

        if (ERR_SPINAND_ECC_CORRECTED == u8_status)
        {
            mtd->ecc_stats.corrected++;
        }

        if (oob_required)
        {
            _flash_wait_dev_ready();
            if (mtd->oobsize != _flash_read_from_cache((mtd->writesize | _flash_offset_wrap(page)), (u32)chip->oob_poi, mtd->oobsize))
            {
                break;
            }
        }
        return 0;
    }while(0);

    return -EIO;
}

int spi_nand_ecc_read_subpage(struct mtd_info *mtd, struct nand_chip *chip,
    uint32_t offs, uint32_t len, uint8_t *buf, int page)
{
    u8 u8_status;
    spi_nand_debug("page = 0x%x, offs = 0x%x, len = 0x%x", page ,offs, len);
    do
    {
        _flash_die_sel(page);
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_page_read(page))
        {
            break;
        }

        if(ERR_SPINAND_ECC_NOT_CORRECTED == (u8_status = _flash_check_page_status()))
        {
            mtd->ecc_stats.failed++;
            spi_nand_msg("ecc failed");
        }
        if (len != _flash_read_from_cache((offs | _flash_offset_wrap(page)), (u32)buf + offs, len))
        {
            break;
        }

        if (ERR_SPINAND_ECC_CORRECTED == u8_status)
        {
            mtd->ecc_stats.corrected++;
        }

        return 0;
    }while (0);

    return -EIO;
}

int spi_nand_ecc_write_subpage(struct mtd_info *mtd, struct nand_chip *chip,
                               uint32_t offset, uint32_t data_len,
                               const uint8_t *data_buf, int oob_required)
{
    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_write_enable())
        {
            break;
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_load(0x0 | _flash_offset_wrap(chip->pagebuf),(u8*)data_buf, 0))
        {
            break;
        }
        _flash_wait_dev_ready();
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_random_program_load(offset | _flash_offset_wrap(chip->pagebuf), (u8*)data_buf, data_len))
        {
            break;
        }

        if (oob_required)
        {
            _flash_wait_dev_ready();
            if (ERR_SPINAND_SUCCESS != DRV_SPINAND_random_program_load((mtd->writesize | _flash_offset_wrap(chip->pagebuf)), chip->oob_poi, mtd->oobsize))
            {
                break;
            }
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_execute(chip->pagebuf))
        {
            break;
        }

        if(NAND_STATUS_FAIL == spi_nand_waitfunc(mtd, chip))
        {
            break;
        }

        return 0;
    } while (0);

    return -EIO;
}

int spi_nand_ecc_write_page(struct mtd_info *mtd, struct nand_chip *chip, const uint8_t *buf, int oob_required)
{
    spi_nand_debug("spi_nand_ecc_write_page\r\n");
    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_write_enable())
        {
            break;
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_load(0x0 | _flash_offset_wrap(chip->pagebuf),(u8 *)buf, mtd->writesize))
        {
            break;
        }

        if (oob_required)
        {
            _flash_wait_dev_ready();
            if (ERR_SPINAND_SUCCESS != DRV_SPINAND_random_program_load((mtd->writesize | _flash_offset_wrap(chip->pagebuf)), chip->oob_poi, mtd->oobsize))
            {
                break;
            }
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_execute(chip->pagebuf))
        {
            break;
        }

        if(NAND_STATUS_FAIL == spi_nand_waitfunc(mtd, chip))
        {
            break;
        }
        return 0;
    } while (0);
    return -EIO;
}

static int spi_nand_read_oob_std(struct mtd_info *mtd, struct nand_chip *chip,int page)
{
    _flash_die_sel(page);
    if (ERR_SPINAND_TIMEOUT != DRV_SPINAND_page_read_with_status(page))
    {
        DRV_QSPI_pull_cs(0);
        DRV_SPINAND_cmd_normal_read_from_cache(_flash_offset_wrap(page) | mtd->writesize);
        DRV_SPINAND_receive_data(chip->oob_poi, mtd->oobsize);
        DRV_QSPI_pull_cs(1);
        return 0;
    }
    return -EIO;
}

 static int spi_nand_write_oob_std(struct mtd_info *mtd, struct nand_chip *chip,
                   int page)
{

    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_write_enable())
        {
            break;
        }
        _flash_wait_dev_ready();
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_random_program_load(mtd->writesize | _flash_offset_wrap(page), chip->oob_poi, mtd->oobsize))
        {
            break;
        }
        _flash_wait_dev_ready();
        if(ERR_SPINAND_SUCCESS != DRV_SPINAND_program_execute(page))
        {
            break;
        }
        if(NAND_STATUS_FAIL == spi_nand_waitfunc(mtd, chip))
        {
            break;
        }

        return 0;
    }while(0);
     return -EIO ;
 }


int board_nand_init(struct nand_chip *nand)
{
    u8 u8_dev_id;
    u32 u32_flash_size;

    struct nand_flash_dev *type;
    do
    {
        if (ERR_SPINAND_SUCCESS != DRV_SPINAND_init())
        {
            printf("[FLASH_ERR] init fail!\r\n");
            break;
        }
        if (0 == g_u32_sni_address)
        {
            if (0 == (g_u32_sni_address = (u32)calloc(1, FLASH_SNI_TABLE_SIZE)))
            {
                break;
            }
        }

        g_pst_spinand_sni = (SPINAND_SNI_t *)g_u32_sni_address;
        g_pst_spinand_info = &g_pst_spinand_sni->spinand_info;
        g_pst_spinand_ext_info = &g_pst_spinand_sni->spinand_ext_info;
        g_pst_spinand_ext_configuration = &g_pst_spinand_sni->spinand_ext_configuration;

        if (!_flash_is_sni_header(au8_magicData, FLASH_SNI_HEADER_SIZE, (u8*)g_u32_sni_address))
        {
            if (!_flash_check_sni_available((u32)g_u32_sni_address))
            {
                break;
            }
        }

        if(!_flash_is_sni_header(au8_extMagicData, FLASH_EXT_SNI_HEADER_SIZE, (u8*)(g_pst_spinand_ext_info->au8_magic)))
        {
            printf("[FLASH] Version mismatch!!!\r\n");
            break;
        }

        u32_flash_size = g_pst_spinand_info->u16_BlkPageCnt * g_pst_spinand_info->u16_PageByteCnt * g_pst_spinand_info->u16_BlkCnt;

        u8_dev_id = 0xEE;

        for (type = nand_flash_ids; type->name != NULL; type++)
        {
		    if (u8_dev_id == type->dev_id)
            {
                printf("[FLASH] dev_id = 0x%x\r\n", type->dev_id);
                type->mfr_id = g_pst_spinand_info->au8_ID[0];
                type->dev_id = g_pst_spinand_info->au8_ID[1];
                type->id_len = g_pst_spinand_info->u8_IDByteCnt;
                strncpy((char*)type->id, (const char*)g_pst_spinand_info->au8_ID, g_pst_spinand_info->u8_IDByteCnt);
                type->chipsize = u32_flash_size >> 20;
                type->pagesize = g_pst_spinand_info->u16_PageByteCnt;
                type->oobsize = g_pst_spinand_info->u16_SpareByteCnt;
                type->erasesize = g_pst_spinand_info->u16_BlkPageCnt * g_pst_spinand_info->u16_PageByteCnt;
                type->ecc.strength_ds = g_pst_spinand_info->u16_SectorByteCnt;
                type->ecc.step_ds = g_pst_spinand_info->u16_PageByteCnt / g_pst_spinand_info->u16_SectorByteCnt;
                printf("[FLASH] mfr_id = 0x%x, dev_id= 0x%x id_len = 0x%x\r\n", type->id[0], type->id[1], type->id_len);
                break;
            }
        }
        _flash_setup_by_sni(g_pst_spinand_ext_info);

        if (0 == g_pst_spinand_info->U8RIURead)
        {
            printf("[FLASH] Use BDMA.\r\n");
            DRV_SPINAND_use_bdma(1);
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
        nand->ecc.size = g_pst_spinand_info->u16_SectorByteCnt;
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
        nand->priv = g_pst_spinand_ext_info;
        return 0;
    } while (0);

    if (0 != g_u32_sni_address)
    {
        free((void*)g_u32_sni_address);
        g_u32_sni_address = 0;
    }
    return -1;

}

u8 spi_nand_is_match(u8 *sni_buf)
{
    SPINAND_SNI_t *pst_sni = NULL;
    SPINAND_INFO_t *pst_info = NULL;

    pst_sni = (SPINAND_SNI_t *)sni_buf;
    pst_info = &pst_sni->spinand_info;

    return _flash_is_id_match(pst_info->au8_ID, pst_info->u8_IDByteCnt);
}

int spi_nand_probe(int column)
{
    if (0 == g_u32_sni_address && 0 == (g_u32_sni_address = (u32)calloc(1, FLASH_SNI_TABLE_SIZE)))
    {
        return -1;
    }

    if (!_flash_load_sni(g_u32_sni_address, column))
    {
        return -1;
    }

    nand_init();

    return 0;
}
