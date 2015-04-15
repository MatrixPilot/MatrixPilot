# this file is included from makefile

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)

ifneq (,$(filter $(TOOLCHAIN), C30 XC16))
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)
else
endif

#$(eval $(call make-library, $(subdirectory)/$(subdirectory).a, $(local_src)))
#$(eval $(call make-library,$(subdirectory).a,$(local_src)))
$(eval $(call make-library,$(subdirectory)/$(subdirectory).a,$(local_src)))

#incpath += $(subdirectory)
#incpath += $(SOURCE_DIR)/$(subdirectory)
