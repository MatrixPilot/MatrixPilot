# this file is included from makefile

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.cpp)

$(eval $(call make-target,$(subdirectory)/$(subdirectory).a,$(local_src)))
