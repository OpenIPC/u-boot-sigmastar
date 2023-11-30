#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#   Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib

PATH_C += \
          $(PATH_camdriver)/aio/hal/$(BB_CHIP_ID)/src

PATH_H += \
          $(PATH_camdriver)/aio/hal/$(BB_CHIP_ID)/pub \
          $(PATH_camdriver)/aio/hal/$(BB_CHIP_ID)/inc \
          $(PATH_camdriver)/aio/hal/common

#-------------------------------------------------------------------------------
#   List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
SRC_C_LIST += \
      hal_audio.c \
      hal_audio_os_api_rtk.c \
      hal_audio_sys.c \
      hal_audio_reg.c \
      hal_audio_config.c