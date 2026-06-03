# dtb-$(CONFIG_INFINITY6B0) += infinity6b0-fpga.dtb \
# 	infinity6b0-ssc009a-s01a.dtb \
# 	infinity6b0-ssc009a-s01a-qfn88-tiandy.dtb \
# 	infinity6b0-ssc009b-s01a.dtb \
# 	infinity6b0-ssc009d-s01a.dtb

DEVICE_TREE ?= $(CONFIG_DEFAULT_DEVICE_TREE:"%"=%)
dtb-y += $(DEVICE_TREE).dtb

targets += $(dtb-y)

DTC_FLAGS += -R 4 -p 0x1000

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
