# this file is included from makefile

# NOTE: this test is no longer required - since we now include module.mk files explicitly
ifeq ($(DEVICE),PX4)

extra_dirs := Src Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates Drivers/STM32F4xx_HAL_Driver/Src Middlewares/Third_Party/FreeRTOS/Source Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang Middlewares/Third_Party/FatFs/src Middlewares/Third_Party/FatFs/src/drivers Middlewares/Third_Party/FatFs/src/option

$(call mkoutdir, $(addprefix $(subdirectory)/,$(extra_dirs)))

local_src := $(wildcard $(SOURCE_DIR)/$(subdirectory)/*.c)
local_src += $(foreach i,$(extra_dirs),$(wildcard $(SOURCE_DIR)/$(subdirectory)/$(i)/*.c))

#local_inc := Config FreeRTOS/include libUDB libSTM libSTM/Inc libSTM/Src
#local_inc += libSTM/Drivers libSTM/Drivers/CMSIS/Device/ST/STM32F4xx/Include libSTM/Drivers/CMSIS/Include libSTM/Drivers/STM32F4xx_HAL_Driver/Inc
#local_inc += libSTM/Middlewares libSTM/Middlewares/Third_Party/FreeRTOS/Source/include libSTM/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS libSTM/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
local_inc := Inc Src Drivers Drivers/CMSIS/Device/ST/STM32F4xx/Include Drivers/CMSIS/Include Drivers/STM32F4xx_HAL_Driver/Inc Middlewares Middlewares/Third_Party/FreeRTOS/Source/include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
local_inc += Middlewares/Third_Party/FatFs/src
local_inc += Middlewares/Third_Party/FatFs/src/drivers

incpath += $(addprefix $(SOURCE_DIR)/$(subdirectory)/,$(local_inc))
#$(warning incpath = $(incpath))

$(eval $(call make-library,$(subdirectory)/$(subdirectory).a,$(local_src)))
#$(eval $(call make-target,$(subdirectory)/$(subdirectory).a,$(local_src)))

endif
