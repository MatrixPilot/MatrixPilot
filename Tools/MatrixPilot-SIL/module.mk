
local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)

$(eval $(call make-library, $(subdirectory)/MatrixPilot-SIL.a, $(local_src)))
