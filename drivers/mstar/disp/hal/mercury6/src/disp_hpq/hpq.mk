#
# Makefile for kernel test
#

INC_PATH += -I$(DIR_PATH)//hal/mercury6/src/disp_hpq/inc/pq
INC_PATH += -I$(DIR_PATH)//hal/mercury6/src/disp_hpq/inc/linux
INC_PATH += -I$(DIR_PATH)//hal/mercury6/src/disp_hpq/inc
INC_PATH += -I$(DIR_PATH)//hal/mercury6/inc

$(MODULE_NAME)-objs +=  .//hal/mercury6/src/disp_hpq/drv_scl_pq_bin.o
$(MODULE_NAME)-objs +=  .//hal/mercury6/src/disp_hpq/drv_scl_pq.o
$(MODULE_NAME)-objs +=  .//hal/mercury6/src/disp_hpq/hal_scl_pq.o


$(MODULE_NAME)-objs +=  .//hal/mercury6/src/disp_hpq/hal_disp_hpq.o