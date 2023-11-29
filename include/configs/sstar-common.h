#undef CONFIG_SYS_CBSIZE
#undef CONFIG_SYS_LOAD_ADDR
#undef CONFIG_SYS_PROMPT

#define CONFIG_SYS_PROMPT "OpenIPC # "
#define CONFIG_SYS_CBSIZE 1024
#define CONFIG_SYS_LOAD_ADDR 0x21000000

#define BOOT_SCRIPT0 "fatload mmc 0 ${baseaddr} boot.scr"
#define BOOT_SCRIPT1 "fatload mmc 1 ${baseaddr} boot.scr"

#define CONFIG_SERVERIP 192.168.1.1
#define CONFIG_GATEWAYIP 192.168.1.1
#define CONFIG_IPADDR 192.168.1.10
#define CONFIG_NETMASK 255.255.255.0

#define CONFIG_CMD_ECHO
#define CONFIG_CMD_LOADB
#define CONFIG_CMD_SOURCE
#define CONFIG_FAT_WRITE
#define CONFIG_SPL_YMODEM_SUPPORT
#define CONFIG_VERSION_VARIABLE

#ifdef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#undef CONFIG_ENV_SIZE
#define CONFIG_ENV_SIZE 0x10000

#ifndef CONFIG_MS_NOR_ONEBIN
#undef CONFIG_ENV_OFFSET
#define CONFIG_ENV_OFFSET 0x40000
#endif

#define CONFIG_BOOTARGS "console=ttyS0,115200 panic=20 root=/dev/mtdblock3 init=/init mtdparts=NOR_FLASH:256k(boot),64k(env),2048k(kernel),\\${rootmtd}(rootfs),-(rootfs_data) LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz} cma=2M"
#define CONFIG_BOOTCOMMAND "setenv bootcmd ${bootcmdnor}; sf probe 0; saveenv; run bootcmd"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"baseaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"kernaddr=0x50000\0" \
	"kernsize=0x200000\0" \
	"rootaddr=0x250000\0" \
	"rootsize=0x500000\0" \
	"rootmtd=5120k\0" \
	"bootcmdnor=sf probe 0; setenv setargs setenv bootargs ${bootargs}; run setargs; sf read ${baseaddr} ${kernaddr} ${kernsize}; bootm ${baseaddr}\0" \
	"ubnor=sf probe 0; sf erase 0x0 ${kernaddr}; sf write ${baseaddr} 0x0 ${kernaddr}\0" \
	"uknor=mw.b ${baseaddr} 0xFF 0x1000000; ${updatetool} ${baseaddr} uImage.${soc}; sf probe 0; sf erase ${kernaddr} ${kernsize}; sf write ${baseaddr} ${kernaddr} ${filesize}\0" \
	"urnor=mw.b ${baseaddr} 0xFF 0x1000000; ${updatetool} ${baseaddr} rootfs.squashfs.${soc}; sf probe 0; sf erase ${rootaddr} ${rootsize}; sf write ${baseaddr} ${rootaddr} ${filesize}\0" \
	"updatetool=tftpboot\0" \
	"soc=" __stringify(PRODUCT_NAME)
#endif

#ifdef CONFIG_MS_SPINAND
#undef MTDPARTS_DEFAULT
#undef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_RANGE
#undef CONFIG_ENV_SIZE

#define CONFIG_SYS_DCACHE_OFF
#define CONFIG_ENV_OFFSET CONFIG_MSTAR_ENV_NAND_OFFSET
#define CONFIG_ENV_RANGE 0x40000
#define CONFIG_ENV_SIZE 0x20000

#ifdef CONFIG_MS_SAVE_ENV_IN_NAND_FLASH
#define CONFIG_BOOTARGS "console=ttyS0,115200 panic=20 root=/dev/mtdblock5 init=/init ubi.mtd=ubi \\${mtdparts} LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz} cma=2M"
#define CONFIG_BOOTCOMMAND "setenv bootcmd ${bootcmdnand}; saveenv; run bootcmd"
#define MTDPARTS_DEFAULT "mtdparts=nand0:1024k(boot0),1024k(boot1),256k(env),-(ubi)"
#else
#define CONFIG_BOOTARGS "console=ttyS0,115200 panic=20 \\${mtdparts} LX_MEM=\\${memlx} mma_heap=mma_heap_name0,miu=0,sz=\\${memsz} cma=2M"
#define CONFIG_BOOTCOMMAND "setenv setargs setenv bootargs ${bootargs}; run setargs; fatload mmc 0 ${baseaddr} uImage.${soc}; bootm ${baseaddr}"
#define MTDPARTS_DEFAULT "mtdparts=nand0:-(nand)"
#define CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_EXTRA_ENV_SETTINGS \
	"baseaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"rootaddr=0x240000\0" \
	"rootsize=0x7DC0000\0" \
	"bootcmdnand=setenv setargs setenv bootargs ${bootargs}; run setargs; ubi part ubi; ubi read ${baseaddr} kernel; bootm ${baseaddr}\0" \
	"ubnand=nand erase 0x0 ${rootaddr}; nand write ${baseaddr} 0x0 ${rootaddr}\0" \
	"urnand=mw.b ${baseaddr} 0xFF 0x1000000; ${updatetool} ${baseaddr} rootfs.ubi.${soc}; nand erase ${rootaddr} ${rootsize}; nand write ${baseaddr} ${rootaddr} ${filesize}\0" \
	"mtdparts=" MTDPARTS_DEFAULT "\0" \
	"updatetool=tftpboot\0" \
	"soc=" __stringify(PRODUCT_NAME)
#endif
