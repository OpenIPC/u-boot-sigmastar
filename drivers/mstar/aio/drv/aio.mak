#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#   Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib

PATH_C += \
	$(PATH_camdriver)/aio/drv/src/common \
	$(PATH_camdriver)/aio/drv/src/rtk \
	$(PATH_camdriver)/aio/ut/rtk

PATH_H += \
	$(PATH_camdriver)/aio/drv/pub \
	$(PATH_camdriver)/aio/drv/inc \
	$(PATH_camdriver)/aio/drv/src/rtk/board_cfg/$(BB_CHIP_ID) \
	$(PATH_driver)/hal/infinity/int/pub \
	$(PATH_system)/MsWrapper/pub

#-------------------------------------------------------------------------------
#   List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
SRC_C_LIST += \
	abi_headers_aio.c \
	mhal_audio.c \
	drv_audio.c \
	drv_audio_api_rtk.c \
	isw_audio.c \
	isw_audio_data.c \
	audio_proc.c \
	rtk_dts_api.c
