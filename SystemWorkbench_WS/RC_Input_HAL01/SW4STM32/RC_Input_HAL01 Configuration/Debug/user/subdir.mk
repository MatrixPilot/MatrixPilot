################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/SW_MX_WS/RC_Input_HAL01/SW4STM32/syscalls.c 

OBJS += \
./user/syscalls.o 

C_DEPS += \
./user/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
user/syscalls.o: D:/SW_MX_WS/RC_Input_HAL01/SW4STM32/syscalls.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -DUSE_HAL_DRIVER -DSTM32F411xE -I../../../Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Include -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"user/syscalls.d" -MT"user/syscalls.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


