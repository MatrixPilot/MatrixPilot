# this file is included from makefile

extra_dirs := MDD-File-System USB USB/CDC-Device-Driver USB/MSD-Device-Driver
#extra_dirs := MDD-File-System
#ifeq ($(DEVICE),AUAV3) 
#extra_dirs +=  USB USB/CDC-Device-Driver USB/MSD-Device-Driver
#endif

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.s)


$(eval $(call make-library, $(subdirectory)/$(subdirectory).a, $(local_src)))
