#extra_dirs := portable/MemMang MPLAB/PIC24_dsPIC
extra_dirs := portable/MemMang portable/MSVC-MingW

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

$(eval $(call make-library, $(subdirectory)/FreeRTOS.a, $(local_src)))
