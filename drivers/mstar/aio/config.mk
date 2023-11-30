include raw-impl-config.mk
SS_ARCH_NAME:=$(subst ",,$(CONFIG_SSTAR_CHIP_NAME))
DEPS:=common
ENV_INC_PATH$(os_rtos):=system/MsWrapper system/fc system/rtk system/sys driver/drv/timer driver/hal/$(CHIP)/timer driver/drv/int driver/hal/$(CHIP)/int system/trc driver/drv/clkgen driver/hal/$(CHIP)/clkgen driver/drv/rtc driver/hal/$(CHIP)/kernel

DRV_PUB_INCS:=pub/mhal_audio.h pub/mhal_audio_datatype.h
DRV_INC_PATH:=pub inc

DRV_SRCS:=src/common/mhal_audio.c
DRV_SRCS+=src/common/drv_audio.c
DRV_SRCS$(os_linux)+=src/linux/drv_audio_api_linux.c
DRV_SRCS$(os_linux)+=src/linux/drv_audio_export.c
DRV_SRCS$(os_linux)+=src/linux/audio_proc.c
ifneq ($(filter rtos,$(IPC_ROUTER)),)
DRV_SRCS$(os_rtos)+=src/rtk/drv_audio_api_rtk.c
DRV_SRCS$(os_rtos)+=src/rtk/isw_audio.c
DRV_SRCS$(os_rtos)+=src/rtk/isw_audio_data.c
endif

HAL_PUB_PATH:=$(SS_ARCH_NAME)/pub common
HAL_INC_PATH:=$(SS_ARCH_NAME)/inc

HAL_SRCS:=$(SS_ARCH_NAME)/src/hal_audio.c
HAL_SRCS+=$(SS_ARCH_NAME)/src/hal_audio_os_api_linux.c
HAL_SRCS+=$(SS_ARCH_NAME)/src/hal_audio_sys.c
HAL_SRCS+=$(SS_ARCH_NAME)/src/hal_audio_reg.c
HAL_SRCS+=$(SS_ARCH_NAME)/src/hal_audio_config.c

include add-config.mk
