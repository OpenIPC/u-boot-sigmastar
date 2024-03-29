/*
 * (C) Copyright 2000-2010
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>
 *
 * (C) Copyright 2008 Atmel Corporation
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <environment.h>
#include <malloc.h>
#include <spi.h>
#include <spi_flash.h>
#include <search.h>
#include <errno.h>
#if defined(CONFIG_MS_NOR_ONEBIN)
#include <jffs2/load_kernel.h>
#include <mdrvParts.h>
#endif
#ifdef CONFIG_MSTAR_ENV_OFFSET
#include <asm/arch/mach/platform.h>
#include <image.h>
#endif
#ifndef CONFIG_ENV_SPI_BUS
# define CONFIG_ENV_SPI_BUS	0
#endif
#ifndef CONFIG_ENV_SPI_CS
# define CONFIG_ENV_SPI_CS	0
#endif
#ifndef CONFIG_ENV_SPI_MAX_HZ
# define CONFIG_ENV_SPI_MAX_HZ	1000000
#endif
#ifndef CONFIG_ENV_SPI_MODE
# define CONFIG_ENV_SPI_MODE	SPI_MODE_3
#endif

#if defined(CONFIG_MS_NOR_ONEBIN)
#ifdef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
u32 ms_nor_env_offset = 0;
#ifdef CONFIG_ENV_OFFSET_REDUND
u32 ms_nor_env_redund_offset = 0;
#endif
#endif


#ifdef CONFIG_ENV_OFFSET_REDUND
static ulong env_offset;
static ulong env_new_offset;

#define ACTIVE_FLAG	1
#define OBSOLETE_FLAG	0
#endif /* CONFIG_ENV_OFFSET_REDUND */
#else
#ifdef CONFIG_ENV_OFFSET_REDUND
static ulong env_offset		= CONFIG_ENV_OFFSET;
static ulong env_new_offset	= CONFIG_ENV_OFFSET_REDUND;

#define ACTIVE_FLAG	1
#define OBSOLETE_FLAG	0
#endif /* CONFIG_ENV_OFFSET_REDUND */
#endif
DECLARE_GLOBAL_DATA_PTR;

char *env_name_spec = "SPI Flash";

static struct spi_flash *env_flash;
#if defined(CONFIG_MS_NOR_ONEBIN)
#if defined(CONFIG_MS_PARTITION) && defined(CONFIG_MSTAR_ENV_OFFSET)
#error "CONFIG_MS_PARTITION and CONFIG_MSTAR_ENV_OFFSET can not be defined the same time"
#endif

#ifdef CONFIG_MS_PARTITION
extern int mtdparts_init(void);
extern int find_dev_and_part(const char *id, struct mtd_device **dev, u8 *part_num, struct part_info **part);

static int get_part(const char *partname, u8 *idx, loff_t *off, loff_t *size,
        loff_t *maxsize)
{
#ifdef CONFIG_CMD_MTDPARTS
    struct mtd_device *dev;
    struct part_info *part;
    u8 pnum;
    int ret;

    ret = mtdparts_init();
    if (ret)
        return ret;

    ret = find_dev_and_part(partname, &dev, &pnum, &part);
    if (ret)
        return ret;

    if (dev->id->type != MTD_DEV_TYPE_NOR) {
        puts("not a NOR device\n");
        return -1;
    }

    *off = part->offset;
    *size = part->size;
    *maxsize = part->size;
    *idx = dev->id->num;

    return 0;
#else
    puts("offset is not a number\n");
    return -1;
#endif
}
#endif

#ifdef CONFIG_MSTAR_ENV_OFFSET
static image_header_t *_get_image_header(void)
{
	return (image_header_t *)(MS_SPI_ADDR+MS_SPI_BOOT_ROM_SIZE);
}

int ms_get_spi_env_offset(void)
{
	int spi_env_offset= MS_SPI_UBOOT_SIZE+MS_SPI_BOOT_ROM_SIZE;

	image_header_t *hdr=_get_image_header();
	if(image_check_magic(hdr))
	{
		spi_env_offset=(((image_get_image_size(hdr) -1 ) / CONFIG_ENV_SECT_SIZE )+1)*CONFIG_ENV_SECT_SIZE + MS_SPI_BOOT_ROM_SIZE;
	}
	else
	{
		printf("Not a img type UBOOT!! Using default spi_env_offset !!\n");
	}

	printf("spi_env_offset=0x%08X\n",spi_env_offset);

	return spi_env_offset;

}
#endif
#endif
#if defined(CONFIG_ENV_OFFSET_REDUND)
int saveenv(void)
{
	env_t	env_new;
	char	*saved_buffer = NULL, flag = OBSOLETE_FLAG;
	u32	saved_size, saved_offset, sector = 1;
	int	ret;

	if (!env_flash) {
		env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
			CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
		if (!env_flash) {
			set_default_env("!spi_flash_probe() failed");
			return 1;
		}
	}

	ret = env_export(&env_new);
	if (ret)
		return ret;
	env_new.flags	= ACTIVE_FLAG;

#if defined(CONFIG_MS_PARTITION) && defined(CONFIG_MS_NOR_ONEBIN)
    u8 idx;
    loff_t u64_env_part_size = 0;
    loff_t u64_env_part_offset = 0;
    PARTS_INFO_t st_partInfo;

    if (!get_part((const char*)ENV_PART_NAME, &idx, &u64_env_part_offset, &u64_env_part_size, &u64_env_part_size))
    {
        printf("%s: get env from mtdpart successful!\r\n", __func__);
    }
    else if (MDRV_PARTS_get_active_part((u8 *)ENV_PART_NAME, &st_partInfo))
    {
        u64_env_part_offset = st_partInfo.u32_Offset;
        u64_env_part_size = st_partInfo.u32_Size;
    }

    CONFIG_ENV_OFFSET = (u32)u64_env_part_offset;

    if (0 == CONFIG_ENV_OFFSET || CONFIG_ENV_SIZE > u64_env_part_size)
    {
        ret = 1;
        goto done;
    }

    if (!get_part((const char*)ENV_REDUND_PART_NAME, &idx, &u64_env_part_offset, &u64_env_part_size, &u64_env_part_size))
    {
        printf("%s: get env from mtdpart successful!\r\n", __func__);
    }
    else if (MDRV_PARTS_get_active_part((u8 *)ENV_REDUND_PART_NAME, &st_partInfo))
    {
        u64_env_part_offset = st_partInfo.u32_Offset;
        u64_env_part_size = st_partInfo.u32_Size;
    }

    CONFIG_ENV_OFFSET_REDUND = (u32)u64_env_part_offset;

    if (0 == CONFIG_ENV_OFFSET_REDUND || CONFIG_ENV_SIZE > u64_env_part_size)
    {
        ret = 1;
        goto done;
    }
#endif

	if (gd->env_valid == 1) {
		env_new_offset = CONFIG_ENV_OFFSET_REDUND;
		env_offset = CONFIG_ENV_OFFSET;
	} else {
		env_new_offset = CONFIG_ENV_OFFSET;
		env_offset = CONFIG_ENV_OFFSET_REDUND;
	}

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		saved_size = CONFIG_ENV_SECT_SIZE - CONFIG_ENV_SIZE;
		saved_offset = env_new_offset + CONFIG_ENV_SIZE;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer) {
			ret = 1;
			goto done;
		}
		ret = spi_flash_read(env_flash, saved_offset,
					saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, env_new_offset,
				sector * CONFIG_ENV_SECT_SIZE);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");

	ret = spi_flash_write(env_flash, env_new_offset,
		CONFIG_ENV_SIZE, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset,
					saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = spi_flash_write(env_flash, env_offset + offsetof(env_t, flags),
				sizeof(env_new.flags), &flag);
	if (ret)
		goto done;

	puts("done\n");

	gd->env_valid = gd->env_valid == 2 ? 1 : 2;

	printf("Valid environment: %d\n", (int)gd->env_valid);

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}

void env_relocate_spec(void)
{
	int ret;
	int crc1_ok = 0, crc2_ok = 0;
	env_t *tmp_env1 = NULL;
	env_t *tmp_env2 = NULL;
	env_t *ep = NULL;

	tmp_env1 = (env_t *)malloc(CONFIG_ENV_SIZE);
	tmp_env2 = (env_t *)malloc(CONFIG_ENV_SIZE);

	if (!tmp_env1 || !tmp_env2) {
		set_default_env("!malloc() failed");
		goto out;
	}

	env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
	if (!env_flash) {
		set_default_env("!spi_flash_probe() failed");
		goto out;
	}

#if defined(CONFIG_MS_PARTITION) && defined(CONFIG_MS_NOR_ONEBIN)
    u32 u32_env_part_size;
    PARTS_INFO_t st_partInfo;

    if (!MDRV_PARTS_get_active_part(ENV_PART_NAME, &st_partInfo))
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }

    CONFIG_ENV_OFFSET = st_partInfo.u32_Offset;
    u32_env_part_size = st_partInfo.u32_Size;
    printf("%s: offset = 0x%x size = 0x%x\r\n", ENV_PART_NAME, CONFIG_ENV_OFFSET, u32_env_part_size);

    if (CONFIG_ENV_SIZE > u32_env_part_size)
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }

    if (!MDRV_PARTS_get_active_part(ENV_REDUND_PART_NAME, &st_partInfo))
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }

    CONFIG_ENV_OFFSET_REDUND = st_partInfo.u32_Offset;
    u32_env_part_size = st_partInfo.u32_Size;
    printf("%s: offset = 0x%x size = 0x%x\r\n", ENV_REDUND_PART_NAME, CONFIG_ENV_OFFSET_REDUND, u32_env_part_size);

    if (CONFIG_ENV_SIZE > u32_env_part_size)
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }
#endif

	ret = spi_flash_read(env_flash, CONFIG_ENV_OFFSET,
				CONFIG_ENV_SIZE, tmp_env1);
	if (ret) {
		set_default_env("!spi_flash_read() failed");
		goto err_read;
	}

	if (crc32(0, tmp_env1->data, ENV_SIZE) == tmp_env1->crc)
		crc1_ok = 1;

	ret = spi_flash_read(env_flash, CONFIG_ENV_OFFSET_REDUND,
				CONFIG_ENV_SIZE, tmp_env2);
	if (!ret) {
		if (crc32(0, tmp_env2->data, ENV_SIZE) == tmp_env2->crc)
			crc2_ok = 1;
	}

	if (!crc1_ok && !crc2_ok) {
		set_default_env("!bad CRC");
		goto err_read;
	} else if (crc1_ok && !crc2_ok) {
		gd->env_valid = 1;
	} else if (!crc1_ok && crc2_ok) {
		gd->env_valid = 2;
	} else if (tmp_env1->flags == ACTIVE_FLAG &&
		   tmp_env2->flags == OBSOLETE_FLAG) {
		gd->env_valid = 1;
	} else if (tmp_env1->flags == OBSOLETE_FLAG &&
		   tmp_env2->flags == ACTIVE_FLAG) {
		gd->env_valid = 2;
	} else if (tmp_env1->flags == tmp_env2->flags) {
		gd->env_valid = 2;
	} else if (tmp_env1->flags == 0xFF) {
		gd->env_valid = 2;
	} else {
		/*
		 * this differs from code in env_flash.c, but I think a sane
		 * default path is desirable.
		 */
		gd->env_valid = 2;
	}

	if (gd->env_valid == 1)
		ep = tmp_env1;
	else
		ep = tmp_env2;

	ret = env_import((char *)ep, 0);
	if (!ret) {
		error("Cannot import environment: errno = %d\n", errno);
		set_default_env("env_import failed");
	}

err_read:
	spi_flash_free(env_flash);
	env_flash = NULL;
out:
	free(tmp_env1);
	free(tmp_env2);
}
#elif defined(CONFIG_MS_PARTITION) && !defined(CONFIG_MS_NOR_ONEBIN)
#include "drivers/mstar/partition/part_mxp.h"
extern int mxp_init_nor_flash(void);

int mxp_get_env(int* offset, int* size)
{
    char strENVName[] = "UBOOT_ENV";
	int idx;
    *offset = CONFIG_ENV_OFFSET;
    *size = CONFIG_ENV_SIZE;
    int ret=0;

    if((ret=mxp_init_nor_flash())<0)
    {
        return -1;
    }

    mxp_load_table();
    idx=mxp_get_record_index(strENVName);
    if(idx>=0)
    {
        mxp_record rec;
        if(0==mxp_get_record_by_index(idx,&rec))
        {
            *offset = rec.start;
            *size = rec.size;
        }
        else
        {
            printf("failed to get MXP record with name: %s\n", strENVName);
            return -1;
        }


    }
    else
    {
        printf("can not found mxp record: %s\n", strENVName);
        return -1;
    }


	printf("env_offset=0x%X env_size=0x%X\n", *offset, *size);

	return 0;

}

int saveenv(void)
{
	int	saved_size, saved_offset;
	char	*saved_buffer = NULL;
	int	ret = 1;
	env_t	env_new;
	int mxp_env_offset, mxp_env_size;

    mxp_get_env(&mxp_env_offset, &mxp_env_size);


	if (!env_flash) {
		env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
			CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
		if (!env_flash) {
			set_default_env("!spi_flash_probe() failed");
			return 1;
		}
	}

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > mxp_env_size) {
		saved_size = CONFIG_ENV_SECT_SIZE - mxp_env_size;
		saved_offset = mxp_env_offset + mxp_env_size;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer)
			goto done;

		ret = spi_flash_read(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}
    else
    {
		saved_size = CONFIG_ENV_SECT_SIZE - mxp_env_size;
		saved_offset = mxp_env_offset + mxp_env_size;
    }


	ret = env_export(&env_new);
	if (ret)
		goto done;

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, mxp_env_offset,
		mxp_env_size);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");
	ret = spi_flash_write(env_flash, mxp_env_offset,
		mxp_env_size, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > mxp_env_size) {
		ret = spi_flash_write(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = 0;
	puts("done\n");

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}

void env_relocate_spec(void)
{
	int ret;
	char *buf = NULL;

	int mxp_env_offset, mxp_env_size;
    mxp_get_env(&mxp_env_offset, &mxp_env_size);


	buf = (char *)malloc(mxp_env_size);
	env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
	if (!env_flash) {
		set_default_env("!spi_flash_probe() failed");
		if (buf)
			free(buf);
		return;
	}

	ret = spi_flash_read(env_flash,
		mxp_env_offset, mxp_env_size, buf);
	if (ret) {
		set_default_env("!spi_flash_read() failed");
		goto out;
	}

	ret = env_import(buf, 1);
	if (ret)
		gd->env_valid = 1;
out:
	spi_flash_free(env_flash);
	if (buf)
		free(buf);
	env_flash = NULL;
}

#elif defined(CONFIG_MSTAR_ENV_OFFSET)


static image_header_t *_get_image_header(void)
{
	return (image_header_t *)(MS_SPI_ADDR+MS_SPI_BOOT_ROM_SIZE);
}


int ms_get_spi_env_offset(void)
{
	int spi_env_offset= MS_SPI_UBOOT_SIZE+MS_SPI_BOOT_ROM_SIZE;

	image_header_t *hdr=_get_image_header();
	if(image_check_magic(hdr))
	{
		spi_env_offset=(((image_get_image_size(hdr) -1 ) / CONFIG_ENV_SECT_SIZE )+1)*CONFIG_ENV_SECT_SIZE + MS_SPI_BOOT_ROM_SIZE;
	}
	else
	{
		printf("Not a img type UBOOT!! Using default spi_env_offset !!\n");
	}

	printf("spi_env_offset=0x%08X\n",spi_env_offset);

	return spi_env_offset;

}


int saveenv(void)
{
	u32	saved_size, saved_offset, sector = 1;
	char	*saved_buffer = NULL;
	int	ret = 1;
	env_t	env_new;
	int CONFIG_ENV_OFFSET= ms_get_spi_env_offset();

	if (!env_flash) {
		env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
			CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
		if (!env_flash) {
			set_default_env("!spi_flash_probe() failed");
			return 1;
		}
	}

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		saved_size = CONFIG_ENV_SECT_SIZE - CONFIG_ENV_SIZE;
		saved_offset = CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer)
			goto done;

		ret = spi_flash_read(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}

	ret = env_export(&env_new);
	if (ret)
		goto done;

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, CONFIG_ENV_OFFSET,
		sector * CONFIG_ENV_SECT_SIZE);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");
	ret = spi_flash_write(env_flash, CONFIG_ENV_OFFSET,
		CONFIG_ENV_SIZE, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = 0;
	puts("done\n");

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}

void env_relocate_spec(void)
{
	int ret;
	char *buf = NULL;
	int CONFIG_ENV_OFFSET= ms_get_spi_env_offset();

	buf = (char *)malloc(CONFIG_ENV_SIZE);
	env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
	if (!env_flash) {
		set_default_env("!spi_flash_probe() failed");
		if (buf)
			free(buf);
		return;
	}

	ret = spi_flash_read(env_flash,
		CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE, buf);
	if (ret) {
		set_default_env("!spi_flash_read() failed");
		goto out;
	}

	ret = env_import(buf, 1);
	if (ret)
		gd->env_valid = 1;
out:
	spi_flash_free(env_flash);
	if (buf)
		free(buf);
	env_flash = NULL;
}

#else

int saveenv(void)
{
	u32	saved_size, saved_offset, sector = 1;
	char	*saved_buffer = NULL;
	int	ret = 1;
	env_t	env_new;

	if (!env_flash) {
		env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS,
			CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
		if (!env_flash) {
			set_default_env("!spi_flash_probe() failed");
			return 1;
		}
	}

#if defined(CONFIG_MS_PARTITION) && defined(CONFIG_MS_NOR_ONEBIN)
    u8 idx;
    loff_t u64_env_part_size = 0;
    loff_t u64_env_part_offset = 0;
    PARTS_INFO_t st_partInfo;

    if (!get_part((const char*)ENV_PART_NAME, &idx, &u64_env_part_offset, &u64_env_part_size, &u64_env_part_size))
    {
        printf("%s: get env from mtdpart successful!\r\n", __func__);
    }
    else if (MDRV_PARTS_get_active_part((u8 *)ENV_PART_NAME, &st_partInfo))
    {
        u64_env_part_offset = st_partInfo.u32_Offset;
        u64_env_part_size = st_partInfo.u32_Size;
    }

    CONFIG_ENV_OFFSET = (u32)u64_env_part_offset;

    if (0 == CONFIG_ENV_OFFSET || CONFIG_ENV_SIZE > u64_env_part_size)
    {
        ret = 1;
        goto done;
    }
#endif

#if defined(CONFIG_MSTAR_ENV_OFFSET) && defined(CONFIG_MS_NOR_ONEBIN)
    CONFIG_ENV_OFFSET = ms_get_spi_env_offset();
#endif

	/* Is the sector larger than the env (i.e. embedded) */
	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		saved_size = CONFIG_ENV_SECT_SIZE - CONFIG_ENV_SIZE;
		saved_offset = CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE;
		saved_buffer = malloc(saved_size);
		if (!saved_buffer)
			goto done;

		ret = spi_flash_read(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	if (CONFIG_ENV_SIZE > CONFIG_ENV_SECT_SIZE) {
		sector = CONFIG_ENV_SIZE / CONFIG_ENV_SECT_SIZE;
		if (CONFIG_ENV_SIZE % CONFIG_ENV_SECT_SIZE)
			sector++;
	}

	ret = env_export(&env_new);
	if (ret)
		goto done;

	puts("Erasing SPI flash...");
	ret = spi_flash_erase(env_flash, CONFIG_ENV_OFFSET,
		sector * CONFIG_ENV_SECT_SIZE);
	if (ret)
		goto done;

	puts("Writing to SPI flash...");
	ret = spi_flash_write(env_flash, CONFIG_ENV_OFFSET,
		CONFIG_ENV_SIZE, &env_new);
	if (ret)
		goto done;

	if (CONFIG_ENV_SECT_SIZE > CONFIG_ENV_SIZE) {
		ret = spi_flash_write(env_flash, saved_offset,
			saved_size, saved_buffer);
		if (ret)
			goto done;
	}

	ret = 0;
	puts("done\n");

 done:
	if (saved_buffer)
		free(saved_buffer);

	return ret;
}

void env_relocate_spec(void)
{
	int ret;
	char *buf = NULL;

	buf = (char *)malloc(CONFIG_ENV_SIZE);
	env_flash = spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS,
			CONFIG_ENV_SPI_MAX_HZ, CONFIG_ENV_SPI_MODE);
	if (!env_flash) {
		set_default_env("!spi_flash_probe() failed");
		if (buf)
			free(buf);
		return;
	}

#if defined(CONFIG_MS_PARTITION) && defined(CONFIG_MS_NOR_ONEBIN)
    u32 u32_env_part_size;
    PARTS_INFO_t st_partInfo;

    if (!MDRV_PARTS_get_active_part((u8 *)ENV_PART_NAME, &st_partInfo))
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }

    CONFIG_ENV_OFFSET = st_partInfo.u32_Offset;
    u32_env_part_size = st_partInfo.u32_Size;
    printf("%s: offset = 0x%x size = 0x%x\r\n", ENV_PART_NAME, CONFIG_ENV_OFFSET, u32_env_part_size);

    if (CONFIG_ENV_SIZE > u32_env_part_size)
    {
        set_default_env("CONFIG_ENV_SIZE > env partition size.");
        goto out;
    }
#endif

#if defined(CONFIG_MSTAR_ENV_OFFSET) && defined(CONFIG_MS_NOR_ONEBIN)
    CONFIG_ENV_OFFSET = ms_get_spi_env_offset();
#endif
	ret = spi_flash_read(env_flash,
		CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE, buf);
	if (ret) {
		set_default_env("!spi_flash_read() failed");
		goto out;
	}

	ret = env_import(buf, 1);
	if (ret)
		gd->env_valid = 1;
out:
	spi_flash_free(env_flash);
	if (buf)
		free(buf);
	env_flash = NULL;
}
#endif

int env_init(void)
{
	/* SPI flash isn't usable before relocation */
	gd->env_addr = (ulong)&default_environment[0];
	gd->env_valid = 1;

	return 0;
}
