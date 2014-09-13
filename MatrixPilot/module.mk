
local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)

$(eval $(call make-library, $(subdirectory)/MatrixPilot.a, $(local_src)))
