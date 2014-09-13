
local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)

ifeq ($(TOOLCHAIN),C30) 
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)
else
endif

$(eval $(call make-library, $(subdirectory)/libVectorMatrix.a, $(local_src)))
