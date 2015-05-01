################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/bsp_driver_sd.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/fatfs.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/gpio.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/main.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/sdio.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/stm32f4xx_hal_msp.c \
D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/stm32f4xx_it.c 

OBJS += \
./Application/User/bsp_driver_sd.o \
./Application/User/fatfs.o \
./Application/User/gpio.o \
./Application/User/main.o \
./Application/User/sdio.o \
./Application/User/stm32f4xx_hal_msp.o \
./Application/User/stm32f4xx_it.o 

C_DEPS += \
./Application/User/bsp_driver_sd.d \
./Application/User/fatfs.d \
./Application/User/gpio.d \
./Application/User/main.d \
./Application/User/sdio.d \
./Application/User/stm32f4xx_hal_msp.d \
./Application/User/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/bsp_driver_sd.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/bsp_driver_sd.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/bsp_driver_sd.d" -MT"Application/User/bsp_driver_sd.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/fatfs.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/fatfs.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/fatfs.d" -MT"Application/User/fatfs.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/gpio.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/gpio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/gpio.d" -MT"Application/User/gpio.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/main.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/main.d" -MT"Application/User/main.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/sdio.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/sdio.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/sdio.d" -MT"Application/User/sdio.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f4xx_hal_msp.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/stm32f4xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/stm32f4xx_hal_msp.d" -MT"Application/User/stm32f4xx_hal_msp.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Application/User/stm32f4xx_it.o: D:/UDB_Nucleo/SystemWorkbench_WS/HAL_SD_Card/Src/stm32f4xx_it.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FatFs/src -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"Application/User/stm32f4xx_it.d" -MT"Application/User/stm32f4xx_it.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


