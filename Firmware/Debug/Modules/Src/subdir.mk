################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modules/Src/fe_uart.c \
../Modules/Src/feetech_scs.c 

C_DEPS += \
./Modules/Src/fe_uart.d \
./Modules/Src/feetech_scs.d 

OBJS += \
./Modules/Src/fe_uart.o \
./Modules/Src/feetech_scs.o 


# Each subdirectory must supply rules for building sources it contributes
Modules/Src/%.o Modules/Src/%.su Modules/Src/%.cyclo: ../Modules/Src/%.c Modules/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Device/ST/STM32F1xx/Include -IC:/Users/Lenovo/STM32Cube/Repository/STM32Cube_FW_F1_V1.8.7/Drivers/CMSIS/Include -I../App/Inc -I../Modules/Inc -I../Platform/nc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Modules-2f-Src

clean-Modules-2f-Src:
	-$(RM) ./Modules/Src/fe_uart.cyclo ./Modules/Src/fe_uart.d ./Modules/Src/fe_uart.o ./Modules/Src/fe_uart.su ./Modules/Src/feetech_scs.cyclo ./Modules/Src/feetech_scs.d ./Modules/Src/feetech_scs.o ./Modules/Src/feetech_scs.su

.PHONY: clean-Modules-2f-Src

