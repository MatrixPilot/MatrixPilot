# this file is included from makefile

TOOLCHAIN ?= ARM
TARGET_TYPE := px4
ARCH := armv7e-m
CPU := cortex-m4
FPU := fpv4-sp-d16
TARGET_ARCH := -mcpu=$(CPU) -march=$(ARCH) -mfpu=$(FPU) -mfloat-abi=hard -mthumb -mlong-calls
#TARGET_ARCH := -mcpu=$(CPU) -march=$(ARCH) -mfpu=$(FPU) -mfloat-abi=hard -mthumb-interwork -mlong-calls

modules += libSTM

#incpath := Config FreeRTOS\include libUDB libSTM libSTM\inc libSTM\src libSTM\cmsis libSTM\Drivers libSTM\Drivers\CMSIS\Device\ST\STM32F4xx\Include libSTM\Drivers\CMSIS\Include libSTM\Drivers\STM32F4xx_HAL_Driver\Inc libSTM\Middlewares libSTM\Middlewares\Third_Party\FreeRTOS\Source\include libSTM\Middlewares\Third_Party\FreeRTOS\Source\CMSIS_RTOS libSTM\Middlewares\Third_Party\FreeRTOS\Source\portable\GCC\ARM_CM4F
#incpath := Config FreeRTOS/include libUDB libSTM
incpath += libSTM
#cfgpath := Config

defines += ARM_MATH_CM4 __FPU_USED STM32F401RE STM32F4XX USE_STDPERIPH_DRIVER STM32F401xE
