# this file is included from makefile

#local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
#local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.cpp)
local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/JSBSim.cpp)
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/JSBSimSIL.cpp)
local_src += $(wildcard $(SOURCE_DIR)/$(subdirectory)/JSBSimLog.cpp)

$(eval $(call make-target,$(subdirectory)/$(subdirectory).a,$(local_src)))
