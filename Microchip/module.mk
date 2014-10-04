extra_dirs := MDD-File-System USB USB/CDC-Device-Driver USB/MSD-Device-Driver

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

#extra_mods := $(addprefix $(subdirectory)/,$(extra_dirs))
#$(call mkoutdir, $(extra_mods))

#extra_modules += $(extra_mods)
#$(warning Microchip extra_modules = $(extra_modules))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)

#locals := $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))
#$(warning Microchip locals = $(locals))

#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/MDD-File-System/*.c)
#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/USB/*.c)
#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/USB/CDC-Device-Driver/*.c)
#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/USB/MSD-Device-Driver/*.c)
#$(warning Microchip local_src = $(local_src))

$(eval $(call make-library, $(subdirectory)/Microchip.a, $(local_src)))
