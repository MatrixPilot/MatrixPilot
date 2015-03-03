# this file is included from makefile

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)

ifneq (,$(filter $(TOOLCHAIN), C30 XC16))
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)
else
endif

local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.sm)

$(eval $(call make-target,$(subdirectory)/$(subdirectory).a,$(local_src)))
