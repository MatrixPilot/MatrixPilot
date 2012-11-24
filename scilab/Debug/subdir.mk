################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MAVLink_scilab.c 

OBJS += \
./MAVLink_scilab.o 

C_DEPS += \
./MAVLink_scilab.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/matt/Documents/MatrixPilot_fbw/scilab/MAVLink/include -I/home/matt/Documents/MatrixPilot_fbw/scilab/MAVLink/include/common -I/home/matt/Documents/MatrixPilot_fbw/scilab/MAVLink/include/matrixpilot -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


