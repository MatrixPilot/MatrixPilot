extra_dirs := MDD-File-System USB USB/CDC-Device-Driver USB/MSD-Device-Driver

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

$(eval $(call make-library, $(subdirectory)/Microchip.a, $(local_src)))
