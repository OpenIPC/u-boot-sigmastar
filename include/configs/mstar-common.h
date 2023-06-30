/*------------------------------------------------------------------------------
    Constant
-------------------------------------------------------------------------------*/
/*#define CONFIG_SYS_ARCH_TIMER   1*/
#define CONFIG_MS_PIU_TIMER   1
#define CONFIG_AUTOBOOT_KEYED
#define CONFIG_AUTOBOOT_DELAY_STR "\x0d" /* press ENTER to interrupt BOOT */

#define CONFIG_DISPLAY_BOARDINFO    1

#define CONFIG_BOARD_LATE_INIT

/*------------------------------------------------------------------------------
    Macro
-------------------------------------------------------------------------------*/
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_SECT_SIZE
#undef CONFIG_ENV_SIZE
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_SYS_CBSIZE
#undef CONFIG_SYS_MAXARGS
#undef CONFIG_SYS_PROMPT

/* boot delay time */
#define CONFIG_BOOTDELAY	0
#define CONFIG_ZERO_BOOTDELAY_CHECK

#define CONFIG_HUSH_PARSER 1

#define CONFIG_SYS_LONGHELP	            /* undef to save memory     */
#define CONFIG_SYS_PROMPT	"OpenIPC # "	/* Monitor Command Prompt   */
#define CONFIG_SYS_CBSIZE	1024		        /* Console I/O Buffer Size  */

/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	        (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	        64		    /* max number of command args   */
#define CONFIG_SYS_BARGSIZE	    CONFIG_SYS_CBSIZE	/* Boot Argument Buffer Size    */

#define CONFIG_CMDLINE_TAG       1    /* enable passing of ATAGs */
#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_INITRD_TAG        1

#define CONFIG_CMDLINE_EDITING 1
#define CONFIG_AUTO_COMPLETE

#define CONFIG_SYS_NO_FLASH 			   1

#ifdef CONFIG_MS_ISP_FLASH
#define CONFIG_CMD_SF
#ifdef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#define CONFIG_ENV_IS_IN_SPI_FLASH
#define CONFIG_ENV_SECT_SIZE    0x10000
#define CONFIG_ENV_SIZE         0x10000	/* Total Size of Environment Sector */
/* bottom 4KB of available space in Uboot */
/* 0x40000 reserved for UBoot, 0x40000 maximum storage size of uboot */
#define CONFIG_ENV_OFFSET       0x40000
#endif

#define CONFIG_VERSION_VARIABLE

#ifndef PRODUCT_NAME
#define PRODUCT_NAME "ssc335"
#endif

#define CONFIG_BOOTARGS "mem=\\${osmem} console=ttyS0,115200 panic=20 root=/dev/mtdblock3 init=/init \\${mtdparts} LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz}"
#define CONFIG_BOOTCOMMAND "setenv bootcmd run setflash; saveenv; reset"

#define CONFIG_EXTRA_ENV_SETTINGS \
    "osmem=32M\0" \
    "baseaddr=0x21000000\0" \
    "uknor8m=mw.b ${baseaddr} 0xFF 1000000; ${updatetool} ${baseaddr} uImage.${soc}; sf probe 0; sf erase 0x50000 0x200000; sf write ${baseaddr} 0x50000 ${filesize}\0" \
    "uknor16m=mw.b ${baseaddr} 0xFF 1000000; ${updatetool} ${baseaddr} uImage.${soc}; sf probe 0; sf erase 0x50000 0x300000; sf write ${baseaddr} 0x50000 ${filesize}\0" \
    "urnor8m=mw.b ${baseaddr} 0xFF 1000000; ${updatetool} ${baseaddr} rootfs.squashfs.${soc}; sf probe 0; sf erase 0x250000 0x500000; sf write ${baseaddr} 0x250000 ${filesize}\0" \
    "urnor16m=mw.b ${baseaddr} 0xFF 1000000; ${updatetool} ${baseaddr} rootfs.squashfs.${soc}; sf probe 0; sf erase 0x350000 0xA00000; sf write ${baseaddr} 0x350000 ${filesize}\0" \
    "mtdpartsnor8m=setenv mtdparts mtdparts=NOR_FLASH:256k(boot),64k(env),2048k(kernel),5120k(rootfs),-(rootfs_data)\0" \
    "mtdpartsnor16m=setenv mtdparts mtdparts=NOR_FLASH:256k(boot),64k(env),3072k(kernel),10240k(rootfs),-(rootfs_data)\0" \
    "bootcmdnor=setenv setargs setenv bootargs ${bootargs}; run setargs; sf probe 0; sf read ${baseaddr} 0x50000 0x300000; bootm ${baseaddr}\0" \
    "setnor8m=run mtdpartsnor8m; setenv bootcmd ${bootcmdnor}; saveenv; run bootcmd\0" \
    "setnor16m=run mtdpartsnor16m; setenv bootcmd ${bootcmdnor}; saveenv; run bootcmd\0" \
    "setsdcard=setenv updatetool fatload mmc 0\0" \
    "updatetool=tftpboot\0" \
    "soc=" __stringify(PRODUCT_NAME)

#endif

#ifdef CONFIG_MS_SDMMC
#define CONFIG_CMD_FAT
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_DOS_PARTITION
#define CONFIG_MS_SDMMC_MAX_READ_BLOCKS 1024
#endif

#ifdef CONFIG_MS_SPINAND
#undef CONFIG_BOOTARGS
#undef CONFIG_BOOTCOMMAND
#undef CONFIG_ENV_RANGE
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef MTDPARTS_DEFAULT

#if defined(CONFIG_MS_SAVE_ENV_IN_NAND_FLASH)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET CONFIG_MSTAR_ENV_NAND_OFFSET
#define CONFIG_MSTAR_ENV_NAND_OFFSET ms_nand_env_offset
#define CONFIG_ENV_RANGE 0x40000
#define CONFIG_ENV_SIZE 0x20000
#endif

#define CONFIG_CMD_SPINAND_CIS
#define CONFIG_CMD_UBI
#define CONFIG_UBI_MWRITE
#define CONFIG_SYS_DCACHE_OFF

#ifdef CONFIG_MS_SAVE_ENV_IN_NAND_FLASH
#define CONFIG_BOOTARGS "mem=\\${osmem} console=ttyS0,115200 panic=20 root=/dev/mtdblock5 init=/init ubi.mtd=ubi \\${mtdparts} LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz}"
#define CONFIG_BOOTCOMMAND "setenv bootcmd ${bootcmdnand}; saveenv; run bootcmd"
#define MTDPARTS_DEFAULT "mtdparts=nand0:1024k(boot0),1024k(boot1),256k(env),-(ubi)"
#else
#define CONFIG_BOOTARGS "mem=\\${osmem} console=ttyS0,115200 panic=20 \\${mtdparts} LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz}"
#define CONFIG_BOOTCOMMAND "setenv setargs setenv bootargs ${bootargs}; run setargs; fatload mmc 0 ${baseaddr} uImage.${soc}; bootm ${baseaddr}"
#define MTDPARTS_DEFAULT "mtdparts=nand0:-(nand)"
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"osmem=32M\0" \
	"baseaddr=0x21000000\0" \
	"bootcmdnand=setenv setargs setenv bootargs ${bootargs}; run setargs; ubi part ubi; ubi read ${baseaddr} kernel; bootm ${baseaddr}\0" \
	"urnand=mw.b ${baseaddr} 0xFF 0x1000000; ${updatetool} ${baseaddr} rootfs.ubi.${soc}; nand erase 0x240000 0x7DC0000; nand write ${baseaddr} 0x240000 ${filesize}\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"setsdcard=setenv updatetool fatload mmc 0\0" \
	"updatetool=tftpboot\0" \
	"soc=" __stringify(PRODUCT_NAME)

#define CONFIG_CMD_NAND

#ifdef CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0 /* not actually used */
#endif /*CONFIG_CMD_NAND*/

#endif /*CONFIG_MS_SPINAND*/

#ifdef CONFIG_MS_NAND

#if defined(CONFIG_MS_SAVE_ENV_IN_NAND_FLASH)
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET       CONFIG_MSTAR_ENV_NAND_OFFSET
#define CONFIG_MSTAR_ENV_NAND_OFFSET ms_nand_env_offset
#define CONFIG_ENV_SIZE         0x00020000
#endif

#define CONFIG_CMD_UBI
/* #define CONFIG_CMD_UBIFS */
#define CONFIG_UBI_MWRITE

#define MTDIDS_DEFAULT			"nand0=nand0"    /* "nor0=physmap-flash.0,nand0=nand" */
#define MTDPARTS_DEFAULT		"mtdparts=nand0:0xC0000@0x140000(NPT),-(UBI)"  /* must be different from real partition to test NAND partition function */

#define CONFIG_EXTRA_ENV_SETTINGS                              \
       "mtdids=" MTDIDS_DEFAULT "\0"                           \
       "mtdparts=" MTDPARTS_DEFAULT "\0"                       \
       "partition=nand0,0\0"                                   \
       ""

#define CONFIG_CMD_NAND
#define CONFIG_CMD_CIS

#ifdef CONFIG_CMD_NAND
#define CONFIG_SYS_NAND_MAX_CHIPS	1
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define CONFIG_SYS_NAND_BASE		0 /* not actually used */
#endif

#endif

#ifdef CONFIG_MS_EMMC
#ifndef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#define CONFIG_ENV_IS_IN_MMC       1
#define CONFIG_SYS_MMC_ENV_DEV     1
#define CONFIG_MS_EMMC_DEV_INDEX   1
#define CONFIG_EMMC_PARTITION
#define CONFIG_UNLZO_DST_ADDR  0x24000000
#define CONFIG_ENV_SIZE         0x00020000
#endif
#endif

#if defined(CONFIG_ENV_IS_IN_NAND) || defined(CONFIG_ENV_IS_IN_MMC) || defined(CONFIG_ENV_IS_IN_SPI_FLASH)

#define CONFIG_CMD_SAVEENV	/* saveenv */
#else
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE         0x00020000	/* Total Size of Environment Sector */
#define CONFIG_ENV_RANGE        0x00060000

#endif

#if defined(CONFIG_MS_NAND) || defined(CONFIG_MS_MTD_ISP_FLASH) || defined(CONFIG_MS_SPINAND)
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE               /* needed for mtdparts commands */
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS
#endif


/* CMD */

#define CONFIG_CMD_CACHE
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_RUN		/* run command in env variable	*/
#define CONFIG_AUTOSCRIPT

/*
 * USB configuration
 */
#ifdef CONFIG_MS_USB

#define     CONFIG_USB
#define     CONFIG_CMD_USB
#define     CONFIG_USB_STORAGE

#ifdef CONFIG_MS_ENABLE_USB_LAN_MODULE
#define ENABLE_USB_LAN_MODULE
#define CONFIG_CMD_NET
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_PING
#define CONFIG_BOOTP_GATEWAY
#endif

#endif


 /* Ethernet configuration */
#ifdef CONFIG_MS_EMAC
#define CONFIG_MINIUBOOT
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_TFTP_PORT
#endif

#define CONFIG_XZ
#define CONFIG_MZ

#define XZ_DICT_ADDR            0x21000000  /*used for XZ decompress*/
#define XZ_DICT_LENGTH          0x01000000

/* I2C configuration */
#define CONFIG_CMD_I2C
#define CONFIG_SYS_I2C
#define CONFIG_I2C_MULTI_BUS
#define CONFIG_SYS_I2C_MSTAR
#define I2C_MS_DECLARATIONS2
#define CONFIG_SYS_I2C_MS_SPEED (300000)
#define CONFIG_SYS_I2C_MS_SLAVE (0x7F)
/*
 * padmux: 1 -> PAD_I2C0_SCL, PAD_I2C0_SDA
 *         2 -> PAD_PWM0, PAD_PWM1
 *         3 -> PAD_SR_IO00, PAD_SR_IO01
 */
#define CONFIG_SYS_I2C_0_PADMUX (0x1)
/*
 * padmux: 1 -> PAD_I2C1_SCL, PAD_I2C1_SDA
 *         2 -> PAD_PWM0, PAD_PWM1
 *         3 -> PAD_SR_IO00, PAD_SR_IO01
 */
#define CONFIG_SYS_I2C_1_PADMUX (0x1)

/* EEPROM */
#define CONFIG_CMD_EEPROM
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN (2)
#define CONFIG_SYS_I2C_EEPROM_ADDR (0x54)

/*
#define ENABLE_DOUBLE_SYSTEM_CHECK  1
*/

/* SENSOR */
#define CONFIG_MS_SRCFG

#define CONFIG_NETMASK 255.255.255.0
#define CONFIG_GATEWAYIP 192.168.1.1
#define CONFIG_SERVERIP 192.168.1.1
#define CONFIG_IPADDR 192.168.1.10
