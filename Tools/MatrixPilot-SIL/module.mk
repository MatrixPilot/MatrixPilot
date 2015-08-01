# this file is included from makefile

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)

$(eval $(call make-target,$(subdirectory)/$(subdirectory).a,$(local_src)))
