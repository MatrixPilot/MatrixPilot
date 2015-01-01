ifeq ($(DEVICE),SILSIM) 
ifeq ($(OS),Windows_NT)
extra_dirs := portable/MSVC-MingW
else
extra_dirs := portable/POSIX
endif
else
extra_dirs := MPLAB/PIC24_dsPIC
endif
extra_dirs += portable/MemMang

#$(warning OS = $(OS))
#$(warning DEVICE = $(DEVICE))
#$(warning FreeRTOS extra_dirs = $(extra_dirs))

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

$(eval $(call make-library, $(subdirectory)/FreeRTOS.a, $(local_src)))
